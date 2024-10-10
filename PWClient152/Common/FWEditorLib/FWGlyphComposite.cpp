// FWGlyphComposite.cpp: implementation of the FWGlyphComposite class.
//
//////////////////////////////////////////////////////////////////////

#include "FWGlyphComposite.h"
#include "CodeTemplate.h"
#include "CharOutline.h"
#include "CharContour.h"

#define new A_DEBUG_NEW


FW_IMPLEMENT_SERIAL(FWGlyphComposite, FWGlyphGraphBase)



FWGlyphComposite::FWGlyphComposite()
{
	
}

FWGlyphComposite::~FWGlyphComposite()
{
	DeleteAll();
}

CCharOutline * FWGlyphComposite::CreateOutline() const
{
	CCharOutline *pOutline = new CCharOutline;
	ALISTPOSITION pos = m_lstGlyph.GetHeadPosition();
	while (pos)
	{
		FWGlyph *pGlyph = m_lstGlyph.GetNext(pos);
		CCharOutline *p = pGlyph->CreateOutline();
		if (p)
		{
			ALISTPOSITION pos2 = p->GetCharContoursList()->GetHeadPosition();
			while (pos2)
			{
				pOutline->GetCharContoursList()->AddTail(
					p->GetCharContoursList()->GetNext(pos2));
			}
			p->GetCharContoursList()->RemoveAll();
			SAFE_DELETE(p);
		}
	}
	return pOutline;
}

void FWGlyphComposite::MoveHandleTo(int nHandle, APointI newPoint)
{
	ARectI rect = GetSizeRect();
	switch(nHandle)
	{
	case 1:
		rect.left = newPoint.x;
		rect.bottom = newPoint.y;
		break;
	case 2:
		rect.left = newPoint.x;
		break;
	case 3:
		rect.left = newPoint.x;
		rect.top = newPoint.y;	
		break;
	case 4:
		rect.top = newPoint.y;
		break;
	case 5:
		rect.right = newPoint.x;
		rect.top = newPoint.y;
		break;
	case 6:
		rect.right = newPoint.x;
		break;
	case 7:
		rect.right = newPoint.x;
		rect.bottom = newPoint.y;
		break;
	case 8:
		rect.bottom = newPoint.y;
		break;
	}
	
	if (rect.left >= rect.right || 
		rect.top >= rect.bottom ||
		rect.Width() < DEFAULT_HANDLE_RADIUS || 
		rect.Height() < DEFAULT_HANDLE_RADIUS)	
		return;

	SetSizeRect(rect);
}

ARectI FWGlyphComposite::GetSizeRect() const
{
	return m_rectSize;
}

void FWGlyphComposite::SetSizeRect(ARectI rect)
{
	ARectI rectOld = GetSizeRect();
	ALISTPOSITION pos = m_lstGlyph.GetHeadPosition();
	ALISTPOSITION posRatio = m_lstRatio.GetHeadPosition();
	while (pos && posRatio)
	{
		FWGlyph *pGlyph = m_lstGlyph.GetNext(pos);
		ARectI rectGlyph = pGlyph->GetSizeRect();
		
		Ratio & rt = m_lstRatio.GetNext(posRatio);
		
		rectGlyph.left = int(rect.left + rect.Width() * rt.l);
		rectGlyph.right = int(rect.left + rect.Width() * rt.r);
		rectGlyph.top = int(rect.top + rect.Height() * rt.t);
		rectGlyph.bottom = int(rect.top + rect.Height() * rt.b);
		
		pGlyph->SetSizeRect(rectGlyph);
	}
	RecalculateRect();
}

double FWGlyphComposite::GetAngle() const
{
	return 0;
}

void FWGlyphComposite::SetAngle(double angle)
{
	// ignore this operation
	// combined glyphs can not be rotated
}

ARectI FWGlyphComposite::GetTrueRect() const
{
	return m_rectTrue;	
}

int FWGlyphComposite::GetHandleCount() const
{
	return 8;	
}

APointI FWGlyphComposite::GetHandle(int nPoint) const
{
	// the first handle is the left-bottom one
	// follow the clockwise direction to get index other handles

	switch(nPoint)
	{
	case 1:
		return APointI(m_rectSize.left, m_rectSize.bottom);
	case 2:
		return APointI(m_rectSize.left, (m_rectSize.top + m_rectSize.bottom) / 2);
	case 3:
		return APointI(m_rectSize.left, m_rectSize.top);
	case 4:
		return APointI((m_rectSize.left + m_rectSize.right) / 2, m_rectSize.top);
	case 5:
		return APointI(m_rectSize.right, m_rectSize.top);
	case 6:
		return APointI(m_rectSize.right, (m_rectSize.top + m_rectSize.bottom) / 2);
	case 7:
		return APointI(m_rectSize.right, m_rectSize.bottom);
	case 8:
		return APointI((m_rectSize.left + m_rectSize.right) / 2, m_rectSize.bottom);
	}

	ASSERT(false);
	return APointI(0 , 0);
}

bool FWGlyphComposite::IsHit(const APointI& point) const
{
	return m_rectTrue.PtInRect(point);
}

void FWGlyphComposite::Serialize(FWArchive &ar)
{
	FWGlyphGraphBase::Serialize(ar);
	
	m_lstGlyph.Serialize(ar);
	
	if (ar.IsStoring())
	{

	}
	else
	{
		RecalculateRect();
	}
}


ALISTPOSITION FWGlyphComposite::GetHeadPosition()
{
	return m_lstGlyph.GetHeadPosition();
}

FWGlyph * FWGlyphComposite::GetHead()
{
	return m_lstGlyph.GetHead();
}

ALISTPOSITION FWGlyphComposite::GetTailPosition()
{
	return m_lstGlyph.GetTailPosition();
}

FWGlyph * FWGlyphComposite::GetTail()
{
	return m_lstGlyph.GetTail()	;
}

FWGlyph * FWGlyphComposite::GetNext(ALISTPOSITION &pos)
{
	return m_lstGlyph.GetNext(pos);
}

FWGlyph * FWGlyphComposite::GetPrev(ALISTPOSITION &pos)
{
	return m_lstGlyph.GetPrev(pos);
}

FWGlyph * FWGlyphComposite::GetAt(ALISTPOSITION pos)
{
	return m_lstGlyph.GetAt(pos);
}

void FWGlyphComposite::RemoveAll()
{
	m_rectTrue = m_rectSize = ARectI(0, 0, 0, 0);
	m_lstGlyph.RemoveAll();
}

ALISTPOSITION FWGlyphComposite::Find(FWGlyph *pGlyph, ALISTPOSITION startAfter)
{
	return m_lstGlyph.Find(pGlyph, startAfter);
}

ALISTPOSITION FWGlyphComposite::AddHead(FWGlyph *pGlyph)
{
	return m_lstGlyph.AddHead(pGlyph);
}

ALISTPOSITION FWGlyphComposite::AddTail(FWGlyph *pGlyph)
{
	return m_lstGlyph.AddTail(pGlyph);
}

int FWGlyphComposite::GetCount()
{
	return m_lstGlyph.GetCount();
}

bool FWGlyphComposite::IsEmpty()
{
	return (m_lstGlyph.IsEmpty() == true);
}


void FWGlyphComposite::RecalculateRect()
{
	// compute rect
	m_rectTrue = m_rectSize = ARectI(0, 0, 0, 0);
	ALISTPOSITION pos = m_lstGlyph.GetHeadPosition();
	while (pos)
	{
		FWGlyph *pGlyph = m_lstGlyph.GetNext(pos);
		m_rectSize.SetUnion(m_rectSize, pGlyph->GetSizeRect());
		m_rectTrue.SetUnion(m_rectTrue, pGlyph->GetTrueRect());
	}

	// update ratio list
	m_lstRatio.RemoveAll();
	pos = m_lstGlyph.GetHeadPosition();
	while (pos)
	{
		FWGlyph *pGlyph = m_lstGlyph.GetNext(pos);
		
		Ratio rt;
		ARectI rectGlyph = pGlyph->GetSizeRect();
		rt.l = float(rectGlyph.left - m_rectSize.left) / m_rectSize.Width();
		rt.r = float(rectGlyph.right - m_rectSize.left) / m_rectSize.Width();
		rt.t = float(rectGlyph.top - m_rectSize.top) / m_rectSize.Height();
		rt.b = float(rectGlyph.bottom - m_rectSize.top) / m_rectSize.Height();

		m_lstRatio.AddTail(rt);
	}

}

void FWGlyphComposite::DeleteAll()
{
	ALISTPOSITION pos = m_lstGlyph.GetHeadPosition();
	while (pos)
	{
		FWGlyph *pGlyph = m_lstGlyph.GetNext(pos);
		SAFE_DELETE(pGlyph);
	}
	m_lstGlyph.RemoveAll();
}
