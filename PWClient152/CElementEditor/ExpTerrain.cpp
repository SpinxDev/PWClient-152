/*
 * FILE: ExpTerrain.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/7/2
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "Global.h"
#include "EL_Archive.h"
#include "ExpTerrain.h"
#include "TerrainLayer.h"
#include "ExpElementMap.h"
#include "Bitmap.h"
#include "TerrainRender.h"

#include "AF.h"
#include "A3D.h"

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
//	Implement CExpTerrain
//	
///////////////////////////////////////////////////////////////////////////

CExpTerrain::CExpTerrain(CElementMap* pMap) : CTerrain(pMap)
{
	m_iLoadFlag		= LOAD_EXPTERRAIN;
	m_iTerrainBlock	= 0;
}

CExpTerrain::~CExpTerrain()
{
}

//	Release object
void CExpTerrain::Release()
{
	//	Release mask bitmap array
	m_aMaskBmps.RemoveAll();

	//	Release blocks
	m_aBlocks.RemoveAll();

	//	Release terrain mesh
	CTerrain::Release();
}

//	Load data
bool CExpTerrain::Load(CELArchive& ar)
{
	//	Read terrain information
	TERRAINFILEDATA Data;
	ar.Read(&Data, sizeof (Data));

	m_fTileSize		= Data.fTileSize;
	m_fMaxHeight	= Data.fMaxHeight;
	m_fYOffset		= Data.fYOffset;
	m_dwRndUpdate1	= Data.dwRndUpdate1;

	if (m_iLoadFlag == LOAD_EXPTERRAIN)
	{
		//	Load height map
		if (!LoadHeightMap(ar, Data))
		{
			g_Log.Log("CExpTerrain::Load, Failed to load height map !");
			return false;
		}

		if (!LoadLayers(ar, Data))
		{
			g_Log.Log("CExpTerrain::Load, Failed to load layers !");
			return false;
		}

		//	Release auto mask maps
		for (int i=0; i < m_aLayers.GetSize(); i++)
		{
			CTerrainLayer* pLayer = m_aLayers[i];

			if (pLayer->IsDefault())
				m_aMaskBmps.Add(NULL);
			else
				m_aMaskBmps.Add(pLayer->GetMaskBitmap());
		}
	}
	else if (m_iLoadFlag == LOAD_EXPLIGHTMAP)
	{
		//	Load height map
		if (!LoadHeightMap(ar, Data))
		{
			g_Log.Log("CExpTerrain::Load, Failed to load height map !");
			return false;
		}

		//	Create terrain mesh
		if (!CreateTerrainMesh())
		{
			g_Log.Log("CExpTerrain::Load, Failed to create terrain mesh !");
			return false;
		}

		//	Create blocks
		if (!CreateBlocks())
		{
			g_Log.Log("CExpTerrain::Load, Failed to call CreateBlocks !");
			return false;
		}

		if (m_HeightMap.pHeightData)
		{
			if (!ReloadRender(false))
			{
				g_Log.Log("CExpTerrain::Load, Failed to reload render !");
				return false;
			}

			m_pTrnRender->SetLODType(A3DTerrain2::LOD_NONE);

		//	Release height map data but remain it's size information
		//	a_free(m_HeightMap.pHeightData);
		//	m_HeightMap.pHeightData = NULL;
		}
	}
	else if (m_iLoadFlag == LOAD_EXPSCENE)
	{
		if (!ExpScene_LoadHeighMap(ar, Data))
		{
			g_Log.Log("CExpTerrain::Load, Failed to load height map !");
			return false;
		}
	}

	return true;
}

//	Load height map for exporting scene (when m_iLoadFlag == LOAD_EXPSCENE)
bool CExpTerrain::ExpScene_LoadHeighMap(CELArchive& ar, const TERRAINFILEDATA& Data)
{
	//	Read height map file name
	CString strFile = ar.ReadString();

	//	Try to load height map
	if (Data.iHMWidth <= 0 || Data.iHMHeight <= 0 || !strFile.GetLength())
		return true;

	//	Release old height map
	ReleaseHeightMap();

	if (!CreateEmptyHeightMap(Data.iHMWidth, Data.iHMHeight))
	{
		g_Log.Log("CExpTerrain::ExpScene_LoadHeighMap, Failed to create empty height map !");
		return false;
	}

	//	Make full file name
	CString strFullFile;
	strFullFile.Format("%s%s\\%s", g_szWorkDir, m_pMap->GetResDir(), strFile);

	FILE* fp = fopen(strFullFile, "rb");
	if (!fp)
	{
		g_Log.Log("CExpTerrain::ExpScene_LoadHeighMap, Failed to open height map file %s !", strFullFile);
		return false;
	}

	int iSize = Data.iHMWidth * Data.iHMHeight * sizeof (float);
	if (fread(m_HeightMap.pHeightData, 1, iSize, fp) != iSize)
	{
		fclose(fp);
		g_Log.Log("CExpTerrain::ExpScene_LoadHeighMap, Failed to read height map data. %s !", strFullFile);
		return false;
	}

	fclose(fp);

	//	Calculate terrain area
	CalcTerrainArea();

	return true;
}

/*	Get mask area information

	rcMap: mask area in terrain grids
*/
CExpTerrain::MASKMAPDATA* CExpTerrain::GetMaskAreaInfo(const ARectI& rcMap)
{
	MASKMAPDATA* aMaskData = new MASKMAPDATA [m_aLayers.GetSize()];
	if (!aMaskData)
	{
		g_Log.Log("CExpTerrain::GetMaskAreaInfo, Not enough memory !");
		return false;
	}

	int iDivWid = (m_HeightMap.iWidth - 1) / rcMap.Width();
	int iDivHei = (m_HeightMap.iHeight - 1) / rcMap.Height();
	float ls = (float)rcMap.left / (m_HeightMap.iWidth - 1);
	float ts = (float)rcMap.top / (m_HeightMap.iHeight - 1);

	for (int i=0; i < m_aLayers.GetSize(); i++)
	{
		CTerrainLayer* pLayer = m_aLayers[i];
		CELBitmap* pMaskBmp = m_aMaskBmps[i];
		MASKMAPDATA& Data = aMaskData[i];

		Data.iLayer = i;

		if (!pMaskBmp)
		{
			Data.pData		= NULL;
			Data.iWidth		= 0;
			Data.iHeight	= 0;
			Data.iPitch		= 0;
		}
		else
		{
			Data.iWidth		= pMaskBmp->GetWidth() / iDivWid;
			Data.iHeight	= pMaskBmp->GetHeight() / iDivHei;
			Data.iPitch		= pMaskBmp->GetPitch();

			int x = (int)(ls * pMaskBmp->GetWidth() + 0.5f);
			int y = (int)(ts * pMaskBmp->GetHeight() + 0.5f);
			
			Data.pData = pMaskBmp->GetBitmapData() + y * pMaskBmp->GetPitch() + x;
		}
	}

	return aMaskData;
}

//	Release mask area information
void CExpTerrain::ReleaseMaskAreaInfo(MASKMAPDATA* aMaskData)
{
	delete [] aMaskData;
}

//	Check whether a mask map is pure black
bool CExpTerrain::IsPureBlackMask(const MASKMAPDATA& MaskData)
{
	CTerrainLayer* pLayer = m_aLayers[MaskData.iLayer];
	if (pLayer->IsDefault())
		return false;

	if (!MaskData.pData || !pLayer->IsEnable())
		return true;

	BYTE* pLineData = MaskData.pData;

	for (int i=0; i < MaskData.iHeight; i++)
	{
		BYTE* pPixel = pLineData;
		for (int j=0; j < MaskData.iWidth; j++, pPixel++)
		{
			if (*pPixel)
				return false;
		}

		pLineData += MaskData.iPitch;
	}

	return true;
}

//	Get the first pure white mask layer
int CExpTerrain::GetFirstPureWriteLayer(const MASKMAPDATA* aMaskData)
{
	for (int iLayer=m_aLayers.GetSize()-1; iLayer > 0; iLayer--)
	{
		CTerrainLayer* pLayer = m_aLayers[iLayer];
		const MASKMAPDATA& MaskData = aMaskData[iLayer];

		if (!MaskData.pData || !pLayer->IsEnable())
			continue;

		BYTE* pLineData = MaskData.pData;

		for (int i=0; i < MaskData.iHeight; i++)
		{
			BYTE* pPixel = pLineData;
			for (int j=0; j < MaskData.iWidth; j++, pPixel++)
			{
				//	We allow a little color error by using 250 instead of 255
				if (*pPixel < 250)
					goto NextLayer;
			}

			pLineData += MaskData.iPitch;
		}

		return iLayer;

	NextLayer:
		;
	}

	return 0;
}

/*	Get height data of specified terrain area

	rcMap: mask area in terrain grids
*/
bool CExpTerrain::GetHeightData(const ARectI& rcMap, HEIGHTINFO& Data)
{
	Data.iWidth		= rcMap.Width() + 1;
	Data.iHeight	= rcMap.Height() + 1;
	Data.iPitch		= m_HeightMap.iWidth * sizeof (float);
	Data.pData		= m_HeightMap.pHeightData + rcMap.top * m_HeightMap.iWidth + rcMap.left;

	return true;
}

//	Get normal data of specified terrain area
bool CExpTerrain::GetNormalData(const ARectI& rcMap, HEIGHTINFO& Data)
{
	Data.iWidth		= rcMap.Width() + 1;
	Data.iHeight	= rcMap.Height() + 1;
	Data.iPitch		= m_HeightMap.iWidth * sizeof (A3DVECTOR3);
	Data.pData		= m_HeightMap.aNormals + rcMap.top * m_HeightMap.iWidth + rcMap.left;
	
	return true;
}


//	Create blocks
bool CExpTerrain::CreateBlocks()
{
	m_iTerrainBlock = (m_HeightMap.iWidth - 1) / BLOCK_GRID;
	m_aBlocks.SetSize(m_iTerrainBlock * m_iTerrainBlock, 10);

	int iCount = 0;

	for (int r=0; r < m_iTerrainBlock; r++)
	{
		for (int c=0; c < m_iTerrainBlock; c++, iCount++)
		{
			BLOCK& Block = m_aBlocks[iCount];
			InitBlock(Block, r, c);
		}
	}

	return true;
}

//	Initialize block data
bool CExpTerrain::InitBlock(BLOCK& Block, int r, int c)
{
	//	Block area in grids
	Block.rcGrid.left	= c << BLOCK_GRID_SHIFT;
	Block.rcGrid.top	= r << BLOCK_GRID_SHIFT;
	Block.rcGrid.right	= Block.rcGrid.left + BLOCK_GRID;
	Block.rcGrid.bottom	= Block.rcGrid.top + BLOCK_GRID;

	//	Calcualte block's aabb
	Block.aabb.Mins.x	= m_rcTerrain.left + Block.rcGrid.left * m_fTileSize;
	Block.aabb.Mins.z	= m_rcTerrain.top - Block.rcGrid.bottom * m_fTileSize;
	Block.aabb.Maxs.x	= m_rcTerrain.left + Block.rcGrid.right * m_fTileSize;
	Block.aabb.Maxs.z	= m_rcTerrain.top - Block.rcGrid.top * m_fTileSize;
	Block.aabb.Maxs.y	= -100000.0f;
	Block.aabb.Mins.y	= 100000.0f;

	for (int i=Block.rcGrid.top; i <= Block.rcGrid.bottom; i++)
	{
		int v = m_HeightMap.iWidth * i + Block.rcGrid.left;

		for (int j=Block.rcGrid.left; j <= Block.rcGrid.right; j++, v++)
		{
			const A3DVECTOR3& vPos = m_aPos[v];

			if (vPos.y < Block.aabb.Mins.y)
				Block.aabb.Mins.y = vPos.y;

			if (vPos.y > Block.aabb.Maxs.y)
				Block.aabb.Maxs.y = vPos.y;
		}
	}

	Block.aabb.Mins -= 0.5f;
	Block.aabb.Maxs += 0.5f;

	return true;
}

//	Get normal of specified position
bool CExpTerrain::GetPosNormal(const A3DVECTOR3& vPos, A3DVECTOR3& vNormal)
{
#if 1

	if (vPos.x < m_rcTerrain.left || vPos.x > m_rcTerrain.right ||
		vPos.z > m_rcTerrain.top || vPos.z < m_rcTerrain.bottom)
	{
		vNormal = g_vAxisY;
		return false;
	}

	//	Get grid this position is in
	float fInvGridSize = 1.0f / m_fTileSize;
	int iCol = (int)((vPos.x - m_rcTerrain.left) * fInvGridSize);
	int iRow = (int)(-(vPos.z - m_rcTerrain.top) * fInvGridSize);
	a_Clamp(iCol, 0, m_HeightMap.iWidth-2);
	a_Clamp(iRow, 0, m_HeightMap.iHeight-2);

	int i0 = iRow * m_HeightMap.iWidth + iCol;
	const A3DVECTOR3& v0 = m_aPos[i0];
	const A3DVECTOR3& v1 = m_aPos[i0 + 1];
	const A3DVECTOR3& v2 = m_aPos[i0 + m_HeightMap.iWidth];
	const A3DVECTOR3& v3 = m_aPos[i0 + m_HeightMap.iWidth + 1];

	A3DVECTOR3 vDest;

	float dx = vPos.x - v0.x;
	float dz = vPos.z - v0.z;

	if (dx > -dz)
	{
		A3DVECTOR3 vEdge1 = v1 - v0;
		A3DVECTOR3 vEdge2 = v3 - v0;
		vNormal = a3d_Normalize(CrossProduct(vEdge1, vEdge2));
	}
	else
	{
		A3DVECTOR3 vEdge1 = v3 - v0;
		A3DVECTOR3 vEdge2 = v2 - v0;
		vNormal = a3d_Normalize(CrossProduct(vEdge1, vEdge2));
	}

#else

/*	if (vPos.x < m_rcTerrain.left || vPos.x > m_rcTerrain.right ||
		vPos.z > m_rcTerrain.top || vPos.z < m_rcTerrain.bottom)
	{
		vNormal = g_vAxisY;
		return false;
	}

	//	Get grid this position is in
	float fInvGridSize = 1.0f / m_fTileSize;
	int iCol = (int)((vPos.x - m_rcTerrain.left) * fInvGridSize);
	int iRow = (int)(-(vPos.z - m_rcTerrain.top) * fInvGridSize);
	a_Clamp(iCol, 0, m_HeightMap.iWidth-1);
	a_Clamp(iRow, 0, m_HeightMap.iHeight-1);

	int i0 = iRow * m_HeightMap.iWidth + iCol;
	const A3DVECTOR3& v0 = m_aPos[i0];
	const A3DVECTOR3& v1 = m_aPos[i0 + 1];
	const A3DVECTOR3& v2 = m_aPos[i0 + m_HeightMap.iWidth];
	const A3DVECTOR3& v3 = m_aPos[i0 + m_HeightMap.iWidth + 1];

	const A3DVECTOR3& n0 = m_HeightMap.aNormals[i0];
	const A3DVECTOR3& n1 = m_HeightMap.aNormals[i0 + 1];
	const A3DVECTOR3& n2 = m_HeightMap.aNormals[i0 + m_HeightMap.iWidth];
	const A3DVECTOR3& n3 = m_HeightMap.aNormals[i0 + m_HeightMap.iWidth + 1];

	float dx = vPos.x - v0.x;
	float dz = vPos.z - v0.z;

	if (dx > -dz)
	{
		vNormal  = n0 + (n1 - n0) * (dx / (v1.x - v0.x));
		vNormal += (n3 - n1) * (dz / (v3.z - v1.z));
	}
	else
	{
		vNormal  = n0 + (n2 - n0) * (dz / (v2.z - v0.z));
		vNormal += (n3 - n2) * (dx / (v3.x - v2.x));
	}

	vNormal.Normalize();
*/

#endif

	return true;
}

//	Get height of specified position
float CExpTerrain::GetPosHeight(const A3DVECTOR3& vPos)
{
	if (vPos.x < m_rcTerrain.left || vPos.x > m_rcTerrain.right ||
		vPos.z > m_rcTerrain.top || vPos.z < m_rcTerrain.bottom)
		return m_fYOffset;

	//	Get grid this position is in
	float fInvGridSize = 1.0f / m_fTileSize;
	int iCol = (int)((vPos.x - m_rcTerrain.left) * fInvGridSize);
	int iRow = (int)(-(vPos.z - m_rcTerrain.top) * fInvGridSize);
	a_Clamp(iCol, 0, m_HeightMap.iWidth-2);
	a_Clamp(iRow, 0, m_HeightMap.iHeight-2);

	A3DVECTOR3 v0, v1, v2, v3;
	int i0 = iRow * m_HeightMap.iWidth + iCol;
	int i1 = i0 + 1;
	int i2 = i0 + m_HeightMap.iWidth;
	int i3 = i0 + m_HeightMap.iWidth + 1;

	if (m_aPos)
	{
		v0 = m_aPos[i0];
		v1 = m_aPos[i1];
		v2 = m_aPos[i2];
		v3 = m_aPos[i3];
	}
	else if (m_HeightMap.pHeightData)
	{
		v0.x = m_rcTerrain.left + iCol * m_fTileSize;
		v0.y = GetRealHeight(i0);
		v0.z = m_rcTerrain.top - iRow * m_fTileSize;

		v1.x = v0.x + m_fTileSize;
		v1.y = GetRealHeight(i1);
		v1.z = v1.z;

		v2.x = v0.x;
		v2.y = GetRealHeight(i2);
		v2.z = v1.z - m_fTileSize;

		v3.x = v0.x + m_fTileSize;
		v3.y = GetRealHeight(i3);
		v3.z = v1.z - m_fTileSize;
	}
	else
	{
		ASSERT(0);
		return 0.0f;
	}

	A3DVECTOR3 vDest;

	float dx = vPos.x - v0.x;
	float dz = vPos.z - v0.z;

	if (dx > -dz)
	{
		vDest  = v0 + (v1 - v0) * (dx / (v1.x - v0.x));
		vDest += (v3 - v1) * (dz / (v3.z - v1.z));
	}
	else
	{
		vDest  = v0 + (v2 - v0) * (dz / (v2.z - v0.z));
		vDest += (v3 - v2) * (dx / (v3.x - v2.x));
	}

	return vDest.y;
}

//	Ray trace function
bool CExpTerrain::RayTrace(const A3DVECTOR3& vStart, const A3DVECTOR3& vVel, float fTime, RAYTRACERT* pTraceRt)
{
	m_RayTraceInfo.vStart		= vStart;
	m_RayTraceInfo.vDelta		= vVel * fTime;
	m_RayTraceInfo.vEnd			= vStart + m_RayTraceInfo.vDelta;
	m_RayTraceInfo.fFraction	= 1.0f;

	pTraceRt->fFraction = 1.0f;

	//	Initlaize some block information
	InitBlockBeforeRayTrace();

	if (RayTraceInTerrain())
	{
		pTraceRt->fFraction	= m_RayTraceInfo.fFraction;
		pTraceRt->vPoint	= m_RayTraceInfo.vNormal;
		pTraceRt->vHitPos	= vStart + m_RayTraceInfo.vDelta * m_RayTraceInfo.fFraction;
		pTraceRt->vPoint	= pTraceRt->vHitPos;

		return true;
	}

	return false;
}

//	Initialize block information before ray trace
bool CExpTerrain::InitBlockBeforeRayTrace()
{
	A3DVECTOR3 vHit1, vHit2, vNormal;
	FLOAT fFraction;

	//	Do collision test between ray and all blocks' aabb
	for (int i=0; i < m_aBlocks.GetSize(); i++)
	{
		BLOCK& Block = m_aBlocks[i];

		Block.bCollision = CLS_RayToAABB3(m_RayTraceInfo.vStart, m_RayTraceInfo.vDelta,
				Block.aabb.Mins, Block.aabb.Maxs, vHit1, &fFraction, vNormal);

		if (Block.bCollision)
		{
			CLS_RayToAABB3(m_RayTraceInfo.vEnd, -m_RayTraceInfo.vDelta,
				Block.aabb.Mins, Block.aabb.Maxs, vHit2, &fFraction, vNormal);

			Block.fRayMinY = a_Min(vHit1.y, vHit2.y);
		}
	}

	return true;
}

//	Initiailze grid 2DDA information
bool CExpTerrain::InitGrid2DDA()
{
	//	Get terrain's AABB
	A3DVECTOR3 vMins, vMaxs;
	vMins.x	= m_rcTerrain.left;
	vMaxs.z = m_rcTerrain.top;
	vMaxs.x = m_rcTerrain.right;
	vMins.z = m_rcTerrain.bottom;
	vMins.y = m_fYOffset - 1.0f;
	vMaxs.y = m_fYOffset + m_fMaxHeight + 1.0f;

	//	Clip ray into active area AABB
	A3DVECTOR3 v0, v1, vDelta, vTemp;
	float fFraction;
	if (!CLS_RayToAABB3(m_RayTraceInfo.vStart, m_RayTraceInfo.vDelta, vMins, vMaxs, v0, &fFraction, vTemp))
		return false;

	if (!CLS_RayToAABB3(m_RayTraceInfo.vEnd, -m_RayTraceInfo.vDelta, vMins, vMaxs, v1, &fFraction, vTemp))
		return false;

	//	Build grid 2DDA information
	float fInvGridSize = 1.0f / m_fTileSize;
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

	a_Clamp(m_Grid2DDA.vStart[0], 0, m_HeightMap.iWidth-1);
	a_Clamp(m_Grid2DDA.vEnd[0], 0, m_HeightMap.iWidth-1);
	a_Clamp(m_Grid2DDA.vStart[1], 0, m_HeightMap.iHeight-1);
	a_Clamp(m_Grid2DDA.vEnd[1], 0, m_HeightMap.iHeight-1);

	m_Grid2DDA.aSteps[0]	= abs(m_Grid2DDA.vEnd[0] - m_Grid2DDA.vStart[0]) + 1;
	m_Grid2DDA.aSteps[1]	= abs(m_Grid2DDA.vEnd[1] - m_Grid2DDA.vStart[1]) + 1;

	vDelta.x = (float)fabs(vDelta.x);
	vDelta.z = (float)fabs(vDelta.z);

	float dx, dz;
	if (m_Grid2DDA.s[0] > 0)
		dx = (vMins.x + (m_Grid2DDA.vStart[0] + 1) * m_fTileSize) - v0.x;
	else
		dx = v0.x - (vMins.x + m_Grid2DDA.vStart[0] * m_fTileSize);

	if (dx == 0.0f)
		dx = m_fTileSize;

	if (m_Grid2DDA.s[1] > 0)
		dz = -((vMaxs.z - (m_Grid2DDA.vStart[1] + 1) * m_fTileSize) - v0.z);
	else
		dz = (vMaxs.z - m_Grid2DDA.vStart[1] * m_fTileSize) - v0.z;

	if (dz == 0.0f)
		dz = m_fTileSize;

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
		m_Grid2DDA.e = m_fTileSize - dz;
		m_Grid2DDA.i = fSlope * m_fTileSize;
		m_Grid2DDA.c = fSlope * dx;
	}
	else	//	z major
	{
		m_Grid2DDA.iMajor = 1;
		float fSlope = vDelta.x / vDelta.z;
		
		m_Grid2DDA.d = dx;
		m_Grid2DDA.e = m_fTileSize - dx;
		m_Grid2DDA.i = fSlope * m_fTileSize;
		m_Grid2DDA.c = fSlope * dz;
	}

	return true;
}

//	Ray trace in whole terrain
bool CExpTerrain::RayTraceInTerrain()
{
	if (!InitGrid2DDA())
		return false;

	int c = m_Grid2DDA.vStart[0];
	int r = m_Grid2DDA.vStart[1];

	int iColPitch = m_Grid2DDA.s[0];
	int iRowPitch = m_HeightMap.iWidth - 1;

	int iGrid = r * iRowPitch + c;

	if (m_Grid2DDA.s[1] < 0)
		iRowPitch = -iRowPitch;

	int sc = m_Grid2DDA.s[0];
	int sr = m_Grid2DDA.s[1];

	//	Visit the start block
	if (RayTraceInGrid(r, c))
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
					if (RayTraceInGrid(r, c))
						return true;
				}

				return false;
			}

			bool bRowInc = false;

			e += m_Grid2DDA.i;

			if (e > m_fTileSize)
			{
				r	   += sr;
				e	   -= m_fTileSize;
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
						if (RayTraceInGrid(r, c-sc))
							return true;
					}
					else
					{
						if (RayTraceInGrid(r-sr, c))
							return true;
					}
				}
			}

			if (RayTraceInGrid(r, c))
				return true;

			m_Grid2DDA.d = m_fTileSize - e;
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
					if (RayTraceInGrid(r, c))
						return true;
				}

				return false;
			}

			bool bColInc = false;

			e += m_Grid2DDA.i;

			if (e > m_fTileSize)
			{
				c	   += sc;
				e	   -= m_fTileSize;
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
						if (RayTraceInGrid(r-sr, c))
							return true;
					}
					else
					{
						if (RayTraceInGrid(r, c-sc))
							return true;
					}
				}
			}

			if (RayTraceInGrid(r, c))
				return true;

			m_Grid2DDA.d = m_fTileSize - e;
		}
	}

	return false;
}

//	Ray trace in grid
bool CExpTerrain::RayTraceInGrid(int r, int c)
{
	if (r < 0 || r >= m_HeightMap.iHeight-1 || c < 0 || c >= m_HeightMap.iWidth-1)
		return false;

	//	Get block this grid belongs to
	int br = r >> BLOCK_GRID_SHIFT;
	int bc = c >> BLOCK_GRID_SHIFT;
	BLOCK& Block = m_aBlocks[br * m_iTerrainBlock + bc];
	if (!Block.bCollision)
		return false;

	int i0 = r * m_HeightMap.iWidth + c;
	A3DVECTOR3& v0 = m_aPos[i0];
	A3DVECTOR3& v1 = m_aPos[i0 + 1];
	A3DVECTOR3& v2 = m_aPos[i0 + m_HeightMap.iWidth];
	A3DVECTOR3& v3 = m_aPos[i0 + m_HeightMap.iWidth + 1];

	//	Get grid's maximum height
	float fMaxHeight = max4(v0.y, v1.y, v2.y, v3.y);
	if (Block.fRayMinY > fMaxHeight)
		return false;

	float fFraction;
	A3DVECTOR3 vHit;
	float fEpsilon = 0.001f / m_fTileSize;

	A3DVECTOR3 vEdge1 = v1 - v0;
	A3DVECTOR3 vEdge2 = v2 - v0;
	v0  = m_aPos[i0] + -(vEdge1 + vEdge2) * fEpsilon;
	v1 += vEdge1 * fEpsilon;
	v2 += vEdge2 * fEpsilon;

	//	Check face 1
	if (CLS_RayToTriangle(m_RayTraceInfo.vStart, m_RayTraceInfo.vDelta, v0, v1, v3, vHit, false, &fFraction))
	{
		m_RayTraceInfo.fFraction = fFraction;
		return true;
	}

	vEdge1 = v2 - v0;
	vEdge2 = v3 - v0;
	v0  = m_aPos[i0] + -(vEdge1 + vEdge2) * fEpsilon;
	v2 += vEdge1 * fEpsilon;
	v3 += vEdge2 * fEpsilon;

	//	Check face 2
	if (CLS_RayToTriangle(m_RayTraceInfo.vStart, m_RayTraceInfo.vDelta, v0, v3, v2, vHit, false, &fFraction))
	{
		m_RayTraceInfo.fFraction = fFraction;
		return true;
	}

	return false;

/*	if (r < 0 || r >= m_HeightMap.iHeight-1 || c < 0 || c >= m_HeightMap.iWidth-1)
		return false;

	int i0 = r * m_HeightMap.iWidth + c;
	const A3DVECTOR3& v0 = m_aPos[i0];
	const A3DVECTOR3& v1 = m_aPos[i0 + 1];
	const A3DVECTOR3& v2 = m_aPos[i0 + m_HeightMap.iWidth];
	const A3DVECTOR3& v3 = m_aPos[i0 + m_HeightMap.iWidth + 1];

	float fFraction;
	A3DVECTOR3 vHit;

	//	Check face 1
	if (CLS_RayToTriangle(m_RayTraceInfo.vStart, m_RayTraceInfo.vDelta, v0, v1, v3, vHit, false, &fFraction))
	{
		m_RayTraceInfo.fFraction = fFraction;
		return true;
	}

	//	Check face 2
	if (CLS_RayToTriangle(m_RayTraceInfo.vStart, m_RayTraceInfo.vDelta, v0, v3, v2, vHit, false, &fFraction))
	{
		m_RayTraceInfo.fFraction = fFraction;
		return true;
	}

	return false;
*/
}

//	Set map offset
void CExpTerrain::SetMapOffset(float x, float z)
{
	CTerrain::SetMapOffset(x, z);
	
	if (m_aPos)
	{
		int i, j, iCount = 0;

		for (i=0; i < m_HeightMap.iHeight; i++)
		{
			for (j=0; j < m_HeightMap.iWidth; j++, iCount++)
			{
				A3DVECTOR3* pPos = &m_aPos[iCount];
				pPos->x += x;
				pPos->z += z;
			}
		}
	}
}

