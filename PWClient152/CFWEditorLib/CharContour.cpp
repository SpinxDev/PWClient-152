// CharContour.cpp: implementation of the CCharContour class.
//
//////////////////////////////////////////////////////////////////////

#include "CharContour.h"
#include "CharCurve.h"
#include "CharVerticesBuffer.h"
#include "CodeTemplate.h"
#include "FWAlgorithm.h"
#include "FWLog.h"
#include "Global.h"
#include <AList2.h>


using namespace FWAlgorithm;

#define new A_DEBUG_NEW


CCharContour::CCharContour()
{
	m_bClockwise = false;
	m_pCharVerticesBuffer = new CCharVerticesBuffer;
}

CCharContour::~CCharContour()
{
	Destroy();
	SAFE_DELETE(m_pCharVerticesBuffer);
}	

void CCharContour::Destroy()
{
	m_pCharVerticesBuffer->Destroy();
}

bool CCharContour::CreateFromFont(TTPOLYGONHEADER *pPolyHeader, BYTE *&pPos)
{

	double dStartPoint[2];
	dStartPoint[0] = FIXEDToDouble(pPolyHeader->pfxStart.x);
	dStartPoint[1] = FIXEDToDouble(pPolyHeader->pfxStart.y);
	
	bool bCreated = false;
	do {
		APtrList<CCharCurve *> lstCharCurves;
		
		// the start point, add it as curve
		CCharCurve *pStartCurve = new CCharCurve;
		pStartCurve->GetCharVerticesBuffer()->Create(1);
		pStartCurve->GetCharVerticesBuffer()->SetVerticesAt(0, dStartPoint);
		lstCharCurves.AddHead(pStartCurve);

		// build body
		if (!(bCreated = BuildCurveListFromFont(pPolyHeader, pPos, &lstCharCurves)))
			break;

		// convert to array
		if (!(bCreated = BuildVerticesBufferFromCurveList(&lstCharCurves)))
			break;

		ClearCurveList(&lstCharCurves, false);

		HideDuplicatedVertex();

		ComputeClockwise();

		bCreated = true;
		
	} while(false);

	if (!bCreated)
		Destroy();

	return bCreated;
}

void CCharContour::ClearCurveList(APtrList<CCharCurve *> *pListCurve, bool bDeleteList)
{
	ASSERT(pListCurve);

	CCharCurve *pCurve = NULL;
	ALISTPOSITION pos = pListCurve->GetHeadPosition();
	while (pos)
	{
		pCurve = pListCurve->GetNext(pos);
		SAFE_DELETE(pCurve);
	}

	if (bDeleteList)
		SAFE_DELETE(pListCurve)
}

bool CCharContour::BuildCurveListFromFont(TTPOLYGONHEADER *pPolyHeader, BYTE *&pPos, APtrList<CCharCurve *> *pListCurve)
{
	ASSERT(pPolyHeader);
	ASSERT(pPos);
	ASSERT(pListCurve);
	ASSERT(pListCurve->GetCount() == 1);

	double *pStartPoint = pListCurve->GetHead()->GetCharVerticesBuffer()->m_paVertices[0];

	TTPOLYCURVE	*pCurveHeader = NULL;
	// for each curves in the contour
	while (pPos - (BYTE *)pPolyHeader < (int)pPolyHeader->cb)
	{
		pCurveHeader = (TTPOLYCURVE *)(pPos);
		pPos = ((BYTE *)(pCurveHeader + 1)) - sizeof(POINTFX);

		// create curve
		CCharCurve *pCurve = new CCharCurve();
		if (!pCurve->CreateFromFont(pStartPoint, pCurveHeader))
		{
			SAFE_DELETE(pCurve);
			ClearCurveList(pListCurve, false);
			return false;
		}
		pListCurve->AddTail(pCurve);

		pPos += sizeof(POINTFX) * pCurveHeader->cpfx;

		CCharVerticesBuffer* pTemp = pListCurve->GetTail()->GetCharVerticesBuffer();
		pStartPoint = pTemp->m_paVertices[pTemp->m_nVerticesCount - 1];
	}

	return true;
}

bool CCharContour::BuildVerticesBufferFromCurveList(APtrList<CCharCurve *> *pListCurve)
{
	ASSERT(pListCurve);

	APtrList<CCharVerticesBuffer *> lstVerticesBuffer;
	ConvertFromCurveListToVerticesBufferList(&lstVerticesBuffer, pListCurve);

	return m_pCharVerticesBuffer->Create(&lstVerticesBuffer);
}

void CCharContour::ConvertFromCurveListToVerticesBufferList(APtrList<CCharVerticesBuffer *> *pListVerticesBuffer, APtrList<CCharCurve *> *pListCurve)
{
	ASSERT(pListVerticesBuffer);
	ASSERT(pListCurve);
	ASSERT(pListVerticesBuffer->GetCount() == 0);

	ALISTPOSITION pos = pListCurve->GetHeadPosition();
	while (pos)
	{
		pListVerticesBuffer->AddTail(pListCurve->GetNext(pos)->GetCharVerticesBuffer());
	}
}

void CCharContour::ComputeClockwise()
{
	ASSERT(m_pCharVerticesBuffer);
	
	if (m_pCharVerticesBuffer->GetVerticesCount() < 3)
		return;
	
	int nIndex0 = 0, nIndex1 = 0, nIndex2 = 0;
	int i;
	double dMinX = 1000000;
	for (i = 0; i < m_pCharVerticesBuffer->m_nVerticesCount; i++)
	{
		if (m_pCharVerticesBuffer->m_paVertices[i][0] < dMinX)
		{
			dMinX = m_pCharVerticesBuffer->m_paVertices[i][0];
			nIndex1 = i;
		}
	}
	nIndex0 = nIndex1 - 1;
	nIndex2 = nIndex1 + 1;
	if (nIndex0 < 0)
		nIndex0 = m_pCharVerticesBuffer->m_nVerticesCount - 1;
	if (nIndex2 > m_pCharVerticesBuffer->m_nVerticesCount - 1)
		nIndex2 = 0;

	if (m_pCharVerticesBuffer->m_paVertices[nIndex0][1] == m_pCharVerticesBuffer->m_paVertices[nIndex2][1])
	{
		m_bClockwise = (m_pCharVerticesBuffer->m_paVertices[nIndex0][0] > m_pCharVerticesBuffer->m_paVertices[nIndex2][0]);					
	}
	else
	{
		m_bClockwise = (m_pCharVerticesBuffer->m_paVertices[nIndex0][1] < m_pCharVerticesBuffer->m_paVertices[nIndex2][1]);
	}
}

void CCharContour::HideDuplicatedVertex()
{
	int nCount = m_pCharVerticesBuffer->GetVerticesCount();
	if (nCount <= 0)
		return;
	if (m_pCharVerticesBuffer->m_paVertices[0][0] == m_pCharVerticesBuffer->m_paVertices[nCount - 1][0] &&
		m_pCharVerticesBuffer->m_paVertices[0][1] == m_pCharVerticesBuffer->m_paVertices[nCount - 1][1])
		m_pCharVerticesBuffer->m_nVerticesCount--;
}

void CCharContour::ReversVertices()
{
	m_pCharVerticesBuffer->ReversVertices();
	ComputeClockwise();
}

void CCharContour::Simplify(float fSimplifyFactor, float fMinLenPerCurve)
{
	int nCountOld = m_pCharVerticesBuffer->GetVerticesCount();
	CCharVerticesBuffer *pNewBuffer = new CCharVerticesBuffer;
	pNewBuffer->Create(nCountOld);

	A3DVECTOR3 vStartDir(0.f, 0.f, 0.f);
	A3DVECTOR3 vLastPoint(0.f, 0.f, 0.f);
	bool bInited = false;
	int nIndex = 0;
	for (int i = 0; i <= nCountOld; i++)
	{
		int j = i;
		if (j == nCountOld) j = 0;

		double *pSrcVertices = m_pCharVerticesBuffer->m_paVertices[j];
		if (i != 0 && vLastPoint == A3DVECTOR3((float)pSrcVertices[0], (float)pSrcVertices[1], 0.f))
			continue;
		if (i == 0)
		{
			pNewBuffer->m_paVertices[0][0] = pSrcVertices[0];
			pNewBuffer->m_paVertices[0][1] = pSrcVertices[1];
			pNewBuffer->m_paVertices[0][2] = 0;
			vLastPoint.x = (float)pSrcVertices[0];
			vLastPoint.y = (float)pSrcVertices[1];
			vLastPoint.z = 0.f;
			nIndex++;
		}
		else if (!bInited)
		{
			vLastPoint.x = (float)pSrcVertices[0];
			vLastPoint.y = (float)pSrcVertices[1];
			vLastPoint.z = 0.f;
			vStartDir = vLastPoint - A3DVECTOR3(
				(float)pNewBuffer->m_paVertices[0][0],
				(float)pNewBuffer->m_paVertices[0][1],
				0.f);
			bInited = true;
			
		}
		else
		{
			A3DVECTOR3 vCurrentPoint = A3DVECTOR3((float)pSrcVertices[0], (float)pSrcVertices[1], 0.f);
			A3DVECTOR3 vCurrentDir = vCurrentPoint - vLastPoint;
			if (GetCosAngle(vStartDir, vCurrentDir) > fSimplifyFactor ||
				(vCurrentPoint - vLastPoint).Magnitude() < fMinLenPerCurve)
			{
				vLastPoint = vCurrentPoint;
			}
			else
			{
				pNewBuffer->m_paVertices[nIndex][0] = vLastPoint.x;
				pNewBuffer->m_paVertices[nIndex][1] = vLastPoint.y;
				pNewBuffer->m_paVertices[nIndex][2] = 0;
				vStartDir = vCurrentDir;
				vLastPoint = vCurrentPoint;
				nIndex++;
			}
		}
	}
	pNewBuffer->m_nVerticesCount = nIndex;

	SAFE_DELETE(m_pCharVerticesBuffer);
	m_pCharVerticesBuffer = pNewBuffer;
}


#define dd 1e-6
float CCharContour::GetCosAngle(const A3DVECTOR3& v1, const A3DVECTOR3 &v2)
{
	float fDP = DotProduct(v1, v2);
	float fLen1 = v1.Magnitude();
	float fLen2 = v2.Magnitude();
	if (fLen1 == 0 || fLen1 == 0)
		return 1.f;
	else
		return fDP / (fLen1 * fLen2) ;
}