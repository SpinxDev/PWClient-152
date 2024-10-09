// File		: DlgGoldPassword.h
// Creator	: Xiao Zhou
// Date		: 2006/3/9

#include "DlgGoldPassword.h"
#include "EC_GameUIMan.h"
#include "EC_GameSession.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgGoldPassword, CDlgBase)

AUI_ON_COMMAND("confirm",		OnCommandConfirm)

AUI_END_COMMAND_MAP()

CDlgGoldPassword::CDlgGoldPassword()
{
}

CDlgGoldPassword::~CDlgGoldPassword()
{
}

void CDlgGoldPassword::OnCommandConfirm(const char * szCommand)
{
	if( stricmp(m_szName, "Win_GoldPwdChange") == 0)
	{
		if( a_stricmp(GetDlgItem("DEFAULT_Txt_Input")->GetText(), GetDlgItem("Txt_PwdConfirm")->GetText()) == 0 )
		{
			GetGameSession()->cash_SetPassword(GetDlgItem("DEFAULT_Txt_Input")->GetText());
		}
		else
		{
			GetDlgItem("DEFAULT_Txt_Input")->SetText(_AL(""));
			GetDlgItem("Txt_PwdConfirm")->SetText(_AL(""));
			GetGameUIMan()->MessageBox("", GetStringFromTable(867), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
			ChangeFocus(GetDlgItem("DEFAULT_Txt_Input"));
		}
	}
	else if( stricmp(m_szName, "Win_GoldPwdInput") == 0)
	{
		GetGameSession()->cash_Lock(0, GetDlgItem("DEFAULT_Txt_Input")->GetText());
	}
}

void CDlgGoldPassword::OnShowDialog()
{
	if( stricmp(m_szName, "Win_GoldPwdChange") == 0)
	{
		GetDlgItem("DEFAULT_Txt_Input")->SetText(_AL(""));
		GetDlgItem("Txt_PwdConfirm")->SetText(_AL(""));
	}
	else if( stricmp(m_szName, "Win_GoldPwdInput") == 0)
	{
		GetDlgItem("DEFAULT_Txt_Input")->SetText(_AL(""));
	}
}
