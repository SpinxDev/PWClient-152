/*
 * FILE: AutoRoad.h
 *
 * DESCRIPTION: Class for generating road
 *
 * CREATED BY: Jiang Dalong, 2005/05/06
 *
 * HISTORY: 
 *
 * Copyright (c) 2001-2008 Archosaur Studio, All Rights Reserved.
 */

#ifndef _AUTOROAD_H_
#define _AUTOROAD_H_

#include "AutoPartTerrainBase.h"
#include "AutoSceneFunc.h"
#include <AArray.h>

class CAutoHome;
class CPerlinNoise2D;

class CAutoRoad : public CAutoPartTerrainBase  
{
public:
	CAutoRoad(CAutoHome* pAutoHome);
	virtual ~CAutoRoad();

public:
	void OnLButtonDown(UINT nFlags, APointI point);
	void OnRButtonUp(UINT nFlags, APointI point);
	void OnRButtonDblClk(UINT nFlags, APointI point);
	bool Render();
	void Release();
	// Points valid to generate road
	bool IsPointsValid();
	// Create road
	bool CreateRoad(float fRoadWidth, float fMaxHeight, float fMainSmooth, float fPartSmooth, DWORD dwSeed, bool bFlat);
	// Set points
	bool SetPoints(int nNumPoints, POINT_FLOAT* pPoints);
	// Get points
	POINT_FLOAT* GetPoints();
	// Store points
	bool StorePoints();
	// Restore points
	bool RestorePoints(); 

	inline int GetNumPoints() { return m_aPoints.GetSize(); }

protected:
	AArray<POINT_FLOAT, POINT_FLOAT>	m_aPoints;
	AArray<POINT_FLOAT, POINT_FLOAT>	m_aResults;
	BYTE*				m_pPathDataDown;
	BYTE*				m_pPathDataUp;
	POINT_FLOAT*		m_pPoints;
	int					m_nNumPoints;
	POINT_FLOAT*		m_pPointsBak;
	int					m_nNumPointsBak;
	float*				m_pDistance;
	POINT_FLOAT*		m_pCrossPointPos;
	float*				m_pFlatHeight;
	float*				m_pRoadHeight;

protected:
	bool SmoothPath();
	bool NoisePath();
	void ReleasePathData();
	bool SmoothCorner(int nIndex, int nDivision);
	bool ExpandPath(float fPathWidth, DWORD dwSeed);
	bool ExpandLine(BYTE* pDataDown, BYTE* pDataUp, float* pDistance, POINT_FLOAT* pCrossPoint, 
		POINT_FLOAT pt1, POINT_FLOAT pt2, float fWidth, CPerlinNoise2D* pNoise);
	bool AddPathTexture();
	// Down road height
	bool DownRoadHeight(float fWidth, float fMaxHeight, float fMainSmooth, float fPartSmooth);
	bool ApplyHeightToTerrain(CPerlinNoise2D* pNoise, bool bFlat);
	// Flat road
	bool FlatRoad(float fFlatWidth);
	// Flat a part line of road
	bool FlatLine(POINT_FLOAT& pt1, POINT_FLOAT& pt2, float* pTerrainHeight, float fFlatWidth, float* pFlatWeights);
};

#endif // #ifndef _AUTOROAD_H_
