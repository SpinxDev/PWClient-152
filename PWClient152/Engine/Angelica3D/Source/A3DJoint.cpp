/*
 * FILE: A3DJoint.cpp
 *
 * DESCRIPTION: a set of classes which define the connection information between two bones.
 *				Joint is the object that a controller will control directly.
 *
 * CREATED BY: Hedi, 2003/7/2
 *
 * HISTORY:
 *
 * Copyright (c) 2003 Archosaur Studio, All Rights Reserved.	
 */

#include "A3DPlatform.h"
#include "A3DTypes.h"
#include "A3DBone.h"
#include "A3DJoint.h"
#include "A3DPI.h"
#include "A3DFuncs.h"
#include "A3DSkeleton.h"
#include "A3DTrackMan.h"
#include "AMemory.h"

///////////////////////////////////////////////////////////////////////////
//
//	Reference to External variables and functions
//
///////////////////////////////////////////////////////////////////////////

#define new A_DEBUG_NEW

///////////////////////////////////////////////////////////////////////////
//
//	Local Types and Variables and Global variables
//
///////////////////////////////////////////////////////////////////////////

using namespace A3D;

///////////////////////////////////////////////////////////////////////////
//
//	Local functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Implement A3DTrackMan
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
//
// class A3DJoint is the base class of joint and it contains both the sliding
// information (position) and the rotational information (orientation).
//
// A3DJoint is a free joint, and A3DControlledJoint is a limited joint.
//
///////////////////////////////////////////////////////////////////////////////
A3DJoint::A3DJoint()
{
	m_dwClassID		= A3D_CID_JOINT;
	m_iJointType	= JOINT_UNKNOWN;
	m_iParentBone	= -1;
	m_iChildBone	= -1;
	m_iSiblingJoint	= -1;
	m_pSkeleton		= NULL;
	m_dwFileNameID	= 0;
}

A3DJoint::~A3DJoint()
{
}

//	Initialize bone
bool A3DJoint::Init(A3DSkeleton* pSkeleton)
{
	m_pSkeleton = pSkeleton;
	return true;
}

//	Release bone
void A3DJoint::Release()
{
	m_pSkeleton	= NULL;
}

void A3DJoint::SetName(const char* szName)
{ 
	m_strName		= szName;
	m_dwFileNameID	= a_MakeIDFromString(szName);
}

bool A3DJoint::Update(int nDeltaTime)
{
/*	A3DMATRIX4 matRelativeTM = GetMatrix();

	//	First update the bone's relative tm
	if (m_iChildBone >= 0)
		m_pSkeleton->GetBone(m_iChildBone)->CombineRelativeTM(matRelativeTM);
*/
	//	then call this level's another joint update
	if (m_iSiblingJoint >= 0)
		m_pSkeleton->GetJoint(m_iSiblingJoint)->Update(nDeltaTime);

	return true;
}

A3DJoint* A3DJoint::NewJoint(DWORD dwType, bool bClassID)
{
	A3DJoint* pNewJoint = NULL;

	if (bClassID)
	{
		switch (dwType)
		{
		case A3D_CID_ANIMJOINT:			pNewJoint = new A3DAnimJoint();			break;
		case A3D_CID_CONTROLLEDJOINT:	pNewJoint = new A3DControlledJoint();	break;
		case A3D_CID_SPRINGJOINT:		pNewJoint = new A3DSpringJoint();		break;
		case A3D_CID_AXISSPRINGJOINT:	pNewJoint = new A3DAxisSpringJoint();	break;
		default:
			ASSERT(0);
			return NULL;
		}
	}
	else
	{
		switch (dwType)
		{
		case JOINT_ANIM:		pNewJoint = new A3DAnimJoint();			break;
		case JOINT_CONTROL:		pNewJoint = new A3DControlledJoint();	break;
		case JOINT_SPRING:		pNewJoint = new A3DSpringJoint();		break;
		case JOINT_AXISSPRING:	pNewJoint = new A3DAxisSpringJoint();	break;
		default:
			ASSERT(0);
			return NULL;
		}
	}

	return pNewJoint;
}

A3DBone* A3DJoint::GetParentBonePtr()
{ 
	return m_iParentBone < 0 ? NULL : m_pSkeleton->GetBone(m_iParentBone);
}

A3DBone* A3DJoint::GetChildBonePtr() 
{ 
	return m_iChildBone < 0 ? NULL : m_pSkeleton->GetBone(m_iChildBone);
}

A3DJoint* A3DJoint::GetSiblingJointPtr()
{ 
	return m_iSiblingJoint < 0 ? NULL : m_pSkeleton->GetJoint(m_iSiblingJoint);
}

bool A3DJoint::Load(AFile* pFile)
{
	//	Load joint name
	if (!pFile->ReadString(m_strName))
	{
		g_A3DErrLog.Log("A3DJoint::Load(), Failed to read joint name!");
		return false;
	}

	m_dwFileNameID = a_MakeIDFromString(m_strName);

	DWORD dwRead;
	JOINTDATA Data;

	if (!pFile->Read(&Data, sizeof (Data), &dwRead) || dwRead != sizeof (Data))
	{
		g_A3DErrLog.Log("A3DJoint::Load(), Failed to read joint data!");
		return false;
	}

	m_iParentBone	= Data.iParentBone;
	m_iChildBone	= Data.iChildBone;
	m_iSiblingJoint	= Data.iSiblingJoint;

	return true;
}

bool A3DJoint::Save(AFile* pFile)
{
	//	Write joint name
	if (!pFile->WriteString(m_strName))
	{
		g_A3DErrLog.Log("A3DJoint::Save(), Failed to write joint name!");
		return false;
	}

	DWORD dwWrite;
	JOINTDATA Data;

	Data.iParentBone	= m_iParentBone; 
	Data.iChildBone		= m_iChildBone; 
	Data.iSiblingJoint	= m_iSiblingJoint;

	if (!pFile->Write(&Data, sizeof (Data), &dwWrite) || dwWrite != sizeof (Data))
	{
		g_A3DErrLog.Log("A3DJoint::Save(), Failed to write joint data!");
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
//
// A3DAnimJoint is the joint that will get its data from a track not a controller
//
///////////////////////////////////////////////////////////////////////////////
A3DAnimJoint::A3DAnimJoint() : A3DJoint()
{
	m_dwClassID		= A3D_CID_ANIMJOINT;
	m_iJointType	= JOINT_ANIM;
}

A3DAnimJoint::~A3DAnimJoint()
{
}

//	Duplicate joint data
A3DJoint* A3DAnimJoint::Duplicate(A3DSkeleton* pDstSkeleton)
{
	A3DAnimJoint* pj = (A3DAnimJoint*)A3DJoint::NewJoint(A3DJoint::JOINT_ANIM, false);
	if (!pj)
	{
		g_A3DErrLog.Log("A3DAnimJoint::Duplicate, Not enough memory !");
		return NULL;
	}

	if (!pj->Init(pDstSkeleton))
	{
		delete pj;
		g_A3DErrLog.Log("A3DAnimJoint::Duplicate, Failed to init joint object");
		return NULL;
	}

	//	Set joint name
	pj->SetName(m_strName);

	pj->m_iParentBone	= m_iParentBone;
	pj->m_iChildBone	= m_iChildBone;
	pj->m_iSiblingJoint	= m_iSiblingJoint;

	return pj;
}

bool A3DAnimJoint::Update(int nDeltaTime)
{
	return A3DJoint::Update(nDeltaTime);
}

bool A3DAnimJoint::Load(AFile* pFile)
{
	if (!A3DJoint::Load(pFile))
		return false;

	return true;
}

bool A3DAnimJoint::Save(AFile* pFile)
{
	if (!A3DJoint::Save(pFile))
		return false;

	return true;
}

A3DMATRIX4 A3DAnimJoint::GetMatrix()
{
	return A3D::g_matIdentity;
}

///////////////////////////////////////////////////////////////////////////////
//
// A3DControlledJoint is the joint that will get its data from a controller not a track
//
///////////////////////////////////////////////////////////////////////////////

A3DControlledJoint::A3DControlledJoint() : A3DJoint()
{
	m_dwClassID		= A3D_CID_CONTROLLEDJOINT;
	m_iJointType	= JOINT_CONTROL;
	m_pController	= NULL;

	m_bXActive		= TRUE;
	m_bYActive		= TRUE;
	m_bZActive		= TRUE;

	m_bXLimited		= FALSE;
	m_bYLimited		= FALSE;
	m_bZLimited		= FALSE;

	m_vXPos			= 0.0f;
	m_vYPos			= 0.0f;
	m_vZPos			= 0.0f;

	m_vXAngle		= 0.0f;
	m_vYAngle		= 0.0f;
	m_vZAngle		= 0.0f;
}

A3DControlledJoint::~A3DControlledJoint()
{
	if( m_pController )
	{
		delete m_pController;
		m_pController = NULL;
	}
}

bool A3DControlledJoint::Update(int nDeltaTime)
{
	int nAbsTime = nDeltaTime; // + m_nTimeLast;

	if( m_pController )
	{
		// the controller will set this joint's parameter when it is doing its Update
		m_pController->Update(nDeltaTime);
	}

	return A3DJoint::Update(nDeltaTime);
}

bool A3DControlledJoint::Load(AFile* pFile)
{
	if (!A3DJoint::Load(pFile))
		return false;

	return true;
}

bool A3DControlledJoint::Save(AFile * pFile)
{
	if (!A3DJoint::Save(pFile))
		return false;

	return true;
}

A3DMATRIX4 A3DControlledJoint::GetMatrix()
{
	A3DMATRIX4 mat;

	mat = a3d_RotateX(m_vXAngle) * a3d_RotateY(m_vYAngle) * a3d_RotateZ(m_vZAngle);

	return mat;
}

///////////////////////////////////////////////////////////////////////////////
//
// Spring joint describe a joint between 2 bones connected via a single spring.
//
///////////////////////////////////////////////////////////////////////////////
A3DSpringJoint::A3DSpringJoint() : A3DControlledJoint()
{
	m_dwClassID			= A3D_CID_SPRINGJOINT;
	m_iJointType		= JOINT_SPRING;
	m_vSpringRest		= 0.0f;
	m_vSpringTension	= 1.0f;
}

A3DSpringJoint::~A3DSpringJoint()
{
}

bool A3DSpringJoint::Update(int nDeltaTime)
{
	return A3DControlledJoint::Update(nDeltaTime);
}

A3DMATRIX4 A3DSpringJoint::GetMatrix()
{
	return A3DControlledJoint::GetMatrix();
}

bool A3DSpringJoint::Load(AFile* pFile)
{
	if (!A3DJoint::Load(pFile))
		return false;

	return true;
}

bool A3DSpringJoint::Save(AFile* pFile)
{
	if (!A3DJoint::Save(pFile))
		return false;

	return true;
}

///////////////////////////////////////////////////////////////////////////////
//
// Spring axis joint describe a joint between 2 bones connected via a set of 
// springs on each axis, it can be used in sliding joints and rotation joints.
//
///////////////////////////////////////////////////////////////////////////////
A3DAxisSpringJoint::A3DAxisSpringJoint() : A3DControlledJoint()
{
	m_dwClassID			= A3D_CID_AXISSPRINGJOINT;
	m_iJointType		= JOINT_AXISSPRING;

	m_vXSpringRest		= 0.0f;
	m_vYSpringRest		= 0.0f;
	m_vZSpringRest		= 0.0f;

	m_vXSpringTension	= 1.0f;
	m_vYSpringTension	= 1.0f;
	m_vZSpringTension	= 1.0f;
}

A3DAxisSpringJoint::~A3DAxisSpringJoint()
{
}

bool A3DAxisSpringJoint::Update(int nDeltaTime)
{
	return A3DControlledJoint::Update(nDeltaTime);
}

A3DMATRIX4 A3DAxisSpringJoint::GetMatrix()
{
	return A3DControlledJoint::GetMatrix();
}

bool A3DAxisSpringJoint::Load(AFile* pFile)
{
	if (!A3DJoint::Load(pFile))
		return false;

	return true;
}

bool A3DAxisSpringJoint::Save(AFile* pFile)
{
	if (!A3DJoint::Save(pFile))
		return false;

	return true;
}

