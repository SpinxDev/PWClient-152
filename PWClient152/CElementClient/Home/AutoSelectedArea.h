/*
 * FILE: AutoSelectedArea.h
 *
 * DESCRIPTION: Class for selecting a area or line
 *
 * CREATED BY: Jiang Dalong, 2006/04/06
 *
 * HISTORY: 
 *
 * Copyright (c) 2001-2008 Archosaur Studio, All Rights Reserved.
 */

#ifndef _AUTOSELECTEDAREA_H_
#define _AUTOSELECTEDAREA_H_

#include "AutoSceneFunc.h"
#include <AArray.h>
#include <APoint.h>
#include <A3DVector.h>

class CAutoHome;

class CAutoSelectedArea
{
public:
	CAutoSelectedArea(CAutoHome* pAutoHome);
	virtual ~CAutoSelectedArea();

	void Release();
	bool Render();

	/************************************************************
	Poly area functions
	*************************************************************/
	// Operation for edit points
	bool AddPolyEditPoint(const POINT_FLOAT& pt, const A3DVECTOR3& vStart, const A3DVECTOR3& vEnd);
	bool DeletePolyLastEditPoint();
	bool DeletePolyEditPoint(int nIndex);
	void DeleteAllPolyEditPoints();
	int GetNumPolyEditPoints() { return m_aPolyEditPoints.GetSize(); }
	POINT_FLOAT* GetPolyEditPoints();
	bool SetPolyEditPoints(int nNumPoints, POINT_FLOAT* pPoints);

	bool TranslatePolyEditPoints(A3DVECTOR3& vTrans);
	bool RotatePolyEditPoints(float fAngle);
	bool ScalePolyEditPoints(float fScale);

	// Check whether area can be closed
	bool PolyAreaCanClosed(AArray<POINT_FLOAT, POINT_FLOAT&>& aPoints);
	bool SetPolyEditAreaClosed(bool bClose);
	inline bool GetPolyEditAreaClosed() { return m_bPolyEditAreaClosed; }

	/************************************************************
	Line area functions
	*************************************************************/
	// Operation for edit points
	bool AddLineEditPoint(const POINT_FLOAT& pt);			
	bool DeleteLineLastEditPoint();
	bool DeleteLineEditPoint(int nIndex);
	void DeleteAllLineEditPoints();
	int GetNumLineEditPoints() { return m_aLineEditPoints.GetSize(); }
	POINT_FLOAT* GetLineEditPoints();
	bool SetLineEditPoints(int nNumPoints, POINT_FLOAT* pPoints);

	bool TranslateLineEditPoints(A3DVECTOR3& vTrans);
	bool RotateLineEditPoints(float fAngle);
	bool ScaleLineEditPoints(float fScale);

	/************************************************************
	View points functions
	*************************************************************/
	// Operation for view points
	void DeleteAllViewPoints();
	int GetNumViewPoints() { return m_aViewPoints.GetSize(); }
	POINT_FLOAT* GetViewPoints();
	bool SetViewPoints(int nNumPoints, POINT_FLOAT* pPoints);
	bool SetViewAreaClosed(bool bClose);
	inline bool GetViewAreaClosed() { return m_bViewAreaClosed; }

	// Operation between view and edit points
	bool CopyPointsFromViewToPolyEdit();
	bool CopyPointsFromViewToLineEdit();
	bool CopyPointsFromPolyEditToView();
	bool CopyPointsFromLineEditToView();

	/************************************************************
	Other functions
	*************************************************************/
	inline void SetAreaType(int nType) { m_nAreatype = nType; }
	inline int GetAreatype() { return m_nAreatype; }

protected:
	CAutoHome*			m_pAutoHome;

	// Poly valuables
	AArray<POINT_FLOAT, POINT_FLOAT&>		m_aPolyEditPoints;		// Edit points on the map
	POINT_FLOAT*		m_pPolyEditPoints;
	POINT_FLOAT			m_ptPolyEditCenter;							// Center of edit area
	bool				m_bPolyEditAreaClosed;						// Is edit area closed?
	bool				m_bPolyEditAreaChanged;						// Is edit area changed?
	float				m_fMaxPolyEditAreaEdge;						// Min area edge

	// Line valuables
	AArray<POINT_FLOAT, POINT_FLOAT&>		m_aLineEditPoints;		// Edit points on the map
	POINT_FLOAT*		m_pLineEditPoints;
	POINT_FLOAT			m_ptLineEditCenter;							// Center of edit area
	bool				m_bLineEditAreaChanged;						// Is edit area changed?
	float				m_fMaxLineEditAreaEdge;						// Min area edge

	// View valuables
	AArray<POINT_FLOAT, POINT_FLOAT&>		m_aViewPoints;			// View points on the map
	POINT_FLOAT*		m_pViewPoints;
	bool				m_bViewAreaClosed;							// Is view area closed?

	int					m_nAreatype;								// Area type

protected:
	bool TranslateEditPoints(A3DVECTOR3& vTrans, AArray<POINT_FLOAT, POINT_FLOAT&>& aPoints, bool* pbChanged);
	bool RotateEditPoints(float fAngle, AArray<POINT_FLOAT, POINT_FLOAT&>& aPoints, float* pfMaxAreaEdge, POINT_FLOAT& ptCenter, bool* pbChanged);
	bool ScaleEditPoints(float fScale, AArray<POINT_FLOAT, POINT_FLOAT&>& aPoints, float* pfMaxAreaEdge, POINT_FLOAT& ptCenter, bool* pbChanged);
	// Is a new point make the lines cross?
	bool IsNextPointCrossLine(const POINT_FLOAT& point, AArray<POINT_FLOAT, POINT_FLOAT&>& pPoints, bool bFromFirstPoint);
	// Calculate center and min edge of edit area
	bool GetEditAreaCenterAndMaxEdge(AArray<POINT_FLOAT, POINT_FLOAT&>& aPoints, float* pfMaxAreaEdge, POINT_FLOAT& ptCenter, bool* pbChanged);
	// Is point in map
	bool IsPointInMap(const POINT_FLOAT& point);
	// Is a new point make the lines cross?
	bool IsNextPointCrossLine(const POINT_FLOAT& point, AArray<POINT_FLOAT, POINT_FLOAT&>& aPoints);
	// Check ray trace from a screen point with a height map point
	bool RayTracePoint(const A3DVECTOR3& vStart, const A3DVECTOR3& vEnd, const POINT_FLOAT& pt);
	// From world to home rect
	bool WorldToHomeRect(const POINT_FLOAT& ptWorld, POINT_FLOAT& ptHome);
};

#endif // #ifndef _AUTOSELECTEDAREA_H
