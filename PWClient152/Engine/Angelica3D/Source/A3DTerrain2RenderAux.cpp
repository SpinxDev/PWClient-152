/*
 * FILE: A3DTerrain2RenderAux.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2010/4/30
 *
 * HISTORY: 
 *
 * Copyright (c) 2010 Archosaur Studio, All Rights Reserved.
 */

#include "A3DTerrain2Render.h"
#include "A3DPI.h"
#include "A3DTerrain2.h"
#include "A3DStream.h"
#include "A3DMacros.h"
#include "A3DTerrain2Blk.h"
#include "A3DTerrain2LOD.h"
#include "A3DDevice.h"
#include "A3DEngine.h"
#include "A3DFuncs.h"

#define new A_DEBUG_NEW

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

#define _CACHE_TIME_	300000

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

//	Sort block by render order
static int _BlockRenderCompare(const void *arg1, const void *arg2)
{
	A3DTrn2RenderSlot::BLOCK_ITEM* pItem1 = *(A3DTrn2RenderSlot::BLOCK_ITEM**)arg1;
	A3DTrn2RenderSlot::BLOCK_ITEM* pItem2 = *(A3DTrn2RenderSlot::BLOCK_ITEM**)arg2;
	return pItem1->iRdOrder - pItem2->iRdOrder;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement A3DTrn2RenderSlot
//	
///////////////////////////////////////////////////////////////////////////

A3DTrn2RenderSlot::A3DTrn2RenderSlot(A3DTerrain2Render* pTrnRender) :
m_pTrnRender(pTrnRender),
m_pVertStream(NULL),
m_pIdxStream(NULL),
m_iNumVert(0),
m_iNumIndex(0),
m_iMaskIdx(-1),
m_iDistance(0),
m_iLayerNum(0)
{
	memset(m_aBlocks, 0, sizeof (m_aBlocks));
}

A3DTrn2RenderSlot::~A3DTrn2RenderSlot()
{
}

//	Reset slot for reusing
void A3DTrn2RenderSlot::Reset(int iMaskIdx)
{
	m_iMaskIdx = iMaskIdx;

	A3DTerrain2Mask* pMaskArea = m_pTrnRender->m_pTerrain->GetMaskArea(iMaskIdx);
	ASSERT(pMaskArea);
	m_iLayerNum = pMaskArea->GetLayerNum();

	memset(m_aBlocks, 0, sizeof (m_aBlocks));
}

//	Register a terrain block which will be rendered.
//	iRdOrder: render order, more small, more early render
bool A3DTrn2RenderSlot::RegisterBlock(A3DTerrain2Block* pBlock, int iRdOrder)
{
	A3DTerrain2* pTrn = m_pTrnRender->m_pTerrain;
	int iMaskBlock = pTrn->GetMaskGrid() / pTrn->GetBlockGrid();

	int row = pBlock->GetRowInMask();
	int col = pBlock->GetColInMask();
	int iIndex = row * iMaskBlock + col;
	ASSERT(iIndex <= MAX_BLOCK_NUM);
	ASSERT(!m_aBlocks[iIndex].pBlock);		//	Don't register repeatedly

	m_aBlocks[iIndex].pBlock = pBlock;
	m_aBlocks[iIndex].iRdOrder = iRdOrder;

	return true;
}

//	Prepare blocks' render info
void A3DTrn2RenderSlot::PrepareRenderInfo()
{
	int i;
    DWORD dwLayerMask = 0;
	for (i = 0; i < MAX_BLOCK_NUM; i++)
	{
		A3DTerrain2Block* pBlock = m_aBlocks[i].pBlock;
		if (pBlock)
		{
			pBlock->BuildRenderData();
			m_aBlocks[i].dwLODFlags = pBlock->GetLODFlags();
            dwLayerMask |= pBlock->GetLayerFlags();
		}
		else
			m_aBlocks[i].dwLODFlags = 0;
	}

    if (m_iLayerNum > 6)  // 如果超过6层，实际上是无法正确渲染的，取前6层保持一致性
    {
        m_nUsedLayerNum = 6;
        for (i = 0; i < 6; i++)
        {
            m_iUsedLayerIndices[i] = i;
        }
    }
    else
    {
        m_nUsedLayerNum = 0;
        for (i = 0; i < m_iLayerNum; i++)
        {
            if (dwLayerMask & (1 << i))
            {
                m_iUsedLayerIndices[m_nUsedLayerNum] = i;
                m_nUsedLayerNum++;
            }
        }
    }
}

//	Transfer block streams to another
bool A3DTrn2RenderSlot::TransferStreams(A3DTrn2RenderSlot* pSlot)
{
	ASSERT(m_iMaskIdx == pSlot->m_iMaskIdx);

	int i;
	bool bBlockChanged = false;

	for (i=0; i < MAX_BLOCK_NUM; i++)
	{
		BLOCK_ITEM& src_block = m_aBlocks[i];
		BLOCK_ITEM& dst_block = pSlot->m_aBlocks[i];

		if ((src_block.dwLODFlags && !dst_block.pBlock) ||
			(!src_block.dwLODFlags && dst_block.pBlock))
		{
			bBlockChanged = true;
			break;
		}
		else if (dst_block.pBlock && src_block.dwLODFlags != dst_block.pBlock->GetLODFlags())
		{
			bBlockChanged = true;
			break;
		}
	}

	//	Transform stream to destination slot if no block mesh changes
	if (!bBlockChanged)
	{
		pSlot->m_pVertStream = m_pVertStream;
		pSlot->m_pIdxStream = m_pIdxStream;
		pSlot->m_iNumVert = m_iNumVert;
		pSlot->m_iNumIndex = m_iNumIndex;
		m_pVertStream = NULL;
		m_pIdxStream = NULL;
		m_iNumVert = 0;
		m_iNumIndex = 0;
	}

	return true;
}

//	Create streams for blocks
void A3DTrn2RenderSlot::CreateStreams()
{
	if (m_pVertStream && m_pIdxStream)
		return;

	A3DTerrain2Block::RENDERDATA* aRenderData[MAX_BLOCK_NUM];
	int i, j, iVertCnt=0, iIndexCnt=0, iRdDataCnt=0;

	for (i=0; i < MAX_BLOCK_NUM; i++)
	{
		A3DTerrain2Block* pBlock = m_aBlocks[i].pBlock;
		if (!pBlock)
			continue;
        
		A3DTerrain2Block::RENDERDATA* pRenderData = (A3DTerrain2Block::RENDERDATA*)pBlock->GetRenderData();
		iVertCnt += pRenderData->iNumVert;
		iIndexCnt += pRenderData->iNumIndex;

		aRenderData[iRdDataCnt++] = pRenderData;
	}

	m_iNumVert = iVertCnt;
	m_iNumIndex = iIndexCnt;

	//	Create stream
	A3DTrn2StreamMan* pStreamMan = m_pTrnRender->GetStreamMan();

	m_pVertStream = pStreamMan->AllocVertStream(m_iNumVert);
	m_pIdxStream = pStreamMan->AllocIndexStream(m_iNumIndex);
	if (!m_pVertStream || !m_pIdxStream)
		return;

	ASSERT(m_iNumVert <= m_pVertStream->GetVertCount());
	ASSERT(m_iNumIndex <= m_pIdxStream->GetIndexCount());

	int iVertSize;
	if (m_pTrnRender->m_pTerrain->UseLightmapTech())
		iVertSize = sizeof (A3DTRN2VERTEX1);
	else
		iVertSize = sizeof (A3DTRN2VERTEX2);

	BYTE* pVerts = NULL;
	WORD* pIndices = NULL;
	int sizeToLockVert = m_iNumVert * iVertSize;
	int sizeToLockIndex = m_iNumIndex * sizeof (WORD);

	//	Lock vertex buffer
	if (!m_pVertStream->LockVertexBufferDynamic(0, sizeToLockVert, (BYTE**)&pVerts, 0))
	{
		g_A3DErrLog.Log("A3DTrn2RenderSlot::CreateStreams, Failed to lock vertex buffer");
		return;
	}

	iVertCnt	= 0;

	//	Fill vertex buffer
	for (i=0; i < iRdDataCnt; i++)
	{
		A3DTerrain2Block::RENDERDATA* pRenderData = aRenderData[i];
		memcpy(pVerts+iVertCnt*iVertSize, pRenderData->aVertices, pRenderData->iNumVert * iVertSize);
		iVertCnt += pRenderData->iNumVert;
	}

	m_pVertStream->UnlockVertexBufferDynamic();

    //	Lock index buffer
    if (!m_pIdxStream->LockIndexBufferDynamic(0, sizeToLockIndex, (BYTE**)&pIndices, 0))
    {
        g_A3DErrLog.Log("A3DTrn2RenderSlot::CreateStreams, Failed to lock index buffer");
        return;
    }

    iIndexCnt = 0;
    iVertCnt = 0;
    //	Fill index buffer
    for (i=0; i < iRdDataCnt; i++)
    {
        A3DTerrain2Block::RENDERDATA* pRenderData = aRenderData[i];
        for (j=0; j < 5; j++)
        {
            WORD* aSrcIndices = pRenderData->aIndices[j];
            for (int n=0; n < pRenderData->aIdxNum[j]; n++)
                pIndices[iIndexCnt++] = aSrcIndices[n] + iVertCnt;
        }
        iVertCnt += pRenderData->iNumVert;
    }

	m_pIdxStream->UnlockIndexBufferDynamic();

	ASSERT(iVertCnt <= m_iNumVert);
	ASSERT(iIndexCnt <= m_iNumIndex);
}

//	Release all block streams
void A3DTrn2RenderSlot::ReleaseStreams()
{
	A3DTrn2StreamMan* pStreamMan = m_pTrnRender->GetStreamMan();

	if (m_pVertStream)
	{
		pStreamMan->FreeVertStream(m_pVertStream);
		m_pVertStream = NULL;
		m_iNumVert = 0;
	}

	if (m_pIdxStream)
	{
		pStreamMan->FreeIndexStream(m_pIdxStream);
		m_pIdxStream = NULL;
		m_iNumIndex = 0;
	}
}

//	Draw all blocks
void A3DTrn2RenderSlot::DrawAllBlocks(bool bUseFVF)
{
	if (!m_pVertStream || !m_pIdxStream)
		return;

	A3DDevice* pA3DDevice = m_pTrnRender->m_pA3DDevice;
	if (!m_pVertStream->AppearVertexOnly(0, bUseFVF) || !m_pIdxStream->AppearIndexOnly())
	{
		ASSERT( FALSE && "Failed to apply vertex buffer | index buffer to device");
		return;
	}

	ASSERT(m_iNumVert > 0 && m_iNumVert <= m_pVertStream->GetVertCount());
	ASSERT(m_iNumIndex > 0 && m_iNumIndex <= m_pIdxStream->GetIndexCount());

	pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, m_iNumVert, 0, m_iNumIndex / 3);
}

//	Clear block object pointers
void A3DTrn2RenderSlot::ClearBlockPtrs()
{
	for (int i=0; i < MAX_BLOCK_NUM; i++)
		m_aBlocks[i].pBlock = NULL;
}

//	Calculate distance from eye to mask area
void A3DTrn2RenderSlot::CalcSortDist(const A3DVECTOR3& vEyePos)
{
	A3DTerrain2Mask* pMaskArea = m_pTrnRender->m_pTerrain->GetMaskArea(m_iMaskIdx);
	ASSERT(pMaskArea);

	const ARectF& rcArea = pMaskArea->GetMaskArea();
	if (vEyePos.x >= rcArea.left && vEyePos.x < rcArea.right &&
		vEyePos.z >= rcArea.bottom && vEyePos.z < rcArea.top)
	{
		//	Eye is in this mask area
		m_iDistance = 0;
	}
	else
	{
		A3DVECTOR3 vCenter(0.0f);
		vCenter.x = (rcArea.left + rcArea.right) * 0.5f;
		vCenter.z = (rcArea.top + rcArea.bottom) * 0.5f;
		m_iDistance = (int)a3d_MagnitudeH(vEyePos - vCenter);
	}
}

///////////////////////////////////////////////////////////////////////////
//	
//	Class A3DTrn2StreamMan::INDEX_STREAM
//	
///////////////////////////////////////////////////////////////////////////

A3DTrn2StreamMan::INDEX_STREAM::INDEX_STREAM()
{
	memset(aStreams, 0, sizeof (aStreams));
}

///////////////////////////////////////////////////////////////////////////
//	
//	Class A3DTrn2StreamMan
//	
///////////////////////////////////////////////////////////////////////////

A3DTrn2StreamMan::A3DTrn2StreamMan() :
m_pTrn(NULL),
m_pTrnRender(NULL),
m_dwIdxStrmCnt(0),
m_dwIdxStrmSize(0),
m_dwVertStrmCnt(0),
m_dwVertStrmSize(0)
{
	memset(m_aIdxStrms, 0, sizeof (m_aIdxStrms));
}

A3DTrn2StreamMan::~A3DTrn2StreamMan()
{
}

//	Initlaize object
bool A3DTrn2StreamMan::Init(A3DTerrain2Render* pTrnRender)
{
	m_pTrnRender = pTrnRender;
	m_pTrn = pTrnRender->m_pTerrain;

	return true;
}

//	Release object
void A3DTrn2StreamMan::Release()
{
	int i, j;

	//	Release index streams
	for (i=0; i < NUM_LOD_LEVEL; i++)
	{
		INDEX_STREAM& stream = m_aIdxStrms[i];

		for (j=0; j < MAX_INDEX_STREAM; j++)
		{
			A3DRELEASE(stream.aStreams[j]);
		}
	}

	//	Release all dynamic vertex streams
	if (m_FreeVSList.GetCount())
	{
		ALISTPOSITION pos = m_FreeVSList.GetHeadPosition();
		while (pos)
		{
			STREAM_NODE& node = m_FreeVSList.GetNext(pos);
			A3DRELEASE(node.pStream);
		}

		m_FreeVSList.RemoveAll();
	}

	//	Release all dyanmic index streams
	if (m_FreeISList.GetCount())
	{
		ALISTPOSITION pos = m_FreeISList.GetHeadPosition();
		while (pos)
		{
			STREAM_NODE& node = m_FreeISList.GetNext(pos);
			A3DRELEASE(node.pStream);
		}

		m_FreeISList.RemoveAll();
	}
}

int SnapSize(int nNeedSize, int nMax)
{
	int nAllocSize = 1;
	while(nAllocSize < nNeedSize)
	{
		nAllocSize *= 2;
	}
	return a_Min(nAllocSize, nMax);
}

//	Allocate a dynamic vertex stream
A3DStream* A3DTrn2StreamMan::AllocVertStream(int iNumVert)
{
	//	Snap vertex number
	const int iBlockVertCnt = (m_pTrn->GetBlockGrid() + 1) * (m_pTrn->GetBlockGrid() + 1);

	//Edit by PanYupin
	int iNeedVertexNum = iNumVert;
	iNumVert = SnapSize(iNumVert, iBlockVertCnt * A3DTrn2RenderSlot::MAX_BLOCK_NUM);

	A3DStream* pA3DStream = NULL;

	//	Try to pick a free stream from cache
	if (m_FreeVSList.GetCount())
	{
		ALISTPOSITION pos = m_FreeVSList.GetHeadPosition();
		while (pos)
		{
			ALISTPOSITION posCur = pos;
			STREAM_NODE& node = m_FreeVSList.GetNext(pos);
			if (node.pStream->GetVertCount() == iNumVert)
			{
				pA3DStream = node.pStream;
				m_FreeVSList.RemoveAt(posCur);
				break;
			}
		}
	}

	if (!pA3DStream)
	{
		//g_A3DErrLog.Log("A3DTrn2StreamMan::AllocVertStream, Need: %d, Max: %d", iNeedVertexNum, iNumVert);
		//	Create a new stream
		int iVertSize, iFVF;
		if (m_pTrnRender->m_pTerrain->UseLightmapTech())
		{
			iVertSize = sizeof (A3DTRN2VERTEX1);
			iFVF = A3DTRN2VERT1_FVF;
		}
		else
		{
			iVertSize = sizeof (A3DTRN2VERTEX2);
			iFVF = A3DTRN2VERT2_FVF;
		}

		pA3DStream = new A3DStream;
		if (!pA3DStream || !pA3DStream->Init(m_pTrnRender->m_pA3DDevice, iVertSize, iFVF, iNumVert, 0, A3DSTRM_REFERENCEPTR, 0))
		{
			g_A3DErrLog.Log("A3DTrn2StreamMan::AllocVertStream, Failed to create vertex stream [%d]!", iNumVert);
			return false;
		}

		m_dwVertStrmCnt++;
		m_dwVertStrmSize += iNumVert * iVertSize;
	}
	return pA3DStream;
}

//	Free a vertex stream
void A3DTrn2StreamMan::FreeVertStream(A3DStream* pA3DStream)
{
	if (!pA3DStream)
		return;

	//	For a vertex stream, it's index counter should be 0 !
	ASSERT(!pA3DStream->GetIndexCount());

	DWORD dwCurTime = a_GetTime();

	//	Add stream to free stream cache
	STREAM_NODE node;
	node.pStream = pA3DStream;
	node.dwTimeStamp = dwCurTime;
	m_FreeVSList.AddHead(node);

	//	Check if there is long-time not used stream in cache. If there is, 
	//	remove it from free cache. Because new free stream is always added
	//	to list head, the one at tail must be the longest not used one
	STREAM_NODE& node_last = m_FreeVSList.GetTail();
	if (dwCurTime >= node_last.dwTimeStamp + _CACHE_TIME_)
	{
		ASSERT(node_last.pStream != pA3DStream);
		m_dwVertStrmCnt--;
		m_dwVertStrmSize -= node_last.pStream->GetVertCount() * node_last.pStream->GetVertexSize();
		A3DRELEASE(node_last.pStream);
		m_FreeVSList.RemoveTail();
	}
}

//	Allocate a index stream
A3DStream* A3DTrn2StreamMan::AllocIndexStream(int iNumIndex)
{
	//	Snap index number
	const int iBlockIdxCnt = m_pTrn->GetBlockGrid() * m_pTrn->GetBlockGrid() * 6;

	//Edit by PanYupin
	int nNeedIndexNum = iNumIndex;
	iNumIndex = SnapSize(iNumIndex, iBlockIdxCnt * A3DTrn2RenderSlot::MAX_BLOCK_NUM);

	A3DStream* pA3DStream = NULL;

	//	Try to pick a free stream from cache
	if (m_FreeISList.GetCount())
	{
		ALISTPOSITION pos = m_FreeISList.GetHeadPosition();
		while (pos)
		{
			ALISTPOSITION posCur = pos;
			STREAM_NODE& node = m_FreeISList.GetNext(pos);
			if (node.pStream->GetIndexCount() == iNumIndex)
			{
				pA3DStream = node.pStream;
				m_FreeISList.RemoveAt(posCur);
				break;
			}
		}
	}

	if (!pA3DStream)
	{
		//g_A3DErrLog.Log("A3DTrn2StreamMan::AllocIndexStream, Need: %d, Max: %d", nNeedIndexNum, iNumIndex);
		//	Create a new stream
		pA3DStream = new A3DStream;
		if (!pA3DStream || !pA3DStream->Init(m_pTrnRender->m_pA3DDevice, 0, 0, 0, iNumIndex, 0, A3DSTRM_REFERENCEPTR))
		{
			g_A3DErrLog.Log("A3DTrn2StreamMan::AllocIndexStream, Failed to create index stream [%d]!", iNumIndex);
			return false;
		}
		
		m_dwIdxStrmCnt++;
		m_dwIdxStrmSize += iNumIndex * sizeof (WORD);
	}
	return pA3DStream;
}

//	Free a index stream
void A3DTrn2StreamMan::FreeIndexStream(A3DStream* pA3DStream)
{
	if (!pA3DStream)
		return;

	//	For a index stream, it's vertex counter should be 0 !
	ASSERT(!pA3DStream->GetVertCount());

	DWORD dwCurTime = a_GetTime();

	//	Add stream to free stream cache
	STREAM_NODE node;
	node.pStream = pA3DStream;
	node.dwTimeStamp = dwCurTime;
	m_FreeISList.AddHead(node);

	//	Check if there is long-time not used stream in cache. If there is, 
	//	remove it from free cache. Because new free stream is always added
	//	to list head, the one at tail must be the longest not used one
	STREAM_NODE& node_last = m_FreeISList.GetTail();
	if (dwCurTime >= node_last.dwTimeStamp + _CACHE_TIME_)
	{
		ASSERT(node_last.pStream != pA3DStream);
		m_dwIdxStrmCnt--;
		m_dwIdxStrmSize -= node_last.pStream->GetIndexCount() * node_last.pStream->GetIndexSize();
		A3DRELEASE(node_last.pStream);
		m_FreeISList.RemoveTail();
	}
}


