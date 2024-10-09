/*
 * FILE: DlgOfflineBooth.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: 
 *
 * HISTORY: 
 *
 * Copyright (c) 2013, All Rights Reserved.
 */
 
#pragma once

#include "DlgBase.h"

class CDlgOfflineShopSelfSetting : public CDlgBase
{
	AUI_DECLARE_EVENT_MAP();
	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgOfflineShopSelfSetting();
	~CDlgOfflineShopSelfSetting();

	void OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver);
	void OnEventLButtonUp_SellItem(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventLButtonUp_BuyItem(WPARAM wParam, LPARAM lParam, AUIObject *pObj);

	void OnCommand_ResetItems(const char* szCommand);
	void OnCommand_Store(const char* szCommand);
	void OnCommandCANCEL(const char *szCommand);
	void OnCommand_Type(const char *szCommand);

protected:
	void OnTick();
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	void UpdateShop();

protected:

		
};

