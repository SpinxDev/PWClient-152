/*
 * FILE: A3DCollision.cpp
 *
 * DESCRIPTION: Some common routines for collision detection
 *
 * CREATED BY: duyuxin, 2001/10/13
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "A3DCollision.h"
#include "A3DMacros.h"
#include "AAssist.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

//	Epsilon for collision detection
#define EPSILON_COLLISION	0.0001f
#define	EPSILON_DISTANCE	0.01f

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

static float _HitPointOfOBBAndPolygon(const A3DOBB& OBB, A3DVECTOR3** aVerts, int iNumVert, const A3DPLANE& Plane);

///////////////////////////////////////////////////////////////////////////
//
//	Implement
//
///////////////////////////////////////////////////////////////////////////

/*	Check whether a ray collision with a 3D AABB, if true, calcualte the hit point.

	Return true if ray collision with AABB, otherwise return false.

	vStart: ray's start point.
	vDelta: ray's moving delta (just is vEnd-vStart, not to be normalized)
	vMins, vMaxs: 3D Axis-Aligned Bounding Box
	vPoint (out): receive collision point.
	pfFraction (out): hit fraction.
	vNormal (out): hit plane's normal if true is returned. If start point is laid in
					specified AABB,	vNormal will be (0, 0, 0);
*/
bool CLS_RayToAABB3(const A3DVECTOR3& vStart, const A3DVECTOR3& vDelta, const A3DVECTOR3& vMins,
					const A3DVECTOR3& vMaxs, A3DVECTOR3& vPoint, FLOAT* pfFraction, A3DVECTOR3& vNormal)
{
	A3DVECTOR3 vN, vMaxT(-1.0f, -1.0f, -1.0f);
	bool bInside = true;
	int i;
	float aSign[3];

	//	Search candidate plane
	for (i=0; i < 3; i++)
	{
		if (vStart.m[i] < vMins.m[i])
		{
			vPoint.m[i]	= vMins.m[i];
			bInside		= false;
			aSign[i]	= 1.0f;

			//	Calcualte T distance to candidate plane
			if (vDelta.m[i] != 0.0f)
				vMaxT.m[i] = (vMins.m[i] - vStart.m[i]) / vDelta.m[i];
		}
		else if (vStart.m[i] > vMaxs.m[i])
		{
			vPoint.m[i]	= vMaxs.m[i];
			bInside		= false;
			aSign[i]	= -1.0f;

			//	Calcualte T distance to candidate plane
			if (vDelta.m[i] != 0.0f)
				vMaxT.m[i] = (vMaxs.m[i] - vStart.m[i]) / vDelta.m[i];
		}
	}

	if (bInside)
	{
		vPoint		= vStart;
		*pfFraction = 0;
		vNormal		= A3DVECTOR3(0.0f);
		return true;
	}

	//	Get largest of the maxT's for final choice of intersection
	int iWhichPlane = 0;
	vN = A3DVECTOR3(-aSign[0], 0.0f, 0.0f);

	if (vMaxT.m[1] > vMaxT.m[iWhichPlane])
	{
		iWhichPlane = 1;
		vN = A3DVECTOR3(0.0f, -aSign[1], 0.0f);
	}

	if (vMaxT.m[2] > vMaxT.m[iWhichPlane])
	{
		iWhichPlane = 2;
		vN = A3DVECTOR3(0.0f, 0.0f, -aSign[2]);
	}

	//	Check final candidate actually inside box
	if (vMaxT.m[iWhichPlane] < 0)
		return false;

	for (i=0; i < 3; i++)
	{
		if (i != iWhichPlane)
		{
			vPoint.m[i] = vStart.m[i] + vMaxT.m[iWhichPlane] * vDelta.m[i];

			if (vPoint.m[i] < vMins.m[i] - EPSILON_COLLISION ||
				vPoint.m[i] > vMaxs.m[i] + EPSILON_COLLISION)
				return false;
		}
	}

	*pfFraction = vMaxT.m[iWhichPlane];
	vNormal		= vN;

	return true;
}

/*	Check whether a ray collision with a 2D AABB, if true, calcualte the hit point.

	Return true if ray collision with AABB, otherwise return false.

	vStart: ray's start point, only the first two components are valid.
	vDelta: ray's moving delta (just is vEnd-vStart, not to be normalized), only the first two
		  components are valid.
	vMins, vMaxs: Axis-Aligned Bounding Box
	vPoint (out): receive collision point, only the first two components are valid
*/
bool CLS_RayToAABB2(const A3DVECTOR3& vStart, const A3DVECTOR3& vDelta, FLOAT vMins[2],
					FLOAT vMaxs[2], A3DVECTOR3& vPoint)
{
	float vMaxT[2] = {-1, -1};
	bool bInside = true;
	int i;

	//	Search candidate side
	for (i=0; i < 2; i++)
	{
		if (vStart.m[i] < vMins[i])
		{
			vPoint.m[i]	= vMins[i];
			bInside		= false;

			//	Calcualte T distance to candidate plane
			if (vDelta.m[i] != 0.0f)
				vMaxT[i] = (vMins[i] - vStart.m[i]) / vDelta.m[i];
		}
		else if (vStart.m[i] > vMaxs[i])
		{
			vPoint.m[i]	= vMaxs[i];
			bInside		= false;

			//	Calcualte T distance to candidate plane
			if (vDelta.m[i] != 0.0f)
				vMaxT[i] = (vMaxs[i] - vStart.m[i]) / vDelta.m[i];
		}
	}

	if (bInside)
	{
		vPoint = vStart;
		return true;
	}

	//	Get largest of the maxT's for final choice of intersection
	if (vMaxT[0] > vMaxT[1])
	{
		//	Check final candidate actually inside box
		if (vMaxT[0] < 0)
			return false;

		vPoint.m[1] = vStart.m[1] + vMaxT[0] * vDelta.m[1];

		if (vPoint.m[1] < vMins[1] - EPSILON_COLLISION ||
			vPoint.m[1] > vMaxs[1] + EPSILON_COLLISION)
			return false;
	}
	else	//	vMaxT[0] <= vMaxT[1]
	{
			//	Check final candidate actually inside box
		if (vMaxT[1] < 0)
			return false;

		vPoint.m[0] = vStart.m[0] + vMaxT[1] * vDelta.m[0];

		if (vPoint.m[0] < vMins[0] - EPSILON_COLLISION ||
			vPoint.m[0] > vMaxs[0] + EPSILON_COLLISION)
			return false;
	}

	return true;
}

/*	Do collision detection between specified ray segment and 3D OBB.

	Return ture if ray segment hit OBB, otherwise return false.

	vStart: start point of ray segment
	vDelta: ray's direction (is just vEnd-vStart, not be normalized)
	OBB: 3D Oriented Bounding Box.
	vPoint (out): used to receive hit position if collision occured.
	pfFraction (out): hit fraction.
	vNormal (out): hit plane's normal
*/
bool CLS_RayToOBB3(const A3DVECTOR3& vStart, const A3DVECTOR3& vDelta, const A3DOBB& OBB, 
				   A3DVECTOR3& vPoint, FLOAT* pfFraction, A3DVECTOR3& vNormal)
{
	float t1, t2, fExt, fDelta;
	float fMinT = -99999, fMaxT = 99999;
	A3DVECTOR3 vDist = OBB.Center - vStart;
	A3DVECTOR3 vN1, vN2;

	//	Planes speak to x axis
	fExt	= DotProduct(OBB.XAxis, vDist);
	fDelta	= DotProduct(OBB.XAxis, vDelta);

	if (fDelta < -EPSILON_COLLISION || fDelta > EPSILON_COLLISION)
	{
		fDelta = 1.0f / fDelta;
		t1 = (fExt + OBB.Extents.x) * fDelta;
		t2 = (fExt - OBB.Extents.x) * fDelta;

		if (t1 < t2)
		{
			if (t1 > fMinT)	{	fMinT = t1;	vN1 = OBB.XAxis;	}
			if (t2 < fMaxT)	{	fMaxT = t2; vN2 = -OBB.XAxis;	}
		}
		else
		{
			if (t2 > fMinT)	{	fMinT = t2; vN1 = -OBB.XAxis;	}
			if (t1 < fMaxT)	{	fMaxT = t1; vN2 = OBB.XAxis;	}
		}

		if (fMinT > fMaxT)		return false;
		if (fMinT > 1)			return false;
		if (fMaxT < 0)			return false;
	}
	else
	{
		if (fExt < -OBB.Extents.x || fExt > OBB.Extents.x)
			return false;
	}

	//	Planes speak to y axis
	fExt	= DotProduct(OBB.YAxis, vDist);
	fDelta	= DotProduct(OBB.YAxis, vDelta);

	if (fDelta < -EPSILON_COLLISION || fDelta > EPSILON_COLLISION)
	{
		fDelta = 1.0f / fDelta;
		t1 = (fExt + OBB.Extents.y) * fDelta;
		t2 = (fExt - OBB.Extents.y) * fDelta;

		if (t1 < t2)
		{
			if (t1 > fMinT)	{	fMinT = t1;	vN1 = OBB.YAxis;	}
			if (t2 < fMaxT)	{	fMaxT = t2; vN2 = -OBB.YAxis;	}
		}
		else
		{
			if (t2 > fMinT)	{	fMinT = t2; vN1 = -OBB.YAxis;	}
			if (t1 < fMaxT)	{	fMaxT = t1; vN2 = OBB.YAxis;	}
		}

		if (fMinT > fMaxT)		return false;
		if (fMinT > 1)			return false;
		if (fMaxT < 0)			return false;
	}
	else
	{
		if (fExt < -OBB.Extents.y || fExt > OBB.Extents.y)
			return false;
	}

	//	Planes speak to z axis
	fExt	= DotProduct(OBB.ZAxis, vDist);
	fDelta	= DotProduct(OBB.ZAxis, vDelta);

	if (fDelta < -EPSILON_COLLISION || fDelta > EPSILON_COLLISION)
	{
		fDelta = 1.0f / fDelta;
		t1 = (fExt + OBB.Extents.z) * fDelta;
		t2 = (fExt - OBB.Extents.z) * fDelta;

		if (t1 < t2)
		{
			if (t1 > fMinT)	{	fMinT = t1; vN1 = OBB.ZAxis;	}
			if (t2 < fMaxT)	{	fMaxT = t2; vN2 = -OBB.ZAxis;	}
		}
		else
		{
			if (t2 > fMinT)	{	fMinT = t2;	vN1 = -OBB.ZAxis;	}
			if (t1 < fMaxT)	{	fMaxT = t1; vN2 = OBB.ZAxis;	}
		}

		if (fMinT > fMaxT)		return false;
		if (fMinT > 1)			return false;
		if (fMaxT < 0)			return false;
	}
	else
	{
		if (fExt < -OBB.Extents.z || fExt > OBB.Extents.z)
			return false;
	}

	//	Calcualte hit point
	if (fMinT > 0)
	{
		*pfFraction = fMinT;
		vPoint	= vStart + vDelta * fMinT;
		vNormal	= vN1;
	}
	else
	{
		*pfFraction = fMaxT;
		vPoint	= vStart + vDelta * fMaxT;
		vNormal	= vN2;
	}

	return true;
}

/*	Check whether a plane collision with a AABB.

	Return value:
		
		-1: aabb is behide of plane
		0: aabb cross with plane
		1: aabb is in front of plane

	vNormal: plane's normal.
	fDist: d parameter of plane equation
	vOrigin: AABB's center
	vExtents: AABB's half length of sides
*/
int CLS_PlaneToAABB(const A3DVECTOR3& vNormal, FLOAT fDist, const A3DVECTOR3& vOrigin,
					const A3DVECTOR3& vExtents)
{
	A3DVECTOR3 vMins, vMaxs;

	if (vNormal.x > 0)
	{
		vMins.x = vOrigin.x - vExtents.x;
		vMaxs.x = vOrigin.x + vExtents.x;
	}
	else
	{
		vMins.x = vOrigin.x + vExtents.x;
		vMaxs.x = vOrigin.x - vExtents.x;
	}

	if (vNormal.y > 0)
	{
		vMins.y = vOrigin.y - vExtents.y;
		vMaxs.y = vOrigin.y + vExtents.y;
	}
	else
	{
		vMins.y = vOrigin.y + vExtents.y;
		vMaxs.y = vOrigin.y - vExtents.y;
	}

	if (vNormal.z > 0)
	{
		vMins.z = vOrigin.z - vExtents.z;
		vMaxs.z = vOrigin.z + vExtents.z;
	}
	else
	{
		vMins.z = vOrigin.z + vExtents.z;
		vMaxs.z = vOrigin.z - vExtents.z;
	}

	if (DotProduct(vNormal, vMaxs) - fDist < EPSILON_COLLISION)
		return -1;

	if (DotProduct(vNormal, vMins) - fDist > -EPSILON_COLLISION)
		return 1;

	return 0;
}

/*	Check whether a plane collision with a AABB.

	Return value:
		
		-1: aabb is behide of plane
		0: aabb cross with plane
		1: aabb is in front of plane

	Plane: plane object, type and signbits will be used
	_vMins, _vMaxs: Axis-aligned bounding box
*/
int CLS_PlaneAABBOverlap(const A3DSPLANE& Plane, const A3DVECTOR3& _vMins, const A3DVECTOR3& _vMaxs)
{
	float d1, d2;
	const float* vNormal = Plane.vNormal.m;
	const float* vMaxs	 = _vMaxs.m;
	const float* vMins	 = _vMins.m;

	//	Fast axial cases
	if (Plane.byType < 3)
	{
		if (Plane.fDist < vMins[Plane.byType])
			return 1;
		else if (Plane.fDist > vMaxs[Plane.byType])
			return -1;
		else
			return 0;
	}
	else if (Plane.byType < 6)
	{
		if (Plane.fDist < -vMaxs[Plane.byType-3])
			return 1;
		else if (Plane.fDist > -vMins[Plane.byType-3])
			return -1;
		else
			return 0;
	}

	//	General case
	switch (Plane.bySignBits)
	{
	case 0:

		d1 = vNormal[0]*vMaxs[0] + vNormal[1]*vMaxs[1] + vNormal[2]*vMaxs[2];
		d2 = vNormal[0]*vMins[0] + vNormal[1]*vMins[1] + vNormal[2]*vMins[2];
		break;

	case 1:

		d1 = vNormal[0]*vMins[0] + vNormal[1]*vMaxs[1] + vNormal[2]*vMaxs[2];
		d2 = vNormal[0]*vMaxs[0] + vNormal[1]*vMins[1] + vNormal[2]*vMins[2];
		break;

	case 2:

		d1 = vNormal[0]*vMaxs[0] + vNormal[1]*vMins[1] + vNormal[2]*vMaxs[2];
		d2 = vNormal[0]*vMins[0] + vNormal[1]*vMaxs[1] + vNormal[2]*vMins[2];
		break;

	case 3:

		d1 = vNormal[0]*vMins[0] + vNormal[1]*vMins[1] + vNormal[2]*vMaxs[2];
		d2 = vNormal[0]*vMaxs[0] + vNormal[1]*vMaxs[1] + vNormal[2]*vMins[2];
		break;

	case 4:

		d1 = vNormal[0]*vMaxs[0] + vNormal[1]*vMaxs[1] + vNormal[2]*vMins[2];
		d2 = vNormal[0]*vMins[0] + vNormal[1]*vMins[1] + vNormal[2]*vMaxs[2];
		break;

	case 5:

		d1 = vNormal[0]*vMins[0] + vNormal[1]*vMaxs[1] + vNormal[2]*vMins[2];
		d2 = vNormal[0]*vMaxs[0] + vNormal[1]*vMins[1] + vNormal[2]*vMaxs[2];
		break;

	case 6:

		d1 = vNormal[0]*vMaxs[0] + vNormal[1]*vMins[1] + vNormal[2]*vMins[2];
		d2 = vNormal[0]*vMins[0] + vNormal[1]*vMaxs[1] + vNormal[2]*vMaxs[2];
		break;

	case 7:
		
		d1 = vNormal[0]*vMins[0] + vNormal[1]*vMins[1] + vNormal[2]*vMins[2];
		d2 = vNormal[0]*vMaxs[0] + vNormal[1]*vMaxs[1] + vNormal[2]*vMaxs[2];
		break;

	default:

		d1 = d2 = 0;		//	Shut up compiler
		break;
	}

	if (d1 - Plane.fDist < EPSILON_COLLISION)
		return -1;

	if (d2 - Plane.fDist > -EPSILON_COLLISION)
		return 1;

	return 0;

/*	A3DVECTOR3 aVerts[8];

	aVerts[0].Set(_vMins.x, _vMins.y, _vMins.z);
	aVerts[1].Set(_vMins.x, _vMins.y, _vMaxs.z);
	aVerts[2].Set(_vMaxs.x, _vMins.y, _vMins.z);
	aVerts[3].Set(_vMaxs.x, _vMins.y, _vMaxs.z);

	aVerts[4].Set(_vMins.x, _vMaxs.y, _vMins.z);
	aVerts[5].Set(_vMins.x, _vMaxs.y, _vMaxs.z);
	aVerts[6].Set(_vMaxs.x, _vMaxs.y, _vMins.z);
	aVerts[7].Set(_vMaxs.x, _vMaxs.y, _vMaxs.z);

	int iNumFront=0, iNumBack=0;

	for (int i=0; i < 8; i++)
	{
		float fDot = DotProduct(Plane.vNormal, aVerts[i]);
		if (fDot > Plane.fDist)
			iNumFront++;
		else if (fDot < Plane.fDist)
			iNumBack++;
	}

	if (iNumFront == 8)
		return 1;
	else if (iNumBack == 8)
		return -1;
	else
		return 0;
*/
}

/*
__declspec( naked ) int CLS_PlaneAABBOverlap(const A3DPLANE& Plane, const A3DVECTOR3& _vMins,
										const A3DVECTOR3& _vMaxs)
{
	static int iInitialized = 0;
	static int aJmpTab[8];

	__asm
	{
		//	Only eax, ecx, edx can be used freely
		push	ebx

		test	iInitialized, 1
		jnz		Initialize
		mov		iInitialized, 1

		mov		aJmpTab[0*4], offset Case0
		mov		aJmpTab[1*4], offset Case1
		mov		aJmpTab[2*4], offset Case2
		mov		aJmpTab[3*4], offset Case3
		mov		aJmpTab[4*4], offset Case4
		mov		aJmpTab[5*4], offset Case5
		mov		aJmpTab[6*4], offset Case6
		mov		aJmpTab[7*4], offset Case7

Initialize:

		mov		edx, dword ptr [esp+8]		//	Address of Plane
		xor		eax, eax
		mov		ecx, dword ptr [esp+12]		//	Address of _vMins
		mov		al, byte ptr [edx+17]		//	Get Plane.bySignBits to al
		mov		ebx, dword ptr [esp+16]		//	Address of _vMaxs
		cmp		al, 8
		jge		Error

		fld		dword ptr[edx]
		fld		st(0)
		jmp		dword ptr[aJmpTab+eax*4]

Case0:	fmul	dword ptr[ebx]
		fld		dword ptr[edx+4]
		fxch	st(2)
		fmul	dword ptr[ecx]
		fxch	st(2)
		fld		st(0)
		fmul	dword ptr[ebx+4]
		fld		dword ptr[edx+8]
		fxch	st(2)
		fmul	dword ptr[ecx+4]
		fxch	st(2)
		fld		st(0)
		fmul	dword ptr[ebx+8]
		fxch	st(5)
		faddp	st(3), st(0)
		fmul	dword ptr[ecx+8]
		fxch	st(1)
		faddp	st(3), st(0)
		fxch	st(3)
		faddp	st(2), st(0)
		jmp		SetSides

Case1:	fmul	dword ptr[ecx]
		fld		dword ptr[edx+4]
		fxch	st(2)
		fmul	dword ptr[ebx]
		fxch	st(2)
		fld		st(0)
		fmul	dword ptr[ebx+4]
		fld		dword ptr[edx+8]
		fxch	st(2)
		fmul	dword ptr[ecx+4]
		fxch	st(2)
		fld		st(0)
		fmul	dword ptr[ebx+8]
		fxch	st(5)
		faddp	st(3), st(0)
		fmul	dword ptr[ecx+8]
		fxch	st(1)
		faddp	st(3), st(0)
		fxch	st(3)
		faddp	st(2), st(0)
		jmp		SetSides

Case2:	fmul	dword ptr[ebx]
		fld		dword ptr[edx+4]
		fxch	st(2)
		fmul	dword ptr[ecx]
		fxch	st(2)
		fld		st(0)
		fmul	dword ptr[ecx+4]
		fld		dword ptr[edx+8]
		fxch	st(2)
		fmul	dword ptr[ebx+4]
		fxch	st(2)
		fld		st(0)
		fmul	dword ptr[ebx+8]
		fxch	st(5)
		faddp	st(3), st(0)
		fmul	dword ptr[ecx+8]
		fxch	st(1)
		faddp	st(3), st(0)
		fxch	st(3)
		faddp	st(2), st(0)
		jmp		SetSides

Case3:	fmul	dword ptr[ecx]
		fld		dword ptr[edx+4]
		fxch	st(2)
		fmul	dword ptr[ebx]
		fxch	st(2)
		fld		st(0)
		fmul	dword ptr[ecx+4]
		fld		dword ptr[edx+8]
		fxch	st(2)
		fmul	dword ptr[ebx+4]
		fxch	st(2)
		fld		st(0)
		fmul	dword ptr[ebx+8]
		fxch	st(5)
		faddp	st(3), st(0)
		fmul	dword ptr[ecx+8]
		fxch	st(1)
		faddp	st(3), st(0)
		fxch	st(3)
		faddp	st(2), st(0)
		jmp		SetSides

Case4:	fmul	dword ptr[ebx]
		fld		dword ptr[edx+4]
		fxch	st(2)
		fmul	dword ptr[ecx]
		fxch	st(2)
		fld		st(0)
		fmul	dword ptr[ebx+4]
		fld		dword ptr[edx+8]
		fxch	st(2)
		fmul	dword ptr[ecx+4]
		fxch	st(2)
		fld		st(0)
		fmul	dword ptr[ecx+8]
		fxch	st(5)
		faddp	st(3), st(0)
		fmul	dword ptr[ebx+8]
		fxch	st(1)
		faddp	st(3), st(0)
		fxch	st(3)
		faddp	st(2), st(0)
		jmp		SetSides

Case5:	fmul	dword ptr[ecx]
		fld		dword ptr[edx+4]
		fxch	st(2)
		fmul	dword ptr[ebx]
		fxch	st(2)
		fld		st(0)
		fmul	dword ptr[ebx+4]
		fld		dword ptr[edx+8]
		fxch	st(2)
		fmul	dword ptr[ecx+4]
		fxch	st(2)
		fld		st(0)
		fmul	dword ptr[ecx+8]
		fxch	st(5)
		faddp	st(3), st(0)
		fmul	dword ptr[ebx+8]
		fxch	st(1)
		faddp	st(3), st(0)
		fxch	st(3)
		faddp	st(2), st(0)
		jmp		SetSides

Case6:	fmul	dword ptr[ebx]
		fld		dword ptr[edx+4]
		fxch	st(2)
		fmul	dword ptr[ecx]
		fxch	st(2)
		fld		st(0)
		fmul	dword ptr[ecx+4]
		fld		dword ptr[edx+8]
		fxch	st(2)
		fmul	dword ptr[ebx+4]
		fxch	st(2)
		fld		st(0)
		fmul	dword ptr[ecx+8]
		fxch	st(5)
		faddp	st(3), st(0)
		fmul	dword ptr[ebx+8]
		fxch	st(1)
		faddp	st(3), st(0)
		fxch	st(3)
		faddp	st(2), st(0)
		jmp		SetSides

Case7:	fmul	dword ptr[ecx]
		fld		dword ptr[edx+4]
		fxch	st(2)
		fmul	dword ptr[ebx]
		fxch	st(2)
		fld		st(0)
		fmul	dword ptr[ecx+4]
		fld		dword ptr[edx+8]
		fxch	st(2)
		fmul	dword ptr[ebx+4]
		fxch	st(2)
		fld		st(0)
		fmul	dword ptr[ecx+8]
		fxch	st(5)
		faddp	st(3), st(0)
		fmul	dword ptr[ebx+8]
		fxch	st(1)
		faddp	st(3), st(0)
		fxch	st(3)
		faddp	st(2), st(0)

SetSides:

		faddp	st(2), st(0)
		fcomp	dword ptr[edx+12]
		fnstsw	ax
		test	ah, 1
		jnz		Back			//	d1 < Plane.fDist

		fcomp	dword ptr[edx+12]
		fnstsw	ax
		test	ah, 1
		jz		Front			//	d2 > Plane.fDist

		pop		ebx
		mov		eax, 0
		ret

Back:	faddp	st(0), st(0)	//	Pop st(0)
		pop		ebx
		mov		eax, -1
		ret

Front:	pop		ebx
		mov		eax, 1
		ret

Error:	int 3
	}
}
*/
/*	Check whether a plane collision with a sphere.

	Return value:
		
		-1: sphere is behide of plane
		0: sphere cross with plane
		1: sphere is in front of plane

	Plane: plane object, type and signbits will be used
	vCenter: sphere's center
	fRadius: sphere's radius
*/
int CLS_PlaneSphereOverlap(const A3DSPLANE& Plane, const A3DVECTOR3& vCenter, FLOAT fRadius)
{
	float fDelta;

	//	Fast axial cases
	if (Plane.byType < 3)
	{
		fDelta = vCenter.m[Plane.byType] - Plane.fDist;
		if (fDelta > fRadius)
			return 1;
		else if (fDelta < -fRadius)
			return -1;
		else
			return 0;
	}
	else if (Plane.byType < 6)
	{
		fDelta = vCenter.m[Plane.byType-3] + Plane.fDist;
		if (fDelta < -fRadius)
			return 1;
		else if (fDelta > fRadius)
			return -1;
		else
			return 0;
	}

	fDelta = DotProduct(vCenter, Plane.vNormal) - Plane.fDist;

	if (fDelta > fRadius)
		return 1;
	else if (fDelta < -fRadius)
		return -1;
	else
		return 0;
}

/*	Check whether a triangle collision with a AABB. This function bases
	separating axis theorem which is brought by S.Gottschalk, M.C.Lin and D.Manocha
	in their paper "OBBTree: A Hierarchical Structure for Rapid Interference Detection".

	Return true if they collision, otherwise return false.

	_v0, _v1, _v2: triangle's vertice.
	Plane: plane on which triangle lies
	AABB: Axis-Aligned Bounding Box. all components will be used
*/
bool CLS_TriangleAABBOverlap(const A3DVECTOR3& _v0, const A3DVECTOR3& _v1, const A3DVECTOR3& _v2, 
						const A3DSPLANE& Plane, const A3DAABB& AABB)
{
	//	First translate triangle into AABB's coordinates
	A3DVECTOR3 v0(_v0 - AABB.Center);
	A3DVECTOR3 v1(_v1 - AABB.Center);
	A3DVECTOR3 v2(_v2 - AABB.Center);

	//	Follow separating axis theorem we should do 12 axis test (x, y, z and corss
	//	product of each triangle edge with x, y, z. But we use AABB, so x, y, z test
	//	are replaced by below codes
	float fMin, fMax;
	
	fMin = min3(v0.x, v1.x, v2.x);
	fMax = max3(v0.x, v1.x, v2.x);
	if (fMin > AABB.Extents.x || fMax < -AABB.Extents.x)
		return false;

	fMin = min3(v0.y, v1.y, v2.y);
	fMax = max3(v0.y, v1.y, v2.y);
	if (fMin > AABB.Extents.y || fMax < -AABB.Extents.y)
		return false;

	fMin = min3(v0.z, v1.z, v2.z);
	fMax = max3(v0.z, v1.z, v2.z);
	if (fMin > AABB.Extents.z || fMax < -AABB.Extents.z)
		return false;

	//	Then whether AABB overlap with triangle plane
	if (CLS_PlaneAABBOverlap(Plane, AABB.Mins, AABB.Maxs))
		return false;

	//	Now, test these 9 axis made from cross product of triangle's edge and x, y, z
	A3DVECTOR3 vEdge;
	float p0, p1, fRadius;
	float fex, fey, fez;	//	Absolutes of edge

	vEdge = v1 - v0;
	fex	= vEdge.x >= 0 ? vEdge.x : -vEdge.x;
	fey	= vEdge.y >= 0 ? vEdge.y : -vEdge.y;
	fez	= vEdge.z >= 0 ? vEdge.z : -vEdge.z;

	//	edge v0-v1 corss product with x
	p0 = vEdge.z * v0.y - vEdge.y * v0.z;
	p1 = vEdge.z * v2.y - vEdge.y * v2.z;
	fRadius = fez * AABB.Extents.y + fey * AABB.Extents.z;
	if (p0 < p1)
	{
		if (p0 > fRadius || p1 < -fRadius)
			return false;
	}
	else if (p1 > fRadius || p0 < -fRadius)
		return false;

	//	edge v0-v1 corss product with y
	p0 = -vEdge.z * v0.x + vEdge.x * v0.z;
	p1 = -vEdge.z * v2.x + vEdge.x * v2.z;
	fRadius = fez * AABB.Extents.x + fex * AABB.Extents.z;
	if (p0 < p1)
	{
		if (p0 > fRadius || p1 < -fRadius)
			return false;
	}
	else if (p1 > fRadius || p0 < -fRadius)
		return false;

	//	edge v0-v1 corss product with z
	p0 = vEdge.y * v1.x - vEdge.x * v1.y;
	p1 = vEdge.y * v2.x - vEdge.x * v2.y;
	fRadius = fey * AABB.Extents.x + fex * AABB.Extents.y;
	if (p0 < p1)
	{
		if (p0 > fRadius || p1 < -fRadius)
			return false;
	}
	else if (p1 > fRadius || p0 < -fRadius)
		return false;

	vEdge = v2 - v1;
	fex	= vEdge.x >= 0 ? vEdge.x : -vEdge.x;
	fey	= vEdge.y >= 0 ? vEdge.y : -vEdge.y;
	fez	= vEdge.z >= 0 ? vEdge.z : -vEdge.z;

	//	edge v1-v2 corss product with x
	p0 = vEdge.z * v0.y - vEdge.y * v0.z;
	p1 = vEdge.z * v2.y - vEdge.y * v2.z;
	fRadius = fez * AABB.Extents.y + fey * AABB.Extents.z;
	if (p0 < p1)
	{
		if (p0 > fRadius || p1 < -fRadius)
			return false;
	}
	else if (p1 > fRadius || p0 < -fRadius)
		return false;

	//	edge v1-v2 corss product with y
	p0 = -vEdge.z * v0.x + vEdge.x * v0.z;
	p1 = -vEdge.z * v2.x + vEdge.x * v2.z;
	fRadius = fez * AABB.Extents.x + fex * AABB.Extents.z;
	if (p0 < p1)
	{
		if (p0 > fRadius || p1 < -fRadius)
			return false;
	}
	else if (p1 > fRadius || p0 < -fRadius)
		return false;

	//	edge v1-v2 corss product with z
	p0 = vEdge.y * v0.x - vEdge.x * v0.y;
	p1 = vEdge.y * v1.x - vEdge.x * v1.y;
	fRadius = fey * AABB.Extents.x + fex * AABB.Extents.y;
	if (p0 < p1)
	{
		if (p0 > fRadius || p1 < -fRadius)
			return false;
	}
	else if (p1 > fRadius || p0 < -fRadius)
		return false;

	vEdge = v0 - v2;
	fex	= vEdge.x >= 0 ? vEdge.x : -vEdge.x;
	fey	= vEdge.y >= 0 ? vEdge.y : -vEdge.y;
	fez	= vEdge.z >= 0 ? vEdge.z : -vEdge.z;

	//	edge v2-v0 corss product with x
	p0 = vEdge.z * v0.y - vEdge.y * v0.z;
	p1 = vEdge.z * v1.y - vEdge.y * v1.z;
	fRadius = fez * AABB.Extents.y + fey * AABB.Extents.z;
	if (p0 < p1)
	{
		if (p0 > fRadius || p1 < -fRadius)
			return false;
	}
	else if (p1 > fRadius || p0 < -fRadius)
		return false;

	//	edge v2-v0 corss product with y
	p0 = -vEdge.z * v0.x + vEdge.x * v0.z;
	p1 = -vEdge.z * v1.x + vEdge.x * v1.z;
	fRadius = fez * AABB.Extents.x + fex * AABB.Extents.z;
	if (p0 < p1)
	{
		if (p0 > fRadius || p1 < -fRadius)
			return false;
	}
	else if (p1 > fRadius || p0 < -fRadius)
		return false;

	//	edge v2-v0 corss product with z
	p0 = vEdge.y * v1.x - vEdge.x * v1.y;
	p1 = vEdge.y * v2.x - vEdge.x * v2.y;
	fRadius = fey * AABB.Extents.x + fex * AABB.Extents.y;
	if (p0 < p1)
	{
		if (p0 > fRadius || p1 < -fRadius)
			return false;
	}
	else if (p1 > fRadius || p0 < -fRadius)
		return false;

	return true;
}

/*	Check whether ray intersect with triangle using the algorithm introduced by
	Tomas Toller and Ben Thumbore in "Fast, Minimum Storage Ray/Triangle Intersection".
	As they said, this algorithm is the fartest ray/triangle routine for triangles which
	havn't precomputed plane equations.

	vStart: start point of ray
	vDelta: ray's moving delta (needn't to be normalized)
	v0, v1, v2: three vertice of triangle
	vPoint (out): used to receive hit point.
	b2Sides: true, triangle is two sides, false, cull triangle which is back to ray
	pfFraction (out): hit fraction, can be NULL
*/
bool CLS_RayToTriangle(const A3DVECTOR3& vStart, const A3DVECTOR3& vDelta, const A3DVECTOR3& v0, 
					   const A3DVECTOR3& v1, const A3DVECTOR3& v2, A3DVECTOR3& vPoint, bool b2Sides, float* pfFraction/* NULL */)
{
	float u, v, t, fErr = 0.00001f;

	A3DVECTOR3 vDir = vDelta;
	float fDist = vDir.Normalize();

	//	Find vectors for two edges sharing vert0
	A3DVECTOR3 vEdge1 = v1 - v0;
	A3DVECTOR3 vEdge2 = v2 - v0;

	//	Begin calculating determinant - also used to calculate U parameter
	A3DVECTOR3 tvec, qvec;
	A3DVECTOR3 pvec = CrossProduct(vDir, vEdge2);

	//	If determinant is near zero, ray lies in plane of triangle
	float fDet = DotProduct(vEdge1, pvec);

/*	if (0)
	{
		//	Ben Thumbore's original algorithm
		if (!b2Sides)
		{
			if (fDet < fErr)
				return false;
			
			//	Calculate distance from vert0 to ray origin
			tvec = vStart - v0;

			//	Calculate U parameter and test bounds
			u = DotProduct(tvec, pvec);
			if (u < -fErr || u > fDet + fErr)
				return false;

			//	Prepare to test V parameter
			qvec = CrossProduct(tvec, vEdge1);

			//	Calculate V parameter and test bounds
			v = DotProduct(vDir, qvec);
			if (v < -fErr || u + v > fDet + fErr)
				return false;

			//	Calculate t, the distance to trangle from vStart
			float fInvDet = 1.0f / fDet;
			t = DotProduct(vEdge2, qvec) * fInvDet;
		}
		else
		{
			if (fDet < fErr && fDet > -fErr)
				return false;

			float fInvDet = 1.0f / fDet;

			//	Calculate distance from vert0 to ray origin
			tvec = vStart - v0;

			u = DotProduct(tvec, pvec) * fInvDet;
			if (u < -fErr || u > 1.0f + fErr)
				return false;

			//	Prepare to test V parameter
			qvec = CrossProduct(tvec, vEdge1);

			//	Calculate V parameter and test bounds
			v = DotProduct(vDir, qvec) * fInvDet;
			if (v < -fErr || u + v > 1.0f + fErr)
				return false;

			//	Calculate t, the distance to trangle from vStart
			t = DotProduct(vEdge2, qvec) * fInvDet;
		}
	}
	else	*/
	{
		//	Our little changed version
		if (!b2Sides)
		{
			if (fDet < fErr)
				return false;
		}	
		else
		{
			if (fDet < fErr && fDet > -fErr)
				return false;	
		}

		//	Calculate distance from vert0 to ray origin
		tvec = vStart - v0;

		//	Calculate U parameter and test bounds
		u = DotProduct(tvec, pvec);
		if (u < -fErr || u > fDet + fErr)
			return false;

		//	Prepare to test V parameter
		qvec = CrossProduct(tvec, vEdge1);

		//	Calculate V parameter and test bounds
		v = DotProduct(vDir, qvec);
		if (v < -fErr || u + v > fDet + fErr)
			return false;

		//	Calculate t, the distance to trangle from vStart
		t = DotProduct(vEdge2, qvec) / fDet;
	}

	//	Ben Thumbore's algorithm check the insection of a unlimited ray and triangle.
	//	t means the distance from vStart position to trangle. But what we want to test
	//	here is the intersection of a line segment (NOT a whole ray) and triangle.
	//	so ignore the case when t < 0.0 or t > fDist
	float fErr2 = 0.001f;
	if (t < -fErr2 || t > fDist)
		return false;

	a_ClampFloor(t, fErr2);
	float fFraction = (t - fErr2) / fDist;
	vPoint = vStart + vDelta * fFraction;

	if (pfFraction)
		*pfFraction = fFraction;
	
	return true;
}

/*	AABB-AABB intersection routine.

	Return true if two AABB collision, otherwise return false.

	vCenter1: first AABB's center.
	vExt1: first AABB's extents.
	vCenter1: second AABB's center.
	vExt1: second AABB's extents.
*/	
bool CLS_AABBAABBOverlap(const A3DVECTOR3& vCenter1, const A3DVECTOR3& vExt1, 
						 const A3DVECTOR3& vCenter2, const A3DVECTOR3& vExt2)
{
	float fDist;

	//	X axis
	fDist = vCenter1.x - vCenter2.x;
	if (fDist < 0)
		fDist = -fDist;
	
	if (vExt1.x + vExt2.x < fDist)
		return false;

	//	Y axis
	fDist = vCenter1.y - vCenter2.y;
	if (fDist < 0)
		fDist = -fDist;
	
	if (vExt1.y + vExt2.y < fDist)
		return false;

	//	Z axis
	fDist = vCenter1.z - vCenter2.z;
	if (fDist < 0)
		fDist = -fDist;
	
	if (vExt1.z + vExt2.z < fDist)
		return false;

	return true;
}

/*	Check whether a ray collision with specified sphere

	Return true if collision occured

	vStart: start point of line segment
	vDelta: no-normalized direction of ray (end point - start point)
	vOrigin: sphere's center point
	fRadius: sphere's sphere.
*/
bool CLS_RaySphereOverlap(const A3DVECTOR3& vStart, const A3DVECTOR3& vDelta, const A3DVECTOR3& vOrigin, FLOAT fRadius)
{
	A3DVECTOR3 vDir, vTemp;
	float fLen, d;

	//	Ray segment's length
	fLen = Normalize(vDelta, vDir);

	vTemp = vOrigin - vStart;
	d = DotProduct(vTemp, vDir);

	//	Too far ahead ?
	if (d > fLen + fRadius)
		return false;

	//	Too far behind ?
	if (d < -fRadius)
		return false;

	vTemp = vOrigin - (vStart + vDir * d);
	fLen  = vTemp.Magnitude();

	//	Too far side ?
	if (fLen > fRadius)
		return false;

	return true;
}

/*	Check whether a ray collision with specified plane.

	vStart: start point of line segment
	vDelta: no-normalized direction of ray (end point - start point)
	vPlaneNormal: plane's normal
	fDist: d parameters of plane equation
	b2Sides: treat plane as two sides
	vHitPos (out): receive hit position
	pfFraction (out): receive hit fraction
*/
bool CLS_RayToPlane(A3DVECTOR3& vStart, A3DVECTOR3& vDelta, A3DVECTOR3& vPlaneNormal, 
					float fDist, bool b2Sides, A3DVECTOR3& vHitPos, float* pfFraction)
{
	A3DVECTOR3 vNormal = vPlaneNormal;

	float d1 = DotProduct(vStart, vNormal) - fDist;
	float d2 = DotProduct(vStart + vDelta, vNormal) - fDist;

	if (b2Sides)
	{
		if (d1 < 0.0f)
			vNormal = -vNormal;
	}
	else if (d1 < 0.0f)		//	Start point is behind side
		return false;

	if (DotProduct(vDelta, vNormal) >= 0.0f)
		return false;

	float d;

	if (d1 < 0.0f)
		d = d1 / (d1 - d2);
	else
		d = d1 / (d1 - d2);

	if (d < 0.0f)
		d = 0.0f;

	if (pfFraction)
		*pfFraction = d;

	vHitPos = vStart + vDelta * d; 

	return true;
}

/*	OBB-quadrangle intersection routine. 

	Return true if OBB collision with quadrangle, otherwise return false

	OBB: OBB information
	ov0, ov1, ov2, ov3: four vertices of quadrangle
	vNormal: normal of plane on which quadrangle lies
	pfFraction: collision fraction (0 is -ExtZ, 1, is ExtZ), cann't be NULL.
*/
bool CLS_OBBToQuadrangle(const A3DOBB& OBB, const A3DVECTOR3& ov0, const A3DVECTOR3& ov1, const A3DVECTOR3& ov2,
						 const A3DVECTOR3& ov3, const A3DVECTOR3& vNormal, FLOAT* pfFraction)
{
	if (DotProduct(OBB.Center - ov0, vNormal) < 0)
		return false;

	//	Build a AABB and transfrom sides in AABB coordinates
	A3DVECTOR3 _v0 = ov0 - OBB.Center;
	A3DVECTOR3 _v1 = ov1 - OBB.Center;
	A3DVECTOR3 _v2 = ov2 - OBB.Center; 
	A3DVECTOR3 _v3 = ov3 - OBB.Center;
	A3DVECTOR3 v0, v1, v2, v3;

	float fMin, fMax;

	//	Check separating axis on x axis
	v0.x = DotProduct(_v0, OBB.XAxis);
	v1.x = DotProduct(_v1, OBB.XAxis);
	v2.x = DotProduct(_v2, OBB.XAxis);
	v3.x = DotProduct(_v3, OBB.XAxis);
	fMin = min4(v0.x, v1.x, v2.x, v3.x);
	fMax = max4(v0.x, v1.x, v2.x, v3.x);
	if (fMin > OBB.Extents.x || fMax < -OBB.Extents.x)
		return false;

	//	Check separating axis on y axis
	v0.y = DotProduct(_v0, OBB.YAxis);
	v1.y = DotProduct(_v1, OBB.YAxis);
	v2.y = DotProduct(_v2, OBB.YAxis);
	v3.y = DotProduct(_v3, OBB.YAxis);
	fMin = min4(v0.y, v1.y, v2.y, v3.y);
	fMax = max4(v0.y, v1.y, v2.y, v3.y);
	if (fMin > OBB.Extents.y || fMax < -OBB.Extents.y)
		return false;

	//	Check separating axis on z axis
	v0.z = DotProduct(_v0, OBB.ZAxis);
	v1.z = DotProduct(_v1, OBB.ZAxis);
	v2.z = DotProduct(_v2, OBB.ZAxis);
	v3.z = DotProduct(_v3, OBB.ZAxis);
	fMin = min4(v0.z, v1.z, v2.z, v3.z);
	fMax = max4(v0.z, v1.z, v2.z, v3.z);
	if (fMin > OBB.Extents.z || fMax < -OBB.Extents.z)
		return false;

	//	Now, test these 9 axis made from cross product of triangle's edge and x, y, z
/*	A3DVECTOR3 vEdge;
	float p0, p1, p2, fRadius;
	float fex, fey, fez;	//	Absolutes of edge

	vEdge = v1 - v0;
	fex	= vEdge.x >= 0 ? vEdge.x : -vEdge.x;
	fey	= vEdge.y >= 0 ? vEdge.y : -vEdge.y;
	fez	= vEdge.z >= 0 ? vEdge.z : -vEdge.z;

	//	edge v0-v1 corss product with x
	p0 = vEdge.z * v0.y - vEdge.y * v0.z;
	p1 = vEdge.z * v2.y - vEdge.y * v2.z;
	p2 = vEdge.z * v3.y - vEdge.y * v3.z;
	fRadius = fez * OBB.Extents.y + fey * OBB.Extents.z;
	if (min3(p0, p1, p2) > fRadius || max3(p0, p1, p2) < -fRadius)
		return false;

	//	edge v0-v1 corss product with y
	p0 = -vEdge.z * v0.x + vEdge.x * v0.z;
	p1 = -vEdge.z * v2.x + vEdge.x * v2.z;
	p2 = -vEdge.z * v3.x + vEdge.x * v3.z;
	fRadius = fez * OBB.Extents.x + fex * OBB.Extents.z;
	if (min3(p0, p1, p2) > fRadius || max3(p0, p1, p2) < -fRadius)
		return false;

	//	edge v0-v1 corss product with z
	p0 = vEdge.y * v1.x - vEdge.x * v1.y;
	p1 = vEdge.y * v2.x - vEdge.x * v2.y;
	p2 = vEdge.y * v3.x - vEdge.x * v3.y;
	fRadius = fey * OBB.Extents.x + fex * OBB.Extents.y;
	if (min3(p0, p1, p2) > fRadius || max3(p0, p1, p2) < -fRadius)
		return false;

	vEdge = v2 - v1;
	fex	= vEdge.x >= 0 ? vEdge.x : -vEdge.x;
	fey	= vEdge.y >= 0 ? vEdge.y : -vEdge.y;
	fez	= vEdge.z >= 0 ? vEdge.z : -vEdge.z;

	//	edge v1-v2 corss product with x
	p0 = vEdge.z * v0.y - vEdge.y * v0.z;
	p1 = vEdge.z * v2.y - vEdge.y * v2.z;
	p2 = vEdge.z * v3.y - vEdge.y * v3.z;
	fRadius = fez * OBB.Extents.y + fey * OBB.Extents.z;
	if (min3(p0, p1, p2) > fRadius || max3(p0, p1, p2) < -fRadius)
		return false;

	//	edge v1-v2 corss product with y
	p0 = -vEdge.z * v0.x + vEdge.x * v0.z;
	p1 = -vEdge.z * v2.x + vEdge.x * v2.z;
	p2 = -vEdge.z * v3.x + vEdge.x * v3.z;
	fRadius = fez * OBB.Extents.x + fex * OBB.Extents.z;
	if (min3(p0, p1, p2) > fRadius || max3(p0, p1, p2) < -fRadius)
		return false;

	//	edge v1-v2 corss product with z
	p0 = vEdge.y * v0.x - vEdge.x * v0.y;
	p1 = vEdge.y * v1.x - vEdge.x * v1.y;
	p2 = vEdge.y * v3.x - vEdge.x * v3.y;
	fRadius = fey * OBB.Extents.x + fex * OBB.Extents.y;
	if (min3(p0, p1, p2) > fRadius || max3(p0, p1, p2) < -fRadius)
		return false;

	if (!bParallel)
	{
		vEdge = v3 - v2;
		fex	= vEdge.x >= 0 ? vEdge.x : -vEdge.x;
		fey	= vEdge.y >= 0 ? vEdge.y : -vEdge.y;
		fez	= vEdge.z >= 0 ? vEdge.z : -vEdge.z;
		
		//	edge v2-v3 corss product with x
		p0 = vEdge.z * v0.y - vEdge.y * v0.z;
		p1 = vEdge.z * v1.y - vEdge.y * v1.z;
		p2 = vEdge.z * v3.y - vEdge.y * v3.z;
		fRadius = fez * OBB.Extents.y + fey * OBB.Extents.z;
		if (min3(p0, p1, p2) > fRadius || max3(p0, p1, p2) < -fRadius)
			return false;
		
		//	edge v2-v3 corss product with y
		p0 = -vEdge.z * v0.x + vEdge.x * v0.z;
		p1 = -vEdge.z * v1.x + vEdge.x * v1.z;
		p2 = -vEdge.z * v3.x + vEdge.x * v3.z;
		fRadius = fez * OBB.Extents.x + fex * OBB.Extents.z;
		if (min3(p0, p1, p2) > fRadius || max3(p0, p1, p2) < -fRadius)
			return false;
		
		//	edge v2-v3 corss product with z
		p0 = vEdge.y * v0.x - vEdge.x * v0.y;
		p1 = vEdge.y * v1.x - vEdge.x * v1.y;
		p2 = vEdge.y * v3.x - vEdge.x * v3.y;
		fRadius = fey * OBB.Extents.x + fex * OBB.Extents.y;
		if (min3(p0, p1, p2) > fRadius || max3(p0, p1, p2) < -fRadius)
			return false;

		vEdge = v0 - v3;
		fex	= vEdge.x >= 0 ? vEdge.x : -vEdge.x;
		fey	= vEdge.y >= 0 ? vEdge.y : -vEdge.y;
		fez	= vEdge.z >= 0 ? vEdge.z : -vEdge.z;
		
		//	edge v3-v0 corss product with x
		p0 = vEdge.z * v0.y - vEdge.y * v0.z;
		p1 = vEdge.z * v1.y - vEdge.y * v1.z;
		p2 = vEdge.z * v2.y - vEdge.y * v2.z;
		fRadius = fez * OBB.Extents.y + fey * OBB.Extents.z;
		if (min3(p0, p1, p2) > fRadius || max3(p0, p1, p2) < -fRadius)
			return false;
		
		//	edge v3-v0 corss product with y
		p0 = -vEdge.z * v0.x + vEdge.x * v0.z;
		p1 = -vEdge.z * v1.x + vEdge.x * v1.z;
		p2 = -vEdge.z * v2.x + vEdge.x * v2.z;
		fRadius = fez * OBB.Extents.x + fex * OBB.Extents.z;
		if (min3(p0, p1, p2) > fRadius || max3(p0, p1, p2) < -fRadius)
			return false;
		
		//	edge v3-v0 corss product with z
		p0 = vEdge.y * v1.x - vEdge.x * v1.y;
		p1 = vEdge.y * v2.x - vEdge.x * v2.y;
		p2 = vEdge.y * v3.x - vEdge.x * v3.y;
		fRadius = fey * OBB.Extents.x + fex * OBB.Extents.y;
		if (min3(p0, p1, p2) > fRadius || max3(p0, p1, p2) < -fRadius)
			return false;
	}

	if (!pfFraction)
		return true;
*/
	A3DPLANE Plane;
	Plane.vNormal.z = DotProduct(vNormal, OBB.ZAxis);

	if (Plane.vNormal.z == 1 || Plane.vNormal.z == -1)
	{
		if (v0.z < -OBB.Extents.z)
			*pfFraction = 0.0f;
		else
			*pfFraction = (v0.z + OBB.Extents.z) / (OBB.Extents.z + OBB.Extents.z);
	}
	else
	{
		Plane.vNormal.x  = DotProduct(vNormal, OBB.XAxis);
		Plane.vNormal.y  = DotProduct(vNormal, OBB.YAxis);
		Plane.fDist		 = DotProduct(v0, Plane.vNormal);

		A3DVECTOR3* aVerts[4] = {&v0, &v1, &v2, &v3};

		*pfFraction = _HitPointOfOBBAndPolygon(OBB, aVerts, 4, Plane);

		if (*pfFraction < 0)
			return false;
	}

	return true;
}

/*	Calculate intersection point of OBB and polygon, before this operation, you should
	ensure OBB and polygon have collistion point.

	Return collision fraction (0 is -ExtZ, 1, is ExtZ), < 0 means no collision occured

	aVerts: vertices which have been transformed into OBB's coordinates.
	iNumVert: number of vertex in aVerts.
	Plane: plane on which polygon is lies, which has been transformed into OBB's coordinates
*/
float _HitPointOfOBBAndPolygon(const A3DOBB& OBB, A3DVECTOR3** aVerts, int iNumVert, const A3DPLANE& Plane)
{
	A3DVECTOR3 aProj1[8], aProj2[8];	//	Vertices projection on x-y plane
	A3DVECTOR3 *v1, *v2, *aOld, *aNew;
	bool bIn1, bIn2;
	int i, iNumOld, iNumNew;
	float d, d1, d2;

	//	Clip side's projection on x-y plane with OBB's projection
	v1		= aVerts[iNumVert-1];
	bIn1	= ((d1 = v1->y - OBB.Extents.y) <= 0);
	iNumNew = 0;
	aNew	= aProj1;

	//	Clip by y=Extents.y line
	for (i=0; i < iNumVert; i++)
	{
		v2	 = aVerts[i];
		bIn2 = ((d2 = v2->y - OBB.Extents.y) <= 0);
	
		if (bIn1)
		{
			aNew[iNumNew].x = v1->x;
			aNew[iNumNew].y = v1->y;
			aNew[iNumNew].z	= v1->z;
			iNumNew++;

			if (!bIn2)
			{
				d = d1 / (d1 - d2);
				aNew[iNumNew].x = v1->x + (v2->x - v1->x) * d;
				aNew[iNumNew].y = OBB.Extents.y;
				aNew[iNumNew].z = v1->z + (v2->z - v1->z) * d;
				iNumNew++;
			}
		}
		else if (bIn2)
		{
			d = d1 / (d1 - d2);
			aNew[iNumNew].x = v1->x + (v2->x - v1->x) * d;
			aNew[iNumNew].y = OBB.Extents.y;
			aNew[iNumNew].z = v1->z + (v2->z - v1->z) * d;
			iNumNew++;
		}

		bIn1 = bIn2;
		v1	 = v2;
		d1	 = d2;
	}

	if ((iNumOld = iNumNew) < 3)
		return -1.0f;

	//	Clip by x=Extents.x line
	aOld	= aNew;
	aNew	= aProj2;
	v1		= &aOld[iNumOld-1];
	bIn1	= ((d1 = v1->x - OBB.Extents.x) <= 0);
	iNumNew = 0;

	for (i=0; i < iNumOld; i++)
	{
		v2	 = &aOld[i];
		bIn2 = ((d2 = v2->x - OBB.Extents.x) <= 0);
	
		if (bIn1)
		{
			aNew[iNumNew].x = v1->x;
			aNew[iNumNew].y = v1->y;
			aNew[iNumNew].z = v1->z;
			iNumNew++;

			if (!bIn2)
			{
				d = d1 / (d1 - d2);
				aNew[iNumNew].x = OBB.Extents.x;
				aNew[iNumNew].y = v1->y + (v2->y - v1->y) * d;
				aNew[iNumNew].z = v1->z + (v2->z - v1->z) * d;
				iNumNew++;
			}
		}
		else if (bIn2)
		{
			d = d1 / (d1 - d2);
			aNew[iNumNew].x = OBB.Extents.x;
			aNew[iNumNew].y = v1->y + (v2->y - v1->y) * d;
			aNew[iNumNew].z = v1->z + (v2->z - v1->z) * d;
			iNumNew++;
		}

		bIn1 = bIn2;
		v1	 = v2;
		d1	 = d2;
	}

	if ((iNumOld = iNumNew) < 3)
		return -1.0f;

	//	Clip by y=-Extents.y line
	aOld	= aNew;
	aNew	= aProj1;
	v1		= &aOld[iNumOld-1];
	bIn1	= ((d1 = v1->y + OBB.Extents.y) >= 0);
	iNumNew = 0;

	for (i=0; i < iNumOld; i++)
	{
		v2	 = &aOld[i];
		bIn2 = ((d2 = v2->y + OBB.Extents.y) >= 0);
	
		if (bIn1)
		{
			aNew[iNumNew].x = v1->x;
			aNew[iNumNew].y = v1->y;
			aNew[iNumNew].z = v1->z;
			iNumNew++;

			if (!bIn2)
			{
				d = d1 / (d1 - d2);
				aNew[iNumNew].x = v1->x + (v2->x - v1->x) * d;
				aNew[iNumNew].y = -OBB.Extents.y;
				aNew[iNumNew].z = v1->z + (v2->z - v1->z) * d;
				iNumNew++;
			}
		}
		else if (bIn2)
		{
			d = d1 / (d1 - d2);
			aNew[iNumNew].x = v1->x + (v2->x - v1->x) * d;
			aNew[iNumNew].y = -OBB.Extents.y;
			aNew[iNumNew].z = v1->z + (v2->z - v1->z) * d;
			iNumNew++;
		}

		bIn1 = bIn2;
		v1	 = v2;
		d1	 = d2;
	}

	if ((iNumOld = iNumNew) < 3)
		return -1.0f;

	//	Clip by x=-Extents.x line
	aOld	= aNew;
	aNew	= aProj2;
	v1		= &aOld[iNumOld-1];
	bIn1	= ((d1 = v1->x + OBB.Extents.x) >= 0);
	iNumNew = 0;

	for (i=0; i < iNumOld; i++)
	{
		v2	 = &aOld[i];
		bIn2 = ((d2 = v2->x + OBB.Extents.x) >= 0);
	
		if (bIn1)
		{
			aNew[iNumNew].x = v1->x;
			aNew[iNumNew].y = v1->y;
			aNew[iNumNew].z = v1->z;
			iNumNew++;

			if (!bIn2)
			{
				d = d1 / (d1 - d2);
				aNew[iNumNew].x = -OBB.Extents.x;
				aNew[iNumNew].y = v1->y + (v2->y - v1->y) * d;
				aNew[iNumNew].z = v1->z + (v2->z - v1->z) * d;
				iNumNew++;
			}
		}
		else if (bIn2)
		{
			d = d1 / (d1 - d2);
			aNew[iNumNew].x = -OBB.Extents.x;
			aNew[iNumNew].y = v1->y + (v2->y - v1->y) * d;
			aNew[iNumNew].z = v1->z + (v2->z - v1->z) * d;
			iNumNew++;
		}

		bIn1 = bIn2;
		v1	 = v2;
		d1	 = d2;
	}

	if (iNumNew < 3)
		return -1.0f;

	//	Calculate the intersection point which has the smallest z value
	float fMinZ=aNew[0].z, fMaxZ=aNew[0].z;

	for (i=1; i < iNumNew; i++)
	{
		if (aNew[i].z < fMinZ)
			fMinZ = aNew[i].z;

		if (aNew[i].z > fMaxZ)
			fMaxZ = aNew[i].z;
	}

	if (fMinZ < -OBB.Extents.z)
	{
		if (fMaxZ < -OBB.Extents.z)
			return -1.0f;
		else
			return 0.0f;
	}
	else if (fMinZ > OBB.Extents.z)
		return -1.0f;

	return (fMinZ + OBB.Extents.z) / (OBB.Extents.z + OBB.Extents.z);
}

/*	Collision routine when AABB move to a plane.

	Return values:
	0: AABB doesn't collision with plane
	1: if AABB collision with specified plane
	2: AABB is corssing with plane on start point.

	vStart: start position of aabb's center
	vEnd: end position of aabb's ceter
	vExts: aabb's half extents
	Plane: plane's information
	pfFraction (out): used to receive moving fraction of aabb.
*/
int CLS_AABBToPlane(const A3DVECTOR3& vStart, const A3DVECTOR3& vEnd, const A3DVECTOR3& vExts, 
					const A3DSPLANE& Plane, float* pfFraction)
{
	A3DVECTOR3 vOffs;

	//	Push the plane out apropriately for mins/maxs
	switch (Plane.bySignBits)
	{
	case 0:	vOffs.x = -vExts.x;		vOffs.y = -vExts.y;		vOffs.z = -vExts.z;		break;
	case 1:	vOffs.x = vExts.x;		vOffs.y = -vExts.y;		vOffs.z = -vExts.z;		break;
	case 2:	vOffs.x = -vExts.x;		vOffs.y = vExts.y;		vOffs.z = -vExts.z;		break;
	case 3:	vOffs.x = vExts.x;		vOffs.y = vExts.y;		vOffs.z = -vExts.z;		break;
	case 4:	vOffs.x = -vExts.x;		vOffs.y = -vExts.y;		vOffs.z = vExts.z;		break;
	case 5:	vOffs.x = vExts.x;		vOffs.y = -vExts.y;		vOffs.z = vExts.z;		break;
	case 6:	vOffs.x = -vExts.x;		vOffs.y = vExts.y;		vOffs.z = vExts.z;		break;
	case 7:	vOffs.x = vExts.x;		vOffs.y = vExts.y;		vOffs.z = vExts.z;		break;
	default: return 0;
	}

	float fOff	= DotProduct(vOffs, Plane.vNormal);
	float fDist = Plane.fDist - fOff;
	float d1	= DotProduct(vStart, Plane.vNormal) - fDist;
	float d2	= DotProduct(vEnd, Plane.vNormal) - fDist;

	//	If completely in front of face, no intersection
	if (d1 > 0.0f && d2 > 0.0f)
	{
		*pfFraction = 1.0f;
		return 0;
	}

	//	If completely behind of face
	if (d1 <= 0.0f && d2 <= 0.0f)
	{
		if (d1 - fOff > 0.0f)	//	Corss at start point ?
		{
			*pfFraction = 0.0f;
			return 2;
		}
		else	//	Completely behind of face
		{
			*pfFraction = 1.0f;
			return 0;
		}
	}

	//	Crosses plane
	if (FLOATISZERO(d1 - d2))
		*pfFraction = 0.0f;
	else if (d1 > d2)	//	Enter
		*pfFraction = (d1 - EPSILON_DISTANCE) / (d1 - d2);
	else	//	Leave
		*pfFraction = (d1 + EPSILON_DISTANCE) / (d1 - d2);

	if (*pfFraction < 0.0f)
		*pfFraction = 0.0f;
	else if (*pfFraction > 1.0f)	//	This check may not be necessary, just for safe
		*pfFraction = 1.0f;

	return 1;
}

/*	OBB-OBB overlap test using the separating axis theorem.

	Return true if boxes overlap.

	obb1: obb A
	obb2: obb B
*/
bool CLS_OBBOBBOverlap(const A3DOBB& obb1, const A3DOBB& obb2)
{
	A3DMATRIX4 mat1, mat2;

	//	mat1: transform matrix from world space to obb1 space
	mat1._11 = obb1.XAxis.x;
	mat1._21 = obb1.XAxis.y;
	mat1._31 = obb1.XAxis.z;
	mat1._12 = obb1.YAxis.x;
	mat1._22 = obb1.YAxis.y;
	mat1._32 = obb1.YAxis.z;
	mat1._13 = obb1.ZAxis.x;
	mat1._23 = obb1.ZAxis.y;
	mat1._33 = obb1.ZAxis.z;

	mat1._41 = -DotProduct(obb1.Center, obb1.XAxis);
	mat1._42 = -DotProduct(obb1.Center, obb1.YAxis);
	mat1._43 = -DotProduct(obb1.Center, obb1.ZAxis);

	mat1._14 = mat1._24 = mat1._34 = 0.0f;
	mat1._44 = 1.0f;

	//	mat2: transform matrix from obb2 space to obb1 space
	mat2._11 = obb2.XAxis.x;
	mat2._12 = obb2.XAxis.y;
	mat2._13 = obb2.XAxis.z;
	mat2._21 = obb2.YAxis.x;
	mat2._22 = obb2.YAxis.y;
	mat2._23 = obb2.YAxis.z;
	mat2._31 = obb2.ZAxis.x;
	mat2._32 = obb2.ZAxis.y;
	mat2._33 = obb2.ZAxis.z;

	mat2._41 = obb2.Center.x;
	mat2._42 = obb2.Center.y;
	mat2._43 = obb2.Center.z;

	mat2._14 = mat2._24 = mat2._34 = 0.0f;
	mat2._44 = 1.0f;

	//	Build matrix which transfroms vertex from obb2's space to obb1's space
	A3DMATRIX4 mat = mat2 * mat1;

	return CLS_OBBOBBOverlap(obb1.Extents, obb2.Extents, mat);
}

/*	OBB-OBB overlap test using the separating axis theorem.

	Return true if boxes overlap.

	vExt1: obb A's extents
	vExt2: obb B's extents
	mat: matrix which does transfrom from obb2's space to obb1's space
*/
bool CLS_OBBOBBOverlap(const A3DVECTOR3& vExt1, const A3DVECTOR3& vExt2, const A3DMATRIX4& mat)
{
	A3DMATRIX4 am;

	//	Absolute matrix
	am.m[0][0] = (float)fabs(mat.m[0][0]);
	am.m[0][1] = (float)fabs(mat.m[0][1]);
	am.m[0][2] = (float)fabs(mat.m[0][2]);
	am.m[1][0] = (float)fabs(mat.m[1][0]);
	am.m[1][1] = (float)fabs(mat.m[1][1]);
	am.m[1][2] = (float)fabs(mat.m[1][2]);
	am.m[2][0] = (float)fabs(mat.m[2][0]);
	am.m[2][1] = (float)fabs(mat.m[2][1]);
	am.m[2][2] = (float)fabs(mat.m[2][2]);

	A3DVECTOR3 vT(mat._41, mat._42, mat._43);

	float t, s;

	//	A1 x A2 = A0
	t = (float)fabs(vT.x);
  
	if (t > vExt1.x + vExt2.x * am.m[0][0] + vExt2.y * am.m[1][0] + vExt2.z * am.m[2][0])
		return false;
  
	//	B1 x B2 = B0
	s = vT.x * mat.m[0][0] + vT.y * mat.m[0][1] + vT.z * mat.m[0][2];
	t = (float)fabs(s);

	if (t > vExt2.x + vExt1.x * am.m[0][0] + vExt1.y * am.m[0][1] + vExt1.z * am.m[0][2])
		return false;
    
	//	A2 x A0 = A1
	t = (float)fabs(vT.y);
  
	if (t > vExt1.y + vExt2.x * am.m[0][1] + vExt2.y * am.m[1][1] + vExt2.z * am.m[2][1])
		return false;

	//	A0 x A1 = A2
	t = (float)fabs(vT.z);

	if (t > vExt1.z + vExt2.x * am.m[0][2] + vExt2.y * am.m[1][2] + vExt2.z * am.m[2][2])
		return false;

	//	B2 x B0 = B1
	s = vT.x * mat.m[1][0] + vT.y * mat.m[1][1] + vT.z * mat.m[1][2];
	t = (float)fabs(s);

	if (t > vExt2.y + vExt1.x * am.m[1][0] + vExt1.y * am.m[1][1] + vExt1.z * am.m[1][2])
		return false;

	//	B0 x B1 = B2
	s = vT.x * mat.m[2][0] + vT.y * mat.m[2][1] + vT.z * mat.m[2][2];
	t = (float)fabs(s);

	if (t > vExt2.z + vExt1.x * am.m[2][0] + vExt1.y * am.m[2][1] + vExt1.z * am.m[2][2])
		return false;

	//	A0 x B0
	s = vT.z * mat.m[0][1] - vT.y * mat.m[0][2];
	t = (float)fabs(s);

	if (t > vExt1.y * am.m[0][2] + vExt1.z * am.m[0][1] + vExt2.y * am.m[2][0] + vExt2.z * am.m[1][0])
		return false;
  
	//	A0 x B1
	s = vT.z * mat.m[1][1] - vT.y * mat.m[1][2];
	t = (float)fabs(s);

	if (t > vExt1.y * am.m[1][2] + vExt1.z * am.m[1][1] + vExt2.x * am.m[2][0] + vExt2.z * am.m[0][0])
		return false;

	//	A0 x B2
	s = vT.z * mat.m[2][1] - vT.y * mat.m[2][2];
	t = (float)fabs(s);

	if (t > vExt1.y * am.m[2][2] + vExt1.z * am.m[2][1] + vExt2.x * am.m[1][0] + vExt2.y * am.m[0][0])
		return false;

	//	A1 x B0
	s = vT.x * mat.m[0][2] - vT.z * mat.m[0][0];
	t = (float)fabs(s);

	if (t > vExt1.x * am.m[0][2] + vExt1.z * am.m[0][0] + vExt2.y * am.m[2][1] + vExt2.z * am.m[1][1])
		return false;

	//	A1 x B1
	s = vT.x * mat.m[1][2] - vT.z * mat.m[1][0];
	t = (float)fabs(s);

	if (t > vExt1.x * am.m[1][2] + vExt1.z * am.m[1][0] + vExt2.x * am.m[2][1] + vExt2.z * am.m[0][1])
		return false;

	//	A1 x B2
	s = vT.x * mat.m[2][2] - vT.z * mat.m[2][0];
	t = (float)fabs(s);

	if (t > vExt1.x * am.m[2][2] + vExt1.z * am.m[2][0] + vExt2.x * am.m[1][1] + vExt2.y * am.m[0][1])
		return false;

	//	A2 x B0
	s = vT.y * mat.m[0][0] - vT.x * mat.m[0][1];
	t = (float)fabs(s);

	if (t > vExt1.x * am.m[0][1] + vExt1.y * am.m[0][0] + vExt2.y * am.m[2][2] + vExt2.z * am.m[1][2])
		return false;

	//	A2 x B1
	s = vT.y * mat.m[1][0] - vT.x * mat.m[1][1];
	t = (float)fabs(s);

	if (t > vExt1.x * am.m[1][1] + vExt1.y * am.m[1][0] + vExt2.x * am.m[2][2] + vExt2.z * am.m[0][2])
		return false;

	//	A2 x B2
	s = vT.y * mat.m[2][0] - vT.x * mat.m[2][1];
	t = (float)fabs(s);

	if (t > vExt1.x * am.m[2][1] + vExt1.y * am.m[2][0] + vExt2.x * am.m[1][2] + vExt2.y * am.m[0][2])
		return false;

	return true;
}

/*	AABB-OBB overlap test using the separating axis theorem.

	Return true if boxes overlap.

	vCenter: aabb's center
	vExts: aabb's extents
	obb: obb's inforamtion
*/
bool CLS_AABBOBBOverlap(const A3DVECTOR3& vCenter, const A3DVECTOR3& vExts, const A3DOBB& obb)
{
	A3DMATRIX4 mat;

	//	mat: transform matrix from obb space to aabb's space
	mat._11 = obb.XAxis.x;
	mat._12 = obb.XAxis.y;
	mat._13 = obb.XAxis.z;
	mat._21 = obb.YAxis.x;
	mat._22 = obb.YAxis.y;
	mat._23 = obb.YAxis.z;
	mat._31 = obb.ZAxis.x;
	mat._32 = obb.ZAxis.y;
	mat._33 = obb.ZAxis.z;

	mat._41 = obb.Center.x - vCenter.x;
	mat._42 = obb.Center.y - vCenter.y;
	mat._43 = obb.Center.z - vCenter.z;

	mat._14 = mat._24 = mat._34 = 0.0f;
	mat._44 = 1.0f;

	return CLS_OBBOBBOverlap(vExts, obb.Extents, mat);
}

/*	AABB-Sphere overlap test routine

	Return true if boxes overlap.

	aabb: aabb's inforamtion, only Center and Extents will be used
	vCenter: sphere's center
	fRadius: sphere's radius
*/
bool CLS_AABBSphereOverlap(const A3DAABB& aabb, const A3DVECTOR3& vCenter, float fRadius)
{
	float d = 0.0f;
	float fRadius2 = fRadius * fRadius;
	
	float tmp = vCenter.x - aabb.Center.x;
	float s = tmp + aabb.Extents.x;

	if (s < 0.0f)
	{
		if ((d += s * s) > fRadius2)
			return false;
	}
	else
	{
		s = tmp - aabb.Extents.x;
		if (s > 0.0f)
		{
			if ((d += s * s) > fRadius2)
				return false;
		}
	}

	tmp = vCenter.y - aabb.Center.y;
	s = tmp + aabb.Extents.y;

	if (s < 0.0f)
	{
		if ((d += s * s) > fRadius2)
			return false;
	}
	else
	{
		s = tmp - aabb.Extents.y;
		if (s > 0.0f)
		{
			if ((d += s * s) > fRadius2)
				return false;
		}
	}

	tmp = vCenter.z - aabb.Center.z;
	s = tmp + aabb.Extents.z;

	if (s < 0.0f)
	{
		if ((d += s * s) > fRadius2)
			return false;
	}
	else
	{
		s = tmp - aabb.Extents.z;
		if (s > 0.0f)
		{
			if ((d += s * s) > fRadius2)
				return false;
		}
	}

	return d <= fRadius2;
}

/*	OBB-Sphere overlap test routine

	Return true if boxes overlap.

	obb: obb's inforamtion
	vCenter: sphere's center
	fRadius: sphere's radius
*/
bool CLS_OBBSphereOverlap(const A3DOBB& obb, const A3DVECTOR3& vCenter, float fRadius)
{
	A3DAABB aabb;
	aabb.Center.Clear();
	aabb.Extents = obb.Extents;

	//	Transfrom sphere's center to obb space
	A3DVECTOR3 vsc, vDelta(vCenter-obb.Center);
	vsc.x = DotProduct(obb.XAxis, vDelta);
	vsc.y = DotProduct(obb.YAxis, vDelta);
	vsc.z = DotProduct(obb.ZAxis, vDelta);

	return CLS_AABBSphereOverlap(aabb, vsc, fRadius);
}

