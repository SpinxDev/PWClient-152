 /*
 * FILE: A3DBone.h
 *
 * DESCRIPTION: This class standing for a bone in A3D Engine, it is actually the bone's pivot
 *		bone only record its parent bone pointer, no child bones can be seen from the bone
 *		object, the bone's hierarchy can only be fully observed in class A3DSkeleton
 *
 * CREATED BY: Hedi, 2003/6/30
 *
 * HISTORY:
 *
 * Copyright (c) 2003 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DBONE_H_
#define _A3DBONE_H_

#include "A3DTypes.h"
#include "AArray.h"
#include "A3DObject.h"
#include "A3DIKData.h"
#include "A3DIKSolver.h"

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

class A3DSkeleton;
class A3DJoint;
class A3DAnimJoint;
class A3DBoneController;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class A3DSkeleton
//
///////////////////////////////////////////////////////////////////////////

class A3DBone : public A3DObject
{
public:		//	Types

	//	Scale type
	enum
	{
		SCALE_NONE = 0,			//	No scale
		SCALE_WHOLE,			//	Scale will effect all children
		SCALE_LOCAL,			//	Scale only effect this bone	and affecting its children's offset
		SCALE_LOCAL_NOOFFSET,	//	Scale only effect this bone and without affecting its children's offset
	};

	//	Bone flags
	enum
	{
		FLAG_FAKEBONE	= 0x01,
		FLAG_FLIPPED	= 0x02,
	};

	//	State blend mode
	enum
	{
		BM_NORMAL = 0,
		BM_COMBINE,
		BM_EXCLUSIVE,
		BM_ABSTRACK,
	};

	//	Transition flag
	enum
	{
		TRANS_POS_X = 0x01,		//	Position X
		TRANS_POS_Y = 0x02,		//	Position Y
		TRANS_POS_Z = 0x04,		//	Position Z
		TRANS_POS = 0x07,		//	Position
		TRANS_ORI = 0x08,		//	Orientation
		TRANS_ALL = 0x0f,		//	All flags
	};

	//	Struct used to save and load bone
	struct BONEDATA
	{
		BYTE			byFlags;		//	Bone flags
		int				iParent;		//	Parent bone's index
		int				iFirstJoint;	//	First joint's index
		int				iNumChild;		//	Number of children
		A3DMATRIX4		matRelative;	//	the matrix bring vertex to parent space
		A3DMATRIX4		matBoneInit;	//	Initial bone offset matrix
	};

	//	A struct that will hold the state transition data for this bone.
	struct TRANSITION
	{
		bool			bDoTrans;		//	true, doing transition
		DWORD			dwTransMask;	//	Transition mask
		A3DVECTOR3		vStartPos;		//	position of start state
		A3DQUATERNION	quStartRot;		//	orientation of start state
		int				iAllTime;		//	total transition time
		int				iCurTime;		//	time elapse of transition
	};
	
	//	Blend state of bone
	struct STATE
	{
		A3DQUATERNION	quOrientation;	//	Blend orientation
		A3DVECTOR3		vPos;			//	Blend position
		float			fWeight;		//	Weight of matrix
		int				iBlendMode;		//	Bone blend mode
	};

public:		//	Constructor and Destructor

	A3DBone(); 
	~A3DBone();

	friend class A3DSkeleton;

public:		//	Attributes

public:		//	Operations

	//	Initialize bone
	bool Init(A3DSkeleton* pSkeleton);
	//	Release bone
	void Release();
	//	Duplicate bone data
	A3DBone* Duplicate(A3DSkeleton* pDstSkeleton);

	//	Load bone from file
	bool Load(AFile* pFile);
	//	Save bone from file
	bool Save(AFile* pFile);

	//	bool Update(int nDeltaTime)
	//		Update the bone's orietation to a specific animation frame
	//	nAnimIndex		IN			the specific animation frame index
	//	return true if success, or on fail return false;
	bool Update(int nDeltaTime);
	//	void CombineRelativeTM(const A3DMATRIX4& mat)
	//	apply a transform matrix to current relative tm
	//	mat			IN			the applied tm
	void CombineRelativeTM(const A3DMATRIX4& mat);

	///////////////////////////////////////////////////////////////////////////
	// Inline get and set interfaces.
	///////////////////////////////////////////////////////////////////////////
	virtual void SetName(const char* szName);

	DWORD GetBoneID() { return m_dwBoneID; }

	int GetParent()	{ return m_iParent; }
	void SetParent(int iParent) { m_iParent = iParent; }
	A3DBone* GetParentPtr();
	A3DBone* GetRootBonePtr();

	bool AddChild(int iChild);
	int GetChild(int iIndex) { return m_aChildren[iIndex]; }
	int GetChildNum() { return m_aChildren.GetSize(); }
	A3DBone* GetChildPtr(int iIndex);

	//	Get / Set bone flags
	void SetFakeFlag(bool bFake) { bFake ? m_byBoneFlags |= FLAG_FAKEBONE : m_byBoneFlags &= ~FLAG_FAKEBONE; }
	bool IsFakeBone() { return (m_byBoneFlags & FLAG_FAKEBONE) ? true : false; }
	void SetFlippedFlag(bool bFlip) { bFlip ? m_byBoneFlags |= FLAG_FLIPPED : m_byBoneFlags &= ~FLAG_FLIPPED; }
	bool IsFlipped() { return (m_byBoneFlags & FLAG_FLIPPED) ? true : false; }
	void SetBoneFlags(BYTE byFlags) { m_byBoneFlags = byFlags; }
	BYTE GetBoneFlags() { return m_byBoneFlags; }

	A3DSkeleton* GetSkeleton() { return m_pSkeleton; }
	void SetSkeleton(A3DSkeleton* pSkeleton) { m_pSkeleton = pSkeleton; }

	//	Get first joint's index
	int GetFirstJoint() { return m_iFirstJoint; }
	//	Set first joint's index
	 void SetFirstJoint(int iJoint) { m_iFirstJoint = iJoint; }
	//	Get first joint's address
	A3DJoint* GetFirstJointPtr();
	//	Add a joint
	bool AddJoint(int iJoint);
	//	Get animation joint associated with this bone
	A3DAnimJoint* GetAnimJointPtr();

	//	Get / Set initial bone offset matrix
	const A3DMATRIX4& GetBoneInitTM() { return m_matBoneInit; }
	void SetBoneInitTM(const A3DMATRIX4& mat) { m_matBoneInit = mat; }
	//	Get / Set bone's original relative matrix
	const A3DMATRIX4& GetOriginalMatrix() { return m_matOriginRel; }
	void SetOriginalMatrix(const A3DMATRIX4& mat) { m_matOriginRel = mat; }

	//	Add blend state
	void AddBlendState(const A3DQUATERNION& quOrientation, const A3DVECTOR3& vPos, float fWeight, int iMode);
	//	Get blend state number
	int GetBlendStateNum() { return m_aBlendStates.GetSize(); }
	//	Get blend state
	STATE& GetBlendState(int n) { return m_aBlendStates[n]; }

	//	Prepare to do transition
	void PrepareTransition(int iTransTime);
	//	Reset bone's relative TM
	void ResetRelativeTM() { SetRelativeTM(m_matOriginRel); }
	//	Set relative matrix directly
	void SetRelativeTM(const A3DMATRIX4& mat);
	//	Get / Set animation driven flag
	bool IsAnimDriven() { return m_bAnimDriven; }
	void SetAnimDriven(bool bAnimDriven);

	/*	----------------------------------------
		SetScaleType, GetScaleType, SetScaleFactor, GetScaleFactor are bypast bone scale interface,
		keep them only in order that old games can run normally.
	-------------------------------------------- */
	//	Set / Get scale type
	void SetScaleType(int iType);
	int GetScaleType() { return m_iScaleType; }
	//	Set / Get scale factor
	void SetScaleFactor(const A3DVECTOR3& vScale);
	const A3DVECTOR3& GetScaleFactor() { return m_vScaleFactor; }
	//	Get old-type accumulated whole scale factor
	const A3DVECTOR3& GetOldAccuWSF() { return m_vOldAccuWSF; }
	/*	----------------------------------------
		SetLocalScale, GetLocalLengthScale, GetLocalThickScale, SetWholeScale, GetWholeScale are
		new bone scale interface, try to use them now.
	-------------------------------------------- */
	//	Set / Get local scale factor
	void SetLocalScale(float fLenFactor, float fThickFactor);
	float GetLocalLengthScale() { return m_fLocalLenSF; }
	float GetLocalThickScale() { return m_fLocalThickSF; }
	//	Set / Get whole scale factor
	void SetWholeScale(float fFactor);
	float GetWholeScale() { return m_fWholeSF; }
	//	Get accumulated whole scale factor
	float GetAccuWholeScale() { return m_fAccuWholeSF; }

	//	Get absolute matrix without scale
	const A3DMATRIX4& GetNoScaleAbsTM() { return m_matNoScaleAbs; }
	//	Get up to root bone matrix
	const A3DMATRIX4& GetUpToRootTM() { return m_matUpToRoot; }
	//	Get relative tm's quaternion
	const A3DQUATERNION& GetRelativeQuat() { return m_quRelativeTM; }
	//	Get absolute TM
	const A3DMATRIX4& GetAbsoluteTM() { return m_matAbsoluteTM; }
	//	Get relative TM
	const A3DMATRIX4& GetRelativeTM() { return m_matRelativeTM; }
	//	Get local scale matrix
	const A3DMATRIX4& GetLocalScaleMatrix() { return m_matLocalScale; }
	//	Get scale matrix 
	const A3DMATRIX4& GetScaleMatrix() { return m_matScale; }
	// Get up to parent matrix
	const A3DMATRIX4& GetUpToParentTM() const { return m_matUpToParent; }

	// Transition mask operation
	bool IsPosXTransSet() { return (m_TransData.dwTransMask & TRANS_POS_X) ? true : false; }
	bool IsPosYTransSet() { return (m_TransData.dwTransMask & TRANS_POS_Y) ? true : false; }
	bool IsPosZTransSet() { return (m_TransData.dwTransMask & TRANS_POS_Z) ? true : false; }
	bool IsOriTransSet() { return (m_TransData.dwTransMask & TRANS_ORI) ? true : false; }
	DWORD GetTransMask() { return m_TransData.dwTransMask; }
	void SetTransMask(DWORD dwMask) { m_TransData.dwTransMask = dwMask; }

	A3DBoneController* GetFirstController() { return m_pFirstController; }
	void SetFirstController(A3DBoneController* pController) { m_pFirstController = pController; }

protected:	//	Attributes
	
	A3DSkeleton*		m_pSkeleton;			//	Skeleton this bone belongs to
	DWORD				m_dwBoneID;				//	Bone's ID
	int					m_iParent;				//	Parent bone's index
	int					m_iFirstJoint;			//	First joint's index
	AArray<short>		m_aChildren;			//	Children
	BYTE				m_byBoneFlags;			//	Bone flags
	bool				m_bAnimDriven;			//	true, this bone is driven by animation

	//	Bypast bone scale parameters	
	int					m_iScaleType;			//	Scale type
	A3DVECTOR3			m_vScaleFactor;			//	Scale factor on x, y and z axis
	A3DVECTOR3			m_vOldAccuWSF;			//	Accumulated whole scale factor for old-type scaling
	//	New bone scale parameters	
	float				m_fLocalLenSF;			//	Local length scale factor
	float				m_fLocalThickSF;		//	Local thickness factor
	float 				m_fWholeSF;				//	Whole scale factor
	float				m_fAccuWholeSF;			//	Accumulated whole scale factor

	A3DMATRIX4			m_matLocalScale;		//	Local scale matrix (scale caused by length and thick changes)
	A3DMATRIX4			m_matScale;				//	Total scale matrix effect this bone space

	A3DMATRIX4			m_matBoneInit;			//	Bone's initial matrix which transform skin mesh from 
													//	world space to local bone space
	A3DMATRIX4			m_matOriginRel;			//	Original relative matrix of bone

	A3DMATRIX4			m_matAbsoluteTM;		//	Absolute TM, from this bone to world
	A3DMATRIX4			m_matRelativeTM;		//	Relative TM, from this bone to parent
	A3DQUATERNION		m_quRelativeTM;			//	Current orientation
	A3DMATRIX4			m_matUpToRoot;			//	Matrix from bone space to root bone space
	A3DMATRIX4			m_matUpToParent;		//	Matrix from bone space to parent space

	A3DMATRIX4			m_matNoScaleAbs;		//	Absolute matrix without scale factor

	TRANSITION			m_TransData;			//	state transition data

	AArray<STATE>		m_aBlendStates;			//	Blend states   
	AArray<STATE>		m_aCombineStates;		//	Combine states

	A3DBoneController*	m_pFirstController;		//	Controller on this bone

protected:	//	Operations

	//	Make no sacle absolute matrix
	void NoScaleAbsoluteTM();
	//	Update bone state using absolute track if 
	bool UpdateByAbsTrack();
	//	Rebuild local scale matrix
	void RebuildLocalScaleMatrix();
	//	Build a scale matrix along specified axis
	void BuildAxisScaleMatrix(A3DMATRIX4& matOut, const A3DVECTOR3& vDir, float fLenScale, float fThickScale);
	//	Pass accumulated whole scale factor to child bone
	void PassAccuWholeScale();
	//	Set old-type scale. Added by dyx 2013.7.16
	void SetOldScaleType(int iType, const A3DVECTOR3& vFactor);

	//	Update bone state for making absolute track, used only by A3DSkeleton. 
	//	This function doesn't consider scale effects
	bool UpdateForMakingAbsTrack();
	//	Build bone's up-to-root matrix using original bone state
	void BuildOriginUpToRootMatrix(bool bUseScale, const A3DMATRIX4& matParentUpToRoot, 
				A3DMATRIX4& matOut, A3DMATRIX4& matOutUpToRoot);
	//	Build bone's up-to-root matrix using specified relative bone state
	void BuildFrameUpToRootMatrix(bool bUseScale, const A3DMATRIX4& matParentUpToRoot, 
				const A3DMATRIX4& matFrameRel, A3DMATRIX4& matOut, A3DMATRIX4& matOutUpToRoot);
};

#endif//_A3DBONE_H_
