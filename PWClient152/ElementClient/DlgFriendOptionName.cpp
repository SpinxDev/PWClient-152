// File		: DlgFriendOptionName.cpp
// Creator	: Xiao Zhou
// Date		: 2005/8/18

#include "AUICommon.h"
#include "AUIEditBox.h"
#include "DlgFriendOptionName.h"
#include "DlgFriendList.h"
#include "DlgFriendHistory.h"
#include "DlgFriendGroup.h"
#include "DlgChannelChat.h"
#include "DlgChat.h"
#include "DlgInputName.h"

#include "EC_UIConfigs.h"
#include "EC_GameSession.h"
#include "EC_GameUIMan.h"
#include "EC_Friend.h"
#include "EC_HostPlayer.h"
#include "Network\\gnetdef.h"
#include "A3DDevice.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgFriendOptionName, CDlgBase)

AUI_ON_COMMAND("Whisper",		OnCommandWhisper)
AUI_ON_COMMAND("Invite",		OnCommandInvite)
AUI_ON_COMMAND("Chat",			OnCommandChat)
AUI_ON_COMMAND("History",		OnCommandHistory)
AUI_ON_COMMAND("Update",		OnCommandUpdate)
AUI_ON_COMMAND("MoveGroup",		OnCommandMoveGroup)
AUI_ON_COMMAND("DeleteFriend",	OnCommandDeleteFriend)
AUI_ON_COMMAND("InviteChannel",	OnCommandInviteChannel)
AUI_ON_COMMAND("JoinChannel",	OnCommandJoinChannel)
AUI_ON_COMMAND("CopyName",		OnCommandCopyName)
AUI_ON_COMMAND("ChangeRemarks",	OnCommandChangeRemarks)

AUI_END_COMMAND_MAP()

bool CDlgFriendOptionName::GetSelectedFriendName(ACString &strFriendName){
	return GetGameUIMan()->m_pDlgFriendList->GetSelectedFriendName(strFriendName);
}
bool CDlgFriendOptionName::GetSelectedFriendID(int &idPlayer){	
	return GetGameUIMan()->m_pDlgFriendList->GetSelectedFriendID(idPlayer);
}
bool CDlgFriendOptionName::GetSelectedFriendGroup(int &idGroup){
	return GetGameUIMan()->m_pDlgFriendList->GetSelectedFriendGroup(idGroup);
}
bool CDlgFriendOptionName::GetSelectedFriendRemarks(ACString &strRemarks){
	return GetGameUIMan()->m_pDlgFriendList->GetSelectedFriendRemarks(strRemarks);
}

void CDlgFriendOptionName::OnCommandWhisper(const char *szCommand)
{
	ACString strFriendName;
	if (!GetSelectedFriendName(strFriendName)){
		return;
	}	
	CDlgChat *pChat = GetGameUIMan()->m_pDlgChat;
	pChat->SwitchToPrivateChat(strFriendName);
}

void CDlgFriendOptionName::OnCommandInvite(const char *szCommand)
{
	int idPlayer = 0;
	ACString strFriendName;
	if (!GetSelectedFriendID(idPlayer) ||
		!GetSelectedFriendName(strFriendName)){
		return;
	}
	GetGameSession()->c2s_CmdTeamInvite(idPlayer);
	GetGameUIMan()->AddChatMessage(GetGameUIMan()->GetStringFromTable(203), GP_CHAT_MISC);
}

void CDlgFriendOptionName::OnCommandChat(const char *szCommand)
{
	int idPlayer = 0;
	ACString strFriendName;
	if (!GetSelectedFriendID(idPlayer) ||
		!GetSelectedFriendName(strFriendName)){
		return;
	}
	
	int i;
	char szName[40];
	PAUIDIALOG pDlgChat;
	
	for( i = 0; i < CECGAMEUIMAN_MAX_CHATS; i++ )
	{
		sprintf(szName, "Win_FriendChat%d", i);
		pDlgChat = GetGameUIMan()->GetDialog(szName);
		if( (int)pDlgChat->GetData() == idPlayer )
		{
			pDlgChat->Show(true);

			return;
		}
	}
	for( i = 0; i < CECGAMEUIMAN_MAX_CHATS; i++ )
	{
		sprintf(szName, "Win_FriendChat%d", i);
		pDlgChat = GetGameUIMan()->GetDialog(szName);
		if( (int)pDlgChat->GetData() == 0 )
		{
			ACString strText;
			ACHAR szUser[40];
			PAUIOBJECT pName = pDlgChat->GetDlgItem("Txt_ToWho");
			PAUIEDITBOX pEdit = dynamic_cast<PAUIEDITBOX>(pDlgChat->GetDlgItem("Txt_Content"));
			PAUITEXTAREA pText = dynamic_cast<PAUITEXTAREA>(pDlgChat->GetDlgItem("Txt_Chat"));

			AUI_ConvertChatString(strFriendName, szUser);
			strText.Format(GetGameUIMan()->GetStringFromTable(548), szUser, GetStringFromTable(574));
			pName->SetText(strText);

			pText->SetText(_AL(""));
			
			pEdit->SetText(_AL(""));
			pEdit->SetIsAutoReturn(true);
			
			pDlgChat->SetData(idPlayer);
			pDlgChat->SetText(strFriendName);
			pDlgChat->Show(true);
			pDlgChat->ChangeFocus(pEdit);
			
			break;
		}
	}
	if( i >= CECGAMEUIMAN_MAX_CHATS )	// No free chat window.
	{
		GetGameUIMan()->MessageBox("", GetGameUIMan()->GetStringFromTable(540),
			MB_OK, A3DCOLORRGBA(255, 255, 255, 160));
	}
}

void CDlgFriendOptionName::OnCommandHistory(const char *szCommand)
{
	int idPlayer = 0;
	if (!GetSelectedFriendID(idPlayer)){
		return;
	}	
	CDlgFriendHistory* pDlgHis = GetGameUIMan()->m_pDlgFriendHistory;
	GetGameUIMan()->m_pDlgFriendList->BuildFriendList(pDlgHis);
	GetGameUIMan()->m_pDlgFriendHistory->BuildChatHistory(idPlayer);
	pDlgHis->SetData(idPlayer);
	pDlgHis->Show(true);
}

void CDlgFriendOptionName::OnCommandMoveGroup(const char *szCommand)
{
	int idPlayer = 0;
	int idGroup(0);
	if (!GetSelectedFriendID(idPlayer) ||
		!GetSelectedFriendGroup(idGroup)){
		return;
	}
	CECFriendMan::GROUP *pGroup = NULL;
	CDlgFriendGroup* pDlgGroup = GetGameUIMan()->m_pDlgFriendGroup;
	PAUICOMBOBOX pCombo = pDlgGroup->m_pComboGroup;
	CECFriendMan *pMan = GetHostPlayer()->GetFriendMan();

	pCombo->ResetContent();
	for(int i = 0; i < pMan->GetGroupNum(); i++ )
	{
		pGroup = pMan->GetGroupByIndex(i);
		pCombo->AddString(pGroup->strName);
		pCombo->SetItemData(i, pGroup->iGroup);
		if( idGroup == pGroup->iGroup )
			pCombo->SetCurSel(i);
	}

	pDlgGroup->Show(true);
	pDlgGroup->SetData(idPlayer);
	pDlgGroup->SetDataPtr((void *)idGroup);
}

void CDlgFriendOptionName::OnCommandDeleteFriend(const char *szCommand)
{
	GetGameUIMan()->MessageBox("Game_DelFriend", GetGameUIMan()->GetStringFromTable(538),
		MB_OKCANCEL, A3DCOLORRGBA(255, 255, 255, 160));
}

void CDlgFriendOptionName::OnCommandInviteChannel(const char *szCommand)
{
	int idPlayer = 0;
	if (!GetSelectedFriendID(idPlayer)){
		return;
	}
	int idRoom = GetGameUIMan()->m_pDlgChannelChat->GetRoomID();
	if( idRoom != 0 )
	{
		GetGameSession()->chatroom_Invite(idRoom, idPlayer);
		GetGameUIMan()->AddChatMessage(GetStringFromTable(633), GP_CHAT_MISC);
	}
}

void CDlgFriendOptionName::OnCommandJoinChannel(const char *szCommand)
{
	ACString strFriendName;
	if (!GetSelectedFriendName(strFriendName)){
		return;
	}	
	if( GetGameUIMan()->m_pDlgChannelChat->GetRoomID() != 0 &&
		GetGameUIMan()->m_pDlgChannelChat->IsCreator() )
	{
		PAUIDIALOG pMsgBox;
		m_pAUIManager->MessageBox("Game_ChannelJoinAsCreator", GetStringFromTable(654), MB_YESNO,
			A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
		pMsgBox->SetText(strFriendName);
	}
	else
		GetGameSession()->chatroom_Join(strFriendName, _AL(""));
}

void CDlgFriendOptionName::OnCommandUpdate(const char *szCommand)
{
	int idPlayer = 0;
	ACString strFriendName;
	if (!GetSelectedFriendID(idPlayer) ||
		!GetSelectedFriendName(strFriendName)){
		return;
	}
	GetGameSession()->SendPrivateChatData(strFriendName, _AL("R"), GNET::CHANNEL_USERINFO, idPlayer);
}

void CDlgFriendOptionName::OnCommandCopyName(const char * szCommand)
{
	ACString strFriendName;
	if (!GetSelectedFriendName(strFriendName)){
		return;
	}
	
	if( OpenClipboard(m_pA3DDevice->GetDeviceWnd()) )
	{
		const ACHAR * szName = strFriendName;
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

void CDlgFriendOptionName::OnCommandChangeRemarks(const char *szCommand)
{
	int idPlayer = 0;
	ACString strRemarks;
	if (!GetSelectedFriendID(idPlayer) ||
		!GetSelectedFriendRemarks(strRemarks)){
		return;
	}
	CDlgInputName* pDlg = GetGameUIMan()->m_pDlgInputName;
	pDlg->ShowForUsage(CDlgInputName::INPUTNAME_FRIEND_CHANGE_REMARKS,
		strRemarks, CECUIConfig::Instance().GetGameUI().nMaxFriendRemarksNameLength,
		(void *)idPlayer, "int");
}
