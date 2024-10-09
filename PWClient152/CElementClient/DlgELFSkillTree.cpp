// Filename	: DlgELFSkillTree.cpp
// Creator	: Chen Zhixin
// Date		: March 2, 2009

#include "DlgELFSkillTree.h"
#include "EC_GameUIMan.h"
#include "EC_HostPlayer.h"
#include "EC_HostGoblin.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgELFSkillTree, CDlgBase)

AUI_ON_COMMAND("Senior",		OnCommand_Senior)
AUI_ON_COMMAND("Junior",		OnCommand_Junior)

AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgELFSkillTree, CDlgBase)

AUI_END_EVENT_MAP()

CDlgELFSkillTree::CDlgELFSkillTree()
{

}

CDlgELFSkillTree::~CDlgELFSkillTree()
{
}

bool CDlgELFSkillTree::OnInitDialog()
{
	return true;
}

void CDlgELFSkillTree::OnShowDialog()
{

}

void CDlgELFSkillTree::OnCommand_Senior(const char *szCommand)
{
	PAUIDIALOG pDlg = GetGameUIMan()->GetDialog("Win_Elf_Skilltree2");
	POINT pt = GetPos();
	pDlg->SetPosEx(pt.x, pt.y);
	pDlg->Show(true);
	Show(false);
}

void CDlgELFSkillTree::OnCommand_Junior(const char * szCommand)
{
	PAUIDIALOG pDlg = GetGameUIMan()->GetDialog("Win_Elf_Skilltree1");
	POINT pt = GetPos();
	pDlg->SetPosEx(pt.x, pt.y);
	pDlg->Show(true);
	Show(false);
}

void CDlgELFSkillTree::OnTick()
{
	CECHostGoblin *pHostGoblin = (CECHostGoblin *)GetHostPlayer()->GetGoblinModel();
	if (!pHostGoblin)
	{
		Show(false);
	}
}