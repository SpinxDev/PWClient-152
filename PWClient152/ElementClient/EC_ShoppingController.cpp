// File		: EC_ShoppingController.cpp
// Creator	: Xu Wenbin
// Date		: 2014/5/15

#include "EC_ShoppingController.h"
#include "EC_ShoppingCartChecker.h"
#include "EC_ShoppingCart.h"
#include "EC_Shop.h"

#include <ABaseDef.h>
#include "EC_IvtrTypes.h"
#include "globaldataman.h"

#include <ALog.h>

//	struct ItemShoppingInfo
ItemShoppingInfo::ItemShoppingInfo()
: m_itemCartID(INVALID_ITEM_SHOPPINGCART_ID)
, m_itemIndex(-1)
, m_buyIndex(-1)
, m_buyedCount(0)
, m_inbuyCount(0)
, m_tobuyCount(0){
}

ItemShoppingInfo::ItemShoppingInfo(const CECShoppingCartItem *pItem)
: m_itemCartID(pItem->ItemCartID())
, m_itemIndex(pItem->GShopItemIndex())
, m_buyIndex(pItem->BuyIndex())
, m_buyedCount(0)
, m_inbuyCount(0)
, m_tobuyCount(pItem->Count()){
}

int ItemShoppingInfo::CountNeedToBuy()const{
	return m_tobuyCount;
}
void ItemShoppingInfo::OnBuy(int count){
	if (count > CountNeedToBuy()){
		ASSERT(false);
		count = CountNeedToBuy();
	}
	m_inbuyCount += count;
	m_tobuyCount -= count;
}

//	class ItemShoppingInfos
int globalGetItemShoppingBuyedCount(const ItemShoppingInfos &infos){
	int result(0);
	for (ItemShoppingInfos::const_iterator cit = infos.begin(); cit != infos.end(); ++ cit){
		result += cit->m_buyedCount;
	}
	return result;
}

int globalGetItemShoppingInBuyCount(const ItemShoppingInfos &infos){
	int result(0);
	for (ItemShoppingInfos::const_iterator cit = infos.begin(); cit != infos.end(); ++ cit){
		result += cit->m_inbuyCount;
	}
	return result;
}

int globalGetItemShoppingToBuyCount(const ItemShoppingInfos &infos){
	int result(0);
	for (ItemShoppingInfos::const_iterator cit = infos.begin(); cit != infos.end(); ++ cit){
		result += cit->m_tobuyCount;
	}
	return result;
}

//	class CECShoppingControllerChange
CECShoppingControllerChange::CECShoppingControllerChange(const CECShoppingCart *pShoppingCart, ChangeMask changeMask
														 , ItemShoppingInfos *pBuyedItems
														 , ItemShoppingInfos *pBuyingItems
														 , ItemShoppingInfos *pWaitingItems
														 , enumShoppingControllerStopBuyingReason stopReason)
														 : m_changeMask(changeMask)
														 , m_pShoppingCart(pShoppingCart)
														 , m_stopReason(stopReason)
{
	if (pBuyedItems){
		m_buyedItems = *pBuyedItems;
	}
	if (pBuyingItems){
		m_buyingItems = *pBuyingItems;
	}
	if (pWaitingItems){
		m_waitingItems = *pWaitingItems;
	}
}

CECShoppingControllerChange::ChangeMask CECShoppingControllerChange::GetChangeMask()const{
	return m_changeMask;
}

const CECShoppingCart* CECShoppingControllerChange::ShoppingCart()const{
	return m_pShoppingCart;
}

const ItemShoppingInfos & CECShoppingControllerChange::BuyedItems()const{
	return m_buyedItems;
}

const ItemShoppingInfos & CECShoppingControllerChange::BuyingItems()const{
	return m_buyingItems;
}

const ItemShoppingInfos & CECShoppingControllerChange::WaitingItems()const{
	return m_waitingItems;
}

enumShoppingControllerStopBuyingReason CECShoppingControllerChange::StopReason()const{
	return m_stopReason;
}

int CECShoppingControllerChange::BuyedItemsCount()const{
	return globalGetItemShoppingBuyedCount(BuyedItems())
		+ globalGetItemShoppingBuyedCount(BuyingItems());
}

int CECShoppingControllerChange::InBuyItemsCount()const{
	return globalGetItemShoppingInBuyCount(BuyingItems());
}

int CECShoppingControllerChange::ToBuyItemsCount()const{
	return globalGetItemShoppingToBuyCount(BuyingItems())
		+ globalGetItemShoppingToBuyCount(WaitingItems());
}

//	class CECShoppingControllerState
bool CECShoppingControllerState::SetShoppingCart(CECShoppingCart *pShoppingCart){
	ASSERT(false);
	return false;
}
bool CECShoppingControllerState::Buy(){
	ASSERT(false);
	return false;
}
bool CECShoppingControllerState::StopBuying(enumShoppingControllerStopBuyingReason reason){
	ASSERT(false);
	return false;
}

//	class SCStateNoShoppingCart
SCStateNoShoppingCart::SCStateNoShoppingCart()
: CECShoppingControllerState(SCS_NO_SHOPPING_CART){
}
bool SCStateNoShoppingCart::SetShoppingCart(CECShoppingCart *pShoppingCart){
	if (!pShoppingCart){
		return false;
	}
	GetParent()->SetShoppingCartImpl(pShoppingCart);
	GetParent()->ChangeState(GetParent()->GetStateIdle());
	return true;
}
//	class SCStateIdle
SCStateIdle::SCStateIdle()
: CECShoppingControllerState(SCS_IDLE){
}
bool SCStateIdle::SetShoppingCart(CECShoppingCart *pShoppingCart){
	GetParent()->m_pShoppingCart = pShoppingCart;
	if (!pShoppingCart){
		GetParent()->ChangeState(GetParent()->GetStateNoShoppingCart());
	}// else ά���ڵ�ǰ״̬����
	return true;
}
bool SCStateIdle::Buy(){

	//	����Ƿ�����Ч��ƷҪ����
	CECShoppingCart *pShoppingCart = GetParent()->m_pShoppingCart;
	CECShoppingCardChecker checker(pShoppingCart);
	if (!checker.HaveItemsToBuy()){
		return false;
	}

	//	�������ﳵ
	pShoppingCart->Lock();

	//	��ʼ����������Ʒ�б��
	GetParent()->m_buyedItems.clear();
	GetParent()->m_buyingItems.clear();
	GetParent()->m_waitingItems.clear();
	for (int i(0); i < pShoppingCart->Count(); ++ i){
		const CECShoppingCartItem *item = pShoppingCart->ItemAt(i);
		if (item->Count() > 0){
			GetParent()->m_waitingItems.push_back(ItemShoppingInfo(item));
		}
	}

	//	�л�������״̬�����һ����Ʒ
	GetParent()->ChangeState(GetParent()->GetStateToSendBuyCommand());

	//	֪ͨ�ѿ�ʼ����
	CECShoppingControllerChange change(pShoppingCart, CECShoppingControllerChange::START_SHOPPING, 
		&GetParent()->m_buyedItems,
		&GetParent()->m_buyingItems,
		&GetParent()->m_waitingItems);
	GetParent()->NotifyObservers(&change);
	return true;
}
//	class SCStateToSendBuyCommand
SCStateToSendBuyCommand::SCStateToSendBuyCommand()
: CECShoppingControllerState(SCS_TO_SEND_BUY_COMMAND){
}
bool SCStateToSendBuyCommand::StopBuying(enumShoppingControllerStopBuyingReason reason){
	GetParent()->StopBuyingImpl(reason);
	return true;
}
void SCStateToSendBuyCommand::Tick(){
	if (GetParent()->m_buyingItems.empty() && GetParent()->m_waitingItems.empty()){
		//	��û����Ʒ����Ҫ����
		StopBuying(SCSBR_FINISHED);
		return;
	}

	const int MAX_ITEM_NUM_BUY_AT_ONCE = 1;	//	ÿ��ͬʱ���͵Ĺ���Э����

	int leftCount = MAX_ITEM_NUM_BUY_AT_ONCE;
	
	//	�ȼ�������δ������ɵ���Ʒ
	CECShoppingCart *pShoppingCart = GetParent()->m_pShoppingCart;
	const CECShopBase *pShop = pShoppingCart->Shop();
	ItemShoppingInfos &buyingItems = GetParent()->m_buyingItems;
	for (int i = 0; i < (int)buyingItems.size() && leftCount > 0; ++ i){
		ItemShoppingInfo &info = buyingItems[i];
		int countNeedToBuy = info.CountNeedToBuy();
		if (countNeedToBuy <= 0){
			ASSERT(false);
			continue;
		}
		int count = min(countNeedToBuy, leftCount);
		info.OnBuy(count);
		for (int j(0); j < count; ++ j){
			pShop->Buy(info.m_itemIndex, info.m_buyIndex);
		}
		leftCount -= count;
	}

	//	�ټ���������Ʒ�б�
	ItemShoppingInfos &waitingItems = GetParent()->m_waitingItems;
	while (leftCount > 0 && !waitingItems.empty()){
		//	��������Ʒ�Ӵ������Ƶ�������
		buyingItems.push_back(waitingItems.front());
		waitingItems.erase(waitingItems.begin());

		ItemShoppingInfo & info = buyingItems.back();
		int countNeedToBuy = info.CountNeedToBuy();
		int count = min(countNeedToBuy, leftCount);

		//	���͹���Э��
		for (int j(0); j < count; ++ j){
			pShop->Buy(info.m_itemIndex, info.m_buyIndex);
		}

		//	������������Ʒ����
		info.OnBuy(count);

		//	���±���ʣ�๺����Ʒ����
		leftCount -= count;
	}

	//	�л����ȴ�����״̬
	GetParent()->ChangeState(GetParent()->GetStateWaitingFeedback());

	//	֪ͨ���¹���Э�鷢��
	CECShoppingControllerChange change(pShoppingCart, CECShoppingControllerChange::SHOPPING_CMD_SENT,
		&GetParent()->m_buyedItems,
		&GetParent()->m_buyingItems,
		&GetParent()->m_waitingItems);
	GetParent()->NotifyObservers(&change);
}
//	class SCStateWaitingFeedback
SCStateWaitingFeedback::SCStateWaitingFeedback()
: CECShoppingControllerState(SCS_WAITING_FEEDBACK){
}
void SCStateWaitingFeedback::Enter(CECShoppingControllerState *prev){
	GetParent()->m_switchTrigger.Reset(2*1000);	//	���Ʊ��ι�������2������ɣ�����޷���ɣ�������״̬�л�
}
bool SCStateWaitingFeedback::StopBuying(enumShoppingControllerStopBuyingReason reason){
	GetParent()->StopBuyingImpl(reason);
	return true;
}
void SCStateWaitingFeedback::Tick(){
	//	����ѹ����������Ʒ�Ƿ����յ�
	ItemShoppingInfos &buyingItems = GetParent()->m_buyingItems;
	for (int i = 0; i < (int)buyingItems.size(); ++ i){
		const ItemShoppingInfo &info = buyingItems[i];
		if (info.m_inbuyCount > 0){
			//	���з���δ�յ�
			if (GetParent()->m_switchTrigger.IsTimeArrived()){
				StopBuying(SCSBR_TIME_ARRIVED);
			}//else �����ȴ�����ȫ���յ�ʱ���л�
			return;
		}
	}
	//	ȫ�������ѳɹ�������ɹ������Ʒ�ƶ�������ɹ���
	ItemShoppingInfos &buyedItems = GetParent()->m_buyedItems;
	for (ItemShoppingInfos::iterator it = buyingItems.begin(); it != buyingItems.end();){
		ItemShoppingInfo &info = *it;
		if (info.CountNeedToBuy() <= 0){
			buyedItems.push_back(info);
			it = buyingItems.erase(it);
		}else{
			++ it;
		}
	}

	//	�л����������ι���
	GetParent()->ChangeState(GetParent()->GetStateToSendBuyCommand());

	//	֪ͨ�����ι���ɹ�
	CECShoppingControllerChange change(GetParent()->m_pShoppingCart, CECShoppingControllerChange::ITEM_APPEAR_IN_PACK,
		&GetParent()->m_buyedItems,
		&GetParent()->m_buyingItems,
		&GetParent()->m_waitingItems);
	GetParent()->NotifyObservers(&change);
}
bool SCStateWaitingFeedback::OnObtainItem(int iPack, int idItem, int iAmount){
	if (iPack != IVTRTYPE_PACK){
		return false;
	}
	const CECShoppingCart *pShoppingCart = GetParent()->m_pShoppingCart;
	const CECShopBase *pShop = pShoppingCart->Shop();
	ItemShoppingInfos &buyingItems = GetParent()->m_buyingItems;
	for (ItemShoppingInfos::iterator it = buyingItems.begin(); it != buyingItems.end(); ++ it){
		ItemShoppingInfo &info = *it;
		if (info.m_inbuyCount <= 0){
			continue;
		}
		const GSHOP_ITEM *pGShopItem = pShop->GetItem(info.m_itemIndex);
		if ((int)pGShopItem->id != idItem || (int)pGShopItem->num != iAmount){
			continue;
		}
		++ info.m_buyedCount;
		-- info.m_inbuyCount;

		const CECShoppingCartItem * pCartItem = pShoppingCart->ItemForID(info.m_itemCartID);
		if (pCartItem->GShopItemIndex() != info.m_itemIndex ||
			pCartItem->BuyIndex() != info.m_buyIndex ||
			pCartItem->Count() < 1){
			ASSERT(false);
		}else{
			GetParent()->UpdateShoppingCartOnObtainItem(info.m_itemCartID);
		}
		//	�� Tick �м���Ƿ���ȫ������

		return true;
	}
	return false;
}

//	class CECShoppingController
CECShoppingController::CECShoppingController()
: m_curState(NULL)
, m_pShoppingCart(NULL)
{
	GetStateNoShoppingCart()->Init(this);
	GetStateIdle()->Init(this);
	GetStateToSendBuyCommand()->Init(this);
	GetStateWaitingFeedback()->Init(this);
	
	ChangeState(GetStateNoShoppingCart());
}

CECShoppingController::~CECShoppingController(){
	if (m_pShoppingCart){
		m_pShoppingCart->UnregisterObserver(this);
		m_pShoppingCart = NULL;
	}
}

void CECShoppingController::OnModelChange(const CECShoppingCart *p, const CECObservableChange *q){
	if (m_pShoppingCart != p){
		ASSERT(false);
		return;
	}
	const CECShoppingCartChange *pChange = dynamic_cast<const CECShoppingCartChange *>(q);
	if (!pChange){
		ASSERT(false);
		return;
	}
	if (!IsBuying()){
		return;
	}
	switch (pChange->GetChangeMask()){
	case CECShoppingCartChange::MALL_ITEMS_CHANGED:
		StopBuyingImpl(SCSBR_MALL_ITEM_CHANGED);
		a_LogOutput(1, "CECShoppingController::OnModelChange, stop buying because mall items updated.");
		break;
	}
}

void CECShoppingController::OnUnregister(const CECShoppingCart *p){
	if (m_pShoppingCart != p){
		ASSERT(false);
		return;
	}
	m_pShoppingCart = NULL;
}

bool CECShoppingController::SetShoppingCart(CECShoppingCart *pShoppingCart){
	return m_curState->SetShoppingCart(pShoppingCart);
}

bool CECShoppingController::Buy(){
	a_LogOutput(1, "%s::Buy", m_curState->GetName());
	return m_curState->Buy();
}

bool CECShoppingController::StopBuying(enumShoppingControllerStopBuyingReason reason){
	a_LogOutput(1, "%s::StopBuying", m_curState->GetName());
	return m_curState->StopBuying(reason);
}

void CECShoppingController::Tick(){
	m_curState->Tick();
}

bool CECShoppingController::IsBuying(){
	return m_curState->IsBuying();
}

bool CECShoppingController::OnObtainItem(int iPack, int idItem, int iAmount){
	return m_curState->OnObtainItem(iPack, idItem, iAmount);
}

void CECShoppingController::ChangeState(CECShoppingControllerState *state){	
	if (state == m_curState){
		return;
	}
	CECShoppingControllerState *prev = m_curState;
	m_curState = state;
	m_curState->Enter(prev);
}

void CECShoppingController::SetShoppingCartImpl(CECShoppingCart *pShoppingCart){
	if (pShoppingCart == m_pShoppingCart){
		return;
	}
	if (m_pShoppingCart){
		m_pShoppingCart->UnregisterObserver(this);
		m_pShoppingCart = NULL;
	}
	if (m_pShoppingCart = pShoppingCart){
		m_pShoppingCart->RegisterObserver(this);
	}
}

void CECShoppingController::StopBuyingImpl(enumShoppingControllerStopBuyingReason reason){
	//	ȡ���������ﳵ
	m_pShoppingCart->Unlock();
	
	//	�л���������״̬
	ChangeState(GetStateIdle());
	
	//	֪ͨ��������
	CECShoppingControllerChange change(m_pShoppingCart, CECShoppingControllerChange::STOP_SHOPPING,
		&m_buyedItems,
		&m_buyingItems,
		&m_waitingItems,
		reason);
	NotifyObservers(&change);
	
	//	�����������Ʒ�б��
	m_buyedItems.clear();
	m_buyingItems.clear();
	m_waitingItems.clear();
}

void CECShoppingController::UpdateShoppingCartOnObtainItem(ItemShoppingCartID itemCartID){
	const CECShoppingCartItem * pCartItem = m_pShoppingCart->ItemForID(itemCartID);
	if (!pCartItem){
		ASSERT(false);
	}else if (pCartItem->Count() == 1){
		m_pShoppingCart->RemoveItemImpl(itemCartID, true);
	}else{
		m_pShoppingCart->DecreaseItemCountImpl(itemCartID, 1, true);
	}
}