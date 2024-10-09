/*
 * FILE: AutoForestLine.cpp
 *
 * DESCRIPTION: Class for automaticlly generating linear forest
 *
 * CREATED BY: Jiang Dalong, 2005/07/15
 *
 * HISTORY: 
 *
 * Copyright (c) 2001-2008 Archosaur Studio, All Rights Reserved.
 */

#pragma warning (disable: 4244)
#pragma warning (disable: 4018)

#include "AutoForestLine.h"
#include "AutoHome.h"
#include <A3DWireCollector.h>
#include "Render.h"
#include <A3DEngine.h>
#include "AutoWater.h"
#include "AutoTerrain.h"
#include "PerlinNoise2D.h"
#include "AutoForest.h" 

const float TREE_SMOOTH_RADIUS = 20.0f;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAutoForestLine::CAutoForestLine(CAutoHome* pAutoHome)
	: CAutoPartTerrainBase(pAutoHome)
{
	m_pAutoForest = pAutoHome->GetAutoForest();
	m_pPoints = NULL;
	m_nNumPoints = 0;
	m_pPointsBak = NULL;
	m_nNumPointsBak = 0;
}

CAutoForestLine::~CAutoForestLine()
{
	Release();
}

void CAutoForestLine::Release()
{
	SAFERELEASE(m_pPoints);
	SAFERELEASE(m_pPointsBak);
}

void CAutoForestLine::AddPoint(POINT_FLOAT pt)
{
	switch(m_status)
	{
	case ST_NOTSTART:
		m_aPoints.Add(pt);
		m_status = ST_START;
		break;
	case ST_START:
		m_aPoints.Add(pt);
		break;
	case ST_END:
		break;
	}
}

void CAutoForestLine::RemoveLastPoint()
{
	switch(m_status)
	{
	case ST_NOTSTART:
		break;
	case ST_START:
		if (m_aPoints.GetSize()>0)
			m_aPoints.RemoveAt(m_aPoints.GetSize()-1);
		if (m_aPoints.GetSize()==0)
			m_status = ST_NOTSTART;
		break;
	case ST_END:
		m_status = ST_START;
		if (m_aPoints.GetSize()>0)
			m_aPoints.RemoveAt(m_aPoints.GetSize()-1);
		if (m_aPoints.GetSize()==0)
			m_status = ST_NOTSTART;
		break;
	}
}

void CAutoForestLine::RemoveAllPoints() 
{
	// TODO: Add your message handler code here and/or call default
	switch(m_status)
	{
	case ST_NOTSTART:
		break;
	case ST_START:
		m_status = ST_NOTSTART;
		break;
	case ST_END:
		m_status = ST_NOTSTART;
		break;
	}

	m_aPoints.RemoveAll();
}

bool CAutoForestLine::Render()
{
	return true;
}

// Points valid to generate road
bool CAutoForestLine::IsPointsValid()
{
	if (m_aPoints.GetSize() < 1)
		return false;

	return true;
}

bool CAutoForestLine::SmoothPath()
{
	int nNumPoints = m_aPoints.GetSize();
	if (nNumPoints==0)
		return true;

	int i;
	m_aResults.RemoveAll();
	m_aResults.Add(m_aPoints[0]);
	for (i=1; i<nNumPoints-1; i++)
	{
		SmoothCorner(i, 4);
	}
	if (nNumPoints>1)
		m_aResults.Add(m_aPoints[nNumPoints-1]);

	return true;
}

bool CAutoForestLine::SmoothCorner(int nIndex, int nDivision)
{
	float fSlopeLine1, fSlopeLine2;
	fSlopeLine1 = ::LineSlope(m_aPoints[nIndex], m_aPoints[nIndex-1]);
	fSlopeLine2 = ::LineSlope(m_aPoints[nIndex], m_aPoints[nIndex+1]);
	if (fabs(fSlopeLine1-fSlopeLine2)<0.01)// Two lines are parallel
	{
		m_aResults.Add(m_aPoints[nIndex]);
		return true;
	}

	// Get smooth distance from corner
	float fSmoothDis = TREE_SMOOTH_RADIUS;
	float fHalfLineLength = ::Distance(m_aPoints[nIndex-1], m_aPoints[nIndex]) / 2.0f;
	if (fSmoothDis > fHalfLineLength)
		fSmoothDis = fHalfLineLength;
	fHalfLineLength = ::Distance(m_aPoints[nIndex], m_aPoints[nIndex+1]) / 2.0f;
	if (fSmoothDis > fHalfLineLength)
		fSmoothDis = fHalfLineLength;

	// Get smooth start point and end point
	POINT_FLOAT pt1, pt2;
	pt1 = ::PointAtLine(m_aPoints[nIndex], m_aPoints[nIndex-1], fSmoothDis);
	pt2 = ::PointAtLine(m_aPoints[nIndex], m_aPoints[nIndex+1], fSmoothDis);

	// Get center of circle for smooth
	(fabs(fSlopeLine1)<0.0001) ? fSlopeLine1 = 9999.0f : fSlopeLine1 = -1 / fSlopeLine1;
	(fabs(fSlopeLine2)<0.0001) ? fSlopeLine2 = 9999.0f : fSlopeLine2 = -1 / fSlopeLine2;

	float fCenterX = (fSlopeLine1*pt1.x-fSlopeLine2*pt2.x-pt1.y+pt2.y)/(fSlopeLine1-fSlopeLine2);
	float fCenterY = fSlopeLine1*(fCenterX-pt1.x)+pt1.y;
	POINT_FLOAT ptCenter;
	ptCenter.x = fCenterX;
	ptCenter.y = fCenterY;
	float fRadius = ::Distance(pt1, ptCenter);

	// Get smooth points
	int i;
	float fDeltaX, fDeltaY;
	fDeltaX = (float)(pt2.x-pt1.x) / nDivision;
	fDeltaY = (float)(pt2.y-pt1.y) / nDivision;

	POINT_FLOAT ptTemp, ptSmooth;
	ptTemp = pt1;
	m_aResults.Add(pt1);
	for (i=0; i<nDivision-1; i++)
	{
		ptTemp.x = int(ptTemp.x + fDeltaX + 0.5);
		ptTemp.y = int(ptTemp.y + fDeltaY + 0.5);
		ptSmooth = ::PointAtLine(ptCenter, ptTemp, fRadius);
		m_aResults.Add(ptSmooth);
	}
	m_aResults.Add(pt2);

	return true;
}

// Set points
bool CAutoForestLine::SetPoints(int nNumPoints, POINT_FLOAT* pPoints)
{
	m_aPoints.RemoveAll();

	if (nNumPoints < 0)
		return false;

	for (int i=0; i<nNumPoints; i++)
	{
		m_aPoints.Add(pPoints[i]);
	}

	return true;

}

// Get points
POINT_FLOAT* CAutoForestLine::GetFinalPoints()
{
	SAFERELEASE(m_pPoints);
	m_nNumPoints = m_aPoints.GetSize();
	m_pPoints = new POINT_FLOAT[m_nNumPoints];

	if (NULL != m_pPoints)
	{
		for (int i=0; i<m_nNumPoints; i++)
		{
			m_pPoints[i] = m_aPoints[i];
		}
	}

	return m_pPoints;
}

// Create forest line
bool CAutoForestLine::CreateForestLine(float fSpace, float fSpaceNoise, float fMaxSlope, 
	int nNumTreeId, DWORD* pTreeIDs, float* pTreeWeights, DWORD dwSeed)
{
	m_status = ST_END;
//	SmoothPath();

	int i;
	float fTotalWeights = 0.0f;

	// Add tree types
	for (i=0; i<nNumTreeId; i++)
	{
		m_pAutoForest->AddTreeType(pTreeIDs[i]);
		fTotalWeights += pTreeWeights[i];
	}
	// Normalize weights
	for (i=0; i<nNumTreeId; i++)
	{
		pTreeWeights[i] /= fTotalWeights;
	}

	float* pLineDis = NULL;
	int nNumLines = m_aPoints.GetSize() - 1;
	if (nNumLines > 0)
	{
		pLineDis = new float[nNumLines];
		for (i=0; i<nNumLines; i++)
		{
			pLineDis[i] = ::Distance(m_aPoints[i], m_aPoints[i+1]);
		}
	}
	
	POINT_FLOAT point;
	float fMaxSlopeCos = (float)cos(fMaxSlope * 0.017453);
	float fSlopeCos;
	A3DVECTOR3 vNormal;
	float fPlantedDis = 0.0f;
	float fRealSpace = 0.0f;
	int nCurLineIndex = -1;
	float fCurLeftDis = 0.0f;

	srand(dwSeed);
	
	while (GetNextTreePos(&point, &nCurLineIndex, &fCurLeftDis, nNumLines, pLineDis, fSpace, fSpaceNoise))
	{
		// If already be water or sand, no tree
		if (m_pAutoHome->GetAutoWater()->CheckWater(point.x, point.y) > WT_NO_WATER)
			continue;

		// If slope < max slope, then may be tree
		vNormal = m_pAutoTerrain->GetHeightMap()->aNormals[int(point.y*m_nTerrainWidth+point.x)];
		vNormal.Normalize();
		fSlopeCos = DotProduct(vNormal, A3DVECTOR3(0, 1, 0));
		if (fSlopeCos < fMaxSlopeCos)
			continue;

		// Plant a tree
		m_pAutoForest->PlantTree(point.x, point.y, nNumTreeId, pTreeIDs, pTreeWeights);
	}
	
	SAFERELEASE(pLineDis);

	return true;
}

// Get next tree position
bool CAutoForestLine::GetNextTreePos(POINT_FLOAT* ptPos, int* pLineIndex, float* pCurLineLeftDis, int nNumLines, float* pLineDis, float fSpace, float fSpaceNoise)
{
	// First tree pos
	if (*pLineIndex == -1)
	{
		*pLineIndex = 0;
		ptPos->x = m_aPoints[0].x;
		ptPos->y = m_aPoints[0].y;
		if (nNumLines == 0)
			*pCurLineLeftDis = 0.0f;
		else
			*pCurLineLeftDis = pLineDis[0];
		return true;
	}

	// Not first tree pos
	float fRealSpace = fSpace + SRANDOM * fSpaceNoise;
	ASSERT(fRealSpace > 0);

	while (*pLineIndex < nNumLines)
	{
		if (fRealSpace >= *pCurLineLeftDis)
		{
			fRealSpace -= *pCurLineLeftDis;
			(*pLineIndex)++;
			if (*pLineIndex < nNumLines)
				*pCurLineLeftDis = pLineDis[*pLineIndex];
		}
		else
		{
			*pCurLineLeftDis -= fRealSpace;
			float fWeight = *pCurLineLeftDis / pLineDis[*pLineIndex];
			ptPos->x = m_aPoints[*pLineIndex].x * (1-fWeight) + m_aPoints[*pLineIndex+1].x * fWeight;
			ptPos->y = m_aPoints[*pLineIndex].y * (1-fWeight) + m_aPoints[*pLineIndex+1].y * fWeight;
			return true;
		}
	}

	return false;
}

// Store points
bool CAutoForestLine::StorePoints()
{
	m_nNumPointsBak = m_nNumPoints;
	SAFERELEASE(m_pPointsBak);

	m_pPointsBak = new POINT_FLOAT[m_nNumPointsBak];
	if (NULL == m_pPointsBak)
		return false;

	memcpy(m_pPointsBak, m_pPoints, sizeof(POINT_FLOAT)*m_nNumPointsBak);

	return true;
}

// Restore points
bool CAutoForestLine::RestorePoints()
{
	m_nNumPoints = m_nNumPointsBak;
	SAFERELEASE(m_pPoints);

	m_pPoints = new POINT_FLOAT[m_nNumPoints];
	if (NULL == m_pPoints)
		return false;

	memcpy(m_pPoints, m_pPointsBak, sizeof(POINT_FLOAT)*m_nNumPoints);

	int i;
	m_aPoints.RemoveAll();
	for (i=0; i<m_nNumPoints; i++)
	{
		m_aPoints.Add(m_pPoints[i]);
	}

	return true;
}