#pragma once

#include <windows.h>
#include <stdio.h>
#include "infocollection.h"

typedef BOOL (CALLBACK *PROCENUMPROC)(DWORD, WORD, LPSTR, LPARAM);

typedef struct {
	DWORD dwPID;
	PROCENUMPROC lpProc;
	DWORD lParam;
	BOOL bEnd;
} EnumInfoStruct;

BOOL WINAPI EnumProcs(PROCENUMPROC lpProc, LPARAM lParam);

BOOL WINAPI Enum16(DWORD dwThreadId, WORD hMod16, WORD hTask16,
				   PSZ pszModName, PSZ pszFileName, LPARAM lpUserDefined);


/*
BOOL CALLBACK MyProcessEnumerator(DWORD dwPID, WORD wTask,
								  LPCSTR szProcess, LPARAM lParam)
{
	if (wTask == 0)
		printf("%5u %s\n", dwPID, szProcess);
	else
		printf(" %5u %s\n", wTask, szProcess);
	
	return TRUE;
}

void MyEnumAllProcesses( ) {
	EnumProcs((PROCENUMPROC) MyProcessEnumerator, NULL);
}
*/
