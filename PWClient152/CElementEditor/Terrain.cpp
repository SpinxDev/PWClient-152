 /*
 * FILE: Terrain.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/6/15
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "Global.h"
#include "Terrain.h"
#include "TerrainLayer.h"
#include "TerrainRender.h"
#include "MaskModifier.h"
#include "ElementMap.h"
#include "EL_Archive.h"
#include "MainFrm.h"
#include "ViewFrame.h"
#include "Viewport.h"
#include "Render.h"
#include "DataChunkFile.h"

#include "AC.h"
#include "A3D.h"

//#define new A_DEBUG_NEW

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

#define TERRAINFILE_VERSION		2

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

//	Calculate triangle's normal
static inline A3DVECTOR3 _CalcTriangleNormal(const A3DVECTOR3& v0, const A3DVECTOR3& v1, const A3DVECTOR3& v2)
{
	A3DVECTOR3 e1 = v1 - v0;
	A3DVECTOR3 e2 = v2 - v0;
	A3DVECTOR3 vNormal = CrossProduct(e1, e2);
	vNormal.Normalize();
	return vNormal;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CTerrain
//	
///////////////////////////////////////////////////////////////////////////

CTerrain::CTerrain(CElementMap* pMap)
{
	m_pMap			= pMap;
	m_pTrnRender	= NULL;
	m_pMaskModify	= NULL;
	m_fTerrainSize	= 0.0f;
	m_fYOffset		= 0.0f;
	m_fTileSize		= 0.0f;
	m_fMaxHeight	= 100.0f;
	m_dwRndUpdate1	= 0;
	m_dwRndUpdate2	= 0;
	m_bSaveNormal	= false;
	m_dwVersion		= TERRAINFILE_VERSION;
	m_pMaskMapFile	= NULL;
	m_aPos			= NULL;

	memset(&m_HeightMap, 0, sizeof (m_HeightMap));
}

CTerrain::~CTerrain()
{
}

//	Initalize object
bool CTerrain::Init(const INITDATA& InitData)
{
	if (!CommonInit())
		return false;
	
	m_fTileSize		= InitData.fTileSize;
	m_fYOffset		= InitData.fMinHeight;
	m_fMaxHeight	= InitData.fMaxHeight - InitData.fMinHeight;

	//	Calculate terrain area
	CalcTerrainArea();

	//	Create default layer
	CTerrainLayer* pLayer = CreateLayer("Default", true);
	if (!pLayer)
		return false;

	//	Set default texture to default layer
	CString strFile;
	strFile.Format("%s%s", g_szWorkDir, g_szDefTexFile);

	if (!pLayer->ChangeTexture(strFile))
	{
		g_Log.Log("CTerrain::Init, Failed to load Temp\\notex.bmp");
		return false;
	}

	return true;
}

//	Initialize some common data
bool CTerrain::CommonInit()
{
	if (!(m_pMaskModify = new CMaskModifier(this)))
		return false;

	if (!m_pMaskModify->Init())
	{
		g_Log.Log("CTerrain::CommonInit, Failed to initalize mask modify");
		return false;
	}

	return true;
}

//	Release object
void CTerrain::Release()
{
	if (m_pMaskModify)
	{
		m_pMaskModify->Release();
		delete m_pMaskModify;
		m_pMaskModify = NULL;
	}

	if (m_pMaskMapFile)
	{
		m_pMaskMapFile->Close();
		delete m_pMaskMapFile;
		m_pMaskMapFile = NULL;
	}

	if (m_aPos)
	{
		a_free(m_aPos);
		m_aPos = NULL;
	}


	//	Release terrain render
	ReleaseRender();

	//	Release height map
	ReleaseHeightMap();

	//	Release all layers
	ReleaseAllLayers();
}

//	Calculate terrain area
void CTerrain::CalcTerrainArea()
{
	float fOffX, fOffZ;
	m_pMap->GetMapOffset(fOffX, fOffZ);
	m_rcTerrain.left	= fOffX - m_fTerrainSize * 0.5f;
	m_rcTerrain.top		= fOffZ + m_fTerrainSize * 0.5f;
	m_rcTerrain.right	= m_rcTerrain.left + m_fTerrainSize;
	m_rcTerrain.bottom	= m_rcTerrain.top - m_fTerrainSize;
}

//	Create empty height map
bool CTerrain::CreateEmptyHeightMap(int iWid, int iHei)
{
	//	Release old height map
	ReleaseHeightMap();

	m_HeightMap.iWidth	= iWid;
	m_HeightMap.iHeight	= iHei;
	m_fTerrainSize	= m_fTileSize * (m_HeightMap.iWidth - 1);

	//	Create row height map data
	int iSize = iWid * iHei * sizeof (float);
	m_HeightMap.pHeightData = (float*)a_malloc(iSize);
	if (!m_HeightMap.pHeightData)
	{
		g_Log.Log("CTerrain::CreateEmptyHeightMap, Not enough memory !");
		return false;
	}

	//	Clear height map data
	memset(m_HeightMap.pHeightData, 0, iSize);

	return true;
}

//	Load height map from raw data file
bool CTerrain::LoadHeightMap(int iWid, int iHei, int iType, const char* szFile, bool bExtraData)
{
	//	Release old height map
	ReleaseHeightMap();

	if (!CreateEmptyHeightMap(iWid, iHei))
	{
		g_Log.Log("CTerrain::LoadHeightMap, Failed to create empty height map !");
		return false;
	}

	FILE* fp = fopen(szFile, "rb");
	if (!fp)
	{
		g_Log.Log("CTerrain::LoadHeightMap, Failed to open height map file %s !", szFile);
		return false;
	}

	if (iType == HM_FLOAT)
	{
		int iSize = iWid * iHei * sizeof (float);
		if (fread(m_HeightMap.pHeightData, 1, iSize, fp) != iSize)
		{
			fclose(fp);
			g_Log.Log("CTerrain::LoadHeightMap, Failed to read height map data. %s !", szFile);
			return false;
		}
	}
	else if (iType == HM_16BIT)
	{
		int iSize = iWid * iHei * sizeof (WORD);
		WORD* pSrcData = (WORD*)a_malloc(iSize);
		if (!pSrcData)
		{
			g_Log.Log("CTerrain::LoadHeightMap, Not enough memory !");
			return false;
		}

		if (fread(pSrcData, 1, iSize, fp) != iSize)
		{
			fclose(fp);
			g_Log.Log("CTerrain::LoadHeightMap, Failed to read height map data. %s !", szFile);
			return false;
		}

		float* pDst = m_HeightMap.pHeightData;
		WORD* pSrc = pSrcData;
		float fScale = 1.0f / 65535.0f;
		int iNumPixel = iWid * iHei;

		for (int i=0; i < iNumPixel; i++, pDst++, pSrc++)
		{
			float f = *pSrc * fScale;
			*pDst = *pSrc * fScale;
		}
				
		a_free(pSrcData);
	}
	else if (iType == HM_8BIT)
	{
		int iSize = iWid * iHei;
		BYTE* pSrcData = (BYTE*)a_malloc(iSize);
		if (!pSrcData)
		{
			g_Log.Log("CTerrain::LoadHeightMap, Not enough memory !");
			return false;
		}

		if (fread(pSrcData, 1, iSize, fp) != iSize)
		{
			fclose(fp);
			g_Log.Log("CTerrain::LoadHeightMap, Failed to read height map data. %s !", szFile);
			return false;
		}

		float* pDst = m_HeightMap.pHeightData;
		BYTE* pSrc = pSrcData;
		float fScale = 1.0f / 255.0f;

		for (int i=0; i < iSize; i++, pDst++, pSrc++)
			*pDst = *pSrc * fScale;
		
		a_free(pSrcData);
	}
	else
		ASSERT(0);

	fclose(fp);

	//	Calculate terrain area
	CalcTerrainArea();
	//	Create absolute height data and vertex normals
	if (bExtraData)
	{
		if (!CreateAbsHeightData() || !CreateVertexNormals() || !CreateTerrainMesh())
		{
			g_Log.Log("CTerrain::LoadHeightMap, Failed to call CreateVertexNormals !");
			return false;
		}
	}

	return true;
}

//	Release current height map
void CTerrain::ReleaseHeightMap()
{
	if (m_HeightMap.pHeightData)
		a_free(m_HeightMap.pHeightData);

	if (m_HeightMap.pAbsHeight)
		a_free(m_HeightMap.pAbsHeight);

	if (m_HeightMap.aNormals)
		a_free(m_HeightMap.aNormals);

	memset(&m_HeightMap, 0, sizeof (m_HeightMap));
}

//	Create absolute height data
bool CTerrain::CreateAbsHeightData()
{
	ASSERT(m_HeightMap.pHeightData);

	//	Release current data
	if (m_HeightMap.pAbsHeight)
	{
		a_free(m_HeightMap.pAbsHeight);
		m_HeightMap.pAbsHeight = NULL;
	}

	int iNumVert = m_HeightMap.iWidth * m_HeightMap.iHeight;

	if (!(m_HeightMap.pAbsHeight = (float*)a_malloc(iNumVert * sizeof (float))))
	{
		g_Log.Log("CTerrain::CreateAbsHeightData, not enough memory !");
		return false;
	}
	
	for (int i=0; i < iNumVert; i++)
		m_HeightMap.pAbsHeight[i] = GetRealHeight(i);

	return true;
}

//	Create vertex normals
bool CTerrain::CreateVertexNormals()
{
	ASSERT(m_HeightMap.pAbsHeight);

	//	Release current normals
	if (m_HeightMap.aNormals)
	{
		a_free(m_HeightMap.aNormals);
		m_HeightMap.aNormals = NULL;
	}

	int iNumVert = m_HeightMap.iWidth * m_HeightMap.iHeight;

	if (!(m_HeightMap.aNormals = (A3DVECTOR3*)a_malloc(iNumVert * sizeof (A3DVECTOR3))))
	{
		g_Log.Log("CTerrain::CreateVertexNormals, not enough memory !");
		return false;
	}

	A3DVECTOR3 v0, v1, v2, vNormal;
	int i0, i, j;
	float x, z;
	float* aHeis = m_HeightMap.pAbsHeight;

	//	Left-top corner vertex
	v0.Set(0.0f, aHeis[0], 0.0f);
	v1.Set(m_fTileSize, aHeis[1], 0.0f);
	v2.Set(0.0f, aHeis[m_HeightMap.iWidth], -m_fTileSize);
	m_HeightMap.aNormals[0] = _CalcTriangleNormal(v0, v1, v2);

	//	Right-top corner vertex
	i0 = m_HeightMap.iWidth - 1;
	v0.Set(m_fTerrainSize, aHeis[i0], 0.0f);
	v1.Set(m_fTerrainSize, aHeis[i0+m_HeightMap.iWidth], -m_fTileSize);
	v2.Set(m_fTerrainSize-m_fTileSize, aHeis[i0-1], 0.0f);
	m_HeightMap.aNormals[i0] = _CalcTriangleNormal(v0, v1, v2);

	//	Left-bottom corner vertex
	i0 = m_HeightMap.iWidth * (m_HeightMap.iHeight - 1);
	v0.Set(0.0f, aHeis[i0], -m_fTerrainSize);
	v1.Set(0.0f, aHeis[i0-m_HeightMap.iWidth], -m_fTerrainSize+m_fTileSize);
	v2.Set(m_fTileSize, aHeis[i0+1], -m_fTerrainSize);
	m_HeightMap.aNormals[i0] = _CalcTriangleNormal(v0, v1, v2);

	//	Right-bottom corner vertex
	i0 = m_HeightMap.iWidth * m_HeightMap.iHeight - 1;
	v0.Set(m_fTerrainSize, aHeis[i0], -m_fTerrainSize);
	v1.Set(m_fTerrainSize-m_fTileSize, aHeis[i0-1], -m_fTerrainSize);
	v2.Set(m_fTerrainSize, aHeis[i0-m_HeightMap.iWidth], -m_fTerrainSize+m_fTileSize);
	m_HeightMap.aNormals[i0] = _CalcTriangleNormal(v0, v1, v2);

	//	Left border
	i0	= m_HeightMap.iWidth;
	x	= 0.0f;
	z	= -m_fTileSize;

	for (i=1; i < m_HeightMap.iHeight-1; i++)
	{
		vNormal.Clear();
		
		v0.Set(x, aHeis[i0], z);
		v1.Set(x, aHeis[i0-m_HeightMap.iWidth], z+m_fTileSize);
		v2.Set(x+m_fTileSize, aHeis[i0+1], z);
		vNormal += _CalcTriangleNormal(v0, v1, v2);

		v1.Set(x, aHeis[i0+m_HeightMap.iWidth], z-m_fTileSize);
		vNormal += _CalcTriangleNormal(v0, v2, v1);

		vNormal.Normalize();
		m_HeightMap.aNormals[i0] = vNormal;

		i0 += m_HeightMap.iWidth;
		z  -= m_fTileSize;
	}

	//	Top border
	i0	= 1;
	x	= m_fTileSize;
	z	= 0.0f;

	for (i=1; i < m_HeightMap.iWidth-1; i++)
	{
		vNormal.Clear();
		
		v0.Set(x, aHeis[i0], z);
		v1.Set(x, aHeis[i0+m_HeightMap.iWidth], z-m_fTileSize);
		v2.Set(x-m_fTileSize, aHeis[i0-1], z);
		vNormal += _CalcTriangleNormal(v0, v1, v2);

		v2.Set(x+m_fTileSize, aHeis[i0+1], z);
		vNormal += _CalcTriangleNormal(v0, v2, v1);

		vNormal.Normalize();
		m_HeightMap.aNormals[i0] = vNormal;

		i0++;
		x += m_fTileSize;
	}

	//	Right border
	i0	= m_HeightMap.iWidth * 2 - 1;
	x	= m_fTerrainSize;
	z	= -m_fTileSize;
	
	for (i=1; i < m_HeightMap.iHeight-1; i++)
	{
		vNormal.Clear();
		
		v0.Set(x, aHeis[i0], z);
		v1.Set(x-m_fTileSize, aHeis[i0-1], z);
		v2.Set(x, aHeis[i0-m_HeightMap.iWidth], z+m_fTileSize);
		vNormal += _CalcTriangleNormal(v0, v1, v2);

		v2.Set(x, aHeis[i0+m_HeightMap.iWidth], z-m_fTileSize);
		vNormal += _CalcTriangleNormal(v0, v2, v1);

		vNormal.Normalize();
		m_HeightMap.aNormals[i0] = vNormal;

		i0 += m_HeightMap.iWidth;
		z  += m_fTileSize;
	}

	//	Bottom border
	i0	= m_HeightMap.iWidth * (m_HeightMap.iHeight - 1) + 1;
	x	= m_fTileSize;
	z	= -m_fTerrainSize;
	
	for (i=1; i < m_HeightMap.iWidth-1; i++)
	{
		vNormal.Clear();
		
		v0.Set(x, aHeis[i0], z);
		v1.Set(x-m_fTileSize, aHeis[i0-1], z);
		v2.Set(x, aHeis[i0-m_HeightMap.iWidth], z+m_fTileSize);
		vNormal += _CalcTriangleNormal(v0, v1, v2);

		v1.Set(x+m_fTileSize, aHeis[i0+1], z);
		vNormal += _CalcTriangleNormal(v0, v2, v1);

		vNormal.Normalize();
		m_HeightMap.aNormals[i0] = vNormal;

		i0++;
		x += m_fTileSize;
	}

	//	Center vertices
	for (i=1; i < m_HeightMap.iHeight-1; i++)
	{
		x	= m_fTileSize;
		z	= -i * m_fTileSize;
		i0	= i * m_HeightMap.iWidth + 1;

		for (j=1; j < m_HeightMap.iWidth-1; j++, i0++)
		{
			vNormal.Clear();
		
			v0.Set(x, aHeis[i0], z);
			v1.Set(x-m_fTileSize, aHeis[i0-1], z);
			v2.Set(x, aHeis[i0-m_HeightMap.iWidth], z+m_fTileSize);
			vNormal += _CalcTriangleNormal(v0, v1, v2);
			
			v1 = v2;
			v2.Set(x+m_fTileSize, aHeis[i0+1], z);
			vNormal += _CalcTriangleNormal(v0, v1, v2);

			v1 = v2;
			v2.Set(x, aHeis[i0+m_HeightMap.iWidth], z-m_fTileSize);
			vNormal += _CalcTriangleNormal(v0, v1, v2);

			v1 = v2;
			v2.Set(x-m_fTileSize, aHeis[i0-1], z);
			vNormal += _CalcTriangleNormal(v0, v1, v2);

			vNormal.Normalize();
			m_HeightMap.aNormals[i0] = vNormal;

			x += m_fTileSize;
		}
	}

	return true;
}

//	Frame move routine
bool CTerrain::FrameMove(DWORD dwDeltaTime, A3DCameraBase* pA3DCamera/* NULL */)
{
	if (!m_pTrnRender)
		return true;

	//	Change view radius
	m_pTrnRender->SetViewRadius(2000.0f);

	if (!pA3DCamera)
	{
		//	Use current viewport's camera
		CViewport* pViewport = AUX_GetMainFrame()->GetViewFrame()->GetActiveViewport();
		pA3DCamera = pViewport->GetA3DCamera();
	}

	m_pTrnRender->Update(dwDeltaTime, pA3DCamera, pA3DCamera->GetPos());

	return true;
}

//	render routine
bool CTerrain::Render(A3DViewport* pA3DViewport)
{
	if (!m_pTrnRender)
		return true;

	//	test code...
/*	static float fDNFactor = 0.0f;

	if (GetAsyncKeyState(VK_F7) & 0x8000)
		fDNFactor -= 0.01f;
	else if (GetAsyncKeyState(VK_F8) & 0x8000)
		fDNFactor += 0.01f;

	if (fDNFactor < 0.0f)
		fDNFactor = 0.0f;
	else if (fDNFactor > 1.0f)
		fDNFactor = 1.0f;

	m_pTrnRender->SetDNFactor(fDNFactor);
*/
	m_pTrnRender->SetMaxLayerWeight(g_Configs.iMaxLayerWeight);
	m_pTrnRender->Render(pA3DViewport);

	if (m_pMaskModify)
		m_pMaskModify->Render(pA3DViewport);
#if _DEBUG
	char szMsg[100];
	sprintf(szMsg, "Rendered block number: %d", m_pTrnRender->GetRenderedBlockNum());
	g_Render.TextOut(10, 10, szMsg, 0xffffff00);
#endif
	return true;
}

//	Ray trace function
bool CTerrain::RayTrace(const A3DVECTOR3& vStart, const A3DVECTOR3& vVel, float fTime, RAYTRACERT* pTraceRt)
{
	if (!m_HeightMap.pHeightData || !m_pTrnRender)
		return false;

	return m_pTrnRender->RayTrace(vStart, vVel, fTime, pTraceRt);
}

//	Get terrain height of specified position
float CTerrain::GetPosHeight(const A3DVECTOR3& vPos)
{
	if (!m_HeightMap.pHeightData || !m_pTrnRender)
		return m_fYOffset;

	return m_pTrnRender->GetPosHeight(vPos);
}

//	Get normal of specified position
bool CTerrain::GetPosNormal(const A3DVECTOR3& vPos, A3DVECTOR3& vNormal)
{
	vNormal = g_vAxisY;
	return false;
}

//	Create a new layer
CTerrainLayer* CTerrain::CreateLayer(const char* szName, bool bDefault)
{
	ASSERT(szName && szName[0]);

	CTerrainLayer* pLayer = new CTerrainLayer(this);
	if (!pLayer)
		return NULL;

	if (!pLayer->Init(szName, bDefault))
		return NULL;

	pLayer->SetScaleU(1.0f);
	pLayer->SetScaleV(1.0f);
	pLayer->SetProjAxis(1);

	//	Add to array
	m_aLayers.Add(pLayer);
	
	return pLayer;
}

//	Remove a layer but not release it
CTerrainLayer* CTerrain::RemoveLayer(int iIndex)
{
	CTerrainLayer* pLayer = m_aLayers[iIndex];
	m_aLayers.RemoveAt(iIndex);
	return pLayer;
}

//	Release a layer and remove it from array
void CTerrain::ReleaseLayer(int iIndex)
{
	CTerrainLayer* pLayer = RemoveLayer(iIndex);
	if (pLayer)
	{
		pLayer->Release();
		delete pLayer;
	}
}

//	Search layer base on name
int CTerrain::SearchLayer(const char* szName)
{
	for (int i=0; i < m_aLayers.GetSize(); i++)
	{
		CTerrainLayer* pLayer = m_aLayers[i];
		if (!stricmp(pLayer->GetName(), szName))
			return i;
	}

	return -1;
}

//	Get layer's index
int CTerrain::GetLayerIndex(CTerrainLayer* pLayer)
{
	for (int i=0; i < m_aLayers.GetSize(); i++)
	{
		if (pLayer == m_aLayers[i])
			return i;
	}

	return -1;
}

//	Release all layers
void CTerrain::ReleaseAllLayers()
{
	for (int i=0; i < m_aLayers.GetSize(); i++)
	{
		CTerrainLayer* pLayer = m_aLayers[i];
		pLayer->Release();
		delete pLayer;
	}

	m_aLayers.RemoveAll();
}

//	Load data
bool CTerrain::Load(CELArchive& ar)
{
	if (!CommonInit())
		return false;
	
	//	Read terrain information
	TERRAINFILEDATA Data;
	ar.Read(&Data, sizeof (Data));

	m_fTileSize		= Data.fTileSize;
	m_fMaxHeight	= Data.fMaxHeight;
	m_fYOffset		= Data.fYOffset;
	m_dwRndUpdate1	= Data.dwRndUpdate1;
	m_dwVersion		= Data.dwVersion;

	//	Load height map
	if (!LoadHeightMap(ar, Data))
	{
		g_Log.Log("CTerrain::Load, Failed to load height map");
		return false;
	}

	CreateTerrainMesh();

	//	Load layers
	if (!LoadLayers(ar, Data))
	{
		g_Log.Log("CTerrain::Load, Failed to load layers");
		return false;
	}

	//	Create and load terrain render
	if (m_HeightMap.pHeightData && !ReloadRender(false))
	{
		g_Log.Log("CTerrain::Load, Failed to load terrain render");
	//	return false;
	}

	return true;
}

//	Load height map
bool CTerrain::LoadHeightMap(CELArchive& ar, const TERRAINFILEDATA& Data)
{
	//	Read height map file name
	CString strFile = ar.ReadString();

	//	Try to load height map
	if (Data.iHMWidth > 0 && Data.iHMHeight > 0 && strFile.GetLength())
	{
		//	Make full file name
		CString strFullFile;
		strFullFile.Format("%s%s\\%s", g_szWorkDir, m_pMap->GetResDir(), strFile);

		//	Load height map
		if (!LoadHeightMap(Data.iHMWidth, Data.iHMHeight, HM_FLOAT, strFullFile, !Data.bNormalData))
		{
			g_Log.Log("CTerrain::LoadHeightMapAndLayers, Failed to load height map %s.", strFile);
			return false;
		}

		if (Data.bNormalData)
		{
			if (!LoadVertexNormals())
			{
				g_Log.Log("CTerrain::LoadHeightMapAndLayers, Failed to load normal data");
				return false;
			}
		}
	}

	return true;
}

//	Load layers from terrain file
bool CTerrain::LoadLayers(CELArchive& ar, const TERRAINFILEDATA& Data)
{
	if (m_dwVersion >= 2)
	{
		m_pMaskMapFile = new CDataChunkFile;

		//	Make mask file name
		char szFile[MAX_PATH];
		sprintf(szFile, "%s%s\\MaskMaps.data", g_szWorkDir, m_pMap->GetResDir());

		if (!m_pMaskMapFile->OpenForRead(szFile))
		{
			g_Log.Log("CTerrain::LoadLayers: Failed to open mask map file %s", szFile);
			return false;
		}
	}

	//	Load layers
	for (int i=0; i < Data.iNumLayer; i++)
	{
		CTerrainLayer* pLayer = new CTerrainLayer(this);
		if (!pLayer->Load(ar, i))
		{
			g_Log.Log("CTerrain::LoadLayers, Failed to load layer %d.", i);
			return false;
		}

		m_aLayers.Add(pLayer);
	}

	if (m_dwVersion >= 2)
	{
		if (m_pMaskMapFile)
		{
			m_pMaskMapFile->Close();
			delete m_pMaskMapFile;
			m_pMaskMapFile = NULL;
		}
	}

	return true;
}

//	Save data
bool CTerrain::Save(CELArchive& ar)
{
	//	File terrain information
	TERRAINFILEDATA Data;
	memset(&Data, 0, sizeof (Data));

	Data.fTileSize		= m_fTileSize;
	Data.fMaxHeight		= m_fMaxHeight;
	Data.fYOffset		= m_fYOffset;
	Data.iNumLayer		= m_aLayers.GetSize();
	Data.dwRndUpdate1	= m_dwRndUpdate1;
	Data.bNormalData	= m_bSaveNormal;
	Data.dwVersion		= TERRAINFILE_VERSION;

	if (m_HeightMap.pHeightData)
	{
		Data.iHMWidth	= m_HeightMap.iWidth;
		Data.iHMHeight	= m_HeightMap.iHeight;
	}

	//	Write terrain information
	if (!ar.Write(&Data, sizeof (Data)))
	{
		g_Log.Log("CTerrain::Save, Failed to write terrain info.");
		return false;
	}

	//	Save height map data
	if (!SaveHeightMap(ar))
	{
		g_Log.Log("CTerrain::Save, Failed to save height map data.");
		return false;
	}

	//	Save layers
	if (!SaveLayers(ar))
	{
		g_Log.Log("CTerrain::Save, Failed to save layers.");
		return false;
	}

	if (m_bSaveNormal && !SaveVertexNormals())
		g_Log.Log("CTerrain::Save, Failed to save normal data.");

	return true;
}

//	Save height map data
bool CTerrain::SaveHeightMap(CELArchive& ar)
{
	if (!m_HeightMap.pHeightData)
	{
		ar.WriteString("");
		return true;
	}

	//	Save height map file name
	char szFile[MAX_PATH];
	sprintf(szFile, "%s.hmap", m_pMap->GetMapName());
	ar.WriteString(szFile);

	//	Create height map file
	char szFullName[MAX_PATH];
	sprintf(szFullName, "%s%s\\%s", g_szWorkDir, m_pMap->GetResDir(), szFile);

	FILE* fp = fopen(szFullName, "wb");
	if (!fp)
	{
		g_Log.Log("CTerrain::SaveHeightMap, Failed to create height map file.");
		return false;
	}

	//	Save height map data to file
	DWORD dwSize = m_HeightMap.iWidth * m_HeightMap.iHeight * sizeof (float);
	if (fwrite(m_HeightMap.pHeightData, 1, dwSize, fp) != dwSize)
	{
		g_Log.Log("CTerrain::SaveHeightMap, Failed to write height map data to file.");
		return false;
	}

	fclose(fp);

	return true;
}

//	Save terrain normal data
bool CTerrain::SaveVertexNormals()
{
	if (!m_HeightMap.aNormals)
		return false;

	//	Make normal file name
	char szFullName[MAX_PATH];
	sprintf(szFullName, "%s%s\\%s.normal", g_szWorkDir, m_pMap->GetResDir(), m_pMap->GetMapName());

	//	Create normal data file
	FILE* fp = fopen(szFullName, "wb");
	if (!fp)
	{
		g_Log.Log("CTerrain::SaveVertexNormals, Failed to create normal file.");
		return false;
	}

	//	Save terrain normal data to file
	DWORD dwSize = m_HeightMap.iWidth * m_HeightMap.iHeight * sizeof (A3DVECTOR3);
	if (fwrite(m_HeightMap.aNormals, 1, dwSize, fp) != dwSize)
	{
		g_Log.Log("CTerrain::SaveVertexNormals, Failed to write normal data to file.");
		return false;
	}

	fclose(fp);

	return true;
}

//	Save layers data
bool CTerrain::SaveLayers(CELArchive& ar)
{
	m_pMaskMapFile = new CDataChunkFile;

	//	Make mask file name
	char szFile[MAX_PATH];
	sprintf(szFile, "%s%s\\MaskMaps.data", g_szWorkDir, m_pMap->GetResDir());

	if (!m_pMaskMapFile->OpenForWrite(szFile, m_aLayers.GetSize(), true))
	{
		g_Log.Log("CTerrain::SaveLayers: Failed to open mask map file %s", szFile);
		return false;
	}

	for (int i=0; i < m_aLayers.GetSize(); i++)
	{
		CTerrainLayer* pLayer = m_aLayers[i];
		if (!pLayer->Save(ar, i))
		{
			g_Log.Log("CTerrain::SaveLayers, Failed to save layer [%d].", i);
			return false;
		}
	}

	if (m_pMaskMapFile)
	{
		m_pMaskMapFile->Close();
		delete m_pMaskMapFile;
		m_pMaskMapFile = NULL;
	}

	return true;
}

//	Load terrain normal data
bool CTerrain::LoadVertexNormals()
{
	//	Release old data
	if (m_HeightMap.aNormals)
	{
		a_free(m_HeightMap.aNormals);
		m_HeightMap.aNormals = NULL;
	}

	int iSize = m_HeightMap.iWidth * m_HeightMap.iHeight * sizeof (A3DVECTOR3);

	if (!(m_HeightMap.aNormals = (A3DVECTOR3*)a_malloc(iSize)))
	{
		g_Log.Log("CTerrain::LoadVertexNormals, not enough memory !");
		return false;
	}

	//	Make height map file name
	char szFullName[MAX_PATH];
	sprintf(szFullName, "%s%s\\%s.normal", g_szWorkDir, m_pMap->GetResDir(), m_pMap->GetMapName());

	FILE* fp = fopen(szFullName, "rb");
	if (!fp)
	{
		a_free(m_HeightMap.aNormals);
		m_HeightMap.aNormals = NULL;
		g_Log.Log("CTerrain::LoadVertexNormals, Failed to open normal data file %s !", szFullName);
		return false;
	}

	fread(m_HeightMap.aNormals, 1, iSize, fp);

	fclose(fp);

	return true;
}

void CTerrain::SetTileSize(float fSize)
{
	m_fTileSize = fSize;
	if (m_HeightMap.pHeightData)
		m_fTerrainSize = m_fTileSize * (m_HeightMap.iWidth - 1);

	//	Reset terrain area
	CalcTerrainArea();
}

//	Reload terrain render
bool CTerrain::ReloadRender(bool bNoRender)
{
	//	Create terrain render
	if (!CreateTerrainRender(bNoRender))
	{
		g_Log.Log("CTerrain::ReloadRender, Failed to create terrain render.");
		return false;
	}

	m_pTrnRender->OffsetTextureUV(m_pMap->m_fUoff, m_pMap->m_fVoff);

	//	Load data
	CString strFile;
	strFile.Format("%s%s\\%sRender.trn2", g_szWorkDir, m_pMap->GetResDir(), m_pMap->GetMapName());

	APointF ptCenter = m_rcTerrain.CenterPoint();
	if (!m_pTrnRender->Load(strFile, ptCenter.x, ptCenter.y))
	{
		g_Log.Log("CTerrain::ReloadRender, Failed to load data from file %s.", strFile);
		return false;
	}

	m_pTrnRender->SetLODDist(m_fTerrainSize * 0.5f * 0.2f, m_fTerrainSize * 0.5f * 0.4f);
	m_pTrnRender->SetLODType(A3DTerrain2::LOD_NOHEIGHT);

	//	Force to load all blocks
	m_pTrnRender->SetLoadCenter(A3DVECTOR3(ptCenter.x, 0.0f, ptCenter.y));

	//	Reset all blocks' LOD scale
	m_pTrnRender->ResetLODScales();

	return true;
}

//	Create terrain render
bool CTerrain::CreateTerrainRender(bool bNoRender)
{
	//	Release current render
	ReleaseRender();

	if (!(m_pTrnRender = new CTerrainRender(this)))
		return false;
	
	if (!m_pTrnRender->Init(g_Render.GetA3DEngine(), 200.0f, m_fTerrainSize * 1.2f, bNoRender))
	{
		g_Log.Log("CTerrain::CreateTerrainRender, Failed to initalize terrain render.");
		return false;
	}

	return true;
}

//	Release terrain render
void CTerrain::ReleaseRender()
{
	if (m_pTrnRender)
	{
		m_pTrnRender->Release();
		delete m_pTrnRender;
		m_pTrnRender = NULL;
	}
}

//	Update vertex positions in terrain render
bool CTerrain::UpdateVertexPos(const ARectF& rcArea)
{
	//	Update absolute height data
	if (m_HeightMap.pAbsHeight)
	{
		ARectI rcVert;
		rcVert.left		= (int)((rcArea.left - m_rcTerrain.left) / m_fTileSize);
		rcVert.top		= (int)(-(rcArea.top - m_rcTerrain.top) / m_fTileSize);
		rcVert.right	= (int)((rcArea.right - m_rcTerrain.left) / m_fTileSize) + 1;
		rcVert.bottom	= (int)(-(rcArea.bottom - m_rcTerrain.top) / m_fTileSize) + 1;

		a_ClampFloor(rcVert.left, 0);
		a_ClampFloor(rcVert.top, 0);
		a_ClampRoof(rcVert.right, m_HeightMap.iWidth);
		a_ClampRoof(rcVert.bottom, m_HeightMap.iHeight);

		for (int i=rcVert.top; i < rcVert.bottom; i++)
		{
			int i0 = i * m_HeightMap.iWidth + rcVert.left;
			for (int j=rcVert.left; j < rcVert.right; j++, i0++)
			{
				m_HeightMap.pAbsHeight[i0] = GetRealHeight(i0);
				m_aPos[i0].y = m_HeightMap.pAbsHeight[i0];
			}
		}
	}

	if (m_pTrnRender)
	{
		m_pTrnRender->UpdateTerrainInfo();
		m_pTrnRender->UpdateTerrainHeight(rcArea, m_HeightMap.pAbsHeight, m_HeightMap.aNormals, m_HeightMap.iWidth);
		m_pTrnRender->GetTerrainRender()->SetRebuildStreamFlag(true);
	}

	SetUpdateRenderFlag(true);

	return true;
}

//	Update vertex normals
bool CTerrain::UpdateVertexNormals(const ARectF& rcArea)
{
	ARectI rcVert;
	rcVert.left		= (int)((rcArea.left - m_rcTerrain.left) / m_fTileSize);
	rcVert.top		= (int)(-(rcArea.top - m_rcTerrain.top) / m_fTileSize);
	rcVert.right	= (int)((rcArea.right - m_rcTerrain.left) / m_fTileSize) + 1;
	rcVert.bottom	= (int)(-(rcArea.bottom - m_rcTerrain.top) / m_fTileSize) + 1;

	a_ClampFloor(rcVert.left, 0);
	a_ClampFloor(rcVert.top, 0);
	a_ClampRoof(rcVert.right, m_HeightMap.iWidth);
	a_ClampRoof(rcVert.bottom, m_HeightMap.iHeight);

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

	if (m_pTrnRender)
		m_pTrnRender->UpdateTerrainHeight(rcArea, m_HeightMap.pAbsHeight, m_HeightMap.aNormals, m_HeightMap.iWidth);

	SetUpdateRenderFlag(true);

	return true;
}

//	Set map offset
void CTerrain::SetMapOffset(float x, float z)
{
	m_rcTerrain.Offset(x, z);
	ReloadRender(false);
}

//	Create terrain mesh
bool CTerrain::CreateTerrainMesh()
{
	if (!(m_aPos = (A3DVECTOR3*)a_malloc(sizeof (A3DVECTOR3) * m_HeightMap.iWidth * m_HeightMap.iHeight)))
	{
		g_Log.Log("CExpTerrain::CreateTerrainMesh, Not enough memory !");
		return false;
	}

	int i, j, iCount = 0;

	for (i=0; i < m_HeightMap.iHeight; i++)
	{
		float z = m_rcTerrain.top - i * m_fTileSize;
		float x = m_rcTerrain.left;

		for (j=0; j < m_HeightMap.iWidth; j++, iCount++)
		{
			A3DVECTOR3* pPos = &m_aPos[iCount];
			float y = GetRealHeight(iCount);

			pPos->Set(x, y, z);

			x += m_fTileSize;
		}
	}

	return true;
}

A3DVECTOR3* CTerrain::GetVertexPos() 
{
	if(m_aPos == NULL)
		CreateTerrainMesh();
	
	return m_aPos; 
}

