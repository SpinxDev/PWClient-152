#include "StdAfx.h"
#include "A3DGFXLightningEx.h"
#include "A3DGFXEx.h"
#include "A3DGFXRenderSlot.h"
#include "A3D.h"

//////////////////////////////////////////////////////////////////////////
// Local variables
//////////////////////////////////////////////////////////////////////////

const static int s_LightningMaxSegNum = 256;

const static char _format_isappend[] = "isappend: %d";
const static char _format_renderside[] = "renderside: %d";
const static char _format_istaildisappear[] = "istaildisappear: %d";
const static char _format_vertslife[] = "vertslife: %d";
const static char _format_tailfadeout[] = "tailfadeout: %d";

//////////////////////////////////////////////////////////////////////////
// Local Functions
//////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////
// Implementation fo A3DGFXLightningEx
//////////////////////////////////////////////////////////////////////////


A3DGFXLightningEx::A3DGFXLightningEx(A3DGFXEx* pGfx) : 
A3DGFXLightning(pGfx),
m_bIsAppendly(true),
m_bIsUseVertsLife(true),
m_bIsTailFadeout(false),
m_nVertsLife(1000),
m_nTimeSpan(0),
m_nTimeStep(0),
m_nCurrentIndex(-1),
m_nRenderSide(RST_MID),
m_lstCurvePoints(s_LightningMaxSegNum)
{
	m_nEleType = ID_ELE_TYPE_LIGHTNINGEX;
}

A3DGFXLightningEx& A3DGFXLightningEx::operator = (const A3DGFXLightningEx& src)
{
	if (&src == this)
		return *this;

	// Assignment for base class
	A3DGFXLightning::operator =(src);
	
	// Assignment for current class
	// ...
	m_bIsAppendly = src.m_bIsAppendly;
	m_nRenderSide = src.m_nRenderSide;

	m_bIsUseVertsLife = src.m_bIsUseVertsLife;
	m_nVertsLife = src.m_nVertsLife;

	m_bIsTailFadeout = src.m_bIsTailFadeout;
	
	m_nTimeSpan = src.m_nTimeSpan;
	m_nTimeStep = src.m_nTimeStep;
	m_nCurrentIndex = src.m_nCurrentIndex;
	

	return *this;
}

A3DGFXElement* A3DGFXLightningEx::Clone(A3DGFXEx* pGfx) const
{
	A3DGFXLightningEx* pLightningEx = new A3DGFXLightningEx(pGfx);
	return &(*pLightningEx = *this);
}

bool A3DGFXLightningEx::Load(A3DDevice * pDevice, AFile* pFile, DWORD dwVersion)
{
	if (!A3DGFXLightning::Load(pDevice, pFile, dwVersion))
		return false;

	char szLine[AFILE_LINEMAXLEN];
	DWORD dwRead;
	if (pFile->IsBinary())
	{
		if (dwVersion >= 67)
		{
			pFile->Read(&m_bIsAppendly, sizeof(m_bIsAppendly), &dwRead);
			pFile->Read(&m_nRenderSide, sizeof(m_nRenderSide), &dwRead);
		}

		if (dwVersion >= 68)
		{
			pFile->Read(&m_bIsUseVertsLife, sizeof(m_bIsUseVertsLife), &dwRead);
			pFile->Read(&m_nVertsLife, sizeof(m_nVertsLife), &dwRead);
		}

		if (dwVersion >= 69)
			pFile->Read(&m_bIsTailFadeout, sizeof(m_bIsTailFadeout), &dwRead);
	}
	else if (dwVersion >= 67)
	{
		int nReadData = 0;
		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwRead);
		sscanf(szLine, _format_isappend, &nReadData);
		m_bIsAppendly = nReadData != 0;

		pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwRead);
		sscanf(szLine, _format_renderside, &m_nRenderSide);
		if (m_nRenderSide != A3DGFXLightningEx::RST_DOWN &&
			m_nRenderSide != A3DGFXLightningEx::RST_MID &&
			m_nRenderSide != A3DGFXLightningEx::RST_UP)
			m_nRenderSide = A3DGFXLightningEx::RST_MID;

		if (dwVersion >= 68)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwRead);
			sscanf(szLine, _format_istaildisappear, &nReadData);
			m_bIsUseVertsLife = nReadData != 0;

			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwRead);
			sscanf(szLine, _format_vertslife, &m_nVertsLife);
		}

		if (dwVersion >= 69)
		{
			pFile->ReadLine(szLine, AFILE_LINEMAXLEN, &dwRead);
			sscanf(szLine, _format_tailfadeout, &nReadData);
			m_bIsTailFadeout = nReadData != 0;
		}
	}
	return true;
}

bool A3DGFXLightningEx::Save(AFile* pFile)
{
	if (!A3DGFXLightning::Save(pFile))
		return false;

	char szLine[AFILE_LINEMAXLEN];
	DWORD dwWrite;
	if (pFile->IsBinary()) 
	{
		pFile->Write(&m_bIsAppendly, sizeof(m_bIsAppendly), &dwWrite);
		pFile->Write(&m_nRenderSide, sizeof(m_nRenderSide), &dwWrite);
		pFile->Write(&m_bIsUseVertsLife, sizeof(m_bIsUseVertsLife), &dwWrite);
		pFile->Write(&m_nVertsLife, sizeof(m_nVertsLife), &dwWrite);
		pFile->Write(&m_bIsTailFadeout, sizeof(m_bIsTailFadeout), &dwWrite);
	}
	else
	{
		_snprintf(szLine, AFILE_LINEMAXLEN, _format_isappend, m_bIsAppendly);
		pFile->WriteLine(szLine);

		_snprintf(szLine, AFILE_LINEMAXLEN, _format_renderside, m_nRenderSide);
		pFile->WriteLine(szLine);

		_snprintf(szLine, AFILE_LINEMAXLEN, _format_istaildisappear, m_bIsUseVertsLife);
		pFile->WriteLine(szLine);

		_snprintf(szLine, AFILE_LINEMAXLEN, _format_vertslife, m_nVertsLife);
		pFile->WriteLine(szLine);

		_snprintf(szLine, AFILE_LINEMAXLEN, _format_tailfadeout, m_bIsTailFadeout);
		pFile->WriteLine(szLine);
	}

	return true;
}

bool A3DGFXLightningEx::Play()
{
	if (!A3DGFXLightning::Play())
		return false;

	if (m_bCurvedMethod && m_bIsAppendly)
	{
		A3DGFXKeyPoint* pKeyPoint = m_KeyPointSet.GetKeyPoint(0);
		if (pKeyPoint && pKeyPoint->GetTimeSpan())
		{
			m_nTimeSpan = pKeyPoint->GetTimeSpan();
			m_nTimeStep = 0;
		}
	}
	return true;
}

bool A3DGFXLightningEx::Stop()
{
	if (!A3DGFXLightning::Stop())
		return false;

	m_nCurrentIndex = 0;
	m_lstCurvePoints.RemoveAll();
	return true;
}

bool A3DGFXLightningEx::TickAnimation(DWORD dwTickTime)
{
	if (!A3DGFXLightning::TickAnimation(dwTickTime))
		return false;

	// 只有渐进生成才会TickList对所有顶点进行生命计数
	if (m_bCurvedMethod && m_bIsAppendly)
	{
		TickAdd(dwTickTime);
		TickList(dwTickTime);
	}
	else if (m_lstCurvePoints.GetDataCount() != m_nVertPairNum)
	{
		m_lstCurvePoints.RemoveAll();
		for (int nIndex = 0 ; nIndex < m_nVertPairNum ; ++nIndex) 
			m_lstCurvePoints.AddData(LnCurvePoint(nIndex, m_nVertsLife));
	}

	return true;
}

void A3DGFXLightningEx::TickList(DWORD dwTickTime)
{
	int nIndex, nCount = m_lstCurvePoints.GetDataCount();
	for (nIndex = 0; nIndex < nCount ; ++nIndex) {
		LnCurvePoint& item = m_lstCurvePoints[nIndex];
		item.Tick(dwTickTime);
	}

	if (m_bIsUseVertsLife && nCount)
	{
		for (int i = nCount - 1 ; i >= 0 ; --i) {
			LnCurvePoint& item = m_lstCurvePoints[i];
			if (item.nAge == item.nLife)
			{
				m_lstCurvePoints.RemoveOldData(i + 1);
				break;
			}
		}
	}
}

void A3DGFXLightningEx::TickAdd(DWORD dwTickTime)
{
	m_nTimeStep += dwTickTime;
	int nTimePerVertAdd = m_nTimeSpan / m_nVertPairNum;
	if (nTimePerVertAdd == 0)
		return;
	
	if (m_nCurrentIndex < m_nVertPairNum && m_nTimeStep > nTimePerVertAdd)
	{
		int nNewAddedNum = m_nTimeStep / nTimePerVertAdd;
		int nOriginalIndex = m_nCurrentIndex;
		m_nCurrentIndex += nNewAddedNum;
		a_ClampRoof(m_nCurrentIndex, m_nVertPairNum);
		m_nTimeStep %= nTimePerVertAdd;
		
		for ( ; nOriginalIndex != m_nCurrentIndex ; ++nOriginalIndex) 
			m_lstCurvePoints.AddData(LnCurvePoint(nOriginalIndex, m_nVertsLife));
	}
}

bool A3DGFXLightningEx::SetProperty(int nOp, const GFX_PROPERTY& prop)
{
	switch(nOp) {
	case ID_GFXOP_LN_EX_RENDERSIDE:
		m_nRenderSide = prop;
		break;
	case ID_GFXOP_LN_EX_ISAPPENDLY:
		m_bIsAppendly = prop;
		break;
	case ID_GFXOP_LN_EX_ISUSEVERTSLIFE:
		m_bIsUseVertsLife = prop;
		break;
	case ID_GFXOP_LN_EX_VERTSLIFE:
		m_nVertsLife = prop;
		break;
	case ID_GFXOP_LN_EX_ISTAILFADEOUT:
		m_bIsTailFadeout = prop;
		break;
	default:
		return A3DGFXLightning::SetProperty(nOp, prop);
	}
	return true;
}

GFX_PROPERTY A3DGFXLightningEx::GetProperty(int nOp) const
{
	switch(nOp) {
	case ID_GFXOP_LN_EX_RENDERSIDE:
		return GFX_PROPERTY(m_nRenderSide).SetType(GFX_VALUE_RENDER_SIDE);
	case ID_GFXOP_LN_EX_ISAPPENDLY:
		return GFX_PROPERTY(m_bIsAppendly);
	case ID_GFXOP_LN_EX_ISUSEVERTSLIFE:
		return GFX_PROPERTY(m_bIsUseVertsLife);
	case ID_GFXOP_LN_EX_VERTSLIFE:
		return GFX_PROPERTY(m_nVertsLife);
	case ID_GFXOP_LN_EX_ISTAILFADEOUT:
		return GFX_PROPERTY(m_bIsTailFadeout);
	default:
		return A3DGFXLightning::GetProperty(nOp);
	}
}

void A3DGFXLightningEx::ResumeLoop()
{
	base_class::ResumeLoop();
	
	// Why test this ?
	// because before version 74, we do nothing here and in the base_class::ResumeLoop() both.
	// m_bResetWhenResumeLoop is added in version 74, it specifies whether to reset the state here.
	// This test is for compatible with the older version files created before version 74.
	if (m_bResetWhenResumeLoop) {
		m_nCurrentIndex = 0;
		m_lstCurvePoints.RemoveAll();
		
		// To make the tick add correct.
		m_nTimeStep = 0;
	}
}

int A3DGFXLightningEx::GetRenderCount() const
{
	return m_lstCurvePoints.GetDataCount();
}

void A3DGFXLightningEx::RenderCurveMethod(const int i, 
										const int nRenderCount,
										const int nBase, 
										float fRatio,
										const float fWidthStart,
										const float fWidthEnd,
										const float fWidthMid,
										float& fAlpha, 
										int& nCount,
										const A3DMATRIX4& matTran,
										const A3DVECTOR3& vNormal, 
										const A3DVECTOR3& vCamPos,
										A3DVECTOR3& vLast, 
										A3DVECTOR3& vLastViewUp,
										A3DGFXVERTEX* pVerts)
{
	if (i == 0)
	{
		LnCurvePoint& curPoint = m_lstCurvePoints[i];
		int nIndex = curPoint.nIndex;
		A3DVECTOR3 vIntStart(matTran * m_pIntPos[nIndex]);
		A3DVECTOR3 vDir(RotateVec(GetParentDir(), m_pIntDir[nIndex]));
		vLast = vIntStart;

		A3DVECTOR3 vViewUp(CrossProduct(vDir, m_bUseNormal ? vNormal : vIntStart - vCamPos));
		vViewUp.Normalize();
		vLastViewUp = vViewUp;
		vViewUp *= fWidthStart;

		A3DVECTOR3 vStart1(GetRenderPos1(vViewUp, vIntStart));
		A3DVECTOR3 vStart2(GetRenderPos2(vViewUp, vIntStart));

		SET_VERT(pVerts[nBase], vStart1);
		SET_VERT(pVerts[nBase+1], vStart2);
		fAlpha = m_fAlphaStart;

		if (m_bIsTailFadeout)
			fAlpha *= 1.f - curPoint.nAge / (float)curPoint.nLife;
	}
	else if (i + 1 == nRenderCount)
	{
		LnCurvePoint& curPoint = m_lstCurvePoints[i];
		int nIndex = curPoint.nIndex;
		A3DVECTOR3 vIntEnd(matTran * m_pIntPos[nIndex]);
		A3DVECTOR3 vDir(vIntEnd - vLast);
		A3DVECTOR3 vViewUp(CrossProduct(vDir, m_bUseNormal ? vNormal : vIntEnd - vCamPos));
		vViewUp.Normalize();

		if (DotProduct(vViewUp, vLastViewUp) < 0)
			vViewUp *= -1;

		vViewUp *= fWidthEnd;
		A3DVECTOR3 vEnd1(GetRenderPos1(vViewUp, vIntEnd));
		A3DVECTOR3 vEnd2(GetRenderPos2(vViewUp, vIntEnd));

		SET_VERT(pVerts[nBase], vEnd1);
		SET_VERT(pVerts[nBase+1], vEnd2);
		fAlpha = m_fAlphaEnd;

		if (m_bIsTailFadeout)
			fAlpha *= 1.f - curPoint.nAge / (float)curPoint.nLife;
	}
	else
	{
		LnCurvePoint& curPoint = m_lstCurvePoints[i];
		int nIndex = curPoint.nIndex;
		A3DVECTOR3 vPos(matTran * m_pRandPos[nIndex]);
		A3DVECTOR3 vDir(vPos - vLast);
		vLast = vPos;
	
		A3DVECTOR3 vViewUp(CrossProduct(vDir, m_bUseNormal ? vNormal : vPos - vCamPos));
		vViewUp.Normalize();
		
		if (DotProduct(vViewUp, vLastViewUp) < 0)
			vViewUp *= -1;
		vLastViewUp = vViewUp;

		if (fRatio < .5f)
		{
			fRatio *= 2.0f;
			vViewUp *= fWidthStart * (1.f - fRatio) + fWidthMid * fRatio;
			fAlpha = m_fAlphaStart * (1.f - fRatio) + m_fAlphaMid * fRatio;
		}
		else
		{
			fRatio = (fRatio - .5f) * 2.0f;
			vViewUp *= fWidthMid * (1.f - fRatio) + fWidthEnd * fRatio;
			fAlpha = m_fAlphaMid * (1.f - fRatio) + m_fAlphaEnd * fRatio;
		}

		A3DVECTOR3 v1(GetRenderPos1(vViewUp, vPos));
		A3DVECTOR3 v2(GetRenderPos2(vViewUp, vPos));

		SET_VERT(pVerts[nBase], v1);
		SET_VERT(pVerts[nBase+1], v2);

		if (m_bIsTailFadeout)
			fAlpha *= 1.f - curPoint.nAge / (float)curPoint.nLife;
	}
}

A3DVECTOR3 A3DGFXLightningEx::GetRenderPos1(const A3DVECTOR3& vViewUp, const A3DVECTOR3& vPos) const
{
	const int impossibleRenderSide = 0;
	switch (m_nRenderSide)
	{
	case A3DGFXLightningEx::RST_MID:
		return vPos - vViewUp;
	case A3DGFXLightningEx::RST_UP:
		return vPos;
	case A3DGFXLightningEx::RST_DOWN:
		return vPos - 2 * vViewUp;
	default:
		ASSERT(impossibleRenderSide);
	}
	return vPos;
}

A3DVECTOR3 A3DGFXLightningEx::GetRenderPos2(const A3DVECTOR3& vViewUp, const A3DVECTOR3& vPos) const
{
	const int impossibleRenderSide = 0;
	switch (m_nRenderSide)
	{
	case A3DGFXLightningEx::RST_MID:
		return vPos + vViewUp;
	case A3DGFXLightningEx::RST_UP:
		return vPos + 2 * vViewUp;
	case A3DGFXLightningEx::RST_DOWN:
		return vPos;
	default:
		ASSERT(impossibleRenderSide);
	}
	return vPos;
}
