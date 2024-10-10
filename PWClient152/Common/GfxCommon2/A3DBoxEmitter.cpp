#include "StdAfx.h"
#include "A3DBoxEmitter.h"

void A3DBoxEmitter::InitParticle(A3DParticle* pParticle)
{
    // Call superclass
    A3DParticleEmitter::InitParticle(pParticle);
	
	if (m_bSurface)
		GenSurface(pParticle);
	else
		GenTotal(pParticle);

    GenDirection(pParticle->m_vMoveDir);
	pParticle->m_vOldPos = pParticle->m_vPos;
	pParticle->m_vOldMoveDir = pParticle->m_vMoveDir;
	pParticle->m_fTotalTTL = GenTTL();
	pParticle->m_fTTL = 0;
}

void A3DBoxEmitter::GenSurface(A3DParticle* pParticle)
{
	float fS1 = m_vSize.x * m_vSize.y;
	float fS2 = m_vSize.y * m_vSize.z;
	float fS3 = m_vSize.z * m_vSize.x;
	float fS = fS1 + fS2 + fS3;
	fS1 /= fS;
	fS2 /= fS;
	fS3 /= fS;

	float fRand = _UnitRandom();
	float xOff, yOff, zOff;
	if (fRand < fS1)
	{
		zOff = _UnitRandom() < .5f ? -1.f : 1.f;
		xOff = _SymmetricRandom();
		yOff = _SymmetricRandom();
	}
	else if (fRand < fS2)
	{
		xOff = _UnitRandom() < .5f ? -1.f : 1.f;
		yOff = _SymmetricRandom();
		zOff = _SymmetricRandom();
	}
	else
	{
		yOff = _UnitRandom() < .5f ? -1.f : 1.f;
		xOff = _SymmetricRandom();
		zOff = _SymmetricRandom();
	}

	pParticle->m_vPos = xOff * m_vXRange + yOff * m_vYRange + zOff * m_vZRange;

	initParticlePosDirByBindAndDragProperty(pParticle);
}

void A3DBoxEmitter::GenTotal(A3DParticle* pParticle)
{
    A3DVECTOR3 xOff, yOff, zOff;
    xOff = _SymmetricRandom() * m_vXRange;
    yOff = _SymmetricRandom() * m_vYRange;
    zOff = _SymmetricRandom() * m_vZRange;

    pParticle->m_vPos = xOff + yOff + zOff;

	initParticlePosDirByBindAndDragProperty(pParticle);
}
