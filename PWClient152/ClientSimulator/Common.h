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
#include <vector.h>
#include <hashmap.h>
#include <AString.h>
#include <AAssist.h>
#include <ALog.h>
#include <AMemory.h>
#include <A3DVector.h>
#include "cslock.h"

///////////////////////////////////////////////////////////////////////////
//  
//  Define and Macro
//  
///////////////////////////////////////////////////////////////////////////

const DWORD GAME_VERSION = ((0 << 24) | (1 << 16) | (5 << 8) | 1);
const DWORD GAME_BUILD = 2305;


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
	ECERR_NOCONFIGFILE,
	ECERR_SERVERERROR,
	ECERR_NOELEMENTDATA,
	ECERR_INITSOCKETERR,
};

// Log text color
enum
{
	LOGCOLOR_WHITE,
	LOGCOLOR_RED,
	LOGCOLOR_GREEN,
	LOGCOLOR_BLUE,
	LOGCOLOR_PURPLE,
	LOGCOLOR_YELLOW,
};

///////////////////////////////////////////////////////////////////////////
//  
//  Declare of Global functions
//  
///////////////////////////////////////////////////////////////////////////

bool glb_InitLogSystem(const char* szFile);
void glb_CloseLogSystem();
unsigned long glb_HandleException( LPEXCEPTION_POINTERS pExceptionPointers );
void glb_ErrorMessage(int iErrCode);

// �����Ϣ������̨
void AddConsoleText(const char* szMessage, int color);

// �������̨��Ϣ
void ClearConsoleText();

// ����ű�������Ϣ
void ScriptErrHandler( const char* szMsg );

// ȡӦ�ó��������
void ParseCommandLine( const AString& line,StringVector& params );

// ö�ٵ�ǰĿ¼ָ�������ļ�
void EnumFolder( const char* pattern,StringVector& file_list );

// ��ȡ��Ʒ�Ķѵ���Ϣ
int GetItemPileLimit( int tid );

// ��ȡָ��ְҵ�ļ����б�
void EnumSkillList(int iProfession, abase::vector<int>& skills);


///////////////////////////////////////////////////////////////////////////
//  
//  Inline functions
//  
///////////////////////////////////////////////////////////////////////////

