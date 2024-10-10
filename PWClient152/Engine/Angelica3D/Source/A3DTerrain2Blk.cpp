   /*
 * FILE: A3DTerrain2Blk.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/6/5
 *
 * HISTORY: 
 * 
 *	2005.2: 使用 pixel shader 1.4 来渲染。每个 pass 可以渲染最多 4 层个地形 layers。
 *			这要求加载 mask 时，将 4 层 mask 合并到 1 个 32-bit 位图中，加上 4 张
 *			layer texture 和 1 张 lightmap. 每个渲染 pass 一共使用 6 张 textures.
 *			使用这种方法渲染，要求每一个 layer mask 的大小一致，这一点在做地形的时候
 *			要多加注意!!
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "A3DTerrain2Blk.h"
#include "A3DPI.h"
#include "A3DTerrain2.h"
#include "A3DTerrain2LOD.h"
#include "A3DTerrain2Env.h"
#include "A3DStream.h"
#include "A3DDevice.h"
#include "A3DMacros.h"
#include "A3DTexture.h"
#include "AFile.h"
#include "AMemory.h"
#include "A3DTerrain2File.h"

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
//	Implement A3DTerrain2Mask
//	
///////////////////////////////////////////////////////////////////////////

A3DTerrain2Mask::A3DTerrain2Mask(A3DTerrain2* pTerrain, int iMaskArea)
{
	m_pTerrain		= pTerrain;
	m_pA3DDevice	= pTerrain->GetA3DDevice();
	m_iMaskArea		= iMaskArea;
	m_pLMTexture	= NULL;
	m_pLMTexture1	= NULL;
	m_pColTexture	= NULL;
	m_pMergedTex	= NULL;
	m_bOwnLM		= false;
	m_bOwnLM1		= false;

	//	Calculate mask area in world space
	int iMaskPitch = pTerrain->GetBlockColNum() * pTerrain->GetBlockGrid() / pTerrain->GetMaskGrid();
	int r = iMaskArea / iMaskPitch;
	int c = iMaskArea % iMaskPitch;
	float fMaskAreaSize = pTerrain->GetMaskGrid() * pTerrain->GetGridSize();
	const ARectF& rcTerrain = pTerrain->GetTerrainArea();
	m_rcArea.left = rcTerrain.left + c * fMaskAreaSize;
	m_rcArea.top = rcTerrain.top - r * fMaskAreaSize;
	m_rcArea.right = m_rcArea.left + fMaskAreaSize;
	m_rcArea.bottom = m_rcArea.top - fMaskAreaSize;
}

A3DTerrain2Mask::~A3DTerrain2Mask()
{
}

//	Initialize object
bool A3DTerrain2Mask::Init(int iNumLayer)
{
	m_aLayers.SetSize(iNumLayer, 10);
	return true;
}

//	Release object
void A3DTerrain2Mask::Release()
{
	//	Release light map texture
	if (m_bOwnLM)
	{
		A3DRELEASE(m_pLMTexture);
	}
	else
	{
		m_pLMTexture = NULL;
	}

	if (m_bOwnLM1)
	{
		A3DRELEASE(m_pLMTexture1);
	}
	else
	{
		m_pLMTexture1 = NULL;
	}

	A3DRELEASE(m_pColTexture);

	int i;

	//	Release textures
	for (i=0; i < m_aLayers.GetSize(); i++)
	{
		const LAYER& Layer = m_aLayers[i];

		//	Normal texture
		if (m_aTextures[i])
			m_pTerrain->ReleaseTexture(Layer.iTextureIdx);

		//	Specular texture
	//	if (m_aSpecularMaps[i])
	//		m_pTerrain->ReleaseTexture(Layer.iSpecMapIdx);
	}

	//	Release mask texture
	for (i=0; i < m_aMaskTextures.GetSize(); i++)
	{
		A3DTexture* pTexture = m_aMaskTextures[i];
		if (pTexture)
		{
			pTexture->Release();
			delete pTexture;
		}
	}

	//	Release merged texture
	if (m_pMergedTex)
	{
		m_pTerrain->GetTextureMerger()->ReleaseMergedTexture(m_pMergedTex);
		m_pMergedTex = NULL;
	}

	m_aTextures.RemoveAll();
	m_aSpecularMaps.RemoveAll();
	m_aMaskTextures.RemoveAll();
	m_aLayers.RemoveAll();
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement A3DTerrain2Block
//	
///////////////////////////////////////////////////////////////////////////

A3DTerrain2Block::A3DTerrain2Block(A3DTerrain2* pTerrain)
{
	m_pTerrain		= pTerrain;
	m_aVertices1	= NULL;
	m_aVertices2	= NULL;
	m_aVertExtras	= NULL;
	m_pA3DDevice	= pTerrain->GetA3DDevice();
	m_fBlockSize	= pTerrain->GetBlockSize();
	m_iBlockGrid	= pTerrain->GetBlockGrid();
	m_iNumVert		= (m_iBlockGrid + 1) * (m_iBlockGrid + 1);
	m_bDataLoaded	= false;
	m_dwBlockFlags	= A3DTRN2LOADERB::T2BKFLAG_DEFAULT;
	m_dwLayerFlags	= 0;
	m_dwLODFlag		= 0;
	m_fDNFactor		= -1.0f;	//	-1 can ensure diffuse color is updated when UpdateVertexDiffuse() is called
	m_fHCWeight		= 0.0f;

	memset(m_aNeighbours, 0, sizeof (m_aNeighbours));
}

A3DTerrain2Block::~A3DTerrain2Block()
{
}

//	Initalize object
bool A3DTerrain2Block::Init()
{
	//	Create vertex buffer
	if (!CreateVertexBuf())
	{
		g_A3DErrLog.Log("A3DTerrain2Block::Init, Failed to create vertex buffer !");
		return false;
	}

	return true;
}

//	Release object
void A3DTerrain2Block::Release()
{
	if (m_aVertices1)
	{
		delete [] m_aVertices1;
		m_aVertices1 = NULL;
	}

	if (m_aVertices2)
	{
		delete [] m_aVertices2;
		m_aVertices2 = NULL;
	}

	if (m_aVertExtras)
	{
		delete [] m_aVertExtras;
		m_aVertExtras = NULL;
	}
}

//	Get flag for this whole block no need for rendering
bool A3DTerrain2Block::IsNoRender() const 
{ 
	return (m_dwBlockFlags & A3DTRN2LOADERB::T2BKFLAG_NORENDER) != 0; 
}

//	Get flag for this whole block no need for tracing
bool A3DTerrain2Block::IsNoTrace() const 
{ 
	return (m_dwBlockFlags & A3DTRN2LOADERB::T2BKFLAG_NOTRACE) != 0; 
}

//	Get flag for this whole block unable for GetPosHeight
bool A3DTerrain2Block::IsNoPosHeight() const 
{ 
	return (m_dwBlockFlags & A3DTRN2LOADERB::T2BKFLAG_NOPOSHEIGHT) != 0; 
}

//	Create vertex buffer
bool A3DTerrain2Block::CreateVertexBuf()
{
	if (m_pTerrain->UseLightmapTech())
	{
		if (!(m_aVertices1 = new A3DTRN2VERTEX1 [m_iNumVert]))
		{
			g_A3DErrLog.Log("A3DTerrain2Block::CreateVertexBuf, Not enough memory !");
			return false;
		}
	}
	else
	{
		if (!(m_aVertices2 = new A3DTRN2VERTEX2 [m_iNumVert]))
		{
			g_A3DErrLog.Log("A3DTerrain2Block::CreateVertexBuf, Not enough memory !");
			return false;
		}

		if (!(m_aVertExtras = new VERTEXEXTRA [m_iNumVert]))
		{
			g_A3DErrLog.Log("A3DTerrain2Block::CreateVertexBuf, Not enough memory !");
			return false;
		}
	}

	return true;
}

//	Fill vertex buffer of stream
//	sx, sz: block's left corner position in world
bool A3DTerrain2Block::FillVertexBuffer(float sx, float sz)
{
	//	Fill block's AABB
	m_aabbBlock.Mins.x	= sx;
	m_aabbBlock.Maxs.z	= sz;
	m_aabbBlock.Maxs.x	= sx + m_fBlockSize;
	m_aabbBlock.Mins.z	= sz - m_fBlockSize;
	m_aabbBlock.Mins.y	= 1e6f;
	m_aabbBlock.Maxs.y	= -1e6f;

	int i, j, iCount = 0, iCount2 = 0;
	int ml = m_iBlockGrid * m_iColInMask;
	int mt = m_iBlockGrid * m_iRowInMask;
	float tl = (float)m_iBlockGrid * m_iColInTrn;
	float tt = (float)m_iBlockGrid * m_iRowInTrn;
	float fMaskStep = 1.0f / m_pTerrain->GetMaskGrid();
	float fMinHei = m_pTerrain->GetSubTerrain(m_iSubTerrain).fMinHei;
	float fGridSize = m_pTerrain->GetGridSize();
	float ou = m_pTerrain->GetTexOffsetU();
	float ov = m_pTerrain->GetTexOffsetV();
	A3DTerrain2::TEMPBUFS& tbuf = m_pTerrain->GetTempBuffers();

	WORD* aIndexMaps = m_pTerrain->GetLODManager()->GetIndexMaps();

	if (m_pTerrain->UseLightmapTech())
	{
		ASSERT(m_aVertices1);

		for (i=0; i <= m_iBlockGrid; i++)
		{
			float z	 = m_aabbBlock.Maxs.z - i * fGridSize;
			float x  = m_aabbBlock.Mins.x;
			float mv = (float)(mt + i) / m_pTerrain->GetMaskGrid();
			float mu = (float)ml / m_pTerrain->GetMaskGrid();

			for (j=0; j <= m_iBlockGrid; j++, iCount++, iCount2+=3)
			{
				//	Note: here we map index
				A3DTRN2VERTEX1* pv = &m_aVertices1[aIndexMaps[iCount]];
				float y = tbuf.aTempHei[iCount];

				if (y < m_aabbBlock.Mins.y)
					m_aabbBlock.Mins.y = y;
				
				if (y > m_aabbBlock.Maxs.y)
					m_aabbBlock.Maxs.y = y;

				pv->vPos[0]	= x;
				pv->vPos[1]	= y;
				pv->vPos[2]	= z;

				pv->vNormal[0] = tbuf.aTempNormal[iCount2+0];
				pv->vNormal[1] = tbuf.aTempNormal[iCount2+1];
				pv->vNormal[2] = tbuf.aTempNormal[iCount2+2];

				float ty = y - fMinHei;

				//	Texture coordinates project on xz
				pv->u1 = x + ou;
				pv->v1 = z + ov;
				//	Texture coordinates project on xy
				pv->u2 = x + ou;
				pv->v2 = ty;
				//	Texture coordinates project on yz
				pv->u3 = z + ov;
				pv->v3 = ty;

				//	Texture coordinate of layer mask
				pv->u4 = mu;
				pv->v4 = mv;

				x  += fGridSize;
				mu += fMaskStep;
			}
		}
	}
	else
	{
		ASSERT(m_aVertices2);

		for (i=0; i <= m_iBlockGrid; i++)
		{
			float z	 = m_aabbBlock.Maxs.z - i * fGridSize;
			float x  = m_aabbBlock.Mins.x;
			float mv = (float)(mt + i) / m_pTerrain->GetMaskGrid();
			float mu = (float)ml / m_pTerrain->GetMaskGrid();

			for (j=0; j <= m_iBlockGrid; j++, iCount++, iCount2+=3)
			{
				//	Note: here we map index
				A3DTRN2VERTEX2* pv = &m_aVertices2[aIndexMaps[iCount]];
				VERTEXEXTRA* pe = &m_aVertExtras[aIndexMaps[iCount]];
				float y = tbuf.aTempHei[iCount];

				if (y < m_aabbBlock.Mins.y)
					m_aabbBlock.Mins.y = y;
				
				if (y > m_aabbBlock.Maxs.y)
					m_aabbBlock.Maxs.y = y;

				pe->dwDiffuse	= tbuf.aTempDiff[iCount] | 0xff000000;
				pe->dwDiffuse1	= tbuf.aTempDiff1[iCount] | 0xff000000;
				pe->vNormal.x	= tbuf.aTempNormal[iCount2+0];
				pe->vNormal.y	= tbuf.aTempNormal[iCount2+1];
				pe->vNormal.z	= tbuf.aTempNormal[iCount2+2];

				pv->vPos[0]		= x;
				pv->vPos[1]		= y;
				pv->vPos[2]		= z;
				pv->dwDiffuse	= pe->dwDiffuse;
				pv->dwSpecular	= 0xff000000;

				float ty = y - fMinHei;

				//	Texture coordinates project on xz
				pv->u1 = x;
				pv->v1 = z;
				//	Texture coordinates project on xy
				pv->u2 = x;
				pv->v2 = ty;
				//	Texture coordinates project on yz
				pv->u3 = z;
				pv->v3 = ty;

				//	Texture coordinate of layer mask
				pv->u4 = mu;
				pv->v4 = mv;

				x  += fGridSize;
				mu += fMaskStep;
			}
		}
		
		//	-1 ensure diffuse color is updated when next time
		//	UpdateVertexDiffuse() is called.
		m_fDNFactor = -1.0f;
	}

	return true;
}

//	Fill vertex color data
bool A3DTerrain2Block::FillVertexColor(ARectI* prcArea/* NULL */)
{
	if (m_pTerrain->UseLightmapTech())
		return false;

	ASSERT(m_aVertices2);

	//	Set fill area
	ARectI rc(0, 0, m_iBlockGrid+1, m_iBlockGrid+1);
	if (prcArea)
	{
		rc = *prcArea;
		ASSERT(rc.left >= 0 && rc.left <= m_iBlockGrid);
		ASSERT(rc.right >= 0 && rc.right <= m_iBlockGrid+1);
		ASSERT(rc.top >= 0 && rc.top <= m_iBlockGrid);
		ASSERT(rc.bottom >= 0 && rc.bottom <= m_iBlockGrid+1);
	}

	WORD* aIndexMaps = m_pTerrain->GetLODManager()->GetIndexMaps();
	A3DTerrain2::TEMPBUFS& tbuf = m_pTerrain->GetTempBuffers();

	int i, j;
	int iPitch = m_iBlockGrid + 1;
	int iStartIdx = rc.top * iPitch + rc.left;

	for (i=rc.top; i < rc.bottom; i++)
	{
		int iCount = iStartIdx;
		iStartIdx += iPitch;

		for (j=rc.left; j < rc.right; j++, iCount++)
		{
			//	Note: here we map index
			A3DTRN2VERTEX2* pv = &m_aVertices2[aIndexMaps[iCount]];
			VERTEXEXTRA* pe = &m_aVertExtras[aIndexMaps[iCount]];

			pe->dwDiffuse	= tbuf.aTempDiff[iCount] | 0xff000000;
			pe->dwDiffuse1	= tbuf.aTempDiff1[iCount] | 0xff000000;
			pv->dwDiffuse	= pe->dwDiffuse;
			pv->dwSpecular	= 0xff000000;
		}
	}

	//	-1 ensure diffuse color is updated when next time
	//	UpdateVertexDiffuse() is called.
	m_fDNFactor = -1.0f;
	
	return true;
}

//	Adjust block's LOD level. This funcion ensure the LOD level difference between
//	block and it's neighbours <= 1. Note: This adjustment can only work properly
//	for maximum 3-level terrain
void A3DTerrain2Block::AdjustLODLevel()
{
	for (int i=0; i < 4; i++)
	{
		A3DTerrain2Block* pNeighbour = m_aNeighbours[i];
		if (pNeighbour && pNeighbour->GetLODLevel() - m_iLODLevel >= 2)
		{
			m_iLODLevel++;
			break;
		}
	}
}

//	Fill index buffer of stream
bool A3DTerrain2Block::BuildRenderData()
{
	ASSERT(m_bDataLoaded);

	//	Update vertex diffuse color
	if (!m_pTerrain->UseLightmapTech())
		UpdateVertexDiffuse();

	A3DTerrain2LOD* pLOD = m_pTerrain->GetLODManager();

	DWORD dwLODFlag = 0;

	if (m_pTerrain->GetLODType() == A3DTerrain2::LOD_NONE)
	{
		dwLODFlag = m_iLODLevel << 16;

		m_RenderData.aIndices[0] = pLOD->GetIndexStream(m_iLODLevel, A3DTerrain2LOD::D_LEFT, 0, &m_RenderData.aIdxNum[0], true);
		m_RenderData.aIndices[1] = pLOD->GetIndexStream(m_iLODLevel, A3DTerrain2LOD::D_TOP, 0, &m_RenderData.aIdxNum[1], true);
		m_RenderData.aIndices[2] = pLOD->GetIndexStream(m_iLODLevel, A3DTerrain2LOD::D_RIGHT, 0, &m_RenderData.aIdxNum[2], true);
		m_RenderData.aIndices[3] = pLOD->GetIndexStream(m_iLODLevel, A3DTerrain2LOD::D_BOTTOM, 0, &m_RenderData.aIdxNum[3], true);
		m_RenderData.aIndices[4] = pLOD->GetIndexStream(m_iLODLevel, A3DTerrain2LOD::D_CENTER, 0, &m_RenderData.aIdxNum[4], true);
	}
	else
	{
		int aDescends[4];

		if (m_iLODLevel > 1)
		{
			for (int i=0; i < 4; i++)
			{
				aDescends[i] = 0;
				A3DTerrain2Block* pNeighbour = m_aNeighbours[i];

				if (pNeighbour && pNeighbour->GetLODLevel() &&
					pNeighbour->GetLODLevel() < m_iLODLevel)
				{
					aDescends[i] = m_iLODLevel - pNeighbour->GetLODLevel();
					dwLODFlag |= (aDescends[i] << (i * 2));
				}
			}
		}
		else
		{
			aDescends[0] = 0;
			aDescends[1] = 0;
			aDescends[2] = 0;
			aDescends[3] = 0;
		}

		dwLODFlag |= (m_iLODLevel << 16);

		m_RenderData.aIndices[0] = pLOD->GetIndexStream(m_iLODLevel, A3DTerrain2LOD::D_LEFT, aDescends[0], &m_RenderData.aIdxNum[0], true);
		m_RenderData.aIndices[1] = pLOD->GetIndexStream(m_iLODLevel, A3DTerrain2LOD::D_TOP, aDescends[1], &m_RenderData.aIdxNum[1], true);
		m_RenderData.aIndices[2] = pLOD->GetIndexStream(m_iLODLevel, A3DTerrain2LOD::D_RIGHT, aDescends[2], &m_RenderData.aIdxNum[2], true);
		m_RenderData.aIndices[3] = pLOD->GetIndexStream(m_iLODLevel, A3DTerrain2LOD::D_BOTTOM, aDescends[3], &m_RenderData.aIdxNum[3], true);
		m_RenderData.aIndices[4] = pLOD->GetIndexStream(m_iLODLevel, A3DTerrain2LOD::D_CENTER, 0, &m_RenderData.aIdxNum[4], true);
	}

	m_RenderData.iNumIndex = m_RenderData.aIdxNum[0] + m_RenderData.aIdxNum[1] +
							 m_RenderData.aIdxNum[2] + m_RenderData.aIdxNum[3] +
							 m_RenderData.aIdxNum[4];

	m_dwLODFlag = dwLODFlag;

	//	Has descend border ?
	if (m_dwLODFlag & 0x0000ffff)
		m_RenderData.iNumVert = m_iNumVert;
	else
		m_RenderData.iNumVert = m_pTerrain->GetLODManager()->GetLevelVertexNum(m_iLODLevel);

	if (m_pTerrain->UseLightmapTech())
		m_RenderData.aVertices = m_aVertices1;
	else
		m_RenderData.aVertices = m_aVertices2;

	return true;
}

/*	Copy block vertices to destination buffer. This function works like BitBlt.
	Copy vertex positions in a rectangle grid area to destination buffer, every
	vertex is like a pixel in BitBlt;

	pDestBuf: destination vertex buffer
	iWid, iHei: vertex rectangle area's size
	iDestPitch: destination vertex buffer pitch in A3DVECTOR3
	sx, sy: grid rectangle area's left-top corner in block
*/
bool A3DTerrain2Block::CopyVertexPos(A3DVECTOR3* pDestBuf, int iWid, int iHei, 
									 int iDestPitch, int sx, int sy)
{
	WORD* aIndexMaps = m_pTerrain->GetLODManager()->GetIndexMaps();
	A3DVECTOR3* pDestLine = pDestBuf;
	int iSrcLine = sy * (m_iBlockGrid+1) + sx;

	if (m_pTerrain->UseLightmapTech())
	{
		ASSERT(m_aVertices1);

		for (int i=0; i < iHei; i++)
		{
			A3DVECTOR3* pDest = pDestLine;
			int iSrc = iSrcLine;
			pDestLine += iDestPitch;
			iSrcLine += m_iBlockGrid + 1;

			for (int j=0; j < iWid; j++, pDest++, iSrc++)
			{
				//	Note: here we map index
				A3DTRN2VERTEX1* pSrc = &m_aVertices1[aIndexMaps[iSrc]];

				pDest->x = pSrc->vPos[0];
				pDest->y = pSrc->vPos[1];
				pDest->z = pSrc->vPos[2];
			}
		}
	}
	else
	{
		ASSERT(m_aVertices2);

		for (int i=0; i < iHei; i++)
		{
			A3DVECTOR3* pDest = pDestLine;
			int iSrc = iSrcLine;
			pDestLine += iDestPitch;
			iSrcLine += m_iBlockGrid + 1;

			for (int j=0; j < iWid; j++, pDest++, iSrc++)
			{
				//	Note: here we map index
				A3DTRN2VERTEX2* pSrc = &m_aVertices2[aIndexMaps[iSrc]];

				pDest->x = pSrc->vPos[0];
				pDest->y = pSrc->vPos[1];
				pDest->z = pSrc->vPos[2];
			}
		}
	}

	return true;
}

/*	Copy grid faces to destination buffer. This function works like BitBlt.
	Copy face in a rectangle grid area to destination buffer,  every face is
	like a pixel in BitBlt;

	pDestVert: destination vertex buffer
	pDestIdx: destination index buffer
	iWid, iHei: grid rectangle area's size
	iDstVertPitch: destination vertex buffer pitch in A3DVECTOR3
	iBaseVert: Index of left-top corner vertex in vertex buffer
	sx, sy: grid rectangle area's left-top corner in block
*/
bool A3DTerrain2Block::CopyFaces(A3DVECTOR3* pDestVert, WORD* pDestIdx, int iWid, int iHei,
								 int iDstVertPitch, int iBaseVert, int sx, int sy)
{
	//	Copy vertices at first. Notice the difference of iWid and iHei in this
	//	function and in CopyVertexPos()
	CopyVertexPos(pDestVert, iWid+1, iHei+1, iDstVertPitch, sx, sy);

	//	Build indices
	WORD* pIndices = pDestIdx;

	for (int i=0; i < iHei; i++)
	{
		WORD v0 = (WORD)iBaseVert;
		iBaseVert += iDstVertPitch;

		for (int j=0; j < iWid; j++, pIndices+=6, v0++)
		{
			//	v3  v0----v1
			//	| \  \    |	
			//	|  \  \   |	
			//	|   \  \  |	
			//	|	 \  \ |	
			//	v5---v4  v2
			pIndices[0] = v0;
			pIndices[1] = v0 + 1;
			pIndices[2] = v0 + iDstVertPitch + 1;

			pIndices[3] = v0;
			pIndices[4] = v0 + iDstVertPitch + 1;
			pIndices[5] = v0 + iDstVertPitch;
		}
	}
	
	//	If this area contain block's right-top corner grid or left-bottom 
	//	corner grid, then we have to adjust index so that they are in
	//	rendering order
	if (!sx && sy + iHei == m_iBlockGrid)
	{
		//	Contain left-bottom corner grid
		//	v0----v1 
		//	|    / |
		//	|   /  |
		//	|  /   |
		//	| /    |
		//	v2----v3
		pIndices = pDestIdx + (iHei - 1) * iWid * 6;
		WORD v0 = pIndices[0];
		WORD v1 = pIndices[1];
		WORD v2 = pIndices[5];
		WORD v3 = pIndices[2];

		pIndices[0] = v0;
		pIndices[1] = v1;
		pIndices[2] = v2;

		pIndices[3] = v1;
		pIndices[4] = v3;
		pIndices[5] = v2;
	}

	if (sx + iWid == m_iBlockGrid && !sy)
	{
		//	Contain right-top corner grid
		//	v0----v1 
		//	|    / |
		//	|   /  |
		//	|  /   |
		//	| /    |
		//	v2----v3
		pIndices = pDestIdx + (iWid - 1) * 6;
		WORD v0 = pIndices[0];
		WORD v1 = pIndices[1];
		WORD v2 = pIndices[5];
		WORD v3 = pIndices[2];

		pIndices[0] = v0;
		pIndices[1] = v1;
		pIndices[2] = v2;

		pIndices[3] = v1;
		pIndices[4] = v3;
		pIndices[5] = v2;
	}

	return true;
}

//	Update diffuse color of block vertices
void A3DTerrain2Block::UpdateVertexDiffuse()
{
	if (!m_aVertices2)
	{
		ASSERT(m_aVertices2);
		return;
	}

	float fDNFactor = m_pTerrain->GetDNFactor();
	float fDelta = (float)fabs(fDNFactor - m_fDNFactor);

	if (fDelta <= 1.0f / 256.0f)
		return;		//	Change is too small, ignore it

	m_fDNFactor = fDNFactor;

	if (m_pTerrain->GetLightEnableFlag())
	{
		//	Re-calculate vertex color
		int i, d, n;
		n = (int)(m_fDNFactor * 255.0f);
		d = 255 - n;

		for (i=0; i < m_iNumVert; i++)
		{
			A3DCOLOR c0, c1, c2, c3;
			c0 = m_aVertExtras[i].dwDiffuse & 0x00ff00ff;
			c1 = m_aVertExtras[i].dwDiffuse & 0x0000ff00;
			c2 = m_aVertExtras[i].dwDiffuse1 & 0x00ff00ff;
			c3 = m_aVertExtras[i].dwDiffuse1 & 0x0000ff00;

			c0 = ((c0 * d + c2 * n) >> 8) & 0x00ff00ff;
			c1 = ((c1 * d + c3 * n) >> 8) & 0x0000ff00;
			m_aVertices2[i].dwDiffuse = 0xff000000 | c0 | c1;
		}
	}
	else
	{
		for (int i=0; i < m_iNumVert; i++)
			m_aVertices2[i].dwDiffuse = 0xffffffff;
	}
}

