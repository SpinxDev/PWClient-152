/*
 * FILE: EC_CDR.h
 *
 * DESCRIPTION:  A set of interfaces define the moving strategy of the avatar.
 *							Which focus on collision detection and give an appropriate response.
 *
 * CREATED BY: He wenfeng, 2005/3/8
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#pragma once
#include "A3DVector.h"

// for other player's movement
struct OtherPlayer_Move_Info
{
	// Bounding sphere of avator
	A3DVECTOR3	vCenter;
	A3DVECTOR3	vExts;
	float fStepHeight;

	// Velocity 
	A3DVECTOR3 vVelocity;

	// time span ( sec )
	float t;
	
	bool bTraceGround;	//	Whether trace the ground
	bool bTestTrnOnly;	//	Trace terrain only
	
	A3DVECTOR3 vecGroundNormal;	// if bTraceGround is true, this will contain the ground normal when returned
};

// for on-air move case
//@note  : change to AABB.  By Kuiwu[22/9/2005]
struct ON_AIR_CDR_INFO
{
	A3DVECTOR3	vCenter;
	A3DVECTOR3  vExtent;  
	
	// Hold a height from the surface of terrain or building
	float	fHeightThresh;
	
	// Velocity Info
	A3DVECTOR3 vVelDir;
	float fSpeed;

	// time span ( sec )
	float t;					

	//@note : SlopeThresh seems useless on air or under water. By Kuiwu[22/9/2005]
	// Slope Thresh
	//float fSlopeThresh;

	// Distance Thresh under the water surface
	float fUnderWaterDistThresh;

	A3DVECTOR3 vTPNormal;

	// On air or water, true if on air, false for on water case.
	bool bOnAir;				
	
	// After the move action is done, If the fHeightThresh 
	// still be satisfied, bMeetHeightThresh is set to true.
	bool bMeetHeightThresh;

	ON_AIR_CDR_INFO()
	{
		vTPNormal.Clear();
	}
};

// for on-ground move case
//@note  : change to AABB.  By Kuiwu[22/9/2005]
struct CDR_INFO
{
	//the aabb
	A3DVECTOR3	vCenter;
	//@note : the caller should make sure ext(.x, .y, .z) > 0.  By Kuiwu[22/9/2005]
	A3DVECTOR3  vExtent; 

	float	fStepHeight;
	
	// Velocity Info
	A3DVECTOR3 vXOZVelDir;
	float fYVel;
	float fSpeed;

	// time span ( sec )
	float t;					

	// Gravity acceleration
	float fGravityAccel;

	// the Climb Slope Thresh
	float fSlopeThresh;

	// Tangent plane Info
	A3DVECTOR3 vTPNormal;

	// Absolute Velocity: output for forcast!
	A3DVECTOR3 vAbsVelocity;


	//the moving dist
	float     fMoveDist;  
};

void OtherPlayerMove(OtherPlayer_Move_Info& OPMoveInfo);

void OnGroundMove(CDR_INFO& CDRInfo);

void OnAirMove(ON_AIR_CDR_INFO& OnAirCDRInfo);

// Trace the building and the ground
bool VertRayTrace(const A3DVECTOR3& vPos, A3DVECTOR3& vHitPos, A3DVECTOR3& vHitNormal, float DeltaY=1000.0f);
bool RayTrace(const A3DVECTOR3& vStart, const A3DVECTOR3& vDelta, A3DVECTOR3& vHitPos, A3DVECTOR3& vNormal, float& fraction);
bool VertAABBTrace(const A3DVECTOR3& vCenter, const A3DVECTOR3& vExt, A3DVECTOR3& vHitPos, A3DVECTOR3& vHitNormal, float DeltaY=1000.0f);


// only trace the building
bool RayTraceEnv(const A3DVECTOR3& vStart, const A3DVECTOR3& vDelta, A3DVECTOR3& vHitPos, A3DVECTOR3& vNormal, float& fraction );
bool VertRayTraceEnv(const A3DVECTOR3& vPos, A3DVECTOR3& vHitPos, A3DVECTOR3& vHitNormal, float DeltaY=1000.0f);

// test if the position lies in the valid submaps
bool IsPosInAvailableMap(const A3DVECTOR3& vPos);


//@desc :used to trace the environment, brush&terrain&water  By Kuiwu[8/10/2005]
struct env_trace_t
{
	A3DVECTOR3 vStart;  // brush start
	A3DVECTOR3 vExt;
	A3DVECTOR3 vDelta;
	A3DVECTOR3 vTerStart;
	A3DVECTOR3 vWatStart;
	DWORD      dwCheckFlag;
	bool       bWaterSolid;

	float      fFraction;
	A3DVECTOR3 vHitNormal;
	bool       bStartSolid;     //start in solid
	DWORD      dwClsFlag;       //collision flag
};

#define  CDR_BRUSH    0x1
#define  CDR_TERRAIN  0x2
#define  CDR_WATER    0x4

bool CollideWithEnv(env_trace_t * pEnvTrc);