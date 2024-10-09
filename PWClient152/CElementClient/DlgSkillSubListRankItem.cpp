// Filename	: DlgSkillSubListRankItem.h
// Creator	: zhangyitian
// Date		: 2014/7/10

#include "DlgSkillSubListRankItem.h"
#include "DlgSkillSubList.h"

#include "AUISubDialog.h"
#include "AUIDef.h"

#include "EC_GameUIMan.h"

AUI_BEGIN_EVENT_MAP(CDlgSkillSubListRankItem, CDlgBase)

AUI_ON_EVENT("*",	WM_MOUSEWHEEL,	OnMouseWheel)

AUI_END_EVENT_MAP()

void CDlgSkillSubListRankItem::OnMouseWheel(WPARAM wParam, LPARAM lParam, AUIObject *pObj) {
	GetGameUIMan()->m_pDlgSkillSubList->GetParentDlgControl()->OnDlgItemMessage(WM_MOUSEWHEEL, wParam, lParam);
}