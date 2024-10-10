// File		: DlgFriendGroup.cpp
// Creator	: Xiao Zhou
// Date		: 2005/8/18

#include "DlgFriendGroup.h"
#include "EC_GameUIMan.h"
#include "EC_GameSession.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgFriendGroup, CDlgBase)

AUI_ON_COMMAND("IDCANCEL",	OnCommandCANCAL)
AUI_ON_COMMAND("confirm",	OnCommandConfirm)

AUI_END_COMMAND_MAP()

CDlgFriendGroup::CDlgFriendGroup()
{
	m_pComboGroup = NULL;
}

CDlgFriendGroup::~CDlgFriendGroup()
{
}

void CDlgFriendGroup::DoDataExchange(bool bSave)
{
	CDlgBase::DoDataExchange(bSave);

	DDX_Control("Combo_Group", m_pComboGroup);
}

void CDlgFriendGroup::OnCommandCANCAL(const char *szCommand)
{
	Show(false);
}

void CDlgFriendGroup::OnCommandConfirm(const char *szCommand)
{
	CECGameSession *pSession = GetGameSession();
	int idPlayer = GetData();
	int idGroup = m_pComboGroup->GetItemData(m_pComboGroup->GetCurSel());
	int idGroupOld = (int)GetDataPtr();
	if (idGroup != idGroupOld)
		pSession->friend_SetGroup(idGroup, idPlayer);
	Show(false);
}
