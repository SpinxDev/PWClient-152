/*
 * FILE: EC_GameRun.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/8/27
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

//	For WM_MOUSEWHEEL message
#pragma warning (disable: 4284)

#define _WIN32_WINDOWS	0x0500

#include "EC_Global.h"
#include "EC_GPDataType.h"
#include "EC_GameRun.h"
#include "EC_UIManager.h"
#include "EC_LoginUIMan.h"
#include "EC_GameUIMan.h"
#include "EC_Game.h"
#include "EC_InputCtrl.h"
#include "EC_HostPlayer.h"
#include "EC_RTDebug.h"
#include "EC_GameSession.h"
#include "EC_GFXCaster.h"
#include "EC_Resource.h"
#include "EC_Viewport.h"
#include "EC_World.h"
#include "EC_Counter.h"
#include "EC_Utility.h"
#include "EC_Configs.h"
#include "EC_UIConfigs.h"
#include "EC_ManPlayer.h"
#include "EC_ManMatter.h"
#include "EC_ManNPC.h"
#include "EC_ManOrnament.h"
#include "EC_ManMessage.h"
#include "EC_ManDecal.h"
#include "EC_LoadProgress.h"
#include "EC_FullGlowRender.h"
#include "EL_BackMusic.h"
#include "EL_Forest.h"
#include "EL_Grassland.h"
#include "EC_SceneLoader.h"
#include "EC_SceneBlock.h"
#include "EC_FixedMsg.h"
#include "EC_SoundCache.h"
#include "EL_CloudManager.h"
#include "EC_Instance.h"
#include "EC_TeamMan.h"
#include "EC_ShortcutSet.h"
#include "EC_Shortcut.h"
#include "EC_SunMoon.h"
#include "EC_Clipboard.h"
#include "EC_Faction.h"
#include "EC_ImageRes.h"
#include "EC_HostInputFilter.h"
#include "EC_AutoHomeInputFilter.h"
#include "defence/defence.h"
#include "acreport.hpp"
#include "acremotecode.hpp"
#include "Render.h"
#include "GT/gt_overlay.h"
#include "EC_Optimize.h"
#include "EC_UIHelper.h"
#include "EC_MCDownload.h"
#include "EC_AutoPolicy.h"
#include "EC_QuickBuyPopMan.h"
#include "EC_RandMallShopping.h"
#include "EC_TimeSafeChecker.h"
#include "EC_Shop.h"
#include "EC_RandomMapPreProcessor.h"
#include "EC_Reconnect.h"
#include "EC_ContinueLoad.h"
#include "EC_MemSimplify.h"
#include "EC_LoginQueue.h"

#ifdef _PROFILE_MEMORY
#include "Memory\EC_HookMemory.h"
#endif

#include "gnetdef.h"
#include "roleinfo"

#include "AFI.h"
#include "A3DEngine.h"
#include "A3DCamera.h"
#include "A3DConfig.h"
#include "A3DSkinRender.h"
#include "A3DSkinMan.h"
#include "A3DWireCollector.h"
#include "A3DFlatCollector.h"
#include "A3DGFXExMan.h"
#include "A3DSkinMeshMan.h"
#include "A3DTrackMan.h"
#include "A3DTerrain2.h"
#include "A3DFont.h"
#include "A3DViewport.h"
#include "AWString.h"
#include "AWScriptFile.h"
#include <AScriptFile.h>

#include "AM3DSoundDevice.h"

#include "AutoScene.h"

#include "globaldataman.h"
#include "defence\cheaterkiller.h"
#include "defence\imagechecker.h"
#include "DlgQShop.h"
#include "DlgPQ.h"
#include "DlgCamera.h"
#include "DlgGMQueryItem.h"
#include "DlgMonsterSpirit.h"

#include "EC_CrossServer.h"
#include "EC_LoginSwitch.h"
#include "ExpTypes.h"
#include "elementdataman.h"

#include "uniquedatamodifybroadcast.hpp"


#define new A_DEBUG_NEW

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

#define TIME_TICKANIMATION	30
#define TIME_UPDATEEAR		100

//	User configs data version
#define USERCFG_VERSION		3

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

//	Game statistic
struct EC_STATISTIC
{
	//	Data size counter
	int		iMeshSize;			//	Skin mesh size
	int		iAllTexSize;		//	Total texture size
	int		iStrmDSize;			//	Dynamic stream size
	int		iStrmSSize;			//	Static stream size
	int		iStrmBSize;			//	Backup stream size
	int		iTrackSize;			//	Track data size
	int		iTrnDataSize;		//	Terrain data size
	int		iSndCacheSize;		//	Current cached sound size

	//	Time counter
	int		iTickTime;			//	Total tick time
	int		iRenderTime;		//	Total render time

	//	Object counter
	int		iNumPlayer;			//	Else player number
	int		iNumNPC;			//	NPC number
	int		iNumMatter;			//	Matter number
	int		iNumScnGFX;			//	Scene GFX number
	int		iNumOnmt;			//	Ornament number
	int		iNumScnModel;		//	Scene ECModel
	float	fSndCacheHit;		//	Sound cache hit percent
	int		iNumTrnBlock;		//	Visible terrain block counter

	//	Memory statistics
	int		iMemPeak;			//	Peak memory size
	DWORD	dwMemAllocCnt;		//	Memory allocate counter
	int		iMemCurSize;		//	Current allocated memory size
	int		iMemRawSize;		//	Current allocated raw memory size
	int		aSMemBlkSize[16];	//	Small memory block size
	int		aSMemSize[16];		//	Small memory allocate size
	int		aSMemBlkCnt[16];	//	Small memory total block counter
	int		aSMemFreeCnt[16];	//	Small memory free block 
	int		iSMemBlkCnt;		//	Small memory block counter
	int		iSMemAllocSize;		//	Allocated small memory size
	int		iLMemBlkCnt;		//	Large memory block counter
	int		iLMemAllocSize;		//	Allocated large memory size
};

//	Frame controller
struct FRAMECTRL
{
	int		iTickCnt;		//	Tick counter
	int		iRenderCnt;		//	Render counter
	int		iTickTime;		//	Accumulate tick time
	int		iAvgRdTime;		//	Average render time
	int		iRenderTime;	//	Accumulate render time
};

static CECCounter		l_SaveCfgCnt;	//	Time counter used to save UI configs
static GNET::RoleInfo	l_SelRoleInfo;	//	Selected character's role info.
static GNET::RoleInfo	l_RedirectLoginPosRoleInfo;	//	Selected character's role Redirect login info.
static EC_STATISTIC		l_Statistic;	//	Game statistic
static CECCounter		l_StatCnt;		//	Time counter used by statistic
static FRAMECTRL		l_fc;			//	Frame controller

//	网络延迟
static bool			l_bFirstQuery;
static int			l_iDelayTimeStamp;	//	上一次发送网络延迟查询协议时的时间戳
static CECCounter	l_DelayQueryCounter;//	查询网络延迟协议的计时器

//	网络对时
static CECCounter	l_QueryServerTime;	//	查询服务器当前时间

///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECGameRun
//	
///////////////////////////////////////////////////////////////////////////

CECGameRun::CECGameRun() : 
m_ID2NameTab(1024),
m_Name2IDTab(1024),
m_InstTab(64),
m_CommonDataTab(1024),
m_pendingLogout(false),
m_bFirstShow(true)
{
	m_pLogo			= NULL;
	m_pClassification = NULL;
	m_pUIManager	= NULL;
	m_pWorld		= NULL;
	m_pInputCtrl	= NULL;
	m_pInputFilter	= NULL;
	m_iGameState	= GS_NONE;
	m_bPVPMode		= false;
	m_dwRenderTime	= 0;
	m_bUIHasCursor	= false;
	m_iCfgDataSize	= 0;
	m_pCfgDataBuf	= NULL;
	m_iLogoutFlag	= -1;
	m_pHostPlayer	= NULL;
	m_pTeamMan		= NULL;
	m_pNormalSCS	= NULL;
	m_pTeamSCS		= NULL;
	m_pTradeSCS		= NULL;
	m_pPoseSCS		= NULL;
	m_pFactionSCS	= NULL;
	m_pMessageMan	= NULL;
	m_pClipboard	= NULL;
	m_iDblExpMode	= 0;
	m_iDExpEndTime	= 0;
	m_iAvaDExpTime	= 0;
	m_bLockGamePt	= false;
	m_nGShopTimeStamp = 0;
	m_nGShopTimeStamp2 = 0;

	m_pLoadProgress	= NULL;
	m_pFullGlowRender = NULL;

	//	Set time counters
	l_SaveCfgCnt.SetPeriod(1200000);
	l_StatCnt.SetPeriod(1000);

	memset(&l_Statistic, 0, sizeof (l_Statistic));
	memset(&l_fc, 0, sizeof (l_fc));
	memset(&m_WallowInfo, 0, sizeof(m_WallowInfo));
	m_bAccountLoginInfoShown = true;

	m_accountInfoFlag = 0;
	m_bAccountInfoShown = true;
	
	l_bFirstQuery = true;
	m_iInGameDelay = 0;
	l_DelayQueryCounter.SetPeriod(30*1000);

	l_QueryServerTime.SetPeriod(10*60*1000);

	m_SellingRoleID = 0;

	m_pMemSimplify = new CECMemSimplify;
}

CECGameRun::~CECGameRun()
{
}

//	Initialize object
bool CECGameRun::Init()
{
	//	Create full glow render
	m_pFullGlowRender = new CECFullGlowRender();
	if( !m_pFullGlowRender->Init(g_pGame->GetA3DDevice()) )
	{
		glb_ErrorOutput(ECERR_NOTENOUGHMEMORY, "CECGameRun::Init", __LINE__);
		m_pFullGlowRender->Release();
		return false;
	}

	if( m_pFullGlowRender->CanDoFullGlow() )
	{
		m_pFullGlowRender->SetGlowOn(g_pGame->GetConfigs()->GetSystemSettings().bFullGlow);
		m_pFullGlowRender->SetWarpOn(g_pGame->GetConfigs()->GetSystemSettings().bSpaceWarp);
		m_pFullGlowRender->SetFlareOn(g_pGame->GetConfigs()->GetSystemSettings().bSunFlare);
	}
	
	//	Load instance information
	if (!LoadInstanceInfo("Configs\\instance.txt"))
	{
		glb_ErrorOutput(ECERR_FAILEDTOCALL, "CECGameRun::Init", __LINE__);
		return false;
	}
	if(!LoadHistoryStage("Configs\\history_stage.txt"))
	{
		glb_ErrorOutput(ECERR_FAILEDTOCALL, "CECGameRun::Init", __LINE__);
	}

	if (!ImportSoundStringTable("Configs\\sound.txt"))
	{
		glb_ErrorOutput(ECERR_FAILEDTOCALL, "CECGameRun::Init", __LINE__);
		return false;
	}

	//	Create UI manager
	if (!(m_pUIManager = new CECUIManager))
	{
		glb_ErrorOutput(ECERR_NOTENOUGHMEMORY, "CECGameRun::Init", __LINE__);
		return false;
	}

	if (!m_pUIManager->Init(g_pGame->GetA3DEngine(), -1))
	{
		a_LogOutput(1, "CECGameRun::Init, Failed to initialize UI manager.");
		return false;
	}

	//	Create input controller
	if (!(m_pInputCtrl = new CECInputCtrl))
	{
		glb_ErrorOutput(ECERR_NOTENOUGHMEMORY, "CECGameRun::Init", __LINE__);
		return false;
	}

	//	Create host input filter
	if (!(m_pInputFilter = new CECHostInputFilter(m_pInputCtrl)))
	{
		glb_ErrorOutput(ECERR_NOTENOUGHMEMORY, "CECGameRun::Init", __LINE__);
		return false;
	}

	//	Create host input filter
// 	if (!(m_pAutoHomeFilter = new CECAutoHomeInputFilter(m_pInputCtrl)))
// 	{
// 		glb_ErrorOutput(ECERR_NOTENOUGHMEMORY, "CECGameRun::Init", __LINE__);
// 		return false;
// 	}

	//	Clipboard
	if (!(m_pClipboard = new CECClipboard))
	{
		glb_ErrorOutput(ECERR_NOTENOUGHMEMORY, "CECGameRun::Init", __LINE__);
		return false;
	}

// 	g_Render.Init();

	g_pGame->ChangeCursor(RES_CUR_NORMAL);
	g_pGame->ShowCursor(true);

	m_pRandomMapProc = NULL;

	return true;
}

//	Release object
void CECGameRun::Release()
{
	//  通知子进程退出
// 	if( g_pGame->GetConfigs()->IsMiniClient() )
// 		CECMCDownload::GetInstance().SendQuit();

	//	Release resources
	EndGameState(false);

	if (m_pCfgDataBuf)
	{
		a_free(m_pCfgDataBuf);
		m_pCfgDataBuf = NULL;
	}

	if (m_pUIManager)
	{
		m_pUIManager->Release();
		delete m_pUIManager;
		m_pUIManager = NULL;
	}

	delete m_pInputFilter;
	m_pInputFilter = NULL;

// 	delete m_pAutoHomeFilter;
// 	m_pAutoHomeFilter = NULL;

	if (m_pInputCtrl)
	{
		m_pInputCtrl->Release();
		delete m_pInputCtrl;
		m_pInputCtrl = NULL;
	}

	if (m_pClipboard)
	{
		delete m_pClipboard;
		m_pClipboard = NULL;
	}

	if (m_pFullGlowRender)
	{
		m_pFullGlowRender->Release();
		delete m_pFullGlowRender;
		m_pFullGlowRender = NULL;
	}

	//	Release all instance information
	InstTable::iterator it = m_InstTab.begin();
	for (; it != m_InstTab.end(); ++it)
	{
		CECInstance* pInst = *it.value();
		delete pInst;
	}	
	m_InstTab.clear();

	ReleaseSoundTable();

	//	Release all player name and ID cache
	ClearNameIDPairs();

	UniqueDataMap::iterator unique_itr = m_UniqueDataMap.begin();
	for (;unique_itr!=m_UniqueDataMap.end();++unique_itr)
	{
		unique_data* p = unique_itr->second;
		delete p;
	}
	m_UniqueDataMap.clear();
	STAGEMAP::iterator stage_itr = m_stageMap.begin();
	for (;stage_itr!=m_stageMap.end();++stage_itr)
	{
		delete stage_itr->second;
	}
	m_stageMap.clear();

	if (m_pRandomMapProc)
	{
		m_pRandomMapProc->Release();
		delete m_pRandomMapProc;
		m_pRandomMapProc = NULL;
	}

	delete m_pMemSimplify;
	m_pMemSimplify = NULL;
}

bool CECGameRun::IsFirstShow()const
{
	return m_bFirstShow;
}

void CECGameRun::OnFirstShowEnd()
{
	m_bFirstShow = false;
}

//	Start game
bool CECGameRun::StartGame(int idInst, const A3DVECTOR3& vHostPos)
{
	//	End current game state
	EndGameState();

	memset(&m_WallowInfo, 0, sizeof(m_WallowInfo));
	m_iGameState = GS_GAME;

	if (!g_pGame->LoadInGameRes())
	{
		a_LogOutput(1, "CECGameRun::StartGame, Failed to call LoadInGameRes().");
		return false;
	}

	//	Create message manager
	if (!(m_pMessageMan = new CECMessageMan(this)))
	{
		glb_ErrorOutput(ECERR_NOTENOUGHMEMORY, "CECGameRun::StartGame", __LINE__);
		return false;
	}

	//	Create default game world
	if (!JumpToInstance(idInst, vHostPos))
	{
		a_LogOutput(1, "CECGameRun::StartGame, Failed to create game world.");
		return false;
	}	
	
	//	设置跨服成功标识，以利于 CECGameUIMan 根据跨服状态做相应初始化
	if (CECCrossServer::Instance().IsWaitLogin()){
		CECCrossServer::Instance().OnLoginSuccess();
	}
	if (CECReconnect::Instance().IsReconnecting()){
		CECReconnect::Instance().OnReconnectSuccess();
	}

	//	Create host player
	if (!CreateHostPlayer())
	{
		a_LogOutput(1, "CECGameRun::StartGame, Failed to create host player.");
		return false;
	}

	//	Create team manager
	if (!(m_pTeamMan = new CECTeamMan))
	{
		glb_ErrorOutput(ECERR_NOTENOUGHMEMORY, "CECGameRun::StartGame", __LINE__);
		return false;
	}

	// Reset faction manager
	g_pGame->GetFactionMan()->Release(false);

	//	Create shortcuts
	if (!CreateShortcuts())
	{
		a_LogOutput(1, "CECGameRun::StartGame, Failed to create shortcuts");
		return false;
	}

	//	Change UI manager
	if (!m_pUIManager->ChangeCurUIManager(CECUIManager::UIMAN_INGAME))
	{
		a_LogOutput(1, "CECGameRun::StartGame, Failed to change UI manager.");
		return false;
	}	
	m_pInputFilter->LoadHotKey();

	CECGameUIMan* pGameUIMan = m_pUIManager->GetInGameUIMan();
	if (pGameUIMan)
		pGameUIMan->ChangeWorldInstance(idInst);

	l_SaveCfgCnt.Reset();

	//	Change cursor to default icon
	g_pGame->ChangeCursor(RES_CUR_NORMAL);
	//	Discard current frame
	g_pGame->DiscardFrame();

	//	Clear frame controller
	memset(&l_fc, 0, sizeof (l_fc));

	//	初始化网络延迟查询
	l_bFirstQuery = true;
	m_iInGameDelay = 0;
	l_DelayQueryCounter.Reset();

	l_QueryServerTime.Reset();

	// clear the selling id
	m_SellingRoleID = 0;

	// 下载器响应进入游戏状态
	if( g_pGame->GetConfigs()->IsMiniClient() )
		CECMCDownload::GetInstance().SendSwitchGame(true);

	return true;
}

//	End game state
void CECGameRun::OnEndGameState()
{
	ReleasePendingActions();

	//	Release UI module
	m_pUIManager->ChangeCurUIManager(-1);

	//	Release shortcuts
	ReleaseShortcuts();

	//	Release team manager
	A3DRELEASE(m_pTeamMan);

	//	Release host player before world released
	ReleaseHostPlayer();

	//	Release world
	ReleaseWorld();

	//	Release message manager
	A3DRELEASE(m_pMessageMan);

	g_pGame->ReleaseInGameRes();
	
	//  Return the default memory state
	m_pMemSimplify->OnEndGameState();
	CECOptimize::Instance().OnEndGameState();
}

void CECGameRun::OnWindowSizeChange(A3DRECT rcOld, A3DRECT rcNew)
{
	//	窗口大小缩放
	AUIManager *pAUIManager = GetUIManager()->GetCurrentUIManPtr();
	if (pAUIManager)
		pAUIManager->RearrangeWindows(rcOld, rcNew);
}

//	Start login interface
bool CECGameRun::StartLogin()
{
	//	End current game state
	EndGameState();

	m_iGameState = GS_LOGIN;

	if( !CreateLoginWorld() )
	{
		a_LogOutput(1, "CECGameRun::StartLogin, Failed to create login world.");
		return false;
	}

	//	Change UI manager
	if (!m_pUIManager->ChangeCurUIManager(CECUIManager::UIMAN_LOGIN))
	{
		a_LogOutput(1, "CECGameRun::StartLogin, Failed to change UI manager.");
		return false;
	}

	m_pUIManager->GetLoginUIMan()->LaunchPreface();
	
	if (!m_pLogo){
		m_pLogo = new A2DSprite;
		if (!m_pLogo->Init(g_pGame->GetA3DDevice(), "logo.dds", 0)){
			A3DRELEASE(m_pLogo);
		}else{
			m_pLogo->SetLinearFilter(true);
		}
	}	
	if (af_IsFileExist("surfaces\\kr.dds"))
	{
		if (!m_pClassification){
			m_pClassification = new A2DSprite;
			if (!m_pClassification->Init(g_pGame->GetA3DDevice(), "kr.dds", 0)){
				A3DRELEASE(m_pClassification);
			}else{
				m_pClassification->SetLinearFilter(true);
			}
		}
	}	
	//	Change cursor to default icon
	g_pGame->ChangeCursor(RES_CUR_NORMAL);
	//	Discard current frame
	g_pGame->DiscardFrame();

	return true;
}

//	End login state
void CECGameRun::OnEndLoginState()
{
	//	Release UI module
	m_pUIManager->ChangeCurUIManager(-1);

	//  Release World
	ReleaseLoginWorld();

	A3DRELEASE(m_pLogo);
	A3DRELEASE(m_pClassification);
}

//	End current game state
void CECGameRun::EndGameState(bool bReset/* true */)
{
	if (m_iGameState == GS_NONE)
		return;

	int iCurState = m_iGameState;
	m_iGameState = GS_NONE;

	if (iCurState == GS_LOGIN)
		OnEndLoginState();
	else if (iCurState == GS_GAME)
		OnEndGameState();

	//	Stop background sound and music
	CELBackMusic* pBackMusic = g_pGame->GetBackMusic();
	if (pBackMusic)
	{
		pBackMusic->StopMusic(true, true);
		pBackMusic->StopBackSFX();
	}

	if (bReset)
		g_pGame->Reset();
}

//	Create world
bool CECGameRun::CreateWorld(int id, const A3DVECTOR3& vHostPos, int iParallelWorldID)
{
	CECInstance* pInst = GetInstance(id);
	if (!pInst)
	{
		a_LogOutput(1, "CECGameRun::CreateWorld, wrong instance id: %d", id);
		return false;
	}

	if (!(m_pWorld = new CECWorld(this)))
	{
		glb_ErrorOutput(ECERR_NOTENOUGHMEMORY, "CECGameRun::CreateWorld", __LINE__);
		return false;
	}

	if (!m_pWorld->Init(id, iParallelWorldID))
	{
		a_LogOutput(1, "CECGameRun::CreateWorld, Failed to initialize world.");
		return false;
	}

	//	Set scene resource loading distance
	CECSceneBlock::SetResLoadDists(g_pGame->GetConfigs()->GetSystemSettings().nSight + 1);

	//	Begin load progress
	BeginLoadProgress(0, 100);

	//	Load scene from file
	char szFile[MAX_PATH];
	sprintf(szFile, "maps\\%s\\%s.ecwld", pInst->GetPath(), pInst->GetPath());
//	sprintf(szFile, "maps\\132\\132.ecwld");
	if (!m_pWorld->LoadWorld(szFile, vHostPos))
	{
		a_LogOutput(1, "CECGameRun::CreateWorld, Failed to load world.");
		return true;
	}

	//	End load progress
	EndLoadProgress();

	// now check files md5 again;
	bool CheckMapMD5(const char *);
	if( !CheckMapMD5(pInst->GetPath()) )
	{
		a_LogOutput(1, "CECGameRun::CreateWorld, Failed to create %s because md5 error.", szFile);
		return false;
	}

	//	Set default fog parameters
	A3DDevice* pA3DDevice = g_pGame->GetA3DDevice();
	CECConfigs* pConfigs = g_pGame->GetConfigs();
	pA3DDevice->SetFogStart(pConfigs->GetDefFogStart());
	pA3DDevice->SetFogEnd(pConfigs->GetDefFogEnd());
	pA3DDevice->SetFogDensity(pConfigs->GetDefFogDensity());
	pA3DDevice->SetFogColor(pConfigs->GetDefFogColor());

	if( m_pWorld->GetForest() )
		m_pWorld->GetForest()->SetLODLevel(pConfigs->GetSystemSettings().nTreeDetail * 0.25f);
	if( m_pWorld->GetGrassLand() )
		m_pWorld->GetGrassLand()->SetLODLevel(pConfigs->GetSystemSettings().nGrassDetail * 0.25f);
	if( m_pWorld->GetCloudManager() )
		m_pWorld->GetCloudManager()->SetCloudLevel(pConfigs->GetSystemSettings().nCloudDetail * 25 / 10);

	// adjust time of the day;
	struct tm serverLocal = g_pGame->GetServerLocalTime();
	int nTimeInDay = serverLocal.tm_hour * 3600 + serverLocal.tm_min * 60 + serverLocal.tm_sec;
	m_pWorld->GetSunMoon()->SetTimeOfTheDay(nTimeInDay / (4.0f * 3600.0f));
	return true;
}

//	Release world
void CECGameRun::ReleaseWorld()
{
	m_pInputCtrl->ClearKBFilterStack();
	m_pInputCtrl->ClearMouFilterStack();

	g_pGame->GetViewport()->SwitchCamera(false);
	
	if (m_pWorld)
	{
		if (m_pHostPlayer)
			m_pHostPlayer->SetPlayerMan(NULL);

		m_pWorld->Release();
		delete m_pWorld;
		m_pWorld = NULL;
	}
}

//	Create Login World
bool CECGameRun::CreateLoginWorld()
{
	A3DVECTOR3	vCameraPos(-874.891968, 54.800098, -280.675232);	//	2014年秋季资料片的登录场景位置

	//	获取登录界面初始位置配置文件，修改登录场景初始加载位置，使场景创建时即加载此处内容
	AIniFile theIni;
	char szFile[MAX_PATH] = {0};
	sprintf(szFile, "%s\\Configs\\SceneCtrl.ini", af_GetBaseDir());
	if (theIni.Open(szFile))
	{
		char szKey[40];
		CECLoginUIMan::LOGIN_SCENE	cameraIndex = CECLoginUIMan::LOGIN_SCENE_LOGIN;
		sprintf(szKey, "PosX%d", cameraIndex);
		vCameraPos.x = theIni.GetValueAsFloat("Camera", szKey, 0.0f);
		sprintf(szKey, "PosY%d", cameraIndex);
		vCameraPos.y = theIni.GetValueAsFloat("Camera", szKey, 0.0f);
		sprintf(szKey, "PosZ%d", cameraIndex);
		vCameraPos.z = theIni.GetValueAsFloat("Camera", szKey, 0.0f);
		theIni.Close();
	}
	else
	{
		a_LogOutput(1, "CECGameRun::CreateLoginWorld, Failed to set initial login position.");
	}

	if (!g_pGame->LoadLoginRes())
	{
		a_LogOutput(1, "CECGameRun::CreateLoginWorld, Failed to call LoadLoginRes().");
		return false;
	}

	if (!(m_pWorld = new CECWorld(this)))
	{
		glb_ErrorOutput(ECERR_NOTENOUGHMEMORY, "CECGameRun::CreateLoginWorld", __LINE__);
		return false;
	}

	if (!m_pWorld->Init(0))
	{
		a_LogOutput(1, "CECGameRun::CreateLoginWorld, Failed to initialize world.");
		return false;
	}

	//	Set scene resource loading distance
	CECSceneBlock::SetResLoadDists(0, 2000.0f);	//	2014年资料片：为减少资源占用（资源释放有BUG？），应尽量减小视野距离

	//	Begin load progress
	BeginLoadProgress(0, 100);

	//	Load login scene from file
	if (!m_pWorld->LoadWorld("Maps\\Login\\Login.ecwld", vCameraPos))
	{
		a_LogOutput(1, "CECGameRun::CreateLoginWorld, Failed to load world.");
		return false;
	}

	//	End load progress
	EndLoadProgress();

	g_pGame->GetViewport()->GetA3DCamera()->SetPos(vCameraPos);

	//	Set default fog parameters
	A3DDevice* pA3DDevice = g_pGame->GetA3DDevice();
	CECConfigs* pConfigs = g_pGame->GetConfigs();
	pA3DDevice->SetFogStart(pConfigs->GetDefFogStart());
	pA3DDevice->SetFogEnd(pConfigs->GetDefFogEnd());
	pA3DDevice->SetFogDensity(pConfigs->GetDefFogDensity());
	pA3DDevice->SetFogColor(pConfigs->GetDefFogColor());

	AString strTheme = pConfigs->GetRandomThemeFile();
	if (!strTheme.IsEmpty())
		g_pGame->GetBackMusic()->PlayMusic(strTheme, false, true);
	
	if( g_pGame->GetGameRun()->GetWorld()->GetForest() )
		g_pGame->GetGameRun()->GetWorld()->GetForest()->SetLODLevel(1.0f);
	if( g_pGame->GetGameRun()->GetWorld()->GetGrassLand() )
		g_pGame->GetGameRun()->GetWorld()->GetGrassLand()->SetLODLevel(1.0f);
	if( g_pGame->GetGameRun()->GetWorld()->GetCloudManager() )
		g_pGame->GetGameRun()->GetWorld()->GetCloudManager()->SetCloudLevel(10);	
	return true;
}

//	Release Login World
void CECGameRun::ReleaseLoginWorld()
{
	if (m_pWorld)
	{
		m_pWorld->Release();
		delete m_pWorld;
		m_pWorld = NULL;
	}

	g_pGame->ReleaseLoginRes();

	return;
}

//	Create host player
bool CECGameRun::CreateHostPlayer()
{
	ASSERT(m_pWorld);
	CECPlayerMan* pPlayerMan = m_pWorld->GetPlayerMan();

	//	Create host player
	if (!(m_pHostPlayer = new CECHostPlayer(pPlayerMan)))
	{
		glb_ErrorOutput(ECERR_NOTENOUGHMEMORY, "CECGameRun::CreateHostPlayer", __LINE__);
		return false;
	}

	return true;
}

//	Release host player
void CECGameRun::ReleaseHostPlayer()
{
	//	Release host player
	if (m_pHostPlayer)
	{
		m_pHostPlayer->Release();
		delete m_pHostPlayer;
		m_pHostPlayer = NULL;
	}
}

//	Create shortcuts
bool CECGameRun::CreateShortcuts()
{
	//	Normal command shortcut set
	m_pNormalSCS = new CECShortcutSet;
	m_pNormalSCS->Init(8);

	CECSCCommand* pSC = new CECSCCommand(CECSCCommand::CMD_SITDOWN);
	m_pNormalSCS->SetShortcut(0, pSC);
	pSC = new CECSCCommand(CECSCCommand::CMD_WALKRUN);
	m_pNormalSCS->SetShortcut(1, pSC);
	pSC = new CECSCCommand(CECSCCommand::CMD_NORMALATTACK);
	m_pNormalSCS->SetShortcut(2, pSC);
//	pSC = new CECSCCommand(CECSCCommand::CMD_FINDTARGET);
//	m_pNormalSCS->SetShortcut(3, pSC);
	pSC = new CECSCCommand(CECSCCommand::CMD_ASSISTATTACK);
	m_pNormalSCS->SetShortcut(3, pSC);
	pSC = new CECSCCommand(CECSCCommand::CMD_FLY);
	m_pNormalSCS->SetShortcut(4, pSC);
	pSC = new CECSCCommand(CECSCCommand::CMD_PICKUP);
	m_pNormalSCS->SetShortcut(5, pSC);
//	pSC = new CECSCCommand(CECSCCommand::CMD_GATHER);
//	m_pNormalSCS->SetShortcut(6, pSC);
	pSC = new CECSCCommand(CECSCCommand::CMD_RUSHFLY);
	m_pNormalSCS->SetShortcut(6, pSC);
	pSC = new CECSCCommand(CECSCCommand::CMD_BINDBUDDY);
	m_pNormalSCS->SetShortcut(7, pSC);
		
	//	Team command shortcut set
	m_pTeamSCS = new CECShortcutSet;
	m_pTeamSCS->Init(2);

	pSC = new CECSCCommand(CECSCCommand::CMD_INVITETOTEAM);
	m_pTeamSCS->SetShortcut(0, pSC);
	pSC = new CECSCCommand(CECSCCommand::CMD_LEAVETEAM);
	m_pTeamSCS->SetShortcut(1, pSC);
//	pSC = new CECSCCommand(CECSCCommand::CMD_KICKTEAMMEM);
//	m_pTeamSCS->SetShortcut(2, pSC);
//	pSC = new CECSCCommand(CECSCCommand::CMD_FINDTEAM);
//	m_pTeamSCS->SetShortcut(2, pSC);

	//	Trade command shortcut set
	m_pTradeSCS = new CECShortcutSet;
	m_pTradeSCS->Init(2);

	pSC = new CECSCCommand(CECSCCommand::CMD_STARTTRADE);
	m_pTradeSCS->SetShortcut(0, pSC);
	pSC = new CECSCCommand(CECSCCommand::CMD_SELLBOOTH);
	m_pTradeSCS->SetShortcut(1, pSC);
	
	//	Pose command shortcut set
	m_pPoseSCS = new CECShortcutSet;
	m_pPoseSCS->Init(NUM_ROLEEXP);

	for (int i=0; i < NUM_ROLEEXP; i++)
	{
		pSC = new CECSCCommand(CECSCCommand::CMD_PLAYPOSE);
		pSC->SetParam(i);
		m_pPoseSCS->SetShortcut(i, pSC);
	}

	//	Faction command shortcut set
	m_pFactionSCS = new CECShortcutSet;
	m_pFactionSCS->Init(1);

	pSC = new CECSCCommand(CECSCCommand::CMD_INVITETOFACTION);
	m_pFactionSCS->SetShortcut(0, pSC);

	return true;
}

//	Release shortcuts
void CECGameRun::ReleaseShortcuts()
{
	A3DRELEASE(m_pNormalSCS);
	A3DRELEASE(m_pTeamSCS);
	A3DRELEASE(m_pTradeSCS);
	A3DRELEASE(m_pPoseSCS);
	A3DRELEASE(m_pFactionSCS);
}

void CECGameRun::ReleasePendingActions()
{
	m_pendingLogout.Clear();
}

//	Release name/id pairs cache
void CECGameRun::ClearNameIDPairs()
{
	if (m_ID2NameTab.size())
	{
		ID2NameTable::iterator it = m_ID2NameTab.begin();
		for (; it != m_ID2NameTab.end(); ++it)
		{
			ACString* pstrName = *it.value();
			delete pstrName;
		}

		m_ID2NameTab.clear();
	}

	m_Name2IDTab.clear();
}

//	Get command shortcut (except pose command) by command ID
CECSCCommand* CECGameRun::GetCmdShortcut(int iCommand)
{
	int i;

	ASSERT(m_pNormalSCS);
	for (i=0; i < m_pNormalSCS->GetShortcutNum(); i++)
	{
		CECSCCommand* pCmdSC = (CECSCCommand*)m_pNormalSCS->GetShortcut(i);
		if (pCmdSC && pCmdSC->GetCommandID() == iCommand)
			return pCmdSC;
	}

	ASSERT(m_pTeamSCS);
	for (i=0; i < m_pTeamSCS->GetShortcutNum(); i++)
	{
		CECSCCommand* pCmdSC = (CECSCCommand*)m_pTeamSCS->GetShortcut(i);
		if (pCmdSC && pCmdSC->GetCommandID() == iCommand)
			return pCmdSC;
	}

	ASSERT(m_pTradeSCS);
	for (i=0; i < m_pTradeSCS->GetShortcutNum(); i++)
	{
		CECSCCommand* pCmdSC = (CECSCCommand*)m_pTradeSCS->GetShortcut(i);
		if (pCmdSC && pCmdSC->GetCommandID() == iCommand)
			return pCmdSC;
	}

	ASSERT(m_pFactionSCS);
	for (i=0; i < m_pFactionSCS->GetShortcutNum(); i++)
	{
		CECSCCommand* pCmdSC = (CECSCCommand*)m_pFactionSCS->GetShortcut(i);
		if (pCmdSC && pCmdSC->GetCommandID() == iCommand)
			return pCmdSC;
	}

	return NULL;
}

//	Get pose command shortcut by pose
CECSCCommand* CECGameRun::GetPoseCmdShortcut(int iPose)
{
	ASSERT(m_pPoseSCS);
	for (int i=0; i < m_pPoseSCS->GetShortcutNum(); i++)
	{
		CECSCCommand* pCmdSC = (CECSCCommand*)m_pPoseSCS->GetShortcut(i);
		if (pCmdSC && pCmdSC->GetParam() == (DWORD)iPose)
			return pCmdSC;
	}

	return NULL;
}

//	Deal windows message
bool CECGameRun::DealWindowsMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (!m_pUIManager || !m_pInputCtrl)
		return false;

	if (m_pUIManager->DealWindowsMessage(message, wParam, lParam))
	{
	//	m_bUIHasCursor = m_pUIManager->UIControlCursor();
		return true;
	}
	else
	{
//		if (message == WM_MOUSEMOVE)
//			m_bUIHasCursor = false;

		switch (message)
		{
		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYDOWN:
		case WM_SYSKEYUP:
			if( message == WM_KEYUP && wParam == VK_SNAPSHOT)
			{
				if (!m_pWorld)
					break;

				int N = 1;

				CECGameUIMan *pGameUIMan = m_pUIManager->GetInGameUIMan();
				if (pGameUIMan)
				{
					CDlgCamera *pDlgCamera = (CDlgCamera *)pGameUIMan->GetDialog("Win_Camera");
					if (pDlgCamera->IsShow())
						N = pDlgCamera->PreparePrint();
				}

				CaptureScreen(N);
				break;
			}

			if (m_pInputCtrl->DealKeyMessage(message, wParam, lParam))
				return true;

			break;

		case WM_LBUTTONDOWN:
		case WM_LBUTTONDBLCLK:
		case WM_LBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONDBLCLK:
		case WM_RBUTTONUP:
	//	case WM_MBUTTONDOWN:
	//	case WM_MBUTTONUP:
		case WM_MOUSEMOVE:
		case WM_MOUSEWHEEL:

			if (m_pInputCtrl->DealMouseMessage(message, wParam, lParam))
				return true;

			break;
		}
	}

	return false;
}

void CECGameRun::TickDiscardedFrame(DWORD dwDeltaTime)
{
	if (m_pWorld){
		if (CECHostPlayer *pHost = m_pWorld->GetHostPlayer()){
			pHost->TickDiscardedFrame(dwDeltaTime);
		}
	}
}

//	Game tick routine
bool CECGameRun::Tick(DWORD dwDeltaTime)
{	
	if (GetWallowInfo().anti_wallow_active &&
		g_pGame->GetGameSession()->IsConnected() &&
		GetGameState() == GS_GAME)
	{
		if (CECUIConfig::Instance().GetGameUI().nWallowHintType != CECUIConfig::GameUI::WHT_KOREA)
		{
			//	防沉迷到3小时，若AU不踢人（消息丢失），则自动下线		
			int stime = g_pGame->GetServerGMTTime();
			int nTime = stime - GetWallowInfo().play_time;
			if (nTime >= 3 * 3600)
			{
				//	已经超过3小时
				g_pGame->GetGameSession()->SetBreakLinkFlag(CECGameSession::LBR_ANTI_WALLOW);
			}
		}		
	}

	DWORD dwTickTime = a_GetTime();

	CECGameSession* pSession = g_pGame->GetGameSession();
	pSession->ProcessNewProtocols();
	DWORD dwRealTime = g_pGame->GetRealTickTime();

	if (m_iLogoutFlag >= 0)
	{
		Logout();
		m_iLogoutFlag = -1;
	}

	CECReconnect::Instance().Tick();

	if (m_pUIManager)
		m_bUIHasCursor = m_pUIManager->UIControlCursor();
	else
		m_bUIHasCursor = false;

	//	Deal input first
	if (m_pInputCtrl)
		m_pInputCtrl->Tick();

	//	Tick world
	if (!TickGameWorld(dwDeltaTime, g_pGame->GetViewport()))
		return false;

	//	Tick UI
	if (m_pUIManager)
		m_pUIManager->Tick();

	//	Tick GFX caster
	g_pGame->GetGFXCaster()->Tick(dwDeltaTime);

	//	Tick GFX Manager
	g_pGame->GetA3DGFXExMan()->Tick(dwDeltaTime);

	//	A3DEngine::TickAnimation trigger animation of many objects.
	//	For example: A3DSky objects, GFX objects etc.
	static DWORD dwAnimTime = 0;
	dwAnimTime += dwDeltaTime;
	while (dwAnimTime >= TIME_TICKANIMATION)
	{
		dwAnimTime -= TIME_TICKANIMATION;
		g_pGame->GetA3DEngine()->TickAnimation();
	}

	//	Update ear position so that all 3D sounds' positions are correct
	static DWORD dwEarTime = 0;
	if ((dwEarTime += dwDeltaTime) >= TIME_UPDATEEAR)
	{
		dwEarTime -= TIME_UPDATEEAR;

		CECHostPlayer* pHostPlayer = NULL;
		if (m_pWorld)
			pHostPlayer = m_pWorld->GetHostPlayer();

		A3DCamera * pCamera = g_pGame->GetViewport()->GetA3DCamera();

		if (GetGameState() == CECGameRun::GS_GAME && pHostPlayer && pHostPlayer->HostIsReady())
		{
			AM3DSoundDevice * pAM3DSoundDevice = g_pGame->GetA3DEngine()->GetAMSoundEngine()->GetAM3DSoundDevice();
			A3DVECTOR3 vecDir = pCamera->GetDirH();
			A3DVECTOR3 vecUp = A3DVECTOR3(0.0f, 1.0f, 0.0f);

			// Now we should adjust the 3d sound device's pos and orientation;
			if (pAM3DSoundDevice)
			{
				pAM3DSoundDevice->SetPosition(pHostPlayer->GetPos() + A3DVECTOR3(0.0f, 0.8f, 0.0f));
				pAM3DSoundDevice->SetOrientation(vecDir, vecUp);
				pAM3DSoundDevice->UpdateChanges();
			}
		}
		else
			g_pGame->GetViewport()->GetA3DCamera()->UpdateEar();
	}

	//	Tick Run-Time debug information
	g_pGame->GetRTDebug()->Tick(dwDeltaTime);

	//	Save UI configs when time reached
	if (m_iGameState == GS_GAME && l_SaveCfgCnt.IncCounter(dwRealTime))
	{
		l_SaveCfgCnt.Reset();
		SaveConfigsToServer();
	}
	
	l_StatCnt.IncCounter(dwDeltaTime);

	pSession->ClearOldProtocols();

	DWORD dwCurrentTick = a_GetTime();
	dwTickTime = (dwCurrentTick > dwTickTime) ? (dwCurrentTick - dwTickTime) : 0;
	l_Statistic.iTickTime = (int)dwTickTime;

	if (GetGameState() == CECGameRun::GS_GAME && l_fc.iAvgRdTime)
	{
		//	Accumulate tick time
		l_fc.iTickCnt++;
		l_fc.iTickTime += (int)dwTickTime;
	}

	if (GetGameState() == GS_GAME && GetHostPlayer() && GetHostPlayer()->HostIsReady())
	{
		if (l_bFirstQuery || l_DelayQueryCounter.IncCounter(dwDeltaTime))
		{
			//	第一次查询，或者查询计数器到点

			//	发送网络拥挤查询协议
			l_iDelayTimeStamp = timeGetTime();
			g_pGame->GetGameSession()->c2s_CmdQueryNetworkDelay(l_iDelayTimeStamp);
			l_DelayQueryCounter.Reset();
			l_bFirstQuery = false;
		}

		if (l_QueryServerTime.IncCounter(dwDeltaTime))
		{
			g_pGame->GetGameSession()->c2s_CmdSendGetServerTime();
			l_QueryServerTime.Reset();
		}
	}

	m_pendingLogout.Update(dwDeltaTime);

	// 优化内存的占用
	m_pMemSimplify->Tick(dwDeltaTime);

	// 更新下载状态
	CECMCDownload::GetInstance().Tick(dwDeltaTime);

	// 更新自动策略
	CECAutoPolicy::GetInstance().Tick(dwDeltaTime);

	if(m_pRandomMapProc)
		m_pRandomMapProc->Tick(dwDeltaTime);

#ifdef _PROFILE_MEMORY
	g_TickMemoryHistory();
#endif

	return true;
}

#define DEFENCE_INTERV_MIN 30000
#define DEFENCE_INTERV_MAX 60000

static DWORD dwDefenceCount = 0;
static DWORD dwDefenceInterval = DEFENCE_INTERV_MIN + (DEFENCE_INTERV_MAX - DEFENCE_INTERV_MIN) * rand() / RAND_MAX;
static DWORD dwLastTime = 0;

DWORD defence_max_check_interval = DEFENCE_INTERV_MAX * 2;
abase::vector<ACRemoteCode*> g_DefenceCodeVec;

extern HANDLE g_hExitGame;
extern CRITICAL_SECTION g_csDefence;

volatile DWORD g_DenfenceTimeMark = 0;
volatile bool g_bHostInGame = false;


// #define DEFENCE_TIME_LOG

void CECGameRun::CheckDefenceCode()
{
	while (true)
	{
		if (g_bExceptionOccured || ::WaitForSingleObject(g_hExitGame, 0) == WAIT_OBJECT_0)
			break;

		g_bHostInGame = (m_pHostPlayer && m_pHostPlayer->HostIsReady());

		if (!g_bHostInGame)
		{
			dwDefenceCount = 0;
			dwLastTime = 0;
			g_DenfenceTimeMark = 0;
			::Sleep(1000);
			continue;
		}

		__try
		{
			CheckDefenceCodeImpl();
		}
		__except (glb_HandleException(GetExceptionInformation()))
		{
			a_LogOutput(1, "exception occured in defence thread (@@CECGameRun::CheckDefenceCodeImpl)!");
		}

		Sleep(2000);
	}
}

void CECGameRun::CheckDefenceCodeImpl()
{
	/*
	 *	anti hacker code here
	 */

	int nCharId = g_pGame->GetGameSession()->GetCharacterID();

	DWORD dwDeltaTime;

	if (dwLastTime == 0)
	{
		dwLastTime = GetTickCount();
		dwDeltaTime = 0;
	}
	else
	{
		DWORD dwNow = GetTickCount();
		dwDeltaTime = CECTimeSafeChecker::ElapsedTime(dwNow, dwLastTime);
		dwLastTime = dwNow;
	}

	dwDefenceCount += dwDeltaTime;

	if (dwDefenceCount >= dwDefenceInterval)
	{
#ifdef DEFENCE_TIME_LOG
		LARGE_INTEGER l, _start, _end, _interv;
		QueryPerformanceFrequency(&l);
		QueryPerformanceCounter(&_start);
#endif

		g_DenfenceTimeMark = ::GetTickCount();
		
		PBYTE buf = NULL;
		int len = Collector::GetInfo(Collector::GetNextInfoType(), &buf, NULL, NULL);
		Octets oct;

		if (buf && len > 0)
		{
			oct.replace(buf, len);
			ACReport report(nCharId, oct);
			g_pGame->GetGameSession()->SendNetData(report, true);
			delete[] buf;
		}

		dwDefenceCount = 0;
		dwDefenceInterval = DEFENCE_INTERV_MIN + (DEFENCE_INTERV_MAX - DEFENCE_INTERV_MIN) * rand() / RAND_MAX;

#ifdef DEFENCE_TIME_LOG
		QueryPerformanceCounter(&_end);
		_interv.QuadPart = _end.QuadPart - _start.QuadPart;
		double _f = (double)_interv.QuadPart * 1000.0 / l.QuadPart;
		char s[100];
		sprintf(s, "1. defence report time = %.2f\n", _f);
		a_LogOutput(1, s);
#endif
	}

	CheaterKiller::TryKillCheaters();

	::EnterCriticalSection(&g_csDefence);
	abase::vector<ACRemoteCode*> vec(g_DefenceCodeVec);
	g_DefenceCodeVec.clear();
	::LeaveCriticalSection(&g_csDefence);

	size_t i(0);
	for (i = 0; i < vec.size(); i++)
	{
#ifdef DEFENCE_TIME_LOG
		LARGE_INTEGER l, _start, _end, _interv;
		QueryPerformanceFrequency(&l);
		QueryPerformanceCounter(&_start);
#endif
		ACRemoteCode* pCode = vec[i];
		if (pCode->dstroleid != nCharId)
		{
			pCode->Destroy();
			continue;
		}

		for (size_t i = 0; i < pCode->content.size(); i++)
			NetDLL::Parse(pCode->content[i].begin(), pCode->content[i].size());
		
		PBYTE buf = NULL;
		int len = Collector::GetInfoNetDLL(&buf);
		Octets oct;

		if (buf && len > 0)
		{
			oct.replace( buf, len );
			ACReport report(nCharId, oct);
			g_pGame->GetGameSession()->SendNetData(report, true);
			delete[] buf;
		}
		
		pCode->Destroy();
		pCode = NULL;

#ifdef DEFENCE_TIME_LOG
		QueryPerformanceCounter(&_end);
		_interv.QuadPart = _end.QuadPart - _start.QuadPart;
		double _f = (double)_interv.QuadPart * 1000.0 / l.QuadPart;
		char s[100];
		sprintf(s, "2. defence check time = %.2f\n", _f);
		a_LogOutput(1, s);
#endif

	}

	for (i = 0; i < 2; i++)
	{
		PBYTE buf = NULL;
		int len = Collector::GetInfoNetDLLEx(&buf);
		Octets oct;

		if (buf && len > 0)
		{
			oct.replace(buf, len);
			ACReport report(nCharId, oct);
			g_pGame->GetGameSession()->SendNetData(report, true);
			delete[] buf;
		}
	}

	ImageChecker::CheckImage();
}

//	Tick game world
bool CECGameRun::TickGameWorld(DWORD dwDeltaTime, CECViewport* pViewport)
{
	if (!m_pWorld)
		return true;

	//	Offset world center for skin model manager, this will fix skin model
	//	flash rendering problem caused by float calculation precision error !
	g_pGame->GetA3DEngine()->GetA3DSkinMan()->SetWorldCenter(pViewport->GetA3DCamera()->GetPos());

	bool bHostReady = m_pHostPlayer && m_pHostPlayer->HostIsReady();
// 	bool bTickHost = bHostReady && (!m_pWorld->IsInAutoSceneState() || m_pWorld->IsAutoScentHostMode());
	bool bTickHost = bHostReady;

	//	Tick host player at first
	if (bTickHost)
		m_pHostPlayer->Tick(dwDeltaTime);

	//	Dispatch game messages
	if (m_pMessageMan)
		m_pMessageMan->Tick(dwDeltaTime);

	//	Some messages also effect host's position and direction, so update
	//	camera position and direction after messages were dispatched
	//	tick host flag must update here
// 	bTickHost = bHostReady && (!m_pWorld->IsInAutoSceneState() || m_pWorld->IsAutoScentHostMode());
	bTickHost = bHostReady;
	if (bTickHost && !m_pHostPlayer->IsChangingFace())
	{
		A3DCamera* pCamera = pViewport->GetA3DCamera();
		A3DCoordinate* pCameraCoord = m_pHostPlayer->GetCameraCoord();
		pCamera->SetPos(pCameraCoord->GetPos());
		pCamera->SetDirAndUp(pCameraCoord->GetDir(), pCameraCoord->GetUp());
	}

	if(m_pWorld->IsValid())
		m_pWorld->Tick(dwDeltaTime, g_pGame->GetViewport());

	if (bTickHost)
	{
		m_pHostPlayer->UpdatePosRelatedGFX(dwDeltaTime);
		m_pHostPlayer->UpdateActionSwitcher(dwDeltaTime);
		CECQuickBuyPopManager::Instance().Tick();
		RandMallShoppingManager::Instance().Tick();
	}

	return true;
}

//	Game render routine
bool CECGameRun::Render(bool bPresent)
{
	if (!m_pWorld)
		return true;
	
	//	转服等连续加载过程中，只显示过场动画
	if (CECContinueLoad::Instance().IsInMergeLoad() &&
		!CECLoginQueue::Instance().InQueue())
		return true;

	CECHostPlayer* pHostPlayer = m_pWorld ? m_pWorld->GetHostPlayer() : NULL;
	A3DEngine* pA3DEngine = g_pGame->GetA3DEngine();
	A3DDevice* pA3DDevice = g_pGame->GetA3DDevice();
	CECViewport* pViewport = g_pGame->GetViewport();

	if (GetGameState() == CECGameRun::GS_GAME && pHostPlayer && !pHostPlayer->HostIsReady())
	{
		if (!BeginRender())
		{
			a_LogOutput(1, "CECGameRun::Render(), Failed to begin render!");
			return false;
		}

		A3DDEVFMT devFmt = pA3DDevice->GetDevFormat();
		A3DVIEWPORTPARAM viewParam;
		viewParam.X = 0;
		viewParam.Y = 0;
		viewParam.Width  = devFmt.nWidth;
		viewParam.Height = devFmt.nHeight;
		viewParam.MinZ = 0.0f;
		viewParam.MaxZ = 1.0f;
		pA3DDevice->SetViewport(&viewParam);
		
		A3DCOLOR bgColor = 0x00000000;
		pA3DDevice->Clear(D3DCLEAR_TARGET, bgColor, 1.0f, 0);

		if (m_pUIManager)
			m_pUIManager->Render();

		EndRender(bPresent);
		return true;
	}

	CECUIManager *pUIManger = g_pGame->GetGameRun()->GetUIManager();
	bool bLogin = (pUIManger->GetCurrentUIMan() == CECUIManager::UIMAN_LOGIN);

	if (g_pGame->GetConfigs()->GetSkipFrameFlag() &&
		GetGameState() == CECGameRun::GS_GAME && l_fc.iAvgRdTime)
	{
		if (l_fc.iTickCnt < 10 &&
			l_fc.iTickTime + l_fc.iAvgRdTime > l_fc.iTickCnt * 150)
			return true;

		l_fc.iTickTime	= 0;
		l_fc.iTickCnt	= 0;
	}

	DWORD dwRenderTime = a_GetTime();

	if (!BeginRender())
	{
		a_LogOutput(1, "CECGameRun::Render(), Failed to begin render!");
		return false;
	}

	if( g_pGame->GetConfigs()->GetSystemSettings().bWideScreen )
	{
		A3DDEVFMT devFmt = pA3DDevice->GetDevFormat();
		A3DVIEWPORTPARAM * pMainView = g_pGame->GetViewport()->GetA3DViewport()->GetParam();
		
		// we must clear top and bottom bar to be pure black, otherwise, there maybe some random images
		A3DVIEWPORTPARAM viewParam;
		viewParam.X = 0;
		viewParam.Width  = devFmt.nWidth;
		viewParam.MinZ = 0.0f;
		viewParam.MaxZ = 1.0f;
		
		viewParam.Y = 0;
		viewParam.Height = pMainView->Y;
		pA3DDevice->SetViewport(&viewParam);
		pA3DDevice->Clear(D3DCLEAR_TARGET, 0x00000000, 1.0f, 0);
		
		viewParam.Y = pMainView->Y + pMainView->Height;
		viewParam.Height = devFmt.nHeight - viewParam.Y;
		pA3DDevice->SetViewport(&viewParam);
		pA3DDevice->Clear(D3DCLEAR_TARGET, 0x00000000, 1.0f, 0);
	}

	bool bFullGlowRender = false;
	if (m_pFullGlowRender)
	{
		bFullGlowRender = true;
		m_pFullGlowRender->BeginGlow();		
		if (GetGameState() == CECGameRun::GS_LOGIN){
			m_pFullGlowRender->SetGlowType(FULLGLOW_TYPE_LOGIN, 0xffffffff);
		}
		m_pFullGlowRender->Update(g_pGame->GetTickTime());
	}

	// don't clear color buffer because we may do motion blur effects.
	pViewport->Activate(false);
	g_pGame->GetA3DDevice()->Clear(D3DCLEAR_ZBUFFER, 0xff000000, 1.0f, 0);
	
	//	渲染开始前，准备记录高亮显示模型，以便恢复非高亮状态
	g_pGame->PrepareHighlightModel();
	
	//	Render world
	if (m_pWorld && m_pWorld->IsValid())
		m_pWorld->Render(pViewport);
	
	//	Render GFX
	g_pGame->GetGFXCaster()->Render(pViewport);
	pA3DEngine->RenderGFX(pViewport->GetA3DViewport(), 0xffffffff);
	
	if (g_pA3DConfig->RT_GetShowBoundBoxFlag())
		g_pGame->GetA3DGFXExMan()->RenderAllGfxAABB();
	
	g_pGame->GetA3DGFXExMan()->RenderAllSkinModels(pViewport->GetA3DViewport());
	A3DSkinRender* psr1 = g_pGame->GetSkinRender1();
	
	//	Render non alpha skin models
	psr1->Render(pViewport->GetA3DViewport(), false);
	psr1->ResetRenderInfo(false);
	psr1->RenderAlpha(pViewport->GetA3DViewport());
	psr1->ResetRenderInfo(true);
	
	g_pGame->GetA3DGFXExMan()->RenderAllGfx(pViewport->GetA3DViewport());
	
	//	Flush wire and flat collectors
	pA3DEngine->GetA3DWireCollector()->Flush();
	pA3DEngine->GetA3DFlatCollector()->Flush();
	
	if( bFullGlowRender)
	{
		m_pFullGlowRender->EndGlow(pViewport->GetA3DViewport());
	}
	
	//	Render all registered pate text
	if (m_pWorld && m_pWorld->GetDecalMan())
	{
		pA3DDevice->SetAlphaBlendEnable(false);
		pA3DDevice->SetAlphaTestEnable(true);
		m_pWorld->GetDecalMan()->RenderPateTexts(pViewport);
		pA3DDevice->SetAlphaBlendEnable(true);
		pA3DDevice->SetAlphaTestEnable(false);
	}
	
	//	Flush images
	g_pGame->GetImageRes()->PresentDraws();
	
	//	Render 2D gfx
	if( m_pUIManager->GetCurrentUIMan() == CECUIManager::UIMAN_INGAME &&
		m_pUIManager->GetInGameUIMan())
	{
		AfxGetGFXExMan()->Register2DGfx();
		AfxGetGFXExMan()->Render2DGfx(false);
	}		
	
	//	Render UI
	if (m_pUIManager)
	{
		g_pGame->GetA3DEngine()->FlushInternalSpriteBuffer();
		m_pUIManager->Render();
	}
	
	{
#ifdef SHOW_AUTOMOVE_FOOTPRINTS
		extern abase::vector<A3DVECTOR3> g_AutoPFFollowPoints;
		extern abase::vector<A3DVECTOR3> g_AutoPFPathPoints;
		if (!g_AutoPFFollowPoints.empty()){
			A3DStream * pStream = new A3DStream();
			A3DDevice *pA3DDevice = g_pGame->GetA3DDevice();
			if(pStream->Init(pA3DDevice, A3DVT_LVERTEX, g_AutoPFFollowPoints.size(), 0, A3DSTRM_REFERENCEPTR, 0)){
				pA3DDevice->ClearTexture(0);
				
				pA3DDevice->SetZWriteEnable(false);
				pA3DDevice->SetAlphaBlendEnable(false);
				pA3DDevice->SetPointSize(5.0f);
				pA3DDevice->SetLightingEnable(false);
				
				pStream->Appear();
				A3DLVERTEX * pVerts = NULL;
				A3DVECTOR3 deltaY(0.0f, 0.1f, 0.0f);
				if (!g_AutoPFFollowPoints.empty() && pStream->LockVertexBuffer(0, 0, (BYTE **)&pVerts, NULL)){
					int i(0);
					A3DCOLOR clrRed = A3DCOLORRGB(255, 0, 0);
					for (i = 0; i < (int)g_AutoPFFollowPoints.size(); ++ i)
					{
						pVerts[i] = A3DLVERTEX(g_AutoPFFollowPoints[i]+deltaY, clrRed, clrRed, 0, 0);
					}
					pStream->UnlockVertexBuffer();
					pA3DDevice->DrawPrimitive(A3DPT_POINTLIST, 0, g_AutoPFFollowPoints.size());
				}
				if (!g_AutoPFPathPoints.empty() && pStream->LockVertexBuffer(0, 0, (BYTE **)&pVerts, NULL)){
					int i(0);
					A3DCOLOR clrGreen = A3DCOLORRGB(0, 255, 0);
					for (i = 0; i < (int)g_AutoPFPathPoints.size(); ++ i)
					{
						pVerts[i] = A3DLVERTEX(g_AutoPFPathPoints[i]+deltaY, clrGreen, clrGreen, 0, 0);
					}
					pStream->UnlockVertexBuffer();
					pA3DDevice->DrawPrimitive(A3DPT_POINTLIST, 0, g_AutoPFPathPoints.size());
				}			
				
				pA3DDevice->SetZWriteEnable(true);
				pA3DDevice->SetAlphaBlendEnable(true);
				pA3DDevice->SetLightingEnable(true);
			}
			A3DRELEASE(pStream);
		}
#endif
	}
	
	// 在此处显示顶层gfx,不会被界面挡住
	g_pGame->GetGFXCaster()->RenderTop(pViewport);

#ifdef _SHOW_AUTOPOLICY_DEBUG
	CECAutoPolicy::GetInstance().Render();
#endif

	//	Render Run-Time debug information
	g_pGame->GetRTDebug()->Render(pViewport);

	//	Render statistic
	if (g_pGame->GetConfigs()->GetShowGameStatFlag())
		DisplayGameStatistic();

	EndRender(bPresent);
	
	//	恢复模型的非高亮显示状态
	g_pGame->RestoreHightlightModel();

	dwRenderTime = a_GetTime() - dwRenderTime;

	if (GetGameState() == CECGameRun::GS_GAME)
	{
		//	Calculate average render time
		l_fc.iRenderTime += (int)dwRenderTime;
		l_fc.iRenderCnt++;

		int iTotal = l_fc.iAvgRdTime ? 30 : 10;
		if (l_fc.iRenderCnt >= iTotal)
		{
			l_fc.iAvgRdTime	 = (int)((float)l_fc.iRenderTime / l_fc.iRenderCnt + 0.5f);
			l_fc.iRenderCnt  = 0;
			l_fc.iRenderTime = 0;
		}
	}

	// clear dynamic light list.
	GfxLightParamList& lightList = g_pGame->GetA3DGFXExMan()->GetLightParamList();
	g_pGame->GetA3DGFXExMan()->LockLightParamList();
	lightList.clear();
	g_pGame->GetA3DGFXExMan()->UnlockLightParamList();
	return true;
}

//	Begin render
bool CECGameRun::BeginRender()
{
	A3DEngine* pA3DEngine = g_pGame->GetA3DEngine();

	if (!pA3DEngine->BeginRender())
		return false;

	m_dwRenderTime = a_GetTime();

	return true;
}

//	End render
bool CECGameRun::EndRender(bool bPresent)
{
	A3DEngine* pA3DEngine = g_pGame->GetA3DEngine();

	A3DSkinRender* psr = g_pGame->GetSkinRender1();
	if (psr)
	{
		psr->ResetRenderInfo(false);
		psr->ResetRenderInfo(true);
	}

	if ((psr = g_pGame->GetSkinRender2()))
	{
		psr->ResetRenderInfo(false);
		psr->ResetRenderInfo(true);
	}

	if (!pA3DEngine->EndRender())
		return false;

	if( bPresent )
		pA3DEngine->Present();

	m_dwRenderTime = a_GetTime() - m_dwRenderTime;
	l_Statistic.iRenderTime = (int)m_dwRenderTime;

	return true;
}

//	Set logout flag
void CECGameRun::SetLogoutFlag(int iFlag)
{
	m_iLogoutFlag = iFlag;
}

//	Logout
void CECGameRun::Logout()
{
	ASSERT(m_iGameState == GS_GAME);

	overlay::GTOverlay::Instance().Logout();
	CECCrossServer::Instance().OnLogout();

	bool bExitApp = false;

	if (CECUIConfig::Instance().GetLoginUI().bAvoidLoginUI && m_iLogoutFlag != 1){
		bExitApp = true;
	}else if (m_iLogoutFlag == 0)		//	Exit application directly
	{
		bExitApp = true;
	}
	else if (m_iLogoutFlag == 1)	//	Logout game and re-select role
	{
		StartLogin();

		// 下载器响应退出游戏状态
		if( g_pGame->GetConfigs()->IsMiniClient() )
			CECMCDownload::GetInstance().SendSwitchGame(false);
		
		//	Goto select role interface directly
		CECLoginUIMan* pLoginUIMan = m_pUIManager->GetLoginUIMan();
		if (pLoginUIMan)
		{
			if(GetSellingRoleID() == 0)
			{
				pLoginUIMan->LaunchCharacter();
			}
			
			g_pGame->GetGameSession()->ReLogin(true);
			pLoginUIMan->SetRoleListReady(false);
			if (!CECReconnect::Instance().IsReconnecting()){
				CECReconnect::Instance().SetRoleID(0);
			}
		}
		else
		{
			ASSERT(pLoginUIMan);
			bExitApp = true;
		}
	}
	else if (m_iLogoutFlag == 2)	//	Logout game and goto login state
	{
		StartLogin();
		
		if (CECLoginUIMan* pLoginUIMan = m_pUIManager->GetLoginUIMan()){
			g_pGame->GetGameRun()->SetSellingRoleID(0);
			g_pGame->GetGameSession()->ReLogin(false);
			if (CECCrossServer::Instance().IsWaitLogin()){
				pLoginUIMan->LaunchCharacter();
				pLoginUIMan->ChangeSceneByRole();
				pLoginUIMan->ReclickLoginButton();
			}else if (CECReconnect::Instance().IsReconnecting()){
				pLoginUIMan->ChangeCameraByScene(CECLoginUIMan::LOGIN_SCENE_SELCHAR);
				pLoginUIMan->ReclickLoginButton();
			}
		}
		else
		{
			ASSERT(NULL);
			bExitApp = true;
		}
	}
	
	if (m_pRandomMapProc)
		A3DRELEASE(m_pRandomMapProc);

	if (bExitApp)
	{
		//	Exit game application
		EndGameState(false);
		::PostMessage(g_pGame->GetGameInit().hWnd, WM_QUIT, 0, 0);
	}
}

//	Save necessary user configs (UI, shortcut, accelerate keys, etc.) to server
//	返回值: 0(保存失败), 1(数据无需重复保存), 2(已向服务器发送协议)
DWORD CECGameRun::SaveConfigsToServer()
{
	if (!m_pWorld || 
		!m_pWorld->GetHostPlayer() || !m_pWorld->GetHostPlayer()->HostIsReady() || 
		!m_pUIManager->GetInGameUIMan())
		return 0;

	g_pGame->GetConfigs()->SaveBlockedID();

	int iTotalSize = 0;

	//	Version
	iTotalSize += sizeof (DWORD);

	//	Calculate data buffer size at first
	iTotalSize += sizeof (int);
	int iHostSize = 0;
	CECHostPlayer* pHost = m_pWorld->GetHostPlayer();
	pHost->SaveConfigData(NULL, &iHostSize);
	iTotalSize += iHostSize;

	iTotalSize += sizeof (int);
	DWORD dwUISize = 0;
	CECGameUIMan* pGameUI = (CECGameUIMan*)m_pUIManager->GetInGameUIMan();
	pGameUI->GetUserLayout(NULL, dwUISize);
	iTotalSize += (int)dwUISize;

	iTotalSize += sizeof (int);
	int iSettingSize = 0;
	g_pGame->GetConfigs()->SaveUserConfigData(NULL, &iSettingSize);
	iTotalSize += iSettingSize;

	//	Allocate data buffer
	void* pDataBuf = a_malloctemp(iTotalSize);
	if (!pDataBuf)
	{
		glb_ErrorOutput(ECERR_NOTENOUGHMEMORY, "CECGameRun::SaveConfigsToServer", __LINE__);
		return 0;
	}

	BYTE* pData = (BYTE*)pDataBuf;

	//	Save version
	*((DWORD*)pData) = USERCFG_VERSION;
	pData += sizeof (DWORD);

	//	Save host config data
	*((int*)pData) = iHostSize;
	pData += sizeof (int);
	pHost->SaveConfigData(pData, &iHostSize);
	pData += iHostSize;

	//	Save UI config data
	*((int*)pData) = (int)dwUISize;
	pData += sizeof (int);
	pGameUI->GetUserLayout(pData, dwUISize);
	pData += dwUISize;

	//	Save user setting data
	*((int*)pData) = iSettingSize;
	pData += sizeof (int);
	g_pGame->GetConfigs()->SaveUserConfigData(pData, &iSettingSize);
	pData += iSettingSize;

	//	Here we make a cache. If the data will be sent is same as last
	//	time, we can ignore it.
	if (m_pCfgDataBuf)
	{
		if (m_iCfgDataSize == iTotalSize && !memcmp(m_pCfgDataBuf, pDataBuf, iTotalSize))
		{
			//	Data of these 2 times is identical
			a_freetemp(pDataBuf);
			return 1;
		}

		//	Data isn't identical
		a_free(m_pCfgDataBuf);
		m_pCfgDataBuf	= NULL;
		m_iCfgDataSize	= 0;
	}

	//	Copy data of this time in order to compare next time
	if ((m_pCfgDataBuf = (BYTE*)a_malloc(iTotalSize)))
	{
		memcpy(m_pCfgDataBuf, pDataBuf, iTotalSize);
		m_iCfgDataSize = iTotalSize;
	}

	//	Compress config data
	DWORD dwCompLen = iTotalSize * 2;
	void* pCompBuf = a_malloctemp(dwCompLen);
	if (!pCompBuf)
	{
		a_freetemp(pDataBuf);
		glb_ErrorOutput(ECERR_NOTENOUGHMEMORY, "CECGameRun::SaveConfigsToServer", __LINE__);
		return 0;
	}

	//	Don't compress version info, directly copy it.
	int iVerLen = sizeof (DWORD);
	memcpy(pCompBuf, pDataBuf, iVerLen);
	BYTE* pSrc = (BYTE*)pDataBuf + iVerLen;
	BYTE* pDst = (BYTE*)pCompBuf + iVerLen;
	dwCompLen -= iVerLen;
	int iRes = AFilePackage::Compress(pSrc, iTotalSize - iVerLen, pDst, &dwCompLen);
	if (0 == iRes)
	{
		// 压缩成功，发送往服务器
		g_pGame->GetGameSession()->SaveConfigData(pCompBuf, dwCompLen+iVerLen);
		iRes = 2;
	}
	else
	{
		// 压缩失败，输出原因
		a_LogOutput(1, "CECGameRun::SaveConfigsToServer, Failed to compress config data (%d:%d)", iRes, iTotalSize);
		iRes = 0;
	}

	a_freetemp(pDataBuf);
	a_freetemp(pCompBuf);

	return iRes;
}

//	Load necessary user configs (UI, shortcut, accelerate keys) from server
bool CECGameRun::LoadConfigsFromServer(const void* pDataBuf, int iDataSize)
{
	if (!pDataBuf || !iDataSize)
	{
		a_LogOutput(1, "CECGameRun::LoadConfigsFromServer, configs data is empty");
		return false;
	}

	ASSERT(m_pWorld && m_pWorld->GetHostPlayer() && m_pUIManager->GetInGameUIMan());

	BYTE* pData = (BYTE*)pDataBuf;

	//	Read version
	DWORD dwVer = *(DWORD*)pData;
	pData += sizeof (DWORD);
	if (dwVer > USERCFG_VERSION)
	{
		ASSERT(dwVer > USERCFG_VERSION);
		return false;
	}

	void* pUncompBuf = NULL;
	DWORD dwRealLen = iDataSize - sizeof (DWORD);

	if (dwVer >= 3)
	{
		//	Uncompress config data
		dwRealLen = 4096;
		if (!(pUncompBuf = a_malloctemp(dwRealLen)))
		{
			glb_ErrorOutput(ECERR_NOTENOUGHMEMORY, "CECGameRun::SaveConfigsToServer", __LINE__);
			return false;
		}

		int iRes = AFilePackage::Uncompress(pData, iDataSize-sizeof (DWORD), (BYTE*)pUncompBuf, &dwRealLen);
		if (0 != iRes)
		{
			// 解压失败，输出返回
			a_LogOutput(1, "CECGameRun::LoadConfigsFromServer, Failed to load uncompress configs data (%d:%d)", iRes, iDataSize);
			a_freetemp(pUncompBuf);
			return false;
		}
		pData = (BYTE*)pUncompBuf;
	}

	try
	{
		CECDataReader dr(pData, (int)dwRealLen);

		//	Load host configs ...
		CECHostPlayer* pHost = m_pWorld->GetHostPlayer();
		int iSize = dr.Read_int();
		if (!pHost->LoadConfigData(dr.Read_Data(iSize)))
		{
			if (pUncompBuf)
				a_freetemp(pUncompBuf);

			a_LogOutput(1, "CECGameRun::LoadConfigsFromServer, Failed to load host configs ...");
			return false;
		}

		//	Load UI configs ...
		CECGameUIMan* pGameUI = m_pUIManager->GetInGameUIMan();
		iSize = dr.Read_int();
		if (!pGameUI->SetUserLayout(dr.Read_Data(iSize), iSize))
		{
			if (pUncompBuf)
				a_freetemp(pUncompBuf);

			a_LogOutput(1, "CECGameRun::LoadConfigsFromServer, Failed to set user layout ...");
			return false;
		}

		//	Load user settings ...
		if (dwVer >= 2)
		{
			iSize = dr.Read_int();
			if(!g_pGame->GetConfigs()->LoadUserConfigData(dr.Read_Data(iSize), iSize))
			{
				if (pUncompBuf)
					a_freetemp(pUncompBuf);
				
				a_LogOutput(1, "CECGameRun::LoadConfigsFromServer, Failed to load user config data ...");
				return false;
			}
		}
	}
	catch (CECException& e)
	{
		ASSERT(0);
		a_LogOutput(1, "CECGameRun::LoadConfigsFromServer, data read error (%d)", e.GetType());

		if (pUncompBuf)
			a_freetemp(pUncompBuf);

		return false;
	}

	if (pUncompBuf)
		a_freetemp(pUncompBuf);

	return true;
}

//	Add fixed format message to chat window
void CECGameRun::AddFixedMessage(int iMsg, ...)
{
	CECStringTab* pStrTab = g_pGame->GetFixedMsgTab();
	const wchar_t* szFixMsg = pStrTab->GetWideString(iMsg);
	if (!szFixMsg)
		return;

	ACHAR szBuf[1024];

	va_list vaList;
	va_start(vaList, iMsg);
	glb_vsnprintf(szBuf, sizeof(szBuf)/sizeof(szBuf[0]), szFixMsg, vaList);
	va_end(vaList);

	CECGameUIMan* pGameUI = m_pUIManager->GetInGameUIMan();
	if (pGameUI)
		pGameUI->AddChatMessage(szBuf, GP_CHAT_SYSTEM);
	else
		g_pGame->RuntimeDebugInfo(RTDCOL_CHATLOCAL, szBuf);
}

void CECGameRun::AddFixedChannelMsg(int iMsg, char cChannel, ...)
{
	CECStringTab* pStrTab = g_pGame->GetFixedMsgTab();
	const wchar_t* szFixMsg = pStrTab->GetWideString(iMsg);
	if (!szFixMsg)
		return;

	ACHAR szBuf[1024];

	va_list vaList;
	va_start(vaList, cChannel);
	glb_vsnprintf(szBuf, sizeof(szBuf)/sizeof(szBuf[0]), szFixMsg, vaList);
	va_end(vaList);

	CECGameUIMan* pGameUI = m_pUIManager->GetInGameUIMan();
	if (pGameUI)
		pGameUI->AddChatMessage(szBuf, cChannel);
	else
		g_pGame->RuntimeDebugInfo(RTDCOL_CHATLOCAL, szBuf);
}

//	Add message to chat window
void CECGameRun::AddChatMessage(const ACHAR *pszMsg, char cChannel, int idPlayer/* -1 */,
								const ACHAR* szName/* NULL */, BYTE byFlag, char cEmotion,
								CECIvtrItem *pItem /* NULL */,const ACHAR *pszMsgOrigion/* NULL */)
{
	CECGameUIMan* pGameUI = m_pUIManager->GetInGameUIMan();
	if (pGameUI){
		pGameUI->AddChatMessage(pszMsg, cChannel, idPlayer, szName, byFlag, cEmotion, pItem,pszMsgOrigion);
	}else{
		g_pGame->RuntimeDebugInfo(RTDCOL_CHATLOCAL, pszMsg);
		delete pItem;
	}
}

//	Set / Get selected character's role info.
const GNET::RoleInfo& CECGameRun::GetSelectedRoleInfo()
{
	return l_SelRoleInfo;
}

void CECGameRun::SetSelectedRoleInfo(const GNET::RoleInfo& Info)
{
	l_SelRoleInfo = Info;
}

const GNET::RoleInfo& CECGameRun::GetRedirectLoginPosRoleInfo()
{
	return l_RedirectLoginPosRoleInfo;
}

void CECGameRun::SetRedirectLoginPosRoleInfo(const GNET::RoleInfo& Info)
{
	l_RedirectLoginPosRoleInfo = Info;
}

//	Display game statistic
void CECGameRun::DisplayGameStatistic()
{
	if (!m_pWorld)
		return;

	if (l_StatCnt.IsFull())
	{
		//	Reset statistic...
		l_StatCnt.Reset();

		l_Statistic.iMeshSize		= g_pGame->GetA3DEngine()->GetA3DSkinMan()->GetSkinMeshMan()->CalcDataSize();
		l_Statistic.iAllTexSize		= g_pGame->GetA3DEngine()->GetA3DTextureMan()->GetTextureDataSize();
		l_Statistic.iStrmDSize		= A3DStream::m_iDynDataSize;
		l_Statistic.iStrmSSize		= A3DStream::m_iStaticDataSize;
		l_Statistic.iStrmBSize		= A3DStream::m_iBackupDataSize;
		l_Statistic.iTrackSize		= A3D::g_A3DTrackMan.GetTrackDataSize();
		l_Statistic.iTrnDataSize	= m_pWorld->GetTerrain() ? m_pWorld->GetTerrain()->GetBlockDataSize() : 0;
		l_Statistic.iSndCacheSize	= g_pGame->GetSoundCache()->GetCacheSize();

		l_Statistic.iNumPlayer		= m_pWorld->GetPlayerMan()->GetElsePlayerNum();
		l_Statistic.iNumNPC			= m_pWorld->GetNPCMan()->GetNPCNum();
		l_Statistic.iNumMatter		= m_pWorld->GetMatterMan()->GetMatterNum();
		l_Statistic.iNumScnGFX		= m_pWorld->GetOrnamentMan()->GetGFXNum();
		l_Statistic.iNumOnmt		= m_pWorld->GetOrnamentMan()->GetOrnamentNum();
		l_Statistic.iNumScnModel	= m_pWorld->GetOrnamentMan()->GetECModelNum();
		l_Statistic.iNumTrnBlock	= m_pWorld->GetTerrain() ? m_pWorld->GetTerrain()->GetRenderedBlockNum() : 0;

		if (g_pGame->GetSoundCache()->GetLoadFileCnt())
			l_Statistic.fSndCacheHit = g_pGame->GetSoundCache()->GetFileCacheHitCnt() * 100.0f / g_pGame->GetSoundCache()->GetLoadFileCnt();
		else
			l_Statistic.fSndCacheHit = 0.0f;

		l_Statistic.iMemPeak		= a_GetMemPeakSize();
		l_Statistic.dwMemAllocCnt	= a_GetMemAllocCounter();
		l_Statistic.iMemCurSize		= a_GetMemCurSize();
		l_Statistic.iMemRawSize		= a_GetMemRawSize();

		l_Statistic.iSMemAllocSize	= 0;
		l_Statistic.iSMemBlkCnt		= 0;
		for (int i=0; i < 16; i++)
		{
			a_GetSmallMemInfo(i, &l_Statistic.aSMemBlkSize[i], &l_Statistic.aSMemBlkCnt[i], &l_Statistic.aSMemFreeCnt[i]);
			l_Statistic.aSMemSize[i] = l_Statistic.aSMemBlkSize[i] * l_Statistic.aSMemBlkCnt[i];
			l_Statistic.iSMemAllocSize += l_Statistic.aSMemSize[i];
			l_Statistic.iSMemBlkCnt += l_Statistic.aSMemBlkCnt[i];
		}

		a_GetLargeMemInfo(&l_Statistic.iLMemBlkCnt, &l_Statistic.iLMemAllocSize);
	}

	A3DFont* pA3DFont = g_pGame->GetA3DEngine()->GetSystemFont();
	ACHAR szMsg[256];
	int x, y, i;
	DWORD dwCol;

	//	Data size counter ...
	dwCol = 0xffff8040;
	x = 50;
	y = 100;
	a_sprintf(szMsg, _AL("Skin Mesh Size: %.2f K"), l_Statistic.iMeshSize / 1024.0f);
	pA3DFont->TextOut(x, y, szMsg, dwCol);
	y += 16;

	a_sprintf(szMsg, _AL("All Texture Size: %.2f K"), l_Statistic.iAllTexSize / 1024.0f);
	pA3DFont->TextOut(x, y, szMsg, dwCol);
	y += 16;

	a_sprintf(szMsg, _AL("Stream (D) Size: %.2f K"), l_Statistic.iStrmDSize / 1024.0f);
	pA3DFont->TextOut(x, y, szMsg, dwCol);
	y += 16;

	a_sprintf(szMsg, _AL("Stream (S) Size: %.2f K"), l_Statistic.iStrmSSize / 1024.0f);
	pA3DFont->TextOut(x, y, szMsg, dwCol);
	y += 16;

	a_sprintf(szMsg, _AL("Stream (B) Size: %.2f K"), l_Statistic.iStrmBSize / 1024.0f);
	pA3DFont->TextOut(x, y, szMsg, dwCol);
	y += 16;

	a_sprintf(szMsg, _AL("Track Size: %.2f K"), l_Statistic.iTrackSize / 1024.0f);
	pA3DFont->TextOut(x, y, szMsg, dwCol);
	y += 16;

	a_sprintf(szMsg, _AL("Terrain Size: %.2f K"), l_Statistic.iTrnDataSize / 1024.0f);
	pA3DFont->TextOut(x, y, szMsg, dwCol);
	y += 16;

	a_sprintf(szMsg, _AL("Sound Cache: %.2f K"), l_Statistic.iSndCacheSize / 1024.0f);
	pA3DFont->TextOut(x, y, szMsg, dwCol);
	y += 16;

	//	Object counter ...
	dwCol = 0xff00ffff;
	x = 270;
	y = 100;
	a_sprintf(szMsg, _AL("player num: %d"), l_Statistic.iNumPlayer);
	pA3DFont->TextOut(x, y, szMsg, dwCol);
	y += 16;

	a_sprintf(szMsg, _AL("NPC num: %d"), l_Statistic.iNumNPC);
	pA3DFont->TextOut(x, y, szMsg, dwCol);
	y += 16;

	a_sprintf(szMsg, _AL("matter num: %d"), l_Statistic.iNumMatter);
	pA3DFont->TextOut(x, y, szMsg, dwCol);
	y += 16;

	a_sprintf(szMsg, _AL("scene GFX num: %d"), l_Statistic.iNumScnGFX);
	pA3DFont->TextOut(x, y, szMsg, dwCol);
	y += 16;

	a_sprintf(szMsg, _AL("building num: %d"), l_Statistic.iNumOnmt);
	pA3DFont->TextOut(x, y, szMsg, dwCol);
	y += 16;

	a_sprintf(szMsg, _AL("scene model num: %d"), l_Statistic.iNumScnModel);
	pA3DFont->TextOut(x, y, szMsg, dwCol);
	y += 16;

	a_sprintf(szMsg, _AL("snd cache hit: %.1f%%"), l_Statistic.fSndCacheHit);
	pA3DFont->TextOut(x, y, szMsg, dwCol);
	y += 16;

	a_sprintf(szMsg, _AL("vis trn block: %d"), l_Statistic.iNumTrnBlock);
	pA3DFont->TextOut(x, y, szMsg, dwCol);
	y += 16;

	//	Time counter ...
	dwCol = 0xffff00ff;
	y += 100;
	a_sprintf(szMsg, _AL("tick time: %d"), l_Statistic.iTickTime);
	pA3DFont->TextOut(x, y, szMsg, dwCol);
	y += 16;

	a_sprintf(szMsg, _AL("render time: %d"), l_Statistic.iRenderTime);
	pA3DFont->TextOut(x, y, szMsg, dwCol);
	y += 16;

	//	Gfx Stat
	x = 50;
	y = 300;
	a_sprintf(szMsg, _AL("gfx tick time: %.2f"), AfxGetGFXExMan()->GetTotalTickTime());
	pA3DFont->TextOut(x, y, szMsg, dwCol);
	y += 16;

	a_sprintf(szMsg, _AL("gfx render time: %.2f"), AfxGetGFXExMan()->GetTotalRenderTime());
	pA3DFont->TextOut(x, y, szMsg, dwCol);
	y += 16;

	a_sprintf(szMsg, _AL("gfx total count: %d"), AfxGetGFXExMan()->GetTotalGfxCount());
	pA3DFont->TextOut(x, y, szMsg, dwCol);
	y += 16;

	a_sprintf(szMsg, _AL("gfx render count: %d"), AfxGetGFXExMan()->GetTotalGfxRenderCount());
	pA3DFont->TextOut(x, y, szMsg, dwCol);
	y += 16;
	
	a_sprintf(szMsg, _AL("gfx draw count: %d"), AfxGetGFXExMan()->GetTotalDrawCount());
	pA3DFont->TextOut(x, y, szMsg, dwCol);
	y += 16;

	a_sprintf(szMsg, _AL("gfx prim count: %d"), AfxGetGFXExMan()->GetTotalPrimCount());
	pA3DFont->TextOut(x, y, szMsg, dwCol);
	y += 16;

	a_sprintf(szMsg, _AL("par total count: %d"), AfxGetGFXExMan()->GetParticleTotalCount());
	pA3DFont->TextOut(x, y, szMsg, dwCol);
	y += 16;

	a_sprintf(szMsg, _AL("par render count: %d"), AfxGetGFXExMan()->GetParticleRenderCount());
	pA3DFont->TextOut(x, y, szMsg, dwCol);
	y += 16;

	//	Memory statistics ...
	dwCol = 0xffffff00;
	x = 440;
	y = 100;
	a_sprintf(szMsg, _AL("mem peak: %.2f K"), l_Statistic.iMemPeak / 1024.0f);
	pA3DFont->TextOut(x, y, szMsg, dwCol);
	y += 16;

	a_sprintf(szMsg, _AL("mem alloc cnt: %u"), l_Statistic.dwMemAllocCnt);
	pA3DFont->TextOut(x, y, szMsg, dwCol);
	y += 16;

	a_sprintf(szMsg, _AL("mem size: %.2f K"), l_Statistic.iMemCurSize / 1024.0f);
	pA3DFont->TextOut(x, y, szMsg, dwCol);
	y += 16;

	a_sprintf(szMsg, _AL("mem raw size: %.2f K"), l_Statistic.iMemRawSize / 1024.0f);
	pA3DFont->TextOut(x, y, szMsg, dwCol);
	y += 16;

	a_sprintf(szMsg, _AL("mem(S) blk cnt: %d"), l_Statistic.iSMemBlkCnt);
	pA3DFont->TextOut(x, y, szMsg, dwCol);
	y += 16;

	a_sprintf(szMsg, _AL("mem(S) size: %.2f K"), l_Statistic.iSMemAllocSize / 1024.0f);
	pA3DFont->TextOut(x, y, szMsg, dwCol);
	y += 16;

	a_sprintf(szMsg, _AL("mem(L) blk cnt: %d"), l_Statistic.iLMemBlkCnt);
	pA3DFont->TextOut(x, y, szMsg, dwCol);
	y += 16;

	a_sprintf(szMsg, _AL("mem(L) size: %.2f K"), l_Statistic.iLMemAllocSize / 1024.0f);
	pA3DFont->TextOut(x, y, szMsg, dwCol);
	y += 16;

	y += 16;
	for (i=0; i < 16; i++)
	{
		a_sprintf(szMsg, _AL("mem(S) [%d]: %d, %d, %d, %.2f K"), i, l_Statistic.aSMemBlkSize[i], 
			l_Statistic.aSMemBlkCnt[i], l_Statistic.aSMemFreeCnt[i], l_Statistic.aSMemSize[i] / 1024.0f);
		pA3DFont->TextOut(x, y, szMsg, dwCol);
		y += 16;
	}

	// sound load count
	dwCol = 0xff00ff00;
	x = 640;
	y = 100;
	a_sprintf(szMsg, _AL("snd always: %d"), g_pGame->GetSoundCache()->GetLoadCount(enumSoundLoadAlways));
	pA3DFont->TextOut(x, y, szMsg, dwCol);
	y += 16;

	a_sprintf(szMsg, _AL("snd noloop: %d"), g_pGame->GetSoundCache()->GetLoadCount(enumSoundLoadNonLoop));
	pA3DFont->TextOut(x, y, szMsg, dwCol);
	y += 16;

	a_sprintf(szMsg, _AL("snd loop  : %d"), g_pGame->GetSoundCache()->GetLoadCount(enumSoundLoadLoop));
	pA3DFont->TextOut(x, y, szMsg, dwCol);
	y += 16;
}

//	Begin load progress
bool CECGameRun::BeginLoadProgress(float Min, float Max)
{
	using namespace overlay;
	if (GTOverlay::Instance().IsLogin())
		GTOverlay::Instance().Toggle(false);

	A3DRELEASE(m_pLoadProgress);

	if (!(m_pLoadProgress = new CECLoadProgress(IsFirstShow() && CECUIConfig::Instance().GetLoginUI().bSkipFirstLoadProgress)))
		return false;

	if( !m_pLoadProgress->Init(Min, Max) )
		return false;

	return true;
}

//	End load progress
void CECGameRun::EndLoadProgress()
{
	A3DRELEASE(m_pLoadProgress);

	using namespace overlay;
	if (GTOverlay::Instance().IsLogin())
		GTOverlay::Instance().Toggle(true);
}

//	Step load progress
void CECGameRun::StepLoadProgress(float Step)
{
	if (m_pLoadProgress)
	{
		m_pLoadProgress->OffsetProgressPos(Step);
		m_pLoadProgress->Render(false);
		g_pGame->DispatchWindowsMessage();
	}
}

//	Get host player's position or a candidate position
A3DVECTOR3 CECGameRun::GetSafeHostPos(CECViewport* pViewport)
{
	if (m_pHostPlayer && m_pHostPlayer->HostIsReady())
		return m_pHostPlayer->GetPos();
	else
	{
		//	Use camera's position to imitate host's position
		if (!pViewport)
			pViewport = g_pGame->GetViewport();

		return pViewport->GetA3DCamera()->GetPos();
	}
}

//	Get player name
const ACHAR* CECGameRun::GetPlayerName(int cid, bool bAutoGen)
{
	ID2NameTable::pair_type Pair = m_ID2NameTab.get(cid);
	if (Pair.second)
		return **Pair.first;

	if (bAutoGen)
	{
		m_strAutoName.Format(_AL("P-%d"), cid);
		return m_strAutoName;
	}

	return NULL;
}

//	Get player ID by name
int CECGameRun::GetPlayerID(const ACHAR* szName)
{
	Name2IDTable::pair_type Pair = m_Name2IDTab.get(szName);
	return Pair.second ? *Pair.first : 0;
}

//	Add player name to table
void CECGameRun::AddPlayerName(int cid, const ACHAR* szName, bool bOverwrite/* false */)
{
	if(!cid)
	{
		ASSERT(false);
		a_LogOutput(1, "CECGameRun::AddPlayerName, Invalid cid(=0)!");
		return;
	}

	ACString strName = szName;
	if (strName.IsEmpty())
	{
		ASSERT(false);
		a_LogOutput(1, "CECGameRun::AddPlayerName, Invalid name for cid=%d!", cid);
		return;
	}

	Name2IDTable::pair_type Pair2 = m_Name2IDTab.get(strName);
	if (!Pair2.second)
		m_Name2IDTab.put(strName, cid);
	else
		*Pair2.first = cid;
	
	ID2NameTable::pair_type Pair = m_ID2NameTab.get(cid);
	if (Pair.second)
	{		
		if (bOverwrite)
		{
			//	Name exists, replace it
			ACString* pstrName = *Pair.first;
			*pstrName = strName;
		}
		
		return;
	}
	
	//	Create new name
	ACString* pstrName = new ACString(strName);
	m_ID2NameTab.put(cid, pstrName);
}

//	Get instance by ID
CECInstance* CECGameRun::GetInstance(int id)
{
	InstTable::pair_type Pair = m_InstTab.get(id);
	return Pair.second ? *Pair.first : NULL;
}

CECInstance* CECGameRun::GetInstance(const ACHAR* szName)
{	//	Release all instance information
	InstTable::iterator it = m_InstTab.begin();
	for (; it != m_InstTab.end(); ++it)
	{
		CECInstance* pInst = *it.value();
		if(AS2AC(pInst->GetPath()).CompareNoCase(szName) == 0)
		{
			return pInst;
		}
	}

	return NULL;
}

//	Load instance information from file
bool CECGameRun::LoadInstanceInfo(const char* szFile)
{
	AWScriptFile sf;
	if (!sf.Open(szFile))
	{
		a_LogOutput(1, "CECGameRun::LoadInstanceInfo, Failed to load %s", szFile);
		return false;
	}

	while (sf.PeekNextToken(true))
	{
		CECInstance* pInst = new CECInstance;
		if (!pInst || !pInst->Load(&sf))
		{
			a_LogOutput(1, "CECGameRun::LoadInstanceInfo, Failed to read %s near line:%d", szFile, sf.GetCurLine());
			return false;
		}

		if (!m_InstTab.put(pInst->GetID(), pInst))
		{
			//	ID collsion ?
			ASSERT(0);
			delete pInst;
		}
	}

	sf.Close();

	return true;
}

//	Jump to specified position in a instance
bool CECGameRun::JumpToInstance(int idInst, const A3DVECTOR3& vPos, int iParallelWorldID)
{
	CECInstance* pInst = GetInstance(idInst);
	if (!pInst)
	{
		CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
		if( pGameUI )
			pGameUI->ShowErrorMsg(pGameUI->GetStringFromTable(10700));

		g_pGame->GetGameSession()->SetBreakLinkFlag(CECGameSession::LBR_MAP_INVALID);
		a_LogOutput(1, "CECGameRun::JumpToInstance, wrong instance id: %d", idInst);
		return false;
	}

	if (m_pWorld)
	{
		if (m_pWorld->GetInstanceID() == pInst->GetID() && m_pWorld->IsValid())
		{
			int iLast = m_pWorld->GetCurParallelWorld();
			if (iLast == iParallelWorldID){
				m_pWorld->SetLoadCenter(vPos);
			}else{
				m_pWorld->OnParallelWorldChange(vPos, iParallelWorldID);
				CECUIHelper::UpdateParallelWorld();
				if (iLast != 0){
					CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
					if (pGameUI){
						ACString strMsg;
						strMsg.Format(pGameUI->GetStringFromTable(10703), pInst->GetName(), iParallelWorldID);
						CECUIHelper::AddHeartBeatHint(strMsg);
					}
				}
			}
			return true;
		}
		else
		{
			//	Release current world
			ReleaseWorld();
		}
	}

	//	Create new world
	if (!CreateWorld(idInst, vPos, iParallelWorldID))
	{
		a_LogOutput(1, "CECGameRun::JumpToInstance, failed to create world %d", idInst);
		g_dwFatalErrorFlag = FATAL_ERROR_LOAD_BUILDING;
		return false;
	}

	CECQuickBuyPopManager::Instance().Reset();

	if (m_pHostPlayer)
	{
		m_pHostPlayer->SetPlayerMan(m_pWorld->GetPlayerMan());
		m_pHostPlayer->OnJumpInstance();
	}

	CECGameUIMan* pGameUIMan = m_pUIManager->GetInGameUIMan();

	if (pGameUIMan)
	{
		pGameUIMan->ChangeWorldInstance(idInst);
		A3DDevice *pA3DDevice = g_pGame->GetA3DDevice();
		pGameUIMan->SetHideIme(!pA3DDevice->GetDevFormat().bWindowed);
	}

	ActivateDefInput();

	return true;
}

void CECGameRun::ActivateDefInput()
{
	m_pInputCtrl->ClearKBFilterStack();
	m_pInputCtrl->ClearMouFilterStack();
	m_pInputCtrl->ActivateKBFilter(m_pInputFilter);
	m_pInputCtrl->ActivateMouseFilter(m_pInputFilter);
}

// void CECGameRun::ActivateAutoHomeInput()
// {
// 	m_pInputCtrl->ClearKBFilterStack();
// 	m_pInputCtrl->ClearMouFilterStack();
// 	m_pInputCtrl->ActivateKBFilter(m_pAutoHomeFilter);
// 	m_pInputCtrl->ActivateMouseFilter(m_pAutoHomeFilter);
// }

// test code
A3DVECTOR3 _org_pos(0);

//	Process message
bool CECGameRun::ProcessMessage(const ECMSG& Msg)
{
	ASSERT(Msg.iManager < 0);

	switch (Msg.dwMsg)
	{
	// test code
	case MSG_ENTERAUTOHOME:
// 		if (m_pWorld)
// 		{
// 			_org_pos = GetHostPlayer()->GetPos();
// 			ActivateAutoHomeInput();
// 			m_pWorld->EnterAutoHome();
// 			CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
// 			pGameUI->ChangeAutoHome(true);
// 		}

		break;

	case MSG_LEAVEAUTOHOME:

// 		ReleaseWorld();
// 		JumpToInstance(1, _org_pos);
// 
// 		{
// 			CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
// 			pGameUI->ChangeAutoHome(false);
// 		}

		break;

/*	case 112:	//	test code added by dyx
	{
		m_pWorld->GetAutoScene()->ExportGameData();
		break;
	}
*/	case MSG_EXITGAME:
	
		//	Exit game
		overlay::GTOverlay::Instance().Logout();
		::PostMessage(g_pGame->GetGameInit().hWnd, WM_QUIT, 0, 0);
		break;

	case MSG_CLOSEALLDIALOG:
		
		break;

	case MSG_SERVERTIME:

		g_pGame->SetServerTime((int)Msg.dwParam1, (int)Msg.dwParam2);
		l_QueryServerTime.Reset();
		break;
		
	case MSG_INSTCHECKOUT:
	{
		const S2C::cmd_inst_data_checkout* pCmd = (const S2C::cmd_inst_data_checkout*)Msg.dwParam1;
		m_nGShopTimeStamp = pCmd->gshop_time_stamp;

		if (m_nGShopTimeStamp != globaldata_getgshop_timestamp())
		{
			AddFixedMessage(FIXMSG_ERR_GSHOPDATA);
			a_LogOutput(1, "gshop timestamp error:server(0x%x) != local(0x%x)", m_nGShopTimeStamp, globaldata_getgshop_timestamp());
		}

		//for test
 		m_nGShopTimeStamp2 = pCmd->gshop_time_stamp2;
 
 		if (m_nGShopTimeStamp2 != globaldata_getgshop_timestamp2())
 		{
			AddFixedMessage(FIXMSG_ERR_GSHOPDATA);
			a_LogOutput(1, "gshop1 timestamp error:server(0x%x) != local(0x%x)", m_nGShopTimeStamp2, globaldata_getgshop_timestamp2());
		}

		if (!m_pWorld || !m_pWorld->CheckOutInst(pCmd->idInst, pCmd->region_time_stamp, pCmd->precinct_time_stamp))
		{
			AddFixedMessage(FIXMSG_ERR_INSTDATA);
		}

		break;
	}

	case MSG_MALLITEMINFO:
	{
		if(Msg.dwParam2 == S2C::MALL_ITEM_PRICE){
			const S2C::cmd_mall_item_price* pCmd = (const S2C::cmd_mall_item_price*)Msg.dwParam1;
			if (CECQShop::Instance().UpdateFromServer(pCmd)){
				CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
				((CDlgQShop*)(pGameUI->GetDialog("Win_QShop")))->UpdateQshopData();
			}
		}else if(Msg.dwParam2 == S2C::MALL_ITEM_BUY_FAILED){
			const S2C::cmd_mall_item_buy_failed* pCmd = (const S2C::cmd_mall_item_buy_failed*)Msg.dwParam1;
			CECQShop::Instance().GetFromServer(pCmd->index, pCmd->index+1);
		}
		break;
	}

	case MSG_DIVIDENDMALLITEMINFO:
	{
		if(Msg.dwParam2 == S2C::DIVIDEND_MALL_ITEM_PRICE){
			const S2C::cmd_dividend_mall_item_price* pCmd = (const S2C::cmd_dividend_mall_item_price*)Msg.dwParam1;
			if (CECBackShop::Instance().UpdateFromServer(pCmd)){
				CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
				((CDlgQShop*)(pGameUI->GetDialog("Win_BackShop")))->UpdateQshopData();
			}
		}else if(Msg.dwParam2 == S2C::DIVIDEND_MALL_ITEM_BUY_FAILED){
			const S2C::cmd_dividend_mall_item_buy_failed* pCmd = (const S2C::cmd_dividend_mall_item_buy_failed*)Msg.dwParam1;
			CECBackShop::Instance().GetFromServer(pCmd->index, pCmd->index+1);		
		}
		break;
	}
	
	case MSG_DOUBLETIME:
	{
		if (Msg.dwParam2 == S2C::DOUBLE_EXP_TIME)
		{
			const S2C::cmd_double_exp_time* pCmd = (const S2C::cmd_double_exp_time*)Msg.dwParam1;
			if (pCmd->mode)
			{
				m_iDblExpMode |= DBEXP_OWN;
				m_iDExpEndTime = pCmd->end_time;
			}
			else
				m_iDblExpMode &= ~DBEXP_OWN;
		}
		else if (Msg.dwParam2 == S2C::AVAILABLE_DOUBLE_EXP_TIME)
		{
			const S2C::cmd_available_double_exp_time* pCmd = (const S2C::cmd_available_double_exp_time*)Msg.dwParam1;
			m_iAvaDExpTime = pCmd->available_time;
		}

		break;
	}
	case MSG_UNIQUE_DATA:
	{
		S2C::cmd_unique_data_notify cmd;
		if(!cmd.Initialize((const void *)Msg.dwParam1,Msg.dwParam3))
			break;
		for (int i=0;i<cmd.count;i++)
		{
			unique_data* pUniqueData = new unique_data;
			pUniqueData->type = cmd.data[i].type;
			pUniqueData->_value.replace(cmd.data[i].pValue,cmd.data[i].len);

			UniqueDataMap::iterator itr = m_UniqueDataMap.find(cmd.data[i].key);
			if (itr!=m_UniqueDataMap.end())
			{
				delete itr->second;
			}
			
			m_UniqueDataMap[cmd.data[i].key] = pUniqueData;
		}
		CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
		CDlgMonsterSpirit* pDlg = dynamic_cast<CDlgMonsterSpirit*>(pGameUI->GetDialog("Win_MonsterSpirit"));
		if (pDlg && pDlg->IsShow()) pDlg->Update();
		
		break;
	}
	case MSG_COMMONDATA:
	{
		if (Msg.dwParam2 == S2C::COMMON_DATA_NOTIFY)
		{
			const S2C::common_data_notify::_node* pCmd = (const S2C::common_data_notify::_node*)Msg.dwParam1;
			int iSize  = Msg.dwParam3/sizeof(S2C::common_data_notify::_node);

			for(int i=0;i<iSize;i++)
			{
				CommonDataTable::pair_type Pair = m_CommonDataTab.get(pCmd->key);
				if (Pair.second)
				{
					long* pValue = Pair.first;
					*pValue = pCmd->value;
				}
				else
				{
					m_CommonDataTab.put(pCmd->key, pCmd->value);
				}

				pCmd++;
			}
		}
		else if (Msg.dwParam2 == S2C::COMMON_DATA_LIST)
		{
			m_CommonDataTab.clear();
			
			const S2C::common_data_list::_node* pCmd = (const S2C::common_data_list::_node*)Msg.dwParam1;
			int iSize = Msg.dwParam3/sizeof(S2C::common_data_list::_node);

			for(int i=0;i<iSize;i++)
			{
				m_CommonDataTab.put(pCmd->key, pCmd->value);
				pCmd++;
			}
		}
		break;
	}
	
	case MSG_PUBLICQUESTINFO:
	{
		if (Msg.dwParam2 == S2C::PUBLIC_QUEST_INFO)
		{
			const S2C::cmd_public_quest_info* pCmd = (const S2C::cmd_public_quest_info*)Msg.dwParam1;

			CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
			CDlgPQ* dlgPQ = (CDlgPQ*)(pGameUI->GetDialog("Win_PQtitle"));

			CDlgPQ::PQInfo pqInfo;
			pqInfo.task_id  = pCmd->task_id;
			pqInfo.sub_task_id = pCmd->child_task_id;
			pqInfo.score = pCmd->score;
			pqInfo.prof_place = pCmd->cls_place;
			pqInfo.all_place = pCmd->all_place;

			dlgPQ->UpdatePQ(pqInfo);
		}
		else if (Msg.dwParam2 == S2C::PUBLIC_QUEST_RANKS)
		{
			const S2C::cmd_public_quest_ranks* pCmd = (const S2C::cmd_public_quest_ranks*)Msg.dwParam1;

			CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
			CDlgPQ* dlgPQ = (CDlgPQ*)(pGameUI->GetDialog("Win_PQrank"));
			
			dlgPQ->UpdatePQPlayerNumber(pCmd->player_count);

			int iIndex = 0;
			for(int i=0;i<sizeof(pCmd->ranks_size)/sizeof(pCmd->ranks_size[0]);i++)
			{
				CDlgPQ::RankList rankList;

				for(int j=0; j<pCmd->ranks_size[i]; j++)
				{
					const S2C::cmd_public_quest_ranks::ranks_entry& re = (pCmd->list[iIndex]);
					
					CDlgPQ::RankInfo rankInfo;
					rankInfo.place		= re.place;
					rankInfo.roleid		= re.roleid;
					rankInfo.rand_score	= re.rand_score;	// 默认为-1
					rankInfo.score		= re.score;

					rankList.push_back(rankInfo);

					iIndex++;
				}

				dlgPQ->UpdateRank(i, rankList);
			}
		}
		break;		
	}

	case MSG_WORLDLIFETIME:
	{
			const S2C::cmd_world_life_time* pCmd = (const S2C::cmd_world_life_time*)Msg.dwParam1;
			if(pCmd)
			{
				GetUIManager()->GetInGameUIMan()->PopupWorldCountDown(pCmd->life_time);
			}
			break;
	}

	case MSG_CALCNETWORKDELAY:
		{
			const S2C::cmd_network_delay_re* pCmd = (const S2C::cmd_network_delay_re*)Msg.dwParam1;
			if (pCmd &&
				pCmd->timestamp == l_iDelayTimeStamp &&
				GetGameState() == GS_GAME)
			{
				int curTimeStamp = timeGetTime();
				m_iInGameDelay = curTimeStamp - l_iDelayTimeStamp;
				if (m_iInGameDelay < 0)
					m_iInGameDelay = 0;
			}
			break;
		}
	case MSG_PLAYERSPECITEMLIST:
		{
			CDlgGMQueryItem* pDlg = 
				(CDlgGMQueryItem*)GetUIManager()->GetInGameUIMan()->GetDialog("Win_GMQueryItem");
			if(pDlg)
			{
				pDlg->OnGMQueryResult(Msg.dwParam2, (void*)Msg.dwParam1, Msg.dwParam3);
				if(!pDlg->IsShow()) pDlg->Show(true);
			}
			break;
		}

	case MSG_PARALLEL_WORLD_INFO:
		if (m_pWorld){
			m_pWorld->ResetParallelWorld((void*)Msg.dwParam1);
			CECUIHelper::UpdateParallelWorld();
		}
		break;
	case MSG_RANDOM_MAP_ORDER:
		{
			S2C::cmd_random_map_order* order = (S2C::cmd_random_map_order*)Msg.dwParam1;
			if (g_pGame->GetGameRun())
			{
				g_pGame->GetGameRun()->OnRandomMap(order->row,order->col,order->room_order);

			/*	CECRandomMapProcess* pRandom = m_pWorld->GetRandomMapProc();
				if (pRandom)
				{
					

					int count = order->row * order->col;
					ASSERT(m_pWorld->GetTerrain()->GetSubTerrainNum() == count);
					abase::vector<int> vec;
					vec.insert(vec.end(),count,0);
					abase::vector<int>::iterator itr = vec.begin();
					std::copy(order->room_order,order->room_order+count,itr);
					pRandom->SetNewSubTernMap(vec);

					m_pWorld->SetRandomMapFlag(true);// 什么时候设置为false
				}*/
			}
			break;
		}
	}

	return true; 
}

//	Post message
bool CECGameRun::PostMessage(DWORD dwMsg, int iManager, int iSubID, DWORD p1/* 0 */, 
						DWORD p2/* 0 */, DWORD p3/* 0 */, DWORD p4/* 0 */, MsgDataBase* pData/* NULL */)
{
	ECMSG Msg;

	Msg.dwMsg		= dwMsg;
	Msg.iManager	= iManager;
	Msg.iSubID		= iSubID;
	Msg.dwParam1	= p1;
	Msg.dwParam2	= p2;
	Msg.dwParam3	= p3;
	Msg.dwParam4	= p4;
	Msg.pMsgData	= pData;

	return PostMessage(Msg);
}

//	Post message
bool CECGameRun::PostMessage(const ECMSG& Msg)
{
	ASSERT(m_pMessageMan);
	if (m_pMessageMan)
		m_pMessageMan->AddMessage(Msg);
	return true;
}

//	Get remain time of current double experence time
int CECGameRun::GetRemainDblExpTime()
{
	int iRemainTime = m_iDExpEndTime - g_pGame->GetServerAbsTime();
	a_ClampFloor(iRemainTime, 0);
	return iRemainTime;
}

bool CECGameRun::CaptureScreen(int N)
{
	//	ANGELICA_2_2 引擎下 且 N <= 1 时使用 jpg，其它情况使用 bmp
	A3DDEVFMT devFormat = g_pGame->GetA3DDevice()->GetDevFormat();

	if (devFormat.fmtTarget != A3DFMT_A8R8G8B8 &&
		devFormat.fmtTarget != A3DFMT_X8R8G8B8)
		N = 1;

	if (g_pGame->GetConfigs()->GetSystemSettings().bWideScreen)
		N = 1;

	//	N > 1 时使用的临时 Surface
	A3DSurface * pTempSurface = NULL;
	if (N > 1)
	{
		pTempSurface = new A3DSurface();
		if (!pTempSurface->Create(g_pGame->GetA3DDevice(), devFormat.nWidth, devFormat.nHeight, devFormat.fmtTarget))
		{
			a_LogOutput(0, "Capture screen failure because creating A3DSurface failure for N=%d and N will be set to 1", N);
			delete pTempSurface;
			pTempSurface = NULL;
			N = 1;
		}
	}

	SYSTEMTIME st;
	GetLocalTime(&st);

	char szTitle[MAX_PATH], szFile[MAX_PATH];
	bool bExportJPG = false;
#ifdef ANGELICA_2_2
	bExportJPG = (N <= 1);
#endif
	char *szFileFormat = bExportJPG ? "%04d-%02d-%02d %02d-%02d-%02d.jpg" : "%04d-%02d-%02d %02d-%02d-%02d.bmp";
	sprintf(szTitle, szFileFormat,	st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
	sprintf(szFile, "%s\\Screenshots\\", af_GetBaseDir());
	glb_CreateDirectory(szFile);
	strcat(szFile, szTitle);

	// we turn off the fps shown and take a snap shot
	bool bShowFPS = g_pA3DConfig->RT_GetShowFPSFlag();
	int bRTDebug = g_pGame->GetConfigs()->GetRTDebugLevel();
	g_pA3DConfig->RT_SetShowFPSFlag(false);
	g_pGame->GetConfigs()->SetRTDebugLevel(0);

	if (N <= 1)
	{
		Render(false);
		Sleep(100);// wait a while to let the entire scene rendered to back.
#ifdef ANGELICA_2_2
		g_pGame->GetA3DDevice()->ExportColorToFile(szFile, bExportJPG);
#else
		g_pGame->GetA3DDevice()->ExportColorToFile(szFile);
#endif
	}
	else
	{
		int nWidth = devFormat.nWidth;
		int nHeight = devFormat.nHeight;

		FILE * fpFile = fopen(szFile, "wb");
		if (!fpFile)
		{
			a_LogOutput(0, "Can not create screen shot file %s", szFile);
		}
		else
		{
			int pitch = N * nWidth * 3;
			pitch += (4 - (pitch & 0x3)) & 0x3;

			BITMAPFILEHEADER bmfh;
			BITMAPINFOHEADER bmih;
			memset(&bmfh, 0, sizeof(bmfh));
			memset(&bmih, 0, sizeof(bmih));
			bmfh.bfType = 0x4D42;
			bmfh.bfSize = sizeof(bmfh) + sizeof(bmih) + pitch * nHeight;
			bmfh.bfOffBits = sizeof(bmfh) + sizeof(bmih);

			bmih.biSize = sizeof(bmih);
			bmih.biWidth = N * nWidth;
			bmih.biHeight = N * nHeight;
			bmih.biPlanes = 1;
			bmih.biBitCount = 24;
			bmih.biCompression = BI_RGB;

			fwrite(&bmfh, sizeof(bmfh), 1, fpFile);
			fwrite(&bmih, sizeof(bmih), 1, fpFile);

			BYTE * pTempLine = (BYTE*)a_malloctemp(pitch * nHeight);
			A3DCameraBase * pCamera = g_pGame->GetViewport()->GetA3DCamera();
			for(int h=0; h<N; h++)
			{
				for(int w=0; w<N; w++)
				{
					A3DMATRIX4 matPostOffset;
					matPostOffset = Scaling((float)N, (float)N, 1.0f) * Translate(N - 1.0f - w * 2, h * 2 - (N - 1.0f), 0.0f);
					pCamera->SetPostProjectTM(matPostOffset);

					Render(false);
					Sleep(100);// wait a while to let the entire scene rendered to back.

					HRESULT hval;
#ifdef ANGELICA_2_2
					hval = D3DXLoadSurfaceFromSurface(pTempSurface->GetDXSurface(), NULL, NULL, g_pGame->GetA3DDevice()->GetBackBuffer(), NULL, NULL, D3DX_DEFAULT, 0);
#else
					hval = g_pGame->GetA3DDevice()->GetD3DDevice()->CopyRects(g_pGame->GetA3DDevice()->GetBackBuffer(), NULL, 0, pTempSurface->GetDXSurface(), NULL);
#endif
					if( hval != D3D_OK )
					{
						a_LogOutput(0, "Capture screen copy surface error=%x for N=%d, h=%d, w=%d", hval, N, h, w);
						continue;
					}

					D3DLOCKED_RECT rectLocked;
					hval = pTempSurface->GetDXSurface()->LockRect(&rectLocked, NULL, D3DLOCK_READONLY);
					if( hval != D3D_OK )
					{
						a_LogOutput(0, "Capture screen Lock surface rect error=%x for N=%d, h=%d, w=%d", hval, N, h, w);
						continue;
					}

					int x, y;
					DWORD * pLines = NULL;
					for(y=0; y<nHeight; y++)
					{
						pLines = (DWORD *)((BYTE*)rectLocked.pBits + y * rectLocked.Pitch);
						for(x=0; x<nWidth; x++)
						{
							A3DCOLOR color = pLines[x];
							pTempLine[(nHeight - 1 - y) * pitch + (w * nWidth + x) * 3] = A3DCOLOR_GETBLUE(color);
							pTempLine[(nHeight - 1 - y) * pitch + (w * nWidth + x) * 3 + 1] = A3DCOLOR_GETGREEN(color);
							pTempLine[(nHeight - 1 - y) * pitch + (w * nWidth + x) * 3 + 2] = A3DCOLOR_GETRED(color);
						}
					}

					pTempSurface->GetDXSurface()->UnlockRect();
				}

				fseek(fpFile, bmfh.bfOffBits + (N * nHeight - h * nHeight - nHeight) * pitch, SEEK_SET);
				fwrite(pTempLine, pitch * nHeight, 1, fpFile);
			}

			fflush(fpFile);
			fclose(fpFile);

			pCamera->SetPostProjectTM(IdentityMatrix());
			a_freetemp(pTempLine);
		}
	}

	if( pTempSurface )
	{
		pTempSurface->Release();
		delete pTempSurface;
		pTempSurface = NULL;
	}

	g_pA3DConfig->RT_SetShowFPSFlag(bShowFPS);
	g_pGame->GetConfigs()->SetRTDebugLevel(bRTDebug);

	const ACHAR* szMsg = g_pGame->GetFixedMsgTab()->GetWideString(FIXMSG_SCREENSHOT);
	g_pGame->GetRTDebug()->OutputNotifyMessage(0xff0000ff, szMsg);
	return true;
}

long CECGameRun::GetCommonData(long key)
{
	CommonDataTable::pair_type Pair = m_CommonDataTab.get(key);
	if (Pair.second)
		return *Pair.first;
	else
		return 0;
}
CECGameRun::unique_data* CECGameRun::GetUniqueData(int key)
{
	UniqueDataMap::iterator itr = m_UniqueDataMap.find(key);
	if(itr==m_UniqueDataMap.end()) return NULL;
	
	return itr->second;
}
const wchar_t * CECGameRun::GetProfName(int iProf)const
{
	const wchar_t *szRet = NULL;
	if (iProf >= 0 && iProf < NUM_PROFESSION)
	{
		static int s_ProfDesc[NUM_PROFESSION] = {			
			FIXMSG_PROF_WARRIOR,
			FIXMSG_PROF_MAGE,
			FIXMSG_PROF_MONK,
			FIXMSG_PROF_HAG,
			FIXMSG_PROF_ORC,
			FIXMSG_PROF_GHOST,
			FIXMSG_PROF_ARCHOR,
			FIXMSG_PROF_ANGEL,
			FIXMSG_PROF_JIANLING,
			FIXMSG_PROF_MEILING,
			FIXMSG_PROF_YEYING,
			FIXMSG_PROF_YUEXIAN,
		};
		CECStringTab* pStrTab = g_pGame->GetFixedMsgTab();
		szRet = pStrTab->GetWideString(s_ProfDesc[iProf]);
	}
	else
	{
		ASSERT(false);
	}
	return szRet;
}

ACString CECGameRun::GetLevel2Name(int iLevel2)const
{
	ACString strRet;
	
	int iIndex = -1;
	if (iLevel2 >= 0 && iLevel2 <= 8)
		iIndex = FIXMSG_LEVEL2_01 + iLevel2;
	else if (iLevel2 >= 20 && iLevel2 <= 22)
		iIndex = FIXMSG_LEVEL2_20 + iLevel2 - 20;
	else if (iLevel2 >= 30 && iLevel2 <= 32)
		iIndex = FIXMSG_LEVEL2_30 + iLevel2 - 30;

	if (iIndex >= 0)
	{
		CECStringTab* pStrTab = g_pGame->GetFixedMsgTab();
		strRet = pStrTab->GetWideString(iIndex);
	}
	else
	{
		ASSERT(false);
	}
	return strRet;
}

ACString CECGameRun::GetLevel2NameForCondition(int iMinLevel2)const
{
	ACString strRet;
	
	CECStringTab* pStrTab = g_pGame->GetFixedMsgTab();

	if (iMinLevel2 >= 0 && iMinLevel2 <= 8)
	{
		strRet = pStrTab->GetWideString(FIXMSG_LEVEL2_01 + iMinLevel2);
	}
	else if (iMinLevel2 >= 20 && iMinLevel2 <= 22)
	{
		strRet.Format(pStrTab->GetWideString(FIXMSG_OR_STRING)
			, pStrTab->GetWideString(FIXMSG_LEVEL2_20 + iMinLevel2 - 20)
			, pStrTab->GetWideString(FIXMSG_LEVEL2_30 + iMinLevel2 - 20));
	}
	else if (iMinLevel2 >= 30 && iMinLevel2 <= 32)
	{
		strRet.Format(pStrTab->GetWideString(FIXMSG_OR_STRING)
			, pStrTab->GetWideString(FIXMSG_LEVEL2_20 + iMinLevel2 - 30)
			, pStrTab->GetWideString(FIXMSG_LEVEL2_30 + iMinLevel2 - 30));
	}
	else
	{
		ASSERT(false);
	}
	return strRet;
}

void CECGameRun::SetSellingRoleID(int roleid)
{
	m_SellingRoleID = roleid;
}

void CECGameRun::SetAccountLoginInfo(const AccountLoginInfo &info)
{
	if (m_AccountLoginInfo != info)
	{
		m_AccountLoginInfo = info;
		m_bAccountLoginInfoShown = false;
	}
}

void CECGameRun::ResetAccountLoginInfo()
{
	m_AccountLoginInfo.Reset();
	m_bAccountLoginInfoShown = true;
}

void CECGameRun::ShowAccountLoginInfo()
{
	if (!m_bAccountLoginInfoShown)
	{
		CECGameUIMan* pGameUIMan = m_pUIManager->GetInGameUIMan();
		if (pGameUIMan)
		{
			m_bAccountLoginInfoShown = true;
			if (CECUIConfig::Instance().GetGameUI().bEnableShowIP)
			{
				ACString strText, strTemp;
				
				//	上次登录时间
				tm t = g_pGame->GetServerLocalTime(m_AccountLoginInfo.login_time);
				strTemp.Format(pGameUIMan->GetStringFromTable(8010),
					t.tm_year+1900,
					t.tm_mon+1,
					t.tm_mday,
					t.tm_hour,
					t.tm_min );
				strText.Format(pGameUIMan->GetStringFromTable(9343), strTemp);
				pGameUIMan->AddChatMessage(strText, GP_CHAT_SYSTEM);
				
				//	上次登录IP
				in_addr ip;
				ip.s_addr = m_AccountLoginInfo.login_ip;
				strTemp = AS2AC(inet_ntoa(ip));
				strText.Format(pGameUIMan->GetStringFromTable(9344), strTemp);
				pGameUIMan->AddChatMessage(strText, GP_CHAT_SYSTEM);
				
				//	当前登录IP
				in_addr cur_ip;
				cur_ip.s_addr = m_AccountLoginInfo.current_ip;
				strTemp = AS2AC(inet_ntoa(cur_ip));
				strText.Format(pGameUIMan->GetStringFromTable(9345), strTemp);
				pGameUIMan->AddChatMessage(strText, GP_CHAT_SYSTEM);
			}
		}
	}
}

void CECGameRun::SetAccountInfoFlag(char accountinfo_flag)
{
	m_accountInfoFlag = accountinfo_flag;
	m_bAccountInfoShown = false;
}

void CECGameRun::ShowAccountInfo()
{
	if (!m_bAccountInfoShown)
	{
		CECGameUIMan* pGameUIMan = m_pUIManager->GetInGameUIMan();
		if (pGameUIMan)
		{
			m_bAccountInfoShown = true;
			if (CECUIConfig::Instance().GetGameUI().bEnableCompleteAccount && (m_accountInfoFlag & 0x03))
			{
				ACString strText = pGameUIMan->GetStringFromTable(9347);
				if (!strText.IsEmpty())
					pGameUIMan->MessageBox("Account_CompleteInfo", strText, MB_OKCANCEL, A3DCOLORRGBA(255, 255, 255, 160));
			}
		}
	}
}

void CECGameRun::ReleaseSoundTable()
{
	if (!m_SoundTable.empty())
	{
		AMSoundBufferMan* pSoundBufMan = g_pGame->GetAMSoundBufferMan();
		SoundTable::iterator it = m_SoundTable.begin();
		for (; it != m_SoundTable.end(); ++ it)
		{
			AMSoundBuffer *pSBuffer = it->second;
			if (pSBuffer)
				pSoundBufMan->Release2DSound(&pSBuffer);
		}
		m_SoundTable.clear();
	}
}

bool CECGameRun::ImportSoundStringTable(const char* szFile)
{	
	AScriptFile sf;
	if (!sf.Open(szFile))
	{
		a_LogOutput(1, "CECGameRun::ImportSoundStringTable, Failed to load %s", szFile);
		return false;
	}
	
	while( !sf.IsEnd() )
	{
		bool bval = sf.GetNextToken(true);
		if( !bval ) break;		// End of file.
		int idString = atoi(sf.m_szToken);
		
		bval = sf.GetNextToken(true);
		if( !bval )
		{
			a_LogOutput(1, "CECGameRun::ImportSoundStringTable, Failed to read from file at line=%d", sf.GetCurLine());
			return false;
		}
		
		if (m_SoundStringTable.find(idString) != m_SoundStringTable.end())
		{
			a_LogOutput(1, "CECGameRun::ImportSoundStringTable, repeated id=%d at line=%d", idString, sf.GetCurLine());
			return false;
		}
		
		m_SoundStringTable[idString] = sf.m_szToken;
	}
	
	sf.Close();
	return true;
}

const AString & CECGameRun::GetSoundStringFromTable(int idString)
{
	return m_SoundStringTable[idString];
}

AMSoundBuffer * CECGameRun::GetOrLoadSoundFromTable(int idString)
{
	AMSoundBuffer * pSBuffer = NULL;

	AString strFile = GetSoundStringFromTable(idString);
	strFile.TrimLeft();
	strFile.TrimRight();
	strFile.MakeLower();
	if (!strFile.IsEmpty())
	{
		SoundTable::iterator it = m_SoundTable.find(strFile);
		if (it != m_SoundTable.end())
		{
			pSBuffer = it->second;
		}
		else
		{
			AMSoundBufferMan* pSoundBufMan = g_pGame->GetAMSoundBufferMan();
			pSBuffer = pSoundBufMan->Load2DSound(strFile);
			m_SoundTable[strFile] = pSBuffer;
		}
	}

	return pSBuffer;
}

bool CECGameRun::LoadHistoryStage(const char* szFile)
{
	AScriptFile sf;
	if (!sf.Open(szFile))
	{
		a_LogOutput(1, "CECGameRun::LoadHistoryStage, failed to open file %s",szFile);
		return false;
	}

	int stage = 0;

	while (sf.GetNextToken(true))
	{
		//	Get current tpye
		stage = sf.GetNextTokenAsInt(false);

		if (!sf.MatchToken("{", false))
		{
			ASSERT(0);
			sf.Close();
			return false;
		}

		MAPVEC* vec = new MAPVEC;
		while (sf.PeekNextToken(true))
		{
			if (!stricmp(sf.m_szToken, "}"))
			{
				sf.GetNextToken(true);
				break;
			}

			sf.GetNextToken(true); // 读取每个地图配置

			CSplit split(sf.m_szToken);
			CSplit::VectorAString v = split.Split(",");
			if (v.size()!=3) // 3个值： 地图号，x，y 坐标
			{
				ASSERT(0);
				sf.Close();
				return false;
			}

			int mapid = v[0].ToInt();
			int x = v[1].ToInt();
			int y = v[2].ToInt();

			HistoryProgConfig hpc;
			hpc.mapid = mapid;
			hpc.x = x;
			hpc.y = y;
			vec->Add(hpc);
		}
		if(m_stageMap.find(stage) != m_stageMap.end())
			delete vec;
		else
			m_stageMap[stage] = vec;
	}

	sf.Close();

	return true;
}

int CECGameRun::GetStageID(int idx)
{
	if(idx<0||idx>=ARRAY_SIZE(((HISTORY_ADVANCE_CONFIG *)NULL)->history_stage_id)) return -1;

	int stage_id = 0;
	
	elementdataman *pDataMan = g_pGame->GetElementDataMan();
	DATA_TYPE DataType;
	int tid = pDataMan->get_first_data_id(ID_SPACE_CONFIG,DataType);
	while(tid)
	{
		if (DataType == DT_HISTORY_ADVANCE_CONFIG)
		{
			HISTORY_ADVANCE_CONFIG* pHistory = (HISTORY_ADVANCE_CONFIG*)pDataMan->get_data_ptr(tid,ID_SPACE_CONFIG,DataType);
			if (pHistory)
			{
				stage_id = pHistory->history_stage_id[idx];
			}
			break;
		}
		tid = pDataMan->get_next_data_id(ID_SPACE_CONFIG,DataType);
	}
	
	return stage_id;
}
bool CECGameRun::GetCurStageInfo(ACString& name,ACString& desc,float& progress)
{
	CECGameRun::unique_data* data = GetUniqueData(0);
	if (data)
	{
		if(data->type ==1)
		{
			int id = GetStageID(data->GetValueAsInt());
			return GetStageInfo(id,name,desc,progress);
		}
	}
	return false;
}
bool CECGameRun::GetStageInfo(int id,ACString& name,ACString& desc,float& progress)
{
	DATA_TYPE DataType;
	elementdataman *pDataMan = g_pGame->GetElementDataMan();
	HISTORY_STAGE_CONFIG* pStage = (HISTORY_STAGE_CONFIG*)pDataMan->get_data_ptr(id,ID_SPACE_CONFIG,DataType);
	if (pStage)
	{
		name = pStage->name;
		desc = pStage->desc;
		CECGameRun::unique_data* udata = GetUniqueData(pStage->progress_value_id);
		if (udata)
		{
			if(udata->type==1 && pStage->progress_value_goal!=0)
			{
				progress = udata->GetValueAsInt() / (float)pStage->progress_value_goal;	
			}
			else if (udata->type ==2)
			{
				progress = (float)udata->GetValueAsDouble() / pStage->progress_value_goal;				
			}
			else
				progress = 0;

			a_Clamp(progress,0.0f,1.0f);

			return true;
		}
	}
	return false;
}
int CECGameRun::GetStageVarKey(int id)
{
	DATA_TYPE DataType;
	elementdataman *pDataMan = g_pGame->GetElementDataMan();
	HISTORY_STAGE_CONFIG* pStage = (HISTORY_STAGE_CONFIG*)pDataMan->get_data_ptr(id,ID_SPACE_CONFIG,DataType);
	if (pStage)
	{
		return pStage->progress_value_id;
	}
	return -1;
}
int CECGameRun::GetCurStageIndex()
{
	CECGameRun::unique_data* data = GetUniqueData(0);
	if (data)
	{
		if(data->type ==1)
		{
			return data->GetValueAsInt();
		}
	}
	return -1;
}
bool CECGameRun::IsInStage(int mapid,int stage)
{
	if(m_stageMap.find(stage) == m_stageMap.end()) return false;

	for (int i=0;i<m_stageMap[stage]->GetSize();i++)
	{
		HistoryProgConfig* p = m_stageMap[stage]->ElementAt(i);
		if (p && p->mapid == mapid)
			return true;
	}
	return false;
}
bool CECGameRun::IsInCurStage(int mapid)
{
	CECGameRun::unique_data* data = GetUniqueData(0);
	if (data)
	{
		if(data->type ==1)
		{
			int id = GetStageID(data->GetValueAsInt());
			return IsInStage(mapid,id);
		}
	}
	return false;
}
CECGameRun::HistoryProgConfig* CECGameRun::GetCurStageProgConfig(int mapid)
{
	CECGameRun::unique_data* data = GetUniqueData(0);
	if (data)
	{
		if(data->type ==1)
		{
			int stage = GetStageID(data->GetValueAsInt());
			if(m_stageMap.find(stage) == m_stageMap.end()) return NULL;
			
			for (int i=0;i<m_stageMap[stage]->GetSize();i++)
			{
				HistoryProgConfig* pHpc = m_stageMap[stage]->ElementAt(i);
				if (pHpc->mapid == mapid)
					return pHpc;
			}
		}
	}
	
	return NULL;
}
void CECGameRun::OnUniqueDataBroadcast(GNET::Protocol* p)
{
	const GNET::UniqueDataModifyBroadcast* cmd = (const GNET::UniqueDataModifyBroadcast*)p;
	
	GNET::GUniqueDataElemNodeVector::const_iterator itr = cmd->values.begin();
	for (;itr!=cmd->values.end();++itr)
	{
		unique_data* pUniqueData = new unique_data;
		
		pUniqueData->type = itr->val.vtype;
		pUniqueData->_value = itr->val.value;
		
		UniqueDataMap::iterator dataitr = m_UniqueDataMap.find(itr->key);
		if (dataitr!=m_UniqueDataMap.end())
		{
			delete dataitr->second;
		}
		m_UniqueDataMap[itr->key] = pUniqueData;

		int v = pUniqueData->GetValueAsInt();
	}
}

void CECGameRun::OnRandomMap(int row,int col,int* pData)
{
	if(m_pRandomMapProc)
		A3DRELEASE(m_pRandomMapProc);

	m_pRandomMapProc = new CECRandomMapProcess();
	if(m_pRandomMapProc)	
		m_pRandomMapProc->Init(row,col,pData);	
}
