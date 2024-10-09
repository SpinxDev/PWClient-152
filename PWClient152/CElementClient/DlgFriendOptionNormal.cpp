// File		: DlgFriendOptionNormal.cpp
// Creator	: Xiao Zhou
// Date		: 2005/8/18

#include "AUIEditBox.h"
#include "AUIStillImagebutton.h"
#include "DlgFriendOptionNormal.h"
#include "EC_GameUIMan.h"
#include "DlgFriendList.h"
#include "DlgInputName.h"
#include "Network\\gnetdef.h"
#include "EC_GPDataType.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgFriendOptionNormal, CDlgBase)

AUI_ON_COMMAND("IDCANCEL",	OnCommandCANCAL)
AUI_ON_COMMAND("AddFriend",	OnCommandAddFriend)
AUI_ON_COMMAND("AddGroup",	OnCommandAddGroup)
AUI_ON_COMMAND("SetAfk",	OnCommandSetAfk)
AUI_ON_COMMAND("OnOffline",	OnCommandOnOffline)

AUI_END_COMMAND_MAP()

CDlgFriendOptionNormal::CDlgFriendOptionNormal()
{
}

CDlgFriendOptionNormal::~CDlgFriendOptionNormal()
{
}

void CDlgFriendOptionNormal::OnCommandCANCAL(const char *szCommand)
{
	Show(false);
}

void CDlgFriendOptionNormal::OnCommandAddFriend(const char *szCommand)
{
	GetGameUIMan()->m_pDlgFriendList->OnCommandAdd("add");
}

void CDlgFriendOptionNormal::OnCommandAddGroup(const char *szCommand)
{
	GetGameUIMan()->m_pDlgInputName->ShowForUsage(CDlgInputName::INPUTNAME_ADD_GROUP);
}

void CDlgFriendOptionNormal::OnCommandSetAfk(const char *szCommand)
{
	GetGameUIMan()->m_bAutoReply = !GetGameUIMan()->m_bAutoReply;
	((PAUISTILLIMAGEBUTTON)GetDlgItem(szCommand))->SetPushed(GetGameUIMan()->m_bAutoReply);
	GetGameUIMan()->AddChatMessage(GetGameUIMan()->GetStringFromTable(
		GetGameUIMan()->m_bAutoReply ? 553 : 554), GP_CHAT_MISC);
}

void CDlgFriendOptionNormal::OnCommandOnOffline(const char *szCommand)
{
	GetGameUIMan()->m_bOnlineNotify = !GetGameUIMan()->m_bOnlineNotify;
	((PAUISTILLIMAGEBUTTON)GetDlgItem(szCommand))->SetPushed(GetGameUIMan()->m_bOnlineNotify);
	GetGameUIMan()->AddChatMessage(GetGameUIMan()->GetStringFromTable(
		GetGameUIMan()->m_bOnlineNotify ? 551 : 552), GP_CHAT_MISC);
}
