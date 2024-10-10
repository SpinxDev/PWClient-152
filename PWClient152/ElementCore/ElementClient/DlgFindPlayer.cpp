#include "DlgFindPlayer.h"
#include "DlgGuildMan.h"
#include "EC_GameUIMan.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgFindPlayer, CDlgBase)

AUI_ON_COMMAND("IDCANCEL", OnCommandCancel)
AUI_ON_COMMAND("confirm", OnCommandQuery)

AUI_END_COMMAND_MAP()

CDlgFindPlayer::CDlgFindPlayer()
{
	m_pEdit_PlayerName = NULL;
}

bool CDlgFindPlayer::OnInitDialog()
{
	DDX_Control("Edt_PlayerName", m_pEdit_PlayerName);
	return true;
}

void CDlgFindPlayer::OnShowDialog()
{
	m_pEdit_PlayerName->SetText(_AL(""));
	ChangeFocus(m_pEdit_PlayerName);
}

void CDlgFindPlayer::OnCommandCancel(const char *szCommand)
{
	Show(false);
}

void CDlgFindPlayer::OnCommandQuery(const char *szCommand)
{
	CDlgGuildMan *pGuildMan = GetGameUIMan()->m_pDlgGuildMan;
	if (pGuildMan->IsShow())
	{
		const ACHAR *szPlayerName = m_pEdit_PlayerName->GetText();
		if (szPlayerName && a_strlen(szPlayerName) > 0)
		{
			if (pGuildMan->FindAndSelect(szPlayerName))
			{
				Show(false);
			}
			else
			{
				ChangeFocus(m_pEdit_PlayerName);
				GetGameUIMan()->MessageBox("", GetStringFromTable(8050), MB_OK, A3DCOLORRGB(255, 255, 255));
			}
		}
	}
}