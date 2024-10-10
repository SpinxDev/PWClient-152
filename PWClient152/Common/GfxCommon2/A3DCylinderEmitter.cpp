#include "StdAfx.h"
#include "A3DCylinderEmitter.h"
#include "A3DGFXElement.h"

static const char _format_isgenaverage[] = "IsAvgGen: %d";
static const char _format_alphasegnum[] = "AlphaSeg: %d";
static const char _format_betasegnum[] = "BetaSeg: %d";

void A3DCylinderEmitter::InitParticle(A3DParticle* pParticle)
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

	GenDirection(pParticle->m_vMoveDir);
	pParticle->m_vOldPos = pParticle->m_vPos;
	pParticle->m_vOldMoveDir = pParticle->m_vMoveDir;
	pParticle->m_fTotalTTL = GenTTL();
	pParticle->m_fTTL = 0;
}

void A3DCylinderEmitter::GenSurface(A3DParticle* pParticle)
{
	A3DQUATERNION q(_unit_z, _UnitRandom() * A3D_2PI);
	A3DVECTOR3 v = RotateVec(q, _unit_x);
	float z = _SymmetricRandom();

	pParticle->m_vPos = v.x * m_vXRange + v.y * m_vYRange + z * m_vZRange;

	initParticlePosDirByBindAndDragProperty(pParticle);
}

void A3DCylinderEmitter::GenTotal(A3DParticle* pParticle)
{
	float x, y, z;
	while (true)
	{
		x = _SymmetricRandom();
		y = _SymmetricRandom();
		z = _SymmetricRandom();
		if (x * x + y * y <= 1.0f) break;
	}

	pParticle->m_vPos = x * m_vXRange + y * m_vYRange + z * m_vZRange;

	initParticlePosDirByBindAndDragProperty(pParticle);
}

void A3DCylinderEmitter::GenAverage( A3DParticle* pParticle )
{
	float fCurAlpha = m_fAngleAlpha;

	A3DQUATERNION qRotAlpha(_unit_z, fCurAlpha);
	A3DVECTOR3 vInitPos(0, 1, 1);
	vInitPos.z = 1 - 2 * ((float)m_nCurrentBeta / m_nBetaSegNum);
	vInitPos = qRotAlpha * vInitPos;

	float fRatio = _UnitRandom();
	A3DVECTOR3 xOff, yOff, zOff;
	xOff = vInitPos.x * m_vXRange * (m_bSurface ? 1.f : fRatio);
	yOff = vInitPos.y * m_vYRange * (m_bSurface ? 1.f : fRatio);
	zOff = vInitPos.z * m_vZRange * (m_bSurface ? 1.f : fRatio);

	pParticle->m_vPos = xOff + yOff + zOff;

	initParticlePosDirByBindAndDragProperty(pParticle);

	float fAlphaDelta = A3D_2PI / m_nAlphaSegNum;
	float fBetaDelta = A3D_PI / m_nBetaSegNum;

	m_fAngleAlpha += fAlphaDelta;
	if (m_fAngleAlpha > A3D_2PI)
	{
		m_fAngleAlpha = 0;
		++m_nCurrentBeta;
	}
	
	if (m_nCurrentBeta > m_nBetaSegNum)
	{
		m_fAngleAlpha = 0;
		m_nCurrentBeta = 0;
	}
}

void A3DCylinderEmitter::Resume()
{
	A3DParticleEmitter::Resume();
	m_fAngleAlpha = 0;
	m_nCurrentBeta = 0;
}

bool A3DCylinderEmitter::Load( AFile* pFile, DWORD dwVersion )
{
	if (!A3DAreaEmitter::Load(pFile, dwVersion))
		return false;

	if (pFile->IsBinary())
	{

	}
	else if (dwVersion >= 99)
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

bool A3DCylinderEmitter::Save( AFile* pFile )
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

void A3DCylinderEmitter::CloneFrom( const A3DParticleEmitter* p )
{
	A3DAreaEmitter::CloneFrom(p);
	const A3DCylinderEmitter* pCylinderEmitter = static_cast<const A3DCylinderEmitter*>(p);
	m_bIsGenAverage = pCylinderEmitter->m_bIsGenAverage;
	m_nAlphaSegNum = pCylinderEmitter->m_nAlphaSegNum;
	m_nBetaSegNum = pCylinderEmitter->m_nBetaSegNum;
	m_fAngleAlpha = pCylinderEmitter->m_fAngleAlpha;
	m_nCurrentBeta = pCylinderEmitter->m_nCurrentBeta;
}