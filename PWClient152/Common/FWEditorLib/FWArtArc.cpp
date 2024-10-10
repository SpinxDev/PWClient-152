// FWArtArc.cpp: implementation of the FWArtArc class.
//
//////////////////////////////////////////////////////////////////////

#include "FWArtArc.h"
#include "FWArchive.h"
#include "CharContour.h"
#include "CharOutline.h"
#include "CharVerticesBuffer.h"
#include "FWAlgorithm.h"
#include <A3DMacros.h>
#include <AList2.h>

using namespace FWAlgorithm;

#define new A_DEBUG_NEW

FW_IMPLEMENT_SERIAL(FWArtArc, FWArt)

FWArtArc::FWArtArc() :
	m_nAngle(45)
{

}

FWArtArc::~FWArtArc()
{

}

FWArtArc & FWArtArc::operator = (const FWArtArc & src)
{
	m_nAngle = src.GetAngle();
	return *this;
}

void FWArtArc::Serialize(FWArchive &ar)
{
	FWArt::Serialize(ar);

	if (ar.IsStoring())
	{
		ar << m_nAngle;
	}
	else
	{
		ar >> m_nAngle;
	}
}

void FWArtArc::FillParamArray(FWParamArray &arrayParam) const
{
	arrayParam.Add(FWParam(m_nAngle, -170, 170, _AL("уе╫г")));
}

void FWArtArc::UpdateFromParamArray(const FWParamArray &arrayParam)
{
	m_nAngle = arrayParam[0].GetVal();
}


void FWArtArc::MakeArtistic(CCharOutline *pOutline) const
{
	if (m_nAngle != 0)
	{
		ARectI rect = GetOutlineRect(pOutline);
		float sign = float(m_nAngle > 0 ? 1 : -1);

		float radius = float(rect.Width() * .5f / sin(DEG2RAD(m_nAngle * .5f)));
		float radius2 = (float)pow(radius, 2);
		float deltaY = float(sqrt(radius2 - pow(rect.Width() * .5f, 2)) * sign);
		
		FOR_EACH_POINT;
		
		y += sqrt(radius2 - x * x) * sign - deltaY;
		
		END_FOR_EACH_POINT;
	}
}
