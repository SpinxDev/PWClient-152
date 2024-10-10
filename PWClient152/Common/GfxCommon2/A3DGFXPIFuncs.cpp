/*
* FILE: A3DGFXPIFuncs.cpp
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan (zhangyachuan000899@wanmei.com), 2012/3/1
*
* HISTORY: 
*
* Copyright (c) 2012 Archosaur Studio, All Rights Reserved.
*/

#include "StdAfx.h"
#include "A3DGFXPIFuncs.h"
#include "A3DRenderTarget.h"
#include "A3DDevice.h"

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
//	Implement A3DGFXPIFuncs
//	
///////////////////////////////////////////////////////////////////////////

A3DRenderTarget* A3DGFXPIFuncs::CreateRenderTarget(A3DDevice* pDev, int iWid, int iHei, bool bTarget, bool bDepth, A3DFORMAT fmtTarget, A3DFORMAT fmtDepth)
{
	std::auto_ptr<A3DRenderTarget> pRenderTarget(new A3DRenderTarget);

#if defined _ANGELICA22 || defined _ANGELICA21

	A3DRenderTarget::RTFMT rtfmt;

	rtfmt.iWidth = iWid;
	rtfmt.iHeight = iHei;
	rtfmt.fmtTarget = fmtTarget;
	rtfmt.fmtDepth = fmtDepth;

#else // 2.0 , 2.1

	A3DDEVFMT rtfmt;
	rtfmt.nWidth = iWid;
	rtfmt.nHeight = iHei;
	rtfmt.fmtTarget = fmtTarget;
	rtfmt.fmtDepth = fmtDepth;

#endif

	if (!pRenderTarget->Init(pDev, rtfmt, bTarget, bDepth))
		return NULL;

	return pRenderTarget.release();
}

void A3DGFXPIFuncs::DestroyRenderTarget(A3DRenderTarget* pRenderTarget)
{
	A3DRELEASE(pRenderTarget);
}

bool A3DGFXPIFuncs::ApplyRenderTarget(A3DDevice* pDev, A3DRenderTarget* pRt)
{
#if defined _ANGELICA22 || defined _ANGELICA21
	if (!pRt->ApplyToDevice())
		return false;
#else	// _ANGELICA21 || _ANGELICA2
	pDev->SetRenderTarget(pRt);
#endif
	return true;
}

void A3DGFXPIFuncs::RestoreRenderTarget(A3DDevice* pDev, A3DRenderTarget* pRt)
{
#if defined _ANGELICA22 || defined _ANGELICA21
	pRt->WithdrawFromDevice();
#else	// _ANGELICA21 || _ANGELICA2
	pDev->RestoreRenderTarget();
#endif
}

bool A3DGFXPIFuncs::CopyRenderTargetToBackBuffer(A3DRenderTarget* pFrameTarget, A3DDevice* pA3DDevice)
{
#if defined _ANGELICA21
    HRESULT hval = pA3DDevice->StretchRect(pFrameTarget->GetTargetSurface()->m_pD3DSurface, NULL, pA3DDevice->GetBackBuffer()->m_pD3DSurface, NULL, A3DTEXF_NONE) ? S_OK : S_OK + 1;
#elif defined _ANGELICA22
	HRESULT hval = pA3DDevice->StretchRect(pFrameTarget->GetTargetSurface()->m_pD3DSurface, NULL, pA3DDevice->GetBackBuffer()->m_pD3DSurface, NULL, A3DTEXF_NONE);
#else	// _ANGELICA2
	HRESULT hval = pA3DDevice->GetD3DDevice()->CopyRects(pFrameTarget->GetTargetSurface(), NULL, 0, pA3DDevice->GetBackBuffer(), NULL);
#endif

	return hval == S_OK;
}

DWORD A3DGFXPIFuncs::GetSkinModelNoPixelShaderFlag() 
{
#ifdef _ANGELICA21
	//return A3DSkinModel::RAO_NOPIXELSHADER | A3DSkinModel::RAO_NOSETALPHABLEND;
	return A3DSkinModel::RAO_NOPIXELSHADER;
#elif defined _ANGELICA22
	return A3DSkinModel::RAO_NOPIXELSHADER | A3DSkinModel::RAO_NOEFFECT;
#endif
}
