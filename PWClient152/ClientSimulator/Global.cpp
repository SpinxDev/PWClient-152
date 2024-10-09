/*
 * FILE: Global.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Shizhenhua, 2012/8/27
 *
 * HISTORY: 
 *
 * Copyright (c) 2011 ACE Studio, All Rights Reserved.
 */


#include <stdio.h>
#include <windows.h>
#include "CmdManager.h"
#include "SessionManager.h"
#include <AFI.h>
#include "Common.h"
#include "Global.h"

#include "luaFunc.h"
#include <string>
#include "gnconf.h"
#include "gnet.h"

#include "zgm_luadebug.h"


///////////////////////////////////////////////////////////////////////////
//  
//  Define and Macro
//  
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//  
//  Global variables and Functions
//  
///////////////////////////////////////////////////////////////////////////

extern char g_szWorkDir[];
extern char g_szIniFile[];

// 当前程序是否运行
volatile bool g_bGameRun = true;

// 逻辑线程句柄
HANDLE g_hThreadLogic = NULL;

/** 跑客户端逻辑的线程
*/
DWORD WINAPI LogicTickThread( LPVOID lpParameter )
{
#ifndef _NOMINIDUMP
	__try
	{
#endif

	SessionManager& clientMgr = SessionManager::GetSingleton();
	if( !clientMgr.Init(g_szIniFile) )
		return 0;

	while( g_bGameRun )
	{
		clientMgr.Tick();

		Sleep(50);
	}

	clientMgr.Release();

#ifndef _NOMINIDUMP
	}
	__except(glb_HandleException(GetExceptionInformation()))
	{
		::OutputDebugString("Exception occurred...\n");
		a_LogOutput(1, "Exception occurred in Logic tick thread... mini dumped!");
		::ExitProcess(-1);
	}
#endif

	return 0;
}


///////////////////////////////////////////////////////////////////////////

// 初始化网络环境
bool InitNetwork()
{
	DWORD dwThreadID;
	g_csException = new CsMutex; 
	af_Initialize();
	GetCurrentDirectory(MAX_PATH, g_szWorkDir);
	sprintf(g_szIniFile, "%s\\%s", g_szWorkDir, "sclient.conf");
	af_SetBaseDir(g_szWorkDir);

	// 初始化LUA
#ifdef _DEBUG
	if( !g_LuaStateMan.Init(true) )
#else
	if( !g_LuaStateMan.Init() )
#endif
	{
		AddConsoleText("Initialize the Lua module failed!\n", LOGCOLOR_RED);
		return false;
	}
	
	LuaBind::RegisterLuaErrHandler(ScriptErrHandler);
	LuaFuncFactory::GetSingleton().RegisterAllAPI();
	CmdManager::GetSingleton();

	// 开启逻辑线程
	g_hThreadLogic = CreateThread(NULL, 0, LogicTickThread, NULL, 0, &dwThreadID);
	return true;
}

// 结束网络环境
void Finalize()
{
	g_bGameRun = false;
	WaitForSingleObject(g_hThreadLogic, INFINITE);
	CloseHandle(g_hThreadLogic);
	af_Finalize();
	delete g_csException;
	// 释放LUA
	g_LuaStateMan.Release();
}