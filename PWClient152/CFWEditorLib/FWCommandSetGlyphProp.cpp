// FWCommandSetGlyphProp.cpp: implementation of the FWCommandSetGlyphProp class.
//
//////////////////////////////////////////////////////////////////////

#include "FWCommandSetGlyphProp.h"
#include "CodeTemplate.h"
#include "FWGlyph.h"
#include "FWDoc.h"
#include <AMemFile.h>
#include "FWArchive.h"

#define new A_DEBUG_NEW




FWCommandSetGlyphProp::FWCommandSetGlyphProp(FWDoc *pDoc, FWGlyph *pGlyph, BYTE *pDataOld, int nSizeOld, BYTE *pDataNew, int nSizeNew)
{
	m_pDoc = pDoc;
	m_pGlyph = pGlyph;
	m_pDataOld = pDataOld;
	m_pDataNew = pDataNew;
	m_nSizeOld = nSizeOld;
	m_nSizeNew = nSizeNew;
}

FWCommandSetGlyphProp::~FWCommandSetGlyphProp()
{
	SAFE_DELETE_ARRAY(m_pDataOld);
	SAFE_DELETE_ARRAY(m_pDataNew);
}

void FWCommandSetGlyphProp::Execute()
{
	AMemFile file;
	file.Attach(m_pDataNew, m_nSizeNew, 0);
	FWArchive ar(&file, FWArchive::MODE_LOAD);
	m_pGlyph->Serialize(ar);
	m_pDoc->UpdateQuotaAfterUpdate(m_pGlyph);
	file.Detach();
}

void FWCommandSetGlyphProp::Unexecute()
{
	AMemFile file;
	file.Attach(m_pDataOld, m_nSizeOld, 0);
	FWArchive ar(&file, FWArchive::MODE_LOAD);
	m_pGlyph->Serialize(ar);
	m_pDoc->UpdateQuotaAfterUpdate(m_pGlyph);
	file.Detach();
}

