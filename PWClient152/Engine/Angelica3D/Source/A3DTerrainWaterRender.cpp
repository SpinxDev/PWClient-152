/*
 * FILE: A3DTerrainWaterRender.cpp
 *
 * DESCRIPTION: Class representing for water on the terrain
 *
 * CREATED BY:  Liyi, 2009/8/4
 *
 * HISTORY:
 *
 * Copyright (c) 2009 Archosaur Studio, All Rights Reserved.	
 */

#include "A3DTypes.h"
#include "A3DCamera.h"
#include "A3DOrthoCamera.h"
#include "A3DCubeTexture.h"
#include "A3DDevice.h"
#include "A3DEngine.h"
#include "A3DStream.h"
#include "A3DLight.h"
#include "A3DTextureMan.h"
#include "A3DTexture.h"
#include "A3DRenderTarget.h"
#include "A3DViewport.h"
#include "A3DPixelShader.h"
#include "A3DVertexShader.h"
#include "A3DShaderMan.h"
#include "A3DSky.h"
#include "A3DTerrainOutline.h"
#include "A3DTerrain2.h"
#include "A3DFuncs.h"
#include "A3DVertex.h"
#include "A3DPI.h"

#include "APerlinNoise1D.h"

#include "A3DTerrainWaterRender.h"
#include "A3DTerrainWater.h"
#include "A3DWaterArea.h"

#define MAX_WAVE_DIS	300.0f

A3DTerrainWaterRender::A3DTerrainWaterRender()
{
	m_strName			= "A3DTerrainWaterRender";
	m_pWater			= NULL;
	m_pHostViewport		= NULL;
	m_pHostCamera		= NULL;
	m_pCameraReflect	= NULL;
	m_pCameraRefract	= NULL;
	m_pLightSun			= NULL;

	m_pCurTerrain		= NULL;
	m_pTrnRender		= NULL;

	memset(m_pTexWaters, 0, sizeof(A3DTexture *) * MAX_WATER_TEX);
	memset(m_pTexNormals, 0, sizeof(A3DTexture *) * MAX_WATER_TEX);
	memset(m_pTexCausts, 0, sizeof(A3DTexture *) * MAX_WATER_TEX);
	m_nNumTexWater		= 0;
	m_nNumTexNormal		= 0;
	m_nNumTexCaust		= 0;

	m_pReflectTarget	= NULL;
	m_pRefractTarget	= NULL;
	m_pReflectViewport	= NULL;
	m_pRefractViewport	= NULL;
	m_pNormalizeCubeMap	= NULL;

	m_pEdgePixelShader	= NULL;
	m_pEdgeVertexShader	= NULL;
	m_pEdgeTerrainStream= NULL;

	m_pTexWaterReflect	= NULL;
	m_fGndUVScale		= 1.0f;
	m_pGndBaseTex		= NULL;
	
	m_pWaterStream		= NULL;
	m_nUpdateHeightTime	= 0;
	m_vCurWaterHeight	= 0.0f;

	m_pTexWave			= NULL;
	m_pWaveStream		= NULL;

	m_pWaterCheapShader = NULL;
	m_pWaterCheapScrollShader = NULL;
	m_pWaterExpensiveShader = NULL;
	m_pWaterExpScrollShader = NULL;
	m_pWaterRefractShader = NULL;
	m_pWaterRefractScrollShader = NULL;
	m_pWaterVertexShader = NULL;
	m_pWaterStaticVertexShader = NULL;
	m_pGndReflectShader	= NULL;
	m_pWaterAlphaSimple = NULL;

	m_pWaveShader		= NULL;
	m_pWaveVertexShader	= NULL;
	m_nTextureTime		= 0;

	m_vCaustDU			= 0.0f;
	m_vCaustDV			= 0.0f;

	m_fogAir			= 0xffffffff;
	m_fogAirStart		= 100.0f;
	m_fogAirEnd			= 400.0f;
	m_fogWater			= 0xff0000ff;
	m_fogWaterStart		= 10.0f;
	m_fogWaterEnd		= 60.0f;

	m_bRenderWaves		= true;
	m_byAlphaWater		= 0xA0;
	m_colorWater		= (((DWORD)m_byAlphaWater) << 24) | 0x00254F55;

	m_vCurEdgeHeight	= 0.66f;

	m_fDNFactor			= 0.0f;

	m_type				= WATER_TYPE_ANIMATED;

	m_bCanReflect		= false;
	m_bCanExpensive		= false;
	m_bSimpleWater		= true;
	m_bExpensiveWater	= false;

	m_bTest				= false;
	m_fViewRadius		= 999999.f;
	m_fLODDist1			= 64.f;
	m_fLODDist2			= 120.f;

	m_bStaticMesh		= true;

}

A3DTerrainWaterRender::~A3DTerrainWaterRender()
{
	Release();
}

bool A3DTerrainWaterRender::Init(A3DTerrainWater* pWater, A3DDevice * pA3DDevice, A3DViewport * pHostViewport, A3DCamera * pHostCamera, int nMaxVerts, int nMaxFaces, bool bStaticMesh)
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

	if( !LoadTextures() )
	{
		g_A3DErrLog.Log("A3DTerrainWater::Init(), failed to load textures needed!");
		return false;
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

	//InitWaterAreasHeightRT();

	if (m_pA3DDevice)
		m_pA3DDevice->AddUnmanagedDevObject(this);

	return true;
}

bool A3DTerrainWaterRender::Release()
{
	if (m_pA3DDevice)
		m_pA3DDevice->RemoveUnmanagedDevObject(this);

	m_Material.Release();
	ReleaseTextures();
	ReleaseStream();
	ReleaseWaterResources();

	A3DRELEASE(m_pTrnRender);

	//ReleaseWaterAreasHeightRT();	

	//	Release ground texture
	if (m_pGndBaseTex)
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pGndBaseTex);
		m_pGndBaseTex = NULL;
	}

	//	Release ground shader
	if (m_pGndReflectShader)
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->ReleasePixelShader(&m_pGndReflectShader);
		m_pGndReflectShader = NULL;
	}

	return true;
}

//	Change host camera and viewport. Pass NULL means don't change
void A3DTerrainWaterRender::ChangeHostCameraAndView(A3DViewport* pViewport, A3DCamera* pCamera)
{
	if (pViewport)
		m_pHostViewport = pViewport;

	if (pCamera)
		m_pHostCamera = pCamera;
}

bool A3DTerrainWaterRender::LoadWaterResources()
{
	if( !m_pA3DDevice->TestPixelShaderVersion(1, 1) ||
		m_pA3DDevice->GetD3DCaps().MaxUserClipPlanes == 0 )
		return false;

	A3DVIEWPORTPARAM * pViewParam = m_pHostViewport->GetParam();

	// first create the render target for the water
	A3DRenderTarget::RTFMT rtFmt;

 //	rtFmt.nWidth		= pViewParam->Width + 100;
 //	rtFmt.nHeight		= pViewParam->Height + 100;
	rtFmt.iWidth		= pViewParam->Width ;
 	rtFmt.iHeight		= pViewParam->Height;

	rtFmt.fmtTarget		= A3DFMT_A8R8G8B8; // we need an alpha channel on refract target to hold water height and terrain height difference
	rtFmt.fmtDepth		= A3DFMT_D24X8;

	m_pReflectTarget = new A3DRenderTarget();
	if( !m_pReflectTarget->Init(m_pA3DDevice, rtFmt, true, true) )
		return false;

	if( !m_bSimpleWater && m_bExpensiveWater )
	{
		m_pRefractTarget = new A3DRenderTarget();
		if( !m_pRefractTarget->Init(m_pA3DDevice, rtFmt, true, true) )
			return false;	
	}

	float fov = m_pHostCamera->GetFOV() + DEG2RAD(5.0f);
	//float fov = m_pHostCamera->GetFOV() ;
	float ratio = rtFmt.iWidth * 1.0f / rtFmt.iHeight;

	// now create a reflected camera
	m_pCameraReflect = new A3DCamera();
	if( !m_pCameraReflect->Init(m_pA3DDevice, fov, 0.2f, 1900.0f, ratio) )
	{
		g_A3DErrLog.Log("A3DTerrainWater::Init(), failed to create the reflected camera!");
		return false;
	}
	m_pCameraReflect->SetMirror(m_pHostCamera, A3DVECTOR3(0.0f, 0.0f, 0.0f), A3DVECTOR3(0.0f, 1.0f, 0.0f));

	m_pCameraRefract = new A3DCamera();
	if( !m_pCameraRefract->Init(m_pA3DDevice, fov, 0.2f, 1900.0f, ratio) )// m_pHostCamera->GetFOV(), 1.0f, 1000.0f, 1.33333f) )
	{
		g_A3DErrLog.Log("A3DTerrainWater::Init(), failed to create the refracted camera!");
		return false;
	}

	//	Create viewports
	if( !m_pA3DDevice->CreateViewport(&m_pReflectViewport, 0, 0, rtFmt.iWidth, rtFmt.iHeight,
									0.0f, 1.0f, true, true, 0xFF008FFD) )
	{
		g_A3DErrLog.Log("A3DTerrainWater::Create reflect viewport fail");
		return false;
	}
	m_pReflectViewport->SetCamera(m_pCameraReflect);

	if( !m_pA3DDevice->CreateViewport(&m_pRefractViewport, 0, 0, rtFmt.iWidth, rtFmt.iHeight,
									0.0f, 1.0f, true, true, 0xFF000000) )
	{
		g_A3DErrLog.Log("A3DTerrainWater::Create refract viewport fail");
		return false;
	}
	m_pRefractViewport->SetCamera(m_pCameraRefract);

	// now create a specular cube map
	m_pNormalizeCubeMap = new A3DCubeTexture();
	if( !m_pNormalizeCubeMap->CreateNormalizationCubeMap(m_pA3DDevice, 256, IdentityMatrix()) )
	{
		g_A3DErrLog.Log("A3DTerrainWater::Create(), failed to create normalization cube map!");
		return false;
	}
	// terrain things when do water edge detect
	m_pEdgeTerrainStream = new A3DStream();
	if( !m_pEdgeTerrainStream->Init(m_pA3DDevice, A3DVT_VERTEX, 16 * 33 * 33, 16 * 32 * 32 * 6, A3DSTRM_REFERENCEPTR, A3DSTRM_REFERENCEPTR) )
	{
		g_A3DErrLog.Log("A3DTerrainWater::LoadWaterResources(), failed to create edge terrain stream!");
		return false;
	}

	m_pEdgePixelShader = m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->LoadPixelShader("Shaders\\2.2\\ps\\water_edge_ps.txt", false);
	if( NULL == m_pEdgePixelShader )
	{
		g_A3DErrLog.Log("A3DTerrainWater::Init(), failed to load water_edge_ps.txt");
		return false;
	}

	D3DVERTEXELEMENT9 vertDeclWater[] = 
	{
		{0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
		{0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		D3DDECL_END()
	};

	m_pEdgeVertexShader = m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->LoadVertexShader("Shaders\\2.2\\vs\\water_edge_vs.txt", false, vertDeclWater);
	if( NULL == m_pEdgeVertexShader )
	{
		g_A3DErrLog.Log("A3DTerrainWater::Init(), failed to load water_edge_vs.txt");
		return false;
	}

	// now load pixel shader
	m_pWaterCheapShader = m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->LoadPixelShader("Shaders\\2.2\\ps\\water_cheap.txt", false);
	if( NULL == m_pWaterCheapShader )
	{
		g_A3DErrLog.Log("A3DTerrainWater::Init(), failed to load water_cheap.txt");
		return false;
	}

	m_pWaterCheapScrollShader = m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->LoadPixelShader("Shaders\\2.2\\ps\\water_cheap_scroll.txt", false);
	if( NULL == m_pWaterCheapScrollShader )
	{
		g_A3DErrLog.Log("A3DTerrainWater::Init(), failed to load water_cheap.txt");
		return false;
	}

	m_pWaterExpensiveShader = m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->LoadPixelShader("Shaders\\2.2\\ps\\water_expensive.txt", false);
	if( NULL == m_pWaterExpensiveShader )
	{
		g_A3DErrLog.Log("A3DTerrainWater::Init(), failed to load water_expensive.txt");
		return false;
	}

	m_pWaterExpScrollShader = m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->LoadPixelShader("Shaders\\2.2\\ps\\water_expensive_scroll.txt", false);
	if( NULL == m_pWaterExpScrollShader )
	{
		g_A3DErrLog.Log("A3DTerrainWater::Init(), failed to load water_expensive.txt");
		return false;
	}

	m_pWaterRefractShader = m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->LoadPixelShader("Shaders\\2.2\\ps\\waterrefract.txt", false);
	if( NULL == m_pWaterRefractShader )
	{
		g_A3DErrLog.Log("A3DTerrainWater::Init(), failed to load waterrefract.txt");
		return false;
	}

	m_pWaterRefractScrollShader = m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->LoadPixelShader("Shaders\\2.2\\ps\\waterrefract_scroll.txt", false);
	if( NULL == m_pWaterRefractShader )
	{
		g_A3DErrLog.Log("A3DTerrainWater::Init(), failed to load waterrefract.txt");
		return false;
	}

	m_pWaterAlphaSimple = m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->LoadPixelShader("Shaders\\2.2\\ps\\water_alpha_simple.txt", false);
	if( NULL == m_pWaterAlphaSimple )
	{
		g_A3DErrLog.Log("A3DTerrainWater::SetWaterType, failed to load water_alpha_simple.txt");
	//	return false;
	}

	m_pWaterVertexShader = m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->LoadVertexShader("Shaders\\2.2\\vs\\water_vs.txt", false, vertDeclWater);
	if( NULL == m_pWaterVertexShader )
	{
		g_A3DErrLog.Log("A3DTerrainWater::Init(), failed to load water_vs.txt");
		return false;
	}

	m_pWaveShader = m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->LoadPixelShader("Shaders\\2.2\\ps\\waveblend.txt", false);
	if( NULL == m_pWaveShader )
	{
		g_A3DErrLog.Log("A3DTerrainWater::Init(), failed to load waveblend.txt");
		return false;
	}
	
	D3DVERTEXELEMENT9 vertDeclWaves[] = 
	{
		{0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},
		{0, 16, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		D3DDECL_END()
	};
	m_pWaveVertexShader = m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->LoadVertexShader("Shaders\\2.2\\vs\\wavevs.txt", false, vertDeclWaves);
	if( NULL == m_pWaveVertexShader )
	{
		g_A3DErrLog.Log("A3DTerrainWater::Init(), failed to load wavevs.txt");
		return false;
	}
	return true;
}

bool A3DTerrainWaterRender::ReleaseWaterResources()
{
	if( m_pWaveVertexShader )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->ReleaseVertexShader(&m_pWaveVertexShader);
		m_pWaveVertexShader = NULL;
	}

	if( m_pWaveShader )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->ReleasePixelShader(&m_pWaveShader);
		m_pWaveShader = NULL;
	}

	if( m_pWaterVertexShader )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->ReleaseVertexShader(&m_pWaterVertexShader);
		m_pWaterVertexShader = NULL;
	}
	if( m_pWaterCheapShader )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->ReleasePixelShader(&m_pWaterCheapShader);
		m_pWaterCheapShader = NULL;
	}

	if (m_pWaterAlphaSimple)
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->ReleasePixelShader(&m_pWaterAlphaSimple);
		m_pWaterAlphaSimple = NULL;
	}

	if( m_pWaterCheapScrollShader )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->ReleasePixelShader(&m_pWaterCheapScrollShader);
		m_pWaterCheapScrollShader = NULL;
	}

	if( m_pWaterExpensiveShader )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->ReleasePixelShader(&m_pWaterExpensiveShader);
		m_pWaterExpensiveShader = NULL;
	}

	if( m_pWaterExpScrollShader )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->ReleasePixelShader(&m_pWaterExpScrollShader);
		m_pWaterExpScrollShader = NULL;
	}

	if( m_pWaterRefractShader )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->ReleasePixelShader(&m_pWaterRefractShader);
		m_pWaterRefractShader = NULL;
	}

	if( m_pWaterRefractScrollShader )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->ReleasePixelShader(&m_pWaterRefractScrollShader);
		m_pWaterRefractScrollShader = NULL;
	}

	if( m_pNormalizeCubeMap )
	{
		m_pNormalizeCubeMap->Release();
		delete m_pNormalizeCubeMap;
		m_pNormalizeCubeMap = NULL;
	}

	if( m_pReflectViewport )
	{
		m_pReflectViewport->Release();
		delete m_pReflectViewport;
		m_pReflectViewport = NULL;
	}

	if( m_pRefractViewport )
	{
		m_pRefractViewport->Release();
		delete m_pRefractViewport;
		m_pRefractViewport = NULL;
	}

	if( m_pCameraRefract )
	{
		m_pCameraRefract->Release();
		delete m_pCameraRefract;
		m_pCameraRefract = NULL;
	}

	if( m_pCameraReflect )
	{
		m_pCameraReflect->Release();
		delete m_pCameraReflect;
		m_pCameraReflect = NULL;
	}

	if( m_pRefractTarget )
	{
		m_pRefractTarget->Release();
		delete m_pRefractTarget;
		m_pRefractTarget = NULL;
	}

	if( m_pReflectTarget )
	{
		m_pReflectTarget->Release();
		delete m_pReflectTarget;
		m_pReflectTarget = NULL;
	}

	if( m_pEdgePixelShader )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->ReleasePixelShader(&m_pEdgePixelShader);
		m_pEdgePixelShader = NULL;
	}

	if( m_pEdgeVertexShader )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->ReleaseVertexShader(&m_pEdgeVertexShader);
		m_pEdgeVertexShader = NULL;
	}

	if( m_pEdgeTerrainStream )
	{
		m_pEdgeTerrainStream->Release();
		delete m_pEdgeTerrainStream;
		m_pEdgeTerrainStream = NULL;
	}

	return true;
}

bool A3DTerrainWaterRender::LoadTextures()
{
	// try to release old textures.
	ReleaseTextures();

	if( !m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFile("textures\\water\\waterreflect.bmp", &m_pTexWaterReflect) )
	{
		g_A3DErrLog.Log("A3DTerrainWater::LoadTextures(), fail to load reflect simple texture");
		return false;
	}

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

	m_nNumTexCaust = 32;
	m_idTexCaust = 0;
	for(i=0; i<m_nNumTexCaust; i++)
	{
		sprintf(szFile, "textures\\water\\caust\\caust%02d.dds", i);
		if( !m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFile(szFile, &m_pTexCausts[i]) )
		{
			g_A3DErrLog.Log("A3DTerrainWater::LoadTextures(), fail to load texture [%s]", szFile);
			return false;
		}		
	}

	if( !m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFile("textures\\water\\wave1.dds", &m_pTexWave) )
	{
		g_A3DErrLog.Log("A3DTerrainWater::LoadTextures(), fail to load texture [%s]", "textures\\water\\wave1.dds");
		return false;
	}

	m_nNumTexNormal = 29;
	m_idTexNormal = 0;

	return true;
}

bool A3DTerrainWaterRender::ReleaseTextures()
{
	if( m_pTexWave )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pTexWave);
		m_pTexWave = NULL;
	}

	int i;
	for(i=0; i<m_nNumTexWater; i++)
	{
		if( m_pTexWaters[i] )
		{
			m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pTexWaters[i]);
			m_pTexWaters[i] = NULL;
		}
	}


	for(i=0; i<m_nNumTexNormal; i++)
	{
		if( m_pTexNormals[i] )
		{
			m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pTexNormals[i]);
			m_pTexNormals[i] = NULL;
		}
	}

	for(i=0; i<m_nNumTexCaust; i++)
	{
		if( m_pTexCausts[i] )
		{
			m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pTexCausts[i]);
			m_pTexCausts[i] = NULL;
		}
	}

	if( m_pTexWaterReflect )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pTexWaterReflect);
		m_pTexWaterReflect = NULL;
	}

	return true;
}

bool A3DTerrainWaterRender::CreateStream(bool bStaticMesh, int nMaxVerts, int nMaxFaces)
{
	ASSERT(!m_pWaterStream && !m_pWaterStream);

	m_nMaxWaveVerts = nMaxVerts * 2;
	m_nMaxWaveFaces = nMaxFaces * 2;

	//	Static mesh can be supported ?
	if (m_pA3DDevice->TestVertexShaderVersion(1, 1))
	{
		D3DVERTEXELEMENT9 vertDecl[] = 
		{
			{0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
			{0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
			{0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
			D3DDECL_END()
		};

		m_pWaterStaticVertexShader = m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->LoadVertexShader("Shaders\\2.2\\vs\\water_static_mesh_vs.txt", false, vertDecl);
		if( NULL == m_pWaterStaticVertexShader )
		{
			g_A3DErrLog.Log("A3DTerrainWater::CreateStream, failed to load water_static_mesh_vs.txt");
			bStaticMesh = false;	//	force to use non-static version
		}
	}
	else
		bStaticMesh = false;

	if (bStaticMesh)
	{
		int iMeshGrid = A3DWaterMeshBlock::GetHighestMeshGridDim();
		m_nMaxVerts = (iMeshGrid + 1) * (iMeshGrid + 1);
		m_nMaxFaces = 0;
	}
	else
	{
		m_nMaxVerts	= nMaxVerts;
		m_nMaxFaces	= nMaxFaces;
	}
	
	m_bStaticMesh = bStaticMesh;

	// first create stream and buffers for water surface
	if (!CreateWaterStream())
	{
		g_A3DErrLog.Log("A3DTerrainWater::CreateStream(), fail to create stream!");
		return false;
	}

	// now create vertex buffers for waves
	m_pWaveStream = new A3DStream();
	if( !m_pWaveStream->Init(m_pA3DDevice, sizeof(A3DWAVEVERTEX), A3DWAVEVERT_FVF, m_nMaxWaveVerts, m_nMaxWaveFaces * 3, A3DSTRM_REFERENCEPTR, A3DSTRM_REFERENCEPTR) )
	{
		g_A3DErrLog.Log("A3DTerrainWater::CreateStream(), fail to create wave stream!");
		return false;
	}

	return true;
}

bool A3DTerrainWaterRender::CreateWaterStream()
{
	//	Create stream
	DWORD dwStrmFlag = m_bStaticMesh ? A3DSTRM_STATIC : A3DSTRM_REFERENCEPTR;
	m_pWaterStream = new A3DStream;
	if (!m_pWaterStream || !m_pWaterStream->Init(m_pA3DDevice, sizeof (A3DWATERVERTEX), A3DWATERVERT_FVF,
		m_nMaxVerts, m_nMaxFaces * 3, dwStrmFlag,  dwStrmFlag))
	{
		g_A3DErrLog.Log("A3DTerrainWater::CreateWaterStream(), Failed to create water stream!");
		return false;
	}
	
	//	Initialize static stream's content
	if (m_bStaticMesh)
	{
		A3DWATERVERTEX*	pVerts = NULL;
		if (!m_pWaterStream->LockVertexBuffer(0, 0, (BYTE**)&pVerts, 0))
			return false;
		
		//	Ensure unit water grid's border is 1 meter
		int iMeshGrid = A3DWaterMeshBlock::GetHighestMeshGridDim();
		float fMeshGridSize = 1.0f / (iMeshGrid / A3DWaterMeshBlock::BLOCK_GRID_DIM);
		
		int r, c;
		A3DVECTOR3 v(0.0f);
		A3DWATERVERTEX* pv = pVerts;
		
		for (r=0; r <= iMeshGrid; r++)
		{
			v.z = 0.0f - fMeshGridSize * r;
			v.x = 0.0f;
			
			for (c=0; c <= iMeshGrid; c++)
			{
				*pv++ = A3DWATERVERTEX(v, A3DVECTOR3(0.0f, 1.0f, 0.0f), v.x, -v.z);
				v.x += fMeshGridSize;
			}
		}
		
		m_pWaterStream->UnlockVertexBuffer();
		
	}
	
	return true;
}

bool A3DTerrainWaterRender::ReleaseStream()
{
	if (m_pWaterStaticVertexShader)
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->ReleaseVertexShader(&m_pWaterStaticVertexShader);
		m_pWaterStaticVertexShader = NULL;
	}

	if( m_pWaveStream )
	{
		m_pWaveStream->Release();
		delete m_pWaveStream;
		m_pWaveStream = NULL;
	}

	if( m_pWaterStream )
	{
		m_pWaterStream->Release();
		delete m_pWaterStream;
		m_pWaterStream = NULL;
	}

	return true;
}

bool A3DTerrainWaterRender::UpdateWaterHeight()
{
	A3DWaterArea * pArea = m_pWater->GetNearestWaterArea(m_pA3DDevice->GetA3DEngine()->GetActiveCamera()->GetPos(), false);
	if( pArea )
		m_vCurWaterHeight = pArea->GetWaterHeight();
	
	return true;
}

bool A3DTerrainWaterRender::Update(int nDeltaTime)
{
	float vRad = DEG2RAD(nDeltaTime / 40.0f);

	if (m_type == WATER_TYPE_ANIMATED)
	{
		A3DVECTOR3 vecOffset = A3DVECTOR3(m_matBump2._31, m_matBump2._32, 0.0f);
		vecOffset = vecOffset * RotateZ(vRad);
		m_matBump2._31 = vecOffset.x;
		m_matBump2._32 = vecOffset.y;
	}
	else if (m_type == WATER_TYPE_SCROLL)
	{
		m_matBump._31 += 0.001f * nDeltaTime * 0.05f;
		m_matBump._32 += 0.001f * nDeltaTime * 0.05f;
		if( m_matBump._31 > 1.0f ) m_matBump._31 -= 1.0f;
		if( m_matBump._32 > 1.0f ) m_matBump._32 -= 1.0f;
	
		m_matBump2._31 += -0.001f * nDeltaTime * 0.05f;
		m_matBump2._32 += 0.001f * nDeltaTime * 0.05f;
		if( m_matBump2._31 < -1.0f ) m_matBump2._31 += 1.0f;
		if( m_matBump2._32 > 1.0f ) m_matBump2._32 -= 1.0f;
	}

	// now move the caustic texture
	m_vCaustDU += 0.03f * vRad;
	m_vCaustDV += 0.03f * vRad;
	//if( m_vCaustDU > 1.0f ) m_vCaustDU -= 1.0f;
	//if( m_vCaustDV > 1.0f ) m_vCaustDV -= 1.0f;

	m_nTextureTime += nDeltaTime;
	while( m_nTextureTime >= 33 )
	{
		m_idTexWater ++;
		m_idTexNormal ++;
		m_idTexCaust ++;

		m_idTexWater %= m_nNumTexWater;
		m_idTexNormal %= m_nNumTexNormal;
		m_idTexCaust %= m_nNumTexCaust;

		m_nTextureTime -= 33;
	}

	// update current water height
	m_nUpdateHeightTime -= nDeltaTime;
	if( m_nUpdateHeightTime <= 0 )
	{
		m_nUpdateHeightTime = 1000;
		UpdateWaterHeight();
	}

	/////////////////////////////////////////
	// update wave offset value
	/////////////////////////////////////////
	m_matTexWave1._42 = -m_waveMoveCtrl1.UpdateValue(nDeltaTime / 1000.0f) * 0.5f;
	m_matTexWave2._42 = -m_waveMoveCtrl2.UpdateValue(nDeltaTime / 1000.0f) * 0.5f;
	m_matTexWave3._42 = -m_waveMoveCtrl3.UpdateValue(nDeltaTime / 1000.0f) * 0.5f;
	return true;
}

bool A3DTerrainWaterRender::ApplyWaveVSConstants(A3DViewport * pViewport, bool bReflect,int nAreaArrayIdx)
{
	A3DVECTOR4	c0(1.0f, 0.0f, 0.0f, 0.0f);
	c0.w = 1.0f / (MAX_WAVE_DIS * MAX_WAVE_DIS);
	m_pA3DDevice->SetVertexShaderConstants(0, &c0, 1);

	A3DMATRIX4 mat;
	// now set vertex shader's constants.
	mat = pViewport->GetCamera()->GetVPTM();
	mat.Transpose();
	m_pA3DDevice->SetVertexShaderConstants(1, &mat, 4);

	mat = m_matTexWave1;
	mat.Transpose();
	m_pA3DDevice->SetVertexShaderConstants(5, &mat, 4);

	mat = m_matTexWave2;
	mat.Transpose();
	m_pA3DDevice->SetVertexShaderConstants(9, &mat, 4);

	mat = m_matTexWave3;
	mat.Transpose();
	m_pA3DDevice->SetVertexShaderConstants(13, &mat, 4);

	A3DMATRIX4 matWorld = IdentityMatrix();
	if( !bReflect )
	{
		matWorld._22 = -1.0f;
		matWorld._42 = 2.0f * m_pWater->GetWaterAreaHeight(nAreaArrayIdx);
	}
	matWorld.Transpose();
	m_pA3DDevice->SetVertexShaderConstants(17, &matWorld, 4);

	A3DVECTOR4 vecCamPos = A3DVECTOR4(pViewport->GetCamera()->GetPos());
	m_pA3DDevice->SetVertexShaderConstants(21, &vecCamPos, 1);
	return true;
}

bool A3DTerrainWaterRender::BlendToScreenPS(const A3DVECTOR3& vecSunDir, A3DViewport * pCurrentViewport, bool bReflect, int nAreaArrayIdx)
{
	A3DCULLTYPE cullType = m_pA3DDevice->GetFaceCull();
	m_pA3DDevice->SetFaceCull(A3DCULL_NONE);

	A3DPixelShader * pWaterShader = NULL;
	A3DCamera * pCamera;
	if( bReflect )
	{
		if (m_type == WATER_TYPE_GROUND)
			pWaterShader = m_pGndReflectShader;
		else if (m_bExpensiveWater)
		{
			// we set a water min alpha for expensive water
			float dh = pCurrentViewport->GetCamera()->GetPos().y - m_pWater->GetWaterAreaHeight(nAreaArrayIdx);
			A3DCOLORVALUE c0;
			c0.a = dh * 0.23f / m_fogWaterEnd;
			a_Clamp(c0.a, 0.11f, 0.23f);
			m_pA3DDevice->SetPixelShaderConstants(0, &c0, 1);

			if (m_type == WATER_TYPE_ANIMATED)
				pWaterShader = m_pWaterExpensiveShader;
			else if (m_type == WATER_TYPE_SCROLL)
				pWaterShader = m_pWaterExpScrollShader;
		}
		else if (m_type == WATER_TYPE_ANIMATED)
			pWaterShader = m_pWaterCheapShader;
		else if (m_type == WATER_TYPE_SCROLL)
			pWaterShader = m_pWaterCheapScrollShader;
		else if (m_type == WATER_TYPE_ALPHA_SIMPLE) //最后一步，渲染水的时候
		{
			//	Compose water color and push it to pixel shader
			A3DCOLORVALUE clWater = ComposeWaterColor(vecSunDir);
			m_pA3DDevice->SetPixelShaderConstants(6, &clWater, 1);

			pWaterShader = m_pWaterAlphaSimple;
		}

		pCamera = m_pCameraReflect;
		m_pA3DDevice->SetZWriteEnable(true);
	}
	else
	{
		if( m_type == WATER_TYPE_ANIMATED )
			pWaterShader = m_pWaterRefractShader;
		else	// if (m_type == WATER_TYPE_SCROLL)
			pWaterShader = m_pWaterRefractScrollShader;

		pCamera = m_pCameraRefract;
		m_pA3DDevice->SetZWriteEnable(true);
	}

	if( !bReflect )
	{
		m_pA3DDevice->SetFogStart(m_fogWaterStart);
		m_pA3DDevice->SetFogEnd(m_fogWaterEnd);
		m_pA3DDevice->SetFogColor(m_fogWater);
	}

	// now render the reflected water surface
	if( m_pNormalizeCubeMap )
		m_pNormalizeCubeMap->Appear(0);

	if( m_type == WATER_TYPE_ANIMATED )
		m_pTexNormals[m_idTexNormal]->Appear(1);
	else if (m_type == WATER_TYPE_SCROLL)
	{
		m_pTexNormals[0]->Appear(1);
		m_pTexNormals[0]->Appear(3);
	}
	else if (m_type == WATER_TYPE_ALPHA_SIMPLE) //最后一步，渲染水的时候
		m_pTexWaters[m_idTexWater]->Appear(1);
	else if (m_type == WATER_TYPE_GROUND)
	{
		if (m_pGndBaseTex)
			m_pGndBaseTex->Appear(1);
	}

	if( m_bSimpleWater )
		m_pTexWaterReflect->Appear(2);
	else
		m_pReflectTarget->AppearAsTexture(2);
		//m_pA3DDevice->GetD3DDevice()->SetTexture(2, m_pReflectTarget->GetTargetTexture());

	m_pWaterStream->Appear(0);

	m_pA3DDevice->SetWorldMatrix(IdentityMatrix());
	m_pA3DDevice->SetLightingEnable(true);
	m_pA3DDevice->SetSpecularEnable(true);

	m_Material.Appear();
	
	m_pA3DDevice->SetTextureFilterType(1, A3DTEXF_LINEAR);
	m_pA3DDevice->SetTextureFilterType(2, A3DTEXF_LINEAR);
	m_pA3DDevice->SetTextureFilterType(3, A3DTEXF_LINEAR);
	m_pA3DDevice->SetTextureFilterType(4, A3DTEXF_LINEAR);
	m_pA3DDevice->SetTextureFilterType(5, A3DTEXF_LINEAR);
	
	pWaterShader->Appear();

	m_pA3DDevice->SetTextureCoordIndex(0, 0);
	m_pA3DDevice->SetTextureCoordIndex(1, 0);
	m_pA3DDevice->SetTextureCoordIndex(2, 0);
	m_pA3DDevice->SetTextureCoordIndex(3, 0);
	m_pA3DDevice->SetTextureCoordIndex(4, 0);
	
	m_pA3DDevice->SetTextureTransformFlags(0, A3DTTFF_COUNT2);
	m_pA3DDevice->SetTextureMatrix(0, m_matBump);
	
	if (m_type == WATER_TYPE_GROUND)
	{
		A3DMATRIX4 mat(A3DMATRIX4::IDENTITY);
		m_pA3DDevice->SetTextureTransformFlags(1, A3DTTFF_COUNT2);
		mat._11 = m_fGndUVScale;
		mat._22 = m_fGndUVScale;
		m_pA3DDevice->SetTextureMatrix(1, mat);
	}
	else
	{
		m_pA3DDevice->SetTextureTransformFlags(1, A3DTTFF_COUNT2);
		m_pA3DDevice->SetTextureMatrix(1, m_matBump2);
	}

	A3DMATRIX4 matScale(A3DMATRIX4::IDENTITY);
	matScale._11 = 0.5f;
	matScale._22 = -0.5f;
	matScale._41 = 0.5f;
	matScale._42 = 0.5f;
	A3DMATRIX4 matProjectedView;
	matProjectedView = InverseTM(m_pHostCamera->GetViewTM()) * pCamera->GetVPTM() * matScale;
	m_pA3DDevice->SetTextureCoordIndex(2, D3DTSS_TCI_CAMERASPACEPOSITION);
	m_pA3DDevice->SetTextureTransformFlags(2, (A3DTEXTURETRANSFLAGS)(A3DTTFF_COUNT4 | A3DTTFF_PROJECTED));
	m_pA3DDevice->SetTextureMatrix(2, matProjectedView);
	
	matScale = InverseTM(m_pHostCamera->GetViewTM());
	matScale._41 = 0.0f;
	matScale._42 = 0.0f;
	matScale._43 = 0.0f;
	matScale._14 = 0.0f;
	matScale._24 = 0.0f;
	matScale._34 = 0.0f;
	m_pA3DDevice->SetTextureCoordIndex(3, D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR);
	m_pA3DDevice->SetTextureTransformFlags(3, A3DTTFF_COUNT3);
	m_pA3DDevice->SetTextureMatrix(3, matScale);

	m_pA3DDevice->SetTextureAddress(0, A3DTADDR_WRAP, A3DTADDR_WRAP);
	m_pA3DDevice->SetTextureAddress(1, A3DTADDR_WRAP, A3DTADDR_WRAP);
	m_pA3DDevice->SetTextureAddress(2, A3DTADDR_CLAMP, A3DTADDR_CLAMP);
	m_pA3DDevice->SetTextureAddress(3, A3DTADDR_WRAP, A3DTADDR_WRAP);
	m_pA3DDevice->SetTextureAddress(4, A3DTADDR_CLAMP, A3DTADDR_CLAMP);
	m_pA3DDevice->SetTextureAddress(5, A3DTADDR_CLAMP, A3DTADDR_CLAMP);
	
	// set up bump map parameters;
	float S = 0.01f;
	if( !bReflect )
		S = 0.005f;
	
	m_pA3DDevice->SetDeviceRenderState(D3DRS_LOCALVIEWER, TRUE);
	m_pA3DDevice->SetDeviceRenderState(D3DRS_NORMALIZENORMALS, TRUE);
	
	if( bReflect && m_type != WATER_TYPE_GROUND)
	{
		if( m_bExpensiveWater )
		{
			// do reflect and refract together, or just use the alpha channel of refract target which contains edge smooth value
			m_pRefractTarget->AppearAsTexture(4);
			m_pRefractTarget->AppearAsTexture(5);
			//m_pA3DDevice->GetD3DDevice()->SetTexture(4, m_pRefractTarget->GetTargetTexture());
			//m_pA3DDevice->GetD3DDevice()->SetTexture(5, m_pRefractTarget->GetTargetTexture());
		}
		else
		{
			m_pReflectTarget->AppearAsTexture(5);
			//m_pA3DDevice->GetD3DDevice()->SetTexture(5, m_pReflectTarget->GetTargetTexture());
		}
	}

	if(m_bStaticMesh)
		DrawWaterAreaArrayStatic(pCurrentViewport, DRAW_NON_MINORWATER, nAreaArrayIdx);
	else
		DrawAllWaterAreas(pCurrentViewport, DRAW_NON_MINORWATER, nAreaArrayIdx);

	m_pA3DDevice->SetDeviceRenderState(D3DRS_NORMALIZENORMALS, FALSE);
		
	m_pA3DDevice->SetTextureTransformFlags(0, A3DTTFF_DISABLE);
	m_pA3DDevice->SetTextureTransformFlags(1, A3DTTFF_DISABLE);
	m_pA3DDevice->SetTextureTransformFlags(2, A3DTTFF_DISABLE);
	m_pA3DDevice->SetTextureTransformFlags(3, A3DTTFF_DISABLE);
	m_pA3DDevice->SetTextureTransformFlags(4, A3DTTFF_DISABLE);
	
	pWaterShader->Disappear();
	
	// restore all texture coord index
	m_pA3DDevice->SetTextureCoordIndex(0, 0);
	m_pA3DDevice->SetTextureCoordIndex(1, 1);
	m_pA3DDevice->SetTextureCoordIndex(2, 2);
	m_pA3DDevice->SetTextureCoordIndex(3, 3);
	m_pA3DDevice->SetTextureCoordIndex(4, 4);
	
	m_pA3DDevice->SetTextureAddress(0, A3DTADDR_WRAP, A3DTADDR_WRAP);
	m_pA3DDevice->SetTextureAddress(1, A3DTADDR_WRAP, A3DTADDR_WRAP);
	m_pA3DDevice->SetTextureAddress(2, A3DTADDR_WRAP, A3DTADDR_WRAP);
	m_pA3DDevice->SetTextureAddress(3, A3DTADDR_WRAP, A3DTADDR_WRAP);
	m_pA3DDevice->SetTextureAddress(4, A3DTADDR_WRAP, A3DTADDR_WRAP);
	m_pA3DDevice->SetTextureAddress(5, A3DTADDR_WRAP, A3DTADDR_WRAP);

	m_pA3DDevice->ClearTexture(2);
	m_pA3DDevice->ClearTexture(4);
	m_pA3DDevice->ClearTexture(5);
	//m_pA3DDevice->GetD3DDevice()->SetTexture(2, NULL);
	//m_pA3DDevice->GetD3DDevice()->SetTexture(4, NULL);
	//m_pA3DDevice->GetD3DDevice()->SetTexture(5, NULL);
	
	if( m_type == WATER_TYPE_ANIMATED )
		m_pTexNormals[m_idTexNormal]->Disappear(1);
	else if (m_type == WATER_TYPE_SCROLL)
	{
		m_pTexNormals[0]->Disappear(1);
		m_pTexNormals[0]->Disappear(3);
	}
	else if (m_type == WATER_TYPE_GROUND)
	{
		if (m_pGndBaseTex)
			m_pGndBaseTex->Disappear(1);
	}

	if( m_pNormalizeCubeMap )
		m_pNormalizeCubeMap->Disappear(0);

	/////////////////////////////////////
	// Now we should render the waves;
	/////////////////////////////////////
	if( m_bRenderWaves && m_type != WATER_TYPE_GROUND)
	{
		m_pTexWave->Appear(0);
		m_pTexWave->Appear(1);
		m_pTexWave->Appear(2);

		m_pA3DDevice->SetTextureAddress(0, A3DTADDR_WRAP, A3DTADDR_CLAMP);
		m_pA3DDevice->SetTextureAddress(1, A3DTADDR_WRAP, A3DTADDR_CLAMP);
		m_pA3DDevice->SetTextureAddress(2, A3DTADDR_WRAP, A3DTADDR_CLAMP);
		
		m_pA3DDevice->SetZWriteEnable(false);
		// stream must appear before a vertex shader appear, or the vertex shader will not take effect
		m_pWaveStream->Appear(0);
		m_pWaveShader->Appear();
		m_pWaveVertexShader->Appear();
		ApplyWaveVSConstants(pCurrentViewport, bReflect, nAreaArrayIdx);

		// now render all waves;
		A3DWaterArea * pArea;
		int nWaveVertCount = 0;
		int nWaveFaceCount = 0;
		A3DWAVEVERTEX * pWaves = NULL;
		WORD * pWaveIndices = NULL;

		if( !m_pWaveStream->LockVertexBuffer(0, 0, (BYTE **)&pWaves, 0) )
			return false;
		if( !m_pWaveStream->LockIndexBuffer(0, 0, (BYTE **)&pWaveIndices, 0) )
		{ 
			m_pWaveStream->UnlockVertexBuffer();
			return false;
		}

		APtrArray<A3DWaterArea*>* pAreaArry = m_pWater->GetWaterAreasArray(nAreaArrayIdx);
		for(int i=0; i<pAreaArry->GetSize(); i++)
		{
			pArea = (*pAreaArry)[i];

			//	see if this is minor water or a too large water area
			if( pArea->GetMinorWaterFlag() ||
				pArea->GetWaveVertCount() > m_nMaxWaveVerts ||
				pArea->GetWaveFaceCount() > m_nMaxWaveFaces )
				continue;

			A3DVECTOR3 vecCamPos = pCamera->GetPos();
			const A3DAABB& aabb = pArea->GetAABB();
			if( !aabb.IsPointIn(pCamera->GetPos()) )
			{
				if( (fabs(vecCamPos.x - aabb.Mins.x) > MAX_WAVE_DIS && fabs(vecCamPos.x - aabb.Maxs.x) > MAX_WAVE_DIS) ||
					(fabs(vecCamPos.z - aabb.Mins.z) > MAX_WAVE_DIS && fabs(vecCamPos.z - aabb.Maxs.z) > MAX_WAVE_DIS) )
					continue;
			}

	CHECKAGAIN:
			int nNewVertCount = nWaveVertCount + pArea->GetWaveVertCount();
			int nNewFaceCount = nWaveFaceCount + pArea->GetWaveFaceCount();

			if( nNewVertCount > m_nMaxWaveVerts || nNewFaceCount > m_nMaxWaveFaces )
			{
				// flush it once here
				m_pWaveStream->UnlockVertexBuffer();
				m_pWaveStream->UnlockIndexBuffer();
				m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, nWaveVertCount, 0, nWaveFaceCount);

				nWaveVertCount = 0;
				nWaveFaceCount = 0;
				if( !m_pWaveStream->LockVertexBuffer(0, 0, (BYTE **)&pWaves, 0) )
					return false;
				if( !m_pWaveStream->LockIndexBuffer(0, 0, (BYTE **)&pWaveIndices, 0) )
				{ 
					m_pWaveStream->UnlockVertexBuffer();
					return false;
				}

				goto CHECKAGAIN;
			}

			pArea->CopyWaveToBuffer(pWaves + nWaveVertCount, pWaveIndices + nWaveFaceCount * 3, nWaveVertCount);

			nWaveVertCount = nNewVertCount;
			nWaveFaceCount = nNewFaceCount;
		}

		m_pWaveStream->UnlockVertexBuffer();
		m_pWaveStream->UnlockIndexBuffer();
		if( nWaveVertCount > 0 && nWaveFaceCount > 0 )
		{
			m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, nWaveVertCount, 0, nWaveFaceCount);
		}

		m_pWaveShader->Disappear();
		m_pWaveVertexShader->Disappear();
		
		m_pA3DDevice->SetTextureAddress(0, A3DTADDR_WRAP, A3DTADDR_WRAP);
		m_pA3DDevice->SetTextureAddress(1, A3DTADDR_WRAP, A3DTADDR_WRAP);
		m_pA3DDevice->SetTextureAddress(2, A3DTADDR_WRAP, A3DTADDR_WRAP);
		
		m_pTexWave->Disappear(0);
		m_pTexWave->Disappear(1);
		m_pTexWave->Disappear(2);
	}
		
	if( !bReflect )
	{
		m_pA3DDevice->SetFogStart(m_fogAirStart);
		m_pA3DDevice->SetFogEnd(m_fogAirEnd);
		m_pA3DDevice->SetFogColor(m_fogAir);
	}

	m_pA3DDevice->SetZWriteEnable(true);
	m_pA3DDevice->SetLightingEnable(true);
	m_pA3DDevice->SetFaceCull(cullType);
	
	return true;
}

bool A3DTerrainWaterRender::BlendToScreenNOPS(const A3DVECTOR3& vecSunDir, A3DViewport * pCurrentViewport, bool bReflect, DWORD dwDrawFlags, int nAreaArrayIdx)
{
	A3DCULLTYPE cullType = m_pA3DDevice->GetFaceCull();
	m_pA3DDevice->SetFaceCull(A3DCULL_NONE);

	m_pA3DDevice->SetWorldMatrix(IdentityMatrix());
	m_pA3DDevice->SetSpecularEnable(false);

	m_Material.Appear();

	if( !bReflect )
	{
		m_pA3DDevice->SetFogStart(m_fogWaterStart);
		m_pA3DDevice->SetFogEnd(m_fogWaterEnd);
		m_pA3DDevice->SetFogColor(m_fogWater);
	}

	if (m_type != WATER_TYPE_GROUND)
	{
		m_pTexWaters[m_idTexWater]->Appear(0);
		m_pTexWaters[m_idTexWater]->Appear(1);
		m_pWaterStream->Appear(0);

		m_pA3DDevice->SetTextureCoordIndex(0, 0);
		m_pA3DDevice->SetTextureCoordIndex(1, 0);
		m_pA3DDevice->SetTextureTransformFlags(0, A3DTTFF_COUNT2);
		m_pA3DDevice->SetTextureMatrix(0, m_matBump * Scaling(0.25f, 0.25f, 0.25f));
		m_pA3DDevice->SetTextureTransformFlags(1, A3DTTFF_COUNT2);
		m_pA3DDevice->SetTextureMatrix(1, m_matBump * Scaling(0.25f, 0.25f, 0.25f));

		//	Compose water color
		A3DCOLORVALUE clWater = ComposeWaterColor(vecSunDir);

		m_pA3DDevice->SetTextureFactor(clWater.ToRGBAColor());
		m_pA3DDevice->SetTextureColorOP(0, A3DTOP_BLENDTEXTUREALPHA);
		m_pA3DDevice->SetTextureColorArgs(0, A3DTA_TEXTURE, A3DTA_TFACTOR);

		if( !m_pA3DDevice->TestPixelShaderVersion(1, 0) )
		{
			m_pA3DDevice->SetTextureAlphaOP(0, A3DTOP_ADD);
			m_pA3DDevice->SetTextureAlphaArgs(0, A3DTA_TEXTURE, A3DTA_TFACTOR);
		}
		else
		{
			m_pA3DDevice->SetTextureAlphaOP(0, A3DTOP_ADD);
			m_pA3DDevice->SetTextureAlphaArgs(0, A3DTA_TEXTURE, A3DTA_TEXTURE);
			m_pA3DDevice->SetTextureColorOP(1, A3DTOP_MODULATEALPHA_ADDCOLOR);
			m_pA3DDevice->SetTextureColorArgs(1, A3DTA_CURRENT, A3DTA_SPECULAR);
			m_pA3DDevice->SetTextureAlphaOP(1, A3DTOP_ADD);
			m_pA3DDevice->SetTextureAlphaArgs(1, A3DTA_TEXTURE, A3DTA_TFACTOR);
		}
		
		m_pA3DDevice->SetSpecularEnable(false);

		if(m_bStaticMesh)
			DrawWaterAreaArrayStatic(pCurrentViewport, dwDrawFlags, nAreaArrayIdx);
		else
			DrawAllWaterAreas(pCurrentViewport, dwDrawFlags, nAreaArrayIdx);

		m_pA3DDevice->SetSpecularEnable(true);

		m_pA3DDevice->SetTextureTransformFlags(0, A3DTTFF_DISABLE);
		m_pA3DDevice->SetTextureCoordIndex(0, 0);
		m_pA3DDevice->SetTextureCoordIndex(1, 1);
		m_pTexWaters[m_idTexWater]->Disappear(0);
		m_pTexWaters[m_idTexWater]->Disappear(1);
		
		m_pA3DDevice->SetTextureColorOP(0, A3DTOP_MODULATE);
		m_pA3DDevice->SetTextureColorArgs(0, A3DTA_TEXTURE, A3DTA_CURRENT);
		m_pA3DDevice->SetTextureAlphaOP(0, A3DTOP_MODULATE);
		m_pA3DDevice->SetTextureAlphaArgs(0, A3DTA_TEXTURE, A3DTA_CURRENT);
		m_pA3DDevice->SetTextureColorOP(1, A3DTOP_DISABLE);
		m_pA3DDevice->SetTextureColorArgs(1, A3DTA_TEXTURE, A3DTA_CURRENT);
		m_pA3DDevice->SetTextureAlphaOP(1, A3DTOP_DISABLE);
		m_pA3DDevice->SetTextureAlphaArgs(1, A3DTA_TEXTURE, A3DTA_CURRENT);
		m_pA3DDevice->SetTextureTransformFlags(0, A3DTTFF_DISABLE);
		m_pA3DDevice->SetTextureAddress(0, A3DTADDR_WRAP, A3DTADDR_WRAP);
		m_pA3DDevice->SetTextureTransformFlags(1, A3DTTFF_DISABLE);
		m_pA3DDevice->SetTextureAddress(1, A3DTADDR_WRAP, A3DTADDR_WRAP);
		m_pA3DDevice->SetTextureCoordIndex(0, 0);
	}
	else
	{
		if (m_pGndBaseTex)
			m_pGndBaseTex->Appear(0);

		m_pWaterStream->Appear(0);

		m_pA3DDevice->SetTextureTransformFlags(0, A3DTTFF_COUNT2);
		A3DMATRIX4 mat(A3DMATRIX4::IDENTITY);
		mat._11 = m_fGndUVScale;
		mat._22 = m_fGndUVScale;
		m_pA3DDevice->SetTextureMatrix(0, mat);

		m_pA3DDevice->SetTextureColorOP(0, A3DTOP_SELECTARG1);//A3DTOP_MODULATE);
		m_pA3DDevice->SetTextureColorArgs(0, A3DTA_TEXTURE, A3DTA_CURRENT);
		m_pA3DDevice->SetTextureAlphaOP(0, A3DTOP_SELECTARG2);
		m_pA3DDevice->SetTextureAlphaArgs(0, A3DTA_TEXTURE, A3DTA_CURRENT);
		m_pA3DDevice->SetTextureColorOP(1, A3DTOP_DISABLE);
		m_pA3DDevice->SetTextureAlphaOP(1, A3DTOP_DISABLE);

		if(m_bStaticMesh)
			DrawWaterAreaArrayStatic(pCurrentViewport, dwDrawFlags, nAreaArrayIdx);
		else
			DrawAllWaterAreas(pCurrentViewport, dwDrawFlags, nAreaArrayIdx);

		m_pA3DDevice->SetTextureTransformFlags(0, A3DTTFF_DISABLE);
		m_pA3DDevice->SetTextureAlphaOP(0, A3DTOP_MODULATE);
		m_pA3DDevice->SetTextureAlphaArgs(0, A3DTA_TEXTURE, A3DTA_CURRENT);

		if (m_pGndBaseTex)
			m_pGndBaseTex->Disappear(0);
	}

	if( !bReflect )
	{
		m_pA3DDevice->SetFogStart(m_fogAirStart);
		m_pA3DDevice->SetFogEnd(m_fogAirEnd);
		m_pA3DDevice->SetFogColor(m_fogAir);
	}

	m_pA3DDevice->SetZWriteEnable(true);
	m_pA3DDevice->SetFaceCull(cullType);
	return true;
}

bool A3DTerrainWaterRender::BlendToScreen(const A3DVECTOR3& vecSunDir, A3DViewport * pViewport, bool bReflect, int nAreaArrayIdx, int iNumMinorArea, int iNumNormalArea)
{
	if (!m_bSimpleWater)
	{
		if (iNumNormalArea > 0)
		{
			if (!BlendToScreenPS(vecSunDir, pViewport, bReflect, nAreaArrayIdx))
				return false;
		}

		if (iNumMinorArea > 0)
		{
			if (!BlendToScreenNOPS(vecSunDir, pViewport, bReflect, DRAW_MINORWATER, nAreaArrayIdx))
				return false;
		}
	}
	else
		return BlendToScreenNOPS(vecSunDir, pViewport, bReflect, DRAW_ALLWATER, nAreaArrayIdx);

	return true;
}

A3DCOLORVALUE A3DTerrainWaterRender::ComposeWaterColor(const A3DVECTOR3& vecSunDir)
{
	A3DCOLORVALUE clWater = a3d_ColorRGBAToColorValue((((DWORD)m_byAlphaWater) << 24) | (m_colorWater & 0xffffff));

	if (m_pLightSun)
	{
		const A3DLIGHTPARAM& param = m_pLightSun->GetLightparam();
		A3DCOLORVALUE clLight = m_pA3DDevice->GetAmbientValue() - vecSunDir.y * param.Diffuse;
		clLight.a = 1.0f;
		clWater = clLight * clWater;
	}

	return clWater;
}

bool A3DTerrainWaterRender::DrawAllWaterAreas(A3DViewport * pCurrentViewport, DWORD dwDrawFlags, int nAreaArrayIdx)
{
	A3DWATERVERTEX *	pVerts;
	WORD *				pIndices;

	m_pWaterStream->Appear(0, false);

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
	}

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
		A3DCOLOR waterColor = pArea->GetWaterColor();
		BYTE * pGridFlags = pArea->GetGridFlags();

		A3DVECTOR3 vecDeltaX = A3DVECTOR3(pArea->GetGridSize(), 0.0f, 0.0f);
		A3DVECTOR3 vecDeltaZ = A3DVECTOR3(0.0f, 0.0f, -pArea->GetGridSize());
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
					if( dx < 160.0f && dz < 160.0f )
					{
						if( dx < 64.0f && dz < 64.0f )
							ss = int(vGridSize / 2.0f);
						else if( dx < 120.0f && dz < 120.0f )
							ss = int(vGridSize / 4.0f);
						else
							ss = int(vGridSize / 8.0f);

						if( ss == 0 )
							ss = 1;
					}
					else
						ss = 1;

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


	if( !m_pA3DDevice->IsHardwareTL() && m_pWaterVertexShader )
	{
		m_pWaterVertexShader->Disappear();
	}
	return true;
}

bool A3DTerrainWaterRender::DrawWaterAreaArrayStatic(A3DViewport* pViewport, DWORD dwDrawFlags, int nArrayIdx)
{
	if (!m_pWater->CullWaterAreasByViewport(pViewport, nArrayIdx))
		return true;
	
	m_pWaterStream->Appear(0, false);

	//Set Vertex shader
	if( m_pWaterStaticVertexShader )
	{
		m_pWaterStaticVertexShader->Appear();
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
	}
	
	const A3DVECTOR3& vecCamPos = pViewport->GetCamera()->GetPos();
	
	A3DWaterArea* pArea;
	APtrArray<A3DWaterArea*>* pWaterAreas = m_pWater->GetWaterAreasArray(nArrayIdx);
	for (int i=0; i < pWaterAreas->GetSize(); i++)
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
		
		A3DCamera* pCameraReflect = m_pCameraReflect;
		if (m_bSimpleWater || vecCamPos.y < pArea->GetWaterHeight())
			pCameraReflect = NULL;

		pArea->DrawStaticMeshes(pViewport, m_fViewRadius, pCameraReflect);
	}

	if( m_pWaterStaticVertexShader )
	{
		m_pWaterStaticVertexShader->Disappear();
	}

	return true;
}

bool A3DTerrainWaterRender::RenderReflect(int nAreaArrayIdx, const A3DVECTOR3& vecSunDir, A3DViewport * pCurrentViewport, 
										  A3DTerrain2 * pTerrain, A3DSky * pSky, A3DTerrainOutline * pTerrainOutline, 
									LPRENDERFORREFLECT pReflectCallBack, LPRENDERFORREFRACT pRefractCallBack, void * pArg,
									LPRENDERONSKY pfnRenderOnSky, void * pArgSky, bool bBlendToScreen, A3DTerrain2 * pRealTerrain)
{		
	if( m_pWater->GetNumWaterAreas() == 0 )
		return true;

	int i, iNumMinorArea=0, iNumNormalArea=0;
	APtrArray<A3DWaterArea*>* pAreaArry = m_pWater->GetWaterAreasArray(nAreaArrayIdx);
	for(i=0; i<pAreaArry->GetSize(); i++)
	{
		A3DWaterArea* pArea = (*pAreaArry)[i];
		
		//	RenderReflect is called when camera is above water, so ignore water area that
		//	higher than camera
		if (m_pHostCamera->GetPos().y < pArea->GetWaterHeight())
			continue;
		
		pArea->m_bDoRender = m_pHostCamera->AABBInViewFrustum(pArea->m_aabbVisGrids);
		if (!pArea->m_bDoRender)
			continue;
		
		if (pArea->GetMinorWaterFlag())
			iNumMinorArea++;
		else
			iNumNormalArea++;
	}	

	if( iNumMinorArea == 0 && iNumNormalArea == 0)
		return true;

// 	if( m_type == WATER_TYPE_ALPHA_SIMPLE)
// 	{
// 
// 		RenderWaterAreasHeightRT();
// 
// 		if( pRealTerrain != NULL)
// 		{
// 			m_pCameraRefract->SetPos(m_pHostCamera->GetPos());
// 			m_pCameraRefract->SetDirAndUp(m_pHostCamera->GetDir(), m_pHostCamera->GetUp());
// 			m_pCameraRefract->SetProjectionParam(-1.0f, -1.0f, 1024.0f, -1.0f);
// 
// 			
// 			RenderSimpleAlphaEdge(m_pRefractViewport, pRealTerrain);
// 
// 			
// 		}
// 		m_pCameraReflect->SetMirror(m_pHostCamera, A3DVECTOR3(0.0f, m_pWater->GetWaterAreaHeight(nAreaArrayIdx), 0.0f), A3DVECTOR3(0.0f, 1.0f, 0.0f));
// 		m_pCameraReflect->Update(); //BlendToScreen() ->BlendToScreenPS()里面会用到m_pCameraRefelect
// 
// 		BlendToScreen(vecSunDir, pCurrentViewport, true, nAreaArrayIdx, iNumMinorArea, iNumNormalArea);
// 		return true;
// 	}

	if( !m_bSimpleWater && m_bCanReflect && iNumNormalArea > 0)
	{
		// first render terrain and sky into reflect target
 		m_pReflectTarget->ApplyToDevice();

		A3DVECTOR3 vecCamPos = m_pHostCamera->GetPos();

 		m_pCameraReflect->SetMirror(m_pHostCamera, A3DVECTOR3(0.0f, m_pWater->GetWaterAreaHeight(nAreaArrayIdx), 0.0f), A3DVECTOR3(0.0f, 1.0f, 0.0f));
 		m_pReflectViewport->Active();
 		m_pReflectViewport->SetClearColor(0xff000000 | m_fogWater);
		m_pReflectViewport->ClearDevice();

		A3DMATRIX4 matVP = m_pCameraReflect->GetVPTM();

		//	Simple water don't have reflection
		if (m_type != WATER_TYPE_ALPHA_SIMPLE)
		{
			if( pSky )
			{
				pSky->SetCamera(m_pCameraReflect);
				pSky->Render();
				pSky->SetCamera(m_pHostCamera);
			}
		
			// setup water clip plane
			// first clip plane in world space
			D3DXPLANE p(0.0f, 1.0f, 0.0f, -m_pWater->GetWaterAreaHeight(nAreaArrayIdx));
			
			m_pA3DDevice->SetClipPlaneProgrammable(0, &p, &matVP);
			m_pA3DDevice->SetDeviceRenderState(D3DRS_CLIPPLANEENABLE, D3DCLIPPLANE0);

			if( pfnRenderOnSky )
				(*pfnRenderOnSky)(m_pReflectViewport, pArgSky);

			if( pTerrainOutline )
				pTerrainOutline->RenderForReflected(m_pReflectViewport);

			if( pTerrain )
			{
				//	Create terrain water render if necessary
				if (m_pCurTerrain != pTerrain)
				{
					if (!CreateTerrainRender(pTerrain))
						return false;

					m_pCurTerrain = pTerrain;
				}

				p.d = -m_pWater->GetWaterAreaHeight(nAreaArrayIdx) + 0.0f;
				m_pA3DDevice->SetClipPlaneProgrammable(0, &p, &matVP);
				
				//	Render Terrain
				{
					m_pCurTerrain->SetExtRender(m_pTrnRender);
					m_pCurTerrain->Render(m_pReflectViewport);
					m_pCurTerrain->SetExtRender(NULL);
				}
			}
			
			p.d = -m_pWater->GetWaterAreaHeight(nAreaArrayIdx);
			m_pA3DDevice->SetClipPlaneProgrammable(0, &p, &matVP);
			
			// now render other things for reflected
			if( pReflectCallBack )
			{
				(*pReflectCallBack)(m_pReflectViewport, pArg);
			}

			m_pA3DDevice->SetDeviceRenderState(D3DRS_CLIPPLANEENABLE, 0);
		}

		m_pReflectTarget->WithdrawFromDevice();

		if (GetAsyncKeyState(VK_F7) & 0x8000)
			m_pReflectTarget->ExportColorToFile("Reflect.dds");
		


		pCurrentViewport->Active();

		//	TODO: In theory, below two liens doesn't have any usage, but in fact it can solve
		//		a very fuck problem in win7 system (WinXP doesn't have the problem). 
		//		If there are the 2 lines, clip plane won't work correctly, the result is that
		//		alpha meshes may disappear if water reflection is enabled.
		D3DXPLANE p = D3DXPLANE(0.0f, -1.0f, 0.0f, m_pWater->GetWaterAreaHeight(nAreaArrayIdx));
		m_pA3DDevice->SetClipPlaneProgrammable(0, &p, &matVP);

		///////////////////////////////////////////////////
		// now see if we need update the refract target
		///////////////////////////////////////////////////
		if (m_type != WATER_TYPE_GROUND)
		{
			if( m_bExpensiveWater)
			{
				m_pRefractTarget->ApplyToDevice();
				
				float dh = pCurrentViewport->GetCamera()->GetPos().y - m_pWater->GetWaterAreaHeight(nAreaArrayIdx);
				static float fogDis = 0.55f;
				float vFogEnd = m_fogWaterEnd * fogDis + dh;
				
				// first render things with air fog
				m_pA3DDevice->SetFogStart(0.01f + dh);
				m_pA3DDevice->SetFogEnd(vFogEnd);
				m_pA3DDevice->SetFogColor(m_fogWater);
				
				// now we setup far clip distance to a little bit more than fog end dist, so most things will be clipped by view frustum, this can
				// save a lot rendering time
				m_pCameraRefract->SetPos(m_pHostCamera->GetPos());
				m_pCameraRefract->SetDirAndUp(m_pHostCamera->GetDir(), m_pHostCamera->GetUp());
				m_pRefractViewport->Active();
				m_pRefractViewport->SetClearColor(0xff000000 | m_fogWater);
				m_pRefractViewport->ClearDevice();
				
				// setup water clip plane
				// first clip plane in world space
				D3DXPLANE p = D3DXPLANE(0.0f, -1.0f, 0.0f, m_pWater->GetWaterAreaHeight(nAreaArrayIdx));
				A3DMATRIX4 matVP = m_pCameraRefract->GetVPTM();
				m_pA3DDevice->SetClipPlaneProgrammable(0, &p, &matVP);
				m_pA3DDevice->SetDeviceRenderState(D3DRS_CLIPPLANEENABLE, D3DCLIPPLANE0);
				
				// try to render terrain, if pTerrain is NULL, then a fake terrain must be rendered by pfnRenderOnSky
				if( pTerrain )
				{
					if( pTerrain->UseLightmapTech() )
					{
						m_pA3DDevice->SetDeviceRenderState(D3DRS_CLIPPLANEENABLE, 0);
						pTerrain->RenderForRefract(m_pRefractViewport, m_pWater->GetWaterAreaHeight(nAreaArrayIdx));
						m_pA3DDevice->SetDeviceRenderState(D3DRS_CLIPPLANEENABLE, D3DCLIPPLANE0);
					}
					else
						pTerrain->Render(m_pRefractViewport);
				}
				else
				{
					p.d = m_pWater->GetWaterAreaHeight(nAreaArrayIdx) + 0.0f;
					m_pA3DDevice->SetClipPlaneProgrammable(0, &p, &matVP);
					if( pfnRenderOnSky )
						(*pfnRenderOnSky)(m_pRefractViewport, pArgSky);
				}
				
				p.d = m_pWater->GetWaterAreaHeight(nAreaArrayIdx);
				m_pA3DDevice->SetClipPlaneProgrammable(0, &p, &matVP);
				
				if( pRefractCallBack )
				{
					(*pRefractCallBack)(m_pRefractViewport, pArg);
				}
				m_pA3DDevice->SetDeviceRenderState(D3DRS_CLIPPLANEENABLE, 0);
				
				// then restore fog setting
				m_pA3DDevice->SetFogStart(m_fogAirStart);
				m_pA3DDevice->SetFogEnd(m_fogAirEnd);
				m_pA3DDevice->SetFogColor(m_fogAir);
				// update alpha channel to contain the edge alpha information
				if( pRealTerrain )
				{
					// we need to see all edge terrain;
					m_pCameraRefract->SetProjectionParam(-1.0f, -1.0f, 1024.0f, -1.0f);
					m_pCameraRefract->Active();
					m_pA3DDevice->Clear(D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
					RenderEdgeInfo(m_pRefractViewport, pRealTerrain,nAreaArrayIdx);
				}

				m_pRefractTarget->WithdrawFromDevice();
			}
			else
			{	// no refract target, so we have to render alpha onto reflect target
				m_pReflectTarget->ApplyToDevice();
 				

				m_pCameraRefract->SetPos(m_pHostCamera->GetPos());
				m_pCameraRefract->SetDirAndUp(m_pHostCamera->GetDir(), m_pHostCamera->GetUp());
				m_pRefractViewport->Active();
				if( pRealTerrain )
				{
					// we need to see all edge terrain;
					m_pCameraRefract->SetProjectionParam(-1.0f, -1.0f, 1024.0f, -1.0f);
					m_pCameraRefract->Active();
					m_pA3DDevice->Clear(D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
					RenderEdgeInfo(m_pRefractViewport, pRealTerrain, nAreaArrayIdx);
				}

				m_pReflectTarget->WithdrawFromDevice();
			}
		}

		pCurrentViewport->Active();
	}
	
	if( m_bTest)
	{
		D3DXSaveTextureToFileA("ReflectRT.dds", D3DXIFF_DDS, m_pReflectTarget->GetTargetTexture(), NULL);
		D3DXSaveTextureToFileA("RefractRT.dds", D3DXIFF_DDS, m_pRefractTarget->GetTargetTexture(), NULL);
		m_bTest = false;
	}

//	if( bBlendToScreen )
	if (1)
	{
		BlendToScreen(vecSunDir, pCurrentViewport, true, nAreaArrayIdx, iNumMinorArea, iNumNormalArea);
	}

	return true;
}



bool A3DTerrainWaterRender::RenderRefract(int nAreaArrayIdx, const A3DVECTOR3& vecSunDir, A3DViewport * pCurrentViewport, A3DTerrain2 * pTerrain, A3DSky * pSky, A3DTerrainOutline * pTerrainOutline, 
									LPRENDERFORREFRACT pRefractCallBack, void * pArg,
									LPRENDERONSKY pfnRenderOnSky, void * pArgSky, bool bBlendToScreen)
{
	if( m_pWater->GetNumWaterAreas() == 0 )
		return true;

	int i, iNumMinorArea=0, iNumNormalArea=0;
	APtrArray<A3DWaterArea*>* pAreaArry = m_pWater->GetWaterAreasArray(nAreaArrayIdx);
	for(i=0; i<pAreaArry->GetSize(); i++)
	{
		A3DWaterArea* pArea = (*pAreaArry)[i];

		//	RenderReflect is called when camera is under water, so ignore water area that
		//	lower than camera
		if (m_pHostCamera->GetPos().y > pArea->GetWaterHeight())
			continue;

		pArea->m_bDoRender = m_pHostCamera->AABBInViewFrustum(pArea->m_aabbVisGrids);
		if (!pArea->m_bDoRender)
			continue;

		if (pArea->GetMinorWaterFlag())
			iNumMinorArea++;
		else
			iNumNormalArea++;
	}
	if( iNumMinorArea == 0 && iNumNormalArea == 0)
		return true;

	if( !m_bSimpleWater && m_bCanReflect && m_type != WATER_TYPE_ALPHA_SIMPLE && iNumNormalArea > 0)
	{
		// first render terrain and sky into reflect target
		m_pReflectTarget->ApplyToDevice();

		// first render things with air fog
		m_pA3DDevice->SetFogStart(m_fogAirStart);
		m_pA3DDevice->SetFogEnd(m_fogAirEnd);
		m_pA3DDevice->SetFogColor(m_fogAir);

		m_pCameraRefract->SetProjectionParam(-1.0f, -1.0f, m_fogAirEnd * 1.1f, -1.0f);
		m_pCameraRefract->SetPos(m_pHostCamera->GetPos());
		m_pCameraRefract->SetDirAndUp(m_pHostCamera->GetDir(), m_pHostCamera->GetUp());
		m_pRefractViewport->Active();
		m_pRefractViewport->ClearDevice();

		if( pSky )
			pSky->Render();
		
		if( pfnRenderOnSky )
			(*pfnRenderOnSky)(m_pRefractViewport, pArgSky);

		if( pTerrainOutline )
			pTerrainOutline->Render(m_pRefractViewport);

		// setup water clip plane
		// first clip plane in world space
		D3DXPLANE p(0.0f, 1.0f, 0.0f, -m_pWater->GetWaterAreaHeight(nAreaArrayIdx));
		A3DMATRIX4 matVP = m_pCameraRefract->GetVPTM();
		m_pA3DDevice->SetClipPlaneProgrammable(0, &p, &matVP);
		m_pA3DDevice->SetDeviceRenderState(D3DRS_CLIPPLANEENABLE, D3DCLIPPLANE0);

		if( pTerrain )
		{
			p.d = -m_pWater->GetWaterAreaHeight(nAreaArrayIdx) + 0.5f;
			m_pA3DDevice->SetClipPlaneProgrammable(0, &p, &matVP);

			pTerrain->Render(m_pRefractViewport);
		}

		p.d = -m_pWater->GetWaterAreaHeight(nAreaArrayIdx);
		m_pA3DDevice->SetClipPlaneProgrammable(0, &p, &matVP);

		if( pRefractCallBack )
		{
			(*pRefractCallBack)(m_pRefractViewport, pArg);
		}

		m_pA3DDevice->SetDeviceRenderState(D3DRS_CLIPPLANEENABLE, 0);

		m_pReflectTarget->WithdrawFromDevice();
		pCurrentViewport->Active();
		
		// then blend onto screen with water fog
		m_pA3DDevice->SetFogStart(m_fogWaterStart);
		m_pA3DDevice->SetFogEnd(m_fogWaterEnd);
		m_pA3DDevice->SetFogColor(m_fogWater);
	}

//	if( bBlendToScreen )
	if (1)
	{
		if( m_type == WATER_TYPE_ALPHA_SIMPLE)
		{
			BlendToScreenNOPS(vecSunDir, pCurrentViewport, false, DRAW_ALLWATER, nAreaArrayIdx);
		}
		else
		{
			BlendToScreen(vecSunDir, pCurrentViewport, false, nAreaArrayIdx, iNumMinorArea, iNumNormalArea);
		}
	}

	return true;
}

bool A3DTerrainWaterRender::SetExpensiveWaterFlag(bool bFlag)
{
	if( !m_bCanExpensive || m_bSimpleWater )
		return true;

	if( m_bExpensiveWater == bFlag )
		return true;

	m_bExpensiveWater = bFlag;

	ReleaseWaterResources();
	if( !LoadWaterResources() )
	{
		ReleaseWaterResources();
		m_bSimpleWater = true;
	}
	return true;
}

bool A3DTerrainWaterRender::SetSimpleWaterFlag(bool bFlag)
{
	if( !m_bCanReflect )
		return true;

	if( m_bSimpleWater == bFlag )
		return true;

	m_bSimpleWater = bFlag;
	
	if( m_bSimpleWater )
	{
		ReleaseWaterResources();
		for(int i=0; i<m_nNumTexNormal; i++)
		{
			if( m_pTexNormals[i] )
			{
				m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pTexNormals[i]);
				m_pTexNormals[i] = NULL;
			}
		}
	}
	else
	{
		if( !LoadWaterResources() )
		{		   
			ReleaseWaterResources();
			m_bSimpleWater = true;
			return false;
		}

		char szFile[MAX_PATH];
		for(int i=0; i<m_nNumTexNormal; i++)
		{
			if( m_pTexNormals[i] == NULL )
			{
				sprintf(szFile, "textures\\water\\waternormal\\water_normal%02d.dds", i);
				if( !m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFile(szFile, &m_pTexNormals[i]) )
				{
					g_A3DErrLog.Log("A3DTerrainWater::SetSimpleWaterFlag(), fail to load texture [%s]", szFile);
					return false;
				}
			}
		}
	}

	return true;
}

bool A3DTerrainWaterRender::ReloadWaterResources()
{			
	if( m_bSimpleWater )
		return true;

	ReleaseWaterResources();

	if( !LoadWaterResources() )
	{
		ReleaseWaterResources();
		m_bSimpleWater = true;
	}

	return true;
}







void A3DTerrainWaterRender::SetDNFactor(float f)
{
	m_fDNFactor = f;
	if( m_fDNFactor < 0.0f )
		m_fDNFactor = 0.0f;
	if( m_fDNFactor > 1.0f )
		m_fDNFactor = 1.0f;
}

bool A3DTerrainWaterRender::RenderEdgeInfo(A3DViewport * pViewport, A3DTerrain2 * pTerrain2, int nAreaArrayIdx)
{
	if( m_bSimpleWater || !m_bCanReflect || m_type == WATER_TYPE_GROUND)
		return true;

	m_Material.Appear();

	m_pA3DDevice->SetDeviceRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_ALPHA);
	m_pA3DDevice->SetLightingEnable(false);
	m_pA3DDevice->SetSpecularEnable(false);
	bool bFog = m_pA3DDevice->GetFogEnable();
	m_pA3DDevice->SetFogEnable(false);
	m_pA3DDevice->SetWorldMatrix(IdentityMatrix());
	m_pA3DDevice->SetAlphaBlendEnable(false);
	m_pEdgeTerrainStream->Appear();

	m_pA3DDevice->ClearTexture(0);
	m_pEdgePixelShader->Appear();
	m_pEdgeVertexShader->Appear();

	A3DCameraBase * pCamera = pViewport->GetCamera();

	A3DVECTOR4 c0;
	c0.x = 1.0f;
	c0.y = m_pWater->GetWaterAreaHeight(nAreaArrayIdx);
	c0.z = 1.0f / m_vCurEdgeHeight;
	c0.w = 0.0f;
	m_pA3DDevice->SetVertexShaderConstants(0, &c0, 1);
	A3DMATRIX4 matVP = pCamera->GetVPTM();
	matVP.Transpose();
	m_pA3DDevice->SetVertexShaderConstants(1, &matVP, 4);
	A3DVECTOR4 c5(pCamera->GetPos());
	m_pA3DDevice->SetVertexShaderConstants(5, &c5, 1);
	
/*	pTerrain2->DoBlocksCull(pViewport);

	A3DTerrain2Render * pTerrain2Render = pTerrain2->GetTerrainRender();
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
		pTerrain2Render->GetRenderMesh(i, (BYTE *)pVerts, sizeof(A3DVERTEX), pIndices, &nVert, &nIndex);
		
		m_pEdgeTerrainStream->UnlockVertexBuffer();
		m_pEdgeTerrainStream->UnlockIndexBuffer();

		if( nVert > 0 && nIndex > 0 )
			m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, nVert, 0, nIndex / 3);
	}
*/
	pTerrain2->RenderRawMesh(pViewport, false, false);

	m_pA3DDevice->SetDeviceRenderState(D3DRS_COLORWRITEENABLE, 0x0000000F);

	m_pEdgeVertexShader->Disappear();
	m_pEdgePixelShader->Disappear();
	m_pA3DDevice->SetTextureCoordIndex(0, 0);
	m_pA3DDevice->SetTextureTransformFlags(0, A3DTTFF_DISABLE);

	m_pA3DDevice->SetLightingEnable(true);
	m_pA3DDevice->SetSpecularEnable(true);
	m_pA3DDevice->SetFogEnable(bFog);
	m_pA3DDevice->SetAlphaBlendEnable(true);

	return true;
}

void A3DTerrainWaterRender::SetWaterType(WATER_TYPE type)
{
	if (m_type == type)
		return;

	if (type == WATER_TYPE_ANIMATED)
	{
		m_matBump._11 = 8.0f;
		m_matBump._22 = 8.0f;
		m_matBump._31 = 0.5f;
		m_matBump._32 = 0.5f;
		m_matBump2._11 = 2.0f;
		m_matBump2._22 = 2.0f;
		m_matBump2._31 = 0.5f;
		m_matBump2._32 = 0.0f;
	}
	else if (type == WATER_TYPE_SCROLL)
	{
		m_matBump._11 = 4.0f;
		m_matBump._22 = 4.0f;
		m_matBump._31 = 0.0f;
		m_matBump._32 = 0.0f;
		m_matBump2._11 = 1.0f;
		m_matBump2._22 = 1.0f;
		m_matBump2._31 = 0.0f;
		m_matBump2._32 = 0.0f;
	}
	else if (type == WATER_TYPE_GROUND)
	{
		if (!m_pGndReflectShader)
		{
			m_pGndReflectShader = m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->LoadPixelShader("Shaders\\2.2\\ps\\ground_reflect.txt", false);
			if (!m_pGndReflectShader)
			{
				g_A3DErrLog.Log("A3DTerrainWater::SetWaterType, failed to load ground_reflect.txt");
				return;
			}
		}
	}
	else if (type == WATER_TYPE_ALPHA_SIMPLE)
	{
		if (!m_bSimpleWater && !m_pWaterAlphaSimple)
		{
			ASSERT(0);
			return;
		}
	}

	m_type = type;
}

void A3DTerrainWaterRender::SetGroundProp(const char* szBaseTex, float fUVScale)
{
	m_fGndUVScale = fUVScale;

	A3DTextureMan* pA3DTexMan = m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan();
	if (m_pGndBaseTex)
	{
		pA3DTexMan->ReleaseTexture(m_pGndBaseTex);
		m_pGndBaseTex = NULL;
	}

	if (!pA3DTexMan->LoadTextureFromFile(szBaseTex, &m_pGndBaseTex))
	{
		g_A3DErrLog.Log("A3DTerrainWater::SetGroundProp, fail to load ground texture");
		return;
	}
}

//	Create terrain render
bool A3DTerrainWaterRender::CreateTerrainRender(A3DTerrain2* pTerrain)
{
	//	Release current render
	A3DRELEASE(m_pTrnRender);

	if (!pTerrain)
		return true;

	//	Create terrain render
	m_pTrnRender = new A3DTerrain2Render;
	if (!m_pTrnRender || !m_pTrnRender->Init(pTerrain))
	{
		g_A3DErrLog.Log("A3DTerrainWaterRender, failed to create terrain render");
		return false;
	}

	if (!m_pTrnRender->OnTerrainLoaded())
	{
		g_A3DErrLog.Log("A3DTerrainWaterRender, failed to create call OnTerrainLoaded");
		return false;
	}

	return true;
}

//	Before device reset
bool A3DTerrainWaterRender::BeforeDeviceReset()
{
	return true;
}

//	After device reset
bool A3DTerrainWaterRender::AfterDeviceReset()
{
	if (m_pTrnRender)
		m_pTrnRender->SetRebuildStreamFlag(true);

	return true;
}
