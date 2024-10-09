// Filename	: DlgShopCart.h
// Creator	: Xu Wenbin
// Date		: 2014/5/20

#ifndef _ELEMENTCLIENT_DLGSHOPCART_H_
#define _ELEMENTCLIENT_DLGSHOPCART_H_

#include "DlgBase.h"
#include "EC_Observer.h"
#include "DlgBuyConfirm.h"

#include <AUISubDialog.h>

class CDlgShopCartSubList;

class CECShoppingCart;
typedef CECObserver<CECShoppingCart>	CECShoppingCartObserver;

class CECShoppingController;
typedef CECObserver<CECShoppingController>	CECShoppingControllerObserver;

//	class CDlgShopCart
class CDlgShopCart : public CDlgBase, public CECShoppingCartObserver, public CECShoppingControllerObserver
{
	AUI_DECLARE_COMMAND_MAP()
	
public:
	CDlgShopCart();
	
	void OnCommand_Buy(const char *szCommand);
	void OnCommand_CANCEL(const char *szCommand);

	bool SetShoppingCart(CECShoppingCart *pShoppingCart);
	bool IsBuying();

	//	派生自 CECShoppingCartObserver
	virtual void OnRegistered(const CECShoppingCart *p);
	virtual void OnModelChange(const CECShoppingCart *p, const CECObservableChange *q);
	virtual void OnUnregister(const CECShoppingCart *p);

	//	派生自	CECShoppingControllerObserver
	virtual void OnModelChange(const CECShoppingController *p, const CECObservableChange *q);
	
protected:
	virtual bool OnInitDialog();
	virtual bool OnChangeLayoutBegin();
	virtual void OnChangeLayoutEnd(bool bAllDone);
	virtual bool Release();

	void RefreshPrice();
	void RefreshBuyButton();

	bool CanClickBuy();
	bool CanClickStopBuy();

	void ShowStopReason(int reason, int buyedCount, int inbuyCount, int tobuyCount);

	CECShoppingController * ShoppingController();
	
private:
	CECShoppingCart		*	m_pShoppingCart;		//	购物车
	CDlgShopCartSubList *	m_pDlgCartSubList;		//	购物车显示
	CECShoppingCart		*	m_pShoppingCartBeforeChangeLayout;	//	更换界面前备份

	A3DCOLOR				m_priceDefaultColor;
	PAUIOBJECT				m_pTxt_TotalPrice;
	PAUIOBJECT				m_pBtn_Buy;

	bool					m_inRelease;			//	当前正在释放中
};

#endif	//	_ELEMENTCLIENT_DLGSHOPCART_H_