// ResizeState.cpp: implementation of the FWResizeState class.
//
//////////////////////////////////////////////////////////////////////

#include "FWStateResize.h"
#include "FWGlyph.h"
#include "FWStateMoveHandle.h"
#include "FWStateMove.h"
#include "FWCommandResize.h"
#include "FWDoc.h"
#include "FWGlyphList.h"
#include "FWView.h"
#include "FWCommandList.h"
#include "FWFlatCollector.h"


FW_IMPLEMENT_DYNAMIC(FWStateResize, FWState)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction                                         //
//////////////////////////////////////////////////////////////////////

#define MARGIN 3


FWStateResize::FWStateResize(FWView *pView, FWGlyph *pGlyph):
	FWState(pView),
	m_pGlyph(pGlyph),
	m_bLButtonDowned(false)
{
	ASSERT(GetDocument()->GetGlyphList()->GetSelCount() == 1);
	ASSERT(pGlyph == GetDocument()->GetGlyphList()->GetSelHead());

	m_rect = pGlyph->GetSizeRect();

	GetDocument()->GetGlyphList()->EmptySel();
}

FWStateResize::~FWStateResize()
{
	ALISTPOSITION pos = GetDocument()->GetGlyphList()->Find(m_pGlyph);
	GetDocument()->GetGlyphList()->AddSel(pos);
}

void FWStateResize::OnDraw()
{
	FWState::OnDraw();

	FWFlatCollector *pCollector = GetView()->GetFlatCollector();

	ARectI rect = m_rect;
	rect.Inflate(MARGIN, MARGIN);
	GetView()->LPtoDP(&rect);
	pCollector->DrawRectangle2D(rect, COLOR_DARK_GRAY);
}


void FWStateResize::OnLButtonDown(UINT nFlags, APointI point)
{
	GetView()->SetCapture();
	m_ptMouseOld = point;
	m_bLButtonDowned = true;
}

void FWStateResize::OnLButtonUp(UINT nFlags, APointI point)
{
	if(!m_bLButtonDowned)	return;
	m_bLButtonDowned = false;

	GetView()->ReleaseCapture();

	ARectI rect = m_rect;
	GetDocument()->GetCommandList()->ExecuteCommand(
		new FWCommandResize(m_pGlyph, rect, m_pGlyph->GetSizeRect()));
}


void FWStateResize::OnMouseMove(UINT nFlags, APointI point)
{
	if (!m_bLButtonDowned) return;

	APointI offset = point - m_ptMouseOld;
	m_rect.Inflate(offset.x, offset.y);
	if (m_rect.right < m_rect.left) 
		m_rect.right = m_rect.left = static_cast<int>((m_rect.left + m_rect.right) * .5f);
	if (m_rect.bottom < m_rect.top) 
		m_rect.bottom = m_rect.top = static_cast<int>((m_rect.top + m_rect.bottom) * .5f);
	m_ptMouseOld = point;
}
