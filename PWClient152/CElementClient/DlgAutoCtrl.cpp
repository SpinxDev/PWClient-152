/*
 * FILE: DlgAutoCtrl.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Liyi, 2005/2/14
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */
/*
#include "DlgAutoCtrl.h"

#include "Render.h"
#include "A3DViewport.h"
#include "A3DCameraBase.h"

#include "aui\\AUIStillImageButton.h"

#include "AutoBuildingOperation.h"

AUI_BEGIN_COMMAND_MAP(CDlgAutoCtrl, CDlgHomeBase)

AUI_ON_COMMAND("DropModel", OnCommandDropModel)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgAutoCtrl, CDlgHomeBase)

AUI_ON_EVENT("Btn_CMO_TURNLEFT", WM_LBUTTONUP, OnEventLButtonUpOnTurnLeftAndRight)
AUI_ON_EVENT("Btn_CMO_TURNRIGHT", WM_LBUTTONUP, OnEventLButtonUpOnTurnLeftAndRight)

AUI_END_EVENT_MAP()

CDlgAutoCtrl::CDlgAutoCtrl()
{
	for(int i = 0; i < CMO_NUM; i++)
	{
		m_pBtn[i] = NULL;
	}
}

CDlgAutoCtrl::~CDlgAutoCtrl()
{

}

void CDlgAutoCtrl::OnShowDialog()
{
	SetCanMove(false);
}

bool CDlgAutoCtrl::OnInitContext()
{
	return true;
}

#define GET_BTN_CTRL_PT(a) DDX_Control("Btn_"#a, m_pBtn[a]);
void CDlgAutoCtrl::DoDataExchange(bool bSave)
{
	GET_BTN_CTRL_PT(CMO_MOVEFRONT);
	GET_BTN_CTRL_PT(CMO_MOVEBACK);
	GET_BTN_CTRL_PT(CMO_MOVELEFT);
	GET_BTN_CTRL_PT(CMO_MOVERIGHT);
	GET_BTN_CTRL_PT(CMO_MOVEUP);
	GET_BTN_CTRL_PT(CMO_MOVEDOWN);
	GET_BTN_CTRL_PT(CMO_TURNLEFT);
	GET_BTN_CTRL_PT(CMO_TURNRIGHT);
}

void CDlgAutoCtrl::OnTick()
{
	if(GetHomeDlgsMgr()->GetModelState() == CECHomeDlgsMgr::MSE_HIT)
		return;

	CAutoBuildingOperation* pOpera = GetAutoScene()->GetAutoBuildingOperation();
	if( pOpera == NULL)
		return;

	float fDeltaR = 0.1f;
	float fDeltaM = 0.1f;

	int nCameraType = GetAutoScene()->GetGameraType();
	A3DVECTOR3 vDir = g_Render.GetA3DViewport()->GetCamera()->GetDir();
	A3DVECTOR3 vUp = g_Render.GetA3DViewport()->GetCamera()->GetUp();
	A3DVECTOR3 vLeft = CrossProduct(vDir,vUp);
	vLeft.Normalize();
	

	A3DVECTOR3 vFront = CrossProduct( vUp, vLeft);
	vFront.Normalize();
	vFront.y = 0.0f;

	int nSelModelIndex = pOpera->GetSelectedModelIndex();
	if( nSelModelIndex == -1)
		return;

	for( int i = 0; i < CMO_NUM; i++)
	{
		if(m_pBtn[i]->GetState() == AUISTILLIMAGEBUTTON_STATE_CLICK)
		{
			switch(i)
			{
			case CMO_MOVEFRONT:
				{
					if( nCameraType == CT_ORTH)
					{
						if (pOpera->IsInCollision(nSelModelIndex)
							|| pOpera->TestModelCollisionTranslate(nSelModelIndex, A3DVECTOR3(0.0f, 0.0f, fDeltaM)))
							break;
						pOpera->TranslateModel(A3DVECTOR3(0.0f, 0.0f, fDeltaM));
						pOpera->TranslateOthersInModel(A3DVECTOR3(0.0f, 0.0f, fDeltaM));
					}
					else
					{
						if (pOpera->IsInCollision(nSelModelIndex)
							|| pOpera->TestModelCollisionTranslate(nSelModelIndex, vFront * fDeltaM))
							break;
						pOpera->TranslateModel(vFront * fDeltaM);
						pOpera->TranslateOthersInModel(vFront * fDeltaM);
					}
				}
				break;
			case CMO_MOVEBACK:
				{
					if( nCameraType == CT_ORTH)
					{
						if (pOpera->IsInCollision(nSelModelIndex)
							|| pOpera->TestModelCollisionTranslate(nSelModelIndex, A3DVECTOR3(0.0f, 0.0f, -fDeltaM)))
							break;
						pOpera->TranslateModel(A3DVECTOR3(0.0f, 0.0f, -fDeltaM));
						pOpera->TranslateOthersInModel(A3DVECTOR3(0.0f, 0.0f, -fDeltaM));
					}
					else
					{
						if (pOpera->IsInCollision(nSelModelIndex)
							|| pOpera->TestModelCollisionTranslate(nSelModelIndex, -vFront * fDeltaM))
							break;
						pOpera->TranslateModel(-vFront * fDeltaM);
						pOpera->TranslateOthersInModel(-vFront * fDeltaM);
					}
				}
				break;
			case CMO_MOVELEFT:
				{
					if( nCameraType == CT_ORTH)
					{
						if (pOpera->IsInCollision(nSelModelIndex)
							|| pOpera->TestModelCollisionTranslate(nSelModelIndex, A3DVECTOR3(-fDeltaM, 0.0f, 0.0f)))
							break;
						pOpera->TranslateModel( A3DVECTOR3(-fDeltaM, 0.0f, 0.0f));
						pOpera->TranslateOthersInModel( A3DVECTOR3(-fDeltaM, 0.0f, 0.0f));
					}
					else
					{
						if (pOpera->IsInCollision(nSelModelIndex)
							|| pOpera->TestModelCollisionTranslate(nSelModelIndex, vLeft * fDeltaM))
							break;
						pOpera->TranslateModel(vLeft * fDeltaM);
						pOpera->TranslateOthersInModel(vLeft * fDeltaM);
					}
				}
				break;
			case CMO_MOVERIGHT:
				{
					if( nCameraType == CT_ORTH)
					{
						if (pOpera->IsInCollision(nSelModelIndex)
							|| pOpera->TestModelCollisionTranslate(nSelModelIndex, A3DVECTOR3(fDeltaM, 0.0f, 0.0f)))
							break;
						pOpera->TranslateModel( A3DVECTOR3(fDeltaM, 0.0f, 0.0f));
						pOpera->TranslateOthersInModel( A3DVECTOR3(fDeltaM, 0.0f, 0.0f));
					}
					else
					{
						if (pOpera->IsInCollision(nSelModelIndex)
							|| pOpera->TestModelCollisionTranslate(nSelModelIndex, -vLeft * fDeltaM))
							break;
						pOpera->TranslateModel(-vLeft * fDeltaM);
						pOpera->TranslateOthersInModel(-vLeft * fDeltaM);
					}
				}
				break;
			case CMO_MOVEUP:
				{
					if (pOpera->IsInCollision(nSelModelIndex)
						|| pOpera->TestModelCollisionTranslate(nSelModelIndex, A3DVECTOR3(0.0f, fDeltaM, 0.0f)))
						break;
					pOpera->TranslateModel(A3DVECTOR3(0.0f, fDeltaM, 0.0f));
					pOpera->TranslateOthersInModel(A3DVECTOR3(0.0f, fDeltaM, 0.0f));
				}
				break;
			case CMO_MOVEDOWN:
				{
					if (pOpera->IsInCollision(nSelModelIndex)
						|| pOpera->TestModelCollisionTranslate(nSelModelIndex, A3DVECTOR3(0.0f, -fDeltaM, 0.0f)))
						break;
					pOpera->TranslateModel(A3DVECTOR3(0.0f, -fDeltaM, 0.0f));
					pOpera->TranslateOthersInModel(A3DVECTOR3(0.0f, -fDeltaM, 0.0f));
				}
				break;
			case CMO_TURNLEFT:
				{			
					if (pOpera->IsInCollision(nSelModelIndex)
						|| pOpera->TestModelCollisionRotate(nSelModelIndex, fDeltaR))
						break;
					pOpera->RotateModelSelf(fDeltaR);
					pOpera->RotateOthersInModel(1, fDeltaR);
				}
				break;
			case CMO_TURNRIGHT:
				{	
					if (pOpera->IsInCollision(nSelModelIndex)
						|| pOpera->TestModelCollisionRotate(nSelModelIndex, -fDeltaR))
						break;
					pOpera->RotateModelSelf(-fDeltaR);
					pOpera->RotateOthersInModel(1, -fDeltaR);
				}
				break;
			default:
				break;

			}

			return;
		}
	}
	return;
}

void CDlgAutoCtrl::OnCommandDropModel(const char* szCommand)
{
	CAutoBuildingOperation* pOpera = GetAutoScene()->GetAutoBuildingOperation();
	if( pOpera == NULL)
		return;

	pOpera->DropCurModel();
	
	return;
}

void CDlgAutoCtrl::OnEventLButtonUpOnTurnLeftAndRight(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	int nIdx = GetAutoScene()->GetAutoBuildingOperation()->GetSelectedModelIndex();	
	GetAutoScene()->GetAutoBuildingOperation()->CalBuildingLight(nIdx, true);

	//GetAutoScene()->GetAutoBuildingOperation()->QuickLightMap();
}
*/