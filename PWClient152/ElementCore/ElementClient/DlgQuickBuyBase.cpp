// Filename	: DlgQuickBuyBase.cpp
// Creator	: Xu Wenbin
// Date		: 2013/12/10

#include "DlgQuickBuyBase.h"
#include "EC_HostPlayer.h"
#include "EC_Inventory.h"
#include "EC_GameUIMan.h"
#include "EC_Shop.h"
#include "EC_ShopSearch.h"
#include "EC_UIConfigs.h"
#include "EC_TimeSafeChecker.h"

#include "globaldataman.h"

#define new A_DEBUG_NEW

//	CDlgQuickBuyBase::ConfirmedItem
CDlgQuickBuyBase::ConfirmedItem::ConfirmedItem(int id)
: m_id(id)
{
	m_confirmTime = ::GetTickCount();
}

bool CDlgQuickBuyBase::ConfirmedItem::IsTimeout(DWORD dwCurTickCount)const
{
	return CECTimeSafeChecker::ElapsedTime(dwCurTickCount, m_confirmTime) >= 5000;	//	5秒后还没有成功购买、则认为失败
}

//	CDlgQuickBuyBase
AUI_BEGIN_COMMAND_MAP(CDlgQuickBuyBase, CDlgBase)
AUI_ON_COMMAND("Btn_QuickBuy",	OnCommandQuickBuy)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgQuickBuyBase, CDlgBase)
AUI_END_EVENT_MAP()

CDlgQuickBuyBase::CDlgQuickBuyBase()
: m_pTxt_QuickBuy(NULL)
, m_pBtn_QuickBuy(NULL)
, m_itemToBuy(0)
, m_needConfirm(true)
{
}

bool CDlgQuickBuyBase::OnInitDialog()
{
	m_pTxt_QuickBuy = GetDlgItem("Txt_QuickBuy");
	m_pBtn_QuickBuy = GetDlgItem("Btn_QuickBuy");
	return CDlgBase::OnInitDialog();
}

void CDlgQuickBuyBase::OnHideDialog()
{
	m_confirmedItems.clear();
	QuickBuyDisappear();
}

void CDlgQuickBuyBase::OnTick()
{
	//	循环中处理，避免个别物品先购买失败导致后续购买物品都无法检查
	//	每次只处理一个，同一物品多次购买会逐次通知
	CECInventory *pack = GetHostPlayer()->GetPack();
	for (ConfirmedItems::iterator it = m_confirmedItems.begin(); it != m_confirmedItems.end(); ++ it)
	{
		const ConfirmedItem &cur = *it;
		int id = cur.GetID();
		int iSlot = pack->FindItem(id);
		if (iSlot >= 0){
			m_confirmedItems.erase(it);
			OnItemAppearInPack(id, iSlot);
			break;
		}
		if (cur.IsTimeout(::GetTickCount())){
			m_confirmedItems.erase(it);
			break;
		}
	}
}

bool CDlgQuickBuyBase::QuickBuyAppear(const ACHAR *szMsg, int itemToBuy, bool needConfirm/* =true */, bool needFlash/* =false */)
{
	if (!CECUIConfig::Instance().GetGameUI().bEnableRecommendQShopItem){
		return false;
	}
	if (!m_pTxt_QuickBuy || !m_pBtn_QuickBuy){
		return false;
	}
	CECShopSearch searcher(GetShopData());
	searcher.SetPolicy(GetShopSearchPolicy(itemToBuy));
	if (!searcher.Search(NULL)){
		//	无法从商城购买时、清除已有显示，以此方式提示错误
		QuickBuyDisappear();
		return false;
	}
	//	覆盖已有内容
	if (m_pTxt_QuickBuy){
		m_pTxt_QuickBuy->SetText(szMsg);
		m_pTxt_QuickBuy->Show(true);
	}
	if (m_pBtn_QuickBuy){
		m_pBtn_QuickBuy->Show(true);
		m_pBtn_QuickBuy->SetFlash(needFlash);
	}
	m_itemToBuy = itemToBuy;
	m_needConfirm = needConfirm;
	return true;
}

void CDlgQuickBuyBase::QuickBuyDisappear()
{
	if (m_pTxt_QuickBuy){
		m_pTxt_QuickBuy->Show(false);
	}
	if (m_pBtn_QuickBuy){
		m_pBtn_QuickBuy->SetFlash(false);
		m_pBtn_QuickBuy->Show(false);
	}
	m_itemToBuy = 0;
	m_needConfirm = true;
}

void CDlgQuickBuyBase::OnBuyConfirmShow(int id, int itemIndex, int buyIndex)
{
	if (m_pBtn_QuickBuy){
		m_pBtn_QuickBuy->SetFlash(false);
	}
}

void CDlgQuickBuyBase::OnBuyConfirmed(int id, int itemIndex, int buyIndex)
{
	if (id <= 0){
		ASSERT(false);
	}else if (id == m_itemToBuy){
		//	添加到队列中等待检查购买结果
		m_confirmedItems.push_back(id);
	}
}

void CDlgQuickBuyBase::OnBuyCancelled(int id, int itemIndex, int buyIndex)
{
}

void CDlgQuickBuyBase::OnCommandQuickBuy(const char * szCommand)
{
	if (m_itemToBuy <= 0){
		return;
	}
	int iSlot = GetHostPlayer()->GetPack()->FindItem(m_itemToBuy);
	if (iSlot >= 0){
		//	包裹中已有此物品
		GetGameUIMan()->AddChatMessage(GetStringFromTable(11210), GP_CHAT_MISC);
		OnItemAppearInPack(m_itemToBuy, iSlot);
		return;
	}
	
	//	检测商城中目前是否符合要求的物品
	CECShopSearchResult candidates(GetShopData());
	CECShopSearch searcher(GetShopData());
	searcher.SetPolicy(GetShopSearchPolicy(m_itemToBuy));
	if (!searcher.Search(&candidates)){
		GetGameUIMan()->AddChatMessage(GetStringFromTable(11211), GP_CHAT_MISC);
		return;
	}

	//	检查元宝是否充足
	CECShopSearchPricePolicy pricePolicy(GetShopData()->GetCash());
	candidates.Filter(pricePolicy);
	if (candidates.IsEmpty()){
		GetGameUIMan()->AddChatMessage(GetStringFromTable(11212), GP_CHAT_MISC);
		return;
	}

	//	检查背包是否有空间
	CECShopSearchPackSpacePolicy spacePolicy(GetHostPlayer()->GetPack()->GetEmptySlotNum());
	candidates.Filter(spacePolicy);
	if (candidates.IsEmpty()){
		GetGameUIMan()->AddChatMessage(GetStringFromTable(11213), GP_CHAT_MISC);
		return;
	}

	//	有可以购买的物品，选择价格最低的
	int itemIndex = -1;
	int buyIndex = -1;
	int price(-1);
	for (int i(0); i < candidates.Count(); ++ i)
	{
		int tmpItemIndex = candidates[i];
		int tmpbuyIndex(-1);
		if (!GetShopData()->CalcBuyIndex(tmpItemIndex, &tmpbuyIndex)){
			assert(false);
			return;
		}
		const GSHOP_ITEM *pItem = GetShopData()->GetItem(tmpItemIndex);
		int tmpPrice = pItem->buy[tmpbuyIndex].price;
		if (itemIndex == -1 || tmpPrice < price){
			itemIndex = tmpItemIndex;
			buyIndex = tmpbuyIndex;
			price = tmpPrice;
		}
	}

	//	弹出购买框、注册通知函数
	CDlgBuyConfirm *pDlgBuyConfirm = dynamic_cast<CDlgBuyConfirm *>(GetGameUIMan()->GetDialog("Win_Message6"));
	pDlgBuyConfirm->Buy(GetShopData(), itemIndex, buyIndex, m_needConfirm, this, true);
}
