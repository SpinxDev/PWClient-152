/*
 * FILE: HomeTerrain.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2006/6/6
 *
 * HISTORY: 
 *
 * Copyright (c) 2006 Archosaur Studio, All Rights Reserved.
 */

#include "HomeTerrain.h"
#include "AutoScene.h"
#include "HomeTrnMask.h"
#include "AutoSceneConfig.h"
#include "Bitmap.h"

#include <A3DTerrain2Env.h>
#include <A3DTerrain2Render.h>
#include <A3DCameraBase.h>

#define new A_DEBUG_NEW

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

#define MAX_TRN_HEIGHT	800.0f

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
//	Implement CHomeTerrainLoader
//	
///////////////////////////////////////////////////////////////////////////

CHomeTerrainLoader::CHomeTerrainLoader(A3DTerrain2* pTerrain, int iLoaderID) : 
A3DTerrain2Loader(pTerrain, iLoaderID)
{
}

CHomeTerrainLoader::~CHomeTerrainLoader()
{
}

//	Initialize object
bool CHomeTerrainLoader::Init(const char* szTrn2File)
{
	return true;
}

//	Release object
void CHomeTerrainLoader::Release()
{
}

//	Load block
A3DTerrain2Block* CHomeTerrainLoader::LoadBlock(int r, int c, int iBlock)
{
	//	Create a A3DTerrain2Block object
	CHomeTrnBlock* pBlock = new CHomeTrnBlock(m_pt);
	if (!pBlock || !pBlock->Init())
		return NULL;

	//	Fill block properties
	pBlock->m_iMaskArea		= iBlock;
	pBlock->m_iRowInTrn		= r;
	pBlock->m_iColInTrn		= c;
//	pBlock->m_fLODScale		= BlockInfo.fLODScale;
	pBlock->m_fLODScale		= 1.0f;
	pBlock->m_dwLayerFlags	= 0xffffffff;
	pBlock->m_iLODLevel		= 3;
	pBlock->m_dwLODFlag		= 0;
	pBlock->m_iFirstLayer	= 0;
	
	//	Search the first layer which effect this block
	for (int i=0; i < 32; i++)
	{
		if (pBlock->m_dwLayerFlags & (1 << i))
		{
			pBlock->m_iFirstLayer = i;
			break;
		}
	}

	//	Calculate block's initial aabb
	const ARectF& rcTerrain = m_pt->GetTerrainArea();
	float sx = rcTerrain.left + c * m_pt->GetBlockSize();
	float sz = rcTerrain.top - r * m_pt->GetBlockSize();
	pBlock->m_aabbBlock.Mins.Set(sx, 0.0f, sz - m_pt->GetBlockSize());
	pBlock->m_aabbBlock.Maxs.Set(sx + m_pt->GetBlockSize(), MAX_TRN_HEIGHT, sz);

	//	Do some calculation
	int iNumBlock = m_pt->GetMaskGrid() / m_pt->GetBlockGrid();
	int iRow = pBlock->m_iRowInTrn / iNumBlock;
	int iCol = pBlock->m_iColInTrn / iNumBlock;
	pBlock->m_iRowInMask = pBlock->m_iRowInTrn % iNumBlock;
	pBlock->m_iColInMask = pBlock->m_iColInTrn % iNumBlock;

	iNumBlock = m_pt->GetSubTerrainGrid() / m_pt->GetBlockGrid();
	iRow = pBlock->m_iRowInTrn / iNumBlock;
	iCol = pBlock->m_iColInTrn / iNumBlock;
	pBlock->m_iSubTerrain = iRow * (m_pt->GetBlockColNum() / iNumBlock) + iCol;

	A3DTerrain2::TEMPBUFS& tbuf = m_pt->GetTempBuffers();
	ASSERT(tbuf.iNumVert == pBlock->m_iNumVert);
	ASSERT(tbuf.aTempHei);
	ASSERT(tbuf.aTempNormal);
	ASSERT(tbuf.aTempDiff);
	ASSERT(tbuf.aTempDiff1);

	pBlock->m_bDataLoaded = false;

	//	Save this block's position
	CHomeTerrain* pTerrain = (CHomeTerrain*)m_pt;
	CHomeTerrain::BLOCKPOS bp = {r, c};
	pTerrain->m_NewBlockList.AddTail(bp);
	
	return pBlock;
}

//	Unload a block
void CHomeTerrainLoader::UnloadBlock(A3DTerrain2Block* pBlock)
{
	ASSERT(pBlock);
	pBlock->Release();
	delete pBlock;
}

//	Load a mask area
A3DTerrain2Mask* CHomeTerrainLoader::LoadMask(int iMaskArea)
{
	//	Create new mask
	CHomeTrnMask* pMask = new CHomeTrnMask(m_pt, iMaskArea);
	if (!pMask)
	{
		a_LogOutput(1, "CHomeTerrainLoader::LoadMask, not enough memory !");
		return NULL;
	}

	if (!pMask->Init(((CHomeTerrain*)m_pt)->GetMaxLayerNum()))
	{
		delete pMask;
		a_LogOutput(1, "CHomeTerrainLoader::LoadMask, Failed to initialize mask !");
		return NULL;
	}

	return pMask;
}

//	Unload a mask area
void CHomeTerrainLoader::UnloadMask(A3DTerrain2Mask* pMaskArea)
{
	ASSERT(pMaskArea);
	pMaskArea->Release();
	delete pMaskArea;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CHomeTerrain
//	
///////////////////////////////////////////////////////////////////////////

CHomeTerrain::CHomeTerrain(CAutoScene* pScene) : A3DTerrain2()
{
	m_pAutoScene	= pScene;
	m_pWhiteBmp		= NULL;
	m_pBlackBmp		= NULL;
	m_iMaxLayerNum	= 0;
	m_iBlkLoadSpeed	= 1024;
}

CHomeTerrain::~CHomeTerrain()
{
}

//	Initialize terrain object
bool CHomeTerrain::Init(float fViewRadius)
{
	if (!A3DTerrain2::Init(m_pAutoScene->GetA3DEngine(), fViewRadius, fViewRadius, false, true))
	{
		a_LogOutput(1, "CHomeTerrain::Init, Failed to call A3DTerrain2::Init.");
		return false;
	}

	//	Initialize some members
	m_iNumAllBlockRow	= m_pAutoScene->GetBlockRowNum();
	m_iNumAllBlockCol	= m_pAutoScene->GetBlockColNum();
	m_fBlockSize		= m_pAutoScene->GetBlockSize();
	m_fTerrainWid		= m_iNumAllBlockCol * m_fBlockSize;
	m_fTerrainLen		= m_iNumAllBlockRow * m_fBlockSize;
	m_fGridSize			= 2.0f;
	m_iBlockGrid		= 64;
	m_iMaskGrid			= 64;
	m_iSubTrnGrid		= m_iNumAllBlockCol * m_iBlockGrid;
	m_iNumBlock			= m_iNumAllBlockRow * m_iNumAllBlockCol;
	m_iNumMaskArea		= m_iNumBlock;
	m_fBlockRadius		= m_fBlockSize * 0.7071f;

	CAutoSceneConfig* pCfg = m_pAutoScene->GetAutoSceneConfig();
	m_iMaxLayerNum = pCfg->GetNumMaxLayer();

	ASSERT(m_iBlockGrid == m_iMaskGrid);

	//	Create LOD manager
	if (!(m_pLODMan = A3D::g_pA3DTerrain2Env->CreateLODManager(m_iBlockGrid)))
	{
		a_LogOutput(1, "CHomeTerrain::Init, Failed to create LOD manager");
		return false;
	}

	//	Adjust LOD distance according to block size
	SetLODType(LOD_NOHEIGHT);
//	SetLODDist(m_aLODDist[0], m_aLODDist[1]);
	SetLODDist(m_fBlockSize * 0.5f, m_fBlockSize + m_fBlockRadius * 2.3f);

	//	Create sub-terrain data
	if (!CreateSubTerrain())
	{
		a_LogOutput(1, "CHomeTerrain::Init, Failed to create sub-terrain");
		return false;
	}

	//	Create pure black and white bitmap
	if (!CreatePureColorBmp())
	{
		a_LogOutput(1, "CHomeTerrain::Init, Failed to call CreatePureColorBmp");
		return false;
	}

	//	Create mask slots
	if (!(m_aMaskSlots = new MASKSLOT [m_iNumMaskArea]))
	{
		a_LogOutput(1, "CHomeTerrain::Init, Not enough memory !");
		return false;
	}

	memset(m_aMaskSlots, 0, m_iNumMaskArea * sizeof (MASKSLOT));

	//	Create terrain loader
	if (!CreateTerrainLoader())
	{
		a_LogOutput(1, "CHomeTerrain::Init, Failed to create terrain laoder");
		return false;
	}

	//	Create temporary buffer
	if (!CreateTempBufs())
	{
		a_LogOutput(1, "CHomeTerrain::Init, Failed to create temporary buffer");
		return false;
	}

	//	Calculate terrain area
	m_rcTerrain.left	= 0 - m_fTerrainWid * 0.5f;
	m_rcTerrain.top		= 0 + m_fTerrainLen * 0.5f;
	m_rcTerrain.right	= m_rcTerrain.left + m_fTerrainWid;
	m_rcTerrain.bottom	= m_rcTerrain.top - m_fTerrainLen;

	m_bMultiThread	= false;
	m_bDataLoaded	= true;

	//	Notify render that terrain data has been loaded
	m_TerrainRender.OnTerrainLoaded();

	return true;
}

//	Release terrain object
void CHomeTerrain::Release()
{
	A3DRELEASE(m_pWhiteBmp);
	A3DRELEASE(m_pBlackBmp);

	A3DTerrain2::Release();
}

//	Create sub-terrain
bool CHomeTerrain::CreateSubTerrain()
{
	m_aSubTerrains.SetSize(1, 4);
	m_aSubTerrains[0].fMinHei = 0.0f;
	m_aSubTerrains[0].fMaxHei = MAX_TRN_HEIGHT;
	return true;
}

//	Create terrain loader
bool CHomeTerrain::CreateTerrainLoader()
{
	CHomeTerrainLoader* pLoader = new CHomeTerrainLoader(this, 0);
	if (!pLoader->Init(""))
	{
		a_LogOutput(1, "CHomeTerrain::CreateTerrainLoader, Failed to initialize terrain laoder");
		return false;
	}

	m_pTrnLoader = pLoader;

	return true;
}

//	Create pure black and white bitmap
bool CHomeTerrain::CreatePureColorBmp()
{
	int iMapSize = CHomeTrnMask::MASKMAP_SIZE;

	//	Create pure white bitmap
	if (!m_pWhiteBmp)
	{
		if (!(m_pWhiteBmp = new CELBitmap))
			return false;

		if (!m_pWhiteBmp->CreateBitmap(iMapSize, iMapSize, 8, NULL))
		{
			a_LogOutput(1, "CHomeTerrain::CreatePureColorBmp, Failed to create pure white bitmap");
			return false;
		}

		BYTE* pData = m_pWhiteBmp->GetBitmapData();
		memset(pData, 0xff, m_pWhiteBmp->GetPitch() * m_pWhiteBmp->GetHeight());
	}

	//	Create pure black bitmap
	if (!m_pBlackBmp)
	{
		if (!(m_pBlackBmp = new CELBitmap))
			return false;

		if (!m_pBlackBmp->CreateBitmap(iMapSize, iMapSize, 8, NULL))
		{
			a_LogOutput(1, "CHomeTerrain::CreatePureColorBmp, Failed to create pure black bitmap");
			return false;
		}

		m_pBlackBmp->Clear();
	}

	return true;
}

//	Create temporary buffers
bool CHomeTerrain::CreateTempBufs()
{
	//	Create temporary buffers
	if (!A3DTerrain2::CreateTempBufs())
	{
		a_LogOutput(1, "CHomeTerrain::CreateTempBufs, Failed to create temporary buffers");
		return false;
	}
	
	//	Because CHomeTerrain don't have the loading process, so we have to
	//	initialize temporary buffers with some values
	int iNumVert = (m_iBlockGrid + 1) * (m_iBlockGrid + 1);
	memset(m_TempBufs.aTempHei, 0, iNumVert * sizeof (float));
	memset(m_TempBufs.aTempDiff, 0xff, iNumVert * sizeof (DWORD));
	memset(m_TempBufs.aTempDiff1, 0xff, iNumVert * sizeof (DWORD));

	for (int i=0; i < iNumVert; i++)
	{
		int n = i * 3;
		m_TempBufs.aTempNormal[n+0] = 0.0f;
		m_TempBufs.aTempNormal[n+1] = 1.0f;
		m_TempBufs.aTempNormal[n+2] = 0.0f;
	}

	return true;
}

//	Update routine
bool CHomeTerrain::Update(DWORD dwDeltaTime, A3DCameraBase* pCamera, const A3DVECTOR3& vLoadCenter)
{
/*	A3DEngine* pA3DEngine = m_pAutoScene->GetA3DEngine();
	pA3DEngine->BeginPerformanceRecord(A3DEngine::PERF_TICK_SCENEUPDATE);
	
	//	Update active blocks
	if (!UpdateActiveBlocks(vLoadCenter))
	{
		pA3DEngine->EndPerformanceRecord(A3DEngine::PERF_TICK_SCENEUPDATE);
		return false;
	}

	//	Load blocks from candidate list
	if (!LoadCandidateBlocks())
	{
		pA3DEngine->EndPerformanceRecord(A3DEngine::PERF_TICK_SCENEUPDATE);
		return false;
	}

	if (m_pCurActBlocks->rcArea.IsEmpty())
	{
		pA3DEngine->EndPerformanceRecord(A3DEngine::PERF_TICK_SCENEUPDATE);
		return true;
	}

	//	Select visible blocks
	SelectVisibleBlocks(pCamera);

	if (m_pTrnLoader)
		m_pTrnLoader->Tick(dwDeltaTime);

	pA3DEngine->EndPerformanceRecord(A3DEngine::PERF_TICK_SCENEUPDATE);

	return true;
*/
	return A3DTerrain2::Update(dwDeltaTime, pCamera, vLoadCenter);
}

//	Get bitmap of specified mask and layer
CELBitmap* CHomeTerrain::GetHomeAreaMaskBmp(int r, int c, int iLayer)
{
	CHomeTrnMask* pMaskArea = GetHomeAreaMask(r, c);
	return pMaskArea ? pMaskArea->GetLayerBitmap(iLayer) : NULL;
}

//	Get specified mask area
CHomeTrnMask* CHomeTerrain::GetHomeAreaMask(int r, int c)
{
	int iMaskColNum = m_iNumAllBlockCol / (m_iMaskGrid / m_iBlockGrid);
	int iMaskArea = r * iMaskColNum + c;
	return (CHomeTrnMask*)GetMaskArea(iMaskArea);
}

//	Update mask area data
bool CHomeTerrain::UpdateHomeAreaMask(int r, int c, int* aLayers, int iNumLayer)
{
	CHomeTrnMask* pMaskArea = GetHomeAreaMask(r, c);
	if (pMaskArea)
		return pMaskArea->ApplyLayerBitmap(aLayers, iNumLayer);
	else
		return false;
}

bool CHomeTerrain::UpdateHomeAreaMask(int r, int c)
{
	AArray<int, int> aLayers(m_iMaxLayerNum, 4);
	for (int i=0; i < m_iMaxLayerNum; i++)
		aLayers.Add(i);

	return UpdateHomeAreaMask(r, c, aLayers.GetData(), aLayers.GetSize());
}

//	Get home areas which are just created and needs updating
bool CHomeTerrain::GetNextNewHomeArea(int n, int& r, int& c)
{
	if (!m_NewBlockList.GetCount())
		return false;

	BLOCKPOS bp = m_NewBlockList.RemoveHead();
	r = bp.r;
	c = bp.c;

	return true;
}

//	Update specified mask area height and normal
bool CHomeTerrain::UpdateHomeAreaHeight(int r, int c, const float* aHeiData, 
								const A3DVECTOR3* aNormals, int iHMSize)
{
	A3DTerrain2Block* pBlock = m_pCurActBlocks->GetBlock(r, c, false);
	if (!pBlock)
		return false;

	const float* pSrcLine1 = aHeiData;
	const A3DVECTOR3* pSrcLine2 = aNormals;
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

	//	Complete aabb
	((CHomeTrnBlock*)pBlock)->m_aabbBlock.CompleteCenterExts();

	((CHomeTrnBlock*)pBlock)->m_bDataLoaded = true;

	return false;
}

//	Update specified mask area vertices color
bool CHomeTerrain::UpdateHomeAreaColor(int r, int c, const DWORD* aColors1, 
								const DWORD* aColors2, int iHMSize)
{
	A3DTerrain2Block* pBlock = m_pCurActBlocks->GetBlock(r, c, false);
	if (!pBlock)
		return false;

	const DWORD* pSrcLine1 = aColors1;
	const DWORD* pSrcLine2 = aColors2;
	int cnt = 0;

	//	Fill temporary height buffer
	for (int i=0; i < m_iBlockGrid + 1; i++)
	{
		const DWORD* psrc1 = pSrcLine1;
		const DWORD* psrc2 = pSrcLine2;
		pSrcLine1 += iHMSize;
		pSrcLine2 += iHMSize;

		for (int j=0; j < m_iBlockGrid + 1; j++, psrc1++, psrc2++)
		{
			m_TempBufs.aTempDiff[cnt]  = *psrc1;
			m_TempBufs.aTempDiff1[cnt] = *psrc2;
			cnt++;
		}
	}

	pBlock->FillVertexColor();

	return true;
}





