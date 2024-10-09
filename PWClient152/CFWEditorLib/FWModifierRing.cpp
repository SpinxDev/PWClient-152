// ModifierRing.cpp: implementation of the FWModifierRing class.
//
//////////////////////////////////////////////////////////////////////

#include "FWModifierRing.h"
#include "FWModifierCreator.h"
#include "FWArchive.h"

#define new A_DEBUG_NEW


FW_IMPLEMENT_SERIAL(FWModifierRing, FWModifier)



FWModifierRing::FWModifierRing()
{

}

FWModifierRing::~FWModifierRing()
{

}

FWModifierRing & FWModifierRing::operator = (const FWModifierRing &src)
{
	FWModifier::operator = (src);
	m_fBlastVel = src.GetBlastVel();
	m_vecBlastDir = src.GetBlastDir();
	m_fResistanceFactor = src.GetResistanceFactor();
	m_vecNormalDir = src.GetNormalDir();
	return *this;
}

int FWModifierRing::GetID()
{
	return FWModifierCreator::ID_MODIFIER_RING;
}

void FWModifierRing::InitParam()
{
	m_fBlastVel = 20.f;
	m_fResistanceFactor = 1.f;
	RadomParamForAll();
	RadomParamForSingle();
}

void FWModifierRing::RadomParamForAll()
{
	float fAngle = 0;
	do {
		m_vecNormalDir.x = a_Random(-1.f, 1.f);
		m_vecNormalDir.y = a_Random(-1.f, 1.f);
		m_vecNormalDir.z = a_Random(-1.f, 1.f);
		fAngle = float(m_vecNormalDir.z / sqrt(pow(m_vecNormalDir.x, 2) + pow(m_vecNormalDir.y, 2)));
	} while(fabs(fAngle) < .4f);

	m_vecNormalDir.Normalize();
	
}

void FWModifierRing::RadomParamForSingle()
{
	m_vecBlastDir.x = a_Random(-1.f, 1.f);
	m_vecBlastDir.y = a_Random(-1.f, 1.f);
	m_vecBlastDir.z = a_Random(-1.f, 1.f);
	m_vecBlastDir = CrossProduct(m_vecBlastDir, m_vecNormalDir);
	m_vecBlastDir.Normalize();

}

bool FWModifierRing::Tick(DWORD dwTickTime)
{
	if (!FWModifier::Tick(dwTickTime)) return true;

	// compute pos
	float fTickTime = dwTickTime * .001f;
	m_vecOffset = m_vecBlastDir * m_fCurVel * fTickTime;
	
	// udpate vel
	m_fCurVel += -1.f * m_fCurVel * m_fResistanceFactor * fTickTime;
	if (m_fCurVel < 0.f)	m_fCurVel = 0.f;

	return true;
}

bool FWModifierRing::Play(bool bForceStop)
{
	FWModifier::Play(bForceStop);

	return true;
}


bool FWModifierRing::Stop()
{
	FWModifier::Stop();

	m_fCurVel = m_fBlastVel;

	return true;
}

void FWModifierRing::Serialize(FWArchive &ar)
{
	FWModifier::Serialize(ar);

	if (ar.IsStoring())
	{
		ar << m_fBlastVel;
		ar << m_fResistanceFactor;
	}
	else
	{
		ar >> m_fBlastVel;
		ar >> m_fResistanceFactor;
		RadomParamForSingle();
		RadomParamForAll();
	}
}

void FWModifierRing::FillParamArray(FWParamArray &arrayParam) const
{
	arrayParam.Add(FWParam(m_fBlastVel, 0.f, 40.f, _AL("爆炸初速度")));
	arrayParam.Add(FWParam(m_fResistanceFactor, 0.F, 1.f, _AL("阻力系数")));
}

void FWModifierRing::UpdateFromParamArray(const FWParamArray &arrayParam)
{
	m_fBlastVel = arrayParam[0].GetVal();
	m_fResistanceFactor = arrayParam[1].GetVal();
}
