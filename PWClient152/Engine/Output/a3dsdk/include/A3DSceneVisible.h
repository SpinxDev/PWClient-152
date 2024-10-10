/*
 * FILE: A3DSceneVisible.h
 *
 * DESCRIPTION: Base class for scene visible solutions
 *
 * CREATED BY: duyuxin, 2003/8/22
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DSCENEVISIBLE_H_
#define _A3DSCENEVISIBLE_H_

#include "A3DTypes.h"
#include "A3DGeometry.h"

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

class A3DViewport;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class A3DSceneVisible
//
///////////////////////////////////////////////////////////////////////////

class A3DSceneVisible
{
public:		//	Types

	//	Return value of GetVisibleSurfs
	enum
	{
		GVS_OK = 0,			//	Ok, visible set was changed
		GVS_OK_NOCHANGE,	//	Ok, Visible set wasn't change
		GVS_INVALIDEYE,		//	Eye position is invalid, in solid or out of scene
		GVS_ERROR,			//	Error occur.
	};

public:		//	Constructors and Destructors

public:		//	Attributes

public:		//	Operations

	//	Release all resources
	virtual void Release() {}

	//	Get visible surfaces
	virtual DWORD GetVisibleSurfs(A3DViewport* pViewport, int** ppSurfs, int* piNumSurf) = 0;

	//	Check whether specified AABB is visible or not at current frame
	virtual bool AABBIsVisible(const A3DAABB& aabb) { return true; }
	//	Check whether specified point is visible or not at current frame
	virtual bool PointIsVisible(const A3DVECTOR3& vPos) { return true; }

	//	Force to recalculate visible set this frame
	virtual void RecalcVisibleSet() {} 

protected:	//	Attributes

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//
//	Class A3DBSP
//
///////////////////////////////////////////////////////////////////////////



#endif	//	_A3DSCENEVISIBLE_H_

