// File		: DlgQuickBuyPop.cpp
// Creator	: Xu Wenbin
// Date		: 2013/12/18

#include "DlgQuickBuyPop.h"

#include <AUICheckBox.h>

//	CDlgQuickBuyPop

AUI_BEGIN_COMMAND_MAP(CDlgQuickBuyPop, CDlgQuickBuyBase)
AUI_ON_COMMAND("Chk_Hide",		OnCommand_Chk_Hide)
AUI_ON_COMMAND("IDCANCEL",		OnCommand_Cancel)
AUI_ON_COMMAND("Btn_Close",		OnCommand_Cancel)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgQuickBuyPop, CDlgQuickBuyBase)
AUI_END_EVENT_MAP()

CDlgQuickBuyPop::CDlgQuickBuyPop()
: m_pChk_Hide(NULL)
{
}

bool CDlgQuickBuyPop::OnInitDialog()
{
	m_pChk_Hide = dynamic_cast<AUICheckBox *>(GetDlgItem("Chk_Hide"));	//	允许为空
	return CDlgQuickBuyBase::OnInitDialog();
}

void CDlgQuickBuyPop::OnTick()
{
	CDlgQuickBuyBase::OnTick();
	if (CECQuickBuyPopPolicies *policy = GetPolicy()){
		policy->TickOnShow(this);
	}
}

void CDlgQuickBuyPop::OnHideDialog()
{
	if (CECQuickBuyPopPolicies *policy = GetPolicy()){
		policy->OnHide();
	}
	if (m_pChk_Hide){
		m_pChk_Hide->Check(false);
	}
	CDlgQuickBuyBase::OnHideDialog();
}

bool CDlgQuickBuyPop::HasCheckHide()
{
	return m_pChk_Hide != NULL;
}

void CDlgQuickBuyPop::OnCommand_Chk_Hide(const char * szCommand)
{
	if (CECQuickBuyPopPolicies *policy = GetPolicy()){
		policy->OnCheckHide();
	}
}

void CDlgQuickBuyPop::OnCommand_Cancel(const char * szCommand)
{
	if (CECQuickBuyPopPolicies *policy = GetPolicy()){
		policy->SetHideReason("HR_UserClose");
	}
	Show(false);
}

CECShopBase * CDlgQuickBuyPop::GetShopData()
{
	if (CECQuickBuyPopPolicies *policy = GetPolicy()){
		return policy->GetShopData();
	}
	return NULL;
}

CECShopSearchPolicyBase * CDlgQuickBuyPop::GetShopSearchPolicy(int itemID)
{
	if (CECQuickBuyPopPolicies *policy = GetPolicy()){
		return policy->GetShopSearchPolicy(itemID);
	}
	return NULL;
}

void CDlgQuickBuyPop::OnItemAppearInPack(int itemID, int iSlot)
{
	if (CECQuickBuyPopPolicies *policy = GetPolicy()){
		policy->OnItemAppearInPack(itemID, iSlot);
	}
}

void CDlgQuickBuyPop::BindPolicy(const CECQuickBuyPopPolicies &policies)
{
	//	设置有效策略时自动显示、设置无效策略（清除策略）时自动隐藏
	if (IsShow()){
		if (CECQuickBuyPopPolicies *policy = GetPolicy()){
			policy->OnHide();
		}
	}
	m_policies = policies;
	if (CECQuickBuyPopPolicies *policy = GetPolicy()){
		int recommendMessage(-1), recommendItem(0);
		bool needConfirm(true), needFlash(false);
		if (policy->GetItem(recommendMessage, recommendItem, needConfirm, needFlash) &&
			QuickBuyAppear(GetStringFromTable(recommendMessage), recommendItem, needConfirm, needFlash)){
			policy->OnShow(this);
			if (!IsShow()){
				Show(true);
			}
			if (m_pChk_Hide){
				m_pChk_Hide->Check(false);
			}
		}else{
			//	隐藏显示、并通过 OnHide 解除与 Policy 绑定
			policy->OnNoItem();
			if (IsShow()){
				Show(false);
			}else{
				policy->OnHide();
			}
		}
	}else{
		if (IsShow()){
			Show(false);
		}
	}
}

CECQuickBuyPopPolicies * CDlgQuickBuyPop::GetPolicy()
{
	return m_policies.IsValid() ? &m_policies : NULL;
}
