// Filename	: DlgFactionPVPRank.h
// Creator	: Xu Wenbin
// Date		: 2014/4/2

#ifndef _ELEMENTCLIENT_DLGFACTIONPVPRANK_H_
#define _ELEMENTCLIENT_DLGFACTIONPVPRANK_H_

#include "DlgBase.h"
#include "EC_FactionPVP.h"

#include <AUIListBox.h>

class CDlgFactionPVPRank : public CDlgBase, public CECFactionPVPModelObserver
{
	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgFactionPVPRank();
	
	void OnCommandHomePage(const char * szCommand);
	void OnCommandPrevPage(const char * szCommand);
	void OnCommandNextPage(const char * szCommand);
	void OnCommandLastPage(const char * szCommand);

	void OnCommandSortByName(const char * szCommand);
	void OnCommandSortByKillNumber(const char * szCommand);
	void OnCommandSortByDeathNumber(const char * szCommand);
	void OnCommandSortByUseItemCount(const char * szCommand);
	void OnCommandSortByScore(const char *szCommand);

	//	继承自 FactionPVPModelObserver
	virtual void OnRegistered(const CECFactionPVPModel *p);
	virtual void OnModelChange(const CECFactionPVPModel *p, const CECObservableChange *q);

	bool CanShow();
	
protected:
	virtual bool OnInitDialog();
	virtual void OnTick();

	void UpdateRankList(const CECFactionPVPModel *p);

	void ResetUI();
	void UpdatePageButtons();
	void UpdateCurrentPage();
	void UpdateOtherInfo();
	bool PlayerName(int roleid, const ACHAR * &szName);

	bool CanGoHomePage()const;
	bool CanGoPrevPage()const;
	bool CanGoNextPage()const;
	bool CanGoLastPage()const;
		
	AUIObject		*m_pTxt_GuildName;
	AUIObject		*m_pTxt_MyScore;
	AUIObject		*m_pTxt_RewardListSize;
	AUIListBox		*m_pLst_Result;
	AUIObject		*m_pBtn_HomePage;
	AUIObject		*m_pBtn_PrevPage;
	AUIObject		*m_pBtn_NextPage;
	AUIObject		*m_pBtn_LastPage;
	int				m_reportCount, m_pageCount, m_currentPage;

	enum ListItemIndex{				//	ListBox 中各列数据含义（对应 CECFactionPVPModel::ReportItem 中各项）
		LST_ITEMINDEX_NAME = 0,		//	角色名称
		LST_ITEMINDEX_KILL_COUNT,	//	击杀数
		LST_ITEMINDEX_DEATH_COUNT,	//	死亡次数
		LST_ITEMINDEX_USETOOL_COUNT,//	保护道具使用次数
		LST_ITEMINDEX_SCORE,		//	积分数
	};
};

#endif