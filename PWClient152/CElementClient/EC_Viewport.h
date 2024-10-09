/*
 * FILE: GL_Viewport.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2003/12/16
 *
 * HISTORY: 
 *
 * Copyright (c) 2003 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include "A3DTypes.h"
#include "ARect.h"

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
class A3DCamera;
class A3DOrthoCamera;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CECViewport
//	
///////////////////////////////////////////////////////////////////////////

class CECViewport
{
public:	//	Types

	//	Initial paramters
	struct INITPARAMS
	{
		ARectI		rcView;			//	Viewport position and size
		float		fMinZ;			//	Minimum z value
		float		fMaxZ;			//	Maximum z value
		bool		bClearColor;	//	true, clear color buffer
		bool		bClearZ;		//	true, clear Z buffer
		A3DCOLOR	colClear;		//	Clear color
		float		fFOV;			//	Camera FOV in radian
		float		fNearPlane;		//	Near plane
		float		fFarPlane;		//	Far plane
		
		bool		bOrthoCamera;	//	true, create another ortho camera
		float		fOCLeft;
		float		fOCTop;
		float		fOCRight;
		float		fOCBottom;
		float		fOCZNear;
		float		fOCZFar;
	};

public:	//	Constructor and Destructor

	CECViewport();
	virtual ~CECViewport();

public:		//	Attributes

public:		//	Operations

	//	Initialize object
	bool Init(const INITPARAMS& Params);

	//	Initialize from an A3DViewport;
	bool InitFromA3D(A3DViewport * pA3DViewport);

	//	Activate viewport
	bool Activate(bool bClear);
	//	Switch camera
	bool SwitchCamera(bool bOrthoCamera);

	//	Move viewport
	bool Move(int x, int y, int iWidth, int iHeight, bool bWideScreen);

	//	Get interface
	A3DViewport* GetA3DViewport() { return m_pA3DViewport; }
	A3DCamera* GetA3DCamera() { return m_pA3DCamera; }
	A3DOrthoCamera* GetA3DOrthoCamera() { return m_pA3DOrthoCamera; }

protected:	//	Attributes

	A3DViewport*	m_pA3DViewport;
	A3DCamera*		m_pA3DCamera;
	A3DOrthoCamera*	m_pA3DOrthoCamera;

	bool			m_bWrapperOnly;		// flag indicates it's just a wrapper class for a A3DViewport
protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

