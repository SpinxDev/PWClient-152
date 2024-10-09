// File		: DlgConsole.cpp
// Creator	: Xiao Zhou
// Date		: 2005/8/16

#include "DlgConsole.h"
#include "EC_GameUIMan.h"

#define new A_DEBUG_NEW


AUI_BEGIN_COMMAND_MAP(CDlgConsole, CDlgBase)

AUI_ON_COMMAND("Command_Edit",	OnCommandEdit)

AUI_END_COMMAND_MAP()


AUI_BEGIN_EVENT_MAP(CDlgConsole, CDlgBase)

AUI_ON_EVENT("*",	WM_KEYDOWN,	OnEventKeyDown)
AUI_ON_EVENT("*",	WM_CHAR,	OnEventChar)

AUI_END_EVENT_MAP()


CDlgConsole::CDlgConsole()
{
	m_pCommand = NULL;
	m_pCommandEdit = NULL;
}

CDlgConsole::~CDlgConsole()
{
}

void CDlgConsole::DoDataExchange(bool bSave)
{
	CDlgBase::DoDataExchange(bSave);

	DDX_Control("DEFAULT_Command", m_pCommand);
	DDX_Control("Command_Edit", m_pCommandEdit);
}

void CDlgConsole::OnCommandEdit(const char *szCommand)
{
	ACString strCmd = m_pCommand->GetText();

	m_pCommandEdit->SetCommandLine(strCmd);
	m_pCommandEdit->OnDlgItemMessage(WM_CHAR, ACHAR('\r'), 0);

	GetGameUIMan()->OnDlgCmd_Console(strCmd);
	m_pCommand->SetText(_AL(""));
}

void CDlgConsole::OnEventKeyDown(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	int nVirtKey = int(wParam);

	if( nVirtKey == VK_ESCAPE )
	{
		Show(false);
		return;
	}
	else if( nVirtKey == VK_UP || nVirtKey == VK_DOWN )
	{
		m_pCommandEdit->OnDlgItemMessage(WM_KEYDOWN, wParam, lParam);
		m_pCommand->SetText(m_pCommandEdit->GetCommandLine());
	}
	else if( nVirtKey == VK_PRIOR || nVirtKey == VK_NEXT )
	{
		m_pCommandEdit->OnDlgItemMessage(WM_KEYDOWN, wParam, lParam);
	}

	ChangeFocus(m_pCommand);
}

void CDlgConsole::OnEventChar(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	if( ACHAR(wParam) == ACHAR('~') )
	{
		Show(false);
		m_pCommand->SetText(_AL(""));
		return;
	}

	ChangeFocus(m_pCommand);
}

void CDlgConsole::Resize(A3DRECT rcOld, A3DRECT rcNew)
{
	// 调用基类处理
	CDlgBase::Resize(rcOld, rcNew);

	SetSize(rcNew.Width(), GetSize().cy);
	SetPosEx(0, GetPos().y);
}