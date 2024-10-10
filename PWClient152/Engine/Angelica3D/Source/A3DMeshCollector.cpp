/*
 * FILE: A3DMeshCollector.h
 *
 * DESCRIPTION: Used to manager meshes which will be rendered
 *
 * CREATED BY: duyuxin, 2001/12/17
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "A3DMeshCollector.h"
#include "A3DPI.h"
#include "A3DFuncs.h"
#include "A3DViewport.h"
#include "A3DEngine.h"
#include "A3DDevice.h"
#include "A3DTexture.h"
#include "A3DMesh.h"
#include "A3DTextureMan.h"
#include "A3DStream.h"
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
//	Implement
//
///////////////////////////////////////////////////////////////////////////

A3DMeshCollector::A3DMeshCollector() : m_aMeshPool(100, 20)
{
	m_pA3DDevice	= NULL;
	m_iPoolCnt		= 0;
}

A3DMeshCollector::~A3DMeshCollector()
{
}

/*	Initialize object.

	Return true for success, otherwise return false.

	pA3DDevice: valid address of A3DDevice object.
*/
bool A3DMeshCollector::Init(A3DDevice* pA3DDevice)
{
	m_pA3DDevice = pA3DDevice;

	if (!m_TextureList.Init())
	{
		g_A3DErrLog.Log("A3DMeshCollector::Init failed to init texture list.");
		return false;
	}

	m_pVertexBuffer = (A3DVERTEX *)malloc(MAXNUM_RENDERVERT * sizeof (A3DVERTEX));
	if( NULL == m_pVertexBuffer )
	{
		g_A3DErrLog.Log("A3DMeshCollector::Init, not enough memory!");
		return false;
	}

	m_pIndexBuffer = (WORD *) malloc(MAXNUM_RENDERIDX * sizeof (WORD));
	if( NULL == m_pIndexBuffer )
	{
		g_A3DErrLog.Log("A3DMeshCollector::Init, not enough memory!");
		return false;
	}

	m_iPoolCnt = 0;

	return true;
}

//	Release resource
void A3DMeshCollector::Release()
{
	ReleaseAllTextures(false);
	ReleaseAllMeshInfo();

	//	Release vertex buffer and index buffer;
	if (m_pVertexBuffer)
	{
		free(m_pVertexBuffer);
		m_pVertexBuffer = NULL;
	}

	if (m_pIndexBuffer)
	{
		free(m_pIndexBuffer);
		m_pIndexBuffer = NULL;
	}

	m_pA3DDevice	= NULL;
	m_iPoolCnt		= 0;
}

//	Reset mesh collector
bool A3DMeshCollector::Reset()
{
	ReleaseAllTextures(true);
	ReleaseAllMeshInfo();
	m_iPoolCnt = 0;
	return true;
}

//	Release texture list
void A3DMeshCollector::ReleaseAllTextures(bool bReset)
{
	//	Release texture slots
	ALISTELEMENT* pElem = m_TextureList.GetFirst();
	while (pElem != m_TextureList.GetTail())
	{
		PTEXTURESLOT pSlot = (PTEXTURESLOT)pElem->pData;

		//	Release texture and material object, but don't call their Release() routines.
		//	Because we created them only with pointer's reference, no real data in them
		if (pSlot->pTexture)
			m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(pSlot->pTexture);

		if (pSlot->pMaterial)
			delete pSlot->pMaterial;

		delete pSlot;
		pElem = pElem->pNext;
	}

	if (bReset)
		m_TextureList.Reset();
	else
		m_TextureList.Release();
}

//	Release all mesh information
void A3DMeshCollector::ReleaseAllMeshInfo()
{
	int i;
	for (i=0; i < m_aMeshPool.GetSize(); i++)
		delete m_aMeshPool[i];
	
	m_aMeshPool.RemoveAll();
}

/*	Add mesh's texture and material information then create a texture information
	slot, if this information don't exist.

	Return a handle of texture's information, it will be used by PrepareMeshToRender.
	0 means failure.

	pTexture: texture used by mesh
	pMaterial: material used by mesh
*/	
DWORD A3DMeshCollector::RegisterTexture(A3DTexture* pTexture, A3DMaterial* pMaterial)
{
	ALISTELEMENT* pElem = m_TextureList.GetFirst();
	TEXTURESLOT* pSlot;

	while (pElem != m_TextureList.GetTail())
	{
		pSlot = (PTEXTURESLOT)pElem->pData;

		if (pSlot->pTexture->GetD3DTexture() == pTexture->GetD3DTexture() &&
			pSlot->pMaterial->Match(pMaterial))
			return (DWORD)pElem;

		pElem = pElem->pNext;
	}

	//	Create a new texture information slot and add it to list tail
	if (!(pSlot = new TEXTURESLOT))
	{
		g_A3DErrLog.Log("A3DMeshCollector::RegisterTexture not enough memory");
		return 0;
	}

	//	Save texture information
	/*
	if (!(pSlot->pTexture = new A3DTexture))
	{
		g_A3DErrLog.Log("A3DMeshCollector::RegisterTexture failed to create texture");
		return 0;
	}
	
	pSlot->pTexture->Init(m_pA3DDevice, pTexture->GetD3DTexture());*/

	if( !m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFile(pTexture->GetMapFile(), &pSlot->pTexture) )
		return false;

	//	Save material information
	if (!(pSlot->pMaterial = new A3DMaterial))
	{
		g_A3DErrLog.Log("A3DMeshCollector::RegisterTexture failed to create material");
		return 0;
	}

	pSlot->pMaterial->Init(m_pA3DDevice, pMaterial->GetMaterialParam());

	pSlot->iMeshCnt	= 0;

	m_TextureList.Append(pSlot, &pSlot->pRecordElement);

	return (DWORD)pSlot->pRecordElement;
}

/*	Prepare a mesh to be rendereded.

	Return true for success, otherwise return false.

	pMesh: mesh object's address.
	iCurFrame: mesh's current frame.
	matTrans: mesh's translate matrix
	hTexture: texture information's handle returned by A3DMeshCollector::RegisterTexture()
*/	
bool A3DMeshCollector::PrepareMeshToRender(A3DMesh* pMesh, int iCurFrame, A3DMATRIX4 matTrans, 
										   DWORD hTexture)
{
	//	Get a new mesh information structure
	if (m_iPoolCnt == m_aMeshPool.GetSize())
	{
		MESHINFO* pMeshInfo = new MESHINFO;
		pMeshInfo->pMesh		= pMesh;
		pMeshInfo->iCurFrame	= iCurFrame;
		pMeshInfo->matTrans		= matTrans;
		m_aMeshPool.Add(pMeshInfo);
	}
	else
	{
		MESHINFO* pMeshInfo = m_aMeshPool[m_iPoolCnt];
		pMeshInfo->pMesh		= pMesh;
		pMeshInfo->iCurFrame	= iCurFrame;
		pMeshInfo->matTrans		= matTrans;
	}

	m_iPoolCnt++;

	//	Search a proper texture slot
	TEXTURESLOT* pSlot = (TEXTURESLOT*)((ALISTELEMENT*)hTexture)->pData;

	if (pSlot->iMeshCnt == pSlot->aMeshInfo.GetSize())
		pSlot->aMeshInfo.Add(m_aMeshPool[m_iPoolCnt-1]);
	else
		pSlot->aMeshInfo[pSlot->iMeshCnt] = m_aMeshPool[m_iPoolCnt-1];

	pSlot->iMeshCnt++;

	return true;
}

//	Render
bool A3DMeshCollector::Render(A3DViewport* pCurViewport)
{
	ALISTELEMENT* pElem = m_TextureList.GetFirst();
	TEXTURESLOT* pSlot;
	int i, iNumIndex, iNumVert, iNumPrim, iIdxInMesh, iVertInMesh;
	MESHINFO* pMeshInfo;
	A3DMATRIX4 matWorld = a3d_IdentityMatrix();

	m_pA3DDevice->SetWorldMatrix(matWorld);
	m_pA3DDevice->SetFVF(A3DFVF_A3DVERTEX);

	//	Clear pool counter for next frame
	m_iPoolCnt = 0;

	if (1)
	{
		while (pElem != m_TextureList.GetTail())
		{
			pSlot = (PTEXTURESLOT)pElem->pData;
			if (!pSlot->iMeshCnt)
			{
				pElem = pElem->pNext;
				continue;
			}

			int iNumMesh = pSlot->iMeshCnt;
			pSlot->iMeshCnt = 0;

			//	Apply texture and material
			pSlot->pTexture->Appear(0);
			pSlot->pMaterial->Appear();

			iNumVert	= 0;
			iNumIndex	= 0;
			iNumPrim	= 0;

			//	Enumlate all meshes in this texture slot and collect their vertices together
			for (i=0; i < iNumMesh; i++)
			{
				pMeshInfo = pSlot->aMeshInfo[i];

				pMeshInfo->pMesh->UpdateToFrame(pMeshInfo->iCurFrame);
				pMeshInfo->pMesh->UpdateLOD(pCurViewport, pMeshInfo->matTrans);

				iVertInMesh = pMeshInfo->pMesh->GetShowVertCount();
				iIdxInMesh	= pMeshInfo->pMesh->GetShowIndexCount();

				// If has reduced polygons down to less than one triangle, needn't render any more
				if( iIdxInMesh < 3 )
					continue;

				//	Collect them and render;
				if (iNumVert + iVertInMesh > MAXNUM_RENDERVERT ||
					iNumIndex + iIdxInMesh > MAXNUM_RENDERIDX)
				{
					if (iVertInMesh > MAXNUM_RENDERVERT || iIdxInMesh > MAXNUM_RENDERIDX)
					{
						//	This shouldn't happend
						g_A3DErrLog.Log("A3DMeshCollector::Render, too much vertex in mesh!");
						return false;
					}

					//	Render collected vertices
					if (!m_pA3DDevice->DrawIndexedPrimitiveUP(A3DPT_TRIANGLELIST, 0, iNumVert, iNumPrim, m_pIndexBuffer, A3DFMT_INDEX16, m_pVertexBuffer, sizeof (A3DVERTEX)))
					{
						g_A3DErrLog.Log("A3DMeshCollector::Render, render failed!");
						return false;
					}

					iNumVert	= 0;
					iNumIndex	= 0;
					iNumPrim	= 0;
				}

				//	Let mesh fill it's vertex and index data into buffer
				pMeshInfo->pMesh->RenderToBuffer(pCurViewport, iNumVert, m_pVertexBuffer+iNumVert, 
												m_pIndexBuffer+iNumIndex, pMeshInfo->matTrans, 
												pMeshInfo->iCurFrame, &iVertInMesh, &iIdxInMesh);

				iNumVert	+= iVertInMesh;
				iNumIndex	+= iIdxInMesh;
				iNumPrim	+= iIdxInMesh / 3;
			}

			//	Render remained vertices
			if (iNumVert)
			{
				if (!m_pA3DDevice->DrawIndexedPrimitiveUP(A3DPT_TRIANGLELIST, 0, iNumVert, iNumPrim, m_pIndexBuffer, A3DFMT_INDEX16, m_pVertexBuffer, sizeof (A3DVERTEX)))
				{
					g_A3DErrLog.Log("A3DMeshCollector::Render, render failed!");
					return false;
				}
			}

			pSlot->pTexture->Disappear(0);

			pElem = pElem->pNext;
		}
	}
	else
	{
		/*
		while (pElem != m_TextureList.GetTail())
		{
			pSlot = (PTEXTURESLOT)pElem->pData;

			//	Apply texture and material
			pSlot->pTexture->Appear(0);
			pSlot->pMaterial->Appear();

			//	Enumlate all meshes in this texture slot and collect their vertices together
			for (i=0; i < pSlot->iMeshCnt; i++)
			{
				pMeshInfo = pSlot->aMeshInfo[i];

				//	Let mesh render its self now;fill it's vertex and index data into buffer
				m_pA3DDevice->SetWorldMatrix(pMeshInfo->matTrans);
				pMeshInfo->pMesh->UpdateToFrame(pMeshInfo->iCurFrame);
				pMeshInfo->pMesh->UpdateLOD(pCurViewport, pMeshInfo->matTrans);
				pMeshInfo->pMesh->RenderDirect(pCurViewport);
			}

			pSlot->pTexture->Disappear(0);
			pSlot->iMeshCnt = 0;

			pElem = pElem->pNext;
		}*/
	}

	return true;
}

//	Remove unrendered mesh's information, call this function after render 
//	every frame.
void A3DMeshCollector::RemoveUnrenderedMeshes()
{
	ALISTELEMENT* pElem = m_TextureList.GetFirst();
	while (pElem != m_TextureList.GetTail())
	{
		TEXTURESLOT* pSlot = (PTEXTURESLOT)pElem->pData;
		pSlot->iMeshCnt = 0;
		pElem = pElem->pNext;
	}

	m_iPoolCnt = 0;
}
