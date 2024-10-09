// File		: DlgCardBuy.cpp
// Creator	: Xiao Zhou
// Date		: 2006/1/19

#include "DlgCardBuy.h"
#include "DlgCardSell.h"
#include "EC_GameUIMan.h"
#include "EC_GameSession.h"
#include "EC_HostPlayer.h"
#include "EC_Game.h"
#include "findsellpointinfo_re.hpp"
#include "buypoint_re.hpp"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgCardBuy, CDlgBase)

AUI_ON_COMMAND("refresh",		OnCommandRefresh)
AUI_ON_COMMAND("last",			OnCommandLast)
AUI_ON_COMMAND("next",			OnCommandNext)
AUI_ON_COMMAND("buy",			OnCommandBuy)
AUI_ON_COMMAND("IDCANCEL",		OnCommandCancel)

AUI_END_COMMAND_MAP()

CDlgCardBuy::CDlgCardBuy()
{
	m_pLst_Item = NULL;
	m_pTxt_RestPoint = NULL;
}

CDlgCardBuy::~CDlgCardBuy()
{
}

void CDlgCardBuy::OnCommandLast(const char * szCommand)
{
	if( m_nCurPage > 0 )
	{
		m_nCurPage--;
		GetGameSession()->account_GetShopSellList(m_nCurPage * 16, false);
	}
	else
		GetGameUIMan()->MessageBox("",GetStringFromTable(758), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
}

void CDlgCardBuy::OnCommandNext(const char * szCommand)
{
	m_nCurPage++;
	GetGameSession()->account_GetShopSellList(m_nCurPage * 16, false);
}

void CDlgCardBuy::OnCommandRefresh(const char * szCommand)
{
	GetGameSession()->account_GetShopSellList(m_nCurPage * 16, false);
}

void CDlgCardBuy::OnCommandBuy(const char * szCommand)
{
	int nSel = m_pLst_Item->GetCurSel();
	if( nSel >= 0 && nSel < m_pLst_Item->GetCount() )
		if( GetHostPlayer()->GetMoneyAmount() < (int)m_pLst_Item->GetItemData(nSel, 2) )
			GetGameUIMan()->MessageBox("", GetStringFromTable(750), 
				MB_OK, A3DCOLORRGBA(255, 255, 255, 160) );
		else	
		{
			ACString strText;
			strText.Format(GetStringFromTable(752), 
				GetGameUIMan()->GetFormatNumber(m_pLst_Item->GetItemData(nSel, 2)), 
				m_pLst_Item->GetItemData(nSel),
				m_pLst_Item->GetItemData(nSel, 3) / 60);
			PAUIDIALOG pMsgBox;
			GetGameUIMan()->MessageBox("Game_BuyPoint", strText, MB_OKCANCEL,
				A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
			pMsgBox->SetData(m_pLst_Item->GetItemData(nSel));
			pMsgBox->SetDataPtr((void*)m_pLst_Item->GetItemData(nSel, 1));
		}
}

void CDlgCardBuy::OnCommandCancel(const char * szCommand)
{
	GetGameUIMan()->EndNPCService();
	Show(false);
}

bool CDlgCardBuy::OnInitDialog()
{
	DDX_Control("Lst_Item", m_pLst_Item);
	DDX_Control("Txt_RestPoint", m_pTxt_RestPoint);

	return true;
}

void CDlgCardBuy::OnShowDialog()
{
	m_nCurPage = 0;
	GetGameSession()->account_GetShopSellList(m_nCurPage * 16, false);
	m_pLst_Item->ResetContent();
}

void CDlgCardBuy::OnTick()
{
	ACHAR szText[20];
	int nRest = int(GetGameSession()->GetAccountTime() / 1000);
	a_sprintf(szText, _AL("%d"), nRest / 60);
	m_pTxt_RestPoint->SetText(szText);
	int i;
	for(i = 0; i < m_pLst_Item->GetCount(); i++ )
		if( GetGame()->GetServerGMTTime() > (int)m_pLst_Item->GetItemData(i, 4) - 300 )
		{
			m_pLst_Item->DeleteString(i);
			i--;
		}
}

void CDlgCardBuy::CardAction(int idAction, void *pData)
{
	if( idAction == CDlgCardSell::CARD_ACTION_GETSHOPSELLLIST_RE )
	{
		FindSellPointInfo_Re *p = (FindSellPointInfo_Re *)pData;
		m_pLst_Item->ResetContent();
		DWORD i;
		for(i = 0; i < p->list.size(); i++ )
		{
			ACHAR szText[256];
			a_sprintf(szText, GetStringFromTable(780), 
				p->list[i].sellid, p->list[i].point / 60, 
				GetGameUIMan()->GetFormatNumber(p->list[i].price));
			m_pLst_Item->AddString(szText);
			m_pLst_Item->SetItemData(i, p->list[i].sellid);
			m_pLst_Item->SetItemData(i, p->list[i].roleid, 1);
			m_pLst_Item->SetItemData(i, p->list[i].price, 2);
			m_pLst_Item->SetItemData(i, p->list[i].point, 3);
			m_pLst_Item->SetItemData(i, p->list[i].etime, 4);
		}
	}
	else if( idAction == CDlgCardSell::CARD_ACTION_BUYPOINT_RE )
	{
		BuyPoint_Re *p = (BuyPoint_Re *)pData;
		if( p->retcode == ERR_SUCCESS )
		{
			GetGameSession()->account_GetShopSellList(m_nCurPage * 16, false);
			m_pLst_Item->ResetContent();
			GetGameUIMan()->MessageBox("",GetStringFromTable(740), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		}
		else if( p->retcode == ERR_SP_EXPIRED )
			GetGameUIMan()->MessageBox("",GetStringFromTable(746), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		else if( p->retcode == ERR_SP_BUYSELF )
			GetGameUIMan()->MessageBox("",GetStringFromTable(747), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		else if( p->retcode == ERR_SP_NOMONEY )
			GetGameUIMan()->MessageBox("",GetStringFromTable(750), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		else if( p->retcode == ERR_TIMEOUT )
			GetGameUIMan()->MessageBox("",GetStringFromTable(759), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		else
			GetGameUIMan()->MessageBox("",GetStringFromTable(741), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
	}
}
