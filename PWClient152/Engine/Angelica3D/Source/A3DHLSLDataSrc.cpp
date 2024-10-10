#include "A3D.h"
#include "A3DHLSL.h"
#include "A3DHLSLDataSrc.h"
#include "A3DPI.h"
#include "AFI.h"
#include "AFilePackage.h"
#include "AFileImage.h"
#include "A3DFuncs.h"
#include <Shlwapi.h>

#define HLSLDATASRC_VERSION_001	0x10000001
#define SAFE_RELEASE(p)	if((p) != NULL){ (p)->Release(); p = NULL; }
const int c_MaxMacroGroup = sizeof(((A3DHLSLDataSrc::MACROPROP*)0)->wGroup) * 8;

const char c_szFileDescDef[] = "#line 1 \"";


//static char s_szEffectSupplyVS[] = "					\
//struct ___VS_INPUT{	float4 vPosition : POSITION;};		\
//struct ___VS_OUTPUT{ float4 Position : POSITION;};		\
//___VS_OUTPUT __vs_main(___VS_INPUT Input){				\
//	___VS_OUTPUT Output;								\
//	Output.Position = Input.vPosition;					\
//return Output;	}\r\n";
//
//static char s_szEffectSupplyPS[] = "					\
//struct ___VS_OUTPUT{ float4 Position : POSITION;};		\
//float4 __ps_main(___VS_OUTPUT Input){					\
//return float4(0,0,0,1);	}\r\n";
//
//static char s_szEffectSupplyTech[] = "\r\n				\
//technique ___TechBasic {								\
//	pass P0	{											\
//		VertexShader = compile vs_3_0 __vs_main();		\
//		PixelShader  = compile ps_3_0 __ps_main();		\
//}}\0\0";
//
//static char s_szEffectSupplyTechFmt[] = "\r\n			\
//technique ___TechBasic {								\
//	pass P0	{											\
//			VertexShader = compile vs_3_0 %s();			\
//			PixelShader  = compile ps_3_0 %s();			\
//}}\0\0";


static char s_szEffectSupplyFull[] = "\r\n				\
 struct ___VS_INPUT{	float4 vPosition : POSITION;};		\
	 struct ___VS_OUTPUT{ float4 Position : POSITION;};		\
	 ___VS_OUTPUT __vs_main(___VS_INPUT Input)  {			\
		 ___VS_OUTPUT Output;								\
		 Output.Position = Input.vPosition;					\
		 return Output;	}\r\n								\
	float4 __ps_main(___VS_OUTPUT Input) : COLOR0  {		\
	 return float4(0,0,0,1);	}\r\n						\
	 technique ___TechBasic  {								\
	 pass P0  {												\
			VertexShader = compile vs_3_0 __vs_main();		\
			 PixelShader  = compile ps_3_0 __ps_main();		\
}}\0\0";

struct A3DHLSLDATASRCFILEHEADER
{
	DWORD cbSize;
	DWORD dwVersion;
	size_t nShaderInst;
	size_t nMacro;
	size_t nNameSema;
	size_t nEntryName;
};

struct SHADERINSTHEADER
{
	DWORD dwEntryID;
	DWORD dwProfile;
	DWORD dwShaderSize;
};


//////////////////////////////////////////////////////////////////////////
extern char g_szDefaultVSEntryName[];
extern char g_szDefaultPSEntryName[];

A3DHLSLDataSrc::ENTRYNAME A3DHLSLDataSrc::s_DefVertexEntry = {
	"vs_3_0", "vs_main", a_MakeIDFromString(g_szDefaultVSEntryName)
};
A3DHLSLDataSrc::ENTRYNAME A3DHLSLDataSrc::s_DefPixelEntry = {
	"ps_3_0", "ps_main", a_MakeIDFromString(g_szDefaultPSEntryName)
};
// Profile|EntryName|Macro01;Macro02=define;Macro03...

A3DHLSLDataSrcInclude::A3DHLSLDataSrcInclude(A3DHLSLDataSrc* pDataSrc)
: m_pHLSLDataSrc(pDataSrc)
{
}

A3DHLSLDataSrcInclude::~A3DHLSLDataSrcInclude()
{
}

bool A3DHLSLDataSrcInclude::Init()
{
	m_strSystemDir = AString(af_GetBaseDir()) + "\\shaders\\2.2\\";
	m_strLocalDir = m_strSystemDir;
	return true;
}

bool A3DHLSLDataSrcInclude::Release()
{
	return true;
}

void A3DHLSLDataSrcInclude::SetDir(D3DXINCLUDE_TYPE IncludeType, const AString& strDir)
{
	if(IncludeType == D3DXINC_LOCAL)
		m_strLocalDir = strDir;
	else if(IncludeType == D3DXINC_SYSTEM)
		m_strSystemDir = strDir;
}

AStringArray* A3DHLSLDataSrcInclude::GetIncludeFiles()
{
	return &m_aIncludeFiles;
}

HRESULT A3DHLSLDataSrcInclude::Open(THIS_ D3DXINCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID *ppData, UINT *pBytes)
{
	char szAbsPath[MAX_PATH];
	char szWholePath[MAX_PATH];

	if(IncludeType == D3DXINC_LOCAL)
		PathCombineA(szWholePath, m_strLocalDir, pFileName);
	else if(IncludeType == D3DXINC_SYSTEM)
		PathCombineA(szWholePath, m_strSystemDir, pFileName);

	char* pSlash;
	while((pSlash = strchr(szWholePath, '/')) != NULL)
	{
		*pSlash = '\\';
	}

	PathCanonicalizeA(szAbsPath, szWholePath);

	AFileImage file;
	if(file.Open(szAbsPath, AFILE_BINARY | AFILE_OPENEXIST | AFILE_TEMPMEMORY) == true)
	{
		DWORD dwLen = file.GetFileLength();
		BYTE* pData = new BYTE[dwLen];
		DWORD dwRead;
		file.Read(pData, dwLen, &dwRead);

		*ppData = pData;
		*pBytes = dwRead;

		m_aIncludeFiles.push_back(szAbsPath);
		return S_OK;
	}
	return MK_E_CANTOPENFILE;
}

HRESULT A3DHLSLDataSrcInclude::Close(THIS_ LPCVOID pData)
{
	delete pData;
	return S_OK;
}
//////////////////////////////////////////////////////////////////////////
A3DHLSLDataSrc::A3DHLSLDataSrc(A3DDevice* pA3DDevice, A3DHLSLMan* pHLSLMgr, const char* szShaderFile)
: m_pA3DDevice		(pA3DDevice)
, m_pHLSLMgr		(pHLSLMgr)
, m_pName2ParamDict	(NULL)
, m_nRefCount		(0)
{
	A3DObject::SetName(szShaderFile);
	m_strName.MakeLower();
	for(int i = 0; i < MAX_SAMPLERCOUNT; i++)
	{
		m_aSampState[i].Cleanup();
	}
	ASSERT(sizeof(MACROPROP) == 12);
}

int A3DHLSLDataSrc::AddRef()
{
	return ++m_nRefCount;
}

int A3DHLSLDataSrc::Release()
{
	if(--m_nRefCount)  {
		return m_nRefCount;
	}
	Cleanup();
	delete this;
	return 0;
}

bool A3DHLSLDataSrc::CleanupParameter()
{
	if(m_pName2ParamDict != NULL)
	{
		Name2ParamDict::iterator it = m_pName2ParamDict->begin();
		for(; it != m_pName2ParamDict->end(); ++it)
		{
			AnnotationArray& aAnno = it->second.aAnnotation;
			for(AnnotationArray::iterator itAnno = aAnno.begin();
				itAnno != aAnno.end(); ++itAnno)
			{
				if(itAnno->pData != NULL)
				{
					delete itAnno->pData;
				}
			}
			//delete it->second;
		}
		delete m_pName2ParamDict;
		m_pName2ParamDict = NULL;
	}
	return true;
}

bool A3DHLSLDataSrc::Cleanup()
{
	CleanupParameter();

	for(ShaderInstArray::iterator it = m_aInstance.begin();
		it != m_aInstance.end(); ++it)
	{
		SHADERINSTANCE* pShaderInst = *it;
		if( ! m_pA3DDevice)
		{
			SAFE_RELEASE(pShaderInst->pConstants);
			SAFE_RELEASE(pShaderInst->pPShader);
			SAFE_RELEASE(pShaderInst->pShaderBuffer);
			SAFE_RELEASE(pShaderInst->pVShader);
		}
		else
		{
			m_pA3DDevice->ReleaseResource(pShaderInst->pConstants); //multiThread Render
			m_pA3DDevice->ReleaseResource(pShaderInst->pPShader); //multiThread Render
			m_pA3DDevice->ReleaseResource(pShaderInst->pShaderBuffer); //multiThread Render
			m_pA3DDevice->ReleaseResource(pShaderInst->pVShader);
		}

		pShaderInst->pConstants = NULL;
		pShaderInst->pPShader = NULL;
		pShaderInst->pShaderBuffer = NULL;
		pShaderInst->pVShader = NULL;

		delete pShaderInst;
	}
	return true;
}

bool A3DHLSLDataSrc::BuildEffect(const BYTE* pShaderCode, size_t uCodeLen, LPD3DXINCLUDE lpInclude, DWORD dwFlags)
{
	bool bval = true;
	if(pShaderCode != NULL)
	{
		BYTE* pEffectCode = NULL;
		UINT uSrcLen = 0;
		LPD3DXBUFFER lpEffectErrorBuffer = NULL;
		LPD3DXEFFECTCOMPILER lpD3DXEffect = NULL;
		//LPDIRECT3DDEVICE9 pd3dDevice = m_pA3DDevice->GetD3DDevice();

		uSrcLen = uCodeLen + sizeof(s_szEffectSupplyFull);
		pEffectCode = new BYTE[uSrcLen];
		memcpy(pEffectCode, pShaderCode, uCodeLen);
		memcpy(pEffectCode + uCodeLen, s_szEffectSupplyFull, sizeof(s_szEffectSupplyFull));

		if(FAILED(D3DXCreateEffectCompiler((LPCSTR)pEffectCode, uSrcLen, NULL, 
			lpInclude, NULL, &lpD3DXEffect, &lpEffectErrorBuffer)))
		{
			g_A3DErrLog.Log("Failed to compile effect, you will not get annotation for the shader.");

			// 完全编译模式如果Effect失败就直接报错
			if(dwFlags & A3DHLSLDATASRC_ASMALLINST)
			{
				m_pHLSLMgr->AppendErrorMsg((const char*)lpEffectErrorBuffer->GetBufferPointer());
				bval = false;
			}
		}

		if(pEffectCode != NULL)
		{
			delete pEffectCode;
			pEffectCode = NULL;
		}
		if(lpD3DXEffect != NULL)  {
			GenerateAnnotation(lpD3DXEffect);
		}
		GenerateSamplerState();
		SAFE_RELEASE(lpD3DXEffect); //MultiThread Render FIXME!!
		return bval;
	}
	return bval;
}

bool A3DHLSLDataSrc::GenerateAnnotation(LPD3DXBASEEFFECT lpD3DXEffect)
{
	ASSERT(lpD3DXEffect != NULL);
	ASSERT(m_pName2ParamDict == NULL);
	ASSERT(m_NameSemanticDict.size() == 0);

	//const bool bSupportVS30 = m_pA3DDevice == NULL ? true : m_pA3DDevice->TestVertexShaderVersion(3, 0);
	//intel G33/G31 等垃圾显卡不支持VS3.0但是返回true,所以这里VS版本以PS为准
	const bool bSupportVS30 = m_pA3DDevice == NULL ? true : m_pA3DDevice->TestPixelShaderVersion(3, 0);
	const bool bSupportPS30 = m_pA3DDevice == NULL ? true : m_pA3DDevice->TestPixelShaderVersion(3, 0);
	const bool bShader30 = bSupportVS30 || bSupportPS30;

	m_pName2ParamDict = new Name2ParamDict;

	D3DXEFFECT_DESC d3ded;
	ENTRYNAME EntryName;

	lpD3DXEffect->GetDesc(&d3ded);
	//NAMESEMANTICPAIR NSPair;
	for(UINT i = 0; i < d3ded.Parameters; i++)
	{
		PARAMETER_DESC Param;
		D3DXHANDLE hHandle = lpD3DXEffect->GetParameter(NULL, i);
		D3DXPARAMETER_DESC d3dxpd;
		lpD3DXEffect->GetParameterDesc(hHandle, &d3dxpd);


		// 保存语义信息
		if(d3dxpd.Semantic != NULL)
		{
			//NSPair.strName = d3dxpd.Name;
			//NSPair.strSemantic = d3dxpd.Semantic;
			//m_NameSemanticDict.push_back(NSPair);
			m_NameSemanticDict[d3dxpd.Name] = d3dxpd.Semantic;
			Param.strSemantic = d3dxpd.Semantic;
		}
		Param.Class = d3dxpd.Class;
		Param.Type = d3dxpd.Type;

		//// 保存Annotation信息.
		//if(d3dxpd.Annotations > 0)
		//{
		//	AnnotationArray* pAnnotation = new AnnotationArray;
		ANNOTATION annot;
		for(UINT n = 0; n < d3dxpd.Annotations; n++)
		{
			D3DXPARAMETER_DESC d3dxad;
			D3DXHANDLE hAnnotation = lpD3DXEffect->GetAnnotation(hHandle, n);

			lpD3DXEffect->GetParameterDesc(hAnnotation, &d3dxad);
			annot.Type       = d3dxad.Type;
			annot.Class      = d3dxad.Class;
			annot.Name       = d3dxad.Name;
			annot.cbDateSize = d3dxad.Bytes;
			annot.pData      = NULL;
			switch(d3dxad.Type)
			{
			case D3DXPT_STRING:
				{
					LPCSTR lpString;
					lpD3DXEffect->GetString(hAnnotation, &lpString);
					annot.cbDateSize = strlen(lpString) + 1;
					annot.pData = new char[annot.cbDateSize];
					strncpy((LPSTR)annot.pData, lpString, annot.cbDateSize);
				}
				break;
			case D3DXPT_INT:
				{
					if(d3dxad.Class == D3DXPC_SCALAR)
					{
						ASSERT(annot.cbDateSize == sizeof(int));
						lpD3DXEffect->GetInt(hAnnotation, &annot.nValue);
					}
					else if(d3dxad.Class == D3DXPC_VECTOR)
					{
						const UINT Count = annot.cbDateSize / sizeof(int);
						annot.pData = new int[Count];
						lpD3DXEffect->GetIntArray(hAnnotation, (INT*)annot.pData, Count);
					}
					else
					{
						// FIXME: 没实现
						ASSERT(0);
					}
				}
				break;
			case D3DXPT_FLOAT:
				{
					if(d3dxad.Class == D3DXPC_SCALAR)
					{
						ASSERT(annot.cbDateSize == sizeof(float));
						lpD3DXEffect->GetFloat(hAnnotation, &annot.fValue);
					}
					else
					{
						// FIXME: 没实现
						ASSERT(0);
					}
				}
				break;
			default:
				ASSERT(0);
				break;
			}
			Param.aAnnotation.push_back(annot);
		}
		(*m_pName2ParamDict)[d3dxpd.Name] = Param;
		//} // if(d3dxpd.Annotations > 0)

		if(d3dxpd.Type == D3DXPT_STRING && _stricmp(d3dxpd.Name, "entrynames") == 0)		// 解析入口函数
		{
			LPCSTR lpString;
			AStringArray aString;
			lpD3DXEffect->GetString(hHandle, &lpString);
			SplitString(lpString, ';', aString);
			for(AStringArray::iterator it = aString.begin();
				it != aString.end(); ++it)
			{
				it->TrimLeft(' ');
				it->TrimRight(' ');
				int nSpace = it->Find(' ', 0);
				if(nSpace < 0)
				{
					m_pHLSLMgr->AppendErrorMsg("error: entrynames 解析错误\r\n");
					return false;
				}
				AString strProfile = it->Mid(0, nSpace);
				strProfile.TrimLeft(' ');
				strProfile.TrimRight(' ');
				if(strProfile != "ps_2_0" && strProfile != "vs_2_0" &&
					strProfile != "vs_3_0" && strProfile != "ps_3_0" )
				{
					m_pHLSLMgr->AppendErrorMsg("error: entrynames 中包含不合法的profile名称.\r\n");
					return false;
				}
				strncpy(EntryName.szProfile, strProfile, 8);
				EntryName.strEntryName = it->Mid(nSpace + 1, -1);
				EntryName.strEntryName.TrimLeft(' ');
				EntryName.strEntryName.TrimRight(' ');
				EntryName.dwEntryID = a_MakeIDFromString(EntryName.strEntryName);

				// HLSL 加载逻辑:
				// 1.指定Shader为2.0, 如果硬件支持3.0, 则编译2.0和3.0版本,
				//   不支持3.0则只编译2.0版本. 此情况下列表中3.0版本在前以保证优先使用
				// 2.制定Shader为3.0, 如果硬件支持3.0, 则编译3.0版本, 否则不加载.
				// 3.SDRX 加载逻辑不走这个位置.
				const bool bSupportSdr3 = (EntryName.szProfile[0] == 'p' && bSupportPS30) ||
					(EntryName.szProfile[0] == 'v' && bSupportVS30);

				if(EntryName.szProfile[3] == '2')	// "vs_2_0" "ps_2_0"
				{
					// 检测如果支持ps/vs 3.0才编译 3.0 的 HLSL
					if(bSupportSdr3)
					{
						EntryName.szProfile[3] = '3';
						ASSERT(strcmp(EntryName.szProfile, "vs_3_0") == 0 ||
							strcmp(EntryName.szProfile, "ps_3_0") == 0);

						m_aEntryName.push_back(EntryName);
						EntryName.szProfile[3] = '2';
					}
					// 放在3.0后面保证3.0先被找到
					m_aEntryName.push_back(EntryName);
				}
				else
				{
					if(bSupportSdr3)
					{
						m_aEntryName.push_back(EntryName);
					}
				}

			}
		}
		else if(d3dxpd.Type == D3DXPT_STRING && d3dxpd.Annotations > 0)			// 解析宏的描述信息
		{
			Name2ParamDict::iterator itMacroAnno = m_pName2ParamDict->find(d3dxpd.Name);
			AnnotationArray& aAnno = itMacroAnno->second.aAnnotation;
			ASSERT(itMacroAnno != m_pName2ParamDict->end());
			MACROPROP MacroProp;
			MacroProp.wGroup = 0;
			MacroProp.wFlags = 0;
			//MacroProp.strMacroName = d3dxpd.Name;
			for(AnnotationArray::iterator itAnnoEle = aAnno.begin();
				itAnnoEle != aAnno.end(); ++itAnnoEle)
			{
				if(itAnnoEle->Type != D3DXPT_STRING || itAnnoEle->pData == NULL)
					continue;

				if(_stricmp(itAnnoEle->Name, "type") == 0)
				{
					AString strDescString = (const char*)itAnnoEle->pData;
					strDescString.MakeUpper();
					if(strstr(strDescString, "ART") == NULL)
					{
						MacroProp.strDesc = AString("*") + MacroProp.strDesc;
					}
					// entrynames 中指定的vs/ps版本同时会编译出更高版本的shader,
					// 例如指定vs/ps 2.0, 则同时也会编译vs/ps 3.0版本; 而如果指定vs/ps 3.0, 则只编译3.0版本 shader

					// 宏描述的"type" 中, 如果没有指定SDR版本,则默认在2.0和3.0都会编译
					// 如果指定了特定版本,则该宏只有在特定版本中有效

					const bool bSdr20 = strstr(strDescString,"SDR_2_0") != NULL;
					const bool bSdr30 = strstr(strDescString,"SDR_3_0") != NULL;

					if((bSdr20 && bSdr30) || ( ! bSdr20 && ! bSdr30))	// 都没指定的话就认为适用所有版本
					{
						MacroProp.wFlags |= (MPF_SHADER20 | MPF_SHADER30);
					}
					else if(bSdr20) // 设置针对指定的版本
					{
						MacroProp.wFlags |= MPF_SHADER20;
					}
					else if(bSdr30)
					{
						MacroProp.wFlags |= MPF_SHADER30;
					}
				}
				else if(_stricmp(itAnnoEle->Name, "name") == 0)
				{
					MacroProp.strMacroName = (const char*)itAnnoEle->pData;
				}
				else if(_stricmp(itAnnoEle->Name, "desc") == 0)
				{
					MacroProp.strDesc += (const char*)itAnnoEle->pData;
				}
				else if(_stricmp(itAnnoEle->Name, "groupid") == 0)
				{
					AString strGroupID = (const char*)itAnnoEle->pData;
					int nPos = 0, nStart = 0;
					int nNum;
					while((nPos = strGroupID.Find(',', nStart)) >= 0)  {
						if(nPos > nStart)  {
							nNum = strGroupID.Mid(nStart, nPos - nStart).ToInt();
							if(nNum < c_MaxMacroGroup)  {
								MacroProp.wGroup |= (1 << nNum);
							}
						}
						nStart = nPos + 1;
					}
					nNum = strGroupID.Mid(nStart, strGroupID.GetLength() - nStart).ToInt();
					if(nNum < c_MaxMacroGroup)  {
						MacroProp.wGroup |= (1 << nNum);
					}

				}
			} // for
			if(MacroProp.strMacroName.GetLength() > 0 && MacroProp.strDesc.GetLength() > 0 &&
				MacroProp.wGroup != NULL && (
				(( ! bShader30) && (MacroProp.wFlags & MPF_SHADER20) != 0) ||
				(    bShader30  && (MacroProp.wFlags & MPF_SHADER30) != 0)))  
			{
					m_aMacroProp.push_back(MacroProp);
			}
		} // else if

	}

	return true;
}

void A3DHLSLDataSrc::GenerateSamplerState()
{
	Name2ParamDict* pName2ParamDict = GetParamDict();
	if(pName2ParamDict == NULL) {
		return;
	}
	int nSampIdx = 0;

	for(Name2ParamDict::iterator it = pName2ParamDict->begin();
		it != pName2ParamDict->end(); ++it)
	{
		PARAMETER_DESC& p = it->second;
		if(p.Class == D3DXPC_OBJECT)
		{
			if(p.Type == D3DXPT_SAMPLER || p.Type == D3DXPT_SAMPLERCUBE || 
				p.Type == D3DXPT_SAMPLER2D || p.Type == D3DXPT_SAMPLER3D)
			{

				AnnotationArray& aAnno = p.aAnnotation;
				for(AnnotationArray::iterator itAnnoElement = aAnno.begin(); 
					itAnnoElement != aAnno.end(); ++itAnnoElement)
				{
					if(itAnnoElement->Type != D3DXPT_STRING)
					{
						continue;
					}
					AString strName = itAnnoElement->Name;
					AString strValue = (const char*)itAnnoElement->pData;
					m_aSampState[nSampIdx].strTextureName = strValue;
					m_aSampState[nSampIdx].strSamplerName = it->first;

					strName.MakeUpper();
					D3DSAMPLERSTATETYPE eType = D3DSAMP_FORCE_DWORD;
					DWORD Value = D3DTADDRESS_FORCE_DWORD;
					Text2EnumDict::iterator itEnum = A3DHLSLMan::s_pText2EnumDict->find(strName);
					if(itEnum == A3DHLSLMan::s_pText2EnumDict->end())
					{
						if(strName == "TEXTURE")
						{
							GetDefaultTexturePath(strValue, &m_aSampState[nSampIdx]);
						}
						continue;
					}

					eType = (D3DSAMPLERSTATETYPE)itEnum->second;
					strValue.MakeUpper();
					switch(eType)
					{
					case D3DSAMP_ADDRESSU:
					case D3DSAMP_ADDRESSV:
					case D3DSAMP_ADDRESSW:
						{
							itEnum = A3DHLSLMan::s_pText2EnumDict->find(strValue);
							if(itEnum == A3DHLSLMan::s_pText2EnumDict->end())
							{
								eType = D3DSAMP_FORCE_DWORD;
								break;
							}
							Value = itEnum->second;
							if(Value > D3DTADDRESS_MIRRORONCE)
							{
								eType = D3DSAMP_FORCE_DWORD;
							}
						}
						break;

						//case dwBORDERCOLOR:
						//	break;
					case D3DSAMP_MAGFILTER:
					case D3DSAMP_MINFILTER:
					case D3DSAMP_MIPFILTER:
						{
							itEnum = A3DHLSLMan::s_pText2EnumDict->find(strValue);
							if(itEnum == A3DHLSLMan::s_pText2EnumDict->end())
							{
								eType = D3DSAMP_FORCE_DWORD;
								break;
							}
							Value = itEnum->second;
							if(Value > D3DTEXF_CONVOLUTIONMONO)
							{
								eType = D3DSAMP_FORCE_DWORD;
							}
						}
						break;
						//case dwMIPMAPLODBIAS:
						//case dwMAXMIPLEVEL:
						//case dwMAXANISOTROPY:
						//case dwSRGBTEXTURE:
						//case dwELEMENTINDEX:
						//case dwDMAPOFFSET:
					default:
						eType = D3DSAMP_FORCE_DWORD;
						break;
					}
					if(eType != D3DSAMP_FORCE_DWORD) {
						m_aSampState[nSampIdx].SetState(eType, Value);
					}					
				}
				nSampIdx++;
			} // p.Type == SAMPLER_XXX
		} // if(p.Class == D3DXPC_OBJECT)
	}
}
void A3DHLSLDataSrc::GetDefaultTexturePath(const char* szTextureName, SAMPLER_STATE* lpSamplerState)
{
	Name2ParamDict* pName2ParamDict = GetParamDict();
	Name2ParamDict::iterator itAnno = pName2ParamDict->find(szTextureName);
	if(itAnno != pName2ParamDict->end())
	{
		AnnotationArray& aAnno = itAnno->second.aAnnotation;
		for(AnnotationArray::iterator itAnnoElement = aAnno.begin(); 
			itAnnoElement != aAnno.end(); ++itAnnoElement)
		{
			if(itAnnoElement->Name == "UIObject" && 
				itAnnoElement->Type == D3DXPT_STRING && 
				itAnnoElement->pData != NULL)
			{
				lpSamplerState->strDefault = (const char*)itAnnoElement->pData;
				lpSamplerState->strSemantic = itAnno->second.strSemantic;
				break;
			}
		}
	}
}

bool A3DHLSLDataSrc::CreateShader(SHADERINSTANCE& ShaderInst, const ENTRYNAME* pEntry)
{
	if(m_pA3DDevice == NULL) {	// 服务器的编译模式没有D3D设备
		return true;
	}

	LPDIRECT3DDEVICE9 pd3dDevice = m_pA3DDevice->GetD3DDevice();

	if(pEntry->szProfile[0] == 'v')  {
		if(FAILED(pd3dDevice->CreateVertexShader((DWORD*)ShaderInst.pShaderBuffer->GetBufferPointer(), &ShaderInst.pVShader)))
		{
			g_A3DErrLog.Log("Failed to create vertex shader.");
			ASSERT(0);
			return false;
		}
	}
	else if(pEntry->szProfile[0] == 'p')  {
		if(FAILED(pd3dDevice->CreatePixelShader((DWORD*)ShaderInst.pShaderBuffer->GetBufferPointer(), &ShaderInst.pPShader)))
		{
			g_A3DErrLog.Log("Failed to create vertex shader.");
			ASSERT(0);
			return false;
		}
	}
	else  {
		return false;
	}
	return true;
}

bool A3DHLSLDataSrc::IntLoadShaderInstMem(SHADERINSTANCE& ShaderInst, const BYTE* pShaderCode, size_t uCodeLen, const char* szFuncName, const char* szProfile, const MacroArray& aMacros, A3DHLSLDataSrcInclude* pInclude)
{
	LPD3DXBUFFER lpErrorBuffer = NULL;
	LPD3DXEFFECT lpD3DXEffect = NULL;
	LPD3DXBUFFER lpEffectErrorBuffer = NULL;
	//LPDIRECT3DDEVICE9 pd3dDevice = m_pA3DDevice->GetD3DDevice();
	bool bval = true;

	//Cleanup();
	// 检查
	ASSERT(ShaderInst.dwEntryID == NULL && 
		ShaderInst.pPShader == NULL && 
		ShaderInst.pVShader == NULL && 
		ShaderInst.pShaderBuffer == NULL &&
		ShaderInst.pConstants == NULL);
	const ENTRYNAME* pEntry = FindProfileByEntryName(szFuncName);
	if(pEntry == NULL)  {
		g_A3DErrLog.Log("%s. Failed to find shader's profile, maybe profile version is too high to load on the VIDEO CARD!", m_strName);
		return false;
	}
	szProfile = szProfile == NULL ? pEntry->szProfile : szProfile;
	ShaderInst.dwEntryID = pEntry->dwEntryID;	
	ShaderInst.dwProfile = ProfileStringToFourCC(szProfile);

	MacroArray aCanlizedMacros = aMacros;
	D3DXMacroArray aD3DMacros;
	D3DXMACRO SdrVerMacro = {NULL, NULL};

	// 根据Profile 的版本设置隐式宏
	if(szProfile[3] == '2') {
		SdrVerMacro.Name = "_SDR_2_0";
	}
	else if(szProfile[3] == '3') {
		SdrVerMacro.Name = "_SDR_3_0";
	}
	else {
		g_A3DErrLog.Log("Unsupported shader version.");
		ASSERT(0); // 不应该到这里,检查一下
	}

	//A3DHLSLMan::StringToMacroArray(szMacros, aMacros);
	// 规范化宏,主要是对宏进行字符串排序
	A3DHLSLMan::CanonicalizeMacroArray(aCanlizedMacros);
	A3DHLSLMan::BuildD3DMacro(aCanlizedMacros, aD3DMacros);
	A3DHLSLMan::MacroArrayToString(aCanlizedMacros, ShaderInst.strMacro);	// 更新ShaderInst里的记录

	// 因为是隐式宏,不会记录在ShaderInst里
	if(SdrVerMacro.Name != NULL)
	{
		// 最后一个是终止标记
		D3DXMACRO Back = aD3DMacros.back();
		aD3DMacros.back() = SdrVerMacro;
		aD3DMacros.push_back(Back);
	}

	//ASSERT(m_aConst.size() == 0);
	//ASSERT(m_NameSemaToConstDict.size() == 0);
	//ASSERT(m_pName2AnnoDict == NULL || m_pName2AnnoDict->size() == 0);
	if(pShaderCode != NULL && uCodeLen != 0)
	{
		if(FAILED(D3DXCompileShader((LPCSTR)pShaderCode, uCodeLen, &aD3DMacros.front(), pInclude, szFuncName, 
			szProfile, NULL, &ShaderInst.pShaderBuffer, &lpErrorBuffer, &ShaderInst.pConstants)) )
		{
			g_A3DErrLog.Log("Failed to compile shader(%s):\n%s\n", GetName(), lpErrorBuffer != NULL
				? lpErrorBuffer->GetBufferPointer()
				: "<empty>");
			m_pHLSLMgr->AppendErrorMsg("%s\r\n%s", GetName(), lpErrorBuffer != NULL
				? (const char*)lpErrorBuffer->GetBufferPointer()
				: "<empty>");
			bval = false;
		}
		SAFE_RELEASE(lpErrorBuffer);//MultiThread Render FIXME!!
	}

	if(bval)  {
		bval = CreateShader(ShaderInst, pEntry);
	}

	return bval;
}

int A3DHLSLDataSrc::ReadCodesFromFile(AFile* pShaderFile, BYTE** ppCodes)	// ppCodes要自己释放!
{
	const int nExtSize = sizeof(c_szFileDescDef) - 1 + m_strName.GetLength() + 3;	// (3) == \" 0x0d 0x0a
	DWORD dwSize = pShaderFile->GetFileLength();
	if(dwSize == 0)
		return 0;
	DWORD dwRead = 0;
	BYTE* pCodes = new BYTE[dwSize + nExtSize];
	memcpy(pCodes, c_szFileDescDef, sizeof(c_szFileDescDef) - 1);
	memcpy(pCodes + sizeof(c_szFileDescDef) - 1, (const char*)m_strName, m_strName.GetLength());
	pCodes[nExtSize - 3] = '\"';
	pCodes[nExtSize - 2] = 0x0d;
	pCodes[nExtSize - 1] = 0x0a;
	pShaderFile->Read(pCodes + nExtSize, dwSize, &dwRead);

	*ppCodes = pCodes;
	return dwSize + nExtSize;
}

bool A3DHLSLDataSrc::IntLoadShaderInst(SHADERINSTANCE& ShaderInst, AFile* pShaderFile, const char* szFuncName, const MacroArray& aMacros, A3DHLSLDataSrcInclude* pInclude)
{
	BYTE* pCodes = NULL;
	A3DHLSLDataSrcInclude* pMyInclude = NULL;

	if(pShaderFile == NULL)  {
		return false;
	}
	
	const int nTotalSize = ReadCodesFromFile(pShaderFile, &pCodes);

	char buffer[MAX_PATH];
	if(pInclude == NULL)
	{
		strncpy(buffer, pShaderFile->GetFileName(), MAX_PATH);
		PathRemoveFileSpecA(buffer);
		pMyInclude = new A3DHLSLDataSrcInclude(this);
		pMyInclude->Init();
		pMyInclude->SetDir(D3DXINC_LOCAL, buffer);
		pInclude = pMyInclude;
	}

	bool bval = IntLoadShaderInstMem(ShaderInst, pCodes, nTotalSize, szFuncName, NULL, aMacros, pInclude);

	A3DRELEASE(pMyInclude);
	delete pCodes;

	return bval;
}

bool A3DHLSLDataSrc::ReloadShaderInst()
{
	AFileImage File;

	if( ! File.Open(m_strName, AFILE_BINARY | AFILE_OPENEXIST | AFILE_TEMPMEMORY))  {
		g_A3DErrLog.Log("A3DHLSLDataSrc::Init, can not open shader file(%s).", m_strName);
		return false;
	}

	CleanupParameter();
	m_aMacroProp.clear();
	m_aEntryName.clear();
	m_NameSemanticDict.clear();

	bool ret = LoadHLSL(&File, A3DHLSLDATASRC_RELOAD);

	File.Close();
	return ret;
}

const A3DHLSLDataSrc::ENTRYNAME* A3DHLSLDataSrc::FindProfileByEntryName(const char* szEntryName)
{
	DWORD dwEntryID = a_MakeIDFromString(szEntryName);
	return FindProfileByEntryID(dwEntryID);
}

const A3DHLSLDataSrc::ENTRYNAME* A3DHLSLDataSrc::FindProfileByEntryID(DWORD dwEntryID)
{
	for(EntryNameArray::iterator it = m_aEntryName.begin();
		it != m_aEntryName.end(); ++it)  {
			if(it->dwEntryID == dwEntryID)  {
				return &*it;
			}
	}

	//if(dwEntryID == a_MakeIDFromString(g_szDefaultVSEntryName) && m_pA3DDevice->TestVertexShaderVersion(3,0))
	//intel G33/G31 等垃圾显卡不支持VS3.0但是返回true,所以这里VS版本以PS为准
	if(dwEntryID == a_MakeIDFromString(g_szDefaultVSEntryName) && m_pA3DDevice->TestPixelShaderVersion(3,0))
		return &s_DefVertexEntry;
	else if(dwEntryID == a_MakeIDFromString(g_szDefaultPSEntryName) && m_pA3DDevice->TestPixelShaderVersion(3,0))
		return &s_DefPixelEntry;
	return NULL;
}

const SHADERINSTANCE* A3DHLSLDataSrc::FindShaderInst(const char* szFuncName, const MacroArray& aMacros)
{
	DWORD dwEntryID = a_MakeIDFromString(szFuncName);
	AString strMacros;
	MacroArray aCanlizeMacros = aMacros;

	//A3DHLSLMan::StringToMacroArray(szMacros, aMacros);
	A3DHLSLMan::CanonicalizeMacroArray(aCanlizeMacros);
	A3DHLSLMan::MacroArrayToString(aCanlizeMacros, strMacros);

	for(ShaderInstArray::iterator it = m_aInstance.begin();
		it != m_aInstance.end(); ++it)
	{
		const SHADERINSTANCE* pShaderInst = *it;
		if(pShaderInst->dwEntryID == dwEntryID && pShaderInst->strMacro == strMacros)
		{
			return pShaderInst;
		}
	}
	return NULL;
}

const SHADERINSTANCE* A3DHLSLDataSrc::LoadShaderInst(const char* szFuncName, const MacroArray& aMacros)
{
	AFileImage File;
	if(m_strName.GetLength() == 0)  {
		return false;
	}

	const SHADERINSTANCE* pShaderInst = FindShaderInst(szFuncName, aMacros);
	if(pShaderInst != NULL)  {
		return pShaderInst;
	}

	if(File.Open(m_strName, AFILE_BINARY | AFILE_OPENEXIST | AFILE_TEMPMEMORY) == false)  {
		g_A3DErrLog.Log("A3DHLSLDataSrc::LoadShaderInst, can not open shader file(%s).", m_strName);
		return false;
	}

	SHADERINSTANCE* pShaderInstNew = new SHADERINSTANCE;
	bool bval = IntLoadShaderInst(*pShaderInstNew, &File, szFuncName, aMacros, NULL);
	if(bval)  {
		ASSERT(pShaderInstNew->dwEntryID != NULL);
		ASSERT(pShaderInstNew->pShaderBuffer != NULL);
		ASSERT(pShaderInstNew->dwProfile != NULL);
		m_aInstance.push_back(pShaderInstNew);
		return m_aInstance.back();
	}
	else {
		delete pShaderInstNew;
		pShaderInstNew = NULL;
	}

	File.Close();

	return NULL;
}

const SHADERINSTANCE* A3DHLSLDataSrc::LoadShaderInst(const char* szFuncName, const char* szMacros)
{
	AStringArray aMacros;
	AStringArray aProvedMacros;
	MacroArray aShaderMacros;
	SplitString(szMacros, ';', aMacros);
	ProvingMacro(aMacros, aProvedMacros, NULL);
	A3DHLSLMan::StringArrayToMacroArray(aProvedMacros, aShaderMacros);
	return LoadShaderInst(szFuncName, aShaderMacros);
}

bool A3DHLSLDataSrc::LoadHLSL(AFile* pShaderFile, DWORD dwFlags)
{
	BYTE* pCodes = NULL;
	A3DHLSLDataSrcInclude* pMyInclude = NULL;

	if(pShaderFile == NULL)  {
		return false;
	}

	//pCodes = new BYTE[dwSize];
	//pShaderFile->Read(pCodes, dwSize, &dwRead);
	int nTotalSize = ReadCodesFromFile(pShaderFile, &pCodes);
	if(nTotalSize == 0)  {
		return false;
	}

	char buffer[MAX_PATH];
	strncpy(buffer, pShaderFile->GetFileName(), MAX_PATH);
	PathRemoveFileSpecA(buffer);
	pMyInclude = new A3DHLSLDataSrcInclude(this);
	pMyInclude->Init();
	pMyInclude->SetDir(D3DXINC_LOCAL, buffer);

	//bool bval = LoadFromMemory(pCodes, dwSize, pMacro, pInclude, szFuncName);

	bool bval = BuildEffect(pCodes, nTotalSize, pMyInclude, dwFlags);
	if(bval)  {
		if(dwFlags & A3DHLSLDATASRC_ASMALLINST)  {
			bval = AssembleAllInst(pCodes, nTotalSize, pMyInclude);
		}
		else if(dwFlags & A3DHLSLDATASRC_RELOAD)  {

			DWORD dwRead;
//			DWORD dwSize;
			// 加载一个标识错误的shader代码
			AFileImage fileError;
			BYTE* pErrorCodes = NULL;
			DWORD dwErrorSize = 0;
			if(fileError.Open(ERROR_MTL_FILE, AFILE_BINARY | AFILE_OPENEXIST | AFILE_TEMPMEMORY))
			{
				dwErrorSize = fileError.GetFileLength();
				pErrorCodes = new BYTE[dwErrorSize];
				fileError.Read(pErrorCodes, dwErrorSize, &dwRead);
			}
			
			bval = ReloadAllInst(pCodes, nTotalSize, pErrorCodes, dwErrorSize, pMyInclude);

			if(pErrorCodes != NULL)
			{
				delete[]pErrorCodes;
				pErrorCodes = NULL;
			}
		}
	}

	A3DRELEASE(pMyInclude);
	delete pCodes;
	return bval;
}

bool A3DHLSLDataSrc::InitFromHLSL(DWORD dwFlags)
{
	AFileImage File;

	if( ! File.Open(m_strName, AFILE_BINARY | AFILE_OPENEXIST | AFILE_TEMPMEMORY))  {
		g_A3DErrLog.Log("A3DHLSLDataSrc::Init, can not open shader file(%s).", m_strName);
		m_pHLSLMgr->AppendErrorMsg("Error: Can not open shader file(%s).\r\n", m_strName);
		return false;
	}
	return LoadHLSL(&File, dwFlags);
}

bool A3DHLSLDataSrc::InitFromObj()
{
	AString strObjFilename;
	GenCompiledFilename(m_strName, strObjFilename);
	if( ! af_IsFileExist(strObjFilename))  {
		return false;
	}

	AFileImage File;
	if( ! File.Open(strObjFilename, AFILE_BINARY|AFILE_OPENEXIST | AFILE_TEMPMEMORY))  {
		g_A3DErrLog.Log("A3DHLSLDataSrc::InitFromObj, can not open the file specified(%s).", strObjFilename);
		return false;
	}
	return Load(&File);
}

bool A3DHLSLDataSrc::AssembleAllInst(const BYTE* pShaderCode, size_t uCodeLen, LPD3DXINCLUDE lpInclude)
{
	ASSERT(m_aInstance.size() == 0);
	typedef abase::hash_map<AString, int>	AsmCompileDict;
	AsmCompileDict l_AsmCompileDict;
	DWORD dwShaderMask = 0;
	bool bval = true;

	if(m_aEntryName.size() == 0)  {
		m_pHLSLMgr->AppendErrorMsg("Error: need specify entryname in hlsl, write out all of entryname you want to compiled!\r\n");
		return false;
	}

	// 装配循环,最外层是入口点的循环
	for(EntryNameArray::iterator itEntry = m_aEntryName.begin();
		itEntry != m_aEntryName.end(); ++itEntry)
	{
		if(itEntry->strEntryName == g_szDefaultVSEntryName)  {
			dwShaderMask |= 1;
		}
		else if(itEntry->strEntryName == g_szDefaultPSEntryName)  {
			dwShaderMask |= 2;
		}
		// 决定宏适用的版本
		DWORD dwProfileVer = 0;
		if(itEntry->szProfile[3] == '2')	// "vs_2_0" "ps_2_0"
		{
			dwProfileVer = MPF_SHADER20;
		}
		else if(itEntry->szProfile[3] == '3')	// "vs_3_0" "ps_3_0"
		{
			dwProfileVer = MPF_SHADER30;
		}
		unsigned int uAsmCount = 1 << m_aMacroProp.size();
		MacroArray aMacros;
		A3DSHDMACRO ShdMacro;
		AString strMacros;
		for(unsigned int i = 0; i < uAsmCount; i++)
		{
			strMacros.Empty();
			aMacros.clear();
			int n = 0;
			DWORD dwMask = NULL;
			for(MacroPropArray::iterator itMacro = m_aMacroProp.begin();
				itMacro != m_aMacroProp.end(); ++itMacro, n++)
			{
				if((i & (1 << n)) && ((dwMask & itMacro->wGroup) == 0) && // 合适的宏组合
					(itMacro->wFlags & dwProfileVer))	// 版本号符合
				{
					dwMask |= itMacro->wGroup;

					ShdMacro.Name = itMacro->strMacroName;
					ShdMacro.Definition.Empty();
					aMacros.push_back(ShdMacro);

					strMacros += itMacro->strMacroName + ';';
				}
			}
			// 这里的宏序列是未经规范化的,所以这里自己建立字典查询是否重复
			AString strAssembly = itEntry->strEntryName + '|' + 
				itEntry->szProfile + '|' + strMacros;
			AsmCompileDict::iterator itResult = l_AsmCompileDict.find(strAssembly);
			if(itResult != l_AsmCompileDict.end())  {
				continue;
			}
			l_AsmCompileDict[strAssembly] = 1;

			SHADERINSTANCE* pShaderInst = new SHADERINSTANCE;
			m_pHLSLMgr->AppendErrorMsg("编译组合(%s):%s, %s\r\n", itEntry->szProfile, itEntry->strEntryName, strMacros);
			bool l_bval = IntLoadShaderInstMem(*pShaderInst, pShaderCode, uCodeLen, itEntry->strEntryName, itEntry->szProfile, aMacros, (A3DHLSLDataSrcInclude*)lpInclude);

			ASSERT(pShaderInst->dwProfile != NULL);
			bval = bval && l_bval;

			if(l_bval)  {
				m_aInstance.push_back(pShaderInst);
			}
			else {
				delete pShaderInst;
				pShaderInst = NULL;
			}
		}
	}
	if(dwShaderMask == 0)  {
		m_pHLSLMgr->AppendErrorMsg("Warning: the HLSL does not specify \"vs_main\" or \"ps_main\", forget it?\r\n");
	}
	return bval;
}

bool A3DHLSLDataSrc::ReloadAllInst(const BYTE* pShaderCode, size_t uCodeLen, const BYTE* pErrorShaderCode, size_t uErrorCodeLen, LPD3DXINCLUDE lpInclude)
{
	bool bval = true;
	for(ShaderInstArray::iterator it = m_aInstance.begin();
		it != m_aInstance.end(); ++it)
	{
		SHADERINSTANCE* pShaderInst = *it;
		DWORD dwEntryID = pShaderInst->dwEntryID;
		if( ! m_pA3DDevice)
		{
			SAFE_RELEASE(pShaderInst->pConstants);
			SAFE_RELEASE(pShaderInst->pPShader);
			SAFE_RELEASE(pShaderInst->pShaderBuffer);
			SAFE_RELEASE(pShaderInst->pVShader);
		}
		else		
		{
			m_pA3DDevice->ReleaseResource(pShaderInst->pConstants); //multiThread Render
			m_pA3DDevice->ReleaseResource(pShaderInst->pPShader); 
			m_pA3DDevice->ReleaseResource(pShaderInst->pShaderBuffer);
			m_pA3DDevice->ReleaseResource(pShaderInst->pVShader);
		}
		pShaderInst->pConstants = NULL;
		pShaderInst->pPShader = NULL;
		pShaderInst->pShaderBuffer = NULL;
		pShaderInst->pVShader = NULL;

		pShaderInst->dwEntryID = NULL;

		const ENTRYNAME* pEntry = FindProfileByEntryID(dwEntryID);

		MacroArray aMacros;
		A3DHLSLMan::StringToMacroArray(pShaderInst->strMacro, aMacros);

		char szProfile[16];
		ProfileFourCCToString(pShaderInst->dwProfile, szProfile);
		bool l_bval = IntLoadShaderInstMem(*pShaderInst, pShaderCode, uCodeLen, pEntry->strEntryName, szProfile,
			aMacros, (A3DHLSLDataSrcInclude*)lpInclude);

		// 如果shander加载错误,则使用替代的shader加载
		if(( ! l_bval) && szProfile[0] == 'p') {
			pShaderInst->dwEntryID = NULL;
			IntLoadShaderInstMem(*pShaderInst, pErrorShaderCode, uErrorCodeLen, "ps_main", szProfile,
				aMacros, (A3DHLSLDataSrcInclude*)lpInclude);
		}

		bval = bval && l_bval;
		ASSERT(pShaderInst->dwEntryID != NULL);
	}
	return bval;
}

bool A3DHLSLDataSrc::Save(AFile* pFile)
{
	DWORD dwWrite;
	A3DHLSLDATASRCFILEHEADER Header;
	Header.cbSize      = sizeof(A3DHLSLDATASRCFILEHEADER);
	Header.dwVersion   = HLSLDATASRC_VERSION_001;
	Header.nShaderInst = m_aInstance.size();
	Header.nMacro      = m_aMacroProp.size();
	Header.nNameSema   = m_NameSemanticDict.size();
	Header.nEntryName  = m_aEntryName.size();

	pFile->Write(&Header, sizeof(A3DHLSLDATASRCFILEHEADER), &dwWrite);

	// Shader 实例
	for(ShaderInstArray::iterator it = m_aInstance.begin();
		it != m_aInstance.end(); ++it)
	{
		SHADERINSTHEADER ShaderInstHeader;
		SHADERINSTANCE* pShaderInst = *it;
		ShaderInstHeader.dwEntryID = pShaderInst->dwEntryID;
		ShaderInstHeader.dwProfile = pShaderInst->dwProfile;
		ShaderInstHeader.dwShaderSize = pShaderInst->pShaderBuffer->GetBufferSize();

		pFile->Write(&ShaderInstHeader, sizeof(ShaderInstHeader), &dwWrite);
		pFile->WriteString(pShaderInst->strMacro);
#ifdef _DEBUG
		if(m_pA3DDevice != NULL)
		{
			ASSERT((pShaderInst->pPShader == NULL && pShaderInst->pVShader != NULL) ||
				(pShaderInst->pPShader != NULL && pShaderInst->pVShader == NULL));
		}
#endif // _DEBUG
		pFile->Write(pShaderInst->pShaderBuffer->GetBufferPointer(), pShaderInst->pShaderBuffer->GetBufferSize(), &dwWrite);
	}

	// 宏信息
	for(MacroPropArray::iterator it = m_aMacroProp.begin();
		it != m_aMacroProp.end(); ++it)
	{
		pFile->WriteString(it->strMacroName);
		pFile->WriteString(it->strDesc);
		pFile->Write(&it->dwData, sizeof(DWORD), &dwWrite);
	}

	// 语意和常量名的对应
	for(NameSemanticDict::iterator it = m_NameSemanticDict.begin();
		it != m_NameSemanticDict.end(); ++it)
	{
		pFile->WriteString(it->first);
		pFile->WriteString(it->second);
	}

	// 入口函数
	for(EntryNameArray::iterator it = m_aEntryName.begin();
		it != m_aEntryName.end(); ++it)
	{
		pFile->Write(&it->szProfile, sizeof(it->szProfile), &dwWrite);
		ASSERT(dwWrite == 8);
		pFile->WriteString(it->strEntryName);
	}

	// 采样器
	for(int i = 0; i < MAX_SAMPLERCOUNT; i++)
	{
		pFile->WriteString(m_aSampState[i].strSamplerName);
		pFile->WriteString(m_aSampState[i].strTextureName);
		pFile->WriteString(m_aSampState[i].strSemantic);
		pFile->WriteString(m_aSampState[i].strDefault);
		pFile->Write(&m_aSampState[i].SampStateMask, sizeof(m_aSampState[i].SampStateMask), &dwWrite);
		pFile->Write(&m_aSampState[i].SampState, sizeof(m_aSampState[i].SampState), &dwWrite);
		ASSERT(dwWrite == sizeof(DWORD) * 13);
	}
	return true;
}

bool A3DHLSLDataSrc::Load(AFile* pFile)
{
	DWORD dwRead;
	A3DHLSLDATASRCFILEHEADER Header;

	//const bool bSupportVS30 = (m_pA3DDevice == NULL) ? true : m_pA3DDevice->TestVertexShaderVersion(3, 0);
	//intel G33/G31 等垃圾显卡不支持VS3.0但是返回true,所以这里VS版本以PS为准
	const bool bSupportVS30 = (m_pA3DDevice == NULL) ? true : m_pA3DDevice->TestPixelShaderVersion(3, 0);
	const DWORD dwHWVSVer = bSupportVS30 ? MAKEFOURCC('v','s','3','0') : MAKEFOURCC('v','s','2','0');

	const bool bSupportPS30 = (m_pA3DDevice == NULL) ? true : m_pA3DDevice->TestPixelShaderVersion(3, 0);
	const DWORD dwHWPSVer = bSupportPS30 ? MAKEFOURCC('p','s','3','0') : MAKEFOURCC('p','s','2','0');

	// 检测文件头和文件版本
	pFile->Read(&Header.cbSize, sizeof(Header.cbSize), &dwRead);
	if(dwRead != 4 || Header.cbSize != sizeof(Header))  {
		return false;
	}
	pFile->Read(((BYTE*)&Header.cbSize) + sizeof(Header.cbSize), Header.cbSize - sizeof(Header.cbSize), &dwRead);
	if(dwRead != Header.cbSize - sizeof(Header.cbSize))  {
		return false;
	}
	if(Header.dwVersion != HLSLDATASRC_VERSION_001)  {
		return false;
	}

	ASSERT(
		m_aInstance.size() == 0 &&
		m_aMacroProp.size() == 0 &&
		m_NameSemanticDict.size() == 0 &&
		m_aEntryName.size() == 0);

	// Shader 实例
	for(size_t i = 0; i < Header.nShaderInst; i++)
	{
		SHADERINSTHEADER ShaderInstHeader;
		SHADERINSTANCE* pShaderInst = new SHADERINSTANCE;
		pFile->Read(&ShaderInstHeader, sizeof(ShaderInstHeader), &dwRead);
		pFile->ReadString(pShaderInst->strMacro);
		pShaderInst->dwEntryID = ShaderInstHeader.dwEntryID;
		pShaderInst->dwProfile = ShaderInstHeader.dwProfile;
		pShaderInst->pPShader  = NULL;
		pShaderInst->pVShader  = NULL;
		D3DXCreateBuffer(ShaderInstHeader.dwShaderSize, &pShaderInst->pShaderBuffer);
		pFile->Read(pShaderInst->pShaderBuffer->GetBufferPointer(), ShaderInstHeader.dwShaderSize, &dwRead);
		D3DXGetShaderConstantTable((DWORD*)pShaderInst->pShaderBuffer->GetBufferPointer(), &pShaderInst->pConstants);
		if(pShaderInst->dwProfile == dwHWVSVer || pShaderInst->dwProfile == dwHWPSVer)
		{
			m_aInstance.push_back(pShaderInst);
		}
		else {
			delete pShaderInst;
		}
	}

	// 宏信息
	const bool bShader30 = bSupportPS30 || bSupportVS30;
	for(size_t i = 0; i < Header.nMacro; i++)
	{
		MACROPROP MacroProp;
		pFile->ReadString(MacroProp.strMacroName);
		pFile->ReadString(MacroProp.strDesc);
		pFile->Read(&MacroProp.dwData, sizeof(DWORD), &dwRead);
		if((( ! bShader30) && (MacroProp.wFlags & MPF_SHADER20) == 0) ||
			(bShader30 && (MacroProp.wFlags & MPF_SHADER30) == 0))
		{
			continue;
		}
		m_aMacroProp.push_back(MacroProp);
	}

	// 语意和常量名的对应
	for(size_t i = 0; i < Header.nNameSema; i++)
	{
		//NAMESEMANTICPAIR NameSemaPair;
		AString strName;
		AString strSemantic;
		pFile->ReadString(strName);
		pFile->ReadString(strSemantic);
		m_NameSemanticDict[strName] = strSemantic;
	}

	// 入口函数
	for(size_t i = 0; i < Header.nEntryName; i++)
	{
		ENTRYNAME EntryName;
		pFile->Read(&EntryName.szProfile, sizeof(EntryName.szProfile), &dwRead);
		ASSERT(dwRead == 8);
		pFile->ReadString(EntryName.strEntryName);
		EntryName.dwEntryID = a_MakeIDFromString(EntryName.strEntryName);

		DWORD dwProfile = ProfileStringToFourCC(EntryName.szProfile);
		if(dwProfile == dwHWVSVer || dwProfile == dwHWPSVer) {
			m_aEntryName.push_back(EntryName);
		}
	}

	// 采样器
	for(int i = 0; i < MAX_SAMPLERCOUNT; i++)
	{
		pFile->ReadString(m_aSampState[i].strSamplerName);
		pFile->ReadString(m_aSampState[i].strTextureName);
		pFile->ReadString(m_aSampState[i].strSemantic);
		pFile->ReadString(m_aSampState[i].strDefault);
		pFile->Read(&m_aSampState[i].SampStateMask, sizeof(m_aSampState[i].SampStateMask), &dwRead);
		pFile->Read(&m_aSampState[i].SampState, sizeof(m_aSampState[i].SampState), &dwRead);
		ASSERT(dwRead == sizeof(DWORD) * 13);
	}

	// 之前因为入口函数名描述表没有载入,所以暂时不能创建Shader
	// 这里已经准备好了
	for(ShaderInstArray::iterator it = m_aInstance.begin();
		it != m_aInstance.end(); ++it)
	{
		const ENTRYNAME* pEntryName = FindProfileByEntryID((*it)->dwEntryID);
		CreateShader(*(*it), pEntryName);
	}
	return true;
}

bool A3DHLSLDataSrc::ProvingMacro(const AStringArray& aMacros, AStringArray& aOutMacros, AStringArray* pMutexMacros)
{
	DWORD dwMask = NULL;
	bool bval = true;
	for(AStringArray::const_iterator it = aMacros.begin();
		it != aMacros.end(); ++it)
	{
		if(it->GetLength() == 0) {
			continue;
		}

		const MACROPROP* pMacroProp = FindMacro(*it);
		if(pMacroProp != NULL)
		{
			if((dwMask & pMacroProp->wGroup) == 0)
			{
				dwMask |= pMacroProp->wGroup;
				aOutMacros.push_back(*it);
			}
			else if(pMutexMacros != NULL)  {
				pMutexMacros->push_back(*it);
			}
		}
		else{
			bval = false;	// 如果队列里有不存在的宏,则最后返回false
		}
	}
	return bval;
}

const A3DHLSLDataSrc::MACROPROP* A3DHLSLDataSrc::FindMacro(const char* szMacro)
{
	for(MacroPropArray::iterator it = m_aMacroProp.begin();
		it != m_aMacroProp.end(); ++it)
	{
		if(it->strMacroName == szMacro)  {
			return &*it;
		}
	}
	return NULL;
}

bool A3DHLSLDataSrc::GenCompiledFilename(const char* szShaderFile, AString& strFilename)
{
	strFilename = szShaderFile;
	strFilename.MakeLower();
	int nPos = strFilename.Find("shaders\\2.2\\hlsl\\", 0);
	if(nPos < 0) {
		strFilename.Empty();
		return false;
	}
	strFilename[nPos + 12] = 's';
	strFilename[nPos + 13] = 'd';
	strFilename[nPos + 14] = 'r';
	strFilename[nPos + 15] = 'x';

	af_ChangeFileExt(strFilename, ".obj");
	return true;
}

DWORD A3DHLSLDataSrc::ProfileStringToFourCC(const char* szProfile)
{
	if(strcmp(szProfile, "ps_2_0") == 0)  {
		return MAKEFOURCC('p','s','2','0');
	}
	else if(strcmp(szProfile, "vs_2_0") == 0)  {
		return MAKEFOURCC('v','s','2','0');
	}
	else if(strcmp(szProfile, "ps_3_0") == 0)  {
		return MAKEFOURCC('p','s','3','0');
	}
	else if(strcmp(szProfile, "vs_3_0") == 0)  {
		return MAKEFOURCC('v','s','3','0');
	}
	return 0;
}

bool A3DHLSLDataSrc::ProfileFourCCToString(DWORD dwCode, char* szProfile)
{
	switch(dwCode)
	{
	case MAKEFOURCC('p','s','2','0'):
		memcpy(szProfile, "ps_2_0\0\0", 8);
		break;
	case MAKEFOURCC('v','s','2','0'):
		memcpy(szProfile, "vs_2_0\0\0", 8);
		break;
	case MAKEFOURCC('p','s','3','0'):
		memcpy(szProfile, "ps_3_0\0\0", 8);
		break;
	case MAKEFOURCC('v','s','3','0'):
		memcpy(szProfile, "vs_3_0\0\0", 8);
		break;
	default:
		return false;
	}
	return true;
}


//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
