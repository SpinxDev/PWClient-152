// File		: DlgQuickBuyPop.h
// Creator	: Xu Wenbin
// Date		: 2013/12/18

#pragma once

#include "DlgQuickBuyBase.h"
#include "EC_QuickBuyPopPolicy.h"

class AUICheckBox;
class CDlgQuickBuyPop : public CDlgQuickBuyBase
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()

	AUICheckBox *m_pChk_Hide;	//	����֮�ڲ�����ʾ������Ϊ�գ�

	CECQuickBuyPopPolicies	m_policies;

protected:
	virtual bool OnInitDialog();
	virtual void OnTick();
	virtual void OnHideDialog();

public:	
	CDlgQuickBuyPop();
	
	void OnCommand_Chk_Hide(const char * szCommand);
	void OnCommand_Cancel(const char * szCommand);
	bool HasCheckHide();

	//	�̳��Ը���
	virtual CECShopBase * GetShopData();
	virtual CECShopSearchPolicyBase * GetShopSearchPolicy(int itemID);
	virtual void OnItemAppearInPack(int itemID, int iSlot);

	//	���ýӿ�
	void BindPolicy(const CECQuickBuyPopPolicies &policies);
	CECQuickBuyPopPolicies * GetPolicy();
};