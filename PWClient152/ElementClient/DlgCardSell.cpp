// File		: DlgCardSell.cpp
// Creator	: Xiao Zhou
// Date		: 2006/1/19

#include "DlgCardSell.h"
#include "DlgInfo.h"
#include "EC_GameUIMan.h"
#include "EC_GameSession.h"
#include "EC_HostPlayer.h"
#include "getselllist_re.hpp"
#include "sellpoint_re.hpp"
#include "sellcancel_re.hpp"
#include "announcesellresult.hpp"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgCardSell, CDlgBase)

AUI_ON_COMMAND("confirm",		OnCommandConfirm)
AUI_ON_COMMAND("cancelsell",	OnCommandCancelSell)
AUI_ON_COMMAND("IDCANCEL",		OnCommandCancel)

AUI_END_COMMAND_MAP()

CDlgCardSell::CDlgCardSell()
{
	m_pTxt_Price = NULL;
	m_pLst_Item = NULL;
	m_pTxt_RestPoint = NULL;
	m_pTxt_SellPoint = NULL;
	m_pTxt_MaxPoint = NULL;
	m_pBtn_Confirm = NULL;
}

CDlgCardSell::~CDlgCardSell()
{
}

void CDlgCardSell::OnCommandConfirm(const char * szCommand)
{
	int nTime = 135000;
	int nPrice = a_atoi(m_pTxt_Price->GetText());
	if( nPrice > 0 )
		if( nPrice > 200000000 )
			GetGameUIMan()->MessageBox("",GetStringFromTable(755), MB_OK, 
				A3DCOLORRGBA(255, 255, 255, 160));
		else if( GetHostPlayer()->GetMoneyAmount() < 10000 )
			GetGameUIMan()->MessageBox("",GetStringFromTable(738), MB_OK, 
				A3DCOLORRGBA(255, 255, 255, 160));
		else if( GetGameSession()->GetAccountTime() >= nTime + 135000 + m_nSellPoint )
		{
			ACString strText;
			strText.Format(GetStringFromTable(753), 
				GetGameUIMan()->GetFormatNumber(nPrice), nTime / 60);
			PAUIDIALOG pMsgBox;
			GetGameUIMan()->MessageBox("Game_SellPoint", strText, MB_OKCANCEL,
				A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
			pMsgBox->SetData(nTime);
			pMsgBox->SetDataPtr((void*)nPrice);
		}
}

void CDlgCardSell::OnCommandCancelSell(const char * szCommand)
{
	int nSel = m_pLst_Item->GetCurSel();
	if( nSel >= 0 && nSel < m_pLst_Item->GetCount() )
	{
		ACString strText;
		strText.Format(GetStringFromTable(754), m_pLst_Item->GetItemData(nSel));
		PAUIDIALOG pMsgBox;
		GetGameUIMan()->MessageBox("Game_CancelSellPoint", strText, MB_OKCANCEL,
			A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
		pMsgBox->SetData(m_pLst_Item->GetItemData(nSel));
	}
}

void CDlgCardSell::OnCommandCancel(const char * szCommand)
{
	GetGameUIMan()->EndNPCService();
	Show(false);
}

bool CDlgCardSell::OnInitDialog()
{
	DDX_Control("Txt_Price", m_pTxt_Price);
	DDX_Control("Lst_Item", m_pLst_Item);
	DDX_Control("Txt_RestPoint", m_pTxt_RestPoint);
	DDX_Control("Txt_SellPoint", m_pTxt_SellPoint);
	DDX_Control("Txt_MaxPoint", m_pTxt_MaxPoint);
	DDX_Control("Btn_Confirm", m_pBtn_Confirm);
	m_pLst_Item->ResetContent();
	m_pTxt_Price->SetIsNumberOnly(true);

	return true;
}

void CDlgCardSell::OnShowDialog()
{
	m_nSellPoint = 2000000000;
	m_pTxt_Price->SetText(_AL(""));
	m_pLst_Item->ResetContent();
	GetGameSession()->account_GetSelfSellList();
}

void CDlgCardSell::OnTick()
{
	ACHAR szText[20];
	int nRest = int(GetGameSession()->GetAccountTime() / 1000);
	a_sprintf(szText, _AL("%d"), nRest / 60);
	m_pTxt_RestPoint->SetText(szText);
	if( m_nSellPoint != 2000000000)
		a_sprintf(szText, _AL("%d"), m_nSellPoint / 60);
	else
		a_strcpy(szText, GetStringFromTable(781) );
	m_pTxt_SellPoint->SetText(szText);
	int nMax = 0;
	if( nRest > m_nSellPoint + 135000 )
		nMax = nRest - m_nSellPoint - 135000;
	a_sprintf(szText, _AL("%d"), nMax / 60);
	m_pTxt_MaxPoint->SetText(szText);
	m_pBtn_Confirm->Enable(nMax >= 135000);
}

void CDlgCardSell::CardAction(int idAction, void *pData)
{
	if( idAction == CARD_ACTION_GETSELFSELLLIST_RE )
	{
		GetSellList_Re *p = (GetSellList_Re *)pData;
		m_pLst_Item->ResetContent();
		m_nSellPoint = 0;
		DWORD i;
		for(i = 0; i < p->list.size(); i++ )
		{
			ACHAR szText[256];
			a_sprintf(szText, GetStringFromTable(780), 
				p->list[i].sellid, p->list[i].point / 60, 
				GetGameUIMan()->GetFormatNumber(p->list[i].price));
			m_pLst_Item->AddString(szText);
			m_pLst_Item->SetItemData(i, p->list[i].sellid);
			m_pLst_Item->SetItemData(i, p->list[i].point, 1);
			m_nSellPoint += p->list[i].point;
		}
	}
	else if( idAction == CARD_ACTION_SELLPOINT_RE )
	{
		SellPoint_Re *p = (SellPoint_Re *)pData;
		if( p->retcode == ERR_SUCCESS )
		{
			ACHAR szText[256];
			a_sprintf(szText, GetStringFromTable(780), 
				p->info.sellid, p->info.point / 60, 
				GetGameUIMan()->GetFormatNumber(p->info.price));
			m_pLst_Item->AddString(szText);
			m_pLst_Item->SetItemData(m_pLst_Item->GetCount() - 1, p->info.sellid);
			m_pLst_Item->SetItemData(m_pLst_Item->GetCount() - 1, p->info.point, 1);
			m_nSellPoint += p->info.point;
		}
		else if( p->retcode == ERR_SP_SPARETIME )
			GetGameUIMan()->MessageBox("",GetStringFromTable(748), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		else if( p->retcode == ERR_SP_MONEYEXCEED )
			GetGameUIMan()->MessageBox("",GetStringFromTable(757), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		else if( p->retcode == ERR_TIMEOUT )
			GetGameUIMan()->MessageBox("",GetStringFromTable(759), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		else if( p->retcode == ERR_SP_EXCESS )
			GetGameUIMan()->MessageBox("",GetStringFromTable(749), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		else
			GetGameUIMan()->MessageBox("",GetStringFromTable(736), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
	}
	else if( idAction == CARD_ACTION_CANCELPOINT_RE )
	{
		SellCancel_Re *p = (SellCancel_Re *)pData;
		if( p->retcode == ERR_SUCCESS )
		{
			int i;
			for(i = 0; i < m_pLst_Item->GetCount(); i++ )
				if( (int)m_pLst_Item->GetItemData(i) == p->sellid )
				{
					m_nSellPoint -= m_pLst_Item->GetItemData(i, 1);
					m_pLst_Item->DeleteString(i);
					break;
				}
		}
		else if( p->retcode == ERR_SP_EXPIRED )
			GetGameUIMan()->MessageBox("",GetStringFromTable(746), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		else if( p->retcode == ERR_SP_SELLING )
			GetGameUIMan()->MessageBox("",GetStringFromTable(751), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		else if( p->retcode == ERR_TIMEOUT )
			GetGameUIMan()->MessageBox("",GetStringFromTable(759), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		else
			GetGameUIMan()->MessageBox("",GetStringFromTable(737), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
	}
	else if( idAction == CARD_ACTION_ANNOUNCESELLRESULT )
	{
		AnnounceSellResult *p = (AnnounceSellResult *)pData;
		ACString szText;
		if( GetHostPlayer()->GetCharacterID() == p->seller )
		{
			if ( p->retcode== ERR_SUCCESS )
			{
				szText.Format(GetStringFromTable(742), p->price, p->sellid, p->point / 60);
				GetGameUIMan()->AddInformation(CDlgInfo::INFO_SYSTEM, "Game_MsgBox", 
					szText, 0xFFFFFFF, 0, 0, 0);
			}
			else if ( p->retcode== ERR_TIMEOUT )
			{
				szText.Format(GetStringFromTable(743), p->sellid);
				GetGameUIMan()->AddInformation(CDlgInfo::INFO_SYSTEM, "Game_MsgBox", 
					szText, 0xFFFFFFF, 0, 0, 0);
			}
		}
		else 
		{
			if( p->retcode== ERR_SUCCESS )
			{
				szText.Format(GetStringFromTable(744), p->price, p->sellid, p->point / 60);
				GetGameUIMan()->AddInformation(CDlgInfo::INFO_SYSTEM, "Game_MsgBox", 
					szText, 0xFFFFFFF, 0, 0, 0);
			}
			else
			{
				szText.Format(GetStringFromTable(745), p->sellid, p->price);
				GetGameUIMan()->AddInformation(CDlgInfo::INFO_SYSTEM, "Game_MsgBox", 
					szText, 0xFFFFFFF, 0, 0, 0);
			}
		}
	}
}