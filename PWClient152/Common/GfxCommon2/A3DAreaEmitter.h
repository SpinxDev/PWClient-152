/*
 * FILE: A3DAreaEmitter.h
 *
 * DESCRIPTION: Area Emitter
 *
 * CREATED BY: ZhangYu, 2004/8/10
 *
 * HISTORY:
 *
 */

#ifndef _A3DAREAEMITTER_H_
#define _A3DAREAEMITTER_H_

#include "A3DParticleEmitter.h"

class A3DAreaEmitter : public A3DParticleEmitter
{
public:
	A3DAreaEmitter() : m_vSize(1.0f) {}
	virtual ~A3DAreaEmitter() {}

protected:
	// size of the area
	A3DVECTOR3	m_vSize;

	// local axis
	A3DVECTOR3	m_vXRange;
	A3DVECTOR3	m_vYRange;
	A3DVECTOR3	m_vZRange;

public:
	int GetEmissionCount(float fTimeSpan) { return GenEmissionCount(fTimeSpan); }

public:
	virtual bool SetProperty(int nOp, const GFX_PROPERTY& prop)
	{
		switch (nOp)
		{
		case ID_GFXOP_EMITTER_AREA_SIZE:
			m_vSize = prop;
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
		case ID_GFXOP_EMITTER_AREA_SIZE:
			return GFX_PROPERTY(m_vSize);
		default:
			return A3DParticleEmitter::GetProperty(nOp);
		}
		return GFX_PROPERTY();
	}
	
	virtual void UpdateEmitterScale(float fScale);
	virtual bool Load(AFile* pFile, DWORD dwVersion);
	virtual bool Save(AFile* pFile);
	virtual void CloneFrom(const A3DParticleEmitter* p);
};

#endif