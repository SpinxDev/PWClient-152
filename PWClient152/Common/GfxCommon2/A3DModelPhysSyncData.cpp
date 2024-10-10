/*
 * FILE: A3DModelPhysSyncData.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2008/5/21
 *
 * HISTORY: 
 *
 * Copyright (c) 2008 Archosaur Studio, All Rights Reserved.
 */

#include "StdAfx.h"
#ifdef A3D_PHYSX

#include "A3DModelPhysSyncData.h"
#include "EC_Model.h"
#include "APhysX.h"
#include "A3DPhysRBCreator.h"

#include <ALog.h>
#include <A3DSkinModel.h>
#include <A3DSkeleton.h>
#include <A3DBone.h>

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

//	Version 4: added attacher
//	Version 5: change attacher structure, remove vector of attacher, add attacher corresponding to "hook" name
//	Version 6: added A3DPhysRBCreator for different types of APhysXSkeletonRBObjectDesc's creation
//	Version 7: added joint's offset relative to linked-bone
//	Version 8: added collision group for A3DModelPhysSyncData, which specify the collision group of the skeleton object
//	Version 9: added A3DPhysActor::bUseDefaultMaterial to specify whether to use the material in the actor or the default one
//	Version 10: added m_uiObjectFlag to A3DModelPhysSyncData which is the model's skeleton object desc's new flag
//	Version 11: added soft key frame multi-config data to serialization
//	Version 12: added joint data flags to serialization
//	Version 13: added actor data flags to serialization
//	Version 14: added soft key frame frozen death config data

#define SYNCFILE_VERSION	14

///////////////////////////////////////////////////////////////////////////
//	
//	Reference to External variables and functions
//	
///////////////////////////////////////////////////////////////////////////

struct SYNCFILE_HEADER
{
	int		iNumActor;		//	Actor number
	int		iNumJoint;		//	Joint number
};

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
//	Implement A3DModelPhysSyncData::BONE_DATA
//	
///////////////////////////////////////////////////////////////////////////

A3DModelPhysSyncData::BONE_DATA::BONE_DATA() 
: matBone2Actor(A3DMATRIX4::IDENTITY)
{
	iBoneIdx	= -1;
	bAnimMajor	= true;
}

A3DModelPhysSyncData::BONE_DATA::~BONE_DATA()
{
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement A3DModelPhysSyncData::LINK_INFO
//	
///////////////////////////////////////////////////////////////////////////

A3DModelPhysSyncData::LINK_INFO::LINK_INFO() 
: matActor2Bone(A3DMATRIX4::IDENTITY)
{
	iMainBone	= -1;
	iLenBone	= -1;
}

A3DModelPhysSyncData::LINK_INFO::LINK_INFO(const LINK_INFO& rhs) 
: strMainBone(rhs.strMainBone)
, iMainBone(rhs.iMainBone)
, strLenBone(rhs.strLenBone)
, iLenBone(rhs.iLenBone)
, matActor2Bone(rhs.matActor2Bone)
{
	int i, nBoneNum = rhs.GetBoneNum();
	for (i = 0 ; i < nBoneNum ; ++i) {
		BONE_DATA* pBoneData = new BONE_DATA;
		*pBoneData = *rhs.GetBoneDataByIndex(i);
		aBones.Add(pBoneData);
	}
}

A3DModelPhysSyncData::LINK_INFO::~LINK_INFO()
{
	for (int j=0; j < aBones.GetSize(); j++)
		delete aBones[j];
}

A3DModelPhysSyncData::LINK_INFO& A3DModelPhysSyncData::LINK_INFO::operator = (const LINK_INFO& rhs)
{
	if (&rhs == this)
		return *this;

	strMainBone = rhs.strMainBone;
	iMainBone	= rhs.iMainBone;
	strLenBone	= rhs.strLenBone;
	iLenBone	= rhs.iLenBone;
	matActor2Bone = rhs.matActor2Bone;
	
	int i, nBoneNum = GetBoneNum();
	for (i = 0 ; i < nBoneNum ; ++i) 
		delete aBones[i];
	aBones.RemoveAll();

	int nDestBoneNum = rhs.GetBoneNum();
	for (i = 0 ; i < nDestBoneNum ; ++i) {
		BONE_DATA* pBoneData = new BONE_DATA;
		*pBoneData = *rhs.GetBoneDataByIndex(i);
		aBones.Add(pBoneData);
	}

	return *this;
}

A3DModelPhysSyncData::BONE_DATA* A3DModelPhysSyncData::LINK_INFO::GetBoneDataByName(const char* szBone) const
{
	for (int i=0; i < aBones.GetSize(); i++)
	{
		BONE_DATA* pBoneData = aBones[i];
		if (!_stricmp(pBoneData->GetBoneName(), szBone))
			return pBoneData;
	}

	return NULL;
}

void A3DModelPhysSyncData::LINK_INFO::SetLengthBone(const char* szBone)
{
	strLenBone  = szBone;
	iLenBone	= -1;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement A3DModelPhysSyncData::ACTOR_DATA
//	
///////////////////////////////////////////////////////////////////////////


A3DModelPhysSyncData::ACTOR_DATA::ACTOR_DATA()
: bUseDefaultMaterial(true)
{
	nFlags = 0;
}

A3DModelPhysSyncData::ACTOR_DATA::ACTOR_DATA(const ACTOR_DATA& rhs)
: A3DPhysActor(rhs)
, Link(rhs.Link)
, bUseDefaultMaterial(rhs.bUseDefaultMaterial)
{
	nFlags = rhs.nFlags;
}

A3DModelPhysSyncData::ACTOR_DATA::~ACTOR_DATA()
{
}

A3DPhysActor* A3DModelPhysSyncData::ACTOR_DATA::Clone() const
{
	return new ACTOR_DATA(*this);
}

A3DModelPhysSyncData::ACTOR_DATA& A3DModelPhysSyncData::ACTOR_DATA::operator = (const ACTOR_DATA& rhs)
{
	if (&rhs == this)
		return *this;
	
	A3DPhysActor::operator =(rhs);
	Link = rhs.Link;
	bUseDefaultMaterial = rhs.bUseDefaultMaterial;
	return *this;
}

//	Set use default material (all shape use default material -> all shape share this state)
void A3DModelPhysSyncData::ACTOR_DATA::SetUseDefaultMaterial(bool bIsUseDefaultMat)
{
	bUseDefaultMaterial = bIsUseDefaultMat;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement A3DModelPhysSyncData::JOINT_DATA
//	
///////////////////////////////////////////////////////////////////////////

A3DModelPhysSyncData::JOINT_DATA::JOINT_DATA()
: pJointDesc(NULL)
, iBoneIdx(-1)
, vOffset(0.f)
, nFlags(0)
{
	aActors[0] = NULL;
	aActors[1] = NULL;
}

A3DModelPhysSyncData::JOINT_DATA::~JOINT_DATA()
{
	if (pJointDesc)
		delete pJointDesc;
}

void A3DModelPhysSyncData::JOINT_DATA::LinkJointToBone(const char* szBone)
{
	if (strBone != szBone)
		vOffset.Clear();
	
	strBone		= szBone;
	iBoneIdx	= -1;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement A3DModelPhysSyncData::ATTACHER_DATA
//	
///////////////////////////////////////////////////////////////////////////

A3DModelPhysSyncData::ATTACHER_DATA::ATTACHER_DATA(const char* szAttacher, const char* szActor, const char* szHook, int nType)
: strAttacher(szAttacher)
, strActor(szActor)
, strCCHook(szHook)
, nAttacherType(nType)
{
	ASSERT(nAttacherType == APX_RB_ATTYPE_FIX || nAttacherType == APX_RB_ATTYPE_ROTATION_FREE);
}

void A3DModelPhysSyncData::ATTACHER_DATA::SetAttacherName(const char* szName)
{
	if (!szName || !szName[0])
		return;

	strAttacher = szName;
}

bool A3DModelPhysSyncData::ATTACHER_DATA::OnAttachActorNameChanged(const char* szOldName, const char* szNewName)
{
	if (!szOldName || !szOldName[0])
		return false;

	if (!szNewName || !szNewName[0])
		return false;

	if (strActor.CompareNoCase(szOldName) == 0)
		strActor = szNewName;
	return true;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement A3DModelPhysSyncData
//	
///////////////////////////////////////////////////////////////////////////

A3DModelPhysSyncData::A3DModelPhysSyncData()
{
	m_bBindInit			= false;
	m_pModelPhysDesc	= NULL;
	m_pAttacherDesc		= NULL;
	m_iRootBone			= -1;
	m_iRootActor		= -1;
	m_pRootBoneData		= NULL;
	m_uiObjectFlag		= APX_OBJFLAG_CC_ADD_PUSHFORCE;
	m_iCollisionGroup	= APX_COLLISION_GROUP_DEFAULT;
	m_iVersion			= SYNCFILE_VERSION;
	m_iInitBindErr		= ERR_OK;
	m_bModified			= false;
	m_pSkinModel		= 0;
	m_pPhysRBDescCreator= new A3DPhysNoneBreakableRBCreator();
#ifdef _ANGELICA21
	CreateStandardSKFDesc();
#endif
}

A3DModelPhysSyncData::~A3DModelPhysSyncData()
{
	m_pSkinModel = 0;
	if (m_pPhysRBDescCreator)
	{
		delete m_pPhysRBDescCreator;
		m_pPhysRBDescCreator = NULL;
	}

	if (m_pModelPhysDesc)
	{
		delete m_pModelPhysDesc;
		m_pModelPhysDesc = NULL;
	}

	if (m_pAttacherDesc)
	{
		delete m_pAttacherDesc;
		m_pAttacherDesc = NULL;
	}

	int i;

	//	Release all actor data
	for (i=0; i < m_aActors.GetSize(); i++)
	{
		ACTOR_DATA* pActor = m_aActors[i];
		delete pActor;
	}
	m_aActors.RemoveAll();

	//	Release all joint data
	for (i=0; i < m_aJoints.GetSize(); i++)
	{
		JOINT_DATA* pJoint = m_aJoints[i];
		delete pJoint;
	}
	m_aJoints.RemoveAll();

	//	Release all attacher data
	for (i=0; i < m_aAttachers.GetSize(); i++)
	{
		ATTACHER_DATA* pAttacher = m_aAttachers[i];
		delete pAttacher;
	}
	m_aAttachers.RemoveAll();

	//	Release all SKF data
#ifdef _ANGELICA21
	int nAll = m_aSKFNotification.GetSize();
	for (i=0; i < m_aSKFConfigs.GetSize(); ++i)
	{
		SoftKeyFrameConfigDesc* pSKF = m_aSKFConfigs[i];
		for (int j = 0; j < nAll; ++j)
			m_aSKFNotification[j]->onReleaseDesc(*pSKF);

		delete pSKF;
	}
	m_aSKFConfigs.RemoveAll();
#endif
}

//	Called when skin model and physical object are bound
bool A3DModelPhysSyncData::InitBind(A3DSkinModel* pSkinModel, bool bForceReset/* false */)
{
	m_iInitBindErr = ERR_OK;
	m_strErrObject.Empty();
	m_pSkinModel = 0;

	if (m_bBindInit && !bForceReset)
	{
		
		return true;
	}

	if (!pSkinModel || !pSkinModel->GetSkeleton())
	{
		m_iInitBindErr = ERR_OTHERS;
		return false;
	}

	m_bBindInit = false;

	if (m_iCollisionGroup < APX_COLLISION_GROUP_DEFAULT || m_iCollisionGroup >= APX_COLLISION_GROUP_INVALID)
	{
		m_iInitBindErr = ERR_COLLISION_GROUP_INVALID;
		return false;
	}

	//	Check all actor's shapes and modify necessary material index in the shape desc
	
	gGetAPhysXScene()->GetPerfStats()->ResumePerfStat("ASD_CheckMat");
	if (!CheckAndUpdateMaterialState())
	{
		
		gGetAPhysXScene()->GetPerfStats()->PausePerfStat("ASD_CheckMat");
		return false;
	}
	
	gGetAPhysXScene()->GetPerfStats()->PausePerfStat("ASD_CheckMat");

	//	Create Standard model physical desc
	
	gGetAPhysXScene()->GetPerfStats()->ResumePerfStat("ASD_CreateDesc");
	if (!CreateStandardModelPhysDesc(pSkinModel->GetSkeleton()))
	{
		
		gGetAPhysXScene()->GetPerfStats()->PausePerfStat("ASD_CreateDesc");
		return false;
	}
	
	gGetAPhysXScene()->GetPerfStats()->PausePerfStat("ASD_CreateDesc");

	//	Check attacher info before create attacher physical description
	
	gGetAPhysXScene()->GetPerfStats()->ResumePerfStat("ASD_CheckAttachInfo");
	if (!CheckAttacherInfo(pSkinModel))
	{
		
		gGetAPhysXScene()->GetPerfStats()->PausePerfStat("ASD_CheckAttachInfo");
		return false;
	}
	
	gGetAPhysXScene()->GetPerfStats()->PausePerfStat("ASD_CheckAttachInfo");

	//	Create Standard attacher physical description
	
	gGetAPhysXScene()->GetPerfStats()->ResumePerfStat("ASD_CreatAttachDesc");
	if (!CreateStandardAttacherPhysDesc(pSkinModel->GetSkeleton()))
	{
		
		gGetAPhysXScene()->GetPerfStats()->PausePerfStat("ASD_CreatAttachDesc");
		return false;
	}
	
	gGetAPhysXScene()->GetPerfStats()->PausePerfStat("ASD_CreatAttachDesc");

	//	Update link info.
	
	gGetAPhysXScene()->GetPerfStats()->ResumePerfStat("ASD_UptLink");
	if (!UpdateLinkInfo(pSkinModel))
	{
		
		gGetAPhysXScene()->GetPerfStats()->PausePerfStat("ASD_UptLink");
		return false;
	}
	
	gGetAPhysXScene()->GetPerfStats()->PausePerfStat("ASD_UptLink");

	m_bBindInit = true;
	m_bModified	= false;
	m_pSkinModel = pSkinModel;

	return true;
}

//	Create attacher physical description
APhysXRBAttacherDesc* A3DModelPhysSyncData::CreateAttacherPhysDescBySkeletonDesc(APhysXSkeletonRBObjectDesc* pSkeletonRBDesc, A3DSkeleton* pSkeleton)
{
	if (!pSkeletonRBDesc)
		return NULL;

	APhysXRBAttacherDesc* pAttacherDesc = new APhysXRBAttacherDesc;
	if (!pAttacherDesc)
		return NULL;

	pAttacherDesc->AddRBComponent(pSkeletonRBDesc);
	int nIdx, nAttacherNum = m_aAttachers.GetSize();
	for (nIdx = 0; nIdx < nAttacherNum; ++nIdx)
	{
		ATTACHER_DATA* pAttacher = m_aAttachers[nIdx];
		DeclareSphereAttacherDesc(rbDesc);

		A3DSkeletonHook* pHook = pSkeleton->GetHook(pAttacher->GetHook(), NULL);
		if (!pHook)
		{
			m_iInitBindErr = ERR_ATTACHER_HOOK_NOTEXIST;
			return false;
		}

		int iBone = pHook->GetBone();
		if (iBone < 0)
		{
			m_iInitBindErr = ERR_ATTACHER_HOOK_HASNOBONE;
			return false;
		}

		NxMat34 nmPos;
#if 1
		A3DBone* pBone = pSkeleton->GetBone(iBone);

		A3DMATRIX4 matHook2Model = pHook->GetHookTM() * pBone->GetScaleMatrix() * pBone->GetUpToRootTM();
		A3DMATRIX4 matHook2ModelNoScale = pHook->GetHookTM() * pBone->GetUpToRootTM();
		matHook2ModelNoScale.SetRow(3, matHook2Model.GetRow(3));

		//----------------------------------------------------------
		// revised by Wenfeng, Sept. 27, 2012
		// here we must consider the foot offset...
		matHook2ModelNoScale._42 -= pSkeleton->GetFootOffset();
		//----------------------------------------------------------

		APhysXConverter::A2N_Matrix44(matHook2ModelNoScale, nmPos);

#else
		// we can not use just pHook's absolute pose without scale since we have to 
		// get the pose in model's reference frame...

		APhysXConverter::A2N_Matrix44(pHook->GetNoScaleAbsTM(), nmPos);
#endif

		pAttacherDesc->AddMultiAttacherComponent(&rbDesc, pAttacher->GetAttacherName(), nmPos);
		pAttacherDesc->AddAttachInfo(pAttacher->GetAttachActor(), (APhysXRBAttachType)pAttacher->GetAttacherType(), nmPos.t, pAttacher->GetAttacherName());
	}

	return pAttacherDesc;
}

//	Create Standard attacher physical description
bool A3DModelPhysSyncData::CreateStandardAttacherPhysDesc(A3DSkeleton* pSkeleton)
{
	if (!pSkeleton || !m_pModelPhysDesc)
		return false;

	if (m_pAttacherDesc)
	{
		delete m_pAttacherDesc;
		m_pAttacherDesc = NULL;
	}

	m_pAttacherDesc = CreateAttacherPhysDescBySkeletonDesc(m_pModelPhysDesc, pSkeleton);
	
	if (!m_pAttacherDesc)
		return false;

	return true;
}

//	Check all actor's shapes and update necessary material state
bool A3DModelPhysSyncData::CheckAndUpdateMaterialState()
{
	for (int iActorIdx = 0; iActorIdx < m_aActors.GetSize(); ++iActorIdx)
	{
		ACTOR_DATA* pActor = m_aActors[iActorIdx];

		for (int iShapeIdx = 0; iShapeIdx < pActor->GetShapeNum(); ++iShapeIdx)
		{
			if (pActor->GetUseDefaultMaterial())
				pActor->GetShapeDesc(iShapeIdx)->UseDefaultMaterial();
			else
				pActor->GetShapeDesc(iShapeIdx)->mMaterialIndex = 0;
		}
	}

	return true;
}

//	Create model physical desciption
bool A3DModelPhysSyncData::CreateStandardModelPhysDesc(A3DSkeleton* pSkeleton)
{
	if (!pSkeleton)
	{
		m_iInitBindErr = ERR_OTHERS;
		return false;
	}

	if (m_pModelPhysDesc)
	{
		delete m_pModelPhysDesc;
		m_pModelPhysDesc = NULL;
	}

	ASSERT(m_pPhysRBDescCreator);

	m_pModelPhysDesc = m_pPhysRBDescCreator->CreateSkeletonRBObjectDesc();
	
	if (!m_pModelPhysDesc)
	{
		m_iInitBindErr = ERR_NO_MEMORY;
		return false;
	}

	// use the stored collision group id
	m_pModelPhysDesc->mCollisionGroup = (APhysXU32)m_iCollisionGroup;

	// model object desc flag
	m_pModelPhysDesc->mFlags = (APhysXU32)m_uiObjectFlag;

	APhysXSkeletonRBObjectDesc& desc = *m_pModelPhysDesc;
	APhysXArray<APhysXShapeDesc*> aShapes;

	int i, j;

	//	Add actors to object
	for (i=0; i < m_aActors.GetSize(); i++)
	{
		ACTOR_DATA* pActor = m_aActors[i];
		LINK_INFO& link = pActor->Link;

		A3DBone* pMainBone = pSkeleton->GetBone(link.strMainBone, NULL);

		if(!pMainBone)
		{
			m_iInitBindErr = ERR_NO_BONE;
			m_strErrObject = link.strMainBone;
			return false;			
		}

		//	Do some check
		if (!pActor->pActorDesc || !pActor->pMaterial || !pActor->aShapes.GetSize())
		{
			m_iInitBindErr = ERR_ACTOR_LACK_SHAPE;
			return false;
		}

		//	If actor doesn't linked with bone, ignore this actor
		if (!pActor->IsLinked())
			continue;

		//	Push shapes
		aShapes.clear();
		for (j=0; j < pActor->aShapes.GetSize(); j++)
		{
			aShapes.push_back(pActor->aShapes[j]);
#if 0		
			// Note: it's very complicated to handle the case that main bone is flipped, 
			// so currently, we just disable this function....
			// handle the right-hand frame case...
			if(pMainBone->IsFlipped())
			{
				NxMat34& matShapePose = aShapes[aShapes.size()-1]->mLocalPose;
				
				matShapePose.M.setRow(3, -matShapePose.M.getRow(3));
				matShapePose.t.z = -matShapePose.t.z;
			}
#else
			if(pMainBone->IsFlipped())
			{
				m_iInitBindErr = ERR_MAINBONE_FLIPPED;
				m_strErrObject = link.strMainBone;
				return false;
			}
#endif
		}

		pActor->pActorDesc->SetName(pActor->strName);

		// for ragdoll breakable joint, there are some enforce settings...
		int nJoints = pActor->GetJointNum();
		for (int k = 0; k < nJoints; ++k)
		{
			JOINT_DATA* pJoint = pActor->GetJoint(k);
			if (pJoint->ReadFlag(JDF_BREAKABLE_EQUIPMENTS))
			{
				if (pJoint->GetActor(1) == pActor)
				{
					pActor->pActorDesc->mDensity = 0.001f;
					break;
				}
			}
		}

		//	Try to add actor
		desc.AddActorComponent(*pActor->pActorDesc, *pActor->pMaterial, aShapes);
	}

	//	Add joints to object
	for (i=0; i < m_aJoints.GetSize(); i++)
	{
		A3DModelPhysSyncData::JOINT_DATA* pJoint = m_aJoints[i];

		A3DBone* pLinkedBone = pSkeleton->GetBone(pJoint->GetLinkedBoneName(), NULL);
		if (!pLinkedBone)
		{
			m_iInitBindErr = ERR_JOINT_NOTLINK;
			return false;
		}

		//	Do some check
		if (!pJoint->pJointDesc || (!pJoint->aActors[0] && !pJoint->aActors[1]))
			continue;

		pJoint->pJointDesc->mNxActor1Name = pJoint->aActors[0] ? gGlobalStringManager->GetString(pJoint->aActors[0]->strName) : NULL;
		pJoint->pJointDesc->mNxActor2Name = pJoint->aActors[1] ? gGlobalStringManager->GetString(pJoint->aActors[1]->strName) : NULL;

		//set joint's name to APhysXJointDesc...
		pJoint->pJointDesc->SetName(pJoint->strName);

		//------------------------------------------------
		// Note by Wenfeng, Sept. 26, 2012
		// since the joint's anchor is very important for correct physical behavior, we have to make sure global anchor
		// is exactly align to the position which is edited in editor. And this code will be called at runtime and maybe
		// the corresponding skin-model is playing some actions so we can not use current position to adjust anchor.

		// just use the global anchor pose kept in sync data...

		//A3DVECTOR3 vGlobalPos = pLinkedBone->GetUpToRootTM().GetRow(3);
		//vGlobalPos.y -= pSkeleton->GetFootOffset();
		//vGlobalPos += pJoint->GetOffset();
		//pJoint->pJointDesc->mGlobalAnchor = APhysXConverter::A2N_Vector3(vGlobalPos);

		// for breakable equipment joint, there are some enforce settings...
		if (pJoint->ReadFlag(JDF_BREAKABLE_EQUIPMENTS))
		{
			APhysXD6JointDesc* pD6Joint = (APhysXD6JointDesc*)pJoint->GetJointDesc();
			pD6Joint->mSwing1Motion = NX_D6JOINT_MOTION_LOCKED;
			pD6Joint->mSwing2Motion = NX_D6JOINT_MOTION_LOCKED;
			pD6Joint->mTwistMotion = NX_D6JOINT_MOTION_LOCKED;
		}

		desc.AddInnerConnector(*pJoint->pJointDesc);
	}
	
	m_pPhysRBDescCreator->PostProcessSkeletonRBObjectDesc(m_pModelPhysDesc);

	return true;
}

//	Update link info.
bool A3DModelPhysSyncData::UpdateLinkInfo(A3DSkinModel* pSkinModel)
{
	if (!pSkinModel)
	{
		m_iInitBindErr = ERR_OTHERS;
		return false;
	}

	A3DSkeleton* pSkeleton = pSkinModel->GetSkeleton();
	ASSERT(pSkeleton);

	int i, j, iBoneIdx;

	//	Check root bone
	if (!pSkeleton->GetBone(m_strRootBone, &iBoneIdx))
	{
		m_iInitBindErr = ERR_NO_ROOTBONE;
		m_strErrObject = m_strRootBone;
		return false;	//	Root bone doesn't exist.
	}

	m_iRootBone		= iBoneIdx;
	m_iRootActor	= -1;
	m_pRootBoneData	= NULL;

	//	Link joint and bone
	for (i=0; i < m_aJoints.GetSize(); i++)
	{
		JOINT_DATA* pJoint = m_aJoints[i];
		if (!pJoint->strBone.GetLength())
		{
			m_iInitBindErr = ERR_JOINT_NOTLINK;
			m_strErrObject = pJoint->GetName();
			return false;
		}
		else if (!pSkeleton->GetBone(pJoint->strBone, &iBoneIdx))
		{
			m_iInitBindErr = ERR_NO_BONE;
			m_strErrObject = pJoint->strBone;
			return false;	//	Bone doesn't exist.
		}

		pJoint->iBoneIdx = iBoneIdx;
	}

	//	Check the link between bones and actors
	AArray<int> aBoneRefs;
	aBoneRefs.SetSize(pSkeleton->GetBoneNum(), 4);
	for (i=0; i < aBoneRefs.GetSize(); i++)
		aBoneRefs[i] = -1;

	for (i=0; i < m_aActors.GetSize(); i++)
	{
		ACTOR_DATA* pActor = m_aActors[i];
		if (!pActor->IsLinked())
			continue;	//	Ignore unlinked actor

		LINK_INFO& link = pActor->Link;

		//	Clear old bone indices
		for (j=0; j < link.aBones.GetSize(); j++)
			link.aBones[j]->iBoneIdx = -1;

		for (j=0; j < link.aBones.GetSize(); j++)
		{
			BONE_DATA* pBoneData = link.aBones[j];

			if (!pSkeleton->GetBone(pBoneData->strBone, &iBoneIdx))
			{
				m_iInitBindErr = ERR_NO_BONE;
				m_strErrObject = pBoneData->strBone;
				return false;	//	Bone doesn't exist.
			}

			if (aBoneRefs[iBoneIdx] >= 0)
			{
				m_iInitBindErr = ERR_BONE_LINKTWICE;
				m_strErrObject = pBoneData->strBone;
				return false;	//	Bone has been linked with other actor
			}

			aBoneRefs[iBoneIdx] = i;

			//	Save bone's index
			pBoneData->iBoneIdx = iBoneIdx;

			//	Record root bone's data
			if (iBoneIdx == m_iRootBone)
				m_pRootBoneData = pBoneData;
		}

		//	Decide main bone's index
		if (!pSkeleton->GetBone(link.strMainBone, &iBoneIdx))
		{
			m_iInitBindErr = ERR_ACTOR_NOTLINK;
			m_strErrObject = link.strMainBone;
			return false;	//	Bone don't exist.
		}

		link.iMainBone = iBoneIdx;

		//	Actor's main bone must be linked with some joint
	/*	int iJoint = GetBoneLinkWithJoint(link.iMainBone);
		if (iJoint < 0)
		{
			m_iInitBindErr = ERR_MAINBONE_NOTJOINT;
			m_strErrObject = pActor->GetName();
			return false;	//	Bone don't exist.
		}
	*/
		//	Decide length bone's index
		if (link.strLenBone.GetLength())
		{
			A3DBone* pLenBone = pSkeleton->GetBone(link.strLenBone, &iBoneIdx);
			if (!pLenBone)
			{
				m_iInitBindErr = ERR_ACTOR_NOTLINK;
				m_strErrObject = link.strLenBone;
				return false;	//	Bone don't exist.
			}

			if (pLenBone->GetParent() != link.iMainBone)
			{
				m_iInitBindErr = ERR_LENBONE_NOTCHILD;
				m_strErrObject = link.strLenBone;
				return false;
			}

			link.iLenBone = iBoneIdx;
		}
		else
			link.iLenBone = -1;
	}

	//	Check if every bone has been linked with actor
	for (i=0; i < aBoneRefs.GetSize(); i++)
	{
		if (aBoneRefs[i] < 0)
		{
			m_iInitBindErr = ERR_BONE_NOTLINK;
			m_strErrObject = pSkeleton->GetBone(i)->GetName();
			return false;	//	This bone hasn't be linked
		}
	}

	//	Root bone must be linked with actor
	if (!m_pRootBoneData)
	{
		m_iInitBindErr = ERR_BONE_NOTLINK;
		return false;
	}

	m_iRootActor = aBoneRefs[m_iRootBone];

	return true;
}

//	Check if specified bone is linked with some joint
//	Return joint's index
int A3DModelPhysSyncData::GetBoneLinkWithJoint(int iBoneIdx)
{
	int i;

	for (i=0; i < m_aJoints.GetSize(); i++)
	{
		JOINT_DATA* pJoint = m_aJoints[i];
		if (pJoint->iBoneIdx == iBoneIdx)
			return i;
	}

	return -1;
}

//	Check attacher info
bool A3DModelPhysSyncData::CheckAttacherInfo(A3DSkinModel* pSkinModel)
{
	int i;

	for (i = 0; i < m_aAttachers.GetSize(); i++)
	{
		if (!GetActorByName(m_aAttachers[i]->GetAttachActor())) {
			m_iInitBindErr = ERR_ATTACHER_ACTOR_NOTEXIST;
			return false;
		}

		int nType = m_aAttachers[i]->GetAttacherType();
		if (nType != APX_RB_ATTYPE_FIX
			&& nType != APX_RB_ATTYPE_ROTATION_FREE) {
			m_iInitBindErr = ERR_ATTACHER_TYPE_UNKNOWN;
			return false;
		}

		A3DSkeletonHook* pHook = pSkinModel->GetSkeletonHook(m_aAttachers[i]->GetHook(), true);
		if (!pHook) {
			m_iInitBindErr = ERR_ATTACHER_HOOK_NOTEXIST;
			return false;
		}

		if (pHook->GetBone() < 0) {
			m_iInitBindErr = ERR_ATTACHER_HOOK_HASNOBONE;
			return false;
		}
	}

	return true;
}

#ifdef _ANGELICA21
void A3DModelPhysSyncData::CreateStandardSKFDesc(int enumSKFConfigName)
{
	static const char* strInjury = A3DModelPhysSync::GetDefSKFConfigName(A3DModelPhysSync::CN_INJURY);
	static const char* strPickThrow = A3DModelPhysSync::GetDefSKFConfigName(A3DModelPhysSync::CN_PICK_THROW);
	static const char* strFrozenDeath = A3DModelPhysSync::GetDefSKFConfigName(A3DModelPhysSync::CN_FROZEN_DEATH);
	JointInfo ji;
	SoftKeyFrameConfigDesc* pSKF = 0;

	if ((0 > enumSKFConfigName) || (A3DModelPhysSync::CN_INJURY == enumSKFConfigName))
	{
		// default injury desc
		RemoveSKFConfigDesc(strInjury);
		pSKF = CreateSKFConfigDesc(strInjury);
		if (pSKF)
		{
			ji.strName = SoftKeyFrameConfigDesc::DefJointName();
			ji.mSpring = 100.0f;
			ji.mDamping = 1.0f;
			pSKF->UpdateJointInfo(ji);
			pSKF->AddFixActorBone("Bip01 L Calf");
			pSKF->AddFixActorBone("Bip01 R Calf");
		}
	}

	if ((0 > enumSKFConfigName) || (A3DModelPhysSync::CN_PICK_THROW == enumSKFConfigName))
	{
		// default pick & throw desc
		RemoveSKFConfigDesc(strPickThrow);
		pSKF = CreateSKFConfigDesc(strPickThrow);
		if (pSKF)
		{
			ji.strName = SoftKeyFrameConfigDesc::DefJointName();
			ji.mSpring = 300.0f;
			ji.mDamping = 1.0f;
			pSKF->UpdateJointInfo(ji);
		}
	}

	if ((0 > enumSKFConfigName) || (A3DModelPhysSync::CN_FROZEN_DEATH == enumSKFConfigName))
	{
		// default frozen death desc
		RemoveSKFConfigDesc(strFrozenDeath);
		pSKF = CreateSKFConfigDesc(strFrozenDeath);
		if (pSKF)
		{
			ji.strName = SoftKeyFrameConfigDesc::DefJointName();
			ji.mSpring = -1.0f;
			ji.mDamping = 1.0f;
			pSKF->UpdateJointInfo(ji);
		}
	}
}
#endif

//	Get actor
A3DModelPhysSyncData::ACTOR_DATA* A3DModelPhysSyncData::GetActorByName(const char* szName, int* piIndex/* NULL */) const
{
	for (int i=0; i < m_aActors.GetSize(); i++)
	{
		ACTOR_DATA* pActor = m_aActors[i];
		if (!pActor->strName.CompareNoCase(szName))
		{
			if (piIndex) 
				*piIndex = i;

			return pActor;
		}
	}

	return NULL;
}

A3DModelPhysSyncData::ACTOR_DATA* A3DModelPhysSyncData::GetActorByMainBone(const char* szMainBone, int* piIndex /*= NULL*/) const
{
	for (int i=0; i < m_aActors.GetSize(); ++i)
	{
		ACTOR_DATA* pActor = m_aActors[i];

		if (0 == strcmp(pActor->GetLinkInfo().GetMainBone(), szMainBone))
		{
			if (piIndex)
				*piIndex = i;

			return pActor;
		}
	}

	return NULL;
}

//	Create a new actor
A3DModelPhysSyncData::ACTOR_DATA* A3DModelPhysSyncData::CreateActor(const char* szName, const A3DVECTOR3& vPos)
{
	if (!szName || !szName[0])
		return NULL;

	//	Check actor name collision
	ACTOR_DATA* pActor = GetActorByName(szName);
	if (pActor)
		return NULL;

	pActor = new ACTOR_DATA;
	pActor->strName = szName;

	//	Initialize actor position
	A3DMATRIX4 mat(A3DMATRIX4::IDENTITY);
	mat.SetRow(3, vPos);
	APhysXConverter::A2N_Matrix44(mat, pActor->pActorDesc->mLocalPose);

	//	Initialize material
	pActor->pMaterial->mRestitution = gDefaultPhysXMaterialDesc.mRestitution;
	pActor->pMaterial->mStaticFriction = gDefaultPhysXMaterialDesc.mStaticFriction;
	pActor->pMaterial->mDynamicFriction = gDefaultPhysXMaterialDesc.mDynamicFriction;
	
	//	Add actor to array
	m_aActors.Add(pActor);

	m_bModified	= true;

	return pActor;
}

//	Remove a actor
void A3DModelPhysSyncData::RemoveActor(const char* szName)
{
	//	Get actor object
	int iActorIdx = -1;
	ACTOR_DATA* pActor = GetActorByName(szName, &iActorIdx);
	if (!pActor)
		return;

	int i, iNumJoint = pActor->aJoints.GetSize();

	//	Unlink actor and joints
	for (i=0; i < iNumJoint; i++)
	{
		//	UnlinkActorAndJoint() remove joint from pActor->aJoints, so we can
		//	get the first joint every time
		JOINT_DATA* pJoint = pActor->aJoints[0];
		UnlinkActorAndJoint(szName, pJoint->strName);
	}

	m_aActors.RemoveAt(iActorIdx);
	delete pActor;

	m_bModified	= true;
}

//	Change actor name
bool A3DModelPhysSyncData::ChangeActorName(const char* szOldName, const char* szNewName)
{
	if (!szNewName || !szNewName[0])
		return false;

	//	Get actor object
	ACTOR_DATA* pActor = GetActorByName(szOldName);
	if (!pActor)
		return false;

	//	Check name collision
	ACTOR_DATA* pTempActor = GetActorByName(szNewName);
	if (pTempActor == pActor)
		return true;	//	szOldName == szNewName
	else if (pTempActor)
		return false;	//	szNewName has been used by other actor

	pActor->strName = szNewName;

	int nIdx, nNum = m_aAttachers.GetSize();
	for (nIdx = 0; nIdx < nNum; ++nIdx)
		m_aAttachers[nIdx]->OnAttachActorNameChanged(szOldName, szNewName);
	

	m_bModified	= true;

	return true;
}

//	Set actor's position and orientation
void A3DModelPhysSyncData::SetActorPose(const char* szActor, const A3DMATRIX4& matActor, A3DSkinModel* pSkinModel)
{
	if (!pSkinModel)
	{
		ASSERT(pSkinModel);
		return;
	}

	//	Get actor object
	ACTOR_DATA* pActor = GetActorByName(szActor);
	if (!pActor)
		return;

	//	Save actor matrix and it's inverse matrix
	APhysXConverter::A2N_Matrix44(matActor, pActor->pActorDesc->mLocalPose);
	A3DMATRIX4 matInvActor;
	a3d_InverseTM(matActor, &matInvActor);

	A3DSkeleton* pSkeleton = pSkinModel->GetSkeleton();
	A3DBone* pBone = NULL;
	LINK_INFO& link = pActor->Link;
	
	//	If actor is linked with bone, rebuild transform matrcies between them
	if (link.strMainBone.GetLength())
	{
		pBone = pSkeleton->GetBone(link.strMainBone, NULL);
		if (pBone)
		{
			A3DMATRIX4 matInvBone;
			a3d_InverseTM(pBone->GetAbsoluteTM(), &matInvBone);
			link.matActor2Bone = matActor * matInvBone;
		}
		else
		{
			ASSERT(pBone);
		}
	}

	//	Transform matrices from bone space to actor space
	for (int i=0; i < link.aBones.GetSize(); i++)
	{
		BONE_DATA* pBoneData = link.aBones[i];
		pBone = pSkeleton->GetBone(pBoneData->strBone, NULL);
		if (pBone)
		{
			pBoneData->matBone2Actor = pBone->GetAbsoluteTM() * matInvActor;
		}
	}

	m_bModified	= true;
}

//	Link actor to a bone
bool A3DModelPhysSyncData::LinkActorToBone(const char* szActor, A3DSkinModel* pSkinModel, const char* szBone)
{
	ASSERT(pSkinModel);

	int iBoneIdx = -1;
	A3DBone* pBone = pSkinModel->GetSkeleton()->GetBone(szBone, &iBoneIdx);
	if (!pBone)
		return false;

	ACTOR_DATA* pActor = GetActorByName(szActor);
	if (!pActor)
		return false;

	//	Link actor to bone
	pActor->Link.strMainBone = szBone;
	pActor->Link.iMainBone = -1;

	//	Build transform matrix from actor space to bone space
	A3DMATRIX4 matInvBone;
	a3d_InverseTM(pBone->GetAbsoluteTM(), &matInvBone);
	A3DMATRIX4 matActor;
	APhysXConverter::N2A_Matrix44(pActor->pActorDesc->mLocalPose, matActor);
	pActor->Link.matActor2Bone = matActor * matInvBone;

	m_bModified	= true;

	return true;
}

//	Un-link actor from bone
bool A3DModelPhysSyncData::UnlinkActorFromBone(const char* szActor)
{
	ACTOR_DATA* pActor = GetActorByName(szActor);
	if (!pActor)
		return false;

	pActor->Link.strMainBone.Empty();
	pActor->Link.iMainBone = -1;
	pActor->Link.matActor2Bone.Identity();

	m_bModified	= true;

	return true;
}

//	Link bone to actor
bool A3DModelPhysSyncData::LinkBoneToActor(const char* szActor, A3DSkinModel* pSkinModel, const char* szBone)
{
	ASSERT(pSkinModel);

	int iBoneIdx = -1;
	A3DBone* pBone = pSkinModel->GetSkeleton()->GetBone(szBone, &iBoneIdx);
	if (!pBone)
		return false;

	int i, j;

	//	Check if this bone has been linked with other actor
	for (i=0; i < m_aActors.GetSize(); i++)
	{
		LINK_INFO& link = m_aActors[i]->Link;

		for (j=0; j < link.aBones.GetSize(); j++)
		{
			BONE_DATA* pBoneData = link.aBones[j];
			if (!pBoneData->strBone.CompareNoCase(szBone))
				return false;
		}
	}

	ACTOR_DATA* pActor = GetActorByName(szActor);
	if (!pActor)
		return false;

	//	Save bone's info
	BONE_DATA* pBoneData = new BONE_DATA;
	pBoneData->strBone = szBone;
	pBoneData->iBoneIdx = -1;

	//	Build transform matrix from bone space to actor space
	A3DMATRIX4 matBone = pBone->GetAbsoluteTM();
	A3DMATRIX4 matActor, matInvActor;
	APhysXConverter::N2A_Matrix44(pActor->pActorDesc->mLocalPose, matActor);
	a3d_InverseTM(matActor, &matInvActor);
	pBoneData->matBone2Actor = matBone * matInvActor;

	pActor->Link.aBones.Add(pBoneData);

	m_bModified	= true;

	return true;
}

//	Un-link bone from actor
bool A3DModelPhysSyncData::UnlinkBoneFromActor(const char* szActor, const char* szBone)
{
	ACTOR_DATA* pActor = GetActorByName(szActor);
	if (!pActor)
		return false;

	LINK_INFO& link = pActor->Link;

	for (int i=0; i < link.aBones.GetSize(); i++)
	{
		BONE_DATA* pBoneData = link.aBones[i];
		if (!pBoneData->strBone.CompareNoCase(szBone))
		{
			delete pBoneData;
			link.aBones.RemoveAt(i);
			break;
		}
	}

	m_bModified	= true;

	return true;
}

//	Link actor and joint
bool A3DModelPhysSyncData::LinkActorAndJoint(const char* szActor, const char* szJoint, int iPos)
{
	ACTOR_DATA* pActor = GetActorByName(szActor);
	JOINT_DATA* pJoint = GetJointByName(szJoint);
	if (!pActor || !pJoint)
		return false;

	return LinkActorAndJoint_Inner(pActor, pJoint, iPos);
}

//	Link actor and joint
bool A3DModelPhysSyncData::LinkActorAndJoint_Inner(ACTOR_DATA* pActor, JOINT_DATA* pJoint, int iPos)
{
	if (iPos != 0 && iPos != 1)
	{
		ASSERT(iPos == 0 || iPos == 1);
		return false;
	}

	//	Joint's two heads couldn't link to a same actor
	if ((iPos == 0 && pJoint->aActors[1] == pActor) ||
		(iPos == 1 && pJoint->aActors[0] == pActor))
		return false;

	int i;

	//	Check whether joint has been linked with actor
	for (i=0; i < pActor->aJoints.GetSize(); i++)
	{
		if (pActor->aJoints[i] == pJoint)
			break;
	}

	if (i >= pActor->aJoints.GetSize())
		pActor->aJoints.Add(pJoint);

	pJoint->aActors[iPos] = pActor;

	return true;
}

//	Un-link actor and joint
void A3DModelPhysSyncData::UnlinkActorAndJoint(const char* szActor, const char* szJoint)
{
	ASSERT(szActor && szJoint);

	JOINT_DATA* pJoint = GetJointByName(szJoint);
	if (!pJoint)
		return;

	if (pJoint->aActors[0] && !pJoint->aActors[0]->strName.CompareNoCase(szActor))
		UnlinkActorAndJoint_Inner(pJoint, 0);

	if (pJoint->aActors[1] && !pJoint->aActors[1]->strName.CompareNoCase(szActor))
		UnlinkActorAndJoint_Inner(pJoint, 1);

	m_bModified	= true;
}

void A3DModelPhysSyncData::UnlinkActorAndJoint(const char* szJoint, int iPos)
{
	JOINT_DATA* pJoint = GetJointByName(szJoint);
	if (pJoint)
		UnlinkActorAndJoint_Inner(pJoint, iPos);

	m_bModified	= true;
}

//	Un-link actor and joint
void A3DModelPhysSyncData::UnlinkActorAndJoint_Inner(JOINT_DATA* pJoint, int iPos)
{
	if (iPos != 0 && iPos != 1)
	{
		ASSERT(iPos == 0 || iPos == 1);
		return;
	}

	//	Remove actor from joint
	ACTOR_DATA* pActor = pJoint->aActors[iPos];
	if (!pActor)
		return;

	pJoint->aActors[iPos] = NULL;

	//	Remove joint from actor
	for (int i=0; i < pActor->aJoints.GetSize(); i++)
	{
		if (pActor->aJoints[i] == pJoint)
		{
			pActor->aJoints.RemoveAt(i);
			break;
		}
	}
}

//	Get joint object by name
A3DModelPhysSyncData::JOINT_DATA* A3DModelPhysSyncData::GetJointByName(const char* szName, int* piIndex/* NULL */) const
{
	for (int i=0; i < m_aJoints.GetSize(); i++)
	{
		JOINT_DATA* pJoint = m_aJoints[i];
		if (!pJoint->strName.CompareNoCase(szName))
		{
			if (piIndex) 
				*piIndex = i;

			return pJoint;
		}
	}

	return NULL;
}

//	Create a new joint
A3DModelPhysSyncData::JOINT_DATA* A3DModelPhysSyncData::CreateJoint(const char* szName, const APhysXJointDesc* pDesc)
{
	if (!szName || !szName[0])
		return NULL;

	//	Check joint name collision
	JOINT_DATA* pJoint = GetJointByName(szName);
	if (pJoint)
		return NULL;

	pJoint = new JOINT_DATA;
	pJoint->strName = szName;
	pJoint->pJointDesc = (APhysXJointDesc*)pDesc->Clone();

	//	Add joint to array
	m_aJoints.Add(pJoint);

	m_bModified	= true;

	return pJoint;
}

//	Remove a joint
void A3DModelPhysSyncData::RemoveJoint(const char* szName)
{
	//	Get joint object
	int iJointIdx = -1;
	JOINT_DATA* pJoint = GetJointByName(szName, &iJointIdx);
	if (!pJoint)
		return;

	UnlinkActorAndJoint_Inner(pJoint, 0);
	UnlinkActorAndJoint_Inner(pJoint, 1);

	m_aJoints.RemoveAt(iJointIdx);
	delete pJoint;

	m_bModified	= true;
}

//	Change joint's name
bool A3DModelPhysSyncData::ChangeJointName(const char* szOldName, const char* szNewName)
{
	if (!szNewName || !szNewName[0])
		return false;

	//	Get joint object
	JOINT_DATA* pJoint = GetJointByName(szOldName);
	if (!pJoint)
		return false;

	//	Check name collision
	JOINT_DATA* pTempJoint = GetJointByName(szNewName);
	if (pTempJoint == pJoint)
		return true;	//	szOldName == szNewName
	else if (pTempJoint)
		return false;	//	szNewName has been used by other joint

	pJoint->strName = szNewName;

	m_bModified	= true;

	return true;
}

//	Get attacher
A3DModelPhysSyncData::ATTACHER_DATA* A3DModelPhysSyncData::GetAttacherByName(const char* szName, int* piIndex/*=NULL*/) const
{
	for (int i=0; i < m_aAttachers.GetSize(); i++)
	{
		ATTACHER_DATA* pAttacher = m_aAttachers[i];
		if (!pAttacher->GetAttacherName().CompareNoCase(szName))
		{
			if (piIndex) 
				*piIndex = i;

			return pAttacher;
		}
	}

	return NULL;
}

//	Create attacher
A3DModelPhysSyncData::ATTACHER_DATA* A3DModelPhysSyncData::CreateAttacher(const char* szName, const char* szActor, const char* szHook, int nAttacherType)
{
	if (!szName || !szName[0])
		return NULL;

	if (!szActor || !szActor[0])
		return NULL;

	if (!szHook || !szHook[0])
		return NULL;

	//	Check attach actor is exist
	if (!GetActorByName(szActor))
		return NULL;

	//	Check attacher name collision
	ATTACHER_DATA* pAttacher = GetAttacherByName(szName);
	if (pAttacher)
		return NULL;

	pAttacher = new ATTACHER_DATA(szName, szActor, szHook, nAttacherType);

	//	Add attacher to array
	m_aAttachers.Add(pAttacher);

	m_bModified	= true;

	return pAttacher;
}

//	Remove an attacher
void A3DModelPhysSyncData::RemoveAttacher(const char* szName)
{
	//	Get attacher object
	int iAttacherIdx = -1;
	ATTACHER_DATA* pAttacher = GetAttacherByName(szName, &iAttacherIdx);
	if (!pAttacher)
		return;

	m_aAttachers.RemoveAt(iAttacherIdx);
	delete pAttacher;
	
	m_bModified = true;
}

//	Change attacher 's name
bool A3DModelPhysSyncData::ChangeAttacherName(const char* szOldName, const char* szNewName)
{
	if (!szNewName || !szNewName[0])
		return false;

	//	Check name collision
	ATTACHER_DATA* pTempAttacher = GetAttacherByName(szNewName);
	if (pTempAttacher)
		return false;	//	szNewName has been used

	int iAttacherIdx = -1;
	ATTACHER_DATA* pAttacher = GetAttacherByName(szOldName, &iAttacherIdx);
	if (!pAttacher)
		return false;

	pAttacher->SetAttacherName(szNewName);
	m_bModified = true;
	return true;
}

#ifdef _ANGELICA21

SoftKeyFrameConfigDesc* A3DModelPhysSyncData::GetSKFConfigDescByIndex(int n) const
{
	int nCount = m_aSKFConfigs.GetSize();
	if ((0 <= n) && (n < nCount))
		return m_aSKFConfigs[n];
	return 0;
}

SoftKeyFrameConfigDesc* A3DModelPhysSyncData::GetSKFConfigDescByName(const char* szName, int* piIndex) const
{
	SoftKeyFrameConfigDesc* pSKFDesc = 0;
	int nCount = m_aSKFConfigs.GetSize();
	for (int i = 0; i < nCount; ++i)
	{
		pSKFDesc = m_aSKFConfigs[i];
		if (!pSKFDesc->GetName().CompareNoCase(szName))
		{
			if (piIndex) 
				*piIndex = i;

			return pSKFDesc;
		}
	}

	return 0;
}

SoftKeyFrameConfigDesc* A3DModelPhysSyncData::CreateSKFConfigDesc(const char* szName)
{
	if (!szName || !szName[0])
		return NULL;

	//	Check name collision
	SoftKeyFrameConfigDesc* pSKF = GetSKFConfigDescByName(szName);
	if (pSKF)
		return NULL;

	pSKF = new SoftKeyFrameConfigDesc(szName);
	if (!pSKF)
		return NULL;

	m_aSKFConfigs.Add(pSKF);
	return pSKF;
}

void A3DModelPhysSyncData::RemoveSKFConfigDesc(const char* szName)
{
	int iIdx = -1;
	SoftKeyFrameConfigDesc* pSKF = GetSKFConfigDescByName(szName, &iIdx);
	if (!pSKF)
		return;

	int nAll = m_aSKFNotification.GetSize();
	for (int i = 0; i < nAll; ++i)
		m_aSKFNotification[i]->onReleaseDesc(*pSKF);

	m_aSKFConfigs.RemoveAt(iIdx);
	delete pSKF;
}

void A3DModelPhysSyncData::ClearAllSKFConfigDesc()
{
	SoftKeyFrameConfigDesc* pSKF = 0;
	int nAllNotify = m_aSKFNotification.GetSize();
	int nAll = m_aSKFConfigs.GetSize();
	for (int i = 0; i < nAll; ++i)
	{
		pSKF = m_aSKFConfigs[i];
		for (int j = 0; j < nAllNotify; ++j)
			m_aSKFNotification[j]->onReleaseDesc(*pSKF);
		delete pSKF;
	}
	m_aSKFConfigs.RemoveAll();
}

bool A3DModelPhysSyncData::ChangeSKFConfigDescName(const char* szOldName, const char* szNewName)
{
	if (!szNewName || !szNewName[0])
		return false;

	//	Check name collision
	SoftKeyFrameConfigDesc* pSKF = GetSKFConfigDescByName(szNewName);
	if (pSKF)
		return false;

	int iIdx = -1;
	pSKF = GetSKFConfigDescByName(szOldName, &iIdx);
	if (!pSKF)
		return false;

	pSKF->SetName(szNewName);
	return true;
}

void A3DModelPhysSyncData::RegisterSKFNotify(SoftKeyFrameDescNotify* pNotify)
{
	if (0 != pNotify)
	{
		int nAll = m_aSKFNotification.GetSize();
		for (int i = 0; i < nAll; ++i)
		{
			if (pNotify == m_aSKFNotification[i])
				return;
		}
		m_aSKFNotification.Add(pNotify);
	}
}

void A3DModelPhysSyncData::UnRegisterSKFNotify(SoftKeyFrameDescNotify* pNotify)
{
	if (0 != pNotify)
	{
		int nAll = m_aSKFNotification.GetSize();
		for (int i = 0; i < nAll; ++i)
		{
			if (pNotify == m_aSKFNotification[i])
			{
				if (i != nAll - 1)
					m_aSKFNotification[i] = m_aSKFNotification[nAll - 1];
				m_aSKFNotification.RemoveTail();
				return;
			}
		}
	}
}

#endif

//	Set RB Object type
bool A3DModelPhysSyncData::SetRBDescObjectType(unsigned int uType)
{
	if (uType < NON_BREAKABLE || uType >= RBDESC_MAX_NUM)
		return false;

	if (m_pPhysRBDescCreator->GetRBType() == uType)
		return true;

	A3DPhysRBCreator* pCreator = A3DPhysRBCreator::CreatePhysRBCreator(uType);
	if (!pCreator)
		return false;

	delete m_pPhysRBDescCreator;
	m_pPhysRBDescCreator = pCreator;

	m_bModified = true;

	return true;
}

//	Set a new RB desc creator
bool A3DModelPhysSyncData::SetRBDescObjectType(A3DPhysRBCreator* pNewCreator)
{
	if (!pNewCreator)
		return false;

	if (pNewCreator == m_pPhysRBDescCreator)
		return true;

	delete m_pPhysRBDescCreator;
	m_pPhysRBDescCreator = pNewCreator;
	m_bModified = true;

	return true;
}

//	Set root bone name
void A3DModelPhysSyncData::SetRootBone(const char* szBone)
{
	m_strRootBone	= szBone;
	m_iRootBone		= -1;
	m_iRootActor	= -1;
	m_pRootBoneData	= NULL;

	m_bModified	= true;
}

//	Set collision group (See APhysXObjectDesc::mCollisionGroup and enum APhysXCollisionGroup)
void A3DModelPhysSyncData::SetCollisionGroup(int iCollisionGroup)
{
	if (iCollisionGroup < APX_COLLISION_GROUP_DEFAULT
		|| iCollisionGroup >= APX_COLLISION_GROUP_INVALID)
		return;

	m_iCollisionGroup = iCollisionGroup;
	m_bModified = true;
}


//	Set model object desc's flag (See APhysXObjectDesc::mFlag and enum )
void A3DModelPhysSyncData::SetModelObjectFlag(unsigned int uFlag)
{
	m_uiObjectFlag = uFlag;
	m_bModified = true;
}

//	Load data from file
bool A3DModelPhysSyncData::Load(const char* szFile)
{
	APhysXFileImageStream stream(szFile, true);

	if (!stream.IsValid())
	{
		a_LogOutput(1, "A3DModelPhysSyncData::Load, failed to open file %s", szFile);
		return false;
	}

	//	Read file version
	m_iVersion = (int)stream.readDword();

	//	Read file header
	SYNCFILE_HEADER fh;
	stream.readBuffer(&fh, sizeof (fh));

	//	Read root bone info
	ReadString(&stream, m_strRootBone);

	int i;

	//	Read actor data
	for (i=0; i < fh.iNumActor; i++)
	{
		ACTOR_DATA* pActor = new ACTOR_DATA;
		if (!ReadActorData(&stream, pActor))
			return false;

		m_aActors.Add(pActor);
	}

	//	Read joint data
	if (!ReadJointData(&stream, fh.iNumJoint))
		return false;

	//	Link actor and joints
	int iNumIndex = fh.iNumJoint * 2;
	AArray<int> aIndices;
	aIndices.SetSize(iNumIndex, 4);

	if (aIndices.GetSize())
		stream.readBuffer(aIndices.GetData(), iNumIndex * sizeof (int));

	for (i=0; i < iNumIndex; i++)
	{
		JOINT_DATA* pJoint = m_aJoints[i >> 1];
		int iPos = i & 1;

		int iActorIdx = aIndices[i];
		if (iActorIdx >= 0)
		{
			ACTOR_DATA* pActor = m_aActors[iActorIdx];
			LinkActorAndJoint_Inner(pActor, pJoint, iPos);
		}
	}

	//	Read attacher data
	if (!ReadAttacherData(&stream))
		return false;

	//	Read rb-desc creator info
	if (!ReadRBCreator(&stream))
		return false;

	if (m_iVersion >= 8)
		m_iCollisionGroup = stream.readDword();

	if (m_iVersion >= 10)
		m_uiObjectFlag = stream.readDword();

#ifdef _ANGELICA21
	ClearAllSKFConfigDesc();
	if (m_iVersion >= 11)
	{
		int nSKFData = stream.readDword();
		SoftKeyFrameConfigDesc* pSKFD = 0;
		for (i = 0; i < nSKFData; ++i)
		{
			pSKFD = CreateSKFConfigDesc("nameless");
			pSKFD->Load(&stream);
		}
		if (m_iVersion < 14)
		{
			CreateStandardSKFDesc(A3DModelPhysSync::CN_FROZEN_DEATH);
		}
	}
	else
	{
		CreateStandardSKFDesc();
	}
#endif

	m_bModified	= false;

	return true;
}

//	Save data to file
bool A3DModelPhysSyncData::Save(const char* szFile)
{
	// for the global anchor has the same value, enforce update mGlobalAnchor by joint data value
	if (0 != m_pSkinModel)
	{
		bool bHasWarning = false;
		NxVec3 gAnchor(0.0f);
		A3DVECTOR3 vGlobalPos(0, 0, 0);
		JOINT_DATA* pJointData = 0;
		A3DSkeleton* pSkeleton = m_pSkinModel->GetSkeleton();
		int iJointNum = m_aJoints.GetSize();
		for (int i = 0; i < iJointNum; ++i)
		{
			pJointData = m_aJoints[i];
			vGlobalPos = pJointData->GetOffset();
			A3DBone* pBone = pSkeleton->GetBone(pJointData->GetLinkedBoneName(), 0);
			if (0 != pBone)
			{
				A3DVECTOR3 vBonePos = pBone->GetUpToRootTM().GetRow(3);
				vBonePos.y -= pSkeleton->GetFootOffset();
				vGlobalPos += vBonePos;
			}
			gAnchor.set(vGlobalPos.x, vGlobalPos.y, vGlobalPos.z);

			APhysXJointDesc* pJointDesc = (APhysXJointDesc*)m_pModelPhysDesc->GetInnerConnector(i);
			if (0 != pJointDesc)
			{
				if (!gAnchor.equals(pJointDesc->mGlobalAnchor, APHYSX_FLOAT_EPSILON))
				{
					bHasWarning = true;
					pJointDesc->mGlobalAnchor = gAnchor;
				}
			}
		}
		if (bHasWarning)
			a_LogOutput(1, "A3DModelPhysSyncData::Save, JOINT_DATA中通过Offset计算的globalAnchor与APhysXJointDesc中globalAnchor数据不一致，以Offset计算结果为准更新APhysXJointDesc中的数据");
	}

	APhysXUserStream stream(szFile, false);

	if (!stream.IsValid())
	{
		a_LogOutput(1, "A3DModelPhysSyncData::Save, failed to create file %s", szFile);
		return false;
	}

	//	Write file version
	stream.storeDword(SYNCFILE_VERSION);

	//	Write file header
	SYNCFILE_HEADER fh;
	memset(&fh, 0, sizeof (fh));
	fh.iNumActor = m_aActors.GetSize();
	fh.iNumJoint = m_aJoints.GetSize();
	stream.storeBuffer(&fh, sizeof (fh));

	//	Write root bone info
	WriteString(&stream, m_strRootBone);

	int i;

	//	Write actor data
	for (i=0; i < fh.iNumActor; i++)
		WriteActorData(&stream, m_aActors[i]);
	
	//	Write joint data
	WriteJointData(&stream);

	//	Write actor and joint link info
	AArray<int, int> aIndices;

	for (i=0; i < fh.iNumJoint; i++)
	{
		JOINT_DATA* pJoint = m_aJoints[i];
		if (pJoint->aActors[0])
		{
			int iActorIdx = m_aActors.Find(pJoint->aActors[0]);
			aIndices.Add(iActorIdx);
		}
		else
			aIndices.Add(-1);

		if (pJoint->aActors[1])
		{
			int iActorIdx = m_aActors.Find(pJoint->aActors[1]);
			aIndices.Add(iActorIdx);
		}
		else
			aIndices.Add(-1);
	}

	if (aIndices.GetSize())
		stream.storeBuffer(aIndices.GetData(), aIndices.GetSize() * sizeof (int));

	//	Write attacher info
	WriteAttacherData(&stream);

	//	Write phys rb-desc creator info
	WriteRBCreator(&stream);

	//	Write collision group
	stream.storeDword((NxU32)m_iCollisionGroup);

	//	Write flag
	stream.storeDword(m_uiObjectFlag);

	//	Write soft key frame config data
#ifdef _ANGELICA21
	int nSKFData = GetSKFConfigDescNum();
	stream.storeDword(nSKFData);
	SoftKeyFrameConfigDesc* pSKFD = 0;
	for (i = 0; i < nSKFData; ++i)
	{
		pSKFD = GetSKFConfigDescByIndex(i);
		pSKFD->Save(&stream);
	}
#endif

	return true;
}

//	Write string to stream
bool A3DModelPhysSyncData::WriteString(NxStream* ps, const AString& str)
{
	//	Write length of string
	int iLen = str.GetLength();
	ps->storeDword((NxI32)iLen);

	//	Write string data
	if (iLen)
		ps->storeBuffer((const char*)str, iLen);

	return true;
}

//	Read string from stream
bool A3DModelPhysSyncData::ReadString(NxStream* ps, AString& str)
{
	//	Read length of string
	int iLen = (int)ps->readDword();

	//	Read string data
	if (iLen)
	{
		char* szBuf = new char [iLen+1];
		if (!szBuf)
			return false;

		ps->readBuffer(szBuf, iLen);
		szBuf[iLen] = '\0';
		str = szBuf;

		delete [] szBuf;
	}
	else
		str = "";

	return true;
}

//	Write A3DVECTOR to stream
bool A3DModelPhysSyncData::WriteVector(NxStream* ps, const A3DVECTOR3& vec)
{
	ps->storeFloat(vec.x);
	ps->storeFloat(vec.y);
	ps->storeFloat(vec.z);

	return true;
}

//	Read A3DVECTOR from stream
bool A3DModelPhysSyncData::ReadVector(NxStream* ps, A3DVECTOR3& vec)
{
	vec.x = ps->readFloat();
	vec.y = ps->readFloat();
	vec.z = ps->readFloat();

	return true;
}

//	Write actor data to stream
bool A3DModelPhysSyncData::WriteActorData(NxStream* ps, ACTOR_DATA* pActor)
{
	//	Actor name
	WriteString(ps, pActor->strName);

	//	Actor desc
	pActor->pActorDesc->Save(ps);

	//	Material desc
	pActor->pMaterial->Save(ps);

	//	Link data
	WriteLinkData(ps, pActor->Link);

	ps->storeDword(pActor->nFlags);

	int i;

	//	Shape data
	APhysXShapeDescManager man;
	for (i=0; i < pActor->aShapes.GetSize(); i++)
		man.CreateAndPushShapeDesc(pActor->aShapes[i]);

	if (!man.Save(ps))
		return false;

	//	Use default material or not
	ps->storeByte((NxU8)pActor->bUseDefaultMaterial);

	return true;
}

//	Write link data to stream
bool A3DModelPhysSyncData::WriteLinkData(NxStream* ps, LINK_INFO& link)
{
	//	Main bone name
	WriteString(ps, link.strMainBone);

	//	Length bone name
	WriteString(ps, link.strLenBone);

	//	Transform matrix from actor to bone
	ps->storeBuffer(&link.matActor2Bone, sizeof (link.matActor2Bone));
	
	//	Bone data number
	int iNumBone = link.aBones.GetSize();
	ps->storeDword((NxI32)iNumBone);

	int i;

	//	Bone data
	for (i=0; i < link.aBones.GetSize(); i++)
	{
		BONE_DATA* pBoneData = link.aBones[i];
		
		//	Bone name
		WriteString(ps, pBoneData->strBone);
		
		//	Transform matrix from bone to actor
		ps->storeBuffer(&pBoneData->matBone2Actor, sizeof (pBoneData->matBone2Actor));

		//	Animation major flag
		ps->storeByte((NxU8)(pBoneData->bAnimMajor ? 1 : 0));
	}

	return true;
}

//	Write joint data to stream
bool A3DModelPhysSyncData::WriteJointData(NxStream* ps)
{
	int i;

	for (i=0; i < m_aJoints.GetSize(); i++)
	{
		JOINT_DATA* pJoint = m_aJoints[i];

		//	Joint name
		WriteString(ps, pJoint->strName);

		//	Linked bone name
		WriteString(ps, pJoint->strBone);

		//	Joint offset relative to linked bone
		WriteVector(ps, pJoint->vOffset);

		ps->storeDword(pJoint->nFlags);
	}

	//	Joint desc
	APhysXObjectConnectorDescManager man;
	for (i=0; i < m_aJoints.GetSize(); i++)
		man.Add(m_aJoints[i]->pJointDesc);

	if (!man.Save(ps))
		return false;

	return true;
}

bool A3DModelPhysSyncData::WriteAttacherData(NxStream* ps)
{
	//	Attacher num
	ps->storeDword(m_aAttachers.GetSize());

	int i;

	for (i=0; i < m_aAttachers.GetSize(); ++i)
	{
		ATTACHER_DATA* pAttacher = m_aAttachers[i];

		//	Attacher name
		WriteString(ps, pAttacher->GetAttacherName());

		//	Attacher link actor name
		WriteString(ps, pAttacher->GetAttachActor());

		//	Attacher type
		ps->storeDword(pAttacher->GetAttacherType());
		
		//	Attacher rel pos
		// ps->storeBuffer(pAttacher->GetPosition().m, sizeof(A3DVECTOR3));

		WriteString(ps, pAttacher->GetHook());
	}
	return true;
}

//	Write RBCreator
bool A3DModelPhysSyncData::WriteRBCreator(NxStream* ps)
{
	if (!m_pPhysRBDescCreator)
		return false;

	A3DPhysRBCreator::StorePhysRBInfoToStream(ps, m_pPhysRBDescCreator->GetRBType());

	if (!m_pPhysRBDescCreator->Save(ps))
		return false;

	return true;
}

//	Read actor data from stream
bool A3DModelPhysSyncData::ReadActorData(NxStream* ps, ACTOR_DATA* pActor)
{
	//	Actor name
	ReadString(ps, pActor->strName);

	//	Actor desc
	pActor->pActorDesc->Load(ps);

	//	Material desc
	pActor->pMaterial->Load(ps);

	//	Link data
	ReadLinkData(ps, pActor->Link);

	if (m_iVersion >= 13)
		pActor->nFlags = ps->readDword();

	//	Shape data
	APhysXShapeDescManager man;
	if (!man.Load(ps))
		return false;

	int i, iNumShape = man.GetNum();
	for (i=0; i < iNumShape; i++)
	{
		APhysXShapeDesc* pShape = man.GetPhysXShapeDesc(i)->Clone();
		pActor->aShapes.Add(pShape);
	}

	//	Use default material or not
	if (m_iVersion >= 9)
		pActor->bUseDefaultMaterial = (ps->readByte() != 0);

	return true;
}

//	Read link data from stream
bool A3DModelPhysSyncData::ReadLinkData(NxStream* ps, LINK_INFO& link)
{
	//	Main bone name
	ReadString(ps, link.strMainBone);

	if (m_iVersion >= 3)
	{
		//	Length bone name
		ReadString(ps, link.strLenBone);
	}

	//	Transform matrix from actor to bone
	ps->readBuffer(&link.matActor2Bone, sizeof (link.matActor2Bone));
	
	//	Bone data number
	int iNumBone = (int)ps->readDword();

	int i;

	//	Bone data
	for (i=0; i < iNumBone; i++)
	{
		BONE_DATA* pBoneData = new BONE_DATA;
		
		//	Bone name
		ReadString(ps, pBoneData->strBone);
		
		//	Transform matrix from bone to actor
		ps->readBuffer(&pBoneData->matBone2Actor, sizeof (pBoneData->matBone2Actor));

		//	Animation major flag
		pBoneData->bAnimMajor = ps->readByte() ? true : false;

		link.aBones.Add(pBoneData);
	}

	return true;
}

//	Read joint data from stream
bool A3DModelPhysSyncData::ReadJointData(NxStream* ps, int iNumJoint)
{
	int i;

	for (i=0; i < iNumJoint; i++)
	{
		JOINT_DATA* pJoint = new JOINT_DATA;

		//	Joint name
		ReadString(ps, pJoint->strName);

		//	Linked bone name
		if (m_iVersion >= 2)
			ReadString(ps, pJoint->strBone);

		//	Offset relative to linked bone
		if (m_iVersion >= 7)
			ReadVector(ps, pJoint->vOffset);

		if (m_iVersion >= 12)
			pJoint->nFlags = ps->readDword();

		m_aJoints.Add(pJoint);
	}

	//	Joint desc
	APhysXObjectConnectorDescManager man;
	if (!man.Load(ps))
		return false;

	if (man.GetNum() != iNumJoint)
	{
		ASSERT(man.GetNum() == iNumJoint);
		return false;
	}

	for (i=0; i < iNumJoint; i++)
	{
		JOINT_DATA* pJoint = m_aJoints[i];
		pJoint->pJointDesc = (APhysXJointDesc*)man.GetPhysXConnectorDesc(i)->Clone();
	}

	return true;
}

//	Read attacher data from stream
bool A3DModelPhysSyncData::ReadAttacherData(NxStream* ps)
{
	if (m_iVersion <= 3)
		return true;

	int iNum = 0;
	iNum = (int)ps->readDword();

	int i;
	for (i=0; i < iNum; ++i)
	{
		AString strAttacher, strActor, strHook;
		A3DVECTOR3 vPos;

		if (!ReadString(ps, strAttacher))
			return false;

		if (!ReadString(ps, strActor))
			return false;

		int nType = (int)ps->readDword();

		if (m_iVersion <= 4)
			ps->readBuffer(vPos.m, sizeof(A3DVECTOR3));
		else
			ReadString(ps, strHook);

		ATTACHER_DATA* pAttacher = new ATTACHER_DATA(strAttacher, strActor, strHook, nType);

		m_aAttachers.Add(pAttacher);
	}
	return true;
}

//	Read RBCreator
bool A3DModelPhysSyncData::ReadRBCreator(NxStream* ps)
{
	if (m_iVersion <= 5)
		return true;

	if (m_pPhysRBDescCreator)
	{
		delete m_pPhysRBDescCreator;
		m_pPhysRBDescCreator = NULL;
	}

	m_pPhysRBDescCreator = A3DPhysRBCreator::CreatePhysRBInfoFromStream(ps);
	if (!m_pPhysRBDescCreator)
		return false;

	if (!m_pPhysRBDescCreator->Load(ps))
		return false;

	return true;
}



#endif	//	A3D_PHYSX
