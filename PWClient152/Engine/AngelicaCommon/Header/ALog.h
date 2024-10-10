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

#ifndef _ALOG_H_
#define _ALOG_H_

#include "ACPlatform.h"
#include "ABaseDef.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
///////////////////////////////////////////////////////////////////////////

//	Default log debug function
typedef void (*LPFNDEFLOGOUTPUT)(const char* szMsg);

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////

LPFNDEFLOGOUTPUT a_RedirectDefLogOutput(LPFNDEFLOGOUTPUT lpfn);
void a_LogOutput(int iLevel, const char* szMsg, ...);
void a_LogOutputNoReturn(int iLevel, const char* szMsg, ...);

//	Send log to debug window "Angelica Debug Info"
void a_SendLogToDebugWnd(int iLevel, const char* szMsg, ...);

///////////////////////////////////////////////////////////////////////////
//
//	Class ALog
//
///////////////////////////////////////////////////////////////////////////

class ALog
{
private:
	FILE *			m_pFile;
	static char		m_szLogDir[MAX_PATH];

protected:
public:
	ALog();
	virtual ~ALog();

	// Init a log file
	//		szLogFile	will be the logs path
	//		szHelloMsg	is the hello message in the log
	//		bAppend		is the flag to append at the end of the log file
	bool Init(const char* szLogFile, const char* szHelloMsg, bool bAppend=false);

	// Release the log file
	//		this call will close the log file pointer and write a finished message
	bool Release();

	// Output a variable arguments log message;
	bool Log(const char* fmt, ...);

	// Output a string as a log message;
	bool LogString(const char* szString);

	static void SetLogDir(const char* szLogDir);
	static const char* GetLogDir() { return m_szLogDir; }
};

typedef ALog * PALog;

#endif
