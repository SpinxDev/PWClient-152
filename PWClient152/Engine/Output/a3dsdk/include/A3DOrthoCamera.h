/*
 * FILE: A3DOrthoCamera.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/6/12
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#ifndef _A3DORTHOCAMERA_H_
#define _A3DORTHOCAMERA_H_

#include "A3DCameraBase.h"

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


///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class A3DOrthoCamera
//	
///////////////////////////////////////////////////////////////////////////

class A3DOrthoCamera : public A3DCameraBase
{
public:		//	Types

public:		//	Constructor and Destructor

	A3DOrthoCamera();
	virtual ~A3DOrthoCamera() {}

public:		//	Attributes

public:		//	Operations

	//	Initialize camera object
	bool Init(A3DDevice* pA3DDevice, float l, float r, float b, float t, float zn, float zf);
	//	Release object
	virtual void Release();

	//	Set / Get projection parameters
	bool SetProjectionParam(float l, float r, float b, float t, float zn, float zf);
	float GetLeft() { return m_fLeft; }
	float GetRight() { return m_fRight; }
	float GetTop() { return m_fTop; }
	float GetBottom() { return m_fBottom; }

	/*	Calculate view frustum's 8 corners' position

		Return number of corners;

		aCorners (out): array used to receive corners position, you can pass NULL to get how
				many corners by return value. 
				corner index is defined as below:
				0 - 3: near plane corners, left-bottom, left-top, right-top, right-bottom
				4 - 7: far plane corners, left-bottom, left-top, right-top, right-bottom
		bInViewSpace: true, positions are in view space; false, in world space
		fNear, fFar: near and far distance.
	*/
	virtual int CalcFrustumCorners(A3DVECTOR3* aCorners, bool bInViewSpace, float fNear, float fFar);
protected:	//	Attributes
	
	float	m_fLeft;
	float	m_fRight;
	float	m_fTop;
	float	m_fBottom;

protected:	//	Operations

	//	Update project TM
	virtual bool UpdateProjectTM();
	//	Create view frustum
	virtual bool CreateViewFrustum();
	//	Update world frustum
	virtual bool UpdateWorldFrustum();
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

#endif	//	_A3DORTHOCAMERA_H_
