// Filename	: DlgBooth.h
// Creator	: Tom Zhou
// Date		: October 11, 2005

#pragma once

#include "DlgBase.h"
#include "Network/EC_GPDataType.h"
#include "AUITextArea.h"
#include "AUILabel.h"
#include "AUIEditBox.h"

class A2DSprite;
class CECDealInventory;
class CDlgBooth : public CDlgBase  
{
public:
	CDlgBooth();
	virtual ~CDlgBooth();

protected:
	__int64 m_nMoney1, m_nMoney2;
	
	virtual bool OnInitDialog();

	virtual CECDealInventory * GetBuyPack() = 0;
	virtual CECDealInventory * GetSellPack() = 0;

	bool UpdateBooth();
};

// PShop1
class CDlgBoothSelf : public CDlgBooth
{
	AUI_DECLARE_EVENT_MAP();
	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgBoothSelf();
	virtual ~CDlgBoothSelf();
	virtual bool Tick(void);

	void OnCommand_reset(const char * szCommand);
	void OnCommand_confirm(const char * szCommand);
	void OnCommand_clearmsg(const char * szCommand);
	void OnCommand_CANCEL(const char * szCommand);
	void OnCommand_Setting(const char * szCommand);

	void OnEventMouseMove_MsgList(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventLButtonDown_MsgList(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventLButtonUp_SellItem(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventLButtonUp_BuyItem(WPARAM wParam, LPARAM lParam, AUIObject *pObj);

	void AddBoothMessage(const ACHAR *pszMsg);

	virtual void OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver);

protected:
	A2DSprite *m_pA2DSpriteForbid;

	virtual bool OnInitDialog();

	virtual CECDealInventory * GetBuyPack();
	virtual CECDealInventory * GetSellPack();

	bool UpdateBoothSelf();
	bool GetNameLinkMouseOn(int x, int y,PAUIOBJECT pObj, P_AUITEXTAREA_NAME_LINK pLink);
};

// PShop2
class CDlgBoothElse : public CDlgBooth
{
	AUI_DECLARE_EVENT_MAP();
	AUI_DECLARE_COMMAND_MAP()

public:
	virtual bool Tick(void);

	void OnCommand_buy(const char * szCommand);
	void OnCommand_sell(const char * szCommand);
	void OnCommand_sendmsg(const char * szCommand);
	void OnCommand_try(const char * szCommand);
	void OnCommand_CANCEL(const char * szCommand);

	void OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventRButtonDown(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventLButtonDblclk_SellItem(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventLButtonDblclk_Sell(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventLButtonDblclk_Buy(WPARAM wParam, LPARAM lParam, AUIObject *pObj);

	virtual void OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver);

protected:
	unsigned int m_nBuyWorth, m_nSellWorth;

	bool UpdateBoothElse();
	void UpdatePriceRemind();
	void UpdateSameItemMark();
	virtual CECDealInventory * GetBuyPack();
	virtual CECDealInventory * GetSellPack();
};

class CDlgInputNO2 : public CDlgBase 
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()
public:
	void OnCommand_max(const char * szCommand);
	void OnCommand_start(const char * szCommand);
	void OnCommand_confirm(const char * szCommand);
	void OnEventChar_Txt(WPARAM wParam, LPARAM lParam, AUIObject *pObj);

protected:
	virtual void OnHideDialog();
};

// pshopset
class CDlgBoothSet : public CDlgBase
{
	enum
	{
		MONEY_EMPTY = 0,
		MONEY_FULL = 200000000,
		MONEY_PER_CHECK = 10000000,

		MIN_MONEY_COUNT = 10000000,
		MAX_MONEY_COUNT = 190000000,
	};

	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgBoothSet();

	void OnCommand_Confirm(const char * szCommand);

protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnTick();

	PAUILABEL m_pLbl_BagMoney;
	PAUIOBJECT m_pTxt_MoneyMin;
	PAUIOBJECT m_pTxt_MoneyMax;
};