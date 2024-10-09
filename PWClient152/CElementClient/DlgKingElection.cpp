// Filename	: DlgKingElection.cpp
// Creator	: Shi && Wang
// Date		: 2013/1/29

#include "DlgKingElection.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_GameUIMan.h"
#include "EC_GameSession.h"
#include "EC_HostPlayer.h"
#include "EC_Inventory.h"
#include "EC_IvtrItem.h"
#include "AUIManager.h"
#include "AIniFile.h"

#include "kevoting_re.hpp"
#include "kegetstatus_re.hpp"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgKingElection, CDlgBase)

AUI_ON_COMMAND("Btn_Vote*", OnCommand_Vote)
AUI_ON_COMMAND("Btn_Refresh", OnCommand_Refresh)
AUI_ON_COMMAND("IDCANCEL", OnCommand_CANCEL)

AUI_END_COMMAND_MAP()


// 对候选人进行排序


CDlgKingElection::CDlgKingElection()
{
	m_iVoteCount = -1;
	m_dwRefreshNextTime = 0;
	m_pBtnRefresh = NULL;
	m_pLblVoteCount = NULL;
	m_bWaitVote = false;
}

CDlgKingElection::~CDlgKingElection()
{
}

bool CDlgKingElection::OnInitDialog()
{
	if (!CDlgBase::OnInitDialog())
		return false;

	m_pBtnRefresh = (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_Refresh");
	m_pLblVoteCount = (PAUILABEL)GetDlgItem("Lbl_Number");

	int i = 1;
	char szName[40];
	PAUILABEL pLbl;
	PAUISTILLIMAGEBUTTON pBtn;
	while(1)
	{
		_snprintf(szName, 40, "Lbl_Rank%d", i);
		pLbl = (PAUILABEL)GetDlgItem(szName);
		if( !pLbl ) break;

		m_LblRunks.push_back(pLbl);
		_snprintf(szName, 40, "Lbl_Name%d", i);
		pLbl = (PAUILABEL)GetDlgItem(szName);
		pLbl->SetText(_AL(""));
		m_LblNames.push_back(pLbl);
		_snprintf(szName, 40, "Lbl_Vote%d", i);
		pLbl = (PAUILABEL)GetDlgItem(szName);
		pLbl->SetText(_AL("0"));
		m_LblVotes.push_back(pLbl);
		_snprintf(szName, 40, "Btn_Vote%d", i);
		pBtn = (PAUISTILLIMAGEBUTTON)GetDlgItem(szName);
		pBtn->Enable(false);
		m_BtnVotes.push_back(pBtn);
		i++;
	}

	return true;
}

void CDlgKingElection::OnShowDialog()
{
	m_pBtnRefresh->Enable(false);
	m_dwRefreshNextTime = GetTickCount() + REFRESH_INTERVAL;
}

void CDlgKingElection::OnTick()
{
	size_t i;

	// 获取选票个数
	ACString str;
	m_iVoteCount = GetHostPlayer()->GetPack()->GetItemTotalNum(VOTE_ITEM);
	m_iVoteCount += GetHostPlayer()->GetPack()->GetItemTotalNum(VOTE_ITEM_EXPIRE);
	str.Format(_AL("%d"), m_iVoteCount);
	m_pLblVoteCount->SetText(str);

	// 投票按钮
	for( i=0; i<m_BtnVotes.size(); i++ )
		m_BtnVotes[i]->Enable(i < m_Candidates.size() && m_iVoteCount > 0 && !m_bWaitVote);

	// 更新名字
	for( i=0; i<m_Candidates.size(); i++ )
	{
		if( m_Candidates[i].need_name )
			UpdateLine(i);
	}

	// 更新刷新按钮
	DWORD dwCurTime = GetTickCount();
	if( !m_pBtnRefresh->IsEnabled() && dwCurTime >= m_dwRefreshNextTime )
		m_pBtnRefresh->Enable(true);
}

// 收到候选人信息
void CDlgKingElection::OnKEGetStatus_Re(void* pData)
{
	using namespace GNET;
	KEGetStatus_Re* p = (KEGetStatus_Re*)pData;
	if( !p ) return;

	m_Candidates.clear();

	for( size_t i=0;i<p->candidate_list.size();i++ )
	{
		Candidate player;
		player.id = p->candidate_list[i].roleid;
		player.vote_count = p->candidate_list[i].votes;
		player.deposit = p->candidate_list[i].deposit;
		player.serial_num = p->candidate_list[i].serial_num;
		player.need_name = true;
		m_Candidates.push_back(player);
	}

	// 按照票数进行排序
	std::sort(m_Candidates.begin(), m_Candidates.end());

	UpdateUI();
}

void CDlgKingElection::OnKEVoting_Re(void* pData)
{
	using namespace GNET;

	ACString errMsg;
	KEVoting_Re* retp = (KEVoting_Re*)pData;
	if( retp && retp->retcode == 0 )
	{
		GetGameUIMan()->AddChatMessage(GetStringFromTable(10315), GP_CHAT_MISC);
		OnCommand_Refresh("");
	}
	else if( retp && g_pGame->GetGameSession()->GetServerError(retp->retcode, errMsg) )
		m_pAUIManager->MessageBox("", errMsg, MB_OK, A3DCOLORRGB(255, 255, 255));
}

void CDlgKingElection::UpdateUI()
{
	size_t i;
	abase::vector<int> ids;

	for( i=0;i<m_LblRunks.size();i++ )
	{
		m_LblNames[i]->SetText(_AL(""));
		m_LblVotes[i]->SetText(_AL("0"));
	}

	for( i=0;i<m_Candidates.size();i++ )
	{
		if( AppendLine(i, &m_Candidates[i]) )
			ids.push_back(m_Candidates[i].id);
	}

	// 请求名字
	if( ids.size() )
		GetGameSession()->CacheGetPlayerBriefInfo(ids.size(), &ids[0], 2);
}

bool CDlgKingElection::AppendLine(int line, Candidate* pPlayer)
{
	bool bNeedName = false;
	ACString strText, strName;

	strName = g_pGame->GetGameRun()->GetPlayerName(pPlayer->id, false);
	if( strName.IsEmpty() )
	{
		strName = _AL("-");
		bNeedName = true;
	}
	else
	{
		pPlayer->need_name = false;
	}

	ACString str;
	str.Format(_AL("%d"), line+1);
	m_LblRunks[line]->SetText(str);
	m_LblNames[line]->SetText(strName);
	str.Format(_AL("%d"), pPlayer->vote_count);
	m_LblVotes[line]->SetText(str);

	return bNeedName;
}

void CDlgKingElection::UpdateLine(int line)
{
	ACString strName;
	strName = g_pGame->GetGameRun()->GetPlayerName(m_Candidates[line].id, false);
	if( !strName.IsEmpty() )
	{
		m_Candidates[line].need_name = false;
		m_LblNames[line]->SetText(strName);
	}
}

void CDlgKingElection::OnCommand_Vote(const char * szCommand)
{
	int iIdx = atoi(szCommand + strlen("Btn_Vote")) - 1;
	ASSERT(iIdx >= 0 && iIdx < (int)m_BtnVotes.size());
	if( iIdx >= (int)m_Candidates.size() ) return;

	int iSlot, tid;
	CECInventory* pPack = GetHostPlayer()->GetPack();
	SelectVoteItem(iSlot, tid);
	int iCandidate = m_Candidates[iIdx].id;
	if( iSlot >= 0 )
		GetGameSession()->king_CandidateVote(iCandidate, tid, iSlot, 1);
}

void CDlgKingElection::OnCommand_Refresh(const char * szCommand)
{
	GetGameSession()->king_GetCandidateStatus();

	m_pBtnRefresh->Enable(false);
	m_dwRefreshNextTime = GetTickCount() + REFRESH_INTERVAL;
}

void CDlgKingElection::OnCommand_CANCEL(const char * szCommand)
{
	Show(false);
	GetGameUIMan()->EndNPCService();
}

void CDlgKingElection::SelectVoteItem(int& iItemSlot, int& tid)
{
	int iSlot = -1, iSlot2 = -1;
	int iExpireTime = INT_MAX;
	CECInventory* pPack = GetHostPlayer()->GetPack();
	for( int i=0;i<pPack->GetSize();i++ )
	{
		CECIvtrItem* pItem = pPack->GetItem(i);
		if( !pItem || (pItem->GetTemplateID() != VOTE_ITEM && pItem->GetTemplateID() != VOTE_ITEM_EXPIRE) )
			continue;

		if( pItem->GetTemplateID() == VOTE_ITEM_EXPIRE )
		{
			if( pItem->GetExpireDate() < iExpireTime )
			{
				iSlot = i;
				iExpireTime = pItem->GetExpireDate();
			}
		}
		else if( pItem->GetTemplateID() == VOTE_ITEM && iSlot2 == -1 )
		{
			iSlot2 = i;
		}
	}

	iItemSlot = (iSlot != -1) ? iSlot : iSlot2;
	tid = (iSlot != -1) ? VOTE_ITEM_EXPIRE : VOTE_ITEM;
}

///////////////////////////////////////////////////////////////////////////
// 国王选举结果

AUI_BEGIN_COMMAND_MAP(CDlgKingResult, CDlgBase)

AUI_ON_COMMAND("Btn_Confirm", OnCommand_CANCEL)
AUI_ON_COMMAND("IDCANCEL", OnCommand_CANCEL)

AUI_END_COMMAND_MAP()


CDlgKingResult::CDlgKingResult()
{
	m_pLblKing = NULL;
}

bool CDlgKingResult::OnInitDialog()
{
	DDX_Control("Label_Name", m_pLblKing);
	return true;
}

void CDlgKingResult::OnShowDialog()
{
	m_bWaitingName = false;
	SetKingID(-1);
}

void CDlgKingResult::OnTick()
{
	if( m_bWaitingName )
	{
		const ACHAR* szName = g_pGame->GetGameRun()->GetPlayerName(m_iKingID, false);
		if( szName )
		{
			m_pLblKing->SetColor(A3DCOLORRGB(255, 255, 255));
			m_pLblKing->SetText(szName);
			m_bWaitingName = false;
		}
	}
}

void CDlgKingResult::SetKingID(int iRoleID)
{
	m_iKingID = iRoleID;

	if( iRoleID > 0 )
	{
		const ACHAR* szName = g_pGame->GetGameRun()->GetPlayerName(iRoleID, false);
		if( szName )
		{
			m_pLblKing->SetColor(A3DCOLORRGB(255, 255, 255));
			m_pLblKing->SetText(szName);
		}
		else
		{
			GetGameSession()->CacheGetPlayerBriefInfo(1, &iRoleID, 2);
			m_bWaitingName = true;
		}
	}
	else
	{
		m_pLblKing->SetColor(A3DCOLORRGB(128, 128, 128));
		m_pLblKing->SetText(GetStringFromTable(10312));
	}
}

void CDlgKingResult::OnCommand_CANCEL(const char * szCommand)
{
	Show(false);
	GetGameUIMan()->EndNPCService();
}