/*
 * FILE: A3DSkeleton.h
 *
 * DESCRIPTION: A3D skeleton class
 *
 * CREATED BY: duyuxin, 2003/8/26
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DSKELETON_H_
#define _A3DSKELETON_H_

#include "A3DTypes.h"
#include "A3DObject.h"
#include "A3DCoordinate.h"
#include "AArray.h"
#include "AString.h"

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

class A3DBone;
class A3DJoint;
class A3DSkinModel;
class A3DSkeleton;
class A3DMATRIX4;
class AFile;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class A3DSkeletonHook
//
///////////////////////////////////////////////////////////////////////////

class A3DSkeletonHook : public A3DObject
{
public:		//	Types

	//	Hook type
	enum
	{
		HOOK_HH = 0,	//	Is hang point
		HOOK_CC,		//	Is connection center
	};

	//	Struct used to save and load hook
	struct HOOKDATA
	{
		DWORD		dwType;		//	Hook type
		int			iBone;		//	Index of bone on which the hook sticks to
		A3DMATRIX4	matHookTM;	//	Hook TM
	};

	friend class A3DSkinModel;

public:		//	Constructors and Destructors

	A3DSkeletonHook(A3DSkeleton* pSkeleton);
	virtual ~A3DSkeletonHook() {}
	
public:		//	Attributes

public:		//	Operations

	//	Duplicate hook data
	A3DSkeletonHook* Duplicate(A3DSkeleton* pDstSkeleton);

	//	Load model from file
	virtual bool Load(AFile* pFile);
	//	Save model from file
	virtual bool Save(AFile* pFile);

	//	Rebuild scaled hook TM, this function is called when parent bone's 
	//	local scale matrix changes.
	void RebuildScaledHookTM();
	//	Get scaled hook TM
	const A3DMATRIX4& GetScaledHookTM() { return m_matScaledHookTM; }

	//	Set / Get hook type
	void SetHookType(DWORD dwType) { m_dwType = dwType; }
	DWORD GetHookType() { return m_dwType; }
	//	Set / Get index of bone to which the hook attaches
	void SetBone(int iBone) { m_iBone = iBone; }
	int GetBone() { return m_iBone; }
	//	Set / Get hook TM
	void SetHookTM(const A3DMATRIX4& mat) { m_matHookTM = mat; }
	const A3DMATRIX4& GetHookTM() { return m_matHookTM; }
	//	Set / Get fix-dir flag
	void SetFixDirFlag(bool bFix);
	bool GetFixDirFlag() { return m_bFixDir; }

	//	Get currently absolute matrix from hook space to world space
	const A3DMATRIX4& GetAbsoluteTM();
	//	Get currently no-scaled absolute matrix from hook space to world space
	const A3DMATRIX4& GetNoScaleAbsTM();

protected:	//	Attributes

	A3DSkeleton*	m_pA3DSkeleton;		//	Skeleton object
	int				m_iBone;			//	Used by HOOK_HH, Index of bone on which the hook sticks to
	A3DMATRIX4		m_matHookTM;		//	Original hook TM, the matrix bring vertex to parent space
	A3DMATRIX4		m_matScaledHookTM;	//	Hook TM including parent bone's scale effect
	DWORD			m_dwType;			//	Hook type
	bool			m_bFixDir;			//	true, fix hook's direction

	//	Below members are used by HOOK_HH type hooks
	A3DMATRIX4		m_matAbs;			//	Currently absolute matrix from hook space to world space
	A3DMATRIX4		m_matNoScaleAbs;	//	Currently no-scaled absolute matrix from hook space to world space
	DWORD			m_dwUpdateCnt;		//	Update counter

protected:	//	Operations

	//	Update hook's state
	void Update(bool bForce);
};

///////////////////////////////////////////////////////////////////////////
//
//	Class A3DSkeleton
//
///////////////////////////////////////////////////////////////////////////

class A3DSkeleton : public A3DCoordinate
{
public:		//	Types

public:		//	Constructors and Destructors

	A3DSkeleton();
	A3DSkeleton(const A3DSkeleton& s);
	virtual ~A3DSkeleton();
	
	friend class A3DSkinMan;
	friend class A3DAbsTrackMaker;

public:		//	Attributes

public:		//	Operations

	//	Save skeleton data from file
	bool Save(AFile* pFile);
	bool Save(const char* szFile);

	//	Update frame
	bool Update(int iDeltaTime);

	//	Add a bone
	int AddBone(A3DBone* pBone);
	//	Get bone by name and index
	A3DBone* GetBone(const char* szName, int* piIndex) const;
	//	Get bone by index
	A3DBone* GetBone(int iIndex) const { return (iIndex >= 0 && iIndex < m_aBones.GetSize()) ? m_aBones[iIndex] : NULL; }
	//	Get bone number
	int GetBoneNum() { return m_aBones.GetSize(); }
	//	Get root bone number
	int GetRootBoneNum() const{ return m_aRootBones.GetSize(); }
	//	Get root bone by index
	int GetRootBone(int n) const { return (n >= 0 && n < m_aRootBones.GetSize()) ? m_aRootBones[n] : NULL; }
	//	Reset all bones' relative TM to original value
	void ResetBoneRelativeTM();
	//	Get the reference bone which be used to translate bound boxes or mesh centers
	int GetRefBone() const { return m_iRefBone; }
	//	Called when a bone's local scale matrix changes
	void OnBoneLocalScaleChanges(A3DBone* pBone);
	//	Called when a bone's whole scale matrix changes
	void OnBoneWholeScaleChanges();
	//	Get original bone state
	void GetOriginBoneState(AArray<A3DMATRIX4>& aMats, bool bUseScale=false);
	//	Get bone state at specified frame
	void GetFrameBoneState(AArray<A3DMATRIX4>& aRelMats, AArray<A3DMATRIX4>& aOutMats, bool bUseScale=false);

	//	Add a joint
	int AddJoint(A3DJoint* pJoint);
	//	Get joint by name and index
	A3DJoint* GetJoint(const char* szName, int* piIndex);
	//	Get joint by index
	A3DJoint* GetJoint(int iIndex) { return (iIndex >= 0 && iIndex < m_aJoints.GetSize()) ? m_aJoints[iIndex] : NULL; }
	//	Get index of joint in skeleton
	int GetJointIndex(A3DJoint* pJoint);
	//	Get joint number
	int GetJointNum() const { return m_aJoints.GetSize(); }
	//	Get approximate animation size
	int GetAnimDataSize();
 
	//	Add hook
	int AddHook(A3DSkeletonHook* pHook);
	//	Get hook by name and index
	A3DSkeletonHook* GetHook(const char* szName, int* piIndex);
	//	Get hook by index
	A3DSkeletonHook* GetHook(int iIndex) { return (iIndex >= 0 && iIndex < m_aHooks.GetSize()) ? m_aHooks[iIndex] : NULL; }
	//	Get hook number
	int GetHookNum() const { return m_aHooks.GetSize(); }

	//	Get file name
	const char* GetFileName() const { return m_strFileName; }
	//	Get skeleton ID
	DWORD GetSkeletonID() const { return m_dwSkeletonID; }
	//	Get update counter
	DWORD GetUpdateCounter() const { return m_dwUpdateCnt; }
	//	Get animation FPS
	int GetAnimFPS() const { return m_iAnimFPS; }
	//	Set animation FPS
	void SetAnimFPS(int iFPS) { m_iAnimFPS = iFPS; }
	//	Get version
	DWORD GetVersion() { return m_dwVersion; }

	//	Get inherit whole scale factor
	float GetInheritScale() { return m_fInheritScale; }
	//	Set inherit whole scale factor
	void SetInheritScale(float fScale);

	//	Automatically calculate foot offset caused by bone scaling
	void CalcFootOffset(const char* szFootBone=NULL);
	//	Set foot offset purposely
	void SetFootOffset(float fOffset) { m_fFootOffset = fOffset; }
	//	Get Foot offset on Y axis caused by bone scaling
	float GetFootOffset() { return m_fFootOffset; }
	//	Get foot bone index
	int GetFootBone() { return m_iFootBone; }
	
	//	Set skin model pointer
	void SetA3DSkinModel(A3DSkinModel* pModel) { m_pSkinModel = pModel; }
	//	Get skin model pointer
	A3DSkinModel* GetA3DSkinModel() const { return m_pSkinModel; }

	//	Set / Get skin man flag
	void SetSkinManFlag(int iFlag) { m_iSkinManFlag = iFlag; }
	int GetSkinManFlag() const { return m_iSkinManFlag; }

protected:	//	Attributes

	A3DSkinModel*	m_pSkinModel;	//	Skin model pointer

	AString		m_strFileName;		//	Skeleton file name
	DWORD		m_dwSkeletonID;		//	Skeleton ID
	DWORD		m_dwUpdateCnt;		//	Update counter
	int			m_iRefBone;			//	Reference bone
	int			m_iSkinManFlag;		//	Flag used by A3DSkinMan
	DWORD		m_dwVersion;		//	Loaded file version
	int			m_iAnimFPS;			//	Animation FPS
	float		m_fFootOffset;		//	Foot offset on Y axis caused by bone scaling
	bool		m_bTrackSetOK;		//	true, track set has been loaded. 
	int			m_iFootBone;		//	Index of foot bone
	float		m_fInheritScale;	//	Inherit whole scale factor

	APtrArray<A3DBone*>			m_aBones;		//	Bones
	APtrArray<A3DJoint*>		m_aJoints;		//	Joints
	APtrArray<A3DSkeletonHook*>	m_aHooks;		//	Hooks
	AArray<int, int>			m_aRootBones;	//	Root bones

protected:	//	Operations

	//	A3DSkeleton should be created and released through A3DSkinMan, so hide it's
	//	initial and release functions here.
	bool Init();
	void Release();
	//	Clone skeleton
	A3DSkeleton* Clone();
	//	Load skeleton data form file
	bool Load(AFile* pFile);
	bool Load(const char* szFile);

	//	Update bone tree
	bool UpdateBoneTree(A3DBone* pRootBone, int iDeltaTime);
	//	Find a reference bone which be used to translate bound boxes or mesh centers
	bool FindRefBone();
	//	Get original bone state
	void GetOriginBoneState_r(int iBoneIdx, int iParent, bool bUseScale, AArray<A3DMATRIX4>& aMats, 
			AArray<A3DMATRIX4>& aUpToRootMats);
	//	Get bone state of specified frame
	void GetFrameBoneState_r(int iBoneIdx, int iParent, bool bUseScale, AArray<A3DMATRIX4>& aRelMats, 
			AArray<A3DMATRIX4>& aOutMats, AArray<A3DMATRIX4>& aUpToRootMats);

	//	Update skeleton state for making absolute track, used only by A3DAbsTrackMaker. 
	//	This function doesn't consider scale effects
	bool UpdateForMakingAbsTrack();
	bool UpdateBoneTreeForMakingAbsTrack(A3DBone* pRootBone);
};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////



#endif	//	_A3DSKELETON_H_

