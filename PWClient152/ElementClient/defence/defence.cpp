//#include "stdafx.h"
#include <windows.h>
#include <winsock.h>
#include <stdio.h>

#include "Mouse.h"
#include "gnoctets.h"
#include "gnmarshal.h"
#include "gnmppc.h"
#include "defence.h"
#include "Process.h"
#include "APILoader.h"
#include "infocollection.h"
#include "stackinfocollection.h"
#include "hardinfo.h"

using namespace GNET;

CRITICAL_SECTION	InfoCollection::locker;


std::deque<int> InfoCollection::send_queue;

int InfoCollection::GetNextType()
{
    static int raw_queue[8] = { INFO_STACK, INFO_MOUSE, INFO_PROCESSTIMES, INFO_MODULES, 
    	INFO_THREADSTIMES, INFO_PROCESSLIST, INFO_WINDOWLIST, INFO_PLATFORMVERSION };
    int r = 0;
    EnterCriticalSection(&locker);
    if( send_queue.empty() )
    {
        std::copy(raw_queue, raw_queue+8, std::back_inserter(send_queue));
        std::random_shuffle(send_queue.begin(), send_queue.end());
    }
    r = send_queue.front();
    send_queue.pop_front();
    LeaveCriticalSection(&locker);
    return r;
}

/*
std::set<int>	InfoCollection::sendTypes;

int InfoCollection::GetNextType()
{
	int count = INFO_MAX-INFO_MIN+1;
	int index = INFO_MIN + rand() % count;
	EnterCriticalSection(&locker);
	for( int c=0; c<count; c++ )
	{
		std::set<int>::const_iterator it = sendTypes.find(index);
		if( it == sendTypes.end() )
		{
			sendTypes.insert(index);
			return index;
		}
		index = (index>=INFO_MAX ? INFO_MIN : index+1);
	}
	sendTypes.clear();
	sendTypes.insert(index);
	LeaveCriticalSection(&locker);
	return index;
}
*/
namespace Collector
{

	Engine::Engine() 
{
	
	InitializeCriticalSection(&InfoCollection::locker);

	hInstKERNEL = NULL;
	hInstPSAPI = NULL;
	hInstVDMDBG = NULL;
	//
	lpfCreateToolhelp32Snapshot = NULL;
	lpfProcess32First = NULL;
	lpfProcess32Next = NULL;
	lpfGetProcessTimes = NULL;
	lpfModule32First = NULL;
	lpfModule32Next = NULL;
	lpfOpenThread = NULL;
	lpfThread32First = NULL;
	lpfThread32Next = NULL;
	lpfGetThreadTimes = NULL;
	lpfEnumProcesses = NULL;
	lpfEnumProcessModules = NULL;
	lpfGetProcessMemoryInfo = NULL;
	lpfQueryWorkingSet = NULL;
	lpfGetModuleBaseName = NULL;
	lpfGetModuleInformation = NULL;
	lpfVDMEnumTaskWOWEx = NULL;

	osver.dwOSVersionInfoSize = sizeof(osver);
	GetVersionEx(&osver);
	
	meminfo.dwLength = sizeof(meminfo);
	GlobalMemoryStatus(&meminfo);
	GetSystemInfo(&sysinfo);	

	if( HasToolHelp32() )
	{
		hInstKERNEL = LoadLibraryA("Kernel32.DLL");
		if( hInstKERNEL )
		{
			lpfCreateToolhelp32Snapshot = (HANDLE (WINAPI *)(DWORD,DWORD))
					GetProcAddress(hInstKERNEL, "CreateToolhelp32Snapshot");
			lpfProcess32First = (BOOL (WINAPI *)(HANDLE,LPPROCESSENTRY32))
					GetProcAddress(hInstKERNEL, "Process32First");
			lpfProcess32Next = (BOOL (WINAPI *)(HANDLE,LPPROCESSENTRY32))
					GetProcAddress(hInstKERNEL, "Process32Next");
			lpfGetProcessTimes = (BOOL (WINAPI *)(HANDLE,LPFILETIME,LPFILETIME,LPFILETIME,LPFILETIME))
					GetProcAddress(hInstKERNEL, "GetProcessTimes");
			lpfModule32First = (BOOL (WINAPI *)(HANDLE,LPMODULEENTRY32))
					GetProcAddress(hInstKERNEL, "Module32First");
			lpfModule32Next = (BOOL (WINAPI *)(HANDLE,LPMODULEENTRY32))
					GetProcAddress(hInstKERNEL, "Module32Next");
			lpfOpenThread = (HANDLE (WINAPI *)(DWORD,BOOL,DWORD))
					GetProcAddress(hInstKERNEL, "OpenThread");
			lpfThread32First = (BOOL (WINAPI *)(HANDLE,LPTHREADENTRY32))
					GetProcAddress(hInstKERNEL, "Thread32First");
			lpfThread32Next = (BOOL (WINAPI *)(HANDLE,LPTHREADENTRY32))
					GetProcAddress(hInstKERNEL, "Thread32Next");
			lpfGetThreadTimes = (BOOL (WINAPI *)(HANDLE,LPFILETIME,LPFILETIME,LPFILETIME,LPFILETIME))
					GetProcAddress(hInstKERNEL, "GetThreadTimes");
		}
	}
	if( HasPSAPI() )
	{
		//
		hInstPSAPI = LoadLibraryA("PSAPI.DLL");
		if( hInstPSAPI )
		{
			lpfEnumProcesses = (BOOL (WINAPI *)(DWORD *, DWORD, DWORD*))
					GetProcAddress(hInstPSAPI, "EnumProcesses");
			lpfEnumProcessModules = (BOOL (WINAPI *)(HANDLE, HMODULE *, DWORD, LPDWORD))
					GetProcAddress(hInstPSAPI, "EnumProcessModules");
			lpfGetProcessMemoryInfo = (BOOL (WINAPI *)(HANDLE, PPROCESS_MEMORY_COUNTERS, DWORD))
					GetProcAddress(hInstPSAPI, "GetProcessMemoryInfo");
			lpfQueryWorkingSet = (BOOL (WINAPI *)(HANDLE hProcess, PVOID pv, DWORD cb))
					GetProcAddress(hInstPSAPI, "QueryWorkingSet");
			lpfGetModuleBaseName = (DWORD (WINAPI *)(HANDLE, HMODULE, LPSTR, DWORD))
					GetProcAddress(hInstPSAPI, "GetModuleBaseNameA");
			lpfGetModuleInformation = (BOOL (WINAPI *)(HANDLE, HMODULE, LPMODULEINFO, DWORD))
					GetProcAddress(hInstPSAPI, "GetModuleInformation");
		}
	}

	if( IsNT() )
	{
		hInstVDMDBG = LoadLibraryA("VDMDBG.DLL");
		if( hInstVDMDBG )
		{
			lpfVDMEnumTaskWOWEx = (INT (WINAPI *)(DWORD, TASKENUMPROCEX,
				LPARAM)) GetProcAddress(hInstVDMDBG, "VDMEnumTaskWOWEx");
		}
	}
}
Engine::~Engine()
{
#ifdef USING_DEFENCE
	WSACleanup();
#endif

	if( hInstPSAPI )	FreeLibrary(hInstPSAPI);
	if( hInstKERNEL )	FreeLibrary(hInstKERNEL);
	if( hInstVDMDBG )	FreeLibrary(hInstVDMDBG);

	DeleteCriticalSection(&InfoCollection::locker);
}
Marshal::OctetsStream& Engine::marshalPlatFormVersion(Marshal::OctetsStream& os) const
{
	Octets hardinfo;
	DWORD hardinfo_len = 1024;
	hardinfo.resize(hardinfo_len);
	GetHardInfo((BYTE*)hardinfo.begin(), &hardinfo_len);
	hardinfo.resize(hardinfo_len);

	return os << CompactUINT(osver.dwPlatformId) << CompactUINT(osver.dwMajorVersion)
		<< CompactUINT(osver.dwMinorVersion) << CompactUINT(osver.dwBuildNumber)
		<< ((unsigned int)(meminfo.dwTotalPhys))
		<< ((unsigned short)(sysinfo.wProcessorArchitecture))
		<< ((unsigned short)(sysinfo.wProcessorLevel))
		<< cpu_ct
		<< adapter_description
		<< hardinfo;
	

}
//Engine	Engine::g_engine;

static void Compress( Octets & os_src, Octets & os_com )
{
	size_t len_src = os_src.size();

#ifdef USE_ZLIB
	uLong len_com =  compressBound(len_src);
	os_com.reserve(len_com);
	bool success = ( Z_OK == compress((Bytef*)os_com.begin(),&len_com,(Bytef*)os_src.begin(),len_src)
		&& len_com<len_src );
#else
	size_t len_com = mppc::compressBound(len_src);
	os_com.reserve(len_com);
	bool success;
	if( len_src <= 8192 )
	{
		success = ( 0 == mppc::compress((unsigned char*)os_com.begin(),(int*)&len_com,
								(const unsigned char*)os_src.begin(),len_src)
				&& len_com<len_src );
	}
	else
	{
		success = ( 0 == mppc::compress2((unsigned char*)os_com.begin(),(int*)&len_com,
								(const unsigned char*)os_src.begin(),len_src)
			&& len_com<len_src );
	}
#endif

	if( success )
	{
		Marshal::OctetsStream	os;
		os.reserve( 2 * sizeof(int) + len_com + os.size() );
		os << CompactUINT(len_src) << CompactUINT(len_com);
		os.push_byte( (const char*)os_com.begin(), len_com );
		os_com.swap( os );
	}
	else
	{
		Marshal::OctetsStream	os;
		os.reserve( 2 * sizeof(int) + len_src + os.size() );
		os << CompactUINT(len_src) << CompactUINT(len_src);
		os.push_byte( (const char*)os_src.begin(), len_src );
		os_com.swap( os );
	}
}
static void Uncompress( Octets & os_com, Octets & os_src )
{
	Marshal::OctetsStream os;
	os_com.swap( os );
	size_t len_src, len_com;
	os >> CompactUINT(len_src) >> Marshal::Begin >> CompactUINT(len_com);
	
	if( len_com < len_src )
	{
		os >> Marshal::Rollback;
		os_com.reserve(len_com+8);
		os >> os_com;
		((Octets&)os_src).reserve( len_src );
#ifdef USE_ZLIB
		bool success = (Z_OK == uncompress((Bytef*)os_src.begin(),(uLongf*)&len_src,
						(const Bytef*)os_com.begin(),os_com.size()));
#else
		bool success = ( (len_src<=8192 && 0 == mppc::uncompress((unsigned char*)os_src.begin(),(int*)&len_src,
						(const unsigned char*)os_com.begin(),os_com.size()))
			|| (len_src>8192 && 0 == mppc::uncompress2((unsigned char*)os_src.begin(),(int*)&len_src,
						(const unsigned char*)os_com.begin(),os_com.size())) );
#endif
		os_src.resize( len_src );
		if( !success )
			os_src.swap( os_com );
		os_com.swap( os );
	}
	else
	{
		os >> Marshal::Rollback;
		os >> os_src;
		os_com.swap( os );
	}
}

void WINAPI Startup()
{
	Mouse::GetInstance().SetMouseHook();
}

void WINAPI Cleanup()
{
	Mouse::GetInstance().ReleaseMouseHook();
}

int WINAPI GetInfoAll( LPBYTE *pbuf, LPCSTR lpWndClass, LPCSTR lpWndName )
{
	Marshal::OctetsStream	os;
#ifdef BRIEF
	os << CHAR(InfoCollection::INFO_STACKBRIEF);
#else
	os << CHAR(InfoCollection::INFO_STACK);
#endif
	os << StackInfoCollection::GetInstance();

	os << CHAR(InfoCollection::INFO_MOUSE);
	os << Mouse::GetInstance();

	Process	p( lpWndClass, lpWndName );
	os << CHAR(InfoCollection::INFO_MEMORY);
	p.marshalMemory(os);
	os << CHAR(InfoCollection::INFO_PROCESSTIMES);
	p.marshalProcessTimes(os);
	os << CHAR(InfoCollection::INFO_MODULES);
	p.marshalModules(os);
	os << CHAR(InfoCollection::INFO_THREADSTIMES);
	p.marshalThreadsTimes(os);
	os << CHAR(InfoCollection::INFO_PROCESSLIST);
	p.marshalProcessList(os);
	os << CHAR(InfoCollection::INFO_WINDOWLIST);
	p.marshalWindowList(os);

	os << CHAR(InfoCollection::INFO_PLATFORMVERSION);
	Collector::Engine::GetInstance().marshalPlatFormVersion(os);

	Octets os_com;
	Compress( os, os_com );
	if( os_com.size() > 0 )
	{
		*pbuf = new BYTE[os_com.size()+1];
		if( *pbuf )
			memcpy( *pbuf, os_com.begin(), os_com.size() );
	}
	return os_com.size();
}
int WINAPI GetInfoBrief( LPBYTE *pbuf, LPCSTR lpWndClass, LPCSTR lpWndName )
{
	Marshal::OctetsStream	os;
#ifdef BRIEF
	os << CHAR(InfoCollection::INFO_STACKBRIEF);
#else
	os << CHAR(InfoCollection::INFO_STACK);
#endif
	os << StackInfoCollection::GetInstance();

	os << CHAR(InfoCollection::INFO_MOUSE);
	os << Mouse::GetInstance();

	Process	p( lpWndClass, lpWndName );
	//os << CHAR(InfoCollection::INFO_MEMORY);
	//p.marshalMemory(os);
	os << CHAR(InfoCollection::INFO_PROCESSTIMES);
	p.marshalProcessTimes(os);
	os << CHAR(InfoCollection::INFO_MODULES);
	p.marshalModules(os);
	os << CHAR(InfoCollection::INFO_THREADSTIMES);
	p.marshalThreadsTimes(os);
	os << CHAR(InfoCollection::INFO_PROCESSLIST);
	p.marshalProcessList(os);
	os << CHAR(InfoCollection::INFO_WINDOWLIST);
	p.marshalWindowList(os);

	os << CHAR(InfoCollection::INFO_PLATFORMVERSION);
	Collector::Engine::GetInstance().marshalPlatFormVersion(os);

	Octets os_com;
	Compress( os, os_com );
	if( os_com.size() > 0 )
	{
		*pbuf = new BYTE[os_com.size()+1];
		if( *pbuf )
			memcpy( *pbuf, os_com.begin(), os_com.size() );
	}
	return os_com.size();
}

int WINAPI GetInfoNetDLL( LPBYTE *pbuf )
{
	Octets os_com;
	Marshal::OctetsStream	os;
	os << CHAR(InfoCollection::INFO_APILOADER);
	NetDLL::DumpResult( os );

	Compress( os, os_com );
	if( os_com.size() > 0 )
	{
		*pbuf = new BYTE[os_com.size()+1];
		if( *pbuf )
			memcpy( *pbuf, os_com.begin(), os_com.size() );
	}
	return os_com.size();
}

int WINAPI GetInfoNetDLLEx( LPBYTE *pbuf )
{
	Octets os_com;
	Marshal::OctetsStream	os;
	os << CHAR(InfoCollection::INFO_SPY);
	if (!NetDLL::DumpResultEx( os ))
		return 0;

	Compress( os, os_com );
	if( os_com.size() > 0 )
	{
		*pbuf = new BYTE[os_com.size()+1];
		if( *pbuf )
			memcpy( *pbuf, os_com.begin(), os_com.size() );
	}
	return os_com.size();
}

int WINAPI GetInfo( int nType, LPBYTE *pbuf, LPCSTR lpWndClass, LPCSTR lpWndName )
{
	Octets os_com;
	Marshal::OctetsStream	os;
	switch( nType )
	{
	case InfoCollection::INFO_STACK:
	case InfoCollection::INFO_STACKBRIEF:
#ifdef BRIEF
		os << CHAR(InfoCollection::INFO_STACKBRIEF);
#else
		os << CHAR(InfoCollection::INFO_STACK);
#endif
		os << StackInfoCollection::GetInstance();
		break;
	case InfoCollection::INFO_MOUSE:
		os << CHAR(InfoCollection::INFO_MOUSE);
		os << Mouse::GetInstance();
		break;
	//case InfoCollection::INFO_MEMORY:
	//	{
	//		Process	p( lpWndClass, lpWndName );
	//		os << CHAR(InfoCollection::INFO_MEMORY);
	//		p.marshalMemory(os);
	//	}
	//	break;
	case InfoCollection::INFO_PROCESSTIMES:
		{
			Process	p( lpWndClass, lpWndName );
			os << CHAR(InfoCollection::INFO_PROCESSTIMES);
			p.marshalProcessTimes(os);
		}
		break;
	case InfoCollection::INFO_MODULES:
		{
			Process	p( lpWndClass, lpWndName );
			os << CHAR(InfoCollection::INFO_MODULES);
			p.marshalModules(os);
		}
		break;
	case InfoCollection::INFO_THREADSTIMES:
		{
			Process	p( lpWndClass, lpWndName );
			os << CHAR(InfoCollection::INFO_THREADSTIMES);
			p.marshalThreadsTimes(os);
		}
		break;
	case InfoCollection::INFO_PROCESSLIST:
		os << CHAR(InfoCollection::INFO_PROCESSLIST);
		Process::marshalProcessList(os);
		break;
	case InfoCollection::INFO_WINDOWLIST:
		os << CHAR(InfoCollection::INFO_WINDOWLIST);
		Process::marshalWindowList(os);
		break;
	case InfoCollection::INFO_PLATFORMVERSION:
		os << CHAR(InfoCollection::INFO_PLATFORMVERSION);
		Collector::Engine::GetInstance().marshalPlatFormVersion(os);
		break;
	//case InfoCollection::INFO_APILOADER:
	//	break;
	default:
		return 0;
	}

	Compress( os, os_com );
	if( os_com.size() > 0 )
	{
		*pbuf = new BYTE[os_com.size()+1];
		if( *pbuf )
			memcpy( *pbuf, os_com.begin(), os_com.size() );
	}
	return os_com.size();
}
//extern void stack_trace(LPCVOID sv0, LPCVOID sv1);
int WINAPI GetNextInfoType()
{
	//char buffer1[60];
	//sprintf(buffer1,"threadid: %08x\n", GetCurrentThreadId());
	//MessageBoxA(0,buffer1,"ok",0);
	//OutputDebugStringA("== timer trace before\n");
	//stack_trace((LPCVOID)1,(LPCVOID)1);
	//OutputDebugStringA("== timer trace after\n");
	return InfoCollection::GetNextType();
}

};

