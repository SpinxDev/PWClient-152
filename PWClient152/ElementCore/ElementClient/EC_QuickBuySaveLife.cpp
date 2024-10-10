// File		: EC_QuickBuySaveLife.cpp
// Creator	: Xu Wenbin
// Date		: 2013/12/20

#include "EC_QuickBuySaveLife.h"
#include "EC_Game.h"
#include "EC_UIConfigs.h"
#include "EC_LogicHelper.h"
#include "EC_HostPlayer.h"
#include "EC_Inventory.h"

#include <AUIDialog.h>
#include <AUIManager.h>
#include <AIniFile.h>

//	CECQuickBuyPopSaveLifeTrigger

CECQuickBuyPopSaveLifeTrigger::CECQuickBuyPopSaveLifeTrigger(CECQuickBuyPopBuyItemPolicy *pBuyItemPolicy)
: CECQuickBuyPopTriggerPolicy(pBuyItemPolicy)
, m_dwTimeupTime(0)
, m_dwCloseTime(0)
{
}

bool CECQuickBuyPopSaveLifeTrigger::CanTriggerCheckMore()const
{
	bool bCan(false);
	while (true)
	{
		CECHostPlayer *pHost = CECLogicHelper::GetHostPlayer();
		if (!pHost->IsFighting()){
			//	不在战斗中
			break;
		}
		if (pHost->IsDead()){
			//	死亡时不弹出
			break;
		}
		if (pHost->IsInCountryWar()){
			//	国战时死亡无损失，不弹出
			break;
		}
		if (IsJustRevived() ||	//	复活后一段时间暂时不弹出
			IsJustTimeup() ||	//	玩家未处理消失后、一段时间内不再弹出
			IsJustClosed()){	//	玩家手动关闭，一段时间内不再弹出		
			break;
		}
		const ROLEEXTPROP& ep = pHost->GetExtendProps();
		const ROLEBASICPROP& bp = pHost->GetBasicProps();
		float cur_hp_percent = bp.iCurHP/(float)max(ep.bs.max_hp, 1) * 100;
		if (cur_hp_percent > 30.0f){
			//	血量未达到要求
			break;
		}
		//	检查包裹中其它愿望灵童
		const static int other_items[] = {12361, 36309, 31878};
		if (pHost->GetPack()->FindItems(other_items, sizeof(other_items)/sizeof(other_items[0]))){
			break;
		}
		bCan = true;
		break;
	}
	return bCan;
}

bool CECQuickBuyPopSaveLifeTrigger::NeedCheckHide()const
{
	//	当前设置要求提供“当天不再限制”勾选项
	return true;
}

const char * CECQuickBuyPopSaveLifeTrigger::GetConfigName()const
{
	return "SAVE_LIFE";
}

void CECQuickBuyPopSaveLifeTrigger::OnHideMore()
{
	const char *szHideReason = GetHideReason();
	if (szHideReason){
		if (!stricmp(szHideReason, "HR_Timeup")){
			m_dwTimeupTime = timeGetTime();
		}else if (!stricmp(szHideReason, "HR_UserClose")){
			m_dwCloseTime = timeGetTime();
		}
	}
}

bool CECQuickBuyPopSaveLifeTrigger::CanHideMore(const char *&szHideReason)const
{
	//	复活时界面不再显示，以处理死亡前未购买、或购买失败等情况
	//	已购买但未返回情况此处忽略
	if (IsJustRevived()){
		szHideReason = "HR_JustRevived";
		return true;
	}
	return false;
}

bool CECQuickBuyPopSaveLifeTrigger::IsJustDone(DWORD dwTimeStamp, int nSeconds)const
{
	bool bRet(false);
	if (dwTimeStamp != 0){
		DWORD dwNow = timeGetTime();
		if (dwNow <= (dwTimeStamp + nSeconds*1000)){
			bRet = true;
		}
	}
	return bRet;
}

bool CECQuickBuyPopSaveLifeTrigger::IsJustRevived()const
{
	return IsJustDone(CECLogicHelper::GetHostPlayer()->GetReviveTime(), 60);
}

bool CECQuickBuyPopSaveLifeTrigger::IsJustTimeup()const
{
	return IsJustDone(m_dwTimeupTime, 45);
}

bool CECQuickBuyPopSaveLifeTrigger::IsJustClosed()const
{
	return IsJustDone(m_dwCloseTime, 60);
}

//	CECQuickBuyPopSaveLifeHide
CECQuickBuyPopSaveLifeHide::CECQuickBuyPopSaveLifeHide(DWORD disappearTimeLength)
{
	m_disappearCnt.SetPeriod(disappearTimeLength);
}

void CECQuickBuyPopSaveLifeHide::Restart()
{
	m_disappearCnt.Reset();
}

void CECQuickBuyPopSaveLifeHide::TickOnShow(AUIDialog *pDlg)
{
	AUIDialog *pDlgMouseOn = NULL;
	pDlg->GetAUIManager()->GetMouseOn(&pDlgMouseOn, NULL);
	if (pDlgMouseOn == pDlg){
		m_disappearCnt.Reset();
	}else if (!m_disappearCnt.IsFull()){
		m_disappearCnt.IncCounter(CECLogicHelper::GetGame()->GetRealTickTime());
	}
}

bool CECQuickBuyPopSaveLifeHide::CanHide(const char *&szHideReason)
{
	if (m_disappearCnt.IsFull()){
		szHideReason = "HR_Timeup";
		return true;
	}
	return false;
}

//	CECQuickBuyPopSaveLifeAlign
void CECQuickBuyPopSaveLifeAlign::OnShow(AUIDialog *pDlg)
{
}

//	namespace QuickBuyPopPolicy
namespace QuickBuyPopPolicy
{
	CECQuickBuyPopPolicies	CreateSaveLifePolicy(CECQuickBuyPopSaveLifeTrigger *&pTriggerPolicy)
	{
		CECQuickBuyPopBuyItemPolicy *pBuyItemPolicy =
			new CECQuickBuyPopBuyItemPolicy(11220,
			CECUIConfig::Instance().GetGameUI().GetRecommendShopItem(CECUIConfig::GameUI::RECOMMEND_SAVE_LIFE),
			false,
			true);
		pTriggerPolicy = new CECQuickBuyPopSaveLifeTrigger(pBuyItemPolicy);
		CECQuickBuyPopSaveLifeHide *pHidePolicy = new CECQuickBuyPopSaveLifeHide(30*1000);
		CECQuickBuyPopSaveLifeAlign *pAlignPolicy = new CECQuickBuyPopSaveLifeAlign();
		return CECQuickBuyPopPolicies(QUICK_BUY_POP_SAFE_LIFE, pBuyItemPolicy, pTriggerPolicy, pHidePolicy, pAlignPolicy);
	}
}