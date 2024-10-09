/*
 * FILE: EC_PortraitRender.cpp
 *
 * DESCRIPTION: a class for portrait rendering
 *
 * CREATED BY: Hedi, 2005/2/25
 *
 * HISTORY:
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.	
 */


#include "A3DTypes.h"
#include "A3DDevice.h"
#include "A3DCamera.h"
#include "A3DTexture.h"
#include "A3DViewport.h"
#include "A3DRenderTarget.h"
#include "A2DSprite.h"

#include "EC_PortraitRender.h"

#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"

#define new A_DEBUG_NEW

CECPortraitRender::CECPortraitRender()
{
	m_pPortraitMapViewport		= NULL;
	m_pPortraitMapCamera		= NULL;
	m_pPortraitMapTarget		= NULL;

	m_pSysBackTarget			= NULL;

	m_fmtTexture				= A3DFMT_A8R8G8B8;
}

CECPortraitRender::~CECPortraitRender()
{
	Release();
}

bool CECPortraitRender::Init(int nPortraitMapSize)
{
	m_nPortraitMapSize = nPortraitMapSize;

	A3DDevice * pA3DDevice = g_pGame->GetA3DDevice();

	// now create render targets for shadow rendering
	m_pPortraitMapCamera = new A3DCamera();
	if( !m_pPortraitMapCamera->Init(pA3DDevice, DEG2RAD(20.0f), 0.05f, 20.0f, 1.0f) )
	{
		g_Log.Log("CECPortraitRender::Init(), failed to create the portrait map camera!");
		return false;
	}
	
	//	Create portrait map Viewport
	RECT rc;
	rc.left = 0;
	rc.right = m_nPortraitMapSize;
	rc.top = 0;
	rc.bottom = m_nPortraitMapSize;
	if( !pA3DDevice->CreateViewport(&m_pPortraitMapViewport, 0, 0, rc.right, rc.bottom,
									0.0f, 1.0f, false, true, 0x00000000) )
	{
		g_Log.Log("CECPortraitRender::Create portrait map viewport fail");
		return false;
	}
	m_pPortraitMapViewport->SetCamera(m_pPortraitMapCamera);

	/*
	// create light's shadow map render target
	A3DDEVFMT devFmt;
	devFmt.bWindowed	= true;
	devFmt.nWidth		= m_nPortraitMapSize;
	devFmt.nHeight		= m_nPortraitMapSize;
	devFmt.fmtTarget	= m_fmtTexture;
	devFmt.fmtDepth		= A3DFMT_D24X8;

	m_pPortraitMapTarget = new A3DRenderTarget();
	if( !m_pPortraitMapTarget->Init(pA3DDevice, devFmt, true, true) )
	{
		g_Log.Log("CECPortraitRender::Create portrait render target fail");
		return false;
	}

	HRESULT hval;
	hval = pA3DDevice->GetD3DDevice()->CreateRenderTarget(m_nPortraitMapSize, m_nPortraitMapSize, 
		(D3DFORMAT)m_fmtTexture, D3DMULTISAMPLE_NONE, TRUE, &m_pSysBackTarget);
	if( D3D_OK != hval )
	{
		g_Log.Log("CECPortraitRender::Create sys back render target fail");
		return false;
	}
	*/

	g_pGame->GetA3DDevice()->AddUnmanagedDevObject(this);
	return true;
}

bool CECPortraitRender::Release()
{
	g_pGame->GetA3DDevice()->RemoveUnmanagedDevObject(this);

	if( m_pSysBackTarget )
	{
		m_pSysBackTarget->Release();
		m_pSysBackTarget = NULL;
	}

	if( m_pPortraitMapTarget )
	{
		m_pPortraitMapTarget->Release();
		delete m_pPortraitMapTarget;
		m_pPortraitMapTarget = NULL;
	}

	if( m_pPortraitMapViewport )
	{
		m_pPortraitMapViewport->Release();
		delete m_pPortraitMapViewport;
		m_pPortraitMapViewport = NULL;
	}

	if( m_pPortraitMapCamera )
	{
		m_pPortraitMapCamera->Release();
		delete m_pPortraitMapCamera;
		m_pPortraitMapCamera = NULL;
	}
	
	return true;
}

bool CECPortraitRender::RenderPortraitToSprite(A3DViewport * pViewport, A2DSprite * pSprite, LPRENDERFORPORTRAIT pfnRenderForPortrait, LPVOID pArg)
{
	return true;
	if( pSprite->GetWidth() != m_nPortraitMapSize || pSprite->GetHeight() != m_nPortraitMapSize )
		return false;

	A3DDevice * pA3DDevice = g_pGame->GetA3DDevice();

	// first render into portrait's map
	pA3DDevice->SetRenderTarget(m_pPortraitMapTarget);
	m_pPortraitMapViewport->Active();
	m_pPortraitMapViewport->ClearDevice();

	if( !pfnRenderForPortrait(m_pPortraitMapViewport, pArg) )
		return false;

	pA3DDevice->RestoreRenderTarget();
	pViewport->Active();

	HRESULT hval;

	// copy to sys back render target
#ifdef ANGELICA_2_2
	hval = pA3DDevice->GetD3DDevice()->StretchRect(m_pPortraitMapTarget->GetTargetSurface(), NULL, m_pSysBackTarget, NULL, D3DTEXF_POINT);
#else
	hval = pA3DDevice->GetD3DDevice()->CopyRects(m_pPortraitMapTarget->GetTargetSurface(), NULL, 0, m_pSysBackTarget, NULL);
#endif // ANGELICA_2_2
	if( D3D_OK != hval )
		return false;

	// update to the texture
	D3DLOCKED_RECT lock;
	hval = m_pSysBackTarget->LockRect(&lock, NULL, 0);
	if( D3D_OK != hval )
		return false;
	
	D3DLOCKED_RECT lockSprite;
	hval = pSprite->GetTexture(0)->GetD3DTexture()->LockRect(0, &lockSprite, NULL, 0);
	if( D3D_OK != hval )
		return false;

	LPBYTE pSrc, pDest;
	pSrc = (LPBYTE) lock.pBits;
	pDest = (LPBYTE) lockSprite.pBits;
	for(int i=0; i<m_nPortraitMapSize; i++)
	{
		memcpy(pDest + lockSprite.Pitch * i,
			pSrc + lock.Pitch * i, 4 * m_nPortraitMapSize);
	}

	pSprite->GetTexture(0)->GetD3DTexture()->UnlockRect(0);
	m_pSysBackTarget->UnlockRect();

	return true;
}

//	Before device reset
bool CECPortraitRender::BeforeDeviceReset()
{
	if( m_pSysBackTarget )
	{
		m_pSysBackTarget->Release();
		m_pSysBackTarget = NULL;
	}

	return true;
}

//	After device reset
bool CECPortraitRender::AfterDeviceReset()
{
	HRESULT hval;
#ifdef ANGELICA_2_2
	hval = g_pGame->GetA3DDevice()->GetD3DDevice()->CreateTexture(m_nPortraitMapSize, m_nPortraitMapSize,
		1, 0, (D3DFORMAT)m_fmtTexture, D3DPOOL_SYSTEMMEM, &m_pSysBackTargetTex, NULL);
#else
	hval = g_pGame->GetA3DDevice()->GetD3DDevice()->CreateRenderTarget(m_nPortraitMapSize, m_nPortraitMapSize, 
		(D3DFORMAT)m_fmtTexture, D3DMULTISAMPLE_NONE, TRUE, &m_pSysBackTarget);
#endif // ANGELICA_2_2
	if( D3D_OK != hval )
	{
		g_Log.Log("CECPortraitRender::Create sys back render target fail");
		return false;
	}

	return true;
}