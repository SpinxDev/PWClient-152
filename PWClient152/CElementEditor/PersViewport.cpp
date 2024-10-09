/*
 * FILE: PersViewport.cpp
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
#include "PersViewport.h"
#include "ViewFrame.h"
#include "Render.h"
#include "Staff.h"
#include "ElementMap.h"

#include "A3D.h"
#include "AC.h"
#include "AM.h"
#include "A3DGFXExMan.h"
//#define new A_DEBUG_NEW

//extern A3DGFXExMan _gfx_ex_man;
///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

#define DEFAULTFOV	56.0f

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
//	Implement CPersViewport
//	
///////////////////////////////////////////////////////////////////////////

CPersViewport::CPersViewport()
{
	m_pA3DCamera	= NULL;
	m_bRDrag		= false;

	m_aAxisStaffs[0] = NULL;
	m_aAxisStaffs[1] = NULL;
	m_aAxisStaffs[2] = NULL;
}

CPersViewport::~CPersViewport()
{
}

//	Initialize object
bool CPersViewport::Init(CViewFrame* pViewFrame, int iWidth, int iHeight)
{
	if (!CViewport::Init(pViewFrame, iWidth, iHeight))
		return false;

	//	Create camera
	if (!(m_pA3DCamera = new A3DCamera))
	{
		g_Log.Log("CPersViewport::Init: Failed to create camera object!");
		return false;
	}

	if (!m_pA3DCamera->Init(g_Render.GetA3DDevice(), DEG2RAD(DEFAULTFOV), 0.1f, g_Configs.fViewRadius))
	{
		g_Log.Log(0, "CPersViewport::Init: Failed to initialize camera.");
		return false;
	}

	m_pA3DCamera->SetPos(g_vOrigin + g_vAxisY * 120.0f);
	m_pA3DCamera->SetDirAndUp(g_vAxisZ, g_vAxisY);

	//	Below line ensure the positions of 3D sounds in world are correct.
	m_pA3DCamera->SetAM3DSoundDevice(g_Render.GetA3DEngine()->GetAMSoundEngine()->GetAM3DSoundDevice());
	
	m_pA3DCameraBase = m_pA3DCamera;

	//	Create axis staffs
	DWORD aCols[3] = {0xffff0000, 0xff00ff00, 0xff0000ff};

	for (int i=0; i < 3; i++)
	{
		if (!(m_aAxisStaffs[i] = new CStaff))
			return false;

		m_aAxisStaffs[i]->Init(&g_Render, 6, 2.0f, 0.05f);
		m_aAxisStaffs[i]->SetColor(aCols[i]);
	}

	return true;
}

//	Release object
void CPersViewport::Release()
{
	for (int i=0; i < 3; i++)
	{
		if (m_aAxisStaffs[i])
		{
			m_aAxisStaffs[i]->Release();
			delete m_aAxisStaffs[i];
			m_aAxisStaffs[i] = NULL;
		}
	}

	A3DRELEASE(m_pA3DCamera);
}

//	Resize viewport
bool CPersViewport::Resize(int cx, int cy)
{
	CViewport::Resize(cx, cy);

	if (m_pA3DCamera)
	{
		//	Adjust camera
		float fRatio = (float)cx / cy;
		m_pA3DCamera->SetProjectionParam(DEG2RAD(DEFAULTFOV), 0.1f, 3000.0f, fRatio);
	}

	return true;
}

//	Tick routine
bool CPersViewport::FrameMove(DWORD dwDeltaTime)
{
	if(m_pViewFrame->GetCurrentOperation())
	{
		(m_pViewFrame->GetCurrentOperation())->Tick(dwDeltaTime);
	}

	if (m_pViewFrame->HasFocus())
	{
		float fDeltaTime = GetFrameTime();

		if (GetAsyncKeyState('W') & 0x8000)
			MoveCameraForward(true, fDeltaTime);
		else if (GetAsyncKeyState('S') & 0x8000)
			MoveCameraForward(false, fDeltaTime);

		if (GetAsyncKeyState('A') & 0x8000)
			MoveCameraRight(false, fDeltaTime);
		else if (GetAsyncKeyState('D') & 0x8000)
			MoveCameraRight(true, fDeltaTime);

		if (GetAsyncKeyState('Q') & 0x8000)
			MoveCameraAbsUp(true, fDeltaTime);
		else if ((GetAsyncKeyState('Z') & 0x8000) && !(GetAsyncKeyState(VK_CONTROL) & 0x8000))
			MoveCameraAbsUp(false, fDeltaTime);
	}

	return true;
}

//	Render rontine
bool CPersViewport::Render(A3DViewport* pA3DViewport)
{
	ASSERT(pA3DViewport && m_pA3DCamera);

	//	Adjust camera's position
	AdjustCameraPos();

	pA3DViewport->SetCamera(m_pA3DCamera);
	pA3DViewport->Active();
	pA3DViewport->ClearDevice();

	A3DEngine* pA3DEngine = g_Render.GetA3DEngine();
	A3DDevice* pA3DDevice = g_Render.GetA3DDevice();

	//	Set render state
	if (g_Configs.bWireFrame)
		pA3DDevice->SetFillMode(A3DFILL_WIREFRAME);
	else
		pA3DDevice->SetFillMode(A3DFILL_SOLID);

	CElementMap* pMap = m_pViewFrame->GetMap();
	if (pMap)
	{
		pMap->Render(pA3DViewport);
	}

	//DWORD fogOldStart = pA3DDevice->GetFogStart();
	//DWORD fogOldEnd = pA3DDevice->GetFogEnd();
	//DWORD fogDensity = pA3DDevice->GetFogDensity();
	//CString msg;
	//msg.Format("!!!!!!!!!!!!  Fog Start = %d, Fog End = %d, Fog Density = %d\n",fogOldStart,fogOldEnd,fogDensity);
	//TRACE(msg);
	pA3DDevice->SetFogEnable(false);
//	g_Render.FlushCollectors();
	pA3DDevice->SetFogEnable(g_Configs.bEnableFog);
	
	g_Render.GetA3DEngine()->RenderGFX(pA3DViewport, 0xffffffff);
	//_gfx_ex_man.RenderAllGfx(pA3DViewport);
	//	Restore render state
	pA3DDevice->SetFillMode(A3DFILL_SOLID);

	//	test code ...
/*	RECT rc = {100, 100, 150, 120};
	g_Render.Draw2DRect(rc, 0xffffffff, true);
	A3DRECT rcArea(rc.left+1, rc.top+1, rc.right, rc.bottom);
	int x=95, y=110;
	g_Render.GetSystemFont()->DrawText(x, y, "test font clipping, hehe, don't worry about!", 0xffffff00, &rcArea);

	rc.top += 50;
	rc.bottom += 50;
	g_Render.Draw2DRect(rc, 0xffffffff, true);
	rcArea.Offset(0, 50);
	g_Render.GetSystemFont()->DrawText(x, y+50, "test font clipping, hehe, don't worry about!", 0xffffff00, NULL);
*/
	//	Draw Operation brush at last\xqf
	if (m_pViewFrame->GetCurrentOperation())
	{
		(m_pViewFrame->GetCurrentOperation())->Render(pA3DViewport);
	}

	//	Draw axis
	RenderAxisStaffs();
	
	return true;
}

bool CPersViewport::OnMouseMove(int x, int y, DWORD dwFlags)
{
	
	//Send event to operation \xqf
	if(m_pViewFrame->GetCurrentOperation())
	{
		if(!(m_pViewFrame->GetCurrentOperation())->OnMouseMove(x,y,dwFlags))
			return false;
	}
	
	if (m_bRDrag)
	{
		float fDelta = (x - m_ptLastMou.x) * g_Configs.fCameraRot;
	//	a_Clamp(fDelta, -60.0f, 60.0f);
		m_pA3DCamera->DegDelta(fDelta);

		fDelta = -(y - m_ptLastMou.y) * g_Configs.fCameraRot;
	//	a_Clamp(fDelta, -60.0f, 60.0f);
		m_pA3DCamera->PitchDelta(fDelta);

		m_ptLastMou.x = x;
		m_ptLastMou.y = y;
	}

	return true;
}

//	Adjust camera's position
void CPersViewport::AdjustCameraPos()
{
	if (!m_pViewFrame->HasTerrainData() || !g_Configs.bAlongGround)
		return;

	CTerrain* pTerrain = m_pViewFrame->GetMap()->GetTerrain();
	A3DVECTOR3 vPos = m_pA3DCamera->GetPos();
	float fHeight = pTerrain->GetPosHeight(vPos);

	if (vPos.y < fHeight + 2.0f)
		vPos.y = fHeight + 2.0f;

	m_pA3DCamera->SetPos(vPos);
}

bool CPersViewport::OnLButtonDown(int x, int y, DWORD dwFlags)
{
	if (m_bRDrag)
	{
		m_bRDrag = false;
		ReleaseCapture();
	}

	//Send event to operation \xqf
	if(m_pViewFrame->GetCurrentOperation())
	{
		m_pViewFrame->GetRenderWnd()->SetCapture();
		return (m_pViewFrame->GetCurrentOperation())->OnLButtonDown(x,y,dwFlags);
	}
	return true; 
}

bool CPersViewport::OnLButtonUp(int x, int y, DWORD dwFlags) 
{
	//Send event to operation \xqf
	if(m_pViewFrame->GetCurrentOperation())
	{
		ReleaseCapture();
		return (m_pViewFrame->GetCurrentOperation())->OnLButtonUp(x,y,dwFlags);
	}
	return true;
}

bool CPersViewport::OnRButtonDown(int x, int y, DWORD dwFlags)
{
	//Send event to operation \xqf
	if(m_pViewFrame->GetCurrentOperation())
	{
		if(!(m_pViewFrame->GetCurrentOperation())->OnRButtonDown(x,y,dwFlags))
			return false;
	}
	
	if (m_pViewFrame->HasTerrainData())
	{
		m_bRDrag = true;
		m_ptLastMou.x = x;
		m_ptLastMou.y = y;
		m_pViewFrame->GetRenderWnd()->SetCapture();
	}

	return true; 
}

bool CPersViewport::OnRButtonUp(int x, int y, DWORD dwFlags)
{
	//Send event to operation \xqf
	if(m_pViewFrame->GetCurrentOperation())
	{
		if(!(m_pViewFrame->GetCurrentOperation())->OnRButtonUp(x,y,dwFlags))
			return false;
	}
	
	if (m_bRDrag)
	{
		m_bRDrag = false;
		ReleaseCapture();
	}

	return true; 
}

bool CPersViewport::OnLButtonDbClk(int x, int y, DWORD dwFlags)
{
	return true; 
}

bool CPersViewport::OnRButtonDbClk(int x, int y, DWORD dwFlags)
{
	return true; 
}

void CPersViewport::OnKeyDown(DWORD dwChar, DWORD dwRepCnt, DWORD dwFlags)
{
}

void CPersViewport::OnKeyUp(DWORD dwChar, DWORD dwRepCnt, DWORD dwFlags)
{
}

//	Render axis staffs
bool CPersViewport::RenderAxisStaffs()
{
	const A3DVECTOR3& vDir = m_pA3DCamera->GetDir();
	const A3DVECTOR3& vUp = m_pA3DCamera->GetUp();
	const A3DVECTOR3& vRight = m_pA3DCamera->GetRight();

	float fHalfLen = 2.0f * 0.5f;
	A3DVECTOR3 vPos = m_pA3DCamera->GetPos() - vRight * 19.0f + vDir * 30.0f - vUp * 14.0f;

	//	X axis
	CStaff* pStaff = m_aAxisStaffs[0];
	pStaff->SetPos(vPos + g_vAxisX * fHalfLen);
	pStaff->SetDirAndUp(g_vAxisX, g_vAxisY);

	//	Y axis
	pStaff = m_aAxisStaffs[1];
	pStaff->SetPos(vPos + g_vAxisY * fHalfLen);
	pStaff->SetDirAndUp(g_vAxisY, g_vAxisX);

	//	Z azis
	pStaff = m_aAxisStaffs[2];
	pStaff->SetPos(vPos + g_vAxisZ * fHalfLen);
	pStaff->SetDirAndUp(g_vAxisZ, g_vAxisY);

	g_Render.GetA3DEngine()->GetA3DFlatCollector()->Flush();

	A3DDevice* pA3DDevice = g_Render.GetA3DDevice();
	pA3DDevice->SetZTestEnable(false);

	for (int i=0; i < 3; i++)
		m_aAxisStaffs[i]->Render();

	g_Render.GetA3DEngine()->GetA3DFlatCollector()->Flush();

	pA3DDevice->SetZTestEnable(true);

	return true;
}



