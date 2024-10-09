// GlyphList.cpp: implementation of the FWGlyphList class.
//
//////////////////////////////////////////////////////////////////////

#include "FWGlyphList.h"
#include "FWGlyph.h"
#include "Mesh.h"
#include "CodeTemplate.h"
#include "GLUPolygon.h"
#include "GLUTess.h"
#include "CharOutline.h"
#include "FWAssemblySet.h"
#include "FWArchive.h"

#ifdef _FW_EDITOR
	#include "FWView.h"
#endif

#define new A_DEBUG_NEW





FWGlyphList::FWGlyphList()
{
	m_fScale = .3f;
}

FWGlyphList::~FWGlyphList()
{
	DeleteAll();
}

ARectI FWGlyphList::GetTrueRect() const
{
	ARectI fullRect(0, 0, 0, 0);
	ALISTPOSITION pos;
	pos = GetHeadPosition();
	FWGlyph* pGlyph;
	while(pos)
	{
		pGlyph = GetNext(pos);
		fullRect.SetUnion(fullRect, pGlyph->GetTrueRect());
	}
	return fullRect;
}


FWGlyph* FWGlyphList::GetHitItem(APointI point)
{
	ALISTPOSITION pos = GetTailPosition();
	FWGlyph* pGlyph;
	while(pos)
	{
		pGlyph = GetPrev(pos);
		if (pGlyph->IsHit(point))
			return pGlyph;
	}
	return NULL;
}

ALISTPOSITION FWGlyphList::GetHitItemPosition(APointI point)
{
	ALISTPOSITION pos = GetTailPosition();
	FWGlyph* pGlyph;
	while(pos)
	{
		pGlyph = GetAt(pos);
		if (pGlyph->IsHit(point))
			break;
		GetPrev(pos);
	}
	return pos;	
}

void FWGlyphList::DeleteGlyph(ALISTPOSITION pos)
{
	ASSERT(pos != NULL);
	FWGlyph* pGlyph = GetAt(pos);
	delete pGlyph;
	RemoveAt(pos);
}

void FWGlyphList::DeleteAll()
{
	ALISTPOSITION pos = GetHeadPosition();
	FWGlyph* pGlyph;
	while(pos)
	{
		pGlyph = GetNext(pos);
		delete pGlyph;
	}
	RemoveAll();
}


ALISTPOSITION FWGlyphList::GetSelHeadPosition() const
{
	return m_selList.GetHeadPosition();
}

ALISTPOSITION FWGlyphList::GetSelTailPosition() const
{
	return m_selList.GetTailPosition();
}

FWGlyph* FWGlyphList::GetSelNext(ALISTPOSITION& rPosition) const
{
	return GetAt(m_selList.GetNext(rPosition));
}

FWGlyph*& FWGlyphList::GetSelNext(ALISTPOSITION& rPosition)
{
	return GetAt(m_selList.GetNext(rPosition));
}

FWGlyph* FWGlyphList::GetSelPrev(ALISTPOSITION& rPosition) const
{
	return GetAt(m_selList.GetPrev(rPosition));
}

FWGlyph*& FWGlyphList::GetSelPrev(ALISTPOSITION& rPosition)
{
	return GetAt(m_selList.GetPrev(rPosition));
}

void FWGlyphList::EmptySel()
{
	m_selList.RemoveAll();
}

void FWGlyphList::AddSel(ALISTPOSITION pos)
{
	ASSERT(pos != NULL);
	ASSERT(GetAt(pos) != NULL);

	ALISTPOSITION glyphPos = GetHeadPosition();
	ALISTPOSITION insertPos = m_selList.GetHeadPosition();
	ALISTPOSITION selPos;
	while(insertPos != NULL)
	{
		selPos = m_selList.GetAt(insertPos);
		while(glyphPos != selPos)
		{
			if (glyphPos == pos)
			{
				if (pos != selPos)
					m_selList.InsertBefore(insertPos, pos);
				return;
			}				
			GetNext(glyphPos);
		}
		GetNext(glyphPos);
		m_selList.GetNext(insertPos);
	}
	m_selList.AddTail(pos);
}

void FWGlyphList::RemoveSel(ALISTPOSITION pos)
{
	ASSERT(pos != NULL);
	ASSERT(GetAt(pos) != NULL);

	ALISTPOSITION posInSel = m_selList.Find(pos);
	if (posInSel != NULL)
		m_selList.RemoveAt(posInSel);
}

void FWGlyphList::InvertSel(ALISTPOSITION pos)
{
	ASSERT(pos != NULL);
	ASSERT(GetAt(pos) != NULL);

	ALISTPOSITION posInSel = m_selList.Find(pos);
	if (posInSel == NULL)
		AddSel(pos);
	else
		m_selList.RemoveAt(posInSel);
}

ARectI FWGlyphList::GetSelTrueRect() const
{
	ARectI rect(0, 0, 0, 0);
	ALISTPOSITION pos = m_selList.GetHeadPosition();
	while(pos)
	{
		rect.SetUnion(
			GetAt(m_selList.GetNext(pos))->GetTrueRect(),
			rect);
	}
	return rect;
}

void FWGlyphList::RemoveAt(ALISTPOSITION pos)
{
	ALISTPOSITION posInSel = m_selList.Find(pos);
	if (posInSel != NULL)
		m_selList.RemoveAt(posInSel);

	FWGlyphListBase::RemoveAt(pos);
}

FWGlyph* FWGlyphList::RemoveTail()
{
	ALISTPOSITION tail = GetTailPosition();
	ALISTPOSITION posInSel = m_selList.Find(tail);
	if (posInSel != NULL)
		m_selList.RemoveAt(posInSel);

	return FWGlyphListBase::RemoveTail();
}

FWGlyph* FWGlyphList::RemoveHead()
{
	ALISTPOSITION head = GetHeadPosition();
	ALISTPOSITION posInSel = m_selList.Find(head);
	if (posInSel != NULL)
		m_selList.RemoveAt(posInSel);

	return FWGlyphListBase::RemoveHead();	
}
	
void FWGlyphList::RemoveAll()
{
	m_selList.RemoveAll();
	FWGlyphListBase::RemoveAll();
}

void FWGlyphList::RemoveAllSel()
{
	ALISTPOSITION posInSel = m_selList.GetHeadPosition();
	ALISTPOSITION pos;
	while(posInSel != NULL)
	{
		pos = m_selList.GetNext(posInSel);
		FWGlyphListBase::RemoveAt(pos);
	}
	m_selList.RemoveAll();
}

void FWGlyphList::Serialize(FWArchive& ar)
{
	if (ar.IsLoading())
		DeleteAll();

	FWGlyphListBase::Serialize(ar);

	if (ar.IsStoring())
	{
		ar << m_fScale;
	}
	else
	{
		ar >> m_fScale;

		m_selList.RemoveAll();

		ALISTPOSITION pos = GetHeadPosition();
		while (pos)
		{
			FWGlyph *pGlyph = GetNext(pos);
			pGlyph->GenerateMeshParam();
		}
	}

}

void FWGlyphList::SelAll()
{
	m_selList.RemoveAll();
	ALISTPOSITION pos = GetHeadPosition();
	while (pos != NULL)
	{
		m_selList.AddTail(pos);
		GetNext(pos);
	}
}

bool FWGlyphList::FillGfxSet(FWAssemblySet *pSet, const A3DMATRIX4& matTM, float fStartTime) const
{
	ALISTPOSITION pos2 = GetHeadPosition();
	while (pos2)
	{
		if (!GetNext(pos2)->CopyTo(pSet, m_fScale, matTM, fStartTime))
		{
			return false;
		}
	}
	return true;
}

void FWGlyphList::SetScale(float fScale)
{
	m_fScale = fScale;
}

float FWGlyphList::GetScale() const
{
	return m_fScale;
}


ACString FWGlyphList::GetNextName() const
{
	int nNextIndex = 0;
	ALISTPOSITION pos = GetHeadPosition();
	while (pos)
	{
		FWGlyph *pGlyph = GetNext(pos);
		ACString tmp = pGlyph->GetName();
		if (tmp.GetLength() >= 3 &&
			tmp.Left(2) == _AL("效果"))
		{
			tmp = tmp.Right(tmp.GetLength() - 2);
			int nIndex = tmp.ToInt();
			if (nIndex > nNextIndex)
				nNextIndex = nIndex;
		}
	}
	ACString name;
	name.Format(_AL("效果%d"), nNextIndex + 1);
	return name;
}

ALISTPOSITION FWGlyphList::AddTail(FWGlyph *pGlyph)
{
	if (FindByName(pGlyph->GetName()))
		pGlyph->SetName(GetNextName());

	return FWGlyphListBase::AddTail(pGlyph);
}

ALISTPOSITION FWGlyphList::FindByName(LPCTSTR szName)
{
	ALISTPOSITION pos = GetHeadPosition();
	while (pos)
	{
		FWGlyph *pGlyph = GetAt(pos);
		if (pGlyph->GetName() == szName)
			return pos;
		GetNext(pos);
	}
	return NULL;
}

#ifdef _FW_EDITOR

void FWGlyphList::Draw(FWView *pView)
{
	ALISTPOSITION pos = GetHeadPosition();
	FWGlyph* pGlyph;
	while(pos)
	{
		pGlyph = GetNext(pos);
		pGlyph->Draw(pView);
	}
}

void FWGlyphList::DrawHandles(FWView *pView)
{
	ALISTPOSITION pos = GetSelHeadPosition();
	FWGlyph* pGlyph;
	while(pos)
	{
		pGlyph = GetSelNext(pos);
		pGlyph->DrawHandles(pView);
	}
}


#endif
