#include "DlgFortressMini.h"
#include "EC_GameUIMan.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgFortressMini, CDlgBase)
AUI_ON_COMMAND("IDCANCEL", OnCommand_CANCEL)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgFortressMini, CDlgBase)
AUI_ON_EVENT("Img_FortressStatus", WM_LBUTTONUP,		OnEventLButtonUp_Img_FortressStatus)
AUI_END_EVENT_MAP()

CDlgFortressMini::CDlgFortressMini()
{
}

void CDlgFortressMini::OnCommand_CANCEL(const char *szCommand)
{
	m_pAUIManager->RespawnMessage();
}

void CDlgFortressMini::OnEventLButtonUp_Img_FortressStatus(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	PAUIDIALOG pDlgStatus = GetGameUIMan()->GetDialog("Win_FortressStatus");
	pDlgStatus->Show(!pDlgStatus->IsShow());
}
