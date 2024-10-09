/*
 * FILE: TerrainSquare.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2006/7/21
 *
 * HISTORY: 
 *
 * Copyright (c) 2006 Archosaur Studio, All Rights Reserved.
 */

#include "Global.h"
#include <float.h>
#include "TerrainSquare.h"
#include "Render.h"

#include <A3DTerrain2.h>
#include <A3DTerrain2Blk.h>
#include <A3DTerrain2LOD.h>
#include <A3DTerrain2CullHC.h>
#include <A3DMacros.h>

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
//	Implement CBlockSquare
//	
///////////////////////////////////////////////////////////////////////////

CBlockSquare::CBlockSquare(CTerrainSquare* pTrnSquare)
{
	m_pTrnSquare	= pTrnSquare;
	m_pBlock		= NULL;
}

CBlockSquare::~CBlockSquare()
{
}

//	Build block square from terrain2 block data
bool CBlockSquare::Build(A3DTerrain2Block* pBlock)
{
	m_pBlock = pBlock;

	//	Calculate block offset
	CalcBlockOffset();

	for (int i=0; i < 4; i++)
	{
		//	Calculate least square sum
		CalcLeastSquareSum(i);

		//	Calculate least square line
		CalcLeastSquareLine(i);

		//	Calculate edge
		CalcEdge(i);
	}

	return true;
}

//	Calculate offset between block's center to origin
void CBlockSquare::CalcBlockOffset()
{
	A3DTerrain2* pTerrain = m_pBlock->GetTerrain();
	float fBlockSize = m_pBlock->GetBlockSize();

	const ARectF& rcTerrain = pTerrain->GetTerrainArea();
	m_vBlkOff.x = rcTerrain.left + m_pBlock->GetColInTerrain() * fBlockSize + fBlockSize * 0.5f;
	m_vBlkOff.z = rcTerrain.top - m_pBlock->GetRowInTerrain() * fBlockSize - fBlockSize * 0.5f;
	m_vBlkOff.y = 0.0f;
}

//	Calculate least square sum
void CBlockSquare::CalcLeastSquareSum(int iEdge)
{
	m_Sum.Clear();

    const int n = m_pBlock->GetBlockGrid() + 1;
    m_Sum.samples = (float)n;

	//	Note: vertices in block are not ordered from top to bottom, from 
	//		left to right. Here we will re-map vertex index
	WORD* aIndexMaps = m_pBlock->GetTerrain()->GetLODManager()->GetIndexMaps();

	//	Get block offset
	int iStart, iStep;

	if (iEdge == EDGE_LEFT)
	{
		iStart	= 0;
		iStep	= n;
	}
	else if (iEdge == EDGE_TOP)
	{
		iStart	= 0;
		iStep	= 1;
	}
	else if (iEdge == EDGE_RIGHT)
	{
		iStart	= n - 1;
		iStep	= n;
	}
	else	//	iEdige == EDGE_BOTTOM
	{
		iStart	= n * (n - 1);
		iStep	= 1;
	}

	if (iEdge == EDGE_LEFT || iEdge == EDGE_RIGHT)
	{
		int i, cnt = iStart;
		for (i=0; i < n; i++, cnt+=iStep)
		{
			int iIndex = aIndexMaps[cnt];
			A3DVECTOR3 v = m_pBlock->GetVertexPos(iIndex);
			v.x -= m_vBlkOff.x;
			v.z -= m_vBlkOff.z;

			m_Sum.x += v.z;
			m_Sum.y += v.y;
			m_Sum.xx += v.z * v.z;
			m_Sum.xy += v.z * v.y;
		}
	}
	else
	{
		int i, cnt = iStart;
		for (i=0; i < n; i++, cnt+=iStep)
		{
			int iIndex = aIndexMaps[cnt];
			A3DVECTOR3 v = m_pBlock->GetVertexPos(iIndex);
			v.x -= m_vBlkOff.x;
			v.z -= m_vBlkOff.z;

			m_Sum.x += v.x;
			m_Sum.y += v.y;
			m_Sum.xx += v.x * v.x;
			m_Sum.xy += v.x * v.y;
		}
	}
}

//	Calculate lease square line
void CBlockSquare::CalcLeastSquareLine(int iEdge)
{
	float fDenominator = 1.0f / (m_Sum.xx * m_Sum.samples - m_Sum.x * m_Sum.x);
	m_Line.a = (m_Sum.xy * m_Sum.samples - m_Sum.x * m_Sum.y) * fDenominator;
	m_Line.c = (m_Sum.y * m_Sum.xx - m_Sum.x * m_Sum.xy) * fDenominator;

    const int n = m_pBlock->GetBlockGrid() + 1;

	//	Note: vertices in block are not ordered from top to bottom, from 
	//		left to right. Here we will re-map vertex index
	WORD* aIndexMaps = m_pBlock->GetTerrain()->GetLODManager()->GetIndexMaps();

	//	Get block offset
	int iStart, iStep;

	if (iEdge == EDGE_LEFT)
	{
		iStart	= 0;
		iStep	= n;
	}
	else if (iEdge == EDGE_TOP)
	{
		iStart	= 0;
		iStep	= 1;
	}
	else if (iEdge == EDGE_RIGHT)
	{
		iStart	= n - 1;
		iStep	= n;
	}
	else	//	iEdige == EDGE_BOTTOM
	{
		iStart	= n * (n - 1);
		iStep	= 1;
	}

	m_Line.e = FLT_MAX;

	if (iEdge == EDGE_LEFT || iEdge == EDGE_RIGHT)
	{
		int i, cnt = iStart;
		for (i=0; i < n; i++, cnt+=iStep)
		{
			int iIndex = aIndexMaps[cnt];
			A3DVECTOR3 v = m_pBlock->GetVertexPos(iIndex);
			v.x -= m_vBlkOff.x;
			v.z -= m_vBlkOff.z;

			float y = v.z * m_Line.a + m_Line.c;
			float e = v.y - y;

			if (e < m_Line.e)
				m_Line.e = e;
		}
	}
	else
	{
		int i, cnt = iStart;
		for (i=0; i < n; i++, cnt+=iStep)
		{
			int iIndex = aIndexMaps[cnt];
			A3DVECTOR3 v = m_pBlock->GetVertexPos(iIndex);
			v.x -= m_vBlkOff.x;
			v.z -= m_vBlkOff.z;

			float y = v.x * m_Line.a + m_Line.c;
			float e = v.y - y;

			if (e < m_Line.e)
				m_Line.e = e;
		}
	}
}

//	Calculate edge
void CBlockSquare::CalcEdge(int iEdge)
{
	A3DAABB aabb = m_pBlock->GetBlockAABB();
	aabb.Mins.x -= m_vBlkOff.x;
	aabb.Maxs.x -= m_vBlkOff.x;
	aabb.Mins.z -= m_vBlkOff.z;
	aabb.Maxs.z -= m_vBlkOff.z;
	aabb.CompleteCenterExts();

	EDGE& edge = m_aEdges[iEdge];

	if (iEdge == EDGE_LEFT)
	{
		float y1 = aabb.Maxs.z * m_Line.a + m_Line.c + m_Line.e;
		float y2 = aabb.Mins.z * m_Line.a + m_Line.c + m_Line.e;
		edge.v1.Set(aabb.Mins.x, y1, aabb.Maxs.z);
		edge.v2.Set(aabb.Mins.x, y2, aabb.Mins.z);
	}
	else if (iEdge == EDGE_TOP)
	{
		float y1 = aabb.Mins.x * m_Line.a + m_Line.c + m_Line.e;
		float y2 = aabb.Maxs.x * m_Line.a + m_Line.c + m_Line.e;
		edge.v1.Set(aabb.Mins.x, y1, aabb.Maxs.z);
		edge.v2.Set(aabb.Maxs.x, y2, aabb.Maxs.z);
	}
	else if (iEdge == EDGE_RIGHT)
	{
		float y1 = aabb.Maxs.z * m_Line.a + m_Line.c + m_Line.e;
		float y2 = aabb.Mins.z * m_Line.a + m_Line.c + m_Line.e;
		edge.v1.Set(aabb.Maxs.x, y1, aabb.Maxs.z);
		edge.v2.Set(aabb.Maxs.x, y2, aabb.Mins.z);
	}
	else	//	iEdige == EDGE_BOTTOM
	{
		float y1 = aabb.Mins.x * m_Line.a + m_Line.c + m_Line.e;
		float y2 = aabb.Maxs.x * m_Line.a + m_Line.c + m_Line.e;
		edge.v1.Set(aabb.Mins.x, y1, aabb.Mins.z);
		edge.v2.Set(aabb.Maxs.x, y2, aabb.Mins.z);
	}
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CTerrainSquare
//	
///////////////////////////////////////////////////////////////////////////

CTerrainSquare::CTerrainSquare()
{
	m_fStepSize	= 512.0f;
	m_pProgress	= NULL;
}

CTerrainSquare::~CTerrainSquare()
{
}

//	Release object
void CTerrainSquare::Release()
{
	int i;

	//	Release all squares
	for (i=0; i < m_aBlkSquares.GetSize(); i++)
	{
		if (m_aBlkSquares[i])
			delete m_aBlkSquares[i];
	}

	m_aBlkSquares.RemoveAll();

	A3DRELEASE(m_pTerrain);
}

//	Build terrain squares from terrain2 file
bool CTerrainSquare::Build(const char* szFile, CProgressCtrl* pProgress/* NULL */)
{
	if (!LoadTerrain(szFile))
		return false;

	ResetBlockSquares();
		
	m_pProgress = pProgress;
	if (m_pProgress)
	{
		m_pProgress->SetRange(0, m_pTerrain->GetBlockNum());
		m_pProgress->SetStep(1);
		m_pProgress->SetPos(0);
	}

	const ARectF& rcTerrain = m_pTerrain->GetTerrainArea();
	float z = rcTerrain.top - m_fStepSize;

	for ( ; ; z -= m_fStepSize)
	{
		float x = rcTerrain.left + m_fStepSize;

		for ( ; ; x += m_fStepSize)
		{
			A3DVECTOR3 vCenter(x, 0.0f, z);
			m_pTerrain->SetLoadCenter(vCenter);

			//	Build block squares in current active area
			BuildBlockSquares();

			if (x + m_fStepSize >= rcTerrain.right)
				break;
		}

		if (z - m_fStepSize <= rcTerrain.bottom)
			break;
	}

	//	Ensure all block squares have been built
	for (int i=0; i < m_aBlkSquares.GetSize(); i++)
	{
		if (!m_aBlkSquares[i])
		{
			ASSERT(m_aBlkSquares[i]);
			return false;
		}
	}

	return true;
}

//	Load terrain
bool CTerrainSquare::LoadTerrain(const char* szFile)
{
	m_pTerrain = new A3DTerrain2;

	if (!m_pTerrain->Init(g_Render.GetA3DEngine(), m_fStepSize, m_fStepSize))
	{
		delete m_pTerrain;
		a_LogOutput(1, "CTerrainSquare::LoadTerrain", "Failed to initialize terrain");
		return false;
	}

	if (!m_pTerrain->Load(szFile, 0.0f, 0.0f))
	{
		m_pTerrain->Release();
		delete m_pTerrain;
		a_LogOutput(1, "CTerrainSquare::LoadTerrain, Failed to load A3DTerrain2.");
		return false;
	}

	return true;
}

//	Reset block squares
void CTerrainSquare::ResetBlockSquares()
{
	int i;

	//	Release all squares
	for (i=0; i < m_aBlkSquares.GetSize(); i++)
	{
		if (m_aBlkSquares[i])
			delete m_aBlkSquares[i];
	}

	m_aBlkSquares.RemoveAll();

	int iNumBlock = m_pTerrain->GetBlockNum();
	m_aBlkSquares.SetSize(iNumBlock, 32);

	for (i=0; i < iNumBlock; i++)
		m_aBlkSquares[i] = NULL;
}

//	Build block squares in current active area
bool CTerrainSquare::BuildBlockSquares()
{
	A3DTerrain2::ACTBLOCKS* pActArea = m_pTerrain->GetActiveBlocks();
	const ARectI& rcArea = pActArea->rcArea;
	int r, c;

	for (r=rcArea.top; r < rcArea.bottom; r++)
	{
		for (c=rcArea.left; c < rcArea.right; c++)
		{
			int iIndex = r * m_pTerrain->GetBlockColNum() + c;
			if (m_aBlkSquares[iIndex])
				continue;

			CBlockSquare* pBlockSquare = new CBlockSquare(this);
			if (!pBlockSquare->Build(pActArea->GetBlock(r, c, false)))
			{
				delete pBlockSquare;
				a_LogOutput(1, "CTerrainSquare::BuildBlockSquares, Failed to build block (%d, %d) square.", r, c);
				return false;
			}

			m_aBlkSquares[iIndex] = pBlockSquare;

			if (m_pProgress)
				m_pProgress->StepIt();
		}
	}

	return true;
}

//	Save square data to file
bool CTerrainSquare::Save(const char* szFile)
{
	FILE* fp = fopen(szFile, "wb+");
	if (!fp)
	{
		a_LogOutput(1, "CTerrainSquare::Save, Failed to create file %s.", szFile);
		return false;
	}

	//	Write file flag and version
	DWORD dwTemp = TRN2HCFILE_IDENTIFY;
	fwrite(&dwTemp, sizeof (dwTemp), 1, fp);
	dwTemp = TRN2HCFILE_VERSION;
	fwrite(&dwTemp, sizeof (dwTemp), 1, fp);

	//	Write file header
	TRN2HCFILEHEADER fh;
	memset(&fh, 0, sizeof (fh));
	fh.iNumBlockRow	= m_pTerrain->GetBlockRowNum();
	fh.iNumBlockCol	= m_pTerrain->GetBlockColNum();
	fh.iNumEdge = (fh.iNumBlockRow * fh.iNumBlockCol * 2) + fh.iNumBlockRow + fh.iNumBlockCol;
	fwrite(&fh, sizeof (fh), 1, fp);

	//	Emit edges
	AArray<TRN2HCFILEEDGE, TRN2HCFILEEDGE&> aEdges;
	aEdges.SetSize(fh.iNumEdge, 16);

	int r, c, cnt=0, cnt2=0;
	for (r=0; r < fh.iNumBlockRow; r++)
	{
		//	Top edges
		cnt = r * fh.iNumBlockCol;
		cnt2 = r * (fh.iNumBlockCol + fh.iNumBlockCol + 1);
		for (c=0; c < fh.iNumBlockCol; c++)
		{
			TRN2HCFILEEDGE& edge = aEdges[cnt2++];
			CBlockSquare* pBlkSquare = m_aBlkSquares[cnt++];
			const CBlockSquare::EDGE& src = pBlkSquare->GetEdges()[CBlockSquare::EDGE_TOP];

			edge.y1 = src.v1.y;
			edge.y2 = src.v2.y;
		}

		//	Left edges
		cnt = r * fh.iNumBlockCol;
		cnt2 = r * (fh.iNumBlockCol + fh.iNumBlockCol + 1) + fh.iNumBlockCol;
		for (c=0; c < fh.iNumBlockCol; c++)
		{
			TRN2HCFILEEDGE& edge = aEdges[cnt2++];
			CBlockSquare* pBlkSquare = m_aBlkSquares[cnt++];
			const CBlockSquare::EDGE& src = pBlkSquare->GetEdges()[CBlockSquare::EDGE_LEFT];

			edge.y1 = src.v1.y;
			edge.y2 = src.v2.y;
		}

		//	The last right edge of this row
		TRN2HCFILEEDGE& edge = aEdges[cnt2];
		CBlockSquare* pBlkSquare = m_aBlkSquares[cnt-1];
		const CBlockSquare::EDGE& src = pBlkSquare->GetEdges()[CBlockSquare::EDGE_RIGHT];

		edge.y1 = src.v1.y;
		edge.y2 = src.v2.y;
	}

	//	The last bottom edges
	cnt = (fh.iNumBlockRow-1) * fh.iNumBlockCol;
	cnt2 = fh.iNumBlockRow * (fh.iNumBlockCol + fh.iNumBlockCol + 1);
	for (c=0; c < fh.iNumBlockCol; c++)
	{
		TRN2HCFILEEDGE& edge = aEdges[cnt2++];
		CBlockSquare* pBlkSquare = m_aBlkSquares[cnt++];
		const CBlockSquare::EDGE& src = pBlkSquare->GetEdges()[CBlockSquare::EDGE_BOTTOM];

		edge.y1 = src.v1.y;
		edge.y2 = src.v2.y;
	}

	//	Write edges
	fwrite(aEdges.GetData(), 1, aEdges.GetSize() * sizeof (TRN2HCFILEEDGE), fp);

	fclose(fp);
	return true;
}


