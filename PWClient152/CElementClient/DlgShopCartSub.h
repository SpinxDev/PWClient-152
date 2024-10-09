// Filename	: DlgShopCartSub.h
// Creator	: Xu Wenbin
// Date		: 2014/5/19

#ifndef _ELEMENTCLIENT_DLGSHOPCARTSUB_H_
#define _ELEMENTCLIENT_DLGSHOPCARTSUB_H_

#include "DlgBase.h"
#include "EC_TimeSafeChecker.h"

class CECShoppingCart;
class CECShoppingCartItem;
typedef int	ItemShoppingCartID;

struct _GSHOP_ITEM;
typedef struct _GSHOP_ITEM GSHOP_ITEM;

class AUIImagePicture;

class CDlgShopCartSub : public CDlgBase
{
	AUI_DECLARE_EVENT_MAP();
	AUI_DECLARE_COMMAND_MAP()
	
public:
	CDlgShopCartSub();

	void SetItem(CECShoppingCart *pShoppingCart, ItemShoppingCartID itemCartID);
	ItemShoppingCartID ItemCartID()const;
	
	void UpdateContent();
	void UpdateLock();
	void OnCountChange();
	
	void OnCommand_Add(const char *szCommand);
	void OnCommand_Sub(const char *szCommand);
	void OnCommand_Edt_Count(const char *szCommand);
	void OnCommand_Delete(const char *szCommand);
	void OnCommand_CANCEL(const char *szCommand);

	void OnEventLButtonDown_Add(WPARAM wParam, LPARAM lParam, PAUIOBJECT pObj);
	void OnEventLButtonDown_Sub(WPARAM wParam, LPARAM lParam, PAUIOBJECT pObj);
	void OnEventKillFocus_Edt_Count(WPARAM wParam, LPARAM lParam, PAUIOBJECT pObj);
	void OnEventMouseWheel(WPARAM wParam, LPARAM lParam, PAUIOBJECT pObj);
	void OnEventKeyDown_Edt_Count(WPARAM wParam, LPARAM lParam, PAUIOBJECT pObj);

protected:
	virtual bool OnInitDialog();
	virtual void OnTick();

	void UpdateContentImpl(const CECShoppingCartItem *pShoppingCartItem = NULL);
	void UpdateLockImpl(const CECShoppingCartItem *pShoppingCartItem = NULL);
	void UpdateCountImpl(const CECShoppingCartItem *pShoppingCartItem = NULL);
	void UpdatePriceImpl(const CECShoppingCartItem *pShoppingCartItem = NULL);

	const CECShoppingCart * ShoppingCart()const;
	const CECShoppingCartItem *CartItem()const;

	CECShoppingCart *	m_pShoppingCart;
	ItemShoppingCartID	m_itemCartID;

	AUIImagePicture	*	m_pImg_Goods;
	AUIImagePicture	*	m_pImg_Gift;
	PAUIOBJECT			m_pTxt_Name;
	PAUIOBJECT			m_pEdt_Count;
	PAUIOBJECT			m_pTxt_Price;
	PAUIOBJECT			m_pBtn_Add;
	PAUIOBJECT			m_pBtn_Sub;
	PAUIOBJECT			m_pBtn_Delete;

	enum
	{
		INTERVAL_WAIT_BEGIN = 618,
		INTERVAL_WAIT_NEXT = 100,
	};
	CECTimeSafeChecker	m_numberTrigger;	//	自动加/减点倒计时
};

#endif	//	_ELEMENTCLIENT_DLGSHOPCARTSUB_H_