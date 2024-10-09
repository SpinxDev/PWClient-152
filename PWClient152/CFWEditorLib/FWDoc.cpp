// FWDoc.cpp: implementation of the FWDoc class.
//
//////////////////////////////////////////////////////////////////////

#include "FWDoc.h"
#include "FWGlyphList.h"
#include "FWCommandList.h"
#include "FWStateSelect.h"
#include "FWView.h"
#include "FWGlyphFirework.h"
#include "FWAssemblySet.h"
#include "FWFireworkLanch.h"
#include "FWOutterProperty.h"
#include <AMemFile.h>


#define new A_DEBUG_NEW





FWDoc::FWDoc()
{
	m_pParticalQuotaList = new AIntList;
	m_pSoundQuotaList = new AIntList;
	m_pCommandList = new FWCommandList;

	m_nCurrentParticalQuota = 0;
	m_nMaxParticalQuota = 3000;

	m_nCurrentSoundQuota = 0;
	m_nMaxSoundQuota = 5;
}

FWDoc::~FWDoc()
{
	SAFE_DELETE(m_pParticalQuotaList);
	SAFE_DELETE(m_pSoundQuotaList);
	SAFE_DELETE(m_pCommandList);
}


void FWDoc::Serialize(FWArchive &ar)
{
	FWTemplate::Serialize(ar);

	if (ar.IsStoring())
	{
	}
	else
	{
		m_pCommandList->DeleteAll();
		
		// update quota data
		m_pParticalQuotaList->RemoveAll();
		m_nCurrentParticalQuota = 0;
		m_pSoundQuotaList->RemoveAll();
		m_nCurrentSoundQuota = 0;
		ALISTPOSITION pos = m_pGlyphList->GetHeadPosition();
		while (pos)
		{
			FWGlyph *pGlyph = m_pGlyphList->GetNext(pos);
			int nParticalCount = pGlyph->GetParticalCount();
			int nSoundCount = pGlyph->GetSoundCount();
			m_pParticalQuotaList->AddTail(nParticalCount);
			m_pSoundQuotaList->AddTail(nSoundCount);
			m_nCurrentParticalQuota += nParticalCount;
			m_nCurrentSoundQuota += nSoundCount;
		}
	}
}

void FWDoc::UpdateQuotaAfterAdd()
{
	// new added glyph should be at tail
	FWGlyph *pGlyph = m_pGlyphList->GetTail();

	// update
	int nCount = pGlyph->GetParticalCount();
	m_pParticalQuotaList->AddTail(nCount);
	m_nCurrentParticalQuota += nCount;

	nCount = pGlyph->GetSoundCount();
	m_pSoundQuotaList->AddTail(nCount);
	m_nCurrentSoundQuota += nCount;
}

void FWDoc::UpdateQuotaBeforeDelete(ALISTPOSITION posGlyph)
{
	// get corresponding pos in quota list
	ALISTPOSITION pos1 = m_pGlyphList->GetHeadPosition();
	ALISTPOSITION pos2 = m_pParticalQuotaList->GetHeadPosition();
	ALISTPOSITION pos3 = m_pSoundQuotaList->GetHeadPosition();
	// (pos1 && pos2 && pos3) is just for safety
	while (pos1 != posGlyph && pos1 && pos2 && pos3)
	{
		m_pGlyphList->GetNext(pos1);
		m_pParticalQuotaList->GetNext(pos2);
		m_pSoundQuotaList->GetNext(pos3);
	}
	ASSERT(pos1 && pos2 && pos3);

	// update
	int nCount = m_pParticalQuotaList->GetAt(pos2);
	m_pParticalQuotaList->RemoveAt(pos2);
	m_nCurrentParticalQuota -= nCount;

	nCount = m_pSoundQuotaList->GetAt(pos3);
	m_pSoundQuotaList->RemoveAt(pos3);
	m_nCurrentSoundQuota -= nCount;
}

void FWDoc::UpdateQuotaAfterUpdate(FWGlyph *pGlyph)
{
	// find position of glyph in the list
	ALISTPOSITION posGlyph = m_pGlyphList->GetHeadPosition();
	while (posGlyph)
	{
		if (pGlyph == m_pGlyphList->GetAt(posGlyph))
		{
			break;
		}
		m_pGlyphList->GetNext(posGlyph);
	}
	if (!posGlyph) return;

	// get corresponding pos in quota list
	ALISTPOSITION pos1 = m_pGlyphList->GetHeadPosition();
	ALISTPOSITION pos2 = m_pParticalQuotaList->GetHeadPosition();
	ALISTPOSITION pos3 = m_pSoundQuotaList->GetHeadPosition();
	// (pos1 && pos2 && pos3) is just for safety
	while (pos1 != posGlyph && pos1 && pos2 && pos3)
	{
		m_pGlyphList->GetNext(pos1);
		m_pParticalQuotaList->GetNext(pos2);
		m_pSoundQuotaList->GetNext(pos3);
	}
	ASSERT(pos1 && pos2 && pos3);

	// update
	int nCountOld = m_pParticalQuotaList->GetAt(pos2);
	int nCountNew = pGlyph->GetParticalCount();
	m_pParticalQuotaList->SetAt(pos2, nCountNew);
	m_nCurrentParticalQuota += -nCountOld + nCountNew;

	nCountOld = m_pSoundQuotaList->GetAt(pos3);
	nCountNew = pGlyph->GetSoundCount();
	m_pSoundQuotaList->SetAt(pos3, nCountNew);
	m_nCurrentSoundQuota += -nCountOld + nCountNew;
}

void FWDoc::AttachView(FWView *pView)
{
	ASSERT(pView);

	m_pView = pView;
	pView->m_pDoc = this;
}
