/*
 * FILE: PolygonMountain.cpp
 *
 * DESCRIPTION: Class for creating mountain in a polygon shape
 *
 * CREATED BY: Jiang Dalong, 2005/04/06
 *
 * HISTORY: 
 *
 * Copyright (c) 2001-2008 Archosaur Studio, All Rights Reserved.
 */

#include <math.h>
#include "PolygonMountain.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPolygonMountain::CPolygonMountain()
{
	m_pSalient = NULL;
	m_pSalientPointsMidLine = NULL;
	m_pMaxDisLineMapping = NULL;
}

CPolygonMountain::~CPolygonMountain()
{
	Release();
}

// Release memory
void CPolygonMountain::Release()
{
	ReleaseSalient();
	CClosedArea::Release();
}

// Release salient
void CPolygonMountain::ReleaseSalient()
{
	if (m_pSalient)
	{
		delete[] m_pSalient;
		m_pSalient = NULL;
	}
	if (m_pSalientPointsMidLine)
	{
		delete[] m_pSalientPointsMidLine;
		m_pSalientPointsMidLine = NULL;
	}
	if (m_pMaxDisLineMapping)
	{
		delete[] m_pMaxDisLineMapping;
		m_pMaxDisLineMapping = NULL;
	}
}

// Min distance to area edge
float CPolygonMountain::DistanceToEdge(POINT_FLOAT& point, bool bInPolygon, int* pMinDisType)
{
	float fDis;
	float fMinDis = 999999.0f;

	*pMinDisType = 0;


	bool bLastValid, bValid;
	int nMinDisLine1, nMinDisLine2;
	int nNumLine = m_nNumPoints;
	int nDisType;
//	float fCosAngle;

	POINT_FLOAT ptCross;
	for (int i=0; i<nNumLine; i++)
	{
		fDis = ::DistanceToPartLine(point, m_pPoints[i], m_pPoints[(i+1)%nNumLine], &ptCross, &nDisType);

		if (nDisType == 0)// 与线段中间距离最近
			bValid = true;
		else// 与线段顶点距离最近
			bValid = false;

		if (fabs(fDis - fMinDis) < 1.0f)
		{
			// 两个距离都不是角连线，且距离大于1个像素（消除误差）
			if (bLastValid && bValid && fDis > 1.5f)
			{
				nMinDisLine2 = i;
				if (bInPolygon)
					*pMinDisType = 1;
				else
					*pMinDisType = 5;
			}
			else
			{
				if (bInPolygon)
					*pMinDisType = 4;
				else
					*pMinDisType = 6;
			}
		}
		else if (fDis < fMinDis)
		{
			fMinDis = fDis;
			*pMinDisType = 0;
			bLastValid = bValid;
			nMinDisLine1 = i;
		}
	}

	if ((nMinDisLine1 == 0) && (nMinDisLine2 == nNumLine - 1))
	{
		nMinDisLine1 = nNumLine - 1;
		nMinDisLine2 = 0;
	}

	if (*pMinDisType == 1)
	{
		// 消除角度过大和中间部分
		if (nMinDisLine2 == (nMinDisLine1 + 1)%nNumLine)
		{
			if (!m_pSalient[nMinDisLine2])
			{
				*pMinDisType = 2;
			}
/*			fCosAngle = CosByLawOfCosine(m_pPoints[nMinDisLine2], m_pPoints[nMinDisLine1], m_pPoints[(nMinDisLine2+1)%m_nNumPoints]);
			if (fCosAngle < -0.3)
			{
				*pMinDisType = 2;
			}
*/
		}
		else
		{
			*pMinDisType = 3;
		}
	}

	int nSalientIndex;
	// Record line 
	if (*pMinDisType == 1)
	{
		nSalientIndex = m_pMaxDisLineMapping[nMinDisLine2];
		m_pSalientPointsMidLine[nSalientIndex].point[m_pSalientPointsMidLine[nSalientIndex].nCount] = point;
		m_pSalientPointsMidLine[nSalientIndex].nCount++;
	}

	
	return fMinDis;
}

// Find salient angle in the polygon
int CPolygonMountain::FindSalient(float fMinCosValue)
{
	ReleaseSalient();
	m_pSalient = new bool[m_nNumPoints];
//	memset(m_pSalient, 0, m_nNumPoints * sizeof(bool));

	m_pMaxDisLineMapping = new int[m_nNumPoints];

//	if (m_nNumPoints < 4)
//		return 0;

	AArray<int, int> aConcaveLines;
	AArray<int, int> aConcave;
	int i, j;
	bool bUpZero = false;
	bool bDownZero = false;
	bool* pSalientLines = new bool[m_nNumPoints];

	// Get lines that all points distribute at same side.
	for (i=0; i<m_nNumPoints; i++)
	{
		bUpZero = false;
		bDownZero = false;
		for (j = i + 2; j < i + m_nNumPoints; j++)
		{
			if (AtLineSide(m_pPoints[j%m_nNumPoints], m_pPoints[i], m_pPoints[(i+1)%m_nNumPoints]) > 0)
			{
				bUpZero = true;
			}
			else
			{
				bDownZero = true;
			}
			if (bUpZero && bDownZero)
			{
//				aConcaveLines.Add(i);
				pSalientLines[i] = false;
				continue;
			}
		}
		pSalientLines[i] = true;
	}

	float fCosAngle;
	int nSalientPointsCount = 0;
	for (i=0; i<m_nNumPoints; i++)
	{
		// Is Salient point?
		if (pSalientLines[i] || pSalientLines[(i+m_nNumPoints-1)%m_nNumPoints])
		{
			// Get cosine value of the angle
			fCosAngle = CosByLawOfCosine(
				m_pPoints[i], 
				m_pPoints[(i+m_nNumPoints-1)%m_nNumPoints], 
				m_pPoints[(i+1)%m_nNumPoints]);
			// Is Cosine of angle more than threshold value?
			if (fCosAngle > fMinCosValue)
			{
				m_pSalient[i] = true;
				m_pMaxDisLineMapping[i] = nSalientPointsCount;
				nSalientPointsCount++;
				continue;
			}
		}
		m_pSalient[i] = false;
		m_pMaxDisLineMapping[i] = -1;
	}

	m_pSalientPointsMidLine = new PARTMAXDISINFO[nSalientPointsCount];
	for (i=0; i<nSalientPointsCount; i++)
	{
		m_pSalientPointsMidLine[i].nCount = 0;
	}
	m_nSalientPoints = nSalientPointsCount;

	delete[] pSalientLines;

/*
	// Get the concave points
	int nNum = aConcaveLines.GetSize();
	for (i=0; i<nNum; i++)
	{
		if ((aConcaveLines[i]+1)%m_nNumPoints == aConcaveLines[(i+1)%nNum]%m_nNumPoints)
		{
			aConcave.Add((aConcaveLines[i]+1)%m_nNumPoints);
		}
	}

	nNum = aConcave.GetSize();
	if (nNum > 0)
	{
		for (i=0; i<nNum; i++)
		{
			m_pConcave[aConcave[i]] = true;
		}
	}
*/
	return nSalientPointsCount;
}