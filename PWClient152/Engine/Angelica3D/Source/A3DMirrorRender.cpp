/*
 * FILE: A3DMirrorRender.cpp
 *
 * DESCRIPTION: Object Based Particle System for Angelica 3D Engine
 *
 * CREATED BY: liyi 2012, 5, 24
 *
 * HISTORY:
 *
 * Copyright (c) 2012 Archosaur Studio, All Rights Reserved.	
 */

#include "A3DMirrorRender.h"
#include "A3DViewport.h"
#include "A3DPI.h"
#include "A3DCamera.h"
#include "A3DFuncs.h"


A3DMirrorRender::A3DMirrorRender()
:m_pA3DDevice(NULL),
m_pMirrorCamera(NULL),
m_pViewport(NULL),
m_pMirrorRT(NULL),
m_bInit(false)
{
}
A3DMirrorRender::~A3DMirrorRender()
{
	Release();
}

bool A3DMirrorRender::Init(A3DDevice* pA3DDevice)
{
	Release();
	//
	m_pA3DDevice	= pA3DDevice;

	//create viewport
	const D3DPRESENT_PARAMETERS& d3dpp = m_pA3DDevice->GetD3DPP();
	m_pViewport = new A3DViewport;
	A3DVIEWPORTPARAM param;
	param.X = 0;
	param.Y = 0;
	param.Height = (int)d3dpp.BackBufferHeight;
	param.Width = (int)d3dpp.BackBufferWidth;
	param.MinZ = 0.0f;
	param.MaxZ = 1.0f;
	if (!m_pViewport||!m_pViewport->Init(m_pA3DDevice, &param, true, true, 0xFF008FFD))
	{
		g_A3DErrLog.Log("A3DMirrorRender::Init, Failed to create viewport!");
		return false;
	}

	//Create reflect camera
	m_pMirrorCamera = new A3DCamera();
	if (!m_pMirrorCamera || !m_pMirrorCamera->Init(m_pA3DDevice, DEG2RAD(56.0f), 0.2f, 1900.0f))
	{
		g_A3DErrLog.Log("A3DMirrorRender::Init, Failed to create reflect camera!"); 
		return false;
	}

	//Create reflect RT
	m_pMirrorRT = new A3DRenderTarget();
	A3DRenderTarget::RTFMT rtFmt;
	rtFmt.iHeight = 0;
	rtFmt.iWidth = 0;
	rtFmt.fmtTarget = A3DFMT_A8R8G8B8;
	rtFmt.fmtDepth = A3DFMT_UNKNOWN;

	if(!m_pMirrorRT ||!m_pMirrorRT->Init(m_pA3DDevice, rtFmt, true, true))
	{
		g_A3DErrLog.Log("A3DMirrorRender::Init, Failed to create reflect Rt!");
		return false;
	}

	m_bInit = true;
	return true;
}
void A3DMirrorRender::Release()
{
	A3DRELEASE(m_pMirrorCamera);
	A3DRELEASE(m_pViewport);
	A3DRELEASE(m_pMirrorRT);
	m_bInit = false;
}

bool A3DMirrorRender::Render(A3DViewport* pCurViewport, A3DPLANE* pMirrorPlane, LPRENDERFORMIRROR pFuncReflectCallBack, void* pArg)
{
	if( !m_bInit || !pCurViewport || !pMirrorPlane)
		return false;
	
	
	//Set mirror render target
	m_pMirrorRT->ApplyToDevice();

	//	Adjust camera parameters
	A3DCameraBase* pCurCamera = pCurViewport->GetCamera();
	if (pCurCamera->IsPerspective())
	{
		A3DCamera* pTempCamera = (A3DCamera*)pCurCamera;
		float fFOV = pTempCamera->GetFOV();
		float fRatio = pTempCamera->GetRatio();

		if (fFOV != m_pMirrorCamera->GetFOV() || fRatio != m_pMirrorCamera->GetRatio())
			m_pMirrorCamera->SetProjectionParam(fFOV, -1.0f, -1.0f, fRatio);
	}

	//	Set mirror info
	m_pMirrorCamera->SetMirror(pCurCamera, pMirrorPlane->vNormal * pMirrorPlane->fDist, pMirrorPlane->vNormal);

	A3DVIEWPORTPARAM* pVP = m_pViewport->GetParam();
	if (int(pVP->Width) != m_pMirrorRT->GetWidth() ||
		int(pVP->Height) != m_pMirrorRT->GetHeight())
	{
		pVP->Width = m_pMirrorRT->GetWidth();
		pVP->Height = m_pMirrorRT->GetHeight();
		m_pViewport->SetParam(pVP);
	}
	
	//viewport set camera
	m_pViewport->SetCamera(m_pMirrorCamera);
	m_pViewport->Active();

	m_pViewport->SetClearColor(0xffff0000);
	m_pViewport->ClearDevice();

	//	Inverse face cull
	A3DCULLTYPE cull_type = m_pA3DDevice->GetFaceCull();

	//set clip plane
	A3DMATRIX4 matVP = m_pMirrorCamera->GetVPTM();
	A3DVECTOR3 vNormal = pMirrorPlane->vNormal;
	D3DXPLANE clipPlane(vNormal.x, vNormal.y, vNormal.z, -pMirrorPlane->fDist);

	m_pA3DDevice->SetClipPlaneProgrammable(0, &clipPlane, &matVP);
	m_pA3DDevice->SetDeviceRenderState(D3DRS_CLIPPLANEENABLE, D3DCLIPPLANE0);

	//render mirror object
	if( pFuncReflectCallBack)
	{
		(*pFuncReflectCallBack)(m_pViewport, pArg);
	}

	//restore clip plane
	m_pA3DDevice->SetDeviceRenderState(D3DRS_CLIPPLANEENABLE, 0);
	m_pA3DDevice->SetFaceCull(cull_type);

	//Restore mirror render target
	m_pMirrorRT->WithdrawFromDevice();

	return true;
}

