/*
* FILE: DlgOfflineShopItemsList.h
*
* DESCRIPTION: 
*
* CREATED BY:  WYD
*
* HISTORY: 
*
* Copyright (c) 2013, All Rights Reserved.
*/

#pragma once

#include "DlgOfflineShopList.h"
#include <AUIStillImageButton.h>
#include <AUILabel.h>
#include <AUIImagePicture.h>

class CECGameUIMan;
class CECHostPlayer;
class COfflineShopCtrl;
class CECGameSession;

class CDlgOfflineShopItemsList;

class CUIActionDelegate
{
public:
	static CUIActionDelegate* CreateDelegate(bool bShopItem,CDlgOfflineShopItemsList* pDlg);
	CUIActionDelegate(CDlgOfflineShopItemsList* pDlg);
	virtual ~CUIActionDelegate(){}
	virtual void PushSellBtn() = 0;
	virtual void PushBuyBtn() = 0;
	virtual void PageDown() {};
	virtual void PageUp() {};
	virtual void OnTick() {};
	virtual void UpdateItems() = 0;
	virtual void OnInstallViewer(CECHostPlayer* pHost,CECGameUIMan* pUI,CECGameSession* pSession,bool bSell) ;

	bool IsSellModel() const { return m_bSellOrBuy;}
protected:
	CDlgOfflineShopItemsList* m_pDlg;
	bool m_bSellOrBuy; //true: 寄卖， false：收购

	CECGameUIMan* m_pGameUIMan;
	CECHostPlayer* m_pHostPlayer;
	COfflineShopCtrl* m_pCtrl;
	CECGameSession* m_pGameSession;
};

class CViewShopItem: public CUIActionDelegate
{
public:
	CViewShopItem(CDlgOfflineShopItemsList* pDlg);
	virtual ~CViewShopItem(){};
	void PushSellBtn();
	void PushBuyBtn();
	void UpdateItems();
	void OnInstallViewer(CECHostPlayer* pHost,CECGameUIMan* pUI,CECGameSession* pSession,bool bSell);
protected:

};

class CViewQueryItem: public CUIActionDelegate
{
public:
	CViewQueryItem(CDlgOfflineShopItemsList* pDlg);
	virtual ~CViewQueryItem(){}
	void PushSellBtn();
	void PushBuyBtn();
	void PageDown();
	void PageUp();
	void OnTick();
	void UpdateItems();
	void OnInstallViewer(CECHostPlayer* pHost,CECGameUIMan* pUI,CECGameSession* pSession,bool bSell);
protected:
	bool 					m_bNeedQuery;
	int						m_QueryNum;
	
	DWORD					m_dwLastListTime;
};

//////////////////////////////////////////////////////////////////////////
//
//
//
//////////////////////////////////////////////////////////////////////////

class CDlgOfflineShopItemsList : public CDlgShopSearchBase
{
	friend class CViewShopItem;
	friend class CViewQueryItem;
	friend class CDlgOfflineShopList;

	AUI_DECLARE_EVENT_MAP();
	AUI_DECLARE_COMMAND_MAP()

public:
	enum 
	{
		IT_SHOPITEM = 0,
		IT_QUERYITEM,
	};

public:
	CDlgOfflineShopItemsList();
	~CDlgOfflineShopItemsList();
	
//	void SetShopItemFlag(bool bShop){m_bItemType = bShop;};
	
	void UpdateControls();
	void ClearControls(bool bSell);
	
	void OnCommand_PageUp(const char *szCommand);
	void OnCommand_PageDown(const char *szCommand);
	void OnCommand_SellList(const char* szCommand);
	void OnCommand_BuyList(const char* szCommand);

	void OnCommand_BuyOrSell(const char* szCommand);
	void OnCommand_BackToShop(const char* szCommand);
	
	bool IsSellMode();

	bool BeforeSearchAction();
	void OnCommandCANCEL(const char *szCommand);

	
	void ToggleDlg(bool bToShopDlg); // 物品列表界面和商店界面切换
	void SetItemViewer(int idx,bool bSell);

protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnHideDialog();
	virtual void OnTick();
	
protected:
	PAUISTILLIMAGEBUTTON 	m_pBtn_PageDown;
	PAUISTILLIMAGEBUTTON 	m_pBtn_PageUp;
	PAUISTILLIMAGEBUTTON	m_pBtn_BackToShop;
	
//	PAUILABEL				m_pLbl_RefreshStatus;
	PAUILABEL				m_pLbl_Title;
	
	PAUILABEL				m_pLbl_ItemName[SHOP_COUNT_PERPAGE];
	PAUISTILLIMAGEBUTTON 	m_pBtn_Buy[SHOP_COUNT_PERPAGE];
	PAUILABEL				m_pLbl_Money[SHOP_COUNT_PERPAGE];
	PAUIIMAGEPICTURE		m_pImg_Item[SHOP_COUNT_PERPAGE];	
	
//	bool m_bItemType; // true: items in shop;  false: items of the searching result	
	
	CUIActionDelegate*		m_pItemViewer[2];
	int						m_iCurViewer;// 0: shop, 1: query item
};

