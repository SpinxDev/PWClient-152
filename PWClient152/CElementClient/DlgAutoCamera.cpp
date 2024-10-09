/*
 * FILE: DlgAutoCamera.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Liyi, 2006/5/19
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

/*
#include "DlgAutoCamera.h"
#include "DlgAutoTools.h"

#include "Render.h"

#include "AUI\\AUIStillImageButton.h"
#include "AUI\\AUISlider.h"
#include "AUI\\AUIDef.h"

#include "A3DCamera.h"
#include "DlgAutoSliderBound.h"

AUI_BEGIN_COMMAND_MAP(CDlgAutoCamera, CDlgHomeBase)

AUI_ON_COMMAND("MoveSlider", OnCommandMoveSlider)
AUI_ON_COMMAND("Camera_*", OnCommandCamera)

AUI_END_COMMAND_MAP()


AUI_BEGIN_EVENT_MAP(CDlgAutoCamera, CDlgHomeBase)


AUI_END_EVENT_MAP()


CDlgAutoCamera::CDlgAutoCamera()
{
	for( int i = 0; i < BTN_NUM; i++)
	{
		m_pBtn[i] = NULL;
	}
	m_pSliderCameraSpeed = NULL;
	m_nSliderCameraSpeed = 0;
}

CDlgAutoCamera::~CDlgAutoCamera()
{
}


void CDlgAutoCamera::OnShowDialog()
{
	this->SetCanMove(false);

}
bool CDlgAutoCamera::OnInitContext()
{
	if( !InitResource())
		return false;
	
	InitVars();

	LoadDefault();

	SetSliderBound();
	SetLabelsText();

	UpdateData(false);
	
	return true;
}

#define GET_BTN_POINTER(a) DDX_Control( #a, m_pBtn[a]);

void CDlgAutoCamera::DoDataExchange(bool bSave)
{
	
	GET_BTN_POINTER(BTN_CAMERA_1);
	GET_BTN_POINTER(BTN_CAMERA_2);
	GET_BTN_POINTER(BTN_CAMERA_3);
	GET_BTN_POINTER(BTN_CAMERA_4);
	GET_BTN_POINTER(BTN_CAMERA_5);
	GET_BTN_POINTER(BTN_CAMERA_6);
	GET_BTN_POINTER(BTN_CAMERA_7);
	GET_BTN_POINTER(BTN_CAMERA_8);
	GET_BTN_POINTER(BTN_CAMERA_9);
	GET_BTN_POINTER(BTN_CAMERA_0);
	
	
	DDX_Control("Slider_Camera_Speed", m_pSliderCameraSpeed);
	DDX_Slider(bSave, "Slider_Camera_Speed", m_nSliderCameraSpeed);
}

void CDlgAutoCamera::LoadDefault()
{
	for( int i = 0; i < BTN_NUM; i++)
	{
		m_pBtn[i]->Enable(false);
	}
	m_nSliderCameraSpeed = 100;
}

bool CDlgAutoCamera::InitResource()
{
	return true;
}

void CDlgAutoCamera::InitVars()
{
	for( int i = 0; i < BTN_NUM; i++)
	{
		m_CameraPosAndDir[i].Reset();
	}

}

void CDlgAutoCamera::SaveCameraPosAndDir(int i_nIdx, const A3DVECTOR3 &i_vPos, const A3DVECTOR3 &i_vDir)
{
	ASSERT( i_nIdx >= 0 && i_nIdx < BTN_NUM);
	
	m_CameraPosAndDir[i_nIdx].vPos  = i_vPos;
	m_CameraPosAndDir[i_nIdx].vDir = i_vDir;

	m_pBtn[i_nIdx]->Enable(true);
	
}

void CDlgAutoCamera::SetLabelsText()
{
	
}
void CDlgAutoCamera::SetSliderBound()
{
	m_pSliderCameraSpeed->SetTotal(MAX_CAMERA_SPEED, MIN_CAMERA_SPEED);
}


void CDlgAutoCamera::OnCommandMoveSlider(const char* szCommand)
{
	UpdateData(true);
	
	AUTOTERRAINCONFIG* pConfig = g_TerrainConfig.GetConfig();
	pConfig->fCameraSpeed = m_nSliderCameraSpeed * 0.01f;

}

void CDlgAutoCamera::OnCommandCamera(const char* szCommand)
{
	int nStrLen  = strlen(szCommand);
	const char* pIdx = &szCommand[nStrLen-1];
	int nIdx = atoi(pIdx);

	ASSERT(nIdx >= 0 && nIdx < BTN_NUM);

	A3DVECTOR3 vPos = m_CameraPosAndDir[nIdx].vPos;
	A3DVECTOR3 vDir = m_CameraPosAndDir[nIdx].vDir;

	g_Render.GetA3DCamera()->SetPos(vPos);
	g_Render.GetA3DCamera()->SetDirAndUp(vDir, A3DVECTOR3(0.0f, 1.0f, 0.0f));
}

void CDlgAutoCamera::AddCameraSpeed(int i_nDelta)
{
	m_nSliderCameraSpeed += i_nDelta;
	Clamp(m_nSliderCameraSpeed, MAX_CAMERA_SPEED, MIN_CAMERA_SPEED);
	UpdateData(false);

	AUTOTERRAINCONFIG* pConfig = g_TerrainConfig.GetConfig();
	pConfig->fCameraSpeed = m_nSliderCameraSpeed * 0.01f;
	return;
}

void CDlgAutoCamera::DealWithMouseWheelChangeCameraSpeed(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if( uMsg != WM_MOUSEWHEEL)
		return;

	if(GetHomeDlgsMgr()->m_pDlgAutoTools->IsInTesting())
		return;

	if((short)HIWORD(wParam) > 0)
	{
		AddCameraSpeed(-10);
	}
	else
	{
		AddCameraSpeed(10);
	}
}
*/