/*
 * FILE: AutoHillLine.h
 *
 * DESCRIPTION: Class for generating line hill
 *
 * CREATED BY: Jiang Dalong, 2005/06/28
 *
 * HISTORY: 
 *
 * Copyright (c) 2001-2008 Archosaur Studio, All Rights Reserved.
 */

#ifndef _AUTOHILLLINE_H_
#define _AUTOHILLLINE_H_

#include "AutoPartTerrainBase.h"
#include "AutoSceneFunc.h"
#include <AArray.h>

class CAutoHome;
class CCurvePoint;
class CPerlinNoise2D;

class CAutoHillLine : public CAutoPartTerrainBase   
{
public:
	CAutoHillLine(CAutoHome* pAutoHome);
	virtual ~CAutoHillLine();

	void AddPoint(POINT_FLOAT pt);
	void RemoveLastPoint();
	void RemoveAllPoints();
	bool Render();
	void Release();
	// Points valid to generate hill
	bool IsPointsValid();
	// Set points
	bool SetPoints(int nNumPoints, POINT_FLOAT* pPoints);
	// Get points
	POINT_FLOAT* GetPoints();
	// Store points
	bool StorePoints();
	// Restore points
	bool RestorePoints(); 

	// Create hill line
	bool CreateHillLine(float fMaxHeight, float fHillWidth, float fMainSmooth, float fPartSmooth, DWORD dwSeed, 
			int nNumCurve, float* pCurvePoints, int nNumCurveLine, float* pCurveLinePoints, bool bHillUp, float fBorderErodibility);
	// Render hill top
	bool RenderHillTop(float fTextureRatio, float fMainTexture, float fPatchTexture, 
					float fPatchDivision, float fTopScope, bool bBlueMask = true);

	inline int GetNumPoints() { return m_aPoints.GetSize(); }

protected:
	AArray<POINT_FLOAT, POINT_FLOAT>	m_aPoints;
	AArray<POINT_FLOAT, POINT_FLOAT>	m_aResults;
	POINT_FLOAT*		m_pPoints;
	int					m_nNumPoints;
	ARectI				m_rtArea;
	POINT_FLOAT*		m_pPointsBak;
	int					m_nNumPointsBak;

	float*				m_pDistance;

	float*				m_pPartLineLength;
	float				m_fTotalLineLength;
	float*				m_pPosWeightOnLine;

protected:
	bool SmoothPath();
	bool NoisePath();
	bool SmoothCorner(int nIndex, int nDivision);
	bool ExpandWholeHill(float fHillWidth, CCurvePoint* pCurvePoint);
	bool ExpandLine(float* pDistance, float* pPosWeightOnLine, float fCurLength, float* pWeightsOnLines,
		POINT_FLOAT pt1, POINT_FLOAT pt2, float fHillWidth, CCurvePoint* pCurvePoint);
	// Apply height to terrain
	bool ApplyHeightToTerrain(CPerlinNoise2D* pNoise, bool bHillUp, CCurvePoint* pCurvePoint, float fBorderErodibility);

	bool CalArea(float fHillWidth);
	// Calculate line length
	bool CalLineLength();

};

#endif // #ifndef _AUTOHILLLINE_H_
