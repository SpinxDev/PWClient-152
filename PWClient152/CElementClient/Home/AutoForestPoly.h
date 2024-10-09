/*
 * FILE: AutoForestPoly.h
 *
 * DESCRIPTION: Class for automaticlly generating polygon forest
 *
 * CREATED BY: Jiang Dalong, 2005/07/12
 *
 * HISTORY: 
 *
 * Copyright (c) 2001-2008 Archosaur Studio, All Rights Reserved.
 */

#ifndef _AUTOFORESTPOLY_H
#define _AUTOFORESTPOLY_H

#include "AutoPartTerrainBase.h"
#include "AutoSceneFunc.h"
#include "ClosedArea.h"

class CAutoHome;
class CAutoForest;

class CAutoForestPoly : public CAutoPartTerrainBase
{
public:
	CAutoForestPoly(CAutoHome* pAutoHome);
	virtual ~CAutoForestPoly();

public:
	// Create forest poly
	bool CreateForestPoly(int nPlantType, float fDensityX, float fDensityY, float fMaxSlope, 
		int nNumTreeId, DWORD* pTreeIDs, float* pTreeWeights, DWORD dwSeed);
	void AddPoint(POINT_FLOAT pt);
	void RemoveLastPoint();
	void RemoveAllPoints();
	bool Render();
	void Release();
	// Is polygon valid
	bool IsPolygonValid();
	// Set area points
	bool SetPoints(int nNumPoints, POINT_FLOAT* pPoints);
	// Get area points
	bool GetAreaPoints(int* pNumPoints, POINT_FLOAT** pPoints);
	// Get area points
	POINT_FLOAT* GetPoints() { m_Area.GetFinalPoints(); }
	// Get points number
	int	GetNumPoints() { return m_Area.GetNumFinalPoints(); }
	// Close area
	bool CloseArea();
	// Store points
	bool StorePoints();
	// Restore points
	bool RestorePoints(); 

	inline CClosedArea* GetArea() { return &m_Area; }

protected:
	CAutoForest*		m_pAutoForest;
	CClosedArea			m_Area;			// Polygon hill area;

	POINT				m_ptAreaLeftTop;
	int					m_nAreaWidth;
	int					m_nAreaHeight;

protected:
	bool CalArea();
};

#endif // #ifndef _AUTOFORESTPOLY_H
