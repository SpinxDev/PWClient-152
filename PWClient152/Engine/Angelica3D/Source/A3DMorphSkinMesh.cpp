/*
 * FILE: A3DMorphSkinMesh.cpp
 *
 * DESCRIPTION: A3D morph skin mesh class
 *
 * CREATED BY: duyuxin, 2003/11/9
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "A3DMorphSkinMesh.h"
#include "A3DPI.h"
#include "A3DFuncs.h"
#include "A3DVSDef.h"
#include "A3DStream.h"
#include "A3DSkin.h"
#include "A3DSkinModel.h"
#include "A3DDevice.h"
#include "A3DEngine.h"
#include "A3DConfig.h"
#include "A3DViewport.h"
#include "A3DSkinMan.h"
#include "A3DCameraBase.h"
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
//	Implement of A3DMorphSkinMesh
//
///////////////////////////////////////////////////////////////////////////

A3DMorphSkinMesh::A3DMorphSkinMesh()
{
	m_dwClassID		= A3D_CID_MORPHSKINMESH;
}

A3DMorphSkinMesh::~A3DMorphSkinMesh()
{
}

//	Initialize object
bool A3DMorphSkinMesh::Init(A3DEngine* pA3DEngine)
{
	if (!A3DSkinMesh::Init(pA3DEngine))
		return false;

	if (!m_MorphData.Init(this))
	{
		g_A3DErrLog.Log("A3DMorphSkinMesh::Init, Failed to initialize morph data!");
		return false;
	}

	return true;
}

//	Release object
void A3DMorphSkinMesh::Release()
{
	m_MorphData.Release();

	A3DSkinMesh::Release();
}

//	Load skin mesh data form file
bool A3DMorphSkinMesh::Load(AFile* pFile, A3DSkin* pSkin, int nSkinVersion)
{
	if (!LoadData(pFile, pSkin, nSkinVersion))
		return false;

	//	Create sub meshes
	if (!CreateSubMeshes(pSkin))
	{
		g_A3DErrLog.Log("A3DMorphSkinMesh::Load, Failed to create sub meshes!");
		return false;
	}

	return true;
}

//	Load mesh data form file
bool A3DMorphSkinMesh::LoadData(AFile* pFile, A3DSkin* pSkin, int nSkinVersion)
{
	if (!A3DSkinMesh::LoadData(pFile, nSkinVersion))
		return false;

	if (!m_MorphData.Load(pFile, pSkin))
	{
		g_A3DErrLog.Log("A3DMorphSkinMesh::LoadData, Failed to load morph data!");
		return false;
	}

	return true;
}

//	Save skin mesh data from file
bool A3DMorphSkinMesh::Save(AFile* pFile,int nSkinVersion,int& nReturnValue)
{
	nReturnValue = RV_UNKNOWN_ERR;
	if (!A3DSkinMesh::Save(pFile, nSkinVersion, nReturnValue))
		return false;

	if (!m_MorphData.Save(pFile))
	{
		g_A3DErrLog.Log("A3DMorphSkinMesh::Save, Failed to save morph data!");
		return false;
	}
	
	if( nReturnValue == RV_UNKNOWN_ERR)
	{
		nReturnValue = RV_OK;
	}
	return true;
}

//	Create sub meshes
bool A3DMorphSkinMesh::CreateSubMeshes(A3DSkin* pSkin)
{
	if (!m_pA3DEngine->GetA3DDevice() || !m_iNumVert || !m_iNumIdx)
		return true;

	ASSERT(!(m_iNumIdx % 3));

	//	Morph skin mesh doesn't consider SMRD_INDEXEDVERTMATRIX
	if (m_pA3DEngine->GetA3DDevice()->GetSkinModelRenderMethod() == A3DDevice::SMRD_INDEXEDVERTMATRIX)
	{
		CalcVSBlendMatNum();
		return CreateSoftwareSubMeshes();
	}
	
	return A3DSkinMesh::CreateSubMeshes(pSkin);
}

//	Calculate number of blend matrix can be used in vertex shader
void A3DMorphSkinMesh::CalcVSBlendMatNum()
{
	A3DDevice* pA3DDevice = m_pA3DEngine->GetA3DDevice();
	if (!pA3DDevice)
	{
		m_iNumVSBlendMat = 0;
		return;
	}

	int iUsedByMorph = m_MorphData.GetMaxVSConstNumUsedByMorph();
	m_iNumVSBlendMat = (pA3DDevice->GetVSConstantNum() - iUsedByMorph - SMVSC_BLENDMAT0) / 3;
}

//	Render mesh
bool A3DMorphSkinMesh::Render(A3DViewport* pViewport, A3DSkinModel* pSkinModel, A3DSkin* pSkin)
{
	A3DDevice* pA3DDevice = m_pA3DEngine->GetA3DDevice();

	if (!pA3DDevice || !m_iNumVert || !m_iNumIdx)
		return true;

	int iRenderMethod = pA3DDevice->GetSkinModelRenderMethod();

	if (iRenderMethod == A3DDevice::SMRD_VERTEXSHADER)
	{
		//	Fill morph position delta and channel indices
		m_MorphData.ApplyVSChannelData(pSkinModel);

		int iBaseVC = SMVSC_BLENDMAT0 + m_MorphData.GetCurVSConstNumUsedByMorph();

		if (m_bSplited)
		{
			ASSERT(pSkinModel);

			int i, j;

			//	Get transposed blend matrices
			const A3DMATRIX4* aTBlendMats = pSkinModel->GetTVSBlendMatrices();
			A3DSkinMan* pSkinMan = pSkinModel->GetA3DEngine()->GetA3DSkinMan();
			const A3DVECTOR3& vc = pSkinMan->GetWorldCenter();
			A3DCameraBase* pCamera = pViewport->GetCamera();
			A3DMATRIX4 matView = a3d_ViewMatrix(pCamera->GetPos()-vc, pCamera->GetDir(), pCamera->GetUp(), 0.0f);
			matView.Transpose();

			//	Render all sub meshes
			for (i=0; i < m_aSubMeshes.GetSize(); i++)
			{
				SUBMESH* pSubMesh = (SUBMESH*)m_aSubMeshes[i];

				//	Apply blend matrices
				for (j=0; j < pSubMesh->iMatCount; j++)
				{
					int iMat = pSubMesh->aMatIndices[j];
					A3DMATRIX4 matBoneView = matView * aTBlendMats[iMat];
					pA3DDevice->SetVertexShaderConstants(iBaseVC+j*3, &aTBlendMats[iMat], 3);
				}

				pSubMesh->pA3DStream->Appear(0, false);

				int iNumVert = pSubMesh->pA3DStream->GetVertCount();
				int iNumFace = pSubMesh->pA3DStream->GetIndexCount() / 3;

				if (!pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, iNumVert, 0, iNumFace))
					return false;
			}
		}
		else	//	Not split
		{
			ApplyVSBlendMatrices(pSkinModel, pSkin, pViewport, iBaseVC);

			SUBMESH* pSubMesh = (SUBMESH*)m_aSubMeshes[0];
			pSubMesh->pA3DStream->Appear(0, false);

			if (!pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, m_iNumVert, 0, m_iNumIdx / 3))
				return false;
		}

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
bool A3DMorphSkinMesh::RenderSoftware(A3DViewport* pViewport, A3DSkinModel* pSkinModel, 
								A3DSkin* pSkin)
{
	A3DDevice* pA3DDevice = m_pA3DEngine->GetA3DDevice();
	A3DStream* pA3DStream = ((SUBMESH*)m_aSubMeshes[0])->pA3DStream;

	pA3DDevice->SetWorldMatrix(A3D::g_matIdentity);

	const A3DMATRIX4* aBlendMats = pSkinModel->GetBlendMatrices();

	//	Get current channel sort array
	BYTE* pChannelSort = m_MorphData.CalcCurrentChannelWeights(pSkinModel);
	if (!pChannelSort)
		return A3DSkinMesh::RenderSoftware(pViewport, pSkinModel, pSkin);
	
	A3DVERTEX* aVerts;

	//	Lock vertex buffer
	if (!pA3DStream->LockVertexBuffer(0, 0, (BYTE**) &aVerts, 0))
		return false;

	int i, j;
	A3DVECTOR3 vPos, vNormal, vSrcPos, vSrcNormal;
	float fTotalWeight;

	for (i=0; i < m_iNumVert; i++)
	{
		A3DVERTEX* pDest = &aVerts[i];
		SKIN_VERTEX* pSrc = &m_aVerts[i];

		//	Calculate source position
		vSrcPos.Set(pSrc->vPos[0], pSrc->vPos[1], pSrc->vPos[2]);
		vSrcNormal.Set(pSrc->vNormal[0], pSrc->vNormal[1], pSrc->vNormal[2]);

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

		vPos.Clear();
		vNormal.Clear();

		fTotalWeight = 0;

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

//	Get approximate mesh data size
int A3DMorphSkinMesh::GetDataSize()
{
	int iSize = A3DSkinMesh::GetDataSize();
	iSize += m_MorphData.GetDataSize();
	return iSize;
}


