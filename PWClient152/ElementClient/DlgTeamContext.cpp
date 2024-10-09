// Filename	: DlgTeamContext.cpp
// Creator	: Tom Zhou
// Date		: September 1, 2005

#include "AUIComboBox.h"
#include "DlgTeamContext.h"
#include "DlgArrangeTeam.h"
#include "DlgChat.h"
#include "EC_GameRun.h"
#include "EC_GameUIMan.h"
#include "EC_GameSession.h"
#include "EC_HostPlayer.h"
#include "EC_Team.h"
#include "EC_Manager.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgTeamContext, CDlgBase)

AUI_ON_COMMAND("OpenT",				OnCommand_OpenT)
AUI_ON_COMMAND("GroupLeave",		OnCommand_GroupLeave)
AUI_ON_COMMAND("GroupDisband",		OnCommand_GroupLeave)
AUI_ON_COMMAND("Whisper",			OnCommand_Whisper)
AUI_ON_COMMAND("MakeLeader",		OnCommand_MakeLeader)
AUI_ON_COMMAND("Kick",				OnCommand_Kick)
AUI_ON_COMMAND("Trade",				OnCommand_Trade)
AUI_ON_COMMAND("Follow",			OnCommand_Follow)
AUI_ON_COMMAND("Assist",			OnCommand_Assist)
AUI_ON_COMMAND("Summon",			OnCommand_Summon)

AUI_END_COMMAND_MAP()

CDlgTeamContext::CDlgTeamContext()
{
}

CDlgTeamContext::~CDlgTeamContext()
{
}

void CDlgTeamContext::OnCommand_OpenT(const char * szCommand)
{
	GetGameUIMan()->UpdateTeam(true);
	GetGameUIMan()->m_pDlgArrangeTeam->Show(true);
}

void CDlgTeamContext::OnCommand_GroupLeave(const char * szCommand)
{
	CECTeam *pTeam = GetHostPlayer()->GetTeam();
	if( !pTeam ) return;

	if( pTeam->GetLeaderID() == GetHostPlayer()->GetCharacterID() )
	{
		GetGameUIMan()->MessageBox("Game_TeamDisband", GetStringFromTable(235),
			MB_OKCANCEL, A3DCOLORRGBA(255, 255, 255, 160));
	}
	else
		GetGameSession()->c2s_CmdTeamLeaveParty();
}

void CDlgTeamContext::OnCommand_Whisper(const char * szCommand)
{
	int idTarget = GetData();
	const ACHAR *pszName = GetGameRun()->GetPlayerName(idTarget, true);

	CDlgChat *pChat = GetGameUIMan()->m_pDlgChat;
	pChat->SwitchToPrivateChat(pszName);
}

void CDlgTeamContext::OnCommand_MakeLeader(const char * szCommand)
{
	GetGameSession()->c2s_CmdTeamChangeLeader(GetData());
}

void CDlgTeamContext::OnCommand_Kick(const char * szCommand)
{
	GetGameSession()->c2s_CmdTeamKickMember(GetData());
}

void CDlgTeamContext::OnCommand_Trade(const char * szCommand)
{
	if( GetGameSession()->trade_Start(GetData()) )
		GetGameUIMan()->AddChatMessage(GetStringFromTable(212), GP_CHAT_MISC);
}

void CDlgTeamContext::OnCommand_Follow(const char * szCommand)
{
	GetGameRun()->PostMessage(MSG_HST_FOLLOW, MAN_PLAYER, 0, GetData());
}

void CDlgTeamContext::OnCommand_Assist(const char * szCommand)
{
	GetGameSession()->c2s_CmdTeamAssistSel(GetData());
}

void CDlgTeamContext::OnCommand_Summon(const char * szCommand)
{
	GetHostPlayer()->SummonPlayer(GetData());
}