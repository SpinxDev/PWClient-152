/*
 * FILE: A3DIKSovler.h
 *
 * DESCRIPTION: a set of inverse kinematics solver classes
 *
 * CREATED BY: Hedi, 2003/7/2
 *
 * HISTORY:
 *
 * Copyright (c) 2003 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DIKSOLVER_H_
#define _A3DIKSOLVER_H_

#include "A3DTypes.h"
#include "A3DIKData.h"
#include "A3DController.h"

// IK solver controller
class A3DIKSolver : public A3DController
{
protected:
	int						m_nNumTargetJoints;		// number of target joints.
	A3DControlledJoint *	m_pTargetJoints;		// target joints' buffer
	A3DIKChain *			m_pIKChain;				// IK chain
	A3DIKGoal *				m_pIKGoal;				// IK goal
	A3DIKSwivelConstraint *	m_pIKSwivelConstraint;	// IK constraint.

public:
	A3DIKSolver();
	virtual ~A3DIKSolver() = 0;
};

// HI IK sovler
class A3DHIIKSolver : public A3DIKSolver
{
private:
protected:
public:
	A3DHIIKSolver();
	virtual ~A3DHIIKSolver();
};

// IK Limb
class A3DIKLimbSolver : public A3DIKSolver
{
private:

protected:

public:
	A3DIKLimbSolver();
	virtual ~A3DIKLimbSolver();
};

#endif//_A3DIKSOLVER_H_