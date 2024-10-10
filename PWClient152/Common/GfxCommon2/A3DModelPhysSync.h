/*
 * FILE: A3DModelPhysSync.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2008/5/21
 *
 * HISTORY: 
 *
 * Copyright (c) 2008 Archosaur Studio, All Rights Reserved.
 */

#ifndef _A3DMODELPHYSSYNC_H_
#define _A3DMODELPHYSSYNC_H_

#include <A3DMatrix.h>
#include <AArray.h>
#include <AString.h>
#include <vector.h>
#include "A3DModelPhysSyncData.h"
#include "A3DModelSoftKeyFrame.h"

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////

class A3DSkinModel;
class APhysXSkeletonRBObject;
class APhysXScene;
class APhysXRBAttacher;
class NxActor;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class A3DModelPhysSync
//	
///////////////////////////////////////////////////////////////////////////

struct PhysBreakInfo
{
	A3DVECTOR3		mBreakPos;
	float			mBreakMagnitude;
};

// for soft key-frame simulation mode
#ifdef _ANGELICA21
class PhysD6JointDriveInfo : public SoftKeyFrameDescNotify
{
public:
	SoftKeyFrameConfigDesc* pAppliedSKFDesc;
	APhysXArray<NxActor* > mKinematicActors;		// the actors which will be driven by animation

	PhysD6JointDriveInfo() : pAppliedSKFDesc(NULL)
	{
	}

	virtual void onReleaseDesc(SoftKeyFrameConfigDesc& skfDesc);
};
#endif

#ifdef _ANGELICA21
class A3DSkeletonBlend;
#endif

class A3DModelPhysSync
{
public:		//	Types

	//	Physical state
	enum
	{
		PHY_STATE_SIMULATE = 0,			//	Pure physical simulation
		PHY_STATE_ANIMATION,			//	Pure model animation
		PHY_STATE_PARTSIMULATE,			//	Partially physical simulation
		NUM_PHY_STATE,
	};


	// The simulation mode of the skeleton rigid body...
	enum
	{
		RB_SIMMODE_HARD_KEYFRAME,		// hard key-frame mode will align the RB actors to 
										// their corresponding bones exactly according to animations,
										// and in this mode, RB actors are simulated as kinematic ones.
		
		RB_SIMMODE_SOFT_KEYFRAME,		// soft key-frame mode will apply joint drives to keep the actors 
										// up with bones which gain poses from key-frame data.
										//---------------
										// Note that in this mode, some actors should be kinematic and aligned
										// with bones directly so that the whole skeleton rigid-body can be supported.
		
		RB_SIMMODE_RAGDOLL,				// the fully free rag-doll mode, this can be simulated for death effect.
	};


	//	Actor implement
	struct ACTOR_IMP
	{
		bool		bIsPinned;
		NxActor*	pActor;				//	Actor object
		A3DModelPhysSyncData::ACTOR_DATA* pActorData;

		//	Operations
		ACTOR_IMP();
	};

	struct JOINT_IMP
	{
		AString strJointName;
		NxD6Joint*	pD6Joint;
		NxActor*	pActor0;		// the pointer is valid even the joint is broken or released
		NxActor*	pActor1;		// the pointer is valid even the joint is broken or released
		NxMat33 mLocalRot0;			// the rotation matrix of joint frame 0 according to actor0
		NxMat33 mLocalRot1;			// the rotation matrix of joint frame 1 according to actor1

		NxVec3 mLocalAnchor0;
		NxVec3 mLocalAnchor1;

		A3DModelPhysSyncData::JOINT_DATA* pJointData;

		JOINT_IMP():pD6Joint(NULL), pActor0(NULL), pActor1(NULL), mLocalRot0(NX_IDENTITY_MATRIX), mLocalRot1(NX_IDENTITY_MATRIX), pJointData(NULL), mLocalAnchor0(0.0f), mLocalAnchor1(0.0f)
		{

		}
	};

	//	Absolute matrices of bones calculated by physical system
	struct BONE_MAT
	{
		A3DMATRIX4	matBoneAbs;			//	Bone's absolute matrix
		A3DMATRIX4	matBoneInvAbs;		//	Bone's inverse absolute matrix
		bool		bMainBone;			//	Main bone flag

		//	Operations
		BONE_MAT();
	};

	//	Attacher link info
	struct ATTACHER_LINK
	{
		AString		strParentHook;
		int			nParentHookIdx;
		AString		strAttacher;
		int			nAttacherIdx;
	};
	
	//	Bone scale cached data
	struct BONE_SCALE_CACHE_DATA
	{
		BONE_SCALE_CACHE_DATA(A3DBone* pBone);
		float fLocalLengthScale;
		float fLocalThickScale;
		float fAccuWholeScale;
	};

	struct ROTATION_BONE_INFO
	{
		ROTATION_BONE_INFO() : bIsMajorBone(false), pActor(0) {}
		AString		strBone;
		bool 		bIsMajorBone;
		NxActor*	pActor;
	};

	friend class A3DModelPhysics;
	
public:		//	Constructor and Destructor

	//	A3DModelPhysSync must be created by A3DModelPhysics's instance.
	//	A3DModelPhysSync also need the A3DModelPhysics pointer during its lifetime.
	A3DModelPhysSync(A3DModelPhysics* pModelPhysics);
	virtual ~A3DModelPhysSync();

public:		//	Attributes
	static NxForceMode TranslateForceMode(int iType);

#ifdef _ANGELICA21
	static bool ConvertSKFDescToDriveInfo(PhysD6JointDriveInfo& outDI, const SoftKeyFrameConfigDesc& rSKFDesc, const A3DModelPhysSync& rMPS);
#endif

	enum DefSKFConfigName
	{
		CN_INJURY = 0,
		CN_PICK_THROW,
		CN_FROZEN_DEATH,
	};
	static const char* GetDefSKFConfigName(DefSKFConfigName id);

public:		//	Operations

	//	Update model's animation. Call this before physics simulation
	bool UpdateModelAnim(int iDeltaTime);
	//	Sync model state from physical data. Call this after physics simulation
	bool SyncModelPhys();

	//	Bind skin model with a physical object
	//	bRebuildDesc means release the old attacher desc and re-build a new one (used in editor for applying the changes)
	bool Bind(A3DSkinModel* pSkinModel, A3DModelPhysSyncData* pCoreData, APhysXScene* pPhysScene, bool bRebuildDesc = false);
	//	After model is teleport to a new position, this function should be could to sync physical data
	bool Teleport();

	//	Check if physical data bound successfully
	bool GetPhysBindFlag() const { return m_bPhysBindOk; }
	//	Change physical state
	bool ChangePhysState(int iState, int iRBSimMode = -1);
	//	Get current physical state
	int GetPhysState() const { return m_iPhysState; }
	//	Get current RB sim mode
	int GetRBSimMode() const { return m_iRBSimMode; }
	//	Get RB object mass
	float GetModelMass(bool bOnlyBodyMass = false) const;

	//	Open physical link
	bool OpenPhysLinkAsChildModel(const char* szParentHook, const char* szChildAttacher);
	//	Close physical link
	void ClosePhysLinkAsChildModel();
	//	Add physical link
	bool AddPhysLinkAsChildModel(const char* szParentHook, const char* szChildAttacher);
	//	Change physical link
	bool ChangePhysLinkAsChildModel(const char* szChildAttacher, const char* szNewHook);
	//	Remove physical link
	bool RemovePhysLinkAsChildModel(const char* szChildAttacher);
	//	Get attacher
	ATTACHER_LINK* GetPhysLinkAsChildModel(const char* szChildAttacher, int* pIndex = NULL) const;
	

	//	Get model physical object
	APhysXSkeletonRBObject* GetSkeletonRBObject() const { return m_pPhysModel; }

	//	Break Joint
	void BreakJoint(const char* szJoint);
	//	Break Joint by bone name
	void BreakJointByBone(const char* szBoneName);
	bool IsBroken() const { return m_bPhysBroken; }
	
	//	Apply force
	//	fMaxVelChangeHint  hint the max value of velocity change, this parameter will be ignored if the ECM is a breakable model(no matter is broken or not).
	//	return a pointer point to the actor which is added force successful, or return a null pointer.
	NxActor* AddForce(const A3DVECTOR3& vStart, const A3DVECTOR3& vDir, float fForceMagnitude, float fMaxDist, int iPhysForceType, float fSweepRadius = 0.0f, float fMaxVelChangeHint = FLT_MAX);
	bool AddTorque(int iRotateAxis, float fTorqueMagnitude, int iPhysTorqueType);

	//----------------------------------------------------------------------------------
	// test code for breakable...
	void SetBoneAnimDriven(bool bAnimDriven = true);
	
	void OnBreak(const PhysBreakInfo& physBreakInfo);				// called by the APhysXEngine's Break Notify...
	void OnFirstBreak();		// called by the APhysXEngine's first Break Notify...
	void OnAllBreak();			// called by the APhysXEngine's all Break Notify...


	A3DSkinModel* GetSkinModel() const { return m_pSkinModel; }

	// get the reference of A3DModelPhysics...
	A3DModelPhysics* GetA3DModelPhysics() { return m_pModelPhysics; } 

	bool IsFullAnimation() 
	{ 
		return 
			(
			
			m_iPhysState == PHY_STATE_ANIMATION 

#ifdef _ANGELICA21

			&& !m_bSetAnimWeight

#endif
			); 
	}


#ifdef _ANGELICA21

	//animation & physx blend(yx), set animation weight to 1 when pAnimWeight is NULL, bChangeState change physx state according to weight? 
	void SetAnimBlendWeight(float* pAnimWeight = NULL, bool bChangeState = true);
	//switch to physx from pRootBone, if fPhsWeight = 0 reset to animation, bIncChild include children or not(yx), 
	void SwitchToPhys(A3DBone* pRootBone, float fPhsWeight = 1, bool bIncChild = true, bool bChangePhyState = true);
	void SwitchToPhys(NxActor* pRootActor, float fPhsWeight = 1, bool bIncChild = true, bool bChangePhyState = true);

#endif

	//get bone's actor
	NxActor* GetNxActor(A3DBone* pBone, NxVec3* pOut3DScale = 0, int* pOutIdxActor = 0) const;
	NxActor* GetNxActor(const char* szBoneName, NxVec3* pOut3DScale = 0, int* pOutIdxActor = 0) const;

	// return true if this object processed the event,or return false
	bool ClothesStateChangeEvent(bool bIsToOpen);
#ifdef _ANGELICA21
	bool ApplySoftKeyFrameConfig(const AString& skfDescName, bool bEnableLinearDrives = false, bool bEnableAngularDrives = true);
	SoftKeyFrameConfigDesc* GetAppliedSoftKeyFrameConfig() const;
#endif
	float GetScaleForSoftKeyFrame(const NxVec3& scale3D, bool bMomentArmOnly = false) const; 
	bool IsLinearDrivesEnabled() const { return m_EnableLinearDrives; }
	bool IsAngularDrivesEnabled() const	{ return m_EnableAngularDrives;	}

	void EnableAnimation(bool bEnableAnim = true)
	{
		m_bEnableAnim = bEnableAnim;
	}
	bool IsAnimationEnabled() const { return m_bEnableAnim; }

	void EnableSyncAnimPoseToPhysX(bool bEnable = true)
	{
		m_bEnableSyncPoseByAnim = bEnable;
	}
	bool IsSyncAnimPoseToPhysXEnabled() const { return m_bEnableSyncPoseByAnim; }

	void AddRotationBone(const AString& strBoneName, bool bIsMajorBone);
	void RemoveRotationBone(const AString& strBoneName);
	void RemoveAllRotationBone();

	bool RaisePinnedFlagToActorImp(const NxActor& rActor);
	int GetActorCount() const { return m_aActorImps.GetSize(); }
	const ACTOR_IMP* GetActorByIndex(int idx) const
	{
		if (idx >= 0 && idx < m_aActorImps.GetSize())
			return m_aActorImps[idx];
		return NULL;
	}
	int GetIndexByActor(const NxActor* pActor) const;

	int GetJointCount() const { return m_aJointImps.GetSize(); }
	const JOINT_IMP* GetJointByIndex(int idx) const
	{
		if (idx >= 0 && idx < m_aJointImps.GetSize())
			return m_aJointImps[idx];
		return NULL;
	}

protected:	//	Attributes

	APhysXScene*			m_pPhysScene;		//	Physical scene object
	A3DSkinModel*			m_pSkinModel;		//	Skin model
	APhysXSkeletonRBObject*	m_pPhysModel;		//	Physical object
	APhysXRBAttacher*		m_pPhysAttacher;	//	Physical attacher
	A3DModelPhysSyncData*	m_pCoreData;		//	Core data
	A3DModelPhysics*		m_pModelPhysics;	//	Model physics;

	bool					m_bPhysBindOk;		//	true, physical data bound successfully
	int						m_iPhysState;		//	Physical state
	bool					m_bPhysLinkOpen;	//	true, physical link opened
	bool					m_bPhysBroken;		//	true, physical skeleton object is broken now(first break called)

	APtrArray<ACTOR_IMP*>	m_aActorImps;		//	Actor implements
	APtrArray<JOINT_IMP*>	m_aJointImps;		//	Joint implements

	APtrArray<ATTACHER_LINK*> m_aAttacherLinks;	//	Attacher link info
	AArray<BONE_MAT>		m_aBoneMats;		//	Absolute matrices of bones calculated by physical system
	APtrArray<BONE_SCALE_CACHE_DATA*>	m_aBoneScaleCache;	//	Bone Scale cache

#ifdef _ANGELICA21
	A3DSkeletonBlend*       m_pSkBlend;         //  Skeleton blend animation and physx(yx)
	float*                  m_pAnimWeight;      //  animation weight(yx)
	bool                    m_bSetAnimWeight;   //  animation weight should be used or not(yx)

 	PhysD6JointDriveInfo*	m_pPhysD6JointDriveInfo;
#endif

	int						m_iRBSimMode;
	bool					m_bIsUsingSelfCollision;
	bool					m_bIsUsingLinearDrives;
	bool					m_bIsUsingAngularDrives;
	bool					m_EnableLinearDrives;
	bool					m_EnableAngularDrives;
	bool					m_bIsEnabledSoftKeyFrame;
	bool					m_bToOpenCloth;

	bool					m_bEnableAnim;
	bool					m_bEnableSyncPoseByAnim;
	APtrArray<ROTATION_BONE_INFO*>	m_aRotationBones;

protected:	//	Operations

	//	Check whether the physical link is already exist
	bool CheckPhysLinkExist(const char* szParentHook, const char* szChildAttacher);
	//	Check whether the skeleton is scaled
	bool CheckSkeletonScaled(A3DSkeleton* pSkeleton);
	//	Create physical model (attacher obj, and get the model from the attacher obj)
	bool CreatePhysAttacherAndModel(A3DSkinModel* pSkinModel, bool bRebuildDesc);
	//	Release physical attacher
	void ReleasePhysAttacher();
	//	Release all actor implements
	void ReleaseActorAndJointImps();
	//	Release all attacher link infos
	void ReleaseAttacherLinkInfo();
	//	Build actor implements
	bool BuildActorAndJointImps();
	//	Build attacher link infos
	bool BuildAttacherLinkInfos(const char* szParentHook, const char* szChildAttacher);
	//	Update actor state by animation
	void UpdateActorsByAnim(bool bTeleport);
	//	Update attachers state by parent's hooks position
	void UpdateAttachersByParentAndLinkInfo();
	//	Update physically driven bones' state
	void UpdateBonesByPhys();
	void UpdateBonesByPhys_r(const A3DMATRIX4& matInvModel, int iBoneIdx);
	//	Find actor through specified bone or hh-type hook
	NxActor* GetActorByHook(int iHookIndex, bool bHHIsBone, A3DMATRIX4& matHook);

	bool ComputePoseAsChildModel(A3DMATRIX4& outNewPoseTM);

	float FormatFloat(float fValue) const;
	bool GetBoneScale(const A3DSkeleton& sel, int idxBone, NxVec3& outScale) const;

	//get pRootBone and its children index(yx)
	void GetBoneIndexs(A3DBone* pRootBone, abase::vector<int>& indexs);
#ifdef _ANGELICA21
	void GetJointSpringAndDamping(const AString& strJointName, float& outfSpring, float& outfDamping);

	enum TorqueScaleType
	{
		TST_D6JOINT = 0,
		TST_FORCE,
		TST_VELCHANGE,
	};
	float EnforceGetTorqueScale(const NxVec3& scale3D, TorqueScaleType tstType) const;
#endif

	void EnableSettingsForSoftKeyFrame();
	void DisableSettingsFromSoftKeyFrame(bool bIsSkipKinematicFlag);

	void SetRBSimMode(int iRBSimMode, bool bForceReset = false);
	void BuildTorqueActors();
	void DestoryTorqueActors();
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

#endif	//	_A3DMODELPHYSSYNC_H_
