/*
* FILE: A3DTerrain2RenderHLSL.cpp
*
* DESCRIPTION: terrain 2 render function with HLSL method
*
* CREATED BY: Zhangyachuan (zhangyachuan000899@wanmei.com), 2012/2/13
*
* HISTORY: 
*
* Copyright (c) 2012 Archosaur Studio, All Rights Reserved.
*/

#include "A3DPlatform.h"
#include "A3DTerrain2.h"
#include "A3DTerrain2Render.h"
#include "A3DHLSL.h"
#include "A3DDevice.h"
#include "A3DEngine.h"
#include "A3DTerrain2Blk.h"
#include "A3DTerrain2Env.h"
#include "A3DViewport.h"
#include "A3DVertexShader.h"
#include "A3DTexture.h"
#include "A3DCameraBase.h"
#include "A3DPI.h"
#include "A3DShadowMap.h"
#include "A3DEnvironment.h"
#include "A3DTextureMan.h"
#include "A3DStream.h"
#include <vector>
#include <map>

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

#define new A_DEBUG_NEW

///////////////////////////////////////////////////////////////////////////
//	
//	Reference to External variables and functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local Types and Variables and Global variables
//	
///////////////////////////////////////////////////////////////////////////

namespace {
template <int LAYERNUM>
class TerrainMtlParams
{
public:
	
	TerrainMtlParams(A3DTerrain2Render* pTrnRender)
		: m_pTrnRender(pTrnRender)
		, m_pBlackTex(pTrnRender->GetBlackTex())
		, m_pColTexture(NULL)
	{
		memset(m_aEnableFlags, 0, sizeof(m_aEnableFlags));
		memset(m_aProjAxis, 0, sizeof(m_aProjAxis));
		memset(m_aScaleU, 0, sizeof(m_aScaleU));
		memset(m_aScaleV, 0, sizeof(m_aScaleV));
		memset(m_aTextures, 0, sizeof(m_aTextures));
		memset(m_aDNLightMaps, 0, sizeof(m_aDNLightMaps));
	}

	bool Init(A3DTerrain2Mask* pMask);
	bool Fill(A3DHLSL* pShader, A3DTexture* pErrorTexture);

private:

	void UpdateShaderParam3f(A3DHLSL* pHLSL, const char* szName, const float* aParams)
	{
		A3DVECTOR3 v(aParams[0], aParams[1], aParams[2]);
		pHLSL->SetValue3f(szName, &v);
	}

private:
	
	A3DTerrain2Render* m_pTrnRender;
	A3DTexture* m_pBlackTex;
	float m_aEnableFlags[LAYERNUM];
	float m_aProjAxis[LAYERNUM];
	float m_aScaleU[LAYERNUM];
	float m_aScaleV[LAYERNUM];
	A3DTexture* m_aTextures[LAYERNUM];
	A3DTexture* m_aMaskTexs[LAYERNUM / 3];
	A3DTexture* m_aDNLightMaps[2];
	A3DTexture* m_pColTexture;

};

template <int LAYERNUM>
bool TerrainMtlParams<LAYERNUM>::Init(A3DTerrain2Mask* pMask)
{
	int iLayerCnt = a_Min(pMask->GetLayerNum(), LAYERNUM);

	for (int i = 0; i < LAYERNUM; ++i)
	{
		m_aProjAxis[i] = 0.0f;
		m_aScaleU[i] = m_aScaleV[i] = 1.0f;

		if (i >= iLayerCnt)
		{
			m_aEnableFlags[i] = 0.0f;
			m_aTextures[i] = m_pBlackTex;
			continue;
		}

		const A3DTerrain2Mask::LAYER& layer = pMask->GetLayer(i);
		m_aProjAxis[i] = layer.byProjAxis;
		m_aScaleU[i] = layer.fUScale;
		m_aScaleV[i] = layer.fVScale;
		m_aEnableFlags[i] = 1.0f;
		m_aTextures[i] = pMask->GetTexture(i);
	}

	int iMaskNum = LAYERNUM / 3;
	for (int i = 0; i < iMaskNum; ++i)
	{
		m_aMaskTexs[i] = pMask->GetMaskTexture(i);
	}

	if (m_pTrnRender->GetTerrain()->GetLightEnableFlag())
	{
		m_aDNLightMaps[0] = pMask->GetLightMap(true);
		m_aDNLightMaps[1] = pMask->GetLightMap(false);
	}
	else
	{
		m_aDNLightMaps[0] = m_aDNLightMaps[1] = A3D::g_pA3DTerrain2Env->GetWhiteTexture();
	}

	m_pColTexture = pMask->GetDetailMap() ? pMask->GetDetailMap() : A3D::g_pA3DTerrain2Env->GetUnitWhiteTexture();

	a_Swap(m_aEnableFlags[0], m_aEnableFlags[2]);

	if (LAYERNUM > 3)
		a_Swap(m_aEnableFlags[3], m_aEnableFlags[5]);

	return true;
}

template <int LAYERNUM>
bool TerrainMtlParams<LAYERNUM>::Fill(A3DHLSL* pShader, A3DTexture* pErrorTexture)
{
	AString strSampler;
	for (int i = 0; i < LAYERNUM; ++i)
	{
		strSampler.Format("g_BaseSampler%d", i);
        pShader->SetTexture(strSampler, (m_aTextures[i] && m_aTextures[i]->GetD3DTexture()) ? m_aTextures[i] : pErrorTexture);
	}

	UpdateShaderParam3f(pShader, "g_vProjAxis0", m_aProjAxis);
	UpdateShaderParam3f(pShader, "g_LayerEnableFlags0", m_aEnableFlags);
	UpdateShaderParam3f(pShader, "g_vScaleU0", m_aScaleU);
	UpdateShaderParam3f(pShader, "g_vScaleV0", m_aScaleV);

	for (int i = 0; i < _countof(m_aMaskTexs); ++i)
	{
		strSampler.Format("g_MaskMapSampler%d", i);
		pShader->SetTexture(strSampler, m_aMaskTexs[i]);
	}

	pShader->SetTexture("g_LMDaySampler", m_aDNLightMaps[0]);
	pShader->SetTexture("g_LMNightSampler", m_aDNLightMaps[1]);
	pShader->SetTexture("g_ColTexSampler", m_pColTexture);

	if (LAYERNUM > 3)
	{
		UpdateShaderParam3f(pShader, "g_vProjAxis1", m_aProjAxis + 3);
		UpdateShaderParam3f(pShader, "g_LayerEnableFlags1", m_aEnableFlags + 3);
		UpdateShaderParam3f(pShader, "g_vScaleU1", m_aScaleU + 3);
		UpdateShaderParam3f(pShader, "g_vScaleV1", m_aScaleV + 3);
	}

	return true;
}

};

///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////


TerrainRenderSlotRec::~TerrainRenderSlotRec()
{
    for (size_t i = 0; i < aBlocks.size(); i++) 
        delete aBlocks[i];
}

TerrainShaderSlotRec::~TerrainShaderSlotRec()
{
    for (size_t i = 0; i < aSlots.size(); i++) 
        delete aSlots[i];
}

static void SetCommonParams(A3DHLSL* pHLSL, A3DDevice* pA3DDevice, A3DTerrain2* pTerrain, A3DViewport* pViewport)
{
	using namespace A3D;
	const A3DMaterial& trnMaterial = pTerrain->GetTerrainMaterial();
	pHLSL->SetValue3f("g_vecMainLightDir", &pTerrain->GetMainLightParam().Direction);
	pHLSL->SetValue3f("g_vecEyePos", &pViewport->GetCamera()->GetPos());
	pHLSL->SetValue4f("g_colMtlDiffuse", (const A3DVECTOR4*)&trnMaterial.GetDiffuse());
	pHLSL->SetValue4f("g_colMtlSpecular", (const A3DVECTOR4*)&trnMaterial.GetSpecular());
    A3DEnvironment* pEnv = pA3DDevice->GetA3DEngine()->GetA3DEnvironment();
	A3DVECTOR4 vLightMapParams(pEnv->GetLightMapParam()->MaxLight, pEnv->GetLightMapParam()->MaxLightNight, 0, pTerrain->GetDNFactor());
    pHLSL->SetValue4f("g_LitmapParams", &vLightMapParams);
	pHLSL->SetValue1f("g_fSpecPower", trnMaterial.GetPower());
	//A3DMATRIX4 matVP = pA3DDevice->GetViewMatrix() * pA3DDevice->GetProjectionMatrix();
	A3DMATRIX4 matVP =pViewport->GetCamera()->GetVPTM();
	pHLSL->SetConstantMatrix("g_matViewProjection", matVP);

	if (pA3DDevice->GetA3DEngine()->GetA3DShadowMap())
		pHLSL->SetConstantMatrix("g_matLightWVP", pA3DDevice->GetA3DEngine()->GetA3DShadowMap()->GetShadowMatrix());
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement A3DTerrain2Render
//	
///////////////////////////////////////////////////////////////////////////

//	Render routine using ps 2.0 hlsl and lightmap
bool A3DTerrain2Render::Render_PS20_LM(A3DViewport* pViewport, bool bShadowMap)
{
	using namespace A3D;
    
    //static bool isPressLast = false;
    //static bool use3 = false;
    //bool isPress = GetAsyncKeyState('U') < 0;

    //if (isPress && !isPressLast)
    //{
    //    use3 = !use3;
    //    OutputDebugStringA(use3 ? "USE new t\n" : "USE old t\n");
    //}
    //isPressLast = isPress;

    //if (use3)
        return Render_PS20_NEW(pViewport, bShadowMap);

	A3DVertexDecl* pDecl = g_pA3DTerrain2Env->GetVertexDecl();
	if (!pDecl)
		return false;

	if (!m_aCurSlots.GetSize())
		return true;

	SyncLastRenderSlots();

	m_pA3DDevice->SetLightingEnable(true);
	m_pA3DDevice->SetSpecularEnable(true);
	m_pA3DDevice->SetSourceAlpha(A3DBLEND_ONE);
	m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);

	APtrArray<A3DTrn2RenderSlot*> a3LayerSlots;
	APtrArray<A3DTrn2RenderSlot*> a6LayerSlots;
	for (int i = 0; i < m_aCurSlots.GetSize(); ++i)
	{
		if (m_aCurSlots[i]->GetSortLayerNum() <= 3)
			a3LayerSlots.Add(m_aCurSlots[i]);
		else if (m_aCurSlots[i]->GetSortLayerNum() <= 6)
			a6LayerSlots.Add(m_aCurSlots[i]);
		else
		{
			// do not render ? ... or take them as 6 layer slot ?
			a6LayerSlots.Add(m_aCurSlots[i]);
		}
	}

// 	const int MAX_TEXTURE_STAGE_NUM = 8;
// 	for (int i = 0; i < MAX_TEXTURE_STAGE_NUM; ++i)
// 	{
// 		m_pA3DDevice->SetTextureCoordIndex(i, i);
// 		m_pA3DDevice->SetTextureTransformFlags(i, A3DTTFF_DISABLE);
// 	}

	//	samplers used in terrain hlsl render
	//const int SAMPLER_USED_NUM = 12;
	//for (int i = 0; i < SAMPLER_USED_NUM; ++i)
	//{
	//	m_pA3DDevice->SetTextureFilterType(i, A3DTEXF_LINEAR);
	//}


	const int SAMPLER_USED_NUM = 12;
	for (int i = 0; i <= SAMPLER_USED_NUM; ++i)
	{
		m_pA3DDevice->SetTextureFilterType(i, A3DTEXF_LINEAR);
	}

	g_pA3DTerrain2Env->GetVertexDecl()->Appear();


	m_pA3DDevice->SetTextureAddress(2, A3DTADDR_WRAP, A3DTADDR_WRAP);
	m_pA3DDevice->SetTextureAddress(3, A3DTADDR_WRAP, A3DTADDR_WRAP);
	m_pA3DDevice->SetTextureAddress(4, A3DTADDR_WRAP, A3DTADDR_WRAP);
	m_pA3DDevice->SetTextureAddress(5, A3DTADDR_CLAMP, A3DTADDR_CLAMP);
	m_pA3DDevice->SetTextureAddress(6, A3DTADDR_CLAMP, A3DTADDR_CLAMP);
	m_pA3DDevice->SetTextureAddress(7, A3DTADDR_CLAMP, A3DTADDR_CLAMP);
	m_pA3DDevice->SetTextureAddress(8, A3DTADDR_CLAMP, A3DTADDR_CLAMP);

	SetCommonParams(g_pA3DTerrain2Env->GetHLSL3LayersLM(bShadowMap), m_pA3DDevice, m_pTerrain, pViewport);

	{
		//	clip plane setting is moved to outside
		for (int i = 0; i < a3LayerSlots.GetSize(); ++i)
		{
			RenderSlot_PS20_LM<3>(pViewport, a3LayerSlots[i], g_pA3DTerrain2Env->GetHLSL3LayersLM(bShadowMap), bShadowMap);
		}
	}

	m_pA3DDevice->SetTextureAddress(2, A3DTADDR_WRAP, A3DTADDR_WRAP);
	m_pA3DDevice->SetTextureAddress(3, A3DTADDR_WRAP, A3DTADDR_WRAP);
	m_pA3DDevice->SetTextureAddress(4, A3DTADDR_WRAP, A3DTADDR_WRAP);
	m_pA3DDevice->SetTextureAddress(5, A3DTADDR_WRAP, A3DTADDR_WRAP);
	m_pA3DDevice->SetTextureAddress(6, A3DTADDR_WRAP, A3DTADDR_WRAP);
	m_pA3DDevice->SetTextureAddress(7, A3DTADDR_WRAP, A3DTADDR_WRAP);
	m_pA3DDevice->SetTextureAddress(8, A3DTADDR_CLAMP, A3DTADDR_CLAMP);
	m_pA3DDevice->SetTextureAddress(9, A3DTADDR_CLAMP, A3DTADDR_CLAMP);
	m_pA3DDevice->SetTextureAddress(10, A3DTADDR_CLAMP, A3DTADDR_CLAMP);
	m_pA3DDevice->SetTextureAddress(11, A3DTADDR_CLAMP, A3DTADDR_CLAMP);
	m_pA3DDevice->SetTextureAddress(12, A3DTADDR_CLAMP, A3DTADDR_CLAMP);

	SetCommonParams(g_pA3DTerrain2Env->GetHLSL6LayersLM(bShadowMap), m_pA3DDevice, m_pTerrain, pViewport);	

	{
		//	clip plane setting is moved to outside
		for (int i = 0; i < a6LayerSlots.GetSize(); ++i)
		{
			RenderSlot_PS20_LM<6>(pViewport, a6LayerSlots[i], g_pA3DTerrain2Env->GetHLSL6LayersLM(bShadowMap), bShadowMap);
		}
	}

	//	Restore render states
	m_pA3DDevice->ClearPixelShader();
	m_pA3DDevice->ClearVertexShader();
	m_pA3DDevice->SetZWriteEnable(true);
	m_pA3DDevice->SetLightingEnable(true);
	m_pA3DDevice->SetSpecularEnable(true);
	m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
	m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);

	for (int i = 0; i <= SAMPLER_USED_NUM; ++i)
		m_pA3DDevice->ClearTexture(i);

	return true;
}

template <int LAYERCNTONEPASS>
bool A3DTerrain2Render::RenderSlot_PS20_LM(A3DViewport* pViewport, A3DTrn2RenderSlot* pSlot, A3DHLSL* pHLSL, bool bShadowMap)
{
	using namespace A3D;

	A3DTerrain2Mask* pMask = m_pTerrain->GetMaskArea(pSlot->m_iMaskIdx);
	if (!pMask)
		return true;

	int iStartLayer = 0;
	int iLayerCnt = a_Min(pMask->GetLayerNum(), LAYERCNTONEPASS);
	if (iLayerCnt)
	{
		//	Render primitives
		TerrainMtlParams<LAYERCNTONEPASS> mtlParams(this);
		mtlParams.Init(pMask);
		mtlParams.Fill(pHLSL, m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->GetErrorTexture());

		A3DShadowMap* pShadowMap = m_pA3DDevice->GetA3DEngine()->GetA3DShadowMap();
		if (bShadowMap && pShadowMap)
		{

			pHLSL->SetTexture("g_ShadowMapSampler", pShadowMap->GetShadowMapTexture());
			pHLSL->SetTexture("g_ShadowMapFloat", pShadowMap->GetShadowMapFloatTexture());
			for(int iShadowMapIndex = 0; iShadowMapIndex <= 1; iShadowMapIndex++)
			{
				m_pA3DDevice->SetTextureAddress(iShadowMapIndex, A3DTADDR_BORDER, A3DTADDR_BORDER);
				m_pA3DDevice->SetSamplerState(iShadowMapIndex, D3DSAMP_BORDERCOLOR,  0xffffffff);
			}
			if (pShadowMap->IsUsingHWPcf() || pShadowMap->IsUsingVSM())
			{
				m_pA3DDevice->SetTextureFilterType(0, A3DTEXF_LINEAR);
				m_pA3DDevice->SetTextureFilterType(1, A3DTEXF_LINEAR);
			}
			else
			{
				m_pA3DDevice->SetTextureFilterType(0, A3DTEXF_POINT);
				m_pA3DDevice->SetTextureFilterType(1, A3DTEXF_POINT);
			}
			A3DVECTOR4 vShadowMapSize((float)pShadowMap->GetShadowMapSize(), 1, 0, 0);
			pHLSL->SetValue4f("g_SettingShadowMapSize", &vShadowMapSize);
		}

		pHLSL->Appear(m_pA3DDevice->GetA3DEngine()->GetA3DEnvironment()->GetCommonConstTable());	

		//Set shadow map texture. after pHLSL->Appear();
		//Set up shadow map


		//	Render
		pSlot->DrawAllBlocks(false);
	}

	return true;
}

static float RenderEnableMask[6][4]=
{
    0.f,0.f,-1.f,0.f,
    0.f,-1.f,0.f,0.f,
    -1.f,0.f,0.f,0.f,

    0.f,0.f,1.f,0.f,
    0.f,1.f,0.f,0.f,
    1.f,0.f,0.f,0.f,
};

int A3DTerrain2Render::PrepareLayers(A3DTerrain2Block* pBlock, A3DTerrain2Mask* pMask, 
                                     float* pProjAxis, float* pScales, 
                                     A3DVECTOR4* pEnableMask, A3DTexture** pTextures)
{
    int iNumLayers = min(pMask->GetLayerNum(),6);
    int iShaderLayer = 0;
    for (int i = 0; i < iNumLayers; i++)
    {
        if (pBlock->GetLayerFlags() & 1 << i)//enable
        {
            pTextures[iShaderLayer] = pMask->GetTexture(i);

            A3DTerrain2Mask::LAYER& Layer = pMask->GetLayer(i);
            
            pProjAxis[iShaderLayer] = Layer.byProjAxis;
            pScales[iShaderLayer * 2] = Layer.fUScale;
            pScales[iShaderLayer * 2 + 1] = Layer.fVScale;
            pEnableMask[iShaderLayer].Set(RenderEnableMask[i][0], RenderEnableMask[i][1], RenderEnableMask[i][2], RenderEnableMask[i][3]);
            iShaderLayer++;
        }
    }

    return iShaderLayer;
}

//	Render routine using ps 2.0 hlsl and lightmap
bool A3DTerrain2Render::Render_PS20_NEW(A3DViewport* pViewport, bool bShadowMap)
{
    using namespace A3D;

    A3DVertexDecl* pDecl = g_pA3DTerrain2Env->GetVertexDecl();
    if (!pDecl)
        return false;

    if (!m_aCurSlots.GetSize())
        return true;

    SyncLastRenderSlots();

    bool bAlphaBlend = m_pA3DDevice->GetAlphaBlendEnable();
    bool bAlphaTest = m_pA3DDevice->GetAlphaTestEnable();

    m_pA3DDevice->SetAlphaBlendEnable(false);
    m_pA3DDevice->SetAlphaTestEnable(false);

    const int SAMPLER_USED_NUM = 12;
    for (int i = 0; i <= SAMPLER_USED_NUM; ++i)
    {
        m_pA3DDevice->SetTextureFilterType(i, A3DTEXF_LINEAR);
    }

    g_pA3DTerrain2Env->GetVertexDecl()->Appear();


    m_pA3DDevice->SetTextureAddress(2, A3DTADDR_WRAP, A3DTADDR_WRAP);
    m_pA3DDevice->SetTextureAddress(3, A3DTADDR_WRAP, A3DTADDR_WRAP);
    m_pA3DDevice->SetTextureAddress(4, A3DTADDR_WRAP, A3DTADDR_WRAP);
    m_pA3DDevice->SetTextureAddress(5, A3DTADDR_WRAP, A3DTADDR_WRAP);
    m_pA3DDevice->SetTextureAddress(6, A3DTADDR_WRAP, A3DTADDR_WRAP);
    m_pA3DDevice->SetTextureAddress(7, A3DTADDR_WRAP, A3DTADDR_WRAP);
    m_pA3DDevice->SetTextureAddress(8, A3DTADDR_CLAMP, A3DTADDR_CLAMP);
    m_pA3DDevice->SetTextureAddress(9, A3DTADDR_CLAMP, A3DTADDR_CLAMP);
    m_pA3DDevice->SetTextureAddress(10, A3DTADDR_CLAMP, A3DTADDR_CLAMP);
    m_pA3DDevice->SetTextureAddress(11, A3DTADDR_CLAMP, A3DTADDR_CLAMP);
    m_pA3DDevice->SetTextureAddress(12, A3DTADDR_CLAMP, A3DTADDR_CLAMP);
    
    // Initialize shader slots (6 layers)
    m_aRenderHLSLSlots[0].pShader = NULL;

    bool bHighShadow = false;
    A3DShadowMap* pShadowMap = m_pA3DDevice->GetA3DEngine()->GetA3DShadowMap();
    if (bShadowMap && pShadowMap)
    {
        if (pShadowMap->GetSampleLevel() > 0)
            bHighShadow = true;
    }

    for (int iLayer = 1; iLayer <= 6; iLayer++)
    {
        A3DHLSL* pShader = g_pA3DTerrain2Env->GetHLSLNewShader(iLayer, bShadowMap, bHighShadow);
        m_aRenderHLSLSlots[iLayer].pShader = pShader;
        m_aRenderHLSLSlots[iLayer].nCount = 0;
        SetCommonParams(pShader, m_pA3DDevice, m_pTerrain, pViewport);
        if (bShadowMap && pShadowMap)
        {
            pShader->SetTexture("g_ShadowMapSampler", pShadowMap->GetShadowMapTexture());
            pShader->SetTexture("g_ShadowMapFloat", pShadowMap->GetShadowMapFloatTexture());
            for(int iShadowMapIndex = 0; iShadowMapIndex <= 1; iShadowMapIndex++)
            {
                m_pA3DDevice->SetTextureAddress(iShadowMapIndex, A3DTADDR_BORDER, A3DTADDR_BORDER);
                m_pA3DDevice->SetSamplerState(iShadowMapIndex, D3DSAMP_BORDERCOLOR,  0xffffffff);
            }
            if (pShadowMap->IsUsingHWPcf() || pShadowMap->IsUsingVSM())
            {
                m_pA3DDevice->SetTextureFilterType(0, A3DTEXF_LINEAR);
                m_pA3DDevice->SetTextureFilterType(1, A3DTEXF_LINEAR);
            }
            else
            {
                m_pA3DDevice->SetTextureFilterType(0, A3DTEXF_POINT);
                m_pA3DDevice->SetTextureFilterType(1, A3DTEXF_POINT);
            }
            A3DVECTOR4 vShadowMapSize((float)pShadowMap->GetShadowMapSize(), 1, 0, 0);
            pShader->SetValue4f("g_SettingShadowMapSize", &vShadowMapSize);
        }

    }

    //pDecl->Appear();

    //for (int iSlot = 0; iSlot < m_aCurSlots.GetSize(); iSlot++)
    //{
    //    RenderSlot_PS20_NEW(pViewport, m_aCurSlots[iSlot], bShadowMap);
    //}

    // Register block (ÏÈshader ºóslot)

    for (int iSlot = 0; iSlot < m_aCurSlots.GetSize(); iSlot++)
    {
        A3DTrn2RenderSlot* pSlot = m_aCurSlots[iSlot];
        A3DTerrain2Mask* pMask = m_pTerrain->GetMaskArea(pSlot->m_iMaskIdx);
        if (!pMask)
            continue;
///****        
        int nLayers = pSlot->m_nUsedLayerNum;
        if (nLayers <= 0 || nLayers > 6)
            continue;

        TerrainShaderSlotRec* pShaderRec = &m_aRenderHLSLSlots[nLayers];

        TerrainRenderSlotRec* pSlotRec = NULL;
        int nCount = a_Min(pShaderRec->nCount, (int)pShaderRec->aSlots.size());
        for (int iShaderSlot = 0; iShaderSlot < nCount; iShaderSlot++)
        {
            if (pShaderRec->aSlots[iShaderSlot]->pSlot == pSlot)
            {
                pSlotRec = pShaderRec->aSlots[iShaderSlot];
                break;
            }
        }

        if (pSlotRec == NULL)
        {
            if (nCount < (int)pShaderRec->aSlots.size())
            {
                pSlotRec = pShaderRec->aSlots[nCount];
            }
            else
            {
                pSlotRec = new TerrainRenderSlotRec;
                pShaderRec->aSlots.push_back(pSlotRec);
            }
            pSlotRec->pMask = pMask;
            pSlotRec->pSlot = pSlot;
            pSlotRec->nCount = 0;
            pShaderRec->nCount++;
        }


        //for (int iBlock = 0; iBlock < A3DTrn2RenderSlot::MAX_BLOCK_NUM; iBlock++)
        //{
        //    A3DTerrain2Block* pBlock = pSlot->m_aBlocks[iBlock].pBlock;
        //    if (!pBlock)
        //        continue;
        //    
        //    A3DTerrain2Block::RENDERDATA* pRenderData = (A3DTerrain2Block::RENDERDATA*)pBlock->GetRenderData();

        //    if (!pViewport->GetCamera()->AABBInViewFrustum(pBlock->GetBlockAABB()))
        //        continue;
        //    
        //    A3DTerrain2Block::RenderParam& renderParam = pBlock->GetRenderParam();
        //    int nLayers = renderParam.nLayers;//  PrepareLayers(pBlock, pMask, pBlockRec->fProjAxis, pBlockRec->fScales, pBlockRec->vEnableMask, pBlockRec->pTextures);
        //    
        //    TerrainShaderSlotRec* pShaderRec = &m_aRenderHLSLSlots[nLayers];

        //    TerrainRenderSlotRec* pSlotRec = NULL;
        //    int nCount = a_Min(pShaderRec->nCount, (int)pShaderRec->aSlots.size());
        //    for (int iShaderSlot = 0; iShaderSlot < nCount; iShaderSlot++)
        //    {
        //        if (pShaderRec->aSlots[iShaderSlot]->pSlot == pSlot)
        //        {
        //            pSlotRec = pShaderRec->aSlots[iShaderSlot];
        //            break;
        //        }
        //    }

        //    if (pSlotRec == NULL)
        //    {
        //        if (nCount < (int)pShaderRec->aSlots.size())
        //        {
        //            pSlotRec = pShaderRec->aSlots[nCount];
        //        }
        //        else
        //        {
        //            pSlotRec = new TerrainRenderSlotRec;
        //            pShaderRec->aSlots.push_back(pSlotRec);
        //        }
        //        pSlotRec->pMask = pMask;
        //        pSlotRec->pSlot = pSlot;
        //        pSlotRec->nCount = 0;
        //        pSlotRec->aBlocks.clear();
        //        pShaderRec->nCount++;
        //    }
        //    
        //    if (pSlotRec->nCount < (int)pSlotRec->aBlocks.size())
        //    {
        //        pSlotRec->aBlocks[pSlotRec->nCount] = pBlock;
        //    }
        //    else
        //    {
        //        pSlotRec->aBlocks.push_back(pBlock);
        //    }
        //    pSlotRec->nCount++;
        //}
    }

    // Do render
    float fProjAxis[8];
    float fScales[12];
    A3DVECTOR4 vEnableMask[6];
    ZeroMemory(fProjAxis, sizeof(fProjAxis));
    bool bFirstShader = true;
    pDecl->Appear();

    for (int iLayer = 1; iLayer <= 6; iLayer++)
    {
        TerrainShaderSlotRec* pShaderRec = &m_aRenderHLSLSlots[iLayer];
        A3DHLSL* pShader = pShaderRec->pShader;
        if (pShaderRec->nCount == 0)
            continue;

        //if (bFirstShader)
            pShader->Appear(m_pA3DDevice->GetA3DEngine()->GetA3DEnvironment()->GetCommonConstTable());
        //else
        //    pShader->Appear(APPEAR_SETSHADERONLY);
        
        for (int iSlot = 0; iSlot < pShaderRec->nCount; iSlot++)
        {
            TerrainRenderSlotRec* pSlotRec = pShaderRec->aSlots[iSlot];

            A3DTexture* pLightMaps[2];
            if (m_pTerrain->GetLightEnableFlag())
            {
                pLightMaps[0] = pSlotRec->pMask->GetLightMap(true);
                pLightMaps[1] = pSlotRec->pMask->GetLightMap(false);
            }
            else
            {
                pLightMaps[0] = pLightMaps[1] = A3D::g_pA3DTerrain2Env->GetWhiteTexture();
            }
            A3DTexture* pColorMap = pSlotRec->pMask->GetDetailMap() ? pSlotRec->pMask->GetDetailMap() : A3D::g_pA3DTerrain2Env->GetUnitWhiteTexture();

            if (pSlotRec->pMask->GetMaskTextureCount() > 0)
            {
                A3DTexture * pTexture = pSlotRec->pMask->GetMaskTexture(0);
                if (pTexture) pTexture->Appear(8);
                //else m_pA3DDevice->ClearTexture(8);
            }
            if (pSlotRec->pMask->GetMaskTextureCount() > 1)
            {
                A3DTexture * pTexture = pSlotRec->pMask->GetMaskTexture(1);
                if (pTexture) pTexture->Appear(9);
                //else m_pA3DDevice->ClearTexture(9);
            }
            
            if (pLightMaps[0])
                pLightMaps[0]->Appear(10);
            if (pLightMaps[1])
                pLightMaps[1]->Appear(11);
            if (pColorMap)
                pColorMap->Appear(12);

            //pSlotRec->pSlot->m_pVertStream->AppearVertexOnly(0, false);
            //pSlotRec->pSlot->m_pIdxStream->AppearIndexOnly();
///***
            for (int iShaderLayer = 0; iShaderLayer < iLayer; iShaderLayer++)
            {
                int iRealIndex = pSlotRec->pSlot->m_iUsedLayerIndices[iShaderLayer];
                A3DTerrain2Mask::LAYER& Layer = pSlotRec->pMask->GetLayer(iRealIndex);

                fProjAxis[iShaderLayer] = Layer.byProjAxis; 
                fScales[iShaderLayer * 2] = Layer.fUScale; 
                fScales[iShaderLayer * 2 + 1] = Layer.fVScale; 
                vEnableMask[iShaderLayer] = *(A3DVECTOR4*)(RenderEnableMask[iRealIndex]);
            }
            m_pA3DDevice->SetVertexShaderConstants(0, fProjAxis, 2);
            m_pA3DDevice->SetVertexShaderConstants(2, fScales, 3);
            m_pA3DDevice->SetPixelShaderConstants(0, vEnableMask, iLayer);

            for (int iShaderLayer = 0; iShaderLayer < iLayer; iShaderLayer++)
            {
                A3DTexture* pTexture = pSlotRec->pMask->GetTexture(pSlotRec->pSlot->m_iUsedLayerIndices[iShaderLayer]);
                m_pA3DDevice->ClearTexture(iShaderLayer + 2);
                if (pTexture)
                    pTexture->Appear(iShaderLayer + 2);
            }

            pSlotRec->pSlot->DrawAllBlocks(false);

            //for (int iBlock = 0; iBlock < pSlotRec->nCount; iBlock++)
            //{
            //    nBlockCount++;
            //    A3DTerrain2Block::RENDERDATA* pRenderData = (A3DTerrain2Block::RENDERDATA*)pSlotRec->aBlocks[iBlock]->GetRenderData();
            //    A3DTerrain2Block::RenderParam& renderParam = pSlotRec->aBlocks[iBlock]->GetRenderParam();


            //    //for (int iShaderLayer = 0; iShaderLayer < renderParam.nLayers; iShaderLayer++)
            //    //{
            //    //    A3DTerrain2Mask::LAYER& Layer = pSlotRec->pMask->GetLayer(renderParam.iLayerIndices[iShaderLayer]);

            //    //    fProjAxis[iShaderLayer] = Layer.byProjAxis; 
            //    //    fScales[iShaderLayer * 2] = Layer.fUScale; 
            //    //    fScales[iShaderLayer * 2 + 1] = Layer.fVScale; 
            //    //}
            //    //m_pA3DDevice->SetVertexShaderConstants(0, fProjAxis, 2);
            //    //m_pA3DDevice->SetVertexShaderConstants(2, fScales, 3);
            //    //m_pA3DDevice->SetPixelShaderConstants(0, renderParam.vEnableMask, renderParam.nLayers);

            //    for (int iShaderLayer = 0; iShaderLayer < renderParam.nLayers; iShaderLayer++)
            //    {
            //        A3DTexture* pTexture = pSlotRec->pMask->GetTexture(renderParam.iLayerIndices[iShaderLayer]);
            //        if (pTexture)
            //            pTexture->Appear(iShaderLayer + 2);
            //    }
            //    deltaTime = (iEnd.QuadPart - iStart.QuadPart) * 1000.0 / iFraq.QuadPart; 
            //    fTimeStat5 += deltaTime;

            //    m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0,
            //        renderParam.iMinIndex, pRenderData->iNumVert, renderParam.iStartIndex, pRenderData->iNumIndex / 3);
            //}
        }
    }

    //AString strDGB;
    //strDGB.Format("terrain time = %f, %f, %f, %f, %f, %f, %d\n", fTimeStat1, fTimeStat2, fTimeStat3, fTimeStat4, fTimeStat5, fTimeStat6, nBlockCount);
    //OutputDebugStringA(strDGB);
    for (int iLayer = 1; iLayer <= 6; iLayer++)
    {
        m_aRenderHLSLSlots[iLayer].nCount = 0;
    }

    //	Restore render states
    m_pA3DDevice->ClearPixelShader();
    m_pA3DDevice->ClearVertexShader();
    m_pA3DDevice->SetAlphaBlendEnable(bAlphaBlend);
    m_pA3DDevice->SetAlphaTestEnable(bAlphaTest);
    m_pA3DDevice->SetZWriteEnable(true);

    for (int i = 0; i <= SAMPLER_USED_NUM; ++i)
        m_pA3DDevice->ClearTexture(i);

    return true;
}

bool A3DTerrain2Render::RenderSlot_PS20_NEW(A3DViewport* pViewport, A3DTrn2RenderSlot* pSlot, bool bShadowMap)
{
    //using namespace A3D;

    //A3DTerrain2Mask* pMask = m_pTerrain->GetMaskArea(pSlot->m_iMaskIdx);
    //if (!pMask)
    //    return true;
    //
    //pSlot->m_pVertStream->AppearVertexOnly(0, false);
    //pSlot->m_pIdxStream->AppearIndexOnly();

    //int iVertCnt = 0;
    //int iIndexCnt = 0;

    //float fProjAxis[8];
    //float fScales[12];
    //A3DTexture* pTextures[6];
    //A3DVECTOR4 vEnableMask[6];

    //A3DTexture* pLightMaps[2];
    //A3DTexture* pColorMap;
    //if (m_pTerrain->GetLightEnableFlag())
    //{
    //    pLightMaps[0] = pMask->GetLightMap(true);
    //    pLightMaps[1] = pMask->GetLightMap(false);
    //}
    //else
    //{
    //    pLightMaps[0] = pLightMaps[1] = A3D::g_pA3DTerrain2Env->GetWhiteTexture();
    //}

    //pColorMap = pMask->GetDetailMap() ? pMask->GetDetailMap() : A3D::g_pA3DTerrain2Env->GetUnitWhiteTexture();


    //for (int iBlock = 0; iBlock < A3DTrn2RenderSlot::MAX_BLOCK_NUM; iBlock++)
    //{
    //    A3DTerrain2Block* pBlock = pSlot->m_aBlocks[iBlock].pBlock;
    //    if (!pBlock)
    //        continue;

    //    ZeroMemory(fProjAxis, sizeof(fProjAxis));
    //    ZeroMemory(fScales, sizeof(fScales));
    //    int nLayers = PrepareLayers(pBlock, pMask, fProjAxis, fScales, vEnableMask, pTextures);
    //    A3DHLSL* pShader = g_pA3DTerrain2Env->GetHLSLNewShader(nLayers, bShadowMap);

    //    SetCommonParams(pShader, m_pA3DDevice, m_pTerrain, pViewport);

    //    A3DShadowMap* pShadowMap = m_pA3DDevice->GetA3DEngine()->GetA3DShadowMap();
    //    if (bShadowMap && pShadowMap)
    //    {

    //        pShader->SetTexture("g_ShadowMapSampler", pShadowMap->GetShadowMapTexture());
    //        pShader->SetTexture("g_ShadowMapFloat", pShadowMap->GetShadowMapFloatTexture());
    //        for(int iShadowMapIndex = 0; iShadowMapIndex <= 1; iShadowMapIndex++)
    //        {
    //            m_pA3DDevice->SetTextureAddress(iShadowMapIndex, A3DTADDR_BORDER, A3DTADDR_BORDER);
    //            m_pA3DDevice->SetSamplerState(iShadowMapIndex, D3DSAMP_BORDERCOLOR,  0xffffffff);
    //        }
    //        if (pShadowMap->IsUsingHWPcf() || pShadowMap->IsUsingVSM())
    //        {
    //            m_pA3DDevice->SetTextureFilterType(0, A3DTEXF_LINEAR);
    //            m_pA3DDevice->SetTextureFilterType(1, A3DTEXF_LINEAR);
    //        }
    //        else
    //        {
    //            m_pA3DDevice->SetTextureFilterType(0, A3DTEXF_POINT);
    //            m_pA3DDevice->SetTextureFilterType(1, A3DTEXF_POINT);
    //        }
    //        A3DVECTOR4 vShadowMapSize((float)pShadowMap->GetShadowMapSize(), (float)pShadowMap->GetSampleNum(), 0, 0);
    //        pShader->SetValue4f("g_SettingShadowMapSize", &vShadowMapSize);
    //    }

    //    pShader->SetConstantArrayF("g_texAxis", (const A3DVECTOR4*)fProjAxis, 2);
    //    pShader->SetConstantArrayF("g_texScale", (const A3DVECTOR4*)fScales, 3);
    //    pShader->SetConstantArrayF("g_EnableMask", vEnableMask, 6);
    //    
    //    if (pMask->GetMaskTextureCount() > 0)
    //        pShader->SetTexture("g_MaskMapSampler0", pMask->GetMaskTexture(0));
    //    if (pMask->GetMaskTextureCount() > 1)
    //        pShader->SetTexture("g_MaskMapSampler1", pMask->GetMaskTexture(1));

    //    pShader->SetTexture("g_LMDaySampler", pLightMaps[0]);
    //    pShader->SetTexture("g_LMNightSampler", pLightMaps[1]);
    //    pShader->SetTexture("g_ColTexSampler", pColorMap);

    //    pShader->Appear(m_pA3DDevice->GetA3DEngine()->GetA3DEnvironment()->GetCommonConstTable());	

    //    for (int iLayer = 0; iLayer < nLayers; iLayer++)
    //    {
    //        if (pTextures[iLayer])
    //            pTextures[iLayer]->Appear(iLayer + 2);
    //    }

    //    A3DTerrain2Block::RENDERDATA* pRenderData = (A3DTerrain2Block::RENDERDATA*)pBlock->GetRenderData();

    //    m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, iVertCnt, pRenderData->iNumVert, iIndexCnt, pRenderData->iNumIndex / 3);

    //    iVertCnt += pRenderData->iNumVert;
    //    iIndexCnt += pRenderData->iNumIndex;
    //}

    return true;
}