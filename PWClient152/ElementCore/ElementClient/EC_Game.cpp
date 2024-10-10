  /*
 * FILE: glb_Game.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2003/12/16
 *
 * HISTORY: 
 *
 * Copyright (c) 2003 Archosaur Studio, All Rights Reserved.
 */

#pragma warning (disable: 4284)

#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_Viewport.h"
#include "EC_GameRun.h"
#include "EC_GameSession.h"
#include "EC_RTDebug.h"
#include "EC_ShadowRender.h"
#include "EC_PortraitRender.h"
#include "EC_GFXCaster.h"
#include "EC_IvtrTypes.h"
#include "EC_Resource.h"
#include "EL_Building.h"
#include "EC_ImageRes.h"
#include "EC_Configs.h"
#include "EC_UIConfigs.h"
#include "EL_BackMusic.h"
#include "EC_UIManager.h"
#include "EC_LoginUIMan.h"
#include "EC_GameUIMan.h"
#include "TaskTemplMan.h"
#include "EC_Model.h"
#include "EC_ModelMan.h"
#include "EC_SceneLoader.h"
#include "EC_SoundCache.h"
#include "EC_Utility.h"
#include "EC_SunMoon.h"
#include "EC_Faction.h"
#include "EC_World.h"
#include "EC_Player.h"
#include "EC_NPC.h"
#include "ACSWrapper.h"
#include "EC_FixedMsg.h"
#include "EC_CommandLine.h"
#include "DlgSettingSystem.h"
#include "DlgExplorer.h"
#include "EC_CrossServer.h"
#include "EC_MCDownload.h"
#include "EC_ServerList.h"
#include "EC_AutoPolicy.h"

#ifdef _PROFILE_MEMORY
#include "Memory\EC_HookMemory.h"
#include <DbgHelp.h>
#endif

#include "privilege.hxx"
#include "elementdataman.h"
#include "itemdataman.h"
#include "ElementSkill.h"
#include "DataPathMan.h"
#include "EC_FactionPVP.h"
#include "ECScriptMan.h"
#include "ECScriptContext.h"
#include "ECScriptCheckState.h"

#include "globaldataman.h"
#include "LuaState.h"

#include <A3DGFXEx.h>
#include <A3DGFXExMan.h>
#include <A3DSkinMan.h>
#include <A3DSkinRender.h>
#include <A3DCursor.h>
#include <A3DFont.h>
#include <A3DFontMan.h>
#include <A3DCamera.h>
#include <A3DLight.h>
#include <A3DLightMan.h>
#include <A3DConfig.h>
#include <A3DSkin.h>
#include <A3DFuncs.h>
#include <A3DViewport.h>
#include <AMEngine.h>
#include <AMConfig.h>
#include <AFileImage.h>
#include <AWScriptFile.h>
#include <AScriptFile.h>
#include <AAssist.h>
#include <A3DLitModelRender.h>
#include <map>
#include <utility>
#include <string>

#include "resource.h"

#define new A_DEBUG_NEW

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

#ifdef ANGELICA_2_2
extern D3DCAPS9		g_d3dcaps;
#else
extern D3DCAPS8		g_d3dcaps;
#endif

//	 Gameversion
DWORD GAME_VERSION = ((0 << 24) | (1 << 16) | (5 << 8) | 2);
DWORD GAME_BUILD = 2457;

#define ELEMENTSKILL_VERSION	15


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

//	Item name color
static int l_aNameCols[10] = 
{
	0xffffffff,		//	Default
	0xff8080ff,		//	Light blue
	0xffffdc50,		//	Yellow
	0xffaa32ff,		//	Purple
	0xffff6000,		//	Dark gold
	0xffffffff,		//	White
	0xffb0b0b0,		//	Gray
	0xff00ffae,		//	Blue-green
	0xffff0000,		//	Red

	//	Un-used now
	0xffffffff,
};

static bool g_bFocused = false;
static DWORD l_idMainThread = 0;

CECGame* g_pGame = NULL;
extern CECConfigs g_GameCfgs;

///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////

//	Time callback function
static void __stdcall _TimeCallback(UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2)
{
	if (!g_pGame || !g_pGame->GetGameSession())
		return;

	g_pGame->GetGameSession()->NetPulse();
}

///////////////////////////////////////////////////////////////////////////
// for itemdataman we create some dummy functions
///////////////////////////////////////////////////////////////////////////
void set_to_classid(DATA_TYPE type, item_data * data, int major_type)
{
}
int addon_generate_arg(DATA_TYPE type, addon_data & data, int arg_num/*初始的参数个数*/)
{
	return arg_num;
} 
int addon_update_ess_data(const addon_data & data, void * essence,size_t ess_size, prerequisition * require)
{
	return 0;
}
void update_require_data(prerequisition *require)
{
	require->durability *= ENDURANCE_SCALE;
	require->max_durability *= ENDURANCE_SCALE;
}
void get_item_guid(int id, int& g1, int& g2)
{
	g1 = 0;
	g2 = 1;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECGame
//	
///////////////////////////////////////////////////////////////////////////

CECGame::CECGame() : 
m_ItemExtProps(1024),
m_SuiteEquipTab(512),
m_SuiteFashionTab(64),
m_ItemColTab(64)
{
	ASSERT(!g_pGame);

	g_pGame = this;

	m_pA3DEngine		= NULL;
	m_pA3DDevice		= NULL;
	m_pBackMusic		= NULL;
	m_pViewport			= NULL;
	m_bActive			= false;
	m_pDirLight			= NULL;
	m_pDynamicLight		= NULL;
	m_pConfigs			= NULL;
	m_dwTickTime		= 0;
	m_dwRealTickTime	= 0;
	m_bDiscardFrame		= false;
	m_pGameRun			= NULL;
	m_pGameSession		= NULL;
	m_pRTDebug			= NULL;
	m_fAverFRate		= 30.0f;
	m_pA3DGFXExMan		= NULL;
	m_pECModelMan		= NULL;
	m_pShadowRender		= NULL;
	m_pPortraitRender	= NULL;
	m_pGFXCaster		= NULL;
	m_pTaskMan			= NULL;
	m_pImageRes			= NULL;
	m_iCurCursor		= -1;
	m_iTimeError		= 0;
	m_iTimeZoneBias		= -480;	// time zone of Beijing
	m_pSkinRender1		= NULL;
	m_pSkinRender2		= NULL;
	m_pBackMusic		= NULL;
	m_pSoundCache		= NULL;
	m_pPrivilege		= NULL;
	m_pFactionMan		= NULL;
	m_pDynObjPath		= NULL;
	m_pLitModelRender1	= NULL;

	memset(&m_GameInit, 0, sizeof (m_GameInit));
	memset(m_aLights, 0, sizeof (m_aLights));

	m_pItemDataMan		= NULL;
	m_pElementDataMan	= NULL;

	::InitializeCriticalSection(&m_csMsg);

	m_AbsTimeStart = time(NULL);
	m_AbsTickStart = timeGetTime();
}

CECGame::~CECGame()
{
	::LeaveCriticalSection(&m_csMsg);
}

#define HookGdiFunc(h, f_name, f_entry, f_content, f_content_new) \
{ \
	DWORD dwTemp; \
	f_entry = (PLONGLONG)GetProcAddress(h, #f_name); \
	if (VirtualProtect(f_entry, 16, PAGE_EXECUTE_READWRITE, &dwTemp)) { \
		f_content = f_content_new = *f_entry; \
		unsigned char* p = (unsigned char*)&f_content_new; \
		*p++ = (unsigned char)0xe9; \
		*(DWORD*)p = DWORD((DWORD)&My##f_name - 5 - (DWORD)f_entry); \
		*f_entry = f_content_new; } \
}

static PLONGLONG GetAsyncKeyState_Entry = 0;
static LONGLONG GetAsyncKeyState_Entry_Content = 0;
static LONGLONG GetAsyncKeyState_Entry_Content_New = 0;

SHORT WINAPI MyGetAsyncKeyState(int key)
{
	if (g_pGame->GetGameInit().hWnd && g_pGame->GetGameInit().hWnd != ::GetForegroundWindow())
		return 0;

	*GetAsyncKeyState_Entry = GetAsyncKeyState_Entry_Content;
	SHORT ret = GetAsyncKeyState(key);
	*GetAsyncKeyState_Entry = GetAsyncKeyState_Entry_Content_New;
	return ret;
}

static void setup_hook()
{
	HMODULE h = ::LoadLibraryA("user32.dll");
	HookGdiFunc(h, GetAsyncKeyState, GetAsyncKeyState_Entry, GetAsyncKeyState_Entry_Content, GetAsyncKeyState_Entry_Content_New)
	::FreeLibrary(h);
}

static PLONGLONG TerminateProcess_Entry = 0;
static LONGLONG TerminateProcess_Entry_Content = 0;
static LONGLONG TerminateProcess_Entry_Content_New = 0;

BOOL WINAPI MyTerminateProcess(HANDLE hProcess, UINT uExitCode)
{
	*TerminateProcess_Entry = TerminateProcess_Entry_Content;
	::MessageBoxA(NULL, "警告：程序被异常终止!!!!", "Error", MB_OK);
	char* p = 0;
	memcpy(p, "error", 5);
	return TRUE;
}

static void hook_terminate_process()
{
	/*
	HMODULE h = ::LoadLibraryA("Kernel32.dll");
	HookGdiFunc(h, TerminateProcess, TerminateProcess_Entry, TerminateProcess_Entry_Content, TerminateProcess_Entry_Content_New)
	::FreeLibrary(h);
	*/
}

static PLONGLONG ExitProcess_Entry = 0;
static LONGLONG ExitProcess_Entry_Content = 0;
static LONGLONG ExitProcess_Entry_Content_New = 0;
extern BOOL g_bHookExitProcess;

void WINAPI MyExitProcess(UINT uExitCode)
{
#ifdef _PROFILE_MEMORY
	g_DisableMemoryHistory();
	::SymCleanup(GetCurrentProcess());
#endif
	*ExitProcess_Entry = ExitProcess_Entry_Content;

	if (g_bExceptionOccured || !g_bHookExitProcess)
		::ExitProcess(uExitCode);
	else
	{
		::MessageBoxA(NULL, "警告：程序异常退出!!!!", "Error", MB_OK);
		char* p = 0;
		memcpy(p, "error", 5);
	}
}

static void hook_exit_process()
{
	HMODULE h = ::LoadLibraryA("Kernel32.dll");
	HookGdiFunc(h, ExitProcess, ExitProcess_Entry, ExitProcess_Entry_Content, ExitProcess_Entry_Content_New)
	::FreeLibrary(h);
}

//	Initialize game
bool CECGame::Init(const GAMEINIT& GameInit)
{
	a_LogOutput(1, "Build version %d", GAME_BUILD);

	l_idMainThread = GetCurrentThreadId();

	// setup_hook();
	hook_terminate_process();
	hook_exit_process();

	// first of all verify the ElementSkill version
	if( ELEMENTSKILL_VERSION != GNET::ElementSkill::GetVersion() )
	{
		MessageBoxA(GameInit.hWnd, "ElementSkill's version is incorrect!", "ERROR", MB_ICONSTOP | MB_OK);
		return false;
	}

	m_GameInit = GameInit;

	//	Initalize random number generator
	a_InitRandom();

	//	Load configs
	m_pConfigs = &g_GameCfgs;	
	
	//	在D3D初始化前调用，以处理全屏时IE控件无法正常工作的问题
	InitBrowser();

	//	Initalize A3D engine
	if (!InitA3DEngine())
	{
		glb_ErrorOutput(ECERR_FAILEDTOCALL, "CECGame::Init", __LINE__);
		return false;
	}

	//	Create fonts
	if (!CreateFonts())
		a_LogOutput(1, "CECGame::Init, failed to create fonts !");

	//	now create gfx manager
	m_pA3DGFXExMan = new A3DGFXExMan();
	if (!m_pA3DGFXExMan->Init(m_pA3DDevice))
	{
		glb_ErrorOutput(ECERR_FAILEDTOCALL, "CECGame::Init", __LINE__);
		return false;
	}

	m_pLitModelRender1 = new A3DLitModelRender;
	if (!m_pLitModelRender1->Init(m_pA3DEngine, 60000, 60000 * 2) )
	{
		a_LogOutput(1, "CECGame::Init, lit model render init fail!");
		return false;
	}

	m_pFactionMan = new CECFactionMan;

	//	Create ECModel manager
	m_pECModelMan = new CECModelMan();
	m_pECModelMan->Init();

	//	Create A3D skin renders
	m_pSkinRender1 = m_pA3DEngine->GetA3DSkinMan()->GetDefSkinRender();
	m_pSkinRender2 = new A3DSkinRender;
	m_pSkinRender2->Init(m_pA3DEngine);

	//	Create sound cache
	if (!(m_pSoundCache = new CECSoundCache))
	{
		glb_ErrorOutput(ECERR_NOTENOUGHMEMORY, "CECGame::Init", __LINE__);
		return false;
	}

	m_pSoundCache->SetMaxCacheSize(10 * 1024 * 1024);
	m_pSoundCache->SetMaxFileSize(/*256*/400 * 1024);
	GetAMSoundBufferMan()->SetUserFileCache(m_pSoundCache);

	//	Load cursors
	if (!LoadCursors())
	{
		glb_ErrorOutput(ECERR_FAILEDTOCALL, "CECGame::Init", __LINE__);
		return false;
	}

	//	Create shadow renderer object
	if (!(m_pShadowRender = new CECShadowRender()))
	{
		glb_ErrorOutput(ECERR_NOTENOUGHMEMORY, "CECGame::Init", __LINE__);
		return false;
	}

	if (!m_pShadowRender->Init(512))
	{
		a_LogOutput(1, "CECGame::Init, failed to initalize shadow renderer!");
		m_pShadowRender->Release();
	}

	//	Create portrait render
	if (!(m_pPortraitRender = new CECPortraitRender()))
	{
		glb_ErrorOutput(ECERR_NOTENOUGHMEMORY, "CECGame::Init", __LINE__);
		return false;
	}

	if (!m_pPortraitRender->Init(64))
	{
		a_LogOutput(1, "CECGame::Init, failed to initalize portrait renderer!");
		return false;
	}

	//	Run time debug object
	if (!(m_pRTDebug = new CECRTDebug))
	{
		glb_ErrorOutput(ECERR_NOTENOUGHMEMORY, "CECGame::Init", __LINE__);
		return false;
	}

	if (!m_pRTDebug->Init())
	{
		glb_ErrorOutput(ECERR_FAILEDTOCALL, "CECGame::Init", __LINE__);
		return false;
	}

	//	Create GFX caster
	if (!(m_pGFXCaster = new CECGFXCaster(m_pA3DGFXExMan)))
	{
		glb_ErrorOutput(ECERR_NOTENOUGHMEMORY, "CECGame::Init", __LINE__);
		return false;
	}

	CECUIConfig::Instance().Load("configs\\uiconfig.ini");	
	if (!CECQShopConfig::Instance().Load("configs\\qshopitem.ini"))
	{
		glb_ErrorOutput(ECERR_FAILEDTOCALL, "CECGame::Init", __LINE__);
		return false;
	}

	//	Create game run object
	if (!(m_pGameRun = new CECGameRun))
	{
		glb_ErrorOutput(ECERR_NOTENOUGHMEMORY, "CECGame::Init", __LINE__);
		return false;
	}

	if (!m_pGameRun->Init())
	{
		glb_ErrorOutput(ECERR_FAILEDTOCALL, "CECGame::Init", __LINE__);
		return false;
	}

	//	Create game session
	if (!(m_pGameSession = new CECGameSession))
	{
		glb_ErrorOutput(ECERR_NOTENOUGHMEMORY, "CECGame::Init", __LINE__);
		return false;
	}	
	CECServerList::Instance().Init(CECUIConfig::Instance().GetLoginUI().strHotServerGroupName);

	CECCommandLine::GetEnable(_AL("wt"), CECUIConfig::Instance().GetGameUI().bEnableWebTradeSort);

	//	Load fixed messages
	if (!m_FixedMsgs.Init("Configs\\fixed_msg.txt", true))
		glb_ErrorOutput(ECERR_FAILEDTOCALL, "CECGame::Init", __LINE__);

	//	Load item description strings
	if (!m_ItemDesc.Init("Configs\\item_desc.txt", true))
		glb_ErrorOutput(ECERR_FAILEDTOCALL, "CECGame::Init", __LINE__);

	//	Load item extend description
	if (!m_ItemExtDesc.Init("Configs\\item_ext_desc.txt", true))
		glb_ErrorOutput(ECERR_FAILEDTOCALL, "CECGame::Init", __LINE__);

	//	Load skill description strings
	if (!m_SkillDesc.Init("Configs\\skillstr.txt", true))
		glb_ErrorOutput(ECERR_FAILEDTOCALL, "CECGame::Init", __LINE__);

	//	Load buff description strings
	if (!m_BuffDesc.Init("Configs\\buff_str.txt", true))
		glb_ErrorOutput(ECERR_FAILEDTOCALL, "CECGame::Init", __LINE__);

	//	Load item properties
	if (!LoadItemExtProps())
		glb_ErrorOutput(ECERR_FAILEDTOCALL, "CECGame::Init", __LINE__);

	//  Load object coordinate data
	if (!LoadObjectCoord())
		glb_ErrorOutput(ECERR_FAILEDTOCALL, "CECGame::Init", __LINE__);

	//  Load consume reward data
	if (!LoadConsumeReward())
		glb_ErrorOutput(ECERR_FAILEDTOCALL, "CECGame::Init", __LINE__);

	//  Load pet auto skill data
	if (!LoadPetAutoSkill())
		glb_ErrorOutput(ECERR_FAILEDTOCALL, "CECGame::Init", __LINE__);

	if (!CECFactionPVPModel::Instance().Init()){
		return false;
	}
	
	if (!globaldata_load() )
	{
		glb_ErrorOutput(ECERR_FAILEDTOCALL, "CECGame::Init", __LINE__);
		return false;
	}

	BYTE md5[16];
	if( !glb_CalcFileMD5("data\\elements.data", md5) )
		a_LogOutput(1, "data\\elements.data is missing...");
	else
		a_LogOutput(1, "elements data's md5: [%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x]", md5[0], md5[1], md5[2], md5[3],
			md5[4], md5[5], md5[6], md5[7], md5[8], md5[9], md5[10], md5[11], md5[12], md5[13], md5[14], md5[15]);

	m_pItemDataMan = new itemdataman();
	//	now load templates from file
	if (0 != m_pItemDataMan->load_data("data\\elements.data"))
	{
		MessageBoxA(NULL, "无法加载游戏数据文件！\n此错误可能是更新不完全所致，请重新进行更新或进行版本验证！", "错误", MB_OK | MB_ICONSTOP);
		glb_ErrorOutput(ECERR_FAILEDTOCALL, "CECGame::Init", __LINE__);
		return false;
	}
	m_pElementDataMan = m_pItemDataMan->GetElementDataMan();

	if( !glb_CalcFileMD5("data\\tasks.data", md5) )
		a_LogOutput(1, "data\\tasks.data is missing...");
	else
		a_LogOutput(1, "tasks data's md5: [%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x]", md5[0], md5[1], md5[2], md5[3],
			md5[4], md5[5], md5[6], md5[7], md5[8], md5[9], md5[10], md5[11], md5[12], md5[13], md5[14], md5[15]);
	//	Load task templates
	if (!(m_pTaskMan = new ATaskTemplMan))
	{
		glb_ErrorOutput(ECERR_NOTENOUGHMEMORY, "CECGame::Init", __LINE__);
		return false;
	}

	m_pTaskMan->Init(m_pElementDataMan);
	if (!m_pTaskMan->InitStorageTask())
	{
		a_LogOutput(1, "CECGame::Init, Storage task Init Failed!");
		return false;
	}

	//	Load iamge resources
	if (!(m_pImageRes = new CECImageRes))
	{
		glb_ErrorOutput(ECERR_NOTENOUGHMEMORY, "CECGame::Init", __LINE__);
		return false;
	}

	m_pImageRes->LoadAllImages();

	//	Load dynamic object path table
	m_pDynObjPath = new DataPathMan;
	if (!m_pDynObjPath || !m_pDynObjPath->Load("configs\\DynamicObjects.data"))
	{
		a_LogOutput(1, "CECGame::Init, Failed to load data\\DynamicObjects.data");
		return false;
	}

	//	Build suite equipment table after elementdata.data has been loaded
	BuildSuiteEquipTab();

	m_pBackMusic = new CELBackMusic();
	if( !m_pBackMusic->Init(g_pGame->GetA3DEngine()->GetAMSoundEngine()) )
	{
		delete m_pBackMusic;
		m_pBackMusic = NULL;

		glb_ErrorOutput(ECERR_FAILEDTOCALL, "CECGame::Init", __LINE__);
		return false;
	}

	// now set sound device volume for the first time
	SetAllVolume(m_pConfigs->GetSystemSettings());

	//  Initialize the auto policies
	CECAutoPolicy::GetInstance().Init();

	//  Start the downloader for mini client
	if (m_pConfigs->IsMiniClient())
	{
		if( !CECMCDownload::GetInstance().StartDownloader() )
			a_LogOutput(1, "CECGame::Init, Start the downloader for mini client failed!");
	}

	//	Initialize player's and NPC's static resources
	CECPlayer::InitStaticRes();
	CECNPC::InitStaticRes();

/*	if (!::timeSetEvent(1000, 100, _TimeCallback, 0, TIME_PERIODIC | TIME_CALLBACK_FUNCTION))
	{
		a_LogOutput(1, "CECGame::Init, failed to call timeSetEvent()");
	}
*/
	extern unsigned long _task_templ_cur_version;
//	m_strAllVersion.Format("%x%x%x", ELEMENTDATA_VERSION, _task_templ_cur_version, globaldata_getgshop_timestamp());
	m_strAllVersion.Format("%x%x%x%x", ELEMENTDATA_VERSION, _task_templ_cur_version, globaldata_getgshop_timestamp(),globaldata_getgshop_timestamp2());

 //	create an icon in tray area
	     
	m_nid.cbSize=(DWORD)sizeof(NOTIFYICONDATA);   
	m_nid.hWnd=m_GameInit.hWnd;   
	m_nid.uID=IDI_MAINICON;   
	m_nid.uFlags=NIF_ICON|NIF_MESSAGE|NIF_TIP   ;   
	m_nid.uCallbackMessage= WM_SHOWWIND;  
	m_nid.hIcon= LoadIcon(m_GameInit.hInstance, MAKEINTRESOURCE(IDI_MAINICON));
	a_strcpy(m_nid.szTip, m_FixedMsgs.GetWideString(FIXMSG_PERFECT_WORLD)); 

	g_bRenderNoFocus = CECCommandLine::GetRenderWhenNoFocus();

	return true;
}

//	Release game
void CECGame::Release()
{	
	FreeBrowser();

	A3DRELEASE(m_pGameRun);

	//	Release player's and NPC's static resources
	CECPlayer::ReleaseStaticRes();
	CECNPC::ReleaseStaticRes();

	CECAutoPolicy::GetInstance().Release();

	if( m_pBackMusic )
	{
		m_pBackMusic->Release();
		delete m_pBackMusic;
		m_pBackMusic = NULL;
	}

	globaldata_release();

	if( m_pItemDataMan )
	{
		delete m_pItemDataMan;
		m_pItemDataMan = NULL;
	}

	m_FixedMsgs.Release();
	m_ItemDesc.Release();
	m_ItemExtDesc.Release();
	m_SkillDesc.Release();
	m_BuffDesc.Release();
	m_ItemExtProps.clear();
	m_SuiteEquipTab.clear();
	m_SuiteFashionTab.clear();
	m_ItemColTab.clear();
	m_PetAutoSkills.clear();

	//	Close session
	if (m_pGameSession)
	{
		m_pGameSession->Close();
		delete m_pGameSession;
		m_pGameSession = NULL;
		Sleep(500);
	}

	A3DRELEASE(m_pDynObjPath);
	A3DRELEASE(m_pRTDebug);
	A3DRELEASE(m_pShadowRender);
	A3DRELEASE(m_pPortraitRender);
	A3DRELEASE(m_pTaskMan);
	A3DRELEASE(m_pImageRes);

	if (m_pPrivilege)
	{
		delete m_pPrivilege;
		m_pPrivilege = NULL;
	}

	if (m_pGFXCaster)
	{
		delete m_pGFXCaster;
		m_pGFXCaster = NULL;
	}

	if (m_pFactionMan)
	{
		delete m_pFactionMan;
		m_pFactionMan = NULL;
	}

	A3DRELEASE(m_pA3DGFXExMan);
	if (m_pECModelMan)
	{
		m_pECModelMan->Release();
		delete m_pECModelMan;
		m_pECModelMan = NULL;
	}

	A3DRELEASE(m_pLitModelRender1);

	A3DRELEASE(m_pSkinRender2);
	m_pSkinRender1 = NULL;
	A3DRELEASE(m_pSoundCache);

	ReleaseCursors();
	ReleaseFonts();

	//	Release A3D engine
	CECGame::ReleaseA3DEngine();

	m_pConfigs = NULL;
	g_pGame = NULL;
}

//	Load cursors
bool CECGame::LoadCursors()
{
	for (int i=0; i < NUM_RES_CURSOR; i++)
	{
		A3DSysCursor* pCursor = new A3DSysCursor;
		
		if (!pCursor->Load(m_pA3DDevice, res_CursorFile(i)))
		{
			a_LogOutput(1, "CECGame::LoadCursors, failed to cursor %s!", res_CursorFile(i));
			delete pCursor;
			pCursor = NULL;
		}

		m_aCursors.Add(pCursor);
	}

	return true;
}

//	Release cursors
void CECGame::ReleaseCursors()
{
	for (int i=0; i < m_aCursors.GetSize(); i++)
	{
		A3DCursor* pCursor = m_aCursors[i];
		if (pCursor)
			delete pCursor;
	}

	m_aCursors.RemoveAll();

	if (m_pA3DDevice)
		m_pA3DDevice->SetCursor(NULL);
}

//	Create fonts
bool CECGame::CreateFonts()
{
	//	Open font description file
	AWScriptFile ScriptFile;
	if (!ScriptFile.Open("Configs\\fonts.txt"))
	{
		m_aFonts.SetSize(NUM_RES_FONT, 10);
		for (int i=0; i < NUM_RES_FONT; i++)
			m_aFonts[i] = NULL;

		a_LogOutput(1, "CECGame::CreateFonts, Failed to open file Configs\\fonts.txt");
		return false;
	}

	A3DFontMan* pFontMan = m_pA3DEngine->GetA3DFontMan();

	wchar_t szFirstFont[256];
	int index = 0;
	//	Read configs
	while (ScriptFile.GetNextToken(true))
	{
		//	Font name
		wchar_t szName[128];
		a_strcpy(szName, ScriptFile.m_szToken);
		//	Font size
		int iHei = ScriptFile.GetNextTokenAsInt(false);
		//	Bold and Italic flag
		DWORD dwFlags = 0;
		dwFlags |= ScriptFile.GetNextTokenAsInt(false) ? A3DFONT_BOLD : 0;
		dwFlags |= ScriptFile.GetNextTokenAsInt(false) ? A3DFONT_ITALIC : 0;
		//	Sample character
		ScriptFile.GetNextToken(false);
		wchar_t chSample = ScriptFile.m_szToken[0];

		if( index == 0 )
		{
			a_strcpy(szFirstFont, szName);
		}

		//	Register fonts
		HA3DFONT hFont = pFontMan->RegisterFontType(szName, iHei, dwFlags, chSample);
		if (!hFont)
			m_aFonts.Add(NULL);
		else
		{
			A3DFont* pFont = pFontMan->CreateA3DFont(hFont, 512, 128, 128);
			m_aFonts.Add(pFont);
		}

		index ++;
	}

	ScriptFile.Close();

	return true;
}

//	Release fonts
void CECGame::ReleaseFonts()
{
	if (!m_pA3DEngine)
		return;

	A3DFontMan* pFontMan = m_pA3DEngine->GetA3DFontMan();

	for (int i=0; i < m_aFonts.GetSize(); i++)
	{
		if (m_aFonts[i])
			pFontMan->ReleaseA3DFont(m_aFonts[i]);
	}

	m_aFonts.RemoveAll();
}

//	Get font object
A3DFont* CECGame::GetFont(int iIndex)
{
	if (m_aFonts[iIndex])
		return m_aFonts[iIndex];

	return m_pA3DEngine->GetSystemFont();
}

//	Get dynamic object path
const char* CECGame::GetDynObjectPath(DWORD dwDynObjID)
{
	return m_pDynObjPath ? m_pDynObjPath->GetPathByID(dwDynObjID) : NULL;
}

//  Get object coordinates
int CECGame::GetObjectCoord(ACString strTargetID, abase::vector<OBJECT_COORD>& TargetCoord)
{
	A3DVECTOR3 vRet(0,0,0);
	
	using namespace std;

	std::pair<std::multimap<ACString, OBJECT_COORD>::iterator, std::multimap<ACString, OBJECT_COORD>::iterator> pos;
	int count = m_CoordTab.count(strTargetID);
	switch(count)
	{
	case 0:
		break;
	case 1:
		{
			std::multimap<ACString, OBJECT_COORD>::iterator iter;
			iter = m_CoordTab.find(strTargetID);
			TargetCoord.push_back(iter->second);

			break;
		}
	default:
		{
			std::multimap<ACString, OBJECT_COORD>::iterator iter;
			pos = m_CoordTab.equal_range(strTargetID);
			for(iter = pos.first; iter != pos.second; ++iter)
			{
				TargetCoord.push_back(iter->second);
			}
			break;
		}	  
	}

	return count;
}

/*	Window message handler

	Return true if message was processed, otherwise return false.
*/
bool CECGame::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{		
	case WM_NCACTIVATE:
		if (wParam == FALSE)
		{
			//	主窗口失去焦点
			HWND hWndFocus = GetForegroundWindow();
			if (hWndFocus == GetGameInit().hWnd)
			{
				//	是子窗口导致，返回 true 以修改 wParam 以使 DefWndProc 显示为未失去焦点
				return true;
			}
			
			CECGameRun *pGameRun = GetGameRun();
			while (pGameRun)
			{
				CECUIManager *pUIMan = pGameRun->GetUIManager();
				if (!pUIMan)	break;
				
				CECBaseUIMan *pBaseUIMan = pUIMan->GetBaseUIMan();
				if (!pBaseUIMan)	break;
				
				if (pBaseUIMan->IsWebBrowser(hWndFocus))
				{
					//	是子窗口导致，返回 true 以修改 wParam 以使 DefWndProc 显示为未失去焦点
					return true;
				}
				
				break;
			}
		}
		return false;

	case WM_KILLFOCUS:
		if( m_bActive )
		{
			if (m_pA3DDevice && !m_pA3DDevice->GetDevFormat().bWindowed)
			{
				//	处理内置浏览器窗口
				if(IsWindow((HWND)wParam))
				{
					DWORD pid;
					GetWindowThreadProcessId((HWND)wParam,&pid);
					if(pid==GetCurrentProcessId())
					{
						break;
					}
				}

				ChangeDisplaySettings(NULL, CDS_RESET);
				ShowWindow(hWnd, SW_MINIMIZE);
				InvalidateRect(NULL, NULL, TRUE);
				m_pA3DDevice->SetNeedResetFlag(true);
			}
		}
		g_bFocused = false;
		OutputDebugStringA("WM_KILLFOCUS\n");
		break;

	case WM_SETFOCUS:
		g_bFocused = true;
		OutputDebugStringA("WM_SETFOCUS\n");
		break;

	case WM_ACTIVATE:
		if( wParam == WA_INACTIVE && m_bActive )
		{
			if (m_pA3DDevice && !m_pA3DDevice->GetDevFormat().bWindowed)
			{
				//	处理内置浏览器窗口
				if(IsWindow((HWND)lParam))
				{
					DWORD pid;
					GetWindowThreadProcessId((HWND)lParam,&pid);
					if(pid==GetCurrentProcessId())
					{
						break;
					}
				}

				ChangeDisplaySettings(NULL, CDS_RESET);
				ShowWindow(hWnd, SW_MINIMIZE);
				InvalidateRect(NULL, NULL, TRUE);
				m_pA3DDevice->SetNeedResetFlag(true);
			}
		}
		OutputDebugStringA("WM_ACTIVATE\n");
		break;

	case WM_ACTIVATEAPP:
		m_bActive = wParam ? true : false;

		//	处理内置浏览器窗口
		if(!m_bActive)
		{
			//SetForegroundWindow(hWnd);
			GUITHREADINFO guithreadinfo;
			guithreadinfo.cbSize = sizeof(GUITHREADINFO);
			if(GetGUIThreadInfo((DWORD)lParam,&guithreadinfo))
			{
				if(IsWindow(guithreadinfo.hwndActive))
				{
					DWORD pid;
					GetWindowThreadProcessId(guithreadinfo.hwndActive,&pid);
					if(pid==GetCurrentProcessId())
					{
						m_bActive = true;
					}
				}
			}
		}

		if( m_bActive )
		{
			SetAllVolume();
			if( m_pBackMusic )
				m_pBackMusic->RestartMusic();
		}
		else
		{
			if (m_pA3DDevice && !m_pA3DDevice->GetDevFormat().bWindowed)
			{
				ChangeDisplaySettings(NULL, CDS_RESET);
				ShowWindow(hWnd, SW_MINIMIZE);
				InvalidateRect(NULL, NULL, TRUE);
				m_pA3DDevice->SetNeedResetFlag(true);
			}
			if (!GetGameInit().bFullscreen)
			{
				//	主窗口失去焦点时，确保标题栏显示为灰色（原标题栏颜色修改为受IE控件影响）
				::PostMessage(g_pGame->GetGameInit().hWnd, WM_NCACTIVATE, FALSE, 0);
			}
		}
		OutputDebugStringA("WM_ACTIVATEAPP\n");
		break;

	case WM_PAINT:
		PAINTSTRUCT	ps;
		HDC hdc;
		hdc = BeginPaint(hWnd, &ps);
		if( m_pConfigs )
			BitBlt(hdc, 0, 0, m_pConfigs->GetSystemSettings().iRndWidth, m_pConfigs->GetSystemSettings().iRndHeight, NULL, 0, 0, BLACKNESS);
		EndPaint(hWnd, &ps);
		break;

	case WM_SETCURSOR:

		if (m_pA3DDevice && m_pA3DDevice->GetShowCursor())
		{
			if( LOWORD(lParam) != HTCLIENT )
			{
				return false;
			}
			else
			{
				A3DCursor* pCursor = m_pA3DDevice->GetCursor();
				if (pCursor)
					pCursor->OnSysSetCursor();
			}
		}
		else
			SetCursor(NULL);
		
		break;

	case WM_CLOSE:
    	if (g_GameCfgs.GetSystemSettings().bFullScreen) 
			ShowWindow(hWnd,SW_MINIMIZE);

		ShowWindow(hWnd,SW_HIDE);
	    Shell_NotifyIcon(NIM_ADD,&m_nid); 
		break;

	case WM_QUIT:
	case WM_DESTROY:

		//	Do nothing to mask ALT-F4
		break;

	case WM_SHOWWIND:
		  
		if(wParam!=IDI_MAINICON)   
        	break;  
         
		switch(lParam)   
        {   
          case WM_LBUTTONUP:
          {   
			   if (g_GameCfgs.GetSystemSettings().bFullScreen) 
			      ShowWindow(hWnd,SW_MAXIMIZE);
               
			   ShowWindow(hWnd,SW_SHOWNORMAL);
			   Shell_NotifyIcon(NIM_DELETE,&m_nid); 
          }
		}   

		break; 
	case WM_GETMINMAXINFO:
		{
			MINMAXINFO * pInfo = (MINMAXINFO *) lParam;
			pInfo->ptMaxSize.x = 9999;
			pInfo->ptMaxSize.y = 9999;
			pInfo->ptMinTrackSize.x = 400;
			pInfo->ptMinTrackSize.y = 400;
			pInfo->ptMaxTrackSize.x = 9999;
			pInfo->ptMaxTrackSize.y = 9999;
		}
		break;

	case WM_SIZING:
		{
			static const float r1 = 16.0f / 9.0f;
			static const float r2 = 9.0f / 16.0f;

			RECT * pDrag = (RECT *) lParam;
			int nWidth = pDrag->right - pDrag->left;
			int nHeight = pDrag->bottom - pDrag->top;

			switch(wParam)
			{
			case WMSZ_TOP:
				if( nWidth > nHeight * r1 )
					pDrag->top = (int)(pDrag->bottom - nWidth * r2);
				break;

			case WMSZ_BOTTOM:
				if( nWidth > nHeight * r1 )
					pDrag->bottom = (int)(pDrag->top + nWidth * r2);
				break;

			case WMSZ_BOTTOMLEFT:
			case WMSZ_BOTTOMRIGHT:
				if( nWidth > nHeight * r1 )
					pDrag->bottom = (int)(pDrag->top + nWidth * r2);
				break;

			case WMSZ_TOPLEFT:
			case WMSZ_TOPRIGHT:
				if( nWidth > nHeight * r1 )
					pDrag->top = (int)(pDrag->bottom - nWidth * r2);
				break;

			case WMSZ_LEFT:
				if( nWidth > nHeight * r1 )
					pDrag->left = (int)(pDrag->right - nHeight * r1);
				break;

			case WMSZ_RIGHT:
				if( nWidth > nHeight * r1 )
					pDrag->right = (int)(pDrag->left + nHeight * r1);
				break;
			}
		}
		break;

	case WM_SIZE:
		if( GetConfigs() && !GetConfigs()->IsAdjusting() && m_pA3DDevice )
		{
			int w, h;
			w = LOWORD(lParam);
			h = HIWORD(lParam);

			if( w == 0 || h == 0 )
				break;

			EC_SYSTEM_SETTING ss = GetConfigs()->GetSystemSettings();
			ss.iRndWidth = w;
			ss.iRndHeight = h;

			A3DViewport * pViewport = g_pGame->GetViewport()->GetA3DViewport();
			A3DVIEWPORTPARAM param1 = *pViewport->GetParam();
			
			GetConfigs()->SetSystemSettings(ss, true);
			GetConfigs()->SaveSystemSettings();
			AfxGetGFXExMan()->Resize2DViewport();

			A3DVIEWPORTPARAM param2 = *pViewport->GetParam();

			if (m_pGameRun)
			{
				A3DRECT rcOld(param1.X, param1.Y, param1.X+param1.Width, param1.Y+param1.Height);
				A3DRECT rcNew(param2.X, param2.Y, param2.X+param2.Width, param2.Y+param2.Height);
				m_pGameRun->OnWindowSizeChange(rcOld, rcNew);
			}
		}
		else
		{
			return false;
		}

		break;

	case WM_SENDPIPEDATA:
		if( wParam == 1 )
			CECMCDownload::GetInstance().SetDownloaderHWnd((HWND)lParam);
		else if( wParam == 2 )
			CECMCDownload::GetInstance().OnSendedDataProcessed();
		else
			CECMCDownload::GetInstance().OnPipeData((int)lParam);
		break;

	default:
		if (m_pGameRun)
			return m_pGameRun->DealWindowsMessage(message, wParam, lParam);

		return false;
	}

	return true;
}

extern HANDLE g_hExitGame;
extern HANDLE g_hMainThread;
extern HANDLE g_hRenderEvent;
extern HANDLE g_hRenderThread;
extern volatile bool g_bMainSuspened;
extern volatile bool g_bMultiThreadRenderMode;
extern CRITICAL_SECTION g_csRenderThread;

int CECGame::RunInRenderThread()
{
	int iRet = -1;

	while (1)
	{
		if (g_bExceptionOccured || ::WaitForSingleObject(g_hExitGame, 0) == WAIT_OBJECT_0)
		{
			iRet = 0;
			break;
		}

		if (g_bMainSuspened)
			::ResumeThread(g_hMainThread);

		WaitForSingleObject(g_hRenderEvent, 20);

		if (!OnceRun())
			break;
	}

	return iRet;
}

bool CECGame::RunInMainThread()
{
	if (!DispatchWindowsMessage())
	{
		a_LogOutput(1, "CECGame::Run(), break because DispatchWindowsMessage return false!");
		return false;
	}

	if (g_bMultiThreadRenderMode)
		return true;

	if (!GetA3DDevice()->GetAutoResetFlag() && GetA3DDevice()->GetNeedResetFlag())
	{
		SuspendLoadThread();

		if (!g_pGame->GetA3DDevice()->Reset())
			a_LogOutput(1, "CECGame::Run(), need reset the device, but we fail to call it!");

		// now restore some once set render states.
		if( m_pConfigs->GetSystemSettings().bMipMapBias )
		{
			float v = -1.0f;
			for(int i=0; i<m_pA3DDevice->GetMaxSimultaneousTextures(); i++)
			{
#ifdef ANGELICA_2_2
				m_pA3DDevice->SetSamplerState(i, D3DSAMP_MIPMAPLODBIAS, *(DWORD *)&v);
#else
				m_pA3DDevice->SetDeviceTextureStageState(i, D3DTSS_MIPMAPLODBIAS, *(DWORD *)&v);
#endif
			}
		}

		StartLoaderThread();
	}

	return OnceRun();
}

bool CECGame::OnceRun()
{
	static DWORD dwLastFrame = 0;
	static int iTickCnt = 0;
	static DWORD dwAccuTime = 0;
	static DWORD dwMGCTime = 0;	//	Memory garbage collect time

	ACSWrapper csa(&g_csRenderThread);

	if (dwLastFrame == 0)
	{
		dwLastFrame = a_GetTime();
		return true;
	}
	else
	{
		DWORD dwCurTime = a_GetTime();
		m_dwRealTickTime = dwCurTime - dwLastFrame;
		dwLastFrame = dwCurTime;
	}

	if (m_bDiscardFrame || !m_dwRealTickTime)
	{
		//	Discard this frame
		m_pGameRun->TickDiscardedFrame(m_dwRealTickTime);
		m_bDiscardFrame = false;
		return true;
	}

	//	Calculate average frame rate
	dwAccuTime += m_dwRealTickTime;
	if (++iTickCnt >= 15)
	{
		m_fAverFRate = 1000.0f / (dwAccuTime / 15.0f);

		iTickCnt	= 0;
		dwAccuTime	= 0;

		CECScriptMan * pScriptMan = GetGameRun()->GetUIManager()->GetScriptMan();
		if (pScriptMan) {
			pScriptMan->GetContext()->GetCheckState()->SetFrameRate((unsigned short)(m_fAverFRate + 0.5));
		}
	}

	//	Update network timer
	m_pGameSession->Update(m_dwRealTickTime);

	//  Update the script for the debug
	g_LuaStateMan.Tick(m_dwRealTickTime);

	//	Clamp logic tick time
	m_dwTickTime = m_dwRealTickTime;
	if (m_dwTickTime > 250)
		m_dwTickTime = 250;

	//	Game tick routine
	if (!m_pGameRun->Tick(m_dwTickTime))
	{
		a_LogOutput(1, "CECGame::Run(), break because CECGameRun::Tick return false");
		return false;
	}

	bool bRenderNoActive = !m_bActive && g_bRenderNoFocus && m_pA3DDevice && m_pA3DDevice->GetDevFormat().bWindowed;
	if (m_bActive || bRenderNoActive)
	{
		if (!m_pGameRun->Render())
		{
			a_LogOutput(1, "CECGame::Run(), break because CECGameRun::Render() return false!");
			return false;
		}

		if (m_bActive)
		{
			if( g_bFocused )
			{
				SetAllVolume();		//	每 Tick 都重设音量，以处理窗口切换可能带来无法静音问题
				m_pBackMusic->Update(m_dwTickTime);
				m_pBackMusic->UpdateSFX(m_dwTickTime);
				m_pBackMusic->UpdateCommentator(m_dwTickTime);
			}
		}
	}
	else
		Sleep(15);

	//	Do memory garbage collect
	if ((dwMGCTime += m_dwRealTickTime) >= 4000)
	{
		dwMGCTime = 0;
	//	a_MemGarbageCollect();
	}

	return true;
}

//	Add windows message
void CECGame::AddWindowsMessage(MSG& msg)
{
	::EnterCriticalSection(&m_csMsg);
	m_MsgList.AddTail(msg);
	::LeaveCriticalSection(&m_csMsg);
}

//	Dispatch windows message
bool CECGame::DispatchWindowsMessage()
{
	MSG msg;
	while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
			return false;

		if (PreTranslateMessage(msg))
			continue;

		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}

	return true;
}

bool CECGame::PreTranslateMessage(MSG &msg)
{
	ACSWrapper csa(&g_csRenderThread);

	bool bRet(false);

	CECGameRun *pGameRun = GetGameRun();
	while (pGameRun)
	{
		CECUIManager *pUIMan = pGameRun->GetUIManager();
		if (!pUIMan)	break;
		
		CECBaseUIMan *pBaseUIMan = pUIMan->GetBaseUIMan();
		if (!pBaseUIMan)	break;

		bRet = pBaseUIMan->PreTranslateMessage(msg);
		break;
	}

	return bRet;
}

//	Initialize A3D engine
bool CECGame::InitA3DEngine()
{
	//	Init Engine
	if (!(m_pA3DEngine = new A3DEngine))
	{
		glb_ErrorOutput(ECERR_NOTENOUGHMEMORY, "CECGame::InitA3DEngine", __LINE__);
		return false;
	}
	
	A3DDEVFMT devFmt;
	devFmt.bWindowed	= !m_GameInit.bFullscreen;
	devFmt.nWidth		= m_GameInit.iRenderWid;
	devFmt.nHeight		= m_GameInit.iRenderHei;
	devFmt.fmtTarget	= A3DFMT_UNKNOWN;
	devFmt.fmtDepth		= A3DFMT_UNKNOWN;
	devFmt.bVSync		= m_pConfigs->GetSystemSettings().bVSync;
						
	// A3DDEV_ALPHATARGETFIRST: if we use alpha target first and the desktop display mode is just what we use 
	// as the full-screen mode for the game, the game may not minimized when switched off. To solve this, we should
	// add a manual call to ChangeDisplaySetting when we receive WM_ACTIVATEAPP
	DWORD devFlags = A3DDEV_ALLOWMULTITHREAD;
	if( g_d3dcaps.PixelShaderVersion >= D3DPS_VERSION(1, 4) )
		devFlags |= A3DDEV_ALPHATARGETFIRST;

	if (!m_pA3DEngine->Init(m_GameInit.hInstance, m_GameInit.hWnd, &devFmt, devFlags))
	{
		glb_ErrorOutput(ECERR_FAILEDTOCALL, "CECGame::InitA3DEngine", __LINE__);
		return false;
	}

	g_pA3DConfig->RT_SetShowFPSFlag(false);

	g_pA3DConfig->SetTextureQuality((A3DTEXTURE_QUALITY)m_pConfigs->GetSystemSettings().iTexDetail);
	m_pA3DEngine->GetAMEngine()->GetAMConfig()->SetSoundQuality((AMSOUND_QUALITY)m_pConfigs->GetSystemSettings().iSoundQuality);

	m_pA3DDevice = m_pA3DEngine->GetA3DDevice();

	// In element client we use multithread to load terrain meshes etc.
	// so the reset operation can not be done by the device automatically, or we may not release all unmanaged
	// device objects before we reset, so reset will fail.
	m_pA3DDevice->SetAutoResetFlag(false);

	// show a black screen without loading text
	m_pA3DDevice->BeginRender();
	m_pA3DDevice->Clear(D3DCLEAR_TARGET, 0x00000000, 1.0f, 0);
	m_pA3DDevice->EndRender();
	m_pA3DDevice->Present();

	//	Create viewport
	if (!(m_pViewport = new CECViewport))
	{
		glb_ErrorOutput(ECERR_NOTENOUGHMEMORY, "CECGame::InitA3DEngine", __LINE__);
		return false;
	}

	CECViewport::INITPARAMS ViewParams;
	memset(&ViewParams, 0, sizeof (ViewParams));

	ViewParams.rcView.left		= 0;
	ViewParams.rcView.top		= 0;
	ViewParams.rcView.right		= m_GameInit.iRenderWid;
	ViewParams.rcView.bottom	= m_GameInit.iRenderHei;
	ViewParams.fMinZ			= 0.0f;
	ViewParams.fMaxZ			= 1.0f;
	ViewParams.bClearColor		= false;
	ViewParams.bClearZ			= true;
	ViewParams.colClear			= 0x00000000;
	ViewParams.fFOV				= DEG2RAD(DEFCAMERA_FOV);
	ViewParams.fNearPlane		= 0.2f;
	ViewParams.fFarPlane		= 2000.0f;

	if( m_pConfigs->GetSystemSettings().bWideScreen && m_GameInit.iRenderHei * 16 > m_GameInit.iRenderWid * 9 )
	{
		int nNewHeight = m_GameInit.iRenderWid * 9 / 16;
		ViewParams.rcView.top = (m_GameInit.iRenderHei - nNewHeight) >> 1;
		ViewParams.rcView.bottom = ViewParams.rcView.top + nNewHeight;
	}

	float w = (float)m_GameInit.iRenderWid;
	float h = (float)m_GameInit.iRenderHei;
	ViewParams.bOrthoCamera	= true;
	ViewParams.fOCLeft		= -w;
	ViewParams.fOCRight		= w;
	ViewParams.fOCBottom	= -h;
	ViewParams.fOCTop		= h;
	ViewParams.fOCZNear		= -1500.0f;
	ViewParams.fOCZFar		= 1500.0f;

	if (!m_pViewport->Init(ViewParams))
	{
		glb_ErrorOutput(ECERR_FAILEDTOCALL, "CECGame::InitA3DEngine", __LINE__);
		return false;
	}
	m_pViewport->Activate(false);

	//	Create directional light
	A3DLIGHTPARAM LightParams;
	memset(&LightParams, 0, sizeof (LightParams));

	//	Set default light parameters
	LightParams.Type = A3DLIGHT_DIRECTIONAL;
	LightParams.Direction = m_pConfigs->m_vDefLightDir;
	LightParams.Diffuse = m_pConfigs->m_dwDefLightDiff;
	LightParams.Specular = m_pConfigs->m_dwDefLightSpec;
	LightParams.Attenuation0 = 1.0f;
	if (!(m_pDirLight = CreateA3DLight(&LightParams)))
	{
		glb_ErrorOutput(ECERR_FAILEDTOCALL, "CECGame::InitA3DEngine", __LINE__);
		return false;
	}
	m_pDirLight->TurnOn();

	//	Create dynamic point light
	LightParams.Type = A3DLIGHT_POINT;
	LightParams.Specular = A3DCOLORVALUE(0.0f, 0.0f, 0.0f, 1.0f);
	if (!(m_pDynamicLight = CreateA3DLight(&LightParams)))
	{
		glb_ErrorOutput(ECERR_FAILEDTOCALL, "CECGame::InitA3DEngine", __LINE__);
		return false;
	}

	m_pDynamicLight->TurnOff();

	//	Set default ambient value and fog parameters
	m_pA3DDevice->SetAmbient(m_pConfigs->m_dwDefAmbient);
	m_pA3DDevice->SetFogEnable(true);
	m_pA3DDevice->SetFogStart(m_pConfigs->m_fDefFogStart);
	m_pA3DDevice->SetFogEnd(m_pConfigs->m_fDefFogEnd);
	m_pA3DDevice->SetFogDensity(m_pConfigs->m_fDefFogDensity);
	m_pA3DDevice->SetFogColor(m_pConfigs->m_dwDefFogColor);

	m_pA3DDevice->SetGammaLevel((int)(30+m_pConfigs->GetSystemSettings().iGamma*1.7f));

	//	Load skin model shaders
	A3DSkinMan* pSkinMan = m_pA3DEngine->GetA3DSkinMan();
#ifdef ANGELICA_2_2
	if (!pSkinMan->LoadSkinModelVertexShaders("Configs\\2.2\\skinmodelvs.cfg"))
#else
	if (!pSkinMan->LoadSkinModelVertexShaders("Configs\\skinmodelvs.cfg"))
#endif // ANGELICA_2_2
	{
		a_LogOutput(1, "CECGame::InitA3DEngine, Failed to load skinmodelvs.cfg");
		return false;
	}

	pSkinMan->SetDirLight(m_pDirLight);
	pSkinMan->SetDPointLight(m_pDynamicLight);
	
	if( m_pConfigs->GetSystemSettings().bMipMapBias )
	{
		float v = -1.0f;
		for(int i=0; i<m_pA3DDevice->GetMaxSimultaneousTextures(); i++)
		{
#ifdef ANGELICA_2_2
			m_pA3DDevice->SetSamplerState(i, D3DSAMP_MIPMAPLODBIAS, *(DWORD *)&v);
#else
			m_pA3DDevice->SetDeviceTextureStageState(i, D3DTSS_MIPMAPLODBIAS, *(DWORD *)&v);
#endif
		}
	}
	
	return true;
}

//	Release A3D engine
void CECGame::ReleaseA3DEngine()
{
	//	Release lights
	ReleaseA3DLight(m_pDirLight);
	ReleaseA3DLight(m_pDynamicLight);

	//	Release viewport
	if (m_pViewport)
	{
		delete m_pViewport;
		m_pViewport = NULL;
	}

	A3DRELEASE(m_pA3DEngine);
	m_pA3DDevice = NULL;
}

/*	Create a A3D Light object

	pParams: light's parameters, if this parameter is NULL, light's parameter must
			 be set through A3DLight::SetLightParam later.
*/
A3DLight* CECGame::CreateA3DLight(const A3DLIGHTPARAM* pParams)
{
	ASSERT(m_pA3DEngine);

	int i;

	for (i=0; i < MAXNUM_LIGHT; i++)
	{
		if (!m_aLights[i])
			break;
	}

	if (i >= MAXNUM_LIGHT)
		return NULL;

	A3DLight* pLight = NULL;

	//	Create a light
	m_pA3DEngine->GetA3DLightMan()->CreateLight(i, &pLight);

	if (!pLight)
	{
		a_LogOutput(1, "Failed to create A3D light");
		return NULL;
	}

	if (pParams)
		pLight->SetLightParam(*pParams);

	m_aLights[i] = pLight;

	return pLight;
}

//	Release A3D Light object
void CECGame::ReleaseA3DLight(A3DLight* pLight)
{
	if (!pLight)
		return;

	for (int i=0; i < MAXNUM_LIGHT; i++)
	{
		if (m_aLights[i] == pLight)
		{
			m_aLights[i] = NULL;
			break;
		}
	}

	m_pA3DEngine->GetA3DLightMan()->ReleaseLight(pLight);
}

//	Reset game, release all resources
bool CECGame::Reset()
{
	if (m_pSkinRender1)
	{
		m_pSkinRender1->ResetRenderInfo(true);
		m_pSkinRender1->ResetRenderInfo(false);
	}

	if (m_pSkinRender2)
	{
		m_pSkinRender2->ResetRenderInfo(true);
		m_pSkinRender2->ResetRenderInfo(false);
	}

	if (m_pGFXCaster)
		m_pGFXCaster->Reset();

	if (m_pA3DGFXExMan)
	{
		m_pA3DGFXExMan->Release();
		m_pA3DGFXExMan->Init(m_pA3DDevice);
	}

	if (m_pImageRes)
		m_pImageRes->Release();

	if (m_pA3DEngine)
	{
		if (!m_pA3DEngine->ResetResource())
		{
			a_LogOutput(1, "Failed to reset A3DEngine resources.");
			return false;
		}
	}

	if (m_pImageRes)
		m_pImageRes->LoadAllImages();

	//	Discard current frame
	m_bDiscardFrame = true;

	return true;
}

//	Load A3D skin model
A3DSkinModel* CECGame::LoadA3DSkinModel(const char* szFile, int iSkinFlag/* 0 */)
{
	A3DSkinModel* pModel = new A3DSkinModel;
	if (!pModel)
	{
		glb_ErrorOutput(ECERR_NOTENOUGHMEMORY, "CECGame::LoadA3DSkinModel", __LINE__);
		return false;
	}

	if (!pModel->Init(m_pA3DEngine))
	{
		delete pModel;
		glb_ErrorOutput(ECERR_FAILEDTOCALL, "CECGame::LoadA3DSkinModel", __LINE__);
		return false;
	}

	if (!pModel->Load(szFile, iSkinFlag))
	{
		delete pModel;
		glb_ErrorOutput(ECERR_FAILEDTOCALL, "CECGame::LoadA3DSkinModel", __LINE__);
		return false;
	}

	return pModel;
}

//	Release A3D skin model
void CECGame::ReleaseA3DSkinModel(A3DSkinModel* pModel)
{
	if (!pModel)
		return;

	pModel->Release();
	delete pModel;
}

//	Load A3D skin
A3DSkin* CECGame::LoadA3DSkin(const char* szFile, bool bUnique/* true */)
{
	A3DSkinMan* pSkinMan = m_pA3DEngine->GetA3DSkinMan();
	return pSkinMan->LoadSkinFile(szFile);
}

//	Release A3D skin
void CECGame::ReleaseA3DSkin(A3DSkin* pSkin, bool bUnique/* true */)
{
	A3DSkinMan* pSkinMan = m_pA3DEngine->GetA3DSkinMan();
	pSkinMan->ReleaseSkin(&pSkin);
}

//	Load a building model from file
CELBuilding* CECGame::LoadBuilding(const char* szFile)
{
	CELBuilding* pBuilding = new CELBuilding;
	if (!pBuilding)
	{
		glb_ErrorOutput(ECERR_NOTENOUGHMEMORY, "CECGame::LoadBuilding", __LINE__);
		return NULL;
	}

	AFileImage File;
	if (!File.Open("", szFile, AFILE_OPENEXIST | AFILE_BINARY | AFILE_TEMPMEMORY))
	{
		delete pBuilding;
		a_LogOutput(1, "CECGame::LoadBuilding, failed to open file %s", szFile);
		return NULL;
	}

	if (!pBuilding->Load(m_pA3DDevice, &File))
	{
		delete pBuilding;
		a_LogOutput(1, "CECGame::LoadBuilding, failed to load lit model %s", szFile);
		return NULL;
	}

	File.Close();

	return pBuilding;
}

//	Release building model
void CECGame::ReleaseBuilding(CELBuilding* pBuilding)
{
	A3DRELEASE(pBuilding);
}

//	Output a run-time debug string
void CECGame::RuntimeDebugInfo(DWORD dwCol, const ACHAR* szMsg, ...)
{
	if (!m_pRTDebug)
		return;

	if (!szMsg || !szMsg[0])
		return;

	ACHAR szBuf[512];

	va_list vaList;
	va_start(vaList, szMsg);
	glb_vsnprintf(szBuf, sizeof(szBuf)/sizeof(szBuf[0]), szMsg, vaList);
	va_end(vaList);

	m_pRTDebug->OutputDebugInfo(dwCol, szBuf);
}

//	Show or hide cursor
void CECGame::ShowCursor(bool bShow)
{
	m_pA3DDevice->ShowCursor(bShow);
}

//	Change current cursor
int CECGame::ChangeCursor(int iCursor)
{
	if (iCursor == m_iCurCursor)
		return iCursor;

	if (m_aCursors[iCursor])
		m_pA3DDevice->SetCursor(m_aCursors[iCursor]);

	// force show this cursor
	ShowCursor(g_pGame->GetA3DDevice()->GetShowCursor());

	if( l_idMainThread != GetCurrentThreadId() )
	{
		// ::SetCursor must be called from main thread to take effects, so here we should post a WM_SETCURSOR message
		// to ensure the main thread receive WM_SETCURSOR and update the cursor again
		PostMessage(m_GameInit.hWnd, WM_SETCURSOR, (WPARAM)m_GameInit.hWnd, MAKELPARAM(HTCLIENT, WM_MOUSEMOVE));
	}

	int iOldCursor = m_iCurCursor;
	m_iCurCursor = iCursor;
	return iOldCursor;
}

//	Set time error
void CECGame::SetServerTime(int iSevTime, int iTimeZoneBias)
{
	a_LogOutput(1, "SetServerTime, iSevTime = %d, iTimeZoneBias = %d", iSevTime, iTimeZoneBias);
	int iOldTimeError = m_iTimeError;
	m_iTimeError = iSevTime - time(NULL);
	m_iTimeZoneBias = iTimeZoneBias;

	struct tm serverLocal = GetServerLocalTime();
	int nTimeInDay = serverLocal.tm_hour * 3600 + serverLocal.tm_min * 60 + serverLocal.tm_sec;
	GetGameRun()->GetWorld()->GetSunMoon()->SetTimeOfTheDay(nTimeInDay / (4.0f * 3600.0f));
	
	S2C::player_wallow_info wallowinfo = GetGameRun()->GetWallowInfo();
	if (wallowinfo.anti_wallow_active)
	{
		wallowinfo.play_time += m_iTimeError - iOldTimeError;
		GetGameRun()->SetWallowInfo(wallowinfo);
	}

	m_AbsTimeStart = iSevTime;
	m_AbsTickStart = timeGetTime();
	a_LogOutput(1, "timeGetTime(), TickStart = %d", m_AbsTickStart);

	CECGameUIMan* pGameUI = GetGameRun()->GetUIManager()->GetInGameUIMan();
	if (pGameUI)
	{
		pGameUI->BuildFriendListEx();
	}
}

//	Get server GMT(UTC) time
int CECGame::GetServerGMTTime()
{ 
	return time(NULL) + m_iTimeError; 
}

int CECGame::GetServerAbsTime()
{
	DWORD curTick = timeGetTime();

	if(curTick < m_AbsTickStart)
	{
		// if player run this game more than 49.71 days...
		DWORD sec = (( (DWORD)(~0) - m_AbsTickStart + 1) +  curTick) / 1000;
		m_AbsTickStart = curTick;
		m_AbsTimeStart += sec;
		return m_AbsTimeStart;
	}
	else
	{
		DWORD sec = (curTick - m_AbsTickStart) / 1000;
		return m_AbsTimeStart + sec;
	}
}

struct tm CECGame::GetServerLocalTime()
{
	time_t serverTime = GetServerGMTTime();
	serverTime -= m_iTimeZoneBias * 60; // UTC = localtime + bias => localtime = UTC - bias
	return *gmtime(&serverTime);
}

tm   CECGame::GetServerLocalTime(int t)
{
	//	将服务器传来的秒数，转化为服务器本地的时间（用于显示或判断）
	//
	int timeBias = GetTimeZoneBias() * 60;
	int serverLocalTime = t - timeBias;
	if (serverLocalTime < 0){
		ASSERT(false);
		serverLocalTime = 0;
		a_LogOutput(1, "CECGame::GetServerLocalTime, invalid parameter (%d) with GetTimeZoneBias()=%d and GetTimeError()=%d", t, GetTimeZoneBias(), GetTimeError());
	}
	return *gmtime((time_t*)&serverLocalTime);
}

bool CECGame::LoadLoginRes()
{
	return true;
}

void CECGame::ReleaseLoginRes()
{
}

bool CECGame::LoadInGameRes()
{
	return true;
}

void CECGame::ReleaseInGameRes()
{
}

//	Get game version
DWORD CECGame::GetGameVersion()
{
	return GAME_VERSION;
}

//	Get game build
DWORD CECGame::GetGameBuild()
{
	return GAME_BUILD;
}

AMSoundBufferMan* CECGame::GetAMSoundBufferMan()
{
	return m_pA3DEngine->GetAMEngine()->GetAMSoundEngine()->GetAMSoundBufferMan();
}

//	Get item's extend description string
const wchar_t* CECGame::GetItemExtDesc(int tid)
{
	return m_ItemExtDesc.GetWideString(tid);
}

//	Get buff description string
const wchar_t* CECGame::GetBuffDesc(int id)
{
	return m_BuffDesc.GetWideString(id);
}

//	Load item extend properties
bool CECGame::LoadItemExtProps()
{
	//	Load item extend properties
	AScriptFile sf;
	if (!sf.Open("Configs\\item_ext_prop.txt"))
	{
		a_LogOutput(1, "CECGame::LoadItemExtProps, failed to open file item_ext_prop.txt");
		return false;
	}

	BYTE byType = 0xff;

	while (sf.GetNextToken(true))
	{
		//	Get current tpye
		byType = (BYTE)sf.GetNextTokenAsInt(false);

		if (!sf.MatchToken("{", false))
		{
			ASSERT(0);
			return false;
		}

		while (sf.PeekNextToken(true))
		{
			if (!stricmp(sf.m_szToken, "}"))
			{
				sf.GetNextToken(true);
				break;
			}

			int idProp = sf.GetNextTokenAsInt(true);
			if (!m_ItemExtProps.put(idProp, byType))
			{
				//	Property id collision, shouldn't happen
				ASSERT(0);
			}
		}
	}

	sf.Close();

	//	Load item color table
	if (sf.Open("Configs\\item_color.txt"))
	{
		while (sf.PeekNextToken(true))
		{
			//	Get item id
			int idItem = sf.GetNextTokenAsInt(true);
			//	Get color index
			BYTE byCol = (BYTE)sf.GetNextTokenAsInt(false);

			if (!m_ItemColTab.put(idItem, byCol))
			{
				//	Property id collision, shouldn't happen
				a_LogOutput(1, "Configs\\item_color.txt, repeated id(%d) ignored", idItem);
				ASSERT(0);
			}
		}

		sf.Close();
	}
	else
	{
		a_LogOutput(1, "CECGame::LoadItemExtProps, failed to open file item_color.txt");
	}

	return true;
}
//  Load objects coordinates data
bool CECGame::LoadObjectCoord()
{
	AScriptFile sf;
	if (!sf.Open("Configs\\Coord_data.txt"))
	{
		a_LogOutput(1, "CECGame::LoadObjectCoord, failed to open file Coord_data.txt");
		return false;
	}


	while (sf.PeekNextToken(true))
	{
		OBJECT_COORD objectCoord;
		
		sf.GetNextToken(true);
		if( 0 == strcmpi(sf.m_szToken, "#") ||
			0 == strcmpi(sf.m_szToken, "//") )
		{
			sf.SkipLine();
		}
		else
		{
			//	Get object id
			ACString strName = AS2AC(sf.m_szToken);

			//  Get the map this object belonging to
			if(sf.PeekNextToken(false))
			{
				sf.GetNextToken(false);
				objectCoord.strMap = AS2AC(sf.m_szToken);
			}
			else
				continue;
			
			float _x, _y, _z;
			//  If less than 3 coordinates were found, there must be some MISTAKE, so we skip
			if(sf.PeekNextToken(false))
				_x = sf.GetNextTokenAsFloat(false);
			else 
				continue;
			
			if(sf.PeekNextToken(false))
				_y = sf.GetNextTokenAsFloat(false);
			else 
				continue;
			
			if(sf.PeekNextToken(false))
				_z = sf.GetNextTokenAsFloat(false);
			else
				continue;

			//  More data were found, maybe exception occers, so we skip current line
			if(sf.PeekNextToken(false))
			{
				sf.SkipLine();
				continue;
			}

			objectCoord.vPos = A3DVECTOR3(_x, _y, _z);
			m_CoordTab.insert(std::make_pair(strName, objectCoord));
		}
	}

	sf.Close();	

	return true;
}

//  Load consume reward data
bool CECGame::LoadConsumeReward()
{
	AIniFile IniFile;

	if (!IniFile.Open("Configs\\Consume_Reward.cfg"))
	{
		a_LogOutput(1, "CECGame::LoadConsumeReward, Failed to open file Consume_Reward.cfg");
		return false;
	}

	AString strSect = "ConsumeReward";

	m_ConsumeReward.bOpen = IniFile.GetValueAsBoolean(strSect, "open", true);

	int temp[4], i=0,j=0;
	IniFile.GetValueAsIntArray(strSect, "begin_time", 4, temp);
	for(i=0;i<4;i++)
		m_ConsumeReward.iBeginTime[i] = temp[i];

	IniFile.GetValueAsIntArray(strSect, "end_time", 4, temp);
	for(i=0;i<4;i++)
		m_ConsumeReward.iEndTime[i] = temp[i];


	IniFile.GetValueAsIntArray(strSect, "reward_time", 2, temp);
	for(i=0;i<2;i++)
		m_ConsumeReward.iRewardTime[i] = temp[i];


	for(i=0;i<7;i++)
	{
		char strTemp[20];
		sprintf(strTemp, "cash_return_type_%d", i);
		IniFile.GetValueAsIntArray(strSect, strTemp, 2, temp);
		for(j=0;j<2;j++)
			m_ConsumeReward.iRewardType[i][j] = temp[j];
	}

	IniFile.Close();

	return true;
}

//  Load the pet auto skill table
bool CECGame::LoadPetAutoSkill()
{
	AScriptFile sf;
	if (!sf.Open("Configs\\petautoskill.txt"))
	{
		a_LogOutput(1, "CECGame::LoadPetAutoSkill, failed to open file petautoskill.txt");
		return false;
	}

	while (sf.PeekNextToken(true))
	{
		abase::vector<int> skillList;
		
		sf.GetNextToken(true);
		if( 0 == strcmpi(sf.m_szToken, "#") ||
			0 == strcmpi(sf.m_szToken, "//") )
		{
			sf.SkipLine();
		}
		else
		{
			//  Read the skill id
			int skill_id = atoi(sf.m_szToken);
			if( skill_id ) m_PetAutoSkills.insert(skill_id);
		}
	}

	sf.Close();
	return true;
}

//	Build suite equipment table
void CECGame::BuildSuiteEquipTab()
{
	if (!m_pElementDataMan)
		return;

	DATA_TYPE DataType = DT_INVALID;
	
	int tid = m_pElementDataMan->get_first_data_id(ID_SPACE_ESSENCE, DataType);

	while (tid)
	{
		const void* pData = m_pElementDataMan->get_data_ptr(tid, ID_SPACE_ESSENCE, DataType);

		switch (DataType){
		case DT_SUITE_ESSENCE:
			{
				SUITE_ESSENCE* pSuiteEss = (SUITE_ESSENCE*)pData;
				pSuiteEss->max_equips = 0;
				for (int i=0; i<12; i++)
				{
					if( pSuiteEss->equipments[i].id )
					{
						pSuiteEss->max_equips ++;
						bool bVal = m_SuiteEquipTab.put(pSuiteEss->equipments[i].id, tid);
						ASSERT(bVal);
					}
				}
				break;
			}
		case DT_POKER_SUITE_ESSENCE:
			{
				POKER_SUITE_ESSENCE* pSuiteEss = (POKER_SUITE_ESSENCE*)pData;
				for (int i=0; i<6; i++)
				{
					if( pSuiteEss->list[i] )
					{
						bool bVal = m_SuiteEquipTab.put(pSuiteEss->list[i], tid);
						ASSERT(bVal);
					}
				}
				break;
			}
		case DT_FASHION_SUITE_ESSENCE:
			{
				FASHION_SUITE_ESSENCE *pSuiteEss = (FASHION_SUITE_ESSENCE *)pData;
				for (int i(0); i < FASHION_SUITE_NUM; ++ i){
					if (pSuiteEss->list[i]){
						bool bVal = m_SuiteFashionTab.put(pSuiteEss->list[i], tid);
						ASSERT(bVal);
					}
				}
				break;
			}
		}

		//	Get next item
		tid = m_pElementDataMan->get_next_data_id(ID_SPACE_ESSENCE, DataType);
	}
}

//	Reset privilege
bool CECGame::ResetPrivilege(void* pBuf, int iDataSize)
{
	using namespace GNET;

	if (!m_pPrivilege)
	{
		if (!(m_pPrivilege = new Privilege))
			return false;
	}

	if (pBuf && iDataSize)
		m_pPrivilege->Init(pBuf, iDataSize);
	else
		m_pPrivilege->Reset();

	return true;
}

//	Render high-light model
bool CECGame::RenderHighLightModel(CECViewport* pView, CECModel* pModel)
{
	if (!pModel || !pModel->GetA3DSkinModel())
		return false;

	A3DSkinModel* pSkinModel = pModel->GetA3DSkinModel();

	A3DCOLORVALUE col = pSkinModel->GetExtraEmissive();
	pSkinModel->SetExtraEmissive(A3DCOLORVALUE(1.0f, 1.0f, 1.0f, 0.0f));

	//	使用非立即方式渲染
	pModel->Render(pView->GetA3DViewport());

	//	记录高亮显示前的 emissive 颜色分量，在本次渲染完成后恢复
	m_HighlightRecords.push_back(HighlightRecord(pSkinModel, col));

	//	A3DSkinRender* psr = m_pA3DEngine->GetA3DSkinMan()->GetCurSkinRender();
	//	A3DTEXTUREOP colOP = psr->GetColorOP();
	//	psr->SetColorOP(A3DTOP_MODULATE2X);
	//	pModel->Render(pView, true);
	//	psr->SetColorOP(colOP);
	return true;
}

bool CECGame::RenderHighLightModel(CECViewport* pView, A3DSkinModel* pModel)
{
	if (!pModel)
		return false;

	A3DCOLORVALUE col = pModel->GetExtraEmissive();
	pModel->SetExtraEmissive(A3DCOLORVALUE(1.0f, 1.0f, 1.0f, 0.0f));
	
	//	使用非立即方式渲染
	pModel->Render(pView->GetA3DViewport());

	//	记录高亮显示前的 emissive 颜色分量，在本次渲染完成后恢复
	m_HighlightRecords.push_back(HighlightRecord(pModel, col));

	//	A3DSkinRender* psr = m_pA3DEngine->GetA3DSkinMan()->GetCurSkinRender();
	//	A3DTEXTUREOP colOP = psr->GetColorOP();
	//	psr->SetColorOP(A3DTOP_MODULATE2X);
	//	pModel->RenderAtOnce(pView);
	//	psr->SetColorOP(colOP);
	return true;
}

void CECGame::PrepareHighlightModel()
{
	//	准备记录高亮显示模型

	ASSERT(m_HighlightRecords.empty());
	m_HighlightRecords.clear();
}

void CECGame::RestoreHightlightModel()
{
	//	恢复本次渲染中高亮显示模型到正常状态

	//	采取堆栈的方式处理高亮显示模型的恢复
	size_t count = m_HighlightRecords.size();
	for (int i = count-1; i >= 0; -- i)
	{
		const HighlightRecord &r = m_HighlightRecords[i];
		r.pSkinModel->SetExtraEmissive(r.lastEmissive);
	}

	//	清除已恢复的高亮显示模型
	m_HighlightRecords.clear();
}

//	Get item name color
int CECGame::GetItemNameColorIdx(int tid, int iDefIndex/* 0 */)
{
	int iIndex = iDefIndex;

	ItemColTable::pair_type Pair = m_ItemColTab.get(tid);
	if (Pair.second)
		iIndex = *Pair.first;

	if (iIndex < 0 || iIndex >= 10)
	{
		ASSERT(iIndex >= 0 && iIndex < 10);
		iIndex = 0;
	}

	return iIndex;
}

DWORD CECGame::GetItemNameColor(int tid, int iDefIndex/* 0 */)
{
	int iIndex = GetItemNameColorIdx(tid, iDefIndex);
	return l_aNameCols[iIndex];
}

void CECGame::SetTrayIconHint(ACString StrHint)
{
 	a_strcpy(m_nid.szTip, StrHint);
}

ACString CECGame::GetFormattedPrice(__int64 i)
{
	ACString szRes;

	CECStringTab* pDescTab = GetItemDesc();
	ACString strSeperator = pDescTab->GetWideString(ITEMDESC_PRICE_SEPERATOR);
	if (strSeperator.IsEmpty())
		strSeperator = _AL(",");

	if( i < 1000 )
		szRes.Format(_AL("%d"), (int)i);
	else if( i < 1000000 )
		szRes.Format(_AL("%d%s%03d"), (int)i / 1000, strSeperator, (int)i % 1000);
	else if( i < 1000000000 )
		szRes.Format(_AL("%d%s%03d%s%03d"), (int)i / 1000000, strSeperator, (int)i / 1000 % 1000, strSeperator, (int)i % 1000);
	else
	{
		int nHigh = int(i / 1000000000);
		int nLow = int(i % 1000000000);

		szRes.Format(_AL("%d%s%03d%s%03d%s%03d"), nHigh, strSeperator, nLow / 1000000 % 1000, strSeperator, nLow / 1000 % 1000, strSeperator, nLow % 1000);
	}
	return szRes;
}

void CECGame::SetAllVolume(const EC_SYSTEM_SETTING &ss)
{
	if (m_pA3DEngine && m_pA3DEngine->GetAMSoundEngine())
	{
		m_pA3DEngine->GetAMSoundEngine()->SetVolume(ss.nSoundVol);
	}
	if (m_pBackMusic)
	{
		m_pBackMusic->SetVolume(ss.nMusicVol);
		m_pBackMusic->SetSFXVolume(ss.nSoundVol);
		m_pBackMusic->SetCommentatorVolume(ss.nSoundVol);
	}
}

void CECGame::SetAllVolume()
{
	if (GetGameRun() && GetGameRun()->GetUIManager()){
		if (CECGameUIMan* pGameUI = GetGameRun()->GetUIManager()->GetInGameUIMan()){
			PAUIDIALOG pDlg = pGameUI->GetDialog("Win_SettingSystem");
			if (pDlg && pDlg->IsShow()){
				//	应用临时结果
				CDlgSettingSystem* pDlgSettingSystem = dynamic_cast<CDlgSettingSystem *>(pDlg);
				pDlgSettingSystem->MakeChangeNow();
				return;
			}
		}
	}
	if (m_pConfigs){
		SetAllVolume(m_pConfigs->GetSystemSettings());
	}
}

bool CECGame::IsPetAutoSkill(int skill_id) const
{
	return m_PetAutoSkills.find(skill_id) != m_PetAutoSkills.end();
}
