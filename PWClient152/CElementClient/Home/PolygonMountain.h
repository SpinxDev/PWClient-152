/*
 * FILE: PolygonMountain.h
 *
 * DESCRIPTION: Class for creating mountain in a polygon shape
 *
 * CREATED BY: Jiang Dalong, 2005/04/06
 *
 * HISTORY: 
 *
 * Copyright (c) 2001-2008 Archosaur Studio, All Rights Reserved.
 */

#ifndef _POLYGONMOUNTAIN_H_
#define _POLYGONMOUNTAIN_H_

#include "ClosedArea.h"

const int MAX_COUNT_PARTMAXDIS			= 770;

class CPolygonMountain : public CClosedArea 
{
public:
	struct PARTMAXDISINFO
	{
		POINT_FLOAT		point[MAX_COUNT_PARTMAXDIS];
		int				nCount;
	};
public:
	CPolygonMountain();
	virtual ~CPolygonMountain();

public:
	// Release memory
	void Release();
	// Min distance to area edge
	// pMinDisType: 0: 不是局部最大点；1:角度小的；2：角度大的；3：中间的；4：凹角周围的
	float DistanceToEdge(POINT_FLOAT& point, bool bInPolygon, int* pMinDisType);
	// Find salient angle in the polygon
	int FindSalient(float fMinCosValue);
	// Release salient
	void ReleaseSalient();

	inline PARTMAXDISINFO* GetSalientPointsMidLine() { return m_pSalientPointsMidLine; }
	inline int GetNumSalientPoints() { return m_nSalientPoints; }
	inline PARTMAXDISINFO* GetSalientPointsMideLine() { return m_pSalientPointsMidLine; }

protected:
	bool*					m_pSalient;			// Salient points
	PARTMAXDISINFO*			m_pSalientPointsMidLine;// Some Points of max distance in a line
	int						m_nSalientPoints;	// The count of Salient points
	int*					m_pMaxDisLineMapping;// Mapping from point index to max distance line index

protected:

};

#endif // #ifndef _POLYGONMOUNTAIN_H_
