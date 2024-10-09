// Filename	: DlgWikiByNameBase.cpp
// Creator	: Feng Ning
// Date		: 2010/04/26

#include "DlgWikiByNameBase.h"
#include "WikiSearchCommand.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgWikiByNameBase, CDlgWikiBase)
AUI_ON_COMMAND("search", OnCommand_SearchByName)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgWikiByNameBase, CDlgWikiBase)

AUI_ON_EVENT("Txt_Search",	WM_LBUTTONDOWN,	OnEventLButtonDown_Txt_Search)
AUI_ON_EVENT("Txt_Search",	WM_KEYDOWN,		OnEventKeyDown)

AUI_END_EVENT_MAP()

CDlgWikiByNameBase::CDlgWikiByNameBase()
:m_pTxt_Search(NULL)	
,m_pBtn_Search(NULL)
,m_ShowHint(false)
{
}

void CDlgWikiByNameBase::ShowHint(bool isShow)
{
	if(isShow)
	{
		m_ShowHint = true;
		m_pBtn_Search->Enable(false);
		m_pTxt_Search->SetText(GetStringFromTable(8722));
		m_pTxt_Search->SetHint(GetStringFromTable(8722));
		m_pTxt_Search->SetFontAndColor(NULL, 0, 0xFF808080);
	}
	else
	{
		m_ShowHint = false;
		m_pTxt_Search->SetText(_AL(""));
		m_pTxt_Search->SetFontAndColor(NULL, 0, 0xFFFFFFFF);
	}
}

bool CDlgWikiByNameBase::OnInitDialog()
{
	DDX_Control("Txt_Search", m_pTxt_Search);	
	DDX_Control("Btn_Search", m_pBtn_Search);

	ShowHint(true);

	return CDlgWikiBase::OnInitDialog();
}

void CDlgWikiByNameBase::OnShowDialog()
{
	//
	CDlgWikiBase::OnShowDialog();
}

void CDlgWikiByNameBase::OnTick()
{
	CDlgWikiBase::OnTick();

	m_pBtn_Search->Enable(!m_ShowHint && WikiSearchByName::IsSearchPatternValid(m_pTxt_Search->GetText()));
}

void CDlgWikiByNameBase::SetSearchText(const ACHAR * szPattern)
{
	// 设置下一次待搜索的内容
	// 用于控制下一次显示时自动搜索内容
	//
	m_pTxt_Search->SetText(szPattern);
}

bool CDlgWikiByNameBase::Release(void)
{
	// 
	return CDlgWikiBase::Release();
}

void CDlgWikiByNameBase::OnCommand_SearchByName(const char *szCommand)
{
	ACString szTxt = m_pTxt_Search->GetText();
	
	// seal the direct call
	if(!WikiSearchByName::IsSearchPatternValid(szTxt))
	{
		return;
	}
	
	// try to use default search command
	ResetSearchCommand();
	
	// set pattern to name search command if existed
	WikiSearchByName* pCommand = dynamic_cast<WikiSearchByName*>(GetSearchCommand());
	if(pCommand)
	{
		pCommand->SetPattern(szTxt);
	}

	TurnPageHome();
}

void CDlgWikiByNameBase::OnEventLButtonDown_Txt_Search(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	if(m_ShowHint)
	{
		ShowHint(false);
	}
}

void CDlgWikiByNameBase::OnEventKeyDown(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	if (wParam == VK_ESCAPE)
		ChangeFocus(NULL);
}