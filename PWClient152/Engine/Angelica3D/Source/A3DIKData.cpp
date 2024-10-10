/*
 * FILE: A3DIKData.cpp
 *
 * DESCRIPTION: a set of classes act as IK data
 *
 * CREATED BY: Hedi, 2003/7/2
 *
 * HISTORY:
 *
 * Copyright (c) 2003 Archosaur Studio, All Rights Reserved.	
 */

#include "A3DIKData.h"

A3DIKChain::A3DIKChain()
{
	m_nMaxBones		= 0;
	m_nNumBones		= 0;

	m_ppBoneChain	= NULL;
	m_pBoneHead		= NULL;
	m_pBoneTail		= NULL;
}

A3DIKChain::~A3DIKChain()
{
}

bool A3DIKChain::Init(int nMaxBones)
{
	return true;
}

bool A3DIKChain::Release()
{
	return true;
}

bool A3DIKChain::AddBone(int nBoneIndex, A3DBone * pBone)
{
	return true;
}

bool A3DIKChain::RemoveBone(A3DBone * pBone)
{
	return true;
}

A3DIKGoal::A3DIKGoal()
{
	m_pPosTrack		= NULL;
}

A3DIKGoal::~A3DIKGoal()
{
	if( m_pPosTrack )
	{
	//	m_pPosTrack->Release();
		m_pPosTrack = NULL;
	}
}

A3DIKSwivelConstraint::A3DIKSwivelConstraint()
{
	m_pPosTrack		= NULL;
}

A3DIKSwivelConstraint::~A3DIKSwivelConstraint()
{
	if (m_pPosTrack)
	{
	//	m_pPosTrack->Release();
		m_pPosTrack = NULL;
	}
}
