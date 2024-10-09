/*
 * FILE: AutoHillPoly.h
 *
 * DESCRIPTION: Class for generating polygon hill
 *
 * CREATED BY: Jiang Dalong, 2005/05/05
 *
 * HISTORY: 
 *
 * Copyright (c) 2001-2008 Archosaur Studio, All Rights Reserved.
 */

#ifndef _AUTOHILLPOLY_H_
#define _AUTOHILLPOLY_H_

#include "AutoPartTerrainBase.h"
#include "PolygonMountain.h"

class CAutoHome;

class CAutoHillPoly : public CAutoPartTerrainBase
{
public:
	CAutoHillPoly(CAutoHome* pAutoHome);
	virtual ~CAutoHillPoly();

public:
	void AddPoint(POINT_FLOAT pt);
	void RemoveLastPoint();
	void RemoveAllPoints();
	bool Render();
	void Release();

	// Create polygon hill
	bool CreateHillPoly(float fMaxHeight, float fMainSmooth, float fPartSmooth, DWORD dwSeed, int nNumCurve, float* pCurvePoints, bool bHillUp, float fBorderErodibility);
	// Render hill top
	bool RenderHillTop(float fTextureRatio, float fMainTexture, float fPatchTexture, 
						float fPatchDivision, float fTopScope, float fBridgeDepth, bool bBlurMask = true);

	// Is polygon valid
	bool IsPolygonValid();
	// Set area points
	bool SetPoints(int nNumPoints, POINT_FLOAT* pPoints);
	// Get area points
	POINT_FLOAT* GetPoints() { m_Area.GetFinalPoints(); }
	// Get points number
	int	GetNumPoints() { return m_Area.GetNumFinalPoints(); }
	// Get area points
	bool GetAreaPoints(int* pNumPoints, POINT_FLOAT** pPoints);
	// Close area
	bool CloseArea();
	// Store points
	bool StorePoints();
	// Restore points
	bool RestorePoints(); 

	inline CPolygonMountain* GetArea() { return &m_Area; }

protected:
	CPolygonMountain	m_Area;			// Polygon hill area;

	float*				m_pInArea;
	POINT				m_ptAreaLeftTop;
	int					m_nAreaWidth;
	int					m_nAreaHeight;
	float				m_fTransformBorder;
	float*				m_pDistance;
	float				m_fMaxDis;
	bool*				m_pInPolygon;
	int*				m_pMinDisType;
	float*				m_pNeedDraw;//É½¼¹

protected:
	void ReleaseArea();
	bool CalArea();
	bool TransformArea(float fMainSmooth, float fPartSmooth, DWORD dwSeed, int nNumCurve, float* pCurvePoints, float fBorderErodibility);
	bool BlurArea(float* pData, int nWidth, int nHeight);
	// Apply height to terrain
	bool ApplyHeightToTerrain(float fMaxHeight, bool bHillUp);
	bool AddTopTexture(float fTextureRatio, float fMainTexture, float fPatchTexture, 
						float fPatchDivision, float fTopScope, float fBridgeDepth);
	// Expand points
	void ExpandPoints(int x, int y, float fExpandDis, float* pNeedDraw);
	bool GetMountainBridge();
};

#endif // #ifndef _AUTOHILLPOLY_H_
