// ModifierParabola.cpp: implementation of the FWModifierParabola class.
//
//////////////////////////////////////////////////////////////////////

#include "FWModifierParabola.h"
#include "FWModifierCreator.h"
#include "FWParam.h"
#include "FWArchive.h"
#include "Global.h"

#define new A_DEBUG_NEW



FW_IMPLEMENT_SERIAL(FWModifierParabola, FWModifier)



FWModifierParabola::FWModifierParabola()
{

}

FWModifierParabola::~FWModifierParabola()
{

}

FWModifierParabola & FWModifierParabola::operator = (const FWModifierParabola &src)
{
	FWModifier::operator = (src);
	m_fBlastVel = src.GetBlastVel();
	m_vecBlastDir = src.GetBlastDir();
	m_fResistanceFactor = src.GetResistanceFactor();
	return *this;
}

bool FWModifierParabola::Play(bool bForceStop)
{
	FWModifier::Play(bForceStop);

	return true;	
}

bool FWModifierParabola::Stop()
{
	FWModifier::Stop();

	// set value to init value
	m_fCurBlastVel = m_fBlastVel;
	m_fCurVertVel = 0.f;

	return true;	
}

bool FWModifierParabola::Tick(DWORD dwTickTime)
{
	if (!FWModifier::Tick(dwTickTime)) return true;

	// compute offset
	float fTickTime = dwTickTime * .001f;
	A3DVECTOR3 vecBlastDelta = m_vecBlastDir * m_fCurBlastVel * fTickTime;
	A3DVECTOR3 vecDownDelta = -g_vAxisY * m_fCurVertVel * fTickTime;
	m_vecOffset = vecDownDelta + vecBlastDelta;
	
	// udpate vel
	m_fCurVertVel += float((GRAVITY_ACC + -.1f * pow(m_fCurVertVel, 2) * m_fResistanceFactor) * fTickTime);
	m_fCurBlastVel += float(-.1f * pow(m_fCurBlastVel, 2) * m_fResistanceFactor * fTickTime);
	if (m_fCurBlastVel < 0.f)	m_fCurBlastVel = 0.f;

	return true;
}

void FWModifierParabola::InitParam()
{
	m_fBlastVel = 20.f;
	m_fResistanceFactor = 1.f;
	RadomParamForAll();
	RadomParamForSingle();
}

void FWModifierParabola::RadomParamForSingle()
{
	m_vecBlastDir.x = a_Random(-1.f, 1.f);
	m_vecBlastDir.y = a_Random(-1.f, 1.f);
	m_vecBlastDir.z = a_Random(-1.f, 1.f);
	m_vecBlastDir.Normalize();
}

int FWModifierParabola::GetID()
{
	return FWModifierCreator::ID_MODIFIER_PARABOLA;
}


void FWModifierParabola::Serialize(FWArchive &ar)
{
	FWModifier::Serialize(ar);

	if (ar.IsStoring())
	{
		ar << m_fResistanceFactor;
		ar << m_fBlastVel;
	}
	else
	{
		ar >> m_fResistanceFactor;
		ar >> m_fBlastVel;
		RadomParamForSingle();
	}
}

void FWModifierParabola::FillParamArray(FWParamArray &arrayParam) const
{
	arrayParam.Add(FWParam(m_fBlastVel, 0.f, 40.f, _AL("爆炸初速度")));
	arrayParam.Add(FWParam(m_fResistanceFactor, 0.F, 1.f, _AL("阻力系数")));
}

void FWModifierParabola::UpdateFromParamArray(const FWParamArray &arrayParam)
{
	m_fBlastVel = arrayParam[0].GetVal();
	m_fResistanceFactor = arrayParam[1].GetVal();
}
