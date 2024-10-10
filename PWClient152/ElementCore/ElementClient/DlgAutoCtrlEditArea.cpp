/*
 * FILE: DlgAutoCtrlEditArea.cpp
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
#include "DlgAutoCtrlEditArea.h"
#include "AutoSelectedArea.h"
#include "Render.h"
#include "A3DViewport.h"
#include "A3DCameraBase.h"

#include "AUI\\AUIStillImageButton.h"

AUI_BEGIN_COMMAND_MAP(CDlgAutoCtrlEditArea, CDlgHomeBase)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgAutoCtrlEditArea, CDlgHomeBase)

AUI_END_EVENT_MAP()

CDlgAutoCtrlEditArea::CDlgAutoCtrlEditArea()
{
	for( int i = 0; i < CEAO_NUM; i++)
	{
		m_pBtn[i] = NULL;
	}
}

CDlgAutoCtrlEditArea::~CDlgAutoCtrlEditArea()
{
}

void CDlgAutoCtrlEditArea::OnShowDialog()
{
	SetCanMove(false);
}

bool CDlgAutoCtrlEditArea::OnInitContext()
{
	return true;
}

#define GET_BTN_CTRL_PT(a) DDX_Control("Btn_"#a, m_pBtn[a]);
void CDlgAutoCtrlEditArea::DoDataExchange(bool bSave)
{
	GET_BTN_CTRL_PT(CEAO_MOVEFRONT);
	GET_BTN_CTRL_PT(CEAO_MOVEBACK);
	GET_BTN_CTRL_PT(CEAO_MOVELEFT);
	GET_BTN_CTRL_PT(CEAO_MOVERIGHT);
	GET_BTN_CTRL_PT(CEAO_TURNLEFT);
	GET_BTN_CTRL_PT(CEAO_TURNRIGHT);
	GET_BTN_CTRL_PT(CEAO_ZOOMOUT);
	GET_BTN_CTRL_PT(CEAO_ZOOMIN);
}

void CDlgAutoCtrlEditArea::OnTick()
{
	CAutoSelectedArea* pEditArea = GetAutoScene()->GetAutoSelectedArea();
	if( pEditArea == NULL)
		return;

	float fDeltaM = 1.0f;
	float fDeltaR = 0.1f;
	float fZoomOut = 1.02f;
	float fZoomIn = 0.98f;

	int nCameraType = GetAutoScene()->GetGameraType();
	A3DVECTOR3 vDir = g_Render.GetA3DViewport()->GetCamera()->GetDir();
	A3DVECTOR3 vUp = g_Render.GetA3DViewport()->GetCamera()->GetUp();
	A3DVECTOR3 vLeft = CrossProduct(vDir,vUp);
	vLeft.Normalize();
	A3DVECTOR3 vFront = CrossProduct( vUp, vLeft);
	vFront.Normalize();

	for( int i = 0; i < CEAO_NUM; i++)
	{
		if(m_pBtn[i]->GetState() == AUISTILLIMAGEBUTTON_STATE_CLICK)
		{
			switch(i)
			{	
			case CEAO_MOVEFRONT:
				{
					if( nCameraType == CT_ORTH)
					{
						if( pEditArea->GetAreatype() == CAutoSelectedArea::AT_POLY)
						{
							pEditArea->TranslatePolyEditPoints(A3DVECTOR3(0.0f, 0.0f, fDeltaM));
						}
						else
						{
							pEditArea->TranslateLineEditPoints(A3DVECTOR3(0.0f, 0.0f, fDeltaM));
						}
					}
					else
					{
						if( pEditArea->GetAreatype() == CAutoSelectedArea::AT_POLY)
						{
							pEditArea->TranslatePolyEditPoints(vFront * fDeltaM);
						}
						else
						{
							pEditArea->TranslateLineEditPoints(vFront * fDeltaM);
						}
	
					}
				}
				break;
			case CEAO_MOVEBACK:
				{
					if( nCameraType == CT_ORTH)
					{
						if( pEditArea->GetAreatype() == CAutoSelectedArea::AT_POLY)
						{
							pEditArea->TranslatePolyEditPoints(A3DVECTOR3(0.0f, 0.0f, -fDeltaM));
						}
						else
						{
							pEditArea->TranslateLineEditPoints(A3DVECTOR3(0.0f, 0.0f, -fDeltaM));
						}

					}
					else
					{
						if( pEditArea->GetAreatype() == CAutoSelectedArea::AT_POLY)
						{
							pEditArea->TranslatePolyEditPoints(-vFront * fDeltaM);
						}
						else
						{
							pEditArea->TranslateLineEditPoints(-vFront * fDeltaM);
						}
					}
				}
				break;
			case CEAO_MOVELEFT:
				{
					if( nCameraType == CT_ORTH)
					{
						if( pEditArea->GetAreatype() == CAutoSelectedArea::AT_POLY)
						{
							pEditArea->TranslatePolyEditPoints(A3DVECTOR3(-fDeltaM,0.0f, 0.0f));
						}
						else
						{
							pEditArea->TranslateLineEditPoints(A3DVECTOR3(-fDeltaM,0.0f, 0.0f));
						}
					}
					else
					{
						if( pEditArea->GetAreatype() == CAutoSelectedArea::AT_POLY)
						{
							pEditArea->TranslatePolyEditPoints( vLeft * fDeltaM);
						}
						else
						{
							pEditArea->TranslateLineEditPoints( vLeft * fDeltaM);
						}
					}
				}
				break;
			case CEAO_MOVERIGHT:
				{
					if( nCameraType == CT_ORTH)
					{
						if( pEditArea->GetAreatype() == CAutoSelectedArea::AT_POLY)
						{
							pEditArea->TranslatePolyEditPoints(A3DVECTOR3(fDeltaM,0.0f, 0.0f));
						}
						else
						{
							pEditArea->TranslateLineEditPoints(A3DVECTOR3(fDeltaM,0.0f, 0.0f));
						}
					}
					else
					{
						if( pEditArea->GetAreatype() == CAutoSelectedArea::AT_POLY)
						{
							pEditArea->TranslatePolyEditPoints(-vLeft * fDeltaM);
						}
						else
						{	
							pEditArea->TranslateLineEditPoints(-vLeft * fDeltaM);
						}

					}
				}
				break;
			case CEAO_TURNLEFT:
				if( pEditArea->GetAreatype() == CAutoSelectedArea::AT_POLY)
				{
					pEditArea->RotatePolyEditPoints(fDeltaR);
				}
				else
				{
					pEditArea->RotateLineEditPoints(fDeltaR);
				}
				break;
			case CEAO_TURNRIGHT:
				if( pEditArea->GetAreatype() == CAutoSelectedArea::AT_POLY)
				{
					pEditArea->RotatePolyEditPoints(-fDeltaR);
				}
				else
				{
					pEditArea->RotateLineEditPoints(-fDeltaR);
				}
				break;
			case CEAO_ZOOMOUT:
				if( pEditArea->GetAreatype() == CAutoSelectedArea::AT_POLY)
				{
					pEditArea->ScalePolyEditPoints(fZoomOut);
				}
				else
				{
					pEditArea->ScaleLineEditPoints(fZoomOut);
				}
				break;
			case CEAO_ZOOMIN:
				if( pEditArea->GetAreatype() == CAutoSelectedArea::AT_POLY)
				{
					pEditArea->ScalePolyEditPoints(fZoomIn);
				}
				else
				{
					pEditArea->ScaleLineEditPoints(fZoomIn);
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
*/