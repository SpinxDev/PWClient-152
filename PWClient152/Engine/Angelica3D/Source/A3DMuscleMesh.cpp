/*
 * FILE: A3DMuscleMesh.cpp
 *
 * DESCRIPTION: Muscle controlled mesh
 *
 * CREATED BY: Hedi, 2004/7/5
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "A3DDevice.h"
#include "A3DMuscleMesh.h"
#include "A3DPI.h"
#include "A3DSkinModel.h"
#include "A3DConfig.h"
#include "AFile.h"
#include "A3DFuncs.h"
#include "A3DMacros.h"
#include "AMemory.h"
#include "A3DViewport.h"
#include "A3DStream.h"
#include "A3DSkin.h"
#include "A3DSkeleton.h"
#include "A3DMuscleOrgan.h"
#include "A3DEngine.h"

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

struct MUSCLEMESHDATA
{
	int		iTexture;		//	Texture index
	int		iMaterial;		//	Material index
	int		iNumVert;		//	Number of vertex
	int		iNumIdx;		//	Number of index
};

A3DMuscleMesh::A3DMuscleMesh()
{
	m_dwClassID	= A3D_CID_MUSCLEMESH;
	m_aVerts	= NULL;
	m_aIndices	= NULL;
}

A3DMuscleMesh::~A3DMuscleMesh()
{
}

bool A3DMuscleMesh::Init(A3DEngine* pA3DEngine)
{
	if (!A3DMeshBase::Init(pA3DEngine))
		return false;

	return true;
}

void A3DMuscleMesh::Release()
{
	//	Release original data buffers
	ReleaseOriginalBuffers();

	m_iNumVert	= 0;
	m_iNumIdx	= 0;

	A3DMeshBase::Release();
}

bool A3DMuscleMesh::Load(AFile* pFile, A3DSkin* pSkin,int nSkinVersion)
{
	ASSERT(m_pA3DEngine);

	//	Load mesh name
	if (!pFile->ReadString(m_strName))
	{
		g_A3DErrLog.Log("A3DMuscleMesh::Load, Failed to read mesh name!");
		return false;
	}

	MUSCLEMESHDATA Data;
	DWORD dwRead;

	if (!pFile->Read(&Data, sizeof (Data), &dwRead) || dwRead != sizeof (Data))
	{
		g_A3DErrLog.Log("A3DMuscleMesh::Load, Failed to load skin mesh data!");
		return false;
	}
	
	m_iNumVert	= Data.iNumVert;
	m_iNumIdx	= Data.iNumIdx;
	m_iTexture	= Data.iTexture;
	m_iMaterial	= Data.iMaterial;

	//	Create original data buffers
	if (!CreateOriginalBuffers(m_iNumVert, m_iNumIdx))
	{
		g_A3DErrLog.Log("A3DMuscleMesh::Load, Failed to create buffers!");
		return false;
	}

	//	Load vertex data
	if (m_iNumVert > 0)
	{
		DWORD dwSize = sizeof (A3DBVERTEX3) * m_iNumVert;

		if (!pFile->Read(m_aVerts, dwSize, &dwRead) || dwRead != dwSize)
		{
			g_A3DErrLog.Log("A3DMuscleMesh::Load, Failed to load vertex data!");
			return false;
		}
	}

	//	Load index data
	if (m_iNumIdx > 0)
	{
		DWORD dwSize = sizeof (WORD) * m_iNumIdx;

		if (!pFile->Read(m_aIndices, dwSize, &dwRead) || dwRead != dwSize)
		{
			g_A3DErrLog.Log("A3DMuscleMesh::Load, Failed to load index data!");
			return false;
		}
	}

	//	Calculate initial mesh aabb
	CalcInitMeshAABB();

	return true;
}

bool A3DMuscleMesh::Save(AFile* pFile, int nSkinVersion,int& nReturnValue)
{
	nReturnValue = RV_UNKNOWN_ERR;
	//	Write mesh name
	if (!pFile->WriteString(m_strName))
	{
		g_A3DErrLog.Log("A3DMuscleMesh::Save(), Failed to write mesh name!");
		return false;
	}

	MUSCLEMESHDATA Data;
	memset(&Data, 0, sizeof (Data));

	Data.iNumVert	= m_iNumVert;
	Data.iNumIdx	= m_iNumIdx;
	Data.iTexture	= m_iTexture;
	Data.iMaterial	= m_iMaterial;

	DWORD dwWrite;

	//	Write data
	if (!pFile->Write(&Data, sizeof (Data), &dwWrite) || dwWrite != sizeof (Data))
	{
		g_A3DErrLog.Log("A3DMuscleMesh::Save, Failed to write skin mesh data!");
		return false;
	}

	//	Write vertex data
	if (m_aVerts && m_iNumVert)
	{
		DWORD dwSize = sizeof (A3DBVERTEX3) * m_iNumVert;

		if (!pFile->Write(m_aVerts, dwSize, &dwWrite) || dwWrite != dwSize)
		{
			g_A3DErrLog.Log("A3DMuscleMesh::Save, Failed to write vertex data!");
			return false;
		}
	}

	//	Write index data
	if (m_aIndices && m_iNumIdx)
	{
		DWORD dwSize = sizeof (WORD) * m_iNumIdx;

		if (!pFile->Write(m_aIndices, dwSize, &dwWrite) || dwWrite != dwSize)
		{
			g_A3DErrLog.Log("A3DMuscleMesh::Save, Failed to write index data!");
			return false;
		}
	}

	if( nReturnValue == RV_UNKNOWN_ERR)
	{
		nReturnValue = RV_OK;
	}
	return true;
}

int A3DMuscleMesh::GetDataSize()
{
	int iSize = 0;
	int iVertSize = m_iNumVert * sizeof (A3DBVERTEX3);
	int iIdxSize = m_iNumIdx * sizeof (WORD);

	if (m_aVerts)
		iSize += iVertSize;

	if (m_aIndices)
		iSize += iIdxSize;

	return iSize;
}

//	Calculate initial mesh aabb
void A3DMuscleMesh::CalcInitMeshAABB()
{
	if (!m_iNumVert || !m_aVerts)
	{
		memset(&m_aabbInitMesh, 0, sizeof (m_aabbInitMesh));
		return;
	}

	m_aabbInitMesh.Clear();
	A3DVECTOR3 v;

	for (int i=0; i < m_iNumVert; i++)
	{
		A3DBVERTEX3* pv = &m_aVerts[i];
		v.Set(pv->vPos[0], pv->vPos[1], pv->vPos[2]);
		m_aabbInitMesh.AddVertex(v);
	}

	m_aabbInitMesh.CompleteCenterExts();
}
	
bool A3DMuscleMesh::CreateOriginalBuffers(int iNumVert, int iNumIdx)
{
	//	Release old buffers
	ReleaseOriginalBuffers();

	if (iNumVert > 0)
	{
		m_aVerts = new A3DBVERTEX3[iNumVert];

		if (!m_aVerts)
		{
			g_A3DErrLog.Log("A3DMuscleMesh::CreateOriginalBuffers, Not enough memory !");
			return false;
		}
	}

	if (iNumIdx > 0)
	{
		m_aIndices = new WORD[iNumIdx];

		if (!m_aIndices)
		{
			g_A3DErrLog.Log("A3DMuscleMesh::CreateOriginalBuffers, Not enough memory !");
			return false;
		}
	}

	m_iNumVert	= iNumVert;
	m_iNumIdx	= iNumIdx;

	return true;
}

void A3DMuscleMesh::ReleaseOriginalBuffers()
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
}

///////////////////////////////////////////////////////////////////////////
//	
//	Class A3DMuscleMeshImp
//	
///////////////////////////////////////////////////////////////////////////

A3DMuscleMeshImp::A3DMuscleMeshImp(A3DMuscleMesh* pCoreMesh, A3DSkinModel* pSkinModel)
{
	m_dwClassID			= A3D_CID_MUSCLEMESHIMP;
	m_pCoreMesh			= pCoreMesh;
	m_pSkinModel		= pSkinModel;
	m_aDeformedVerts	= NULL;
	m_pA3DStream		= NULL;
	m_pMuscleOrgan		= NULL;
}

A3DMuscleMeshImp::~A3DMuscleMeshImp()
{
}

bool A3DMuscleMeshImp::Init(A3DEngine* pA3DEngine)
{
	ASSERT(m_pCoreMesh);

	if (!A3DMeshBase::Init(pA3DEngine))
		return false;

	m_iNumVert		= m_pCoreMesh->GetVertexNum();
	m_iNumIdx		= m_pCoreMesh->GetIndexNum();
	m_iTexture		= m_pCoreMesh->GetTextureIndex();
	m_iMaterial		= m_pCoreMesh->GetMaterialIndex();
	m_aabbInitMesh	= m_pCoreMesh->GetInitMeshAABB();

	if( m_iNumVert )
	{
		if( !CreateDeformedBuffers() )
		{
			g_A3DErrLog.Log("A3DMuscleMeshImp::Init, Failed to create deformed buffers!");
			return false;
		}
	}

	//	Create stream
	if (m_iNumVert > 0 && m_iNumIdx > 0)
	{
		if (!CreateStream())
		{
			g_A3DErrLog.Log("A3DMuscleMeshImp::Init, Failed to create stream!");
			return false;
		}
	}

	// we should calculate the muscle affect region, but now, now muscle data known
	// user must apply a muscle organ to this muscle mesh explictly
	return true;
}

void A3DMuscleMeshImp::Release()
{
	ReleaseStream();
	ReleaseDeformedBuffers();
	A3DMeshBase::Release();
}

bool A3DMuscleMeshImp::Render(A3DViewport* pViewport, A3DSkinModel* pSkinModel, A3DSkin* pSkin)
{
	// Only use software vertex processing now
	A3DDevice* pA3DDevice = m_pA3DEngine->GetA3DDevice();
	A3DStream* pA3DStream = m_pA3DStream;
	if (!pA3DDevice || !pA3DStream)
		return true;

	pA3DDevice->SetWorldMatrix(A3D::g_matIdentity);

	const A3DMATRIX4* aBlendMats = pSkinModel->GetBlendMatrices();

	A3DVERTEX* aVerts;

	//	Lock vertex buffer
	if (!pA3DStream->LockVertexBuffer(0, 0, (BYTE**) &aVerts, 0))
		return false;

	A3DVECTOR3 vPos, vNormal, vSrcPos, vSrcNormal;
	float fTotalWeight;

	for (int i=0; i < m_iNumVert; i++)
	{
		A3DVERTEX* pDest = &aVerts[i];
		A3DBVERTEX3* pSrc = &m_aDeformedVerts[i];

		vSrcPos.Set(pSrc->vPos[0], pSrc->vPos[1], pSrc->vPos[2]);
		vSrcNormal.Set(pSrc->vNormal[0], pSrc->vNormal[1], pSrc->vNormal[2]);

		vPos.Clear();
		vNormal.Clear();

		fTotalWeight = 0;
		int j = 0;
		for (j=0; j < 3; j++)
		{
			if (!pSrc->aWeights[j])
				break;

			int iMat = (pSrc->dwMatIndices >> (j << 3)) & 0x000000ff;
			vPos	+= (aBlendMats[iMat] * vSrcPos) * pSrc->aWeights[j];
			vNormal += a3d_VectorMatrix3x3(vSrcNormal, aBlendMats[iMat]) * pSrc->aWeights[j];

			fTotalWeight += pSrc->aWeights[j];
		}

		if (j >= 3)
		{
			fTotalWeight = 1.0f - fTotalWeight;
			if (fTotalWeight > 0.0f)
			{
				int iMat = (pSrc->dwMatIndices & 0xff000000) >> 24;
				vPos	+= (aBlendMats[iMat] * vSrcPos) * fTotalWeight;
				vNormal += a3d_VectorMatrix3x3(vSrcNormal, aBlendMats[iMat]) * fTotalWeight;
			}
		}

		vNormal.Normalize();

		pDest->x = vPos.x;
		pDest->y = vPos.y;
		pDest->z = vPos.z;

		pDest->nx = vNormal.x;
		pDest->ny = vNormal.y;
		pDest->nz = vNormal.z;
	//	pDest->nx = vSrcNormal.x;
	//	pDest->ny = vSrcNormal.y;
	//	pDest->nz = vSrcNormal.z;

		pDest->tu = pSrc->tu;
		pDest->tv = pSrc->tv;
	}

	if (!pA3DStream->UnlockVertexBuffer())
		return false;

	pA3DStream->Appear();

	if (!pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, m_iNumVert, 0, m_iNumIdx / 3))
		return false;

	return true;
}

int A3DMuscleMeshImp::GetDataSize()
{
	int iSize = 0;
	int iVertSize = m_iNumVert * sizeof (A3DBVERTEX3);
	
	if (m_aDeformedVerts)
		iSize += iVertSize;

	return iSize;
}

bool A3DMuscleMeshImp::Update(int iDeltaTime, A3DSkinModel* pSkinModel)
{
	// if not controlled by a muscle organ yet
	if( !m_pMuscleOrgan )
		return true;

	// Let organ update itself's animation data
	if( !m_pMuscleOrgan->Update(iDeltaTime) )
	{
		g_A3DErrLog.Log("A3DMuscleMeshImp::Update, fail to update the muscle organ!");
		return false;
	}

	// if not modifed since last update or by muscle tension animation track data
	if( !m_pMuscleOrgan->IsChanged() )
		return true;

	// let my muscle organ deform me.
	if( !m_pMuscleOrgan->DeformMesh() )
	{
		g_A3DErrLog.Log("A3DMuscleMeshImp::Update, fail to let muscle organ deform me!");
		return false;
	}

	m_pMuscleOrgan->ClearChangeFlag();

	// now fill the stream
	// only update the stream when render, because there may be a skin processing
	//m_pA3DStream->SetVerts((BYTE *) m_aDeformedVerts, m_iNumVert);
	return true;
}

bool A3DMuscleMeshImp::CreateDeformedBuffers()
{
	//	Release old buffers
	ReleaseDeformedBuffers();

	if (m_iNumVert > 0)
	{
		m_aDeformedVerts = new A3DBVERTEX3[m_iNumVert];

		if (!m_aDeformedVerts)
		{
			g_A3DErrLog.Log("A3DMuscleMeshImp::CreateDeformedBuffers, Not enough memory !");
			return false;
		}

		// we duplicate the verts from original buffers at first.
		memcpy(m_aDeformedVerts, m_pCoreMesh->GetOriginVertexBuf(), sizeof(A3DBVERTEX3) * m_iNumVert);
	}

	return true;
}

void A3DMuscleMeshImp::ReleaseDeformedBuffers()
{
	if (m_aDeformedVerts)
	{
		delete [] m_aDeformedVerts;
		m_aDeformedVerts = NULL;
	}
}

bool A3DMuscleMeshImp::CreateStream()
{
	if (!m_pA3DEngine->GetA3DDevice())
		return true;

	A3DDevice* pA3DDevice = m_pA3DEngine->GetA3DDevice();

	//	Release old stream
	ReleaseStream();

	if (!(m_pA3DStream = new A3DStream))
	{
		g_A3DErrLog.Log("A3DMuscleMeshImp::CreateStream, Not Enough Memory!");
		return false;
	}

	ASSERT(!(m_iNumIdx % 3));

	if (!m_pA3DStream->Init(pA3DDevice, A3DVT_VERTEX, m_iNumVert, m_iNumIdx, 
			A3DSTRM_REFERENCEPTR, A3DSTRM_STATIC | A3DSTRM_REFERENCEPTR))
	{
		g_A3DErrLog.Log("A3DMuscleMeshImp::CreateStream, Failed to initialize A3DStream !");
		return false;
	}

	//	Fill index data
	if (!m_pA3DStream->SetIndices(m_pCoreMesh->GetOriginIndexBuf(), m_iNumIdx))
	{
		g_A3DErrLog.Log("A3DMuscleMeshImp::CreateStream, Failed to set index stream data !");
		return false;
	}

	m_pA3DStream->SetIndexRef(m_pCoreMesh->GetOriginIndexBuf());
	return true;
}

void A3DMuscleMeshImp::ReleaseStream()
{
	A3DRELEASE(m_pA3DStream);
}

bool A3DMuscleMeshImp::CreateMuscleOrgan(const A3DMuscleData& muscleData)
{
	ReleaseMuscleOrgan();

	m_pMuscleOrgan = new A3DMuscleOrgan();
	if( !m_pMuscleOrgan )
	{
		g_A3DErrLog.Log("A3DMuscleMeshImp::CreateMuscleOrgan, Not Enough Memory!");
		return false;
	}

	if( !m_pMuscleOrgan->Init(m_pSkinModel->GetSkeleton()->GetBoneNum(), m_pSkinModel->GetBlendMatrices(), this, muscleData) )
	{
		g_A3DErrLog.Log("A3DMuscleMeshImp::CreateMuscleOrgan, Failed to init muscle organ");
		return false;
	}

	return true;
}

void A3DMuscleMeshImp::ReleaseMuscleOrgan()
{
	if( m_pMuscleOrgan )
	{
		m_pMuscleOrgan->Release();
		delete m_pMuscleOrgan;
		m_pMuscleOrgan = NULL;
	}
}

void A3DMuscleMeshImp::CalculateVertices(int iDeltaTime, A3DSkinModel* pSkinModel)
{
}

A3DBVERTEX3* A3DMuscleMeshImp::GetOriginalVertexBuf()
{
	if( !m_pCoreMesh )
		return NULL;

	return m_pCoreMesh->GetOriginVertexBuf();
}