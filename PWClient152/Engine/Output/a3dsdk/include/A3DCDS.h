/*
 * FILE: A3DCDS.h
 *
 * DESCRIPTION: The class resolve collision detection and visibility check
 *
 * CREATED BY: Hedi, 2001/7/27
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DCDS_H_
#define _A3DCDS_H_

#include "A3DTypes.h"
#include "A3DGeometry.h"
#include "A3DTrace.h"
#include "A3DVertex.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

//	Trace object
enum
{
	TRACEOBJ_PARTICAL	= -1,	//	Particle
	TRACEOBJ_RAIN		= -2,	//	Rain
	TRACEOBJ_LIGHT		= -3,	//	Light (lamp, sun light...)
	TRACEOBJ_USER		= 0,	//	User defined object start from here
};

//	Visible check object
enum
{
	VISOBJ_PARTICAL		= -1,	//	Particle
	VISOBJ_LAMP			= -2,	//	Lamp
	VISOBJ_SKINMODEL	= -3,	//	Skin model
	VISOBJ_MODEL		= -4,	//	Traditional model
	VISOBJ_USER			= 0,	//	User defined object start from here
};

///////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class A3DCDS
//
///////////////////////////////////////////////////////////////////////////

class A3DCDS
{
public:

	//	Ray trace routine, Return true if collision occured
	virtual bool RayTrace(const A3DVECTOR3& vStart, const A3DVECTOR3& vVel, FLOAT fTime, RAYTRACERT* pTraceRt, int iTraceObj, DWORD dwUserData) = 0;
	//	AABB trace routine, Return true if collision occured
	virtual bool AABBTrace(const A3DVECTOR3& vStart, const A3DVECTOR3& vExts, const A3DVECTOR3& vVel, FLOAT fTime, AABBTRACERT* pTraceRt, int iTraceObj, DWORD dwUserData) = 0;
	//	Sphere trace routine, Return true if collision occured
	virtual bool SphereTrace(const A3DVECTOR3& vStart, float fRadius, A3DVECTOR3& vVel, FLOAT fTime, SPHERETRACERT* pTraceRt, int iTraceObj, DWORD dwUserData) = 0;
	//	Split mark routine
	virtual bool SplitMark(const A3DAABB& aabb, const A3DVECTOR3& vNormal, A3DLVERTEX* aVerts, WORD* aIndices, 
						bool bJog, int* piNumVert, int* piNumIdx, float fRadiusScale=0.2f) = 0;

	//	Return true if this CDS can split mark
	virtual bool CanSplitMark() = 0;

	//	Position is visible ?
	virtual bool PosIsVisible(const A3DVECTOR3& vPos, int iVisObj, DWORD dwUserData) = 0;
	//	AABB is visible ?
	virtual bool AABBIsVisible(const A3DAABB& aabb, int iVisObj, DWORD dwUserData) = 0;
	//	AABB is visible ?
	virtual bool AABBIsVisible(const A3DVECTOR3& vMins, const A3DVECTOR3& vMaxs, int iVisObj, DWORD dwUserData) = 0;
};


#endif	//	_A3DCSD_H_
