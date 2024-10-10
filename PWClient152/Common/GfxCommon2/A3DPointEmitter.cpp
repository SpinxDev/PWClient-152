#include "StdAfx.h"
#include "A3DPointEmitter.h"
#include "A3DGFXKeyPoint.h"
#include "A3DGFXElement.h"

static const float _init_delta = .001f; // add some delta value

void A3DPointEmitter::InitParticle(A3DParticle* pParticle)
{
	A3DParticleEmitter::InitParticle(pParticle);
	static const float fHalfPI = A3D_PI * .5f;

	if (m_bSurface)
	{
		A3DQUATERNION q(_unit_z, _UnitRandom() * A3D_2PI);
		A3DVECTOR3 vUp = RotateVec(q, _unit_x);
		A3DQUATERNION q1(vUp, fHalfPI - m_fAngle);
		pParticle->m_vDir = q1 * q;

		if (!m_bBind)
			pParticle->m_vDir = /*m_qParentDir*/ m_qParticleDir * pParticle->m_vDir;

		pParticle->m_vMoveDir = RotateVec(pParticle->m_vDir, _unit_y);

		if (m_bBind)
			pParticle->m_vPos = pParticle->m_vMoveDir * _init_delta;
		else
		{
			A3DVECTOR3 vOrgOff = pParticle->m_vMoveDir * _init_delta;
			pParticle->m_vPos = m_vParentPos + vOrgOff;

			if (m_bDrag)
				pParticle->m_vOrgOffset = m_vParentPos;
		}
	}
	else
	{
		GenDirection(pParticle->m_vMoveDir);

		if (m_bBind)
			pParticle->m_vPos = pParticle->m_vMoveDir * _init_delta;
		else
		{
			A3DVECTOR3 vOrgOff = pParticle->m_vMoveDir * _init_delta;
			pParticle->m_vPos = m_vParentPos + vOrgOff;
//			pParticle->m_vDir = m_qParentDir;
			pParticle->m_vDir = m_qParticleDir;

			if (m_bDrag)
				pParticle->m_vOrgOffset = m_vParentPos;
		}
	}

	pParticle->m_vOldPos = pParticle->m_vPos;
	pParticle->m_vOldMoveDir	= pParticle->m_vMoveDir;
	pParticle->m_fTotalTTL = GenTTL();
	pParticle->m_fTTL = 0;
}
