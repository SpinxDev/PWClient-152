/*
* FILE: A3DWaterArea2.cpp
*
* DESCRIPTION: Class representing for water on the terrain
*
* CREATED BY: liyi, 2012, 3, 5
*
* HISTORY:
*
* Copyright (c) 2004 Archosaur Studio, All Rights Reserved.	
*/


#include "A3DMacros.h"
#include "A3DPI.h"
#include "A3DStream.h"
#include "A3DDevice.h"
#include "A3DViewport.h"
#include "A3DCameraBase.h"
#include "A3DEngine.h"
#include "AFile.h"
#include "A3DHLSL.h"

//FIXME!! water2
#include "A3DWaterArea2.h"
#include "A3DTerrainWater2.h"
#include "A3DOcclusionMan.h"
#include "A3DConfig.h"

//////////////////////////////////////////////////////////////////////////////////////////
//
//	Macros
//
//////////////////////////////////////////////////////////////////////////////////////////

#define WATERAREA_VER 3

//	dwFlags in WATERAREA_FILEHEADER
enum
{
	FHFLAGS_MINORWATER = 0x0001,
};

//////////////////////////////////////////////////////////////////////////////////////////
//
//	Types
//
//////////////////////////////////////////////////////////////////////////////////////////

struct WATERAREA_FILEHEADER
{
	DWORD	idArea;
	float	fWaterHeight;
	float	fEdgeSize;
	int		iWidth;
	int		iHeight;
	float	fGridSize;
	float	vCenter[3];
	int		iNumWaves;
	float	fEdgeHeight;
	float	fNMScale;
	float	vNMOffSpeed0[2];
	float	vNMOffSpeed1[2];
	bool	bMinorWater;
};

//	When WATERAREA_VER >= 2
struct WATERAREA_FILEHEADER2
{
	DWORD	idArea;
	float	fWaterHeight;
	float	fEdgeSize;
	int		iWidth;
	int		iHeight;
	float	fGridSize;
	float	vCenter[3];
	int		iNumWaves;
	float	fEdgeHeight;
	float	fNMScale;
	float	vNMOffSpeed0[2];
	float	vNMOffSpeed1[2];
	DWORD	dwFlags;			//	bool flags, FHFLAGS_xxx
	float	fWaveAlpha;
};

//	When WATERAREA_VER >= 3
struct WATERAREA_FILEHEADER3
{
	DWORD	idArea;
	float	fWaterHeight;
	float	fEdgeSize;
	int		iWidth;
	int		iHeight;
	float	fGridSize;
	float	vCenter[3];
	int		iNumWaves;
	float	fEdgeHeight;
	float	fNMScale;
	float	vNMOffSpeed0[2];
	float	vNMOffSpeed1[2];
	DWORD	dwFlags;			//	bool flags, FHFLAGS_xxx
	float	fWaveAlpha;

	struct
	{
		A3DCOLOR	colWater;
		float		fFogStart;
		float		fFogEnd;
		float		fFogDensity;
		float		fDarkFactor;

	} tp[2];
};

//////////////////////////////////////////////////////////////////////////////////////////
//
//	Implement A3DWaterMeshBlock
//
//////////////////////////////////////////////////////////////////////////////////////////

A3DWaterMeshBlock2::A3DWaterMeshBlock2() :
m_pWaterArea(NULL),
m_iVisGridNum(0)
{
	m_aabb.Clear();

	memset(m_aIdxStrms, 0, sizeof (m_aIdxStrms));
}

A3DWaterMeshBlock2::~A3DWaterMeshBlock2()
{
}

//	Get the highest mesh grid dim
int A3DWaterMeshBlock2::GetHighestMeshGridDim()
{
	int iDetail = 1 << (MAX_LOD_LEVEL - 1);
	return BLOCK_GRID_DIM * iDetail;
}

//	Intialize object
bool A3DWaterMeshBlock2::Init(A3DWaterArea2* pWaterArea, int left, int top)
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
void A3DWaterMeshBlock2::Release()
{
	int i;

	//	Release index streams
	for (i=0; i < MAX_LOD_LEVEL; i++)
	{
		A3DRELEASE(m_aIdxStrms[i]);
	}
}

//	Initialize block data, return visible grid number
bool A3DWaterMeshBlock2::InitData(int left, int top, int* aRows, int* aCols)
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
bool A3DWaterMeshBlock2::CreateStreams(int* aRows, int* aCols)
{
	ASSERT(m_iVisGridNum);

	A3DDevice* pA3DDevice = m_pWaterArea->GetTerrainWater()->GetA3DDevice();

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
void A3DWaterMeshBlock2::AddIndices(WORD* aIndices, int& iIdxCnt, int r, int c, int iStepSize)
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
A3DStream* A3DWaterMeshBlock2::GetIndexStream(int iLOD)
{
	ASSERT(iLOD >= 1 && iLOD <= MAX_LOD_LEVEL);
	return m_aIdxStrms[iLOD-1];
}

//////////////////////////////////////////////////////////////////////////////////////////
//
//	water area object
//
//////////////////////////////////////////////////////////////////////////////////////////

A3DWaterArea2::A3DWaterArea2()
{
	m_pWater		= NULL;
	m_pGridFlags	= NULL;
	m_bMinorWater	= false;
	m_bDoRender		= false;
	m_dwAreaID		= 0;
	m_vGridSize		= 10.0f;
	m_vWaterHeight	= 0.0f;
	m_vEdgeSize		= 10.0f;
	m_fWaveAlpha	= 1.0f;

	m_fEdgeHeight	= 1.0f;
	m_fNMScale		= 0.5f;
	//m_fNMScale		= 5.0f;

	m_vNMOffSpeed0[0] = 0.02f;
	m_vNMOffSpeed0[1] = 0.03f;
	m_vNMOffSpeed1[0] = -0.04f;
	m_vNMOffSpeed1[1] = 0.02f;

    m_pOccProxy     = NULL;
}

A3DWaterArea2::~A3DWaterArea2()
{
	Release();
}

bool A3DWaterArea2::Init(A3DTerrainWater2 * pWater, DWORD dwID, int nWidth, int nHeight, float vGridSize, const A3DVECTOR3& vecCenter, float vWaterHeight)
{
	m_pWater		= pWater;
	m_dwAreaID		= dwID;

	m_nWidth		= nWidth;
	m_nHeight		= nHeight;
	m_vGridSize		= vGridSize;
	m_vecCenter		= vecCenter;
	m_vWaterHeight	= vWaterHeight;

	m_pGridFlags = new BYTE[m_nWidth * m_nHeight];
	memset(m_pGridFlags, 0, sizeof(BYTE) * m_nWidth * m_nHeight);

    CreateOccQuery();

    if (m_pOccProxy)
        m_pOccProxy->SetEnabled(false);
    return true;
}

bool A3DWaterArea2::Release()
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

    if (m_pOccProxy)
    {
        A3DEngine* pA3DEngine = m_pWater->m_pA3DDevice->GetA3DEngine();
        if (pA3DEngine->GetA3DOcclusionMan())
            pA3DEngine->GetA3DOcclusionMan()->ReleaseQuery(m_pOccProxy);
        m_pOccProxy = NULL;
    }

	return true;
}

bool A3DWaterArea2::Load(A3DTerrainWater2 * pWater, AFile * pFileToLoad)
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
		DWORD dwTemp;
		if( !pFileToLoad->Read(&dwTemp, sizeof(A3DCOLOR), &dwReadLen) )
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

		// now load the waves here;
		int nNumWaves = 0;
		if( !pFileToLoad->Read(&nNumWaves, sizeof(int), &dwReadLen) )
			goto READFAIL;
		int	i;
		for(i=0; i<nNumWaves; i++)
		{
			A3DWaterWave2 * pWave = new A3DWaterWave2;
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
		DWORD dwTemp;
		if( !pFileToLoad->Read(&dwTemp, sizeof(A3DCOLOR), &dwReadLen) )
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
			A3DWaterWave2 * pWave = new A3DWaterWave2;
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
		WATERAREA_FILEHEADER3 hd;
		memset(&hd, 0, sizeof (hd));

		if (dwVersion < 2)
		{
			WATERAREA_FILEHEADER hdTemp;
			if (!pFileToLoad->Read(&hdTemp, sizeof (hdTemp), &dwReadLen))
				goto READFAIL;

			int iOffset = (int)(&(((WATERAREA_FILEHEADER*)NULL)->bMinorWater));
			memcpy(&hd, &hdTemp, iOffset);

			hd.dwFlags |= (hdTemp.bMinorWater ? FHFLAGS_MINORWATER : 0);
			hd.fWaveAlpha = 1.0f;
		}
		else if (dwVersion < 3)
		{
			WATERAREA_FILEHEADER2 hdTemp;
			if (!pFileToLoad->Read(&hdTemp, sizeof (hdTemp), &dwReadLen))
				goto READFAIL;

			memcpy(&hd, &hdTemp, sizeof (hdTemp));
		}
		else	//	dwVersion >= 3
		{
			if (!pFileToLoad->Read(&hd, sizeof (hd), &dwReadLen))
				goto READFAIL;
		}

		m_dwAreaID = hd.idArea;
		m_vWaterHeight = hd.fWaterHeight;
		m_vEdgeSize = hd.fEdgeSize;
		m_nWidth = hd.iWidth;
		m_nHeight = hd.iHeight;
		m_vGridSize = hd.fGridSize;
		m_vecCenter.x = hd.vCenter[0];
		m_vecCenter.y = hd.vCenter[1];
		m_vecCenter.z = hd.vCenter[2];
		m_fEdgeHeight = hd.fEdgeHeight;
		m_fNMScale = hd.fNMScale;
		m_vNMOffSpeed0[0] = hd.vNMOffSpeed0[0];
		m_vNMOffSpeed0[1] = hd.vNMOffSpeed0[1];
		m_vNMOffSpeed1[0] = hd.vNMOffSpeed1[0];
		m_vNMOffSpeed1[1] = hd.vNMOffSpeed1[1];
		m_bMinorWater = (hd.dwFlags & FHFLAGS_MINORWATER) != 0;
		m_fWaveAlpha = hd.fWaveAlpha;

		//	Time parameters
		if (dwVersion < 3)
		{
			struct TEMP_TIME_PARAM
			{
				A3DCOLOR	colWater;		//	water color
				float		fFogStart;
				float		fFogEnd;
				float		fDarkFactor;	//	Darkness factor
			};

			TEMP_TIME_PARAM day, night;
			pFileToLoad->Read(&day, sizeof (day), &dwReadLen);
			pFileToLoad->Read(&night, sizeof (night), &dwReadLen);

			m_paramDay.colWater = day.colWater;
			m_paramDay.fFogStart = day.fFogStart;
			m_paramDay.fFogEnd = day.fFogEnd;
			m_paramDay.fFogDensity = 0.004f;
			m_paramDay.fDarkFactor = day.fDarkFactor;

			m_paramNight.colWater = night.colWater;
			m_paramNight.fFogStart = night.fFogStart;
			m_paramNight.fFogEnd = night.fFogEnd;
			m_paramNight.fFogDensity = 0.004f;
			m_paramNight.fDarkFactor = night.fDarkFactor;
		}
		else	//	dwVersion >= 3
		{
			m_paramDay.colWater = hd.tp[0].colWater;
			m_paramDay.fFogStart = hd.tp[0].fFogStart;
			m_paramDay.fFogEnd = hd.tp[0].fFogEnd;
			m_paramDay.fFogDensity = hd.tp[0].fFogDensity;
			m_paramDay.fDarkFactor = hd.tp[0].fDarkFactor;

			m_paramNight.colWater = hd.tp[1].colWater;
			m_paramNight.fFogStart = hd.tp[1].fFogStart;
			m_paramNight.fFogEnd = hd.tp[1].fFogEnd;
			m_paramNight.fFogDensity = hd.tp[1].fFogDensity;
			m_paramNight.fDarkFactor = hd.tp[1].fDarkFactor;
		}

		//	then read in the grid visible flags
		m_pGridFlags = new BYTE[m_nWidth * m_nHeight];
		if (!pFileToLoad->Read(m_pGridFlags, sizeof(BYTE) * m_nWidth * m_nHeight, &dwReadLen))
			goto READFAIL;

		// now load the waves here;
		int	i;
		for (i=0; i < hd.iNumWaves; i++)
		{
			A3DWaterWave2* pWave = new A3DWaterWave2;
			if (!pWave->Load(this, pFileToLoad))
			{
				g_A3DErrLog.Log("A3DWaterArea::Load(), failed to load one wave!");
				return false;
			}

			m_theWaves.Add(pWave);
		}
	}

    CreateOccQuery();
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

bool A3DWaterArea2::Save(AFile* pFileToSave)
{
	DWORD dwWriteLen;
	DWORD dwVersion = WATERAREA_VER;

	// first write current version of the area
	if( !pFileToSave->Write(&dwVersion, sizeof(DWORD), &dwWriteLen) )
		goto WRITEFAIL;

	WATERAREA_FILEHEADER3 hd;
	memset(&hd, 0, sizeof (hd));

	hd.idArea = m_dwAreaID;
	hd.fWaterHeight = m_vWaterHeight;
	hd.fEdgeSize = m_vEdgeSize;
	hd.iWidth = m_nWidth;
	hd.iHeight = m_nHeight;
	hd.fGridSize = m_vGridSize;
	hd.vCenter[0] = m_vecCenter.x;
	hd.vCenter[1] = m_vecCenter.y;
	hd.vCenter[2] = m_vecCenter.z;
	hd.fEdgeHeight = m_fEdgeHeight;
	hd.fNMScale = m_fNMScale;
	hd.vNMOffSpeed0[0] = m_vNMOffSpeed0[0];
	hd.vNMOffSpeed0[1] = m_vNMOffSpeed0[1];
	hd.vNMOffSpeed1[0] = m_vNMOffSpeed1[0];
	hd.vNMOffSpeed1[1] = m_vNMOffSpeed1[1];
	hd.iNumWaves = m_theWaves.GetSize();
	hd.dwFlags |= (m_bMinorWater ? FHFLAGS_MINORWATER : 0);
	hd.fWaveAlpha = m_fWaveAlpha;

	//	Time parameters
	hd.tp[0].colWater = m_paramDay.colWater;
	hd.tp[0].fFogStart = m_paramDay.fFogStart;
	hd.tp[0].fFogEnd = m_paramDay.fFogEnd;
	hd.tp[0].fFogDensity = m_paramDay.fFogDensity;
	hd.tp[0].fDarkFactor = m_paramDay.fDarkFactor;

	hd.tp[1].colWater = m_paramNight.colWater;
	hd.tp[1].fFogStart = m_paramNight.fFogStart;
	hd.tp[1].fFogEnd = m_paramNight.fFogEnd;
	hd.tp[1].fFogDensity = m_paramNight.fFogDensity;
	hd.tp[1].fDarkFactor = m_paramNight.fDarkFactor;

	if (!pFileToSave->Write(&hd, sizeof (hd), &dwWriteLen))
		goto WRITEFAIL;

	//	then write out the grid visible flags
	if (!pFileToSave->Write(m_pGridFlags, sizeof(BYTE) * m_nWidth * m_nHeight, &dwWriteLen))
		goto WRITEFAIL;

	//	now save the waves here;
	int	i;
	for (i=0; i<hd.iNumWaves; i++)
	{
		A3DWaterWave2 * pWave = m_theWaves[i];
		if (!pWave->Save(pFileToSave))
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
//bool A3DWaterArea::CopyWaveToBuffer(A3DWAVEVERTEX * pVerts, WORD * pIndices, int nStartVert)
//{
//	int nNumWaves = m_theWaves.GetSize();
//
//	int nVertCount = 0;
//	int nFaceCount = 0;
//	for(int i=0; i<nNumWaves; i++)
//	{
//		A3DWaterWave * pWave = m_theWaves[i];
//
//		if( !pWave->CopyToBuffer(pVerts + nVertCount, pIndices + nFaceCount * 3, nStartVert + nVertCount) )
//		{
//			g_A3DErrLog.Log("A3DWaterArea::CopyWaveToBuffer(), failed to call A3DWaterWave::CopyToBuffer()!");
//			return false;
//		}
//
//		nVertCount += pWave->GetVertCount();
//		nFaceCount += pWave->GetFaceCount();
//	}
//
//	return true;
//}

int A3DWaterArea2::GetWaveVertCount()
{
	int nVertCount = 0;
	int nNumWaves = m_theWaves.GetSize();

	for(int i=0; i<nNumWaves; i++)
	{
		A3DWaterWave2 * pWave = m_theWaves[i];
		nVertCount += pWave->GetVertCount();
	}

	return nVertCount;
}

int A3DWaterArea2::GetWaveFaceCount()
{
	int nFaceCount = 0;
	int nNumWaves = m_theWaves.GetSize();

	for(int i=0; i<nNumWaves; i++)
	{
		A3DWaterWave2 * pWave = m_theWaves[i];
		nFaceCount += pWave->GetFaceCount();
	}

	return nFaceCount;
}

// update area vertex and indices according to new grid flags
bool A3DWaterArea2::UpdateAll()
{
	// now update the vertex buffer of all waves
	int nNumWaves = m_theWaves.GetSize();
	for(int i=0; i<nNumWaves; i++)
	{
		A3DWaterWave2 * pWave = m_theWaves[i];
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

void A3DWaterArea2::SetGridFlag(int x, int y, BYTE flag)
{
	assert(x >= 0 && x < m_nWidth && y >= 0 && y < m_nHeight);
	m_pGridFlags[y * m_nWidth + x] = flag;

	//	Build visible grids aabb
	BuildVisGridsAABB();
}

void A3DWaterArea2::ClearGridFlag(int x, int y, BYTE flag)
{
	assert(x >= 0 && x < m_nWidth && y >= 0 && y < m_nHeight);
	m_pGridFlags[y * m_nWidth + x] &= ~flag;

	//	Build visible grids aabb
	BuildVisGridsAABB();
}

//	Build visible grids aabb
bool A3DWaterArea2::BuildVisGridsAABB()
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

    if (m_pOccProxy)
    {
        m_pOccProxy->SetEnabled(true);
        m_pOccProxy->SetAABB(m_aabbVisGrids);
    }
	return true;
}

// adjust the water area's width and height, after adjusting, all grid are invisible
bool A3DWaterArea2::AdjustWidthHeight(int nWidth, int nHeight)
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

bool A3DWaterArea2::ShiftCenterTo(const A3DVECTOR3& vecCenter)
{
	A3DVECTOR3 vecDelta = vecCenter - m_vecCenter;
	m_vecCenter = vecCenter;

	int nNumWaves = m_theWaves.GetSize();
	for(int i=0; i<nNumWaves; i++)
	{
		A3DWaterWave2 * pWave = m_theWaves[i];
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
bool A3DWaterArea2::AddWave(float vEdgeSize, int nMaxSegment, A3DWaterWave2 ** ppWave)
{
	A3DWaterWave2 * pWave = new A3DWaterWave2;
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

bool A3DWaterArea2::DeleteWave(int nIndex)
{
	A3DWaterWave2 * pWave = m_theWaves[nIndex];

	pWave->Release();
	delete pWave;

	m_theWaves.RemoveAt(nIndex);
	return true;
}

bool A3DWaterArea2::RemoveAllWaves()
{
	int nNumWaves = m_theWaves.GetSize();

	for(int i=0; i<nNumWaves; i++)
	{
		A3DWaterWave2 * pWave = m_theWaves[i];

		pWave->Release();
		delete pWave;
	}

	m_theWaves.RemoveAll();
	return true;
}

//	Check if specified position is in a visible grid
bool A3DWaterArea2::IsInVisbleGrid(const A3DVECTOR3& vecPos)
{
	int nX = (int)((vecPos.x - m_vecCenter.x) / m_vGridSize + m_nWidth / 2.0f);
	int nY = (int)((m_vecCenter.z - vecPos.z) / m_vGridSize + m_nHeight / 2.0f);

	if (nX >= m_nWidth || nX < 0 ||
		nY >= m_nHeight || nY < 0)
		return false;

	return m_pGridFlags[nY * m_nWidth + nX] ? true : false;
}

//	Create static meshes
bool A3DWaterArea2::CreateStaticMeshes()
{
	int r, c;

	for (r=0; r < m_nHeight; r+=A3DWaterMeshBlock2::BLOCK_GRID_DIM)
	{
		for (c=0; c < m_nWidth; c+=A3DWaterMeshBlock2::BLOCK_GRID_DIM)
		{
			A3DWaterMeshBlock2* pMeshBlk = new A3DWaterMeshBlock2;
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
bool A3DWaterArea2::DrawStaticMeshes(A3DViewport* pViewport, A3DHLSL* pHLSL, float fViewDist, bool bPPL)
{
	int i;
	A3DCameraBase* pCamera = pViewport->GetCamera();
	A3DDevice* pA3DDevice = m_pWater->GetA3DDevice();
	const A3DVECTOR3& vEyePos = pCamera->GetPos();
	
	for (i=0; i < m_aMeshBlks.GetSize(); i++)
	{
		A3DWaterMeshBlock2* pMeshBlk = m_aMeshBlks[i];
		const A3DAABB& aabbBlk = pMeshBlk->GetMeshAABB();

		if (!pCamera->AABBInViewFrustum(aabbBlk.Mins, aabbBlk.Maxs))
			continue;

		float fBlkRadius = aabbBlk.Extents.Magnitude();
		A3DVECTOR3 vDelta = vEyePos - aabbBlk.Center;
		float fDist = vDelta.MagnitudeH() - fBlkRadius;
		if (fDist >= fViewDist)
			continue;	//	Block is out of view sight

		//	Upload mesh block parameters
		const ARectF& rcBlkArea = pMeshBlk->GetBlockArea();
		A3DVECTOR4 vMeshBlkParam( rcBlkArea.left, m_vWaterHeight, rcBlkArea.top, m_vGridSize);
		pHLSL->SetValue4f("g_vMeshBlkParams", &vMeshBlkParam);
		pHLSL->Commit();

		//	Decide LOD level
		int iLOD = 3;
		if (!bPPL)
		{
			fDist = vDelta.Magnitude() - fBlkRadius;
			if (fDist < 64.0f)
				iLOD = 1;
			else if (fDist < 128.0f)
				iLOD = 2;
		}

		//	Apply index stream
		A3DStream* pIdxStream = pMeshBlk->GetIndexStream(iLOD);
		pIdxStream->AppearIndexOnly();

		int iPrimCnt = pIdxStream->GetIndexCount() / 3;
		int iVertCnt = m_pWater->GetMaxVertNum();

		pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, 0, iVertCnt, 0, iPrimCnt);
	}

	return true;
}

//	Get normal map scroll speed for high quality water
//	iIndex: normal map index. 0 or 1
void A3DWaterArea2::GetNMScrollSpeed(int iIndex, float& fOffU, float& fOffV) const
{
	if (iIndex == 0)
	{
		fOffU = m_vNMOffSpeed0[0];
		fOffV = m_vNMOffSpeed0[1];
	}
	else if (iIndex == 1)
	{
		fOffU = m_vNMOffSpeed1[0];
		fOffV = m_vNMOffSpeed1[1];
	}
	else
	{
		ASSERT(0);
	}
}

//	Set normal map scroll speed for high quality water
//	iIndex: normal map index. 0 or 1
void A3DWaterArea2::SetNMScrollSpeed(int iIndex, float fOffU, float fOffV)
{
	if (iIndex == 0)
	{
		m_vNMOffSpeed0[0] = fOffU;
		m_vNMOffSpeed0[1] = fOffV;
	}
	else if (iIndex == 1)
	{
		m_vNMOffSpeed1[0] = fOffU;
		m_vNMOffSpeed1[1] = fOffV;
	}
	else
	{
		ASSERT(0);
	}
}

//	Get time parameters
//	bDay: true, get day paraemters; false, get night parameters
A3DWaterArea2::TIME_PARAM* A3DWaterArea2::GetTimeParams(bool bDay)
{
	if (bDay)
		return &m_paramDay;
	else
		return &m_paramNight;
}

A3DCOLOR A3DWaterArea2::GetCurWaterColor() const
{
	float fDNFactor = m_pWater->GetDNFactor();
	A3DCOLORVALUE col1(m_paramDay.colWater);
	A3DCOLORVALUE col2(m_paramNight.colWater);
	A3DCOLORVALUE col = col1 * (1.0f - fDNFactor) + col2 * fDNFactor;
	return col.ToRGBAColor();
}

float A3DWaterArea2::GetCurFogStart() const
{
	float fDNFactor = m_pWater->GetDNFactor();
	float fFogStart = m_paramDay.fFogStart * (1.0f - fDNFactor) + m_paramNight.fFogStart * fDNFactor;
	return fFogStart;
}

float A3DWaterArea2::GetCurFogEnd() const
{
	float fDNFactor = m_pWater->GetDNFactor();
	float fFogEnd = m_paramDay.fFogEnd * (1.0f - fDNFactor) + m_paramNight.fFogEnd * fDNFactor;
	return fFogEnd;
}

float A3DWaterArea2::GetCurFogDensity() const
{
	float fDNFactor = m_pWater->GetDNFactor();
	float fFogDensity = m_paramDay.fFogDensity * (1.0f - fDNFactor) + m_paramNight.fFogDensity * fDNFactor;
	return fFogDensity;
}

float A3DWaterArea2::GetCurDarkFactor() const
{
	float fDNFactor = m_pWater->GetDNFactor();
	float fDarkFactor = m_paramDay.fDarkFactor * (1.0f - fDNFactor) + m_paramNight.fDarkFactor * fDNFactor;
	return fDarkFactor;
}

bool A3DWaterArea2::IsOccluded(A3DViewport* pViewport) const
{
    
    //occlusion culling
    if (m_pOccProxy && g_pA3DConfig->RT_GetUseOcclusionCullingFlag())
    {
        return m_pOccProxy->IsOccluded(pViewport);
    }
    else
    {
        return false;
    }

}

void A3DWaterArea2::CreateOccQuery()
{
    A3DEngine* pA3DEngine = m_pWater->m_pA3DDevice->GetA3DEngine();

    A3DOcclusionMan* pOccMan = pA3DEngine->GetA3DOcclusionMan();

    if (pOccMan)
    {
        if (m_pOccProxy)
            pOccMan->ReleaseQuery(m_pOccProxy);

        m_pOccProxy = pOccMan->CreateQuery();
    }
}