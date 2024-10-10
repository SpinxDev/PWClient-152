  /*
 * FILE: A3DTerrain2.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/6/5
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "A3DTerrain2.h"
#include "A3DPI.h"
#include "A3DTerrain2Blk.h"
#include "A3DCollision.h"
#include "A3DTerrain2Env.h"
#include "A3DTerrain2LOD.h"
#include "AMemory.h"
#include "AAssist.h"

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
//	Implement A3DTerrain2
//	
///////////////////////////////////////////////////////////////////////////

//	Ray trace function
bool A3DTerrain2::RayTrace(const A3DVECTOR3& vStart, const A3DVECTOR3& vVel, float fTime, RAYTRACERT* pTraceRt)
{
	m_RayTraceInfo.vStart		= vStart;
	m_RayTraceInfo.vDelta		= vVel * fTime;
	m_RayTraceInfo.vEnd			= vStart + m_RayTraceInfo.vDelta;
	m_RayTraceInfo.fFraction	= 1.0f;

	if (RayTraceInActiveBlocks())
	{
		pTraceRt->fFraction	= m_RayTraceInfo.fFraction;
		pTraceRt->vNormal	= m_RayTraceInfo.vNormal;
		pTraceRt->vHitPos	= vStart + m_RayTraceInfo.vDelta * m_RayTraceInfo.fFraction;
		pTraceRt->vPoint	= pTraceRt->vHitPos;

		return true;
	}

	pTraceRt->fFraction = 1.0f;

	return false;
}

//	Initialize block 2DDA information
bool A3DTerrain2::InitBlock2DDA()
{
	if (m_pCurActBlocks->rcArea.IsEmpty())
		return false;

	//	Currenly active area AABB
	A3DVECTOR3 vMins, vMaxs;
	vMins.x	= m_rcTerrain.left + m_pCurActBlocks->rcArea.left * m_fBlockSize;
	vMaxs.z = m_rcTerrain.top - m_pCurActBlocks->rcArea.top * m_fBlockSize;
	vMaxs.x = vMins.x + m_pCurActBlocks->rcArea.Width() * m_fBlockSize;
	vMins.z = vMaxs.z - m_pCurActBlocks->rcArea.Height() * m_fBlockSize;
	vMins.y = -99999.0f;
	vMaxs.y = 99999.0f;
	
	//	Clip ray into active area AABB
	A3DVECTOR3 v0, v1, vDelta, vTemp;
	float fFraction;
	if (!CLS_RayToAABB3(m_RayTraceInfo.vStart, m_RayTraceInfo.vDelta, vMins, vMaxs, v0, &fFraction, vTemp))
		return false;

	if (!CLS_RayToAABB3(m_RayTraceInfo.vEnd, -m_RayTraceInfo.vDelta, vMins, vMaxs, v1, &fFraction, vTemp))
		return false;

	//	Build block 2DDA information
	float fInvBlockSize = 1.0f / m_fBlockSize;
	vDelta = m_RayTraceInfo.vDelta;

	//	Start and end blocks
	float fStartX	= (v0.x - vMins.x) * fInvBlockSize;
	float fEndX		= (v1.x - vMins.x) * fInvBlockSize;
	float fStartZ	= -(v0.z - vMaxs.z) * fInvBlockSize;
	float fEndZ		= -(v1.z - vMaxs.z) * fInvBlockSize;

	m_Block2DDA.vStart[0]	= (int)fStartX;
	m_Block2DDA.vEnd[0]		= (int)fEndX;
	m_Block2DDA.vStart[1]	= (int)fStartZ;
	m_Block2DDA.vEnd[1]		= (int)fEndZ;

	m_Block2DDA.s[0]		= vDelta.x < 0 ? -1 : 1;
	m_Block2DDA.s[1]		= vDelta.z < 0 ? 1 : -1;

	if (m_Block2DDA.s[0] < 0 && fStartX - (int)fStartX == 0.0f)
		m_Block2DDA.vStart[0]--;

	if (m_Block2DDA.s[1] < 0 && fStartZ - (int)fStartZ == 0.0f)
		m_Block2DDA.vStart[1]--;

	a_Clamp(m_Block2DDA.vStart[0], 0, m_pCurActBlocks->rcArea.Width()-1);
	a_Clamp(m_Block2DDA.vEnd[0], 0, m_pCurActBlocks->rcArea.Width()-1);
	a_Clamp(m_Block2DDA.vStart[1], 0, m_pCurActBlocks->rcArea.Height()-1);
	a_Clamp(m_Block2DDA.vEnd[1], 0, m_pCurActBlocks->rcArea.Height()-1);

	m_Block2DDA.aSteps[0]	= abs(m_Block2DDA.vEnd[0] - m_Block2DDA.vStart[0]) + 1;
	m_Block2DDA.aSteps[1]	= abs(m_Block2DDA.vEnd[1] - m_Block2DDA.vStart[1]) + 1;

	vDelta.x = (float)fabs(vDelta.x);
	vDelta.z = (float)fabs(vDelta.z);

	float dx, dz;
	if (m_Block2DDA.s[0] > 0)
		dx = (vMins.x + (m_Block2DDA.vStart[0] + 1) * m_fBlockSize) - v0.x;
	else
		dx = v0.x - (vMins.x + m_Block2DDA.vStart[0] * m_fBlockSize);

	if (dx == 0.0f)
		dx = m_fBlockSize;

	if (m_Block2DDA.s[1] > 0)
		dz = -((vMaxs.z - (m_Block2DDA.vStart[1] + 1) * m_fBlockSize) - v0.z);
	else
		dz = (vMaxs.z - m_Block2DDA.vStart[1] * m_fBlockSize) - v0.z;

	if (dz == 0.0f)
		dz = m_fBlockSize;

	if (vDelta.y == 0.0f)
	{
		m_Block2DDA.iMajor = 0;
		m_Block2DDA.d = 0.0f;
		m_Block2DDA.e = 0.0f;
		m_Block2DDA.i = 0.0f;
		m_Block2DDA.c = 0.0f;
	}
	else if (vDelta.x > vDelta.z)	//	x major
	{
		m_Block2DDA.iMajor = 0;
		float fSlope = vDelta.z / vDelta.x;
		
		m_Block2DDA.d = dz;
		m_Block2DDA.e = m_fBlockSize - dz;
		m_Block2DDA.i = fSlope * m_fBlockSize;
		m_Block2DDA.c = fSlope * dx;
	}
	else	//	z major
	{
		m_Block2DDA.iMajor = 1;
		float fSlope = vDelta.x / vDelta.z;
		
		m_Block2DDA.d = dx;
		m_Block2DDA.e = m_fBlockSize - dx;
		m_Block2DDA.i = fSlope * m_fBlockSize;
		m_Block2DDA.c = fSlope * dz;
	}

	return true;
}

//	Initiailze grid 2DDA information
bool A3DTerrain2::InitGrid2DDA(A3DTerrain2Block* pBlock)
{
	//	Get block's AABB
	const A3DVECTOR3& vMins = pBlock->GetBlockAABB().Mins;
	const A3DVECTOR3& vMaxs = pBlock->GetBlockAABB().Maxs;

	//	Clip ray into active area AABB
	A3DVECTOR3 v0, v1, vDelta, vTemp;
	float fFraction;
	if (!CLS_RayToAABB3(m_RayTraceInfo.vStart, m_RayTraceInfo.vDelta, vMins, vMaxs, v0, &fFraction, vTemp))
		return false;

	if (!CLS_RayToAABB3(m_RayTraceInfo.vEnd, -m_RayTraceInfo.vDelta, vMins, vMaxs, v1, &fFraction, vTemp))
		return false;

	//	Build grid 2DDA information
	float fInvGridSize = 1.0f / m_fGridSize;
	vDelta = m_RayTraceInfo.vDelta;

	//	Start and end grid
	float fStartX	= (v0.x - vMins.x) * fInvGridSize;
	float fEndX		= (v1.x - vMins.x) * fInvGridSize;
	float fStartZ	= -(v0.z - vMaxs.z) * fInvGridSize;
	float fEndZ		= -(v1.z - vMaxs.z) * fInvGridSize;

	m_Grid2DDA.vStart[0]	= (int)fStartX;
	m_Grid2DDA.vEnd[0]		= (int)fEndX;
	m_Grid2DDA.vStart[1]	= (int)fStartZ;
	m_Grid2DDA.vEnd[1]		= (int)fEndZ;

	m_Grid2DDA.s[0]			= vDelta.x < 0 ? -1 : 1;
	m_Grid2DDA.s[1]			= vDelta.z < 0 ? 1 : -1;

	if (m_Grid2DDA.s[0] < 0 && fStartX - (int)fStartX == 0.0f)
		m_Grid2DDA.vStart[0]--;

	if (m_Grid2DDA.s[1] < 0 && fStartZ - (int)fStartZ == 0.0f)
		m_Grid2DDA.vStart[1]--;

	a_Clamp(m_Grid2DDA.vStart[0], 0, m_iBlockGrid-1);
	a_Clamp(m_Grid2DDA.vEnd[0], 0, m_iBlockGrid-1);
	a_Clamp(m_Grid2DDA.vStart[1], 0, m_iBlockGrid-1);
	a_Clamp(m_Grid2DDA.vEnd[1], 0, m_iBlockGrid-1);

	m_Grid2DDA.aSteps[0]	= abs(m_Grid2DDA.vEnd[0] - m_Grid2DDA.vStart[0]) + 1;
	m_Grid2DDA.aSteps[1]	= abs(m_Grid2DDA.vEnd[1] - m_Grid2DDA.vStart[1]) + 1;

	vDelta.x = (float)fabs(vDelta.x);
	vDelta.z = (float)fabs(vDelta.z);

	float dx, dz;
	if (m_Grid2DDA.s[0] > 0)
		dx = (vMins.x + (m_Grid2DDA.vStart[0] + 1) * m_fGridSize) - v0.x;
	else
		dx = v0.x - (vMins.x + m_Grid2DDA.vStart[0] * m_fGridSize);

	if (dx == 0.0f)
		dx = m_fGridSize;

	if (m_Grid2DDA.s[1] > 0)
		dz = -((vMaxs.z - (m_Grid2DDA.vStart[1] + 1) * m_fGridSize) - v0.z);
	else
		dz = (vMaxs.z - m_Grid2DDA.vStart[1] * m_fGridSize) - v0.z;

	if (dz == 0.0f)
		dz = m_fGridSize;

	if (vDelta.y == 0.0f)
	{
		m_Grid2DDA.iMajor = 0;
		m_Grid2DDA.d = 0.0f;
		m_Grid2DDA.e = 0.0f;
		m_Grid2DDA.i = 0.0f;
		m_Grid2DDA.c = 0.0f;
	}
	else if (vDelta.x > vDelta.z)	//	x major
	{
		m_Grid2DDA.iMajor = 0;
		float fSlope = vDelta.z / vDelta.x;
		
		m_Grid2DDA.d = dz;
		m_Grid2DDA.e = m_fGridSize - dz;
		m_Grid2DDA.i = fSlope * m_fGridSize;
		m_Grid2DDA.c = fSlope * dx;
	}
	else	//	z major
	{
		m_Grid2DDA.iMajor = 1;
		float fSlope = vDelta.x / vDelta.z;
		
		m_Grid2DDA.d = dx;
		m_Grid2DDA.e = m_fGridSize - dx;
		m_Grid2DDA.i = fSlope * m_fGridSize;
		m_Grid2DDA.c = fSlope * dz;
	}

	return true;
}

//	Ray trace in all active blocks
bool A3DTerrain2::RayTraceInActiveBlocks()
{
	if (!InitBlock2DDA())
		return false;

	int c = m_Block2DDA.vStart[0];
	int r = m_Block2DDA.vStart[1];

	int iColPitch = m_Block2DDA.s[0];
	int iRowPitch = m_pCurActBlocks->rcArea.Width();

	int iBlock = r * iRowPitch + c;

	if (m_Block2DDA.s[1] < 0)
		iRowPitch = -iRowPitch;

	int sc = m_Block2DDA.s[0];
	int sr = m_Block2DDA.s[1];

	BlockArray& aBlocks = m_pCurActBlocks->aBlocks;

	//	Visit the start block
	if (RayTraceInBlock(r, c, iBlock))
		return true;

	if (m_Block2DDA.aSteps[0] == 1 && m_Block2DDA.aSteps[1] == 1)
		return false;

	if (m_Block2DDA.iMajor == 0)	//	x major
	{
		float e	= m_Block2DDA.e;
		int nx=1, nz=1;

		while (nx < m_Block2DDA.aSteps[0] || nz < m_Block2DDA.aSteps[1])
		{
			//	Handle the special case
			if (nx == m_Block2DDA.aSteps[0])
			{
				for (; nz < m_Block2DDA.aSteps[1]; nz++)
				{
					r += sr;
					iBlock += iRowPitch;
					if (RayTraceInBlock(r, c, iBlock))
						return true;
				}

				return false;
			}

			bool bRowInc = false;

			e += m_Block2DDA.i;

			if (e > m_fBlockSize)
			{
				r	   += sr;
				e	   -= m_fBlockSize;
				iBlock += iRowPitch;
				bRowInc	= true;
				nz++;
			}

			if (nx < m_Block2DDA.aSteps[0])
			{
				c		+= sc;
				iBlock	+= iColPitch;
				nx++;

				if (bRowInc)
				{
					if (m_Block2DDA.c > m_Block2DDA.d)
					{
						int iTemp = iBlock - iColPitch;
						if (RayTraceInBlock(r, c-sc, iTemp))
							return true;
					}
					else
					{
						int iTemp = iBlock - iRowPitch;
						if (RayTraceInBlock(r-sr, c, iTemp))
							return true;
					}
				}
			}

			if (RayTraceInBlock(r, c, iBlock))
				return true;

			m_Block2DDA.d = m_fBlockSize - e;
		}
	}
	else	//	z major
	{
		float e	= m_Block2DDA.e;
		int nx=1, nz=1;

		while (nx < m_Block2DDA.aSteps[0] || nz < m_Block2DDA.aSteps[1])
		{
			//	Handle the special case
			if (nz == m_Block2DDA.aSteps[1])
			{
				for (; nx < m_Block2DDA.aSteps[0]; nx++)
				{
					c += sc;
					iBlock += iColPitch;
					if (RayTraceInBlock(r, c, iBlock))
						return true;
				}

				return false;
			}

			bool bColInc = false;

			e += m_Block2DDA.i;

			if (e > m_fBlockSize)
			{
				c	   += sc;
				e	   -= m_fBlockSize;
				iBlock += iColPitch;
				bColInc	= true;
				nx++;
			}

			if (nz < m_Block2DDA.aSteps[1])
			{
				r		+= sr;
				iBlock	+= iRowPitch;
				nz++;

				if (bColInc)
				{
					if (m_Block2DDA.c > m_Block2DDA.d)
					{
						int iTemp = iBlock - iRowPitch;
						if (RayTraceInBlock(r-sr, c, iTemp))
							return true;
					}
					else
					{
						int iTemp = iBlock - iColPitch;
						if (RayTraceInBlock(r, c-sc, iTemp))
							return true;
					}
				}
			}

			if (RayTraceInBlock(r, c, iBlock))
				return true;

			m_Block2DDA.d = m_fBlockSize - e;
		}
	}

	return false;
}

/*	Ray trace in active blocks

	iBlockRow, iBlockCol: block position in active area.
	iBlock: block index in active area
*/
bool A3DTerrain2::RayTraceInBlock(int iBlockRow, int iBlockCol, int iBlock)
{
	if (iBlockCol < 0 || iBlockCol >= m_pCurActBlocks->rcArea.Width() ||
		iBlockRow < 0 || iBlockRow >= m_pCurActBlocks->rcArea.Height())
		return false;

	ASSERT(iBlock >= 0 && iBlock < m_pCurActBlocks->rcArea.Width() * m_pCurActBlocks->rcArea.Height());
	A3DTerrain2Block* pBlock = m_pCurActBlocks->aBlocks[iBlock];
	if (!pBlock || !pBlock->IsDataLoaded())
		return false;

	if (pBlock->IsNoTrace())
		return false;

	if (!InitGrid2DDA(pBlock))
		return false;

	int c = m_Grid2DDA.vStart[0];
	int r = m_Grid2DDA.vStart[1];

	int iColPitch = m_Grid2DDA.s[0];
	int iRowPitch = m_iBlockGrid;

	int iGrid = r * m_iBlockGrid + c;

	if (m_Grid2DDA.s[1] < 0)
		iRowPitch = -iRowPitch;

	int sc = m_Grid2DDA.s[0];
	int sr = m_Grid2DDA.s[1];

	//	Visit the start block
	if (RayTraceInGrid(pBlock, r, c, iGrid))
		return true;

	if (m_Grid2DDA.aSteps[0] == 1 && m_Grid2DDA.aSteps[1] == 1)
		return false;

	if (m_Grid2DDA.iMajor == 0)	//	x major
	{
		float e	= m_Grid2DDA.e;
		int nx=1, nz=1;

		while (nx < m_Grid2DDA.aSteps[0] || nz < m_Grid2DDA.aSteps[1])
		{
			//	Handle the special case
			if (nx == m_Grid2DDA.aSteps[0])
			{
				for (; nz < m_Grid2DDA.aSteps[1]; nz++)
				{
					r += sr;
					iGrid += iRowPitch;
					if (RayTraceInGrid(pBlock, r, c, iGrid))
						return true;
				}

				return false;
			}

			bool bRowInc = false;

			e += m_Grid2DDA.i;

			if (e > m_fGridSize)
			{
				r	   += sr;
				e	   -= m_fGridSize;
				iGrid  += iRowPitch;
				bRowInc	= true;
				nz++;
			}

			if (nx < m_Grid2DDA.aSteps[0])
			{
				c		+= sc;
				iGrid	+= iColPitch;
				nx++;

				if (bRowInc)
				{
					if (m_Grid2DDA.c > m_Grid2DDA.d)
					{
						int iTemp = iGrid - iColPitch;
						if (RayTraceInGrid(pBlock, r, c-sc, iTemp))
							return true;
					}
					else
					{
						int iTemp = iGrid - iRowPitch;
						if (RayTraceInGrid(pBlock, r-sr, c, iTemp))
							return true;
					}
				}
			}

			if (RayTraceInGrid(pBlock, r, c, iGrid))
				return true;

			m_Grid2DDA.d = m_fGridSize - e;
		}
	}
	else	//	z major
	{
		float e	= m_Grid2DDA.e;
		int nx=1, nz=1;

		while (nx < m_Grid2DDA.aSteps[0] || nz < m_Grid2DDA.aSteps[1])
		{
			//	Handle the special case
			if (nz == m_Grid2DDA.aSteps[1])
			{
				for (; nx < m_Grid2DDA.aSteps[0]; nx++)
				{
					c += sc;
					iGrid += iColPitch;
					if (RayTraceInGrid(pBlock, r, c, iGrid))
						return true;
				}

				return false;
			}

			bool bColInc = false;

			e += m_Grid2DDA.i;

			if (e > m_fGridSize)
			{
				c	   += sc;
				e	   -= m_fGridSize;
				iGrid  += iColPitch;
				bColInc	= true;
				nx++;
			}

			if (nz < m_Grid2DDA.aSteps[1])
			{
				r		+= sr;
				iGrid	+= iRowPitch;
				nz++;

				if (bColInc)
				{
					if (m_Grid2DDA.c > m_Grid2DDA.d)
					{
						int iTemp = iGrid - iRowPitch;
						if (RayTraceInGrid(pBlock, r-sr, c, iTemp))
							return true;
					}
					else
					{
						int iTemp = iGrid - iColPitch;
						if (RayTraceInGrid(pBlock, r, c-sc, iTemp))
							return true;
					}
				}
			}

			if (RayTraceInGrid(pBlock, r, c, iGrid))
				return true;

			m_Grid2DDA.d = m_fGridSize - e;
		}
	}

	return false;
}

/*	Ray trace in grid

	pBlock: terrain block object
	r, c: grid position in block
	iGrid: grid index in block
*/
bool A3DTerrain2::RayTraceInGrid(A3DTerrain2Block* pBlock, int r, int c, int iGrid)
{
	if (r < 0 || r >= m_iBlockGrid || c < 0 || c >= m_iBlockGrid)
		return false;

	ASSERT(iGrid >= 0 && iGrid < m_iBlockGrid * m_iBlockGrid);

	A3DTerrain2LOD::GRID* aGrids = m_pLODMan->GetGrids();
	A3DTerrain2LOD::GRID* pGrid = &aGrids[iGrid];
	
	float fFraction;
	A3DVECTOR3 vHit;
	float fEpsilon = 0.001f / m_fGridSize;

	//	Check face 1
	A3DVECTOR3 v0 = pBlock->GetVertexPos(pGrid->v0);
	A3DVECTOR3 v1 = pBlock->GetVertexPos(pGrid->v1);
	A3DVECTOR3 v2 = pBlock->GetVertexPos(pGrid->v2);

	A3DVECTOR3 vEdge1 = v1 - v0;
	A3DVECTOR3 vEdge2 = v2 - v0;
	v0  = v0 + -(vEdge1 + vEdge2) * fEpsilon;
	v1 += vEdge1 * fEpsilon;
	v2 += vEdge2 * fEpsilon;

	if (CLS_RayToTriangle(m_RayTraceInfo.vStart, m_RayTraceInfo.vDelta, v0, v1, v2, vHit, false, &fFraction))
	{
	//	if (fFraction < 1.0f)
	//	{
		m_RayTraceInfo.fFraction = fFraction;

		//	Calculate normal
		m_RayTraceInfo.vNormal.CrossProduct(vEdge1, vEdge2);
		m_RayTraceInfo.vNormal.Normalize();
	//	}

		return true;
	}

	//	Check face 2
	A3DVECTOR3 v3 = pBlock->GetVertexPos(pGrid->v3);
	A3DVECTOR3 v4 = pBlock->GetVertexPos(pGrid->v4);
	A3DVECTOR3 v5 = pBlock->GetVertexPos(pGrid->v5);

	vEdge1 = v4 - v3;
	vEdge2 = v5 - v3;
	v3  = v3 + -(vEdge1 + vEdge2) * fEpsilon;
	v4 += vEdge1 * fEpsilon;
	v5 += vEdge2 * fEpsilon;

	if (CLS_RayToTriangle(m_RayTraceInfo.vStart, m_RayTraceInfo.vDelta, v3, v4, v5, vHit, false, &fFraction))
	{
	//	if (fFraction < 1.0f)
	//	{
		m_RayTraceInfo.fFraction = fFraction;

		//	Calculate normal
		m_RayTraceInfo.vNormal.CrossProduct(vEdge1, vEdge2);
		m_RayTraceInfo.vNormal.Normalize();
	//	}

		return true;
	}

	return false;

/*	if (r < 0 || r >= m_iBlockGrid || c < 0 || c >= m_iBlockGrid)
		return false;

	ASSERT(iGrid >= 0 && iGrid < m_iBlockGrid * m_iBlockGrid);

	const A3DTRN2VERTEX* vVertices = pBlock->GetVertices();
	A3DTerrain2LOD::GRID* aGrids = A3D::g_pA3DTerrain2Env->GetTerrainLOD()->GetGrids();

	A3DTerrain2LOD::GRID* pGrid = &aGrids[iGrid];
	float fFraction;
	A3DVECTOR3 vHit;

	//	Check face 1
	const A3DVECTOR3& v0 = vVertices[pGrid->v0];
	const A3DVECTOR3& v1 = vVertices[pGrid->v1];
	const A3DVECTOR3& v2 = vVertices[pGrid->v2];

	if (CLS_RayToTriangle(m_RayTraceInfo.vStart, m_RayTraceInfo.vDelta, v0, v1, v2, vHit, false, &fFraction))
	{
		m_RayTraceInfo.fFraction = fFraction;

		A3DVECTOR3 e1 = v1 - v0;
		A3DVECTOR3 e2 = v2 - v0;
		m_RayTraceInfo.vNormal.CrossProduct(e1, e2);
		m_RayTraceInfo.vNormal.Normalize();

		return true;
	}

	//	Check face 2
	const A3DVECTOR3& v3 = vVertices[pGrid->v3];
	const A3DVECTOR3& v4 = vVertices[pGrid->v4];
	const A3DVECTOR3& v5 = vVertices[pGrid->v5];

	if (CLS_RayToTriangle(m_RayTraceInfo.vStart, m_RayTraceInfo.vDelta, v3, v4, v5, vHit, false, &fFraction))
	{
		m_RayTraceInfo.fFraction = fFraction;

		A3DVECTOR3 e1 = v4 - v3;
		A3DVECTOR3 e2 = v5 - v3;
		m_RayTraceInfo.vNormal.CrossProduct(e1, e2);
		m_RayTraceInfo.vNormal.Normalize();

		return true;
	}

	return false;
*/
}

//	Get height and normal of specified position
float A3DTerrain2::GetPosHeight(const A3DVECTOR3& vPos, A3DVECTOR3* pvNormal/* NULL */)
{
	//	Give a default value to normal
	if (pvNormal)
		pvNormal->Clear();

	if (m_pCurActBlocks->rcArea.IsEmpty())
		return 0.0f;
	
	//	Currenly active area AABB
	ARectF rcActive;
	rcActive.left	= m_rcTerrain.left + m_pCurActBlocks->rcArea.left * m_fBlockSize;
	rcActive.top	= m_rcTerrain.top - m_pCurActBlocks->rcArea.top * m_fBlockSize;
	rcActive.right	= rcActive.left + m_pCurActBlocks->rcArea.Width() * m_fBlockSize;
	rcActive.bottom = rcActive.top - m_pCurActBlocks->rcArea.Height() * m_fBlockSize;
	
	if (vPos.x < rcActive.left || vPos.x > rcActive.right ||
		vPos.z > rcActive.top || vPos.z < rcActive.bottom)
		return 0.0f;

	//	Get block this position is in
	float fInvBlockSize = 1.0f / m_fBlockSize;
	int iCol = (int)((vPos.x - rcActive.left) * fInvBlockSize);
	int iRow = (int)(-(vPos.z - rcActive.top) * fInvBlockSize);
	a_Clamp(iCol, 0, m_pCurActBlocks->rcArea.Width()-1);
	a_Clamp(iRow, 0, m_pCurActBlocks->rcArea.Height()-1);

	int iBlock = iRow * m_pCurActBlocks->rcArea.Width() + iCol;
	A3DTerrain2Block* pBlock = m_pCurActBlocks->aBlocks[iBlock];
	if (!pBlock || !pBlock->IsDataLoaded())
		return 0.0f;

	//	If whole block is a hole, return as if there is no block here
	if (pBlock->IsNoPosHeight())
		return 0.0f;

	//	Get block's AABB
	const A3DAABB& aabb = pBlock->GetBlockAABB();

	//	Get grid this position is in
	float fInvGridSize = 1.0f / m_fGridSize;
	iCol = (int)((vPos.x - aabb.Mins.x) * fInvGridSize);
	iRow = (int)(-(vPos.z - aabb.Maxs.z) * fInvGridSize);
	a_Clamp(iCol, 0, m_iBlockGrid-1);
	a_Clamp(iRow, 0, m_iBlockGrid-1);

	int iGrid = iRow * m_iBlockGrid + iCol;
	const A3DTerrain2LOD::GRID& Grid = m_pLODMan->GetGrids()[iGrid];

	A3DVECTOR3 v0 = pBlock->GetVertexPos(Grid.v0);
	A3DVECTOR3 v1 = pBlock->GetVertexPos(Grid.v1);
	A3DVECTOR3 v2 = pBlock->GetVertexPos(Grid.v2);
	A3DVECTOR3 v3 = pBlock->GetVertexPos(Grid.v3);
	A3DVECTOR3 v4 = pBlock->GetVertexPos(Grid.v4);
	A3DVECTOR3 v5 = pBlock->GetVertexPos(Grid.v5);

	A3DVECTOR3 vDest;
	float dx, dz;

	if (iGrid == m_iBlockGrid-1 || iGrid == m_iBlockGrid * (m_iBlockGrid - 1))
	{
		//	The grid is on right-top corner or left-bottom corner
		dx = vPos.x - v2.x;
		dz = vPos.z - v2.z;

		if (dx > dz)
		{
			vDest  = v5 + (v4 - v5) * (dx / (v4.x - v5.x));
			vDest += (v3 - v4) * (dz / (v3.z - v4.z));

			if (pvNormal)
			{
				pvNormal->CrossProduct(v4 - v3, v5 - v3);
				pvNormal->Normalize();
			}
		}
		else
		{
			vDest  = v2 + (v0 - v2) * (dz / (v0.z - v2.z));
			vDest += (v1 - v0) * (dx / (v1.x - v0.x));

			if (pvNormal)
			{
				pvNormal->CrossProduct(v1 - v0, v2 - v0);
				pvNormal->Normalize();
			}
		}
	}
	else
	{
		dx = vPos.x - v0.x;
		dz = vPos.z - v0.z;

		if (dx > -dz)
		{
			vDest  = v0 + (v1 - v0) * (dx / (v1.x - v0.x));
			vDest += (v2 - v1) * (dz / (v2.z - v1.z));

			if (pvNormal)
			{
				pvNormal->CrossProduct(v1 - v0, v2 - v0);
				pvNormal->Normalize();
			}
		}
		else
		{
			vDest  = v3 + (v5 - v3) * (dz / (v5.z - v3.z));
			vDest += (v4 - v5) * (dx / (v4.x - v5.x));

			if (pvNormal)
			{
				pvNormal->CrossProduct(v4 - v3, v5 - v3);
				pvNormal->Normalize();
			}
		}
	}

	return vDest.y;
}

/*	Get grid vertices of specified area

	Return true for success, otherwise return false.

	vCenter: center of rectangle area
	iGridWid, iGridLen: rectangle area size in grid number
	pVertBuf: buffer used to store vertex positions. Note, pVertBuf must can store
			at least (iGridWid+1) * (iGridLen+1) vertices, because n x n grid has
			(n+1) x (n+1) vertices.
*/
bool A3DTerrain2::GetVertsOfArea(const A3DVECTOR3& vCenter, int iGridWid, int iGridLen,
								 A3DVECTOR3* pVertBuf)
{
	if (m_pCurActBlocks->rcArea.IsEmpty())
		return false;
	
	float fInvGridSize = 1.0f / m_fGridSize;

	ARectI rcGrid;
	rcGrid.left		= (int)((vCenter.x - m_rcTerrain.left) * fInvGridSize) - (iGridWid >> 1);
	rcGrid.top		= (int)((m_rcTerrain.top - vCenter.z) * fInvGridSize) - (iGridLen >> 1);
	rcGrid.right	= rcGrid.left + iGridWid;
	rcGrid.bottom	= rcGrid.top + iGridLen;

	int iMaxGrid = m_iNumAllBlockCol * m_iBlockGrid;
	a_Clamp(rcGrid.left, 0, iMaxGrid);
	a_Clamp(rcGrid.right, 0, iMaxGrid);
	iMaxGrid = m_iNumAllBlockRow * m_iBlockGrid;
	a_Clamp(rcGrid.top, 0, iMaxGrid);
	a_Clamp(rcGrid.bottom, 0, iMaxGrid);

	if (rcGrid.IsEmpty())
		return false;

	ARectI rcBlock;
	rcBlock.left	= rcGrid.left / m_iBlockGrid;
	rcBlock.top		= rcGrid.top / m_iBlockGrid;
	rcBlock.right	= (rcGrid.right - 1) / m_iBlockGrid;
	rcBlock.bottom	= (rcGrid.bottom - 1) / m_iBlockGrid;

	int r, c;

	//	Ensure all blocks in active area and have been loaded
	for (r=rcBlock.top; r <= rcBlock.bottom; r++)
	{
		for (c=rcBlock.left; c <= rcBlock.right; c++)
		{
			if (!m_pCurActBlocks->rcArea.PtInRect(c, r) ||
				!m_pCurActBlocks->GetBlock(r, c, false))
				return false;
		}
	}

	for (r=rcBlock.top; r <= rcBlock.bottom; r++)
	{
		ARectI rect;
		rect.left	= rcBlock.left * m_iBlockGrid;
		rect.top	= r * m_iBlockGrid;
		rect.right	= rect.left + m_iBlockGrid;
		rect.bottom	= rect.top + m_iBlockGrid;

		for (c=rcBlock.left; c <= rcBlock.right; c++)
		{
			A3DTerrain2Block* pBlock = m_pCurActBlocks->GetBlock(r, c, false);
			ASSERT(pBlock);

			ARectI rc = rcGrid & rect;
			ASSERT(!rc.IsEmpty());

			int dx = rc.left - rcGrid.left;
			int dy = rc.top - rcGrid.top;
			A3DVECTOR3* pDest = pVertBuf + dy * (iGridWid+1) + dx;

			int sx = rc.left - rect.left;
			int sy = rc.top - rect.top;

			//	Remeber the rule: n x n grid has (n+1) x (n+1) vertices.
			pBlock->CopyVertexPos(pDest, rc.Width()+1, rc.Height()+1, iGridWid+1, sx, sy);

			rect.left	+= m_iBlockGrid;
			rect.right	+= m_iBlockGrid;
		}
	}

	return true;
}

/*	Get grid faces of specified area

	Return true for success, otherwise return false.

	vCenter: center of rectangle area
	iGridWid, iGridLen: rectangle area size in grid number
	pVertBuf: buffer used to store face vertices. Note, pVertBuf must can store
			at least (iGridWid+1) * (iGridLen+1) vertices, because n x n grids have
			(n+1) x (n+1) vertices.
	pIdxBuf: buffer used to store face indices. n x n grids have n x n x 6 indices
*/
bool A3DTerrain2::GetFacesOfArea(const A3DVECTOR3& vCenter, int iGridWid, int iGridLen,
								 A3DVECTOR3* pVertBuf, WORD* pIdxBuf)
{
	if (m_pCurActBlocks->rcArea.IsEmpty())
		return false;
	
	float fInvGridSize = 1.0f / m_fGridSize;

	ARectI rcGrid;
	rcGrid.left		= (int)((vCenter.x - m_rcTerrain.left) * fInvGridSize) - (iGridWid >> 1);
	rcGrid.top		= (int)((m_rcTerrain.top - vCenter.z) * fInvGridSize) - (iGridLen >> 1);
	rcGrid.right	= rcGrid.left + iGridWid;
	rcGrid.bottom	= rcGrid.top + iGridLen;

	return GetFacesOfArea(rcGrid, pVertBuf, pIdxBuf);
}


/*	Get grid faces of specified area

	Return true for success, otherwise return false.

	rcGridArea: area represented by grids, the rightmost column and bottommost row won't be used
	pVertBuf: buffer used to store face vertices. Note, pVertBuf must can store
			at least (rcGridArea.Width()+1) * (rcGridArea.Height()+1) vertices, because n x n grids have
			(n+1) x (n+1) vertices.
	pIdxBuf: buffer used to store face indices. n x n grids have n x n x 6 indices
*/
bool A3DTerrain2::GetFacesOfArea(const ARectI& rcGridArea, A3DVECTOR3* pVertBuf, WORD* pIdxBuf)
{
	if (m_pCurActBlocks->rcArea.IsEmpty())
		return false;
	
	float fInvGridSize = 1.0f / m_fGridSize;

	ARectI rcGrid = rcGridArea;

	int iMaxGrid = m_iNumAllBlockCol * m_iBlockGrid;
	a_Clamp(rcGrid.left, 0, iMaxGrid);
	a_Clamp(rcGrid.right, 0, iMaxGrid);
	iMaxGrid = m_iNumAllBlockRow * m_iBlockGrid;
	a_Clamp(rcGrid.top, 0, iMaxGrid);
	a_Clamp(rcGrid.bottom, 0, iMaxGrid);

	if (rcGrid.IsEmpty())
		return false;

	ARectI rcBlock;
	rcBlock.left	= rcGrid.left / m_iBlockGrid;
	rcBlock.top		= rcGrid.top / m_iBlockGrid;
	rcBlock.right	= (rcGrid.right - 1) / m_iBlockGrid;
	rcBlock.bottom	= (rcGrid.bottom - 1) / m_iBlockGrid;

	int r, c;

	//	Ensure all blocks in active area and have been loaded
	for (r=rcBlock.top; r <= rcBlock.bottom; r++)
	{
		for (c=rcBlock.left; c <= rcBlock.right; c++)
		{
			if (!m_pCurActBlocks->rcArea.PtInRect(c, r) ||
				!m_pCurActBlocks->GetBlock(r, c, false))
				return false;
		}
	}

	WORD* pDestIdx = pIdxBuf;

	for (r=rcBlock.top; r <= rcBlock.bottom; r++)
	{
		ARectI rect;
		rect.left	= rcBlock.left * m_iBlockGrid;
		rect.top	= r * m_iBlockGrid;
		rect.right	= rect.left + m_iBlockGrid;
		rect.bottom	= rect.top + m_iBlockGrid;

		for (c=rcBlock.left; c <= rcBlock.right; c++)
		{
			A3DTerrain2Block* pBlock = m_pCurActBlocks->GetBlock(r, c, false);
			ASSERT(pBlock);

			ARectI rc = rcGrid & rect;
			ASSERT(!rc.IsEmpty());

			int dx = rc.left - rcGrid.left;
			int dy = rc.top - rcGrid.top;
			int iBaseVert = dy * (rcGrid.Width() + 1) + dx;
			A3DVECTOR3* pVertDest = pVertBuf + iBaseVert;

			int sx = rc.left - rect.left;
			int sy = rc.top - rect.top;

			pBlock->CopyFaces(pVertDest, pDestIdx, rc.Width(), rc.Height(), rcGrid.Width()+1, iBaseVert, sx, sy);

			pDestIdx	+= rc.Width() * rc.Height() * 6;
			rect.left	+= m_iBlockGrid;
			rect.right	+= m_iBlockGrid;
		}
	}

	return true;
}


