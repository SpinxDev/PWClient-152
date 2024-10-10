/*
 * FILE: A3DClothMesh.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2008/6/16
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "A3DClothMesh.h"
#include "A3DPI.h"
#include "A3DEngine.h"
#include "A3DDevice.h"
#include "A3DStream.h"
#include "A3DMacros.h"
#include "A3DSkin.h"
#include "A3DSkinMeshMan.h"
#include "A3DSkinMesh.h"
#include "A3DRigidMesh.h"
#include "AMemory.h"

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
//	Implement A3DClothMeshImp
//	
///////////////////////////////////////////////////////////////////////////

A3DClothMeshImp::A3DClothMeshImp(A3DMeshBase* pCoreMesh)
{
	ASSERT(pCoreMesh);

	m_dwClassID		= A3D_CID_CLOTHMESHIMP;
	m_pCoreMesh		= pCoreMesh;
	m_aVerts		= NULL;
	m_aIndices		= NULL;
	m_bMeshChanged	= false;

	//	Copy info from core mesh
	SetName(pCoreMesh->GetName());
	m_iTexture	= pCoreMesh->GetTextureIndex();
	m_iMaterial	= pCoreMesh->GetMaterialIndex();

	m_aabbInitMesh.Center = pCoreMesh->GetInitMeshAABB().Center;
	m_aabbInitMesh.Extents = pCoreMesh->GetInitMeshAABB().Extents;
	m_aabbInitMesh.CompleteMinsMaxs();
}

A3DClothMeshImp::~A3DClothMeshImp()
{
}

//	Initialize object
bool A3DClothMeshImp::Init(A3DEngine* pA3DEngine)
{
	if (!A3DMeshBase::Init(pA3DEngine))
		return false;

	//	Setup original vertex info
	if (!ResetVertexInfo())
		return false;

	m_bMeshChanged = false;

	return true;
}

//	Release object
void A3DClothMeshImp::Release()
{
	if (m_aVerts)
	{
		delete [] m_aVerts;
		m_aVerts = NULL;
	}

	if (m_aIndices)
	{
		delete [] m_aIndices;
		m_aIndices = NULL;
	}

	m_iNumVert	= 0;
	m_iNumIdx	= 0;

	m_aVertInfo.RemoveAll();

	A3DMeshBase::Release();
}

//	Setup original vertex info
bool A3DClothMeshImp::ResetVertexInfo()
{
	if (!m_pCoreMesh || !m_pCoreMesh->GetVertexNum())
		return false;

	int i, iNumVert = m_pCoreMesh->GetVertexNum();
	m_aVertInfo.SetSize(iNumVert, 128);

	if (m_pCoreMesh->GetClassID() == A3D_CID_SKINMESH)
	{
		const SKIN_VERTEX* aSrcVerts = ((A3DSkinMesh*)m_pCoreMesh)->GetOriginVertexBuf();
		for (i=0; i < iNumVert; i++)
		{
			const SKIN_VERTEX& src = aSrcVerts[i];
			VERTEX_INFO& dst = m_aVertInfo[i];
			dst.tu = src.tu;
			dst.tv = src.tv;
		}
	}
	else if (m_pCoreMesh->GetClassID() == A3D_CID_RIGIDMESH)
	{
		A3DVERTEX* aSrcVerts = ((A3DRigidMesh*)m_pCoreMesh)->GetOriginVertexBuf();
		for (i=0; i < iNumVert; i++)
		{
			const A3DVERTEX& src = aSrcVerts[i];
			VERTEX_INFO& dst = m_aVertInfo[i];
			dst.tu = src.tu;
			dst.tv = src.tv;
		}
	}
	else
	{
		ASSERT(0);
		return false;
	}

	return true;
}

//	Reset mesh structure to initial state
bool A3DClothMeshImp::Reset()
{
	if (!ResetVertexInfo())
		return false;

	if (m_aVerts)
	{
		delete [] m_aVerts;
		m_aVerts = NULL;
	}

	if (m_aIndices)
	{
		delete [] m_aIndices;
		m_aIndices = NULL;
	}

	m_iNumVert		= 0;
	m_iNumIdx		= 0;
	m_bMeshChanged	= false;

	return true;
}

//	Update mesh data
bool A3DClothMeshImp::UpdateMeshData(int iNumVert, int iNumIndex, const A3DVECTOR3* aPos, const A3DVECTOR3* aNormals, const int* aIndices, 
				int iParentIdxNum, const int* aParentIdx)
{
	if (iParentIdxNum)
	{
		ASSERT(aParentIdx);
		m_bMeshChanged = true;
	}

	int i;

	//	Extend vertex info if necessary
	if (iNumVert > m_aVertInfo.GetSize() && iParentIdxNum)
	{
		int iOldInfoNum = m_aVertInfo.GetSize();

		for (i=iOldInfoNum; i < iParentIdxNum; i++)
		{
			int iParentIdx = aParentIdx[i];
			ASSERT(iParentIdx >= 0 && iParentIdx < iOldInfoNum);

			VERTEX_INFO info;
			info.tu = m_aVertInfo[iParentIdx].tu;
			info.tv = m_aVertInfo[iParentIdx].tv;
			m_aVertInfo.Add(info);
		}
	}

	//	Create vertex buffer
	if (m_iNumVert < iNumVert)
	{
		if (m_aVerts)
			delete [] m_aVerts;

		m_aVerts = new A3DVERTEX[iNumVert];
		if (!m_aVerts)
			return false;
	}

	//	Create index buffer
	if (m_iNumIdx < iNumIndex)
	{
		if (m_aIndices)
			delete [] m_aIndices;

		m_aIndices = new WORD[iNumIndex];
		if (!m_aIndices)
			return false;
	}

	//	Update vertex data
	for (i=0; i < iNumVert; i++)
	{
		A3DVERTEX& dst = m_aVerts[i];
		const VERTEX_INFO& info = m_aVertInfo[i];
		dst.Set(aPos[i], aNormals[i], info.tu, info.tv);
	}

	//	Update index data
	for (i=0; i < iNumIndex; i++)
		m_aIndices[i] = (WORD)aIndices[i];
	
	m_iNumVert	= iNumVert;
	m_iNumIdx	= iNumIndex;

	return true;
}

//	Prepare stream for rendering
A3DStream* A3DClothMeshImp::PrepareStream(A3DSkin* pSkin)
{
	A3DSkinMeshMan* pMeshMan = pSkin->GetSkinMeshMan();

	A3DStream* pStream = pMeshMan->RequestClothStream(m_iNumVert, m_iNumIdx);
	if (!pStream)
		return NULL;

	//	Fill vertex data
	DWORD dwSizeLock = m_iNumVert * sizeof (A3DVERTEX);
	A3DVERTEX* pVerts = NULL;
	if (!pStream->LockVertexBufferDynamic(0, dwSizeLock, (BYTE**) &pVerts, D3DLOCK_DISCARD))
		return NULL;

	memcpy(pVerts, m_aVerts, dwSizeLock);

	if (!pStream->UnlockVertexBufferDynamic())
		return NULL;

	//	Fill index data
	dwSizeLock = m_iNumIdx * sizeof (WORD);
	WORD* pIndices = NULL;
	if (!pStream->LockIndexBufferDynamic(0, dwSizeLock, (BYTE**) &pIndices, D3DLOCK_DISCARD))
		return NULL;

	memcpy(pIndices, m_aIndices, dwSizeLock);

	if (!pStream->UnlockIndexBufferDynamic())
		return NULL;

	return pStream;
}

//	Render mesh
bool A3DClothMeshImp::Render(A3DViewport* pViewport, A3DSkinModel* pSkinModel, A3DSkin* pSkin)
{
	A3DDevice* pA3DDevice = m_pA3DEngine->GetA3DDevice();
	if (!pA3DDevice)
		return true;

	if (!m_iNumVert || !m_iNumIdx)
		return true;

	pA3DDevice->SetWorldMatrix(A3D::g_matIdentity);

	A3DStream* pStream = PrepareStream(pSkin);
	if (!pStream)
		return false;

	pStream->Appear();

	if (!pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, m_iNumVert, 0, m_iNumIdx / 3))
		return false;


	return true;
}

//	Get approximate mesh data size
int A3DClothMeshImp::GetDataSize()
{
	//	Return approximate size
	int iNumVert = m_aVertInfo.GetSize();
	return iNumVert * (sizeof (VERTEX_INFO) + sizeof (A3DVERTEX));
}

