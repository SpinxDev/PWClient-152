#include "FWStateSelect.h"
#include "FWStateNetSelect.h"
#include "FWDoc.h"
#include "FWGlyph.h"
#include "FWStateMoveHandle.h"
#include "FWStateMove.h"
#include "FWGlyphList.h"
#include "FWView.h"
#include "FWDialogEdit.h"
#include "FWStateMoveWindow.h"

#define new A_DEBUG_NEW



FW_IMPLEMENT_DYNAMIC(FWStateSelect, FWState)

FWStateSelect::FWStateSelect(FWView *pView) :
	FWState(pView)
{
}

FWStateSelect::~FWStateSelect()
{
}

void FWStateSelect::OnLBeginDrag(UINT nFlags, APointI point)
{
	// click on board frame, move window
	// note : here we should use device coordinates
	if (!GetView()->GetClientRectLP().PtInRect(point))
	{
		GetView()->ChangeState(new FWStateMoveWindow(GetView()));
		return;
	}

	FWGlyphList *pList = GetDocument()->GetGlyphList();

	// Check for moving handle (only allowed on single selections)
	if (pList->GetSelCount() == 1)
	{
		FWGlyph* pGlyph = pList->GetSelHead();
		int nDragHandle = pGlyph->GetHitHandle(point);
		if (nDragHandle != 0)
		{
			GetView()->ChangeState(new FWStateMoveHandle(GetView(), pGlyph, nDragHandle));
			return;
		}
	}

	// See if the click was on an object, select and start move if so
	ALISTPOSITION pos = pList->GetHitItemPosition(point);
	if (pos != NULL)
	{
		// select if shift pressed
		if ((nFlags & MK_SHIFT) != 0)
		{
			// we just simple add it to the selection
			ARectI rect;
			pList->AddSel(pos);
		}
		else
		{	
			// if click on a not selected object
			if (pList->GetSelPosition(pos) == NULL)
			{
				// we should select this object only
				pList->EmptySel();
				pList->AddSel(pos);
			}
		}
		GetView()->ChangeState(new FWStateMove(GetView(), point));
		return;
	}

	// Click on backgroud,start a net-selection
	if ((nFlags & MK_SHIFT) == 0)
	{
		if (pList->GetSelCount() != 0)
		{
			pList->EmptySel();
		}
	}
	
	
	GetView()->ChangeState(new FWStateNetSelect(GetView(), point));	
}

void FWStateSelect::OnDraw()
{
	FWState::OnDraw();
}

void FWStateSelect::OnMouseMove(UINT nFlags, APointI point)
{
	if (GetView()->GetCapture())
		return;

	FWGlyphList* pList = GetDocument()->GetGlyphList();
	
	// check if the cursor is within the area of a handle
	// only alllowed on single selection
	if (pList->GetSelCount() == 1)
	{
		FWGlyph* pGlyph = pList->GetSelHead();
		int nDragHandle = pGlyph->GetHitHandle(point);
		if (nDragHandle != 0)
		{
			::SetCursor(pGlyph->GetHandleCursor(nDragHandle));
			return;
		}
	}

	::SetCursor(::LoadCursor(NULL, IDC_ARROW));
}

void FWStateSelect::OnLClick(UINT nFlags, APointI point)
{
	FWGlyphList* pList = GetDocument()->GetGlyphList();

	ALISTPOSITION pos = pList->GetHitItemPosition(point);
	if (pos != NULL)
	{
		// select if shift pressed
		if ((nFlags & MK_SHIFT) != 0)
		{// we just simple add it to the selection
		    pList->InvertSel(pos);
		}
		else
		{	
			// if click on a not selected object
			if (pList->GetSelPosition(pos) == NULL)
			{
				// we should select this object only
				pList->EmptySel();
				pList->AddSel(pos);
			}
		}
	}
	else
	{
		// click on the background
		if (pList->GetSelCount() != 0)
		{
			pList->EmptySel();
		}
	}
}


void FWStateSelect::OnRClick(UINT nFlags, APointI point)
{
	OnLClick(nFlags, point);

	FWGlyphList* pList = GetDocument()->GetGlyphList();
	if (pList->GetSelCount() == 1)
	{
		FWGlyph *pGlyph = pList->GetSelHead();

		GetView()->ShowContextMenu(pGlyph, point);
	}
}



void FWStateSelect::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (ProcessDirKey(nChar, nRepCnt)) return;
}

#define MOVE_STEP 2	
bool FWStateSelect::ProcessDirKey(UINT nChar, UINT nRepCnt)
{
	FWGlyphList *pList = GetDocument()->GetGlyphList();
	if (!pList->GetSelCount()) return false;

	int offsetX = 0, offsetY = 0;
	switch(nChar) 
	{
	case VK_LEFT:
		offsetX = -nRepCnt * MOVE_STEP;
		break;
	case VK_RIGHT:
		offsetX = nRepCnt * MOVE_STEP;
		break;
	case VK_UP:
		offsetY = -nRepCnt * MOVE_STEP;
		break;
	case VK_DOWN:
		offsetY = nRepCnt * MOVE_STEP;
		break;
	default:
		return false;
	}

	// use move state to do the move tricks
	FWStateMove moveState(GetView(), APointI(0, 0));
	moveState.OnLButtonUp(0, APointI(offsetX, offsetY));
	return true;
}

void FWStateSelect::OnLButtonDblClk(UINT nFlags, APointI point)
{
	OnLClick(nFlags, point);

	FWGlyphList *pList = GetDocument()->GetGlyphList();

	if (pList->GetSelCount() == 1)
	{
		FWGlyph *pGlyph = pList->GetSelHead();
		GetView()->ShowPropDlg(pGlyph);
	}
}
