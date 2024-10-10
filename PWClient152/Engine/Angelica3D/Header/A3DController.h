/*
 * FILE: A3DController.h
 *
 * DESCRIPTION: a set of class which used for controlled joint
 *
 * CREATED BY: Hedi, 2003/7/2
 *
 * HISTORY:
 *
 * Copyright (c) 2003 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DCONTROLLER_H_
#define _A3DCONTROLLER_H_

#include "A3DTypes.h"

class A3DControlledJoint;
//////////////////////////////////////////////////////////////////////////////////////
// class A3DController is the abstract base class of all logic control objects.
//
// These controllers are mainly used for logic programs to control the behavior of
// a joint, their control targets are A3DControlledJoint, but can have many controlled 
// joints if needed.
//
//////////////////////////////////////////////////////////////////////////////////////
class A3DController
{
private:
	A3DControlledJoint *	m_pTargetJoint;

protected:
public:
	A3DController();
	virtual ~A3DController() = 0;

	///////////////////////////////////////////////////////////////////////////////
	// state control methods.
	//
	// bool Update(...)
	//  update the controller's state to some specific time
	// 
	// nDeltaTime	IN		delta time from last update
	///////////////////////////////////////////////////////////////////////////////
	virtual bool Update(int nDeltaTime) = 0;
};

// Position controller
class A3DPosController : public A3DController
{
private:

protected:

public:
	A3DPosController();
	virtual ~A3DPosController();

	///////////////////////////////////////////////////////////////////////////////
	// state control methods.
	//
	// bool Update(...)
	//  update the controller's state to some specific time
	// 
	// nDeltaTime	IN		delta time from last update
	///////////////////////////////////////////////////////////////////////////////
	virtual bool Update(int nDeltaTime);
};

// Rotation controller
class A3DRotController : public A3DController
{
private:

protected:
public:
	A3DRotController();
	virtual ~A3DRotController();

	///////////////////////////////////////////////////////////////////////////////
	// state control methods.
	//
	// bool Update(...)
	//  update the controller's state to some specific time
	// 
	// nDeltaTime	IN		delta time from last update
	///////////////////////////////////////////////////////////////////////////////
	virtual bool Update(int nDeltaTime);
};

class A3DLookAtController : public A3DController
{
private:
	A3DVECTOR3			m_vecTargetPos;

protected:
public:
	A3DLookAtController();
	virtual ~A3DLookAtController();

	///////////////////////////////////////////////////////////////////////////////
	// state control methods.
	//
	// bool Update(...)
	//  update the controller's state to some specific time
	// 
	// nDeltaTime	IN		delta time from last update
	///////////////////////////////////////////////////////////////////////////////
	virtual bool Update(int nDeltaTime);
};


#endif//_A3DCONTROLLER_H_
