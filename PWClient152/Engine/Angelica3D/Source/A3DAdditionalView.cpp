/*
 * FILE: A3DAdditionalView.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2009/2/9
 *
 * HISTORY: 
 *
 * Copyright (c) 2009 Archosaur Studio, All Rights Reserved.
 */

#include "A3DAdditionalView.h"
#include "A3DPI.h"
#include "A3DEngine.h"
#include "A3DDevice.h"
#include "A3DViewport.h"
#include "A3DCamera.h"
#include "A3DOrthoCamera.h"
#include "A3DRenderTargetMan.h"
#include "AMemory.h"

#define new A_DEBUG_NEW

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


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


///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Implement A3DAdditionalView
//	
///////////////////////////////////////////////////////////////////////////

A3DAdditionalView::A3DAdditionalView()
{
	m_pA3DEngine	= NULL;
	m_pA3DDevice	= NULL;
	m_pCurViewport	= NULL;
	m_pCurCamera	= NULL;
	m_pD3DSwapChain	= NULL;
	m_hWnd			= NULL;
	m_iWidth		= 0;
	m_iHeight		= 0;
	m_bDepthBuf		= false;
	m_idAppied		= -1;
	m_bHoriMajor	= true;
	m_bNeedReApply	= false;
}

A3DAdditionalView::~A3DAdditionalView()
{
}

//	Initiaalize object
bool A3DAdditionalView::Init(A3DEngine* pA3DEngine, HWND hWnd, bool bDepthBuffer)
{
	m_pA3DEngine = pA3DEngine;
	m_pA3DDevice = pA3DEngine->GetA3DDevice();
	m_bDepthBuf	 = bDepthBuffer;

	//	Create swap chain
	if (!CreateSwapChain(hWnd, bDepthBuffer))
	{
		g_A3DErrLog.Log("A3DAdditionalView::Init, failed to create swap chain !");
		return false;
	}

	RECT rect;
	GetClientRect(hWnd, &rect);
	m_hWnd = hWnd;
	m_iWidth = rect.right;
	m_iHeight = rect.bottom;

	m_pA3DDevice->AddUnmanagedDevObject(this);

	return true;
}

//	Release object
void A3DAdditionalView::Release()
{
	if (m_pA3DDevice)
		m_pA3DDevice->RemoveUnmanagedDevObject(this);

	//	Release swap chain
	ReleaseSwapChain();
}

//	Create swap chain
bool A3DAdditionalView::CreateSwapChain(HWND hWnd, bool bDepthBuffer)
{
	if (!m_pA3DDevice || !hWnd)
	{
		ASSERT(hWnd);
		return false;
	}

	RECT rect;
	GetClientRect(hWnd, &rect);
	int iWidth = rect.right;
	int iHeight = rect.bottom;

	if (!iWidth || !iHeight)
	{
		g_A3DErrLog.Log("A3DAdditionalView::CreateSwapChain, invalid window size !");
		return false;
	}

	//	Fill present parameters
	const D3DPRESENT_PARAMETERS& d3dppMain = m_pA3DDevice->GetD3DPP();
	const A3DDEVFMT& devFmt = m_pA3DDevice->GetDevFormat();
	D3DPRESENT_PARAMETERS d3dpp;
	memset(&d3dpp, 0, sizeof(d3dpp));

	d3dpp.BackBufferWidth	= iWidth;
	d3dpp.BackBufferHeight	= iHeight;
	d3dpp.BackBufferFormat	= (D3DFORMAT)devFmt.fmtTarget;
	d3dpp.BackBufferCount	= 1;
	d3dpp.MultiSampleType	= d3dpp.MultiSampleType;
	d3dpp.SwapEffect		= D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow		= hWnd;
	d3dpp.Windowed			= TRUE;

	//	Create swap chain
	IDirect3DSwapChain9* pSwapChain = NULL;
	HRESULT hrErr = m_pA3DDevice->GetD3DDevice()->CreateAdditionalSwapChain(&d3dpp, &pSwapChain);
	if (FAILED(hrErr))
	{
		g_A3DErrLog.Log("A3DAdditionalView::CreateSwapChain, failed to create D3D swap chain !");
		return false;
	}

	//	Create depth-stencil buffer
	IDirect3DSurface9* pDepthBuffer = NULL;
	if (bDepthBuffer)
	{
		//	Note: device may use INTZ or RAWZ as depth buffer format, those formats can not be used
		//		to create a depth/stencil surface, so we always use A3DFMT_D24S8 here !
		hrErr = m_pA3DDevice->GetD3DDevice()->CreateDepthStencilSurface(iWidth, iHeight,
					(D3DFORMAT)A3DFMT_D24S8, D3DMULTISAMPLE_NONE, 0, FALSE, &pDepthBuffer, NULL);
		if (FAILED(hrErr))
		{
			pSwapChain->Release();
			return false;
		}
	}

	//	Get back buffer
	IDirect3DSurface9* pBackBuffer = NULL;
	pSwapChain->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);

	//	Release current swap chain
	ReleaseSwapChain();

	m_pD3DSwapChain = pSwapChain;
	m_BackBuf.m_pD3DSurface	= pBackBuffer;
	m_DepthBuf.m_pD3DSurface = pDepthBuffer;

	return true;
}

//	Release swap chain
void A3DAdditionalView::ReleaseSwapChain()
{
	//	Appied view couldn't be released
	if (IsAppliedToDevice())
	{
		ASSERT(m_idAppied == -1);
		return;
	}

	if (m_pA3DDevice)
	{
		if(m_pA3DDevice->GetNeedResetFlag())
		{
			A3DINTERFACERELEASE(m_BackBuf.m_pD3DSurface);
			A3DINTERFACERELEASE(m_DepthBuf.m_pD3DSurface);
			A3DINTERFACERELEASE(m_pD3DSwapChain);
		}
		else
		{
			//multithread render
			if( m_BackBuf.m_pD3DSurface)
			{
				m_pA3DDevice->ReleaseResource(m_BackBuf.m_pD3DSurface);
				m_BackBuf.m_pD3DSurface = NULL;
			}

			if( m_DepthBuf.m_pD3DSurface)
			{
				m_pA3DDevice->ReleaseResource(m_DepthBuf.m_pD3DSurface);
				m_DepthBuf.m_pD3DSurface = NULL;
			}

			if( m_pD3DSwapChain)
			{
				m_pA3DDevice->ReleaseResource(m_pD3DSwapChain);
				m_pD3DSwapChain = NULL;
			}
		}
	}
}

//	Set active viewport
void A3DAdditionalView::SetActiveViewport(A3DViewport* pViewport)
{
	m_pCurViewport = pViewport;
	UpdateViewportWhenResize();
}

//	Set active camera
void A3DAdditionalView::SetActiveCamera(A3DCameraBase* pCamera)
{
	m_pCurCamera = pCamera;
	UpdateCameraWhenResize();
}

//	Resize view when window's size changed
bool A3DAdditionalView::OnSize()
{
	if (!m_hWnd)
		return true;

	//	Check device state
	if (m_pA3DDevice->GetD3DDevice()->TestCooperativeLevel() != D3D_OK)
		return true;

	RECT rect;
	GetClientRect(m_hWnd, &rect);

	if (rect.right <= 0 || rect.right <= 0)
		return false;

	if (m_iWidth == rect.right && m_iHeight == rect.bottom)
		return true;

	//	Recreate swap chain
	if (!CreateSwapChain(m_hWnd, m_bDepthBuf))
		return false;

	m_iWidth = rect.right;
	m_iHeight = rect.bottom;

	//	Update viewport
	UpdateViewportWhenResize();
	//	Update camera
	UpdateCameraWhenResize();

	return true;
}

//	Update viewport when view size changed
void A3DAdditionalView::UpdateViewportWhenResize()
{
	if (!m_pCurViewport)
		return;
	
	A3DVIEWPORTPARAM Params;
	Params.X		= 0;
	Params.Y		= 0;
	Params.Width	= m_iWidth;
	Params.Height	= m_iHeight;
	Params.MinZ		= 0.0f;
	Params.MaxZ		= 1.0f;

	m_pCurViewport->SetParam(&Params);
}

//	Update camera when view size changed
void A3DAdditionalView::UpdateCameraWhenResize()
{
	if (!m_pCurCamera)
		return;

	if (m_pCurCamera->GetClassID() == A3D_CID_ORTHOCAMERA)
	{
		//	Orthogonal camera
		A3DOrthoCamera* pOrthoCamera = (A3DOrthoCamera*)m_pCurCamera;

		float fNear = pOrthoCamera->GetZFront();
		float fFar = pOrthoCamera->GetZBack();
		float fLeft = pOrthoCamera->GetLeft();
		float fRight = pOrthoCamera->GetRight();
		float fTop = pOrthoCamera->GetTop();
		float fBottom = pOrthoCamera->GetBottom();

		if (m_bHoriMajor)
		{
			float fRatio = (float)m_iHeight / m_iWidth;
			float fWidth = fRight - fLeft;
			float fHalfLength = fWidth * fRatio * 0.5f;
			float fCenter = (fTop + fBottom) * 0.5f;

			pOrthoCamera->SetProjectionParam(fLeft, fRight, fCenter-fHalfLength, fCenter+fHalfLength, fNear, fFar);
		}
		else	//	Vertical major
		{
			float fRatio = (float)m_iWidth / m_iHeight;
			float fLength = fTop - fBottom;
			float fHalfWidth = fLength * fRatio * 0.5f;
			float fCenter = (fLeft + fRight) * 0.5f;

			pOrthoCamera->SetProjectionParam(fCenter-fHalfWidth, fCenter+fHalfWidth, fBottom, fTop, fNear, fFar);
		}
	}
	else if (m_pCurCamera->GetClassID() == A3D_CID_CAMERA)
	{
		//	Perspective camera
		A3DCamera* pPersCamera = (A3DCamera*)m_pCurCamera;

		float fNear = pPersCamera->GetZFront();
		float fFar = pPersCamera->GetZBack();
		float fFOV = pPersCamera->GetFOV();
		float fRatio = (float)m_iWidth / m_iHeight;
		pPersCamera->SetProjectionParam(fFOV, fNear, fFar, fRatio);
	}
}

//	Before device reset
bool A3DAdditionalView::BeforeDeviceReset()
{
	//	It is possible that device is lost after we applied this to device
	//	so when device reset, we should first withdraw it, and release resources
	//	after device reset, we then could re-apply it on to the device
	if (IsAppliedToDevice())
	{
		WithdrawFromDevice();
		m_bNeedReApply = true;
	}

	ReleaseSwapChain();
	return true;
}

//	After device reset
bool A3DAdditionalView::AfterDeviceReset()
{
	//	Re-create swap chain
	if (!CreateSwapChain(m_hWnd, m_bDepthBuf))
		return false;

	if (m_bNeedReApply)
	{
		ApplyToDevice();
		m_bNeedReApply = false;
	}

	return true;
}

//	Apply view to device
bool A3DAdditionalView::ApplyToDevice()
{
	//	Apply twice ?
	if (IsAppliedToDevice())
	{
		ASSERT(m_idAppied == -1);
		return false;
	}

	//	Additional view always used the first render target slot
	const int iRTIndex = 0;

	A3DRenderTargetMan* pRTMan = m_pA3DDevice->GetRenderTargetMan();
	if (!pRTMan->PushRenderTarget(iRTIndex, this))
		return false;
	
	int iStackDepth = pRTMan->GetRTStackDepth(iRTIndex);
	m_idAppied = (iRTIndex << 16) | iStackDepth;

	return true;
}

//	Withdraw view from device
void A3DAdditionalView::WithdrawFromDevice()
{
	if (IsAppliedToDevice())
	{
		A3DRenderTargetMan* pRTMan = m_pA3DDevice->GetRenderTargetMan();
		int iStackDepth = m_idAppied & 0xffff;
		int iRTIndex = m_idAppied >> 16;
		ASSERT(iRTIndex == 0);

		if (pRTMan->GetRTStackDepth(iRTIndex) != iStackDepth)
		{
			ASSERT(0 && "Render target stack wrong !");
			return;
		}

		pRTMan->PopRenderTarget(iRTIndex);
		m_idAppied = -1;
	}
	else
	{
		ASSERT(m_idAppied != -1);
	}
}

//	Present
//FIXME!! 在d3d9EX下会返回错误
bool A3DAdditionalView::Present()
{
	if (!m_pD3DSwapChain)
		return true;



	HRESULT hr = m_pA3DDevice->SwapChainPresent(m_pD3DSwapChain, NULL, NULL, NULL, NULL, 0);
	if (FAILED(hr))
	{
		if (hr == D3DERR_DEVICELOST)
			return m_pA3DDevice->HandleDeviceLost();
		else
		{
			g_A3DErrLog.Log("A3DDevice: Present Failure!");
			return false;
		}
	}

	//return hr == D3D_OK;
	return true; //d3d9ex 
}

