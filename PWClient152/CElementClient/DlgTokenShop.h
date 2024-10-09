// Filename	: DlgTokenShop.h
// Creator	: Han Guanghui
// Date		: 2013/10/05

#pragma once

#include "DlgBase.h"
#include <vector>
#include "AUIScroll.h"

class ATaskTempl;
class CECIvtrItem;
class TokenShopItemBase
{
public:
	TokenShopItemBase(){}
	virtual ~TokenShopItemBase(){};

	enum ShopItemType
	{
		ITEM_BY_TASK,
	};
	virtual void			Init(int id){};
	virtual CECIvtrItem*	GetItem(){return NULL;}
	virtual CECIvtrItem*	GetExchangeItem(){return NULL;}
	virtual ACString		GetItemNumber(){return ACString(_AL(""));}
	virtual ACString		GetItemPrice(){return ACString(_AL(""));}
	virtual ACString		GetItemLeft(){return ACString(_AL(""));}
	virtual bool			BuyItem(){return true;}
	virtual bool			CanBuyItem(){return false;}

	static TokenShopItemBase* CreateOneShopItem(int item_id, ShopItemType type);
};

class TokenShopItemByTask : public TokenShopItemBase
{
public:
	TokenShopItemByTask();
	~TokenShopItemByTask();

	void					Init(int task_id);
	CECIvtrItem*			GetItem();
	CECIvtrItem*			GetExchangeItem();
	ACString				GetItemNumber();
	ACString				GetItemPrice();
	ACString				GetItemLeft();
	bool					BuyItem();
	bool					CanBuyItem();

protected:
	ATaskTempl*			m_pTaskTempl;
};
class CDlgTokenShop : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP();
	AUI_DECLARE_EVENT_MAP()
public:
	CDlgTokenShop();
	virtual ~CDlgTokenShop();

	void					OnCommandConfirm(const char* szCommand);
	void					OnCommandCancel(const char* szCommand);

	void					OnMouseWheel(WPARAM wParam, LPARAM lParam, AUIObject *pObj);	
	void					InitTokenShopItem(int token_id);

protected:
	enum {ITEM_PER_LINE = 3};
	virtual bool			OnInitDialog();
	virtual void			OnTick();

	int						Update();
	bool					UpdateOneItem(TokenShopItemBase* pShopItem, int index);
	void					ScrollPage(bool bUp, int count);
	void					ClearTokenShopItem();
	TokenShopItemBase*		GetOneItem(int index);

	typedef std::vector<TokenShopItemBase*> ShopItemContainer;

	ShopItemContainer		m_ShopItems;
	PAUISCROLL				m_pScl_Right;
};
