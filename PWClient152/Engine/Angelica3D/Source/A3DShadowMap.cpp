/*
 * FILE: A3DShadowMap.cpp
 *
 * DESCRIPTION: shadow map
 *
 * CREATED BY: Liyi, 2010/4/6
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */
#include "A3DShadowMap.h"

#include "A3DDevice.h"
#include "A3DRenderTarget.h"
#include "A3DOrthoCamera.h"
#include "A3DViewport.h"
#include "A3DMacros.h"
#include "A3DPI.h"
#include "A3DTypes.h"
#include "A3DTexture.h"
#include "A3DSurface.h"
#include "A3DFuncs.h"
#include "A3DHLSL.h"
#include "A3DStream.h"
#include "A3DVertexShader.h"
#include "a3drendertargetman.h"
#include "A3DDeviceCaps.h"

#define FLT_AS_DW(F) (*(DWORD*)&(F))
#define ALMOST_ZERO(F) ((FLT_AS_DW(F) & 0x7f800000L)==0)

//#define _DEBUG_SHADOW_MAP

A3DShadowMap::A3DShadowMap()
:m_pShadowMap(NULL),
m_pShadowCamera(NULL),
m_pShadowViewport(NULL),
m_nShadowMapSize(2048),
m_pA3DDevice(NULL),
m_bInit(false),
m_bOptimize(false),
m_bTerrainShadowOnePass(false),
m_pShadowTexture(NULL),
m_pShadowTextureCamera(NULL),
m_pTerrainShadowMapL8(NULL),
m_pTerrainShadowViewport(NULL),
m_pTerrainShadowCamera(NULL),
m_nTerrainShadowMapDivision(2),
m_nTerrainShadowMapTileCount(4),
m_nTerrainShadowMapCurTile(-1),
m_nTerrainShadowMapTileSize(512),
m_bUseTerrain(false),
m_fTSMDelta(0.3f),  //默认是0.2
m_bUseHWPCF(false),
m_bUseVSM(false),
m_bUseFloatTexture(false),
m_bEnableTSM(false),
m_pBlurShadowMapVS(NULL),
m_pBlurShadowMapXPS(NULL),
m_pBlurShadowMapYPS(NULL),
m_pStream(NULL),
m_pStreamDecl(NULL),
m_nSampleLevel(1),
m_fShadowRadius(1.0f)
{
	memset(m_ppShadowCaches, 0, sizeof(A3DSurface*) * SHADOW_CACHE_NUM);
}

A3DShadowMap::~A3DShadowMap()
{
}

bool A3DShadowMap::Init(A3DDevice* pA3DDevice, int nShadowMapSize, bool bOptimize, 
						bool bUseFloatTexture, bool bUseTerrain, bool bUseHWPCF, bool bUseVSM)
{
	m_pA3DDevice = pA3DDevice;
	m_nShadowMapSize = nShadowMapSize;
	m_bOptimize = bOptimize;
	m_bUseFloatTexture = bUseFloatTexture;
	m_bUseHWPCF = bUseHWPCF;
	m_bUseVSM = bUseVSM;
	
	Release();

	if(!CreateShadowMapRT())
		goto FAILED;

	//Create Viewport
	if( !m_pA3DDevice->CreateViewport(&m_pShadowViewport, 0, 0, m_nShadowMapSize, m_nShadowMapSize, 
		0.0f, 1.0f, true, true, 0xFF000000) )
	{
		g_A3DErrLog.Log("A3DShadowMap::Init(), Failed to create shadow map viewport!");
		goto FAILED;
	}
	
	//Create shadow camera
	m_pShadowCamera = new A3DOrthoCamera();
	if( !m_pShadowCamera->Init(m_pA3DDevice, -1.0f, 1.0f, -1.0f, 1.0f, 0.2f, 2000.0f))
	{
		g_A3DErrLog.Log("A3DShadowMap::Init(), Failed to create shadow map camera!");
		goto FAILED;
	}
	m_pShadowViewport->SetCamera(m_pShadowCamera);
	
	m_bUseTerrain = bUseTerrain;
	if(bUseTerrain)
	{
		HRESULT hr;
		A3DRenderTarget::RTFMT fmt;

	// many cards do not support l8, so change to r5g6b5
		hr = m_pA3DDevice->GetA3DEngine()->GetD3D()->CheckDeviceFormat(
			D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL,
			(D3DFORMAT)m_pA3DDevice->GetDevFormat().fmtAdapter,
			D3DUSAGE_RENDERTARGET,
			D3DRTYPE_TEXTURE,
			D3DFMT_R5G6B5);

		if (SUCCEEDED(hr))
		{
			int nSize = GetShadowMapSize();
			m_pTerrainShadowMapL8 = new A3DRenderTarget();
			fmt.iWidth = nSize;
			fmt.iHeight = nSize;
			fmt.fmtTarget = A3DFMT_R5G6B5;
			fmt.fmtDepth = A3DFMT_D24X8;

			if (!m_pTerrainShadowMapL8->Init(m_pA3DDevice, fmt, true, false))
			{
				g_A3DErrLog.Log("A3DShadowMap::Init(), Failed to create terrain shadow map rendertarget!");
				goto FAILED;
			}

			// Create Viewport
			if (!m_pA3DDevice->CreateViewport(&m_pTerrainShadowViewport, 0, 0, nSize, nSize,
				0.0f, 1.0f, true, true, 0xFF000000))
			{
				g_A3DErrLog.Log("A3DShadowMap::Init(), Failed to create terrain shadow map viewport!");
				goto FAILED;
			}
			
			// Create shadow camera
			m_pTerrainShadowCamera = new A3DOrthoCamera();

			if (!m_pTerrainShadowCamera->Init(m_pA3DDevice, -1.0f, 1.0f, -1.0f, 1.0f, 0.2f, 2000.0f))
			{
				g_A3DErrLog.Log("A3DShadowMap::Init(), Failed to create terrain shadow map camera!");
				goto FAILED;
			}

			m_pTerrainShadowViewport->SetCamera(m_pTerrainShadowCamera);
			m_nTerrainShadowMapTileSize = nSize / m_nTerrainShadowMapDivision;
			m_bTerrainShadowOnePass = true;
			g_A3DErrLog.Log("A3DShadowMap::Init(), Chosen rendering terrain shadow in one pass");
		}

		
	}
	//create optimize resource
	if( bOptimize)
	{
		if(!InitOptimize())
			goto FAILED;
	}

	if(bUseVSM)
	{
		//VSM Blur Shader
		A3DHLSLMan* pHLSLMan = m_pA3DDevice->GetA3DEngine()->GetA3DHLSLMan();
		m_pBlurShadowMapVS = pHLSLMan->LoadShader("shaders\\2.2\\HLSL\\Shadow\\Shadow_BlurVS.hlsl", NULL, NULL);
		m_pBlurShadowMapXPS = pHLSLMan->LoadShader(NULL, "shaders\\2.2\\HLSL\\Shadow\\Shadow_BlurXPS.hlsl", NULL);
		m_pBlurShadowMapYPS = pHLSLMan->LoadShader(NULL, "shaders\\2.2\\HLSL\\Shadow\\Shadow_BlurYPS.hlsl", NULL);
		if(!m_pBlurShadowMapVS || !m_pBlurShadowMapXPS || !m_pBlurShadowMapYPS)
		{
			g_A3DErrLog.Log("A3DShadowMap::Init(), Can not load vsm blur shader!");
			goto FAILED;
		}

		m_aVertices[0].Set(A3DVECTOR3(-1, -1, 0), 0xffffffff, 0, 0, 1, 0, 1);
		m_aVertices[1].Set(A3DVECTOR3( 1, -1, 0), 0xffffffff, 0, 1, 1, 1, 1);
		m_aVertices[2].Set(A3DVECTOR3(-1,  1, 0), 0xffffffff, 0, 0, 0, 0, 0);
		m_aVertices[3].Set(A3DVECTOR3( 1,  1, 0), 0xffffffff, 0, 1, 0, 1, 0);

		m_aVertices[4] = m_aVertices[0];
		m_aVertices[5] = m_aVertices[1];
		m_aVertices[6] = m_aVertices[2];
		m_aVertices[7] = m_aVertices[3];
		static WORD aIndices[] = {0, 2, 1, 1, 2, 3};

		m_pStream = new A3DStream;
		if(!m_pStream->Init(m_pA3DDevice, A3DVT_IBLVERTEX, 8, 6, NULL, NULL))
			return false;

		m_pStream->SetVerts((BYTE*)m_aVertices, 8);
		m_pStream->SetIndices(aIndices, 6);

		D3DVERTEXELEMENT9 aDecl[MAX_FVF_DECL_SIZE];
		a3d_MakeVSDeclarator(A3DVT_IBLVERTEX, 0, aDecl);
		m_pStreamDecl = new A3DVertexDecl();
		m_pStreamDecl->Init(m_pA3DDevice, aDecl);
	}


	m_bInit = true;

	return true;

FAILED:
	Release();
	m_bInit = false;
	return false;
}

bool A3DShadowMap::InitOptimize()
{
	//create shadow caches
	for(int i = 0; i < SHADOW_CACHE_NUM; i++)
	{
		m_ppShadowCaches[i] = new A3DSurface;
		if( !m_ppShadowCaches[i]->Create(m_pA3DDevice, m_nShadowMapSize, m_nShadowMapSize, A3DFMT_A8R8G8B8))
		{
			g_A3DErrLog.Log("A3DShadowMap::InitOptimize(), Failed to create shadow map caches!");
			return false;
		}
	}

	//create shadow texture
	m_pShadowTexture = new A3DTexture;
	if(!m_pShadowTexture->Create(m_pA3DDevice, m_nShadowMapSize * SHADOW_CACHE_COL, m_nShadowMapSize * SHADOW_CACHE_ROW,  A3DFMT_A8R8G8B8, 1))
	{
		g_A3DErrLog.Log("A3DShadowMap::c(), Failed to create shadow texture!");
		return false;
	}

		//Create shadow  texture camera
	m_pShadowTextureCamera = new A3DOrthoCamera();
	if( !m_pShadowTextureCamera->Init(m_pA3DDevice, -1.0f, 1.0f, -1.0f, 1.0f, 0.2f, 2000.0f))
	{
		g_A3DErrLog.Log("A3DShadowMap::m_pShadowTextureCamera(), Failed to create shadow texture camera!");
		return false;
	}

	return true;
}

void A3DShadowMap::Release()
{
	A3DRELEASE(m_pShadowMap);
	A3DRELEASE(m_pShadowCamera);
	A3DRELEASE(m_pShadowViewport);

	A3DRELEASE(m_pTerrainShadowMapL8);
	A3DRELEASE(m_pTerrainShadowCamera);
	A3DRELEASE(m_pTerrainShadowViewport);

	for( int i = 0; i < SHADOW_CACHE_NUM; i++)
	{
		A3DRELEASE(m_ppShadowCaches[i]);
	}
	A3DRELEASE(m_pShadowTexture);
	A3DRELEASE(m_pShadowTextureCamera);

	A3DHLSLMan* pHLSLMan = m_pA3DDevice->GetA3DEngine()->GetA3DHLSLMan();
	pHLSLMan->ReleaseShader(m_pBlurShadowMapVS);
	pHLSLMan->ReleaseShader(m_pBlurShadowMapXPS);
	pHLSLMan->ReleaseShader(m_pBlurShadowMapYPS);
	m_pBlurShadowMapVS = NULL;
	m_pBlurShadowMapXPS = NULL;
	m_pBlurShadowMapYPS = NULL;

	A3DRELEASE(m_pStream);
	A3DRELEASE(m_pStreamDecl);

	m_bInit = false;
	m_bOptimize = false;
	m_bTerrainShadowOnePass = false;
}

void A3DShadowMap::SetShadowCamera(const A3DVECTOR3* pShadowCenter, const A3DVECTOR3* pSunLightDir, const float fShadowRadius,const float fShadowRealRadius)
{
    m_fShadowRadius = fShadowRadius;
	//m_pShadowCamera->SetProjectionParam(-fShadowRadius, fShadowRadius, -fShadowRadius, fShadowRadius, 0.2f, fShadowRadius * 4.0f);
	m_pShadowCamera->SetProjectionParam(-fShadowRadius, fShadowRadius, -fShadowRadius, fShadowRadius, 0.2f, fShadowRealRadius * 4.0f);

	A3DVECTOR3 vOffset = (*pSunLightDir) * fShadowRealRadius * 2.0f;
	m_pShadowCamera->SetPos(*pShadowCenter +  vOffset);
	A3DVECTOR3 vCameraDir = -(*pSunLightDir);
	m_pShadowCamera->SetDirAndUp(vCameraDir , A3D::g_vAxisY);
}

void A3DShadowMap::SetupTerrainShadowMapViewport(const A3DVECTOR3& vShadowCenter, float fShadowRealRadius, const A3DVECTOR3& vSunLightDir, bool bUpdateTotal, bool bActive)
{
	if(!m_bUseTerrain)
		return;

	if (bUpdateTotal)
	{
		m_pTerrainShadowCamera->SetProjectionParam(-fShadowRealRadius, fShadowRealRadius, -fShadowRealRadius, fShadowRealRadius, 0.2f, fShadowRealRadius * 6.0f);
		m_pTerrainShadowCamera->SetPos(vShadowCenter + fShadowRealRadius * 3.0f * A3D::g_vAxisY);
		m_pTerrainShadowCamera->SetDirAndUp(-A3D::g_vAxisY, vSunLightDir);

		int nSize = GetShadowMapSize();
		A3DVIEWPORTPARAM param;
		param.X = 0;
		param.Y = 0;
		param.Width = nSize;
		param.Height = nSize;
		param.MinZ = 0;
		param.MaxZ = 1.0f;
		m_pTerrainShadowViewport->SetParam(&param);

		if (bActive)
		{
			m_pTerrainShadowViewport->Active();
			m_pA3DDevice->Clear(D3DCLEAR_TARGET, A3DCOLORRGBA(255,255,255,255), 0, 0);
		}
	}
	else
	{
		m_pTerrainShadowCamera->SetPos(vShadowCenter + fShadowRealRadius * 3.0f * A3D::g_vAxisY);
		m_pTerrainShadowCamera->SetDirAndUp(-A3D::g_vAxisY, vSunLightDir);

		int row = m_nTerrainShadowMapCurTile / m_nTerrainShadowMapDivision;
		int col = m_nTerrainShadowMapCurTile % m_nTerrainShadowMapDivision;
		float fDivisionSize = fShadowRealRadius * 2.f / m_nTerrainShadowMapDivision;

		float cl = (col - 1) * fDivisionSize;
		float cr = cl + fDivisionSize;
		float ct = (1 - row) * fDivisionSize;
		float cb = ct - fDivisionSize;

		m_pTerrainShadowCamera->SetProjectionParam(cl, cr, cb, ct, 0.2f, fShadowRealRadius * 6.0f);

		A3DVIEWPORTPARAM param;
		param.X = col * m_nTerrainShadowMapTileSize;
		param.Y = row * m_nTerrainShadowMapTileSize;
		param.Width = m_nTerrainShadowMapTileSize;
		param.Height = m_nTerrainShadowMapTileSize;
		param.MinZ = 0;
		param.MaxZ = 1.0f;
		m_pTerrainShadowViewport->SetParam(&param);

		if (bActive)
		{
			m_pTerrainShadowViewport->Active();

			if (m_nTerrainShadowMapCurTile == 0)
				m_pA3DDevice->Clear(D3DCLEAR_TARGET, A3DCOLORRGBA(255,255,255,255), 0, 0);
		}
	}
}

bool A3DShadowMap::SetShadowTextureCamera(const A3DVECTOR3* pShadowRealCenter, const A3DVECTOR3* pSunLightDir, const float fShadowRealRadius)
{
	if(m_pShadowTextureCamera)
	{
		//shadow texture camera
		m_pShadowTextureCamera->SetProjectionParam(-fShadowRealRadius, fShadowRealRadius, -fShadowRealRadius, fShadowRealRadius, 0.2f, fShadowRealRadius * 4.0f);

		A3DVECTOR3 vOffset = (*pSunLightDir) * fShadowRealRadius * 2.0f;
		m_pShadowTextureCamera->SetPos(*pShadowRealCenter +  vOffset);
		A3DVECTOR3 vCameraDir = -(*pSunLightDir);
		m_pShadowTextureCamera->SetDirAndUp(vCameraDir , A3D::g_vAxisY);
		return true;
	}
	return false;

}

// void A3DShadowMap::AppearAsTexture(int nLayer)
// {
// 	if(m_bOptimize)
// 		m_pShadowTexture->Appear(nLayer);
// 	else
// 	{
// 		if(IsUsingHWPcf())
// 		{
// 			m_pShadowMap->AppearAsTexture(nLayer, true);
// 			//m_pA3DDevice->GetD3DDevice()->SetTexture(nLayer, m_pShadowMap->GetDepthTexture());
// 		}
// 		else
// 			m_pShadowMap->AppearAsTexture(nLayer);	
// 	}
// }

// void A3DShadowMap::DisappearAsTexture(int nLayer)
// {
// 	if(m_bOptimize)
// 		m_pShadowTexture->Disappear(nLayer);
// 	else
// 		m_pShadowMap->DisappearAsTexture(nLayer);	
// }

bool A3DShadowMap::IncorporateShadowTexture()
{
	if( !m_bOptimize)
		return false;

	IDirect3DSurface9* pShadowSurface = NULL;
	if( D3D_OK != m_pShadowTexture->GetD3DTexture()->GetSurfaceLevel(0, &pShadowSurface))
		return false;

	POINT destPoint;
	RECT rc;
	rc.left = 0;
	rc.right = m_nShadowMapSize;
	rc.top = 0;
	rc.bottom = m_nShadowMapSize;

	for( int nRow = 0; nRow < SHADOW_CACHE_ROW; nRow++)
	{
		for( int nCol = 0; nCol < SHADOW_CACHE_COL; nCol++)
		{

			destPoint.x = nCol * m_nShadowMapSize;
			destPoint.y = nRow * m_nShadowMapSize;

			if (D3D_OK != m_pA3DDevice->UpdateSurface(m_ppShadowCaches[nRow* SHADOW_CACHE_COL + nCol]->GetDXSurface(), &rc, pShadowSurface, &destPoint))
			{
				assert(false);
			}
		}
	}

	pShadowSurface->Release();
	return true;
}

bool A3DShadowMap::Set(const A3DVECTOR3* pShadowCenter, const A3DVECTOR3* pSunLightDir,const float fShadowRadius, const float fShadowRealRadius)
{
	m_pShadowMap->ApplyToDevice();

	SetShadowCamera(pShadowCenter, pSunLightDir, fShadowRadius,fShadowRealRadius);

	A3DMATRIX4 matScale = CalcTexScaleTM(m_nShadowMapSize, m_nShadowMapSize);
	m_matShadow = m_pShadowCamera->GetVPTM() * matScale;

	m_pShadowViewport->SetCamera(m_pShadowCamera);

	A3DVIEWPORTPARAM vp;
	vp.X = 0;
	vp.Y = 0;
	vp.Width = m_nShadowMapSize;
	vp.Height = m_nShadowMapSize;
	vp.MinZ = 0;
	vp.MaxZ = 1.f;
	m_pShadowViewport->SetParam(&vp);
	m_pShadowViewport->Active();


	return true;
}

bool A3DShadowMap::Restore()
{
	m_pShadowMap->WithdrawFromDevice();

	return true;
}

bool A3DShadowMap::ClearDevice( A3DCOLOR clearColor/* = 0xFFFFE000 */)
{
	m_pShadowViewport->SetClearColor(clearColor);
	m_pShadowViewport->ClearDevice();

	return true;
}

bool A3DShadowMap::Cache(int nCurCache)
{
	if( m_bOptimize)
	{
		if(D3D_OK != D3DXLoadSurfaceFromSurface(m_ppShadowCaches[nCurCache]->GetDXSurface(), NULL, NULL, 
			m_pShadowMap->GetTargetSurface()->m_pD3DSurface, NULL, NULL, D3DX_DEFAULT, 0))
			return false;
	}
	return true;
}

int A3DShadowMap::GetShadowMapSize()
{
	if( m_bOptimize)
		return m_nShadowMapSize * SHADOW_CACHE_COL;
	else
		return m_nShadowMapSize;
}




//TSM相关


//	Computes eye frustum corner points in world space
void A3DShadowMap::CalcEyeFrustumCorners(A3DCameraBase* pCamera, A3DVECTOR3* aCorners, 
										 float fNear, float fFar, float fScale)
{
	pCamera->CalcFrustumCorners(aCorners, false, fNear, fFar);

	//	Extend points if fScale != 1.0f
	if (fScale != 1.0f)
	{
		int i;

		//	Calculate center of points
		A3DVECTOR3 vCenter(0.0f);
		for(i=0; i < 8; i++)
			vCenter += aCorners[i];

		vCenter *= 0.125f;	//	vCenter /= 8

		//	Extend points from center
		for (i=0; i < 8; i++)
		{
			// scale by adding offset from center
			aCorners[i] += (aCorners[i] - vCenter) * (fScale - 1.0f);
		}
	}
}
void A3DShadowMap::CalcLightViewMatrix(const A3DVECTOR3& vCenter, const A3DVECTOR3& vLightDir)
{
	//	Calculate light position
	A3DVECTOR3 vLightPos = vCenter - vLightDir * 100.0f;

	//	Snap area's border to 1 meter increaments so that moving the camera does not 
	//	cause the shadows to jitter
	for (int m=0; m < 3; m++)
		vLightPos.m[m] = floorf(vLightPos.m[m]);

	//	Adjust light's position and get current light's view matrix
	A3DVECTOR3 up = A3DVECTOR3(0,1,0);
	if( (A3DVECTOR3(0,1,0) + vLightDir).Magnitude() <= 0.00001)
		up = A3DVECTOR3(1,0, 0);

	m_matLightView = a3d_LookAtMatrix(vLightPos, vLightPos + vLightDir, up/*A3DVECTOR3::vAxisY*/, 0.0f);
}

void A3DShadowMap::CalcFrustumInLightSpace(const A3DVECTOR3* aCorners, const A3DMATRIX4& matLightView, 
										   int iMapSize, A3DAABB& aabbInLight)
{
	//	Transfrom frustum corner points into light space, and calculate
	//	the bounding box
	aabbInLight.Clear();
	for (int j=0; j < 8; j++)
	{
		A3DVECTOR3 v = aCorners[j] * matLightView;
		aabbInLight.AddVertex(v);
	}

	aabbInLight.CompleteCenterExts();

	if (iMapSize)
	{
		//	This code removes the shimmering effect along the edges of shadows due to
		//	the light changing to fit the camera.
		//	To do this, we pad the ortho transform so that it is always big enough to cover 
		//	the entire camera view frustum.
		A3DVECTOR3 vDiagonal = aCorners[0] - aCorners[6];
		float fBound = vDiagonal.Magnitude();

		//	The offset calculated will pad the ortho projection so that it is always the same size 
		//	and big enough to cover the entire cascade interval.
		A3DVECTOR3 vBoarderOffset = (A3DVECTOR3(fBound) - 2.0f * aabbInLight.Extents) * 0.5f;
		vBoarderOffset.z = 0.0f;

		//	Add the offsets to the projection.
		aabbInLight.Maxs += vBoarderOffset;
		aabbInLight.Mins -= vBoarderOffset;

		//	The world units per texel are used to snap the shadow the orthographic projection
		//	to texel sized increments.  This keeps the edges of the shadows from shimmering.
		float fWorldUnitsPerTexel = fBound / (float)iMapSize;

		//	We snape the camera to 1 pixel increments so that moving the camera does not cause the shadows to jitter.
		//	This is a matter of integer dividing by the world space size of a texel
		//	Only adjust x and y
		for (int m=0; m < 2; m++)
		{
			float f = aabbInLight.Maxs.m[m] / fWorldUnitsPerTexel;
			//	aabbInLight.Maxs.m[m] = ceilf(f) * fWorldUnitsPerTexel;
			aabbInLight.Maxs.m[m] = (int)f * fWorldUnitsPerTexel;

			f = aabbInLight.Mins.m[m] / fWorldUnitsPerTexel;
			//	aabbInLight.Mins.m[m] = floorf(f) * fWorldUnitsPerTexel;
			aabbInLight.Mins.m[m] = (int)f * fWorldUnitsPerTexel;
		}

		aabbInLight.CompleteCenterExts();
	}
}

//	Calculate light's view matrix
void A3DShadowMap::CalcTSMLightViewMatrix(A3DCameraBase* pEyeCamera, const A3DVECTOR3& vLightDir)
{
	//	we need to transform the eye into the light's post-projective space.
	//	however, the sun is a directional light, so we first need to find an appropriate
	//	rotate/translate matrix, before constructing an ortho projection.
	A3DVECTOR3 vXAxis, vYAxis, vZAxis;

	//	Use light direction as Z axis, and let eye's direction project on Y axis
	vZAxis = vLightDir;
	//vXAxis.CrossProduct(pEyeCamera->GetDir(), vZAxis);
	vXAxis = CrossProduct(pEyeCamera->GetDir(), vZAxis);
	vXAxis.Normalize();
	vYAxis = CrossProduct(vZAxis, vXAxis);
	//vYAxis.CrossProduct(vZAxis, vXAxis);


	m_matLightView.Identity();
	m_matLightView.SetCol(0, vXAxis);
	m_matLightView.SetCol(1, vYAxis);
	m_matLightView.SetCol(2, vZAxis);

	const A3DVECTOR3& vFrom = pEyeCamera->GetPos();
	m_matLightView.m[3][0] = -DotProduct(vXAxis, vFrom);
	m_matLightView.m[3][1] = -DotProduct(vYAxis, vFrom);
	m_matLightView.m[3][2] = -DotProduct(vZAxis, vFrom);
}


/*	Adjust directional light's view TM and generate light's projection matrix.
	This function generate orthogonal projection, so it's only appropriate for directional light

	aabbInLight (in & out): AABB (in temporary light's space) of the area that includes shadow receivers and casters.
				the mins.z and maxs.z of aabbInLight may be modified by this function
	matLightView (in & out): in: temporary light's view TM; out: adjusted TM
	matLightProj (out): light's orthogonal projection matrix
*/
void A3DShadowMap::CalcDirLightViewAndProj(A3DAABB& aabbInLight, A3DMATRIX4& matLightView, A3DMATRIX4& matLightProj)
{
	float fDegradeScale = 1.f;
	//	Calculate light view TM for this slice
	float l = aabbInLight.Mins.x * fDegradeScale;
	float r = aabbInLight.Maxs.x * fDegradeScale;
	float t = aabbInLight.Maxs.y * fDegradeScale;
	float b = aabbInLight.Mins.y * fDegradeScale;

	float min_z = aabbInLight.Mins.z;
	float max_z = aabbInLight.Maxs.z;

	min_z -= 50.f;
	//	Adjust light space's origin, so that all objects are in front of light
	if (min_z < 0.0f)
	{
		matLightView = matLightView * a3d_Translate(0.0f, 0.0f, -min_z);
		max_z += -min_z;
		min_z = 0.0f;
	}
/*	else
	{
		matLightView = matLightView * a3d_Translate(0.0f, 0.0f, -min_z);
		max_z -= min_z;
		min_z = 0.0f;
	}
*/
	//aabbInLight.Mins.z = min_z;
	//aabbInLight.Maxs.z = max_z;
	//aabbInLight.CompleteCenterExts();

	//	Projection matrix
	D3DXMatrixOrthoOffCenterLH((D3DXMATRIX*)&matLightProj, l, r, b, t, min_z, max_z);

	m_pShadowCamera->SetViewTM(m_matLightView);
	m_pShadowCamera->SetProjectionParam(l, r, b, t, min_z, max_z); //好大的陷阱. SetProjectionTM之后,并不会给left和top设上正确的值
	m_pShadowViewport->SetCamera(m_pShadowCamera);
}

//	Build TSM matrix
void A3DShadowMap::BuildTSMMatrix(A3DCameraBase* pEyeCamera, A3DAABB& aabbInLight)
{
	A3DMATRIX4 matLightVP = m_matLightView * m_matLightProj;
	int i;

	//	Translate frustum corners into light's VP space
	A3DVECTOR3 aFrustumVerts[8];	//	Eye frustum corners
	CalcEyeFrustumCorners(pEyeCamera, aFrustumVerts, pEyeCamera->GetZFront(), m_fShadowRange, 1.0f);

	for (i=0; i < 8; i++)
	{
		aFrustumVerts[i] = aFrustumVerts[i] * matLightVP;
	}

	//	Get the center position of projected near plane and far plane
	A3DVECTOR3 aCenterPts[2];
	//  near plane
	aCenterPts[0].x = 0.25f * (aFrustumVerts[0].x + aFrustumVerts[1].x + aFrustumVerts[2].x + aFrustumVerts[3].x);
	aCenterPts[0].y = 0.25f * (aFrustumVerts[0].y + aFrustumVerts[1].y + aFrustumVerts[2].y + aFrustumVerts[3].y);
	aCenterPts[0].z = 0.0f;
	//  far plane
	aCenterPts[1].x = 0.25f * (aFrustumVerts[4].x + aFrustumVerts[5].x + aFrustumVerts[6].x + aFrustumVerts[7].x);
	aCenterPts[1].y = 0.25f * (aFrustumVerts[4].y + aFrustumVerts[5].y + aFrustumVerts[6].y + aFrustumVerts[7].y);
	aCenterPts[1].z = 0.0f;

	//	Move trapezoid center to origin point
	A3DVECTOR3 vCenterOrig = (aCenterPts[0] + aCenterPts[1]) * 0.5f;

	A3DMATRIX4 xlate_center(1.0f,			0.0f,			0.0f, 0.0f,
							0.0f,			1.0f,			0.0f, 0.0f,
							0.0f,			0.0f,			1.0f, 0.0f,
							-vCenterOrig.x,	-vCenterOrig.y, 0.0f, 1.0f);

	//	Rotate trapezoid around Z axis
	float half_center_len = a3d_Magnitude(aCenterPts[1] - vCenterOrig);
	float x_len = aCenterPts[1].x - vCenterOrig.x;
	float y_len = aCenterPts[1].y - vCenterOrig.y;
	float cos_theta = x_len / half_center_len;
	float sin_theta = y_len / half_center_len;

	A3DMATRIX4 rot_center(cos_theta,	sin_theta,	0.0f, 0.0f,
						  -sin_theta,	cos_theta,	0.0f, 0.0f,
						  0.0f,			0.0f,		1.0f, 0.0f,
						  0.0f,			0.0f,		0.0f, 1.0f);

	//  this matrix transforms the center line to y=0.
	//  since Top and Base are orthogonal to Center, we can skip computing the convex hull, and instead
	//  just find the view frustum X-axis extrema.  The most negative is Top, the most positive is Base
	//  Point Q (trapezoid projection point) will be a point on the y=0 line.
	m_matTSM = xlate_center * rot_center;

	A3DAABB aabbFrustum2D;
	aabbFrustum2D.Clear();

	//	Transform all frustum corners, build 2D AABB at same time
	for (i=0; i < 8; i++)
	{
		A3DVECTOR3& v = aFrustumVerts[i];
		v = v * m_matTSM;
		aabbFrustum2D.AddVertex(v);
	}

	aabbFrustum2D.CompleteCenterExts();	

	float x_scale = a_Max(fabsf(aabbFrustum2D.Maxs.x), fabsf(aabbFrustum2D.Mins.x));
	float y_scale = a_Max(fabsf(aabbFrustum2D.Maxs.y), fabsf(aabbFrustum2D.Mins.y));
	x_scale = 1.0f / x_scale;
	y_scale = 1.0f / y_scale;

	//	Maximize the area occupied by the bounding box
	A3DMATRIX4 scale_center(x_scale,	0.0f,		0.0f, 0.0f,
							0.0f,		y_scale,	0.0f, 0.0f,
							0.0f,		0.0f,		1.0f, 0.0f,
							0.0f,		0.0f,		0.0f, 1.0f);

	m_matTSM = m_matTSM * scale_center;

	//  Scale the frustum AABB up by these amounts (keep all values in the same space)
	aabbFrustum2D.Mins.x *= x_scale;
	aabbFrustum2D.Maxs.x *= x_scale;
	aabbFrustum2D.Mins.y *= y_scale;
	aabbFrustum2D.Maxs.y *= y_scale;
	aabbFrustum2D.CompleteCenterExts();

	//  compute eta.
	float lambda = aabbFrustum2D.Maxs.x - aabbFrustum2D.Mins.x;
	float delta_proj = m_fTSMDelta * lambda; //	focusPt.x - frustumAABB2D.minPt.x;

	const float xi = -0.6f;  // 80% line

	float eta = (lambda * delta_proj * (1.f + xi)) / (lambda * (1.f - xi) - 2.f * delta_proj);

	//  Compute the projection point a distance eta from the top line.  this point is on the center line, y=0
	float fProjectionPtQ = aabbFrustum2D.Maxs.x + eta;

	//  find the maximum slope from the projection point to any point in the frustum.  this will be the
	//  projection field-of-view
	float max_slope = -FLT_MAX;
	float min_slope = FLT_MAX;

	for (i=0; i < 8; i++)
	{
		A3DVECTOR3 tmp(aFrustumVerts[i].x * x_scale, aFrustumVerts[i].y * y_scale, 0.0f);
		float x_dist = tmp.x - fProjectionPtQ;

		if (!(ALMOST_ZERO(tmp.y) || ALMOST_ZERO(x_dist)))
		{
			max_slope = a_Max(max_slope, tmp.y / x_dist);
			min_slope = a_Min(min_slope, tmp.y / x_dist);
		}
	}

	float xn = eta;
	float xf = lambda + eta;

	//	dyx: note, here multiply a -1 to x axis, this must be restored later
	A3DMATRIX4 ptQ_xlate(-1.0f,				0.0f, 0.0f, 0.0f,
						 0.0f,				1.0f, 0.0f, 0.0f,
						 0.0f,				0.0f, 1.0f, 0.0f,
						 fProjectionPtQ,	0.0f, 0.0f, 1.0f);

	m_matTSM = m_matTSM * ptQ_xlate;

	//  this shear balances the "trapezoid" around the y=0 axis (no change to the projection pt position)
	//  since we are redistributing the trapezoid, this affects the projection field of view (shear_amt)
	float shear_amt = (max_slope + fabsf(min_slope)) * 0.5f - max_slope;
	max_slope = max_slope + shear_amt;

	A3DMATRIX4 trapezoid_shear(1.0f, shear_amt,	0.0f, 0.0f,
							   0.0f, 1.0f,		0.0f, 0.0f,
							   0.0f, 0.0f,		1.0f, 0.0f,
							   0.0f, 0.0f,		0.0f, 1.0f);

	m_matTSM = m_matTSM * trapezoid_shear;

	float z_aspect = (aabbInLight.Maxs.z - aabbInLight.Mins.z) / (aabbFrustum2D.Maxs.y - aabbFrustum2D.Mins.y);

	//	Perform a 2DH projection to 'unsqueeze' the top line.
	A3DMATRIX4 trapezoid_projection(xf / (xf - xn),		0.0f,				0.0f,							1.0f,
									0.0f,				1.0f / max_slope,	0.0f,							0.0f,
									0.0f,				0.0f,				1.0f / (z_aspect * max_slope),	0.0f,
									-xn * xf / (xf-xn),	0.0f,				0.0f,							0.0f);

	m_matTSM = m_matTSM * trapezoid_projection;

	//  the x axis is compressed to [0..1] as a result of the projection, so expand it to [-1,1]
	//	dyx: note, multiply -1 to x axis to couteract the one multiplied in ptQ_xlate
	A3DMATRIX4 biasedScaleX(-2.0f,	0.0f, 0.0f, 0.0f,
							0.0f,	1.0f, 0.0f, 0.0f,
							0.0f,	0.0f, 1.0f, 0.0f,
							1.0f,	0.0f, 0.0f, 1.0f);

	m_matTSM = m_matTSM * biasedScaleX;
}
//	Calculate the scale TM needed to sample pixel on shadow map
A3DMATRIX4 A3DShadowMap::CalcTexScaleTM(int iTexWid, int iTexHei)
{
	const float fOffsetU = 0.5f + (0.5f / iTexWid);
	const float fOffsetV = 0.5f + (0.5f / iTexHei);
	const float offSetDepth = 1e-5f;
	A3DMATRIX4 matScale(0.5f,		0.0f,		0.0f,			0.0f,
						0.0f,		-0.5f,		0.0f,			0.0f,
						0.0f,		0.0f,		1.0f,			0.0f,
						fOffsetU,	fOffsetV,	offSetDepth,	1.0f);

	return matScale;
}
bool A3DShadowMap::SetTSM( A3DCameraBase* pEyeCamera, const A3DVECTOR3& vCenter, float fFarDist, const A3DVECTOR3& vLightDir )
{
	//m_pA3DDevice->SetRenderTarget(m_pShadowMap);
	m_pShadowMap->ApplyToDevice();

	m_matLightView.Identity();
	m_matLightProj.Identity();
	m_matTSM.Identity();
	m_matShadow.Identity();
	//以下TSM算法来自Angelica 3.1

	//  This isn't strictly necessary for TSMs; however, my 'light space' matrix has a
	//  degeneracy when view == light, so this avoids the problem.
	
	if(!m_bEnableTSM)
		m_bDegrade = true;
	else
	{
		m_bDegrade = false;
	 	float fCosCamera = fabsf(DotProduct(pEyeCamera->GetDir(), vLightDir));
	 	const float fLimitCos = 0.9f;//0.75f;
	 	if (fCosCamera >= fLimitCos)
	 	{
	 		m_bDegrade = true;
	 		float fFactor = (fCosCamera - fLimitCos) / (1.0f - fLimitCos);
	 		a_Clamp(fFactor, 0.0f, 1.0f);
	 		fFarDist *= (1.0f - 0.6f * fFactor);
	 	}
	}

	m_fShadowRange = fFarDist;
    m_fShadowRadius = fFarDist;

	A3DVECTOR3 aCorners[8];

	

	if(vCenter == pEyeCamera->GetPos())
		CalcEyeFrustumCorners(pEyeCamera, aCorners, pEyeCamera->GetZFront(), fFarDist, 1.0f);
	else
	{
		float fExtent = fFarDist * 0.5f;
		A3DAABB aabb;
		aabb.Center = vCenter;
		aabb.Extents = A3DVECTOR3(fExtent, fExtent, fExtent);
		aabb.CompleteMinsMaxs();
		aabb.GetVertices(aCorners, NULL, false);
		
  		A3DVECTOR3 eyePos = pEyeCamera->GetPos();
  		A3DVECTOR3 eyeDir = pEyeCamera->GetDir();
  		float A = eyeDir.x;
  		float B = eyeDir.y;
  		float C = eyeDir.z;
  		float D = -eyePos.x * A - eyePos.y * B - eyePos.z * C;
  		float A2 = A * A;
  		float B2 = B * B;
  		float C2 = C * C;
  		for(int i = 0; i < 8; i++)
  		{
  			A3DVECTOR3& v = aCorners[i];
  			A3DVECTOR3 eyeToV = (v - eyePos);
  			
  			if(DotProduct(eyeToV, eyeDir) <= 0)
  			{
  				float t = (v.x * A + v.y * B + v.z * C + D) / (A2 + B2 + C2);
  				v = v - eyeDir * t;
  			}
  		}

	}

	A3DAABB aabbInLight;

	//	Build temporary light's view TM
	if (m_bDegrade)
	{
		CalcLightViewMatrix(vCenter, vLightDir);
		CalcFrustumInLightSpace(aCorners, m_matLightView, m_nShadowMapSize/*m_iMapSize*/, aabbInLight);
	}
	
	/*else
	{
		//CalcLightViewMatrix(pEyeCamera->GetPos(), vLightDir);
		CalcTSMLightViewMatrix(pEyeCamera, vLightDir);
		CalcFrustumInLightSpace(aCorners, m_matLightView, 0, aabbInLight);
	}
	*/

	//  This isn't strictly necessary for TSMs; however, my 'light space' matrix has a
	//  degeneracy when view == light, so this avoids the problem.
	if (m_bDegrade)
	{
		//aabbInLight.Mins.z = 1.f;
		//aabbInLight.CompleteCenterExts();
		//	Switch to SSM
		aabbInLight.Maxs.z = aabbInLight.Mins.z + 60.0f;
		//CalcDirLightViewAndProj(aabbInLight, m_matLightView, m_matLightProj);
		//m_matTSM.Identity();

		float fDegradeScale = 1.f;
		//	Calculate light view TM for this slice
		float l = aabbInLight.Mins.x * fDegradeScale;
		float r = aabbInLight.Maxs.x * fDegradeScale;
		float t = aabbInLight.Maxs.y * fDegradeScale;
		float b = aabbInLight.Mins.y * fDegradeScale;

		float min_z = aabbInLight.Mins.z;
		float max_z = aabbInLight.Maxs.z;

		min_z -= 50.f;
		//	Adjust light space's origin, so that all objects are in front of light
		if (min_z < 0.0f)
		{
			m_matLightView = m_matLightView * a3d_Translate(0.0f, 0.0f, -min_z);
			max_z += -min_z;
			min_z = 0.0f;
		}
		/*	else
		{
			matLightView = matLightView * a3d_Translate(0.0f, 0.0f, -min_z);
			max_z -= min_z;
			min_z = 0.0f;
		}
		*/
		//aabbInLight.Mins.z = min_z;
		//aabbInLight.Maxs.z = max_z;
		//aabbInLight.CompleteCenterExts();

		//	Projection matrix
		D3DXMatrixOrthoOffCenterLH((D3DXMATRIX*)&m_matLightProj, l, r, b, t, min_z, max_z);

		m_pShadowCamera->SetViewTM(m_matLightView);
		m_pShadowCamera->SetProjectionParam(l, r, b, t, min_z, max_z); //好大的陷阱. SetProjectionTM之后,并不会给left和top设上正确的值
		m_pShadowViewport->SetCamera(m_pShadowCamera);


	}
	/*
	else
	{
		aabbInLight.Maxs.z = aabbInLight.Mins.z + 60.0f;
		CalcDirLightViewAndProj(aabbInLight, m_matLightView, m_matLightProj);

		BuildTSMMatrix(pEyeCamera, aabbInLight);
	}
	*/


	//	TSM can't use linear depth !! Just set a non-zero value to m_fLFFarDist
	//m_fLFFarDist = 1.0f;

	//	Apply TMs to camera


	//	Build shadow map TM
	A3DMATRIX4 matScale = CalcTexScaleTM(m_nShadowMapSize, m_nShadowMapSize);
	
	m_matShadow = m_matLightView * m_matLightProj * m_matTSM * matScale;

	A3DVIEWPORTPARAM vp;
	vp.X = 0;
	vp.Y = 0;
	vp.Width = m_nShadowMapSize;
	vp.Height = m_nShadowMapSize;
	vp.MinZ = 0;
	vp.MaxZ = 1.f;
	m_pShadowViewport->SetParam(&vp);
	m_pShadowViewport->Active();

	return true;
}

A3DTexture* A3DShadowMap::GetShadowMapTexture()
{
	if(m_bOptimize)
		return m_pShadowTexture;
	else
	{
		if(IsUsingHWPcf())
		{
			m_shadowMapTexture.SetD3D9Texture(m_pShadowMap->GetDepthTexture());
			return &m_shadowMapTexture;
		}
		else
		{
			m_shadowMapTexture.SetD3D9Texture(m_pShadowMap->GetTargetTexture());
			return &m_shadowMapTexture;
		}
	}
}

A3DTexture* A3DShadowMap::GetShadowMapFloatTexture()
{
	m_shadowMapFloatTexture.SetD3D9Texture(m_pShadowMap->GetTargetTexture());
	return &m_shadowMapFloatTexture;
}

void A3DShadowMap::SetShadowMapSize( int iWidth )
{
	int oldSize = m_nShadowMapSize;
	m_nShadowMapSize = iWidth;
	if(CreateShadowMapRT())
		return;
	else
	{
		m_nShadowMapSize = oldSize;
		CreateShadowMapRT();
		return;
	}
}

bool A3DShadowMap::CreateShadowMapRT()
{
	A3DRELEASE(m_pShadowMap);

	m_bUseHWPCF = SupportHWPCF(m_pA3DDevice) && m_bUseHWPCF;

	//Create shadow map
	m_pShadowMap = new A3DRenderTarget();
	
	if(!m_pShadowMap)
		return false;

	A3DRenderTarget::RTFMT fmt;
	fmt.iWidth = m_nShadowMapSize;
	fmt.iHeight = m_nShadowMapSize;
	//fmt.fmtTarget = A3DFMT_A8R8G8B8;

	//只有D24S8的纹理可以做PCF
	//只有INTZ的纹理可以取深度值
	//由于需要尽可能做HWPCF,所以这里创建D24S8的深度缓冲

	fmt.fmtDepth = A3DFMT_D24S8;

	bool bSupport16bitRRT = m_pA3DDevice->GetA3DDeviceCaps()->SupportR16F_RT();
	bool bSupport32bitRRT = m_pA3DDevice->GetA3DDeviceCaps()->SupportR32F_RT();

	if(bSupport16bitRRT)
		fmt.fmtTarget = (A3DFORMAT)D3DFMT_R16F;
	else if(bSupport32bitRRT)
		fmt.fmtTarget = (A3DFORMAT)D3DFMT_R32F;
	else
	{
		delete m_pShadowMap;
		m_pShadowMap = NULL;
		return false;
	}

	if(!m_pShadowMap->Init(m_pA3DDevice, fmt, true, true))
	{
		g_A3DErrLog.Log("A3DShadowMap::Init(), Failed to create shadow map rendertarget!");
		delete m_pShadowMap;
		m_pShadowMap = NULL;
		return false;
	}
	return true;
}

bool A3DShadowMap::SupportHWPCF( A3DDevice* pDevice )
{
	HRESULT hr;
	hr = pDevice->GetA3DEngine()->GetD3D()->CheckDeviceFormat(D3DADAPTER_DEFAULT, 
		(D3DDEVTYPE)pDevice->GetDevType(), 
		(D3DFORMAT)pDevice->GetDevFormat().fmtAdapter, 
		D3DUSAGE_DEPTHSTENCIL, 
		D3DRTYPE_TEXTURE, 
		D3DFMT_D24S8);

	return (hr == D3D_OK);
}

bool A3DShadowMap::BlurVSM()
{
#ifdef _DEBUG_SHADOW_MAP
	if(GetAsyncKeyState(VK_F3) & 0x8000)
		return true;
	if(GetAsyncKeyState(VK_F1) & 0x8000)
		ReloadBlurShader();
	if(GetAsyncKeyState(VK_F2) & 0x8000)
	{
		D3DXSaveSurfaceToFileA("OriDepth.dds", D3DXIFF_DDS, m_pShadowMap->GetTargetSurface()->m_pD3DSurface, NULL, NULL);
	}
#endif
	A3DRenderTarget* pTempRenderTarget = m_pA3DDevice->GetRenderTargetMan()
		->RentRenderTargetColor(m_nShadowMapSize, m_nShadowMapSize, m_pShadowMap->GetFormat().fmtTarget);

	//先把当前状态保存下来.
	DWORD dwState;
	bool bOldAlphaBlendEnable;
	bool bOldAlphaTestEnable;
	bool bOldZTestEnable;
	bool bOldZWriteEnable;
	A3DCULLTYPE oldCullType;
	DWORD dwColorWrite;

	dwState = m_pA3DDevice->GetDeviceRenderState(D3DRS_ALPHABLENDENABLE);
	bOldAlphaBlendEnable = (dwState != 0 ? true : false);
	bOldAlphaTestEnable = m_pA3DDevice->GetAlphaTestEnable();
	dwState = m_pA3DDevice->GetDeviceRenderState(D3DRS_ZENABLE);
	bOldZTestEnable = (dwState != 0 ? true : false);
	dwState = m_pA3DDevice->GetDeviceRenderState(D3DRS_ZWRITEENABLE);
	bOldZWriteEnable = (dwState != 0 ? true : false);
	oldCullType = m_pA3DDevice->GetFaceCull();
	dwColorWrite = m_pA3DDevice->GetDeviceRenderState(D3DRS_COLORWRITEENABLE);

	m_pA3DDevice->SetAlphaBlendEnable(false);
	m_pA3DDevice->SetAlphaTestEnable(false);
	m_pA3DDevice->SetZTestEnable(false);
	m_pA3DDevice->SetZWriteEnable(false);
	m_pA3DDevice->SetFaceCull(A3DCULL_NONE);
	m_pA3DDevice->SetDeviceRenderState(D3DRS_COLORWRITEENABLE, 
		D3DCOLORWRITEENABLE_ALPHA | D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_GREEN);
	m_pA3DDevice->SetFogEnable(false);

	float fPixelSize = 1.f / (float)m_nShadowMapSize;

	m_pStream->Appear(0, false);
	m_pStreamDecl->Appear();
	m_pBlurShadowMapVS->SetValue2f("g_vPixelSize", fPixelSize, fPixelSize);
	m_pBlurShadowMapVS->Appear();
	
	//X pass
	pTempRenderTarget->ApplyToDevice();
	
	m_pBlurShadowMapXPS->SetValue2f("g_vPixelSize", fPixelSize, fPixelSize);
	m_pBlurShadowMapXPS->SetTexture("g_sampler", m_pShadowMap);
	m_pBlurShadowMapXPS->Appear();
	m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);
	m_pBlurShadowMapXPS->Disappear();
	pTempRenderTarget->WithdrawFromDevice();
	
	m_pShadowMap->ApplyToDevice();
	m_pBlurShadowMapYPS->SetValue2f("g_vPixelSize", fPixelSize, fPixelSize);
	m_pBlurShadowMapYPS->SetTexture("g_sampler", pTempRenderTarget);
	m_pBlurShadowMapYPS->Appear();
	m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);
	m_pBlurShadowMapYPS->Disappear();
	m_pShadowMap->WithdrawFromDevice();

	m_pBlurShadowMapVS->Disappear();


#ifdef _DEBUG_SHADOW_MAP
	if(GetAsyncKeyState(VK_F2) & 0x8000)
	{
		D3DXSaveSurfaceToFileA("BlurDepth.dds", D3DXIFF_DDS, m_pShadowMap->GetTargetSurface()->m_pD3DSurface, NULL, NULL);
	}
#endif

	m_pA3DDevice->SetAlphaBlendEnable(bOldAlphaBlendEnable);
	m_pA3DDevice->SetAlphaTestEnable(bOldAlphaTestEnable);
	m_pA3DDevice->SetZTestEnable(bOldZTestEnable);
	m_pA3DDevice->SetZWriteEnable(bOldZWriteEnable);
	m_pA3DDevice->SetFaceCull(oldCullType);
	m_pA3DDevice->SetDeviceRenderState(D3DRS_COLORWRITEENABLE, dwColorWrite);
	m_pA3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	m_pA3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );

	m_pA3DDevice->GetRenderTargetMan()->ReturnRenderTarget(pTempRenderTarget);
	
	return true;
}

bool A3DShadowMap::ReloadBlurShader()
{
	A3DHLSLMan* pHLSLMan = m_pA3DDevice->GetA3DEngine()->GetA3DHLSLMan();
	pHLSLMan->ReleaseShader(m_pBlurShadowMapVS);
	pHLSLMan->ReleaseShader(m_pBlurShadowMapXPS);
	pHLSLMan->ReleaseShader(m_pBlurShadowMapYPS);
	m_pBlurShadowMapVS = NULL;
	m_pBlurShadowMapXPS = NULL;
	m_pBlurShadowMapYPS = NULL;

	m_pBlurShadowMapVS = pHLSLMan->LoadShader("shaders\\2.2\\HLSL\\Shadow\\Shadow_BlurVS.hlsl", NULL, NULL);
	m_pBlurShadowMapXPS = pHLSLMan->LoadShader(NULL, "shaders\\2.2\\HLSL\\Shadow\\Shadow_BlurXPS.hlsl", NULL);
	m_pBlurShadowMapYPS = pHLSLMan->LoadShader(NULL, "shaders\\2.2\\HLSL\\Shadow\\Shadow_BlurYPS.hlsl", NULL);

	return true;
}

void A3DShadowMap::SetSampleLevel(int nSampleLevel)
{
    a_Clamp(nSampleLevel, 0, 1);
	m_nSampleLevel = nSampleLevel;
}
