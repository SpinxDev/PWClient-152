#include "A3D.h"
#include "A3DHLSL.h"
#include "A3DPI.h"
#include "AFI.h"
#include "AFilePackage.h"
#include "AFileImage.h"
#include "A3DFuncs.h"
#include "A3DHLSLDataSrc.H"
#include "A3DEnvironment.h"
#include <Shlwapi.h>

#define HLSLCORE_VERSION_001	0x10000001
#define SAFE_RELEASE(p)	if((p) != NULL){ (p)->Release(); p = NULL; }
#define CACHEDIR "Shaders\\2.2\\HLSL"
const char* szCachePath = CACHEDIR"\\HLSL.Cache";

char g_szDefaultVSEntryName[] = "vs_main";
char g_szDefaultPSEntryName[] = "ps_main";

Text2EnumDict* A3DHLSLMan::s_pText2EnumDict = NULL;

//// BuildConstDict 的标志位
//#define BCD_ADDCONSTNAME      0x00000001
//#define BCD_ADDSEMANTIC       0x00000002
//#define BCD_CLEARBEFOREBUILD  0x00000100
//#define BCD_ALL               (BCD_CLEARBEFOREBUILD | BCD_ADDCONSTNAME | BCD_ADDSEMANTIC)

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

struct HLSLCOREHEADER
{
	DWORD cbSize;
	DWORD dwVersion;
	DWORD dwID;
	DWORD cbVSFunc;
	DWORD cbPSFunc;
	DWORD nSemaCount;
	DWORD nMaxSamplerUsed; // m_nMaxSamplerIdx
};

struct HLSLMANHEADER
{
	DWORD cbSize;
	DWORD dwShaderCount;
	DWORD dwRefFileCount;
};
//////////////////////////////////////////////////////////////////////////
A3DHLSLCore::A3DHLSLCore(A3DDevice* pA3DDevice, A3DHLSLMan* pHLSLMgr, A3DHLSLDataSrc* pVSDataSrc, A3DHLSLDataSrc* pPSDataSrc)
: m_pDevice				(pA3DDevice)
, m_pHLSLMgr			(pHLSLMgr)
, m_nRefCount			(NULL)
, m_dwID				(NULL)
, m_pVSDataSource		(pVSDataSrc)
, m_pPSDataSource		(pPSDataSrc)
, m_nMaxSamplerIdx		(0)
, m_pVertexShaderInst	(NULL)
, m_pPixelShaderInst	(NULL)
{
	m_dwClassID = A3D_CID_HLSLCORE;
	memset(m_aSampler, 0, sizeof(m_aSampler));

	if(m_pVSDataSource != NULL) {
		m_pVSDataSource->AddRef();
	}
	if(m_pPSDataSource != NULL) {
		m_pPSDataSource->AddRef();
	}
}

A3DHLSLCore::~A3DHLSLCore()
{
}

bool A3DHLSLCore::Init()
{
	return true;
}

int A3DHLSLCore::AddRef()
{
	return ++m_nRefCount;
}

int A3DHLSLCore::Release()
{
	ASSERT(m_nRefCount >= 0);
	if(m_nRefCount == 0)
		return 0;
	m_nRefCount--;
	if(m_nRefCount == 0)
	{
		Cleanup();
		SAFE_RELEASE(m_pVSDataSource);
		SAFE_RELEASE(m_pPSDataSource);
		return 0;
	}
	return m_nRefCount;
}

void A3DHLSLCore::Cleanup()
{
	m_pVertexShaderInst = NULL;
	m_pPixelShaderInst  = NULL;
	m_nMaxSamplerIdx    = 0;

	m_NameSemaToConstDict.clear();
	m_aConst.clear();
	memset(m_aSampler, 0, sizeof(m_aSampler));
}

bool A3DHLSLCore::LoadCore(const char* szVSFuncName, const char* szPSFuncName, const char* szMacro)
{
	Cleanup();

	if(szVSFuncName == NULL)  {
		szVSFuncName = g_szDefaultVSEntryName;
	}

	if(szPSFuncName == NULL)  {
		szPSFuncName = g_szDefaultPSEntryName;
	}

	if(m_pVSDataSource == NULL && m_pPSDataSource == NULL)
	{
		return false;
	}

	if(m_pVSDataSource != NULL)
	{
		m_pVertexShaderInst = m_pVSDataSource->LoadShaderInst(szVSFuncName, szMacro);
		if(m_pVertexShaderInst == NULL)  {
			return false;
		}
	}
	
	if(m_pPSDataSource != NULL)
	{
		m_pPixelShaderInst = m_pPSDataSource->LoadShaderInst(szPSFuncName, szMacro);
		if(m_pPixelShaderInst == NULL)  {
			return false;
		}
	}

	return Rebuild();
}

void A3DHLSLCore::BuildConstDict(/*DWORD dwFlags*/)
{
	//if(dwFlags & BCD_CLEARBEFOREBUILD) {
		m_NameSemaToConstDict.clear();
	//}

	for(ConstArray::iterator itArray = m_aConst.begin();
		itArray != m_aConst.end(); ++itArray)
	{
		//if(dwFlags & BCD_ADDCONSTNAME)
		//{
			ConstDict::iterator itDict = m_NameSemaToConstDict.find(itArray->cd.Name);
			if(itDict != m_NameSemaToConstDict.end())
			{
				itDict->second->bUnique = false;
				itArray->bUnique = false;
				m_NameSemaToConstDict[AString("@") + itArray->cd.Name] = &(*itArray);
			}
			else  {
				m_NameSemaToConstDict[itArray->cd.Name] = &(*itArray);
			}
		//}
		//if(dwFlags & BCD_ADDSEMANTIC)
		//{
			if(itArray->Semantic.GetLength() > 0)
			{
				m_NameSemaToConstDict[itArray->Semantic] = &(*itArray);
			}
		//}
	}
}
void A3DHLSLCore::GetDefaultTexturePath(const char* szTextureName, AString& strPath)
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
				strPath = (const char*)itAnnoElement->pData;
				break;
			}
		}
	}
}
void A3DHLSLCore::GenerateSamplerState()
{
	LPCSAMPLER_STATE pSampState = m_pPSDataSource->GetSamplerStateArray();

	for(ConstArray::iterator it = m_aConst.begin();
		it != m_aConst.end(); ++it)
	{
		if(it->cd.Class == D3DXPC_OBJECT)
		{
			ASSERT(it->cd.Type == D3DXPT_SAMPLER || it->cd.Type == D3DXPT_SAMPLERCUBE || 
				it->cd.Type == D3DXPT_SAMPLER2D || it->cd.Type == D3DXPT_SAMPLER3D);
			for(int i = 0; i < MAX_SAMPLERCOUNT; i++)  {
				if(pSampState[i].strSamplerName.GetLength() == 0)  {
					break;
				}
				if(pSampState[i].strSamplerName == it->cd.Name)  {
					m_nMaxSamplerIdx = a_Max(m_nMaxSamplerIdx, it->cd.RegisterIndex);
					m_aSampler[it->cd.RegisterIndex] = i;
					break;
				}
			}
		}
	}
}

int A3DHLSLCore::GetSemaCount()		// 得到有Semantic的const数量
{
	int nSema = 0;
	for(ConstArray::iterator it = m_aConst.begin();
		it != m_aConst.end(); ++it)
	{
		if(it->Semantic.GetLength() > 0)
		{
			nSema++;
		}
	}
	return nSema;
}

//void A3DHLSLCore::Transfer(A3DHLSLCore& Src)
//{
//	m_pDevice          = Src.m_pDevice;
//	m_nRefCount        = Src.m_nRefCount;
//	m_dwID             = Src.m_dwID;
//	m_pHLSLMgr         = Src.m_pHLSLMgr;
//	//m_pVertexShader    = Src.m_pVertexShader;
//	//m_pPixelShader     = Src.m_pPixelShader;
//	//m_pVertexConstants = Src.m_pVertexConstants;
//	//m_pPixelConstants  = Src.m_pPixelConstants;
//	//m_lpVShaderFunc    = Src.m_lpVShaderFunc;
//	//m_lpPShaderFunc    = Src.m_lpPShaderFunc;
//	//m_pName2AnnoDict   = Src.m_pName2AnnoDict;
//	m_aConst           = Src.m_aConst;
//	m_nMaxSamplerIdx   = Src.m_nMaxSamplerIdx;
//	m_strVSFuncName    = Src.m_strVSFuncName;
//	m_strPSFuncName    = Src.m_strPSFuncName;
//	//m_aMacroProp       = Src.m_aMacroProp;
//
//
//	//for(int i = 0; i < MAX_SAMPLERCOUNT; i++)
//	//{
//	//	m_aSampState[i] = Src.m_aSampState[i];
//	//	Src.m_aSampState[i].Cleanup();
//	//}
//	m_NameSemaToConstDict.clear();
//}

bool A3DHLSLCore::Rebuild()
{
	m_aConst.clear();
	m_NameSemaToConstDict.clear();

	if(m_pVertexShaderInst != NULL)  {
		GenerateConstTable(m_pVSDataSource, m_pVertexShaderInst, true);
	}
	if(m_pPixelShaderInst != NULL)  {
		GenerateConstTable(m_pPSDataSource, m_pPixelShaderInst, false);
	}

	BuildConstDict(/*BCD_ALL*/);
	GenerateSamplerState();

	return true;
}

void A3DHLSLCore::IntGetApproximately(const SHADERINSTANCE* pShaderInst, int* nTotalInst, int* nTexInst, int* nAriInst)
{
	LPD3DXBUFFER pBuffer = NULL;
	if(pShaderInst == NULL || pShaderInst->pShaderBuffer == NULL)
	{
		*nTotalInst = 0;
		*nTexInst = 0;
		*nAriInst = 0;
		return;
	}
	if(SUCCEEDED(D3DXDisassembleShader((const DWORD*)pShaderInst->pShaderBuffer->GetBufferPointer(), FALSE, FALSE, &pBuffer)))
	{
		const char* szCode = (const char*) pBuffer->GetBufferPointer();
		const char* szStart = "// approximately";
		const char* szTexInsStart = "used (";
		const char* szAriInsStart = "texture, ";
		const char* szNum = strstr(szCode, szStart);

		if(szNum != NULL)
		{
			szNum += strlen(szStart);
			*nTotalInst = (UINT)atoi(szNum);

			const char* szTexIns = strstr(szNum, szTexInsStart);
			const char* szAriIns = strstr(szNum, szAriInsStart);

			if(szTexIns != NULL && szAriIns != NULL)
			{

				szTexIns += strlen(szTexInsStart);
				szAriIns += strlen(szAriInsStart);
				*nTexInst = (UINT)atoi(szTexIns);
				*nAriInst = (UINT)atoi(szAriIns);
			}
			else
			{
				*nTexInst = 0;
				*nAriInst = 0;
			}
		}
		
	}
	SAFE_RELEASE(pBuffer);
}

void A3DHLSLCore::GetApproximately(int* nVSTotalInst, int* nVSTexInst, int* nVSAriInst, int* nPSTotalInst, int* nPSTexInst, int* nPSAriInst)
{
	if(m_pVertexShaderInst != NULL && nVSTotalInst != NULL && nVSTexInst != NULL && nVSAriInst != NULL) {
		IntGetApproximately(m_pVertexShaderInst, nVSTotalInst, nVSTexInst, nVSAriInst);
	}
	if(m_pPixelShaderInst != NULL && nPSTotalInst != NULL && nPSTexInst != NULL && nPSAriInst != NULL) {
		IntGetApproximately(m_pPixelShaderInst, nPSTotalInst, nPSTexInst, nPSAriInst);
	}
}

A3DHLSLCore::Name2ParamDict* A3DHLSLCore::GetParamDict()
{
	return m_pPSDataSource 
		? m_pPSDataSource->GetParamDict() 
		: m_pVSDataSource 
		? m_pVSDataSource->GetParamDict() 
		: NULL;
}
A3DHLSLCore::LPCSAMPLER_STATE A3DHLSLCore::GetSamplerState(int nSamplerIdx)
{
	ASSERT(nSamplerIdx >= 0 && nSamplerIdx < MAX_SAMPLERCOUNT);
	return &m_pPSDataSource->GetSamplerStateArray()[m_aSampler[nSamplerIdx]];
}

bool A3DHLSLCore::GenerateConstTable(A3DHLSLDataSrc* pDataSrc, const SHADERINSTANCE* pShaderInst, bool bVertexShader)
{
	ID3DXConstantTable* pD3DXConstTabel = NULL;
	if(pShaderInst == NULL || (pD3DXConstTabel = pShaderInst->pConstants) == NULL)  {
		return false;
	}
	A3DHLSLDataSrc::NameSemanticDict& NameSemaDict = pDataSrc->GetNameSemanticDict();

	D3DXCONSTANTTABLE_DESC ctd;
	pD3DXConstTabel->GetDesc(&ctd);
	unsigned int nTopIndex = m_aConst.size();

	typedef abase::vector<D3DXCONSTANT_DESC> D3DConstDescArray;
	D3DConstDescArray aConstDesc;

	// 常量排序, 宝成常量按照寄存器索引顺序
	// 采样器会优先放在前面
	for(UINT i = 0; i < ctd.Constants; i++)
	{
		D3DXCONSTANT_DESC d3dcd;
		UINT nCount = 0;
		D3DXHANDLE hHandle = pD3DXConstTabel->GetConstant(NULL, i);
		pD3DXConstTabel->GetConstantDesc(hHandle, &d3dcd, &nCount);

		D3DConstDescArray::iterator it = aConstDesc.begin();
		for(;it != aConstDesc.end(); ++it)
		{
			int a = (it->RegisterSet == D3DXRS_SAMPLER ? 0 : 100) + it->RegisterIndex;
			int b = (d3dcd.RegisterSet == D3DXRS_SAMPLER ? 0 : 100) + d3dcd.RegisterIndex;
			if(a > b)
			{
				aConstDesc.insert(it, d3dcd);
				break;
			}
		}
		if(it == aConstDesc.end())
		{
			aConstDesc.push_back(d3dcd);
		}
	}
	ASSERT((UINT)aConstDesc.size() == ctd.Constants);

	for(UINT i = 0; i < ctd.Constants; i++)
	{
		CONSTANTDESC cd;
		UINT nCount = 0;

		cd.cd = aConstDesc[i];
		cd.bVertexShader = bVertexShader;
		cd.dwNameID      = a_MakeIDFromString(cd.cd.Name);
		cd.nIdx          = nTopIndex++;
		cd.bUnique       = true;

		ASSERT(cd.dwSemaID == 0);
		A3DHLSLDataSrc::NameSemanticDict::iterator& itSema = NameSemaDict.find(cd.cd.Name);
		if(itSema != NameSemaDict.end())  {
			cd.Semantic = AString(":") + itSema->second;
			cd.dwSemaID = a_MakeIDFromString(cd.Semantic);
		}
		
		ASSERT(strlen(cd.cd.Name) <= 40);

		m_aConst.push_back(cd);
	}
	return true;
}

bool A3DHLSLCore::Appear()
{
	if(m_pVertexShaderInst == NULL && m_pPixelShaderInst == NULL)
		return false;

	LPDIRECT3DDEVICE9 pd3dDevice = m_pDevice->GetD3DDevice();
	if(m_pPixelShaderInst != NULL)
	{
		ASSERT(m_pPSDataSource != NULL);
		LPCSAMPLER_STATE pSampStateArray = m_pPSDataSource->GetSamplerStateArray();

		for(UINT i = 0; i <= m_nMaxSamplerIdx; i++)
		{
			LPCSAMPLER_STATE pSampState = &pSampStateArray[m_aSampler[i]];
			if(pSampState->SampStateMask == 0)
				continue;
			for(int n = 1; n <= 13; n++)
			{
				if(pSampState->SampStateMask & (1 << n))
				{
					HRESULT hval = m_pDevice->SetSamplerState(i, 
						(D3DSAMPLERSTATETYPE)n, pSampState->SampState[n - 1]);
					ASSERT(SUCCEEDED(hval));
				}
			}
		}
	}

	return (
		(m_pVertexShaderInst
		? SUCCEEDED(m_pDevice->raw_SetVertexShader(m_pVertexShaderInst->pVShader))
		: true ) &&
		(m_pPixelShaderInst
		? SUCCEEDED(m_pDevice->raw_SetPixelShader(m_pPixelShaderInst->pPShader))
		: true));
}

bool A3DHLSLCore::Disappear()
{
	if(m_pVertexShaderInst != NULL)
		m_pDevice->ClearVertexShader();
	if(m_pPixelShaderInst != NULL)
		m_pDevice->ClearPixelShader();

	for(UINT i = 0; i < m_nMaxSamplerIdx; i++)
	{
		m_pDevice->ClearTexture(i);
	}
	return true;
}

void A3DHLSLCore::SetName(const char* szName)
{
	m_dwID = a_MakeIDFromString(szName);
	A3DObject::SetName(szName);
}

//////////////////////////////////////////////////////////////////////////
A3DHLSL::CONSTREGCONTEXT::CONSTREGCONTEXT() 
: nMin(0xffff)
, nMax(0)
, pConstBuffer(NULL)
{}
A3DHLSL::CONSTREGCONTEXT::~CONSTREGCONTEXT()
{
	if(pConstBuffer != NULL)
	{
		delete pConstBuffer;
		pConstBuffer = NULL;
	}
}

void A3DHLSL::CONSTREGCONTEXT::Build()
{
	ASSERT(pConstBuffer == NULL);
	if(nMin < nMax)
	{
		pConstBuffer = new A3DVECTOR4[nMax];
		memset(pConstBuffer, 0, sizeof(A3DVECTOR4) * nMax);
	}
}


A3DHLSL::A3DHLSL(A3DHLSLCore* pCore)
	: m_pCore				(pCore)
	//, m_cbConstTableSize	(NULL)
	//, m_lpCommConstDesc		(NULL)
	, m_aConstName			(NULL)
	, m_lpDataBinder			(NULL)
{
	m_dwClassID = A3D_CID_HLSL;
	memset(m_aSamplerSlot, 0, sizeof(m_aSamplerSlot));
	m_pCore->AddRef();
}

A3DHLSL::~A3DHLSL()
{
}

bool A3DHLSL::Init()
{
	GenConstBuffer(&m_VertexRegContext, true);
	GenConstBuffer(&m_PixelRegContext, false);
	BindCommConstData(NULL);
	return true;
}

bool A3DHLSL::Release()
{
	if(m_aConstName != NULL)
	{
		delete[] m_aConstName;
		m_aConstName = NULL;
	}
	m_pCore->Release();
	return true;
}

bool A3DHLSL::Rebuild()
{
	//ASSERT(0);
	//int cbConstTableSize = m_cbConstTableSize;
	A3DCCDBinder* lpDataBinder = m_lpDataBinder;
	UnbindCommConstData();
	m_VertexRegContext.~CONSTREGCONTEXT();
	m_PixelRegContext.~CONSTREGCONTEXT();
	GenConstBuffer(&m_VertexRegContext, true);
	GenConstBuffer(&m_PixelRegContext, false);
	BindCommConstData(lpDataBinder);
	return true;
}

bool A3DHLSL::UnbindCommConstData()
{
	if(m_lpDataBinder == NULL || m_aConstName == NULL) {
		return false;
	}
	CCDDict* pCCDDict = &m_lpDataBinder->GetDict();
	int nCount = GetConstCount();
	for(int i = 0; i < nCount; i++)
	{
		CONSTMAPDESC& cmd = m_aConstName[i];
		if(cmd.uMemberOffset >= 0)
		{
			CCDDict::iterator& itConst = pCCDDict->find(cmd.cd.dwNameID);
			if(itConst == pCCDDict->end())  {
				itConst = pCCDDict->find(cmd.cd.dwSemaID);
			}
			if(itConst != pCCDDict->end())
			{
				cmd.uMemberOffset = 0;
				itConst->second.nRef--;
			}
		}
	}
	m_lpDataBinder = NULL;
	return true;
}

bool A3DHLSL::BindCommConstData(A3DCCDBinder* lpCCDBinder)
{
	//ASSERT(0);
	if(m_lpDataBinder == lpCCDBinder && lpCCDBinder != NULL)  {
		return false;
	}

	if(m_aConstName != NULL)
	{
		delete[] m_aConstName;
		m_aConstName = NULL;
	}

	CCDDict* pCCDDict = NULL;
	if(lpCCDBinder != NULL)  {
		pCCDDict = &lpCCDBinder->GetDict();
	}

	int nCount = GetConstCount();
	m_aConstName = new CONSTMAPDESC[nCount];
	ConstArray aConst = m_pCore->GetConstArray();

	for(ConstArray::iterator it = aConst.begin();
		it != aConst.end(); ++it)
	{
		CONSTMAPDESC& cmd = m_aConstName[it->nIdx];
		cmd.uMemberOffset = -1;
		cmd.cd = *it;

		if(pCCDDict != NULL)
		{
			CCDDict::iterator& itConst = pCCDDict->find(cmd.cd.dwNameID);
			if(itConst == pCCDDict->end())  {
				itConst = pCCDDict->find(cmd.cd.dwSemaID);
			}
			if( itConst != pCCDDict->end() &&
				(cmd.cd.cd.Bytes >= (UINT)itConst->second.cbSize || 
				(cmd.cd.cd.RegisterSet == D3DXRS_SAMPLER && itConst->second.cbSize == A3DCOMMONCONSTDESC::CCD_SAMPLER) ))  
			{
				cmd.uMemberOffset = itConst->second.uMemberOffset;
				itConst->second.nRef++;
			}
		}
	}
	m_lpDataBinder = lpCCDBinder;
	return true;
}

bool A3DHLSL::Commit(const A3DCOMMCONSTTABLE* lpCommConstTable)
{
	A3DDevice* pDevice = m_pCore->GetA3DDevice();
	LPDIRECT3DDEVICE9 pd3dDevice = pDevice->GetD3DDevice();
	//pDevice->SetHLSLShader(this);

	if( lpCommConstTable != APPEAR_SETSHADERONLY )
	{
		int nCount = GetConstCount();
		//for(ConstMapArray::iterator it = m_aConstName.begin();
		//	it != m_aConstName.end(); ++it)
		for(int i = 0; i < nCount; i++)
		{
			CONSTMAPDESC& cmd = m_aConstName[i];
			A3DHLSLCore::CONSTANTDESC& cd = cmd.cd;

			if(cmd.uMemberOffset >= 0)
			{
				if(lpCommConstTable == NULL) {
					continue;
				}
				LPVOID* ppParam = (LPVOID*)(((BYTE*)lpCommConstTable) + cmd.uMemberOffset);
				if(cd.cd.RegisterSet == D3DXRS_SAMPLER)
				{
					//pd3dDevice->SetTexture(cd.cd.RegisterIndex, (IDirect3DBaseTexture9*)(*ppParam));
                    pDevice->raw_SetTexture(cd.cd.RegisterIndex, (IDirect3DBaseTexture9*)(*ppParam));
				}
				else if(cd.cd.RegisterSet == D3DXRS_FLOAT4)
				{
					if( cd.bVertexShader )
						pd3dDevice->SetVertexShaderConstantF(cd.cd.RegisterIndex, (const float*)(ppParam), cd.cd.RegisterCount);
					else
						pd3dDevice->SetPixelShaderConstantF(cd.cd.RegisterIndex, (const float*)(ppParam), cd.cd.RegisterCount);
				}
				else if(cd.cd.RegisterSet == D3DXRS_INT4)
				{
					if( cd.bVertexShader )
						pd3dDevice->SetVertexShaderConstantI(cd.cd.RegisterIndex, (const int*)(ppParam), cd.cd.RegisterCount);
					else
						pd3dDevice->SetPixelShaderConstantI(cd.cd.RegisterIndex, (const int*)(ppParam), cd.cd.RegisterCount);
				}
				else if(cd.cd.RegisterSet == D3DXRS_BOOL)
				{
					g_A3DErrLog.Log("A3DHLSL::Appear, unsupport RegisterSet == D3DXRS_BOOL.");
				}
				else
				{
					g_A3DErrLog.Log("A3DHLSL::Appear, invalidate RegisterSet.");
				}
			}
			else
			{
				if(cd.cd.RegisterSet == D3DXRS_SAMPLER)
				{
					//pd3dDevice->SetTexture(cd.cd.RegisterIndex, m_aSamplerSlot[cd.cd.RegisterIndex].pTextureBase);
                    pDevice->raw_SetTexture(cd.cd.RegisterIndex, m_aSamplerSlot[cd.cd.RegisterIndex].pTextureBase);
				}
				else if(cd.cd.RegisterSet == D3DXRS_FLOAT4)
				{
					if( cd.bVertexShader )
						pd3dDevice->SetVertexShaderConstantF(
						cd.cd.RegisterIndex, (const float*)(&m_VertexRegContext.pConstBuffer[cd.cd.RegisterIndex]), cd.cd.RegisterCount);
					else
						pd3dDevice->SetPixelShaderConstantF(
						cd.cd.RegisterIndex, (const float*)(&m_PixelRegContext.pConstBuffer[cd.cd.RegisterIndex]), cd.cd.RegisterCount);
				}
				else if(cd.cd.RegisterSet == D3DXRS_INT4)
				{
					if( cd.bVertexShader )
						pd3dDevice->SetVertexShaderConstantI(
						cd.cd.RegisterIndex, (const int*)(&m_VertexRegContext.pConstBuffer[cd.cd.RegisterIndex]), cd.cd.RegisterCount);
					else
						pd3dDevice->SetPixelShaderConstantI(
						cd.cd.RegisterIndex, (const int*)(&m_PixelRegContext.pConstBuffer[cd.cd.RegisterIndex]), cd.cd.RegisterCount);
				}
				else if(cd.cd.RegisterSet == D3DXRS_BOOL)
				{
					g_A3DErrLog.Log("A3DHLSL::Appear, unsupport RegisterSet == D3DXRS_BOOL.");
				}
				else
				{
					g_A3DErrLog.Log("A3DHLSL::Appear, invalidate RegisterSet.");
				}
			}
		}
	}
	return true;
}

bool A3DHLSL::Appear(const A3DCOMMCONSTTABLE* lpCommConstTable /* = NULL */)
{
	Commit(lpCommConstTable);
	return m_pCore->Appear();
}

bool A3DHLSL::Disappear()
{
	return m_pCore->Disappear();
}
bool A3DHLSL::SubmitCommTable(const A3DCOMMCONSTTABLE* lpCommConstTable)
{
	A3DDevice* pDevice = m_pCore->GetA3DDevice();
	LPDIRECT3DDEVICE9 pd3dDevice = pDevice->GetD3DDevice();

	if( lpCommConstTable != APPEAR_SETSHADERONLY )
	{
		if(lpCommConstTable == NULL) {
			return false;
		}

		int nCount = GetConstCount();
		for(int i = 0; i < nCount; i++)
		{
			CONSTMAPDESC& cmd = m_aConstName[i];
			A3DHLSLCore::CONSTANTDESC& cd = cmd.cd;

			if(cmd.uMemberOffset >= 0)
			{
				LPVOID* ppParam = (LPVOID*)(((BYTE*)lpCommConstTable) + cmd.uMemberOffset);
				if(cd.cd.RegisterSet == D3DXRS_SAMPLER)
				{
					//pd3dDevice->SetTexture(cd.cd.RegisterIndex, (IDirect3DBaseTexture9*)(*ppParam));
                    pDevice->raw_SetTexture(cd.cd.RegisterIndex, (IDirect3DBaseTexture9*)(*ppParam));
				}
				else if(cd.cd.RegisterSet == D3DXRS_FLOAT4)
				{
					if( cd.bVertexShader )
						pd3dDevice->SetVertexShaderConstantF(cd.cd.RegisterIndex, (const float*)(ppParam), cd.cd.RegisterCount);
					else
						pd3dDevice->SetPixelShaderConstantF(cd.cd.RegisterIndex, (const float*)(ppParam), cd.cd.RegisterCount);
				}
				else if(cd.cd.RegisterSet == D3DXRS_INT4)
				{
					if( cd.bVertexShader )
						pd3dDevice->SetVertexShaderConstantI(cd.cd.RegisterIndex, (const int*)(ppParam), cd.cd.RegisterCount);
					else
						pd3dDevice->SetPixelShaderConstantI(cd.cd.RegisterIndex, (const int*)(ppParam), cd.cd.RegisterCount);
				}
				else if(cd.cd.RegisterSet == D3DXRS_BOOL)
				{
					g_A3DErrLog.Log("A3DHLSL::SubmitCommTable, unsupport RegisterSet == D3DXRS_BOOL.");
				}
				else
				{
					g_A3DErrLog.Log("A3DHLSL::SubmitCommTable, invalidate RegisterSet.");
				}
			}
		}
	}
	return true;
}

A3DCCDBinder* A3DHLSL::CreateCommonConstDataBinder(const A3DCOMMONCONSTDESC* pConstDesc, int cbTable)
{
	A3DCCDBinder* pBinder = new A3DCCDBinder;

	if( ! pBinder->Init(pConstDesc, cbTable))
	{
		pBinder->Release();
		return NULL;
	}
	return pBinder;
}

bool A3DHLSL::SetValue1f(const char* szName, const float fConstant)
{
	A3DVECTOR4 v((float)0);
	v.x = fConstant;
	return SetConstantArrayF(szName, &v, 1);
}

bool A3DHLSL::SetValue2f(const char* szName, const float fConstant1, const float fConstant2)
{
	A3DVECTOR4 v((float)0);
	v.x = fConstant1;
	v.y = fConstant2;
	return SetConstantArrayF(szName, &v, 1);
}

bool A3DHLSL::SetValue3f(const char* szName, const A3DVECTOR3* pConstant)
{
	A3DVECTOR4 v(*pConstant);
	return SetConstantArrayF(szName, &v, 1);
}

bool A3DHLSL::SetValue4f(const char* szName, const A3DVECTOR4* pConstant)
{
	return SetConstantArrayF(szName, pConstant, 1);
}

bool A3DHLSL::SetValue1i(const char* szName, const int n1)
{
	A3DVECTOR4 v((float)0);
	v.x = (float)n1;
	return SetConstantArrayF(szName, &v, 1);
}

bool A3DHLSL::SetValue2i(const char* szName, const int n1, const int n2)
{
	A3DVECTOR4 v((float)0);
	v.x = (float)n1;
	v.y = (float)n2;
	return SetConstantArrayF(szName, &v, 1);
}
bool A3DHLSL::SetValue3i(const char* szName, const int n1, const int n2, const int n3)
{
	A3DVECTOR4 v((float)0);
	v.x = (float)n1;
	v.y = (float)n2;
	v.z = (float)n3;
	return SetConstantArrayF(szName, &v, 1);
}

bool A3DHLSL::SetValue4i(const char* szName, const int n1, const int n2, const int n3, const int n4)
{
	A3DVECTOR4 v((float)0);
	v.x = (float)n1;
	v.y = (float)n2;
	v.z = (float)n3;
	v.w = (float)n4;
	return SetConstantArrayF(szName, &v, 1);
}

bool A3DHLSL::SetValue1b(const char* szName, const bool b1)
{
	A3DVECTOR4 v((float)0);
	v.x = b1;
	return SetConstantArrayF(szName, &v, 1);
}

bool A3DHLSL::SetValue2b(const char* szName, const bool b1, const bool b2)
{
	A3DVECTOR4 v((float)0);
	v.x = b1;
	v.y = b2;
	return SetConstantArrayF(szName, &v, 1);
}

bool A3DHLSL::SetValue3b(const char* szName, const bool b1, const bool b2, const bool b3)
{
	A3DVECTOR4 v((float)0);
	v.x = b1;
	v.y = b2;
	v.z = b3;
	return SetConstantArrayF(szName, &v, 1);
}

bool A3DHLSL::SetValue4b(const char* szName, const bool b1, const bool b2, const bool b3, const bool b4)
{
	A3DVECTOR4 v((float)0);
	v.x = b1;
	v.y = b2;
	v.z = b3;
	v.w = b4;
	return SetConstantArrayF(szName, &v, 1);
}

bool A3DHLSL::SetConstantArrayF(const char* szName, const A3DVECTOR4* pConstant, int nFloat4Count)
{
	A3DDevice* pDevice = m_pCore->GetA3DDevice();
	LPDIRECT3DDEVICE9 pd3dDevice = pDevice->GetD3DDevice();
	bool bRet = false;

	ConstDict& sConstNameDict = m_pCore->GetConstDict();
	ConstDict::iterator it = sConstNameDict.find(AString(szName));
	if(it == sConstNameDict.end())
		return false;

UPDATE_SHADER_CACHE:
	CONSTMAPDESC& cmd = m_aConstName[it->second->nIdx];
	A3DHLSLCore::CONSTANTDESC& cd = *(it->second);

	if(cmd.uMemberOffset < 0/* && nFloat4Count == cd.cd.RegisterCount*/)
	{
		if(cd.cd.RegisterSet == D3DXRS_FLOAT4 || cd.cd.RegisterSet == D3DXRS_INT4)
		{
			bRet = true;
			if( cd.bVertexShader )
			{
				memcpy(&m_VertexRegContext.pConstBuffer[cd.cd.RegisterIndex], pConstant, cd.cd.RegisterCount * sizeof(A3DVECTOR4));
				if(pDevice->IsActivatedHLSLVS(m_pCore))
				{
					if(cd.cd.RegisterSet == D3DXRS_FLOAT4)
					{
						pd3dDevice->SetVertexShaderConstantF(cd.cd.RegisterIndex, (const float*)(pConstant), cd.cd.RegisterCount);
					}
					else if(cd.cd.RegisterSet == D3DXRS_INT4)
					{
						pd3dDevice->SetVertexShaderConstantI(cd.cd.RegisterIndex, (const int*)(pConstant), cd.cd.RegisterCount);
					}
				}
			}
			else
			{
				memcpy(&m_PixelRegContext.pConstBuffer[cd.cd.RegisterIndex], pConstant, cd.cd.RegisterCount * sizeof(A3DVECTOR4));
				if(pDevice->IsActivatedHLSLPS(m_pCore))
				{
					if(cd.cd.RegisterSet == D3DXRS_FLOAT4)
					{
						pd3dDevice->SetPixelShaderConstantF(cd.cd.RegisterIndex, (const float*)(pConstant), cd.cd.RegisterCount);
					}
					else if(cd.cd.RegisterSet == D3DXRS_INT4)
					{
						pd3dDevice->SetPixelShaderConstantI(cd.cd.RegisterIndex, (const int*)(pConstant), cd.cd.RegisterCount);
					}
				}
			}
		}
		else if(cd.cd.RegisterSet == D3DXRS_BOOL)
		{
			g_A3DErrLog.Log("A3DHLSL::SetConstantArrayF, unsupport RegisterSet == D3DXRS_BOOL.");
		}
		else
		{
			g_A3DErrLog.Log("A3DHLSL::SetConstantArrayF, invalidate RegisterSet.");
			return false;
		}
	}

	if(cd.bUnique == false && it->first[0] != '@')
	{
		it = sConstNameDict.find(AString("@") + szName);
		if(it == sConstNameDict.end() || it->second->bUnique != false)
		{
			ASSERT(0);	// 不应该出现这样的错误.
			return false;
		}
		goto UPDATE_SHADER_CACHE;
	}

	return bRet;
}
bool A3DHLSL::GetConstantArrayF(const char* szName, A3DVECTOR4* pConstant, int nFloat4Count)
{
	LPDIRECT3DDEVICE9 pd3dDevice = m_pCore->GetA3DDevice()->GetD3DDevice();
	DWORD dwNameID = a_MakeIDFromString(szName);
	bool bRet = false;

	A3DHLSLCore::ConstDict& sConstNameDict = m_pCore->GetConstDict();
	A3DHLSLCore::ConstDict::iterator it = sConstNameDict.find(szName);

	if(it == sConstNameDict.end())
		return false;

	CONSTMAPDESC& cmd = m_aConstName[it->second->nIdx];
	A3DHLSLCore::CONSTANTDESC& cd = *(it->second);

	if(cmd.uMemberOffset < 0 && (cd.dwNameID == dwNameID || (szName[0] == ':' && cd.dwSemaID == dwNameID)))
	{
		if(cd.cd.RegisterSet == D3DXRS_FLOAT4 || cd.cd.RegisterSet == D3DXRS_INT4)
		{
			if( cd.bVertexShader )
			{
				memcpy(pConstant, &m_VertexRegContext.pConstBuffer[cd.cd.RegisterIndex], cd.cd.RegisterCount * sizeof(A3DVECTOR4));
				bRet = true;
			}
			else
			{
				memcpy(pConstant, &m_PixelRegContext.pConstBuffer[cd.cd.RegisterIndex], cd.cd.RegisterCount * sizeof(A3DVECTOR4));
				bRet = true;
			}
		}
		else if(cd.cd.RegisterSet == D3DXRS_BOOL)
		{
			g_A3DErrLog.Log("A3DHLSL::GetConstantArrayF, unsupport RegisterSet == D3DXRS_BOOL.");
		}
		else
		{
			g_A3DErrLog.Log("A3DHLSL::GetConstantArrayF, invalidate RegisterSet.");
			return false;
		}
	}

	return bRet;
}

bool A3DHLSL::ReloadTexture()
{
	for(int i = 0; i < MAX_SAMPLERCOUNT; i++)
	{
		if(m_aSamplerSlot[i].pA3DObject != NULL)
			m_aSamplerSlot[i].UpdateD3DTexPtr(m_aSamplerSlot[i].pA3DObject);
	}
	return true;
}

//A3DHLSL* A3DHLSL::Clone(bool bRegisterToMgr)
//{
//	A3DHLSL* pHLSL = new A3DHLSL(m_pCore);
//
//	int nConstCount = GetConstCount();
//	pHLSL->m_aConstName = new CONSTMAPDESC[nConstCount];
//	memcpy(pHLSL->m_aConstName, m_aConstName, nConstCount * sizeof(CONSTMAPDESC));
//
//	//pHLSL->m_cbConstTableSize = m_cbConstTableSize;
//	//pHLSL->->m_lpDataBinder
//	pHLSL->m_VertexRegContext.nMin = m_VertexRegContext.nMin;
//	pHLSL->m_VertexRegContext.nMax = m_VertexRegContext.nMax;
//	pHLSL->m_PixelRegContext.nMin  = m_PixelRegContext.nMin;
//	pHLSL->m_PixelRegContext.nMax  = m_PixelRegContext.nMax;
//
//	if(m_VertexRegContext.nMin < m_VertexRegContext.nMax)
//	{
//		pHLSL->m_VertexRegContext.Build();
//		memcpy(pHLSL->m_VertexRegContext.pConstBuffer, m_VertexRegContext.pConstBuffer, sizeof(A3DVECTOR4) * m_VertexRegContext.nMax);
//	}
//
//	if(m_PixelRegContext.nMin < m_PixelRegContext.nMax)
//	{
//		pHLSL->m_PixelRegContext.Build();
//		memcpy(pHLSL->m_PixelRegContext.pConstBuffer, m_PixelRegContext.pConstBuffer, sizeof(A3DVECTOR4) * m_PixelRegContext.nMax);
//	}
//
//	//pHLSL->m_lpCommConstDesc = m_lpCommConstDesc;
//	memcpy(pHLSL->m_aSamplerSlot, m_aSamplerSlot, sizeof(SAMPLERSLOT) * MAX_SAMPLERCOUNT);
//
//	if(bRegisterToMgr)  {
//		m_pCore->m_pHLSLMgr->m_aShaderInstPtr.push_back(pHLSL);
//	}
//	return pHLSL;
//}

bool A3DHLSL::SetMacroAdapter(const char* szMacros, DWORD dwFlags)
{
	if(szMacros == NULL)
		ASSERT(0);
	AString strVertexFile;
	AString strPixelFile;
	AString strVSFuncName;
	AString strPSFuncName;
	AString strMacros;
	AString strNewMacros;
	
	A3DHLSLMan::ParseName(m_pCore->GetName(), &strVertexFile, &strVSFuncName, &strPixelFile, &strPSFuncName, &strMacros);

	if(dwFlags & A3DHLSL_CANONICALIZEDMACROS)  {
		strNewMacros = szMacros;
	}
	else  {
		A3DHLSLMan::CanonicalizeMacroString(szMacros, strNewMacros);
	}

	if(strMacros == strNewMacros)  {
		return false;
	}

	A3DHLSLCore* pCore = m_pCore->m_pHLSLMgr->LoadCore(strVertexFile, strVSFuncName, strPixelFile, strPSFuncName, strNewMacros, NULL);
	if(pCore == NULL)  {
		return false;
	}

	//int cbConstTableSize = m_cbConstTableSize;
	A3DCCDBinder* lpDataBinder = m_lpDataBinder;
	UnbindCommConstData();
	// 更新Core对象
	if(m_pCore->Release() == 1)  {
		pCore->m_pHLSLMgr->ReleaseCore(m_pCore);
	}
	m_pCore = pCore;
	m_pCore->AddRef();

	// 释放原来的资源
	// m_aConstName 在 SetCommConstDesc 中重新设置
	m_VertexRegContext.~CONSTREGCONTEXT();
	m_PixelRegContext.~CONSTREGCONTEXT();
	memset(m_aSamplerSlot, 0, sizeof(m_aSamplerSlot));

	// 初始化新的资源
	GenConstBuffer(&m_VertexRegContext, true);
	GenConstBuffer(&m_PixelRegContext, false);
	BindCommConstData(lpDataBinder);

	//A3DRELEASE(pHLSL);
	return true;
}

bool A3DHLSL::GetMacroAdapter(AString& strMacros)
{
	return A3DHLSLMan::ParseName(m_pCore->GetName(), NULL, NULL, NULL, NULL, &strMacros);
}

const A3DHLSL::ANNOTATION* A3DHLSL::FindAnnotationByName(const char* szName, const char* szAnnoName)
{
	typedef A3DHLSLCore::Name2ParamDict Name2ParamDict;
	typedef A3DHLSLCore::AnnotationArray AnnotationArray;
	
	Name2ParamDict* pParamDict = m_pCore->GetParamDict();
	if(pParamDict == NULL)
	{
		return NULL;
	}

	Name2ParamDict::iterator itAnnoArray = pParamDict->find(szName);
	if(itAnnoArray == pParamDict->end())
	{
		return NULL;
	}
	
	AnnotationArray& aAnno = itAnnoArray->second.aAnnotation;
	AnnotationArray::iterator itAnno = aAnno.begin();
	for(; itAnno != aAnno.end(); ++itAnno)
	{
		if(itAnno->Name == szAnnoName)
		{
			return &(*itAnno);
		}
	}
	return NULL;
}

const A3DHLSL::CONSTMAPDESC* A3DHLSL::GetConstMapByName(const char* szName)
{
	ConstDict& sConstNameDict = m_pCore->GetConstDict();
	ConstDict::iterator it = sConstNameDict.find(AString(szName));
	if(it == sConstNameDict.end())
	{
		return NULL;
	}
	return &(m_aConstName[it->second->nIdx]);
}
bool A3DHLSL::SetConstantMatrix(const char* szName, const A3DMATRIX4& mat)
{
	A3DMATRIX4 matT = mat;
	matT.Transpose();
	return SetConstantArrayF(szName, (A3DVECTOR4*)&matT, 4);
}

bool A3DHLSL::SetTexture(const char* szName, A3DObject* pA3DObject)
{
	DWORD dwNameID = a_MakeIDFromString(szName);
	bool bRet = false;
	const DWORD dwClassID = pA3DObject == NULL 
		? A3D_CID_UNKNOWN 
		: pA3DObject->GetClassID();

	if( dwClassID != A3D_CID_TEXTURE2D &&
		dwClassID != A3D_CID_TEXTURE3D &&
		dwClassID != A3D_CID_TEXTURECUBE &&
		dwClassID != A3D_CID_RENDERTARGET && 
		dwClassID != A3D_CID_HLSLSHADER && 
		pA3DObject != NULL )
	{
		return false;
	}

	A3DHLSLCore::ConstDict& sConstNameDict = m_pCore->GetConstDict();
	A3DHLSLCore::ConstDict::iterator it = sConstNameDict.find(szName);

	if(it == sConstNameDict.end())
		return false;

	CONSTMAPDESC& cmd = m_aConstName[it->second->nIdx];
	A3DHLSLCore::CONSTANTDESC& cd = *(it->second);

	if( cmd.uMemberOffset < 0 && (cd.dwNameID == dwNameID || (szName[0] == ':' && cd.dwSemaID == dwNameID)) )
	{
		if(cd.cd.RegisterSet == D3DXRS_SAMPLER)
		{
			ASSERT(cd.cd.RegisterIndex < MAX_SAMPLERCOUNT);
			SAMPLERSLOT& SamplerSlot = m_aSamplerSlot[cd.cd.RegisterIndex];

			SamplerSlot.bDepthTex = false;
			SamplerSlot.UpdateD3DTexPtr(pA3DObject);

			// 如果PS在设备上则同时更新设备上的Texture
			A3DDevice* pA3DDevice = m_pCore->GetA3DDevice();
			if(pA3DDevice->IsActivatedHLSLPS(m_pCore))
			{
				//pA3DDevice->GetD3DDevice()->SetTexture(cd.cd.RegisterIndex, SamplerSlot.pTextureBase);
                pA3DDevice->raw_SetTexture(cd.cd.RegisterIndex, SamplerSlot.pTextureBase);
			}

			bRet = true;
		}
	}

	return bRet;
}

bool A3DHLSL::SetTextureFromRT(const char* szName, A3DRenderTarget* pRanderTarget, bool bUseDepth)
{
	// TODO: 想想怎么能和SetTexture统一起来呢?
	DWORD dwNameID = a_MakeIDFromString(szName);
	bool bRet = false;
	if(bUseDepth) {
		const A3DRenderTarget::RTFMT& rtfmt = pRanderTarget->GetFormat();
		if(pRanderTarget->GetDepthTexture() == NULL || 
			(rtfmt.fmtDepth != A3DFMT_INTZ && rtfmt.fmtDepth != A3DFMT_RAWZ)) {
			return false;
		}
	}

	A3DHLSLCore::ConstDict& sConstNameDict = m_pCore->GetConstDict();
	A3DHLSLCore::ConstDict::iterator it = sConstNameDict.find(szName);

	if(it == sConstNameDict.end())
		return false;

	CONSTMAPDESC& cmd = m_aConstName[it->second->nIdx];
	A3DHLSLCore::CONSTANTDESC& cd = *(it->second);

	if( cmd.uMemberOffset < 0 && (cd.dwNameID == dwNameID || (szName[0] == ':' && cd.dwSemaID == dwNameID)) )
	{
		if(cd.cd.RegisterSet == D3DXRS_SAMPLER)
		{
			ASSERT(cd.cd.RegisterIndex < MAX_SAMPLERCOUNT);
			SAMPLERSLOT& SamplerSlot = m_aSamplerSlot[cd.cd.RegisterIndex];

			SamplerSlot.bDepthTex = bUseDepth;
			SamplerSlot.UpdateD3DTexPtr(pRanderTarget);

			// 如果PS在设备上则同时更新设备上的Texture
			A3DDevice* pA3DDevice = m_pCore->GetA3DDevice();
			if(pA3DDevice->IsActivatedHLSLPS(m_pCore))
			{
				pA3DDevice->raw_SetTexture(cd.cd.RegisterIndex, SamplerSlot.pTextureBase);
			}

			bRet = true;
		}
	}
	return bRet;
}

A3DObject* A3DHLSL::GetTexture(int nSamplerIdx)
{
	ASSERT(nSamplerIdx < MAX_SAMPLERCOUNT);
	return m_aSamplerSlot[nSamplerIdx].pA3DObject;
}

bool A3DHLSL::GenConstBuffer(CONSTREGCONTEXT* pContext, bool bVertexShader)
{
	A3DHLSLCore::ConstArray& aConst = m_pCore->GetConstArray();
	for(ConstArray::iterator it = aConst.begin();
		it != aConst.end(); ++it)
	{
		A3DHLSLCore::CONSTANTDESC& cd = *it;

		if(cd.bVertexShader == bVertexShader)
		{
			if(cd.cd.RegisterSet != D3DXRS_SAMPLER && cd.cd.RegisterSet != D3DXRS_BOOL)	// FIXME: 不支持BOOL类型
			{
				if((UINT)pContext->nMin > cd.cd.RegisterIndex)
					pContext->nMin = cd.cd.RegisterIndex;
				if((UINT)pContext->nMax < cd.cd.RegisterIndex + cd.cd.RegisterCount)
					pContext->nMax = cd.cd.RegisterIndex + cd.cd.RegisterCount;
			}
		}
	}

	pContext->Build();

	// 按照hlsl文件里的默认值初始化常量缓存
	if(pContext->pConstBuffer != NULL)
	{
		for(ConstArray::iterator it = aConst.begin();
			it != aConst.end(); ++it)
		{
			A3DHLSLCore::CONSTANTDESC& cd = *it;
			if(cd.bVertexShader == bVertexShader &&
				cd.cd.DefaultValue != NULL &&
				(cd.cd.RegisterSet != D3DXRS_SAMPLER && cd.cd.RegisterSet != D3DXRS_BOOL))	// FIXME: 不支持BOOL类型
			{
				memcpy(
					cd.bVertexShader 
					? &pContext->pConstBuffer[cd.cd.RegisterIndex]
					: &m_PixelRegContext.pConstBuffer[cd.cd.RegisterIndex],
					cd.cd.DefaultValue, cd.cd.RegisterCount * sizeof(A3DVECTOR4));
			}
		}
	}
	return true;
}


//////////////////////////////////////////////////////////////////////////
A3DCCDBinder::A3DCCDBinder()
: m_cbTable(0), m_nRefCount(0)
{
}

bool A3DCCDBinder::Init(const A3DCOMMONCONSTDESC* pConstDesc, int cbTable)
{
	for(int i = 0;; i++)
	{
		if(pConstDesc[i].szConstName == NULL)  {
			break;
		}
		DWORD dwNameID = a_MakeIDFromString(pConstDesc[i].szConstName);
		m_CommConstDescDict[dwNameID] = pConstDesc[i];
	}
	m_cbTable = cbTable;
	return true;
}

bool A3DCCDBinder::Release()
{
	return true;
}

A3DCCDBinder::CCDDict& A3DCCDBinder::GetDict()
{
	return m_CommConstDescDict;
}

const int* A3DCCDBinder::GetBindCountPtr(char* szName) const
{
	DWORD dwNameID = a_MakeIDFromString(szName);
	CCDDict::const_iterator it = m_CommConstDescDict.find(dwNameID);
	if(it != m_CommConstDescDict.end())
	{
		return &it->second.nRef;
	}
	return NULL;
}
//////////////////////////////////////////////////////////////////////////
A3DHLSLMan::A3DHLSLMan(A3DDevice* pA3DDevice)
	: m_pDevice			(pA3DDevice)
	, m_bChanged		(false)
	, m_bEnableCache	(false)
	, m_lpLogReceiver	(NULL)
	//, m_dwFlags			(NULL)
{
}

A3DHLSLMan::~A3DHLSLMan()
{
}

bool A3DHLSLMan::Init(bool bEnableCache)
{
	m_bEnableCache = bEnableCache;

	s_pText2EnumDict = new Text2EnumDict;
	if(s_pText2EnumDict != NULL)
	{
		(*s_pText2EnumDict)["ADDRESSU"]      = D3DSAMP_ADDRESSU;
		(*s_pText2EnumDict)["ADDRESSV"]      = D3DSAMP_ADDRESSV;
		(*s_pText2EnumDict)["ADDRESSW"]      = D3DSAMP_ADDRESSW;
		(*s_pText2EnumDict)["BORDERCOLOR"]   = D3DSAMP_BORDERCOLOR;
		(*s_pText2EnumDict)["MAGFILTER"]     = D3DSAMP_MAGFILTER;
		(*s_pText2EnumDict)["MINFILTER"]     = D3DSAMP_MINFILTER;
		(*s_pText2EnumDict)["MIPFILTER"]     = D3DSAMP_MIPFILTER;
		(*s_pText2EnumDict)["MIPMAPLODBIAS"] = D3DSAMP_MIPMAPLODBIAS;
		(*s_pText2EnumDict)["MAXMIPLEVEL"]   = D3DSAMP_MAXMIPLEVEL;
		(*s_pText2EnumDict)["MAXANISOTROPY"] = D3DSAMP_MAXANISOTROPY;
		(*s_pText2EnumDict)["SRGBTEXTURE"]   = D3DSAMP_SRGBTEXTURE;
		(*s_pText2EnumDict)["ELEMENTINDEX"]  = D3DSAMP_ELEMENTINDEX;
		(*s_pText2EnumDict)["DMAPOFFSET"]    = D3DSAMP_DMAPOFFSET;

		(*s_pText2EnumDict)["WRAP"]          = D3DTADDRESS_WRAP;
		(*s_pText2EnumDict)["MIRROR"]        = D3DTADDRESS_MIRROR;
		(*s_pText2EnumDict)["CLAMP"]         = D3DTADDRESS_CLAMP;
		(*s_pText2EnumDict)["BORDER"]        = D3DTADDRESS_BORDER;
		(*s_pText2EnumDict)["MIRRORONCE"]    = D3DTADDRESS_MIRRORONCE;

		(*s_pText2EnumDict)["NONE"]            = D3DTEXF_NONE;
		(*s_pText2EnumDict)["POINT"]           = D3DTEXF_POINT;
		(*s_pText2EnumDict)["LINEAR"]          = D3DTEXF_LINEAR;
		(*s_pText2EnumDict)["ANISOTROPIC"]     = D3DTEXF_ANISOTROPIC;
		(*s_pText2EnumDict)["PYRAMIDALQUAD"]   = D3DTEXF_PYRAMIDALQUAD;
		(*s_pText2EnumDict)["GAUSSIANQUAD"]    = D3DTEXF_GAUSSIANQUAD;
		(*s_pText2EnumDict)["CONVOLUTIONMONO"] = D3DTEXF_CONVOLUTIONMONO;
	}

	InitializeCriticalSection(&m_Locker);
	return true;
}



bool A3DHLSLMan::Release()
{
	DWORD         dwLength;
	HLSLMANHEADER Header;
	Header.cbSize = sizeof(Header);
	Header.dwShaderCount = 0;
	Header.dwRefFileCount = m_FileTimeDict.size();

	AFile file;
	int fileOffset = 0;
	if(m_bChanged == true && m_bEnableCache == true)
	{
		BOOL bRet = TRUE;

		if(bRet == FALSE)
			m_bChanged = false;
		else
		{
			if(file.Open(szCachePath, AFILE_BINARY | AFILE_CREATENEW | AFILE_TEMPMEMORY) == false)		// TODO: 改成不是每次都写盘
			{
				m_bChanged = false;
				g_A3DErrLog.Log("A3DHLSLMan::Release() : Can not write cache file!");
			}
			else
			{
				fileOffset = file.GetPos();
				file.Write(&Header, sizeof(Header), &dwLength);
			}
		}
	}

	if(m_bChanged == true && m_bEnableCache == true)
	{
		for(FileTimeDict::iterator it = m_FileTimeDict.begin(); 
			it != m_FileTimeDict.end(); ++it)
		{
			file.WriteString(it->first);
		}

		// 更新文件头中的dwShaderCount值
		file.Seek(fileOffset, AFILE_SEEK_SET);
		file.Write(&Header, sizeof(Header), &dwLength);
	}
	if(s_pText2EnumDict != NULL)
	{
		delete s_pText2EnumDict;
		s_pText2EnumDict = NULL;
	}
	// 释放掉所有 A3DHLSL
	ReleaseShader((A3DHLSL*)-1);

	//// 释放所有HLSLCore
	//for(ShaderCorePtrArray::iterator it = m_aShaderCorePtrArray.begin();
	//	it != m_aShaderCorePtrArray.end(); ++it)
	//{
	//	A3DHLSLCore* pCore = *it;
	//	m_aShaderCorePtrArray.erase(it);
	//	while(pCore->Release() == 0);
	//	delete pCore;
	//}

	for(HLSLDataSrcDict::iterator it = m_DataSrcDict.begin();
		it != m_DataSrcDict.end(); ++it)
	{
		SAFE_RELEASE(it->second);
	}
	DeleteCriticalSection(&m_Locker);

	file.Close(); 

	return true;
}


bool A3DHLSLMan::GenerateName(const char* pszVertexShaderFile, const char* szVSFuncName, const char* pszPixelShaderFile, const char* szPSFuncName, MacroArray& aMacros, AString& strName)
{
	AString strVertexFile;
	AString strPixelFile;

	if(pszVertexShaderFile != NULL)
		af_GetRelativePath(pszVertexShaderFile, strVertexFile);
	if(pszPixelShaderFile != NULL)
		af_GetRelativePath(pszPixelShaderFile, strPixelFile);
	strVertexFile.MakeLower();
	strPixelFile.MakeLower();
	strName.Empty();
	strName = strVertexFile + "|" + szVSFuncName+ "|" + strPixelFile + "|" + szPSFuncName + "|";
	for(MacroArray::iterator it = aMacros.begin(); it != aMacros.end(); ++it)
	{
		if(it->Name.GetLength() == 0)
			continue;
		if(it->Definition.GetLength() > 0)
			strName += it->Name + "=" + it->Definition + ";";
		else
			strName += it->Name + ";";
	}
	return true;
}

bool A3DHLSLMan::ParseName(const AString& strName, AString* pVertexShaderFile, AString* pVSFuncName, AString* pPixelShaderFile, AString* pPSFuncName, AString* pMacros)
{
	int nPos1 = strName.Find('|');
	int nPos2 = strName.Find('|', nPos1 + 1);
	int nPos3 = strName.Find('|', nPos2 + 1);
	int nPos4 = strName.Find('|', nPos3 + 1);
	int nPos5 = strName.Find('|', nPos4 + 1);

	if(pVertexShaderFile != NULL)
		*pVertexShaderFile = strName.Mid(0, nPos1);

	if(pVSFuncName != NULL)
		*pVSFuncName = strName.Mid(nPos1 + 1, nPos2 - nPos1 - 1);

	if(pPixelShaderFile != NULL)
		*pPixelShaderFile  = strName.Mid(nPos2 + 1, nPos3 - nPos2 - 1);

	if(pPSFuncName != NULL)
		*pPSFuncName = strName.Mid(nPos3 + 1, nPos4 - nPos3 - 1);

	if(pMacros != NULL)
		*pMacros = strName.Mid(nPos4 + 1, nPos5 - nPos4 - 1);

	return true;
}

bool A3DHLSLMan::ParseNameFromFile(const char* szShaderName, AString* pVertexShaderFile, AString* pVSFuncName, AString* pPixelShaderFile, AString* pPSFuncName, AString* pMacros)
{
	AFileImage     File;
	HLSLCOREHEADER Header;
	DWORD          dwLength;
	AString		   strName;
	if( ! File.Open(szShaderName, AFILE_BINARY | AFILE_OPENEXIST | AFILE_TEMPMEMORY))
	{
		return false;
	}

	File.Read(&Header.cbSize, sizeof(Header.cbSize), &dwLength);
	File.Read(((BYTE*)&Header.cbSize) + sizeof(Header.cbSize), Header.cbSize - sizeof(Header.cbSize), &dwLength);
	
	if(File.ReadString(strName))
	{
		return ParseName(strName, pVertexShaderFile, pVSFuncName, pPixelShaderFile, pPSFuncName, pMacros);
	}

	File.Close();

	return false;
}

bool A3DHLSLMan::BuildD3DMacro(const MacroArray& aMacros, D3DXMacroArray& aD3DXMacros)
{
	D3DXMACRO macro;
	aD3DXMacros.clear();

	for(MacroArray::const_iterator it = aMacros.begin();
		it != aMacros.end(); ++it)
	{
		macro.Name = it->Name;
		macro.Definition = it->Definition;
		aD3DXMacros.push_back(macro);
	}
	macro.Name = NULL;
	macro.Definition = NULL;
	aD3DXMacros.push_back(macro);
	return true;
}

bool A3DHLSLMan::StringToMacroArray(const char* szMacros, MacroArray& aMacros)
{
	if(szMacros == NULL || szMacros[0] == '\0')  {
		return false;
	}
	A3DSHDMACRO Macro;
	AStringArray aString;
	SplitString(szMacros, ';', aString);
	for(AStringArray::iterator it = aString.begin();
		it != aString.end(); ++it)
	{
		int nDef = it->Find('=');
		if(nDef > 0)
		{
			Macro.Name = it->Mid(0, nDef);
			Macro.Definition = it->Mid(nDef + 1, it->GetLength() - nDef - 1);
		}
		else
		{
			Macro.Name = *it;
			Macro.Definition.Empty();
		}
		if(Macro.Name.GetLength() > 0){
			aMacros.push_back(Macro);
		}
	}
	return true;
}

bool A3DHLSLMan::MacroArrayToString(const MacroArray& aMacros, AString& strMacros)
{
	strMacros.Empty();
	// TODO: 预先分配 strMacros
	for(MacroArray::const_iterator it = aMacros.begin(); it != aMacros.end(); ++it)
	{
		if(it->Name.GetLength() == 0)
			continue;
		if(it->Definition.GetLength() > 0)
			strMacros += it->Name + "=" + it->Definition + ";";
		else
			strMacros += it->Name + ";";
	}
	return true;
}

bool A3DHLSLMan::StringArrayToMacroArray(const AStringArray& aStrings, MacroArray& aMacros)
{
	A3DSHDMACRO SHDMacro;
	aMacros.clear();

	for(AStringArray::const_iterator it = aStrings.begin();
		it != aStrings.end(); ++it)
	{
		int p = it->Find('=');
		if(p >= 0) {
			SHDMacro.Name = it->Mid(0, p);
			SHDMacro.Definition = it->Mid(p + 1, it->GetLength() - p - 1);
		}
		else {
			SHDMacro.Name = *it;
			SHDMacro.Definition.Empty();
		}
		aMacros.push_back(SHDMacro);
	}
	return true;		 
}

// 排序
bool A3DHLSLMan::CanonicalizeMacroArray(MacroArray& aMacros)
{
	for(MacroArray::iterator it = aMacros.begin();
		it != aMacros.end(); ++it)  {
			it->Name.TrimLeft(' ');
			it->Name.TrimRight(' ');

			if(it->Name.GetLength() == NULL)  {
				it = aMacros.erase(it);
				if(it == aMacros.end())  {
					break;
				}
			}

			it->Definition.TrimLeft(' ');
			it->Definition.TrimRight(' ');
	}

	int nCount = (int)aMacros.size();
	if(nCount >= 2)
	{
		A3DSHDMACRO* pMacro = &aMacros.front();
		for(int n = 0; n < nCount; n++)
		{
			for(int i = 0; i < nCount - n - 1; i++)
			{
				int r = strcmp(pMacro[i].Name, pMacro[i + 1].Name);
				if(r == 0)
				{
					pMacro[i + 1].Name.Empty();
				}
				else if(r > 0)
				{
					A3DSHDMACRO Macro = pMacro[i];
					pMacro[i] = pMacro[i + 1];
					pMacro[i + 1] = Macro;
				}
			}
		}
	}
	return true;
}

bool A3DHLSLMan::CanonicalizeMacroString(const char* szMacros, AString& strMacros)
{
	MacroArray aMacros;
	if( ! StringToMacroArray(szMacros, aMacros))  {
		strMacros.Empty();
		return true;
	}

	if(aMacros.size() <= 1)  {
		strMacros = szMacros;
		return true;
	}
	CanonicalizeMacroArray(aMacros);
	MacroArrayToString(aMacros, strMacros);
	return true;
}

int SplitString(const AString& str, char ch, AStringArray& aString)
{
	int pos = 0;
	int next;
	int len = str.GetLength();
	do 
	{
		next = str.Find(ch, pos);
		if(next < 0)  {
			next = len;
		}
		aString.push_back(str.Mid(pos, next - pos));
		pos = next + 1;
	} while(next != len);

	return (int)aString.size();
}

A3DHLSL* CreateA3DHLSL_Common( const char* szVSFileName, const char* szVSEntryName, const char* szPSFileName, const char* szPSEntryName, int nMacroNum, const char** szMacroList, A3DDevice* pA3DDev, const D3DVERTEXELEMENT9* aVertElement9 )
{
	AString strMacro = "_ANGELICA_2_2;";
	if(nMacroNum > 0)
	{
		for(int i = 0; i < nMacroNum; i += 1)
		{
			strMacro += AString(szMacroList[i]) + AString(";");
		}
	}
	A3DHLSL* pHLSL = pA3DDev->GetA3DEngine()->GetA3DHLSLMan()->LoadShader(szVSFileName, szVSEntryName,
		szPSFileName, szPSEntryName, strMacro, 0);
	
	if(!pHLSL)
		return NULL;

	A3DEnvironment* pEnvironment = pA3DDev->GetA3DEngine()->GetA3DEnvironment();	// 取环境对象
	A3DCCDBinder* pBinder = pEnvironment->GetCCDBinder();			// 取环境对象的绑定对象
	pHLSL->BindCommConstData(pBinder);
	return pHLSL;
}

void ReleaseA3DHLSL_Common( A3DDevice* pDev, A3DHLSL* pHLSL )
{
	A3DHLSLMan* pHLSLMan = pDev->GetA3DEngine()->GetA3DHLSLMan();
	pHLSLMan->ReleaseShader(pHLSL);
	pHLSL = NULL;
}

bool ReloadA3DHLSL_Common( A3DHLSL* pHLSL )
{
	//TODO:没有实现重新读取shader
	return false;//return pHLSL->Rebuild();
}

A3DHLSLMan::ShaderCorePtrArray::iterator A3DHLSLMan::FindByName(const AString& strName)
{
	for(ShaderCorePtrArray::iterator it = m_aShaderCorePtrArray.begin(); 
		it != m_aShaderCorePtrArray.end(); ++it)
	{
		if(strName == (*it)->GetName())
			return it;
	}
	return m_aShaderCorePtrArray.end();
}

bool A3DHLSLMan::RegisterRefFile(const AString& strFile)
{
	FILETIME FileTime;
	FileTime.dwLowDateTime = 0;
	FileTime.dwHighDateTime = 0;
	m_FileTimeDict[strFile] = FileTime;
	return true;
}

bool A3DHLSLMan::GenCompiledFilename(const char* szVertexShaderFile, const char* szPixelShaderFile, DWORD dwNameID, AString& strFilename)
{
	AString strPath;
	if(szPixelShaderFile == 0 && szVertexShaderFile == 0) {
		return false;
	}
	const char* szFilename = (szVertexShaderFile == 0)
		? szPixelShaderFile
		: ((szPixelShaderFile == 0) 
			? szVertexShaderFile 
			: szPixelShaderFile);
	strPath = szFilename;
	if(strPath.GetLength() == 0){
		return false;
	}
	strPath.MakeLower();
	int nPos = strPath.Find("\\shaders\\2.2\\hlsl\\", 0);
	if(nPos < 0) {
		return false;
	}
	strPath[nPos + 13] = 's';
	strPath[nPos + 14] = 'd';
	strPath[nPos + 15] = 'r';
	strPath[nPos + 16] = 'x';

	af_GetFilePath(strPath, strPath);
	strFilename.Format("%s\\%08X_%s.obj", strPath, dwNameID, szFilename == szVertexShaderFile ? "VS" : "PS");

	return true;
}

A3DHLSLCore* A3DHLSLMan::LoadCore(const char* pszVertexShaderFile, const char* szVSFuncName, const char* pszPixelShaderFile, const char* szPSFuncName, const char* szMacro, DWORD dwFlags)
{
	AString			strName;
	MacroArray		aMacros;
	D3DXMacroArray	aD3DXMacros;
	A3DHLSLCore*	pShader = NULL;

	AString strVertexShaderFile;
	AString strPixelShaderFile;

	if(pszVertexShaderFile != NULL)
	{
		af_GetFullPath(strVertexShaderFile, pszVertexShaderFile);
		pszVertexShaderFile = strVertexShaderFile;
		RegisterRefFile(pszVertexShaderFile);
	}

	if(pszPixelShaderFile != NULL)
	{
		af_GetFullPath(strPixelShaderFile, pszPixelShaderFile);
		pszPixelShaderFile = strPixelShaderFile;
		RegisterRefFile(pszPixelShaderFile);
	}

	A3DHLSLDataSrc* pVertexDataSrc = NULL;
	A3DHLSLDataSrc* pPixelDataSrc = NULL;

	if(pszPixelShaderFile != NULL)
	{
		pPixelDataSrc = MountDataSource(strPixelShaderFile);
	}
	if(pszVertexShaderFile != NULL)
	{
		pVertexDataSrc = MountDataSource(strVertexShaderFile);
	}

	StringToMacroArray(szMacro, aMacros);
	BuildD3DMacro(aMacros, aD3DXMacros);

	GenerateName(pszVertexShaderFile, szVSFuncName, pszPixelShaderFile, szPSFuncName, aMacros, strName);
	ShaderCorePtrArray::iterator itShader = FindByName(strName);
	if(itShader != m_aShaderCorePtrArray.end())
	{
		ASSERT((*itShader)->m_pVertexShaderInst != NULL || (*itShader)->m_pPixelShaderInst != NULL);
		pShader = *itShader;
		//goto CREATE_NEW_HLSL;
		return pShader;
	}

	pShader = new A3DHLSLCore(m_pDevice, this, pVertexDataSrc, pPixelDataSrc);
	if(pShader->Init() == true)
	{
		pShader->AddRef();
		pShader->SetName(strName);
		if(pShader->LoadCore(szVSFuncName, szPSFuncName, szMacro))
		{
#ifdef _DEBUG
			if(pszVertexShaderFile != NULL && pszVertexShaderFile[0] != '\0')
				ASSERT(pShader->m_pVertexShaderInst != NULL);
			if(pszPixelShaderFile != NULL && pszPixelShaderFile[0] != '\0')
				ASSERT(pShader->m_pPixelShaderInst != NULL);
#endif // _DEBUG
			m_aShaderCorePtrArray.push_back(pShader);
			m_bChanged = true;
			return pShader;
		}
	}

	pShader->Release();
	delete pShader;

	g_A3DErrLog.Log("Failed to create HLSL shader.VS:%s\nPS:%s", pszVertexShaderFile, pszPixelShaderFile);
	return NULL;
}

bool A3DHLSLMan::ReleaseCore(A3DHLSLCore* pCore)
{
	ShaderCorePtrArray::iterator itCore = FindByName(pCore->GetName());
	if(itCore != m_aShaderCorePtrArray.end())
	{
		m_aShaderCorePtrArray.erase(itCore);
		pCore->Release();
		delete pCore;
		return true;
	}
	ASSERT(0);
	return false;
}

A3DHLSL* A3DHLSLMan::LoadShader(const char* pszVertexShaderFile, const char* szVSFuncName, const char* pszPixelShaderFile, const char* szPSFuncName, const char* szMacro, DWORD dwFlags)
{
	ACSWrapper locker(&m_Locker);
	A3DHLSLCore* pCore = LoadCore(pszVertexShaderFile, szVSFuncName, pszPixelShaderFile, szPSFuncName, szMacro, dwFlags);
	if(pCore == NULL)  {
		return NULL;
	}

	A3DHLSL* pHLSL;
	pHLSL = new A3DHLSL(pCore);
	pHLSL->Init();
	m_aShaderInstPtr.push_back(pHLSL);
	return pHLSL;
}

A3DHLSL* A3DHLSLMan::LoadShader( const char* pszVertexShaderFile, const char* pszPixelShaderFile, const char* szMacro )
{
	return LoadShader(pszVertexShaderFile, g_szDefaultVSEntryName, pszPixelShaderFile, g_szDefaultPSEntryName, szMacro, 0);
}

void A3DHLSLMan::ReleaseShader(A3DHLSL* pHLSL)
{
	if(pHLSL == NULL)
		return;

	ACSWrapper locker(&m_Locker);
	for(ShaderInstPtrArray::iterator it = m_aShaderInstPtr.begin();
		it != m_aShaderInstPtr.end(); ++it)
	{
		if(*it == pHLSL || pHLSL == (A3DHLSL*)-1) // -1 为内部使用的参数
		{
			A3DHLSLCore* pCore = (*it)->GetCore();
			(*it)->Release();
			delete *it;
			if(pHLSL != (A3DHLSL*)-1) {
				m_aShaderInstPtr.erase(it);
			}
			else {
				*it = NULL;
			}

			// 如果Core计数为0, 则释放掉Core
			if(pCore->m_nRefCount == 1)
			{
				ReleaseCore(pCore);
			}
			if(pHLSL != (A3DHLSL*)-1) {
				return;
			}
		}
	}
}

bool A3DHLSLMan::CheckUpdate()
{
	if(m_bEnableCache == false)
		return true;

	HANDLE hFile;
	FILETIME ftCache;
	FILETIME ftSource;

	hFile = CreateFileA(szCachePath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile == INVALID_HANDLE_VALUE)
		return false;

	GetFileTime(hFile, NULL, NULL, &ftCache);
	CloseHandle(hFile);

	for(FileTimeDict::iterator it = m_FileTimeDict.begin();
		it != m_FileTimeDict.end(); ++it)
	{
		hFile = CreateFileA(it->first, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if(hFile == INVALID_HANDLE_VALUE)
			continue;
		GetFileTime(hFile, NULL, NULL, &ftSource);
		CloseHandle(hFile);

		if(CompareFileTime(&ftCache, &ftSource) < 0)
		{
			RebuildAll();
			return true;
		}
	}
	return false;
}

A3DHLSLDataSrc*	A3DHLSLMan::MountDataSource(const char* szShaderFile)
{
	if(szShaderFile == NULL || szShaderFile[0] == '\0')  {
		return NULL;
	}

	HLSLDataSrcDict::iterator it = m_DataSrcDict.find(szShaderFile);
	if(it != m_DataSrcDict.end())  {
		return it->second;
	}

	A3DHLSLDataSrc* pDataSrc = new A3DHLSLDataSrc(m_pDevice, this, szShaderFile);
	if( (g_pA3DConfig->GetFlagDoNotReplaceBySdrx() == false && pDataSrc->InitFromObj()) ||
		pDataSrc->InitFromHLSL(NULL) )
	{
		pDataSrc->AddRef();
		m_DataSrcDict[szShaderFile] = pDataSrc;
	}
	else  {
		pDataSrc->AddRef();
		SAFE_RELEASE(pDataSrc);
	}

	return pDataSrc;
}

void A3DHLSLMan::AppendErrorMsg(const char* szFormat, ...)
{
	char buffer[1024];
	//if(m_lpLogReceiver != NULL)
	//{
		class MyString : public AString
		{
		public:
			static int GetFormatLen(const char* szFormat, va_list argList)
			{
				return AString::GetFormatLen(szFormat, argList);
			}
		};
		va_list argList;
		va_start(argList, szFormat);

		int nMaxLen = MyString::GetFormatLen(szFormat, argList) + 1;

		char* pBuffer = nMaxLen > 1024 ? new char[nMaxLen] : buffer;
		vsprintf(pBuffer, szFormat, argList);
		if(m_lpLogReceiver != NULL)
		{
			m_lpLogReceiver(pBuffer);
		}
		else
		{
			OutputDebugStringA(pBuffer);
		}

		if(pBuffer != buffer)  {
			delete pBuffer;
		}
		pBuffer = NULL;

		va_end(argList);
	//}
}


bool A3DHLSLMan::RebuildAll()
{
	return RebuildByKeyword(NULL);
}

bool A3DHLSLMan::RebuildByKeyword(const char* pszKeyword)
{
	bool bval = true;
	ShaderCorePtrArray aChanged;
	AString strKeyword = pszKeyword;

	typedef abase::hash_map<DWORD_PTR, DWORD_PTR> ObjChangedDict;
	ObjChangedDict DataSrcDict;
	ObjChangedDict CoreDict;
	ACSWrapper locker(&m_Locker);

	strKeyword.MakeLower();
	AppendErrorMsg("\r\n\r\n\r\n--------------------------------------------------------\r\n");
	for(HLSLDataSrcDict::iterator it = m_DataSrcDict.begin(); 
		it != m_DataSrcDict.end(); ++it)
	{
		A3DHLSLDataSrc* pDataSrc = it->second;
		if(pszKeyword == NULL || strstr(pDataSrc->GetName(), strKeyword) != NULL)
		{
			AppendErrorMsg("编译Shader:%s\r\n", pDataSrc->GetName());
			pDataSrc->ReloadShaderInst();
			DataSrcDict[(DWORD_PTR)pDataSrc] = 0;
		}
	}

	for(ShaderCorePtrArray::iterator it = m_aShaderCorePtrArray.begin();
		it != m_aShaderCorePtrArray.end(); ++it)
	{
		A3DHLSLCore* pCore = *it;
		ObjChangedDict::iterator itpsds = DataSrcDict.find((DWORD_PTR)pCore->m_pPSDataSource);
		ObjChangedDict::iterator itvsds = DataSrcDict.find((DWORD_PTR)pCore->m_pVSDataSource);
		if(itpsds != DataSrcDict.end() || itvsds != DataSrcDict.end())
		{
			pCore->Rebuild();
			CoreDict[(DWORD_PTR)pCore] = 0;
		}
	}
	for(ShaderInstPtrArray::iterator it = m_aShaderInstPtr.begin();
		it != m_aShaderInstPtr.end(); ++it)
	{
		A3DHLSL* pHLSL = *it;
		ObjChangedDict::iterator itCore = CoreDict.find((DWORD_PTR)pHLSL->GetCore());
		if(itCore != CoreDict.end())
		{
			pHLSL->Rebuild();
		}
	}
	m_bChanged = bval;
	return bval;
}

void A3DHLSLMan::SetLogReceiver(LPLOGRECEIVER lpLogReceiver)
{
	ACSWrapper locker(&m_Locker);
	m_lpLogReceiver = lpLogReceiver;
}

void A3DHLSL::SAMPLERSLOT::UpdateD3DTexPtr(A3DObject* _pA3DObject)
{
	pA3DObject = _pA3DObject;

	const DWORD dwClassID = pA3DObject == NULL 
		? A3D_CID_UNKNOWN 
		: pA3DObject->GetClassID();

	if(pA3DObject == NULL)
	{
		pTextureBase = NULL;
		return;
	}

	if(dwClassID == A3D_CID_RENDERTARGET)
	{
		if(bDepthTex) {
			pTextureBase = ((A3DRenderTarget*)pA3DObject)->GetDepthTexture();
		}
		else {
			pTextureBase = ((A3DRenderTarget*)pA3DObject)->GetTargetTexture();
		}
	}
	else
	{
		pTextureBase = ((A3DTexture*)pA3DObject)->GetD3DBaseTexture();
	}
}

void A3DHLSLCore::SAMPLER_STATE::SetState(D3DSAMPLERSTATETYPE Type, DWORD dwState)
{
	SampStateMask |= (1 << Type);
	SampState[(int)Type - 1] = dwState;
}

void A3DHLSLCore::SAMPLER_STATE::ResetState(D3DSAMPLERSTATETYPE Type)
{
	SampStateMask &= (~(1 << Type));
}

void A3DHLSLCore::SAMPLER_STATE::Cleanup()
{
	SampStateMask = 0;
	memset(SampState, 0, sizeof(DWORD) * 13);
	strSamplerName.Empty();
	strTextureName.Empty();
	strSemantic.Empty();
	strDefault.Empty();
}