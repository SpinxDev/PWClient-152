/*
 * FILE: A3DAbsTrackMaker.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2007/1/29
 *
 * HISTORY: 
 *
 * Copyright (c) 2007 Archosaur Studio, All Rights Reserved.
 */

#include "A3DPI.h"
#include "A3DAbsTrackMaker.h"
#include "A3DSkeleton.h"
#include "A3DTrackMan.h"
#include "A3DJoint.h"
#include "A3DBone.h"
#include "AMemory.h"

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

#define new A_DEBUG_NEW

///////////////////////////////////////////////////////////////////////////
//	
//	Reference to External variables and functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local Types and Variables and Global variables
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Implement A3DAbsTrackMaker
//	
///////////////////////////////////////////////////////////////////////////

A3DAbsTrackMaker::A3DAbsTrackMaker(A3DSkeleton* pSkeleton, A3DSklTrackSet* pTrackSet)
{
	m_pSkeleton	= pSkeleton;
	m_pTrackSet	= pTrackSet;
}

A3DAbsTrackMaker::~A3DAbsTrackMaker()
{
}

//	Make absolute track
bool A3DAbsTrackMaker::Make(int iNumJoint, int* aJoints)
{
	if (!m_pSkeleton || !m_pTrackSet)
		return false;

	APtrArray<A3DSklTrackSet::BONETRACK*> aCandidates;
	int i, iValidCand = 0;

	int iNumKey = m_pTrackSet->GetAnimEndFrame() - m_pTrackSet->GetAnimStartFrame() + 1;
	int iStartTime = m_pTrackSet->FrameToTime(m_pTrackSet->GetAnimStartFrame());
	int iEndTime = m_pTrackSet->FrameToTime(m_pTrackSet->GetAnimEndFrame());

	//	Create candidate tracks
	for (i=0; i < iNumJoint; i++)
	{
		A3DSklTrackSet::BONETRACK* pTrack = m_pTrackSet->GetBoneTrack(aJoints[i]);
		if (!pTrack)
		{
			aCandidates.Add(NULL);
			continue;
		}

		if (pTrack->pAbsPosTrack || pTrack->pAbsRotTrack)
		{
			aCandidates.Add(NULL);
			continue;	//	Absolute tracks have been created
		}

		A3DVector3Track* pAbsPosTck = new A3DVector3Track;
		A3DQuaternionTrack* pAbsRotTck = new A3DQuaternionTrack;
		if (!pAbsPosTck || !pAbsRotTck)
			return false;

		if (!pAbsPosTck->Create(iNumKey, m_pTrackSet->GetAnimFPS(), 1) ||
			!pAbsRotTck->Create(iNumKey, m_pTrackSet->GetAnimFPS(), 1))
		{
			g_A3DErrLog.Log("A3DAbsTrackMaker::Make, Failed to create tracks !");
			delete pAbsPosTck;
			delete pAbsRotTck;
			return false;
		}

		A3DVector3Track::SEGMENT seg1;
		seg1.iStartTime	= iStartTime;
		seg1.iEndTime	= iEndTime;
		seg1.iStartKey	= 0;
		seg1.iEndKey	= iNumKey - 1;

		A3DQuaternionTrack::SEGMENT seg2;
		seg2.iStartTime	= iStartTime;
		seg2.iEndTime	= iEndTime;
		seg2.iStartKey	= 0;
		seg2.iEndKey	= iNumKey - 1;

		pAbsPosTck->GetSegments()[0] = seg1;
		pAbsRotTck->GetSegments()[0] = seg2;

		pTrack->pAbsPosTrack = pAbsPosTck;
		pTrack->pAbsRotTrack = pAbsRotTck;

		aCandidates.Add(pTrack);
		iValidCand++;
	}

	if (!iValidCand)
		return true;

	int iStartFrame = m_pTrackSet->GetAnimStartFrame();

	for (i=0; i < iNumKey; i++)
	{
		int iAbsTime = m_pTrackSet->FrameToTime(iStartFrame + i);
		
		//	Simulate action play, update bone state every frame
		int j, iCount = m_pSkeleton->GetJointNum();
		for (j=0; j < iCount; j++)
		{
			A3DJoint* pJoint = m_pSkeleton->GetJoint(j);
			if (pJoint->GetJointType() == A3DJoint::JOINT_ANIM)
			{
				A3DVECTOR3 vPos;
				A3DQUATERNION quRot;
				GetBoneStateOnTrack(j, iAbsTime, vPos, quRot);

				A3DAnimJoint* pAnimJoint = (A3DAnimJoint*)pJoint;
				pAnimJoint->GetChildBonePtr()->AddBlendState(quRot, vPos, 1.0f, A3DBone::BM_EXCLUSIVE);
			}
		}

		m_pSkeleton->UpdateForMakingAbsTrack();

		//	Get bone state and fill tracks
		for (j=0; j < iNumJoint; j++)
		{
			A3DSklTrackSet::BONETRACK* pTrack = aCandidates[j];
			if (!pTrack)
				continue;

			A3DJoint* pJoint = m_pSkeleton->GetJoint(aJoints[j]);
			if (!pJoint)
				continue;

			A3DBone* pBone = pJoint->GetChildBonePtr();
			const A3DMATRIX4& mat = pBone->GetUpToRootTM();

			A3DQUATERNION quat(mat);
			pTrack->pAbsPosTrack->SetKeyValue(i, mat.GetRow(3));
			pTrack->pAbsRotTrack->SetKeyValue(i, quat);
		}
	}

	return true;
}

//	Get position and orientation of specified time
bool A3DAbsTrackMaker::GetBoneStateOnTrack(int idJoint, int iAbsTime, 
					A3DVECTOR3& vPos, A3DQUATERNION& quRot)
{
	A3DSklTrackSet::BONETRACK* pBoneTrack = m_pTrackSet->GetBoneTrack(idJoint);
	if (!pBoneTrack)
	{
		ASSERT(pBoneTrack);
		return false;
	}

	vPos = pBoneTrack->pPosTrack->GetKeyValue(iAbsTime);
	quRot = pBoneTrack->pRotTrack->GetKeyValue(iAbsTime);
	quRot.Normalize();

	return true;
}
