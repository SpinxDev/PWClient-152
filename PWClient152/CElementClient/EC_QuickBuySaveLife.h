// File		: EC_QuickBuySaveLife.h
// Creator	: Xu Wenbin
// Date		: 2013/12/20

#pragma once

//	保命符快捷购买

#include "EC_QuickBuyPopPolicy.h"
#include "EC_Counter.h"

class CECQuickBuyPopSaveLifeTrigger : public CECQuickBuyPopTriggerPolicy
{
	DWORD	m_dwTimeupTime;	//	上次隐藏时间（CECQuickBuyPopSaveLifeHide 中时间到策略导致）
	DWORD	m_dwCloseTime;	//	玩家手动关闭时间

	bool IsJustDone(DWORD dwTimeStamp, int nSeconds)const;
	bool IsJustRevived()const;
	bool IsJustTimeup()const;
	bool IsJustClosed()const;
protected:
	virtual bool CanTriggerCheckMore()const;
	virtual const char *GetConfigName()const;
	virtual void OnHideMore();
	virtual bool CanHideMore(const char *&szHideReason)const;

public:
	CECQuickBuyPopSaveLifeTrigger(CECQuickBuyPopBuyItemPolicy *pBuyItemPolicy);
	virtual bool NeedCheckHide()const;
};

class CECQuickBuyPopSaveLifeHide : public CECQuickBuyPopHidePolicy
{
	CECCounter	m_disappearCnt;
public:
	CECQuickBuyPopSaveLifeHide(DWORD disappearTimeLength);
	
	//	从父类继承
	virtual void Restart();
	virtual void TickOnShow(AUIDialog *pDlg);
	virtual bool CanHide(const char *&szHideReason);
};

class CECQuickBuyPopSaveLifeAlign : public CECQuickBuyPopAlignPolicy
{
public:
	virtual ~CECQuickBuyPopSaveLifeAlign(){}	
	virtual void OnShow(AUIDialog *pDlg);
};

namespace QuickBuyPopPolicy
{
	CECQuickBuyPopPolicies	CreateSaveLifePolicy(CECQuickBuyPopSaveLifeTrigger *&pTriggerPolicy);
}