/*
 * FILE: A3DPointEmitter.h
 *
 * DESCRIPTION: Point Emitter
 *
 * CREATED BY: ZhangYu, 2004/8/3
 *
 * HISTORY:
 *
 */

#ifndef _A3DPOINTEMITTER_H_
#define _A3DPOINTEMITTER_H_

#include "A3DParticleEmitter.h"

class A3DPointEmitter : public A3DParticleEmitter
{
public:
	A3DPointEmitter() { m_nType = EMITTER_TYPE_POINT; }

	void InitParticle(A3DParticle* pParticle);
	int GetEmissionCount(float fTimeSpan) { return GenEmissionCount(fTimeSpan); }
};

#endif