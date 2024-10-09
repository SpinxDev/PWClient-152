// FWDialogBoard.cpp: implementation of the FWDialogBoard class.
//
//////////////////////////////////////////////////////////////////////

#include "FWDialogBoard.h"
#include "FWState.h"
#include "FWDoc.h"
#include "FWStateSelect.h"
#include "FWFlatCollector.h"
#include "FWDialogBoard.h"
#include "FWGlyph.h"
#include "FWGlyphList.h"
#include "FWView.h"


#define new A_DEBUG_NEW


FW_IMPLEMENT_DYNCREATE(FWDialogBoard, FWDialogBase)

BEGIN_EVENT_MAP_FW(FWDialogBoard, FWDialogBase)

ON_EVENT_FW(NULL, WM_LBUTTONDOWN, FWDialogBoard::OnLButtonDown)
ON_EVENT_FW(NULL, WM_LBUTTONUP, FWDialogBoard::OnLButtonUp)
ON_EVENT_FW(NULL, WM_MOUSEMOVE, FWDialogBoard::OnMouseMove)
ON_EVENT_FW(NULL, WM_TIMER, FWDialogBoard::OnTimer)
ON_EVENT_FW(NULL, WM_KEYDOWN, FWDialogBoard::OnKeyDown)
ON_EVENT_FW(NULL, WM_KEYUP, FWDialogBoard::OnKeyUp)
ON_EVENT_FW(NULL, WM_RBUTTONDOWN, FWDialogBoard::OnRButtonDown)
ON_EVENT_FW(NULL, WM_RBUTTONUP, FWDialogBoard::OnRButtonUp)
ON_EVENT_FW(NULL, WM_LBUTTONDBLCLK, FWDialogBoard::OnLButtonDblClk)

END_EVENT_MAP_FW()


#define GET_X_LPARAM(lp)                        ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp)                        ((int)(short)HIWORD(lp))


FWDialogBoard::FWDialogBoard()
{
}

FWDialogBoard::~FWDialogBoard()
{
}



void FWDialogBoard::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
	APointI pt(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));

	GetView()->OnLButtonDown(wParam, pt);
}

void FWDialogBoard::OnLButtonUp(WPARAM wParam, LPARAM lParam)
{
	APointI pt(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));

	GetView()->OnLButtonUp(wParam, pt);
}

void FWDialogBoard::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
	APointI pt(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));

	GetView()->OnMouseMove(wParam, pt);
}

void FWDialogBoard::OnRButtonDown(WPARAM wParam, LPARAM lParam)
{
	APointI pt(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));

	GetView()->OnRButtonDown(wParam, pt);
}

void FWDialogBoard::OnRButtonUp(WPARAM wParam, LPARAM lParam)
{
	APointI pt(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));

	GetView()->OnRButtonUp(wParam, pt);
}

void FWDialogBoard::OnTimer(WPARAM wParam, LPARAM lParam)
{
	GetView()->OnTimer(wParam);
}

void FWDialogBoard::OnKeyDown(WPARAM wParam, LPARAM lParam)
{
	GetView()->OnKeyDown(wParam, LOWORD(lParam), HIWORD(lParam));
}

void FWDialogBoard::OnKeyUp(WPARAM wParam, LPARAM lParam)
{
	GetView()->OnKeyUp(wParam, LOWORD(lParam), HIWORD(lParam));
}


ARectI FWDialogBoard::GetClientRect(void)
{
	A3DRECT aRect = GetRect();
	ARectI rect(aRect.left, aRect.top, aRect.right, aRect.bottom);
	rect.Deflate(8, 8);
	return rect;
}

void FWDialogBoard::OnLButtonDblClk(WPARAM wParam, LPARAM lParam)
{
	APointI pt(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));

	GetView()->OnLButtonDblClk(wParam, pt);
}

bool FWDialogBoard::Render(void)
{
	if (!FWDialogBase::Render()) return false;


	if (GetView()->IsShowGrid())
	{
		// draw axis and grid
		ARectI rect = GetClientRect();
		APointI center = rect.CenterPoint();
		center.x--;
		
		FWFlatCollector *pCollector = GetView()->GetFlatCollector();
		
		// draw x-, y-, axis
#define COLOR_AXIS 0x60ff0000
		pCollector->DrawLine(APointI(rect.left, center.y - 1), APointI(rect.right, center.y - 1), COLOR_AXIS);
		pCollector->DrawLine(APointI(rect.left, center.y), APointI(rect.right, center.y), COLOR_AXIS);
		pCollector->DrawLine(APointI(rect.left, center.y + 1), APointI(rect.right, center.y + 1), COLOR_AXIS);
		pCollector->DrawLine(APointI(center.x - 1, rect.top), APointI(center.x - 1, rect.bottom), COLOR_AXIS);
		pCollector->DrawLine(APointI(center.x, rect.top), APointI(center.x, rect.bottom), COLOR_AXIS);
		pCollector->DrawLine(APointI(center.x + 1, rect.top), APointI(center.x + 1, rect.bottom), COLOR_AXIS);
		
		// draw grid
#define COLOR_GRID 0x60008000
		
		const float fStep = 1 / GetDocument()->GetGlyphList()->GetScale() * 5;
		float x = center.x, y = center.y;
		
		y = center.y;
		while ((y -= fStep) > rect.top)
			pCollector->DrawLine(APointI(rect.left, y), APointI(rect.right, y), COLOR_GRID);
		
		y = center.y;
		while ((y += fStep) < rect.bottom)
			pCollector->DrawLine(APointI(rect.left, y), APointI(rect.right, y), COLOR_GRID);
		
		x = center.x;
		while ((x -= fStep) > rect.left)
			pCollector->DrawLine(APointI(x, rect.top), APointI(x, rect.bottom), COLOR_GRID);
		
		x = center.x;
		while ((x += fStep) < rect.right)
			pCollector->DrawLine(APointI(x, rect.top), APointI(x, rect.bottom), COLOR_GRID);
	}


	return true;
}
