/*
 * FILE: ClosedArea.h
 *
 * DESCRIPTION: Class for a closed area
 *
 * CREATED BY: Jiang Dalong, 2005/03/25
 *
 * HISTORY: 
 *
 * Copyright (c) 2001-2008 Archosaur Studio, All Rights Reserved.
 */

#ifndef _CLOSEDAREA_H_
#define _CLOSEDAREA_H_

#include <AArray.h>
#include "AutoSceneFunc.h"


class CClosedArea  
{
public:
	CClosedArea();
	virtual ~CClosedArea();

public:
	// Release memory
	virtual void Release();

	// Add a point
	void AddPoint(POINT_FLOAT& point);
	// Get a point
	POINT_FLOAT& GetPoint(int nIndex);
	// Finish area
	bool AreaFinish();
	// Is a new point make the lines cross?
	bool IsNextPointCrossLine(POINT_FLOAT& point, bool bFromFirstPoint);
	// Clear area
	void Clear();
	// Remove last point
	void RemoveLastPoint();
	// Is a point in area?
	bool IsInArea(POINT_FLOAT& point);
	// 判断射线是否与边相交
	bool IsCrossLine(POINT_FLOAT pt, int nIndex);
	// Distance to a point
	float DistanceToPoint(POINT_FLOAT pt);
	// Is polygon valid
	bool IsPolygonValid();

	// Set points
	bool SetPoints(int nCount, POINT_FLOAT* pPoints);
	// Store points
	bool StorePoints();
	// Restore points
	bool RestorePoints(); 

	inline bool IsAreaFinished() { return m_bAreaFinished; }
	inline void SetAreaFinished(bool bFinished) { m_bAreaFinished = bFinished; }
	inline int GetNumPoints() { return m_aPoints.GetSize(); }
	inline AArray<POINT_FLOAT, POINT_FLOAT&>* GetPoints() { return &m_aPoints; }
	inline int GetNumFinalPoints() { return m_nNumPoints; }
	inline POINT_FLOAT* GetFinalPoints() { return m_pPoints; }

protected:
	AArray<POINT_FLOAT, POINT_FLOAT&>		m_aPoints;		// Temp points on the area
	POINT_FLOAT*			m_pPoints;			// Points on the complete area
	int						m_nNumPoints;		// Total point count
	POINT_FLOAT*			m_pPointsBak;		// Backuped points
	int						m_nNumPointsBak;	// Backuped total point count
	
	bool					m_bAreaFinished;	// Is Area finish?

protected:
	// Release points
	void ReleasePoints();
	// Release backuped points
	void ReleasePointsBak();
};

#endif // #ifndef _CLOSEDAREA_H_
