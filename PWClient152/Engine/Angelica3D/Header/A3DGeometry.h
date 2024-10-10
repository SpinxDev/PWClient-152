/*
 * FILE: A3DGeometry.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/4/9
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#ifndef _A3DGEOMETRY_H_
#define _A3DGEOMETRY_H_

#include "ABaseDef.h"
#include "A3DVector.h"

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

class A3DOBB;


///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////

/*	Create a indexed sphere mesh. This function build a sphere mesh whose center locates at
	vPos and has radius specified by fRadius. Sphere's segments are specified by iRow (>= 3) 
	and iCol (>= 3). The final mesh will have below vertex number and index number
	vertex number: (2 + (iRow-1) * iCol)
	index number: ((iRow-1) * iCol * 6)
*/
bool a3d_CreateIndexedSphere(const A3DVECTOR3& vPos, float fRadius, int iRow, int iCol, 
					A3DVECTOR3* aVerts, int iMaxVertNum, WORD* aIndices, int iMaxIdxNum);

/*	Create a indexed taper mesh. This function build a taper mesh whose top locates at
	vPos and vDir specified it's height axis. 
	The final mesh will have below vertex number and index number

	if (bHasBottom == true)
	{
		vertex number: (iNumSeg + 1)
		index number: iNumSeg * 3 + (iNumSeg - 2) * 3
	}
	else
	{
		vertex number: (iNumSeg + 1)
		index number: iNumSeg * 3
	}

	vDir: normalized direction from top position to center of bottom
	fHeight: distance between taper top and bottom plane
	fRadius: radius of bottom circle
	iNumSeg: segment of bottom circle, >= 3
	bHasBottom: true, has bottom cap; false, don't has bottom cap
*/
bool a3d_CreateIndexedTaper(const A3DVECTOR3& vPos, const A3DVECTOR3& vDir,
					float fRadius, float fHeight, int iNumSeg, bool bHasBottom, 
					A3DVECTOR3* aVerts, int iMaxVertNum, WORD* aIndices, int iMaxIdxNum);

//	Create a indexed box that has 8 vertices and 36 indices
//	vExtent: half border length
bool a3d_CreateIndexedBox(const A3DVECTOR3& vPos, const A3DVECTOR3& vX,
					const A3DVECTOR3& vY, const A3DVECTOR3& vZ, const A3DVECTOR3& vExtent,
					A3DVECTOR3* aVerts, WORD* aIndices);

///////////////////////////////////////////////////////////////////////////
//	
//	Class A3DAABB
//	
///////////////////////////////////////////////////////////////////////////

//	Axis-Aligned Bounding Box
class A3DAABB
{
public:		//	Constructors and Destructors

	A3DAABB() {}

	A3DAABB(const A3DAABB& aabb) : 
	Center(aabb.Center),
	Extents(aabb.Extents),
	Mins(aabb.Mins),
	Maxs(aabb.Maxs) {}

	A3DAABB(const A3DVECTOR3& vMins, const A3DVECTOR3& vMaxs) :
	Mins(vMins),
	Maxs(vMaxs),
	Center((vMins + vMaxs) * 0.5f)
	{
		Extents = vMaxs - Center;
	}

public:		//	Attributes

	A3DVECTOR3	Center;
	A3DVECTOR3	Extents;
	A3DVECTOR3	Mins;
	A3DVECTOR3	Maxs;

public:		//	Operations

	//	Clear aabb
	void Clear()
	{
		Mins.Set(999999.0f, 999999.0f, 999999.0f);
		Maxs.Set(-999999.0f, -999999.0f, -999999.0f);
	}

	//	Add a vertex to aabb
	void AddVertex(const A3DVECTOR3& v);

	//	Build AABB from obb
	void Build(const A3DOBB& obb);
	//	Merge two aabb
	void Merge(const A3DAABB& subAABB);

	//	Compute Mins and Maxs
	void CompleteMinsMaxs()
	{
		Mins = Center - Extents;
		Maxs = Center + Extents;
	}

	//	Compute Center and Extents
	void CompleteCenterExts()
	{
		Center  = (Mins + Maxs) * 0.5f;
		Extents = Maxs - Center;
	}

	//	Check whether a point is in this aabb
	bool IsPointIn(const A3DVECTOR3& v) const
	{
		if (v.x > Maxs.x || v.x < Mins.x ||
			v.y > Maxs.y || v.y < Mins.y ||
			v.z > Maxs.z || v.z < Mins.z)
			return false;

		return true;
	}

	//	Check whether another aabb is in this aabb
	bool IsAABBIn(const A3DAABB& aabb) const;

	//	Build AABB from vertices
	void Build(const A3DVECTOR3* aVertPos, int iNumVert);
	//	Get vertices of aabb
	void GetVertices(A3DVECTOR3* aVertPos, WORD* aIndices, bool bWire) const;
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class A3DOBB
//	
///////////////////////////////////////////////////////////////////////////

//	Oriented Bounding Box
class A3DOBB
{
public:		//	Constructors and Destructors

	A3DOBB() {}
	A3DOBB(const A3DOBB& obb) : 
	Center(obb.Center),
	XAxis(obb.XAxis),
	YAxis(obb.YAxis),
	ZAxis(obb.ZAxis),
	ExtX(obb.ExtX),
	ExtY(obb.ExtY),
	ExtZ(obb.ExtZ),
	Extents(obb.Extents) {}

public:		//	Attributes

	A3DVECTOR3	Center;
	A3DVECTOR3	XAxis;
	A3DVECTOR3	YAxis;
	A3DVECTOR3	ZAxis;
	A3DVECTOR3	ExtX;
	A3DVECTOR3	ExtY;
	A3DVECTOR3	ExtZ;
	A3DVECTOR3	Extents;

public:		//	Operations

	//	Check whether a point is in this aabb
	bool IsPointIn(const A3DVECTOR3& v) const;

	//	Compute ExtX, ExtY, ExtZ
	void CompleteExtAxis()
	{
		ExtX = XAxis * Extents.x;
		ExtY = YAxis * Extents.y;
		ExtZ = ZAxis * Extents.z;
	}

	//	Clear obb
	void Clear();

	//	Build obb from two obbs
	void Build(const A3DOBB& obb1, const A3DOBB obb2);
	//	Build obb from vertices
	void Build(const A3DVECTOR3* aVertPos, int nVertCount);
	//	Build obb from aabb
	void Build(const A3DAABB& aabb);
	//	Get vertices of obb
	void GetVertices(A3DVECTOR3* aVertPos, WORD* aIndices, bool bWire) const;
};

///////////////////////////////////////////////////////////////////////////
//	
//	class A3DPLANE and A3DSPLANE
//	
///////////////////////////////////////////////////////////////////////////

//	Plane
class A3DPLANE
{
public:		//	Constructors and Destructors

	A3DPLANE() {}
	A3DPLANE(const A3DPLANE& p) : vNormal(p.vNormal) { fDist = p.fDist;	}
	A3DPLANE(const A3DVECTOR3& n, FLOAT d) { vNormal = n; fDist = d; }

public:		//	Attributes

	A3DVECTOR3	vNormal;		//	Normal
	FLOAT		fDist;			//	d parameter

public:		//	Operations

	//	Create plane from 3 points
	bool CreatePlane(const A3DVECTOR3& v1, const A3DVECTOR3& v2, const A3DVECTOR3& v3);
};

//	Plane with sign
class A3DSPLANE : public A3DPLANE
{
public:		//	Types

	//	Plane type
	enum
	{
		TYPE_BAD		= -1,	//	Bad plane
		TYPE_PX			= 0,	//	Positive x axis
		TYPE_PY			= 1,	//	Positive y axis
		TYPE_PZ			= 2,	//	Positive z axis
		TYPE_NX			= 3,	//	Negative x axis
		TYPE_NY			= 4,	//	Negative y axis
		TYPE_NZ			= 5,	//	Negative z axis
		TYPE_MAJORX		= 6,	//	Major axis is x
		TYPE_MAJORY		= 7,	//	Major axis is y
		TYPE_MAJORZ		= 8,	//	Major axis is z
		TYPE_UNKNOWN	= 9,	//	unknown
	};

	//	Plane sign flag
	enum
	{
		SIGN_P	= 0,	//	Positive
		SIGN_NX	= 1,	//	x axis is negative
		SIGN_NY = 2,	//	y axis is negative
		SIGN_NZ = 4,	//	z axis is negative
	};

public:		//	Constructors and Destructors

	A3DSPLANE() {}
	A3DSPLANE(const A3DSPLANE& p) { vNormal = p.vNormal; fDist = p.fDist; byType = p.byType; bySignBits = p.bySignBits;	}
	A3DSPLANE(const A3DVECTOR3& n, FLOAT d) : A3DPLANE(n, d) { byType = TYPE_UNKNOWN; bySignBits = SIGN_P; }

public:		//	Attributes

	BYTE	byType;			//	Type of plane
	BYTE	bySignBits;		//	Sign flags

public:		//	Operations

	//	Make plane type
	void MakeType();
	//	Make plane sign-bits
	void MakeSignBits();
	//	Make plane type and sign-bites
	void MakeTypeAndSignBits()
	{
		MakeType();
		MakeSignBits();
	}
};

///////////////////////////////////////////////////////////////////////////
//	
//	class A3DCAPSULE
//	
///////////////////////////////////////////////////////////////////////////

//	Capsule
class A3DCAPSULE
{
public:		//	Constructors and Destructors

	A3DCAPSULE() {}
	A3DCAPSULE(const A3DCAPSULE& src) :
	vCenter(src.vCenter),
	vAxisX(src.vAxisX),
	vAxisY(src.vAxisY),
	vAxisZ(src.vAxisZ)
	{
		fHalfLen	= src.fHalfLen;
		fRadius		= src.fRadius;
	}

public:		//	Attributes

	A3DVECTOR3	vCenter;
	A3DVECTOR3	vAxisX;
	A3DVECTOR3	vAxisY;
	A3DVECTOR3	vAxisZ;
	float		fHalfLen;		//	Half length (on Y axis)
	float		fRadius;		//	Radius

public:		//	Operations

	//	Check whether a point is in this capsule
	bool IsPointIn(const A3DVECTOR3& vPos);
};

///////////////////////////////////////////////////////////////////////////
//	
//	class A3DCYLINDER
//	
///////////////////////////////////////////////////////////////////////////

//	Cylinder
class A3DCYLINDER
{
public:		//	Constructors and Destructors

	A3DCYLINDER() {}
	A3DCYLINDER(const A3DCYLINDER& src) :
	vCenter(src.vCenter),
	vAxisX(src.vAxisX),
	vAxisY(src.vAxisY),
	vAxisZ(src.vAxisZ)
	{
		fHalfLen	= src.fHalfLen;
		fRadius		= src.fRadius;
	}

public:		//	Attributes

	A3DVECTOR3	vCenter;
	A3DVECTOR3	vAxisX;
	A3DVECTOR3	vAxisY;
	A3DVECTOR3	vAxisZ;
	float		fHalfLen;		//	Half length (on Y axis)
	float		fRadius;		//	Radius

public:		//	Operations

	//	Check whether a point is in this cylinder
	bool IsPointIn(const A3DVECTOR3& vPos);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////


#endif	//	_A3DGEOMETRY_H_
