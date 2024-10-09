/*
 * FILE: AutoGrassPoly.h
 *
 * DESCRIPTION: Class for automaticlly generating polygon grass
 *
 * CREATED BY: Jiang Dalong, 2005/07/16
 *
 * HISTORY: 
 *
 * Copyright (c) 2001-2008 Archosaur Studio, All Rights Reserved.
 */

#ifndef _AUTOGRASSPOLY_H_
#define _AUTOGRASSPOLY_H_

#include "AutoPartTerrainBase.h"
#include "AutoSceneFunc.h"
#include "ClosedArea.h"

class CAutoHome;
class CAutoGrass;

class CAutoGrassPoly : public CAutoPartTerrainBase 
{
public:
	CAutoGrassPoly(CAutoHome* pAutoHome);
	virtual ~CAutoGrassPoly();

public:
	// Create grass poly
	bool CreateGrassPoly(int nGrassKind, float fDensity, int nNumGrassId, DWORD* pGrassIDs, float* pGrassWeights, DWORD dwSeed);
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
	CAutoGrass*			m_pAutoGrass;
	CClosedArea			m_Area;			// Polygon hill area;

	POINT				m_ptAreaLeftTop;
	int					m_nAreaWidth;
	int					m_nAreaHeight;

protected:
	bool CalArea();
};

#endif // #ifndef _AUTOGRASSPOLY_H_

