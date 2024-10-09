#include "FWGlyphRectangle.h"
#include "FWAlgorithm.h"
#include "CharOutline.h"
#include "CharContour.h"
#include "CharVerticesBuffer.h"

using namespace FWAlgorithm;

#define new A_DEBUG_NEW


FW_IMPLEMENT_SERIAL(FWGlyphRectangle, FWGlyphRectBase)

FWGlyphRectangle::FWGlyphRectangle()
{

}

FWGlyphRectangle::~FWGlyphRectangle()
{

}


CCharOutline * FWGlyphRectangle::CreateOutline() const
{
	CCharOutline *pOutline = new CCharOutline;
	CCharContour *pContour = new CCharContour;
	
	CCharVerticesBuffer *pBuffer = pContour->GetCharVerticesBuffer();
	pBuffer->Create(4);

	if (GetAngle() == 0)
	{
		Point2DoubleArray(APointI(m_rect.left, m_rect.top), pBuffer->m_paVertices[0]);
		Point2DoubleArray(APointI(m_rect.left, m_rect.bottom), pBuffer->m_paVertices[1]);
		Point2DoubleArray(APointI(m_rect.right, m_rect.bottom), pBuffer->m_paVertices[2]);
		Point2DoubleArray(APointI(m_rect.right, m_rect.top), pBuffer->m_paVertices[3]);
	}
	else
	{
		POINT *pPtArray = GetCornerPoint();
		for (int i = 0; i < 4; i++)
			Point2DoubleArray(pPtArray[i], pBuffer->m_paVertices[i]);
	}

	pOutline->GetCharContoursList()->AddTail(pContour);

	return pOutline;
}
