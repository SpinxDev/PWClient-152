/*
 * FILE: TerrainRender.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/7/15
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "Global.h"
#include "TerrainRender.h"
#include "Terrain.h"
#include "AMemory.h"
#include "AAssist.h"
#include "A3D.h"
#include "A3DTerrain2Blk.h"

//#define new A_DEBUG_NEW

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
//	Implement CTerrainRender
//	
///////////////////////////////////////////////////////////////////////////

CTerrainRender::CTerrainRender(CTerrain* pTerrain)
{
	m_pTerrain	= pTerrain;
}

CTerrainRender::~CTerrainRender()
{
}

//	Reset terrain information
void CTerrainRender::UpdateTerrainInfo()
{
	m_rcTerrain		= m_pTerrain->GetTerrainArea();
	m_fTerrainWid	= m_pTerrain->GetTerrainSize();
	m_fTerrainLen	= m_pTerrain->GetTerrainSize();
	m_fBlockSize	= m_pTerrain->GetTileSize() * m_iBlockGrid;
	m_fGridSize		= m_pTerrain->GetTileSize();

	//	Update block size
	float fBlockSize = m_pTerrain->GetTileSize() * m_iBlockGrid;
	for (int i=0; i < m_pCurActBlocks->aBlocks.GetSize(); i++)
	{
		A3DTerrain2Block* pBlock = m_pCurActBlocks->aBlocks[i];
		if (!pBlock)
			continue;

		if (pBlock->GetBlockSize() == fBlockSize)
			break;
		
		pBlock->SetBlockSize(fBlockSize);
	}
}

/*	Update terrain height of specified area

	rcArea: area (in meters) of terrain whose height will be udpated.
	aHeiData: whole map's terrain absolute height data
	aNormals: whole map's terrain normal data
	iHMSize: size of height map's border in pixels
*/
bool CTerrainRender::UpdateTerrainHeight(const ARectF& rcArea, const float* aHeiData, 
										 const A3DVECTOR3* aNormals, int iHMSize)
{
	ARectI rcBlock;
	rcBlock.left	= (int)((rcArea.left - m_rcTerrain.left) / m_fBlockSize);
	rcBlock.top		= (int)(-(rcArea.top - m_rcTerrain.top) / m_fBlockSize);
	rcBlock.right	= (int)((rcArea.right - m_rcTerrain.left) / m_fBlockSize) + 1;
	rcBlock.bottom	= (int)(-(rcArea.bottom - m_rcTerrain.top) / m_fBlockSize) + 1;

	a_ClampFloor(rcBlock.left, 0);
	a_ClampFloor(rcBlock.top, 0);
	a_ClampRoof(rcBlock.right, m_iNumAllBlockCol);
	a_ClampRoof(rcBlock.bottom, m_iNumAllBlockRow);

	for (int i=rcBlock.top; i < rcBlock.bottom; i++)
	{
		for (int j=rcBlock.left; j < rcBlock.right; j++)
		{
			A3DTerrain2Block* pBlock = m_pCurActBlocks->GetBlock(i, j, false);
			if (pBlock)
				UpdateBlockHeight(pBlock, aHeiData, aNormals, iHMSize);
		}
	}

	return true;
}

//	Update height of a block
//	aHeiData: whole map's terrain absolute height data
bool CTerrainRender::UpdateBlockHeight(A3DTerrain2Block* pBlock, const float* aHeiData, 
									   const A3DVECTOR3* aNormals, int iHMSize)
{
	int r = pBlock->GetRowInTerrain() * m_iBlockGrid;
	int c = pBlock->GetColInTerrain() * m_iBlockGrid;

	const float* pSrcLine1 = aHeiData + r * iHMSize + c;
	const A3DVECTOR3* pSrcLine2 = aNormals + r * iHMSize + c;
	int cnt0=0, cnt1=0;

	//	Fill temporary height buffer
	for (int i=0; i < m_iBlockGrid + 1; i++)
	{
		const float* pHeight = pSrcLine1;
		const A3DVECTOR3* pNormal = pSrcLine2;

		pSrcLine1 += iHMSize;
		pSrcLine2 += iHMSize;

		for (int j=0; j < m_iBlockGrid + 1; j++, pHeight++, pNormal++)
		{
			m_TempBufs.aTempHei[cnt0++] = *pHeight;

			m_TempBufs.aTempNormal[cnt1++] = pNormal->x;
			m_TempBufs.aTempNormal[cnt1++] = pNormal->y;
			m_TempBufs.aTempNormal[cnt1++] = pNormal->z;
		}
	}

	float sx = m_rcTerrain.left + pBlock->GetColInTerrain() * m_fBlockSize;
	float sz = m_rcTerrain.top - pBlock->GetRowInTerrain() * m_fBlockSize;

	pBlock->FillVertexBuffer(sx, sz);

	return true;
}

//	Reset all blocks' LOD scale
void CTerrainRender::ResetLODScales()
{
	int iNumBlock = m_pCurActBlocks->aBlocks.GetSize();

	for (int i=0; i < iNumBlock; i++)
	{
		A3DTerrain2Block* pBlock = m_pCurActBlocks->aBlocks[i];
		if (pBlock)
			pBlock->SetLODScale(1.0f);
	}
}

//	Get specified block
A3DTerrain2Block* CTerrainRender::GetBlock(int r, int c)
{
	if (!m_pCurActBlocks)
		return NULL;

	if (!m_pCurActBlocks->rcArea.PtInRect(c, r))
		return NULL;

	return m_pCurActBlocks->GetBlock(r, c, false);
}


