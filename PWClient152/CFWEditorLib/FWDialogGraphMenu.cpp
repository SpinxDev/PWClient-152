// FWDGraphMenu.cpp: implementation of the FWDGraphMenu class.
//
//////////////////////////////////////////////////////////////////////

#include "FWDialogGraphMenu.h"
#include "AUIObject.h"
#include "AUIDialog.h"
#include "FWGlyphRectangle.h"
#include "FWStateCreateRect.h"
#include "FWView.h"
#include "FWAUIManager.h"
#include "FWDialogEdit.h"
#include "FWGlyphEllipse.h"
#include "FWStateCreatePolygon.h"
#include "FWGlyphPolygon.h"
#include "FWStateCreateBezier.h"
#include "FWGlyphBezier.h"
#include "FWFireworkGraph.h"
#include "FWDoc.h"
#include "FWGlyphList.h"


#define new A_DEBUG_NEW




FW_IMPLEMENT_DYNCREATE(FWDialogGraphMenu, FWDialogBase)

BEGIN_EVENT_MAP_FW(FWDialogGraphMenu, FWDialogBase)

ON_EVENT_FW("BTN_RECTANGLE", WM_LCLICK, FWDialogGraphMenu::OnClickBtnRectangle)
ON_EVENT_FW("BTN_ELLIPSE", WM_LCLICK, FWDialogGraphMenu::OnClickBtnEllipse)
ON_EVENT_FW("BTN_POLYGON", WM_LCLICK, FWDialogGraphMenu::OnClickBtnPlygon)
ON_EVENT_FW("BTN_BEZIER", WM_LCLICK, FWDialogGraphMenu::OnClickBtnBezier)

END_EVENT_MAP_FW()

void FWDialogGraphMenu::OnClickBtnRectangle(WPARAM wParam, LPARAM lParam)
{
	Show(false);

	FWGlyphRectangle *pGlyph = new FWGlyphRectangle;
	pGlyph->Init(new FWFireworkGraph, GetDocument()->GetGlyphList()->GetNextName());

	GetView()->ChangeState(new FWStateCreateRect(GetView(), pGlyph));
}

void FWDialogGraphMenu::OnClickBtnEllipse(WPARAM wParam, LPARAM lParam)
{
	Show(false);

	FWGlyphEllipse *pGlyph = new FWGlyphEllipse;
	pGlyph->Init(new FWFireworkGraph, GetDocument()->GetGlyphList()->GetNextName());

	GetView()->ChangeState(new FWStateCreateRect(GetView(), pGlyph));
}

void FWDialogGraphMenu::OnClickBtnPlygon(WPARAM wParam, LPARAM lParam)
{
	Show(false);

	FWGlyphPolygon *pGlyph = new FWGlyphPolygon;
	pGlyph->Init(new FWFireworkGraph, GetDocument()->GetGlyphList()->GetNextName());

	GetView()->ChangeState(new FWStateCreatePolygon(GetView(), pGlyph));
}

void FWDialogGraphMenu::OnClickBtnBezier(WPARAM wParam, LPARAM lParam)
{
	Show(false);

	FWGlyphBezier *pGlyph = new FWGlyphBezier;
	pGlyph->Init(new FWFireworkGraph, GetDocument()->GetGlyphList()->GetNextName());
	GetView()->ChangeState(new FWStateCreateBezier(GetView(), pGlyph));
}
