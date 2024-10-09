// File		: DlgGoldAccount.h
// Creator	: Xiao Zhou
// Date		: 2006/9/20

#include "DlgGoldAccount.h"
#include "DlgGoldTrade.h"
#include "DlgGoldInquire.h"
#include "DlgInputNO.h"
#include "EC_GameUIMan.h"
#include "EC_GameSession.h"
#include "EC_HostPlayer.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgGoldAccount, CDlgBase)

AUI_ON_COMMAND("confirm",		OnCommandConfirm)
AUI_ON_COMMAND("goldmax",		OnCommandGoldMax)
AUI_ON_COMMAND("moneymax",		OnCommandMoneyMax)

AUI_END_COMMAND_MAP()

CDlgGoldAccount::CDlgGoldAccount()
{
}

CDlgGoldAccount::~CDlgGoldAccount()
{
}

void CDlgGoldAccount::OnCommandConfirm(const char * szCommand)
{
	int nCash = a_atoi(m_pTxt_Gold->GetText()) * 100 + a_atoi(m_pTxt_Silver->GetText());
	if( nCash > (int)m_pTxt_Gold->GetData() )
		nCash = m_pTxt_Gold->GetData();
	int nMoney = a_atoi(m_pTxt_Money->GetText());
	if( nMoney > (int)m_pTxt_Money->GetData() )
		nMoney = m_pTxt_Money->GetData();
	if( nMoney != 0 || nCash != 0 )
	{
		GetGameSession()->c2s_CmdNPCSevStockTransaction(GetData(), nCash, nMoney);
	}
	Show(false);
}

void CDlgGoldAccount::OnCommandGoldMax(const char * szCommand)
{
	int nCash = m_pTxt_Gold->GetData();
	ACHAR szText[20];
	a_sprintf(szText, _AL("%d"), nCash / 100);
	m_pTxt_Gold->SetText(szText);
	a_sprintf(szText, _AL("%d"), nCash % 100);
	m_pTxt_Silver->SetText(szText);
}

void CDlgGoldAccount::OnCommandMoneyMax(const char * szCommand)
{
	ACHAR szMoney[20];
	a_sprintf(szMoney, _AL("%d"), m_pTxt_Money->GetData());
	m_pTxt_Money->SetText(szMoney);
}

bool CDlgGoldAccount::OnInitDialog()
{
	m_pLab_Gold = (PAUILABEL)GetDlgItem("Lab_Gold");
	m_pLab_Gold1 = (PAUILABEL)GetDlgItem("Lab_Gold1");
	m_pLab_Money = (PAUILABEL)GetDlgItem("Lab_Money");
	m_pLab_Money1 = (PAUILABEL)GetDlgItem("Lab_Money1");
	m_pTxt_Gold = dynamic_cast<PAUIEDITBOX>(GetDlgItem("Txt_Gold"));
	m_pTxt_Gold->IsNumberOnly();
	m_pTxt_Silver = dynamic_cast<PAUIEDITBOX>(GetDlgItem("Txt_Silver"));
	m_pTxt_Silver->IsNumberOnly();
	m_pTxt_Money = GetDlgItem("Txt_Money");

	return true;
}

void CDlgGoldAccount::OnShowDialog()
{
	m_pTxt_Money->SetText(_AL("0"));
	m_pTxt_Gold->SetText(_AL(""));
	m_pTxt_Silver->SetText(_AL(""));
}
