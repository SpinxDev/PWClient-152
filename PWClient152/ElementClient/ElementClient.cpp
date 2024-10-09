/*
 * FILE: ElementClient.cpp
 *
 * DESCRIPTION: ElementClient, Main file of game
 *
 * CREATED BY: Duyuxin, 2003/12/11
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#define _WIN32_WINDOWS 0x0410 

#include "EC_Global.h"
#include "EC_Configs.h"
#include "EC_Game.h"
#include "EC_SceneLoader.h"
#include "EC_Utility.h"
#include "EC_GameRun.h"
#include "EC_InputCtrl.h"
#include "ACSWrapper.h"
#include "defence/defence.h"
#include "defence/infocollection.h"
#include "defence/APILoader.h"
#include "defence/cheaterkiller.h"
#include "defence/stackchecker.h"
#include "defence/imagechecker.h"
#include "defence/pwacp.h"
#include "EC_GameUIMan.h"
#include "EC_UIManager.h"
#include "EC_Model.h"
#include "EC_CommandLine.h"
#include "GT/gt_overlay.h"
#include "Arc/arc_overlay.h"
#include "Arc/Asia/EC_ArcAsia.h"
#include "EC_TimeSafeChecker.h"
#include "EC_WinAPI.h"

#ifdef _PROFILE_MEMORY
#include "Memory\EC_HookMemory.h"
#include <DbgHelp.h>
#endif

#include "AUIEditBox.h"

#include <A3DPlatform.h>
#include <A3DTypes.h>
#include <AF.h>
#include <ddraw.h>
#include "iolib.h"
#include <A3DDevice.h>
#include <A3DCursor.h>

#include "elementpckdir.h"
#include "resource.h"

#include <stdlib.h>
#include <stdio.h>

#include "LuaBind.h"
#include "LuaState.h"
#include "LuaUtil.h"
#include "LuaScript.h"
#include "ScriptValue.h"
#include "LuaDlgApi.h"

#include <AFilePackMan.h>

#define new A_DEBUG_NEW

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////
#ifdef ANGELICA_2_2
D3DCAPS9		g_d3dcaps;
#else
D3DCAPS8		g_d3dcaps;
#endif

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

//	Parameters can be provided using program command line
struct s_CMDLINEPARAMS
{
	int		iFullScreen;	//	1: fullscreen; 0: windowed
	int		iRenderWid;		//	Render size
	int		iRenderHei;
	ACHAR	szIPAddr[256];	//	Link server IP address
	int		iPort;			//	Link server port
	int		iConsole;		//	Console enable flag
};

static s_CMDLINEPARAMS l_CmdParams;

static HINSTANCE l_hInstance = NULL;
static HWND	l_hMainWnd = NULL;
static const ACHAR* l_szClassName = _AL("ElementClient Window");

static CECGame l_theGame;

CECConfigs g_GameCfgs;

static bool l_bHasAntiHackerMsg = false;
ACString	l_AntiHackerMsg;
BOOL		g_bHookExitProcess = TRUE;

///////////////////////////////////////////////////////////////////////////
//
//	Local functions
//
///////////////////////////////////////////////////////////////////////////

static void _ParseCommandLine(ACHAR* szCmdLine);
static void _DefaultCmdParams();
static bool _InitGameApp();
static void _OutputSystemVersion();
static void _FinalizeGameApp();
static ATOM _RegisterWndClass(HINSTANCE hInstance);
static bool _CreateMainWnd(HINSTANCE hInstance, int nCmdShow, bool bFullScreen);
static LRESULT CALLBACK _WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static bool _InitNetwork();
static void _FinalizeNetwork();

#define ENABLE_UNKNOWN_EXIT_CHECK 
#ifdef ENABLE_UNKNOWN_EXIT_CHECK
	static bool l_bEnableExitCheck = false;
	void on_unknown_exit(void)
	{
		if( l_bEnableExitCheck )
		{
			const char * szErr = "abcdefghijklmn";
			memset((void *)szErr, 0, 4);
		}
	}
#endif

//	Get command line parameters
bool glb_IsConsoleEnable() { return l_CmdParams.iConsole ? true : false; }
void glb_EnableConsole(bool bEnable){ l_CmdParams.iConsole = bEnable ? 1 : 0; }

//	clear files in log folder
bool prepare_file_structures()
{
	const char * szCachePath = "cache";
	// now try to create a cache dir
	if( _access(szCachePath, 0) == -1 )
	{
		if( !CreateDirectoryA(szCachePath, NULL) )
		{
			MessageBoxA(NULL, "无法创建cache目录，游戏无法运行！\n请确保硬盘有足够的剩余空间再运行本游戏！", "错误", MB_OK | MB_ICONSTOP);
			return false;
		}
	}

	const char * szScreenShotsPath = "screenshots";
	// now try to create a cache dir
	if( _access(szScreenShotsPath, 0) == -1 )
	{
		if( !CreateDirectoryA(szScreenShotsPath, NULL) )
		{
			MessageBoxA(NULL, "无法创建screenshots目录，游戏无法运行！\n请确保硬盘有足够的剩余空间再运行本游戏！", "错误", MB_OK | MB_ICONSTOP);
			return false;
		}
	}

	const char * szLogPath = "logs";
	// now try to create a cache dir
	if( _access(szLogPath, 0) == -1 )
	{
		if( !CreateDirectoryA(szLogPath, NULL) )
		{
			MessageBoxA(NULL, "无法创建logs目录，游戏无法运行！\n请确保硬盘有足够的剩余空间再运行本游戏！", "错误", MB_OK | MB_ICONSTOP);
			return false;
		}
	}

	// now remove logs generated in last game run
	WIN32_FIND_DATAA fd;
	HANDLE hFind = FindFirstFileA("logs\\*.*", &fd);
	if( INVALID_HANDLE_VALUE == hFind )
		return true;

	do
	{
		char szFileName[MAX_PATH];
		
		sprintf(szFileName, "logs\\%s", fd.cFileName);
		strlwr(szFileName);
		
		if( fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
		{
			// ignore folders
			continue;
		}
		else
		{
			//	we only delete .log and .dmp files in Logs folder
			const char * pszExt = strstr(szFileName, ".log");
			if( !pszExt )
			{
				pszExt = strstr(szFileName, ".dmp");
				if( !pszExt )
					pszExt = strstr(szFileName, ".bin");
			}

			if( pszExt && pszExt + 4 == szFileName + strlen(szFileName) )
			{
				DeleteFileA(szFileName);
			}
		}

	} while( FindNextFileA(hFind, &fd) );

	if( af_IsFileExist("logs\\dumpfile") )
		g_bTrojanDumpLastTime = true;
	
	return true;
}

bool CheckHardwareCaps(const char *& pszInfo)
{
	pszInfo = "未知的错误";

	// first see if NetMeeting is running, if so just pop up a warning.
	if( FindWindow(_AL("MPWClass"), NULL) )
	{
		MessageBoxA(NULL, "游戏程序检测到您的机器上有NetMeeting正在运行！\n"
			"由于NetMeeting运行时其他程序无法使用显卡的三维加速功能，所以请先关闭NetMeeting 后再进行游戏！", 
			"警告", MB_OK | MB_ICONINFORMATION);
	}

	HRESULT hval;

	// first get vertex shader version to determine whether or not create sw vp device
#ifdef ANGELICA_2_2
	IDirect3D9 *	pD3D;
	pD3D = Direct3DCreate9(D3D_SDK_VERSION);
#else
	IDirect3D8 *	pD3D;
	pD3D = Direct3DCreate8(D3D_SDK_VERSION);
#endif // ANGELICA_2_2
	if( pD3D == NULL )
	{
		pszInfo = "无法创建Direct3D对象，请检查DirectX版本";
		return false;
	}
	pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &g_d3dcaps);
	if( g_d3dcaps.MaxSimultaneousTextures < 2 || g_d3dcaps.MaxTextureBlendStages < 2 )
	{
		pszInfo = "显卡至少要支持两层纹理的多层纹理映射！";
		return false;
	}

	D3DFORMAT fmtTarget16 = D3DFMT_UNKNOWN;
	D3DFORMAT fmtTarget32 = D3DFMT_UNKNOWN;
	hval = pD3D->CheckDeviceType(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_R5G6B5, D3DFMT_R5G6B5, false);
	if( D3D_OK != hval )
	{
		hval = pD3D->CheckDeviceType(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_X1R5G5B5, D3DFMT_X1R5G5B5, false);
		if( D3D_OK != hval )
		{
			pszInfo = "无法获得16位模式的显示格式，请检查您的显卡驱动以及DirectX的版本";
			return false;
		}
		else
			fmtTarget16 = D3DFMT_X1R5G5B5;
	}
	else
		fmtTarget16 = D3DFMT_R5G6B5;

	hval = pD3D->CheckDeviceType(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_X8R8G8B8, D3DFMT_X8R8G8B8, false);
	if( D3D_OK == hval )
		fmtTarget32 = D3DFMT_X8R8G8B8;

	D3DFORMAT fmtTarget = fmtTarget16;
	if( fmtTarget32 != D3DFMT_UNKNOWN )
		fmtTarget = fmtTarget32;
	
	/*
	// first check if DXT1 and DXT3 is supported;
	hval = pD3D->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, fmtTarget, 0, D3DRTYPE_TEXTURE, D3DFMT_DXT1);
	if( D3D_OK != hval )
	{
		pszInfo = "贴图格式DXT1不被支持，显卡硬件性能不符合要求";
		return false;
	}

	hval = pD3D->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, fmtTarget, 0, D3DRTYPE_TEXTURE, D3DFMT_DXT3);
	if( D3D_OK != hval )
	{
		pszInfo = "贴图格式DXT3不被支持，显卡硬件性能不符合要求";
		return false;
	}*/
	
	pD3D->Release(); 

	// now query video memory through ddraw interface
	IDirectDraw * pDD;
	IDirectDraw2 * pDD2;
	DirectDrawCreate(NULL, &pDD, NULL);
	hval = pDD->QueryInterface(IID_IDirectDraw2, (LPVOID *) &pDD2);
	if( DD_OK != hval )
	{
		pszInfo = "无法创建DirectDraw2对象，请检查DirectX版本";
		return false;
	}
	pDD->Release();

	DDSCAPS		ddscaps;
	ddscaps.dwCaps = DDSCAPS_LOCALVIDMEM;
	DWORD dwLocalTotal, dwLocalFree;
    hval = pDD2->GetAvailableVidMem(&ddscaps, &dwLocalTotal, &dwLocalFree);
	if( DD_OK != hval )
	{
		pszInfo = "无法获取显存大小，请检查DirectX版本";
		return false;
	}
	ddscaps.dwCaps = DDSCAPS_TEXTURE;
	DWORD dwTextureTotal, dwTextureFree;
    hval = pDD2->GetAvailableVidMem(&ddscaps, &dwTextureTotal, &dwTextureFree);
	if( DD_OK != hval )
	{
		pszInfo = "无法获取显存大小，请检查DirectX版本";
		return false;
	}

	pDD2->Release();

	const char * szTexQuality[] = {"高", "中", "低"};
	char  szInfo[256];

	int iTexDetail = g_GameCfgs.GetSystemSettings().iTexDetail;
	iTexDetail = min(iTexDetail, 2);
	iTexDetail = max(iTexDetail, 0);
	if( dwLocalTotal < 16 * 1024 * 1024 )
	{
		if( iTexDetail != 2 )
		{
			sprintf(szInfo, "您的显卡的显存小于16M，而您的贴图精度选择了[%s]，这将使您在游戏中很卡！建议您将贴图精度调低后再进行游戏！\n\n是否让系统自动选择合适的贴图精度来进行游戏？", szTexQuality[iTexDetail]);
			if( IDYES == MessageBoxA(NULL, szInfo, "贴图精度过高", MB_YESNO | MB_ICONQUESTION) )
			{
				g_GameCfgs.GetSystemSettingsPtr()->iTexDetail = 2;
			}
		}
	}
	else if( dwLocalTotal < 32 * 1024 * 1024 )
	{
		if( iTexDetail != 2 )
		{
			sprintf(szInfo, "您的显卡只有32M显存，而您的贴图精度选择了[%s]，这将使您在游戏中很卡！建议您将贴图精度调低后再进行游戏！\n\n是否让系统自动选择合适的贴图精度来进行游戏？", szTexQuality[iTexDetail]);
			if( IDYES == MessageBoxA(NULL, szInfo, "贴图精度过高", MB_YESNO | MB_ICONQUESTION) )
			{
				g_GameCfgs.GetSystemSettingsPtr()->iTexDetail = 2;
			}
		}
	}
	else if( dwLocalTotal < 64 * 1024 * 1024 )
	{
		if( iTexDetail == 0 )
		{
			sprintf(szInfo, "您的显卡有64M显存，而您的贴图精度选择了[%s]，这将使您在游戏中很卡！建议您将贴图精度调低后再进行游戏！\n\n是否让系统自动选择合适的贴图精度来进行游戏？", szTexQuality[iTexDetail]);
			if( IDYES == MessageBoxA(NULL, szInfo, "贴图精度过高", MB_YESNO | MB_ICONQUESTION) )
			{
				g_GameCfgs.GetSystemSettingsPtr()->iTexDetail = 1;
			}
		}
	}

	DWORD dwMaxTextureNeeded = (DWORD)(16 * 1024 * 1024 * pow(2.0, 2 - g_GameCfgs.GetSystemSettingsPtr()->iTexDetail));
	if( dwTextureFree < dwMaxTextureNeeded )
	{
		sprintf(szInfo, "您的系统剩余的可用于贴图的显存小于16M，在游戏的过程中可能会导致花屏、死机等严重的错误！您是否还要进入游戏？");
		if( IDYES != MessageBoxA(NULL, szInfo, "贴图精度过高", MB_YESNO | MB_ICONQUESTION) )
		{
			pszInfo = "系统剩余的可用于贴图的显存小于16M，用户取消了游戏运行。";
			return false;		
		}		
	}

	glb_RepairExeInMemory();
	return true;
}

bool CheckImportantFiles(const char *& pszInfo)
{
	const char * szImportantFiles[] = {
		"Models\\Players\\形象\\白虎\\白虎.ecm",
		"Models\\Players\\形象\\武侠男\\躯干\\武侠男.ecm",
		"Models\\Players\\形象\\武侠女\\躯干\\武侠女.ecm",
		"Models\\Players\\形象\\法师男\\躯干\\法师男.ecm",
		"Models\\Players\\形象\\法师女\\躯干\\法师女.ecm",
		"Models\\Players\\形象\\羽灵男\\躯干\\羽灵男.ecm",
		"Models\\Players\\形象\\羽灵女\\躯干\\羽灵女.ecm",
		"Models\\Players\\形象\\妖兽男\\躯干\\妖兽男.ecm",
		"Models\\Players\\形象\\剑灵男\\躯干\\剑灵男.ecm",
		"Models\\Players\\形象\\魅灵女\\躯干\\魅灵女.ecm",
		"font\\FZL2JW.ttf",
		"font\\FZLBJW.ttf",
		"font\\FZXH1JW.ttf",
	};

	for(int i=0; i<sizeof(szImportantFiles) / sizeof(const char *); i++)
	{
		AFileImage file;

		if( !file.Open(szImportantFiles[i], AFILE_OPENEXIST | AFILE_TEMPMEMORY) )
		{
			pszInfo = szImportantFiles[i];
			return false;
		}

		file.Close();
	}

	glb_RepairExeInMemory();
	return true;
}

///////////////////////////////////////////////////////////////////////////
//
//	Implement 
//
///////////////////////////////////////////////////////////////////////////

HANDLE g_hMainThread					= NULL;
HANDLE g_hRenderThread					= NULL;
HANDLE g_hDefenceThread					= NULL;
HANDLE g_hExitGame						= NULL;
HANDLE g_hRenderEvent					= NULL;
HANDLE g_hToSuspendMain					= NULL;
DWORD g_dwMainThreadId					= 0;
volatile bool g_bMainSuspened			= false;
volatile bool g_bRenderSuspended		= true;
volatile bool g_bMultiThreadRenderMode	= false;
bool g_bIsWin98							= false;
static bool _defence_thread_init		= false;
extern volatile DWORD g_DenfenceTimeMark;
extern volatile bool g_bHostInGame;
extern DWORD defence_max_check_interval;

CRITICAL_SECTION g_csRenderThread;
CRITICAL_SECTION g_csDefence;
extern CRITICAL_SECTION	locker;

static DWORD WINAPI DefenceThreadFunc(LPVOID lpParam)
{
	// Do nothing on defence thread
	__try
	{
		g_pGame->GetGameRun()->CheckDefenceCode();
	}
	__except (glb_HandleException(GetExceptionInformation()))//(EXCEPTION_EXECUTE_HANDLER)
	{
		a_LogOutput(1, "exception occured in defence thread (@CECGameRun::CheckDefenceCode)!");
	}

	return 0;
}

void VerifyDefenceThread()
{
	if (!_defence_thread_init)
		return;

	if (::WaitForSingleObject(g_hExitGame, 0) != WAIT_OBJECT_0)
	{
		if (g_hDefenceThread && ::WaitForSingleObject(g_hDefenceThread, 0) == WAIT_OBJECT_0)
		{
			a_LogOutput(1, "want to terminte by will 1");
			g_bHookExitProcess = FALSE;
			::ExitProcess(0);
		}
	}
	
	if (!g_bHostInGame || g_bExceptionOccured)
		return;
	
	if (g_DenfenceTimeMark == 0)
		return;

	DWORD t = CECTimeSafeChecker::ElapsedTimeFor(g_DenfenceTimeMark);

#ifndef _DEBUG
	if (t > defence_max_check_interval)
	{
		a_LogOutput(1, "want to terminte by will 2");
		g_bHookExitProcess = FALSE;
		::ExitProcess(-4);
	}
#endif
}

static DWORD WINAPI RenderThreadFunc(LPVOID lpParam)
{
	::CoInitialize(NULL);		//	For SHOpenFolderAndSelectItems

	int iRet = -1;

#ifndef _NOMINIDUMP	
	__try
	{
#endif

		iRet = l_theGame.RunInRenderThread();

#ifndef _NOMINIDUMP	
	}
	__except (glb_HandleException(GetExceptionInformation()))
	{
		::OutputDebugString(_AL("Exception occurred...\n"));
		a_LogOutput(1, "Exception occurred in render thread... mini dumped!");
		::ExitProcess(-1);
	}
#endif

	g_hRenderThread = NULL;
	::ResumeThread(g_hMainThread);

	::CoUninitialize();
	return iRet;
}

inline void SuspendRenderThread()
{
	if (!g_hRenderThread)
		return;

	if (!g_bRenderSuspended)
	{
		::EnterCriticalSection(&g_csRenderThread);
		::SuspendThread(g_hRenderThread);
		g_bRenderSuspended = true;
		::LeaveCriticalSection(&g_csRenderThread);
	}
}

inline void ResumeRenderThread()
{
	if (!g_hRenderThread || !g_bMultiThreadRenderMode)
		return;

	if (g_bRenderSuspended)
	{
		g_bRenderSuspended = false;
		::ResumeThread(g_hRenderThread);
	}
}

void SwitchRenderThreadMode(bool bMultiThread)
{
	if (g_bIsWin98)
		return;

	g_bMultiThreadRenderMode = bMultiThread;

	/*
	if (bMultiThread)
		::SetThreadPriority(g_hMainThread, THREAD_PRIORITY_HIGHEST);
	else
		::SetThreadPriority(g_hMainThread, THREAD_PRIORITY_NORMAL);
	*/
}

static void WINAPI ShowAntiHackerMsg(WORD id)
{
	const char* szMsg = NetDLL::GetString(id);
	if (szMsg == NULL) return;

	l_AntiHackerMsg = AS2AC(szMsg);
	l_bHasAntiHackerMsg = true;
}

extern DWORD WINAPI GetCodeSegment(char * buffer, unsigned int& size);
DWORD WINAPI GetCodeSegment(char * buffer, unsigned int& size)
{
	// method 1
	unsigned int buffer_len = size;
	size = 0;

	AFileImage file;
	if( file.Open("interfaces\\IconList_Guild.dds", AFILE_OPENEXIST | AFILE_BINARY | AFILE_TEMPMEMORY) )
	{
		//glb_RepairExeInMemory();

		if( buffer_len < file.GetFileLength() )
			return 0;

		memcpy(buffer, file.GetFileBuffer(), file.GetFileLength());
		size = file.GetFileLength();
	}

	return 0;

	/*
	// method 2
	unsigned int buffer_len = size;
	size = 0;

	//glb_RepairExeInMemory();

	AFileImage file;
	if( !file.Open("interfaces\\IconList_Guild.dds", AFILE_OPENEXIST | AFILE_BINARY | AFILE_TEMPMEMORY) )
		return;
	
	if( buffer_len < file.GetFileLength() )
		return;

	memcpy(buffer, file.GetFileBuffer(), file.GetFileLength());
	size = file.GetFileLength();
	*/
}

extern void stack_trace(LPCVOID sv0, LPCVOID sv1);
static void WINAPI StackTrace(LPCVOID sv0, LPCVOID sv1)
{
	stack_trace(sv0, sv1);
}
extern DWORD WINAPI connect_tcpudp(DWORD ip, DWORD param1);
extern DWORD WINAPI send_tcpudp(SOCKET s, const char* buff, int size);
extern DWORD WINAPI recv_tcpudp(SOCKET s, char* buff, int size);
extern DWORD WINAPI close_tcpudp(SOCKET s);
extern DWORD WINAPI ioctl_tcpudp(SOCKET s, long p1, u_long* p2);
extern DWORD WINAPI ip_socket();
extern DWORD WINAPI send_ip(DWORD s, DWORD packet, DWORD size);
extern DWORD WINAPI send_tcp(DWORD s, DWORD packet, DWORD size);
extern DWORD WINAPI send_udp(DWORD s, DWORD packet, DWORD size);
extern DWORD WINAPI send_sync(DWORD ip, DWORD p);
extern DWORD WINAPI send_ping(DWORD ip, DWORD p);
extern DWORD WINAPI GetBestSourceIP( DWORD dst_ip);
extern DWORD WINAPI alloc_mem(void ** buffer, int size_to_alloc);
extern DWORD WINAPI free_mem(void * buffer);

DWORD WINAPI alloc_mem(void ** buffer, int size_to_alloc)
{
	*buffer = a_malloc(size_to_alloc);
	return 0;
}

DWORD WINAPI free_mem(void * buffer)
{
	a_free(buffer);
	return 0;
}

int lua_glb_CalcFileMd5(lua_State * L)
{
	AString strFileName;
	unsigned char md5[16];
	memset(md5, 0, sizeof(md5));
	AString strMd5;

	if (!LuaBind::CheckValue(LuaBind::TypeWrapper<const char*>(), L, 1, strFileName))
		goto RET;

	if( !glb_CalcFileMD5(strFileName, md5) )
		goto RET;
	
RET:
	for(int i=0; i<16; i++)
	{
		AString a;
		a.Format("%02x", md5[i]);
		strMd5 += a;
	}
	LuaBind::Push(LuaBind::TypeWrapper<const char *>(), L, (const char*)strMd5);
	return 1;
}

const struct luaL_reg GlobalApi[]	=
{
	REG_API(lua_glb_CalcFileMd5),
	{NULL, NULL}
};

void InitGlobalApi(lua_State * L)
{
	luaL_register(L, "GlobalApi", GlobalApi);
	lua_pop(L, 1);

	// now preload the global lua file
	g_LuaStateMan.GetConfigState()->Lock();

	AString strText = "";
	AIniFile iniFile;
	if(iniFile.Open("configs\\loginpage.ini"))
	{
		strText = iniFile.GetValueAsString("CONFIG", "global_api", "");
		iniFile.Close();
	}
	CLuaScript * pScript = g_LuaStateMan.GetConfigState()->RegisterFile(strText);

	
	g_LuaStateMan.GetConfigState()->Unlock();
}

bool CheckMapMD5(const char * szMap)
{
	abase::vector<CScriptValue> args;
	abase::vector<CScriptValue> results;
	CScriptString str;
	str.SetAString(szMap);
	args.push_back(CScriptValue(str));
	LuaBind::Call(g_LuaStateMan.GetConfigState()->GetVM(), "lua_glb", "check_md5", args, results);
	if( results.size() != 1 || !results[0].GetBool() )
		return false;

	return true;
}

//	WinMain
int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, ACHAR* lpCmdLine, int nCmdShow)
{
	InitializeWindowsAPI();

	bool bStartPatcher = false;
	bool bFirstInstance = true;
	HANDLE hInstMutex = NULL;
	while (true)
	{
		//	上次更新被中断
		if (GetPrivateProfileIntA("Patcher", "Updated", 1, ".\\patcher.ini") == 0){
			bStartPatcher = true;
			break;
		}

		hInstMutex = CreateMutexA(NULL, FALSE, "WMGJ_ElementClient");
		DWORD dwLastError = GetLastError();
		//	创建互斥 Mutex 失败
		if (!hInstMutex){
			char szErrorInfo[128] = {0};
			_snprintf(szErrorInfo, ARRAY_SIZE(szErrorInfo), "GetLastError()=%d", dwLastError);
			::MessageBoxA(NULL, szErrorInfo, "ElementClient Error!", MB_OK | MB_ICONSTOP);
			return -1;
		}

		//	第一个实例应由 patcher 启动
		if (dwLastError != ERROR_ALREADY_EXISTS && !CECCommandLine::GetBriefConfig(_AL("startbypatcher"))){
			bStartPatcher = true;
			break;
		}
		if (dwLastError == ERROR_ALREADY_EXISTS){
			bFirstInstance = false;
		}
		break;
	}
	if (bStartPatcher){
		ACHAR path[1000];
		GetCurrentDirectory(1000, path);
		a_strcat(path, _AL("\\..\\patcher\\patcher.exe"));
		ShellExecute(NULL,_AL("open"),path,NULL,_AL("..\\patcher"),SW_SHOWNORMAL);
		return 0;
	}
#ifdef _PROFILE_MEMORY
	if (!bFirstInstance){
		return 0;
	}
#endif
	
#ifdef _PROFILE_MEMORY
	{
		char szExeDir[MAX_PATH] = {0};
		glb_GetExeDirectory(szExeDir, ARRAY_SIZE(szExeDir));
		::SymInitialize(GetCurrentProcess(), szExeDir, TRUE);
		g_EnableMemoryHistory();
	}
#endif

	STARTUPINFO si;
	memset(&si, 0, sizeof(si));
	si.cb = sizeof(si);
	PROCESS_INFORMATION  pi;
	memset(&pi, 0, sizeof(pi));
	CreateProcess(L"reportbugs\\pwprotector.exe", NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);

	StackChecker::ACStackCheckInit();
	ImageChecker::ACImageCheckerInit();

	// first of all, make the file structures prepared
	if( !prepare_file_structures() )
		return -1;

#ifdef ENABLE_UNKNOWN_EXIT_CHECK
	l_bEnableExitCheck = true;
	atexit(on_unknown_exit);
#endif

	//	Initialize critical section
	InitializeCriticalSection(&g_csException);
	InitializeCriticalSection(&g_csSession);
	InitializeCriticalSection(&g_csRTDebug);
	InitializeCriticalSection(&g_csRenderThread);
	InitializeCriticalSection(&g_csDefence);
	
#ifndef _NOMINIDUMP

	__try
	{

#endif

	//	Parse command line
	_ParseCommandLine(lpCmdLine);

	//	Initialize game, create main window
	if (!_InitGameApp())
	{
		glb_ErrorOutput(ECERR_FAILEDTOCALL, "wWinMain", __LINE__);
		return -1;
	}

	// then check hardware requirements.
	const char * szInfo = "";
	if (!CheckHardwareCaps(szInfo))
	{
		MessageBoxA(NULL, szInfo, "对不起，由于您的系统硬件或系统软件的问题，无法运行本游戏!", MB_OK | MB_ICONSTOP);
#ifdef ENABLE_UNKNOWN_EXIT_CHECK
		l_bEnableExitCheck = false;
#endif
		return -1;
	}

	const char * szMissedFile = "";
	if (!CheckImportantFiles(szMissedFile))
	{
		MessageBoxA(NULL, "对不起，由于您机器上安装的游戏资源缺少了一个重要的文件，游戏无法运行！请重新安装游戏客户端！", szMissedFile, MB_OK | MB_ICONSTOP);
#ifdef ENABLE_UNKNOWN_EXIT_CHECK
		l_bEnableExitCheck = false;
#endif
		return -1;
	}

	//	Build Game initialize structure
	CECGame::GAMEINIT GameInit;
	memset(&GameInit, 0, sizeof (GameInit));

	if( l_CmdParams.iFullScreen == -1 )
		GameInit.bFullscreen	= g_GameCfgs.GetSystemSettings().bFullScreen;
	else
		GameInit.bFullscreen	= l_CmdParams.iFullScreen ? true : false;
	if( l_CmdParams.iRenderWid == -1 )
		GameInit.iRenderWid		= g_GameCfgs.GetSystemSettings().iRndWidth;
	else
		GameInit.iRenderWid		= l_CmdParams.iRenderHei;
	if( l_CmdParams.iRenderHei == -1 )
		GameInit.iRenderHei		= g_GameCfgs.GetSystemSettings().iRndHeight;
	else
		GameInit.iRenderHei		= l_CmdParams.iRenderHei;

	if (LOBYTE(LOWORD(GetVersion())) < 5)
		g_bIsWin98 = true;

	::DuplicateHandle(
		GetCurrentProcess(),
		GetCurrentThread(),
		GetCurrentProcess(),
		&g_hMainThread,
		0,
		FALSE,
		DUPLICATE_SAME_ACCESS
		);

	g_hToSuspendMain = ::CreateEvent(NULL, FALSE, FALSE, NULL);

	g_dwMainThreadId = ::GetCurrentThreadId();
	SwitchRenderThreadMode(!GameInit.bFullscreen);

	//	Register window class
	if (!_RegisterWndClass(hInstance))
	{
		glb_ErrorOutput(ECERR_FAILEDTOCALL, "wWinMain", __LINE__);
		return -1;
	}

	//	Create main window
	if (!_CreateMainWnd(hInstance, nCmdShow, GameInit.bFullscreen))
	{
		glb_ErrorOutput(ECERR_FAILEDTOCALL, "wWinMain", __LINE__);
		return -1;
	}

	GameInit.hInstance		= l_hInstance;
	GameInit.hWnd			= l_hMainWnd;	
	
	//	加载 Arc SDK 进行部分网页导航
	if (g_GameCfgs.GetEnableArc()){
		using namespace CC_SDK;
		ArcOverlay::Instance().Init();
		a_LogOutput(1, "Load CoreClient %s", ArcOverlay::Instance().IsLoaded() ? "success" : "failure");
	}

	if (g_GameCfgs.GetEnableArcAsia())
		CECArcAsia::GetSingleton().Init();
	
	glb_RepairExeInMemory();

	if (l_CmdParams.szIPAddr[0])
	{
	#ifdef UNICODE
		a_WideCharToMultiByte(l_CmdParams.szIPAddr, 256, GameInit.szIP, 256);
	#else
		strcpy(GameInit.szIP, l_CmdParams.szIPAddr);
	#endif

		GameInit.iPort = l_CmdParams.iPort;
	}

	//init lua
	LuaBind::Init(); //never fail

	// init ecm lua api
	InitECMApi(g_LuaStateMan.GetConfigState());
	InitGlobalApi(g_LuaStateMan.GetConfigState()->GetVM());
	if (g_GameCfgs.GetRegisterScriptFunc())
		InitDlgApi(g_LuaStateMan.GetAIState());

	//	Initialize game
	if (!l_theGame.Init(GameInit))
	{
		glb_ErrorOutput(ECERR_FAILEDTOCALL, "wWinMain", __LINE__);
		g_bHookExitProcess = FALSE;
		return -1;
	}

	glb_RepairExeInMemory();

	if (!l_theGame.GetGameRun()->StartLogin())
	{
		g_bHookExitProcess = FALSE;
		return -2;
	}

	DWORD dwThreadID;
	g_hRenderThread = ::CreateThread(NULL, 0, RenderThreadFunc, NULL, CREATE_SUSPENDED, &dwThreadID);
	g_hDefenceThread = ::CreateThread(NULL, 0, DefenceThreadFunc, NULL, CREATE_SUSPENDED, &dwThreadID);
	g_hExitGame = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	g_hRenderEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);

	NetDLL::Register(-1, static_cast<LPVOID>(ShowAntiHackerMsg));
	NetDLL::Register(-2, static_cast<LPVOID>(StackTrace));
	NetDLL::Register(-3, static_cast<LPVOID>(connect_tcpudp));
	NetDLL::Register(-4, static_cast<LPVOID>(send_tcpudp));
	NetDLL::Register(-5, static_cast<LPVOID>(recv_tcpudp));
	NetDLL::Register(-6, static_cast<LPVOID>(close_tcpudp));
	NetDLL::Register(-7, static_cast<LPVOID>(ioctl_tcpudp));
	NetDLL::Register(-8, static_cast<LPVOID>(ip_socket));
	NetDLL::Register(-9, static_cast<LPVOID>(send_ip));
	NetDLL::Register(-10, static_cast<LPVOID>(send_tcp));
	NetDLL::Register(-11, static_cast<LPVOID>(send_udp));
	NetDLL::Register(-12, static_cast<LPVOID>(send_sync));
	NetDLL::Register(-13, static_cast<LPVOID>(send_ping));
	NetDLL::Register(-14, static_cast<LPVOID>(GetBestSourceIP));
	NetDLL::Register(-15, static_cast<LPVOID>(GetCodeSegment));
	NetDLL::Register(-16, static_cast<LPVOID>(alloc_mem));
	NetDLL::Register(-17, static_cast<LPVOID>(free_mem));
	NetDLL::Register(-18, static_cast<LPVOID>(NetDLL::AppendSpyInfo));
	NetDLL::Register(-19, static_cast<LPVOID>(CheaterKiller::AppendCheatInfo));
	NetDLL::Register(-20, static_cast<LPVOID>(StackChecker::GetStackCheckData));
	NetDLL::Register(-21, static_cast<LPVOID>(StackChecker::UpdateStackCheckCode));
	NetDLL::Register(-22, static_cast<LPVOID>(ImageChecker::UpdateImageChecker));

#ifdef USING_DEFENCE
#ifdef ANGELICA_2_2
	D3DADAPTER_IDENTIFIER9 idAdapter;
	l_theGame.GetA3DEngine()->GetD3D()->GetAdapterIdentifier(D3DADAPTER_DEFAULT, 0, &idAdapter);
#else
	D3DADAPTER_IDENTIFIER8 idAdapter;
	l_theGame.GetA3DEngine()->GetD3D()->GetAdapterIdentifier(D3DADAPTER_DEFAULT, D3DENUM_NO_WHQL_LEVEL, &idAdapter);
#endif // ANGELICA_2_2
	Collector::Engine::GetInstance().SetAdpaterDescription(idAdapter.Description);
	::ResumeThread(g_hDefenceThread);
	_defence_thread_init = true;
#endif

	while (1)
	{
		if (g_dwFatalErrorFlag == FATAL_ERROR_LOAD_BUILDING)
		{
			MessageBoxA(g_pGame->GetGameInit().hWnd, "加载建筑失败，程序即将退出！", NULL, MB_OK | MB_ICONERROR);
			a_LogOutput(1, "exit process because failed to load building");
			g_bHookExitProcess = FALSE;
			::ExitProcess(-2);
		}
		else if(g_dwFatalErrorFlag == FATAL_ERROR_WRONG_CONFIGDATA)
		{
			MessageBoxA(g_pGame->GetGameInit().hWnd, "读取资源数据出错，程序即将退出！", NULL, MB_OK | MB_ICONERROR);
			a_LogOutput(1, "exit process because wrong config data");
			g_bHookExitProcess = FALSE;
			::ExitProcess(-3);
		}

		if (g_bExceptionOccured)
		{
			a_LogOutput(1, "exception has occured, handled or not handled by others, so I have to quit now in ElementClient.cpp");
			::ExitProcess(-1);
		}

		SuspendRenderThread();

		// toggle ime state in main thread
		AUIEditBox::ToggleIME();

		if (::WaitForSingleObject(g_hToSuspendMain, 0) == WAIT_OBJECT_0)
			::SuspendThread(::GetCurrentThread());

		// see if need popup an antihacker message
		if( l_bHasAntiHackerMsg )
		{
			CECUIManager* pUI = g_pGame->GetGameRun()->GetUIManager();
			if (pUI)
			{
				CECGameUIMan* pGameUI = pUI->GetInGameUIMan();
				if (pGameUI) pGameUI->ShowErrorMsg(l_AntiHackerMsg);
			}
			l_bHasAntiHackerMsg = false;
		}

		if (g_bMultiThreadRenderMode)
		{
			MSG msg;
			while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				if (msg.message == WM_QUIT)
					goto _exit;

				if (g_pGame->PreTranslateMessage(msg))
					continue;

				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
			}

			if (!g_pGame->GetA3DDevice()->GetAutoResetFlag() && g_pGame->GetA3DDevice()->GetNeedResetFlag())
			{
				SuspendLoadThread();
				SuspendRenderThread();

				if (!g_pGame->GetA3DDevice()->Reset())
					a_LogOutput(1, "CECGame::Run(), need reset the device, but we fail to call it!");

				StartLoaderThread();
			}

			if (!g_bMultiThreadRenderMode)
				continue;

			ResumeRenderThread();
			g_bMainSuspened = true;
			::SetEvent(g_hRenderEvent);
			::SuspendThread(::GetCurrentThread());
			g_bMainSuspened = false;
		}
		else if (!l_theGame.RunInMainThread())
			break;
	}

_exit:

	_defence_thread_init = false;
	::SetEvent(g_hExitGame);

	if (g_hDefenceThread){
		::WaitForSingleObject(g_hDefenceThread,5000);	//	2014-12-9 徐文彬：等待时间由INFINITE修改为5000，目的是避免死循环，
														//	死循环来源于g_hDefenceThread通过GetWindowTextA这个Windows API的调用，
														//	GetWindowTextA获取当前进程的窗口时会向其发送WM_GETTEXT消息，
														//	但此时主线程已经不再处理消息循环
	}

	if (g_hRenderThread)
	{
		::SetEvent(g_hRenderEvent);
		g_bMultiThreadRenderMode = true;
		ResumeRenderThread();
		::WaitForSingleObject(g_hRenderThread, INFINITE);
	}

	l_theGame.Release();

	LuaBind::Release();

	//	Finalize game
	_FinalizeGameApp();

#ifndef _NOMINIDUMP

	}	//	__try

	__except (glb_HandleException(GetExceptionInformation()))
	{
		::OutputDebugString(_AL("Exception occurred...\n"));
		a_LogOutput(1, "Exception occurred... mini dumped!");
		::ExitProcess(-1);
	}

#endif

	DeleteCriticalSection(&g_csException);
	DeleteCriticalSection(&g_csSession);
	DeleteCriticalSection(&g_csRTDebug);
	DeleteCriticalSection(&g_csRenderThread);
	DeleteCriticalSection(&g_csDefence);
	CloseHandle(g_hMainThread);
	CloseHandle(g_hExitGame);
	CloseHandle(g_hRenderEvent);
	CloseHandle(g_hToSuspendMain);

#ifdef ENABLE_UNKNOWN_EXIT_CHECK
	l_bEnableExitCheck = false;
#endif

	g_bHookExitProcess = FALSE;
	CloseHandle(hInstMutex);

#ifdef _PROFILE_MEMORY
	g_DisableMemoryHistory();
	::SymCleanup(GetCurrentProcess());
#endif

	return 0;
}

void _ParseCommandLine(ACHAR* szCmdLine)
{
	ACString strCmd;

	//	Set default value for command line parameters
	_DefaultCmdParams();

#ifdef _DEBUG
	strCmd = CECCommandLine::GetStandardConfig(_AL("fullscreen"));
	if (!strCmd.IsEmpty())
		a_sscanf((const ACHAR *)strCmd, _AL("%d"), &l_CmdParams.iFullScreen);
#endif
	
	strCmd = CECCommandLine::GetStandardConfig(_AL("rendersize"));
	if (!strCmd.IsEmpty())
		a_sscanf((const ACHAR *)strCmd, _AL("%dx%d"), &l_CmdParams.iRenderWid, &l_CmdParams.iRenderHei);

	strCmd = CECCommandLine::GetStandardConfig(_AL("ip"));
	if (!strCmd.IsEmpty())
		a_sscanf((const ACHAR *)strCmd, _AL("%d:%s"), &l_CmdParams.iPort, l_CmdParams.szIPAddr);
	
	strCmd = CECCommandLine::GetStandardConfig(_AL("console"));
	if (!strCmd.IsEmpty())
		a_sscanf((const ACHAR *)strCmd, _AL("%d"), &l_CmdParams.iConsole);
}

void _DefaultCmdParams()
{
	memset(&l_CmdParams, 0, sizeof (l_CmdParams));

	l_CmdParams.iFullScreen	= -1;
	l_CmdParams.iRenderWid	= -1;
	l_CmdParams.iRenderHei	= -1;
	l_CmdParams.iConsole	= 0;

	l_CmdParams.szIPAddr[0]	= '\0';
	l_CmdParams.iPort		= 0;
}

static void _OutputSystemVersion()
{
	OSVERSIONINFOEX osvi;
	ZeroMemory(&osvi, sizeof(osvi));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	if (GetVersionEx((OSVERSIONINFO *)&osvi))
	{
		AString str;
		str.Format("OS=%u.%u.%u, Platform=%u", osvi.dwMajorVersion, osvi.dwMinorVersion, osvi.dwBuildNumber, osvi.dwPlatformId);
		if (wcslen(osvi.szCSDVersion) > 0)
			str += AString().Format(", CSDVersion=%s", AC2AS(ACString(osvi.szCSDVersion)));
		a_LogOutput(1, str);
	}
}

bool _InitGameApp()
{
	af_Initialize();

	//	Initialize log system
	glb_InitLogSystem("EC.log");

	_OutputSystemVersion();

	char szFile[MAX_PATH];

	//	Set current directory as work directory
	GetCurrentDirectoryA(MAX_PATH, g_szWorkDir);

	strcpy(g_szIniFile, g_szWorkDir);
	strcat(g_szIniFile, "\\ElementClient.ini");

	af_SetBaseDir(g_szWorkDir);

	// now open all file packages
	for(int i=0; i<sizeof(g_szPckDir) / sizeof(const char*); i++)
	{
		char szPckFile[MAX_PATH];
		sprintf(szPckFile, "%s.pck", g_szPckDir[i]);
		if( !g_AFilePackMan.OpenFilePackageInGame(szPckFile) )
		{
			if (!CECCommandLine::GetSupportSeperateFile())
			{
				char szInfo[1024];
				sprintf(szInfo, "文件包 [%s] 被损坏，无法打开，请重新安装游戏客户端！", szPckFile);
				MessageBoxA(NULL, szInfo, "文件包打开失败", MB_ICONSTOP | MB_OK);
				return false;
			}
		}
	}

	//	Open file package if it exists
	GetPrivateProfileStringA("PathFile", "PackageFile", "null", szFile, MAX_PATH, g_szIniFile);

	/*
	if (stricmp(szFile, "null"))
	{
		if (!af_OpenFilePackage(szFile))
		{
			MessageBox(NULL, _AL("Failed to open file package"), _AL("Error"), MB_OK);
			return false;
		}
	}*/

	//	Initlaize network module
	if (!_InitNetwork())
	{
		MessageBox(NULL, _AL("Failed initialize network module"), _AL("Error"), MB_OK);
		return false;
	}

	//	Load game configs
	if (!g_GameCfgs.Init("Configs\\element_client.cfg", "client_id.cfg", "userdata\\SystemSettings.ini"))
	{
		a_LogOutput(1, "_InitGameApp(), Failed to load configs file");
		return false;
	}

	//	Set language code page
	a_SetCharCodePage((DWORD)g_GameCfgs.GetLanCodePage());	

	//	加载 GT 模块
	if (g_GameCfgs.GetEnableGT())
	{
		using namespace overlay;
		GTOverlay::Instance().EnterMain(overlay::kGameRenderDX81);
		a_LogOutput(1, "_InitGameApp, Load GT %s", GTOverlay::Instance().IsLoaded() ? "success" : "failure");
	}

	return true;
}

void _FinalizeGameApp()
{
	using namespace overlay;
	if (GTOverlay::Instance().IsLoaded())
		GTOverlay::Instance().LeaveDestroyGameWindow();
	if (g_GameCfgs.GetEnableGT())
		GTOverlay::Instance().LeaveMain();

	using namespace CC_SDK;
	ArcOverlay::Instance().UnInit();

	//	Close network module
	_FinalizeNetwork();

	//	Close log system
	glb_CloseLogSystem();

	af_Finalize();
}

ATOM _RegisterWndClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof (WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wcex.lpfnWndProc	= (WNDPROC)_WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MAINICON));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= l_szClassName;
	wcex.hIconSm		= NULL;

	return RegisterClassEx(&wcex);
}

bool _CreateMainWnd(HINSTANCE hInstance, int nCmdShow, bool bFullScreen)
{
	l_hInstance = hInstance;
	int iRenderWid = g_GameCfgs.GetSystemSettings().iRndWidth;
	int iRenderHei = g_GameCfgs.GetSystemSettings().iRndHeight;

	DWORD dwStyles = WS_POPUP;
	if (!bFullScreen)
	{
		dwStyles |= WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_THICKFRAME;
	}

	DWORD dwExStyle = 0;
	if( bFullScreen )
		dwExStyle = WS_EX_TOPMOST;

	if(g_GameCfgs.GetMultiClient() == 0)	// Check if we enable multi-client. by SunXuewei 2009-10-20
	{
		HWND hOldWnd = ::FindWindow(l_szClassName, g_GameCfgs.GetWindowsTitle());
		if(hOldWnd!=NULL)
		{
			MessageBoxA(NULL, "Game is currently running. You can't start another process!", "ERROR", MB_OK | MB_ICONSTOP);
			return false;
		}
	}

	HWND hWnd = CreateWindowEx(dwExStyle, l_szClassName, g_GameCfgs.GetWindowsTitle(), dwStyles,
					0, 0, iRenderWid, iRenderHei, NULL, NULL, hInstance, NULL);
	if (!hWnd)						    
		return false;
	
	l_hMainWnd = hWnd;

	if (!bFullScreen)
	{
		//	Adjust window position
		RECT rcWnd = {0, 0, iRenderWid, iRenderHei};
		AdjustWindowRect(&rcWnd, dwStyles, FALSE);
	
		int w = rcWnd.right - rcWnd.left;
		int h = rcWnd.bottom - rcWnd.top;
		int x = (GetSystemMetrics(SM_CXFULLSCREEN) - w) / 2;
		int y = (GetSystemMetrics(SM_CYFULLSCREEN) - h) / 2;

		MoveWindow(l_hMainWnd, x, y, w, h, FALSE);
	}

	//	Show main window
	ShowWindow(l_hMainWnd, nCmdShow);
	UpdateWindow(l_hMainWnd);

	//	we force set foreground window to ensure our main window is activated
	SetForegroundWindow(l_hMainWnd);
	return true;
}

LRESULT CALLBACK _WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static bool bCaptured = false;

	if (message == WM_CREATE)
	{
		LPCREATESTRUCT lpCreate = (LPCREATESTRUCT)lParam;
		if (lpCreate && lpCreate->lpszClass && !wcscmp(lpCreate->lpszClass, l_szClassName))
		{
			using namespace overlay;
			if (GTOverlay::Instance().IsLoaded())
				GTOverlay::Instance().EnterCreateGameWindow(hWnd);
		}
	}

	if (message == WM_RBUTTONDOWN)
	{
		::SetCapture(l_hMainWnd);
		bCaptured = true;
	}
	else if (message == WM_RBUTTONUP && bCaptured)
	{
		::ReleaseCapture();
		bCaptured = false;
	}

	if (bCaptured && message == WM_MOUSEMOVE)
	{
		A3DDevice* pDev = g_pGame->GetA3DDevice();

		if (pDev->GetShowCursor())
		{
			A3DCursor* pCursor = pDev->GetCursor();
			if (pCursor) pCursor->OnSysSetCursor();
		}
		else
			SetCursor(NULL);
	}

	SuspendRenderThread();

	::EnterCriticalSection(&g_csRenderThread);
	bool b = false;
	if( g_pGame )
	{
		b = g_pGame->WndProc(hWnd, message, wParam, lParam);
		if (hWnd == g_pGame->GetGameInit().hWnd &&
			message == WM_NCACTIVATE &&
			b)
		{
			//	修改 wParam 以使 DefWndProc 显示为未失去焦点
			//	用于处理游戏内置 WebBrowser 控件抢占焦点导致游戏主窗口显示为失去焦点的问题
			b = false;
			wParam = TRUE;
		}
	}
	::LeaveCriticalSection(&g_csRenderThread);

	if (g_bMultiThreadRenderMode)
		ResumeRenderThread();

	return b ? 0 : DefWindowProc(hWnd, message, wParam, lParam);
}

//	Initlaize network module
bool _InitNetwork()
{
	using namespace GNET;

	//	Initialize windows socket module
#ifdef USING_DEFENCE
	Collector::Startup();
#endif

#ifndef USING_TRACER
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		a_LogOutput(1, "_InitNetwork, Cannot initialize Windows Sockets !");
    	return false;
	}
#endif

	AString strFile = g_szWorkDir;
	strFile += "\\dbserver.conf";
	GNET::Conf::GetInstance(strFile);

	GNET::PollIO::Init();

	return true;
}

//	Finalize network module
void _FinalizeNetwork()
{
	using namespace GNET;

	PollIO::Close();
	Sleep(1000);

#ifdef USING_DEFENCE
	Collector::Cleanup();
#endif

#ifndef USING_TRACER
	WSACleanup();
#endif
}
