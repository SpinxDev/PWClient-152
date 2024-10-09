// Filename	: DlgQuickBuyBase.h
// Creator	: Xu Wenbin
// Date		: 2013/12/10

#pragma once

#include "DlgBase.h"
#include "DlgBuyConfirm.h"

#include <vector.h>

class CECShopBase;
class CECShopSearchPolicyBase;

//	快速购买商城中物品
class CDlgQuickBuyBase : public CDlgBase, public CECShopBuyCommandObserver
{
	AUI_DECLARE_EVENT_MAP()
	AUI_DECLARE_COMMAND_MAP()
	
protected:
	//	子类应调用
	virtual bool OnInitDialog();
	virtual void OnHideDialog();
	virtual void OnTick();

public:
	CDlgQuickBuyBase();

	void OnCommandQuickBuy(const char * szCommand);

	bool QuickBuyAppear(const ACHAR *szMsg, int itemToBuy, bool needConfirm=true, bool needFlash=false);	//	显示信息提示玩家购买
	void QuickBuyDisappear();											//	已经购买、或当前不需要再显示		
	
	//	从父类 CECShopBuyCommandObserver 继承
	virtual void OnBuyConfirmShow(int id, int itemIndex, int buyIndex);	//	确认购买界面显示
	virtual void OnBuyConfirmed(int id, int itemIndex, int buyIndex);	//	物品已经确认购买、发送了购买协议
	virtual void OnBuyCancelled(int id, int itemIndex, int buyIndex);	//	购买确认时玩家点击取消购买

protected:
	//	子类应实现
	virtual CECShopBase * GetShopData()=0;
	virtual CECShopSearchPolicyBase * GetShopSearchPolicy(int itemID)=0;
	virtual void OnItemAppearInPack(int itemID, int iSlot)=0;	//	成功获取物品通知，子类处理

private:
	int			m_itemToBuy;		//	需购买的物品
	bool		m_needConfirm;		//	是否需要弹框确认
	PAUIOBJECT	m_pTxt_QuickBuy;	//	购买提示内容
	PAUIOBJECT	m_pBtn_QuickBuy;	//	购买按钮

	class ConfirmedItem
	{
		int		m_id;
		DWORD	m_confirmTime;		//	确认购买时间，用于限制检查物品的时间
	public:
		ConfirmedItem(int id);
		bool	IsTimeout(DWORD dwCurTickCount)const;
		int		GetID()const{ return m_id; }
	};
	typedef	abase::vector<ConfirmedItem> ConfirmedItems;	//	确认购买的物品列表，用于检查是否购买成功
	ConfirmedItems	m_confirmedItems;
};
