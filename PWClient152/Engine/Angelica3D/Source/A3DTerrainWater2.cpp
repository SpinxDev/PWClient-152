/*
* FILE: A3DTerrainWater2.cpp
*
* DESCRIPTION: Class representing for water on the terrain
*
* CREATED BY: liyi 2012 , 3, 5
*
* HISTORY:
*
* Copyright (c) 2004 Archosaur Studio, All Rights Reserved.	
*/



#include "A3DTypes.h"
#include "A3DCamera.h"
#include "A3DCubeTexture.h"
#include "A3DDevice.h"
#include "A3DEngine.h"
#include "A3DStream.h"
#include "A3DLight.h"
#include "A3DTextureMan.h"
#include "A3DTexture.h"
#include "A3DRenderTarget.h"
#include "A3DViewport.h"
#include "A3DPixelShader.h"
#include "A3DVertexShader.h"
#include "A3DShaderMan.h"
#include "A3DSky.h"
#include "A3DTerrainOutline.h"
#include "A3DTerrain2.h"
#include "A3DVertex.h"
#include "A3DFuncs.h"
#include "A3DPI.h"
#include "A3DEnvironment.h"

//FIXME!! water2
#include "A3DTerrainWater2.h"
#include "A3DWaterArea2.h"
#include "A3DTerrainWater2SimpleRender.h"
#include "A3DTerrainWater2NormalRender.h"




#define new A_DEBUG_NEW

#define MAX_WAVE_DIS	300.0f

///////////////////////////////////////////////////////////////////////////
//	
//	Implement A3DTerrainWater
//	
///////////////////////////////////////////////////////////////////////////

A3DTerrainWater2::A3DTerrainWater2() : 
m_pA3DDevice		(NULL),
m_pWaterRender		(NULL),
m_pDefWaterRender	(NULL),
//m_pWaterDimple		(NULL),
m_pWaterAreaVertDecl(NULL),
m_pWaterWaveVertDecl(NULL),
m_pWaterStream		(NULL),
m_nMaxFaces			(0),
m_nMaxVerts			(0),
m_pCurWaterArea		(NULL),
m_clrWater			(0xA0254F55),
m_nWaterRenderLevel	(-1),
m_bStaticMesh		(false),
m_fViewRadius		(5000.0f),
m_pWaveTex			(NULL),
m_pWaveSmokeTex		(NULL),
m_fWaveAttenStart	(45.0),
m_fWaveAttenEnd		(60.0),
m_pWaterVertRef     (NULL),
m_pWaterIdxRef      (NULL)
{
	m_fEdgeHeight		= 1.0f;

	m_fDNFactor			= 0.0f;
}

A3DTerrainWater2::~A3DTerrainWater2()
{
	Release();
}

bool A3DTerrainWater2::Init(A3DDevice * pA3DDevice, int nMaxVerts, int nMaxFaces, 
					int nWaterRenderLevel/* WRL_NORMAL */)
{
	m_pA3DDevice = pA3DDevice;
	m_nMaxVerts = nMaxVerts;
	m_nMaxFaces = nMaxFaces;
	m_bStaticMesh = false;

	//	Create water stream
	if (!CreateWaterStream())
		return false;

	//	Create water render
	if (WRL_UNKNOWN != nWaterRenderLevel)
	{
		if (!SetWaterRenderLevel(nWaterRenderLevel))
			return false;
	}

	// 初始化waterwave的一些资源,失败的话只是导致waterwave不会渲染, 不影响全局效果,所以不用返回false
	InitWaterWaveResource();
	return true;
}

//	Initialize water object with static stream (high performance but cann't be edited)
bool A3DTerrainWater2::InitStatically(A3DDevice* pA3DDevice, int nWaterRenderLevel/* WRL_LPP_MIDDLE */)
{
	m_pA3DDevice = pA3DDevice;
	m_bStaticMesh = true;

	int iMeshGrid = A3DWaterMeshBlock2::GetHighestMeshGridDim();
	m_nMaxVerts = (iMeshGrid + 1) * (iMeshGrid + 1);
	m_nMaxFaces = 0;

	//	Create water stream
	if (!CreateWaterStream())
		return false;

	//	Create water render
	if (WRL_UNKNOWN != nWaterRenderLevel)
	{
		if (!SetWaterRenderLevel(nWaterRenderLevel))
			return false;
	}

	// 初始化waterwave的一些资源,失败的话只是导致waterwave不会渲染, 不影响全局效果,所以不用返回false
	InitWaterWaveResource();
	return true;
}

bool A3DTerrainWater2::InitWaterWaveResource()
{
	//const char* pszWaveTexFile = "Materials\\Textures\\Water\\wave1.dds";
	//const char* pszWaveSmokeFile = "Materials\\Textures\\Smoke1.png";

	//m_pMtlWaterWave = new A3DMtlWaterWave;
	//if(m_pMtlWaterWave == NULL || m_pMtlWaterWave->Init(m_pA3DDevice->GetA3DEngine()) == false)
	//{
	//	g_A3DErrLog.Log("A3DTerrainWaterLPPRender, failed to load water wave material.");
	//	return false;
	//}

	//if(!m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFile(pszWaveTexFile, &m_pWaveTex))
	//{
	//	g_A3DErrLog.Log("A3DTerrainWaterExpansiveRender::LoadTextures(), fail to load texture [%s]", pszWaveTexFile);
	//}
	//if(!m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->LoadTextureFromFile(pszWaveSmokeFile, &m_pWaveSmokeTex))
	//{
	//	g_A3DErrLog.Log("A3DTerrainWaterExpansiveRender::LoadTextures(), fail to load texture [%s]", pszWaveSmokeFile);
	//}
	return true;
}

bool A3DTerrainWater2::Release()
{
	//DisableWaterDimple();

	ReleaseAllWaterAreas();
	ReleaseWaterStream();

	if(m_pWaveTex)
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pWaveTex);
		m_pWaveTex = NULL;
	}
	if( m_pWaveSmokeTex)
	{
		m_pA3DDevice->GetA3DEngine()->GetA3DTextureMan()->ReleaseTexture(m_pWaveSmokeTex);
		m_pWaveSmokeTex = NULL;
	}
	//A3DRELEASE(m_pMtlWaterWave); //FIXME!!

	A3DRELEASE(m_pDefWaterRender);
	m_pWaterRender = NULL;

	return true;
}

bool A3DTerrainWater2::CreateWaterStream()
{
	//	Create stream
	DWORD dwStrmFlag = m_bStaticMesh ? A3DSTRM_STATIC : A3DSTRM_REFERENCEPTR;
	m_pWaterStream = new A3DStream;
	if (!m_pWaterStream || !m_pWaterStream->Init(m_pA3DDevice, sizeof (A3DWATERVERTEX2), A3DWATERVERT2_FVF,
					m_nMaxVerts, m_nMaxFaces * 3, dwStrmFlag,  dwStrmFlag))
	{
		g_A3DErrLog.Log("A3DTerrainWater::CreateWaterStream(), Failed to create water stream!");
		return false;
	}
    
    m_pWaterVertRef = new A3DWATERVERTEX2[m_nMaxVerts];
    m_pWaterIdxRef = new WORD[m_nMaxFaces * 3];

	//	Initialize static stream's content
	if (m_bStaticMesh)
	{
		A3DWATERVERTEX2*	pVerts = NULL;
		if (!m_pWaterStream->LockVertexBuffer(0, 0, (BYTE**)&pVerts, 0))
			return false;

		//	Ensure unit water grid's border is 1 meter
		int iMeshGrid = A3DWaterMeshBlock2::GetHighestMeshGridDim();
		float fMeshGridSize = 1.0f / (iMeshGrid / A3DWaterMeshBlock2::BLOCK_GRID_DIM);

		int r, c;
		A3DVECTOR3 v(0.0f);
		A3DWATERVERTEX2* pv = pVerts;

		for (r=0; r <= iMeshGrid; r++)
		{
			v.z = 0.0f - fMeshGridSize * r;
			v.x = 0.0f;

			for (c=0; c <= iMeshGrid; c++)
			{
				*pv++ = A3DWATERVERTEX2(v, A3D::g_vAxisY, v.x, -v.z);
				v.x += fMeshGridSize;
			}
		}

		m_pWaterStream->UnlockVertexBuffer();

		ASSERT(pv - pVerts == m_pWaterStream->GetVertCount());
	}

	//	Create water area vert decl
	const static D3DVERTEXELEMENT9 elements[] =
	{
		{0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
		{0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		D3DDECL_END()
	};

	m_pWaterAreaVertDecl = new A3DVertexDecl;
	if (!m_pWaterAreaVertDecl->Init(m_pA3DDevice, elements))
	{
		g_A3DErrLog.Log("A3DTerrainWater::CreateWaterStream(), Failed to create water area vertex decl!");
		return false;
	}

	const static D3DVERTEXELEMENT9 elementsWave[] =
	{
		{0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		{0, 24, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},
		{0, 28, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 1},
		D3DDECL_END()
	};

	m_pWaterWaveVertDecl = new A3DVertexDecl;
	if (!m_pWaterWaveVertDecl->Init(m_pA3DDevice, elementsWave))
	{
		g_A3DErrLog.Log("A3DTerrainWater::CreateWaterStream(), Failed to create water wave vertex decl!");
		return false;
	}

	return true;
}

void A3DTerrainWater2::ReleaseWaterStream()
{
	A3DRELEASE(m_pWaterStream);
    if (m_pWaterVertRef)
    {
        delete m_pWaterVertRef;
        m_pWaterVertRef = NULL;
    }

    if (m_pWaterIdxRef)
    {
        delete m_pWaterIdxRef;
        m_pWaterIdxRef = NULL;
    }
	A3DRELEASE(m_pWaterAreaVertDecl);
	A3DRELEASE(m_pWaterWaveVertDecl);
}

bool A3DTerrainWater2::SetWaterRenderLevel( int nWaterRenderLevel)
{
	if (m_nWaterRenderLevel == nWaterRenderLevel)
		return true;

	A3DTerrainWater2Render* pRender = NULL;

	//FIXME!! water2
	if (nWaterRenderLevel == WRL_NORMAL)
	{	
		A3DTerrainWater2NormalRender* pWaterRender = new A3DTerrainWater2NormalRender;
		if (!pWaterRender || !pWaterRender->Init(this, true, true,false))
			return false;

		pRender = pWaterRender;
	}
	else if(nWaterRenderLevel == WRL_NORMAL_CUBE_REFLECT)
	{
		A3DTerrainWater2NormalRender* pWaterRender = new A3DTerrainWater2NormalRender;
		if (!pWaterRender || !pWaterRender->Init(this, true, true, true))
			return false;

		pRender = pWaterRender;
	}
	else if (nWaterRenderLevel == WRL_SIMPLE)
	{
		A3DTerrainWater2SimpleRender* pWaterRender = new A3DTerrainWater2SimpleRender;
		if (!pWaterRender || !pWaterRender->Init(this))
			return false;

		pRender = pWaterRender;
	}
	else
		return false;

	if (m_pDefWaterRender == m_pWaterRender)
		m_pWaterRender = pRender;

	A3DRELEASE(m_pDefWaterRender);
	m_pDefWaterRender = pRender;

	m_nWaterRenderLevel = nWaterRenderLevel;

	return true;
}

A3DWaterArea2 * A3DTerrainWater2::GetNearestWaterArea(const A3DVECTOR3& vecPos, bool bIncMinorWater)
{
	int	nMinDis = 999999, nDis;
	A3DWaterArea2 * pMinArea = NULL;

	int nNumArray = m_WaterAreas.GetSize();
	for( int nArray = 0; nArray < nNumArray; nArray++)
	{
		HeightWaterAreaArray* pAreaArray = m_WaterAreas[nArray];
		int	i;
		for(i=0; i<pAreaArray->waterAreaArray.GetSize(); i++)
		{
			A3DWaterArea2 * pArea = pAreaArray->waterAreaArray[i];
			if (pArea->GetMinorWaterFlag() && !bIncMinorWater)
				continue;

			int nX = (int)((vecPos.x - pArea->GetCenter().x) / pArea->GetGridSize() + pArea->GetWidth() / 2.0f);
			int nY = (int)((pArea->GetCenter().z - vecPos.z) / pArea->GetGridSize() + pArea->GetHeight() / 2.0f);

			for(int y=0; y<pArea->GetHeight(); y++)
			{
				for(int x=0; x<pArea->GetWidth(); x++)
				{
					if( pArea->GetGridFlags()[y * pArea->GetWidth() + x] )
					{
						nDis = abs(nX - x) + abs(nY - y);
						if( nDis < nMinDis )
						{
							nMinDis = nDis;
							pMinArea = pArea;
						}
					}
				}
			}
		}	
	}

	return pMinArea;
}

bool A3DTerrainWater2::LoadWaterAreas(const char * szFileName)
{
	DWORD dwReadLen;

	AFile fileToLoad;
	if( !fileToLoad.Open(szFileName, AFILE_OPENEXIST) )
	{
		g_A3DErrLog.Log("A3DTerrainWater::LoadWaterAreas(), failed to open file [%s]", szFileName);
		return false;
	}

	int nNumArea;
	// first read the number of water areas
	if( !fileToLoad.Read(&nNumArea, sizeof(int), &dwReadLen) )
	{
		g_A3DErrLog.Log("A3DTerrainWater::LoadWaterAreas(), read from file failed");
		return false;
	}

	// then save all water areas one by one
	for(int i=0; i<nNumArea; i++)
	{
		A3DWaterArea2 * pArea = new A3DWaterArea2;

		if( !pArea->Load(this, &fileToLoad) )
		{
			g_A3DErrLog.Log("A3DTerrainWater::LoadWaterAreas(), failed to load one water area");
			return false;
		}

		AddWaterAreaForRender(pArea);
	}

	fileToLoad.Close();
	return true;
}

bool A3DTerrainWater2::SaveWaterAreas(const char * szFileName)
{
	DWORD dwWriteLen;

	AFile fileToSave;
	if( !fileToSave.Open(szFileName, AFILE_CREATENEW | AFILE_BINARY) )
	{
		g_A3DErrLog.Log("A3DTerrainWater::SaveWaterAreas(), failed to create file [%s]", szFileName);
		return false;
	}

	int nNumArea = GetNumWaterAreas();
	// first write the number of water areas
	if( !fileToSave.Write(&nNumArea, sizeof(int), &dwWriteLen) )
	{
		g_A3DErrLog.Log("A3DTerrainWater::SaveWaterAreas(), write to file failed");
		return false;
	}

	// then save all water areas one by one
	for( int nArray = 0; nArray < m_WaterAreas.GetSize(); nArray++)
	{
		HeightWaterAreaArray* pAreaArray = m_WaterAreas[nArray];

		for(int i=0; i<pAreaArray->waterAreaArray.GetSize(); i++)
		{
			A3DWaterArea2 * pArea = pAreaArray->waterAreaArray[i];

			if( !pArea->Save(&fileToSave) )
			{
				g_A3DErrLog.Log("A3DTerrainWater::SaveWaterAreas(), failed to save one water area");
				return false;
			}
		}
	}
	fileToSave.Close();
	return true;
}

int A3DTerrainWater2::GetNumWaterAreas()
{
	int nNumArea = 0; 
	for( int nArray = 0; nArray < m_WaterAreas.GetSize(); nArray++)
	{
		nNumArea += m_WaterAreas[nArray]->waterAreaArray.GetSize();
	}
	return nNumArea;
}

bool A3DTerrainWater2::AddWaterArea(DWORD dwID, int nWidth, int nHeight, float vGridSize, const A3DVECTOR3& vecCenter, float vWaterHeight)
{
	A3DWaterArea2 * pArea = new A3DWaterArea2();

	if( !pArea->Init(this, dwID, nWidth, nHeight, vGridSize, vecCenter, vWaterHeight) )
	{
		g_A3DErrLog.Log("A3DTerrainWater::AddWaterArea(), fail to init new water area!");
		return false;
	}

	//Add water area
	AddWaterAreaForRender(pArea);
	return true;
}

bool A3DTerrainWater2::AddWaterAreaForRender(A3DWaterArea2 * pWaterArea)
{
	//Add water area
	bool bFind = false;
	for( int i = 0; i < m_WaterAreas.GetSize(); i++)
	{
		if( fabs(m_WaterAreas[i]->fWaterHeight - pWaterArea->GetWaterHeight()) < 0.001f )
		{
			m_WaterAreas[i]->waterAreaArray.Add(pWaterArea);
			bFind = true;
			break;
		}
	}
	if( !bFind)
	{
		HeightWaterAreaArray* pWaterAreaArray = new HeightWaterAreaArray;
		pWaterAreaArray->fWaterHeight = pWaterArea->GetWaterHeight();
		pWaterAreaArray->waterAreaArray.Add(pWaterArea);

		m_WaterAreas.Add(pWaterAreaArray);
	}

	return true;
}

bool A3DTerrainWater2::DeleteWaterAreaByID(DWORD dwID)
{
	int nNumAreaArray = m_WaterAreas.GetSize();

	for( int nArray = 0; nArray < nNumAreaArray; nArray++)
	{
		int nNumArea = m_WaterAreas[nArray]->waterAreaArray.GetSize();
		for(int i=0; i<nNumArea; i++)
		{
			A3DWaterArea2 * pArea = m_WaterAreas[nArray]->waterAreaArray[i];

			if( pArea->GetID() == dwID )
			{
				pArea->Release();
				delete pArea;

				m_WaterAreas[nArray]->waterAreaArray.RemoveAt(i);
				if( m_WaterAreas[nArray]->waterAreaArray.GetSize() == 0)
				{
					delete m_WaterAreas[nArray];
					m_WaterAreas.RemoveAt(nArray);
				}

				return true;
			}
		}
	}
	return false;
}

bool A3DTerrainWater2::RemoveWaterAreaByID(DWORD dwID)
{
	int nNumAreaArray = m_WaterAreas.GetSize();

	for( int nArray = 0; nArray < nNumAreaArray; nArray++)
	{
		int nNumArea = m_WaterAreas[nArray]->waterAreaArray.GetSize();
		for(int i=0; i<nNumArea; i++)
		{
			A3DWaterArea2 * pArea = m_WaterAreas[nArray]->waterAreaArray[i];

			if( pArea->GetID() == dwID )
			{				
				m_WaterAreas[nArray]->waterAreaArray.RemoveAt(i);
				if( m_WaterAreas[nArray]->waterAreaArray.GetSize() == 0)
				{
					delete m_WaterAreas[nArray];
					m_WaterAreas.RemoveAt(nArray);
				}

				return true;
			}
		}
	}
	return false;
}

A3DWaterArea2 * A3DTerrainWater2::GetWaterAreaByID(DWORD dwID)
{
	int nNumAreaArray = m_WaterAreas.GetSize();

	for( int nArray = 0; nArray < nNumAreaArray; nArray++)
	{
		int nNumArea = m_WaterAreas[nArray]->waterAreaArray.GetSize();
		for(int i=0; i<nNumArea; i++)
		{
			A3DWaterArea2 * pArea = m_WaterAreas[nArray]->waterAreaArray[i];

			if( pArea->GetID() == dwID )
			{

				return pArea;
			}
		}
	}

	return NULL;
}

bool A3DTerrainWater2::ReleaseAllWaterAreas()
{
	int nNumAreaArray = m_WaterAreas.GetSize();

	for( int nArray = 0; nArray < nNumAreaArray; nArray++)
	{
		int nNumArea = m_WaterAreas[nArray]->waterAreaArray.GetSize();
		for(int i=0; i<nNumArea; i++)
		{
			A3DWaterArea2 * pArea = m_WaterAreas[nArray]->waterAreaArray[i];
			pArea->Release();
			delete pArea;
		}
		m_WaterAreas[nArray]->waterAreaArray.RemoveAll();
		delete m_WaterAreas[nArray];
	}
	m_WaterAreas.RemoveAll();

	return true;
}

bool A3DTerrainWater2::IsUnderWater(const A3DVECTOR3& vecPos, bool bOnlyVisGrid/* false */)
{
	for( int nArray = 0; nArray < m_WaterAreas.GetSize(); nArray++)
	{
		HeightWaterAreaArray* pAreaArray = m_WaterAreas[nArray];

		for(int i=0; i< pAreaArray->waterAreaArray.GetSize(); i++)
		{
			A3DWaterArea2* pArea = pAreaArray->waterAreaArray[i];

			A3DVECTOR3 vecDelta = vecPos - pArea->GetCenter();
			if( fabs(vecDelta.x) < pArea->GetWidth() * 0.5f * pArea->GetGridSize() &&
				fabs(vecDelta.z) < pArea->GetHeight() * 0.5f * pArea->GetGridSize() )
			{
				if (vecDelta.y < 0.0f)
				{
					if (bOnlyVisGrid)
						return pArea->IsInVisbleGrid(vecPos);
					else
						return true;
				}
				else
					return false;
			}
		}
	}

	return false;
}


float A3DTerrainWater2::GetWaterHeight(const A3DVECTOR3& vecPos, bool bOnlyVisGrid/* false */, A3DWaterArea2** pOutWaterArea/* NULL */)
{
	if (pOutWaterArea)
		*pOutWaterArea = NULL;

	for (int nArray = 0; nArray < m_WaterAreas.GetSize(); nArray++)
	{
		HeightWaterAreaArray* pAreaArray = m_WaterAreas[nArray];

		for (int i=0; i< pAreaArray->waterAreaArray.GetSize(); i++)
		{
			A3DWaterArea2* pArea = pAreaArray->waterAreaArray[i];

			A3DVECTOR3 vecDelta = vecPos - pArea->GetCenter();
			if (fabs(vecDelta.x) < pArea->GetWidth() * 0.5f * pArea->GetGridSize() &&
				fabs(vecDelta.z) < pArea->GetHeight() * 0.5f * pArea->GetGridSize())
			{
				if (bOnlyVisGrid)
				{
					if (pArea->IsInVisbleGrid(vecPos))
					{
						if (pOutWaterArea)
							*pOutWaterArea = pArea;

						return pArea->GetCenter().y;
					}
					else
						return 0.0f;
				}
				else
				{
					if (pOutWaterArea)
						*pOutWaterArea = pArea;

					return pArea->GetCenter().y;
				}
			}
		}
	}

	return 0.0f;
}

bool A3DTerrainWater2::Update(int nDeltaTime)
{
	//FIXME!!
	//if (m_pWaterDimple)
	//	m_pWaterDimple->Tick(nDeltaTime);

	return m_pWaterRender->Update(nDeltaTime);
}

//	Determine water area, this function should be called every frame before render.
void A3DTerrainWater2::DetermineWaterArea(const A3DVECTOR3& vEyePos)
{
	m_pCurWaterArea = NULL;
	float fWaterHeight = GetWaterHeight(vEyePos, false, &m_pCurWaterArea);
	if (m_pCurWaterArea)
	{
		m_clrWater = m_pCurWaterArea->GetCurWaterColor();
	}
}

bool A3DTerrainWater2::Render(const A3DTerrainWater2Render::RenderParam* pRenderParam)
{
	//FIXME!! water2
	//A3DPerformance* pA3DPerf = m_pA3DDevice->GetA3DEngine()->GetA3DPerformance();
	//A3DPerformanceWrapper apw(pA3DPerf, A3DPerformance::PERF_RENDER_WATER);
	//A3DPerformanceWrapper apw_lpp(pA3DPerf, A3DPerformance::PERF_RENDER_LPP_WATER);

	const bool bRet = m_pWaterRender->Render(pRenderParam);
	if(IsUnderWater(pRenderParam->pCurViewport->GetCamera()->GetPos()) == false)
	{
		const int nNumAreaArray = GetWaterAreasArrayNum();
		for(int i = 0; i < nNumAreaArray; i++)
		{
			RenderWave(pRenderParam, i);
		}
	}
	//pA3DPerf->SetPass(A3DPerformance::PDC_LPP_WATER);
	return bRet;
}

bool A3DTerrainWater2::CullWaterAreasByViewport(A3DViewport* pViewport, int nArrayIdx)
{
	int iNumMinorArea=0, iNumNormalArea=0;

	APtrArray<A3DWaterArea2*>* pAreaArry = GetWaterAreasArray(nArrayIdx);
	for (int i=0; i<pAreaArry->GetSize(); i++)
	{
		A3DWaterArea2* pArea = (*pAreaArry)[i];

		//	RenderReflect is called when camera is above water, so ignore water area that
		//	higher than camera
		A3DCameraBase* pCamera = pViewport->GetCamera();
		//if (pCamera->GetPos().y < pArea->GetWaterHeight())
		//	continue;

		//设置是否渲染标志
		pArea->m_bDoRender = pCamera->AABBInViewFrustum(pArea->m_aabbVisGrids);
        if (pArea->IsOccluded(pViewport))
            pArea->m_bDoRender = false;

		if (!pArea->m_bDoRender) 
			continue;


		if (pArea->GetMinorWaterFlag())
			iNumMinorArea++;
		else
			iNumNormalArea++;
	}	

	if (iNumMinorArea <= 0 && iNumNormalArea <= 0)
		return false;

	return true;
}

bool A3DTerrainWater2::DrawWaterAreaArray(A3DViewport* pViewport, DWORD dwDrawFlags, int nArrayIdx)
{
	//if (!CullWaterAreasByViewport(pViewport, nArrayIdx))
	//	return true;

	//A3DPerformance* const pA3DPerf = m_pA3DDevice->GetA3DEngine()->GetA3DPerformance();
	A3DWATERVERTEX2*	pVerts = m_pWaterVertRef;
	WORD* pIndices = m_pWaterIdxRef;

	m_pWaterStream->Appear(0, false);

	A3DVECTOR3 vecCamPos = pViewport->GetCamera()->GetPos();

	A3DWaterArea2 * pArea;
	int nVertCount = 0;
	int nFaceCount = 0;
	APtrArray<A3DWaterArea2*>* pWaterAreas = GetWaterAreasArray(nArrayIdx);
	for(int i=0; i< pWaterAreas->GetSize(); i++)
	{
		pArea = (*pWaterAreas)[i];

		if (!pArea->m_bDoRender)
			continue;

		if (dwDrawFlags != DRAW_ALLWATER)
		{
			if (((dwDrawFlags & DRAW_MINORWATER) && !pArea->GetMinorWaterFlag()) ||
				((dwDrawFlags & DRAW_NON_MINORWATER) && pArea->GetMinorWaterFlag()))
				continue;
		}

		//	Give water render a chance to upload parameters for this water area
		m_pWaterRender->UploadParamsForWaterArea(pArea); //FIXME!!

		// now fill one water area's verts and faces into stream
		int nWidth = pArea->GetWidth();
		int nHeight = pArea->GetHeight();
		float vGridSize = pArea->GetGridSize();
		BYTE* pGridFlags = pArea->GetGridFlags();

		A3DVECTOR3 vecDeltaX = A3DVECTOR3(pArea->GetGridSize(), 0.0f, 0.0f);
		A3DVECTOR3 vecDeltaZ = A3DVECTOR3(0.0f, 0.0f, -pArea->GetGridSize());
		A3DVECTOR3 vecOrg = pArea->GetCenter() - vecDeltaX * nWidth / 2.0f - vecDeltaZ * nHeight / 2.0f;
		vecOrg.y = pArea->GetWaterHeight();

		float	ts = (float)(int)(vGridSize * 0.025f + 0.5f);
		for(int y=0; y<nHeight; y++)
		{
			const int iBase = y * nWidth;

			for(int x=0; x<nWidth; x++)
			{
				if( pGridFlags[iBase + x] & WATERGRID_VISIBLE )
				{
					A3DVECTOR3 p0 = vecOrg + vecDeltaX * x + vecDeltaZ * y;
					float u0 = x * ts;
					float v0 = y * ts;

					int ss;
					A3DVECTOR3 c = p0 + vecDeltaX * 0.5f + vecDeltaZ * 0.5f;

					float dx = float(fabs(c.x - vecCamPos.x));
					float dz = float(fabs(c.z - vecCamPos.z));
					if( dx < 160.0f && dz < 160.0f )
					{
						if( dx < 64.0f && dz < 64.0f )
							ss = int(vGridSize / 2.0f);
						else if( dx < 120.0f && dz < 120.0f )
							ss = int(vGridSize / 4.0f);
						else
							ss = int(vGridSize / 8.0f);

						if( ss == 0 )
							ss = 1;
					}
					else
						ss = 1;

					if( nVertCount + (ss + 1) * (ss + 1) > m_nMaxVerts ||
						nFaceCount + ss * ss * 2 > m_nMaxFaces )
					{
                        // flush once;
                        if( !m_pWaterStream->LockVertexBufferDynamic(0, 0, (BYTE**)&pVerts, 0) )
                            return false;
                        memcpy(pVerts, m_pWaterVertRef, sizeof(A3DWATERVERTEX2) * m_nMaxVerts);
						m_pWaterStream->UnlockVertexBufferDynamic();

                        if( !m_pWaterStream->LockIndexBufferDynamic(0, 0, (BYTE**)&pIndices, 0) )
                            return false;
                        memcpy(pIndices, m_pWaterIdxRef, sizeof(WORD) * m_nMaxFaces * 3);
                        m_pWaterStream->UnlockIndexBufferDynamic();

						m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, 0, nVertCount, 0, nFaceCount);
						//pA3DPerf->IncDelayedPassDrawCount(nVertCount, nFaceCount);

						nVertCount = 0;
						nFaceCount = 0;

                        pVerts = m_pWaterVertRef;
                        pIndices = m_pWaterIdxRef;
					}

					float rss = 1.0f / ss;
					for(int yy=0; yy<=ss; yy++)
					{
						for(int xx=0; xx<=ss; xx++)
						{
							*pVerts = A3DWATERVERTEX2(p0 + xx * rss * vecDeltaX + yy * rss * vecDeltaZ, 
								A3D::g_vAxisY, u0 + xx * rss, v0 + yy * rss);

							pVerts ++;

							if( xx != ss && yy != ss )
							{
								pIndices[0] = nVertCount + yy * (ss + 1) + xx;
								pIndices[1] = pIndices[0] + 1;
								pIndices[2] = pIndices[0] + ss + 1;

								pIndices[3] = pIndices[2];
								pIndices[4] = pIndices[1];
								pIndices[5] = pIndices[2] + 1;

								pIndices += 6;
							}
						}
					}

					nVertCount += (ss + 1) * (ss + 1);
					nFaceCount += ss * ss * 2;
				}
			}
		}
	}

	if (nVertCount > 0 && nFaceCount > 0)
	{
        if( !m_pWaterStream->LockVertexBufferDynamic(0, 0, (BYTE**)&pVerts, 0) )
            return false;
        memcpy(pVerts, m_pWaterVertRef, sizeof(A3DWATERVERTEX2) * m_nMaxVerts);
        m_pWaterStream->UnlockVertexBufferDynamic();

        if( !m_pWaterStream->LockIndexBufferDynamic(0, 0, (BYTE**)&pIndices, 0) )
            return false;
        memcpy(pIndices, m_pWaterIdxRef, sizeof(WORD) * m_nMaxFaces * 3);
        m_pWaterStream->UnlockIndexBufferDynamic();

        m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0,0, nVertCount, 0, nFaceCount);
		//pA3DPerf->IncDelayedPassDrawCount(nVertCount, nFaceCount);
	}

	return true;
}

bool A3DTerrainWater2::DrawWaterAreaArrayStatic(A3DViewport* pViewport, DWORD dwDrawFlags, int nArrayIdx,
									A3DHLSL* pHLSL, bool bPPL)
{
	if (!CullWaterAreasByViewport(pViewport, nArrayIdx))
		return true;

	m_pWaterStream->Appear(0, false);

	const A3DVECTOR3& vecCamPos = pViewport->GetCamera()->GetPos();

	A3DWaterArea2* pArea;
	APtrArray<A3DWaterArea2*>* pWaterAreas = GetWaterAreasArray(nArrayIdx);
	for (int i=0; i < pWaterAreas->GetSize(); i++)
	{
		pArea = (*pWaterAreas)[i];

		if (!pArea->m_bDoRender)
			continue;

		if (dwDrawFlags != DRAW_ALLWATER)
		{
			if (((dwDrawFlags & DRAW_MINORWATER) && !pArea->GetMinorWaterFlag()) ||
				((dwDrawFlags & DRAW_NON_MINORWATER) && pArea->GetMinorWaterFlag()))
				continue;
		}

		//	Give water render a chance to upload parameters for this water area
		m_pWaterRender->UploadParamsForWaterArea(pArea); //FIXME!!

		pArea->DrawStaticMeshes(pViewport,pHLSL, m_fViewRadius, bPPL);
	}

	return true;
}

A3DCOLORVALUE A3DTerrainWater2::ComposeWaterColor(const A3DTerrainWater2Render::RenderParam* pParam)
{
	
	A3DCOLORVALUE clrWater = a3d_ColorRGBAToColorValue(0xA0000000 | (m_clrWater & 0xffffff));
/*
	A3DEnvironment* pEnv = m_pA3DDevice->GetA3DEngine()->GetA3DEnvironment();
	const A3DVECTOR3& vEyePos = pParam->pCurViewport->GetCamera()->GetPos();
	A3DVECTOR3 vMLPos = pEnv->CalcMainLightPos(vEyePos);
	A3DVECTOR3 vMLDir = a3d_Normalize(vMLPos - vEyePos);

	float fDiffuse = vMLDir.y;

	A3DCOLORVALUE clLight = pEnv->GetAmbient1() * (1.0f - fDiffuse) + fDiffuse * pEnv->GetMainLightColor();
	clLight.a = 1.0f;
	clrWater = clLight * clrWater;
*/
	return clrWater;
}

//	Set current water render, NULL means to restore default render
void A3DTerrainWater2::SetWaterRender(A3DTerrainWater2Render* pWaterRender)
{
	if (pWaterRender)
		m_pWaterRender = pWaterRender;
	else
		m_pWaterRender = m_pDefWaterRender;
}

bool A3DTerrainWater2::RenderWave(const A3DTerrainWater2Render::RenderParam* prp, int nWaterAreaArrayIdx)
{
	//FIXME!! water2
	/*
	A3DWaterArea * pArea;
	APtrArray<A3DWaterArea*>* pAreaArry = GetWaterAreasArray(nWaterAreaArrayIdx);
	int nVert = 0;
	int nFaces = 0;

	if(m_pWaveTex == NULL || m_pWaveSmokeTex == NULL || m_pMtlWaterWave == NULL)
		return false;

	A3DEnvironment* pA3DEnv = m_pA3DDevice->GetA3DEngine()->GetA3DEnvironment();
	A3DVECTOR4 vAmbient = (const A3DVECTOR4&)pA3DEnv->GetAmbient1();
	A3DVECTOR4 vMainLight = (const A3DVECTOR4&)pA3DEnv->GetMainLightColor();
	m_pMtlWaterWave->paramWaveTex->SetExtDXTexture(m_pWaveTex->GetD3DTexture());
	m_pMtlWaterWave->paramWaveSmokeTex->SetExtDXTexture(m_pWaveSmokeTex->GetD3DTexture());
	m_pMtlWaterWave->paramDayNightFactor->Set1f(pA3DEnv->GetDNFactor());
	m_pMtlWaterWave->paramMainLightColor->Set4f(vMainLight);
	m_pMtlWaterWave->paramAmbientColor->Set4f(vAmbient);

	//m_pMtlWaterWave->paramMainLightColor->Set4f(A3DVECTOR4(1,0,0,1));	// 肿么设?肿么设?
	//m_pMtlWaterWave->paramAmbientColor->Set4f(A3DVECTOR4(0,0,1,1));		// 肿么设?肿么设?

	m_pA3DDevice->SetVertexDeclaration(GetWaterWaveVertDecl());

	A3DFXMaterial* pFXMtl = m_pMtlWaterWave->GetFXMaterial();
	pFXMtl->GetCommonParams()->Update(A3DMATRIX4::matIdentity, prp->pCurViewport->GetCamera());
	pFXMtl->SetTechnique("RenderWaterWave");

	DWORD dwNumPass = pFXMtl->BeginRender();
	for (DWORD dwPass=0; dwPass < dwNumPass; dwPass++)
	{
		pFXMtl->BeginPass(dwPass);


		for(int i=0; i<pAreaArry->GetSize(); i++)
		{
			pArea = (*pAreaArry)[i];
			m_pMtlWaterWave->paramSightRange->Set3f(m_fWaveAttenStart, 1.0f / m_fWaveAttenEnd, pArea->GetWaveAlpha());

			int nNumWave = pArea->GetWaveCount();
			for(int i = 0; i < nNumWave; i++)
			{
				A3DWaterWave* pWave = pArea->GetWave(i);
				nVert = pWave->GetVertCount();
				nFaces = pWave->GetFaceCount();

				A3DStream* pStream = pWave->GetStream();
				if(pStream != NULL)
				{
					pStream->Appear(0, false);
					m_pA3DDevice->DrawIndexedPrimitive(A3DPT_TRIANGLELIST, 0, 0, nVert, 0, nFaces);
				}
			}
		}
		pFXMtl->EndPass();
	}
	pFXMtl->EndRender();
	*/
	return true;
}

//FIXME!!
////	Enable water dimple. This function create resources, so don't call it frequently
//bool A3DTerrainWater2::EnableWaterDimple(float fAreaRadius/* 100.0f */, int iMapSize/* 512 */)
//{
//	
//	if (m_pWaterDimple)
//		return true;
//
//	if (!A3DWaterDimple::CheckDeviceSupport(m_pA3DDevice))
//		return false;	//	Device couldn't support dimple effect
//
//	m_pWaterDimple = new A3DWaterDimple;
//	if (!m_pWaterDimple || !m_pWaterDimple->Init(m_pA3DDevice->GetA3DEngine(), fAreaRadius, iMapSize))
//	{
//		g_A3DErrLog.Log("A3DTerrainWater::EnableWaterDimple, failed to create water dimple");
//		return false;
//	}
//	
//	return true;
//}
//
////	Disable water dimple. This function release resources, so don't call it frequently
//void A3DTerrainWater2::DisableWaterDimple()
//{
//	
//	A3DRELEASE(m_pWaterDimple);
//}
//
//
////	Add a water dimple
////	
////	vPos: dimple center position in world space
////	fRadius: dimple radius
////	fSpeed: wave move speed
////	fLength: wave length
////	fAmp: wave amplitude
////	fTime: current time. If fTime < 0.0f, this dimple will stay in latent period for some time
////	fAtten: attenuation factor when dimple move away it's center position
////
//bool A3DTerrainWater2::AddWaterDimple(const A3DVECTOR3& vPos, float fRadius/* 12.0f */, float fSpeed/* 8.0f */, 
//								float fLength/* 2.0f */, float fAmp/* 0.8f */, float fTime/* 0.0f */, float fAtten/* 2.0f */)
//{
//	
//	
//	if (!m_pWaterDimple)
//		return false;
//
//	return m_pWaterDimple->CreateDimple(vPos, fRadius, fSpeed, fLength, fAmp, fTime, fAtten);
//	
//	return true;
//}


