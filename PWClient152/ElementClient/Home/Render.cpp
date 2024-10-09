/*
 * FILE: Render.cpp
 *
 * DESCRIPTION: Render class
 *
 * CREATED BY: Duyuxin, 2004/4/2
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "Render.h"
#include <A3DEngine.h>
#include <A3DTypes.h>
#include <A3DDevice.h>
#include <A3DLightMan.h>
#include <A3DCamera.h>
#include <A3DOrthoCamera.h>
#include <A3DViewport.h>
#include <A3DLight.h>
#include <A3DFont.h>
#include <A3DGDI.h>
#include <A3DFuncs.h>

#ifdef AUTO_TERRAIN_EDITOR
#include <AMSoundEngine.h>
#else
#include "EC_Game.h"
#include "EC_Viewport.h"
#include "EC_Global.h"
#endif

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

#ifdef AUTO_TERRAIN_EDITOR
	m_pA3DCamera	= NULL;
	m_pA3DOrthoCamera = NULL;
	m_pA3DViewport	= NULL;
	m_pFont			= NULL;
	m_hInstance		= NULL;
	m_hRenderWnd	= NULL;
	m_bEngineOK		= false;
#else
	m_pECViewport	= NULL;
#endif
}

#ifdef AUTO_TERRAIN_EDITOR
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

	DWORD dwDevFlags = A3DDEV_ALLOWMULTITHREAD;// | A3DDEV_FORCESOFTWARETL;	//	Force software T&L

	if (!m_pA3DEngine->Init(m_hInstance, m_hRenderWnd, &devFmt, dwDevFlags))
	{
		return false;
	}

	m_pA3DDevice = m_pA3DEngine->GetA3DDevice();

	//	Add a directional light into scene
	m_pA3DEngine->GetA3DLightMan()->CreateDirecionalLight(1, &m_pDirLight, A3DVECTOR3(0.0f, 0.0f, 0.0f),
					A3DVECTOR3(0.0f, -0.5f, 1.0f), A3DCOLORVALUE(0.8f, 0.8f, 0.9f, 1.0f), 
					A3DCOLORVALUE(1.0f, 1.0f, 1.0f, 1.0f), A3DCOLORVALUE(0.0f, 0.0f, 0.0f, 1.0f));

	//	Set some values
	m_pA3DDevice->SetAmbient(A3DCOLORRGB(150, 150, 180));
	m_pA3DDevice->SetFogStart(0.0f);
	m_pA3DDevice->SetFogEnd(5000.0f);
	m_pA3DDevice->SetFogEnable(false);

	//	Get system font
	m_pFont = m_pA3DEngine->GetSystemFont();

	//	Create perspective camera
	if (!(m_pA3DCamera = new A3DCamera))
	{
		return false;
	}

	if (!m_pA3DCamera->Init(g_Render.GetA3DDevice(), DEG2RAD(56.0f), 0.1f, 2000.0f))
	{
		return false;
	}
	m_pA3DCamera->SetPos(A3DVECTOR3(0.0, 2.0, -10.0));
	m_pA3DCamera->SetDirAndUp(A3DVECTOR3(0.0, 0.0, 1.0), A3DVECTOR3(0.0, 1.0, 0.0));

	//	Create ortho camera
	if (!(m_pA3DOrthoCamera = new A3DOrthoCamera))
	{
		return false;
	}

	float fW = Rect.right;
	float fH = Rect.bottom;

	if (!m_pA3DOrthoCamera->Init(g_Render.GetA3DDevice(), 
				-fW, fW, -fH, fH, -ORTHO_RANGE, ORTHO_RANGE))
	{
		return false;
	}
	m_pA3DOrthoCamera->SetPos(A3DVECTOR3(0.0, 50.0, 0.0));
	m_pA3DOrthoCamera->SetDirAndUp(A3DVECTOR3(0.0, -1.0, 0.0), A3DVECTOR3(0.0, 0.0, 1.0));
	float fRange = m_pA3DOrthoCamera->GetRight();
	float fTop = fRange * m_iRenderHei / m_iRenderWid;
	m_pA3DOrthoCamera->SetProjectionParam(-fRange, fRange, -fTop, fTop, -ORTHO_RANGE, ORTHO_RANGE);

	//	Create main Viewport
	RECT rc;
	::GetClientRect(m_hRenderWnd, &rc);

	if (!m_pA3DDevice->CreateViewport(&m_pA3DViewport, 0, 0, rc.right, rc.bottom,
									0.0f, 1.0f, true, true, 0xff404040))//0xffdbe1f9))
	{
		return false;
	}

	m_pA3DViewport->SetCamera(m_pA3DCamera);
//	m_pA3DViewport->SetCamera(m_pA3DOrthoCamera);

	//	Below line ensure the positions of 3D sounds in world are correct.
	m_pA3DCamera->SetAM3DSoundDevice(m_pA3DEngine->GetAMSoundEngine()->GetAM3DSoundDevice());

	m_bEngineOK = true;

	return true;
}

//	Release A3D engine
void CRender::ReleaseA3DEngine()
{
	if (m_pA3DEngine && m_pDirLight)
	{
		m_pA3DEngine->GetA3DLightMan()->ReleaseLight(m_pDirLight);
		m_pDirLight = NULL;
	}
	A3DRELEASE(m_pA3DCamera);
	A3DRELEASE(m_pA3DOrthoCamera);
	A3DRELEASE(m_pA3DViewport);
	A3DRELEASE(m_pA3DEngine);

	m_bEngineOK = false;
}

/*	Resize device

	Return true for success, otherwise return false.

	iWid, iHei: new size of render area
*/
bool CRender::ResizeDevice(int iWid, int iHei)
{
	if (!m_bEngineOK)
		return true;

//	m_pA3DEngine->SetDisplayMode(iWid, iHei, A3DFMT_UNKNOWN, 0, SDM_WIDTH | SDM_HEIGHT);
	m_pA3DEngine->SetDisplayMode(iWid, iHei, A3DFMT_UNKNOWN, true, false, SDM_WIDTH | SDM_HEIGHT);

	m_iRenderWid = iWid;
	m_iRenderHei = iHei;

	ASSERT(m_pA3DViewport && m_pA3DCamera);

	A3DVIEWPORTPARAM Params;
	Params.X		= 0;
	Params.Y		= 0;
	Params.Width	= iWid;
	Params.Height	= iHei;
	Params.MinZ		= 0.0f;
	Params.MaxZ		= 1.0f;

	m_pA3DViewport->SetParam(&Params);

	//	Adjust camera
	float fRatio = (float)iWid / iHei;
	m_pA3DCamera->SetProjectionParam(DEG2RAD(56.0f), 0.1f, 2000.0f, fRatio);

	float fRange = m_pA3DOrthoCamera->GetRight();
	float fTop = fRange * iHei / iWid;

	m_pA3DOrthoCamera->SetProjectionParam(-fRange, fRange, -fTop, fTop, -ORTHO_RANGE, ORTHO_RANGE);

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
void CRender::Draw2DRect(const ARectI& rc, DWORD dwCol, bool bFrame)
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
#else
/*	Initialize game

	Return true for success, otherwise return false.

*/
bool CRender::Init()
{
	m_pA3DEngine	= g_pGame->GetA3DEngine();
	m_pA3DDevice	= g_pGame->GetA3DDevice();
	m_pECViewport	= g_pGame->GetViewport();
	m_pDirLight		= g_pGame->GetDirLight();

	return true;
}

A3DCamera* CRender::GetA3DCamera() 
{ 
	return m_pECViewport->GetA3DCamera(); 
}

A3DOrthoCamera* CRender::GetA3DOrthoCamera() 
{ 
	return m_pECViewport->GetA3DOrthoCamera(); 
}

A3DViewport* CRender::GetA3DViewport()
{ 
	return m_pECViewport->GetA3DViewport(); 
}
#endif