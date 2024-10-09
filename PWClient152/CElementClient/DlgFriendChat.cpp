// File		: DlgFriendChat.cpp
// Creator	: Xiao Zhou
// Date		: 2005/8/18

#include "DlgFriendChat.h"
#include "DlgFriendList.h"
#include "DlgFriendHistory.h"
#include "DlgMinimizeBar.h"
#include "EC_GameUIMan.h"
#include "EC_GameSession.h"
#include "EC_HostPlayer.h"
#include "EC_GameRun.h"
#include "EC_Friend.h"
#include "time.h"
#include "Network\\gnetdef.h"
#include "AUIDef.h"
#include "AUICommon.h"
#include "EC_UIManager.h"
#include "EC_IvtrItem.h"
#include "EC_Inventory.h"
#include "EC_GameTalk.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgFriendChat, CDlgBase)

AUI_ON_COMMAND("IDCANCEL",	OnCommandCANCAL)
AUI_ON_COMMAND("send",		OnCommandSend)
AUI_ON_COMMAND("popface",	OnCommandPopface)
AUI_ON_COMMAND("history",	OnCommandHistory)
AUI_ON_COMMAND("save",		OnCommandSave)
AUI_ON_COMMAND("delall",	OnCommandDelall)
AUI_ON_COMMAND("minimize",	OnCommandMinimize)

AUI_END_COMMAND_MAP()


AUI_BEGIN_EVENT_MAP(CDlgFriendChat, CDlgBase)

AUI_ON_EVENT("Txt_Content",	WM_SYSCHAR,		OnEventSysChar)
AUI_ON_EVENT("Txt_Content",	WM_KEYDOWN,		OnEventKeyDown)
AUI_ON_EVENT("Txt_Chat",	WM_LBUTTONDOWN,	OnEventLButtonDown_Txt_Chat)
AUI_ON_EVENT("Txt_Chat",	WM_LBUTTONUP,	OnEventLButtonUp_Txt_Chat)
AUI_ON_EVENT("Txt_Chat",	WM_RBUTTONUP,	OnEventRButtonUp_Txt_Chat)
AUI_ON_EVENT("Txt_Chat",	WM_MOUSEMOVE,	OnEventMouseMove_Txt_Chat)

AUI_END_EVENT_MAP()

CDlgFriendChat::LinkedChatMessages CDlgFriendChat::m_vecLinkedChatMsg;
int CDlgFriendChat::m_nMsgIndex = 1;

CDlgFriendChat::LinkedChatMessages & CDlgFriendChat::GetLinkedChatMsg(){
	return m_vecLinkedChatMsg;
}

CDlgFriendChat::CDlgFriendChat()
{
	m_pTxtContent = NULL;
	m_pTxtChat = NULL;
	m_pChkSave = NULL;
}

CDlgFriendChat::~CDlgFriendChat()
{
	LinkedChatMessages &vecChatMsgs = GetLinkedChatMsg();
	for (size_t i = 0; i < vecChatMsgs.size(); i++)
		delete vecChatMsgs[i].pItem;
	vecChatMsgs.clear();
}

void CDlgFriendChat::DoDataExchange(bool bSave)
{
	CDlgBase::DoDataExchange(bSave);

	DDX_Control("Txt_Content", m_pTxtContent);
	DDX_Control("Txt_Chat", m_pTxtChat);
	DDX_Control("Chk_Save", m_pChkSave);
}

void CDlgFriendChat::OnCommandCANCAL(const char *szCommand)
{
	Show(false);
	SetData(0);
}

void CDlgFriendChat::OnCommandMinimize(const char *szCommand)
{
	CECFriendMan *pMan = GetHostPlayer()->GetFriendMan();
	CECFriendMan::FRIEND *pFriend = pMan->GetFriendByID(GetData());

	ACString szSubject;
	if( pFriend )
		szSubject = pFriend->GetName();
	else
		szSubject = GetGameRun()->GetPlayerName(GetData(), true);

	GetGameUIMan()->GetMiniBarMgr()->MinimizeDialog(this, 
		CECMiniBarMgr::BarInfo(szSubject, A3DCOLORRGB(0, 255, 0)));
}

void CDlgFriendChat::OnCommandSend(const char *szCommand)
{
	CECGameSession *pSession = GetGameSession();
	int idPlayer = GetData();
	ACString strText = m_pTxtContent->GetText();

	if( strText.GetLength() > 0 )
	{
		ACHAR szText[512];
		ACString strTarget = GetText();
		CECHostPlayer *pHost = GetHostPlayer();
		
		// 查找 LINK 物品
		int nPack(-1), nSlot(-1);
		CECIvtrItem *pItem = NULL;
		if (m_pTxtContent->HasIvtrItem())
		{
			nPack = m_pTxtContent->GetIvtrItemPack();
			nSlot = m_pTxtContent->GetIvtrItemIndex();
			if (nPack >= 0)
			{
				CECInventory *pPack = GetHostPlayer()->GetPack(nPack);
				if (pPack)
				{
					if (nSlot >= 0)
					{
						pItem = pPack->GetItem(nSlot);
					}
				}
			}
		}

		// 处理聊天内容
		AUI_ConvertChatString(strText, szText, false);
		GetGameRun()->GetUIManager()->FilterBadWords(strText);

		// 添加到显示
		AddMessageToDisplay(idPlayer, pHost->GetName(), szText, pItem);

		// 发送消息
		GNET::PRIVATE_CHANNEL channel = CECGameTalk::Instance().GetChannelToSend(idPlayer, false, false);
		pSession->SendPrivateChatData(strTarget, strText, channel, idPlayer, nPack, nSlot);
		ChangeFocus(m_pTxtContent);
		m_pTxtContent->SetText(_AL(""));
	}
}

void CDlgFriendChat::AddMessageToDisplay(int idOther, const ACHAR *pszSayer, const ACHAR *pszMsg, CECIvtrItem *pItem)
{
	// idOther 聊天对象
	// pszSaider 谁说的这话
	// pItem 不负责内存管理

	// 转换文字内容
	ACHAR szName[40];
	ACHAR szMsg[512];
	AUI_ConvertChatString(pszMsg, szMsg, false);
	AUI_ConvertChatString(pszSayer, szName);

	// 保存历史到文件
	if (GetGameUIMan()->m_bSaveHistory)
		GetGameUIMan()->AddHistory(idOther, time(NULL), szName, szMsg);

	// 添加到显示
	if( a_strlen(m_pTxtChat->GetText()) > 0 )
		m_pTxtChat->AppendText(_AL("\r"));
	m_pTxtChat->AppendText(szName);
	m_pTxtChat->AppendText(_AL(": "));
	if (pItem)
	{
		// 将物品添加到显示
		ACString strName;
		A3DCOLOR clrName;
		GetGameUIMan()->TransformNameColor(pItem, strName, clrName);
		m_pTxtChat->AppendText(szMsg, m_nMsgIndex, strName, clrName);		
		
		// 保存LINK物品的历史供点击
		if (pItem)
		{
			LINKED_MSG msg;
			msg.pItem = pItem->Clone();
			msg.nMsgIndex = m_nMsgIndex++;
			
			LinkedChatMessages &vecChatMsgs = GetLinkedChatMsg();
			if (vecChatMsgs.size() >= 200)
			{
				LINKED_MSG &msgDelete = *(vecChatMsgs.begin());
				delete msgDelete.pItem;
				msgDelete.pItem = NULL;
				vecChatMsgs.erase(vecChatMsgs.begin());
			}
			vecChatMsgs.push_back(msg);
		}
	}
	else
	{
		m_pTxtChat->AppendText(szMsg);
	}

	// 显示当前内容
	m_pTxtChat->ScrollToTop();
	m_pTxtChat->ScrollToBottom();
}

void CDlgFriendChat::OnCommandPopface(const char *szCommand)
{
	PAUIDIALOG pShow = GetGameUIMan()->GetDialog("Win_Popface");
	
	pShow->Show(!pShow->IsShow());
	pShow->AlignTo(this, AUIDIALOG_ALIGN_INSIDE, AUIDIALOG_ALIGN_LEFT,
		AUIDIALOG_ALIGN_OUTSIDE, AUIDIALOG_ALIGN_BOTTOM);
	pShow->SetDataPtr(this,"ptr_AUIDialog");

	pShow = GetGameUIMan()->GetDialog("Win_Popface01");
	pShow->Show(false);
	pShow = GetGameUIMan()->GetDialog("Win_Popface02");
	pShow->Show(false);
}

void CDlgFriendChat::OnCommandHistory(const char *szCommand)
{
	int idPlayer = GetData();

	GetGameUIMan()->m_pDlgFriendList->BuildFriendList(GetGameUIMan()->m_pDlgFriendHistory);
	GetGameUIMan()->m_pDlgFriendHistory->BuildChatHistory(idPlayer);
	GetGameUIMan()->m_pDlgFriendHistory->SetData(idPlayer);
	GetGameUIMan()->m_pDlgFriendHistory->Show(true);
}

void CDlgFriendChat::OnCommandSave(const char *szCommand)
{
	int i;
	char szName[40];
	PAUICHECKBOX pCheck = m_pChkSave;

	GetGameUIMan()->m_bSaveHistory = m_pChkSave->IsChecked();
	for( i = 0; i < CECGAMEUIMAN_MAX_CHATS; i++ )
	{
		sprintf(szName, "Win_FriendChat%d", i);
		pCheck = (PAUICHECKBOX)GetGameUIMan()->GetDialog(szName)->GetDlgItem("Chk_Save");
		pCheck->Check(GetGameUIMan()->m_bSaveHistory);
	}
}

void CDlgFriendChat::OnCommandDelall(const char *szCommand)
{
	GetGameUIMan()->MessageBox("Game_DelHistory", GetStringFromTable(566),
		MB_OKCANCEL, A3DCOLORRGBA(255, 255, 255, 160));
}

void CDlgFriendChat::OnEventSysChar(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	if( (TCHAR(wParam) == 's' || TCHAR(wParam) == 'S') )
		OnCommandSend("send");
}

void CDlgFriendChat::OnEventKeyDown(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	if( int(wParam) == VK_RETURN && AUI_PRESS(VK_CONTROL) ) 
		OnCommandSend("send");
	
	if (wParam == VK_ESCAPE)
		ChangeFocus(NULL);
}

void CDlgFriendChat::OnShowDialog()
{
	CDlgBase::OnShowDialog();
	
	GetGameUIMan()->GetMiniBarMgr()->RestoreDialog(this);
	ChangeFocus(m_pTxtContent);
}

void CDlgFriendChat::Resize(A3DRECT rcOld, A3DRECT rcNew)
{
	CDlgBase::Resize(rcOld, rcNew);
	if (m_pTxtChat)
	{
		ACString strText = m_pTxtChat->GetText();
		m_pTxtChat->SetText(strText);
		m_pTxtChat->ScrollToTop();
		m_pTxtChat->ScrollToBottom();
	}
}

void CDlgFriendChat::OnEventMouseMove_Txt_Chat(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	GetGameUIMan()->m_pDlgChat->OnEventMouseMove_Txt_Content(wParam, lParam, pObj);
}

void CDlgFriendChat::OnEventLButtonDown_Txt_Chat(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
	int x = GET_X_LPARAM(lParam) - p->X;
	int y = GET_Y_LPARAM(lParam) - p->Y;

	CDlgChat* pChat = GetGameUIMan()->m_pDlgChat;

	bool bClickUserName = false;
	AUITEXTAREA_NAME_LINK Link;
	bool bClickChatPart = pChat->GetNameLinkMouseOn(x, y, pObj, &Link);
	if( bClickChatPart && Link.strName.GetLength() != 0 )
	{
		pChat->SwitchToPrivateChat(Link.strName);
		bClickUserName = true;
	}
	else
	{
		AUITEXTAREA_EDITBOX_ITEM Item;
		Item.m_pItem = NULL;
		if( pChat->GetItemLinkItemOn(x, y, pObj, &Item) && Item.m_pItem != NULL )
		{
			bClickUserName = true;
			if( Item.m_pItem->GetType() == enumEIIvtrlItem )
			{
				LinkedChatMessages &vecChatMsgs = GetLinkedChatMsg();
				for (size_t i = 0; i < vecChatMsgs.size(); i++)
				{
					if (vecChatMsgs[i].nMsgIndex == Item.m_pItem->GetMsgIndex() )
					{
						CECIvtrItem *pItem = vecChatMsgs[i].pItem;
						GetGameUIMan()->OnLinkItemClicked(pItem, wParam, lParam);
						break;
					}
				}
			}
		}
	}

	if (bClickChatPart && !bClickUserName)
		GetGameUIMan()->ContinueDealMessage();
}

void CDlgFriendChat::OnEventLButtonUp_Txt_Chat(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	GetGameUIMan()->m_pDlgChat->OnEventLButtonUp_Txt_Content(wParam, lParam, pObj);
}

void CDlgFriendChat::OnEventRButtonUp_Txt_Chat(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	GetGameUIMan()->m_pDlgChat->OnEventRButtonUp_Txt_Content(wParam, lParam, pObj);
}