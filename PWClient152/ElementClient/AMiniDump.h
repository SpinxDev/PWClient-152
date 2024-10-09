/*
 * FILE: AMiniDump.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/8/28
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#ifndef _AMINIDUMP_H_
#define _AMINIDUMP_H_

#include "ACPlatform.h"
#include <DbgHelp.h>

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


///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class AMiniDump
//	
///////////////////////////////////////////////////////////////////////////

class AMiniDump
{
public:		//	Types

	typedef BOOL (WINAPI *MINIDUMPWRITEDUMP)(HANDLE hProcess, DWORD dwPid, HANDLE hFile, MINIDUMP_TYPE DumpType,
											CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
											CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
											CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam);

	//	A couple of private smart types
	struct CSmartModule
	{
		HMODULE m_hModule;

		CSmartModule(HMODULE h) { m_hModule = h; }
		~CSmartModule() { if (m_hModule) ::FreeLibrary(m_hModule); }
	};

	struct CSmartHandle
	{
		HANDLE m_hHandle;

		CSmartHandle(HANDLE h) { m_hHandle = h; }
		~CSmartHandle() { if (m_hHandle && m_hHandle != INVALID_HANDLE_VALUE) ::CloseHandle(m_hHandle); }
	};

public:		//	Constructor and Destructor

	AMiniDump() {}
	~AMiniDump() {}

public:		//	Attributes

public:		//	Operations

	static bool Create(HMODULE hModule, PEXCEPTION_POINTERS pExceptionInfo, const TCHAR* szDumpFile, LPCRITICAL_SECTION pCS);

protected:	//	Attributes

protected:	//	Operations

	static bool GetImpersonationToken(HANDLE* phToken);
	static bool EnablePriv(const TCHAR* pszPriv, HANDLE hToken, TOKEN_PRIVILEGES* ptpOld);
	static bool RestorePriv(HANDLE hToken, TOKEN_PRIVILEGES* ptpOld);
	static HMODULE LoadDbgHelpDLL(HMODULE hModule);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

#endif	//	_AMINIDUMP_H_
