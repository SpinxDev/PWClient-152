/*
 * FILE: GL_CDS.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/1/12
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "EC_Global.h"
#include "EC_CDS.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_World.h"
#include "EC_Viewport.h"
#include "EC_Manager.h"
#include "EC_InputCtrl.h"

#include "A3DCDS.h"
#include "A3DTerrain2.h"
#include "A3DCamera.h"

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

//	Visibility check flags used by PosIsVisible() and AABBIsVisible()
#define CKVIS_RAYTRACE		0x0004
#define CKVIS_CAMERA		0x0008

//	Trace flags used by RayTrace() and AABBTrace()
#define CKTRACE_TERRAIN		0x0001
#define CKTRACE_BUILDING	0x0002
#define CKTRACE_SCENE		0x0003		//	CKTRACE_TERRAIN | CKTRACE_BUILDING
#define CKTRACE_OBJECT		0x0004
#define CKTRACE_PLAYER		0x0008

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


///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECCDS
//	
///////////////////////////////////////////////////////////////////////////

CECCDS::CECCDS()
{
	m_RayTraceRt.iEntity	= ECENT_NONE;
	m_AABBTraceRt.iEntity	= ECENT_NONE;
}

CECCDS::~CECCDS()
{
}

//	Ray trace routine, Return true if collision occured
bool CECCDS::RayTrace(const A3DVECTOR3& vStart, const A3DVECTOR3& vVel, FLOAT fTime, 
					  RAYTRACERT* pTraceRt, int iTraceObj, DWORD dwUserData)
{
	DWORD dwFlags = 0;
	DWORD dwManMask = 0;
	
	bool bPressShift = (iTraceObj & 0x80000000) ? true : false;
//	bool bPressAlt = (iTraceObj & 0x40000000) ? true : false;
	bool bPressAlt = (GetAsyncKeyState(VK_MENU) & 0x8000) ? true : false;
	iTraceObj &= 0x00ffffff;

	switch (iTraceObj)
	{
	case TRACEOBJ_LIGHT:
	case TRACEOBJ_PARTICAL:
	case TRACEOBJ_RAIN:	
		
		dwFlags = CKTRACE_SCENE;
		break;

	case TRACEOBJ_TESTRAY:

		dwFlags		= CKTRACE_SCENE | CKTRACE_OBJECT;
		dwManMask	= MANMASK_PLAYER | MANMASK_NPC | MANMASK_MATTER | MANMASK_ORNAMENT;
		break;

	case TRACEOBJ_LBTNCLICK:

		dwFlags		= CKTRACE_SCENE | CKTRACE_OBJECT;
		dwManMask	= MANMASK_PLAYER | MANMASK_NPC | MANMASK_MATTER | MANMASK_ORNAMENT;
		break;

	case TRACEOBJ_RBTNCLICK:

		dwFlags		= CKTRACE_SCENE | CKTRACE_OBJECT;
		dwManMask	= MANMASK_PLAYER | MANMASK_ORNAMENT;
		break;

	case TRACEOBJ_CAMERA:

		dwFlags		= CKTRACE_SCENE;
		dwManMask	= 0;
		break;
		
	case TRACEOBJ_SHADOW:
		
		dwFlags		= CKTRACE_OBJECT;
		dwManMask	= MANMASK_ORNAMENT;
		break;
		
	case TRACEOBJ_SCENE:
		
		dwFlags		= CKTRACE_SCENE | CKTRACE_OBJECT;
		dwManMask	= MANMASK_ORNAMENT;
		break;

	default:
		return false;
	}

	m_RayTraceRt.iEntity	= ECENT_NONE;

	pTraceRt->fFraction = 1.0f;
	pTraceRt->vPoint	= vStart + vVel * fTime;
	pTraceRt->vHitPos	= pTraceRt->vPoint;

	if (dwFlags & CKTRACE_SCENE)
	{
		//	Trace in scene
		RAYTRACERT TraceRt;
		A3DTerrain2* pTerrain = g_pGame->GetGameRun()->GetWorld()->GetTerrain();

		if (pTerrain)
		{
			if (pTerrain->RayTrace(vStart, vVel, fTime, &TraceRt) && 
				TraceRt.fFraction < pTraceRt->fFraction)
			{
				*pTraceRt = TraceRt;

				m_RayTraceRt.fFraction	= TraceRt.fFraction;
				m_RayTraceRt.vNormal	= TraceRt.vNormal;
				m_RayTraceRt.iEntity	= ECENT_TERRAIN;
				m_RayTraceRt.iObjectID	= 0;
			}
		}
	}

	//	Fill Ray trace structure
	ECRAYTRACE RayTrace;

	RayTrace.vStart			= vStart;
	RayTrace.vDelta			= vVel * fTime;
	RayTrace.iTraceObj		= iTraceObj;
	RayTrace.dwUserData		= dwUserData;
	RayTrace.pTraceRt		= pTraceRt;
	RayTrace.pECTraceRt		= &m_RayTraceRt;
	RayTrace.dwKeyStates	= 0;

	if (bPressShift)
		RayTrace.dwKeyStates |= EC_KSF_SHIFT;

	if (bPressAlt)
		RayTrace.dwKeyStates |= EC_KSF_ALT;

	if (dwFlags & CKTRACE_OBJECT)
	{
		//	Trace in managers
		RayTraceInManagers(&RayTrace, dwManMask);
	}

	if (pTraceRt->fFraction < 1.0f)
		return true;

	return false;
}

//	AABB trace routine, Return true if collision occured
bool CECCDS::AABBTrace(const A3DVECTOR3& vStart, const A3DVECTOR3& vExts, const A3DVECTOR3& vVel, 
					   FLOAT fTime, AABBTRACERT* pTraceRt, int iTraceObj, DWORD dwUserData)
{
	DWORD dwFlags = 0;
	DWORD dwManMask = 0;

	return false;
/*	switch (iTraceObj)
	{
	default:
		return false;
	}
*/
	m_AABBTraceRt.iEntity	= ECENT_NONE;

	pTraceRt->fFraction = 1.0f;
	pTraceRt->vDestPos	= vStart + vVel * fTime;

	//	Biild trace info.
	AABBTRACEINFO TraceInfo;
	TRA_AABBTraceInit(&TraceInfo, vStart, vExts, vVel, fTime);

	if (dwFlags & CKTRACE_SCENE)
	{
		//	Trace in scene
	//	AABBTRACERT TraceRt;
	}

	//	Fill AABB trace structure
	ECAABBTRACE AABBTrace;

	AABBTrace.pInfo			= &TraceInfo;
	AABBTrace.iTraceObj		= iTraceObj;
	AABBTrace.dwUserData	= dwUserData;
	AABBTrace.pTraceRt		= pTraceRt;
	AABBTrace.pECTraceRt	= &m_AABBTraceRt;

	if (dwFlags & CKTRACE_OBJECT)
	{
		//	Trace in managers
		AABBTraceInManagers(&AABBTrace, dwManMask);
	}

	if (pTraceRt->fFraction < 1.0f)
		return true;

	return false;
}

//	Sphere trace routine, Return true if collision occured
bool CECCDS::SphereTrace(const A3DVECTOR3& vStart, float fRadius, A3DVECTOR3& vVel, FLOAT fTime, SPHERETRACERT* pTraceRt, int iTraceObj, DWORD dwUserData)
{
	DWORD dwFlags = 0;

	switch (iTraceObj)
	{
	case TRACEOBJ_CAMERA:
		
		dwFlags = CKTRACE_SCENE | CKTRACE_OBJECT;
		break;

	default:
		return false;
	}

	pTraceRt->fFraction = 1.0f;
	pTraceRt->vDestPos	= vStart + vVel * fTime;

	//	Biild trace info.
	SPHERETRACEINFO TraceInfo;
	TRA_SphereTraceInit(&TraceInfo, vStart, fRadius, vVel, fTime);

	if (dwFlags & CKTRACE_SCENE)
	{
		//	Trace in scene
	//	SPHERETRACERT TraceRt;
	}

	if (pTraceRt->fFraction < 1.0f)
		return true;

	return false;
}

//	AABB trace in managers
bool CECCDS::AABBTraceInManagers(ECAABBTRACE* pTraceInfo, DWORD dwManMask)
{
/*	CGLWndGame* pGameWnd = g_pGame->GetGameWnd();
	CGLManager* pRetManager = NULL;

	for (int i=0; i < GLMAN_NUM; i++)
	{
		if (dwManMask & (1 << i))
		{
			CGLManager* pManager = pGameWnd->GetManager(i);
			if (pManager->AABBTrace(pTraceInfo))
				pRetManager = pManager;
		}
	}

	return pRetManager;
*/
	return NULL;
}

//	Ray trace in managers
bool CECCDS::RayTraceInManagers(ECRAYTRACE* pTraceInfo, DWORD dwManMask)
{
	CECWorld* pWorld = g_pGame->GetGameRun()->GetWorld();
	bool bRet = false;

	for (int i=0; i < NUM_MANAGER; i++)
	{
		if (dwManMask & (1 << i))
		{
			CECManager* pManager = pWorld->GetManager(i);
			if (pManager->RayTrace(pTraceInfo))
				bRet = true;
		}
	}

	return bRet;
}

//	Sphere trace in managers
bool CECCDS::SphereTraceInManagers(ECSPHERETRACE* pTraceInfo, DWORD dwManMask)
{
/*	CGLWndGame* pGameWnd = g_pGame->GetGameWnd();
	CGLManager* pRetManager = NULL;

	for (int i=0; i < GLMAN_NUM; i++)
	{
		if (dwManMask & (1 << i))
		{
			CGLManager* pManager = pGameWnd->GetManager(i);
			if (pManager->SphereTrace(pTraceInfo))
				pRetManager = pManager;
		}
	}

	return pRetManager;
*/
	return NULL;
}

//	Player trace
bool CECCDS::PlayerTrace(const A3DVECTOR3& vStart, const A3DVECTOR3& vExts, 
						 const A3DVECTOR3& vVel, float fTime)
{
	m_PlayerTraceRt.iEntity		= ECENT_NONE;
	m_PlayerTraceRt.fFraction	= 1.0f;

	//	Biild trace info.
	AABBTRACEINFO TraceInfo;
	TRA_AABBTraceInit(&TraceInfo, vStart, vExts, vVel, fTime);

	//	Fill AABB trace structure
	ECAABBTRACE AABBTrace;
	AABBTRACERT AABBTraceRt;

	AABBTrace.pInfo			= &TraceInfo;
	AABBTrace.iTraceObj		= TRACEOBJ_PLAYER;
	AABBTrace.dwUserData	= 0;
	AABBTrace.pTraceRt		= &AABBTraceRt;
	AABBTrace.pECTraceRt	= &m_PlayerTraceRt;

	//	Trace in managers
	DWORD dwManMask = MANMASK_PLAYER | MANMASK_NPC;
	AABBTraceInManagers(&AABBTrace, dwManMask);

	//	Check terrain collision using ray trace instead of AABB trace
	A3DVECTOR3 vPos = vStart - A3DVECTOR3(0.0f, vExts.y, 0.0f);
	A3DTerrain2* pTerrain = g_pGame->GetGameRun()->GetWorld()->GetTerrain();
	if (pTerrain)
	{
		RAYTRACERT RayTraceRt;
		RayTraceRt.fFraction = 1.0f;

		if (pTerrain->RayTrace(vPos, vVel, fTime, &RayTraceRt))
		{
			A3DVECTOR3 vDelta = vVel * -fTime;
			float fDot = DotProduct(vDelta, RayTraceRt.vNormal);
			if (fDot > 0.0f)
			{
				RayTraceRt.fFraction -= 0.1f / fDot;
				a_ClampFloor(RayTraceRt.fFraction, 0.0f);
			}
		}

		if (RayTraceRt.fFraction < m_PlayerTraceRt.fFraction)
		{
			m_PlayerTraceRt.fFraction	= RayTraceRt.fFraction;
			m_PlayerTraceRt.iEntity		= ECENT_TERRAIN;
			m_PlayerTraceRt.vNormal		= RayTraceRt.vNormal;
			m_PlayerTraceRt.iObjectID	= 0;
		}
	}

	if (m_PlayerTraceRt.fFraction < 1.0f)
		return true;

	return false;
}

//	Test ground
bool CECCDS::TestGround(const A3DVECTOR3& vStart, const A3DVECTOR3& vExts, float fTestDepth)
{
	m_PlayerTraceRt.iEntity		= ECENT_NONE;
	m_PlayerTraceRt.fFraction	= 1.0f;

	A3DVECTOR3 vVel(0.0f, -fTestDepth, 0.0f);

	//	Biild trace info.
	AABBTRACEINFO TraceInfo;
	TRA_AABBTraceInit(&TraceInfo, vStart, vExts, vVel, 1.0f);

	//	Fill AABB trace structure
	ECAABBTRACE AABBTrace;
	AABBTRACERT AABBTraceRt;

	AABBTrace.pInfo			= &TraceInfo;
	AABBTrace.iTraceObj		= TRACEOBJ_PLAYER;
	AABBTrace.dwUserData	= 0;
	AABBTrace.pTraceRt		= &AABBTraceRt;
	AABBTrace.pECTraceRt	= &m_PlayerTraceRt;

	//	Trace in managers
	DWORD dwManMask = MANMASK_PLAYER | MANMASK_NPC;
	AABBTraceInManagers(&AABBTrace, dwManMask);

	//	Check terrain collision using ray trace instead of AABB trace
	A3DTerrain2* pTerrain = g_pGame->GetGameRun()->GetWorld()->GetTerrain();
	if (pTerrain)
	{
		A3DVECTOR3 vNormal;
		float fHei = pTerrain->GetPosHeight(vStart, &vNormal);
		float fOff = vStart.y - vExts.y - fHei;
		float fFrac = 1.0f;

		if (fOff < fTestDepth && fOff > -1.0f)
		{
			if (fOff <= 0.0f)
				fFrac = 0.0f;
			else
				fFrac = fFrac / fTestDepth;

			if (fFrac < m_PlayerTraceRt.fFraction)
			{
				m_PlayerTraceRt.fFraction	= fFrac;
				m_PlayerTraceRt.iEntity		= ECENT_TERRAIN;
				m_PlayerTraceRt.vNormal		= vNormal;
				m_PlayerTraceRt.iObjectID	= 0;
			}
		}
	}

	if (m_PlayerTraceRt.fFraction < 1.0f)
		return true;

	return false;
}

//	Split mark routine
bool CECCDS::SplitMark(const A3DAABB& aabb, const A3DVECTOR3& vNormal, A3DLVERTEX* aVerts, 
					   WORD* aIndices, bool bJog, int* piNumVert, int* piNumIdx, float fRadiusScale/* 0.2f */)
{
	return true;
}

//	Return true if this CDS can split mark
bool CECCDS::CanSplitMark()
{
	return false;
}

//	Position is visible ?
bool CECCDS::PosIsVisible(const A3DVECTOR3& vPos, int iVisObj, DWORD dwUserData)
{
	DWORD dwFlags = 0;

	switch (iVisObj)
	{
	case VISOBJ_LAMP:		dwFlags = CKVIS_CAMERA | CKVIS_RAYTRACE;	break;
	case VISOBJ_PARTICAL:	dwFlags = CKVIS_CAMERA;	break;
	case VISOBJ_SKINMODEL:	
	case VISOBJ_MODEL:		dwFlags = CKVIS_CAMERA;	break;
	default:
		return false;
	}

	if (dwFlags & CKVIS_CAMERA)
	{
		A3DCamera* pCamera = g_pGame->GetViewport()->GetA3DCamera();
		if (!pCamera->PointInViewFrustum(vPos))
			return false;
	}

	if (dwFlags & CKVIS_RAYTRACE)
	{
		A3DVECTOR3 vDest = g_pGame->GetViewport()->GetA3DCamera()->GetPos();

		RAYTRACERT TraceRt;
		if (RayTrace(vPos, vDest - vPos, 1.0f, &TraceRt, TRACEOBJ_LIGHT, dwUserData))
			return false;
	}

	return true;
}

//	AABB is visible ?
bool CECCDS::AABBIsVisible(const A3DAABB& aabb, int iVisObj, DWORD dwUserData)
{
	DWORD dwFlags = 0;

	switch (iVisObj)
	{
	case VISOBJ_LAMP:		dwFlags = CKVIS_CAMERA;	break;
	case VISOBJ_PARTICAL:	dwFlags = CKVIS_CAMERA;	break;
	case VISOBJ_SKINMODEL:	
	case VISOBJ_MODEL:		dwFlags = CKVIS_CAMERA;	break;
	case VISOBJ_MATTER:		dwFlags = CKVIS_CAMERA;	break;
	default:
		return false;
	}

	if (dwFlags & CKVIS_CAMERA)
	{
		A3DCamera* pCamera = g_pGame->GetViewport()->GetA3DCamera();
		if (!pCamera->AABBInViewFrustum(aabb))
			return false;
	}

	return true;
}

//	AABB is visible ?
bool CECCDS::AABBIsVisible(const A3DVECTOR3& vMins, const A3DVECTOR3& vMaxs, int iVisObj, DWORD dwUserData)
{
	A3DAABB aabb(vMins, vMaxs);
	return AABBIsVisible(aabb, iVisObj, dwUserData);
}

