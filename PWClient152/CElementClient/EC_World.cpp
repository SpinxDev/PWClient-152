/*
 * FILE: EC_World.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/8/25
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "EC_Global.h"
#include "EC_World.h"
#include "EC_Viewport.h"
#include "EC_ManPlayer.h"
#include "EC_ManNPC.h"
#include "EC_ManMatter.h"
#include "EC_ManOrnament.h"
#include "EC_ManDecal.h"
#include "EC_ManSkillGfx.h"
#include "EC_ManAttacks.h"
#include "EC_MsgDef.h"
#include "EC_Game.h"
#include "EC_HostPlayer.h"
#include "EC_CDS.h"
#include "EC_ShadowRender.h"
#include "EC_GameRun.h"
#include "EC_Scene.h"
#include "EC_SunMoon.h"
#include "EC_Configs.h"
#include "EC_Matter.h"
#include "EC_UIManager.h"
#include "EC_Utility.h"
#include "EC_SceneLoader.h"
#include "EC_GameUIMan.h"
#include "EC_GameSession.h"
#include "EL_Tree.h"
#include "EL_Forest.h"
#include "EL_GrassLand.h"
#include "EL_GrassType.h"
#include "EL_Precinct.h"
#include "EL_Region.h"
#include "EL_BackMusic.h"
#include "EL_CloudManager.h"
#include "EC_Faction.h"
#include "EC_Instance.h"
#include "EC_TriangleMan.h"
#include "EC_BrushMan.h"
#include "AutoScene.h"
#include "EC_InputCtrl.h"
#include "EC_SceneLights.h"
#include "EC_AssureMove.h"
#include "EC_Pet.h"
#include "EC_PetCorral.h"
#include "EC_GPDataType.h"
#include "EC_IntelligentRoute.h"
#include "EC_UIConfigs.h"
#include "EC_FullGlowRender.h"

#include "ELTerrainOutline2.h"

#include "HomeTerrain.h"		// Added by JDL, 2006.7.11
#include "AutoSceneConfig.h"
#include "AutoHomeMan.h"

#include "globaldataman.h"

#include "A3D.h"
#include <A3DTerrain2CullHC.h>
#include "AFI.h"
#include "A3DGFXExMan.h"

#include "EC_RandomMapPreProcessor.h"
#include "EC_HPWorkForceNavigate.h"
#include "EC_PlayerClone.h"

#include <A3DTerrain2.h>

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

static void WorldRenderForReflected(A3DViewport * pViewport, void * pArg);
static void WorldRenderForRefracted(A3DViewport * pViewport, void * pArg);
static void WorldRenderOnSky(A3DViewport * pViewport, void * pArg);


///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECWorld
//	
///////////////////////////////////////////////////////////////////////////

CECWorld::CECWorld(CECGameRun* pGameRun)
{
	m_pGameRun			= pGameRun;
// 	m_pAutoScene		= NULL;
	m_pA3DTerrain		= NULL;
	m_pA3DTerrainWater	= NULL;
	m_pA3DTrnCuller		= NULL;
	m_pA3DSky			= NULL;
	m_bWorldLoaded		= false;
	m_pCDS				= NULL;
	m_pForest			= NULL;
	m_pGrassLand		= NULL;
	m_pTerrainOutline	= NULL;
	m_pCloudManager		= NULL;
	m_pScene			= NULL;
	m_pSunMoon			= NULL;
	m_pRain				= NULL;
	m_pSnow				= NULL;
	m_bResetEnv			= true;
	m_pPrecinctSet		= NULL;
	m_pCurPrecinct		= NULL;
	m_pRegionSet		= NULL;
	m_pCurRegion		= NULL;
	m_pCurPetRegion		= NULL;
	m_pSceneLights		= NULL;
	m_pAssureMove		= NULL;
	m_bCenterReset		= false;
	m_dwBornStamp		= 1;
	m_idInst			= 0;
	m_iParallelWorldID				= 0;

	m_fTrnLoadDelta		= 0.0f;
	m_fTrnLoadCnt		= 0.0f;
	m_fScnLoadDelta		= 0.0f;
	m_fScnLoadCnt		= 0.0f;

// 	m_bAutoSceneHostMode= false;

	memset(m_aManagers, 0, sizeof (m_aManagers));
}

CECWorld::~CECWorld()
{
}

//	Initialize object
bool CECWorld::Init(int idInst, int iParallelWorldID)
{
	m_idInst = idInst;
	m_iParallelWorldID = iParallelWorldID;
	if (m_worlds.empty() && iParallelWorldID > 0){
		m_worlds.push_back(ParallelWorldInfo(iParallelWorldID, 0));
	}

	//	Create managers
	if (!CreateManagers())
	{
		a_LogOutput(1, "CECWorld::Init, Failed to create managers");
		return false;
	}

	//	Create CDS object
	if (!(m_pCDS = new CECCDS))
	{
		glb_ErrorOutput(ECERR_NOTENOUGHMEMORY, "CECWorld::Init", __LINE__);
		return false;
	}

	g_pGame->GetA3DEngine()->SetA3DCDS(m_pCDS);

	// Init nature objects
	if( !InitNatureObjects() )
	{
		a_LogOutput(1, "CECWorld::Init, Failed to init nature objects");
		return false;
	}

	//	Create precinct set
	if (!(m_pPrecinctSet = new CELPrecinctSet))
	{
		glb_ErrorOutput(ECERR_NOTENOUGHMEMORY, "CECWorld::Init", __LINE__);
		return false;
	}

	//	Create region set
	if (!(m_pRegionSet = new CELRegionSet))
	{
		glb_ErrorOutput(ECERR_NOTENOUGHMEMORY, "CECWorld::Init", __LINE__);
		return false;
	}

	//	Create scene light object
	if (!(m_pSceneLights = new CECSceneLights) || !m_pSceneLights->Init(g_pGame->GetA3DDevice()))
	{
		glb_ErrorOutput(ECERR_NOTENOUGHMEMORY, "CECWorld::Init", __LINE__);
		return false;
	}

	CECIntelligentRoute::Instance().ChangeWorldInstance(idInst);

	//	Create assure move object
	m_pAssureMove = new CECAssureMove();

	// set shadow render flags once.
	g_pGame->GetShadowRender()->SetCastShadowFlag(g_pGame->GetConfigs()->GetSystemSettings().bShadow);
	return true;
}

//	Initialize terrain water
bool CECWorld::InitTerrainWater()
{
	//	Load terrain
	if (!(m_pA3DTerrainWater = new A3DTerrainWater))
	{
		glb_ErrorOutput(ECERR_NOTENOUGHMEMORY, "CECWorld::InitTerrainWater", __LINE__);
		return false;
	}
	
	bool bInitWater = false;

	if(IsRandomMap(m_idInst))
		bInitWater = m_pA3DTerrainWater->Init(g_pGame->GetA3DDevice(), g_pGame->GetViewport()->GetA3DViewport(), (A3DCamera*)(g_pGame->GetViewport()->GetA3DCamera()));
	else
		bInitWater = m_pA3DTerrainWater->InitStatically(g_pGame->GetA3DDevice(), g_pGame->GetViewport()->GetA3DViewport(), (A3DCamera*)(g_pGame->GetViewport()->GetA3DCamera()));
	
	if(!bInitWater)
	{
		glb_ErrorOutput(ECERR_FAILEDTOCALL, "CECWorld::InitTerrainWater", __LINE__);
		return false;
	}

	// set water effect once.
	m_pA3DTerrainWater->SetSimpleWaterFlag(!g_pGame->GetConfigs()->GetSystemSettings().nWaterEffect);
	m_pA3DTerrainWater->SetExpensiveWaterFlag(g_pGame->GetConfigs()->GetSystemSettings().bAdvancedWater);
	m_pA3DTerrainWater->SetLightSun(g_pGame->GetDirLight());
	return true;
}

//	Initialize plants objects
bool CECWorld::InitPlantsObjects()
{
	//	Load Forest here;
	m_pForest = new CELForest();
	if (!m_pForest->Init(g_pGame->GetA3DDevice(), g_pGame->GetDirLight(), &g_Log))
	{
		glb_ErrorOutput(ECERR_FAILEDTOCALL, "CECWorld::InitPlantsObjects", __LINE__);
		return false;
	}

	m_pGrassLand = new CELGrassLand();
	if (!m_pGrassLand->Init(g_pGame->GetA3DDevice(), m_pA3DTerrain, &g_Log))
	{
		glb_ErrorOutput(ECERR_FAILEDTOCALL, "CECWorld::InitPlantsObjects", __LINE__);
		return false;
	}
	
	return true;
}

//	Initialize nature effects
bool CECWorld::InitNatureObjects()
{
	//	now create some nature effects here
	m_pSunMoon = new CECSunMoon();
	if( !m_pSunMoon->Init() )
	{
		glb_ErrorOutput(ECERR_NOTENOUGHMEMORY, "CECWorld::InitNatureObjects", __LINE__);
		return false;
	}

	struct tm serverLocal = g_pGame->GetServerLocalTime();
	int nTimeInDay = serverLocal.tm_hour * 3600 + serverLocal.tm_min * 60 + serverLocal.tm_sec;
	m_pSunMoon->SetTimeOfTheDay(nTimeInDay / (4.0f * 3600.0f));

	m_pRain = new A3DRain();
	if( !m_pRain->Init(g_pGame->GetA3DDevice(), 1.0f) )
	{
		glb_ErrorOutput(ECERR_NOTENOUGHMEMORY, "CECWorld::InitNatureObjects", __LINE__);
		return false;
	}
	m_pRain->SetHostCamera(g_pGame->GetViewport()->GetA3DCamera());
	m_pRain->SetEmitterWidth(20.0f);
	m_pRain->SetEmitterLength(20.0f);
	m_pRain->SetEmitterHeight(8.0f);
	m_pRain->SetUseRateMode(20, true);
	m_pRain->SetSize(0.04f);
	m_pRain->SetSizeVariation(30.0f);
	m_pRain->SetSpeed(0.5f);
	m_pRain->SetSpeedVariation(50.0f);
	m_pRain->SetInheritInfluence(0.0f);
	m_pRain->SetLife(20);
	m_pRain->SetTextureMap("rain.tga");
	m_pRain->SetSrcBlend(A3DBLEND_SRCALPHA);
	m_pRain->SetDestBlend(A3DBLEND_INVSRCALPHA);
	m_pRain->CreateRain();
	m_pRain->SetColorNum(1);
	m_pRain->SetColor(0, A3DCOLORRGBA(255, 255, 255, 160));
	//m_pRain->StartRain();

	m_pSnow = new A3DSnow();
	if( !m_pSnow->Init(g_pGame->GetA3DDevice(), 1.8f) )
	{
		glb_ErrorOutput(ECERR_NOTENOUGHMEMORY, "CECWorld::InitNatureObjects", __LINE__);
		return false;
	}

	m_pSnow->SetHostCamera(g_pGame->GetViewport()->GetA3DCamera());
	m_pSnow->SetEmitterWidth(80.0f);
	m_pSnow->SetEmitterLength(80.0f);
	m_pSnow->SetEmitterHeight(40.0f);
	m_pSnow->SetUseRateMode(20, true);
	m_pSnow->SetSize(0.3f);
	m_pSnow->SetSizeVariation(20.0f);
	m_pSnow->SetSpeed(0.5f);
	m_pSnow->SetSpeedVariation(20.0f);
	m_pSnow->SetLife(100);
	m_pSnow->SetTextureMap("snow.bmp");
	m_pSnow->SetSrcBlend(A3DBLEND_ONE);
	m_pSnow->SetDestBlend(A3DBLEND_INVSRCCOLOR);
	m_pSnow->SetSpinTime(100);
	m_pSnow->SetSpinTimeVariation(50);
	m_pSnow->CreateSnow();
	m_pSnow->SetColor(0, A3DCOLORRGBA(160, 160, 160, 160));
	//m_pSnow->StartSnow();

	return true;
}

void CECWorld::ReleaseNatureObjects()
{
	if( m_pSunMoon )
	{
		m_pSunMoon->Release();
		delete m_pSunMoon;
		m_pSunMoon = NULL;
	}

	if( m_pRain )
	{
		m_pRain->Release();
		delete m_pRain;
		m_pRain = NULL;
	}

	if( m_pSnow )
	{
		m_pSnow->Release();
		delete m_pSnow;
		m_pSnow = NULL;
	}
}

// Render nature objects
bool CECWorld::RenderNatureObjects(CECViewport* pViewport)
{
	A3DVECTOR3 vecCamPos = pViewport->GetA3DViewport()->GetCamera()->GetPos();
	bool bCamBelowWater = m_pA3DTerrainWater->IsUnderWater(vecCamPos);
	// see if camera is under water, if so, just return without rendering these particles.
	if( bCamBelowWater )
		return true;

	if( m_pRain )
		m_pRain->RenderParticles(pViewport->GetA3DViewport());

	if( m_pSnow )
		m_pSnow->RenderParticles(pViewport->GetA3DViewport());
	
	return true;
}

// Tick nature objects
bool CECWorld::TickNatureObjects(DWORD dwDeltaTime)
{
	if( m_pSunMoon )
	{
		if( m_idInst == 0 )
		{
			// login world, must always be at midnoon
			m_pSunMoon->Tick(0);
		}
		else
			m_pSunMoon->Tick(dwDeltaTime);
	}

	static DWORD dwOldTicks = 0;
	dwOldTicks += dwDeltaTime;
	while(dwOldTicks > 33)
	{
		if( m_pRain )
			m_pRain->TickEmitting();

		if( m_pSnow )
			m_pSnow->TickEmitting();

		dwOldTicks -= 33;
	}

	return true;
}

//	Release object
void CECWorld::Release()
{
	// Release auto home
// 	ReleaseAutoHome();
	CECIntelligentRoute::Instance().Release();

	//	Release CDS object
	if (m_pCDS)
	{
		g_pGame->GetA3DEngine()->SetA3DCDS(NULL);
		delete m_pCDS;
		m_pCDS = NULL;
	}

	// Release nature objects
	ReleaseNatureObjects();

	//	Release scene before managers
	ReleaseScene();

	//	Release managers
	ReleaseManagers();

	// force to release all loaded resource
	ThreadRemoveAllLoaded();

	if (m_pPrecinctSet)
	{
		delete m_pPrecinctSet;
		m_pPrecinctSet = NULL;
	}

	if (m_pRegionSet)
	{
		delete m_pRegionSet;
		m_pRegionSet = NULL;
	}

	if (m_pSceneLights)
	{
		delete m_pSceneLights;
		m_pSceneLights = NULL;
	}

	if (m_pAssureMove)
	{
		m_pAssureMove->ReleaseMap();
		delete m_pAssureMove;
		m_pAssureMove = NULL;
	}	

	m_dwBornStamp = 1;
}

//	Load scene
bool CECWorld::LoadWorld(const char* szFile, const A3DVECTOR3& vInitLoadPos)
{
	if (m_bWorldLoaded)
	{
		//	Scene couldn't be loaded twice
		a_LogOutput(1, "CECWorld::LoadWorld, scene couldn't be loaded twice");
		return false;
	}

	CECGameRun* pGameRun = g_pGame->GetGameRun();

	m_vInitLoadPos = vInitLoadPos;

	af_GetFilePath(szFile, m_strMapPath);

	//	Load terrain data
	const char* pExt = strrchr(szFile, '.');
	AString strResFile(szFile, pExt - szFile);
	strResFile += ".trn2";

	CECRandomMapProcess* randMap = g_pGame->GetGameRun()->GetRandomMapProc();
	if (randMap)
		randMap->ClearMapName();
	
	char szRandomMap[MAX_PATH];
	if (g_pGame->GetGameRun()->GetGameState() == CECGameRun::GS_GAME && IsRandomMap(m_idInst)) // 如果是随机地图
	{	
		CECInstance* pInst = g_pGame->GetGameRun()->GetInstance(GetInstanceID());
		const char* mapName = pInst->GetPath();
		if (randMap)		
			randMap->DoProcess(mapName);
		else {
		//	ASSERT(FALSE);
			a_LogOutput(1,"CECWorld::LoadWorld, randommap process object cannot be created!");
			return false;
		}
		
		strResFile.Format("temp\\maps\\%s\\%s.trn2",randMap->GetMapName(),mapName);

		sprintf(szRandomMap,"temp\\maps\\%s\\%s.ecwld",randMap->GetMapName(),mapName);

		szFile = szRandomMap;
		if (!glb_FileExist(strResFile) || !glb_FileExist(szFile)){
			ASSERT(FALSE);
			return false;
		}
		pExt = strrchr(szFile, '.');			
	}

	if (!LoadTerrain(strResFile))
	{
		glb_ErrorOutput(ECERR_FILEOPERATION, "CECWorld::LoadWorld", __LINE__);
		return false;
	}

	char szMapName[MAX_PATH];
	af_GetRelativePathNoBase(m_strMapPath, "maps\\", szMapName);

	bool bNeedAssureMove = (stricmp(szMapName, "world") != 0 && stricmp(szMapName, "a21") != 0 && stricmp(szMapName, "a22") != 0) ? true : false;

	//	Load assure move map
	float fWidth = m_pA3DTerrain->GetBlockColNum() * m_pA3DTerrain->GetBlockSize();
	float fHeight = m_pA3DTerrain->GetBlockRowNum() * m_pA3DTerrain->GetBlockSize();
	A3DVECTOR3 vecMapOrigin = A3DVECTOR3(-fWidth / 2.0f, 0.0f, fHeight / 2.0f);
	if( m_pAssureMove && bNeedAssureMove )
	{
		m_pAssureMove->LoadMap(szFile, vecMapOrigin, fWidth, fHeight);
		m_pAssureMove->StepLoadMap(0);
	}

	pGameRun->StepLoadProgress(2);

	//	Initialize terrain water
	if (!InitTerrainWater())
	{
		a_LogOutput(1, "CECWorld::LoadWorld, failed to initalize terrain water!");
		return false;
	}

	if( m_pAssureMove && bNeedAssureMove )
		m_pAssureMove->StepLoadMap(1);
	pGameRun->StepLoadProgress(2);

	//	Initialize plants objects
	if (!InitPlantsObjects())
	{
		a_LogOutput(1, "CECWorld::LoadWorld, failed to initalize plants objects!");
		return false;
	}

	if( m_pAssureMove && bNeedAssureMove )
		m_pAssureMove->StepLoadMap(2);
	pGameRun->StepLoadProgress(2);

	//	Load scene objects
	if (!LoadSceneObjects(szFile))
	{
		glb_ErrorOutput(ECERR_FILEOPERATION, "CECWorld::LoadWorld", __LINE__);
		return false;
	}
	char navigateFile[MAX_PATH];
	sprintf(navigateFile,"%s\\navigate.dat",m_strMapPath);
	if (!m_pScene->LoadBezierNavigate(navigateFile, 0.0f, 0.0f))	
		a_LogOutput(1, "CECWorld::LoadSceneObjects, Failed to load scene file %s", navigateFile);
	

	if( m_pAssureMove && bNeedAssureMove )
		m_pAssureMove->StepLoadMap(3);
	pGameRun->StepLoadProgress(1);

	//	Load sky
	if (!LoadSky())
	{
		glb_ErrorOutput(ECERR_FILEOPERATION, "CECWorld::LoadWorld", __LINE__);
		return false;
	}

	if( m_pAssureMove && bNeedAssureMove )
		m_pAssureMove->StepLoadMap(4);

	pGameRun->StepLoadProgress(1);

	//	Load terrain's outline data
	char szTilePath[MAX_PATH];
	sprintf(szTilePath, "loddata\\%s\\olm\\", szMapName);

	// we will always use 1024 size birdviews to avoid seams if DXT1 is supported
	// or else, we use 256 size texture and the outline texture will smooth texture by copy adjacent edges together
	char szTexPath[MAX_PATH];
	HRESULT hval;
	hval = g_pGame->GetA3DDevice()->GetA3DEngine()->GetD3D()->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, (D3DFORMAT)g_pGame->GetA3DDevice()->GetDevFormat().fmtAdapter, 0, D3DRTYPE_TEXTURE, (D3DFORMAT) A3DFMT_DXT1);
	if( D3D_OK == hval )
		sprintf(szTexPath, "loddata\\%s\\birdviews\\1024\\", szMapName);
	else
		sprintf(szTexPath, "loddata\\%s\\birdviews\\256\\", szMapName);
	
	if (!LoadTerrainOutline(szTilePath, szTexPath))
		glb_ErrorOutput(ECERR_FILEOPERATION, "CECWorld::LoadWorld", __LINE__);
	
	//	Load clouds here
	strResFile = AString(szFile, pExt - szFile);
	strResFile += ".cld";
	strcat(szTexPath, "textures\\cumulus01.tga");
	if (!LoadClouds(strResFile, szTexPath) )
		glb_ErrorOutput(ECERR_FILEOPERATION, "CECWorld::LoadWorld", __LINE__);
	
	if( m_pAssureMove && bNeedAssureMove )
		m_pAssureMove->StepLoadMap(5);
	pGameRun->StepLoadProgress(2);

	//	Load precincts
	strResFile = m_strMapPath;
	strResFile += "\\precinct.clt";
	if (glb_FileExist(strResFile) && !m_pPrecinctSet->Load(strResFile))
		glb_ErrorOutput(ECERR_FILEOPERATION, "CECWorld::LoadWorld", __LINE__);

	//	Load regions
	strResFile = m_strMapPath;
	strResFile += "\\region.clt";
	if (glb_FileExist(strResFile) && !m_pRegionSet->Load(strResFile))
		glb_ErrorOutput(ECERR_FILEOPERATION, "CECWorld::LoadWorld", __LINE__);
	
	// 初始化 precinct 和 target对应关系	
	m_RegionWayPointsMap.clear();
	const abase::vector<TRANS_TARGET>& transtarget = *globaldata_gettranstargets();
	for (unsigned int j=0;j<transtarget.size();j++){
		const TRANS_TARGET& target = transtarget[j];
		if(GetInstanceID() != target.world_id) continue;
		CELRegion* region = m_pRegionSet->IsPointInRegion(target.vecPos.x,target.vecPos.z);
		int index = GetReginIndex(region);
		if(index >=0){
			m_RegionWayPointsMap[index].push_back(target.id);				
		}			
	}

	strResFile = m_strMapPath;
	strResFile += "\\scenelights.dat";
	if (glb_FileExist(strResFile) && !m_pSceneLights->Load(strResFile))
		glb_ErrorOutput(ECERR_FILEOPERATION, "CECWorld::LoadWorld", __LINE__);

	//	Set load center
	SetLoadCenter(vInitLoadPos);

	//	Notify managers
	for (int i=0; i < NUM_MANAGER; i++)
	{
		if (m_aManagers[i])
			m_aManagers[i]->OnEnterGameWorld();
	}

	//	If IsLoadThreadReady() == true, all resources have been loaded in
	//	SetLoadCenter();
	if (!IsLoadThreadReady())
		LoadInMainThread(true);

#ifdef USING_BRUSH_MAN
	GetOrnamentMan()->GetBrushMan()->Build(vInitLoadPos, true);
#endif

	if( m_pAssureMove && bNeedAssureMove )
		m_pAssureMove->StepLoadMap(6);

	m_bWorldLoaded = true;

	// now according to shadow render flag, we decide whether use triangle manager
	if( g_pGame->GetShadowRender() && g_pGame->GetShadowRender()->GetCastShadowFlag() )
		GetOrnamentMan()->LoadTriangleMan(vInitLoadPos);
	
	if (pGameRun->GetGameState() == CECGameRun::GS_GAME)
	{
		A3DVECTOR3 vPos = vInitLoadPos;
		vPos.y = GetTerrainHeight(vInitLoadPos);
		g_pGame->GetViewport()->GetA3DCamera()->SetPos(vPos + A3DVECTOR3(0.0f, 1.6f, 0.0f));

		InitLoaderThread();
		StartLoaderThread();
	}
	else
	{
		// login world always in day
		m_pSunMoon->SetTimeOfTheDay(0.5f);
	}

	return true;
}

//	Enter auto home
// void CECWorld::EnterAutoHome()
// {
// 	if (m_pAutoScene)
// 		return;
// 
// 	ReleaseScene();
// 	CreateAutoScene();
// }

//	Create auto home
// bool CECWorld::CreateAutoScene()
// {
// 	// Edited by JDL. 2006.7.11
// 	m_pAutoScene = new CAutoScene(this, g_pGame->GetA3DEngine());
// 
// 	AString strFile = "Configs\\AutoFamilyConfigs\\AutoScene.cfg";
// 	if (!m_pAutoScene->Init(strFile))
// 	{
// 		A3DRELEASE(m_pAutoScene);
// 		return false;
// 	}
// 	// Set home terrain to ecworld
// 	ASSERT(!m_pA3DTerrain);
// 	m_pA3DTerrain = m_pAutoScene->GetHomeTerrain();
// 
// 	// Create sky
// 	ASSERT(!m_pA3DSky);
// 	m_pA3DSky = new A3DSkySphere();
// 	CECConfigs* pConfig = g_pGame->GetConfigs();
// 	if (!m_pA3DSky->Init(g_pGame->GetA3DDevice(), NULL, pConfig->m_strDefSkyFile0, 
// 					pConfig->m_strDefSkyFile1, pConfig->m_strDefSkyFile2))
// 	{
// 		glb_ErrorOutput(ECERR_FILEOPERATION, "CECWorld::CreateAutoScene", __LINE__);
// 		return false;
// 	}
// 	if (m_pA3DSky)
// 		g_pGame->GetA3DEngine()->SetSky(m_pA3DSky);
// 	// Create forest
// 	ASSERT(!m_pForest);
// 	m_pForest = new CELForest();
// 	if( !m_pForest->Init(g_pGame->GetA3DDevice(), g_pGame->GetDirLight(), &g_Log) )
// 	{
// 		glb_ErrorOutput(ECERR_FILEOPERATION, "CECWorld::CreateAutoScene", __LINE__);
// 		return false;
// 	}
// 	// Create grass
// 	ASSERT(!m_pGrassLand);
// 	m_pGrassLand = new CELGrassLand();
// 	if( !m_pGrassLand->Init(g_pGame->GetA3DDevice(), m_pA3DTerrain, &g_Log) )
// 	{
// 		glb_ErrorOutput(ECERR_FILEOPERATION, "CECWorld::CreateAutoScene", __LINE__);
// 		return false;
// 	}
// 	// Create water
// 	ASSERT(!m_pA3DTerrainWater);
// 	m_pA3DTerrainWater = new A3DTerrainWater();
// 	if (!m_pA3DTerrainWater->Init(g_pGame->GetA3DDevice(), g_pGame->GetViewport()->GetA3DViewport(), (A3DCamera*)(g_pGame->GetViewport()->GetA3DCamera())))
// 	{
// 		glb_ErrorOutput(ECERR_FAILEDTOCALL, "CECWorld::CreateAutoScene", __LINE__);
// 		return false;
// 	}
// 	// set water effect once.
// 	m_pA3DTerrainWater->SetSimpleWaterFlag(!g_pGame->GetConfigs()->GetSystemSettings().nWaterEffect);
// 	m_pA3DTerrainWater->SetExpensiveWaterFlag(g_pGame->GetConfigs()->GetSystemSettings().bAdvancedWater);
// 	m_pA3DTerrainWater->SetLightSun(g_pGame->GetDirLight());
// 
// 	A3DCameraBase* pCamera = g_pGame->GetViewport()->GetA3DCamera();
// 
// 	float vHeight = 300.0f;
// 	if( GetTerrain() )
// 		vHeight = GetTerrain()->GetPosHeight(A3DVECTOR3(0.0f, 0.0f, 0.0f));
// 	pCamera->SetPos(A3DVECTOR3(0.0, vHeight + 2.0f, 0.0));
// 	pCamera->SetDirAndUp(A3DVECTOR3(0.0, 0.0, 1.0), A3DVECTOR3(0.0, 1.0, 0.0));
// 	m_vHostPrevPos = g_pGame->GetGameRun()->GetHostPlayer()->GetPos();
// 
// 	// Test. Set edit home
// 	Tick(0, g_pGame->GetViewport());
// 	m_pAutoScene->GetAutoHomeMan()->SetHome(3, 3);
// 
// 	return true;
// }

//	Release auto home
// void CECWorld::ReleaseAutoHome()
// {
// 	if (m_pAutoScene)
// 	{
// 		m_pAutoScene->Release();
// 		delete m_pAutoScene;
// 		m_pAutoScene = NULL;
// 
// 		LeaveAutoScentHostMode();
// 		if( g_pGame->GetGameRun()->GetHostPlayer() )
// 			g_pGame->GetGameRun()->GetHostPlayer()->SetPos(m_vHostPrevPos);
// 	}
// }
// 
// //	Enter host mode
// void CECWorld::EnterAutoSceneHostMode()
// {
// 	if (!m_pAutoScene || m_bAutoSceneHostMode)
// 		return;
// 
// 	m_bAutoSceneHostMode = true;
// 
// 	A3DCameraBase* pCamera = g_pGame->GetViewport()->GetA3DCamera();
// 	CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
// 	pHost->GetMoveControl().SetLocalMoveFlag(true);
// 	pHost->SetPos(pCamera->GetPos() + pCamera->GetDir() * 5.0f);
// 	pHost->SetDirAndUp(-pCamera->GetDir(), A3DVECTOR3(0, 1.0f, 0));
// 
// 	g_pGame->GetGameRun()->ActivateDefInput();
// }
// 
// //	Leave host mode
// void CECWorld::LeaveAutoScentHostMode()
// {
// 	if (!m_bAutoSceneHostMode)
// 		return;
// 
// 	m_bAutoSceneHostMode = false;
// 
// 	CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
// 
// 	if (pHost)
// 		pHost->GetMoveControl().SetLocalMoveFlag(false);
// 
// 	g_pGame->GetGameRun()->ActivateAutoHomeInput();
// }

//	Release current scene
void CECWorld::ReleaseScene()
{
	g_pGame->GetA3DEngine()->SetSky(NULL);

	A3DRELEASE(m_pScene);
	A3DRELEASE(m_pGrassLand);
	A3DRELEASE(m_pForest);
	A3DRELEASE(m_pA3DSky);

	// 1. force to exit loader thread
	ExitLoaderThread();

	// 2. release manager
	for (int i=0; i < NUM_MANAGER; i++)
	{
		if (m_aManagers[i])
			m_aManagers[i]->OnLeaveGameWorld();
	}

	// 3. force to release all loaded resource
	ThreadRemoveAllLoaded();

	//	Release terrain after loading thread has been ended
	A3DRELEASE(m_pA3DTrnCuller);
	A3DRELEASE(m_pA3DTerrainWater);
	A3DRELEASE(m_pA3DTerrain);
	A3DRELEASE(m_pTerrainOutline);
	A3DRELEASE(m_pCloudManager);
	
	m_bWorldLoaded = false;

	// 删除随机地图信息
	CECRandomMapProcess::DeleteAllRandomMapDataForSingleUser();
}

//	Load terrain
bool CECWorld::LoadTerrain(const char* szFile)
{
	//	Load terrain
	if (!(m_pA3DTerrain = new A3DTerrain2))
	{
		glb_ErrorOutput(ECERR_NOTENOUGHMEMORY, "CECWorld::LoadTerrain", __LINE__);
		return false;
	}
	// 随机地图限制视野为最小
	float fRadius = IsRandomMap() ? g_pGame->GetConfigs()->GetSevActiveRadius() : g_pGame->GetConfigs()->GetSceneLoadRadius();
	if (!m_pA3DTerrain->Init(g_pGame->GetA3DEngine(), fRadius, fRadius))
	{
		glb_ErrorOutput(ECERR_FAILEDTOCALL, "CECWorld::LoadTerrain", __LINE__);
		return false;
	}

//	bool bMultiThread = g_pGame->GetGameRun()->GetGameState() == CECGameRun::GS_GAME ? true : false;
	bool bMultiThread = true;
	if (!m_pA3DTerrain->Load(szFile, 0.0f, 0.0f, bMultiThread))
	{
		a_LogOutput(1, "CECWorld::LoadTerrain, Failed to load A3DTerrain2.");
		return false;
	}

	CECConfigs* pConfigs = g_pGame->GetConfigs();

	m_pA3DTerrain->SetBlockLoadSpeed(4);
	//m_pA3DTerrain->SetMaxLayerWeight(pConfigs->GetSystemSettings().nGroundDetail);
	m_pA3DTerrain->SetLODType(A3DTerrain2::LOD_NOHEIGHT);
	m_pA3DTerrain->SetLODDist(pConfigs->GetTerrainLODDist1(), pConfigs->GetTerrainLODDist2());

	//	Create terrain culler
	if ((m_pA3DTrnCuller = new A3DTerrain2CullHC))
	{
		//	Try to load horizon culling data
		char szHCFile[MAX_PATH];
		strcpy(szHCFile, szFile);
		af_ChangeFileExt(szHCFile, MAX_PATH, ".t2hc");

		if (m_pA3DTrnCuller->Init(szHCFile))
		{
			m_pA3DTerrain->AttachTerrainCuller(m_pA3DTrnCuller);
		}
		else
		{
			A3DRELEASE(m_pA3DTrnCuller);
		//	a_LogOutput(1, "CECWorld::LoadTerrain, Failed to create horizon cull object");
		}
	}
	
	return true;
}

//	Load terrain outline
bool CECWorld::LoadTerrainOutline(const char* szTilePath, const char * szTexPath)
{
	CECInstance * pInstance = g_pGame->GetGameRun()->GetInstance(m_idInst);

	int w = 1, h = 1;
	if( pInstance )
	{
		w = pInstance->GetColNum();
		h = pInstance->GetRowNum();
	}

	m_pTerrainOutline = new CELTerrainOutline2();
	if( !m_pTerrainOutline->Init(g_pGame->GetA3DDevice(), szTexPath, szTilePath, m_pA3DTerrain, w, h) )
	{
		delete m_pTerrainOutline;
		m_pTerrainOutline = NULL;
		return false;
	}

	switch( g_pGame->GetConfigs()->GetSystemSettings().nSight + 1 )
	{
	case 1:
		m_pTerrainOutline->SetEndDist(0, 400.0f);
		m_pTerrainOutline->SetEndDist(1, 500.0f);
		m_pTerrainOutline->SetEndDist(2, 600.0f);
		break;

	case 2:
		m_pTerrainOutline->SetEndDist(0, 800.0f);
		m_pTerrainOutline->SetEndDist(1, 900.0f);
		m_pTerrainOutline->SetEndDist(2, 1000.0f);
		break;

	case 3:	
		m_pTerrainOutline->SetEndDist(0, 800.0f);
		m_pTerrainOutline->SetEndDist(1, 900.0f);
		m_pTerrainOutline->SetEndDist(2, 1000.0f);
		break;

	case 4:
		m_pTerrainOutline->SetEndDist(0, 800.0f);
		m_pTerrainOutline->SetEndDist(1, 1200.0f);
		m_pTerrainOutline->SetEndDist(2, 1400.0f);
		break;

	case 5:
		m_pTerrainOutline->SetEndDist(0, 800.0f);
		m_pTerrainOutline->SetEndDist(1, 1200.0f);
		m_pTerrainOutline->SetEndDist(2, 1900.0f);
		break;
	}

	if( g_pGame->GetConfigs()->GetSystemSettings().bSimpleTerrain )
	{
		m_pTerrainOutline->SetReplaceTerrain(true);
	}
	else
	{
		m_pTerrainOutline->SetReplaceTerrain(false);
	}

	m_pTerrainOutline->SetViewScheme(g_pGame->GetConfigs()->GetSystemSettings().nSight);
	return true;
}

//	Load clouds
bool CECWorld::LoadClouds(const char * szCloudFile, const char * szTexPath)
{
	m_pCloudManager = new CELCloudManager();

	if( !m_pCloudManager->Init(g_pGame->GetA3DDevice(), szCloudFile) )
	{
		m_pCloudManager->Release();
		delete m_pCloudManager;
		m_pCloudManager = NULL;
		return false;
	}

	return true;	
}

//	Load sky
bool CECWorld::LoadSky()
{
	//	Sphere type sky
	A3DSkySphere* pSky = new A3DSkySphere;
	if (!pSky)
		return false;

	//	Sky texture file name shouldn't be set here
	CECConfigs* pConfig = g_pGame->GetConfigs();
	if (!pSky->Init(g_pGame->GetA3DDevice(), NULL, pConfig->m_strDefSkyFile0, 
					pConfig->m_strDefSkyFile1, pConfig->m_strDefSkyFile2))
	{
		a_LogOutput(1, "CECWorld::LoadSky, Failed to create sphere sky !");
		return false;
	}

	pSky->SetFlySpeedU(0.003f);
	pSky->SetFlySpeedV(0.003f);

	m_pA3DSky = pSky;
	
	//	This enable sky can do animation when A3DEngine::TickAnimation is called
	if (m_pA3DSky)
		g_pGame->GetA3DEngine()->SetSky(m_pA3DSky);

	return true;
}

bool CECWorld::LoadPlants(const char * szFile)
{
	//	Load Forest here;
	m_pForest = new CELForest();
	if( !m_pForest->Init(g_pGame->GetA3DDevice(), g_pGame->GetDirLight(), &g_Log) )
	{
		glb_ErrorOutput(ECERR_FILEOPERATION, "CECWorld::LoadPlants", __LINE__);
		return false;
	}
	// temp code, we should get tree type's information and call pForest->AddTreeType
	// before we can use it.
	if( !m_pForest->LoadTypesFromConfigFile("trees\\trees.cfg") )
		return false;

	m_pGrassLand = new CELGrassLand();
	if( !m_pGrassLand->Init(g_pGame->GetA3DDevice(), m_pA3DTerrain, &g_Log) )
	{
		glb_ErrorOutput(ECERR_FILEOPERATION, "CECWorld::LoadPlants", __LINE__);
		return false;
	}

	// test code, load from a file.
	m_pGrassLand->Load("maps\\world\\world.gld");
	return true;
}

//	Load scene objects
bool CECWorld::LoadSceneObjects(const char* szFile)
{
	if (!(m_pScene = new CECScene(this)))
	{
		glb_ErrorOutput(ECERR_NOTENOUGHMEMORY, "CECWorld::LoadSceneObjects", __LINE__);
		return false;
	}

	if (!m_pScene->Load(szFile, 0.0f, 0.0f))
	{
		a_LogOutput(1, "CECWorld::LoadSceneObjects, Failed to load scene file %s", szFile);
		return false;
	}

	float fRadius = g_pGame->GetConfigs()->GetSceneLoadRadius();
	m_pScene->SetActRadius(2000.0f);//fRadius);
	m_pScene->SetBlockLoadSpeed(2);

	return true;
}

//	Set load center
void CECWorld::SetLoadCenter(const A3DVECTOR3& vCenter)
{
	if (IsLoadThreadReady())
	{
		SuspendLoadThread();
		ThreadRemoveAllUnNeeded();
	}

	if (m_pA3DTerrain)
		m_pA3DTerrain->SetLoadCenter(vCenter);

	if (m_pScene)
		m_pScene->SetLoadCenter(vCenter);

	if (IsLoadThreadReady())
	{
		LoadInMainThread(true);

#ifdef USING_BRUSH_MAN
		GetOrnamentMan()->GetBrushMan()->Build(vCenter, true);
#endif

#ifdef USING_TRIANGLE_MAN
		GetOrnamentMan()->GetTriangleMan()->Build(vCenter, true);
#endif

		StartLoaderThread();
	}

	if( m_pGrassLand )
	{
		// we want grassland to build all active grasses when we enter the game 
		m_pGrassLand->SetProgressLoad(false);
		m_pGrassLand->Update(vCenter, 0);
		m_pGrassLand->SetProgressLoad(true);
	}

	g_pGame->GetViewport()->GetA3DCamera()->SetPos(vCenter + A3DVECTOR3(0.0f, 2.0f, 0.0f));

	m_bResetEnv = true;
	m_bCenterReset = true;

	//	Force to update objects in mini-map
	GetPlayerMan()->OnLoadCenterChanged();
	GetNPCMan()->OnLoadCenterChanged();
	GetMatterMan()->OnLoadCenterChanged();

	//	Discard current frame
	g_pGame->DiscardFrame();
}

//	Set view radius
void CECWorld::SetViewRadius(float fRadius)
{
	CECConfigs* pConfigs = g_pGame->GetConfigs();

	a_ClampFloor(fRadius, pConfigs->GetSevActiveRadius());

	CECHostPlayer* pPlayer = GetHostPlayer();
	if (pPlayer && m_pA3DTerrain && m_pScene)
	{
		//	View radius should be equal to active radius, otherwise there
		//	will be block missing at the border between terrain and outline
		m_pA3DTerrain->SetActRadius(fRadius/* + 64.0f */);
		m_pA3DTerrain->SetViewRadius(fRadius);
		m_pScene->SetActRadius(2000.0f);//fRadius);

		SetLoadCenter(pPlayer->GetPos());
	}
}

//	Create managers
bool CECWorld::CreateManagers()
{
	CECPlayerMan* pPlayerMan = new CECPlayerMan(m_pGameRun);
	m_aManagers[MAN_PLAYER] = pPlayerMan;

	CECNPCMan* pNPCMan = new CECNPCMan(m_pGameRun);
	m_aManagers[MAN_NPC] = pNPCMan;

	CECMatterMan* pMatterMan = new CECMatterMan(m_pGameRun);
	m_aManagers[MAN_MATTER] = pMatterMan;

	CECOrnamentMan* pOnmtMan = new CECOrnamentMan(m_pGameRun);
	m_aManagers[MAN_ORNAMENT] = pOnmtMan;

	CECDecalMan* pDecalMan = new CECDecalMan(m_pGameRun);
	m_aManagers[MAN_DECAL] = pDecalMan;

	CECSkillGfxMan* pSkillGfxMan = new CECSkillGfxMan(m_pGameRun);
	m_aManagers[MAN_SKILLGFX] = pSkillGfxMan;

	CECAttacksMan* pAttacksMan = new CECAttacksMan(m_pGameRun);
	m_aManagers[MAN_ATTACKS] = pAttacksMan;
	
	if (pPlayerMan){
		g_pGame->GetFactionMan()->RegisterObserver(pPlayerMan);
	}
	if (pNPCMan){
		g_pGame->GetFactionMan()->RegisterObserver(pNPCMan);
	}

	return true;
}

//	Release managers
void CECWorld::ReleaseManagers()
{
	if (CECPlayerMan *pPlayerMan = GetPlayerMan()){
		g_pGame->GetFactionMan()->UnregisterObserver(pPlayerMan);
	}
	if (CECNPCMan *pNPCMan = GetNPCMan()){
		g_pGame->GetFactionMan()->UnregisterObserver(pNPCMan);
	}
	for (int i=0; i < NUM_MANAGER; i++)
	{
		CECManager* pManager = m_aManagers[i];
		if (pManager)
		{
			pManager->Release();
			delete pManager;
		}

		m_aManagers[i] = NULL;
	}
}

//	Calculate terrain and scene loading speed
void CECWorld::CalcSceneLoadSpeed()
{
	CECHostPlayer* pHost = GetHostPlayer();

	//	If world center is just reset, skip calculation
	if (m_bCenterReset || !pHost || !pHost->IsAllResReady())
		return;

	float fAvgRate = g_pGame->GetAverageFrameRate();
	float fSpeedH = pHost->GetMoveControl().GetAverHoriSpeed();
	if (fabs(fSpeedH) > 30.0f)
		return;

	//	Now, we use multi-thread to load terrain data, so needn't
	//	handle it any more
/*	if (m_pA3DTerrain)
	{
		int iWaitBlock = m_pA3DTerrain->GetCandidateBlockNum();
		if (iWaitBlock)
		{
			//	time to pass a block in second
			float fPassTime = 100.0f;
			if (fSpeedH)
			{
				fPassTime = m_pA3DTerrain->GetBlockSize() * 0.9f / fSpeedH;
				a_ClampRoof(fPassTime, 100.0f);
			}

			//	Frame number to pass a block
			float fPassFrame = fPassTime * fAvgRate;
			m_fTrnLoadDelta = iWaitBlock / fPassFrame;
		}

		m_fTrnLoadCnt += m_fTrnLoadDelta;
		m_pA3DTerrain->SetBlockLoadSpeed((int)m_fTrnLoadCnt);
		m_fTrnLoadCnt -= (int)m_fTrnLoadCnt;
	}
*/
	if (m_pScene)
	{
		int iWaitBlock = m_pScene->GetCandidateBlockNum();
		if (iWaitBlock)
		{
			//	time to pass a block in second
			float fPassTime = 100.0f;
			if (fSpeedH)
			{
				fPassTime = m_pScene->GetBlockSize() * 0.8f / fSpeedH;
				a_ClampRoof(fPassTime, 100.0f);
			}

			//	Frame number to pass a block
			float fPassFrame = fPassTime * fAvgRate;
			m_fScnLoadDelta = iWaitBlock / fPassFrame;
		}

		m_fScnLoadCnt += m_fScnLoadDelta;
		m_pScene->SetBlockLoadSpeed((int)m_fScnLoadCnt);
		m_fScnLoadCnt -= (int)m_fScnLoadCnt;
	}
}

//	Tick routine
bool CECWorld::Tick(DWORD dwDeltaTime, CECViewport* pViewport)
{
	//	test code ...
//	g_pGame->GetA3DEngine()->GetA3DSkinMan()->SetWorldCenter(pViewport->GetA3DCamera()->GetPos());
//	g_pGame->GetA3DEngine()->GetA3DSkinMan()->SetWorldCenter(g_vOrigin);

/*	CECHostPlayer* pHost = GetHostPlayer();
	bool bHostReady = pHost && pHost->HostIsReady();

	//	Tick host separately
	CECPlayerMan* pPlayerMan = GetPlayerMan();
	if (pPlayerMan && bHostReady)
		pPlayerMan->TickHostPlayer(dwDeltaTime, pViewport);
*/
	for (int i=0; i < NUM_MANAGER; i++)
	{
		if (m_aManagers[i])
			m_aManagers[i]->Tick(dwDeltaTime);
	}

	//	Calculate terrain and block loading speed
	CalcSceneLoadSpeed();

	A3DVECTOR3 vHostPos = GetHostPosForClientTick(pViewport);
	A3DCamera* pCamera = pViewport->GetA3DCamera();

	if (m_pA3DTerrain)
	{
		// Edited by JDL 2006.7.11
// 		if (m_pAutoScene && !m_bAutoSceneHostMode)
// 			m_pA3DTerrain->Update(dwDeltaTime, pCamera, pCamera->GetPos());
// 		else

			m_pA3DTerrain->Update(dwDeltaTime, pCamera, vHostPos);
		
		if (m_pTerrainOutline)
			m_pTerrainOutline->Update(pCamera, vHostPos);

		if (m_pGameRun->GetGameState() == CECGameRun::GS_GAME &&
			m_pA3DTerrain->IsMultiThreadLoad())
		{
			int r, c, iBlock;
			while (m_pA3DTerrain->GetNextCandidateBlock(r, c, iBlock))
			{
				QueueTerrainBlock(r, c, iBlock);
			}
		}
	}
	
	if (m_pCloudManager)
	{
		A3DVECTOR3 vecLightDir = Normalize(A3DVECTOR3(-1.0f, -1.0f, -1.0f));
		if( m_pSunMoon )
			vecLightDir = m_pSunMoon->GetLightDir();

		m_pCloudManager->Tick(dwDeltaTime, pCamera->GetPos() - vecLightDir * 10000.0f);
	}

	if (m_pScene)
		m_pScene->Update(dwDeltaTime, pCamera, vHostPos);

	if (m_pSceneLights)
		m_pSceneLights->Update(dwDeltaTime, m_pSunMoon->GetDNFactor(), vHostPos);

	if (m_pA3DTerrainWater)
		m_pA3DTerrainWater->Update(dwDeltaTime);

	if (m_pForest)
		m_pForest->Update(dwDeltaTime);

	if (m_pGrassLand && g_pGame->GetConfigs()->m_bShowGrassLand)
	{
		m_pGrassLand->SetProgressLoad(true);
		m_pGrassLand->Update(vHostPos, dwDeltaTime);
	}

	TickNatureObjects(dwDeltaTime);

	if (m_pGameRun->GetGameState() == CECGameRun::GS_GAME)
	{
		//	Update current precinct
		UpdateCurPrecinct();

		//	Update current region
		UpdateCurRegion();
		
		UpdateMusicForRandomMap();
	}

	m_bCenterReset = false;

	LoadInMainThread(false);

// 	if (m_pAutoScene)
// 		m_pAutoScene->Update(dwDeltaTime, pCamera, pCamera->GetPos());

	g_pGame->GetConfigs()->ApplyWorldSpecificSetting();
	
	return true;
}

//	Tick animation
bool CECWorld::TickAnimation()
{
	for (int i=0; i < NUM_MANAGER; i++)
	{
		if (m_aManagers[i])
			m_aManagers[i]->TickAnimation();
	}

	return true;
}

//	Render routine
bool CECWorld::Render(CECViewport* pViewport)
{
// 	if (m_pAutoScene)
// 	{
// 		CAutoSceneConfig* pAutoSceneConfig = m_pAutoScene->GetAutoSceneConfig();
// 		A3DEngine* pA3DEngine = g_pGame->GetA3DEngine();
// 		A3DViewport* pA3DViewport = pViewport->GetA3DViewport();
// 		bool bUnderWater = m_pAutoScene->IsUnderWater(pViewport->GetA3DCamera()->GetPos());
// 
// 		// Clear background by fog color
// 		float nf = m_pAutoScene->GetDayWeight();
// 		float df = 1.0f - nf;
// 		A3DCOLOR fogColor = 0xff000000;
// 		A3DCOLORVALUE colDay = A3DCOLORVALUE(pAutoSceneConfig->GetSunColorDay());
// 		A3DCOLORVALUE colNight = A3DCOLORVALUE(pAutoSceneConfig->GetSunColorNight());
// 		if( bUnderWater )
// 			fogColor = pAutoSceneConfig->GetFogColorWater();
// 		else
// 			fogColor = (colDay * df + colNight * nf).ToRGBAColor();
// 		g_pGame->GetA3DDevice()->Clear(D3DCLEAR_TARGET, fogColor & 0x00ffffff, 1.0f, 0);
// 
// 		A3DSkinRender* psr1 = g_pGame->GetSkinRender1();
// 		g_pGame->GetA3DEngine()->GetA3DSkinMan()->SetCurSkinRender(psr1);
// 
// 		// Render sky
// 		if (m_pA3DSky)
// 		{
// 			m_pA3DSky->SetCamera(pViewport->GetA3DCamera());
// 
// 			if( !bUnderWater )
// 				m_pA3DSky->Render();
// 		}
// 
// 		// Render terrain
// 		if (m_pA3DTerrain)
// 		{
// 			m_pA3DTerrain->SetDNFactor(nf);
// 			if (!bUnderWater)
// 				m_pA3DTerrain->Render(pA3DViewport);
// 			else
// 				m_pA3DTerrain->RenderUnderWater(pA3DViewport, m_pA3DTerrainWater->GetCurCaustTexture(),
// 									m_pA3DTerrainWater->GetCaustDU(), m_pA3DTerrainWater->GetCaustDV());
// 		}
// 
// 		// Render forest
// 		if (m_pForest && !bUnderWater)
// 		{
// 			m_pForest->Render(pA3DViewport);
// 		}
// 
// 		if (m_bAutoSceneHostMode)
// 		{
// 			CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
// 
// 			if (pHost)
// 			{
// 				if (pHost->IsHangerOn())
// 					pHost->GetFaceModel()->Tick(0);
// 
// 				pHost->Render(pViewport);
// 			}
// 		}
// 
// 		g_pGame->GetA3DDevice()->SetAlphaBlendEnable(false);
// 		g_pGame->GetA3DDevice()->SetAlphaTestEnable(true);
// 		g_pGame->GetA3DDevice()->SetAlphaFunction(A3DCMP_GREATER);
// 		g_pGame->GetA3DDevice()->SetAlphaRef(84);
// 
// 		// Render models
// 		for (int i=0; i < NUM_MANAGER-1; i++)
// 		{
// 			if (m_aManagers[i])
// 				m_aManagers[i]->Render(pViewport);
// 		}
// 
// 		g_pGame->GetA3DDevice()->SetAlphaBlendEnable(false);
// 		g_pGame->GetA3DDevice()->SetAlphaTestEnable(true);
// 
// 		g_pGame->GetA3DEngine()->GetA3DLitModelRender()->SetDNFactor(m_pSunMoon->GetDNFactor());
// 		g_pGame->GetA3DEngine()->GetA3DLitModelRender()->Render(pA3DViewport, false);
// 		g_pGame->GetA3DEngine()->GetA3DLitModelRender()->ResetRenderInfo(false);
// 		
// 		//	Render non alpha skin models
// 		psr1->Render(pA3DViewport, false);
// 		psr1->ResetRenderInfo(false);
// 
// 		// Render grass
// 		if (m_pGrassLand)
// 		{
// 			m_pGrassLand->SetDNFactor(nf);
// 			m_pGrassLand->Render(pA3DViewport, false);
// 		}
// 
// 		// now render shadows here
// 		RenderShadows(pViewport);
// 
// 		pA3DEngine->FlushCachedAlphaMesh(pA3DViewport);
// 		g_pGame->GetA3DDevice()->SetAlphaTestEnable(false);
// 		g_pGame->GetA3DDevice()->SetAlphaBlendEnable(true);
// 
// 		g_pGame->GetA3DDevice()->SetTextureColorOP(1, A3DTOP_DISABLE);
// 		g_pGame->GetA3DDevice()->SetTextureAlphaOP(1, A3DTOP_DISABLE);
// 		g_pGame->GetA3DDevice()->ClearTexture(1);
// 
// 		// Render water
// 		A3DVECTOR3 vSunDir = pAutoSceneConfig->GetSunDir();
// 		if (m_pA3DTerrainWater)
// 		{
// 			m_pA3DTerrainWater->SetDNFactor(nf);
// 			if (!bUnderWater)
// 			{							   
// 				m_pA3DTerrainWater->RenderReflect(vSunDir, pA3DViewport, m_pA3DTerrain, m_pA3DSky, NULL, WorldRenderForReflected, WorldRenderForRefracted, this, WorldRenderOnSky, this, true, m_pA3DTerrain);
// 			}
// 			else
// 				m_pA3DTerrainWater->RenderRefract(vSunDir, pA3DViewport, m_pA3DTerrain, m_pA3DSky, NULL, WorldRenderForRefracted, this, WorldRenderOnSky, this, true);
// 		}
// 
// 		g_pGame->GetA3DEngine()->GetA3DLitModelRender()->RenderAlpha(pA3DViewport);
// 		g_pGame->GetA3DEngine()->GetA3DLitModelRender()->ResetRenderInfo(true);
// 
// 		//	Render alpha skin models
// 		// alpha skin mesh dont set z
// 		psr1->RenderAlpha(pA3DViewport);
// 		psr1->ResetRenderInfo(true);
// 		
// 		// Render grasses
// 		if (m_pGrassLand)
// 		{
// 			m_pGrassLand->SetDNFactor(nf);
// 			m_pGrassLand->Render(pA3DViewport, true);
// 		}
// 
// 		m_pAutoScene->Render(pA3DViewport);
// 		
// 		return true;
// 	}

	A3DEngine* pA3DEngine = g_pGame->GetA3DEngine();
	A3DViewport* pA3DViewport = pViewport->GetA3DViewport();
	CECHostPlayer* pHost = GetHostPlayer();
	CECConfigs* pConfigs = g_pGame->GetConfigs();

	//	test code...
	if (m_pGameRun->GetGameState() == CECGameRun::GS_GAME && (!pHost || !pHost->HostIsReady()))
		return true;

	if( pHost && pHost->IsHangerOn() )
	{
		if (!pHost->IsChangingFace())
			pHost->UpdateFollowCamera(pHost->IsPlayerMoving(), 0);

		// we may readjust the camera in above call, so just reactivate the camera.
		A3DCameraBase* pCamera = pA3DViewport->GetCamera();
		A3DCoordinate* pCameraCoord = pHost->GetCameraCoord();
		pCamera->SetPos(pCameraCoord->GetPos());
		pCamera->SetDirAndUp(pCameraCoord->GetDir(), pCameraCoord->GetUp());
		pCamera->Active();
	}
	m_bResetEnv = !m_pScene->AdjustEnvironment(g_pGame->GetTickTime(), pViewport->GetA3DCamera(), m_bResetEnv);

	float nf = m_pSunMoon->GetDNFactor();
	float df = 1.0f - nf;
	A3DCOLOR fogColor = 0xff000000;
	if( m_pScene->m_Env.bUnderWater )
		fogColor = (m_pScene->m_Env.UWFogCol.cur * df + m_pScene->m_Env.UWFogCol_n.cur * nf).ToRGBAColor();
	else
		fogColor = (m_pScene->m_Env.FogCol.cur * df + m_pScene->m_Env.FogCol_n.cur * nf).ToRGBAColor();
	g_pGame->GetA3DDevice()->Clear(D3DCLEAR_TARGET, fogColor & 0x00ffffff, 1.0f, 0);

	A3DSkinRender* psr1 = g_pGame->GetSkinRender1();
	pA3DEngine->GetA3DSkinMan()->SetCurSkinRender(psr1);
	g_pGame->GetA3DEngine()->GetA3DLitModelRender()->SetAlphaMeshesArray(0);

	//	Render sky first
	if (m_pA3DSky)
	{
		m_pA3DSky->SetCamera(pViewport->GetA3DCamera());

		if( !m_pScene->m_Env.bUnderWater )
			m_pA3DSky->Render();
	}

	if( !RenderOnSky(pA3DViewport) )
		return false;
	
	bool bRenderTerrain = true;
	if( m_pTerrainOutline && m_pTerrainOutline->GetReplaceTerrain() )
		bRenderTerrain = false;

	if (m_pA3DTerrain && bRenderTerrain)
	{
		m_pA3DTerrain->SetDNFactor(m_pSunMoon->GetDNFactor());

		if (!m_pScene->m_Env.bUnderWater)
			m_pA3DTerrain->Render(pA3DViewport);
		else
			m_pA3DTerrain->RenderUnderWater(pA3DViewport, m_pA3DTerrainWater->GetCurCaustTexture(),
								m_pA3DTerrainWater->GetCaustDU(), m_pA3DTerrainWater->GetCaustDV());
	}

	m_pScene->Render(pViewport);

	if (m_pForest && !m_pScene->m_Env.bUnderWater && pConfigs->m_bShowForest)
	{
		m_pForest->Render(pA3DViewport);
	}
	
	g_pGame->GetA3DDevice()->SetAlphaBlendEnable(false);
	g_pGame->GetA3DDevice()->SetAlphaTestEnable(true);
	g_pGame->GetA3DDevice()->SetAlphaFunction(A3DCMP_GREATER);
	g_pGame->GetA3DDevice()->SetAlphaRef(84);

	//	Delay CECDecalMan's rendering
	for (int i=0; i < NUM_MANAGER-1; i++)
	{
		if (m_aManagers[i])
			m_aManagers[i]->Render(pViewport);
	}

	g_pGame->GetA3DDevice()->SetAlphaBlendEnable(false);
	g_pGame->GetA3DDevice()->SetAlphaTestEnable(true);

	g_pGame->GetA3DEngine()->GetA3DLitModelRender()->SetDNFactor(m_pSunMoon->GetDNFactor());
	g_pGame->GetA3DEngine()->GetA3DLitModelRender()->Render(pA3DViewport, false);
	g_pGame->GetA3DEngine()->GetA3DLitModelRender()->ResetRenderInfo(false);
	
	//	Render non alpha skin models
	psr1->Render(pA3DViewport, false);
	psr1->ResetRenderInfo(false);

	RenderGrasses(pViewport, false);

	pA3DEngine->FlushCachedAlphaMesh(pA3DViewport);
	g_pGame->GetA3DDevice()->SetAlphaTestEnable(false);
	g_pGame->GetA3DDevice()->SetAlphaBlendEnable(true);

	g_pGame->GetA3DDevice()->SetTextureColorOP(1, A3DTOP_DISABLE);
	g_pGame->GetA3DDevice()->SetTextureAlphaOP(1, A3DTOP_DISABLE);
	g_pGame->GetA3DDevice()->ClearTexture(1);

	RenderShadows(pViewport);

	//	Render alpha objects on the other side of water
	if (m_pA3DTerrainWater->GetNumWaterAreas() && g_pGame->GetA3DDevice()->GetD3DCaps().MaxUserClipPlanes > 1)
	{
		//	If camera is above water, render underwater alpha object before water is rendered
		//	If camera is under water, then render abovewater alpha object before water is rendered
		if (m_pScene->m_Env.bUnderWater)
			RenderAlphaObjects(pA3DViewport, true);
		else
			RenderAlphaObjects(pA3DViewport, false);
	}

	//	Render water
	A3DLIGHTPARAM lightParam = g_pGame->GetDirLight()->GetLightparam();
	if (m_pA3DTerrainWater)
	{
		m_pA3DTerrainWater->SetDNFactor(m_pSunMoon->GetDNFactor());
		A3DTerrain2 * pTerrain = m_pA3DTerrain;
		if( !bRenderTerrain )
			pTerrain = NULL;
		if (!m_pScene->m_Env.bUnderWater)
		{							   
			m_pA3DTerrainWater->RenderReflect(lightParam.Direction, pViewport->GetA3DViewport(), pTerrain, m_pA3DSky, NULL, WorldRenderForReflected, WorldRenderForRefracted, this, WorldRenderOnSky, this, true, m_pA3DTerrain);
		}
		else
			m_pA3DTerrainWater->RenderRefract(lightParam.Direction, pViewport->GetA3DViewport(), pTerrain, m_pA3DSky, NULL, WorldRenderForRefracted, this, WorldRenderOnSky, this, true);
	}

	g_pGame->GetA3DEngine()->GetA3DLitModelRender()->RenderAlpha(pA3DViewport);
	g_pGame->GetA3DEngine()->GetA3DLitModelRender()->ResetRenderInfo(true);

	//	Render alpha objects on the same side of water
	if (m_pA3DTerrainWater->GetNumWaterAreas() && g_pGame->GetA3DDevice()->GetD3DCaps().MaxUserClipPlanes > 1)
	{
		//	If camera is above water, render abovewater alpha object after water is rendered
		//	If camera is under water, then render underwater alpha object after water is rendered
		if (m_pScene->m_Env.bUnderWater)
			RenderAlphaObjects(pA3DViewport, false);
		else
			RenderAlphaObjects(pA3DViewport, true);
	}
	else
	{
		//	Render alpha skin models
		// alpha skin mesh dont set z
		psr1->RenderAlpha(pA3DViewport);
	}

	psr1->ResetRenderInfo(true);
	
	// now we render nature objects
	RenderNatureObjects(pViewport);

	// Render grasses
	RenderGrasses(pViewport, true);

	if( m_pSunMoon )
		m_pSunMoon->RenderSolarFlare(pViewport);

	// Render Clouds
	if( m_pCloudManager )
	{
		CECConfigs* pConfigs = g_pGame->GetConfigs();
		if( pConfigs->GetSystemSettings().nCloudDetail > 0 )
			m_pCloudManager->Render(pViewport->GetA3DViewport());
	}

	if (m_aManagers[MAN_DECAL])
		m_aManagers[MAN_DECAL]->Render(pViewport);

	return true;
}

//	Render alpha objects
void CECWorld::RenderAlphaObjects(A3DViewport* pA3DViewport, bool bAboveWaterOjbects)
{
	//	Setup water clip plane
	D3DXPLANE p;
	if (bAboveWaterOjbects)
		p = D3DXPLANE(0.0f, 1.0f, 0.0f, -m_pA3DTerrainWater->GetCurWaterHeight());
	else
		p = D3DXPLANE(0.0f, -1.0f, 0.0f, m_pA3DTerrainWater->GetCurWaterHeight());

	g_pGame->GetA3DDevice()->GetD3DDevice()->SetClipPlane(0, p);
	g_pGame->GetA3DDevice()->SetDeviceRenderState(D3DRS_CLIPPLANEENABLE, D3DCLIPPLANE0);
	g_pGame->GetSkinRender1()->RenderAlpha(pA3DViewport);
	g_pGame->GetA3DDevice()->SetDeviceRenderState(D3DRS_CLIPPLANEENABLE, 0);
}

//	Render shadows routine
bool CECWorld::RenderShadows(CECViewport* pViewport)
{
	if( !g_pGame->GetShadowRender() )
		return true;

	// first setup terrain object pointer for shadows
	A3DLIGHTPARAM param = g_pGame->GetDirLight()->GetLightparam();
	// now render the shadows
	if (g_pGame->GetShadowRender())
	{
		g_pGame->GetShadowRender()->SetA3DTerrain(GetTerrain());
		g_pGame->GetShadowRender()->Render(pViewport->GetA3DViewport(), param.Direction);
	}

	return true;
}

//	Render grasses
bool CECWorld::RenderGrasses(CECViewport* pViewport, bool bRenderAlpha)
{
	if( !g_pGame->GetConfigs()->m_bShowGrassLand )
		return true;

	if (m_pGrassLand)
	{
		m_pGrassLand->SetDNFactor(m_pSunMoon->GetDNFactor());
		m_pGrassLand->Render(pViewport->GetA3DViewport(), bRenderAlpha);
	}

	return true;
}

//	Get host player object
CECHostPlayer* CECWorld::GetHostPlayer()
{
	CECPlayerMan* pPlayerMan = GetPlayerMan();
	return pPlayerMan ? pPlayerMan->GetHostPlayer() : NULL;
}

//	Get terrain height of specified position
float CECWorld::GetTerrainHeight(const A3DVECTOR3& vPos, A3DVECTOR3* pvNormal/* NULL */)
{
	A3DTerrain2* pTerrain = GetTerrain();
	ASSERT(pTerrain);
	return pTerrain->GetPosHeight(vPos, pvNormal);
}

//	Get ground height of specified poistion
float CECWorld::GetGroundHeight(const A3DVECTOR3& vPos, A3DVECTOR3* pvNormal/* NULL */)
{
	A3DVECTOR3 vGndPos, vNormal;
	VertRayTrace(vPos, vGndPos, vNormal);
	if (pvNormal) *pvNormal = vNormal;
	return vGndPos.y;
}

//	Get water height of specified position
float CECWorld::GetWaterHeight(const A3DVECTOR3& vPos)
{
	A3DTerrainWater* pWater = GetTerrainWater();
	ASSERT(pWater);
	if(!pWater) return 0.f;
	return pWater->GetWaterHeight(vPos);
}

//	Get terrain object
A3DTerrain2* CECWorld::GetTerrain()
{
	return m_pA3DTerrain;
}

CECOrnamentMan* CECWorld::GetOrnamentMan()
{
	return (CECOrnamentMan*)m_aManagers[MAN_ORNAMENT];
}

//	Get A3DSky object
A3DSkySphere * CECWorld::GetSkySphere()
{
	return m_pA3DSky;
}

bool CECWorld::RenderForReflected(A3DViewport* pViewport)
{
	CECViewport ecViewport;
	ecViewport.InitFromA3D(pViewport); 

	CECConfigs* pConfigs = g_pGame->GetConfigs();

	//	Use a temporary skin render so that we don't break world's main rendering
	A3DSkinRender* psr = g_pGame->GetSkinRender2();
	A3DSkinRender* poldsr = g_pGame->GetA3DEngine()->GetA3DSkinMan()->GetCurSkinRender();
	g_pGame->GetA3DEngine()->GetA3DSkinMan()->SetCurSkinRender(psr);

	A3DLitModelRender* plmr = g_pGame->GetLitModelRender1();
	g_pGame->GetA3DEngine()->SetA3DLitModelRender(plmr);

	GfxRenderContainer rc;
	g_pGame->GetA3DGFXExMan()->SetCustomGfxRenderContainer(&rc);

	if (pConfigs->GetSystemSettings().nWaterEffect == 2 && m_pForest)
		m_pForest->Render(pViewport);
		
	g_pGame->GetA3DDevice()->SetAlphaBlendEnable(false);
	g_pGame->GetA3DDevice()->SetAlphaTestEnable(true);
	g_pGame->GetA3DDevice()->SetAlphaFunction(A3DCMP_GREATER);
	g_pGame->GetA3DDevice()->SetAlphaRef(84);

	for (int i=0; i < NUM_MANAGER; i++)
	{
		if (m_aManagers[i])
		{
			if( m_aManagers[i]->GetManagerID() == MAN_NPC )
			{
				if (pConfigs->GetSystemSettings().nWaterEffect == 2)
					m_aManagers[i]->RenderForReflect(&ecViewport);
			}
			else
				m_aManagers[i]->RenderForReflect(&ecViewport);
		}
	}

	g_pGame->GetA3DEngine()->GetA3DLitModelRender()->Render(pViewport, false);
	g_pGame->GetA3DEngine()->GetA3DLitModelRender()->ResetRenderInfo(false);
	
	psr->Render(pViewport, false);
	psr->ResetRenderInfo(false);

	if (pConfigs->GetSystemSettings().nWaterEffect == 2)
		RenderGrasses(&ecViewport, false);

	g_pGame->GetA3DDevice()->SetAlphaTestEnable(false);
	g_pGame->GetA3DDevice()->SetAlphaBlendEnable(true);

	RenderShadows(&ecViewport);

	g_pGame->GetA3DEngine()->GetA3DLitModelRender()->RenderAlpha(pViewport);
	g_pGame->GetA3DEngine()->GetA3DLitModelRender()->ResetRenderInfo(true);

	psr->RenderAlpha(pViewport);
	psr->ResetRenderInfo(true);

	// last flush gfx
	g_pGame->GetA3DGFXExMan()->RenderAllSkinModels(pViewport);
	//	Render skin models from gfx
	psr->Render(pViewport, false);
	psr->ResetRenderInfo(false);
	psr->RenderAlpha(pViewport);
	psr->ResetRenderInfo(true);
	g_pGame->GetA3DGFXExMan()->RenderAllGfx(pViewport);

	//	Restore skin render
	g_pGame->GetA3DEngine()->GetA3DSkinMan()->SetCurSkinRender(poldsr);
	g_pGame->GetA3DEngine()->SetA3DLitModelRender(NULL);
	g_pGame->GetA3DGFXExMan()->SetCustomGfxRenderContainer(NULL);

	return true;
}

bool CECWorld::RenderForRefracted(A3DViewport* pViewport)
{
	CECViewport ecViewport;
	ecViewport.InitFromA3D(pViewport); 

	CECConfigs* pConfigs = g_pGame->GetConfigs();

	//	Use a temporary skin render so that we don't break world's main rendering
	A3DSkinRender* psr = g_pGame->GetSkinRender2();
	A3DSkinRender* poldsr = g_pGame->GetA3DEngine()->GetA3DSkinMan()->GetCurSkinRender();
	g_pGame->GetA3DEngine()->GetA3DSkinMan()->SetCurSkinRender(psr);

	A3DLitModelRender* plmr = g_pGame->GetLitModelRender1();
	g_pGame->GetA3DEngine()->SetA3DLitModelRender(plmr);

	GfxRenderContainer rc;
	g_pGame->GetA3DGFXExMan()->SetCustomGfxRenderContainer(&rc);

	A3DVECTOR3 vecCamPos = pViewport->GetCamera()->GetPos();
	bool bCamBelowWater = m_pA3DTerrainWater->IsUnderWater(vecCamPos);

	if( bCamBelowWater )
	{
		if (m_pForest)
			m_pForest->Render(pViewport);
	}

	g_pGame->GetA3DDevice()->SetAlphaBlendEnable(false);
	g_pGame->GetA3DDevice()->SetAlphaTestEnable(true);
	g_pGame->GetA3DDevice()->SetAlphaFunction(A3DCMP_GREATER);
	g_pGame->GetA3DDevice()->SetAlphaRef(84);

	for (int i=0; i < NUM_MANAGER; i++)
	{
		if (m_aManagers[i])
			m_aManagers[i]->RenderForRefract(&ecViewport);
	}

	// render critters.
	m_pScene->Render(&ecViewport);

	if( bCamBelowWater )
	{
		g_pGame->GetA3DEngine()->GetA3DLitModelRender()->Render(pViewport, false);
		g_pGame->GetA3DEngine()->GetA3DLitModelRender()->ResetRenderInfo(false);
	}
	else
	{
		g_pGame->GetA3DEngine()->GetA3DLitModelRender()->RenderForRefract(pViewport, m_pA3DTerrainWater->GetCurWaterHeight());
		g_pGame->GetA3DEngine()->GetA3DLitModelRender()->ResetRenderInfo(false);
	}

	psr->Render(pViewport, false);
	psr->ResetRenderInfo(false);

	if (pConfigs->GetSystemSettings().nWaterEffect == 2)
	{
		if( bCamBelowWater )
			RenderGrasses(&ecViewport, false);
		else
		{
			if (m_pGrassLand)
			{
				m_pGrassLand->SetDNFactor(m_pSunMoon->GetDNFactor());
				m_pGrassLand->RenderForRefract(pViewport, m_pA3DTerrainWater->GetCurWaterHeight());
			}
		}
	}

	g_pGame->GetA3DDevice()->SetAlphaBlendEnable(true);
	g_pGame->GetA3DDevice()->SetAlphaTestEnable(false);

	RenderShadows(&ecViewport);

	g_pGame->GetA3DEngine()->GetA3DLitModelRender()->RenderAlpha(pViewport);
	g_pGame->GetA3DEngine()->GetA3DLitModelRender()->ResetRenderInfo(true);

	psr->RenderAlpha(pViewport);
	psr->ResetRenderInfo(true);

	if (pConfigs->GetSystemSettings().nWaterEffect == 2)
		RenderGrasses(&ecViewport, true);

	// last flush gfx
	g_pGame->GetA3DGFXExMan()->RenderAllSkinModels(pViewport);
	//	Render skin models from gfx
	psr->Render(pViewport, false);
	psr->ResetRenderInfo(false);
	psr->RenderAlpha(pViewport);
	psr->ResetRenderInfo(true);
	g_pGame->GetA3DGFXExMan()->RenderAllGfx(pViewport);

	if (m_pCloudManager && bCamBelowWater)
		m_pCloudManager->Render(pViewport);

	//	Restore skin render
	g_pGame->GetA3DEngine()->GetA3DSkinMan()->SetCurSkinRender(poldsr);
	g_pGame->GetA3DEngine()->SetA3DLitModelRender(NULL);
	g_pGame->GetA3DGFXExMan()->SetCustomGfxRenderContainer(NULL);

	return true;
}

bool CECWorld::RenderOnSky(A3DViewport* pViewport)
{
	CECViewport ecViewport;

	ecViewport.InitFromA3D(pViewport);

	if( m_pSunMoon )
		m_pSunMoon->Render(&ecViewport);

	CECConfigs* pConfigs = g_pGame->GetConfigs();

	if (m_pTerrainOutline)
	{
		m_pTerrainOutline->SetDNFactor(m_pSunMoon->GetDNFactor());
		m_pTerrainOutline->Render2(pViewport);
	}

	return true;
}

//	Get object by specified ID
//	iAliveFlag: 0, both alive and dead; 1, must be alive; 2, must be dead
CECObject* CECWorld::GetObject(int idObject, int iAliveFlag)
{
	CECObject* pObject = NULL;

	if (ISNPCID(idObject))
	{
		if (!(pObject = GetNPCMan()->GetNPC(idObject)))
			return NULL;

		if ((iAliveFlag == 1 && ((CECNPC*)pObject)->IsDead()) ||
			(iAliveFlag == 2 && !((CECNPC*)pObject)->IsDead()))
			return NULL;
	}
	else if (ISPLAYERID(idObject))
	{
		if (!(pObject = GetPlayerMan()->GetPlayer(idObject)))
			return NULL;

		if ((iAliveFlag == 1 && ((CECPlayer*)pObject)->IsDead()) ||
			(iAliveFlag == 2 && !((CECPlayer*)pObject)->IsDead()))
			return NULL;
	}
	else if (ISMATTERID(idObject))
		pObject = GetMatterMan()->GetMatter(idObject);

	return pObject;
}

class CECRandomMapMusicManager{
	int		m_idInstance;
	bool	m_bMusicPlayed;
	bool	m_bNightSFX;
public:
	CECRandomMapMusicManager()
		: m_idInstance(-1)
		, m_bMusicPlayed(false)
		, m_bNightSFX(false)
	{
	}
	void Reset(){
		if (m_idInstance > 0){
			m_idInstance = -1;
			m_bMusicPlayed = false;
			m_bNightSFX = false;
		}
	}
	bool IsHostReady()const{
		CECHostPlayer* pHost = g_pGame->GetGameRun()->GetWorld()->GetHostPlayer();
		return pHost != NULL && pHost->IsSkeletonReady();
	}
	bool IsNight()const{
		return g_pGame->GetGameRun()->GetWorld()->GetSunMoon()->GetDNFactor() > 0.5f;
	}
	const char *GetSFXPath()const{
		return IsNight() ? "music\\ambiencestereo\\desertnight.wav" : "music\\ambiencestereo\\desertday.wav";
	}
	void StartPlay(int idInstance){
		m_idInstance = idInstance;
		g_pGame->GetBackMusic()->SetLoopType(CELBackMusic::LOOP_ALL);
		g_pGame->GetBackMusic()->SetSilenceTime(0);
		g_pGame->GetBackMusic()->PlayMusic("Music\\2014\\随机副本.mp3");
		g_pGame->GetBackMusic()->PlayBackSFX(GetSFXPath());
		m_bMusicPlayed = true;
		m_bNightSFX = IsNight();
	}
	void UpdatePlay(){
		if (m_bNightSFX != IsNight()){
			g_pGame->GetBackMusic()->PlayBackSFX(GetSFXPath());
			m_bNightSFX = IsNight();
		}
	}
	void Update(int idInstance){
		if (!CECWorld::IsRandomMap(idInstance)){
			Reset();
			return;
		}
		if (!IsHostReady()){
			return;
		}
		if (m_idInstance <= 0 || m_idInstance != idInstance){
			StartPlay(idInstance);
		}else{
			UpdatePlay();
		}
	}
};
void CECWorld::UpdateMusicForRandomMap(){
	static CECRandomMapMusicManager s_musicManager;
	s_musicManager.Update(GetInstanceID());
}

//	Update current precinct which the host player is in
void CECWorld::UpdateCurPrecinct()
{
	if (!m_pPrecinctSet)
		return;

	CECHostPlayer* pHost = GetHostPlayer();
	if (!pHost || !pHost->IsSkeletonReady())
		return;

	if (IsRandomMap()){
		return;
	}
	
	CELBackMusic* pBackMusic = g_pGame->GetBackMusic();
	A3DVECTOR3 vPos = pHost->GetMoveControl().GetLastSevPos();//pHost->GetPos();
	CELPrecinct* pPrecinct = m_pPrecinctSet->IsPointIn(vPos.x, vPos.z, GetInstanceID());
	if (!pPrecinct)
	{
		pBackMusic->StopMusic(true, true);
		pBackMusic->StopBackSFX(true);
		return;
	}

	static DWORD dwEnterTime = 0;

	//	Change precinct ...
	
	bool bNight = m_pSunMoon->GetDNFactor() > 0.5f ? true : false;
	if (!m_pCurPrecinct || m_pCurPrecinct != pPrecinct)
	{
		if( !m_pCurPrecinct || m_pCurPrecinct->GetID() != pPrecinct->GetID() || pPrecinct->GetID() == 0 )
		{
			//	Change background music
			pBackMusic->SetLoopType((CELBackMusic::LOOPTYPE)pPrecinct->GetMusicLoopType());
			pBackMusic->SetSilenceTime(0);//pPrecinct->GetMusicInterval() * 1000);

			int i, iNumMusic = pPrecinct->GetMusicFileNum();
			for (i=0; i < iNumMusic; i++)
				pBackMusic->PlayMusic(pPrecinct->GetMusicFile(i), false/*true*/, i == 0 ? true : false);

			//	Change background sfx
			AString str;
			if( bNight )
				str = pPrecinct->GetSoundFile_n();
			else
				str = pPrecinct->GetSoundFile();
			
			pPrecinct->SetNightSFX(bNight);
			if (str.GetLength() && af_CheckFileExt(str, ".wav"))
				pBackMusic->PlayBackSFX(str);
			else
				pBackMusic->StopBackSFX(true);
		}

		if( !m_pCurPrecinct || m_pCurPrecinct->GetID() != pPrecinct->GetID() )
		{
			if (pPrecinct->GetID() && g_pGame->GetGameRun()->GetGameState() == CECGameRun::GS_GAME)
			{
				//	Notify UI module
				CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
				pGameUI->ChangePrecinct(pPrecinct);
			}			
		}

		// pk 检查
		bool bPink = pHost->IsInvader() || pHost->IsPariah(); // 红粉名
		bool bPKCheck = (!m_pCurPrecinct && pPrecinct->IsPKProtect()) || (m_pCurPrecinct && m_pCurPrecinct->IsPKProtect() != pPrecinct->IsPKProtect());
		if ( pHost->PKLevelCheck() && bPKCheck && !bPink)
		{
			g_pGame->GetGameSession()->c2s_CmdSendEnterPKPrecinct();
			dwEnterTime = a_GetTime();
		}		

		m_pCurPrecinct = pPrecinct;
	}
	else
	{
		if( m_pCurPrecinct->GetNightSFX() != bNight )
		{
			AString str;
			if( bNight )
				str = m_pCurPrecinct->GetSoundFile_n();
			else
				str = m_pCurPrecinct->GetSoundFile();

			m_pCurPrecinct->SetNightSFX(bNight);
			if (str.GetLength() && af_CheckFileExt(str, ".wav"))
				pBackMusic->PlayBackSFX(str);
			else
				pBackMusic->StopBackSFX(true);
		}

		// 
		//	Is host still PVP ? Try again
		DWORD dwTime = a_GetTime();
		bool bPink = pHost->IsInvader() || pHost->IsPariah(); // 红粉名
		if (!bPink && m_pCurPrecinct->IsPKProtect() && pHost->IsPVPOpen() && pHost->PKLevelCheck() && dwEnterTime + 5000 < dwTime)
		{
			g_pGame->GetGameSession()->c2s_CmdSendEnterPKPrecinct();
			dwEnterTime = dwTime;
		}
	}
}

//	Update current region which the host player is in
void CECWorld::UpdateCurRegion()
{
	if (!m_pRegionSet)
		return;

	CECHostPlayer* pHost = GetHostPlayer();
	if (!pHost || !pHost->IsSkeletonReady())
		return;

	//	Note: we use the host position that client sent to server last time
	//		rather than current host's position in client.
	static DWORD dwEnterTime = 0;
	A3DVECTOR3 vPos = pHost->GetMoveControl().GetLastSevPos();
	CELRegion* pRegion = m_pRegionSet->IsPointInRegion(vPos.x, vPos.z);
	if (pRegion)
	{
		if (pRegion != m_pCurRegion)
		{
			//	Change region ...
			g_pGame->GetGameSession()->c2s_CmdEnterSanctuary(pHost->GetCharacterID());
			m_pCurRegion = pRegion;			
		}
		else
		{
			//	Is host still out of sanctuary are ? Try again
			DWORD dwTime = a_GetTime();
			if (!pHost->IsInSanctuary() && dwEnterTime + 5000 < dwTime)
			{
				g_pGame->GetGameSession()->c2s_CmdEnterSanctuary(pHost->GetCharacterID());
				dwEnterTime = dwTime;	
			}
		}

		// 开传送点
		OpenNewWayPoints();
	}
	else
	{
		m_pCurRegion = NULL;
	}

	// Check whether the pet is in the sanctuary.
	static DWORD dwEnterTime2 = 0;
	int iPetID = 0;
	if( pHost->GetPetCorral() ) iPetID = pHost->GetPetCorral()->GetActivePetNPCID();
	CECPet* pPet = dynamic_cast<CECPet*>(GetObject(iPetID, 1));
	if( pPet && (pPet->IsCombatPet() || pPet->IsSummonPet() || pPet->IsEvolutionPet()) )	// 只检测战宠和召唤宠, 进化宠和战宠一样
	{
		A3DVECTOR3 vPetPos = pPet->GetServerPos();
		CELRegion* pPetRegion = m_pRegionSet->IsPointInRegion(vPetPos.x, vPetPos.z);
		if( pPetRegion )
		{
			if( pPetRegion != m_pCurPetRegion )
			{
				g_pGame->GetGameSession()->c2s_CmdEnterSanctuary(iPetID);
				m_pCurPetRegion = pPetRegion;
			}
			else
			{
				DWORD dwTime = a_GetTime();
				if( !pHost->IsPetInSanctuary() && dwEnterTime2 + 5000 < dwTime )
				{
					g_pGame->GetGameSession()->c2s_CmdEnterSanctuary(iPetID);
					dwEnterTime2 = dwTime;
				}
			}
		}
		else
		{
			m_pCurPetRegion = NULL;
		}
	}

	if( pHost->IsClothesOn() )
	{
		//  we wait until the host player has loaded its all equipments.
		//	Check transport box
		CELTransportBox* pTrans = m_pRegionSet->IsPointInTransport(vPos.x, vPos.y, vPos.z, GetInstanceID());
		if (pTrans)
		{
			bool bCanTrans = true;

			// 仙魔副本对修真有要求
			if( pTrans->GetInstanceID() == 121 || pTrans->GetInstanceID() == 122 )
			{
				if( pHost->GetBasicProps().iLevel2 < 20 )
					bCanTrans = false;
			}

			if( bCanTrans )
				g_pGame->GetGameSession()->c2s_CmdEnterInstance(pTrans->GetIndex(), pTrans->GetInstanceID());
		}
	}
}

void CECWorld::SetTimeOfDay(float t)
{
	while( m_pA3DSky->IsTextureLoading() )
		Sleep(10);

	m_pA3DSky->SetTimePassed(0x7fffffff);
	m_bResetEnv = true;
	m_pSunMoon->SetTimeOfTheDay(t);
}

//	Checkout instance data
bool CECWorld::CheckOutInst(int idInst, DWORD dwRegionTimeStamp, DWORD dwPrecinctTimeStamp)
{
	if (m_idInst != idInst)
		return false;

	if (!m_pRegionSet || m_pRegionSet->GetTimeStamp() != dwRegionTimeStamp){
		if (m_pRegionSet){
			a_LogOutput(1, "regionset timestamp error:server(0x%x) != local(0x%x)", dwRegionTimeStamp, m_pRegionSet->GetTimeStamp());
		}
		return false;
	}

	if (!m_pPrecinctSet || m_pPrecinctSet->GetTimeStamp() != dwPrecinctTimeStamp){
		if (m_pPrecinctSet){
			a_LogOutput(1, "regionset timestamp error:server(0x%x) != local(0x%x)", dwPrecinctTimeStamp, m_pPrecinctSet->GetTimeStamp());
		}
		return false;
	}

	return true;
}

void WorldRenderForReflected(A3DViewport * pViewport, void * pArg)
{
	CECWorld * pWorld = (CECWorld *) pArg;
	pWorld->RenderForReflected(pViewport);
}

void WorldRenderForRefracted(A3DViewport * pViewport, void * pArg)
{
	CECWorld * pWorld = (CECWorld *) pArg;
	pWorld->RenderForRefracted(pViewport);
}

void WorldRenderOnSky(A3DViewport * pViewport, void * pArg)
{
	CECWorld * pWorld = (CECWorld *) pArg;
	pWorld->RenderOnSky(pViewport);
}


void CECWorld::ResetParallelWorld(void *p)
{
	using namespace S2C;
	const cmd_parallel_world_info *pCmd = (const cmd_parallel_world_info *)p;
	if (pCmd->worldtag != GetInstanceID()){
		return;
	}
	m_worlds.clear();
	m_worlds.reserve(pCmd->count);
	for (int i(0); i < pCmd->count; ++ i)
	{
		m_worlds.push_back(ParallelWorldInfo(pCmd->list[i].line, pCmd->list[i].load));
	}
}

int CECWorld::GetParallelWorldCount()const
{
	return (int)m_worlds.size();
}

int CECWorld::GetParallelWorldID(int index)const
{
	if (index >= 0 && index < GetParallelWorldCount()){
		return m_worlds[index].id;
	}
	ASSERT(false);
	return 0;
}

float CECWorld::GetParallelWorldLoad(int index)const
{
	if (index >= 0 && index < GetParallelWorldCount()){
		return m_worlds[index].load;
	}
	ASSERT(false);
	return 0.0f;
}

void CECWorld::OnParallelWorldChange(const A3DVECTOR3 &vPos, int iParallelWorldID)
{
	if (m_iParallelWorldID != iParallelWorldID){
		m_iParallelWorldID = iParallelWorldID;		
		if (m_worlds.empty() && iParallelWorldID > 0){
			m_worlds.push_back(ParallelWorldInfo(iParallelWorldID, 0));
		}
		if (IsLoadThreadReady()){
			SuspendLoadThread();
		}
		//	清除 NPC、Matter、ElserPlayer、Decal等
		for (int i=0; i < NUM_MANAGER; i++)
		{
			if (m_aManagers[i] && i != MAN_ORNAMENT)	//	场景不变，所以场景相关不做清除
				m_aManagers[i]->OnLeaveGameWorld();
		}
		//	加载进程中数据不做清除，避免主玩家的模型加载受影响，其它已删除的 NPC、ElsePlayer、Matter 等模型在递交加载结果时会因为找不到对象自行清除
		if (IsLoadThreadReady()){
			StartLoaderThread();
		}
	}
	SetLoadCenter(vPos);
}

bool GetMapBounds(const A3DAABB& aabb, const A3DMATRIX4& viewTM, float& l, float& r, float& b, float& t, float& n, float& f)
{
	A3DVECTOR3 verts[8];
	A3DVECTOR3 vecX = A3DVECTOR3(2 * aabb.Extents.x, 0.0f, 0.0f);
	A3DVECTOR3 vecY = A3DVECTOR3(0.0f, 2 * aabb.Extents.y, 0.0f);
	A3DVECTOR3 vecZ = A3DVECTOR3(0.0f, 0.0f, 2 * aabb.Extents.z);
	
	verts[0] = aabb.Mins;
	verts[1] = aabb.Mins + vecX;
	verts[2] = aabb.Mins + vecY;
	verts[3] = aabb.Mins + vecZ;
	verts[4] = aabb.Maxs;
	verts[5] = aabb.Maxs - vecX;
	verts[6] = aabb.Maxs - vecY;
	verts[7] = aabb.Maxs - vecZ;
	
	A3DAABB aabbWorld;
	aabbWorld.Clear();
	
	A3DVECTOR3 vert;
	int i;
	for(i=0; i<8; i++)
	{
		vert = verts[i]	* viewTM;
		aabbWorld.AddVertex(vert);
	}
	
	l = aabbWorld.Mins.x;
	r = aabbWorld.Maxs.x;
	b = aabbWorld.Mins.y;
	t = aabbWorld.Maxs.y;
	n = aabbWorld.Mins.z;
	f = aabbWorld.Maxs.z;
	
	return true;
}


bool CECWorld::IsRandomMap(int id)
{
	return CECUIConfig::Instance().GetGameUI().IsRandomMap(id);
}
bool CECWorld::IsRandomMap()const
{
	return CECUIConfig::Instance().GetGameUI().IsRandomMap(GetInstanceID());
}

// Export a bitmap of top view
bool CECWorld::ExpTopView(int nWidth,bool bNight)
{
//	AString exp
	int nMapWidth = nWidth;
	bool bReadOnlyOld = true;
	bool g_bReadOnly = false;
	bool bOldFlags = g_pA3DConfig->RT_GetShowFPSFlag();
	float fOldFactor = 0.5;
//	if(bNight) g_fNightFactor = 1.0f;
//	else g_fNightFactor = 0.0f;
	//Only for topview export,start...
	/*
	m_Lighting.dwAmbient = A3DCOLORRGB(192,192,192);
	m_Lighting.dwSunCol = A3DCOLORRGB(192,192,192);
	m_Lighting.fSunPower = 1.0f;
	A3DLight* pLight = g_Render.GetDirLight();
	A3DLIGHTPARAM param;
	param = pLight->GetLightparam();
	param.Ambient = A3DCOLORVALUE(m_Lighting.dwAmbient);
	param.Diffuse = A3DCOLORVALUE(m_Lighting.dwSunCol);
	param.Direction = m_Lighting.vSunDir;
    pLight->SetLightParam(param);
	RecreateTerrainRender(true,true);
	*/
	// Topview end.
	
	g_pA3DConfig->RT_SetShowFPSFlag(false);
	//Create top view map Viewport
	A3DViewport* pViewport = NULL;
	A3DEngine* pA3DEngine = g_pGame->GetA3DEngine();
	A3DDevice* pA3DDevice = g_pGame->GetA3DDevice();
//	pViewport = pA3DEngine->GetActiveViewport();
	CECViewport* pECViewport = g_pGame->GetViewport();
	pViewport = pECViewport->GetA3DViewport();
	A3DVIEWPORTPARAM* ptemp = pViewport->GetParam();
	A3DVIEWPORTPARAM newvp;
	A3DVIEWPORTPARAM oldvp;
	oldvp = *ptemp;
	newvp.Height = nMapWidth;
	newvp.Width  = nMapWidth;
	newvp.MinZ   = 0.0f;
	newvp.MaxZ   = 1.0f;
	newvp.X      = 0.0f;
	newvp.Y      = 0.0f;
	pViewport->SetParam(&newvp);
	A3DCameraBase *pOldCam = pViewport->GetCamera();
	
	// now create a ortho camera
	A3DOrthoCamera *pCamera = new A3DOrthoCamera();
	ASSERT(pCamera);
	if( !pCamera->Init(pA3DDevice, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f) )
	{
		g_Log.Log("CElementMap::ExpTopView(), failed to init ortho camera!");
		return false;
	}
	
	A3DTerrain2 *pTerrain = GetTerrain();


	pTerrain->SetLODType(A3DTerrain2::LOD_NONE);

	ARectF rectTerrain = pTerrain->GetTerrainArea();
	A3DVECTOR3 vecCenter = A3DVECTOR3((rectTerrain.left + rectTerrain.right) / 2.0f, 
	50  + pTerrain->GetPosHeight(A3DVECTOR3(0)), (rectTerrain.top + rectTerrain.bottom) / 2.0f);
	A3DVECTOR3 vecExt = A3DVECTOR3(1024);
	A3DAABB aabbWorld(vecCenter - vecExt, vecCenter + vecExt);
	
	float l,r,b,t,n,f;
	pCamera->SetPos(vecCenter);
	pCamera->SetDirAndUp(-g_vAxisY, g_vAxisZ);
	
	GetMapBounds(aabbWorld,pCamera->GetViewTM(),l,r,b,t,n,f);
	pCamera->SetProjectionParam(l, r, b, t, n, f);

//	if(m_pTerrainWater) m_pTerrainWater->GetA3DTerrainWater()->ChangeHostCameraAndView(g_Render.GetA3DEngine()->GetActiveViewport(),(A3DCamera*)pCamera);
	// create top view map render target
	A3DDEVFMT devFmt;
	devFmt.bWindowed	= true;
	devFmt.nWidth		= nMapWidth;
	devFmt.nHeight		= nMapWidth;
	devFmt.fmtTarget	= A3DFMT_X8R8G8B8;
	devFmt.fmtDepth		= A3DFMT_D24X8;

	A3DRenderTarget *pRenderTarget = new A3DRenderTarget();
	ASSERT(pRenderTarget);
	if( !pRenderTarget->Init(pA3DDevice, devFmt, true, true) )
	{
		g_Log.Log("CElementMap::ExpTopView(), failed to init render target!");
//		g_fNightFactor = fOldFactor;
		return false;
	}
	
	if(pTerrain) pTerrain->Update(0,pCamera,A3DVECTOR3(0));
//	if(m_pTerrainWater) m_pTerrainWater->FrameMove(0);
//	if(m_pSceneObjectMan) m_pSceneObjectMan->Tick(0);

	pA3DDevice->SetRenderTarget(pRenderTarget);
	pA3DDevice->SetRestoreToRenderTarget(pRenderTarget);
	
//	if (!g_Render.BeginRender())
	{
	//	g_fNightFactor = fOldFactor;
	//	return false;
	}

	g_pGame->GetGameRun()->BeginRender();


	A3DMaterial& Mat = (pTerrain->GetTerrainMaterial());
	A3DCOLORVALUE old = Mat.GetSpecular();
	Mat.SetSpecular(0,0,0,1.0f);
	pViewport->SetClearColor(A3DCOLORRGB(255,0,255));
	pViewport->SetCamera(pCamera);
//	pECViewport->SwitchCamera(true);
	pViewport->Active();
	pViewport->ClearDevice();

//	pA3DEngine->RenderAllRenderTarget();
	pA3DDevice->SetFillMode(A3DFILL_SOLID);
	Render(pECViewport);
	//g_Render.GetA3DEngine()->RenderGFX(pViewport, 0xffffffff);
	//pA3DEngine->FlushVertexCollector(pViewport);
	//pA3DEngine->FlushCachedAlphaMesh(pViewport);
	//pA3DEngine->FlushInternalSpriteBuffer();
	//pA3DEngine->FlushMeshCollector(pViewport);
	
	Mat.SetSpecular(old);
//	if (!g_Render.EndRender())
//	{
	//	g_fNightFactor = fOldFactor;
//		return false;
//	}

	g_pGame->GetGameRun()->EndRender();

	pA3DDevice->SetDefaultRestoreToRenderTarget();
	pA3DDevice->RestoreRenderTarget();

/*	AString pathName;
	int k = 0;//atof(GetMapName());
	if(k>=0 && k < 10) pathName.Format("%s\\0%s.bmp","d:\\",1);
	else pathName.Format("%s\\%s.bmp","d:\\",1);
	*/
	D3DXSaveSurfaceToFile(_AL("D:\\dd.bmp"), D3DXIFF_BMP, pRenderTarget->GetTargetSurface(), NULL , NULL);

	g_bReadOnly = bReadOnlyOld;
//	g_fNightFactor = fOldFactor;
	pViewport->SetCamera(pOldCam);
	pViewport->SetParam(&oldvp);
	pA3DEngine->SetActiveViewport(pViewport);
	pA3DEngine->SetActiveCamera(pOldCam);
	pA3DEngine->RemoveRenderTarget(pRenderTarget);
	pRenderTarget->Release();
	delete pRenderTarget;
	pCamera->Release();
	delete pCamera;
	g_pA3DConfig->RT_SetShowFPSFlag(bOldFlags);
	return true;
}

bool CECWorld::IsValid()
{
	if (IsRandomMap())
		return m_pA3DTerrain != NULL;
	else
		return true;
}

A3DVECTOR3 CECWorld::GetHostPosForClientTick(CECViewport* pViewport)
{
	if (GetHostPlayer() && GetHostPlayer()->IsInForceNavigateState()){
		CECHostNavigatePlayer* player = GetHostPlayer()->GetNavigatePlayer();
		return player->GetPos();			
		
	}

	return m_pGameRun->GetSafeHostPos(pViewport);
}

int CECWorld::GetReginIndex(CELRegion* pRegion)
{
	if(!m_pRegionSet || !pRegion) return -1;

	for (int i=0;i<m_pRegionSet->GetRegionNum();i++)
	{
		if(pRegion == m_pRegionSet->GetRegion(i))
			return i;
	}

	return -1;
}
void CECWorld::OpenNewWayPoints()
{	
	int index = GetReginIndex(m_pCurRegion);
	if (index >= 0 && m_RegionWayPointsMap.find(index)!=m_RegionWayPointsMap.end())
	{
		abase::vector<int> newPoints;
		for (unsigned int i=0;i<m_RegionWayPointsMap[index].size();i++)
		{
			int j = 0;
			for(j=0;j<GetHostPlayer()->GetWayPointNum();j++){
				if (m_RegionWayPointsMap[index].at(i) == GetHostPlayer()->GetWayPoint(j))
					break;							
			}
			if(j>=GetHostPlayer()->GetWayPointNum())
				newPoints.push_back(m_RegionWayPointsMap[index].at(i));
		}
		if(newPoints.size())
			g_pGame->GetGameSession()->c2s_CmdSendActiveWayPoints(newPoints.size(),newPoints.begin());	
	}
}

bool CECWorld::IsAtOboroMajorCity(const A3DVECTOR3 &vPos)const{
	//	当前是否是胧族主城、是则对场景和显示有特殊处理
	if (!IsMajorMap()){
		return false;
	}
	CECInstance *pInst = g_pGame->GetGameRun()->GetInstance(GetInstanceID());
	float x = vPos.x + pInst->GetColNum() * 512.0f;
	float z = pInst->GetRowNum() * 512.0f - vPos.z;
	int su = (int)(x / 1024.0f);
	int sv = (int)(z / 1024.0f);
	if (su >= pInst->GetColNum() || su < 0 || 
		sv >= pInst->GetRowNum() || sv < 0){
		return false;
	}
	return (sv == 1 || sv == 2) && su == 7;
}

bool CECWorld::IsAtOboroMajorCity()const{
	return m_pScene && IsAtOboroMajorCity(m_pScene->GetLoadCenter());
}