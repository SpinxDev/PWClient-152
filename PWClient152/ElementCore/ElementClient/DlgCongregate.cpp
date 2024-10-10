// Filename	: DlgCongregate.cpp
// Creator	: Feng Ning
// Date		: 2011/04/14

#include "DlgCongregate.h"
#include "EC_GameSession.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_GameUIMan.h"
#include "EC_HostPlayer.h"

#include "DlgMinimizeBar.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgCongregate, CDlgBase)

AUI_ON_COMMAND("Btn_Confirm",	OnCommand_Confrim)
AUI_ON_COMMAND("Btn_Reject",	OnCommand_Reject)
AUI_ON_COMMAND("Btn_Later",		OnCommand_Later)
AUI_ON_COMMAND("IDCANCEL",		OnCommand_Later)

AUI_END_COMMAND_MAP()

CDlgCongregate::CDlgCongregate()
:m_Type(-1)
,m_LastTime(0)
,m_Sponsor(0)
,m_Timeout(0)
,m_pBtn_Later(NULL)
,m_pTxt_Msg(NULL)
,m_pBtn_Confirm(NULL)
{
}

CDlgCongregate::~CDlgCongregate()
{
}

bool CDlgCongregate::OnInitDialog()
{
	if(!CDlgBase::OnInitDialog())
		return false;
	
	m_pTxt_Msg = GetDlgItem("Txt_Msg");
	m_pBtn_Confirm = GetDlgItem("Btn_Confirm");
	m_pBtn_Later = GetDlgItem("Btn_Later");

	size_t prefix = strlen("Win_Congregate");
	if(strlen(GetName()) > prefix)
	{
		const char* pType = GetName() + prefix;
		m_Type = atoi(pType);
	}

	m_LastTime = 0;
	m_Sponsor = 0;
	m_Timeout = 0;

	return true;
}

void CDlgCongregate::Reset()
{
	// cancel the mini bar
	GetGameUIMan()->GetMiniBarMgr()->RestoreDialog(this);
	
	if(IsShow()) Show(false);
	
	m_LastTime = 0;
	m_Sponsor = 0;
	m_Timeout = 0;
}

void CDlgCongregate::OnTick()
{
	RefreshPendingMsg();

	// check the response time
	if(!IsAvailable())
	{
		return;
	}

	int leftTime = GetGame()->GetServerAbsTime() - m_LastTime;

	if( leftTime >= m_Timeout || 
		leftTime < 0 /* error time stamp */)
	{
		// show the timeout msg
		ACString msg;
		GetGameUIMan()->AddChatMessage(msg.Format(GetStringFromTable(5713), GetTypeName()), GP_CHAT_MISC);

		// hide UI
		Reset();
		return;
	}

	// refresh time
	if(m_pTxt_Msg)
	{
		ACString strMsg;
		strMsg.Format(  GetStringFromTable(5710), 
						GetGameRun()->GetPlayerName(m_Sponsor, true),
						GetTypeName(),
						m_pBtn_Later ? m_pBtn_Later->GetText() : _AL("--"),
						GetGameUIMan()->GetFormatTime(m_Timeout - leftTime) );
		m_pTxt_Msg->SetText(strMsg);
	}

	// check the status, but this is NOT reliable
	if(m_pBtn_Confirm)
	{
		m_pBtn_Confirm->Enable(
			!GetHostPlayer()->IsFighting() && 
			!GetHostPlayer()->IsCongregating() &&
			!GetHostPlayer()->IsTrading() &&
			!GetHostPlayer()->GetBoothState() != 0 );
	}
}

void CDlgCongregate::OnCommand_Confrim(const char * szCommand)
{
	if(IsAvailable())
	{
		// NOTICE: hide UI in congregate start message
		GetGameSession()->c2s_CmdCongregateReply(m_Type, true, m_Sponsor);
	}
}

void CDlgCongregate::OnCommand_Reject(const char * szCommand)
{
	if(IsAvailable())
	{
		GetGameSession()->c2s_CmdCongregateReply(m_Type, false, m_Sponsor);
		Reset();
	}
}

void CDlgCongregate::OnCommand_Later(const char * szCommand)
{
	if(IsShow())
	{
		GetGameUIMan()->GetMiniBarMgr()->MinimizeDialog(this,
			CECMiniBarMgr::BarInfo(GetTypeName(), A3DCOLORRGB(255, 255, 255)));
	}
}

void CDlgCongregate::OnCongregateAction(int idAction, void* pData)
{
	using namespace S2C;

	if(CONGREGATE_REQUEST == idAction)
	{
		cmd_congregate_request* pCmd = (cmd_congregate_request*)pData;
		ASSERT(pCmd->type == m_Type);
		m_Sponsor = pCmd->sponsor;
		m_Timeout = pCmd->timeout;
		m_LastTime = GetGame()->GetServerAbsTime();
		if(!IsShow())
		{
			// cancel the mini bar
			GetGameUIMan()->GetMiniBarMgr()->RestoreDialog(this);
			Show(true);
		}

		if(!GetGameRun()->GetPlayerName(m_Sponsor, false))
		{
			GetGameSession()->GetPlayerBriefInfo(1, &m_Sponsor, 2);
		}
	}
	else if(REJECT_CONGREGATE == idAction)
	{
		cmd_reject_congregate* pCmd = (cmd_reject_congregate*)pData;
		int playerID = pCmd->id;
		if(!GetGameRun()->GetPlayerName(playerID, false))
		{
			GetGameSession()->GetPlayerBriefInfo(1, &playerID, 2);
		}
		m_RejectID.push_back(playerID);
	}
	else if(CONGREGATE_START == idAction)
	{
		Reset();
	}
	else if(CANCEL_CONGREGATE == idAction)
	{
		ACString msg;
		GetGameUIMan()->AddChatMessage(msg.Format(GetStringFromTable(5714), GetTypeName()), GP_CHAT_MISC);
		Reset();
	}
}

ACString CDlgCongregate::GetTypeName()
{
	ACString tyepName = GetStringFromTable(5700 + m_Type);
	if(tyepName.GetLength() == 0)
	{
		// unknown congregate type
		tyepName.Format(GetStringFromTable(5709), m_Type);
		ASSERT(FALSE);
	}

	return tyepName;
}

void CDlgCongregate::RefreshPendingMsg()
{
	if(m_RejectID.size() == 0)
	{
		return;
	}

	// refresh the pending message if player's name is empty
	abase::vector<int> pending;
	for(size_t i=0;i<m_RejectID.size();++i)
	{
		int playerID = m_RejectID[i];
		ACString playerName = GetGameRun()->GetPlayerName(playerID, false);
		if(playerName.IsEmpty())
		{
			pending.push_back(playerID);
		}
		else
		{
			ACString msg;
			GetGameUIMan()->AddChatMessage(msg.Format(GetStringFromTable(5712), playerName, GetTypeName()), GP_CHAT_MISC);
		}
	}
	m_RejectID.swap(pending);
}