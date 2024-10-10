/*
* FILE: A3DWaterArea.cpp
*
* DESCRIPTION: Class representing for water on the terrain
*
* CREATED BY: liyi, 2011, 04, 12
*
* HISTORY:
*
* Copyright (c) 2004 Archosaur Studio, All Rights Reserved.	
*/

#include "A3DWaterArea.h"
#include "A3DMacros.h"
#include "A3DPI.h"
#include "A3DStream.h"
#include "A3DTerrainWater.h"
#include "A3DDevice.h"
#include "A3DViewport.h"
#include "A3DCameraBase.h"
#include "A3DEngine.h"
#include "AFile.h"
#include "A3DFuncs.h"
#include "A3DCamera.h"

//////////////////////////////////////////////////////////////////////////////////////////
//
//	Implement A3DWaterMeshBlock
//
//////////////////////////////////////////////////////////////////////////////////////////

A3DWaterMeshBlock::A3DWaterMeshBlock() :
m_pWaterArea(NULL),
m_iVisGridNum(0)
{
	m_aabb.Clear();

	memset(m_aIdxStrms, 0, sizeof (m_aIdxStrms));
}

A3DWaterMeshBlock::~A3DWaterMeshBlock()
{
}

//	Get the highest mesh grid dim
int A3DWaterMeshBlock::GetHighestMeshGridDim()
{
	int iDetail = 1 << (MAX_LOD_LEVEL - 1);
	return BLOCK_GRID_DIM * iDetail;
}

//	Intialize object
bool A3DWaterMeshBlock::Init(A3DWaterArea* pWaterArea, int left, int top)
{
	m_pWaterArea = pWaterArea;

	//	Initailize block data
	int aRows[BLOCK_GRID_DIM * BLOCK_GRID_DIM];
	int aCols[BLOCK_GRID_DIM * BLOCK_GRID_DIM];
	if (!InitData(left, top, aRows, aCols))
		return false;

	//	Check if there is any visible grid in this block
	if (!m_iVisGridNum)
		return false;

	//	Create index streams
	if (!CreateStreams(aRows, aCols))
	{
		g_A3DErrLog.Log("A3DWaterMeshBlock::Init, failed to create streams");
		return false;
	}

	return true;
}

//	Release object
void A3DWaterMeshBlock::Release()
{
	int i;

	//	Release index streams
	for (i=0; i < MAX_LOD_LEVEL; i++)
	{
		A3DRELEASE(m_aIdxStrms[i]);
	}
}

//	Initialize block data, return visible grid number
bool A3DWaterMeshBlock::InitData(int left, int top, int* aRows, int* aCols)
{
	const A3DVECTOR3& vAreaCenter = m_pWaterArea->GetCenter();
	float fGridSize = m_pWaterArea->GetGridSize();

	const int iWidth = m_pWaterArea->GetWidth();
	const int iHeight = m_pWaterArea->GetHeight();

	//	Calculate water area's left-top corner's position in world space
	A3DVECTOR3 vecOrg;
	vecOrg.x = vAreaCenter.x - fGridSize * iWidth * 0.5f;
	vecOrg.z = vAreaCenter.z + fGridSize * iHeight * 0.5f;
	vecOrg.y = vAreaCenter.y;

	//	Calculate grids convered by this block
	m_rcBlkGrid.left = left;
	m_rcBlkGrid.top = top;
	m_rcBlkGrid.right = left + BLOCK_GRID_DIM;
	m_rcBlkGrid.bottom = top + BLOCK_GRID_DIM;

	if (m_rcBlkGrid.right > iWidth)
		m_rcBlkGrid.right = iWidth;
	if (m_rcBlkGrid.bottom > iHeight)
		m_rcBlkGrid.bottom = iHeight;

	//	Record visible grids position and calculate block's aabb at same time
	A3DVECTOR3 v1;
	A3DVECTOR3 v2(fGridSize, 0.0f, 0.0f);
	A3DVECTOR3 v3(0.0f, 0.0f, -fGridSize);

	m_aabb.Clear();

	int x, y;
	m_iVisGridNum = 0;

	for (y=top; y < m_rcBlkGrid.bottom; y++)
	{
		const int iBase = y * iWidth;

		v1.x = vecOrg.x + left * fGridSize;
		v1.y = vAreaCenter.y;
		v1.z = vecOrg.z - y * fGridSize;

		for (x=left; x < m_rcBlkGrid.right; x++)
		{
			BYTE byGridFlag = m_pWaterArea->GetGridFlag(x, y);

			if (byGridFlag & WATERGRID_VISIBLE)
			{
				m_aabb.AddVertex(v1);
				m_aabb.AddVertex(v1 + v2);
				m_aabb.AddVertex(v1 + v3);
				m_aabb.AddVertex(v1 + v2 + v3);

				//	Record grid's position
				aRows[m_iVisGridNum] = y;
				aCols[m_iVisGridNum] = x;
				m_iVisGridNum++;
			}

			v1.x += fGridSize;
		}
	}

	m_aabb.CompleteCenterExts();

	//	Calculate block area
	m_rcBlkArea.left = vecOrg.x + left * fGridSize;
	m_rcBlkArea.top = vecOrg.z - top * fGridSize;
	m_rcBlkArea.right = m_rcBlkArea.left + BLOCK_GRID_DIM * fGridSize;
	m_rcBlkArea.bottom = m_rcBlkArea.top - BLOCK_GRID_DIM * fGridSize;

	return true;
}

//	Create stream
bool A3DWaterMeshBlock::CreateStreams(int* aRows, int* aCols)
{
	ASSERT(m_iVisGridNum);

	A3DDevice* pA3DDevice = m_pWaterArea->GetTerrainWater()->GetWaterRender()->GetA3DDevice();

	int i, j;

	//	Create streams
	for (i=0; i < MAX_LOD_LEVEL; i++)
	{
		int iDetail = 1 << (MAX_LOD_LEVEL - i - 1);
		int iGridIndexNum = iDetail * iDetail * 6;
		int iNumIndex = m_iVisGridNum * iGridIndexNum;

		A3DStream* pStream = new A3DStream;
		if (!pStream || !pStream->Init(pA3DDevice, 0, 0, 0, iNumIndex, A3DSTRM_STATIC, A3DSTRM_STATIC))
		{
			g_A3DErrLog.Log("A3DWaterMeshBlock, failed to create index buffer");
			return false;
		}

		m_aIdxStrms[i] = pStream;

		WORD* pIndices = NULL;
		if (!pStream->LockIndexBuffer(0, 0, (BYTE**) &pIndices, 0))
			return false;

		int iIndexCnt = 0;
		int iStepSize = 1 << i;

		//	Fill stream content
		for (j=0; j < m_iVisGridNum; j++)
		{
			//	Calculate grid's row and column in this block
			int r = aRows[j] - m_rcBlkGrid.top;
			int c = aCols[j] - m_rcBlkGrid.left;
			AddIndices(pIndices, iIndexCnt, r, c, iStepSize);
		}

		ASSERT(iIndexCnt == iNumIndex);

		pStream->UnlockIndexBuffer();
	}

	return true;
}

//	Add mesh indices of a grid
void A3DWaterMeshBlock::AddIndices(WORD* aIndices, int& iIdxCnt, int r, int c, int iStepSize)
{
	int iBlkMeshGrid = GetHighestMeshGridDim();
	int iGridIndexNum = iBlkMeshGrid / BLOCK_GRID_DIM;
	int iRowPitch = iBlkMeshGrid + 1;

	int i, j;
	int iBase = r * iGridIndexNum * iRowPitch + c * iGridIndexNum;
	int iStepPitch = iRowPitch * iStepSize;

	for (i=0; i < iGridIndexNum; i+=iStepSize)
	{
		int i0 = iBase + i * iRowPitch;

		for (j=0; j < iGridIndexNum; j+=iStepSize)
		{
			aIndices[iIdxCnt++] = (WORD)(i0);
			aIndices[iIdxCnt++] = (WORD)(i0 + iStepSize);
			aIndices[iIdxCnt++] = (WORD)(i0 + iStepSize + iStepPitch);

			aIndices[iIdxCnt++] = (WORD)(i0);
			aIndices[iIdxCnt++] = (WORD)(i0 + iStepSize + iStepPitch);
			aIndices[iIdxCnt++] = (WORD)(i0 + iStepPitch);

			i0 += iStepSize;
		}
	}
}

//	Get index stream of specified LOD
A3DStream* A3DWaterMeshBlock::GetIndexStream(int iLOD)
{
	ASSERT(iLOD >= 1 && iLOD <= MAX_LOD_LEVEL);
	return m_aIdxStrms[iLOD-1];
}

//////////////////////////////////////////////////////////////////////////////////////////
//
//	water area object
//
//////////////////////////////////////////////////////////////////////////////////////////

A3DWaterArea::A3DWaterArea()
{
	m_pWater		= NULL;
	m_pGridFlags	= NULL;
	m_bMinorWater	= false;
	m_bDoRender		= false;
}

A3DWaterArea::~A3DWaterArea()
{
	Release();
}

bool A3DWaterArea::Init(A3DTerrainWater * pWater, DWORD dwID, int nWidth, int nHeight, float vGridSize, const A3DVECTOR3& vecCenter, float vWaterHeight)
{
	m_pWater		= pWater;
	m_dwAreaID		= dwID;
	m_waterColor	= A3DCOLORRGBA(204, 204, 204, 255);

	m_nWidth		= nWidth;
	m_nHeight		= nHeight;
	m_vGridSize		= vGridSize;
	m_vecCenter		= vecCenter;
	m_vWaterHeight	= vWaterHeight;

	m_vEdgeSize		= 10.0f; // default size of edge

	m_pGridFlags = new BYTE[m_nWidth * m_nHeight];
	memset(m_pGridFlags, 0, sizeof(BYTE) * m_nWidth * m_nHeight);
	return true;
}

bool A3DWaterArea::Release()
{
	RemoveAllWaves();

	int i;

	//	Release all static meshes
	for (i=0; i < m_aMeshBlks.GetSize(); i++)
	{
		A3DRELEASE(m_aMeshBlks[i]);
	}

	if( m_pGridFlags )
	{
		delete [] m_pGridFlags;
		m_pGridFlags = NULL;
	}

	return true;
}

bool A3DWaterArea::Load(A3DTerrainWater * pWater, AFile * pFileToLoad)
{
	m_pWater	= pWater;

	DWORD dwReadLen;
	DWORD dwVersion;

	// first write current version of the area
	if( !pFileToLoad->Read(&dwVersion, sizeof(DWORD), &dwReadLen) )
		goto READFAIL;

	if( dwVersion >= 0x10002000 )
	{
		// then read basic information of the area
		if( !pFileToLoad->Read(&m_dwAreaID, sizeof(DWORD), &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(&m_waterColor, sizeof(A3DCOLOR), &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(&m_vWaterHeight, sizeof(float), &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(&m_vEdgeSize, sizeof(float), &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(&m_nWidth, sizeof(int), &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(&m_nHeight, sizeof(int), &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(&m_vGridSize, sizeof(float), &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(&m_vecCenter, sizeof(A3DVECTOR3), &dwReadLen) )
			goto READFAIL;

		if (dwVersion >= 0x10003000)
		{
			if( !pFileToLoad->Read(&m_bMinorWater, sizeof(bool), &dwReadLen) )
				goto READFAIL;
		}

		m_pGridFlags = new BYTE[m_nWidth * m_nHeight];
		// then read in the grid visible flags
		if( !pFileToLoad->Read(m_pGridFlags, sizeof(BYTE) * m_nWidth * m_nHeight, &dwReadLen) )
			goto READFAIL;

		//	Test code...
	/*	for (int y=0; y < m_nHeight; y++)
		{
			int offset = y * m_nWidth;
			char sz[20];
			for (int x=0; x < m_nWidth; x++)
			{
				sprintf(sz, "%d ", m_pGridFlags[offset+x]);
				OutputDebugStringA(sz);
			}

			OutputDebugStringA("\n");
		}
	*/
		// now load the waves here;
		int nNumWaves = 0;
		if( !pFileToLoad->Read(&nNumWaves, sizeof(int), &dwReadLen) )
			goto READFAIL;
		int	i;
		for(i=0; i<nNumWaves; i++)
		{
			A3DWaterWave * pWave = new A3DWaterWave;
			if( !pWave->Load(this, pFileToLoad) )
			{
				g_A3DErrLog.Log("A3DWaterArea::Load(), failed to load one wave!");
				return false;
			}
			m_theWaves.Add(pWave);
		}
	}
	else if( dwVersion == 0x10001000 )
	{
		// then read basic information of the area
		if( !pFileToLoad->Read(&m_dwAreaID, sizeof(DWORD), &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(&m_waterColor, sizeof(A3DCOLOR), &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(&m_vWaterHeight, sizeof(float), &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(&m_nWidth, sizeof(int), &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(&m_nHeight, sizeof(int), &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(&m_vGridSize, sizeof(float), &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(&m_vecCenter, sizeof(A3DVECTOR3), &dwReadLen) )
			goto READFAIL;

		m_pGridFlags = new BYTE[m_nWidth * m_nHeight];
		// then read in the grid visible flags
		if( !pFileToLoad->Read(m_pGridFlags, sizeof(BYTE) * m_nWidth * m_nHeight, &dwReadLen) )
			goto READFAIL;

		// now load the waves here;
		int nNumWaves = 0;
		if( !pFileToLoad->Read(&nNumWaves, sizeof(int), &dwReadLen) )
			goto READFAIL;
		int	i;
		for(i=0; i<nNumWaves; i++)
		{
			A3DWaterWave * pWave = new A3DWaterWave;
			if( !pWave->Load(this, pFileToLoad) )
			{
				g_A3DErrLog.Log("A3DWaterArea::Load(), failed to load one wave!");
				return false;
			}
			m_theWaves.Add(pWave);
		}
	}
	else if( dwVersion == 0x10000000 )
	{
		// then read basic information of the area
		if( !pFileToLoad->Read(&m_dwAreaID, sizeof(DWORD), &dwReadLen) )
			goto READFAIL;
		m_waterColor = A3DCOLORRGBA(204, 204, 204, 255);
		if( !pFileToLoad->Read(&m_vWaterHeight, sizeof(float), &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(&m_nWidth, sizeof(int), &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(&m_nHeight, sizeof(int), &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(&m_vGridSize, sizeof(float), &dwReadLen) )
			goto READFAIL;
		if( !pFileToLoad->Read(&m_vecCenter, sizeof(A3DVECTOR3), &dwReadLen) )
			goto READFAIL;

		m_pGridFlags = new BYTE[m_nWidth * m_nHeight];
		// then read in the grid visible flags
		if( !pFileToLoad->Read(m_pGridFlags, sizeof(BYTE) * m_nWidth * m_nHeight, &dwReadLen) )
			goto READFAIL;
	}
	else
	{
		g_A3DErrLog.Log("A3DWaterArea::Load(), version [%x] is not supported now!", dwVersion);
		return false;
	}

	//	Update data
	UpdateAll();

	//	Create static mesh if necessary
	if (m_pWater->IsStaticMesh())
	{
		if (!CreateStaticMeshes())
		{
			g_A3DErrLog.Log("A3DWaterArea::Load(), failed to create static meshes !");
			return false;
		}
	}

	return true;

READFAIL:

	g_A3DErrLog.Log("A3DWaterArea::Load(), read from the file failed!");
	return false;
}

bool A3DWaterArea::Save(AFile * pFileToSave)
{
	DWORD dwWriteLen;
	DWORD dwVersion = WATERAREA_VER;

	// first write current version of the area
	if( !pFileToSave->Write(&dwVersion, sizeof(DWORD), &dwWriteLen) )
		goto WRITEFAIL;

	// then write basic information of the area
	if( !pFileToSave->Write(&m_dwAreaID, sizeof(DWORD), &dwWriteLen) )
		goto WRITEFAIL;
	if( !pFileToSave->Write(&m_waterColor, sizeof(A3DCOLOR), &dwWriteLen) )
		goto WRITEFAIL;
	if( !pFileToSave->Write(&m_vWaterHeight, sizeof(float), &dwWriteLen) )
		goto WRITEFAIL;
	if( !pFileToSave->Write(&m_vEdgeSize, sizeof(float), &dwWriteLen) )
		goto WRITEFAIL;
	if( !pFileToSave->Write(&m_nWidth, sizeof(int), &dwWriteLen) )
		goto WRITEFAIL;
	if( !pFileToSave->Write(&m_nHeight, sizeof(int), &dwWriteLen) )
		goto WRITEFAIL;
	if( !pFileToSave->Write(&m_vGridSize, sizeof(float), &dwWriteLen) )
		goto WRITEFAIL;
	if( !pFileToSave->Write(&m_vecCenter, sizeof(A3DVECTOR3), &dwWriteLen) )
		goto WRITEFAIL;
	if( !pFileToSave->Write(&m_bMinorWater, sizeof(m_bMinorWater), &dwWriteLen) )
		goto WRITEFAIL;

	// then write out the grid visible flags
	if( !pFileToSave->Write(m_pGridFlags, sizeof(BYTE) * m_nWidth * m_nHeight, &dwWriteLen) )
		goto WRITEFAIL;

	// now save the waves here;
	int nNumWaves;
	int	i;
	nNumWaves = m_theWaves.GetSize();
	if( !pFileToSave->Write(&nNumWaves, sizeof(int), &dwWriteLen) )
		goto WRITEFAIL;
	for(i=0; i<nNumWaves; i++)
	{
		A3DWaterWave * pWave = m_theWaves[i];
		if( !pWave->Save(pFileToSave) )
		{
			g_A3DErrLog.Log("A3DWaterArea::Save(), failed to save one wave!");
			return false;
		}
	}
	return true;

WRITEFAIL:
	g_A3DErrLog.Log("A3DWaterArea::Save(), Write to the file failed!");
	return false;
}

// copy the wave vertices of this water area into a buffer, and set correct indices buffer
bool A3DWaterArea::CopyWaveToBuffer(A3DWAVEVERTEX * pVerts, WORD * pIndices, int nStartVert)
{
	int nNumWaves = m_theWaves.GetSize();

	int nVertCount = 0;
	int nFaceCount = 0;
	for(int i=0; i<nNumWaves; i++)
	{
		A3DWaterWave * pWave = m_theWaves[i];

		if( !pWave->CopyToBuffer(pVerts + nVertCount, pIndices + nFaceCount * 3, nStartVert + nVertCount) )
		{
			g_A3DErrLog.Log("A3DWaterArea::CopyWaveToBuffer(), failed to call A3DWaterWave::CopyToBuffer()!");
			return false;
		}

		nVertCount += pWave->GetVertCount();
		nFaceCount += pWave->GetFaceCount();
	}

	return true;
}

int A3DWaterArea::GetWaveVertCount()
{
	int nVertCount = 0;
	int nNumWaves = m_theWaves.GetSize();

	for(int i=0; i<nNumWaves; i++)
	{
		A3DWaterWave * pWave = m_theWaves[i];
		nVertCount += pWave->GetVertCount();
	}

	return nVertCount;
}

int A3DWaterArea::GetWaveFaceCount()
{
	int nFaceCount = 0;
	int nNumWaves = m_theWaves.GetSize();

	for(int i=0; i<nNumWaves; i++)
	{
		A3DWaterWave * pWave = m_theWaves[i];
		nFaceCount += pWave->GetFaceCount();
	}

	return nFaceCount;
}

// update area vertex and indices according to new grid flags
bool A3DWaterArea::UpdateAll()
{
	// now update the vertex buffer of all waves
	int nNumWaves = m_theWaves.GetSize();
	for(int i=0; i<nNumWaves; i++)
	{
		A3DWaterWave * pWave = m_theWaves[i];
		if( !pWave->UpdateVB() )
		{
			g_A3DErrLog.Log("A3DWaterArea::UpdateAll(), failed to update wave's VB!");
			return false;
		}
	}

	// now determine the bounding box
	m_aabb.Mins.x = m_vecCenter.x - m_vGridSize * m_nWidth / 2.0f;
	m_aabb.Mins.z = m_vecCenter.z - m_vGridSize * m_nHeight / 2.0f;
	m_aabb.Mins.y = 0.0f;
	m_aabb.Maxs.x = m_vecCenter.x + m_vGridSize * m_nWidth / 2.0f;
	m_aabb.Maxs.z = m_vecCenter.z + m_vGridSize * m_nHeight / 2.0f;
	m_aabb.Maxs.y = 1000.0f;
	m_aabb.CompleteCenterExts();

	//	Build visible grids aabb
	BuildVisGridsAABB();

	return true;
}

void A3DWaterArea::SetGridFlag(int x, int y, BYTE flag)
{
	assert(x >= 0 && x < m_nWidth && y >= 0 && y < m_nHeight);
	m_pGridFlags[y * m_nWidth + x] = flag;

	//	Build visible grids aabb
	BuildVisGridsAABB();
}

void A3DWaterArea::ClearGridFlag(int x, int y, BYTE flag)
{
	assert(x >= 0 && x < m_nWidth && y >= 0 && y < m_nHeight);
	m_pGridFlags[y * m_nWidth + x] &= ~flag;

	//	Build visible grids aabb
	BuildVisGridsAABB();
}

//	Build visible grids aabb
bool A3DWaterArea::BuildVisGridsAABB()
{
	m_aabbVisGrids.Clear();

	A3DVECTOR3 vecOrg;
	vecOrg.x = m_vecCenter.x - m_vGridSize * m_nWidth * 0.5f;
	vecOrg.z = m_vecCenter.z + m_vGridSize * m_nHeight * 0.5f;
	vecOrg.y = m_vecCenter.y;

	int x, y;
	A3DVECTOR3 v1;
	A3DVECTOR3 v2(m_vGridSize, 0.0f, 0.0f);
	A3DVECTOR3 v3(0.0f, 0.0f, -m_vGridSize);

	for (y=0; y < m_nHeight; y++)
	{
		const int iBase = y * m_nWidth;

		v1.x = vecOrg.x;
		v1.y = m_vecCenter.y;
		v1.z = vecOrg.z - y * m_vGridSize;

		for (x=0; x < m_nWidth; x++)
		{
			if (m_pGridFlags[iBase+x] & WATERGRID_VISIBLE)
			{
				m_aabbVisGrids.AddVertex(v1);
				m_aabbVisGrids.AddVertex(v1 + v2);
				m_aabbVisGrids.AddVertex(v1 + v3);
				m_aabbVisGrids.AddVertex(v1 + v2 + v3);
			}

			v1.x += m_vGridSize;
		}
	}

	m_aabbVisGrids.CompleteCenterExts();

	return true;
}

// adjust the water area's width and height, after adjusting, all grid are invisible
bool A3DWaterArea::AdjustWidthHeight(int nWidth, int nHeight)
{
	if( m_pGridFlags )
	{
		delete [] m_pGridFlags;
		m_pGridFlags = NULL;
	}

	m_nWidth = nWidth;
	m_nHeight = nHeight;

	m_pGridFlags = new BYTE[m_nWidth * m_nHeight];
	memset(m_pGridFlags, 0, sizeof(BYTE) * m_nWidth * m_nHeight);
	return true;
}

bool A3DWaterArea::ShiftCenterTo(const A3DVECTOR3& vecCenter)
{
	A3DVECTOR3 vecDelta = vecCenter - m_vecCenter;
	m_vecCenter = vecCenter;

	int nNumWaves = m_theWaves.GetSize();
	for(int i=0; i<nNumWaves; i++)
	{
		A3DWaterWave * pWave = m_theWaves[i];
		if( !pWave->SetDeltaPos(vecDelta) )
		{
			g_A3DErrLog.Log("A3DWaterArea::ShiftTo(), failed to call wave's SetDeltaPos!");
			return false;
		}
	}

	UpdateAll();
	return true;
}

// wave manipulations
bool A3DWaterArea::AddWave(float vEdgeSize, int nMaxSegment, A3DWaterWave ** ppWave)
{
	A3DWaterWave * pWave = new A3DWaterWave;
	if( !pWave->Init(this, vEdgeSize, nMaxSegment) )
	{
		g_A3DErrLog.Log("A3DWaterArea::AddWave(), failed to call A3DWaterWave::Init()!");
		return false;
	}

	m_theWaves.Add(pWave);
	m_vEdgeSize = vEdgeSize;

	*ppWave = pWave;
	return true;
}

bool A3DWaterArea::DeleteWave(int nIndex)
{
	A3DWaterWave * pWave = m_theWaves[nIndex];

	pWave->Release();
	delete pWave;

	m_theWaves.RemoveAt(nIndex);
	return true;
}

bool A3DWaterArea::RemoveAllWaves()
{
	int nNumWaves = m_theWaves.GetSize();

	for(int i=0; i<nNumWaves; i++)
	{
		A3DWaterWave * pWave = m_theWaves[i];

		pWave->Release();
		delete pWave;
	}

	m_theWaves.RemoveAll();
	return true;
}

//	Check if specified position is in a visible grid
bool A3DWaterArea::IsInVisbleGrid(const A3DVECTOR3& vecPos)
{
	int nX = (int)((vecPos.x - m_vecCenter.x) / m_vGridSize + m_nWidth / 2.0f);
	int nY = (int)((m_vecCenter.z - vecPos.z) / m_vGridSize + m_nHeight / 2.0f);

	if (nX >= m_nWidth || nX < 0 ||
		nY >= m_nHeight || nY < 0)
		return false;

	return m_pGridFlags[nY * m_nWidth + nX] ? true : false;
}

//	Create static meshes
bool A3DWaterArea::CreateStaticMeshes()
{
	int r, c;

	for (r=0; r < m_nHeight; r+=A3DWaterMeshBlock::BLOCK_GRID_DIM)
	{
		for (c=0; c < m_nWidth; c+=A3DWaterMeshBlock::BLOCK_GRID_DIM)
		{
			A3DWaterMeshBlock* pMeshBlk = new A3DWaterMeshBlock;
			if (!pMeshBlk->Init(this, c, r))
			{
				A3DRELEASE(pMeshBlk);
				continue;
			}

			m_aMeshBlks.Add(pMeshBlk);
		}
	}

	return true;
}

//	Draw static meshes
bool A3DWaterArea::DrawStaticMeshes(A3DViewport* pViewport, float fViewDist, A3DCamera* pReflectCamera)
{
	A3DCameraBase* pCamera = pViewport->GetCamera();
	A3DDevice* pA3DDevice = m_pWater->GetWaterRender()->GetA3DDevice();
	const A3DVECTOR3& vEyePos = pCamera->GetPos();
	
	for (int i=0; i < m_aMeshBlks.GetSize(); i++)
	{
		A3DWaterMeshBlock* pMeshBlk = m_aMeshBlks[i];
		const A3DAABB& aabbBlk = pMeshBlk->GetMeshAABB();

 		if (!pCamera->AABBInViewFrustum(aabbBlk.Mins, aabbBlk.Maxs))
 			continue;

		//Calcuate distance
		float fBlkRadius = aabbBlk.Extents.Magnitude();
		A3DVECTOR3 vDelta = vEyePos - aabbBlk.Center;
		float fDist = vDelta.MagnitudeH() - fBlkRadius;
		if (fDist >= fViewDist)
			continue;	//	Block is out of view sight

		//	Decide LOD level
		fDist = vDelta.Magnitude() - fBlkRadius;
		int iLOD = 3;
		if (fDist < 64.0f)
			iLOD = 1;
		else if (fDist < 128.0f)
			iLOD = 2;

		//	Upload mesh block parameters
		const ARectF& rcBlkArea = pMeshBlk->GetBlockArea();
		A3DVECTOR4 c34(rcBlkArea.left, m_vWaterHeight, rcBlkArea.top, m_vGridSize);
		pA3DDevice->SetVertexShaderConstants(34, &c34, 1);

		//set refelect matrix
		A3DMATRIX4 matScale(A3DMATRIX4::IDENTITY);
		matScale._11 = 0.5f;
		matScale._22 = -0.5f;
		matScale._41 = 0.5f;
		matScale._42 = 0.5f;
		A3DMATRIX4 matProjectedView;

		A3DMATRIX4 matVP = pReflectCamera ? pReflectCamera->GetVPTM() : pCamera->GetVPTM();
		matProjectedView = matVP * matScale;
		matProjectedView.Transpose();
		pA3DDevice->SetVertexShaderConstants(14, &matProjectedView, 4);
		
		//	Apply index stream
		A3DStream* pIdxStream = pMeshBlk->GetIndexStream(iLOD);
		pIdxStream->AppearIndexOnly();

		int iPrimCnt = pIdxStream->GetIndexCount() / 3;
		int iVertCnt = m_pWater->GetWaterRender()->GetMaxVertNum();
		pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, iVertCnt, 0, iPrimCnt);
		
	}

	return true;
}

bool A3DWaterArea::IsUnderWater(const A3DVECTOR3& vecPos)
{
	int nX = (int)((vecPos.x - m_vecCenter.x) / m_vGridSize + m_nWidth / 2.0f);
	int nY = (int)((m_vecCenter.z - vecPos.z) / m_vGridSize + m_nHeight / 2.0f);
	
	if( nX >= m_nWidth ) nX = m_nWidth - 1;
	else if( nX < 0 ) nX = 0;
	if( nY >= m_nHeight ) nY = m_nHeight - 1;
	else if( nY < 0 ) nY = 0;
	
	if( m_pGridFlags[nY * m_nWidth + nX] )
		return true;
	else
		return false;
}

