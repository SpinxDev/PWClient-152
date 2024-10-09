// Filename	: DlgGuildCreate.cpp
// Creator	: Tom Zhou
// Date		: October 25, 2005

#include "DlgGuildCreate.h"
#include "EC_GameUIMan.h"
#include "EC_GameSession.h"
#include "EC_Faction.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgGuildCreate, CDlgBase)

AUI_ON_COMMAND("confirm",		OnCommand_confirm)
AUI_ON_COMMAND("IDCANCEL",		OnCommand_CANCEL)

AUI_END_COMMAND_MAP()

CDlgGuildCreate::CDlgGuildCreate()
{
}

CDlgGuildCreate::~CDlgGuildCreate()
{
}

bool CDlgGuildCreate::OnInitDialog()
{
	m_pDEFAULT_Txt_Input = GetDlgItem("DEFAULT_Txt_Input");

	return true;
}

void CDlgGuildCreate::OnCommand_confirm(const char * szCommand)
{
	ACString strName = m_pDEFAULT_Txt_Input->GetText();
	if( strName.GetLength() <= 0 ) return;

	GetGameSession()->faction_create(strName, _AL(""));
	GetGameUIMan()->EndNPCService();
	Show(false);
}

void CDlgGuildCreate::OnCommand_CANCEL(const char * szCommand)
{
	GetGameUIMan()->EndNPCService();
	Show(false);
}
