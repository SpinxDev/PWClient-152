/*
 * FILE: A3DVertexCollector.cpp
 *
 * DESCRIPTION: Used to manager vertices which will be rendered
 *
 * CREATED BY: duyuxin, 2001/12/20
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "A3DVertexCollector.h"
#include "A3DPI.h"
#include "A3DFuncs.h"
#include "A3DViewport.h"
#include "A3DEngine.h"
#include "A3DTextureMan.h"
#include "A3DConfig.h"

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
//	Implement
//
///////////////////////////////////////////////////////////////////////////

A3DVertexCollector::A3DVertexCollector()
{
	m_pA3DDevice = NULL;

	m_aSizes[0]	= sizeof (A3DVERTEX);
	m_aSizes[1] = sizeof (A3DLVERTEX);
	m_aSizes[2] = sizeof (A3DTLVERTEX);
}

A3DVertexCollector::~A3DVertexCollector()
{
}

/*	Initialize object.

	Return true for success, otherwise return false.

	pA3DDevice: valid A3D device's address
*/
bool A3DVertexCollector::Init(A3DDevice* pA3DDevice)
{
	if( g_pA3DConfig->GetRunEnv() == A3DRUNENV_PURESERVER )
		return true;

	m_pA3DDevice = pA3DDevice;

	if (!m_TextureList.Init())
	{
		g_A3DErrLog.Log("A3DVertexCollector::Init failed to init texture list.");
		return false;
	}

	return true;
}

//	Release resources
void A3DVertexCollector::Release()
{
	if( !m_pA3DDevice ) return;

	ReleaseTextureSlots();

	m_TextureList.Release();

	m_pA3DDevice = NULL;
}

//	Reset collector
bool A3DVertexCollector::Reset()
{
	if( !m_pA3DDevice ) return true;

	ReleaseTextureSlots();

	m_TextureList.Reset();

	return true;
}

//	Release texture slots
void A3DVertexCollector::ReleaseTextureSlots()
{
	ALISTELEMENT* pElem = m_TextureList.GetFirst();
	TEXTURESLOT* pSlot;
	SLOTBUFFER* pSlotBuf;

	while (pElem != m_TextureList.GetTail())
	{
		pSlot = (PTEXTURESLOT)pElem->pData;

		//	Release buffers
		pSlotBuf = &pSlot->aBufs[0];
		if (pSlotBuf->bValid && pSlotBuf->pA3DStream)
		{
			pSlotBuf->pA3DStream->Release();
			delete pSlotBuf->pA3DStream;
		}

		pSlotBuf = &pSlot->aBufs[1];
		if (pSlotBuf->bValid && pSlotBuf->pA3DStream)
		{
			pSlotBuf->pA3DStream->Release();
			delete pSlotBuf->pA3DStream;
		}

		pSlotBuf = &pSlot->aBufs[2];
		if (pSlotBuf->bValid && pSlotBuf->pA3DStream)
		{
			pSlotBuf->pA3DStream->Release();
			delete pSlotBuf->pA3DStream;
		}

		//	Release texture and material object, but don't call their Release() routines.
		//	Because we created them only with pointer's reference, no real data in them
		if (pSlot->pTexture)
			m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(pSlot->pTexture);

		if (pSlot->pMaterial)
			delete pSlot->pMaterial;
		
		//	Release slot
		free(pSlot);

		pElem = pElem->pNext;
	}
}

/*	Add texture information and create texture slot, if texture slot has the same 
	information has been existed, it's handle will be returned.

	Return a handle of texture information for success, otherwise return 0 for failure

	iVertexType: A3D type (not A3DVertexCollector type) of vertex to which specified
				 texture will be applied to.
	pTexture: texture object, can be NULL
	pMaterial: material object, can be NULL
	pShader: shader object, can be NULL
*/
DWORD A3DVertexCollector::RegisterTexture(A3DVERTEXTYPE iVertexType, A3DTexture* pTexture, 
										  A3DMaterial* pMaterial, A3DSHADER* pShader)
{
	// We return a fake texture handle, it is only to deal with error check 
	if( !m_pA3DDevice ) return 0x12345678;

	ALISTELEMENT* pElem = m_TextureList.GetFirst();
	TEXTURESLOT* pSlot;
	bool bFind = false;

	while (pElem != m_TextureList.GetTail())
	{
		pSlot = (PTEXTURESLOT)pElem->pData;

		if ((!pSlot->pTexture && !pTexture) || (pSlot->pTexture && pTexture &&
			pSlot->pTexture->GetD3DTexture() == pTexture->GetD3DTexture()))
		{
			if ((!pMaterial && !pSlot->pMaterial) ||
				(pMaterial && pSlot->pMaterial && pSlot->pMaterial->Match(pMaterial)))
			{
				if (!pShader || (pShader->SrcBlend == pSlot->Shader.SrcBlend &&
					pShader->DestBlend == pSlot->Shader.DestBlend))
				{
					bFind = true;
					break;
				}
			}
		}

		pElem = pElem->pNext;
	}

	if (bFind)
	{
		//	Add a vertex buffer to this slot
		if (!AllocateSlotBuffer(pSlot, VTA3DToCollector(iVertexType)))
			return 0;

		return (DWORD)pSlot->pRecordElement;
	}

	if (!(pSlot = AllocateSlot(VTA3DToCollector(iVertexType))))
		return 0;

	if (pShader)
		pSlot->Shader = *pShader;

	if (pTexture)
	{
		//	Save texture information
		if( !m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFile(pTexture->GetMapFile(), &pSlot->pTexture) )
			return false;
		/*
		if (!(pSlot->pTexture = new A3DTexture))
		{
			g_A3DErrLog.Log("A3DVertexCollector::RegisterTexture failed to create texture");
			return 0;
		}
		
		pSlot->pTexture->Init(m_pA3DDevice, pTexture->GetD3DTexture());*/
	}
	else
		pSlot->pTexture = NULL;

	if (pMaterial)
	{
		//	Save material information
		if (!(pSlot->pMaterial = new A3DMaterial))
		{
			g_A3DErrLog.Log("A3DVertexCollector::RegisterTexture failed to create material");
			return 0;
		}
		
		pSlot->pMaterial->Init(m_pA3DDevice, pMaterial->GetMaterialParam());
	}
	else
		pSlot->pMaterial = NULL;

	m_TextureList.Append(pSlot, &pSlot->pRecordElement);

	return (DWORD)pSlot->pRecordElement;
}

/*	Allocate vertex buffer for a texture slot.

	Return true for success, otherwise return false.

	pSlot: specified texture slot to which new vertex buffer will be added.
	iVertexType: A3DVertexCollector vertex type
*/
bool A3DVertexCollector::AllocateSlotBuffer(PTEXTURESLOT pSlot, int iVertexType)
{
	if (iVertexType == -1 || iVertexType >= NUMVERTTYPE)
		return false;

	//	Buffer has been existed ?
	if (pSlot->aBufs[iVertexType].bValid)
		return true;

	A3DStream* pStream;

	//	Create vertex and index stream
	if (!(pStream = new A3DStream))
	{
		g_A3DErrLog.Log("A3DVertexCollector::AllocateSlotBuffer, not enough memory for stream!");
		return false;
	}

	if (!pStream->Init(m_pA3DDevice, VTCollectorToA3D(iVertexType), MAXNUM_VERTINSLOT,
		MAXNUM_INDEXINSLOT, A3DSTRM_REFERENCEPTR, A3DSTRM_REFERENCEPTR))
	{
		g_A3DErrLog.Log("A3DVertexCollector::AllocateSlotBuffer, Failed to initialize stream!");
		return false;
	}

	pSlot->aBufs[iVertexType].bValid	 = true;
	pSlot->aBufs[iVertexType].pA3DStream = pStream;
	pSlot->aBufs[iVertexType].iNumIndex	 = 0;
	pSlot->aBufs[iVertexType].iNumVert	 = 0;

	return true;
}

/*	Allocate a new texture slot.

	Return new slot's address for success, otherwise return NULL

	iVertexType: A3DVertexCollector type of vertex new slot will have. If this parameter
				 is -1, a empty slot without vertex buffer will be created.
*/
A3DVertexCollector::TEXTURESLOT* A3DVertexCollector::AllocateSlot(int iVertexType)
{
	//	Create a new texture information slot and add it to list tail
	TEXTURESLOT* pSlot = (TEXTURESLOT*)malloc(sizeof (TEXTURESLOT));
	if (!pSlot)
	{
		g_A3DErrLog.Log("A3DVertexCollector::AllocateSlot not enough memory");
		return 0;
	}

	memset(pSlot, 0, sizeof (TEXTURESLOT));

	pSlot->Shader.SrcBlend	= A3DBLEND_SRCALPHA;
	pSlot->Shader.DestBlend	= A3DBLEND_INVSRCALPHA;

	if (iVertexType != -1)
	{
		if (!AllocateSlotBuffer(pSlot, iVertexType))
		{
			free(pSlot);
			g_A3DErrLog.Log("A3DVertexCollector::AllocateSlot, Failed to create slot buffer!");
			return NULL;
		}
	}

	return pSlot;
}

/*	Push vertices and ready to render.

	Return true for success, otherwise return false.

	hTexture: handle of texture information returned by A3DVertexCollector::RegisterTexture.
	iType: A3D vertex type.
	aInVerts: vertex buffer.
	iNumVert: number of vertex
	aIndices: index buffer
	iNumIdx: number of index
*/
bool A3DVertexCollector::PushVertices(DWORD hTexture, A3DVERTEXTYPE iType, void* aInVerts,
									  int iNumVert, WORD* aInIndices, int iNumIdx)
{
	if( !m_pA3DDevice ) return true;

	int iVertexType = VTA3DToCollector(iType);

	if (!hTexture || iVertexType == -1)
		return false;

	//	Search a proper texture slot
	TEXTURESLOT* pSlot = (TEXTURESLOT*)((ALISTELEMENT*)hTexture)->pData;
	if (!pSlot->aBufs[iVertexType].bValid)
		return false;

	PSLOTBUFFER pSlotBuf = &pSlot->aBufs[iVertexType];

	if (pSlotBuf->iNumVert + iNumVert > MAXNUM_VERTINSLOT ||
		pSlotBuf->iNumIndex + iNumIdx > MAXNUM_INDEXINSLOT)
	{
		//	Set world matrix to identity matrix
		m_pA3DDevice->SetWorldMatrix(a3d_IdentityMatrix());

		//	Appear texture and material
		if (pSlot->pTexture)
			pSlot->pTexture->Appear(0);
		else
			m_pA3DDevice->ClearTexture(0);

		m_pA3DDevice->SetZWriteEnable(false);
		m_pA3DDevice->SetFaceCull(A3DCULL_NONE);
		m_pA3DDevice->SetLightingEnable(false);

		if (pSlot->pMaterial)
			pSlot->pMaterial->Appear();

		m_pA3DDevice->SetSourceAlpha(pSlot->Shader.SrcBlend);
		m_pA3DDevice->SetDestAlpha(pSlot->Shader.DestBlend);
		
		if (!FlushBuffer(pSlot, pSlotBuf))
			return false;

		m_pA3DDevice->SetZWriteEnable(true);
		m_pA3DDevice->SetFaceCull(A3DCULL_CCW);
		m_pA3DDevice->SetLightingEnable(true);

		if (pSlot->pTexture)
			pSlot->pTexture->Disappear(0);
		//	Restore render state
		m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
		m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
	}

	A3DStream* pStream = pSlotBuf->pA3DStream;
	BYTE* aVerts;
	WORD* aIndices;

	int iSize = m_aSizes[iVertexType];

	//	Lock vertex and index buffer
	if (!pStream->LockIndexBuffer(0, 0, (BYTE**) &aIndices, 0))
	{
		g_A3DErrLog.Log("A3DVertexCollector::PushVertices, Failed to lock index buffer!");
		return false;
	}
	
	if (!pStream->LockVertexBuffer(0, 0, &aVerts, 0))
	{
		g_A3DErrLog.Log("A3DVertexCollector::PushVertices, Failed to lock vertex buffer!");
		return false;
	}

	//	Add vertices and indices to buffer
	memcpy(aVerts + pSlotBuf->iNumVert * iSize, aInVerts, iSize * iNumVert);

	aIndices += pSlotBuf->iNumIndex;
	for (int i=0; i < iNumIdx; i++)
		aIndices[i] = pSlotBuf->iNumVert + aInIndices[i];

	pStream->UnlockIndexBuffer();
	pStream->UnlockVertexBuffer();

	pSlotBuf->iNumVert	+= iNumVert;
	pSlotBuf->iNumIndex	+= iNumIdx;

	return true;
}

/*	Flush specified vertex buffer.

	Return true for success, otherwise return false.

	pSlot: texture slot.
	pSlotBuf: slot buffer will be flushed
*/
bool A3DVertexCollector::FlushBuffer(PTEXTURESLOT pSlot, PSLOTBUFFER pSlotBuf)
{
	if( !m_pA3DDevice ) return true;

	A3DStream* pStream = pSlotBuf->pA3DStream;

	pStream->Appear();
	
	if (!m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, pSlotBuf->iNumVert, 
											0, pSlotBuf->iNumIndex / 3))
	{
		g_A3DErrLog.Log("A3DVertexCollector::FlushBuffer, render failed!");
		return false;
	}
				
	pSlotBuf->iNumIndex	= 0;
	pSlotBuf->iNumVert	= 0;

	return true;
}

//	Render all remained vertices
bool A3DVertexCollector::Flush(A3DViewport* pCurViewport)
{
	if( !m_pA3DDevice ) return true;

	ALISTELEMENT* pElem = m_TextureList.GetFirst();
	TEXTURESLOT* pSlot;
	SLOTBUFFER* pSlotBuf;

	//	Set world matrix to identity matrix
	m_pA3DDevice->SetWorldMatrix(a3d_IdentityMatrix());

	while (pElem != m_TextureList.GetTail())
	{
		pSlot = (PTEXTURESLOT)pElem->pData;

		//	Apply texture and material
		if (pSlot->pTexture)
			pSlot->pTexture->Appear(0);
		else
			m_pA3DDevice->ClearTexture(0);

		if (pSlot->pMaterial)
			pSlot->pMaterial->Appear();

		m_pA3DDevice->SetSourceAlpha(pSlot->Shader.SrcBlend);
		m_pA3DDevice->SetDestAlpha(pSlot->Shader.DestBlend);
		
		pSlotBuf = &pSlot->aBufs[0];
		if (pSlotBuf->bValid && pSlotBuf->iNumVert)
			FlushBuffer(pSlot, pSlotBuf);

		pSlotBuf = &pSlot->aBufs[1];
		if (pSlotBuf->bValid && pSlotBuf->iNumVert)
			FlushBuffer(pSlot, pSlotBuf);

		pSlotBuf = &pSlot->aBufs[2];
		if (pSlotBuf->bValid && pSlotBuf->iNumVert)
			FlushBuffer(pSlot, pSlotBuf);

		if (pSlot->pTexture)
			pSlot->pTexture->Disappear(0);
		
		//	Restore render state
		m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
		m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
		
		pElem = pElem->pNext;
	}

	return true;
}


