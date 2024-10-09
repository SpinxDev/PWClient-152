/*
 * FILE: DlgAutoSwitchDlg.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Liyi, 2005/4/5
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */
/*
#include "DlgAutoSwitchDlg.h"
#include "DlgAutoBaseTerrain.h"
#include "DlgAutoLight.h"
#include "DlgAutoOutdoorTree.h"
#include "DlgAutoIndoorFurniture.h"
#include "DlgAutoCtrl.h"
#include "DlgAutoCtrlEditArea.h"
#include "DlgAutoNote2.h"


#include "EC_HomeDlgsMgr.h"

#include "AUI\\AUIStillImageButton.h"

#include "AutoTerrainCommon.h"


AUI_BEGIN_COMMAND_MAP(CDlgAutoSwitchDlg, CDlgHomeBase)

AUI_ON_COMMAND("OnSBScene", OnCommandOnSBScene)
AUI_ON_COMMAND("OnSBOutdoor", OnCommandOnSBOutdoor)
AUI_ON_COMMAND("OnSBIndoor", OnCommandOnSBIndoor)
AUI_ON_COMMAND("OnSBLighting", OnCommandOnSBLighting)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgAutoSwitchDlg, CDlgHomeBase)
AUI_END_EVENT_MAP()

CDlgAutoSwitchDlg::CDlgAutoSwitchDlg()
{
	for(int i = 0; i < SB_NUM; i++)
	{
		m_pBtn[i] = NULL;
	}

	m_pBtnCur = NULL;
}

CDlgAutoSwitchDlg::~CDlgAutoSwitchDlg()
{
	
}

bool CDlgAutoSwitchDlg::OnInitContext()
{
	for( int i = 0; i < SB_NUM; i++)
	{
		m_pBtn[i]->SetPushed(false);
	}
	
	m_pBtnCur = m_pBtn[SB_SCENE];
	m_pBtnCur->SetPushed(true);
	
	return true;
}

void CDlgAutoSwitchDlg::OnShowDialog()
{
	this->SetCanMove(false);


}

#define GET_BTN_CTRL_PT(a) DDX_Control("BTN_" # a, m_pBtn[a]);

void CDlgAutoSwitchDlg::DoDataExchange(bool bSave)
{
	GET_BTN_CTRL_PT(SB_SCENE);
	GET_BTN_CTRL_PT(SB_OUTDOOR);
	GET_BTN_CTRL_PT(SB_INDOOR);
	GET_BTN_CTRL_PT(SB_LIGHTING);

// 	for( int i = 0;i < SB_NUM; i++)
// 	{
// 		m_pBtn[i]->SetPushLike(true);
// 	}
}

void CDlgAutoSwitchDlg::OnCommandOnSBScene(const char* szCommand)
{
	if( GetHomeDlgsMgr()->GetModifying())
		return;

	GetHomeDlgsMgr()->m_pDlgCurIndoor->Show(false);
	GetHomeDlgsMgr()->m_pDlgCurOutdoor->Show(false);
	GetHomeDlgsMgr()->m_pDlgAutoLight->Show(false);

	GetHomeDlgsMgr()->m_pDlgAutoCtrl->Show(false);
	GetHomeDlgsMgr()->m_pDlgAutoCtrlEditArea->Show(false);

	GetHomeDlgsMgr()->m_pDlgCurBase->Show(false);
	GetHomeDlgsMgr()->m_pDlgCurBase = GetHomeDlgsMgr()->m_pDlgAutoBaseTerrain;
	GetHomeDlgsMgr()->m_pDlgCurBase->Show(true);

	GetHomeDlgsMgr()->m_pBtnCurBase->SetPushed(false);
	GetHomeDlgsMgr()->m_pBtnCurBase = GetHomeDlgsMgr()->m_pBtnBaseTerrain;
	GetHomeDlgsMgr()->m_pBtnCurBase->SetPushed(true);

	GetHomeDlgsMgr()->m_pDlgAutoNote2->Show(false);

	GetHomeDlgsMgr()->HideAllAdvanceDlg();

	SetOperationType(OT_NORMAL);
	
	SwitchBtn(SB_SCENE);
}
void CDlgAutoSwitchDlg::OnCommandOnSBOutdoor(const char* szCommand)
{
	if( GetHomeDlgsMgr()->GetModifying())
		return;

	GetHomeDlgsMgr()->m_pDlgCurIndoor->Show(false);
	GetHomeDlgsMgr()->m_pDlgAutoLight->Show(false);
	GetHomeDlgsMgr()->m_pDlgCurBase->Show(false);		

	GetHomeDlgsMgr()->m_pDlgAutoCtrl->Show(false);
	GetHomeDlgsMgr()->m_pDlgAutoCtrlEditArea->Show(true);

	GetHomeDlgsMgr()->m_pDlgCurOutdoor->Show(false);
	GetHomeDlgsMgr()->m_pDlgCurOutdoor = GetHomeDlgsMgr()->m_pDlgAutoOutdoorTree;
	GetHomeDlgsMgr()->m_pDlgCurOutdoor->Show(true);

	GetHomeDlgsMgr()->m_pBtnCurOutdoor->SetPushed(false);
	GetHomeDlgsMgr()->m_pBtnCurOutdoor = GetHomeDlgsMgr()->m_pBtnOutdoorTree;
	GetHomeDlgsMgr()->m_pBtnCurOutdoor->SetPushed(true);

	GetHomeDlgsMgr()->HideAllAdvanceDlg();
	
	GetHomeDlgsMgr()->m_pDlgAutoOutdoorTree->SetPolyAndLineBtnPush(true, false);

	SetOperationType(OT_TREE_POLY);
	
	GetHomeDlgsMgr()->m_pDlgAutoNote2->Show(true);
	GetHomeDlgsMgr()->m_pDlgAutoNote2->SetNote2(CDlgAutoNote2::AT_POLY);


	SwitchBtn(SB_OUTDOOR);
}

void CDlgAutoSwitchDlg::OnCommandOnSBIndoor(const char* szCommand)
{
	if( GetHomeDlgsMgr()->GetModifying())
		return;

	GetHomeDlgsMgr()->m_pDlgCurOutdoor->Show(false);
	GetHomeDlgsMgr()->m_pDlgAutoLight->Show(false);
	GetHomeDlgsMgr()->m_pDlgCurBase->Show(false);
	

	GetHomeDlgsMgr()->m_pDlgCurIndoor->Show(false);
	GetHomeDlgsMgr()->m_pDlgCurIndoor = GetHomeDlgsMgr()->m_pDlgAutoIndoorFurniture;
	GetHomeDlgsMgr()->m_pDlgCurIndoor->Show(true);

	GetHomeDlgsMgr()->m_pBtnCurIndoor->SetPushed(false);
	GetHomeDlgsMgr()->m_pBtnCurIndoor = GetHomeDlgsMgr()->m_pBtnIndoorFurniture;
	GetHomeDlgsMgr()->m_pBtnCurIndoor->SetPushed(true);

	GetHomeDlgsMgr()->m_pDlgAutoCtrl->Show(true);
	GetHomeDlgsMgr()->m_pDlgAutoCtrlEditArea->Show(false);

	GetHomeDlgsMgr()->m_pDlgAutoNote2->Show(false);

	GetHomeDlgsMgr()->HideAllAdvanceDlg();

	SetOperationType(OT_BUILDING);

	SwitchBtn(SB_INDOOR);
}

void CDlgAutoSwitchDlg::OnCommandOnSBLighting(const char* szCommand)
{
	if( GetHomeDlgsMgr()->GetModifying())
		return;

	GetHomeDlgsMgr()->m_pDlgCurIndoor->Show(false);
	GetHomeDlgsMgr()->m_pDlgCurOutdoor->Show(false);
	GetHomeDlgsMgr()->m_pDlgCurBase->Show(false);
	GetHomeDlgsMgr()->m_pDlgAutoCtrl->Show(false);
	GetHomeDlgsMgr()->m_pDlgAutoCtrlEditArea->Show(false);

	GetHomeDlgsMgr()->m_pDlgAutoLight->Show(true);

	GetHomeDlgsMgr()->m_pDlgAutoNote2->Show(false);

	GetHomeDlgsMgr()->HideAllAdvanceDlg();

	SetOperationType(OT_LIGHT);

	SwitchBtn(SB_LIGHTING);
}

void CDlgAutoSwitchDlg::SwitchBtn(SwitchBtn_e i_Btn)
{
	m_pBtnCur->SetPushed(false);
	m_pBtnCur = m_pBtn[i_Btn];
	m_pBtnCur->SetPushed(true);
}
*/