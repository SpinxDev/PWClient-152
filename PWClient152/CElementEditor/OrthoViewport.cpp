/*
 * FILE: OrthoViewport.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/6/14
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "Global.h"
#include "OrthoViewport.h"
#include "ViewFrame.h"
#include "Render.h"
#include "ElementMap.h"

#include "A3D.h"
#include "AC.h"

//#define new A_DEBUG_NEW

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

#define VIEW_RANGE	3000.0f

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
//	Implement COrthoViewport
//	
///////////////////////////////////////////////////////////////////////////

COrthoViewport::COrthoViewport(int iType)
{
	m_iType			= iType;
	m_pA3DCamera	= NULL;
	m_bRDrag		= false;
}

COrthoViewport::~COrthoViewport()
{
}

//	Initialize object
bool COrthoViewport::Init(CViewFrame* pViewFrame, int iWidth, int iHeight)
{
	if (!CViewport::Init(pViewFrame, iWidth, iHeight))
		return false;

	//	Create camera
	if (!(m_pA3DCamera = new A3DOrthoCamera))
	{
		g_Log.Log("COrthoViewport::Init: Failed to create camera object!");
		return false;
	}

	if (!m_pA3DCamera->Init(g_Render.GetA3DDevice(), -100.0f, 100.0f, -100.0f, 100.0f, -100.0f, 100.0f))
	{
		g_Log.Log(0, "COrthoViewport::Init: Failed to initialize camera.");
		return false;
	}

	m_pA3DCamera->SetPos(g_vOrigin);

	if (m_iType == TYPE_XZ)
	{
		m_pA3DCamera->SetDirAndUp(-g_vAxisY, g_vAxisZ);
		m_pA3DCamera->SetProjectionParam(-100, 100, -100, 100, -VIEW_RANGE, VIEW_RANGE);
	}else if (m_iType == TYPE_XY)
	{
			m_pA3DCamera->SetDirAndUp(g_vAxisZ, g_vAxisY);
			m_pA3DCamera->SetProjectionParam(-100, 100, -100, 100, 0, VIEW_RANGE);
	}else if (m_iType == TYPE_YZ)
	{
		m_pA3DCamera->SetDirAndUp(-g_vAxisX, g_vAxisY);
		m_pA3DCamera->SetProjectionParam(-100, 100, -100, 100, 0, VIEW_RANGE);
	}

	m_pA3DCameraBase = m_pA3DCamera;
	
	
	return true;
}

//	Release object
void COrthoViewport::Release()
{
	A3DRELEASE(m_pA3DCamera);
}

//	Resize viewport
bool COrthoViewport::Resize(int cx, int cy)
{
	CViewport::Resize(cx, cy);

	if (m_pA3DCamera)
	{
		float fRange = m_pA3DCamera->GetRight();
		float fTop = fRange * cy / cx;

		if (m_iType == VIEW_XZ)		//	Top view
			m_pA3DCamera->SetProjectionParam(-fRange, fRange, -fTop, fTop, -VIEW_RANGE, VIEW_RANGE);
		else	//	Front or left view
			m_pA3DCamera->SetProjectionParam(-fRange, fRange, -fTop, fTop, 0.0f, VIEW_RANGE);
	}

	return true;
}

//	Tick routine
bool COrthoViewport::FrameMove(DWORD dwDeltaTime)
{
	if (m_pViewFrame->HasFocus())
	{
		float fDeltaTime = GetFrameTime();

		if (m_iType != VIEW_XZ)	//	Isn't top view
		{
			if (GetAsyncKeyState('W') & 0x8000)
				MoveCameraForward(true, fDeltaTime);
			else if (GetAsyncKeyState('S') & 0x8000)
				MoveCameraForward(false, fDeltaTime);
		}

		if (GetAsyncKeyState('A') & 0x8000)
			MoveCameraRight(false, fDeltaTime);
		else if (GetAsyncKeyState('D') & 0x8000)
			MoveCameraRight(true, fDeltaTime);

		if (GetAsyncKeyState('W') & 0x8000)
			MoveCameraUp(true, fDeltaTime);
		else if (GetAsyncKeyState('S') & 0x8000)
			MoveCameraUp(false, fDeltaTime);
	}

	return true;
}

//	Render rontine
bool COrthoViewport::Render(A3DViewport* pA3DViewport)
{
	ASSERT(pA3DViewport && m_pA3DCamera);

	pA3DViewport->SetCamera(m_pA3DCamera);
	pA3DViewport->Active();
	pA3DViewport->ClearDevice();

	A3DEngine* pA3DEngine = g_Render.GetA3DEngine();
	A3DDevice* pA3DDevice = g_Render.GetA3DDevice();

	//	Set render states
	if (g_Configs.bWireFrame)
		pA3DDevice->SetFillMode(A3DFILL_WIREFRAME);
	else
		pA3DDevice->SetFillMode(A3DFILL_SOLID);

	CElementMap* pMap = m_pViewFrame->GetMap();
	if (pMap)
	{
		pMap->Render(pA3DViewport);
	}

//	g_Render.FlushCollectors();
	g_Render.GetA3DEngine()->RenderGFX(pA3DViewport, 0xffffffff);

	//	Restore render state
	pA3DDevice->SetFillMode(A3DFILL_SOLID);

	if (m_pViewFrame->GetCurrentOperation())
	{
		(m_pViewFrame->GetCurrentOperation())->Render(pA3DViewport);
	}
	return true;
}

//	Mouse action handler,
bool COrthoViewport::OnMouseMove(int x, int y, DWORD dwFlags)
{
	//Send event to operation \xqf
	if(m_pViewFrame->GetCurrentOperation())
	{
		if(!(m_pViewFrame->GetCurrentOperation())->OnMouseMove(x,y,dwFlags))
			return false;
	}
	
	if (m_bRDrag)
	{
		float fDelta = -(y - m_ptLastMou.y) * 10.0f;
		float fRange = m_pA3DCamera->GetRight() + fDelta;
		a_ClampFloor(fRange, 10.0f);

		float fTop = fRange * m_iHeight / m_iWidth;

		if (m_iType == VIEW_XZ)		//	Top view
			m_pA3DCamera->SetProjectionParam(-fRange, fRange, -fTop, fTop, -VIEW_RANGE, VIEW_RANGE);
		else	//	Front or left view
			m_pA3DCamera->SetProjectionParam(-fRange, fRange, -fTop, fTop, 0.0f, VIEW_RANGE);

		m_ptLastMou.x = x;
		m_ptLastMou.y = y;
	}

	return true;
}

bool COrthoViewport::OnLButtonDown(int x, int y, DWORD dwFlags)
{
	//Send event to operation \xqf
	if(m_pViewFrame->GetCurrentOperation())
	{
		m_pViewFrame->GetRenderWnd()->SetCapture();
		return (m_pViewFrame->GetCurrentOperation())->OnLButtonDown(x,y,dwFlags);
	}
	return true;
}

bool COrthoViewport::OnLButtonUp(int x, int y, DWORD dwFlags)
{
	//Send event to operation \xqf
	if(m_pViewFrame->GetCurrentOperation())
	{
		ReleaseCapture();
		return (m_pViewFrame->GetCurrentOperation())->OnLButtonUp(x,y,dwFlags);
	}
	return true;
}

bool COrthoViewport::OnRButtonDown(int x, int y, DWORD dwFlags)
{
	if (m_pViewFrame->HasTerrainData())
	{
		m_bRDrag = true;
		m_ptLastMou.x = x;
		m_ptLastMou.y = y;
		m_pViewFrame->GetRenderWnd()->SetCapture();
	}

	//Send event to operation \xqf
	if(m_pViewFrame->GetCurrentOperation())
	{
		if(!(m_pViewFrame->GetCurrentOperation())->OnRButtonDown(x,y,dwFlags))
			return false;
	}

	return true;
}

bool COrthoViewport::OnRButtonUp(int x, int y, DWORD dwFlags)
{
	if (m_bRDrag)
	{
		m_bRDrag = false;
		ReleaseCapture();
	}

	//Send event to operation \xqf
	if(m_pViewFrame->GetCurrentOperation())
	{
		if(!(m_pViewFrame->GetCurrentOperation())->OnRButtonUp(x,y,dwFlags))
			return false;
	}

	return true;
}

bool COrthoViewport::OnLButtonDbClk(int x, int y, DWORD dwFlags)
{
	return true;
}

bool COrthoViewport::OnRButtonDbClk(int x, int y, DWORD dwFlags)
{
	return true;
}

void COrthoViewport::OnKeyDown(DWORD dwChar, DWORD dwRepCnt, DWORD dwFlags)
{
}

void COrthoViewport::OnKeyUp(DWORD dwChar, DWORD dwRepCnt, DWORD dwFlags)
{
}



