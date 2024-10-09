
//#include "stdafx.h"
#include "pwacp.h"
#include <stdio.h>
#include <time.h>
#include <winioctl.h>
//#include <imagehlp.h>

namespace PWACP
{

#define IOCTL_ATTACH CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_READ_ACCESS + FILE_WRITE_ACCESS)
#define IOCTL_DETACH CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_READ_ACCESS + FILE_WRITE_ACCESS)
//#define IOCTL_TALK CTL_CODE(FILE_DEVICE_UNKNOWN, 0x802, METHOD_BUFFERED, FILE_READ_ACCESS + FILE_WRITE_ACCESS)

static SC_HANDLE hSCManager = NULL;
static SC_HANDLE hService	 = NULL;
static HANDLE hDevice	 = INVALID_HANDLE_VALUE;
static SERVICE_STATUS ss;

static char szFullName[MAX_PATH] = { 0 };
//static char szDeviceName[MAX_PATH] = { 0 };
//static char szVarName[18] = { 0 };
static char szMsgBuffer[128] = { 0 };




static void err(const char* msg)
{
	//	MessageBoxA(NULL, msg, NULL, NULL);
}
static void err1(const char*msg, DWORD p1)
{
	//FILE *f = fopen("c:\\err.log", "wb");
	//fprintf(f, msg, p1);
	//fclose(f);
}


static bool IsWinXP()
{
	OSVERSIONINFOEX osvi;
	BOOL bOsVersionInfoEx;
	
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	
	if(  !(bOsVersionInfoEx = GetVersionEx((OSVERSIONINFO*)&osvi)))
	{
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		if( !GetVersionEx((OSVERSIONINFO*)&osvi))
			return false;
	}
	
	if( osvi.dwPlatformId == VER_PLATFORM_WIN32_NT 
		&& osvi.dwMajorVersion == 5
		&& osvi.dwMinorVersion == 1 )
		return true;
	return false;
}

static BOOL HasDebugger()
{
	return FALSE;
}

BOOL WINAPI ACPInit(VOID)
{
	if( HasDebugger() ) return FALSE;

	FILE *f = fopen("pwacp.sys","r");
	if( f == NULL ) return FALSE;
	fclose(f);

	if( ! IsWinXP() ) return TRUE;
	
	char *dummy = NULL;
	GetFullPathNameA("pwacp.sys", MAX_PATH, szFullName, &dummy);
	
	hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if( hSCManager != NULL ) 
	{
		
		
		hService = CreateServiceA(hSCManager, "pwacp", "pwacp", 
			SERVICE_START+DELETE, SERVICE_KERNEL_DRIVER, SERVICE_DEMAND_START, 
			SERVICE_ERROR_IGNORE,
			szFullName,
			NULL, NULL, NULL, NULL, NULL);
		
		if( hService == NULL )
		{
			hService = OpenServiceA(hSCManager, "pwacp", SERVICE_START+DELETE);
		}
		
		if( hService != NULL )
		{
			StartService(hService, NULL, NULL );
			DeleteService(hService);
			CloseServiceHandle(hService);
			hService = NULL;
		}
		
	}
	//
	
	hDevice = CreateFileA("\\\\.\\slPWACP", GENERIC_READ + GENERIC_WRITE, 
		0, NULL, OPEN_EXISTING, 0, NULL);
	
	if( hDevice != INVALID_HANDLE_VALUE )
	{
		DWORD dwPID = GetCurrentProcessId();
		DWORD dwRetCode = 0;
		DWORD dwBytesReturned = 0;
		
		if( DeviceIoControl(hDevice, IOCTL_ATTACH, &dwPID, sizeof(DWORD), 
			&dwRetCode, sizeof(DWORD), &dwBytesReturned, NULL))
		{
			if( dwBytesReturned != 0 )
			{
				if( dwRetCode == 1 )
				{
					return FALSE;
				}
			}
			
		}
		else
			return FALSE;
	}
				
	return TRUE;
}

DWORD WINAPI ACPTalk(DWORD* param)
{
	return 0;
}

BOOL WINAPI ACPDestroy(VOID)
{
	if( ! IsWinXP() ) return TRUE;
	
	if( hDevice != INVALID_HANDLE_VALUE )
	{
		DWORD dwBytesReturned = 0;
		DWORD dummy = GetCurrentProcessId();
		if( DeviceIoControl(hDevice, IOCTL_DETACH, &dummy, sizeof(DWORD), 
			&dummy, sizeof(DWORD), &dwBytesReturned, NULL))
		{
			if( dwBytesReturned != 0 )
			{
				
				//sprintf(szMsgBuffer, "ret = %d", dummy);
				//MessageBox(0, szMsgBuffer, "DeviceReturn", 0);
			}
			else
				//MessageBox(NULL, "dwBytesReturned == 0 ", NULL, MB_OK + MB_ICONSTOP)
				;
		}
		else
		{
			//DWORD en = GetLastError();
			//sprintf(szMsgBuffer, "errno = 0x%08x, foo= 0x%08x", en, IOCTL_DETACH);
			//err(szMsgBuffer);
		}
	}			
				
	
	//if( hDevice == INVALID_HANDLE_VALUE )
	//	return FALSE;
	//	CloseHandle(hDevice);
	//	hDevice = INVALID_HANDLE_VALUE;
	
	
	
	//	return FALSE;
	//ControlService(hService, SERVICE_CONTROL_STOP, &ss);
	
	
	//if( hSCManager == NULL )
	//	return FALSE;
	if( hSCManager != NULL )
		CloseServiceHandle(hSCManager);
	hSCManager = NULL;
	return TRUE;
}

};