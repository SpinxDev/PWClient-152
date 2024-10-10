/*
 * FILE: A3DIBLMesh.cpp
 *
 * DESCRIPTION: Class representing a mesh using image based lighting
 *
 * CREATED BY: Hedi, 2002/11/15
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "A3DIBLMesh.h"
#include "A3DPI.h"
#include "A3DDevice.h"
#include "A3DMacros.h"
#include "A3DFuncs.h"
#include "A3DTexture.h"
#include "A3DStream.h"
#include "A3DViewport.h"
#include "A3DTexture.h"
#include "A3DEngine.h"
#include "A3DTextureMan.h"

IAnimObj::IAnimObj()
{
	m_pA3DDevice = NULL;
}

IAnimObj::~IAnimObj()
{
}

A3DSubMesh::A3DSubMesh()
{
	m_nVertCount	= 0;
	m_nFaceCount	= 0;
}

A3DSubMesh::~A3DSubMesh()
{
}

A3DVSubMesh::A3DVSubMesh()
{
	m_pVertices	= NULL;
	m_pIndices		= NULL;
}

A3DVSubMesh::~A3DVSubMesh()
{
}

A3DLSubMesh::A3DLSubMesh()
{
	m_pLVertices	= NULL;
	m_pLIndices		= NULL;
}

A3DLSubMesh::~A3DLSubMesh()
{
}
  
bool A3DLSubMesh::Init(A3DDevice * pDevice)
{
	A3DSubMesh::Init(pDevice);

	m_pA3DDevice = pDevice;
	return true;
}

bool A3DLSubMesh::Release()
{
	return A3DSubMesh::Release();
}

bool A3DLSubMesh::AddVerts(void* pVertices, int nVertCount, A3DVERTEXTYPE vertexType)
{
	if( A3DVT_LVERTEX != vertexType )
		return false;

	int nNewVertCount = m_nVertCount + nVertCount;
	A3DLVERTEX * pVB = NULL;
	pVB = (A3DLVERTEX *) realloc(m_pLVertices, sizeof(A3DLVERTEX) * nNewVertCount);
	if( NULL == pVB )
	{
		g_A3DErrLog.Log("A3DLSubMesh::AddVerts(), Not enough memory!");
		return false;
	}

	m_pLVertices = pVB;
	memcpy(m_pLVertices + sizeof(A3DLVERTEX) * m_nVertCount, pVertices, sizeof(A3DLVERTEX) * nVertCount);

	m_nVertCount = nNewVertCount;
	return true;
}

bool A3DLSubMesh::AddFaces(void* pIndices, int nFaceCount, A3DFORMAT indexFormat)
{
	if( indexFormat != A3DFMT_INDEX16 )
		return false;

	int nNewFaceCount = m_nFaceCount + nFaceCount;
	WORD * pIB = NULL;
	pIB = (WORD *) realloc(m_pLIndices, sizeof(WORD) * nNewFaceCount * 3);
	if( NULL == pIB )
	{
		g_A3DErrLog.Log("A3DLSubMesh::AddFaces(), Not enough memory!");
		return false;
	}

	m_pLIndices = pIB;
	memcpy(m_pLIndices + sizeof(WORD) * m_nFaceCount * 3, pIndices, sizeof(WORD) * nFaceCount * 3);

	m_nFaceCount = nNewFaceCount;
	return true;
}

bool A3DLSubMesh::Render(A3DViewport * pCurrentViewport)
{
	m_pA3DDevice->DrawIndexedPrimitiveUP(A3DPT_TRIANGLELIST, 0, m_nVertCount, m_nFaceCount, m_pLIndices, A3DFMT_INDEX16, m_pLVertices, sizeof(A3DLVERTEX));
	return true;
}

bool A3DLSubMesh::TickAnimation(float vDeltaTime)
{
	return true;
}

A3DTLSubMesh::A3DTLSubMesh()
{
	m_pTLVertices	= NULL;
	m_pTLIndices		= NULL;
}

A3DTLSubMesh::~A3DTLSubMesh()
{
}

A3DIBLSubMesh::A3DIBLSubMesh()
{
	m_bManTexture		= false;
	m_szLightMapPath[0] = '\0';

	m_pLightMap			= NULL;
	m_pIBLVertices		= NULL;
	m_pIBLIndices		= NULL;
}

A3DIBLSubMesh::~A3DIBLSubMesh()
{
}

bool A3DIBLSubMesh::Init(A3DDevice * pDevice)
{
	m_pA3DDevice = pDevice;
	
	m_nVertCount = 0;
	m_nFaceCount = 0;
	return true;
}

bool A3DIBLSubMesh::Release()
{
	if( m_pLightMap )
	{
		if( m_bManTexture )
			m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pLightMap);
		else
		{
			m_pLightMap->Release();
			delete m_pLightMap;
		}
		m_pLightMap = NULL;
	}

	if( m_pIBLVertices )
	{
		free(m_pIBLVertices);
		m_pIBLVertices = NULL;
	}

	if( m_pIBLIndices )
	{
		free(m_pIBLIndices);
		m_pIBLIndices = NULL;
	}
	return true;
}

bool A3DIBLSubMesh::SetLightMap(int nLMWidth, int nLMHeight, LPBYTE pLMImage)
{
	if( m_pLightMap )
		return false;

	m_pLightMap = new A3DTexture();

	if( !m_pLightMap->Create(m_pA3DDevice, nLMWidth, nLMHeight, A3DFMT_A8R8G8B8) )
	{
		g_A3DErrLog.Log("A3DIBLSubMesh::AddIBLFace(), Create light map fail!");
		return false;
	}

	LPBYTE pTexData;
	int    nPitch;
	if( !m_pLightMap->LockRect(NULL, (void **)&pTexData, &nPitch, NULL) )
	{
		g_A3DErrLog.Log("A3DIBLSubMesh::AddIBLFace(), Lock light map fail!");
		return false;
	}
	BYTE * pDestPixel = pTexData;
	BYTE * pSrcPixel = pLMImage;
	for(int y=0; y<nLMHeight; y++)
	{
		pDestPixel = pTexData + y * nPitch;
		pSrcPixel  = pLMImage + y * 3 * nLMWidth;
		for(int x=0; x<nLMWidth; x++)
		{
			pDestPixel[0] = pSrcPixel[0];
			pDestPixel[1] = pSrcPixel[1];
			pDestPixel[2] = pSrcPixel[2];
			pDestPixel[3] = 255;
			
			pDestPixel += 4;
			pSrcPixel  += 3;
		}

	}
	m_pLightMap->UnlockRect();

	m_bManTexture = false;
	return true;
}

bool A3DIBLSubMesh::LoadLightMap()
{
	if( m_pLightMap )
		return false;

	if( !m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFile(m_szLightMapPath, &m_pLightMap, A3DTF_LIGHTMAP) )
	{
		g_A3DErrLog.Log("A3DIBLSubMesh::LoadLightMap(), fail to load lightmap texture [%s]!", m_szLightMapPath);
		return false;
	}

	// For this sub mesh will use its lightmaps for lighting pass, so vertex color will not need anymore
	for(int i=0; i<m_nVertCount; i++)
	{
		m_pIBLVertices[i].diffuse = A3DCOLORRGB(255, 255, 255);			
	}
	m_bManTexture = true;
	return true;
}

// First pass we should render the light maps onto the screen
bool A3DIBLSubMesh::LightPass(A3DViewport * pCurrentViewport)
{
	// texture stage flags and render flags has been set by A3DIBLMesh
	if( m_pLightMap )
		m_pLightMap->Appear(0);
	else
		m_pA3DDevice->ClearTexture(0);

	m_pA3DDevice->DrawIndexedPrimitiveUP(A3DPT_TRIANGLELIST, 0, m_nVertCount, m_nFaceCount, m_pIBLIndices, A3DFMT_INDEX16, m_pIBLVertices, sizeof(A3DIBLVERTEX));

	if( m_pLightMap )
		m_pLightMap->Disappear(0);
	return true;
}

// This pass we should render some extra lighting object onto the screen to accumulate the lights
bool A3DIBLSubMesh::ExtraPass(A3DViewport * pCurrentViewport)
{
	// texture stage flags and render flags has been set by A3DIBLMesh
	return true;
}

// This is the render pass that draw base textured mesh onto the screen
// and when use one pass rendering, we call only this pass;
bool A3DIBLSubMesh::BasePass(A3DViewport * pCurrentViewport)
{
	// texture stage flags and render flags has been set by A3DIBLMesh
	m_pA3DDevice->DrawIndexedPrimitiveUP(A3DPT_TRIANGLELIST, 0, m_nVertCount, m_nFaceCount, m_pIBLIndices, A3DFMT_INDEX16, m_pIBLVertices, sizeof(A3DIBLVERTEX));
	return true;
}

bool A3DIBLSubMesh::OnePass(A3DViewport * pCurrentViewport)
{
	// texture stage flags and render flags has been set by A3DIBLMesh
	if( m_pLightMap )
		m_pLightMap->Appear(1);
	else
		m_pA3DDevice->ClearTexture(1);

	m_pA3DDevice->DrawIndexedPrimitiveUP(A3DPT_TRIANGLELIST, 0, m_nVertCount, m_nFaceCount, m_pIBLIndices, A3DFMT_INDEX16, m_pIBLVertices, sizeof(A3DIBLVERTEX));

	if( m_pLightMap )
		m_pLightMap->Disappear(1);
	return true;
}

bool A3DIBLSubMesh::VertexColorPass(A3DViewport * pCurrentViewport)
{
	m_pA3DDevice->DrawIndexedPrimitiveUP(A3DPT_TRIANGLELIST, 0, m_nVertCount, m_nFaceCount, m_pIBLIndices, A3DFMT_INDEX16, m_pIBLVertices, sizeof(A3DIBLVERTEX));

	return true;
}

bool A3DIBLSubMesh::AddVerts(void* pVertices, int nVertCount, A3DVERTEXTYPE vertexType)
{
	return true;
}

bool A3DIBLSubMesh::AddFaces(void* pIndices, int nIndexCount, A3DFORMAT indexFormat)
{
	return true;
}

/*****************************************************************/
/*		                A3DIBLTriSubMesh						 */
/*****************************************************************/
A3DIBLTriSubMesh::A3DIBLTriSubMesh()
{
}

A3DIBLTriSubMesh::~A3DIBLTriSubMesh()
{
}

bool A3DIBLTriSubMesh::AddIBLFace(const A3DIBLVERTEX& v0, const A3DIBLVERTEX& v1, const A3DIBLVERTEX& v2, int n0, int n1, int n2)
{
	// For temp solution, we can only accept one triangle;
	int nVertBegin = m_nVertCount;
	int nIndexBegin = m_nFaceCount * 3;

	m_nVertCount += 3;
	m_nFaceCount ++;
	m_pIBLVertices = (A3DIBLVERTEX *) realloc(m_pIBLVertices, sizeof(A3DIBLVERTEX) * m_nVertCount);
	m_pIBLIndices = (WORD *) realloc(m_pIBLIndices, sizeof(WORD) * m_nFaceCount * 3);

	m_pIBLVertices[nVertBegin] = v0;
	m_pIBLVertices[nVertBegin + 1] = v1;
	m_pIBLVertices[nVertBegin + 2] = v2;
	
	m_pIBLIndices[nIndexBegin] = nVertBegin + n0;
	m_pIBLIndices[nIndexBegin + 1] = nVertBegin + n1;
	m_pIBLIndices[nIndexBegin + 2] = nVertBegin + n2;

	return true;
}

bool A3DIBLTriSubMesh::AddRectFace(const A3DIBLVERTEX& v0, const A3DIBLVERTEX& v1, const A3DIBLVERTEX& v2, const A3DIBLVERTEX& v3, int n0, int n1, int n2, int n3, int n4, int n5)
{
	int nVertBegin = m_nVertCount;
	int nIndexBegin = m_nFaceCount * 3;

	m_nVertCount += 4;
	m_nFaceCount += 2;
	m_pIBLVertices = (A3DIBLVERTEX *) realloc(m_pIBLVertices, sizeof(A3DIBLVERTEX) * m_nVertCount);
	m_pIBLIndices = (WORD *) realloc(m_pIBLIndices, sizeof(WORD) * m_nFaceCount * 3);

	m_pIBLVertices[nVertBegin] = v0;
	m_pIBLVertices[nVertBegin + 1] = v1;
	m_pIBLVertices[nVertBegin + 2] = v2;
	m_pIBLVertices[nVertBegin + 3] = v3;
	
	m_pIBLIndices[nIndexBegin] = nVertBegin + n0;
	m_pIBLIndices[nIndexBegin + 1] = nVertBegin + n1;
	m_pIBLIndices[nIndexBegin + 2] = nVertBegin + n2;

	m_pIBLIndices[nIndexBegin + 3] = nVertBegin + n3;
	m_pIBLIndices[nIndexBegin + 4] = nVertBegin + n4;
	m_pIBLIndices[nIndexBegin + 5] = nVertBegin + n5;
	return true;
}

/*****************************************************************/
/*		                A3DIBLBBSubMesh						 */
/*****************************************************************/
A3DIBLBBSubMesh::A3DIBLBBSubMesh()
{
}

A3DIBLBBSubMesh::~A3DIBLBBSubMesh()
{
}

bool A3DIBLBBSubMesh::AddIBLFace(const A3DIBLVERTEX& v0, const A3DIBLVERTEX& v1, const A3DIBLVERTEX& v2, int n0, int n1, int n2)
{
	// For temp solution, we can only accept one triangle;
	int nVertBegin = m_nVertCount;
	int nIndexBegin = m_nFaceCount * 3;

	m_nVertCount += 3;
	m_nFaceCount ++;
	m_pIBLVertices = (A3DIBLVERTEX *) realloc(m_pIBLVertices, sizeof(A3DIBLVERTEX) * m_nVertCount);
	m_pIBLIndices = (WORD *) realloc(m_pIBLIndices, sizeof(WORD) * m_nFaceCount * 3);

	m_pIBLVertices[nVertBegin] = v0;
	m_pIBLVertices[nVertBegin + 1] = v1;
	m_pIBLVertices[nVertBegin + 2] = v2;
	
	m_pIBLIndices[nIndexBegin] = nVertBegin + n0;
	m_pIBLIndices[nIndexBegin + 1] = nVertBegin + n1;
	m_pIBLIndices[nIndexBegin + 2] = nVertBegin + n2;

	return true;
}

/*****************************************************************/
/*		                A3DIBLPlaneSubMesh						 */
/*****************************************************************/
A3DIBLPlaneSubMesh::A3DIBLPlaneSubMesh()
{
}

A3DIBLPlaneSubMesh::~A3DIBLPlaneSubMesh()
{
}

bool A3DIBLPlaneSubMesh::AddIBLFace(const A3DIBLVERTEX& v0, const A3DIBLVERTEX& v1, const A3DIBLVERTEX& v2, int n0, int n1, int n2)
{
	// For temp solution, we can only accept one triangle;
	int nVertBegin = m_nVertCount;
	int nIndexBegin = m_nFaceCount * 3;

	m_nVertCount += 3;
	m_nFaceCount ++;
	m_pIBLVertices = (A3DIBLVERTEX *) realloc(m_pIBLVertices, sizeof(A3DIBLVERTEX) * m_nVertCount);
	m_pIBLIndices = (WORD *) realloc(m_pIBLIndices, sizeof(WORD) * m_nFaceCount * 3);

	m_pIBLVertices[nVertBegin] = v0;
	m_pIBLVertices[nVertBegin + 1] = v1;
	m_pIBLVertices[nVertBegin + 2] = v2;
	
	m_pIBLIndices[nIndexBegin] = nVertBegin + n0;
	m_pIBLIndices[nIndexBegin + 1] = nVertBegin + n1;
	m_pIBLIndices[nIndexBegin + 2] = nVertBegin + n2;

	return true;
}

/*********************************************************/
/*			A3DIBLMesh section							 */
/*														 */
/*	 Image based mesh for Angelica 3D engine			 */
/*********************************************************/

A3DIBLMesh::A3DIBLMesh()
{
	m_pA3DDevice	= NULL;
	m_pBaseTexture	= NULL;

	m_IBLLightRange = A3DIBLLR_1X;
	m_bUseLightMaps = true;
}

A3DIBLMesh::~A3DIBLMesh()
{
}

bool A3DIBLMesh::Init(A3DDevice * pDevice, char * pszBaseTexture, A3DIBLLIGHTRANGE iblLightRange, bool bUseLightMaps)
{
	m_pA3DDevice = pDevice;
	m_IBLLightRange = iblLightRange;
	m_bUseLightMaps = bUseLightMaps;

	m_ListIBLSubMesh.Init();
	m_ListLSubMesh.Init();

	if( pszBaseTexture )
	{
		if( !m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFile(pszBaseTexture, &m_pBaseTexture) )
		{
			g_A3DErrLog.Log("A3DIBLMesh::Init(), fail to load base texture [%s]!", pszBaseTexture);
			return false;
		}
	}

	return true;
}

bool A3DIBLMesh::Release()
{
	// First release sub-meshes;
	ALISTELEMENT * pSubMeshElement = m_ListIBLSubMesh.GetFirst();
	while( pSubMeshElement != m_ListIBLSubMesh.GetTail() )
	{
		A3DIBLSubMesh * pSubMesh = (A3DIBLSubMesh *) pSubMeshElement->pData;

		pSubMesh->Release();
		delete pSubMesh;

		pSubMeshElement = pSubMeshElement->pNext;
	}

	pSubMeshElement = m_ListLSubMesh.GetFirst();
	while( pSubMeshElement != m_ListLSubMesh.GetTail() )
	{
		A3DLSubMesh * pSubMesh = (A3DLSubMesh *) pSubMeshElement->pData;

		pSubMesh->Release();
		delete pSubMesh;

		pSubMeshElement = pSubMeshElement->pNext;
	}

	m_ListLSubMesh.Release();
	m_ListIBLSubMesh.Release();

	if( m_pBaseTexture )
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pBaseTexture);
	}

	return true;
}

bool A3DIBLMesh::Render(A3DViewport * pCurrentViewport)
{
	A3DMATRIX4	matWorld = a3d_IdentityMatrix();

	m_pA3DDevice->SetLightingEnable(false);
	m_pA3DDevice->SetWorldMatrix(matWorld);
	
	if( m_bUseLightMaps )
	{
		m_pA3DDevice->SetTextureAddress(1, A3DTADDR_CLAMP, A3DTADDR_CLAMP);
		if( (GetKeyState('0') & 0x8000) == 0 )
		{
			// Use one-pass rendering;
			if( !RenderByOnePass(pCurrentViewport) )
				return false;
		}
		else
		{
			// Use multi-pass rendering;
			if( !RenderByMultiPass(pCurrentViewport) )
				return false;
		}
		m_pA3DDevice->SetTextureAddress(1, A3DTADDR_WRAP, A3DTADDR_WRAP);
	}
	else
	{
		if( !RenderByVertexColorOnly(pCurrentViewport) )
			return false;
	}

	m_pA3DDevice->SetLightingEnable(true);
	return true;
}

// show this image based lighting mesh using just one pass rendering;
bool A3DIBLMesh::RenderByOnePass(A3DViewport * pCurrentViewport)
{
	A3DTEXTUREOP textureOP;
	switch(m_IBLLightRange)
	{
	case A3DIBLLR_1X:
		textureOP = A3DTOP_MODULATE;
		break;
	case A3DIBLLR_2X:
		textureOP = A3DTOP_MODULATE2X;
		break;
	case A3DIBLLR_4X:
		textureOP = A3DTOP_MODULATE4X;
		break;
	default:
		textureOP = A3DTOP_MODULATE;
		break;
	}

	if( m_pBaseTexture )
		m_pBaseTexture->Appear(0);
	else
		m_pA3DDevice->ClearTexture(0);

	ALISTELEMENT * pSubMeshElement;

	// first render A3DLSubMesh
	m_pA3DDevice->SetTextureColorArgs(0, A3DTA_TEXTURE, A3DTA_CURRENT);
	m_pA3DDevice->SetTextureAlphaArgs(0, A3DTA_TEXTURE, A3DTA_CURRENT);
	m_pA3DDevice->SetTextureColorOP(0, textureOP);
	m_pA3DDevice->SetTextureAlphaOP(0, A3DTOP_MODULATE);
	m_pA3DDevice->SetFVF(A3DFVF_A3DLVERTEX);

	pSubMeshElement = m_ListLSubMesh.GetFirst();
	while( pSubMeshElement != m_ListLSubMesh.GetTail() )
	{
		A3DLSubMesh * pSubMesh = (A3DLSubMesh *) pSubMeshElement->pData;

		pSubMesh->Render(pCurrentViewport);

		pSubMeshElement = pSubMeshElement->pNext;
	}
	m_pA3DDevice->SetTextureCoordIndex(1, 1);
	m_pA3DDevice->SetTextureFilterType(1, A3DTEXF_LINEAR);
	
	// enable multi-texture blending;
	m_pA3DDevice->SetTextureColorArgs(0, A3DTA_TEXTURE, A3DTA_CURRENT);
	m_pA3DDevice->SetTextureAlphaArgs(0, A3DTA_TEXTURE, A3DTA_CURRENT);
	if( m_pBaseTexture )
	{
		m_pA3DDevice->SetTextureColorOP(0, A3DTOP_SELECTARG1);
		m_pA3DDevice->SetTextureAlphaOP(0, A3DTOP_SELECTARG1);
	}
	else
	{
		// On some card (MGA G450 for example), if we select a texture color but there is no texture,
		// the final result will be pure white immediately
		// We assume that the diffuse color is (1, 1, 1)
		m_pA3DDevice->SetTextureColorOP(0, A3DTOP_SELECTARG2);
		m_pA3DDevice->SetTextureAlphaOP(0, A3DTOP_SELECTARG2);
	}
	
	m_pA3DDevice->SetTextureColorArgs(1, A3DTA_TEXTURE, A3DTA_CURRENT);
	m_pA3DDevice->SetTextureAlphaArgs(1, A3DTA_TEXTURE, A3DTA_CURRENT);
	m_pA3DDevice->SetTextureColorOP(1, textureOP);
	m_pA3DDevice->SetTextureAlphaOP(1, A3DTOP_MODULATE);
	m_pA3DDevice->SetFVF(A3DFVF_A3DIBLVERTEX);
	
	pSubMeshElement = m_ListIBLSubMesh.GetFirst();
	while( pSubMeshElement != m_ListIBLSubMesh.GetTail() )
	{
		A3DIBLSubMesh * pSubMesh = (A3DIBLSubMesh *) pSubMeshElement->pData;

		pSubMesh->OnePass(pCurrentViewport);

		pSubMeshElement = pSubMeshElement->pNext;
	}

	// restore texture stage flags;
	m_pA3DDevice->SetTextureColorArgs(0, A3DTA_TEXTURE, A3DTA_CURRENT);
	m_pA3DDevice->SetTextureAlphaArgs(0, A3DTA_TEXTURE, A3DTA_CURRENT);
	m_pA3DDevice->SetTextureAlphaOP(0, A3DTOP_MODULATE);
	m_pA3DDevice->SetTextureColorOP(0, A3DTOP_MODULATE);

	m_pA3DDevice->SetTextureColorArgs(1, A3DTA_TEXTURE, A3DTA_CURRENT);
	m_pA3DDevice->SetTextureAlphaArgs(1, A3DTA_TEXTURE, A3DTA_CURRENT);
	m_pA3DDevice->SetTextureAlphaOP(1, A3DTOP_DISABLE);
	m_pA3DDevice->SetTextureColorOP(1, A3DTOP_DISABLE);
	
	if( m_pBaseTexture )
		m_pBaseTexture->Disappear(0);

	return true;
}

// show this image based lighting mesh using multi-pass rendering;
bool A3DIBLMesh::RenderByMultiPass(A3DViewport * pCurrentViewport)
{
	m_pA3DDevice->SetFVF(A3DFVF_A3DIBLVERTEX);

	ALISTELEMENT * pSubMeshElement = m_ListIBLSubMesh.GetFirst();
	while( pSubMeshElement != m_ListIBLSubMesh.GetTail() )
	{
		A3DIBLSubMesh * pSubMesh = (A3DIBLSubMesh *) pSubMeshElement->pData;

		// first render the light channel;
		m_pA3DDevice->SetTextureCoordIndex(0, 1);
		m_pA3DDevice->SetTextureColorArgs(0, A3DTA_TEXTURE, A3DTA_CURRENT);
		m_pA3DDevice->SetTextureAlphaArgs(0, A3DTA_TEXTURE, A3DTA_CURRENT);
		if( m_IBLLightRange == A3DIBLLR_4X ) 
			m_pA3DDevice->SetTextureColorOP(0, A3DTOP_MODULATE2X);
		else
			m_pA3DDevice->SetTextureColorOP(0, A3DTOP_SELECTARG1);

		m_pA3DDevice->SetTextureAlphaOP(0, A3DTOP_SELECTARG1);
		m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
		m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
		
		pSubMesh->LightPass(pCurrentViewport);

		m_pA3DDevice->SetTextureCoordIndex(0, 0);
		m_pA3DDevice->SetTextureAddress(0, A3DTADDR_WRAP, A3DTADDR_WRAP);
		
		m_pA3DDevice->SetTextureColorOP(0, A3DTOP_MODULATE);
		m_pA3DDevice->SetTextureAlphaOP(0, A3DTOP_MODULATE);
		
		// Now prepare for extra pass
		// accumulate the lights;
		m_pA3DDevice->SetSourceAlpha(A3DBLEND_ONE);
		m_pA3DDevice->SetDestAlpha(A3DBLEND_ONE);

		pSubMesh->ExtraPass(pCurrentViewport);

		// Now use base pass and make it modulate with the screen
		m_pA3DDevice->SetSourceAlpha(A3DBLEND_DESTCOLOR);
		switch(m_IBLLightRange)
		{
		case A3DIBLLR_1X:
			m_pA3DDevice->SetDestAlpha(A3DBLEND_ZERO);
			break;
		case A3DIBLLR_2X:
		case A3DIBLLR_4X:
			m_pA3DDevice->SetDestAlpha(A3DBLEND_SRCCOLOR);
			break;
		default:
			m_pA3DDevice->SetDestAlpha(A3DBLEND_ZERO);
			break;
		}

		m_pA3DDevice->SetTextureColorArgs(0, A3DTA_TEXTURE, A3DTA_CURRENT);
		m_pA3DDevice->SetTextureAlphaArgs(0, A3DTA_TEXTURE, A3DTA_CURRENT);
		if( m_pBaseTexture )
		{
			m_pA3DDevice->SetTextureColorOP(0, A3DTOP_SELECTARG1);
			m_pA3DDevice->SetTextureAlphaOP(0, A3DTOP_SELECTARG1);
		}
		else
		{
			// On some card (MGA G450 for example), if we select a texture color but there is no texture,
			// the final result will be pure white immediately
			// We assume that the diffuse color is (1, 1, 1)
			m_pA3DDevice->SetTextureColorOP(0, A3DTOP_SELECTARG2);
			m_pA3DDevice->SetTextureAlphaOP(0, A3DTOP_SELECTARG2);
		}

		if( m_pBaseTexture )
			m_pBaseTexture->Appear(0);
		else
			m_pA3DDevice->ClearTexture(0);

		pSubMesh->BasePass(pCurrentViewport);

		if( m_pBaseTexture )
			m_pBaseTexture->Disappear(0);

		// restore alpha blend flags
		m_pA3DDevice->SetSourceAlpha(A3DBLEND_SRCALPHA);
		m_pA3DDevice->SetDestAlpha(A3DBLEND_INVSRCALPHA);
	
		pSubMeshElement = pSubMeshElement->pNext;
	}

	// restore texture stage flags;
	m_pA3DDevice->SetTextureColorArgs(0, A3DTA_TEXTURE, A3DTA_CURRENT);
	m_pA3DDevice->SetTextureColorArgs(1, A3DTA_TEXTURE, A3DTA_CURRENT);
	m_pA3DDevice->SetTextureAlphaOP(0, A3DTOP_MODULATE);
	m_pA3DDevice->SetTextureColorOP(0, A3DTOP_MODULATE);
	m_pA3DDevice->SetTextureAlphaOP(1, A3DTOP_DISABLE);
	m_pA3DDevice->SetTextureColorOP(1, A3DTOP_DISABLE);
	return true;
}				

bool A3DIBLMesh::RenderByVertexColorOnly(A3DViewport * pCurrentViewport)
{
	A3DTEXTUREOP textureOP;
	switch(m_IBLLightRange)
	{
	case A3DIBLLR_1X:
		textureOP = A3DTOP_MODULATE;
		break;
	case A3DIBLLR_2X:
		textureOP = A3DTOP_MODULATE2X;
		break;
	case A3DIBLLR_4X:
		textureOP = A3DTOP_MODULATE4X;
		break;
	default:
		textureOP = A3DTOP_MODULATE;
		break;
	}

	if( m_pBaseTexture )
		m_pBaseTexture->Appear(0);
	else
		m_pA3DDevice->ClearTexture(0);

	ALISTELEMENT * pSubMeshElement;

	// first render A3DLSubMesh
	m_pA3DDevice->SetTextureColorArgs(0, A3DTA_TEXTURE, A3DTA_CURRENT);
	m_pA3DDevice->SetTextureAlphaArgs(0, A3DTA_TEXTURE, A3DTA_CURRENT);
	m_pA3DDevice->SetTextureColorOP(0, textureOP);
	m_pA3DDevice->SetTextureAlphaOP(0, A3DTOP_MODULATE);
	m_pA3DDevice->SetFVF(A3DFVF_A3DLVERTEX);

	pSubMeshElement = m_ListLSubMesh.GetFirst();
	while( pSubMeshElement != m_ListLSubMesh.GetTail() )
	{
		A3DLSubMesh * pSubMesh = (A3DLSubMesh *) pSubMeshElement->pData;

		pSubMesh->Render(pCurrentViewport);

		pSubMeshElement = pSubMeshElement->pNext;
	}
	
	m_pA3DDevice->SetFVF(A3DFVF_A3DIBLVERTEX);
	pSubMeshElement = m_ListIBLSubMesh.GetFirst();
	while( pSubMeshElement != m_ListIBLSubMesh.GetTail() )
	{
		A3DIBLSubMesh * pSubMesh = (A3DIBLSubMesh *) pSubMeshElement->pData;

		pSubMesh->VertexColorPass(pCurrentViewport);

		pSubMeshElement = pSubMeshElement->pNext;
	}

	// restore texture stage flags;
	m_pA3DDevice->SetTextureColorArgs(0, A3DTA_TEXTURE, A3DTA_CURRENT);
	m_pA3DDevice->SetTextureAlphaArgs(0, A3DTA_TEXTURE, A3DTA_CURRENT);
	m_pA3DDevice->SetTextureAlphaOP(0, A3DTOP_MODULATE);
	m_pA3DDevice->SetTextureColorOP(0, A3DTOP_MODULATE);
	
	if( m_pBaseTexture )
		m_pBaseTexture->Disappear(0);

	return true;
}

// Create an empty ibl tri sub mesh for use
bool A3DIBLMesh::AddIBLTriSubMesh(A3DIBLTriSubMesh ** ppNewIBLTriSubMesh)
{
	A3DIBLTriSubMesh * pTriSubMesh;
	pTriSubMesh = new A3DIBLTriSubMesh;
	if( !pTriSubMesh->Init(m_pA3DDevice) )
	{
		g_A3DErrLog.Log("A3DIBLMesh::AddIBLTriSubMesh(), call new sub-mesh's Init() fail!");
		return false;
	}

	// Add into the list;
	m_ListIBLSubMesh.Append(pTriSubMesh, NULL);

	*ppNewIBLTriSubMesh = pTriSubMesh;
	return true;
}

// Create an empty ibl bb sub mesh for use
bool A3DIBLMesh::AddIBLBBSubMesh(A3DIBLBBSubMesh ** ppNewIBLBBSubMesh)
{
	A3DIBLBBSubMesh * pBBSubMesh;
	pBBSubMesh = new A3DIBLBBSubMesh;
	if( !pBBSubMesh->Init(m_pA3DDevice) )
	{
		g_A3DErrLog.Log("A3DIBLMesh::AddIBLBBSubMesh(), call new sub-mesh's Init() fail!");
		return false;
	}

	// Add into the list;
	m_ListIBLSubMesh.Append(pBBSubMesh, NULL);

	*ppNewIBLBBSubMesh = pBBSubMesh;
	return true;
}

// Create an empty ibl bb sub mesh for use
bool A3DIBLMesh::AddIBLPlaneSubMesh(A3DIBLPlaneSubMesh ** ppNewIBLPlaneSubMesh)
{
	A3DIBLPlaneSubMesh * pPlaneSubMesh;
	pPlaneSubMesh = new A3DIBLPlaneSubMesh;
	if( !pPlaneSubMesh->Init(m_pA3DDevice) )
	{
		g_A3DErrLog.Log("A3DIBLMesh::AddIBLPlaneSubMesh(), call new sub-mesh's Init() fail!");
		return false;
	}

	// Add into the list;
	m_ListIBLSubMesh.Append(pPlaneSubMesh, NULL);

	*ppNewIBLPlaneSubMesh = pPlaneSubMesh;
	return true;
}

// Create an empty A3DLSubMesh for use
bool A3DIBLMesh::AddLSubMesh(A3DLSubMesh ** ppNewLSubMesh)
{
	A3DLSubMesh * pLSubMesh;
	pLSubMesh = new A3DLSubMesh;
	if( !pLSubMesh->Init(m_pA3DDevice) )
	{
		g_A3DErrLog.Log("A3DIBLMesh::AddLSubMesh(), call new sub-mesh's Init() fail!");
		return false;
	}

	// Add into the list;
	m_ListLSubMesh.Append(pLSubMesh, NULL);

	*ppNewLSubMesh = pLSubMesh;
	return true;
}

// Load all sub mesh's light map texture
bool A3DIBLMesh::LoadLightMaps()
{
	if( !m_bUseLightMaps )
		return true;

	ALISTELEMENT * pSubMeshElement = m_ListIBLSubMesh.GetFirst();
	while( pSubMeshElement != m_ListIBLSubMesh.GetTail() )
	{
		A3DIBLSubMesh * pSubMesh = (A3DIBLSubMesh *) pSubMeshElement->pData;

		pSubMesh->LoadLightMap();
		
		pSubMeshElement = pSubMeshElement->pNext;
	}

	return true;
}
