/*
 * FILE: ALog.h
 *
 * DESCRIPTION: Class used to manipulate the log file
 *
 * CREATED BY: Hedi, 2001/12/3
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#include "ALog.h"
#include "ATime.h"
#include <io.h>

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

//	Default debug output function
static LPFNDEFLOGOUTPUT	l_lpfnDefLogOutput	= NULL;
static HWND				l_hDebugWnd			= NULL;

char ALog::m_szLogDir[MAX_PATH] = "Logs";

///////////////////////////////////////////////////////////////////////////
//
//	Local functions
//
///////////////////////////////////////////////////////////////////////////

static void _DefLogOutput(const char* szMsg)
{
	if (l_lpfnDefLogOutput)
	{
		//	Use user defined log output function
		l_lpfnDefLogOutput(szMsg);
		return;
	}

	//	Use system log output function

#ifdef _WIN32
	OutputDebugStringA(szMsg);
#else
	printf(szMsg);
#endif	//	_WIN32
}

static void _SafeCreateDir(const char* szDir)
{
	if (_access(szDir, 0) == -1)
	{
		CreateDirectoryA(szDir, NULL);
	}
}

///////////////////////////////////////////////////////////////////////////
//
//	Implement
//
///////////////////////////////////////////////////////////////////////////

/*	Redirect default log ouput function. This function can be used to
	redirect the destination all ACommon's internal logs.
	
	Return previous function set by user.

	lpfn: used defined log output function.
*/
LPFNDEFLOGOUTPUT a_RedirectDefLogOutput(LPFNDEFLOGOUTPUT lpfn)
{
	LPFNDEFLOGOUTPUT lpOld = l_lpfnDefLogOutput;
	l_lpfnDefLogOutput = lpfn;
	return lpOld;
}

/*	Output log using default output function. This function adds a return
	character at the end of message automatically.

	iLevel: log level. 0 = log; 1 = error
	szMsg: log message.
*/
void a_LogOutput(int iLevel, const char* szMsg, ...)
{
	char szBuf[1024];

	if (iLevel)
		strcpy(szBuf, "<!> ");
	else
		strcpy(szBuf, "<-> ");

	va_list vaList;
	va_start(vaList, szMsg);
	_vsnprintf(szBuf+4, sizeof (szBuf)-4, szMsg, vaList);
	va_end(vaList);

	strcat(szBuf, "\n");
	_DefLogOutput(szBuf);
}

/*	Output log using default output function. This function output message without
	return character appended.

	iLevel: log level. 0 = log; 1 = error
	szMsg: log message.
*/
void a_LogOutputNoReturn(int iLevel, const char* szMsg, ...)
{
	char szBuf[1024];

	if (iLevel)
		strcpy(szBuf, "<!> ");
	else
		strcpy(szBuf, "<-> ");

	va_list vaList;
	va_start(vaList, szMsg);
	_vsnprintf(szBuf+4, sizeof (szBuf)-4, szMsg, vaList);
	va_end(vaList);

	_DefLogOutput(szBuf);
}

//	Send log to debug window "Angelica Debug Info"
void a_SendLogToDebugWnd(int iLevel, const char* szMsg, ...)
{
	if (!(l_hDebugWnd = FindWindow(NULL, _AL("Angelica Debug Window"))))
		return;

	char szBuf[1024];

	va_list vaList;
	va_start(vaList, szMsg);
	_vsnprintf(szBuf, sizeof (szBuf), szMsg, vaList);
	va_end(vaList);

	COPYDATASTRUCT CopyData;
	CopyData.lpData = szBuf;
	CopyData.cbData	= strlen(szBuf);
	CopyData.dwData	= (DWORD)iLevel;

	::SendMessage(l_hDebugWnd, WM_COPYDATA, 0, (LPARAM)&CopyData);
}

///////////////////////////////////////////////////////////////////////////
//
//	Implement ALog
//
///////////////////////////////////////////////////////////////////////////

ALog::ALog()
{
	m_pFile = NULL;
}

ALog::~ALog()
{
}

bool ALog::Init(const char* szLogFile, const char* szHelloMsg, bool bAppend)
{
	WORD nYear, nMonth, nDay, nDate, nHour, nMinute, nSecond;
	char szLogPath[MAX_PATH];

	_SafeCreateDir(m_szLogDir);

	sprintf(szLogPath, "%s\\%s", m_szLogDir, szLogFile);
	if( bAppend )
		m_pFile = fopen(szLogPath, "at");
	else
		m_pFile = fopen(szLogPath, "wt");
	if( NULL == m_pFile ) 
		return true; 

	a_GetSystemTime(&nYear, &nMonth, &nDay, &nDate, &nHour, &nMinute, &nSecond);
	fprintf(m_pFile, "%s\nCreated(or opened) on %02d/%02d/%04d %02d:%02d:%02d\n\n", szHelloMsg,
		nDay, nMonth, nYear, nHour, nMinute, nSecond);
	return true;
}

bool ALog::Release()
{
	LogString("Log file closed successfully!");

	if( m_pFile )
	{
		fclose(m_pFile);
		m_pFile = NULL;
	}

	return true;
}

bool ALog::Log(const char* fmt, ...)
{
	char szErrorMsg[2048];
	va_list args_list;

    va_start(args_list, fmt);	   
    _vsnprintf(szErrorMsg, sizeof (szErrorMsg), fmt, args_list);
	va_end(args_list);

	return LogString(szErrorMsg);
}

bool ALog::LogString(const char* szString)
{
	if (!m_pFile)
		return true;

	//WORD nYear, nMonth, nDay, nDate, nHour, nMinute, nSecond;
    //a_GetSystemTime(&nYear, &nMonth, &nDay, &nDate, &nHour, &nMinute, &nSecond);

	SYSTEMTIME st;
	GetLocalTime(&st);

    //fprintf(m_pFile, "[%02d:%02d:%02d] %s\n", nHour, nMinute, nSecond, szString);
	fprintf(m_pFile, "[%02d:%02d:%02d.%03d] %s\n", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, szString);
	fflush(m_pFile);
	return true;
}

void ALog::SetLogDir(const char * szLogDir)
{
	strncpy(m_szLogDir, szLogDir, MAX_PATH);

	if( m_szLogDir[strlen(m_szLogDir) - 1] == '\\' )
		m_szLogDir[strlen(m_szLogDir) - 1] = '\0';

	_SafeCreateDir(m_szLogDir);
}

