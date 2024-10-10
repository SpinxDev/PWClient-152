/*
 * FILE: CDlgOfflineShopStore.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: WYD
 *
 * HISTORY: 
 *
 * Copyright (c) 2013, All Rights Reserved.
 */
 
#pragma once

#include "DlgBase.h"
#include <AUIEditBox.h>

//////////////////////////////////////////////////////////////////////////
//
//
//
//////////////////////////////////////////////////////////////////////////

class CDlgOfflineShopStore : public CDlgBase
{
	AUI_DECLARE_EVENT_MAP();
	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgOfflineShopStore();
	~CDlgOfflineShopStore();

	void OnEventLButtonUp_SellItem(WPARAM wParam, LPARAM lParam, AUIObject *pObj);

	void OnCommand_GetMoney(const char* szCommand);
	void OnCommand_SaveMoney(const char* szCommand);
	void OnCommand_TakeAllItems(const char* szCommand);
	int TakeItemToPack(int iStart);
	bool IsGetAllItems() const { return m_bGetAllItemsFromStoreToPack;}
	void SetAllItemsFlag(bool bGetAll) {m_bGetAllItemsFromStoreToPack = bGetAll;}
protected:
	virtual void OnTick();
	virtual void UpdateShop();
	virtual bool OnInitDialog();

protected:
	bool m_bGetAllItemsFromStoreToPack; // 全取物品到包裹
		
};

//////////////////////////////////////////////////////////////////////////
