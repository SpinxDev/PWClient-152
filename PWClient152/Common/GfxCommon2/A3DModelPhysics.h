/*
 * FILE: A3DModelPhysics.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2008/6/25
 *
 * HISTORY: 
 *
 * Copyright (c) 2008 Archosaur Studio, All Rights Reserved.
 */

#ifndef _A3DMODELPHYSICS_H_
#define _A3DMODELPHYSICS_H_

#include <AArray.h>
#include <A3DMatrix.h>

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
class A3DSkin;
class A3DSkinPhysSync;
class A3DSkinPhysSyncData;
class A3DModelPhysSync;
class A3DModelPhysSyncData;
class APhysXScene;
class APhysXObjectConnector;
struct PhysRBCreatorInfo;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class A3DModelPhysics
//	
///////////////////////////////////////////////////////////////////////////

class A3DModelPhysics
{
public:		//	Types

	//	Skin slot
	struct SKIN_SLOT
	{
		A3DSkinPhysSync*	pPhysSync;		//	Skin physical sync
		bool				bSoftLink;		//	true, soft link flag

		AArray<APhysXObjectConnector*>	m_aHLCons;	//	Hard-link connectors

		SKIN_SLOT()
		{
			pPhysSync	= NULL;
			bSoftLink	= false;
		}
	};

	struct EMBED_ACTOR
	{
		EMBED_ACTOR()
		{
			bToEmbedAction = false;
			nbDelayFrame = 0;
			fHalfExtentX = 0.0f;
			apxForceInfo.mActor = 0;
			apxForceInfo.mForce.set(0, 0, 0);
			apxForceInfo.mPos.set(0, 0, 0);
			apxForceInfo.mForceFlag = APX_FF_ATPOS | APX_FF_LOCALPOS;
			apxForceInfo.mForceMode = NX_FORCE;
		}

		bool				bToEmbedAction;
		int					nbDelayFrame;
		float				fHalfExtentX;
		APhysXForceInfo		apxForceInfo;
	};


public:		//	Constructor and Destructor

	A3DModelPhysics();
	virtual ~A3DModelPhysics();

public:		//	Attributes

public:		//	Operations

	//	Initialize object
	bool Init(APhysXScene* pPhysScene, A3DSkinModel* pSkinModel);
	bool Init(APhysXScene* pPhysScene, CECModel* pECModel);
	//	Release object
	void Release();
	//	Create model sync
	bool CreateModelSync(A3DModelPhysSyncData* pSyncData);

	//	Update routine before physics simulation
	bool UpdateBeforePhysXSim(DWORD dwDeltaTime);
	//	Sync routine after physics simulation
	bool SyncAfterPhysXSim();

	//	Add a new skin
	bool AddSkinFile(const char* szSkinFile);
	//	Add a skin
	bool AddSkin(A3DSkin* pSkin, bool bAutoRem=true);
	//	Replace a skin
	bool ReplaceSkinFile(int iSlot, const char* szSkinFile, bool bAutoRem=true);
	//	Replace a skin
	bool ReplaceSkin(int iSlot, A3DSkin* pSkin, bool bAutoRem=true);
	//	Remove a skin item, this operation release both skin and skin item
	void RemoveSkinItem(int iSlot);
	//	Release all skins
	void ReleaseAllSkins();

	//----------------------------------------------------------
	// cloth operations...

	//	Change specified skin to cloth.
	//	iLinkType: 0 - default type; 1: soft link; 2: hard link
	bool OpenClothSkin(int iSlot, int iLinkType=0, int iClothSimDelayTime = 500, int iClothSimBlendTime = 800);
	//	Close physical sync for specified skin
	void CloseClothSkin(int iSlot, bool bCompletely=false);
	//	Check if skin is cloth or not
	bool IsClothSkin(int iSlot) const;
	//	Get skin physical sync object
	A3DSkinPhysSync* GetSkinPhysSync(int iSlot) const;
	//	Get link type of specified cloth skin. 0-not link; 1-soft link; 2-hard link
	int GetClothLinkType(int iSlot) const;

	void EnableClothSimulation(bool bEnable = true);

	//	Create phys link between parent model and child model
	//	link info contains parent's hook name and child's attacher name
	//	if child model(current model) is in phys simulate state, we update the attachers by parent's hooks
	//	else if the child model is in animation state, we update the child model by parent's hooks using HH and CC hook as usual
	bool OpenPhysLinkAsChildModel(const char* szParentHook, const char* szChildAttacher) const;
	void ClosePhysLinkAsChildModel() const;
	bool AddPhysLinkAsChildModel(const char* szParentHook, const char* szChildAttacher) const;
	bool RemovePhysLinkAsChildModel(const char* szParentHook, const char* szChildAttacher) const;
	bool ChangePhysLinkAsChildModel(const char* szChildAttacher, const char* szNewHook) const;

	//	Set model collision channel
	//	iMethod: COLLISION_CHANNEL_INDEPENDENT - As indenpend collision channel
	//			 COLLISION_CHANNEL_AS_PARENT - As same as parent (if exist parent)
	//			 COLLISION_CHANNEL_AS_COMMON - As common (collision with all objects)
	bool SetModelCollisionChannel(int iMethod, A3DModelPhysics* pParentPhysics);

	//	Get Collision Channel ID
	unsigned int GetCollisionChannel() { return m_idCollisionCh; }

	//	Get phys rb creator information including breakable info, ...
	bool GetRBCreatorInfo(PhysRBCreatorInfo* pInfo);

	//	Breakable related interface
	//	Break Joint
	void BreakJoint(const char* szJointName);
	//	Break Joint By Bone
	void BreakJointByBone(const char* szBoneName);
	//	Break all equipments(split decorative RBs from body RBs)
	//  The break action will NOT considering the "GetPhysState" flag and "GetRBSimMode" flag
	//  fMaxLinearVel		: the max linear velocity limitation on broken equipment
	//  fMaxAngularVelRad	: the max angular velocity limitation on broken equipment
	//						  Unit: radian  Default value: 3 * 360 deg
	//  nbDelayFrame		: the postpone frames after call this function
	void BreakEquipments(float fMaxLinearVel = 10, float fMaxAngularVelRad = 18.85, unsigned int nbDelayFrames = 3);
	//	Apply force
	bool AddForce(const A3DVECTOR3& vStart, const A3DVECTOR3& vDir, float fForceMagnitude, float fMaxDist, int iPhysForceType, float fSweepRadius = 0.0f, float fMaxVelChangeHint = FLT_MAX);
	//	End of Breakable related interface

	EMBED_ACTOR* GetEmbeddableActor(NxActor* pActor) const;
	void ClearEmbeddableActor(const EMBED_ACTOR* pEmbed);
	void ReleaseAllEmbeddableActors();

	//	Change physical state
	bool ChangePhysState(int iState, int iRBSimMode = -1);
	//	Get current physical state
	int GetPhysState() const;
	//	Get model mass
	float GetModelMass(bool bOnlyBodyMass = false) const;
	//	After model is teleport to a new position, this function should be could to sync physical data
	bool Teleport();

	void OnRender();

	//	Get interface
	A3DModelPhysSync* GetModelSync() const { return m_pModelSync; }
	A3DModelPhysSyncData* GetModelSyncData() const { return m_pModelSyncData; }
	CECModel* GetECModel() const { return m_pECModel; }

	void UpdateWindEffectedRBs() {	m_bIsRBValid = false; }

	//////////////////////////////////////////////////////////////
	//
	//	Below methods should be called by editor only !!
	//
	//////////////////////////////////////////////////////////////

	//	Begin editing specified cloth skin. 
	A3DSkinPhysSyncData* BeginEditClothSkin(int iSlot, const AArray<int>& aRigidMeshIdx, const AArray<int>& aSkinMeshIdx);
	//	End editing specified cloth skin. 
	bool EndEditClothSkin(A3DSkinPhysSyncData* pSkinSyncData);
	//	Create an empty model sync. 
	bool CreateEmptyModelSync();
	//	Release model sync. 
	void ReleaseModelSync();
	//	Get skin slot number
	int GetSkinSlotNum() const { return m_aSkinSlots.GetSize(); }
	//	Get skin slot
	SKIN_SLOT* GetSkinSlot(int n) { return m_aSkinSlots[n]; }

protected:	//	Attributes

	APhysXScene*			m_pPhysScene;		//	Physical scene object
	CECModel*				m_pECModel;			//	Bound EC_Model
	A3DSkinModel*			m_pSkinModel;		//	Bound skin model
	A3DModelPhysSync*		m_pModelSync;		//	Model (skeleton) sync
	A3DModelPhysSyncData*	m_pModelSyncData;	//	Model (skeleton) sync

	APtrArray<SKIN_SLOT*>	m_aSkinSlots;		//	Skin slots

	// bool					m_bCollisionCh;		//	true, use collision channel
	unsigned int			m_idCollisionCh;	//	Collision channel id
	unsigned int			m_uiMajorRBCollisionCh;		//	Collision channel of the major RB...

	int						m_nbDelayFrames;
	float					m_fMaxLinearVel;
	float					m_fMaxAngularVel;
	APtrArray<EMBED_ACTOR*>	m_aEmbeddable;

	APhysXForceInfo			m_WindInfo;
	bool					m_bIsClothValid;
	bool					m_bIsRBValid;
	APtrArray<NxActor*>		m_aActors;
	A3DVECTOR3				m_curClothAcc;

protected:	//	Operations

	//	Release all skin slots
	void ReleaseAllSkinPhys();
	//	Bind physical sync for specified skin
	bool BindSkinPhys(int iSlot, int iClothSimDelayTime, int iClothSimBlendTime);
	//	Release physical sync for specified skin
	void ReleaseSkinPhys(int iSlot, bool bRemSlot=false);

	//	Change specified skin to cloth. Soft-Link version
	bool OpenClothSkin_Inner(int iSlot, int iClothSimDelayTime, int iClothSimBlendTime);
	//	Change specified skin to cloth. Soft-Link version
	bool OpenClothSkin_SL(int iSlot);
	//	Change specified skin to cloth. Hard-Link version
	bool OpenClothSkin_HL(int iSlot);

	//	Set collision channel for all physical skins
	void SetCollisionChForClothes(unsigned int idChannel);
	//-------------------------------------------------------------------------------
	// obsolete and do not use it...
	//	Enable / Disable collision channel
	// void EnableCollisionChannel(bool bEnable);
	
	//-------------------------------------------------------------------------------
	// you should make sure that uCollisionChannel is meaningful and valid...
	void SetCollisionChannel(unsigned int uCollisionChannel);			
	//-------------------------------------------------------------------------------

	bool IsRootBone(const A3DSkeleton& skl, const AString& strName);
	void TickWindEffectOnRBs(DWORD dwDeltaTime);
	void SetClothWindAcceleration(const A3DVECTOR3& vWindAcceleration);
	void UpdateWindEffectedRBsImp();

private:
	void TickForBreakAndEmbed(DWORD dwDeltaTime);
	void ExecuteBreakEquipments();
	void ExecuteEmbedAction();
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

#endif	//	_A3DMODELPHYSICS_H_
