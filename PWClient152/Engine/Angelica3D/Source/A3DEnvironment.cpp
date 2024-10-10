#include "A3D.h"
#include "A3DPI.h"
#include "A3DHLSL.h"
#include "A3DEnvironment.h"
typedef A3DEnvironment::COMMON COMMON;

static A3DCOMMONCONSTDESC s_aEnvCommConst[] =
{
	{":Diffuse",								MEMBER_OFFSET(COMMON, DirDiffuse), A3DCOMMONCONSTDESC::CCD_COLORVAL},
	{":LightDir",								MEMBER_OFFSET(COMMON, LightDirection), A3DCOMMONCONSTDESC::CCD_FLOAT3},
	{":LightDirInView",							MEMBER_OFFSET(COMMON, LightDirInView), A3DCOMMONCONSTDESC::CCD_FLOAT3},
	{":DirPower",								MEMBER_OFFSET(COMMON, DirPower), A3DCOMMONCONSTDESC::CCD_FLOAT},
	{":Ambient0",								MEMBER_OFFSET(COMMON, Ambient0), A3DCOMMONCONSTDESC::CCD_COLORVAL},
	{":Ambient1",								MEMBER_OFFSET(COMMON, Ambient1), A3DCOMMONCONSTDESC::CCD_COLORVAL},
	{":PointDiffuse",							MEMBER_OFFSET(COMMON, PtDiffuse), A3DCOMMONCONSTDESC::CCD_COLORVAL},
	{":PointPos",								MEMBER_OFFSET(COMMON, PtPosition), A3DCOMMONCONSTDESC::CCD_FLOAT3},
	{":PointPosInView",							MEMBER_OFFSET(COMMON, PtPosInView), A3DCOMMONCONSTDESC::CCD_FLOAT3},
	{":PointAttenRange",						MEMBER_OFFSET(COMMON, ptAttenRange), A3DCOMMONCONSTDESC::CCD_FLOAT3},

	{":FogParam1",							    MEMBER_OFFSET(COMMON, vFogParam1), A3DCOMMONCONSTDESC::CCD_FLOAT4},
    {":FogParam2",							    MEMBER_OFFSET(COMMON, vFogParam2), A3DCOMMONCONSTDESC::CCD_FLOAT4},
    {":FogColor1",		 						MEMBER_OFFSET(COMMON, vFogColor1), A3DCOMMONCONSTDESC::CCD_FLOAT4},
    {":FogColor2",		 						MEMBER_OFFSET(COMMON, vFogColor2), A3DCOMMONCONSTDESC::CCD_FLOAT4},

	{":TimePack",								MEMBER_OFFSET(COMMON, vTime), A3DCOMMONCONSTDESC::CCD_FLOAT4},
	{":ViewportPack",							MEMBER_OFFSET(COMMON, vViewportDim), A3DCOMMONCONSTDESC::CCD_FLOAT4},
	{":ViewPosition",							MEMBER_OFFSET(COMMON, vViewPosition), A3DCOMMONCONSTDESC::CCD_FLOAT4},
	{":ViewDirection",							MEMBER_OFFSET(COMMON, vViewDirection), A3DCOMMONCONSTDESC::CCD_FLOAT4},

	{":ViewProjection",							MEMBER_OFFSET(COMMON, matViewProj), A3DCOMMONCONSTDESC::CCD_FLOAT4X4},
	{":ViewProjectionInverse",					MEMBER_OFFSET(COMMON, matViewProjInv), A3DCOMMONCONSTDESC::CCD_FLOAT4X4},
	{":View",									MEMBER_OFFSET(COMMON, matView), A3DCOMMONCONSTDESC::CCD_FLOAT4X4},
    {":ViewInverse",        					MEMBER_OFFSET(COMMON, matViewInv), A3DCOMMONCONSTDESC::CCD_FLOAT4X4},
	{":Projection",								MEMBER_OFFSET(COMMON, matProj), A3DCOMMONCONSTDESC::CCD_FLOAT4X4},
	{":ProjectionInverse",						MEMBER_OFFSET(COMMON, matProjInv), A3DCOMMONCONSTDESC::CCD_FLOAT4X4},
	{":WorldViewProjection",					MEMBER_OFFSET(COMMON, matWorldViewProj), A3DCOMMONCONSTDESC::CCD_FLOAT4X4},
	{":WorldViewProjectionInverse",				MEMBER_OFFSET(COMMON, matWorldViewProjInv), A3DCOMMONCONSTDESC::CCD_FLOAT4X4},
	{":World",									MEMBER_OFFSET(COMMON, matWorld), A3DCOMMONCONSTDESC::CCD_FLOAT4X4},
	{":WorldInverse",							MEMBER_OFFSET(COMMON, matWorldInv), A3DCOMMONCONSTDESC::CCD_FLOAT4X4},
	{":WorldView",								MEMBER_OFFSET(COMMON, matWorldView), A3DCOMMONCONSTDESC::CCD_FLOAT4X4},
	{":WorldViewInverse",						MEMBER_OFFSET(COMMON, matWorldViewInv), A3DCOMMONCONSTDESC::CCD_FLOAT4X4},
	//{":ViewProjectionInverseTranspose",		MEMBER_OFFSET(COMMON, matViewProjInvTran), A3DCOMMONCONSTDESC::CCD_FLOAT4X4},
	//{":ViewProjectionTranspose",				MEMBER_OFFSET(COMMON, matViewProjTran), A3DCOMMONCONSTDESC::CCD_FLOAT4X4},
	//{":ViewTranspose",						MEMBER_OFFSET(COMMON, matViewTran), A3DCOMMONCONSTDESC::CCD_FLOAT4X4},
	//{":ViewInverseTranspose",					MEMBER_OFFSET(COMMON, matViewInvTran), A3DCOMMONCONSTDESC::CCD_FLOAT4X4},
	//{":ProjectionTranspose",					MEMBER_OFFSET(COMMON, matProjTran), A3DCOMMONCONSTDESC::CCD_FLOAT4X4},
	//{":ProjectionInverseTranspose",			MEMBER_OFFSET(COMMON, matProjInvTran), A3DCOMMONCONSTDESC::CCD_FLOAT4X4},
	//{":WorldViewProjectionTranspose",			MEMBER_OFFSET(COMMON, matWorldViewProjTran), A3DCOMMONCONSTDESC::CCD_FLOAT4X4},
	//{":WorldViewProjectionInverseTranspose",	MEMBER_OFFSET(COMMON, matWorldViewProjInvTran), A3DCOMMONCONSTDESC::CCD_FLOAT4X4},
	//{":WorldTranspose",						MEMBER_OFFSET(COMMON, matWorldTran), A3DCOMMONCONSTDESC::CCD_FLOAT4X4},
	//{":WorldInverseTranspose",				MEMBER_OFFSET(COMMON, matWorldInvTran), A3DCOMMONCONSTDESC::CCD_FLOAT4X4},
	//{":WorldViewTranspose",					MEMBER_OFFSET(COMMON, matWorldViewTran), A3DCOMMONCONSTDESC::CCD_FLOAT4X4},
	//{":WorldViewInverseTranspose",			MEMBER_OFFSET(COMMON, matWorldViewInvTran), A3DCOMMONCONSTDESC::CCD_FLOAT4X4},
    {":EyePos",                 				MEMBER_OFFSET(COMMON, vEyePos), A3DCOMMONCONSTDESC::CCD_FLOAT3},
    {":ProjectionRatio",           				MEMBER_OFFSET(COMMON, vProjectionRatio), A3DCOMMONCONSTDESC::CCD_FLOAT4},
    {":PosExtendToNormal",         				MEMBER_OFFSET(COMMON, fPosExtendToNormal), A3DCOMMONCONSTDESC::CCD_FLOAT},

	{":GlobalUseShadowMap",           		MEMBER_OFFSET(COMMON, fGlobalUseShadowMap), A3DCOMMONCONSTDESC::CCD_FLOAT},
	{":GlobalUseHWPCF",           			MEMBER_OFFSET(COMMON, fGlobalUseHWPCF), A3DCOMMONCONSTDESC::CCD_FLOAT},
	{":GlobalShadowMapSize",           		MEMBER_OFFSET(COMMON, fGlobalShadowMapSize), A3DCOMMONCONSTDESC::CCD_FLOAT},
	{":GlobalShadowSampleNum",				MEMBER_OFFSET(COMMON, fGlobalShadowSampleNum), A3DCOMMONCONSTDESC::CCD_FLOAT},
	{":GlobalShadowMap",           			MEMBER_OFFSET(COMMON, pGlobalShadowMap), A3DCOMMONCONSTDESC::CCD_SAMPLER},
	{":GlobalShadowMapFloat",           	MEMBER_OFFSET(COMMON, pGlobalShadowMapFloat), A3DCOMMONCONSTDESC::CCD_SAMPLER},
	{":GlobalLightMapParam",           		MEMBER_OFFSET(COMMON, vGlobalLightMapParam), A3DCOMMONCONSTDESC::CCD_FLOAT4},
	
	{":LitModelUseLightMap",           			MEMBER_OFFSET(COMMON, LitModelUseLightMap), A3DCOMMONCONSTDESC::CCD_FLOAT},
	{":LitModelUseVerlit",           			MEMBER_OFFSET(COMMON, LitModelUseVerlit), A3DCOMMONCONSTDESC::CCD_FLOAT},
	{":LitModelAlpha",           				MEMBER_OFFSET(COMMON, LitModelAlpha), A3DCOMMONCONSTDESC::CCD_FLOAT},
	{":LitModelDiffuseTexture",           		MEMBER_OFFSET(COMMON, LitModelDiffuseTexture), A3DCOMMONCONSTDESC::CCD_SAMPLER},
	{":LitModelLightMap1",           			MEMBER_OFFSET(COMMON, LitModelLightMap1), A3DCOMMONCONSTDESC::CCD_SAMPLER},
	{":LitModelLightMap2",           			MEMBER_OFFSET(COMMON, LitModelLightMap2), A3DCOMMONCONSTDESC::CCD_SAMPLER},

// 	IDirect3DBaseTexture9* LitModelShadowMap;
// 	IDirect3DBaseTexture9* LitModelDiffuseTexture;
// 	IDirect3DBaseTexture9* LitModelLightMap1;
// 	IDirect3DBaseTexture9* LitModelLightMap2;
// 	float			LitModelUseShadowMap;
// 	float			LitModelUseHWPCF;
// 	float			LitModelUseLightMap;
// 	float			LitModelUseVerlit;
// 	A3DVECTOR4		LitModelLightMapParam;
//	float			LitModelAlpha;
//	float			LitModelShadowMapSize;


    {NULL,0}

};

A3DEnvironment::A3DEnvironment()
{
	m_DirLight.Diffuse   = 0xffffffff;
	m_DirLight.DirPower  = 1.0f;
	m_DirLight.Ambient0  = 0xff808080;
	m_DirLight.Ambient1  = 0xff404040;
	m_DirLight.Direction = A3DVECTOR3(1,-1,1);

	m_PointLight.Diffuse      = 0xffffffff;
	m_PointLight.Position     = A3DVECTOR3(1,1,1);
	m_PointLight.Attenuation0 = 1.0f;
	m_PointLight.Attenuation1 = 3.0f;
	m_PointLight.Range        = 100.0f;

    m_LightMap.MaxLight = 2.0f;
    m_LightMap.MaxLightNight = 2.0f;
    m_LightMap.Gamma = 1.0f;

    m_FogParamUnderWater.bGlobalFogEnable = false;
    m_FogParamUnderWater.crFog1 = 0xFFFFFFFF;
    m_FogParamUnderWater.crFog2 = 0xFF0000FF;
    m_FogParamUnderWater.fFogStart = 10.0f;
    m_FogParamUnderWater.fFogEnd = 800.0f;
    m_FogParamUnderWater.fDensity1 = 2.0f;
    m_FogParamUnderWater.fDensity2 = 0.0f;
    m_FogParamUnderWater.fColorIntensity1 = 1.0f;
    m_FogParamUnderWater.fColorIntensity2 = 1.0f;
    m_FogParamUnderWater.fHorzDensity = 1.0f;
    m_FogParamUnderWater.fHorzPower = 1.0f;
    m_FogParamUnderWater.fHeightStart = 0.0f;
    m_FogParamUnderWater.fHeightEnd = 300.0f;
    m_FogParamUnderWater.fHeightDensity = 1.0f;

    m_FogParamAboveWater = m_FogParamUnderWater;
    m_bIsUnderWater = false;
    m_fDNFactor = 0;

    UpdateFogParam();
	SetDirLightParam(&m_DirLight);
	SetPointLightParam(&m_PointLight);
    SetLightMapParam(&m_LightMap);

	m_Common.matViewProj.Identity();
	m_Common.matViewProjInv.Identity();
	m_Common.matView.Identity();
	m_Common.matViewInv.Identity();
	m_Common.matProj.Identity();
	m_Common.matProjInv.Identity();
	m_Common.matWorldViewProj.Identity();
	m_Common.matWorldViewProjInv.Identity();
	m_Common.matWorld.Identity();
	m_Common.matWorldInv.Identity();
	m_Common.matWorldView.Identity();
	m_Common.matWorldViewInv.Identity();
    m_Common.vProjectionRatio.Set(1, 0, 0, 0);
    m_Common.fPosExtendToNormal = 0;
	//m_Common.matViewProjInvTran.Identity();
	//m_Common.matViewProjTran.Identity();
	//m_Common.matViewTran.Identity();
	//m_Common.matViewInvTran.Identity();
	//m_Common.matProjTran.Identity();
	//m_Common.matProjInvTran.Identity();
	//m_Common.matWorldViewProjTran.Identity();
	//m_Common.matWorldViewProjInvTran.Identity();
	//m_Common.matWorldTran.Identity();
	//m_Common.matWorldInvTran.Identity();
	//m_Common.matWorldViewTran.Identity();
	//m_Common.matWorldViewInvTran.Identity();
    m_Common.pGlobalShadowMap = NULL;
	m_Common.pGlobalShadowMapFloat = NULL;
    m_Common.LitModelLightMap1 = NULL;
    m_Common.LitModelLightMap2 = NULL;
    m_Common.LitModelDiffuseTexture = NULL;
}

A3DEnvironment::~A3DEnvironment()
{

}

bool A3DEnvironment::Init()
{
	m_pCCDBinder = A3DHLSL::CreateCommonConstDataBinder(s_aEnvCommConst, sizeof(COMMON));
	if(m_pCCDBinder == NULL)  {
		g_A3DErrLog.Log("A3DEnvironment::Init, failed to create common const data binder!");
		return false;
	}
	return true;
}

bool A3DEnvironment::Release()
{
	A3DRELEASE(m_pCCDBinder);
	return true;
}

bool A3DEnvironment::Tick()
{
	DWORD dwTimeLoop = a_GetTime() % 10000;
	m_Common.vTime.x = (float)dwTimeLoop * 0.001f;
	float fTime = m_Common.vTime.x * A3D_PI;
	m_Common.vTime.y = cos(fTime);
	m_Common.vTime.z = sin(fTime);
	m_Common.vTime.w = tan(fTime);
	return true;
}

bool A3DEnvironment::UpdateFogParam()
{
    m_FogParam = m_bIsUnderWater ? m_FogParamUnderWater : m_FogParamAboveWater;

    float fRcpDist = m_FogParam.fFogEnd - m_FogParam.fFogStart;
    fRcpDist = fRcpDist <= 0 ? 0 : 1 / fRcpDist;
    float fHeight = m_FogParam.fHeightEnd - m_FogParam.fHeightStart;
    float fHeightRcpDensity = fHeight <= 0 ? 0 : m_FogParam.fHeightDensity / fHeight;

	m_Common.vFogParam1.Set(m_FogParam.fFogStart, fRcpDist, m_FogParam.fHeightEnd, fHeightRcpDensity);

	m_Common.vFogParam2.Set(m_FogParam.fDensity1, m_FogParam.fDensity2, m_FogParam.fHorzDensity, m_FogParam.fHorzPower);

	m_Common.vFogColor1.Set(
		((m_FogParam.crFog1 & 0x00FF0000) >> 16) / 255.0f,
		((m_FogParam.crFog1 & 0x0000FF00) >> 8) / 255.0f,
		((m_FogParam.crFog1 & 0x000000FF) >> 0) / 255.0f,
		m_FogParam.bGlobalFogEnable ? m_FogParam.fColorIntensity1 : 0.0f
		);

    m_Common.vFogColor2.Set(
        ((m_FogParam.crFog2 & 0x00FF0000) >> 16) / 255.0f,
        ((m_FogParam.crFog2 & 0x0000FF00) >> 8) / 255.0f,
        ((m_FogParam.crFog2 & 0x000000FF) >> 0) / 255.0f,
        m_FogParam.bGlobalFogEnable ? m_FogParam.fColorIntensity2 : 0.0f
        );

    return true;
}

bool A3DEnvironment::SetFogParamUnderWater(const FOG* pFog)
{
    if(pFog != &m_FogParamUnderWater)
    {
        memcpy(&m_FogParamUnderWater, pFog, sizeof(FOG));
    }
    if (m_bIsUnderWater)
        UpdateFogParam();
    return true;
}

bool A3DEnvironment::SetFogParamAboveWater(const FOG* pFog)
{
    if(pFog != &m_FogParamAboveWater)
    {
        memcpy(&m_FogParamAboveWater, pFog, sizeof(FOG));
    }
    if (!m_bIsUnderWater)
        UpdateFogParam();
    return true;
}

const A3DEnvironment::FOG* A3DEnvironment::GetFogParam()
{
	return &m_FogParam;
}

const A3DEnvironment::FOG* A3DEnvironment::GetFogParamUnderWater()
{
    return &m_FogParamUnderWater;
}

const A3DEnvironment::FOG* A3DEnvironment::GetFogParamAboveWater()
{
    return &m_FogParamAboveWater;
}

bool A3DEnvironment::SetDirLightParam(const DIRLIGHT* pDirLight)
{
	if(pDirLight != &m_DirLight)  {
		memcpy(&m_DirLight, pDirLight, sizeof(DIRLIGHT));
	}
	m_Common.DirDiffuse		= m_DirLight.Diffuse;
	m_Common.DirPower		= m_DirLight.DirPower;
	m_Common.Ambient0		= m_DirLight.Ambient0;
	m_Common.Ambient1		= m_DirLight.Ambient1;
	m_Common.LightDirection = m_DirLight.Direction;
	m_Common.LightDirection.Normalize();
	m_Common.LightDirInView = a3d_VectorMatrix3x3(m_Common.LightDirection, m_Common.matView.GetTranspose());
	return true;
}

const A3DEnvironment::DIRLIGHT* A3DEnvironment::GetDirLightParam()
{
	return &m_DirLight;
}

bool A3DEnvironment::SetPointLightParam(const POINTLIGHT* pPointLight)
{
	if(pPointLight != &m_PointLight)  {
		memcpy(&m_PointLight, pPointLight, sizeof(POINTLIGHT));
	}
	m_Common.PtDiffuse      = m_PointLight.Diffuse;
	m_Common.PtPosition     = m_PointLight.Position;
	m_Common.ptAttenRange.x = m_PointLight.Attenuation0;
	m_Common.ptAttenRange.y = m_PointLight.Attenuation1;
	m_Common.ptAttenRange.z = m_PointLight.Range;

	m_Common.PtPosInView = m_Common.PtPosition * m_Common.matView.GetTranspose();

	return true;
}

const A3DEnvironment::POINTLIGHT* A3DEnvironment::GetPointLightParam()
{
	return &m_PointLight;
}

bool A3DEnvironment::SetLightMapParam(const A3DEnvironment::LITMAP* pLitMap)
{
    if(pLitMap != &m_LightMap)  {
        memcpy(&m_LightMap, pLitMap, sizeof(LITMAP));
    }
    return true;
}

const A3DEnvironment::LITMAP* A3DEnvironment::GetLightMapParam()
{
    return &m_LightMap;
}

bool A3DEnvironment::SetViewport(A3DViewport* pA3DViewport, const A3DMATRIX4* pmatWorld)
{
	A3DCameraBase* pCamera = pA3DViewport->GetCamera();
	m_Common.matView				= pCamera->GetViewTM().GetTranspose();
	m_Common.matProj				= pCamera->GetProjectionTM().GetTranspose();
	m_Common.matViewProj			= pCamera->GetVPTM().GetTranspose();

	m_Common.matViewInv				= m_Common.matView.GetInverseTM();
	m_Common.matProjInv				= m_Common.matProj.GetInverseTM();
	m_Common.matViewProjInv			= m_Common.matViewProj.GetInverseTM();

	if(pmatWorld == NULL)
	{
		m_Common.matWorldView			= m_Common.matView;
		m_Common.matWorldViewProj		= m_Common.matViewProj;

		m_Common.matWorld.Identity();	// [单位矩阵]
		m_Common.matWorldInv.Identity();	// [单位矩阵]

		m_Common.matWorldViewProjInv	= m_Common.matWorldViewProj.GetInverseTM();
		m_Common.matWorldViewInv		= m_Common.matWorldView.GetInverseTM();
	}
	else
	{
		SetWorldMatrix(*pmatWorld);
	}

	A3DVIEWPORTPARAM* paramVP = pA3DViewport->GetParam();
	m_Common.LightDirInView = a3d_VectorMatrix3x3(m_Common.LightDirection, pCamera->GetViewTM());
	m_Common.PtPosInView = m_Common.PtPosition * pCamera->GetViewTM();
	m_Common.vEyePos        = pCamera->GetPos();
	m_Common.vViewDirection = pCamera->GetDir();
	m_Common.vViewPosition  = pCamera->GetPos();
	m_Common.vViewportDim.x = (float)paramVP->Width;
	m_Common.vViewportDim.y = (float)paramVP->Height;
	m_Common.vViewportDim.z = m_Common.vViewportDim.x / m_Common.vViewportDim.y;
	m_Common.vViewportDim.w = 1.0f / m_Common.vViewportDim.z;
    float fZFar = pCamera->GetZBack();
    float fZNear = pCamera->GetZFront();
    m_Common.vProjectionRatio.Set(fZFar / (fZFar - fZNear), fZNear / (fZNear - fZFar), 0, 0);

	return true;
}

bool A3DEnvironment::SetViewport(A3DViewport* pA3DViewport)
{
	return SetViewport(pA3DViewport, NULL);
}

bool A3DEnvironment::SetWorldMatrix(const A3DMATRIX4& matWorld)
{
	m_Common.matWorld				= matWorld.GetTranspose();
	m_Common.matWorldInv			= m_Common.matWorld.GetInverseTM();

	// 转制矩阵的相乘
	m_Common.matWorldView			= m_Common.matView * m_Common.matWorld;
	m_Common.matWorldViewInv		= m_Common.matWorldView.GetInverseTM();

	m_Common.matWorldViewProj		= m_Common.matViewProj * m_Common.matWorld;
	m_Common.matWorldViewProjInv	= m_Common.matWorldViewProj.GetInverseTM();

	return true;
}

bool A3DEnvironment::SetLightInfo(const A3DSkinModel::LIGHTINFO& LightInfo)
{
	m_Common.PtPosition = LightInfo.vPtLightPos;
	m_Common.PtDiffuse = LightInfo.colPtDiff;		//	Point light's ambient color
	m_Common.ptAttenRange = A3DVECTOR3(LightInfo.vPtAtten.x, LightInfo.vPtAtten.y, LightInfo.fPtRange);

	m_Common.PtPosInView = m_Common.PtPosition * m_Common.matView.GetTranspose();
	return true;
}

void A3DEnvironment::SetIsUnderWater(bool bUnder)
{
    m_bIsUnderWater = bUnder;
    UpdateFogParam();
}

bool A3DEnvironment::SetGlobalFogEnable(bool bEnable)
{
    if (m_bIsUnderWater)
        m_FogParamUnderWater.bGlobalFogEnable = bEnable;
    else
        m_FogParamAboveWater.bGlobalFogEnable = bEnable;
    UpdateFogParam();
    return true;
}

void A3DEnvironment::SetDNFactor(float fFactor)
{
    m_fDNFactor = fFactor;
}


bool A3DEnvironment::SetLightMapShadowGlobalParam( const LIGHTMAP_SHADOW_GLOBAL* pLitModelGlobal )
{
	if(pLitModelGlobal != &m_LitModelGlobal)
	{
		memcpy(&m_LitModelGlobal, pLitModelGlobal, sizeof(LIGHTMAP_SHADOW_GLOBAL));
	}
	m_Common.fGlobalUseShadowMap = m_LitModelGlobal.bShadowMap ? 1.f : 0.f;
	m_Common.fGlobalUseHWPCF = m_LitModelGlobal.bHWPCF ? 1.f : 0.f;
	m_Common.vGlobalLightMapParam = m_LitModelGlobal.vLitMapParam;
	m_Common.fGlobalShadowMapSize = m_LitModelGlobal.fShadowMapSize;
	m_Common.fGlobalShadowSampleNum = (float)pLitModelGlobal->nShadowSampleNum;
	m_Common.pGlobalShadowMap = m_LitModelGlobal.LitModelShadowMap;
	m_Common.pGlobalShadowMapFloat = m_LitModelGlobal.LitModelShadowMapFloat;
	return true;
}

const A3DEnvironment::LIGHTMAP_SHADOW_GLOBAL* A3DEnvironment::GetLightMapShadowGlobalParam()
{
	return &m_LitModelGlobal;
}

bool A3DEnvironment::SetLitModelIndividualParam( const LITMODEL_INDIVIDUAL* pLitModel )
{
	if(pLitModel != &m_LitModelIndividual)
	{
		memcpy(&m_LitModelIndividual, pLitModel, sizeof(LITMODEL_INDIVIDUAL));
	}
	m_Common.LitModelUseLightMap = m_LitModelIndividual.bLightMap ? 1.f : 0.f;
	m_Common.LitModelUseVerlit = m_LitModelIndividual.bVertLit ? 1.f : 0.f;
	m_Common.LitModelAlpha = m_LitModelIndividual.fAlpha;
	m_Common.LitModelDiffuseTexture = m_LitModelIndividual.LitModelDiffuseTexture;
	m_Common.LitModelLightMap1 = m_LitModelIndividual.LitModelLightMap1;
	m_Common.LitModelLightMap2 = m_LitModelIndividual.LitModelLightMap2;
	return true;
}

const A3DEnvironment::LITMODEL_INDIVIDUAL* A3DEnvironment::GetLitModelIndividualParam()
{
	return &m_LitModelIndividual;
}

//const float rad = D3DX_PI / 180.0f;
//
//A3D_PI
//////////////////////////////////////////////////////////////////////////
const float c_lumR = 0.3086f;
const float c_lumG = 0.6094f;
const float c_lumB = 0.0820f;


void A3DColorTone::ShearColor(A3DMATRIX4& matrix,int whichColor,int color1,int color2)
//void CDirectxps_hslView::ShearColor(D3DXMATRIX& matrix,int whichColor,float color1,float color2)
{
	A3DMATRIX4 mShearBlue;
	mShearBlue.Identity();
	//D3DXMatrixIdentity(&mShearBlue);

	switch(whichColor)
	{
	case 0:// 去掉红色
		mShearBlue.m[1][0] = (float)color1;
		mShearBlue.m[2][0] = (float)color2;
		break;
	case 1:// 剔除绿色
		mShearBlue.m[0][1] = (float)color1;
		mShearBlue.m[2][1] = (float)color2;
		break;
	case 2:// 剔除蓝色
		mShearBlue.m[0][2] = (float)color1;
		mShearBlue.m[1][2] = (float)color2;
		break;
	default:
		mShearBlue.m[1][0] = (float)color1;
		mShearBlue.m[2][0] = (float)color2;
		break;
	}


	mShearBlue = mShearBlue * mShearBlue;
	//D3DXMatrixMultiply(&matrix,&matrix,&mShearBlue);

}
//const float rad = D3DX_PI / 180.0f;
void A3DColorTone::RotateColor(A3DMATRIX4 &matrix,float phi,int axiIndex)
{
	phi = DEG2RAD(phi);

	A3DMATRIX4 m;
	switch(axiIndex)
	{
	case 0:// x轴
		{
			m.RotateX(phi);
			//D3DXMatrixRotationX(&m,phi);
		}
		break;
	case 1:// y轴
		{
			m.RotateY(phi);
			//D3DXMatrixRotationY(&m,phi);
		}
		break;
	case 2:// z轴
		{
			m.RotateZ(phi);
			//D3DXMatrixRotationZ(&m,phi);
		}
		break;
	default:
		{
			m.RotateX(phi);
			//D3DXMatrixRotationX(&m,phi);
		}
	}
	matrix = matrix * m;
	//D3DXMatrixMultiply(&matrix,&matrix,&m);
}
void A3DColorTone::InitHue()
{
	const float greenRotation = 35.0f;
	//D3DXMatrixIdentity(&m_preHueMatrix);
	m_preHueMatrix.Identity();

	RotateColor(m_preHueMatrix,45.0,0);
	RotateColor(m_preHueMatrix,-greenRotation,1);

	A3DVECTOR3 lum;
	lum.x = c_lumR;
	lum.y = c_lumG;
	lum.z = c_lumB;

	//A3DTranso
	//m_preHueMatrix.tr

	lum = m_preHueMatrix * lum;

	//D3DXVec3TransformCoord(&lum,&lum,&m_preHueMatrix);

	float red = lum.x / lum.z;
	float green = lum.y / lum.z;

	ShearColor(m_preHueMatrix,2,(int)red,(int)green);

	m_postHueMatrix.Identity();
	//D3DXMatrixIdentity(&m_postHueMatrix);

	ShearColor(m_postHueMatrix,2,(int)-red,(int)-green);

	RotateColor(m_postHueMatrix,greenRotation,1);

	RotateColor(m_postHueMatrix,-45.0,0);
}

void A3DColorTone::ComputeColorMatrix(A3DMATRIX4& matrix, const A3DCOLORVALUE& crFrom, const A3DCOLORVALUE& crTo)
{
	float h0, s0, l0, h, s, l;
	a3d_RGBToHSL(crFrom.r, crFrom.g, crFrom.b, h0, s0, l0);
	a3d_RGBToHSL(crTo.r, crTo.g, crTo.b, h, s, l);

	matrix.Identity();
	//D3DXMatrixIdentity(&matrix);

	/*
	D3DXMATRIX grayMatrix;
	D3DXMatrixIdentity(&grayMatrix);
	for(int i = 0;i < 3;i++)
	{
		grayMatrix.m[0][i] = lumR;
		grayMatrix.m[1][i] = lumG;
		grayMatrix.m[2][i] = lumB;
	}
	// 是否变灰色
	if(m_toolPanelDlg.m_bGray)
	{
		D3DXMatrixMultiply(&matrix,&grayMatrix,&matrix);
	}*/

	// 饱和度
	float fSaturation = 0;//(m_toolPanelDlg.m_nSaturation + 100)/100.0f;
	fSaturation = 1+(s-s0);
	if(s-s0 > 0)
		fSaturation = fSaturation*fSaturation;
	float satCompl = 1.0f - fSaturation;
	float satComplR = c_lumR * satCompl;
	float satComplG = c_lumG * satCompl;
	float satComplB = c_lumB * satCompl;

	
	A3DMATRIX4 mSaturation;
	//D3DXMatrixIdentity(&mSaturation);
	mSaturation.Identity();
	mSaturation.m[0][0] = satComplR + fSaturation;
	mSaturation.m[0][1] = satComplR;
	mSaturation.m[0][2] = satComplR;
	mSaturation.m[1][0] = satComplG;
	mSaturation.m[1][1] = satComplG + fSaturation;
	mSaturation.m[1][2] = satComplG;
	mSaturation.m[2][0] = satComplB;
	mSaturation.m[2][1] = satComplB;
	mSaturation.m[2][2] = satComplB+ fSaturation;

	//D3DXMatrixMultiply(&matrix,&mSaturation,&matrix);
	matrix = mSaturation * matrix;
	


	// 色相矩阵
	
	matrix = matrix * m_preHueMatrix;
	//D3DXMatrixMultiply(&matrix,&matrix,&m_preHueMatrix);
	//RotateColor(matrix,m_toolPanelDlg.m_nHue,2);
	RotateColor(matrix, (h-h0)*360.0f,2);
	//D3DXMatrixMultiply(&matrix,&matrix,&m_postHueMatrix);
	matrix = matrix * m_postHueMatrix;
	
	float dl = l-l0;//m_toolPanelDlg.m_nBright*0.01f;
	matrix._41 += dl;
	matrix._42 += dl;
	matrix._43 += dl;
}