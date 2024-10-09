// FWCommandCombine.cpp: implementation of the FWCommandCombine class.
//
//////////////////////////////////////////////////////////////////////

#include "FWCommandCombine.h"
#include "FWGlyphComposite.h"
#include "CodeTemplate.h"
#include "FWDoc.h"
#include "FWGlyphList.h"

#define new A_DEBUG_NEW




FWCommandCombine::FWCommandCombine(FWDoc *pDoc, FWGlyphComposite *pComposite)
{
	m_pDoc = pDoc;
	m_pComposite = pComposite;
	m_bCompositeInUse = false;
}

FWCommandCombine::~FWCommandCombine()
{
	if (!m_bCompositeInUse)
	{
		m_pComposite->RemoveAll();
		SAFE_DELETE(m_pComposite);
	}
}

void FWCommandCombine::Execute()
{
	FWGlyphList *pList = m_pDoc->GetGlyphList();

	// remove from glyph list
	ALISTPOSITION posLocal = m_pComposite->GetHeadPosition();
	while (posLocal)
	{
		FWGlyph *pGlyph = m_pComposite->GetNext(posLocal);
		ALISTPOSITION posToDel = pList->Find(pGlyph);
		m_pDoc->UpdateQuotaBeforeDelete(posToDel);
		pList->RemoveAt(posToDel);
	}
	pList->RemoveAllSel();

	// add composite to glyph list
	m_pComposite->RecalculateRect();
	m_pComposite->GenerateMeshParam();
	ALISTPOSITION posGlyph = pList->AddTail(m_pComposite);
	pList->AddSel(posGlyph);
	m_pDoc->UpdateQuotaAfterAdd();

	m_bCompositeInUse = true;
}

void FWCommandCombine::Unexecute()
{
	// remove composite glyph from list
	FWGlyphList *pList = m_pDoc->GetGlyphList();
	ALISTPOSITION posGlyph = pList->Find(m_pComposite);
	m_pDoc->UpdateQuotaBeforeDelete(posGlyph);
	pList->RemoveSel(posGlyph);
	pList->RemoveAt(posGlyph);

	// copy combined glyphs to list
	ALISTPOSITION posLocal = m_pComposite->GetHeadPosition();
	while (posLocal)
	{
		FWGlyph *pGlyph = m_pComposite->GetNext(posLocal);
		pGlyph->GenerateMeshParam();
		ALISTPOSITION posGlyph = pList->AddTail(pGlyph);
		pList->AddSel(posGlyph);
		m_pDoc->UpdateQuotaAfterAdd();
	}

	m_bCompositeInUse = false;
}
