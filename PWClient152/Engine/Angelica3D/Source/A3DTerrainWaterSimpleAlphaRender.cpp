/*
 * FILE: A3DTerrainWaterSimpleAlphaRender.cpp
 *
 * DESCRIPTION: Class representing for water on the terrain
 *
 * CREATED BY: Liyi, 2009/10/16
 *
 * HISTORY:
 *
 * Copyright (c) 2009 Archosaur Studio, All Rights Reserved.	
 */

#include "A3DTerrainWaterSimpleAlphaRender.h"

#include "A3DDevice.h"
#include "A3DEngine.h"
#include "A3DShaderMan.h"
#include "A3DVertexShader.h"
#include "A3DPixelShader.h"
#include "A3DTerrainWater.h"
#include "A3DCamera.h"
#include "A3DOrthoCamera.h"
#include "A3DTexture.h"
#include "A3DRenderTarget.h"
#include "A3DViewport.h"
#include "A3DStream.h"
#include "A3DTerrain2.h"
#include "A3DTerrain2Render.h"
#include "A3DPI.h"
#include "A3DFuncs.h"
#include "A3DTextureMan.h"
#include "A3DWaterArea.h"

A3DTerrainWaterSimpleAlphaRender::A3DTerrainWaterSimpleAlphaRender()
{
	m_strName						= "A3DTerrainWaterSimpleAlphaRender";

	m_pWaterAreasHeightRT			= NULL;
	m_pWaterAreasViewport			= NULL;
	m_pWaterAreasCamera				= NULL;
	m_nWaterAreasRTWidth			= 512;
	m_nWaterAreasRTHeight			= 512;
	m_pWaterAreasRTVertexShader		= NULL;
	m_pWaterAreasRTPixelShader		= NULL;
	m_pSimpleAlphaEdgeVertexShader	= NULL;
	m_pSimpleAlphaEdgePixelShader	= NULL;
}

A3DTerrainWaterSimpleAlphaRender::~A3DTerrainWaterSimpleAlphaRender()
{
	Release();
}

bool A3DTerrainWaterSimpleAlphaRender::Init(A3DTerrainWater* pWater, A3DDevice * pA3DDevice, A3DViewport * pHostViewport, A3DCamera * pHostCamera, int nMaxVerts, int nMaxFaces, bool bStaticMesh)
{
	m_pWater		= pWater;
	m_pA3DDevice	= pA3DDevice;
	m_pHostViewport = pHostViewport;
	m_pHostCamera	= pHostCamera;

	//	Use default material
	m_Material.Init(m_pA3DDevice);
	m_Material.SetPower(50.0f);
	m_Material.SetSpecular(A3DCOLORVALUE(1.0f));

	if (!CreateStream(bStaticMesh, nMaxVerts, nMaxFaces))
	{
		g_A3DErrLog.Log("A3DTerrainWater::Init(), failed to create stream!");
		return false;
	}

	if( !m_pA3DDevice->TestPixelShaderVersion(1, 4) || m_pA3DDevice->GetD3DCaps().MaxUserClipPlanes == 0 )
		m_bCanReflect = false;
	else
		m_bCanReflect = true;
	m_bCanExpensive = m_pA3DDevice->TestPixelShaderVersion(1, 4);

	m_bSimpleWater	= true;
	m_bExpensiveWater = false;

	//load texs
	char szFile[MAX_PATH];
	int i;
	m_nNumTexWater = 30;
	m_idTexWater = 0;
	for(i=0; i<m_nNumTexWater; i++)
	{
		sprintf(szFile, "textures\\water\\l\\l%d.dds", i);
		if( !m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFile(szFile, &m_pTexWaters[i]) )
		{
			g_A3DErrLog.Log("A3DTerrainWater::LoadTextures(), fail to load texture [%s]", szFile);
			return false;
		}
	}

	m_nTextureTime = 0;

	// now initialize wave move contrl
	m_waveMoveCtrl1.SetCtrlValues(1.0f, 2.0f, 3.0f, 4.0f, -0.6f, -0.35f, 0.0f, 0.35f, 0.5f);
	m_waveMoveCtrl2.SetCtrlValues(1.0f, 2.0f, 3.0f, 4.0f, -0.6f, -0.35f, 0.0f, 0.35f, 0.5f);
	m_waveMoveCtrl3.SetCtrlValues(1.0f, 2.0f, 3.0f, 4.0f, -0.3f, -0.175f, 0.0f, 0.175f, 0.25f);

	// move wave control 2 forward to make it just half paced with wave 2
	m_waveMoveCtrl1.UpdateValue(0.0f);
	m_waveMoveCtrl2.UpdateValue(2.5f);
	m_waveMoveCtrl3.UpdateValue(0.4f);

	m_matTexWave1 = IdentityMatrix();
	m_matTexWave2 = IdentityMatrix();
	m_matTexWave2._41 = 0.5f;
	m_matTexWave3 = Scaling(0.5f, 0.5f, 0.5f);

	// bump mat parameters.
	m_matBump = Scaling(8.0f, 8.0f, 8.0f);
	m_matBump._31 = 0.05f;
	m_matBump._32 = 0.0f;

	m_matBump2 = Scaling(2.0f, 2.0f, 2.0f);
	m_matBump2._31 = 0.05f;
	m_matBump2._32 = 0.0f;

	if( !InitWaterAreasHeightRT())
		return false;

	if (!LoadWaterResources())
		return false;

	if (m_pA3DDevice)
		m_pA3DDevice->AddUnmanagedDevObject(this);

	return true;
}

bool A3DTerrainWaterSimpleAlphaRender::Release()
{
	if(!A3DTerrainWaterRender::Release())
		return false;

	ReleaseWaterAreasHeightRT();

	return true;
}

bool A3DTerrainWaterSimpleAlphaRender::RenderReflect(int nAreaArrayIdx, 
													 const A3DVECTOR3& vecSunDir, 
													 A3DViewport * pCurrentViewport, 
													 A3DTerrain2 * pTerrain, 
													 A3DSky * pSky, 
													 A3DTerrainOutline * pTerrainOutline, 
													 LPRENDERFORREFLECT pReflectCallBack, 
													 LPRENDERFORREFRACT pRefractCallBack, 
													 void * pArg, 
													 LPRENDERONSKY pRenderOnSky, 
													 void * pArgSky, 
													 bool bBlendToScreen, 
													 A3DTerrain2 * pRealTerrain)
{
	if( m_pWater->GetNumWaterAreas() == 0 )
		return true;

	int i, iNumMinorArea=0, iNumNormalArea=0;
	for( int idx = 0; idx < m_pWater->GetWaterAreasArrayNum(); idx++)
	{
		APtrArray<A3DWaterArea*>* pAreaArry = m_pWater->GetWaterAreasArray(idx);
		for(i=0; i<pAreaArry->GetSize(); i++)
		{
			A3DWaterArea* pArea = (*pAreaArry)[i];
			
			//	RenderReflect is called when camera is above water, so ignore water area that
			//	higher than camera
			if (m_pHostCamera->GetPos().y < pArea->GetWaterHeight())
				continue;
			
			//设置是否渲染标志
			const A3DVECTOR3& vCamPos = m_pHostCamera->GetPos();
			const A3DVECTOR3& vMins = pArea->m_aabbVisGrids.Mins;
			const A3DVECTOR3& vMaxs = pArea->m_aabbVisGrids.Maxs;
			float dx1 = vCamPos.x - vMins.x;
			float dx2 = vMaxs.x - vCamPos.x;
			float dz1 = vCamPos.z - vMins.z;
			float dz2 = vMaxs.z - vCamPos.z;
			float fDistToCam;

			if (dx1 < 0.0f)
			{
				if (dz1 < 0.0f)
					fDistToCam = sqrtf(dx1 * dx1 + dz1 * dz1);
				else if (dz2 < 0.0f)
					fDistToCam = sqrtf(dx1 * dx1 + dz2 * dz2);
				else
					fDistToCam = -dx1;
			}
			else if (dx2 < 0.0f)
			{
				if (dz1 < 0.0f)
					fDistToCam = sqrtf(dx2 * dx2 + dz1 * dz1);
				else if (dz2 < 0.0f)
					fDistToCam = sqrtf(dx2 * dx2 + dz2 * dz2);
				else
					fDistToCam = -dx2;
			}
			else
			{
				if (dz1 < 0.0f)
					fDistToCam = -dz1;
				else if (dz2 < 0.0f)
					fDistToCam = -dz2;
				else
					fDistToCam = 0.0f;
			}

			if (fDistToCam > m_fViewRadius)
			{
				pArea->m_bDoRender = false;
				continue;
			}

			pArea->m_bDoRender = m_pHostCamera->AABBInViewFrustum(pArea->m_aabbVisGrids);
			if (!pArea->m_bDoRender) 
				continue;
			
			if (pArea->GetMinorWaterFlag())
				iNumMinorArea++;
			else
				iNumNormalArea++;
		}	
	}
	if( iNumMinorArea == 0 && iNumNormalArea == 0)
		return true;
			
	RenderWaterAreasHeightRT();
	
	if( pRealTerrain != NULL)
	{
		m_pCameraRefract->SetPos(m_pHostCamera->GetPos());
		m_pCameraRefract->SetDirAndUp(m_pHostCamera->GetDir(), m_pHostCamera->GetUp());
		m_pCameraRefract->SetProjectionParam(m_pHostCamera->GetFOV(), -1.0f, 1024.0f, -1.0f);
		m_pCameraRefract->Active();		
		RenderSimpleAlphaEdge(m_pRefractViewport, pRealTerrain);	
	}


	for( int nIdx = 0; nIdx < m_pWater->GetWaterAreasArrayNum(); nIdx++)
	{
		RenderWaterAreas(vecSunDir, pCurrentViewport, true, nIdx);

	}
	
	return true;

}

bool A3DTerrainWaterSimpleAlphaRender::RenderWaterAreas(const A3DVECTOR3& vecSunDir, A3DViewport * pCurrentViewport, bool bReflect, int nAreaArrayIdx)
{	
	//
	pCurrentViewport->Active();

	//set render states
	A3DCULLTYPE cullType = m_pA3DDevice->GetFaceCull();
	m_pA3DDevice->SetFaceCull(A3DCULL_NONE);
	m_pA3DDevice->SetZWriteEnable(true);
	m_pA3DDevice->SetZTestEnable(true);	
	m_pA3DDevice->SetWorldMatrix(IdentityMatrix());
	m_pA3DDevice->SetLightingEnable(true);
	m_pA3DDevice->SetSpecularEnable(true);
	m_pA3DDevice->SetDeviceRenderState(D3DRS_LOCALVIEWER, TRUE);
	m_pA3DDevice->SetDeviceRenderState(D3DRS_NORMALIZENORMALS, TRUE);

	//Set Material
	m_Material.Appear();

	//set textures
	m_pTexWaters[m_idTexWater]->Appear(1);
	m_pReflectTarget->AppearAsTexture(2);

	m_pA3DDevice->SetTextureFilterType(1, A3DTEXF_LINEAR);
	m_pA3DDevice->SetTextureFilterType(2, A3DTEXF_LINEAR);

	m_pA3DDevice->SetTextureTransformFlags(1, A3DTTFF_COUNT2);
	m_pA3DDevice->SetTextureMatrix(1, m_matBump2);
	m_pA3DDevice->SetTextureCoordIndex(1, 0);
	
	A3DMATRIX4 matScale(A3DMATRIX4::IDENTITY);
	matScale._11 = 0.5f;
	matScale._22 = -0.5f;
	matScale._41 = 0.5f;
	matScale._42 = 0.5f;
	A3DMATRIX4 matProjectedView;

	//m_pCameraReflect->SetMirror(m_pHostCamera, A3DVECTOR3(0.0f, m_pWater->GetWaterAreaHeight(nAreaArrayIdx), 0.0f), A3DVECTOR3(0.0f, 1.0f, 0.0f));
	//m_pCameraReflect->Update(); 

	matProjectedView = InverseTM(m_pHostCamera->GetViewTM()) * m_pCameraRefract->GetVPTM() * matScale;
	m_pA3DDevice->SetTextureCoordIndex(2, D3DTSS_TCI_CAMERASPACEPOSITION);
	m_pA3DDevice->SetTextureTransformFlags(2, (A3DTEXTURETRANSFLAGS)(A3DTTFF_COUNT4 | A3DTTFF_PROJECTED));
	m_pA3DDevice->SetTextureMatrix(2, matProjectedView);

	m_pA3DDevice->SetTextureAddress(1, A3DTADDR_WRAP, A3DTADDR_WRAP);
	m_pA3DDevice->SetTextureAddress(2, A3DTADDR_CLAMP, A3DTADDR_CLAMP);
	
	//intel 945G do not support T&L 
	if( !m_pA3DDevice->IsHardwareTL() && m_pWaterVertexShader )
	{
		m_pWaterVertexShader->Appear();
		// set vertex shader constants here
		A3DMATRIX4 matVP = m_pA3DDevice->GetViewMatrix() * m_pA3DDevice->GetProjectionMatrix();
		matVP.Transpose();
		m_pA3DDevice->SetVertexShaderConstants(0, &matVP, 4);
		
		A3DVECTOR4 c4(m_pHostCamera->GetPos());
		m_pA3DDevice->SetVertexShaderConstants(4, &c4, 1);
		
		A3DVECTOR4 c5(1.0f, 0.0f, 0.0f, 0.0f);
		m_pA3DDevice->SetVertexShaderConstants(5, &c5, 1);
		
		for(int i=0; i<6; i++)
		{
			A3DMATRIX4 matTex = m_pA3DDevice->GetTextureMatrix(i);
			matTex.Transpose();
			m_pA3DDevice->SetVertexShaderConstants(6 + i * 4, &matTex, 4);
		}
		
		A3DMATRIX4 matView = m_pA3DDevice->GetViewMatrix();
		matView.Transpose();
		m_pA3DDevice->SetVertexShaderConstants(30, &matView, 4);

		//set stream
		m_pWaterStream->Appear(0, false);
	}
	else
	{
		m_pWaterStream->Appear(0, true);
	}
	
	//set shaders
	m_pWaterAlphaSimple->Appear();

	//set shader const
	A3DCOLORVALUE clWater = ComposeWaterColor(vecSunDir);
	m_pA3DDevice->SetPixelShaderConstants(6, &clWater, 1);

	

	//draw primitives
	DrawWaterAreas(pCurrentViewport, DRAW_ALLWATER, nAreaArrayIdx, false);

	//restore textures
	m_pTexWaters[m_idTexWater]->Disappear(1);
	m_pReflectTarget->DisappearAsTexture(2);
	m_pA3DDevice->SetTextureTransformFlags(1, A3DTTFF_DISABLE);
	m_pA3DDevice->SetTextureTransformFlags(2, A3DTTFF_DISABLE);
	m_pA3DDevice->SetTextureAddress(1, A3DTADDR_WRAP, A3DTADDR_WRAP);
	m_pA3DDevice->SetTextureAddress(2, A3DTADDR_WRAP, A3DTADDR_WRAP);
	m_pA3DDevice->SetTextureCoordIndex(1, 1);

	//restore shaders
	//
	if( !m_pA3DDevice->IsHardwareTL() && m_pWaterVertexShader )	
	{
		m_pWaterVertexShader->Disappear();
	}
	m_pWaterAlphaSimple->Disappear();

	//restore render states
	m_pA3DDevice->SetDeviceRenderState(D3DRS_NORMALIZENORMALS, FALSE);
	m_pA3DDevice->SetZWriteEnable(true);
	m_pA3DDevice->SetZTestEnable(true);	
	m_pA3DDevice->SetLightingEnable(true);
	m_pA3DDevice->SetFaceCull(cullType);

	return true;
}

bool A3DTerrainWaterSimpleAlphaRender::RenderRefract(int nAreaArrayIdx, 
													 const A3DVECTOR3& vecSunDir, 
													 A3DViewport * pCurrentViewport, 
													 A3DTerrain2 * pTerrain, 
													 A3DSky * pSky, 
													 A3DTerrainOutline * pTerrainOutline, 
													 LPRENDERFORREFRACT pRefractCallBack, 
													 void * pArg, 
													 LPRENDERONSKY pRenderOnSky, 
													 void * pArgSky, 
													 bool bBlendToScreen)
{
	return true;
}


bool A3DTerrainWaterSimpleAlphaRender::InitWaterAreasHeightRT()
{
	if( m_pA3DDevice == NULL)
		return false;
	
	//create render target
	
	A3DRenderTarget::RTFMT rtFmt;
	rtFmt.iHeight = m_nWaterAreasRTHeight;
	rtFmt.iWidth = m_nWaterAreasRTWidth;
	rtFmt.fmtTarget = A3DFMT_A8R8G8B8;
	rtFmt.fmtDepth = A3DFMT_D24X8;
	
	m_pWaterAreasHeightRT = new A3DRenderTarget;
	if( !m_pWaterAreasHeightRT->Init(m_pA3DDevice, rtFmt, true, false))
		return false;
	
	//create ortho camera
	m_pWaterAreasCamera = new A3DOrthoCamera;
	if( !m_pWaterAreasCamera->Init(m_pA3DDevice, -10.0f, 10.0f, -10.0f, 10.0f, 0.0f, 10.0f))
		return false;
	
	
	//create viewport
	A3DVIEWPORTPARAM param;
	param.X = 0;
	param.Y = 0;
	param.Width = m_nWaterAreasRTWidth;
	param.Height = m_nWaterAreasRTHeight;
	
	if(!m_pA3DDevice->CreateViewport(&m_pWaterAreasViewport, 0, 0, m_nWaterAreasRTWidth, m_nWaterAreasRTHeight, 0.0f, 1.0f, 
		true, false, 0xff000000))
		return false;
	
	m_pWaterAreasViewport->SetCamera(m_pWaterAreasCamera);
	
	//load vertex shader
	D3DVERTEXELEMENT9 vertDeclWater[] = 
	{
		{0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
		{0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		D3DDECL_END()
	};
	
	A3DShaderMan* pShaderMan = m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan();
	if( pShaderMan == NULL)
		return false;
	
	m_pWaterAreasRTVertexShader = pShaderMan->LoadVertexShader("Shaders\\2.2\\vs\\water_areas_rt_vs.txt", false, vertDeclWater);
	if( m_pWaterAreasRTVertexShader == NULL)
		return false;
	
	m_pWaterAreasRTPixelShader = pShaderMan->LoadPixelShader("Shaders\\2.2\\ps\\water_areas_rt_ps.txt", false);
	if( m_pWaterAreasRTPixelShader == NULL)
		return false;
	
	m_pSimpleAlphaEdgeVertexShader = pShaderMan->LoadVertexShader("Shaders\\2.2\\vs\\simple_alpha_edge_vs.txt", false, vertDeclWater);
	if( m_pSimpleAlphaEdgeVertexShader == NULL)
		return false;
	m_pSimpleAlphaEdgePixelShader = pShaderMan->LoadPixelShader("Shaders\\2.2\\ps\\simple_alpha_edge_ps.txt", false);
	if( m_pSimpleAlphaEdgePixelShader == NULL)
		return false;
	
	return true;
}

void A3DTerrainWaterSimpleAlphaRender::ReleaseWaterAreasHeightRT()
{
	A3DRELEASE( m_pWaterAreasHeightRT);
	A3DRELEASE( m_pWaterAreasCamera);
	A3DRELEASE( m_pWaterAreasViewport);
	
	A3DShaderMan* pShaderMan = m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan();
	if( pShaderMan != NULL)
	{
		if( m_pWaterAreasRTVertexShader != NULL)
		{
			pShaderMan->ReleaseVertexShader(&m_pWaterAreasRTVertexShader);
			m_pWaterAreasRTVertexShader = NULL;
		}
		
		if( m_pWaterAreasRTPixelShader != NULL)
		{
			pShaderMan->ReleasePixelShader(&m_pWaterAreasRTPixelShader);
			m_pWaterAreasRTPixelShader = NULL;
		}
		
		if ( m_pSimpleAlphaEdgeVertexShader != NULL)
		{
			pShaderMan->ReleaseVertexShader(&m_pSimpleAlphaEdgeVertexShader);
			m_pSimpleAlphaEdgeVertexShader = NULL;
		}
		if( m_pSimpleAlphaEdgePixelShader != NULL)
		{
			pShaderMan->ReleasePixelShader(&m_pSimpleAlphaEdgePixelShader);
			m_pSimpleAlphaEdgePixelShader = NULL;
		}
	}
	return;
	
}

bool A3DTerrainWaterSimpleAlphaRender::RenderWaterAreasHeightRT()
{
	//
	if( m_pWater->GetNumWaterAreas()  == 0)
		return false;
	if( m_pWaterAreasRTVertexShader == NULL || m_pWaterAreasRTPixelShader == NULL)
		return false;
	
	A3DAABB aabb;
	aabb.Clear();
	int nArrayNum = m_pWater->GetWaterAreasArrayNum();
	for( int i = 0; i < nArrayNum; i++)
	{
		APtrArray<A3DWaterArea*>* pArray = m_pWater->GetWaterAreasArray(i);
		for( int j = 0; j < pArray->GetSize(); j++)
		{
			A3DWaterArea* pArea = (*pArray)[j];
			
			if (!pArea->m_bDoRender)
				continue;

			A3DAABB areaAABB = pArea->GetAABB();
			aabb.AddVertex(areaAABB.Maxs);
			aabb.AddVertex(areaAABB.Mins);
		}
	}
	aabb.CompleteCenterExts();
	
	float fWidth = aabb.Maxs.x - aabb.Mins.x;
	float fHeight = aabb.Maxs.z - aabb.Mins.z;
	float fZFar = aabb.Maxs.y - aabb.Mins.y + 10;
	
	float fSize  = fWidth > fHeight ? fWidth : fHeight;
	float fRadius = fSize / 2.0f;
	a_ClampRoof(fRadius, m_fViewRadius);

	//Set Camera
	m_pWaterAreasCamera->SetProjectionParam(-fRadius, fRadius, -fRadius, fRadius,	0, fZFar);
	
	A3DVECTOR3 vCenter = m_pHostCamera->GetPos() + m_pHostCamera->GetDir() * 10.f;
	vCenter.y = aabb.Center.y;
	m_pWaterAreasCamera->SetPos(vCenter + A3DVECTOR3(0.0, fZFar / 2.0f, 0.0f));
	m_pWaterAreasCamera->SetDirAndUp(A3DVECTOR3(0.0f, -1.0f, 0.0f), A3DVECTOR3( 0.0f, 0.0f, 1.0f));
	
	//Set render target
	m_pWaterAreasHeightRT->ApplyToDevice();
	
	m_pWaterAreasViewport->Active();
	m_pWaterAreasViewport->SetClearColor(0xff000000);
	m_pWaterAreasViewport->ClearDevice();
	
	
	//set shader
	m_pWaterAreasRTVertexShader->Appear();
	m_pWaterAreasRTPixelShader->Appear();
	
	//set vertex shader constants
	A3DMATRIX4 matVP = m_pWaterAreasCamera->GetVPTM();
	matVP.Transpose();
	m_pA3DDevice->SetVertexShaderConstants(0, &matVP, 4);
	
	
	//Set render state
	m_pA3DDevice->SetZTestEnable(false);
	m_pA3DDevice->SetZWriteEnable(false);

	//set water stream
	m_pWaterStream->Appear(0, false);
	
	//Draw water areas
	for(int nIdx = 0; nIdx < nArrayNum; nIdx++)
	{
		float fHeight = m_pWater->GetWaterAreaHeight(nIdx);
		A3DVECTOR4 vHeight;
		int nHeight = int(fHeight);
		vHeight.x = nHeight / 256 / 255.0f;
		vHeight.y = (nHeight % 256) / 255.0f;
		vHeight.z = fHeight - nHeight;
		vHeight.w = 1.0f;
		
		m_pA3DDevice->SetVertexShaderConstants(4, &vHeight, 1);
		
		DrawWaterAreas(m_pWaterAreasViewport, DRAW_ALLWATER, nIdx, true);
	}
	
	//Restore render target
	m_pWaterAreasHeightRT->WithdrawFromDevice();

	if (m_bTest)
		D3DXSaveTextureToFileA("waterheight.dds", D3DXIFF_DDS, m_pWaterAreasHeightRT->GetTargetTexture(), NULL);
	
	m_pWaterAreasRTVertexShader->Disappear();
	m_pWaterAreasRTPixelShader->Disappear();
	
	//restore render state
	m_pA3DDevice->SetZTestEnable(true);
	m_pA3DDevice->SetZWriteEnable(true);
		
	return true;
	
}

bool A3DTerrainWaterSimpleAlphaRender::DrawWaterAreas(A3DViewport * pCurrentViewport, DWORD dwDrawFlags, int nAreaArrayIdx, bool bNoDivideGrid)
{
	A3DWATERVERTEX *	pVerts;
	WORD *				pIndices;
	

	if( !m_pWaterStream->LockVertexBuffer(0, 0, (BYTE**)&pVerts, 0) )
		return false;

	if( !m_pWaterStream->LockIndexBuffer(0, 0, (BYTE**)&pIndices, 0) )
		return false;

	A3DVECTOR3 vecCamPos = pCurrentViewport->GetCamera()->GetPos();

	A3DWaterArea * pArea;
	int nVertCount = 0;
	int nFaceCount = 0;
	APtrArray<A3DWaterArea*>* pWaterAreas = m_pWater->GetWaterAreasArray(nAreaArrayIdx);
	for(int i=0; i< pWaterAreas->GetSize(); i++)
	{
		pArea = (*pWaterAreas)[i];

		if (!pArea->m_bDoRender)
			continue;

		if (dwDrawFlags != DRAW_ALLWATER)
		{
			if (((dwDrawFlags & DRAW_MINORWATER) && !pArea->GetMinorWaterFlag()) ||
				((dwDrawFlags & DRAW_NON_MINORWATER) && pArea->GetMinorWaterFlag()))
				continue;
		}

		// now fill one water area's verts and faces into stream
		int nWidth = pArea->GetWidth();
		int nHeight = pArea->GetHeight();
		float vGridSize = pArea->GetGridSize();
		float vHalfGridSize = vGridSize * .5f;
		A3DCOLOR waterColor = pArea->GetWaterColor();
		BYTE * pGridFlags = pArea->GetGridFlags();

		A3DVECTOR3 vecDeltaX = A3DVECTOR3(vGridSize, 0.0f, 0.0f);
		A3DVECTOR3 vecDeltaZ = A3DVECTOR3(0.0f, 0.0f, -vGridSize);
		A3DVECTOR3 vecOrg = pArea->GetCenter() - vecDeltaX * nWidth / 2.0f - vecDeltaZ * nHeight / 2.0f;
		vecOrg.y = pArea->GetWaterHeight();

		float	ts = (float)(int)(vGridSize * 0.025f + 0.5f);
		for(int y=0; y<nHeight; y++)
		{
			const int iBase = y * nWidth;

			for(int x=0; x<nWidth; x++)
			{
				if( pGridFlags[iBase + x] & WATERGRID_VISIBLE )
				{
					A3DVECTOR3 p0 = vecOrg + vecDeltaX * x + vecDeltaZ * y;
					float u0 = x * ts;
					float v0 = y * ts;

					int ss;
					A3DVECTOR3 c = p0 + vecDeltaX * 0.5f + vecDeltaZ * 0.5f;

					float dx = float(fabs(c.x - vecCamPos.x));
					float dz = float(fabs(c.z - vecCamPos.z));

					if (dx - vHalfGridSize > m_fViewRadius || dz - vHalfGridSize > m_fViewRadius)
						continue;

					if (bNoDivideGrid)
						ss = 1;
					else
					{
						if( dx < 160.0f && dz < 160.0f )
						{
							if( dx < m_fLODDist1 && dz < m_fLODDist1 )
								ss = int(vGridSize / 2.0f);
							else if( dx < m_fLODDist2 && dz < m_fLODDist2 )
								ss = int(vGridSize / 4.0f);
							else
								ss = int(vGridSize / 8.0f);

							if( ss == 0 )
								ss = 1;
						}
						else
							ss = 1;
					}

					if( nVertCount + (ss + 1) * (ss + 1) > m_nMaxVerts ||
						nFaceCount + ss * ss * 2 > m_nMaxFaces )
					{
						// flush once;
						m_pWaterStream->UnlockVertexBuffer();
						m_pWaterStream->UnlockIndexBuffer();

						m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, nVertCount, 0, nFaceCount);

						nVertCount = 0;
						nFaceCount = 0;

						// lock again
						if( !m_pWaterStream->LockVertexBuffer(0, 0, (BYTE**)&pVerts, 0) )
							return false;

						if( !m_pWaterStream->LockIndexBuffer(0, 0, (BYTE**)&pIndices, 0) )
							return false;
					}

					float rss = 1.0f / ss;
					for(int yy=0; yy<=ss; yy++)
					{
						for(int xx=0; xx<=ss; xx++)
						{
							*pVerts = A3DWATERVERTEX(p0 + xx * rss * vecDeltaX + yy * rss * vecDeltaZ, 
								A3D::g_vAxisY, u0 + xx * rss, v0 + yy * rss);

							pVerts ++;

							if( xx != ss && yy != ss )
							{
								pIndices[0] = nVertCount + yy * (ss + 1) + xx;
								pIndices[1] = pIndices[0] + 1;
								pIndices[2] = pIndices[0] + ss + 1;

								pIndices[3] = pIndices[2];
								pIndices[4] = pIndices[1];
								pIndices[5] = pIndices[2] + 1;

								pIndices += 6;
							}
						}
					}
					
					nVertCount += (ss + 1) * (ss + 1);
					nFaceCount += ss * ss * 2;
				}
			}
		}
	}

	m_pWaterStream->UnlockVertexBuffer();
	m_pWaterStream->UnlockIndexBuffer();

	if( nVertCount > 0 && nFaceCount > 0 )
	{
		m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, nVertCount, 0, nFaceCount);
	}

	return true;

}

bool A3DTerrainWaterSimpleAlphaRender::DrawTerrain(A3DViewport* pViewport, A3DTerrain2* pTerrain)
{
	m_pEdgeTerrainStream->Appear(0, false);
	
	pTerrain->DoBlocksCull(pViewport);
	
	A3DTerrain2Render * pTerrain2Render = pTerrain->GetTerrainRender();
	int nMesh = pTerrain2Render->GetRenderMeshNum();
	for(int i=0; i<nMesh; i++) 
	{
		// now lock terrain stream before we render
		A3DVERTEX * pVerts;
		WORD * pIndices;
		
		if( !m_pEdgeTerrainStream->LockVertexBuffer(0, 0, (BYTE **)&pVerts, 0) )
			return false;
		if( !m_pEdgeTerrainStream->LockIndexBuffer(0, 0, (BYTE **)&pIndices, 0) )
		{
			m_pEdgeTerrainStream->UnlockVertexBuffer();
			return false;
		}
		
		int nVert, nIndex;
		pTerrain2Render->GetRenderMesh(i, (BYTE *)pVerts, sizeof(A3DVERTEX), pIndices, &nVert, &nIndex, A3DTRN2LOADERB::T2BKFLAG_NORENDER | A3DTRN2LOADERB::T2BKFLAG_NORENDERWITHWATER);
		
		m_pEdgeTerrainStream->UnlockVertexBuffer();
		m_pEdgeTerrainStream->UnlockIndexBuffer();
		
		if( nVert > 0 && nIndex > 0 )
			m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, nVert, 0, nIndex / 3);
	}
	
	return true;
}

bool A3DTerrainWaterSimpleAlphaRender::RenderSimpleAlphaEdge(A3DViewport* pViewport, A3DTerrain2* pTerrain)
{
	m_pReflectTarget->ApplyToDevice();
	
	m_pA3DDevice->Clear(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xff000000, 1.0f, 0);
	
	//set render state
	m_pA3DDevice->SetLightingEnable(false);
	m_pA3DDevice->SetSpecularEnable(false);
	bool bFog = m_pA3DDevice->GetFogEnable();
	m_pA3DDevice->SetFogEnable(false);
	m_pA3DDevice->SetAlphaBlendEnable(false);
	
	//set texture
	m_pWaterAreasHeightRT->AppearAsTexture(0);
	m_pA3DDevice->SetTextureAddress(0, A3DTADDR_CLAMP, A3DTADDR_CLAMP);
	m_pA3DDevice->SetTextureFilterType(0, A3DTEXF_POINT);
	
	m_pA3DDevice->SetTextureTransformFlags(0, A3DTTFF_DISABLE);
	m_pA3DDevice->SetTextureTransformFlags(1, A3DTTFF_DISABLE);
	m_pA3DDevice->SetTextureTransformFlags(2, A3DTTFF_DISABLE);
	
	
	//set vertex shader and pixel shader
	if( m_pSimpleAlphaEdgeVertexShader)
		m_pSimpleAlphaEdgeVertexShader->Appear();
	
	if( m_pSimpleAlphaEdgePixelShader )
		m_pSimpleAlphaEdgePixelShader->Appear();
	
	//set vertex shader const
	A3DCameraBase* pCamera = pViewport->GetCamera();
	A3DMATRIX4 matVP = pCamera->GetVPTM();
	matVP.Transpose();
	m_pA3DDevice->SetVertexShaderConstants(0, &matVP, 4);
	
	A3DMATRIX4 matRT = m_pWaterAreasCamera->GetVPTM();
	A3DMATRIX4 matScale(A3DMATRIX4::IDENTITY);
	matScale.m[0][0] = 0.5f;
	matScale.m[1][1] = -0.5f;
	matScale.m[3][0] = 0.5f;
	matScale.m[3][1] = 0.5f;
	matRT *= matScale;
	

	matRT.Transpose();
	m_pA3DDevice->SetVertexShaderConstants( 4, &matRT, 4);	
	
	A3DVECTOR4 c8(256.0 * 255.0f, 255.0f, 1.0f, 0.0f);
	m_pA3DDevice->SetVertexShaderConstants(8, &c8, 1);	
	
	//set pixel shader const
	A3DVECTOR4 c9(0.0f);
	c9.z = 1.0f/ m_vCurEdgeHeight;
	m_pA3DDevice->SetVertexShaderConstants(9, &c9, 1);

	pViewport->Active();
	m_pA3DDevice->Clear(D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
	
	//draw terrain
	DrawTerrain(pViewport, pTerrain);
	
	//restore render state
	m_pA3DDevice->SetLightingEnable(true);
	m_pA3DDevice->SetSpecularEnable(true);
	m_pA3DDevice->SetFogEnable(bFog);
	m_pA3DDevice->SetAlphaBlendEnable(true);
	
	//restore texture
	m_pWaterAreasHeightRT->DisappearAsTexture(0);
	m_pA3DDevice->SetTextureFilterType(0, A3DTEXF_LINEAR);
	m_pA3DDevice->SetTextureAddress(0, A3DTADDR_WRAP, A3DTADDR_WRAP);
	
	//restore shaders
	m_pSimpleAlphaEdgeVertexShader->Disappear();
	m_pSimpleAlphaEdgePixelShader->Disappear();
	
	m_pReflectTarget->WithdrawFromDevice();
	
	if( m_bTest)
	{
		D3DXSaveTextureToFileA("Reflect.dds", D3DXIFF_DDS, m_pReflectTarget->GetTargetTexture(), NULL);
		m_bTest = false;
	}
	
	return true;
}

bool A3DTerrainWaterSimpleAlphaRender::SetSimpleWaterFlag(bool bFlag)
{
	return true;
}

bool A3DTerrainWaterSimpleAlphaRender::Update(int nDeltaTime)
{
	m_nTextureTime += nDeltaTime;
	while( m_nTextureTime >= 33 )
	{
		m_idTexWater ++;
		m_idTexWater %= m_nNumTexWater;
		m_nTextureTime -= 33;
	}
	return true;
}