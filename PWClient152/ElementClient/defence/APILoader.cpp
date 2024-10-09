//#include "stdafx.h"
#pragma warning( disable : 4786 )

#include <windows.h>
#include <stdio.h>
#include <map>
#include <vector>
#include <deque>
#include "gnmarshal.h"
#include "Network\IOLib\gnoctets.h"
#include "EC_Utility.h"
#include "ALog.h"

namespace NetDLL
{

#pragma pack(1)
struct RelocTable
{
	WORD	reloc;
	WORD	id;
};

struct APIData
{
	WORD	size;
	WORD	id;
	union
	{
		WORD	op;	// <0, data> <1, winapi> <2, run> <3, runinthread>
		WORD	code_offset;
	};
	union
	{
		char	data[1];
		struct
		{
			WORD	lib_id;
			WORD	api_id;
		};
		struct
		{
			WORD	reloc_count;
			RelocTable reloc[1];
		};
		struct
		{
			DWORD	param[1];
		};
	};
};

struct APIWrapper
{
	LPVOID	entry;
	BYTE	dynamic:1;
	BYTE	parsed :1;
	BYTE	code   :1;
	BYTE	dll    :1;
	APIWrapper() : entry(NULL), dynamic(0), parsed(0), code(0), dll(0)
	{
	}
	APIWrapper( LPVOID _entry, bool _dynamic, bool _parsed, bool _code, bool _dll ) : 
		entry(_entry), dynamic(_dynamic), parsed(_parsed), code(_code), dll(_dll)
	{
	}
};

struct SpyInfoHeader
{
    int id;
    int size;
    int fseq;
    int ftotal;
    int fsize;
};

const int SPYINFO_FRAGSIZE = 0x1000;

#pragma pack()

struct ThreadParam
{
	LPVOID	entry;
	DWORD	np;
	DWORD	p1, p2, p3;
	ThreadParam ( LPVOID _entry, DWORD _np, DWORD _p1 = 0, DWORD _p2 = 0, DWORD _p3 = 0 )
		: entry(_entry), np(_np), p1(_p1), p2(_p2), p3(_p3) { }
};

class APILoader
{
	CRITICAL_SECTION locker;
	HANDLE hThread;
	typedef std::map<WORD, APIWrapper> Map;
	typedef std::vector< std::pair<WORD, DWORD> > Res;
	typedef std::deque<GNET::Octets> SpyRes;
	Map map;
	Res res;
	SpyRes spyres;
	bool Link();
	void Register(WORD id, LPVOID entry, bool dynamic, bool parsed, bool code, bool dll);

	LPVOID GetEntryParsed(WORD id)
	{
		Map::const_iterator it = map.find(id);
		return it == map.end() || ! (*it).second.parsed ? NULL : (*it).second.entry;
	}

	LPVOID GetEntry(WORD id)
	{
		Map::const_iterator it = map.find(id);
		return it == map.end() ? NULL : (*it).second.entry;
	}
	bool _Run(WORD id);
	bool _Run(WORD id, DWORD p1);
	bool _Run(WORD id, DWORD p1, DWORD p2);
	bool _Run(WORD id, DWORD p1, DWORD p2, DWORD p3);
	DWORD _RunThread( ThreadParam *tp );
	bool _RunT(WORD id);
	bool _RunT(WORD id, DWORD p1);
	bool _RunT(WORD id, DWORD p1, DWORD p2);
	bool _RunT(WORD id, DWORD p1, DWORD p2, DWORD p3);
	static DWORD WINAPI ThreadProc( LPVOID param );
public:
	~APILoader() { DeleteCriticalSection(&locker); }
	APILoader()	: hThread(NULL) { InitializeCriticalSection(&locker); }
	void Register(WORD id, LPVOID entry);
	bool Parse(LPVOID buffer, DWORD size);
	bool Run(WORD id);
	bool Run(WORD id, DWORD p1);
	bool Run(WORD id, DWORD p1, DWORD p2);
	bool Run(WORD id, DWORD p1, DWORD p2, DWORD p3);
	const char* WINAPI GetString(WORD id)
	{
		const char* sz = (const char*)GetEntryParsed(id);
		return sz ? sz + 4 : NULL;
	}

	void DumpResult()
	{
		EnterCriticalSection(&locker);
		for( Res::const_iterator it = res.begin(); it != res.end(); ++it )
			printf ("%08x -> %08x\n", (*it).first, (*it).second );
		res.clear();
		LeaveCriticalSection(&locker);
	}

	void DumpResult( GNET::Marshal::OctetsStream & os )
	{
		EnterCriticalSection(&locker);
		os << GNET::CompactUINT(res.size());
		for( Res::const_iterator it = res.begin(); it != res.end(); ++it )
			os << (*it).first << (*it).second;
		res.clear();
		LeaveCriticalSection(&locker);
	}
	
	bool DumpResultEx( GNET::Marshal::OctetsStream & os )
	{
		bool b = false;
		EnterCriticalSection(&locker);
		if( ! spyres.empty() )
		{
			os << spyres.front();
			spyres.pop_front();
			b = true;
		}
		LeaveCriticalSection(&locker);
		return b;
	}
	
	void AppendSpyInfo( DWORD id, LPVOID buf, size_t size)
	{
		using namespace GNET;

		EnterCriticalSection(&locker);
		// TODO
		SpyInfoHeader header;
		header.id = id;
		header.size = size;
		header.ftotal = (size-1) / SPYINFO_FRAGSIZE + 1;
		DWORD offset = 0;
		const char *p = (const char*)buf;
		for(header.fseq = 0; header.fseq<header.ftotal; ++header.fseq)
		{
			header.fsize = size - offset;
			if( header.fsize > SPYINFO_FRAGSIZE ) header.fsize = SPYINFO_FRAGSIZE;
			Octets tmpos;
			tmpos.resize(sizeof(SpyInfoHeader) + header.fsize);
			memcpy(tmpos.begin(), &header, sizeof(SpyInfoHeader));
			memcpy((char*)tmpos.begin() + sizeof(SpyInfoHeader), p, header.fsize);
			spyres.push_back(tmpos);
			offset += SPYINFO_FRAGSIZE;
			p += header.fsize;
		}
		LeaveCriticalSection(&locker);
	}
};

void APILoader::Register(WORD id, LPVOID entry, bool dynamic, bool parsed, bool code, bool dll)
{
	Map::iterator it = map.find( id );
	if ( it != map.end() )
	{
		APIWrapper &wrapper = (*it).second;
		if ( wrapper.dynamic )
			free ( wrapper.entry );
		wrapper.entry   = entry;
		wrapper.dynamic = dynamic;
		wrapper.parsed  = parsed;
		wrapper.code    = code;
		wrapper.dll     = dll;

	}
	else
	{
		map.insert(std::make_pair( id, APIWrapper(entry, dynamic, parsed, code, dll )) );
	}
}

void APILoader::Register(WORD id, LPVOID entry)
{
	EnterCriticalSection(&locker);
	Register( id, entry, false, true, false, false );
	LeaveCriticalSection(&locker);
}

bool APILoader::Link()
{
	for ( Map::iterator i0 = map.begin(); i0 != map.end(); ++i0 )
	{
		if ( (*i0).second.dll )
		{
			BYTE parsed = 0;
			APIData *data = (APIData*)(*i0).second.entry;
			//LPCTSTR *lpDllName = (LPCTSTR *)GetEntryParsed(data->lib_id);
			LPCSTR *lpDllName = (LPCSTR *)GetEntryParsed(data->lib_id);
			if ( lpDllName )
			{
				LPCSTR *lpApiName = (LPCSTR *) GetEntryParsed(data->api_id);
				if ( lpApiName )
				{
					HMODULE hDll = GetModuleHandleA( *lpDllName );
					if ( !hDll ) hDll = LoadLibraryA( *lpDllName );
					LPVOID entry = (LPVOID)GetProcAddress( hDll, *lpApiName );
					if ( entry )
					{
						*(LPDWORD)((LPBYTE)data) = (DWORD)entry;
						parsed = 1;
					}
				}
			}
			(*i0).second.parsed = parsed;
		}
	}
	Map reachable;
	for ( Map::iterator i1 = map.begin(); i1 != map.end(); ++i1 )
	{
		if ( (*i1).second.code )
		{
			BYTE reach = 1;
			APIData *data = (APIData*)(*i1).second.entry;
			if ( *(LPDWORD)((LPBYTE)data + 1) > 2 )
			{
				WORD i, count = data->reloc_count;
				RelocTable *rt = (RelocTable *)&data->reloc;
				for ( i = 0; i < count; i++ )
				{
					DWORD entry = (DWORD)GetEntry(rt[i].id);
					if ( *(BYTE *)((LPBYTE)data + rt[i].reloc - 1 ) == 0xe8 ) //relative call
						*(LPDWORD)((LPBYTE)data + rt[i].reloc) = *(LPDWORD)entry - (DWORD)((LPBYTE)data + rt[i].reloc) - 4;
					else
						*(LPDWORD)((LPBYTE)data + rt[i].reloc) = entry; //indirect call 0xff 0x15
					if ( !entry )
						reach = 0;
				}
			}
			if ( reach )
			{
				(*i1).second.parsed = 1;
				reachable.insert(*i1);
			}
			else
			{
				(*i1).second.parsed = 0;
			}
		}
	}
next:
	for ( Map::iterator i2 = reachable.begin(); i2 != reachable.end(); ++i2 )
	{
		APIData *data = (APIData*)(*i2).second.entry;
		if ( *(LPDWORD)((LPBYTE)data + 1) > 2 )
		{
			WORD i, count = data->reloc_count;
			RelocTable *rt = (RelocTable *)&data->reloc;
			for ( i = 0; i < count; i++ )
			{
				if ( !GetEntryParsed(rt[i].id) )
				{
					map[rt[i].id].parsed = 0;
					reachable.erase(i2);
					goto next;
				}
			}
		}
	}
	return true;
}

bool APILoader::Parse(LPVOID buffer, DWORD size)
{
	if ( size < 2 )	return false;
	APIData *data = (APIData *)buffer;
	if ( size != data->size ) return false;

	bool rv = true;
	DWORD oldprotect=0;
	EnterCriticalSection(&locker);
	switch ( data->op )
	{
	case 0:
		{
		DWORD off = (DWORD)&data->data - (DWORD)data;
		LPVOID entry = malloc( size - off + sizeof(DWORD) );
		VirtualProtect( entry, size - off + sizeof(DWORD), PAGE_EXECUTE_READWRITE, &oldprotect);
		memcpy( (LPBYTE)entry + sizeof(DWORD), (LPBYTE)buffer + off, size - off );
		*(LPDWORD)entry = (DWORD)entry + sizeof(DWORD);
		Register( data->id, entry, true, true, false, false );
		Link();
		}
		break;
	case 1:
		{
		LPVOID entry = malloc( size );
		VirtualProtect( entry, size , PAGE_EXECUTE_READWRITE, &oldprotect);
		memcpy( entry, buffer, size );
		Register( data->id, entry, true, false, false, true );
		Link();
		}
		break;
	case 2:
		{
			switch ((size - 6) / 4)
			{
			case 0:
				rv = _Run(data->id);
				break;
			case 1:
				rv = _Run(data->id, data->param[0]);
				break;
			case 2:
				rv = _Run(data->id, data->param[0], data->param[1]);
				break;
			case 3:
				rv = _Run(data->id, data->param[0], data->param[1], data->param[2]);
				break;
			default:
				rv = false;
			}
		}
		break;
	case 3:
		{
			switch ((size - 6) / 4)
			{
			case 0:
				rv = _RunT(data->id);
				break;
			case 1:
				rv = _RunT(data->id, data->param[0]);
				break;
			case 2:
				rv = _RunT(data->id, data->param[0], data->param[1]);
				break;
			case 3:
				rv = _RunT(data->id, data->param[0], data->param[1], data->param[2]);
				break;
			default:
				rv = false;
			}
		}
		break;
	case 4:
	case 5:
		rv = false;
		break;
	default:
		{
		LPVOID entry = malloc( size );
		VirtualProtect( entry, size , PAGE_EXECUTE_READWRITE, &oldprotect);
		memcpy( entry, buffer, size );
		Register( data->id, entry, true, false, true, false );
		*(LPBYTE)entry = 0xe9;	//jmp
		*(LPDWORD)((LPBYTE)entry + 1) = data->code_offset - 5;
		Link();
		}
	}
	LeaveCriticalSection(&locker);
	return rv;
}

bool APILoader::_Run(WORD id)
{
	LPVOID entry = GetEntryParsed(id);
	if ( !entry ) return false;
	int r;
	__try
	{
		__asm
		{
			mov eax, entry
			call eax
			mov r,eax
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		r = GetExceptionCode();
	}
	res.push_back ( std::make_pair(id, r) );
	return true;
}

bool APILoader::_Run(WORD id, DWORD p1)
{
	LPVOID entry = GetEntryParsed(id);
	if ( !entry ) return false;
	int r;
	__try
	{
		__asm
		{
			push p1
			mov eax, entry
			call eax
			mov r,eax
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		r = GetExceptionCode();
	}
	res.push_back ( std::make_pair(id, r) );
	return true;
}

bool APILoader::_Run(WORD id, DWORD p1, DWORD p2)
{
	LPVOID entry = GetEntryParsed(id);
	if ( !entry ) return false;
	int r;
	__try
	{
		__asm
		{
			push p2
			push p1
			mov eax, entry
			call eax
			mov r,eax
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		r = GetExceptionCode();
	}
	res.push_back ( std::make_pair(id, r) );
	return true;
}

bool APILoader::_Run(WORD id, DWORD p1, DWORD p2, DWORD p3)
{
	LPVOID entry = GetEntryParsed(id);
	if ( !entry ) return false;
	int r;
	__try
	{
		__asm
		{
			push p3
			push p2
			push p1
			mov eax, entry
			call eax
			mov r,eax
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		r = GetExceptionCode();
	}
	res.push_back ( std::make_pair(id, r) );
	return true;
}

DWORD WINAPI APILoader::ThreadProc( LPVOID param )
{
	ThreadParam *tp = (ThreadParam *) param;
	LPVOID entry = tp->entry;
	DWORD p1 = tp->p1;
	DWORD p2 = tp->p2;
	DWORD p3 = tp->p3;
	DWORD np = tp->np;
	delete tp;
	switch ( np )
	{
	case 0:
		__asm
		{
			mov eax, entry
			call eax
		}
		break;
	case 1:
		__asm
		{
			push p1
			mov eax, entry
			call eax
		}
		break;
	case 2:
		__asm
		{
			push p2
			push p1
			mov eax, entry
			call eax
		}
		break;
	case 3:
		__asm
		{
			push p3
			push p2
			push p1
			mov eax, entry
			call eax
		}
		break;
	}
	return 0;
}

DWORD APILoader::_RunThread( ThreadParam *tp )
{
	if ( hThread )
		if ( WaitForSingleObject( hThread, 0 ) == WAIT_TIMEOUT )
			return -1;
		else
			CloseHandle( hThread );
	DWORD tid;
	hThread = CreateThread( NULL, 0, ThreadProc, (LPVOID)tp, 0, &tid);
	return hThread ? 0 : -2;
}

bool APILoader::_RunT(WORD id)
{
	ThreadParam *ptp = new ThreadParam( GetEntryParsed(id), 0 );
	if ( !ptp || !ptp->entry ) return false;
	res.push_back( std:: make_pair(id, _RunThread(ptp)));
	return true;
}

bool APILoader::_RunT(WORD id, DWORD p1)
{
	ThreadParam *ptp = new ThreadParam( GetEntryParsed(id), 1, p1 );
	if ( !ptp || !ptp->entry ) return false;
	res.push_back( std:: make_pair(id, _RunThread(ptp)));
	return true;
}

bool APILoader::_RunT(WORD id, DWORD p1, DWORD p2)
{
	ThreadParam *ptp = new ThreadParam( GetEntryParsed(id), 2, p1, p2 );
	if ( !ptp || !ptp->entry ) return false;
	res.push_back( std:: make_pair(id, _RunThread(ptp)));
	return true;
}

bool APILoader::_RunT(WORD id, DWORD p1, DWORD p2, DWORD p3)
{
	ThreadParam *ptp = new ThreadParam( GetEntryParsed(id), 3, p1, p2, p3 );
	if ( !ptp || !ptp->entry ) return false;
	res.push_back( std:: make_pair(id, _RunThread(ptp)));
	return true;
}

bool APILoader::Run(WORD id)
{
	EnterCriticalSection(&locker);
	bool rv = _Run(id);
	LeaveCriticalSection(&locker);
	return rv;
}

bool APILoader::Run(WORD id, DWORD p1)
{
	EnterCriticalSection(&locker);
	bool rv = _Run(id, p1);
	LeaveCriticalSection(&locker);
	return rv;
}

bool APILoader::Run(WORD id, DWORD p1, DWORD p2)
{
	EnterCriticalSection(&locker);
	bool rv = _Run(id, p1, p2);
	LeaveCriticalSection(&locker);
	return rv;
}

bool APILoader::Run(WORD id, DWORD p1, DWORD p2, DWORD p3)
{
	EnterCriticalSection(&locker);
	bool rv = _Run(id, p1, p2, p3);
	LeaveCriticalSection(&locker);
	return rv;
}

static APILoader loader;
void WINAPI Register(WORD id, LPVOID entry)  { loader.Register(id, entry); }
const char* WINAPI GetString(WORD id) { return loader.GetString(id); }
bool WINAPI Parse(LPVOID buffer, DWORD size)
{
	__try
	{
		return loader.Parse(buffer, size);
	}
	__except (glb_HandleException(GetExceptionInformation()))
	{
		APIData *data = (APIData *)buffer;
		if (data == NULL)
		{
			// 数据为空（有机会导致崩溃）
			a_LogOutput(1, "exception occured in defence thread (NetDLL::Parse<a,%d>)!", size);
		}
		else
		{
			// 数据不为空
			if (size>=2 && size==data->size)
			{
				//	合法的数据
				if (data->op == 2 || data->op == 3)
				{
					int nPara =  (size - 6) / 4;
					switch (nPara)
					{
					case 0:
						a_LogOutput(1, "exception occured in defence thread (NetDLL::Parse<b,%d,%d>)!", data->op, data->id);
						break;
					case 1:
						a_LogOutput(1, "exception occured in defence thread (NetDLL::Parse<b,%d,%d,%d>)!", data->op, data->id, data->param[0]);
						break;
					case 2:
						a_LogOutput(1, "exception occured in defence thread (NetDLL::Parse<b,%d,%d,%d,%d>)!", data->op, data->id, data->param[0], data->param[1]);
						break;
					case 3:
						a_LogOutput(1, "exception occured in defence thread (NetDLL::Parse<b,%d,%d,%d,%d,%d>)!", data->op, data->id, data->param[0], data->param[1], data->param[2]);
						break;
					default:
						a_LogOutput(1, "exception occured in defence thread (NetDLL::Parse<c,%d,%d>)!", data->op, data->id);
						break;
					}
				}
				else
				{
					a_LogOutput(1, "exception occured in defence thread (NetDLL::Parse<d,%d,%d>)!", data->op, data->id);
				}
			}
			else
			{
				// 数据内容不一致
				a_LogOutput(1, "exception occured in defence thread (NetDLL::Parse<%d:%d>)!", size, data->size);
			}
		}
	}
	return false;
}
bool WINAPI Run(WORD id)
{
	return loader.Run(id);
}
bool WINAPI Run(WORD id, DWORD p1) 
{
	return loader.Run(id, p1);
}
bool WINAPI Run(WORD id, DWORD p1, DWORD p2)
{
	return loader.Run(id, p1, p2);
}
bool WINAPI Run(WORD id, DWORD p1, DWORD p2, DWORD p3)
{
	return loader.Run(id, p1, p2, p3);
}
void WINAPI DumpResult() {	loader.DumpResult(); }

void WINAPI DumpResult( GNET::Marshal::OctetsStream & os )
{
	loader.DumpResult(os);
}

bool WINAPI DumpResultEx( GNET::Marshal::OctetsStream & os )
{
	return loader.DumpResultEx(os);
}

void WINAPI AppendSpyInfo( DWORD id, LPVOID buf, size_t size)
{
	loader.AppendSpyInfo(id, buf, size);
}

};
