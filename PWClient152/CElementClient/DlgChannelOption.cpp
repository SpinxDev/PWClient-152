// File		: DlgChannelOption.cpp
// Creator	: Xiao Zhou
// Date		: 2005/10/25

#include "DlgChannelOption.h"
#include "EC_GameUIMan.h"
#include "AUICommon.h"
#include "DlgChat.h"
#include "DlgChannelChat.h"
#include "EC_GameRun.h"
#include "EC_HostPlayer.h"
#include "EC_Friend.h"
#include "EC_GameSession.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgChannelOption, CDlgBase)

AUI_ON_COMMAND("Whisper",		OnCommandWhisper)
AUI_ON_COMMAND("FriendInvite",	OnCommandFriendInvite)
AUI_ON_COMMAND("GroupInvite",	OnCommandGroupInvite)
AUI_ON_COMMAND("Kick",			OnCommandKick)

AUI_END_COMMAND_MAP()

CDlgChannelOption::CDlgChannelOption()
{
}

CDlgChannelOption::~CDlgChannelOption()
{
}

void CDlgChannelOption::OnCommandWhisper(const char *szCommand)
{
	Show(false);

	CDlgChat *pDlgWhisper = GetGameUIMan()->m_pDlgChatWhisper;
	if( pDlgWhisper->IsShow() && pDlgWhisper->GetData() == GetData() ) return;

	pDlgWhisper->Show(true);
	pDlgWhisper->SetData(GetData());
	pDlgWhisper->GetDlgItem("DEFAULT_Txt_Speech")->SetText(_AL(""));

	PAUITEXTAREA pText = dynamic_cast<PAUITEXTAREA>(pDlgWhisper->GetDlgItem("Txt_Chat"));
	pText->SetText(_AL(""));

	pDlgWhisper->RefreshWhisperPlayerName();
}

void CDlgChannelOption::OnCommandFriendInvite(const char *szCommand)
{
	int idTarget = GetData();
	const ACHAR *pszName = GetGameRun()->GetPlayerName(idTarget, true);

	if( GetHostPlayer()->GetFriendMan()->GetFriendByID(idTarget) )
		GetGameUIMan()->AddChatMessage(GetStringFromTable(565), GP_CHAT_MISC);
	else
	{
		GetGameSession()->friend_Add(idTarget, pszName);
		GetGameUIMan()->AddChatMessage(GetStringFromTable(533), GP_CHAT_MISC);
	}

	Show(false);
}

void CDlgChannelOption::OnCommandGroupInvite(const char *szCommand)
{
	int idTarget = GetData();
	const ACHAR *pszName = GetGameRun()->GetPlayerName(idTarget, true);

	GetGameSession()->c2s_CmdTeamInvite(idTarget);
	GetGameUIMan()->AddChatMessage(GetStringFromTable(203), GP_CHAT_MISC);

	Show(false);
}

void CDlgChannelOption::OnCommandKick(const char *szCommand)
{
	int idTarget = GetData();
	int idRoom = GetGameUIMan()->m_pDlgChannelChat->GetRoomID();
	if( idRoom != 0)
		GetGameSession()->chatroom_Expel(idRoom, idTarget);
	Show(false);
}
