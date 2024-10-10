// FWArtStepScale.cpp: implementation of the FWArtStepScale class.
//
//////////////////////////////////////////////////////////////////////

#include "FWArtStepScale.h"
#include "FWArchive.h"
#include "CharContour.h"
#include "CharOutline.h"
#include "CharVerticesBuffer.h"
#include "FWAlgorithm.h"
#include <AList2.h>

#define new A_DEBUG_NEW


FW_IMPLEMENT_SERIAL(FWArtStepScale, FWArt)

FWArtStepScale::FWArtStepScale() :
	m_fRatioLeft(.5f),
	m_fRatioRight(1.5f)
{

}

FWArtStepScale::~FWArtStepScale()
{

}

FWArtStepScale & FWArtStepScale::operator = (const FWArtStepScale & src)
{
	m_fRatioLeft = src.GetRatioLeft();
	m_fRatioRight = src.GetRatioRight();
	return *this;
}


void FWArtStepScale::MakeArtistic(CCharOutline *pOutline) const
{
	ARectI rect = FWAlgorithm::GetOutlineRect(pOutline);
	
	float k = (m_fRatioRight - m_fRatioLeft) / rect.Width();
	float b = (m_fRatioLeft + m_fRatioRight) * .5f;

	FOR_EACH_POINT;

	y *= k * x + b;

	END_FOR_EACH_POINT;

}

void FWArtStepScale::Serialize(FWArchive &ar)
{
	if (ar.IsStoring())
	{
		ar << m_fRatioLeft << m_fRatioRight;
	}
	else
	{
		ar >> m_fRatioLeft >> m_fRatioRight;
	}
}

void FWArtStepScale::FillParamArray(FWParamArray &arrayParam) const
{
	arrayParam.Add(FWParam(m_fRatioLeft, 0.f, 2.f, _AL("×ó¶Ë·ÅËõ±ÈÀý")));
	arrayParam.Add(FWParam(m_fRatioRight, 0.f, 2.f, _AL("ÓÒ¶Ë·ÅËõ±ÈÀý")));	
}

void FWArtStepScale::UpdateFromParamArray(const FWParamArray &arrayParam)
{
	m_fRatioLeft = arrayParam[0].GetVal();
	m_fRatioRight = arrayParam[1].GetVal();
}

