// Filename	: DlgChannelChat.cpp
// Creator	: Xiao Zhou
// Date		: 2005/10/21

#include "DlgChannelChat.h"
#include "DlgMinimizeBar.h"
#include "EC_GameUIMan.h"
#include "EC_GameSession.h"
#include "EC_HostPlayer.h"
#include "EC_GameRun.h"
#include "EC_Utility.h"
#include "DlgChannelPW.h"
#include "DlgChat.h"
#include "DlgInfo.h"
#include "DlgInputName.h"
#include "chatroomcreate_re.hpp"
#include "chatroomjoin_re.hpp"
#include "chatroominvite.hpp"
#include "chatroominvite_re.hpp"
#include "chatroomleave.hpp"
#include "chatroomexpel.hpp"
#include "chatroomspeak.hpp"
#include "AUIDef.h"
#include "AUICommon.h"
#include "EC_UIManager.h"

AUI_BEGIN_COMMAND_MAP(CDlgChannelChat, CDlgBase)

AUI_ON_COMMAND("IDCANCEL",	OnCommandCancel)
AUI_ON_COMMAND("send",		OnCommandSend)
AUI_ON_COMMAND("leave",		OnCommandLeave)
AUI_ON_COMMAND("invite",	OnCommandInvite)
AUI_ON_COMMAND("popface",	OnCommandPopface)
AUI_ON_COMMAND("minimize",	OnCommandMinimize)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgChannelChat, CDlgBase)

AUI_ON_EVENT("Txt_Content",		WM_SYSCHAR,		OnEventSysChar)
AUI_ON_EVENT("Txt_Content",		WM_KEYDOWN,		OnEventKeyDown)
AUI_ON_EVENT("List_ChatMember",	WM_LBUTTONUP,	OnEventLButtonUp_ChatMember)
AUI_ON_EVENT("List_ChatMember",	WM_RBUTTONUP,	OnEventRButtonUp_ChatMember)

AUI_END_EVENT_MAP()

const ACHAR * CDlgChannelChat::m_pszColor[CDLGCHANNELCHAT_COLOR_MAX] =
{
	_AL("^FFFFFF"),		// CDLGCHANNELCHAT_COLOR_PUBLIC
	_AL("^FF4AB0"),		// CDLGCHANNELCHAT_COLOR_PRIVATE
	_AL("^00CC1D"),		// CDLGCHANNELCHAT_COLOR_SPEAKTO
	_AL("^9AA6FF"),		// CDLGCHANNELCHAT_COLOR_MISC
};

CDlgChannelChat::CDlgChannelChat()
{
	m_pTxt_RoomID = NULL;
	m_pTxt_Subject = NULL;
	m_pTxt_Owner = NULL;
	m_pTxt_Capacity = NULL;
	m_pTxt_SpeakTo = NULL;
	m_pList_ChatMember = NULL;
	m_pTxt_Content = NULL;
	m_pTxt_Chat = NULL;
	m_pChk_Whisper = NULL;
	m_nRoomId = 0;
	m_bCreator = false;
}

CDlgChannelChat::~CDlgChannelChat()
{
}

bool CDlgChannelChat::OnInitDialog()
{
	DDX_Control("Txt_RoomID", m_pTxt_RoomID);
	DDX_Control("Txt_Subject", m_pTxt_Subject);
	DDX_Control("Txt_Owner", m_pTxt_Owner);
	DDX_Control("Txt_Capacity", m_pTxt_Capacity);
	DDX_Control("Txt_SpeakTo", m_pTxt_SpeakTo);
	DDX_Control("List_ChatMember", m_pList_ChatMember);
	DDX_Control("Txt_Content", m_pTxt_Content);
	DDX_Control("Txt_Chat", m_pTxt_Chat);
	DDX_Control("Chk_Whisper", m_pChk_Whisper);
	m_pTxt_Chat->SetMaxLines(200);
	m_pTxt_Content->SetIsAutoReturn(true);
	
	return true;
}

void CDlgChannelChat::OnCommandCancel(const char* szCommand)
{
	GetGameUIMan()->RespawnMessage();
}

void CDlgChannelChat::OnCommandMinimize(const char *szCommand)
{
	GetGameUIMan()->GetMiniBarMgr()->MinimizeDialog(this, 
		CECMiniBarMgr::BarInfo(m_pTxt_Subject->GetText(), A3DCOLORRGB(255, 255, 255)));
}

void CDlgChannelChat::OnCommandLeave(const char* szCommand)
{
	if( IsCreator() )
	{
		PAUIDIALOG pMsgBox;
		m_pAUIManager->MessageBox("Game_ChannelClose", GetStringFromTable(654), MB_YESNO,
			A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
		pMsgBox->SetData(GetRoomID());
		return;
	}
	GetGameSession()->chatroom_Leave(m_nRoomId);
	Show(false);
	m_nRoomId = 0;
	m_bCreator = false;
}

void CDlgChannelChat::OnCommandSend(const char* szCommand)
{
	if( glb_IsTextEmpty(m_pTxt_Content))
		return;

	int i;
	ACString szText;
	ACString strText = m_pTxt_Content->GetText();
	GetGameRun()->GetUIManager()->FilterBadWords(strText);
	for( i = 0; i < m_pList_ChatMember->GetCount(); i++ )
	{
		if( (DWORD)m_idSpeakTo == m_pList_ChatMember->GetItemData(i, 0) )
		{
			if( m_idSpeakTo != 0 && m_pChk_Whisper->IsChecked() )
			{
				szText.Format(GetStringFromTable(629), 
					GetGameRun()->GetPlayerName(m_idSpeakTo, true), strText);
				AddChannelChatMessage(szText, CDLGCHANNELCHAT_COLOR_PRIVATE);
				GetGameSession()->chatroom_Speak(m_nRoomId, m_idSpeakTo, strText);
			}
			else
				PublicSpeak(m_idSpeakTo, strText);
			m_pTxt_Content->SetText(_AL(""));
			ChangeFocus(m_pTxt_Content);
			return;
		}
	}
	szText.Format(GetStringFromTable(631), m_pTxt_SpeakTo->GetText());
	AddChannelChatMessage(szText);
	ChangeFocus(m_pTxt_Content);
}

void CDlgChannelChat::OnCommandInvite(const char* szCommand)
{
	GetGameUIMan()->m_pDlgInputName->ShowForUsage(CDlgInputName::INPUTNAME_CHANNEL_INVITE, NULL, -1, (void*)m_nRoomId, "int");
}

void CDlgChannelChat::OnCommandPopface(const char* szCommand)
{
	PAUIOBJECT pEdit = m_pTxt_Content;
	PAUIDIALOG pShow = GetGameUIMan()->GetDialog("Win_Popface");
	
	pShow->Show(!pShow->IsShow());
	pShow->AlignTo(this,
		AUIDIALOG_ALIGN_INSIDE, AUIDIALOG_ALIGN_LEFT,
		AUIDIALOG_ALIGN_OUTSIDE, AUIDIALOG_ALIGN_BOTTOM);
	pShow->SetDataPtr(this,"ptr_AUIDialog");

	GetGameUIMan()->BringWindowToTop(this);
	ChangeFocus(pEdit);
	
	pShow = GetGameUIMan()->GetDialog("Win_Popface01");
	pShow->Show(false);
	pShow = GetGameUIMan()->GetDialog("Win_Popface02");
	pShow->Show(false);
}

void CDlgChannelChat::OnEventSysChar(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	if( (TCHAR(wParam) == 's' || TCHAR(wParam) == 'S') )
		OnCommandSend("send");
}

void CDlgChannelChat::OnEventKeyDown(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	if( int(wParam) == VK_RETURN && AUI_PRESS(VK_CONTROL) ) 
		OnCommandSend("send");
	
	if (wParam == VK_ESCAPE)
		ChangeFocus(NULL);
}

void CDlgChannelChat::OnEventLButtonUp_ChatMember(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	int nSel = m_pList_ChatMember->GetCurSel();
	if( nSel < m_pList_ChatMember->GetCount() &&
		GetHostPlayer()->GetCharacterID() != (int)m_pList_ChatMember->GetItemData(nSel, 0) )
	{
		m_idSpeakTo = m_pList_ChatMember->GetItemData(nSel, 0);
		m_pTxt_SpeakTo->SetText(m_pList_ChatMember->GetText(nSel));

		// NOTICE: change align property by code may 
		// cause problem when theme changing.
		//
		int nWidth, nHeight, nLines;
		AUI_GetTextRect(GetGameUIMan()->GetDefaultFont(), m_pTxt_SpeakTo->GetText(), nWidth, nHeight, nLines);
		if( nWidth <= m_pTxt_SpeakTo->GetSize().cx )
			m_pTxt_SpeakTo->SetAlign(AUIFRAME_ALIGN_CENTER);
		else
			m_pTxt_SpeakTo->SetAlign(AUIFRAME_ALIGN_LEFT);

		m_pChk_Whisper->Enable(m_idSpeakTo != 0);
		m_pChk_Whisper->Check(m_idSpeakTo != 0);
	}
	ChangeFocus(m_pTxt_Content);
}

void CDlgChannelChat::OnEventRButtonUp_ChatMember(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	OnEventLButtonUp_ChatMember(NULL, NULL, NULL);
	int nSel = m_pList_ChatMember->GetCurSel();
	int idCreator = m_pList_ChatMember->GetItemData(1, 0);
	int idSel = m_pList_ChatMember->GetItemData(nSel, 0);
	int idHostPlayer = GetHostPlayer()->GetCharacterID();
	if( nSel > 0 && nSel < m_pList_ChatMember->GetCount() && idSel != idHostPlayer )
	{
		PAUIDIALOG pMenu = (AUIDialog *)GetGameUIMan()->m_pDlgChannelOption;
		pMenu->SetData(idSel);
		A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
		int x = GET_X_LPARAM(lParam) - p->X;
		int y = GET_Y_LPARAM(lParam) - p->Y;
		pMenu->SetPosEx(x, y);
		
		if( idHostPlayer == idCreator )
			pMenu->GetDlgItem("Kick")->Enable(true);
		else
			pMenu->GetDlgItem("Kick")->Enable(false);

		pMenu->Show(true);
	}
}

void CDlgChannelChat::OnShowDialog()
{
	CDlgBase::OnShowDialog();

	GetGameUIMan()->GetMiniBarMgr()->RestoreDialog(this);
	ChangeFocus(m_pTxt_Content);
}

void CDlgChannelChat::OpenNewChatRoom()
{
	m_pList_ChatMember->ResetContent();
	m_pList_ChatMember->AddString(GetStringFromTable(782));
	m_pList_ChatMember->SetItemData(0, 0);
	m_pTxt_Content->SetText(_AL(""));
	m_pTxt_Chat->SetText(_AL(""));
	m_idSpeakTo = 0;
	m_pTxt_SpeakTo->SetText(GetStringFromTable(782));
	m_pTxt_SpeakTo->SetAlign(AUIFRAME_ALIGN_CENTER);
	m_pChk_Whisper->Check(false);
	m_pChk_Whisper->Enable(false);
}

void CDlgChannelChat::ChannelAction(int idAction, void *pData)
{
	PAUIDIALOG pMsgBox;
	ACString szText;
	if( idAction == CHANNEL_ACTION_CREATE_RE )
	{
		ChatRoomCreate_Re *p = (ChatRoomCreate_Re *)pData;
		if( p->retcode == ERR_SUCCESS)
		{
			m_nRoomId = p->roomid;
			m_bCreator = true;
			OpenNewChatRoom();
			Show(true);
			m_pList_ChatMember->AddString(GetHostPlayer()->GetName());
			m_pList_ChatMember->SetItemData(
				m_pList_ChatMember->GetCount() - 1 , GetHostPlayer()->GetCharacterID());
			m_nNumber = 1;
			m_nCapacity = p->capacity;
			szText.Format(_AL("%d"), m_nRoomId);
			m_pTxt_RoomID->SetText(szText);
			szText.Format(_AL("%d/%d"), 1, p->capacity);
			m_pTxt_Capacity->SetText(szText);
			ACString strSubject((const ACHAR*)p->subject.begin(), p->subject.size() / sizeof (ACHAR));
			m_pTxt_Subject->SetText(strSubject);
			m_pTxt_Owner->SetText(GetHostPlayer()->GetName());
		}
		else if( p->retcode == ERR_CHAT_CREATE_FAILED)
		{
			GetGameUIMan()->MessageBox("", GetStringFromTable(617),
				MB_OK, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
			pMsgBox->SetLife(3);
		}
		else if( p->retcode == ERR_CHAT_INVALID_SUBJECT)
		{
			GetGameUIMan()->MessageBox("", GetStringFromTable(618),
				MB_OK, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
			pMsgBox->SetLife(3);
		}
	}
	else if( idAction == CHANNEL_ACTION_JOIN_RE )
	{
		ChatRoomJoin_Re *p = (ChatRoomJoin_Re *)pData;
		if( p->retcode == ERR_SUCCESS )
		{
			if( p->roleid == GetHostPlayer()->GetCharacterID() )
			{
				m_nRoomId = p->roomid;
				m_bCreator = false;
				OpenNewChatRoom();
				Show(true);
				((AUIDialog *)GetGameUIMan()->m_pDlgChannelJoin)->Show(false);

				GNET::GRoomDetail detail = p->detail;
				m_nCapacity = detail.capacity;
				m_nNumber = detail.members.size();

				int i;
				for( i = 0; i < m_nNumber; i++ )
				{
					GNET::GChatMember member = detail.members[i];
					ACString strName((const ACHAR*)member.name.begin(), member.name.size() / sizeof (ACHAR));
					GetGameRun()->AddPlayerName(member.roleid, strName);
					m_pList_ChatMember->AddString(strName);
					m_pList_ChatMember->SetItemData(m_pList_ChatMember->GetCount() - 1, member.roleid);
				}
				m_pList_ChatMember->AddString(GetHostPlayer()->GetName());
				m_pList_ChatMember->SetItemData(m_pList_ChatMember->GetCount() - 1, GetHostPlayer()->GetCharacterID());
				szText.Format(_AL("%d"), m_nRoomId);
				m_pTxt_RoomID->SetText(szText);
				ACString strSubject((const ACHAR*)detail.subject.begin(), detail.subject.size() / sizeof (ACHAR));
				m_pTxt_Subject->SetText(strSubject);
				ACString strName((const ACHAR*)detail.members[0].name.begin(), detail.members[0].name.size() / sizeof (ACHAR));
				m_pTxt_Owner->SetText(strName);
				szText.Format(_AL("%d/%d"), m_nNumber, m_nCapacity);
				m_pTxt_Capacity->SetText(szText);
			}
			else
			{
				ACString strName((const ACHAR*)p->name.begin(), p->name.size() / sizeof (ACHAR));
				GetGameRun()->AddPlayerName(p->roleid, strName);
				m_pList_ChatMember->AddString(strName);
				m_pList_ChatMember->SetItemData(m_pList_ChatMember->GetCount() - 1 , p->roleid);
				szText.Format(GetStringFromTable(624), strName);
				AddChannelChatMessage(szText);
				m_nNumber++;
				szText.Format(_AL("%d/%d"), m_nNumber, m_nCapacity);
				m_pTxt_Capacity->SetText(szText);
			}
		}
		else if( p->retcode == ERR_CHAT_INVALID_PASSWORD )
		{
			GetGameUIMan()->m_pDlgChannelPW->SetData(p->roomid);
			GetGameUIMan()->m_pDlgChannelPW->Show(true, true);
		}
		else if( p->retcode == ERR_CHAT_ROOM_NOT_FOUND )
		{
			GetGameUIMan()->MessageBox("", GetStringFromTable(619),
				MB_OK, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
			pMsgBox->SetLife(3);
		}
		else if( p->retcode == ERR_CHAT_JOIN_REFUSED )
		{
			GetGameUIMan()->MessageBox("", GetStringFromTable(620),
				MB_OK, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
			pMsgBox->SetLife(3);
		}
		else if( p->retcode == ERR_CHAT_ROOM_FULL )
		{
			GetGameUIMan()->MessageBox("", GetStringFromTable(622),
				MB_OK, A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
			pMsgBox->SetLife(3);
		}
	}
	else if( idAction == CHANNEL_ACTION_INVITE )
	{
		ChatRoomInvite *p = (ChatRoomInvite *)pData;
		if( GetGameUIMan()->PlayerIsBlack(p->inviter) )
			return;
		
		ACString strName((const ACHAR*)p->name.begin(), p->name.size() / sizeof (ACHAR));
		ACString strSubject((const ACHAR*)p->subject.begin(), p->subject.size() / sizeof (ACHAR));
		ACString strPW((const ACHAR*)p->password.begin(), p->password.size() / sizeof (ACHAR));
		ACString strText;
		strText.Format(GetStringFromTable(632), strName, strSubject);
		ACString strText1;
		strText1.Format(_AL("%s%s %d"), strText, strPW, strPW.GetLength());

		GetGameUIMan()->AddInformation(CDlgInfo::INFO_SYSTEM, "Game_InviteChannel", 
			strText1, 0xFFFFFFF, p->roomid, p->inviter, 0);
	}
	else if( idAction == CHANNEL_ACTION_INVITE_RE )
	{
		ChatRoomInvite_Re *p = (ChatRoomInvite_Re *)pData;
		if( p->retcode == ERR_CHAT_INVITE_REFUSED )
		{
			const ACHAR *szName = GetGameRun()->GetPlayerName(p->invitee, true);
			szText.Format(GetStringFromTable(623), szName);
			GetGameUIMan()->AddChatMessage(szText, GP_CHAT_MISC);
		}
	}
	else if( idAction == CHANNEL_ACTION_LEAVE )
	{
		ChatRoomLeave *p = (ChatRoomLeave *)pData;
		if( p->roleid == 0 )
		{
			GetGameUIMan()->GetMiniBarMgr()->RestoreDialog(this);
			Show(false);
			m_nRoomId = 0;
			m_bCreator = false;
			szText.Format(GetStringFromTable(648), m_pTxt_Subject->GetText());
			GetGameUIMan()->AddChatMessage(szText, GP_CHAT_MISC);
		}
		else
		{
			int i;
			for( i = 1; i < m_pList_ChatMember->GetCount(); i++ )
				if( (int)m_pList_ChatMember->GetItemData(i) == p->roleid )
				{
					m_nNumber--;
					szText.Format(_AL("%d/%d"), m_nNumber, m_nCapacity);
					m_pTxt_Capacity->SetText(szText);

					szText.Format(GetStringFromTable(625), m_pList_ChatMember->GetText(i));
					AddChannelChatMessage(szText);
					m_pList_ChatMember->DeleteString(i);
					break;
				}
		}
	}
	else if( idAction == CHANNEL_ACTION_EXPEL )
	{
		ChatRoomExpel *p = (ChatRoomExpel *)pData;
		if( p->roleid == GetHostPlayer()->GetCharacterID() )
		{
			GetGameUIMan()->GetMiniBarMgr()->RestoreDialog(this);
			Show(false);
			m_nRoomId = 0;
			m_bCreator = false;
			szText.Format(GetStringFromTable(627), m_pTxt_Subject->GetText());
			GetGameUIMan()->AddChatMessage(szText, GP_CHAT_MISC);
		}
		else
		{
			int i;
			for( i = 1; i < m_pList_ChatMember->GetCount(); i++ )
				if( (int)m_pList_ChatMember->GetItemData(i) == p->roleid )
				{
					m_nNumber--;
					szText.Format(_AL("%d/%d"), m_nNumber, m_nCapacity);
					m_pTxt_Capacity->SetText(szText);
					
					szText.Format(GetStringFromTable(626), m_pList_ChatMember->GetText(i));
					AddChannelChatMessage(szText);
					m_pList_ChatMember->DeleteString(i);
					break;
				}
		}
	}
	else if( idAction == CHANNEL_ACTION_SPEAK )
	{
		ChatRoomSpeak *p = (ChatRoomSpeak *)pData;
		ACString strMessage((const ACHAR*)p->message.begin(), p->message.size() / sizeof (ACHAR));
		if( p->dst == 0 )
		{
			int nPos = strMessage.Find(_AL(" "));
			int idSpeakTo = strMessage.Left(nPos).ToInt();
			strMessage.CutLeft(nPos + 1);
			if( idSpeakTo == 0)
			{
				szText.Format(GetStringFromTable(628), 
					GetGameRun()->GetPlayerName(p->src, true), strMessage );
				AddChannelChatMessage(szText, CDLGCHANNELCHAT_COLOR_PUBLIC);
			}
			else if( p->src == GetHostPlayer()->GetCharacterID() )
			{
				szText.Format(GetStringFromTable(645), 
					GetGameRun()->GetPlayerName(idSpeakTo, true), strMessage );
				AddChannelChatMessage(szText, CDLGCHANNELCHAT_COLOR_SPEAKTO);
			}
			else if( idSpeakTo == GetHostPlayer()->GetCharacterID() )
			{
				szText.Format(GetStringFromTable(646), 
					GetGameRun()->GetPlayerName(p->src, true), strMessage );
				AddChannelChatMessage(szText, CDLGCHANNELCHAT_COLOR_SPEAKTO);
			}
			else
			{
				szText.Format(GetStringFromTable(647), 
					GetGameRun()->GetPlayerName(p->src, true), 
					GetGameRun()->GetPlayerName(idSpeakTo, true),
					strMessage );
				AddChannelChatMessage(szText, CDLGCHANNELCHAT_COLOR_PUBLIC);
			}
		}
		else
		{
			szText.Format(GetStringFromTable(630), 
				GetGameRun()->GetPlayerName(p->src, true), strMessage);
			AddChannelChatMessage(szText, CDLGCHANNELCHAT_COLOR_PRIVATE);
		}
		if( !IsShow() )
			GetGameUIMan()->GetMiniBarMgr()->FlashDialog(this);
	}
}

void CDlgChannelChat::AddChannelChatMessage(const ACHAR *szText, int idType)
{
	ACString strText = GetGameUIMan()->FilterInvalidTags(szText, true);
	strText = GetGameUIMan()->ConvertChatString(strText);
	if( idType )
		m_pTxt_Chat->AppendText(m_pszColor[idType]);
	m_pTxt_Chat->AppendText(strText);
	if( idType )
		m_pTxt_Chat->AppendText(m_pszColor[CDLGCHANNELCHAT_COLOR_PUBLIC]);
	m_pTxt_Chat->ScrollToTop();
	m_pTxt_Chat->ScrollToBottom();
}

int CDlgChannelChat::GetRoomID()
{
	return m_nRoomId;
}

bool CDlgChannelChat::IsCreator()
{
	return m_bCreator;
}

void CDlgChannelChat::PublicSpeak(int idSpeakTo, const ACHAR *szText)
{
	ACString szTemp;
	szTemp.Format(_AL("%d %s"), idSpeakTo, szText);
	GetGameSession()->chatroom_Speak(m_nRoomId, 0, szTemp);
}

void CDlgChannelChat::Resize(A3DRECT rcOld, A3DRECT rcNew)
{
	CDlgBase::Resize(rcOld, rcNew);
	if (m_pTxt_Chat)
	{
		ACString strText = m_pTxt_Chat->GetText();
		m_pTxt_Chat->SetText(strText);
		m_pTxt_Chat->ScrollToTop();
		m_pTxt_Chat->ScrollToBottom();
	}
}
