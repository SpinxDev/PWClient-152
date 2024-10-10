/*
 * FILE: GL_CDS.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/1/12
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include "A3DCDS.h"

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

//	Visible check object
#define VISOBJ_MATTER		(VISOBJ_USER + 1)		//	Fixed matter

//	Trace check object
#define TRACEOBJ_TESTRAY	(TRACEOBJ_USER + 1)		//	Test ray
#define TRACEOBJ_CAMERA		(TRACEOBJ_USER + 2)		//	Player's camera
#define TRACEOBJ_PLAYER		(TRACEOBJ_USER + 3)		//	Player
#define TRACEOBJ_LBTNCLICK	(TRACEOBJ_USER + 4)		//	Player click left button
#define TRACEOBJ_RBTNCLICK	(TRACEOBJ_USER + 5)		//	Player click right button
#define TRACEOBJ_SHADOW		(TRACEOBJ_USER + 6)		//	Shadow
#define TRACEOBJ_SCENE		(TRACEOBJ_USER + 7)		//	Ray used

//	Entity in ECTRACERT
enum
{
	ECENT_NONE = 0,
	ECENT_TERRAIN,
	ECENT_BUILDING,
	ECENT_PLAYER,
	ECENT_NPC,
	ECENT_MATTER,
	ECENT_DYN_OBJ,
	ECENT_FOREST,
};

///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////

//	Trace result
struct ECTRACERT
{
	int			iEntity;		//	Collision entity type
	int			iObjectID;		//	Object ID
	float		fFraction;		//	Trace faction
	A3DVECTOR3	vNormal;		//	Collision face normal
};

//	AABB trace information
struct ECAABBTRACE
{
	//	IN parameters
	AABBTRACEINFO*	pInfo;			//	Trace information
	int				iTraceObj;		//	Trace object
	DWORD			dwUserData;		//	User data

	//	OUT parameters
	AABBTRACERT		TempTraceRt;	//	Temporary trace result
	AABBTRACERT*	pTraceRt;		//	Real trace result
	ECTRACERT*		pECTraceRt;		//	Trace result
};

//	Ray trace information
struct ECRAYTRACE
{
	//	IN parameters
	A3DVECTOR3		vStart;			//	Start position
	A3DVECTOR3		vDelta;			//	Move delta
	int				iTraceObj;		//	Trace object
	DWORD			dwUserData;		//	User data
	DWORD			dwKeyStates;	//	Key states

	//	OUT parameters
	RAYTRACERT		TempTraceRt;	//	Temporary trace result
	RAYTRACERT*		pTraceRt;		//	Trace result
	ECTRACERT*		pECTraceRt;		//	Trace result
};

//	Sphere trace information
struct ECSPHERETRACE
{
	//	IN parameters
	SPHERETRACEINFO*	pInfo;		//	Trace information

	int				iTraceObj;		//	Trace object
	DWORD			dwUserData;		//	User data

	//	OUT parameters
	SPHERETRACERT	TempTraceRt;	//	Temporary trace result
	SPHERETRACERT*	pTraceRt;		//	Trace result
	ECTRACERT*		pECTraceRt;		//	Trace result
};

class CECManager;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CECCDS
//	
///////////////////////////////////////////////////////////////////////////

class CECCDS : public A3DCDS
{
public:		//	Types

public:		//	Constructor and Destructor

	CECCDS();
	virtual ~CECCDS();

public:		//	Attributes

	ECTRACERT	m_RayTraceRt;		//	Last ray trace result
	ECTRACERT	m_AABBTraceRt;		//	Last AABB trace result
	ECTRACERT	m_PlayerTraceRt;	//	Player trace result

public:		//	Operations

	//	Ray trace routine, Return true if collision occured
	virtual bool RayTrace(const A3DVECTOR3& vStart, const A3DVECTOR3& vVel, FLOAT vTime, RAYTRACERT* pTraceRt, int iTraceObj, DWORD dwUserData);
	//	AABB trace routine, Return true if collision occured
	virtual bool AABBTrace(const A3DVECTOR3& vStart, const A3DVECTOR3& vExts, const A3DVECTOR3& vVel, FLOAT fTime, AABBTRACERT* pTraceRt, int iTraceObj, DWORD dwUserData);
	//	Sphere trace routine, Return true if collision occured
	virtual bool SphereTrace(const A3DVECTOR3& vStart, float fRadius, A3DVECTOR3& vVel, FLOAT fTime, SPHERETRACERT* pTraceRt, int iTraceObj, DWORD dwUserData);
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

	//	Player trace
	bool PlayerTrace(const A3DVECTOR3& vStart, const A3DVECTOR3& vExts, const A3DVECTOR3& vVel, float fTime);
	//	Test ground
	bool TestGround(const A3DVECTOR3& vStart, const A3DVECTOR3& vExts, float fTestDepth);

protected:	//	Attributes

protected:	//	Operations

	//	AABB trace in managers
	bool AABBTraceInManagers(ECAABBTRACE* pTraceInfo, DWORD dwManMask);
	//	Ray trace in managers
	bool RayTraceInManagers(ECRAYTRACE* pTraceInfo, DWORD dwManMask);
	//	Sphere trace in managers
	bool SphereTraceInManagers(ECSPHERETRACE* pTraceInfo, DWORD dwManMask);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

