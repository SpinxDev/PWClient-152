/*
 * FILE: EC_Inventory.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/11/18
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "EC_Global.h"
#include "EC_Inventory.h"
#include "EC_IvtrItem.h"
#include "EC_IvtrEquip.h"
#include "EC_Game.h"
#include "EC_RTDebug.h"

#define new A_DEBUG_NEW

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Reference to External variables and functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local Types and Variables and Global variables
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECInventory
//	
///////////////////////////////////////////////////////////////////////////

CECInventory::CECInventory()
{
}

CECInventory::~CECInventory()
{
}

//	Initalize object
bool CECInventory::Init(int iSize)
{
	Resize(iSize);
	return true;
}

//	Release object
void CECInventory::Release()
{
	//	Release all items
	for (int i=0; i < m_aItems.GetSize(); i++)
	{
		if (m_aItems[i])
			delete m_aItems[i];
	}

	m_aItems.RemoveAll();
}

//	Resize inventory
void CECInventory::Resize(int iNewSize)
{
	int iOldSize = m_aItems.GetSize();

	m_aItems.SetSize(iNewSize, 10);

	if(iOldSize < iNewSize)
	{
		//	Clear new slots
		for (int i=iOldSize; i < iNewSize; i++)
			m_aItems[i] = NULL;
	}
}

//	Put a item into inventory
//	Return the old item in specified slot
CECIvtrItem* CECInventory::PutItem(int iSlot, CECIvtrItem* pItem)
{
	if (iSlot < 0 || iSlot >= m_aItems.GetSize())
	{
		ASSERT(0);
		return NULL;
	}

	CECIvtrItem* pOldItem = m_aItems[iSlot];
	m_aItems[iSlot] = pItem;
	return pOldItem;
}

//	Set a item into inventory and release old item at this position automatically
//	If NULL is passed to pItem, this function can be used to delete existing item
void CECInventory::SetItem(int iSlot, CECIvtrItem* pItem)
{
	if (iSlot < 0 || iSlot >= m_aItems.GetSize())
	{
		ASSERT(0);
		return;
	}

	if (m_aItems[iSlot])
	{
	//	g_pGame->RuntimeDebugInfo(RTDCOL_WARNING, _AL("Inventory item was deleted forcedly"));
		delete m_aItems[iSlot];
	}

	m_aItems[iSlot] = pItem;
}

//	Get a item from inventory
CECIvtrItem* CECInventory::GetItem(int iSlot, bool bRemove/* false */)
{
	if (iSlot < 0 || iSlot >= m_aItems.GetSize())
	{
//		ASSERT(0);
		return NULL;
	}

	CECIvtrItem* pItem = m_aItems[iSlot];
	if (bRemove)
		m_aItems[iSlot] = NULL;

	return pItem;
}

//	Exchange item in inventory
void CECInventory::ExchangeItem(int iSlot1, int iSlot2)
{
	if (iSlot1 < 0 || iSlot1 >= m_aItems.GetSize() ||
		iSlot2 < 0 || iSlot2 >= m_aItems.GetSize())
	{
		ASSERT(0);
		return;
	}

	if (iSlot1 != iSlot2)
	{
		CECIvtrItem* pItem = m_aItems[iSlot1];
		m_aItems[iSlot1] = m_aItems[iSlot2];
		m_aItems[iSlot2] = pItem;
	}
}

/*	Put items into inventory, merge it with same kind item when necessary

	Return true if all item have been put into this inventory. Return false
	means all or part amount of item weren't put into.

	tid: item's template id
	iAmount: total amount of item will be put into inventory
	piLastSlot (out): if true is returned, this is index of the last slot 
		item was put into.
	piSlotAmount (out): if true is returned, this is total amount of item in slot 
		after items were merged.
*/
bool CECInventory::MergeItem(int tid, int iExpireDate, int iAmount, int* piLastSlot, int* piSlotAmount)
{
	int iFirstEmpty = -1;

	for (int i=0; i < m_aItems.GetSize(); i++)
	{
		CECIvtrItem* pSlotItem = m_aItems[i];
		if (pSlotItem)
		{
			int iNumMerge = pSlotItem->MergeItem(tid, iAmount);
			iAmount -= iNumMerge;

			if (!iAmount)
			{
				*piLastSlot	  = i;
				*piSlotAmount = pSlotItem->GetCount();
				return true;
			}
		}
		else if (iFirstEmpty < 0)
			iFirstEmpty = i;
	}

	if (iFirstEmpty < 0 || !iAmount)
	{
		ASSERT(iFirstEmpty >= 0 && iAmount);
		return false;
	}

	CECIvtrItem* pItem = CECIvtrItem::CreateItem(tid, iExpireDate, iAmount);
	m_aItems[iFirstEmpty] = pItem;

	*piLastSlot	  = iFirstEmpty;
	*piSlotAmount = iAmount;

	return true;
}

//	Reset all inventory items
bool CECInventory::ResetItems(const S2C::cmd_own_ivtr_info& Info)
{
	//	Release all old items
	RemoveAllItems();

	if (m_aItems.GetSize() != Info.ivtr_size)
		Resize(Info.ivtr_size);

	if (!Info.content_length)
	{
		ASSERT(Info.content_length);
		return false;
	}

	try
	{
		CECDataReader dr((void*) &Info.content, Info.content_length);

		for (int i=0; i < m_aItems.GetSize(); i++)
		{
			int tidItem = dr.Read_int();
			if (tidItem < 0)
			{
				SetItem(i, NULL);
			}
			else
			{
				int expire_date = dr.Read_int();
				int iAmount = dr.Read_int();
				if (iAmount > 0)
				{
					//	TODO: Check if every item needs to be updated
					CECIvtrItem* pItem = CECIvtrItem::CreateItem(tidItem, expire_date, iAmount);
					SetItem(i, pItem);
				}
			}
		}
	}

	catch (CECException& e)
	{
		ASSERT(0);
		a_LogOutput(1, "CECInventory::ResetItems, data read error (%d)", e.GetType());
		return false;
	}

	return true;
}

bool CECInventory::ResetItems(const S2C::cmd_own_ivtr_detail_info& Info)
{
	return ResetWithDetailData(Info.ivtr_size, (void*) &Info.content, Info.content_length);
}

//	Reset inventory with detail item data
bool CECInventory::ResetWithDetailData(int iIvtrSize, void* pData, int iDataLen)
{
	//	Release all old items
	RemoveAllItems();

	if (iIvtrSize > 0)
	{
		if (m_aItems.GetSize() != iIvtrSize)
			Resize(iIvtrSize);
	}

	if (!iDataLen)
	{
		ASSERT(iDataLen);
		return true;
	}

	try
	{
		CECDataReader dr(pData, iDataLen);

		int iNumItem = dr.Read_int();
		for (int i=0; i < iNumItem; i++)
		{
			int iIndex = dr.Read_int();
			if (iIndex < 0)
				continue;

			int tid			= dr.Read_int();
			int expire_date = dr.Read_int();
			int state		= dr.Read_int();
			int iAmount		= dr.Read_int();
			WORD crc		= dr.Read_WORD();
			WORD wDataLen	= dr.Read_WORD();

			BYTE* pData = NULL;
			if (wDataLen)
				pData = (BYTE*)dr.Read_Data(wDataLen);

			CECIvtrItem* pItem = CECIvtrItem::CreateItem(tid, expire_date, iAmount);
			if (pItem)
			{
				pItem->SetProcType(state);

				if (pData)
					pItem->SetItemInfo(pData, (int)wDataLen);
				else
				{
					// clear need update flag
					((CECIvtrItem *)pItem)->SetItemInfo(NULL, 0);
				}

				SetItem(iIndex, pItem);
			}
		}
	}

	catch (CECException& e)
	{
		ASSERT(0);
		a_LogOutput(1, "CECInventory::ResetItemsWithDetailData, data read error (%d)", e.GetType());
		return false;
	}

	return true;
}

//	Move item to another slot which contains the same kind of item
bool CECInventory::MoveItem(int iSrc, int iDest, int iAmount)
{
	if (iSrc < 0 || iSrc >= m_aItems.GetSize() ||
		iDest < 0 || iDest >= m_aItems.GetSize())
	{
		ASSERT(0);
		return false;
	}

	CECIvtrItem* pSrcItem = m_aItems[iSrc];
	CECIvtrItem* pDstItem = m_aItems[iDest];

	if (!pSrcItem)
	{
		ASSERT(pSrcItem);
		return false;
	}

	if (iAmount == 0)
	{
		ASSERT(false);
		return false;
	}

	if (!pDstItem)
	{
	//	pDstItem = CECIvtrItem::CreateItem(pSrcItem->GetTemplateID(), iAmount);
		pDstItem = pSrcItem->Clone();
		pDstItem->SetCount(iAmount);
		SetItem(iDest, pDstItem);
	}
	else
	{
		ASSERT(pSrcItem->GetTemplateID() == pDstItem->GetTemplateID());
		int iNumAdded = pDstItem->MergeItem(pSrcItem->GetTemplateID(), iAmount);
		ASSERT(iNumAdded == iAmount);
	}

	RemoveItem(iSrc, iAmount);

	return true;
}

//	Remove item from invertory
bool CECInventory::RemoveItem(int iSlot, int iAmount)
{
	if (iSlot < 0 || iSlot >= m_aItems.GetSize())
	{
		ASSERT(0);
		return false;
	}

	CECIvtrItem* pItem = m_aItems[iSlot];
	if (!pItem)
		return true;

	pItem->AddAmount(-iAmount);

	if (pItem->GetCount() <= 0)
		SetItem(iSlot, NULL);	//	Remove all items

	return true;
}

//	Remove all items
void CECInventory::RemoveAllItems()
{
	//	Release all items
	for (int i=0; i < m_aItems.GetSize(); i++)
	{
		if (m_aItems[i])
		{
			delete m_aItems[i];
			m_aItems[i] = NULL;
		}
	}
}

//	Unfreeze all items
void CECInventory::UnfreezeAllItems()
{
	//	Release all items
	for (int i=0; i < m_aItems.GetSize(); i++)
	{
		if (m_aItems[i])
			m_aItems[i]->Freeze(false);
	}
}

//	Check whether this inventory has disrepair item
bool CECInventory::HasDisrepairItem()
{
	for (int i=0; i < m_aItems.GetSize(); i++)
	{
		CECIvtrItem* pItem = m_aItems[i];
		if (pItem && pItem->IsEquipment())
		{
			if (((CECIvtrEquip*)pItem)->NeedRepair())
				return true;
		}
	}

	return false;
}

//	Get cost of repairing all items
int CECInventory::GetRepairAllItemCost()
{
	float fCost = 0.0f;

	for (int i=0; i < m_aItems.GetSize(); i++)
	{
		CECIvtrItem* pItem = m_aItems[i];
		if (pItem && pItem->IsEquipment() && pItem->IsRepairable())
			fCost += ((CECIvtrEquip*)pItem)->GetRawRepairCost();
	}

	int iCost = (int)fCost;
	return iCost;
}

//	Repair all items
void CECInventory::RepairAllItems()
{
	for (int i=0; i < m_aItems.GetSize(); i++)
	{
		CECIvtrItem* pItem = m_aItems[i];
		if (pItem && pItem->IsEquipment() && pItem->IsRepairable())
			((CECIvtrEquip*)pItem)->Repair();
	}
}

//	Find an item with specified id
//	Return item's position
int CECInventory::FindItem(int idItem, int baseIdx)
{
	if(baseIdx < 0) baseIdx = 0;

	for (int i= baseIdx; i < m_aItems.GetSize(); i++)
	{
		CECIvtrItem* pItem = m_aItems[i];
		if (pItem && pItem->GetTemplateID() == idItem)
			return i;
	}

	return -1;
}

bool CECInventory::FindTheVerySameItem(CECIvtrItem* pItem)
{
	for (int i= 0; i < m_aItems.GetSize(); i++)
		if(m_aItems[i] == pItem) return true;
	return false;
}

//	Get total number of specified item
int CECInventory::GetItemTotalNum(int idItem)
{
	int iCount = 0;

	for (int i=0; i < m_aItems.GetSize(); i++)
	{
		CECIvtrItem* pItem = m_aItems[i];
		if (pItem && pItem->GetTemplateID() == idItem)
			iCount += pItem->GetCount();
	}

	return iCount;
}

//	Search a empty slot
int CECInventory::SearchEmpty()
{
	for (int i=0; i < m_aItems.GetSize(); i++)
	{
		if (!m_aItems[i])
			return i;
	}

	return -1;
}

//	Check whether some items can be put into this inventory or not
int CECInventory::CanAddItem(int idItem, int iAmount, bool tryPile)
{
	int foundEmpty = -1;
	for (int i=0; i < m_aItems.GetSize(); i++)
	{
		CECIvtrItem* pItem = m_aItems[i];
		if(!pItem)
		{
			// return first empty slot if not trying to pile item
			if(!tryPile) return i;
			if(foundEmpty < 0) foundEmpty = i;
		}
		else if (!pItem->IsFrozen() && pItem->GetTemplateID() == idItem &&  
			     pItem->GetCount() + iAmount <= pItem->GetPileLimit() )
		{
			return i;
		}
	}

	return foundEmpty;
}

//	Get empty slots number
int CECInventory::GetEmptySlotNum()
{
	int i, iCount=0;

	for (i=0; i < m_aItems.GetSize(); i++)
	{
		if (!m_aItems[i])
			iCount++;
	}

	return iCount;
}

bool CECInventory::FindItemByType(int type)
{
	int i = 0;
	for (; i < m_aItems.GetSize(); i++)
	{
		CECIvtrItem* pItem = m_aItems[i];
		if (pItem && pItem->GetClassID() == type)
			return true;
	}
	
	return false;
}

int CECInventory::GetItemCanPileCount(int tid)
{
	int ret(0);
	CECIvtrItem* pItem = NULL;
	int index = FindItem(tid);
	while (index != -1) {
		pItem = GetItem(index);
		if (pItem)
			ret += pItem->GetPileLimit() - pItem->GetCount();
		index = FindItem(tid, index + 1);
	}
	return ret;
}
bool CECInventory::CanHoldItemMulti(const CECInventoryPutParam& param)
{
	int empty_slot = GetEmptySlotNum();
	int min_slot = param.GetSize();
	if (empty_slot >= min_slot) {
		empty_slot -= min_slot;
		int empty_slot_taken(0);
		for (int i = 0; i < min_slot; ++i) {
			const CECInventoryPutParam::ItemParam* pParam = param.GetItemParam(i);
			if (pParam && pParam->pilelimit) {
				int num_put_to_empty = pParam->num * pParam->put_count - pParam->pilecanput;
				if (num_put_to_empty > 0) {
					// 计算需要占用的空位数量
					empty_slot_taken += num_put_to_empty / pParam->pilelimit;
					// 多出来的需要占用一个空位
					if (num_put_to_empty % pParam->pilelimit) empty_slot_taken++;
					if (empty_slot_taken > empty_slot) return false;
				}
			} else return false;
		}
		return empty_slot_taken <= empty_slot;
	} else return false;
}
int CECInventory::GetMaxItemNumCanAdd(int* tid, int* num, int count)
{
	int ret(0);
	if (tid && num && count > 0) {
		CECInventoryPutParam param_left, param_right;
		int empty_slot = GetEmptySlotNum();
		if (empty_slot < count) return 0;
		int min_pileput(0), max_slotput(0), min_slotput(0);
		for (int i = 0; i < count && num[i]; ++i) {
			int pilecanput = GetItemCanPileCount(tid[i]);
			int pileitem = pilecanput / num[i];
			if (i == 0) min_pileput = pileitem;
			else if (pileitem < min_pileput) min_pileput = pileitem;
			CECIvtrItem* pItem = CECIvtrItem::CreateItem(tid[i], 0, 1);
			int pilelimit = pItem->GetPileLimit();
			delete pItem;
			// 一次投放物品的数量不能超过物品的堆叠上限
			if (num[i] > pilelimit) return 0;
			int slotitem = (empty_slot - count) * pilelimit / num[i];
			if (slotitem > max_slotput) max_slotput = slotitem;
			int slotitemsplit = slotitem / count;
			if (i == 0) min_slotput = slotitemsplit;
			else if (slotitemsplit < min_slotput) min_slotput = slotitemsplit;
			param_left.AddParam(tid[i], num[i], pilecanput, pilelimit);
			param_right.AddParam(tid[i], num[i], pilecanput, pilelimit);
		}
		int min_put = min_pileput + min_slotput;
		int max_put = min_put + max_slotput + 1;// +1是考虑pileput和slotput的时候的余数
		param_left.SetItemPut(min_put);
		param_right.SetItemPut(max_put);
		while (true) {
			if (CanHoldItemMulti(param_left)) {
				param_left.AddItemPut(1);
				min_put++;
			} else {
				ret = min_put - 1;
				break;
			}
			if (CanHoldItemMulti(param_right)) {
				ret = max_put;
				break;
			} else {
				max_put--;
				param_right.AddItemPut(-1);
			}
		}
		// 刨掉的n个空位还能放1个
		ret++;
	}
	return ret;
}

//	逐个模拟购买过程，判断给定交易是否能完整进行
bool CECInventory::CanBuy(const GShopItemBatches &batches){
	while (!batches.empty()){
		int i(0);

		//	验证输入并计算辅助数据
		typedef abase::hash_map<int, int> ID2NumHash;
		ID2NumHash pileLimit;		//	堆叠上限查询表
		ID2NumHash canPileCount;	//	当前包裹每种物品剩余可堆叠数查询表
		for (i = 0; i < (int)batches.size(); ++ i){
			const GShopItemBatch &batch = batches[i];

			//	商品
			int id = batch.id;
			if (id <= 0 || batch.num <= 0 || batch.times <= 0){
				//	验证ID、单次购买商品数量、总购买次数
				return false;
			}
			if (pileLimit.find(id) == pileLimit.end()){
				int limit = CECIvtrItem::GetPileLimit(id);
				if (limit <= 0){
					//	验证堆叠上限
					return false;
				}
				pileLimit[id] = limit;
				canPileCount[id] = GetItemCanPileCount(id);
			}

			//	商品赠品
			id = batch.gift;
			if (id > 0){
				if (batch.giftNum <= 0){
					//	验证单次购买赠品数量
					return false;
				}
				if (pileLimit.find(id) == pileLimit.end()){
					int limit = CECIvtrItem::GetPileLimit(id);
					if (limit <= 0){
						//	验证堆叠上限
						return false;
					}
					pileLimit[id] = limit;
					canPileCount[id] = GetItemCanPileCount(id);
				}
			}
		}		

		//	逐批次逐物品购买测试
		int	emptySlot = GetEmptySlotNum();
		for (i = 0; i < (int)batches.size(); ++ i){
			const GShopItemBatch &batch = batches[i];
			int requireEmptySlotEveryTime = batch.gift > 0 ? 2 : 1;

			//	从查询表获取信息，避免频繁访问查询表
			const int currentPileLimit = pileLimit[batch.id];
			int currentCanPileCount = canPileCount[batch.id];
			const int currentGiftPileLimit = batch.gift > 0 ? pileLimit[batch.gift] : 0;
			int currentGiftCanPileCount = batch.gift > 0 ? canPileCount[batch.gift]: 0;

			//	逐次购买测试
			for (int j(0); j < batch.times; ++ j){
				if (requireEmptySlotEveryTime > emptySlot){
					//	服务器每次购买前的特殊要求
					return false;
				}

				//	模拟购买商品
				if (currentCanPileCount >= batch.num){
					currentCanPileCount -= batch.num;
				}else{
					int num = batch.num - currentCanPileCount;
					currentCanPileCount = 0;
					int emptySlotToTake = num / currentPileLimit;
					if (num % currentPileLimit){
						++ emptySlotToTake;
					}
					if (emptySlotToTake > emptySlot){
						return false;
					}
					emptySlot -= emptySlotToTake;
					currentCanPileCount = emptySlotToTake * currentPileLimit - num;
				}

				//	模拟购买商品赠品
				if (batch.gift > 0){
					if (currentGiftCanPileCount >= batch.giftNum){
						currentGiftCanPileCount -= batch.giftNum;
					}else{
						int num = batch.giftNum - currentGiftCanPileCount;
						currentGiftCanPileCount = 0;
						int emptySlotToTake = num / currentGiftPileLimit;
						if (num % currentGiftPileLimit){
							++ emptySlotToTake;
						}
						if (emptySlotToTake > emptySlot){
							return false;
						}
						emptySlot -= emptySlotToTake;
						currentGiftCanPileCount = emptySlotToTake * currentGiftPileLimit - num;
					}
				}
			}

			//	购买成功发生，将剩余的可购买信息记录回查询表
			canPileCount[batch.id] = currentCanPileCount;
			if (batch.gift > 0){
				canPileCount[batch.gift] = currentGiftCanPileCount;
			}
		}
		return true;
	}
	return false;
}

void CECInventoryPutParam::AddParam(int tid, int num, int pilecanput, int pilelimit)
{
	ItemParam to_add;
	to_add.tid = tid;
	to_add.num = num;
	to_add.pilecanput = pilecanput;
	to_add.pilelimit = pilelimit;
	m_Items.push_back(to_add);
}

CECInventoryPutParam::ItemParam* CECInventoryPutParam::GetItemParam(int index)
{
	return const_cast<ItemParam*>(static_cast<const CECInventoryPutParam&>(*this).GetItemParam(index));
}

const CECInventoryPutParam::ItemParam* CECInventoryPutParam::GetItemParam(int index) const
{
	if (index >= 0 && index < GetSize()) return &m_Items[index];
	else return NULL;
}

void CECInventoryPutParam::AddItemPut(int n)
{
	int size = GetSize();
	for (int i = 0; i < size; ++i)
		m_Items[i].put_count += n;
}

void CECInventoryPutParam::SetItemPut(int n)
{
	int size = GetSize();
	for (int i = 0; i < size; ++i)
		m_Items[i].put_count = n;
}

bool CECInventory::FindItems(const int *ids, int num)
{
	bool bFound(false);
	for (int i(0); i < num; ++ i)
	{
		if (FindItem(ids[i]) >= 0){
			bFound = true;
			break;
		}
	}
	return bFound;
}