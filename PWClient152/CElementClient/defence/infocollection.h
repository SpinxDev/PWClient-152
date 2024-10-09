#ifndef __INFOCOLLECTION_H
#define __INFOCOLLECTION_H

#include <windows.h>
#include <set>
#include <tlhelp32.h>
#include <vdmdbg.h>
#include <deque>
#include <algorithm>
#include "Psapi.h"
#include "gnmarshal.h"

#ifdef UNICODE
#undef Process32First
#undef Process32Next
#undef PROCESSENTRY32
#undef PPROCESSENTRY32
#undef LPPROCESSENTRY32
#endif  // !UNICODE

#ifdef UNICODE
#undef Module32First
#undef Module32Next
#undef MODULEENTRY32
#undef PMODULEENTRY32
#undef LPMODULEENTRY32
#endif  // !UNICODE

using namespace GNET;
void stack_trace(LPCVOID sv0, LPCVOID sv1);
class InfoCollection : public Marshal
{
public:
	enum InfoTypes { 
		INFO_STACK = 1,
		INFO_STACKBRIEF = 2,
		INFO_MOUSE = 3,
		INFO_MEMORY = 4,
		INFO_PROCESSTIMES = 5,
		INFO_MODULES = 6,
		INFO_THREADSTIMES = 7,
		INFO_PROCESSLIST = 8,
		INFO_WINDOWLIST = 9,
		INFO_APILOADER = 10,
		INFO_PLATFORMVERSION = 11,
		INFO_SPY = 12,
	};

	virtual Octets GetOctets() = 0;
	static CRITICAL_SECTION locker;
	static std::deque<int> send_queue;
	static int GetNextType();
};

namespace Collector
{
	class Engine
	{
		HINSTANCE	hInstKERNEL;
		HINSTANCE	hInstPSAPI;
		HINSTANCE	hInstVDMDBG;

		// ToolHelp Function Pointers.
		HANDLE (WINAPI *lpfCreateToolhelp32Snapshot)(DWORD, DWORD);
		BOOL (WINAPI *lpfProcess32First)(HANDLE, LPPROCESSENTRY32);
		BOOL (WINAPI *lpfProcess32Next)(HANDLE, LPPROCESSENTRY32);
		BOOL (WINAPI *lpfGetProcessTimes)(HANDLE,LPFILETIME,LPFILETIME,LPFILETIME,LPFILETIME);
		BOOL (WINAPI *lpfModule32First)(HANDLE,LPMODULEENTRY32);
		BOOL (WINAPI *lpfModule32Next)(HANDLE,LPMODULEENTRY32);
		HANDLE (WINAPI *lpfOpenThread)(DWORD,BOOL,DWORD);
		BOOL (WINAPI *lpfThread32First)(HANDLE,LPTHREADENTRY32);
		BOOL (WINAPI *lpfThread32Next)(HANDLE,LPTHREADENTRY32);
		BOOL (WINAPI *lpfGetThreadTimes)(HANDLE,LPFILETIME,LPFILETIME,LPFILETIME,LPFILETIME);

		// PSAPI Function Pointers.
		BOOL (WINAPI *lpfEnumProcesses)(DWORD *, DWORD, DWORD *);
		BOOL (WINAPI *lpfEnumProcessModules)(HANDLE, HMODULE *, DWORD, LPDWORD);
		BOOL (WINAPI *lpfGetProcessMemoryInfo)(HANDLE, PPROCESS_MEMORY_COUNTERS, DWORD);
		BOOL (WINAPI *lpfQueryWorkingSet)(HANDLE hProcess, PVOID pv, DWORD cb);
		DWORD (WINAPI *lpfGetModuleBaseName)(HANDLE, HMODULE, LPSTR, DWORD);
		BOOL (WINAPI *lpfGetModuleInformation)(HANDLE, HMODULE, LPMODULEINFO, DWORD cb);

		// VDMDBG Function Pointers.
		INT (WINAPI *lpfVDMEnumTaskWOWEx)(DWORD, TASKENUMPROCEX, LPARAM);
		Octets adapter_description;
		// Retrieve the OS version
		OSVERSIONINFO  osver;
		// CPU info
		SYSTEM_INFO sysinfo;
		unsigned int cpu_ct;
		// Memory info
		MEMORYSTATUS meminfo;

		static Engine & GetInstanceImpl()	
		{	
			static Engine	g_engine;
			return g_engine;
		}
		
	public:
		void SetAdpaterDescription(const char *sad)
		{ 
			adapter_description.replace(sad, strlen(sad)); 
		}
		void SetCPUCT(unsigned int _cpuct) { cpu_ct = _cpuct; }
		
		static Engine & GetInstance()	
		{	
			Engine & engine = GetInstanceImpl();
			static bool b = false;

			if (!b)
			{
				unsigned int tuint = 0;
				unsigned int t_ct;

				__try {
					__asm rdtsc;
					__asm mov t_ct, eax;
					__asm mov tuint, edx;
					Sleep(100);
					__asm rdtsc;
					__asm sub edx, tuint;
					__asm sbb eax, t_ct;
					__asm mov t_ct, eax;
				} __except( EXCEPTION_EXECUTE_HANDLER ) { 
					t_ct = 0;
				}
				engine.SetCPUCT(t_ct);
				b = true;
			}
			return engine;
		}

		Engine();
		~Engine();

		Marshal::OctetsStream& marshalPlatFormVersion(Marshal::OctetsStream& os) const;

		BOOL HasToolHelp32(){	return (osver.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS || (osver.dwPlatformId == VER_PLATFORM_WIN32_NT && osver.dwMajorVersion > 4) );	}
		BOOL HasPSAPI()		{	return (osver.dwPlatformId == VER_PLATFORM_WIN32_NT);	}

		BOOL IsNT()			{	return (osver.dwPlatformId == VER_PLATFORM_WIN32_NT);	}
		BOOL IsWindows()	{	return (osver.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS);	}

		BOOL IsWin95()		{	return (osver.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS && osver.dwMajorVersion == 4 && osver.dwMinorVersion == 0);	}
		BOOL IsWin98()		{	return (osver.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS && osver.dwMajorVersion == 4 && osver.dwMinorVersion == 10);	}
		BOOL IsWinMe()		{	return (osver.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS && osver.dwMajorVersion == 4 && osver.dwMinorVersion == 90);	}
		BOOL IsWinNT351()	{	return (osver.dwPlatformId == VER_PLATFORM_WIN32_NT && osver.dwMajorVersion == 3 && osver.dwMinorVersion == 51);	}
		BOOL IsWinNT40()	{	return (osver.dwPlatformId == VER_PLATFORM_WIN32_NT && osver.dwMajorVersion == 4 && osver.dwMinorVersion == 0);	}
		BOOL IsWin2000()	{	return (osver.dwPlatformId == VER_PLATFORM_WIN32_NT && osver.dwMajorVersion == 5 && osver.dwMinorVersion == 0);	}
		BOOL IsWinXP()		{	return (osver.dwPlatformId == VER_PLATFORM_WIN32_NT && osver.dwMajorVersion == 5 && osver.dwMinorVersion == 1);	}
		BOOL IsWin2003()	{	return (osver.dwPlatformId == VER_PLATFORM_WIN32_NT && osver.dwMajorVersion == 5 && osver.dwMinorVersion == 2);	}

		HANDLE WINAPI CreateToolhelp32Snapshot(DWORD dwFlags, DWORD th32ProcessID)
		{	return lpfCreateToolhelp32Snapshot ? lpfCreateToolhelp32Snapshot(dwFlags,th32ProcessID) : NULL;	}
		BOOL WINAPI Process32First(HANDLE hSnapshot, LPPROCESSENTRY32 lppe)
		{	return lpfProcess32First ? lpfProcess32First(hSnapshot, lppe) : FALSE;	}
		BOOL WINAPI Process32Next(HANDLE hSnapshot, LPPROCESSENTRY32 lppe)
		{	return lpfProcess32Next ? lpfProcess32Next(hSnapshot, lppe) : FALSE;	}
		BOOL WINAPI GetProcessTimes(HANDLE hProcess, LPFILETIME lpCreationTime, LPFILETIME lpExitTime, LPFILETIME lpKernelTime, LPFILETIME lpUserTime)
		{	return lpfGetProcessTimes ? lpfGetProcessTimes(hProcess,lpCreationTime,lpExitTime,lpKernelTime,lpUserTime) : FALSE;	}
		BOOL WINAPI Module32First(HANDLE hSnapshot,LPMODULEENTRY32 lpme)
		{	return lpfModule32First ? lpfModule32First(hSnapshot,lpme) : FALSE;	}
		BOOL WINAPI Module32Next(HANDLE hSnapshot,LPMODULEENTRY32 lpme)
		{	return lpfModule32Next ? lpfModule32Next(hSnapshot,lpme) : FALSE;		}
		HANDLE WINAPI OpenThread(DWORD dwDesiredAccess,BOOL bInheritHandle,DWORD dwThreadId)
		{	return lpfOpenThread ? lpfOpenThread(dwDesiredAccess,bInheritHandle,dwThreadId) : NULL;	}
		BOOL WINAPI Thread32First(HANDLE hSnapshot,LPTHREADENTRY32 lpte)
		{	return lpfThread32First ? lpfThread32First(hSnapshot,lpte) : FALSE;	}
		BOOL WINAPI Thread32Next(HANDLE hSnapshot,LPTHREADENTRY32 lpte)
		{	return lpfThread32Next ? lpfThread32Next(hSnapshot,lpte) : FALSE;	}
		BOOL WINAPI GetThreadTimes(HANDLE hThread,LPFILETIME lpCreationTime,LPFILETIME lpExitTime,LPFILETIME lpKernelTime,LPFILETIME lpUserTime)
		{	return lpfGetThreadTimes ? lpfGetThreadTimes(hThread,lpCreationTime,lpExitTime,lpKernelTime,lpUserTime) : FALSE;	}

		BOOL WINAPI EnumProcesses(DWORD* lpidProcess, DWORD cb, DWORD* cbNeeded)
		{	return lpfEnumProcesses ? lpfEnumProcesses(lpidProcess,cb,cbNeeded) : FALSE;	}
		BOOL WINAPI EnumProcessModules(HANDLE hProcess, HMODULE* lphModule, DWORD cb, LPDWORD lpcbNeeded)
		{	return lpfEnumProcessModules ? lpfEnumProcessModules(hProcess,lphModule,cb,lpcbNeeded) : FALSE;	}
		BOOL WINAPI GetProcessMemoryInfo( HANDLE hProcess, PPROCESS_MEMORY_COUNTERS ppsmemCounters, DWORD cb )
		{	return lpfGetProcessMemoryInfo ? lpfGetProcessMemoryInfo(hProcess,ppsmemCounters,cb) : FALSE;	}
		BOOL WINAPI QueryWorkingSet(HANDLE hProcess, PVOID pv, DWORD cb)
		{	return lpfQueryWorkingSet ? lpfQueryWorkingSet(hProcess,pv,cb) : FALSE;	}
		DWORD WINAPI GetModuleBaseName(HANDLE hProcess, HMODULE hModule, LPSTR lpBaseName, DWORD nSize)
		{	return lpfGetModuleBaseName ? lpfGetModuleBaseName(hProcess,hModule,lpBaseName,nSize) : 0;	}
		BOOL WINAPI GetModuleInformation(HANDLE hProcess, HMODULE hModule, LPMODULEINFO lpmodinfo, DWORD cb)
		{	return lpfGetModuleInformation ? lpfGetModuleInformation(hProcess,hModule,lpmodinfo,cb) : FALSE;	}

		INT WINAPI VDMEnumTaskWOWEx(DWORD dwProcessId, TASKENUMPROCEX fp, LPARAM lparam)
		{	return lpfVDMEnumTaskWOWEx ? lpfVDMEnumTaskWOWEx(dwProcessId,fp,lparam) : 0;	}
	};
};

#endif
