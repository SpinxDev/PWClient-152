/*
 * FILE: A3DSkinPhysSyncData.cpp
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


#include <ALog.h>
#include <A3DSkinModel.h>
#include <A3DSkin.h>
#include <A3DSkinMesh.h>
#include <A3DRigidMesh.h>
#include <ACSWrapper.h>


///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------
// SKINPHYSFILE_VERSION:
//		 from 6 to 7: Add Cloth Animation Constraint Info...
//       from 7 to 8: Add adhere mesh parameter info;
//       from 8 to 9: Add adhere weight in adhere mesh parameter info;

#ifdef _ANGELICA21
#define SKINPHYSFILE_VERSION	9
#else
#define SKINPHYSFILE_VERSION	7
#endif


///////////////////////////////////////////////////////////////////////////
//	
//	Reference to External variables and functions
//	
///////////////////////////////////////////////////////////////////////////

//	File header when SKINPHYSFILE_VERSION < 6
struct SKINPHYSFILE_HEADER
{
	int		iNumActor;			//	Actor number
	int		iNumMesh;			//	Mesh number
	int		iSkinMeshNum;		//	How many meshes are skin mesh
	int		iRigidMeshNum;		//	How many meshes are rigid mesh
	bool	bBindInit;			//	true, physical data has been prepared
};

//	File header when SKINPHYSFILE_VERSION >= 6
struct SKINPHYSFILE_HEADER6
{
	int		iNumActor;			//	Actor number
	int		iNumMesh;			//	Mesh number
	int		iSkinMeshNum;		//	How many meshes are skin mesh
	int		iRigidMeshNum;		//	How many meshes are rigid mesh
	bool	bBindInit;			//	true, physical data has been prepared
	bool	bSoftLink;			//	Soft link flag
};

struct ACTOR_DESC
{
	A3DSkinPhysSyncData::ACTOR_DATA*	pSrcActor;	//	Source actor

	APhysXActorDesc*	pActorDesc;		//	Actor's physical description
	A3DVECTOR3			vPos;			//	Actor's position
	float				fBoundRad;		//	Radius of bounding ball that is made up by dragged vertices

	ACTOR_DESC() : vPos(0.0f)
	{
		pSrcActor	= NULL;
		pActorDesc	= NULL;
		fBoundRad	= 0.0f;
	}

	~ACTOR_DESC()
	{
		if (pActorDesc)
		{
			delete pActorDesc;
			pActorDesc = NULL;
		}
	}
};

///////////////////////////////////////////////////////////////////////////
//	
//	Local Types and Variables and Global variables
//	
///////////////////////////////////////////////////////////////////////////

A3DSkinPhysSyncDataMan	g_SkinPhysDataMan;

///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Implement A3DSkinPhysSyncData::ACTOR_DATA
//	
///////////////////////////////////////////////////////////////////////////


A3DSkinPhysSyncData::ACTOR_DATA::ACTOR_DATA()
{
	fBoundRad	= 0.0f;
	bHHIsBone	= false;
}

A3DSkinPhysSyncData::ACTOR_DATA::ACTOR_DATA(const ACTOR_DATA& rhs) :
A3DPhysActor(rhs),
strHHHook(rhs.strHHHook),
bHHIsBone(rhs.bHHIsBone),
fBoundRad(rhs.fBoundRad)
{
	int i, nDraggedVertsNum = rhs.aDraggedVerts.GetSize();
	for (i = 0 ; i < nDraggedVertsNum ; ++i) {
		DWORD dwTmp = rhs.aDraggedVerts[i];
		aDraggedVerts.Add(dwTmp);
	}
}

A3DSkinPhysSyncData::ACTOR_DATA::~ACTOR_DATA()
{
}

A3DSkinPhysSyncData::ACTOR_DATA& A3DSkinPhysSyncData::ACTOR_DATA::operator = (const ACTOR_DATA& rhs)
{
	if (this == &rhs)
		return *this;

	A3DPhysActor::operator =(rhs);
	strHHHook = rhs.strHHHook;
	bHHIsBone = rhs.bHHIsBone;
	fBoundRad = rhs.fBoundRad;
	
	aDraggedVerts.RemoveAll();
	int i, nDraggedVertsNum = rhs.aDraggedVerts.GetSize();
	for (i = 0 ; i < nDraggedVertsNum ; ++i) {
		DWORD dwTmp = rhs.aDraggedVerts[i];
		aDraggedVerts.Add(dwTmp);
	}

	return *this;
}

A3DPhysActor* A3DSkinPhysSyncData::ACTOR_DATA::Clone() const
{
	return new ACTOR_DATA(*this);
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement A3DSkinPhysSyncData::MESH_DATA
//	
///////////////////////////////////////////////////////////////////////////

A3DSkinPhysSyncData::MESH_DATA::MESH_DATA()
{
	iNumVert	= 0;
	iNumIndex	= 0;
	bRigid		= false;
	iBoneIdx	= -1;
	aVerts		= NULL;
	aIndices	= NULL;
	
	aClothVertexConstraints = NULL;

	pClothDesc	= new APhysXClothObjectDesc;
	// Special Cloth Parameter : Define the weight of physical calculation when there is a MOVEMENT of cloth
	pClothDesc->mClothParameters.mMinAdhereVelocity = .35f;

#ifdef _ANGELICA21
	aAdherParams = NULL;
	strAdhereMeshName.Empty();
#endif
}

A3DSkinPhysSyncData::MESH_DATA::~MESH_DATA()
{
	if (pClothDesc)
		delete pClothDesc;

	if (aVerts)
		delete [] aVerts;

	if (aIndices)
		delete [] aIndices;

	if(aClothVertexConstraints)
		delete [] aClothVertexConstraints;

#ifdef _ANGELICA21
	if (aAdherParams)
		delete [] aAdherParams;
#endif
}

//	Get vertex position by index
bool A3DSkinPhysSyncData::MESH_DATA::GetVertexPos(int n, A3DVECTOR3& vPos) const
{
	if (n >= 0 && n < iNumVert && aVerts)
	{
		vPos = aVerts[n];
		return true;
	}
	else
	{
		ASSERT(n >= 0 && n < iNumVert);
		return false;
	}
}

//	Set vertex position by index
void A3DSkinPhysSyncData::MESH_DATA::SetVertexPos(int n, const A3DVECTOR3& vPos)
{
	if (n >= 0 && n < iNumVert && aVerts)
		aVerts[n] = vPos;
	else
	{
		ASSERT(n >= 0 && n < iNumVert);
	}
}

//	Get vertex index
int A3DSkinPhysSyncData::MESH_DATA::GetIndex(int n) const
{
	if (n >=0 && n < iNumIndex && aIndices)
		return aIndices[n];
	else
	{
		ASSERT(n >=0 && n < iNumIndex && aIndices);
		return -1;
	}
}

//	Get cloth parameters
APhysXClothParameters* A3DSkinPhysSyncData::MESH_DATA::GetClothParams() const
{
	return &pClothDesc->mClothParameters;
}

//	Set cloth parameters
void A3DSkinPhysSyncData::MESH_DATA::SetClothParams(const APhysXClothParameters* pSrc)
{
	pClothDesc->mClothParameters = *pSrc;
}

#ifdef _ANGELICA21
void A3DSkinPhysSyncData::MESH_DATA::SetAdhereParams(A3DAdhereMesh::AdhereParam* params)
{
	if(aAdherParams)
	{
		delete [] aAdherParams;
		aAdherParams = NULL;
	}

	if (params)
	{
		aAdherParams = new A3DAdhereMesh::AdhereParam[iNumVert];
		memcpy(aAdherParams, params, iNumVert * sizeof(A3DAdhereMesh::AdhereParam));	
	}	
}
#endif


///////////////////////////////////////////////////////////////////////////
//	
//	Implement A3DSkinPhysSyncData
//	
///////////////////////////////////////////////////////////////////////////

A3DSkinPhysSyncData::A3DSkinPhysSyncData()
{
	m_iInitBindErr	= ERR_OK;
	m_bBindInit		= false;
	m_pSkinPhysDesc	= NULL;
	m_iVersion		= SKINPHYSFILE_VERSION;
	m_bSoftLink		= true;
}

A3DSkinPhysSyncData::~A3DSkinPhysSyncData()
{
	int i;

	//	Release all source mesh info
	for (i=0; i < m_aRigidMeshInfo.GetSize(); i++)
		delete m_aRigidMeshInfo[i];
	
	m_aRigidMeshInfo.RemoveAll();

	for (i=0; i < m_aSkinMeshInfo.GetSize(); i++)
		delete m_aSkinMeshInfo[i];

	m_aSkinMeshInfo.RemoveAll();

	//	Release physical description
	if (m_pSkinPhysDesc)
	{
		delete m_pSkinPhysDesc;
		m_pSkinPhysDesc = NULL;
	}

	//	Release all meshes
	ReleaseMeshes();

	//	Release all actor data
	for (i=0; i < m_aActors.GetSize(); i++)
	{
		ACTOR_DATA* pActor = m_aActors[i];
		delete pActor;
	}

	m_aActors.RemoveAll();
}

//	Record info of skin meshes that will create cloth data
void A3DSkinPhysSyncData::RecordSourceMeshInfo(A3DSkin* pSkin, const AArray<int>& aRigidMeshIdx, const AArray<int>& aSkinMeshIdx)
{
	int i;

	//	Release all source mesh info
	for (i=0; i < m_aRigidMeshInfo.GetSize(); i++)
		delete m_aRigidMeshInfo[i];
	
	m_aRigidMeshInfo.RemoveAll();

	for (i=0; i < m_aSkinMeshInfo.GetSize(); i++)
		delete m_aSkinMeshInfo[i];

	m_aSkinMeshInfo.RemoveAll();

	//	Record new names
	for (i=0; i < aRigidMeshIdx.GetSize(); i++)
	{
		int idx = aRigidMeshIdx[i];
		A3DRigidMesh* pMesh = pSkin->GetRigidMesh(idx);

		SRC_MESH* pInfo = new SRC_MESH;
		pInfo->iIndex = idx;
		pInfo->strName = pMesh->GetName();
		m_aRigidMeshInfo.Add(pInfo);
	}

	for (i=0; i < aSkinMeshIdx.GetSize(); i++)
	{
		int idx = aSkinMeshIdx[i];
		A3DSkinMesh* pMesh = pSkin->GetSkinMesh(idx);

		SRC_MESH* pInfo = new SRC_MESH;
		pInfo->iIndex = idx;
		pInfo->strName = pMesh->GetName();
		m_aSkinMeshInfo.Add(pInfo);
	}
}

//	Build meshes from skin
bool A3DSkinPhysSyncData::BuildMeshFromSkin(A3DSkinModel* pSkinModel, A3DSkin* pSkin, 
								const AArray<int>& aRigidMeshIdx, const AArray<int>& aSkinMeshIdx)
{
	if (!pSkin)
	{
		ASSERT(pSkin);
		return false;
	}

	int i, j;

	//	Record names for candidate meshes
	RecordSourceMeshInfo(pSkin, aRigidMeshIdx, aSkinMeshIdx);

	//	Collect skin meshes
	APtrArray<A3DMeshBase*> aSkinMeshes;
	if (!CollectSkinMesh(pSkin, aSkinMeshes, false))
		return false;

	int iNumNewMesh = aSkinMeshes.GetSize();
	if (!iNumNewMesh)
	{
		ReleaseMeshes();
		return true;
	}

	//	Initialize cloth and adhere parameter array
	APtrArray<APhysXClothParameters*> aClothParams;

#ifdef _ANGELICA21
	abase::hash_map<int, A3DAdhereMesh::AdhereParam*> mapParams;
	abase::hash_map<int, AString> mapAdhereNames;
#endif
	
	
	aClothParams.SetSize(iNumNewMesh, 16);

	for (i=0; i < iNumNewMesh; i++)
	{
		aClothParams[i] = NULL;
	}

	//	Try to keep cloth and adhere parameters
	for (i=0; i < m_aMeshes.GetSize(); i++)
	{
		MESH_DATA* pMesh = m_aMeshes[i];
		A3DMeshBase* pSkinMesh = NULL;

		for (j=0; j < iNumNewMesh; j++)
		{
			if (!_stricmp(pMesh->strName, aSkinMeshes[j]->GetName()))
			{
				pSkinMesh = aSkinMeshes[j];
				break;
			}
		}

		if (pSkinMesh && CompareMesh(pMesh, pSkinMesh))
		{
			APhysXClothParameters* pParam = aClothParams[j];
			if (!pParam)
			{
				pParam = new APhysXClothParameters;
				*pParam = *pMesh->GetClothParams();
				aClothParams[j] = pParam;
			}

#ifdef _ANGELICA21
			if (pMesh->aAdherParams)
			{
				A3DAdhereMesh::AdhereParam* p = new A3DAdhereMesh::AdhereParam[pMesh->iNumVert];
				if (mapParams.find(j) != mapParams.end())
				{
					delete [] mapParams[j]; 
				}
				memcpy(p, pMesh->aAdherParams, sizeof(A3DAdhereMesh::AdhereParam) * pMesh->iNumVert);
				mapParams[j] = p;
				mapAdhereNames[j] = pMesh->strAdhereMeshName;
			}
#endif
		}
	}

	//	Release all old meshes
	ReleaseMeshes();

	bool bSuccess = true;

	if (iNumNewMesh)
	{
		A3DSkeleton* pSkeleton = pSkinModel->GetSkeleton();

		AArray<A3DMATRIX4> aMats;
		pSkinModel->GetSkeleton()->GetOriginBoneState(aMats, false);
		ASSERT(aMats.GetSize() == pSkeleton->GetBoneNum());

		//	Add bone init matrix
		for (i=0; i < aMats.GetSize(); i++)
		{
			A3DBone* pBone = pSkeleton->GetBone(i);
			aMats[i] = pBone->GetBoneInitTM() * aMats[i];
		}

		//	Re-arrange mesh array
		m_aMeshes.SetSize(0, iNumNewMesh);

		for (i=0; i < iNumNewMesh; i++)
		{
			MESH_DATA* pMesh = CreateMeshData(pSkin, aSkinMeshes[i], aMats);
			if (!pMesh)
			{
				bSuccess = false;
				break;
			}

			m_aMeshes.Add(pMesh);

			if (aClothParams[i])
				pMesh->SetClothParams(aClothParams[i]);

#ifdef _ANGELICA21
			if (mapAdhereNames.find(i) != mapAdhereNames.end())
			{
				pMesh->strAdhereMeshName = mapAdhereNames[i];
				pMesh->aAdherParams = mapParams[i];
			}
#endif
			
		}
	}

	//	Release cloth parameters
	for (i=0; i < aClothParams.GetSize(); i++)
	{
		if (aClothParams[i])
		{
			delete aClothParams[i];
			aClothParams[i] = NULL;
		}
	}

	if (!bSuccess)
		return false;

	return true;
}

//	Release all meshes
void A3DSkinPhysSyncData::ReleaseMeshes()
{
	for (int i=0; i < m_aMeshes.GetSize(); i++)
	{
		delete m_aMeshes[i];
	}

	m_aMeshes.RemoveAll();
}

//	Collect available skin mesh
//	bCheckName: true, check name collision
bool A3DSkinPhysSyncData::CollectSkinMesh(A3DSkin* pSkin, APtrArray<A3DMeshBase*>& aMeshes, bool bCheckName)
{
	int i, j;

	//	Get all skin meshes
	for (i=0; i < m_aSkinMeshInfo.GetSize(); i++)
	{
		SRC_MESH& info = *m_aSkinMeshInfo[i];
		if (info.iIndex < 0 || info.iIndex >= pSkin->GetSkinMeshNum())
			continue;

		//	Try to get mesh from skin
		A3DMeshBase* pSkinMesh = pSkin->GetSkinMesh(info.iIndex);
		if (!pSkinMesh || info.strName.CompareNoCase(pSkinMesh->GetName()) != 0)
		{
			int iNum = pSkin->GetSkinMeshNum();
			for (j=0; j < iNum; j++)
			{
				A3DMeshBase* pTempMesh = pSkin->GetSkinMesh(j);
				if (info.strName.CompareNoCase(pTempMesh->GetName()) == 0)
				{
					pSkinMesh = pTempMesh;
					info.iIndex = j;	//	Update mesh index
					break;
				}
			}

			if (j >= iNum)
				return false;
		}

		//	Check mesh name collision
		if (bCheckName && GetMeshByName(pSkinMesh->GetName()))
		{
			aMeshes.RemoveAll();
			return false;
		}

		aMeshes.Add(pSkinMesh);
	}

	//	Get all rigid meshes
	for (i=0; i < m_aRigidMeshInfo.GetSize(); i++)
	{
		SRC_MESH& info = *m_aRigidMeshInfo[i];

		//	Try to get mesh from skin
		A3DMeshBase* pRigidMesh = pSkin->GetRigidMesh(info.iIndex);
		if (!pRigidMesh || info.strName.CompareNoCase(pRigidMesh->GetName()) != 0)
		{
			int iNum = pSkin->GetRigidMeshNum();
			for (j=0; j < iNum; j++)
			{
				A3DMeshBase* pTempMesh = pSkin->GetRigidMesh(j);
				if (info.strName.CompareNoCase(pTempMesh->GetName()) == 0)
				{
					pRigidMesh = pTempMesh;
					info.iIndex = j;	//	Update mesh index
					break;
				}
			}

			if (j >= iNum)
				return false;
		}

		//	Check mesh name collision
		if (bCheckName && GetMeshByName(pRigidMesh->GetName()))
		{
			aMeshes.RemoveAll();
			return false;
		}

		aMeshes.Add(pRigidMesh);
	}

	return true;
}

//	Check if mesh match
bool A3DSkinPhysSyncData::CompareMesh(MESH_DATA* pMesh, A3DMeshBase* pMeshBase)
{
	ASSERT(pMesh && pMeshBase);

	//	Check vertex and index number
	if (pMesh->iNumVert != pMeshBase->GetVertexNum() ||
		pMesh->iNumIndex != pMeshBase->GetIndexNum())
		return false;

	//	Check name
	if (pMesh->strName.CompareNoCase(pMeshBase->GetName()))
		return false;

	int i;

	//	Check index data
	if (pMeshBase->GetClassID() == A3D_CID_SKINMESH)
	{
		if (pMesh->bRigid)
			return false;

		const WORD* aSrcIndices = ((A3DSkinMesh*)pMeshBase)->GetOriginIndexBuf();
		for (i=0; i < pMesh->iNumIndex; i++)
		{
			if (pMesh->aIndices[i] != aSrcIndices[i])
				return false;
		}
	}
	else if (pMeshBase->GetClassID() == A3D_CID_RIGIDMESH)
	{
		if (!pMesh->bRigid)
			return false;	

		const WORD* aSrcIndices = ((A3DRigidMesh*)pMeshBase)->GetOriginIndexBuf();
		for (i=0; i < pMesh->iNumIndex; i++)
		{
			if (pMesh->aIndices[i] != aSrcIndices[i])
				return false;
		}
	}
	else
	{
		ASSERT(0);
		return false;
	}

	return true;
}

//	Create mesh data from specified skinmesh
A3DSkinPhysSyncData::MESH_DATA* A3DSkinPhysSyncData::CreateMeshData(A3DSkin* pSkin, A3DMeshBase* pMeshBase, 
														AArray<A3DMATRIX4>& aBlendMats)
{
	ASSERT(pMeshBase);

	MESH_DATA* pMesh = new MESH_DATA;
	if (!pMesh)
		return NULL;

	pMesh->strName		= pMeshBase->GetName();
	pMesh->iNumVert		= pMeshBase->GetVertexNum();
	pMesh->iNumIndex	= pMeshBase->GetIndexNum();

	if (pMeshBase->GetClassID() == A3D_CID_SKINMESH)
		pMesh->bRigid = false;
	else if (pMeshBase->GetClassID() == A3D_CID_RIGIDMESH)
		pMesh->bRigid = true;
	else
	{
		ASSERT(0);
		return NULL;
	}

	pMesh->aVerts = new A3DVECTOR3[pMesh->iNumVert];
	if (!pMesh->aVerts)
		return false;

	pMesh->aIndices = new int [pMesh->iNumIndex];
	if (!pMesh->aIndices)
	{
		delete [] pMesh->aVerts;
		return false;
	}

	//	Create mesh vertices
	if (!CreateScaledMesh(pSkin, pMeshBase, aBlendMats, pMesh, pMesh->aVerts, &pMesh->aBlendBones))
	{
		delete [] pMesh->aVerts;
		delete [] pMesh->aIndices;
		return false;
	}

	//	Copy indices
	if (pMeshBase->GetClassID() == A3D_CID_SKINMESH)
	{
		const WORD* aSrcIndices = ((A3DSkinMesh*)pMeshBase)->GetOriginIndexBuf();
		for (int i=0; i < pMesh->iNumIndex; i++)
			pMesh->aIndices[i] = aSrcIndices[i];
	}
	else if (pMeshBase->GetClassID() == A3D_CID_RIGIDMESH)
	{
		const WORD* aSrcIndices = ((A3DRigidMesh*)pMeshBase)->GetOriginIndexBuf();
		for (int i=0; i < pMesh->iNumIndex; i++)
			pMesh->aIndices[i] = aSrcIndices[i];
	}

	return pMesh;
}

//	Get mesh data by name
A3DSkinPhysSyncData::MESH_DATA* A3DSkinPhysSyncData::GetMeshByName(const char* szName, int* piIndex/* NULL */) const
{
	for (int i=0; i < m_aMeshes.GetSize(); i++)
	{
		MESH_DATA* pMesh = m_aMeshes[i];
		if (!pMesh->strName.CompareNoCase(szName))
		{
			if (piIndex)
				*piIndex = i;

			return pMesh;
		}
	}

	return NULL;
}

//	Initialize bind data
//	When call InitBind(), ensure pSkinModel has been reset to initial state by pSkinModel->ResetToInitPose()
bool A3DSkinPhysSyncData::InitBind(APhysXScene* pPhysScene, A3DSkinModel* pSkinModel)
{
	m_bBindInit = false;
	m_iInitBindErr = ERR_OK;

	int i;

	//	Check link info
	for (i=0; i < m_aActors.GetSize(); i++)
	{
		ACTOR_DATA* pActor = m_aActors[i];

		if (!pActor->strHHHook.GetLength())
		{
			m_iInitBindErr = ERR_ACTOR_NOTLINK;
			return false;
		}

		//	Get hh-type hook
		if (pActor->bHHIsBone)
		{
			if (!pSkinModel->GetSkeleton()->GetBone(pActor->strHHHook, NULL))
			{
				m_iInitBindErr = ERR_NO_HHHOOK;
				return false;
			}
		}
		else
		{
			if (!pSkinModel->GetSkeleton()->GetHook(pActor->strHHHook, NULL))
			{
				m_iInitBindErr = ERR_NO_HHHOOK;
				return false;
			}
		}
	}

	//	Create physical desc
	if (!CreateSkinPhysDesc(pPhysScene))
		return false;

	m_bBindInit = true;

	return true;
}

//	Calculate dragged vertices for actors
bool A3DSkinPhysSyncData::CalcActorDraggedVerts(APhysXClothAttacher* pAttacherObject)
{
	int i, j, m;

	for (i=0; i < m_aActors.GetSize(); i++)
	{
		ACTOR_DATA* pActorData = m_aActors[i];
		pActorData->aDraggedVerts.RemoveAll(false);
		pActorData->fBoundRad = 0.0f;

		APhysXRigidBodyObject* pAttacher = pAttacherObject->GetAttacherObject(pActorData->strName);
		if (pAttacher->GetNxActorNum() != 1)
		{
			ASSERT(pAttacher->GetNxActorNum() == 1);
			continue;
		}
		
		NxActor* pActor = pAttacher->GetNxActor(0);
		A3DVECTOR3 vCenter = APhysXConverter::N2A_Vector3(pActor->getGlobalPosition());

		for (m=0; m < m_aMeshes.GetSize(); m++)
		{
			MESH_DATA* pMeshData = m_aMeshes[m];
			int iNumVert = pMeshData->GetVertexNum();

			for (j=0; j < iNumVert; j++)
			{
				const A3DVECTOR3& vPos = pMeshData->aVerts[j];

				if (APhysXUtilLib::IsPointInNxActor(pActor, APhysXConverter::A2N_Vector3(vPos)))
				{
					DWORD dwIndex = (DWORD)((m << 16) | j);
					pActorData->aDraggedVerts.Add(dwIndex);

					A3DVECTOR3 vDelta = vPos - vCenter;
					float fRadius = DotProduct(vDelta, vDelta);
					if (pActorData->fBoundRad < fRadius)
						pActorData->fBoundRad = fRadius;
				}
			}
		}

		if (pActorData->fBoundRad != 0.0f)
			pActorData->fBoundRad = (float)sqrt(pActorData->fBoundRad);
	}

	return true;
}

#define ENABLE_CONSTRAINED_CLOTH

//	Create skin's cloth data. Called by InitBind()
bool A3DSkinPhysSyncData::CreateSkinPhysDesc(APhysXScene* pPhysScene)
{
	//	Release old data
	if (m_pSkinPhysDesc)
	{
		delete m_pSkinPhysDesc;
		m_pSkinPhysDesc = NULL;
	}

#ifndef ENABLE_CONSTRAINED_CLOTH

	if (!m_aActors.GetSize())
	{
		m_iInitBindErr = ERR_NO_ACTOR;
		return false;
	}

#endif

	if (!m_aMeshes.GetSize())
	{
		m_iInitBindErr = ERR_NO_MESH;
		return false;
	}

	//	Create skin physical desc
	m_pSkinPhysDesc = new APhysXClothAttacherDesc;
	if (!m_pSkinPhysDesc)
	{
		m_iInitBindErr = ERR_NO_MEMORY;
		return false;
	}

	//	Create rigid body description
	int i, j, iCount=0;

	//	Add actors to rigid body
	for (i=0; i < m_aActors.GetSize(); i++)
	{
		ACTOR_DATA* pActor = m_aActors[i];

		//	Do some check
		if (!pActor->pActorDesc || !pActor->pMaterial || !pActor->aShapes.GetSize())
			continue;

		APhysXSkeletonRBObjectDesc RBdesc;
		APhysXArray<APhysXShapeDesc*> aShapes;

		//	Push shapes
		aShapes.clear();
		for (j=0; j < pActor->aShapes.GetSize(); j++)
			aShapes.push_back(pActor->aShapes[j]);

		pActor->pActorDesc->SetName(pActor->strName);

		//	Try to add actor
		RBdesc.AddActorComponent(*pActor->pActorDesc, *pActor->pMaterial, aShapes);

		m_pSkinPhysDesc->AddMultiAttacherComponent(&RBdesc, pActor->strName);

		iCount++;
	}

#ifndef ENABLE_CONSTRAINED_CLOTH
	
	//	Check valid actor number
	if (!iCount)
	{
		m_iInitBindErr = ERR_NO_ACTOR;
		return false;
	}

#endif

	//	Create cloth description
	for (i=0; i < m_aMeshes.GetSize(); i++)
	{
		MESH_DATA* pMesh = m_aMeshes[i];

#ifdef _ANGELICA21
		if (pMesh->aAdherParams)
			continue;// according to adhered mesh
#endif

		//	Setup mesh desc
		NxClothMeshDesc MeshDesc;
		MeshDesc.numVertices			= pMesh->GetVertexNum();
		MeshDesc.numTriangles			= pMesh->GetIndexNum() / 3;
		MeshDesc.pointStrideBytes		= sizeof (A3DVECTOR3);
		MeshDesc.triangleStrideBytes	= sizeof (int) * 3;
		MeshDesc.points					= pMesh->aVerts;
		MeshDesc.triangles				= pMesh->aIndices;

		MeshDesc.flags |= NX_CLOTH_MESH_WELD_VERTICES;
		MeshDesc.weldingDistance = 0.001f;
		//	Setup cloth mesh
		pMesh->pClothDesc->InitClothMesh(MeshDesc);

		pMesh->pClothDesc->SetClothVertexConstraints(pMesh->GetClothVertexConstraints());
		m_pSkinPhysDesc->AddMultiClothComponent(pMesh->pClothDesc, pMesh->GetMeshName());
	}

	// m_pSkinPhysDesc->CompleteInnerConnectorInfo();

	//	We will try to create a cloth attacher object to check if all data
	//	are set valid.
	APhysXObjectInstanceDesc apxObjInstanceDesc;
	apxObjInstanceDesc.mPhysXObjectDesc = m_pSkinPhysDesc;
	apxObjInstanceDesc.mGlobalPose.id();
	apxObjInstanceDesc.mScale3D = NxVec3(1.0f);

	ASSERT(pPhysScene);

	APhysXClothAttacher* pAttacherObject = (APhysXClothAttacher*)pPhysScene->CreatePhysXObject(apxObjInstanceDesc);
	if (!pAttacherObject)
	{
		m_iInitBindErr = ERR_FAILED_CREATE;
		return false;
	}

	//	Decide dragged vertices for each actor
	CalcActorDraggedVerts(pAttacherObject);
		
	pPhysScene->ReleasePhysXObject(pAttacherObject);
	pAttacherObject = NULL;

	return true;
}

//	Create skin's cloth data. Called by Load()
bool A3DSkinPhysSyncData::CreateSkinPhysDesc_OnLoad()
{
	//	Check flag
	if (!m_bBindInit)
		return false;

	ASSERT(!m_pSkinPhysDesc);

	//	Create skin physical desc
	m_pSkinPhysDesc = new APhysXClothAttacherDesc;
	if (!m_pSkinPhysDesc)
		return false;

	//	Create rigid body description
	int i, j;

	//	Add actors to rigid body
	for (i=0; i < m_aActors.GetSize(); i++)
	{
		ACTOR_DATA* pActor = m_aActors[i];

		//	Do some check
		if (!pActor->pActorDesc || !pActor->pMaterial || !pActor->aShapes.GetSize())
			continue;

		APhysXSkeletonRBObjectDesc RBdesc;
		APhysXArray<APhysXShapeDesc*> aShapes;

		//	Push shapes
		aShapes.clear();
		for (j=0; j < pActor->aShapes.GetSize(); j++)
			aShapes.push_back(pActor->aShapes[j]);

		pActor->pActorDesc->SetName(pActor->strName);

		//	Try to add actor
		RBdesc.AddActorComponent(*pActor->pActorDesc, *pActor->pMaterial, aShapes);

		m_pSkinPhysDesc->AddMultiAttacherComponent(&RBdesc, pActor->strName);
	}

	//	Create cloth description
	for (i=0; i < m_aMeshes.GetSize(); i++)
	{
		MESH_DATA* pMesh = m_aMeshes[i];
		ASSERT(pMesh->pClothDesc);
#ifdef _ANGELICA21
		if (pMesh->aAdherParams == NULL)
			m_pSkinPhysDesc->AddMultiClothComponent(pMesh->pClothDesc, pMesh->GetMeshName());
#endif
		
	}

	// m_pSkinPhysDesc->CompleteInnerConnectorInfo();

	return true;
}

//	Create skin's physical description with new scale
bool A3DSkinPhysSyncData::CreateScaledSkinPhysDesc(A3DSkinModel* pSkinModel, A3DSkin* pSkin, 
										APhysXScene* pPhysScene, APhysXClothAttacherDesc* pPhysDesc)
{
	if (!pPhysDesc || !m_bBindInit)
		return false;

	int i, j;
	int iMaxVertNum=0;

	//	Get maximum vertex number
	for (i=0; i < m_aMeshes.GetSize(); i++)
	{
		MESH_DATA* pMesh = m_aMeshes[i];
		if (pMesh->GetVertexNum() > iMaxVertNum)
			iMaxVertNum = pMesh->GetVertexNum();
	}

	//	Allocate verex and index buffer
	A3DVECTOR3* aVerts = new A3DVECTOR3 [iMaxVertNum];
	if (!aVerts)
		return false;

	A3DSkeleton* pSkeleton = pSkinModel->GetSkeleton();

	//	Get bones' states in initial time
	AArray<A3DMATRIX4> aMats;
	pSkeleton->GetOriginBoneState(aMats, true);
	ASSERT(aMats.GetSize() == pSkeleton->GetBoneNum());

	//	Generate actor descriptions with scaled pose info
	AArray<ACTOR_DESC*> aActorDesc;
	for (i=0; i < m_aActors.GetSize(); i++)
	{
		ACTOR_DATA* pActor = m_aActors[i];

		//	Do some check
		if (!pActor->pActorDesc || !pActor->pMaterial || !pActor->aShapes.GetSize())
			continue;

		ACTOR_DESC* pNewActor = new ACTOR_DESC;
		pNewActor->pSrcActor = pActor;
		pNewActor->pActorDesc = pActor->pActorDesc->Clone();

		//	Only snap actor's position to hook's position, but not axises
		A3DVECTOR3 vActorPos;

		if (pActor->GetHHIsBone())
		{
			int iBoneIdx;
			A3DBone* pBone = pSkeleton->GetBone(pActor->GetHookName(), &iBoneIdx);
			if (!pBone)
				continue;

			vActorPos = aMats[iBoneIdx].GetRow(3);
		}
		else
		{
			A3DSkeletonHook* pHook = pSkeleton->GetHook(pActor->GetHookName(), NULL);
			if (!pHook)
				continue;

			int iBoneIdx = pHook->GetBone();
			if (iBoneIdx >= 0)
				vActorPos = pHook->GetHookTM().GetRow(3) * aMats[iBoneIdx];
			else
				vActorPos = pHook->GetHookTM().GetRow(3);
		}

		pNewActor->pActorDesc->mLocalPose.t = APhysXConverter::A2N_Vector3(vActorPos);
		pNewActor->vPos = vActorPos;

		aActorDesc.Add(pNewActor);
	}

	//	Add bone init matrix
	for (i=0; i < aMats.GetSize(); i++)
	{
		A3DBone* pBone = pSkeleton->GetBone(i);
		aMats[i] = pBone->GetBoneInitTM() * aMats[i];
	}

	//	Create mesh descriptions
	for (i=0; i < m_aMeshes.GetSize(); i++)
	{
		MESH_DATA* pMeshData = m_aMeshes[i];
		A3DSkinMeshRef* pMeshRef = NULL;

		if (pMeshData->IsRigid())
			pMeshRef = pSkin->GetRigidMeshRefByName(pMeshData->GetMeshName());
		else
			pMeshRef = pSkin->GetSkinMeshRefByName(pMeshData->GetMeshName());

		if (!pMeshRef || !pMeshRef->pMesh)
			continue;

#ifdef _ANGELICA21
		if (pMeshData->aAdherParams)
			continue;// according to adhered mesh
#endif

		if (!CreateScaledMesh(pSkin, pMeshRef->pMesh, aMats, pMeshData, aVerts, NULL))
			continue;

		//	Setup mesh desc
		NxClothMeshDesc MeshDesc;
		MeshDesc.numVertices			= pMeshData->GetVertexNum();
		MeshDesc.numTriangles			= pMeshData->GetIndexNum() / 3;
		MeshDesc.pointStrideBytes		= sizeof (A3DVECTOR3);
		MeshDesc.triangleStrideBytes	= sizeof (int) * 3;
		MeshDesc.points					= aVerts;
		MeshDesc.triangles				= pMeshData->aIndices;

		MeshDesc.flags |= NX_CLOTH_MESH_WELD_VERTICES;
		MeshDesc.weldingDistance = 0.001f;
		//	Setup cloth mesh
		pMeshData->pClothDesc->InitClothMesh(MeshDesc);

		pMeshData->pClothDesc->SetClothVertexConstraints(pMeshData->GetClothVertexConstraints());
		pPhysDesc->AddMultiClothComponent(pMeshData->pClothDesc, pMeshData->GetMeshName());

		//	Calculate new actor bounding radius with scaled mesh data
		for (j=0; j < aActorDesc.GetSize(); j++)
		{
			ACTOR_DESC* pNewActor = aActorDesc[j];
			ACTOR_DATA* pSrcActor = pNewActor->pSrcActor;
			A3DVECTOR3 vDelta;

			for (int m=0; m < pSrcActor->aDraggedVerts.GetSize(); m++)
			{
				int iMeshIdx = (int)(pSrcActor->aDraggedVerts[m] >> 16);
				if (iMeshIdx == i)
				{
					int iVertIdx = (int)(pSrcActor->aDraggedVerts[m] & 0xffff);
					vDelta = aVerts[iVertIdx] - pNewActor->vPos;
					float fRadius = DotProduct(vDelta, vDelta);

					if (pNewActor->fBoundRad < fRadius)
						pNewActor->fBoundRad = fRadius;
				}
			}
		}
	}

	delete [] aVerts;

	//	Finish actor description and add them to object
	APhysXArray<APhysXShapeDesc*> aShapes;

	for (i=0; i < aActorDesc.GetSize(); i++)
	{
		ACTOR_DESC* pNewActor = aActorDesc[i];

		APhysXSkeletonRBObjectDesc RBdesc;

		//	Set actor scale factor
		float fScale = 1.0f;
		if (pNewActor->fBoundRad != 0.0f && pNewActor->pSrcActor->GetBoundRadius() != 0.0f)
			fScale = (float)sqrt(pNewActor->fBoundRad) / pNewActor->pSrcActor->GetBoundRadius();

		pNewActor->pActorDesc->mScale3D.set(fScale, fScale, fScale);

		//	Push shapes
		aShapes.clear();
		int iNumShape = pNewActor->pSrcActor->GetShapeNum();
		for (j=0; j < iNumShape; j++)
			aShapes.push_back(pNewActor->pSrcActor->GetShapeDesc(j));

		//	Try to add actor
		RBdesc.AddActorComponent(*pNewActor->pActorDesc, *pNewActor->pSrcActor->pMaterial, aShapes);

		pPhysDesc->AddMultiAttacherComponent(&RBdesc, pNewActor->pSrcActor->strName);

		//	Release resources
		delete pNewActor;
		aActorDesc[i] = NULL;
	}

	// pPhysDesc->CompleteInnerConnectorInfo();

	return true;
}

//	Create scaled mesh
bool A3DSkinPhysSyncData::CreateScaledMesh(A3DSkin* pSkin, A3DMeshBase* pMeshBase, AArray<A3DMATRIX4>& aBlendMats, 
								MESH_DATA* pMeshData, A3DVECTOR3* aOutVerts, AArray<int>* aBlendBones)
{
	ASSERT(pMeshBase);

	int i, j;
	int iNumVert = pMeshData->GetVertexNum();

	if (pMeshData->IsRigid())
	{
		A3DRigidMesh* pRigidMesh = (A3DRigidMesh*)pMeshBase;

		int iBoneIdx = pRigidMesh->GetBoneIndex();
		const A3DMATRIX4& mat = aBlendMats[iBoneIdx];

#ifdef _ANGELICA21
		RIGID_VERTEX* pSrcVerts = pRigidMesh->GetOriginVertexBuf();
#else
		A3DVERTEX* pSrcVerts = pRigidMesh->GetOriginVertexBuf();
#endif
		A3DVECTOR3 vSrcPos;

		for (i=0; i < iNumVert; i++)
		{
#ifdef _ANGELICA21
			RIGID_VERTEX* pSrc = &pSrcVerts[i];
#else
			A3DVERTEX* pSrc = &pSrcVerts[i];
#endif
			vSrcPos.Set(pSrc->x, pSrc->y, pSrc->z);
			aOutVerts[i] = vSrcPos * mat;
		}

		//	Record blend bone index
		if (aBlendBones)
			aBlendBones->UniquelyAdd(iBoneIdx);
	}
	else
	{
		A3DSkinMesh* pSkinMesh = (A3DSkinMesh*)pMeshBase;
		SKIN_VERTEX* pSrcVerts = pSkinMesh->GetOriginVertexBuf();

		A3DVECTOR3 vSrcPos;

		for (i=0; i < iNumVert; i++)
		{
			SKIN_VERTEX* pSrc = &pSrcVerts[i];

			vSrcPos.Set(pSrc->vPos[0], pSrc->vPos[1], pSrc->vPos[2]);

			A3DVECTOR3& vPos = aOutVerts[i];
			vPos.Clear();

			float fTotalWeight = 0.0f;

			for (j=0; j < 3; j++)
			{
				if (!pSrc->aWeights[j])
					break;

				//	Map bone index
				int iMat = (pSrc->dwMatIndices >> (j << 3)) & 0x000000ff;
				iMat = pSkin->MapBoneIndex(iMat);
				vPos += (aBlendMats[iMat] * vSrcPos) * pSrc->aWeights[j];

				fTotalWeight += pSrc->aWeights[j];

				//	Record blend bone index
				if (aBlendBones)
					aBlendBones->UniquelyAdd(iMat);
			}

			if (j >= 3)
			{
				fTotalWeight = 1.0f - fTotalWeight;
				if (fTotalWeight > 0.0f)
				{
					//	Map bone index
					int iMat = (pSrc->dwMatIndices & 0xff000000) >> 24;
					iMat = pSkin->MapBoneIndex(iMat);
					vPos += (aBlendMats[iMat] * vSrcPos) * fTotalWeight;

					//	Record blend bone index
					if (aBlendBones)
						aBlendBones->UniquelyAdd(iMat);
				}
			}
		}
	}

	return true;
}

//	Create a new actor
A3DSkinPhysSyncData::ACTOR_DATA* A3DSkinPhysSyncData::CreateActor(const char* szName, const A3DVECTOR3& vPos)
{
	if (!szName || !szName[0])
		return NULL;

	//	Check actor name collision
	ACTOR_DATA* pActor = GetActorByName(szName);
	if (pActor)
		return NULL;

	pActor = new ACTOR_DATA;
	pActor->strName = szName;

	//	Initialize actor position
	A3DMATRIX4 mat(A3DMATRIX4::IDENTITY);
	mat.SetRow(3, vPos);
	APhysXConverter::A2N_Matrix44(mat, pActor->pActorDesc->mLocalPose);

	//	Initialize material
	// pActor->pMaterial->mDynamicFriction = 0.1f;
	// pActor->pMaterial->mRestitution = 1.0f;
	//------------------------------------------------
	// change the default PhysX material values...
	pActor->pMaterial->mRestitution = 0.3f;
	pActor->pMaterial->mDynamicFriction = 0.5f;
	pActor->pMaterial->mStaticFriction = 0.5f;
	//------------------------------------------------

	//	Add actor to array
	m_aActors.Add(pActor);

	return pActor;
}

//	Remove a actor
void A3DSkinPhysSyncData::RemoveActor(const char* szName)
{
	//	Get actor object
	int iActorIdx = -1;
	ACTOR_DATA* pActor = GetActorByName(szName, &iActorIdx);
	if (!pActor)
		return;

	m_aActors.RemoveAt(iActorIdx);
	delete pActor;
}

//	Change actor name
bool A3DSkinPhysSyncData::ChangeActorName(const char* szOldName, const char* szNewName)
{
	if (!szNewName || !szNewName[0])
		return false;

	//	Get actor object
	ACTOR_DATA* pActor = GetActorByName(szOldName);
	if (!pActor)
		return false;

	//	Check name collision
	ACTOR_DATA* pTempActor = GetActorByName(szNewName);
	if (pTempActor == pActor)
		return true;	//	szOldName == szNewName
	else if (pTempActor)
		return false;	//	szNewName has been used by other actor

	pActor->strName = szNewName;

	return true;
}

//	Get actor
A3DSkinPhysSyncData::ACTOR_DATA* A3DSkinPhysSyncData::GetActorByName(const char* szName, int* piIndex/* NULL */) const
{
	for (int i=0; i < m_aActors.GetSize(); i++)
	{
		ACTOR_DATA* pActor = m_aActors[i];
		if (!pActor->strName.CompareNoCase(szName))
		{
			if (piIndex) 
				*piIndex = i;

			return pActor;
		}
	}

	return NULL;
}

//	Check if specified skin can be bound with this physical data
bool A3DSkinPhysSyncData::SkinBindCheck(A3DSkin* pSkin)
{
	if (!pSkin || !m_bBindInit)
		return false;

	//	Check mesh number
	if (pSkin->GetSkinMeshNum() < m_aSkinMeshInfo.GetSize() ||
		pSkin->GetRigidMeshNum() < m_aRigidMeshInfo.GetSize())
		return false;

	//	Collect skin mesh, this time we needn't to check
	//	mesh name collision any more
	APtrArray<A3DMeshBase*> aSkinMeshes;
	if (!CollectSkinMesh(pSkin, aSkinMeshes, false))
		return false;

	//	Compare meshes
	for (int i=0; i < m_aMeshes.GetSize() && i < aSkinMeshes.GetSize(); i++)
	{
		if (!CompareMesh(m_aMeshes[i], aSkinMeshes[i]))
			return false;
	}

	return true;
}

//	Write string to stream
bool A3DSkinPhysSyncData::WriteString(NxStream* ps, AString& str)
{
	//	Write length of string
	int iLen = str.GetLength();
	ps->storeDword((NxI32)iLen);

	//	Write string data
	if (iLen)
		ps->storeBuffer((const char*)str, iLen);

	return true;
}

//	Read string from stream
bool A3DSkinPhysSyncData::ReadString(NxStream* ps, AString& str)
{
	//	Read length of string
	int iLen = (int)ps->readDword();

	//	Read string data
	if (iLen)
	{
		char* szBuf = new char [iLen+1];
		if (!szBuf)
			return false;

		ps->readBuffer(szBuf, iLen);
		szBuf[iLen] = '\0';
		str = szBuf;

		delete [] szBuf;
	}
	else
		str = "";

	return true;
}

//	Load data from file
bool A3DSkinPhysSyncData::Load(const char* szFile)
{
	if (!szFile || !szFile[0])
		return false;

	APhysXFileImageStream stream(szFile, true);

	if (!stream.IsValid())
	{
		a_LogOutput(1, "A3DSkinPhysSyncData::Load, failed to open file %s", szFile);
		return false;
	}

	//	Record file name
	m_strFileName = szFile;

	//	Read file version
	m_iVersion = (int)stream.readDword();
	if (m_iVersion < 5 || m_iVersion > SKINPHYSFILE_VERSION)
	{
		a_LogOutput(1, "A3DSkinPhysSyncData::Load, error file version, %s", szFile);
		return false;
	}

	//	Read file header
	SKINPHYSFILE_HEADER6 fh;
	memset(&fh, 0, sizeof (fh));

	if (m_iVersion < 6)
	{
		SKINPHYSFILE_HEADER fhTemp;
		stream.readBuffer(&fhTemp, sizeof (fhTemp));
		memcpy(&fh, &fhTemp, sizeof (fhTemp));
	}
	else
	{
		stream.readBuffer(&fh, sizeof (fh));
	}

	m_bBindInit	= fh.bBindInit;
	m_bSoftLink = fh.bSoftLink;
	
	int i;

	//	Read actor data
	for (i=0; i < fh.iNumActor; i++)
	{
		ACTOR_DATA* pActor = new ACTOR_DATA;
		if (!ReadActorData(&stream, pActor))
		{
			delete pActor;
			return false;
		}

		m_aActors.Add(pActor);
	}

	//	Read mesh data
	for (i=0; i < fh.iNumMesh; i++)
	{
		MESH_DATA* pMesh = new MESH_DATA;
		if (!ReadMeshData(&stream, pMesh))
		{
			delete pMesh;
			return false;
		}

		m_aMeshes.Add(pMesh);
	}

	//	Read source mesh info
	for (i=0; i < fh.iRigidMeshNum; i++)
	{
		SRC_MESH* pInfo = new SRC_MESH;
		pInfo->iIndex = (int)stream.readDword();
		ReadString(&stream, pInfo->strName);
		m_aRigidMeshInfo.Add(pInfo);
	}

	for (i=0; i < fh.iSkinMeshNum; i++)
	{
		SRC_MESH* pInfo = new SRC_MESH;
		pInfo->iIndex = (int)stream.readDword();
		ReadString(&stream, pInfo->strName);
		m_aSkinMeshInfo.Add(pInfo);
	}

	//	Actors haven't link info when version < 3, so reset m_bBindInit flag
	//	to force user to redo InitBind
	if (m_iVersion < 3)
		m_bBindInit = false;

	//	Cloth attacher object desc
	if (m_bBindInit)
	{
		if (!CreateSkinPhysDesc_OnLoad())
		{
			m_bBindInit = false;
			return false;
		}
	}

	return true;
}

//	Save data to file
bool A3DSkinPhysSyncData::Save(const char* szFile)
{
	APhysXUserStream stream(szFile, false);

	if (!stream.IsValid())
	{
		a_LogOutput(1, "A3DSkinPhysSyncData::Save, failed to create file %s", szFile);
		return false;
	}

	//	Write file version
	stream.storeDword(SKINPHYSFILE_VERSION);

	//	Write file header
	SKINPHYSFILE_HEADER6 fh;
	memset(&fh, 0, sizeof (fh));

	fh.iNumActor		= m_aActors.GetSize();
	fh.iNumMesh			= m_aMeshes.GetSize();
	fh.iSkinMeshNum		= m_aSkinMeshInfo.GetSize();
	fh.iRigidMeshNum	= m_aRigidMeshInfo.GetSize();
	fh.bBindInit		= m_bBindInit;
	fh.bSoftLink		= m_bSoftLink;

	stream.storeBuffer(&fh, sizeof (fh));

	int i;

	//	Write actor data
	for (i=0; i < m_aActors.GetSize(); i++)
		WriteActorData(&stream, m_aActors[i]);

	//	Write mesh data
	for (i=0; i < m_aMeshes.GetSize(); i++)
		WriteMeshData(&stream, m_aMeshes[i]);

	//	Write source mesh info
	for (i=0; i < fh.iRigidMeshNum; i++)
	{
		SRC_MESH* pInfo = m_aRigidMeshInfo[i];
		stream.storeDword(pInfo->iIndex);
		WriteString(&stream, pInfo->strName);
	}

	for (i=0; i < fh.iSkinMeshNum; i++)
	{
		SRC_MESH* pInfo = m_aSkinMeshInfo[i];
		stream.storeDword(pInfo->iIndex);
		WriteString(&stream, pInfo->strName);
	}

	return true;
}

//	Write actor data to stream
bool A3DSkinPhysSyncData::WriteActorData(NxStream* ps, ACTOR_DATA* pActor)
{
	//	Actor name
	WriteString(ps, pActor->strName);
	//	Actor desc
	pActor->pActorDesc->Save(ps);
	//	Material desc
	pActor->pMaterial->Save(ps);
	//	bHHIsBone flag
	ps->storeByte((BYTE)(pActor->bHHIsBone ? 1 : 0));
	//	hh-type hook name
	WriteString(ps, pActor->strHHHook);

	int i;

	//	Shape data
	APhysXShapeDescManager man;
	for (i=0; i < pActor->aShapes.GetSize(); i++)
		man.CreateAndPushShapeDesc(pActor->aShapes[i]);

	if (!man.Save(ps))
		return false;

	//	Radius of bounding ball
	ps->storeFloat(pActor->fBoundRad);

	//	Dragged vertices
	int iCount = pActor->aDraggedVerts.GetSize();
	ps->storeDword(iCount);
	if (iCount)
	{
		ps->storeBuffer(pActor->aDraggedVerts.GetData(), iCount * sizeof (DWORD));
	}

	return true;
}

//	Write mesh data to stream
bool A3DSkinPhysSyncData::WriteMeshData(NxStream* ps, MESH_DATA* pMesh)
{
	//	Mesh name
	WriteString(ps, pMesh->strName);
	//	Vertex number
	ps->storeDword(pMesh->iNumVert);
	//	Index number
	ps->storeDword(pMesh->iNumIndex);
	//	Rigid flag
	ps->storeByte((BYTE)(pMesh->bRigid ? 1 : 0));

#ifdef _ANGELICA21
	// adhere mesh info, version 8 only
	WriteString(ps, pMesh->strAdhereMeshName);
	if (pMesh->strAdhereMeshName.IsEmpty())
	{
		if (m_bBindInit)
		{
			if (!pMesh->pClothDesc->Save(ps))
				return false;
		}
		else
		{
			//	Only save cloth parameters
			int iSize = sizeof (pMesh->pClothDesc->mClothParameters);
			ps->storeDword(iSize);
			ps->storeBuffer(&pMesh->pClothDesc->mClothParameters, iSize);
		}
	}
#else
	//	Cloth desc
	if (m_bBindInit)
	{
		if (!pMesh->pClothDesc->Save(ps))
			return false;
	}
	else
	{
		//	Only save cloth parameters
		int iSize = sizeof (pMesh->pClothDesc->mClothParameters);
		ps->storeDword(iSize);
		ps->storeBuffer(&pMesh->pClothDesc->mClothParameters, iSize);
	}
#endif

	

	//	Vertex data
	if (pMesh->iNumVert)
	{
		if (!pMesh->aVerts)
		{
			ASSERT(pMesh->aVerts);
			return false;
		}

		int iSize = pMesh->iNumVert * sizeof (A3DVECTOR3);
		ps->storeBuffer(pMesh->aVerts, iSize);
	}

	//	Index data
	if (pMesh->iNumIndex)
	{
		if (!pMesh->aIndices)
		{
			ASSERT(pMesh->aIndices);
			return false;
		}

		int iSize = pMesh->iNumIndex * sizeof (int);
		ps->storeBuffer(pMesh->aIndices, iSize);
	}

	//	Blend bone indices
	int i, iNumBone = pMesh->aBlendBones.GetSize();
	ps->storeDword(iNumBone);

	for (i=0; i < iNumBone; i++)
		ps->storeDword(pMesh->aBlendBones[i]);
	
	// cloth vertex constraints...
	if(pMesh->aClothVertexConstraints)
	{
		ps->storeByte((NxU8)1);
		ps->storeBuffer(pMesh->aClothVertexConstraints, pMesh->iNumVert * sizeof(APhysXClothVertexConstraintParameters));
	}
	else
		ps->storeByte((NxU8)0);

#ifdef _ANGELICA21
	// adhere mesh info, version 8
	//WriteString(ps, pMesh->strAdhereMeshName);
	if (!pMesh->strAdhereMeshName.IsEmpty())
	{
		ps->storeBuffer(pMesh->aAdherParams, pMesh->iNumVert * sizeof(A3DAdhereMesh::AdhereParam));
	}
#endif	

	return true;
}

//	Read actor data to stream
bool A3DSkinPhysSyncData::ReadActorData(NxStream* ps, ACTOR_DATA* pActor)
{
	//	Actor name
	ReadString(ps, pActor->strName);
	//	Actor desc
	pActor->pActorDesc->Load(ps);
	//	Material desc
	pActor->pMaterial->Load(ps);

	if (m_iVersion >= 3)
	{
		//	bHHIsBone flag
		pActor->bHHIsBone = ps->readByte() ? true : false;
		//	hh-type hook name
		ReadString(ps, pActor->strHHHook);
	}

	//	Shape data
	APhysXShapeDescManager man;
	if (!man.Load(ps))
		return false;

	int i, iNumShape = man.GetNum();
	for (i=0; i < iNumShape; i++)
	{
		APhysXShapeDesc* pShape = man.GetPhysXShapeDesc(i)->Clone();
		pActor->aShapes.Add(pShape);
	}

	if (m_iVersion >= 4)
	{
		//	Radius of bounding ball
		pActor->fBoundRad = ps->readFloat();

		//	Dragged vertices
		int iCount = (int)ps->readDword();
		if (iCount)
		{
			pActor->aDraggedVerts.SetSize(iCount, 4);
			ps->readBuffer(pActor->aDraggedVerts.GetData(), iCount * sizeof (DWORD));
		}
	}

	return true;
}

//	Read mesh data to stream
bool A3DSkinPhysSyncData::ReadMeshData(NxStream* ps, MESH_DATA* pMesh)
{
	//	Mesh name
	ReadString(ps, pMesh->strName);
	//	Vertex number
	pMesh->iNumVert = (int)ps->readDword();
	//	Index number
	pMesh->iNumIndex = (int)ps->readDword();
	//	Rigid flag
	if (m_iVersion >= 3)
		pMesh->bRigid = ps->readByte() ? true : false;

#ifdef _ANGELICA21
	if (m_iVersion >= 8)
	{
		ReadString(ps, pMesh->strAdhereMeshName);		
	}
	else
		pMesh->strAdhereMeshName.Empty();
	
	if (pMesh->strAdhereMeshName.IsEmpty())
	{
		if (m_bBindInit)
		{
			if (!pMesh->pClothDesc->Load(ps))
				return false;
		}
		else if (m_iVersion >= 2)
		{
			//	Read cloth parameters, check size first
			DWORD dwSize = ps->readDword();
			if (dwSize != sizeof (pMesh->pClothDesc->mClothParameters))
			{
				ASSERT(0);
				return false;
			}

			ps->readBuffer(&pMesh->pClothDesc->mClothParameters, dwSize);
		}
	}
#else
	//	Cloth desc
	if (m_bBindInit)
	{
		if (!pMesh->pClothDesc->Load(ps))
			return false;
	}
	else if (m_iVersion >= 2)
	{
		//	Read cloth parameters, check size first
		DWORD dwSize = ps->readDword();
		if (dwSize != sizeof (pMesh->pClothDesc->mClothParameters))
		{
			ASSERT(0);
			return false;
		}

		ps->readBuffer(&pMesh->pClothDesc->mClothParameters, dwSize);
	}
#endif

	pMesh->aVerts	= NULL;
	pMesh->aIndices	= NULL;

	if (pMesh->iNumVert)
	{
		A3DVECTOR3* aVerts = new A3DVECTOR3 [pMesh->iNumVert];
		if (!aVerts)
			return false;

		int iSize = pMesh->iNumVert * sizeof (A3DVECTOR3);
		ps->readBuffer(aVerts, iSize);
		pMesh->aVerts = aVerts;
	}

	if (pMesh->iNumIndex)
	{
		int* aIndices = new int [pMesh->iNumIndex];
		if (!aIndices)
			return false;

		int iSize = pMesh->iNumIndex * sizeof (int);
		ps->readBuffer(aIndices, iSize);
		pMesh->aIndices = aIndices;
	}

	//	Blend bones
	if (m_iVersion >= 3)
	{
		int i, iNumBone = (int)ps->readDword();
		pMesh->aBlendBones.SetSize(iNumBone, 4);

		for (i=0; i < iNumBone; i++)
			pMesh->aBlendBones[i] = (int)ps->readDword();
	}

	if(m_iVersion >= 7)
	{
		// try to read cloth vertex constraints...
		if(ps->readByte())
		{
			APhysXClothVertexConstraintParameters* aCVConstraints = new APhysXClothVertexConstraintParameters [pMesh->iNumVert];
			if (!aCVConstraints)
				return false;

			int iSize = pMesh->iNumVert * sizeof (APhysXClothVertexConstraintParameters);
			ps->readBuffer(aCVConstraints, iSize);
			pMesh->aClothVertexConstraints= aCVConstraints;	
		}
	}

#ifdef _ANGELICA21
	if (m_iVersion >= 8)
	{
		
		if (!pMesh->strAdhereMeshName.IsEmpty())
		{
			if (m_iVersion >= 9)
			{
				pMesh->aAdherParams = new A3DAdhereMesh::AdhereParam[pMesh->iNumVert];
				int iSize = pMesh->iNumVert * sizeof(A3DAdhereMesh::AdhereParam);
				ps->readBuffer(pMesh->aAdherParams, iSize);
			}
			else // version 8
			{
				struct Param
				{
					int iTriangleIdx;
					float    m_fDist;//distance from source vertex to dest triangle plane
					float    m_fWeight1;
					float    m_fWeight2;
				};
				Param para;
				pMesh->aAdherParams = new A3DAdhereMesh::AdhereParam[pMesh->iNumVert];
				for (int i = 0; i < pMesh->iNumVert; i++)
				{
					ps->readBuffer(&para, sizeof(Param));
					pMesh->aAdherParams[i].iTriangleIdx = para.iTriangleIdx;
					pMesh->aAdherParams[i].m_fDist = para.m_fDist;
					pMesh->aAdherParams[i].m_fWeight1 = para.m_fWeight1;
					pMesh->aAdherParams[i].m_fWeight2 = para.m_fWeight2;
					pMesh->aAdherParams[i].m_fAdhereWeight = 1.0f;
				}
			}
		}
	}
#endif
	
	return true;
}

//	Link actor to a HH-type hook
bool A3DSkinPhysSyncData::LinkActorToHook(const char* szActor, A3DSkinModel* pSkinModel, const char* szHookHH, bool bHHIsBone)
{
	ASSERT(pSkinModel);

	//	Get hh-type hook
	if (bHHIsBone)
	{
		if (!pSkinModel->GetSkeleton()->GetBone(szHookHH, NULL))
			return false;
	}
	else
	{
		if (!pSkinModel->GetSkeleton()->GetHook(szHookHH, NULL))
			return false;
	}

	ACTOR_DATA* pActor = GetActorByName(szActor);
	if (!pActor)
		return false;

	//	Link actor to hh-type hook
	pActor->strHHHook = szHookHH;
	pActor->bHHIsBone = bHHIsBone;

	return true;
}

//	Un-link actor from hook
bool A3DSkinPhysSyncData::UnlinkActorFromHook(const char* szActor)
{
	ACTOR_DATA* pActor = GetActorByName(szActor);
	if (!pActor)
		return false;

	pActor->strHHHook.Empty();
	pActor->bHHIsBone = false;

	return true;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement A3DSkinPhysSyncDataMan
//	
///////////////////////////////////////////////////////////////////////////

A3DSkinPhysSyncDataMan& A3DSkinPhysSyncDataMan::GetGlobalMan()
{
	return g_SkinPhysDataMan;
}

A3DSkinPhysSyncDataMan::A3DSkinPhysSyncDataMan() :
m_SyncDataTab(128)
{
	InitializeCriticalSection(&m_csSyncDataTab);
}

A3DSkinPhysSyncDataMan::~A3DSkinPhysSyncDataMan()
{
	if (m_SyncDataTab.size())
	{
		//	Only give a warning but not try to destroy thest un-released objects
		ASSERT(0);
		a_LogOutput(1, "A3DSkinPhysSyncDataMan, find un-released object !");
	}

	DeleteCriticalSection(&m_csSyncDataTab);
}

//	Load skin physical data from file
A3DSkinPhysSyncData* A3DSkinPhysSyncDataMan::LoadSyncData(const char* szFile)
{
	if (!szFile || !szFile[0])
		return NULL;

	char szRelFile[MAX_PATH];
	szRelFile[0] = '\0';
	af_GetRelativePath(szFile, szRelFile);
	DWORD dwFileID = a_MakeIDFromLowString(szRelFile);

	ACSWrapper csw(&m_csSyncDataTab);

	DATA_NODE* pNode = SearchDataNode(szRelFile, dwFileID);
	if (pNode)
	{
		pNode->dwRefCnt++;
		return pNode->pData;
	}

	csw.Detach(true);

	A3DSkinPhysSyncData* pSyncData = new A3DSkinPhysSyncData;
	if (!pSyncData || !pSyncData->Load(szFile))
	{
		if (pSyncData)
		{
			delete pSyncData;
			pSyncData = NULL;
		}

		a_LogOutput(1, "A3DSkinPhysSyncDataMan::Load, failed to load file %s !", szFile);
		return NULL;
	}

	csw.Attach(&m_csSyncDataTab);

	//	Add sync data to cache
	pNode = new DATA_NODE;
	pNode->pData = pSyncData;
	m_SyncDataTab.put((int)dwFileID, pNode);

	return pSyncData;
}

//	Release sync data
void A3DSkinPhysSyncDataMan::ReleaseSyncData(A3DSkinPhysSyncData* pSyncData)
{
	if (!pSyncData)
		return;

	ACSWrapper csw(&m_csSyncDataTab);

	//	Try to find this object from cache
	DWORD dwFileID = a_MakeIDFromLowString(pSyncData->GetFileName());
	DATA_NODE* pNode = SearchDataNode(pSyncData->GetFileName(), dwFileID);
	if (!pNode)
		return;		//	This data object isn't managed by us

	pNode->dwRefCnt--;
	if (pNode->dwRefCnt)
		return;
	
	//	Completely release object
	m_SyncDataTab.erase(dwFileID);
	delete pNode->pData;
	delete pNode;
}

//	Search data node
A3DSkinPhysSyncDataMan::DATA_NODE* A3DSkinPhysSyncDataMan::SearchDataNode(const char* szFile, DWORD dwFileID)
{
	ACSWrapper csw(&m_csSyncDataTab);

	SyncDataTable::pair_type Pair = m_SyncDataTab.get((int)dwFileID);
	if (!Pair.second)
		return NULL;	//	Counldn't find this sync data

	DATA_NODE* pNode = *Pair.first;

	if (szFile)
	{
		if (stricmp(pNode->pData->GetFileName(), szFile))
		{
			ASSERT(0);
			a_LogOutput(1, "A3DSkinPhysSyncDataMan::SearchDataNode, skin sync data file name collision !");
			return NULL;
		}
	}

	return pNode;
}

#endif	//	A3D_PHYSX
