// Filename	: DlgPQ.h
// Creator	: Xu Wenbin
// Date		: 2009/10/12

#pragma once

#include "DlgTaskTrace.h"
#include "EC_RoleTypes.h"
#include "AUILabel.h"
#include "AUIStillImageButton.h"
#include "AUIRadioButton.h"
#include "AUIImagePicture.h"
#include "AUIScroll.h"

class CDlgPQ : public CDlgTaskTrace
{
	AUI_DECLARE_EVENT_MAP();
	AUI_DECLARE_COMMAND_MAP()
		
public:
	CDlgPQ();

	void OnEventMouseWheel(WPARAM wParam, LPARAM lParam, AUIObject *pObj);

	void OnCommand_CANCEL(const char * szCommand);
	void OnCommand_Rank(const char *szCommand);
	void OnCommand_RankList(const char *szCommand);

public:
	struct RankInfo
	{
		int place;
		int roleid;
		ACString rolename;
		int rand_score;
		int score;
	};
	typedef abase::vector<RankInfo> RankList;
	void UpdateRank(int profession, const RankList &rList);
	
	struct PQInfo
	{
		int task_id;
		int sub_task_id;
		int awardinfo_task_id;  // task id for giving award info
		                        // NOT the same as sub_task_id
		                        // for display only
		int score;
		int prof_place;
		int all_place;

		PQInfo()
		{
			Clear();
		}

		void Clear()
		{
			task_id = 0;
			sub_task_id = 0;
			awardinfo_task_id = 0;
			score = 0;
			prof_place = 0;
			all_place = 0;
		}

		bool IsValid()const
		{
			return task_id>0 && sub_task_id>0;
		}
	};
	void UpdatePQ(const PQInfo &pqInfo);

	void UpdatePQPlayerNumber(int num);
	
protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual bool Tick();

	void RefreshPQUI();
	void RefreshRankUI();

	bool IsPQtitle();
	bool IsPQrank();

	// PQtitle
	PAUILABEL m_pTxt_Name0;
	PAUISTILLIMAGEBUTTON m_pBtn_Rank;
	PAUISTILLIMAGEBUTTON m_pBtn_Award;
	PAUILABEL m_pTxt_Name1;
	
	// PQrank
	PAUILABEL m_pLab_Radio[NUM_PROFESSION+1];
	int GetAllProfID(){ return NUM_PROFESSION; }

	enum {PAGE_SIZE = 7, RANDOM_COUNT = 5};
	struct RankListItemUI;
	friend struct RankListItemUI;
	struct RankListItemUI
	{
		PAUILABEL Txt_Rank;
		PAUIIMAGEPICTURE Img_Awards;
		PAUILABEL Txt_Name;
		PAUIIMAGEPICTURE Img_Random[RANDOM_COUNT];
		PAUILABEL Txt_Contribution;

		void Show(bool bShow)
		{
			Txt_Rank->Show(bShow);
			Img_Awards->Show(bShow);
			Txt_Name->Show(bShow);
			for (int i = 0; i < RANDOM_COUNT; ++ i)
				Img_Random[i]->Show(bShow);
			Txt_Contribution->Show(bShow);
		}
	};
	RankListItemUI m_rankListUI[PAGE_SIZE];
	PAUILABEL m_pTxt_Time;   // Estimated time left for another update of rank list
	int m_lastProfSelected;  // Last selected radio button: equals to profession
	PAUISCROLL m_pScl_List;  // Scroll bar to select part of current profession rank list
	PAUILABEL m_pTxt_PlayerNum;
	
	PAUILABEL m_pTxt_myRank;
	PAUIIMAGEPICTURE m_pImg_myAwards;
	PAUILABEL m_pTxt_myName;
	PAUILABEL m_pTxt_myContribution;

private:
	static RankList s_rankList[NUM_PROFESSION+1];
	static PQInfo s_PQInfo;
	static int    s_playerNumber;
	static PQInfo & GetPQInfo(){return s_PQInfo;}
	static RankList & GetRankList(int profession) { return s_rankList[profession];}
	static RankInfo & GetRankListItem(int profession, int item) { return s_rankList[profession][item]; }
	static void ClearPQ();
	static void ClearRank();
	static bool IsHasPQ();
	static time_t m_lastTimeUpdated;  // latest update time of PQ rank list	
	static time_t GetUpdateTimeLength(){return 60;} // in seconds
	static void SetConstrainedHint(PAUIOBJECT pObj, const ACString &strHint);
};
