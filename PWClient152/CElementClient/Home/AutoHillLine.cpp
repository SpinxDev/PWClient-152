/*
 * FILE: AutoHillLine.cpp
 *
 * DESCRIPTION: Class for generating line hill
 *
 * CREATED BY: Jiang Dalong, 2005/06/28
 *
 * HISTORY: 
 *
 * Copyright (c) 2001-2008 Archosaur Studio, All Rights Reserved.
 */
#pragma warning (disable: 4244)
#pragma warning (disable: 4018)

#include "AutoHillLine.h"
#include "AutoHome.h"
#include "Render.h"
#include "AutoTerrain.h"
#include "AutoSceneFunc.h"
#include "CurvePoint.h"
#include "PerlinNoise2D.h"
#include "AutoTexture.h"
#include <A3DWireCollector.h>
#include <A3DEngine.h>
#include <AAssist.h>

const float HILL_SMOOTH_RADIUS = 20.0f;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAutoHillLine::CAutoHillLine(CAutoHome* pAutoHome)
	: CAutoPartTerrainBase(pAutoHome)
{
	m_pPoints = NULL;
	m_pDistance	= NULL;
	m_pPartLineLength = NULL;
	m_pPosWeightOnLine = NULL;
	m_pPoints = NULL;
	m_nNumPoints = 0;
	m_pPointsBak = NULL;
	m_nNumPointsBak = 0;
}

CAutoHillLine::~CAutoHillLine()
{
	Release();
}

void CAutoHillLine::Release()
{
	SAFERELEASE(m_pDistance);
	SAFERELEASE(m_pPoints);
	SAFERELEASE(m_pPointsBak);
	SAFERELEASE(m_pPartLineLength);
	SAFERELEASE(m_pPosWeightOnLine);
}

void CAutoHillLine::AddPoint(POINT_FLOAT pt)
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

void CAutoHillLine::RemoveLastPoint()
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

void CAutoHillLine::RemoveAllPoints()
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

bool CAutoHillLine::Render()
{
	return true;
}

// Points valid to generate road
bool CAutoHillLine::IsPointsValid()
{
	if (m_aPoints.GetSize() < 1)
		return false;

	return true;
}

bool CAutoHillLine::SmoothPath()
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

bool CAutoHillLine::SmoothCorner(int nIndex, int nDivision)
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
	float fSmoothDis = HILL_SMOOTH_RADIUS;
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

bool CAutoHillLine::NoisePath()
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
		a_ClampRoof(fNoiseBenth, 30.0f);
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

// Set points
bool CAutoHillLine::SetPoints(int nNumPoints, POINT_FLOAT* pPoints)
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
POINT_FLOAT* CAutoHillLine::GetPoints()
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

// Create hill line
bool CAutoHillLine::CreateHillLine(float fMaxHeight, float fHillWidth, float fMainSmooth, float fPartSmooth, DWORD dwSeed, int nNumCurve, float* pCurvePoints, int nNumCurveLine, float* pCurveLinePoints, bool bHillUp, float fBorderErodibility)
{
	m_status = ST_END;

	srand(dwSeed);

	// Set curve points
	CCurvePoint cp, cpLine;
	cp.SetPointValues(nNumCurve, pCurvePoints);
	cpLine.SetPointValues(nNumCurveLine, pCurveLinePoints);

	// Create hill region
	SmoothPath();
	NoisePath();
	CalArea(fHillWidth/2);
	// Calculate line length
	CalLineLength();
	
	ExpandWholeHill(fHillWidth/2, &cpLine);


	// Apply height to terrain
	CPerlinNoise2D noise;
	noise.Init(m_nTerrainWidth, m_nTerrainHeight, fMaxHeight, fMainSmooth, fPartSmooth, 4, -1, true);


	if (!ApplyHeightToTerrain(&noise, bHillUp, &cp, fBorderErodibility))
		return false;


	return true;
}

// Expand whole hill by width
bool CAutoHillLine::ExpandWholeHill(float fHillWidth, CCurvePoint* pCurvePoint)
{
	int nAreaSize = m_nTerrainWidth * m_nTerrainHeight;
	SAFERELEASE(m_pDistance);
	m_pDistance = new float[nAreaSize];
	if (NULL == m_pDistance)
		return false;
	memset(m_pDistance, 0, nAreaSize*sizeof(float));

	SAFERELEASE(m_pPosWeightOnLine);
	m_pPosWeightOnLine = new float[nAreaSize];
	if (NULL == m_pPosWeightOnLine)
		return false;
	memset(m_pPosWeightOnLine, 0, nAreaSize*sizeof(float));

	int i, j;
	int nNumLine = m_aResults.GetSize();
	float fCurLength = 0.0f;
	ASSERT(nNumLine>0);

	float* pTotalWeights;
	pTotalWeights = new float[nAreaSize];
	memset(pTotalWeights, 0, nAreaSize*sizeof(float));
	
	if (1 == nNumLine)
		ExpandLine(m_pDistance, m_pPosWeightOnLine, fCurLength, pTotalWeights, m_aResults[0], m_aResults[0], fHillWidth, pCurvePoint);
	else
	{
		for (i=0; i<nNumLine-1; i++)
		{
			ExpandLine(m_pDistance, m_pPosWeightOnLine, fCurLength, pTotalWeights, m_aResults[i], m_aResults[i+1], fHillWidth, pCurvePoint);
			fCurLength += m_pPartLineLength[i];
		}
		int nPos;
		for (i=0; i<m_nTerrainHeight; i++)
		{
			for (j=0; j<m_nTerrainWidth; j++)
			{
				nPos = i*m_nTerrainWidth+j;
				if (m_pDistance[nPos] <= 0)
					continue;
				m_pPosWeightOnLine[nPos] /= pTotalWeights[nPos];
//				ASSERT(m_pPosWeightOnLine[nPos]>=0.0f && m_pPosWeightOnLine[nPos]<=1.0f);
			}
		}
	}
	delete[] pTotalWeights;

	return true;
}

// Expand one part line of hill
bool CAutoHillLine::ExpandLine(float* pDistance, float* pPosWeightOnLine, float fCurLength, float* pWeightsOnLines, POINT_FLOAT pt1, POINT_FLOAT pt2, float fHillWidth, CCurvePoint* pCurvePoint)
{
	int nLeft, nRight, nTop, nBottom;
	nLeft = int(a_Min(pt1.x, pt2.x) - fHillWidth - 1);
	a_ClampFloor(nLeft, 0);
	nRight = int(a_Max(pt1.x, pt2.x) + fHillWidth + 1);
	a_ClampRoof(nRight, m_nTerrainWidth-1);
	nTop = int(a_Min(pt1.y, pt2.y) - fHillWidth - 1);
	a_ClampFloor(nTop, 0);
	nBottom = int(a_Max(pt1.y, pt2.y) + fHillWidth + 1);
	a_ClampRoof(nBottom, m_nTerrainHeight-1);

	int i,j;
	float fDis, fValue;
	int nPos, nType;
	POINT_FLOAT pt, ptCross;
	float fWeightOnLine, fDisLine;
	float fTempWeight;
	fDisLine = ::Distance(pt1, pt2);
	for (i=nTop; i<=nBottom; i++)
	{
		for (j=nLeft; j<=nRight; j++)
		{
			pt.x = j;
			pt.y = i;
			fDis = ::DistanceToPartLine(pt, pt1, pt2, &ptCross, &nType);
			if (fDis < fHillWidth)
			{
				fValue = (fHillWidth - fDis) / fHillWidth;
				nPos = i*m_nTerrainWidth+j;
				if (fValue > pDistance[nPos])
				{
					pDistance[nPos] = fValue;
				}
				fWeightOnLine = ::WeightOnLine(ptCross, pt1, pt2);
				ASSERT(fWeightOnLine>=0.0f && fWeightOnLine<=1.0f);
				if (m_fTotalLineLength > 0.0f)
				{
//					fTempWeight = fValue;
					pWeightsOnLines[nPos] += fValue;
					fTempWeight = (fCurLength + fDisLine * fWeightOnLine) / m_fTotalLineLength;
					pPosWeightOnLine[nPos] += (1 - pCurvePoint->GetValue(fTempWeight)) * fValue;
//					pPosWeightOnLine[nPos] += (fCurLength + fDisLine * fWeightOnLine) / m_fTotalLineLength * fTempWeight;;
				}
				else
					pPosWeightOnLine[nPos] = 0.5f;
			}
		}
	}
	return true;
}

// Apply height to terrain
bool CAutoHillLine::ApplyHeightToTerrain(CPerlinNoise2D* pNoise, bool bHillUp, CCurvePoint* pCurvePoint, float fBorderErodibility)
{
	// Create hill height

	int i, j;
	int nPos;
	float fValue;
	float fHeight;

	CPerlinNoise2D noiseBorder;
	if (!noiseBorder.Init(m_rtArea.Width(), m_rtArea.Height(), fBorderErodibility, 10, 1.0f, 2, -1, true))
		return false;

	for (i=m_rtArea.top; i<m_rtArea.bottom; i++)
	{
		for (j=m_rtArea.left; j<m_rtArea.right; j++)
		{
			nPos = i * m_nTerrainWidth + j;
			if (m_pDistance[nPos] > 0)
			{
				fValue = 1 - pCurvePoint->GetValue(m_pDistance[nPos]);
				if (m_pDistance[nPos] < 0.6)
				{
					fValue = fValue - noiseBorder.GetValue(j-m_rtArea.left, i-m_rtArea.top) * (0.6 - m_pDistance[nPos])*0.5f;
					a_ClampFloor(fValue, 0.0f);
					//fValue *= 0.5 - cos(fValue*3.14159/0.6) * 0.5;
				}
				fHeight = fValue * pNoise->GetValue(j, i);
				fValue = m_pPosWeightOnLine[nPos];
				fHeight *= fValue;
				if (bHillUp)
				{
					if (fHeight > m_pAutoTerrain->GetHeightMap()->pAbsHeightUp[nPos])
						m_pAutoTerrain->GetHeightMap()->pAbsHeightUp[nPos] = fHeight;
				}
				else
				{
					if (-fHeight < m_pAutoTerrain->GetHeightMap()->pAbsHeightDown[nPos])
						m_pAutoTerrain->GetHeightMap()->pAbsHeightDown[nPos] = -fHeight;
				}
				m_pAutoTerrain->UpdateAbsHeightAtPos(nPos);
			}
		}
	}

	if (CAutoTerrain::UDP_BOTH != m_pAutoTerrain->GetUpDownPri())
		m_pAutoTerrain->BlurArea(m_pAutoTerrain->GetHeightMap()->pAbsHeight, m_pAutoTerrain->GetHeightMap()->iWidth, m_pAutoTerrain->GetHeightMap()->iHeight);
//	m_pAutoTerrain->ResetHeightData(MAX_HEIGHT);
//	m_pAutoTerrain->UpdateVertexNormals();

	m_pAutoTerrain->SetNeedRefreshHeight(true);

	return true;
}

// Cal area
bool CAutoHillLine::CalArea(float fHillWidth)
{
	m_rtArea.left = m_nTerrainWidth;
	m_rtArea.right = 0;
	m_rtArea.top = m_nTerrainHeight;
	m_rtArea.bottom = 0;
	int i;
	int nCount = m_aResults.GetSize();
	int nValue;
	for (i=0; i<nCount; i++)
	{
		nValue = (int)(m_aResults[i].x - fHillWidth + 0.5f);
		m_rtArea.left = a_Min(m_rtArea.left, nValue);
		nValue = (int)(m_aResults[i].x + fHillWidth + 0.5f);
		m_rtArea.right = a_Max(m_rtArea.right, nValue);
		nValue = (int)(m_aResults[i].y - fHillWidth + 0.5f);
		m_rtArea.top = a_Min(m_rtArea.top, nValue);
		nValue = (int)(m_aResults[i].y + fHillWidth + 0.5f);
		m_rtArea.bottom = a_Max(m_rtArea.bottom, nValue);
	}
	a_Clamp(m_rtArea.left, 0, m_nTerrainWidth);
	a_Clamp(m_rtArea.right, 0, m_nTerrainWidth);
	a_Clamp(m_rtArea.top, 0, m_nTerrainHeight);
	a_Clamp(m_rtArea.bottom, 0, m_nTerrainHeight);

	return true;
}

// Render hill top
bool CAutoHillLine::RenderHillTop(float fTextureRatio, float fMainTexture, float fPatchTexture, 
								  float fPatchDivision, float fTopScope, bool bBlueMask)
{
	CAutoTexture* pTexture = m_pAutoHome->GetAutoTexture();

	// Restore back mask
//	pTexture->RestoreBridgeMask();

	float fNoiseAmp = 1.0f / 15.0f;
	a_Clamp(fTopScope, 0.0f, 1.0f);
	float fHeightTop = (1 - fTopScope);

	CPerlinNoise2D pnHeight;
	pnHeight.Init(m_rtArea.Width(), m_rtArea.Height(), fNoiseAmp, 3, 0.5, 4, -1, false);

	float fHeight, fNoisedHeight;
	int i,j;
	ARectI rcInMask;
	rcInMask.left = int((float)m_rtArea.left * MASK_SIZE / m_nTerrainWidth + 0.5);
	rcInMask.right = int((float)m_rtArea.right * MASK_SIZE / m_nTerrainWidth + 0.5);
	rcInMask.top = int((float)m_rtArea.top * MASK_SIZE / m_nTerrainHeight + 0.5);
	rcInMask.bottom = int((float)m_rtArea.bottom * MASK_SIZE / m_nTerrainHeight + 0.5);

	float fMaskWeight = fTextureRatio;// Ratio of main mask and patch mask
	float fMainSelfWeight = fMainTexture;
	float fPatchSelfWeight = fPatchTexture;
	float fPatchNumber = fPatchDivision;

	CPerlinNoise2D pnPatch;
	pnPatch.Init(m_rtArea.Width(), m_rtArea.Height(), 1.0f, fPatchNumber, 0.8f, 2, -1, false);
	float fPatchHeight = (pnPatch.GetMaxValue() + pnPatch.GetMinValue()) * 0.5f
						+ (pnPatch.GetMaxValue() - pnPatch.GetMinValue()) * 0.5f * fMaskWeight;
	float fTransWeight;
	fTransWeight = (pnPatch.GetMaxValue() - fPatchHeight) / fPatchSelfWeight;

	float fMaskTransWeight;
	fMaskTransWeight = (pnPatch.GetMaxValue() - pnPatch.GetMinValue());


	int nPosX, nPosY;
	int nNoiseX, nNoiseY;
	float fMainValue, fPatchValue;
	float fRenderWeight;
	float fDisWeight;
	float fRectMiddleX = rcInMask.left + (rcInMask.Width()) / 2.0f;
	float fRectMiddleY = rcInMask.top + (rcInMask.Height()) / 2.0f;
	BYTE btColor;
	TEXTURETYPE tMainTexture, tPatchTexture;
	tMainTexture = TT_BRIDGE;
	tPatchTexture = TT_BRIDGE_PATCH;

	float fDeltaX = (float)m_nTerrainWidth / MASK_SIZE;
	float fDeltaY = (float)m_nTerrainHeight / MASK_SIZE;
	float fPosX;
	float fPosY = rcInMask.top * fDeltaY;
	for (i=rcInMask.top; i<rcInMask.bottom; i++)
	{
		nPosY = (int)(fPosY + 0.5f);
		fPosY += fDeltaY;
		if (nPosY<m_rtArea.top || nPosY>=m_rtArea.bottom)
			continue;
		fPosX = rcInMask.left * fDeltaX;
		nPosX = (int)(fPosX + 0.5f);
		for (j=rcInMask.left; j<rcInMask.right; j++)
		{
			nPosX = (int)(fPosX + 0.5f);
			fPosX += fDeltaX;
			if (nPosX<m_rtArea.left || nPosX>=m_rtArea.right)
				continue;
			nNoiseX = nPosX - m_rtArea.left;
			nNoiseY = nPosY - m_rtArea.top;
			// Get height
			fHeight = m_pDistance[nPosY*m_nTerrainWidth+nPosX];
			// Get real height line
			fNoisedHeight = fHeightTop + pnHeight.GetValue(nNoiseX, nNoiseY);

			if (fHeight > fNoisedHeight)
			{
				fDisWeight = 1.0f;
				fRenderWeight = (fHeight - fNoisedHeight) / (1 - fNoisedHeight) * fDisWeight;
				fMainValue = 255 * ((pnPatch.GetValue(nNoiseX, m_rtArea.Height()-nNoiseY-1) - pnPatch.GetMinValue()) / fMaskTransWeight + fMainSelfWeight);
				a_ClampRoof(fMainValue, 255.0f);
				pTexture->GetMaskPoint(tMainTexture, j, i, &btColor);
				if (255*fRenderWeight > btColor)
					pTexture->UpdateMaskPoint(tMainTexture, j, i, BYTE(255*fRenderWeight));
				pTexture->GetMaskPoint(tPatchTexture, j, i, &btColor);
				if ((255-fMainValue)*fRenderWeight > btColor)
					pTexture->UpdateMaskPoint(tPatchTexture, j, i, BYTE((255-fMainValue)*fRenderWeight));
					pTexture->UpdateMaskPoint(tMainTexture, j, i, BYTE(fMainValue*fRenderWeight));
				if (pnPatch.GetValue(nNoiseX, nNoiseY)>fPatchHeight)
				{
					fPatchValue = 255 * (pnPatch.GetValue(nNoiseX, nNoiseY) - fPatchHeight) / fTransWeight * fRenderWeight;
					a_ClampRoof(fPatchValue, 255.0f);
					pTexture->GetMaskPoint(tPatchTexture, j, i, &btColor);
					if (fPatchValue>btColor)
						pTexture->UpdateMaskPoint(tPatchTexture, j, i, BYTE(fPatchValue));
						pTexture->UpdateMaskPoint(tMainTexture, j, i, BYTE(255-fPatchValue));
				}
			}
		}
	}

	if (bBlueMask)
	{
		pTexture->BlurMask(tMainTexture);
		pTexture->BlurMask(tPatchTexture);
	}

//	pTexture->ApplyPatchOnMain(tMainTexture, tPatchTexture);
	
	// Store back mask
//	pTexture->StoreBridgeMask();

	return true;
}

// Calculate line length
bool CAutoHillLine::CalLineLength()
{
	int nNumLine = m_aResults.GetSize();
	if (nNumLine <= 1)
		return false;
	SAFERELEASE(m_pPartLineLength);
	m_pPartLineLength = new float[nNumLine-1];

	int i;
	m_fTotalLineLength = 0.0f;
	for (i=0; i<nNumLine-1; i++)
	{
		m_pPartLineLength[i] = ::Distance(m_aResults[i], m_aResults[i+1]);
		m_fTotalLineLength += m_pPartLineLength[i];
	}

	return true;
}

// Store points
bool CAutoHillLine::StorePoints()
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
bool CAutoHillLine::RestorePoints()
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