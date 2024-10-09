// File		: EC_ShoppingCart.cpp
// Creator	: Xu Wenbin
// Date		: 2014/5/14

#include "EC_ShoppingCart.h"
#include "EC_Shop.h"

#include <ABaseDef.h>
#include <ALog.h>

const ItemShoppingCartID INVALID_ITEM_SHOPPINGCART_ID = -1;

//	class CECShoppingCartChange
CECShoppingCartChange::CECShoppingCartChange(ChangeMask changeMask, CECShoppingCartItem *pCartItem/* =NULL */, int addedItemCount/* =0 */)
: m_changeMask(changeMask)
, m_pItem(pCartItem)
, m_addedCount(addedItemCount){
}

CECShoppingCartChange::ChangeMask CECShoppingCartChange::GetChangeMask()const{
	return m_changeMask;
}

const CECShoppingCartItem *CECShoppingCartChange::Item()const{
	return m_pItem;
}

int CECShoppingCartChange::AddedItemCount()const{
	return m_addedCount;
}

bool CECShoppingCartChange::IsClear()const{
	return m_changeMask == CART_CLEARED;
}

bool CECShoppingCartChange::IsLock()const{
	return m_changeMask == CART_LOCKED;
}

bool CECShoppingCartChange::IsUnlock()const{
	return m_changeMask == CART_UNLOCKED;
}

bool CECShoppingCartChange::IsItemCountChanged()const{
	return m_changeMask == ITEM_COUNT_CHANGE;
}

bool CECShoppingCartChange::IsRemoveItem()const{
	return m_changeMask == ITEM_REMOVED;
}

bool CECShoppingCartChange::IsAddItem()const{
	return m_changeMask == ITEM_ADDED;
}

bool CECShoppingCartChange::IsMallItemsChanged()const{
	return m_changeMask == MALL_ITEMS_CHANGED;
}

//	class CECShoppingCartItem
CECShoppingCartItem::CECShoppingCartItem(ItemShoppingCartID id, int itemIndex, int buyIndex, int count)
: m_shopCardItemID(id)
, m_itemIndex(itemIndex)
, m_buyIndex(buyIndex)
, m_count(count){
}

ItemShoppingCartID CECShoppingCartItem::ItemCartID()const{
	return m_shopCardItemID;
}

int	CECShoppingCartItem::GShopItemIndex()const{
	return m_itemIndex;
}

int	CECShoppingCartItem::BuyIndex()const{
	return m_buyIndex;
}

int	CECShoppingCartItem::Count()const{
	return m_count;
}

void CECShoppingCartItem::Increase(int count){
	if (count > 0){
		m_count += count;
	}
}

void CECShoppingCartItem::Decrease(int count){
	if (count > 0 && count <= m_count){		//	允许减少到0
		m_count -= count;
	}
}

bool CECShoppingCartItem::operator == (ItemShoppingCartID id)const{
	return ItemCartID() == id;
}

//	class CECShoppingCart
CECShoppingCart::CECShoppingCart(CECShopBase *pShop)
: m_pShop(pShop)
, m_nextID(1)
, m_locked(false){
	if (pShop){
		pShop->RegisterObserver(this);
	}
}

CECShoppingCart::~CECShoppingCart(){
	if (m_pShop){
		m_pShop->UnregisterObserver(this);
	}
}

void CECShoppingCart::OnModelChange(const CECShopBase *p, const CECObservableChange *q){
	if (p != m_pShop){
		ASSERT(false);
		return;
	}
	const CECShopBaseChange * pChange = dynamic_cast<const CECShopBaseChange *>(q);
	if (pChange->ItemChanged()){
		for (ShoppingCartItems::iterator it = m_items.begin(); it != m_items.end();){
			const CECShoppingCartItem & item = *it;
			if (m_pShop->ReadyToBuy(item.GShopItemIndex(), item.BuyIndex())){
				++ it;
			}else{
				it = m_items.erase(it);
			}
		}
		CECShoppingCartChange change(CECShoppingCartChange::MALL_ITEMS_CHANGED);
		NotifyObservers(&change);
	}
}

const CECShopBase * CECShoppingCart::Shop()const{
	return m_pShop;
}

bool CECShoppingCart::IsEmpty()const{
	return m_items.empty();
}

int	CECShoppingCart::Count()const{
	return (int)m_items.size();
}

const CECShoppingCartItem *CECShoppingCart::ItemAt(int index)const{
	if (index >= 0 && index < Count()){
		return &m_items[index];
	}
	return NULL;
}

const CECShoppingCartItem *CECShoppingCart::ItemForIDImpl(ItemShoppingCartID id)const{
	ShoppingCartItems::const_iterator cit = std::find(m_items.begin(), m_items.end(), id);
	return cit == m_items.end() ? NULL : &*cit;
}

const CECShoppingCartItem *CECShoppingCart::ItemForShopIndexImpl(int itemIndex, int buyIndex)const{
	for (ShoppingCartItems::const_iterator cit = m_items.begin(); cit != m_items.end(); ++ cit){
		const CECShoppingCartItem & item = *cit;
		if (item.GShopItemIndex() == itemIndex && item.BuyIndex() == buyIndex){
			return &item;
		}
	}
	return NULL;
}

const CECShoppingCartItem *CECShoppingCart::ItemForID(ItemShoppingCartID id)const{
	return ItemForIDImpl(id);
}

CECShoppingCartItem *CECShoppingCart::ItemForID(ItemShoppingCartID id){
	return const_cast<CECShoppingCartItem *>(ItemForIDImpl(id));
}

const CECShoppingCartItem *CECShoppingCart::ItemForShopIndex(int itemIndex, int buyIndex)const{
	return ItemForShopIndexImpl(itemIndex, buyIndex);
}

CECShoppingCartItem *CECShoppingCart::ItemForShopIndex(int itemIndex, int buyIndex){
	return const_cast<CECShoppingCartItem *>(ItemForShopIndexImpl(itemIndex, buyIndex));
}

int	CECShoppingCart::CashCost()const{
	int cost(0);
	for (int i(0); i < Count(); ++ i){
		const CECShoppingCartItem * item = ItemAt(i);
		int unitPrice = Shop()->GetPrice(item->GShopItemIndex(), item->BuyIndex());
		if (unitPrice < 0){
			cost = CECSHOP_INVALID_PRICE;
			break;
		}
		cost += unitPrice * item->Count();
	}
	return cost;
}

bool CECShoppingCart::IsLocked()const{
	return m_locked;
}

void CECShoppingCart::Clear(){
	if (IsLocked()){
		return;
	}
	m_items.clear();
	CECShoppingCartChange change(CECShoppingCartChange::CART_CLEARED);
	NotifyObservers(&change);
}

void CECShoppingCart::Lock(){
	if (m_locked){
		ASSERT(false);
		return;
	}
	m_locked = true;
	CECShoppingCartChange change(CECShoppingCartChange::CART_LOCKED);
	NotifyObservers(&change);
}

void CECShoppingCart::Unlock(){
	if (!m_locked){
		ASSERT(false);
		return;
	}
	m_locked = false;
	CECShoppingCartChange change(CECShoppingCartChange::CART_UNLOCKED);
	NotifyObservers(&change);
}

bool CECShoppingCart::AddItemImpl(int itemIndex, int buyIndex, int count, bool bForce){
	if (!Shop()->IsValid(itemIndex, buyIndex) || count <= 0){
		ASSERT(false);
		return false;
	}
	if (!bForce && IsLocked()){
		return false;
	}
	if (CECShoppingCartItem *pItem = ItemForShopIndex(itemIndex, buyIndex)){
		return ModifyItemCount(pItem, count);
	}
	m_items.push_back(CECShoppingCartItem(m_nextID ++, itemIndex, buyIndex, count));
	
	CECShoppingCartChange change(CECShoppingCartChange::ITEM_ADDED, &m_items.back());
	NotifyObservers(&change);

	return true;
}

bool CECShoppingCart::IncreaseItemCountImpl(ItemShoppingCartID id, int count, bool bForce){
	if (count <= 0){
		ASSERT(false);
		return false;
	}
	CECShoppingCartItem *pItem = ItemForID(id);
	if (!pItem){
		ASSERT(false);
		return false;
	}
	if (!bForce && IsLocked()){
		return false;
	}
	return ModifyItemCount(pItem, count);
}

bool CECShoppingCart::DecreaseItemCountImpl(ItemShoppingCartID id, int count, bool bForce){
	if (count <= 0){
		ASSERT(false);
		return false;
	}
	CECShoppingCartItem *pItem = ItemForID(id);
	if (!pItem){
		ASSERT(false);
		return false;
	}
	if (!bForce && IsLocked()){
		return false;
	}
	return ModifyItemCount(pItem, -count);
}

bool CECShoppingCart::SetItemCountImpl(ItemShoppingCartID id, int count, bool bForce){
	if (count < 0){
		ASSERT(false);
		return false;
	}
	CECShoppingCartItem *pItem = ItemForID(id);
	if (!pItem){
		ASSERT(false);
		return false;
	}
	if (!bForce && IsLocked()){
		return false;
	}
	count -= pItem->Count();
	return ModifyItemCount(pItem, count);
}

bool CECShoppingCart::RemoveItemImpl(ItemShoppingCartID id, bool bForce){
	ShoppingCartItems::iterator it = std::find(m_items.begin(), m_items.end(), id);
	if (it == m_items.end()){
		ASSERT(false);
		return false;
	}
	if (!bForce && IsLocked()){
		return false;
	}
	CECShoppingCartItem itemToDelete = *it;	//	备份待删除物品、用于通知
	m_items.erase(it);
	CECShoppingCartChange change(CECShoppingCartChange::ITEM_REMOVED, &itemToDelete);
	NotifyObservers(&change);
	return true;
}

bool CECShoppingCart::ModifyItemCount(CECShoppingCartItem *pItem, int count){	//	允许减少到0
	if (!pItem || count == 0){
		return false;
	}
	if (pItem->Count()+count < 0){
		if (count > 0){
			a_LogOutput(1, "CECShoppingCart::ModifyItemCount, Attemp to OVERFLOW %d by adding %d", pItem->Count(), count);
		}else{ // count < 0
			a_LogOutput(1, "CECShoppingCart::ModifyItemCount, Attemp to UNDERFLOW %d by adding %d", pItem->Count(), count);
		}
		return false;
	}
	if (count > 0){
		if (pItem->Count() + count >= 100){	//	限制最多增加 99 件商品
			return false;
		}
		pItem->Increase(count);
	}else{
		pItem->Decrease(-count);
	}
	
	CECShoppingCartChange change(CECShoppingCartChange::ITEM_COUNT_CHANGE, pItem, count);
	NotifyObservers(&change);
	return true;
}

bool CECShoppingCart::AddItem(int itemIndex, int buyIndex, int count){
	return AddItemImpl(itemIndex, buyIndex, count, false);
}
bool CECShoppingCart::IncreaseItemCount(ItemShoppingCartID id, int count){
	return IncreaseItemCountImpl(id, count, false);
}
bool CECShoppingCart::DecreaseItemCount(ItemShoppingCartID id, int count){
	return DecreaseItemCountImpl(id, count, false);
}
bool CECShoppingCart::SetItemCount(ItemShoppingCartID id, int count){
	return SetItemCountImpl(id, count, false);
}
bool CECShoppingCart::RemoveItem(ItemShoppingCartID id){
	return RemoveItemImpl(id, false);
}