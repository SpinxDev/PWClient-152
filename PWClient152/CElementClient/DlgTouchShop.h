// Filename	: DlgTouchShop.h
// Creator	: Han Guanghui
// Date		: 2013/3/30

#ifndef _DLGTOUCHSHOP_H_ 
#define _DLGTOUCHSHOP_H_

#include "DlgBase.h"
#include <vector>

class CDlgTouchShop : public CDlgBase
{
	AUI_DECLARE_COMMAND_MAP()
public:
	CDlgTouchShop();
	virtual ~CDlgTouchShop();

	void				OnCommandBuy(const char* szCommand);
	void				OnCommandEnterTouch(const char* szCommand);
	void				OnCommandPagePrevious(const char* szCommand);
	void				OnCommandPageNext(const char* szCommand);
	void				OnCommandMainType(const char* szCommand);

	void				OnQueryTouchPointRe(__int64 income, __int64 remain, int retcode);
	void				OnSpendTouchPointRe(__int64 income, __int64 remain, unsigned int cost, unsigned int index, unsigned int lots, int retcode);
	void				OnTotalRecharge(__int64 recharge);
	void				OnBuyShopItem(int lots);

protected:
	struct ShopItemParam
	{
		unsigned int id;
		unsigned int num;
		unsigned int price;
		int expire_timelength;
	};
	static void			GetShopItemParam(int page, int index, ShopItemParam& param, unsigned short*& shop_title);

	virtual bool		OnInitDialog();
	virtual void		OnShowDialog();

	void				UpdateItemDisplay();
	void				UpdatePage();
	void				SetOneItem(int index, const ShopItemParam& param);
	void				QueryTouchPoint();
	void				BuyItemUsingTouchPoint(int page, int index, unsigned int id, unsigned int num,	unsigned int price, int expire_time, unsigned int lots = 1);
	void				SetCurrentPointText(__int64 remain);
	bool				IsBuyingItem(int page, int index);
	int					GetRMoneyByRecharge(__int64 recharge);

	std::vector<char>	m_PageIndex;			  // 第n页第一个物品在配置表中的下标
	int					m_nPageItemCount;		  // 界面中共有几个位置可以显示物品
	int					m_nCurrentPage;
	PAUIOBJECT			m_pLabCurrentPoint;
	std::vector<int>	m_BuyingItemIndex;		  // 已发送购买协议并等待服务器返回的物品的index，用于按钮的enable
	int					m_iConfirmBuyItemIndex;
	PAUIOBJECT			m_pObjBuyButton;
};

#endif // _DLGTOUCHSHOP_H_

