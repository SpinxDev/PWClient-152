// NetSelectState.cpp: implementation of the FWNetSelectState class.
//
//////////////////////////////////////////////////////////////////////

#include "FWDoc.h"
#include "FWStateNetSelect.h"
#include "FWGlyph.h"
#include "FWStateSelect.h"
#include "FWGlyphList.h"
#include "FWAUIManager.h"
#include "FWView.h"
#include "FWFlatCollector.h"


#define new A_DEBUG_NEW


FW_IMPLEMENT_DYNAMIC(FWStateNetSelect, FWState)



FWStateNetSelect::FWStateNetSelect(FWView *pView, APointI mousePoint) :
	FWState(pView),
	m_originalPoint(mousePoint),
	m_Rect(0, 0, 0, 0)
{
	GetView()->SetCapture();
}

FWStateNetSelect::~FWStateNetSelect()
{
	GetView()->ReleaseCapture();	
}

void FWStateNetSelect::OnMouseMove(UINT nFlags, APointI point)
{
	m_Rect = ARectI(m_originalPoint.x, m_originalPoint.y, point.x, point.y);
	m_Rect.Normalize();
}

void FWStateNetSelect::OnLButtonUp(UINT nFlags, APointI point)
{
	FWGlyphList* pList = GetDocument()->GetGlyphList();
	ARectI rect(m_originalPoint.x, m_originalPoint.y, point.x, point.y);
	rect.Normalize();
	ALISTPOSITION pos = pList->GetTailPosition();
	FWGlyph* pGlyph;
	while(pos)
	{
		pGlyph = pList->GetAt(pos);
		APointI center = pGlyph->GetCentralPoint();
		if (rect.PtInRect(center))
		{		
		    pList->InvertSel(pos);
		}
		pList->GetPrev(pos);
	}

	GetView()->ReleaseCapture();
	GetView()->ChangeState(new FWStateSelect(GetView()));
}

void FWStateNetSelect::OnLButtonDown(UINT nFlags, APointI point)
{
	
}

void FWStateNetSelect::OnDraw()
{
	FWFlatCollector *pCollector = GetView()->GetFlatCollector();

	ARectI rectDP = m_Rect;
	GetView()->LPtoDP(&rectDP);
	pCollector->DrawRectangle2D(
		rectDP,
		0x20000000,
		true,
		0x20000000);
	pCollector->DrawRectangle2D(
		rectDP,
		0xff606060);

	FWState::OnDraw();
	
}
