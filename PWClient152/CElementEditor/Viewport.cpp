/*
 * FILE: Viewport.cpp
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
#include "Viewport.h"
#include "ViewFrame.h"
#include "A3D.h"

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////
//#define new A_DEBUG_NEW

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
//	Implement CViewport
//	
///////////////////////////////////////////////////////////////////////////

CViewport::CViewport()
{
	m_pViewFrame		= NULL;
	m_pA3DCameraBase	= NULL;	
	m_dwLastMove		= 0;

	m_iWidth	= 0;
	m_iHeight	= 0;
}

CViewport::~CViewport()
{
}

bool CViewport::Init(CViewFrame* pViewFrame, int iWidth, int iHeight)
{
	m_pViewFrame	= pViewFrame;
	m_iWidth		= iWidth;
	m_iHeight		= iHeight;

	return true;
}

//	Release object
void CViewport::Release()
{
}

//	Resize viewport
bool CViewport::Resize(int cx, int cy)
{
	m_iWidth	= cx;
	m_iHeight	= cy;

	return true;
}

//	Reset camera position
void CViewport::ResetCameraPos(float x, float y, float z)
{
	if (m_pA3DCameraBase)
		m_pA3DCameraBase->SetPos(A3DVECTOR3(x, y, z));
}

//	Get frame time
float CViewport::GetFrameTime()
{
	DWORD dwTime = a_GetTime();
	float fDeltaTime = (dwTime - m_dwLastMove) * 0.001f;
	if (!fDeltaTime)
		return 0.0f;

	m_dwLastMove = dwTime;
	a_ClampRoof(fDeltaTime, 0.1f);

	return fDeltaTime;
}

//	Move camera forward / backward
void CViewport::MoveCameraForward(bool bForward, float fDeltaTime)
{
	if (!m_pA3DCameraBase || !m_pViewFrame->HasTerrainData() || !fDeltaTime)
		return;

	A3DVECTOR3 vDist = m_pA3DCameraBase->GetDir() * (g_Configs.fCameraMove * fDeltaTime);

	if (bForward)
		m_pA3DCameraBase->Move(vDist);
	else
		m_pA3DCameraBase->Move(-vDist);
}

//	Move camera up / down
void CViewport::MoveCameraUp(bool bUp, float fDeltaTime)
{
	if (!m_pA3DCameraBase || !m_pViewFrame->HasTerrainData())
		return;

	A3DVECTOR3 vDist = m_pA3DCameraBase->GetUp() * (g_Configs.fCameraMove * fDeltaTime);

	if (bUp)
		m_pA3DCameraBase->Move(vDist);
	else
		m_pA3DCameraBase->Move(-vDist);
}

//	Move camera right / left
void CViewport::MoveCameraRight(bool bRight, float fDeltaTime)
{
	if (!m_pA3DCameraBase || !m_pViewFrame->HasTerrainData() || !fDeltaTime)
		return;

	A3DVECTOR3 vDist = m_pA3DCameraBase->GetRight() * (g_Configs.fCameraMove * fDeltaTime);

	if (bRight)
		m_pA3DCameraBase->Move(vDist);
	else
		m_pA3DCameraBase->Move(-vDist);
}

//	Move camera absolute up / down
void CViewport::MoveCameraAbsUp(bool bUp, float fDeltaTime)
{
	if (!m_pA3DCameraBase || !m_pViewFrame->HasTerrainData() || !fDeltaTime)
		return;

	A3DVECTOR3 vDist = g_vAxisY * (g_Configs.fCameraMove * fDeltaTime);

	if (bUp)
		m_pA3DCameraBase->Move(vDist);
	else
		m_pA3DCameraBase->Move(-vDist);
}

