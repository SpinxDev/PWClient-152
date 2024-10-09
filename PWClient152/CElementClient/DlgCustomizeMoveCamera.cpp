/*
 * FILE: DlgCustomizeMoveCamera.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Liyi, 2005/8/25
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "DlgCustomizeMoveCamera.h"
#include "EC_CustomizeMgr.h"

#include "EC_Game.h"
#include "EC_Face.h"
#include "AUIImagePicture.h"
#include "AUIListBox.h"
#include "FaceBoneController.h"
#include "EC_Game.h"
#include "EC_UIManager.h"
#include "EC_GameRun.h"
#include "EC_LoginUIMan.h"
#include "A3DCamera.h"
#include "EC_Viewport.h"


//----------------------------------------------------------------------
CDlgCustomizeMoveCamera::CDlgCustomizeMoveCamera()
{
	//Init ddx control
	m_pBtnCameraMoveFront = NULL;
	m_pBtnCameraMoveBack = NULL;
	m_pBtnCameraMoveLeft = NULL;
	m_pBtnCameraMoveRight = NULL;
	
	m_pBtnCameraTurnLeft = NULL;
	m_pBtnCameraTurnRight = NULL;

	m_pBtnCameraMoveUp = NULL;
	m_pBtnCameraMoveDown = NULL;
}

//----------------------------------------------------------------------
CDlgCustomizeMoveCamera::~CDlgCustomizeMoveCamera()
{
}

//----------------------------------------------------------------------
bool CDlgCustomizeMoveCamera::OnInitContext()
{
	if (!CDlgCustomizeBase::OnInitContext())
		return false;
	
	return true;
}

//----------------------------------------------------------------------
void CDlgCustomizeMoveCamera::DoDataExchange(bool bSave)
{
	m_pBtnCameraMoveFront = dynamic_cast<AUIStillImageButton *>(GetDlgItem("Btn_CameraMoveFront"));
	m_pBtnCameraMoveBack = dynamic_cast<AUIStillImageButton *>(GetDlgItem("Btn_CameraMoveBack"));
	m_pBtnCameraTurnLeft = dynamic_cast<AUIStillImageButton *>(GetDlgItem("Btn_CameraTurnLeft"));
	m_pBtnCameraTurnRight = dynamic_cast<AUIStillImageButton *>(GetDlgItem("Btn_CameraTurnRight"));

	m_pBtnCameraMoveUp = dynamic_cast<AUIStillImageButton *>(GetDlgItem("Btn_CameraMoveUp"));
	m_pBtnCameraMoveDown = dynamic_cast<AUIStillImageButton *>(GetDlgItem("Btn_CameraMoveDown"));
	m_pBtnCameraMoveLeft = dynamic_cast<AUIStillImageButton *>(GetDlgItem("Btn_CameraMoveLeft"));
	m_pBtnCameraMoveRight = dynamic_cast<AUIStillImageButton *>(GetDlgItem("Btn_CameraMoveRight"));
}

//----------------------------------------------------------------------
void CDlgCustomizeMoveCamera::OnShowDialog()
{
	CDlgCustomizeBase::OnShowDialog();
}

//----------------------------------------------------------------------
void CDlgCustomizeMoveCamera::OnTick()
{
	static float fDeltaDist = 0.02f;
	static float fDeltaDist2 = 0.002f;

	A3DCamera * pCamera = GetGame()->GetViewport()->GetA3DCamera();
	
	if( m_pBtnCameraMoveFront && m_pBtnCameraMoveFront->GetState() == AUISTILLIMAGEBUTTON_STATE_CLICK)
	{
	

		if( GetCustomizeMan()->m_fMoveZDist < 1.0f)
		{
			GetCustomizeMan()->m_fMoveZDist += fDeltaDist;
			pCamera->MoveFront(fDeltaDist);
		}

	}
	
	if( m_pBtnCameraMoveBack && m_pBtnCameraMoveBack->GetState() == AUISTILLIMAGEBUTTON_STATE_CLICK)
	{
	
		if( GetCustomizeMan()->m_fMoveZDist > -1.0f)
		{
			GetCustomizeMan()->m_fMoveZDist -= fDeltaDist;
			pCamera->MoveBack(fDeltaDist);
		}
			
	}

	
	if( m_pBtnCameraMoveLeft && m_pBtnCameraMoveLeft->GetState() == AUISTILLIMAGEBUTTON_STATE_CLICK)
	{
		if( GetCustomizeMan()->m_pBtnChoosePreCustomize->IsPushed())
		{
			if( GetCustomizeMan()->m_fMoveXDist < 1.0f)
			{
				GetCustomizeMan()->m_fMoveXDist += fDeltaDist;
				pCamera->MoveLeft(fDeltaDist);
			}
		}
		else
		{
			if( GetCustomizeMan()->m_fMoveXDist < 1.0f)
			{
				GetCustomizeMan()->m_fMoveXDist += fDeltaDist2;
				pCamera->MoveLeft(fDeltaDist2);
			}
		}
	}

	if( m_pBtnCameraMoveRight && m_pBtnCameraMoveRight->GetState() == AUISTILLIMAGEBUTTON_STATE_CLICK)
	{
		if( GetCustomizeMan()->m_pBtnChoosePreCustomize->IsPushed())
		{
			if( GetCustomizeMan()->m_fMoveXDist > -1.0f)
			{
				GetCustomizeMan()->m_fMoveXDist -= fDeltaDist;
				pCamera->MoveRight(fDeltaDist);
			}
		}
		else
		{
			if( GetCustomizeMan()->m_fMoveXDist > -1.0f)
			{
				GetCustomizeMan()->m_fMoveXDist -= fDeltaDist2;
				pCamera->MoveRight(fDeltaDist2);
			}
		}
		
	}
	if( m_pBtnCameraMoveUp && m_pBtnCameraMoveUp->GetState() == AUISTILLIMAGEBUTTON_STATE_CLICK)
	{
		if( GetCustomizeMan()->m_pBtnChoosePreCustomize->IsPushed())
		{
			if( GetCustomizeMan()->m_fMoveYDist < 1.0f)
			{
				GetCustomizeMan()->m_fMoveYDist += fDeltaDist;
				pCamera->Move(A3DVECTOR3(0.0f, fDeltaDist, 0.0f));
					
			}
		}
		else
		{
			if( GetCustomizeMan()->m_fMoveYDist < 1.0f)
			{
				GetCustomizeMan()->m_fMoveYDist += fDeltaDist2;
				pCamera->Move(A3DVECTOR3(0.0f, fDeltaDist2, 0.0f));
					
			}

		}
	}
	if( m_pBtnCameraMoveDown && m_pBtnCameraMoveDown->GetState() == AUISTILLIMAGEBUTTON_STATE_CLICK)
	{
		if( GetCustomizeMan()->m_pBtnChoosePreCustomize->IsPushed())
		{
			if( GetCustomizeMan()->m_fMoveYDist > -0.3f)
			{
				GetCustomizeMan()->m_fMoveYDist -= fDeltaDist;
				pCamera->Move(A3DVECTOR3(0.0f, -fDeltaDist, 0.0f));
			}
		}
		else
		{
			if( GetCustomizeMan()->m_fMoveYDist > -0.3f)
			{
				GetCustomizeMan()->m_fMoveYDist -= fDeltaDist2;
				pCamera->Move(A3DVECTOR3(0.0f, -fDeltaDist2, 0.0f));
			}
		}
	}

	
	if( m_pBtnCameraTurnLeft && m_pBtnCameraTurnLeft->GetState() == AUISTILLIMAGEBUTTON_STATE_CLICK)
	{
		A3DMATRIX4 matRotate = RotateY(0.015f);

		A3DVECTOR3 vecDir = GetPlayer()->GetDir();
		A3DVECTOR3 vecUp = GetPlayer()->GetUp();
		vecDir = a3d_VectorMatrix3x3(vecDir, matRotate);
		vecUp = a3d_VectorMatrix3x3(vecUp, matRotate);

		GetPlayer()->ChangeModelMoveDirAndUp(vecDir, vecUp);
	}

	if( m_pBtnCameraTurnRight && m_pBtnCameraTurnRight->GetState() == AUISTILLIMAGEBUTTON_STATE_CLICK)
	{
		A3DMATRIX4 matRotate = RotateY(-0.015f);

		A3DVECTOR3 vecDir = GetPlayer()->GetDir();
		A3DVECTOR3 vecUp = GetPlayer()->GetUp();
		vecDir = a3d_VectorMatrix3x3(vecDir, matRotate);
		vecUp = a3d_VectorMatrix3x3(vecUp, matRotate);

		GetPlayer()->ChangeModelMoveDirAndUp(vecDir, vecUp);
	}
		
}
