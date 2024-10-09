/********************************************************************
	created:	2005/08/11
	created:	11:8:2005   15:11
	file base:	DlgSystem3
	file ext:	cpp
	author:		yaojun
	
	purpose:	
*********************************************************************/

#include "DlgSystem3.h"
#include "DlgSetting.h"
#include "DlgAskHelpToGM.h"
#include "DlgHelp.h"
#include "DlgExit.h"
#include "DlgSettingQuickKey.h"
#include "EC_GameUIMan.h"
#include "EC_GPDataType.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_HostPlayer.h"
#include "EC_CrossServer.h"
#include "EC_TimeSafeChecker.h"

#include "AIniFile.h"

#define new A_DEBUG_NEW


AUI_BEGIN_COMMAND_MAP(CDlgSystem3, CDlgBase)

AUI_ON_COMMAND("wservertype", OnCommandServerType)
AUI_ON_COMMAND("wgm", OnCommandGM)
AUI_ON_COMMAND("whelp", OnCommandHelp)
AUI_ON_COMMAND("wsetting", OnCommandSetting)
AUI_ON_COMMAND("repick", OnCommandRepick)
AUI_ON_COMMAND("quitgame", OnCommandQuit)
AUI_ON_COMMAND("chat", OnCommandChat)
AUI_ON_COMMAND("wquickkey", OnCommandQuickKey)

AUI_END_COMMAND_MAP()


CDlgSystem3::CDlgSystem3()
{

}

CDlgSystem3::~CDlgSystem3()
{

}

void CDlgSystem3::OnCommandServerType(const char *szCommand)
{
	PAUIDIALOG pDlg;
	if( GetGameRun()->GetPVPMode() )
		pDlg = GetGameUIMan()->GetDialog("Win_ServerTypePVP");
	else
		pDlg = GetGameUIMan()->GetDialog("Win_ServerTypePVE");
	if( pDlg )
		pDlg->Show(!pDlg->IsShow());
}

void CDlgSystem3::OnCommandGM(const char *szCommand)
{
	AString strText = GetBaseUIMan()->GetURL("URL", "Support");
	if (!strText.IsEmpty())
	{
		GetBaseUIMan()->NavigateURL(strText, NULL, true);
		return;
	}

	CDlgAskHelpToGM *pDlg = GetGameUIMan()->m_pDlgAskHelpToGM;
	if (pDlg->IsShow())
	{
		pDlg->Show(false);
	}
	else
	{
#ifndef _DEBUG
		{
			// GM can send msg without delta-time restriction
			if (CECTimeSafeChecker::ElapsedTimeFor(pDlg->GetData()) < 300 * 1000 &&
				!g_pGame->GetGameRun()->GetHostPlayer()->IsGM())
			{
				GetGameUIMan()->AddChatMessage(GetStringFromTable(612), GP_CHAT_MISC);
				return;
			}
		}
#endif
		pDlg->ShowWithHelpMessage(CDlgAskHelpToGM::GM_REPORT_STUCK | CDlgAskHelpToGM::GM_REPORT_TALK);
	}

}

void CDlgSystem3::OnCommandHelp(const char *szCommand)
{
	GetGameUIMan()->m_pDlgHelp->Show(
		!GetGameUIMan()->m_pDlgHelp->IsShow());
}

void CDlgSystem3::OnCommandSetting(const char *szCommand)
{
	GetGameUIMan()->m_pDlgSettingCurrent->Show(
		!GetGameUIMan()->m_pDlgSettingCurrent->IsShow());
}

void CDlgSystem3::OnCommandRepick(const char *szCommand)
{
	if( !GetGameUIMan()->m_pDlgExit->IsShow() &&
		!GetGameUIMan()->GetDialog("Game_Quit") )
	{
		AUIDialog *pMsgBox = NULL;
		GetGameUIMan()->MessageBox("Game_Quit",
			GetGameUIMan()->GetStringFromTable(CECCrossServer::Instance().IsOnSpecialServer() ? 10131 : 202),
			MB_YESNO, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
		pMsgBox->SetIsModal(false);
	}
}

void CDlgSystem3::OnCommandQuit(const char *szCommand)
{
	if( !GetGameUIMan()->m_pDlgExit->IsShow() &&
		!GetGameUIMan()->GetDialog("Game_Quit") )
	{
		GetGameUIMan()->m_pDlgExit->Show(true);
	}
}

void CDlgSystem3::OnCommandChat(const char *szCommand)
{
	AString strText = GetBaseUIMan()->GetURL("Chat", "URL");
	GetBaseUIMan()->NavigateURL(strText);
}

void CDlgSystem3::OnCommandQuickKey(const char *szCommand)
{
	CDlgSettingQuickKey *pSettingQuickKey = GetGameUIMan()->m_pDlgSettingQuickKey;
	
	if (!pSettingQuickKey->IsShow())
	{
		if (GetGameUIMan()->m_pDlgSettingCurrent != NULL) {
			GetGameUIMan()->m_pDlgSettingCurrent->OnCommandSetting("quickkey");
		} else {
			pSettingQuickKey->Show(true);
			GetGameUIMan()->m_pDlgSettingCurrent = pSettingQuickKey;
		}
	
	}
	else
	{
		// 调用 IDCANCEL 命令消除快捷键自定义状态
		pSettingQuickKey->Show(false);
	}
}