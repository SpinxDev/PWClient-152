/*
 * FILE: AutoTerrain.cpp
 *
 * DESCRIPTION: Class for automaticlly generating terrain
 *
 * CREATED BY: Jiang Dalong, 2005/01/04
 *
 * HISTORY: 
 *
 * Copyright (c) 2001-2008 Archosaur Studio, All Rights Reserved.
 */
#pragma warning (disable: 4244)
#pragma warning (disable: 4018)

#include "AutoTerrain.h"
#include "AutoHome.h"
#include "AutoScene.h"
#include "PerlinNoise2D.h"
#include "HomeTerrain.h"
#include "HomeTrnMask.h"
#include "AutoScene.h"
#include "AutoSceneFunc.h"
#include "AutoHomeBorderData.h"
#include "SoftGouraud.h"
#include "CurvePoint.h"
#include "AutoHomeBorder.h"
#include "AutoSceneConfig.h"

#include <ALog.h>
#include <A3DMacros.h>
#include <A3DStream.h>
#include <A3DEngine.h>
#include <A3DFrustum.h>
#include <A3DCameraBase.h>
#include <A3DWireCollector.h>
#include <AFileImage.h>
#include <A3DDevice.h>
#include <A3DTexture.h>
#include <A3DTerrain2Env.h>

const int MASK_SIZE				= CHomeTrnMask::MASKMAP_SIZE;
const int MAX_HEIGHT			= 800;
const int BORDER_SIZE			= 10;

#define HEIGHTMAP_WIDTH			65
#define HEIGHTMAP_HEIGHT		65
#define TILE_SIZE				2.0f

///////////////////////////////////////////////////////////////////////////
//	Local functions
///////////////////////////////////////////////////////////////////////////

//	Calculate triangle's normal
static inline A3DVECTOR3 _CalcTriangleNormal(const A3DVECTOR3& v0, const A3DVECTOR3& v1, const A3DVECTOR3& v2)
{
	A3DVECTOR3 e1 = v1 - v0;
	A3DVECTOR3 e2 = v2 - v0;
	A3DVECTOR3 vNormal = CrossProduct(e1, e2);
	vNormal.Normalize();
	return vNormal;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAutoTerrain::CAutoTerrain(CAutoHome* pAutoHome)
{
	m_pAutoHome		= pAutoHome;
	m_pAutoScene	= pAutoHome->GetAutoScene();
	m_pHomeTerrain	= m_pAutoScene->GetHomeTerrain();
	
	memset(&m_HeightMap, 0, sizeof (m_HeightMap));
	m_HeightMap.iWidth = HEIGHTMAP_WIDTH;
	m_HeightMap.iHeight = HEIGHTMAP_HEIGHT;

	m_bReady = false;
	m_bNeedRefreshHeight = false;
	m_iOldLODType = A3DTerrain2::LOD_NONE;

	m_MinVertex.x = 0;
	m_MinVertex.z = 0;
	m_MinVertex.fHeight = 0.0f;
	m_MaxVertex.x = 0;
	m_MaxVertex.z = 0;
	m_MaxVertex.fHeight = 0.0f;

	m_nUpDownPri = UDP_BOTH;
}

CAutoTerrain::~CAutoTerrain()
{
	Release();
}

bool CAutoTerrain::Init()
{
	m_fTileSize		= TILE_SIZE;
	m_fMaxHeight	= MAX_HEIGHT;
	m_fYOffset		= 0;
	m_fTerrainSize	= m_fTileSize * (m_HeightMap.iWidth - 1);

	CalcTerrainArea();

	// Alloc height map memory
	int iSize = m_HeightMap.iWidth * m_HeightMap.iHeight;

	m_HeightMap.aNormals		= new A3DVECTOR3[iSize];
	m_HeightMap.pHeightData		= new float[iSize];
	m_HeightMap.pAbsHeight		= new float[iSize];
	m_HeightMap.pAbsHeightBak	= new float[iSize];
	m_HeightMap.pAbsHeightOri	= new float[iSize];
	m_HeightMap.pAbsHeightUp	= new float[iSize];
	m_HeightMap.pAbsHeightDown	= new float[iSize];
	m_HeightMap.pAbsHeightRoad	= new float[iSize];

	if ((NULL == m_HeightMap.pAbsHeight) || (NULL == m_HeightMap.pAbsHeightBak)
		|| (NULL == m_HeightMap.pAbsHeightUp) || (NULL == m_HeightMap.pAbsHeightDown)
		|| (NULL == m_HeightMap.pAbsHeightRoad) || (NULL == m_HeightMap.pHeightData) 
		|| (NULL == m_HeightMap.aNormals || (NULL == m_HeightMap.pAbsHeightOri)))
	{
		a_LogOutput(1, "CAutoTerrain::Init, Creating height map faied!");
		return false;
	}

	//	Fill height map data
	memset(m_HeightMap.pAbsHeight, 0, iSize * sizeof (float));
	memset(m_HeightMap.pAbsHeightBak, 0, iSize * sizeof (float));
	memset(m_HeightMap.pAbsHeightOri, 0, iSize * sizeof (float));
	memset(m_HeightMap.pAbsHeightUp, 0, iSize * sizeof (float));
	memset(m_HeightMap.pAbsHeightDown, 0, iSize * sizeof (float));
	memset(m_HeightMap.pAbsHeightRoad, 0, iSize * sizeof (float));
	memset(m_HeightMap.pHeightData, 0, iSize * sizeof (float));

	int i, j;
	for (i=0; i < m_HeightMap.iHeight; i++)
	{
		for (j=0; j < m_HeightMap.iWidth; j++)
		{
			int n = i * m_HeightMap.iWidth + j;
			m_HeightMap.aNormals[n].x = 0.0f;
			m_HeightMap.aNormals[n].y = 1.0f;
			m_HeightMap.aNormals[n].z = 0.0f;
		}
	}

	m_bReady = true;

	return true;
}

// Release
void CAutoTerrain::Release()
{
	ReleaseHeightMap();
}

// Create height map
bool CAutoTerrain::CreateHeightMap(float fAmplitude, int nFrequency, float fPersistence, DWORD dwRandSeed)
{
	if ((NULL == m_HeightMap.pAbsHeight) || (NULL == m_HeightMap.pAbsHeightBak)
		|| (NULL == m_HeightMap.pAbsHeightUp) || (NULL == m_HeightMap.pAbsHeightDown))
	{
		a_LogOutput(1, "CAutoTerrain::CreateHeightMap, Creating height map faied!");
		return false;
	}

	// Create noise
	CPerlinNoise2D noise;
	if (!noise.Init(m_HeightMap.iWidth, m_HeightMap.iHeight, fAmplitude, nFrequency, fPersistence, 4, dwRandSeed))
	{
		a_LogOutput(1, "CAutoTerrain::CreateHeightMap, Initialize perlin noise faied!");
		return false;
	}

	// Get height value from noise
	noise.CopyAllNoise(m_HeightMap.pAbsHeightBak);
	noise.CopyAllNoise(m_HeightMap.pAbsHeightOri);
/*
	int i, j;
	int nPos;
	for (i=0; i<m_HeightMap.iHeight; i++)
	{
		for (j=0; j<m_HeightMap.iWidth; j++)
		{
			nPos = i*m_HeightMap.iWidth+j;
			UpdateAbsHeightAtPos(nPos);
		}
	}

	if (UDP_BOTH != GetUpDownPri())
		BlurArea(m_HeightMap.pAbsHeight, m_HeightMap.iWidth, m_HeightMap.iHeight);
	
	m_bNeedRefreshHeight = true;
*/
	return true;
}

// Create main terrain
bool CAutoTerrain::CreateTerrainHeight(float fAmplitude, int nFrequency, float fPersistence, DWORD dwRandSeed)
{
	//	Create new height map
	if (!CreateHeightMap(fAmplitude, nFrequency, fPersistence, dwRandSeed))
		return false;
	if (!SmoothHeightBorder())
		return false;
	if (!UpdateAbsHeight())
		return false;
/*
	//	Reset height map data
	if (!ResetHeightData(MAX_HEIGHT))
		return false;

	//	Update normals
	if (!UpdateVertexNormals())
		return false;
*/
	return true;
}

// Release height map
void CAutoTerrain::ReleaseHeightMap()
{
	SAFERELEASE(m_HeightMap.aNormals);
	SAFERELEASE(m_HeightMap.pAbsHeight);
	SAFERELEASE(m_HeightMap.pHeightData);
	SAFERELEASE(m_HeightMap.pAbsHeightBak);
	SAFERELEASE(m_HeightMap.pAbsHeightOri);
	SAFERELEASE(m_HeightMap.pAbsHeightUp);
	SAFERELEASE(m_HeightMap.pAbsHeightDown);
	SAFERELEASE(m_HeightMap.pAbsHeightRoad);
}

//	Update vertex normals
bool CAutoTerrain::UpdateVertexNormals()
{
	if (!m_HeightMap.aNormals || !m_HeightMap.pAbsHeight)
	{
		ASSERT(m_HeightMap.aNormals && m_HeightMap.pAbsHeight);
		return false;
	}

	ARectI rcVert(0, 0, m_HeightMap.iWidth, m_HeightMap.iHeight);

	int i, j, i0;
	A3DVECTOR3 v0, v1, v2, vNormal;
	float x, z;
	float* aHeis = m_HeightMap.pAbsHeight;

	//	Center vertices
	for (i=rcVert.top; i < rcVert.bottom; i++)
	{
		x	= rcVert.left * m_fTileSize;
		z	= -i * m_fTileSize;
		i0	= i * m_HeightMap.iWidth + rcVert.left;

		for (j=rcVert.left; j < rcVert.right; j++, i0++)
		{
			vNormal.Clear();

			v0.Set(x, aHeis[i0], z);

			if (i > 0 && j > 0)
			{
				v1.Set(x-m_fTileSize, aHeis[i0-1], z);
				v2.Set(x, aHeis[i0-m_HeightMap.iWidth], z+m_fTileSize);
				vNormal += _CalcTriangleNormal(v0, v1, v2);
			}
			
			if (i > 0 && j < m_HeightMap.iWidth-1)
			{
				v1.Set(x, aHeis[i0-m_HeightMap.iWidth], z+m_fTileSize);
				v2.Set(x+m_fTileSize, aHeis[i0+1], z);
				vNormal += _CalcTriangleNormal(v0, v1, v2);
			}

			if (i < m_HeightMap.iHeight-1 && j < m_HeightMap.iWidth-1)
			{
				v1.Set(x+m_fTileSize, aHeis[i0+1], z);
				v2.Set(x, aHeis[i0+m_HeightMap.iWidth], z-m_fTileSize);
				vNormal += _CalcTriangleNormal(v0, v1, v2);
			}

			if (i < m_HeightMap.iHeight-1 && j > 0)
			{
				v1.Set(x, aHeis[i0+m_HeightMap.iWidth], z-m_fTileSize);
				v2.Set(x-m_fTileSize, aHeis[i0-1], z);
				vNormal += _CalcTriangleNormal(v0, v1, v2);
			}

			vNormal.Normalize();
			m_HeightMap.aNormals[i0] = vNormal;

			x += m_fTileSize;
		}
	}

	return true;
}

//	Reset height data for file
bool CAutoTerrain::ResetHeightData(float fMaxHeight)
{
	if (!m_HeightMap.pAbsHeight || !m_HeightMap.pHeightData)
	{
		ASSERT(m_HeightMap.pAbsHeight && m_HeightMap.pHeightData);
		return false;
	}

	float fInvMaxHei = 1.0f / fMaxHeight;
	int iNumVert = m_HeightMap.iWidth * m_HeightMap.iHeight;

	for (int i=0; i < iNumVert; i++)
		m_HeightMap.pHeightData[i] = m_HeightMap.pAbsHeight[i] * fInvMaxHei;

	return true;
}

// Get vertices of min and max height
bool CAutoTerrain::GetExtremeVertices(VERTEXINFO& min, VERTEXINFO& max)
{
	if (!m_bNeedRefreshHeight)
	{
		min = m_MinVertex;
		max = m_MaxVertex;
		return true;
	}

	int i,j;
	min.x = -1;
	min.z = -1;
	min.fHeight = 99999;
	max.x = -1;
	max.z = -1;
	max.fHeight = -99999;

	if (!GetReady())
		return false;

	float fHeight;
	for (i=0; i<m_HeightMap.iHeight; i++)
	{
		for (j=0; j<m_HeightMap.iWidth; j++)
		{
			fHeight = m_HeightMap.pAbsHeight[i*m_HeightMap.iWidth+j];
			if (fHeight < min.fHeight)
			{
				min.x = j;
				min.z = i;
				min.fHeight = fHeight;
			}
			if (fHeight > max.fHeight)
			{
				max.x = j;
				max.z = i;
				max.fHeight = fHeight;
			}
		}
	}

	m_MinVertex = min;
	m_MaxVertex = max;
	m_bNeedRefreshHeight = false;

	return true;
}

//	Restore base terrain
bool CAutoTerrain::RestoreBaseTerrain(bool bApply/* true */)
{
	if (!m_bReady)
		return false;

	int nNumVert = m_HeightMap.iHeight * m_HeightMap.iWidth;

	//	Restore height map
	::CopyMemory(m_HeightMap.pAbsHeight, m_HeightMap.pAbsHeightBak, nNumVert * sizeof(float));

	//	Clear displacement of height map
	::ZeroMemory(m_HeightMap.pAbsHeightUp, nNumVert * sizeof(float));
	::ZeroMemory(m_HeightMap.pAbsHeightDown, nNumVert * sizeof(float));
	::ZeroMemory(m_HeightMap.pAbsHeightRoad, nNumVert * sizeof(float));

	//	Update height data
//	ResetHeightData(MAX_HEIGHT);
	//	Update normals
//	UpdateVertexNormals();

	//	Apply terrain changes
	if (bApply && m_pHomeTerrain)
	{
		int r = m_pAutoHome->GetRowIndex();
		int c = m_pAutoHome->GetColIndex();
		m_pHomeTerrain->UpdateHomeAreaHeight(r, c, m_HeightMap.pAbsHeight, m_HeightMap.aNormals, m_HeightMap.iWidth);
	}

	m_bNeedRefreshHeight = true;

	return true;
}


// Reset terrain
void CAutoTerrain::ResetTerrain(bool bApply/* true */)
{
	if (!m_bReady)
		return;

	int nNumVert = m_HeightMap.iHeight * m_HeightMap.iWidth;
	
	::ZeroMemory(m_HeightMap.pAbsHeight, nNumVert * sizeof (float));
	::ZeroMemory(m_HeightMap.pAbsHeightBak, nNumVert * sizeof (float));
	::ZeroMemory(m_HeightMap.pAbsHeightOri, nNumVert * sizeof (float));
	::ZeroMemory(m_HeightMap.pAbsHeightUp, nNumVert * sizeof (float));
	::ZeroMemory(m_HeightMap.pAbsHeightDown, nNumVert * sizeof (float));
	::ZeroMemory(m_HeightMap.pAbsHeightRoad, nNumVert * sizeof (float));

	memset(m_HeightMap.pHeightData, 0, nNumVert * sizeof (float));

	//	Update normals
	UpdateVertexNormals();

	//	Apply terrain changes
	if (bApply && m_pHomeTerrain)
	{
		int r = m_pAutoHome->GetRowIndex();
		int c = m_pAutoHome->GetColIndex();
		m_pHomeTerrain->UpdateHomeAreaHeight(r, c, m_HeightMap.pAbsHeight, m_HeightMap.aNormals, m_HeightMap.iWidth);
	}

	m_bNeedRefreshHeight = true;
}

//	Apply terrain height and normal
bool CAutoTerrain::ApplyHeightAndNormal(bool bUpdateNormal/* true */)
{
	if (!m_HeightMap.pAbsHeight)
	{
		ASSERT(m_HeightMap.pAbsHeight);
		return false;
	}

	if (bUpdateNormal)
	{
		ResetHeightData(MAX_HEIGHT);
		UpdateVertexNormals();
	}

	if (m_pHomeTerrain)
	{
		int r = m_pAutoHome->GetRowIndex();
		int c = m_pAutoHome->GetColIndex();
		m_pHomeTerrain->UpdateHomeAreaHeight(r, c, m_HeightMap.pAbsHeight, m_HeightMap.aNormals, m_HeightMap.iWidth);
	}

	return true;
}

// Get vertices of min and max height in rect
bool CAutoTerrain::GetExtremeVerticesInRect(VERTEXINFO& min, VERTEXINFO& max, ARectI rect)
{
	if (!GetReady())
		return false;

	int i,j;
	min.x = -1;
	min.z = -1;
	min.fHeight = 99999;
	max.x = -1;
	max.z = -1;
	max.fHeight = -99999;

	a_Clamp(rect.top, 0, m_HeightMap.iHeight-1);	
	a_Clamp(rect.bottom, 0, m_HeightMap.iHeight-1);	
	a_Clamp(rect.left, 0, m_HeightMap.iWidth-1);	
	a_Clamp(rect.right, 0, m_HeightMap.iWidth-1);	

	float fHeight;
	for (i=rect.top; i<rect.bottom; i++)
	{
		for (j=rect.left; j<rect.right; j++)
		{
			fHeight = m_HeightMap.pAbsHeight[i*m_HeightMap.iWidth+j];
			if (fHeight < min.fHeight)
			{
				min.x = j;
				min.z = i;
				min.fHeight = fHeight;
			}
			if (fHeight > max.fHeight)
			{
				max.x = j;
				max.z = i;
				max.fHeight = fHeight;
			}
		}
	}

	return true;
}

//	Get terrain height of specified position
float CAutoTerrain::GetPosHeight(const A3DVECTOR3& vPos)
{
	if (!m_pHomeTerrain)
		return 0.0f;

	return m_pHomeTerrain->GetPosHeight(vPos);
}

//	Create absolute height data
bool CAutoTerrain::CreateAbsHeightData()
{
	ASSERT(m_HeightMap.pHeightData);

	int iNumVert = m_HeightMap.iWidth * m_HeightMap.iHeight;

	for (int i=0; i < iNumVert; i++)
		m_HeightMap.pAbsHeight[i] = GetRealHeight(i);

	// Backup height map
	::CopyMemory(m_HeightMap.pAbsHeightBak, m_HeightMap.pAbsHeight, iNumVert*sizeof(float));
	// Clear displacement of height map
	::ZeroMemory(m_HeightMap.pAbsHeightUp, iNumVert*sizeof(float));
	::ZeroMemory(m_HeightMap.pAbsHeightDown, iNumVert*sizeof(float));
	::ZeroMemory(m_HeightMap.pAbsHeightRoad, iNumVert*sizeof(float));

	return true;
}

//	Calculate terrain area
void CAutoTerrain::CalcTerrainArea()
{
	const ARectF& rcScene = m_pAutoScene->GetSceneRect();
	int r = m_pAutoHome->GetRowIndex();
	int c = m_pAutoHome->GetColIndex();

	m_rcTerrain.left	= rcScene.left + c * m_pAutoScene->GetBlockSize();
	m_rcTerrain.top		= rcScene.top - r * m_pAutoScene->GetBlockSize();
	m_rcTerrain.right	= m_rcTerrain.left + m_fTerrainSize;
	m_rcTerrain.bottom	= m_rcTerrain.top - m_fTerrainSize;
}

// Get height at a position
float CAutoTerrain::GetPosHeightFromHeightmap(const A3DVECTOR3& vPos)
{
	// m_HeightMap.iWidth == m_fTerrainSize * 0.5f
	float fTerrainSize = m_fTerrainSize * 0.5f;
	float fHalfTerrainSize = fTerrainSize / 2;
	int x = int(vPos.x / 2 + fHalfTerrainSize + 0.5);
	int y = int(fTerrainSize - (vPos.z / 2 + fHalfTerrainSize) + 0.5);

	return m_HeightMap.pAbsHeight[y*m_HeightMap.iWidth+x];
}

// Update absolute height by up and down at a position
bool CAutoTerrain::UpdateAbsHeightAtPos(int nPos)
{
	if (m_HeightMap.pAbsHeightUp[nPos] > 1)
		int n=0;
	switch (m_nUpDownPri)
	{
	case UDP_BOTH:
		m_HeightMap.pAbsHeight[nPos] = m_HeightMap.pAbsHeightBak[nPos]
							+ m_HeightMap.pAbsHeightUp[nPos]
							+ m_HeightMap.pAbsHeightDown[nPos]
							+ m_HeightMap.pAbsHeightRoad[nPos];
		break;
	case UDP_UP:
		if (m_HeightMap.pAbsHeightUp[nPos] > 0)
		{
			m_HeightMap.pAbsHeight[nPos] = m_HeightMap.pAbsHeightBak[nPos]
							+ m_HeightMap.pAbsHeightUp[nPos]
							+ m_HeightMap.pAbsHeightRoad[nPos];
		}
		else
		{
			m_HeightMap.pAbsHeight[nPos] = m_HeightMap.pAbsHeightBak[nPos]
							+ m_HeightMap.pAbsHeightDown[nPos]
							+ m_HeightMap.pAbsHeightRoad[nPos];
		}
		break;
	case UDP_DOWN:
		if (m_HeightMap.pAbsHeightDown[nPos] < 0)
		{
			m_HeightMap.pAbsHeight[nPos] = m_HeightMap.pAbsHeightBak[nPos]
							+ m_HeightMap.pAbsHeightDown[nPos]
							+ m_HeightMap.pAbsHeightRoad[nPos];
		}
		else
		{
			m_HeightMap.pAbsHeight[nPos] = m_HeightMap.pAbsHeightBak[nPos]
							+ m_HeightMap.pAbsHeightUp[nPos]
							+ m_HeightMap.pAbsHeightRoad[nPos];
		}
		break;
	}
	return true;
}

bool CAutoTerrain::BlurArea(float* pData, int nWidth, int nHeight)
{
	float* pTemp = new float[nWidth*nHeight];
	if (NULL == pTemp)
	{
		return false;
	}
	int i, j, k;
	int nPos;

	for (k=0; k<3; k++)
	{
		memcpy(pTemp, pData, nWidth*nHeight*sizeof(float));
		for (i=2; i<nHeight-2; i++)
		{
			for (j=2; j<nWidth-2; j++)
			{
				nPos = i*nWidth+j;
				pTemp[nPos] = (pData[nPos-2*nWidth-2] + 1*pData[nPos-2*nWidth-1] + 1*pData[nPos-2*nWidth] + 1*pData[nPos-2*nWidth+1] + pData[nPos-2*nWidth+2]
								+ 1*pData[nPos-nWidth-2] + 1*pData[nPos-nWidth-1] + 1*pData[nPos-nWidth] + 1*pData[nPos-nWidth+1] + 1*pData[nPos-nWidth+2]
								+ 1*pData[nPos-2] + 1*pData[nPos-1] + 1*pData[nPos] + 1*pData[nPos+1] + 1*pData[nPos+2]
								+ 1*pData[nPos+nWidth-2] + 1*pData[nPos+nWidth-1] + 1*pData[nPos+nWidth] + 1*pData[nPos+nWidth+1] + 1*pData[nPos+nWidth+2]
								+ pData[nPos+2*nWidth-2] + 1*pData[nPos+2*nWidth-1] + 1*pData[nPos+2*nWidth] + 1*pData[nPos+2*nWidth+1] + pData[nPos+2*nWidth+2]
								) / 25;
			}
		}
		memcpy(pData, pTemp, nWidth*nHeight*sizeof(float));
	}
	
	delete[] pTemp;

	return true;
}

// Update Absolute height by up and down
bool CAutoTerrain::UpdateAbsHeight()
{
	int i, j;
	int nPos;
	for (i=0; i<m_HeightMap.iHeight; i++)
	{
		for (j=0; j<m_HeightMap.iWidth; j++)
		{
			nPos = i * m_HeightMap.iWidth + j;
			UpdateAbsHeightAtPos(nPos);
		}
	}

	if (UDP_BOTH != GetUpDownPri())
		BlurArea(m_HeightMap.pAbsHeight, m_HeightMap.iWidth, m_HeightMap.iHeight);

//	ResetHeightData(MAX_HEIGHT);
//	UpdateVertexNormals();
	SetNeedRefreshHeight(true);

	return true;
}

// Clear all hill
bool CAutoTerrain::ClearAllHill(bool bUpdate)
{
	// Clear displacement of height map
	::ZeroMemory(m_HeightMap.pAbsHeightUp, m_HeightMap.iHeight*m_HeightMap.iWidth*sizeof(float));
	::ZeroMemory(m_HeightMap.pAbsHeightDown, m_HeightMap.iHeight*m_HeightMap.iWidth*sizeof(float));

	if (bUpdate)
	{
		UpdateAbsHeight();
//		ResetHeightData(MAX_HEIGHT);
//		UpdateVertexNormals();
	}

	m_bNeedRefreshHeight = true;

	return true;
}

// Clear road height
bool CAutoTerrain::ClearRoadHeight(bool bUpdate)
{
	// Clear displacement of height map
	::ZeroMemory(m_HeightMap.pAbsHeightRoad, m_HeightMap.iHeight*m_HeightMap.iWidth*sizeof(float));

	if (bUpdate)
	{
		UpdateAbsHeight();
//		ResetHeightData(MAX_HEIGHT);
//		UpdateVertexNormals();
	}

	m_bNeedRefreshHeight = true;

	return true;
}

//	Apply terrain vertices color
bool CAutoTerrain::ApplyTerrainColor(const DWORD* aColors1, const DWORD* aColors2)
{
	if (!m_pHomeTerrain)
		return false;

	int r = m_pAutoHome->GetRowIndex();
	int c = m_pAutoHome->GetColIndex();
	return m_pHomeTerrain->UpdateHomeAreaColor(r, c, aColors1, aColors2, m_HeightMap.iWidth);
}

// Get height weight by x,y weights
float CAutoTerrain::GetHeightWeight(float fWeightX, float fWeightY)
{
	float fHeight;
	float fDistance;
	fDistance = sqrt(fWeightX*fWeightX + fWeightY*fWeightY);
	if (fDistance < 1.0)
	{
		fHeight = 1 - CosWeight(fDistance);
	}
	else
		fHeight = 0.0f;

	return fHeight;
}

// Get local terrain area
void CAutoTerrain::GetTerrainAreaLocal(ARectF& rcTerrain)
{
	float fHalfBlockSize = m_pAutoHome->GetAutoScene()->GetBlockSize() * 0.5f;
	rcTerrain.left = - fHalfBlockSize;
	rcTerrain.right = fHalfBlockSize;
	rcTerrain.top = fHalfBlockSize;
	rcTerrain.bottom = -fHalfBlockSize;
}

// Smooth height map border
bool CAutoTerrain::SmoothHeightBorder()
{
	int nBorderSize = BORDER_SIZE;
	int nBlockRow = m_pAutoHome->GetRowIndex();
	int nBlockCol = m_pAutoHome->GetColIndex();
	int i, j;

	memcpy(m_HeightMap.pAbsHeight, m_HeightMap.pAbsHeightOri, m_HeightMap.iWidth*m_HeightMap.iHeight*sizeof(float));

	CAutoHomeBorder* pAutoHomeBorder = m_pAutoHome->GetAutoHomeBorder();
	float fLineWeight;
	const float fDeltaLine = 1.0f / (m_HeightMap.iHeight - 1);
	float fBorderValue, fResultValue;
	float fSmoothDelta = 1.0f / nBorderSize;
	float fSmoothWeight;

	float* pLinePoints = new float[NUM_HOME_BORDER_POINT];
	CCurvePoint cp;

	// Smooth top border
	fLineWeight = 0.0f;
	pAutoHomeBorder->GetHomeTopPoints(pLinePoints);
	cp.SetPointValues(NUM_HOME_BORDER_POINT, pLinePoints);
	for (i=0; i<m_HeightMap.iWidth; i++)
	{
		fBorderValue = cp.GetValue(fLineWeight);
		for (j=0; j<=nBorderSize; j++)
		{
//			fSmoothWeight = CosWeight(CosWeight((nBorderSize-j)*fSmoothDelta));
			fSmoothWeight = CosWeight((nBorderSize-j)*fSmoothDelta);
			fResultValue = (fBorderValue * fSmoothWeight + m_HeightMap.pAbsHeight[j*m_HeightMap.iWidth+i] * (1-fSmoothWeight));
			m_HeightMap.pAbsHeight[j*m_HeightMap.iWidth+i] = fResultValue;
		}
		fLineWeight += fDeltaLine;
	}

	// Smooth bottom border
	fLineWeight = 0.0f;
	pAutoHomeBorder->GetHomeBottomPoints(pLinePoints);
	cp.SetPointValues(NUM_HOME_BORDER_POINT, pLinePoints);
	for (i=0; i<m_HeightMap.iWidth; i++)
	{
		fBorderValue = cp.GetValue(fLineWeight);
		for (j=0; j<=nBorderSize; j++)
		{
//			fSmoothWeight = CosWeight(CosWeight((nBorderSize-j)*fSmoothDelta));
			fSmoothWeight =CosWeight((nBorderSize-j)*fSmoothDelta);
			fResultValue = (fBorderValue * fSmoothWeight + m_HeightMap.pAbsHeight[(m_HeightMap.iHeight-j-1)*m_HeightMap.iWidth+i] * (1-fSmoothWeight));
			m_HeightMap.pAbsHeight[(m_HeightMap.iHeight-j-1)*m_HeightMap.iWidth+i] = fResultValue;
		}
		fLineWeight += fDeltaLine;
	}

	// Smooth left border
	fLineWeight = 0.0f;
	pAutoHomeBorder->GetHomeLeftPoints(pLinePoints);
	cp.SetPointValues(NUM_HOME_BORDER_POINT, pLinePoints);
	for (i=0; i<m_HeightMap.iHeight; i++)
	{
		fBorderValue = cp.GetValue(fLineWeight);
		for (j=0; j<=nBorderSize; j++)
		{
//			fSmoothWeight = CosWeight(CosWeight((nBorderSize-j)*fSmoothDelta));
			fSmoothWeight = CosWeight((nBorderSize-j)*fSmoothDelta);
			fResultValue = (fBorderValue * fSmoothWeight + m_HeightMap.pAbsHeight[i*m_HeightMap.iWidth+j] * (1-fSmoothWeight));
			m_HeightMap.pAbsHeight[i*m_HeightMap.iWidth+j] = fResultValue;
		}
		fLineWeight += fDeltaLine;
	}
	
	// Smooth right border
	fLineWeight = 0.0f;
	pAutoHomeBorder->GetHomeRightPoints(pLinePoints);
	cp.SetPointValues(NUM_HOME_BORDER_POINT, pLinePoints);
	for (i=0; i<m_HeightMap.iHeight; i++)
	{
		fBorderValue = cp.GetValue(fLineWeight);
		for (j=0; j<=nBorderSize; j++)
		{
//			fSmoothWeight = CosWeight(CosWeight((nBorderSize-j)*fSmoothDelta));
			fSmoothWeight = CosWeight((nBorderSize-j)*fSmoothDelta);
			fResultValue = (fBorderValue * fSmoothWeight + m_HeightMap.pAbsHeight[i*m_HeightMap.iWidth+(m_HeightMap.iWidth-j-1)] * (1-fSmoothWeight));
			m_HeightMap.pAbsHeight[i*m_HeightMap.iWidth+(m_HeightMap.iWidth-j-1)] = fResultValue;
		}
		fLineWeight += fDeltaLine;
	}
	
	delete pLinePoints;

//	ResetHeightData(m_fMaxHeight);
//	UpdateVertexNormals();
	
	memcpy(m_HeightMap.pAbsHeightBak, m_HeightMap.pAbsHeight, m_HeightMap.iWidth*m_HeightMap.iHeight*sizeof(float));
	UpdateAbsHeight();

	return true;
}

//	Apply terrain vertices color
bool CAutoTerrain::ApplyTerrainColor()
{
	CAutoSceneConfig* pAutoSceneConfig = m_pAutoHome->GetAutoScene()->GetAutoSceneConfig();
	CSoftGouraud sg;
	DWORD *pdwColorDay;
	DWORD *pdwColorNight;
	sg.Create(m_pAutoHome, pAutoSceneConfig->GetSunDir(),
		pAutoSceneConfig->GetSunColorDay(), pAutoSceneConfig->GetAmbientDay(),
		pAutoSceneConfig->GetSunColorNight(), pAutoSceneConfig->GetAmbientNight());
	sg.CalculateSceneVertsColor(&pdwColorDay, &pdwColorNight);
	if (!ApplyTerrainColor(pdwColorDay, pdwColorNight))
		return false;

	return true;
}

