// Filename	: DlgWiki.h
// Creator	: Xu Wenbin
// Date		: 2010/04/06

#include "AFI.h"
#include "DlgWiki.h"
#include "EC_GameUIMan.h"
#include "WikiGuideData.h"
#include "AUICheckBox.h"
#include "DlgAutoHelp.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgWiki, CDlgBase)
AUI_ON_COMMAND("Win_*",	OnCommand_ShowDialog)
AUI_ON_COMMAND("Btn_OpenWikiPop",	OnCommand_OpenAutoHelp)
AUI_ON_COMMAND("Chk_Close",	OnCommand_CloseAutoHelp)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgWiki, CDlgBase)
AUI_END_EVENT_MAP()

CDlgWiki::CDlgWiki()
{
}

bool CDlgWiki::OnInitDialog()
{
	// initialize the data for guide wiki
	WikiGuideData::Init();

	// 所有Wiki子界面按钮置为非开放状态
	//
	PAUIOBJECTLISTELEMENT pElement = GetFirstControl();
	while (pElement)
	{
		PAUIOBJECT pObj = pElement->pThis;
		if (strstr(pObj->GetName(), "Btn_"))
		{
			// 所有按钮置为不可用状态
			pObj->Enable(false);
		}
		
		pElement = GetNextControl(pElement);
	}
	
	// 开放Wiki界面在此
	PAUISTILLIMAGEBUTTON pBtn = NULL;
	DDX_Control("Btn_Close", pBtn);
	pBtn->Enable(true);

	pBtn = NULL;
	DDX_Control("Btn_Help", pBtn);
	if(pBtn) pBtn->Enable(true);

	pBtn = NULL;
	DDX_Control("Btn_Monster", pBtn);
	if(pBtn) pBtn->Enable(true);

	pBtn = NULL;
	DDX_Control("Btn_Equip", pBtn);
	if(pBtn) pBtn->Enable(true);

	pBtn = NULL;
	DDX_Control("Btn_Task", pBtn);
	if(pBtn) pBtn->Enable(true);

	pBtn = NULL;
	DDX_Control("Btn_Activity", pBtn);
	if(pBtn) pBtn->Enable(true);

	pBtn = NULL;
	DDX_Control("Btn_Recipe", pBtn);
	if(pBtn) pBtn->Enable(true);
	
	pBtn = NULL;
	DDX_Control("Btn_Item", pBtn);
	if(pBtn) pBtn->Enable(true);

	pBtn = NULL;
	DDX_Control("Btn_Skill", pBtn);
	if(pBtn) pBtn->Enable(true);

	pBtn = NULL;
	DDX_Control("Btn_Feature", pBtn);
	if(pBtn) pBtn->Enable(true);

	pBtn = NULL;
	DDX_Control("Btn_Guide", pBtn);
	if(pBtn) pBtn->Enable(true);

	return true;
}
void CDlgWiki::OnShowDialog()
{
	PAUICHECKBOX pChk = (PAUICHECKBOX)GetDlgItem("Chk_Close");
	if (pChk)
	{		
		pChk->Check(GetGameUIMan()->m_pDlgAutoHelp->IsForbidAutoHelp());
		if(GetDlgItem("Btn_OpenWikiPop"))
			GetDlgItem("Btn_OpenWikiPop")->Enable(!pChk->IsChecked());
	}	
}
void CDlgWiki::OnCommand_ShowDialog(const char *szCommand)
{
	PAUIDIALOG pDlg = GetGameUIMan()->GetDialog(szCommand);
	if(pDlg) pDlg->Show(!pDlg->IsShow());
}
void CDlgWiki::OnCommand_CloseAutoHelp(const char* szCommand)
{
	PAUICHECKBOX pChk = (PAUICHECKBOX)GetDlgItem("Chk_Close");
	if (pChk)
	{
		GetGameUIMan()->m_pDlgAutoHelp->SetForbidAutoHelpFlag(pChk->IsChecked());
		GetDlgItem("Btn_OpenWikiPop")->Enable(!pChk->IsChecked());
		if (pChk->IsChecked())
		{
	//		GetGameUIMan()->m_pDlgAutoHelp->SetAutoHelpState(false);
			GetGameUIMan()->m_pDlgAutoHelp->Show(false);
		}
	}	
}
void CDlgWiki::OnCommand_OpenAutoHelp(const char* szCommand)
{
	GetGameUIMan()->GetDialog("Win_WikiPop")->Show(true);
	GetGameUIMan()->m_pDlgAutoHelp->SetAutoHelpState(true);
}