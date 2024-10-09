/*
 * FILE: Common.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Shizhenhua, 2012/8/28
 *
 * HISTORY: 
 *
 * Copyright (c) 2011 ACE Studio, All Rights Reserved.
 */


#define _WIN32_WINDOWS 0x0410

#include "Common.h"
#include "AMiniDump.h"
#include "SessionManager.h"
#include "EC_RoleTypes.h"
#include "gnconf.h"

#include <AF.h>
#include <string>
#include <algorithm>
#include <set>
#include <vector>


///////////////////////////////////////////////////////////////////////////
//  
//  Define and Macro
//  
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//  
//  Types and Global variables
//  
///////////////////////////////////////////////////////////////////////////

DWORD GAME_VERSION = 0;				// 客户端大版本
AString GAME_VERSION_STRING;		// 客户端小版本字符串

ALog	g_Log;
char	g_szWorkDir[MAX_PATH];
char	g_szIniFile[MAX_PATH];

A3DVECTOR3	g_vOrigin(0.0f);
A3DVECTOR3	g_vAxisX(1.0f, 0.0f, 0.0f);
A3DVECTOR3	g_vAxisY(0.0f, 1.0f, 0.0f);
A3DVECTOR3	g_vAxisZ(0.0f, 0.0f, 1.0f);

CsMutex*	g_csLog;
CsMutex*  	g_csException;

//	Error messages
static char* l_aErrorMsgs[] = 
{
	"Unknown error.",			//	ECERR_UNKNOWN
	"Invalid parameter.",		//	ECERR_INVALIDPARAMETER	
	"Not enough memory.",		//	ECERR_NOTENOUGHMEMORY
	"File operation error.",	//	ECERR_FILEOPERATION	
	"Failed to call function.",	//	ECERR_FAILEDTOCALL	
	"Wrong version.",			//	ECERR_WRONGVERSION
};


///////////////////////////////////////////////////////////////////////////
//  
//  Implement Global functions
//  
///////////////////////////////////////////////////////////////////////////

static void _LogOutput(const char* szMsg)
{
	if( CsLockScoped lock(g_csLog) )
	{
		g_Log.Log(szMsg);
		printf("%s\n", szMsg);
	}
}

//	Initialize log system
bool glb_InitLogSystem(const char* szFile)
{
	if (!g_Log.Init(szFile, "Element client login test log file"))
		return false;

	a_RedirectDefLogOutput(_LogOutput);
	g_csLog = new CsMutex;
	return true;
}

//	Close log system
void glb_CloseLogSystem()
{
	g_Log.Release();
	a_RedirectDefLogOutput(NULL);
	delete g_csLog;
	g_csLog = NULL;
}

//	Output predefined error
void glb_ErrorOutput(int iErrCode, const char* szFunc, int iLine)
{
	char szMsg[1024];
	sprintf(szMsg, "%s: %s (line: %d)", szFunc, l_aErrorMsgs[iErrCode], iLine);
	a_LogOutput(1, szMsg);
}

unsigned long glb_HandleException( LPEXCEPTION_POINTERS pExceptionPointers )
{
	if( IsDebuggerPresent() )
		return EXCEPTION_CONTINUE_SEARCH;

	char szFile[MAX_PATH];
	sprintf(szFile, "%s\\Logs\\client_logintest.dmp", g_szWorkDir);

	if( GetVersion() < 0x80000000 )
	{
		// WinNT or Win2000
		AMiniDump::Create(NULL, pExceptionPointers, szFile, g_csException->GetMtx());
	}
	else
	{
		// Win95 or Win98 or Win32		
	}

	STARTUPINFO si;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_SHOW;
	PROCESS_INFORMATION pi;
	ZeroMemory(&pi, sizeof(pi));
	CreateProcess("ReportBugs\\CReportBugs.exe", NULL, NULL, NULL, FALSE, 0, NULL, "ReportBugs", &si, &pi);

	return EXCEPTION_EXECUTE_HANDLER;
}

// 取应用程序参数表
void ParseCommandLine( const AString& line,StringVector& params )
{
	int i = 0;
	int len = line.GetLength();
	params.clear();

#define IS_SEPCHAR(ch) ((ch) == ' ' || (ch) == '\t')

	while( i < len )
	{
		while( i < len && IS_SEPCHAR(line[i]) ) i++;
		if( i >= len ) return;

		if( line[i] == '"' )
		{
			i++;
			int j = i;
			while( i < len && line[i] != '"' ) i++;
			params.push_back( line.Mid(j, i-j) );
			i++;
		}
		else
		{
			int j = i;
			while( i < len && !IS_SEPCHAR(line[i]) ) i++;
			params.push_back( line.Mid(j, i-j) );
		}
	}
}

// 分割字符串
void SplitString(const AString& str, const char* sep, StringVector& strList)
{
	strList.clear();

	AString temp = str;
	size_t pos;

	while(true)
	{
		pos = temp.Find(sep);
		if(pos == -1)
		{
			strList.push_back(temp);
			break;
		}
		strList.push_back(temp.Left(pos));
		temp.CutLeft(pos + strlen(sep));
		while(true)
		{
			pos = temp.Find(sep);
			if(pos != 0) break;
			temp.CutLeft(pos + strlen(sep));
		}
	}
}

AString BigVersionStr(DWORD dwVersion)
{
	int i;
	AString str;
	int iVer[4];
	iVer[0] = (dwVersion & 0xff000000) >> 24;
	iVer[1] = (dwVersion & 0x00ff0000) >> 16;
	iVer[2] = (dwVersion & 0x0000ff00) >> 8;
	iVer[3] = dwVersion & 0x000000ff;

	for( i=0;i<4;i++ )
	{
		if( iVer[i] ) break;
	}

	for( int n=i;n<4;n++ )
	{
		AString temp;
		temp.Format("%d.", iVer[n]);
		str += temp;
	}

	str.CutRight(1);
	return str;
}

// 读取客户端的版本信息
bool GetClientVersion()
{
	FILE* pFile;

	// 大版本
	std::string strVersion = GNET::Conf::GetInstance()->find("GameClient", "client_version");
	if( !strVersion.empty() )
	{
		StringVector strList;
		int iVer[4] = {0};
		SplitString(strVersion.c_str(), ".", strList);

		int n = 4;
		while(strList.size())
		{
			iVer[--n] = atoi(strList.back());
			strList.pop_back();
		}

		GAME_VERSION = (iVer[0] << 24) | (iVer[1] << 16) | (iVer[2] << 8) | iVer[3];
	}
	else
	{
		return false;
	}

	// 小版本信息
	GAME_VERSION_STRING = "未知";

	// 1. elements.data
	int elementdata_ver;
	if( !(pFile = fopen("data\\elements.data", "rb")) )
		return false;
	fread(&elementdata_ver, sizeof(int), 1, pFile);
	fclose(pFile);

	// 2. tasks.data
	unsigned long task_ver;
	if( !(pFile = fopen("data\\tasks.data", "rb")) )
		return false;
	fread(&task_ver, sizeof(unsigned long), 1, pFile);
	fread(&task_ver, sizeof(unsigned long), 1, pFile);
	fclose(pFile);

	// 3. gshop.data
	DWORD gshop_timestamp;
	if( !(pFile = fopen("data\\gshop.data", "rb")) )
		return false;
	fread(&gshop_timestamp, sizeof(DWORD), 1, pFile);
	fclose(pFile);

	// 4. gshop1.data
	DWORD gshop_timestamp2;
	if( !(pFile = fopen("data\\gshop1.data", "rb")) )
		return false;
	fread(&gshop_timestamp2, sizeof(DWORD), 1, pFile);
	fclose(pFile);

	GAME_VERSION_STRING.Format("%x%x%x%x", elementdata_ver, task_ver, gshop_timestamp, gshop_timestamp2);
	return true;
}