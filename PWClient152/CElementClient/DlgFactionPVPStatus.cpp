// Filename	: DlgFactionPVPStatus.cpp
// Creator	: Xu Wenbin
// Date		: 2014/4/2

#include "DlgFactionPVPStatus.h"
#include "DlgFactionPVPRank.h"
#include "DlgGuildMap.h"

#include "EC_GameUIMan.h"
#include "EC_GameSession.h"
#include "EC_Faction.h"
#include "EC_HostPlayer.h"
#include "EC_GameRun.h"
#include "EC_FixedMsg.h"
#include "EC_UIHelper.h"

//	class CDlgFactionPVPStatus

AUI_BEGIN_COMMAND_MAP(CDlgFactionPVPStatus, CDlgBase)
AUI_ON_COMMAND("IDCANCEL",		OnCommandCancel)
AUI_ON_COMMAND("Btn_Reward",	OnCommandViewReward)
AUI_END_COMMAND_MAP()

CDlgFactionPVPStatus::CDlgFactionPVPStatus()
: m_pTxt_GuildName(NULL)
, m_pTxt_Score(NULL)
, m_pTxt_MineCar(NULL)
, m_pTxt_MineBase(NULL)
, m_pTxt_Win(NULL)
, m_pBtn_Reward(NULL)
{
}

bool CDlgFactionPVPStatus::OnInitDialog()
{
	if (CDlgBase::OnInitDialog()){
		DDX_Control("Txt_GuildName",m_pTxt_GuildName);
		DDX_Control("Txt_Score",	m_pTxt_Score);
		DDX_Control("Txt_MineCar",	m_pTxt_MineCar);
		DDX_Control("Txt_MineBase",	m_pTxt_MineBase);
		DDX_Control("Txt_Win",		m_pTxt_Win);
		DDX_Control("Btn_Reward",	m_pBtn_Reward);
		SetCanMove(false);
		ResetInBattleUI();
		return true;
	}
	return false;
}

void CDlgFactionPVPStatus::ResetInBattleUI()
{
	m_pTxt_GuildName->SetText(_AL(""));
	m_pTxt_Score->SetText(_AL(""));
	m_pTxt_MineCar->SetText(_AL(""));
	m_pTxt_MineBase->SetText(_AL(""));
	m_pTxt_Win->SetText(_AL(""));
}

void CDlgFactionPVPStatus::OnShowDialog()
{
	Align();
	CECFactionPVPModel::Instance().QueryFactionPVPInfo();
}

void CDlgFactionPVPStatus::Align()
{
	GetGameUIMan()->m_pDlgGuildMap->AlignFactionPVPStatus();
}

void CDlgFactionPVPStatus::OnCommandCancel(const char * szCommand)
{
	if (PAUIDIALOG pDlgGuildMap = GetAUIManager()->GetDialog("Win_GuildMap")){
		pDlgGuildMap->OnCommand("IDCANCEL");
	}
}

void CDlgFactionPVPStatus::OnCommandViewReward(const char * szCommand)
{
	CDlgFactionPVPRank * pDlgPVPReward = GetGameUIMan()->m_pDlgFactionPVPRank;
	if (!pDlgPVPReward->IsShow()){
		pDlgPVPReward->Show(true);
	}
	//	²éÑ¯ÅÅÐÐ°ñ
	GetGameSession()->factionPVP_GetRank();
}

bool CDlgFactionPVPStatus::ShouldShow()
{
	bool bShould(false);
	if (CECFactionPVPModel::Instance().CanShowInBattleStatus()){
		if (PAUIDIALOG pDlg = GetAUIManager()->GetDialog("Win_GuildMap")){
			bShould = pDlg->IsShow();
		}
	}
	return bShould;
}

void CDlgFactionPVPStatus::OnRegistered(const CECFactionPVPModel *p)
{
	UpdateStatus(p);
}

void CDlgFactionPVPStatus::OnModelChange(const CECFactionPVPModel *p, const CECObservableChange *q)
{
	const CECFactionPVPModelChange *pChange = dynamic_cast<const CECFactionPVPModelChange *>(q);
	if (!pChange){
		ASSERT(false);
	}else if (!pChange->IsInBattleStatusChanged()){
		return;
	}
	UpdateStatus(p);
	if (const CECFactionPVPModelScoreChange *pScoreChange = pChange->ScoreChange()){
		ACString strMsg;
		CECStringTab *pFixedMsgTab = CECUIHelper::GetGame()->GetFixedMsgTab();
		switch (pScoreChange->GetScoreType()){
		case CECFactionPVPModelScoreChange::EVENT_ROB_MINECAR:
			strMsg.Format(pFixedMsgTab->GetWideString(FIXMSG_FACTION_PVP_GOT_SCORE_FOR_MINECAR), pScoreChange->GetScore());
			break;
		case CECFactionPVPModelScoreChange::EVENT_ROB_MINEBASE:
			strMsg.Format(pFixedMsgTab->GetWideString(FIXMSG_FACTION_PVP_GOT_SCORE_FOR_MINEBASE), pScoreChange->GetScore());
			break;
		default:
			ASSERT(false);
			return;
		}
		CECUIHelper::GetGameUIMan()->AddChatMessage(strMsg, GP_CHAT_SYSTEM);
		CECUIHelper::AddHeartBeatHint(strMsg);
	}
}

void CDlgFactionPVPStatus::UpdateStatus(const CECFactionPVPModel *p)
{
	if (p->CanShowInBattleStatus()){
		if (p->InBattleStatusReady()){
			const Faction_Info *pFInfo = GetGame()->GetFactionMan()->GetFaction(p->FactionID());
			m_pTxt_GuildName->SetText(pFInfo ? pFInfo->GetName() : _AL(""));
			
			ACString strText;
			
			strText.Format(_AL("%u"), p->Score());
			m_pTxt_Score->SetText(strText);
			
			strText.Format(_AL("%u/%d"), p->RobbedMineCarCount(), p->MineCarCountCanRob());
			m_pTxt_MineCar->SetText(strText);
			
			strText.Format(_AL("%u/%d"), p->RobbedMineBaseCount(), p->MineBaseCountCanRob());
			m_pTxt_MineBase->SetText(strText);
			
			m_pTxt_Win->SetText(GetStringFromTable(p->CanGetBonus() ? 9772 : 9773));
		}else{
			ResetInBattleUI();
		}
	}else{
		ResetInBattleUI();
	}
	m_pBtn_Reward->Enable(p->CanShowReportList());
	if (IsShow() != ShouldShow()){
		SwitchShow();
	}
}