/*
 * FILE: A3DModelPhysSyncData.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2008/5/21
 *
 * HISTORY: 
 *
 * Copyright (c) 2008 Archosaur Studio, All Rights Reserved.
 */

#ifndef _A3DMODELPHYSSYNCDATA_H_
#define _A3DMODELPHYSSYNCDATA_H_

#include <AString.h>
#include <A3DMatrix.h>
#include <AArray.h>
#include "A3DPhysActor.h"

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
class A3DSkeleton;
class APhysXActorDesc;
class APhysXShapeDesc;
class APhysXJointDesc;
class APhysXMaterialDesc;
class APhysXSkeletonRBObject;
class APhysXSkeletonRBObjectDesc;
class APhysXRBAttacherDesc;
class A3DPhysRBCreator;
class NxStream;
#ifdef _ANGELICA21
class SoftKeyFrameConfigDesc;
#endif
class SoftKeyFrameDescNotify;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class A3DModelPhysSyncData
//	
///////////////////////////////////////////////////////////////////////////

class A3DModelPhysSyncData
{
public:		//	Types

	//	Init bind error code
	enum
	{
		ERR_OK = 0,					//	No error
		ERR_NO_MEMORY,				//	Not enough memory
		ERR_NO_ROOTBONE,			//	Not find root bone in skin model
		ERR_NO_BONE,				//	Not find some bones in skin model
		ERR_BONE_LINKTWICE,			//	Some bone is linked to more than one actor
		ERR_ACTOR_NOTLINK,			//	Some actor isn't linked to bone
		ERR_BONE_NOTLINK,			//	Some bone isn't linked to actor
		ERR_JOINT_NOTLINK,			//	Some joint isn't linked to bone
		ERR_LENBONE_NOTCHILD,		//	Length bone isn't child of main bone
		ERR_MAINBONE_NOTJOINT,		//	Main bone isn't a joint bone
		ERR_ATTACHER_ACTOR_NOTEXIST,//	Attacher's binding actor is not exist
		ERR_ATTACHER_HOOK_NOTEXIST,	//	Attacher's corresponding hook is not exist
		ERR_ATTACHER_HOOK_HASNOBONE,//	Attacher's corresponding hook's parent bone does not exist
		ERR_ATTACHER_TYPE_UNKNOWN,	//	Attacher's type is unknown
		ERR_ACTOR_LACK_SHAPE,		//	Some Actor has zero shape
		ERR_COLLISION_GROUP_INVALID,//	Invalid collision group
		ERR_MAINBONE_FLIPPED,			// We have to make sure that mainbone's hand frame is correct!

		ERR_OTHERS,					//	All other error
	};

	//	Bone data
	struct BONE_DATA
	{
	protected:

		AString		strBone;			//	Bone's name
		int			iBoneIdx;			//	Index of bone. initialized in InitBind()
		A3DMATRIX4	matBone2Actor;		//	Transform matrix from bone space to actor space
		bool		bAnimMajor;			//	This flag means which state this bone will remain when whole model is doing
											//	partially physical simulation. in animation or physical state. True means
											//	bone will be driven by animation and false means it's driven by physical system
	public:

		friend class A3DModelPhysSync;
		friend class A3DModelPhysSyncData;

		//	Operations
		BONE_DATA();
		~BONE_DATA();
		//	Get bone name
		const char* GetBoneName() const { return strBone; }
		//	Set animation major flag
		void SetAnimMajorFlag(bool bTrue) { bAnimMajor = bTrue; }
		//	Get animation major flag
		bool GetAnimMajorFlag() const { return bAnimMajor; }
	};

	//	Actor and bone link info
	struct LINK_INFO
	{
	protected:

		AString		strMainBone;		//	Main bone's name
		int			iMainBone;			//	Main bone's index. initialized in InitBind()
		AString		strLenBone;			//	The bone that decides actor's length
		int			iLenBone;			//	Length bone's index. initialized in InitBind()
		A3DMATRIX4	matActor2Bone;		//	Transform matrix from actor space to main bone space
		
		APtrArray<BONE_DATA*>	aBones;	//	Data of bones that linked with this actor

	public:

		friend class A3DModelPhysSync;
		friend class A3DModelPhysSyncData;

		//	Operations
		LINK_INFO();
		LINK_INFO(const LINK_INFO& rhs);
		~LINK_INFO();
		//	Operator =
		LINK_INFO& operator = (const LINK_INFO& rhs);
		//	Has valid main bone ?
		bool HasMainBone() const { return strMainBone.GetLength() ? true : false; }
		//	Get main bone name
		const char* GetMainBone() const { return strMainBone; }
		//	Set main bone name
		void SetMainBone(const char* szName) { strMainBone = szName; }

		//	main bone index
		int GetMainBoneIndex() const { return iMainBone; }

		//	Get length bone name
		const char* GetLengthBone() const { return strLenBone; }
		//	Set length bone for actor that will decide actor's length
		void SetLengthBone(const char* szBone);
		//	Get number of bone that linked to actor
		int GetBoneNum() const { return aBones.GetSize(); }
		//	Get bone data
		BONE_DATA* GetBoneDataByIndex(int n) const { return aBones[n]; }
		BONE_DATA* GetBoneDataByName(const char* szBone) const;

		const A3DMATRIX4& GetMatActor2Bone() const { return matActor2Bone; }
		A3DMATRIX4& GetMatActor2Bone() { return matActor2Bone; }
	};

	struct JOINT_DATA;

	enum ActorDataFlag
	{
		ADF_EMBEDDABLE = (1<<0),
	};

	//	Actor data
	struct ACTOR_DATA : public A3DPhysActor
	{
	protected:

		LINK_INFO	Link;					//	Link info

		APtrArray<JOINT_DATA*>	aJoints;	//	Joint array

		bool	bUseDefaultMaterial;		// Shape use default material

		int					nFlags;

	public:

		friend class A3DModelPhysSync;
		friend class A3DModelPhysSyncData;

		//	Constructors / Destructor
		ACTOR_DATA();
		//	Notion: copy constructor will not copy Joints' pointers, for they may be invalid in a ACTOR_DATA copy when a joint is removed
		ACTOR_DATA(const ACTOR_DATA& rhs);
		virtual ~ACTOR_DATA();

		//	Operations
		//	Notion: operator = will not copy Joints' pointers, for they may be invalid pointers
		ACTOR_DATA& operator = (const ACTOR_DATA& rhs);
		virtual A3DPhysActor* Clone() const;
		bool IsLinked() const { return Link.HasMainBone(); }
		//	Get link info
		LINK_INFO& GetLinkInfo() { return Link; }
		//	Get const link info
		const LINK_INFO& GetLinkInfo() const { return Link; }

		//	Get joint number
		int GetJointNum() const { return aJoints.GetSize(); }
		//	Get joint data
		JOINT_DATA* GetJoint(int n) const { return aJoints[n]; }
		
		//	Set use default material (all shape use default material -> all shape share this state)
		void SetUseDefaultMaterial(bool bIsUseDefaultMat);
		//	Get use default material
		bool GetUseDefaultMaterial() const { return bUseDefaultMaterial; }

		bool ReadFlag(ActorDataFlag flag) const { return (nFlags & flag)? true : false; }
		void RaiseFlag(ActorDataFlag flag) { nFlags |= flag; }
		void ClearFlag(ActorDataFlag flag) { nFlags &= ~flag; }
	};

	//	Joint data
	enum JointDataFlag
	{
		JDF_BREAKABLE_EQUIPMENTS = (1<<0),
	};

	struct JOINT_DATA
	{
	protected:

		AString				strName;		//	Joint name
		APhysXJointDesc*	pJointDesc;		//	Joint desc
		ACTOR_DATA*			aActors[2];		//	Actors

		AString				strBone;		//	Name of bone that this joint is linked with
		int					iBoneIdx;		//	Index of bone. initialized in InitBind()

		A3DVECTOR3			vOffset;		//	Local offset from the linked bone.
		int					nFlags;

	public:

		friend class A3DModelPhysSync;
		friend class A3DModelPhysSyncData;

		//	Operations
		JOINT_DATA();
		~JOINT_DATA();

		//	Get joint name
		const char* GetName() const { return strName; }
		//	Get joint desc
		APhysXJointDesc* GetJointDesc() const { return pJointDesc; }
		//	Get actor
		ACTOR_DATA* GetActor(int n) const { return (n >= 0 && n < 2) ? aActors[n] : NULL; }
		//	Get linked bone's name
		const char* GetLinkedBoneName() const { return strBone; }
		//	Link joint to bone
		void LinkJointToBone(const char* szBone);
		//	Get offset from linked bone
		A3DVECTOR3 GetOffset() const { return vOffset; }
		//	Set offset from linked bone
		void SetOffset(const A3DVECTOR3& vOff) { vOffset = vOff; }

		bool ReadFlag(JointDataFlag flag) const { return (nFlags & flag)? true : false; }
		void RaiseFlag(JointDataFlag flag) { nFlags |= flag; }
		void ClearFlag(JointDataFlag flag) { nFlags &= ~flag; }
	};

	//	Attacher data
	struct ATTACHER_DATA
	{
	protected:

		AString				strAttacher;	//	Name of attacher
		AString				strActor;		//	Name of attach actor
		AString				strCCHook;		//	Name of attacher corresponding-to hook (bone)
		int					nAttacherType;	//	Connection the attached actor has with the attacher

		//	Not allow to copy the whole struct
		ATTACHER_DATA(const ATTACHER_DATA&);
		ATTACHER_DATA& operator = (const ATTACHER_DATA&);

	public:

		//	Operations
		ATTACHER_DATA(const char* szAttacher
			, const char* szActor
			, const char* szHook
			, int nType);
		~ATTACHER_DATA() {}

		//	Get attacher name
		const AString& GetAttacherName() const { return strAttacher; }
		//	Set attacher name
		void SetAttacherName(const char* szName);
		//	Get attach actor
		const AString& GetAttachActor() const { return strActor; }
		//	Set attach actor name
		void SetAttachActor(const char* szName) { strActor = szName; }
		//	Get attacher corresponding-to hook (bone)
		const AString& GetHook() const { return strCCHook; }
		//	Set attacher corresponding-to hook (bone)
		void SetHook(const char* szHook) { strCCHook = szHook; }
		//	Get attacher type
		int GetAttacherType() const { return nAttacherType; }
		//	Set attacher type
		void SetAttacherType(int nType) { nAttacherType = nType; }
		//	On attach actor name changed
		bool OnAttachActorNameChanged(const char* szOldName, const char* szNewName);
	};

	friend class A3DModelPhysSync;
	friend class A3DModelPhysics;

public:		//	Constructor and Destructor

	A3DModelPhysSyncData();
	virtual ~A3DModelPhysSyncData();

public:		//	Attributes

public:		//	Operations

	//	Load data from file
	bool Load(const char* szFile);
	//	Save data to file
	bool Save(const char* szFile);

	//	Initialize bind data
	bool InitBind(A3DSkinModel* pSkinModel, bool bForceReset=false);
	//	Check bind flag
	bool GetBindInitFlag() const { return m_bBindInit; }
	//	Get last bind error code
	int GetLastBindError() const { return m_iInitBindErr; }
	//	Get object that occurs error
	const char* GetErrorObject() const { return m_strErrObject; }
	//	Get attacher physical description
	APhysXRBAttacherDesc* GetAttacherPhysDesc() const { return m_pAttacherDesc; }
	//	Create attacher physical description by specified skeleton desc, currently only support one link
	APhysXRBAttacherDesc* CreateAttacherPhysDescBySkeletonDesc(APhysXSkeletonRBObjectDesc* pSkeletonRBDesc, A3DSkeleton* pSkeleton);

	//	Link actor and joint
	bool LinkActorAndJoint(const char* szActor, const char* szJoint, int iPos);
	//	Un-link actor and joint
	void UnlinkActorAndJoint(const char* szActor, const char* szJoint);
	void UnlinkActorAndJoint(const char* szJoint, int iPos);
	//	Link actor to a bone
	bool LinkActorToBone(const char* szActor, A3DSkinModel* pSkinModel, const char* szBone);
	//	Un-link actor from bone
	bool UnlinkActorFromBone(const char* szActor);
	//	Link bone to actor
	bool LinkBoneToActor(const char* szActor, A3DSkinModel* pSkinModel, const char* szBone);
	//	Un-link bone from actor
	bool UnlinkBoneFromActor(const char* szActor, const char* szBone);

	//	Create a new actor
	ACTOR_DATA* CreateActor(const char* szName, const A3DVECTOR3& vPos);
	//	Remove a actor
	void RemoveActor(const char* szName);
	//	Set actor's position and orientation
	void SetActorPose(const char* szActor, const A3DMATRIX4& matActor, A3DSkinModel* pSkinModel);
	//	Change actor name
	bool ChangeActorName(const char* szOldName, const char* szNewName);
	//	Get actor number
	int GetActorNum() const { return m_aActors.GetSize(); }
	//	Get actor
	ACTOR_DATA* GetActorByName(const char* szName, int* piIndex=NULL) const;
	ACTOR_DATA* GetActorByIndex(int n) const { return m_aActors[n]; }
	ACTOR_DATA* GetActorByMainBone(const char* szMainBone, int* piIndex = NULL) const;

	//	Get joint number
	int GetJointNum() const { return m_aJoints.GetSize(); }
	//	Get joint
	JOINT_DATA* GetJointByName(const char* szName, int* piIndex=NULL) const;
	JOINT_DATA* GetJointByIndex(int n) const { return m_aJoints[n]; }
	//	Create a new joint
	JOINT_DATA* CreateJoint(const char* szName, const APhysXJointDesc* pDesc);
	//	Remove a joint
	void RemoveJoint(const char* szName);
	//	Change joint's name
	bool ChangeJointName(const char* szOldName, const char* szNewName);

	//	Get attacher number
	int GetAttacherNum() const { return m_aAttachers.GetSize(); }
	//	Get attacher
	ATTACHER_DATA* GetAttacherByIndex(int n) const { return m_aAttachers[n]; }
	ATTACHER_DATA* GetAttacherByName(const char* szName, int* piIndex=NULL) const;
	//	Create attacher
	ATTACHER_DATA* CreateAttacher(const char* szName, const char* szActor, const char* szHook, int nAttacherType);
	//	Remove an attacher
	void RemoveAttacher(const char* szName);
	//	Change attacher 's name
	bool ChangeAttacherName(const char* szOldName, const char* szNewName);

#ifdef _ANGELICA21
	// Soft key frame related functions
	int GetSKFConfigDescNum() const { return m_aSKFConfigs.GetSize(); }

	SoftKeyFrameConfigDesc* GetSKFConfigDescByIndex(int n) const;
	SoftKeyFrameConfigDesc* GetSKFConfigDescByName(const char* szName, int* piIndex=NULL) const;
	SoftKeyFrameConfigDesc* CreateSKFConfigDesc(const char* szName);

	void RemoveSKFConfigDesc(const char* szName);
	bool ChangeSKFConfigDescName(const char* szOldName, const char* szNewName);
	void RegisterSKFNotify(SoftKeyFrameDescNotify* pNotify);
	void UnRegisterSKFNotify(SoftKeyFrameDescNotify* pNotify);
#endif
	//	Set RB Object type
	bool SetRBDescObjectType(unsigned int uType);
	//	Set a new RB desc creator
	bool SetRBDescObjectType(A3DPhysRBCreator* pNewCreator);
	A3DPhysRBCreator* GetPhysRBDescCreator() const { return m_pPhysRBDescCreator; }

	//	Set root bone name
	void SetRootBone(const char* szBone);
	//	Get root bone name
	const char* GetRootBone() const { return m_strRootBone; }

	//	Set collision group (See APhysXObjectDesc::mCollisionGroup and enum APhysXObjectFlag)
	void SetCollisionGroup(int iCollisionGroup);
	//	Get collision group
	int GetCollisionGroup() const { return m_iCollisionGroup; }

	//	Set model object desc's flag (See APhysXObjectDesc::mFlag and enum )
	void SetModelObjectFlag(unsigned int uFlag);
	//	Get model object desc's flag
	unsigned int GetModelObjectFlag() const { return m_uiObjectFlag; }

	//	Set modified flag
	void SetModifiedFlag() { m_bModified = true; }
	//	Get modified flag
	bool GetModifiedFlag() const { return m_bModified; }

protected:	//	Attributes

	AString		m_strRootBone;		//	Bone will be treated as model's root
	int			m_iRootBone;		//	Root bone index in skeleton. Initialized in InitBind()
	int			m_iRootActor;		//	The actor that root bone linked with. Initialized in InitBind()
	BONE_DATA*	m_pRootBoneData;	//	Root bone data, Initialized in InitBind()
	int			m_iCollisionGroup;	//	Phys object's collision group, modify by editor
	unsigned int m_uiObjectFlag;	//	Phys model object's flag

	bool		m_bBindInit;		//	true, bind data has been initialized
	int			m_iInitBindErr;		//	Last bind error code
	AString		m_strErrObject;		//	The object occurs error in InitBind() 
	int			m_iVersion;			//	File data version
	bool		m_bModified;		//	Data has been modified flag

	A3DSkinModel*				m_pSkinModel;
	A3DPhysRBCreator*			m_pPhysRBDescCreator;	//	Rigidbody desc creator

	APhysXSkeletonRBObjectDesc*	m_pModelPhysDesc;	//	Model's physical description
	APhysXRBAttacherDesc*		m_pAttacherDesc;	//	Model attacher's physical description	

	APtrArray<ACTOR_DATA*>		m_aActors;			//	Actor array
	APtrArray<JOINT_DATA*>		m_aJoints;			//	Joint array
	APtrArray<ATTACHER_DATA*>	m_aAttachers;		//	Attacher array
#ifdef _ANGELICA21
	APtrArray<SoftKeyFrameConfigDesc*>	m_aSKFConfigs;
#endif
	APtrArray<SoftKeyFrameDescNotify*>	m_aSKFNotification;

protected:	//	Operations

	//	Get model's physical description
	APhysXSkeletonRBObjectDesc* GetModelPhysDesc() const { return m_pModelPhysDesc; }
	//	Check all actor's shapes and modify necessary material state
	bool CheckAndUpdateMaterialState();
	//	Create Standard model physical description
	bool CreateStandardModelPhysDesc(A3DSkeleton* pSkeleton);
	//	Create Standard attacher physical description
	bool CreateStandardAttacherPhysDesc(A3DSkeleton* pSkeleton);
	//	Update link info.
	bool UpdateLinkInfo(A3DSkinModel* pSkinModel);
	//	Link actor and joint
	bool LinkActorAndJoint_Inner(ACTOR_DATA* pActor, JOINT_DATA* pJoint, int iPos);
	//	Un-link actor and joint
	void UnlinkActorAndJoint_Inner(JOINT_DATA* pJoint, int iPos);
	//	Check if specified bone is linked with some joint
	int GetBoneLinkWithJoint(int iBoneIdx);
	//	Check attacher info
	bool CheckAttacherInfo(A3DSkinModel* pSkinModel);
	//	Create Standard soft key frame config description that used to injury status
#ifdef _ANGELICA21
	void CreateStandardSKFDesc(int enumSKFConfigName = -1);
	void ClearAllSKFConfigDesc();
#endif
public:
	//	Write string to stream
	static bool WriteString(NxStream* ps, const AString& str);
	//	Read string from stream
	static bool ReadString(NxStream* ps, AString& str);
	//	Write A3DVECTOR to stream
	static bool WriteVector(NxStream* ps, const A3DVECTOR3& vec);
	//	Read A3DVECTOR from stream
	static bool ReadVector(NxStream* ps, A3DVECTOR3& vec);

protected:
	//	Write actor data to stream
	bool WriteActorData(NxStream* ps, ACTOR_DATA* pActor);
	//	Write link data to stream
	bool WriteLinkData(NxStream* ps, LINK_INFO& link);
	//	Write joint data to stream
	bool WriteJointData(NxStream* ps);
	//	Write attacher data to stream
	bool WriteAttacherData(NxStream* ps);
	//	Write RBCreator
	bool WriteRBCreator(NxStream* ps);
	//	Read actor data from stream
	bool ReadActorData(NxStream* ps, ACTOR_DATA* pActor);
	//	Read link data from stream
	bool ReadLinkData(NxStream* ps, LINK_INFO& link);
	//	Read joint data from stream
	bool ReadJointData(NxStream* ps, int iNumJoint);
	//	Read attacher data from stream
	bool ReadAttacherData(NxStream* ps);
	//	Read RBCreator
	bool ReadRBCreator(NxStream* ps);
	
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

#endif	//	_A3DMODELPHYSSYNCDATA_H_
