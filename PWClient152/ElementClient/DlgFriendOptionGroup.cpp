// File		: DlgFriendOptionGroup.cpp
// Creator	: Xiao Zhou
// Date		: 2005/8/18

#include "AUIEditBox.h"
#include "DlgFriendOptionGroup.h"
#include "EC_GameUIMan.h"
#include "DlgFriendList.h"
#include "DlgFriendOptionNormal.h"
#include "DlgFriendColor.h"
#include "DlgInputName.h"
#include "EC_Friend.h"
#include "EC_HostPlayer.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgFriendOptionGroup, CDlgBase)

AUI_ON_COMMAND("IDCANCEL",		OnCommandCANCAL)
AUI_ON_COMMAND("AddFriend",		OnCommandAddFriend)
AUI_ON_COMMAND("AddGroup",		OnCommandAddGroup)
AUI_ON_COMMAND("RenameGroup",	OnCommandRenameGroup)
AUI_ON_COMMAND("GroupColor",	OnCommandGroupColor)
AUI_ON_COMMAND("DeleteGroup",	OnCommandDeleteGroup)

AUI_END_COMMAND_MAP()

CDlgFriendOptionGroup::CDlgFriendOptionGroup()
{
}

CDlgFriendOptionGroup::~CDlgFriendOptionGroup()
{
}

void CDlgFriendOptionGroup::OnCommandCANCAL(const char *szCommand)
{
	Show(false);
}

void CDlgFriendOptionGroup::OnCommandAddFriend(const char *szCommand)
{
	GetGameUIMan()->m_pDlgFriendList->OnCommandAdd("add");
}

void CDlgFriendOptionGroup::OnCommandAddGroup(const char *szCommand)
{
	GetGameUIMan()->m_pDlgFriendOptionNormal->OnCommandAddGroup("AddGroup");
}

void CDlgFriendOptionGroup::OnCommandRenameGroup(const char *szCommand)
{
	CDlgFriendList* pDlgFriend = GetGameUIMan()->m_pDlgFriendList;
	PAUITREEVIEW pTree = pDlgFriend->m_pTvFriendList;
	P_AUITREEVIEW_ITEM pItem = pTree->GetSelectedItem();
	if( !pItem ) return;

	int idGroup = pTree->GetItemData(pItem);
	ACString strName = pTree->GetItemText(pItem);
	GetGameUIMan()->m_pDlgInputName->ShowForUsage(CDlgInputName::INPUTNAME_RENAME_GROUP, strName, -1, (void *)idGroup, "int");
}

void CDlgFriendOptionGroup::OnCommandGroupColor(const char *szCommand)
{
	CDlgFriendList* pDlgFriend = GetGameUIMan()->m_pDlgFriendList;
	PAUITREEVIEW pTree = pDlgFriend->m_pTvFriendList;
	P_AUITREEVIEW_ITEM pItem = pTree->GetSelectedItem();
	if( !pItem ) return;

	int idGroup = pTree->GetItemData(pItem);
	CECFriendMan *pMan = GetHostPlayer()->GetFriendMan();
	CECFriendMan::GROUP *pGroup = pMan->GetGroupByID(idGroup);

	CDlgFriendColor* pDlg = GetGameUIMan()->m_pDlgFriendColor;
	pDlg->SetData(idGroup);
	pDlg->Show(true, true);

	if( pGroup )
	{
		pDlg->m_pSliderColorR->SetLevel(A3DCOLOR_GETRED(pGroup->color));
		pDlg->m_pSliderColorG->SetLevel(A3DCOLOR_GETGREEN(pGroup->color));
		pDlg->m_pSliderColorB->SetLevel(A3DCOLOR_GETBLUE(pGroup->color));
	}

	pDlg->OnCommandColor("ColorR");
}

void CDlgFriendOptionGroup::OnCommandDeleteGroup(const char *szCommand)
{
	GetGameUIMan()->MessageBox("Game_DelGroup", GetStringFromTable(539),
		MB_OKCANCEL, A3DCOLORRGBA(255, 255, 255, 160));
}

