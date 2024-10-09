// File		: EC_QuickBuyActivity.h
// Creator	: Xu Wenbin
// Date		: 2013/12/20

#pragma once

//	保命符快捷购买

#include "EC_QuickBuyPopPolicy.h"
#include "EC_Counter.h"

class CECQuickBuyPopActivityTrigger : public CECQuickBuyPopTriggerPolicy
{
	int		m_activityID;			//	活动配置ID，用于直接前往
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
	//	从父类继承
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