#include <sys/stat.h>
#include "A3D.h"
#include "A3DHLSL.h"
#include "A3DEffect.h"
#include "A3DTexture.h"
#include "AFileImage.h"
#include "a3dpi.h"
#include <Shlwapi.h>
#include "afi.h"
#include "A3DEnvironment.h"

#define SHADERINI_SECT "ShaderDesc"
#define SHADERINI_KEY_HLSL "HLSL"
#define SHADERINI_KEY_MACRO "Macro"
#define STRING_MASK		0x8e
#
class EffectFile : public AFileImage
{
public:
	bool ReadDecryptString(AString& str);
};

class EffectFileSave : public AFile
{
public:
	bool WriteEncryptString(const AString& str);
};

bool EffectFile::ReadDecryptString(AString& str)
{
	DWORD dwRead;

	//	Read length of string
	int iLen;
	if( !Read((BYTE*) &iLen, sizeof (int), &dwRead) )
		return false;

	//	Read string data
	if (iLen)
	{
		char* szBuf = new char [iLen+1];
		if (!szBuf)
			return false;

		if( !Read((BYTE*) szBuf, iLen, &dwRead) )
		{
			delete [] szBuf;
			return false;
		}
		szBuf[iLen] = '\0';
		for(int i = 0; i < iLen; i++)
		{
			szBuf[i] ^= STRING_MASK;
		}
		str = szBuf;

		delete [] szBuf;
	}
	else
		str = "";

	return true;
}

bool EffectFileSave::WriteEncryptString(const AString& str)
{
	int nCount = str.GetLength();
	if(nCount == 0)
	{
		return WriteString(str);
	}

	char* pBuffer = new char[nCount + 1];
	strncpy(pBuffer, str, nCount + 1);
	for(int i = 0; i < nCount; i++)
	{
		pBuffer[i] ^= STRING_MASK;
	}
	bool bVal = WriteString(pBuffer);
	delete pBuffer;
	return bVal;
}

//extern A3DCOMMONCONSTDESC s_aSkinRenderCommConst[];
//extern size_t g_cbSizeOfSkinRenderCommConst;
int A3DEFFECT_TOTAL_COUNT = 0;
A3DEffect::A3DEffect()
: m_pHLSL		(NULL)
, m_pOriginTex	(NULL)
, Diffuse		(0xffffffff)
, Emissive		(0)
//, m_crSource	(NULL)
//, m_crDest		(NULL)
{
	m_dwClassID	= A3D_CID_HLSLSHADER;
	A3DEFFECT_TOTAL_COUNT++;
}

A3DEffect::~A3DEffect()
{
	A3DEFFECT_TOTAL_COUNT--;
}

bool A3DEffect::Init(A3DDevice* pDevice)
{
	m_pA3DDevice = pDevice;
	return true;
}

bool A3DEffect::ReleaseSamplerDesc()
{
	for(SamplerDescArray::iterator it = m_aSampDesc.begin();
		it != m_aSampDesc.end(); ++it)	{
			if(it->pTexture != m_pOriginTex)
				m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(it->pTexture);
	}
	m_aSampDesc.clear();
	return true;
}
bool A3DEffect::Release()
{
	ReleaseSamplerDesc();
	A3DHLSLMan* pHLSLMan = m_pA3DDevice->GetA3DEngine()->GetA3DHLSLMan();
    m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pOriginTex);
	pHLSLMan->ReleaseShader(m_pHLSL);
	A3DTexture::Release();
	return true;
}

bool A3DEffect::Reload(bool bForceReload)
{
	// 不能调用基类的Reload, 这里面会重新加载m_pD3DTexture, 
	// 而A3DEffect是不需要且不能有私有的m_pD3DTexture对象的, 会占用过多内存
	//if( ! A3DTexture::Reload(bForceReload)) {
	//	return false;
	//}

	AString strMtlFile = GetMtlFilename();
	// 保存参数
	ParamArray aParam;
	GenerateParam(aParam);

	if( ! af_IsFileExist(strMtlFile))
	{
		if(m_pHLSL != NULL)
			return m_pHLSL->ReloadTexture();
		else
			return false;
	}

	if( !bForceReload )
	{
		struct stat fileStat;

		if(stat(strMtlFile, &fileStat) != 0 ||
			m_dwTimeStamp == fileStat.st_mtime)
		{
			if(m_pHLSL != NULL)
				return m_pHLSL->ReloadTexture();
			return true;
		}
	}

	ReleaseSamplerDesc();

	A3DHLSLMan* pHLSLMan = m_pA3DDevice->GetA3DEngine()->GetA3DHLSLMan();
	pHLSLMan->ReleaseShader(m_pHLSL);
	m_pHLSL = NULL;

	m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pOriginTex);


	bool bSucessLoad = Load(strMtlFile, A3DTexture::GetMapFile());

	if(bSucessLoad)
	{
		// 根据保存的参数恢复数据
		for(ParamArray::iterator it = aParam.begin();
			it != aParam.end(); ++it)
		{
			const A3DHLSL::CONSTMAPDESC* pConstMapDesc = m_pHLSL->GetConstMapByName(it->strKey);
			if(pConstMapDesc != NULL)
			{
				SetValueByString(*pConstMapDesc, it->strValue);
			}
		}
	}

	return bSucessLoad;
	//}
	//else
	//{
	//	if(m_pHLSL != NULL)
	//		return m_pHLSL->ReloadTexture();
	//}
	//return false;
}

bool A3DEffect::UpdateTexturePtr()
{
	return m_pHLSL->ReloadTexture();
}

// 这个函数在LitModel::Load也会使用
bool A3DEffect::Load(AFile* pFile, const char* szOriginFile)
{
	DWORD dwRead;
	EffectFile& file = (EffectFile&)*pFile;

	//char szFileDir[MAX_PATH];
	char buffer[MAX_PATH];
	EFFECTFILEHEADER2 efh;
	memset(&efh, 0, sizeof(EFFECTFILEHEADER2));
	//af_GetFilePath(pszFile, szFileDir, MAX_PATH);

	const size_t nHeaderSize = sizeof(efh.cbSize);

	if( ! file.Read(&efh.cbSize, nHeaderSize, &dwRead) || dwRead != nHeaderSize)
	{
		return false;
	}

	if (efh.cbSize != sizeof(EFFECTFILEHEADER) && 
		efh.cbSize != sizeof(EFFECTFILEHEADER2))
	{
		return false;
	}

	const size_t nHeaderSizeRamain = efh.cbSize - sizeof(efh.cbSize);
	if( ! file.Read(&efh.nParamCount, nHeaderSizeRamain, &dwRead) || dwRead != nHeaderSizeRamain)
	{
		return false;
	}

	if(efh.nParamCount < 0 || efh.nParamCount > 64)
	{
		return false;
	}

	AString strHLSLFile;
	AString strMacro;

	if(( ! file.ReadString(strHLSLFile)) || ( ! file.ReadString(strMacro)))
	{
		return false;
	}
	ParamDict sParamDict;
	for(int i = 0; i < efh.nParamCount; i++)
	{
		MTLPARAM Param;
		if(( ! file.ReadDecryptString(Param.strKey)) || ( ! file.ReadDecryptString(Param.strValue)))
		{
			return false;
		}
		sParamDict[Param.strKey] = Param.strValue;
	}

	if(strHLSLFile.GetLength() == 0)
	{
		g_A3DErrLog.Log("A3DEffect::Load, require to specify HLSL file.");
		return false;
	}
	A3DEngine* pA3DEngine = m_pA3DDevice->GetA3DEngine();
	A3DHLSLMan* pHLSLMan = pA3DEngine->GetA3DHLSLMan();
	A3DEnvironment* pEnvironment = pA3DEngine->GetA3DEnvironment();
	A3DCCDBinder* pBinder = pEnvironment->GetCCDBinder();
	A3DTextureMan* pA3DTextureMan = m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan();

	strncpy(buffer, strHLSLFile, MAX_PATH);
	//if( PathIsFileSpecA(buffer) )
	//{
	//	PathCombineA(buffer, szFileDir, strHLSLFile);
	//}
	m_pHLSL = pHLSLMan->LoadShader(NULL, NULL, buffer, "ps_main", strMacro.GetLength() == 0 ? NULL : strMacro, NULL);
	if(m_pHLSL == NULL)
	{
		g_A3DErrLog.Log("A3DEffect::Load, load hlsl failed(%s).", buffer);
		m_pHLSL = pHLSLMan->LoadShader(NULL, NULL, ERROR_MTL_FILE, "ps_main", NULL, NULL);
		if(m_pHLSL == NULL)  {
			return false;
		}
	}
	m_pHLSL->BindCommConstData(pBinder);

	if( ! LoadFromFile(m_pA3DDevice, szOriginFile, pFile->GetTimeStamp())) {
		return false;
	}
	//// 填充基类数据
	//if(szOriginFile != NULL &&
	//	pA3DTextureMan->LoadTextureFromFile(szOriginFile, &m_pOriginTex, A3DTF_DONOTLOADASSHADER) == false
	//	/*A3DTexture::LoadFromFile(m_pA3DDevice, szOriginFile, szOriginFile) == false*/)
	//{
	//	g_A3DErrLog.Log("A3DEffect::Load, can't load texture(%s).", m_strMapFile);
	//	return false;
	//}
	////A3DTexture::m_pA3DDevice = m_pA3DDevice;
	//m_bAlphaTexture = false;
	//m_nMipLevel = 1;
	//m_dwTimeStamp = pFile->GetTimeStamp();
	//af_GetRelativePath(szOriginFile, "", m_strMapFile);

	//// 替换m_dwTextureID数据,这个原来由A3DTexture::LoadFromFile计算
	//if(m_pHLSL->GetCore() != NULL)
	//{
	//	m_dwTextureID = m_pHLSL->GetCore()->GetID();
	//}

	const A3DHLSL::CONSTMAPDESC* pConstName = m_pHLSL->GetConstMapArray();
	int nCount = m_pHLSL->GetConstCount();
	for(int i = 0; i < nCount; i++)
	{
		const A3DHLSL::CONSTMAPDESC& cmd = pConstName[i];
		AString strValue;// = IniFile.GetValueAsString(SHADERINI_SECT, cmd.cd.cd.Name);
		ParamDict::iterator it = sParamDict.find(cmd.cd.cd.Name);

		if(it != sParamDict.end())
		{
			strValue = it->second;
		}

		// 如果配置文件没有找到键值
		if(strValue.GetLength() == 0)
		{
			// 如果是sampler则设置一个默认的纹理
			if(cmd.cd.cd.RegisterSet == D3DXRS_SAMPLER)
			{
				SAMPLER_DESC SampDesc;

				SampDesc.strName = cmd.cd.cd.Name;
				SampDesc.pTexture = m_pOriginTex;
				m_aSampDesc.push_back(SampDesc);
				m_pHLSL->SetTexture(SampDesc.strName, SampDesc.pTexture);
			}
			continue;
		}
		SetValueByString(cmd, strValue);
	}
	m_bAlphaTexture = (efh.bAlphaTexture != 0);
	return true;
}

bool A3DEffect::LoadFromFile(A3DDevice* pA3DDevice, const char* szRelPath, DWORD dwTimeStamp)
{
	A3DTextureMan* pA3DTextureMan = m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan();

	if(szRelPath != NULL &&
		pA3DTextureMan->LoadTextureFromFile(szRelPath, &m_pOriginTex, NULL) == false)
	{
		g_A3DErrLog.Log("A3DEffect::LoadFromFile, can't load texture(%s).", m_strMapFile);
		return false;
	}
	m_bAlphaTexture = false;
	m_nMipLevel = 1;
	if(dwTimeStamp == 0) {
		AFileImage file;
		if(file.Open(szRelPath, AFILE_BINARY | AFILE_OPENEXIST | AFILE_TEMPMEMORY)) {
			m_dwTimeStamp = file.GetTimeStamp();
		}
		file.Close();
	}
	else {
		m_dwTimeStamp = dwTimeStamp;
	}
	m_strMapFile = szRelPath;
	return true;
}

// 这个函数在LitModel::Load也会使用
bool A3DEffect::Save(AFile* pFile)
{
	AString strBuffer;
	AString strFilename;
	AString strMacro;
	AString strValue;
	DWORD dwWrite;

	int nCount = m_pHLSL->GetConstCount();

	EffectFileSave& file = (EffectFileSave&)*pFile;

	EFFECTFILEHEADER2 efh;
	efh.cbSize = sizeof(EFFECTFILEHEADER2);
	efh.nParamCount = nCount;
	efh.bAlphaTexture = m_bAlphaTexture;

	if( ! file.Write(&efh, sizeof(EFFECTFILEHEADER2), &dwWrite) || dwWrite != sizeof(EFFECTFILEHEADER2))
	{
		return false;
	}


	//// Section 名
	//strBuffer.Format("[%s]", SHADERINI_SECT);
	//file.WriteLine(strBuffer);

	//// 写入 HLSL 名
	strFilename = GetHLSLFilename();
	//strBuffer.Format("%s = %s", SHADERINI_KEY_HLSL, strFilename);
	//file.WriteLine(strBuffer);
	m_pHLSL->GetMacroAdapter(strMacro);

	file.WriteString(strFilename);
	file.WriteString(strMacro);

	// 写入参数
	const A3DHLSL::CONSTMAPDESC* pConstName = m_pHLSL->GetConstMapArray();
	for(int i = 0; i < nCount; i++)
	{
		const A3DHLSL::CONSTMAPDESC& cmd = pConstName[i];

		if( ! GetValueByString(cmd, strValue))
		{
			strValue.Empty();
		}
		file.WriteEncryptString(cmd.cd.cd.Name);
		file.WriteEncryptString(strValue);
	}

	return true;
}

bool A3DEffect::Load(const char* pszFile, const char* pszOriginFile)
{
	AFileImage file;

	if( ! file.Open(pszFile, AFILE_BINARY | AFILE_OPENEXIST | AFILE_TEMPMEMORY))
	{
		return false;
	}

	bool ret = Load(&file, pszOriginFile);

	file.Close();
	return ret;
}

bool A3DEffect::Save(const char* pszFile)
{
	//DWORD dwWrite;
	AFile file;
	AString strMtlFilename;

	// 得到材质名
	if(pszFile == NULL)
	{
		strMtlFilename = GetMtlFilename();
		pszFile = strMtlFilename;
	}

	if(file.Open(pszFile, AFILE_CREATENEW | AFILE_BINARY | AFILE_NOHEAD) == false)
	{
		g_A3DErrLog.Log("A3DEffect::Save, Can not mtl file(%s).", pszFile);
		return false;
	}

	return Save(&file);	
}

bool A3DEffect::SetValueByString(const A3DHLSL::CONSTMAPDESC& ConstDesc, const AString& strVaule)
{
	A3DVECTOR4 vValue;
	switch(ConstDesc.cd.cd.Class)
	{
	case D3DXPC_SCALAR:
		{
			sscanf(strVaule, "%f", &vValue.x);
			m_pHLSL->SetConstantArrayF(ConstDesc.cd.cd.Name, &vValue, 1);
		}
		break;
	case D3DXPC_VECTOR:
		{			
			switch(ConstDesc.cd.cd.Columns)
			{
			case 1:
				sscanf(strVaule, "(%f)", &vValue.x);
				break;
			case 2:
				sscanf(strVaule, "(%f,%f)", &vValue.x, &vValue.y);
				break;
			case 3:
				sscanf(strVaule, "(%f,%f,%f)", &vValue.x, &vValue.y, &vValue.z);
				break;
			case 4:
				sscanf(strVaule, "(%f,%f,%f,%f)", &vValue.x, &vValue.y, &vValue.z, &vValue.w);
				break;
			}
			m_pHLSL->SetConstantArrayF(ConstDesc.cd.cd.Name, &vValue, 1);
		}
		break;
	case D3DXPC_MATRIX_ROWS:
		// FIXME: 未实现
		ASSERT(0);
		break;
	case D3DXPC_MATRIX_COLUMNS:
		{
			A3DMATRIX4 mat;
			switch(ConstDesc.cd.cd.Rows)
			{
			case 1:
				sscanf(strVaule, "(%f,%f,%f,%f)", 
					&mat._11, &mat._12, &mat._13, &mat._14);
				break;
			case 2:
				sscanf(strVaule,
					"(%f,%f,%f,%f),(%f,%f,%f,%f)", 
					&mat._11, &mat._12, &mat._13, &mat._14,
					&mat._21, &mat._22, &mat._23, &mat._24);
				break;
			case 3:
				sscanf(strVaule, "(%f,%f,%f,%f),"
					"(%f,%f,%f,%f),(%f,%f,%f,%f)", 
					&mat._11, &mat._12, &mat._13, &mat._14,
					&mat._21, &mat._22, &mat._23, &mat._24,
					&mat._31, &mat._32, &mat._33, &mat._34);
				break;
			case 4:
				sscanf(strVaule,
					"(%f,%f,%f,%f),(%f,%f,%f,%f),"
					"(%f,%f,%f,%f),(%f,%f,%f,%f)", 
					&mat._11, &mat._12, &mat._13, &mat._14,
					&mat._21, &mat._22, &mat._23, &mat._24,
					&mat._31, &mat._32, &mat._33, &mat._34,
					&mat._41, &mat._42, &mat._43, &mat._44);
				break;
			}
			m_pHLSL->SetConstantArrayF(ConstDesc.cd.cd.Name, (A3DVECTOR4*)&mat, ConstDesc.cd.cd.Rows);
		}
		break;
	case D3DXPC_OBJECT:
		{
			SetTextureByName(ConstDesc.cd.cd.Name, strVaule);
		}
		break;
	case D3DXPC_STRUCT:
		// FIXME: 未实现
		ASSERT(0);
		break;
	}
	return true;
}

bool A3DEffect::GetValueByString(const A3DHLSL::CONSTMAPDESC& ConstDesc, AString& strVaule)
{
	A3DVECTOR4 vValue;
	bool bVal = false;
	switch(ConstDesc.cd.cd.Class)
	{
	case D3DXPC_SCALAR:
		{
			bVal = m_pHLSL->GetConstantArrayF(ConstDesc.cd.cd.Name, &vValue, 1);
			if(bVal)
			{
				strVaule.Format("%.3f", vValue.x);
			}
		}
		break;
	case D3DXPC_VECTOR:
		{
			bVal = m_pHLSL->GetConstantArrayF(ConstDesc.cd.cd.Name, &vValue, 1);
			if(bVal)
			{
				switch(ConstDesc.cd.cd.Columns)
				{
				case 1:
					strVaule.Format("(%.3f)", vValue.x);
					break;
				case 2:
					strVaule.Format("(%.3f,%.3f)", vValue.x, vValue.y);
					break;
				case 3:
					strVaule.Format("(%.3f,%.3f,%.3f)", vValue.x, vValue.y, vValue.z);
					break;
				case 4:
					strVaule.Format("(%.3f,%.3f,%.3f,%.3f)", vValue.x, vValue.y, vValue.z, vValue.w);
					break;
				}
			}
		}
		break;
	case D3DXPC_MATRIX_ROWS:
		// FIXME: 未实现
		ASSERT(0);
		break;
	case D3DXPC_MATRIX_COLUMNS:
		{
			A3DMATRIX4 mat;
			bVal = m_pHLSL->GetConstantArrayF(ConstDesc.cd.cd.Name, (A3DVECTOR4*)&mat, ConstDesc.cd.cd.Rows);
			if(bVal)
			{
				switch(ConstDesc.cd.cd.Rows)
				{
				case 1:
					strVaule.Format(
						"(%.3f,%.3f,%.3f,%.3f)", 
						mat._11, mat._12, mat._13, mat._14);
					break;
				case 2:
					strVaule.Format(
						"(%.3f,%.3f,%.3f,%.3f),"
						"(%.3f,%.3f,%.3f,%.3f)", 
						mat._11, mat._12, mat._13, mat._14,
						mat._21, mat._22, mat._23, mat._24);
					break;
				case 3:
					strVaule.Format(
						"(%.3f,%.3f,%.3f,%.3f),"
						"(%.3f,%.3f,%.3f,%.3f),"
						"(%.3f,%.3f,%.3f,%.3f)", 
						mat._11, mat._12, mat._13, mat._14,
						mat._21, mat._22, mat._23, mat._24,
						mat._31, mat._32, mat._33, mat._34);
					break;
				case 4:
					strVaule.Format(
						"(%.3f,%.3f,%.3f,%.3f),"
						"(%.3f,%.3f,%.3f,%.3f),"
						"(%.3f,%.3f,%.3f,%.3f),"
						"(%.3f,%.3f,%.3f,%.3f)", 
						mat._11, mat._12, mat._13, mat._14,
						mat._21, mat._22, mat._23, mat._24,
						mat._31, mat._32, mat._33, mat._34,
						mat._41, mat._42, mat._43, mat._44);
					break;
				}
			}
		}
		break;
	case D3DXPC_OBJECT:
		{
			A3DTexture* pTexture = GetTextureByName(ConstDesc.cd.cd.Name);
			if(pTexture != NULL)
			{
				AString strDir = GetMtlFilename();
				AString strPath;
				AString strFullPath;
				af_GetFilePath(strDir, strDir);
				af_GetFullPath(strFullPath, pTexture->GetMapFile());
				af_GetRelativePath(strFullPath, strDir, strPath);
				af_GetRelativePathNoBase(strPath, af_GetBaseDir(), strPath);
				strVaule = strPath;
				bVal = true;
			}
			else
			{
				strVaule.Empty();
			}
		}
		break;
	case D3DXPC_STRUCT:
		// FIXME: 未实现
		ASSERT(0);
		break;
	}
	return bVal;
}


A3DEffect* A3DEffect::CreateHLSLShader(A3DDevice* pA3DDevice, const char* pszTextureFile, const char* pszHLSLFile, bool bForceLoad)
{
	A3DEffect* pHLSLShader = NULL;
	AString strFullPath;
	char szMtlFile[MAX_PATH];
	strncpy(szMtlFile, pszTextureFile, MAX_PATH);
	//PathRenameExtensionA(szMtlFile, ".mtl");
	af_ChangeFileExt(szMtlFile, MAX_PATH, ".mtl");
	A3DTextureMan* pTextureMan = pA3DDevice->GetA3DEngine()->GetA3DTextureMan();

	pTextureMan->LoadTextureFromFile(szMtlFile, (A3DTexture**)&pHLSLShader, A3DTF_CREATEHLSLSHADER);

	// 如果已经初始化,则直接返回,不加这个在多线程载入时会有问题.
	if(pHLSLShader->m_pHLSL != NULL && ( ! bForceLoad))
	{
		return pHLSLShader;
	}

	// pTextureMan->LoadTextureFromFile 也调用了一次 Init
	if( ! pHLSLShader->Init(pA3DDevice) )
	{
		g_A3DErrLog.Log("A3DEffect::CreateHLSLShader, Can not init mtl.");
		goto FAILED_RET;
	}

	af_GetFullPath(strFullPath, pszTextureFile);
	//if( ! pHLSLShader->LoadFromFile(pA3DDevice, strFullPath, pszTextureFile) )
	if( ! pHLSLShader->LoadFromFile(pA3DDevice, pszTextureFile, 0))
	{
		g_A3DErrLog.Log("A3DEffect::CreateHLSLShader, can't load texture(%s).", pszTextureFile);
		goto FAILED_RET;
	}

	//pHLSLShader->m_bAlphaTexture = false;

	if( ! pHLSLShader->SetHLSL(pszHLSLFile))
	{
		g_A3DErrLog.Log("A3DEffect::CreateHLSLShader, can't load hlsl(%s), maybe has some errors in it.", pszHLSLFile);
		goto FAILED_RET;
	}
	return pHLSLShader;

FAILED_RET:
	pTextureMan->ReleaseTexture(((A3DTexture*&)pHLSLShader));
	return NULL;
}

bool A3DEffect::SetHLSL(const char* pszHLSLFile)
{
	A3DEngine* pA3DEngine = m_pA3DDevice->GetA3DEngine();
	A3DHLSLMan* pHLSLMan = pA3DEngine->GetA3DHLSLMan();
	A3DEnvironment* pEnvironment = pA3DEngine->GetA3DEnvironment();
	A3DCCDBinder* pBinder = pEnvironment->GetCCDBinder();

	ReleaseSamplerDesc();
	if(m_pHLSL != NULL) {
		pHLSLMan->ReleaseShader(m_pHLSL);
	}

	//if(af_CheckFileExt(pszHLSLFile, ".obj")) {
	//	m_pHLSL = pHLSLMan->LoadShader(pszHLSLFile, NULL);
	//}
	//else {
		m_pHLSL = pHLSLMan->LoadShader(NULL, NULL, pszHLSLFile, "ps_main", NULL, NULL);
	//}

	if(m_pHLSL == NULL)
	{
		g_A3DErrLog.Log("A3DEffect::SetHLSL, can't load hlsl(%s).", pszHLSLFile);
		return false;
	}

	m_pHLSL->BindCommConstData(pBinder);

	const A3DHLSL::CONSTMAPDESC* pConstName = m_pHLSL->GetConstMapArray();
	int nCount = m_pHLSL->GetConstCount();
	for(int i = 0; i < nCount; i++)
	{
		const A3DHLSL::CONSTMAPDESC& cmd = pConstName[i];

		if(cmd.cd.cd.RegisterSet == D3DXRS_SAMPLER)
		{
			SAMPLER_DESC SampDesc;

			// 设置Sampler中的纹理为原纹理
			SampDesc.strName = cmd.cd.cd.Name;
			SampDesc.pTexture = m_pOriginTex;
			m_aSampDesc.push_back(SampDesc);
			m_pHLSL->SetTexture(SampDesc.strName, SampDesc.pTexture);

			// 查询 Annotation 是否有纹理的描述,如果有则替换
			A3DHLSLCore* pCore = m_pHLSL->GetCore();
			A3DHLSLCore::LPCCONSTANTDESC pConstDesc = pCore->GetConstDescByName(SampDesc.strName);
			if(pConstDesc != NULL)
			{
				A3DHLSLCore::LPCSAMPLER_STATE pSamplerState = pCore->GetSamplerState(pConstDesc->cd.RegisterIndex);
				if(pSamplerState != NULL && pSamplerState->strDefault.GetLength() > 0)
				{
					SetTextureByName(SampDesc.strName, pSamplerState->strDefault);
				}
			}
			//const A3DHLSL::ANNOTATION* pAnno = m_pHLSL->FindAnnotationByName(SampDesc.strName, "Texture");
			//if(pAnno != NULL && pAnno->Type == D3DXPT_STRING)
			//{
			//	pAnno = m_pHLSL->FindAnnotationByName((const char*)pAnno->pData, "UIObject");
			//	if(pAnno != NULL && pAnno->Type == D3DXPT_STRING)
			//	{
			//		SetTextureByName(SampDesc.strName, (const char*)pAnno->pData);
			//	}
			//}
		}
	}
	return true;
}

bool A3DEffect::SetMacros(AStringArray& aMacros, DWORD dwFlags)
{
	AString strMacros;
	for(AStringArray::iterator it = aMacros.begin(); 
		it != aMacros.end(); ++it)  {
			if(it->GetLength() == 0)  {
				continue;
			}
			strMacros += (*it) + ';';
	}
	return SetMacros(strMacros, dwFlags);
}

bool A3DEffect::SetMacros(const char* szMacros, DWORD dwFlags)
{
	if(m_pHLSL != NULL)
	{
		ParamArray aParam;

		GenerateParam(aParam);	// 保存参数

		bool bval = m_pHLSL->SetMacroAdapter(szMacros, dwFlags);
		if( ! bval)  {
			return false;
		}

		ReleaseSamplerDesc();	// 释放所有相关纹理

		// 根据保存的参数恢复数据
		for(ParamArray::iterator it = aParam.begin();
			it != aParam.end(); ++it)
		{
			const A3DHLSL::CONSTMAPDESC* pConstMapDesc = m_pHLSL->GetConstMapByName(it->strKey);
			if(pConstMapDesc != NULL)
			{
				SetValueByString(*pConstMapDesc, it->strValue);
			}
		}

		// 检查纹理, 如果采样器是空纹理则使用默认纹理填补
		int nCount = m_pHLSL->GetConstCount();
		A3DHLSLCore* pCore = m_pHLSL->GetCore();
		const A3DHLSL::CONSTMAPDESC* pConstName = m_pHLSL->GetConstMapArray();
		for(int i = 0; i < nCount; i++)
		{
			const A3DHLSL::CONSTMAPDESC& cmd = pConstName[i];
			if(cmd.cd.cd.Class == D3DXPC_OBJECT && m_pHLSL->GetTexture(cmd.cd.cd.RegisterIndex) == NULL)
			{
				A3DHLSLCore::LPCCONSTANTDESC pConstDesc = pCore->GetConstDescByName(cmd.cd.cd.Name);
				if(pConstDesc != NULL)
				{
					A3DHLSLCore::LPCSAMPLER_STATE pSamplerState = pCore->GetSamplerState(pConstDesc->cd.RegisterIndex);
					if(pSamplerState != NULL)
					{
						SetTextureByName(cmd.cd.cd.Name, pSamplerState->strDefault);
					}
				}
			}
		}
		return bval;
	}
	return false;
}

bool A3DEffect::GetMacros(AString& strMacros)
{
	if(m_pHLSL == NULL)  {
		strMacros.Empty();
		return false;
	}

	return m_pHLSL->GetMacroAdapter(strMacros);
}

void A3DEffect::SetAlphaTexture(bool bAlpha)
{
	m_bAlphaTexture = bAlpha;
}

bool A3DEffect::Appear(const A3DCOMMCONSTTABLE* lpCommConstTable)
{
	if(m_pHLSL != NULL)
	{
		m_pHLSL->Appear(lpCommConstTable);
		return true;
	}
	return false;
}

bool A3DEffect::Commit(const A3DCOMMCONSTTABLE* lpCommConstTable)
{
	if(m_pHLSL != NULL)
	{
		m_pHLSL->Commit(lpCommConstTable);
		return true;
	}
	return false;
}

bool A3DEffect::Appear(int nLayer)
{
	if(m_pHLSL != NULL)
	{
		m_pHLSL->Appear(NULL);
		return true;
	}
	return false;
}

bool A3DEffect::Disappear(int nLayer)
{
	if(m_pHLSL != NULL) {
		m_pHLSL->Disappear();
	}
	//m_pA3DDevice->ClearVertexShader();
	//m_pA3DDevice->ClearPixelShader();
	return true;
}

A3DTexture* A3DEffect::GetTextureByName(const char* pszName) const
{
	for(SamplerDescArray::const_iterator it = m_aSampDesc.begin();
		it != m_aSampDesc.end(); ++it)	
	{
		if(it->strName == pszName)
		{
			return it->pTexture;
		}
	}
	return NULL;
}

bool A3DEffect::SetTextureByName(const char* pszName, const char* pszFilename)
{
	A3DTexture* pNewTexture;
	A3DTextureMan* pA3DTextureMan = m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan();
	AString strFilename;
	if(m_pHLSL == NULL)
	{
		g_A3DErrLog.Log("A3DEffect::SetTextureByName, invalid HLSL object.");
		return false;
	}

	// 如果只有文件名,则将当前Mtl路径加到文件名前
	if( PathIsFileSpecA(pszFilename) )
	{
		char buffer[MAX_PATH];
		af_GetFilePath(GetMtlFilename(), buffer, MAX_PATH);
		af_GetFullPathNoBase(strFilename, buffer, pszFilename);

	}
	else
		strFilename = pszFilename;

	const A3DHLSL::CONSTMAPDESC* pConstMap = m_pHLSL->GetConstMapByName(pszName);
	if(pConstMap == NULL)
	{
		g_A3DErrLog.Log("A3DEffect::SetTextureByName, can not find the specified name of the sampler.");
		return false;
	}

	DWORD dwFlags = NULL;
	if(pConstMap->cd.cd.Type == D3DXPT_SAMPLER3D)  {
		dwFlags |= A3DTF_TEXTURE3D;
	}
	else if(pConstMap->cd.cd.Type == D3DXPT_SAMPLERCUBE)  {
		dwFlags |= A3DTF_TEXTURECUBE;
	}

	//Edited By PYP, 2013/1/25, add Mipmap when Loading textures in the effect
	dwFlags |= A3DTF_MIPLEVEL;

	if(pA3DTextureMan->LoadTextureFromFile(strFilename, (A3DTexture**)&pNewTexture, dwFlags, 0) == false)
	{
		g_A3DErrLog.Log("A3DEffect::SetTextureByName, Can not load texture(%s).", strFilename);
		return false;
	}

    if (pNewTexture == m_pOriginTex)
    {
        pA3DTextureMan->ReleaseTexture(pNewTexture);
        pNewTexture = m_pOriginTex;
    }

	for(SamplerDescArray::iterator it = m_aSampDesc.begin();
		it != m_aSampDesc.end(); ++it)	
	{
		if(it->strName == pszName)
		{
			if(it->pTexture != m_pOriginTex)
				pA3DTextureMan->ReleaseTexture(it->pTexture);

			it->pTexture = pNewTexture;
			m_pHLSL->SetTexture(pszName, pNewTexture);
			return true;
		}
	}

	// 没有找到则尝试设置, 如果有对应采样器就增加到列表里
	if(m_pHLSL->SetTexture(pszName, pNewTexture) == true)
	{
		SAMPLER_DESC SampDesc;
		SampDesc.strName = pszName;
		SampDesc.pTexture = pNewTexture;
		m_aSampDesc.push_back(SampDesc);
		return true;
	}

    if (pNewTexture != m_pOriginTex)
	    pA3DTextureMan->ReleaseTexture(pNewTexture);

	return false;
}

AString A3DEffect::GetMtlFilename()
{
	char buffer[MAX_PATH];
	strncpy(buffer, m_strMapFile, MAX_PATH);
	//PathRenameExtensionA(buffer, ".mtl");
	af_ChangeFileExt(buffer, MAX_PATH, ".mtl");
	return AString(buffer);
}

AString A3DEffect::GetHLSLFilename()
{
	AString strFilename;
	if(m_pHLSL != NULL && m_pHLSL->GetCore() != NULL)
	{
		A3DHLSLMan::ParseName(m_pHLSL->GetCore()->GetName(), NULL, NULL, &strFilename, NULL, NULL);
	}
	return strFilename;
}
bool A3DEffect::GenerateParam(ParamArray& aParam)
{
	if(m_pHLSL == NULL)
	{
		return false;
	}

	aParam.clear();
	int nCount = m_pHLSL->GetConstCount();
	const A3DHLSL::CONSTMAPDESC* pConstDesc = m_pHLSL->GetConstMapArray();
	for(int i = 0; i < nCount; i++)
	{
		MTLPARAM Param;
		Param.strKey = pConstDesc[i].cd.cd.Name;
		GetValueByString(pConstDesc[i], Param.strValue);
		if(Param.strValue.GetLength() > 0) {
			aParam.push_back(Param);
		}
	}
	return true;
}

bool A3DEffect::SetDiffuse(const A3DCOLOR& clr)
{
	Diffuse = clr;
	A3DCOLORVALUE clrval = clr;
	return m_pHLSL->SetValue4f(":DiffuseFactor", (const A3DVECTOR4*)&clrval);
}

bool A3DEffect::SetEmissive(const A3DCOLOR& clr)
{
	Emissive = clr;
	A3DCOLORVALUE clrval = clr;
	return m_pHLSL->SetValue4f(":EmissiveFactor", (const A3DVECTOR4*)&clrval);
}

IDirect3DBaseTexture9* A3DEffect::GetD3DBaseTexture() const
{
	return m_pOriginTex->GetD3DBaseTexture();
}

bool A3DEffect::AppearTextureOnly( int nLayer )
{
	if(m_pDXTexture)
		return A3DTexture::Appear(nLayer);
	for(int i = 0; i < MAX_SAMPLERCOUNT; i++)
	{
		LPDIRECT3DBASETEXTURE9 pTexture = m_pHLSL->GetD3DTexture(i);
		if(pTexture)
		{
			return m_pA3DDevice->raw_SetTexture(nLayer, pTexture) == S_OK;
		}
	}
	return false;
}


bool A3DEffect::HasBloomEffect()
{
	return GetHLSLFilename().Find("bloom") >= 0;
}
