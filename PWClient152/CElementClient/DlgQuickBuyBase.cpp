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
	return CECTimeSafeChecker::ElapsedTime(dwCurTickCount, m_confirmTime) >= 5000;	//	5���û�гɹ���������Ϊʧ��
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
	//	ѭ���д������������Ʒ�ȹ���ʧ�ܵ��º���������Ʒ���޷����
	//	ÿ��ֻ����һ����ͬһ��Ʒ��ι�������֪ͨ
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
		//	�޷����̳ǹ���ʱ�����������ʾ���Դ˷�ʽ��ʾ����
		QuickBuyDisappear();
		return false;
	}
	//	������������
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
		//	��ӵ������еȴ���鹺����
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
		//	���������д���Ʒ
		GetGameUIMan()->AddChatMessage(GetStringFromTable(11210), GP_CHAT_MISC);
		OnItemAppearInPack(m_itemToBuy, iSlot);
		return;
	}
	
	//	����̳���Ŀǰ�Ƿ����Ҫ�����Ʒ
	CECShopSearchResult candidates(GetShopData());
	CECShopSearch searcher(GetShopData());
	searcher.SetPolicy(GetShopSearchPolicy(m_itemToBuy));
	if (!searcher.Search(&candidates)){
		GetGameUIMan()->AddChatMessage(GetStringFromTable(11211), GP_CHAT_MISC);
		return;
	}

	//	���Ԫ���Ƿ����
	CECShopSearchPricePolicy pricePolicy(GetShopData()->GetCash());
	candidates.Filter(pricePolicy);
	if (candidates.IsEmpty()){
		GetGameUIMan()->AddChatMessage(GetStringFromTable(11212), GP_CHAT_MISC);
		return;
	}

	//	��鱳���Ƿ��пռ�
	CECShopSearchPackSpacePolicy spacePolicy(GetHostPlayer()->GetPack()->GetEmptySlotNum());
	candidates.Filter(spacePolicy);
	if (candidates.IsEmpty()){
		GetGameUIMan()->AddChatMessage(GetStringFromTable(11213), GP_CHAT_MISC);
		return;
	}

	//	�п��Թ������Ʒ��ѡ��۸���͵�
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

	//	���������ע��֪ͨ����
	CDlgBuyConfirm *pDlgBuyConfirm = dynamic_cast<CDlgBuyConfirm *>(GetGameUIMan()->GetDialog("Win_Message6"));
	pDlgBuyConfirm->Buy(GetShopData(), itemIndex, buyIndex, m_needConfirm, this, true);
}
