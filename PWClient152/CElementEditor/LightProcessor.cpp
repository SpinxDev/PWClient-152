/*
 * FILE: LightProcessor.cpp
 *
 * DESCRIPTION: Class representing for light processor;
 *
 * CREATED BY: Hedi, 2004/8/30
 *
 * HISTORY:
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.	
 */

#include "LightProcessor.h"
#include "A3DEngine.h"
#include "A3DLight.h"
#include "A3DLightMan.h"
#include "A3DLitModel.h"
#include "A3DTypes.h"
#include "A3DVertex.h"
#include "A3DDevice.h"
#include "A3DOrthoCamera.h"
#include "A3DStream.h"
#include "A3DTexture.h"
#include "A3DViewport.h"
#include "A3DRenderTarget.h"
#include "A3DSurface.h"
#include "A3DFuncs.h"
#include "A3DTerrainWater.h"
#include "Render.h"
#include "SceneObjectManager.h"
#include "SceneObject.h"
#include "StaticModelObject.h"
#include "SceneLightObject.h"
#include "EL_Building.h"
#include "ATime.h"
#include "a3dpixelshader.h"
#include "a3dvertexshader.h"
#include "A3DShaderMan.h"

#include   <list> 
#include   <tchar.h>   
#include   <locale.h>   
#include   <A3DCollision.h>
#include <A3DWaterArea.h>


//#define new A_DEBUG_NEW

static void GaussianBlurColorValue(A3DCOLORVALUE * pImage, int nSize, double vBlurRange);

CLightProcessor::CLightProcessor()
{
	m_pA3DDevice				= NULL;
	m_pTerrain					= NULL;

	m_pLightShadowMapViewport	= NULL;	
	m_pLightShadowMapCamera		= NULL;	
	m_pLightShadowMapTarget		= NULL;	
	                           
	m_nShadowMapSize			= 0;
	m_nLightMapBlockSize		= 0;		
	m_nLightMapBlockNum			= 0;
	
	m_pLightMapViewport			= NULL;
	m_pLightMapCamera			= NULL;
	m_pLightMapTarget			= NULL;

	m_pPixelRenderCamera		= NULL;
	m_pPixelRenderViewport		= NULL;

	m_pTempSurface				= NULL;
	
	m_pLightSun					= NULL;

	memset(m_pSceneLights, 0, sizeof(A3DLight *) * 8);

	m_pTerrainStream			= NULL;
	m_pTerrainVerts				= NULL;
	m_pTerrainIndices			= NULL;
	m_nPixelForModelSize        = 2048;
	m_bSoftware					= true;

	m_pShadowVertexShader = NULL;	
	m_pShadowRenderShader = NULL;

	m_bFast = false;
}

CLightProcessor::~CLightProcessor()
{
	Release();
}

bool CLightProcessor::CreateTerrainStream()
{
	// try to release old one
	ReleaseTerrainStream();

	m_nGridX = m_pTerrain->GetHeightMap()->iWidth - 1;
	m_nGridZ = m_pTerrain->GetHeightMap()->iHeight - 1;

	m_pTerrainStream = new A3DStream();
	if( !m_pTerrainStream->Init(m_pA3DDevice, A3DVT_VERTEX, (m_nGridX + 3) * (m_nGridZ + 3), (m_nGridX + 2) * (m_nGridZ + 2) * 6, A3DSTRM_REFERENCEPTR, A3DSTRM_INDEX32 | A3DSTRM_REFERENCEPTR) )
	{
		g_Log.Log("CLightProcessor::CreateTerrainStream(), fail to init terrain stream!");
		return false;
	}

	int		nIndex;
	int		nVertIndex;
	m_pTerrainVerts	= new A3DVERTEX[(m_nGridX + 3) * (m_nGridZ + 3)];
	m_pTerrainIndices = new DWORD[(m_nGridX + 2) * (m_nGridZ + 2) * 6];

	A3DVECTOR3 * pVerts = m_pTerrain->GetVertexPos();
	CTerrain::HEIGHTMAP * pHeightMap = m_pTerrain->GetHeightMap();
	A3DVECTOR3 vecTerrain;
	A3DVECTOR3 vecNormal;
	for(int i=0; i<=m_nGridZ+2; i++)
	{
		for(int j=0; j<=m_nGridX+2; j++)
		{
			nVertIndex = i * (m_nGridX + 3) + j;

			if( i == 0 )
			{
				if( j == 0 )
				{
					vecTerrain = pVerts[0];
					vecNormal = pHeightMap->aNormals[0];
				}
				else if( j == m_nGridX + 2 )
				{
					vecTerrain = pVerts[m_nGridX];
					vecNormal = pHeightMap->aNormals[m_nGridX];
				}
				else
				{
					vecTerrain = pVerts[j - 1];
					vecNormal = pHeightMap->aNormals[j - 1];
				}
				vecTerrain.y = m_pTerrain->GetYOffset();
			}
			else if( i == m_nGridZ + 2 )
			{
				int nLastRow = m_nGridZ * (m_nGridX + 1);
				if( j == 0 )
				{
					vecTerrain = pVerts[nLastRow];
					vecNormal = pHeightMap->aNormals[nLastRow];
				}
				else if( j == m_nGridX + 2 )
				{
					vecTerrain = pVerts[nLastRow + m_nGridX];
					vecNormal = pHeightMap->aNormals[nLastRow + m_nGridX];
				}
				else
				{
					vecTerrain = pVerts[nLastRow + j - 1];
					vecNormal = pHeightMap->aNormals[nLastRow + j - 1];
				}
				vecTerrain.y = m_pTerrain->GetYOffset();
			}
			else if( j == 0 )
			{
				vecTerrain = pVerts[(i - 1) * (m_nGridX + 1)];
				vecNormal = pHeightMap->aNormals[(i - 1) * (m_nGridX + 1)];
				vecTerrain.y = m_pTerrain->GetYOffset();
			}
			else if( j == m_nGridX + 2 )
			{
				vecTerrain = pVerts[(i - 1) * (m_nGridX + 1) + m_nGridX];
				vecNormal = pHeightMap->aNormals[(i - 1) * (m_nGridX + 1) + m_nGridX];
				vecTerrain.y = m_pTerrain->GetYOffset();
			}
			else
			{
				vecTerrain = pVerts[(i - 1) * (m_nGridX + 1) + j - 1];
				vecNormal = pHeightMap->aNormals[(i - 1) * (m_nGridX + 1) + j - 1];
			}

			m_pTerrainVerts[nVertIndex] = A3DVERTEX(vecTerrain,	vecNormal, 0.0f, 0.0f);
			
			if( i != m_nGridZ + 2 && j != m_nGridX + 2)
			{
				nIndex = (i * (m_nGridX + 2) + j) * 6;
				m_pTerrainIndices[nIndex    ] = nVertIndex;
				m_pTerrainIndices[nIndex + 1] = nVertIndex + 1;
				m_pTerrainIndices[nIndex + 2] = nVertIndex + m_nGridX + 3;

				m_pTerrainIndices[nIndex + 3] = m_pTerrainIndices[nIndex + 2];
				m_pTerrainIndices[nIndex + 4] = m_pTerrainIndices[nIndex + 1];
				m_pTerrainIndices[nIndex + 5] = nVertIndex + m_nGridX + 3 + 1;
			}
		}
	}

	m_pTerrainStream->SetVerts((BYTE*) m_pTerrainVerts, (m_nGridX + 3) * (m_nGridZ + 3));
	m_pTerrainStream->SetIndices((BYTE*) m_pTerrainIndices, (m_nGridX + 2) * (m_nGridZ + 2) * 6); 
	return true;
}

bool CLightProcessor::ReleaseTerrainStream()
{
	if( m_pTerrainIndices )
	{
		delete [] m_pTerrainIndices;
		m_pTerrainIndices = NULL;
	}

	if( m_pTerrainVerts )
	{
		delete [] m_pTerrainVerts;
		m_pTerrainVerts = NULL;
	}

	if( m_pTerrainStream )
	{
		m_pTerrainStream->Release();
		delete m_pTerrainStream;
		m_pTerrainStream = NULL;
	}

	return true;
}

bool CLightProcessor::CreateRenderTargets()
{
	// now create a shadow map generator camera
	m_pLightShadowMapCamera = new A3DOrthoCamera();
	if( !m_pLightShadowMapCamera->Init(m_pA3DDevice, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f) )
	{
		g_Log.Log("CLightProcessor::Init(), failed to create the shadow map camera!");
		return false;
	}
	
	//	Create light's shadow map Viewport
	RECT rc;
	rc.left = 0;
	rc.right = m_nShadowMapSize;
	rc.top = 0;
	rc.bottom = m_nShadowMapSize;
	if( !m_pA3DDevice->CreateViewport(&m_pLightShadowMapViewport, 0, 0, rc.right, rc.bottom,
									0.0f, 1.0f, true, true, 0x00000000) )
	{
		g_Log.Log("CLightProcessor::Create shadow map viewport fail");
		return false;
	}
	m_pLightShadowMapViewport->SetCamera(m_pLightShadowMapCamera);

	// create light's shadow map render target
	A3DDEVFMT devFmt;
	devFmt.bWindowed	= true;
	devFmt.nWidth		= m_nShadowMapSize;
	devFmt.nHeight		= m_nShadowMapSize;
	devFmt.fmtTarget	= A3DFMT_X8R8G8B8;
	if(m_bSoftware) devFmt.fmtDepth		= (A3DFORMAT) MAKEFOURCC('I', 'N', 'T', 'Z'); //A3DFMT_D24X8;
	else devFmt.fmtDepth = A3DFMT_D24X8;

	m_pLightShadowMapTarget = new A3DRenderTarget();
	
	if( !m_pLightShadowMapTarget->Init(m_pA3DDevice, devFmt, true, true) )
		return false;

	// now create a light map generator camera
	m_pLightMapCamera = new A3DOrthoCamera();
	if( !m_pLightMapCamera->Init(m_pA3DDevice, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f) )
	{
		g_Log.Log("CLightProcessor::Init(), failed to create the light map camera!");
		return false;
	}

	// now create a pixel render camera
	m_pPixelRenderCamera = new A3DOrthoCamera();
	if( !m_pPixelRenderCamera->Init(m_pA3DDevice, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f) )
	{
		g_Log.Log("CLightProcessor::Init(), failed to create the pixel render camera!");
		return false;
	}
	
	//	Create light's shadow map Viewport
	rc.left = 0;
	rc.right = m_nLightMapBlockSize;
	rc.top = 0;
	rc.bottom = m_nLightMapBlockSize;
	if( !m_pA3DDevice->CreateViewport(&m_pLightMapViewport, 0, 0, rc.right, rc.bottom,
									0.0f, 1.0f, true, true, 0x00000000) )
	{
		g_Log.Log("CLightProcessor::Create light map viewport fail");
		return false;
	}
	m_pLightMapViewport->SetCamera(m_pLightMapCamera);

	//	Create pixel render Viewport
	rc.left = 0;
	rc.right = 2;
	rc.top = 0;
	rc.bottom = 2;
	if( !m_pA3DDevice->CreateViewport(&m_pPixelRenderViewport, 0, 0, rc.right, rc.bottom,
									0.0f, 1.0f, true, true, 0x00000000) )
	{
		g_Log.Log("CLightProcessor::Create pixel render viewport fail");
		return false;
	}
	m_pPixelRenderViewport->SetCamera(m_pPixelRenderCamera);

	// create light's shadow map render target
	devFmt.bWindowed	= true;
	devFmt.nWidth		= m_nLightMapBlockSize;
	devFmt.nHeight		= m_nLightMapBlockSize;
	devFmt.fmtTarget	= A3DFMT_X8R8G8B8;
	devFmt.fmtDepth		= A3DFMT_D24X8;

	m_pLightMapTarget = new A3DRenderTarget();
	if( !m_pLightMapTarget->Init(m_pA3DDevice, devFmt, true, true) )
		return false;

	// now create temp surface;
	m_pTempSurface = new A3DSurface();
	if( !m_pTempSurface->Create(m_pA3DDevice, m_nLightMapBlockSize, m_nLightMapBlockSize, A3DFMT_X8R8G8B8) )
	{
		g_Log.Log("CLightProcessor::CreateRenderTargets(), failed to create temp surface!");
		return false;
	}

	//创建一个为模型光照的专门视口
	rc.left = 0;
	rc.right = m_nPixelForModelSize;
	rc.top = 0;
	rc.bottom = m_nPixelForModelSize;
	if( !m_pA3DDevice->CreateViewport(&m_pPixelForModelViewport, 0, 0, rc.right, rc.bottom,
									0.0f, 1.0f, true, true, 0x00000000) )
	{
		g_Log.Log("CLightProcessor::Create viewport for model light fail");
		return false;
	}
	m_pPixelForModelViewport->SetCamera(m_pLightMapCamera);

	//创建一个为模型单独准备的临时表面
	m_pTempSurfaceForModel = new A3DSurface();
	if( !m_pTempSurfaceForModel->Create(m_pA3DDevice, m_nPixelForModelSize, m_nPixelForModelSize, A3DFMT_X8R8G8B8) )
	{
		g_Log.Log("CLightProcessor::CreateRenderTargets(), failed to create temp surface for models!");
		return false;
	}

	//创建一个单独为模型准备的渲染目标
	// create light's shadow map render target
	devFmt.bWindowed	= true;
	devFmt.nWidth		= m_nPixelForModelSize;
	devFmt.nHeight		= m_nPixelForModelSize;
	devFmt.fmtTarget	= A3DFMT_X8R8G8B8;
	devFmt.fmtDepth		= A3DFMT_D24X8;

	m_pLightMapTargetForModel = new A3DRenderTarget();
	if( !m_pLightMapTargetForModel->Init(m_pA3DDevice, devFmt, true, true) )
		return false;
	return true;
}

bool CLightProcessor::ReleaseRenderTargets()
{
	if( m_pTempSurface )
	{
		m_pTempSurface->Release();
		delete m_pTempSurface;
		m_pTempSurface = NULL;
	}

	if( m_pPixelForModelViewport )
	{
		m_pPixelForModelViewport->Release();
		delete m_pPixelForModelViewport;
		m_pPixelForModelViewport = NULL;
	}

	if( m_pPixelRenderViewport )
	{
		m_pPixelRenderViewport->Release();
		delete m_pPixelRenderViewport;
		m_pPixelRenderViewport = NULL;
	}

	if( m_pPixelRenderCamera )
	{
		m_pPixelRenderCamera->Release();
		delete m_pPixelRenderCamera;
		m_pPixelRenderCamera = NULL;
	}

	if( m_pLightMapTarget )
	{
		m_pLightMapTarget->Release();
		delete m_pLightMapTarget;
		m_pLightMapTarget = NULL;
	}

	if( m_pLightMapViewport )
	{
		m_pLightMapViewport->Release();
		delete m_pLightMapViewport;
		m_pLightMapViewport = NULL;
	}

	if( m_pLightMapCamera )
	{
		m_pLightMapCamera->Release();
		delete m_pLightMapCamera;
		m_pLightMapCamera = NULL;
	}

	if( m_pLightShadowMapTarget )
	{
		m_pLightShadowMapTarget->Release();
		delete m_pLightShadowMapTarget;
		m_pLightShadowMapTarget = NULL;
	}

	if(m_pLightMapTargetForModel)
	{
		m_pLightMapTargetForModel->Release();
		delete m_pLightMapTargetForModel;
		m_pLightMapTargetForModel = NULL;
	}

	if( m_pLightShadowMapViewport )
	{
		m_pLightShadowMapViewport->Release();
		delete m_pLightShadowMapViewport;
		m_pLightShadowMapViewport = NULL;
	}

	if( m_pLightShadowMapCamera )
	{
		m_pLightShadowMapCamera->Release();
		delete m_pLightShadowMapCamera;
		m_pLightShadowMapCamera = NULL;
	}

	if( m_pTempSurfaceForModel )
	{
		m_pTempSurfaceForModel->Release();
		delete m_pTempSurfaceForModel;
		m_pTempSurfaceForModel = NULL;
	}
	return true;
}

bool CLightProcessor::Init(A3DDevice * pA3DDevice, CExpTerrain * pTerrain, CTerrainWater * pTerrainWater, CSceneObjectManager * pSceneObjectManager, int nShadowMapSize, int nLightMapSize, int nLightMapDivide, bool bOnlyRenderShadow, bool bFast)
{
	m_bFast = bFast;
	
	HRESULT hval;
	hval = pA3DDevice->GetA3DEngine()->GetD3D()->CheckDeviceFormat(D3DADAPTER_DEFAULT, (D3DDEVTYPE)pA3DDevice->GetDevType(), 
		(D3DFORMAT)pA3DDevice->GetDevFormat().fmtTarget, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_TEXTURE, (D3DFORMAT)A3DFMT_D24X8);
	if( D3D_OK != hval )
		return false;

	int r = 1;//DisplayCardInfo();
	if(r==-1) 
	{
		g_Log.Log("CLightProcessor::Init(), display card doesn't support !");
		return false;
	}
	m_bSoftware = (r == 1) ? true : false ;

	m_pA3DDevice			= pA3DDevice;
	m_pTerrain				= pTerrain;
	m_pTerrainWater			= pTerrainWater;
	m_pSceneObjectManager	= pSceneObjectManager;

	m_nShadowMapSize		= nShadowMapSize;

	m_nLightMapBlockSize	= nLightMapSize / nLightMapDivide;
	m_nLightMapBlockNum		= nLightMapDivide;
	m_bOnlyRenderShadow     = bOnlyRenderShadow;

	// now load the vertex shader
	DWORD	dwVSDecl[20];
	int		n = 0;				
	dwVSDecl[n ++] = D3DVSD_STREAM(0);					// begin tokens.
	dwVSDecl[n ++] = D3DVSD_REG(0, D3DVSDT_FLOAT3);		// x, y, z in world
	dwVSDecl[n ++] = D3DVSD_REG(1, D3DVSDT_FLOAT3);		// normal in world
	dwVSDecl[n ++] = D3DVSD_REG(2, D3DVSDT_FLOAT2);		// u, v coords.
	dwVSDecl[n ++] = D3DVSD_END();						// end tokens.
	
	m_pShadowVertexShader = m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->LoadVertexShader("Shaders\\vs\\shadow_map_vs.txt", false, dwVSDecl);
	if( NULL == m_pShadowVertexShader )
	{
		g_Log.Log("CLightProcessor::Init(), failed to load shadow_map_vs.txt");
	}

	m_pShadowRenderShader = m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->LoadPixelShader("Shaders\\ps\\shadow_map_ps.txt", false);
	if( NULL == m_pShadowRenderShader )
	{
		g_Log.Log("CLightProcessor::Init(), failed to load shadow_map_ps.txt");
	}

	if( !CreateRenderTargets() )
	{
		g_Log.Log("CLightProcessor::Init(), Can not create render targets!");
		return false;
	}

	if( !CreateTerrainStream() )
	{
		g_Log.Log("CLightProcessor::Init(), Can not create terrain stream!");
		return false;
	}

	if( !CreateAllLights() )
	{
		g_Log.Log("CLightProcessor::Init(), Can not create scene lights!");
		return false;
	}
	// define terrain material
	m_Material.Init(m_pA3DDevice);
	m_Material.SetPower(0.0f);
	m_Material.SetDiffuse(A3DCOLORVALUE(1.0f, 1.0f, 1.0f, 1.0f));
	m_Material.SetAmbient(A3DCOLORVALUE(1.0f, 1.0f, 1.0f, 1.0f));
	m_Material.SetEmissive(A3DCOLORVALUE(0.0f));
	m_Material.SetSpecular(A3DCOLORVALUE(0.0f));

	// now setup a projected texture scale matrix
	m_matTexProjScale = IdentityMatrix();
	float offset = 0.5f / m_nShadowMapSize;

	m_matTexProjScale._11 = 0.5f;
	m_matTexProjScale._22 = -0.5f;
	m_matTexProjScale._33 = float(1<<24);
	m_matTexProjScale._41 = 0.5f + offset;
	m_matTexProjScale._42 = 0.5f + offset;
	m_matTexProjScale._43 = -20000.0f;
	
	return true;
}

bool CLightProcessor::UpdateHeightMap(CExpTerrain * pTerrain)
{
	// to be implemented!
	return true;
}

bool CLightProcessor::Release()
{
	if(m_pShadowVertexShader)
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->ReleaseVertexShader(&m_pShadowVertexShader);
		m_pShadowVertexShader = NULL;
	}

	if(m_pShadowRenderShader)
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->ReleasePixelShader(&m_pShadowRenderShader);
		m_pShadowRenderShader = NULL;
	}

	m_Material.Release();

	ReleaseAllLights();
	ReleaseTerrainStream();
	ReleaseRenderTargets();
	return true;
}

bool CLightProcessor::CreateAllLights()
{
	//	create a directional light to shadowing
	m_pA3DDevice->GetA3DEngine()->GetA3DLightMan()->CreateDirecionalLight(9, &m_pLightSun, 
		A3DVECTOR3(0.0f, 0.0f, 0.0f), A3DVECTOR3(-1.0f, -0.4f, -1.0f), A3DCOLORVALUE(1.0f, 1.0f, 1.0f, 1.0f), 
		A3DCOLORVALUE(0.0f, 0.0f, 0.0f, 1.0f), A3DCOLORVALUE(0.0f, 0.0f, 0.0f, 1.0f));
	m_pLightSun->TurnOff();

	// create scene lights for lighting calculations
	for(int i=0; i<8; i++)
	{
		m_pSceneLights[i] = new A3DLight();
		m_pSceneLights[i]->Init(m_pA3DDevice, i + 10);

		A3DLIGHTPARAM param;

		ZeroMemory(&param, sizeof(param));
		param.Type = A3DLIGHT_POINT;
		param.Position = A3DVECTOR3(0.0f);
		param.Direction = A3DVECTOR3(0.0f, 0.0f, 1.0f);
		param.Diffuse = 0xffffffff;
		param.Specular = 0xff000000;
		param.Ambient = 0xff000000;

		m_pSceneLights[i]->SetLightParam(param);
		m_pSceneLights[i]->TurnOff();
	}

	return true;
}

bool CLightProcessor::ReleaseAllLights()
{
	for(int i=0; i<8; i++)
	{
		if( m_pSceneLights[i] )
		{
			m_pSceneLights[i]->Release();
			delete m_pSceneLights[i];
			m_pSceneLights[i] = NULL;
		}
	}

	if( m_pLightSun )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DLightMan()->ReleaseLight(m_pLightSun);
		m_pLightSun = NULL;
	}

	return true;
}

bool CLightProcessor::TurnOffAllLights()
{
	for(int i=0; i<8; i++)
	{
		if( m_pSceneLights[i] )
			m_pSceneLights[i]->TurnOff();
	}

	return true;
}

bool CLightProcessor::SetupSceneLight(int index, CSceneLightObject * pSceneLight)
{
	A3DLIGHTPARAM param;
	ZeroMemory(&param, sizeof(param));

	param.Diffuse	= 0xff000000 | pSceneLight->m_Data.m_dwColor;
	param.Specular	= 0xff000000;
	param.Ambient	= pSceneLight->m_Data.m_dwAbient;
	param.Diffuse	= param.Diffuse * (0.5f * pSceneLight->m_Data.m_fColorScale);
	param.Ambient	= param.Ambient * 0.5f;
	switch(pSceneLight->m_Data.m_nType)
	{
	case 0: // point light
		param.Type		= A3DLIGHT_POINT;
		param.Position	= pSceneLight->GetPos();

		param.Range		= pSceneLight->m_Data.m_fRange;
		param.Attenuation0 = pSceneLight->m_Data.m_fAttenuation0;
		param.Attenuation1 = pSceneLight->m_Data.m_fAttenuation1;
		param.Attenuation2 = pSceneLight->m_Data.m_fAttenuation2;

		break;

	case 1: // direction light
		param.Type		= A3DLIGHT_DIRECTIONAL;
		param.Direction = pSceneLight->GetRotateMatrix().GetRow(2);

		break;

	case 2: // spot light
		param.Type		= A3DLIGHT_SPOT;
		param.Position	= pSceneLight->GetPos();
		param.Direction = pSceneLight->GetRotateMatrix().GetRow(2);

		param.Range		= pSceneLight->m_Data.m_fRange;
		param.Attenuation0 = pSceneLight->m_Data.m_fAttenuation0;
		param.Attenuation1 = pSceneLight->m_Data.m_fAttenuation1;
		param.Attenuation2 = pSceneLight->m_Data.m_fAttenuation2;

		param.Falloff	= pSceneLight->m_Data.m_fFalloff;
		param.Theta		= pSceneLight->m_Data.m_fTheta;
		param.Phi		= pSceneLight->m_Data.m_fPhi;
		break;
	}

	m_pSceneLights[index]->SetLightParam(param);
	m_pSceneLights[index]->TurnOn();
	return true;
}

bool CLightProcessor::GenerateLightMaps(A3DCOLORVALUE * pColors, bool bNight, LPRENDERFORSHADOW pfnRenderForShadow, LPVOID pArg)
{
	//DWORD m_dwTestTime = a_GetTime();//del
	A3DViewport * pCurrentViewport = m_pA3DDevice->GetA3DEngine()->GetActiveViewport();
	
	m_pLightShadowMapCamera->SetPos(m_vecWorldCenter);
	m_pLightShadowMapCamera->SetDirAndUp(m_vecSunDir, A3DVECTOR3(0.0f, 1.0f, 0.0f));

	A3DMATRIX4 matView = m_pLightShadowMapCamera->GetViewTM();
	float l, r, b, t, n, f;
	GetWorldBounds(m_aabbWorld, matView, l, r, b, t, n, f);
	m_pLightShadowMapCamera->SetProjectionParam(l * 1.01f, r * 1.01f, b * 1.01f, t * 1.01f, n * 1.01f, f * 1.01f);
	m_pLightShadowMapCamera->SetPos(m_vecWorldCenter);

	// Apply material
	m_Material.Appear();

	m_pA3DDevice->SetLightingEnable(true);
	m_pA3DDevice->SetSpecularEnable(false);

	bool bFogEnable = m_pA3DDevice->GetFogEnable();
	m_pA3DDevice->SetFogEnable(false);

	// first render into light's shadow map
	m_pA3DDevice->SetRenderTarget(m_pLightShadowMapTarget);
	m_pLightShadowMapViewport->Active();
	m_pLightShadowMapViewport->ClearDevice();
	
	// 1. render terrain
	m_pTerrainStream->Appear();
	m_pA3DDevice->ClearTexture(0);
	m_pA3DDevice->SetWorldMatrix(IdentityMatrix());
	m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, (m_nGridX + 3) * (m_nGridZ + 3),
		0, (m_nGridX + 2) * (m_nGridZ + 2) * 2);
	
	// 2. render other things that can cast shadows
	if( pfnRenderForShadow )
	{
		(*pfnRenderForShadow)(m_pLightShadowMapViewport, pArg);
	}
	
	// output a test file
	
	//CString outpath;
	//outpath.Format("Shadowmap%s.bmp",m_pSceneObjectManager->GetMap()->GetMapName());
	//D3DXSaveTextureToFile(outpath, D3DXIFF_BMP, m_pLightShadowMapTarget->GetTargetTexture(), NULL);
	
	// Apply material again
	m_Material.Appear();
	m_pA3DDevice->SetLightingEnable(true);
	m_pA3DDevice->SetSpecularEnable(false);
	m_pA3DDevice->SetFogEnable(false);
	m_pTerrainStream->Appear();
	m_pA3DDevice->ClearTexture(0);
	m_pA3DDevice->SetWorldMatrix(IdentityMatrix());

	// now render from top of the world to get out the light map
	m_pLightMapCamera->SetPos(m_vecWorldCenter);
	m_pLightMapCamera->SetDirAndUp(A3DVECTOR3(0.0f, -1.0f, 0.0f), A3DVECTOR3(0.0f, 0.0f, 1.0f));
	matView = m_pLightMapCamera->GetViewTM();
	GetWorldBounds(m_aabbWorld, matView, l, r, b, t, n, f);
	m_pLightMapCamera->SetProjectionParam(l, r, b, t, n, f);
	m_pLightMapCamera->SetPos(m_vecWorldCenter);

	// update pixel render camera to see all terrain scene
	m_pPixelRenderCamera->SetProjectionParam(l, r, b, t, n, f);

	m_pA3DDevice->SetRenderTarget(m_pLightMapTarget);
	m_pLightMapViewport->Active();
	A3DCOLORVALUE clScaleAmbient(m_clAmbient);
	clScaleAmbient = clScaleAmbient * g_Configs.fSunFaction;
	m_pLightMapViewport->SetClearColor(a3d_ColorValueToColorRGBA(clScaleAmbient));
	m_pLightMapViewport->ClearDevice();

	m_pA3DDevice->SetTextureFilterType(0, A3DTEXF_LINEAR);
	m_matTexProjScale._43 = -20000.0f;
	A3DMATRIX4 matProjectedView = InverseTM(m_pLightMapCamera->GetViewTM()) * m_pLightShadowMapCamera->GetVPTM() * m_matTexProjScale;
	m_pA3DDevice->SetTextureCoordIndex(0, D3DTSS_TCI_CAMERASPACEPOSITION);
	m_pA3DDevice->SetTextureTransformFlags(0, (A3DTEXTURETRANSFLAGS)(A3DTTFF_COUNT4 | A3DTTFF_PROJECTED));
	m_pA3DDevice->SetTextureMatrix(0, matProjectedView);
	m_pA3DDevice->GetD3DDevice()->SetTexture(0, m_pLightShadowMapTarget->GetDepthTexture());

	if( m_bSoftware )
	{
		m_pShadowVertexShader->Appear();
		m_pShadowRenderShader->Appear();
		// set constants
		A3DVECTOR4 c0(1.0f, 0.0f, 0.0f, 1.0f);
		m_pA3DDevice->SetVertexShaderConstants(0, &c0, 1);

		A3DMATRIX4 matVP = m_pLightMapViewport->GetCamera()->GetVPTM();
		matVP.Transpose();
		m_pA3DDevice->SetVertexShaderConstants(1, &matVP, 4);

		A3DMATRIX4 matTexProjScale = m_matTexProjScale;
		matTexProjScale._33 = 1.0f;
		matTexProjScale._43 = -0.0004f;
		A3DMATRIX4 matLVP = m_pLightShadowMapCamera->GetVPTM() * matTexProjScale;
		matLVP.Transpose();
		m_pA3DDevice->SetVertexShaderConstants(5, &matLVP, 4);

		A3DVECTOR4 vecLightDir(Normalize(m_vecSunDir));
		m_pA3DDevice->SetVertexShaderConstants(9, &vecLightDir, 1);

		A3DCOLORVALUE clDiffuse = m_pLightSun->GetLightparam().Diffuse;
		m_pA3DDevice->SetVertexShaderConstants(10, &clDiffuse, 1);

		A3DCOLORVALUE clAmbient = m_clAmbient;
		m_pA3DDevice->SetVertexShaderConstants(11, &clAmbient, 1);
	}

	// turn off current light in the world
	g_Render.GetDirLight()->TurnOff();
	A3DCOLOR clAmbient = m_pA3DDevice->GetAmbientColor();
	
	// mask out the shadow part.
	m_pA3DDevice->SetAmbient(A3DCOLORRGB(255, 255, 255));
	if( m_bSoftware )
	{
		A3DCOLORVALUE clDiffuse(0.0f, 0.0f, 0.0f, 1.0f);
		m_pA3DDevice->SetVertexShaderConstants(10, &clDiffuse, 1);
		A3DCOLORVALUE clAmbient(1.0f, 1.0f, 1.0f, 1.0f);
		m_pA3DDevice->SetVertexShaderConstants(11, &clAmbient, 1);
	}

	m_pA3DDevice->SetSourceAlpha(A3DBLEND_ZERO);
	m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCCOLOR);

	m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, (m_nGridX + 3) * (m_nGridZ + 3),
		0, (m_nGridX + 2) * (m_nGridZ + 2) * 2);

	
	// now render the unshadow part with ambient color on
	m_pA3DDevice->SetAmbient(m_clAmbient);
	A3DLIGHTPARAM param		= m_pLightSun->GetLightparam();
	param.Direction			= m_vecSunDir;
	param.Diffuse			= m_clSun * m_vSunPower;
	m_pLightSun->SetLightParam(param);
	m_pLightSun->TurnOn();
	if( m_bSoftware )
	{
		A3DCOLORVALUE clDiffuse = param.Diffuse;
		m_pA3DDevice->SetVertexShaderConstants(10, &clDiffuse, 1);
		A3DCOLORVALUE clAmbient = m_clAmbient;
		m_pA3DDevice->SetVertexShaderConstants(11, &clAmbient, 1);
	}

	m_pA3DDevice->SetSourceAlpha(A3DBLEND_ONE);
	m_pA3DDevice->SetDestAlpha(A3DBLEND_ONE);

	m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, (m_nGridX + 3) * (m_nGridZ + 3),
		0, (m_nGridX + 2) * (m_nGridZ + 2) * 2);

	m_pLightSun->TurnOff();
	

	///////////////////
	D3DLOCKED_RECT rectLocked;
	int i, j;
	DWORD * pLines = NULL;
	A3DCOLORVALUE * pPixel = pColors;
	HRESULT hval;
	IDirect3DSurface8 * pTempDXSurface = NULL; 
	IDirect3DSurface8* pLightMapSurface = NULL;  
	
	/*
	pTempDXSurface = m_pTempSurface->GetDXSurface();
	pLightMapSurface = m_pLightMapTarget->GetTargetSurface();
	hval = m_pA3DDevice->GetD3DDevice()->CopyRects(pLightMapSurface, NULL, 0, pTempDXSurface, NULL);
	if( hval != D3D_OK )
	{
		g_Log.Log("--CLightProcessor::GenerateTerrainLightMaps(), copy rects failed!");
		return false;
	}
	hval = pTempDXSurface->LockRect(&rectLocked, NULL, 0);
	if( hval != D3D_OK )
	{
		g_Log.Log("--CLightProcessor::GenerateTerrainLightMaps(), Lock light map texture failed!");
		return false;
	}
	
	
	for(i=0; i<m_nLightMapBlockSize; i++)
	{
		pLines = (DWORD *)((BYTE*)rectLocked.pBits + i * rectLocked.Pitch);
		for(j=0; j<m_nLightMapBlockSize; j++)
		{
			A3DCOLOR color = pLines[j];
			int r = ((color & 0x00ff0000) >> 16);
			int g = ((color & 0x0000ff00) >> 8);
			int b = (color & 0x000000ff);

			if( i < 50 && j < 50)
			{
				r = int(r * 0.5f);
				g = int(g * 0.5f);
				b = int(b * 0.5f);
			}

			//pPixel ++;
			color = (int)r << 16 | (int)g << 8 | (int)b;
			pLines[j] = color;
		}
	}
	pTempDXSurface->UnlockRect();
	hval = m_pA3DDevice->GetD3DDevice()->CopyRects(pTempDXSurface, NULL, 0, pLightMapSurface, NULL);
	if( hval != D3D_OK )
	{
		g_Log.Log("--CLightProcessor::GenerateTerrainLightMaps(), copy rects failed1!");
		return false;
	}
	*/
	///////////////////


	if( m_bSoftware )
	{
		m_pA3DDevice->ClearVertexShader();
		m_pA3DDevice->ClearPixelShader();
		m_pTerrainStream->Appear();
	}

	m_pA3DDevice->SetAmbient(A3DCOLORRGB(0, 0, 0));
	// calculate scene light objects
	int nNumLights = 0;
	m_pA3DDevice->ClearTexture(0);
	APtrList<CSceneObject *>* listLight = m_pSceneObjectManager->GetSortObjectList(CSceneObject::SO_TYPE_LIGHT);
	ALISTPOSITION pos = listLight->GetTailPosition();
	while( pos )
	{
		CSceneObject* ptemp = listLight->GetPrev(pos);
		if( !ptemp->IsDeleted() )
		{
			CSceneLightObject * pLightObject = (CSceneLightObject *) ptemp;
			if(bNight && (pLightObject->m_Data.m_nEffectType == CSceneLightObject::LIGHT_EFFECT_NIGHT))
			{
				SetupSceneLight(nNumLights, pLightObject);
				nNumLights ++;
			}else if(!bNight && (pLightObject->m_Data.m_nEffectType == CSceneLightObject::LIGHT_EFFECT_DAY))
			{
				SetupSceneLight(nNumLights, pLightObject);
				nNumLights ++;
			}else if(pLightObject->m_Data.m_nEffectType == CSceneLightObject::LIGHT_EFFECT_BOTH)
			{
				SetupSceneLight(nNumLights, pLightObject);
				nNumLights ++;
			}
			if( nNumLights == 8 )
			{
				m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, (m_nGridX + 3) * (m_nGridZ + 3),
					0, (m_nGridX + 2) * (m_nGridZ + 2) * 2);

				nNumLights = 0;
				TurnOffAllLights();
			}
		}
	}

	if( nNumLights )
	{
		m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, (m_nGridX + 3) * (m_nGridZ + 3),
			0, (m_nGridX + 2) * (m_nGridZ + 2) * 2);

		nNumLights = 0;
		TurnOffAllLights();
	}

	m_pLightSun->TurnOn();

	// now output these colors into color array;
	pTempDXSurface = m_pTempSurface->GetDXSurface();
	pLightMapSurface = m_pLightMapTarget->GetTargetSurface();
	hval = m_pA3DDevice->GetD3DDevice()->CopyRects(pLightMapSurface, NULL, 0, pTempDXSurface, NULL);
	if( hval != D3D_OK )
	{
		g_Log.Log("CLightProcessor::GenerateTerrainLightMaps(), copy rects failed!");
		return false;
	}
	hval = pTempDXSurface->LockRect(&rectLocked, NULL, D3DLOCK_READONLY);
	if( hval != D3D_OK )
	{
		g_Log.Log("CLightProcessor::GenerateTerrainLightMaps(), Lock light map texture failed!");
		return false;
	}

	pLines = NULL;
	pPixel = pColors;
	for(i=0; i<m_nLightMapBlockSize; i++)
	{
		pLines = (DWORD *)((BYTE*)rectLocked.pBits + i * rectLocked.Pitch);
		for(j=0; j<m_nLightMapBlockSize; j++)
		{
			A3DCOLOR color = pLines[j];
			pPixel->r = ((color & 0x00ff0000) >> 16) / 255.0f;
			pPixel->g = ((color & 0x0000ff00) >> 8) / 255.0f;
			pPixel->b = (color & 0x000000ff) / 255.0f;

			pPixel ++;
		}
	}

	pTempDXSurface->UnlockRect();
	//Test time
	//m_dwTestTime = a_GetTime() - m_dwTestTime;
	// output a test file
	//D3DXSaveTextureToFile("lightmap.bmp", D3DXIFF_BMP, m_pLightMapTarget->GetTargetTexture(), NULL);

#ifndef _DEBUG
	GaussianBlurColorValue(pColors, m_nLightMapBlockSize, m_nLightMapBlockSize / 512.0f);
#endif

	// now we want to lit the static models in the scene
	if(!m_bOnlyRenderShadow) LitStaticModelInScene(bNight,pCurrentViewport);
	// now we want to lit the water objects in the scene
	if(!m_bOnlyRenderShadow) LitWaters(bNight);

	m_pA3DDevice->ClearTexture(0);
	m_pA3DDevice->SetTextureTransformFlags(0, A3DTTFF_DISABLE);
	m_pA3DDevice->SetTextureCoordIndex(0, 0);
	m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
	m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);

	m_pLightSun->TurnOff();
	m_pA3DDevice->SetAmbient(clAmbient);
	g_Render.GetDirLight()->TurnOn();

	m_pA3DDevice->RestoreRenderTarget();
	pCurrentViewport->Active();
	m_pA3DDevice->SetFogEnable(bFogEnable);
	return true;
}

bool CLightProcessor::LitStaticModelInScene(bool bNight, A3DViewport* pOldView)
{
	m_pA3DDevice->SetRenderTarget(m_pLightMapTargetForModel);
	m_pA3DDevice->SetLightingEnable(true);
	m_pA3DDevice->SetSpecularEnable(false);
	m_pA3DDevice->SetFogEnable(false);
	m_pA3DDevice->GetD3DDevice()->SetTexture(0, m_pLightShadowMapTarget->GetDepthTexture());
	m_pA3DDevice->SetWorldMatrix(IdentityMatrix());
	m_pA3DDevice->GetD3DDevice()->SetVertexShader(A3DFVF_A3DVERTEX);
	
	// first clear back to ambient color
	A3DCOLORVALUE clScaleAmbient(m_clAmbient);
	clScaleAmbient = clScaleAmbient * g_Configs.fSunFaction;
	m_pPixelForModelViewport->SetClearColor(a3d_ColorValueToColorRGBA(clScaleAmbient));
	m_pPixelForModelViewport->Active();
	m_pPixelForModelViewport->ClearDevice();

	int		nPixel = 0;
	// now we should light each static model using hardware shadows.
	APtrList<CSceneObject *>* listModel = m_pSceneObjectManager->GetSortObjectList(CSceneObject::SO_TYPE_STATIC_MODEL);
	APtrList<CSceneObject *> listUsed;
	APtrList<CSceneObject *> listLight;
	ALISTPOSITION pos = listModel->GetTailPosition();
	while( pos )
	{
		CSceneObject* ptemp = listModel->GetPrev(pos);
		if( !ptemp->IsDeleted() )
		{
			// Lit this model
			CStaticModelObject * pStaticModel = (CStaticModelObject *) ptemp;
			if(NULL == pStaticModel->GetA3DModel() || NULL == pStaticModel->GetA3DModel()->pModel) continue;
			A3DLitModel * pModel = (pStaticModel->GetA3DModel()->pModel)->GetA3DLitModel();
			
			if(m_bFast && pOldView)
				if(!pOldView->GetCamera()->AABBInViewFrustum(pStaticModel->GetMin(),pStaticModel->GetMax())) continue;

			// Clear lights and build lights
			listLight.RemoveAll();
			A3DAABB mab(ptemp->GetMin(),ptemp->GetMax());
			APtrList<CSceneObject *>* listTempLight = m_pSceneObjectManager->GetSortObjectList(CSceneObject::SO_TYPE_LIGHT);
			ALISTPOSITION lt_pos = listTempLight->GetTailPosition();
			while(lt_pos)
			{
				CSceneLightObject* plight = (CSceneLightObject*)listTempLight->GetPrev(lt_pos);
				LIGHTDATA ldata;
				plight->GetLightData(ldata);
				A3DAABB lab(plight->GetPos() - A3DVECTOR3(ldata.m_fRange),plight->GetPos() + A3DVECTOR3(ldata.m_fRange));
				if(CLS_AABBAABBOverlap(mab.Center,mab.Extents,lab.Center,lab.Extents) || ldata.m_nType != 0)
					listLight.AddHead(plight);
			}
			
			
			int nNumMeshes = pModel->GetNumMeshes();
			for(int idMesh=0; idMesh<nNumMeshes; idMesh++)
			{
				A3DLitMesh *	pMesh = pModel->GetMesh(idMesh);
				A3DLMVERTEX *	pVerts = pMesh->GetVerts();
				WORD *			pIndices = pMesh->GetIndices();	
				A3DVECTOR3 *	pNormals = pMesh->GetNormals();

				int nNumVerts = pMesh->GetNumVerts();
				int nNumFaces = pMesh->GetNumFaces();

				int		i;
				
				A3DMATRIX4 matAbsoluteTM = pModel->GetAbsoluteTM();
				A3DMaterial matWithEmissive = pMesh->GetMaterial();
				A3DMaterial matWithoutEmissive = pMesh->GetMaterial();
				matWithEmissive.SetEmissive(A3DCOLORVALUE(matWithEmissive.GetEmissive().r*0.5f, 
				matWithEmissive.GetEmissive().g*0.5f, matWithEmissive.GetEmissive().b*0.5f, 1.0f));
				matWithEmissive.SetDiffuse(A3DCOLORVALUE(0.0f, 0.0f, 0.0f, 1.0f));
				matWithEmissive.SetAmbient(A3DCOLORVALUE(0.0f, 0.0f, 0.0f, 1.0f));
				matWithoutEmissive.SetEmissive(A3DCOLORVALUE(0.0f, 0.0f, 0.0f, 1.0f));

				m_pA3DDevice->SetWorldMatrix(IdentityMatrix());
				for(i=0; i<nNumVerts; i++)
				{
					A3DVECTOR3 vecPos(pVerts[i].pos.x, pVerts[i].pos.y, pVerts[i].pos.z);
					vecPos = vecPos * matAbsoluteTM;
					A3DVECTOR3 vecNormal(pNormals[i].x, pNormals[i].y, pNormals[i].z);
					if(pMesh->GetMaterial().Is2Sided())
					{
						vecNormal = A3DVECTOR3(0.0f,1.0f,0.0f);
					}else
					{
						vecNormal = a3d_VectorMatrix3x3(vecNormal, matAbsoluteTM);
						vecNormal.Normalize();
					}
					
					A3DVERTEX vert;
					vert.x = vecPos.x;
					vert.y = vecPos.y;
					vert.z = vecPos.z;
					vert.nx = vecNormal.x;
					vert.ny = vecNormal.y;
					vert.nz = vecNormal.z;
					vert.tu = 0.0f;
					vert.tv = 0.0f;

					A3DVECTOR3 vecCamPos = vecPos;
					m_pPixelRenderCamera->SetPos(A3DVECTOR3(vecCamPos.x, vecCamPos.y + 1.0f, vecCamPos.z));
					m_pPixelRenderCamera->SetDirAndUp(A3DVECTOR3(0.0f, -1.0f, 0.0f), A3DVECTOR3(0.0f, 0.0f, 1.0f));
				
					// adjust viewport to let this vert appear at the prefered position on the target
					A3DVIEWPORTPARAM param = *m_pPixelRenderViewport->GetParam();
					param.X = nPixel % (m_nPixelForModelSize - 1);
					param.Y = nPixel / (m_nPixelForModelSize - 1);
					param.Width = 2;
					param.Height = 2;
					m_pPixelRenderViewport->SetParam(&param);
					m_pPixelRenderViewport->Active();

					m_matTexProjScale._43 = -10000.0f;
					A3DMATRIX4 matProjectedView = InverseTM(m_pPixelRenderCamera->GetViewTM()) * m_pLightShadowMapCamera->GetVPTM() * m_matTexProjScale;
					//m_pA3DDevice->SetTextureCoordIndex(0, D3DTSS_TCI_CAMERASPACEPOSITION);//Added by xqf
					//m_pA3DDevice->SetTextureTransformFlags(0, (A3DTEXTURETRANSFLAGS)(A3DTTFF_COUNT4 | A3DTTFF_PROJECTED));//Added by xqf
					m_pA3DDevice->SetTextureMatrix(0, matProjectedView);

					// calculate only direct light.
					matWithoutEmissive.Appear();
					m_pA3DDevice->GetD3DDevice()->SetTexture(0, m_pLightShadowMapTarget->GetDepthTexture());
					if( m_bSoftware )
					{
						m_pShadowVertexShader->Appear();
						m_pShadowRenderShader->Appear();
						// set constants
						A3DVECTOR4 c0(1.0f, 0.0f, 0.0f, 1.0f);
						m_pA3DDevice->SetVertexShaderConstants(0, &c0, 1);

						A3DMATRIX4 matVP = m_pPixelRenderCamera->GetVPTM();
						matVP.Transpose();
						m_pA3DDevice->SetVertexShaderConstants(1, &matVP, 4);

						A3DMATRIX4 matTexProjScale = m_matTexProjScale;
						matTexProjScale._33 = 1.0f;
						matTexProjScale._43 = -0.0004f;
						A3DMATRIX4 matLVP = m_pLightShadowMapCamera->GetVPTM() * matTexProjScale;
						matLVP.Transpose();
						m_pA3DDevice->SetVertexShaderConstants(5, &matLVP, 4);

						A3DVECTOR4 vecLightDir(Normalize(m_vecSunDir));
						m_pA3DDevice->SetVertexShaderConstants(9, &vecLightDir, 1);

						A3DCOLORVALUE clDiffuse = m_pLightSun->GetLightparam().Diffuse;
						m_pA3DDevice->SetVertexShaderConstants(10, &clDiffuse, 1);

						A3DCOLORVALUE clAmbient(0.0f, 0.0f, 0.0f, 1.0f);
						m_pA3DDevice->SetVertexShaderConstants(11, &clAmbient, 1);
					}
					m_pA3DDevice->DrawPrimitiveUP(A3DPT_POINTLIST, 1, &vert, sizeof(A3DVERTEX));
					m_pA3DDevice->ClearTexture(0);
					if( m_bSoftware )
					{
						m_pA3DDevice->ClearVertexShader();
						m_pA3DDevice->ClearPixelShader();
					}

					// calculate only emissive light, which will not consider shadow map
					matWithEmissive.Appear();
					m_pA3DDevice->DrawPrimitiveUP(A3DPT_POINTLIST, 1, &vert, sizeof(A3DVERTEX));
					
					matWithoutEmissive.Appear(); // set default material avoid emissive calculated mutli-times
					// let scene light take effects.
					m_pLightSun->TurnOff();
					int nNumLights = 0;
					m_pA3DDevice->ClearTexture(0);

					ALISTPOSITION posLight = listLight.GetTailPosition();
					while( posLight )
					{
						CSceneObject* ptemp = listLight.GetPrev(posLight);
						
						if( !ptemp->IsDeleted())
						{
							
							CSceneLightObject * pLightObject = (CSceneLightObject *) ptemp;
							
							if(bNight && (pLightObject->m_Data.m_nEffectType == CSceneLightObject::LIGHT_EFFECT_NIGHT))
							{
								SetupSceneLight(nNumLights, pLightObject);
								nNumLights ++;
							}else if(!bNight && (pLightObject->m_Data.m_nEffectType == CSceneLightObject::LIGHT_EFFECT_DAY))
							{
								SetupSceneLight(nNumLights, pLightObject);
								nNumLights ++;
							}else if(pLightObject->m_Data.m_nEffectType == CSceneLightObject::LIGHT_EFFECT_BOTH)
							{
								SetupSceneLight(nNumLights, pLightObject);
								nNumLights ++;
							}
							
							if( nNumLights == 8 )
							{
								m_pA3DDevice->DrawPrimitiveUP(A3DPT_POINTLIST, 1, &vert, sizeof(A3DVERTEX));

								nNumLights = 0;
								TurnOffAllLights();
							}
						}
					}
					if( nNumLights )
					{
						m_pA3DDevice->DrawPrimitiveUP(A3DPT_POINTLIST, 1, &vert, sizeof(A3DVERTEX));

						nNumLights = 0;
						TurnOffAllLights();
					}
					m_pLightSun->TurnOn();

					nPixel ++;
				}
			}
			listUsed.AddHead(ptemp);
		}
	
	}

	m_Material.Appear();
	// now output these colors into color array;
	HRESULT hval;
	IDirect3DSurface8 * pTempDXSurface = m_pTempSurfaceForModel->GetDXSurface();
	IDirect3DSurface8 * pLightMapSurface = m_pLightMapTargetForModel->GetTargetSurface();
	hval = m_pA3DDevice->GetD3DDevice()->CopyRects(pLightMapSurface, NULL, 0, pTempDXSurface, NULL);
	if( hval != D3D_OK )
	{
		g_Log.Log("CLightProcessor::GenerateTerrainLightMaps(), copy rects failed!");
		return false;
	}

	D3DLOCKED_RECT rectLocked;
	hval = pTempDXSurface->LockRect(&rectLocked, NULL, D3DLOCK_READONLY);
	if( hval != D3D_OK )
	{
		g_Log.Log("CLightProcessor::GenerateTerrainLightMaps(), Lock light map texture failed!");
		return false;
	}

	nPixel = 0;
	// now we should get each pixel's color from the shadow map target
	pos = listUsed.GetTailPosition();
	while( pos )
	{
		CSceneObject* ptemp = listUsed.GetPrev(pos);
		if( !ptemp->IsDeleted() )
		{
			// Lit this model
			 CStaticModelObject * pStaticModel = (CStaticModelObject *) ptemp;
			 if(NULL == pStaticModel->GetA3DModel() || NULL == pStaticModel->GetA3DModel()->pModel) continue;
			A3DLitModel * pModel = pStaticModel->GetA3DModel()->pModel->GetA3DLitModel();
			int nNumMeshes = pModel->GetNumMeshes();
			for(int idMesh=0; idMesh<nNumMeshes; idMesh++)
			{
				A3DLitMesh *	pMesh = pModel->GetMesh(idMesh);
				A3DCOLOR *		pColors = NULL;
				
				if( bNight )
					pColors = pMesh->GetNightColors();
				else
					pColors = pMesh->GetDayColors();
				
				int nNumVerts = pMesh->GetNumVerts();
				
				int		i;
				for(i=0; i<nNumVerts; i++)
				{
					int x = nPixel % (m_nPixelForModelSize - 1) + 1;
					int y = nPixel / (m_nPixelForModelSize - 1) + 1;
					DWORD * pLineColor = (DWORD *)((BYTE*)rectLocked.pBits + y * rectLocked.Pitch);

					pColors[i] = 0xff000000 | pLineColor[x];

					nPixel ++;
				}

				pMesh->UpdateColors();
			}
		}
	}

	pTempDXSurface->UnlockRect();

	// output a test file
	//D3DXSaveTextureToFile("vertsmap.bmp", D3DXIFF_BMP, m_pLightMapTargetForModel->GetTargetTexture(), NULL);
	return true;
}

bool CLightProcessor::LitWaters(bool bNight)
{
	m_pA3DDevice->SetLightingEnable(true);
	m_pA3DDevice->SetSpecularEnable(false);
	m_pA3DDevice->SetFogEnable(false);
	m_pA3DDevice->SetWorldMatrix(IdentityMatrix());
	m_pA3DDevice->GetD3DDevice()->SetVertexShader(A3DFVF_A3DVERTEX);

	int		idArea, idWave, i;

	// first clear back to ambient color
	m_pLightMapViewport->Active();
	m_pLightMapViewport->ClearDevice();

	int		nPixel = 0;
	int nNumWaterAreas = m_pTerrainWater->GetNumWaterAreas();

	for(idArea=0; idArea<nNumWaterAreas; idArea++)
	{
		A3DWaterArea * pArea = m_pTerrainWater->GetWaterArea(idArea);

		int nNumWaves = pArea->GetWaveCount();
		for(idWave=0; idWave<nNumWaves; idWave++)
		{
			A3DWaterWave * pWave = pArea->GetWave(idWave);

			int nNumVerts = pWave->GetVertCount();
			A3DWAVEVERTEX * pVerts = pWave->GetVerts();

			for(i=0; i<nNumVerts; i++)
			{
				A3DVERTEX vert;
				vert.x = pVerts[i].pos.x;
				vert.y = pVerts[i].pos.y;
				vert.z = pVerts[i].pos.z;
				vert.nx = 0.0f;
				vert.ny = 1.0f;
				vert.nz = 0.0f;
				vert.tu = 0.0f;
				vert.tv = 0.0f;

				A3DVECTOR3 vecCamPos = A3DVECTOR3(vert.x, vert.y, vert.z);
				m_pPixelRenderCamera->SetPos(A3DVECTOR3(vecCamPos.x, vecCamPos.y + 1.0f, vecCamPos.z));
				m_pPixelRenderCamera->SetDirAndUp(A3DVECTOR3(0.0f, -1.0f, 0.0f), A3DVECTOR3(0.0f, 0.0f, 1.0f));
				
				// adjust viewport to let this vert appear at the prefered position on the target
				A3DVIEWPORTPARAM param = *m_pPixelRenderViewport->GetParam();
				param.X = nPixel % (m_nLightMapBlockSize - 1);
				param.Y = nPixel / (m_nLightMapBlockSize - 1);
				param.Width = 2;
				param.Height = 2;
				m_pPixelRenderViewport->SetParam(&param);
				m_pPixelRenderViewport->Active();

				m_matTexProjScale._43 = -20000.0f;
				A3DMATRIX4 matProjectedView = InverseTM(m_pPixelRenderCamera->GetViewTM()) * m_pLightShadowMapCamera->GetVPTM() * m_matTexProjScale;
				//m_pA3DDevice->SetTextureCoordIndex(0, D3DTSS_TCI_CAMERASPACEPOSITION);//Added by xqf
				//m_pA3DDevice->SetTextureTransformFlags(0, (A3DTEXTURETRANSFLAGS)(A3DTTFF_COUNT4 | A3DTTFF_PROJECTED));//Added by xqf
				m_pA3DDevice->SetTextureMatrix(0, matProjectedView);

				//m_pA3DDevice->ClearTexture(0);
				m_pA3DDevice->GetD3DDevice()->SetTexture(0, m_pLightShadowMapTarget->GetDepthTexture());
				if( m_bSoftware )
				{
					m_pShadowVertexShader->Appear();
					m_pShadowRenderShader->Appear();
					// set constants
					A3DVECTOR4 c0(1.0f, 0.0f, 0.0f, 1.0f);
					m_pA3DDevice->SetVertexShaderConstants(0, &c0, 1);

					A3DMATRIX4 matVP = m_pPixelRenderCamera->GetVPTM();
					matVP.Transpose();
					m_pA3DDevice->SetVertexShaderConstants(1, &matVP, 4);

					A3DMATRIX4 matTexProjScale = m_matTexProjScale;
					matTexProjScale._33 = 1.0f;
					matTexProjScale._43 = -0.0004f;
					A3DMATRIX4 matLVP = m_pLightShadowMapCamera->GetVPTM() * matTexProjScale;
					matLVP.Transpose();
					m_pA3DDevice->SetVertexShaderConstants(5, &matLVP, 4);

					A3DVECTOR4 vecLightDir(Normalize(m_vecSunDir));
					m_pA3DDevice->SetVertexShaderConstants(9, &vecLightDir, 1);

					A3DCOLORVALUE clDiffuse = m_pLightSun->GetLightparam().Diffuse;
					m_pA3DDevice->SetVertexShaderConstants(10, &clDiffuse, 1);

					A3DCOLORVALUE clAmbient(0.0f, 0.0f, 0.0f, 1.0f);
					m_pA3DDevice->SetVertexShaderConstants(11, &clAmbient, 1);
				}
				m_pA3DDevice->DrawPrimitiveUP(A3DPT_POINTLIST, 1, &vert, sizeof(A3DVERTEX));
				m_pA3DDevice->ClearTexture(0);
				if( m_bSoftware )
				{
					m_pA3DDevice->ClearVertexShader();
					m_pA3DDevice->ClearPixelShader();
				}

				// let scene light take effects.
				m_pLightSun->TurnOff();
				int nNumLights = 0;
				m_pA3DDevice->ClearTexture(0);
				APtrList<CSceneObject *>* listLight = m_pSceneObjectManager->GetSortObjectList(CSceneObject::SO_TYPE_LIGHT);
				ALISTPOSITION posLight = listLight->GetTailPosition();
				while( posLight )
				{
					CSceneObject* ptemp = listLight->GetPrev(posLight);
					if( !ptemp->IsDeleted() )
					{
						CSceneLightObject * pLightObject = (CSceneLightObject *) ptemp;
						if(bNight && (pLightObject->m_Data.m_nEffectType == CSceneLightObject::LIGHT_EFFECT_NIGHT))
						{
							SetupSceneLight(nNumLights, pLightObject);
							nNumLights ++;
						}else if(!bNight && (pLightObject->m_Data.m_nEffectType == CSceneLightObject::LIGHT_EFFECT_DAY))
						{
							SetupSceneLight(nNumLights, pLightObject);
							nNumLights ++;
						}else if(pLightObject->m_Data.m_nEffectType == CSceneLightObject::LIGHT_EFFECT_BOTH)
						{
							SetupSceneLight(nNumLights, pLightObject);
							nNumLights ++;
						}
						if( nNumLights == 8 )
						{
							m_pA3DDevice->DrawPrimitiveUP(A3DPT_POINTLIST, 1, &vert, sizeof(A3DVERTEX));

							nNumLights = 0;
							TurnOffAllLights();
						}
					}
				}
				if( nNumLights )
				{
					m_pA3DDevice->DrawPrimitiveUP(A3DPT_POINTLIST, 1, &vert, sizeof(A3DVERTEX));

					nNumLights = 0;
					TurnOffAllLights();
				}
				m_pLightSun->TurnOn();

				nPixel ++;
			}
		}
	}

	// now output these colors into color array;
	HRESULT hval;
	IDirect3DSurface8 * pTempDXSurface = m_pTempSurface->GetDXSurface();
	IDirect3DSurface8 * pLightMapSurface = m_pLightMapTarget->GetTargetSurface();
	hval = m_pA3DDevice->GetD3DDevice()->CopyRects(pLightMapSurface, NULL, 0, pTempDXSurface, NULL);
	if( hval != D3D_OK )
	{
		g_Log.Log("CLightProcessor::GenerateTerrainLightMaps(), copy rects failed!");
		return false;
	}

	D3DLOCKED_RECT rectLocked;
	hval = pTempDXSurface->LockRect(&rectLocked, NULL, D3DLOCK_READONLY);
	if( hval != D3D_OK )
	{
		g_Log.Log("CLightProcessor::GenerateTerrainLightMaps(), Lock light map texture failed!");
		return false;
	}

	nPixel = 0;
	// now we should light each static model using hardware shadows.
	for(idArea=0; idArea<nNumWaterAreas; idArea++)
	{
		A3DWaterArea * pArea = m_pTerrainWater->GetWaterArea(idArea);

		int nNumWaves = pArea->GetWaveCount();
		for(idWave=0; idWave<nNumWaves; idWave++)
		{
			A3DWaterWave * pWave = pArea->GetWave(idWave);

			int nNumVerts = pWave->GetVertCount();
			A3DCOLOR * pColors = NULL;
			if( bNight )
				pColors = pWave->GetNightColors();
			else
				pColors = pWave->GetDayColors();

			for(i=0; i<nNumVerts; i++)
			{
				int x = nPixel % (m_nLightMapBlockSize - 1) + 1;
				int y = nPixel / (m_nLightMapBlockSize - 1) + 1;
				DWORD * pLineColor = (DWORD *)((BYTE*)rectLocked.pBits + y * rectLocked.Pitch);

				pColors[i] = 0xff000000 | pLineColor[x];
				
				nPixel ++;
			}

			pWave->UpdateColors();
		}
	}

	pTempDXSurface->UnlockRect();

	// output a test file
	//D3DXSaveTextureToFile("vertsmap2.bmp", D3DXIFF_BMP, m_pLightMapTarget->GetTargetTexture(), NULL);
	return true;
}

bool CLightProcessor::GetWorldBounds(const A3DAABB& aabb, const A3DMATRIX4& viewTM, float& l, float& r, float& b, float& t, float& n, float& f)
{
	A3DVECTOR3 verts[8];
	A3DVECTOR3 vecX = A3DVECTOR3(2 * aabb.Extents.x, 0.0f, 0.0f);
	A3DVECTOR3 vecY = A3DVECTOR3(0.0f, 2 * aabb.Extents.y, 0.0f);
	A3DVECTOR3 vecZ = A3DVECTOR3(0.0f, 0.0f, 2 * aabb.Extents.z);

	verts[0] = aabb.Mins;
	verts[1] = aabb.Mins + vecX;
	verts[2] = aabb.Mins + vecY;
	verts[3] = aabb.Mins + vecZ;
	verts[4] = aabb.Maxs;
	verts[5] = aabb.Maxs - vecX;
	verts[6] = aabb.Maxs - vecY;
	verts[7] = aabb.Maxs - vecZ;

	A3DAABB aabbWorld;
	aabbWorld.Clear();

	A3DVECTOR3 vert;
	int i;
	for(i=0; i<8; i++)
	{
		vert = verts[i]	* viewTM;
		aabbWorld.AddVertex(vert);
	}

	l = aabbWorld.Mins.x;
	r = aabbWorld.Maxs.x;
	b = aabbWorld.Mins.y;
	t = aabbWorld.Maxs.y;
	n = aabbWorld.Mins.z;
	f = aabbWorld.Maxs.z;

	return true;
}



int CLightProcessor::DisplayCardInfo()
{
	HKEY         keyServ   =   NULL;   
	HKEY         keyEnum   =   NULL;   
	HKEY         key           =   NULL;   
	HKEY         key2         =   NULL;   
	DWORD       error       =   0;   
	LPCTSTR   api           =   NULL;   
	DWORD       type,   count,   size,   i,   iServ;   
	std::list   <AString>   save;   
    
	setlocale   (LC_ALL,   ".ACP");   
    
	error   =   RegOpenKeyEx   (HKEY_LOCAL_MACHINE,     
		TEXT   ("SYSTEM\\CurrentControlSet\\Services"),     
		0,   KEY_READ,   &keyServ);   
    
	//CHECK_ERROR   ("RegOpenKeyEx");   
    
	error   =   RegOpenKeyEx   (HKEY_LOCAL_MACHINE,     
		TEXT   ("SYSTEM\\CurrentControlSet\\Enum"),     
		0,   KEY_READ,   &keyEnum);   
    
	//CHECK_ERROR   ("RegOpenKeyEx");   
    
	i   =   0;   
	for   (iServ=0;;++iServ)   
	{   
		TCHAR   name   [0x200];   
		size   =   0x200;   
		error   =   RegEnumKeyEx   (keyServ,   iServ,   name,   &size,     
			NULL,   NULL,   NULL,   NULL);   
		
		if   (error   ==   ERROR_NO_MORE_ITEMS)   
			break;   
		
		//CHECK_ERROR   ("RegEnumKeyEx");   
		
		error   =   RegOpenKeyEx   (keyServ,   name,   0,   KEY_READ,   &key);   
		
		//CHECK_ERROR   ("RegOpenKeyEx");   
		
		size   =   0x200;   
		error   =   RegQueryValueEx   (key,   TEXT   ("Group"),     
			0,   &type,   (LPBYTE)name,   &size);   
		
		if   (error   ==   ERROR_FILE_NOT_FOUND)   
		{       //     键不存在   
			RegCloseKey   (key);   
			continue;   
		};   
		
		//CHECK_ERROR   ("RegQueryValueEx");   
		
		if   (_tcscmp   (TEXT   ("Video"),   name)   !=   0)   
		{       //     不是显卡的内核驱动   
			RegCloseKey   (key);   
			continue;   
		};   
		
		error   =   RegOpenKeyEx   (key,   TEXT   ("Enum"),     
			0,   KEY_READ,   &key2);   
		
		//CHECK_ERROR   ("RegOpenKeyEx");   
		RegCloseKey   (key);   
		key   =   key2;   
		count = 0;
		size   =   sizeof   (count);   
		error   =   RegQueryValueEx   (key,   TEXT   ("Count"),     
			0,   &type,   (LPBYTE)&count,   &size);   
		
		//CHECK_ERROR   ("RegQueryValueEx");   
		
		
		for   (i=0;i<count;++i)   
		{   
			TCHAR   sz   [0x200];   
			TCHAR   name   [0x40];   
			
			wsprintf   (name,   TEXT   ("%d"),   i);   
			size   =   sizeof   (sz);   
			error   =   RegQueryValueEx   (key,   name,     
				0,   &type,   (LPBYTE)sz,   &size);   
			
			//CHECK_ERROR   ("RegQueryValueEx");   
			
			error   =   RegOpenKeyEx   (keyEnum,   sz,     
				0,   KEY_READ,   &key2);   
			
			//CHECK_ERROR   ("RegOpenKeyEx");   
			
			size   =   sizeof   (sz);   
			error   =   RegQueryValueEx   (key2,   TEXT   ("FriendlyName"),     
				0,   &type,   (LPBYTE)sz,   &size);   
			
			if   (error   ==   ERROR_FILE_NOT_FOUND)   
			{   
				size   =   sizeof   (sz);   
				error   =   RegQueryValueEx   (key2,   TEXT   ("DeviceDesc"),     
					0,   &type,   (LPBYTE)sz,   &size);   
			};   
			//CHECK_ERROR   ("RegQueryValueEx");   
			
			save.push_back   (sz);   
			RegCloseKey   (key2);   
			key2   =   NULL;   
		};   
		RegCloseKey   (key);   
		key   =   NULL;   
	};   
    
	RegCloseKey   (keyEnum);   
	RegCloseKey   (keyServ);   
    
	
	std::list   <AString>::iterator   it,   end;   
	it     =   save.begin   ();   
	end   =   save.end   ();   
	
	for   (i=0;it!=end;++it)   
	{
		AString info = *it;
		if(info.Find("GeForce 6") != -1) return 0;
		if(info.Find("GeForce 7") != -1) return 0;

		if(info.Find("GeForce 8") != -1) return 1;
		if(info.Find("GeForce 9") != -1) return 1;
		if(info.Find("GeForce GT") != -1) return 1;
		if(info.Find("GeForce GTX") != -1) return 1;
	}
	
	return   -1;   
}

static int build_gaussian(float *output, double size)
{
    double	sigma2;
    double	l;
    int		length, n, i;

    sigma2 = -(size + 1) * (size + 1) / log(1.0 / 255.0);
    l = size;

    n = int(ceil(l) * 2.0);
    if( (n % 2) == 0 ) n++;
	length = n / 2;

    for(i=0; i<=length; i++)
	    output[i] = (float)exp(-(i * i) / sigma2);
    
    return length;
}

static void GaussianBlurColorValue(A3DCOLORVALUE * pImage, int nSize, double vBlurRange)
{
	A3DCOLORVALUE * pLine = new A3DCOLORVALUE[nSize];

	float gaussian[32]; // max blur radius is 32 pixel

	// first setup blur factors
	int		gwidth, kwidth;
	gwidth = build_gaussian(gaussian, vBlurRange);
    kwidth = gwidth * 2 + 1;

	int i, j, k;
	float total = 0.0f;
	// now normalize these factors
	for(i=0; i<kwidth; i++) 
	{
        int xpos = abs(i - gwidth);
        total += gaussian[xpos];
    }

	for(i=0; i<=gwidth; i++)
		gaussian[i] /= total;
	
	A3DCOLORVALUE	result;
	int				index;
	A3DCOLORVALUE	value;

	int index1, index2;

	// first do horizontal blur and put into a temp line buffer first, then copy it to result
	for(i=0; i<nSize; i++) 
	{
		for(j=0; j<gwidth; j++)
		{
			result.Set(0.0f, 0.0f, 0.0f, 0.0f);
			index = i * nSize + j - gwidth;
			for(k=0; k<kwidth; k++)
			{
				if( j + k - gwidth < 0 )
					value = pImage[i * nSize];
				else
					value = pImage[index];
				
				int xpos = abs(k - gwidth);
				result += value * gaussian[xpos];
                index ++;
            }
			pLine[j] = result;
		}
		for(j=gwidth; j<nSize-gwidth; j++)
		{
			result.Set(0.0f, 0.0f, 0.0f, 0.0f);
			index = i * nSize + j - gwidth;
			for(k=0; k<kwidth; k++)
			{
				value = pImage[index];
				
				int xpos = abs(k - gwidth);
				result += value * gaussian[xpos];
                index ++;
            }
			pLine[j] = result;
        }
		for(j=nSize-gwidth; j<nSize; j++)
		{
			result.Set(0.0f, 0.0f, 0.0f, 0.0f);
			index = i * nSize + j - gwidth;
			for(k=0; k<kwidth; k++)
			{
				if( j + k - gwidth >= nSize )
					value = pImage[i * nSize + nSize - 1];
				else
					value = pImage[index];
				
				int xpos = abs(k - gwidth);
				result += value * gaussian[xpos];
                index ++;
            }
			pLine[j] = result;
		}
		memcpy(pImage + i * nSize, pLine, nSize * sizeof(A3DCOLORVALUE));
    }

	// then transpose the image to make cache works
	for(i=0; i<nSize; i++)
	{
		index1 = i * nSize;
		index2 = i;
		for(j=0; j<i; j++)
		{
			A3DCOLORVALUE temp = pImage[index1];
			pImage[index1] = pImage[index2];
			pImage[index2] = temp;
			index1 ++;
			index2 += nSize;
		}
	}

	// then do blur horizontal again.
	for(i=0; i<nSize; i++) 
	{
		for(j=0; j<gwidth; j++)
		{
			result.Set(0.0f, 0.0f, 0.0f, 0.0f);
			index = i * nSize + j - gwidth;
			for(k=0; k<kwidth; k++)
			{
				if( j + k - gwidth < 0 )
					value = pImage[i * nSize];
				else
					value = pImage[index];
				
				int xpos = abs(k - gwidth);
				result += value * gaussian[xpos];
                index ++;
            }
			pLine[j] = result;
		}
		for(j=gwidth; j<nSize-gwidth; j++)
		{
			result.Set(0.0f, 0.0f, 0.0f, 0.0f);
			index = i * nSize + j - gwidth;
			for(k=0; k<kwidth; k++)
			{
				value = pImage[index];
				
				int xpos = abs(k - gwidth);
				result += value * gaussian[xpos];
                index ++;
            }
			pLine[j] = result;
        }
		for(j=nSize-gwidth; j<nSize; j++)
		{
			result.Set(0.0f, 0.0f, 0.0f, 0.0f);
			index = i * nSize + j - gwidth;
			for(k=0; k<kwidth; k++)
			{
				if( j + k - gwidth >= nSize )
					value = pImage[i * nSize + nSize - 1];
				else
					value = pImage[index];
				
				int xpos = abs(k - gwidth);
				result += value * gaussian[xpos];
                index ++;
            }
			pLine[j] = result;
		}
		memcpy(pImage + i * nSize, pLine, nSize * sizeof(A3DCOLORVALUE));
    }

	// then transpose the image back
	for(i=0; i<nSize; i++)
	{
		index1 = i * nSize;
		index2 = i;
		for(j=0; j<i; j++)
		{
			A3DCOLORVALUE temp = pImage[index1];
			pImage[index1] = pImage[index2];
			pImage[index2] = temp;
			index1 ++;
			index2 += nSize;
		}
	}

	delete [] pLine;
}