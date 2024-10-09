#pragma once

#pragma warning( disable : 4786 )

#include <windows.h>
#include <tchar.h>
#include <winsvc.h>
#include <vector>
#include <string>

#include "infocollection.h"

class Process : public InfoCollection
{
public:
	typedef std::vector<Process *> Array;
	typedef std::vector<MEMORY_BASIC_INFORMATION>	MemInfoArray;

public:
	Process(DWORD _dwPID);
	Process(LPCSTR lpFileTitle);
	Process(LPCSTR lpClassName, LPCSTR lpWindowName);
	virtual ~Process(void);
	void Destroy(void)
	{
		delete this;
	}

	OctetsStream& marshalMemory(OctetsStream& os) const;
	OctetsStream& marshalProcessTimes(OctetsStream& os) const;
	OctetsStream& marshalModules(OctetsStream& os) const;
	OctetsStream& marshalThreadsTimes(OctetsStream& os) const;
	static OctetsStream& marshalProcessList(OctetsStream& os);
	static OctetsStream& marshalWindowList(OctetsStream& os);
	OctetsStream& marshal(OctetsStream& os) const;
	const OctetsStream& unmarshal(const OctetsStream& os);
	Octets GetOctets();

	DWORD GetPID() const { return dwPID; }
	PROCESS_MEMORY_COUNTERS GetMemoryCounters() const;
	DWORD * GetWorkingSet( bool reload = false );
	DWORD * GetWorkingSetCompress( bool reload = false );
	std::vector<std::string> GetWorkingSetString( bool reload = false );
	MemInfoArray & GetMemInfo( bool reload = false );
	BOOL GetProcessModules( std::vector<std::pair<std::string, int> > & aModules ) const;
	static BOOL sGetProcessModules( std::vector<std::string> & aModules ,DWORD pid);
	BOOL GetProcessTimes( DWORD &dwAllSecs, DWORD &dwKernelSecs, DWORD &dwUserSecs ) const;
	BOOL GetThreads( std::vector<DWORD> & aTID ) const;
	BOOL GetThreadsTimes( std::vector<DWORD> & adwTimes ) const;

	static BOOL GetProcessList( std::vector<DWORD> & aPID, std::vector<std::string> & aPName );
	static BOOL GetWindowList( std::vector<std::string> & aWName );
	static BOOL GetThreadTimes( HANDLE hThread, DWORD &dwAllSecs, DWORD &dwKernelSecs, DWORD &dwUserSecs );
private:
	DWORD dwPID;
	DWORD	* pWorkingSet;
	MemInfoArray	aMemInfo;
	static	std::vector<std::pair<std::string, int> >	aModules;
	static	std::vector<std::string>	aProcesses;
	static	std::vector<std::string>	aWindows;
};
