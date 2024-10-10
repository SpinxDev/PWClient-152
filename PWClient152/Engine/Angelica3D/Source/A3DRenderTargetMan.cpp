/*
 * FILE: A3DRenderTargetMan.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2009/7/8
 *
 * HISTORY: 
 *
 * Copyright (c) 2009 Archosaur Studio, All Rights Reserved.
 */

#include "A3DRenderTargetMan.h"
#include "A3DPI.h"
#include "A3DRenderTarget.h"
#include "A3DCubeRenderTarget.h"
#include "A3DEngine.h"
#include "A3DDevice.h"
#include "A3DAdditionalView.h"
#include "A3DMacros.h"
#include "A3DSurface.h"
#include "ATime.h"

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
//	Implement A3DRenderTargetMan
//	
///////////////////////////////////////////////////////////////////////////

A3DRenderTargetMan::A3DRenderTargetMan()
: m_nRenderTargetSize(0)
{
	m_pA3DDevice			= NULL;
	m_iRentableRTNum		= 0;
	m_iRentableCubeRTNum	= 0;
}

A3DRenderTargetMan::~A3DRenderTargetMan()
{
}

//	Initialize object
bool A3DRenderTargetMan::Init(A3DDevice* pA3DDevice)
{
	ASSERT(pA3DDevice);
	m_pA3DDevice = pA3DDevice;

	//	Create RT stacks
	int i, iMaxRTNum = m_pA3DDevice->GetD3DCaps().NumSimultaneousRTs;
	for (i=0; i < iMaxRTNum; i++)
	{
		RTStack* pStack = new RTStack;
		m_aRTStacks.Add(pStack);
	}

	//	Initialize stack 0
	RTStack* pStack = m_aRTStacks[0];
	RT_SURFS surfs;
	surfs.pTargetBuffer = m_pA3DDevice->GetDefRenderTarget();
	surfs.pDepthBuffer = m_pA3DDevice->GetDefDepthBuffer();
	pStack->Push(surfs);

	for (i=1; i < iMaxRTNum; i++)
	{
		surfs.pTargetBuffer = NULL;
		surfs.pDepthBuffer = NULL;
		m_aRTStacks[i]->Push(surfs);
	}

	return true;
}

//	Release object
void A3DRenderTargetMan::Release()
{
	int i;

	//	Clear render target stack
	for (i=0; i < m_aRTStacks.GetSize(); i++)
	{
		RTStack* pStack = m_aRTStacks[i];
		delete pStack;
	}

	m_aRTStacks.RemoveAll();

	//	Check if all rentable render targets have been returned
	ASSERT(m_iRentableRTNum == m_aRentableRTs.GetSize());

	g_A3DErrLog.Log("%d rentable render targets were created !", m_iRentableRTNum);

	//	Release all rentable render targets. 
	for (i=0; i < m_aRentableRTs.GetSize(); i++)
	{
		A3DRenderTarget* pRT = m_aRentableRTs[i];

	#ifdef _DEBUG
		const A3DRenderTarget::RTFMT& fmt = pRT->GetFormat();
		if (pRT->GetTargetSurface()->m_pD3DSurface)
			g_A3DErrLog.Log("color RT: [%d x %d], format [%d] !", pRT->GetWidth(), pRT->GetHeight(), fmt.fmtTarget);
		else
			g_A3DErrLog.Log("depth RT: [%d x %d], format [%d] !", pRT->GetWidth(), pRT->GetHeight(), pRT->GetDepthFormat());
	#endif

		A3DRELEASE(pRT);
	}

	m_aRentableRTs.RemoveAll();

	//	Release cube render targets
	ASSERT(m_iRentableCubeRTNum == m_aRentableCubeRTs.GetSize());

	g_A3DErrLog.Log("%d cube rentable render targets were created !", m_iRentableCubeRTNum);

	//	Release all rentable render targets. 
	for (i=0; i < m_aRentableCubeRTs.GetSize(); i++)
	{
		A3DCubeRenderTarget* pRT = m_aRentableCubeRTs[i];
		A3DRELEASE(pRT);
	}

	m_aRentableCubeRTs.RemoveAll();
}

//	Reset render targets
bool A3DRenderTargetMan::Reset()
{
	ASSERT(m_pA3DDevice);
	IDirect3DDevice9* pD3DDevice = m_pA3DDevice->GetD3DDevice();

	int i;

	//	Restore render targets
	for (i=0; i < m_aRTStacks.GetSize(); i++)
	{
		RTStack* pStack = m_aRTStacks[i];
		if (!pStack->GetElementNum())
			continue;

		RT_SURFS surfs = pStack->Peek();

		if (surfs.pTargetBuffer)
		{
			ASSERT(surfs.pTargetBuffer->m_pD3DSurface);
			HRESULT hval = pD3DDevice->SetRenderTarget(i, surfs.pTargetBuffer->m_pD3DSurface);
			if (D3D_OK != hval)
			{
				g_A3DErrLog.Log("A3DRenderTargetMan::Reset(), D3DDevice SetRenderTarget Fail!");
				return false;
			}
		}
	}

	//	Restore depth buffer
	RT_SURFS surfs = m_aRTStacks[0]->Peek();
	if (surfs.pDepthBuffer)
	{
		ASSERT(surfs.pDepthBuffer->m_pD3DSurface);
		HRESULT hval = pD3DDevice->SetDepthStencilSurface(surfs.pDepthBuffer->m_pD3DSurface);
		if (D3D_OK != hval)
		{
			g_A3DErrLog.Log("A3DRenderTargetMan::Reset(), D3DDevice SetRenderTarget Fail!");
			return false;
		}
	}

	return true;
}

//	Push render target
bool A3DRenderTargetMan::PushRenderTarget(int iRTIndex, A3DSurfaceProxy* pTargetBuffer, 
									A3DSurfaceProxy* pDepthBuffer)
{
	if (iRTIndex < 0 || iRTIndex >= m_aRTStacks.GetSize())
	{
		ASSERT(iRTIndex >= 0 && iRTIndex < m_aRTStacks.GetSize());
		return false;
	}

	RT_SURFS top;
	if (!PeekRenderTarget(iRTIndex, top))
		return false;

	IDirect3DDevice9* pD3DDevice = m_pA3DDevice->GetD3DDevice();

	//	Apply new render target
	RT_SURFS surfs;

	if (iRTIndex == 0)
	{
		surfs.pTargetBuffer = pTargetBuffer->m_pD3DSurface ? pTargetBuffer : top.pTargetBuffer;
		surfs.pDepthBuffer = pDepthBuffer->m_pD3DSurface ? pDepthBuffer : top.pDepthBuffer;
		ASSERT(surfs.pTargetBuffer->m_pD3DSurface);
		ASSERT(surfs.pDepthBuffer->m_pD3DSurface);

		HRESULT hval1 = pD3DDevice->SetRenderTarget(iRTIndex, surfs.pTargetBuffer->m_pD3DSurface);
		HRESULT hval2 = pD3DDevice->SetDepthStencilSurface(surfs.pDepthBuffer->m_pD3DSurface);
		if (D3D_OK != hval1 || D3D_OK != hval2)
		{
			g_A3DErrLog.Log("A3DRenderTargetMan::PushRenderTarget() D3DDevice SetRenderTarget Fail!");
			return false;
		}
	}
	else
	{
		//	Only set color buffer
		surfs.pTargetBuffer = pTargetBuffer->m_pD3DSurface ? pTargetBuffer : top.pTargetBuffer;
		surfs.pDepthBuffer = NULL;

		HRESULT hval;
		if (surfs.pTargetBuffer)
			hval = pD3DDevice->SetRenderTarget(iRTIndex, surfs.pTargetBuffer->m_pD3DSurface);
		else
			hval = pD3DDevice->SetRenderTarget(iRTIndex, NULL);

		if (D3D_OK != hval)
		{
			g_A3DErrLog.Log("A3DRenderTargetMan::PushRenderTarget() D3DDevice SetRenderTarget Fail!");
			return false;
		}
	}

	RTStack* pRTStack = m_aRTStacks[iRTIndex];
	pRTStack->Push(surfs);

	return true;
}

bool A3DRenderTargetMan::PushRenderTarget(int iRTIndex, A3DRenderTarget* pRenderTarget)
{
	A3DSurfaceProxy* pBackBuffer = pRenderTarget->GetTargetSurface();
	A3DSurfaceProxy* pDepthBuffer = pRenderTarget->GetDepthSurface();
	return PushRenderTarget(iRTIndex, pBackBuffer, pDepthBuffer);
}

bool A3DRenderTargetMan::PushRenderTarget(int iRTIndex, A3DAdditionalView* pAdditionalView)
{
	A3DSurfaceProxy* pBackBuffer = pAdditionalView->GetBackBuffer();
	A3DSurfaceProxy* pDepthBuffer = pAdditionalView->GetDepthBuffer();
	return PushRenderTarget(iRTIndex, pBackBuffer, pDepthBuffer);
}

//	Pop render target
bool A3DRenderTargetMan::PopRenderTarget(int iRTIndex)
{
	if (iRTIndex < 0 || iRTIndex >= m_aRTStacks.GetSize())
	{
		ASSERT(iRTIndex >= 0 && iRTIndex < m_aRTStacks.GetSize());
		return false;
	}

	RTStack* pRTStack = m_aRTStacks[iRTIndex];
	int iCurDepth = pRTStack->GetElementNum();

	//	Basic target shouldn't be poped
	if (iRTIndex == 0)
	{
		if (iCurDepth <= 1)
		{
			ASSERT(iCurDepth > 1);
			return false;
		}
	}
	else if (iCurDepth == 0)
	{
		ASSERT(iCurDepth > 0);
		return false;
	}

	pRTStack->Pop();

	IDirect3DDevice9* pD3DDevice = m_pA3DDevice->GetD3DDevice();

	//	Restore to previous stack level
	if (iRTIndex == 0)
	{
		RT_SURFS surfs = pRTStack->Peek();
		HRESULT hval1 = pD3DDevice->SetRenderTarget(iRTIndex, surfs.pTargetBuffer->m_pD3DSurface);
		HRESULT hval2 = pD3DDevice->SetDepthStencilSurface(surfs.pDepthBuffer->m_pD3DSurface);
		if (D3D_OK != hval1 || D3D_OK != hval2)
		{
			g_A3DErrLog.Log("A3DRenderTargetMan::PopupRenderTarget() D3DDevice SetRenderTarget Fail!");
			return false;
		}
	}
	else
	{
		//	Only restore color buffer
		RT_SURFS surfs = pRTStack->Peek();

		HRESULT hval;
		if (surfs.pTargetBuffer)
			hval = pD3DDevice->SetRenderTarget(iRTIndex, surfs.pTargetBuffer->m_pD3DSurface);
		else
			hval = pD3DDevice->SetRenderTarget(iRTIndex, NULL);

		if (D3D_OK != hval)
		{
			g_A3DErrLog.Log("A3DRenderTargetMan::PopupRenderTarget() D3DDevice SetRenderTarget Fail!");
			return false;
		}
	}

	return true;
}

//	Peek render target but not pop it
bool A3DRenderTargetMan::PeekRenderTarget(int iRTIndex, RT_SURFS& rtSurfs)
{
	if (iRTIndex < 0 || iRTIndex >= m_aRTStacks.GetSize())
	{
		ASSERT(iRTIndex >= 0 && iRTIndex < m_aRTStacks.GetSize());
		return false;
	}

	RTStack* pRTStack = m_aRTStacks[iRTIndex];
	if (!pRTStack->GetElementNum())
	{
		ASSERT(pRTStack->GetElementNum());
		return false;
	}

	rtSurfs = pRTStack->Peek();
	return true;
}

//	Get render target depth
int A3DRenderTargetMan::GetRTStackDepth(int iRTIndex)
{
	if (iRTIndex < 0 || iRTIndex >= m_aRTStacks.GetSize())
	{
		ASSERT(iRTIndex >= 0 && iRTIndex < m_aRTStacks.GetSize());
		return false;
	}

	RTStack* pRTStack = m_aRTStacks[iRTIndex];
	return pRTStack->GetElementNum();
}

/*	Rent render target from manager

	iWid, iHei: size of target. there are some pre-defined value:
			0: same as current device's size
			-2: 1/2 of current device's size
			-4: 1/4 of current device's size
	fmtColor: color buffer format
	fmtDepth: depth buffer format
	bAutoMipMap: true, automatically generate mipmap
*/
A3DRenderTarget* A3DRenderTargetMan::RentRenderTargetColor(int iWid, int iHei, A3DFORMAT fmtColor, bool bAutoMipMap/* false */)
{
	int i;
	
	//	Check there is a RT with same properties can be rent
	for (i=0; i < m_aRentableRTs.GetSize(); i++)
	{
		A3DRenderTarget* pRT = m_aRentableRTs[i];
		if (pRT->GetTargetSurface()->m_pD3DSurface)
		{
			const A3DRenderTarget::RTFMT& rtFmt = pRT->GetFormat();
			if (rtFmt.iWidth == iWid && rtFmt.iHeight == iHei && rtFmt.fmtTarget == fmtColor && pRT->IsAutoMipMap() == bAutoMipMap)
			{
				//	All properties match
				m_aRentableRTs.RemoveAtQuickly(i);
				return pRT;
			}
		}
	}

	//	Release long-time not used target
	ReleaseIdleRenderTarget(false);

	//	Create a new render target
	A3DRenderTarget::RTFMT rtFmt;
	rtFmt.iWidth	= iWid;
	rtFmt.iHeight	= iHei;
	rtFmt.fmtTarget	= fmtColor;
	rtFmt.fmtDepth	= A3DFMT_D16;	//	Has no effect

	A3DRenderTarget* pRT = new A3DRenderTarget;
	if (!pRT || !pRT->Init(m_pA3DDevice, rtFmt, true, false, bAutoMipMap))
	{
		A3DRELEASE(pRT);
		g_A3DErrLog.Log("A3DRenderTargetMan::RentRenderTargetColor, failed to create render target !");
		return NULL;
	}

	//	Mark rent time
	pRT->m_dwRentTime = a_GetTime();

	m_iRentableRTNum++;

	//	Test code...
//	g_A3DErrLog.Log("A3DRenderTargetMan::RentRenderTargetColor, [%d x %d, format: %d]", iWid, iHei, fmtColor);

	return pRT;
}

A3DRenderTarget* A3DRenderTargetMan::RentRenderTargetDepth(int iWid, int iHei, A3DFORMAT fmtDepth)
{
	int i;

	//	The real format will be used
	A3DFORMAT fmtReal = fmtDepth;
	if (fmtReal == A3DFMT_UNKNOWN)
		fmtReal = m_pA3DDevice->GetDepthFormat();

	//	Check there is a RT with same properties can be rent
	for (i=0; i < m_aRentableRTs.GetSize(); i++)
	{
		A3DRenderTarget* pRT = m_aRentableRTs[i];
		if (pRT->GetDepthSurface()->m_pD3DSurface)
		{
			const A3DRenderTarget::RTFMT& rtFmt = pRT->GetFormat();
			if (rtFmt.iWidth == iWid && rtFmt.iHeight == iHei && pRT->GetDepthFormat() == fmtReal)
			{
				//	All properties match
				m_aRentableRTs.RemoveAtQuickly(i);
				return pRT;
			}
		}
	}

	//	Release long-time not used target
	ReleaseIdleRenderTarget(false);

	//	Create a new render target
	A3DRenderTarget::RTFMT rtFmt;
	rtFmt.iWidth	= iWid;
	rtFmt.iHeight	= iHei;
	rtFmt.fmtTarget	= A3DFMT_A8R8G8B8;	//	Has no effect
	rtFmt.fmtDepth	= fmtDepth;

	A3DRenderTarget* pRT = new A3DRenderTarget;
	if (!pRT || !pRT->Init(m_pA3DDevice, rtFmt, false, true, false))
//	if (!pRT || !pRT->Init(m_pA3DDevice, rtFmt, false, true, A3DMULTISAMPLE_NONE, 0))
	{
		A3DRELEASE(pRT);
		g_A3DErrLog.Log("A3DRenderTargetMan::RentRenderTargetDepth, failed to create render target !");
		return NULL;
	}

	//	Mark rent time
	pRT->m_dwRentTime = a_GetTime();

	m_iRentableRTNum++;

	//	Test code...
//	g_A3DErrLog.Log("A3DRenderTargetMan::RentRenderTargetDepth, [%d x %d, format: %d]", iWid, iHei, fmtDepth);

	return pRT;
}

A3DCubeRenderTarget* A3DRenderTargetMan::RentRenderTargetCube(int iEdgeSize, A3DFORMAT fmtColor)
{
	int i;
	
	//	Check there is a RT with same properties can be rent
	for (i=0; i < m_aRentableCubeRTs.GetSize(); i++)
	{
		A3DCubeRenderTarget* pRT = m_aRentableCubeRTs[i];
		if (iEdgeSize == pRT->GetEdgeSize() && fmtColor == pRT->GetFormat())
		{
			//	All properties match
			m_aRentableCubeRTs.RemoveAtQuickly(i);
			return pRT;
		}
	}

	//	Release long-time not used target
	ReleaseIdleRenderTarget(true);

	//	Create a new render target
	A3DCubeRenderTarget* pRT = new A3DCubeRenderTarget;
	if (!pRT || !pRT->Init(m_pA3DDevice, iEdgeSize, fmtColor))
	{
		A3DRELEASE(pRT);
		g_A3DErrLog.Log("A3DRenderTargetMan::RentRenderTargetCube, failed to create render target !");
		return NULL;
	}

	//	Mark rent time
	pRT->m_dwRentTime = a_GetTime();

	m_iRentableCubeRTNum++;

	return pRT;
}

//	Return render target to manager
void A3DRenderTargetMan::ReturnRenderTarget(A3DRenderTarget* pRT)
{
	if (pRT)
	{
		//	Is a rentable render target ?
		if (!pRT->m_dwRentTime)
		{
			ASSERT(pRT && pRT->m_dwRentTime);
			return;
		}

		//	Update rent time
		pRT->m_dwRentTime = a_GetTime();

		m_aRentableRTs.UniquelyAdd(pRT);
	}
}

void A3DRenderTargetMan::ReturnCubeRenderTarget(A3DCubeRenderTarget* pRT)
{
	if (pRT)
	{
		//	Is a rentable render target ?
		if (!pRT->m_dwRentTime)
		{
			ASSERT(pRT && pRT->m_dwRentTime);
			return;
		}

		//	Update rent time
		pRT->m_dwRentTime = a_GetTime();

		m_aRentableCubeRTs.UniquelyAdd(pRT);
	}
}

//	Release all rentable render targets that are not rent. This method can be used to release
//	redundant render targets resources.
void A3DRenderTargetMan::ClearRentableRenderTargets()
{
	int i;

	//	Release normal render targets
	for (i=0; i < m_aRentableRTs.GetSize(); i++)
	{
		A3DRenderTarget* pRT = m_aRentableRTs[i];
		A3DRELEASE(pRT);
		m_iRentableRTNum--;
	}

	m_aRentableRTs.RemoveAll(false);

	//	Release cube render targets
	for (i=0; i < m_aRentableCubeRTs.GetSize(); i++)
	{
		A3DCubeRenderTarget* pRT = m_aRentableCubeRTs[i];
		A3DRELEASE(pRT);
		m_iRentableCubeRTNum--;
	}

	m_aRentableCubeRTs.RemoveAll(false);
}

//	Get current (the top most) color render target's size
//	bColorRT: true, get color render target's size; false, get depth render target's size
void A3DRenderTargetMan::GetCurRTSize(bool bColorRT, int& iWidth, int& iHeight, int iRTIndex/* 0 */)
{
	RTStack* pStack = m_aRTStacks[iRTIndex];
	RT_SURFS surfs = pStack->Peek();

	A3DSurfaceProxy* pSurf = bColorRT ? surfs.pTargetBuffer : surfs.pDepthBuffer;

	if (!pSurf)
	{
		iWidth = 0;
		iHeight = 0;
	}
	else
	{
		D3DSURFACE_DESC desc;
		pSurf->m_pD3DSurface->GetDesc(&desc);
		iWidth = (int)desc.Width;
		iHeight = (int)desc.Height;
	}
}

//	Remove render target that was long-time not rented
void A3DRenderTargetMan::ReleaseIdleRenderTarget(bool bCubeRT)
{
	DWORD dwTime = a_GetTime();
	DWORD dwMaxTime = 10000;	//	10s

	//	Because render target is always added at list tail, so the first
	//	one in list must have the longest idle time
	if (bCubeRT)
	{
		if (!m_aRentableCubeRTs.GetSize())
			return;

		A3DCubeRenderTarget* pRT = m_aRentableCubeRTs[0];
		if (dwTime >= pRT->m_dwRentTime + dwMaxTime)
		{
			pRT->Release();
			delete pRT;
			m_aRentableCubeRTs.RemoveAt(0);
			m_iRentableCubeRTNum--;
		}
	}
	else
	{
		if (!m_aRentableRTs.GetSize())
			return;

		A3DRenderTarget* pRT = m_aRentableRTs[0];
		if (dwTime >= pRT->m_dwRentTime + dwMaxTime)
		{
			pRT->Release();
			delete pRT;
			m_aRentableRTs.RemoveAt(0);
			m_iRentableRTNum--;
		}
	}
}


