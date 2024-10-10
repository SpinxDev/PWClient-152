  /*
 * FILE: A3DTerrain2.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/6/5
 *
 * HISTORY: 
 * 
 *	2005.2: 使用 pixel shader 1.4 来渲染。每个 pass 可以渲染最多 4 层个地形 layers。
 *			这要求加载 mask 时，将 4 层 mask 合并到 1 个 32-bit 位图中，加上 4 张
 *			layer texture 和 1 张 lightmap. 每个渲染 pass 一共使用 6 张 textures.
 *			使用这种方法渲染，要求每一个 layer mask 的大小一致，这一点在做地形的时候
 *			要多加注意!!
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "A3DTerrain2.h"
#include "A3DPI.h"
#include "A3DConfig.h"
#include "A3DEngine.h"
#include "A3DDevice.h"
#include "A3DTerrain2Blk.h"
#include "A3DTexture.h"
#include "A3DTextureMan.h"
#include "A3DCameraBase.h"
#include "A3DTerrain2Env.h"
#include "A3DViewport.h"
#include "A3DTerrain2LoaderB.h"
#include "A3DTerrain2LoaderA.h"
#include "A3DTerrain2Cull.h"
#include "A3DFuncs.h"
#include "AMemory.h"
#include "AAssist.h"
#include "ACSWrapper.h"
#include "A3DWireCollector.h"

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

//	Structure used by CompressHeightMap and DecompressHeightMap
struct CH_DATAINFO
{
	BYTE	byVer;
	float	fMinHei;
	float	fMaxHei;
	int		iNumVert;
};

//	Structure used by CompressNormalMap and DecompressNormalMap
struct CN_DATAINFO
{
	BYTE	byVer;
	int		iNumVert;
};

//	Structure used by CompressColorMap and DecompressColorMap
struct CC_DATAINFO
{
	BYTE	byVer;
	int		iNumVert;
};

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

A3DTerrain2::A3DTerrain2() : 
m_CandidateBlkList(64),
m_LightScale(1.0f)
{
	m_pA3DEngine		= NULL;
	m_pA3DDevice		= NULL;
	m_pCurRender		= NULL;
	m_iNumAllBlockRow	= 0;
	m_iNumAllBlockCol	= 0;
	m_fTerrainWid		= 0.0f;
	m_fTerrainLen		= 0.0f;
	m_bDataLoaded		= false;
	m_bMultiThread		= false;
	m_fViewRadius		= 100.0f;
	m_fActRadius		= 100.0f;
	m_fBlockSize		= 0.0f;
	m_fBlockRadius		= 0.0f;
	m_iBlockGrid		= 0;
	m_iMaskGrid			= 0;
	m_bAsynLoad			= false;
	m_pCurActBlocks		= NULL;
	m_pOldActBlocks		= NULL;
	m_iMaxCachedBlk		= 128;
	m_iNumBlock			= 0;
	m_iNumMaskArea		= 0;
	m_LODType			= LOD_USEHEIGHT;
	m_bForceUpdate		= false;
	m_iBlkLoadSpeed		= 8;
	m_aMaskSlots		= NULL;
	m_bNoRender			= false;
	m_iLayerWeight		= MAX_LAYERWEIGHT;
	m_iLoaderID			= A3DTerrain2Loader::LOADER_A;
	m_pTrnLoader		= NULL;
	m_pTrnCuller		= NULL;
	m_pLODMan			= NULL;
	m_fDNFactor			= 0.0f;
	m_bLightEnable		= true;
	m_fTexOffU			= 0.0f;
	m_fTexOffV			= 0.0f;
	m_bVertexLight		= false;
	m_bTexMerge			= false;


	m_bRenderForRefract	= false;
	m_vRefractSurfaceHeight = 0.0f;
	m_bUnderWaterRender	= false;
	m_pTexCaustic		= NULL;

	m_aLODDist[0]		= 200.0f;
	m_aLODDist[1]		= 400.0f;

	m_vLoadCenter.Clear();

	memset(&m_MainLightParam, 0, sizeof (m_MainLightParam));
	memset(&m_TempBufs, 0, sizeof (m_TempBufs));

	InitializeCriticalSection(&m_csLoadBlk);
	InitializeCriticalSection(&m_csUnloadBlk);
	InitializeCriticalSection(&m_csMaskSlot);
}

A3DTerrain2::~A3DTerrain2()
{
	DeleteCriticalSection(&m_csLoadBlk);
	DeleteCriticalSection(&m_csUnloadBlk);
	DeleteCriticalSection(&m_csMaskSlot);
}

/*	Initialize terrain object

	pA3DEngine: a3d engine object
	fViewRadius: view radius (metres)
	fActRadius: active area radius (metres)
	bNoRender: true, terrain won't have render data
	bVertexLight: true, force to use vertex light rather than lightmap
*/
bool A3DTerrain2::Init(A3DEngine* pA3DEngine, float fViewRadius, float fActRadius,
					bool bNoRender/* false */, bool bVertexLight/* false */, bool bLightMap32Bit/* false */)
{
	ASSERT(pA3DEngine);

	if (g_pA3DConfig->GetRunEnv() == A3DRUNENV_PURESERVER)
		bNoRender = true;

	m_bNoRender		= bNoRender;
	m_pA3DEngine	= pA3DEngine;
	m_pA3DDevice	= pA3DEngine->GetA3DDevice();
	m_fViewRadius	= fViewRadius;
	m_fActRadius	= fActRadius;

	//	Use default material
	m_Material.Init(m_pA3DDevice);
	m_Material.SetPower(30.0f);
	m_Material.SetEmissive(A3DCOLORVALUE(1.0f));
	m_Material.SetSpecular(A3DCOLORVALUE(0.8f));

	//	Initlaize terrain2 environment
	if (!A3DTerrain2Env::CreateTerrain2Env(pA3DEngine, bLightMap32Bit))
	{
		g_A3DErrLog.Log("A3DTerrain2::Init, Failed to create terrain2 environment.");
		return false;
	}

	//	When video card doesn't support PS, force to use vertex light 
	m_bVertexLight = A3D::g_pA3DTerrain2Env->GetSupportPSFlag() ? bVertexLight : true;

	//	Limit view radius to active area radius
	a_ClampRoof(m_fViewRadius, m_fActRadius);

	m_ActBlocks1.rcArea.Clear();
	m_ActBlocks2.rcArea.Clear();
	m_pCurActBlocks	= &m_ActBlocks1;
	m_pOldActBlocks	= &m_ActBlocks2;

	//	Initialize texture merger
	if (!m_TexMerger.Init(this))
	{
		g_A3DErrLog.Log("A3DTerrain2::Init, Failed to init texture merger.");
		return false;
	}

	//	Initalize render
	if (!m_TerrainRender.Init(this))
	{
		g_A3DErrLog.Log("A3DTerrain2::Init, Failed to init render.");
		return false;
	}

	m_pCurRender = &m_TerrainRender;

	if (m_pA3DDevice)
		m_pA3DDevice->AddUnmanagedDevObject(this);

	return true;
}

//	Release terrain object
void A3DTerrain2::Release()
{
	if (m_pA3DDevice)
		m_pA3DDevice->RemoveUnmanagedDevObject(this);

	m_Material.Release();

	//	Release all loaded blocks
	ReleaseAllBlocks();

	//	Release all mask slots
	ReleaseMaskSlots();

	//	Release texture slots
	A3DTextureMan* pTexMan = m_pA3DEngine->GetA3DTextureMan();

	int i;
	for (i=0; i < m_aTextures.GetSize(); i++)
	{
		TEXTURE* pSlot = m_aTextures[i];

		if (pSlot->pA3DTexture)
			pTexMan->ReleaseTexture(pSlot->pA3DTexture);

		delete pSlot;
	}

	m_aTextures.RemoveAll();

	m_CandidateBlkList.RemoveAll();

	//	Release loader
	A3DRELEASE(m_pTrnLoader);
	//	Release texture merger
	m_TexMerger.Release();
	//	Release render
	m_TerrainRender.Release();

	//	Release temporary buffers
	ReleaseTempBufs();

	m_pLODMan		= NULL;
	m_bDataLoaded	= false;
}

//	Before device reset
bool A3DTerrain2::BeforeDeviceReset()
{
	return true;
}

//	After device reset
bool A3DTerrain2::AfterDeviceReset()
{
	m_TerrainRender.SetRebuildStreamFlag(true);
	if (m_pCurRender)
		m_pCurRender->SetRebuildStreamFlag(true);

	m_bForceUpdate = true;

	//	Force to update all blocks
	BlockArray& aBlocks = m_pCurActBlocks->aBlocks;
	for (int i=0; i < aBlocks.GetSize(); i++)
	{
		if (aBlocks[i])
			aBlocks[i]->m_dwLODFlag = 0;
	}

	return true;
}

//	Set view radius
void A3DTerrain2::SetViewRadius(float fViewRadius)
{
	//	Limit view radius to active area radius
	m_fViewRadius = fViewRadius;
	a_ClampRoof(m_fViewRadius, m_fActRadius);
}

//	Set active radius
void A3DTerrain2::SetActRadius(float fActRadius)
{
	//	Limit view radius to active area radius
	m_fActRadius = fActRadius;
	a_ClampRoof(m_fViewRadius, m_fActRadius);
}

/*	Load terrain data from file

	szFileName: terrain file name.
	cx, cz: terrain center's position on xz plane
	bMultiThread: terrain resources will be loaded by other threads.
*/
bool A3DTerrain2::Load(const char* szFileName, float cx, float cz, bool bMultiThread/* false */)
{
	if (m_bDataLoaded)
		return false;

	AFileImage DescFile;
	if (!DescFile.Open("", szFileName, AFILE_OPENEXIST | AFILE_BINARY | AFILE_TEMPMEMORY))
	{
		g_A3DErrLog.Log("A3DTerrain2::Load, Failed to open file %s", szFileName);
		return false;
	}

	DWORD dwRead;

	//	Read file ID and version
	TRN2FILEIDVER IdVer;
	if (!DescFile.Read(&IdVer, sizeof (IdVer), &dwRead))
	{
		g_A3DErrLog.Log("A3DTerrain2::Load, Failed to read file version");
		return false;
	}

	if (IdVer.dwIdentify != TRN2FILE_IDENTIFY)
	{
		g_A3DErrLog.Log("A3DTerrain2::Load, File format error");
		return false;
	}
	
	if (IdVer.dwVersion > TRN2FILE_VERSION)
	{
		g_A3DErrLog.Log("A3DTerrain2::Load, Version %d expected but %d is given", TRN2FILE_VERSION, IdVer.dwVersion);
		return false;
	}

	//	Read file header
	TRN2FILEHEADER T2Header;
	if (!DescFile.Read(&T2Header, sizeof (T2Header), &dwRead))
	{
		g_A3DErrLog.Log("A3DTerrain2::Load, Failed to read terrain file header");
		return false;
	}

	m_iNumAllBlockRow	= T2Header.iTrnBlkRow;
	m_iNumAllBlockCol	= T2Header.iTrnBlkCol;	
	m_fTerrainWid		= T2Header.fTerrainWid;
	m_fTerrainLen		= T2Header.fTerrainHei;
	m_fBlockSize		= T2Header.fBlockSize;
	m_fGridSize			= T2Header.fGridSize;
	m_iBlockGrid		= T2Header.iBlockGrid;
	m_iMaskGrid			= T2Header.iMaskGrid;
	m_iSubTrnGrid		= T2Header.iSubTrnGrid;
	m_iNumBlock			= T2Header.iNumBlock;
	m_iNumMaskArea		= T2Header.iNumMaskArea;
	m_fBlockRadius		= m_fBlockSize * 0.7071f;

	ASSERT(m_iNumBlock == m_iNumAllBlockRow * m_iNumAllBlockCol);
//	ASSERT(m_iBlockGrid == g_pA3DConfig->GetTerrain2BlkGrid());

	//	Create LOD manager
	if (!(m_pLODMan = A3D::g_pA3DTerrain2Env->CreateLODManager(m_iBlockGrid)))
	{
		g_A3DErrLog.Log("A3DTerrain2::Load, Failed to create LOD manager");
		return false;
	}

	//	Adjust LOD distance according to block size
	SetLODDist(m_aLODDist[0], m_aLODDist[1]);

	//	Load sub-terrain data
	if (!LoadSubTerrainData(&DescFile, T2Header.iSubTrnRow * T2Header.iSubTrnCol))
	{
		g_A3DErrLog.Log("A3DTerrain2::Load, Failed to load sub-terrain data");
		return false;
	}

	//	Load texture information
	DescFile.Seek((int)T2Header.dwTexOffset, AFILE_SEEK_SET);
	if (!LoadTextureInfo(&DescFile, T2Header.iNumTexture))
	{
		g_A3DErrLog.Log("A3DTerrain2::Load, Failed to load texture information");
		return false;
	}

	DescFile.Close();

	//	Create mask slots
	if (!(m_aMaskSlots = new MASKSLOT [m_iNumMaskArea]))
	{
		g_A3DErrLog.Log("A3DTerrain2::Load, Not enough memory !");
		return false;
	}

	memset(m_aMaskSlots, 0, m_iNumMaskArea * sizeof (MASKSLOT));

	//	Create terrain loader
	if (T2Header.iLoaderID == A3DTerrain2Loader::LOADER_A)
	{
		//	Terrain loader A doesn't support multithread now !!
		if (bMultiThread)
		{
			ASSERT(!bMultiThread);
			return false;
		}

		A3DTerrain2LoaderA* pLoader = new A3DTerrain2LoaderA(this);
		if (!pLoader->Init(szFileName))
		{
			g_A3DErrLog.Log("A3DTerrain2::Load, Failed to initialize terrain2 laoder");
			return false;
		}

		m_pTrnLoader = pLoader;
	}
	else if (T2Header.iLoaderID == A3DTerrain2Loader::LOADER_B)
	{
		A3DTerrain2LoaderB* pLoader = CreateLoaderB();
		if (!pLoader->Init(szFileName))
		{
			g_A3DErrLog.Log("A3DTerrain2::Load, Failed to initialize terrain2 laoder");
			return false;
		}

		m_pTrnLoader = pLoader;
	}
	else
	{
		ASSERT(0);
		g_A3DErrLog.Log("A3DTerrain2::Load, unknown terrain loader ID %d", T2Header.iLoaderID);
		return false;
	}

	//	Create temporary buffers
	if (!CreateTempBufs())
	{
		g_A3DErrLog.Log("A3DTerrain2::Load, Failed to create temporary buffers");
		return false;
	}

	//	Calculate terrain area
	m_rcTerrain.left	= cx - m_fTerrainWid * 0.5f;
	m_rcTerrain.top		= cz + m_fTerrainLen * 0.5f;
	m_rcTerrain.right	= m_rcTerrain.left + m_fTerrainWid;
	m_rcTerrain.bottom	= m_rcTerrain.top - m_fTerrainLen;

	m_bMultiThread	= bMultiThread;
	m_bDataLoaded	= true;

	//	Notify render that terrain data has been loaded
	m_TerrainRender.OnTerrainLoaded();

	return true;
}

//	create terrain loader B
A3DTerrain2LoaderB* A3DTerrain2::CreateLoaderB()
{
	return new A3DTerrain2LoaderB(this);
}

//	Read sub-terrain data in terrain file
bool A3DTerrain2::LoadSubTerrainData(AFile* pFile, int iNumSubTerrain)
{
	TRN2FILESUBTRN* aSrc = new TRN2FILESUBTRN [iNumSubTerrain];
	if (!aSrc)
		return false;

	m_aSubTerrains.SetSize(iNumSubTerrain, 100);

	DWORD dwRead;
	if (!pFile->Read(aSrc, iNumSubTerrain * sizeof (TRN2FILESUBTRN), &dwRead))
	{
		delete [] aSrc;
		return false;
	}

	for (int i=0; i < iNumSubTerrain; i++)
	{
		m_aSubTerrains[i].fMinHei	= aSrc[i].fMinHei;
		m_aSubTerrains[i].fMaxHei	= aSrc[i].fMaxHei;
	}

	delete [] aSrc;

	return true;
}

//	Read texture information in terrain file
bool A3DTerrain2::LoadTextureInfo(AFile* pFile, int iNumTexture)
{
	if (m_bNoRender)
		return true;

	m_aTextures.SetSize(iNumTexture, 100);

	for (int i=0; i < iNumTexture; i++)
	{
		TEXTURE* pSlot = new TEXTURE;
		if (!pSlot)
			return false;
		
		pSlot->pA3DTexture	= NULL;
		pSlot->iRefCount	= 0;

		m_aTextures[i] = pSlot;

		//	Read texture file name from file
		pFile->ReadString(pSlot->strFileName);
	}

	return true;
}

/*	Update routine. This function clip blocks which couldn't be seen by
	specified camera.
 	
	dwDeltaTime: tick time
	pCamera: camera used to select visible blocks
	vLoadCenter: center position used to calculate active terrain blocks
*/
bool A3DTerrain2::Update(DWORD dwDeltaTime, A3DCameraBase* pCamera,
						 const A3DVECTOR3& vLoadCenter)
{
	ASSERT(m_pA3DEngine);
	m_pA3DEngine->BeginPerformanceRecord(A3DEngine::PERF_TICK_SCENEUPDATE);
	
	//	Update active blocks
	if (!UpdateActiveBlocks(vLoadCenter))
	{
		m_pA3DEngine->EndPerformanceRecord(A3DEngine::PERF_TICK_SCENEUPDATE);
		return false;
	}

	if (!m_bMultiThread)
	{
		//	Load blocks from candidate list
		if (!LoadCandidateBlocks())
		{
			m_pA3DEngine->EndPerformanceRecord(A3DEngine::PERF_TICK_SCENEUPDATE);
			return false;
		}
	}
	else
	{
		//	Activate loaded blocks
		ActivateLoadedBlocks();
	}

	if (m_pCurActBlocks->rcArea.IsEmpty())
	{
		m_pA3DEngine->EndPerformanceRecord(A3DEngine::PERF_TICK_SCENEUPDATE);
		return true;
	}

	//	Update block LOD info
	UpdateBlocksLOD(vLoadCenter);

	if (m_pTrnLoader)
	{
		m_pTrnLoader->Tick(dwDeltaTime);

		if (!m_bMultiThread && m_pTrnLoader->GetLoaderID() == A3DTerrain2Loader::LOADER_B)
			((A3DTerrain2LoaderB*)m_pTrnLoader)->DeactivateIdleSubTerrains();
	}

	m_pA3DEngine->EndPerformanceRecord(A3DEngine::PERF_TICK_SCENEUPDATE);

	return true;
}

//	Render routine
bool A3DTerrain2::Render(A3DViewport* pViewport, bool bShadow, A3DRenderTarget* pTerrainColorRT,
						 float fZBias, float fShadowLum, bool bHDRLightMap, bool bOnePassHDRLM, bool bShadowMap)
{
	if (!m_bDataLoaded || !m_pA3DDevice || m_bNoRender)
		return true;

	ASSERT(m_pA3DEngine);
	m_pA3DEngine->BeginPerformanceRecord(A3DEngine::PERF_RENDER_SCENE);

	ASSERT(m_pCurRender);

	//	Do blocks cull
	DoBlocksCull(pViewport);

	//	render normal scene
	m_bUnderWaterRender = false;

	//	Do render
	if (!m_pCurRender->Render(pViewport, bShadow, pTerrainColorRT, fZBias, fShadowLum, bHDRLightMap, bOnePassHDRLM, bShadowMap))
	{
		m_pA3DEngine->EndPerformanceRecord(A3DEngine::PERF_RENDER_SCENE);
		return false;
	}

	m_bForceUpdate = false;

	m_pA3DEngine->EndPerformanceRecord(A3DEngine::PERF_RENDER_SCENE);

	return true;
}

bool A3DTerrain2::ZPrePass(A3DViewport* pViewport)
{
	if (!m_bDataLoaded || !m_pA3DDevice || m_bNoRender)
		return true;

	ASSERT(m_pA3DEngine);
	//m_pA3DEngine->BeginPerformanceRecord(A3DEngine::PERF_RENDER_SCENE);

	ASSERT(m_pCurRender);

	//	Do blocks cull
	DoBlocksCull(pViewport);

	//	render normal scene
	m_bUnderWaterRender = false;

	//	Do render
	if (!m_pCurRender->ZPrePass(pViewport))
	{
		//m_pA3DEngine->EndPerformanceRecord(A3DEngine::PERF_RENDER_SCENE);
		return false;
	}

	m_bForceUpdate = false;

	//m_pA3DEngine->EndPerformanceRecord(A3DEngine::PERF_RENDER_SCENE);

	return true;
}

bool A3DTerrain2::RenderForRefract(A3DViewport * pViewport, float vRefractSurfaceHeight)
{
	if (!m_bDataLoaded || !m_pA3DDevice || m_bNoRender)
		return true;

	ASSERT(m_pA3DEngine);
	m_pA3DEngine->BeginPerformanceRecord(A3DEngine::PERF_RENDER_SCENE);

	ASSERT(m_pCurRender);

	//	Do blocks cull
	DoBlocksCull(pViewport);
	
	m_bRenderForRefract = true;
	m_vRefractSurfaceHeight = vRefractSurfaceHeight;

	bool bFogTable = m_pA3DDevice->GetFogTableEnable();
	m_pA3DDevice->SetFogTableEnable(false);
	
	//	Do render
	if (!m_pCurRender->Render(pViewport))
	{
		m_bRenderForRefract = false;
		m_pA3DDevice->SetFogTableEnable(bFogTable);
		m_pA3DEngine->EndPerformanceRecord(A3DEngine::PERF_RENDER_SCENE);
		return false;
	}

	m_pA3DDevice->SetFogTableEnable(bFogTable);
	m_bRenderForRefract = false;
	m_bForceUpdate = false;

	m_pA3DEngine->EndPerformanceRecord(A3DEngine::PERF_RENDER_SCENE);
	return true;
}

//	Render under water scene
bool A3DTerrain2::RenderUnderWater(A3DViewport * pViewport, A3DTexture * pTexCaustic, float vCaustDU, float vCaustDV)
{
	if (!m_bDataLoaded || !m_pA3DDevice || m_bNoRender)
		return true;

	ASSERT(m_pA3DEngine);
	m_pA3DEngine->BeginPerformanceRecord(A3DEngine::PERF_RENDER_SCENE);

	ASSERT(m_pCurRender);

	//	Do blocks cull
	DoBlocksCull(pViewport);

	// render normal scene
	m_bUnderWaterRender = true;
	m_pTexCaustic = pTexCaustic;
	m_vCaustDU = vCaustDU;
	m_vCaustDV = vCaustDV;

	//	Do render
	if (!m_pCurRender->Render(pViewport))
		return false;

	m_bForceUpdate = false;

	m_pA3DEngine->EndPerformanceRecord(A3DEngine::PERF_RENDER_SCENE);
	return true;
}

//	Render raw meshes without any textures
bool A3DTerrain2::RenderRawMesh(A3DViewport * pViewport, bool bCullBlock, bool bApplyFVF)
{
	if (!m_bDataLoaded || !m_pA3DDevice || m_bNoRender)
		return true;

	ASSERT(m_pA3DEngine);
	m_pA3DEngine->BeginPerformanceRecord(A3DEngine::PERF_RENDER_SCENE);

	ASSERT(m_pCurRender);

	//	Do blocks cull
	if (bCullBlock)
		DoBlocksCull(pViewport);

	//	Do render
	if (!m_pCurRender->RenderRawMesh(pViewport, bApplyFVF))
		return false;

	m_bForceUpdate = false;

	m_pA3DEngine->EndPerformanceRecord(A3DEngine::PERF_RENDER_SCENE);
	return true;
}

//	Reset render information
void A3DTerrain2::ResetRenderInfo()
{
	//	Reset render information
	m_pCurRender->ResetRenderInfo();
}

//	Set external render. NULL means to use default render
void A3DTerrain2::SetExtRender(A3DTerrain2Render* pExtRender)
{
	if (pExtRender)
		m_pCurRender = pExtRender;
	else
		m_pCurRender = &m_TerrainRender;
}

//	Do blocks cull
void A3DTerrain2::DoBlocksCull(A3DViewport* pViewport)
{
	if (m_bNoRender)
		return;

	//	Reset render information
	ResetRenderInfo();

	//	Calculate visible area in blocks
	ARectI rcView;
	CalcAreaInBlocks(m_vLoadCenter, m_fViewRadius, rcView);

	if (!rcView.Width() || !rcView.Height())
	{
		//	This may occur if render directly without tick routine was called.
		//	This shouldn't happen.
		ASSERT(rcView.Width() && rcView.Height());
		return;
	}

	//	Do blocks culling
	if (m_pTrnCuller && m_pTrnCuller->PrepareToCull(pViewport))
		m_pTrnCuller->BlocksCull(pViewport, rcView);
	else
		DefaultBlocksCull(pViewport, rcView);
}

//	Default blocks culling routine
void A3DTerrain2::DefaultBlocksCull(A3DViewport* pViewport, const ARectI& rcView)
{
	int iBaseIdx = m_pCurActBlocks->GetBlockIndex(rcView.top, rcView.left);
	int iBlkPitch = m_pCurActBlocks->rcArea.Width();
	BlockArray& aBlocks = m_pCurActBlocks->aBlocks;

	m_iRdBlockCnt = 0;
    
    //	Set HC (horizon culling) weight for blocks
	for (int r=rcView.top; r < rcView.bottom; r++)
	{
		int iIndex = iBaseIdx;
		iBaseIdx += iBlkPitch;

		for (int c=rcView.left; c < rcView.right; c++, iIndex++)
		{
			A3DTerrain2Block* pBlock = aBlocks[iIndex];
			if (!pBlock || !pBlock->IsDataLoaded())
				continue;

			pBlock->SetVisibleFlag(false);

			if (pBlock->IsNoRender())
				continue;

			const A3DAABB& aabb = pBlock->GetBlockAABB();
			if (!pViewport->GetCamera()->AABBInViewFrustum(aabb.Mins, aabb.Maxs))
				continue;
            
			pBlock->SetVisibleFlag(true);
			m_pCurRender->RegisterBlock(pBlock);
			m_iRdBlockCnt++;
		}
	}	
}

//	Update block LOD info
void A3DTerrain2::UpdateBlocksLOD(const A3DVECTOR3& vCenter)
{
	if (m_LODType == LOD_NONE)
		return;

	int iBaseIdx = 0;
	const ARectI& rcView = m_pCurActBlocks->rcArea;
	int iBlkPitch = m_pCurActBlocks->rcArea.Width();
	BlockArray& aBlocks = m_pCurActBlocks->aBlocks;

	A3DVECTOR3 vDist;
	float fAverageHei = 0.0f;
	int r = 0;
	for (r=rcView.top; r < rcView.bottom; r++)
	{
		int iIndex = iBaseIdx;
		iBaseIdx += iBlkPitch;

		for (int c=rcView.left; c < rcView.right; c++, iIndex++)
		{
			A3DTerrain2Block* pBlock = aBlocks[iIndex];
			if (!pBlock || !pBlock->IsDataLoaded())
				continue;

			if (pBlock->IsNoRender())
			{
				pBlock->SetLODLevel(0);
				continue;
			}

			//	Add average height
			fAverageHei += pBlock->GetBlockAABB().Center.y;

			//	Set left neighbour of this block
			A3DTerrain2Block* pNeighbour;
			int n = c - 1;
			pNeighbour = n >= m_pCurActBlocks->rcArea.left ? aBlocks[iIndex-1] : NULL;
			pBlock->SetNeighbour(A3DTerrain2Block::NB_LEFT, pNeighbour);

			//	Set top neighbour of this block
			n = r - 1;
			pNeighbour = n >= m_pCurActBlocks->rcArea.top ? aBlocks[iIndex-iBlkPitch] : NULL;
			pBlock->SetNeighbour(A3DTerrain2Block::NB_TOP, pNeighbour);

			//	Set right neighbour of this block
			n = c + 1;
			pNeighbour = n < m_pCurActBlocks->rcArea.right ? aBlocks[iIndex+1] : NULL;
			pBlock->SetNeighbour(A3DTerrain2Block::NB_RIGHT, pNeighbour);

			//	Set bottom neighbour of this block
			n = r + 1;
			pNeighbour = n < m_pCurActBlocks->rcArea.bottom ? aBlocks[iIndex+iBlkPitch] : NULL;
			pBlock->SetNeighbour(A3DTerrain2Block::NB_BOTTOM, pNeighbour);

			//	Calculate LOD level for block without holes
			vDist = pBlock->GetBlockAABB().Center - vCenter;
			float fDist = vDist.MagnitudeH() - m_fBlockRadius;
			if (fDist < m_aLODDist[0] * pBlock->GetLODScale())
				pBlock->SetLODLevel(3);
			else if (fDist < m_aLODDist[1] * pBlock->GetLODScale())
				pBlock->SetLODLevel(2);
			else
				pBlock->SetLODLevel(1);
		}
	}

	if (m_LODType == LOD_USEHEIGHT)
	{
		int iDecLevel = 0;

		//	Distance between eye's height and average height
		fAverageHei /= m_iRdBlockCnt;
		float fDist2 = vCenter.y - fAverageHei;

		if (fDist2 > m_aLODDist[1])
			iDecLevel = 2;
		else if (fDist2 > m_aLODDist[0])
			iDecLevel = 1;

		if (iDecLevel)
		{
			for (r=0; r < aBlocks.GetSize(); r++)
			{
				A3DTerrain2Block* pBlock = aBlocks[r];
				if (!pBlock || !pBlock->IsDataLoaded() || pBlock->IsNoRender())
					continue;
			
				int iLevel = pBlock->GetLODLevel() - iDecLevel;
				pBlock->SetLODLevel(iLevel > 1 ? iLevel : 1);
			}
		}
	}

	//	Do last adjustment
	for (r=0; r < aBlocks.GetSize(); r++)
	{
		A3DTerrain2Block* pBlock = aBlocks[r];
		if (!pBlock || !pBlock->IsDataLoaded() || pBlock->IsNoRender())
			continue;

		if (pBlock->GetLODLevel() == 1)
			pBlock->AdjustLODLevel();
	}
}

//	Load all active blocks immediately
bool A3DTerrain2::LoadActiveBlocks(ACTBLOCKS& ActBlocks)
{
	int r, c;
	int iBaseIdx = ActBlocks.rcArea.top * m_iNumAllBlockCol + ActBlocks.rcArea.left;

	if (!m_bMultiThread)
	{
		for (r=ActBlocks.rcArea.top; r < ActBlocks.rcArea.bottom; r++)
		{
			int iIndex = iBaseIdx;

			for (c=ActBlocks.rcArea.left; c < ActBlocks.rcArea.right; c++, iIndex++)
			{
				A3DTerrain2Block* pBlock = LoadBlock(r, c, iIndex);
				ActBlocks.aBlocks.Add(pBlock);
			}

			iBaseIdx += m_iNumAllBlockCol;
		}
	}
	else
	{
		ActBlocks.aBlocks.SetSize(ActBlocks.rcArea.Width() * ActBlocks.rcArea.Height(), 10);
		for (r=0; r < ActBlocks.aBlocks.GetSize(); r++)
			ActBlocks.aBlocks[r] = NULL;

		for (r=ActBlocks.rcArea.top; r < ActBlocks.rcArea.bottom; r++)
		{
			int iIndex = iBaseIdx;

			for (c=ActBlocks.rcArea.left; c < ActBlocks.rcArea.right; c++, iIndex++)
				m_CandidateBlkList.AddTail(iIndex);

			iBaseIdx += m_iNumAllBlockCol;
		}
	}

	return true;
}

//	Load blocks from candidate list
bool A3DTerrain2::LoadCandidateBlocks()
{
	//	Number of block which will be loaded
	int iNumBlock = m_CandidateBlkList.GetCount();
	if (iNumBlock > m_iBlkLoadSpeed)
		iNumBlock = m_iBlkLoadSpeed;

	if (!iNumBlock)
		return true;

	for (int i=0; i < iNumBlock; i++)
	{
		int iIndex = m_CandidateBlkList.RemoveHead();

		//	Row and column in whole terrain
		int r = iIndex / m_iNumAllBlockCol;
		int c = iIndex - r * m_iNumAllBlockCol;

		//	Check whether candidate block is still in active area. We have
		//	to do this check because the Active area may change every frame.
		if (!m_pCurActBlocks->rcArea.PtInRect(c, r))
			continue;

		//	If camera move very fast and change it's direction frequently, 
		//	there is possible one candidate appears twice. 
		if (m_pCurActBlocks->GetBlock(r, c, false))
			continue;

		m_pCurActBlocks->SetBlock(r, c, LoadBlock(r, c, iIndex));
	}

	return true;
}

/*	Get a candidate block.

	iRow, iCol (out): block's position in whole world
	iBlock (out): block's index

	Note: this function should be called in main thread rather than in
		resource loading thread, because m_CandidateBlkList isn't thread safe.
*/
bool A3DTerrain2::GetNextCandidateBlock(int& iRow, int& iCol, int& iBlock)
{
	int iNumBlock = m_CandidateBlkList.GetCount();

	for (int i=0; i < iNumBlock; i++)
	{
		int iIndex = m_CandidateBlkList.RemoveHead();

		//	Row and column in whole terrain
		int r = iIndex / m_iNumAllBlockCol;
		int c = iIndex - r * m_iNumAllBlockCol;

		//	Check whether candidate block is still in active area. We have
		//	to do this check because the Active area may change every frame.
		if (!m_pCurActBlocks->rcArea.PtInRect(c, r))
			continue;

		//	If camera move very fast and change it's direction frequently, 
		//	there is possible one candidate appears twice. 
		if (m_pCurActBlocks->GetBlock(r, c, false))
			continue;

		iRow = r;
		iCol = c;
		iBlock = iIndex;
		return true;
	}

	return false;
}

/*	Load a block

	r, c: row and column of block in whole terrain
	iBlock: block index in whole terrain, if this parameter is -1, iBlock
		will be auto calculated
*/
A3DTerrain2Block* A3DTerrain2::LoadBlock(int r, int c, int iBlock)
{
	if (iBlock < 0)
		iBlock = r * m_iNumAllBlockCol + c;

	ASSERT(iBlock >= 0 && iBlock < m_iNumBlock);

	//	Create a A3DTerrain2Block object
	A3DTerrain2Block* pBlock = m_pTrnLoader->LoadBlock(r, c, iBlock);
	if (!pBlock)
		return NULL;

	//	Load mask which this block belongs to
	LoadMask(pBlock->GetMaskAreaIdx());

	return pBlock;
}

//	Unload all active blocks
void A3DTerrain2::UnloadActiveBlocks(ACTBLOCKS& ActBlocks)
{
	BlockArray& aBlocks = ActBlocks.aBlocks;

	if (!m_bMultiThread)
	{
		for (int i=0; i < aBlocks.GetSize(); i++)
		{
			A3DTerrain2Block* pBlock = aBlocks[i];
			if (pBlock)
				UnloadBlock(pBlock);
		}
	}
	else
	{
		EnterCriticalSection(&m_csUnloadBlk);

		for (int i=0; i < aBlocks.GetSize(); i++)
		{
			A3DTerrain2Block* pBlock = aBlocks[i];
			if (pBlock)
				m_UnldBlkList.AddTail(pBlock);
		}

		LeaveCriticalSection(&m_csUnloadBlk);
	}

	ActBlocks.aBlocks.RemoveAll(false);
	ActBlocks.rcArea.Clear();
}

//	Unload a block
void A3DTerrain2::UnloadBlock(A3DTerrain2Block* pBlock)
{
	ASSERT(pBlock);

	//	Unload mask which this block belongs to
	UnloadMask(pBlock->GetMaskAreaIdx());

	//	Cache, release this block
	m_pTrnLoader->UnloadBlock(pBlock);
}

//	Release all loaded block
void A3DTerrain2::ReleaseAllBlocks()
{
	//	Release all active blocks
	BlockArray& aBlocks = m_pCurActBlocks->aBlocks;

	for (int i=0; i < aBlocks.GetSize(); i++)
	{
		A3DTerrain2Block* pBlock = aBlocks[i];
		if (pBlock)
			m_pTrnLoader->UnloadBlock(pBlock);
	}
	
	m_ActBlocks1.aBlocks.RemoveAll();
	m_ActBlocks2.aBlocks.RemoveAll();

	//	Release all loaded blocks
	EnterCriticalSection(&m_csLoadBlk);

	ALISTPOSITION pos = m_LdBlkList.GetHeadPosition();
	while (pos)
	{
		A3DTerrain2Block* pBlock = m_LdBlkList.GetNext(pos);
		m_pTrnLoader->UnloadBlock(pBlock);
	}

	m_LdBlkList.RemoveAll();

	LeaveCriticalSection(&m_csLoadBlk);

	//	Release all blocks waiting to be released
	EnterCriticalSection(&m_csUnloadBlk);

	pos = m_UnldBlkList.GetHeadPosition();
	while (pos)
	{
		A3DTerrain2Block* pBlock = m_UnldBlkList.GetNext(pos);
		m_pTrnLoader->UnloadBlock(pBlock);
	}

	LeaveCriticalSection(&m_csUnloadBlk);
}

//	Set center position used to load terrain blocks, this function will update
//	all active blocks immediately
bool A3DTerrain2::SetLoadCenter(const A3DVECTOR3& vCenter)
{
	if (!m_bDataLoaded)
		return false;

/*	//	Update active blocks
	if (!UpdateActiveBlocks(vCenter))
		return false;

	if (!m_bMultiThread)
	{
		//	Magnify load speed so that all candidate blocks are loaded at once
		int iLoadSpeed = m_iBlkLoadSpeed;
		m_iBlkLoadSpeed = m_CandidateBlkList.GetCount();

		//	Load blocks from candidate list
		if (!LoadCandidateBlocks())
		{
			m_iBlkLoadSpeed = iLoadSpeed;
			return false;
		}

		m_iBlkLoadSpeed = iLoadSpeed;
	}
	else
	{
		//	Activate loaded blocks
		ActivateLoadedBlocks();
	}

	return true;
*/
	//	Unload current active blocks
	UnloadActiveBlocks(*m_pCurActBlocks);

	m_CandidateBlkList.RemoveAll();

	ARectI rcNew;
	CalcAreaInBlocks(vCenter, m_fActRadius, rcNew);
	m_vLoadCenter = vCenter;

	m_pCurActBlocks->rcArea = rcNew;
	bool bRet = LoadActiveBlocks(*m_pCurActBlocks);

	//	Update block LOD info
	UpdateBlocksLOD(vCenter);

	return bRet;
}

/*	Calculate area represented in blocks

	vCenter: center position.
	fRadius: radius
	rcArea (out): used to store area represented in blocks
*/
void A3DTerrain2::CalcAreaInBlocks(const A3DVECTOR3& vCenter, float fRadius, ARectI& rcArea)
{
	float fInvBlockSize = 1.0f / m_fBlockSize;
	int iRadiusInBlock = (int)(ceil(fRadius * fInvBlockSize) + 0.5f);
	int l, t, r, b;

	int imaxr = m_iNumAllBlockCol;
	int imaxb = m_iNumAllBlockRow;

	int cx = (int)((vCenter.x - m_rcTerrain.left) * fInvBlockSize);
	int cz = (int)(-(vCenter.z - m_rcTerrain.top) * fInvBlockSize);

	int d  = iRadiusInBlock * 2 + 1;

	l = cx - iRadiusInBlock;
	t = cz - iRadiusInBlock;
	r = l + d;
	b = t + d;

	if (l < 0)
	{
		l = 0;
		r = imaxr > d ? d : imaxr;
	}

	if (t < 0)
	{
		t = 0;
		b = imaxb > d ? d : imaxb;
	}

	if (r > imaxr) 
	{
		r = imaxr;
		l = imaxr > d ? imaxr - d : 0;
	}

	if (b > imaxb)
	{
		b = imaxb;
		t = imaxb > d ? imaxb - d : 0;
	}

	a_ClampFloor(l, 0);
	a_ClampFloor(t, 0);
	a_ClampRoof(r, imaxr);
	a_ClampRoof(b, imaxb);

	if (l > m_iNumAllBlockCol || t > m_iNumAllBlockRow ||
		r < 0 || b < 0 || l > r || t > b)
	{
		ASSERT(0);
		return;
	}

	rcArea.SetRect(l, t, r, b);
}

/*	Update active blocks

	vCenter: center position used to load terrain blocks
*/
bool A3DTerrain2::UpdateActiveBlocks(const A3DVECTOR3& vCenter)
{
	if (!m_bDataLoaded)
		return false;

	ARectI rcNew;
	CalcAreaInBlocks(vCenter, m_fActRadius, rcNew);
	m_vLoadCenter = vCenter;

	if (!m_pCurActBlocks->aBlocks.GetSize())
	{
		//	Create active block buffer
		m_pCurActBlocks->rcArea = rcNew;
		return LoadActiveBlocks(*m_pCurActBlocks);
	}

	if (rcNew == m_pCurActBlocks->rcArea)
	{
		//	Active block don't change
		return true;
	}

	//	Exchange currently active block buffer
	a_Swap(m_pCurActBlocks, m_pOldActBlocks);

	ASSERT(!m_pCurActBlocks->aBlocks.GetSize());
	m_pCurActBlocks->rcArea = rcNew;
	
	ARectI rcInter = rcNew & m_pOldActBlocks->rcArea;
	if (rcInter.IsEmpty())
	{
		//	The two rectangles havn't intersection
		UnloadActiveBlocks(*m_pOldActBlocks);
		return LoadActiveBlocks(*m_pCurActBlocks);
	}
	else
	{
		int iBaseIdx = rcNew.top * m_iNumAllBlockCol + rcNew.left;

		for (int r=rcNew.top; r < rcNew.bottom; r++)
		{
			int iIndex = iBaseIdx;

			for (int c=rcNew.left; c < rcNew.right; c++, iIndex++)
			{
				if (m_pOldActBlocks->rcArea.PtInRect(c, r))
					m_pCurActBlocks->aBlocks.Add(m_pOldActBlocks->GetBlock(r, c, true));
				else
				{
					//	Add block to candidate list
					m_CandidateBlkList.AddTail(iIndex);
					m_pCurActBlocks->aBlocks.Add(NULL);
				}
			}

			iBaseIdx += m_iNumAllBlockCol;
		}

		UnloadActiveBlocks(*m_pOldActBlocks);

		return true;
	}
}

/*	Load normal texture

	iIndex: index in texture index lump
*/
A3DTexture* A3DTerrain2::LoadTexture(int iIndex)
{
	if (m_bNoRender)
		return NULL;

	TEXTURE* pSlot = m_aTextures[iIndex];
	if (!pSlot->strFileName.GetLength())
		return NULL;

	if (!pSlot->pA3DTexture)
	{
		pSlot->iRefCount = 0;

		A3DTextureMan* pTexMan = m_pA3DEngine->GetA3DTextureMan();
		if (!pTexMan->LoadTextureFromFile(pSlot->strFileName, &pSlot->pA3DTexture))
			return NULL;
	}

	pSlot->iRefCount++;

	return pSlot->pA3DTexture;
}

/*	Release normal texture

	iIndex: index in texture index lump
*/
void A3DTerrain2::ReleaseTexture(int iIndex)
{
	if (m_bNoRender)
		return;

	TEXTURE* pSlot = m_aTextures[iIndex];
	if (!pSlot->pA3DTexture)
		return;

	ASSERT(pSlot->iRefCount > 0);
	if (!(--pSlot->iRefCount))
	{
		A3DTextureMan* pTexMan = m_pA3DEngine->GetA3DTextureMan();
		pTexMan->ReleaseTexture(pSlot->pA3DTexture);
		pSlot->pA3DTexture = NULL;
	}
}

//	Load specified mask area data
bool A3DTerrain2::LoadMask(int iMaskArea)
{
	if (m_bNoRender)
		return true;

	ASSERT(iMaskArea >= 0 && iMaskArea < m_iNumMaskArea);

	EnterCriticalSection(&m_csMaskSlot);

	MASKSLOT* pSlot = &m_aMaskSlots[iMaskArea];
	if (pSlot->pMask)
	{
		//	Mask has been laoded
		pSlot->dwRef++;
		LeaveCriticalSection(&m_csMaskSlot);
		return true;
	}

	LeaveCriticalSection(&m_csMaskSlot);

	//	Create new mask
	A3DTerrain2Mask* pMask = m_pTrnLoader->LoadMask(iMaskArea);
	if (!pMask)
	{
		g_A3DErrLog.Log("A3DTerrain2::LoadMask, Failed to load mask area !");
		return false;
	}

	//	For the reason of multi-threads, we have to check slot status again
	EnterCriticalSection(&m_csMaskSlot);

	if (pSlot->pMask)
	{
		pSlot->dwRef++;
		m_pTrnLoader->UnloadMask(pMask);
	}
	else
	{
		pSlot->pMask = pMask;	
		pSlot->dwRef = 1;
	}

	LeaveCriticalSection(&m_csMaskSlot);

	return true;
}

//	Unload specified mask area data
void A3DTerrain2::UnloadMask(int iMaskArea)
{
	if (m_bNoRender)
		return;

	ASSERT(iMaskArea >= 0 && iMaskArea < m_iNumMaskArea);

	ACSWrapper csw(&m_csMaskSlot);

	MASKSLOT* pSlot = &m_aMaskSlots[iMaskArea];
	if (!pSlot->pMask)
	{
		return;
	}

	ASSERT(pSlot > 0);
	pSlot->dwRef--;

	if (!pSlot->dwRef)
	{
		m_pTrnLoader->UnloadMask(pSlot->pMask);
		pSlot->pMask = NULL;
	}
}

//	Release all mask slots
void A3DTerrain2::ReleaseMaskSlots()
{
	if (!m_aMaskSlots)
		return;

	ACSWrapper csw(&m_csMaskSlot);

	for (int i=0; i < m_iNumMaskArea; i++)
	{
		MASKSLOT* pSlot = &m_aMaskSlots[i];
		if (pSlot->pMask)
			m_pTrnLoader->UnloadMask(pSlot->pMask);
	}

	delete [] m_aMaskSlots;
	m_aMaskSlots = NULL;
}

//	Set LOD distance
void A3DTerrain2::SetLODDist(float fLevel2, float fLevel1)
{
	ASSERT(fLevel2 > 0.0f && fLevel1 > 0.0f);

	if (fLevel1 < fLevel2)
		fLevel1 = fLevel2;

	m_aLODDist[0] = fLevel2;
	m_aLODDist[1] = fLevel1;
}

//	Set layer weight limit
void A3DTerrain2::SetMaxLayerWeight(int iWeight)
{
	m_iLayerWeight = iWeight;
	a_Clamp(m_iLayerWeight, 0, (int)MAX_LAYERWEIGHT);
}

/*	Get light map of specified position

	Return true for success, otherwise return false.

	vPos: specified position whose lightmap will be got.
	prcArea (out): store world area covered by this light map, this parameter
		can be NULL.

		left, right: min and max on x axis.
		bottom, top: min and max on z axis.
*/
bool A3DTerrain2::GetPosLightMap(const A3DVECTOR3& vPos, ARectF* prcArea, 
								A3DTexture** ppDayLM, A3DTexture** ppNightLM)
{
	*ppDayLM	= NULL;
	*ppNightLM	= NULL;

	if (m_pCurActBlocks->rcArea.IsEmpty())
		return false;
	
	//	Get grid this position is in
	float fInvGridSize = 1.0f / m_fGridSize;

	int gc = (int)((vPos.x - m_rcTerrain.left) * fInvGridSize);
	int gr = (int)((m_rcTerrain.top - vPos.z) * fInvGridSize);

	//	Get block this position is in
	int bc = gc / m_iBlockGrid;
	int br = gr / m_iBlockGrid;

	//	Ensure this block is in active area has been loaded
	A3DTerrain2Block* pBlock = NULL;
	if (!m_pCurActBlocks->rcArea.PtInRect(bc, br) ||
		!(pBlock = m_pCurActBlocks->GetBlock(br, bc, false)))
		return false;

	//	Get mask area this block belongs to
	A3DTerrain2Mask* pMask = GetMaskArea(pBlock->GetMaskAreaIdx());
	if (!pMask)
		return false;

	if (prcArea)
	{
		//	Calculate mask area's area
		float fMaskSize = m_iMaskGrid * m_fGridSize;
		prcArea->left	= m_rcTerrain.left + (gc / m_iMaskGrid) * fMaskSize;
		prcArea->top	= m_rcTerrain.top - (gr / m_iMaskGrid) * fMaskSize;
		prcArea->right	= prcArea->left + fMaskSize;
		prcArea->bottom	= prcArea->top - fMaskSize;
	}

	*ppDayLM	= pMask->GetLightMap(true);
	*ppNightLM	= pMask->GetLightMap(false);

	return true;
}

//	Activate loaded blocks to current active area
void A3DTerrain2::ActivateLoadedBlocks()
{
	ACSWrapper csw(&m_csLoadBlk);

	int iNumBlock = m_LdBlkList.GetCount();

	for (int i=0; i < iNumBlock; i++)
	{
		A3DTerrain2Block* pBlock = m_LdBlkList.RemoveHead();

		//	Row and column in whole terrain
		int r = pBlock->GetRowInTerrain();
		int c = pBlock->GetColInTerrain();

		//	Check whether candidate block is still in active area.	
		//	If camera move very fast and change it's direction frequently, 
		//	there is possible one candidate appears twice. 
		if (m_pCurActBlocks->rcArea.PtInRect(c, r) && 
			!m_pCurActBlocks->GetBlock(r, c, false))
		{
			m_pCurActBlocks->SetBlock(r, c, pBlock);
		}
		else
		{
			EnterCriticalSection(&m_csUnloadBlk);
			m_UnldBlkList.AddTail(pBlock);
			LeaveCriticalSection(&m_csUnloadBlk);
		}
	}
}

//	Block loading routine by loading thread
bool A3DTerrain2::ThreadLoadBlock(int r, int c, int iBlock)
{
	A3DTerrain2Block* pBlock = LoadBlock(r, c, iBlock);
	if (pBlock)
	{
		EnterCriticalSection(&m_csLoadBlk);
		m_LdBlkList.AddTail(pBlock);
		LeaveCriticalSection(&m_csLoadBlk);
		return true;
	}

	return false;
}

//	Block unloading routine used by other thread
void A3DTerrain2::ThreadUnloadBlock(A3DTerrain2Block* pBlock)
{
	UnloadBlock(pBlock);
}

//	Get next block which is waiting to be unloaded
A3DTerrain2Block* A3DTerrain2::GetNextBlockToBeUnloaded()
{
	ACSWrapper csw(&m_csUnloadBlk);

	A3DTerrain2Block* pBlock = NULL;
	if (m_UnldBlkList.GetCount())
		pBlock = m_UnldBlkList.RemoveHead();

	return pBlock;
}

//	Get the approximate memory size hold by terrain blocks
int A3DTerrain2::GetBlockDataSize()
{
	if (!m_pCurActBlocks)
		return 0;

	if (UseLightmapTech())
	{
		int iNumBlock = m_pCurActBlocks->rcArea.Width() * m_pCurActBlocks->rcArea.Height();
		int iBlockDataSize = (m_iBlockGrid+1) * (m_iBlockGrid+1) * sizeof (A3DTRN2VERTEX1);
		return iNumBlock * iBlockDataSize;
	}
	else
	{
		int iNumBlock = m_pCurActBlocks->rcArea.Width() * m_pCurActBlocks->rcArea.Height();
		int iBlockDataSize = (m_iBlockGrid+1) * (m_iBlockGrid+1) * (sizeof (A3DTRN2VERTEX2) + sizeof (A3DTerrain2Block::VERTEXEXTRA));
		return iNumBlock * iBlockDataSize;
	}
}

//	Compress float height map using incremental compression algorithm.
//	Size of buffer pointed by pDstBuf can be calculated by CalcCompressedSize()
//	pdwSize (out): compressed data size
bool A3DTerrain2::CompressHeightMap(const float* aSrcHeis, int iNumVert, void* pDstBuf,
								DWORD* pdwSize)
{
	if (!aSrcHeis || !pDstBuf)
	{
		ASSERT(aSrcHeis && pDstBuf);
		return false;
	}

	//	Get the minimum and maximum height
	float fMin = aSrcHeis[0];
	float fMax = aSrcHeis[0];
	int i;

	for (i=1; i < iNumVert; i++)
	{
		if (aSrcHeis[i] < fMin)	fMin = aSrcHeis[i];
		if (aSrcHeis[i] > fMax) fMax = aSrcHeis[i];
	}

	float fDelta = (fMin != fMax) ? 32767.0f / (fMax - fMin) : 0.0f;

	//	Write data header
	CH_DATAINFO* pi = (CH_DATAINFO*)pDstBuf;
	pi->byVer		= 1;
	pi->fMinHei		= fMin;
	pi->fMaxHei		= fMax;
	pi->iNumVert	= iNumVert;

	//	Write vertex's data
	int d, ld = 0;
	short* pDst = (short*)(pi+1);

	for (i=0; i < iNumVert; i++, pDst++)
	{
		d = (int)((aSrcHeis[i] - fMin) * fDelta + 0.5f);
		a_Clamp(d, -32768, 32767);
		*pDst = (short)(d - ld);
		ld = d;
	}

	if (pdwSize)
		*pdwSize = sizeof (CH_DATAINFO) + iNumVert * sizeof (short);

	return true;
}

//	Return size of decompressed data
DWORD A3DTerrain2::DecompressHeightMap(const void* pSrcData, float* aDstHeis)
{
	if (!pSrcData || !aDstHeis)
	{
		ASSERT(pSrcData && aDstHeis);
		return 0;
	}

	const CH_DATAINFO* pi = (const CH_DATAINFO*)pSrcData;
	float fDelta = (pi->fMaxHei - pi->fMinHei) / 32767.0f;
	int i, ld=0;

	const short* pSrc = (const short*)(pi+1);
	for (i=0; i < pi->iNumVert; i++, pSrc++)
	{
		int d = ld + *pSrc;
		aDstHeis[i] = pi->fMinHei + d * fDelta;
		ld = d;
	}

	return pi->iNumVert * sizeof (float);
}

bool A3DTerrain2::CompressNormalMap(const float* aSrcNormals, int iNumVert, void* pDstBuf,
								DWORD* pdwSize)
{
	if (!aSrcNormals || !pDstBuf)
	{
		ASSERT(aSrcNormals && pDstBuf);
		return false;
	}

	//	Write data header
	CN_DATAINFO* pi = (CN_DATAINFO*)pDstBuf;
	pi->byVer		= 1;
	pi->iNumVert	= iNumVert;

	BYTE* pDst = (BYTE*)(pi+1);
	const float* pSrc = aSrcNormals;

	for (int i=0; i < iNumVert; i++)
	{
		A3DVECTOR3 v(pSrc[0], pSrc[1], pSrc[2]);
		a3d_CompressDir(v, pDst[0], pDst[1]);

		pSrc += 3;
		pDst += 2;
	}

	if (pdwSize)
		*pdwSize = sizeof (CN_DATAINFO) + iNumVert * sizeof (BYTE) * 2;

	return true;
}

DWORD A3DTerrain2::DecompressNormalMap(const void* pSrcData, float* aDstNormals)
{
	if (!pSrcData || !aDstNormals)
	{
		ASSERT(pSrcData && aDstNormals);
		return 0;
	}

	const CN_DATAINFO* pi = (const CN_DATAINFO*)pSrcData;
	const BYTE* pSrc = (const BYTE*)(pi+1);
	float* pDst = aDstNormals;

	for (int i=0; i < pi->iNumVert; i++)
	{
		A3DVECTOR3 v = a3d_DecompressDir(pSrc[0], pSrc[1]);
		pDst[0] = v.x;
		pDst[1] = v.y;
		pDst[2] = v.z;

		pDst += 3;
		pSrc += 2;
	}

	return pi->iNumVert * sizeof (float) * 3;
}

bool A3DTerrain2::CompressColorMap(const DWORD* aSrcCols, int iNumVert, void* pDstBuf,
								DWORD* pdwSize)
{
	if (!aSrcCols || !pDstBuf)
	{
		ASSERT(aSrcCols && pDstBuf);
		return false;
	}

	//	Write data header
	CC_DATAINFO* pi = (CC_DATAINFO*)pDstBuf;
	pi->byVer		= 1;
	pi->iNumVert	= iNumVert;

	const BYTE* pSrc = (const BYTE*)aSrcCols;
	WORD* pDst = (WORD*)pDstBuf;

	for (int i=0; i < iNumVert; i++, pDst++, pSrc+=sizeof (DWORD))
	{
		WORD r = pSrc[2] >> 3;
		WORD g = pSrc[1] >> 2;
		WORD b = pSrc[0] >> 3;
		*pDst = (r << 11) | (g << 5) | b;
	}

	if (pdwSize)
		*pdwSize = iNumVert * sizeof (WORD);

	return true;
}

DWORD A3DTerrain2::DecompressColorMap(const void* pSrcData, DWORD* aDstCols)
{
	if (!pSrcData || !aDstCols)
	{
		ASSERT(pSrcData && aDstCols);
		return false;
	}

	const CC_DATAINFO* pi = (const CC_DATAINFO*)pSrcData;
	const WORD* pSrc = (const WORD*)(pi+1);
	DWORD* pDst = aDstCols;

	static const float fScale = 1.0f / 31.0f * 255.0f;

	for (int i=0; i < pi->iNumVert; i++, pDst++, pSrc++)
	{
		BYTE r = (BYTE)(((*pSrc & 0xf800) >> 11) * fScale);
		BYTE g = (BYTE)(((*pSrc & 0x07e0) >> 5) * fScale);
		BYTE b = (BYTE)(((*pSrc & 0x001f)) * fScale);
		*pDst = RGB(r, g, b);
	}

	return pi->iNumVert * sizeof (DWORD);
}

//	Set vertex light flag
void A3DTerrain2::UseVertexLight(bool bTrue)
{
	if (!m_bDataLoaded || m_bVertexLight == bTrue)
		return;

	m_bVertexLight = bTrue;

	//	Notify render that terrain data has been changed
	m_TerrainRender.OnTerrainLoaded();

	//	Force to update terrain data
	SetLoadCenter(m_vLoadCenter);

	//	Force to update 
	SetForceUpdateFlag(true);
}

//	Create temporary buffers
bool A3DTerrain2::CreateTempBufs()
{
	int iNumVert = (m_iBlockGrid + 1) * (m_iBlockGrid + 1);

	if (!(m_TempBufs.aTempHei = new float [iNumVert]))
	{
		g_A3DErrLog.Log("A3DTerrain2::CreateTempBufs, Not enough memory !");
		return false;
	}

	if (!(m_TempBufs.aTempNormal = new float [iNumVert * 3]))
	{
		g_A3DErrLog.Log("A3DTerrain2::CreateTempBufs, Not enough memory !");
		return false;
	}

	if (!(m_TempBufs.aTempDiff = new DWORD [iNumVert]))
	{
		g_A3DErrLog.Log("A3DTerrain2::CreateTempBufs, Not enough memory !");
		return false;
	}

	if (!(m_TempBufs.aTempDiff1 = new DWORD [iNumVert]))
	{
		g_A3DErrLog.Log("A3DTerrain2::CreateTempBufs, Not enough memory !");
		return false;
	}

	m_TempBufs.iNumVert = iNumVert;

	return true;
}

//	Release temporary buffers
void A3DTerrain2::ReleaseTempBufs()
{
	if (m_TempBufs.aTempHei)
		delete [] m_TempBufs.aTempHei;

	if (m_TempBufs.aTempNormal)
		delete [] m_TempBufs.aTempNormal;

	if (m_TempBufs.aTempDiff)
		delete [] m_TempBufs.aTempDiff;

	if (m_TempBufs.aTempDiff1)
		delete [] m_TempBufs.aTempDiff1;

	memset(&m_TempBufs, 0, sizeof (m_TempBufs));
}

//	Attach terrain culler
bool A3DTerrain2::AttachTerrainCuller(A3DTerrain2Cull* pTrnCuller)
{
	if (pTrnCuller && !pTrnCuller->AttachTerrain(this))
	{
		g_A3DErrLog.Log("A3DTerrain2::AttachTerrainCuller, Failed to attach culler !");
		return false;
	}

	m_pTrnCuller = pTrnCuller;
	return true;
}
