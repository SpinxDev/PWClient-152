// Filename	: DlgChannalJoin.cpp
// Creator	: Xiao Zhou
// Date		: 2005/10/21

#include "DlgChannelJoin.h"
#include "DlgChannelChat.h"
#include "EC_GameUIMan.h"
#include "EC_GameSession.h"
#include "chatroomlist_re.hpp"
#include "DlgChannelPW.h"

AUI_BEGIN_COMMAND_MAP(CDlgChannelJoin, CDlgBase)

AUI_ON_COMMAND("joinascreator",	OnCommandJoinAsCreator)
AUI_ON_COMMAND("joinasnumber",	OnCommandJoinAsNumber)
AUI_ON_COMMAND("jump",			OnCommandJump)
AUI_ON_COMMAND("last",			OnCommandLast)
AUI_ON_COMMAND("refresh",		OnCommandRefresh)
AUI_ON_COMMAND("next",			OnCommandNext)
AUI_ON_COMMAND("join",			OnCommandJoin)

AUI_END_COMMAND_MAP()

CDlgChannelJoin::CDlgChannelJoin()
{
	m_pTxt_RoomTotal = NULL;
	m_pLst_AllChannel = NULL;
	m_pTxt_CreatorName = NULL;
	m_pTxt_Number = NULL;
	m_pTxt_RoomIDStart = NULL;
}

CDlgChannelJoin::~CDlgChannelJoin()
{
}

bool CDlgChannelJoin::OnInitDialog()
{
	DDX_Control("Txt_RoomTotal", m_pTxt_RoomTotal);
	DDX_Control("Lst_AllChannel", m_pLst_AllChannel);
	DDX_Control("Default_Txt_CreatorName", m_pTxt_CreatorName);
	DDX_Control("Txt_Number", m_pTxt_Number);
	DDX_Control("Txt_RoomIDStart", m_pTxt_RoomIDStart);
	m_pTxt_Number->SetIsNumberOnly(true);
	m_pTxt_RoomIDStart->SetIsNumberOnly(true);
	
	return true;
}

void CDlgChannelJoin::OnCommandJoinAsCreator(const char* szCommand)
{
	if( GetGameUIMan()->m_pDlgChannelChat->GetRoomID() != 0 &&
		GetGameUIMan()->m_pDlgChannelChat->IsCreator() )
	{
		PAUIDIALOG pMsgBox;
		m_pAUIManager->MessageBox("Game_ChannelJoinAsCreator", GetStringFromTable(654), MB_YESNO,
			A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
		pMsgBox->SetText(m_pTxt_CreatorName->GetText());
	}
	else
		GetGameSession()->chatroom_Join(m_pTxt_CreatorName->GetText(), _AL(""));
}

void CDlgChannelJoin::OnCommandJoinAsNumber(const char* szCommand)
{
	ACString szRoomID = m_pTxt_Number->GetText();
	int nRoomId = szRoomID.ToInt();
	if( nRoomId > 0 )
		if( GetGameUIMan()->m_pDlgChannelChat->GetRoomID() != 0 &&
			GetGameUIMan()->m_pDlgChannelChat->IsCreator() )
		{
			PAUIDIALOG pMsgBox;
			m_pAUIManager->MessageBox("Game_ChannelJoinAsNumber", GetStringFromTable(654), MB_YESNO,
				A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
			pMsgBox->SetData(nRoomId);
		}
		else
			GetGameSession()->chatroom_Join(nRoomId, _AL(""));
}

void CDlgChannelJoin::OnCommandJump(const char* szCommand)
{
	ACString szText = m_pTxt_RoomIDStart->GetText();
	int nStart = szText.ToInt();
	if( nStart > 0 )
		nStart --;
	if( nStart < 0 )
	{
		nStart = 0;
		m_pTxt_RoomIDStart->SetText(_AL("1"));
	}
	GetGameSession()->chatroom_List(nStart, 0);
}

void CDlgChannelJoin::OnCommandLast(const char* szCommand)
{
	if( m_pLst_AllChannel->GetCount() > 0 )
	{
		int nRoomID = m_pLst_AllChannel->GetItemData(0, 0) - 1;
		if( nRoomID > 0 )
			GetGameSession()->chatroom_List(nRoomID, 1);
	}
	else
		GetGameSession()->chatroom_List(65535, 1);
}

void CDlgChannelJoin::OnCommandNext(const char* szCommand)
{
	if( m_pLst_AllChannel->GetCount() > 0 )
	{
		int nRoomID = m_pLst_AllChannel->GetItemData(m_pLst_AllChannel->GetCount() - 1, 0);
		GetGameSession()->chatroom_List(nRoomID, 0);
	}
	else
		GetGameSession()->chatroom_List(0, 0);
}

void CDlgChannelJoin::OnCommandRefresh(const char* szCommand)
{
	if( m_pLst_AllChannel->GetCount() > 0 )
		GetGameSession()->chatroom_List(m_pLst_AllChannel->GetItemData(0, 0), 0);
}


void CDlgChannelJoin::OnCommandJoin(const char* szCommand)
{
	int nSel = m_pLst_AllChannel->GetCurSel();
	if( nSel < m_pLst_AllChannel->GetCount() )
	{
		if( GetGameUIMan()->m_pDlgChannelChat->GetRoomID() != 0 &&
			GetGameUIMan()->m_pDlgChannelChat->IsCreator() )
		{
			PAUIDIALOG pMsgBox;
			m_pAUIManager->MessageBox("Game_ChannelJoinAsNumber", GetStringFromTable(654), MB_YESNO,
				A3DCOLORRGBA(255, 255, 255, 160), &pMsgBox);
			pMsgBox->SetData(m_pLst_AllChannel->GetItemData(nSel, 0));
		}
		else if( m_pLst_AllChannel->GetItemData(nSel, 1) )
		{
			GetGameUIMan()->m_pDlgChannelPW->SetData(m_pLst_AllChannel->GetItemData(nSel, 0), "int");
			GetGameUIMan()->m_pDlgChannelPW->Show(true, true);
		}
		else
			GetGameSession()->chatroom_Join(m_pLst_AllChannel->GetItemData(nSel, 0), _AL(""));
	}
}

void CDlgChannelJoin::OnShowDialog()
{
	CDlgBase::OnShowDialog();

	m_pTxt_CreatorName->SetText(_AL(""));
	m_pTxt_RoomIDStart->SetText(_AL("1"));
	m_pTxt_RoomTotal->SetText(_AL("0"));
	GetGameSession()->chatroom_List(0, 0);
}

void CDlgChannelJoin::BuildChannelList(void* pChannelList)
{
	ChatRoomList_Re *p = (ChatRoomList_Re *)pChannelList;

	m_pLst_AllChannel->ResetContent();
	DWORD i;
	ACString szText;
	szText.Format(_AL("%d"), p->sum);
	m_pTxt_RoomTotal->SetText(szText);
	for( i = 0; i < p->rooms.size(); i++ )
	{
		GNET::GChatRoom room = p->rooms[i];
		ACString strSubject((const ACHAR*)room.subject.begin(), room.subject.size() / sizeof (ACHAR));
		ACString strOwner((const ACHAR*)room.owner.begin(), room.owner.size() / sizeof (ACHAR));
		ACString strPW;
		if( (room.status & 1) == 1 )
			strPW = GetStringFromTable(783);
		else
			strPW = GetStringFromTable(784);
		szText.Format(_AL("%d\t%s\t%s\t%s\t%d/%d"),room.roomid, strPW, strSubject, strOwner, room.number, room.capacity);
		m_pLst_AllChannel->AddString(szText);
		m_pLst_AllChannel->SetItemData(i, room.roomid, 0);
		m_pLst_AllChannel->SetItemData(i, room.status & 1, 1);
	}
}