/*
 * FILE: A3DSceneHull.h
 *
 * DESCRIPTION: Base class for scene collision solutions
 *
 * CREATED BY: duyuxin, 2003/8/21
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DSCENEHULL_H_
#define _A3DSCENEHULL_H_

#include "A3DTrace.h"
#include "A3DVertex.h"

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

class A3DEngine;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class A3DSceneHull
//
///////////////////////////////////////////////////////////////////////////

class A3DSceneHull
{
public:		//	Types

	enum
	{
		MAXNUM_MARKVERT		= 256,	//	Maximum number of vertices of splited mark
		MAXNUM_MARKINDEX	= 512	//	Maximum number of indices of splited mark
	};

public:		//	Constructors and Destructors

	A3DSceneHull()
	{ 
		m_pA3DEngine			= NULL;
		m_bRayTraceEnable		= true;
		m_bAABBTraceEnable		= true;
		m_bSphereTraceEnable	= true;
	}

	virtual ~A3DSceneHull() {}

public:		//	Attributes

public:		//	Operations

	//	Release all resources
	virtual void Release() {}

	//	Do ray trace
	virtual bool RayTrace(RAYTRACERT* pTraceRt, const A3DVECTOR3& vStart, const A3DVECTOR3& vVelocity, FLOAT fTime)
	{ 
		pTraceRt->fFraction = 1.0f;
		return false;
	}

	//	Do AABB trace
	virtual bool AABBTrace(AABBTRACERT* pTraceRt, AABBTRACEINFO* pInfo)
	{ 
		pTraceRt->fFraction = 1.0f;
		return false;
	}

	//	Do sphere trace
	virtual bool SphereTrace(SPHERETRACERT* pTraceRt, SPHERETRACEINFO* pInfo)
	{
		pTraceRt->fFraction = 1.0f;
		return false;
	}

	//	Split explosion mark
	virtual bool SplitMark(const A3DAABB& aabb, const A3DVECTOR3& vNormal, A3DLVERTEX* aVerts, WORD* aIndices, 
						   bool bJog, int* piNumVert, int* piNumIdx, float fRadiusScale=0.2f)
	{
		*piNumVert = 0;
		*piNumIdx  = 0;
		return true;
	}

	void SetRayTraceEnable(bool bEnable) { m_bRayTraceEnable = bEnable; }
	void SetAABBTraceEnable(bool bEnable) { m_bAABBTraceEnable = bEnable; }
	void SetSphereTraceEnable(bool bEnable) { m_bSphereTraceEnable = bEnable; }
	bool GetRayTraceEnable() { return m_bRayTraceEnable; }
	bool GetAABBTraceEnable() { return m_bAABBTraceEnable; }
	bool GetSphereTraceEnable() { return m_bSphereTraceEnable; }

	//	Set A3D engine
	void SetA3DEngine(A3DEngine* pA3DEngine) { m_pA3DEngine = pA3DEngine; }

protected:	//	Attributes

	A3DEngine*	m_pA3DEngine;		//	A3D Engine

	bool	m_bRayTraceEnable;		//	true, enable ray trace;
	bool	m_bAABBTraceEnable;		//	true, enable AABB trace;
	bool	m_bSphereTraceEnable;	//	true, enable sphere trace
};



#endif	//	_A3DSCENEHULL_H_

