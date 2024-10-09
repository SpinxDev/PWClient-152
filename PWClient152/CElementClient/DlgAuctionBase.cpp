// Filename	: DlgAuctionBase.cpp
// Creator	: Xiao Zhou
// Date		: 2005/11/10

#include "DlgAuctionBase.h"
#include "DlgAuctionBuyList.h"
#include "DlgAuctionList.h"
#include "DlgAuctionSellList.h"
#include "CSplit.h"
#include "AUICTranslate.h"
#include "EC_Game.h"
#include "EC_GameUIMan.h"
#include "EC_GameSession.h"
#include "EC_HostPlayer.h"
#include "EC_IvtrItem.h"
#include "auctionget_re.hpp"
#include "auctionattendlist_re.hpp"
#include "auctionlist_re.hpp"
#include "auctionbid_re.hpp"
#include "auctionlistupdate_re.hpp"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgAuctionBase, CDlgBase)

AUI_ON_COMMAND("IDCANCEL",			OnCommandCancel)
AUI_ON_COMMAND("auctionbuylist",	OnCommandAuctionBuyList)
AUI_ON_COMMAND("auctionlist",		OnCommandAuctionList)
AUI_ON_COMMAND("auctionselllist",	OnCommandAuctionSellList)
AUI_ON_COMMAND("auctionmyfavor",	OnCommandAuctionMyFavor)
AUI_ON_COMMAND("buy",				OnCommandBuy)
AUI_ON_COMMAND("binbuy",			OnCommandBinBuy)
AUI_ON_COMMAND("sortid",			OnCommandSortID)
AUI_ON_COMMAND("sortname",			OnCommandSortName)
AUI_ON_COMMAND("sorttime",			OnCommandSortTime)
AUI_ON_COMMAND("sortprice",			OnCommandSortPrice)
AUI_ON_COMMAND("sortbinprice",		OnCommandSortBinPrice)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgAuctionBase, CDlgBase)

AUI_ON_EVENT("Lst_Item",	WM_LBUTTONDOWN,	OnEventLButtonDown_Lst_Item)

AUI_END_EVENT_MAP()


CDlgAuctionBase::CDlgAuctionBase()
{
	m_pBtn_AuctionBuyList = NULL;
	m_pBtn_AuctionList = NULL;
	m_pBtn_AuctionSellList = NULL;
	m_pBtn_Btn_MyAttention = NULL;
	m_pLst_Item = NULL;
	m_pTxt_Price = NULL;
	m_bSortID = false;
	m_bSortName = false;
	m_bSortTime = false;
	m_bSortPrice = false;
	m_bSortBinPrice = false;
}

CDlgAuctionBase::~CDlgAuctionBase()
{
}

bool CDlgAuctionBase::OnInitDialog()
{
	DDX_Control("Btn_AuctionBuyList", m_pBtn_AuctionBuyList);
	DDX_Control("Btn_AuctionList", m_pBtn_AuctionList);
	DDX_Control("Btn_AuctionSellList", m_pBtn_AuctionSellList);
	DDX_Control("Btn_MyAttention", m_pBtn_Btn_MyAttention);
	DDX_Control("Lst_Item", m_pLst_Item);
	m_pTxt_Price = dynamic_cast<PAUIEDITBOX>(GetDlgItem("Txt_Price"));
	if( m_pTxt_Price )
		m_pTxt_Price->SetIsNumberOnly(true);

	return true;
}

void CDlgAuctionBase::OnShowDialog()
{
}

void CDlgAuctionBase::OnCommandCancel(const char* szCommand)
{
	Show(false);
	GetGameUIMan()->EndNPCService();
	GetGameUIMan()->m_pDlgAuctionBuyList->m_pLst_Item->ResetContent();
	GetGameUIMan()->m_pDlgAuctionList->m_pLst_Item->ResetContent();
	GetGameUIMan()->m_pDlgAuctionSellList->m_pLst_Item->ResetContent();
	GetGameUIMan()->m_pDlgAuctionMyFavor->m_pLst_Item->ResetContent();
}

void CDlgAuctionBase::OnCommandAuctionBuyList(const char* szCommand)
{
	GetGameUIMan()->m_pDlgAuctionList->Show(false);
	GetGameUIMan()->m_pDlgAuctionSellList->Show(false);
	GetGameUIMan()->m_pDlgAuctionMyFavor->Show(false);
	GetGameUIMan()->m_pDlgAuctionBuyList->SetPosEx(GetPos().x, GetPos().y);
	GetGameUIMan()->m_pDlgAuctionBuyList->Show(true);
}

void CDlgAuctionBase::OnCommandAuctionList(const char* szCommand)
{
	GetGameUIMan()->m_pDlgAuctionBuyList->Show(false);
	GetGameUIMan()->m_pDlgAuctionSellList->Show(false);
	GetGameUIMan()->m_pDlgAuctionMyFavor->Show(false);
	GetGameUIMan()->m_pDlgAuctionList->SetPosEx(GetPos().x, GetPos().y);
	GetGameUIMan()->m_pDlgAuctionList->Show(true);
}

void CDlgAuctionBase::OnCommandAuctionSellList(const char* szCommand)
{
	GetGameUIMan()->m_pDlgAuctionBuyList->Show(false);
	GetGameUIMan()->m_pDlgAuctionList->Show(false);
	GetGameUIMan()->m_pDlgAuctionMyFavor->Show(false);
	GetGameUIMan()->m_pDlgAuctionSellList->SetPosEx(GetPos().x, GetPos().y);
	GetGameUIMan()->m_pDlgAuctionSellList->Show(true);
}

void CDlgAuctionBase::OnCommandAuctionMyFavor(const char* szCommand)
{
	GetGameUIMan()->m_pDlgAuctionBuyList->Show(false);
	GetGameUIMan()->m_pDlgAuctionList->Show(false);
	GetGameUIMan()->m_pDlgAuctionSellList->Show(false);
	GetGameUIMan()->m_pDlgAuctionMyFavor->SetPosEx(GetPos().x, GetPos().y);
	GetGameUIMan()->m_pDlgAuctionMyFavor->Show(true);
}

void CDlgAuctionBase::OnEventLButtonDown_Lst_Item(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	if( stricmp(m_szName, "Win_AuctionSellList") == 0 )
		return;

	int nSel = m_pLst_Item->GetCurSel();
	if( nSel >= 0 && nSel < m_pLst_Item->GetCount() )
	{
		DWORD nLastPrice = m_pLst_Item->GetItemData(nSel, 2);
		int nBinPrice = m_pLst_Item->GetItemData(nSel, 3);
		int nPrice;
		if( (int)m_pLst_Item->GetItemData(nSel, 4) == 0 )
			nPrice = nLastPrice;
		else
			nPrice = nLastPrice + (nLastPrice - 1) / 20 + 1;
		
		if( nBinPrice > 0 && nPrice > nBinPrice )
			nPrice = m_pLst_Item->GetItemData(nSel, 3);
		ACHAR szText[20];
		a_sprintf(szText, _AL("%d"), nPrice);
		m_pTxt_Price->SetText(szText);
	}
	else
		m_pTxt_Price->SetText(_AL("0"));
}


void CDlgAuctionBase::OnCommandBuy(const char* szCommand)
{
	int nSel = m_pLst_Item->GetCurSel();
	if( nSel >= 0 && nSel < m_pLst_Item->GetCount() )
	{
		if( (int)m_pLst_Item->GetItemData(nSel, 1) == GetHostPlayer()->GetCharacterID() )
		{
			GetGameUIMan()->MessageBox("",GetStringFromTable(673), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
			return;			
		}
		int nPrice = a_atoi(m_pTxt_Price->GetText());
		int nBinPrice = (int)m_pLst_Item->GetItemData(nSel, 3);
		if( nBinPrice > 0 && nPrice >= nBinPrice )
		{
			OnCommandBinBuy("");
			return;
		}

		DWORD nLastPrice = m_pLst_Item->GetItemData(nSel, 2);
		if( (int)m_pLst_Item->GetItemData(nSel, 4) != 0 )
			nLastPrice = nLastPrice + (nLastPrice - 1) / 20 + 1;
		if( nPrice < int(nLastPrice) )
			GetGameUIMan()->MessageBox("",GetStringFromTable(655), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		else if( nPrice > GetHostPlayer()->GetMoneyAmount() )
			GetGameUIMan()->MessageBox("",GetStringFromTable(656), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		else
		{
			PAUIDIALOG pMsgBox;
			CSplit s(m_pLst_Item->GetText(nSel));
			CSPLIT_STRING_VECTOR vec = s.Split(_AL("\t"));
			ACString szText;
			if( (int)m_pLst_Item->GetItemData(nSel, 4) == GetHostPlayer()->GetCharacterID() )
				szText.Format(GetStringFromTable(663), GetGameUIMan()->GetFormatNumber(nPrice), vec[1], m_pLst_Item->GetItemData(nSel), GetStringFromTable(728));
			else
				szText.Format(GetStringFromTable(663), GetGameUIMan()->GetFormatNumber(nPrice), vec[1], m_pLst_Item->GetItemData(nSel), _AL(""));
			GetGameUIMan()->MessageBox("Game_AuctionBid", szText, MB_YESNO, 
				A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
			pMsgBox->SetData(m_pLst_Item->GetItemData(nSel));
			pMsgBox->SetDataPtr((void*)nPrice);
		}
	}
}

void CDlgAuctionBase::OnCommandBinBuy(const char* szCommand)
{
	int nSel = m_pLst_Item->GetCurSel();
	if( nSel >= 0 && nSel < m_pLst_Item->GetCount() )
	{
		if( (int)m_pLst_Item->GetItemData(nSel, 1) == GetHostPlayer()->GetCharacterID() )
		{
			GetGameUIMan()->MessageBox("",GetStringFromTable(673), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
			return;			
		}
		int nPrice = m_pLst_Item->GetItemData(nSel, 3);
		if( nPrice == 0 )
			GetGameUIMan()->MessageBox("",GetStringFromTable(662), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		else if( nPrice > GetHostPlayer()->GetMoneyAmount() )
			GetGameUIMan()->MessageBox("",GetStringFromTable(656), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		else
		{
			PAUIDIALOG pMsgBox;
			CSplit s(m_pLst_Item->GetText(nSel));
			CSPLIT_STRING_VECTOR vec = s.Split(_AL("\t"));
			ACString szText;
			if( (int)m_pLst_Item->GetItemData(nSel, 4) == GetHostPlayer()->GetCharacterID() )
				szText.Format(GetStringFromTable(664), GetGameUIMan()->GetFormatNumber(nPrice),
				vec[1], m_pLst_Item->GetItemData(nSel), GetStringFromTable(728));
			else
				szText.Format(GetStringFromTable(664), GetGameUIMan()->GetFormatNumber(nPrice),
				vec[1], m_pLst_Item->GetItemData(nSel), _AL(""));
			GetGameUIMan()->MessageBox("Game_AuctionBin", szText, MB_YESNO, 
				A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
			pMsgBox->SetData(m_pLst_Item->GetItemData(nSel));
			pMsgBox->SetDataPtr((void*)nPrice);
		}
	}
}

void CDlgAuctionBase::OnCommandSortID(const char* szCommand)
{
	m_bSortID = !m_bSortID;
	if( m_bSortID )
		m_pLst_Item->SortItems(AUILISTBOX_SORT_ASCENT, AUILISTBOX_SORTTYPE_INT, 0);
	else
		m_pLst_Item->SortItems(AUILISTBOX_SORT_DESCENT, AUILISTBOX_SORTTYPE_INT, 0);
}

void CDlgAuctionBase::OnCommandSortName(const char* szCommand)
{
	m_bSortName = !m_bSortName;
	if( m_bSortName )
		m_pLst_Item->SortItems(AUILISTBOX_SORT_ASCENT, AUILISTBOX_SORTTYPE_STRING, 1);
	else
		m_pLst_Item->SortItems(AUILISTBOX_SORT_DESCENT, AUILISTBOX_SORTTYPE_STRING, 1);
}

void CDlgAuctionBase::OnCommandSortTime(const char* szCommand)
{
	m_bSortTime = !m_bSortTime;
	if( m_bSortTime )
		m_pLst_Item->SortItemsByData(AUILISTBOX_SORT_ASCENT, AUILISTBOX_SORTTYPE_INT, 5);
	else
		m_pLst_Item->SortItemsByData(AUILISTBOX_SORT_DESCENT, AUILISTBOX_SORTTYPE_INT, 5);
}

void CDlgAuctionBase::OnCommandSortPrice(const char* szCommand)
{
	m_bSortPrice = !m_bSortPrice;
	if( m_bSortPrice )
		m_pLst_Item->SortItems(AUILISTBOX_SORT_ASCENT, AUILISTBOX_SORTTYPE_INT, 3);
	else
		m_pLst_Item->SortItems(AUILISTBOX_SORT_DESCENT, AUILISTBOX_SORTTYPE_INT, 3);
}

void CDlgAuctionBase::OnCommandSortBinPrice(const char* szCommand)
{
	m_bSortBinPrice = !m_bSortBinPrice;
	if( m_bSortBinPrice )
		m_pLst_Item->SortItems(AUILISTBOX_SORT_ASCENT, AUILISTBOX_SORTTYPE_INT, 4);
	else
		m_pLst_Item->SortItems(AUILISTBOX_SORT_DESCENT, AUILISTBOX_SORTTYPE_INT, 4);
}

void CDlgAuctionBase::UpdateHint(DWORD auctionid)
{
	int i;
	for(i = 0; i < m_pLst_Item->GetCount(); i++ )
		if( m_pLst_Item->GetItemData(i) == auctionid )
		{
			AuctionHintMap::iterator it = GetGameUIMan()->m_pDlgAuctionBuyList->m_mapAuctionHint.find(m_pLst_Item->GetItemData(i));
			if( it != GetGameUIMan()->m_pDlgAuctionBuyList->m_mapAuctionHint.end() )
			{
				const GNET::GRoleInventory & info = it->second;
				CECIvtrItem *pItem = CECIvtrItem::CreateItem(info.id, info.expire_date, info.count);
				pItem->SetItemInfo((unsigned char*)info.data.begin(), info.data.size());
				pItem->SetProcType(info.proctype);
				
				ACString strHint = GetGameUIMan()->GetItemDescHint(pItem, true, true);
				m_pLst_Item->SetItemHint(i, strHint);
				

				delete pItem;
			}
		}
}

void CDlgAuctionBase::UpdateList(void *pData)
{
	m_pLst_Item->ResetContent();
	std::vector<GAuctionItem> *items;
	int nType;
	if( stricmp(m_szName, "Win_AuctionBuyList") == 0 || stricmp(m_szName, "Win_AuctionMyAttention") == 0)
		nType = 0;
	else if( stricmp(m_szName, "Win_AuctionSellList") == 0 )
		nType = 1;
	else
		nType = 2;
	if( nType == 0 )
	{
		if (stricmp(m_szName, "Win_AuctionBuyList") == 0)
		{
			AuctionList_Re *pList = (AuctionList_Re *)pData;
			items = &pList->items;
			int PageEnd = (int)pList->end;
			if (items->size() > 0)
			{
				CDlgAuctionBuyList *pDlg = GetGameUIMan()->m_pDlgAuctionBuyList;
				if (PageEnd > pDlg->m_nPageBegin)
				{
					pDlg->m_nPageEnd = PageEnd - 1;
				}
				else
				{
					pDlg->m_nPageEnd = pDlg->m_nPageBegin;
					pDlg->m_nPageBegin = PageEnd + 1;
				}
			}
		}
		else if (stricmp(m_szName, "Win_AuctionMyAttention") == 0)
		{
			AuctionListUpdate_Re *pList = (AuctionListUpdate_Re *)pData;
			items = &pList->items;
		}
	}
	else
	{
		AuctionAttendList_Re *pList = (AuctionAttendList_Re *)pData;
		items = &pList->items;
	}
	DWORD i;
	int idNeedGet[100];
	int nNeedGet = 0;
	for(i = 0; i < items->size(); i++ )
	{
		GAuctionItem pAuctionItem = (*items)[i];
		if( nType != 0)
		{
			bool bHostSell = (int)pAuctionItem.seller == GetHostPlayer()->GetCharacterID();
			if( nType == 2 && bHostSell || nType == 1 && !bHostSell )
				continue;
		}

		CECIvtrItem *pItem = CECIvtrItem::CreateItem(pAuctionItem.itemid, 0, pAuctionItem.count);
		ACString szText, szTime, szName;
		int nRestTime = pAuctionItem.end_time - GetGame()->GetServerGMTTime();
		if( nRestTime > 3600 )
			szTime.Format(GetStringFromTable(1603), nRestTime / 3600);
		else
			szTime.Format(GetStringFromTable(1602), nRestTime / 60);
		if( pAuctionItem.count > 1)
			szName.Format(_AL("%s(%d)"), pItem->GetName(), pAuctionItem.count);
		else
			szName = pItem->GetName();

		if( pAuctionItem.binprice > 0 )
			szText.Format(_AL("%d\t%s\t%s\t%s\t%s"), pAuctionItem.auctionid, szName, 
				szTime, GetGameUIMan()->GetFormatNumber(pAuctionItem.bidprice), 
				GetGameUIMan()->GetFormatNumber(pAuctionItem.binprice));
		else
			szText.Format(_AL("%d\t%s\t%s\t%s\t "), pAuctionItem.auctionid, szName, 
				szTime, GetGameUIMan()->GetFormatNumber(pAuctionItem.bidprice));
		m_pLst_Item->AddString(szText);

		m_pLst_Item->SetItemData(m_pLst_Item->GetCount() - 1, pAuctionItem.auctionid);
		m_pLst_Item->SetItemData(m_pLst_Item->GetCount() - 1, pAuctionItem.seller, 1);
		m_pLst_Item->SetItemData(m_pLst_Item->GetCount() - 1, pAuctionItem.bidprice, 2);
		m_pLst_Item->SetItemData(m_pLst_Item->GetCount() - 1, pAuctionItem.binprice, 3);
		m_pLst_Item->SetItemData(m_pLst_Item->GetCount() - 1, pAuctionItem.bidder, 4);
		m_pLst_Item->SetItemData(m_pLst_Item->GetCount() - 1, pAuctionItem.end_time, 5);
		m_pLst_Item->SetItemDataPtr(m_pLst_Item->GetCount() - 1, (void *)pAuctionItem.itemid);
		if( nType == 2 )
			if( (int)pAuctionItem.bidder == GetHostPlayer()->GetCharacterID() )
				m_pLst_Item->SetItemTextColor(m_pLst_Item->GetCount() - 1, A3DCOLORRGB(255, 255, 255));
			else
				m_pLst_Item->SetItemTextColor(m_pLst_Item->GetCount() - 1, A3DCOLORRGB(128, 128, 128));

		if( nType != 2 || (int)pAuctionItem.bidder == GetHostPlayer()->GetCharacterID() )
		{
			m_pLst_Item->SetItemTextColor(m_pLst_Item->GetCount()-1, GetGameUIMan()->GetPriceColor(pAuctionItem.bidprice), 3);
			m_pLst_Item->SetItemTextColor(m_pLst_Item->GetCount()-1, GetGameUIMan()->GetPriceColor(pAuctionItem.binprice), 4);
		}

		/*m_pLst_Item->SetItemData(m_pLst_Item->GetCount() - 1, pAuctionItem.end_time, 5);*/
		AuctionHintMap::iterator it = GetGameUIMan()->m_pDlgAuctionBuyList->m_mapAuctionHint.find(pAuctionItem.auctionid);
		if( it != GetGameUIMan()->m_pDlgAuctionBuyList->m_mapAuctionHint.end() )
		{
			const GNET::GRoleInventory &info = it->second;
			pItem->SetItemInfo((unsigned char*)info.data.begin(), info.data.size());
			pItem->SetProcType(info.proctype);
			
			ACString strHint = GetGameUIMan()->GetItemDescHint(pItem, true, true);
			m_pLst_Item->SetItemHint(m_pLst_Item->GetCount() - 1, strHint);
		}
		else
			idNeedGet[nNeedGet++] = pAuctionItem.auctionid;
		delete pItem;
	}
	while( nNeedGet > 8 )
	{
		GetGameSession()->auction_GetItems(8, idNeedGet + nNeedGet - 8);
		nNeedGet -= 8;
	}
	if( nNeedGet > 0 )
		GetGameSession()->auction_GetItems(nNeedGet, idNeedGet);
}

void CDlgAuctionBase::BidRe(void *pData)
{
	AuctionBid_Re *p = (AuctionBid_Re *)pData;
	if( p->retcode == ERR_AS_MARKET_UNOPEN )
		GetGameUIMan()->MessageBox("",GetStringFromTable(672), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
	else if( p->retcode == ERR_AS_ATTEND_OVF )
	{
		GetGameUIMan()->MessageBox("",GetStringFromTable(669), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
	}
	else if( p->retcode == ERR_AS_MAILBOXFULL )
		GetGameUIMan()->MessageBox("",GetStringFromTable(727), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
	else if( p->retcode == ERR_SUCCESS )
	{
		GetGameUIMan()->MessageBox("",GetStringFromTable(665), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		int i;
		PAUILISTBOX pList;
		pList = GetGameUIMan()->m_pDlgAuctionBuyList->m_pLst_Item;
		for( i = 0; i < pList->GetCount(); i++ )
			if( pList->GetItemData(i) == p->auctionid )
			{
				pList->SetItemData(i, GetHostPlayer()->GetCharacterID(), 4);
				pList->SetItemData(i, p->bidprice, 2);
				CSplit s(pList->GetText(i));
				CSPLIT_STRING_VECTOR vec = s.Split(_AL("\t"));
				ACString szText;
				szText.Format(_AL("%s\t%s\t%s\t%s\t%s"), vec[0], vec[1], vec[2], 
					GetGameUIMan()->GetFormatNumber(p->bidprice), vec[4]);
				pList->SetText(i, szText);
				break;
			}
		pList = GetGameUIMan()->m_pDlgAuctionList->m_pLst_Item;
		for( i = 0; i < pList->GetCount(); i++ )
			if( pList->GetItemData(i) == p->auctionid )
			{
				pList->SetItemData(i, GetHostPlayer()->GetCharacterID(), 4);
				pList->SetItemData(i, p->bidprice, 2);
				CSplit s(pList->GetText(i));
				CSPLIT_STRING_VECTOR vec = s.Split(_AL("\t"));
				ACString szText;
				szText.Format(_AL("%s\t%s\t%s\t%s\t%s"), vec[0], vec[1], vec[2], 
					GetGameUIMan()->GetFormatNumber(p->bidprice), vec[4]);
				pList->SetText(i, szText);
				break;
			}
		GetGameSession()->auction_AttendList();
	}
	else if( p->retcode == ERR_AS_BID_BINSUCCESS )
	{
		GetGameUIMan()->MessageBox("",GetStringFromTable(666), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		int i;
		PAUILISTBOX pList;
		pList = GetGameUIMan()->m_pDlgAuctionBuyList->m_pLst_Item;
		for( i = 0; i < pList->GetCount(); i++ )
			if( pList->GetItemData(i) == p->auctionid )
			{
				pList->DeleteString(i);
				break;
			}
		pList = GetGameUIMan()->m_pDlgAuctionList->m_pLst_Item;
		for( i = 0; i < pList->GetCount(); i++ )
			if( pList->GetItemData(i) == p->auctionid )
			{
				pList->DeleteString(i);
				break;
			}
	}
	else if( p->retcode == ERR_AS_BID_LOWBID )
	{
		GetGameUIMan()->MessageBox("",GetStringFromTable(667), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		int i;
		PAUILISTBOX pList;
		pList = GetGameUIMan()->m_pDlgAuctionBuyList->m_pLst_Item;
		for( i = 0; i < pList->GetCount(); i++ )
			if( pList->GetItemData(i) == p->auctionid )
			{
				pList->SetItemData(i, 1, 4);
				pList->SetItemData(i, p->bidprice, 2);
				CSplit s(pList->GetText(i));
				CSPLIT_STRING_VECTOR vec = s.Split(_AL("\t"));
				ACString szText;
				szText.Format(_AL("%s\t%s\t%s\t%s\t%s"), vec[0], vec[1], vec[2], 
					GetGameUIMan()->GetFormatNumber(p->bidprice), vec[4]);
				pList->SetText(i, szText);
				break;
			}
		pList = GetGameUIMan()->m_pDlgAuctionList->m_pLst_Item;
		for( i = 0; i < pList->GetCount(); i++ )
			if( pList->GetItemData(i) == p->auctionid )
			{
				pList->SetItemData(i, 1, 4);
				pList->SetItemData(i, p->bidprice, 2);
				CSplit s(pList->GetText(i));
				CSPLIT_STRING_VECTOR vec = s.Split(_AL("\t"));
				ACString szText;
				szText.Format(_AL("%s\t%s\t%s\t%s\t%s"), vec[0], vec[1], vec[2], 
					GetGameUIMan()->GetFormatNumber(p->bidprice), vec[4]);
				pList->SetText(i, szText);
				break;
			}
	}
	else if( p->retcode == ERR_AS_BID_NOTFOUND )
	{
		int i;
		PAUILISTBOX pList;
		pList = GetGameUIMan()->m_pDlgAuctionBuyList->m_pLst_Item;
		for( i = 0; i < pList->GetCount(); i++ )
			if( pList->GetItemData(i) == p->auctionid )
			{
				pList->DeleteString(i);
				GetGameUIMan()->MessageBox("",GetStringFromTable(668), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
				break;
			}
		pList = GetGameUIMan()->m_pDlgAuctionList->m_pLst_Item;
		for( i = 0; i < pList->GetCount(); i++ )
			if( pList->GetItemData(i) == p->auctionid )
			{
				pList->DeleteString(i);
				GetGameUIMan()->MessageBox("",GetStringFromTable(668), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
				break;
			}
	}
}