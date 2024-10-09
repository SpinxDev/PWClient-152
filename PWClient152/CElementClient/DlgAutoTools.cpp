/*
 * FILE: DlgAutoTools.cpp
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
#include "DlgAutoTools.h"

#include "aui/AUIManager.h"
#include "aui/AUIStillImageButton.h"

#include "DlgAutoGenLightMapProgress.h"
#include "DlgAutoAllRes.h"
#include "DlgAutoSwitchDlg.h"
#include "DlgAutoCtrl.h"
#include "DlgAutoCamera.h"
#include "DlgAutoNote2.h"
#include "DlgAutoCtrlEditArea.h"
#include "DlgAutoLight.h"


#include "A3DMacros.h"
#include "AutoTerrainCommon.h"
#include "AutoTerrainAction.h"
#include "AutoTerrain.h"
#include "AutoBuildingOperation.h"

#include "Render.h"
#include "A3DCamera.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_World.h"

#include "SoftGouraud.h"


AUI_BEGIN_COMMAND_MAP(CDlgAutoTools, CDlgHomeBase)

AUI_ON_COMMAND("OrthCamera", OnCommandOrthCamera)
AUI_ON_COMMAND("PersCamera", OnCommandPersCamera)
AUI_ON_COMMAND("StandardView", OnCommandStandardView)


AUI_ON_COMMAND("GenLightMap", OnCommandGenLightMap)
AUI_ON_COMMAND("HideResDlg", OnCommandHideResDlg)

AUI_ON_COMMAND("Test", OnCommandTest)

AUI_ON_COMMAND("SoftGouraud", OnCommandSoftGouraud)
AUI_ON_COMMAND("UpdateScene", OnCommandUpdateScene)

//temp
AUI_ON_COMMAND("SaveScene", OnCommandSaveScene)
AUI_ON_COMMAND("LoadScene", OnCommandLoadScene)

AUI_END_COMMAND_MAP()


AUI_BEGIN_EVENT_MAP(CDlgAutoTools, CDlgHomeBase)

AUI_END_EVENT_MAP()


CDlgAutoTools::CDlgAutoTools()
{
	for( int i = 0; i < BTN_NUM; i++)
	{
		m_pBtn[i] = NULL;
	}
	

	m_BaseDlg = DLG_BASE_TERR;
	m_bShowNote2 = false;
	m_bShowCtrlArea = false;
	m_bShowCtrl = false;
}

CDlgAutoTools::~CDlgAutoTools()
{
}

void CDlgAutoTools::OnShowDialog()
{
	SetCanMove(false);
}

bool CDlgAutoTools::OnInitContext()
{
	LoadDefault();
	UpdateData(false);

	return true;
}

void CDlgAutoTools::OnCommandOrthCamera(const char* szCommand)
{
	GetAutoScene()->SetOrthCamera();
}

void CDlgAutoTools::OnCommandPersCamera(const char* szCommand)
{
	GetAutoScene()->SetPersCamera();
}

void CDlgAutoTools::OnCommandStandardView(const char* szCommand)
{

	GetAutoScene()->SetPersCamera();

	
	A3DVECTOR3 vPos(0.0f, 250.0f, -800.0f);
	A3DVECTOR3 vDir = A3DVECTOR3(0.0f, 0.0f, 0.0f) - vPos;
	vDir.Normalize();
	g_Render.GetA3DCamera()->SetPos(vPos);
	g_Render.GetA3DCamera()->SetDirAndUp(vDir, A3DVECTOR3(0.0f, 1.0f, 0.0f));
}

void CDlgAutoTools::OnCommandUndo(const char* szCommand)
{


	CAutoScene* pScene = GetAutoScene();
	int nOperation = pScene->GetOperationType();

	switch(nOperation)
	{
	case OT_HILL_RECT:
		if (!pScene->UndoAction(TAT_HILL_RECT))
			return;
		break;
	case OT_HILL_POLY:
		if (!pScene->UndoAction(TAT_HILL_POLY))
			return;
		break;
	case OT_HILL_LINE:
		if (!pScene->UndoAction(TAT_HILL_LINE))
			return;
		break;
	case OT_TEXTURE_POLY:
		if (!pScene->UndoAction(TAT_TEXTURE_POLY))
			return;
		break;
	case OT_ROAD:
		if (!pScene->UndoAction(TAT_ROAD))
			return;
		break;
	case OT_TREE_POLY:
		if (!pScene->UndoAction(TAT_FOREST_POLY))
			return;
		break;
	case OT_TREE_LINE:
		if (!pScene->UndoAction(TAT_FOREST_LINE))
			return;
		break;
	case OT_GRASS_POLY:
		if (!pScene->UndoAction(TAT_GRASS_POLY))
			return;
		break;
	case OT_TERRAIN_FLAT:
		if (!pScene->UndoAction(TAT_TERRAIN_FLAT))
			return;
		break;
	}


}

void CDlgAutoTools::OnCommandRedo(const char* szCommand)
{
	CAutoScene* pScene = GetAutoScene();
	int nOperation = pScene->GetOperationType();

	switch(nOperation)
	{
	case OT_HILL_RECT:
		if (!pScene->RedoAction(TAT_HILL_RECT))
			return;
		break;
	case OT_HILL_POLY:
		if (!pScene->RedoAction(TAT_HILL_POLY))
			return;
		break;
	case OT_HILL_LINE:
		if (!pScene->RedoAction(TAT_HILL_LINE))
			return;
		break;
	case OT_TEXTURE_POLY:
		if (!pScene->RedoAction(TAT_TEXTURE_POLY))
			return;
		break;
	case OT_ROAD:
		if (!pScene->RedoAction(TAT_ROAD))
			return;
		break;
	case OT_TREE_POLY:
		if (!pScene->RedoAction(TAT_FOREST_POLY))
			return;
		break;
	case OT_TREE_LINE:
		if (!pScene->RedoAction(TAT_FOREST_LINE))
			return;
		break;
	case OT_GRASS_POLY:
		if (!pScene->RedoAction(TAT_GRASS_POLY))
			return;
		break;
	case OT_TERRAIN_FLAT:
		if (!pScene->RedoAction(TAT_TERRAIN_FLAT))
			return;
		break;
	}
}

void CDlgAutoTools::OnCommandGenLightMap(const char* szCommand)
{
	CAutoScene* pScene = GetAutoScene();
	
	GetAUIManager()->MessageBox("Auto_GenLightMap", GetStringFromTable(1001) , MB_OKCANCEL, A3DCOLORRGBA(255, 255, 255, 160));
}

void CDlgAutoTools::OnCommandHideResDlg(const char* szCommand)
{
	bool bPushed = m_pBtn[BTN_HIDE_RES]->IsPushed();

	GetHomeDlgsMgr()->m_pDlgAutoAllRes->Show(!bPushed);
	m_pBtn[BTN_HIDE_RES]->SetPushed(!bPushed);

}

#define GET_BTN_POINTER(a) DDX_Control( #a, m_pBtn[a]);

void CDlgAutoTools::DoDataExchange(bool bSave)
{

	
	GET_BTN_POINTER(BTN_TEST);
	GET_BTN_POINTER(BTN_ORTH_CAMERA);
	GET_BTN_POINTER(BTN_PERS_CAMERA);
	GET_BTN_POINTER(BTN_FREE_VIEW);
	GET_BTN_POINTER(BTN_GEN_LIGHTMAP);
	GET_BTN_POINTER(BTN_HIDE_RES);
	GET_BTN_POINTER(BTN_HIDE_CHAT);
	GET_BTN_POINTER(BTN_EXIT);
}

void CDlgAutoTools::LoadDefault()
{
	m_pBtn[BTN_HIDE_RES]->SetPushed(true);
	m_BaseDlg = DLG_BASE_TERR;
	m_bShowNote2 = false;
	m_bShowCtrlArea = false;
	m_bShowCtrl = false;
}

void CDlgAutoTools::OnCommandTest(const char* szCommand)
{
	bool bPushed = m_pBtn[BTN_TEST]->IsPushed();
	m_pBtn[BTN_TEST]->SetPushed(!bPushed);

	if( !bPushed)
	{
		GetGame()->GetGameRun()->GetWorld()->EnterAutoSceneHostMode();
		
	}
	else
	{
		GetGame()->GetGameRun()->GetWorld()->LeaveAutoScentHostMode();
	}

	ShowUnTestDlgs(bPushed);
}

void CDlgAutoTools::ShowUnTestDlgs(bool i_bShow)
{
	if(!i_bShow) //隐藏时,获取当前基本对话框
	{
		if(GetHomeDlgsMgr()->m_pDlgCurBase->IsShow())
			m_BaseDlg = DLG_BASE_TERR;
		else if( GetHomeDlgsMgr()->m_pDlgCurIndoor->IsShow())
			m_BaseDlg = DLG_INDOOR;
		else if( GetHomeDlgsMgr()->m_pDlgCurOutdoor->IsShow())
			m_BaseDlg = DLG_OUTDOOR;
		else if( GetHomeDlgsMgr()->m_pDlgAutoLight->IsShow())
			m_BaseDlg = DLG_LIGHT;

		m_bShowNote2 = GetHomeDlgsMgr()->m_pDlgAutoNote2->IsShow();
		m_bShowCtrl = GetHomeDlgsMgr()->m_pDlgAutoCtrl->IsShow();
		m_bShowCtrlArea = GetHomeDlgsMgr()->m_pDlgAutoCtrlEditArea->IsShow();
		
		GetHomeDlgsMgr()->m_pDlgCurBase->Show(false);
		GetHomeDlgsMgr()->m_pDlgCurIndoor->Show(false);
		GetHomeDlgsMgr()->m_pDlgCurOutdoor->Show(false);
		GetHomeDlgsMgr()->m_pDlgAutoLight->Show(false);

		GetHomeDlgsMgr()->m_pDlgAutoCtrl->Show(false);
		GetHomeDlgsMgr()->m_pDlgAutoCtrlEditArea->Show(false);
		GetHomeDlgsMgr()->m_pDlgAutoNote2->Show(false);
	}
	else//显示当前基本对话框
	{
		if( m_BaseDlg == DLG_BASE_TERR)
		{
			GetHomeDlgsMgr()->m_pDlgCurBase->Show(true);
		}
		else if( m_BaseDlg == DLG_INDOOR)
		{
			GetHomeDlgsMgr()->m_pDlgCurIndoor->Show(true);
		}
		else if( m_BaseDlg == DLG_OUTDOOR)
		{
			GetHomeDlgsMgr()->m_pDlgCurOutdoor->Show(true);
		}
		else if( m_BaseDlg == DLG_LIGHT)
		{
			GetHomeDlgsMgr()->m_pDlgAutoLight->Show(true);
		}
		
		GetHomeDlgsMgr()->m_pDlgAutoCtrl->Show(m_bShowCtrl);
		GetHomeDlgsMgr()->m_pDlgAutoCtrlEditArea->Show(m_bShowCtrlArea);
		GetHomeDlgsMgr()->m_pDlgAutoNote2->Show(m_bShowNote2);
	}
	
	GetHomeDlgsMgr()->m_pDlgAutoAllRes->Show(i_bShow);
	GetHomeDlgsMgr()->m_pDlgAutoSwitchDlg->Show(i_bShow);
	GetHomeDlgsMgr()->m_pDlgAutoCamera->Show(i_bShow);
	

	for( int i = 0; i < BTN_NUM; i++)
	{
		if( i != BTN_TEST)
		{
			m_pBtn[i]->Enable(i_bShow);
		}
	}
}

bool CDlgAutoTools::IsInTesting(void)
{
	return m_pBtn[BTN_TEST]->IsPushed();
}

void CDlgAutoTools::OnCommandSoftGouraud(const char* szCommand)
{
	PROFILE_BEGIN();

	GetAutoScene()->GetAutoTerrain()->SetLightMapEnable(false);

	LIGHTINGPARAMS lightParam = GetAutoScene()->GetLightingParams();
	CSoftGouraud softGouraud;
	softGouraud.Create(GetAutoScene(), lightParam.vSunDir);
	softGouraud.CalculateSceneVertsColor();
	softGouraud.Destroy();

	PROFILE_END();
	
	return;
}

void CDlgAutoTools::OnCommandUpdateScene(const char* szCommand)
{
	GetAutoScene()->UpdateScene();
	GetAutoScene()->RecreateTerrainRender(true);
	GetAutoScene()->GetAutoBuildingOperation()->QuickLightMap();

}

void CDlgAutoTools::OnCommandSaveScene(const char* szCommand)
{
	GetAutoScene()->SaveScene("userdata\\home\\default.action");

	GetAUIManager()->MessageBox("",GetStringFromTable(1002), MB_OK, A3DCOLORRGBA(255, 255, 255, 160)); 

}

void CDlgAutoTools::OnCommandLoadScene(const char* szCommand)
{
	
	GetHomeDlgsMgr()->Reset();
	GetAutoScene()->LoadScene("userdata\\home\\default.action");

	//快速光照
	GetAutoScene()->RecreateTerrainRender(true);
	GetAutoScene()->GetAutoBuildingOperation()->QuickLightMap();

	//重载资源树
	GetHomeDlgsMgr()->m_pDlgAutoAllRes->ReLoadSceneRes();

	//载入完毕
	GetAUIManager()->MessageBox("",GetStringFromTable(1003), MB_OK, A3DCOLORRGBA(255, 255, 255, 160)); 
}
*/