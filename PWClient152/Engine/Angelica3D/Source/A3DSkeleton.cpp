 /*
 * FILE: A3DSkeleton.cpp
 *
 * DESCRIPTION: A3D skeleton class
 *
 * CREATED BY: duyuxin, 2003/8/26
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "A3DPI.h"
#include "A3DSkeleton.h"
#include "A3DBone.h"
#include "A3DJoint.h"
#include "A3DFuncs.h"
#include "A3DEngine.h"
#include "A3DSkinModel.h"
#include "A3DTrackMan.h"
#include "A3DConfig.h"
#include "AMemory.h"
#include "AF.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

#define new A_DEBUG_NEW

//	Identity and version of skeleton file
#define	SKEFILE_IDENTIFY	(('A'<<24) | ('S'<<16) | ('K'<<8) | 'E')
#define SKEFILE_VERSION		6

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

#pragma pack (push, 1)

//	Skeleton file header
struct A3DSKELETONFILEHEADER
{
	DWORD	dwFlags;		//	File's identity, SKEFILE_IDENTIFY
	DWORD	dwVersion;		//	File's version, SKEFILE_VERSION
	int		iNumBone;		//	Number of bones
	int		iNumJoint;		//	Number of joint
	int		iNumHook;		//	Number of hooks
	int		iAnimStart;		//	Animation start frame
	int		iAnimEnd;		//	Animation end frame
	int		iAnimFPS;		//	Animation FPS
	BYTE	aReserved[64];	//	Reserved
};

#pragma pack (pop)

///////////////////////////////////////////////////////////////////////////
//
//	Local functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Implement A3DSkeletonHook
//
///////////////////////////////////////////////////////////////////////////

A3DSkeletonHook::A3DSkeletonHook(A3DSkeleton* pSkeleton) :
m_matScaledHookTM(A3DMATRIX4::IDENTITY),
m_matHookTM(A3DMATRIX4::IDENTITY)
{
	m_dwClassID		= A3D_CID_SKELETONHOOK;
	m_pA3DSkeleton	= pSkeleton;
	m_iBone			= -1;
	m_dwUpdateCnt	= 0;
	m_bFixDir		= 0;
}

//	Duplicate hook data
A3DSkeletonHook* A3DSkeletonHook::Duplicate(A3DSkeleton* pDstSkeleton)
{
	A3DSkeletonHook* ph = new A3DSkeletonHook(pDstSkeleton);
	if (!ph)
	{
		g_A3DErrLog.Log("A3DSkeletonHook::Duplicate, Not enough memory");
		return NULL;
	}

	//	Set hook name
	ph->SetName(m_strName);

	ph->m_iBone				= m_iBone;
	ph->m_matHookTM			= m_matHookTM;
	ph->m_matScaledHookTM	= m_matHookTM;	//	Don't copy scale effect
	ph->m_dwType			= m_dwType;

	return ph;
}

//	Load model from file
bool A3DSkeletonHook::Load(AFile* pFile)
{
	//	Read hook name
	pFile->ReadString(m_strName);

	//	Read hook data
	HOOKDATA HookData;
	DWORD dwRead;

	if (!pFile->Read(&HookData, sizeof (HookData), &dwRead) || dwRead != sizeof (HookData))
	{
		g_A3DErrLog.Log("A3DSkeletonHook::Load, Failed to read hook data");
		return false;
	}

	m_iBone				= HookData.iBone;
	m_matHookTM			= HookData.matHookTM;
	m_dwType			= HookData.dwType;
	m_matScaledHookTM	= m_matHookTM;

	return true;
}

//	Save model from file
bool A3DSkeletonHook::Save(AFile* pFile)
{
	//	Write action name
	pFile->WriteString(m_strName);

	HOOKDATA HookData;
	memset(&HookData, 0, sizeof (HookData));

	HookData.iBone		= m_iBone;
	HookData.matHookTM	= m_matHookTM;
	HookData.dwType		= m_dwType;

	DWORD dwWrite;
	if (!pFile->Write(&HookData, sizeof (HookData), &dwWrite) ||
		dwWrite != sizeof (HookData))
	{
		g_A3DErrLog.Log("A3DSkeletonHook::Save, Failed to save hook data");
		return false;
	}

	return true;
}

//	Update hook's state
void A3DSkeletonHook::Update(bool bForce)
{
	ASSERT(m_pA3DSkeleton);

	if (m_iBone < 0)
	{
		m_matAbs = m_matScaledHookTM * m_pA3DSkeleton->GetAbsoluteTM();
		m_matNoScaleAbs = m_matAbs;
	}
	else if (bForce || m_dwUpdateCnt != m_pA3DSkeleton->GetUpdateCounter())
	{
		m_dwUpdateCnt = m_pA3DSkeleton->GetUpdateCounter();
		
		A3DBone* pBone = m_pA3DSkeleton->GetBone(m_iBone);
		ASSERT(pBone);
	
		A3DMATRIX4 matWholeScale;
		if (g_pA3DConfig->GetFlagNewBoneScale())
		{
			float fWholeScale = pBone->GetAccuWholeScale();
			matWholeScale.Scale(fWholeScale, fWholeScale, fWholeScale);
		}
		else
		{
			const A3DVECTOR3& vAccuWSF = pBone->GetOldAccuWSF();
			matWholeScale.Scale(vAccuWSF.x, vAccuWSF.y, vAccuWSF.z);
		}

		m_matNoScaleAbs = m_matScaledHookTM * pBone->GetNoScaleAbsTM();
		m_matAbs = matWholeScale * m_matNoScaleAbs;

		if (m_bFixDir)
		{
			//	Fix direction to skeleton's direction
			A3DVECTOR3 v = m_matAbs.GetRow(3);
			m_matAbs = m_pA3DSkeleton->GetAbsoluteTM();
			m_matAbs.SetRow(3, v);

			v = m_matNoScaleAbs.GetRow(3);
			m_matNoScaleAbs = m_pA3DSkeleton->GetAbsoluteTM();
			m_matNoScaleAbs.SetRow(3, v);
		}
	}
}

//	Get currently absolute matrix from hook space to world space
const A3DMATRIX4& A3DSkeletonHook::GetAbsoluteTM()
{
	Update(false);
	return m_matAbs;
}

//	Get currently no-scaled absolute matrix from hook space to world space
const A3DMATRIX4& A3DSkeletonHook::GetNoScaleAbsTM()
{
	Update(false);
	return m_matNoScaleAbs;
}

//	Set / Get fix-dir flag
void A3DSkeletonHook::SetFixDirFlag(bool bFix)
{
	m_bFixDir = bFix;
	Update(true);
}

//	Rebuild scaled hook TM, this function is called when parent bone's 
//	local scale matrix changes.
void A3DSkeletonHook::RebuildScaledHookTM()
{
	if (!m_pA3DSkeleton || m_iBone < 0)
		return;

	A3DBone* pBone = m_pA3DSkeleton->GetBone(m_iBone);
	ASSERT(pBone);

	//	Bone's local scale matrix only effect hook's position
	A3DVECTOR3 vPos = m_matHookTM.GetRow(3);
	vPos = vPos * pBone->GetScaleMatrix();
	m_matScaledHookTM = m_matHookTM;
	m_matScaledHookTM.SetRow(3, vPos);
}

///////////////////////////////////////////////////////////////////////////
//
//	Implement A3DSkeleton
//
///////////////////////////////////////////////////////////////////////////

A3DSkeleton::A3DSkeleton()
{
	m_dwClassID		= A3D_CID_SKELETON;
	m_dwSkeletonID	= 0;
	m_pSkinModel	= NULL;
	m_dwUpdateCnt	= 0;
	m_iRefBone		= -1;
	m_iSkinManFlag	= -1;
	m_iAnimFPS		= 15;
	m_dwVersion		= SKEFILE_VERSION;
	m_fFootOffset	= 0.0f;
	m_bTrackSetOK	= false;
	m_iFootBone		= -1;
	m_fInheritScale	= 1.0f;
}

A3DSkeleton::~A3DSkeleton()
{
}

A3DSkeleton::A3DSkeleton(const A3DSkeleton& s)
{
	m_dwClassID		= A3D_CID_SKELETON;
	m_pSkinModel	= NULL;
	m_iSkinManFlag	= -1;
	m_dwUpdateCnt	= 0;
	m_fFootOffset	= 0.0f;
	m_bTrackSetOK	= false;
	m_fInheritScale	= 1.0f;

	m_dwVersion		= s.m_dwVersion;
	m_iRefBone		= s.m_iRefBone;
	m_iAnimFPS		= s.m_iAnimFPS;
	m_iFootBone		= s.m_iFootBone;

	//	Here we don't use s.m_strFileName directly mainly to avoid thread safe
	//	problems, after all AString isn't thread safty
	m_strFileName	= s.GetFileName();
	m_dwSkeletonID	= s.m_dwSkeletonID;

	int i;

	//	Clone bones
	for (i=0; i < s.m_aBones.GetSize(); i++)
	{
		A3DBone* pBone = s.m_aBones[i]->Duplicate(this);
		if (!pBone)
		{
			g_A3DErrLog.Log("A3DSkeleton::A3DSkeleton, Failed to clone bones");
			return;
		}

		AddBone(pBone);
	}

	//	Clone joints
	for (i=0; i < s.m_aJoints.GetSize(); i++)
	{
		A3DJoint* pJoint = s.m_aJoints[i]->Duplicate(this);
		if (!pJoint)
		{
			g_A3DErrLog.Log("A3DSkeleton::A3DSkeleton, Failed to clone joints");
			return;
		}

		AddJoint(pJoint);
	}

	//	Clone hooks
	for (i=0; i < s.m_aHooks.GetSize(); i++)
	{
		A3DSkeletonHook* pHook = s.m_aHooks[i]->Duplicate(this);
		if (!pHook)
		{
			g_A3DErrLog.Log("A3DSkeleton::A3DSkeleton, Failed to clone hooks");
			return;
		}

		AddHook(pHook);
	}
}

//	Initialize object
bool A3DSkeleton::Init()
{
	return true;
}

//	Release object
void A3DSkeleton::Release()
{
	//	Release track set if this is an old version .bon file
	if (m_dwVersion < 6 && m_bTrackSetOK)
	{
		A3D::g_A3DTrackMan.ReleaseSkeletonTrackSet(m_dwSkeletonID, m_strFileName);
	}

	int i;

	//	Release all bones
	for (i=0; i < m_aBones.GetSize(); i++)
	{
		A3DBone* pBone = m_aBones[i];
		pBone->Release();
		delete pBone;
	}

	m_aBones.RemoveAll();
	m_aRootBones.RemoveAll();

	//	Release all joints
	for (i=0; i < m_aJoints.GetSize(); i++)
	{
		A3DJoint* pJoint = m_aJoints[i];
		pJoint->Release();
		delete pJoint;
	}

	m_aJoints.RemoveAll();

	//	Release all hooks
	for (i=0; i < m_aHooks.GetSize(); i++)
	{
		delete m_aHooks[i];
	}

	m_aHooks.RemoveAll();
}

//	Clone skeleton
A3DSkeleton* A3DSkeleton::Clone()
{
	A3DSkeleton* pSkeleton = new A3DSkeleton(*this);
	if (!pSkeleton)
		return NULL;

	//	Create track set for old version .bon file
	if (m_dwVersion < 6 && m_dwSkeletonID)
	{
		bool bNewTrackSet;
		A3DSklTrackSet* pTrackSet = A3D::g_A3DTrackMan.CreateSkeletonTrackSet(m_dwSkeletonID, m_strFileName, true, bNewTrackSet);
		if (!pTrackSet)
		{
			g_A3DErrLog.Log("A3DSkeleton::Clone, Failed to create track set of skeleton %s", m_strFileName);
			return false;
		}

		ASSERT(!bNewTrackSet);
		
		//	Note: The newly created track set is used by cloned skeleton, not by this skeleton !
		pSkeleton->m_bTrackSetOK = true;
	}

	return pSkeleton;
}

//	Load skeleton data form file
bool A3DSkeleton::Load(AFile* pFile)
{
	A3DSKELETONFILEHEADER Header;
	DWORD dwRead;

	//	Load file header
	if (!pFile->Read(&Header, sizeof (Header), &dwRead) || dwRead != sizeof (Header))
	{
		g_A3DErrLog.Log("A3DSkeleton::Load, Failed to read file header");
		return false;
	}

	//	Compare identity and version
	if (Header.dwFlags != SKEFILE_IDENTIFY || Header.dwVersion > SKEFILE_VERSION)
	{
		g_A3DErrLog.Log("A3DSkeleton::Load, Invalid file identity or version");
		return false;
	}

	m_dwVersion		= Header.dwVersion;
	m_iAnimFPS		= Header.iAnimFPS;
	m_strFileName	= pFile->GetRelativeName();
	m_dwSkeletonID	= a_MakeIDFromLowString(m_strFileName);

	int i;

	//	Read bones
	for (i=0; i < Header.iNumBone; i++)
	{
		A3DBone* pBone = new A3DBone;
		if (!pBone)
			return false;

		if (!pBone->Init(this))
		{
			g_A3DErrLog.Log("A3DSkeleton::Load, Failed to init bone object");
			return false;
		}

		if (!pBone->Load(pFile))
		{
			g_A3DErrLog.Log("A3DSkeleton::Load, Failed to load bone %d", i);
			return false;
		}

		AddBone(pBone);
	}

	//	Load joints
	if (Header.iNumJoint)
	{
		//	Read joint types
		DWORD* aIDs = new DWORD [Header.iNumJoint];
		if (!aIDs)
			return false;

		if (!pFile->Read(aIDs, Header.iNumJoint * sizeof (DWORD), &dwRead) ||
			dwRead != Header.iNumJoint * sizeof (DWORD))
		{
			g_A3DErrLog.Log("A3DSkeleton::Load, Failed to load joint IDs");
			return false;
		}
	
		if (Header.dwVersion < 6)
		{
			//	Create skeleton track set
			bool bNewTrackSet;
			A3DSklTrackSet* pTrackSet = A3D::g_A3DTrackMan.CreateSkeletonTrackSet(m_dwSkeletonID, m_strFileName, true, bNewTrackSet);
			if (!pTrackSet)
			{
				g_A3DErrLog.Log("A3DSkeleton::Load, Failed to create track set of skeleton %s", m_strFileName);
				return false;
			}

			m_bTrackSetOK = true;

			pTrackSet->SetAnimFrameRange(Header.iAnimStart, Header.iAnimEnd);
			pTrackSet->SetAnimFPS(Header.iAnimFPS);

			//	Load joint data
			for (i=0; i < Header.iNumJoint; i++)
			{
				A3DJoint* pJoint = A3DJoint::NewJoint(aIDs[i], true);
				if (!pJoint)
					return false;

				if (!pJoint->Init(this))
					return false;

				if (!pJoint->Load(pFile))
				{
					g_A3DErrLog.Log("A3DSkeleton::Load, Failed to load joint %d", i);
					return false;
				}

				AddJoint(pJoint);

				ASSERT(bNewTrackSet);
				pTrackSet->LoadBoneTrack(pFile, i);
			}
			
			A3D::g_A3DTrackMan.AddActTrackDataSize(pTrackSet->GetDataSize());
		}
		else	//	Header.dwVersion >= 6
		{
			//	Load joint data
			for (i=0; i < Header.iNumJoint; i++)
			{
				A3DJoint* pJoint = A3DJoint::NewJoint(aIDs[i], true);
				if (!pJoint)
					return false;

				if (!pJoint->Init(this))
					return false;

				if (!pJoint->Load(pFile))
				{
					g_A3DErrLog.Log("A3DSkeleton::Load, Failed to load joint %d", i);
					return false;
				}

				AddJoint(pJoint);
			}
		}

		delete [] aIDs;
	}

	//	Load hooks
	for (i=0; i < Header.iNumHook; i++)
	{
		A3DSkeletonHook* pHook = new A3DSkeletonHook(this);
		if (!pHook)
		{
			g_A3DErrLog.Log("A3DSkeleton::Load, Not enough memory for hooks");
			return false;
		}

		if (!pHook->Load(pFile))
		{
			g_A3DErrLog.Log("A3DSkeleton::Load, Failed to load hook %d", i);
			return false;
		}

		AddHook(pHook);
	}

	//	Find reference bone
	FindRefBone();

	return true;
}

bool A3DSkeleton::Load(const char* szFile)
{
	AFileImage File;

	if (!File.Open("", szFile, AFILE_OPENEXIST | AFILE_BINARY | AFILE_TEMPMEMORY))
	{
		File.Close();
		g_A3DErrLog.Log("A3DSkeleton::Load, Cannot open file %s!", szFile);
		return false;
	}

	if (!Load(&File))
	{
		File.Close();
		return false;
	}

	File.Close();

	return true;
}

//	Save skeleton data from file
bool A3DSkeleton::Save(AFile* pFile)
{
	//	Fill header information
	A3DSKELETONFILEHEADER Header;
	memset(&Header, 0, sizeof (Header));

	Header.dwFlags		= SKEFILE_IDENTIFY;
	Header.dwVersion	= SKEFILE_VERSION;
	Header.iNumBone		= m_aBones.GetSize();
	Header.iNumJoint	= m_aJoints.GetSize();
	Header.iNumHook		= m_aHooks.GetSize();
	Header.iAnimStart	= 0;
	Header.iAnimEnd		= 0;
	Header.iAnimFPS		= m_iAnimFPS;
	
	//	Write file header
	DWORD dwWrite;
	if (!pFile->Write(&Header, sizeof (Header), &dwWrite) || dwWrite != sizeof (Header))
	{
		g_A3DErrLog.Log("A3DSkeleton::Save, Failed to write file header!");
		return false;
	}

	int i;

	//	Write bones
	for (i=0; i < Header.iNumBone; i++)
	{
		A3DBone* pBone = m_aBones[i];
		if (!pBone->Save(pFile))
		{
			g_A3DErrLog.Log("A3DSkeleton::Save, Failed to save bone %d !", i);
			return false;
		}
	}

	//	Write joints
	if (Header.iNumJoint)
	{
		DWORD* aIDs = new DWORD [Header.iNumJoint];
		if (!aIDs)
		{
			g_A3DErrLog.Log("A3DSkeleton::Save, Not enough memory for joint IDs");
			return false;
		}

		for (i=0; i < Header.iNumJoint; i++)
			aIDs[i] = m_aJoints[i]->GetClassID();

		//	Write joint ID
		if (!pFile->Write(aIDs, Header.iNumJoint * sizeof (DWORD), &dwWrite) ||
			dwWrite != Header.iNumJoint * sizeof (DWORD))
		{
			g_A3DErrLog.Log("A3DSkeleton::Save, Failed to save joint IDs");
			return false;
		}
	
		//	Load joint data
		for (i=0; i < Header.iNumJoint; i++)
		{
			A3DJoint* pJoint = m_aJoints[i];
			if (!pJoint->Save(pFile))
			{
				g_A3DErrLog.Log("A3DSkeleton::Save, Failed to save joint %d", i);
				return false;
			}
		}

		delete [] aIDs;
	}

	//	Write hooks
	for (i=0; i < Header.iNumHook; i++)
	{
		A3DSkeletonHook* pHook = m_aHooks[i];
		if (!pHook->Save(pFile))
		{
			g_A3DErrLog.Log("A3DSkeleton::Save, Failed to save hook %d !", i);
			return false;
		}
	}

	if( strlen(m_strFileName) == 0 )
	{
		// retrieve file name from the file saved to.
		char szRelativeName[MAX_PATH];
		af_GetFileTitle(pFile->GetFileName(), szRelativeName, MAX_PATH);
		m_strFileName = szRelativeName;
	}

	return true;
}

bool A3DSkeleton::Save(const char* szFile)
{
	AFile File;

	if (!File.Open((char*)szFile, AFILE_BINARY | AFILE_CREATENEW))
	{
		File.Close();
		g_A3DErrLog.Log("A3DSkeleton::Save, Cannot create file %s!", szFile);
		return false;
	}

	if (!Save(&File))
	{
		File.Close();
		return false;
	}

	File.Close();

	return true;
}

/*	Add a bone

	Return bone's index for success, otherwise return -1.

	pBone: bone object
*/
int A3DSkeleton::AddBone(A3DBone* pBone)
{
	int iIndex = m_aBones.Add(pBone);
	if (pBone->GetParent() < 0)
		m_aRootBones.Add(iIndex);

	return iIndex;
}

/*	Add a joint

	Return joint's index for success, otherwise return -1.

	pJoint: Joint object
*/
int A3DSkeleton::AddJoint(A3DJoint* pJoint)
{
	int iIndex = m_aJoints.Add(pJoint);
	return iIndex;
}

/*	Update frame

	iDeltaTime: frame time in ms
*/
bool A3DSkeleton::Update(int iDeltaTime)
{
	m_dwUpdateCnt++;

	if (!m_aBones.GetSize())
		return true;

	if (m_pSkinModel && m_pSkinModel->GetA3DEngine())
		m_pSkinModel->GetA3DEngine()->BeginPerformanceRecord(A3DEngine::PERF_TICK_SKELETON);

	//	Update all root bones
	for (int i=0; i < m_aRootBones.GetSize(); i++)
	{
		A3DBone* pBone = m_aBones[m_aRootBones[i]];
		UpdateBoneTree(pBone, iDeltaTime);
	}

	if (m_pSkinModel && m_pSkinModel->GetA3DEngine())
		m_pSkinModel->GetA3DEngine()->EndPerformanceRecord(A3DEngine::PERF_TICK_SKELETON);

	return true;
}

//	Update bone tree
bool A3DSkeleton::UpdateBoneTree(A3DBone* pRootBone, int iDeltaTime)
{
	pRootBone->Update(iDeltaTime);

	int iNumChild = pRootBone->GetChildNum();
	if (!iNumChild)
		return true;

	//	Update children
	for (int i=0; i < iNumChild; i++)
	{
		A3DBone* pBone = m_aBones[pRootBone->GetChild(i)];
		UpdateBoneTree(pBone, iDeltaTime);
	}

	return true;
}

/*	Get bone by name and index. GetBnoe() will first try to get bone through
	index passed by *piIndex, if it failed, szName will be used then.

	Return bone object's address for success, otherwise return NULL.

	szName: bone's name
	piIndex (in, out): bone's index, can be NULL
*/
A3DBone* A3DSkeleton::GetBone(const char* szName, int* piIndex) const
{
	//	Try index at first
	if (piIndex && *piIndex >= 0 && *piIndex < m_aBones.GetSize())
	{
		A3DBone* pBone = m_aBones[*piIndex];
		if (!_stricmp(pBone->GetName(), szName))
			return pBone;
	}

	//	Enumerate all bones
	for (int i=0; i < m_aBones.GetSize(); i++)
	{
		A3DBone* pBone = m_aBones[i];
		if (!_stricmp(pBone->GetName(), szName))
		{
			if (piIndex)
				*piIndex = i;

			return pBone;
		}
	}

	return NULL;
}

/*	Get joint by name and index. GetJoint() will first try to get joint through
	index passed by *piIndex, if it failed, szName will be used then.

	Return joint object's address for success, otherwise return NULL.

	szName: joint's name
	piIndex (in, out): joint's index, can be NULL
*/
A3DJoint* A3DSkeleton::GetJoint(const char* szName, int* piIndex)
{
	//	Try index at first
	if (piIndex && *piIndex >= 0 && *piIndex < m_aJoints.GetSize())
	{
		A3DJoint* pJoint = m_aJoints[*piIndex];
		if (!_stricmp(pJoint->GetName(), szName))
			return pJoint;
	}

	//	Enumerate all joints
	for (int i=0; i < m_aJoints.GetSize(); i++)
	{
		A3DJoint* pJoint = m_aJoints[i];
		if (!_stricmp(pJoint->GetName(), szName))
		{
			if (piIndex)
				*piIndex = i;

			return pJoint;
		}
	}

	return NULL;
}

//	Get index of joint in skeleton
int A3DSkeleton::GetJointIndex(A3DJoint* pJoint)
{
	for (int i=0; i < m_aJoints.GetSize(); i++)
	{
		if (m_aJoints[i] == pJoint)
			return i;
	}

	return -1;
}

/*	Add hook

	Return hook's index for success, otherwise return -1.
*/
int A3DSkeleton::AddHook(A3DSkeletonHook* pHook)
{
	return m_aHooks.Add(pHook);
}

/*	Get hook. GetHook() will first try to get hook object through
	index passed by *piIndex, if it failed, szName will be used then.

	Return hook object's address for success, otherwise return NULL.

	szName: hook's name.
	piIndex (in & out): hook's index, can be NULL.
*/
A3DSkeletonHook* A3DSkeleton::GetHook(const char* szName, int* piIndex)
{
	//	Try index at first
	if (piIndex && *piIndex >= 0 && *piIndex < m_aHooks.GetSize())
	{
		A3DSkeletonHook* pHook = m_aHooks[*piIndex];
		if (!_stricmp(pHook->GetName(), szName))
			return pHook;
	}

	//	Enumerate all hooks
	for (int i=0; i < m_aHooks.GetSize(); i++)
	{
		A3DSkeletonHook* pHook = m_aHooks[i];
		if (!_stricmp(pHook->GetName(), szName))
		{
			if (piIndex)
				*piIndex = i;

			return pHook;
		}
	}

	return NULL;
}

//	Get approximate animation size
int A3DSkeleton::GetAnimDataSize()
{
	int iSize = 0;
	return iSize;
}

//	Reset all bones' relative TM to original value
void A3DSkeleton::ResetBoneRelativeTM()
{
	for (int i=0; i < m_aBones.GetSize(); i++)
		m_aBones[i]->ResetRelativeTM();
}

//	Find a reference bone which be used to translate bound boxes or mesh centers
bool A3DSkeleton::FindRefBone()
{
	ASSERT(m_aRootBones.GetSize());

	//	Find a root boot
	for (int i=0; i < m_aRootBones.GetSize(); i++)
	{
		int iBone = m_aRootBones[i];
		A3DBone* pBone = m_aBones[iBone];

		if (pBone->GetBoneInitTM() != A3D::g_matIdentity)
		{
			m_iRefBone = iBone;
			return true;
		}

		for (int j=0; j < pBone->GetChildNum(); j++)
		{
			A3DBone* pChild = pBone->GetChildPtr(j);
			if (pChild->GetBoneInitTM() != A3D::g_matIdentity)
			{
				m_iRefBone = pBone->GetChild(j);
				return true;
			}
		}
	}

	return false;
}

//	Update skeleton state for making absolute track, used only by A3DAbsTrackMaker. 
//	This function doesn't consider scale effects
bool A3DSkeleton::UpdateForMakingAbsTrack()
{
	if (!m_aBones.GetSize())
		return true;

	//	Update all root bones
	for (int i=0; i < m_aRootBones.GetSize(); i++)
	{
		A3DBone* pBone = m_aBones[m_aRootBones[i]];
		UpdateBoneTreeForMakingAbsTrack(pBone);
	}

	return true;
}

bool A3DSkeleton::UpdateBoneTreeForMakingAbsTrack(A3DBone* pRootBone)
{
	pRootBone->UpdateForMakingAbsTrack();

	int iNumChild = pRootBone->GetChildNum();
	if (!iNumChild)
		return true;

	//	Update children
	for (int i=0; i < iNumChild; i++)
	{
		A3DBone* pBone = m_aBones[pRootBone->GetChild(i)];
		UpdateBoneTreeForMakingAbsTrack(pBone);
	}

	return true;
}

//	Called when a bone's local scale matrix changes
void A3DSkeleton::OnBoneLocalScaleChanges(A3DBone* pBone)
{
	if (!pBone || pBone->GetSkeleton() != this)
	{
		ASSERT(0);
		return;
	}

	//	Notify all hooks which bound to the specified bone
	for (int i=0; i < m_aHooks.GetSize(); i++)
	{
		A3DSkeletonHook* pHook = m_aHooks[i];
		int iBone = pHook->GetBone();
		if (iBone >= 0 && m_aBones[iBone] == pBone)
			pHook->RebuildScaledHookTM();
	}
}

//	Called when a bone's whole scale matrix changes
void A3DSkeleton::OnBoneWholeScaleChanges()
{
	//	Notify all hooks which bound on this skeleton
	for (int i=0; i < m_aHooks.GetSize(); i++)
	{
		A3DSkeletonHook* pHook = m_aHooks[i];
		pHook->RebuildScaledHookTM();
	}

	//	Added by dyx, 2013-7-17
	//	Note: old-type bone scaling doesn't pass whole scale factor to children
	if (g_pA3DConfig->GetFlagNewBoneScale())
	{
		if (m_pSkinModel)
			m_pSkinModel->OnBoneWholeScaleChanges();
	}
}

//	Set inherit whole scale factor
void A3DSkeleton::SetInheritScale(float fScale)
{
	if (m_fInheritScale == fScale)
		return;

	m_fInheritScale = fScale;

	//	Pass inherit whole scale info to all bones
	for (int i=0; i < m_aRootBones.GetSize(); i++)
	{
		A3DBone* pBone = m_aBones[m_aRootBones[i]];
		pBone->PassAccuWholeScale();
	}
}

//	Calculate foot offset caused by bone scaling
void A3DSkeleton::CalcFootOffset(const char* szFootBone/* NULL */)
{
	int iNumBone = m_aBones.GetSize();
	if (!iNumBone)
		return;

	//	Update all bones in original state, don't consider bone scale
	AArray<A3DMATRIX4> aMats;
	GetOriginBoneState(aMats, false);
	
	//	If user specified a font bone, try to use it
	m_iFootBone = -1;
	if (szFootBone)
		GetBone(szFootBone, &m_iFootBone);

	int i;
	A3DVECTOR3 vPos;

	if (m_iFootBone < 0)
	{
		//	Find the lowest bone and treat it as foot
		vPos.Set(0.0f, FLT_MAX, 0.0f);

		for (i=0; i < iNumBone; i++)
		{
			A3DVECTOR3 v = aMats[i].GetRow(3);
			if (v.y < vPos.y)
			{
				vPos = v;
				m_iFootBone = i;
			}
		}
	}
	else
		vPos = aMats[m_iFootBone].GetRow(3);

	//	Get the cross-position of vertical line (which through foot bone origin) 
	//	with world space's xz-plane, and transform the position into foot bone's space
	A3DVECTOR3 vPosOnFoot = vPos;
	vPosOnFoot.y = 0.0f;	//	Move position to xz plane
	A3DMATRIX4 matInv;
	A3DMATRIX4 mattemp1 = aMats[m_iFootBone];
	a3d_InverseTM(aMats[m_iFootBone], &matInv);
	vPosOnFoot = vPosOnFoot * matInv;

	//	Update all bones in original state again, but this time
	//	consider bone scale
	GetOriginBoneState(aMats, true);

	//	Transform foot position to world space again
	A3DMATRIX4 mattemp2 = aMats[m_iFootBone];
	A3DVECTOR3 vPos1 = vPosOnFoot * aMats[m_iFootBone];

	//	The foot offset just is the new position's y
	m_fFootOffset = vPos1.y;
}

//	Get original bone state
void A3DSkeleton::GetOriginBoneState(AArray<A3DMATRIX4>& aMats, bool bUseScale/* false */)
{
	int iNumBone = m_aBones.GetSize();
	if (iNumBone)
	{
		aMats.SetSize(iNumBone, 4);

		AArray<A3DMATRIX4> aUpToRootMats;
		aUpToRootMats.SetSize(iNumBone, 4);

		for (int i=0; i < m_aRootBones.GetSize(); i++)
			GetOriginBoneState_r(m_aRootBones[i], -1, bUseScale, aMats, aUpToRootMats);
	}
}

//	Get original bone state
void A3DSkeleton::GetOriginBoneState_r(int iBoneIdx, int iParent, bool bUseScale, 
							AArray<A3DMATRIX4>& aMats, AArray<A3DMATRIX4>& aUpToRootMats)
{
	A3DBone* pBone = m_aBones[iBoneIdx];
	if (iParent >= 0)
		pBone->BuildOriginUpToRootMatrix(bUseScale, aUpToRootMats[iParent], aMats[iBoneIdx], aUpToRootMats[iBoneIdx]);
	else
		pBone->BuildOriginUpToRootMatrix(bUseScale, A3D::g_matIdentity, aMats[iBoneIdx], aUpToRootMats[iBoneIdx]);

	int i, iNumChild = pBone->GetChildNum();

	for (i=0; i < iNumChild; i++)
	{
		int iChild = pBone->GetChild(i);
		GetOriginBoneState_r(iChild, iBoneIdx, bUseScale, aMats, aUpToRootMats);
	}
}

//	Get bone state at specified frame
void A3DSkeleton::GetFrameBoneState(AArray<A3DMATRIX4>& aRelMats, AArray<A3DMATRIX4>& aOutMats,
							bool bUseScale/* false */)
{
	int iNumBone = m_aBones.GetSize();
	if (iNumBone)
	{
		aOutMats.SetSize(iNumBone, 4);

		AArray<A3DMATRIX4> aUpToRootMats;
		aUpToRootMats.SetSize(iNumBone, 4);

		for (int i=0; i < m_aRootBones.GetSize(); i++)
			GetFrameBoneState_r(m_aRootBones[i], -1, bUseScale, aRelMats, aOutMats, aUpToRootMats);
	}
}

//	Get bone state of specified frame
void A3DSkeleton::GetFrameBoneState_r(int iBoneIdx, int iParent, bool bUseScale, 
							AArray<A3DMATRIX4>& aRelMats, AArray<A3DMATRIX4>& aOutMats, 
							AArray<A3DMATRIX4>& aUpToRootMats)
{
	A3DBone* pBone = m_aBones[iBoneIdx];
	if (iParent >= 0)
		pBone->BuildFrameUpToRootMatrix(bUseScale, aUpToRootMats[iParent], aRelMats[iBoneIdx], aOutMats[iBoneIdx], aUpToRootMats[iBoneIdx]);
	else
		pBone->BuildFrameUpToRootMatrix(bUseScale, A3D::g_matIdentity, aRelMats[iBoneIdx], aOutMats[iBoneIdx], aUpToRootMats[iBoneIdx]);

	int i, iNumChild = pBone->GetChildNum();

	for (i=0; i < iNumChild; i++)
	{
		int iChild = pBone->GetChild(i);
		GetFrameBoneState_r(iChild, iBoneIdx, bUseScale, aRelMats, aOutMats, aUpToRootMats);
	}
}

