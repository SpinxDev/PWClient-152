// File		: DlgDoubleExp.cpp
// Creator	: Xiao Zhou
// Date		: 2005/12/15

#include "DlgDoubleExp.h"
#include "EC_GameUIMan.h"
#include "EC_GameSession.h"
#include "EC_GameRun.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgDoubleExp, CDlgBase)

AUI_ON_COMMAND("confirm",		OnCommandConfirm)
AUI_ON_COMMAND("IDCANCEL",		OnCommandCancel)

AUI_END_COMMAND_MAP()

CDlgDoubleExp::CDlgDoubleExp()
{
	m_pRdo_1 = NULL;
	m_pRdo_2 = NULL;
	m_pRdo_3 = NULL;
	m_pRdo_4 = NULL;
	m_pTxt_Remain = NULL;
}

CDlgDoubleExp::~CDlgDoubleExp()
{
}


void CDlgDoubleExp::OnCommandConfirm(const char *szCommand)
{
	CECGameSession* pSession = GetGameSession();

	if( m_pRdo_1->IsChecked() )
		pSession->c2s_CmdNPCSevDblExpTime(0);
	else if( m_pRdo_2->IsChecked() )
		pSession->c2s_CmdNPCSevDblExpTime(1);
	else if( m_pRdo_3->IsChecked() )
		pSession->c2s_CmdNPCSevDblExpTime(2);
	else if( m_pRdo_4->IsChecked() )
		pSession->c2s_CmdNPCSevDblExpTime(3);
	GetGameUIMan()->EndNPCService();
	Show(false);
	
	//	Added by dyx: update double-experience time from server.
	pSession->c2s_CmdQueryDoubleExp();
}

void CDlgDoubleExp::OnCommandCancel(const char *szCommand)
{
	Show(false);
	GetGameUIMan()->EndNPCService();
}

bool CDlgDoubleExp::OnInitDialog()
{
	DDX_Control("Txt_Remain", m_pTxt_Remain);
	DDX_Control("Rdo_1", m_pRdo_1);
	DDX_Control("Rdo_2", m_pRdo_2);
	DDX_Control("Rdo_3", m_pRdo_3);
	DDX_Control("Rdo_4", m_pRdo_4);

	return true;
}

void CDlgDoubleExp::OnShowDialog()
{
	m_pRdo_1->Check(true);
	m_pRdo_2->Check(false);
	m_pRdo_3->Check(false);
	m_pRdo_4->Check(false);
	int nTime = GetGameRun()->GetAvailableDblExpTime();
	ACHAR szText[20];
	a_sprintf(szText, _AL("%dСʱ"), nTime / 3600);
	m_pTxt_Remain->SetText(szText);
}
