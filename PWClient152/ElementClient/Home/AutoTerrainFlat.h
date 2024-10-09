/*
 * FILE: AutoTerrainFlat.h
 *
 * DESCRIPTION: Class for automaticlly flat terrain
 *
 * CREATED BY: Jiang Dalong, 2005/08/08
 *
 * HISTORY: 
 *
 * Copyright (c) 2001-2008 Archosaur Studio, All Rights Reserved.
 */

#ifndef _AUTOTERRAINFLAT_H_
#define _AUTOTERRAINFLAT_H_

#include "AutoPartTerrainBase.h"
#include "ClosedArea.h"

class CAutoHome;

class CAutoTerrainFlat : public CAutoPartTerrainBase 
{
public:
	CAutoTerrainFlat(CAutoHome* pAutoHome);
	virtual ~CAutoTerrainFlat();

public:
	bool FlatTerrain();
	void OnLButtonDown(UINT nFlags, APointI point);
	void OnRButtonUp(UINT nFlags, APointI point);
	void OnRButtonDblClk(UINT nFlags, APointI point);
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

	inline CClosedArea* GetArea() { return &m_Area; }

protected:
	CClosedArea			m_Area;			// Polygon hill area;

	POINT				m_ptAreaLeftTop;
	int					m_nAreaWidth;
	int					m_nAreaHeight;
	float				m_fTransformBorder;

protected:
	bool CalArea();
	// Get average height
	float GetAverageHeight();
	// Flat Terrain poly
	bool FlatTerrainPoly(float fHeight);
};

#endif // #ifndef _AUTOTERRAINFLAT_H_
