// FWStateCreateFirework.cpp: implementation of the FWStateCreateFirework class.
//
//////////////////////////////////////////////////////////////////////

#include "FWStateCreateFirework.h"
#include "FWFirework.h"
#include "FWAlgorithm.h"
#include "FWGlyphFirework.h"
#include "FWView.h"
#include "FWDoc.h"
#include "FWCommandList.h"
#include "FWCommandCreate.h"
#include "FWStateSelect.h"

#define new A_DEBUG_NEW


FW_IMPLEMENT_DYNAMIC(FWStateCreateFirework, FWState)



FWStateCreateFirework::FWStateCreateFirework(FWView *pView, FWGlyphFirework *pGlyph) :
	FWState(pView)
{
	m_pGlyph = pGlyph;
	m_bGlyphInUse = false;
}

FWStateCreateFirework::~FWStateCreateFirework()
{
	if (!m_bGlyphInUse)
		SAFE_DELETE(m_pGlyph);
}

void FWStateCreateFirework::OnDraw()
{
	FWState::OnDraw();
}

#define HALF_LENGTH 10

void FWStateCreateFirework::OnLClick(UINT nFlags, APointI point)
{
	ARectI rect(
		point.x - HALF_LENGTH,
		point.y - HALF_LENGTH,
		point.x + HALF_LENGTH,
		point.y + HALF_LENGTH);

	FWAlgorithm::ClampRectToRect(rect, GetView()->GetClientRectLP());

	m_pGlyph->m_rect = rect;

	GetDocument()->GetCommandList()->ExecuteCommand(
		new FWCommandCreate(GetDocument(), m_pGlyph));
	m_bGlyphInUse = true;

	GetView()->ChangeState(new FWStateSelect(GetView()));
}

