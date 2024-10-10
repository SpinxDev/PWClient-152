/*
 * FILE: A3DSkinPhysSync.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2008/6/17
 *
 * HISTORY: 
 *
 * Copyright (c) 2008 Archosaur Studio, All Rights Reserved.
 */

#include "StdAfx.h"
#ifdef A3D_PHYSX

#include "A3DSkinPhysSync.h"
#include "A3DSkinPhysSyncData.h"

#include "APhysX.h"

#include <ALog.h>
#include <A3DSkinModel.h>
#include <A3DSkin.h>
#include <A3DSkeleton.h>
#include <A3DBone.h>
#include <A3DClothMesh.h>

#ifdef _ANGELICA21
#include "A3DAdhereMesh.h"
#endif


///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

// the time unit is ms...
#define CLOTH_SIM_DELAY_TIME 500
#define CLOTH_SIM_BLEND_TIME 800

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
//	Implement A3DSkinPhysSync
//	
///////////////////////////////////////////////////////////////////////////

A3DSkinPhysSync::A3DSkinPhysSync()
{
	m_pCoreData			= NULL;
	m_pPhysScene		= NULL;
	m_pPhysSkin			= NULL;
	m_pScaleClothDesc	= NULL;
	m_pA3DSkin			= NULL;

	m_iClothSimDelayTime = CLOTH_SIM_DELAY_TIME;
	m_iClothSimBlendTime = CLOTH_SIM_BLEND_TIME;

	m_iClothSimBlendCounter = 0;

	m_fClothSimWeight = 0.0f;

	m_bIgnoreClothSimBlendCounter = false;
}

A3DSkinPhysSync::~A3DSkinPhysSync()
{
	//	Release physical skin
	Unbind(true);
}

//	Bind physical data with specified skin
bool A3DSkinPhysSync::Bind(APhysXScene* pPhysScene, A3DSkinModel* pSkinModel, A3DSkin* pSkin, const A3DVECTOR3& vRefPos, int iClothSimDelayTime, int iClothSimBlendTime)
{
	if (!pPhysScene || !pSkinModel || !pSkin)
		return false;

	//	Release old physical cloth

	gGetAPhysXScene()->GetPerfStats()->ResumePerfStat("ASP_UnBind");
	Unbind(false);
	gGetAPhysXScene()->GetPerfStats()->PausePerfStat("ASP_UnBind");

	//	Clear old link info
	m_aRBActorLinks.RemoveAll();

	//	Create core data if it doesn't exist
	gGetAPhysXScene()->GetPerfStats()->ResumePerfStat("ASP_LoadSphy");

	if (!m_pCoreData)
	{
		//	Make sync data file name
		AString strSyncFile = pSkin->GetFileName();
		af_ChangeFileExt(strSyncFile, ".sphy");

		//	Create sync data object
		m_pCoreData = A3DSkinPhysSyncDataMan::GetGlobalMan().LoadSyncData(strSyncFile);
		if (!m_pCoreData || !m_pCoreData->GetBindInitFlag())
		{
			gGetAPhysXScene()->GetPerfStats()->PausePerfStat("ASP_LoadSphy");
			return false;
		}
	}
	gGetAPhysXScene()->GetPerfStats()->PausePerfStat("ASP_LoadSphy");

	m_pPhysScene = pPhysScene;

	//	Check if skin can be bound with this physical data
	gGetAPhysXScene()->GetPerfStats()->ResumePerfStat("ASP_BindCheck");
	if (!m_pCoreData->SkinBindCheck(pSkin))
	{
		a_LogOutput(1, "A3DSkinPhysSync::Bind, failed to call SkinBindCheck() for skin %s", pSkin->GetFileName());
		gGetAPhysXScene()->GetPerfStats()->PausePerfStat("ASP_BindCheck");
		return false;
	}
		gGetAPhysXScene()->GetPerfStats()->PausePerfStat("ASP_BindCheck");

	//	Create RB actor lnik info
	int i, iNumActor = m_pCoreData->GetActorNum();
	m_aRBActorLinks.SetSize(iNumActor, 4);

	for (i=0; i < iNumActor; i++)
	{
		A3DSkinPhysSyncData::ACTOR_DATA* pActor = m_pCoreData->GetActorByIndex(i);
		RBACTOR_LINK& link = m_aRBActorLinks[i];
		link.bHHIsBone = pActor->bHHIsBone;		

		if (pActor->bHHIsBone)
		{
			if (!pSkinModel->GetSkeleton()->GetBone(pActor->strHHHook, &link.iHookHHIdx))
			{
				a_LogOutput(1, "A3DSkinPhysSync::Bind, failed to find bone %s", pActor->strHHHook);
				return false;
			}
		}
		else
		{
			if (!pSkinModel->GetSkeleton()->GetHook(pActor->strHHHook, &link.iHookHHIdx))
			{
				a_LogOutput(1, "A3DSkinPhysSync::Bind, failed to find HH hook %s", pActor->strHHHook);
				return false;
			}
		}
	}

	gGetAPhysXScene()->GetPerfStats()->ResumePerfStat("ASP_CheckScale");
	bool bMeshScaled = CheckMeshScaled(pSkinModel);
	gGetAPhysXScene()->GetPerfStats()->PausePerfStat("ASP_CheckScale");

	APhysXClothAttacherDesc* pAttacherDesc = NULL;

	if (bMeshScaled)
	{
		if (!m_pScaleClothDesc)
		{
			m_pScaleClothDesc = new APhysXClothAttacherDesc;
			
			gGetAPhysXScene()->GetPerfStats()->ResumePerfStat("ASP_CreateScaledDesc");
			m_pCoreData->CreateScaledSkinPhysDesc(pSkinModel, pSkin, m_pPhysScene, m_pScaleClothDesc);
			gGetAPhysXScene()->GetPerfStats()->PausePerfStat("ASP_CreateScaledDesc");

		}
		
		pAttacherDesc = m_pScaleClothDesc;
	}
	else
		pAttacherDesc = m_pCoreData->GetSkinPhysDesc();

	//	Create physical object
	APhysXObjectInstanceDesc apxObjInstanceDesc;
	apxObjInstanceDesc.mPhysXObjectDesc = pAttacherDesc;
//	apxObjInstanceDesc.mGlobalPose.id();
	apxObjInstanceDesc.mScale3D = NxVec3(1.0f);


	//-------------------------------------------------------------------
	// revised by wenfeng, Dec. 23, 2008
	// we should consider the orientation as well as the position when 
	// creating the cloth object... and so, here we just ignore the 
	// vRefPos passed in...

	A3DMATRIX4 mat = pSkinModel->GetAbsoluteTM();
	// A3DMATRIX4 mat = a3d_Translate(vRefPos.x, vRefPos.y, vRefPos.z);
	//-------------------------------------------------------------------

	
	APhysXConverter::A2N_Matrix44(mat, apxObjInstanceDesc.mGlobalPose);

	gGetAPhysXScene()->GetPerfStats()->ResumePerfStat("ASP_CreateClothObj");
	m_pPhysSkin = dynamic_cast<APhysXClothAttacher*>(pPhysScene->CreatePhysXObject(apxObjInstanceDesc));
	gGetAPhysXScene()->GetPerfStats()->PausePerfStat("ASP_CreateClothObj");

	if (!m_pPhysSkin)
	{
		ASSERT(m_pPhysSkin);

		if (bMeshScaled)
		{
			delete pAttacherDesc;
			pAttacherDesc = NULL;
			m_pScaleClothDesc = NULL;
		}

		a_LogOutput(1, "A3DSkinPhysSync::Bind, failed to create APhysXClothAttacher for skin %s", pSkin->GetFileName());
		return false;
	}

	//	Record bound skin
	m_pA3DSkin = pSkin;
	
	m_iClothSimDelayTime = iClothSimDelayTime;
	m_iClothSimBlendTime =  iClothSimBlendTime;
	m_iClothSimBlendCounter = 0;
	m_fClothSimWeight = 0.0f;


	// call this function to init the cloth constraint position
	gGetAPhysXScene()->GetPerfStats()->ResumePerfStat("ASP_UpdateSkin");
	UpdateSkinByAnimation(0, pSkinModel);
	gGetAPhysXScene()->GetPerfStats()->PausePerfStat("ASP_UpdateSkin");

#ifdef _ANGELICA21
	m_pSkinModel = pSkinModel;
#endif	

	return true;
}


void A3DSkinPhysSync::InitAnimVertInfo()
{
	// Init AnimVertInfo...
	int iClothNum = m_pA3DSkin->GetClothMeshNum();
	for(int i=0; i<iClothNum; i++)
	{
		AnimVertInfo* pAvInfo = new AnimVertInfo;
		pAvInfo->mClothMesh = m_pA3DSkin->GetClothMeshImp(i);
		pAvInfo->mSkinMesh = pAvInfo->mClothMesh ? (A3DSkinMesh*)(m_pA3DSkin->GetSkinMeshRefByName(pAvInfo->mClothMesh->GetName())->pMesh) : NULL;

		m_ArrAnimVertInfo.Add(pAvInfo);
	}
}


#ifdef _ANGELICA21
bool A3DSkinPhysSync::InitAdhereMesh()
{
	for (int i = 0; i < m_aAdhereMeshes.GetSize(); i++)
	{
		delete m_aAdhereMeshes[i];
	}
	m_aAdhereMeshes.RemoveAll();

	int n = m_pA3DSkin->GetClothMeshNum();

	if (m_pCoreData == NULL)
		return false;

	int num = m_pCoreData->GetMeshNum();

	for (int i = 0; i < num; i++)
	{
		A3DSkinPhysSyncData::MESH_DATA* meshdata =  m_pCoreData->GetMeshByIndex(i);
		if (meshdata->GetAdhereParams() == NULL)
			continue;
		AString strAdhereMesh = meshdata->GetAdhereMeshName();
		AString strMeshName = meshdata->GetMeshName();

		//find the A3DClothMeshImp
		A3DClothMeshImp* pSrcMesh = NULL;
		A3DClothMeshImp* pDstMesh = NULL;
		for (int j = 0; j < m_pA3DSkin->GetClothMeshNum(); j++)
		{
			A3DClothMeshImp* pMesh = m_pA3DSkin->GetClothMeshImp(j);
			if (!pMesh)
				continue;

			const char* strName = pMesh->GetName();
			if (!strName)
			{
				assert(0);
				continue;
			}

			if (strAdhereMesh == strName)
				pDstMesh = pMesh;
			else if (strMeshName == strName)
				pSrcMesh = pMesh;
		}
		if (pSrcMesh == NULL || pDstMesh == NULL)
		{
			a_LogOutput(1, "A3DSkinPhysSync::InitAdhereMesh() error pSrcMesh == NULL or pDstMesh == NULL");
			return false;
		}

		A3DAdhereMesh* pAdhereMesh = new A3DAdhereMesh;
		if (!pAdhereMesh->Init(m_pA3DSkin, m_pSkinModel, pSrcMesh, pDstMesh, meshdata->aAdherParams))
		{
			a_LogOutput(1, "A3DAdhereMesh::Init() return false");
			return false;
		}

		m_aAdhereMeshes.Add(pAdhereMesh);
	}

	return true;
}
#endif

//	Un-bind physical data
void A3DSkinPhysSync::Unbind(bool bDelCoreData/* false */)
{
	//	Note: Physical system require use release m_pPhysSkin before m_pCoreData
	if (m_pPhysSkin && m_pPhysScene)
	{
		m_pPhysScene->ReleasePhysXObject(m_pPhysSkin);
		m_pPhysSkin = NULL;
	}

	if (bDelCoreData)
	{
		if (m_pScaleClothDesc)
		{
			delete m_pScaleClothDesc;
			m_pScaleClothDesc = NULL;
		}

		if (m_pCoreData)
		{
			A3DSkinPhysSyncDataMan::GetGlobalMan().ReleaseSyncData(m_pCoreData);
			m_pCoreData = NULL;
		}
	}

	for (int i = 0; i < m_ArrAnimVertInfo.GetSize(); i++)
	{
		delete m_ArrAnimVertInfo[i];
	}
	m_ArrAnimVertInfo.RemoveAll(true);

#ifdef _ANGELICA21
	for (int i = 0; i < m_aAdhereMeshes.GetSize(); i++)
	{
		delete m_aAdhereMeshes[i];
	}
	m_aAdhereMeshes.RemoveAll(true);
#endif
}

//	Check if mesh needs to be scaled before it changes to cloth
bool A3DSkinPhysSync::CheckMeshScaled(A3DSkinModel* pSkinModel)
{
	A3DSkeleton* pSkeleton = pSkinModel->GetSkeleton();

	int i, j;
	int iNumMesh = m_pCoreData->GetMeshNum();
	
	for (i=0; i < iNumMesh; i++)
	{
		A3DSkinPhysSyncData::MESH_DATA* pMeshData = m_pCoreData->GetMeshByIndex(i);
		AArray<int>& aBlendBones = pMeshData->GetBlendBones();

		for (j=0; j < aBlendBones.GetSize(); j++)
		{
			A3DBone* pBone = pSkeleton->GetBone(aBlendBones[j]);
			if (pBone->GetAccuWholeScale() != 1.0f)// ||
			//	pBone->GetLocalLengthScale() != 1.0f)
				return true;

			//	Child's thick and length change will also effect parent bone's
			//	local scale matrix. 
			int m, iNumChild = pBone->GetChildNum();
			if (iNumChild)
			{
				for (m=0; m < iNumChild; m++)
				{
					A3DBone* pChildBone = pBone->GetChildPtr(m);
					if (pChildBone->GetLocalThickScale() != 1.0f ||
						pChildBone->GetLocalLengthScale() != 1.0f)
						return true;
				}
			}
			else if (pBone->GetLocalThickScale() != 1.0f)
			{
				//	If bone has child, it's thickness only effect it's parent bone, 
				//	but if bone hasn't child, we will have to consider it's thick factor.
				return true;		
			}
		}
	}

	return false;
}

//	Update pose of RB actors using soft-link. Call this before physics simulation
bool A3DSkinPhysSync::UpdateRBActorPose(A3DSkinModel* pSkinModel)
{
	if (!pSkinModel || !IsSkinBound())
		return false;

	int i;
	A3DSkeleton* pSkeleton = pSkinModel->GetSkeleton();
	A3DMATRIX4 matHH;
	NxMat34 matNx;

	for (i=0; i < m_aRBActorLinks.GetSize(); i++)
	{
		const RBACTOR_LINK& link = m_aRBActorLinks[i];

		//	Get absolute TM of hh-type hook
		if (link.bHHIsBone)
		{
			A3DBone* pBone = pSkeleton->GetBone(link.iHookHHIdx);
			matHH = pBone->GetNoScaleAbsTM();
		}
		else
		{
			A3DSkeletonHook* pHook = pSkeleton->GetHook(link.iHookHHIdx);
			matHH = pHook->GetNoScaleAbsTM();
		}

		//	Try to get RB actor
		A3DSkinPhysSyncData::ACTOR_DATA* pActor = m_pCoreData->GetActorByIndex(i);
		if (!pActor)
		{
			//	This shouldn't happen
			ASSERT(pActor);
			continue;
		}

		APhysXConverter::A2N_Matrix44(matHH, matNx);
		m_pPhysSkin->SetAttacherPose(pActor->strName, matNx);
	}

	return true;
}

bool A3DSkinPhysSync::UpdateSkinByAnimation(int iDt, A3DSkinModel* pSkinModel, bool bUpdateNormals /*= true*/)
{
#if APHYSX_CURRENT_PHYSX_SDK_VERSION >= APHYSX_GET_PHYSX_SDK_VERSION(2, 8, 4)

	// do the skin by animation...
	if(m_pCoreData)
	{
		int iClothNum = m_pA3DSkin->GetClothMeshNum();
		int iAnimVertNum = m_ArrAnimVertInfo.GetSize();
		if (iClothNum <= 0 || iAnimVertNum <= 0)
			return false; // AnimVert Not Inited yet!

		if (iClothNum != iAnimVertNum)
		{
			ASSERT(iClothNum == iAnimVertNum);
			return false;
		}

		// firstly, we build the bone's matrices for CPU skinning...
		
		A3DSkeleton* pSkeleton = pSkinModel->GetSkeleton();
		int iBoneNum = pSkeleton->GetBoneNum();
		A3DMATRIX4* aMats = new A3DMATRIX4[iBoneNum];
		for(int i=0; i<iBoneNum; i++)
		{
			A3DBone* pBone = pSkeleton->GetBone(i);
			aMats[i] = pBone->GetBoneInitTM() * pBone->GetAbsoluteTM();
		}


		// then, we do the skinning
		for(int i=0; i<iClothNum; i++)
		{

#ifdef _ANGELICA21
			if (m_pCoreData->GetMeshByIndex(i)->GetAdhereParams())
				continue;
#endif
			

			A3DClothMeshImp* pClothMesh = m_pA3DSkin->GetClothMeshImp(i);
			APhysXClothObject* pClothObject = pClothMesh ? m_pPhysSkin->GetClothObject(pClothMesh->GetName()) : NULL;

			AnimVertInfo* pAnimVertInfo = m_ArrAnimVertInfo[i];
			ASSERT(pAnimVertInfo);

			A3DSkinMesh* pSkinMesh = NULL;
			if (pAnimVertInfo)
			{
				APHYSX_ASSERT(pClothMesh == pAnimVertInfo->mClothMesh);
				pSkinMesh = pAnimVertInfo->mSkinMesh;
			}

			if (pClothObject && pSkinMesh && pClothObject->IsConstraintsEnabled())
			{
				int iVertNum = pSkinMesh->GetVertexNum();
				A3DVECTOR3* Verts = new A3DVECTOR3[iVertNum];
				A3DVECTOR3* Normals = new A3DVECTOR3[iVertNum];

				pSkinMesh->GetBlendedVertices(m_pA3DSkin, aMats, Verts, Normals);
				pClothObject->SetConstrainPositions((NxVec3* )Verts);
				//--------------------------------------
				// Currently, do not use constraint normals because in Angelica2 cloth meshes are always two-sided.

#ifdef _ANGELICA21				
				
				//--------------------------------------
				// Note by wenfeng, Jun. 13, 2012
				//	In A21, all constrained cloth mesh are single-side mesh with two-side texture support.
				// so, we can use the constrain normals to augment the collision effect by the extra collsion sphere

				if(bUpdateNormals)	pClothObject->SetConstrainNormals((NxVec3* )Normals);

#endif

				if (pAnimVertInfo)
				{
					// fill the anim vertex info...
					if(!pAnimVertInfo->mAnimPositions)
					{
						pAnimVertInfo->mAnimPositions = new A3DVECTOR3[iVertNum];
					}
					memcpy(pAnimVertInfo->mAnimPositions, Verts, sizeof(A3DVECTOR3) * iVertNum);

					if(!pAnimVertInfo->mAnimNormals)
					{
						pAnimVertInfo->mAnimNormals = new A3DVECTOR3[iVertNum];
					}
					memcpy(pAnimVertInfo->mAnimNormals, Normals, sizeof(A3DVECTOR3) * iVertNum);

					pAnimVertInfo->mInvalid = false;
				}

				delete [] Verts;
				delete [] Normals;
			}
		}


		delete [] aMats;

#if 0
		//----------------------------------------------------------
		// Note by Wenfeng, Dec. 19, 2012
		// do not compute the blend weight here by iDt, since users sometime 
		// call ECModel's Tick(sometime) to jump to special frame of animation
		// and this would disable the effect of blending...

		// compute the blend weight...
		if(m_iClothSimBlendCounter > CLOTH_SIM_BLEND_TIME)
		{
			m_fClothSimWeight = 1.0f;
		}
		else
		{
			m_fClothSimWeight = m_iClothSimBlendCounter / (float)CLOTH_SIM_BLEND_TIME;
			m_iClothSimBlendCounter += iDt;
		}

#endif

		return true;
	}
	else
		return true;



#else
	// for case of 
	return false;

#endif

}

//	Sync skin mesh from physical data. Call this after physics simulation
bool A3DSkinPhysSync::SyncSkinPhys(A3DSkin* pSkin)
{
	if (!pSkin || !IsSkinBound())
		return false;

	//	Check if skin has been bound with physical data
	if (m_pA3DSkin != pSkin)
	{
		ASSERT(m_pA3DSkin == pSkin);
		return false;
	}

	if (!pSkin->GetClothMeshNum() || !pSkin->IsClothesEnable())
		return true;

	//	Check mesh number, ASSERT is enough because m_pA3DSkin == pSkin means 
	//	that this skin has been bound with m_pCoreData exactly
	ASSERT(pSkin->GetSkinMeshNum() >= m_pCoreData->m_aSkinMeshInfo.GetSize());
	ASSERT(pSkin->GetRigidMeshNum() >= m_pCoreData->m_aRigidMeshInfo.GetSize());
	ASSERT(pSkin->GetClothMeshNum() == m_pCoreData->GetMeshNum());

	int i, iNumMesh = pSkin->GetClothMeshNum();

	for (i=0; i < iNumMesh; i++)
	{
#ifdef _ANGELICA21
		if (m_pCoreData->GetMeshByIndex(i)->GetAdhereParams())
			continue;
#endif
		A3DClothMeshImp* pClothMesh = pSkin->GetClothMeshImp(i);
		APhysXClothObject* pClothPhys = pClothMesh ? m_pPhysSkin->GetClothObject(pClothMesh->GetName()) : NULL;
		if (!pClothPhys)
			continue;


		// blend the final positions and normals by m_fClothSimWeight...
		A3DVECTOR3* Verts = NULL;
		bool bReleaseVerts = false;
		A3DVECTOR3* Normals = NULL;
		bool bReleaseNormals = false;

		AnimVertInfo* pAnimVertInfo = NULL;
		if (i < m_ArrAnimVertInfo.GetSize())
		{
			pAnimVertInfo = m_ArrAnimVertInfo[i];
			ASSERT(pAnimVertInfo);
		}
		else
			ASSERT(i < m_ArrAnimVertInfo.GetSize());

		if (m_fClothSimWeight > 1.0f - 1e-4f ||
		   !pAnimVertInfo || pAnimVertInfo->mInvalid ||
		   !pAnimVertInfo->mAnimPositions || !pAnimVertInfo->mAnimNormals || 
		   !pAnimVertInfo->mSkinMesh)
		{
			Verts = (A3DVECTOR3*)pClothPhys->GetVertexPositions();
			Normals = (A3DVECTOR3*)pClothPhys->GetVertexNormals();
		}
		else
		{
			int iOriginVertNum = pAnimVertInfo->mSkinMesh->GetVertexNum();
			int iVertNum = pClothPhys->GetVertexNum();
			APHYSX_ASSERT(iVertNum >= iOriginVertNum);

#ifdef _ANGELICA21
			if (iVertNum < iOriginVertNum)
			{
				assert(false);
				continue;
			}
#endif

			Verts = new A3DVECTOR3[iVertNum];
			bReleaseVerts = true;	
			for(int j=0; j<iOriginVertNum; j++)
			{
				Verts[j] = (1.0f - m_fClothSimWeight) * pAnimVertInfo->mAnimPositions[j] + m_fClothSimWeight * APhysXConverter::N2A_Vector3(pClothPhys->GetVertexPositions()[j]);
			}

			for(int j = iOriginVertNum; j<iVertNum; j++)
			{
				Verts[j] = APhysXConverter::N2A_Vector3(pClothPhys->GetVertexPositions()[j]);
			}

			if(m_fClothSimWeight<1e-4f)
			{
				Normals = new A3DVECTOR3[iVertNum];
				bReleaseNormals = true;	

				for(int j=0; j<iOriginVertNum; j++)
				{
					Normals[j] =  pAnimVertInfo->mAnimNormals[j];
				}

				for(int j = iOriginVertNum; j<iVertNum; j++)
				{
					Normals[j] = APhysXConverter::N2A_Vector3(pClothPhys->GetVertexNormals()[j]);
				}
			}
			else
			{
				Normals = (A3DVECTOR3*)pClothPhys->GetVertexNormals();
			}
		}
		
		pClothMesh->UpdateMeshData(
			pClothPhys->GetVertexNum(), 
			pClothPhys->GetIndexNum(),
			Verts, 
			Normals,
			(const int*)pClothPhys->GetIndices(), 
			pClothPhys->GetParentIndexNum(), 
			(const int*)pClothPhys->GetParentIndices());

		if(bReleaseVerts)
		{
			delete [] Verts;
		}
		if(bReleaseNormals)
		{
			delete [] Normals;
		}

	}

#ifdef _ANGELICA21
	
	gGetAPhysXScene()->GetPerfStats()->StartPerfStat("adheremesh sync");
	for (i = 0; i < m_aAdhereMeshes.GetSize(); i++)
	{
		m_aAdhereMeshes[i]->SyncSrcMesh();
	}
	
	gGetAPhysXScene()->GetPerfStats()->EndPerfStat();
#endif

	if(!m_bIgnoreClothSimBlendCounter)
	{
		//----------------------------------------------------------
		// Note by Wenfeng, Dec. 19, 2012
		// compute the blend weight here
		if(m_iClothSimBlendCounter > m_iClothSimBlendTime + m_iClothSimDelayTime)
		{
			m_fClothSimWeight = 1.0f;
		}
		else if(m_iClothSimBlendCounter < m_iClothSimDelayTime)
		{
			m_fClothSimWeight = 0.0f;
			m_iClothSimBlendCounter += (int)(gGetAPhysXScene()->GetExactSimDt() * 1000);
		}
		else
		{
			if(m_iClothSimBlendTime>0)
				m_fClothSimWeight = (m_iClothSimBlendCounter - m_iClothSimDelayTime) / (float)m_iClothSimBlendTime;
			else
				m_fClothSimWeight = 1.0f;

			m_iClothSimBlendCounter += (int)(gGetAPhysXScene()->GetExactSimDt() * 1000);		
		}
	}


#if 0
	
	AString strInfo;
	strInfo.Format("BlendCounter: %d, Weight: %f\n", m_iClothSimBlendCounter, m_fClothSimWeight);
	OutputDebugStringA(strInfo);

#endif

	return true;
}

//	Get specified RB actor
//	matActor (out): store actor's transform matrix
APhysXRigidBodyObject* A3DSkinPhysSync::GetRBActor(int iActorIdx)
{
	if (!IsSkinBound())
		return NULL;

	A3DSkinPhysSyncData::ACTOR_DATA* pActorData = m_pCoreData->GetActorByIndex(iActorIdx);
	if (pActorData)
		return m_pPhysSkin->GetAttacherObject(pActorData->strName);
	else
		return NULL;
}

void A3DSkinPhysSync::SetClothWindAcceleration(const A3DVECTOR3& vWindAcceleration)
{
	if(m_pA3DSkin && m_pPhysSkin )
	{
		NxVec3 vWindAcc = APhysXConverter::A2N_Vector3(vWindAcceleration);

		int i, iNumMesh = m_pA3DSkin->GetClothMeshNum();

		for (i=0; i < iNumMesh; i++)
		{			
#ifdef _ANGELICA21
			if (i < m_pCoreData->GetMeshNum())
			{
				A3DSkinPhysSyncData::MESH_DATA* pMeshData = m_pCoreData->GetMeshByIndex(i);
				if (pMeshData && pMeshData->GetAdhereParams())
					continue;
			}
#endif
			A3DClothMeshImp* pClothMesh = m_pA3DSkin->GetClothMeshImp(i);
			APhysXClothObject* pClothPhys = pClothMesh ? m_pPhysSkin->GetClothObject(pClothMesh->GetName()) : NULL;

			if (pClothPhys)
				pClothPhys->SetWindAcceleration(vWindAcc);
		}

	}
}

void A3DSkinPhysSync::EnableClothSimulation(bool bEnable /*= true*/)
{
	if(m_pA3DSkin && m_pPhysSkin )
	{
		int i, iNumMesh = m_pA3DSkin->GetClothMeshNum();

		for (i=0; i < iNumMesh; i++)
		{			
#ifdef _ANGELICA21
			if (i < m_pCoreData->GetMeshNum())
			{
				A3DSkinPhysSyncData::MESH_DATA* pMeshData = m_pCoreData->GetMeshByIndex(i);
				if (pMeshData && pMeshData->GetAdhereParams())
					continue;
			}
#endif
			A3DClothMeshImp* pClothMesh = m_pA3DSkin->GetClothMeshImp(i);
			APhysXClothObject* pClothPhys = pClothMesh ? m_pPhysSkin->GetClothObject(pClothMesh->GetName()) : NULL;

			if (pClothPhys)
			{
#if 0
				if(bEnable)
				{
					m_bIgnoreClothSimBlendCounter = false;
					m_iClothSimBlendCounter = m_iClothSimBlendTime + m_iClothSimDelayTime - 100;
					
				}
				else
				{
					m_bIgnoreClothSimBlendCounter = true;
					m_fClothSimWeight = 0.0f;
				}

#else
				NxCloth* pNxCloth = pClothPhys->GetNxCloth();
				if(pNxCloth)
				{
					NxU32 uFlags = pNxCloth->getFlags();
					uFlags = bEnable? (uFlags & ~NX_CLF_STATIC):(uFlags | NX_CLF_STATIC);
					gGetAPhysXScene()->LockWritingScene();
					pNxCloth->setFlags(uFlags);
					gGetAPhysXScene()->UnlockWritingScene();
				}
#endif
			}
		}

	}
}

#endif	//	A3D_PHYSX

