// Filename	: DlgAuctionSellList.cpp
// Creator	: Xiao Zhou
// Date		: 2005/11/10

#include "AFI.h"
#include "DlgAuctionBuyList.h"
#include "DlgAuctionList.h"
#include "DlgAuctionSellList.h"
#include "EC_GameUIMan.h"
#include "EC_HostPlayer.h"
#include "EC_GameSession.h"
#include "EC_Inventory.h"
#include "EC_IvtrItem.h"
#include "EC_IvtrEquip.h"
#include "auctionopen_re.hpp"
#include "auctionclose_re.hpp"
#include "EC_ShortcutMgr.h"

#define new A_DEBUG_NEW

#define MAX_AUCTION_PRICE 200000000

AUI_BEGIN_COMMAND_MAP(CDlgAuctionSellList, CDlgBase)

AUI_ON_COMMAND("IDCANCEL",			OnCommandCancel)
AUI_ON_COMMAND("auctionbuylist",	OnCommandAuctionBuyList)
AUI_ON_COMMAND("auctionlist",		OnCommandAuctionList)
AUI_ON_COMMAND("auctionselllist",	OnCommandAuctionSellList)
AUI_ON_COMMAND("auctionmyfavor",	OnCommandAuctionMyFavor)
AUI_ON_COMMAND("sell",				OnCommandSell)
AUI_ON_COMMAND("closeauction",		OnCommandCloseAuction)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgAuctionSellList, CDlgBase)

AUI_ON_EVENT("Img_Item",	WM_LBUTTONDOWN,	OnEventLButtonDown_Img_Item)

AUI_END_EVENT_MAP()

//------------------------------------------------------------------------
// Right-Click Shortcut for CDlgAuctionSellList
//------------------------------------------------------------------------
typedef CECShortcutMgr::SimpleClickShortcut<CDlgAuctionSellList> AuctionSellListClickShortcut;
//------------------------------------------------------------------------

CDlgAuctionSellList::CDlgAuctionSellList()
{
	m_pTxt_PackGold = NULL;
	m_pTxt_Poundage = NULL;
	m_pImg_Item = NULL;
	m_pTxt_Name = NULL;
	m_pTxt_SellPrice = NULL;
	m_pTxt_BinPrice = NULL;
	m_pRdo_8h = NULL;
	m_pRdo_16h = NULL;
	m_pRdo_24h = NULL;
	m_pChk_SavePrice = NULL;
	m_idItem = 0;
	m_nItemPos = -1;
	m_nSellID = 0;
	m_pItem = NULL;
}

CDlgAuctionSellList::~CDlgAuctionSellList()
{
}

bool CDlgAuctionSellList::OnInitDialog()
{
	GetGameUIMan()->GetShortcutMgr()->RegisterShortCut(new AuctionSellListClickShortcut(this));

	DDX_Control("Txt_PackGold", m_pTxt_PackGold);
	DDX_Control("Txt_Name", m_pTxt_Name);
	DDX_Control("Txt_SellPrice", m_pTxt_SellPrice);
	DDX_Control("Txt_BinPrice", m_pTxt_BinPrice);
	DDX_Control("Txt_Poundage", m_pTxt_Poundage);
	DDX_Control("Img_Item", m_pImg_Item);
	DDX_Control("Rdo_8h", m_pRdo_8h);
	DDX_Control("Rdo_16h", m_pRdo_16h);
	DDX_Control("Rdo_24h", m_pRdo_24h);
	DDX_Control("Chk_SavePrice", m_pChk_SavePrice);
	m_pTxt_PackGold->SetIsNumberOnly(true);
	m_pTxt_SellPrice->SetIsNumberOnly(true);
	m_pTxt_BinPrice->SetIsNumberOnly(true);
	m_pRdo_8h->Check(true);
	m_pRdo_16h->Check(false);
	m_pRdo_24h->Check(false);

	return CDlgAuctionBase::OnInitDialog();
}

void CDlgAuctionSellList::OnShowDialog()
{
	CDlgAuctionBase::OnShowDialog();

	m_pBtn_AuctionSellList->SetPushed(true);
	m_pTxt_Poundage->SetText(_AL("0"));
}

void CDlgAuctionSellList::OnTick()
{
	CDlgAuctionBase::OnTick();

	if( IsShow() )
	{
		if( m_nItemPos != -1 )
		{
			CECInventory *pPack = GetHostPlayer()->GetPack();
			CECIvtrItem *pItem = pPack->GetItem(m_nItemPos);
			if( !pItem || m_pItem != pItem )
			{
				m_pImg_Item->SetCover(NULL, -1);
				m_pImg_Item->SetText(_AL(""));
				m_pTxt_Name->SetText(_AL(""));
				if( !m_pChk_SavePrice->IsChecked() )
				{
					m_pTxt_SellPrice->SetText(_AL(""));
					m_pTxt_BinPrice->SetText(_AL(""));
				}
				m_pTxt_Poundage->SetText(_AL("0"));
				m_idItem = 0;
				m_nItemPos = -1;
				m_pItem = NULL;
			}
		}

		ACString strGold;
		if( m_pItem )
		{
			INT64 nTotalPrice = (INT64)m_pItem->GetUnitPrice() * m_pItem->GetCount();
			if( nTotalPrice > MAX_AUCTION_PRICE )
				nTotalPrice = MAX_AUCTION_PRICE;
			if( m_pRdo_8h->IsChecked() )
				nTotalPrice /= 10;
			else if( m_pRdo_16h->IsChecked() )
				nTotalPrice = nTotalPrice * 15 / 100;
			else
				nTotalPrice /= 5;
			if( nTotalPrice < 500 )
				strGold = GetGameUIMan()->GetFormatNumber(500);
			else
				strGold = GetGameUIMan()->GetFormatNumber(nTotalPrice);
			m_pTxt_Poundage->SetText(strGold);
		}
		else
			m_pTxt_Poundage->SetText(_AL(""));

		m_pTxt_PackGold->SetColor(GetGameUIMan()->GetPriceColor(GetHostPlayer()->GetMoneyAmount()));
		m_pTxt_PackGold->SetText(GetGameUIMan()->GetFormatNumber(GetHostPlayer()->GetMoneyAmount()));

		int nBasePrice = a_atoi(m_pTxt_SellPrice->GetText());
	}
}

void CDlgAuctionSellList::OnCommandCancel(const char* szCommand)
{
	OnEventLButtonDown_Img_Item(0, 0, NULL);
	CDlgAuctionBase::OnCommandCancel("");
}

void CDlgAuctionSellList::OnCommandAuctionBuyList(const char* szCommand)
{
	OnEventLButtonDown_Img_Item(0, 0, NULL);
	CDlgAuctionBase::OnCommandAuctionBuyList("");
}

void CDlgAuctionSellList::OnCommandAuctionList(const char* szCommand)
{
	OnEventLButtonDown_Img_Item(0, 0, NULL);
	CDlgAuctionBase::OnCommandAuctionList("");
}

void CDlgAuctionSellList::OnCommandAuctionSellList(const char* szCommand)
{
	OnEventLButtonDown_Img_Item(0, 0, NULL);
	CDlgAuctionBase::OnCommandAuctionList("");
}

void CDlgAuctionSellList::OnCommandAuctionMyFavor(const char* szCommand)
{
	OnEventLButtonDown_Img_Item(0, 0, NULL);
	CDlgAuctionBase::OnCommandAuctionMyFavor("");
}

void CDlgAuctionSellList::OnCommandSell(const char* szCommand)
{
	if( m_idItem != 0 && m_pItem )
	{
		m_nSellID = GetGameUIMan()->m_pDlgAuctionBuyList->m_mapAuctionID[m_idItem];
		int nNumber = m_pItem->GetCount();
		int nBasePrice = a_atoi(m_pTxt_SellPrice->GetText());
		int nBinPrice = a_atoi(m_pTxt_BinPrice->GetText());
		int nPoundage = a_atoi(m_pTxt_Poundage->GetText());
		if (nNumber > 65535)
		{
			GetGameUIMan()->MessageBox("", GetGameUIMan()->GetStringFromTable(658), 
				MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
			return;
		}
		if( nBasePrice <= 0 || nBasePrice > nBinPrice && nBinPrice != 0 )
		{
			GetGameUIMan()->MessageBox("", GetGameUIMan()->GetStringFromTable(659), 
				MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
			return;
		}

		if( nBasePrice > MAX_AUCTION_PRICE || nBinPrice > MAX_AUCTION_PRICE )
		{
			GetGameUIMan()->MessageBox("", GetGameUIMan()->GetStringFromTable(661), 
				MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
			return;
		}

		int nTime = 0;
		if( m_pRdo_8h->IsChecked() )
			nTime = 8 * 3600;
		else if( m_pRdo_16h->IsChecked() )
			nTime = 16 * 3600;
		else
			nTime = 24 * 3600;
		if( nPoundage > GetHostPlayer()->GetMoneyAmount() )
		{
			GetGameUIMan()->MessageBox("", GetGameUIMan()->GetStringFromTable(660), 
				MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
			return;
		}
		
		GetGameSession()->auction_Open(m_nSellID, m_idItem, m_nItemPos, nNumber,
			nBasePrice, nBinPrice, nTime);
	}
}

void CDlgAuctionSellList::OnCommandCloseAuction(const char* szCommand)
{
	int nSel = m_pLst_Item->GetCurSel();
	if( nSel >= 0 && nSel < m_pLst_Item->GetCount() )
	{
		int nRestTime = m_pLst_Item->GetItemData(nSel, 5) - GetGame()->GetServerGMTTime();
		if( nRestTime  > 7200)
		{
			PAUIDIALOG pMsgBox;
			GetGameUIMan()->MessageBox("Game_AuctionClose", GetStringFromTable(677), MB_YESNO, 
				A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
			pMsgBox->SetData(m_pLst_Item->GetItemData(nSel));
		}
		else
			GetGameUIMan()->MessageBox("", GetGameUIMan()->GetStringFromTable(7602), 
				MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
	}
}

void CDlgAuctionSellList::OnEventLButtonDown_Img_Item(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	if( m_idItem == 0 )
		return;

	if( m_nItemPos != -1 )
	{
		CECInventory *pPack = GetHostPlayer()->GetPack();
		CECIvtrItem *pItem = pPack->GetItem(m_nItemPos);
		if( pItem )
			pItem->Freeze(false);
	}
	m_pImg_Item->SetCover(NULL, -1);
	m_pImg_Item->SetText(_AL(""));
	m_pTxt_Name->SetText(_AL(""));
	if( !m_pChk_SavePrice->IsChecked() )
	{
		m_pTxt_SellPrice->SetText(_AL(""));
		m_pTxt_BinPrice->SetText(_AL(""));
	}
	m_pTxt_Poundage->SetText(_AL("0"));
	m_idItem = 0;
	m_nItemPos = -1;
	m_pItem = NULL;
}

void CDlgAuctionSellList::SetSellItem(CECIvtrItem *pItem, int nItemPos)
{
	AuctionIDMap::iterator it = GetGameUIMan()->m_pDlgAuctionBuyList->m_mapAuctionID.find(pItem->GetTemplateID());
	if( it == GetGameUIMan()->m_pDlgAuctionBuyList->m_mapAuctionID.end() )
	{
		GetGameUIMan()->MessageBox("", GetGameUIMan()->GetStringFromTable(657), 
			MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}
	if( pItem->GetExpireDate() != 0 )
	{
		GetGameUIMan()->MessageBox("", GetGameUIMan()->GetStringFromTable(826), 
			MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		return;
	}
	if( m_idItem != 0 )
		OnEventLButtonDown_Img_Item(0, 0 , NULL);
	pItem->Freeze(true);
	ACHAR szText[20];
	AString strFile;
	af_GetFileTitle(pItem->GetIconFile(), strFile);
	strFile.MakeLower();
	m_pImg_Item->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
		GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][strFile]);

	m_pImg_Item->SetColor(
		(pItem->IsEquipment() && ((CECIvtrEquip *)pItem)->IsDestroying())
		? A3DCOLORRGB(128, 128, 128)
		: A3DCOLORRGB(255, 255, 255));

	a_sprintf(szText, _AL("%d"), pItem->GetCount());
	m_pImg_Item->SetText(szText);
	m_pTxt_Name->SetText(pItem->GetName());
	INT64 nTotalPrice = (INT64)pItem->GetUnitPrice() * pItem->GetCount();
	if( nTotalPrice > MAX_AUCTION_PRICE )
		nTotalPrice = MAX_AUCTION_PRICE;
	if( !m_pChk_SavePrice->IsChecked() )
	{
		a_sprintf(szText, _AL("%d"), nTotalPrice);
		m_pTxt_SellPrice->SetText(szText);
		m_pTxt_BinPrice->SetText(_AL(""));
	}
	m_idItem = pItem->GetTemplateID();
	m_pTxt_Poundage->SetText(szText);
	m_nItemPos = nItemPos;
	m_pItem = pItem;
}

void CDlgAuctionSellList::CloseRe(void *pData)
{
	AuctionClose_Re *p = (AuctionClose_Re *)pData;
	if( p->retcode == ERR_SUCCESS )
	{
//		GetGameUIMan()->MessageBox("", GetGameUIMan()->GetStringFromTable(674), 
//			MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
		GetGameSession()->auction_AttendList();
	}
//	else if( p->retcode == ERR_AS_BID_UNREDEEMABLE )
//		GetGameUIMan()->MessageBox("", GetGameUIMan()->GetStringFromTable(675), 
//			MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
	else
		GetGameUIMan()->MessageBox("", GetGameUIMan()->GetStringFromTable(676), 
			MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
}

void CDlgAuctionSellList::SellRe(void *pData)
{
	AuctionOpen_Re *p = (AuctionOpen_Re *)pData;
	if( p->retcode == ERR_SUCCESS )
	{
		m_pImg_Item->SetCover(NULL, -1);
		m_pImg_Item->SetText(_AL(""));
		m_pTxt_Name->SetText(_AL(""));
		if( !m_pChk_SavePrice->IsChecked() )
		{
			m_pTxt_SellPrice->SetText(_AL(""));
			m_pTxt_BinPrice->SetText(_AL(""));
		}
		m_idItem = 0;
		m_nItemPos = -1;
		m_pItem = NULL;
		GetGameSession()->auction_AttendList();
	}
	else if( p->retcode == ERR_AS_ID_EXHAUSE )
		GetGameUIMan()->MessageBox("",GetStringFromTable(670), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
	else if( p->retcode == ERR_AS_MARKET_UNOPEN )
		GetGameUIMan()->MessageBox("",GetStringFromTable(672), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
	else if( p->retcode == ERR_AS_ATTEND_OVF )
		GetGameUIMan()->MessageBox("",GetStringFromTable(669), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
	else if( p->retcode == ERR_AS_MAILBOXFULL )
		GetGameUIMan()->MessageBox("",GetStringFromTable(727), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
}

void CDlgAuctionSellList::OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver)
{
	// no need to check name "Img_Item", only one available drag-drop target
	if( !pIvtrSrc->IsTradeable() )
	{
		GetGameUIMan()->MessageBox("", GetStringFromTable(652), MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
	}
	else if (pIvtrSrc->IsEquipment() && ((CECIvtrEquip *)pIvtrSrc)->IsDestroying())
	{
		GetGameUIMan()->AddChatMessage(GetStringFromTable(7922), GP_CHAT_MISC);
	}
	else
	{
		this->SetSellItem( pIvtrSrc, iSrc );
	}
}