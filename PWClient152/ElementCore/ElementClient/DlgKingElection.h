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
		REFRESH_INTERVAL = 2000,	// ����ˢ�¼��
		VOTE_ITEM = 38208,			// ���õ�ѡƱ
		VOTE_ITEM_EXPIRE = 38207,	// ��ʱ�޵�ѡƱ
	};

	struct Candidate
	{
		int id;				// ��ѡ��ID
		int vote_count;		// ��Ʊ��
		int deposit;		// ����ʱ����Ʊ��
		int serial_num;		// ���ĵ�˳��
		bool need_name;		// �Ƿ���Ҫ��ȡ����

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

	// ���º�ѡ����Ϣ
	void OnKEGetStatus_Re(void* pData);

	// �ȴ�ͶƱ
	void SetWaitVote(bool bWaitVote) { m_bWaitVote = bWaitVote; }

protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnTick();

	void UpdateUI();
	bool AppendLine(int line, Candidate* pPlayer);
	void UpdateLine(int line);

	// ѡȡһ��ѡƱ
	void SelectVoteItem(int& iItemSlot, int& tid);

	int m_iVoteCount;					// ӵ��ѡƱ����
	CandidateList m_Candidates;			// ��ѡ������
	DWORD m_dwRefreshNextTime;			// ��������ˢ��

	PAUILABEL m_pLblVoteCount;
	abase::vector<PAUILABEL> m_LblRunks;
	abase::vector<PAUILABEL> m_LblNames;
	abase::vector<PAUILABEL> m_LblVotes;
	abase::vector<PAUISTILLIMAGEBUTTON> m_BtnVotes;
	PAUISTILLIMAGEBUTTON m_pBtnRefresh;
	bool m_bWaitVote;
};

// ����ѡ�ٽ��
class CDlgKingResult : public CDlgBase
{
	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgKingResult();

	void OnCommand_CANCEL(const char * szCommand);

	// ���ù�������
	void SetKingID(int iRoleID);

protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnTick();

	PAUILABEL m_pLblKing;
	bool m_bWaitingName;
	int m_iKingID;
};