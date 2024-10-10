// File		: EC_QuickBuySaveLife.h
// Creator	: Xu Wenbin
// Date		: 2013/12/20

#pragma once

//	��������ݹ���

#include "EC_QuickBuyPopPolicy.h"
#include "EC_Counter.h"

class CECQuickBuyPopSaveLifeTrigger : public CECQuickBuyPopTriggerPolicy
{
	DWORD	m_dwTimeupTime;	//	�ϴ�����ʱ�䣨CECQuickBuyPopSaveLifeHide ��ʱ�䵽���Ե��£�
	DWORD	m_dwCloseTime;	//	����ֶ��ر�ʱ��

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
	
	//	�Ӹ���̳�
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