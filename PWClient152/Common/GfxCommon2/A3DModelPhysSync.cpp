/*
 * FILE: A3DModelPhysSync.cpp
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

#include "A3DModelPhysSync.h"
#include "A3DModelPhysSyncData.h"

#include "APhysX.h"
#include "EC_Model.h"

#include <A3DSkinModel.h>
#include <A3DSkeleton.h>
#include <memory>

#ifdef _ANGELICA21
#include "A3DSkeletonBlend.h"
#endif

//------------------------------------------------------------------------
// for breakable skeleton objects...

class ASkinModelBreakReport : public APhysXUserBreakReport
{
	
public:

	virtual void OnBreakNotify(APhysXBreakable* pBreakable)
	{

		APhysXObject* pBreakObj = pBreakable->IsAPhysXObject();

		if(pBreakObj)
		{
		
			PhysBreakInfo physBreakInfo;
			physBreakInfo.mBreakPos = APhysXConverter::N2A_Vector3(pBreakable->GetBreakPos());
			physBreakInfo.mBreakMagnitude = pBreakable->GetBreakMagnitude();

			if(pBreakObj->IsObjectType(APX_OBJTYPE_SKELETON_SIMPLE_BREAKABLE) && pBreakObj->mUserData)
			{
				A3DModelPhysSync* pSkinModelPhysSync = (A3DModelPhysSync*)pBreakObj->mUserData;
				pSkinModelPhysSync->OnBreak(physBreakInfo);
		
			}

			if(pBreakObj->IsObjectType(APX_OBJTYPE_SKELETON_BREAKABLE) && pBreakObj->mUserData)
			{
				A3DModelPhysSync* pSkinModelPhysSync = (A3DModelPhysSync*)pBreakObj->mUserData;
				pSkinModelPhysSync->OnBreak(physBreakInfo);
			}

		}

	}

	virtual void OnFirstBreakNotify(APhysXBreakable* pBreakable) 
	{
		APhysXObject* pBreakObj = pBreakable->IsAPhysXObject();
		
		if(pBreakObj)
		{
			if(pBreakObj->IsObjectType(APX_OBJTYPE_SKELETON_SIMPLE_BREAKABLE) && pBreakObj->mUserData)
			{
				A3DModelPhysSync* pSkinModelPhysSync = (A3DModelPhysSync*)pBreakObj->mUserData;
				pSkinModelPhysSync->OnFirstBreak();
				
			}
			
			if(pBreakObj->IsObjectType(APX_OBJTYPE_SKELETON_BREAKABLE) && pBreakObj->mUserData)
			{
				A3DModelPhysSync* pSkinModelPhysSync = (A3DModelPhysSync*)pBreakObj->mUserData;
				pSkinModelPhysSync->OnFirstBreak();
				
			}
			
		}
	}
	
	virtual void OnAllBreakNotify(APhysXBreakable* pBreakable) 
	{
		APhysXObject* pBreakObj = pBreakable->IsAPhysXObject();
		
		if(pBreakObj)
		{
			if(pBreakObj->IsObjectType(APX_OBJTYPE_SKELETON_SIMPLE_BREAKABLE) && pBreakObj->mUserData)
			{
				A3DModelPhysSync* pSkinModelPhysSync = (A3DModelPhysSync*)pBreakObj->mUserData;
				pSkinModelPhysSync->OnAllBreak();
				
			}
			
			if(pBreakObj->IsObjectType(APX_OBJTYPE_SKELETON_BREAKABLE) && pBreakObj->mUserData)
			{
				A3DModelPhysSync* pSkinModelPhysSync = (A3DModelPhysSync*)pBreakObj->mUserData;
				pSkinModelPhysSync->OnAllBreak();
				
			}
			
		}
	}

	ASkinModelBreakReport()
	{
		gPhysXBreakReport->Register(this);
	}


};

// try to register this break report after the APhysXScene is created...
static ASkinModelBreakReport gSkinModelBreakReport;

class AECMHitGroundReport: public NxUserContactReport
{
public:
	
	// Note: it will always be the case that when some contact occurs, actor[0] is dynamic and actor[1] is static.

	virtual void onContactNotify(NxContactPair& pair, NxU32 events)
	{
		if(events == NX_NOTIFY_ON_START_TOUCH)	
		{
			if(pair.isDeletedActor[0] || pair.isDeletedActor[1])
				return;

			APhysXRigidBodyObject* pRBObject0 =	(APhysXRigidBodyObject*)pair.actors[0]->userData;
			APhysXRigidBodyObject* pRBObject1 =	(APhysXRigidBodyObject*)pair.actors[1]->userData;	

			if (!pRBObject0 || !pRBObject1 || pRBObject0 == pRBObject1)
				return;

			bool bRB0_Static = pRBObject0->IsStaticRB();
			bool bRB1_Static = pRBObject1->IsStaticRB();
			if (bRB0_Static == bRB1_Static)
				return;

			if (!(bRB0_Static || bRB1_Static))
				return;

			NxActor* pHitActor = 0;
			if (bRB0_Static)
				pHitActor = pair.actors[1];
			else
				pHitActor = pair.actors[0];

			APhysXRigidBodyObject* pRBObj_NoStatic = bRB0_Static ? pRBObject1 : pRBObject0;
			A3DModelPhysSync* pSkinModelPhysSync = (A3DModelPhysSync*)pRBObj_NoStatic->mUserData;
			if (!pSkinModelPhysSync)
				return;

			A3DModelPhysics* pModelPhysics = pSkinModelPhysSync->GetA3DModelPhysics();
			if (!pModelPhysics)
				return;

			CECModel* pECModel = pModelPhysics->GetECModel();
			if (!pECModel)
				return;

			A3DModelPhysics::EMBED_ACTOR* pEmbedActor = pModelPhysics->GetEmbeddableActor(pHitActor);
			if (0 != pEmbedActor)
			{
				NxVec3 vNormal = pair.sumNormalForce;
				vNormal.normalize();
				if (bRB0_Static)
					vNormal = -vNormal;

				bool bToCleared = true;
				NxVec3 vAxisX = pHitActor->getGlobalOrientation().getColumn(0);
				float dot = vAxisX.dot(vNormal);
				if (-0.4f > dot)
				{
					NxVec3 velDir = pHitActor->getLinearVelocity();
					float fVel = velDir.normalize();
					float dotVX = vAxisX.dot(velDir);
					if (0.5 < dotVX)
					{
						if (5 < fVel)
						{
							float dotVPN = vNormal.dot(velDir);
							if (-0.5 > dotVPN)
							{
								bToCleared = false;
								pEmbedActor->bToEmbedAction = true;
								NxVec3 vAxisX = pEmbedActor->apxForceInfo.mActor->getGlobalOrientation().getColumn(0);
								NxVec3 pos = pEmbedActor->apxForceInfo.mActor->getGlobalPosition();
								pos += vAxisX * pEmbedActor->fHalfExtentX * 0.4f;
								pEmbedActor->apxForceInfo.mPos = pos;
							}
						}
					}
				}
				if (bToCleared)
					pModelPhysics->ClearEmbeddableActor(pEmbedActor);
			}

			// compute the central contact pos
			NxContactStreamIterator csi(pair.stream);
			int iPointNum = 0;
			NxVec3 vCentralContactPos(0.0f);
			while(csi.goNextPair())
			{
				//user can also call getShape() and getNumPatches() here
				while(csi.goNextPatch())
				{
					//user can also call getPatchNormal() and getNumPoints() here
					const NxVec3& contactNormal = csi.getPatchNormal();

					while(csi.goNextPoint())
					{
						iPointNum++;				
						//user can also call getPoint() and getSeparation() here
						const NxVec3& contactPoint = csi.getPoint();
						vCentralContactPos += contactPoint;
					}
				}
			}

			vCentralContactPos /= (float)iPointNum;
			
			ECMPhysXHitGroundInfo hgInfo;
			hgInfo.mHitActor = pHitActor;
			hgInfo.mHitNormalForce = APhysXConverter::N2A_Vector3(pair.sumNormalForce);
			hgInfo.mHitPos = APhysXConverter::N2A_Vector3(vCentralContactPos);

			pECModel->OnPhysHitGround(hgInfo);
		}		
	}

	AECMHitGroundReport()
	{
		gPhysXContactReport->Register(this);
	}
};

static AECMHitGroundReport s_ECMHitGroundReport;


#ifdef _ANGELICA21
//------------------------------------------------------------------------

// code snippets for soft key-frame
void PhysD6JointDriveInfo::onReleaseDesc(SoftKeyFrameConfigDesc& skfDesc)
{
	if (&skfDesc == pAppliedSKFDesc)
	{
		pAppliedSKFDesc = 0;
		mKinematicActors.clear();
	}
}

#endif
/*

struct PhysXJointDrivePara
{
	float mSpringRatio;
	float mDampingRatio;

	PhysXJointDrivePara(float fSpringRatio, float fDampingRatio): 
		mSpringRatio(fSpringRatio), mDampingRatio(fDampingRatio)
	{

	}

	PhysXJointDrivePara():mSpringRatio(1.0f), mDampingRatio(1.0f)
	{

	}
};

typedef abase::hash_map<AString, PhysXJointDrivePara> PhysXJointDriveParaMap;
typedef PhysXJointDriveParaMap::value_type PhysXJointDriveParaPair;

struct PhysXJointDriveConfigs: public PhysXJointDriveParaMap
{

	PhysXJointDriveConfigs()
	{
		// head
		this->insert(PhysXJointDriveParaPair("Joint_spine1_head", PhysXJointDrivePara(1.0f, 1.0f)));

		this->insert(PhysXJointDriveParaPair("Joint_pelvis_spine1", PhysXJointDrivePara(1.0f, 1.0f)));
		this->insert(PhysXJointDriveParaPair("Joint_spine1_head", PhysXJointDrivePara(1.0f, 1.0f)));
		this->insert(PhysXJointDriveParaPair("Joint_spine1_head", PhysXJointDrivePara(1.0f, 1.0f)));
		this->insert(PhysXJointDriveParaPair("Joint_spine1_head", PhysXJointDrivePara(1.0f, 1.0f)));
		this->insert(PhysXJointDriveParaPair("Joint_spine1_head", PhysXJointDrivePara(1.0f, 1.0f)));
		this->insert(PhysXJointDriveParaPair("Joint_spine1_head", PhysXJointDrivePara(1.0f, 1.0f)));
		this->insert(PhysXJointDriveParaPair("Joint_spine1_head", PhysXJointDrivePara(1.0f, 1.0f)));
		this->insert(PhysXJointDriveParaPair("Joint_spine1_head", PhysXJointDrivePara(1.0f, 1.0f)));
		this->insert(PhysXJointDriveParaPair("Joint_spine1_head", PhysXJointDrivePara(1.0f, 1.0f)));
		this->insert(PhysXJointDriveParaPair("Joint_spine1_head", PhysXJointDrivePara(1.0f, 1.0f)));
	}


};

static PhysXJointDriveConfigs s_CommonPhysXJointDriveConfigs;

*/

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


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
//	Implement A3DModelPhysSync::ACTOR_IMP
//	
///////////////////////////////////////////////////////////////////////////

A3DModelPhysSync::ACTOR_IMP::ACTOR_IMP()
{
	bIsPinned = false;
	pActor	= NULL;
	pActorData = NULL;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement A3DModelPhysSync::BONE_MAT
//	
///////////////////////////////////////////////////////////////////////////

A3DModelPhysSync::BONE_MAT::BONE_MAT()
: matBoneAbs(A3DMATRIX4::IDENTITY)
, matBoneInvAbs(A3DMATRIX4::IDENTITY)
{
	bMainBone = false;
}

A3DModelPhysSync::BONE_SCALE_CACHE_DATA::BONE_SCALE_CACHE_DATA(A3DBone* pBone)
: fLocalLengthScale(pBone->GetLocalLengthScale())
, fLocalThickScale(pBone->GetLocalThickScale())
, fAccuWholeScale(pBone->GetAccuWholeScale())
{

}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement A3DModelPhysSync
//	
///////////////////////////////////////////////////////////////////////////
NxForceMode A3DModelPhysSync::TranslateForceMode(int iType)
{
	NxForceMode nxForceMode = NX_FORCE;
	switch (iType)
	{
	case CECModel::PFT_FORCE:
		nxForceMode = NX_FORCE;
		break;
	case CECModel::PFT_IMPULSE:
		nxForceMode = NX_IMPULSE;
		break;
	case CECModel::PFT_VELOCITY_CHANGE:
		nxForceMode = NX_VELOCITY_CHANGE;
		break;
	case CECModel::PFT_SMOOTH_IMPULSE:
		nxForceMode = NX_SMOOTH_IMPULSE;
		break;
	case CECModel::PFT_SMOOTH_VELOCITY_CHANGE:
		nxForceMode = NX_SMOOTH_VELOCITY_CHANGE;
		break;
	case CECModel::PFT_ACCELERATION:
		nxForceMode = NX_ACCELERATION;
		break;
	default:
		ASSERT(FALSE && "Invalid Force Type.");
		break;
	}
	return nxForceMode;
}

#ifdef _ANGELICA21
bool A3DModelPhysSync::ConvertSKFDescToDriveInfo(PhysD6JointDriveInfo& outDI, const SoftKeyFrameConfigDesc& rSKFDesc, const A3DModelPhysSync& rMPS)
{
	outDI.mKinematicActors.clear();
	NxActor* pActor = 0;
	AString strBone;
	int nFixActors = rSKFDesc.GetNBFixActorBones();
	for (int i = 0; i < nFixActors; ++i)
	{
		if (rSKFDesc.GetFixActorBone(i, strBone))
		{
			pActor = rMPS.GetNxActor(strBone);
			if (0 != pActor)
				APhysXUtilLib::APhysXArray_PushOnce(outDI.mKinematicActors, pActor);
		}
	}

	//--------------------------------------------------
	// Noted by Wenfeng, Nov. 6, 2012
	//
	// considering some non human body like monsters such as smokes, we have to
	// fix some actors when they are in Soft-Keyframe mode
	//

	// temp workaround, only used for injury case
	if (0 == outDI.mKinematicActors.size())
	{
		AString strInjury = GetDefSKFConfigName(CN_INJURY);
		if (strInjury == rSKFDesc.GetName())
		{
			// firstly try the foot actor...
			A3DSkinModel* pSM = rMPS.GetSkinModel();
			A3DSkeleton* pSkeleton = pSM->GetSkeleton();
			if (pSkeleton)
			{
				A3DBone* pFootBone = pSkeleton->GetBone(pSkeleton->GetFootBone());
				if(pFootBone)
				{
					pActor = rMPS.GetNxActor(pFootBone);
					if (0 != pActor)
						outDI.mKinematicActors.push_back(pActor);
				}
				else
				{
					// if no foot bone are found, just push the root bone
					A3DBone* pRootBone = pSkeleton->GetBone(pSkeleton->GetRootBone(0));
					pActor = rMPS.GetNxActor(pRootBone);
					if (0 != pActor)
						outDI.mKinematicActors.push_back(pActor);
				}
			}
		}
	}
	
	return true;
}
#endif

const char* A3DModelPhysSync::GetDefSKFConfigName(DefSKFConfigName id)
{
	switch (id)
	{
	case CN_INJURY:
		return "Injury_SKFD";
	case CN_PICK_THROW:
		return "PickThrow_SKFD";
	case CN_FROZEN_DEATH:
		return "FrozenDEATH_SKFD";
	default:
		assert(!"Unknown config id!");
	}
	return 0;
}

A3DModelPhysSync::A3DModelPhysSync(A3DModelPhysics* pModelPhysics)
: m_pModelPhysics(pModelPhysics)
{
	ASSERT(m_pModelPhysics);

	m_pPhysScene	= NULL;
	m_pCoreData		= NULL;
	m_pPhysModel	= NULL;
	m_pPhysAttacher	= NULL;
	m_pSkinModel	= NULL;
	m_bPhysBindOk	= false;
	m_bPhysBroken	= false;
	m_iPhysState	= PHY_STATE_ANIMATION;

#ifdef _ANGELICA21
	m_pSkBlend     =  NULL;
	m_pAnimWeight  =  NULL;
	m_bSetAnimWeight = false;

	m_pPhysD6JointDriveInfo = new PhysD6JointDriveInfo;
#endif

	m_iRBSimMode	= RB_SIMMODE_HARD_KEYFRAME;
	m_bIsUsingSelfCollision = false;
	m_bIsUsingLinearDrives  = false;
	m_bIsUsingAngularDrives = false;
	m_EnableLinearDrives  = false;
	m_EnableAngularDrives = true;
	m_bIsEnabledSoftKeyFrame = false;
	m_bToOpenCloth = false;

	m_bEnableAnim = true;
	m_bEnableSyncPoseByAnim = true;

	AddRotationBone("Bip01 Pelvis", true);
	AddRotationBone("Bip01 Spine", false);
	AddRotationBone("Bip01 Spine1", false);
	AddRotationBone("Bip01 Spine2", false);
}

A3DModelPhysSync::~A3DModelPhysSync()
{
#ifdef _ANGELICA21
	if (0 != m_pCoreData)
		m_pCoreData->UnRegisterSKFNotify(m_pPhysD6JointDriveInfo);
	delete m_pPhysD6JointDriveInfo;
	m_pPhysD6JointDriveInfo = 0;
#endif

	//	Release physical attacher (including phys model)
	ReleasePhysAttacher();

	//	Release all actor implements
	ReleaseActorAndJointImps();

	//	Release all attacher link info
	ReleaseAttacherLinkInfo();

	m_aBoneMats.RemoveAll();

	RemoveAllRotationBone();

#ifdef _ANGELICA21
	delete m_pSkBlend;
	delete[] m_pAnimWeight;
#endif
}

//	Release physical attacher
void A3DModelPhysSync::ReleasePhysAttacher()
{
	if (m_pPhysAttacher && m_pPhysScene)
	{
		m_pPhysScene->ReleasePhysXObject(m_pPhysAttacher);
		m_pPhysAttacher = NULL;
		m_pPhysModel = NULL;
	}
}

/*	Bind skin model with a physical object

	bReBindAll: if this flag is set to false, we assume that transform matrices between bone and actor have been 
			calculated and they won't be changed. otherwise (bReBindAll = true) all the transform matrices will
			be re-calculated.

	bRebuildDesc: if this flag is set to true, the old cached attacher-desc would be released and re-build a new one
*/
bool A3DModelPhysSync::Bind(A3DSkinModel* pSkinModel, A3DModelPhysSyncData* pCoreData, APhysXScene* pPhysScene, bool bRebuildDesc /*= false*/)
{
	if (!pSkinModel || !pCoreData || !pPhysScene)
		return false;

	if (!pCoreData->GetBindInitFlag())
		return false;

	m_bPhysBindOk	= false;
	m_bPhysBroken	= false;
	m_pPhysScene	= pPhysScene;
	m_pSkinModel	= pSkinModel;
#ifdef _ANGELICA21
	if (0 != m_pCoreData)
		m_pCoreData->UnRegisterSKFNotify(m_pPhysD6JointDriveInfo);
#endif
	m_pCoreData		= pCoreData;
#ifdef _ANGELICA21
	m_pCoreData->RegisterSKFNotify(m_pPhysD6JointDriveInfo);
#endif

	//	Create new physical attacher including physical model
	
	gGetAPhysXScene()->GetPerfStats()->ResumePerfStat("AMPS_CreatePhysX");
	if (!CreatePhysAttacherAndModel(pSkinModel, bRebuildDesc))
	{
		
		gGetAPhysXScene()->GetPerfStats()->PausePerfStat("AMPS_CreatePhysX");
		return false;
	}
	
	gGetAPhysXScene()->GetPerfStats()->PausePerfStat("AMPS_CreatePhysX");

	//	Build actor implements
	
	gGetAPhysXScene()->GetPerfStats()->ResumePerfStat("AMPS_BuildImp");
	if (!BuildActorAndJointImps())
	{
		
		gGetAPhysXScene()->GetPerfStats()->PausePerfStat("AMPS_BuildImp");
		return false;
	}
	
	gGetAPhysXScene()->GetPerfStats()->PausePerfStat("AMPS_BuildImp");

	//	Allocalte space for transform data
	A3DSkeleton* pSkeleton = m_pSkinModel->GetSkeleton();
	if (!pSkeleton)
		return false;

	int iBoneNum = pSkeleton->GetBoneNum();
	m_aBoneMats.SetSize(iBoneNum, 4);

	m_bPhysBindOk = true;

	//	After creation, set all attachers be dynamic
	
	gGetAPhysXScene()->GetPerfStats()->ResumePerfStat("AMPS_EnableK");
	m_pPhysAttacher->EnableAttacherKinematic(false);
	
	gGetAPhysXScene()->GetPerfStats()->PausePerfStat("AMPS_EnableK");

#ifdef _ANGELICA21

	
	gGetAPhysXScene()->GetPerfStats()->ResumePerfStat("AMPS_SKBlend");

	if (m_pSkBlend)
		delete m_pSkBlend;
	m_pSkBlend = new A3DSkeletonBlend;
	m_pSkBlend->Init(pSkeleton);
	if (m_pAnimWeight)
		delete[] m_pAnimWeight;
	m_pAnimWeight = new float[pSkeleton->GetBoneNum()];
	SetAnimBlendWeight(NULL, false);

	//-----------------------------------------------------
	// revised by wenfeng, Dec. 24, 2012
	// as for the breakable objects, we turn off the limit of bone's orientation change
	// 	if(m_pPhysModel->IsBreakable())
	{
		m_pSkBlend->SetLimitQuatDiff(false);
	}
	//-----------------------------------------------------

	
	gGetAPhysXScene()->GetPerfStats()->PausePerfStat("AMPS_SKBlend");

#endif

	//	Apply current physical state
	
	gGetAPhysXScene()->GetPerfStats()->ResumePerfStat("AMPS_ChangePS");
	ChangePhysState(m_iPhysState);
	
	gGetAPhysXScene()->GetPerfStats()->PausePerfStat("AMPS_ChangePS");
	
	return true;
}

//	Check whether the skeleton is scaled
bool A3DModelPhysSync::CheckSkeletonScaled(A3DSkeleton* pSkeleton)
{
	ASSERT(pSkeleton);

	const float fScaleDelta = 0.001f;

	if (!pSkeleton)
		return false;

	for (int iBoneIdx = 0; iBoneIdx < pSkeleton->GetBoneNum(); ++iBoneIdx)
	{
		A3DBone* pBone = pSkeleton->GetBone(iBoneIdx);
		ASSERT(pBone);

		if (fabs(pBone->GetLocalLengthScale() - 1.f) > fScaleDelta
			|| fabs(pBone->GetLocalThickScale() - 1.f) > fScaleDelta
			|| fabs(pBone->GetWholeScale() - 1.f) > fScaleDelta
			|| fabs(pBone->GetAccuWholeScale() - 1.f) > fScaleDelta)

			return true;
	}

	return false;
}

float A3DModelPhysSync::FormatFloat(float fValue) const
{
	int intPart = (int)fValue;
	float numPart = fValue - intPart;

	float fTemp = numPart * 1000 + 0.5f;
	numPart = int(fTemp) * 0.001f;
	
	float newVal = intPart + numPart;
	return newVal;
}

//	Create physical model
bool A3DModelPhysSync::CreatePhysAttacherAndModel(A3DSkinModel* pSkinModel, bool bRebuildDesc)
{
	if (!m_pPhysScene || !m_pCoreData || !m_pCoreData->GetModelPhysDesc())
		return false;

	APhysXRBAttacher* pAttacherObj = NULL;
	
	//	Set model's pose 
	NxMat34 mtPose;
	APhysXConverter::A2N_Matrix44(pSkinModel->GetAbsoluteTM(), mtPose);

	if (!bRebuildDesc && !CheckSkeletonScaled(pSkinModel->GetSkeleton()))
	{
		
		gGetAPhysXScene()->GetPerfStats()->ResumePerfStat("AMPS_NoScaleCreate");
		
		APhysXObjectInstanceDesc apxObjInstanceDesc;
		apxObjInstanceDesc.mPhysXObjectDesc = m_pCoreData->GetAttacherPhysDesc();
		apxObjInstanceDesc.mGlobalPose = mtPose;
		apxObjInstanceDesc.mScale3D = NxVec3(1.0f);
		pAttacherObj = (APhysXRBAttacher*)m_pPhysScene->CreatePhysXObject(apxObjInstanceDesc);

		
		gGetAPhysXScene()->GetPerfStats()->PausePerfStat("AMPS_NoScaleCreate");
	}
	else
	{
		
		gGetAPhysXScene()->GetPerfStats()->ResumePerfStat("AMPS_ScaleCreate");

		//	Use RRID (Resource release is destruction) to delete the pointer at the destructing time
		std::auto_ptr<APhysXSkeletonRBObjectDesc> pObjectDesc((APhysXSkeletonRBObjectDesc*)m_pCoreData->GetModelPhysDesc()->Clone());
		if (!pObjectDesc.get())
			return false;
		
		//	Conside model's scale factor
		A3DSkeleton* pSkeleton = pSkinModel->GetSkeleton();
		if (!pSkeleton)
			return false;
		
		int i, iActorNum = m_pCoreData->GetActorNum();
		A3DMATRIX4 matLocalPose, matUpToRoot;
		A3DVECTOR3 vOrigin, vAxisX, vAxisY, vAxisZ;
		
		for (i=0; i < iActorNum; i++)
		{
			A3DModelPhysSyncData::ACTOR_DATA* pActorData = m_pCoreData->GetActorByIndex(i);
			A3DBone* pMainBone = pSkeleton->GetBone(pActorData->Link.iMainBone);
			if (!pMainBone)
				return false;
			
			//	Get the child bone that decides actor's length
			A3DBone* pLenBone = NULL;
			if (pActorData->Link.iLenBone >= 0)
				pLenBone = pSkeleton->GetBone(pActorData->Link.iLenBone);
			
			vOrigin = pActorData->Link.matActor2Bone.GetRow(3);
			vAxisY = vOrigin + pActorData->Link.matActor2Bone.GetRow(1);
			vAxisX = vOrigin + pActorData->Link.matActor2Bone.GetRow(0);
			
			matUpToRoot = pMainBone->GetScaleMatrix() * pMainBone->GetUpToRootTM();
			vOrigin = vOrigin * matUpToRoot;
			vAxisY = vAxisY * matUpToRoot - vOrigin;
			vAxisX = vAxisX * matUpToRoot - vOrigin;
			
			float fLenY = vAxisY.Normalize();
			float fLenFactor = pLenBone ? pLenBone->GetLocalLengthScale() * pMainBone->GetAccuWholeScale() : fLenY;
			float fRadFactor = vAxisX.Normalize();
			vAxisZ = CrossProduct(vAxisX, vAxisY);
			
			//	Conside foot offset
			vOrigin.y -= pSkeleton->GetFootOffset();
			
			matLocalPose.SetRow(0, vAxisX);
			matLocalPose.SetRow(1, vAxisY);
			matLocalPose.SetRow(2, vAxisZ);
			matLocalPose.SetRow(3, vOrigin);
			
			fLenFactor = FormatFloat(fLenFactor);
			fRadFactor = FormatFloat(fRadFactor);

			APhysXActorDesc* pActorDesc = pObjectDesc->GetActorDesc(i);
			APhysXConverter::A2N_Matrix44(matLocalPose, pActorDesc->mLocalPose);
			pActorDesc->mScale3D.set(fRadFactor, fLenFactor, fRadFactor);
		}
		
		//	Adjust joint's position
		int iJointNum = m_pCoreData->GetJointNum();
		for (i=0; i < iJointNum; i++)
		{
			A3DModelPhysSyncData::JOINT_DATA* pJointData = m_pCoreData->GetJointByIndex(i);
			A3DBone* pBone = pSkeleton->GetBone(pJointData->iBoneIdx);
			if (!pBone)
				return false;
			
			//	Adjust joint's position
			vOrigin = pBone->GetUpToRootTM().GetRow(3);
			vOrigin.y -= pSkeleton->GetFootOffset();
			vOrigin += pJointData->GetOffset() * pBone->GetAccuWholeScale();
			APhysXJointDesc* pJointDesc = (APhysXJointDesc*)pObjectDesc->GetInnerConnector(i);
			pJointDesc->mGlobalAnchor = APhysXConverter::A2N_Vector3(vOrigin);
		}
		
		//	Create attacher desc by skeleton desc we generated previously
		//	We need to delay the creation of attacher's desc to this moment because the skeleton desc may be changed on scaling
		//	This creation would generate all the attachers we created in the editor by adding attacher
		std::auto_ptr<APhysXAttacherDesc> pAttacherDesc(m_pCoreData->CreateAttacherPhysDescBySkeletonDesc(pObjectDesc.get(), m_pSkinModel->GetSkeleton()));
		if (!pAttacherDesc.get())
			return false;

		APhysXObjectInstanceDesc apxObjInstanceDesc;
		apxObjInstanceDesc.mPhysXObjectDesc = pAttacherDesc.get();
		apxObjInstanceDesc.mGlobalPose = mtPose;
		apxObjInstanceDesc.mScale3D = NxVec3(1.0f);
		pAttacherObj = (APhysXRBAttacher*)m_pPhysScene->CreatePhysXObject(apxObjInstanceDesc);

		
		gGetAPhysXScene()->GetPerfStats()->PausePerfStat("AMPS_ScaleCreate");
	}


	if (!pAttacherObj)
		return false;

	ReleasePhysAttacher();
	m_pPhysAttacher = pAttacherObj;

	// Get the physical model's pointer out instead of creating a separate physical model object 
	m_pPhysModel = dynamic_cast<APhysXSkeletonRBObject*>(pAttacherObj->GetRBObject());

	if (m_pPhysModel)
		m_pPhysModel->mUserData = this;

	return true;
}

//	Release all actor implements
void A3DModelPhysSync::ReleaseActorAndJointImps()
{
	DestoryTorqueActors();

	for (int i=0; i < m_aActorImps.GetSize(); i++)
	{
		ACTOR_IMP* pImp = m_aActorImps[i];
		delete pImp;
	}

	m_aActorImps.RemoveAll();


	for (int i=0; i < m_aJointImps.GetSize(); i++)
	{
		JOINT_IMP* pImp = m_aJointImps[i];
		delete pImp;
	}

	m_aJointImps.RemoveAll();
}

//	Release all attacher link infos
void A3DModelPhysSync::ReleaseAttacherLinkInfo()
{
	for (int i = 0; i < m_aAttacherLinks.GetSize(); ++i)
	{
		ATTACHER_LINK* pLink = m_aAttacherLinks[i];
		delete pLink;
	}

	m_aAttacherLinks.RemoveAll();
}

//	Build actor implements
bool A3DModelPhysSync::BuildActorAndJointImps()
{
	//	Release old actor implements

	gGetAPhysXScene()->GetPerfStats()->ResumePerfStat("AMPS_ReleaseImp");
	ReleaseActorAndJointImps();
	gGetAPhysXScene()->GetPerfStats()->PausePerfStat("AMPS_ReleaseImp");

	if (!m_pPhysModel || !m_pCoreData)
		return false;

	
	int i, iActorNum = m_pCoreData->GetActorNum();
	if(iActorNum != m_pPhysModel->GetNxActorNum())
	{
		gPhysXLog->Log("Failed when call BuildActorAndJointImps() for ECModel(%s)", m_pSkinModel->GetFileName());
		return false;
	}

	gGetAPhysXScene()->GetPerfStats()->ResumePerfStat("AMPS_BuildActorImp");

	for (i=0; i < iActorNum; i++)
	{
		A3DModelPhysSyncData::ACTOR_DATA* pActorData = m_pCoreData->GetActorByIndex(i);
		if (!pActorData)
		{
			gGetAPhysXScene()->GetPerfStats()->PausePerfStat("AMPS_BuildActorImp");
			return false;
		}

		ACTOR_IMP* pImp = new ACTOR_IMP;

		pImp->pActorData = pActorData;

		pImp->pActor = m_pPhysModel->GetNxActor(i);
		if (!pImp->pActor)
		{
			gGetAPhysXScene()->GetPerfStats()->PausePerfStat("AMPS_BuildActorImp");
			return false;
		}

		const char* szName = pImp->pActor->getName();
		if (!szName || strcmp(szName, pActorData->strName) != 0)
		{
			gGetAPhysXScene()->GetPerfStats()->PausePerfStat("AMPS_BuildActorImp");
			return false;
		}

		m_aActorImps.Add(pImp);
	}
	gGetAPhysXScene()->GetPerfStats()->PausePerfStat("AMPS_BuildActorImp");


	gGetAPhysXScene()->GetPerfStats()->ResumePerfStat("AMPS_BuildJointImp");
	int iJointNum = m_pCoreData->GetJointNum();
	for (i=0; i < iJointNum; i++)
	{
		A3DModelPhysSyncData::JOINT_DATA* pJointData = m_pCoreData->GetJointByIndex(i);
		JOINT_IMP* pJointImp = new JOINT_IMP;
		pJointImp->pJointData = pJointData;

		APhysXJoint* pJoint = m_pPhysModel->GetJoint(pJointData->strName);
		if(pJoint && pJoint->GetNxJoint())
		{
			pJointImp->pD6Joint = pJoint->GetNxJoint()->isD6Joint();
			pJointImp->strJointName = pJointData->strName;
			pJointImp->pD6Joint->getActors(&(pJointImp->pActor0), &(pJointImp->pActor1));

			// save the local anchor position for use of X, Y, Z drives.
			NxD6JointDesc d6Desc;
			pJointImp->pD6Joint->saveToDesc(d6Desc);
			pJointImp->mLocalAnchor0 = d6Desc.localAnchor[0];
			pJointImp->mLocalAnchor1 = d6Desc.localAnchor[1];
		}
		
		NxVec3 vX, vY, vZ;
		vX = pJointData->GetJointDesc()->mLocalAxis[0];
		vY = pJointData->GetJointDesc()->mLocalNormal[0];
		vZ = vX ^ vY;

		pJointImp->mLocalRot0.setColumn(0, vX);
		pJointImp->mLocalRot0.setColumn(1, vY);
		pJointImp->mLocalRot0.setColumn(2, vZ);

		vX = pJointData->GetJointDesc()->mLocalAxis[1];
		vY = pJointData->GetJointDesc()->mLocalNormal[1];
		vZ = vX ^ vY;

		pJointImp->mLocalRot1.setColumn(0, vX);
		pJointImp->mLocalRot1.setColumn(1, vY);
		pJointImp->mLocalRot1.setColumn(2, vZ);

		m_aJointImps.Add(pJointImp);
	}
	gGetAPhysXScene()->GetPerfStats()->PausePerfStat("AMPS_BuildJointImp");

#ifdef _ANGELICA21

	gGetAPhysXScene()->GetPerfStats()->ResumePerfStat("AMPS_ApplySKF");

	if (0 < m_pCoreData->GetSKFConfigDescNum())
	{
		SoftKeyFrameConfigDesc* pSKFD = m_pCoreData->GetSKFConfigDescByIndex(0);
		ApplySoftKeyFrameConfig(pSKFD->GetName());
	}
	gGetAPhysXScene()->GetPerfStats()->PausePerfStat("AMPS_ApplySKF");

#endif
	gGetAPhysXScene()->GetPerfStats()->ResumePerfStat("AMPS_BuildTorque");
	BuildTorqueActors();
	gGetAPhysXScene()->GetPerfStats()->PausePerfStat("AMPS_BuildTorque");

	return true;
}

//	Build attacher link infos
bool A3DModelPhysSync::BuildAttacherLinkInfos(const char* szParentHook, const char* szChildAttacher)
{
	ASSERT(szParentHook && szChildAttacher);

	if (!m_pPhysAttacher || !m_pPhysModel || !m_pCoreData || !m_pSkinModel)
		return false;

 	A3DSkinModel* pParentModel = m_pSkinModel->GetParent();
 	if (!pParentModel)
 		return false;

 	A3DSkeleton* pParentSkeleton = pParentModel->GetSkeleton();
 	if (!pParentSkeleton)
 		return false;

	//	Can not find parent hook
	int iHookIdx = -1;
	if (!pParentSkeleton->GetHook(szParentHook, &iHookIdx))
		return false;

	//	Can not find attacher
	int iAttacherIdx = -1;
	if (!m_pCoreData->GetAttacherByName(szChildAttacher, &iAttacherIdx))
		return false;

	ATTACHER_LINK* pAttacherLink = new ATTACHER_LINK;
	pAttacherLink->strParentHook = szParentHook;
	pAttacherLink->strAttacher = szChildAttacher;
	pAttacherLink->nParentHookIdx = iHookIdx;
	pAttacherLink->nAttacherIdx = iAttacherIdx;
	m_aAttacherLinks.Add(pAttacherLink);
	return true;
}

//	Update model's animation. Call this before physics simulation
bool A3DModelPhysSync::UpdateModelAnim(int iDeltaTime)
{
	if (!m_pSkinModel)
		return false;

#ifdef _ANGELICA21
	A3DSkeleton* pSkeleton = m_pSkinModel->GetSkeleton();
	if (pSkeleton)
	{
		bool bUseFootOff = !(m_iPhysState == PHY_STATE_SIMULATE && m_iRBSimMode == RB_SIMMODE_RAGDOLL);

		if (bUseFootOff != pSkeleton->GetUseFootOffset())
		{
		pSkeleton->SetUseFootOffset(true);    // always using foot offset while update animation

#if 0
			a_LogOutput(1, "@%d: A3DModelPhysSync::UpdateModelAnim: SKELETON 0x%X, m_iPhysState %d, m_iRBSimMode %d, SetUseFootOffset(%d)!",
				a_GetTime(), pSkeleton, m_iPhysState, m_iRBSimMode, bUseFootOff ? 1 : 0);
#endif
		}
	}
#endif

	if (!m_bPhysBindOk || m_pCoreData->GetModifiedFlag())
	{
		m_pSkinModel->Update(iDeltaTime);
		return true;
	}

#ifdef _ANGELICA21

	m_pSkinModel->Update(iDeltaTime, !m_bEnableAnim);

#else

	if (m_iPhysState == PHY_STATE_ANIMATION)
		m_pSkinModel->Update(iDeltaTime);
	else
		m_pSkinModel->UpdateSkeletonAndAnim(iDeltaTime, false);

#endif
	
	//	Update actor pose by animation
	UpdateActorsByAnim(false);

	//	Update attachers by parent & link info
	UpdateAttachersByParentAndLinkInfo();

	return true;
}

//	Update actor state by animation
void A3DModelPhysSync::UpdateActorsByAnim(bool bTeleport)
{
	if (!m_bPhysBindOk || m_pCoreData->GetModifiedFlag() || !m_pSkinModel)
		return;

	if (!IsSyncAnimPoseToPhysXEnabled())
		return;

	{
		A3DSkeleton* pSkeleton = m_pSkinModel->GetSkeleton();
		if (!pSkeleton)
			return;

		A3DMATRIX4 mat;
		NxMat34 matNx;

		//---------------------------------------------------
		//	Sync actors pose from bones
		int nbActorImps = m_aActorImps.GetSize();
		for (int i=0; i < nbActorImps; i++)
		{
			ACTOR_IMP* pActorImp = m_aActorImps[i];
			if (!pActorImp->pActor->readBodyFlag(NX_BF_KINEMATIC))
				continue;

			if (pActorImp->bIsPinned)
				continue;

			A3DModelPhysSyncData::ACTOR_DATA* pActorData = pActorImp->pActorData;
			A3DModelPhysSyncData::LINK_INFO& link = pActorData->Link;

			//	Get major bone
			A3DBone* pBone = pSkeleton->GetBone(link.iMainBone);
			mat = link.matActor2Bone * pBone->GetNoScaleAbsTM();


#if 0		// Note: it's very complicated to handle the case that main bone is flipped, 
			// so currently, we just disable this function....

			if(pBone->IsFlipped())
			// if(DotProduct(CrossProduct(mat.GetRow(0), mat.GetRow(1)), mat.GetRow(2)) < 0.0f)
			{
				mat._31 = - mat._31;
				mat._32 = - mat._32;
				mat._33 = - mat._33;
			}

#endif

			APhysXConverter::A2N_Matrix44(mat, matNx);

			//---------------------------------------------------
			// revised by Wenfeng, Dec. 3, 2012
			// we must use APhysXScene's function SetActorPose() to
			// support sub-simulation case instead of setGlobalPose()
			// directly to NxActor...

#if 0
			//---------------------------------------------------
			// try to acquire the writelock of the scene before 
			// make changes on actors...

			m_pPhysScene->LockWritingScene();
			
			if (bTeleport)
				pActorImp->pActor->setGlobalPose(matNx);
			else
			{
				assert(matNx.isFinite());
				pActorImp->pActor->moveGlobalPose(matNx);
			}
			m_pPhysScene->UnlockWritingScene();
#else

			m_pPhysScene->SetActorPose(pActorImp->pActor, matNx);

#endif
		}

		//---------------------------------------------------
		// apply joint slerp drive...
		if(m_iRBSimMode == RB_SIMMODE_SOFT_KEYFRAME)
		{
			if (m_bIsUsingLinearDrives || m_bIsUsingAngularDrives)
			{
				NxMat33 mtTargetPose;
				NxVec3 vTargetDrivePos;
				for (int i=0; i < m_aJointImps.GetSize(); i++)
				{
					JOINT_IMP* pJointImp = m_aJointImps[i];
					if(!pJointImp->pD6Joint) continue;

					A3DModelPhysSyncData::JOINT_DATA* pJointData = m_pCoreData->GetJointByIndex(i);

					// compute the actor0's pose...
					A3DModelPhysSyncData::ACTOR_DATA* pActorData0 = pJointData->GetActor(0);
					A3DModelPhysSyncData::LINK_INFO& link0 = pActorData0->Link;
					A3DBone* pBone0 = pSkeleton->GetBone(link0.iMainBone);
					NxMat34 mtActor0;
					APhysXConverter::A2N_Matrix44(link0.matActor2Bone * pBone0->GetNoScaleAbsTM(), mtActor0);

					// compute the actor1's pose...
					A3DModelPhysSyncData::ACTOR_DATA* pActorData1 = pJointData->GetActor(1);
					A3DModelPhysSyncData::LINK_INFO& link1 = pActorData1->Link;
					A3DBone* pBone1 = pSkeleton->GetBone(link1.iMainBone);
					NxMat34 mtActor1;
					APhysXConverter::A2N_Matrix44(link1.matActor2Bone * pBone1->GetNoScaleAbsTM(), mtActor1);

					NxMat33 mtJoint0 = mtActor0.M * pJointImp->mLocalRot0;
					NxMat33 mtJoint1 = mtActor1.M * pJointImp->mLocalRot1;

					if (m_bIsUsingAngularDrives)
						mtTargetPose.multiplyTransposeLeft(mtJoint0, mtJoint1);

					if (m_bIsUsingLinearDrives)
					{
						//-------------------------------------------------------------------------
						// Noted by Wenfeng, Mar. 8, 2013
						// 
						// turn on the X, Y and Z drive so as to support bones' position 
						// offset in their parent frame.
						// 
						// Beware: to make this work well, you have to adjust the jointed
						// actors' solver iteration to a high value, such as 100.
						// 
						// compute anchor target position

						NxD6JointDesc d6Desc;
						NxVec3 vAnchor0GPos = mtActor0 * pJointImp->mLocalAnchor0;
						NxVec3 vAnchor1GPos = mtActor1 * pJointImp->mLocalAnchor1;

						mtJoint0.multiplyByTranspose(vAnchor1GPos - vAnchor0GPos, vTargetDrivePos);

						//-------------------------------------------------------------------------
					}

					m_pPhysScene->LockWritingScene();
					if (m_bIsUsingAngularDrives)
						pJointImp->pD6Joint->setDriveOrientation(NxQuat(mtTargetPose));
					if (m_bIsUsingLinearDrives)
						pJointImp->pD6Joint->setDrivePosition(vTargetDrivePos);
					m_pPhysScene->UnlockWritingScene();
				}
			}
		}
	}
}

//	Update attachers state by parent's hooks position
void A3DModelPhysSync::UpdateAttachersByParentAndLinkInfo()
{
	if (!m_bPhysBindOk || m_pCoreData->GetModifiedFlag() || !m_pSkinModel)
		return;
		
	//	Phys attacher object is generated (bind succeed)
	if (!m_pPhysAttacher)
		return;

	//	Only if this model is a child model, we need to update its attacher by parent's hook
	A3DSkinModel* pParent = m_pSkinModel->GetParent();
	if (!pParent)
		return;

	A3DSkeleton* pParentSkeleton = pParent->GetSkeleton();
	if (!pParentSkeleton)
		return;
	
	int nIdx, nNum = m_aAttacherLinks.GetSize();
	for (nIdx = 0; nIdx < nNum; ++nIdx)
	{
		if (m_aAttacherLinks[nIdx]->nParentHookIdx == -1)
			pParentSkeleton->GetHook(m_aAttacherLinks[nIdx]->strParentHook, &m_aAttacherLinks[nIdx]->nParentHookIdx);

		if (m_aAttacherLinks[nIdx]->nAttacherIdx == -1)
			m_pCoreData->GetAttacherByName(m_aAttacherLinks[nIdx]->strAttacher, &m_aAttacherLinks[nIdx]->nAttacherIdx);

		//	Can not look for the specified hook & attacher, skip this
		if (m_aAttacherLinks[nIdx]->nParentHookIdx == -1
			|| m_aAttacherLinks[nIdx]->nAttacherIdx == -1) {
			ASSERT(FALSE);
			continue;
		}
		
		A3DSkeletonHook* pHook = pParentSkeleton->GetHook(m_aAttacherLinks[nIdx]->nParentHookIdx);
		if (!pHook)
			continue;
		
		NxMat34 nmPos;
		APhysXConverter::A2N_Matrix44(pHook->GetNoScaleAbsTM(), nmPos);
//		assert(nmPos.M.isRotation());
		assert(nmPos.isFinite());
		//if(stricmp(m_aAttacherLinks[nIdx]->strAttacher, "chest") == 0)
		//{
		//	int i = 0;	
		//}

		m_pPhysAttacher->SetAttacherPose(m_aAttacherLinks[nIdx]->strAttacher, nmPos);
		/*
#ifdef _DEBUG
		AString ss;
		NxVec3 r1, r2, r3;
		r1 = nmPos.M.getRow(0);
		r2 = nmPos.M.getRow(1);
		r3 = nmPos.M.getRow(2);
		ss.Format("nmPos: %f, %f, %f, %f, %f, %f, %f, %f, %f     %f, %f, %f\n", r1.x, r1.y, r1.z, r2.x, r2.y, r2.z, r3.x, r3.y, r3.z, nmPos.t.x, nmPos.t.y, nmPos.t.z);
		OutputDebugStringA(ss);
#endif
		*/
	}
}

//	Sync model state from physical data. Call this after physics simulation
bool A3DModelPhysSync::SyncModelPhys()
{
#ifdef _ANGELICA21
	if (	(m_iPhysState == PHY_STATE_ANIMATION && !m_bSetAnimWeight) 
		||	!m_bPhysBindOk 
		||	m_pCoreData->GetModifiedFlag()
		)
	 	return true;
#else
	if (m_iPhysState == PHY_STATE_ANIMATION || !m_bPhysBindOk || m_pCoreData->GetModifiedFlag())
		return true;
#endif

	A3DSkeleton* pSkeleton = m_pSkinModel ? m_pSkinModel->GetSkeleton() : NULL;

#ifdef _ANGELICA21
	//保存动画数据
	if (m_bSetAnimWeight && m_pSkBlend && pSkeleton && m_pAnimWeight)
	{
		int iNumBone = pSkeleton->GetBoneNum();
		A3DSkeletonBlend::Blend_Node BNode;
		BNode.bRelative = true;
		for (int i = 0; i < iNumBone; i++)
		{
			if (m_pAnimWeight[i] > 0)
			{
				BNode.fWeight = m_pAnimWeight[i];
				BNode.pBone = pSkeleton->GetBone(i);
				BNode.mat = BNode.pBone->GetUpToParentTM();

				m_pSkBlend->AddBlendNode(BNode);
			}
		}
	}
#endif

	bool bWrongSimulate = false;
	if (m_iPhysState == PHY_STATE_SIMULATE)
	{
		//	Set model's position base on simulation result
		int iRootBoneActor = m_pCoreData->m_iRootActor;
		if (iRootBoneActor < 0 || !m_pCoreData->m_pRootBoneData)
		{
			ASSERT(iRootBoneActor >= 0 && m_pCoreData->m_pRootBoneData);
			return false;
		}

		NxActor* pActor = m_aActorImps[iRootBoneActor]->pActor;
		if (!pActor)
			return false;

		//	Root bone's position will be used as model's position. At the same time
		//	we should reset model's direction to binding-time (when physical actors
		//	were bound with model) value.
		A3DMATRIX4 matActor;
		APhysXConverter::N2A_Matrix44(pActor->getGlobalPose(), matActor);
		A3DMATRIX4 mat = m_pCoreData->m_pRootBoneData->matBone2Actor * matActor;
		if(!pActor->getGlobalPose().isFinite())
		{
			ChangePhysState(PHY_STATE_ANIMATION);
			bWrongSimulate = true;
		}
		else if (pSkeleton)
		{
			//-----------------------------------------------------
			// revised by wenfeng, Oct. 12, 2011
			// compute the real position/orientation of m_pSkinModel
			A3DBone* pRootBone = pSkeleton->GetBone(m_pCoreData->m_pRootBoneData->iBoneIdx);
			A3DMATRIX4 matRootBoneOrigin = pRootBone->GetOriginalMatrix(), matSkinModel2RootBoneOrigin;
			a3d_InverseTM(matRootBoneOrigin, &matSkinModel2RootBoneOrigin);
			mat = matSkinModel2RootBoneOrigin * mat;

			//-----------------------------------------------------
			// revised by wenfeng, Dec. 24, 2012
			// as for the breakable objects, we just ignore the changing of skin model's pose

			// we change skin model's pose only in ragdoll mode
			if(m_iRBSimMode == RB_SIMMODE_RAGDOLL && m_pPhysModel && !m_pPhysModel->IsBreakable())
			{
				m_pSkinModel->SetPos(mat.GetRow(3));
				m_pSkinModel->SetDirAndUp(mat.GetRow(2), mat.GetRow(1));
			}
		}
	}
	
	if(!bWrongSimulate)
	{
#ifdef _ANGELICA21
		if(m_iPhysState == PHY_STATE_SIMULATE)
		{
			bool bUseFootOff = !(m_iPhysState == PHY_STATE_SIMULATE && m_iRBSimMode == RB_SIMMODE_RAGDOLL);
			if (pSkeleton && pSkeleton->GetUseFootOffset() != bUseFootOff)
			{
#if 0
				a_LogOutput(1, "@%d: A3DModelPhysSync::SyncModelPhys: SKELETON 0x%X, m_iPhysState %d, m_iRBSimMode %d, SetUseFootOffset %d!",
					a_GetTime(), pSkeleton, m_iPhysState, m_iRBSimMode, bUseFootOff);
#endif

				pSkeleton->SetUseFootOffset(bUseFootOff);
			}
		}
#endif

		//	Update physically driven bones' state
		UpdateBonesByPhys();

		//	Sync physical state
#ifdef _ANGELICA21

		A3DMATRIX4 matNewPose(A3DMATRIX4::IDENTITY);
		bool HasNewPose = ComputePoseAsChildModel(matNewPose);

		//blend 
		if (pSkeleton)
		{
			if (m_bSetAnimWeight && m_pSkBlend && m_pAnimWeight)
			{
				int iNumBone = pSkeleton->GetBoneNum();
				int i;

				A3DSkeletonBlend::Blend_Node BNode;
				BNode.bRelative = false;
				for (i = 0; i < iNumBone; i++)
				{
					if (i == 0 && m_iPhysState != PHY_STATE_SIMULATE)///*&& m_pAnimWeight[0] != 0*/)
						continue;

					if (m_pAnimWeight[i] < 1)
					{
						BNode.fWeight = 1 - m_pAnimWeight[i];
						BNode.pBone = pSkeleton->GetBone(i);
						BNode.mat = m_aBoneMats[i].matBoneAbs;
						int iParent = BNode.pBone->GetParent();
						if (iParent == -1)
							BNode.matAbsParentInv = m_pSkinModel->GetAbsoluteTM().GetInverseTM();
						else if (iParent == 0 && m_iPhysState != PHY_STATE_SIMULATE)//&& )
							BNode.matAbsParentInv = pSkeleton->GetBone(0)->GetNoScaleAbsTM().GetInverseTM();
						else
							BNode.matAbsParentInv = m_aBoneMats[iParent].matBoneInvAbs;

						m_pSkBlend->AddBlendNode(BNode);
					}
				}

				m_pSkBlend->Update();

			}
			else
			{
				// note: do not call skinmodel's update() here, since it will overwrite the bone's matrix from animation.
				pSkeleton->RefreshAllAfterIK();
			}

			// make sure that set new pose after blending
			if (HasNewPose)
			{
				m_pSkinModel->SetAbsoluteTM(matNewPose);
				pSkeleton->RefreshAllAfterIK();
			}
		}

		// force updating all my hangers/hooks...
		m_pSkinModel->UpdateHanger(0);
#else

		m_pSkinModel->Update(0, true);
#endif
	}

	return true;
}

// test whether pTestBone is the ancestor of pRefBone
static bool TestAncestorBone(A3DBone* pRefBone, A3DBone* pTestBone)
{
	if(pRefBone)
	{
		A3DBone* pParent = pRefBone->GetParentPtr();
		while(pParent)
		{
			if(pTestBone == pParent) return true;
			pParent = pParent->GetParentPtr();
		}
	}
	
	return false;
}


//	Update physically driven bones' state
void A3DModelPhysSync::UpdateBonesByPhys()
{
	ASSERT(m_pSkinModel && m_pPhysModel);

	A3DSkeleton* pSkeleton = m_pSkinModel->GetSkeleton();
	if (!pSkeleton)
		return;

	int i, j;
	A3DMATRIX4 mat, matActor;

	//	Update all bones absolute matrices and their inverse matrices
	for (i=0; i < m_aActorImps.GetSize(); i++)
	{
		ACTOR_IMP* pActorImp = m_aActorImps[i];
		A3DModelPhysSyncData::ACTOR_DATA* pActorData = pActorImp->pActorData;
		A3DModelPhysSyncData::LINK_INFO& link = pActorData->Link;
		A3DBone* pMainBone = pSkeleton->GetBone(link.iMainBone);

		//	Get actor's absolute matrix
		APhysXConverter::N2A_Matrix44(pActorImp->pActor->getGlobalPose(), matActor);

		for (j=0; j < link.aBones.GetSize(); j++)
		{
			A3DModelPhysSyncData::BONE_DATA* pBoneData = link.aBones[j];
			int iBoneIdx = pBoneData->iBoneIdx;
			A3DBone* pBone = pSkeleton->GetBone(iBoneIdx);
			if (!pBone)
			{
				ASSERT(pBone);
				return;
			}

			//-----------------------------------------------------------
			// noted by Wenfeng, May 22, 2013
			// some bones may be not the direct ancestor of the main bone, such as the sibling bone of the main bone
			// so here we have to revise the conditions
			m_aBoneMats[iBoneIdx].bMainBone = (iBoneIdx == link.iMainBone) || !TestAncestorBone(pBone, pMainBone);

			// noted by Wenfeng, Oct.17, 2012
			// The bones which are the ancestors of main bone are also set this flag
			// so that their absolute TM is computed from physx Actor as well as main bone.
			// m_aBoneMats[iBoneIdx].bMainBone = (iBoneIdx == link.iMainBone) || TestAncestorBone(pMainBone, pBone);

			//	Get bone's absolute matrix and it's inverse matrix
#ifdef _ANGELICA21

			//-----------------------------------------------------------
			// noted by wenfeng, Oct.15, 2012
			// beware of the following conditions only the root bone which is not the main bone will
			// apply the animation pose. And if the root bone is some actor's main bone, we have to use 
			// the actor's simulated pose to update the bone's pose.

			if (m_pAnimWeight && m_pAnimWeight[iBoneIdx] == 1 || 
				iBoneIdx != link.iMainBone && pBone->GetParent() < 0) 
			{
				// full animation mode...
				m_aBoneMats[iBoneIdx].matBoneAbs = pBone->GetNoScaleAbsTM();
				a3d_InverseTM(m_aBoneMats[iBoneIdx].matBoneAbs, &m_aBoneMats[iBoneIdx].matBoneInvAbs);
			}
			else if (m_aBoneMats[iBoneIdx].bMainBone)
			{
				// simulation mode...
				
				// if pBone is a right-hand frame bone, flip Z axis...
				mat = pBoneData->matBone2Actor * matActor;

#if 0		// Note: it's very complicated to handle the case that main bone is flipped, 
			// so currently, we just disable this function....

				if(pBone->IsFlipped())
				{
					mat._31 = -mat._31;
					mat._32 = -mat._32;
					mat._33 = -mat._33;
				}
#endif

				m_aBoneMats[iBoneIdx].matBoneAbs = mat;
				a3d_InverseTM(mat, &m_aBoneMats[iBoneIdx].matBoneInvAbs);
			}
/*
			else if ( pBone->GetParent() < 0)
			{
				// root bone...
				mat = pBone->GetOriginalMatrix();

				//	Conside scale factor on bone's relative offset
				float fLocalLenSF = pBone->GetAccuWholeScale() ;

				if (fLocalLenSF != 1.0f)
				{
					mat._41 *= fLocalLenSF;
					mat._42 *= fLocalLenSF;
					mat._43 *= fLocalLenSF;
				}

				m_aBoneMats[iBoneIdx].matBoneAbs = mat;
				a3d_InverseTM(mat, &m_aBoneMats[iBoneIdx].matBoneInvAbs);

			}

*/

#else
			if (pBone->IsAnimDriven())			
			{
				m_aBoneMats[iBoneIdx].matBoneAbs = pBone->GetNoScaleAbsTM();
				a3d_InverseTM(m_aBoneMats[iBoneIdx].matBoneAbs, &m_aBoneMats[iBoneIdx].matBoneInvAbs);
			}
			else if (pBone->GetParent() < 0 || iBoneIdx == link.iMainBone)
			{
				mat = pBoneData->matBone2Actor * matActor;
				m_aBoneMats[iBoneIdx].matBoneAbs = mat;
				a3d_InverseTM(mat, &m_aBoneMats[iBoneIdx].matBoneInvAbs);
			}
#endif
		}
	}

	//	Get model's inverse absolute matrix
	A3DVECTOR3 vModelPos = m_pSkinModel->GetPos();
	A3DMATRIX4 matInvModel;
	//a3d_InverseTM(a3d_Translate(vModelPos.x, vModelPos.y, vModelPos.z), &matInvModel);
	a3d_InverseTM(m_pSkinModel->GetAbsoluteTM(), &matInvModel);

	//	Calculate all bones relative matrices
	int iNumRoot = pSkeleton->GetRootBoneNum();
	for (i=0; i < iNumRoot; i++)
	{
		int iRootBone = pSkeleton->GetRootBone(i);
		UpdateBonesByPhys_r(matInvModel, iRootBone);
	}
}

void A3DModelPhysSync::UpdateBonesByPhys_r(const A3DMATRIX4& matInvModel, int iBoneIdx)
{
	A3DSkeleton* pSkeleton = m_pSkinModel->GetSkeleton();
	if (!pSkeleton)
		return;
	
	A3DBone* pBone = pSkeleton->GetBone(iBoneIdx);
	if (!pBone)
	{
		assert(0);
		return;
	}

#ifdef _ANGELICA21
	if (m_pAnimWeight && m_pAnimWeight[iBoneIdx] < 1)
#else
	if (!pBone->IsAnimDriven())
#endif
	
	{
		A3DMATRIX4 mat;

		int iParentIdx = pBone->GetParent();
		if (iParentIdx >= 0)
		{
			if (m_aBoneMats[iBoneIdx].bMainBone)
				mat = m_aBoneMats[iBoneIdx].matBoneAbs * m_aBoneMats[iParentIdx].matBoneInvAbs;
			else
			{	
				// use the animation's poses instead of original poses so as to get better visual effect.
#if 0				
				{
					mat = pBone->GetOriginalMatrix();

					//	Conside scale factor on bone's relative offset
					A3DBone* pParent = pBone->GetParentPtr();
					float fLocalLenSF = pBone->GetLocalLengthScale() * pParent->GetAccuWholeScale();

					if (fLocalLenSF != 1.0f)
					{
						mat._41 *= fLocalLenSF;
						mat._42 *= fLocalLenSF;
						mat._43 *= fLocalLenSF;
					}
				}
#else				
				{
					mat = pBone->GetUpToParentTM();
					if(pBone->IsFlipped() && DotProduct(CrossProduct(mat.GetRow(0), mat.GetRow(1)), mat.GetRow(2)) < 0.0f)
					{
						mat._11 = -mat._11;
						mat._12 = -mat._12;
						mat._13 = -mat._13;
					}
				}
#endif
				A3DMATRIX4 matBoneAbs = mat * m_aBoneMats[iParentIdx].matBoneAbs;
				m_aBoneMats[iBoneIdx].matBoneAbs = matBoneAbs;
				a3d_InverseTM(matBoneAbs, &m_aBoneMats[iBoneIdx].matBoneInvAbs);
			}
		}
		else
			mat = m_aBoneMats[iBoneIdx].matBoneAbs * matInvModel;

		//	Use SetRelativeTM() directly can accelerate bone's update
	//	A3DQUATERNION quat(mat);
	//	pBone->AddBlendState(quat, mat.GetRow(3), 1.0f, A3DBone::BM_NORMAL);
		
#ifdef _ANGELICA21
		//pBone->SetUpToParentTM(mat);
#else
		pBone->SetRelativeTM(mat);
#endif

	}

	//	Handle children
	int i, iNumChild = pBone->GetChildNum();
	for (i=0; i < iNumChild; i++)
		UpdateBonesByPhys_r(matInvModel, pBone->GetChild(i));
}


void A3DModelPhysSync::SetBoneAnimDriven(bool bAnimDriven)
{
	if (!m_bPhysBindOk || m_pCoreData->GetModifiedFlag())
		return;

	A3DSkeleton* pSkeleton = m_pSkinModel->GetSkeleton();
	if (!pSkeleton)
		return;

	A3DBone* pBone = 0;
	int i, iNumBone = pSkeleton->GetBoneNum();

#ifdef _ANGELICA21
	for (i = 0; i < iNumBone; ++i)
	{
		pBone = pSkeleton->GetBone(i);
		if (pBone)
			pBone->SetAnimDriven(bAnimDriven);	

		if (!bAnimDriven && m_pAnimWeight)
			m_pAnimWeight[i] = 0.0f;
	}
	if (!bAnimDriven && m_pAnimWeight)
		m_bSetAnimWeight = true;

#else
	for (i = 0; i < iNumBone; ++i)
	{
		pBone = pSkeleton->GetBone(i);
		if (pBone)
			pBone->SetAnimDriven(bAnimDriven);	
	}
#endif
}

//	每次Break
void A3DModelPhysSync::OnBreak(const PhysBreakInfo& physBreakInfo)
{
	// 在A3DModelPhysSync的生命期中， m_pModelPhysics总应当是有效的
	ASSERT(m_pModelPhysics);
	
	// 有些场合可能不需要ECModel的指针 (A3DModelPhysics在初始化Init时，使用的不是CECModel*而是A3DSkinModel*)
	// 这种情况下也就不需要调用ECModel的OnPhysBreak
	//ASSERT(m_pModelPhysics->GetECModel());

	if (!m_pModelPhysics || !m_pModelPhysics->GetECModel())
		return;

	// 破碎时的逻辑变更，交由ECModel处理，包括调用脚本(更换皮肤，播放GFX，播放SFX)，以及回调外界传入的函数指针等
	m_pModelPhysics->GetECModel()->OnPhysBreak(physBreakInfo);

}

//	First Break
void A3DModelPhysSync::OnFirstBreak()
{

	// callback when the the object firstly breaks from sanity status

	// 在A3DModelPhysSync的生命期中， m_pModelPhysics总应当是有效的
	ASSERT(m_pModelPhysics);
	
	if (!m_pModelPhysics || !m_pModelPhysics->GetECModel())
		return;
	
	// 初次破碎时的逻辑变更，交由ECModel处理，包括调用脚本(更换皮肤，播放GFX，播放SFX)，以及回调外界传入的函数指针等
	// m_pModelPhysics->GetECModel()->OnPhysBreak();

	// set all bone's driven mode to physics
	// SetBoneAnimDriven(false);
	
	// m_iPhysState = PHY_STATE_SIMULATE;
	ChangePhysState(PHY_STATE_SIMULATE);

	m_bPhysBroken = true;
}

//	All Break
void A3DModelPhysSync::OnAllBreak()
{
	// callback when all breaking part have been broken...
	
	// to do...

}


//	Change physical state
bool A3DModelPhysSync::ChangePhysState(int iState, int iRBSimMode /*= -1*/)
{
	if (!m_bPhysBindOk || m_pCoreData->GetModifiedFlag())
		return false;

	A3DSkeleton* pSkeleton = m_pSkinModel->GetSkeleton();
	// pSkeleton->PrepareTransition(500, false);

	bool bStateChanged = m_iPhysState != iState ;
	m_iPhysState = iState;

	if (iState == PHY_STATE_SIMULATE)
	{
		if(iRBSimMode == -1) 
			iRBSimMode = RB_SIMMODE_RAGDOLL;

		if(iRBSimMode == RB_SIMMODE_RAGDOLL)
			 m_pSkinModel->StopAllActions();

		int iNumBone = 0;
		if (pSkeleton)
		{
			int i;
			iNumBone = pSkeleton->GetBoneNum();
			for (i=0; i < iNumBone; i++)
			{
				A3DBone* pBone = pSkeleton->GetBone(i);
				
				if(iRBSimMode==RB_SIMMODE_RAGDOLL)
					pBone->SetAnimDriven(false);
				else
					pBone->SetAnimDriven(true);
			}
		}

		if (m_pPhysModel)
		{
			if (iRBSimMode == RB_SIMMODE_HARD_KEYFRAME)
				m_pPhysModel->SetDrivenMode(APX_RBDRIVENMODE_ANIMATION);
			else
				m_pPhysModel->SetDrivenMode(APX_RBDRIVENMODE_PHYSICS);
		}

		SetRBSimMode(iRBSimMode, bStateChanged);


		//	when this model is not a child model
		//	when we make the model in part-physical state / physical state
		//	we need to let the Attacher object be in the physical state too (simulate with other actors)
		//	while when the model is a child model
		//	we skip this and set the attachers' position by the parent's hooks' position
		if (!m_pSkinModel->GetParent())
			m_pPhysAttacher->EnableAttacherKinematic(false);

#ifdef _ANGELICA21
		if (m_pAnimWeight)
		{
			memset(m_pAnimWeight, 0, sizeof(float) * iNumBone);
			m_bSetAnimWeight = true;
		}

		//--------------------------------------------------
		// enable update hook only by force to apply simulation results 
		// and not modified by animation
		m_pSkinModel->EnableHookUpdateOnlyByForce();
		//--------------------------------------------------

#endif
		
	}
	else if (iState == PHY_STATE_ANIMATION)
	{
		if(iRBSimMode == -1) 
			iRBSimMode = RB_SIMMODE_HARD_KEYFRAME;

		int i, iNumBone = 0;
		if (pSkeleton)
		{
			iNumBone = pSkeleton->GetBoneNum();
			for (i=0; i < iNumBone; i++)
			{
				A3DBone* pBone = pSkeleton->GetBone(i);

				pBone->SetAnimDriven(true);
			}
		}

		if (m_pPhysModel)
		{
			if (iRBSimMode == RB_SIMMODE_HARD_KEYFRAME)
				m_pPhysModel->SetDrivenMode(APX_RBDRIVENMODE_ANIMATION);
			else
				m_pPhysModel->SetDrivenMode(APX_RBDRIVENMODE_PHYSICS);
		}

		SetRBSimMode(iRBSimMode, bStateChanged);

		if (m_bPhysBroken && m_pPhysModel)
		{
			// Once the Physical skeleton object is broken
			// and we want it to be set to kinematic
			// we need to re-bind it (in order to re-create the skeleton object)
			// Bind will change the m_bPhysBroken to false.
			//	Bind(m_pSkinModel, m_pCoreData, m_pPhysScene);

			// APhysXBreakableSkeletonRBObject* pBreakableSkeleton = (APhysXBreakableSkeletonRBObject*)m_pPhysModel;
			// pBreakableSkeleton->Reset();

			// try to use the latest function to retrieve the Breakable interface
			APhysXBreakable* pBreakable = m_pPhysModel->IsBreakable();
			if(pBreakable)
				pBreakable->Reset();

		}

#ifdef _ANGELICA21
		if (m_pAnimWeight)
		{
			for (i = 0; i < iNumBone; i++)
				m_pAnimWeight[i] = 1;

			m_bSetAnimWeight = false;
		}

		//--------------------------------------------------
		// enable update hook only by force to apply simulation results 
		// and not modified by animation
		m_pSkinModel->EnableHookUpdateOnlyByForce(false);
		//--------------------------------------------------

#endif
	}
	else if (iState == PHY_STATE_PARTSIMULATE)
	{

		if(iRBSimMode == -1) 
			iRBSimMode = RB_SIMMODE_RAGDOLL;

//		if(iRBSimMode == RB_SIMMODE_HARD_KEYFRAME)
//		{
//			m_pPhysModel->SetDrivenMode(APX_RBDRIVENMODE_ANIMATION);
//		}
//		else
		if (pSkeleton && m_pPhysModel)
		{
			APhysXArray<NxActor* > arrDynamicActors;
			int i, j;
			int nbActorImps = m_aActorImps.GetSize();
			for (i = 0; i < nbActorImps; ++i)
			{
				ACTOR_IMP* pActorImp = m_aActorImps[i];
				A3DModelPhysSyncData::LINK_INFO& link = pActorImp->pActorData->Link;

				int nbBones = link.aBones.GetSize();
				for (j = 0; j < nbBones; ++j)
				{
					A3DModelPhysSyncData::BONE_DATA* pBoneData = link.aBones[j];
					A3DBone* pBone = pSkeleton->GetBone(pBoneData->iBoneIdx);
					if (!pBone)
						continue;

	#ifdef _ANGELICA21
					pBone->SetAnimDriven(true);
					if (pBoneData->bAnimMajor)
					{
						if (m_pAnimWeight)
						{
							m_bSetAnimWeight = true;
							m_pAnimWeight[pBoneData->iBoneIdx] = 1.0f;
						}
					}
					else
					{
						if (m_pAnimWeight)
						{
							m_bSetAnimWeight = true;
							m_pAnimWeight[pBoneData->iBoneIdx] = 0;
						}
						arrDynamicActors.push_back(pActorImp->pActor);
					}
	#else
					if (pBoneData->bAnimMajor)
					{
						pBone->SetAnimDriven(true);
					}
					else
					{
						pBone->SetAnimDriven(false);
						arrDynamicActors.push_back(pActorImp->pActor);
					}
	#endif
				}

			}

			m_pPhysModel->SetDrivenMode(APX_RBDRIVENMODE_MIX, &arrDynamicActors);

		}

		SetRBSimMode(iRBSimMode, true);

		//	when this model is not a child model
		//	when we make the model in part-physical state / physical state
		//	we need to let the Attacher object be in the physical state too (simulate with other actors)
		//	while when the model is a child model
		//	we skip this and set the attachers' position by the parent's hooks' position
		if (!m_pSkinModel->GetParent())
			m_pPhysAttacher->EnableAttacherKinematic(false);

		

#ifdef _ANGELICA21
		
		//--------------------------------------------------
		// enable update hook only by force to apply simulation results 
		// and not modified by animation
		m_pSkinModel->EnableHookUpdateOnlyByForce();
		//--------------------------------------------------

#endif

	}
	else
	{
		ASSERT(0);
		return false;
	}
	m_pModelPhysics->UpdateWindEffectedRBs();

	return true;
}

//	Get RB object mass
float A3DModelPhysSync::GetModelMass(bool bOnlyBodyMass) const
{
	if (!m_pPhysModel)
		return 0.0f;

	float fAllMass = m_pPhysModel->GetMass();
	if (bOnlyBodyMass)
	{
		int nbJoints = m_aJointImps.GetSize();
		for (int i = 0; i < nbJoints; ++i)
		{
			JOINT_IMP* pImp = m_aJointImps[i];
			if (pImp->pJointData->ReadFlag(A3DModelPhysSyncData::JDF_BREAKABLE_EQUIPMENTS))
			{
				if (0 != pImp->pActor1)
					fAllMass -= pImp->pActor1->getMass();
			}
		}
	}
	return fAllMass;
}

//	After model is teleport to a new position, this function should be could to sync physical data
bool A3DModelPhysSync::Teleport()
{
	if (!m_bPhysBindOk || m_pCoreData->GetModifiedFlag())
		return true;

	ASSERT(m_pSkinModel);

	//	Change physical state so that we can reset all actors' pose
	int iPhysState = -1;
	if (m_iPhysState != PHY_STATE_ANIMATION)
	{
		iPhysState = m_iPhysState;
		if (!ChangePhysState(PHY_STATE_ANIMATION))
			return false;
	}

	//	Update actor pose
	UpdateActorsByAnim(true);

	//	Restore physical state
	if (iPhysState != -1)
	{
		ChangePhysState(iPhysState);

		//	After we change actor's pose through setGlobalPose() method, actor may enter a static state
		//	even if it's still in dynamic state. So we force to wake up all actors if they are driven
		//	by physical engine.
		if (iPhysState == PHY_STATE_SIMULATE || iPhysState == PHY_STATE_PARTSIMULATE)
		{
			for (int i=0; i < m_aActorImps.GetSize(); i++)
			{
				ACTOR_IMP* pActorImp = m_aActorImps[i];
				pActorImp->pActor->wakeUp();
			}
		}
	}

	return true;
}

//	Find actor through specified bone or hh-type hook
//	matHook (out): store hook or bone's absolute TM
NxActor* A3DModelPhysSync::GetActorByHook(int iHookIndex, bool bHHIsBone, A3DMATRIX4& matHook)
{
	if (!m_bPhysBindOk)
		return NULL;

	A3DSkeleton* pSkeleton = m_pSkinModel->GetSkeleton();
	if (!pSkeleton)
		return NULL;

	int iBoneIdx = -1;

	//	Get absolute TM
	if (bHHIsBone)
	{
		A3DBone* pBone = pSkeleton->GetBone(iHookIndex);
		if (!pBone)
			return NULL;

		iBoneIdx = iHookIndex;
		matHook = pBone->GetNoScaleAbsTM();
	}
	else
	{
		A3DSkeletonHook* pHook = pSkeleton->GetHook(iHookIndex);
		if (!pHook)
			return NULL;

		iBoneIdx = pHook->GetBone();
		matHook	= pHook->GetNoScaleAbsTM();
	}
	
	if (iBoneIdx < 0)
		iBoneIdx = m_pCoreData->m_iRootBone;

	//	Get the actor on which hook / bone is bound
	int i, j;
	int iNumActor = m_pCoreData->GetActorNum();

	for (i=0; i < iNumActor; i++)
	{
		A3DModelPhysSyncData::ACTOR_DATA* pActorData = m_pCoreData->GetActorByIndex(i);
		A3DModelPhysSyncData::LINK_INFO& link = pActorData->Link;

		for (j=0; j < link.aBones.GetSize(); j++)
		{
			A3DModelPhysSyncData::BONE_DATA* pBoneData = link.aBones[j];
			if (iBoneIdx == pBoneData->iBoneIdx)
				return m_aActorImps[i]->pActor;
		}
	}

	//	Couldn't find the actor that specified bone is bound with ! 
	//	This shouldn't happen when m_bPhysBindOk == true
	ASSERT(0);

	return NULL;
}

bool A3DModelPhysSync::OpenPhysLinkAsChildModel(const char* szParentHook, const char* szChildAttacher)
{
	if ((0 == szParentHook) || (0 == szChildAttacher))
	{
		ASSERT(szParentHook && szChildAttacher);
		return false;
	}

	//	Release old attacher link infos
	ReleaseAttacherLinkInfo();

	if (!BuildAttacherLinkInfos(szParentHook, szChildAttacher))
		return false;

	if (!m_pPhysAttacher)
		return false;

	m_pPhysAttacher->EnableAttacherKinematic(false);
	m_pPhysAttacher->EnableAttacherKinematic(szChildAttacher, true);
	return true;
}

void A3DModelPhysSync::ClosePhysLinkAsChildModel()
{
	ReleaseAttacherLinkInfo();
	if (m_pPhysAttacher)
		m_pPhysAttacher->EnableAttacherKinematic(false);
}

//	Check whether the physical link is already exist
bool A3DModelPhysSync::CheckPhysLinkExist(const char* szParentHook, const char* szChildAttacher)
{
	for (int iPhysLinkIdx = 0; iPhysLinkIdx < m_aAttacherLinks.GetSize(); ++iPhysLinkIdx)
	{
		ATTACHER_LINK* pLink = m_aAttacherLinks[iPhysLinkIdx];
		if (!pLink)
			continue;

		//	允许一个Hook对应多个Attacher，但是不能允许一个Attacher对应多个Hook
		if (pLink->strAttacher == szChildAttacher)
			return true;
	}

	return false;
}

//	Add physical link
bool A3DModelPhysSync::AddPhysLinkAsChildModel(const char* szParentHook, const char* szChildAttacher)
{
	if ((0 == szParentHook) || (0 == szChildAttacher))
	{
		ASSERT(szParentHook && szChildAttacher);
		return false;
	}

	if (CheckPhysLinkExist(szParentHook, szChildAttacher))
		return false;

	if (!BuildAttacherLinkInfos(szParentHook, szChildAttacher))
		return false;

	m_pPhysAttacher->EnableAttacherKinematic(szChildAttacher, true);
	return true;
}

//	Get attacher
A3DModelPhysSync::ATTACHER_LINK* A3DModelPhysSync::GetPhysLinkAsChildModel(const char* szChildAttacher, int* pIndex) const
{
	for (int iPhysLinkIdx = 0; iPhysLinkIdx < m_aAttacherLinks.GetSize(); ++iPhysLinkIdx)
	{
		ATTACHER_LINK* pAttacherLink = m_aAttacherLinks[iPhysLinkIdx];
		if (pAttacherLink->strAttacher == szChildAttacher)
		{
			if (pIndex)
				*pIndex = iPhysLinkIdx;

			return pAttacherLink;
		}
	}

	return NULL;
}

//	Remove physical link
bool A3DModelPhysSync::RemovePhysLinkAsChildModel(const char* szChildAttacher)
{
	ASSERT(szChildAttacher);
	
	int iIndex = -1;
	ATTACHER_LINK* pAttacherLink = GetPhysLinkAsChildModel(szChildAttacher, &iIndex);
	if (!pAttacherLink)
		return false;

	m_pPhysAttacher->EnableAttacherKinematic(szChildAttacher, false);
			
	delete pAttacherLink;
	m_aAttacherLinks.RemoveAt(iIndex);
	return false;
}

//	Change physical link
bool A3DModelPhysSync::ChangePhysLinkAsChildModel(const char* szChildAttacher, const char* szNewHook)
{
	int iIndex = -1;

	ATTACHER_LINK* pAttacherLink = GetPhysLinkAsChildModel(szChildAttacher, &iIndex);
	if (!pAttacherLink)
		return false;

	A3DSkinModel* pParentModel = m_pSkinModel->GetParent();
 	if (!pParentModel)
 		return false;

 	A3DSkeleton* pParentSkeleton = pParentModel->GetSkeleton();
 	if (!pParentSkeleton)
 		return false;

	//	Can not find parent hook
	int iHookIdx = -1;
	if (!pParentSkeleton->GetHook(szNewHook, &iHookIdx))
		return false;

	pAttacherLink->strParentHook = szNewHook;
	pAttacherLink->nParentHookIdx = iHookIdx;
	return true;
}

//	Break Joint
void A3DModelPhysSync::BreakJoint(const char* szJoint)
{
	if (!m_pPhysModel)
		return;

	m_pPhysModel->RemoveJoint(szJoint);

	if (0 != szJoint)
	{
		AString strName = szJoint;
		int nbJoints = m_aJointImps.GetSize();
		for (int i = 0; i < nbJoints; ++i)
		{
			JOINT_IMP* pJointImp = m_aJointImps[i];
			if (pJointImp->strJointName == strName)
			{
				pJointImp->pD6Joint = 0;
				m_bPhysBroken = true;
				return;
			}
		}
	}
}

//	Break Joint by bone name
void A3DModelPhysSync::BreakJointByBone(const char* szBoneName)
{
	if (!m_pPhysModel || !m_pCoreData || !szBoneName || !szBoneName[0])
		return;

	for (int iJointIdx = 0; iJointIdx < m_pCoreData->GetJointNum(); ++iJointIdx)
	{
		A3DModelPhysSyncData::JOINT_DATA* pJoint = m_pCoreData->GetJointByIndex(iJointIdx);
		if (strcmp(pJoint->GetLinkedBoneName(), szBoneName) == 0)
		{
			AString strName = pJoint->GetName();
			m_pPhysModel->RemoveJoint(strName);

			int nbJoints = m_aJointImps.GetSize();
			for (int i = 0; i < nbJoints; ++i)
			{
				JOINT_IMP* pJointImp = m_aJointImps[i];
				if (pJointImp->strJointName == strName)
				{
					pJointImp->pD6Joint = 0;
					m_bPhysBroken = true;
					break;
				}
			}
			return;
		}
	}
}

//	Apply force
NxActor* A3DModelPhysSync::AddForce(const A3DVECTOR3& vStart, const A3DVECTOR3& vDir, float fForceMagnitude, float fMaxDist, int iPhysForceType, float fSweepRadius /*= 0.0f*/, float fMaxVelChangeHint /*= FLT_MAX*/)
{
	if (!m_pPhysModel)
		return 0;

	float fRealForceMagnitude = fForceMagnitude > NX_MAX_F32 ? NX_MAX_F32 : fForceMagnitude;
	
	NxVec3 vDirNor = APhysXConverter::A2N_Vector3(vDir);
	vDirNor.normalize();
	NxRay forceRay(APhysXConverter::A2N_Vector3(vStart), vDirNor);
	NxForceMode nxForceMode = TranslateForceMode(iPhysForceType);

	if(fSweepRadius == 0.0f)
		return m_pPhysModel->AddForce(forceRay, fRealForceMagnitude, fMaxDist, nxForceMode, fMaxVelChangeHint);
	else
		return m_pPhysModel->AddSweepForce(forceRay, fRealForceMagnitude, fSweepRadius, fMaxDist, nxForceMode, fMaxVelChangeHint);
}

bool A3DModelPhysSync::AddTorque(int iRotateAxis, float fTorqueMagnitude, int iPhysTorqueType)
{
	bool bOnlyMajor = true;
	NxVec3 rotAxis(1, 0, 0);
	// hard code the rotate axis from model space to actor(main bone) space
	switch (iRotateAxis)
	{
	case CECModel::AXIS_X:
		rotAxis.set(0, 1, 0);
		break;
	case CECModel::AXIS_Y:
		rotAxis.set(1, 0, 0);
		bOnlyMajor = false;
		break;
	case CECModel::AXIS_Z:
		rotAxis.set(0, 0, 1);
		break;
	default:
		ASSERT(FALSE && "Invalid Rotate Axis.");
		break;
	}

	APtrArray<NxActor*> nxActors;
	int nbRotBones = m_aRotationBones.GetSize();
	for (int i = 0; i < nbRotBones; ++i)
	{
		if (0 != m_aRotationBones[i]->pActor)
		{
			if (m_aRotationBones[i]->bIsMajorBone || bOnlyMajor == m_aRotationBones[i]->bIsMajorBone)
				nxActors.UniquelyAdd(m_aRotationBones[i]->pActor);
		}
	}

	int nAlls = nxActors.GetSize();
	if (0 >= nAlls)
		return false;

	float fHitMass = 0.0f;
	for (int i = 0; i < nAlls; ++i)
		fHitMass += nxActors[i]->getMass();

	float fAllMass = GetModelMass(true);
	float fRatio = fAllMass / fHitMass;
	float fResMag = fTorqueMagnitude * fRatio;

	NxVec3 torque = rotAxis * fResMag;
	NxForceMode nxForceMode = TranslateForceMode(iPhysTorqueType);
	for (int i = 0; i < nAlls; ++i)
		nxActors[i]->addLocalTorque(torque, nxForceMode);

	return true;
}

#ifdef _ANGELICA21
//switch to PhysX pRootBone and its children(yx)
void A3DModelPhysSync::SwitchToPhys(A3DBone* pRootBone, float fPhsWeight, bool bIncChild, bool bChangePhyState)
{
	if (pRootBone == NULL)
		return;

	if (!m_pAnimWeight)
		return;

	A3DSkeleton* pSkeleton = m_pSkinModel->GetSkeleton();
	if (!pSkeleton)
		return;

	int index = -1;
	pSkeleton->GetBone(pRootBone->GetName(), &index);
	if (index >= 0)
		m_pAnimWeight[index] = 1 - fPhsWeight;

	if (bIncChild)
	{
		abase::vector<int> PhysBoneIndexs;
		GetBoneIndexs(pRootBone, PhysBoneIndexs);
		size_t nbIdx = PhysBoneIndexs.size();
		for (size_t i = 0; i < nbIdx; ++i)
			m_pAnimWeight[PhysBoneIndexs[i]] = 1 - fPhsWeight;
	}
	SetAnimBlendWeight(m_pAnimWeight, bChangePhyState);

}
#endif

//get pRootBone and its children index(yx)
void A3DModelPhysSync::GetBoneIndexs(A3DBone* pRootBone, abase::vector<int>& indexs)
{
	if (pRootBone == NULL)
		return;
	int idx = -1;

	A3DSkeleton* pSkeleton = m_pSkinModel->GetSkeleton();
	if (!pSkeleton)
		return;

	pSkeleton->GetBone(pRootBone->GetName(), &idx);
	indexs.push_back(idx);
	for (int i = 0; i < pRootBone->GetChildNum(); i++)
	{
		GetBoneIndexs(pRootBone->GetChildPtr(i), indexs);
	}
}
#ifdef _ANGELICA21
void A3DModelPhysSync::GetJointSpringAndDamping(const AString& strJointName, float& outfSpring, float& outfDamping)
{
	JointInfo tmp;

	if ((0 == m_pPhysD6JointDriveInfo) || (0 == m_pPhysD6JointDriveInfo->pAppliedSKFDesc))
	{
		assert(!"Missing applied SKFDesc!");
	}
	else
	{
		bool bSuccess = m_pPhysD6JointDriveInfo->pAppliedSKFDesc->GetJointInfo(tmp, strJointName);
		if (!bSuccess)
		{
			if (0 < m_pPhysD6JointDriveInfo->pAppliedSKFDesc->GetNBJointInfo())
			{
				m_pPhysD6JointDriveInfo->pAppliedSKFDesc->GetJointInfo(tmp);
			}
			else
			{
				assert(!"Missing general joint value!");
			}
		}
	}
	outfSpring = tmp.mSpring;
	outfDamping = tmp.mDamping;
}

float A3DModelPhysSync::EnforceGetTorqueScale(const NxVec3& scale3D, TorqueScaleType tstType) const
{
	float volScale = scale3D.x * scale3D.y * scale3D.z;
	float avgRScale = (scale3D.x + scale3D.y + scale3D.z) / 3.0f;

	float fResult = 1.0f;
	switch (tstType)
	{
	case TST_D6JOINT:
		fResult = avgRScale * avgRScale * volScale;
		break;
	case TST_FORCE:
		fResult = avgRScale * volScale;
		break;
	case TST_VELCHANGE:
		fResult = avgRScale;
		break;
	default:
		assert(!"Unknown Scale Type!");
	}
	return fResult;
}

void A3DModelPhysSync::SwitchToPhys(NxActor* pRootActor, float fPhsWeight, bool bIncChild, bool bChangePhyState)
{
	if (!m_pAnimWeight)
		return;

	int i;
	int nbActorImps = m_aActorImps.GetSize();
	for (i = 0; i < nbActorImps; ++i)
	{
		if (pRootActor == m_aActorImps[i]->pActor)
			break;
	}
	if (i < nbActorImps)
	{
		A3DModelPhysSyncData::LINK_INFO& link = m_aActorImps[i]->pActorData->Link;
		int nbBones = link.aBones.GetSize();
		abase::vector<int> vBoneIdx;
		vBoneIdx.reserve(nbBones);
		for (int j = 0; j < nbBones; j++)
		{
			vBoneIdx.push_back(link.aBones[j]->iBoneIdx);
		}

		size_t nbIdx = vBoneIdx.size();
		for (size_t m = 0; m < nbIdx; ++m)
		{
			m_pAnimWeight[vBoneIdx[m]] = 1 - fPhsWeight;
			if (bIncChild)
			{
				A3DSkeleton* pSkeleton = m_pSkinModel->GetSkeleton();
				if (pSkeleton)
				{
					abase::vector<int> indexs;
					GetBoneIndexs(pSkeleton->GetBone(vBoneIdx[m]), indexs);
					size_t nbChildIdx = indexs.size();
					for (size_t k = 0; k < nbChildIdx; ++k)
					{
						m_pAnimWeight[indexs[k]] = 1 - fPhsWeight;
					}
				}
			}
		}
		SetAnimBlendWeight(m_pAnimWeight, bChangePhyState);
	}
}

//animation & physx blend(yx)
void A3DModelPhysSync::SetAnimBlendWeight(float* pAnimWeight, bool bChangeState)
{
	A3DSkeleton* pSkeleton = m_pSkinModel->GetSkeleton();
	if (!pSkeleton)
		return;

	int nBoneNum = pSkeleton->GetBoneNum();
	if (pAnimWeight == NULL)
	{
		if (bChangeState)
			ChangePhysState(PHY_STATE_ANIMATION);
		else
		{
			m_bSetAnimWeight = false;
			if (m_pAnimWeight)
			{
				for (int i = 0; i < nBoneNum; ++i)
					m_pAnimWeight[i] = 1;
			}
		}
		return;
	}

	int iState = PHY_STATE_ANIMATION;
	for (int i = 0; i < nBoneNum; ++i)
	{
		if (pAnimWeight[i] != 1)
		{
			iState = PHY_STATE_PARTSIMULATE;
			break;
		}
	}

	// must call ChangePhysState() at first, then copy the weight
	// since the weight will be updated in ChangePhysState()
	if (bChangeState)
	{
		if (PHY_STATE_PARTSIMULATE == iState)
			assert(!"Not Support: change to any physics state! Only copy weight data!");
		else
			ChangePhysState(iState);
	}

	int j = 0;
	int nbActorImps = m_aActorImps.GetSize();
	for (int i = 0; i < nbActorImps; ++i)
	{
		ACTOR_IMP* pActorImp = m_aActorImps[i];
		A3DModelPhysSyncData::LINK_INFO& link = pActorImp->pActorData->Link;
		int nbBones = link.aBones.GetSize();
		for (j = 0; j < nbBones; ++j)
		{
			A3DModelPhysSyncData::BONE_DATA* pBoneData = link.aBones[j];
			if (pBoneData->iBoneIdx >= 0)
			{
				if (pAnimWeight[pBoneData->iBoneIdx] == 1)
					break;
			}
			else
			{
				assert(0 && "invalid BoneIdx!");
			}
		}
		if (j < nbBones)//contain bone driven by animation
		{
			for (j = 0; j < nbBones; ++j)
			{
				A3DModelPhysSyncData::BONE_DATA* pBoneData = link.aBones[j];
				if (pBoneData->iBoneIdx >= 0)
				{
					if (pBoneData->bAnimMajor)
						pAnimWeight[pBoneData->iBoneIdx] = 1;
				}
				else
				{
					assert(0 && "invalid BoneIdx!");
				}
			}
		}
	}

	if (m_pAnimWeight != pAnimWeight)
	{
		if (m_pAnimWeight)
			memcpy(m_pAnimWeight, pAnimWeight, sizeof(float) * nBoneNum);
	}
}

#endif

int A3DModelPhysSync::GetIndexByActor(const NxActor* pActor) const
{
	int nSize = m_aActorImps.GetSize();
	for (int i = 0; i < nSize; ++i)
	{
		if (pActor == m_aActorImps[i]->pActor)
			return i;
	}
	return -1;
}

bool A3DModelPhysSync::GetBoneScale(const A3DSkeleton& sel, int idxBone, NxVec3& outScale) const
{
	A3DBone* pMainBone = sel.GetBone(idxBone);
	if (0 == pMainBone)
		return false;

	const A3DMATRIX4& matScale = pMainBone->GetScaleMatrix();
	outScale.set(matScale.m[0][0], matScale.m[1][1], matScale.m[2][2]);
	outScale.x = FormatFloat(outScale.x);
	outScale.y = FormatFloat(outScale.y);
	outScale.z = FormatFloat(outScale.z);
	return true;
}

NxActor* A3DModelPhysSync::GetNxActor(A3DBone* pBone, NxVec3* pOut3DScale /* = 0 */, int* pOutIdxActor /* = 0 */) const
{
	if (!pBone)
		return NULL;

	A3DSkeleton* pSkeleton = m_pSkinModel->GetSkeleton();
	if (0 == pSkeleton)
		return NULL;

	int nSize = m_aActorImps.GetSize();
	for (int i = 0; i < nSize; ++i)
	{
		A3DModelPhysSyncData::LINK_INFO& link = m_aActorImps[i]->pActorData->Link;
		int nBoneSize = link.aBones.GetSize();
		for (int j = 0; j < nBoneSize; ++j)
		{
			A3DBone* pLinkBone = pSkeleton->GetBone(link.aBones[j]->iBoneIdx);
			if (pLinkBone == pBone)
			{
				if (0 != pOut3DScale)
				{
					pOut3DScale->set(1.0f);
					GetBoneScale(*pSkeleton, link.iMainBone, *pOut3DScale);
				}

				if (pOutIdxActor)
					*pOutIdxActor = i;

				return m_aActorImps[i]->pActor;
			}
		}
	}
	return NULL;
}

NxActor* A3DModelPhysSync::GetNxActor(const char* szBoneName, NxVec3* pOut3DScale /* = 0 */, int* pOutIdxActor /* = 0 */) const
{
	if (m_pSkinModel && m_pSkinModel->GetSkeleton())
	{
		A3DBone* pBone = m_pSkinModel->GetSkeleton()->GetBone(szBoneName, NULL);
		return GetNxActor(pBone, pOut3DScale, pOutIdxActor);
	}

	return NULL;
}


void A3DModelPhysSync::SetRBSimMode(int iRBSimMode, bool bForceReset /*= false*/)
{
	if(!bForceReset && m_iRBSimMode == iRBSimMode) return;

	EnableSyncAnimPoseToPhysX(true);
	if(iRBSimMode == RB_SIMMODE_HARD_KEYFRAME)
	{
		if (m_iRBSimMode == RB_SIMMODE_SOFT_KEYFRAME)
			DisableSettingsFromSoftKeyFrame(true);
	}
	else if (iRBSimMode == RB_SIMMODE_SOFT_KEYFRAME)
	{
		EnableSettingsForSoftKeyFrame();
	}
	else if (iRBSimMode == RB_SIMMODE_RAGDOLL)
	{
		if (m_iPhysState == PHY_STATE_SIMULATE)
			EnableSyncAnimPoseToPhysX(false);

		DisableSettingsFromSoftKeyFrame(false);
	}

	m_iRBSimMode = iRBSimMode;
}

void A3DModelPhysSync::EnableSettingsForSoftKeyFrame()
{
#ifdef _ANGELICA21
	// close cloth simulation during SoftKeyFrame mode, since there is one frame delay which will cause jittering...
	CECModel* pECM = m_pModelPhysics->GetECModel();
	m_bToOpenCloth = pECM->IsClothSimOpened();
	if (m_bToOpenCloth)
	{
		pECM->CloseAllClothes();
		pECM->CloseOnBoneChildCloth();
	}

	// handle kinematic flag and disable actors' collision...
	bool bIsKinematic = false;
	unsigned int nbKActors = m_pPhysD6JointDriveInfo->mKinematicActors.size();
	int nActors = m_aActorImps.GetSize();
	for (int i = 0; i < nActors; ++i)
	{
		bIsKinematic = APhysXUtilLib::APhysXArray_Find(m_pPhysD6JointDriveInfo->mKinematicActors, m_aActorImps[i]->pActor);
		gGetAPhysXScene()->LockWritingScene();
		if (0 < nbKActors)
		{
			m_aActorImps[i]->pActor->raiseActorFlag(NX_AF_DISABLE_COLLISION);
		}
		else
		{
			// recover original state for double call EnableSettingsForSoftKeyFrame()
			A3DModelPhysSyncData::ACTOR_DATA* pActorData = m_aActorImps[i]->pActorData;
			APhysXActorDesc* pActorDesc = pActorData->GetActorDesc();
			if (pActorDesc->mActorFlags & NX_AF_DISABLE_COLLISION)
				m_aActorImps[i]->pActor->raiseActorFlag(NX_AF_DISABLE_COLLISION);	
			else
				m_aActorImps[i]->pActor->clearActorFlag(NX_AF_DISABLE_COLLISION);
		}
		if (bIsKinematic)
			m_aActorImps[i]->pActor->raiseBodyFlag(NX_BF_KINEMATIC);
		else
		{
			m_aActorImps[i]->pActor->clearBodyFlag(NX_BF_KINEMATIC);
			
			// clear the linear velocity and angular velocity here...
			m_aActorImps[i]->pActor->setLinearVelocity(NxVec3(0.0f));
			m_aActorImps[i]->pActor->setAngularVelocity(NxVec3(0.0f));

		}
		gGetAPhysXScene()->UnlockWritingScene();
	}

	if (0 == nbKActors)
	{
		if (m_pPhysModel)
		{
			m_bIsUsingSelfCollision = true;
			m_pPhysModel->EnableSelfCollision(false);
		}
	}

	// enable joint's slerp drive & motion drive...
	m_bIsUsingLinearDrives = m_EnableLinearDrives;
	m_bIsUsingAngularDrives = m_EnableAngularDrives;

	float fSpring = 0.0f;
	float fDamping = 0.0f;
	A3DSkeleton* pSkeleton = m_pSkinModel->GetSkeleton();
	int nJoints = m_aJointImps.GetSize();
	for (int i = 0; i < nJoints; ++i)
	{
		JOINT_IMP* pJointImp = m_aJointImps[i];
		if(!pJointImp->pD6Joint)
			continue;

		if (!m_bIsUsingLinearDrives)
		{
			if (pJointImp->pJointData->ReadFlag(A3DModelPhysSyncData::JDF_BREAKABLE_EQUIPMENTS))
				continue;
		}

		GetJointSpringAndDamping(pJointImp->strJointName, fSpring, fDamping);

		NxVec3 scale3D(1.0f);
		if (0 != pSkeleton)
			GetBoneScale(*pSkeleton, pJointImp->pJointData->GetActor(1)->Link.iMainBone, scale3D);
		float fAllSacle = EnforceGetTorqueScale(scale3D, TST_D6JOINT);
		fSpring *= fAllSacle;
		fDamping *= fAllSacle;
		if (0 > fSpring)
			fSpring = FLT_MAX;
		if (0 > fDamping)
			fDamping = FLT_MAX;

		NxD6JointDesc d6JointDesc;
		pJointImp->pD6Joint->saveToDesc(d6JointDesc);

		if (m_bIsUsingAngularDrives)
		{
			// adjust the parameters of d6JointDesc so as to apply slerp drive...
			d6JointDesc.flags|=NX_D6JOINT_SLERP_DRIVE;    
			d6JointDesc.slerpDrive.driveType=NX_D6JOINT_DRIVE_POSITION;    
			d6JointDesc.slerpDrive.spring = fSpring; 
			d6JointDesc.slerpDrive.damping= fDamping;

			d6JointDesc.twistMotion = NX_D6JOINT_MOTION_FREE;
			d6JointDesc.swing1Motion = NX_D6JOINT_MOTION_FREE;
			d6JointDesc.swing2Motion = NX_D6JOINT_MOTION_FREE;

			d6JointDesc.xMotion = NX_D6JOINT_MOTION_LOCKED;
			d6JointDesc.yMotion = NX_D6JOINT_MOTION_LOCKED;
			d6JointDesc.zMotion = NX_D6JOINT_MOTION_LOCKED;
		}

		if (m_bIsUsingLinearDrives)
		{
			//-------------------------------------------------------------------------
			// Noted by Wenfeng, Mar. 8, 2013
			// 
			// turn on the X, Y and Z drive so as to support bones' position 
			// offset in their parent frame.
			// 
			// Beware: to make this work well, you have to adjust the jointed
			// actors' solver iteration to a high value, such as 100.

			d6JointDesc.xDrive.driveType = NX_D6JOINT_DRIVE_POSITION;
			d6JointDesc.xDrive.spring = FLT_MAX;

			d6JointDesc.yDrive.driveType = NX_D6JOINT_DRIVE_POSITION;
			d6JointDesc.yDrive.spring = FLT_MAX;

			d6JointDesc.zDrive.driveType = NX_D6JOINT_DRIVE_POSITION;
			d6JointDesc.zDrive.spring = FLT_MAX;

			d6JointDesc.xMotion = NX_D6JOINT_MOTION_FREE;
			d6JointDesc.yMotion = NX_D6JOINT_MOTION_FREE;
			d6JointDesc.zMotion = NX_D6JOINT_MOTION_FREE;
			//-------------------------------------------------------------------------
		}

		gGetAPhysXScene()->LockWritingScene();
		pJointImp->pD6Joint->loadFromDesc(d6JointDesc);
		gGetAPhysXScene()->UnlockWritingScene();
	}
	m_bIsEnabledSoftKeyFrame = true;
#endif
}

void A3DModelPhysSync::DisableSettingsFromSoftKeyFrame(bool bIsSkipKinematicFlag)
{
	m_bIsEnabledSoftKeyFrame = false;
	// enable actors' collision and kinematic flag according to the actor's description saved in sync data.
	APhysXActorDesc* pActorDesc = 0;
	int nActors = m_aActorImps.GetSize();
	for (int i = 0; i < nActors; ++i)
	{
		A3DModelPhysSyncData::ACTOR_DATA* pActorData = m_aActorImps[i]->pActorData;
		pActorDesc = pActorData->GetActorDesc();
		gGetAPhysXScene()->LockWritingScene();
		if (pActorDesc->mActorFlags & NX_AF_DISABLE_COLLISION)
			m_aActorImps[i]->pActor->raiseActorFlag(NX_AF_DISABLE_COLLISION);	
		else
			m_aActorImps[i]->pActor->clearActorFlag(NX_AF_DISABLE_COLLISION);

		if (!bIsSkipKinematicFlag)
		{
			if (pActorDesc->mBodyFlags & NX_BF_KINEMATIC)
				m_aActorImps[i]->pActor->raiseBodyFlag(NX_BF_KINEMATIC);
			else
				m_aActorImps[i]->pActor->clearBodyFlag(NX_BF_KINEMATIC);
		}
		gGetAPhysXScene()->UnlockWritingScene();
	}
	if (m_bIsUsingSelfCollision)
	{
		if (m_pPhysModel)
		{
			m_bIsUsingSelfCollision = false;
			m_pPhysModel->EnableSelfCollision(true);
		}
	}

	// disable joint's slerp drive...
	for (int i=0; i < m_aJointImps.GetSize(); i++)
	{
		JOINT_IMP* pJointImp = m_aJointImps[i];
		if(!pJointImp->pD6Joint)
			continue;

		if (!m_bIsUsingLinearDrives)
		{
			if (pJointImp->pJointData->ReadFlag(A3DModelPhysSyncData::JDF_BREAKABLE_EQUIPMENTS))
				continue;
		}

		NxD6JointDesc d6JointDesc;
		pJointImp->pD6Joint->saveToDesc(d6JointDesc);

		d6JointDesc.flags &= ~NX_D6JOINT_SLERP_DRIVE;    

		APhysXD6JointDesc* pD6JointDesc = (APhysXD6JointDesc*)pJointImp->pJointData->GetJointDesc();
		d6JointDesc.twistMotion = pD6JointDesc->mTwistMotion;
		d6JointDesc.swing1Motion = pD6JointDesc->mSwing1Motion;
		d6JointDesc.swing2Motion = pD6JointDesc->mSwing2Motion;

		d6JointDesc.xMotion = pD6JointDesc->mXMotion;
		d6JointDesc.yMotion = pD6JointDesc->mYMotion;
		d6JointDesc.zMotion = pD6JointDesc->mZMotion;

		gGetAPhysXScene()->LockWritingScene();
		pJointImp->pD6Joint->loadFromDesc(d6JointDesc);
		gGetAPhysXScene()->UnlockWritingScene();
	}
	m_bIsUsingLinearDrives = false;
	m_bIsUsingAngularDrives = false;

	if (m_bToOpenCloth)
	{
		CECModel* pECM = m_pModelPhysics->GetECModel();
		const ClothSimDesc& paras = pECM->GetClothSimParameters();
		pECM->OpenAllClothes(paras.iLinkType, paras.iDelayTime, paras.iBlendTime);

		pECM->OpenOnBoneChildCloth();

		m_bToOpenCloth = false;
	}
}

bool A3DModelPhysSync::ClothesStateChangeEvent(bool bIsToOpen)
{
	if (m_bIsEnabledSoftKeyFrame)
	{
		m_bToOpenCloth = bIsToOpen;
		return true;
	}
	return false;
}

#ifdef _ANGELICA21

bool A3DModelPhysSync::ApplySoftKeyFrameConfig(const AString& skfDescName, bool bEnableLinearDrives, bool bEnableAngularDrives)
{
	SoftKeyFrameConfigDesc* pSKFCD = m_pCoreData->GetSKFConfigDescByName(skfDescName);
	if (0 == pSKFCD)
		return false; 

	if (m_pPhysD6JointDriveInfo->pAppliedSKFDesc == pSKFCD)
	{
		if (!pSKFCD->IsModified())
		{
			if ((m_EnableLinearDrives == bEnableLinearDrives) && (m_EnableAngularDrives == bEnableAngularDrives))
				return true;
		}
	}

	m_pPhysD6JointDriveInfo->pAppliedSKFDesc = pSKFCD;
	m_pPhysD6JointDriveInfo->pAppliedSKFDesc->ClearModifiedFlag();
	ConvertSKFDescToDriveInfo(*m_pPhysD6JointDriveInfo, *pSKFCD, *this);
	m_EnableLinearDrives = bEnableLinearDrives;
	m_EnableAngularDrives = bEnableAngularDrives;
	
	// reset soft key-frame mode...
	if (m_iRBSimMode == RB_SIMMODE_SOFT_KEYFRAME)
		SetRBSimMode(RB_SIMMODE_SOFT_KEYFRAME, true);
	return true;
}

SoftKeyFrameConfigDesc* A3DModelPhysSync::GetAppliedSoftKeyFrameConfig() const
{
	if (0 == m_pPhysD6JointDriveInfo)
		return NULL;

	return m_pPhysD6JointDriveInfo->pAppliedSKFDesc;
}

#endif

float A3DModelPhysSync::GetScaleForSoftKeyFrame(const NxVec3& scale3D, bool bMomentArmOnly) const
{
	float fResult = 1.0f;

#ifdef _ANGELICA21
	if (GetRBSimMode() == RB_SIMMODE_SOFT_KEYFRAME)
	{
		if (bMomentArmOnly)
			fResult = EnforceGetTorqueScale(scale3D, TST_VELCHANGE);
		else
			fResult = EnforceGetTorqueScale(scale3D, TST_FORCE);
	}
#endif

	return fResult;
}

void A3DModelPhysSync::AddRotationBone(const AString& strBoneName, bool bIsMajorBone)
{
	int nAll = m_aRotationBones.GetSize();
	for (int i = 0; i < nAll; ++i)
	{
		if (m_aRotationBones[i]->strBone == strBoneName)
		{
			m_aRotationBones[i]->bIsMajorBone = bIsMajorBone;
			return;
		}
	}

	NxActor* pActor = 0;
	int nbActors = m_aActorImps.GetSize();
	for (int i = 0; i < nbActors; ++i)
	{
		A3DModelPhysSyncData::LINK_INFO& link = m_aActorImps[i]->pActorData->Link;
		int nbBones = link.aBones.GetSize();
		for (int j = 0; j < nbBones; ++j)
		{
			if (link.aBones[j]->GetBoneName() == strBoneName)
			{
				pActor = m_aActorImps[i]->pActor;
				break;
			}
		}
		if (0 != pActor)
			break;
	}

	ROTATION_BONE_INFO* pRBI = new ROTATION_BONE_INFO;
	pRBI->bIsMajorBone = bIsMajorBone;
	pRBI->strBone = strBoneName;
	pRBI->pActor = pActor;
	m_aRotationBones.Add(pRBI);
}

void A3DModelPhysSync::RemoveRotationBone(const AString& strBoneName)
{
	int nAll = m_aRotationBones.GetSize();
	for (int i = 0; i < nAll; ++i)
	{
		if (m_aRotationBones[i]->strBone == strBoneName)
		{
			delete m_aRotationBones[i];
			m_aRotationBones.RemoveAt(i);
			return;
		}
	}
}

void A3DModelPhysSync::RemoveAllRotationBone()
{
	int nAll = m_aRotationBones.GetSize();
	for (int i = 0; i < nAll; ++i)
		delete m_aRotationBones[i];
	m_aRotationBones.RemoveAll();
}

void A3DModelPhysSync::BuildTorqueActors()
{
	bool bGoNext = false;
	int nbActors = m_aActorImps.GetSize();
	for (int i = 0; i < nbActors; ++i)
	{
		bGoNext = false;
		A3DModelPhysSyncData::LINK_INFO& link = m_aActorImps[i]->pActorData->Link;
		int nbBones = link.aBones.GetSize();
		for (int j = 0; j < nbBones; ++j)
		{
			int nbRotBones = m_aRotationBones.GetSize();
			for (int k = 0; k < nbRotBones; ++k)
			{
				AString strName = link.aBones[j]->GetBoneName();
				if (strName == m_aRotationBones[k]->strBone)
				{
					m_aRotationBones[k]->pActor = m_aActorImps[i]->pActor;
					bGoNext = true;
					break;
				}
			}
			if (bGoNext)
				break;
		}
	}
}

void A3DModelPhysSync::DestoryTorqueActors()
{
	int nbRotBones = m_aRotationBones.GetSize();
	for (int k = 0; k < nbRotBones; ++k)
		m_aRotationBones[k]->pActor = 0;
}

bool A3DModelPhysSync::RaisePinnedFlagToActorImp(const NxActor& rActor)
{
	int nAll = m_aActorImps.GetSize();
	for (int i = 0; i < nAll; ++i)
	{
		ACTOR_IMP* pImp = m_aActorImps[i];
		if (&rActor == pImp->pActor)
		{
			pImp->bIsPinned = true;
			return true;
		}
	}
	return false;
}

bool A3DModelPhysSync::ComputePoseAsChildModel(A3DMATRIX4& outNewPoseTM)
{
#ifdef _ANGELICA21
	if (0 == m_pModelPhysics)
		return false;

	CECModel* pModel = m_pModelPhysics->GetECModel();
	CECModel* pParent = pModel->GetParentModel();
	if (0 == pParent)
		return false;

	A3DModelPhysSync* pParentSync = pParent->GetPhysSync();
	if (0 == pParentSync)
		return false;

	if (PHY_STATE_ANIMATION == pParentSync->GetPhysState())
		return false;

	A3DHanger* pHanger = 0;
	A3DSkinModelHanger* pSMH =0;
	A3DSkinModel* pParentModel = pParent->GetA3DSkinModel();
	int nbHanger = pParentModel->GetHangerNum();
	for (int i = 0; i < nbHanger; ++i)
	{
		pHanger = pParentModel->GetHanger(i);
		if (A3DHanger::TYPE_SKINMODEL != pHanger->GetType())
			continue;

		pSMH = (A3DSkinModelHanger*)pHanger;
		if (m_pSkinModel != pSMH->GetModelObject())
			continue;

		AString strCCName = pSMH->GetCCName();
		A3DSkeletonHook* pChildCCHook = m_pSkinModel->GetSkeletonHook(strCCName, true);
		A3DSkeletonHook* pParentHook = pSMH->GetHook(); // Note: this interface only exist in Angelica21
		if ((0 == pChildCCHook) || (0 == pParentHook))
			continue;

		// set original pose to model
		A3DMATRIX4 matPoseBackup = m_pSkinModel->GetAbsoluteTM();
		A3DMATRIX4 matRes(A3DMATRIX4::IDENTITY);
		m_pSkinModel->SetAbsoluteTM(matRes);
		m_pSkinModel->Update(0, true);

		pChildCCHook->Update(true);
		A3DMATRIX4 matInvCCTM;
		a3d_InverseTM(pChildCCHook->GetNoScaleAbsTM(), &matInvCCTM);

		A3DMATRIX4 matOffset;
		pSMH->GetScaledOffsetMat(matOffset); // get Offset from parent hook

		A3DMATRIX4 matAbsHookPose = pParentHook->GetAbsoluteTM();
		outNewPoseTM = matInvCCTM * matOffset * matAbsHookPose;

		// recover model pose
		m_pSkinModel->SetAbsoluteTM(matPoseBackup);
		m_pSkinModel->Update(0, true);
		return true;
	}

#endif

	return false;
}

#endif	//	A3D_PHYSX

