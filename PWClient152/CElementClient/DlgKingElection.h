// Filename	: DlgKingElection.h
// Creator	: Shi && Wang
// Date		: 2013/1/29

#pragma once

#include "DlgBase.h"
#include "EC_RoleTypes.h"
#include <AUIStillImageButton.h>
#include <AUILabel.h>
#include <vector.h>


class CDlgKingElection : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP()

	enum
	{
		REFRESH_INTERVAL = 2000,	// 连续刷新间隔
		VOTE_ITEM = 38208,			// 永久的选票
		VOTE_ITEM_EXPIRE = 38207,	// 有时限的选票
	};

	struct Candidate
	{
		int id;				// 候选人ID
		int vote_count;		// 得票数
		int deposit;		// 竞拍时的银票数
		int serial_num;		// 竞拍的顺序
		bool need_name;		// 是否需要获取名字

		Candidate() : id(0), vote_count(0), deposit(0), serial_num(0), need_name(true)
		{
		}

		bool operator < (const Candidate& rhs) const
		{
			return (vote_count > rhs.vote_count) ||
				(vote_count == rhs.vote_count && deposit > rhs.deposit) ||
				(vote_count == rhs.vote_count && deposit == rhs.deposit && serial_num < rhs.serial_num);
		}
	};

	typedef abase::vector<Candidate> CandidateList;

public:
	CDlgKingElection();
	virtual ~CDlgKingElection();

	void OnCommand_Vote(const char * szCommand);
	void OnCommand_Refresh(const char * szCommand);
	void OnCommand_CANCEL(const char * szCommand);

	void OnKEVoting_Re(void* pData);

	// 更新候选人信息
	void OnKEGetStatus_Re(void* pData);

	// 等待投票
	void SetWaitVote(bool bWaitVote) { m_bWaitVote = bWaitVote; }

protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnTick();

	void UpdateUI();
	bool AppendLine(int line, Candidate* pPlayer);
	void UpdateLine(int line);

	// 选取一个选票
	void SelectVoteItem(int& iItemSlot, int& tid);

	int m_iVoteCount;					// 拥有选票数量
	CandidateList m_Candidates;			// 候选人数据
	DWORD m_dwRefreshNextTime;			// 限制连续刷新

	PAUILABEL m_pLblVoteCount;
	abase::vector<PAUILABEL> m_LblRunks;
	abase::vector<PAUILABEL> m_LblNames;
	abase::vector<PAUILABEL> m_LblVotes;
	abase::vector<PAUISTILLIMAGEBUTTON> m_BtnVotes;
	PAUISTILLIMAGEBUTTON m_pBtnRefresh;
	bool m_bWaitVote;
};

// 国王选举结果
class CDlgKingResult : public CDlgBase
{
	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgKingResult();

	void OnCommand_CANCEL(const char * szCommand);

	// 设置国王名字
	void SetKingID(int iRoleID);

protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnTick();

	PAUILABEL m_pLblKing;
	bool m_bWaitingName;
	int m_iKingID;
};