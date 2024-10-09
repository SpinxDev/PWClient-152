/*
 * FILE: EC_DealInventory.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/12/22
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include "EC_Inventory.h"

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


///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CECDealInventory
//	
///////////////////////////////////////////////////////////////////////////

class CECDealInventory : public CECInventory
{
public:		//	Types

	//	Item extra information
	struct ITEMINFO
	{
		int		iOrigin;	//	Original position in player's normal pack
		int		iAmount;	//	Amount
		bool	bDelete;	//	true, item object can be released
		int		iFlag;		//	Item flag
	};

public:		//	Constructor and Destructor

	CECDealInventory();
	virtual ~CECDealInventory();

public:		//	Attributes

public:		//	Operations

	//	Release object
	virtual void Release();
	//	Remove all items
	virtual void RemoveAllItems();
	//	Resize inventory
	virtual void Resize(int iNewSize);

	//	Add an item. This function is used when trade with other players
	bool AddItem(CECIvtrItem* pItem, int iOrigin, int iAmount, bool bDelete=false);
	//	Add an item freely. This function is used when trade with NPCs
	int AddItemFree(CECIvtrItem* pItem, int iOrigin, int iAmount);
	//	Add an booth item. This function is used when open booth
	int AddBoothItem(CECIvtrItem* pItem, int iOrigin, int iAmount, int iUnitPrice);
	//  Booth item origin exchanged. This function is used when booth not in use
	void UpdateExchangedBoothItem(int iOrigin1, int tid1, int iOrigin2, int tid2);
	//  Booth item origin moved. This function is used when booth not in use
	void UpdateMovedBoothItem(int iOriginSrc, int iOriginDst, int tid, bool bSrcRemoved, bool bDstReferencedByOthers);
	//  Booth item removed. This function is used when booth not in use
	void UpdateRemovedBoothItem(int iOrigin);
	//	Set an booth item. This function is used when visit else player's booth
	bool AddEPBoothItem(int idItem, int iExpireDate, int iOrigin, int iAmount, int iUnitPrice, BYTE* pData, int iDataLen);
	//	Remove an item
	void RemoveItem(int iOrigin, int iAmount);
	void RemoveItemByFlag(int iFlag, int iAmount);
	void RemoveItemByIndex(int iIndex, int iAmount);
	//	Get item's index in inventory according to item's original position
	int GetItemIndex(int iOrigin);
	//  Get item's count in inventory according to item's original position
	int GetItemCount(int iOrigin);
	//	Get item's index through item's flag
	int GetItemIndexByFlag(int iFlag);
	//	Set flag in item info.
	void SetItemInfoFlag(int iIndex, int iFlag);

	//	Add money
	int AddMoney(int iMoney) { m_iMoney += iMoney; return m_iMoney; }
	//	Get money
	int GetMoney() { return m_iMoney; }
	//	Get item extra information
	const ITEMINFO& GetItemInfo(int n) { return m_aItemInfo[n]; }
	//  Validate item with given origin and tid
	bool ValidateItem(int iOrigin, int tid);

protected:	//	Attributes

	int		m_iMoney;

	AArray<ITEMINFO, ITEMINFO&>	m_aItemInfo;	//	Item extra information

protected:	//	Operations
	void UpdateBoothItem(int iOrigin, int tid);

};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

