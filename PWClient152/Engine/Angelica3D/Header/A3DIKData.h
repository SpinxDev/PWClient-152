/*
 * FILE: A3DIKData.h
 *
 * DESCRIPTION: a set of classes act as IK data
 *
 * CREATED BY: Hedi, 2003/7/2
 *
 * HISTORY:
 *
 * Copyright (c) 2003 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DIKDATA_H_
#define _A3DIKDATA_H_

#include "A3DTypes.h"
#include "A3DTrackData.h"

class A3DBone;
class A3DIKChain
{
private:
	int				m_nMaxBones;
	int				m_nNumBones;

	A3DBone **		m_ppBoneChain;
	A3DBone *		m_pBoneHead;
	A3DBone *		m_pBoneTail;

protected:
public:
	A3DIKChain();
	~A3DIKChain();

	///////////////////////////////////////////////////////////////////////////
	// Initialize and Finalize methods.
	//
	// bool Init(...)
	//	allocate the memory needed by this IK chain.
	// nNumBones		IN			the number of bones in this IK chain.
	// return true if success and false on fail.
	///////////////////////////////////////////////////////////////////////////
	bool Init(int nMaxBones);
	bool Release();

	bool AddBone(int nBoneIndex, A3DBone * pBone);
	bool RemoveBone(A3DBone * pBone);
};

class A3DIKGoal
{
private:

	A3DVector3Track*	m_pPosTrack;

protected:

public:
	A3DIKGoal();
	~A3DIKGoal();
};

class A3DIKSwivelConstraint
{
private:
	
	A3DVector3Track*	m_pPosTrack;

protected:
public:
	A3DIKSwivelConstraint();
	~A3DIKSwivelConstraint();
};

#endif//_A3DIKDATA_H_