#include "StdAfx.h"
#include "A3DEllipsoidEmitter.h"
#include "A3DGFXKeyPoint.h"
#include "A3DGFXElement.h"

static const char _format_isgenaverage[] = "IsAvgGen: %d";
static const char _format_alphasegnum[] = "AlphaSeg: %d";
static const char _format_betasegnum[] = "BetaSeg: %d";


void A3DEllipsoidEmitter::CloneFrom(const A3DParticleEmitter* p)
{
	A3DAreaEmitter::CloneFrom(p);
	const A3DEllipsoidEmitter* pEllipsoidEmitter = static_cast<const A3DEllipsoidEmitter*>(p);
	m_bIsGenAverage = pEllipsoidEmitter->m_bIsGenAverage;
	m_nAlphaSegNum = pEllipsoidEmitter->m_nAlphaSegNum;
	m_nBetaSegNum = pEllipsoidEmitter->m_nBetaSegNum;
	m_fAngleAlpha = pEllipsoidEmitter->m_fAngleAlpha;
	m_fAngleBeta = pEllipsoidEmitter->m_fAngleBeta;
}

bool A3DEllipsoidEmitter::Load(AFile* pFile, DWORD dwVersion)
{
	if (!A3DAreaEmitter::Load(pFile, dwVersion))
		return false;

	if (pFile->IsBinary())
	{

	}
	else if (dwVersion >= 71)
	{
		char	szLine[AFILE_LINEMAXLEN];
		DWORD	dwReadLen;

		int nRead = 0;
		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_isgenaverage, &nRead);
		m_bIsGenAverage = nRead != 0;

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_alphasegnum, &m_nAlphaSegNum);

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_betasegnum, &m_nBetaSegNum);
	}

	return true;
}

bool A3DEllipsoidEmitter::Save(AFile* pFile)
{
	if (!A3DAreaEmitter::Save(pFile))
		return false;

	if (pFile->IsBinary())
	{
	}
	else
	{
		char	szLine[AFILE_LINEMAXLEN];

		_snprintf(szLine, AFILE_LINEMAXLEN, _format_isgenaverage, m_bIsGenAverage);
		pFile->WriteLine(szLine);

		_snprintf(szLine, AFILE_LINEMAXLEN, _format_alphasegnum, m_nAlphaSegNum);
		pFile->WriteLine(szLine);

		_snprintf(szLine, AFILE_LINEMAXLEN, _format_betasegnum, m_nBetaSegNum);
		pFile->WriteLine(szLine);
	}

	return true;
}

void A3DEllipsoidEmitter::InitParticle(A3DParticle* pParticle)
{
    // Call superclass
    A3DParticleEmitter::InitParticle(pParticle);

	if (m_bIsGenAverage)
		GenAverage(pParticle);
	else {
		if (m_bSurface)
			GenSurface(pParticle);
		else
			GenTotal(pParticle);
	}
	
	pParticle->m_vOldPos = pParticle->m_vPos;
	pParticle->m_vOldMoveDir = pParticle->m_vMoveDir;
	pParticle->m_fTotalTTL = GenTTL();
	pParticle->m_fTTL = 0;
}

void A3DEllipsoidEmitter::GenSurface(A3DParticle* pParticle)
{
	A3DQUATERNION q(_unit_y, _UnitRandom() * A3D_2PI);
	A3DVECTOR3 v = RotateVec(q, _unit_x);
	q.ConvertFromAxisAngle(v, _UnitRandom() * A3D_2PI);
	v = RotateVec(q, _unit_y);

    A3DVECTOR3 xOff, yOff, zOff;
    xOff = v.x * m_vXRange;
    yOff = v.y * m_vYRange;
    zOff = v.z * m_vZRange;

	pParticle->m_vMoveDir = -v;
    pParticle->m_vPos = xOff + yOff + zOff;

	initParticlePosDirByBindAndDragProperty(pParticle);
}

void A3DEllipsoidEmitter::GenTotal(A3DParticle* pParticle)
{
	float x, y, z;

	while (true)
	{
		x = _SymmetricRandom();
		y = _SymmetricRandom();
		z = _SymmetricRandom();

		if (x * x + y * y + z * z <= 1.0f) break;
	}

	pParticle->m_vPos = x * m_vXRange + y * m_vYRange + z * m_vZRange;

	initParticlePosDirByBindAndDragProperty(pParticle);

	GenDirection(pParticle->m_vMoveDir);
	pParticle->m_vOldMoveDir = pParticle->m_vMoveDir;
}

void A3DEllipsoidEmitter::GenAverage(A3DParticle* pParticle)
{
	float fCurAlpha = m_fAngleAlpha;
	float fCurBeta = m_fAngleBeta;

	A3DQUATERNION qRotAlpha(_unit_y, fCurAlpha);
	A3DQUATERNION qRotBeta(_unit_x, fCurBeta);
	A3DVECTOR3 vInitPos(0, 1, 0);
	vInitPos = qRotBeta * vInitPos;
	vInitPos = qRotAlpha * vInitPos;
	
	float fRatio = _UnitRandom();
    A3DVECTOR3 xOff, yOff, zOff;
    xOff = vInitPos.x * m_vXRange * (m_bSurface ? 1.f : fRatio);
    yOff = vInitPos.y * m_vYRange * (m_bSurface ? 1.f : fRatio);
    zOff = vInitPos.z * m_vZRange * (m_bSurface ? 1.f : fRatio);

	pParticle->m_vPos = xOff + yOff + zOff;

	initParticlePosDirByBindAndDragProperty(pParticle);
	GenDirection(pParticle->m_vMoveDir);
	pParticle->m_vOldMoveDir = pParticle->m_vMoveDir;

	float fAlphaDelta = A3D_2PI / m_nAlphaSegNum;
	float fBetaDelta = A3D_PI / m_nBetaSegNum;

	if (fabs(m_fAngleBeta) < FLT_EPSILON)
	{
		m_fAngleAlpha = 0;
		m_fAngleBeta += fBetaDelta;
	}
	else
	{
		m_fAngleAlpha += fAlphaDelta;
		if (m_fAngleAlpha > A3D_2PI)
		{
			m_fAngleAlpha = 0;
			m_fAngleBeta += fBetaDelta;
		}
		else if ((double)m_fAngleBeta + fBetaDelta / 2 > A3D_PI)
		{
			m_fAngleAlpha = 0;
			m_fAngleBeta = 0;
		}
	}

}

void A3DEllipsoidEmitter::Resume()
{
	A3DParticleEmitter::Resume();
	m_fAngleAlpha = 0;
	m_fAngleBeta = 0;
}