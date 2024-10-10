/*
 * FILE: A3DIKSovler.cpp
 *
 * DESCRIPTION: a set of inverse kinematics solver classes
 *
 * CREATED BY: Hedi, 2003/7/2
 *
 * HISTORY:
 *
 * Copyright (c) 2003 Archosaur Studio, All Rights Reserved.	
 */

#include "A3DIKSolver.h"

// IK solver controller
A3DIKSolver::A3DIKSolver()
{
	m_pIKChain	= NULL;
	m_pIKGoal	= NULL;
	m_pIKSwivelConstraint = NULL;
}

// HI IK sovler
A3DHIIKSolver::A3DHIIKSolver() : A3DIKSolver()
{
}

A3DHIIKSolver::~A3DHIIKSolver()
{
}

// IK Limb
A3DIKLimbSolver::A3DIKLimbSolver() : A3DIKSolver()
{
}

A3DIKLimbSolver::~A3DIKLimbSolver()
{
}
