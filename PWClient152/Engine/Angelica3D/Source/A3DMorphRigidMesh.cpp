/*
 * FILE: A3DMorphRigidMesh.cpp
 *
 * DESCRIPTION: A3D morph rigid mesh class
 *
 * CREATED BY: duyuxin, 2003/11/9
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "A3DMorphRigidMesh.h"
#include "A3DPI.h"
#include "A3DVSDef.h"
#include "A3DStream.h"
#include "A3DSkin.h"
#include "A3DSkinModel.h"
#include "A3DDevice.h"
#include "A3DEngine.h"
#include "A3DConfig.h"
#include "A3DViewport.h"
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
//	Implement of A3DMorphRigidMesh
//
///////////////////////////////////////////////////////////////////////////

A3DMorphRigidMesh::A3DMorphRigidMesh()
{
	m_dwClassID		= A3D_CID_MORPHRIGIDMESH;
}

A3DMorphRigidMesh::~A3DMorphRigidMesh()
{
}

//	Initialize object
bool A3DMorphRigidMesh::Init(A3DEngine* pA3DEngine)
{
	if (!A3DRigidMesh::Init(pA3DEngine))
		return false;

	if (!m_MorphData.Init(this))
	{
		g_A3DErrLog.Log("A3DMorphRigidMesh::Init, Failed to initialize morph data!");
		return false;
	}

	return true;
}

//	Release object
void A3DMorphRigidMesh::Release()
{
	m_MorphData.Release();

	A3DRigidMesh::Release();
}

//	Load skin mesh data form file
bool A3DMorphRigidMesh::Load(AFile* pFile, A3DSkin* pSkin, int nSkinVersion)
{
	A3DDevice* pA3DDevice = m_pA3DEngine->GetA3DDevice();
	if (!pA3DDevice || pA3DDevice->GetSkinModelRenderMethod() != A3DDevice::SMRD_VERTEXSHADER)
		m_bStaticBuf = false;
	else
		m_bStaticBuf = true;

	if (!A3DRigidMesh::Load(pFile, pSkin, nSkinVersion))
		return false;

	if (!m_MorphData.Load(pFile, pSkin))
	{
		g_A3DErrLog.Log("A3DMorphRigidMesh::Load, Failed to load morph data!");
		return false;
	}

	return true;
}

//	Save skin mesh data from file
bool A3DMorphRigidMesh::Save(AFile* pFile,int nSkinVersion,int& nReturnValue)
{
	nReturnValue = RV_UNKNOWN_ERR;
	if (!A3DRigidMesh::Save(pFile, nSkinVersion, nReturnValue))
		return false;

	if (!m_MorphData.Save(pFile))
	{
		g_A3DErrLog.Log("A3DMorphRigidMesh::Save, Failed to save morph data!");
		return false;
	}

	if( nReturnValue == RV_UNKNOWN_ERR)
	{
		nReturnValue = RV_OK;
	}
	return true;
}

//	Render mesh
bool A3DMorphRigidMesh::Render(A3DViewport* pViewport, A3DSkinModel* pSkinModel, A3DSkin* pSkin)
{
	A3DDevice* pA3DDevice = m_pA3DEngine->GetA3DDevice();

	if (!pA3DDevice || !m_iNumVert || !m_iNumIdx)
		return true;

	if (m_bStaticBuf)
	{
		//	Fill morph position delta and channel indices
		m_MorphData.ApplyVSChannelData(pSkinModel);

		int iBaseVC = SMVSC_BLENDMAT0 + m_MorphData.GetCurVSConstNumUsedByMorph();

		A3DMATRIX4 mat = pSkinModel->GetBlendMatrix(m_MorphData.GetBoundBone());
		mat.Transpose();
		pA3DDevice->SetVertexShaderConstants(iBaseVC, &mat, 3);

		m_pA3DStream->Appear(0, false);

		if (!pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, m_iNumVert, 0, m_iNumIdx / 3))
			return false;

		//	A3DSkinMan will clear streams after all meshes are rendered
		//	Clear render state
	//	m_MorphData.ClearVSChannelData();
	}
	else	//	Software rendering
	{
		if (!RenderSoftware(pViewport, pSkinModel, pSkin))
			return false;
	}

	return true;
}

//	Software rendering
bool A3DMorphRigidMesh::RenderSoftware(A3DViewport* pViewport, A3DSkinModel* pSkinModel,
								A3DSkin* pSkin)
{
	A3DDevice* pA3DDevice = m_pA3DEngine->GetA3DDevice();

	const A3DMATRIX4& matWorld = pSkinModel->GetBlendMatrix(m_MorphData.GetBoundBone());
	pA3DDevice->SetWorldMatrix(matWorld);

	//	Get current channel sort array
	BYTE* pChannelSort = m_MorphData.CalcCurrentChannelWeights(pSkinModel);
	if (!pChannelSort)
		return A3DRigidMesh::Render(pViewport, pSkinModel, pSkin);
	
	A3DVERTEX* aVerts;

	//	Lock vertex buffer
	if (!m_pA3DStream->LockVertexBuffer(0, 0, (BYTE**) &aVerts, 0))
		return false;

	int i, j;
	A3DVECTOR3 vSrcPos, vSrcNormal;

	for (i=0; i < m_iNumVert; i++)
	{
		A3DVERTEX* pDest = &aVerts[i];
		A3DVERTEX* pSrc = &m_aVerts[i];

		//	Calculate source position
		vSrcPos.Set(pSrc->x, pSrc->y, pSrc->z);
		vSrcNormal.Set(pSrc->nx, pSrc->ny, pSrc->nz);

		for (j=0; j < m_MorphData.GetActiveMorphChannelNum(); j++)
		{
			A3DMorphMesh::CHANNEL* pChannel = m_MorphData.GetChannel(pChannelSort[j]);
			if (pChannel->fCurWeight == 0.0f)
				break;

			A3DMCVERTEX* pv = &pChannel->aOriginVerts[i];
			vSrcPos.x += pv->vPos[0] * pChannel->fCurWeight;
			vSrcPos.y += pv->vPos[1] * pChannel->fCurWeight;
			vSrcPos.z += pv->vPos[2] * pChannel->fCurWeight;

			vSrcNormal.x += pv->vNormal[0] * pChannel->fCurWeight;
			vSrcNormal.y += pv->vNormal[1] * pChannel->fCurWeight;
			vSrcNormal.z += pv->vNormal[2] * pChannel->fCurWeight;
		}
	
		vSrcNormal.Normalize();
	
		pDest->x = vSrcPos.x;
		pDest->y = vSrcPos.y;
		pDest->z = vSrcPos.z;

		pDest->nx = vSrcNormal.x;
		pDest->ny = vSrcNormal.y;
		pDest->nz = vSrcNormal.z;

		pDest->tu = pSrc->tu;
		pDest->tv = pSrc->tv;
	}

	if (!m_pA3DStream->UnlockVertexBuffer())
		return false;

	m_pA3DStream->Appear();

	if (!pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, m_iNumVert, 0, m_iNumIdx / 3))
		return false;

	return true;
}

//	Get approximate mesh data size
int A3DMorphRigidMesh::GetDataSize()
{
	int iSize = A3DRigidMesh::GetDataSize();
	iSize += m_MorphData.GetDataSize();
	return iSize;
}

