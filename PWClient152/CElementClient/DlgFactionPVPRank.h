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

	//	�̳��� FactionPVPModelObserver
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

	enum ListItemIndex{				//	ListBox �и������ݺ��壨��Ӧ CECFactionPVPModel::ReportItem �и��
		LST_ITEMINDEX_NAME = 0,		//	��ɫ����
		LST_ITEMINDEX_KILL_COUNT,	//	��ɱ��
		LST_ITEMINDEX_DEATH_COUNT,	//	��������
		LST_ITEMINDEX_USETOOL_COUNT,//	��������ʹ�ô���
		LST_ITEMINDEX_SCORE,		//	������
	};
};

#endif