/*
 * FILE: A3DBoxEmitter.h
 *
 * DESCRIPTION: Box Emitter
 *
 * CREATED BY: ZhangYu, 2004/8/10
 *
 * HISTORY:
 *
 */

#ifndef _A3DBOXEMITTER_H_
#define _A3DBOXEMITTER_H_

#include "A3DAreaEmitter.h"

class A3DBoxEmitter : public A3DAreaEmitter
{
public:
	A3DBoxEmitter()	{ m_nType = EMITTER_TYPE_BOX; }

public:
	void InitParticle(A3DParticle* pParticle);
	
protected:
	void GenSurface(A3DParticle* pParticle);
	void GenTotal(A3DParticle* pParticle);
};

#endif