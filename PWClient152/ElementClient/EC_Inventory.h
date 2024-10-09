/*
 * FILE: EC_Inventory.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/11/18
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include "AArray.h"
#include "AAssist.h"
#include "EC_GPDataType.h"
#include <vector>

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////

class CECIvtrItem;
class CECInventoryPutParam;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CECInventory
//	
///////////////////////////////////////////////////////////////////////////

class CECInventory
{
public:		//	Types

public:		//	Constructor and Destructor

	CECInventory();
	virtual ~CECInventory();

public:		//	Attributes

public:		//	Operations

	//	Initalize object
	virtual bool Init(int iSize);
	//	Release object
	virtual void Release();
	//	Remove all items
	virtual void RemoveAllItems();
	//	Resize inventory
	virtual void Resize(int iNewSize);

	//	Put a item into inventory
	CECIvtrItem* PutItem(int iSlot, CECIvtrItem* pItem);
	//	Get a item from inventory
	CECIvtrItem* GetItem(int iSlot, bool bRemove=false);
	//	Set a item into inventory and release old item at this position automatically
	void SetItem(int iSlot, CECIvtrItem* pItem);
	//	Put a item into inventory, merge it with same kind item when necessary
	bool MergeItem(int tid, int iExpireDate, int iAmount, int* piLastSlot, int* piLastAmount);
	//	Exchange item in inventory
	void ExchangeItem(int iSlot1, int iSlot2);
	//	Reset all inventory items
	bool ResetItems(const S2C::cmd_own_ivtr_info& Info);
	bool ResetItems(const S2C::cmd_own_ivtr_detail_info& Info);
	bool ResetWithDetailData(int iIvtrSize, void* pData, int iDataLen);
	//	Move item to another slot which contains the same kind of item
	bool MoveItem(int iSrc, int iDest, int iAmount);
	//	Remove item from invertory
	bool RemoveItem(int iSlot, int iAmount);
	//	Find an item with specified id
	int FindItem(int idItem, int baseIdx = 0);
	//	Get total number of specified item
	int GetItemTotalNum(int idItem);
	//	Search a empty slot
	int SearchEmpty();
	//	Get empty slots number
	int GetEmptySlotNum();
	//	Check whether some item can be put into this inventory or not
	int CanAddItem(int idItem, int iAmount, bool tryPile);

	//	Unfreeze all items
	void UnfreezeAllItems();
	//	Get cost of repairing all items
	int GetRepairAllItemCost();
	//	Repair all items
	void RepairAllItems();
	//	Check whether this inventory has disrepair item
	bool HasDisrepairItem();

	//	Get inventory size
	int GetSize() { return m_aItems.GetSize(); }

	//
	bool FindItemByType(int type);

	bool FindTheVerySameItem(CECIvtrItem* pItem);

	// ����ĳ��Ʒ��������жѵ���ŵĸ���
	int	GetItemCanPileCount(int tid);
	// ÿ��ͬʱ���������Ӳ�ͬ������a����Ʒ������ӵ�������
	int	GetMaxItemNumCanAdd(int* tid, int* num, int count);

	//	�̳Ƕ�����Ʒ�����ɹ����Բ�ѯ
	struct GShopItemBatch{
		int	id;
		int	num;
		int	gift;
		int giftNum;//	���� 4 ���ݶ�Ӧһ�� GSHOP_ITEM
		int	times;	//	��Ӧ����һ�� GSHOP_ITEM ���ظ�����

		GShopItemBatch():id(0), num(0), gift(0), giftNum(0), times(0){}
	};
	typedef abase::vector<GShopItemBatch> GShopItemBatches;
	bool CanBuy(const GShopItemBatches &batches);

	bool FindItems(const int *ids, int num);

protected:	//	Attributes

	APtrArray<CECIvtrItem*>	m_aItems;	//	Item array

protected:	//	Operations

	// ��ȥ����n����λ��ǰ���£��������Ƿ������n����Ʒ
	bool CanHoldItemMulti(const CECInventoryPutParam& param);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECNPCInventory
//	
///////////////////////////////////////////////////////////////////////////

class CECNPCInventory : public CECInventory
{
public:		//	Types

public:		//	Constructor and Destructor

	CECNPCInventory() {}
	virtual ~CECNPCInventory() {}

public:		//	Attributes

public:		//	Operations

	//	Set inventory name
	void SetName(const ACHAR* szName) { m_strName = szName; }
	//	Get inventory name
	const ACHAR* GetName() { return m_strName; }

protected:	//	Attributes

	ACString	m_strName;		//	Inventory name

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECInventoryPutParam
//	
///////////////////////////////////////////////////////////////////////////

class CECInventoryPutParam
{	
public:		//	Constructor and Destructor
	
	CECInventoryPutParam() {}
	virtual ~CECInventoryPutParam() {}
	
public:		//	Attributes
	
public:		//	Operations
	
	struct ItemParam
	{
		int		tid; // ģ��id
		int		num;		   // һ�δ������
		int		put_count;  // ��ż���
		int		pilecanput; // �����пɶѵ���ŵ��������������ֵ��Ϊ�˱����ε���GetItemCanPileCount
		int		pilelimit;  // �ѵ�����

		ItemParam():tid(0), num(0), pilecanput(0), pilelimit(0), put_count(1){}
	};
	typedef std::vector<ItemParam> ITEMPARAMVECTOR;

	void AddParam(int tid, int num, int pilecanput, int pilelimit);
	int GetSize() const { return m_Items.size(); }
	ItemParam* GetItemParam(int index);
	const ItemParam* GetItemParam(int index) const;
	void AddItemPut(int n);
	void SetItemPut(int n);
	
protected:	//	Attributes
	ITEMPARAMVECTOR m_Items;
	
protected:	//	Operations
	
};


