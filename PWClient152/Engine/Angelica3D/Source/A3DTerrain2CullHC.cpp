/*
 * FILE: A3DTerrain2CullHC.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2006/8/3
 *
 * HISTORY: 
 *
 * Copyright (c) 2006 Archosaur Studio, All Rights Reserved.
 */

#include "A3DPI.h"
#include "A3DTerrain2CullHC.h"
#include "A3DTerrain2.h"
#include "A3DMacros.h"
#include "A3DHorizonCull.h"
#include "A3DTerrain2Blk.h"
#include "A3DTerrain2Render.h"
#include "A3DViewport.h"
#include "A3DCamera.h"
#include "AMemory.h"

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

static int _BlockCompare(const void* arg1, const void* arg2)
{
	A3DTerrain2Block* pb1 = *(A3DTerrain2Block**)arg1;
	A3DTerrain2Block* pb2 = *(A3DTerrain2Block**)arg2;
	return (int)((pb1->GetHCWeight() - pb2->GetHCWeight()) * 10.0f);
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement A3DTerrain2CullHC
//	
///////////////////////////////////////////////////////////////////////////

A3DTerrain2CullHC::A3DTerrain2CullHC()
{
	m_ptrn			= NULL;
	m_pHoriCull		= NULL;
	m_aEdgeIns		= NULL;
	m_iNumEdgeBytes	= 0;
	m_iNumBlockRow	= 0;
	m_iNumBlockCol	= 0;
	m_bEnable		= true;
}

A3DTerrain2CullHC::~A3DTerrain2CullHC()
{
}

//	Initialize object
bool A3DTerrain2CullHC::Init(const char* szFile)
{
	//	Try to load horizon culling data
	if (!LoadHorizonCull(szFile))
	{
		g_A3DErrLog.Log("A3DTerrain2CullHC::Init, Failed to load file %s !", szFile);
		return false;
	}

	//	Create horizon cull object
	if (!(m_pHoriCull = new A3DHorizonCull))
		return false;

	if (!m_pHoriCull->Init(800, 600))
	{
		m_aBlkEdges.RemoveAll();
		A3DRELEASE(m_pHoriCull);
		g_A3DErrLog.Log("A3DTerrain2CullHC::Init, Failed to create horizon cull object");
		return false;
	}

	//	Create block edge inserted flag buffer
	m_iNumEdgeBytes	= (m_aBlkEdges.GetSize() + 7) >> 3;
	if (!(m_aEdgeIns = new BYTE [m_iNumEdgeBytes]))
	{
		m_aBlkEdges.RemoveAll();
		A3DRELEASE(m_pHoriCull);
		g_A3DErrLog.Log("A3DTerrain2CullHC::LoadHorizonCull, Not enough memory!");
		return false;
	}

	return true;
}

//	Load horizon culling data from file
bool A3DTerrain2CullHC::LoadHorizonCull(const char* szFile)
{
	DWORD dwRead;
	AFile file;

	if (!file.Open(szFile, AFILE_OPENEXIST | AFILE_BINARY))
	{
		g_A3DErrLog.Log("A3DTerrain2CullHC::LoadHorizonCull, Failed to open file %s", szFile);
		return false;
	}
	
	//	Read file flag and version
	DWORD dwFileFlag, dwVersion;
	file.Read(&dwFileFlag, sizeof (DWORD), &dwRead);
	file.Read(&dwVersion, sizeof (DWORD), &dwRead);
	if (dwFileFlag != TRN2HCFILE_IDENTIFY || dwVersion > TRN2HCFILE_VERSION)
	{
		file.Close();
		g_A3DErrLog.Log("A3DTerrain2CullHC::LoadHorizonCull, Wrong file format or version");
		return false;
	}

	//	Read file header
	TRN2HCFILEHEADER fh;
	if (!file.Read(&fh, sizeof (fh), &dwRead))
	{
		file.Close();
		return false;
	}
	
	m_iNumBlockRow	= fh.iNumBlockRow;
	m_iNumBlockCol	= fh.iNumBlockCol;
	
	//	Load edge data
	m_aBlkEdges.SetSize(fh.iNumEdge, 10);

	for (int i=0; i < fh.iNumEdge; i++)
	{
		TRN2HCFILEEDGE src;
		if (!file.Read(&src, sizeof (src), &dwRead))
		{
			m_aBlkEdges.RemoveAll();
			file.Close();
			return false;
		}

		BLKEDGE& dst = m_aBlkEdges[i];
		dst.y1	= src.y1;
		dst.y2	= src.y2;
	}

	file.Close();

	return true;
}

//	Release resources
void A3DTerrain2CullHC::Release()
{
	A3DRELEASE(m_pHoriCull);

	m_aBlkEdges.RemoveAll();

	if (m_aEdgeIns)
	{
		delete [] m_aEdgeIns;
		m_aEdgeIns = NULL;
	}
}

//	Attach terrain
bool A3DTerrain2CullHC::AttachTerrain(A3DTerrain2* pTerrain)
{
	m_ptrn	= pTerrain;
	return true;
}

//	Prepare to work
bool A3DTerrain2CullHC::PrepareToCull(A3DViewport* pViewport)
{
	if (!m_bEnable)
		return false;

	if (!m_ptrn || !m_pHoriCull)
	{
		ASSERT(m_ptrn && m_pHoriCull);
		return false;
	}

	//	Is attached terrain proper to our HC data ?
	if (m_iNumBlockCol != m_ptrn->GetBlockColNum() ||
		m_iNumBlockRow != m_ptrn->GetBlockRowNum())
	{
		ASSERT(0);
		return false;
	}

	if (!m_pHoriCull->PrepareToWork(pViewport))
		return false;

	memset(m_aEdgeIns, 0, m_iNumEdgeBytes);

	return true;
}

//	Do block culling
void A3DTerrain2CullHC::BlocksCull(A3DViewport* pViewport, const ARectI& rcView)
{
	if (!m_bEnable)
		return;
	
	if (!m_ptrn || !m_pHoriCull)
	{
		ASSERT(m_ptrn && m_pHoriCull);
		return;
	}

	const A3DVECTOR3& vEyePos = pViewport->GetCamera()->GetPos();
	const A3DVECTOR3& vDirH = pViewport->GetCamera()->GetDirH();

	//	Find the block camera is in
	float fInvBlockSize = 1.0f / m_ptrn->GetBlockSize();
	const ARectF& rcTerrain = m_ptrn->GetTerrainArea();
	int cx = (int)((vEyePos.x - rcTerrain.left) * fInvBlockSize);
	int cz = (int)(-(vEyePos.z - rcTerrain.top) * fInvBlockSize);

	A3DTerrain2::ACTBLOCKS* pCurActBlocks = m_ptrn->GetActiveBlocks();
	int iBaseIdx = pCurActBlocks->GetBlockIndex(rcView.top, rcView.left);
	int iBlkPitch = pCurActBlocks->rcArea.Width();

	m_aCandBlks.RemoveAll(false);
	m_ptrn->m_iRdBlockCnt = 0;

	//	Set HC (horizon culling) weight for blocks
	for (int r=rcView.top; r < rcView.bottom; r++)
	{
		int iIndex = iBaseIdx;
		iBaseIdx += iBlkPitch;

		for (int c=rcView.left; c < rcView.right; c++, iIndex++)
		{
			A3DTerrain2Block* pBlock = pCurActBlocks->aBlocks[iIndex];
			if (!pBlock || !pBlock->IsDataLoaded())
				continue;

			//	Calculate block weight
			A3DVECTOR3 vDist = pBlock->GetBlockAABB().Center - vEyePos;
			vDist.y = 0.0f;
			float fWeight = DotProduct(vDist, vDirH) * 10000.0f;
			float fOffset = ((float)(abs(r-cz) + abs(c-cx)));
			if (fWeight >= 0.0f)
				fWeight += fOffset;
			else
				fWeight -= fOffset;

			pBlock->SetHCWeight(fWeight);

			m_aCandBlks.Add(pBlock);
		}
	}	

	if (m_aCandBlks.GetSize())
	{
		//	Sort candidate blocks basing on HC weight
		qsort(m_aCandBlks.GetData(), m_aCandBlks.GetSize(), sizeof (A3DTerrain2Block*), _BlockCompare);

		//	Determine block visibility
		for (int i=0; i < m_aCandBlks.GetSize(); i++)
		{
			A3DTerrain2Block* pBlock = m_aCandBlks[i];
			pBlock->SetVisibleFlag(false);
			
			if (pBlock->IsNoRender())
				continue;

			const A3DAABB& aabb = pBlock->GetBlockAABB();
			if (!pViewport->GetCamera()->AABBInViewFrustum(aabb.Mins, aabb.Maxs))
				continue;

			if (!BlockIsVisible(pBlock))
				continue;

			pBlock->SetVisibleFlag(true);
			m_ptrn->m_pCurRender->RegisterBlock(pBlock);
			m_ptrn->m_iRdBlockCnt++;
		}
	}
}

//	Determine block visibility using horizon culling
bool A3DTerrain2CullHC::BlockIsVisible(A3DTerrain2Block* pBlock)
{
	const A3DAABB& aabb = pBlock->GetBlockAABB();
	const int iPitch = m_ptrn->GetBlockColNum() * 2 + 1;

	A3DHorizonCull::OCCEDGES OccEdges;
	OccEdges.iNumEdge = 0;

	int iEdgeCnt = 0;

	//	Calculate block's top edge index
	int iTop = pBlock->GetRowInTerrain() * iPitch + pBlock->GetColInTerrain();
	if (!GetBlockEdgeInserted(iTop))
	{
		OccEdges.v1[iEdgeCnt].Set(aabb.Mins.x, m_aBlkEdges[iTop].y1, aabb.Maxs.z);
		OccEdges.v2[iEdgeCnt].Set(aabb.Maxs.x, m_aBlkEdges[iTop].y2, aabb.Maxs.z);
		iEdgeCnt++;
	}

	//	Left edge
	int iLeft = iTop + m_ptrn->GetBlockColNum();
	if (!GetBlockEdgeInserted(iLeft))
	{
		OccEdges.v1[iEdgeCnt].Set(aabb.Mins.x, m_aBlkEdges[iLeft].y1, aabb.Maxs.z);
		OccEdges.v2[iEdgeCnt].Set(aabb.Mins.x, m_aBlkEdges[iLeft].y2, aabb.Mins.z);
		iEdgeCnt++;
	}

	//	Right edge
	int iRight = iTop + m_ptrn->GetBlockColNum() + 1;
	if (!GetBlockEdgeInserted(iRight))
	{
		OccEdges.v1[iEdgeCnt].Set(aabb.Maxs.x, m_aBlkEdges[iRight].y1, aabb.Maxs.z);
		OccEdges.v2[iEdgeCnt].Set(aabb.Maxs.x, m_aBlkEdges[iRight].y2, aabb.Mins.z);
		iEdgeCnt++;
	}

	//	Bottom edge
	int iBottom = iTop + iPitch;
	if (!GetBlockEdgeInserted(iBottom))
	{
		OccEdges.v1[iEdgeCnt].Set(aabb.Mins.x, m_aBlkEdges[iBottom].y1, aabb.Mins.z);
		OccEdges.v2[iEdgeCnt].Set(aabb.Maxs.x, m_aBlkEdges[iBottom].y2, aabb.Mins.z);
		iEdgeCnt++;
	}

	OccEdges.iNumEdge = iEdgeCnt;

	//	Test quad
	A3DHorizonCull::TESTQUAD TestQuad;
	TestQuad.v1.Set(aabb.Mins.x, aabb.Maxs.y, aabb.Maxs.z);
	TestQuad.v2.Set(aabb.Maxs.x, aabb.Maxs.y, aabb.Maxs.z);
	TestQuad.v3.Set(aabb.Maxs.x, aabb.Maxs.y, aabb.Mins.z);
	TestQuad.v4.Set(aabb.Mins.x, aabb.Maxs.y, aabb.Mins.z);

	if (!m_pHoriCull->BlockVisibility(aabb, OccEdges, TestQuad))
		return false;

	SetBlockEdgeInserted(iTop);
	SetBlockEdgeInserted(iLeft);
	SetBlockEdgeInserted(iRight);
	SetBlockEdgeInserted(iBottom);

	return true;
}



