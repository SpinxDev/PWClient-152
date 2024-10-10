/*
 * FILE: A3DSkinPhysSync.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2008/6/17
 *
 * HISTORY: 
 *
 * Copyright (c) 2008 Archosaur Studio, All Rights Reserved.
 */

#ifndef _A3DSKINPHYSSYNC_H_
#define _A3DSKINPHYSSYNC_H_

#include <AString.h>
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
class A3DSkinPhysSyncData;
class APhysXScene;
class APhysXClothAttacher;
class APhysXRigidBodyObject;
class APhysXClothAttacherDesc;

#ifdef _ANGELICA21
class A3DAdhereMesh;
#endif

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class A3DSkinPhysSync
//	
///////////////////////////////////////////////////////////////////////////

class A3DSkinPhysSync
{
public:		//	Types

	//	RB actor link info
	struct RBACTOR_LINK
	{
		int		iHookHHIdx;		//	hh-type hook's index
		bool	bHHIsBone;		//	hh-type hook is a bone
	};

	friend class A3DModelPhysics;

public:		//	Constructor and Destructor

	A3DSkinPhysSync();
	virtual ~A3DSkinPhysSync();

public:		//	Attributes

public:		//	Operations

	//	Bind physical data with specified skin
	bool Bind(APhysXScene* pPhysScene, A3DSkinModel* pSkinModel, A3DSkin* pSkin, const A3DVECTOR3& vRefPos, int iClothSimDelayTime, int iClothSimBlendTime);
	//	Un-bind physical data
	void Unbind(bool bDelCoreData=false);

	//	Update pose of RB actors using soft-link. Call this before physics simulation
	bool UpdateRBActorPose(A3DSkinModel* pSkinModel);

	// Update the skin by animation. This is to constrain Cloth simulation. Call this before physics simulation.
	bool UpdateSkinByAnimation(int iDt, A3DSkinModel* pSkinModel, bool bUpdateNormals = true);

	//	Sync skin mesh from physical data. Call this after physics simulation
	bool SyncSkinPhys(A3DSkin* pSkin);

	//	Check if some skin has been bound ?
	bool IsSkinBound() const { return m_pPhysSkin ? true : false; }

	//	Get sync data
	A3DSkinPhysSyncData* GetSyncData() const { return m_pCoreData; }
	//	Get cloth attacher object
	APhysXClothAttacher* GetClothAttacher() const { return m_pPhysSkin; }

	void SetClothWindAcceleration(const A3DVECTOR3& vWindAcceleration);
	void EnableClothSimulation(bool bEnable = true);

	void InitAnimVertInfo();

#ifdef _ANGELICA21
	// adhere mesh operation
	bool InitAdhereMesh();
	int GetAdhereMeshNum() const { return m_aAdhereMeshes.GetSize(); } 
#endif

protected:	//	Attributes

	A3DSkinPhysSyncData*		m_pCoreData;		//	Sync data
	APhysXScene*				m_pPhysScene;		//	Physical scene object
	APhysXClothAttacher*		m_pPhysSkin;		//	Skin's physical representation
	APhysXClothAttacherDesc*	m_pScaleClothDesc;	//	Scaled cloth's physical description
	A3DSkin*					m_pA3DSkin;			//	Bound skin object

	AArray<RBACTOR_LINK>		m_aRBActorLinks;	//	RB actor link info

	//---------------------------------------------------------
	// revised by Wenfeng, Dec. 13, 2012
	// in most cases, when we turn on the cloth simulation, there 
	// will be a little jittering before the cloth calms down. So,
	// we just blend the cloth simulation results by animation skinning.
	// 
	// Time Unit is ms
	
	int m_iClothSimDelayTime;		// how many time it takes to start blending simulation and animation after Bind() is called
	int	m_iClothSimBlendTime;		// how many time it takes to blend simulation and animation after the delay time is over

	int m_iClothSimBlendCounter;

	bool m_bIgnoreClothSimBlendCounter;		// if this flag is set to true, we'll ignore the Cloth Simulation Blend Counter 
											// and use m_fClothSimWeight directly...

	struct AnimVertInfo
	{
		A3DSkinMesh* mSkinMesh;
		A3DClothMeshImp* mClothMesh;

		bool mInvalid;

		A3DVECTOR3* mAnimPositions;
		A3DVECTOR3* mAnimNormals;

		//----------------------------------
		AnimVertInfo()
		{
			mSkinMesh = NULL;
			mClothMesh = NULL;

			mInvalid = true;

			mAnimPositions = NULL;
			mAnimNormals = NULL;
		}

		~AnimVertInfo() { Release(); }

		void Release()
		{
			if(mAnimPositions)
			{
				delete [] mAnimPositions;
				mAnimPositions = NULL;
			}

			if(mAnimNormals)
			{
				delete [] mAnimNormals;
				mAnimNormals = NULL;
			}
		}
	};

	APtrArray<AnimVertInfo*>	m_ArrAnimVertInfo;				// array of all cloth meshes' animation vertex information
	float m_fClothSimWeight;									// the blending weight between animation skin and cloth simulation
	
	

#ifdef _ANGELICA21
	APtrArray<A3DAdhereMesh*>      m_aAdhereMeshes;
	A3DSkinModel*                  m_pSkinModel;
#endif	

protected:	//	Operations

	//	Get specified RB actor
	APhysXRigidBodyObject* GetRBActor(int iActorIdx);
	//	Check if mesh needs to be scaled before it changes to cloth
	bool CheckMeshScaled(A3DSkinModel* pSkinModel);
	
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

#endif	//	_A3DSKINPHYSSYNC_H_


