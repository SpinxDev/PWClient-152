/*
 * FILE: AutoHomeBorder.h
 *
 * DESCRIPTION: Class for lines of home border
 *
 * CREATED BY: Jiang Dalong, 2006/07/04
 *
 * HISTORY: 
 *
 * Copyright (c) 2001-2008 Archosaur Studio, All Rights Reserved.
 */

#ifndef _AUTOHOMEBORDER_H_
#define _AUTOHOMEBORDER_H_

#include <A3DVector.h>

class CAutoHome;

class CAutoHomeBorder  
{
public:
	enum BORDERPOINTTYPE
	{
		BPT_CROSS,
		BPT_ROW,
		BPT_COL,
	};
	struct BORDERPOINT 
	{
		int nType;
		int nIndex;
		A3DVECTOR3 vPosWorld;
	};

public:
	CAutoHomeBorder(CAutoHome* pAutoHome);
	virtual ~CAutoHomeBorder();

	// Initialize border points
	bool Init();
	void Release();
	bool Render();
	bool SelectPoint(const A3DVECTOR3& vStart, const A3DVECTOR3& vEnd);
	void ReleasePoint();
	bool GetSelectedPos(A3DVECTOR3& vPos);
	bool MoveSelectedPoint(float fDis);

	bool GetHomeTopPoints(float* pPoints);
	bool GetHomeRightPoints(float* pPoints);
	bool GetHomeBottomPoints(float* pPoints);
	bool GetHomeLeftPoints(float* pPoints);

	inline float GetHeight(int nIndex) { return m_pBorderPoints[nIndex].vPosWorld.y; }
	inline void SetSelectedPoint(bool bSelected) { m_bSelectedPoint = bSelected; }
	inline bool IsSelectedPoint() { return m_bSelectedPoint; }
	
protected:
	CAutoHome*		m_pAutoHome;
	BORDERPOINT*	m_pBorderPoints;	// Border key points
	int				m_nNumPoints;		// Total border points
	int				m_nNumInside;		// a border's points without corner
	bool			m_bSelectedPoint;
	int				m_nSelectedPoint;

protected:
	// Check ray trace with a world point
	bool RayTracePoint(const A3DVECTOR3& vStart, const A3DVECTOR3& vEnd, const A3DVECTOR3& vWorld);
	// Move point in data
	bool MovePointData(int nIndex, float fDis);
};

#endif // #ifndef _AUTOHOMEBORDER_H_

