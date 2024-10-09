/*
 * FILE: Common.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Shizhenhua, 2012/8/28
 *
 * HISTORY: 
 *
 * Copyright (c) 2011 ACE Studio, All Rights Reserved.
 */


#pragma once

#include <ABaseDef.h>
#include <AMemory.h>
#include <vector.h>
#include <hashmap.h>
#include <AString.h>
#include <ALog.h>
#include <A3DVector.h>
#include "cslock.h"

///////////////////////////////////////////////////////////////////////////
//  
//  Define and Macro
//  
///////////////////////////////////////////////////////////////////////////


extern DWORD GAME_VERSION;
extern AString GAME_VERSION_STRING;


///////////////////////////////////////////////////////////////////////////
//  
//  Types and Global variables
//  
///////////////////////////////////////////////////////////////////////////

typedef abase::vector<AString> StringVector;


extern ALog g_Log;
extern char g_szWorkDir[];
extern char g_szIniFile[];
extern A3DVECTOR3 g_vOrigin;
extern A3DVECTOR3 g_vAxisX;
extern A3DVECTOR3 g_vAxisY;
extern A3DVECTOR3 g_vAxisZ;

extern CsMutex*	g_csLog;
extern CsMutex* g_csException;

//	Error code
enum
{
	ECERR_UNKNOWN = 0,			//	Unknown
	ECERR_INVALIDPARAMETER,		//	Invalid parameter
	ECERR_NOTENOUGHMEMORY,		//	Not enough memory
	ECERR_FILEOPERATION,		//	File operation
	ECERR_FAILEDTOCALL,			//	Failed to call function
	ECERR_WRONGVERSION,			//	Wrong version
};


///////////////////////////////////////////////////////////////////////////
//  
//  Declare of Global functions
//  
///////////////////////////////////////////////////////////////////////////

bool glb_InitLogSystem(const char* szFile);
void glb_CloseLogSystem();
unsigned long glb_HandleException( LPEXCEPTION_POINTERS pExceptionPointers );

// 取应用程序参数表
void ParseCommandLine( const AString& line,StringVector& params );

// 分割字符串
void SplitString(const AString& str, const char* sep, StringVector& strList);

// 大版本字符串
AString BigVersionStr(DWORD dwVersion);

// 读取客户端的版本信息
bool GetClientVersion();


///////////////////////////////////////////////////////////////////////////
//  
//  Inline functions
//  
///////////////////////////////////////////////////////////////////////////

