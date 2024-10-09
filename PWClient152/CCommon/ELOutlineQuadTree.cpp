/*
 * FILE: ELOutlineQuadTree.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Wangkuiwu, 2005/7/20
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#include <AFileImage.h>
#include <A3DVector.h>
#include <A3DStream.h>
#include <A3DCameraBase.h>
#include <A3DTerrain2.h>
#include <A3DTerrain2Blk.h>
#include <A3DDevice.h>


#include "ELOutlineQuadTree.h"
#include "ELOutlineTexture.h"

//#define EL_OLQ_DEBUG
#ifdef  EL_OLQ_DEBUG
char    msg[200];
#define OUTPUT_DEBUG_INFO(s)    OutputDebugStringA((s))
#endif

CELOutlineQuadTree::CELOutlineQuadTree()
{
	m_pELOTexture = NULL;
	m_pA3DDevice = NULL;
	m_RectWorld.Clear();
	m_Rect.Clear();
	m_RectBlk.Clear();
	m_iSubIndex = 0;
	
	m_pVertPos   = NULL;
	m_nVert = 0;
	m_nFace = 0;
	m_nMaxFace = 0;
	m_pStream = NULL;
	m_pSeamStream = NULL;

	m_pQNodes = NULL;
	m_nQNodes = 0;
	m_bStreamDirty = true;

	m_nSeamFace = 0;
	m_nSeamVert = 0;

	m_bLoadOk = false;
}

CELOutlineQuadTree::~CELOutlineQuadTree()
{
	Release();
}

void CELOutlineQuadTree::Release()
{
	if (m_pVertPos) {
		delete[] m_pVertPos;
		m_pVertPos = NULL;
	}
	if (m_pQNodes) {
		delete[] m_pQNodes;
		m_pQNodes = NULL;
	}
	if (m_pStream) {
		m_pStream->Release();
		delete m_pStream;
		m_pStream = NULL;
	}
	if (m_pSeamStream) {
		m_pSeamStream->Release();
		delete m_pSeamStream;
		m_pSeamStream = NULL;
	}
}

bool CELOutlineQuadTree::Init(A3DDevice * pDevice , int iSubIndex, float wx, float wz)
{
	m_iSubIndex = iSubIndex;
	m_pA3DDevice = pDevice;
	float fw = OUTLINE_TILE_WIDTH;
	float fh = OUTLINE_TILE_HEIGHT;
	int iw = (iSubIndex-1)%OUTLINE_TEX_LEVEL[0];
	int ih = (iSubIndex-1)/OUTLINE_TEX_LEVEL[0];  //level 0

	m_RectWorld.left = wx + iw*fw;
	m_RectWorld.top  = wz - ih*fh;
	m_RectWorld.right = m_RectWorld.left + fw;
	m_RectWorld.bottom = m_RectWorld.top - fh;
	
	m_Rect.left = iw * (OUTLINE_TILE_WIDTH/OUTLINE_TILE_GRID_LEN);
	m_Rect.top  = ih * (OUTLINE_TILE_HEIGHT/OUTLINE_TILE_GRID_LEN);
	m_Rect.right = m_Rect.left + (OUTLINE_TILE_WIDTH/OUTLINE_TILE_GRID_LEN);
	m_Rect.bottom = m_Rect.top + (OUTLINE_TILE_HEIGHT/OUTLINE_TILE_GRID_LEN);
	
	m_RectBlk.left = iw * (OUTLINE_TILE_WIDTH/ (OUTLINE_TILE_GRID_LEN * OUTLINE_BLK_LEN));
	m_RectBlk.top  = ih *(OUTLINE_TILE_HEIGHT/ (OUTLINE_TILE_GRID_LEN * OUTLINE_BLK_LEN));
	m_RectBlk.right = m_RectBlk.left + (OUTLINE_TILE_WIDTH/ (OUTLINE_TILE_GRID_LEN * OUTLINE_BLK_LEN));
	m_RectBlk.bottom = m_RectBlk.top + (OUTLINE_TILE_HEIGHT/ (OUTLINE_TILE_GRID_LEN * OUTLINE_BLK_LEN));

	m_vBBMins = A3DVECTOR3(m_RectWorld.left, -9000, m_RectWorld.bottom);
	m_vBBMaxs = A3DVECTOR3(m_RectWorld.right, +9000, m_RectWorld.top);


	return true;
}

void CELOutlineQuadTree::SetOwnerTex(CELOutlineTexture * pELOTex)
{
	m_pELOTexture = pELOTex;
	m_bStreamDirty = true;
	if ((!m_bLoadOk) || (m_pStream == NULL)) {
		return;
	}
	A3DLVERTEX * pVerts;
	if( !m_pStream->LockVertexBuffer(0, 0, (BYTE **)&pVerts, 0) )
		return ;
	float txleft = m_pELOTexture->m_RectWorld.left;
	float tztop = m_pELOTexture->m_RectWorld.top;
	float tscale = m_pELOTexture->m_vTexScale;
	int i;
	for (i =0; i < m_nVert; i++) {
		pVerts[i].tu =(m_pVertPos[i].x - txleft)*tscale;
		pVerts[i].tv =(tztop - m_pVertPos[i].z)*tscale;
	}
	m_pStream->UnlockVertexBuffer();


}

bool CELOutlineQuadTree::Load(AString& szDir)
{
	Release();

	m_bLoadOk = false;
	char path[MAX_PATH];
	sprintf(path, "%s%d.olm", szDir, m_iSubIndex);
	AFileImage fileImage;
	if( !fileImage.Open(path, AFILE_OPENEXIST) )
		return false;
	DWORD dwReadLen;

	DWORD dwMagic;
	if (!fileImage.Read(&dwMagic, sizeof(DWORD), &dwReadLen)) {
		return false;
	}
	assert(dwMagic == OUTLINE_TILE_MAGIC);
	DWORD dwVersion;
	if (!fileImage.Read(&dwVersion, sizeof(DWORD), &dwReadLen)) {
		return false;
	}
	assert(dwVersion == OUTLINE_TILE_VERSION);
	
	if( !fileImage.Read(&m_nVert, sizeof(int), &dwReadLen) )
		return false;
	
	m_pVertPos = new A3DVECTOR3[m_nVert];
	if (!fileImage.Read(m_pVertPos, sizeof(A3DVECTOR3)*m_nVert, &dwReadLen)) {
		return false;
	}

	if (!fileImage.Read(&m_nMaxFace, sizeof(int), &dwReadLen)) {
		return false;
	}

	if (!fileImage.Read(&m_nQNodes, sizeof(int), &dwReadLen)) {
		return false;
	}

	m_pQNodes = new CELOutlineQuadNode[m_nQNodes];
	int i;
	
	struct export_head{
		WORD ix, iz;
		WORD iLen;
		WORD iActiveFlag;
		WORD iVertCenter;
		int iParent;
		int iChildren[4];
	} ;
	
	//export_head  _head;
	for (i = 0; i < m_nQNodes; i++) {
		if (!fileImage.Read(&m_pQNodes[i], sizeof(export_head), &dwReadLen)) {
		//if (!fileImage.Read(&_head, sizeof(export_head), &dwReadLen)) {
			assert(0);
			return false;
		}
//		m_pQNodes[i].ix = _head.ix;
//		m_pQNodes[i].iz = _head.iz;
//		m_pQNodes[i].iLen = _head.iLen;
//		m_pQNodes[i].iActiveFlag = _head.iActiveFlag;

		m_pQNodes[i].bRender = m_pQNodes[i].IsActive();
		if ((m_pQNodes[i].IsActive()) || (m_pQNodes[i].iLen == OUTLINE_BLK_LEN)) {
		//if (m_pQNodes[i].IsActive())  {
			for (int j = 0; j < 4; j++) {
				if (!fileImage.Read(&m_pQNodes[i].nEdgeVert[j], sizeof(int), &dwReadLen)) {
					return false;
				}		
				assert(m_pQNodes[i].nEdgeVert[j] > 0);
				m_pQNodes[i].pEdgeVert[j] = new WORD[m_pQNodes[i].nEdgeVert[j]];
				if (!fileImage.Read(m_pQNodes[i].pEdgeVert[j], sizeof(WORD)*m_pQNodes[i].nEdgeVert[j], &dwReadLen)) {
					return false;
				}		
			}
		}
	}
	fileImage.Close();

	//create the stream
	m_pStream  = new A3DStream;
	if (!m_pStream->Init(m_pA3DDevice, A3DVT_LVERTEX, m_nVert, m_nMaxFace*3,
		A3DSTRM_STATIC, A3DSTRM_STATIC)) {
		return false;
	}
	A3DLVERTEX * pVerts;
	if( !m_pStream->LockVertexBuffer(0, 0, (BYTE **)&pVerts, 0) )
		return false;
	float txleft = m_pELOTexture->m_RectWorld.left;
	float tztop = m_pELOTexture->m_RectWorld.top;
	float tscale = m_pELOTexture->m_vTexScale;

	for (i =0; i < m_nVert; i++) {
		pVerts[i] = A3DLVERTEX(m_pVertPos[i], OUTLINE_DIFFUSE_VIEW, OUTLINE_SPECULAR,
			(m_pVertPos[i].x - txleft)*tscale, (tztop - m_pVertPos[i].z)*tscale);
	}
	m_pStream->UnlockVertexBuffer();

	m_bStreamDirty = true;

	//create the seam stream
	m_pSeamStream  = new A3DStream;
	if (!m_pSeamStream->Init(m_pA3DDevice, A3DVT_LVERTEX, m_nVert*2, m_nMaxFace*3,
		A3DSTRM_STATIC, A3DSTRM_STATIC)) {
		return false;
	}
	
	//////////////////////////////////////////////////////////////////////////
	//debug
	//BuildStream();
	//////////////////////////////////////////////////////////////////////////
	
	m_bLoadOk = true;
	return true;
}

void CELOutlineQuadTree::_BuildStream()
{
	if (!m_bStreamDirty) {
		return;
	}
	WORD * pIndices;
	if( !m_pStream->LockIndexBuffer(0, 0, (BYTE **)&pIndices, 0) )
		return;
	m_nFace = 0;
	int i;
	for (i = 0; i < m_nQNodes; i++) {
		if (!m_pQNodes[i].bRender) {
			continue;
		}
		int v0, v1, v2;
		int j;
		v0 = m_pQNodes[i].iCenterVert;
		//top edge
		assert(m_pQNodes[i].nEdgeVert[OLQ_EDGE_TOP] > 0);
		v1 = m_pQNodes[i].pEdgeVert[OLQ_EDGE_TOP][0];
		for (j = 1; j < m_pQNodes[i].nEdgeVert[OLQ_EDGE_TOP]; j++) {
			v2 = m_pQNodes[i].pEdgeVert[OLQ_EDGE_TOP][j];
			pIndices[m_nFace*3] = v0;
			pIndices[m_nFace*3+1] = v1;
			pIndices[m_nFace*3+2] = v2;
			m_nFace++;
			v1 = v2;
		}
		//right edge
		assert(m_pQNodes[i].nEdgeVert[OLQ_EDGE_RIGHT] > 0);
		v1 = m_pQNodes[i].pEdgeVert[OLQ_EDGE_RIGHT][0];
		for (j = 1; j < m_pQNodes[i].nEdgeVert[OLQ_EDGE_RIGHT]; j++) {
			v2 = m_pQNodes[i].pEdgeVert[OLQ_EDGE_RIGHT][j];
			pIndices[m_nFace*3] = v0;
			pIndices[m_nFace*3+1] = v1;
			pIndices[m_nFace*3+2] = v2;
			m_nFace++;
			v1 = v2;
		}
		//bottom edge
		assert(m_pQNodes[i].nEdgeVert[OLQ_EDGE_BOTTOM] > 0);
		v1 = m_pQNodes[i].pEdgeVert[OLQ_EDGE_BOTTOM][0];
		for (j = 1; j < m_pQNodes[i].nEdgeVert[OLQ_EDGE_BOTTOM]; j++) {
			v2 = m_pQNodes[i].pEdgeVert[OLQ_EDGE_BOTTOM][j];
			pIndices[m_nFace*3] = v0;
			pIndices[m_nFace*3+1] = v2;
			pIndices[m_nFace*3+2] = v1;
			m_nFace++;
			v1 = v2;
		}
		//left edge
		assert(m_pQNodes[i].nEdgeVert[OLQ_EDGE_LEFT] > 0);
		v1 = m_pQNodes[i].pEdgeVert[OLQ_EDGE_LEFT][0];
		for (j = 1; j < m_pQNodes[i].nEdgeVert[OLQ_EDGE_LEFT]; j++) {
			v2 = m_pQNodes[i].pEdgeVert[OLQ_EDGE_LEFT][j];
			pIndices[m_nFace*3] = v0;
			pIndices[m_nFace*3+1] = v2;
			pIndices[m_nFace*3+2] = v1;
			m_nFace++;
			v1 = v2;
		}
	}
	m_pStream->UnlockIndexBuffer();

	m_bStreamDirty = false;
}




void CELOutlineQuadTree::CreateView(DWORD dwFrame, const ARectI& rcSeam, const ARectI& rcView)
{
	if ((!m_bLoadOk) ||(m_pELOTexture->m_dwFrameStamp != dwFrame) 
		|| (!CELOutlineTextureCache::RcOverlapRc(m_Rect, rcView))){
		m_nFace = 0;
		return;
	}

	ARectI rcNode;
	for (int i =0; i < m_nQNodes; i++ ) {
		if (!m_pQNodes[i].IsActive()) {
			continue;
		}
		rcNode.left = m_pQNodes[i].ix;
		rcNode.top = m_pQNodes[i].iz;
		rcNode.right = rcNode.left + m_pQNodes[i].iLen;
		rcNode.bottom = rcNode.top + m_pQNodes[i].iLen;
		bool bRender = CELOutlineTextureCache::RcOverlapRc(rcNode, rcView);
		if (m_pQNodes[i].bRender != bRender) {
			m_pQNodes[i].bRender = bRender;
			// m_bStreamDirty = true;
		}
	}

	if (CELOutlineTextureCache::RcOverlapRc(rcSeam, m_Rect)) {
		//m_bStreamDirty = true;
		ARectI rc;
		//top edge
		if (rcSeam.top >= m_Rect.top) {
			rc.left = max(m_Rect.left, rcSeam.left+OUTLINE_BLK_LEN);
			rc.top = rcSeam.top;
			rc.right = min(m_Rect.right, rcSeam.right-OUTLINE_BLK_LEN);
			rc.bottom = rc.top + OUTLINE_BLK_LEN;
			rc.Inflate(1, 1);
			_SetQNodeRenderFlag(rc, false);
		}
		//middle
		rc.left = rcSeam.left;
		rc.top = rcSeam.top + OUTLINE_BLK_LEN;
		rc.right = rcSeam.right;
		rc.bottom = rcSeam.bottom- OUTLINE_BLK_LEN;
		rc.Inflate(1,1);
		_SetQNodeRenderFlag(rc, false);
		//bottom edge
		if (rcSeam.bottom <= m_Rect.bottom) {
			ARectI rc;
			rc.left = max(m_Rect.left, rcSeam.left+OUTLINE_BLK_LEN);
			rc.bottom = rcSeam.bottom;
			rc.top = rc.bottom - OUTLINE_BLK_LEN;
			rc.right = min(m_Rect.right, rcSeam.right-OUTLINE_BLK_LEN);
			rc.Inflate(1, 1);
			_SetQNodeRenderFlag(rc, false);
		}

	}
	m_bStreamDirty = true;
	_BuildStream();
}


void CELOutlineQuadTree::_SetQNodeRenderFlag(ARectI& rc, bool bRender)
{
	for (int i = 0; i < m_nQNodes; i++) {
		if (rc.PtInRect(m_pQNodes[i].ix, m_pQNodes[i].iz) 
			&& rc.PtInRect(m_pQNodes[i].ix+m_pQNodes[i].iLen, m_pQNodes[i].iz + m_pQNodes[i].iLen)) {
			if (m_pQNodes[i].bRender != bRender){
				m_pQNodes[i].bRender = bRender;
			// 	m_bStreamDirty = true;
			}
			
		}
	}
}

/**
 * \brief stitch the terrain blk and complete the unloaded terrain blk with olm.
 * \param[in] rcVisible the visible rect area in blk(i.e. 64meters)
 * \param[out]
 * \return
 * \note
 * \warning
 * \todo   
 * \author kuiwu 
 * \date 14/2/2006
 * \see 
 */
void CELOutlineQuadTree::StitchAndCompleteTerrain(A3DTerrain2::ACTBLOCKS *pActBlocks, const ARectI& rcVisible,  A3DTerrain2LOD::GRID * pTerGrids)
{
	
	m_nSeamFace = 0;
	m_nSeamVert = 0;


	ARectI rcSeamBlk(rcVisible);
	rcSeamBlk.Inflate(1,1);

	if (!m_bLoadOk || !CELOutlineTextureCache::RcOverlapRc(rcSeamBlk, m_RectBlk)) {
		return;
	}
	A3DLVERTEX * pVerts;
	if (!m_pSeamStream->LockVertexBuffer(0, 0, (BYTE**)&pVerts, 0)) {
		return ;
	}
	WORD * pIndices;
	if (!m_pSeamStream->LockIndexBuffer(0,0, (BYTE**)&pIndices, 0)) {
		return ;
	}

	_CompleteTerrain(pActBlocks, rcVisible, pTerGrids, pVerts,pIndices);
	//_Sticth(pActBlocks, rcVisible, pTerGrids, pVerts,pIndices);
	_Sticth2(pActBlocks, rcVisible, pTerGrids, pVerts,pIndices);

	m_pSeamStream->UnlockVertexBuffer();
	m_pSeamStream->UnlockIndexBuffer();

	//////////////////////////////////////////////////////////////////////////
	//debug
//	char msg[100];
//	sprintf(msg, "stream max vert %d , face %d  real vert %d face %d\n", m_nVert, m_nMaxFace, m_nSeamVert, m_nSeamFace);
//	OutputDebugStringA(msg);
	//////////////////////////////////////////////////////////////////////////
	

}

void CELOutlineQuadTree::Render(A3DCameraBase * pCamera)
{
	if((!m_bLoadOk)|| (!pCamera->AABBInViewFrustum(m_vBBMins, m_vBBMaxs))) {
		return;
	}
	if ((m_nFace > 0) && (m_pStream != NULL)) {
		m_pStream->Appear();
		m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, m_nVert, 0, m_nFace);
	}
	if ((m_nSeamFace > 0) && (m_pSeamStream != NULL)) {
		m_pSeamStream->Appear();
		m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, m_nSeamVert, 0, m_nSeamFace);
	}

}

void CELOutlineQuadTree::_StitchSingleBlkTop(CELOutlineQuadNode * pNode, A3DVECTOR3* pBlkEdge, int nBlkEdge, int iBlkEdgeLen,  
											 int iLeft, int iTop, A3DLVERTEX * pVerts, WORD* pIndices)
{
	int iBlkTop = iTop + OUTLINE_BLK_LEN;
#define SEAM(p)   (((p)->iLen <= iBlkEdgeLen)&&(((p)->iz + (p)->iLen) == iBlkTop))   
	if (pNode->IsLeaf() || SEAM(pNode) ) {
		_AddSeam(pNode, pBlkEdge,nBlkEdge, iBlkEdgeLen, OLQ_EDGE_TOP, iLeft, iTop, pVerts,pIndices);
	}
#undef  SEAM
	if (!pNode->IsLeaf()) {
		for (int i= 0; i< 4; i++) {
			CELOutlineQuadNode * pChild = &m_pQNodes[pNode->iChildren[i]];
			_StitchSingleBlkTop(pChild, pBlkEdge, nBlkEdge, iBlkEdgeLen, iLeft, iTop, pVerts, pIndices);
		}
	}

}

void CELOutlineQuadTree::_StitchSingleBlkBottom(CELOutlineQuadNode * pNode, A3DVECTOR3* pBlkEdge, int nBlkEdge, int iBlkEdgeLen,  
											 int iLeft, int iTop, A3DLVERTEX * pVerts, WORD* pIndices)
{
	int iBlkBottom = iTop ;
#define SEAM(p)   (((p)->iLen <= iBlkEdgeLen)&&((p)->iz == iBlkBottom))   
	if (pNode->IsLeaf() || SEAM(pNode) ) {
		_AddSeam(pNode, pBlkEdge,nBlkEdge, iBlkEdgeLen, OLQ_EDGE_BOTTOM, iLeft, iTop, pVerts,pIndices);
	}
#undef  SEAM
	if (!pNode->IsLeaf()) {
		for (int i= 0; i< 4; i++) {
			CELOutlineQuadNode * pChild = &m_pQNodes[pNode->iChildren[i]];
			_StitchSingleBlkBottom(pChild, pBlkEdge, nBlkEdge, iBlkEdgeLen, iLeft, iTop, pVerts, pIndices);
		}
	}
}

void CELOutlineQuadTree::_StitchSingleBlkRight(CELOutlineQuadNode * pNode, A3DVECTOR3* pBlkEdge, int nBlkEdge, int iBlkEdgeLen,  
											 int iLeft, int iTop, A3DLVERTEX * pVerts, WORD* pIndices)
{
	int iBlkRight = iLeft ;
#define SEAM(p)   (((p)->iLen <= iBlkEdgeLen)&&((p)->ix == iBlkRight))   
	if (pNode->IsLeaf() || SEAM(pNode) ) {
		_AddSeam(pNode, pBlkEdge,nBlkEdge, iBlkEdgeLen, OLQ_EDGE_RIGHT, iLeft, iTop, pVerts,pIndices);
	}
#undef  SEAM
	if (!pNode->IsLeaf()) {
		for (int i= 0; i< 4; i++) {
			CELOutlineQuadNode * pChild = &m_pQNodes[pNode->iChildren[i]];
			_StitchSingleBlkRight(pChild, pBlkEdge, nBlkEdge, iBlkEdgeLen, iLeft, iTop, pVerts, pIndices);
		}
	}
}


void CELOutlineQuadTree::_StitchSingleBlkLeft(CELOutlineQuadNode * pNode, A3DVECTOR3* pBlkEdge, int nBlkEdge, int iBlkEdgeLen,  
											 int iLeft, int iTop, A3DLVERTEX * pVerts, WORD* pIndices)
{
	int iBlkLeft = iLeft + OUTLINE_BLK_LEN ;
#define SEAM(p)   (((p)->iLen <= iBlkEdgeLen)&&(((p)->ix + (p)->iLen) == iBlkLeft))   
	if (pNode->IsLeaf() || SEAM(pNode) ) {
		_AddSeam(pNode, pBlkEdge,nBlkEdge, iBlkEdgeLen, OLQ_EDGE_LEFT, iLeft, iTop, pVerts,pIndices);
	}
#undef  SEAM
	if (!pNode->IsLeaf()) {
		for (int i= 0; i< 4; i++) {
			CELOutlineQuadNode * pChild = &m_pQNodes[pNode->iChildren[i]];
			_StitchSingleBlkLeft(pChild, pBlkEdge, nBlkEdge, iBlkEdgeLen, iLeft, iTop, pVerts, pIndices);
		}
	}
}


void CELOutlineQuadTree::_ExtractNodeEdge(CELOutlineQuadNode * pNode)
{
	if (pNode->pEdgeVert[OLQ_EDGE_TOP] != NULL) {
		return;
	}
	assert(!pNode->IsLeaf());
	int i;
	for (i= 0; i < 4; i++ ) {
		CELOutlineQuadNode * pChild = &m_pQNodes[pNode->iChildren[i]];
		_ExtractNodeEdge(pChild);
		pNode->pEdgeVert[i] = new WORD[pNode->iLen/OUTLINE_TILE_GRID_LEN + 2];
	}
	
	CELOutlineQuadNode * pChild;
	WORD * pEdgeVert;
	int    nEdgeVert;
	WORD  * pDest;
	int   * pDestCount;
		
	//TOP
	pDest = pNode->pEdgeVert[OLQ_EDGE_TOP];
	pDestCount = &pNode->nEdgeVert[OLQ_EDGE_TOP];
	(*pDestCount) = 0;

	pChild = &m_pQNodes[pNode->iChildren[OLQ_CHILD_LEFTTOP]];
	pEdgeVert = pChild->pEdgeVert[OLQ_EDGE_TOP];
	nEdgeVert = pChild->nEdgeVert[OLQ_EDGE_TOP];
	for (i = 0; i < nEdgeVert; i++) {
		(*pDest) = (*pEdgeVert);
		pDest++;
		pEdgeVert++;
		(*pDestCount)++;
	}
	pChild = &m_pQNodes[pNode->iChildren[OLQ_CHILD_RIGHTTOP]];
	pEdgeVert = pChild->pEdgeVert[OLQ_EDGE_TOP];
	nEdgeVert = pChild->nEdgeVert[OLQ_EDGE_TOP];
	for (i = 1; i < nEdgeVert; i++) {
		(*pDest) = (*pEdgeVert);
		pDest++;
		pEdgeVert++;
		(*pDestCount)++;
	}


	//Right
	pDest = pNode->pEdgeVert[OLQ_EDGE_RIGHT];
	pDestCount = &pNode->nEdgeVert[OLQ_EDGE_RIGHT];
	(*pDestCount) = 0;

	pChild = &m_pQNodes[pNode->iChildren[OLQ_CHILD_RIGHTTOP]];
	pEdgeVert = pChild->pEdgeVert[OLQ_EDGE_RIGHT];
	nEdgeVert = pChild->nEdgeVert[OLQ_EDGE_RIGHT];
	for (i = 0; i < nEdgeVert; i++) {
		(*pDest) = (*pEdgeVert);
		pDest++;
		pEdgeVert++;
		(*pDestCount)++;
	}
	pChild = &m_pQNodes[pNode->iChildren[OLQ_CHILD_RIGHTBOTTOM]];
	pEdgeVert = pChild->pEdgeVert[OLQ_EDGE_RIGHT];
	nEdgeVert = pChild->nEdgeVert[OLQ_EDGE_RIGHT];
	for (i = 1; i < nEdgeVert; i++) {
		(*pDest) = (*pEdgeVert);
		pDest++;
		pEdgeVert++;
		(*pDestCount)++;
	}

	
	//bottom
	pDest = pNode->pEdgeVert[OLQ_EDGE_BOTTOM];
	pDestCount = &pNode->nEdgeVert[OLQ_EDGE_BOTTOM];
	(*pDestCount) = 0;

	pChild = &m_pQNodes[pNode->iChildren[OLQ_CHILD_LEFTBOTTOM]];
	pEdgeVert = pChild->pEdgeVert[OLQ_EDGE_BOTTOM];
	nEdgeVert = pChild->nEdgeVert[OLQ_EDGE_BOTTOM];
	for (i = 0; i < nEdgeVert; i++) {
		(*pDest) = (*pEdgeVert);
		pDest++;
		pEdgeVert++;
		(*pDestCount)++;
	}
	pChild = &m_pQNodes[pNode->iChildren[OLQ_CHILD_RIGHTBOTTOM]];
	pEdgeVert = pChild->pEdgeVert[OLQ_EDGE_BOTTOM];
	nEdgeVert = pChild->nEdgeVert[OLQ_EDGE_BOTTOM];
	for (i = 1; i < nEdgeVert; i++) {
		(*pDest) = (*pEdgeVert);
		pDest++;
		pEdgeVert++;
		(*pDestCount)++;
	}

	//Left
	pDest = pNode->pEdgeVert[OLQ_EDGE_LEFT];
	pDestCount = &pNode->nEdgeVert[OLQ_EDGE_LEFT];
	(*pDestCount) = 0;

	pChild = &m_pQNodes[pNode->iChildren[OLQ_CHILD_LEFTTOP]];
	pEdgeVert = pChild->pEdgeVert[OLQ_EDGE_LEFT];
	nEdgeVert = pChild->nEdgeVert[OLQ_EDGE_LEFT];
	for (i = 0; i < nEdgeVert; i++) {
		(*pDest) = (*pEdgeVert);
		pDest++;
		pEdgeVert++;
		(*pDestCount)++;
	}
	pChild = &m_pQNodes[pNode->iChildren[OLQ_CHILD_LEFTBOTTOM]];
	pEdgeVert = pChild->pEdgeVert[OLQ_EDGE_LEFT];
	nEdgeVert = pChild->nEdgeVert[OLQ_EDGE_LEFT];
	for (i = 1; i < nEdgeVert; i++) {
		(*pDest) = (*pEdgeVert);
		pDest++;
		pEdgeVert++;
		(*pDestCount)++;
	}

}

/**
 * \brief  get vert index in the seam stream.
 *		   If the vert already in the vertbuffer, it returns early.
 *		   Otherwise it creates vert and return the index
 * \param[in] ix, iz  the vert in grid(world)
 * \param[in] iLeft, iTop  the block top left in grid(world)
 * \param[out]
 * \return  the vert index in the seam stream vertex buffer.
 * \note
 * \warning
 * \todo   
 * \author kuiwu 
 * \date 
 * \see 
 */
int CELOutlineQuadTree::_GetSeamVertIndex(int ix, int iz, int iLeft, int iTop, const A3DVECTOR3& pos, A3DLVERTEX * pVerts)
{
	int i, j;

	j = ix - iLeft;
	i = iz - iTop;

	float txleft = m_pELOTexture->m_RectWorld.left;
	float tztop = m_pELOTexture->m_RectWorld.top;
	float tscale = m_pELOTexture->m_vTexScale;

	VERT_INDEX_INFO * pVertIndexInfo = &m_SeamVertIndexInfo[i][j];

	if ((pVertIndexInfo->ix != ix) || (pVertIndexInfo->iz != iz) || (pVertIndexInfo->index < 0 )) {
		pVertIndexInfo->ix = ix;
		pVertIndexInfo->iz = iz;
		pVerts[m_nSeamVert] = A3DLVERTEX(pos, OUTLINE_DIFFUSE, OUTLINE_SPECULAR, (pos.x-txleft)*tscale, (tztop - pos.z)*tscale);
		pVertIndexInfo->index = m_nSeamVert;
		m_nSeamVert++;
	}

	return (pVertIndexInfo->index);
}

/*
 * @param  iLeft :  the seam node block(not in visible active block) left
 * @param  iTop :  the seam node block(not in visible active block) top
 * @param  iEdge :  the block edge flag
 */
void CELOutlineQuadTree::_AddSeam(CELOutlineQuadNode * pNode, A3DVECTOR3* pBlkEdge, int nBlkEdge, int iBlkEdgeLen, int iEdge, int iLeft, int iTop, A3DLVERTEX * pVerts, WORD* pIndices)
{

	if (pNode->pEdgeVert[OLQ_EDGE_TOP] == NULL) {
		_ExtractNodeEdge(pNode);
	}

	int v0, v1, v2;
    int ix, iz;
	int i;

	ix = pNode->ix + (pNode->iLen>>1) ;
	iz = pNode->iz + (pNode->iLen>>1) ;
	v0 = _GetSeamVertIndex(ix, iz, iLeft, iTop, m_pVertPos[pNode->iCenterVert], pVerts);

	//top
	if ((iEdge == OLQ_EDGE_BOTTOM) && (pNode->iz == iTop )) {
		//node top == blk bottom
		i = (pNode->ix - iLeft)/iBlkEdgeLen;
		ix = pNode->ix;
		iz = pNode->iz;
		v1 = _GetSeamVertIndex(ix, iz, iLeft,iTop, pBlkEdge[i], pVerts);
		ix += iBlkEdgeLen;
		i++;
		while (ix <= pNode->ix + pNode->iLen) {
			v2 = _GetSeamVertIndex(ix, iz, iLeft, iTop, pBlkEdge[i], pVerts);
			pIndices[m_nSeamFace*3] = v0;
			pIndices[m_nSeamFace*3+1] = v1;
			pIndices[m_nSeamFace*3+2] = v2;
			m_nSeamFace++;
			v1 = v2;
			ix += iBlkEdgeLen;
			i++;
		}
	}
	else{
		ix = pNode->ix;
		iz = pNode->iz;
		WORD * pNodeEdge =  pNode->pEdgeVert[OLQ_EDGE_TOP];
		int    nNodeEdge = pNode->nEdgeVert[OLQ_EDGE_TOP];

		v1 = _GetSeamVertIndex(ix, iz, iLeft, iTop, m_pVertPos[pNodeEdge[0]], pVerts);
		float fLeft;
		fLeft = m_pVertPos[pNodeEdge[0]].x;
		for (i = 1; i < nNodeEdge; i++) {
			ix = (int)((m_pVertPos[pNodeEdge[i]].x - fLeft)/OUTLINE_TILE_GRID_LEN + 0.5);
			ix += pNode->ix;
			v2 = _GetSeamVertIndex(ix, iz, iLeft, iTop, m_pVertPos[pNodeEdge[i]], pVerts);
			pIndices[m_nSeamFace*3] = v0;
			pIndices[m_nSeamFace*3+1] = v1;
			pIndices[m_nSeamFace*3+2] = v2;
			m_nSeamFace++;
			v1 = v2;
		}
	}
		

		//right
	if ((iEdge == OLQ_EDGE_LEFT) && (pNode->ix + pNode->iLen == iLeft + OUTLINE_BLK_LEN)) {
		//node right == blk left
		i = (pNode->iz - iTop)/iBlkEdgeLen;
		ix = pNode->ix + pNode->iLen;
		iz = pNode->iz;
		v1 = _GetSeamVertIndex(ix, iz, iLeft,iTop, pBlkEdge[i], pVerts);
		iz += iBlkEdgeLen;
		i++;
		while (iz <= pNode->iz + pNode->iLen) {
			v2 = _GetSeamVertIndex(ix, iz, iLeft, iTop, pBlkEdge[i], pVerts);
			pIndices[m_nSeamFace*3] = v0;
			pIndices[m_nSeamFace*3+1] = v1;
			pIndices[m_nSeamFace*3+2] = v2;
			m_nSeamFace++;
			v1 = v2;
			iz += iBlkEdgeLen;
			i++;
		}
	}
	else{
		ix = pNode->ix+ pNode->iLen;
		iz = pNode->iz;
		WORD * pNodeEdge =  pNode->pEdgeVert[OLQ_EDGE_RIGHT];
		int    nNodeEdge = pNode->nEdgeVert[OLQ_EDGE_RIGHT];

		v1 = _GetSeamVertIndex(ix, iz, iLeft, iTop, m_pVertPos[pNodeEdge[0]], pVerts);
		float fTop;
		fTop = m_pVertPos[pNodeEdge[0]].z;
		for (i = 1; i < nNodeEdge; i++) {
			iz = (int)((fTop - m_pVertPos[pNodeEdge[i]].z )/OUTLINE_TILE_GRID_LEN + 0.5);
			iz += pNode->iz;
			v2 = _GetSeamVertIndex(ix, iz, iLeft, iTop, m_pVertPos[pNodeEdge[i]], pVerts);
			pIndices[m_nSeamFace*3] = v0;
			pIndices[m_nSeamFace*3+1] = v1;
			pIndices[m_nSeamFace*3+2] = v2;
			m_nSeamFace++;
			v1 = v2;
		}
	}
		
	

	//bottom
	if ((iEdge == OLQ_EDGE_TOP) && (pNode->iz + pNode->iLen == iTop + OUTLINE_BLK_LEN)) {
		//node bottom == blk top
		i = (pNode->ix - iLeft)/iBlkEdgeLen;
		ix = pNode->ix;
		iz = pNode->iz+ pNode->iLen;
		v1 = _GetSeamVertIndex(ix, iz, iLeft,iTop, pBlkEdge[i], pVerts);
		ix += iBlkEdgeLen;
		i++;
		while (ix <= pNode->ix + pNode->iLen) {
			v2 = _GetSeamVertIndex(ix, iz, iLeft, iTop, pBlkEdge[i], pVerts);
			pIndices[m_nSeamFace*3] = v0;
			pIndices[m_nSeamFace*3+1] = v2;
			pIndices[m_nSeamFace*3+2] = v1;
			m_nSeamFace++;
			v1 = v2;
			ix += iBlkEdgeLen;
			i++;
		}
	}
	else{
		ix = pNode->ix;
		iz = pNode->iz+ pNode->iLen;
		WORD * pNodeEdge =  pNode->pEdgeVert[OLQ_EDGE_BOTTOM];
		int    nNodeEdge = pNode->nEdgeVert[OLQ_EDGE_BOTTOM];

		v1 = _GetSeamVertIndex(ix, iz, iLeft, iTop, m_pVertPos[pNodeEdge[0]], pVerts);
		float fLeft;
		fLeft = m_pVertPos[pNodeEdge[0]].x;
		for (i = 1; i < nNodeEdge; i++) {
			ix = (int)((m_pVertPos[pNodeEdge[i]].x - fLeft)/OUTLINE_TILE_GRID_LEN + 0.5);
			ix += pNode->ix;
			v2 = _GetSeamVertIndex(ix, iz, iLeft, iTop, m_pVertPos[pNodeEdge[i]], pVerts);
			pIndices[m_nSeamFace*3] = v0;
			pIndices[m_nSeamFace*3+1] = v2;
			pIndices[m_nSeamFace*3+2] = v1;
			m_nSeamFace++;
			v1 = v2;
		}
	}
		

	//left
	if ((iEdge == OLQ_EDGE_RIGHT) && (pNode->ix  == iLeft )) {
		//node left == blk right
		i = (pNode->iz - iTop)/iBlkEdgeLen;
		ix = pNode->ix ;
		iz = pNode->iz;
		v1 = _GetSeamVertIndex(ix, iz, iLeft,iTop, pBlkEdge[i], pVerts);
		iz += iBlkEdgeLen;
		i++;
		while (iz <= pNode->iz + pNode->iLen) {
			v2 = _GetSeamVertIndex(ix, iz, iLeft, iTop, pBlkEdge[i], pVerts);
			pIndices[m_nSeamFace*3] = v0;
			pIndices[m_nSeamFace*3+1] = v2;
			pIndices[m_nSeamFace*3+2] = v1;
			m_nSeamFace++;
			v1 = v2;
			iz += iBlkEdgeLen;
			i++;
		}
	}
	else{
		ix = pNode->ix;
		iz = pNode->iz;
		WORD * pNodeEdge =  pNode->pEdgeVert[OLQ_EDGE_LEFT];
		int    nNodeEdge = pNode->nEdgeVert[OLQ_EDGE_LEFT];

		v1 = _GetSeamVertIndex(ix, iz, iLeft, iTop, m_pVertPos[pNodeEdge[0]], pVerts);
		float fTop;
		fTop = m_pVertPos[pNodeEdge[0]].z;
		for (i = 1; i < nNodeEdge; i++) {
			iz = (int)((fTop - m_pVertPos[pNodeEdge[i]].z )/OUTLINE_TILE_GRID_LEN + 0.5);
			iz += pNode->iz;
			v2 = _GetSeamVertIndex(ix, iz, iLeft, iTop, m_pVertPos[pNodeEdge[i]], pVerts);
			pIndices[m_nSeamFace*3] = v0;
			pIndices[m_nSeamFace*3+1] = v2;
			pIndices[m_nSeamFace*3+2] = v1;
			m_nSeamFace++;
			v1 = v2;
		}
	}

	
}

void CELOutlineQuadTree::_StitchTop(int tblr, int iStart, int iEnd, A3DTerrain2::ACTBLOCKS *pActBlocks, A3DTerrain2LOD::GRID * pTerGrid, A3DLVERTEX*pVerts, WORD*pIndices)
{
	int tlGrid = tblr * OUTLINE_BLK_LEN;
	int iCurGrid = iStart * OUTLINE_BLK_LEN;
	A3DTerrain2Block * pBlock;
	int nEdgeVert;
	A3DVECTOR3 edgeVert[OUTLINE_BLK_LEN+4];
	
	VERT_INDEX_INFO commonVertIndex[OUTLINE_BLK_LEN+1];
	

	int iBlkEdgeLen;
	
	//clear the common vert info
	memset(m_SeamVertIndexInfo, -1, sizeof(VERT_INDEX_INFO) * (OUTLINE_BLK_LEN+1) *(OUTLINE_BLK_LEN+1));

	for (int i = iStart; i < iEnd; i++) {
		CELOutlineQuadNode * pQNode = _GetBlkQNode(iCurGrid, tlGrid);

		pBlock = pActBlocks->GetBlock(tblr+1, i, false);
		if (pBlock != NULL) {
			_ExtractBlkEdge(pBlock, pTerGrid, OLQ_EDGE_TOP, nEdgeVert, edgeVert);
			int iLod = pBlock->GetLODLevel();
			//assert(iLod >= 1 && iLod <= 3);
			if (iLod < 1 || iLod > 3)
				iLod = 1;
			iBlkEdgeLen = 1<<(3-iLod);
		}
		else{
			
			nEdgeVert = 0;
			edgeVert[nEdgeVert] = m_pVertPos[pQNode->pEdgeVert[OLQ_EDGE_BOTTOM][0]];
			nEdgeVert++;
			int index = pQNode->nEdgeVert[OLQ_EDGE_BOTTOM]-1;
			edgeVert[nEdgeVert] = m_pVertPos[pQNode->pEdgeVert[OLQ_EDGE_BOTTOM][index]];
			nEdgeVert++;
			iBlkEdgeLen = OUTLINE_BLK_LEN;
		}

		_StitchSingleBlkTop(pQNode, edgeVert, nEdgeVert, iBlkEdgeLen, iCurGrid, tlGrid, pVerts, pIndices );
		//save common edge
		int j;
		for (j = 0; j < OUTLINE_BLK_LEN+1; j++) {
			commonVertIndex[j].ix = m_SeamVertIndexInfo[j][OUTLINE_BLK_LEN].ix;
			commonVertIndex[j].iz = m_SeamVertIndexInfo[j][OUTLINE_BLK_LEN].iz;
			commonVertIndex[j].index = m_SeamVertIndexInfo[j][OUTLINE_BLK_LEN].index;
		}
		//clear vert index info
		memset(m_SeamVertIndexInfo, -1, sizeof(VERT_INDEX_INFO) * (OUTLINE_BLK_LEN+1) *(OUTLINE_BLK_LEN+1));
		//set back
		for (j = 0; j < OUTLINE_BLK_LEN+1; j++) {
			m_SeamVertIndexInfo[j][0].ix = commonVertIndex[j].ix;
			m_SeamVertIndexInfo[j][0].iz = commonVertIndex[j].iz;
			m_SeamVertIndexInfo[j][0].index = commonVertIndex[j].index;
		}
		iCurGrid += OUTLINE_BLK_LEN;
		
	}
}



void CELOutlineQuadTree::_StitchBottom(int tblr, int iStart, int iEnd, A3DTerrain2::ACTBLOCKS *pActBlocks, A3DTerrain2LOD::GRID * pTerGrid, A3DLVERTEX*pVerts, WORD*pIndices)
{
	int tlGrid = tblr * OUTLINE_BLK_LEN;
	int iCurGrid = iStart * OUTLINE_BLK_LEN;
	A3DTerrain2Block * pBlock;
	int nEdgeVert;
	A3DVECTOR3 edgeVert[OUTLINE_BLK_LEN+4];
	
	VERT_INDEX_INFO commonVertIndex[OUTLINE_BLK_LEN+1];
	

	int iBlkEdgeLen;
	
	//clear the common vert info
	memset(m_SeamVertIndexInfo, -1, sizeof(VERT_INDEX_INFO) * (OUTLINE_BLK_LEN+1) *(OUTLINE_BLK_LEN+1));

	for (int i = iStart; i < iEnd; i++) {
		CELOutlineQuadNode * pQNode = _GetBlkQNode(iCurGrid, tlGrid);
		pBlock = pActBlocks->GetBlock(tblr-1, i, false);
		if (pBlock != NULL) {
			_ExtractBlkEdge(pBlock, pTerGrid, OLQ_EDGE_BOTTOM, nEdgeVert, edgeVert);
			int iLod = pBlock->GetLODLevel();
			//assert(iLod >= 1 && iLod <= 3);
			if (iLod < 1 || iLod > 3)
				iLod = 1;
			iBlkEdgeLen = 1<<(3-iLod);
		}
		else{
			nEdgeVert = 0;
			edgeVert[nEdgeVert] = m_pVertPos[pQNode->pEdgeVert[OLQ_EDGE_TOP][0]];
			nEdgeVert++;
			int index = pQNode->nEdgeVert[OLQ_EDGE_TOP]-1;
			edgeVert[nEdgeVert] = m_pVertPos[pQNode->pEdgeVert[OLQ_EDGE_TOP][index]];
			nEdgeVert++;
			iBlkEdgeLen = OUTLINE_BLK_LEN;
		}

		_StitchSingleBlkBottom(pQNode, edgeVert, nEdgeVert, iBlkEdgeLen, iCurGrid, tlGrid, pVerts, pIndices );
		//save common edge
		int j;
		for (j = 0; j < OUTLINE_BLK_LEN+1; j++) {
			commonVertIndex[j].ix = m_SeamVertIndexInfo[j][OUTLINE_BLK_LEN].ix;
			commonVertIndex[j].iz = m_SeamVertIndexInfo[j][OUTLINE_BLK_LEN].iz;
			commonVertIndex[j].index = m_SeamVertIndexInfo[j][OUTLINE_BLK_LEN].index;
		}
		//clear vert index info
		memset(m_SeamVertIndexInfo, -1, sizeof(VERT_INDEX_INFO) * (OUTLINE_BLK_LEN+1) *(OUTLINE_BLK_LEN+1));
		//set back
		for (j = 0; j < OUTLINE_BLK_LEN+1; j++) {
			m_SeamVertIndexInfo[j][0].ix = commonVertIndex[j].ix;
			m_SeamVertIndexInfo[j][0].iz = commonVertIndex[j].iz;
			m_SeamVertIndexInfo[j][0].index = commonVertIndex[j].index;
		}
		iCurGrid += OUTLINE_BLK_LEN;
		
	}
}


void CELOutlineQuadTree::_StitchRight(int tblr, int iStart, int iEnd, A3DTerrain2::ACTBLOCKS *pActBlocks, A3DTerrain2LOD::GRID * pTerGrid, A3DLVERTEX*pVerts, WORD*pIndices)
{
	int tlGrid = tblr * OUTLINE_BLK_LEN;
	int iCurGrid = iStart * OUTLINE_BLK_LEN;
	A3DTerrain2Block * pBlock;
	int nEdgeVert;
	A3DVECTOR3 edgeVert[OUTLINE_BLK_LEN+4];
	
	VERT_INDEX_INFO commonVertIndex[OUTLINE_BLK_LEN+1];
	

	int iBlkEdgeLen;
	
	//clear the common vert info
	memset(m_SeamVertIndexInfo, -1, sizeof(VERT_INDEX_INFO) * (OUTLINE_BLK_LEN+1) *(OUTLINE_BLK_LEN+1));

	for (int i = iStart; i < iEnd; i++) {
		CELOutlineQuadNode * pQNode = _GetBlkQNode(tlGrid, iCurGrid);


		pBlock = pActBlocks->GetBlock(i, tblr-1, false);
		if (pBlock != NULL) {
			_ExtractBlkEdge(pBlock, pTerGrid, OLQ_EDGE_RIGHT, nEdgeVert, edgeVert);
			int iLod = pBlock->GetLODLevel();
			//assert(iLod >= 1 && iLod <= 3);
			if (iLod < 1 || iLod > 3)
				iLod = 1;
			iBlkEdgeLen = 1<<(3-iLod);
		}
		else{
			nEdgeVert = 0;
			edgeVert[nEdgeVert] = m_pVertPos[pQNode->pEdgeVert[OLQ_EDGE_LEFT][0]];
			nEdgeVert++;
			int index = pQNode->nEdgeVert[OLQ_EDGE_LEFT]-1;
			edgeVert[nEdgeVert] = m_pVertPos[pQNode->pEdgeVert[OLQ_EDGE_LEFT][index]];
			nEdgeVert++;
			iBlkEdgeLen = OUTLINE_BLK_LEN;
		}

		_StitchSingleBlkRight(pQNode, edgeVert, nEdgeVert, iBlkEdgeLen, tlGrid, iCurGrid, pVerts, pIndices );
		//save common edge
		int j;
		for (j = 0; j < OUTLINE_BLK_LEN+1; j++) {
			commonVertIndex[j].ix = m_SeamVertIndexInfo[OUTLINE_BLK_LEN][j].ix;
			commonVertIndex[j].iz = m_SeamVertIndexInfo[OUTLINE_BLK_LEN][j].iz;
			commonVertIndex[j].index = m_SeamVertIndexInfo[OUTLINE_BLK_LEN][j].index;
		}
		//clear vert index info
		memset(m_SeamVertIndexInfo, -1, sizeof(VERT_INDEX_INFO) * (OUTLINE_BLK_LEN+1) *(OUTLINE_BLK_LEN+1));
		//set back
		for (j = 0; j < OUTLINE_BLK_LEN+1; j++) {
			m_SeamVertIndexInfo[0][j].ix = commonVertIndex[j].ix;
			m_SeamVertIndexInfo[0][j].iz = commonVertIndex[j].iz;
			m_SeamVertIndexInfo[0][j].index = commonVertIndex[j].index;
		}
		iCurGrid += OUTLINE_BLK_LEN;
		
	}
}



void CELOutlineQuadTree::_StitchLeft(int tblr, int iStart, int iEnd, A3DTerrain2::ACTBLOCKS *pActBlocks, A3DTerrain2LOD::GRID * pTerGrid, A3DLVERTEX*pVerts, WORD*pIndices)
{
	int tlGrid = tblr * OUTLINE_BLK_LEN;
	int iCurGrid = iStart * OUTLINE_BLK_LEN;
	A3DTerrain2Block * pBlock;
	int nEdgeVert;
	A3DVECTOR3 edgeVert[OUTLINE_BLK_LEN+4];
	
	VERT_INDEX_INFO commonVertIndex[OUTLINE_BLK_LEN+1];
	

	int iBlkEdgeLen;
	
	//clear the common vert info
	memset(m_SeamVertIndexInfo, -1, sizeof(VERT_INDEX_INFO) * (OUTLINE_BLK_LEN+1) *(OUTLINE_BLK_LEN+1));

	for (int i = iStart; i < iEnd; i++) {
		CELOutlineQuadNode * pQNode = _GetBlkQNode(tlGrid, iCurGrid);

		pBlock = pActBlocks->GetBlock(i, tblr+1, false);
		if (pBlock != NULL) {
			_ExtractBlkEdge(pBlock, pTerGrid, OLQ_EDGE_LEFT, nEdgeVert, edgeVert);
			int iLod = pBlock->GetLODLevel();
			//assert(iLod >= 1 && iLod <= 3);
			if (iLod < 1 || iLod > 3)
				iLod = 1;
			iBlkEdgeLen = 1<<(3-iLod);
		}
		else{
			nEdgeVert = 0;
			edgeVert[nEdgeVert] = m_pVertPos[pQNode->pEdgeVert[OLQ_EDGE_RIGHT][0]];
			nEdgeVert++;
			int index = pQNode->nEdgeVert[OLQ_EDGE_RIGHT]-1;
			edgeVert[nEdgeVert] = m_pVertPos[pQNode->pEdgeVert[OLQ_EDGE_RIGHT][index]];
			nEdgeVert++;
			iBlkEdgeLen = OUTLINE_BLK_LEN;

		}
		_StitchSingleBlkLeft(pQNode, edgeVert, nEdgeVert, iBlkEdgeLen, tlGrid, iCurGrid, pVerts, pIndices );
		//save common edge
		int j;
		for (j = 0; j < OUTLINE_BLK_LEN+1; j++) {
			commonVertIndex[j].ix = m_SeamVertIndexInfo[OUTLINE_BLK_LEN][j].ix;
			commonVertIndex[j].iz = m_SeamVertIndexInfo[OUTLINE_BLK_LEN][j].iz;
			commonVertIndex[j].index = m_SeamVertIndexInfo[OUTLINE_BLK_LEN][j].index;
		}
		//clear vert index info
		memset(m_SeamVertIndexInfo, -1, sizeof(VERT_INDEX_INFO) * (OUTLINE_BLK_LEN+1) *(OUTLINE_BLK_LEN+1));
		//set back
		for (j = 0; j < OUTLINE_BLK_LEN+1; j++) {
			m_SeamVertIndexInfo[0][j].ix = commonVertIndex[j].ix;
			m_SeamVertIndexInfo[0][j].iz = commonVertIndex[j].iz;
			m_SeamVertIndexInfo[0][j].index = commonVertIndex[j].index;
		}
		iCurGrid += OUTLINE_BLK_LEN;
		
	}
}





void CELOutlineQuadTree::_Sticth(A3DTerrain2::ACTBLOCKS *pActBlocks, const ARectI& rcVisible, A3DTerrain2LOD::GRID * pTerGrid, A3DLVERTEX*pVerts, WORD*pIndices)
{
	ARectI rcSeamBlk(rcVisible);
	rcSeamBlk.Inflate(1,1);
//   overlapping has been already checked.  By kuiwu. [14/2/2006]
//	if (!CELOutlineTextureCache::RcOverlapRc(rcSeamBlk, m_RectBlk)) {
//		return;
//	}

	//top edge
	if (rcSeamBlk.top >= m_RectBlk.top) {
		int iStart, iEnd;
		iStart = max(m_RectBlk.left, rcSeamBlk.left+1);
		iEnd = min(m_RectBlk.right, rcSeamBlk.right-1);
		
		_StitchTop(rcSeamBlk.top,  iStart, iEnd, pActBlocks, pTerGrid, pVerts, pIndices);
	}
	//right edge
	if (rcSeamBlk.right <= m_RectBlk.right) {
		int iStart, iEnd;
		iStart = max(m_RectBlk.top, rcSeamBlk.top+1);
		iEnd = min(m_RectBlk.bottom, rcSeamBlk.bottom-1);
		_StitchRight(rcSeamBlk.right-1, iStart, iEnd, pActBlocks, pTerGrid, pVerts, pIndices);
	}
	//bottom edge
	if (rcSeamBlk.bottom <= m_RectBlk.bottom) {
		int iStart, iEnd;
		iStart = max(m_RectBlk.left, rcSeamBlk.left+1);
		iEnd = min(m_RectBlk.right, rcSeamBlk.right-1);
		_StitchBottom(rcSeamBlk.bottom-1, iStart, iEnd, pActBlocks, pTerGrid, pVerts, pIndices );
	}
	//left edge
	if (rcSeamBlk.left >= m_RectBlk.left) {
		int iStart, iEnd;
		iStart = max(m_RectBlk.top, rcSeamBlk.top+1);
		iEnd = min(m_RectBlk.bottom, rcSeamBlk.bottom-1);
		_StitchLeft(rcSeamBlk.left, iStart, iEnd, pActBlocks, pTerGrid, pVerts, pIndices);
	}
}
void CELOutlineQuadTree::_CompleteTerrain(A3DTerrain2::ACTBLOCKS *pActBlocks, const ARectI& rcVisible, A3DTerrain2LOD::GRID * pTerGrids, A3DLVERTEX*pVerts, WORD*pIndices)
{
	if (!CELOutlineTextureCache::RcOverlapRc(m_RectBlk, rcVisible)) {
		return;
	}
	int left, top, right, bottom;
	left = max(m_RectBlk.left, rcVisible.left);
	right = min(m_RectBlk.right, rcVisible.right);
	top = max(m_RectBlk.top, rcVisible.top);
	bottom = min(m_RectBlk.bottom, rcVisible.bottom);
	
	int r, c;
	A3DTerrain2Block * pBlock;
	//build the terrain
	for (r = top; r < bottom; r++) {
		for (c = left;  c < right; c++ ) {
			pBlock = pActBlocks->GetBlock(r, c, false);
			if (pBlock == NULL){
				//_CompleteTerrain(r, c, pActBlocks, rcVisible, pTerGrids, pVerts, pIndices);
				_CompleteTerrain2(r, c, pActBlocks, pTerGrids, pVerts, pIndices);
			}
		}
	}
	
}


float CELOutlineQuadTree::_GetHeight(float x, float z)
{
	float y = -1.0f;
	for (int i = 0; i < m_nVert; i++) {
		if((m_pVertPos[i].x - x < 0.1 ) 
		  &&(m_pVertPos[i].x - x > -0.1 ) 
		  &&(m_pVertPos[i].z - z < 0.1 ) 
		  &&(m_pVertPos[i].z - z > -0.1 ) ){
			 y= m_pVertPos[i].y;
			 break;
		}
	}
	assert(y > -0.1f );
	return y;
}

CELOutlineQuadNode * CELOutlineQuadTree::_GetBlkQNode(float x, float z)
{
	//world -> grid
	int ix, iz;
	ix = m_Rect.left + (int)(x - m_RectWorld.left + 0.1)/OUTLINE_TILE_GRID_LEN;
	iz = m_Rect.top  + (int)(m_RectWorld.top - z + 0.1)/OUTLINE_TILE_GRID_LEN;

	return _GetBlkQNode(ix, iz);
}

CELOutlineQuadNode * CELOutlineQuadTree::_GetBlkQNode(int ix, int iz)
{
	CELOutlineQuadNode * pQNode = NULL;
	for (int i = 0; i < m_nQNodes; i++) {
		if ((m_pQNodes[i].iLen == OUTLINE_BLK_LEN) 
			&& (m_pQNodes[i].ix == ix)
			&& (m_pQNodes[i].iz == iz)){
			pQNode = &m_pQNodes[i];
			break;
		}
	}
	assert(pQNode != NULL);
	return pQNode;
}
/**
 * \brief 
 * \param[in]
 * \param[out]
 * \return  the terrain blk edge interval(in grid)
 * \note    the edge vert order
 *			|--------------->		
 *          |				|
 *			|				|
 *			|				|
 *			|				|
 *		   \|/			   \|/
 *			 --------------->
 * \warning
 * \todo   
 * \author kuiwu 
 * \date 
 * \see 
 */
int CELOutlineQuadTree::_ExtractBlkEdge(A3DTerrain2Block * pBlock, A3DTerrain2LOD::GRID * pTerGrids,int edge, int& nVert, A3DVECTOR3 * pVerts)
{
	
	nVert = 0;
	int iLod = pBlock->GetLODLevel();
	int nGrid = pBlock->GetBlockGrid();
	int edgelen; //in grid
	if (iLod < 1 || iLod > 3)
		iLod = 1;
	edgelen = 1<<(3-iLod);
	int iCurGrid= 0;
	int iGrid, index;
	A3DTerrain2LOD::GRID* pGrid;
	
   /**< 
   // v3  v0----v1
   // | \  \    | 
   // |  \  \   | 
   // |   \  \  | 
   // | \  \ | 
   // v5---v4  v2
   */
	
	switch(edge) {
	case OLQ_EDGE_BOTTOM:
		while (iCurGrid < nGrid) {
			iGrid = ((nGrid-1)*nGrid + iCurGrid);
			pGrid = &pTerGrids[iGrid];
			index =  (pGrid->v5);
			pVerts[nVert]  = pBlock->GetVertexPos(index);
			nVert++;
			iCurGrid += edgelen;
		}
		iGrid = (nGrid*nGrid -1);
		pGrid = &pTerGrids[iGrid];
		index = (pGrid->v4);
		pVerts[nVert]  = pBlock->GetVertexPos(index);
		nVert++;
		break;
	case OLQ_EDGE_LEFT:
		while (iCurGrid < nGrid) {
			iGrid = (iCurGrid*nGrid);
			pGrid = &pTerGrids[iGrid];
			index = (pGrid->v3);
			pVerts[nVert]  = pBlock->GetVertexPos(index);
			nVert++;
			iCurGrid += edgelen;
		}
		iGrid = (nGrid*(nGrid-1));
		pGrid = &pTerGrids[iGrid];
		index = (pGrid->v5);
		pVerts[nVert]  = pBlock->GetVertexPos(index);
		nVert++;
		break;
	case OLQ_EDGE_TOP:
		while (iCurGrid < nGrid) {
			iGrid = (iCurGrid);
			pGrid = &pTerGrids[iGrid];
			index =  (pGrid->v0);
			pVerts[nVert]  = pBlock->GetVertexPos(index);
			nVert++;
			iCurGrid += edgelen;
		}
		iGrid = (nGrid-1);
		pGrid = &pTerGrids[iGrid];
		index = pGrid->v1;
		pVerts[nVert]  = pBlock->GetVertexPos(index);
		nVert++;
		break;
	case OLQ_EDGE_RIGHT:
		while (iCurGrid < nGrid) {
			iGrid = (iCurGrid*nGrid) + nGrid-1;
			pGrid = &pTerGrids[iGrid];
			index = (pGrid->v1);
			pVerts[nVert]  = pBlock->GetVertexPos(index);
			nVert++;
			iCurGrid += edgelen;
		}
		iGrid = (nGrid*nGrid-1);
		pGrid = &pTerGrids[iGrid];
		index = (pGrid->v2);
		pVerts[nVert]  = pBlock->GetVertexPos(index);
		nVert++;
		break;
	default:
		assert(0);
		break;
	}
	
	return edgelen;

}

void CELOutlineQuadTree::_CompleteTerrain(int r, int c, A3DTerrain2::ACTBLOCKS *pActBlocks, 
										  const ARectI& rcVisible, A3DTerrain2LOD::GRID * pTerGrids, 
										  A3DLVERTEX*pVerts, WORD*pIndices)
{
		///the current block is empty
		int ix, iz;
		ix = m_Rect.left + (c - m_RectBlk.left)* OUTLINE_BLK_LEN;
		iz = m_Rect.top  + (r - m_RectBlk.top)* OUTLINE_BLK_LEN;
		CELOutlineQuadNode * pQNode = _GetBlkQNode(ix, iz);
		
		A3DVECTOR3 center;
		center = m_pVertPos[pQNode->iCenterVert];
		
		int nEdgeVert[4];
		nEdgeVert[OLQ_EDGE_TOP] = 0;
		nEdgeVert[OLQ_EDGE_RIGHT] = 0;
		nEdgeVert[OLQ_EDGE_BOTTOM] = 0;
		nEdgeVert[OLQ_EDGE_LEFT] = 0;
		A3DVECTOR3  edgeVert[4][OUTLINE_BLK_LEN+4];

		//////////////////////////////////////////////////////////////////////////
		//debug
//		int facebegin = m_nSeamFace;
//		char msg[100] ;
		//////////////////////////////////////////////////////////////////////////
		
		
		A3DTerrain2Block *pLeftBlock, *pTopBlock, *pRightBlock, *pBottomBlock;
		if (pActBlocks->rcArea.PtInRect(c-1, r)
			&& (pLeftBlock = pActBlocks->GetBlock(r, c-1, false))!= NULL) {
			_ExtractBlkEdge(pLeftBlock, pTerGrids, OLQ_EDGE_RIGHT, nEdgeVert[OLQ_EDGE_LEFT], edgeVert[OLQ_EDGE_LEFT]);
		}
		else
		{
			int index = pQNode->pEdgeVert[OLQ_EDGE_LEFT][0];
			edgeVert[OLQ_EDGE_LEFT][nEdgeVert[OLQ_EDGE_LEFT]] = m_pVertPos[index];
			nEdgeVert[OLQ_EDGE_LEFT]++;
			index = pQNode->pEdgeVert[OLQ_EDGE_LEFT][pQNode->nEdgeVert[OLQ_EDGE_LEFT]-1];
			edgeVert[OLQ_EDGE_LEFT][nEdgeVert[OLQ_EDGE_LEFT]] = m_pVertPos[index];
			nEdgeVert[OLQ_EDGE_LEFT]++;
		}
		
		if (pActBlocks->rcArea.PtInRect(c+1, r)
			&&(pRightBlock = pActBlocks->GetBlock(r, c+1, false))!= NULL) {
			_ExtractBlkEdge(pRightBlock, pTerGrids, OLQ_EDGE_LEFT, nEdgeVert[OLQ_EDGE_RIGHT], edgeVert[OLQ_EDGE_RIGHT]);
		}
		else
		{
			int index = pQNode->pEdgeVert[OLQ_EDGE_RIGHT][0];
			edgeVert[OLQ_EDGE_RIGHT][nEdgeVert[OLQ_EDGE_RIGHT]] = m_pVertPos[index];
			nEdgeVert[OLQ_EDGE_RIGHT]++;
			index = pQNode->pEdgeVert[OLQ_EDGE_RIGHT][pQNode->nEdgeVert[OLQ_EDGE_RIGHT]-1];
			edgeVert[OLQ_EDGE_RIGHT][nEdgeVert[OLQ_EDGE_RIGHT]] = m_pVertPos[index];
			nEdgeVert[OLQ_EDGE_RIGHT]++;
		}


		if (pActBlocks->rcArea.PtInRect(c, r-1)
			&&(pTopBlock = pActBlocks->GetBlock(r-1, c, false))!= NULL) {
			_ExtractBlkEdge(pTopBlock, pTerGrids, OLQ_EDGE_BOTTOM, nEdgeVert[OLQ_EDGE_TOP], edgeVert[OLQ_EDGE_TOP]);
		}
		else
		{
			int index = pQNode->pEdgeVert[OLQ_EDGE_TOP][0];
			edgeVert[OLQ_EDGE_TOP][nEdgeVert[OLQ_EDGE_TOP]] = m_pVertPos[index];
			nEdgeVert[OLQ_EDGE_TOP]++;
			index = pQNode->pEdgeVert[OLQ_EDGE_TOP][pQNode->nEdgeVert[OLQ_EDGE_TOP]-1];
			edgeVert[OLQ_EDGE_TOP][nEdgeVert[OLQ_EDGE_TOP]] = m_pVertPos[index];
			nEdgeVert[OLQ_EDGE_TOP]++;
		}
		if (pActBlocks->rcArea.PtInRect(c, r+1)
			&&(pBottomBlock = pActBlocks->GetBlock(r+1, c, false))!= NULL) {
			_ExtractBlkEdge(pBottomBlock, pTerGrids, OLQ_EDGE_TOP, nEdgeVert[OLQ_EDGE_BOTTOM], edgeVert[OLQ_EDGE_BOTTOM]);
		}
		else
		{
			int index = pQNode->pEdgeVert[OLQ_EDGE_BOTTOM][0];
			edgeVert[OLQ_EDGE_BOTTOM][nEdgeVert[OLQ_EDGE_BOTTOM]] = m_pVertPos[index];
			nEdgeVert[OLQ_EDGE_BOTTOM]++;
			index = pQNode->pEdgeVert[OLQ_EDGE_BOTTOM][pQNode->nEdgeVert[OLQ_EDGE_BOTTOM]-1];
			edgeVert[OLQ_EDGE_BOTTOM][nEdgeVert[OLQ_EDGE_BOTTOM]] = m_pVertPos[index];
			nEdgeVert[OLQ_EDGE_BOTTOM]++;
		}
		
		float txleft = m_pELOTexture->m_RectWorld.left;
		float tztop = m_pELOTexture->m_RectWorld.top;
		float tscale = m_pELOTexture->m_vTexScale;
		int v0, v1, v2;

		pVerts[m_nSeamVert] = A3DLVERTEX(center, OUTLINE_DIFFUSE, OUTLINE_SPECULAR,
			(center.x-txleft)*tscale, (tztop - center.z)*tscale );
		v0 = m_nSeamVert;
		m_nSeamVert++;
		int tl, tr, bl, br;
		A3DVECTOR3 * ppos;
		//top left
		ppos =  &edgeVert[OLQ_EDGE_TOP][0];
		pVerts[m_nSeamVert] = A3DLVERTEX(*ppos, OUTLINE_DIFFUSE, OUTLINE_SPECULAR,
			(ppos->x - txleft)*tscale, (tztop -ppos->z)*tscale);
		tl = m_nSeamVert;
		m_nSeamVert++;
		//top right
		ppos = &edgeVert[OLQ_EDGE_TOP][nEdgeVert[OLQ_EDGE_TOP]-1]; 
		pVerts[m_nSeamVert] = A3DLVERTEX(*ppos, OUTLINE_DIFFUSE, OUTLINE_SPECULAR,
			(ppos->x - txleft)*tscale, (tztop -ppos->z)*tscale);
		tr = m_nSeamVert;
		m_nSeamVert++;
		//bottom left
		ppos = &edgeVert[OLQ_EDGE_BOTTOM][0];
		pVerts[m_nSeamVert] = A3DLVERTEX(*ppos, OUTLINE_DIFFUSE, OUTLINE_SPECULAR,
			(ppos->x - txleft)*tscale, (tztop - ppos->z)*tscale);
		bl = m_nSeamVert;
		m_nSeamVert++;
		//bottom right
		ppos =&edgeVert[OLQ_EDGE_BOTTOM][nEdgeVert[OLQ_EDGE_BOTTOM]-1];
		pVerts[m_nSeamVert] = A3DLVERTEX(*ppos, OUTLINE_DIFFUSE, OUTLINE_SPECULAR,
			(ppos->x - txleft)*tscale, (tztop -ppos->z)*tscale);
		br = m_nSeamVert;
		m_nSeamVert++;
		int i;
		//top edge
		v1 = tl;
		for ( i = 1; i < nEdgeVert[OLQ_EDGE_TOP]-1; i++) {
			pVerts[m_nSeamVert] = A3DLVERTEX(edgeVert[OLQ_EDGE_TOP][i], OUTLINE_DIFFUSE, OUTLINE_SPECULAR,
				(edgeVert[OLQ_EDGE_TOP][i].x - txleft)*tscale, (tztop -edgeVert[OLQ_EDGE_TOP][i].z)*tscale);
			v2 = m_nSeamVert;
			m_nSeamVert++;
			pIndices[m_nSeamFace*3] = v0;
			pIndices[m_nSeamFace*3+1] = v1;
			pIndices[m_nSeamFace*3+2] = v2;
			m_nSeamFace++;
			v1 = v2;
		}
		v2 = tr;
		pIndices[m_nSeamFace*3] = v0;
		pIndices[m_nSeamFace*3+1] = v1;
		pIndices[m_nSeamFace*3+2] = v2;
		m_nSeamFace++;
		
		//right edge
		v1 = tr;
		for (i = 1; i < nEdgeVert[OLQ_EDGE_RIGHT]-1; i++) {
			pVerts[m_nSeamVert] = A3DLVERTEX(edgeVert[OLQ_EDGE_RIGHT][i], OUTLINE_DIFFUSE, OUTLINE_SPECULAR,
				(edgeVert[OLQ_EDGE_RIGHT][i].x - txleft)*tscale, (tztop -edgeVert[OLQ_EDGE_RIGHT][i].z)*tscale);
			v2 = m_nSeamVert;	
			m_nSeamVert++;
			pIndices[m_nSeamFace*3] = v0;
			pIndices[m_nSeamFace*3+1] = v1;
			pIndices[m_nSeamFace*3+2] = v2;
			m_nSeamFace++;
			v1 = v2;
		}
		v2 = br;
		pIndices[m_nSeamFace*3] = v0;
		pIndices[m_nSeamFace*3+1] = v1;
		pIndices[m_nSeamFace*3+2] = v2;
		m_nSeamFace++;


		//bottom edge
		v1 = bl;
		for ( i = 1; i < nEdgeVert[OLQ_EDGE_BOTTOM]-1; i++) {
			pVerts[m_nSeamVert] = A3DLVERTEX(edgeVert[OLQ_EDGE_BOTTOM][i], OUTLINE_DIFFUSE, OUTLINE_SPECULAR,
				(edgeVert[OLQ_EDGE_BOTTOM][i].x - txleft)*tscale, (tztop -edgeVert[OLQ_EDGE_BOTTOM][i].z)*tscale);
			v2 = m_nSeamVert;	
			m_nSeamVert++;
			pIndices[m_nSeamFace*3] = v0;
			pIndices[m_nSeamFace*3+1] = v2;
			pIndices[m_nSeamFace*3+2] = v1;
			m_nSeamFace++;
			v1 = v2;
		}
		v2 = br;
		pIndices[m_nSeamFace*3] = v0;
		pIndices[m_nSeamFace*3+1] = v2;
		pIndices[m_nSeamFace*3+2] = v1;
		m_nSeamFace++;

		//left edge
		v1 = tl;
		for ( i = 1; i < nEdgeVert[OLQ_EDGE_LEFT]-1; i++) {
			pVerts[m_nSeamVert] = A3DLVERTEX(edgeVert[OLQ_EDGE_LEFT][i], OUTLINE_DIFFUSE, OUTLINE_SPECULAR,
				(edgeVert[OLQ_EDGE_LEFT][i].x - txleft)*tscale, (tztop -edgeVert[OLQ_EDGE_LEFT][i].z)*tscale);
			v2 = m_nSeamVert;	
			m_nSeamVert++;
			pIndices[m_nSeamFace*3] = v0;
			pIndices[m_nSeamFace*3+1] = v2;
			pIndices[m_nSeamFace*3+2] = v1;
			m_nSeamFace++;
			v1 = v2;
		}
		v2 = bl;
		pIndices[m_nSeamFace*3] = v0;
		pIndices[m_nSeamFace*3+1] = v2;
		pIndices[m_nSeamFace*3+2] = v1;
		m_nSeamFace++;

		//////////////////////////////////////////////////////////////////////////
		//debug
//		sprintf(msg, "fake face %d\n", m_nSeamFace - facebegin);
//		OutputDebugStringA(msg);
//		for (i = facebegin; i < m_nSeamFace; i++) {
//			v0 = pIndices[i*3];
//			v1 = pIndices[i*3+1];
//			v2 = pIndices[i*3+2];
//			sprintf (msg, "%d %3.1f, %3.1f %3.1f %d %3.1f, %3.1f %3.1f 	%d %3.1f, %3.1f %3.1f\n", 
//				v0, pVerts[v0].x, pVerts[v0].y, pVerts[v0].z,
//				v1, pVerts[v1].x, pVerts[v1].y, pVerts[v1].z,
//				v2, pVerts[v2].x, pVerts[v2].y, pVerts[v2].z);
//			OutputDebugStringA(msg);
//		}
		//////////////////////////////////////////////////////////////////////////
		
}


/**
 * \brief extract the edge verts from the QNode 
 * \param[in]  
 * \param[out] ppEdgeVert the result edge vert pointer, end with NULL.
 * \return  the count of the edge vert
 * \note
 * \warning  
 * \todo   
 * \author kuiwu 
 * \date 14/2/2006
 * \see 
 */
 int CELOutlineQuadTree::_ExtractNodeEdge(CELOutlineQuadNode * pQNode, int EdgeFlag, A3DVECTOR3 * ppEdgeVert[])
 {
	 if (pQNode->pEdgeVert[EdgeFlag] == NULL)
	 {
		 _ExtractNodeEdge(pQNode);
	 }
	WORD * pInx = pQNode->pEdgeVert[EdgeFlag];
	assert(pInx != NULL && pQNode->nEdgeVert[EdgeFlag] > 0);
	int c = 0;
	for (int j = 0; j < pQNode->nEdgeVert[EdgeFlag]; ++j)
	{
		ppEdgeVert[c] = &m_pVertPos[pInx[j]];
		++c;
	}
	ppEdgeVert[c] = NULL;

	return c;
 }

/**
 * \brief  complete the terrain block with olm if not loaded yet.
 * \param[in]
 * \param[out]
 * \return
 * \note  
 * \warning  
 * \todo     
 * \author kuiwu 
 * \date 14/2/2006
 * \see 
 */

void CELOutlineQuadTree::_CompleteTerrain2(int r, int c, A3DTerrain2::ACTBLOCKS *pActBlocks, 
										  A3DTerrain2LOD::GRID * pTerGrids, 
										  A3DLVERTEX*pVerts, WORD*pIndices)
{
		///the current block is empty

		Seam_Block_Info  seamInfo;
		A3DVECTOR3  BlkEdgeVert[4][OUTLINE_BLK_LEN+4];

		//default extract the edge from the olm
		seamInfo.edge[OLQ_EDGE_LEFT].pVerts = NULL;
		seamInfo.edge[OLQ_EDGE_TOP].pVerts = NULL;
		seamInfo.edge[OLQ_EDGE_RIGHT].pVerts = NULL;
		seamInfo.edge[OLQ_EDGE_BOTTOM].pVerts = NULL;

		///< extract the edge from the terrain blk if loaded		
		A3DTerrain2Block *pNeighborBlk;
		if (pActBlocks->rcArea.PtInRect(c-1, r)
			&& (pNeighborBlk = pActBlocks->GetBlock(r, c-1, false))!= NULL) 
		{
			seamInfo.edge[OLQ_EDGE_LEFT].nInterval= _ExtractBlkEdge(pNeighborBlk, pTerGrids, OLQ_EDGE_RIGHT, 
								seamInfo.edge[OLQ_EDGE_LEFT].nVerts, BlkEdgeVert[OLQ_EDGE_LEFT]);
			seamInfo.edge[OLQ_EDGE_LEFT].pVerts = BlkEdgeVert[OLQ_EDGE_LEFT];
			
		}
			
		if (pActBlocks->rcArea.PtInRect(c+1, r)
			&&(pNeighborBlk = pActBlocks->GetBlock(r, c+1, false))!= NULL) 
		{
			seamInfo.edge[OLQ_EDGE_RIGHT].nInterval= _ExtractBlkEdge(pNeighborBlk, pTerGrids, OLQ_EDGE_LEFT, 
								seamInfo.edge[OLQ_EDGE_RIGHT].nVerts, BlkEdgeVert[OLQ_EDGE_RIGHT]);
			seamInfo.edge[OLQ_EDGE_RIGHT].pVerts = BlkEdgeVert[OLQ_EDGE_RIGHT];
		}

		if (pActBlocks->rcArea.PtInRect(c, r-1)
			&&(pNeighborBlk = pActBlocks->GetBlock(r-1, c, false))!= NULL) 
		{
			seamInfo.edge[OLQ_EDGE_TOP].nInterval= _ExtractBlkEdge(pNeighborBlk, pTerGrids, OLQ_EDGE_BOTTOM, 
								seamInfo.edge[OLQ_EDGE_TOP].nVerts, BlkEdgeVert[OLQ_EDGE_TOP]);
			seamInfo.edge[OLQ_EDGE_TOP].pVerts = BlkEdgeVert[OLQ_EDGE_TOP];
		}

		if (pActBlocks->rcArea.PtInRect(c, r+1)
			&&(pNeighborBlk = pActBlocks->GetBlock(r+1, c, false))!= NULL) 
		{
			seamInfo.edge[OLQ_EDGE_BOTTOM].nInterval= _ExtractBlkEdge(pNeighborBlk, pTerGrids, OLQ_EDGE_TOP, 
								seamInfo.edge[OLQ_EDGE_BOTTOM].nVerts, BlkEdgeVert[OLQ_EDGE_BOTTOM]);
			seamInfo.edge[OLQ_EDGE_BOTTOM].pVerts = BlkEdgeVert[OLQ_EDGE_BOTTOM];
		}

		//clear the common vert info
		memset(m_SeamVertIndexInfo, -1, sizeof(VERT_INDEX_INFO) * (OUTLINE_BLK_LEN+1) *(OUTLINE_BLK_LEN+1));
		seamInfo.pBlkQNode = _GetBlkQNode(m_Rect.left + (c - m_RectBlk.left)* OUTLINE_BLK_LEN, m_Rect.top  + (r - m_RectBlk.top)* OUTLINE_BLK_LEN);
		_StitchBlk(seamInfo, pVerts, pIndices);

}

/**
 * \brief 
 * \param[in]
 * \param[out]
 * \return
 * \note
 * \warning the caller should set the common vert info(m_SeamVertIndexInfo) correctly.
 * \todo   
 * \author kuiwu 
 * \date 15/2/2006
 * \see 
 */
void CELOutlineQuadTree::_StitchBlk(const Seam_Block_Info& seamInfo, A3DLVERTEX * pVerts, WORD* pIndices)
{
	
	_StitchQNode(seamInfo.pBlkQNode, seamInfo, pVerts, pIndices);
}

/**
 * \brief 
 * \param[in]
 * \param[out]
 * \return
 * \note
 * \warning
 * \todo   
 * \author kuiwu 
 * \date 15/2/2006
 * \see 
 */
void CELOutlineQuadTree::_StitchQNode(CELOutlineQuadNode * pQNode, const Seam_Block_Info& seamInfo, A3DLVERTEX * pVerts, WORD * pIndices)
{

#define SEAM_TOP     ((pQNode->iz == seamInfo.pBlkQNode->iz)\
	&& seamInfo.edge[OLQ_EDGE_TOP].pVerts\
	&& (pQNode->iLen == seamInfo.edge[OLQ_EDGE_TOP].nInterval))
#define SEAM_LEFT     ((pQNode->ix == seamInfo.pBlkQNode->ix)\
	&& seamInfo.edge[OLQ_EDGE_LEFT].pVerts\
	&& (pQNode->iLen == seamInfo.edge[OLQ_EDGE_LEFT].nInterval))
#define SEAM_BOTTOM     ((pQNode->iz+pQNode->iLen == seamInfo.pBlkQNode->iz+seamInfo.pBlkQNode->iLen)\
	&& seamInfo.edge[OLQ_EDGE_BOTTOM].pVerts\
	&& (pQNode->iLen == seamInfo.edge[OLQ_EDGE_BOTTOM].nInterval))
#define SEAM_RIGHT     ((pQNode->ix+pQNode->iLen == seamInfo.pBlkQNode->ix+seamInfo.pBlkQNode->iLen)\
	&& seamInfo.edge[OLQ_EDGE_RIGHT].pVerts\
	&& (pQNode->iLen == seamInfo.edge[OLQ_EDGE_RIGHT].nInterval))

	if (pQNode->IsLeaf() || SEAM_TOP || SEAM_LEFT || SEAM_RIGHT || SEAM_BOTTOM)
	{ //add the node to the seam stream
		
		_AddSeam(pQNode, seamInfo, pVerts, pIndices);
		return;
	}
	assert(!pQNode->IsLeaf());
	for (int i = 0; i < 4; ++i)
	{
		_StitchQNode(&m_pQNodes[pQNode->iChildren[i]], seamInfo, pVerts, pIndices);
	}

}


/**
 * \brief 
 * \param[in]
 * \param[out]
 * \return
 * \note
 * \warning
 * \todo   
 * \author kuiwu 
 * \date 15/2/2006
 * \see 
 */
void CELOutlineQuadTree::_AddSeam(CELOutlineQuadNode * pQNode, const Seam_Block_Info& seamInfo, A3DLVERTEX * pVerts, WORD* pIndices)
{


	A3DVECTOR3 * pNodeEdgeVert[4][OUTLINE_BLK_LEN+4];

	//default as a non-border node, extract edge by its self
	_ExtractNodeEdge(pQNode, OLQ_EDGE_LEFT, pNodeEdgeVert[OLQ_EDGE_LEFT]);
	_ExtractNodeEdge(pQNode, OLQ_EDGE_TOP, pNodeEdgeVert[OLQ_EDGE_TOP]);
	_ExtractNodeEdge(pQNode, OLQ_EDGE_RIGHT, pNodeEdgeVert[OLQ_EDGE_RIGHT]);
	_ExtractNodeEdge(pQNode, OLQ_EDGE_BOTTOM, pNodeEdgeVert[OLQ_EDGE_BOTTOM]);


	if( seamInfo.edge[OLQ_EDGE_LEFT].pVerts && (pQNode->ix == seamInfo.pBlkQNode->ix))
	{///<  the left border node and the neighbor is from terrain
		WORD * pEdgeInx =   pQNode->pEdgeVert[OLQ_EDGE_LEFT];
		A3DVECTOR3 * pStart = &m_pVertPos[pEdgeInx[0]];
		A3DVECTOR3 * pEnd   = &m_pVertPos[pEdgeInx[pQNode->nEdgeVert[OLQ_EDGE_LEFT]-1]];
		assert(pStart->z > pEnd->z);
		int c = 0;
		for (int j = 0; j < seamInfo.edge[OLQ_EDGE_LEFT].nVerts; ++j)
		{
			if ((seamInfo.edge[OLQ_EDGE_LEFT].pVerts[j].z < pStart->z + 0.1f)
				&& (seamInfo.edge[OLQ_EDGE_LEFT].pVerts[j].z > pEnd->z - 0.1f))
			{
				pNodeEdgeVert[OLQ_EDGE_LEFT][c] = &seamInfo.edge[OLQ_EDGE_LEFT].pVerts[j];
				++c;
			}
		}
		pNodeEdgeVert[OLQ_EDGE_LEFT][c] = NULL;
	}	
	
	if (seamInfo.edge[OLQ_EDGE_TOP].pVerts && (pQNode->iz == seamInfo.pBlkQNode->iz) )
	{///< the top border node
		WORD * pEdgeInx =   pQNode->pEdgeVert[OLQ_EDGE_TOP];
		A3DVECTOR3 * pStart = &m_pVertPos[pEdgeInx[0]];
		A3DVECTOR3 * pEnd   = &m_pVertPos[pEdgeInx[pQNode->nEdgeVert[OLQ_EDGE_TOP]-1]];
		assert(pStart->x < pEnd->x);
		int c = 0;
		for (int j = 0; j < seamInfo.edge[OLQ_EDGE_TOP].nVerts; ++j)
		{
			if( (seamInfo.edge[OLQ_EDGE_TOP].pVerts[j].x > pStart->x - 0.1f)
			&& (seamInfo.edge[OLQ_EDGE_TOP].pVerts[j].x < pEnd->x + 0.1f))
			{
				pNodeEdgeVert[OLQ_EDGE_TOP][c] = &seamInfo.edge[OLQ_EDGE_TOP].pVerts[j];
				++c;
			}
		}
		pNodeEdgeVert[OLQ_EDGE_TOP][c] = NULL;
	}	

	if (seamInfo.edge[OLQ_EDGE_RIGHT].pVerts &&(pQNode->ix + pQNode->iLen == seamInfo.pBlkQNode->ix + seamInfo.pBlkQNode->iLen))
	{///< the right border node
		WORD * pEdgeInx =   pQNode->pEdgeVert[OLQ_EDGE_RIGHT];
		A3DVECTOR3 * pStart = &m_pVertPos[pEdgeInx[0]];
		A3DVECTOR3 * pEnd   = &m_pVertPos[pEdgeInx[pQNode->nEdgeVert[OLQ_EDGE_RIGHT]-1]];
		assert(pStart->z > pEnd->z);
		int c = 0;
		for (int j = 0; j < seamInfo.edge[OLQ_EDGE_RIGHT].nVerts; ++j)
		{
			if ((seamInfo.edge[OLQ_EDGE_RIGHT].pVerts[j].z < pStart->z + 0.1f)
			   && (seamInfo.edge[OLQ_EDGE_RIGHT].pVerts[j].z > pEnd->z - 0.1f))
			{
				pNodeEdgeVert[OLQ_EDGE_RIGHT][c] = &seamInfo.edge[OLQ_EDGE_RIGHT].pVerts[j];
				++c;
			}
		}
		pNodeEdgeVert[OLQ_EDGE_RIGHT][c] = NULL;
	}	
	
	if(seamInfo.edge[OLQ_EDGE_BOTTOM].pVerts &&(pQNode->iz + pQNode->iLen == seamInfo.pBlkQNode->iz + seamInfo.pBlkQNode->iLen))
	{///< the bottom border node
		WORD * pEdgeInx =   pQNode->pEdgeVert[OLQ_EDGE_BOTTOM];
		A3DVECTOR3 * pStart = &m_pVertPos[pEdgeInx[0]];
		A3DVECTOR3 * pEnd   = &m_pVertPos[pEdgeInx[pQNode->nEdgeVert[OLQ_EDGE_BOTTOM]-1]];
		assert(pStart->x < pEnd->x);
		int c = 0;
		for (int j = 0; j < seamInfo.edge[OLQ_EDGE_BOTTOM].nVerts; ++j)
		{
			if( (seamInfo.edge[OLQ_EDGE_BOTTOM].pVerts[j].x > pStart->x - 0.1f)
			&& (seamInfo.edge[OLQ_EDGE_BOTTOM].pVerts[j].x < pEnd->x + 0.1f))
			{
				pNodeEdgeVert[OLQ_EDGE_BOTTOM][c] = &seamInfo.edge[OLQ_EDGE_BOTTOM].pVerts[j];
				++c;
			}
		}
		pNodeEdgeVert[OLQ_EDGE_BOTTOM][c] = NULL;
	}	
	
	//set the render stream
	int v0, v1, v2;
	//center as v0
	v0 = _GetSeamVertIndex(pQNode->ix+ (pQNode->iLen>>1), pQNode->iz + (pQNode->iLen>>1),
						  seamInfo.pBlkQNode->ix, seamInfo.pBlkQNode->iz, m_pVertPos[pQNode->iCenterVert], pVerts);
	int j;
	int ix, iz;
	A3DVECTOR3 * pPos;
	float fLeft = pNodeEdgeVert[OLQ_EDGE_TOP][0]->x;
	float fTop = pNodeEdgeVert[OLQ_EDGE_TOP][0]->z;
	//top edge
	ix = pQNode->ix;
	iz = pQNode->iz;
	j = 0;
	pPos = pNodeEdgeVert[OLQ_EDGE_TOP][j];
	v1 = _GetSeamVertIndex(ix, iz, seamInfo.pBlkQNode->ix, seamInfo.pBlkQNode->iz, *pPos, pVerts);
	++j;
	pPos =pNodeEdgeVert[OLQ_EDGE_TOP][j];
	while (pPos != NULL)
	{
		ix = (int)((pPos->x - fLeft)/OUTLINE_TILE_GRID_LEN + 0.5);
		ix += pQNode->ix;
		v2= _GetSeamVertIndex(ix, iz, seamInfo.pBlkQNode->ix, seamInfo.pBlkQNode->iz, *pPos, pVerts);
		pIndices[m_nSeamFace*3] = v0;
		pIndices[m_nSeamFace*3+1] = v1;
		pIndices[m_nSeamFace*3+2] = v2;
#ifdef EL_OLQ_DEBUG
		assert(fabs(pVerts[v0].x - pVerts[v1].x) < 40 
			  && fabs(pVerts[v0].z - pVerts[v1].z) < 40  );
#endif
		m_nSeamFace++;
		v1 = v2;
		++j;
		pPos = pNodeEdgeVert[OLQ_EDGE_TOP][j];
	}
	//right edge
	ix = pQNode->ix + pQNode->iLen;
	iz = pQNode->iz;
	j = 0; 
	pPos = pNodeEdgeVert[OLQ_EDGE_RIGHT][j];
	v1 = _GetSeamVertIndex(ix, iz, seamInfo.pBlkQNode->ix, seamInfo.pBlkQNode->iz, *pPos, pVerts);
	++j;
	pPos = pNodeEdgeVert[OLQ_EDGE_RIGHT][j];
	while (pPos != NULL)
	{
		iz = (int)((fTop - pPos->z )/OUTLINE_TILE_GRID_LEN + 0.5);
		iz += pQNode->iz;
		v2= _GetSeamVertIndex(ix, iz, seamInfo.pBlkQNode->ix, seamInfo.pBlkQNode->iz, *pPos, pVerts);
		pIndices[m_nSeamFace*3] = v0;
		pIndices[m_nSeamFace*3+1] = v1;
		pIndices[m_nSeamFace*3+2] = v2;
#ifdef EL_OLQ_DEBUG
		assert(fabs(pVerts[v0].x - pVerts[v1].x) < 40 
			  && fabs(pVerts[v0].z - pVerts[v1].z) < 40  );
#endif
		m_nSeamFace++;
		v1 = v2;
		++j;
		pPos = pNodeEdgeVert[OLQ_EDGE_RIGHT][j];
	}

	//bottom edge
	ix = pQNode->ix;
	iz = pQNode->iz + pQNode->iLen;
	j = 0;
	pPos = pNodeEdgeVert[OLQ_EDGE_BOTTOM][j];
	v1 = _GetSeamVertIndex(ix, iz, seamInfo.pBlkQNode->ix, seamInfo.pBlkQNode->iz, *pPos, pVerts);
	++j;
	pPos = pNodeEdgeVert[OLQ_EDGE_BOTTOM][j];
	while (pPos != NULL)
	{
		ix = (int)((pPos->x - fLeft)/OUTLINE_TILE_GRID_LEN + 0.5);
		ix += pQNode->ix;
		v2= _GetSeamVertIndex(ix, iz, seamInfo.pBlkQNode->ix, seamInfo.pBlkQNode->iz, *pPos, pVerts);
		pIndices[m_nSeamFace*3] = v0;
		pIndices[m_nSeamFace*3+1] = v2;
		pIndices[m_nSeamFace*3+2] = v1;
#ifdef EL_OLQ_DEBUG
		assert(fabs(pVerts[v0].x - pVerts[v1].x) < 40 
			  && fabs(pVerts[v0].z - pVerts[v1].z) < 40  );
#endif
		m_nSeamFace++;
		v1 = v2;
		++j;
		pPos = pNodeEdgeVert[OLQ_EDGE_BOTTOM][j];
	}
	//left edge
	ix = pQNode->ix;
	iz = pQNode->iz;
	j = 0;
	pPos = pNodeEdgeVert[OLQ_EDGE_LEFT][j];
	v1 = _GetSeamVertIndex(ix, iz, seamInfo.pBlkQNode->ix, seamInfo.pBlkQNode->iz, *pPos, pVerts);
	++j;
	pPos = pNodeEdgeVert[OLQ_EDGE_LEFT][j];
	while (pPos != NULL)
	{
		iz = (int)((fTop - pPos->z )/OUTLINE_TILE_GRID_LEN + 0.5);
		iz += pQNode->iz;
		v2= _GetSeamVertIndex(ix, iz, seamInfo.pBlkQNode->ix, seamInfo.pBlkQNode->iz, *pPos, pVerts);
		pIndices[m_nSeamFace*3] = v0;
		pIndices[m_nSeamFace*3+1] = v2;
		pIndices[m_nSeamFace*3+2] = v1;
#ifdef EL_OLQ_DEBUG
		assert(fabs(pVerts[v0].x - pVerts[v1].x) < 40 
			  && fabs(pVerts[v0].z - pVerts[v1].z) < 40  );
#endif
		m_nSeamFace++;
		v1 = v2;
		++j;
		pPos = pNodeEdgeVert[OLQ_EDGE_LEFT][j];
	}

	
}

/**
 * \brief 
 * \param[in]
 * \param[out]
 * \return
 * \note
 * \warning
 * \todo   
 * \author kuiwu 
 * \date 15/2/2006
 * \see 
 */
void CELOutlineQuadTree::_StitchTop2(int tblr, int iStart, int iEnd, A3DTerrain2::ACTBLOCKS *pActBlocks, A3DTerrain2LOD::GRID * pTerGrid, A3DLVERTEX*pVerts, WORD*pIndices)
{
	int tlGrid = tblr * OUTLINE_BLK_LEN;
	int iCurGrid = iStart * OUTLINE_BLK_LEN;
	A3DTerrain2Block * pBlock;
	A3DVECTOR3 edgeVert[OUTLINE_BLK_LEN+4];
	VERT_INDEX_INFO commonVertIndex[OUTLINE_BLK_LEN+1];
	Seam_Block_Info seamInfo;
	
	//clear the common vert info
	memset(m_SeamVertIndexInfo, -1, sizeof(VERT_INDEX_INFO) * (OUTLINE_BLK_LEN+1) *(OUTLINE_BLK_LEN+1));

	for (int i = iStart; i < iEnd; i++) 
	{
		seamInfo.pBlkQNode = _GetBlkQNode(iCurGrid, tlGrid);
		pBlock = pActBlocks->GetBlock(tblr+1, i, false);  //neighbor
		if (pBlock != NULL) 
		{
			seamInfo.edge[OLQ_EDGE_BOTTOM].nInterval = _ExtractBlkEdge(pBlock, pTerGrid, OLQ_EDGE_TOP, 
												seamInfo.edge[OLQ_EDGE_BOTTOM].nVerts, edgeVert);
			seamInfo.edge[OLQ_EDGE_BOTTOM].pVerts = edgeVert;
		}
		else
		{
			seamInfo.edge[OLQ_EDGE_BOTTOM].pVerts = NULL;
		}
		seamInfo.edge[OLQ_EDGE_TOP].pVerts = NULL;
		seamInfo.edge[OLQ_EDGE_LEFT].pVerts = NULL;
		seamInfo.edge[OLQ_EDGE_RIGHT].pVerts = NULL;
		_StitchBlk(seamInfo, pVerts, pIndices);
		//save common edge
		int j;
		for (j = 0; j < OUTLINE_BLK_LEN+1; j++)
		{
			commonVertIndex[j].ix = m_SeamVertIndexInfo[j][OUTLINE_BLK_LEN].ix;
			commonVertIndex[j].iz = m_SeamVertIndexInfo[j][OUTLINE_BLK_LEN].iz;
			commonVertIndex[j].index = m_SeamVertIndexInfo[j][OUTLINE_BLK_LEN].index;
		}
		//clear vert index info
		memset(m_SeamVertIndexInfo, -1, sizeof(VERT_INDEX_INFO) * (OUTLINE_BLK_LEN+1) *(OUTLINE_BLK_LEN+1));
		//set back
		for (j = 0; j < OUTLINE_BLK_LEN+1; j++) 
		{
			m_SeamVertIndexInfo[j][0].ix = commonVertIndex[j].ix;
			m_SeamVertIndexInfo[j][0].iz = commonVertIndex[j].iz;
			m_SeamVertIndexInfo[j][0].index = commonVertIndex[j].index;
		}
		iCurGrid += OUTLINE_BLK_LEN;
	}
}

/**
 * \brief 
 * \param[in]
 * \param[out]
 * \return
 * \note
 * \warning
 * \todo   
 * \author kuiwu 
 * \date 15/2/2006
 * \see 
 */
void CELOutlineQuadTree::_StitchBottom2(int tblr, int iStart, int iEnd, A3DTerrain2::ACTBLOCKS *pActBlocks, A3DTerrain2LOD::GRID * pTerGrid, A3DLVERTEX*pVerts, WORD*pIndices)
{
	int tlGrid = tblr * OUTLINE_BLK_LEN;
	int iCurGrid = iStart * OUTLINE_BLK_LEN;
	A3DTerrain2Block * pBlock;
	A3DVECTOR3 edgeVert[OUTLINE_BLK_LEN+4];
	VERT_INDEX_INFO commonVertIndex[OUTLINE_BLK_LEN+1];
	Seam_Block_Info seamInfo;

	
	//clear the common vert info
	memset(m_SeamVertIndexInfo, -1, sizeof(VERT_INDEX_INFO) * (OUTLINE_BLK_LEN+1) *(OUTLINE_BLK_LEN+1));

	for (int i = iStart; i < iEnd; i++)
	{
		seamInfo.pBlkQNode = _GetBlkQNode(iCurGrid, tlGrid);
		pBlock = pActBlocks->GetBlock(tblr-1, i, false);
		if (pBlock != NULL) 
		{
			seamInfo.edge[OLQ_EDGE_TOP].nInterval = _ExtractBlkEdge(pBlock, pTerGrid, OLQ_EDGE_BOTTOM, 
									seamInfo.edge[OLQ_EDGE_TOP].nVerts, edgeVert);
			seamInfo.edge[OLQ_EDGE_TOP].pVerts = edgeVert;

		}
		else
		{ //from the olm
			seamInfo.edge[OLQ_EDGE_TOP].pVerts = NULL;
		}

		seamInfo.edge[OLQ_EDGE_BOTTOM].pVerts = NULL;
		seamInfo.edge[OLQ_EDGE_RIGHT].pVerts = NULL;
		seamInfo.edge[OLQ_EDGE_LEFT].pVerts = NULL;
		_StitchBlk(seamInfo, pVerts, pIndices);
		
		//save common edge
		int j;
		for (j = 0; j < OUTLINE_BLK_LEN+1; j++) 
		{
			commonVertIndex[j].ix = m_SeamVertIndexInfo[j][OUTLINE_BLK_LEN].ix;
			commonVertIndex[j].iz = m_SeamVertIndexInfo[j][OUTLINE_BLK_LEN].iz;
			commonVertIndex[j].index = m_SeamVertIndexInfo[j][OUTLINE_BLK_LEN].index;
		}
		//clear vert index info
		memset(m_SeamVertIndexInfo, -1, sizeof(VERT_INDEX_INFO) * (OUTLINE_BLK_LEN+1) *(OUTLINE_BLK_LEN+1));
		//set back
		for (j = 0; j < OUTLINE_BLK_LEN+1; j++) 
		{
			m_SeamVertIndexInfo[j][0].ix = commonVertIndex[j].ix;
			m_SeamVertIndexInfo[j][0].iz = commonVertIndex[j].iz;
			m_SeamVertIndexInfo[j][0].index = commonVertIndex[j].index;
		}
		iCurGrid += OUTLINE_BLK_LEN;
	}
}

/**
 * \brief 
 * \param[in]
 * \param[out]
 * \return
 * \note
 * \warning
 * \todo   
 * \author kuiwu 
 * \date 15/2/2006
 * \see 
 */
void CELOutlineQuadTree::_StitchLeft2(int tblr, int iStart, int iEnd, A3DTerrain2::ACTBLOCKS *pActBlocks, A3DTerrain2LOD::GRID * pTerGrid, A3DLVERTEX*pVerts, WORD*pIndices)
{
	int tlGrid = tblr * OUTLINE_BLK_LEN;
	int iCurGrid = iStart * OUTLINE_BLK_LEN;
	A3DTerrain2Block * pBlock;
	A3DVECTOR3 edgeVert[OUTLINE_BLK_LEN+4];
	VERT_INDEX_INFO commonVertIndex[OUTLINE_BLK_LEN+1];
	Seam_Block_Info seamInfo;
	
	//clear the common vert info
	memset(m_SeamVertIndexInfo, -1, sizeof(VERT_INDEX_INFO) * (OUTLINE_BLK_LEN+1) *(OUTLINE_BLK_LEN+1));

	for (int i = iStart; i < iEnd; i++) 
	{
		seamInfo.pBlkQNode = _GetBlkQNode(tlGrid, iCurGrid);
		pBlock = pActBlocks->GetBlock(i, tblr+1, false);
		if (pBlock != NULL) 
		{
			seamInfo.edge[OLQ_EDGE_RIGHT].nInterval = _ExtractBlkEdge(pBlock, pTerGrid, 
				OLQ_EDGE_LEFT, seamInfo.edge[OLQ_EDGE_RIGHT].nVerts, edgeVert);
			seamInfo.edge[OLQ_EDGE_RIGHT].pVerts = edgeVert;
		}
		else
		{ //from the olm
		    seamInfo.edge[OLQ_EDGE_RIGHT].pVerts = NULL;
		}
		seamInfo.edge[OLQ_EDGE_BOTTOM].pVerts = NULL;
		seamInfo.edge[OLQ_EDGE_TOP].pVerts = NULL;
		seamInfo.edge[OLQ_EDGE_LEFT].pVerts = NULL;
		_StitchBlk(seamInfo, pVerts, pIndices);
		//save common edge
		int j;
		for (j = 0; j < OUTLINE_BLK_LEN+1; j++) 
		{
			commonVertIndex[j].ix = m_SeamVertIndexInfo[OUTLINE_BLK_LEN][j].ix;
			commonVertIndex[j].iz = m_SeamVertIndexInfo[OUTLINE_BLK_LEN][j].iz;
			commonVertIndex[j].index = m_SeamVertIndexInfo[OUTLINE_BLK_LEN][j].index;
		}
		//clear vert index info
		memset(m_SeamVertIndexInfo, -1, sizeof(VERT_INDEX_INFO) * (OUTLINE_BLK_LEN+1) *(OUTLINE_BLK_LEN+1));
		//set back
		for (j = 0; j < OUTLINE_BLK_LEN+1; j++) 
		{
			m_SeamVertIndexInfo[0][j].ix = commonVertIndex[j].ix;
			m_SeamVertIndexInfo[0][j].iz = commonVertIndex[j].iz;
			m_SeamVertIndexInfo[0][j].index = commonVertIndex[j].index;
		}
		iCurGrid += OUTLINE_BLK_LEN;
	}
}

/**
 * \brief 
 * \param[in]
 * \param[out]
 * \return
 * \note
 * \warning
 * \todo   
 * \author kuiwu 
 * \date 15/2/2006
 * \see 
 */
void CELOutlineQuadTree::_StitchRight2(int tblr, int iStart, int iEnd, A3DTerrain2::ACTBLOCKS *pActBlocks, A3DTerrain2LOD::GRID * pTerGrid, A3DLVERTEX*pVerts, WORD*pIndices)
{
	int tlGrid = tblr * OUTLINE_BLK_LEN;
	int iCurGrid = iStart * OUTLINE_BLK_LEN;
	A3DTerrain2Block * pBlock;
	A3DVECTOR3 edgeVert[OUTLINE_BLK_LEN+4];
	VERT_INDEX_INFO commonVertIndex[OUTLINE_BLK_LEN+1];
	Seam_Block_Info seamInfo;
	
	//clear the common vert info
	memset(m_SeamVertIndexInfo, -1, sizeof(VERT_INDEX_INFO) * (OUTLINE_BLK_LEN+1) *(OUTLINE_BLK_LEN+1));

	for (int i = iStart; i < iEnd; i++) 
	{
		seamInfo.pBlkQNode = _GetBlkQNode(tlGrid, iCurGrid);
		pBlock = pActBlocks->GetBlock(i, tblr-1, false);
		if (pBlock != NULL) 
		{
			seamInfo.edge[OLQ_EDGE_LEFT].nInterval = _ExtractBlkEdge(pBlock, pTerGrid, 
				OLQ_EDGE_RIGHT, seamInfo.edge[OLQ_EDGE_LEFT].nVerts, edgeVert);
			seamInfo.edge[OLQ_EDGE_LEFT].pVerts = edgeVert;
		}
		else
		{ //from the olm
			seamInfo.edge[OLQ_EDGE_LEFT].pVerts = NULL;
		}	
		seamInfo.edge[OLQ_EDGE_RIGHT].pVerts = NULL;
		seamInfo.edge[OLQ_EDGE_TOP].pVerts = NULL;
		seamInfo.edge[OLQ_EDGE_BOTTOM].pVerts = NULL;
		_StitchBlk(seamInfo, pVerts, pIndices);
		//save common edge
		int j;
		for (j = 0; j < OUTLINE_BLK_LEN+1; j++) 
		{
			commonVertIndex[j].ix = m_SeamVertIndexInfo[OUTLINE_BLK_LEN][j].ix;
			commonVertIndex[j].iz = m_SeamVertIndexInfo[OUTLINE_BLK_LEN][j].iz;
			commonVertIndex[j].index = m_SeamVertIndexInfo[OUTLINE_BLK_LEN][j].index;
		}
		//clear vert index info
		memset(m_SeamVertIndexInfo, -1, sizeof(VERT_INDEX_INFO) * (OUTLINE_BLK_LEN+1) *(OUTLINE_BLK_LEN+1));
		//set back
		for (j = 0; j < OUTLINE_BLK_LEN+1; j++) 
		{
			m_SeamVertIndexInfo[0][j].ix = commonVertIndex[j].ix;
			m_SeamVertIndexInfo[0][j].iz = commonVertIndex[j].iz;
			m_SeamVertIndexInfo[0][j].index = commonVertIndex[j].index;
		}
		iCurGrid += OUTLINE_BLK_LEN;
		
	}
}

/**
 * \brief 
 * \param[in]
 * \param[out]
 * \return
 * \note
 * \warning
 * \todo   
 * \author kuiwu 
 * \date 15/2/2006
 * \see 
 */
void CELOutlineQuadTree::_Sticth2(A3DTerrain2::ACTBLOCKS *pActBlocks, const ARectI& rcVisible, A3DTerrain2LOD::GRID * pTerGrid, A3DLVERTEX*pVerts, WORD*pIndices)
{
	ARectI rcSeamBlk(rcVisible);
	rcSeamBlk.Inflate(1,1);
//   overlapping has been already checked.  By kuiwu. [14/2/2006]
//	if (!CELOutlineTextureCache::RcOverlapRc(rcSeamBlk, m_RectBlk)) {
//		return;
//	}

	//top edge
	if (rcSeamBlk.top >= m_RectBlk.top) 
	{
		int iStart, iEnd;
		iStart = max(m_RectBlk.left, rcSeamBlk.left+1);
		iEnd = min(m_RectBlk.right, rcSeamBlk.right-1);
		if (iStart < iEnd)
		{
			_StitchTop2(rcSeamBlk.top,  iStart, iEnd, pActBlocks, pTerGrid, pVerts, pIndices);
		}
	}
	//right edge
	if (rcSeamBlk.right <= m_RectBlk.right) 
	{
		int iStart, iEnd;
		iStart = max(m_RectBlk.top, rcSeamBlk.top+1);
		iEnd = min(m_RectBlk.bottom, rcSeamBlk.bottom-1);
		if (iStart < iEnd)
		{
			_StitchRight2(rcSeamBlk.right-1, iStart, iEnd, pActBlocks, pTerGrid, pVerts, pIndices);
		}
	}
	//bottom edge
	if (rcSeamBlk.bottom <= m_RectBlk.bottom) 
	{
		int iStart, iEnd;
		iStart = max(m_RectBlk.left, rcSeamBlk.left+1);
		iEnd = min(m_RectBlk.right, rcSeamBlk.right-1);
		if (iStart < iEnd)
		{
			_StitchBottom2(rcSeamBlk.bottom-1, iStart, iEnd, pActBlocks, pTerGrid, pVerts, pIndices );
		}
	}
	//left edge
	if (rcSeamBlk.left >= m_RectBlk.left) 
	{
		int iStart, iEnd;
		iStart = max(m_RectBlk.top, rcSeamBlk.top+1);
		iEnd = min(m_RectBlk.bottom, rcSeamBlk.bottom-1);
		if (iStart < iEnd)
		{
			_StitchLeft2(rcSeamBlk.left, iStart, iEnd, pActBlocks, pTerGrid, pVerts, pIndices);
		}
	}
}
