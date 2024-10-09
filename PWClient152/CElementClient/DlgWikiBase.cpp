// Filename	: DlgWikiBase.h
// Creator	: Xu Wenbin
// Date		: 2010/04/06

#include "AFI.h"
#include "DlgWikiBase.h"
#include "EC_GameUIMan.h"
#include "elementdataman.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "AWStringWithWildcard.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgWikiBase, CDlgNameLink)
AUI_ON_COMMAND("prev", OnCommand_Prev)
AUI_ON_COMMAND("next", OnCommand_Next)
AUI_ON_COMMAND("IDCANCEL", OnCommand_CANCEL)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgWikiBase, CDlgNameLink)
AUI_END_EVENT_MAP()

CDlgWikiBase::CDlgWikiBase()
{
	m_pBtn_Prev = NULL;
	m_pBtn_Next = NULL;
}

bool CDlgWikiBase::OnInitDialog()
{
	m_pBtn_Prev = dynamic_cast<PAUISTILLIMAGEBUTTON>(GetDlgItem("Btn_Prev"));
	m_pBtn_Next = dynamic_cast<PAUISTILLIMAGEBUTTON>(GetDlgItem("Btn_Next"));

	if(m_pBtn_Prev) m_pBtn_Prev->Enable(false);
	if(m_pBtn_Next) m_pBtn_Next->Enable(false);

	ResetSearchCommand();

	return true;
}

void CDlgWikiBase::OnShowDialog()
{
	TurnPageHome();
}

void CDlgWikiBase::OnTick()
{
	//
}

void CDlgWikiBase::OnEndSearch()
{
	if(m_pBtn_Next) m_pBtn_Next->Enable(HaveNextPage());
	if(m_pBtn_Prev) m_pBtn_Prev->Enable(HavePrevPage());
}

bool CDlgWikiBase::Release(void)
{
	WikiSearcher::Release();

	return CDlgNameLink::Release();
}

void CDlgWikiBase::OnCommand_CANCEL(const char * szCommand)
{
	// 隐藏自身
	Show(false);

	// 隐藏其它相关界面
	OnCommandCancel();
}

void CDlgWikiBase::OnCommand_Next(const char *szCommand)
{
	TurnPageDown();
}

void CDlgWikiBase::OnCommand_Prev(const char *szCommand)
{
	TurnPageUp();
}

bool CDlgWikiBase::RefreshByNewCommand(const CDlgWikiBase::SearchCommand* pCommand, bool bForceShow)
{
	// refresh the window, keep the show state
	bool bShow = IsShow();
	OnCommand_CANCEL("");
	if(bShow || bForceShow)
	{
		Show(true);
		GetGameUIMan()->BringWindowToTop(this);
	}

	return Refresh(pCommand);
}

void CDlgWikiBase::SetSearchDataPtr(void* ptr)
{
	SetDataPtr(ptr);
}