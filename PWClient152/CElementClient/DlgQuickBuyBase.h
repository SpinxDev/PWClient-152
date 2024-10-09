// Filename	: DlgQuickBuyBase.h
// Creator	: Xu Wenbin
// Date		: 2013/12/10

#pragma once

#include "DlgBase.h"
#include "DlgBuyConfirm.h"

#include <vector.h>

class CECShopBase;
class CECShopSearchPolicyBase;

//	���ٹ����̳�����Ʒ
class CDlgQuickBuyBase : public CDlgBase, public CECShopBuyCommandObserver
{
	AUI_DECLARE_EVENT_MAP()
	AUI_DECLARE_COMMAND_MAP()
	
protected:
	//	����Ӧ����
	virtual bool OnInitDialog();
	virtual void OnHideDialog();
	virtual void OnTick();

public:
	CDlgQuickBuyBase();

	void OnCommandQuickBuy(const char * szCommand);

	bool QuickBuyAppear(const ACHAR *szMsg, int itemToBuy, bool needConfirm=true, bool needFlash=false);	//	��ʾ��Ϣ��ʾ��ҹ���
	void QuickBuyDisappear();											//	�Ѿ����򡢻�ǰ����Ҫ����ʾ		
	
	//	�Ӹ��� CECShopBuyCommandObserver �̳�
	virtual void OnBuyConfirmShow(int id, int itemIndex, int buyIndex);	//	ȷ�Ϲ��������ʾ
	virtual void OnBuyConfirmed(int id, int itemIndex, int buyIndex);	//	��Ʒ�Ѿ�ȷ�Ϲ��򡢷����˹���Э��
	virtual void OnBuyCancelled(int id, int itemIndex, int buyIndex);	//	����ȷ��ʱ��ҵ��ȡ������

protected:
	//	����Ӧʵ��
	virtual CECShopBase * GetShopData()=0;
	virtual CECShopSearchPolicyBase * GetShopSearchPolicy(int itemID)=0;
	virtual void OnItemAppearInPack(int itemID, int iSlot)=0;	//	�ɹ���ȡ��Ʒ֪ͨ�����ദ��

private:
	int			m_itemToBuy;		//	�蹺�����Ʒ
	bool		m_needConfirm;		//	�Ƿ���Ҫ����ȷ��
	PAUIOBJECT	m_pTxt_QuickBuy;	//	������ʾ����
	PAUIOBJECT	m_pBtn_QuickBuy;	//	����ť

	class ConfirmedItem
	{
		int		m_id;
		DWORD	m_confirmTime;		//	ȷ�Ϲ���ʱ�䣬�������Ƽ����Ʒ��ʱ��
	public:
		ConfirmedItem(int id);
		bool	IsTimeout(DWORD dwCurTickCount)const;
		int		GetID()const{ return m_id; }
	};
	typedef	abase::vector<ConfirmedItem> ConfirmedItems;	//	ȷ�Ϲ������Ʒ�б����ڼ���Ƿ���ɹ�
	ConfirmedItems	m_confirmedItems;
};
