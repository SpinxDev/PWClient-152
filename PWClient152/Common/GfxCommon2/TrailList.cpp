/*
* FILE: TrailList.cpp
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan, 2010/04/23
*
* HISTORY: 
*
* Copyright (c) 2009 Archosaur Studio, All Rights Reserved.
*/

#include "StdAfx.h"
#include "TrailList.h"
#include <A3DHermiteSpline.h>

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Reference to External variables and functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local Types and Variables and Global variables
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Implement ITrailList
//	
///////////////////////////////////////////////////////////////////////////

TRAIL_DATA& TRAIL_DATA::operator = (const TRAIL_DATA& src)
{
	if (&src == this)
		return *this;
	
	m_vSeg1		= src.m_vSeg1;
	m_vSeg2		= src.m_vSeg2;
	m_nSegLife	= src.m_nSegLife;
	m_Color		= src.m_Color;
	return *this;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement ITrailList
//	
///////////////////////////////////////////////////////////////////////////


ITrailList* ITrailList::CreateTrailList(bool bIsSplineMode)
{
	if (bIsSplineMode)
        return new SplineTrailList();
		//return new DirectionalSplineTrailList();
		//return new RadianInterpList();
	else
		return new LineTrailList();
}


void ITrailList::DestroyTrailList(ITrailList* pTrailImpl)
{
	if (!pTrailImpl)
		return;
	
	delete pTrailImpl;
}



//////////////////////////////////////////////////////////////////////////
//
//	Implement SplineTrailList
//
//////////////////////////////////////////////////////////////////////////

SplineTrailList::SplineTrailList()
{
	m_iSampleFreq = 10;
	m_pTrailSpline[0] = new A3DHermiteSpline;
	m_pTrailSpline[1] = new A3DHermiteSpline;
	m_nStartIdx = 0;
}

SplineTrailList::~SplineTrailList()
{
	delete m_pTrailSpline[0];
	delete m_pTrailSpline[1];
	m_pTrailSpline[0] = m_pTrailSpline[1] = NULL;
}

int SplineTrailList::GetCount() const
{
	return GetKPNum();
}

void SplineTrailList::AddKeyData(const TRAIL_DATA& keyData)
{
	m_aKPData.push_back(KPData(keyData.m_nSegLife, keyData.m_Color));
	m_pTrailSpline[0]->AddPoint(keyData.m_vSeg1);
	m_pTrailSpline[1]->AddPoint(keyData.m_vSeg2);
    //m_aKp.push_back(keyData.m_vSeg1);
    //m_aKp.push_back(keyData.m_vSeg2);
}

void SplineTrailList::UpdateRecentKeyData(const TRAIL_DATA& keyData)
{
	int nIdx = static_cast<int>(m_aKPData.size() - 1);
	if (nIdx < 0)
	{
		AddKeyData(keyData);
		return;
	}

	KPData& kpData = m_aKPData[nIdx];
	kpData.m_nSegLife = keyData.m_nSegLife;
	m_pTrailSpline[0]->UpdatePoint(nIdx, keyData.m_vSeg1);
	m_pTrailSpline[1]->UpdatePoint(nIdx, keyData.m_vSeg2);
}

int SplineTrailList::GetRenderCount() const
{
	return m_aRenderData.size();
	//return GetKPNum() * DETAIL_POINTFACTOR;
}

void SplineTrailList::PrepareRenderData()
{
	m_aRenderData.clear();

	const int iKPNum = static_cast<int>(GetKPNum());
	for (int i = 0; i < iKPNum - 1; ++i)
	{
		GenerateRenderData(i, i+1);
	}


}

void SplineTrailList::GenerateRenderData(int i, int j)
{
	int iSegLife0 = GetKPLife(i);
	int iSegLife1 = GetKPLife(j);

	int iLifeDelta = abs(iSegLife1 - iSegLife0);
	int iInterpSegs = (iLifeDelta / m_iSampleFreq) + 1;

	for (int iSeg = 0; iSeg < iInterpSegs; ++iSeg)
	{
		float t = iSeg / (float)iInterpSegs;

		TRAIL_DATA td;
		td.m_vSeg1 = InterpPos(m_pTrailSpline[0], i, t);
		td.m_vSeg2 = InterpPos(m_pTrailSpline[1], i, t);
		td.m_nSegLife = InterpSegLife(i, t);
		td.m_Color = InterpColor(i, t);

		m_aRenderData.push_back(td);
	}	
}

void SplineTrailList::GetRenderTrailData(int iRenderIdx, TRAIL_DATA& td) const
{
	td = m_aRenderData[iRenderIdx];
	//int iKPIdx = iRenderIdx / DETAIL_POINTFACTOR;
	//float t = (iRenderIdx % DETAIL_POINTFACTOR) * DETAIL_INVPOINTFACTOR;
	//td.m_vSeg1 = InterpPos(0, iKPIdx, t);
	//td.m_vSeg2 = InterpPos(1, iKPIdx, t);
	//td.m_nSegLife = InterpSegLife(iKPIdx, t);
	//td.m_Color = InterpColor(iKPIdx, t);


    //if (GetAsyncKeyState(VK_F10) < 0)
    //{
    //    for (int i = 0; i < (int)m_aKp.size(); i += 2)
    //    {
    //        AfxGetA3DDevice()->GetA3DEngine()->GetA3DWireCollector()->Add3DLine(m_aKp[i], m_aKp[i + 1], A3DCOLORRGB(255, 0, 128));
    //        if (i > 0)
    //        {
    //            AfxGetA3DDevice()->GetA3DEngine()->GetA3DWireCollector()->Add3DLine(m_aKp[i], m_aKp[i - 2], A3DCOLORRGB(255, 128, 0));
    //            AfxGetA3DDevice()->GetA3DEngine()->GetA3DWireCollector()->Add3DLine(m_aKp[i + 1], m_aKp[i - 1], A3DCOLORRGB(255, 128, 0));
    //        }
    //    }
    //}

}

void SplineTrailList::Reset()
{
	m_nStartIdx = 0;
	m_pTrailSpline[0]->Clear();
	m_pTrailSpline[1]->Clear();
	m_aKPData.clear();
}

void SplineTrailList::Tick(DWORD dwTickDelta)
{
	for (size_t nIdx = 0; nIdx < m_aKPData.size(); ++nIdx)
	{
		m_aKPData[nIdx].m_nSegLife -= dwTickDelta;

		// 更新 m_nStartIdx, 是第一个仍然活着的关键点的Index
		if (m_aKPData[nIdx].m_nSegLife <= 0)
        {
			m_nStartIdx = nIdx + 1;
            //m_aKp.erase(m_aKp.begin());
            //m_aKp.erase(m_aKp.begin());
        }
	}

	if (m_aKPData.size() > 256 && m_nStartIdx > m_aKPData.size() / 2)
	{
		m_aKPData.erase(m_aKPData.begin(), m_aKPData.begin() + m_nStartIdx);
		m_pTrailSpline[0]->Erase(0, m_nStartIdx);
		m_pTrailSpline[1]->Erase(0, m_nStartIdx);
		m_nStartIdx = 0;
	}
}

size_t SplineTrailList::GetKPNum() const
{
	return m_aKPData.size() - m_nStartIdx;
}

A3DVECTOR3 SplineTrailList::GetKPPosition(int nSpline, size_t nKPIndex) const
{
	unsigned short nMappedIndex = unsigned short(nKPIndex + m_nStartIdx);
	assert(nMappedIndex < m_pTrailSpline[nSpline]->GetNumPoints() && "nKPIndex is out of bound!!");
	return m_pTrailSpline[nSpline]->GetPoint(nMappedIndex);
}

A3DVECTOR3 SplineTrailList::InterpPos(A3DHermiteSpline* pSpline, size_t nKPIndex, FLOAT t) const
{
	unsigned short nMappedIndex = unsigned short(nKPIndex + m_nStartIdx);
	assert(nMappedIndex < pSpline->GetNumPoints() && "nKPIndex is out of bound!!");
	return pSpline->Interpolate(nMappedIndex, t);
}

A3DCOLOR SplineTrailList::InterpColor(size_t nKPIndex, FLOAT t) const
{
	size_t nMappedIndex = nKPIndex + m_nStartIdx;
	assert(nMappedIndex < m_aKPData.size() && "nKPIndex is out of bound!!");
	if (nMappedIndex < m_aKPData.size() - 1)
		return Interp_Color(m_aKPData[nMappedIndex].m_Color, m_aKPData[nMappedIndex + 1].m_Color, t);

	return m_aKPData[nMappedIndex].m_Color;
}

int SplineTrailList::InterpSegLife(size_t nKPIndex, FLOAT t) const 
{
	size_t nMappedIndex = nKPIndex + m_nStartIdx;
	assert(nMappedIndex < m_aKPData.size() && "nKPIndex is out of bound!!");
	if (nMappedIndex < m_aKPData.size() - 1)
		return m_aKPData[nMappedIndex].m_nSegLife + int((m_aKPData[nMappedIndex + 1].m_nSegLife - m_aKPData[nMappedIndex].m_nSegLife) * t);
	
	return m_aKPData[nMappedIndex].m_nSegLife;
}

float SplineTrailList::InterpSegLifeRate(size_t nKPIndex, FLOAT t) const 
{
    size_t nMappedIndex = nKPIndex + m_nStartIdx;
    assert(nMappedIndex < m_aKPData.size() && "nKPIndex is out of bound!!");
    if (nMappedIndex < m_aKPData.size() - 1)
    {
        float fRate1 = GetKPLifeRate(nMappedIndex);
        float fRate2 = GetKPLifeRate(nMappedIndex + 1);
        return fRate1 + (fRate2 - fRate1) * t;
    }

    return GetKPLifeRate(nMappedIndex);
}

float SplineTrailList::GetKPLifeRate(int nKPIdx) const
{
    int nMaxLife = a_Min(500, m_aKPData[nKPIdx].m_nSegMaxLife);
    int nElapsed = m_aKPData[nKPIdx].m_nSegMaxLife - m_aKPData[nKPIdx].m_nSegLife;
    return nElapsed / (float) nMaxLife;
}

//////////////////////////////////////////////////////////////////////////
//
//	LineTrailList
//
//////////////////////////////////////////////////////////////////////////

LineTrailList::LineTrailList()
: m_TrailLst(SEG_SIZE_DEFAULT)
{
}

int LineTrailList::GetCount() const
{
	return m_TrailLst.GetDataCount();
}

void LineTrailList::AddKeyData(const TRAIL_DATA& keyData)
{
	m_TrailLst.AddData(keyData);
}

void LineTrailList::UpdateRecentKeyData(const TRAIL_DATA& keyData)
{
	int nIdx = m_TrailLst.GetDataCount() - 1;
	if (nIdx < 0)
		return;

	m_TrailLst[nIdx].m_nSegLife = keyData.m_nSegLife;
	m_TrailLst[nIdx].m_vSeg1 = keyData.m_vSeg1;
	m_TrailLst[nIdx].m_vSeg2 = keyData.m_vSeg2;
	m_TrailLst[nIdx].m_Color = keyData.m_Color;
}

void LineTrailList::Tick(DWORD dwTickDelta)
{
	int nCount = m_TrailLst.GetDataCount();
	for (int i = nCount-1; i >= 0; i--)
	{
		TRAIL_DATA& td = m_TrailLst[i];
		td.m_nSegLife -= dwTickDelta;

		if (td.m_nSegLife <= 0)
		{
			m_TrailLst.RemoveOldData(i+1);
			break;
		}
	}

}

void LineTrailList::Reset()
{
	m_TrailLst.RemoveAll();
}

int LineTrailList::GetRenderCount() const
{
	return m_TrailLst.GetDataCount();
}

void LineTrailList::GetRenderTrailData(int iRenderIdx, TRAIL_DATA& td) const
{
	td = m_TrailLst[iRenderIdx];
}


///////////////////////////////////////////////////////////////////////////
//	
//	Implement QuaternionInterpList
//	
///////////////////////////////////////////////////////////////////////////


void RadianInterpList::Tick(DWORD dwTickDelta)
{
	LineTrailList::Tick(dwTickDelta);
}

void RadianInterpList::PrepareRenderData()
{
	// generate render data
	m_aRenderData.clear();

	if (GetCount() == 0)
		return;

	for (int i = 0; i < GetCount() - 1; ++i)
	{
		GenerateRenderDataByTrailData(m_TrailLst[i], m_TrailLst[i + 1]);
	}

	m_aRenderData.push_back(m_TrailLst[GetCount() - 1]);
}

//	line segment (a, b) intersects with plane p
//	while return t as the fraction, in [0, 1] means the cross point is in the range between (a, b)
//	return 1 if intersection point is between a and b
//	return 0 if not
int IntersectSegmentPlane(A3DVECTOR3 a, A3DVECTOR3 b, A3DPLANE p, float &t, A3DVECTOR3 &q)
{
	A3DVECTOR3 ab = b - a;
	t = (p.fDist - DotProduct(p.vNormal, a)) / DotProduct(p.vNormal, ab);

	q = a + t * ab;
	if (t >= 0.0f && t <= 1.0f)
		return 1;

	// not between a and b
	return 0;
}

//#define _OLD_IMPL_
#define _USE_SELF_ROTATE
#ifdef _OLD_IMPL_
void RadianInterpList::GenerateRenderDataByTrailData(const TRAIL_DATA& td0, const TRAIL_DATA& td1)
{
#define INTERP_DEBUG 1
	A3DVECTOR3 s0 = td0.m_vSeg1;
	A3DVECTOR3 s1 = td0.m_vSeg2;
	A3DVECTOR3 p0 = td1.m_vSeg1;
	A3DVECTOR3 p1 = td1.m_vSeg2;
	A3DVECTOR3 s01 = s1 - s0;
	A3DVECTOR3 p01 = p1 - p0;
	// 公垂线向量
	A3DVECTOR3 L = CrossProduct(a3d_Normalize(s01), a3d_Normalize(p01));
	// 如果两个seg几乎平行，则CrossProduct的magnitude接近0
	if (L.SquaredMagnitude() < 0.00001f)
	{
		m_aRenderData.push_back(td0);
		m_aRenderData.push_back(td1);
	}
	else
	{
		L = a3d_Normalize(L);

#if INTERP_DEBUG
		AfxGetA3DDevice()->GetA3DEngine()->GetA3DWireCollector()->Add3DLine(s0, s0 + L , A3DCOLORRGB(255, 0, 0));
		AfxGetA3DDevice()->GetA3DEngine()->GetA3DWireCollector()->Add3DLine(p0, p0 + L , A3DCOLORRGB(255, 255, 0));
#endif
		// 公垂线方向位移
		A3DVECTOR3 d = DotProduct((s1 - p1), L) * L;
		A3DVECTOR3 d1 = DotProduct((s0 - p1), L) * L;
		A3DVECTOR3 d2 = DotProduct((s1 - p0), L) * L;
	//	ASSERT( d.x == d1.x && d.y == d1.y && d.z == d1.z );
	//	ASSERT( d2.x == d1.x && d2.y == d1.y && d2.z == d1.z );
		// 公垂线与s0 s1直线交点 (公垂线与p0 p1构成的面与s0 s1交点)
		A3DVECTOR3 vNorm = a3d_Normalize(CrossProduct(a3d_Normalize(p01), L));
		A3DPLANE planeP0P1O(vNorm, DotProduct(vNorm, p1));
		
		// 计算面O P0 P1与S0 S1直线交点
		A3DVECTOR3 O;
		float t;
		IntersectSegmentPlane(s0, s1, planeP0P1O, t, O);

		//////////////////////////////////////////////////////////////////////////
		A3DVECTOR3 OP1 = p1 - O;
		A3DVECTOR3 OP0 = p0 - O;
		A3DVECTOR3 OSn = CrossProduct(a3d_Normalize(OP1), a3d_Normalize(OP0));
		A3DVECTOR3 OS1 = a3d_Normalize(s1 - O);
		float fcheck = fabs(DotProduct(L, OS1));
		//ASSERT(fcheck < 0.0001f );
		if (fcheck > 0.0001f)
		{
			a_LogOutput(1, "Error L dot OS1 is larger than 0.0001 (actual: %f)", fcheck);
		}
		//A3DVECTOR3 normOSn = a3d_Normalize(OSn);
		//float fOSn2OS1 = DotProduct(normOSn, OS1);
		//ASSERT( fabs(fOSn2OS1) >= 0.99999f );
		//////////////////////////////////////////////////////////////////////////

		//AfxGetA3DDevice()->GetA3DEngine()->GetA3DFlatCollector()->AddBox_3D(O, A3DVECTOR3(0, 0, 1), A3DVECTOR3(0, 1, 0), A3DVECTOR3(1, 0, 0), A3DVECTOR3(0.05f), A3DCOLORRGB(0, 0, 255));
#if INTERP_DEBUG
		AfxGetA3DDevice()->GetA3DEngine()->GetA3DWireCollector()->Add3DLine(O, s0, A3DCOLORRGB(0, 0, 255));
		AfxGetA3DDevice()->GetA3DEngine()->GetA3DWireCollector()->Add3DLine(O, s1, A3DCOLORRGB(0, 0, 255));
#endif

		A3DVECTOR3 O1 = O - d;

#if INTERP_DEBUG
		AfxGetA3DDevice()->GetA3DEngine()->GetA3DWireCollector()->Add3DLine(O, O1, A3DCOLORRGB(0, 255, 0));
#endif

		//////////////////////////////////////////////////////////////////////////

		A3DVECTOR3 O1P0 = p0 - O1;
		A3DVECTOR3 O1P1 = p1 - O1;
		float fDotO1P0O1P1 = DotProduct(a3d_Normalize(O1P0), a3d_Normalize(O1P1));
		//ASSERT( fabs(fDotO1P0O1P1) > 0.9999f );
		//////////////////////////////////////////////////////////////////////////

#if INTERP_DEBUG
		AfxGetA3DDevice()->GetA3DEngine()->GetA3DWireCollector()->Add3DLine(O1, p0, A3DCOLORRGB(0, 255, 255));
		AfxGetA3DDevice()->GetA3DEngine()->GetA3DWireCollector()->Add3DLine(O1, p1, A3DCOLORRGB(0, 255, 255));
		AfxGetA3DDevice()->GetA3DEngine()->GetA3DWireCollector()->Add3DLine(p1, a3d_Normalize(p0 - p1) * (O - s1).Magnitude() + p1, A3DCOLORRGB(0, 255, 255));
#endif

		A3DVECTOR3 vP0O1 = O1 - p0;
		float fP0O1 = vP0O1.Magnitude();
		float t1 = vP0O1.Magnitude() / p01.Magnitude();
		if (DotProduct(vP0O1, p01) < 0)
			t1 = -t1;
		
		A3DVECTOR3 dir0 = s1 - O;
		A3DVECTOR3 dir1 = p1 - O1;
		A3DVECTOR3 norm_s01 = a3d_Normalize(s01);
		A3DVECTOR3 norm_p01 = a3d_Normalize(p01);
		float fTheta = acosf(DotProduct(norm_s01, norm_p01));
		//float fTheta = acosf(DotProduct(s01, p01) / (s01.Magnitude() * p01.Magnitude()));
		//if (DotProduct((p0 - O1), (p1 - p0)) * DotProduct((s0 - O), (s1 - s0)) < 0 && fTheta > A3D_PI / 2)
		//	fTheta -= A3D_PI;

		int iStepCount = (td1.m_nSegLife - td0.m_nSegLife) / m_iSampleFreq + 1;

		A3DQUATERNION q0(L, 0);
		A3DQUATERNION q1(L, fTheta);

		float fScale = sqrt(p01.SquaredMagnitude() / s01.SquaredMagnitude());
		float fMovementOnAxis = ((p1 - p0).Magnitude() - (s1 - s0).Magnitude()) / fScale;
		
		float fStepScale = fScale / iStepCount;
		float fStepMovementOnAxis = fMovementOnAxis / iStepCount;

		A3DVECTOR3 stepMovement = -d / iStepCount;
		A3DMATRIX4 matRot = a3d_RotateAxis(O, L, fTheta / iStepCount);// * a3d_Translate(stepMovement.x, stepMovement.y, stepMovement.z);

		A3DVECTOR3 v0_Origin_Delta = p0 - O1 + O - s0 * a3d_RotateAxis(O, L, fTheta);
		A3DVECTOR3 v1_Origin_Delta = p1 - O1 + O - s1 * a3d_RotateAxis(O, L, fTheta);

		//A3DMATRIX3 matRotVec = a3d_Matrix3Rotate(fTheta / iStepCount);

		float os0 = (s0 - O).Magnitude();
		float os1 = (s1 - O).Magnitude();
		float o1p0 = (p0 - O1).Magnitude();
		float o1p1 = (p1 - O1).Magnitude();
		A3DVECTOR3 originStep = 0;//O;//0;
		A3DVECTOR3 dirStep0 = a3d_Normalize(s0 - O);
		A3DVECTOR3 dirStep1 = a3d_Normalize(s1 - O);
		
		//A3DVECTOR3 v0 = td0.m_vSeg1 - O;
		//A3DVECTOR3 v1 = td0.m_vSeg2 - O;
		A3DVECTOR3 v0 = s0;
		A3DVECTOR3 v1 = s1;
		for (int i = 0; i < iStepCount; ++i)
		{
			const float fPortion = i / (float)iStepCount;
			const float fScaleStep0 = os0 * (1 - fPortion) + o1p0 * fPortion;
			const float fScaleStep1 = os1 * (1 - fPortion) + o1p1 * fPortion;
			
			//A3DQUATERNION qCur = SLERPQuad(q0, q1, fPortion);
			//A3DVECTOR3 vCurDir0 = qCur * dirStep0;
			//A3DVECTOR3 vCurDir1 = qCur * dirStep1;

			TRAIL_DATA td;
			td.m_Color = Interp_Color(td0.m_Color, td1.m_Color, fPortion);
			td.m_nSegLife = td0.m_nSegLife * (1 - fPortion) + td1.m_nSegLife * fPortion;
			td.m_vSeg1 = originStep + dirStep0 * fScaleStep0;
			td.m_vSeg2 = originStep + dirStep1 * fScaleStep1;
			//td.m_vSeg1 = originStep + vCurDir0 * fScaleStep0;
			//td.m_vSeg2 = originStep + vCurDir1 * fScaleStep1;
			A3DMATRIX4 matRot = a3d_RotateAxis(O + v0_Origin_Delta * i / iStepCount, L, fTheta / iStepCount);// * a3d_Translate(stepMovement.x, stepMovement.y, stepMovement.z);
			//A3DVECTOR3 v0 = s0 * matRot;
			//A3DVECTOR3 v1 = s1 * matRot;
			td.m_vSeg1 = v0 + originStep;
			td.m_vSeg2 = v1 + originStep;
			m_aRenderData.push_back(td);

			v0 = v0 * matRot;
			v1 = v1 * matRot;
			originStep = originStep + stepMovement;
			A3DVECTOR3 v01 = a3d_Normalize(v1 - v0);
			v0 += v0_Origin_Delta / iStepCount;// + v01 * fMovementOnAxis / iStepCount;
			v1 += v1_Origin_Delta / iStepCount;// + v01 * fMovementOnAxis / iStepCount;

			dirStep0 = dirStep0 * matRot - matRot.GetRow(3);
			//dirStep0 = a3d_RotateVecAroundLine(dirStep0, O, L, fTheta / iStepCount);
			dirStep0.Normalize();
			dirStep1 = dirStep1 * matRot - matRot.GetRow(3);
			//dirStep1 = a3d_RotateVecAroundLine(dirStep1, O, L, fTheta / iStepCount);
			dirStep1.Normalize();
		}

		//m_aRenderData.push_back(td1);
	}
}
#elif defined _USE_SELF_ROTATE
bool g_INTERP_DEBUG = true;
void RadianInterpList::GenerateRenderDataByTrailData(const TRAIL_DATA& td0, const TRAIL_DATA& td1)
{
#define INTERP_DEBUG 1
	A3DVECTOR3 s0 = td0.m_vSeg1; 
	A3DVECTOR3 s1 = td0.m_vSeg2;
	A3DVECTOR3 p0 = td1.m_vSeg1;
	A3DVECTOR3 p1 = td1.m_vSeg2;
	A3DVECTOR3 s01 = s1 - s0;
	A3DVECTOR3 p01 = p1 - p0;
	A3DVECTOR3 smid = (s1 + s0) / 2;
	A3DVECTOR3 pmid = (p1 + p0) / 2;
	// 公垂线向量
	A3DVECTOR3 L = CrossProduct(a3d_Normalize(s01), a3d_Normalize(p01));
	// 如果两个seg几乎平行，则CrossProduct的magnitude接近0
	if (L.SquaredMagnitude() < 0.00001f)
	{
		m_aRenderData.push_back(td0);
		m_aRenderData.push_back(td1);
	}
	else
	{
		L = a3d_Normalize(L);

		// 公垂线方向位移
		A3DVECTOR3 d = DotProduct((s1 - p1), L) * L;
		// 公垂线与s0 s1直线交点 (公垂线与p0 p1构成的面与s0 s1交点)
		A3DVECTOR3 vNorm = a3d_Normalize(CrossProduct(a3d_Normalize(p01), L));
		A3DPLANE planeP0P1O(vNorm, DotProduct(vNorm, p1));

		// 计算面O P0 P1与S0 S1直线交点
		A3DVECTOR3 O;
		float t;
		IntersectSegmentPlane(s0, s1, planeP0P1O, t, O);

if (g_INTERP_DEBUG)
{
		AfxGetA3DDevice()->GetA3DEngine()->GetA3DWireCollector()->Add3DLine(O, s0, A3DCOLORRGB(0, 0, 255));
		AfxGetA3DDevice()->GetA3DEngine()->GetA3DWireCollector()->Add3DLine(O, s1, A3DCOLORRGB(0, 0, 255));
}

		A3DVECTOR3 O1 = O - d;

if (g_INTERP_DEBUG)
{
		AfxGetA3DDevice()->GetA3DEngine()->GetA3DWireCollector()->Add3DLine(O, O1, A3DCOLORRGB(0, 255, 0));
		AfxGetA3DDevice()->GetA3DEngine()->GetA3DWireCollector()->Add3DLine(O1, p0, A3DCOLORRGB(0, 255, 255));
		AfxGetA3DDevice()->GetA3DEngine()->GetA3DWireCollector()->Add3DLine(O1, p1, A3DCOLORRGB(0, 255, 255));
		AfxGetA3DDevice()->GetA3DEngine()->GetA3DWireCollector()->Add3DLine(p1, a3d_Normalize(p0 - p1) * (O - s1).Magnitude() + p1, A3DCOLORRGB(0, 255, 255));
}

		//A3DVECTOR3 norm_s01 = a3d_Normalize(s01);
		//A3DVECTOR3 norm_p01 = a3d_Normalize(p01);
		//float fTheta = acosf(DotProduct(norm_s01, norm_p01));
		int iStepCount = (td1.m_nSegLife - td0.m_nSegLife) / m_iSampleFreq + 1;

		// s中点到O向量
		A3DVECTOR3 norm_smid_o = a3d_Normalize(smid - O);
		// p中点到O1向量
		A3DVECTOR3 norm_pmid_o = a3d_Normalize(pmid - O1);
		// 公转角
		float fBigRotateTheta = acosf(DotProduct(norm_smid_o, norm_pmid_o));
		A3DVECTOR3 vBigRotateAxis = a3d_Normalize(CrossProduct(norm_smid_o, norm_pmid_o));

		// 是否需要自转？
		bool bSelfRotate = DotProduct(CrossProduct(s01, p01), CrossProduct(norm_smid_o, norm_pmid_o)) < 0;
		//float fSelfRotateRadian = bSelfRotate ? A3D_PI : 0;
		float fSelfRotateRadian = 0;
		iStepCount = bSelfRotate ? iStepCount * 10 : iStepCount;

		if (bSelfRotate)
		{
			vBigRotateAxis = a3d_Normalize(CrossProduct(norm_smid_o, -norm_pmid_o));
			fBigRotateTheta = acosf(DotProduct(norm_smid_o, -norm_pmid_o));
		}

		// 公转矩阵
		A3DMATRIX4 matBigRotate = a3d_RotateAxis(O, vBigRotateAxis, fBigRotateTheta);
		// 公转完成后的s01中点位置
		A3DVECTOR3 SMid_Final = smid * matBigRotate;
		// 公转过程中，mid点需要平移这么多距离
		A3DVECTOR3 Final_Delta = pmid - SMid_Final;;// + d;
		//A3DVECTOR3 Step_Delta = (Final_Delta - d) / iStepCount;
		
		A3DVECTOR3 vSMidRotateCenter = O;
		A3DVECTOR3 vCurNormS01 = a3d_Normalize(s01);
		A3DVECTOR3 vCurSMid = smid;
		for (int i = 0; i <= iStepCount; ++i)
		{
			float fPortion = (float)i / iStepCount;
			
			A3DMATRIX4 matBigRotate = a3d_RotateAxis(O, vBigRotateAxis, fPortion * fBigRotateTheta);
			// 公转轴点位置
			vCurSMid = smid * matBigRotate;

if (g_INTERP_DEBUG) 
{
			AfxGetA3DDevice()->GetA3DEngine()->GetA3DWireCollector()->Add3DLine(O - d * fPortion, vCurSMid + Final_Delta * fPortion, A3DCOLORRGB(255, 0, 0));
}
			// 被公转之后的朝向
			A3DVECTOR3 vCurNormS01 = a3d_RotateVecAroundLine(s01, O, vBigRotateAxis, fPortion * fBigRotateTheta);
			vCurNormS01.Normalize();
			//vCurNormS01 = a3d_Normalize(a3d_Normalize(s01) * matBigRotate - matBigRotate.GetRow(3));

			// 当前自转角度
			A3DVECTOR3 vCurActualS01Dir = a3d_RotateVecAroundLine(vCurNormS01, vCurSMid, -vBigRotateAxis, fSelfRotateRadian * fPortion);
			vCurActualS01Dir.Normalize();
			
			// 根据最终方向得出当前s0, s1
			float fCurLength = s01.Magnitude() * (1 - fPortion) + p01.Magnitude() * fPortion;
			A3DVECTOR3 vCurS0 = vCurSMid - 0.5f * vCurActualS01Dir * fCurLength;
			A3DVECTOR3 vCurS1 = vCurSMid + 0.5f * vCurActualS01Dir * fCurLength;

			// 补上当前偏移
			vCurS0 += Final_Delta * fPortion;
			vCurS1 += Final_Delta * fPortion;

			TRAIL_DATA td;
			td.m_Color = Interp_Color(td0.m_Color, td1.m_Color, fPortion);
			td.m_nSegLife = int(td0.m_nSegLife * (1 - fPortion) + td1.m_nSegLife * fPortion);
			td.m_vSeg1 = vCurS0;
			td.m_vSeg2 = vCurS1;
			m_aRenderData.push_back(td);
		}
	}
}
#else
void RadianInterpList::GenerateRenderDataByTrailData(const TRAIL_DATA& td0, const TRAIL_DATA& td1)
{
#define INTERP_DEBUG 1
	A3DVECTOR3 s0 = td0.m_vSeg1; 
	A3DVECTOR3 s1 = td0.m_vSeg2;
	A3DVECTOR3 p0 = td1.m_vSeg1;
	A3DVECTOR3 p1 = td1.m_vSeg2;
	A3DVECTOR3 s01 = s1 - s0;
	A3DVECTOR3 p01 = p1 - p0;
	A3DVECTOR3 smid = (s1 + s0) / 2;
	A3DVECTOR3 pmid = (p1 + p0) / 2;
	// 公垂线向量
	A3DVECTOR3 L = CrossProduct(a3d_Normalize(s01), a3d_Normalize(p01));
	// 如果两个seg几乎平行，则CrossProduct的magnitude接近0
	if (L.SquaredMagnitude() < 0.00001f)
	{
		m_aRenderData.push_back(td0);
		m_aRenderData.push_back(td1);
	}
	else
	{
		L = a3d_Normalize(L);

		// 公垂线方向位移
		A3DVECTOR3 d = DotProduct((s1 - p1), L) * L;
		// 公垂线与s0 s1直线交点 (公垂线与p0 p1构成的面与s0 s1交点)
		A3DVECTOR3 vNorm = a3d_Normalize(CrossProduct(a3d_Normalize(p01), L));
		A3DPLANE planeP0P1O(vNorm, DotProduct(vNorm, p1));

		// 计算面O P0 P1与S0 S1直线交点
		A3DVECTOR3 O;
		float t;
		IntersectSegmentPlane(s0, s1, planeP0P1O, t, O);

#if INTERP_DEBUG
		AfxGetA3DDevice()->GetA3DEngine()->GetA3DWireCollector()->Add3DLine(O, s0, A3DCOLORRGB(0, 0, 255));
		AfxGetA3DDevice()->GetA3DEngine()->GetA3DWireCollector()->Add3DLine(O, s1, A3DCOLORRGB(0, 0, 255));
#endif

		A3DVECTOR3 O1 = O - d;

#if INTERP_DEBUG
		AfxGetA3DDevice()->GetA3DEngine()->GetA3DWireCollector()->Add3DLine(O, O1, A3DCOLORRGB(0, 255, 0));
		AfxGetA3DDevice()->GetA3DEngine()->GetA3DWireCollector()->Add3DLine(O1, p0, A3DCOLORRGB(0, 255, 255));
		AfxGetA3DDevice()->GetA3DEngine()->GetA3DWireCollector()->Add3DLine(O1, p1, A3DCOLORRGB(0, 255, 255));
		AfxGetA3DDevice()->GetA3DEngine()->GetA3DWireCollector()->Add3DLine(p1, a3d_Normalize(p0 - p1) * (O - s1).Magnitude() + p1, A3DCOLORRGB(0, 255, 255));
#endif

		A3DVECTOR3 norm_s01 = a3d_Normalize(s01);
		A3DVECTOR3 norm_p01 = a3d_Normalize(p01);
		float fTheta = acosf(DotProduct(norm_s01, norm_p01));

		int iStepCount = (td1.m_nSegLife - td0.m_nSegLife) / m_iSampleFreq + 1;

		//// s中点到O向量
		A3DVECTOR3 norm_smid_o = a3d_Normalize(smid - O);
		//// p中点到O1向量
		A3DVECTOR3 norm_pmid_o = a3d_Normalize(pmid - O1);
		// 公转角
		float fBigRotateTheta = acosf(DotProduct(norm_s01, norm_p01));
		// 公转矩阵
		A3DMATRIX4 matBigRotate = a3d_RotateAxis(O, L, fBigRotateTheta);

		// 公转完成后的s01中点位置
		A3DVECTOR3 SMid_Final = smid * matBigRotate;
		A3DVECTOR3 S0_Final = s0 * matBigRotate;
		// 公转过程中，mid点需要平移这么多距离
		A3DVECTOR3 Final_Delta = p0 - S0_Final;//-d;//pmid - SMid_Final;;// + d;
		//A3DVECTOR3 Step_Delta = (Final_Delta - d) / iStepCount;

		// 是否需要自转？
		bool bSelfRotate = DotProduct(CrossProduct(s01, p01), CrossProduct(norm_smid_o, norm_pmid_o)) < 0;
		float fSelfRotateRadian = bSelfRotate ? A3D_PI : 0;

		A3DVECTOR3 vSMidRotateCenter = O;
		A3DVECTOR3 vCurNormS01 = a3d_Normalize(s01);
		A3DVECTOR3 vCurSMid = smid;
		for (int i = 0; i < iStepCount; ++i)
		{
			float fPortion = (float)i / iStepCount;

			A3DMATRIX4 matBigRotate = a3d_RotateAxis(O, L, fPortion * fBigRotateTheta);
			// 公转轴点位置
			//A3DVECTOR3 vCurS0 = s0 * matBigRotate;
			A3DVECTOR3 vCurS0 = s0 * (1 - fPortion) + p0 * fPortion;
			// 被公转之后的朝向
			vCurNormS01 = a3d_Normalize(a3d_Normalize(s01) * matBigRotate - matBigRotate.GetRow(3));

			// 当前自转角度
			float fCurSelfRotateRadian = fSelfRotateRadian * fPortion;

			//A3DQUATERNION qSelfRotation(-L, fCurSelfRotateRadian);
			//qSelfRotation.Normalize();
			//// 将当前公转朝向再叠加上自转角度，得到最终的方向
			//A3DVECTOR3 vCurActualS01Dir = qSelfRotation * vCurNormS01;
			//vCurActualS01Dir.Normalize();
			//A3DMATRIX4 matSelfRotate = a3d_RotateAxis(vCurS0, L, fCurSelfRotateRadian);
			//A3DVECTOR3 vCurActualS01Dir = matSelfRotate * vCurNormS01;
			//vCurActualS01Dir.Normalize();
			A3DVECTOR3 vCurActualS01Dir = a3d_RotateVecAroundLine(s01, vCurS0, L, fCurSelfRotateRadian);
			vCurActualS01Dir.Normalize();

			// 根据最终方向得出当前s0, s1
			A3DVECTOR3 vCurS1 = vCurS0 + vCurActualS01Dir * s01.Magnitude();

			// 补上当前偏移
		//	vCurS0 += Final_Delta * fPortion;
		//	vCurS1 += Final_Delta * fPortion;

			TRAIL_DATA td;
			td.m_Color = Interp_Color(td0.m_Color, td1.m_Color, fPortion);
			td.m_nSegLife = td0.m_nSegLife * (1 - fPortion) + td1.m_nSegLife * fPortion;
			td.m_vSeg1 = vCurS0;
			td.m_vSeg2 = vCurS1;
			m_aRenderData.push_back(td);
		}
	} 
}
#endif

int RadianInterpList::GetRenderCount() const
{
	return m_aRenderData.size();
}

void RadianInterpList::GetRenderTrailData(int iRenderIdx, TRAIL_DATA& td) const
{
	td = m_aRenderData[iRenderIdx];
}


///////////////////////////////////////////////////////////////////////////
//	
//	Implement LineTrailGravityList
//	
///////////////////////////////////////////////////////////////////////////

LineTrailGravityList::LineTrailGravityList()
{
	memset(&m_GravityData,0,sizeof(m_GravityData));
	m_bGravity = false;
}

LineTrailGravityList::~LineTrailGravityList()
{

}

void LineTrailGravityList::AddKeyData( const TRAIL_DATA& keyData )
{
	if (m_bGravity)
	{
		float fNoise = m_GravityData.m_fAmplitude * sin(GetTickCount() * m_GravityData.m_fHSpeed * 0.001f);
		float x = m_GravityData.m_bXNoise ? fNoise : 0;
		float z = m_GravityData.m_bZNoise ? fNoise : 0;
		A3DVECTOR3 vOffset = A3DVECTOR3(x,0,z);
		TRAIL_DATA data = keyData;
		data.m_vSeg1 += vOffset;
		data.m_vSeg2 += vOffset;
		m_TrailLst.AddData(data);
	}
	else
	{
		m_TrailLst.AddData(keyData);
	}
}

void LineTrailGravityList::Tick( DWORD dwTickDelta )
{
	if (m_bGravity)
		TickGravity(dwTickDelta);
	else
		LineTrailList::Tick(dwTickDelta);
}

void LineTrailGravityList::SetGravityData( const GRAVITY_DATA& gravityData )
{
	m_GravityData = gravityData;
	if (m_GravityData.m_fVerticalNoise < 0.0f)
		m_GravityData.m_fVerticalNoise = 0.0f;
	if (m_GravityData.m_fVerticalNoise > 1.0f)
		m_GravityData.m_fVerticalNoise = 1.0f;
}

void LineTrailGravityList::UseGravity( bool bUse )
{
	m_bGravity=bUse;
}

void LineTrailGravityList::TickGravity( DWORD dwTickDelta )
{
	int nCount = m_TrailLst.GetDataCount();
	for (int i = nCount-1; i >= 0; i--)
	{
		TRAIL_DATA& td = m_TrailLst[i];
		td.m_nSegLife -= dwTickDelta;

		if (td.m_nSegLife <= 0)
		{
			m_TrailLst.RemoveOldData(i+1);
			break;
		}

		AffectGravity(i, m_GravityData.m_fGravity);
	}
}

void LineTrailGravityList::AffectGravity( int nIdx,float fGravity )
{
	float fNoise = fGravity * (1-m_GravityData.m_fVerticalNoise + m_GravityData.m_fVerticalNoise*abs(sin(GetTickCount() * m_GravityData.m_fVSpeed * 0.001f)));

	A3DVECTOR3 v0 = m_TrailLst[nIdx].m_vSeg1;
	A3DVECTOR3 v1 = m_TrailLst[nIdx].m_vSeg2;

	v0.y -= fNoise;
	v1.y -= fNoise;

	m_TrailLst[nIdx].m_vSeg1 = v0;
	m_TrailLst[nIdx].m_vSeg2 = v1;
}


///////////////////////////////////////////////////////////////////////////
//	
//	Implement DirectionalSplineTrailList
//	
///////////////////////////////////////////////////////////////////////////

DirectionalSplineTrailList::DirectionalSplineTrailList() : SplineTrailList()
{
    m_pTrailSplineRef[0] = new A3DHermiteSpline;
    m_pTrailSplineRef[1] = new A3DHermiteSpline;

//    m_pTrailSpline[0]->SetAutoCalculate(false);
//    m_pTrailSpline[1]->SetAutoCalculate(false);
    m_pTrailSplineRef[0]->SetAutoCalculate(true);
    m_pTrailSplineRef[1]->SetAutoCalculate(true);
}

DirectionalSplineTrailList::~DirectionalSplineTrailList()
{
    delete m_pTrailSplineRef[0];
    delete m_pTrailSplineRef[1];
    m_pTrailSplineRef[0] = NULL;
    m_pTrailSplineRef[1] = NULL;
}

void DirectionalSplineTrailList::AddKeyData(const TRAIL_DATA& keyData)
{
    m_aKPData.push_back(KPData(keyData.m_nSegLife, keyData.m_Color));
    m_aKp.push_back(keyData.m_vSeg1);
    m_aKp.push_back(keyData.m_vSeg2);
    A3DVECTOR3 vTangent1;
    A3DVECTOR3 vTangent2;
    int nNumPoints = m_pTrailSpline[0]->GetNumPoints();
    if (nNumPoints == 0)
    {
        vTangent1.Set(0, 0, 0);
        vTangent2.Set(0, 0, 0);
    }
    else
    {
        if (nNumPoints == 1)
        {
            CalcSegTangents(keyData.m_vSeg1, keyData.m_vSeg2, m_pTrailSpline[0]->GetPoint(0), m_pTrailSpline[1]->GetPoint(0), vTangent1, vTangent2);
            m_pTrailSpline[0]->UpdateTangent(0, -vTangent1);
            m_pTrailSpline[1]->UpdateTangent(0, -vTangent2);
        }
        CalcSegTangents(m_pTrailSpline[0]->GetPoint(nNumPoints - 1), m_pTrailSpline[1]->GetPoint(nNumPoints - 1), 
            keyData.m_vSeg1, keyData.m_vSeg2, vTangent1, vTangent2);
    }
    m_pTrailSpline[0]->AddPointAndTangent(keyData.m_vSeg1, vTangent1);
    m_pTrailSpline[1]->AddPointAndTangent(keyData.m_vSeg2, vTangent2);
    m_pTrailSplineRef[0]->AddPoint(keyData.m_vSeg1);
    m_pTrailSplineRef[1]->AddPoint(keyData.m_vSeg2);
    if (m_aKp.size() / 2 > 256 && m_nStartIdx > m_aKp.size() / 2 / 2)
    {
        m_aKp.erase(m_aKp.begin(), m_aKp.begin() + m_nStartIdx * 2);
    }

}

void DirectionalSplineTrailList::UpdateRecentKeyData(const TRAIL_DATA& keyData)
{

}

float g_fTempTrailMag = 0.7f;
float g_fTempTrailLerp = 0.6f;

bool g_bTempTrailShowSample = false;

void DirectionalSplineTrailList::CalcSegTangents(const A3DVECTOR3& vLast1, const A3DVECTOR3& vLast2, 
                                                 const A3DVECTOR3& vCur1, const A3DVECTOR3& vCur2, 
                                                 A3DVECTOR3& vTangent1, A3DVECTOR3& vTangent2)
{
    A3DVECTOR3 vLastMid = 0.5f * (vLast1 + vLast2);
    A3DVECTOR3 vEdge1 = vLastMid - vCur1;
    A3DVECTOR3 vEdge2 = vLastMid - vCur2;
    A3DVECTOR3 vPlaneNorm = CrossProduct(vEdge1, vEdge2);
    A3DVECTOR3 vCurLine = vCur2 - vCur1;
    A3DVECTOR3 vDir = CrossProduct(vCurLine, vPlaneNorm);
    A3DVECTOR3 vDir1 = vCur1 - vLast1;
    A3DVECTOR3 vDir2 = vCur2 - vLast2;
    vDir.Normalize();
	#define LERP(s, a, b) ((a) * (1.0f - (s)) + (b) * (s))
	vTangent1 = LERP(g_fTempTrailLerp, vDir1.Magnitude() * vDir, vDir1) * g_fTempTrailMag;
	vTangent2 = LERP(g_fTempTrailLerp, vDir2.Magnitude() * vDir, vDir2) * g_fTempTrailMag;

}

void DirectionalSplineTrailList::PrepareRenderData()
{
    SplineTrailList::PrepareRenderData();

    //int nNumPoints = m_pTrailSpline[0]->GetNumPoints();
    //if (nNumPoints < 1)
    //{
    //}
    //else
    //{
    //    A3DVECTOR3 vTangent1;
    //    A3DVECTOR3 vTangent2;

    //    CalcSegTangents(m_pTrailSpline[0]->GetPoint(1), m_pTrailSpline[1]->GetPoint(1), 
    //                    m_pTrailSpline[0]->GetPoint(0), m_pTrailSpline[1]->GetPoint(0), vTangent1, vTangent2);
    //    m_pTrailSpline[0]->UpdateTangent(0, -vTangent1);
    //    m_pTrailSpline[1]->UpdateTangent(0, -vTangent2);
    //    for (int i = 1; i < nNumPoints; i++)
    //    {
    //        CalcSegTangents(m_pTrailSpline[0]->GetPoint(i - 1), m_pTrailSpline[1]->GetPoint(i - 1), 
    //            m_pTrailSpline[0]->GetPoint(i), m_pTrailSpline[1]->GetPoint(i), vTangent1, vTangent2);
    //        m_pTrailSpline[0]->UpdateTangent(i, vTangent1);
    //        m_pTrailSpline[1]->UpdateTangent(i, vTangent2);
    //    }
    //}

    //if (g_bTempTrailShowSample)
    //{
    //    for (int i = 0; i < (int)m_aKp.size(); i += 2)
    //    {
    //        AfxGetA3DDevice()->GetA3DEngine()->GetA3DWireCollector()->Add3DLine(m_aKp[i], m_aKp[i + 1], A3DCOLORRGB(255, 0, 128));
    //        if (i > 0)
    //        {
    //            AfxGetA3DDevice()->GetA3DEngine()->GetA3DWireCollector()->Add3DLine(m_aKp[i], m_aKp[i - 2], A3DCOLORRGB(255, 128, 0));
    //            AfxGetA3DDevice()->GetA3DEngine()->GetA3DWireCollector()->Add3DLine(m_aKp[i + 1], m_aKp[i - 1], A3DCOLORRGB(255, 128, 0));
    //        }
    //    }
    //}
}

void DirectionalSplineTrailList::GenerateRenderData(int i, int j)
{
    int iSegLife0 = GetKPLife(i);
    int iSegLife1 = GetKPLife(j);

    int iLifeDelta = abs(iSegLife1 - iSegLife0);
    int iInterpSegs = (iLifeDelta / m_iSampleFreq) + 1;

    for (int iSeg = 0; iSeg < iInterpSegs; ++iSeg)
    {
        float t = iSeg / (float)iInterpSegs;

        TRAIL_DATA td;
        A3DVECTOR3 vPos1 = InterpPos(m_pTrailSpline[0], i, t);
        A3DVECTOR3 vPos2 = InterpPos(m_pTrailSpline[1], i, t);
        A3DVECTOR3 vPosRef1 = InterpPos(m_pTrailSplineRef[0], i, t);
        A3DVECTOR3 vPosRef2 = InterpPos(m_pTrailSplineRef[1], i, t);
        float fRate = InterpSegLifeRate(i, t);
        fRate = fRate * 2;
        a_Clamp(fRate, 0.0f, 1.0f);
        td.m_vSeg1 = vPos1 + (vPosRef1 - vPos1) * fRate;
        td.m_vSeg2 = vPos2 + (vPosRef2 - vPos2) * fRate;
        td.m_nSegLife = InterpSegLife(i, t);
        td.m_Color = InterpColor(i, t);

        m_aRenderData.push_back(td);
    }	
}

void DirectionalSplineTrailList::Tick(DWORD dwTickDelta)
{
    for (size_t nIdx = 0; nIdx < m_aKPData.size(); ++nIdx)
    {
        m_aKPData[nIdx].m_nSegLife -= dwTickDelta;

        // 更新 m_nStartIdx, 是第一个仍然活着的关键点的Index
        if (m_aKPData[nIdx].m_nSegLife <= 0)
            m_nStartIdx = nIdx + 1;
    }

    if (m_aKPData.size() > 256 && m_nStartIdx > m_aKPData.size() / 2)
    {
        m_aKPData.erase(m_aKPData.begin(), m_aKPData.begin() + m_nStartIdx);
        m_pTrailSpline[0]->Erase(0, m_nStartIdx);
        m_pTrailSpline[1]->Erase(0, m_nStartIdx);
        m_pTrailSplineRef[0]->Erase(0, m_nStartIdx);
        m_pTrailSplineRef[1]->Erase(0, m_nStartIdx);
        m_nStartIdx = 0;
    }
}

void DirectionalSplineTrailList::Reset()
{
    m_nStartIdx = 0;
    m_pTrailSpline[0]->Clear();
    m_pTrailSpline[1]->Clear();
    m_pTrailSplineRef[0]->Clear();
    m_pTrailSplineRef[1]->Clear();
    m_aKPData.clear();
}