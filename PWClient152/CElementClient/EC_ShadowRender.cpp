/*
 * FILE: EC_ShadowRender.cpp
 *
 * DESCRIPTION: a class for shadow rendering
 *
 * CREATED BY: Hedi, 2004/9/21
 *
 * HISTORY:
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.	
 */

#include <A3DDevice.h>
#include <A3DEngine.h>
#include <A3DFuncs.h>
#include <A3DCamera.h>
#include <A3DOrthoCamera.h>
#include <A3DStream.h>
#include <A3DTexture.h>
#include <A3DViewport.h>
#include <A3DTerrain2.h>
#include <A3DPixelShader.h>
#include <A3DVertexShader.h>
#include <A3DRenderTarget.h>
#include <A3DShaderMan.h>
#include <A3DLitModel.h>

#include "EC_ShadowRender.h"

#include "EC_Global.h"
#include "EC_Model.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_ManOrnament.h"
#include "EC_Ornament.h"
#include "EC_World.h"
#include "EC_CDS.h"
#include "EL_Building.h"

#include "EC_TriangleMan.h"

#define new A_DEBUG_NEW

#ifdef ANGELICA_2_2
static const D3DVERTEXELEMENT9 aVertexShaderDecl[] =
{
	{0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
	{0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},
	D3DDECL_END()
};
#endif // ANGELICA_2_2

static bool GetWorldBounds(const A3DAABB& aabb, const A3DMATRIX4& viewTM, float& l, float& r, float& b, float& t, float& n, float& f)
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

CECShadowRender::CECShadowRender()
{
	m_pA3DTerrain				= NULL;

	m_pShadowMapViewport		= NULL;
	m_pShadowMapCamera			= NULL;
	m_pShadowMapTarget			= NULL;

	m_pVertsArea				= NULL;
	m_pShadowStream				= NULL;

	m_pVertexShader				= NULL;
	m_pShadowShader				= NULL;

	m_bCanDoShadow				= false;
	m_bCastShadow				= false;
}

CECShadowRender::~CECShadowRender()
{
	Release();
}

bool CECShadowRender::AllocResource()
{
	A3DDevice * pA3DDevice = g_pGame->GetA3DDevice();

	// now create render targets for shadow rendering
	m_pShadowMapCamera = new A3DOrthoCamera();
	if( !m_pShadowMapCamera->Init(pA3DDevice, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f) )
	{
		g_Log.Log("CECShadowRender::Init(), failed to create the shadow map camera!");
		return false;
	}
	
	//	Create shadow map Viewport
	RECT rc;
	rc.left = 0;
	rc.right = m_nShadowMapSize;
	rc.top = 0;
	rc.bottom = m_nShadowMapSize;
	if( !pA3DDevice->CreateViewport(&m_pShadowMapViewport, 0, 0, rc.right, rc.bottom,
									0.0f, 1.0f, true, true, 0x00000000) )
	{
		g_Log.Log("CECShadowRender::Create shadow map viewport fail");
		return false;
	}
	m_pShadowMapViewport->SetCamera(m_pShadowMapCamera);

	// create light's shadow map render target
	A3DDEVFMT devFmt;
	devFmt.bWindowed	= true;
	devFmt.nWidth		= m_nShadowMapSize;
	devFmt.nHeight		= m_nShadowMapSize;
	devFmt.fmtTarget	= A3DFMT_A8R8G8B8;
	devFmt.fmtDepth		= A3DFMT_D24X8;

	m_pShadowMapTarget = new A3DRenderTarget();
	if( !m_pShadowMapTarget->Init(pA3DDevice, devFmt, true, true) )
		return false;

#ifdef ANGELICA_2_2
	// now load vertex shader for shadow render
	int n = 0;				
	m_pVertexShader = pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->LoadVertexShader(SHADER_DIR"vs\\shadow_vs.txt", false, (D3DVERTEXELEMENT9*)aVertexShaderDecl);
	if( NULL == m_pVertexShader )
	{
		g_Log.Log("A3DTerrainWater::Init(), failed to load water_vs.txt");
		return false;
	}

	// now load pixel shader for render shadow
	m_pShadowShader = pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->LoadPixelShader(SHADER_DIR"ps\\shadowrenderblur_14.txt", false);
	if( NULL == m_pShadowShader )
	{
		g_Log.Log("CECShadowRender::Init(), failed to load shadowrenderblur_14.txt");
		return false;
	}
#else
	// now load vertex shader for shadow render
	DWORD dwVSDecl[32];
	int n = 0;				
	dwVSDecl[n ++] = D3DVSD_STREAM(0);					// begin tokens.
	dwVSDecl[n ++] = D3DVSD_REG(0, D3DVSDT_FLOAT3);		// x, y, z in world
	dwVSDecl[n ++] = D3DVSD_REG(1, D3DVSDT_D3DCOLOR);	// diffuse color
	dwVSDecl[n ++] = D3DVSD_END();						// end tokens.
	m_pVertexShader = pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->LoadVertexShader(SHADER_DIR"vs\\shadow_vs.txt", false, dwVSDecl);
	if( NULL == m_pVertexShader )
	{
		g_Log.Log("A3DTerrainWater::Init(), failed to load water_vs.txt");
		return false;
	}

	// now load pixel shader for render shadow
	m_pShadowShader = pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->LoadPixelShader(SHADER_DIR"ps\\shadowrenderblur_14.txt", false);
	if( NULL == m_pShadowShader )
	{
		g_Log.Log("CECShadowRender::Init(), failed to load shadowrenderblur_14.txt");
		return false;
	}
#endif

	
	// now create streams
	if( !CreateStream(4096, 4096) )
	{
		g_Log.Log("CECShadowRender::Init(), failed to create stream!");
		return false;
	}

	return true;
}

bool CECShadowRender::ReleaseResource()
{
	A3DDevice * pA3DDevice = g_pGame->GetA3DDevice();;

	ReleaseStream();

	if( m_pShadowMapTarget )
	{
		m_pShadowMapTarget->Release();
		delete m_pShadowMapTarget;
		m_pShadowMapTarget = NULL;
	}

	if( m_pShadowMapViewport )
	{
		m_pShadowMapViewport->Release();
		delete m_pShadowMapViewport;
		m_pShadowMapViewport = NULL;
	}

	if( m_pShadowMapCamera )
	{
		m_pShadowMapCamera->Release();
		delete m_pShadowMapCamera;
		m_pShadowMapCamera = NULL;
	}
	
	if( m_pVertexShader )
	{
		pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->ReleaseVertexShader(&m_pVertexShader);
		m_pVertexShader = NULL;
	}

	if( m_pShadowShader )
	{
		pA3DDevice->GetA3DEngine()->GetA3DShaderMan()->ReleasePixelShader(&m_pShadowShader);
		m_pShadowShader = NULL;
	}

	return true;
}

bool CECShadowRender::Init(int nShadowMapSize)
{
	m_nShadowMapSize = nShadowMapSize;

	if( !AllocResource() )
	{
		m_bCanDoShadow = false;
		m_bCastShadow = false;	

		ReleaseResource();
	}
	else
	{
		m_bCanDoShadow = true;
		m_bCastShadow = true;
	}

	return true;
}

bool CECShadowRender::Release()
{
	m_arrayShadowers.RemoveAll();

	ReleaseResource();
	return true;
}

bool CECShadowRender::CreateStream(int nMaxVerts, int nMaxFaces)
{
	// try to release old ones.
	ReleaseStream();

	A3DDevice * pA3DDevice = g_pGame->GetA3DDevice();

	m_nMaxVerts = nMaxVerts;
	m_nMaxFaces = nMaxFaces;

	m_pShadowStream = new A3DStream();
	if( !m_pShadowStream->Init(pA3DDevice, sizeof(A3DSHADOWVERTEX), A3DSHADOWVERT_FVF, m_nMaxVerts, m_nMaxFaces * 3, A3DSTRM_REFERENCEPTR, A3DSTRM_REFERENCEPTR) )
	{
		g_Log.Log("CECShadowRender::CreateStream(), fail to create stream!");
		return false;
	}

	m_nVertCount = 0;
	m_nFaceCount = 0;

	m_nOccludeVertCount = 0;
	m_nOccludeFaceCount = 0;

	m_pVertsArea = new A3DVECTOR3[m_nMaxVerts];

	return true;
}

bool CECShadowRender::ReleaseStream()
{
	if( m_pVertsArea )
	{
		delete [] m_pVertsArea;
		m_pVertsArea = NULL;
	}

	if( m_pShadowStream )
	{
		m_pShadowStream->Release();
		delete m_pShadowStream;
		m_pShadowStream = NULL;
	}

	return true;
}

bool CECShadowRender::AddShadower(const A3DVECTOR3& vecCenter, const A3DAABB& aabb, SHADOW_RECEIVER shadowReceiver, LPRENDERFORSHADOW pfnRenderForShadow, LPVOID pArg)
{
	if( !m_bCanDoShadow || !m_bCastShadow )
		return true;

	SHADOWER s;

	s.vecCenter				= vecCenter;
	s.aabb					= aabb;
	s.shadowReceiver		= shadowReceiver;
	s.pfnRenderForShadow	= pfnRenderForShadow;
	s.pArg					= pArg;

	m_arrayShadowers.Add(s);
	return true;
}

bool CECShadowRender::Render(A3DViewport * pViewport, const A3DVECTOR3& vecLightDir)
{
	if( !m_bCanDoShadow || !m_bCastShadow )
	{
		ClearShadowers();
		return true;
	}

	int nNumShadow = m_arrayShadowers.GetSize();

	A3DMATRIX4 matScale = IdentityMatrix();
	matScale._11 = 0.5f;
	matScale._22 = -0.5f;
	matScale._41 = 0.5f;
	matScale._42 = 0.5f;
	A3DMATRIX4 matProjectedView;

	A3DCameraBase * pHostCamera = pViewport->GetCamera();
	A3DDevice * pA3DDevice = g_pGame->GetA3DDevice();
	pA3DDevice->SetLightingEnable(false);
	
	m_pShadowMapCamera->SetDirAndUp(vecLightDir, A3DVECTOR3(0.0f, 1.0f, 0.0f));
	A3DMATRIX4 matOffsetLT, matOffsetRT, matOffsetLB, matOffsetRB;
	matOffsetLT = IdentityMatrix();
	matOffsetLT._41 = -1.0f / m_nShadowMapSize;
	matOffsetRT = IdentityMatrix();
	matOffsetRT._41 = 1.0f / m_nShadowMapSize;
	matOffsetLB = IdentityMatrix();
	matOffsetLB._42 = -1.0f / m_nShadowMapSize;
	matOffsetRB = IdentityMatrix();
	matOffsetRB._42 = 1.0f / m_nShadowMapSize;

	for(int i=0; i<nNumShadow; i++)
	{
		SHADOWER s = m_arrayShadowers[i];

		if( !SetupStreamForShadower(pViewport, s, vecLightDir) )
			continue;

		if( m_nVertCount == 0 || m_nFaceCount == 0 )
			continue;

		m_pShadowMapCamera->SetPos(s.vecCenter);
		A3DMATRIX4 matView = m_pShadowMapCamera->GetViewTM();
		float l, r, b, t, n, f;
		GetWorldBounds(s.aabb, matView, l, r, b, t, n, f);
		float size1 = (float)(max(fabs(l), fabs(r)));
		float size2 = (float)(max(fabs(b), fabs(t)));
		float size = max(size1, size2);
		
		size *= 1.5f;
		m_pShadowMapCamera->SetProjectionParam(-size, size, -size, size, -100.0f, 100.0f);
		m_pShadowMapCamera->SetPos(s.vecCenter);

		// first render into light's shadow map
		pA3DDevice->SetRenderTarget(m_pShadowMapTarget);
		m_pShadowMapViewport->Active();
		m_pShadowMapViewport->ClearDevice();
		
		// first render the ground for clip, use z buffer to do clip
		m_pShadowStream->Appear(0);
		pA3DDevice->ClearTexture(0);

		A3DCULLTYPE cullType = pA3DDevice->GetFaceCull();
		pA3DDevice->SetFaceCull(A3DCULL_NONE);

		if( m_nOccludeVertCount && m_nOccludeFaceCount )
		{
			pA3DDevice->SetWorldMatrix(IdentityMatrix());
			pA3DDevice->SetDeviceRenderState(D3DRS_COLORWRITEENABLE, 0);
			pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, m_nOccludeVertCount, 0, m_nOccludeFaceCount);
			pA3DDevice->SetDeviceRenderState(D3DRS_COLORWRITEENABLE, 0x0000000F);
		}
		
		// 1. render shadower
		if( !s.pfnRenderForShadow || !s.pfnRenderForShadow(m_pShadowMapViewport, s.pArg) )
			continue;
		
		pA3DDevice->SetFaceCull(cullType);

		// 2. render shadow in the world
		pA3DDevice->RestoreRenderTarget();
		pViewport->Active();

		pA3DDevice->GetD3DDevice()->SetTexture(0, m_pShadowMapTarget->GetTargetTexture());
		pA3DDevice->GetD3DDevice()->SetTexture(1, m_pShadowMapTarget->GetTargetTexture());
		pA3DDevice->GetD3DDevice()->SetTexture(2, m_pShadowMapTarget->GetTargetTexture());
		pA3DDevice->GetD3DDevice()->SetTexture(3, m_pShadowMapTarget->GetTargetTexture());
		pA3DDevice->GetD3DDevice()->SetTexture(4, m_pShadowMapTarget->GetTargetTexture());
		m_pShadowShader->Appear();
		m_pShadowStream->Appear(0);
		m_pVertexShader->Appear();

		D3DXPLANE cp;
		DWORD dwState = pA3DDevice->GetDeviceRenderState(D3DRS_CLIPPLANEENABLE);
		if( dwState == D3DCLIPPLANE0 ) // only one clip plane supported now
		{
			D3DXPLANE hcp;
			pA3DDevice->GetD3DDevice()->GetClipPlane(0, (float *)&cp);
			A3DMATRIX4 matVP = pViewport->GetCamera()->GetVPTM();
			matVP.InverseTM();
			matVP.Transpose();
			D3DXPlaneTransform(&hcp, &cp, (D3DXMATRIX *) &matVP);
			pA3DDevice->GetD3DDevice()->SetClipPlane(0, hcp);
		}

		pA3DDevice->SetWorldMatrix(IdentityMatrix());
		
		matProjectedView = m_pShadowMapCamera->GetVPTM() * matScale;

		pA3DDevice->SetTextureAddress(0, A3DTADDR_CLAMP, A3DTADDR_CLAMP);
		pA3DDevice->SetTextureAddress(1, A3DTADDR_CLAMP, A3DTADDR_CLAMP);
		pA3DDevice->SetTextureAddress(2, A3DTADDR_CLAMP, A3DTADDR_CLAMP);
		pA3DDevice->SetTextureAddress(3, A3DTADDR_CLAMP, A3DTADDR_CLAMP);
		pA3DDevice->SetTextureAddress(4, A3DTADDR_CLAMP, A3DTADDR_CLAMP);
		
		// set vertex shader constants here
		A3DMATRIX4 matVP = pA3DDevice->GetViewMatrix() * pA3DDevice->GetProjectionMatrix();
		matVP.Transpose();
		pA3DDevice->SetVertexShaderConstants(0, &matVP, 4);

		A3DVECTOR4 c4(pHostCamera->GetPos());
		pA3DDevice->SetVertexShaderConstants(4, &c4, 1);

		A3DVECTOR4 c5(1.0f, 0.0f, 0.0f, 0.0f);
		pA3DDevice->SetVertexShaderConstants(5, &c5, 1);

		A3DMATRIX4 matTex = IdentityMatrix();
		matTex = matProjectedView;
		matTex.Transpose();
		pA3DDevice->SetVertexShaderConstants(6, &matTex, 4);

		matTex = matProjectedView * matOffsetLT;
		matTex.Transpose();
		pA3DDevice->SetVertexShaderConstants(10, &matTex, 4);

		matTex = matProjectedView * matOffsetRT;
		matTex.Transpose();
		pA3DDevice->SetVertexShaderConstants(14, &matTex, 4);

		matTex = matProjectedView * matOffsetLB;
		matTex.Transpose();
		pA3DDevice->SetVertexShaderConstants(18, &matTex, 4);
		
		matTex = matProjectedView * matOffsetRB;
		matTex.Transpose();
		pA3DDevice->SetVertexShaderConstants(22, &matTex, 4);

		//	Set shadow's color
	//	A3DCOLORVALUE colShadow(1.0f, 0.0f, 0.0f, 1.0f);
	//	pA3DDevice->SetPixelShaderConstants(0, &colShadow, 1);
			
		pA3DDevice->SetZWriteEnable(false);
		pA3DDevice->SetZBias(2);

		pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, m_nVertCount, 0, m_nFaceCount);

		pA3DDevice->SetZBias(0);
		pA3DDevice->SetZWriteEnable(true);
		pA3DDevice->SetTextureAddress(0, A3DTADDR_WRAP, A3DTADDR_WRAP);
		pA3DDevice->SetTextureAddress(1, A3DTADDR_WRAP, A3DTADDR_WRAP);
		pA3DDevice->SetTextureAddress(2, A3DTADDR_WRAP, A3DTADDR_WRAP);
		pA3DDevice->SetTextureAddress(3, A3DTADDR_WRAP, A3DTADDR_WRAP);
		pA3DDevice->SetTextureAddress(4, A3DTADDR_WRAP, A3DTADDR_WRAP);
		pA3DDevice->ClearTexture(0);
		pA3DDevice->ClearTexture(1);
		pA3DDevice->ClearTexture(2);
		pA3DDevice->ClearTexture(3);
		pA3DDevice->ClearTexture(4);
		pA3DDevice->SetTextureTransformFlags(0, A3DTTFF_DISABLE);
		pA3DDevice->SetTextureTransformFlags(1, A3DTTFF_DISABLE);
		pA3DDevice->SetTextureTransformFlags(2, A3DTTFF_DISABLE);
		pA3DDevice->SetTextureTransformFlags(3, A3DTTFF_DISABLE);
		pA3DDevice->SetTextureTransformFlags(4, A3DTTFF_DISABLE);
		pA3DDevice->SetTextureCoordIndex(0, 0);
		pA3DDevice->SetTextureCoordIndex(1, 1);
		pA3DDevice->SetTextureCoordIndex(2, 2);
		pA3DDevice->SetTextureCoordIndex(3, 3);
		pA3DDevice->SetTextureCoordIndex(4, 4);

		dwState = pA3DDevice->GetDeviceRenderState(D3DRS_CLIPPLANEENABLE);
		if( dwState == D3DCLIPPLANE0 ) // only one clip plane supported now
		{
			pA3DDevice->GetD3DDevice()->SetClipPlane(0, cp);
		}

		m_pVertexShader->Disappear();
		m_pShadowShader->Disappear();
	}
	
	pA3DDevice->SetLightingEnable(true);
	ClearShadowers();
	return true;
}

bool CECShadowRender::ClearShadowers()
{
	if( m_arrayShadowers.GetSize() )
		m_arrayShadowers.RemoveAll();
	return true;
}

#define MAX_SHADOW_DIS		50.0f
bool CECShadowRender::SetupStreamForShadower(A3DViewport * pViewport, const SHADOWER& shadower, const A3DVECTOR3& vecLightDir)
{
	m_nVertCount = 0;
	m_nFaceCount = 0;

	m_nOccludeVertCount = 0;
	m_nOccludeFaceCount = 0;

	A3DSHADOWVERTEX * pVerts;
	WORD * pIndices;

	if( !m_pShadowStream->LockVertexBuffer(0, 0, (BYTE **)&pVerts, 0) )
		return false;
	if( !m_pShadowStream->LockIndexBuffer(0, 0, (BYTE **)&pIndices, 0) )
	{
		m_pShadowStream->UnlockVertexBuffer();
		return false;
	}

	if( shadower.shadowReceiver & SHADOW_RECEIVER_TERRAIN )
	{
		if( !m_pA3DTerrain )
			goto LITMODELPART;

		// first get the shadow cast center on terrain, if can't get it, the shadow can be discarded
		RAYTRACERT trace;
		if( !m_pA3DTerrain->RayTrace(shadower.vecCenter - vecLightDir * 1.0f, vecLightDir * MAX_SHADOW_DIS, 1.0f, &trace) )
		{
			m_nFaceCount = 0;
			m_nVertCount = 0;
			goto LITMODELPART;
		}

		//	计算 shadower.aabb 经 vecLightDir 投影到水平地面后的 AABB
		D3DXVECTOR4 shadowLight(vecLightDir.x, vecLightDir.y, vecLightDir.z, 0.0f);
		D3DXPLANE	terrianPlane(0.0f, 1.0f, 0.0f, -trace.vHitPos.y);
		D3DXMATRIX shadowMatrix;
		D3DXMatrixShadow(&shadowMatrix, &shadowLight, &terrianPlane);
		const A3DVECTOR3 &aabbCenter = shadower.aabb.Center;
		const A3DVECTOR3 &aabbExtents = shadower.aabb.Extents;
		D3DXVECTOR3 aabbVertices[] =
		{
			D3DXVECTOR3((aabbCenter + A3DVECTOR3(aabbExtents.x, aabbExtents.y, aabbExtents.z)).m),
			D3DXVECTOR3((aabbCenter + A3DVECTOR3(aabbExtents.x, aabbExtents.y, -aabbExtents.z)).m),
			D3DXVECTOR3((aabbCenter + A3DVECTOR3(aabbExtents.x, -aabbExtents.y, aabbExtents.z)).m),
			D3DXVECTOR3((aabbCenter + A3DVECTOR3(aabbExtents.x, -aabbExtents.y, -aabbExtents.z)).m),
			D3DXVECTOR3((aabbCenter + A3DVECTOR3(-aabbExtents.x, aabbExtents.y, aabbExtents.z)).m),
			D3DXVECTOR3((aabbCenter + A3DVECTOR3(-aabbExtents.x, aabbExtents.y, -aabbExtents.z)).m),
			D3DXVECTOR3((aabbCenter + A3DVECTOR3(-aabbExtents.x, -aabbExtents.y, aabbExtents.z)).m),
			D3DXVECTOR3((aabbCenter + A3DVECTOR3(-aabbExtents.x, -aabbExtents.y, -aabbExtents.z)).m),
		};

		A3DAABB terrainAABB;
		terrainAABB.Clear();
		D3DXVECTOR3 temp;
		for (int k(0); k < sizeof(aabbVertices)/sizeof(aabbVertices[0]); ++ k)
		{
			const D3DXVECTOR3 &v = aabbVertices[k];
			D3DXVec3TransformCoord(&temp, &v, &shadowMatrix);
			terrainAABB.AddVertex(A3DVECTOR3(temp.x, temp.y, temp.z));
		}
		terrainAABB.CompleteCenterExts();

		//	计算接受阴影的地面面片范围（以trace.vHitPos为中心）
		A3DVECTOR3 terrainSize1 = terrainAABB.Maxs - trace.vHitPos;
		float fSize1 = max(fabs(terrainSize1.x), fabs(terrainSize1.z));

		A3DVECTOR3 terrainSize2 = terrainAABB.Mins - trace.vHitPos;
		float fSize2 = max(fabs(terrainSize2.x), fabs(terrainSize2.z));

		float fSize = max(fSize1, fSize2);
		fSize += fSize;
		a_ClampFloor(fSize, 1.0f);

		int nx(1), nz(1);
		nx = nz = (int)ceil(fSize);
		
		// see if too much faces or verts
		if( (nx+1) * (nz+1) > m_nMaxVerts || nx * nz * 2 > m_nMaxFaces )
			goto LITMODELPART;

		int nVertCount = (nx + 1) * (nz + 1);
		int nIndexCount = nx * nz * 6;

		int alpha = min(255, max(0, (int)(255 * (1.0f - trace.fFraction + fSize / MAX_SHADOW_DIS))));

		if( !m_pA3DTerrain->GetFacesOfArea(trace.vHitPos, nx, nz, m_pVertsArea, pIndices) )
		{
			m_nFaceCount = 0;
			m_nVertCount = 0;
			goto LITMODELPART;
		}

		// now fill the vertex buffer here;
		A3DVECTOR3 vecLightDirH = vecLightDir;
		vecLightDirH.y = 0.0f;
		vecLightDirH.Normalize();
		A3DVECTOR3 vecCenter = shadower.vecCenter - vecLightDirH * 1.0f;
		A3DCOLOR	color;
		int	i, j;
		for(i=0; i<=nz; i++)
		{
			for(j=0; j<=nx; j++)
			{
				int nVertIndex = i * (nx + 1) + j;

				if( i == 0 || i == nz || j == 0 || j == nz )
					color = 0x00ffffff;
				else
					color = (alpha << 24) | 0x00ffffff;

				pVerts[nVertIndex] = A3DSHADOWVERTEX(m_pVertsArea[nVertIndex], color);
				pVerts[nVertIndex].vPos.y += 0.02f;
			}
		}

		m_nVertCount = nVertCount;
		m_nFaceCount = nIndexCount / 3;

		m_nOccludeVertCount = m_nVertCount;
		m_nOccludeFaceCount = m_nFaceCount;

		pVerts += nVertCount;
		pIndices += nIndexCount;
	}

LITMODELPART:
	if( shadower.shadowReceiver & SHADOW_RECEIVER_LITMODEL )
	{
		CECTriangleMan * pTriangleMan = g_pGame->GetGameRun()->GetWorld()->GetOrnamentMan()->GetTriangleMan();
		if( pTriangleMan )
		{
			TMan_Overlap_Info info;
			info.vStart = shadower.aabb.Center;
			info.vStart.y += shadower.aabb.Extents.y;
			info.vExtent = shadower.aabb.Extents * 4.0f;
			info.vDelta = vecLightDir * MAX_SHADOW_DIS;
			info.pCamera = pViewport->GetCamera();
			pTriangleMan->TestOverlap(info);

			int nTriVertCount = info.pVert.size();
			int nTriIndexCount = info.pIndex.size();
			if( nTriVertCount && nTriIndexCount && nTriVertCount + m_nVertCount <= m_nMaxVerts && nTriIndexCount / 3 + m_nFaceCount <= m_nMaxFaces )
			{
				// now fill these verts into shadow buffer.
				int i;
				for(i=0; i<nTriVertCount; i++)
					pVerts[i] = A3DSHADOWVERTEX(info.pVert[i], 0xffffffff);
				for(i=0; i<nTriIndexCount; i++)
					pIndices[i] = m_nVertCount + info.pIndex[i];
				
				m_nVertCount += nTriVertCount;
				m_nFaceCount += nTriIndexCount / 3;
			}
		}
	}

	m_pShadowStream->UnlockVertexBuffer();
	m_pShadowStream->UnlockIndexBuffer();
	return true;
}

bool CECShadowRender::SetCastShadowFlag(bool bFlag)
{
	if( !m_bCanDoShadow )
		return true;

	if( bFlag == m_bCastShadow )
		return true;

	m_bCastShadow = bFlag;
	if( m_bCastShadow )
	{
		// we should create resource for shadow
		if( !AllocResource() )
		{
			ReleaseResource();
			m_bCastShadow = false;
			return false;
		}
	}
	else
	{
		// resource for shadow are not needed, so just release them
		ReleaseResource();
	}

	return true;
}
