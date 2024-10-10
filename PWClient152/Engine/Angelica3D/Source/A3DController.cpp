/*
 * FILE: A3DController.cpp
 *
 * DESCRIPTION: a set of class which used for animation control
 *
 * CREATED BY: Hedi, 2003/7/2
 *
 * HISTORY:
 *
 * Copyright (c) 2003 Archosaur Studio, All Rights Reserved.	
 */

#include "A3DController.h"

//////////////////////////////////////////////////////////////////////////////////////
// class A3DController is the abstract base class of all animation control objects.
//
// animation controller is the data source that will create the animations.
// all controller will take some paramter as its control data, and keyframing these data,
// during runtime, the controller will use its keyframe data to do control jobs.
//
//////////////////////////////////////////////////////////////////////////////////////
A3DController::A3DController()
{
	m_pTargetJoint		= NULL;
}

//////////////////////////////////////////////////////////////////////////////////////
// Position controller
//////////////////////////////////////////////////////////////////////////////////////
A3DPosController::A3DPosController() : A3DController()
{
}

A3DPosController::~A3DPosController()
{
}

bool A3DPosController::Update(int nDeltaTime)
{
	return true;
}


//////////////////////////////////////////////////////////////////////////////////////
// Rotation controller
//////////////////////////////////////////////////////////////////////////////////////
A3DRotController::A3DRotController() : A3DController()
{
}

A3DRotController::~A3DRotController()
{
}

bool A3DRotController::Update(int nDeltaTime)
{
	return true;
}
