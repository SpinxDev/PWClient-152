// File		: DlgGoldTrade.cpp
// Creator	: Xiao Zhou
// Date		: 2006/9/20

#include "DlgGoldTrade.h"
#include "DlgGoldAccount.h"
#include "DlgGoldInquire.h"
#include "DlgInputNO.h"
#include "DlgQShop.h"
#include "EC_Game.h"
#include "EC_GameUIMan.h"
#include "EC_GameSession.h"
#include "EC_HostPlayer.h"
#include "EC_IvtrItem.h"
#include "AUIStillImageButton.h"
#include "CSplit.h"
#include "stockaccount_re.hpp"
#include "stockbill_re.hpp"
#include "stockcommission_re.hpp"
#include "stocktransaction_re.hpp"
#include "stockcancel_re.hpp"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgGoldTrade, CDlgBase)

AUI_ON_COMMAND("sell",			OnCommandSell)
AUI_ON_COMMAND("buy",			OnCommandBuy)
AUI_ON_COMMAND("account*",		OnCommandAccount)
AUI_ON_COMMAND("inquire",		OnCommandInquire)
AUI_ON_COMMAND("confirm",		OnCommandConfirm)
AUI_ON_COMMAND("refresh",		OnCommandRefresh)
AUI_ON_COMMAND("pwd",			OnCommandPwd)
AUI_ON_COMMAND("unlock",		OnCommandUnlock)
AUI_ON_COMMAND("IDCANCEL",		OnCommandCancel)

AUI_END_COMMAND_MAP()

CDlgGoldTrade::CDlgGoldTrade()
{
	m_nBuy = 1;
	m_bLocked = false;
}

CDlgGoldTrade::~CDlgGoldTrade()
{
}

void CDlgGoldTrade::OnCommandAccount(const char * szCommand)
{
	GetGameUIMan()->m_pDlgGoldAccount->Show(true, true);
	if( stricmp(szCommand, "accountsave") == 0 )
	{
		GetGameUIMan()->m_pDlgGoldAccount->m_pTxt_Money->SetData(GetHostPlayer()->GetMoneyAmount());
		GetGameUIMan()->m_pDlgGoldAccount->m_pTxt_Gold->SetData(GetHostPlayer()->GetCash());
		GetGameUIMan()->m_pDlgGoldAccount->SetData(0);
		GetGameUIMan()->m_pDlgGoldAccount->m_pLab_Gold->Show(true);
		GetGameUIMan()->m_pDlgGoldAccount->m_pLab_Gold1->Show(false);
		GetGameUIMan()->m_pDlgGoldAccount->m_pLab_Money->Show(true);
		GetGameUIMan()->m_pDlgGoldAccount->m_pLab_Money1->Show(false);
	}
	else
	{
		GetGameUIMan()->m_pDlgGoldAccount->m_pTxt_Money->SetData(m_pTxt_Money4->GetData());
		GetGameUIMan()->m_pDlgGoldAccount->m_pTxt_Gold->SetData(m_pTxt_Gold->GetData());
		GetGameUIMan()->m_pDlgGoldAccount->SetData(1);
		GetGameUIMan()->m_pDlgGoldAccount->m_pLab_Gold1->Show(true);
		GetGameUIMan()->m_pDlgGoldAccount->m_pLab_Gold->Show(false);
		GetGameUIMan()->m_pDlgGoldAccount->m_pLab_Money1->Show(true);
		GetGameUIMan()->m_pDlgGoldAccount->m_pLab_Money->Show(false);
	}
}

void CDlgGoldTrade::OnCommandInquire(const char * szCommand)
{
	Show(false);
	GetGameUIMan()->m_pDlgGoldInquire->Show(true);
	GetGameUIMan()->m_pDlgGoldInquire->SetPosEx(GetPos().x, GetPos().y);
}

void CDlgGoldTrade::OnCommandConfirm(const char * szCommand)
{
	int nNum = a_atoi(m_pTxt_Num->GetText());
	int nMoney = a_atoi(m_pTxt_Money1->GetText());
	if( nMoney > 0 && nNum > 0 )
	{
		if( nMoney % 100 > 0 || nMoney >= 4000000 )
		{
			GetGameUIMan()->MessageBox("", GetStringFromTable(866), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
			return;
		}
		PAUIDIALOG pMsgBox;
		int nPrice = a_atoi(m_pTxt_Money1->GetText());
		ACString strText;
		if( m_nBuy == 1 )
		{
			strText.Format(GetStringFromTable(841), GetGameUIMan()->GetFormatNumber(nPrice), nNum);
			GetGameUIMan()->MessageBox("Game_GoldBuy", strText, MB_YESNO, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
		}
		else
		{
			strText.Format(GetStringFromTable(842), GetGameUIMan()->GetFormatNumber(nPrice), nNum);
			GetGameUIMan()->MessageBox("Game_GoldSell", strText, MB_YESNO, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
		}
		pMsgBox->SetData(a_atoi(m_pTxt_Money1->GetText()));
		pMsgBox->SetDataPtr((void*)nNum);
	}
}

void CDlgGoldTrade::OnCommandRefresh(const char * szCommand)
{
	GetGameSession()->stock_account();
}

void CDlgGoldTrade::OnCommandPwd(const char * szCommand)
{
	GetGameUIMan()->GetDialog("Win_GoldPwdChange")->Show(true);
}

void CDlgGoldTrade::OnCommandUnlock(const char * szCommand)
{
	GetGameUIMan()->GetDialog("Win_GoldPwdInput")->Show(true);
}

void CDlgGoldTrade::OnCommandBuy(const char * szCommand)
{
	m_nBuy = 1;
	GetDlgItem("Lab_Sell01")->Show(false);
	GetDlgItem("Lab_Sella1")->Show(true);
	GetDlgItem("Lab_Sell04")->Show(false);
	GetDlgItem("Lab_Sella4")->Show(true);
	((PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_Buy"))->SetPushed(true);
	((PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_Sell"))->SetPushed(false);
	((PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_Buy"))->SetColor(A3DCOLORRGB(255, 205, 75));
	((PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_Sell"))->SetColor(A3DCOLORRGB(255, 255, 255));
}

void CDlgGoldTrade::OnCommandSell(const char * szCommand)
{
	m_nBuy = 0;
	GetDlgItem("Lab_Sell01")->Show(true);
	GetDlgItem("Lab_Sella1")->Show(false);
	GetDlgItem("Lab_Sell04")->Show(true);
	GetDlgItem("Lab_Sella4")->Show(false);
	((PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_Buy"))->SetPushed(false);
	((PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_Sell"))->SetPushed(true);
	((PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_Buy"))->SetColor(A3DCOLORRGB(255, 255, 255));
	((PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_Sell"))->SetColor(A3DCOLORRGB(255, 205, 75));
}

void CDlgGoldTrade::OnCommandCancel(const char * szCommand)
{
	GetGameSession()->cash_Lock(1, _AL(""));
	GetGameUIMan()->EndNPCService();
	Show(false);
}

bool CDlgGoldTrade::OnInitDialog()
{
	m_pLst_Sell = (PAUILISTBOX)GetDlgItem("Lst_Sell");
	m_pLst_Buy = (PAUILISTBOX)GetDlgItem("Lst_Buy");
	m_pTxt_Num = dynamic_cast<PAUIEDITBOX>(GetDlgItem("Txt_Num"));
	m_pTxt_Num->SetIsNumberOnly(true);
	m_pTxt_Gold = (PAUILABEL)GetDlgItem("Txt_Gold");
	m_pTxt_Gold1 = (PAUILABEL)GetDlgItem("Txt_Gold1");
	m_pTxt_Money1 = GetDlgItem("Txt_Money1");
	m_pTxt_Money2 = GetDlgItem("Txt_Money2");
	m_pTxt_Money4 = GetDlgItem("Txt_Money4");
	m_pTxt_Money5 = GetDlgItem("Txt_Money5");
	m_pTxt_Poundage = (PAUILABEL)GetDlgItem("Txt_Poundage");
	
	return true;
}

void CDlgGoldTrade::OnShowDialog()
{
	m_vecAccountOrders.clear();
	GetGameSession()->stock_account();
	m_pLst_Sell->ResetContent();
	m_pLst_Buy->ResetContent();
	m_pTxt_Num->SetText(_AL(""));
	m_pTxt_Money1->SetText(_AL(""));
	m_pTxt_Num->SetData(0);
	m_pTxt_Gold->SetData(0);
	OnCommandBuy("");
}

void CDlgGoldTrade::OnTick()
{
	int nNum = a_atoi(m_pTxt_Num->GetText());
	if( nNum > 1000 )
	{
		m_pTxt_Num->SetText(_AL("1000"));
		nNum = 1000;
	}
	int nMoney = a_atoi(m_pTxt_Money1->GetText());
	ACHAR szMoney[256];
	
	nMoney *= nNum;
	a_sprintf(szMoney, _AL("%d"), nMoney);
	m_pTxt_Money2->SetText(szMoney);
	if( m_nBuy == 1 )
	{
		nMoney = int(nMoney * 0.02);
		a_sprintf(szMoney, _AL("%d"), nMoney);
		m_pTxt_Poundage->SetText(szMoney);
	}
	else
	{
		nMoney = nNum * 2;
		m_pTxt_Poundage->SetText(GetGameUIMan()->m_pDlgQShop->GetCashText(nMoney));
	}

	nMoney = m_pTxt_Money4->GetData();
	a_sprintf(szMoney, _AL("%d"), nMoney);
	m_pTxt_Money4->SetText(szMoney);

	nMoney = GetHostPlayer()->GetMoneyAmount();
	a_sprintf(szMoney, _AL("%d"), nMoney);
	m_pTxt_Money5->SetText(szMoney);

	m_pTxt_Gold->SetText(GetGameUIMan()->m_pDlgQShop->GetCashText(m_pTxt_Gold->GetData()));
	m_pTxt_Gold1->SetText(GetGameUIMan()->m_pDlgQShop->GetCashText(GetHostPlayer()->GetCash()));
}

void CDlgGoldTrade::GoldAction(int idAction, void *pData)
{
	if( idAction == CDlgGoldTrade::GOLD_ACTION_ACCOUNT_RE )
	{
		StockAccount_Re *p = (StockAccount_Re *)pData;

		SetLocked(p->locked != 0);
		m_pTxt_Gold->SetData(p->cash);
		m_pTxt_Money4->SetData(p->money);
		PAUILISTBOX pListBuy2 = GetGameUIMan()->m_pDlgGoldInquire->m_pLst_Buy;
		PAUILISTBOX pListSell2 = GetGameUIMan()->m_pDlgGoldInquire->m_pLst_Sell;
		m_pLst_Buy->ResetContent();
		m_pLst_Sell->ResetContent();
		pListBuy2->ResetContent();
		pListSell2->ResetContent();
		DWORD i;
		ACString strText, strGold;
		for(i = 0; i < p->prices.size(); i++ )
		{
			strGold.Format(GetStringFromTable(854), p->prices[i].volume);
			if( p->prices[i].price > 0 )
			{
				strText.Format(_AL("%s\t%s\t%s"), GetGameUIMan()->GetFormatNumber(p->prices[i].price), GetStringFromTable(853), strGold);
				m_pLst_Sell->AddString(strText);
				pListSell2->AddString(strText);
				m_pLst_Sell->SetItemTextColor(m_pLst_Sell->GetCount() - 1, A3DCOLORRGB(255, 255, 0));
				pListSell2->SetItemTextColor(pListSell2->GetCount() - 1, A3DCOLORRGB(255, 255, 0));
			}
			else
			{
				strText.Format(_AL("%s\t%s\t%s"), GetGameUIMan()->GetFormatNumber(-p->prices[i].price), GetStringFromTable(852), strGold);
				m_pLst_Buy->AddString(strText);
				pListBuy2->AddString(strText);
				m_pLst_Buy->SetItemTextColor(m_pLst_Buy->GetCount() - 1, A3DCOLORRGB(0, 255, 0));
				pListBuy2->SetItemTextColor(pListBuy2->GetCount() - 1, A3DCOLORRGB(0, 255, 0));
			}
		}
	}
	else if( idAction == CDlgGoldTrade::GOLD_ACTION_BILL_RE )
	{
		StockBill_Re *p = (StockBill_Re *)pData;

		m_vecAccountOrders.clear();
		DWORD i;
		ACString strText;
		ACString strTime;
		for(i = 0; i < p->orders.size(); i++ )
		{
			StockInfo info;
			info.price = p->orders[i].price;
			info.volume = p->orders[i].volume;
			info.tid = p->orders[i].tid;
			info.time = 0;
			m_vecAccountOrders.push_back(info);
		}
		PAUILISTBOX pList =	GetGameUIMan()->m_pDlgGoldInquire->m_pLst_Inquire;
		pList->ResetContent();
		for(i = 0; i < m_vecAccountOrders.size(); i++ )
		{
			if( m_vecAccountOrders[i].price > 0 )
			{
				strText.Format(_AL("%d\t%s\t%d\t%d\t%s\t%s"), m_vecAccountOrders[i].tid, 
					GetStringFromTable(847), 
					m_vecAccountOrders[i].volume, m_vecAccountOrders[i].price, _AL("---"),
					GetStringFromTable(850));
			}
			else
			{
				strText.Format(_AL("%d\t%s\t%d\t%d\t%s\t%s"), m_vecAccountOrders[i].tid, 
					GetStringFromTable(846), 
					m_vecAccountOrders[i].volume, -m_vecAccountOrders[i].price, _AL("---"),
					GetStringFromTable(850));
			}
			pList->AddString(strText);
			pList->SetItemData(pList->GetCount() - 1, m_vecAccountOrders[i].tid);
			pList->SetItemDataPtr(pList->GetCount() - 1, (void*)m_vecAccountOrders[i].price);
			if( m_vecAccountOrders[i].price > 0 )
				pList->SetItemTextColor(pList->GetCount() - 1, A3DCOLORRGB(255, 255, 0));
			else
				pList->SetItemTextColor(pList->GetCount() - 1, A3DCOLORRGB(0, 255, 0));
		}
		int n = pList->GetCount();
		for(i = 0; i < p->logs.size(); i++ )
		{
			long stime = p->logs[i].time;
			stime -= GetGame()->GetTimeZoneBias() * 60; // localtime = UTC - bias, in which bias is in minutes
			tm *gtime = gmtime(&stime);
			strTime.Format(GetStringFromTable(789), 
				gtime->tm_mon + 1, gtime->tm_mday, gtime->tm_hour, gtime->tm_min);
			ACString strPrice, strTotalPrice;
			StockLog &log = p->logs[i];
			if( p->logs[i].result == 1 )
			{
				strPrice.Format(_AL("%d"), abs(p->logs[i].cost));
				strTotalPrice = GetStringFromTable(864);
			}
			else if( p->logs[i].result == 2 )
			{
				strPrice.Format(_AL("%d"), abs(p->logs[i].cost));
				strTotalPrice = GetStringFromTable(865);
			}
			else
			{
				strPrice = _AL("---");
				strTotalPrice.Format(_AL("%d"), abs(p->logs[i].cost));
			}
			if( p->logs[i].cost < 0 )
			{
				strText.Format(_AL("%d\t%s\t%d\t%s\t%s\t%s"), p->logs[i].tid, 
					GetStringFromTable(846), 
					p->logs[i].volume, strPrice, strTotalPrice,
					strTime);
			}
			else
			{
				strText.Format(_AL("%d\t%s\t%d\t%s\t%s\t%s"), p->logs[i].tid, 
					GetStringFromTable(847), 
					p->logs[i].volume, strPrice, strTotalPrice,
					strTime);
			}
			pList->InsertString(n, strText);
			if( p->logs[i].cost < 0 )
				pList->SetItemTextColor(n, A3DCOLORRGB(0, 255, 0));
			else
				pList->SetItemTextColor(n, A3DCOLORRGB(255, 255, 0));
		}
	}
	else if( idAction == CDlgGoldTrade::GOLD_ACTION_COMMISSION_RE )
	{
		StockCommission_Re *p = (StockCommission_Re *)pData;
		if( !CheckRetcode(p->retcode, 848) )
			return;

		m_pTxt_Gold->SetData(p->cash);
		m_pTxt_Money4->SetData(p->money);
		PAUILISTBOX pListBuy2 = GetGameUIMan()->m_pDlgGoldInquire->m_pLst_Buy;
		PAUILISTBOX pListSell2 = GetGameUIMan()->m_pDlgGoldInquire->m_pLst_Sell;
		m_pLst_Buy->ResetContent();
		m_pLst_Sell->ResetContent();
		pListBuy2->ResetContent();
		pListSell2->ResetContent();
		DWORD i;
		ACString strText, strGold;
		for(i = 0; i < p->prices.size(); i++ )
		{
			strGold.Format(GetStringFromTable(854), p->prices[i].volume);
			if( p->prices[i].price > 0 )
			{
				strText.Format(_AL("%s\t%s\t%s"), GetGameUIMan()->GetFormatNumber(p->prices[i].price), GetStringFromTable(853), strGold);
				m_pLst_Sell->AddString(strText);
				pListSell2->AddString(strText);
				m_pLst_Sell->SetItemTextColor(m_pLst_Sell->GetCount() - 1, A3DCOLORRGB(255, 255, 0));
				pListSell2->SetItemTextColor(pListSell2->GetCount() - 1, A3DCOLORRGB(255, 255, 0));
			}
			else
			{
				strText.Format(_AL("%s\t%s\t%s"), GetGameUIMan()->GetFormatNumber(-p->prices[i].price), GetStringFromTable(852), strGold);
				m_pLst_Buy->AddString(strText);
				pListBuy2->AddString(strText);
				m_pLst_Buy->SetItemTextColor(m_pLst_Buy->GetCount() - 1, A3DCOLORRGB(0, 255, 0));
				pListBuy2->SetItemTextColor(pListBuy2->GetCount() - 1, A3DCOLORRGB(0, 255, 0));
			}
		}
	}
	else if( idAction == CDlgGoldTrade::GOLD_ACTION_TRANSACTION_RE )
	{
		StockTransaction_Re *p = (StockTransaction_Re *)pData;
		if( !CheckRetcode(p->retcode, 849) )
			return;

		m_pTxt_Gold->SetData(p->cash);
		m_pTxt_Money4->SetData(p->money);
	}
	else if( idAction == CDlgGoldTrade::GOLD_ACTION_CANCEL_RE )
	{
		StockCancel_Re *p = (StockCancel_Re *)pData;
		if( !CheckRetcode(p->retcode, 850) )
			return;

		PAUILISTBOX pList =	GetGameUIMan()->m_pDlgGoldInquire->m_pLst_Inquire;
		for(int i = 0; i < pList->GetCount(); i++ )
			if( pList->GetItemData(i) == p->tid )
			{
				CSplit s(pList->GetText(i));
				CSPLIT_STRING_VECTOR vec = s.Split(_AL("\t"));
				vec[4] = GetStringFromTable(864);
				ACString strText;
				strText.Format(_AL("%s\t%s\t%s\t%s\t%s\t%s"), vec[0], vec[1], vec[2], vec[3], vec[4], vec[5]);
				pList->SetText(i, strText);
				pList->SetItemData(i, 0);
				break;
			}
	}
}

bool CDlgGoldTrade::CheckRetcode(int retcode, int defaulErrMsg)
{
	if( retcode == ERR_SUCCESS )
		return true;
	if( retcode == ERR_STOCK_CLOSED )
		GetGameUIMan()->MessageBox("", GetStringFromTable(855), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
	else if( retcode == ERR_STOCK_ACCOUNTBUSY )
		GetGameUIMan()->MessageBox("", GetStringFromTable(856), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
	else if( retcode == ERR_STOCK_INVALIDINPUT )
		GetGameUIMan()->MessageBox("", GetStringFromTable(857), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
	else if( retcode == ERR_STOCK_OVERFLOW )
		GetGameUIMan()->MessageBox("", GetStringFromTable(858), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
	else if( retcode == ERR_STOCK_DATABASE )
		GetGameUIMan()->MessageBox("", GetStringFromTable(859), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
	else if( retcode == ERR_STOCK_NOTENOUGH )
		GetGameUIMan()->MessageBox("", GetStringFromTable(860), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
	else if( retcode == ERR_STOCK_CASHLOCKED )
		GetGameUIMan()->MessageBox("", GetStringFromTable(871), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
	else
		GetGameUIMan()->MessageBox("", GetStringFromTable(defaulErrMsg), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
	return false;
}

void CDlgGoldTrade::SetLocked(bool bLock)
{
	m_bLocked = bLock;
	GetDlgItem("Btn_Pwd")->Show(!bLock);
	GetDlgItem("Btn_Consign")->Enable(!bLock);
	GetDlgItem("Btn_In")->Enable(!bLock);
	GetDlgItem("Btn_Out")->Enable(!bLock);
	GetDlgItem("Btn_Unlock")->Show(bLock);
	GetDlgItem("Txt_Lock")->Show(bLock);
	GetGameUIMan()->GetDialog("Win_GoldInquire")->GetDlgItem("Btn_Unlock")->Show(bLock);
	GetGameUIMan()->GetDialog("Win_GoldInquire")->GetDlgItem("Txt_Lock")->Show(bLock);
}