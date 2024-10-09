// Filename	: DlgQShopBuy.h
// Creator	: Han Guanghui
// Date		: 2013/11/04

#pragma once

#include "DlgBase.h"

class CECShopBase;
class CDlgQShopBuy : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP();
	AUI_DECLARE_EVENT_MAP()
public:
	CDlgQShopBuy();
	virtual ~CDlgQShopBuy();

	void					OnCommandConfirm(const char* szCommand);
	void					OnCommandCancel(const char* szCommand);
	void					OnCommandMaximum(const char* szCommand);
	void					OnCommandAdd(const char* szCommand);
	void					OnCommandMinus(const char* szCommand);

	void					OnEventMouseWheel(WPARAM wParam, LPARAM lParam, AUIObject *pObj);

	void					SetBuyingItem(int itemindex, int buyindex, CECShopBase *pShop);
	void					BatchBuy();

protected:
	virtual bool			OnInitDialog();
	virtual void			OnTick();

	CECShopBase*			Shop();
	void					SearchForSale();
	int						GetMaximumNumCanBuy();
	int						CalculateMoneyNeeded();
	bool					CalcuateBuyItemCount();
	void					SetTotalMoneyNeededText();
	int						GetInputBuyCount();
	void					SetInputBuyCount(int count);
	void					SetBtnBuyEnable(bool hint = true);
	virtual	void			OnChangeLayoutEnd(bool bAllDone);
	void					Clear();
	bool					CheckBuyedItem();

	CECShopBase	*			m_pShop;
	int						m_ItemIndex;
	int						m_BuyIndex;
	int						m_SaleItemIndex;
	int						m_SaleBuyIndex;
	bool					m_bBuying;
	int						m_BuyItemCount;
	int						m_BuySaleItemCount;
	PAUIOBJECT				m_TxtInputBuyCount;
	int						m_MaxItemCanBuy;
};
