/********************************************************************
	created:	2005/08/16
	created:	16:8:2005   20:35
	file name:	DlgStoragePW.cpp
	author:		yaojun
	
	purpose:	
*********************************************************************/


#include "DlgStoragePW.h"
#include "EC_GameSession.h"
#include "EC_HostPlayer.h"
#include "EC_GameUIMan.h"


AUI_BEGIN_COMMAND_MAP(CDlgStoragePW, CDlgBase)

AUI_ON_COMMAND("confirm", OnCommandConfirm)
AUI_ON_COMMAND("IDCANCEL", OnCommandIDCANCEL)

AUI_END_COMMAND_MAP()

CDlgStoragePW::CDlgStoragePW()
{

}

CDlgStoragePW::~CDlgStoragePW()
{

}

void CDlgStoragePW::OnShowDialog()
{
	CDlgBase::OnShowDialog();

	m_strText.Empty();
	UpdateData(false);
}

void CDlgStoragePW::DoDataExchange(bool bSave)
{
	CDlgBase::DoDataExchange(bSave);

	DDX_EditBox(bSave, "DEFAULT_Txt_Input", m_strText);
}

void CDlgStoragePW::OnCommandConfirm(const char *szCommand)
{
	UpdateData(true);

	if (0 == stricmp(m_szName, "Win_InputString10"))
	{
		GetGameSession()->c2s_CmdOpenFashionTrash(AC2AS(m_strText));
	}
	else if (0 == stricmp(m_szName, "Win_InputString"))
	{
		GetGameSession()->c2s_CmdNPCSevOpenTrash(AC2AS(m_strText));
	}

	CloseDialog();
}

void CDlgStoragePW::OnCommandIDCANCEL(const char *szCommand)
{
	CloseDialog();
}

void CDlgStoragePW::CloseDialog()
{
	if (0 == stricmp(m_szName, "Win_InputString"))
	{
		GetGameUIMan()->m_pCurNPCEssence = NULL;
		GetHostPlayer()->EndNPCService();
	}
	Show(false);
}
