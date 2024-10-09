/********************************************************************
	created:	2005/08/11
	created:	11:8:2005   17:03
	file name:	DlgSetting.cpp
	author:		yaojun
	
	purpose:	
*********************************************************************/

#include "A3DDevice.h"
#include "EC_Game.h"
#include "EC_Viewport.h"
#include "EC_GameUIMan.h"
#include "DlgSetting.h"
#include "DlgSettingVideo.h"
#include "DlgSettingSystem.h"
#include "DlgSettingGame.h"
#include "DlgSettingQuickKey.h"
#include "AUIStillImageButton.h"
#include <A3DGFXExMan.h>

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgSetting, CDlgBase)

AUI_ON_COMMAND("gamesetting", OnCommandSetting)
AUI_ON_COMMAND("videosetting", OnCommandSetting)
AUI_ON_COMMAND("systemsetting", OnCommandSetting)
AUI_ON_COMMAND("quickkey", OnCommandSetting)
AUI_ON_COMMAND("default", OnCommandDefault)
AUI_ON_COMMAND("apply", OnCommandApply)
AUI_ON_COMMAND("confirm", OnCommandConfirm)
AUI_ON_COMMAND("IDCANCEL", OnCommandCancel)

AUI_END_COMMAND_MAP()


AUI_BEGIN_EVENT_MAP(CDlgSetting, CDlgBase)

AUI_END_EVENT_MAP()

CDlgSetting::CDlgSetting() :
	m_pBtnSystem(NULL),
	m_pBtnVideo(NULL),
	m_pBtnGame(NULL),
	m_pBtnQuickKey(NULL),
	m_pBtnDefault(NULL),
	m_bValidateSucceed(false)
{

}

CDlgSetting::~CDlgSetting()
{

}

void CDlgSetting::DoDataExchange(bool bSave)
{
	CDlgBase::DoDataExchange(bSave);

	DDX_Control("Btn_System", m_pBtnSystem);
	DDX_Control("Btn_Video", m_pBtnVideo);
	DDX_Control("Btn_Game", m_pBtnGame);
	DDX_Control("Btn_QuickKey", m_pBtnQuickKey);
	DDX_Control("Btn_Default", m_pBtnDefault);
}

void CDlgSetting::OnCommandSetting(const char *szCommand)
{
	// get dlg to show
	CDlgSetting * pDlg = NULL;
	AString strCommand = szCommand;
	if (strCommand == "systemsetting")
		pDlg = GetGameUIMan()->m_pDlgSettingSystem;
	else if (strCommand == "videosetting")
		pDlg = GetGameUIMan()->m_pDlgSettingVideo;
	else if (strCommand == "gamesetting")
		pDlg = GetGameUIMan()->m_pDlgSettingGame;
	else if (strCommand == "quickkey")
		pDlg = GetGameUIMan()->m_pDlgSettingQuickKey;
	ASSERT(pDlg);

	// hide old dialog
	POINT pos = GetGameUIMan()->m_pDlgSettingCurrent->GetPos();
	GetGameUIMan()->m_pDlgSettingCurrent->Show(false);

	// show dialog
	GetGameUIMan()->m_pDlgSettingCurrent = pDlg;

	// old : pDlg->SetPos(pos.x, pos.y);
	pDlg->SetPosEx(pos.x, pos.y);

	GetGameUIMan()->m_pDlgSettingCurrent->Show(true);
}


void CDlgSetting::OnCommandDefault(const char *szCommand)
{
	GetGameUIMan()->m_pDlgSettingGame->SetToDefault();
	GetGameUIMan()->m_pDlgSettingSystem->SetToDefault();
	GetGameUIMan()->m_pDlgSettingVideo->SetToDefault();
	GetGameUIMan()->m_pDlgSettingQuickKey->SetToDefault();
}

void CDlgSetting::OnCommandApply(const char *szCommand)
{
	m_bValidateSucceed = 
		GetGameUIMan()->m_pDlgSettingGame->Validate() &&
		GetGameUIMan()->m_pDlgSettingSystem->Validate() &&
		GetGameUIMan()->m_pDlgSettingVideo->Validate() &&
		GetGameUIMan()->m_pDlgSettingQuickKey->Validate();
	if (!m_bValidateSucceed) return;

	// old param
	A3DViewport * pViewport = GetGame()->GetViewport()->GetA3DViewport();
	A3DVIEWPORTPARAM param1 = *pViewport->GetParam();

	// do apply
	if( stricmp(m_szName, "Win_SettingGame") == 0 )
		GetGameUIMan()->m_pDlgSettingGame->Apply();
	else if( stricmp(m_szName, "Win_SettingSystem") == 0 )
	{
		GetGameUIMan()->m_pDlgSettingSystem->Apply();
		// save system setting to file
		GetGame()->GetConfigs()->SaveSystemSettings();
	}
	else if( stricmp(m_szName, "Win_SettingVideo") == 0 )
		GetGameUIMan()->m_pDlgSettingVideo->Apply();
	else if( stricmp(m_szName, "Win_SettingQuickKey") == 0 )
		GetGameUIMan()->m_pDlgSettingQuickKey->Apply();

	// new param
	A3DVIEWPORTPARAM param2 = *pViewport->GetParam();
	

	// update IME state
	GetGameUIMan()->SetHideIme(!m_pA3DDevice->GetDevFormat().bWindowed);
	
	// RearrangeWindows if res is changed
	A3DRECT rcOld(param1.X, param1.Y, param1.X+param1.Width, param1.Y+param1.Height);
	A3DRECT rcNew(param2.X, param2.Y, param2.X+param2.Width, param2.Y+param2.Height);
	if(rcOld != rcNew)
	{		
		GetGameUIMan()->RearrangeWindows(rcOld, rcNew);
		AfxGetGFXExMan()->Resize2DViewport();
	}

}

void CDlgSetting::OnCommandConfirm(const char *szCommand)
{
	OnCommandApply(NULL);

	if (m_bValidateSucceed)	Show(false);
}

void CDlgSetting::OnCommandCancel(const char *szCommand)
{
	Show(false);
}





