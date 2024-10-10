/*
* FILE: A3DTerrainWaterSimpleRender.cpp
*
* DESCRIPTION: Class representing for water on the terrain
*
* CREATED BY: Liyi, 2012, 3, 5
*
* HISTORY:
*
* Copyright (c) 2009 Archosaur Studio, All Rights Reserved.	
*/

#include "A3DTerrainWater2SimpleRender.h"

#include "A3DDevice.h"
#include "A3DEngine.h"
#include "A3DFuncs.h"
#include "A3DPI.h"
#include "A3DTextureMan.h"
#include "A3DVertexShader.h"
#include "A3DViewport.h"
#include "A3DCameraBase.h"
#include "A3DEnvironment.h"
#include "A3DHLSL.h"

//water2
#include "A3DTerrainWater2.h"
#include "A3DWaterArea2.h"

A3DTerrainWater2SimpleRender::A3DTerrainWater2SimpleRender()
:m_pA3DDevice(NULL),
m_pWater(NULL),
m_nNumTexWater(30),
m_idTexWater(0),
m_nTextureTime(0),
m_WaterAniType(WTAT_ANIMATED),
m_pAboveWaterHLSL(NULL),
m_pUnderWaterHLSL(NULL),
m_pSimpleWaterHLSL(NULL)
{
	m_matTexTrans = Scaling(2.0f, 2.0f, 2.0f);
	m_matTexTrans._31 = 0.05f;
	m_matTexTrans._32 = 0.0f;

	memset(m_ppWaterTex, 0, sizeof(A3DTexture*) * MAX_WATER_TEX);
}

A3DTerrainWater2SimpleRender::~A3DTerrainWater2SimpleRender()
{
	Release();
}

bool A3DTerrainWater2SimpleRender::Init(A3DTerrainWater2* pWater)
{
	if (pWater == NULL)
		return false;

	m_pWater = pWater;
	m_pA3DDevice = pWater->GetA3DDevice();

	if( !LoadTextures())
		return false;

	//	Load shaders
	LoadShaders();

	return true;
}


bool A3DTerrainWater2SimpleRender::LoadTextures()
{
	ReleaseTextures();

	char szFile[MAX_PATH];
	for(int i=0; i<m_nNumTexWater; i++)
	{
		sprintf(szFile, "Shaders\\textures\\water\\l\\l%d.dds", i);
		if( !m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFile(szFile, &m_ppWaterTex[i]) )
		{
			g_A3DErrLog.Log("A3DTerrainWaterSimpleRender::LoadTextures(), fail to load texture [%s]", szFile);
			return false;
		}
	}

	return true;
}

void A3DTerrainWater2SimpleRender::ReleaseTextures()
{
	//Release Textures
	for(int i = 0; i < m_nNumTexWater; i++)
	{
		if( m_ppWaterTex[i] )
		{
			m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_ppWaterTex[i]);
			m_ppWaterTex[i] = NULL;
		}
	}
}

bool A3DTerrainWater2SimpleRender::LoadShaders()
{
	A3DHLSLMan* pHLSLMan = m_pA3DDevice->GetA3DEngine()->GetA3DHLSLMan();

	if( !pHLSLMan)
		return false;
	
	m_pAboveWaterHLSL = pHLSLMan->LoadShader("shaders\\2.2\\HLSL\\Water\\SimpleWater.hlsl", 
											"vs_Main", 
											"shaders\\2.2\\HLSL\\Water\\SimpleWater.hlsl",
											"ps_EyeAboveWater",
											"_FOG_", 
											0);
	
	m_pUnderWaterHLSL = pHLSLMan->LoadShader("shaders\\2.2\\HLSL\\Water\\SimpleWater.hlsl",
											"vs_Main",
											"shaders\\2.2\\HLSL\\Water\\SimpleWater.hlsl",
											 "ps_EyeUnderWater",
											"_FOG_",
											0);
	if(!m_pAboveWaterHLSL || !m_pUnderWaterHLSL)
	{
		g_A3DErrLog.Log("A3DTerrainWater2SimpleRender::LoadShaders(), Failed to load shaders!");
		return false;
	}

	
	A3DCCDBinder* pBinder = m_pA3DDevice->GetA3DEngine()->GetA3DEnvironment()->GetCCDBinder();
	m_pAboveWaterHLSL->BindCommConstData(pBinder);
	m_pUnderWaterHLSL->BindCommConstData(pBinder);
	
	return true;
}
void A3DTerrainWater2SimpleRender::ReleaseShaders()
{
	A3DHLSLMan* pHLSLMan = m_pA3DDevice->GetA3DEngine()->GetA3DHLSLMan();
	if( pHLSLMan)
	{
		if( m_pAboveWaterHLSL)
		{
			pHLSLMan->ReleaseShader(m_pAboveWaterHLSL);
			m_pAboveWaterHLSL = NULL;
		}

		if( m_pUnderWaterHLSL)
		{
			pHLSLMan->ReleaseShader(m_pUnderWaterHLSL);
			m_pUnderWaterHLSL = NULL;
		}

	}
}

void A3DTerrainWater2SimpleRender::Release()
{
	ReleaseShaders();

	ReleaseTextures();
}

bool A3DTerrainWater2SimpleRender::Update(int nDeltaTime)
{
	//Update matTexTrans offset
	float fRad = DEG2RAD(nDeltaTime / 40.0f);
	if( m_WaterAniType == WTAT_ANIMATED)
	{
		A3DVECTOR3 vOffset = A3DVECTOR3(m_matTexTrans._31, m_matTexTrans._32, 0.0f);
		vOffset = vOffset * RotateZ(fRad);
		m_matTexTrans._31 = vOffset.x;
		m_matTexTrans._32 = vOffset.y;
	}

	//Update tex id
	m_nTextureTime += nDeltaTime;
	while( m_nTextureTime >= 33 )
	{
		m_idTexWater ++;
		m_idTexWater %= m_nNumTexWater;
		m_nTextureTime -= 33;
	}

	return true;
}

bool A3DTerrainWater2SimpleRender::Render(const RenderParam* prp)
{
	//	Active Viewport
	prp->pCurViewport->Active();

	const A3DVECTOR3& vCameraPos = prp->pCurViewport->GetCamera()->GetPos();
	bool bUnderWater = m_pWater->IsUnderWater(vCameraPos);

	//	Active Vertex Decl
	m_pWater->GetWaterAreaVertDecl()->Appear();

	//select shader
	m_pSimpleWaterHLSL = m_pAboveWaterHLSL;

	//	Set proper fog if eye is in water
	
	if (bUnderWater)
	{
		//select shader
		m_pSimpleWaterHLSL = m_pUnderWaterHLSL;		
	}

	//render state
	bool bOriginalZEnable = m_pA3DDevice->GetZWriteEnable();
	bool bOriginalAlphaBlendEnable = m_pA3DDevice->GetAlphaBlendEnable();

	m_pA3DDevice->SetZWriteEnable(true);
	m_pA3DDevice->SetAlphaBlendEnable(true);
	m_pA3DDevice->SetFaceCull(A3DCULL_NONE);

	//set shader parameters
	A3DEnvironment* pEnv = m_pA3DDevice->GetA3DEngine()->GetA3DEnvironment();
	A3DCameraBase* pCamera = prp->pCurViewport->GetCamera();
	A3DMATRIX4 matWVP = A3DMATRIX4(A3DMATRIX4::IDENTITY) * pCamera->GetVPTM();
	m_pSimpleWaterHLSL->SetConstantMatrix("g_matWVP", matWVP);

	const A3DEnvironment::DIRLIGHT* dirLight = pEnv->GetDirLightParam();
	A3DVECTOR3 vPos = pCamera->GetPos() - dirLight->Direction * 10000.0f;
	A3DVECTOR4 vMainLightPos = A3DVECTOR4(vPos.x, vPos.y, vPos.z, 1.0f);
	m_pSimpleWaterHLSL->SetValue4f("g_MainLightPos", &vMainLightPos);

	//set main light color
	A3DCOLORVALUE clrMainLight = dirLight->Diffuse;
	A3DVECTOR4 vMainLightColor(clrMainLight.r, clrMainLight.g, clrMainLight.b, clrMainLight.a);
	m_pSimpleWaterHLSL->SetValue4f("g_MainLightColor", &vMainLightColor);

	m_pSimpleWaterHLSL->SetValue3f("g_vCameraPos", &vCameraPos);

	
	//	Static mesh flag
	m_pSimpleWaterHLSL->SetValue1f("g_bStaticMesh", m_pWater->IsStaticMesh()); 

	//	Textures
	m_pSimpleWaterHLSL->SetTexture("g_BaseSampler", (A3DObject*)m_ppWaterTex[m_idTexWater]);
	//Set Sampler state
	m_pA3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pA3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pA3DDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	m_pA3DDevice->SetTextureAddress(0, A3DTADDR_WRAP, A3DTADDR_WRAP);

	m_pSimpleWaterHLSL->SetConstantMatrix("g_matTexTrans", m_matTexTrans);

	//	Set shader
	m_pSimpleWaterHLSL->Appear(m_pA3DDevice->GetA3DEngine()->GetA3DEnvironment()->GetCommonConstTable());

	//render
	int iAreaNum = m_pWater->GetWaterAreasArrayNum();
	for (int iArea=0; iArea < iAreaNum; iArea++)
	{
        if (m_pWater->CullWaterAreasByViewport(prp->pCurViewport, iArea))
        {
		    if (m_pWater->IsStaticMesh())
			    m_pWater->DrawWaterAreaArrayStatic(prp->pCurViewport, A3DTerrainWater2::DRAW_ALLWATER, iArea, m_pSimpleWaterHLSL, false);
		    else
			    m_pWater->DrawWaterAreaArray(prp->pCurViewport, A3DTerrainWater2::DRAW_ALLWATER, iArea);
        }
	}

	//restore shaders
	m_pSimpleWaterHLSL->Disappear();

	//restore render state
	m_pA3DDevice->SetZWriteEnable(bOriginalZEnable);
	m_pA3DDevice->SetAlphaBlendEnable(bOriginalAlphaBlendEnable);
	m_pA3DDevice->SetFaceCull(A3DCULL_CCW);

	return true;
}

void A3DTerrainWater2SimpleRender::UploadParamsForWaterArea(A3DWaterArea2* pWaterArea)
{
	//	Water color
	A3DCOLORVALUE colWater(pWaterArea->GetCurWaterColor());
	A3DVECTOR4 vColWater(colWater.r, colWater.g, colWater.b, colWater.a);
	m_pSimpleWaterHLSL->SetValue4f("g_vWaterColor",&vColWater);
	m_pSimpleWaterHLSL->Commit();
}


