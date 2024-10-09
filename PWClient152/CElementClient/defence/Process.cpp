//#include "stdafx.h"
#include "process.h"
#include "EnumWin32Pro.h"
#include "infocollection.h"
#include <windows.h>
#include <algorithm>
#include <functional>

using namespace Collector;

std::vector<std::pair<std::string,int > > Process::aModules;
std::vector<std::string> Process::aProcesses;
std::vector<std::string> Process::aWindows;

Process::Process(DWORD _dwPID) : dwPID(_dwPID), pWorkingSet(NULL)
{
}

Process::Process( LPCSTR lpFileTitle ) : dwPID(0), pWorkingSet(NULL)
{
	if( NULL == lpFileTitle )
		return;

	std::vector<DWORD>			aPID;
	std::vector<std::string>	aPName;

	if( !GetProcessList( aPID, aPName ) )
		return;

	for( size_t i=0; i<aPID.size() && i<aPName.size(); i++ )
	{
		if( 0 == strcmpi(aPName[i].c_str(), lpFileTitle) )
		{
			dwPID = aPID[i];
			break;
		}
	}
}

Process::Process( LPCSTR lpClassName, LPCSTR lpWindowName) : dwPID(0), pWorkingSet(NULL)
{
	if( lpWindowName && strlen(lpWindowName) > 0 )
	{
		if( NULL == lpClassName || strlen(lpClassName) == 0 )
			lpClassName = NULL;

		HWND hWnd = ::FindWindowA( lpClassName, lpWindowName );
		if( NULL == hWnd )
			return;

		DWORD tid = ::GetWindowThreadProcessId( hWnd, &dwPID );
	}
	else
	{
		dwPID = ::GetCurrentProcessId();
	}
}

Process::~Process(void)
{
	if( pWorkingSet )
	{
		free(pWorkingSet);
		pWorkingSet = NULL;
	}
}

Marshal::OctetsStream& Process::marshalMemory(Marshal::OctetsStream& os) const
{
	size_t i;

	// memory
	Process * pThis = (Process *)this;
	DWORD * ws = pThis->GetWorkingSet();
	if( ws )
	{
		os << CompactUINT(ws[0]);
		for( i=1; i<=ws[0]; i++ )
			os << ws[i];
	}
	else
	{
		os << CompactUINT((DWORD)0);
	}
	return os;
}
Marshal::OctetsStream& Process::marshalProcessTimes(Marshal::OctetsStream& os) const
{
	// process times
	DWORD dwAllSecs = 0, dwKernelSecs = 0, dwUserSecs = 0;
	GetProcessTimes( dwAllSecs, dwKernelSecs, dwUserSecs );
	os << CompactUINT(dwAllSecs) << CompactUINT(dwKernelSecs) << CompactUINT(dwUserSecs);
	return os;
}
Marshal::OctetsStream& Process::marshalModules(Marshal::OctetsStream& os) const
{
	std::vector<std::pair<std::string, int> >		aMName;
	BOOL ret = GetProcessModules( aMName );
	if( !ret )
		aMName.clear();

	std::sort( aMName.begin(), aMName.end(), std::less<std::pair<std::string, int> >() );
	std::vector<std::pair<std::string, int> >	diff_less, diff_more;

	if( aModules.empty() )
	{
		diff_more = aMName;
		aModules = aMName;
	}
	else
	{
		diff_less.resize( aMName.size()+aModules.size() );
		diff_more.resize( aMName.size()+aModules.size() );
		std::vector<std::pair<std::string, int> >::iterator itend = std::set_difference( aModules.begin(), aModules.end(), aMName.begin(), aMName.end(), diff_less.begin(), std::less<std::pair<std::string, int> >() );
		diff_less.erase( itend, diff_less.end() );
		itend = std::set_difference( aMName.begin(), aMName.end(), aModules.begin(), aModules.end(), diff_more.begin(), std::less<std::pair<std::string, int> >() );
		diff_more.erase( itend, diff_more.end() );
		aModules = aMName;
	}

	std::vector<std::pair<std::string, int> >::const_iterator it;
	os << CompactUINT(diff_less.size());
	for( it=diff_less.begin(); it!=diff_less.end(); it++)
		os << (*it).first << (*it).second;
	os << CompactUINT(diff_more.size());
	for( it=diff_more.begin(); it!=diff_more.end(); it++)
		os << (*it).first << (*it).second;
	return os;
}
Marshal::OctetsStream& Process::marshalThreadsTimes(Marshal::OctetsStream& os) const
{
	// thread times list
	std::vector<DWORD> adwTimes;
	GetThreadsTimes( adwTimes );
	os << CompactUINT(adwTimes.size());
	for( std::vector<DWORD>::const_iterator ittm=adwTimes.begin(); ittm!=adwTimes.end(); ittm++)
		os << CompactUINT(*ittm);
	return os;
}
Marshal::OctetsStream& Process::marshalProcessList(Marshal::OctetsStream& os)
{
	// process list
	std::vector<DWORD> aPID;
	std::vector<std::string> aPName;
	BOOL ret = GetProcessList( aPID, aPName );
	if( !ret )
		aPName.clear();

	std::sort( aPName.begin(), aPName.end(), std::less<std::string>() );
	std::vector<std::string>	diff_less, diff_more;

	if( aProcesses.empty() )
	{
		diff_more = aPName;
		aProcesses = aPName;
	}
	else
	{
		diff_less.resize( aPName.size()+aProcesses.size() );
		diff_more.resize( aPName.size()+aProcesses.size() );
		std::vector<std::string>::iterator itend = std::set_difference( aProcesses.begin(), aProcesses.end(), aPName.begin(), aPName.end(), diff_less.begin(), std::less<std::string>() );
		diff_less.erase( itend, diff_less.end() );
		itend = std::set_difference( aPName.begin(), aPName.end(), aProcesses.begin(), aProcesses.end(), diff_more.begin(), std::less<std::string>() );
		diff_more.erase( itend, diff_more.end() );
		aProcesses = aPName;
	}

	std::vector<std::string>::const_iterator it;
	os << CompactUINT(diff_less.size());
	for( it=diff_less.begin(); it!=diff_less.end(); it++)
		os << (*it);
	os << CompactUINT(diff_more.size());
	for( it=diff_more.begin(); it!=diff_more.end(); it++)
		os << (*it);
	return os;
}
Marshal::OctetsStream& Process::marshalWindowList(Marshal::OctetsStream& os)
{
	// window list
	std::vector<std::string> aWName;
	BOOL ret = GetWindowList( aWName );
	if( !ret )
		aWName.clear();

	std::sort( aWName.begin(), aWName.end(), std::less<std::string>() );
	std::vector<std::string>	diff_less, diff_more;

	if( aWindows.empty() )
	{
		diff_more = aWName;
		aWindows = aWName;
	}
	else
	{
		diff_less.resize( aWName.size()+aWindows.size() );
		diff_more.resize( aWName.size()+aWindows.size() );
		std::vector<std::string>::iterator itend = std::set_difference( aWindows.begin(), aWindows.end(), aWName.begin(), aWName.end(), diff_less.begin(), std::less<std::string>() );
		diff_less.erase( itend, diff_less.end() );
		itend = std::set_difference( aWName.begin(), aWName.end(), aWindows.begin(), aWindows.end(), diff_more.begin(), std::less<std::string>() );
		diff_more.erase( itend, diff_more.end() );
		aWindows = aWName;
	}

	std::vector<std::string>::const_iterator it;
	os << CompactUINT(diff_less.size());
	for( it=diff_less.begin(); it!=diff_less.end(); it++)
		os << (*it);
	os << CompactUINT(diff_more.size());
	for( it=diff_more.begin(); it!=diff_more.end(); it++)
		os << (*it);
	return os;
}

Marshal::OctetsStream& Process::marshal(Marshal::OctetsStream& os) const
{
	marshalMemory(os);
	marshalProcessTimes(os);
	marshalModules(os);
	marshalThreadsTimes(os);
	marshalProcessList(os);
	marshalWindowList(os);
	return os;
}

const Marshal::OctetsStream& Process::unmarshal(const Marshal::OctetsStream& os)
{
	// TODO
	return os;
}

Octets Process::GetOctets()
{
	Marshal::OctetsStream os;
	os << *this;
	return (Octets&)os;
}

PROCESS_MEMORY_COUNTERS Process::GetMemoryCounters() const
{
	PROCESS_MEMORY_COUNTERS pmc;
	memset( &pmc, 0, sizeof(pmc) );
	HANDLE hProcess = ::OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwPID );
	Engine::GetInstance().GetProcessMemoryInfo( hProcess, &pmc, sizeof(pmc));
	::CloseHandle( hProcess );
	return pmc;
}

DWORD * Process::GetWorkingSet( bool reload )
{
	if( NULL != pWorkingSet && !reload )
		return pWorkingSet;

	if( NULL != pWorkingSet )
	{
		free( pWorkingSet );
		pWorkingSet = NULL;
	}
	if( Engine::GetInstance().HasPSAPI() )
	{
		HANDLE hProcess = ::OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwPID );

		DWORD	cb = 4096*sizeof(DWORD);
		DWORD * pv = (DWORD*)malloc(cb);
		if( pv )	pv[0] = 0;
		int querycount = 0;
		while( pv && !Engine::GetInstance().QueryWorkingSet(hProcess,pv,cb) )
		{
			if( querycount ++ > 10 || cb >= (DWORD)(pv[0])*sizeof(DWORD) )
			{
				free(pv);
				::CloseHandle(hProcess);
				return NULL;
			}
			pv = (DWORD *)realloc(pv, cb = (DWORD)(pv[0]+1024)*sizeof(DWORD));
		}
		::CloseHandle(hProcess);

		if( pv && pv[0] < cb/sizeof(DWORD) )
		{
			pWorkingSet = pv;
			return pWorkingSet;
		}

		if( pv )	free(pv);
	}
	return NULL;
}

DWORD * Process::GetWorkingSetCompress( bool reload )
{
	int count = 0;
	DWORD * pv = GetWorkingSet( reload );

	DWORD size = pv[0];
	DWORD base_last = 0;
	DWORD property_last = 0;
	for( DWORD i=1; i<=size; i++ )
	{
		DWORD page = pv[i];
		DWORD base = (DWORD)(page & 0xFFFFFFFFFFFFF000);
		DWORD property = (DWORD)(page & 0xFFF);
		if( i>1 && property == property_last && abs((long)(base - base_last)) == 0x1000 )
		{
			count ++;
			continue;
		}
		base_last = base;
		property_last = property;
	}

	return pv;
}

std::vector<std::string> Process::GetWorkingSetString( bool reload )
{
	std::vector<std::string> saPages;

	DWORD * pv = GetWorkingSet( reload );
	if( NULL == pv )
		return saPages;

	char buffer[1024];
	DWORD	size = pv[0];
	for( DWORD i=1; i<=size; i++ )
	{
		DWORD page = pv[i];
		DWORD base = (DWORD)(page & 0xFFFFFFFFFFFFF000);
		BOOL isShared = (BOOL)((long)(page & 0x100) >> 8);
		long shareCount = ((long)(page & 0x0E0) >> 5);
		long attr = (long)(page & 0x01F);

		_snprintf( buffer, sizeof(buffer), "%llp\t%d\t%d\t", base, isShared, shareCount );
		std::string info = buffer;
		switch( attr )
		{
		case 0:	info += "0 The page is not accessed. ";	break;
		case 1:	info += "1 The page is read-only. ";	break;
		case 2:	info += "2 The page is executable. ";	break;
		case 3:	info += "3 The page is executable and read-only. ";	break;
		case 4:	info += "4 The page has read/write access. ";	break;
		case 5:	info += "5 The page is copy-on-write. ";	break;
		case 6:	info += "6 The page is executable and has read/write access. ";	break;
		case 7:	info += "7 The page is executable and has copy-on-write access. ";	break;
		case 8:	info += "8 The page is not accessed. ";	break;
		case 9:	info += "9 The page is not cached and has read-only access. ";	break;
		case 10:	info += "10 The page is not cached and is executable. ";	break;
		case 11:	info += "11 The page is not cached, is executable, and has read-only access. ";	break;
		case 12:	info += "12 The page is not cached and has read/write access. ";	break;
		case 13:	info += "13 The page is not cached and has copy-on-write access. ";	break;
		case 14:	info += "14 The page is not cached, is executable, and has read/write access. ";	break;
		case 15:	info += "15 The page is not cached, is executable, and has copy-on-write access. ";	break;
		case 16:	info += "16 The page is not accessed. ";	break;
		case 17:	info += "17 The page is a guard page and has read-only access. ";	break;
		case 18:	info += "18 The page is a guard page and is executable. ";	break;
		case 19:	info += "19 The page is a guard page, is executable, and has read-only access. ";	break;
		case 20:	info += "20 The page is a guard page and has read/write access. ";	break;
		case 21:	info += "21 The page is a guard page and has copy-on-write access. ";	break;
		case 22:	info += "22 The page is a guard page, is executable, and has read/write access. ";	break;
		case 23:	info += "23 The page is a guard page, is executable, and has copy-on-write access. ";	break;
		case 24:	info += "24 The page is not accessed. ";	break;
		case 25:	info += "25 The page is a non-cacheable guard page and has read-only access. ";	break;
		case 26:	info += "26 The page is a non-cacheable guard page and has executable access. ";	break;
		case 27:	info += "27 The page is a non-cacheable guard page, is executable, and has read-only access. ";	break;
		case 28:	info += "28 The page is a non-cacheable guard page and has read/write access. ";	break;
		case 29:	info += "29 The page is a non-cacheable guard page and has copy-on-write access. ";	break;
		case 30:	info += "30 The page is a non-cacheable guard page, is executable, and has read/write access. ";	break;
		case 31:	info += "31 The page is a non-cacheable guard page, is executable, and has copy-on-write access. ";	break;
		default:	info += "error";
		}
		saPages.insert( saPages.end(), info );
	}
	return saPages;
}

Process::MemInfoArray & Process::GetMemInfo( bool reload )
{
	if( !aMemInfo.empty() && !reload )
		return aMemInfo;
	aMemInfo.clear();

	DWORD * pv = GetWorkingSet( reload );
	if( NULL == pv )
		return aMemInfo;

	HANDLE hProcess = ::OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwPID );
	DWORD size = pv[0];
	for( DWORD i=1; i<=size; i++ )
	{
		DWORD page = pv[i];
		DWORD base = (DWORD)(page & 0xFFFFFFFFFFFFF000);
		DWORD property = (DWORD)(page & 0xFFF);

		MEMORY_BASIC_INFORMATION mbi;
		memset( &mbi, 0, sizeof(mbi) );
		size_t ret = ::VirtualQueryEx( hProcess, (LPCVOID)base, &mbi, sizeof(mbi) );
		if( ret > 0 )
			aMemInfo.insert( aMemInfo.end(), mbi );
	}
	::CloseHandle(hProcess);
	return aMemInfo;
}

BOOL Process::GetProcessModules( std::vector<std::pair<std::string, int> > & aModules ) const
{
	if( Engine::GetInstance().HasToolHelp32() )
	{
		// Get a handle to a Toolhelp snapshot of all processes.
		HANDLE hSnapShot = Engine::GetInstance().CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);
		if (hSnapShot == INVALID_HANDLE_VALUE) {
			return FALSE;
		}

		// Get the first process' information.
		MODULEENTRY32 modentry;
		modentry.dwSize = sizeof(MODULEENTRY32);
		BOOL bFlag = Engine::GetInstance().Module32First(hSnapShot, &modentry);

		// While there are processes, keep looping.
		while (bFlag) {
			aModules.push_back( std::pair<std::string, int>(modentry.szModule, modentry.modBaseSize) );
			modentry.dwSize = sizeof(MODULEENTRY32);
			bFlag = Engine::GetInstance().Module32Next(hSnapShot, &modentry);
		};
		if( hSnapShot) ::CloseHandle(hSnapShot);
		return TRUE;
	}
	else if( Engine::GetInstance().HasPSAPI() )
	{
		HANDLE hProcess = ::OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwPID );

		DWORD cbSupply = 4096, cbNeeded;
		HMODULE *hMod = (HMODULE *)malloc(cbSupply);
		if ( Engine::GetInstance().EnumProcessModules ( hProcess, hMod, cbSupply, &cbNeeded) )
		{
			while ( cbSupply < cbNeeded )
			{
				hMod = (HMODULE *)realloc(hMod, cbSupply = cbNeeded);
				Engine::GetInstance().EnumProcessModules ( hProcess, hMod, cbSupply, &cbNeeded);
			}
		}

		size_t size = cbNeeded / sizeof(HMODULE);
		for ( DWORD i = 0; i < size; i++ )
		{
			char lpBaseName[MAX_PATH+1];
			lpBaseName[0] = 0;
			Engine::GetInstance().GetModuleBaseName(hProcess, hMod[i], lpBaseName, sizeof(lpBaseName));
			aModules.push_back( std::pair<std::string, int>(lpBaseName, 0) );
		}
		free(hMod);
		::CloseHandle(hProcess);
		return TRUE;
	}
	return FALSE;
}
BOOL Process::sGetProcessModules( std::vector<std::string> & aModules,DWORD dwPID)
{
	if( Engine::GetInstance().HasToolHelp32() )
	{
		// Get a handle to a Toolhelp snapshot of all processes.
		HANDLE hSnapShot = Engine::GetInstance().CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);
		if (hSnapShot == INVALID_HANDLE_VALUE) {
			return FALSE;
		}

		// Get the first process' information.
		MODULEENTRY32 modentry;
		modentry.dwSize = sizeof(MODULEENTRY32);
		BOOL bFlag = Engine::GetInstance().Module32First(hSnapShot, &modentry);

		// While there are processes, keep looping.
		while (bFlag) {
			aModules.push_back( modentry.szModule );
			modentry.dwSize = sizeof(MODULEENTRY32);
			bFlag = Engine::GetInstance().Module32Next(hSnapShot, &modentry);
		};
		if( hSnapShot ) ::CloseHandle(hSnapShot);
		return TRUE;
	}
	else if( Engine::GetInstance().HasPSAPI() )
	{
		HANDLE hProcess = ::OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwPID );

		DWORD cbSupply = 4096, cbNeeded;
		HMODULE *hMod = (HMODULE *)malloc(cbSupply);
		if ( Engine::GetInstance().EnumProcessModules ( hProcess, hMod, cbSupply, &cbNeeded) )
		{
			while ( cbSupply < cbNeeded )
			{
				hMod = (HMODULE *)realloc(hMod, cbSupply = cbNeeded);
				Engine::GetInstance().EnumProcessModules ( hProcess, hMod, cbSupply, &cbNeeded);
			}
		}

		size_t size = cbNeeded / sizeof(HMODULE);
		for ( DWORD i = 0; i < size; i++ )
		{
			char lpBaseName[MAX_PATH+1];
			lpBaseName[0] = 0;
			Engine::GetInstance().GetModuleBaseName(hProcess, hMod[i], lpBaseName, sizeof(lpBaseName));
			aModules.push_back( lpBaseName );
		}
		free(hMod);
		::CloseHandle(hProcess);
		return TRUE;
	}
	return FALSE;
}

BOOL Process::GetProcessTimes( DWORD &dwAllSecs, DWORD &dwKernelSecs, DWORD &dwUserSecs ) const
{
	FILETIME	creationTime, exitTime, kernelTime, userTime;
	HANDLE hProcess = ::OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwPID );
	BOOL ret = Engine::GetInstance().GetProcessTimes( hProcess, &creationTime, &exitTime, &kernelTime, &userTime ); 
	::CloseHandle(hProcess);
	if( ret )
	{
		ULARGE_INTEGER	creation, exit, kernel, user;
		memcpy( &creation, &creationTime, sizeof(creation) );
		memcpy( &exit, &exitTime, sizeof(exit) );
		memcpy( &kernel, &kernelTime, sizeof(kernel) );
		memcpy( &user, &userTime, sizeof(user) );
		if( 0 == exit.QuadPart )
		{
			SYSTEMTIME st;
			GetSystemTime(&st);
			SystemTimeToFileTime(&st, &exitTime);
			memcpy( &exit, &exitTime, sizeof(exit) );
		}

		dwAllSecs = (DWORD)( (exit.QuadPart - creation.QuadPart) / 10000000 );
		dwKernelSecs = DWORD(kernel.QuadPart / 10000000);
		dwUserSecs = DWORD(user.QuadPart / 10000000);
	}
	return ret;
}

BOOL Process::GetThreads( std::vector<DWORD> & aTID ) const
{
	HANDLE hThreadSnap = Engine::GetInstance().CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, dwPID);
	if (hThreadSnap == INVALID_HANDLE_VALUE)
		return FALSE;

	THREADENTRY32	te32	= {0};
	te32.dwSize = sizeof(THREADENTRY32);
	if (Engine::GetInstance().Thread32First(hThreadSnap, &te32))
	{
		do
		{
			aTID.insert( aTID.end(), te32.th32ThreadID );
		}
		while (Engine::GetInstance().Thread32Next(hThreadSnap, &te32));
	}
 
	::CloseHandle (hThreadSnap); 
	return TRUE;
}

BOOL Process::GetThreadsTimes( std::vector<DWORD> & adwTimes ) const
{
	if( 0 == dwPID )
		return FALSE;

	HANDLE hThreadSnap = Engine::GetInstance().CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, dwPID);
	if (INVALID_HANDLE_VALUE == hThreadSnap)
		return FALSE;

	THREADENTRY32	te32	= {0};
	te32.dwSize = sizeof(THREADENTRY32);
	if (Engine::GetInstance().Thread32First(hThreadSnap, &te32))
	{
		do
		{
			if( 0 != te32.th32ThreadID && te32.th32OwnerProcessID == dwPID )
			{
				HANDLE hThread = Engine::GetInstance().OpenThread( THREAD_QUERY_INFORMATION, FALSE, te32.th32ThreadID );
				DWORD dwAllSecs = 0, dwKernelSecs = 0, dwUserSecs = 0;
				BOOL b = Process::GetThreadTimes( hThread, dwAllSecs, dwKernelSecs, dwUserSecs );
				if(hThread)	::CloseHandle( hThread );

				if( b )
				{
					adwTimes.insert( adwTimes.end(), te32.th32ThreadID );
					adwTimes.insert( adwTimes.end(), dwAllSecs );
					adwTimes.insert( adwTimes.end(), dwKernelSecs );
					adwTimes.insert( adwTimes.end(), dwUserSecs );
				}
			}
		}
		while (Engine::GetInstance().Thread32Next(hThreadSnap, &te32));
	}

	::CloseHandle (hThreadSnap);
	return TRUE;
}

typedef struct {
	std::vector<DWORD>			* paPID;
	std::vector<std::string>	* paPName;
} MyEnumInfoStruct;

BOOL CALLBACK MyProcessEnumerator(DWORD dwPID, WORD wTask,
								  LPCSTR szProcess, LPARAM lParam)
{
	MyEnumInfoStruct * pInfo = (MyEnumInfoStruct *)lParam;
	if( pInfo )
	{
		if (wTask == 0)
			pInfo->paPID->insert( pInfo->paPID->end(), dwPID );
		else
			pInfo->paPID->insert( pInfo->paPID->end(), wTask );
		pInfo->paPName->insert( pInfo->paPName->end(), szProcess );
	}
	return TRUE;
}

BOOL Process::GetProcessList( std::vector<DWORD> & aPID, std::vector<std::string> & aPName )
{
	MyEnumInfoStruct	sInfo;
	sInfo.paPID = &aPID;
	sInfo.paPName = &aPName;
	return EnumProcs((PROCENUMPROC) MyProcessEnumerator, (LPARAM)&sInfo);
}

BOOL CALLBACK MyWindowEnumerator( HWND hWnd, LPARAM lParam )
{
	std::vector<std::string>	* paWName = (std::vector<std::string> *)lParam;
	if( paWName )
	{
		char wndname[MAX_PATH], clsname[MAX_PATH];
		std::string	string = "<";
		if( ::GetClassNameA(hWnd, clsname, sizeof(clsname) ) )	string += clsname;
		string += ">:<";
		if( ::GetWindowTextA(hWnd, wndname, sizeof(wndname) ) )	string	+=	wndname;
		string += ">";
		if( NULL != hWnd && string.length() > 5 
			&& paWName->end() == std::find(paWName->begin(),paWName->end(),string) )
			paWName->insert( paWName->end(), string );
	}
	return TRUE;
}	

BOOL Process::GetWindowList( std::vector<std::string> & aWName )
{
	return EnumWindows( (WNDENUMPROC) MyWindowEnumerator, (LPARAM)&aWName );
}

BOOL Process::GetThreadTimes( HANDLE hThread, DWORD &dwAllSecs, DWORD &dwKernelSecs, DWORD &dwUserSecs )
{
	if( NULL == hThread )
		return FALSE;

	FILETIME	creationTime, exitTime, kernelTime, userTime;
	BOOL ret = Engine::GetInstance().GetThreadTimes( hThread, &creationTime, &exitTime, &kernelTime, &userTime ); 
	if( ret )
	{
		ULARGE_INTEGER	creation, exit, kernel, user;
		memcpy( &creation, &creationTime, sizeof(creation) );
		memcpy( &exit, &exitTime, sizeof(exit) );
		memcpy( &kernel, &kernelTime, sizeof(kernel) );
		memcpy( &user, &userTime, sizeof(user) );
		if( 0 == exit.QuadPart )
		{
			SYSTEMTIME st;
			GetSystemTime(&st);
			SystemTimeToFileTime(&st, &exitTime);
			memcpy( &exit, &exitTime, sizeof(exit) );
		}

		dwAllSecs = (DWORD)( (exit.QuadPart - creation.QuadPart) / 10000000 );
		dwKernelSecs = DWORD(kernel.QuadPart / 10000000);
		dwUserSecs = DWORD(user.QuadPart / 10000000);
	}
	return ret;
}
