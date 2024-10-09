/*
 * FILE: AutoForestLine.h
 *
 * DESCRIPTION: Class for automaticlly generating linear forest
 *
 * CREATED BY: Jiang Dalong, 2005/07/15
 *
 * HISTORY: 
 *
 * Copyright (c) 2001-2008 Archosaur Studio, All Rights Reserved.
 */

#ifndef _AUTOFORESTLINE_H
#define _AUTOFORESTLINE_H

#include "AutoPartTerrainBase.h"
#include "AutoSceneFunc.h"
#include <AArray.h>

class CAutoHome;
class CAutoForest;

class CAutoForestLine : public CAutoPartTerrainBase 
{
public:
	CAutoForestLine(CAutoHome* pAutoHome);
	virtual ~CAutoForestLine();

public:
	// Create forest line
	bool CreateForestLine(float fSpace, float fSpaceNoise, float fMaxSlope, 
		int nNumTreeId, DWORD* pTreeIDs, float* pTreeWeights, DWORD dwSeed);
	void AddPoint(POINT_FLOAT pt);
	void RemoveLastPoint();
	void RemoveAllPoints();
	bool Render();
	void Release();
	// Points valid to generate road
	bool IsPointsValid();
	// Set points
	bool SetPoints(int nNumPoints, POINT_FLOAT* pPoints);
	// Get points
	POINT_FLOAT* GetFinalPoints();
	// Store points
	bool StorePoints();
	// Restore points
	bool RestorePoints(); 

	inline int GetNumPoints() { return m_aPoints.GetSize(); }

protected:
	CAutoForest*		m_pAutoForest;
	AArray<POINT_FLOAT, POINT_FLOAT>	m_aPoints;
	AArray<POINT_FLOAT, POINT_FLOAT>	m_aResults;
	POINT_FLOAT*		m_pPoints;
	int					m_nNumPoints;
	POINT_FLOAT*		m_pPointsBak;
	int					m_nNumPointsBak;

protected:
	bool SmoothPath();
	bool SmoothCorner(int nIndex, int nDivision);
	// Get next tree position
	bool GetNextTreePos(POINT_FLOAT* ptPos, int* pLineIndex, float* pCurLineLeftDis, int nNumLines, float* pLineDis, float fSpace, float fSpaceNoise);
};

#endif // #ifndef _AUTOFORESTLINE_H
