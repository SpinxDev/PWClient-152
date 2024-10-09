// FWArtDoubleArc.cpp: implementation of the FWArtDoubleArc class.
//
//////////////////////////////////////////////////////////////////////

#include "FWArtDoubleArc.h"
#include "FWArchive.h"
#include "CharContour.h"
#include "CharOutline.h"
#include "CharVerticesBuffer.h"
#include "FWAlgorithm.h"
#include <AList2.h>

#define new A_DEBUG_NEW


FW_IMPLEMENT_SERIAL(FWArtDoubleArc, FWArt)

FWArtDoubleArc::FWArtDoubleArc() :
	m_fRatio(.5f)
{

}

FWArtDoubleArc::~FWArtDoubleArc()
{

}

FWArtDoubleArc & FWArtDoubleArc::operator = (const FWArtDoubleArc & src)
{
	m_fRatio = src.GetRatio();
	return *this;
}

void FWArtDoubleArc::MakeArtistic(CCharOutline *pOutline) const
{
	ARectI rect = FWAlgorithm::GetOutlineRect(pOutline);

	float w = rect.Width() * .5f;
	float h = rect.Height() * .5f;
	float r = (w * w - h * h) / (2 * h);

	FOR_EACH_POINT;

	float dh = float(sqrt(r * r - x * x) - (r - h));
	y *= 1 + dh / h * m_fRatio;

	END_FOR_EACH_POINT;
}

void FWArtDoubleArc::Serialize(FWArchive &ar)
{
	FWArt::Serialize(ar);

	if (ar.IsStoring())
	{
		ar << m_fRatio;
	}
	else
	{
		ar >> m_fRatio;
	}
}

void FWArtDoubleArc::FillParamArray(FWParamArray &arrayParam) const
{
	arrayParam.Add(FWParam(m_fRatio, -1.f, 1.f, _AL("ÇúÂÊÏµÊý")));
}

void FWArtDoubleArc::UpdateFromParamArray(const FWParamArray &arrayParam)
{
	m_fRatio = arrayParam[0].GetVal();
}
