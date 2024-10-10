#ifndef _CURVEEMITTER_H_
#define _CURVEEMITTER_H_

#include "A3DParticleEmitter.h"

class A3DGFXCtrlCurveMove;
class A3DCurveEmitter : public A3DParticleEmitter
{
public:
	A3DCurveEmitter() :
	m_pCurve(0),
	m_pDir(0),
	m_PtCount(0),
	m_fRadius(0),
	m_fScale(1.f),
	m_nSegmentNum(0),
	m_fEmitterStep(0),
	m_bIsAvg(false),
	m_bIsAvgGensequentially(true)
	{
		m_nType = EMITTER_TYPE_CURVE;
		m_pKeyPoint = new A3DGFXKeyPoint;
	}

	virtual ~A3DCurveEmitter()
	{
		delete m_pKeyPoint;
		delete[] m_pDir;
	}

protected:
	A3DGFXCtrlCurveMove* m_pCurve;
	A3DVECTOR3* m_pDir;
	size_t m_PtCount;
	float m_fRadius;
	float m_fScale;
	float m_fEmitterStep;
	int m_nSegmentNum;
	bool m_bIsAvgGensequentially;
	bool m_bIsAvg;

public:
	void InitParticle(A3DParticle* pParticle);
	int GetEmissionCount(float fTimeSpan) { return m_pCurve ? GenEmissionCount(fTimeSpan) : 0; }
	void Init();
	virtual bool Load(AFile* pFile, DWORD dwVersion);
	virtual bool Save(AFile* pFile);
	virtual void CloneFrom(const A3DParticleEmitter* p);

	virtual bool SetProperty(int nOp, const GFX_PROPERTY& prop)
	{
		switch (nOp)
		{
		case ID_GFXOP_EMITTER_CURVE_R:
			m_fRadius = prop;
			break;
		case ID_GFXOP_EMITTER_CURVE_ISAVG:
			m_bIsAvg = prop;
			break;
		case ID_GFXOP_EMITTER_CURVE_AVGSEGNUM:
			m_nSegmentNum = prop;
			break;
		case ID_GFXOP_EMITTER_CURVE_AVGGENSEQUTIALLY:
			m_bIsAvgGensequentially = prop;
			break;
		default:
			if (!A3DParticleEmitter::SetProperty(nOp, prop))
				return false;
		}

		return true;
	}

	virtual GFX_PROPERTY GetProperty(int nOp) const
	{
		switch (nOp)
		{
		case ID_GFXOP_EMITTER_CURVE_R:
			return GFX_PROPERTY(m_fRadius);
		case ID_GFXOP_EMITTER_CURVE_ISAVG:
			return GFX_PROPERTY(m_bIsAvg);
		case ID_GFXOP_EMITTER_CURVE_AVGSEGNUM:
			return GFX_PROPERTY(m_nSegmentNum);
		case ID_GFXOP_EMITTER_CURVE_AVGGENSEQUTIALLY:
			return GFX_PROPERTY(m_bIsAvgGensequentially);
		default:
			return A3DParticleEmitter::GetProperty(nOp);
		}

		return GFX_PROPERTY();
	}

	virtual void Resume();
	virtual void UpdateEmitterScale(float fScale);
};

#endif
