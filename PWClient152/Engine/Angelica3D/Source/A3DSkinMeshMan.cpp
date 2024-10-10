/*
 * FILE: A3DSkinMeshMan.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2005/4/13
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#include "A3DSkinMeshMan.h"
#include "A3DPI.h"
#include "A3DSkin.h"
#include "A3DSkinMesh.h"
#include "A3DRigidMesh.h"
#include "A3DMorphRigidMesh.h"
#include "A3DMorphSkinMesh.h"
#include "A3DSuppleMesh.h"
#include "A3DMuscleMesh.h"
#include "A3DEngine.h"
#include "A3DMacros.h"
#include "A3DStream.h"

#define new A_DEBUG_NEW

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


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
//	Implement A3DSkinMeshMan
//	
///////////////////////////////////////////////////////////////////////////

A3DSkinMeshMan::A3DSkinMeshMan() : 
m_SkinTab(256)
{
	m_pA3DEngine	= NULL;
	m_pA3DDevice	= NULL;
	m_dwTimeCnt		= 0;
	m_pClothStream	= NULL;
	
	InitializeCriticalSection(&m_cs);
}

A3DSkinMeshMan::~A3DSkinMeshMan()
{
	DeleteCriticalSection(&m_cs);
}

//	Initialize object
bool A3DSkinMeshMan::Init(A3DEngine* pA3DEngine)
{
	ASSERT(pA3DEngine);
	m_pA3DEngine = pA3DEngine;
	m_pA3DDevice = pA3DEngine->GetA3DDevice();
	return true;
}

//	Release object
void A3DSkinMeshMan::Release()
{
	ACSWrapper csa(&m_cs);

	//	Release all skin nodes
	SkinTable::iterator it = m_SkinTab.begin();
	for (; it != m_SkinTab.end(); ++it)
	{
		SKIN* pNode = *it.value();
		while (pNode)
		{
			SKIN* pNext = pNode->pNext;
			ReleaseSkinNode(pNode);
			pNode = pNext;
		}
	}

	m_SkinTab.clear();

	//	Release cloth stream
	A3DRELEASE(m_pClothStream);
}

/*	Load mesh object from file

	iMeshClassID: mesh's class ID
	iMeshIdx: mesh's index
	iMaxMeshNum: maximum number of this type mesh
*/
A3DMeshBase* A3DSkinMeshMan::LoadMeshObject(AFile* pFile, A3DSkin* pSkin, int iMeshClassID,
								int iMeshIdx, int iMaxMeshNum, int nSkinVersion)
{
	ACSWrapper csa(&m_cs);

	int iSkinID = (int)pSkin->GetSkinID();
	SKIN* pSkinNode = GetSkin(iSkinID, pSkin->GetFileName());

	if (!pSkinNode)
	{
		//	Create a new skin node
		if (!(pSkinNode = new SKIN))
			return NULL;

		pSkinNode->strSkinFile	= pSkin->GetFileName();
		pSkinNode->pNext		= NULL;

		SkinTable::pair_type Pair = m_SkinTab.get(iSkinID);
		if (!Pair.second)
			m_SkinTab.put(iSkinID, pSkinNode);
		else
		{
			g_A3DErrLog.Log("A3DSkinMeshMan::LoadMeshObject, different skin file have same ID !!");

			SKIN* pTempNode = *Pair.first;
			while (pTempNode->pNext)
				pTempNode = pTempNode->pNext;

			pTempNode->pNext = pSkinNode;
		}
	}

	A3DMeshBase* pMesh = NULL;

	switch (iMeshClassID)
	{
	case A3D_CID_SKINMESH:

		if (!pSkinNode->aSkinMeshes.GetSize())
		{
			pSkinNode->aSkinMeshes.SetSize(iMaxMeshNum, 10);
			for (int i=0; i < iMaxMeshNum; i++)
				pSkinNode->aSkinMeshes[i] = NULL;
		}
		else
		{
			if (pSkinNode->aSkinMeshes.GetSize() != iMaxMeshNum)
			{
				ASSERT(pSkinNode->aSkinMeshes.GetSize() != iMaxMeshNum);
				return NULL;
			}

			//	Did this mesh has been loaded ?
			if (pSkinNode->aSkinMeshes[iMeshIdx])
			{
				pSkinNode->aSkinMeshes[iMeshIdx]->AddRefCount();
				return pSkinNode->aSkinMeshes[iMeshIdx];
			}
		}

		if (!(pMesh = LoadMeshObject(pFile, pSkin, iMeshClassID, nSkinVersion)))
			return NULL;

		pSkinNode->aSkinMeshes[iMeshIdx] = (A3DSkinMesh*)pMesh;
		break;

	case A3D_CID_RIGIDMESH:
		
		if (!pSkinNode->aRigidMeshes.GetSize())
		{
			pSkinNode->aRigidMeshes.SetSize(iMaxMeshNum, 10);
			for (int i=0; i < iMaxMeshNum; i++)
				pSkinNode->aRigidMeshes[i] = NULL;
		}
		else
		{
			if (pSkinNode->aRigidMeshes.GetSize() != iMaxMeshNum)
			{
				ASSERT(pSkinNode->aRigidMeshes.GetSize() != iMaxMeshNum);
				return NULL;
			}

			//	Did this mesh has been loaded ?
			if (pSkinNode->aRigidMeshes[iMeshIdx])
			{
				pSkinNode->aRigidMeshes[iMeshIdx]->AddRefCount();
				return pSkinNode->aRigidMeshes[iMeshIdx];
			}
		}

		if (!(pMesh = LoadMeshObject(pFile, pSkin, iMeshClassID, nSkinVersion)))
			return NULL;

		pSkinNode->aRigidMeshes[iMeshIdx] = (A3DRigidMesh*)pMesh;
		break;

	case A3D_CID_MORPHSKINMESH:
		
		if (!pSkinNode->aMorphSkMeshes.GetSize())
		{
			pSkinNode->aMorphSkMeshes.SetSize(iMaxMeshNum, 10);
			for (int i=0; i < iMaxMeshNum; i++)
				pSkinNode->aMorphSkMeshes[i] = NULL;
		}
		else
		{
			if (pSkinNode->aMorphSkMeshes.GetSize() != iMaxMeshNum)
			{
				ASSERT(pSkinNode->aMorphSkMeshes.GetSize() != iMaxMeshNum);
				return NULL;
			}

			//	Did this mesh has been loaded ?
			if (pSkinNode->aMorphSkMeshes[iMeshIdx])
			{
				pSkinNode->aMorphSkMeshes[iMeshIdx]->AddRefCount();
				return pSkinNode->aMorphSkMeshes[iMeshIdx];
			}
		}

		if (!(pMesh = LoadMeshObject(pFile, pSkin, iMeshClassID, nSkinVersion)))
			return NULL;

		pSkinNode->aMorphSkMeshes[iMeshIdx] = (A3DMorphSkinMesh*)pMesh;
		break;

	case A3D_CID_MORPHRIGIDMESH:
		
		if (!pSkinNode->aMorphRdMeshes.GetSize())
		{
			pSkinNode->aMorphRdMeshes.SetSize(iMaxMeshNum, 10);
			for (int i=0; i < iMaxMeshNum; i++)
				pSkinNode->aMorphRdMeshes[i] = NULL;
		}
		else
		{
			if (pSkinNode->aMorphRdMeshes.GetSize() != iMaxMeshNum)
			{
				ASSERT(pSkinNode->aMorphRdMeshes.GetSize() != iMaxMeshNum);
				return NULL;
			}

			//	Did this mesh has been loaded ?
			if (pSkinNode->aMorphRdMeshes[iMeshIdx])
			{
				pSkinNode->aMorphRdMeshes[iMeshIdx]->AddRefCount();
				return pSkinNode->aMorphRdMeshes[iMeshIdx];
			}
		}

		if (!(pMesh = LoadMeshObject(pFile, pSkin, iMeshClassID, nSkinVersion)))
			return NULL;

		pSkinNode->aMorphRdMeshes[iMeshIdx] = (A3DMorphRigidMesh*)pMesh;
		break;

	case A3D_CID_SUPPLEMESH:
		
		if (!pSkinNode->aSuppleMeshes.GetSize())
		{
			pSkinNode->aSuppleMeshes.SetSize(iMaxMeshNum, 10);
			for (int i=0; i < iMaxMeshNum; i++)
				pSkinNode->aSuppleMeshes[i] = NULL;
		}
		else
		{
			if (pSkinNode->aSuppleMeshes.GetSize() != iMaxMeshNum)
			{
				ASSERT(pSkinNode->aSuppleMeshes.GetSize() != iMaxMeshNum);
				return NULL;
			}

			//	Did this mesh has been loaded ?
			if (pSkinNode->aSuppleMeshes[iMeshIdx])
			{
				pSkinNode->aSuppleMeshes[iMeshIdx]->AddRefCount();
				return pSkinNode->aSuppleMeshes[iMeshIdx];
			}
		}

		if (!(pMesh = LoadMeshObject(pFile, pSkin, iMeshClassID, nSkinVersion)))
			return NULL;

		pSkinNode->aSuppleMeshes[iMeshIdx] = (A3DSuppleMesh*)pMesh;
		break;

	case A3D_CID_MUSCLEMESH:
		
		if (!pSkinNode->aMuscleMeshes.GetSize())
		{
			pSkinNode->aMuscleMeshes.SetSize(iMaxMeshNum, 10);
			for (int i=0; i < iMaxMeshNum; i++)
				pSkinNode->aMuscleMeshes[i] = NULL;
		}
		else
		{
			if (pSkinNode->aMuscleMeshes.GetSize() != iMaxMeshNum)
			{
				ASSERT(pSkinNode->aMuscleMeshes.GetSize() != iMaxMeshNum);
				return NULL;
			}

			//	Did this mesh has been loaded ?
			if (pSkinNode->aMuscleMeshes[iMeshIdx])
			{
				pSkinNode->aMuscleMeshes[iMeshIdx]->AddRefCount();
				return pSkinNode->aMuscleMeshes[iMeshIdx];
			}
		}

		if (!(pMesh = LoadMeshObject(pFile, pSkin, iMeshClassID, nSkinVersion)))
			return NULL;

		pSkinNode->aMuscleMeshes[iMeshIdx] = (A3DMuscleMesh*)pMesh;
		break;

	default:
		ASSERT(iMeshClassID);
		return NULL;
	}

	ASSERT(pMesh);
	return pMesh;
}

//	Load mesh object from file
A3DMeshBase* A3DSkinMeshMan::LoadMeshObject(AFile* pFile, A3DSkin* pSkin, int iMeshClassID,int nSkinVersion)
{
	A3DMeshBase* pMesh = NULL;

	switch (iMeshClassID)
	{
	case A3D_CID_SKINMESH:			pMesh = new A3DSkinMesh;		break;
	case A3D_CID_RIGIDMESH:			pMesh = new A3DRigidMesh;		break;
	case A3D_CID_MORPHSKINMESH:		pMesh = new A3DMorphSkinMesh;	break;
	case A3D_CID_MORPHRIGIDMESH:	pMesh = new A3DMorphRigidMesh;	break;
	case A3D_CID_SUPPLEMESH:		pMesh = new A3DSuppleMesh;		break;
	case A3D_CID_MUSCLEMESH:		pMesh = new A3DMuscleMesh;		break;
	default:
		ASSERT(0);
		return NULL;
	}

	if (!pMesh->Init(m_pA3DEngine))
	{
		delete pMesh;
		g_A3DErrLog.Log("A3DSkinMeshMan::LoadMeshObject, Failed to initialize mesh !");
		return NULL;
	}

	if (!pMesh->Load(pFile, pSkin, nSkinVersion))
	{
		delete pMesh;
		g_A3DErrLog.Log("A3DSkinMeshMan::LoadMeshObject, Failed to load mesh !");
		return NULL;
	}

	return pMesh;
}

//	Get skin node with specified ID and file name
A3DSkinMeshMan::SKIN* A3DSkinMeshMan::GetSkin(int idSkin, const char* szFileName)
{
	ACSWrapper csa(&m_cs);

	//	Skin exists ?
	SkinTable::pair_type Pair = m_SkinTab.get(idSkin);
	if (!Pair.second)
		return NULL;	//	Counldn't find this skin

	SKIN* pNode = *Pair.first;
	while (pNode)
	{
		if (!pNode->strSkinFile.CompareNoCase(szFileName))
			return pNode;

		pNode = pNode->pNext;
	}

	return NULL;
}

//	Tick routine
void A3DSkinMeshMan::Tick(DWORD dwDeltaTime)
{
	if ((m_dwTimeCnt += dwDeltaTime) >= 60000)
	{
		ACSWrapper csa(&m_cs);

		m_dwTimeCnt = 0;

		//	Check all skin
		SkinTable::iterator it = m_SkinTab.begin();
		for (; it != m_SkinTab.end(); ++it)
		{
			SKIN* pPrev = NULL;
			SKIN* pNode = *it.value();
			while (pNode)
			{
				SKIN* pNext = pNode->pNext;

				if (IsGarbageSkin(pNode))
				{
					//	Erase from list
					if (!pPrev)
						*it.value() = pNext;
					else
						pPrev->pNext = pNext;

					ReleaseSkinNode(pNode);
				}
				else
					pPrev = pNode;

				pNode = pNext;
			}
		}
	}
}

//	Check whether a skin's all meshes have been released
bool A3DSkinMeshMan::IsGarbageSkin(SKIN* pSkinNode)
{
	//	If a skin whose all meshes reference counter are zero, the
	//	skin is considered a garbage
	int i;

	for (i=0; i < pSkinNode->aSkinMeshes.GetSize(); i++)
	{
		A3DMeshBase* pMesh = pSkinNode->aSkinMeshes[i];
		if (pMesh && pMesh->GetRefCount() > 0)
			return false;
	}

	for (i=0; i < pSkinNode->aRigidMeshes.GetSize(); i++)
	{
		A3DMeshBase* pMesh = pSkinNode->aRigidMeshes[i];
		if (pMesh && pMesh->GetRefCount() > 0)
			return false;
	}

	for (i=0; i < pSkinNode->aMorphSkMeshes.GetSize(); i++)
	{
		A3DMeshBase* pMesh = pSkinNode->aMorphSkMeshes[i];
		if (pMesh && pMesh->GetRefCount() > 0)
			return false;
	}

	for (i=0; i < pSkinNode->aMorphRdMeshes.GetSize(); i++)
	{
		A3DMeshBase* pMesh = pSkinNode->aMorphRdMeshes[i];
		if (pMesh && pMesh->GetRefCount() > 0)
			return false;
	}

	for (i=0; i < pSkinNode->aSuppleMeshes.GetSize(); i++)
	{
		A3DMeshBase* pMesh = pSkinNode->aSuppleMeshes[i];
		if (pMesh && pMesh->GetRefCount() > 0)
			return false;
	}

	for (i=0; i < pSkinNode->aMuscleMeshes.GetSize(); i++)
	{
		A3DMeshBase* pMesh = pSkinNode->aMuscleMeshes[i];
		if (pMesh && pMesh->GetRefCount() > 0)
			return false;
	}

	return true;
}

//	Release skin node
void A3DSkinMeshMan::ReleaseSkinNode(SKIN* pSkinNode)
{
	int i, iNumLeak=0;

	for (i=0; i < pSkinNode->aSkinMeshes.GetSize(); i++)
	{
		A3DMeshBase* pMesh = pSkinNode->aSkinMeshes[i];
		if (pMesh)
		{
			if (pMesh->GetRefCount() > 0)
				iNumLeak++;

			pMesh->Release();
			delete pMesh;
		}
	}

	for (i=0; i < pSkinNode->aRigidMeshes.GetSize(); i++)
	{
		A3DMeshBase* pMesh = pSkinNode->aRigidMeshes[i];
		if (pMesh)
		{
			if (pMesh->GetRefCount() > 0)
				iNumLeak++;

			pMesh->Release();
			delete pMesh;
		}
	}

	for (i=0; i < pSkinNode->aMorphSkMeshes.GetSize(); i++)
	{
		A3DMeshBase* pMesh = pSkinNode->aMorphSkMeshes[i];
		if (pMesh)
		{
			if (pMesh->GetRefCount() > 0)
				iNumLeak++;

			pMesh->Release();
			delete pMesh;
		}
	}

	for (i=0; i < pSkinNode->aMorphRdMeshes.GetSize(); i++)
	{
		A3DMeshBase* pMesh = pSkinNode->aMorphRdMeshes[i];
		if (pMesh)
		{
			if (pMesh->GetRefCount() > 0)
				iNumLeak++;

			pMesh->Release();
			delete pMesh;
		}
	}

	for (i=0; i < pSkinNode->aSuppleMeshes.GetSize(); i++)
	{
		A3DMeshBase* pMesh = pSkinNode->aSuppleMeshes[i];
		if (pMesh)
		{
			if (pMesh->GetRefCount() > 0)
				iNumLeak++;

			pMesh->Release();
			delete pMesh;
		}
	}

	for (i=0; i < pSkinNode->aMuscleMeshes.GetSize(); i++)
	{
		A3DMeshBase* pMesh = pSkinNode->aMuscleMeshes[i];
		if (pMesh)
		{
			if (pMesh->GetRefCount() > 0)
				iNumLeak++;

			pMesh->Release();
			delete pMesh;
		}
	}

	if (iNumLeak)
		g_A3DErrLog.Log("A3DSkinMeshMan::ReleaseSkinNode, skin mesh leak (%s) !", pSkinNode->strSkinFile);

	delete pSkinNode;
}

//	Release all mesh object of specified skin
void A3DSkinMeshMan::ReleaseAllSkinMesh(A3DSkin* pSkin)
{
	if (!pSkin->GetSkinID())
		return;

	ACSWrapper csa(&m_cs);

	SKIN* pSkinNode = GetSkin((int)pSkin->GetSkinID(), pSkin->GetFileName());
	if (!pSkinNode)
	{
		//ASSERT(pSkinNode);
		return;
	}

	int i;

	for (i=0; i < pSkinNode->aSkinMeshes.GetSize(); i++)
	{
		A3DMeshBase* pMesh = pSkinNode->aSkinMeshes[i];
		if (pMesh && !pMesh->DecRefCount())
		{
			pMesh->Release();
			delete pMesh;
			pSkinNode->aSkinMeshes[i] = NULL;
		}
	}

	for (i=0; i < pSkinNode->aRigidMeshes.GetSize(); i++)
	{
		A3DMeshBase* pMesh = pSkinNode->aRigidMeshes[i];
		if (pMesh && !pMesh->DecRefCount())
		{
			pMesh->Release();
			delete pMesh;
			pSkinNode->aRigidMeshes[i] = NULL;
		}
	}

	for (i=0; i < pSkinNode->aMorphSkMeshes.GetSize(); i++)
	{
		A3DMeshBase* pMesh = pSkinNode->aMorphSkMeshes[i];
		if (pMesh && !pMesh->DecRefCount())
		{
			pMesh->Release();
			delete pMesh;
			pSkinNode->aMorphSkMeshes[i] = NULL;
		}
	}

	for (i=0; i < pSkinNode->aMorphRdMeshes.GetSize(); i++)
	{
		A3DMeshBase* pMesh = pSkinNode->aMorphRdMeshes[i];
		if (pMesh && !pMesh->DecRefCount())
		{
			pMesh->Release();
			delete pMesh;
			pSkinNode->aMorphRdMeshes[i] = NULL;
		}
	}

	for (i=0; i < pSkinNode->aSuppleMeshes.GetSize(); i++)
	{
		A3DMeshBase* pMesh = pSkinNode->aSuppleMeshes[i];
		if (pMesh && !pMesh->DecRefCount())
		{
			pMesh->Release();
			delete pMesh;
			pSkinNode->aSuppleMeshes[i] = NULL;
		}
	}

	for (i=0; i < pSkinNode->aMuscleMeshes.GetSize(); i++)
	{
		A3DMeshBase* pMesh = pSkinNode->aMuscleMeshes[i];
		if (pMesh && !pMesh->DecRefCount())
		{
			pMesh->Release();
			delete pMesh;
			pSkinNode->aMuscleMeshes[i] = NULL;
		}
	}
}

//	Calculate approximate data size of all meshes
int A3DSkinMeshMan::CalcDataSize()
{
	int iTotalSize = 0;

	ACSWrapper csa(&m_cs);

	//	Check all skin
	SkinTable::iterator it = m_SkinTab.begin();
	for (; it != m_SkinTab.end(); ++it)
	{
		SKIN* pNode = *it.value();
		while (pNode)
		{
			iTotalSize += CalculateSkinSize(pNode);
			pNode = pNode->pNext;
		}
	}

	return iTotalSize;
}

//	Calcuate data size of skin node
int A3DSkinMeshMan::CalculateSkinSize(SKIN* pSkinNode)
{
	int i, iTotalSize = 0;

	for (i=0; i < pSkinNode->aSkinMeshes.GetSize(); i++)
	{
		if (pSkinNode->aSkinMeshes[i])
			iTotalSize += pSkinNode->aSkinMeshes[i]->GetDataSize();
	}

	for (i=0; i < pSkinNode->aRigidMeshes.GetSize(); i++)
	{
		if (pSkinNode->aRigidMeshes[i])
			iTotalSize += pSkinNode->aRigidMeshes[i]->GetDataSize();
	}

	for (i=0; i < pSkinNode->aMorphSkMeshes.GetSize(); i++)
	{
		if (pSkinNode->aMorphSkMeshes[i])
			iTotalSize += pSkinNode->aMorphSkMeshes[i]->GetDataSize();
	}

	for (i=0; i < pSkinNode->aMorphRdMeshes.GetSize(); i++)
	{
		if (pSkinNode->aMorphRdMeshes[i])
			iTotalSize += pSkinNode->aMorphRdMeshes[i]->GetDataSize();
	}

	for (i=0; i < pSkinNode->aSuppleMeshes.GetSize(); i++)
	{
		if (pSkinNode->aSuppleMeshes[i])
			iTotalSize += pSkinNode->aSuppleMeshes[i]->GetDataSize();
	}

	for (i=0; i < pSkinNode->aMuscleMeshes.GetSize(); i++)
	{
		if (pSkinNode->aMuscleMeshes[i])
			iTotalSize += pSkinNode->aMuscleMeshes[i]->GetDataSize();
	}

	return iTotalSize;
}

//	Request cloth stream
A3DStream* A3DSkinMeshMan::RequestClothStream(int iVertNum, int iIndexNum)
{
	if (!m_pA3DDevice)
		return NULL;

	if (m_pClothStream)
	{
		if (iVertNum <= m_pClothStream->GetVertCount() &&
			iIndexNum <= m_pClothStream->GetIndexCount())
			return m_pClothStream;
	}

	//	Release old stream
	A3DRELEASE(m_pClothStream);

	if (!(m_pClothStream = new A3DStream))
	{
		g_A3DErrLog.Log("A3DSkinMeshMan::RequestClothStream, Not Enough Memory!");
		return false;
	}

	ASSERT(!(iIndexNum % 3));

	if (!m_pClothStream->Init(m_pA3DDevice, A3DVT_VERTEX, iVertNum, iIndexNum, 
			A3DSTRM_REFERENCEPTR, A3DSTRM_REFERENCEPTR))
	{
		g_A3DErrLog.Log("A3DSkinMeshMan::RequestClothStream, Failed to initialize A3DStream !");
		A3DRELEASE(m_pClothStream);
		return false;
	}

	return m_pClothStream;
}

