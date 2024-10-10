// FWArtTilt.cpp: implementation of the FWArtTilt class.
//
//////////////////////////////////////////////////////////////////////

#include "FWArtTilt.h"
#include "CharContour.h"
#include "CharOutline.h"
#include "CharVerticesBuffer.h"
#include "FWArchive.h"
#include <A3DMacros.h>
#include <AList2.h>

#define new A_DEBUG_NEW

FW_IMPLEMENT_SERIAL(FWArtTilt, FWArt)

FWArtTilt::FWArtTilt() :
	m_nAngle(30)
{
	
}

FWArtTilt::~FWArtTilt()
{

}


FWArtTilt & FWArtTilt::operator = (const FWArtTilt & src)
{
	m_nAngle = src.GetAngle();
	return *this;
}

void FWArtTilt::Serialize(FWArchive &ar)
{
	FWArt::Serialize(ar);

	if (ar.IsStoring()) 
		ar << m_nAngle;
	else 
		ar >> m_nAngle;
}

void FWArtTilt::FillParamArray(FWParamArray &arrayParam) const
{
	arrayParam.Add(FWParam(m_nAngle, -60L, 60L, _AL("«„–±Ω«∂»")));
}

void FWArtTilt::UpdateFromParamArray(const FWParamArray &arrayParam)
{
	m_nAngle = arrayParam[0].GetVal();
}

void FWArtTilt::MakeArtistic(CCharOutline *pOutline) const
{
	float fRadian = m_nAngle * A3D_PI / 180.f;
	float s = float(tan(fRadian));

	FOR_EACH_POINT;

	y += s * x;

	END_FOR_EACH_POINT;
}
