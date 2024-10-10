// Filename	: DlgMailOption.cpp
// Creator	: Xiao Zhou
// Date		: 2005/10/31

#include "DlgMailOption.h"
#include "DlgMailList.h"
#include "CSplit.h"
#include "EC_GameUIMan.h"
#include "EC_GameSession.h"
#include "DlgMailWrite.h"


AUI_BEGIN_COMMAND_MAP(CDlgMailOption, CDlgBase)

AUI_ON_COMMAND("Reply",			OnCommandReply)
AUI_ON_COMMAND("Delete",		OnCommandDelete)
AUI_ON_COMMAND("Preserve",		OnCommandPreserve)
AUI_ON_COMMAND("NoPreserve",	OnCommandNoPreserve)

AUI_END_COMMAND_MAP()

CDlgMailOption::CDlgMailOption()
{
}

CDlgMailOption::~CDlgMailOption()
{
}

void CDlgMailOption::OnCommandReply(const char* szCommand)
{
	CSplit s(GetText());
	CSPLIT_STRING_VECTOR vec = s.Split(_AL("\t"));
	ACString strSubject;
	strSubject.Format(_AL("Re: %s"), vec[1] );
	if( strSubject.GetLength() > 20 )
		strSubject = strSubject.Left(20);
	GetGameUIMan()->m_pDlgMailWrite->CreateNewMail(vec[2].Mid(2), strSubject );
	Show(false);
}

void CDlgMailOption::OnCommandDelete(const char* szCommand)
{
	Show(false);
	int n = GetGameUIMan()->m_pDlgMailList->m_pLst_Mail->GetSelCount();
	if( GetData() < 0x10000 )
	{
		GetGameUIMan()->m_pDlgMailList->SetWaitingResponse(GetData());
		GetGameSession()->mail_Delete(GetData() & 0xFFFF);
	}
	else
	{
		PAUIDIALOG pMsgBox;
		GetGameUIMan()->MessageBox("Game_MailDelete", GetStringFromTable(653),
			MB_OKCANCEL, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
		pMsgBox->SetData(GetData() & 0xFFFF);
	}

}

void CDlgMailOption::OnCommandPreserve(const char* szCommand)
{
	GetGameUIMan()->m_pDlgMailList->SetWaitingResponse(GetData() & 0xFFFF);
	GetGameSession()->mail_Preserve(GetData() & 0xFFFF, true);
	Show(false);
}

void CDlgMailOption::OnCommandNoPreserve(const char* szCommand)
{
	GetGameUIMan()->m_pDlgMailList->SetWaitingResponse(GetData() & 0xFFFF);
	GetGameSession()->mail_Preserve(GetData() & 0xFFFF, false);
	Show(false);
}
