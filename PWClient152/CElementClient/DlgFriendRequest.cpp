// File		: DlgFriendRequest.cpp
// Creator	: Xiao Zhou
// Date		: 2005/8/18

#include "DlgFriendRequest.h"
#include "DlgBlackList.h"

#include "AUIEditBox.h"

#include "EC_GameUIMan.h"
#include "EC_GameRun.h"
#include "EC_GameSession.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgFriendRequest, CDlgBase)

AUI_ON_COMMAND("IDCANCEL",	OnCommandCANCAL)
AUI_ON_COMMAND("accept",	OnCommandAccept)
AUI_ON_COMMAND("refuse",	OnCommandRefuse)
AUI_ON_COMMAND("add",		OnCommandAdd)
AUI_ON_COMMAND("blacklist",	OnCommandBlackList)

AUI_END_COMMAND_MAP()

CDlgFriendRequest::CDlgFriendRequest()
{
	m_pBtnApply = NULL;
	m_pBtnRefuse = NULL;
	m_pBtnAdd = NULL;
	m_pBtnBlackList = NULL;
}

CDlgFriendRequest::~CDlgFriendRequest()
{
}

void CDlgFriendRequest::DoDataExchange(bool bSave)
{
	CDlgBase::DoDataExchange(bSave);

	DDX_Control("Btn_Apply", m_pBtnApply);
	DDX_Control("Btn_Refuse", m_pBtnRefuse);
	DDX_Control("Btn_Add", m_pBtnAdd);
	DDX_Control("Btn_BlackList", m_pBtnBlackList);
}

void CDlgFriendRequest::OnCommandCANCAL(const char *szCommand)
{
	Show(false);
}

void CDlgFriendRequest::OnCommandAccept(const char *szCommand)
{
	CECGameSession *pSession = GetGameSession();
	DWORD dwHandle = (DWORD)GetDataPtr("dword");
	pSession->friend_AddResponse(dwHandle, true);
	GetGameUIMan()->AddChatMessage(GetGameUIMan()->GetStringFromTable(536), GP_CHAT_MISC);
	m_pBtnApply->Enable(false);
	m_pBtnRefuse->Enable(false);
}

void CDlgFriendRequest::OnCommandRefuse(const char *szCommand)
{
	CECGameSession *pSession = GetGameSession();
	DWORD dwHandle = (DWORD)GetDataPtr("dword");
	pSession->friend_AddResponse(dwHandle, false);
	GetGameUIMan()->AddChatMessage(GetGameUIMan()->GetStringFromTable(535), GP_CHAT_MISC);
	m_pBtnApply->Enable(false);
	m_pBtnRefuse->Enable(false);
}

void CDlgFriendRequest::OnCommandAdd(const char *szCommand)
{
	CECGameSession *pSession = GetGameSession();
	int idPlayer = (int)GetData();
	pSession->friend_Add(idPlayer, NULL);
	GetGameUIMan()->AddChatMessage(GetGameUIMan()->GetStringFromTable(533), GP_CHAT_MISC);
	m_pBtnAdd->Enable(false);
	m_pBtnBlackList->Enable(false);
}

void CDlgFriendRequest::OnCommandBlackList(const char *szCommand)
{
	int idPlayer = (int)GetData();
	const ACHAR *pszName = GetGameRun()->GetPlayerName(idPlayer, true);
	GetGameUIMan()->m_pDlgBlackList->m_pEbxName->SetText(pszName);
	GetGameUIMan()->m_pDlgBlackList->OnCommandAdd(NULL);

	m_pBtnAdd->Enable(false);
	m_pBtnBlackList->Enable(false);
}
