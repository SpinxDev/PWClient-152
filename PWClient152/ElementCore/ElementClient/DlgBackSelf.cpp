// Filename	: DlgBackSelf.cpp
// Creator	: Xu Wenbin
// Date		: 2009/08/13

#include "AFI.h"
#include "DlgBackSelf.h"
#include "DlgQShop.h"
#include "EC_HostPlayer.h"
#include "EC_GameUIMan.h"
#include "EC_GameSession.h"
#include "getrewardlist_re.hpp"
#include "exchangeconsumepoints_re.hpp"
#include "rewardmaturenotice.hpp"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgBackSelf, CDlgBase)

AUI_ON_COMMAND("change",  OnCommand_Change)
AUI_ON_COMMAND("first",   OnCommand_First)
AUI_ON_COMMAND("prev",    OnCommand_Prev)
AUI_ON_COMMAND("next",    OnCommand_Next)
AUI_ON_COMMAND("end",     OnCommand_End)
AUI_ON_COMMAND("refresh", OnCommand_Refresh)

AUI_END_COMMAND_MAP()

CDlgBackSelf::CDlgBackSelf()
{
	m_pTxt_Point = NULL;
	m_pTxt_Have = NULL;
	m_pLst_Config = NULL;
	m_pLst_Reward = NULL;
	m_pTxt_Time = NULL;
	m_pTxt_Rule = NULL;
	m_pTxt_Page = NULL;

	m_rewardStartIndex = -1;
	m_rewardCount = 0;
	m_consumePoints = 0;
}

bool CDlgBackSelf::OnInitDialog()
{
	DDX_Control("Txt_Point", m_pTxt_Point);
	m_pTxt_Point->SetText(_AL(""));
	
	DDX_Control("Txt_HaveGold", m_pTxt_Have);
	m_pTxt_Have->SetText(_AL(""));

	CECGame::CONSUME_REWARD &config = GetGame()->GetConsumeReward();
	ACString strText;

	DDX_Control("Lst_Config", m_pLst_Config);
	int nRewardType = sizeof(config.iRewardType)/sizeof(config.iRewardType[0]);
	for (int i = 0; i < nRewardType; ++ i)
	{
		int point = config.iRewardType[i][0];
		int gold = config.iRewardType[i][1];
		if (point > 0 || gold > 0)
		{
			strText.Format(_AL("%d\t%d"), point, gold);
			m_pLst_Config->AddString(strText);
			m_pLst_Config->SetItemData(m_pLst_Config->GetCount()-1, i);
		}
	}

	DDX_Control("Lst_Reward", m_pLst_Reward);

	DDX_Control("Txt_Time", m_pTxt_Time);
	strText.Format(GetStringFromTable(8001),
		config.iEndTime[0],
		config.iEndTime[1],
		config.iEndTime[2],
		config.iEndTime[3]);
	m_pTxt_Time->SetText(strText);

	DDX_Control("Txt_Rule", m_pTxt_Rule);
	strText.Format(GetStringFromTable(8002),
		config.iRewardTime[1],
		config.iRewardTime[0]);
	m_pTxt_Rule->SetText(strText);

	DDX_Control("Txt_Page", m_pTxt_Page);
	m_pTxt_Page->SetText(_AL(""));

	return true;
}

void CDlgBackSelf::OnShowDialog()
{
	UpdateText();
	UpdateRewardList();
}

void CDlgBackSelf::OnTick()
{
	UpdateText();
}

ACString CDlgBackSelf::GetCashText(int nCash)
{
	return GetGameUIMan()->GetCashText(nCash);
}

void CDlgBackSelf::UpdateText()
{
	ACString strTemp;

	strTemp = GetCashText(GetHostPlayer()->GetDividend());
	m_pTxt_Have->SetText(strTemp);

	strTemp.Format(_AL("%d"), m_consumePoints);
	m_pTxt_Point->SetText(strTemp);

	int iPage(0), nPage(0);
	if (m_rewardCount > 0)
	{
		iPage = m_rewardStartIndex/REWARD_PAGE_SIZE + 1;
		nPage = m_rewardCount/REWARD_PAGE_SIZE;
		if (m_rewardCount % REWARD_PAGE_SIZE)
			nPage ++;
	}
	strTemp.Format(_AL("%d/%d"), iPage, nPage);
	m_pTxt_Page->SetText(strTemp);
}

void CDlgBackSelf::UpdateRewardList()
{
	if (m_rewardStartIndex < 0)
		m_rewardStartIndex = 0;
	
	GetGameSession()->reward_GetRewardList(m_rewardStartIndex);
}

void CDlgBackSelf::OnCommand_Change(const char *szCommand)
{
	int nSel = m_pLst_Config->GetCurSel();
	if (nSel >= 0 && nSel < m_pLst_Config->GetCount())
	{
		int nRewardType = m_pLst_Config->GetItemData(nSel);
		GetGameSession()->reward_ExchangeConsumePoints(nRewardType);
	}
}

void CDlgBackSelf::OnCommand_First(const char *szCommand)
{
	if (m_rewardCount > 0)
	{
		m_rewardStartIndex = 0;
		UpdateRewardList();
	}
}

void CDlgBackSelf::OnCommand_Prev(const char *szCommand)
{
	if (m_rewardCount > 0)
	{
		if (m_rewardStartIndex - REWARD_PAGE_SIZE >= 0)
			m_rewardStartIndex -= REWARD_PAGE_SIZE;
		UpdateRewardList();
	}
}

void CDlgBackSelf::OnCommand_Next(const char *szCommand)
{
	if (m_rewardCount > 0)
	{
		if (m_rewardStartIndex + REWARD_PAGE_SIZE < m_rewardCount)
			m_rewardStartIndex += REWARD_PAGE_SIZE;
		UpdateRewardList();
	}
}

void CDlgBackSelf::OnCommand_End(const char *szCommand)
{
	if (m_rewardCount > 0)
	{
		m_rewardStartIndex = m_rewardCount;

		m_rewardStartIndex -=
			(m_rewardCount % REWARD_PAGE_SIZE)
			?m_rewardCount % REWARD_PAGE_SIZE
			:REWARD_PAGE_SIZE;

		UpdateRewardList();
	}
}

void CDlgBackSelf::OnCommand_Refresh(const char *szCommand)
{
	UpdateRewardList();
}

void CDlgBackSelf::OnPrtcGetRewardListRe(GNET::GetRewardList_Re *p)
{
	if (p->retcode != ERR_SUCCESS)
		return;

	m_rewardCount = p->total;
	m_rewardStartIndex = p->start_index;
	m_consumePoints = p->consume_points;

	m_pLst_Reward->ResetContent();

	ACString strText;
	tm t;
	int localTime(0);
	size_t count = p->rewardlist.size();
	int timeBias = GetGame()->GetTimeZoneBias() * 60;
	for (size_t i = 0; i < count; ++ i)
	{
		const RewardItem &temp = p->rewardlist[i];
		
		localTime = temp.reward_time - timeBias;
		t = *gmtime((time_t*)&localTime);
		strText.Format(
			GetStringFromTable(8003), 
			t.tm_year + 1900,
			t.tm_mon + 1,
			t.tm_mday,
			t.tm_hour,
			temp.reward_bonus);

		m_pLst_Reward->AddString(strText);
	}
}

void CDlgBackSelf::OnPrtcExchangeConsumePointsRe(GNET::ExchangeConsumePoints_Re *p)
{
	if (p->retcode != ERR_SUCCESS)
		return;

	ACString strText;
	CECGame::CONSUME_REWARD &config = GetGame()->GetConsumeReward();
	strText.Format(GetStringFromTable(8004), p->bonus_add, config.iRewardTime[1]);
	GetGameUIMan()->AddChatMessage(strText, GP_CHAT_MISC);

	// Update consume points
	UpdateRewardList();
}

void CDlgBackSelf::OnPrtcRewardMatrueNotice(GNET::RewardMatureNotice *p)
{
	//	在 CECHostPlayer::SetDividend 时，已经增加提示，此处去掉
// 	ACString strText;
// 	strText.Format(GetStringFromTable(8005), p->bonus_reward);
// 	GetGameUIMan()->AddChatMessage(strText, GP_CHAT_MISC);

	// Update reward list
	UpdateRewardList();
}