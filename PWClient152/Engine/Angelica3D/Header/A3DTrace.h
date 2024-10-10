/*
 * FILE: A3DTrace.h
 *
 * DESCRIPTION: Routines for trace algorithm and file
 *
 * CREATED BY: duyuxin, 2001/11/5
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef	_A3DTRACE_H_
#define _A3DTRACE_H_

#include "A3DTypes.h"
#include "A3DGeometry.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

#define MOVE_OVERBOUNCE		1.001f

//	An obb may have 26 bevels, 6 side + 8 vertices + 12 borders
#define MAXNUM_OBBBEVEL		26

///////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
///////////////////////////////////////////////////////////////////////////

//	Ray trace result structure
struct RAYTRACERT
{
	A3DVECTOR3		vPoint;			//	Adjusted collision point
	A3DVECTOR3		vHitPos;		//	Real collision point, this is only valid when hit object is a part of ESP
	A3DVECTOR3		vNormal;		//	Normal of plane on which side lies
	FLOAT			fFraction;		//	Hit fraction (0 start point, 1 end point)
};

//	OBB trace structure
struct OBBTRACERT
{
	A3DVECTOR3		vPoint;			//	Collision point
	A3DVECTOR3		vNormal;		//	Normal of plane on which side lies
	FLOAT			fFraction;		//	move fraction (0 start point, 1 end point)
	FLOAT			fOBBFraction;	//	OBB fraction
};

//	AABB trace result structure
struct AABBTRACERT
{
	A3DVECTOR3		vDestPos;		//	Position AABB can move to
	A3DVECTOR3		vNormal;		//	Normal of plane on which side lies
	FLOAT			fFraction;		//	move fraction (0 start point, 1 end point)
};

//	Sphere trace result structure
struct SPHERETRACERT
{
	A3DVECTOR3		vDestPos;		//	Position sphere can move to
	A3DVECTOR3		vNormal;		//	Normal of plane on which side lies
	FLOAT			fFraction;		//	move fraction (0 start point, 1 end point)
};

//	AABB trace information used in AABB trace routines
struct AABBTRACEINFO
{
	//	In
	A3DVECTOR3	vStart;			//	Start point
	A3DVECTOR3	vExtents;		//	AABB's extents
	FLOAT		fTime;			//	Time
	A3DVECTOR3	vDelta;			//	Move delta = velocity * time
	A3DVECTOR3	vEnd;			//	End point = vStart + vDelta
	A3DAABB		BoundAABB;		//	AABB which contain whole sweep area
	int			iType;			//	Trace type
	FLOAT		fEpsilon;		//	Epsilon

	//	Out
	bool		bStartSolid;	//	Collide something at start point
	bool		bAllSolid;		//	All in something
	A3DPLANE	ClipPlane;		//	Clip plane
	int			iClipPlane;		//	Clip plane's index
	float		fFraction;		//	Fraction
};

//	OBB bevels
struct OBBBEVELS
{
	A3DSPLANE	aPlanes[MAXNUM_OBBBEVEL];		//	Bevel planes
	A3DSPLANE*	aPlaneAddrs[MAXNUM_OBBBEVEL];	//	Plane addresses
	int			iNumPlane;						//	Number of planes in aPlanes
};

//	Sphere trace information used by sphere trace routines
struct SPHERETRACEINFO
{
	A3DVECTOR3	vStart;			//	Start point
	float		fRadius;		//	Sphere's radius
	float		fTime;			//	Time
	A3DVECTOR3	vDelta;			//	Move delta = velocity * time
	A3DVECTOR3	vEnd;			//	End point = vStart + vDelta
	A3DAABB		BoundAABB;		//	AABB which contain whole sweep area
	int			iType;			//	Trace type
	FLOAT		fEpsilon;		//	Epsilon

	//	Out
	bool		bStartSolid;	//	Collide something at start point
	bool		bAllSolid;		//	All in something
	A3DPLANE	ClipPlane;		//	Clip plane
	int			iClipPlane;		//	Clip plane's index
	float		fFraction;		//	Fraction
};

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////

void TRA_AABBTraceInit(AABBTRACEINFO* pInfo, const A3DVECTOR3& vStart, const A3DVECTOR3& vExts, const A3DVECTOR3& vVelocity, FLOAT fTime, FLOAT fEpsilon=0.03125f);
void TRA_BuildOBBBevels(const A3DOBB& OBB, OBBBEVELS* pBevels);
bool TRA_AABBMoveToBrush(AABBTRACEINFO* pInfo, A3DSPLANE** aPlanes, int iNumPlane);
bool TRA_AABBMoveToAABB(AABBTRACEINFO* pInfo, const A3DAABB& aabb);
bool TRA_AABBMoveToOBB(AABBTRACEINFO* pInfo, const A3DOBB& obb);

void TRA_SphereTraceInit(SPHERETRACEINFO* pInfo, const A3DVECTOR3& vStart, FLOAT fRadius, const A3DVECTOR3& vVelocity, FLOAT fTime, FLOAT fEpsilon=0.03125f);
bool TRA_SphereMoveToBrush(SPHERETRACEINFO* pInfo, A3DSPLANE** aPlanes, int iNumPlane);
bool TRA_SphereMoveToAABB(SPHERETRACEINFO* pInfo, const A3DAABB& aabb);
bool TRA_SphereMoveToOBB(SPHERETRACEINFO* pInfo, const A3DOBB& obb);

#endif	//	_A3DTRACE_H_
