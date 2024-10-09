// FWGlyphGraphBase.cpp: implementation of the FWGlyphGraphBase class.
//
//////////////////////////////////////////////////////////////////////

#include "FWGlyphGraphBase.h"
#include "FWFireworkGraph.h"
#include "CharOutline.h"
#include "CharContour.h"
#include "CharVerticesBuffer.h"
#include "FWGlyphList.h"
#include "FWAlgorithm.h"


using namespace FWAlgorithm;

#define new A_DEBUG_NEW


FW_IMPLEMENT_DYNAMIC(FWGlyphGraphBase, FWGlyph)



FWGlyphGraphBase::FWGlyphGraphBase()
{

}

FWGlyphGraphBase::~FWGlyphGraphBase()
{

}

bool FWGlyphGraphBase::PrepareFireworkData(float fScale)
{
	CCharOutline *pOutline = NULL;

	BEGIN_FAKE_WHILE;

	// get outline
	FWFireworkGraph *pFirework = static_cast<FWFireworkGraph *>(m_pFirework);
	
	pOutline = CreateOutline();
	CHECK_BREAK(pOutline);
	
	// transform from logic coordinate to world coordinate
	APtrList<CCharContour *>* pList = pOutline->GetCharContoursList();
	ALISTPOSITION pos = pList->GetHeadPosition();
	while (pos)
	{
		CCharContour *pContour = pList->GetNext(pos);
		CCharVerticesBuffer* pBuffer = pContour->GetCharVerticesBuffer();
		for (int i = 0; i < pBuffer->m_nVerticesCount; i++)
		{
			// scale
			pBuffer->m_paVertices[i][0] *= fScale;
			pBuffer->m_paVertices[i][1] *= -fScale;
		}
	}
	
	// we should use offset to position glyph, rather than using offseted
	// plane data directly, or the scaling will not work properly
	
	// move to origin
	ARectI rect = GetOutlineRect(pOutline);
	APointI center = rect.CenterPoint();
	pos = pList->GetHeadPosition();
	while (pos)
	{
		CCharContour *pContour = pList->GetNext(pos);
		CCharVerticesBuffer* pBuffer = pContour->GetCharVerticesBuffer();
		for (int i = 0; i < pBuffer->m_nVerticesCount; i++)
		{
			pBuffer->m_paVertices[i][0] -= center.x;
			pBuffer->m_paVertices[i][1] -= center.y;
		}
	}
	// set offset
	SetOffsetPos(FWFirework::OFFSET_INDEX_BY_GLYPH, 
		A3DVECTOR3(static_cast<float>(center.x), static_cast<float>(center.y), 0.f));
	
	// build firework
	CHECK_BREAK(pFirework->BuildDataFromOutline(pOutline));


	END_FAKE_WHILE;

	SAFE_DELETE(pOutline);

	RETURN_FAKE_WHILE_RESULT;
}

const char * FWGlyphGraphBase::GetDlgToShow()
{
	return TO_STRING(FWDialogGraph);
}

void FWGlyphGraphBase::Serialize(FWArchive &ar)
{
	FWGlyph::Serialize(ar);
}

