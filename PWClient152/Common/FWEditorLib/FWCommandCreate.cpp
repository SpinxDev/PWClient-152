#include "FWCommandCreate.h"
#include "FWGlyph.h"
#include "FWDoc.h"
#include "FWGlyphList.h"
#include "FWView.h"

#define new A_DEBUG_NEW


FWCommandCreate::FWCommandCreate(FWDoc *pDoc, FWGlyph* pGlyph):
	m_bGlyphUsed(false),
	m_pDoc(pDoc)
{
	m_glyphList.AddTail(pGlyph);
}

FWCommandCreate::FWCommandCreate(FWDoc *pDoc, FWGlyphList& glyphList):
	m_pDoc(pDoc),
	m_bGlyphUsed(false)
{
	m_glyphList.AddTail(&glyphList);
}

FWCommandCreate::~FWCommandCreate()
{
	if (!m_bGlyphUsed)
	{
		ALISTPOSITION pos = m_glyphList.GetHeadPosition();
		FWGlyph* pGlyph;
		while(pos != NULL)
		{
			pGlyph = m_glyphList.GetNext(pos);
			delete pGlyph;
		}
	}
}

void FWCommandCreate::Execute()
{
	// copy glyph pointers to docuemnt
	m_bGlyphUsed = true;
	FWGlyph* pGlyph;
	ALISTPOSITION pos = m_glyphList.GetHeadPosition();
	while(pos != NULL)
	{
		pGlyph = m_glyphList.GetNext(pos);
		m_pDoc->GetGlyphList()->AddTail(pGlyph);
		m_pDoc->UpdateQuotaAfterAdd();
	}
}

void FWCommandCreate::Unexecute()
{
	// remove glyph pointers from document
	m_bGlyphUsed = false;
	FWGlyph* pGlyph;
	ARectI rect(0, 0, 0, 0);
	ALISTPOSITION pos = m_glyphList.GetHeadPosition();
	while(pos != NULL)
	{
		pGlyph = m_glyphList.GetNext(pos);
		ALISTPOSITION posToDel = m_pDoc->GetGlyphList()->GetTailPosition();
		m_pDoc->UpdateQuotaBeforeDelete(posToDel);
		m_pDoc->GetGlyphList()->RemoveTail();
	}
}

