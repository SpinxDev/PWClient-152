// Filename	: DlgBShop.cpp
// Creator	: Feng Ning
// Date		: 2010/05/12

#include "DlgBShop.h"
#include "DlgChat.h"
#include "AFI.h"
#include "AIniFile.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_GameUIMan.h"
#include "EC_IvtrItem.h"
#include "EC_HostPlayer.h"
#include "EC_GameSession.h"
#include "AUILabel.h"
#include "AUIListBox.h"
#include "AUIImagePicture.h"
#include "AUICTranslate.h"
#include "sysauctionaccount_re.hpp"
#include "sysauctionbid_re.hpp"
#include "sysauctioncashtransfer_re.hpp"
#include "sysauctiongetitem_re.hpp"
#include "sysauctionlist_re.hpp"
#include "gsysauctionitem"
#include "gnoctets.h"

#include <A2DSprite.h>

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgBShop, CDlgBase)
AUI_ON_COMMAND("payment", OnCommand_Payment)
AUI_ON_COMMAND("biditem", OnCommand_BidItem)
AUI_ON_COMMAND("returncash", OnCommand_ReturnCash)
AUI_ON_COMMAND("startauto", OnCommand_StartAuto)
AUI_ON_COMMAND("cancelauto", OnCommand_CancelAuto)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgBShop, CDlgBase)
AUI_END_EVENT_MAP()

#define BSHOP_MSGBOX(str) GetGameUIMan()->MessageBox("", str, MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
#define SYSAUCTION_MAX_CASH 10000000

CDlgBShop::AuctionItemInfo::~AuctionItemInfo()
{
	if(pNetData)
	{
		delete pNetData;
		pNetData = NULL;
	}

	if(pItem)
	{
		delete pItem;
		pItem = NULL;
	}

	A3DRELEASE(pCover);
}

CDlgBShop::CDlgBShop()
:m_State(STATE_WAIT)
,m_StateLast(STATE_WAIT)

,m_UpdateNow(true)
,m_Dirty(true)
,m_AutoBidItem(0)
,m_LastUpdateTime(0)
,m_LastLockTime(0)

,m_pBtn_BidItem(NULL)
,m_pBtn_StartAuto(NULL)
,m_pBtn_CancelAuto(NULL)
,m_pBtn_ReturnCash(NULL)
,m_pEdt_BidPrice(NULL)
,m_pEdt_MaxPrice(NULL)
,m_pEdt_DeltaPrice(NULL)
,m_pList_Items(NULL)
,m_pTxt_Details(NULL)

,m_pShopItems(NULL)
,m_pCurrent(NULL)
{

}

bool CDlgBShop::OnInitDialog()
{
	DDX_Control("Btn_BidItem", m_pBtn_BidItem);
	DDX_Control("Btn_StartAuto", m_pBtn_StartAuto);
	DDX_Control("Btn_CancelAuto", m_pBtn_CancelAuto);
	DDX_Control("Btn_ReturnCash", m_pBtn_ReturnCash);
	DDX_Control("Txt_BidPrice", m_pEdt_BidPrice);
	DDX_Control("Txt_MaxPrice", m_pEdt_MaxPrice);
	DDX_Control("Txt_DeltaPrice", m_pEdt_DeltaPrice);
	DDX_Control("List_AuctionItems", m_pList_Items);
	DDX_Control("Txt_Details", m_pTxt_Details);

	SetAuto(false);

	m_pShopItems = new AuctionItemInfoMap;

	return CDlgBase::OnInitDialog();
}

void CDlgBShop::OnShowDialog()
{
	m_UpdateNow = true;
	CDlgBase::OnShowDialog();
}

bool CDlgBShop::Release()
{
	// delete the stored item info
	SwitchItemList(NULL);

	SetSelectedItem(NULL);

	return CDlgBase::Release();
}

bool CDlgBShop::IsBiddable()
{
	if(!GetSelectedItem())
	{
		return false;
	}

	if(STATE_WAIT != GetBidState())
	{
		return false;
	}

	if( m_Account.IsIn(GetSelectedItem()->pNetData->sa_id) >= 0)
	{
		return false;
	}

	return ASTATE_START == GetSelectedItem()->pNetData->state;
}

CDlgBShop::AuctionItemInfo* CDlgBShop::GetListSelectedItem()
{
	AuctionItemInfo* p = NULL;
	
	if(m_pShopItems && m_pList_Items->GetCurSel() >= 0)
	{
		unsigned int sa_id = m_pList_Items->GetItemData(m_pList_Items->GetCurSel());
		AuctionItemInfoMap::iterator itr = m_pShopItems->find(sa_id);
		if(itr != m_pShopItems->end())
		{
			p = itr->second;
		}
	}
	
	return p;
}

void CDlgBShop::OnTick()
{
	// NOTICE: do this dirty hack here because the server may not give us any feedback
	if(m_LastLockTime != 0 && GetGame()->GetServerGMTTime() - m_LastLockTime > 10)
	{
		SetBidState(STATE_WAIT);
	}

	// update account information and itemlist when necessary
	if( m_UpdateNow || 
		( IsShow() && STATE_WAIT == GetBidState() && 
		  m_LastUpdateTime != GetGame()->GetServerGMTTime() 
		  && GetGame()->GetServerGMTTime() % 10 == 0 ) )
	{
		if(m_UpdateNow) GetGameSession()->sysAuction_Account();
		m_UpdateNow = false;
		m_LastUpdateTime = GetGame()->GetServerGMTTime();
		GetGameSession()->sysAuction_List();
	}

	// update GUI
	if(IsDirty())
	{
		SetDirty(false);
		RefreshContent();
	}

	CheckDelayMessage();

	// sync this after RefreshContent()
	AuctionItemInfo* p = GetListSelectedItem();
	if(!IsAuto()) SetSelectedItem(p);
	ShowAuctionItem(p ? p->pNetData : NULL);
	ShowItemDesc(p);
	
	// player cash must be updated here
	ShowCash(m_Account.Cash, GetHostPlayer()->GetCash());
	
	// make sure the button is available
	m_pBtn_ReturnCash->Enable(!IsAuto() && STATE_WAIT == GetBidState() && m_Account.Cash > 0);

	// switch two button
	m_pBtn_StartAuto->Show(!IsAuto());
	m_pBtn_CancelAuto->Show(IsAuto());

	// seal the controls
	m_pBtn_BidItem->Enable(IsBiddable() && !IsAuto());
	m_pEdt_BidPrice->Enable(IsBiddable() && !IsAuto());
	m_pEdt_MaxPrice->Enable(!IsAuto());
	m_pEdt_DeltaPrice->Enable(!IsAuto());
	
	// do bid here because we may transfer cash first
	if( STATE_BIDDING == GetBidState() )
	{
		DoBidding();
	}
	else if( STATE_WAIT == GetBidState() )
	{
		DoAutoBidding();
	}
}

void CDlgBShop::DoBidding()
{
	ACString szNum = m_pEdt_BidPrice->GetText();
	int price = szNum.ToInt() * 100;
	
	if(!GetSelectedItem() || price <= 0 ||
		((unsigned int)price > m_Account.Cash + GetHostPlayer()->GetCash()))
	{
		// invalid input
		SetBidState(STATE_WAIT);
		return;
	}
	
	// logically client allowed to bid a item whose bidder is current player.
	// forbidden this situation in IsBiddable() method.
	int cash = m_Account.Cash;
	if(GetSelectedItem()->pNetData->bidder_userid == GetGameSession()->GetUserID())
	{
		cash += GetSelectedItem()->pNetData->bid_price;
	}
	
	// transfer cash first if cash is not enough
	if(cash >= price)
	{
		GetGameSession()->sysAuction_Bid(GetSelectedItem()->pNetData->sa_id, price);
		SetBidState(STATE_BIDDING_WAIT);
	}
	else if(!GetHostPlayer()->CanTransferCash())
	{
		SetBidState(STATE_WAIT);
	}
	else if(GetHostPlayer()->GetCash() + cash >= price)
	{
		GetGameSession()->sysAuction_CashTransfer(false, price - cash);
		SetBidState(STATE_AUTOTRANSFERING);
	}
}

void CDlgBShop::DoAutoBidding()
{
	if(!IsAuto() || !m_pShopItems)
	{
		return;
	}

	if( m_LastUpdateTime == GetGame()->GetServerGMTTime() || GetGame()->GetServerGMTTime() % 3 != 0 )
	{
		return;
	}
	m_LastUpdateTime = GetGame()->GetServerGMTTime();

	// get the auto bid item
	AuctionItemInfoMap::iterator itr = m_pShopItems->find(m_AutoBidItem);
	if(itr == m_pShopItems->end())
	{
		SetAuto(0);
		return;
	}

	AuctionItemInfo* pInfo = itr->second;

	if(m_Account.IsIn(pInfo->pNetData->sa_id) >= 0)
	{
		// no need to do auto bidding
		return;
	}

	if(pInfo->pNetData->state != ASTATE_START)
	{
		// stop auto bidding when bid over
		SetAuto(0);
		return;
	}
	
	int maxPrice = pInfo->MaxPrice;
	int deltaPrice = pInfo->DeltaPrice;

	if(maxPrice <= 0 || deltaPrice <= 0 || deltaPrice > maxPrice)
	{
		// invalid input
		SetAuto(0);
		return;
	}

	if((unsigned int)maxPrice > m_Account.Cash + GetHostPlayer()->GetCash())
	{
		// cash not enough
		SetAuto(0);
		return;
	}

	unsigned int price = pInfo->pNetData->bid_price;
	if(price < pInfo->pNetData->base_price)
	{
		price = pInfo->pNetData->base_price;
	}
	else
	{
		price += deltaPrice;
	}
	
	// format to gold only
	if(price % 100 != 0)
	{
		price = ((unsigned int)(price / 100) + 1) * 100;
	}

	if(price > (unsigned int)maxPrice)
	{
		// over limitation
		SetAuto(0);
		return;
	}

	// set current price
	ACString szNum;
	m_pEdt_BidPrice->SetText(szNum.Format(_AL("%d"), price / 100));
	SetSelectedItem(pInfo);
	SetBidState(STATE_BIDDING);
}

void CDlgBShop::SwitchItemList(AuctionItemInfoMap* pItems)
{
	// delete the stored item info
	if(m_pShopItems)
	{
		for(AuctionItemInfoMap::iterator itr = m_pShopItems->begin();itr!=m_pShopItems->end();++itr)
		{
			delete itr->second;
			itr->second = NULL;
		}
		
		delete m_pShopItems;
		m_pShopItems = NULL;

		// clear the iconlist too
		m_vecIconList.clear();
	}
	
	m_pShopItems = pItems;
}

void CDlgBShop::RefreshContent()
{
	ShowCash(m_Account.Cash, GetHostPlayer()->GetCash());
	ShowItems(m_pShopItems);
	AuctionItemInfo* p = GetListSelectedItem();
	ShowAuctionItem(p ? p->pNetData : NULL);
	ShowItemDesc(p);
}

CDlgBShop::STATE CDlgBShop::SetBidState(STATE state)
{
	if(m_State != state)
	{
		if(STATE_WAIT == m_State)
		{
			m_LastLockTime =  GetGame()->GetServerGMTTime();
		}
		else if(STATE_WAIT == state)
		{
			m_LastLockTime = 0;
		}

		m_StateLast = m_State;
		m_State = state;
	}

	return m_State;
}

void CDlgBShop::OnSysAuctionAction(int idAction, void *pData)
{
	// TODO: need to handle all error code

	switch(idAction)
	{
	case PROTOCOL_SYSAUCTIONBID_RE:
		OnSysAuctionBid(pData);
		break;
	case PROTOCOL_SYSAUCTIONACCOUNT_RE:
		OnSysAuctionAccount(pData);
		break;
	case PROTOCOL_SYSAUCTIONCASHTRANSFER_RE:
		OnSysAuctionCashTransfer(pData);
		break;
	case PROTOCOL_SYSAUCTIONGETITEM_RE:
		OnSysAuctionGetItem(pData);
		break;
	case PROTOCOL_SYSAUCTIONLIST_RE:
		OnSysAuctionList(pData);
		break;

	default:
		// unexpected protocol
		break;
	};
}

CDlgBShop::AuctionItemInfo* CDlgBShop::UpdateAuctionItem(const GNET::GSysAuctionItem* pItem)
{
	AuctionItemInfo* pNewItem = NULL;

	AuctionItemInfoMap::iterator itr = m_pShopItems->find(pItem->sa_id);
	if(itr != m_pShopItems->end())
	{
		pNewItem = itr->second;
		GSysAuctionItem* pOldItem = pNewItem->pNetData;
		if(pOldItem) delete pOldItem;
		pNewItem->pNetData = (GNET::GSysAuctionItem*)pItem->Clone();
	}
	else
	{
		pNewItem = new AuctionItemInfo();
		pNewItem->pNetData = (GNET::GSysAuctionItem *)pItem->Clone();
		AuctionItemInfoMap::value_type v(pItem->sa_id, pNewItem);
		m_pShopItems->insert(v);

		// this item is new to current list
		unsigned int newID = pItem->sa_id;
		GetGameSession()->sysAuction_GetItem(&newID, 1);
	}

	return pNewItem;
}

void CDlgBShop::OnSysAuctionBid(void* pData)
{
	SysAuctionBid_Re* p = (SysAuctionBid_Re*)pData;

	if(ERR_SA_BID_FAILED == p->retcode)
	{
		m_Account.Cash = p->cash;
		m_Account.Remove(p->info.sa_id);
		UpdateAuctionItem(&p->info);
		GetGame()->GetGameRun()->AddChatMessage(GetStringFromTable(5617), GP_CHAT_MISC);
	}
	else 
	{
		if(ERR_SUCCESS == p->retcode)
		{
			m_Account.Cash = p->cash;
			m_Account.Add(p->info.sa_id);
			UpdateAuctionItem(&p->info);
		}

		SetBidState(STATE_WAIT);
		SetDirty(true);
	}
}

void CDlgBShop::OnSysAuctionAccount(void* pData)
{
	SysAuctionAccount_Re* p = (SysAuctionAccount_Re*)pData;
	if(ERR_SUCCESS == p->retcode)
	{
		m_Account.Cash = p->cash;
		if (p->bid_ids.empty())
			m_Account.Items.swap(abase::vector<unsigned int>());
		else
			m_Account.Items.swap(abase::vector<unsigned int>(&p->bid_ids[0], &p->bid_ids[0] + p->bid_ids.size()));
		SetDirty(true);
	}
}

void CDlgBShop::OnSysAuctionCashTransfer(void* pData)
{
	SysAuctionCashTransfer_Re* p = (SysAuctionCashTransfer_Re*)pData;
	if(ERR_SUCCESS == p->retcode)
	{
		m_Account.Cash = p->cash;
		SetBidState((STATE_AUTOTRANSFERING == GetBidState()) ? STATE_BIDDING:STATE_WAIT);
	}
	else
	{
		SetBidState(STATE_WAIT);
	}

	SetDirty(true);
}

void CDlgBShop::OnSysAuctionGetItem(void* pData)
{
	SysAuctionGetItem_Re* p = (SysAuctionGetItem_Re*)pData;

	if(!m_pShopItems || p->ids.empty())
	{
		return;
	}

	ASSERT(p->ids.size() == p->items.size());

	// copy the detailed information
	std::vector<unsigned int>& ids = p->ids;
	for(size_t i=0; i<ids.size(); ++i)
	{
		GRoleInventory& pIvtr = p->items[i];
		AuctionItemInfoMap::iterator itr = m_pShopItems->find(ids[i]);
		if(itr == m_pShopItems->end())
		{
			continue;
		}

		CECIvtrItem *pItem = CECIvtrItem::CreateItem(pIvtr.id, pIvtr.expire_date, pIvtr.count);
		pItem->SetItemInfo((unsigned char*)pIvtr.data.begin(), pIvtr.data.size());
		pItem->SetProcType(pIvtr.proctype);
		itr->second->pItem = pItem;
	}

	SetDirty(true);
}

void CDlgBShop::OnSysAuctionList(void* pData)
{
	SysAuctionList_Re* p = (SysAuctionList_Re*)pData;
	typedef std::vector<GSysAuctionItem> Items;
	Items& items = p->items;

	abase::vector<unsigned int> ids;

	// trying to refresh item list
	AuctionItemInfoMap* pNewItems = new AuctionItemInfoMap();
	AuctionItemInfo* pCurrent = NULL;
	for(Items::iterator i=items.begin();i!=items.end();++i)
	{
		GSysAuctionItem& item = *i;

		AuctionItemInfoMap::iterator itr = m_pShopItems->find(item.sa_id);
		if(itr != m_pShopItems->end())
		{
			AuctionItemInfo* pInfo = itr->second;
			if(pInfo->pNetData) delete pInfo->pNetData;
			pInfo->pNetData = (GNET::GSysAuctionItem *)item.Clone();
			AuctionItemInfoMap::value_type v(item.sa_id, pInfo);
			pNewItems->insert(v);

			// record new current info
			if(GetSelectedItem() && item.sa_id == GetSelectedItem()->pNetData->sa_id)
			{
				pCurrent = pInfo;
			}

			m_pShopItems->erase(itr);
		}
		else
		{
			AuctionItemInfo* pInfo = new AuctionItemInfo();
			pInfo->pNetData = (GNET::GSysAuctionItem *)item.Clone();
			AuctionItemInfoMap::value_type v(item.sa_id, pInfo);
			pNewItems->insert(v);
			
			// new sa_id, need to refresh
			ids.push_back(item.sa_id);
		}
	}

	// update current item info
	SetSelectedItem(pCurrent);
	
	// select the first one by default
	if(!GetSelectedItem() && !pNewItems->empty())
	{
		SetSelectedItem(pNewItems->begin()->second);
	}
	
	// switch to new item list
	SwitchItemList(pNewItems);

	if(!ids.empty())
	{
		GetGameSession()->sysAuction_GetItem(ids.begin(), ids.size());
	}

	SetDirty(true);
}

void CDlgBShop::ShowAuctionItem(const GNET::GSysAuctionItem* pItem)
{
	PAUILABEL pTxt_CurrentPrice = 
		dynamic_cast<PAUILABEL>(GetDlgItem("Txt_CurrentPrice"));

	PAUILABEL pLab_CurrentPrice = 
		dynamic_cast<PAUILABEL>(GetDlgItem("Lab_CurrentPrice"));

	PAUILABEL pTxt_BasePrice = 
		dynamic_cast<PAUILABEL>(GetDlgItem("Txt_Price"));
	
	PAUILABEL pTxt_BeginTime = 
		dynamic_cast<PAUILABEL>(GetDlgItem("Txt_BeginTime"));
	
	PAUILABEL pTxt_LeftTime = 
		dynamic_cast<PAUILABEL>(GetDlgItem("Txt_LeftTime"));

	PAUILABEL pTxt_FreezeTime = 
		dynamic_cast<PAUILABEL>(GetDlgItem("Txt_FreezeTime"));

	PAUILABEL pTxt_Pname = 
		dynamic_cast<PAUILABEL>(GetDlgItem("Txt_Pname"));

	// change the title
	pLab_CurrentPrice->SetText(GetStringFromTable((!pItem || ASTATE_END != pItem->state) ? 5623 : 5624));

	if(!pItem)
	{
		// clear the UI
		pTxt_CurrentPrice->SetText(_AL(""));
		pTxt_BasePrice->SetText(_AL(""));
		pTxt_FreezeTime->SetText(_AL(""));
		pTxt_BeginTime->SetText(_AL(""));
		pTxt_LeftTime->SetText(_AL(""));
		pTxt_Pname->SetText(_AL(""));
	}
	else
	{
		pTxt_BasePrice->SetText(GetCashText(pItem->base_price, false));

		if(pItem->bid_price < pItem->base_price)
		{
			// still nobody bid
			pTxt_CurrentPrice->SetText(GetStringFromTable(ASTATE_END == pItem->state ? 5621:5620));
		}
		else
		{
			// bidding
			pTxt_CurrentPrice->SetText(GetCashText(pItem->bid_price, false));
		}
		
		// show bid start time if necessary
		ACString strText;
		long stime = pItem->auction_starttime;
		stime -= GetGame()->GetTimeZoneBias() * 60; // localtime = UTC - bias, in which bias is in minutes
		const tm* pt = gmtime(&stime);
		if(!pt || pItem->auction_starttime == 0)
		{
			strText = GetStringFromTable(5600);
		}
		else
		{
			strText.Format(	GetStringFromTable(8010),
							pt->tm_year+1900,
							pt->tm_mon+1,
							pt->tm_mday,
							pt->tm_hour,
							pt->tm_min );
		}
		pTxt_BeginTime->SetText(strText);

		// show the bid freeze time
		int freezeTime = 0;
		if(pItem->state == ASTATE_START)
		{
			freezeTime = pItem->bid_freezetime - GetGame()->GetServerGMTTime();
		}
		pTxt_FreezeTime->SetText( freezeTime <= 0 ? 
			GetStringFromTable(ASTATE_END == pItem->state ? 5601:5600) : GetGameUIMan()->GetFormatTime(freezeTime));

		// show the bid left time
		int leftTime = 0;
		if(pItem->state == ASTATE_START)
		{
			leftTime = pItem->auction_endtime - GetGame()->GetServerGMTTime();
		}
		pTxt_LeftTime->SetText( leftTime <= 0 ? 
			GetStringFromTable(ASTATE_END == pItem->state ? 5601:5600) : GetGameUIMan()->GetFormatTime(leftTime));

		// check bidder name
		int bidder = pItem->bidder_roleid;
		const ACHAR* szName = bidder > 0 ? GetGame()->GetGameRun()->GetPlayerName(bidder, false) : NULL;
		if(!szName && pItem->bidder_roleid > 0)
		{
			GetGameSession()->GetPlayerBriefInfo(1, &bidder, 2);
		}

		pTxt_Pname->SetText(szName ? szName:GetStringFromTable(8720));
	}
}

bool CDlgBShop::LoadItemIcon(AuctionItemInfo* pInfo)
{
	if(!pInfo->pItem)
	{
		return false;
	}

	if(pInfo->pCover)
	{
		return true;
	}
	
	AString szFile;
	af_GetFileTitle(pInfo->pItem->GetIconFile(), szFile);

	AString szBigIconFile;
	szBigIconFile.Format("surfaces\\¾ºÅÄÆ·\\%s", szFile);
	if(!af_IsFileExist(szBigIconFile))
	{
		return false;
	}
	
	szBigIconFile.Format("¾ºÅÄÆ·\\%s", szFile);
	pInfo->pCover = new A2DSprite();
	bool bval = pInfo->pCover->Init(m_pA3DDevice, szBigIconFile, 0);
	if(!bval)
	{
		AUI_ReportError(__LINE__, __FILE__);
		A3DRELEASE(pInfo->pCover);
		return false;
	}
	
	pInfo->pCover->SetLinearFilter(true);
	return true;
}

void CDlgBShop::ShowItemDesc(AuctionItemInfo* pInfo)
{
	CECIvtrItem *pItem = pInfo ? pInfo->pItem : NULL;

	PAUILABEL pTxt_ItemName = 
		dynamic_cast<PAUILABEL>(GetDlgItem("Txt_ItemName"));

	PAUIIMAGEPICTURE pImg_Item = 
		dynamic_cast<PAUIIMAGEPICTURE>(GetDlgItem("Img_Item"));

	if(!pItem)
	{
		pTxt_ItemName->SetText(_AL(""));
		pImg_Item->SetCover(NULL, -1);
		pImg_Item->SetText(_AL(""));
	}
	else
	{
		pTxt_ItemName->SetText(pItem->GetName());

		// trying to load a new icon
		if(LoadItemIcon(pInfo))
		{
			pImg_Item->SetCover(pInfo->pCover, 0);
		}
		else
		{
			// set original icon
			AString szFile;
			af_GetFileTitle(pItem->GetIconFile(), szFile);

			szFile.MakeLower();
			pImg_Item->SetCover( GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
								 GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][szFile] );
		}

		AUICTranslate trans;
		pImg_Item->SetHint(trans.Translate(pItem->GetDesc()));
	}
}

void CDlgBShop::ShowItems(AuctionItemInfoMap* pItems)
{
	unsigned int old_sa_id = 0;
	if(m_pList_Items->GetCurSel() >= 0)
	{
		old_sa_id = m_pList_Items->GetItemData(m_pList_Items->GetCurSel());
	}

	m_pList_Items->ResetContent();
	m_pList_Items->SetImageList(NULL);
	m_vecIconList.clear();
	if(!pItems || pItems->empty())
	{
		m_pTxt_Details->SetText(_AL(""));
		m_pTxt_Details->ScrollToTop();
		return;
	}

	ACString strItem;
	AuctionItemInfoMap::iterator itr = m_pShopItems->begin();

	for(;itr != m_pShopItems->end();++itr)
	{
		AuctionItemInfo* pInfo = itr->second;
		if(pInfo->pItem)
		{
			strItem.Format(pInfo->pItem->GetName());
		}
		else
		{
			strItem.Format(_AL("%d"), pInfo->pNetData->item_id);
		}

		ACString str;

		if (pInfo->pItem)
		{
			bool bCover = LoadItemIcon(pInfo);

			if (bCover)
			{
				m_vecIconList.push_back(pInfo->pCover);	

				EditBoxItemBase item(enumEIImage);
				item.SetImageIndex(m_vecIconList.size() - 1);
				item.SetImageFrame(0);
				item.SetImageScale(0.15f * GetGameUIMan()->GetWindowScale());
				str = (ACHAR)AUICOMMON_ITEM_CODE_START + item.Serialize() + _AL(" ");
			}
			else
			{
				// set original icon
				AString szFile;
				af_GetFileTitle(pInfo->pItem->GetIconFile(), szFile);

				szFile.MakeLower();

				m_vecIconList.push_back(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY]);	

				//	pImg_Item->SetCover( GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
				//		GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][szFile] );

				EditBoxItemBase item(enumEIImage);
				item.SetImageIndex(m_vecIconList.size() - 1);
				item.SetImageFrame(GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][szFile]);
				item.SetImageScale(0.5f * GetGameUIMan()->GetWindowScale());
				str = (ACHAR)AUICOMMON_ITEM_CODE_START + item.Serialize() + _AL(" ");
			}
		}

		m_pList_Items->SetItemMask(-1);
		m_pList_Items->AddString(str + strItem);
		m_pList_Items->SetItemData(m_pList_Items->GetCount()-1, pInfo->pNetData->sa_id);

		if(old_sa_id && old_sa_id == itr->second->pNetData->sa_id)
		{
			m_pList_Items->SetCurSel(m_pList_Items->GetCount()-1);
		}
	}
	m_pList_Items->SetImageList(&m_vecIconList);

}

void CDlgBShop::OnCommand_BidItem(const char *szCommand)
{
	if(IsAuto() || GetBidState() != STATE_WAIT)
	{
		return;
	}

	ACString szNum = m_pEdt_BidPrice->GetText();
	if(szNum.ToInt() <= 0 || szNum.ToFloat() != (float)(szNum.ToInt()))
	{
		// invalid input
		BSHOP_MSGBOX(GetStringFromTable(5613));
		return;
	}

	unsigned int price = szNum.ToInt() * 100;

	if(price >= SYSAUCTION_MAX_CASH)
	{
		ACString szTxt;
		szTxt.Format(GetStringFromTable(5618), GetCashText(SYSAUCTION_MAX_CASH, false));
		BSHOP_MSGBOX(szTxt);
		return;
	}

	if(price > m_Account.Cash + GetHostPlayer()->GetCash())
	{
		// cash not enough
		BSHOP_MSGBOX(GetStringFromTable(5616));
		return;
	}
	else if(price > m_Account.Cash && !GetHostPlayer()->CanTransferCash())
	{
		// invalid state
		BSHOP_MSGBOX(GetStringFromTable(5622));
		return;
	}

	if(GetSelectedItem())
	{
		unsigned int bid_price = GetSelectedItem()->pNetData->bid_price;
		unsigned int base_price = GetSelectedItem()->pNetData->base_price;

		if( (bid_price >= base_price && price <= bid_price) || price < base_price )
		{
			// low price
			BSHOP_MSGBOX(GetStringFromTable(5612));
			return;
		}
	}

	ACString strText;
	if (m_strLastSelectedItem == _AL("") || m_strLastSelectedItem == GetSelectedItem()->pItem->GetName())
	{
		strText.Format(GetStringFromTable(5610),GetSelectedItem()->pItem->GetName());
	}
	else
	{
		strText.Format(GetStringFromTable(5609),m_strLastSelectedItem,GetSelectedItem()->pItem->GetName());
	}

	PAUIDIALOG pMsgBox;
	GetGameUIMan()->MessageBox("Game_BShop_Bid_Confirm", strText, MB_YESNO, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);

	SetBidState(STATE_CONFIRM);
}

void CDlgBShop::OnCommand_ReturnCash(const char *szCommand)
{
	if(GetBidState() != STATE_WAIT || m_Account.Cash <= 0)
	{
		return;
	}

	GetGameSession()->sysAuction_CashTransfer(true, m_Account.Cash);
	SetBidState(STATE_TRANSFERING);
}

void CDlgBShop::OnCommand_StartAuto(const char *szCommand)
{
	ACString szNum = m_pEdt_MaxPrice->GetText();
	int maxPrice = szNum.ToInt() * 100;
	
	if(szNum.ToInt() <= 0 || szNum.ToFloat() != (float)(szNum.ToInt()))
	{
		// invalid input
		BSHOP_MSGBOX(GetStringFromTable(5613));
		return;
	}

	szNum = m_pEdt_DeltaPrice->GetText();
	int deltaPrice = szNum.ToInt() * 100;

	if(szNum.ToInt() <= 0 || szNum.ToFloat() != (float)(szNum.ToInt()))
	{
		// invalid input
		BSHOP_MSGBOX(GetStringFromTable(5613));
		return;
	}
	
	if(maxPrice <= 0 || deltaPrice <= 0 || deltaPrice > maxPrice)
	{
		// invalid input
		BSHOP_MSGBOX(GetStringFromTable(5614));
		return;
	}

	if(maxPrice >= SYSAUCTION_MAX_CASH)
	{
		// over limitation
		ACString szTxt;
		szTxt.Format(GetStringFromTable(5618), GetCashText(SYSAUCTION_MAX_CASH, false));
		BSHOP_MSGBOX(szTxt);
		return;
	}
	
	if((unsigned int)maxPrice > m_Account.Cash + GetHostPlayer()->GetCash())
	{
		// cash not enough
		BSHOP_MSGBOX(GetStringFromTable(5615));
		return;
	}

	if(!GetSelectedItem())
	{
		return;
	}

	GetSelectedItem()->MaxPrice = maxPrice;
	GetSelectedItem()->DeltaPrice = deltaPrice;

	PAUIDIALOG pMsgBox;
	GetGameUIMan()->MessageBox("Game_BShop_Autobid_Confirm", GetStringFromTable(5611), MB_YESNO, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
	SetBidState(STATE_CONFIRM);
}

void CDlgBShop::OnCommand_CancelAuto(const char *szCommand)
{
	SetAuto(false);
}

ACString CDlgBShop::GetCashText(int nCash, bool bFullText)
{
	return GetGameUIMan()->GetCashText(nCash, bFullText);
}

void CDlgBShop::OnMessageBox(AUIDialog* pDlg, int iRetVal)
{
	if(!stricmp(pDlg->GetName(), "Game_BShop_Bid_Confirm"))
	{
		bool isOnOK = iRetVal == IDYES;
		SetBidState( isOnOK ? STATE_BIDDING:STATE_WAIT );
		if (isOnOK)
		{
			m_strLastSelectedItem = GetSelectedItem()->pItem->GetName();
		}
	}
	else if(!stricmp(pDlg->GetName(), "Game_BShop_Autobid_Confirm"))
	{
		SetAuto((GetSelectedItem() && iRetVal == IDYES) ? GetSelectedItem()->pNetData->sa_id : 0);
		SetBidState(STATE_WAIT);
	}
}

ACString CDlgBShop::GetItemName(unsigned int sa_id, unsigned int item_id)
{
	ACString szTxt;
	
	if(item_id > 0)
	{
		CECIvtrItem *pItem = CECIvtrItem::CreateItem(item_id, 0, 1);
		szTxt = pItem->GetName();
		delete pItem;
	}
	
	return szTxt;
}

void CDlgBShop::CheckDelayMessage()
{
	ALISTPOSITION p = m_MsgCache.GetHeadPosition();
	while(p)
	{
		ALISTPOSITION cur = p;
		AuctionMessage& m = m_MsgCache.GetNext(p);
		if(OnSysAuctionMessage(m.type, m.o.begin(), m.o.size(), false))
		{
			m_MsgCache.RemoveAt(cur);
		}
	}
}

const ACHAR* CDlgBShop::CheckUserName(int idType, Octets& o, int roleid, bool isEnqueue)
{
	// trying to check name
	const ACHAR* szName = GetGame()->GetGameRun()->GetPlayerName(roleid, false);
	
	if(!szName && isEnqueue)
	{
		AuctionMessage m;
		m.type = idType;
		m.o	= o;
		m_MsgCache.AddTail(m);
		GetGameSession()->GetPlayerBriefInfo(1, &roleid, 2);
	}

	return szName;
}

bool CDlgBShop::OnSysAuctionMessage(int idType, const void* pBuf, size_t sz, bool isEnqueue)
{
	switch(idType)
	{
	case 18: // CMSG_SYSAUCTION_FORENOTICE
		if( sz >= sizeof(unsigned int) * 1)
		{
			unsigned int hour_second;
			Marshal::OctetsStream(Octets(pBuf, sz)) >> hour_second;
			
			ACString strMsg;
			strMsg.Format(	GetStringFromTable(5602), hour_second / 3600);
			GetGame()->GetGameRun()->AddChatMessage(strMsg, GP_CHAT_BROADCAST, idType);
		}
		else
		{
			// invalid message
			ASSERT(FALSE);
		}
		break;

	case 19: // CMSG_SYSAUCTION_START
		if( sz >= sizeof(unsigned int) * 3)
		{
			unsigned int sa_id;
			unsigned int item_id;
			unsigned int base_price;
			Marshal::OctetsStream(Octets(pBuf, sz)) >> sa_id >> item_id >> base_price;
			
			ACString strMsg;
			strMsg.Format( GetStringFromTable(5603), GetItemName(sa_id, item_id), base_price/100 );
			GetGame()->GetGameRun()->AddChatMessage(strMsg, GP_CHAT_BROADCAST, idType);
		}
		else
		{
			// invalid message
			ASSERT(FALSE);
		}
		break;

	case 20: // CMSG_SYSAUCTION_BID
		if( sz >= sizeof(unsigned int) * 4)
		{
			unsigned int sa_id;
			unsigned int item_id;
			unsigned int bid_price;
			int bidder_roleid;

			Octets o(pBuf, sz);
			Marshal::OctetsStream(o) >> sa_id >> item_id >> bid_price >> bidder_roleid;
			
			// trying to check name
			const ACHAR* szName = CheckUserName(idType, o, bidder_roleid, isEnqueue);
			if(!szName)
			{
				return false;
			}
			
			// add to SysAuction GUI
			ACString strMsg;
			strMsg.Format(GetStringFromTable(5604), szName, GetItemName(sa_id, item_id), bid_price/100);
			m_pTxt_Details->AppendText(strMsg);
			m_pTxt_Details->AppendText(_AL("\r"));
			m_pTxt_Details->ScrollToBottom();
		}
		else
		{
			// invalid message
			ASSERT(FALSE);
		}
		break;

	case 21: // CMSG_SYSAUCTION_BIDINFO
		if( sz >= sizeof(unsigned int) * 6)
		{
			unsigned int sa_id;
			unsigned int item_id;
			unsigned int bid_price;
			int bidder_roleid;
			unsigned int freeze_time;
			unsigned int left_time;
			
			Octets o(pBuf, sz);
			Marshal::OctetsStream(o) >> sa_id >> item_id >> bid_price >> bidder_roleid >> freeze_time >>  left_time;
			
			// trying to check name
			const ACHAR* szName = CheckUserName(idType, o, bidder_roleid, isEnqueue);
			if(!szName)
			{
				return false;
			}

			// show world broadcast
			ACString strMsg;
			float ratio = (float)left_time / (float)freeze_time;
			if(ratio > (1.f/3.f)) // [0~0.333) period
			{
				strMsg.Format( GetStringFromTable(5606), GetItemName(sa_id, item_id), szName, bid_price / 100,
					freeze_time / 60, szName);
			}
			else if(left_time == 0) // auction end
			{
				strMsg.Format( GetStringFromTable(5608), GetItemName(sa_id, item_id), szName, bid_price / 100,
					freeze_time / 60, szName);
			}
			else // [0.3333~1) period
			{
				strMsg.Format( GetStringFromTable(5607), GetItemName(sa_id, item_id), szName, bid_price / 100,
					(freeze_time - left_time) / 60);
			}
			
			GetGame()->GetGameRun()->AddChatMessage(strMsg, GP_CHAT_BROADCAST, idType);
		}
		else
		{
			// invalid message
			ASSERT(FALSE);
		}
		break;

	case 22: // CMSG_SYSAUCTION_END
		if( sz >= sizeof(unsigned int) * 4)
		{
			unsigned int sa_id;
			unsigned int item_id;
			unsigned int bid_price;
			int bidder_roleid;

			Octets o(pBuf, sz);
			Marshal::OctetsStream(o) >> sa_id >> item_id >> bid_price >> bidder_roleid;

			// trying to check name
			const ACHAR* szName = CheckUserName(idType, o, bidder_roleid, isEnqueue);
			if(!szName)
			{
				return false;
			}
			
			// show world broadcast
			ACString strMsg;
			strMsg.Format( GetStringFromTable(5605), GetItemName(sa_id, item_id), szName, bid_price / 100 );
			GetGame()->GetGameRun()->AddChatMessage(strMsg, GP_CHAT_BROADCAST, idType);
		}
		else
		{
			// invalid message
			ASSERT(FALSE);
		}
	}

	return true;
}

void CDlgBShop::OnCommand_Payment(const char *szCommand)
{
	AString strText = GetBaseUIMan()->GetURL("Payment", "URL");	
	GetBaseUIMan()->NavigateURL(strText, "Win_Explorer", true);
}

void CDlgBShop::ShowCash(unsigned int freezeCash, unsigned int curCash)
{
	PAUILABEL pTxt_Cash = 
		dynamic_cast<PAUILABEL>(GetDlgItem("Txt_Cash"));
	
	PAUILABEL pTxt_Frozen = 
		dynamic_cast<PAUILABEL>(GetDlgItem("Txt_Frozen"));
	
	pTxt_Frozen->SetText(GetCashText(freezeCash, false));
	pTxt_Cash->SetText(GetCashText(curCash, false));
}

bool CDlgBShop::AuctionAccount::Add(unsigned int id)
{
	if(IsIn(id) >= 0)
	{
		return false;
	}

	this->Items.push_back(id);
	return true;
}

bool CDlgBShop::AuctionAccount::Remove(unsigned int id)
{
	int index = IsIn(id);
	if(index >= 0)
	{
		this->Items.erase(this->Items.begin() + index);
		return true;
	}

	return false;
}

int CDlgBShop::AuctionAccount::IsIn(unsigned int id)
{
	for(size_t i=0;i<this->Items.size();++i)
	{
		if(this->Items[i] == id)
		{
			return i;
		}
	}

	return -1;
}