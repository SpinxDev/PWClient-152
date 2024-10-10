/*
 * FILE: A3DWorldCDS.h
 *
 * DESCRIPTION: The class that collect the collision detection interface of A3DWorld
 *
 * CREATED BY: Hedi, 2001/7/27
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DWORLDCDS_H_
#define _A3DWORLDCDS_H_

#include "A3DTypes.h"
#include "A3DCDS.h"

class A3DWorld;

class A3DWorldCDS : public A3DCDS
{
protected:
	A3DWorld *			m_pA3DWorld;

public:

	A3DWorldCDS();
	virtual ~A3DWorldCDS();

	bool Init(A3DWorld * pA3DWorld);
	bool Release();

	//	Ray trace routine, Return true if collision occured
	virtual bool RayTrace(const A3DVECTOR3& vStart, const A3DVECTOR3& vVel, FLOAT vTime, RAYTRACERT* pTraceRt, int iTraceObj, DWORD dwUserData);
	//	AABB trace routine, Return true if collision occured
	virtual bool AABBTrace(const A3DVECTOR3& vStart, const A3DVECTOR3& vExts, const A3DVECTOR3& vVel, FLOAT fTime, AABBTRACERT* pTraceRt, int iTraceObj, DWORD dwUserData);
	//	Sphere trace routine, Return true if collision occured
	virtual bool SphereTrace(const A3DVECTOR3& vStart, float fRadius, A3DVECTOR3& vVel, FLOAT fTime, SPHERETRACERT* pTraceRt, int iTraceObj, DWORD dwUserData) { return true; }
	//	Split mark routine
	virtual bool SplitMark(const A3DAABB& aabb, const A3DVECTOR3& vNormal, A3DLVERTEX* aVerts, WORD* aIndices, 
						bool bJog, int* piNumVert, int* piNumIdx, float fRadiusScale=0.2f);

	//	Return true if this CDS can split mark
	virtual bool CanSplitMark();

	//	Position is visible ?
	virtual bool PosIsVisible(const A3DVECTOR3& vPos, int iVisObj, DWORD dwUserData);
	//	AABB is visible ?
	virtual bool AABBIsVisible(const A3DAABB& aabb, int iVisObj, DWORD dwUserData);
	//	AABB is visible ?
	virtual bool AABBIsVisible(const A3DVECTOR3& vMins, const A3DVECTOR3& vMaxs, int iVisObj, DWORD dwUserData);
};

#endif//_A3DCSD_H_
