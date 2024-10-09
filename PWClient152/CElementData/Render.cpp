/*
 * FILE: Render.cpp
 *
 * DESCRIPTION: Render class
 *
 * CREATED BY: Duyuxin, 2004/6/8
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */
#include "StdAfx.h"
#include "Global.h"
#include "A3D.h"
#include "Render.h"

#define new A_DEBUG_NEW

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

CRender	g_Render;

///////////////////////////////////////////////////////////////////////////
//
//	Local functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Implement CRender
//
///////////////////////////////////////////////////////////////////////////

CRender::CRender()
{
	m_pA3DEngine	= NULL;
	m_pA3DDevice	= NULL;
	m_pDirLight		= NULL;
	m_pFont			= NULL;
	m_hInstance		= NULL;
	m_hRenderWnd	= NULL;
	m_bEngineOK		= false;
}

/*	Initialize game

	Return true for success, otherwise return false.

	hInst: handle of instance.
	hWnd: handle of render window.
*/
bool CRender::Init(HINSTANCE hInst, HWND hWnd)
{
	m_hInstance		= hInst;
	m_hRenderWnd	= hWnd;

	if (!InitA3DEngine())
	{
		g_Log.Log("CRender::Init, Failed to initialize A3D engine!");
		return false;
	}

	if (!CheckDevice())
	{
		g_Log.Log("CRender::Init, Device isn't eligible !");
		return false;
	}

	return true;
}

//	Release game
void CRender::Release()
{
	ReleaseA3DEngine();
}

/*	Initialize A3D engine

	Return true for success, otherwise return false.
*/
bool CRender::InitA3DEngine()
{
	//	Init Engine
	if (!(m_pA3DEngine = new A3DEngine()))
	{
		g_Log.Log("CRender::InitA3DEngine: Not enough memory!");
		return false;
	}
	
	RECT Rect;
	GetClientRect(m_hRenderWnd, &Rect);

	m_iRenderWid = Rect.right;
	m_iRenderHei = Rect.bottom;
	
	A3DDEVFMT devFmt;
	devFmt.bWindowed	= true;
	devFmt.nWidth		= m_iRenderWid;
	devFmt.nHeight		= m_iRenderHei;
	devFmt.fmtTarget	= A3DFMT_UNKNOWN;
	devFmt.fmtDepth		= A3DFMT_UNKNOWN;

	DWORD dwDevFlags = A3DDEV_FPU_PRESERVE;// | A3DDEV_FORCESOFTWARETL;	//	Force software T&L

	if (!m_pA3DEngine->Init(m_hInstance, m_hRenderWnd, &devFmt, dwDevFlags))
	{
		g_Log.Log("CRender::InitA3DEngine: Failed to initialize A3D engine.");
		return false;
	}

	m_pA3DDevice = m_pA3DEngine->GetA3DDevice();

	//	Add a directional light into scene
	m_pA3DEngine->GetA3DLightMan()->CreateDirecionalLight(1, &m_pDirLight, A3DVECTOR3(0.0f, 0.0f, 0.0f),
					A3DVECTOR3(0.0f, -0.5f, 1.0f), A3DCOLORVALUE(0.8f, 0.8f, 0.9f, 1.0f), 
					A3DCOLORVALUE(1.0f, 1.0f, 1.0f, 1.0f), A3DCOLORVALUE(0.0f, 0.0f, 0.0f, 1.0f));

	//	Set some values
	m_pA3DDevice->SetAmbient(A3DCOLORRGB(50, 50, 50));
	m_pA3DDevice->SetFogStart(5000.0f);
	m_pA3DDevice->SetFogEnd(5000.0f);
	m_pA3DDevice->SetFogEnable(false);

	m_pFont = m_pA3DEngine->GetSystemFont();
//	m_pFont->EnableFilter(false);

	m_pA3DEngine->GetA3DSkinMan()->SetDirLight(m_pDirLight);

//	g_pA3DConfig->RT_SetShowPerformanceFlag(true);

	m_bEngineOK = true;

	return true;
}

//	Release A3D engine
void CRender::ReleaseA3DEngine()
{
	if (m_pDirLight)
	{
		m_pDirLight->Release();
		delete m_pDirLight;
		m_pDirLight = NULL;
	}

	if (m_pA3DEngine)
	{
		m_pA3DEngine->Release();
		delete m_pA3DEngine;
		m_pA3DEngine = NULL;
	}

	m_pA3DDevice = NULL;
}

//	Check device
bool CRender::CheckDevice()
{
	//	We must support pixel shader 1.1
	if (!m_pA3DDevice->TestPixelShaderVersion(1, 1))
	{
		g_Log.Log("CRender::CheckDevice, Device doesn't support pixel shader 1.1");
		return false;
	}

	//	Number of texture stage must >= 3
	if (m_pA3DDevice->GetMaxSimultaneousTextures() < 4)
	{
		g_Log.Log("CRender::CheckDevice, Device must support more than 3 texture stages");
		return false;
	}
	
	return true;
}

/*	Resize device

	Return true for success, otherwise return false.

	iWid, iHei: new size of render area
*/
bool CRender::ResizeDevice(int iWid, int iHei)
{
	if (!m_pA3DEngine)
		return false;

	m_pA3DEngine->SetDisplayMode(iWid, iHei, A3DFMT_UNKNOWN, 0, false, SDM_WIDTH | SDM_HEIGHT);

	m_iRenderWid = iWid;
	m_iRenderHei = iHei;

	return true;
}

//	Begin render
bool CRender::BeginRender()
{
	ASSERT(m_pA3DEngine);
	return m_pA3DEngine->BeginRender();
}

//	End render
bool CRender::EndRender()
{
	ASSERT(m_pA3DEngine);

//	m_pA3DEngine->DisplayPerformanceRecord();

	if (!m_pA3DEngine->EndRender())
		return false;

	return m_pA3DEngine->Present();
}

//	Output text
void CRender::TextOut(int x, int y, const ACHAR* szText, DWORD color)
{
	if (m_pFont)
		m_pFont->TextOut(x, y, szText, color);
}

//	Draw 2D rectangle
void CRender::Draw2DRect(const RECT& rc, DWORD dwCol, bool bFrame)
{
	if (bFrame)
	{
		A3DPOINT2 pt1, pt2;

		//	Top border
		pt1.Set(rc.left, rc.top);
		pt2.Set(rc.right, rc.top);
		g_pA3DGDI->Draw2DLine(pt1, pt2, dwCol);

		//	Left border
		pt1.Set(rc.left, rc.top);
		pt2.Set(rc.left, rc.bottom);
		g_pA3DGDI->Draw2DLine(pt1, pt2, dwCol);

		//	Right border
		pt1.Set(rc.right, rc.top);
		pt2.Set(rc.right, rc.bottom);
		g_pA3DGDI->Draw2DLine(pt1, pt2, dwCol);

		//	Bottom border
		pt1.Set(rc.left, rc.bottom);
		pt2.Set(rc.right, rc.bottom);
		g_pA3DGDI->Draw2DLine(pt1, pt2, dwCol);
	}
	else
	{
		A3DRECT rect(rc.left, rc.top, rc.right, rc.bottom);
		g_pA3DGDI->Draw2DRectangle(rect, dwCol);
	}
}

//	Set ambient color and diretional light's direction and color
void CRender::ChangeLight(const A3DVECTOR3& vDir, DWORD dwDirCol, DWORD dwAmbient)
{
	m_pA3DDevice->SetAmbient(dwAmbient | 0xff000000);

	if (!m_pDirLight)
		return;

	A3DLIGHTPARAM Params = m_pDirLight->GetLightparam();

	Params.Direction = a3d_Normalize(vDir);
	Params.Diffuse	 = dwDirCol | 0xff000000;
	Params.Specular	 = dwDirCol | 0xff000000;

	m_pDirLight->SetLightParam(Params);
}
