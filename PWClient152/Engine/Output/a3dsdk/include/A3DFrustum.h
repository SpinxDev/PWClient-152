/*
 * FILE: A3DFrustum.h
 *
 * DESCRIPTION: Frustum class
 *
 * CREATED BY: duyuxin, 2003/6/18
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#ifndef _A3DFRUSTUM_H_
#define _A3DFRUSTUM_H_

#include "A3DTypes.h"
#include "A3DGeometry.h"
#include "AArray.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
///////////////////////////////////////////////////////////////////////////

class A3DCameraBase;
class A3DViewport;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	class A3DFrustum
//
///////////////////////////////////////////////////////////////////////////

class A3DFrustum
{
public:		//	Types

	//	Index of view frustum plane
	enum
	{
		VF_LEFT = 0,
		VF_RIGHT,
		VF_TOP,	
		VF_BOTTOM,
		VF_NEAR,
		VF_FAR,
	};

	//	Clip plane
	struct PLANE
	{
		A3DSPLANE	Plane;		//	Plane data
		bool		bEnable;	//	Enable flag
	};

public:		//	Constructions and Destructions

	A3DFrustum();
	virtual ~A3DFrustum() {}

public:		//	Attributes

public:		//	Operaitons

	//	Initialize object
	bool Init(int iNumPlane);
	bool Init(A3DCameraBase* pCamera, float fLeft, float fTop, float fRight, float fBottom, bool bZClip);
	bool Init(A3DViewport* pView, int iLeft, int iTop, int iRight, int iBottom, bool bZClip);
	//	Release object
	void Release();

	//	Add a clip plane
	bool AddPlane(const A3DSPLANE& Plane);
	bool AddPlane(const A3DVECTOR3& vNormal, float fDist);

	//	Check if AABB is in or out frustum
	int AABBInFrustum(const A3DVECTOR3& vMins, const A3DVECTOR3& vMaxs);
	int AABBInFrustum(const A3DAABB& aabb) { return AABBInFrustum(aabb.Mins, aabb.Maxs); }
	//	Check if sphere is in or out frustum
	int SphereInFrustum(const A3DVECTOR3& vCenter, float fRadius);
	//	Check if point is in or out frustum
	bool PointInFrustum(const A3DVECTOR3& vPos) { return SphereInFrustum(vPos, 0.0f) >= 0 ? true : false; }

	//	Get plane's pointer. The return address is temporary, so use it immedialtely
	A3DSPLANE* GetPlane(int iIndex) { return &m_aPlanes[iIndex].Plane; }
	//	Get number of clip plane
	int GetPlaneNum() { return m_aPlanes.GetSize(); }
	//	Enable clip plane
	void EnablePlane(int iIndex, bool bEnable) { m_aPlanes[iIndex].bEnable = bEnable; }
	//	Get enable flag
	bool PlaneIsEnable(int iIndex) { return m_aPlanes[iIndex].bEnable; }

	inline const A3DFrustum& operator = (const A3DFrustum& frustum)
	{
		assert(m_aPlanes.GetSize() == frustum.m_aPlanes.GetSize());
		for(int i=0; i<m_aPlanes.GetSize(); i++)
			m_aPlanes.SetAt(i, frustum.m_aPlanes[i]);
		return *this;
	}

protected:	//	Attributes

	AArray<PLANE, PLANE&>	m_aPlanes;

protected:	//	Operations
	
	//	Create clip plane
	void CreateClipPlane(const A3DVECTOR3& v0, const A3DVECTOR3& v1, const A3DVECTOR3& v2, A3DSPLANE* pPlane);
};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_A3DFRUSTUM_H_
