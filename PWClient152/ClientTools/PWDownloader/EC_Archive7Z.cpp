/*
 * FILE: EC_Archive7Z.cpp
 *
 * DESCRIPTION: This class is used to extract files from *.7z
 *
 * CREATED BY: Shizhenhua, 2013/7/16
 *
 * HISTORY: 
 *
 * Copyright (c) 2011 ACE Studio, All Rights Reserved.
 */


#include "EC_Archive7Z.h"

#include "Common/MyString.h"
#include "Common/StringConvert.h"
#include "Common/MyInitGuid.h"
#include "7zip/IStream.h"
#include "7zip/Archive/IArchive.h"
#include "7zip/IPassword.h"
#include "7zip/PropID.h"
#include "7zip/Common/FileStreams.h"
#include "Windows/NtCheck.h"
#include "Windows/PropVariant.h"
#include "Windows/FileDir.h"
#include "Windows/FileFind.h"
#include "Windows/DLL.h"
#include "Windows/Error.h"

#include "LogFile.h"
#include <algorithm>


///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

#define SEVENZIP_DLL_PATH "7z.dll"
#define SAFE_RELEASE(p) if(p) { (p)->Release(); (p) = NULL; }

// use another CLSIDs, if you want to support other formats (zip, rar, ...).
// {23170F69-40C1-278A-1000-000110070000}
DEFINE_GUID(CLSID_CFormat7z, 0x23170F69, 0x40C1, 0x278A, 0x10, 0x00, 0x00, 0x01, 0x10, 0x07, 0x00, 0x00);
DEFINE_GUID(CLSID_CFormatRar, 0x23170F69, 0x40C1, 0x278A, 0x10, 0x00, 0x00, 0x01, 0x10, 0x03, 0x00, 0x00);
DEFINE_GUID(CLSID_CFormatZip, 0x23170F69, 0x40C1, 0x278A, 0x10, 0x00, 0x00, 0x01, 0x10, 0x01, 0x00, 0x00);



///////////////////////////////////////////////////////////////////////////
//	
//	Local Types and Variables and Global variables
//	
///////////////////////////////////////////////////////////////////////////

typedef UINT32 (WINAPI* CreateObjectFunc)( const GUID *clsID,
										   const GUID *interfaceID,
										   void **outObject );



///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////

static HRESULT IsArchiveItemProp(IInArchive *archive, UInt32 index, PROPID propID, bool &result)
{
	NWindows::NCOM::CPropVariant prop;
	RINOK(archive->GetProperty(index, propID, &prop));
	if (prop.vt == VT_BOOL)
		result = VARIANT_BOOLToBool(prop.boolVal);
	else if (prop.vt == VT_EMPTY)
		result = false;
	else
		return E_FAIL;
	return S_OK;
}

static HRESULT IsArchiveItemFolder(IInArchive *archive, UInt32 index, bool &result)
{
	return IsArchiveItemProp(archive, index, kpidIsDir, result);
}

// 获取文件的扩展名
static std::string GetFileExt(const char* filename)
{
	std::string strFile = filename;
	size_t pos = strFile.find_last_of('.');
	if( pos != std::string::npos )
	{
		std::string strExt = strFile.substr(pos + 1, strFile.length() - pos);
		std::transform(strExt.begin(), strExt.end(), strExt.begin(), tolower);	// 转为小写
		return strExt;
	}
	else
		return "";
}


///////////////////////////////////////////////////////////////////////////
//  
//  打开文档的回调处理
//  
///////////////////////////////////////////////////////////////////////////

class COpenCallback : public IArchiveOpenCallback, public ICryptoGetTextPassword
{
public:
	COpenCallback() : ref_count_(1)
	{
	}

	STDMETHOD_(ULONG, AddRef)()
	{
		++ ref_count_;
		return ref_count_;
	}

	STDMETHOD_(ULONG, Release)()
	{
		-- ref_count_;
		if( 0 == ref_count_ )
		{
			delete this;
			return 0;
		}

		return ref_count_;
	}

	STDMETHOD(QueryInterface)(REFGUID iid, void** outObject)
	{
		if( IID_ICryptoGetTextPassword == iid )
		{
			*outObject = static_cast<ICryptoGetTextPassword*>(this);
			AddRef();
			return S_OK;
		}
		else
		{
			if( IID_IArchiveOpenCallback == iid )
			{
				*outObject = static_cast<IArchiveOpenCallback*>(this);
				AddRef();
				return S_OK;
			}
			else
			{
				return E_NOINTERFACE;
			}
		}
	}

	STDMETHOD(SetTotal)(const UInt64* files, const UInt64* bytes)
	{
		return S_OK;
	}

	STDMETHOD(SetCompleted)(const UInt64* files, const UInt64* bytes)
	{
		return S_OK;
	}

	STDMETHOD(CryptoGetTextPassword)(BSTR* password)
	{
		if( !m_bPWIsDefined )
			return E_ABORT;
		else
		{
			*password = ::SysAllocString(m_sPassword);
			return (*password != 0) ? S_OK : E_OUTOFMEMORY;
		}
	}

	// 初始化
	void Init(const UString& pw)
	{
		m_bPWIsDefined = !pw.IsEmpty();
		m_sPassword = pw;
	}

protected:
	int ref_count_;
	bool m_bPWIsDefined;
	UString m_sPassword;
};

///////////////////////////////////////////////////////////////////////////
//  
//  解压文档的回调处理
//  
///////////////////////////////////////////////////////////////////////////

class CExtractCallback : public IArchiveExtractCallback, public ICryptoGetTextPassword
{
public:
	CExtractCallback() : ref_count_(1), m_bPWIsDefined(false)
	{
	}

	STDMETHOD_(ULONG, AddRef)()
	{
		++ ref_count_;
		return ref_count_;
	}

	STDMETHOD_(ULONG, Release)()
	{
		-- ref_count_;
		if( 0 == ref_count_ )
		{
			delete this;
			return 0;
		}

		return ref_count_;
	}

	STDMETHOD(QueryInterface)(REFGUID iid, void** outObject)
	{
		if( IID_ICryptoGetTextPassword == iid )
		{
			*outObject = static_cast<ICryptoGetTextPassword*>(this);
			AddRef();
			return S_OK;
		}
		else
		{
			if( IID_IArchiveExtractCallback == iid )
			{
				*outObject = static_cast<IArchiveExtractCallback*>(this);
				AddRef();
				return S_OK;
			}
			else
			{
				return E_NOINTERFACE;
			}
		}
	}

	STDMETHOD(SetTotal)(UInt64 size)
	{
		if( m_pCBExtract )
			m_pCBExtract(IArchive7Z::OP_GETTOTAL, IArchive7Z::PROPERTY(size));

		return S_OK;
	}

	STDMETHOD(SetCompleted)(const UInt64* completeValue)
	{
		if( m_pCBExtract )
			m_pCBExtract(IArchive7Z::OP_GETCOMPLETED, IArchive7Z::PROPERTY(*completeValue));

		return S_OK;
	}

	STDMETHOD(GetStream)(UInt32 index, ISequentialOutStream** outStream, Int32 askExtractMode)
	{
		using namespace NWindows;

		*outStream = NULL;
		m_pOutStream.Release();

		{
			// Get Name
			NCOM::CPropVariant prop;
			RINOK(m_pArchive->GetProperty(index, kpidPath, &prop));

			UString fullPath;
			if (prop.vt == VT_EMPTY)
				fullPath = L"[Content]";
			else
			{
				if (prop.vt != VT_BSTR)
					return E_FAIL;

				fullPath = prop.bstrVal;
			}

			m_sPathInArchive = fullPath;
		}

		if (askExtractMode != NArchive::NExtract::NAskMode::kExtract)
			return S_OK;

		{
			// Get Attrib
			NCOM::CPropVariant prop;
			RINOK(m_pArchive->GetProperty(index, kpidAttrib, &prop));
			if (prop.vt == VT_EMPTY)
			{
				m_ProcessedFileInfo.Attrib = 0;
				m_ProcessedFileInfo.AttribDefined = false;
			}
			else
			{
				if (prop.vt != VT_UI4)
					return E_FAIL;

				m_ProcessedFileInfo.Attrib = prop.ulVal;
				m_ProcessedFileInfo.AttribDefined = true;
			}
		}

		RINOK(IsArchiveItemFolder(m_pArchive, index, m_ProcessedFileInfo.isDir));

		{
			// Get Modified Time
			NCOM::CPropVariant prop;
			RINOK(m_pArchive->GetProperty(index, kpidMTime, &prop));
			m_ProcessedFileInfo.MTimeDefined = false;
			switch(prop.vt)
			{
			case VT_EMPTY:
				// m_ProcessedFileInfo.MTime = _utcMTimeDefault;
				break;
			case VT_FILETIME:
				m_ProcessedFileInfo.MTime = prop.filetime;
				m_ProcessedFileInfo.MTimeDefined = true;
				break;
			default:
				return E_FAIL;
			}
		}

		{
			// Create folders for file
			int slashPos = m_sPathInArchive.ReverseFind(WCHAR_PATH_SEPARATOR);
			if (slashPos >= 0)
				NFile::NDirectory::CreateComplexDirectory(m_sOutDir + m_sPathInArchive.Left(slashPos));
		}

		UString fullProcessedPath = m_sOutDir + m_sPathInArchive;
		m_sPathInDisk = fullProcessedPath;

		if (m_ProcessedFileInfo.isDir)
		{
			NFile::NDirectory::CreateComplexDirectory(fullProcessedPath);
		}
		else
		{
			NFile::NFind::CFileInfoW fi;
			if (fi.Find(fullProcessedPath))
			{
				if (!NFile::NDirectory::DeleteFileAlways(fullProcessedPath))
				{
					LogOutput("CExtractCallback::GetStream, Can't delete output file (%s)!", GetAnsiString(fullProcessedPath));
					return E_ABORT;
				}
			}

			m_pOutStreamSpec = new COutFileStream;
			CMyComPtr<ISequentialOutStream> outStreamLoc(m_pOutStreamSpec);
			if (!m_pOutStreamSpec->Open(fullProcessedPath, CREATE_ALWAYS))
			{
				LogOutput("CExtractCallback::GetStream, Can't open output file (%s)!", GetAnsiString(fullProcessedPath));
				return E_ABORT;
			}

			m_pOutStream = outStreamLoc;
			*outStream = outStreamLoc.Detach();
		}

		return S_OK;
	}

	STDMETHOD(PrepareOperation)(Int32 askExtractMode)
	{
		m_bExtractMode = false;

		switch (askExtractMode)
		{
		case NArchive::NExtract::NAskMode::kExtract:
			m_bExtractMode = true;
			break;
		};

		if( m_pCBExtract )
			m_pCBExtract(IArchive7Z::OP_GETCURFILE, IArchive7Z::PROPERTY(GetAnsiString(m_sPathInArchive)));

		return S_OK;
	}

	STDMETHOD(SetOperationResult)(Int32 resultEOperationResult)
	{
		switch(resultEOperationResult)
		{
		case NArchive::NExtract::NOperationResult::kOK:
			break;
		default:
			m_nNumErrors++;

			switch(resultEOperationResult)
			{
			case NArchive::NExtract::NOperationResult::kUnSupportedMethod:
				LogOutput("CExtractCallback::SetOperationResult, extract file (%s) failed! Unsupported Method!", GetAnsiString(m_sPathInArchive));
				break;
			case NArchive::NExtract::NOperationResult::kCRCError:
				LogOutput("CExtractCallback::SetOperationResult, extract file (%s) failed! CRC Failed!", GetAnsiString(m_sPathInArchive));
				break;
			case NArchive::NExtract::NOperationResult::kDataError:
				LogOutput("CExtractCallback::SetOperationResult, extract file (%s) failed! Data Error!", GetAnsiString(m_sPathInArchive));
				break;
			default:
				LogOutput("CExtractCallback::SetOperationResult, extract file (%s) failed! Unknown Error!", GetAnsiString(m_sPathInArchive));
				break;
			}
		}

		if (m_pOutStream != NULL)
		{
			if (m_ProcessedFileInfo.MTimeDefined)
				m_pOutStreamSpec->SetMTime(&m_ProcessedFileInfo.MTime);

			RINOK(m_pOutStreamSpec->Close());
		}

		m_pOutStream.Release();
		if (m_bExtractMode && m_ProcessedFileInfo.AttribDefined)
			NWindows::NFile::NDirectory::MySetFileAttributes(m_sPathInDisk, m_ProcessedFileInfo.Attrib);

		return S_OK;
	}

	STDMETHOD(CryptoGetTextPassword)(BSTR* password)
	{
		if( !m_bPWIsDefined )
			return E_ABORT;
		else
		{
			*password = ::SysAllocString(m_sPassword);
			return S_OK;
		}
	}

	// 初始化
	void Init(IInArchive* pArchive, const UString& outDir, const UString& pw, IArchive7Z::ExtractCallback pCB = NULL)
	{
		m_pArchive = pArchive;
		m_sOutDir = outDir;
		NWindows::NFile::NName::NormalizeDirPathPrefix(m_sOutDir);
		m_bPWIsDefined = !pw.IsEmpty();
		m_sPassword = pw;
		m_nNumErrors = 0;
		m_pCBExtract = pCB;
	}

protected:
	int ref_count_;

	CMyComPtr<IInArchive> m_pArchive;
	UString m_sOutDir;
	UString m_sPathInArchive;
	UString m_sPathInDisk;
	bool m_bExtractMode;
	IArchive7Z::ExtractCallback m_pCBExtract;

	CMyComPtr<ISequentialOutStream> m_pOutStream;
	COutFileStream* m_pOutStreamSpec;

	UInt32 m_nNumErrors;
	bool m_bPWIsDefined;
	UString m_sPassword;

	struct SProcessedFileInfo
	{
		FILETIME MTime;
		UInt32 Attrib;
		bool isDir;
		bool AttribDefined;
		bool MTimeDefined;
	} m_ProcessedFileInfo;
};


///////////////////////////////////////////////////////////////////////////
//	
//	Implement the interface IArchive7Z
//	
///////////////////////////////////////////////////////////////////////////

class CECArchive7Z : public IArchive7Z
{
public:
	CECArchive7Z();

	// 销毁对象
	void Release();

	// 设置解压过程的回调函数
	void SetExtractCB(ExtractCallback pfnCallback);

	// 加载7z压缩包
	bool LoadPack(const char* filename, const char* password);

	// 解压到指定目录
	bool ExtractTo(const char* szDestDir);

	// 卸载压缩包
	void Unload();

protected:
	NWindows::NDLL::CLibrary m_DLL;
	CreateObjectFunc m_pCreateFunc;
	ExtractCallback m_pCBExtract;
	IInArchive* m_pArchive;
	CInFileStream* m_pStreamSpec;
	COpenCallback* m_pOpenCallback;
	CExtractCallback* m_pExtractCallback;
	UString m_sPassword;
};

// 构造函数
CECArchive7Z::CECArchive7Z() : m_pArchive(NULL), m_pStreamSpec(NULL), m_pCreateFunc(NULL),
	m_pOpenCallback(NULL), m_pExtractCallback(NULL)
{
	if( !m_DLL.Load(SEVENZIP_DLL_PATH) )
	{
		assert(0);
		LogOutput("CECArchive7Z::CECArchive7Z, Can't load the 7z.dll!");
	}
	
	m_pCreateFunc = (CreateObjectFunc)m_DLL.GetProc("CreateObject");
	if( !m_pCreateFunc )
	{
		assert(0);
		LogOutput("CECArchive7Z::CECArchive7Z, Can't find the CreateObject function!");
	}
}

// 销毁对象
void CECArchive7Z::Release()
{
	Unload();
	delete this;
}

// 设置解压过程的回调函数
void CECArchive7Z::SetExtractCB(ExtractCallback pfnCallback)
{
	m_pCBExtract = pfnCallback;
}

// 加载7Z压缩包
bool CECArchive7Z::LoadPack(const char* filename, const char* password)
{
	if( !filename || !strcmp(filename, "") || !m_pCreateFunc )
		return false;

	if( m_pArchive )
		Unload();

	const GUID* pGUID = NULL;
	std::string strExt = GetFileExt(filename);
	if( strExt == "7z" )
		pGUID = &CLSID_CFormat7z;
	else if( strExt == "rar" )
		pGUID = &CLSID_CFormatRar;
	else if( strExt == "zip" )
		pGUID = &CLSID_CFormatZip;
	else
	{
		LogOutput("CECArchive7Z::LoadPack, Unknown archive format, open failed!");
		return false;
	}

	// Create IInArchive
	if( FAILED(m_pCreateFunc(pGUID, &IID_IInArchive, (void**)&m_pArchive)) )
	{
		LogOutput("CECArchive7Z::LoadPack, Create the IInArchive failed!");
		return false;
	}

	// Create CInFileStream
	m_pStreamSpec = new CInFileStream;	// 创建出来的引用计数居然为0，搞不懂
	m_pStreamSpec->AddRef();			// 这里手动置为1
	if( !m_pStreamSpec->Open(filename) )
	{
		LogOutput("CECArchive7Z::LoadPack, Open the archive file(%s) failed!", filename);
		return false;
	}

	// Create the open callback
	m_pOpenCallback = new COpenCallback;
	m_pOpenCallback->Init(password ? GetUnicodeString(password) : L"");

	// Open the archive
	if( FAILED(m_pArchive->Open(m_pStreamSpec, 0, m_pOpenCallback)) )
	{
		LogOutput("CECArchive7Z::LoadPack, load the 7z archive (%s) failed!", filename);
		return false;
	}

	return true;
}

// 解压所有文件到指定目录
bool CECArchive7Z::ExtractTo(const char* szDestDir)
{
	if( !szDestDir || !m_pArchive )
		return false;

	// Create the extract callback
	SAFE_RELEASE(m_pExtractCallback);
	m_pExtractCallback = new CExtractCallback;
	m_pExtractCallback->Init(m_pArchive, GetUnicodeString(szDestDir), m_sPassword, m_pCBExtract);

	// Start to extract...
	HRESULT hr = m_pArchive->Extract(NULL, (UInt32)(Int32)(-1), false, m_pExtractCallback);
	if( FAILED(hr) )
	{
		CSysString msg;
		NWindows::NError::MyFormatMessage(hr, msg);
		MessageBoxA(NULL, msg, "错误", MB_OK|MB_ICONERROR);
		LogOutput("CECArchive7Z::ExtractTo, Extract files from archive failed! %s", msg);
		return false;
	}

	return true;
}

// 卸载压缩包
void CECArchive7Z::Unload()
{
	m_sPassword = L"";
	SAFE_RELEASE(m_pExtractCallback);
	SAFE_RELEASE(m_pOpenCallback);
	SAFE_RELEASE(m_pStreamSpec);
	SAFE_RELEASE(m_pArchive);
}

///////////////////////////////////////////////////////////////////////////

// 创建指定的接口
IArchive7Z* CreateArchive7Z()
{
	return new CECArchive7Z();
}