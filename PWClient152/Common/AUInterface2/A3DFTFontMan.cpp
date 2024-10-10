#include "AUI.h"

#include "A3DFTFont.h"
#include "A3DFTFontMan.h"
#include "FTInterface.h"
#include "A3DMacros.h"
#include <io.h>
#include "AWString.h"
#include "A3DDevice.h"
#include "shlwapi.h"
#include "A3DEngine.h"
#include "A3DShaderMan.h"
#include "AFileImage.h"
#include "AUICommon.h"
#include <A3DVertexShader.h>
#include <A3DViewport.h>
#include <AFI.h>

wchar_t _widest_char = 0;
static const int _font_buf_size = 1024;
static const char* _format_widest_char		= "WidestChar: %d";


// Define language ID
#define LID_TRADITIONAL_CHINESE	0x0404
#define LID_JAPANESE			0x0411
#define LID_KOREAN				0x0412
#define LID_SIMPLIFIED_CHINESE	0x0804

DWORD _font_cur_language = LID_SIMPLIFIED_CHINESE;

typedef struct _tagTT_OFFSET_TABLE{
	USHORT	uMajorVersion;
	USHORT	uMinorVersion;
	USHORT	uNumOfTables;
	USHORT	uSearchRange;
	USHORT	uEntrySelector;
	USHORT	uRangeShift;
}TT_OFFSET_TABLE;

typedef struct _tagTT_TABLE_DIRECTORY{
	char	szTag[4];			// table name
	ULONG	uCheckSum;			// check sum
	ULONG	uOffset;			// Offset from beginning of file
	ULONG	uLength;			// length of the table in bytes
}TT_TABLE_DIRECTORY;

typedef struct _tagTT_NAME_TABLE_HEADER{
	USHORT	uFSelector;			// format selector. Always 0
	USHORT	uNRCount;			// mame records count
	USHORT	uStorageOffset;		// Offset for strings storage, from start of the table
}TT_NAME_TABLE_HEADER;

typedef struct _tagTT_NAME_RECORD{
	USHORT	uPlatformID;
	USHORT	uEncodingID;
	USHORT	uLanguageID;
	USHORT	uNameID;
	USHORT	uStringLength;
	USHORT	uStringOffset;	//from start of storage area
}TT_NAME_RECORD;

typedef struct _tagTT_TTC_HEADER{
	DWORD	dwMagic;
	WORD	_unknown[3];
	WORD	wTTFCount;
}TT_TTC_HEADER;

#define SWAPWORD(x)		MAKEWORD(HIBYTE(x), LOBYTE(x))
#define SWAPLONG(x)		MAKELONG(SWAPWORD(HIWORD(x)), SWAPWORD(LOWORD(x)))

void _GetFontTTFName(FILE* fp, long offset, wchar_t* wszFaceNames)
{
	TT_OFFSET_TABLE ttOffsetTable;

	fseek(fp, offset, SEEK_SET);
	fread(&ttOffsetTable, sizeof(TT_OFFSET_TABLE), 1, fp);
	ttOffsetTable.uNumOfTables = SWAPWORD(ttOffsetTable.uNumOfTables);
	ttOffsetTable.uMajorVersion = SWAPWORD(ttOffsetTable.uMajorVersion);
	ttOffsetTable.uMinorVersion = SWAPWORD(ttOffsetTable.uMinorVersion);

	//check is this is a true type font and the version is 1.0
	if(ttOffsetTable.uMajorVersion != 1 || ttOffsetTable.uMinorVersion != 0)
		return;

	TT_TABLE_DIRECTORY tblDir;
	bool bFound = false;
	char szTemp[5];

	for (int i = 0; i < ttOffsetTable.uNumOfTables; i++)
	{
		fread(&tblDir, sizeof(TT_TABLE_DIRECTORY), 1, fp);
		strncpy(szTemp, tblDir.szTag, 4);
		szTemp[4] = 0;

		if (stricmp(szTemp, "name") == 0)
		{
			bFound = true;
			tblDir.uLength = SWAPLONG(tblDir.uLength);
			tblDir.uOffset = SWAPLONG(tblDir.uOffset);
			break;
		}
		else if (strlen(szTemp) == 0)
			break;
	}

	if (bFound)
	{
		fseek(fp, tblDir.uOffset, SEEK_SET);
		TT_NAME_TABLE_HEADER ttNTHeader;
		fread(&ttNTHeader, sizeof(TT_NAME_TABLE_HEADER), 1, fp);
		ttNTHeader.uNRCount = SWAPWORD(ttNTHeader.uNRCount);
		ttNTHeader.uStorageOffset = SWAPWORD(ttNTHeader.uStorageOffset);
		TT_NAME_RECORD ttRecord;
		bFound = false;

		for (int i = 0; i < ttNTHeader.uNRCount; i++)
		{
			fread(&ttRecord, sizeof(TT_NAME_RECORD), 1, fp);
			ttRecord.uLanguageID = SWAPWORD(ttRecord.uLanguageID);
			ttRecord.uNameID = SWAPWORD(ttRecord.uNameID);

			if ((ttRecord.uLanguageID != _font_cur_language && ttRecord.uLanguageID != 0)
			 || ttRecord.uNameID != 4)
				continue;

			ttRecord.uStringLength = SWAPWORD(ttRecord.uStringLength);
			ttRecord.uStringOffset = SWAPWORD(ttRecord.uStringOffset);

			int nPos = ftell(fp);
			fseek(fp, tblDir.uOffset + ttRecord.uStringOffset + ttNTHeader.uStorageOffset, SEEK_SET);

			WORD* pBuf = (WORD*)wszFaceNames;

			if (ttRecord.uStringLength)
			{
				char* buf = new char[ttRecord.uStringLength];
				fread(buf, ttRecord.uStringLength, 1, fp);
				int off = 0;

				while (off < ttRecord.uStringLength)
				{
					if (ttRecord.uEncodingID)
					{
						WORD* p = (WORD*)(buf + off);
						*pBuf++ = SWAPWORD(*p);
						off += 2;
					}
					else
					{
						*pBuf++ = MAKEWORD(buf[off], 0);
						off++;
					}
				}

				*pBuf = L'\0';
				delete[] buf;
			}

			fseek(fp, nPos, SEEK_SET);
		}			
	}
}

bool _GetFontFaceNames(const char* szFilePath, wchar_t* wszFaceNames, size_t size)
{
	bool bTTC = strnicmp(PathFindExtensionA(szFilePath), ".ttc", 4) == 0;
	FILE* fp = fopen(szFilePath, "rb");
	if (!fp) return false;

	memset(wszFaceNames, 0, sizeof(wchar_t) * size);

	if (bTTC)
	{
		TT_TTC_HEADER ttc;
		wchar_t* p = wszFaceNames;

		fread(&ttc, sizeof(ttc), 1, fp);
		ttc.wTTFCount = SWAPWORD(ttc.wTTFCount);

		DWORD* pOffs = new DWORD[ttc.wTTFCount];
		fread(pOffs, sizeof(DWORD), ttc.wTTFCount, fp);

		for (WORD i = 0; i < ttc.wTTFCount; i++)
		{
			pOffs[i] = SWAPLONG(pOffs[i]);
			_GetFontTTFName(fp, pOffs[i], p);
			p += wcslen(p) + 1;
			*p = L'\0';
		}

		delete[] pOffs;
	}
	else
	{
		_GetFontTTFName(fp, 0, wszFaceNames);
		wszFaceNames[wcslen(wszFaceNames) + 1] = L'\0';
	}

	fclose(fp);
	return true;
}

struct _face_path_pair
{
	const wchar_t*	face_name;
	const char*		font_path;
};

static _face_path_pair _font_inner_map[] =
{
	{ L"方正隶二简体",		"font\\FZL2JW.ttf" },
	{ L"方正细黑一简体",	"font\\FZXH1JW.ttf" },
	{ L"方正隶变简体",		"font\\FZLBJW.ttf" }
};

const _face_path_pair* _get_inner_font_path_by_name(const wchar_t* wszFace)
{
	for (int i = 0; i < sizeof(_font_inner_map) / sizeof(_face_path_pair); i++)
		if (wcsicmp(wszFace, _font_inner_map[i].face_name) == 0)
			return &_font_inner_map[i];

	return NULL;
}

typedef abase::hash_map<AWString, const char*> FaceNameMap;
typedef abase::vector<char*> FontPathArray;

static FaceNameMap _face_name_map;
static int _face_name_ref = 0;
static const wchar_t* _default_font_face = L"方正细黑一简体";
static const int _default_font_size = 10;

static FontPathArray _font_path;

A3DFTFontMan::A3DFTFontMan() :
m_pDevice(0),
m_pFTFontMan(0),
m_pShadowShader(0),
m_pStereoShader(0),
m_pStereoShadowShader(0),
m_pFrontStream(0),
m_pShadowStream(0),
m_pDefaultFont(0),
m_pMultiThreadLoadFunc(0),
m_dwMainThreadId(0),
m_dwMaxNewCharPerFrame((DWORD)-1),
m_nCurPosInFrontStream(0),
m_nCurPosInShadowStream(0)
{
	::InitializeCriticalSection(&m_cs);
}

void A3DFTFontMan::InitFaceNameMap()
{
	if (_face_name_ref++ != 0) return;

	_font_path.reserve(512);

	char szPath[MAX_PATH];
	wchar_t wszFaceName[1024];

	::GetWindowsDirectoryA(szPath, MAX_PATH);
	char* off = szPath + strlen(szPath) + 7;
	strcat(szPath, "\\Fonts\\*.tt*");

	struct _finddata_t c_file;
    long hFile;
	hFile = _findfirst(szPath, &c_file);

	if (hFile == -1L) return;

	while (true)
	{
		if (!(c_file.attrib & _A_SUBDIR))
		{
			*off = '\0';
			strcpy(off, c_file.name);

			if (_GetFontFaceNames(szPath, wszFaceName, sizeof(wszFaceName) / sizeof(wchar_t))
			&& wcslen(wszFaceName))
			{
				char* szSave = new char[strlen(szPath) + 1];
				strcpy(szSave, szPath);
				_font_path.push_back(szSave);

				const wchar_t* p = wszFaceName;
				int nLen = wcslen(p);

				while (nLen)
				{
					AWString strFace = p;
					strFace.MakeLower();
					_face_name_map[strFace] = szSave;
					p += nLen + 1;
					nLen = wcslen(p);
				}
			}
		}

		if (_findnext(hFile, &c_file) != 0) break;
	}

	_findclose(hFile);
}

bool A3DFTFontMan::LoadConfig(const char* szConfigFile)
{
	AFileImage file;

	if (!file.Open(szConfigFile, AFILE_OPENEXIST))
		return false;

	char szLine[AFILE_LINEMAXLEN];
	DWORD dwReadLen;
	int nRead;

	if( !file.ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen) )
		return false;
	sscanf(szLine, _format_widest_char, &nRead);
	_widest_char = (wchar_t)nRead;

	file.Close();
	return true;
}

bool A3DFTFontMan::Create(A3DDevice* pDevice, const char* szConfigFile)
{
	Lock();

	if (!LoadConfig(szConfigFile))
	{
		Unlock();
		return false;
	}

	m_pFTFontMan = ::CreateFTManager(32, 128, 512 * 1024);

	if (!m_pFTFontMan)
	{
		Unlock();
		return false;
	}

	m_pDevice = pDevice;
	m_pShadowShader = pDevice->GetA3DEngine()->GetA3DShaderMan()->LoadPixelShader(SHADERS_ROOT_DIR"ps\\dropshadow_14.txt", false);

#if defined(_DX9) || defined(_ANGELICA22)

#else

	// now load the vertex shader
	DWORD	dwVSDecl[20];
	int		n = 0;
	dwVSDecl[n ++] = D3DVSD_STREAM(0);					// begin tokens.
	dwVSDecl[n ++] = D3DVSD_REG(0, D3DVSDT_FLOAT4);		// x, y, z and w in cuboid space
	dwVSDecl[n ++] = D3DVSD_REG(1, D3DVSDT_D3DCOLOR);	// diffuse color
	dwVSDecl[n ++] = D3DVSD_REG(2, D3DVSDT_D3DCOLOR);	// specular color
	dwVSDecl[n ++] = D3DVSD_REG(3, D3DVSDT_FLOAT2);		// u, v texture coord
	dwVSDecl[n ++] = D3DVSD_END();						// end tokens.
	
	static const char* _stereo = SHADERS_ROOT_DIR"vs\\stereo_vs.txt";

	if (af_IsFileExist(_stereo))
		m_pStereoShader = m_pDevice->GetA3DEngine()->GetA3DShaderMan()->LoadVertexShader(_stereo, false, dwVSDecl);
	else
		m_pStereoShader = NULL;

	n = 0;
	dwVSDecl[n ++] = D3DVSD_STREAM(0);					// begin tokens.
	dwVSDecl[n ++] = D3DVSD_REG(0, D3DVSDT_FLOAT4);		// x, y, z and w in cuboid space
	dwVSDecl[n ++] = D3DVSD_REG(1, D3DVSDT_D3DCOLOR);	// diffuse color
	dwVSDecl[n ++] = D3DVSD_REG(2, D3DVSDT_D3DCOLOR);	// specular color
	dwVSDecl[n ++] = D3DVSD_REG(3, D3DVSDT_FLOAT2);		// u, v texture coord
	dwVSDecl[n ++] = D3DVSD_REG(4, D3DVSDT_FLOAT2);		// u, v texture coord
	dwVSDecl[n ++] = D3DVSD_REG(5, D3DVSDT_FLOAT2);		// u, v texture coord
	dwVSDecl[n ++] = D3DVSD_REG(6, D3DVSDT_FLOAT2);		// u, v texture coord
	dwVSDecl[n ++] = D3DVSD_REG(7, D3DVSDT_FLOAT2);		// u, v texture coord
	dwVSDecl[n ++] = D3DVSD_END();						// end tokens.

	static const char* _stereo_shadow = SHADERS_ROOT_DIR"vs\\font_shadow_stereo_vs.txt";

	if (af_IsFileExist(_stereo_shadow))
		m_pStereoShadowShader = m_pDevice->GetA3DEngine()->GetA3DShaderMan()->LoadVertexShader(_stereo_shadow, false, dwVSDecl);
	else
		m_pStereoShadowShader = NULL;

#endif

	m_pShadowStream = CreateStream(FVF_TLVERTEX_FONT_SHADOW, sizeof(TLVERTEX_FONT_SHADOW));
	m_pFrontStream = CreateStream(A3DFVF_A3DTLVERTEX, sizeof(A3DTLVERTEX));

	// 圣斗士部分玩家调用InitFaceNameMap会出现闪退现象
	// 所以屏蔽这里
#ifdef _ANGELICA22
#else
	InitFaceNameMap();
#endif

	m_pDefaultFont = CreatePointFont(_default_font_size, _default_font_face, 0);

	if (!m_pDefaultFont)
	{
		Release();
		Unlock();
		return false;
	}

	Unlock();
	return true;
}

void A3DFTFontMan::Release()
{
	Lock();

	if (m_pShadowShader)
	{
		m_pDevice->GetA3DEngine()->GetA3DShaderMan()->ReleasePixelShader(&m_pShadowShader);
		m_pShadowShader = NULL;
	}

	if (m_pStereoShader)
	{
		m_pDevice->GetA3DEngine()->GetA3DShaderMan()->ReleaseVertexShader(&m_pStereoShader);
		m_pStereoShader = NULL;
	}

	if (m_pStereoShadowShader)
	{
		m_pDevice->GetA3DEngine()->GetA3DShaderMan()->ReleaseVertexShader(&m_pStereoShadowShader);
		m_pStereoShadowShader = NULL;
	}

	A3DRELEASE(m_pShadowStream);
	A3DRELEASE(m_pFrontStream);

	size_t i;

	for (i = 0; i < m_FontBuf.size(); i++)
		ReleaseFont(m_FontBuf[i]);
	m_FontBuf.clear();

	for (i = 0; i < m_RefCntFontBuf.size(); i++)
		ReleaseFont(m_RefCntFontBuf[i]);
	m_RefCntFontBuf.clear();

	if (m_pFTFontMan)
	{
		m_pFTFontMan->Release();
		m_pFTFontMan = NULL;
	}

	m_pDevice = NULL;
	m_pDefaultFont = NULL;

	if (--_face_name_ref == 0)
	{
		_face_name_map.clear();

		for (i = 0; i < _font_path.size(); i++)
			delete[] _font_path[i];

		_font_path.clear();
	}

	for (i = 0; i < m_DataBufferPtrVec.size(); i++)
		delete[] m_DataBufferPtrVec[i];

	m_DataBufferPtrVec.clear();
	m_DataBufferMap.clear();

	Unlock();
}

void A3DFTFontMan::CreateFontDataBuffer(const wchar_t* wszFace, const char* szPath)
{
	if (m_DataBufferMap.find(wszFace) != m_DataBufferMap.end())
		return;

	A3DFTFontDataBufferMap::iterator it = m_DataBufferMap.begin();

	for (; it != m_DataBufferMap.end(); ++it)
	{
		A3DFTFONT_DATA_BUFFER& data = it->second;

		if (stricmp(data.path, szPath) == 0)
		{
			m_DataBufferMap[wszFace] = data;
			return;
		}
	}

	FILE* fp = fopen(szPath, "rb");

	if (!fp)
		return;

	A3DFTFONT_DATA_BUFFER buf;
	fseek(fp, 0, SEEK_END);
	buf.length = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	buf.buffer = new char[buf.length];
	fread(buf.buffer, buf.length, 1, fp);
	fclose(fp);
	strcpy(buf.path, szPath);
	m_DataBufferMap[wszFace] = buf;
	m_DataBufferPtrVec.push_back(buf.buffer);
}

A3DFTFont* A3DFTFontMan::CreatePointFont(
	float fPointSize,
	const wchar_t* wszFaceName,
	int nStyle,
	int nCreateMode,
	int nOutlineWidth,
	A3DCOLOR clOutline,
	A3DCOLOR clInnerText,
	bool bInterpColor,
	A3DCOLOR clUpper,
	A3DCOLOR clLower,
	bool bStereo,
	bool bNoCache,
	bool bLoadInThread)
{
	Lock();

	int nPointSize = int(fPointSize) * 10;

	if (nPointSize <= 0)
	{
		A3DFTFont* p = m_pDefaultFont;
		p->IncRefCnt();
		Unlock();
		return p;
	}

	if (wcslen(wszFaceName) == 0)
	{
		wszFaceName = _default_font_face;
		nPointSize = _default_font_size * 10;
		nStyle = 0;
		nOutlineWidth = 0;
		clOutline = 0;
	}

	AWString strFace = wszFaceName;
	strFace.MakeLower();

	const char* szFontPath;
	const _face_path_pair* font = _get_inner_font_path_by_name(strFace);

	if (!font)
	{
		FaceNameMap::iterator it = _face_name_map.find(strFace);

		if (it == _face_name_map.end())
		{
			strFace = _default_font_face;
			strFace.MakeLower();
			font = _get_inner_font_path_by_name(strFace);

			if (!font)
			{
				A3DFTFont* p = m_pDefaultFont;
				p->IncRefCnt();
				Unlock();
				return p;
			}
			else
				szFontPath = font->font_path;
		}
		else
			szFontPath = it->second;
	}
	else
		szFontPath = font->font_path;

	for (size_t i = 0; i < m_FontBuf.size(); i++)
	{
		A3DFTFont& ft = *m_FontBuf[i];

		if (nPointSize == ft.m_iPointSize
		 && nStyle == ft.m_iStyle
		 && nOutlineWidth == ft.m_iOutline
		 && clOutline == ft.m_clOutlineReserve
		 && clInnerText == ft.m_clInnerReserve
		 && bInterpColor == ft.m_bInterpColor
		 && clUpper == ft.m_clUpperReserve
		 && clLower == ft.m_clLowerReserve
		 && nCreateMode == ft.m_nCreateMode
		 && stricmp(szFontPath, ft.m_szFontPath) == 0
		 && bStereo == ft.m_bStereo)
		{
			A3DFTFont* p = m_FontBuf[i];
			p->IncRefCnt();
			Unlock();
			return p;
		}
	}

#ifdef _DX9

	A3DFTFont* pFTFont = new A3DFTFont(this);

	if (bLoadInThread && ::GetCurrentThreadId() == m_dwMainThreadId)
	{
		pFTFont->SetParam(nPointSize, szFontPath, nStyle, nOutlineWidth, clOutline, clInnerText, bInterpColor, clUpper, clLower, nCreateMode, wszFaceName, bStereo);
		m_pMultiThreadLoadFunc(pFTFont);
		m_FontBuf.push_back(pFTFont);
		Unlock();
		return pFTFont;
	}

	Unlock();

	if (!pFTFont->Create(nPointSize, szFontPath, nStyle, nOutlineWidth, clOutline, clInnerText, bInterpColor, clUpper, clLower, nCreateMode, wszFaceName, bStereo, m_pDevice->GetA3DEngine()))
	{
		delete pFTFont;
		A3DFTFont* p = m_pDefaultFont;
		p->IncRefCnt();
		return p;
	}

	Lock();
	m_FontBuf.push_back(pFTFont);
	Unlock();

	return pFTFont;

#else

	A3DFTFont* pFTFont = new A3DFTFont(this);

	if (!pFTFont->Create(nPointSize, szFontPath, nStyle, nOutlineWidth, clOutline, clInnerText, bInterpColor, clUpper, clLower, nCreateMode, wszFaceName, bStereo,m_pDevice->GetA3DEngine()))
	{
		delete pFTFont;
		A3DFTFont* p = m_pDefaultFont;
		p->IncRefCnt();
		Unlock();
		return p;
	}

	m_FontBuf.push_back(pFTFont);
	Unlock();

	return pFTFont;

#endif
}

void A3DFTFontMan::CreateFontInThread(A3DFTFont* pFont)
{
	pFont->Create(
		pFont->m_iPointSize,
		pFont->m_szFontPath,
		pFont->m_iStyle,
		pFont->m_iOutline,
		pFont->m_clOutlineReserve,
		pFont->m_clInnerReserve,
		pFont->m_bInterpColor,
		pFont->m_clUpperReserve,
		pFont->m_clLowerReserve,
		pFont->m_nCreateMode,
		pFont->m_strFaceName,
		pFont->m_bStereo,
		m_pDevice->GetA3DEngine());
}

void A3DFTFontMan::ReleaseFont(A3DFTFont* pFTFont)
{
	pFTFont->Release();
	delete pFTFont;
}

void A3DFTFontMan::ReleasePointFont(A3DFTFont* pFTFont)
{
	pFTFont->DecRefCnt();

	if (pFTFont->IsZeroRef())
	{
		Lock();

		for (size_t i = 0; i < m_FontBuf.size(); i++)
		{
			if (m_FontBuf[i] == pFTFont)
			{
				m_FontBuf.erase(m_FontBuf.begin() + i);
				break;
			}

		}

		Unlock();
		ReleaseFont(pFTFont);
	}
}

A3DStream* A3DFTFontMan::CreateStream(DWORD dwVertType, int iVertSize)
{
	A3DStream* pA3DStream = new A3DStream;

	if (!pA3DStream->Init(
		m_pDevice,
		iVertSize,
		dwVertType,
		_font_buf_size * 4,
		_font_buf_size * 6,
		A3DSTRM_REFERENCEPTR,
		A3DSTRM_STATIC))
	{
		delete pA3DStream;
		return NULL;
	}

	WORD* pVBIndices;
	if (!pA3DStream->LockIndexBuffer(0, 0, (LPBYTE*) &pVBIndices, NULL))
	{
		pA3DStream->Release();
		delete pA3DStream;
		return NULL;
	}

	for (int n=0, iIdx=0, iVert=0; n < _font_buf_size; n++, iIdx+=6, iVert+=4)
	{
		pVBIndices[iIdx+0] = iVert;
		pVBIndices[iIdx+1] = iVert + 1;
		pVBIndices[iIdx+2] = iVert + 2;
		pVBIndices[iIdx+3] = iVert + 2;
		pVBIndices[iIdx+4] = iVert + 1;
		pVBIndices[iIdx+5] = iVert + 3;
	}

	pA3DStream->UnlockIndexBuffer();
	return pA3DStream;
}

void A3DFTFontMan::RenderStream(
	int iVertSize,
	int iNumRect,
	const void* pSrc,
	bool bShadow,
	bool bStereo)
{
#ifdef _ANGELICA22
A3D_ITTX_SCOPED_TASK_NAMED(AGPAPerfIntegration::GetInstance().GetA3DDomain(), TEXT(__FUNCTION__));
#endif

	A3DStream* pStream = bShadow ? m_pShadowStream : m_pFrontStream;
	A3DTLVERTEX* pDestBuf;
	const char* pOff = (const char*)pSrc;
	A3DVertexShader* pShader = bShadow ? m_pStereoShadowShader : m_pStereoShader;
	A3DViewport* pView = m_pDevice->GetA3DEngine()->GetActiveViewport();

	int nMaxPosInStream = _font_buf_size * 4;
	while (iNumRect)
	{
		int nCount = iNumRect > _font_buf_size ? _font_buf_size : iNumRect;
		iNumRect -= nCount;

		int iNumVert = nCount * 4;
		int iNumIndex = nCount * 6;
		DWORD dwLockSize = iNumVert * iVertSize;

#ifdef _ANGELICA22
		if (bShadow)
		{
			if (m_nCurPosInShadowStream + iNumVert >= nMaxPosInStream)
			{
				if (!pStream->LockVertexBufferDynamic(0, dwLockSize, (BYTE**) &pDestBuf, 0))
					return;
				m_nCurPosInShadowStream = iNumVert;
			}
			else
			{
				if (!pStream->LockVertexBufferDynamic(m_nCurPosInShadowStream * iVertSize, dwLockSize, (BYTE**)&pDestBuf, D3DLOCK_NOOVERWRITE))
					return;
				m_nCurPosInShadowStream += iNumVert;
			}
		}
		else
		{
			if (m_nCurPosInFrontStream + iNumVert >= nMaxPosInStream)
			{
				if (!pStream->LockVertexBufferDynamic(0, dwLockSize, (BYTE**) &pDestBuf, 0))
					return;
				m_nCurPosInFrontStream = iNumVert;
			}
			else
			{
				if (!pStream->LockVertexBufferDynamic(m_nCurPosInFrontStream * iVertSize, dwLockSize, (BYTE**)&pDestBuf, D3DLOCK_NOOVERWRITE))
					return;
				m_nCurPosInFrontStream += iNumVert;
			}
		}
#else
		if (!pStream->LockVertexBuffer(0, dwLockSize, (BYTE**) &pDestBuf, 0))
			return;
#endif

		//	Copy data
		memcpy(pDestBuf, pOff, dwLockSize);
		pOff += dwLockSize;

		//	Unlock stream
		pStream->UnlockVertexBufferDynamic();

		pStream->Appear();

		if (pShader && bStereo)
		{
			pShader->Appear();
			A3DVECTOR4 c0(2.0f / pView->GetParam()->Width, -2.0f / pView->GetParam()->Height, 1.0f, 0.0f);
			m_pDevice->SetVertexShaderConstants(0, &c0, 1);
			A3DVECTOR4 c1(pView->GetParam()->X, pView->GetParam()->Y, 0.0f, 0.0f);
			m_pDevice->SetVertexShaderConstants(1, &c1, 1);
		}

		//	Render
#ifdef _ANGELICA22
		if (!m_pDevice->DrawIndexedPrimitive(
			A3DPT_TRIANGLELIST,
			bShadow ? (m_nCurPosInShadowStream - iNumVert) : (m_nCurPosInFrontStream - iNumVert),
			0,
			iNumVert,
			0,
			nCount * 2))
			return;
#else
		if (!m_pDevice->DrawIndexedPrimitive(
			A3DPT_TRIANGLELIST,
			0,
			iNumVert,
			0,
			nCount * 2))
			return;
#endif
	}
}

void A3DFTFontMan::Flush()
{
#ifdef _ANGELICA22
	A3D_ITTX_SCOPED_TASK_NAMED(AGPAPerfIntegration::GetInstance().GetA3DDomain(), TEXT(__FUNCTION__));
#endif
	Lock();
	bool b = m_pDevice->GetFogEnable();
	m_pDevice->SetFogEnable(false);

	size_t i;

	for (i = 0; i < m_RefCntFontBuf.size(); i++)
		m_RefCntFontBuf[i]->Flush();

	for (i = 0; i < m_FontBuf.size(); i++)
		m_FontBuf[i]->Flush();

	m_pDevice->SetFogEnable(b);
	Unlock();
}

A3DFTFont* A3DFTFontMan::CreatePointFont(const float fPointSize, const A3DFTFont* pTmplFont)
{
	if (!pTmplFont || fPointSize<=0 || pTmplFont->GetPointSize() == int(fPointSize)*10)
	{
		return const_cast<A3DFTFont*>(pTmplFont);
	}

	return CreatePointFont( fPointSize, 
							pTmplFont->GetFaceName(), 
							pTmplFont->GetStyle(),
							pTmplFont->GetCreateMode(), 
							pTmplFont->GetOutlineWidth(), 
							pTmplFont->GetOutlineColor(), 
							pTmplFont->GetInnerColorReserve(), 
							pTmplFont->IsInterpColor(), 
							pTmplFont->GetUpperColorReserve(),
							pTmplFont->GetLowerColorReserve(), 
							pTmplFont->IsStereo());
}