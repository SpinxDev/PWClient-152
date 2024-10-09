// Filename	: DlgQuickAction.cpp
// Creator	: Tom Zhou
// Date		: October 10, 2005

#include "DlgQuickAction.h"
#include "DlgReportToGM.h"
#include "DlgChat.h"
#include "DlgChannelChat.h"
#include "EC_GameRun.h"
#include "EC_GameSession.h"
#include "EC_GameUIMan.h"
#include "EC_Friend.h"
#include "EC_World.h"
#include "EC_ManPlayer.h"
#include "EC_HostPlayer.h"
#include "EC_Shortcut.h"
#include "EC_ShortcutSet.h"
#include "EC_ElsePlayer.h"
#include "EC_Team.h"
#include "EC_FixedMsg.h"
#include "Network\\ids.hxx"
#include "AUICommon.h"
#include "AUIEditBox.h"
#include "AUIComboBox.h"
#include "DlgAskHelpToGM.h"
#include "A3DDevice.h"
#include "DlgAskHelpToGM2.h"
#include "EC_UIConfigs.h"
#include "EC_CrossServer.h"
#include "EC_TimeSafeChecker.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgQuickAction, CDlgBase)

AUI_ON_COMMAND("SecretMsg",		OnCommand_SecretMsg)
AUI_ON_COMMAND("Report",		OnCommand_Report)
AUI_ON_COMMAND("Extra",			OnCommand_Extra)
AUI_ON_COMMAND("Trade",			OnCommand_Trade)
AUI_ON_COMMAND("WhisperChat",	OnCommand_Whisper)
AUI_ON_COMMAND("AddFriend",		OnCommand_AddFriend)
AUI_ON_COMMAND("InviteGroup",	OnCommand_InviteGroup)
AUI_ON_COMMAND("InviteGuild",	OnCommand_InviteGuild)
AUI_ON_COMMAND("Follow",		OnCommand_Follow)
AUI_ON_COMMAND("JoinChannel",	OnCommand_JoinChannel)
AUI_ON_COMMAND("InviteChannel",	OnCommand_InviteChannel)
AUI_ON_COMMAND("Bind",			OnCommand_Bind)
AUI_ON_COMMAND("Duel",			OnCommand_Duel)
AUI_ON_COMMAND("EPEquip",		OnCommand_EPEquip)
AUI_ON_COMMAND("CopyName",		OnCommand_CopyName)
AUI_ON_COMMAND("ProfView",		OnCommand_ProfView)
AUI_ON_COMMAND("IDCANCEL",		OnCommand_CANCEL)
AUI_ON_COMMAND("shop",			OnCommand_shop)

AUI_END_COMMAND_MAP()

CDlgQuickAction::CDlgQuickAction()
{
}

CDlgQuickAction::~CDlgQuickAction()
{
}

bool CDlgQuickAction::OnInitDialog()
{
	m_pInviteGuild = (PAUISTILLIMAGEBUTTON)GetDlgItem("InviteGuild");
	m_pInviteChannel = (PAUISTILLIMAGEBUTTON)GetDlgItem("InviteChannel");
	m_pInviteGroup = (PAUISTILLIMAGEBUTTON)GetDlgItem("InviteGroup");
	m_pDuel = (PAUISTILLIMAGEBUTTON)GetDlgItem("Duel");
	m_pBind = (PAUISTILLIMAGEBUTTON)GetDlgItem("Bind");
	m_pTrade = (PAUISTILLIMAGEBUTTON)GetDlgItem("Trade");
	m_pshop = (PAUISTILLIMAGEBUTTON)GetDlgItem("shop");
	m_pExtra = (PAUISTILLIMAGEBUTTON)GetDlgItem("Extra");
	DisableForCrossServer("Extra");
	return true;
}

void CDlgQuickAction::OnShowDialog()
{
	m_pInviteGuild->Enable(false);
	m_pInviteChannel->Enable(false);
	m_pDuel->Enable(false);
	m_pBind->Enable(false);
	m_pTrade->Enable(false);
	m_pshop->Enable(false);
}

void CDlgQuickAction::OnCommand_SecretMsg(const char * szCommand)
{
	Show(false);

	int idTarget = GetData();
	const ACHAR *pszName = GetGameRun()->GetPlayerName(idTarget, true);

	CDlgChat *pChat = GetGameUIMan()->m_pDlgChat;
	pChat->SwitchToPrivateChat(pszName);
}

void CDlgQuickAction::OnCommand_Report(const char * szCommand)
{
	if (CECUIConfig::Instance().GetGameUI().bEnableReportPlayerSpeakToGM)
	{
		CDlgAskHelpToGM2 *pDlg = GetGameUIMan()->m_pDlgAskHelpToGM2;				
#ifndef _DEBUG
		if(CECTimeSafeChecker::ElapsedTimeFor(pDlg->GetData()) / 1000 <= 5 * 60 )
		{
			GetGameUIMan()->AddChatMessage(GetStringFromTable(532), GP_CHAT_MISC);
			Show(false);
			return;
		}
#endif
		
		const ACHAR *pszName = GetGameRun()->GetPlayerName(GetData(), true);
		ACHAR szText[40];
		AUI_ConvertChatString(pszName, szText);
		pDlg->ShowWithHelpMessage(GetData(), pszName);

	}
	else
	{
		CDlgAskHelpToGM *pDlg = GetGameUIMan()->m_pDlgAskHelpToGM;				
#ifndef _DEBUG
		if( CECTimeSafeChecker::ElapsedTimeFor(pDlg->GetData()) / 1000 <= 5 * 60 )
		{
			GetGameUIMan()->AddChatMessage(GetStringFromTable(532), GP_CHAT_MISC);
			Show(false);
			return;
		}
#endif		
		const ACHAR *pszName = GetGameRun()->GetPlayerName(GetData(), true);
		ACHAR szText[40];
		AUI_ConvertChatString(pszName, szText);
		
		ACString msg;
		msg.Format(GetStringFromTable(6007), szText);
		pDlg->ShowWithHelpMessage(CDlgAskHelpToGM::GM_REPORT_PLUGIN, msg, GetData(), pszName);
	}


	Show(false);
}

void CDlgQuickAction::OnCommand_Extra(const char * szCommand)
{
	PAUIDIALOG pDlg = GetGameUIMan()->GetDialog("Win_Blacklist");
	if( pDlg )
	{
		PAUIOBJECT pEdit = pDlg->GetDlgItem("Txt_Name");
		if( pEdit )
		{
			ACString szTemp = pEdit->GetText();
			pEdit->SetText(GetGameRun()->GetPlayerName(GetData(), true));
			pDlg->OnCommand("add");
			pEdit->SetText(szTemp);
		}
	}
	Show(false);
}

void CDlgQuickAction::OnCommand_Trade(const char * szCommand)
{
	Show(false);
	if( GetGameSession()->trade_Start(GetData()) )
		GetGameUIMan()->AddChatMessage(GetStringFromTable(212), GP_CHAT_MISC);
}

void CDlgQuickAction::OnCommand_AddFriend(const char * szCommand)
{
	int idTarget = GetData();
	const ACHAR *pszName = GetGameRun()->GetPlayerName(idTarget, true);
	CECFriendMan::FRIEND* pFriend = GetHostPlayer()->GetFriendMan()->GetFriendByID(idTarget);

	//	In friend list, one player ID may have two names, so check name here !
	if( pFriend && !a_strcmp(pszName, pFriend->GetName()) )
		GetGameUIMan()->AddChatMessage(GetStringFromTable(565), GP_CHAT_MISC);
	else
	{
		GetGameSession()->friend_Add(idTarget, pszName);
		GetGameUIMan()->AddChatMessage(GetStringFromTable(533), GP_CHAT_MISC);
	}

	Show(false);
}

void CDlgQuickAction::OnCommand_InviteGroup(const char * szCommand)
{
	int idTarget = GetData();
	CECElsePlayer *pPlayer = GetWorld()->GetPlayerMan()->GetElsePlayer(idTarget);

	const ACHAR *pszName = GetGameRun()->GetPlayerName(idTarget, true);
	if( pPlayer && pPlayer->GetInTeamState() != 0 )
	{
		if( GetHostPlayer()->GetTeam() != NULL )
			GetGameUIMan()->AddChatMessage(GetStringFromTable(687), GP_CHAT_MISC);
		else
		{
			GetGameSession()->c2s_CmdTeamAskJoin(idTarget);
			GetGameUIMan()->AddChatMessage(GetStringFromTable(685), GP_CHAT_MISC);
		}
	}
	else
	{
		GetGameSession()->c2s_CmdTeamInvite(idTarget);
		GetGameUIMan()->AddChatMessage(GetStringFromTable(203), GP_CHAT_MISC);
	}

	Show(false);
}

void CDlgQuickAction::OnCommand_InviteGuild(const char * szCommand)
{
	int idTarget = GetData();
	const ACHAR *pszName = GetGameRun()->GetPlayerName(idTarget, true);

	GetGameSession()->faction_accept_join(idTarget);
	GetGameUIMan()->AddChatMessage(GetStringFromTable(601), GP_CHAT_MISC);

	Show(false);
}

void CDlgQuickAction::OnCommand_Follow(const char * szCommand)
{
	int idTarget = GetData();
	const ACHAR *pszName = GetGameRun()->GetPlayerName(idTarget, true);

	GetGameRun()->PostMessage(MSG_HST_FOLLOW, MAN_PLAYER, 0, idTarget);

	Show(false);
}

void CDlgQuickAction::OnCommand_JoinChannel(const char * szCommand)
{
	int idTarget = GetData();
	const ACHAR *pszName = GetGameRun()->GetPlayerName(idTarget, true);
	Show(false);
	if( GetGameUIMan()->m_pDlgChannelChat->GetRoomID() != 0 &&
		GetGameUIMan()->m_pDlgChannelChat->IsCreator() )
	{
		PAUIDIALOG pMsgBox;
		m_pAUIManager->MessageBox("Game_ChannelJoinAsCreator", GetStringFromTable(654), MB_YESNO,
			A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
		pMsgBox->SetText(pszName);
	}
	else
		GetGameSession()->chatroom_Join(pszName, _AL(""));

}

void CDlgQuickAction::OnCommand_InviteChannel(const char * szCommand)
{
	int idTarget = GetData();
	int idRoom = GetGameUIMan()->m_pDlgChannelChat->GetRoomID();
	if( idRoom != 0 )
	{
		GetGameSession()->chatroom_Invite(idRoom, idTarget);
		GetGameUIMan()->AddChatMessage(GetStringFromTable(633), GP_CHAT_MISC);
	}

	Show(false);
}

void CDlgQuickAction::OnCommand_Bind(const char * szCommand)
{
	GetHostPlayer()->CmdBindBuddy(GetData());
	Show(false);
}

void CDlgQuickAction::OnCommand_Duel(const char * szCommand)
{
	GetHostPlayer()->CmdAskDuel(GetData());
	Show(false);
}

void CDlgQuickAction::OnCommand_EPEquip(const char * szCommand)
{
	GetHostPlayer()->CmdViewOtherEquips(GetData());
	Show(false);
}


void CDlgQuickAction::OnCommand_ProfView(const char * szCommand)
{
	GetHostPlayer()->CmdViewOtherInfo(GetData());
/*	GetGameUIMan()->GetDialog("Win_ProfView")->Show(true);*/
	Show(false);
}

void CDlgQuickAction::OnCommand_CANCEL(const char * szCommand)
{
	Show(false);
}

void CDlgQuickAction::OnCommand_CopyName(const char * szCommand)
{
	if( OpenClipboard(m_pA3DDevice->GetDeviceWnd()) )
	{
		int idTarget = GetData();
		const ACHAR * szName = GetGameRun()->GetPlayerName(idTarget, true);	
		HLOCAL hLocal = GlobalAlloc(LMEM_ZEROINIT, a_strlen(szName) * 2 + 2);
		if( hLocal )
		{
			ACHAR *pszName = (ACHAR *)GlobalLock(hLocal);
			a_strcpy(pszName, szName);
			EmptyClipboard();
#ifdef UNICODE
			SetClipboardData(CF_UNICODETEXT, pszName);
#else
			SetClipboardData(CF_TEXT, pszName);
#endif
			GlobalUnlock(hLocal);
		}

		CloseClipboard();
	}
	Show(false);
}

void CDlgQuickAction::OnCommand_Whisper(const char * szCommand)
{
	Show(false);

	CDlgChat * pDlgWhisper = GetGameUIMan()->m_pDlgChatWhisper;
	if( pDlgWhisper->IsShow() && pDlgWhisper->GetData() == GetData() ) return;

	pDlgWhisper->Show(true);
	pDlgWhisper->SetData(GetData());
	pDlgWhisper->GetDlgItem("DEFAULT_Txt_Speech")->SetText(_AL(""));

	PAUIOBJECT pText = pDlgWhisper->GetDlgItem("Txt_Chat");
	pText->SetText(_AL(""));

	pDlgWhisper->RefreshWhisperPlayerName();
}

void CDlgQuickAction::OnCommand_shop(const char * szCommand)
{
	//	打开对方的摆摊

	Show(false);

	//	检查自身状态
	if (GetHostPlayer()->IsJumping())
		return;

	//	检查对方的状态
	int idTarget = GetData();
	CECElsePlayer *pPlayer = GetWorld()->GetPlayerMan()->GetElsePlayer(idTarget);
	if (!pPlayer || !pPlayer->GetBoothState())
		return;

	//	检查离对方距离
	const A3DVECTOR3& vPlayerPos = pPlayer->GetServerPos();
	if (!GetHostPlayer()->CanTouchTarget(vPlayerPos, 0.0f, 3))
	{
		GetGameRun()->AddFixedMessage(FIXMSG_TARGETISFAR);
		return;
	}

	//	发送协议
	GetGameSession()->c2s_CmdNPCSevHello(idTarget);
}

void CDlgQuickAction::OnTick()
{
	int idTarget = GetData();
	CECElsePlayer *pPlayer = GetWorld()->GetPlayerMan()->GetElsePlayer(idTarget);

	int idFRole = GetHostPlayer()->GetFRoleID();
	int idFaction = GetHostPlayer()->GetFactionID();
	if( pPlayer && 
		(pPlayer->GetFactionID() > 0 || idFaction == 0 || idFRole == GNET::_R_MEMBER) )
		m_pInviteGuild->Enable(false);
	else
		m_pInviteGuild->Enable(true);

	if( GetGameUIMan()->m_pDlgChannelChat->GetRoomID() != 0 )
		m_pInviteChannel->Enable(true);
	else
		m_pInviteChannel->Enable(false);

	if( pPlayer && 
		pPlayer->GetGender() != GetHostPlayer()->GetGender() && 
		pPlayer->CanBindBuddy() &&
		GetHostPlayer()->CanBindBuddy())
	{
		m_pBind->Enable(true);
	}
	else
		m_pBind->Enable(false);
	
	if( !pPlayer )
	{
//		m_pInviteGroup->SetText(GetStringFromTable(802));
		m_pInviteGroup->Enable(true);
	}
	else if( pPlayer->GetInTeamState() == 0 )
	{
//		m_pInviteGroup->SetText(GetStringFromTable(802));
		if( GetHostPlayer()->GetTeam() == NULL ||
			GetHostPlayer()->GetTeam()->GetLeaderID() == GetHostPlayer()->GetCharacterID() )
			m_pInviteGroup->Enable(true);
		else
			m_pInviteGroup->Enable(false);
	}
	else
	{
//		m_pInviteGroup->SetText(GetStringFromTable(803));
		if( GetHostPlayer()->GetTeam() == NULL )
			m_pInviteGroup->Enable(true);
		else
			m_pInviteGroup->Enable(false);
	}

	m_pDuel->Enable(pPlayer && !GetHostPlayer()->IsInDuel() && !pPlayer->IsInDuel() && !GetHostPlayer()->IsInvisible());
	m_pTrade->Enable(pPlayer && !GetHostPlayer()->IsTrading() && !GetHostPlayer()->IsInvisible());
	m_pshop->Enable(pPlayer && pPlayer->GetBoothState());
}
