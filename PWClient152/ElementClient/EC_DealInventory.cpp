/*
 * FILE: EC_DealInventory.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/12/22
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "EC_Global.h"
#include "EC_DealInventory.h"
#include "EC_IvtrItem.h"
#include "EC_IvtrTypes.h"
#include "EC_RTDebug.h"
#include "EC_Game.h"

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
//	Implement CECDealInventory
//	
///////////////////////////////////////////////////////////////////////////

CECDealInventory::CECDealInventory()
{
	m_iMoney = 0;
}

CECDealInventory::~CECDealInventory()
{
}

//	Release object
void CECDealInventory::Release()
{
	for (int i=0; i < m_aItems.GetSize(); i++)
	{
		if (m_aItems[i] && m_aItemInfo[i].bDelete)
			delete m_aItems[i];
	}

	m_aItems.RemoveAll();
}

//	Resize inventory
void CECDealInventory::Resize(int iNewSize)
{
	int iOldSize = m_aItemInfo.GetSize();
	if(iNewSize < iOldSize)
	{
		for(int i=iNewSize;i<iOldSize;i++)
		{
			if (m_aItems[i] && m_aItemInfo[i].bDelete)
				delete m_aItems[i];	
		}
	}
	
	CECInventory::Resize(iNewSize);

	m_aItemInfo.SetSize(iNewSize, 10);

	if(iOldSize < iNewSize)
	{
		//	Clear new slots
		for (int i=iOldSize; i < iNewSize; i++)
		{
			m_aItemInfo[i].iOrigin	= 0;
			m_aItemInfo[i].iAmount	= 0;
			m_aItemInfo[i].bDelete	= false;
			m_aItemInfo[i].iFlag	= 0;
		}
	}
}

//	Add an item. This function is used when trade with other players
bool CECDealInventory::AddItem(CECIvtrItem* pItem, int iOrigin, int iAmount,
						bool bDelete/* false */)
{
	int i, iEmpty = -1;

	//	Check whether there is an item came from the same original posiiton
	for (i=0; i < m_aItems.GetSize(); i++)
	{
		if (!m_aItems[i])
		{
			if (iEmpty < 0)
				iEmpty = i;
		}
		else if (m_aItemInfo[i].iOrigin == iOrigin)
		{
			if (bDelete)
				delete pItem;

			m_aItemInfo[i].iAmount += iAmount;
			return true;
		}
	}

	//	Add to the first empty slot
	if (iEmpty < 0)
	{
		g_pGame->RuntimeDebugInfo(RTDCOL_WARNING, _AL("Deal package is full"));
		return false;
	}

	m_aItems[iEmpty] = pItem;
	m_aItemInfo[iEmpty].iOrigin = iOrigin;
	m_aItemInfo[iEmpty].iAmount = iAmount;
	m_aItemInfo[iEmpty].bDelete = bDelete;
	m_aItemInfo[iEmpty].iFlag	= 0;

	return true;
}

//	Add an item freely. This function is used when trade with NPCs
//	Return the number of items which cannot be added to package (package is full)
int CECDealInventory::AddItemFree(CECIvtrItem* pItem, int iOrigin, int iAmount)
{
	int iRemain = iAmount;

	for (int i=0; i < m_aItems.GetSize(); i++)
	{
		if (!m_aItems[i])
		{
			int iSlotNum = iRemain;
			if (iSlotNum > pItem->GetPileLimit())
				iSlotNum = pItem->GetPileLimit();

			m_aItems[i] = pItem;
			m_aItemInfo[i].iOrigin	= iOrigin;
			m_aItemInfo[i].iAmount	= iSlotNum;
			m_aItemInfo[i].bDelete	= false;
			m_aItemInfo[i].iFlag	= 0;

			iRemain -= iSlotNum;
		}
		else if (m_aItemInfo[i].iOrigin == iOrigin &&
			m_aItemInfo[i].iAmount < pItem->GetPileLimit())
		{
			int iSlotNum = iRemain;
			if (iSlotNum > pItem->GetPileLimit() - m_aItemInfo[i].iAmount)
				iSlotNum = pItem->GetPileLimit() - m_aItemInfo[i].iAmount;

			m_aItemInfo[i].iAmount += iSlotNum;

			iRemain -= iSlotNum;
		}

		if (!iRemain)
			break;
	}

	return iRemain;
}

//	Add an booth item. This function is used when open booth
int CECDealInventory::AddBoothItem(CECIvtrItem* pItem, int iOrigin, int iAmount, int iUnitPrice)
{
	int i;

	//	Search for empty slot
	for (i=0; i < m_aItems.GetSize(); i++)
	{
		if (!m_aItems[i])
			break;
	}

	if (i >= m_aItems.GetSize())
		return -1;

	//	Clone item
	CECIvtrItem* pClonedItem = pItem->Clone();
	if (!pClonedItem)
	{
		a_LogOutput(1, "CECDealInventory::AddBoothItem, Failed to clone item");
		return -1;
	}

	m_aItems[i]	= pClonedItem;

	pClonedItem->SetPriceScale(CECIvtrItem::SCALE_BOOTH, 1.0f);
	pClonedItem->SetUnitPrice(iUnitPrice);
	pClonedItem->SetAmount(iAmount);

	m_aItemInfo[i].iOrigin	= iOrigin;
	m_aItemInfo[i].iAmount	= iAmount;
	m_aItemInfo[i].bDelete	= true;
	m_aItemInfo[i].iFlag	= 0;

	return i;
}

void CECDealInventory::UpdateExchangedBoothItem(int iOrigin1, int tid1, int iOrigin2, int tid2)
{
	// 检查可能参考 iOriginSrc 及 iOriginDst 位置的物品
	// 将指向 iOriginSrc 的索引改为索引 iOriginDst
	// 同时将指向 iOriginDst 的索引改为索引 iOriginSrc
	//

	// 同步更新，检查当前指向 iOriginSrc 及 iOriginDst 的索引并更新
	UpdateBoothItem(iOrigin1, tid1);
	UpdateBoothItem(iOrigin2, tid2);
	
	int iIndex1 = GetItemIndex(iOrigin1);
	int iIndex2 = GetItemIndex(iOrigin2);
	if (iIndex1 >= 0)
	{
		ITEMINFO &ii = m_aItemInfo[iIndex1];
		ii.iOrigin = iOrigin2;
	}
	if (iIndex2 >= 0)
	{
		ITEMINFO &ii = m_aItemInfo[iIndex2];
		ii.iOrigin = iOrigin1;
	}
}

void CECDealInventory::UpdateMovedBoothItem(int iOriginSrc, int iOriginDst, int tid, bool bSrcRemoved, bool bDstReferencedByOthers)
{
	// 检查可能参考 iOriginSrc 及 iOriginDst 位置的索引
	// 根据参考位置 iOriginSrc 的变化做相应处理
	// 参数 bSrcRemoved 是指是否所有的 tid 物品都从包裹中的 iOriginSrc 位置移到 iOriginDst 中去了
	// 参数 bDstReferencedByOther 是指 iOriginDst 处的 tid 物品，是否已经被其它包裹索引
	//

	// 同步更新，检查当前指向 iOriginSrc 及 iOriginDst 的索引并更新
	UpdateBoothItem(iOriginSrc, tid);
	UpdateBoothItem(iOriginDst, tid);

	int iIndexSrc = GetItemIndex(iOriginSrc);
	if (iIndexSrc >=0)
	{
		if (bSrcRemoved)
		{
			if (bDstReferencedByOthers || GetItemIndex(iOriginDst)>=0)
			{
				// 为防止多重引用，此处删除之
				RemoveItemByIndex(iIndexSrc, GetItemInfo(iIndexSrc).iAmount);
			}
			else
			{
				// 没有其它包裹对 iOriginDst 进行索引，iOriginSrc 可以重定向到 iOriginDst
				m_aItemInfo[iIndexSrc].iOrigin = iOriginDst;
			}
		}
	}
}

void CECDealInventory::UpdateRemovedBoothItem(int iOrigin)
{
	// 删除可能参考 iOrigin 处的索引
	//
	UpdateBoothItem(iOrigin, -1);
}

void CECDealInventory::UpdateBoothItem(int iOrigin, int tid)
{
	// 检查可能参考的 iOrigin 位置是否仍然是模板ID为 tid 的物品，如果不是则删除
	// 用于摆摊包裹与玩家包裹的一致性更新
	//
	int iIndex = GetItemIndex(iOrigin);
	if (iIndex < 0)
		return;
	if (m_aItems[iIndex]->GetTemplateID() == tid)
		return;

	const ITEMINFO &ii = GetItemInfo(iIndex);
	RemoveItemByIndex(iIndex, ii.iAmount);
}

bool CECDealInventory::ValidateItem(int iOrigin, int tid)
{
	// 验证索引 iOrigin 位置的物品是否存在，及是否具有指定物品ID
	//
	bool valid(false);

	int iIndex = GetItemIndex(iOrigin);
	if (iIndex >= 0 && 
		m_aItems[iIndex]->GetTemplateID() == tid)
		valid = true;

	return valid;
}

//	Set an booth item. This function is used when visite else player's booth
bool CECDealInventory::AddEPBoothItem(int idItem, int iExpireDate, int iOrigin, int iAmount, 
							int iUnitPrice, BYTE* pData, int iDataLen)
{
	int i;

	//	Search an empty slot
	for (i=0; i < m_aItems.GetSize(); i++)
	{
		if (!m_aItems[i])
			break;
	}

	if (i >= m_aItems.GetSize())
		return false;

	CECIvtrItem* pItem = CECIvtrItem::CreateItem(idItem, iExpireDate, iAmount);
	if (!pItem)
		return false;

	if (pData && iDataLen)
		pItem->SetItemInfo(pData, iDataLen);
	else
		((CECIvtrItem*)pItem)->SetItemInfo(NULL, 0);
	
	pItem->SetPriceScale(CECIvtrItem::SCALE_BOOTH, 1.0f);
	pItem->SetUnitPrice(iUnitPrice);
	pItem->SetExpireDate(iExpireDate);
	pItem->SetAmount(iAmount);

	m_aItems[i] = pItem;

	m_aItemInfo[i].iOrigin	= iOrigin;
	m_aItemInfo[i].iAmount	= iAmount;
	m_aItemInfo[i].bDelete	= true;
	m_aItemInfo[i].iFlag	= 0;
	
	return true;
}

//	Remove an item
void CECDealInventory::RemoveItem(int iOrigin, int iAmount)
{
	for (int i=0; i < m_aItems.GetSize(); i++)
	{
		if (m_aItems[i] && m_aItemInfo[i].iOrigin == iOrigin)
		{
			if (iAmount < 0 || iAmount >= m_aItemInfo[i].iAmount)
			{
				if (m_aItemInfo[i].bDelete)
					delete m_aItems[i];

				m_aItems[i] = NULL;
			}
			else
			{
				m_aItemInfo[i].iAmount -= iAmount;

				if (m_aItemInfo[i].bDelete)
					m_aItems[i]->AddAmount(-iAmount);
			}

			return;
		}
	}
}

void CECDealInventory::RemoveItemByFlag(int iFlag, int iAmount)
{
	for (int i=0; i < m_aItems.GetSize(); i++)
	{
		if (m_aItems[i] && m_aItemInfo[i].iFlag == iFlag)
		{
			if (iAmount < 0 || iAmount >= m_aItemInfo[i].iAmount)
			{
				if (m_aItemInfo[i].bDelete)
					delete m_aItems[i];

				m_aItems[i] = NULL;
			}
			else
			{	
				m_aItemInfo[i].iAmount -= iAmount;

				if (m_aItemInfo[i].bDelete)
					m_aItems[i]->AddAmount(-iAmount);
			}

			return;
		}
	}
}

void CECDealInventory::RemoveItemByIndex(int iIndex, int iAmount)
{
	if (iIndex < 0 || iIndex >= m_aItems.GetSize())
	{
		ASSERT(0);
		return;
	}

	if (m_aItems[iIndex])
	{
		if (iAmount < 0 || iAmount >= m_aItemInfo[iIndex].iAmount)
		{
			if (m_aItemInfo[iIndex].bDelete)
				delete m_aItems[iIndex];

			m_aItems[iIndex] = NULL;
		}
		else
		{
			m_aItemInfo[iIndex].iAmount -= iAmount;

			if (m_aItemInfo[iIndex].bDelete)
				m_aItems[iIndex]->AddAmount(-iAmount);
		}
	}
}

//	Get item's index in inventory according to item's original position
int CECDealInventory::GetItemIndex(int iOrigin)
{
	for (int i=0; i < m_aItems.GetSize(); i++)
	{
		if (m_aItems[i] && m_aItemInfo[i].iOrigin == iOrigin)
			return i;
	}

	return -1;
}

//  Get item's count in inventory according to item's original position
int CECDealInventory::GetItemCount(int iOrigin)
{
	int iCount = 0;

	for (int i=0; i < m_aItems.GetSize(); i++)
	{
		if (m_aItems[i] && m_aItemInfo[i].iOrigin == iOrigin)
			iCount += m_aItemInfo[i].iAmount;
	}

	return iCount;
}

//	Get item's index through item's flag
int CECDealInventory::GetItemIndexByFlag(int iFlag)
{
	for (int i=0; i < m_aItems.GetSize(); i++)
	{
		if (m_aItems[i] && m_aItemInfo[i].iFlag == iFlag)
			return i;
	}

	return -1;
}

//	Set flag in item info.
void CECDealInventory::SetItemInfoFlag(int iIndex, int iFlag)
{
	if (iIndex < 0 || iIndex >= m_aItems.GetSize())
	{
		ASSERT(0);
		return;
	}

	m_aItemInfo[iIndex].iFlag = iFlag;
}

//	Remove all items
void CECDealInventory::RemoveAllItems()
{
	for (int i=0; i < m_aItems.GetSize(); i++)
	{
		if (m_aItems[i])
		{
			if (m_aItemInfo[i].bDelete)
				delete m_aItems[i];

			m_aItems[i] = NULL;
		}
	}

	m_iMoney = 0;
}



