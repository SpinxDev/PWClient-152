// Filename	: DlgBackSelf.h
// Creator	: Xu Wenbin
// Date		: 2009/08/13

#pragma once

#include "DlgBase.h"
#include "AUILabel.h"
#include "AUIListBox.h"

namespace GNET
{
	class GetRewardList_Re;
	class ExchangeConsumePoints_Re;
	class RewardMatureNotice;
}

class CDlgBackSelf : public CDlgBase
{
	AUI_DECLARE_COMMAND_MAP();

public:
	CDlgBackSelf();

	void OnCommand_Change(const char *szCommand);
	void OnCommand_First(const char *szCommand);
	void OnCommand_Prev(const char *szCommand);
	void OnCommand_Next(const char *szCommand);
	void OnCommand_End(const char *szCommand);
	void OnCommand_Refresh(const char *szCommand);

	void OnPrtcGetRewardListRe(GNET::GetRewardList_Re *p);
	void OnPrtcExchangeConsumePointsRe(GNET::ExchangeConsumePoints_Re *p);
	void OnPrtcRewardMatrueNotice(GNET::RewardMatureNotice *p);
	
protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnTick();

	ACString GetCashText(int nCash);
	void UpdateText();
	void UpdateRewardList();

	PAUILABEL m_pTxt_Point;
	PAUILABEL m_pTxt_Have;
	PAUILISTBOX m_pLst_Config;
	PAUILISTBOX m_pLst_Reward;
	PAUILABEL m_pTxt_Time;
	PAUILABEL m_pTxt_Rule;
	PAUILABEL m_pTxt_Page;
	
	enum {REWARD_PAGE_SIZE = 15};
	int  m_rewardStartIndex;
	int  m_rewardCount;
	int  m_consumePoints;
};