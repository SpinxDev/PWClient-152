 /*
 * FILE: A3DLitModelRender.cpp
 *
 * DESCRIPTION: A3D lit model renderer class
 *
 * CREATED BY: Hedi, 2005/4/4
 *
 * HISTORY:
 *
 * Copyright (c) 2001~2008 Archosaur Studio, All Rights Reserved.	
 */

#include "A3DEngine.h"
#include "A3DDevice.h"
#include "A3DViewport.h"
#include "A3DTexture.h"
#include "A3DFrame.h"
#include "A3DLitModelRender.h"
#include "A3DLitModel.h"
#include "A3DStream.h"
#include "A3DPI.h"
#include "A3DFuncs.h"
#include "A3DShaderMan.h"
#include "A3DTextureMan.h"
#include "A3DPixelShader.h"
#include "A3DVertexShader.h"
#include "A3DCameraBase.h"
#include "A3DOrthoCamera.h"
#include "A3DCamera.h"
#include "A3DRenderTarget.h"
#include "A3DShaderMan.h"
#include "A3DLight.h"
#include "A3DShadowMap.h"
#include "A3DConfig.h"
#include "A3DHLSL.h"
#include "A3DEffect.h"
#include "A3DShader.h"
#include "AFI.h"
#include "AGPAPerfIntegration.h"
#include "A3DEnvironment.h"

#define _LITMODEL_PERF_DEBUG_

enum LitModelShaderLevel
{
    SHADER_UNLIT = 0,
    SHADER_LIGHTMAP = 1,
    SHADER_VERTLIT = 2,
    SHADER_REALTIME = 3,
    SHADER_SHADOWMAP = 4,
    SHADER_SHADOWMAP_HIGH = 8,
    SHADER_DETAILMAP = 16,
};

A3DLitModelRender::A3DLitModelRender()
: m_aRenderSlots(128, 32),
m_aAlphaMeshes(128, 32),
m_aAlphaMeshes2(128, 32),
m_aShaderMeshes(128, 32),
m_aVertlitSlots(128,32)
{
	m_idAlphaMeshArray	= 0;
	m_fDNFactor			= 0.0f;

	m_bIsRenderForRefract = false;
	m_vRefractSurfaceHeight = 0.0f;

	m_pVSHLSL						= NULL;
	m_pVSHLSL_LM                    = NULL;
    m_pPSHLSL_Fixed                 = NULL;
	m_bSupportSM20					= false;

	m_pSmallMap1x1					= NULL;
    m_pEmptyDetailMap               = NULL;
    m_bDetailMap                    = true;

    ZeroMemory(m_pVSShaders, sizeof(m_pVSShaders));
    ZeroMemory(m_pPSShaders, sizeof(m_pPSShaders));

    m_bRealtime = false;
    m_fClipMinDistance = 50.0f;
    m_fClipMaxAngle = 0.05f;
}

A3DLitModelRender::~A3DLitModelRender()
{
}

//	Initialize object
bool A3DLitModelRender::Init(A3DEngine* pA3DEngine, int nMaxVerts, int nMaxIndices)
{
	m_pA3DEngine		= pA3DEngine;
	m_pA3DDevice		= m_pA3DEngine->GetA3DDevice();

	m_bSupportSM20		= m_pA3DDevice->TestPixelShaderVersion(2, 0) && m_pA3DDevice->TestVertexShaderVersion(2, 0);

	m_pLightMapLookup = new A3DTexture();
	if( !m_pLightMapLookup->Create(m_pA3DDevice, LIGHTMAP_LOOKUP_SIZE, 1, A3DFMT_A8R8G8B8))
	{
		g_A3DErrLog.Log("A3DLitModelRender::InitShadowResource(), Failed to create lightmap lookup texture!");
		return false;
	}

	void* pData = NULL;
	int nPitch = 0;
	if(!m_pLightMapLookup->LockRect(NULL, &pData, &nPitch, 0, 0))
		return false;

	DWORD* pMap = (DWORD*)pData;
	for( INT i = 0; i < LIGHTMAP_LOOKUP_SIZE; i++ )
	{
		float fD =float(i)/LIGHTMAP_LOOKUP_SIZE;
		float fC = fD/ max(0.001f, 1.0f - fD);
		if(fC >= 9.0f)
			fC = 8.99f;

		int nR = 0;
		int nG = 0;
		int nB = 0;
		float fR  = 0.0f;
		float fG = 0.0f;
		float fB = 0.0f;

		fR = fC - int(fC);
		nR = int(fR *255);
		nG = (int)(int(fC)/8.0f * 255);
		*pMap++ = D3DCOLOR_RGBA( nR , nG, nB, 0 );	
	}

	if(!m_pLightMapLookup->UnlockRect())
		return false;

	m_pSmallMap1x1 = new A3DTexture;
	m_pSmallMap1x1->Create(m_pA3DDevice, 1, 1, A3DFMT_A8R8G8B8);

	D3DVERTEXELEMENT9 vertDeclPosDiffuseUV[] = 
	{
		{0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
		{0, 24, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},
		{0, 28, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		D3DDECL_END()
	};
	m_pVertDeclPosNormalDiffuseUV = new A3DVertexDecl();
	m_pVertDeclPosNormalDiffuseUV->Init(m_pA3DDevice, vertDeclPosDiffuseUV);

	D3DVERTEXELEMENT9 vertDeclPosUVUV[] = 
	{
		{0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
		{0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		{0, 32, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1},
		D3DDECL_END()
	};
	m_pVertDeclPosNormalUVUV = new A3DVertexDecl();
	m_pVertDeclPosNormalUVUV->Init(m_pA3DDevice, vertDeclPosUVUV);

	if(!InitShaderResource())
		return false;	

    m_pEmptyDetailMap = A3DTexture::CreateColorTexture(m_pA3DDevice, 32, 32, 0xFF7F7F7F);

    return true;
}

bool A3DLitModelRender::InitShaderResource()
{

	//release
	ReleaseShaderResource();
	A3DShaderMan* pShaderMan = m_pA3DEngine->GetA3DShaderMan();
	if( pShaderMan == NULL)
		return false;

	D3DVERTEXELEMENT9 vertDecl[] = 
	{
		{0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
		{0, 24, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},
		{0, 28, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		D3DDECL_END()
	};

	A3DEnvironment* pEnvironment = m_pA3DEngine->GetA3DEnvironment();	// 取环境对象
	A3DCCDBinder* pBinder = pEnvironment->GetCCDBinder();			// 取环境对象的绑定对象


	m_pVSHLSL = m_pA3DEngine->GetA3DHLSLMan()->LoadShader(
		"shaders\\2.2\\HLSL\\LitModel\\litmodel_vs.hlsl", NULL, NULL);
	if(!m_pVSHLSL)
	{
		g_A3DErrLog.Log("A3DLitModelRender::InitShadowResource(), Failed to create litmodel vertlit m_pVSHLSL!");
		return false;
	}
	m_pVSHLSL->BindCommConstData(pBinder);

	m_pVSHLSL_LM = m_pA3DEngine->GetA3DHLSLMan()->LoadShader(
		"shaders\\2.2\\HLSL\\LitModel\\litmodel_vs.hlsl", NULL, "_LIGHT_MAP_");
	if(!m_pVSHLSL_LM)
	{
		g_A3DErrLog.Log("A3DLitModelRender::InitShadowResource(), Failed to create litmodel vertlit m_pVSHLSL_LM!");
		return false;
	}
	m_pVSHLSL_LM->BindCommConstData(pBinder);


    //m_pPSHLSL_Fixed = m_pA3DEngine->GetA3DHLSLMan()->LoadShader(
    //    NULL, "shaders\\2.2\\HLSL\\LitModel\\litmodel_mtl_fixed.hlsl", NULL);
    //if(!m_pPSHLSL_Fixed)
    //{
    //    g_A3DErrLog.Log("A3DLitModelRender::InitShadowResource(), Failed to create litmodel fixed material!");
    //    return false;
    //}
    //m_pPSHLSL_Fixed->BindCommConstData(pBinder);

    for (int iShader = 0; iShader < MAX_SHADERS; iShader++)
        CreateDefaultShader(iShader);

    return true;
}


void A3DLitModelRender::ReleaseShaderResource()
{
	A3DShaderMan* pShaderMan = m_pA3DEngine->GetA3DShaderMan();
	if( pShaderMan != NULL)
	{
		if(m_pVSHLSL != NULL)
		{
			m_pA3DEngine->GetA3DHLSLMan()->ReleaseShader(m_pVSHLSL);
			m_pVSHLSL = NULL;
		}
		if(m_pVSHLSL_LM != NULL)
		{
			m_pA3DEngine->GetA3DHLSLMan()->ReleaseShader(m_pVSHLSL_LM);
			m_pVSHLSL_LM = NULL;
		}
        if(m_pPSHLSL_Fixed != NULL)
        {
            m_pA3DEngine->GetA3DHLSLMan()->ReleaseShader(m_pPSHLSL_Fixed);
            m_pPSHLSL_Fixed = NULL;
        }

        for (int i = 0; i < MAX_SHADERS; i++)
        {
            if (m_pVSShaders[i] != NULL)
                m_pA3DEngine->GetA3DHLSLMan()->ReleaseShader(m_pVSShaders[i]);
            m_pVSShaders[i] = NULL;
            if (m_pPSShaders[i] != NULL)
                m_pA3DEngine->GetA3DHLSLMan()->ReleaseShader(m_pPSShaders[i]);
            m_pPSShaders[i] = NULL;
        }
	}
}

//	Release object
void A3DLitModelRender::Release()
{
	ReleaseShaderResource();

	int i;

	for(i=0; i<m_aRenderSlots.GetSize(); i++)
	{
		delete m_aRenderSlots[i];
	}

	m_aRenderSlots.RemoveAll();

    m_aLightmapModelGroups->RemoveAll();

	for (i = 0; i < m_aVertlitSlots.GetSize(); i++)
	{
		delete m_aVertlitSlots[i];
	}
	m_aVertlitSlots.RemoveAll();

	A3DRELEASE(m_pLightMapLookup);
	A3DRELEASE(m_pSmallMap1x1);
	A3DRELEASE(m_pVertDeclPosNormalDiffuseUV);
	A3DRELEASE(m_pVertDeclPosNormalUVUV);
    A3DRELEASE(m_pEmptyDetailMap);
}

void A3DLitModelRender::RegisterAlphaMesh(A3DLitMesh* pLitMesh)
{
    APtrArray<A3DLitMesh *>* pAlphaMeshes;

    if (pLitMesh->GetTexturePtr()->IsShaderTexture())
        pAlphaMeshes = &m_aShaderMeshes;
    else if (m_idAlphaMeshArray == 0)
        pAlphaMeshes = &m_aAlphaMeshes;
    else
        pAlphaMeshes = &m_aAlphaMeshes2;

    int i = 0;
    // insert from far to near
    for (i=0; i<pAlphaMeshes->GetSize(); i++)
    {
        A3DLitMesh * pNextMesh = (*pAlphaMeshes)[i];

        if (pLitMesh->GetDisToCam() > pNextMesh->GetDisToCam())
        {
            // insert it before pNextMesh;
            pAlphaMeshes->InsertAt(i, pLitMesh);
            break;
        }
    }
    if( i == pAlphaMeshes->GetSize() )
    {
        pAlphaMeshes->Add(pLitMesh);
    }
}

bool A3DLitModelRender::RegisterRenderModel(A3DViewport* pViewport, A3DLitModel* pLitModel, bool bAlphaFlag/*false*/)
{
    float fDisToCam = sqrtf(a3d_DistSquareToAABB(pViewport->GetCamera()->GetPos(), pLitModel->GetModelAABB()));
    if (fDisToCam > m_fClipMinDistance)
    {
        if (pLitModel->GetModelAABB().Extents.Magnitude() / fDisToCam < m_fClipMaxAngle)
            return true;
    }

    int nMesh = 0;
    for (int iMesh = 0; iMesh < pLitModel->GetNumMeshes(); iMesh++)
    {
        A3DLitMesh* pLitMesh = pLitModel->GetMesh(iMesh);

        if (!pLitMesh->IsVisible())
            continue;
        if (pLitModel->IsNoTransform() && !pViewport->GetCamera()->AABBInViewFrustum(pLitMesh->GetAABB()))
            continue;

        if (pLitMesh->GetTexturePtr()->IsShaderTexture() || pLitMesh->GetAlphaValue() < 255 || bAlphaFlag )
        {
            RegisterAlphaMesh(pLitMesh);
        }
        else if (pLitMesh->IsUseLightMap() && pLitMesh->IsSupportLightMap())
        {
            nMesh++;
        }
        else
        {
            RegisterVertLitMesh(pViewport, pLitMesh);
        }
    }

    if (nMesh > 0)
    {
        int iSlotIndex = (int)(fDisToCam / 300 * MAX_SLOT_GROUP);

        a_Clamp(iSlotIndex, 0, MAX_SLOT_GROUP - 1);
        m_aLightmapModelGroups[iSlotIndex].Add(pLitModel);
    }

    return true;
}


bool A3DLitModelRender::RegisterRenderMesh(A3DViewport* pViewport, A3DLitMesh * pLitMesh, bool bAlphaFlag/*false*/)
{
	// if is a shader mesh or alpha flag mesh
	if( pLitMesh->GetTexturePtr()->IsShaderTexture() || pLitMesh->GetAlphaValue() < 255 || bAlphaFlag )
	{
        RegisterAlphaMesh(pLitMesh);
	}
	else
	{
		// first determine which slot to use
		A3DTexture *	pTexture = pLitMesh->GetTexturePtr();
		A3DTexture *	pReflectTexture = pLitMesh->GetReflectTexturePtr();
		BOOL			b2Sided = pLitMesh->GetMaterial().Is2Sided();

		for(int i=0; i < m_aRenderSlots.GetSize(); i++)
		{
			RENDERSLOT * pSlot = m_aRenderSlots[i];
			if( pTexture == pSlot->pTexture && b2Sided == pSlot->b2Sided )
			{
				// found the same texture slot, put it here
				m_aRenderSlots[i]->LitMeshes.Add(pLitMesh);
				return true;
			}
			else if (pSlot->pTexture == NULL)
			{
				// found an empty slot, so there is no same texture and put it here
				pSlot->pTexture = pTexture;
				pSlot->pReflectTexture = pReflectTexture;
				pSlot->b2Sided = b2Sided;
				pSlot->LitMeshes.Add(pLitMesh);
				return true;
			}
		}

		// not found in current slots, so add a new one slot here
		RENDERSLOT * pNewSlot = new RENDERSLOT;
		pNewSlot->pTexture = pTexture;
		pNewSlot->pReflectTexture = pReflectTexture;
		pNewSlot->b2Sided = b2Sided;
		pNewSlot->LitMeshes.Add(pLitMesh);
		m_aRenderSlots.Add(pNewSlot);
	}

	return true;
}

bool A3DLitModelRender::RegisterVertLitMesh(A3DViewport* pViewport, A3DLitMesh* pLitMesh)
{
	// first determine which slot to use
	A3DTexture* pTexture = pLitMesh->GetTexturePtr();

    A3DEffect*	pEffect = (A3DEffect*) pTexture;
    if (!pEffect)
        return false;

    pTexture = pEffect->GetOriginTex();
    if (!pTexture)
        pTexture = m_pA3DEngine->GetA3DTextureMan()->GetErrorTexture();

	A3DTexture *	pReflectTexture = pLitMesh->GetReflectTexturePtr();
	BOOL			b2Sided = pLitMesh->GetMaterial().Is2Sided();
    pLitMesh->UpdateColorByDayNight();
	for (int i = 0; i < m_aVertlitSlots.GetSize(); i++)
	{
		RENDERSLOT * pSlot = m_aVertlitSlots[i];
		if (pTexture == pSlot->pTexture && b2Sided == pSlot->b2Sided && m_bRealtime)
		{
			m_aVertlitSlots[i]->LitMeshes.Add(pLitMesh);
			return true;
		}
		else if( pSlot->pTexture == NULL )
		{
			// found an empty slot, so there is no same texture and put it here
			pSlot->pTexture = pTexture;
			pSlot->pReflectTexture = pReflectTexture;
			pSlot->b2Sided = b2Sided;
			pSlot->LitMeshes.Add(pLitMesh);
			return true;
		}
	}

	// not found in current slots, so add a new one slot here
	RENDERSLOT * pNewSlot = new RENDERSLOT;
	pNewSlot->pTexture = pTexture;
	pNewSlot->pReflectTexture = pReflectTexture;
	pNewSlot->b2Sided = b2Sided;
	pNewSlot->LitMeshes.Add(pLitMesh);
	m_aVertlitSlots.Add(pNewSlot);
	return true;
}

//	Render all lit models (不考虑LightMap)
bool A3DLitModelRender::Render(A3DViewport* pViewport, 
							   bool bRenderAlpha, 
							   bool bUseLightMap, 
							   bool bUseShadowMap )
{
	A3D_ITTX_SCOPED_TASK_NAMED(AGPAPerfIntegration::GetInstance().GetA3DDomain(), TEXT(__FUNCTION__));
	using namespace A3D;

	//m_bUseShadowMap = bUseShadowMap;
	if(!m_pA3DEngine->GetA3DShadowMap())
		bUseShadowMap = false;

	bool b1 = RenderWithEffect(pViewport, bUseShadowMap, bUseLightMap, true);

	bool b2 = true;
	if(bRenderAlpha)
		b2 = RenderAlpha(pViewport);
	return b1 && b2;
}

//	Render alpha lit models
bool A3DLitModelRender::RenderAlpha(A3DViewport* pViewport, bool bUseLightMap, bool bUseShadowMap)
{
	APtrArray<A3DLitMesh *> * pAlphaMeshes;
	if( m_idAlphaMeshArray == 0 )
		pAlphaMeshes = &m_aAlphaMeshes;
	else
		pAlphaMeshes = &m_aAlphaMeshes2;

	int nAlphaRefOld = m_pA3DDevice->GetAlphaRef();
	bool bAlphaTestOld = m_pA3DDevice->GetAlphaTestEnable();
	bool bZWriteEnable = m_pA3DDevice->GetZWriteEnable();
	bool bZTestEnable = m_pA3DDevice->GetZTestEnable();
	bool bLightingEnable = (m_pA3DDevice->GetDeviceRenderState(D3DRS_LIGHTING) == TRUE);
	bool bSpecularEnable = (m_pA3DDevice->GetDeviceRenderState(D3DRS_SPECULARENABLE) == TRUE);
    
	m_pA3DDevice->SetZWriteEnable(false);
	m_pA3DDevice->SetZTestEnable(true);
	m_pA3DDevice->SetLightingEnable(false);
	m_pA3DDevice->SetSpecularEnable(false);

	if(!m_pA3DEngine->GetA3DShadowMap())
		bUseShadowMap = false;

	A3DEnvironment* pEnv = m_pA3DEngine->GetA3DEnvironment();
	pEnv->SetViewport(pViewport);

	//设置LitModel渲染的全局参数
	A3DVECTOR4 vLitMapParam(pEnv->GetLightMapParam()->MaxLight, pEnv->GetLightMapParam()->MaxLightNight, 
        a_Max(pEnv->GetLightMapParam()->MaxLight, pEnv->GetLightMapParam()->MaxLightNight), m_fDNFactor);
	A3DEnvironment::LIGHTMAP_SHADOW_GLOBAL litModelGlobalParam;
	litModelGlobalParam.bShadowMap = bUseShadowMap;
	if(bUseShadowMap)
	{
		litModelGlobalParam.bHWPCF = m_pA3DEngine->GetA3DShadowMap()->IsUsingHWPcf();
		litModelGlobalParam.fShadowMapSize = (float)m_pA3DEngine->GetA3DShadowMap()->GetShadowMapSize();
		litModelGlobalParam.nShadowSampleNum = m_pA3DEngine->GetA3DShadowMap()->GetSampleLevel();
	}

	litModelGlobalParam.vLitMapParam = vLitMapParam;
	if(bUseShadowMap)
	{
		if(m_pA3DEngine->GetA3DShadowMap()->IsUsingHWPcf())
		{
			litModelGlobalParam.LitModelShadowMap = m_pA3DEngine->GetA3DShadowMap()->GetShadowMapTexture()->GetD3DTexture();
			litModelGlobalParam.LitModelShadowMapFloat = m_pSmallMap1x1->GetD3DTexture();
		}
		else
		{
			litModelGlobalParam.LitModelShadowMap = m_pSmallMap1x1->GetD3DTexture();
			litModelGlobalParam.LitModelShadowMapFloat = m_pA3DEngine->GetA3DShadowMap()->GetShadowMapFloatTexture()->GetD3DTexture();
		}
	}
	else
	{
		litModelGlobalParam.LitModelShadowMap = m_pSmallMap1x1->GetD3DTexture();
		litModelGlobalParam.LitModelShadowMapFloat = m_pSmallMap1x1->GetD3DTexture();
	}
	pEnv->SetLightMapShadowGlobalParam(&litModelGlobalParam);

	//Get alphaBlend state
	bool bAlphaBlend = m_pA3DDevice->GetDeviceRenderState(D3DRS_ALPHABLENDENABLE) ? true : false;


	pViewport->Active();

	//设置VS参数

	if(bUseShadowMap)
	{
		using namespace A3D;

		A3DMATRIX4 matShadowMVP;
		matShadowMVP = m_pA3DEngine->GetA3DShadowMap()->GetShadowMatrix();

		m_pVSHLSL->SetConstantMatrix("matLightWVP", matShadowMVP);
		m_pVSHLSL_LM->SetConstantMatrix("matLightWVP", matShadowMVP);

		m_pA3DDevice->SetTextureAddress(3, A3DTADDR_BORDER, A3DTADDR_BORDER);
		m_pA3DDevice->SetSamplerState(3, D3DSAMP_BORDERCOLOR,  0xffffffff);

		m_pA3DDevice->SetTextureAddress(4, A3DTADDR_BORDER, A3DTADDR_BORDER);
		m_pA3DDevice->SetSamplerState(4, D3DSAMP_BORDERCOLOR,  0xffffffff);

		if(m_pA3DEngine->GetA3DShadowMap()->IsUsingHWPcf() || m_pA3DEngine->GetA3DShadowMap()->IsUsingVSM())
		{
			m_pA3DDevice->SetTextureFilterType(3, A3DTEXF_LINEAR);
			m_pA3DDevice->SetTextureFilterType(4, A3DTEXF_LINEAR);
		}
		else
		{
			m_pA3DDevice->SetTextureFilterType(3, A3DTEXF_POINT);
			m_pA3DDevice->SetTextureFilterType(4, A3DTEXF_POINT);
		}
	}

	A3DMATRIX4 matVP= pViewport->GetCamera()->GetVPTM();
	m_pVSHLSL->SetConstantMatrix("matViewProj", matVP);
	m_pVSHLSL_LM->SetConstantMatrix("matViewProj", matVP);
	m_pVSHLSL->SetValue3f("g_vecEyePos", &pViewport->GetCamera()->GetPos());
	m_pVSHLSL_LM->SetValue3f("g_vecEyePos", &pViewport->GetCamera()->GetPos());

	{
		m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
		m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
		m_pA3DDevice->SetAlphaBlendEnable(true);

		A3DVECTOR4 vLitMapParam(pEnv->GetLightMapParam()->MaxLight, pEnv->GetLightMapParam()->MaxLightNight, 
            a_Max(pEnv->GetLightMapParam()->MaxLight, pEnv->GetLightMapParam()->MaxLightNight), m_fDNFactor);

		for(int i=0; i<pAlphaMeshes->GetSize(); i++)
		{

			A3DLitMesh * pLitMesh = (*pAlphaMeshes)[i];
			A3DTexture* pTexture = pLitMesh->GetTexturePtr();

			A3DEffect* pEffect = dynamic_cast<A3DEffect*>(pTexture);
			if(!pEffect)
				return false;

			//设置PS参数
			A3DEnvironment::LITMODEL_INDIVIDUAL litModelParam;
			litModelParam.fAlpha = float(pLitMesh->GetAlphaValue()) / 255.f;
			litModelParam.LitModelDiffuseTexture = pEffect->GetOriginTex()->GetD3DTexture();
			if(pLitMesh->IsUseLightMap() && pLitMesh->IsSupportLightMap())
			{
				litModelParam.bLightMap = true;
				litModelParam.bVertLit = false;			
				litModelParam.LitModelLightMap1 = NULL;
				litModelParam.LitModelLightMap2 = NULL;
				if( pLitMesh->GetLightMap() != NULL )
					litModelParam.LitModelLightMap1 = pLitMesh->GetLightMap()->GetD3DTexture();
				if( pLitMesh->GetNightLightMap() != NULL)
					litModelParam.LitModelLightMap2 = pLitMesh->GetNightLightMap()->GetD3DTexture();
			}
			else
			{
				litModelParam.LitModelLightMap1 = NULL;
				litModelParam.LitModelLightMap2 = NULL;
				litModelParam.bLightMap = false;
				litModelParam.bVertLit = true;
			}

			pEnv->SetLitModelIndividualParam(&litModelParam);

			m_pA3DDevice->SetTextureFilterType(0, A3DTEXF_LINEAR);
			m_pA3DDevice->SetTextureAddress(0, A3DTADDR_WRAP, A3DTADDR_WRAP);

			if( pLitMesh->GetLightMap() != NULL )
			{
				m_pA3DDevice->SetTextureFilterType(1, A3DTEXF_LINEAR);
				m_pA3DDevice->SetTextureAddress(1,A3DTADDR_CLAMP, A3DTADDR_CLAMP);
			}

			if( pLitMesh->GetNightLightMap() != NULL)
			{	
				m_pA3DDevice->SetTextureFilterType(2, A3DTEXF_LINEAR);
				m_pA3DDevice->SetTextureAddress(2,A3DTADDR_CLAMP, A3DTADDR_CLAMP);
			}

			if( pLitMesh->GetAlphaValue() != 255 )
			{
				// it's a fading non alpha mesh, so alpha compare also needed
				m_pA3DDevice->SetAlphaTestEnable(true);
				m_pA3DDevice->SetAlphaRef(84 * pLitMesh->GetAlphaValue() / 255);
			}
			else
			{
				// it's a alpha mesh, so no alpha compare needed
				m_pA3DDevice->SetAlphaTestEnable(false);
			}


			A3DCULLTYPE cullOld = m_pA3DDevice->GetFaceCull();
			if( pLitMesh->GetMaterial().Is2Sided() )
				m_pA3DDevice->SetFaceCull(A3DCULL_NONE);

			if(pLitMesh->GetVertexFormat() == A3DLITMESH_VERTEX_FORMAT_POS_NORMAL_DIFFUSE_UV)
				m_pVertDeclPosNormalDiffuseUV->Appear();
			else if(pLitMesh->GetVertexFormat() == A3DLITMESH_VERTEX_FORMAT_POS_NORMAL_UV1_UV2)
				m_pVertDeclPosNormalUVUV->Appear();
			else
				ASSERT( 0 && "ERROR!");
			pLitMesh->GetStream()->Appear(0, false);

			if(pLitMesh->IsUseLightMap() && pLitMesh->IsSupportLightMap())
			{
				m_pVSHLSL_LM->SetConstantMatrix("matWorld", pLitMesh->GetParentModel()->GetAbsoluteTM());
				m_pVSHLSL_LM->Appear(pEnv->GetCommonConstTable());
			}
			else
			{
				m_pVSHLSL->SetConstantMatrix("matWorld", pLitMesh->GetParentModel()->GetAbsoluteTM());
				m_pVSHLSL->Appear(pEnv->GetCommonConstTable());
			}

			pEffect->Appear(pEnv->GetCommonConstTable());

			m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, 0, pLitMesh->GetNumVerts(), 0, pLitMesh->GetNumFaces());

			m_pA3DDevice->SetFaceCull(cullOld);

			//restore texture 
			if(pLitMesh->GetLightMap() != NULL)
			{
				//pLitMesh->GetLightMap()->Disappear(1);	
				m_pA3DDevice->SetTextureFilterType(1, A3DTEXF_LINEAR);
				m_pA3DDevice->SetTextureAddress(1,A3DTADDR_WRAP, A3DTADDR_WRAP);
			}
			if( pLitMesh->GetNightLightMap() != NULL)
			{
				//pLitMesh->GetNightLightMap()->Disappear(2);
				m_pA3DDevice->SetTextureFilterType(2, A3DTEXF_LINEAR);
				m_pA3DDevice->SetTextureAddress(2,A3DTADDR_WRAP, A3DTADDR_WRAP);
			}

			pEffect->Disappear();
		}
	}

    //if (GetAsyncKeyState('M') >= 0)
    //    RenderShaderMeshesHLSL(pViewport);
    //else
        RenderShaderMeshes(pViewport);
    
	//restore render state
	m_pA3DDevice->SetLightingEnable(bLightingEnable);
	m_pA3DDevice->SetSpecularEnable(bSpecularEnable);

	//clear shader
	m_pA3DDevice->ClearPixelShader();
	m_pA3DDevice->ClearVertexShader();


	//Restore alpha blending state
	m_pA3DDevice->SetAlphaBlendEnable(bAlphaBlend);
	m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
	m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);

	m_pA3DDevice->SetAlphaRef(nAlphaRefOld);
	m_pA3DDevice->SetAlphaTestEnable(bAlphaTestOld);

	m_pA3DDevice->SetZWriteEnable(bZWriteEnable);
	m_pA3DDevice->SetZTestEnable(bZTestEnable);

    return true;
}

//	Render sdr models with fixed pipeline
bool A3DLitModelRender::RenderShaderMeshes(A3DViewport* pViewport)
{
    APtrArray<A3DLitMesh *> * pAlphaMeshes;
    pAlphaMeshes = &m_aShaderMeshes;
    
    if (pAlphaMeshes->GetSize() == 0)
        return true;

    bool bClipPlaneEnable = (m_pA3DDevice->GetDeviceRenderState(D3DRS_CLIPPLANEENABLE) & D3DCLIPPLANE0) != 0;

    // 此时的clip plane是clip空间的，固定管线要转换为世界空间
    D3DXPLANE clipPlaneInClip;
    m_pA3DDevice->GetClipPlane(0, (float*)&clipPlaneInClip);

    if (bClipPlaneEnable)
    {
        A3DMATRIX4 matVP_T = pViewport->GetCamera()->GetVPTM();
        matVP_T.Transpose();
        D3DXPLANE clipPlaneInWorld;
        D3DXPlaneTransform(&clipPlaneInWorld, &clipPlaneInClip,(const D3DXMATRIX*) &matVP_T);
        m_pA3DDevice->SetClipPlane(0, (float*)&clipPlaneInWorld);
    }

    int nAlphaRefOld = m_pA3DDevice->GetAlphaRef();
    bool bAlphaTestOld = m_pA3DDevice->GetAlphaTestEnable();
    bool bZWriteEnable = m_pA3DDevice->GetZWriteEnable();
    bool bZTestEnable = m_pA3DDevice->GetZTestEnable();
    bool bLightingEnable = (m_pA3DDevice->GetDeviceRenderState(D3DRS_LIGHTING) == TRUE);
    bool bSpecularEnable = (m_pA3DDevice->GetDeviceRenderState(D3DRS_SPECULARENABLE) == TRUE);
    bool bAlphaBlend = m_pA3DDevice->GetDeviceRenderState(D3DRS_ALPHABLENDENABLE) == TRUE;

    DWORD dwArg1Old = m_pA3DDevice->GetDeviceTextureStageState(0, D3DTSS_COLORARG1);
    DWORD dwArg2Old = m_pA3DDevice->GetDeviceTextureStageState(0, D3DTSS_COLORARG2);
    DWORD dwOpOld = m_pA3DDevice->GetDeviceTextureStageState(0, D3DTSS_COLOROP);
    DWORD dwAlphaArg1Old = m_pA3DDevice->GetDeviceTextureStageState(0, D3DTSS_ALPHAARG1);
    DWORD dwAlphaArg2Old = m_pA3DDevice->GetDeviceTextureStageState(0, D3DTSS_ALPHAARG2);
    DWORD dwAlphaOpOld = m_pA3DDevice->GetDeviceTextureStageState(0, D3DTSS_ALPHAOP);
    DWORD dwOpOld1 = m_pA3DDevice->GetDeviceTextureStageState(1, D3DTSS_COLOROP);
    m_pA3DDevice->SetTextureColorArgs(0, A3DTA_DIFFUSE, A3DTA_CURRENT);
    m_pA3DDevice->SetTextureColorOP(0, A3DTOP_SELECTARG1);
    m_pA3DDevice->SetTextureAlphaArgs(0, A3DTA_DIFFUSE, A3DTA_CURRENT);
    m_pA3DDevice->SetTextureAlphaOP(0, A3DTOP_SELECTARG1);
    m_pA3DDevice->SetTextureColorOP(1, A3DTOP_DISABLE);


    A3DCULLTYPE cullOld = m_pA3DDevice->GetFaceCull();

    m_pA3DDevice->SetZWriteEnable(false);
    m_pA3DDevice->SetZTestEnable(true);
    m_pA3DDevice->SetLightingEnable(false);
    m_pA3DDevice->SetSpecularEnable(false);
    m_pA3DDevice->SetLightingEnable(false);
    m_pA3DDevice->ClearVertexShader();
    m_pA3DDevice->ClearPixelShader();
    m_pA3DDevice->SetAlphaTestEnable(false);
    m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
    m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
    m_pA3DDevice->SetAlphaBlendEnable(true);
    A3DEnvironment* pEnv = m_pA3DEngine->GetA3DEnvironment();

    m_pA3DDevice->SetFogEnable(pEnv->GetGlobalFogEnable());

    pViewport->Active();

    {

        A3DVECTOR4 vLitMapParam(pEnv->GetLightMapParam()->MaxLight, pEnv->GetLightMapParam()->MaxLightNight, 
            a_Max(pEnv->GetLightMapParam()->MaxLight, pEnv->GetLightMapParam()->MaxLightNight), m_fDNFactor);

        for(int i=0; i < pAlphaMeshes->GetSize(); i++)
        {
            A3DLitMesh * pLitMesh = (*pAlphaMeshes)[i];
            A3DTexture* pTexture = pLitMesh->GetTexturePtr();
            
            if (!pTexture)
                continue;
            
            m_pA3DDevice->SetWorldMatrix(pLitMesh->GetParentModel()->GetAbsoluteTM());
            pTexture->Appear();
    
            if( pLitMesh->GetMaterial().Is2Sided() )
                m_pA3DDevice->SetFaceCull(A3DCULL_NONE);
            //else
            //    m_pA3DDevice->SetFaceCull(A3DCULL_CCW);
            if (pLitMesh->GetVertexFormat() == A3DLITMESH_VERTEX_FORMAT_POS_NORMAL_DIFFUSE_UV)
                m_pVertDeclPosNormalDiffuseUV->Appear();
            else if (pLitMesh->GetVertexFormat() == A3DLITMESH_VERTEX_FORMAT_POS_NORMAL_UV1_UV2)
                m_pVertDeclPosNormalUVUV->Appear();
            else
                ASSERT( 0 && "ERROR!");
            pLitMesh->GetStream()->Appear(0, false);

            m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, 0, pLitMesh->GetNumVerts(), 0, pLitMesh->GetNumFaces());

            pTexture->Disappear();
        }
    }

    //restore render state
    m_pA3DDevice->SetLightingEnable(bLightingEnable);
    m_pA3DDevice->SetSpecularEnable(bSpecularEnable);

    //clear shader
    m_pA3DDevice->ClearPixelShader();
    m_pA3DDevice->ClearVertexShader();


    //Restore alpha blending state
    m_pA3DDevice->SetAlphaBlendEnable(bAlphaBlend);
    m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
    m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);

    m_pA3DDevice->SetAlphaRef(nAlphaRefOld);
    m_pA3DDevice->SetAlphaTestEnable(bAlphaTestOld);

    m_pA3DDevice->SetZWriteEnable(bZWriteEnable);
    m_pA3DDevice->SetZTestEnable(bZTestEnable);
    m_pA3DDevice->SetFaceCull(cullOld);

    m_pA3DDevice->SetTextureColorArgs(0, dwArg1Old, dwArg2Old);
    m_pA3DDevice->SetTextureColorOP(0, (A3DTEXTUREOP)dwOpOld);
    m_pA3DDevice->SetTextureAlphaArgs(0, dwAlphaArg1Old, dwAlphaArg2Old);
    m_pA3DDevice->SetTextureAlphaOP(0, (A3DTEXTUREOP)dwAlphaOpOld);
    m_pA3DDevice->SetTextureColorOP(1, (A3DTEXTUREOP)dwOpOld1);
    m_pA3DDevice->SetFogEnable(false);

    if (bClipPlaneEnable)
    {
        m_pA3DDevice->SetClipPlane(0, (float*)&clipPlaneInClip);
    }

    return true;
}

//	Render sdr models with fixed pipeline
bool A3DLitModelRender::RenderShaderMeshesHLSL(A3DViewport* pViewport)
{
    if (!m_pPSHLSL_Fixed)
        return false;
    APtrArray<A3DLitMesh *> * pAlphaMeshes;
    pAlphaMeshes = &m_aShaderMeshes;

    if (pAlphaMeshes->GetSize() == 0)
        return true;

    int nAlphaRefOld = m_pA3DDevice->GetAlphaRef();
    bool bAlphaTestOld = m_pA3DDevice->GetAlphaTestEnable();
    bool bZWriteEnable = m_pA3DDevice->GetZWriteEnable();
    bool bZTestEnable = m_pA3DDevice->GetZTestEnable();
    bool bLightingEnable = (m_pA3DDevice->GetDeviceRenderState(D3DRS_LIGHTING) == TRUE);
    bool bSpecularEnable = (m_pA3DDevice->GetDeviceRenderState(D3DRS_SPECULARENABLE) == TRUE);
    bool bAlphaBlend = m_pA3DDevice->GetDeviceRenderState(D3DRS_ALPHABLENDENABLE) == TRUE;

    A3DCULLTYPE cullOld = m_pA3DDevice->GetFaceCull();

    m_pA3DDevice->SetZWriteEnable(false);
    m_pA3DDevice->SetZTestEnable(true);
    m_pA3DDevice->SetLightingEnable(false);
    m_pA3DDevice->SetSpecularEnable(false);
    m_pA3DDevice->SetLightingEnable(false);
    m_pA3DDevice->ClearVertexShader();
    m_pA3DDevice->ClearPixelShader();
    m_pA3DDevice->SetAlphaTestEnable(false);
    m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
    m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
    m_pA3DDevice->SetAlphaBlendEnable(true);
    A3DEnvironment* pEnv = m_pA3DEngine->GetA3DEnvironment();

    pViewport->Active();

    static const int nMaxLayer = 3;
    A3DVECTOR4 fColorFactors[nMaxLayer];
    A3DVECTOR4 fAlphaFactors[nMaxLayer];
    A3DVECTOR4 fTextureMat[nMaxLayer * 2];
    m_pPSHLSL_Fixed->Appear(pEnv->GetCommonConstTable());

    A3DMATRIX4 matVP= pViewport->GetCamera()->GetVPTM();
    m_pVSHLSL->SetConstantMatrix("matViewProj", matVP);
    m_pVSHLSL->SetValue3f("g_vecEyePos", &pViewport->GetCamera()->GetPos());
    m_pVertDeclPosNormalDiffuseUV->Appear();

    m_pVSHLSL->Appear(pEnv->GetCommonConstTable());
    {

        A3DVECTOR4 vLitMapParam(pEnv->GetLightMapParam()->MaxLight, pEnv->GetLightMapParam()->MaxLightNight, 
            a_Max(pEnv->GetLightMapParam()->MaxLight, pEnv->GetLightMapParam()->MaxLightNight), m_fDNFactor);

        for(int i=0; i < pAlphaMeshes->GetSize(); i++)
        {
            A3DLitMesh * pLitMesh = (*pAlphaMeshes)[i];
            A3DTexture* pTexture = pLitMesh->GetTexturePtr();

            if (!pTexture)
                continue;
            
            A3DShader* pShader = (A3DShader*) pTexture;

            for (int iLayer = 0; iLayer < nMaxLayer; iLayer++)
            {
                if (iLayer < pShader->GetStageNum())
                {
                    const SHADERSTAGE* pStage = &pShader->GetStage(iLayer);
                    fColorFactors[iLayer] = pStage->vColorFactor;
                    fAlphaFactors[iLayer] = pStage->vAlphaFactor;
                    A3DMATRIX4 matTex = pShader->GetTextureMatrix(iLayer);
                    fTextureMat[iLayer * 2].Set(matTex._11, matTex._21, matTex._31, 0);
                    fTextureMat[iLayer * 2 + 1].Set(matTex._12, matTex._22, matTex._32, 0);
                }
                else
                {
                    fColorFactors[iLayer].Set(0, 1, 0, 0);
                    fAlphaFactors[iLayer].Set(0, 1, 0, 0);
                    A3DMATRIX4 matTex = pShader->GetTextureMatrix(iLayer);
                    fTextureMat[iLayer * 2].Set(0, 1, 0, 0);
                    fTextureMat[iLayer * 2 + 1].Set(0, 1, 0, 0);
                }
            }
            m_pPSHLSL_Fixed->SetConstantArrayF("g_colorFactors", fColorFactors, nMaxLayer);
            m_pPSHLSL_Fixed->SetConstantArrayF("g_alphaFactors", fAlphaFactors, nMaxLayer);
            m_pPSHLSL_Fixed->SetConstantArrayF("g_texMatrix", fTextureMat, nMaxLayer * 2);

            pTexture->Appear();

            if( pLitMesh->GetMaterial().Is2Sided() )
                m_pA3DDevice->SetFaceCull(A3DCULL_NONE);
            else
                m_pA3DDevice->SetFaceCull(A3DCULL_CCW);

            m_pVSHLSL->SetConstantMatrix("matWorld", pLitMesh->GetParentModel()->GetAbsoluteTM());

            if (pLitMesh->GetVertexFormat() != A3DLITMESH_VERTEX_FORMAT_POS_NORMAL_DIFFUSE_UV)
            {
                ASSERT( 0 && "Model with SDR shader should be vertlit!");
                continue;
            }
            pLitMesh->GetStream()->Appear(0, false);

            m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, 0, pLitMesh->GetNumVerts(), 0, pLitMesh->GetNumFaces());

            pTexture->Disappear();
        }
    }

    m_pPSHLSL_Fixed->Disappear();
    m_pVSHLSL->Disappear();

    //restore render state
    m_pA3DDevice->SetLightingEnable(bLightingEnable);
    m_pA3DDevice->SetSpecularEnable(bSpecularEnable);

    //clear shader
    m_pA3DDevice->ClearPixelShader();
    m_pA3DDevice->ClearVertexShader();


    //Restore alpha blending state
    m_pA3DDevice->SetAlphaBlendEnable(bAlphaBlend);
    m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
    m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);

    m_pA3DDevice->SetAlphaRef(nAlphaRefOld);
    m_pA3DDevice->SetAlphaTestEnable(bAlphaTestOld);

    m_pA3DDevice->SetZWriteEnable(bZWriteEnable);
    m_pA3DDevice->SetZTestEnable(bZTestEnable);
    m_pA3DDevice->SetFaceCull(cullOld);

    return true;
}


//	Reset render information, this function should be called every frame
void A3DLitModelRender::ResetRenderInfo(bool bAlpha)
{
	//shadow map
	int i = 0;

	for( i = 0; i < m_aVertlitSlots.GetSize(); i++)
	{
		m_aVertlitSlots[i]->pTexture = NULL;
		m_aVertlitSlots[i]->LitMeshes.RemoveAll(false);
	}

    for (int iGroup = 0; iGroup < MAX_SLOT_GROUP; iGroup++)
    {
        m_aLightmapModelGroups[iGroup].RemoveAll(false);
    }

    if( !bAlpha )
	{
		for( i = 0; i<m_aRenderSlots.GetSize(); i++)
		{
			m_aRenderSlots[i]->LitMeshes.RemoveAll(false);
			m_aRenderSlots[i]->pTexture = NULL;
			m_aRenderSlots[i]->pReflectTexture = NULL;
		}
	}
	else
	{
		if( m_idAlphaMeshArray == 0 )
			m_aAlphaMeshes.RemoveAll(false);
		else
			m_aAlphaMeshes2.RemoveAll(false);
        m_aShaderMeshes.RemoveAll(false);
	}
}

bool A3DLitModelRender::RenderWithLightMap(A3DViewport* pViewport, bool bHDR, bool bRenderAlpha, bool bRenderWithShadowMap)
{

	if(!m_pA3DEngine->GetA3DShadowMap())
		bRenderWithShadowMap = false;

	if(!m_bSupportSM20)
	{
		return Render(pViewport, bRenderAlpha, true, bRenderWithShadowMap);
	}

	return RenderWithEffect(pViewport, bRenderWithShadowMap, true, true);
}

void A3DLitModelRender::SetDNFactor(float f)
{
	m_fDNFactor = f;
	if( m_fDNFactor < 0.0f )
		m_fDNFactor = 0.0f;
	if( m_fDNFactor > 1.0f )
		m_fDNFactor = 1.0f;
}

bool A3DLitModelRender::RenderForRefract(A3DViewport * pViewport, float vRefractSurfaceHeight)
{
	bool bFogTable = m_pA3DDevice->GetFogTableEnable();

	m_pA3DDevice->SetFogTableEnable(false);
	m_bIsRenderForRefract = true;
	m_vRefractSurfaceHeight = vRefractSurfaceHeight;

	Render(pViewport, false);

	m_pA3DDevice->SetFogTableEnable(bFogTable);
	m_bIsRenderForRefract = false;
	return true;
}

//纯粹的DP
bool A3DLitModelRender::RenderRaw( A3DViewport* pViewport )
{
	using namespace A3D;

	{
		for(int i=0; i<m_aRenderSlots.GetSize(); i++)
		{
			RENDERSLOT * pSlot = m_aRenderSlots[i];

			if( pSlot->pTexture == NULL )
				break; // meet empty slot here, so there is no more after this

			A3DShader* pA3DShader = dynamic_cast<A3DShader*>(pSlot->pTexture);
			A3DEffect* pA3DEffect = dynamic_cast<A3DEffect*>(pSlot->pTexture);

			A3DVECTOR4 mask[5];
			mask[0] = A3DVECTOR4(1,0,0,0);
			mask[1] = A3DVECTOR4(0,1,0,0);
			mask[2] = A3DVECTOR4(0,0,1,0);
			mask[3] = A3DVECTOR4(0,0,0,1);
			mask[4] = A3DVECTOR4(1,1,1,1);
			if(pA3DShader)
			{
				if(pA3DShader->GetTransparentMaskTexture())
				{
					pA3DShader->GetTransparentMaskTexture()->Appear(0);
					int chan = pA3DShader->GetTransparentChannel();
					m_pA3DDevice->SetPixelShaderConstants(0, &mask[chan].x, 1);
					//m_pA3DDevice->GetD3DDevice()->SetPixelShaderConstantF(0, &mask[chan].x, 1);
				}
				else
				{
					m_pA3DDevice->SetPixelShaderConstants(0, &mask[4].x, 1);
					m_pA3DDevice->ClearTexture(0);
					//m_pA3DDevice->GetD3DDevice()->SetPixelShaderConstantF(0, &mask[4].x, 1);
					//m_pA3DDevice->GetD3DDevice()->SetTexture(0, NULL);
				}
			}
			else if(pA3DEffect)
			{
				pA3DEffect->AppearTextureOnly(0);
				m_pA3DDevice->SetPixelShaderConstants(0, &mask[3].x, 1);
				//m_pA3DDevice->GetD3DDevice()->SetTexture(0, pA3DEffect->GetD3DBaseTexture());
				//m_pA3DDevice->GetD3DDevice()->SetPixelShaderConstantF(0, &mask[3].x, 1);
			}
			else//这时应该已经是普通的纹理了.
			{
				pSlot->pTexture->Appear();
				m_pA3DDevice->SetPixelShaderConstants(0, &mask[3].x, 1);
				//m_pA3DDevice->GetD3DDevice()->SetPixelShaderConstantF(0, &mask[3].x, 1);
			}

			m_pA3DDevice->SetTextureAddress(0, A3DTADDR_WRAP, A3DTADDR_WRAP);
			m_pA3DDevice->SetTextureFilterType(0, A3DTEXF_LINEAR);

			A3DCULLTYPE cullOld = m_pA3DDevice->GetFaceCull();
			if( pSlot->b2Sided )
				m_pA3DDevice->SetFaceCull(A3DCULL_NONE);

			for(int n=0; n<pSlot->LitMeshes.GetSize(); n++)
			{
				A3DLitMesh * pLitMesh = pSlot->LitMeshes[n];

				if(pLitMesh->GetVertexFormat() == A3DLITMESH_VERTEX_FORMAT_POS_NORMAL_DIFFUSE_UV)
					m_pVertDeclPosNormalDiffuseUV->Appear();
				else if(pLitMesh->GetVertexFormat() == A3DLITMESH_VERTEX_FORMAT_POS_NORMAL_UV1_UV2)
					m_pVertDeclPosNormalUVUV->Appear();
				else
					ASSERT( 0 && "ERROR!");
				pLitMesh->GetStream()->Appear(0, false);

				m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, 0, pLitMesh->GetNumVerts(), 0, pLitMesh->GetNumFaces());
			}

			if( pSlot->b2Sided )
				m_pA3DDevice->SetFaceCull(cullOld);

		}
	}
	m_pA3DDevice->ClearTexture(0);
	//m_pA3DDevice->GetD3DDevice()->SetTexture(0, NULL);
	A3DVECTOR4 vZero = A3DVECTOR4(0,0,0,0);
	m_pA3DDevice->SetPixelShaderConstants(0, &vZero.x, 1);
	//m_pA3DDevice->GetD3DDevice()->SetPixelShaderConstantF(0, &vZero.x, 1);

	return true;
}

bool A3DLitModelRender::RenderWithHLSL(A3DViewport* pViewport, 
                                       APtrArray<RENDERSLOT*>* pSlotsArray, int nSlotGroup,
                                       A3DVertexDecl* pVertDecl,
                                       A3DHLSL* pVS, A3DHLSL* pPS,
                                       bool bShadowMap, bool bLightMap)
{
    if (!pVS || !pPS)
        return false;

    if (nSlotGroup <= 0)
        return true;

    //Get alphaBlend state

    A3DEnvironment* pEnv = m_pA3DEngine->GetA3DEnvironment();

    // 设置阴影相关的VS参数
    if(bShadowMap)
    {
        using namespace A3D;

        A3DMATRIX4 matShadowMVP;
        matShadowMVP = m_pA3DEngine->GetA3DShadowMap()->GetShadowMatrix();

        pVS->SetConstantMatrix("matLightWVP", matShadowMVP);

    }

    //设置VS
    A3DMATRIX4 matVP = pViewport->GetCamera()->GetVPTM();
    pVS->SetConstantMatrix("matViewProj", matVP);
    pVS->SetValue3f("g_vecEyePos", &pViewport->GetCamera()->GetPos());

    const A3DHLSL::CONSTMAPDESC* pDesc = pVS->GetConstMapByName("matWorld");
    int iWorldMatIndex = pDesc ? pDesc->cd.cd.RegisterIndex : -1;

    pVS->Appear(pEnv->GetCommonConstTable());	

    pVertDecl->Appear();

    A3DTexture* pErrorLmTex = m_pA3DEngine->GetA3DTextureMan()->GetErrLMTexture();
    A3DTexture* pErrorTex = m_pA3DEngine->GetA3DTextureMan()->GetErrorTexture();

    pPS->Appear(pEnv->GetCommonConstTable());

    bool bIdentityWorldMat = false;

    bool bUseDetail = m_bDetailMap;

    A3DCULLTYPE cullOld = m_pA3DDevice->GetFaceCull();
    int nSlot = 0;
    for (int iGroup = 0; iGroup < nSlotGroup; iGroup++)
    {
        APtrArray<RENDERSLOT*>& aSlots = pSlotsArray[iGroup];
        for(int i = 0; i < aSlots.GetSize(); i++)
        {
            nSlot++;
            RENDERSLOT * pSlot = aSlots[i];

            if(pSlot->pTexture == NULL)
                break;

            pSlot->pTexture->Appear(0);
    
            // now only vertex lit rendering will call this procedure
            //if (bLightMap)
            //{
            //    if (pSlot->pLightMap != NULL)
            //    {
            //        pSlot->pLightMap->Appear(1);
            //    }
            //    else if (pErrorLmTex)
            //    {
            //        pErrorLmTex->Appear(1);
            //    }
            //    if (pSlot->pNightLightMap != NULL)
            //    {
            //        pSlot->pNightLightMap->Appear(2);
            //    }
            //    else if (pErrorLmTex)
            //    {
            //        pErrorLmTex->Appear(2);
            //    }
            //}

            m_pA3DDevice->SetFaceCull(pSlot->b2Sided ? A3DCULL_NONE : cullOld);

            for (int n = 0; n < pSlot->LitMeshes.GetSize(); n++)
            {
                A3DLitMesh * pLitMesh = pSlot->LitMeshes[n];

                pLitMesh->GetStream()->Appear(0, false);

                if (pLitMesh->GetParentModel()->IsNoTransform())
                {
                    if (!bIdentityWorldMat)
                    {
                        A3DMATRIX4 matWorld = a3d_IdentityMatrix();
                        m_pA3DDevice->SetVertexShaderConstants(iWorldMatIndex, &matWorld, 4);
                        bIdentityWorldMat = true;
                    }
                }
                else
                {
                    A3DMATRIX4 matWorld = pLitMesh->GetParentModel()->GetAbsoluteTM();
                    matWorld.Transpose();
                    m_pA3DDevice->SetVertexShaderConstants(iWorldMatIndex, &matWorld, 4);
                }

                if (bUseDetail)
                {
                    if (pLitMesh->GetDetailTexture() && pLitMesh->GetDetailTexture()->GetD3DTexture())
                    {
                        pLitMesh->GetDetailTexture()->Appear(5);
                    }
                    else
                    {
                        m_pEmptyDetailMap->Appear(5);
                    }
                    A3DVECTOR4 vDetailParam(pLitMesh->GetDetailTile(), pLitMesh->GetDetailHardness(), 0, 0);
                    m_pA3DDevice->SetPixelShaderConstants(7, &vDetailParam.x, 1);
                }
                m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, 0, pLitMesh->GetNumVerts(), 0, pLitMesh->GetNumFaces());
            }
        }
    }
    m_pA3DDevice->SetFaceCull(cullOld);
    return true;
}

bool A3DLitModelRender::RenderWithEffect(A3DViewport* pViewport, bool bShadowMap, bool bLightMap, bool bFog)
{
    if(!m_pA3DEngine->GetA3DShadowMap())
        bShadowMap = false;

    //Get alphaBlend state
    bool bAlphaBlend = m_pA3DDevice->GetDeviceRenderState(D3DRS_ALPHABLENDENABLE) ? true : false;

    m_pA3DDevice->SetAlphaBlendEnable(false);
    m_pA3DDevice->SetLightingEnable(false);
    m_pA3DDevice->SetSpecularEnable(false);

    //pViewport->Active();

    A3DEnvironment* pEnv = m_pA3DEngine->GetA3DEnvironment();
    pEnv->SetViewport(pViewport);

    A3DEnvironment::LITMODEL_INDIVIDUAL litModelParam;
    litModelParam.bLightMap = true;
    if(!bLightMap)
        litModelParam.bLightMap = false;
    litModelParam.bVertLit = false;
    litModelParam.fAlpha = 1.f;
    litModelParam.LitModelDiffuseTexture = NULL;
    litModelParam.LitModelLightMap1 = NULL;
    litModelParam.LitModelLightMap2 = NULL;
    pEnv->SetLitModelIndividualParam(&litModelParam);

    //设置LitModel渲染的全局参数
    A3DVECTOR4 vLitMapParam(pEnv->GetLightMapParam()->MaxLight, pEnv->GetLightMapParam()->MaxLightNight, 
        a_Max(pEnv->GetLightMapParam()->MaxLight, pEnv->GetLightMapParam()->MaxLightNight),
        m_fDNFactor);

    A3DEnvironment::LIGHTMAP_SHADOW_GLOBAL litModelGlobalParam;
    litModelGlobalParam.bShadowMap = bShadowMap;
    if(bShadowMap)
    {
        litModelGlobalParam.bHWPCF = m_pA3DEngine->GetA3DShadowMap()->IsUsingHWPcf();
        litModelGlobalParam.fShadowMapSize = (float)m_pA3DEngine->GetA3DShadowMap()->GetShadowMapSize();
        litModelGlobalParam.nShadowSampleNum = 1;// m_pA3DEngine->GetA3DShadowMap()->GetSampleNum();
    }

    litModelGlobalParam.vLitMapParam = vLitMapParam;
    if(bShadowMap)
    {
        if(m_pA3DEngine->GetA3DShadowMap()->IsUsingHWPcf())
        {
            litModelGlobalParam.LitModelShadowMap = m_pA3DEngine->GetA3DShadowMap()->GetShadowMapTexture()->GetD3DTexture();
            litModelGlobalParam.LitModelShadowMapFloat = m_pSmallMap1x1->GetD3DTexture();
        }
        else
        {
            litModelGlobalParam.LitModelShadowMap = m_pSmallMap1x1->GetD3DTexture();
            litModelGlobalParam.LitModelShadowMapFloat = m_pA3DEngine->GetA3DShadowMap()->GetShadowMapFloatTexture()->GetD3DTexture();
        }
    }
    else
    {
        litModelGlobalParam.LitModelShadowMap = m_pSmallMap1x1->GetD3DTexture();
        litModelGlobalParam.LitModelShadowMapFloat = m_pSmallMap1x1->GetD3DTexture();
    }

    pEnv->SetLightMapShadowGlobalParam(&litModelGlobalParam);

    if (bShadowMap)
    {
        m_pA3DDevice->SetTextureAddress(3, A3DTADDR_BORDER, A3DTADDR_BORDER);
        m_pA3DDevice->SetSamplerState(3, D3DSAMP_BORDERCOLOR,  0xffffffff);

        m_pA3DDevice->SetTextureAddress(4, A3DTADDR_BORDER, A3DTADDR_BORDER);
        m_pA3DDevice->SetSamplerState(4, D3DSAMP_BORDERCOLOR,  0xffffffff);

        if(m_pA3DEngine->GetA3DShadowMap()->IsUsingHWPcf() || m_pA3DEngine->GetA3DShadowMap()->IsUsingVSM())
        {
            m_pA3DDevice->SetTextureFilterType(3, A3DTEXF_LINEAR);
            m_pA3DDevice->SetTextureFilterType(4, A3DTEXF_LINEAR);
        }
        else
        {
            m_pA3DDevice->SetTextureFilterType(3, A3DTEXF_POINT);
            m_pA3DDevice->SetTextureFilterType(4, A3DTEXF_POINT);
        }
    }

    //设置纹理采样方法
    m_pA3DDevice->SetTextureFilterType(0, A3DTEXF_LINEAR);
    m_pA3DDevice->SetTextureAddress(0, A3DTADDR_WRAP, A3DTADDR_WRAP);
    m_pA3DDevice->SetTextureFilterType(1, A3DTEXF_LINEAR);
    m_pA3DDevice->SetTextureAddress(1,A3DTADDR_CLAMP, A3DTADDR_CLAMP);
    m_pA3DDevice->SetTextureFilterType(2, A3DTEXF_LINEAR);
    m_pA3DDevice->SetTextureAddress(2,A3DTADDR_CLAMP, A3DTADDR_CLAMP);

    m_pA3DDevice->SetTextureFilterType(5, A3DTEXF_LINEAR);
    m_pA3DDevice->SetTextureAddress(5, A3DTADDR_WRAP, A3DTADDR_WRAP);

    int nSlotShadow = 0;
    
    if (bShadowMap)
    {
        A3DShadowMap* pShadowMap = m_pA3DEngine->GetA3DShadowMap();
        if (pShadowMap)
        {
            float fShadowRadius = pShadowMap->GetShadowRadius();
            while (nSlotShadow < MAX_SLOT_GROUP && nSlotShadow * 300.0f / MAX_SLOT_GROUP < fShadowRadius)
            {
                nSlotShadow++;
            }
        }

        int nShaderType = SHADER_SHADOWMAP;
        if (pShadowMap->GetSampleLevel() > 0)
            nShaderType |= SHADER_SHADOWMAP_HIGH;
        if (m_bRealtime)
            nShaderType |= SHADER_REALTIME;
        if (bLightMap)
            nShaderType |= SHADER_LIGHTMAP;
        if (m_bDetailMap)
            nShaderType |= SHADER_DETAILMAP;

        RenderWithHLSLByModel(pViewport, m_aLightmapModelGroups, nSlotShadow, m_pVertDeclPosNormalUVUV, 
            m_pVSShaders[nShaderType], m_pPSShaders[nShaderType], bShadowMap, bLightMap);
    }

    int nShaderType = SHADER_VERTLIT;
    if (bShadowMap)
        nShaderType |= SHADER_SHADOWMAP;
    if (m_bDetailMap)
        nShaderType |= SHADER_DETAILMAP;

    RenderWithHLSL(pViewport, &m_aVertlitSlots, 1, m_pVertDeclPosNormalDiffuseUV, 
        m_pVSShaders[nShaderType], m_pPSShaders[nShaderType], bShadowMap, false);

    nShaderType = SHADER_UNLIT;
    if (m_bRealtime)
        nShaderType |= SHADER_REALTIME;
    if (bLightMap)
        nShaderType |= SHADER_LIGHTMAP;
    if (m_bDetailMap)
        nShaderType |= SHADER_DETAILMAP;

    RenderWithHLSLByModel(pViewport, m_aLightmapModelGroups + nSlotShadow, MAX_SLOT_GROUP - nSlotShadow, m_pVertDeclPosNormalUVUV, 
        m_pVSShaders[nShaderType], m_pPSShaders[nShaderType], false, bLightMap);
    
    //restore render state
    m_pA3DDevice->SetLightingEnable(true);
    m_pA3DDevice->SetSpecularEnable(true);

    //clear shader
    m_pA3DDevice->ClearPixelShader();
    m_pA3DDevice->ClearVertexShader();

    //Restore alpha blending state
    m_pA3DDevice->SetAlphaBlendEnable(bAlphaBlend);
    m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
    m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);

    m_pA3DDevice->ClearTexture(0);
    m_pA3DDevice->ClearTexture(1);
    m_pA3DDevice->ClearTexture(2);
    m_pA3DDevice->ClearTexture(3);
    m_pA3DDevice->ClearTexture(4);
    m_pA3DDevice->ClearTexture(5);

    return true;
}

bool A3DLitModelRender::RenderWithHLSLByModel(A3DViewport* pViewport, 
                                              APtrArray<A3DLitModel*>* pModelsArray, int nModelGroup,
                                              A3DVertexDecl* pVertDecl,
                                              A3DHLSL* pVS, A3DHLSL* pPS,
                                              bool bShadowMap, bool bLightMap)
{
    if (!pVS || !pPS)
        return false;

    if (nModelGroup <= 0)
        return true;

    //Get alphaBlend state

    A3DEnvironment* pEnv = m_pA3DEngine->GetA3DEnvironment();

    // 设置阴影相关的VS参数
    if(bShadowMap)
    {
        using namespace A3D;

        A3DMATRIX4 matShadowMVP;
        matShadowMVP = m_pA3DEngine->GetA3DShadowMap()->GetShadowMatrix();

        pVS->SetConstantMatrix("matLightWVP", matShadowMVP);

    }

    //设置VS
    A3DMATRIX4 matVP = pViewport->GetCamera()->GetVPTM();
    pVS->SetConstantMatrix("matViewProj", matVP);
    pVS->SetValue3f("g_vecEyePos", &pViewport->GetCamera()->GetPos());
    pVS->Setup1f("g_fGamma", pEnv->GetLightMapParam()->Gamma);
    const A3DHLSL::CONSTMAPDESC* pDesc = pVS->GetConstMapByName("matWorld");
    int iWorldMatIndex = pDesc ? pDesc->cd.cd.RegisterIndex : -1;

    pVS->Appear(pEnv->GetCommonConstTable());	

    pVertDecl->Appear();

    A3DTexture* pErrorLmTex = m_pA3DEngine->GetA3DTextureMan()->GetErrLMTexture();
    A3DTexture* pErrorTex = m_pA3DEngine->GetA3DTextureMan()->GetErrorTexture();

    pPS->Appear(pEnv->GetCommonConstTable());

    bool bUseDetail = m_bDetailMap;

    bool bIdentityWorldMat = false;
    A3DCULLTYPE cullOld = m_pA3DDevice->GetFaceCull();

    AArray<A3DLitMesh*> aMeshes(100, 100);

    for (int iGroup = 0; iGroup < nModelGroup; iGroup++)
    {
        APtrArray<A3DLitModel*>& aModels = pModelsArray[iGroup];
        for(int i = 0; i < aModels.GetSize(); i++)
        {
            A3DLitModel* pModel = aModels[i];

            int nMeshes = pModel->GetNumMeshes();
            
            if (nMeshes == 0)
                continue;
            
            aMeshes.RemoveAll(false);
            for (int iMesh = 0; iMesh < nMeshes; iMesh++)
            {
                A3DLitMesh* pLitMesh = pModel->GetMesh(iMesh);
                if (!pLitMesh->IsVisible() || !pLitMesh->IsUseLightMap() 
                    || (pModel->IsNoTransform() && !pViewport->GetCamera()->AABBInViewFrustum(pLitMesh->GetAABB())))
                    continue;
                aMeshes.Add(pLitMesh);
            }

            int nVisiableMeshes = aMeshes.GetSize();

            if (nVisiableMeshes == 0)
                continue;

            if (bLightMap)
            {
                if (pModel->GetLightMap() != NULL)
                {
                    pModel->GetLightMap()->Appear(1);
                }
                else if (pErrorLmTex)
                {
                    pErrorLmTex->Appear(1);
                }
                if (pModel->GetNightLightMap() != NULL)
                {
                    pModel->GetNightLightMap()->Appear(2);
                }
                else if (pErrorLmTex)
                {
                    pErrorLmTex->Appear(2);
                }
            }

            if (pModel->IsNoTransform())
            {
                if (!bIdentityWorldMat)
                {
                    A3DMATRIX4 matWorld = a3d_IdentityMatrix();
                    m_pA3DDevice->SetVertexShaderConstants(iWorldMatIndex, &matWorld, 4);
                    bIdentityWorldMat = true;
                }
            }
            else
            {
                A3DMATRIX4 matWorld = pModel->GetAbsoluteTM();
                matWorld.Transpose();
                m_pA3DDevice->SetVertexShaderConstants(iWorldMatIndex, &matWorld, 4);
            }

            for (int iMesh = 0; iMesh < nVisiableMeshes; iMesh++)
            {
                A3DLitMesh* pLitMesh = aMeshes[iMesh];

                A3DEffect* pEffect = (A3DEffect*)(pLitMesh->GetTexturePtr());
                if (!pEffect)
                    continue;

                if (pEffect->GetOriginTex() != NULL)
                {
                    pEffect->GetOriginTex()->Appear(0);
                }
                else if (pErrorTex)
                {
                    pErrorTex->Appear(0);
                }

                if (bUseDetail)
                {
                    if (pLitMesh->GetDetailTexture() && pLitMesh->GetDetailTexture()->GetD3DTexture())
                    {
                        pLitMesh->GetDetailTexture()->Appear(5);
                    }
                    else
                    {
                        m_pEmptyDetailMap->Appear(5);
                    }
                    A3DVECTOR4 vDetailParam(pLitMesh->GetDetailTile(), pLitMesh->GetDetailHardness(), 0, 0);
                    m_pA3DDevice->SetPixelShaderConstants(7, &vDetailParam.x, 1);
                }
                m_pA3DDevice->SetFaceCull(pLitMesh->GetMaterial().Is2Sided() ? A3DCULL_NONE : cullOld);
                pLitMesh->GetStream()->Appear(0, false);
                m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, 0, pLitMesh->GetNumVerts(), 0, pLitMesh->GetNumFaces());
            }
        }
    }
    m_pA3DDevice->SetFaceCull(cullOld);

    return true;
}


bool A3DLitModelRender::CreateDefaultShader(int nType)
{
    AString strMacro;
    if ((nType & SHADER_REALTIME) == SHADER_REALTIME)
    {
        strMacro += "_REAL_TIME_;";
    }
    else
    {
        if (nType & SHADER_LIGHTMAP)
            strMacro += "_LIGHT_MAP_;";
        if (nType & SHADER_VERTLIT)
            strMacro += "_VERT_LIT_;";
    }
    if (nType & SHADER_SHADOWMAP)
    {
        strMacro += "_SHADOW_MAP_;";
        if (A3DShadowMap::SupportHWPCF(m_pA3DDevice))
            strMacro += "_HWPCF_;";
        if (nType & SHADER_SHADOWMAP_HIGH)
            strMacro += "_SHADOW_HIGH_;";
    }

    if (nType & SHADER_DETAILMAP)
    {
        strMacro += "_DETAIL_MAP_;";
    }

    if (strMacro.GetLength() > 0)
        strMacro.CutRight(1);

    A3DEnvironment* pEnvironment = m_pA3DEngine->GetA3DEnvironment();	// 取环境对象
    A3DCCDBinder* pBinder = pEnvironment->GetCCDBinder();			// 取环境对象的绑定对象

    A3DHLSL* pVS = m_pA3DEngine->GetA3DHLSLMan()->LoadShader(
        "shaders\\2.2\\HLSL\\LitModel\\litmodel_vs_new.hlsl", NULL, strMacro);
    if (!pVS)
    {
        g_A3DErrLog.Log("A3DLitModelRender::CreateDefaultShader(), Failed to create vertex shader %s!", strMacro);
        return false;
    }
    pVS->BindCommConstData(pBinder);
    m_pVSShaders[nType] = pVS;

    A3DHLSL* pPS = m_pA3DEngine->GetA3DHLSLMan()->LoadShader(
        NULL, "shaders\\2.2\\HLSL\\LitModel\\litmodel_ps_new.hlsl", strMacro);
    if (!pPS)
    {
        g_A3DErrLog.Log("A3DLitModelRender::CreateDefaultShader(), Failed to create pixel shader %s!", strMacro);
        return false;
    }
    pPS->BindCommConstData(pBinder);
    m_pPSShaders[nType] = pPS;
    return true;
}

void A3DLitModelRender::SetDetailMap(const char* szFilename)
{
    A3DTexture* pTexture = new A3DTexture;
    pTexture->SetNoDownSample(true);
    if (!pTexture->LoadFromFile(m_pA3DDevice, szFilename, 0, 0, A3DFMT_UNKNOWN, 0)
        || !pTexture->GetD3DTexture())
    {
        A3DRELEASE(pTexture);
        return;
    }
    A3DRELEASE(m_pEmptyDetailMap);    
    m_pEmptyDetailMap = pTexture;
}