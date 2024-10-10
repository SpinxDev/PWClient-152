/*
 * FILE: A3DEllipsoidEmitter.h
 *
 * DESCRIPTION: Ellipsoid Emitter
 *
 * CREATED BY: ZhangYu, 2004/8/14
 *
 * HISTORY:
 *
 */

#ifndef _A3DELLIPSOIDEMITTER_H_
#define _A3DELLIPSOIDEMITTER_H_

#include "A3DAreaEmitter.h"

class A3DEllipsoidEmitter : public A3DAreaEmitter
{
public:
	A3DEllipsoidEmitter() : 
	    m_bIsGenAverage(false),
		m_fAngleAlpha(0), 
		m_fAngleBeta(0), 
		m_nAlphaSegNum(10),
		m_nBetaSegNum(10)
		{ m_nType = EMITTER_TYPE_ELLIPSOID; }

public:
	void InitParticle(A3DParticle* pParticle);
	
protected:
	void GenSurface(A3DParticle* pParticle);
	void GenTotal(A3DParticle* pParticle);
	void GenAverage(A3DParticle* pParticle);
	void Resume();

	virtual bool SetProperty(int nOp, const GFX_PROPERTY& prop)
	{
		switch (nOp)
		{
		case ID_GFXOP_EMITTER_ELLIPSOID_ISAVG:
			m_bIsGenAverage = prop;
			break;
		case ID_GFXOP_EMITTER_ELLIPSOID_ALPHASEG:
			m_nAlphaSegNum = prop;
			if (m_nAlphaSegNum <= 0)
				m_nAlphaSegNum = 1;
			break;
		case ID_GFXOP_EMITTER_ELLIPSOID_BETASEG:
			m_nBetaSegNum = prop;
			if (m_nBetaSegNum <= 0)
				m_nBetaSegNum = 1;
			m_nBetaSegNum = prop;
			break;
		}
		return A3DAreaEmitter::SetProperty(nOp, prop);
	}
	virtual GFX_PROPERTY GetProperty(int nOp) const
	{
		switch (nOp)
		{
		case ID_GFXOP_EMITTER_ELLIPSOID_ISAVG:
			return GFX_PROPERTY(m_bIsGenAverage);	
		case ID_GFXOP_EMITTER_ELLIPSOID_ALPHASEG:
			return GFX_PROPERTY(m_nAlphaSegNum);
		case ID_GFXOP_EMITTER_ELLIPSOID_BETASEG:
			return GFX_PROPERTY(m_nBetaSegNum);
		}
		return A3DAreaEmitter::GetProperty(nOp);
	}

	virtual bool Load(AFile* pFile, DWORD dwVersion);
	virtual bool Save(AFile* pFile);
	virtual void CloneFrom(const A3DParticleEmitter* p);

private:
	bool m_bIsGenAverage;
	int m_nAlphaSegNum;
	int m_nBetaSegNum;

	// runtime data
	float m_fAngleAlpha;
	float m_fAngleBeta;
};

#endif