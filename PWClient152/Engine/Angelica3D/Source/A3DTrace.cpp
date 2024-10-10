/*
 * FILE: A3DTrace.cpp
 *
 * DESCRIPTION: Routines for trace algorithm
 *
 * CREATED BY: duyuxin, 2001/11/5
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "A3DPI.h"
#include "A3DFuncs.h"
#include "A3DCollision.h"
#include "A3DTrace.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

//	Trace object type
enum
{
	TRACE_STATIC = 0,	//	Not move
	TRACE_MOVING,
};

#define EPSILON_FRACTION	0.03125f

///////////////////////////////////////////////////////////////////////////
//
//	Reference to External variables and functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Local Types and Variables and Global variables
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Local functions
//
///////////////////////////////////////////////////////////////////////////

static bool _MovingAABBToBrush(AABBTRACEINFO* pInfo, A3DSPLANE** aPlanes, int iNumPlane);
static bool _StaticAABBToBrush(AABBTRACEINFO* pInfo, A3DSPLANE** aPlanes, int iNumPlane);
static bool _MovingSphereToBrush(SPHERETRACEINFO* pInfo, A3DSPLANE** aPlanes, int iNumPlane);
static bool _StaticSphereToBrush(SPHERETRACEINFO* pInfo, A3DSPLANE** aPlanes, int iNumPlane);

///////////////////////////////////////////////////////////////////////////
//
//	Implement
//
///////////////////////////////////////////////////////////////////////////

/*	Build OBB bevels for AABB trace.

	OBB: OBB information.
	pBevels: buffer used to store bevels
*/
void TRA_BuildOBBBevels(const A3DOBB& OBB, OBBBEVELS* pBevels)
{
	A3DVECTOR3 vVert, vNormal;
	const A3DVECTOR3* aAxis[3];
	int i, j, n = 0;
	A3DSPLANE* pPlane;
	float fDist;

	aAxis[0] = &OBB.XAxis;
	aAxis[1] = &OBB.YAxis;
	aAxis[2] = &OBB.ZAxis;

	vVert = OBB.Center + OBB.ExtX + OBB.ExtY + OBB.ExtZ;

	//	6 side planes ----------------------------
	for (i=0; i < 3; i++)
	{
		//	Positive side
		pPlane = &pBevels->aPlanes[n++];
		pPlane->vNormal = *aAxis[i];
		pPlane->fDist	= fDist = DotProduct(vVert, pPlane->vNormal);
		
		//	Negative side
		pPlane = &pBevels->aPlanes[n++];
		pPlane->vNormal = -(*aAxis[i]);
		pPlane->fDist	= -(fDist - OBB.Extents.m[i] * 2.0f);
	}

	//	If this OBB is a AABB, these are enough
	for (i=0; i < 2; i++)
	{
		for (j=0; j < 3; j++)
		{
			if (aAxis[i]->m[j] == 1.0f || aAxis[i]->m[j] == -1.0f)
				break;
		}

		if (j >= 3)
			break;
	}

	if (i >= 2)		//	Ok, two axis are axial
		goto End;

	//	8 vertex planes --------------------------
	for (i=-1; i <= 1; i+=2)
	{
		for (j=-1; j <= 1; j+=2)
		{
			vVert = OBB.Center + OBB.ExtX*(float)i + OBB.ExtY*(float)j - OBB.ExtZ;

			pPlane = &pBevels->aPlanes[n++];
			pPlane->vNormal = Normalize(OBB.XAxis*(float)i + OBB.YAxis*(float)j - OBB.ZAxis);
			pPlane->fDist	= DotProduct(vVert, pPlane->vNormal);
			
			vVert = OBB.Center + OBB.ExtX*(float)i + OBB.ExtY*(float)j + OBB.ExtZ;

			pPlane = &pBevels->aPlanes[n++];
			pPlane->vNormal = Normalize(OBB.XAxis*(float)i + OBB.YAxis*(float)j + OBB.ZAxis);
			pPlane->fDist	= DotProduct(vVert, pPlane->vNormal);
		}
	}

	//	12 border planes --------------------------
	//	Pair 1 ---------
	vVert  = OBB.Center + OBB.ExtX + OBB.ExtY;	
	pPlane = &pBevels->aPlanes[n++];
	pPlane->vNormal = Normalize(OBB.XAxis + OBB.YAxis);
	pPlane->fDist	= DotProduct(vVert, pPlane->vNormal);

	vVert  = OBB.Center - OBB.ExtX - OBB.ExtY;	
	pPlane = &pBevels->aPlanes[n++];
	pPlane->vNormal = -pBevels->aPlanes[n-2].vNormal;
	pPlane->fDist	= DotProduct(vVert, pPlane->vNormal);

	//	Pair 2---------
	vVert  = OBB.Center + OBB.ExtX - OBB.ExtY;	
	pPlane = &pBevels->aPlanes[n++];
	pPlane->vNormal = Normalize(OBB.XAxis - OBB.YAxis);
	pPlane->fDist	= DotProduct(vVert, pPlane->vNormal);

	vVert  = OBB.Center - OBB.ExtX + OBB.ExtY;	
	pPlane = &pBevels->aPlanes[n++];
	pPlane->vNormal = -pBevels->aPlanes[n-2].vNormal;
	pPlane->fDist	= DotProduct(vVert, pPlane->vNormal);

	//	Pair 3 --------
	vVert  = OBB.Center + OBB.ExtX + OBB.ExtZ;	
	pPlane = &pBevels->aPlanes[n++];
	pPlane->vNormal = Normalize(OBB.XAxis + OBB.ZAxis);
	pPlane->fDist	= DotProduct(vVert, pPlane->vNormal);

	vVert  = OBB.Center - OBB.ExtX - OBB.ExtZ;	
	pPlane = &pBevels->aPlanes[n++];
	pPlane->vNormal = -pBevels->aPlanes[n-2].vNormal;
	pPlane->fDist	= DotProduct(vVert, pPlane->vNormal);

	//	Pair 4 --------
	vVert  = OBB.Center + OBB.ExtX - OBB.ExtZ;	
	pPlane = &pBevels->aPlanes[n++];
	pPlane->vNormal = Normalize(OBB.XAxis - OBB.ZAxis);
	pPlane->fDist	= DotProduct(vVert, pPlane->vNormal);

	vVert  = OBB.Center - OBB.ExtX + OBB.ExtZ;	
	pPlane = &pBevels->aPlanes[n++];
	pPlane->vNormal = -pBevels->aPlanes[n-2].vNormal;
	pPlane->fDist	= DotProduct(vVert, pPlane->vNormal);

	//	Pair 5 --------
	vVert  = OBB.Center + OBB.ExtY + OBB.ExtZ;
	pPlane = &pBevels->aPlanes[n++];
	pPlane->vNormal = Normalize(OBB.YAxis + OBB.ZAxis);
	pPlane->fDist	= DotProduct(vVert, pPlane->vNormal);

	vVert  = OBB.Center - OBB.ExtY - OBB.ExtZ;
	pPlane = &pBevels->aPlanes[n++];
	pPlane->vNormal = -pBevels->aPlanes[n-2].vNormal;
	pPlane->fDist	= DotProduct(vVert, pPlane->vNormal);

	//	Pair 6 --------
	vVert  = OBB.Center + OBB.ExtY - OBB.ExtZ;	
	pPlane = &pBevels->aPlanes[n++];
	pPlane->vNormal = Normalize(OBB.YAxis - OBB.ZAxis);
	pPlane->fDist	= DotProduct(vVert, pPlane->vNormal);

	vVert  = OBB.Center - OBB.ExtY + OBB.ExtZ;	
	pPlane = &pBevels->aPlanes[n++];
	pPlane->vNormal = -pBevels->aPlanes[n-2].vNormal;
	pPlane->fDist	= DotProduct(vVert, pPlane->vNormal);

End:

	pBevels->iNumPlane = n;

	for (i=0; i < n; i++)
	{
		pBevels->aPlanes[i].MakeSignBits();
		pBevels->aPlaneAddrs[i] = &pBevels->aPlanes[i];
	}
}

/*	Initialize data for AABB trace.

	pInfo: used to receive initalized data.
	vStart: start point of AABB's center
	vExts: AABB's half extents
	vVelocity: AABB's velocity.
	fTime: time.
*/
void TRA_AABBTraceInit(AABBTRACEINFO* pInfo, const A3DVECTOR3& vStart, const A3DVECTOR3& vExts,
					   const A3DVECTOR3& vVelocity, FLOAT fTime, FLOAT fEpsilon/* 0.03125f */)
{
	A3DVECTOR3 vDelta = vVelocity * fTime;

	pInfo->vStart		= vStart;
	pInfo->vExtents		= vExts;
	pInfo->vDelta		= vDelta;
	pInfo->vEnd			= vStart + vDelta;
	pInfo->fTime		= fTime;
	pInfo->fEpsilon		= fEpsilon;
	pInfo->bStartSolid	= false;
	pInfo->bAllSolid	= false;
	pInfo->iClipPlane	= -1;

	//	Calculate bounding box for whole sweep area
	A3DVECTOR3 vDest = pInfo->vEnd;

	//	Extents bound AABB a little is necessary! This ensure AABB won't
	//	be too near to sides in scene.
	pInfo->BoundAABB.Mins = a3d_VecMin(vDest - vExts, vStart - vExts) - 0.5f;
	pInfo->BoundAABB.Maxs = a3d_VecMax(vDest + vExts, vStart + vExts) + 0.5f;
	pInfo->BoundAABB.CompleteCenterExts();

	if (vDelta.x == 0.0f && vDelta.y == 0.0f && vDelta.z == 0.0f)
		pInfo->iType = TRACE_STATIC;
	else
		pInfo->iType = TRACE_MOVING;
}

/*	Collision routine between AABB and a brush

	Return true if collision occurs, otherwise return false.

	pInfo: AABB trace information.
	aPlanes: plane pointer array contains brush's planes
	iNumPlane: number of plane pointer in aPlanes
*/
bool TRA_AABBMoveToBrush(AABBTRACEINFO* pInfo, A3DSPLANE** aPlanes, int iNumPlane)
{
	if (!iNumPlane)
		return false;

	bool bRet;

	if (pInfo->iType == TRACE_STATIC)
		bRet = _StaticAABBToBrush(pInfo, aPlanes, iNumPlane);
	else
		bRet = _MovingAABBToBrush(pInfo, aPlanes, iNumPlane);

	return bRet;
}

/*	Collision routine between moving AABB and static AABB

	Return true if collision occurs, otherwise return false.

	pInfo: AABB trace information.
	aabb: static aabb's information
*/
bool TRA_AABBMoveToAABB(AABBTRACEINFO* pInfo, const A3DAABB& aabb)
{
	//	Do approximate check
	if (!CLS_AABBAABBOverlap(pInfo->BoundAABB.Center, pInfo->BoundAABB.Extents, 
		aabb.Center, aabb.Extents))
		return false;

	A3DSPLANE aPlanes[6];
	A3DSPLANE* aPlaneAddrs[6];

	aPlanes[0].vNormal.Set(1.0f, 0.0f, 0.0f);
	aPlanes[0].fDist = aabb.Maxs.x;
	aPlanes[0].bySignBits = A3DSPLANE::SIGN_P;

	aPlanes[1].vNormal.Set(-1.0f, 0.0f, 0.0f);
	aPlanes[1].fDist = -aabb.Mins.x;
	aPlanes[1].bySignBits = A3DSPLANE::SIGN_NX;

	aPlanes[2].vNormal.Set(0.0f, 1.0f, 0.0f);
	aPlanes[2].fDist = aabb.Maxs.y;
	aPlanes[2].bySignBits = A3DSPLANE::SIGN_P;

	aPlanes[3].vNormal.Set(0.0f, -1.0f, 0.0f);
	aPlanes[3].fDist = -aabb.Mins.y;
	aPlanes[3].bySignBits = A3DSPLANE::SIGN_NY;

	aPlanes[4].vNormal.Set(0.0f, 0.0f, 1.0f);
	aPlanes[4].fDist = aabb.Maxs.z;
	aPlanes[4].bySignBits = A3DSPLANE::SIGN_P;

	aPlanes[5].vNormal.Set(0.0f, 0.0f, -1.0f);
	aPlanes[5].fDist = -aabb.Mins.z;
	aPlanes[5].bySignBits = A3DSPLANE::SIGN_NZ;

	for (int i=0; i < 6; i++)
		aPlaneAddrs[i] = &aPlanes[i];

	return TRA_AABBMoveToBrush(pInfo, aPlaneAddrs, 6);
} 

/*	Collision routine between moving AABB and static OBB

	Return true if collision occurs, otherwise return false.

	pInfo: AABB trace information.
	obb: static obb's information
*/
bool TRA_AABBMoveToOBB(AABBTRACEINFO* pInfo, const A3DOBB& obb)
{
	OBBBEVELS Bevels;
	TRA_BuildOBBBevels(obb, &Bevels);
	return TRA_AABBMoveToBrush(pInfo, (A3DSPLANE**)Bevels.aPlaneAddrs, Bevels.iNumPlane);
}

/*	Collision routine when AABB move to a brush (convex polyhedron)

	Return true if collision occurs, otherwise return false.

	vStart: start position of aabb's center
	vEnd: end position of aabb's ceter
	vExts: aabb's half extents
	aPlanes: plane pointer array contains brush's planes
	iNumPlane: number of plane pointer in aPlanes
	ppClipPlane (out): used to receive clip plane's pointer, this parameter is valid
					   only when 1 is returned.
	pfFraction (out): used to receive moving fraction of aabb.
*/
bool _MovingAABBToBrush(AABBTRACEINFO* pInfo, A3DSPLANE** aPlanes, int iNumPlane)
{
	int iClipPlane = -1;
	float fEnterFrac = -1e7f, fLeaveFrac = 1e7f, fFrac;
	bool bGetOut = false, bStartOut = false;

	pInfo->fFraction	= 1.0f;
	pInfo->bStartSolid	= false;
	pInfo->bAllSolid	= false;

	A3DVECTOR3 vExts(pInfo->vExtents);

	for (int i=0 ; i < iNumPlane; i++)
	{
		const A3DSPLANE* pPlane = aPlanes[i];

		A3DVECTOR3 vOffs;

		//	Push the plane out apropriately for mins/maxs
		switch (pPlane->bySignBits)
		{
		case 0:	vOffs.x = -vExts.x;		vOffs.y = -vExts.y;		vOffs.z = -vExts.z;		break;
		case 1:	vOffs.x = vExts.x;		vOffs.y = -vExts.y;		vOffs.z = -vExts.z;		break;
		case 2:	vOffs.x = -vExts.x;		vOffs.y = vExts.y;		vOffs.z = -vExts.z;		break;
		case 3:	vOffs.x = vExts.x;		vOffs.y = vExts.y;		vOffs.z = -vExts.z;		break;
		case 4:	vOffs.x = -vExts.x;		vOffs.y = -vExts.y;		vOffs.z = vExts.z;		break;
		case 5:	vOffs.x = vExts.x;		vOffs.y = -vExts.y;		vOffs.z = vExts.z;		break;
		case 6:	vOffs.x = -vExts.x;		vOffs.y = vExts.y;		vOffs.z = vExts.z;		break;
		case 7:	vOffs.x = vExts.x;		vOffs.y = vExts.y;		vOffs.z = vExts.z;		break;
		default: 
			return false;
		}
		
		float fDist = pPlane->fDist - DotProduct(vOffs, pPlane->vNormal);
		float d1 = DotProduct(pInfo->vStart, pPlane->vNormal) - fDist;
		float d2 = DotProduct(pInfo->vEnd, pPlane->vNormal) - fDist;

		if (d2 > 0.0f)
			bGetOut = true;		//  Endpoint is not in solid

		if (d1 > 0.0f)
			bStartOut = true;

		//	If completely in front of face, no intersection
		if (d1 > 0.0f && d2 >= d1)
			return false;

		//	If completely behide face, no intersection
		if (d1 <= 0.0f && d2 <= 0.0f)
			continue;

		//	Crosses face
		if (d1 > d2)
		{
			//	Enter
			fFrac = (d1 - pInfo->fEpsilon) / (d1 - d2);
			if (fFrac > fEnterFrac)
			{
				fEnterFrac = fFrac;
				iClipPlane = i;
			}
		}
		else
		{	//	Leave
			fFrac = (d1 + pInfo->fEpsilon) / (d1 - d2);
			if (fFrac < fLeaveFrac)
				fLeaveFrac = fFrac;
		}
	}

	if (!bStartOut)
	{
		//	Ooriginal point was inside brush
		pInfo->bStartSolid = true;

		if (!bGetOut)
			pInfo->bAllSolid = true;

	//	pInfo->fFraction = 0.0f;
	//	pInfo->ClipPlane = *aPlanes[0];	//	This isn't correct, but better than random
	//	return true;
		return false;
	}

	if (fEnterFrac < fLeaveFrac)
	{
	//	if (fEnterFrac > -1.0f && fEnterFrac < 1.0f)
		if (fEnterFrac != -1e7 && fEnterFrac < 1.0f)
		{
			if (fEnterFrac < 0.0f)
				fEnterFrac = 0.0f;

			pInfo->fFraction	= fEnterFrac;
			pInfo->iClipPlane	= iClipPlane;

			ASSERT(iClipPlane >= 0);
			pInfo->ClipPlane.vNormal = aPlanes[iClipPlane]->vNormal;
			pInfo->ClipPlane.fDist	 = aPlanes[iClipPlane]->fDist;

			return true;
		}
	}

	return false;
}

/*	Collision routine of static AABB and brush (convex polyhedron)

	Return true if collision occurs, otherwise return false.

	vStart: start position of aabb's center
	vExts: aabb's half extents
	aPlanes: plane pointer array contains brush's planes
	iNumPlane: number of plane pointer in aPlanes
*/
bool _StaticAABBToBrush(AABBTRACEINFO* pInfo, A3DSPLANE** aPlanes, int iNumPlane)
{
	pInfo->fFraction	= 1.0f;
	pInfo->bStartSolid	= false;
	pInfo->bAllSolid	= false;

	A3DVECTOR3 vExts(pInfo->vExtents);

	for (int i=0; i < iNumPlane; i++)
	{
		const A3DSPLANE* pPlane = aPlanes[i];

		A3DVECTOR3 vOffs;

		//	Push the plane out apropriately for mins/maxs
		switch (pPlane->bySignBits)
		{
		case 0:	vOffs.x = -vExts.x;		vOffs.y = -vExts.y;		vOffs.z = -vExts.z;		break;
		case 1:	vOffs.x = vExts.x;		vOffs.y = -vExts.y;		vOffs.z = -vExts.z;		break;
		case 2:	vOffs.x = -vExts.x;		vOffs.y = vExts.y;		vOffs.z = -vExts.z;		break;
		case 3:	vOffs.x = vExts.x;		vOffs.y = vExts.y;		vOffs.z = -vExts.z;		break;
		case 4:	vOffs.x = -vExts.x;		vOffs.y = -vExts.y;		vOffs.z = vExts.z;		break;
		case 5:	vOffs.x = vExts.x;		vOffs.y = -vExts.y;		vOffs.z = vExts.z;		break;
		case 6:	vOffs.x = -vExts.x;		vOffs.y = vExts.y;		vOffs.z = vExts.z;		break;
		case 7:	vOffs.x = vExts.x;		vOffs.y = vExts.y;		vOffs.z = vExts.z;		break;
		default: return false;
		}
		
		float fDist = pPlane->fDist - DotProduct(vOffs, pPlane->vNormal);
		float d1 = DotProduct(pInfo->vStart, pPlane->vNormal) - fDist;

		//	If completely in front of face, no intersection
		if (d1 > 0.0f)
			return false;
	}

	//	Inside this brush
	pInfo->bStartSolid	= true;
	pInfo->bAllSolid	= true;
	pInfo->fFraction	= 0.0f;

	//	This isn't correct, but better than random
	pInfo->ClipPlane.vNormal = aPlanes[0]->vNormal;
	pInfo->ClipPlane.fDist	 = aPlanes[0]->fDist;
	pInfo->iClipPlane		 = 0;

	return true;
}

void TRA_SphereTraceInit(SPHERETRACEINFO* pInfo, const A3DVECTOR3& vStart, FLOAT fRadius, const A3DVECTOR3& vVelocity, FLOAT fTime, FLOAT fEpsilon/* 0.03125f */)
{
	A3DVECTOR3 vDelta = vVelocity * fTime;

	pInfo->vStart		= vStart;
	pInfo->fRadius		= fRadius;
	pInfo->vDelta		= vDelta;
	pInfo->vEnd			= vStart + vDelta;
	pInfo->fTime		= fTime;
	pInfo->fEpsilon		= fEpsilon;
	pInfo->bStartSolid	= false;
	pInfo->bAllSolid	= false;
	pInfo->iClipPlane	= -1;

	//	Calculate bounding box for whole sweep area
	A3DVECTOR3 vDest(pInfo->vEnd);
	A3DVECTOR3 vExts(fRadius);

	//	Extents bound AABB a little is necessary! This ensure sphere won't
	//	be too near to sides in scene.
	pInfo->BoundAABB.Mins = a3d_VecMin(vDest - vExts, vStart - vExts) - 0.5f;
	pInfo->BoundAABB.Maxs = a3d_VecMax(vDest + vExts, vStart + vExts) + 0.5f;
	pInfo->BoundAABB.CompleteCenterExts();

	if (vDelta.x == 0.0f && vDelta.y == 0.0f && vDelta.z == 0.0f)
		pInfo->iType = TRACE_STATIC;
	else
		pInfo->iType = TRACE_MOVING;
}

bool TRA_SphereMoveToBrush(SPHERETRACEINFO* pInfo, A3DSPLANE** aPlanes, int iNumPlane)
{
	if (!iNumPlane)
		return false;

	bool bRet;

	if (pInfo->iType == TRACE_STATIC)
		bRet = _StaticSphereToBrush(pInfo, aPlanes, iNumPlane);
	else
		bRet = _MovingSphereToBrush(pInfo, aPlanes, iNumPlane);

	return bRet;
}

bool _MovingSphereToBrush(SPHERETRACEINFO* pInfo, A3DSPLANE** aPlanes, int iNumPlane)
{
	int iClipPlane = -1;
	float fEnterFrac = -1e7f, fLeaveFrac = 1e7f, fFrac;
	bool bGetOut = false, bStartOut = false;

	pInfo->fFraction	= 1.0f;
	pInfo->bStartSolid	= false;
	pInfo->bAllSolid	= false;

	for (int i=0 ; i < iNumPlane; i++)
	{
		const A3DSPLANE* pPlane = aPlanes[i];

		//	Push the plane out apropriately
		float d1 = DotProduct(pInfo->vStart, pPlane->vNormal) - pPlane->fDist - pInfo->fRadius;
		float d2 = DotProduct(pInfo->vEnd, pPlane->vNormal) - pPlane->fDist - pInfo->fRadius;

		if (d2 > 0.0f)
			bGetOut = true;		//  Endpoint is not in solid

		if (d1 > 0.0f)
			bStartOut = true;

		//	If completely in front of face, no intersection
		if (d1 > 0.0f && d2 >= d1)
			return 0;

		//	If completely behide face, no intersection
		if (d1 <= 0.0f && d2 <= 0.0f)
			continue;

		//	Crosses face
		if (d1 > d2)
		{
			//	Enter
			fFrac = (d1 - pInfo->fEpsilon) / (d1 - d2);
			if (fFrac > fEnterFrac)
			{
				fEnterFrac = fFrac;
				iClipPlane = i;
			}
		}
		else
		{	//	Leave
			fFrac = (d1 + pInfo->fEpsilon) / (d1 - d2);
			if (fFrac < fLeaveFrac)
				fLeaveFrac = fFrac;
		}
	}

	if (!bStartOut)
	{
		//	Ooriginal point was inside brush
		pInfo->bStartSolid = true;

		if (!bGetOut)
			pInfo->bAllSolid = true;

		//	Note: Return false means that once the sphere intersects with the
		//	brush, it can go through it !!!
		return false;
	}

	if (fEnterFrac < fLeaveFrac)
	{
	//	if (fEnterFrac > -1.0f && fEnterFrac < 1.0f)
		if (fEnterFrac != -1e7 && fEnterFrac < 1.0f)
		{
			if (fEnterFrac < 0.0f)
				fEnterFrac = 0.0f;

			pInfo->fFraction	= fEnterFrac;
			pInfo->iClipPlane	= iClipPlane;

			ASSERT(iClipPlane >= 0);
			pInfo->ClipPlane.vNormal = aPlanes[iClipPlane]->vNormal;
			pInfo->ClipPlane.fDist	 = aPlanes[iClipPlane]->fDist;

			return true;
		}
	}

	return false;
}

bool _StaticSphereToBrush(SPHERETRACEINFO* pInfo, A3DSPLANE** aPlanes, int iNumPlane)
{
	pInfo->fFraction	= 1.0f;
	pInfo->bStartSolid	= false;
	pInfo->bAllSolid	= false;

	for (int i=0; i < iNumPlane; i++)
	{
		const A3DSPLANE* pPlane = aPlanes[i];

		//	Push the plane out apropriately for mins/maxs
		float d = DotProduct(pInfo->vStart, pPlane->vNormal) - pPlane->fDist - pInfo->fRadius;

		//	If completely in front of face, no intersection
		if (d > 0.0f)
			return false;
	}

	//	Inside this brush
	pInfo->bStartSolid	= true;
	pInfo->bAllSolid	= true;
	pInfo->fFraction	= 0.0f;

	//	This isn't correct, but better than random
	pInfo->ClipPlane.vNormal = aPlanes[0]->vNormal;
	pInfo->ClipPlane.fDist	 = aPlanes[0]->fDist;
	pInfo->iClipPlane		 = 0;

	return true;
}

bool TRA_SphereMoveToAABB(SPHERETRACEINFO* pInfo, const A3DAABB& aabb)
{
	//	Do approximate check
	if (!CLS_AABBAABBOverlap(pInfo->BoundAABB.Center, pInfo->BoundAABB.Extents, 
		aabb.Center, aabb.Extents))
		return false;

	A3DSPLANE aPlanes[6];
	A3DSPLANE* aPlaneAddrs[6];

	aPlanes[0].vNormal.Set(1.0f, 0.0f, 0.0f);
	aPlanes[0].fDist = aabb.Maxs.x;

	aPlanes[1].vNormal.Set(-1.0f, 0.0f, 0.0f);
	aPlanes[1].fDist = -aabb.Mins.x;

	aPlanes[2].vNormal.Set(0.0f, 1.0f, 0.0f);
	aPlanes[2].fDist = aabb.Maxs.y;

	aPlanes[3].vNormal.Set(0.0f, -1.0f, 0.0f);
	aPlanes[3].fDist = -aabb.Mins.y;

	aPlanes[4].vNormal.Set(0.0f, 0.0f, 1.0f);
	aPlanes[4].fDist = aabb.Maxs.z;

	aPlanes[5].vNormal.Set(0.0f, 0.0f, -1.0f);
	aPlanes[5].fDist = -aabb.Mins.z;

	for (int i=0; i < 6; i++)
		aPlaneAddrs[i] = &aPlanes[i];

	return TRA_SphereMoveToBrush(pInfo, aPlaneAddrs, 6);
}

bool TRA_SphereMoveToOBB(SPHERETRACEINFO* pInfo, const A3DOBB& obb)
{
	SPHERETRACEINFO Info = *pInfo;

	//	Transform sphere track to OBB space 
	Info.vStart.x = DotProduct(pInfo->vStart, obb.XAxis);
	Info.vStart.y = DotProduct(pInfo->vStart, obb.YAxis);
	Info.vStart.z = DotProduct(pInfo->vStart, obb.ZAxis);

	Info.vEnd.x = DotProduct(pInfo->vEnd, obb.XAxis);
	Info.vEnd.y = DotProduct(pInfo->vEnd, obb.YAxis);
	Info.vEnd.z = DotProduct(pInfo->vEnd, obb.ZAxis);

	//	Build aabb in obb space
	A3DSPLANE aPlanes[6];
	A3DSPLANE* aPlaneAddrs[6];

	aPlanes[0].vNormal.Set(1.0f, 0.0f, 0.0f);
	aPlanes[0].fDist = obb.Extents.x;

	aPlanes[1].vNormal.Set(-1.0f, 0.0f, 0.0f);
	aPlanes[1].fDist = -obb.Extents.x;

	aPlanes[2].vNormal.Set(0.0f, 1.0f, 0.0f);
	aPlanes[2].fDist = obb.Extents.y;

	aPlanes[3].vNormal.Set(0.0f, -1.0f, 0.0f);
	aPlanes[3].fDist = -obb.Extents.y;

	aPlanes[4].vNormal.Set(0.0f, 0.0f, 1.0f);
	aPlanes[4].fDist = obb.Extents.z;

	aPlanes[5].vNormal.Set(0.0f, 0.0f, -1.0f);
	aPlanes[5].fDist = -obb.Extents.z;

	for (int i=0; i < 6; i++)
		aPlaneAddrs[i] = &aPlanes[i];

	if (TRA_SphereMoveToBrush(&Info, aPlaneAddrs, 6))
	{
		pInfo->bStartSolid	= Info.bStartSolid;
		pInfo->bAllSolid	= Info.bAllSolid;
		pInfo->fFraction	= Info.fFraction;
		pInfo->iClipPlane	= Info.iClipPlane;

		pInfo->ClipPlane.vNormal = aPlanes[Info.iClipPlane].vNormal;
		pInfo->ClipPlane.fDist	 = aPlanes[Info.iClipPlane].fDist;

		return true;
	}

	pInfo->fFraction	= 1.0f;
	pInfo->bStartSolid	= false;
	pInfo->bAllSolid	= false;

	return false;
}


