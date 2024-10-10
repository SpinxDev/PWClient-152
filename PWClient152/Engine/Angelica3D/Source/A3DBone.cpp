/*
 * FILE: A3DBone.cpp
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

#include "A3DBone.h"
#include "A3DFuncs.h"
#include "A3DPI.h"
#include "A3DJoint.h"
#include "A3DSkeleton.h"
#include "A3DBoneController.h"
#include "AMemory.h"
#include "A3DConfig.h"

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
//	Implement A3DBone
//
///////////////////////////////////////////////////////////////////////////

A3DBone::A3DBone() : 
m_vScaleFactor(1.0f),
m_vOldAccuWSF(1.0f),
m_matAbsoluteTM(A3DMATRIX4::IDENTITY),
m_matRelativeTM(A3DMATRIX4::IDENTITY),
m_matLocalScale(A3DMATRIX4::IDENTITY),
m_matScale(A3DMATRIX4::IDENTITY)
{
	m_dwClassID			= A3D_CID_BONE;
	m_iParent			= -1;
	m_iFirstJoint		= -1;
	m_pSkeleton			= NULL;
	m_dwBoneID			= 0;
	m_byBoneFlags		= 0;
	m_iScaleType		= SCALE_NONE;
	m_fLocalLenSF		= 1.0f;
	m_fLocalThickSF		= 1.0f;
	m_fWholeSF			= 1.0f;
	m_fAccuWholeSF		= 1.0f;
	m_bAnimDriven		= true;
	m_pFirstController	= NULL;

	m_TransData.bDoTrans = false;
	m_TransData.dwTransMask = TRANS_ALL;
}

A3DBone::~A3DBone()
{
	m_iParent		= -1;
	m_iFirstJoint	= -1;
}

//	Initialize bone
bool A3DBone::Init(A3DSkeleton* pSkeleton)
{
	m_pSkeleton = pSkeleton;
	return true;
}

//	Release bone
void A3DBone::Release()
{
	m_pSkeleton	= NULL;
}

//	Duplicate bone data
A3DBone* A3DBone::Duplicate(A3DSkeleton* pDstSkeleton)
{
	A3DBone* pb = new A3DBone;
	if (!pb)
	{
		g_A3DErrLog.Log("A3DBone::Duplicate, Not enougth memory !");
		return NULL;
	}

	if (!pb->Init(pDstSkeleton))
	{
		delete pb;
		g_A3DErrLog.Log("A3DBone::Duplicate, Failed to init bone object");
		return NULL;
	}

	//	Set bone name
	pb->SetName(m_strName);

	pb->m_byBoneFlags	= m_byBoneFlags;
	pb->m_iFirstJoint	= m_iFirstJoint;
	pb->m_iParent		= m_iParent;
	pb->m_matBoneInit	= m_matBoneInit;
	pb->m_matOriginRel	= m_matOriginRel;

	pb->ResetRelativeTM();

	//	Copy children
	pb->m_aChildren.SetSize(m_aChildren.GetSize(), 4);
	for (int i=0; i < m_aChildren.GetSize(); i++)
		pb->m_aChildren[i] = m_aChildren[i];
	
	return pb;
}

void A3DBone::SetName(const char* szName)
{ 
	m_strName	= szName;
	m_dwBoneID	= a_MakeIDFromString(szName);
}

//	Animation control interface.
bool A3DBone::Update(int nDeltaTime)
{
	//	Check if driven by absolute track
	if (UpdateByAbsTrack())
		return true;

	bool bNewRelative = false;
	A3DQUATERNION quCurOrient;
	A3DVECTOR3 vCurPos;

	if (m_aBlendStates.GetSize())
	{
		//	Blend all states.
		int i, iNumState = m_aBlendStates.GetSize();
		float fInvTotal = 0.0f;

		//	Blend all state
		for (i=0; i < iNumState; i++)
			fInvTotal += m_aBlendStates[i].fWeight;

		if (fInvTotal)
		{
			if (iNumState == 1)
			{
				quCurOrient	= m_aBlendStates[0].quOrientation;
				vCurPos = m_aBlendStates[0].vPos;
			}
			else
			{
				quCurOrient.Clear();
				vCurPos.Clear();

				fInvTotal = 1.0f / fInvTotal;

				for (i=0; i < iNumState; i++)
				{
					const STATE& State = m_aBlendStates[i];
					float fWeight = State.fWeight * fInvTotal;
					if (i && DotProduct(quCurOrient, State.quOrientation) < 0.0f)
						fWeight = -fWeight;

					quCurOrient = quCurOrient + State.quOrientation * fWeight;
					vCurPos += State.vPos * fWeight;
				}
			}

			// combined states can not be used without blend states
			if( m_aCombineStates.GetSize() )
			{
				iNumState = m_aCombineStates.GetSize();
				for(i=0; i<iNumState; i++)
				{
					const STATE& State = m_aCombineStates[i];
					float fWeight = State.fWeight;

					// for combined states, weight is not used for orientation, only for position
					quCurOrient = quCurOrient * State.quOrientation;
					vCurPos += State.vPos * fWeight;
				//	a_Clamp(fWeight, 0.0f, 1.0f);
				//	quCurOrient = quCurOrient * (1.0f - fWeight) + State.quOrientation * fWeight;
				//	vCurPos = vCurPos * (1.0f - fWeight) + State.vPos * fWeight;
				}
			}

			bNewRelative = true;
		}

		//	Remove all states
		m_aBlendStates.RemoveAll(false);
		m_aCombineStates.RemoveAll(false);
	}

	//	Do transition between current state and new blended state
	if (m_TransData.bDoTrans && m_TransData.dwTransMask && bNewRelative)
	{
		if (m_TransData.iAllTime <= m_TransData.iCurTime + nDeltaTime)
		{
			m_TransData.bDoTrans = false;
		}
		else
		{
			m_TransData.iCurTime += nDeltaTime;
			float f = (float)m_TransData.iCurTime / m_TransData.iAllTime;

			// simulate a acceleration
			f = (float)pow(f, 1.5f);

			A3DVECTOR3 vNewPos = vCurPos * f + m_TransData.vStartPos * (1.0f - f);
			A3DQUATERNION quNewOrient = SLERPQuad(m_TransData.quStartRot, quCurOrient, f);

			if (IsPosXTransSet())
				vCurPos.x = vNewPos.x;
			if (IsPosYTransSet())
				vCurPos.y = vNewPos.y;
			if (IsPosZTransSet())
				vCurPos.z = vNewPos.z;
			if (IsOriTransSet())
				quCurOrient = quNewOrient;
		}
	}

	if (bNewRelative)
	{
		quCurOrient.Normalize();
		quCurOrient.ConvertToMatrix(m_matRelativeTM);
		m_quRelativeTM = quCurOrient;

		if (IsFlipped())
		{
			m_matRelativeTM._11 = -m_matRelativeTM._11;
			m_matRelativeTM._12 = -m_matRelativeTM._12;
			m_matRelativeTM._13 = -m_matRelativeTM._13;
		}

		m_matRelativeTM.SetRow(3, vCurPos);
	}

	// let the bone controller take effects.
	A3DMATRIX4 matControlled = IdentityMatrix();
	if (m_pFirstController)
		m_pFirstController->Update(nDeltaTime, this, matControlled);
	
	m_matUpToParent = m_matRelativeTM * matControlled;
	A3DBone* pParent = m_iParent >= 0 ? m_pSkeleton->GetBone(m_iParent) : NULL;

	if (g_pA3DConfig->GetFlagNewBoneScale())
	{
		//	Update the transform matrix for using.
		if (!pParent)
		{
			m_matUpToRoot = m_matUpToParent;
		}
		else if (!m_bAnimDriven)
		{
			//	If this bone is driven by physical system and it's linked with a joint,
			//	it's offset should have been calculated outside.
			m_matUpToRoot = m_matUpToParent * pParent->GetUpToRootTM();
		}
		else
		{
			float fLocalLenSF = m_fLocalLenSF * pParent->GetAccuWholeScale();

			if (fLocalLenSF != 1.0f)
			{
				//	Only scale child bone's offset
				m_matUpToParent._41 *= fLocalLenSF;
				m_matUpToParent._42 *= fLocalLenSF;
				m_matUpToParent._43 *= fLocalLenSF;
			}

			m_matUpToRoot = m_matUpToParent * pParent->GetUpToRootTM();
		}

		m_matAbsoluteTM = m_matScale * m_matUpToRoot * m_pSkeleton->GetAbsoluteTM();
	}
	else	//	Bypast bone scale
	{
		if (pParent)
		{
			A3DVECTOR3 vScale = pParent->GetOldAccuWSF();
			if (pParent->GetScaleType() == SCALE_LOCAL)
			{
				const A3DVECTOR3& v = pParent->GetScaleFactor();
				vScale.x *= v.x;
				vScale.y *= v.y;
				vScale.z *= v.z;
			}

			//	Scale child bone's offset
			m_matUpToParent._41 *= vScale.x;
			m_matUpToParent._42 *= vScale.y;
			m_matUpToParent._43 *= vScale.z;

			m_matUpToRoot = m_matUpToParent * pParent->GetUpToRootTM();
		}
		else
			m_matUpToRoot = m_matUpToParent;

		m_matAbsoluteTM = m_matScale * m_matUpToRoot * m_pSkeleton->GetAbsoluteTM();
	}

	if (m_bAnimDriven)
		m_matAbsoluteTM._42 -= m_pSkeleton->GetFootOffset();

	//	Make no sacle absolute matrix
	NoScaleAbsoluteTM();

	return true;
}

//	Make no sacle absolute matrix
void A3DBone::NoScaleAbsoluteTM()
{
	m_matNoScaleAbs = m_matUpToRoot * m_pSkeleton->GetAbsoluteTM();

	if (m_bAnimDriven)
		m_matNoScaleAbs._42 -= m_pSkeleton->GetFootOffset();
}

//	Update bone state using absolute track if 
bool A3DBone::UpdateByAbsTrack()
{
	if (!m_aBlendStates.GetSize())
		return false;

	const STATE& st = m_aBlendStates[0];
	if (st.iBlendMode != BM_ABSTRACK)
		return false;

	st.quOrientation.ConvertToMatrix(m_matUpToRoot);
	m_matUpToRoot.SetRow(3, st.vPos);
	m_matAbsoluteTM = m_matUpToRoot * m_pSkeleton->GetAbsoluteTM();
	m_matAbsoluteTM._42 -= m_pSkeleton->GetFootOffset();

	m_matNoScaleAbs = m_matAbsoluteTM;

	//	Remove all states
	m_aBlendStates.RemoveAll(false);
	m_aCombineStates.RemoveAll(false);

	//	We must update bone's relative infos, because they will be used when bone state
	//	translates from this action to next non-abstrack action.
	A3DBone* pParent = m_iParent >= 0 ? m_pSkeleton->GetBone(m_iParent) : NULL;
	if (pParent)
	{
		A3DMATRIX4 mat;
		a3d_InverseTM(pParent->GetUpToRootTM(), &mat);
		m_matRelativeTM = m_matUpToRoot * mat;
	}
	else
		m_matRelativeTM = m_matUpToRoot;

	m_matUpToParent = m_matRelativeTM;
	m_quRelativeTM.ConvertFromMatrix(m_matRelativeTM);

	return true;
}

//	Update bone state for making absolute track, used only by A3DSkeleton. 
//	This function doesn't consider scale effects
bool A3DBone::UpdateForMakingAbsTrack()
{
	bool bNewRelative = false;
	A3DQUATERNION quCurOrient;
	A3DVECTOR3 vCurPos;

	if (m_aBlendStates.GetSize())
	{
		//	Blend all states.
		int i, iNumState = m_aBlendStates.GetSize();
		float fInvTotal = 0.0f;

		//	Blend all state
		for (i=0; i < iNumState; i++)
			fInvTotal += m_aBlendStates[i].fWeight;

		if (fInvTotal)
		{
			if (iNumState == 1)
			{
				quCurOrient	= m_aBlendStates[0].quOrientation;
				vCurPos = m_aBlendStates[0].vPos;
			}
			else
			{
				quCurOrient.Clear();
				vCurPos.Clear();

				fInvTotal = 1.0f / fInvTotal;

				for (i=0; i < iNumState; i++)
				{
					const STATE& State = m_aBlendStates[i];
					float fWeight = State.fWeight * fInvTotal;
					if (i && DotProduct(quCurOrient, State.quOrientation) < 0.0f)
						fWeight = -fWeight;

					quCurOrient = quCurOrient + State.quOrientation * fWeight;
					vCurPos += State.vPos * fWeight;
				}
			}

			bNewRelative = true;
		}

		//	Remove all states
		m_aBlendStates.RemoveAll(false);
		m_aCombineStates.RemoveAll(false);
	}

	if (bNewRelative)
	{
		quCurOrient.Normalize();
		quCurOrient.ConvertToMatrix(m_matRelativeTM);
		m_quRelativeTM = quCurOrient;

		if (IsFlipped())
		{
			m_matRelativeTM._11 = -m_matRelativeTM._11;
			m_matRelativeTM._12 = -m_matRelativeTM._12;
			m_matRelativeTM._13 = -m_matRelativeTM._13;
		}

		m_matRelativeTM.SetRow(3, vCurPos);
	}

	m_matUpToParent = m_matRelativeTM;
	A3DBone* pParent = m_iParent >= 0 ? m_pSkeleton->GetBone(m_iParent) : NULL;

	if (pParent)
		m_matUpToRoot = m_matUpToParent * pParent->GetUpToRootTM();
	else
		m_matUpToRoot = m_matUpToParent;

//	m_matAbsoluteTM = m_matUpToRoot * m_pSkeleton->GetAbsoluteTM();
//	m_matAbsoluteTM._42 -= m_pSkeleton->GetFootOffset();

	return true;
}

void A3DBone::CombineRelativeTM(const A3DMATRIX4& mat)
{
	m_matRelativeTM = m_matRelativeTM * mat;
	m_quRelativeTM.ConvertFromMatrix(m_matRelativeTM);
}

A3DBone* A3DBone::GetParentPtr()
{
	return m_iParent < 0 ? NULL : m_pSkeleton->GetBone(m_iParent);
}

A3DBone* A3DBone::GetRootBonePtr()
{
	if (m_iParent < 0)
		return this;

	A3DBone* pParent = GetParentPtr();
	while (pParent)
	{
		A3DBone* pRoot = pParent->GetParentPtr();
		if (!pRoot)
			return pParent;

		pParent = pRoot;
	}

	//	Shouldn't come here
	return NULL;
}

A3DBone* A3DBone::GetChildPtr(int iIndex)
{
	return m_pSkeleton->GetBone(GetChild(iIndex));
}

A3DJoint* A3DBone::GetFirstJointPtr()
{
	return m_iFirstJoint < 0 ? NULL : m_pSkeleton->GetJoint(m_iFirstJoint);
}

bool A3DBone::AddChild(int iChild)
{ 
	m_aChildren.Add((short&)iChild); 
	return true;
}

//	Add a joint to bone's joint list
bool A3DBone::AddJoint(int iJoint)
{
	if (m_iFirstJoint < 0)
	{
		m_iFirstJoint = iJoint;
		return true;
	}

	A3DJoint* pJoint = GetFirstJointPtr();
	ASSERT(pJoint);

	while (pJoint->GetSiblingJointPtr())
		pJoint = pJoint->GetSiblingJointPtr();
	
	pJoint->SetSiblingJoint(iJoint);

	return true;
}

//	Get animation joint associated with this bone
A3DAnimJoint* A3DBone::GetAnimJointPtr()
{
	A3DJoint* pJoint = GetFirstJointPtr();
	while (pJoint)
	{
		if (pJoint->GetClassID() == A3D_CID_ANIMJOINT)
			return (A3DAnimJoint*)pJoint;

		pJoint = pJoint->GetSiblingJointPtr();
	}

	return NULL;
}

bool A3DBone::Load(AFile* pFile)
{
	//	Load bone name
	if (!pFile->ReadString(m_strName))
	{
		g_A3DErrLog.Log("A3DBone::Load, Failed to load bone name!");
		return false;
	}

	m_dwBoneID = a_MakeIDFromString(m_strName);

	//	Load bone data
	DWORD dwRead;
	BONEDATA Data;

	if (!pFile->Read(&Data, sizeof (Data), &dwRead) || dwRead != sizeof (Data))
	{
		g_A3DErrLog.Log("A3DBone::Load, Failed to load bone data!");
		return false;
	}

	m_byBoneFlags	= Data.byFlags;
	m_iFirstJoint	= Data.iFirstJoint;
	m_iParent		= Data.iParent;
	m_matBoneInit	= Data.matBoneInit;
	m_matOriginRel	= Data.matRelative;

	ResetRelativeTM();

	//	Load children
	m_aChildren.SetSize(Data.iNumChild, 4);
	for (int i=0; i < Data.iNumChild; i++)
	{
		int iChild;
		pFile->Read(&iChild, sizeof (int), &dwRead);
		m_aChildren[i] = (short)iChild;
	}

	return true;
}

bool A3DBone::Save(AFile* pFile)
{
	//	Write bone name
	if (!pFile->WriteString(m_strName))
	{
		g_A3DErrLog.Log("A3DBone::Save, Failed to write bone name!");
		return false;
	}

	//	Fill bone data
	BONEDATA Data;
	memset(&Data, 0, sizeof (Data));

	Data.byFlags		= m_byBoneFlags;
	Data.iFirstJoint	= m_iFirstJoint;
	Data.iParent		= m_iParent;
	Data.iNumChild		= m_aChildren.GetSize();
	Data.matRelative	= m_matOriginRel;
	Data.matBoneInit	= m_matBoneInit;

	//	Write bone data
	DWORD dwWrite;
	if (!pFile->Write(&Data, sizeof (Data), &dwWrite) || dwWrite != sizeof (Data))
	{
		g_A3DErrLog.Log("A3DBone::Save, Failed to write bone data!");
		return false;
	}

	//	Write children
	for (int i=0; i < m_aChildren.GetSize(); i++)
	{
		int iChild = m_aChildren[i];
		pFile->Write(&iChild, sizeof (int), &dwWrite);
	}

	return true;
}

//	Add blend state
void A3DBone::AddBlendState(const A3DQUATERNION& quOrientation, const A3DVECTOR3& vPos,
						float fWeight, int iMode)
{
	STATE State;
	State.quOrientation = quOrientation;
	State.vPos			= vPos;
	State.fWeight		= fWeight;
	State.iBlendMode	= iMode;

	if (iMode == BM_EXCLUSIVE || iMode == BM_ABSTRACK)
	{
		//	Remove old states
		m_aBlendStates.RemoveAll(false);
		m_aBlendStates.Add(State);
	}
	else if (iMode == BM_COMBINE)
	{
		m_aCombineStates.Add(State);
	}
	else if (iMode == BM_NORMAL)
	{
		//	If exclusive blend state has been added, ignore this one
		if (m_aBlendStates.GetSize() == 1 &&
			(m_aBlendStates[0].iBlendMode == BM_EXCLUSIVE ||
			 m_aBlendStates[0].iBlendMode == BM_EXCLUSIVE))
			return;

		m_aBlendStates.Add(State);
	}
}

//	Prepare to do transition
void A3DBone::PrepareTransition(int iTransTime)
{
	ASSERT(iTransTime >= 0);

	if (!iTransTime)
	{
		m_TransData.bDoTrans = false;
		return;
	}

	A3DVECTOR3 vStartPos = m_matRelativeTM.GetRow(3);
	A3DQUATERNION quStartRot;

	if (IsFlipped())
	{
		A3DMATRIX4 mat = m_matRelativeTM;
		mat._11 = -mat._11;
		mat._12 = -mat._12;
		mat._13 = -mat._13;
		quStartRot.ConvertFromMatrix(mat);
	}
	else
		quStartRot.ConvertFromMatrix(m_matRelativeTM);

	// see if now is just during a previous transition, if so, we must adjust start data to avoid jerk
	if (m_TransData.bDoTrans && m_TransData.dwTransMask)
	{
		float f = 1.0f * m_TransData.iCurTime / m_TransData.iAllTime;

		A3DVECTOR3 vNewPos = vStartPos * f + m_TransData.vStartPos * (1.0f - f);
		A3DQUATERNION quNewOrient = SLERPQuad(m_TransData.quStartRot, quStartRot, f);

		if (IsPosXTransSet())
			vStartPos.x = vNewPos.x;
		if (IsPosYTransSet())
			vStartPos.y = vNewPos.y;
		if (IsPosZTransSet())
			vStartPos.z = vNewPos.z;
		if (IsOriTransSet())
			quStartRot = quNewOrient;
	}

	m_TransData.bDoTrans	= true;
	m_TransData.iAllTime	= iTransTime;
	m_TransData.iCurTime	= 0;
	m_TransData.vStartPos	= vStartPos;
	m_TransData.quStartRot	= quStartRot;
}

//	Set relative matrix directly
void A3DBone::SetRelativeTM(const A3DMATRIX4& mat)
{
	m_matRelativeTM = mat;

	if (IsFlipped())
	{
		A3DMATRIX4 matTemp = m_matRelativeTM;
		matTemp._11 = -matTemp._11;
		matTemp._12 = -matTemp._12;
		matTemp._13 = -matTemp._13;
		m_quRelativeTM.ConvertFromMatrix(matTemp);
	}
	else
		m_quRelativeTM.ConvertFromMatrix(m_matRelativeTM);
}

//	Set / Get scale type
void A3DBone::SetScaleType(int iType) 
{
	ASSERT(!g_pA3DConfig->GetFlagNewBoneScale());
	if (m_iScaleType != iType)
		SetOldScaleType(iType, m_vScaleFactor);
}

//	Set / Get scale factor
void A3DBone::SetScaleFactor(const A3DVECTOR3& vScale)
{
	ASSERT(!g_pA3DConfig->GetFlagNewBoneScale());
	SetOldScaleType(m_iScaleType, vScale);
}

//	Set old-type scale. Added by dyx 2013.7.16
void A3DBone::SetOldScaleType(int iType, const A3DVECTOR3& vFactor)
{
	if (iType == SCALE_NONE)
	{
		m_matLocalScale.Identity();
	}
	else if (iType == SCALE_WHOLE)
	{
		m_matLocalScale.Identity();
	}
	else if (iType == SCALE_LOCAL || iType == SCALE_LOCAL_NOOFFSET)
	{
		m_matLocalScale = a3d_Scaling(vFactor.x, vFactor.y, vFactor.z);
	}
	else
	{
		ASSERT(0);
		return;
	}

	m_iScaleType = iType;
	m_vScaleFactor = vFactor;

	//	Update accumulated whole scale factor and pass it to all children
	PassAccuWholeScale();

	//	Notify skeleton to update all hooks' state
	m_pSkeleton->OnBoneWholeScaleChanges();
}

void A3DBone::SetWholeScale(float fFactor)
{
	ASSERT(g_pA3DConfig->GetFlagNewBoneScale());

	if (m_fWholeSF == fFactor)
		return;

	m_fWholeSF = fFactor;

	//	Update accumulated whole scale factor and pass it to all children
	PassAccuWholeScale();
	
	//	Notify skeleton to update all hooks' state
	m_pSkeleton->OnBoneWholeScaleChanges();
}

//	Pass accumulated whole scale factor to child bone
void A3DBone::PassAccuWholeScale()
{
	if (g_pA3DConfig->GetFlagNewBoneScale())
	{
		//	Update accumulated whole scale factor
		A3DBone* pParent = m_iParent >= 0 ? m_pSkeleton->GetBone(m_iParent) : NULL;
		if (pParent)
			m_fAccuWholeSF = m_fWholeSF * pParent->GetAccuWholeScale();
		else
			m_fAccuWholeSF = m_fWholeSF * m_pSkeleton->GetInheritScale();

		//	Update scale matrix
		m_matScale = a3d_Scaling(m_matLocalScale, m_fAccuWholeSF, m_fAccuWholeSF, m_fAccuWholeSF);

		int i;

		//	Pass whole scale factor to all children
		for (i=0; i < m_aChildren.GetSize(); i++)
		{
			A3DBone* pBone = m_pSkeleton->GetBone(m_aChildren[i]);
			if (pBone)
				pBone->PassAccuWholeScale();
		}
	}
	else
	{
		A3DVECTOR3 vOurWholeScale(1.0f);
		if (m_iScaleType == SCALE_WHOLE)
			vOurWholeScale = m_vScaleFactor;

		//	Update accumulated whole scale factor
		A3DBone* pParent = m_iParent >= 0 ? m_pSkeleton->GetBone(m_iParent) : NULL;
		if (pParent)
		{
			const A3DVECTOR3& s = pParent->GetOldAccuWSF();
			m_vOldAccuWSF.x = vOurWholeScale.x * s.x;
			m_vOldAccuWSF.y = vOurWholeScale.y * s.y;
			m_vOldAccuWSF.z = vOurWholeScale.z * s.z;
		}
		else
			m_vOldAccuWSF = vOurWholeScale * m_pSkeleton->GetInheritScale();

		//	Update scale matrix
		m_matScale = a3d_Scaling(m_matLocalScale, m_vOldAccuWSF.x, m_vOldAccuWSF.y, m_vOldAccuWSF.z);

		int i;

		//	Pass whole scale factor to all children
		for (i=0; i < m_aChildren.GetSize(); i++)
		{
			A3DBone* pBone = m_pSkeleton->GetBone(m_aChildren[i]);
			if (pBone)
				pBone->PassAccuWholeScale();
		}
	}
}

/*	Set / Get local scale factor.

	fLenFactor: length factor. 0 means keeping current value.
	fThickFactor: thickness factor. 0 means keeping current value.
*/	
void A3DBone::SetLocalScale(float fLenFactor, float fThickFactor)
{
	ASSERT(g_pA3DConfig->GetFlagNewBoneScale());

	//	Must have parent bone
	A3DBone* pParent = GetParentPtr();
	if (!pParent)
		return;

	if (fLenFactor != 0.0f)
		m_fLocalLenSF = fLenFactor;

	if (fThickFactor != 0.0f)
		m_fLocalThickSF = fThickFactor;

	pParent->RebuildLocalScaleMatrix();

	//	If this is a leaf bone, apply thickness scale to itself.
	if (!GetChildNum() && fThickFactor != 0.0f)
	{
		//	Transform child - parent direction into child space
		A3DVECTOR3 vPos = m_matOriginRel.GetRow(3);
		A3DVECTOR3 vDir;
		vDir.x = DotProduct(vPos, m_matOriginRel.GetRow(0));
		vDir.y = DotProduct(vPos, m_matOriginRel.GetRow(1));
		vDir.z = DotProduct(vPos, m_matOriginRel.GetRow(2));
		
		BuildAxisScaleMatrix(m_matLocalScale, vDir, 1.0f, m_fLocalThickSF);

		//	Update scale matrix with considing whole scale factor
		m_matScale = a3d_Scaling(m_matLocalScale, m_fAccuWholeSF, m_fAccuWholeSF, m_fAccuWholeSF);

		//	Notify skeleton to update all hooks' state which are 
		//	bound to this bone
		m_pSkeleton->OnBoneLocalScaleChanges(this);
	}
}

//	Rebuild local scale matrix
void A3DBone::RebuildLocalScaleMatrix()
{
	int i, iNumChild = GetChildNum();
	A3DMATRIX4 mat;

	m_matLocalScale.Identity();

	for (i=0; i < iNumChild; i++)
	{
		A3DBone* pChild = GetChildPtr(i);
		float f1 = pChild->m_fLocalLenSF;
		float f2 = pChild->m_fLocalThickSF;
		if (f1 != 1.0f || f2 != 1.0f)
		{
			BuildAxisScaleMatrix(mat, pChild->m_matOriginRel.GetRow(3), f1, f2);
			m_matLocalScale *= mat;
		}
	}

	//	Update scale matrix with considing whole scale factor
	m_matScale = a3d_Scaling(m_matLocalScale, m_fAccuWholeSF, m_fAccuWholeSF, m_fAccuWholeSF);

	//	Notify skeleton to update all hooks' state which are 
	//	bound to this bone
	m_pSkeleton->OnBoneLocalScaleChanges(this);
}

/*	Build a scale matrix along specified axis

	matOut: matrix used to store result.
	vDir: axis direction, can be non-normalized
	fLenScale: length scale.
	fThickScale: thickness scale
*/
void A3DBone::BuildAxisScaleMatrix(A3DMATRIX4& matOut, const A3DVECTOR3& vDir, 
							float fLenScale, float fThickScale)
{
	matOut.Identity();

	if (fLenScale == 1.0f && fThickScale == 1.0f)
		return;

	A3DMATRIX4 matView = a3d_LookAtMatrix(A3D::g_vOrigin, vDir, A3D::g_vAxisY, 0.0f);

	//	v1 is the eye direction, also is the length direction
	A3DVECTOR3 v1 = matView.GetCol(2);
	A3DVECTOR3 v2 = matView.GetCol(1);
	A3DVECTOR3 v3 = matView.GetCol(0);

	if (fLenScale != 1.0f)
		matOut = a3d_ScaleAlongAxis(v1, fLenScale);

	if (fThickScale != 1.0f)
	{
		A3DMATRIX4 mat1 = a3d_ScaleAlongAxis(v2, fThickScale);
		A3DMATRIX4 mat2 = a3d_ScaleAlongAxis(v3, fThickScale);
		matOut *= mat1 * mat2;
	}
}

/*	Build bone's up-to-root matrix using original bone state

	bUseScale: true, consider bone scaling
	matParent: parent bone's up-to-root matrix
	matOut (out): used to store result
*/
void A3DBone::BuildOriginUpToRootMatrix(bool bUseScale, const A3DMATRIX4& matParentUpToRoot, 
								A3DMATRIX4& matOut, A3DMATRIX4& matOutUpToRoot)
{
	BuildFrameUpToRootMatrix(bUseScale, matParentUpToRoot, m_matOriginRel, matOut, matOutUpToRoot);
}

/*	Build bone's up-to-root matrix using specified relative bone state
  
	bUseScale: true, consider bone scaling
	matParent: parent bone's up-to-root matrix
	matFrameRel: bone's frame relative matrix
	matOut (out): used to store result
*/
void A3DBone::BuildFrameUpToRootMatrix(bool bUseScale, const A3DMATRIX4& matParentUpToRoot, 
								const A3DMATRIX4& matFrameRel, A3DMATRIX4& matOut, A3DMATRIX4& matOutUpToRoot)
{
	A3DMATRIX4 matRel = matFrameRel;

	if (IsFlipped())
	{
		matRel._11 = -matRel._11;
		matRel._12 = -matRel._12;
		matRel._13 = -matRel._13;
	}

	A3DBone* pParent = m_iParent >= 0 ? m_pSkeleton->GetBone(m_iParent) : NULL;

	//	Update the transform matrix for using.
	if (pParent)
	{
		if (bUseScale)
		{
			float fLocalLenSF = m_fLocalLenSF * pParent->GetAccuWholeScale();

			if (fLocalLenSF != 1.0f)
			{
				//	Only scale child bone's offset
				matRel._41 *= fLocalLenSF;
				matRel._42 *= fLocalLenSF;
				matRel._43 *= fLocalLenSF;
			}
		}

		matOut = matRel * matParentUpToRoot;
	}
	else
		matOut = matRel;

	matOutUpToRoot = matOut;

	if (bUseScale)
		matOut = m_matScale * matOut;
	else
		matOut = matOutUpToRoot;
}

void A3DBone::SetAnimDriven(bool bAnimDriven)
{
	if (m_bAnimDriven == bAnimDriven)
		return;

	m_bAnimDriven = bAnimDriven;

	//	Restore distance between this bone and his parent to original state
	if (m_bAnimDriven && m_iParent >= 0)
	{
		float fOriginLen = m_matOriginRel.GetRow(3).Magnitude();

		A3DVECTOR3 vOffset = m_matRelativeTM.GetRow(3);
		vOffset.Normalize();
		vOffset *= fOriginLen;

		m_matRelativeTM.SetRow(3, vOffset);
		m_quRelativeTM.ConvertFromMatrix(m_matRelativeTM);
	}
}


