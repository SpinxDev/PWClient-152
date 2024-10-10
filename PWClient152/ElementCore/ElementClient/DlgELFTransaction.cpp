// Filename	: DlgELFGeniusReset.cpp
// Creator	: Chen Zhixin
// Date		: October 17, 2008

#include "WindowsX.h"
#include "AFI.h"
#include "DlgELFTransaction.h"
#include "EC_GameUIMan.h"
#include "EC_GameSession.h"
#include "EC_HostPlayer.h"
#include "EC_HostGoblin.h"
#include "EC_Game.h"
#include "EC_Model.h"



#define new A_DEBUG_NEW
#define INTERAL_LONG	500
#define INTERAL_SHORT	200
#define TEMPERED_TIME	3000

AUI_BEGIN_COMMAND_MAP(CDlgELFTransaction, CDlgBase)

AUI_ON_COMMAND("IDCANCEL",			OnCommand_CANCEL)
AUI_ON_COMMAND("toprotect",			OnCommand_ToProtect)
AUI_ON_COMMAND("tofree",			OnCommand_ToFree)
AUI_ON_COMMAND("changeclear",		OnCommand_StopChange)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgELFTransaction, CDlgBase)

AUI_END_EVENT_MAP()

CDlgELFTransaction::CDlgELFTransaction()
{
}

CDlgELFTransaction::~CDlgELFTransaction()
{
}

bool CDlgELFTransaction::OnInitDialog()
{
	m_pTxt_Change = (PAUILABEL)GetDlgItem("Txt_Change");
	m_pLab_ChangeTime = (PAUILABEL)GetDlgItem("Lab_ChangeTime");
	m_pTxt_ChangeTime = (PAUILABEL)GetDlgItem("Txt_ChangeTime");
	return true;
}

void CDlgELFTransaction::OnTick()
{
	CECHostGoblin *pHostGoblin = (CECHostGoblin *)GetHostPlayer()->GetGoblinModel();
	if (!pHostGoblin)
	{
		Show(false);
	}
}

void CDlgELFTransaction::OnShowDialog()
{
	if (m_szName == "Win_Transaction2")
	{
		CECHostGoblin *pHostGoblin = (CECHostGoblin *)GetHostPlayer()->GetGoblinModel();
		int status = pHostGoblin->GetSecurityState();
		if (status == 0)
		{
			m_pTxt_Change->Show(false);
			m_pLab_ChangeTime->Show(false);
			m_pTxt_ChangeTime->Show(false);
			GetDlgItem("Btn_Change")->Enable(true);
			GetDlgItem("Btn_ChangeClear")->Enable(false);
		}
		else if (status > 0)
		{
			ACString strText;
			time_t timeLeft = status - GetGame()->GetServerGMTTime();
			int HoursPassed = 167 - timeLeft/3600;
			a_Clamp(HoursPassed, 0, 167);
 			strText.Format(GetStringFromTable(7354),HoursPassed, 168); 
			m_pTxt_Change->Show(true);
			m_pLab_ChangeTime->Show(true);
			m_pTxt_ChangeTime->Show(true);
			m_pTxt_ChangeTime->SetText(strText);
			GetDlgItem("Btn_Change")->Enable(false);
			GetDlgItem("Btn_ChangeClear")->Enable(true);
		}			
	}
}

void CDlgELFTransaction::OnCommand_ToProtect(const char *szCommand)
{
	PAUIDIALOG pMsg;
	GetGameUIMan()->MessageBox("Game_ELFToProtect", GetStringFromTable(7351), MB_YESNO, A3DCOLORRGB(255, 255, 255), &pMsg);
}

void CDlgELFTransaction::OnCommand_ToFree(const char *szCommand)
{
	PAUIDIALOG pMsg;
	GetGameUIMan()->MessageBox("Game_ELFToFree", GetStringFromTable(7352), MB_YESNO, A3DCOLORRGB(255, 255, 255), &pMsg);
}

void CDlgELFTransaction::OnCommand_StopChange(const char *szCommand)
{
	PAUIDIALOG pMsg;
	GetGameUIMan()->MessageBox("Game_ELFToProtect", GetStringFromTable(7353), MB_YESNO, A3DCOLORRGB(255, 255, 255), &pMsg);
}

void CDlgELFTransaction::OnCommand_CANCEL(const char * szCommand)
{
	Show(false);
}



