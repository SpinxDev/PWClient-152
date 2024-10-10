// FWGlyphFirework.cpp: implementation of the FWGlyphFirework class.
//
//////////////////////////////////////////////////////////////////////

#include "FWGlyphFirework.h"
#include "FWGlyphList.h"
#include "FWFirework.h"

#ifdef _FW_EDITOR
	#include "FWFlatCollector.h"
	#include "FWView.h"
#endif

#define new A_DEBUG_NEW


#define HANDLE_HALF_WIDTH 2

FW_IMPLEMENT_SERIAL(FWGlyphFirework, FWGlyph)



FWGlyphFirework::FWGlyphFirework()
{

}

FWGlyphFirework::~FWGlyphFirework()
{

}


ARectI FWGlyphFirework::GetSizeRect() const
{
	ARectI rect = m_rect;
	rect.Inflate(HANDLE_HALF_WIDTH, HANDLE_HALF_WIDTH, HANDLE_HALF_WIDTH, HANDLE_HALF_WIDTH);
	return rect;
}

void FWGlyphFirework::SetSizeRect(ARectI rect)
{
	// note : size can not be changed, only the position can!
	if (GetSizeRect().Width() != rect.Width() ||
		GetSizeRect().Height() != rect.Height())
		return;

	rect.Deflate(HANDLE_HALF_WIDTH, HANDLE_HALF_WIDTH, HANDLE_HALF_WIDTH, HANDLE_HALF_WIDTH);
	m_rect = rect;
}

bool FWGlyphFirework::PrepareFireworkData(float fScale)
{
	APointI center = GetCentralPoint();
	A3DVECTOR3 pos;
	pos.x = center.x * fScale;
	pos.y = center.y * -fScale;
	pos.z = 0.f;

	SetOffsetPos(FWFirework::OFFSET_INDEX_BY_GLYPH, pos);

	return true;
}

void FWGlyphFirework::Serialize(FWArchive &ar)
{
	FWGlyph::Serialize(ar);

	if (ar.IsStoring())
	{
		ar << (short) m_rect.left << (short) m_rect.top << (short) m_rect.right << (short) m_rect.bottom;
	}
	else
	{
		short tmp = 0;
		ar >> tmp; m_rect.left = tmp;
		ar >> tmp; m_rect.top = tmp;
		ar >> tmp; m_rect.right = tmp;
		ar >> tmp; m_rect.bottom = tmp;
	}
}

#ifdef _FW_EDITOR

void FWGlyphFirework::Draw(FWView *pView) const
{
	ARectI rectDP = m_rect;
	pView->LPtoDP(&rectDP);
	
	pView->GetFlatCollector()->DrawRectangle2D(
		rectDP,
		COLOR_DARK_GRAY,
		true,
		COLOR_LIGHT_GRAY);
}

void FWGlyphFirework::DrawHandles(FWView *pView) const
{
	ARectI rectDP = m_rect;
	pView->LPtoDP(&rectDP);
	
	FWFlatCollector *pCollector = pView->GetFlatCollector();
	
	APointI pt1(rectDP.left, rectDP.top);
	pt1.x -= HANDLE_HALF_WIDTH;
	pt1.y -= HANDLE_HALF_WIDTH;
	APointI pt2(rectDP.left, rectDP.top);
	pt2.x += HANDLE_HALF_WIDTH;
	pt2.y += HANDLE_HALF_WIDTH;
	
	ARectI rect(pt1.x, pt1.y, pt2.x, pt2.y);
	rect.Normalize();
	pCollector->DrawRectangle2D(rect, COLOR_BLACK, true, COLOR_BLACK);
	rect.Offset(rectDP.Width(), 0);
	pCollector->DrawRectangle2D(rect, COLOR_BLACK, true, COLOR_BLACK);
	rect.Offset(0, rectDP.Height());
	pCollector->DrawRectangle2D(rect, COLOR_BLACK, true, COLOR_BLACK);
	rect.Offset(-rectDP.Width(), 0);
	pCollector->DrawRectangle2D(rect, COLOR_BLACK, true, COLOR_BLACK);
}

#endif