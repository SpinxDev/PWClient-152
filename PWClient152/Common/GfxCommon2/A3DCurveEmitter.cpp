#include "StdAfx.h"
#include "A3DCurveEmitter.h"
#include "A3DGFXKeyPoint.h"
#include "A3DGFXElement.h"
#include "A3DParticleSystemEx.h"

static const char* _format_radius = "Radius: %f";
static const char* _format_isavg = "IsAvgGen: %d";
static const char* _format_avgseg = "AvgGenSeg: %d";
static const char* _format_isavggenseq = "IsAvgGenSeq: %d"; 

//////////////////////////////////////////////////////////////////////////
// Local Functions
//////////////////////////////////////////////////////////////////////////

static float genNextStep(bool bIsSeq, float fCurStep, int nSegNum)
{
	float fNextStep = fCurStep;
	float fStep = 1 / (float)nSegNum;
	if (bIsSeq)
		fNextStep = fCurStep + fStep;
	else
		fNextStep = (int)(_UnitRandom() * nSegNum) * fStep;

	return fNextStep > 1.f ? 0.f : fNextStep;
}

//////////////////////////////////////////////////////////////////////////
// Implementation of A3DCurveEmitter
//////////////////////////////////////////////////////////////////////////

void A3DCurveEmitter::InitParticle(A3DParticle* pParticle)
{
	assert(m_pCurve && m_pDir);
	A3DParticleEmitter::InitParticle(pParticle);
	A3DQUATERNION vDir;
	size_t index = 0;

	if (m_bIsAvg && m_pCurve->GetInterpPoint(m_fEmitterStep, pParticle->m_vPos, vDir, index))
		m_fEmitterStep = genNextStep(m_bIsAvgGensequentially, m_fEmitterStep, m_nSegmentNum > 1 ? m_nSegmentNum : 1);
	else if (m_pCurve->GetInterpPoint(_UnitRandom(), pParticle->m_vPos, vDir, index))
		;
	else
	// Else Get interpPoint failed, do something
	{
		pParticle->m_vPos.Clear();
		vDir.Normalize();
	}

	if (m_fRadius > 1e-3f)
	{
		float fRadius = m_bSurface ? m_fRadius : m_fRadius * _UnitRandom();
		A3DQUATERNION q(m_pDir[index], _UnitRandom() * A3D_2PI);
		pParticle->m_vPos += RotateVec(q,  CalcVertVec(m_pDir[index])) * fRadius;
	}

	if (!m_bBind)
	{
		//	为什么只有这里乘了m_fScale？
		//	而其他所有类似的代码都没有乘？ 其他代码已被提到函数initParticlePosDirByBindAndDragProperty中，可直接调用之初始化
		//	因为这里不同，所以无法公用该函数
		pParticle->m_vPos *= m_fScale;
		
		if (m_bDrag)
			pParticle->m_vOrgOffset = m_vParentPos;

		pParticle->m_vPos += m_vParentPos;
		pParticle->m_vDir = m_qParticleDir;
	}

	GenDirection(pParticle->m_vMoveDir);
	pParticle->m_vOldPos = pParticle->m_vPos;
	pParticle->m_vOldMoveDir = pParticle->m_vMoveDir;
	pParticle->m_fTotalTTL = GenTTL();
	pParticle->m_fTTL = 0;
}

void A3DCurveEmitter::Init()
{
	m_pCurve = static_cast<A3DGFXCtrlCurveMove*>(m_pKeyPoint->GetCtrlMethodByType(ID_KPCTRL_CURVE_MOVE));

	if (!m_pCurve)
		return;

	m_PtCount = m_pCurve->GetBezierPtCount();

	if (m_PtCount < 2)
		return;

	if (m_pDir)
		delete [] m_pDir;
	
	m_pDir = new A3DVECTOR3[m_PtCount];
	const A3DGFXCtrlCurveMove::BezierPt* pts = m_pCurve->GetBezierPt();

	for (size_t i = 0; i < m_PtCount - 1; i++)
	{
		A3DVECTOR3& dir = m_pDir[i];
		dir = pts[i + 1].m_vec - pts[i].m_vec;
		dir.Normalize();
	}

	m_pDir[m_PtCount - 1] = m_pDir[m_PtCount - 2];
}

bool A3DCurveEmitter::Load(AFile* pFile, DWORD dwVersion)
{
	if (!A3DParticleEmitter::Load(pFile, dwVersion))
		return false;

	if (pFile->IsBinary())
	{
		DWORD dwRead;
		pFile->Read(&m_fRadius, sizeof(m_fRadius), &dwRead);

		if (dwVersion >= 66)
		{
			pFile->Read(&m_bIsAvg, sizeof(m_bIsAvg), &dwRead);
			pFile->Read(&m_nSegmentNum, sizeof(m_nSegmentNum), &dwRead);
			pFile->Read(&m_bIsAvgGensequentially, sizeof(m_bIsAvgGensequentially), &dwRead);
		}
	}
	else
	{
		char	szLine[AFILE_LINEMAXLEN];
		DWORD	dwReadLen;

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
		sscanf(szLine, _format_radius, &m_fRadius);

		if (dwVersion >= 66)
		{
			int nReadData = 0;
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_isavg, &nReadData);
			m_bIsAvg = nReadData != 0;

			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_avgseg, &m_nSegmentNum);

			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwReadLen);
			sscanf(szLine, _format_isavggenseq, &nReadData);
			m_bIsAvgGensequentially = nReadData != 0;
		}
	}

	if (!m_pKeyPoint->Load(pFile, dwVersion))
		return false;

	Init();
	return true;
}

bool A3DCurveEmitter::Save(AFile* pFile)
{
	if (!A3DParticleEmitter::Save(pFile))
		return false;

	if (pFile->IsBinary())
	{
		DWORD dwWrite;
		pFile->Write(&m_fRadius, sizeof(m_fRadius), &dwWrite);

		// For version >= 66
		pFile->Write(&m_bIsAvg, sizeof(m_bIsAvg), &dwWrite);
		pFile->Write(&m_nSegmentNum, sizeof(m_nSegmentNum), &dwWrite);
		pFile->Write(&m_bIsAvgGensequentially, sizeof(m_bIsAvgGensequentially), &dwWrite);
	}
	else
	{
		char	szLine[AFILE_LINEMAXLEN];

		_snprintf(szLine, AFILE_LINEMAXLEN, _format_radius, m_fRadius);
		pFile->WriteLine(szLine);

		// For version >= 66
		_snprintf(szLine, AFILE_LINEMAXLEN, _format_isavg, m_bIsAvg);
		pFile->WriteLine(szLine);

		_snprintf(szLine, AFILE_LINEMAXLEN, _format_avgseg, m_nSegmentNum);
		pFile->WriteLine(szLine);

		_snprintf(szLine, AFILE_LINEMAXLEN, _format_isavggenseq, m_bIsAvgGensequentially);
		pFile->WriteLine(szLine);
	}

	return m_pKeyPoint->Save(pFile);
}

void A3DCurveEmitter::CloneFrom(const A3DParticleEmitter* p)
{
	A3DParticleEmitter::CloneFrom(p);
	const A3DCurveEmitter* pSrc = static_cast<const A3DCurveEmitter*>(p);
	m_PtCount = pSrc->m_PtCount;
	m_pDir = new A3DVECTOR3 [m_PtCount];
	memcpy(m_pDir, pSrc->m_pDir, sizeof(A3DVECTOR3) * m_PtCount);
	*m_pKeyPoint = *pSrc->m_pKeyPoint;
	m_pCurve = static_cast<A3DGFXCtrlCurveMove*>(m_pKeyPoint->GetCtrlMethodByType(ID_KPCTRL_CURVE_MOVE));
	m_fRadius = pSrc->m_fRadius;
	
	m_bIsAvg = pSrc->m_bIsAvg;
	m_nSegmentNum = pSrc->m_nSegmentNum;
	m_fEmitterStep = pSrc->m_fEmitterStep;
	m_bIsAvgGensequentially = pSrc->m_bIsAvgGensequentially;
}

void A3DCurveEmitter::Resume()
{
#ifdef GFX_EDITOR

	A3DParticleEmitter::Resume();
	Init();

#endif

	m_fEmitterStep = 0.f;
}

void A3DCurveEmitter::UpdateEmitterScale(float fScale)
{
	m_fScale = fScale;
}