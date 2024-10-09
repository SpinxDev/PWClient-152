// File		: EC_QuickBuyActivity.cpp
// Creator	: Xu Wenbin
// Date		: 2013/12/20

#include "EC_QuickBuyActivity.h"
#include "EC_UIConfigs.h"
#include "EC_UIHelper.h"
#include "EC_AutoTeamConfig.h"
#include "EC_GameUIMan.h"
#include "EC_GameSession.h"

#include <AUIDialog.h>
#include <AUIManager.h>
#include <AIniFile.h>

//	CECQuickBuyPopActivityTrigger

CECQuickBuyPopActivityTrigger::CECQuickBuyPopActivityTrigger(CECQuickBuyPopBuyItemPolicy *pBuyItemPolicy)
: CECQuickBuyPopTriggerPolicy(pBuyItemPolicy)
, m_activityID(0)
{
}

bool CECQuickBuyPopActivityTrigger::CanTriggerCheckMore()const
{
	bool bCan(false);
	while (true)
	{
		if (m_activityID <= 0){
			break;
		}
		if (CECAutoTeamConfig::Instance().IsHaveTransmitItem()){
			//	包裹中有其它愿望灵童物品
			break;
		}
		bCan = true;
		break;
	}
	return bCan;
}

bool CECQuickBuyPopActivityTrigger::NeedCheckHide()const
{
	//	当前设置不要求提供“当天不再限制”勾选项
	return false;
}

const char * CECQuickBuyPopActivityTrigger::GetConfigName()const
{
	return "ACTIVITY";
}

void CECQuickBuyPopActivityTrigger::OnItemAppearInPackMore()
{
	if (m_activityID > 0){
		CECUIHelper::GetGameSession()->c2s_CmdAutoTeamJumpToGoal(m_activityID);
	}
}

bool CECQuickBuyPopActivityTrigger::CanHideMore(const char *&szHideReason)const
{
	//	任务通过 SetActivityID(0) 清空时自动隐藏
	if (m_activityID <= 0){
		szHideReason = "HR_NoActivity";
		return true;
	}
	return false;
}

void CECQuickBuyPopActivityTrigger::OnHideMore()
{
	m_activityID = 0;
}

void CECQuickBuyPopActivityTrigger::SetActivityID(int id)
{
	m_activityID = id;
}

//	CECQuickBuyPopActivityHide
bool CECQuickBuyPopActivityHide::CanHide(const char *&szHideReason)
{
	PAUIDIALOG pDlgQuest = CECUIHelper::GetGameUIMan()->GetDialog("Win_Quest");
	if (!pDlgQuest->IsShow()){
		szHideReason = "HR_QuestClosed";
		return true;
	}
	return false;
}

//	CECQuickBuyPopActivityAlign
void CECQuickBuyPopActivityAlign::OnShow(AUIDialog *pDlg)
{
	PAUIDIALOG pDlgQuest = CECUIHelper::GetGameUIMan()->GetDialog("Win_Quest");
	PAUIOBJECT pObj = pDlgQuest->GetDlgItem("Btn_GotoNPC");
	if (pObj){
		//	放置在 Btn_GotoNPC 右侧
		A3DRECT rect = pObj->GetRect();
		pDlg->SetPosEx(rect.right+10, rect.top);
	}
}

//	namespace QuickBuyPopPolicy
namespace QuickBuyPopPolicy
{
	CECQuickBuyPopPolicies	CreateActivityPolicy(CECQuickBuyPopActivityTrigger *&pTriggerPolicy)
	{
		CECQuickBuyPopBuyItemPolicy *pBuyItemPolicy =
			new CECQuickBuyPopBuyItemPolicy(11221,
			CECUIConfig::Instance().GetGameUI().GetRecommendShopItem(CECUIConfig::GameUI::RECOMMEND_ACTIVITY),
			true,
			false);
		pTriggerPolicy = new CECQuickBuyPopActivityTrigger(pBuyItemPolicy);
		CECQuickBuyPopActivityHide *pHidePolicy = new CECQuickBuyPopActivityHide();
		CECQuickBuyPopActivityAlign *pAlignPolicy = new CECQuickBuyPopActivityAlign();
		return CECQuickBuyPopPolicies(QUICK_BUY_POP_ACTIVITY, pBuyItemPolicy, pTriggerPolicy, pHidePolicy, pAlignPolicy);
	}
}