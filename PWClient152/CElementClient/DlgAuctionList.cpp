// Filename	: DlgAuctionList.cpp
// Creator	: Xiao Zhou
// Date		: 2005/11/10

#include "DlgAuctionList.h"
#include "DlgAuctionBuyList.h"
#include "DlgAuctionSellList.h"
#include "EC_GameUIMan.h"
#include "CSplit.h"
#include "auctionexitbid_re.hpp"
#include "auctiongetitem_re.hpp"
#include "gnetdef.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgAuctionList, CDlgBase)

AUI_ON_COMMAND("IDCANCEL",			OnCommandCancel)
AUI_ON_COMMAND("auctionbuylist",	OnCommandAuctionBuyList)
AUI_ON_COMMAND("auctionlist",		OnCommandAuctionList)
AUI_ON_COMMAND("auctionselllist",	OnCommandAuctionSellList)
AUI_ON_COMMAND("auctionmyfavor",	OnCommandAuctionMyFavor)
AUI_ON_COMMAND("buy",				OnCommandBuy)
AUI_ON_COMMAND("binbuy",			OnCommandBinBuy)
AUI_ON_COMMAND("giveup",			OnCommandGiveUp)

AUI_END_COMMAND_MAP()

CDlgAuctionList::CDlgAuctionList()
{
}

CDlgAuctionList::~CDlgAuctionList()
{
}

void CDlgAuctionList::OnCommandGiveUp(const char* szCommand)
{
	int nSel = m_pLst_Item->GetCurSel();
	if( nSel >= 0 && nSel < m_pLst_Item->GetCount() )
	{
		PAUIDIALOG pMsgBox;
		CSplit s(m_pLst_Item->GetText(nSel));
		CSPLIT_STRING_VECTOR vec = s.Split(_AL("\t"));
		ACString szText;
		szText.Format(GetStringFromTable(694), vec[1], m_pLst_Item->GetItemData(nSel));
		GetGameUIMan()->MessageBox("Game_AuctionGiveUp", szText, MB_YESNO, 
			A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
		pMsgBox->SetData(m_pLst_Item->GetItemData(nSel));
	}
}

bool CDlgAuctionList::OnInitDialog()
{
	return CDlgAuctionBase::OnInitDialog();
}

void CDlgAuctionList::OnShowDialog()
{
	CDlgAuctionBase::OnShowDialog();

	m_pTxt_Price->SetText(_AL("0"));
	m_pBtn_AuctionList->SetPushed(true);
}

void CDlgAuctionList::OnTick()
{
	CDlgAuctionBase::OnTick();
}

void CDlgAuctionList::AuctionAction(int idAction, void *pData)
{
	if( idAction == AUCTION_ACTION_OPEN_RE )
	{
		GetGameUIMan()->m_pDlgAuctionSellList->SellRe(pData);
	}
	else if( idAction == AUCTION_ACTION_BID_RE )
	{
		if( GetGameUIMan()->m_pDlgAuctionBuyList->IsShow() )
			GetGameUIMan()->m_pDlgAuctionBuyList->BidRe(pData);
		else
			GetGameUIMan()->m_pDlgAuctionList->BidRe(pData);
	}
	else if( idAction == AUCTION_ACTION_LIST_RE )
	{
		GetGameUIMan()->m_pDlgAuctionBuyList->UpdateList(pData);
	}
	else if( idAction == AUCTION_ACTION_CLOSE_RE )
	{
		GetGameUIMan()->m_pDlgAuctionSellList->CloseRe(pData);
	}
	else if( idAction == AUCTION_ACTION_GET_RE )
	{
		GetGameUIMan()->m_pDlgAuctionBuyList->UpdateItem(pData);
	}
	else if( idAction == AUCTION_ACTION_ATTENDLIST_RE )
	{
		UpdateList(pData);
		GetGameUIMan()->m_pDlgAuctionSellList->UpdateList(pData);
	}
	else if( idAction == AUCTION_ACTION_EXITBID_RE )
	{
		AuctionExitBid_Re *p = (AuctionExitBid_Re *)pData;
		if( p->retcode == ERR_SUCCESS )
		{
			int i;
			for( i = 0; i < m_pLst_Item->GetCount(); i++ )
				if( m_pLst_Item->GetItemData(i) == p->auctionid )
				{
					m_pLst_Item->DeleteString(i);
					break;
				}
		}
		else
		{
			int i;
			for( i = 0; i < m_pLst_Item->GetCount(); i++ )
				if( m_pLst_Item->GetItemData(i) == p->auctionid )
				{
					CSplit s(m_pLst_Item->GetText(i));
					CSPLIT_STRING_VECTOR vec = s.Split(_AL("\t"));
					ACString szText;
					szText.Format(GetStringFromTable(695), vec[1], p->auctionid);
					GetGameUIMan()->MessageBox("",szText, MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
					break;
				}
		}
	}
	else if( idAction == AUCTION_ACTION_GETITEM_RE )
	{
		AuctionGetItem_Re *p = (AuctionGetItem_Re *)pData;
		DWORD i;
		for(i = 0; i < p->items.size(); i++ )
		{
			GetGameUIMan()->m_pDlgAuctionBuyList->m_mapAuctionHint[p->ids[i]]
				= p->items[i];
			GetGameUIMan()->m_pDlgAuctionSellList->UpdateHint(p->ids[i]);
			GetGameUIMan()->m_pDlgAuctionBuyList->UpdateHint(p->ids[i]);
			GetGameUIMan()->m_pDlgAuctionMyFavor->UpdateHint(p->ids[i]);
			UpdateHint(p->ids[i]);
		}
		//需要更新我的收藏的Hint
	}
	else if (idAction == AUCTION_ACTION_LISTUPDATE_RE)
	{
		GetGameUIMan()->m_pDlgAuctionMyFavor->UpdateList(pData);
	}
		
}
