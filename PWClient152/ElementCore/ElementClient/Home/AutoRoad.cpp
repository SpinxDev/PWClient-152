/*
 * FILE: AutoRoad.cpp
 *
 * DESCRIPTION: Class for generating road
 *
 * CREATED BY: Jiang Dalong, 2005/05/06
 *
 * HISTORY: 
 *
 * Copyright (c) 2001-2008 Archosaur Studio, All Rights Reserved.
 */
#pragma warning (disable: 4244)
#pragma warning (disable: 4018)

#include "AutoRoad.h"
#include "AutoHome.h"
#include "Render.h"
#include <AAssist.h>
#include "AutoTerrain.h"
#include "AutoTexture.h"
#include "PerlinNoise2D.h"
#include <A3DWireCollector.h>
#include <A3DEngine.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const float ROAD_SMOOTH_RADIUS = 20.0f;

CAutoRoad::CAutoRoad(CAutoHome* pAutoHome)
	: CAutoPartTerrainBase(pAutoHome)
{
	m_pPathDataDown = NULL;
	m_pPathDataUp = NULL;
	m_pDistance = NULL;
	m_pCrossPointPos = NULL;
	m_pFlatHeight = NULL;
	m_pRoadHeight = NULL;
	m_pPoints = NULL;
	m_nNumPoints = 0;
	m_pPointsBak = NULL;
	m_nNumPointsBak = 0;
}

CAutoRoad::~CAutoRoad()
{
	Release();
}

void CAutoRoad::Release()
{
	ReleasePathData();
	SAFERELEASE(m_pPoints);
	SAFERELEASE(m_pPointsBak);
}

void CAutoRoad::ReleasePathData()
{
	SAFERELEASE(m_pPathDataDown);
	SAFERELEASE(m_pPathDataUp);
	SAFERELEASE(m_pDistance);
	SAFERELEASE(m_pCrossPointPos);
	SAFERELEASE(m_pFlatHeight);
	SAFERELEASE(m_pRoadHeight);
}

void CAutoRoad::OnLButtonDown(UINT nFlags, APointI point)
{
	POINT_FLOAT pt;
	pt.x = point.x;
	pt.y = point.y;

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

void CAutoRoad::OnRButtonUp(UINT nFlags, APointI point)
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

void CAutoRoad::OnRButtonDblClk(UINT nFlags, APointI point) 
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

bool CAutoRoad::Render()
{
	return true;
}

// Points valid to generate road
bool CAutoRoad::IsPointsValid()
{
	if (m_aPoints.GetSize() < 1)
		return false;

	return true;
}

bool CAutoRoad::SmoothPath()
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

bool CAutoRoad::SmoothCorner(int nIndex, int nDivision)
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
	float fSmoothDis = ROAD_SMOOTH_RADIUS;
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

bool CAutoRoad::NoisePath()
{
	int nNumLine = m_aResults.GetSize();
	if (nNumLine < 2)
		return true;
	int nIndex = 0;
	int nNumAdd;
	POINT_FLOAT pt1, pt2, ptAdd, ptTemp;
	float fDis;
	float fSlope;
	float fNoiseBenth;
	float fNoise;

	while (nIndex < nNumLine-1)
	{
		pt1 = m_aResults[nIndex];
		pt2 = m_aResults[nIndex+1];
		fDis = ::Distance(pt1, pt2);
		fNoiseBenth = fDis / 40.0f;
		nNumAdd = fDis / 30 - 1;
		fSlope = ::LineSlope(pt1, pt2);
		(fabs(fSlope)<0.0001) ? fSlope = 9999.0f : fSlope = -1 / fSlope;
		for (int i=1; i<=nNumAdd; i++)
		{
			ptAdd = ::PointAtLine(pt1, pt2, 30*i);
			fNoise = SRANDOM * fNoiseBenth;
			ptTemp.x = ptAdd.x + 2;
			ptTemp.y = fSlope * (ptTemp.x - ptAdd.x) + ptAdd.y;
			ptAdd = ::PointAtLine(ptAdd, ptTemp, fNoise);
			a_Clamp(ptAdd.x, 0.0f, float(m_nTerrainWidth-1));
			a_Clamp(ptAdd.y, 0.0f, float(m_nTerrainHeight-1));
			m_aResults.InsertAt(nIndex + 1, ptAdd);
			nIndex++;
			nNumLine++;
		}
		nIndex++;
	}

	return true;
}

bool CAutoRoad::ExpandPath(float fPathWidth, DWORD dwSeed)
{
	int nNumLine = m_aResults.GetSize();
	int nAreaSize = m_nTerrainWidth*m_nTerrainHeight;
	if (NULL == m_pPathDataDown)
		m_pPathDataDown = new BYTE[nAreaSize];
	memset(m_pPathDataDown, 0, nAreaSize);
	if (NULL == m_pPathDataUp)
		m_pPathDataUp = new BYTE[nAreaSize];
	memset(m_pPathDataUp, 0, nAreaSize);
	if (NULL == m_pCrossPointPos)
		m_pCrossPointPos = new POINT_FLOAT[nAreaSize];
	if (NULL == m_pDistance)
		m_pDistance = new float[nAreaSize];
	for (int i=0; i<m_nTerrainHeight; i++)
		for (int j=0; j<m_nTerrainWidth; j++)
			m_pDistance[i*m_nTerrainWidth+j] = 99999.0f;
	CPerlinNoise2D noise;
	float fFreq;
	if (fPathWidth < 10)
		fFreq = 200;
	else if (fPathWidth < 20)
		fFreq = 100;
	else
		fFreq = 50;
	noise.Init(m_nTerrainWidth, m_nTerrainHeight, 1.0f, fFreq, 0.8f, 2, dwSeed, false);
	if (nNumLine==1)
		ExpandLine(m_pPathDataDown, m_pPathDataUp, m_pDistance, m_pCrossPointPos, m_aResults[0], m_aResults[0], fPathWidth, &noise);
	else
	{
		for (int i=0; i<nNumLine-1; i++)
		{
			ExpandLine(m_pPathDataDown, m_pPathDataUp, m_pDistance, m_pCrossPointPos, m_aResults[i], m_aResults[i+1], fPathWidth, &noise);
		}
	}

	return true;
}

bool CAutoRoad::ExpandLine(BYTE* pDataDown, BYTE* pDataUp, float* pDistance, POINT_FLOAT* pCrossPoint, 
						   POINT_FLOAT pt1, POINT_FLOAT pt2, float fWidth, CPerlinNoise2D* pNoise)
{
	float fDownPathWidth = 3 * fWidth;
	int nLeft, nRight, nTop, nBottom;
	nLeft = int(a_Min(pt1.x, pt2.x) - fDownPathWidth);
	a_ClampFloor(nLeft, 0);
	nRight = int(a_Max(pt1.x, pt2.x) + fDownPathWidth);
	a_ClampRoof(nRight, m_nTerrainWidth-1);
	nTop = int(a_Min(pt1.y, pt2.y) - fDownPathWidth);
	a_ClampFloor(nTop, 0);
	nBottom = int(a_Max(pt1.y, pt2.y) + fDownPathWidth);
	a_ClampRoof(nBottom, m_nTerrainHeight-1);

	float* pHeightMap = m_pAutoTerrain->GetHeightMap()->pAbsHeight;
	if (!pHeightMap)
		return false;
	POINT_FLOAT pt, ptCross;

	int i,j;
	float fDis;
	int nPos, nType;
	for (i=nTop; i<=nBottom; i++)
	{
		for (j=nLeft; j<=nRight; j++)
		{
			nPos = i*m_nTerrainWidth+j;
			pt.x = j;
			pt.y = i;
			fDis = ::DistanceToPartLine(pt, pt1, pt2, &ptCross, &nType);
			if (fDis<fWidth) 
			{
				float fValueWeight = (fWidth-fDis)/fWidth;
				float fValue = 255 * ((1-0.6f)*fValueWeight+0.6f);
				if (fDis<pDistance[nPos])
				{
					pDataDown[nPos] = (BYTE)fValue;
					pDistance[nPos] = fDis;
					pCrossPoint[nPos].x = ptCross.x;
					pCrossPoint[nPos].y = ptCross.y;
				}

				float fHeightBenth = (pNoise->GetMaxValue() + pNoise->GetMinValue()) * 0.5f
						+ (pNoise->GetMaxValue() - pNoise->GetMinValue()) * 0.5f * 0.0;
				float fNoiseValue = pNoise->GetValue(j, i);
				float fUpWeight;
				if (fNoiseValue > fHeightBenth)
				{
					fUpWeight = 1 - (fNoiseValue-fHeightBenth) / (pNoise->GetMaxValue()-fHeightBenth) - 0.2;
					a_ClampFloor(fUpWeight, 0.0f);
				}

				if (fDis<fWidth*0.3f)
				{
					pDataUp[nPos] = (BYTE)(255*fUpWeight);
				}
				else if (fDis<fWidth*0.7f)
				{
					fValueWeight = (fWidth*0.7-fDis)/(fWidth*(0.7-0.3));
					fValue = 255 * fValueWeight;
					a_ClampRoof(fValue, 255.0f);
					if (fValue > pDataUp[nPos])
						pDataUp[nPos] = (BYTE)(fValue*fUpWeight);
				}
			}
			else if (fDis<fDownPathWidth)
			{
				float fNoise = MRANDOM;
				if (fNoise > 0.6)
				{
					fNoise = (255 * (1-(fDis-fWidth) / (fDownPathWidth-fWidth)));
					if (fDis<pDistance[nPos])
					{
						pDataDown[nPos] = (BYTE)fNoise;
						pDistance[nPos] = fDis;
						pCrossPoint[nPos].x = ptCross.x;
						pCrossPoint[nPos].y = ptCross.y;
					}
				}

				else
				{
					if (fDis<pDistance[nPos])
					{
						pDataDown[nPos] = 0;
						pDistance[nPos] = fDis;
						pCrossPoint[nPos].x = ptCross.x;
						pCrossPoint[nPos].y = ptCross.y;
					}
				}

			}
		}
	}

	return true;
}

bool CAutoRoad::AddPathTexture()
{
	CAutoTexture* pTexture = m_pAutoHome->GetAutoTexture();

	int i,j;
	BYTE btColor, btOldColor;
	int nPosX, nPosY;
	for (i=0; i<MASK_SIZE; i++)
	{
		for (j=0; j<MASK_SIZE; j++)
		{
			nPosX = int((float)j * m_nTerrainWidth / MASK_SIZE + 0.5);
			nPosY = int((float)i * m_nTerrainHeight / MASK_SIZE + 0.5);

			pTexture->GetMaskPoint(TT_ROADBED, j, i, &btOldColor);
			btColor = m_pPathDataDown[nPosY*m_nTerrainWidth+nPosX];
			btColor = a_Max(btColor, btOldColor);
			pTexture->UpdateMaskPoint(TT_ROADBED, j, i, btColor);

			pTexture->GetMaskPoint(TT_ROAD, j, i, &btOldColor);
			btColor = m_pPathDataUp[nPosY*m_nTerrainWidth+nPosX];
			btColor = a_Max(btColor, btOldColor);
			pTexture->UpdateMaskPoint(TT_ROAD, j, i, btColor);
		}
	}
	pTexture->BlurMask(TT_ROADBED);
	pTexture->BlurMask(TT_ROAD);

//	pTexture->ApplyPatchOnMain(TT_ROADBED, TT_ROAD);
	return true;
}

// Create road
bool CAutoRoad::CreateRoad(float fRoadWidth, float fMaxHeight, float fMainSmooth, float fPartSmooth, DWORD dwSeed, bool bFlat)
{
	m_status = ST_END;
	srand(dwSeed);

	// Create road path
	SmoothPath();
	NoisePath();

	ReleasePathData();
	// Expand road
	ExpandPath(fRoadWidth/2, -1);
	// Add road texture
	AddPathTexture();

	// Flat road
	if (bFlat)
		FlatRoad(fRoadWidth/2);
	// Down the height of road
	DownRoadHeight(fRoadWidth*1.5, fMaxHeight/2, fMainSmooth, fPartSmooth);

	CPerlinNoise2D noise;
	if (!noise.Init(m_nTerrainWidth, m_nTerrainHeight, 0.2f, fMainSmooth, fPartSmooth, 2, -1, false))
		return false;
	// Apply height to terrain
	ApplyHeightToTerrain(&noise, bFlat);

	return true;
}

// Set points
bool CAutoRoad::SetPoints(int nNumPoints, POINT_FLOAT* pPoints)
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
POINT_FLOAT* CAutoRoad::GetPoints()
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

// Down road height
bool CAutoRoad::DownRoadHeight(float fWidth, float fMaxHeight, float fMainSmooth, float fPartSmooth)
{

	int i, j;
	int nPos;
//	float fMaxHeight = fWidth / 5;
	float fValue;
	if (NULL == m_pRoadHeight)
		m_pRoadHeight = new float[m_nTerrainWidth*m_nTerrainHeight];
	memset(m_pRoadHeight, 0, m_nTerrainWidth*m_nTerrainHeight*sizeof(float));
	for (i=0; i<m_nTerrainHeight; i++)
	{
		for (j=0; j<m_nTerrainWidth; j++)
		{
			nPos = i * m_nTerrainWidth + j;
			if (m_pDistance[nPos] > 99990.0f)
				continue;
			fValue = cos(m_pDistance[nPos] / fWidth * 3.1415927) * 0.5 + 0.5;
			m_pRoadHeight[nPos] = fValue * fMaxHeight;
		}
	}
	return true;
}

bool CAutoRoad::ApplyHeightToTerrain(CPerlinNoise2D* pNoise, bool bFlat)
{
	float fRoadHeight;
	int i, j;
	int nPosTerrain;
	for (i=0; i<m_nTerrainHeight; i++)
	{
		for (j=0; j<m_nTerrainWidth; j++)
		{
			nPosTerrain = i * m_nTerrainWidth + j;
			if (m_pDistance[nPosTerrain] > 99990.0f)
				continue;
			fRoadHeight = m_pRoadHeight[nPosTerrain];
			if (bFlat)
				fRoadHeight += m_pFlatHeight[nPosTerrain];
			fRoadHeight *= (1 + pNoise->GetValue(j, i));
			m_pAutoTerrain->GetHeightMap()->pAbsHeightRoad[nPosTerrain] += -fRoadHeight;
			m_pAutoTerrain->UpdateAbsHeightAtPos(nPosTerrain);
		}
	}

	if (CAutoTerrain::UDP_BOTH != m_pAutoTerrain->GetUpDownPri())
		m_pAutoTerrain->BlurArea(m_pAutoTerrain->GetHeightMap()->pAbsHeight, m_pAutoTerrain->GetHeightMap()->iWidth, m_pAutoTerrain->GetHeightMap()->iHeight);
//	m_pAutoTerrain->ResetHeightData(MAX_HEIGHT);
//	m_pAutoTerrain->UpdateVertexNormals();

	m_pAutoTerrain->SetNeedRefreshHeight(true);

	return true;	
}

// Flat road
bool CAutoRoad::FlatRoad(float fFlatWidth)
{
	int i;
	float* pTerrainHeight = m_pAutoTerrain->GetHeightMap()->pAbsHeight;
	int nSize = m_nTerrainWidth*m_nTerrainHeight;
	if (NULL == m_pFlatHeight)
		m_pFlatHeight = new float[nSize];
	memset(m_pFlatHeight, 0, nSize*sizeof(float));
	// Copy terrain height
	float* pTempTerrainHeight = new float[nSize];
	memcpy(pTempTerrainHeight, pTerrainHeight, nSize*sizeof(float));

	float* pFlatWeights = new float[nSize];
	memset(pFlatWeights, 0, nSize*sizeof(float));

	int nNumLine = m_aResults.GetSize();
	if (nNumLine==1)
		FlatLine(m_aResults[0], m_aResults[0], pTempTerrainHeight, fFlatWidth, pFlatWeights);
	else
	{
		for (i=0; i<nNumLine-1; i++)
		{
			FlatLine(m_aResults[i], m_aResults[i+1], pTempTerrainHeight, fFlatWidth, pFlatWeights);
		}
	}

	delete[] pTempTerrainHeight;
	delete[] pFlatWeights;
	return true;
}

// Flat a part line of road
bool CAutoRoad::FlatLine(POINT_FLOAT& pt1, POINT_FLOAT& pt2, float* pTerrainHeight, float fFlatWidth, float* pFlatWeights)
{
	// Calculate posible flat area
	float fMaxFlatWidth = 2 * fFlatWidth;
	int nLeft, nRight, nTop, nBottom;
	nLeft = int(a_Min(pt1.x, pt2.x) - fMaxFlatWidth);
	a_ClampFloor(nLeft, 0);
	nRight = int(a_Max(pt1.x, pt2.x) + fMaxFlatWidth);
	a_ClampRoof(nRight, m_nTerrainWidth-1);
	nTop = int(a_Min(pt1.y, pt2.y) - fMaxFlatWidth);
	a_ClampFloor(nTop, 0);
	nBottom = int(a_Max(pt1.y, pt2.y) + fMaxFlatWidth);
	a_ClampRoof(nBottom, m_nTerrainHeight-1);

	int i, j;
	float fDis;
	int nPos, nType;
	POINT_FLOAT pt, ptCross;
	float fWeightX, fWeightY, fCrossHeight, fWeight;
	int nCrossLeft, nCrossRight, nCrossTop, nCrossBottom;
	float fFlatHeight;
	for (i=nTop; i<=nBottom; i++)
	{
		for (j=nLeft; j<=nRight; j++)
		{
			nPos = i*m_nTerrainWidth+j;
			pt.x = j;
			pt.y = i;
			fDis = ::DistanceToPartLine(pt, pt1, pt2, &ptCross, &nType);
/*			if (nType == 2)
				continue;
			if (nType == 1 && pFlated[nPos])
				continue;
			if (nType == 0 && pFlated[nPos])
				continue;
*/			if (fDis >= fMaxFlatWidth)
				continue;
			// Get interpolate height value at cross point
			nCrossLeft = (int)ptCross.x;
			nCrossRight = nCrossLeft + 1;
			a_ClampRoof(nCrossRight, m_nTerrainWidth);
			nCrossTop = (int)ptCross.y;
			nCrossBottom = nCrossTop + 1;
			a_ClampRoof(nCrossBottom, m_nTerrainHeight);
			fWeightX = ptCross.x - nCrossLeft;
			fWeightY = ptCross.y - nCrossTop;
			fCrossHeight = (pTerrainHeight[nCrossTop*m_nTerrainWidth+nCrossLeft]*(1-fWeightX)
						+ pTerrainHeight[nCrossTop*m_nTerrainWidth+nCrossRight]*fWeightX) * (1-fWeightY)
						+ (pTerrainHeight[nCrossBottom*m_nTerrainWidth+nCrossLeft]*(1-fWeightX)
						+ pTerrainHeight[nCrossBottom*m_nTerrainWidth+nCrossRight]*fWeightX) * fWeightY;

			if (fDis < fFlatWidth)
			{
				fFlatHeight = pTerrainHeight[nPos] - fCrossHeight;
				//m_pFlatHeight[nPos] =  pTerrainHeight[nPos] - fCrossHeight;
			}
			else if (fDis < fMaxFlatWidth)
			{
				fCrossHeight = pTerrainHeight[nPos] - fCrossHeight;
				fWeight = (fDis - fFlatWidth) / fFlatWidth;
				//m_pFlatHeight[nPos] = fCrossHeight * (cos(fWeight*3.1415927) * 0.5 + 0.5);
				fFlatHeight = fCrossHeight * (cos(fWeight*3.1415927) * 0.5 + 0.5);
			}
//			m_pFlatHeight[nPos] = fFlatHeight;
//			pTerrainHeight[nPos] -= m_pFlatHeight[nPos];
//			pFlated[nPos] = true;
			if (fDis < 0.001f)
				fWeight = 99999.0f;
			else
				fWeight = (fMaxFlatWidth - fDis) / fDis;
			fFlatHeight = fFlatHeight * fWeight + m_pFlatHeight[nPos] * pFlatWeights[nPos];
			pFlatWeights[nPos] += fWeight;
			m_pFlatHeight[nPos] = fFlatHeight / pFlatWeights[nPos];
		
		}
	}

	return true;
}

// Store points
bool CAutoRoad::StorePoints()
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
bool CAutoRoad::RestorePoints()
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