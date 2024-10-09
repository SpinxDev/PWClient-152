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

	AUICheckBox *m_pChk_Hide;	//	今天之内不再提示（允许为空）

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

	//	继承自父类
	virtual CECShopBase * GetShopData();
	virtual CECShopSearchPolicyBase * GetShopSearchPolicy(int itemID);
	virtual void OnItemAppearInPack(int itemID, int iSlot);

	//	复用接口
	void BindPolicy(const CECQuickBuyPopPolicies &policies);
	CECQuickBuyPopPolicies * GetPolicy();
};