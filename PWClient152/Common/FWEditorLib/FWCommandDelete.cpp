// CutCommand.cpp: implementation of the FWDelCommand class.
//
//////////////////////////////////////////////////////////////////////

#include "FWCommandDelete.h"
#include "FWDoc.h"
#include "FWGlyph.h"
#include "FWGlyphList.h"

#define new A_DEBUG_NEW






FWCommandDelete::FWCommandDelete(FWDoc *pDoc):
	m_bGlyphUsed(true),
	m_pDoc(pDoc)
{
	FWGlyphList* pList = m_pDoc->GetGlyphList();

	// Store the selected item in the list
	ALISTPOSITION selPos = pList->GetSelHeadPosition();
	ALISTPOSITION glyphPos = pList->GetHeadPosition();
	FWGlyph* pGlyph;
	int nIndex = 0;
	while(selPos != NULL)
	{	
		// get sel glyph
		pGlyph = pList->GetSelNext(selPos);

		// get index of sel glyph in glyphlist
		// note : assume the order of sellist is the same as glyphlist
		while(pGlyph != pList->GetNext(glyphPos)) nIndex++;
		
		// record glyph
		m_listGlyph.AddTail(pGlyph);
		// record index
		m_listIndex.AddTail(nIndex);
	}
}

FWCommandDelete::~FWCommandDelete()
{
	if (!m_bGlyphUsed)
	{
		FWGlyph* pGlyph;
		ALISTPOSITION pos = m_listGlyph.GetHeadPosition();
		while(pos != NULL)
		{
			pGlyph = m_listGlyph.GetNext(pos);
			delete pGlyph;
		}
	}
}

void FWCommandDelete::Execute()
{
	FWGlyphList* pList = m_pDoc->GetGlyphList();

	// remove pointers of currently selected glyph
	ALISTPOSITION posInt = m_listIndex.GetHeadPosition();
	ALISTPOSITION glyphPos = pList->GetHeadPosition();
	ALISTPOSITION posToDel;
	int nIndex = 0, i = 0;
	while(posInt != NULL)
	{
		nIndex = m_listIndex.GetNext(posInt);
		
		while ( i < nIndex )
		{
			pList->GetNext(glyphPos);
			i++;
		}
		posToDel = glyphPos;
		pList->GetNext(glyphPos);
		m_pDoc->UpdateQuotaBeforeDelete(posToDel);
		pList->RemoveAt(posToDel);
	}
	m_bGlyphUsed = false;
}

void FWCommandDelete::Unexecute()
{
	// put pointers of deleted glyph back to glyphlist

	FWGlyphList* pList = m_pDoc->GetGlyphList();

	ALISTPOSITION pos = m_listGlyph.GetHeadPosition();
	ALISTPOSITION posInt = m_listIndex.GetHeadPosition();
	ALISTPOSITION glyphPos = pList->GetHeadPosition();
	FWGlyph* pGlyph;
	int nIndex, i = 0;
	while(pos != NULL)
	{
		ASSERT(posInt != NULL);
		pGlyph = m_listGlyph.GetNext(pos);
		nIndex = m_listIndex.GetNext(posInt);
		
		while ( i < nIndex )
		{
			pList->GetNext(glyphPos);
			i++;
		}
		ALISTPOSITION newPos;
		newPos = pList->AddTail(pGlyph);
		pList->AddSel(newPos);
		m_pDoc->UpdateQuotaAfterAdd();
	}
	
	
	m_bGlyphUsed = true;
}
