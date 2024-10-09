// File		: EC_QuickBuyActivity.h
// Creator	: Xu Wenbin
// Date		: 2013/12/20

#pragma once

//	��������ݹ���

#include "EC_QuickBuyPopPolicy.h"
#include "EC_Counter.h"

class CECQuickBuyPopActivityTrigger : public CECQuickBuyPopTriggerPolicy
{
	int		m_activityID;			//	�����ID������ֱ��ǰ��
protected:
	virtual bool CanTriggerCheckMore()const;
	virtual const char *GetConfigName()const;
	virtual void OnItemAppearInPackMore();
	virtual bool CanHideMore(const char *&szHideReason)const;
	virtual void OnHideMore();

public:
	CECQuickBuyPopActivityTrigger(CECQuickBuyPopBuyItemPolicy *pBuyItemPolicy);
	virtual bool NeedCheckHide()const;

	void SetActivityID(int id);
};

class CECQuickBuyPopActivityHide : public CECQuickBuyPopHidePolicy
{
public:	
	//	�Ӹ���̳�
	virtual void Restart(){}
	virtual void TickOnShow(AUIDialog *pDlg){}
	virtual bool CanHide(const char *&szHideReason);
};

class CECQuickBuyPopActivityAlign : public CECQuickBuyPopAlignPolicy
{
public:
	virtual ~CECQuickBuyPopActivityAlign(){}	
	virtual void OnShow(AUIDialog *pDlg);
};

namespace QuickBuyPopPolicy
{
	CECQuickBuyPopPolicies	CreateActivityPolicy(CECQuickBuyPopActivityTrigger *&pTriggerPolicy);
}