/********************************************************************
	created:	2012/8/30
	author:		dongjie
	
	purpose:    利用detours库统计客户端的当前内存和历史内存记录并输出
	Copyright (C) All Rights Reserved
*********************************************************************/
#ifdef _PROFILE_MEMORY

#pragma warning (disable : 4786)

#ifndef _WIN32_WINNT
#define _WIN32_WINNT  0x0500
#endif

#include <windows.h>
#include <assert.h>
#include <set>
#include <fstream.h>
#define _NO_CVCONST_H
#include <dbghelp.h>
#include <crtdbg.h>
#include <time.h>
#include <direct.h>
#include <ERRNO.H>
#include <stdio.h>

#ifdef _ELEMENTCLIENT
#include <ATime.h>
#include <AMemory.h>
#else //_ELEMENTCLIENT
void _a_SetPdbSearchPath(const char* szUserSearchPath){}
void _a_DumpMemoryBlocks(FILE* pFile, FILE* pAddiInfoFile){}
void _a_EnableMemoryHistoryLog(FILE * pMemHisFile){}
unsigned long a_GetTime(){ return GetTickCount(); }
#endif //_ELEMENTCLIENT

#include "EC_HookMemory.h"
#include "ClientMemoryLog.h"
#include "EC_WinAPI.h"


#include "detours.h" //用于钩子函数的detours库
#pragma comment( lib, "detours.lib")
#pragma comment( lib, "detoured.lib")

/////////////////////////////////////////////////////////////////////////////////////////////
// local 

//根据内存地址加载函数信息
#pragma pack(1)
struct CSymbolInfoPackage 
{
	CSymbolInfoPackage() 
	{
		si.SizeOfStruct = sizeof(SYMBOL_INFO); 
		si.MaxNameLen   = sizeof(buffer); 
		memset(buffer, 0, sizeof(buffer));
	}
	SYMBOL_INFO si;
	char buffer[4096];
};
#pragma pack()

struct CImageHlpLine : public IMAGEHLP_LINE 
{
	CImageHlpLine() 
	{
		memset(this, 0, sizeof(*this));
		SizeOfStruct = sizeof(IMAGEHLP_LINE); 
	}
}; 

class MemoryHookFile
{
public:
	enum
	{
		FILE_BUFFER_LEN = 10*1024*1024,
	};
public:
	MemoryHookFile();
	~MemoryHookFile();
	
	bool  open(const char* file_name, const unsigned int hint_file_num = 0);
	void  close();
	bool  write(const char* buffer, const unsigned int len);
	
	FILE* get_file();
	bool  is_to_add_file();
	void  add_file();
	unsigned int		get_cur_file_index();
	inline const char*	get_base_file_name() const { return base_file_name[0]?base_file_name:NULL; }
	bool				get_file_name(char* buffer, const unsigned int index) const;
	
public:
	CRITICAL_SECTION*	cs_counter;
	unsigned __int64	counter;

protected:
	bool  _open(const char* file_name, const unsigned int hint_file_num);
	bool  delete_file(const char* file_name);
	
protected:
	char			base_file_name[MAX_PATH];
	unsigned int	cur_index;
	FILE**			file_array;
	unsigned int	file_num;
	
	char*			file_buffer;
//	CRITICAL_SECTION* cs_file_ptr;

};

struct sCriticalSectionWrapper
{
	sCriticalSectionWrapper(CRITICAL_SECTION* cs)
	{
		cs_counter = cs;
		if (cs_counter)
		{
			EnterCriticalSection(cs_counter);
		}
	}
	~sCriticalSectionWrapper()
	{
		Release();
	}
	void Release()
	{
		if (cs_counter)
		{
			LeaveCriticalSection(cs_counter);
			cs_counter = NULL;
		}
	}
	CRITICAL_SECTION* cs_counter;
};

static MemoryHookFile	l_angelica_history;
static MemoryHookFile	l_client_history;
static DWORD			l_TlsValueIndex = TLS_OUT_OF_INDEXES;

class CriticalSectionEasyWrapper{
	CRITICAL_SECTION m_cs;
	CriticalSectionEasyWrapper(const CriticalSectionEasyWrapper &);
	CriticalSectionEasyWrapper & operator = (const CriticalSectionEasyWrapper &);
public:
	CriticalSectionEasyWrapper(){
		::InitializeCriticalSectionAndSpinCount(&m_cs, 4000);
	}
	operator CRITICAL_SECTION * (){
		return &m_cs;
	}
	~CriticalSectionEasyWrapper(){
		::DeleteCriticalSection(&m_cs);
	}
};
static CriticalSectionEasyWrapper g_csMemoryProfile;


BOOL hook_begin();
BOOL hook_end();

static bool l_GenHistoryMemAddiLog()
{
	if (!l_angelica_history.get_base_file_name())
	{
		return false;
	}
	l_angelica_history.add_file();

	std::set<unsigned int> addressSet;

	////read history memory file
	for (unsigned int i = 0; i < l_angelica_history.get_cur_file_index(); ++i )
	{
		char address_FileName[MAX_PATH] = {0};
		l_angelica_history.get_file_name(address_FileName, i);
		FILE* addrFile = fopen(address_FileName, "rb");
		if (!addrFile)
		{
			return false;
		}

		const unsigned int buffer_size = 4096;
		char buffer[buffer_size]="";
		while (fgets(buffer, buffer_size, addrFile))
		{
			angelica_memory_operate_info info;
			if(!info.init(buffer))
			{
				assert(0);
				continue;
			}
			for (int j  = 0; j < angelica_memory_operate_info::ADDRESS_NUM; ++j )
			{
			    if ( addressSet.find(info.call_stack[j]) != addressSet.end() )
		    	{
			      	continue;
			    }
			    else
		    	{
			      	addressSet.insert( info.call_stack[j] );
			    }	
			}
		}
		fclose( addrFile );
	}

	////creat history memory addition file
	char addition_file_name[MAX_PATH] = {0};
	sprintf(addition_file_name, "%s_1.addi", l_angelica_history.get_base_file_name());
	FILE* additionFile = fopen(addition_file_name, "wb");
	if (!additionFile)
	{
		return false;
	}

	BOOL bRet = FALSE;  
	DWORD SymAddr = 0; 	
	std::set<unsigned int>::iterator it;
	for ( it = addressSet.begin(); it!=addressSet.end(); it++ )
	{
		const unsigned int buffer_size = 4096;
		char buffer[buffer_size]="";
		char* ptr = buffer;

		if ( it == addressSet.begin() )
		{
			ptr += sprintf( ptr, "----- ----- ----- -----\n" );
		}
		ptr += sprintf( ptr, "0x%08X", (*it) );

		SymAddr = (DWORD)(*it);		
		CSymbolInfoPackage sip; 	
		DWORD64 Displacement = 0; 
		CImageHlpLine LineInfo; 
		DWORD LineDisplacement = 0; 

		bRet = SymGetLineFromAddr(	GetCurrentProcess(), SymAddr, &LineDisplacement, &LineInfo  ); 		
		if( !bRet ) 
		{ 
			ptr += sprintf( ptr, " Line Error code: %d\n(-1): ", ::GetLastError() );
		}
		else 
		{   	
			ptr += sprintf( ptr, " %s\n(%d): ", LineInfo.FileName, LineInfo.LineNumber );
		}				

		
		bRet = ::SymFromAddr( GetCurrentProcess(), SymAddr, &Displacement, &sip.si );				
		if( !bRet ) 
		{ 
			ptr += sprintf( ptr, "Name Error code: %d\n", ::GetLastError() );
		}
		else 
		{
			ptr += sprintf( ptr, "%s\n", sip.si.Name );
		}	

		const size_t result = fwrite( buffer, ptr-buffer, 1, additionFile );
		assert(1==result);
	}
	fclose( additionFile );

	return true; 
}

static bool l_GenHistoryMemAddiLog_Client()
{
	if (!l_client_history.get_base_file_name())
	{
		assert(0);
		return false;
	}
	l_client_history.add_file();

	std::set<unsigned int> addressSet;

	////read client history memory file
	for (unsigned int i = 0; i < l_client_history.get_cur_file_index(); ++i )
	{
		char address_FileName[MAX_PATH] = {0};
		l_client_history.get_file_name(address_FileName, i);
		FILE* addrFile = fopen(address_FileName, "rb");
		if (!addrFile)
		{
			return false;
		}

		const unsigned int buffer_size = 4096;
		char buffer[buffer_size]="";
		while (fgets(buffer, buffer_size, addrFile))
		{
			client_memory_operate_info info;
			if(!info.init(buffer))
			{
				assert(0);
				continue;
			}
			for (int j=0; j < client_memory_operate_info::ADDRESS_NUM; ++j )
			{
				if ( addressSet.find(info.call_stack[j]) != addressSet.end() )
				{
					continue;
				}
				else
				{
					addressSet.insert(info.call_stack[j]);
				}	
			}
			
		}
		fclose( addrFile );
	}

	char addition_file_name[MAX_PATH] = {0};
	sprintf(addition_file_name, "%s_1.addi", l_client_history.get_base_file_name());
	FILE* additionFile = fopen(addition_file_name, "wb");
	if (!additionFile)
	{
		return false;
	}

	////create client history memory file
	BOOL bRet = FALSE;  
	DWORD SymAddr = 0; 	
	std::set<unsigned int>::iterator it;
	for ( it = addressSet.begin(); it!=addressSet.end(); it++ )
	{
		const unsigned int buffer_size = 4096;
		char buffer[buffer_size]="";
		char* ptr = buffer;

		if ( it == addressSet.begin() )
		{
			ptr += sprintf( ptr, "----- ----- ----- -----\n" );
		}
		ptr += sprintf( ptr, "0x%08X", (*it) );

		SymAddr = (DWORD)(*it);		
		CSymbolInfoPackage sip; 	
		DWORD64 Displacement = 0; 
		CImageHlpLine LineInfo; 
		DWORD LineDisplacement = 0; 

		bRet = SymGetLineFromAddr(	GetCurrentProcess(), SymAddr, &LineDisplacement, &LineInfo  ); 		
		if( !bRet ) 
		{ 
			ptr += sprintf( ptr, " Line Error code: %d\n(-1): ", ::GetLastError() );
		}
		else 
		{   	
			ptr += sprintf( ptr, " %s\n(%d): ", LineInfo.FileName, LineInfo.LineNumber );
		}				

		
		bRet = ::SymFromAddr( GetCurrentProcess(), SymAddr, &Displacement, &sip.si );				
		if( !bRet ) 
		{ 
			ptr += sprintf( ptr, "Name Error code: %d\n", ::GetLastError() );
		}
		else 
		{
			ptr += sprintf( ptr, "%s\n", sip.si.Name );
		}	
		const unsigned int result = fwrite( buffer, ptr-buffer, 1, additionFile);	
		assert(1==result);
	}
	fclose( additionFile );

	return true; 
}

void g_EnableMemoryHistory()
{
	if (TLS_OUT_OF_INDEXES==l_TlsValueIndex)
	{
		l_TlsValueIndex = TlsAlloc(); //don't call TlsFree(), just hold the index all the time.
		if (TLS_OUT_OF_INDEXES==l_TlsValueIndex) 
		{
			assert(0);
			return;
		}
	}
	
	const char sz_update[MAX_PATH] = "update";
	const int str_len_update = strlen(sz_update);
	char cur_dir[MAX_PATH]="";
	GetCurrentDirectoryA(MAX_PATH, cur_dir);
	const int str_len_cur_dir = strlen(cur_dir);
	if (str_len_cur_dir>str_len_update && strcmp(cur_dir+str_len_cur_dir-str_len_update, sz_update)==0)
	{
		char& last_char = cur_dir[str_len_cur_dir-str_len_update-1];
		if ('\\'==last_char || '/'==last_char)
		{
			last_char = 0; //remove "\\update"
		}
	}
	strcat(cur_dir, "\\logs");
	_mkdir(cur_dir);
	char mem_file_name_prefix[MAX_PATH] = "";
	time_t t = time(0);
	tm cur_tm = *localtime(&t);
	sprintf(mem_file_name_prefix,
		"%s\\%d-%d-%d_%d-%d-%d",
		cur_dir,
		cur_tm.tm_year+1900,
		cur_tm.tm_mon+1,
		cur_tm.tm_mday,
		cur_tm.tm_hour,
		cur_tm.tm_min,
		cur_tm.tm_sec);

	char name_buffer[MAX_PATH] = {0};
#ifdef _ELEMENTCLIENT
	//start history memory
	sprintf(name_buffer, "%s.mem_history",mem_file_name_prefix);
	l_angelica_history.open(name_buffer, 256);
	_a_EnableMemoryHistoryLog(l_angelica_history.get_file());
#endif

	//start clinet history memory
	sprintf(name_buffer, "%s.mem_history_client",mem_file_name_prefix);
	l_client_history.open(name_buffer);
	hook_begin();
}

void g_DisableMemoryHistory()
{
	g_GenerateAdditionFile(true);
	l_client_history.close();
	l_angelica_history.close();
}

void g_GenerateAdditionFile(const bool stop_hook)
{	
	if (!l_angelica_history.get_base_file_name() && !l_client_history.get_base_file_name())
	{
		return;
	}
	hook_end();
	_a_EnableMemoryHistoryLog(NULL);
	
	l_GenHistoryMemAddiLog();		
	l_GenHistoryMemAddiLog_Client();

	if (!stop_hook)
	{
		_a_EnableMemoryHistoryLog(l_angelica_history.get_file());
		hook_begin();
	}	
}

void g_MemoryDump()
{	
	////end 2 history memory
	hook_end();
	_a_EnableMemoryHistoryLog(NULL);
	l_client_history.add_file();
	l_angelica_history.add_file();
	
	char mem_file_name_prefix[MAX_PATH] = "";
	time_t t = time(0);
	tm cur_tm = *localtime(&t);
	sprintf(mem_file_name_prefix,
		"logs\\%d-%d-%d_%d-%d-%d",
		cur_tm.tm_year+1900,
		cur_tm.tm_mon+1,
		cur_tm.tm_mday,
		cur_tm.tm_hour,
		cur_tm.tm_min,
		cur_tm.tm_sec);
	
	char mem_file_name[MAX_PATH] = "";
	
	sprintf(mem_file_name,"%s.mem_dump",mem_file_name_prefix);
	FILE* pFile = fopen(mem_file_name, "wb");
	sprintf(mem_file_name,"%s.mem_dump.addi",mem_file_name_prefix);
	FILE* pAddiInfoFile = fopen(mem_file_name, "wb");
	if (pFile && pAddiInfoFile)
	{
		_a_DumpMemoryBlocks(pFile, pAddiInfoFile);
	}
	if (pFile)
	{
		fclose(pFile);
		pFile = NULL;
	}
	if (pAddiInfoFile)
	{
		fclose(pAddiInfoFile);
		pAddiInfoFile = NULL;
	}

	_a_EnableMemoryHistoryLog(l_angelica_history.get_file());
	hook_begin();		
}

void g_TickMemoryHistory()
{
	if (l_angelica_history.is_to_add_file())
	{
		_a_EnableMemoryHistoryLog(NULL);
		l_angelica_history.add_file();
		_a_EnableMemoryHistoryLog(l_angelica_history.get_file());
	}		
	if (l_client_history.is_to_add_file())
	{
		hook_end();
		l_client_history.add_file();	
		hook_begin();
		if ( 3 == l_client_history.get_cur_file_index() )//到生成第四个文件时打印addition文件
		{
			g_GenerateAdditionFile();
		}
	}
}

// Architecture-specific definitions for x86 and x64
#if defined(_M_IX86)
#define SIZEOFPTR 4
#define X86X64ARCHITECTURE IMAGE_FILE_MACHINE_I386
#define AXREG eax
#define BPREG ebp
#elif defined(_M_X64)
#define SIZEOFPTR 8
#define X86X64ARCHITECTURE IMAGE_FILE_MACHINE_AMD64
#define AXREG rax
#define BPREG rbp
#endif // _M_IX86

#if defined(_M_IX86) || defined(_M_X64)
#pragma auto_inline(off)
DWORD_PTR getprogramcounterx86x64 ()
{
    DWORD_PTR programcounter;
	
    __asm mov AXREG, [BPREG + SIZEOFPTR] // Get the return address out of the current stack frame
		__asm mov [programcounter], AXREG    // Put the return address into the variable we'll return
		
		return programcounter;
}
#pragma auto_inline(on)
#endif // defined(_M_IX86) || defined(_M_X64)

void dump_callstack(client_memory_operate_info& operate_info)
{
	int addr_count = 0;

	if (g_pRtlCaptureContext){
		CONTEXT c;
		g_pRtlCaptureContext(&c);

		STACKFRAME64 sf = {0};
		sf.AddrPC.Mode = AddrModeFlat;
		sf.AddrPC.Offset    = c.Eip;
		sf.AddrStack.Mode = AddrModeFlat;
		sf.AddrStack.Offset = c.Esp;
		sf.AddrFrame.Mode = AddrModeFlat;
		sf.AddrFrame.Offset = c.Ebp;

		for( ; ; )
		{		
			if( addr_count >= (client_memory_operate_info::ADDRESS_NUM+2) || !StackWalk64(X86X64ARCHITECTURE, GetCurrentProcess(), GetCurrentThread(), &sf, &c, NULL, SymFunctionTableAccess64, SymGetModuleBase64, NULL ) )
			{
				break;
			}
			if( sf.AddrFrame.Offset == 0 )
			{
				break;
			}
			
			if ( addr_count >= 2) //the first 2 functions are dump_callstack and MyAllocHook or hook function
			{
				operate_info.call_stack[addr_count-2] = (unsigned int)sf.AddrPC.Offset;
			}	
			++addr_count;
		}
	}
	for ( int i = addr_count; i < (client_memory_operate_info::ADDRESS_NUM+2); i++)
	{
		if (i >= 2){
			operate_info.call_stack[i-2] = 0;
		}
	}
}

typedef LPVOID (WINAPI* pHeapAlloc)( HANDLE hHeap, DWORD dwFlags, DWORD dwBytes );
pHeapAlloc g_pHeapAlloc = HeapAlloc;
typedef BOOL (WINAPI* pHeapFree)( HANDLE hHeap, DWORD dwFlags, LPVOID lpMem );
pHeapFree g_pHeapFree = HeapFree;
typedef LPVOID (WINAPI* pHeapReAlloc)( HANDLE hHeap, DWORD dwFlags, LPVOID plMem, SIZE_T dwBytes );
pHeapReAlloc g_pHeapReAlloc = HeapReAlloc;

LPVOID WINAPI Hook_HeapAlloc( HANDLE hHeap, DWORD dwFlags, DWORD dwBytes )
{
	sCriticalSectionWrapper _dummy(g_csMemoryProfile);
	assert(TLS_OUT_OF_INDEXES!=l_TlsValueIndex);
	const int is_from_hook = (TLS_OUT_OF_INDEXES!=l_TlsValueIndex)?(int)TlsGetValue(l_TlsValueIndex):true;
	if (is_from_hook) //避免递归嵌套调用
	{
		return g_pHeapAlloc(hHeap, dwFlags, dwBytes);	
	}
	
	TlsSetValue(l_TlsValueIndex, (void*)1);
	sCriticalSectionWrapper _csWrapper(l_client_history.cs_counter);
	const unsigned __int64 cur_counter = l_client_history.counter++;
	LPVOID hRet = g_pHeapAlloc(hHeap, dwFlags, dwBytes);
	_csWrapper.Release();
	if (hRet)
	{
		const DWORD dwTime = a_GetTime();
		char buffer[4096]="";
		client_memory_operate_info operate_info;
		operate_info.type = client_memory_operate_info::TYPE_ALLOC;
		dump_callstack(operate_info);
		operate_info.memory_address = (unsigned int)hRet;
		operate_info.memory_size	= dwBytes;
		operate_info.time			= dwTime;
		operate_info.hHeap			= (unsigned int)hHeap;
		operate_info.dwFlags		= dwFlags;
		operate_info.log_index		= cur_counter;
		operate_info.to_string(buffer);
		l_client_history.write( buffer, strlen(buffer));	
	}	
	TlsSetValue(l_TlsValueIndex, 0);
	return hRet;
}

BOOL WINAPI Hook_HeapFree( HANDLE hHeap, DWORD dwFlags, LPVOID lpMem )
{
	sCriticalSectionWrapper _dummy(g_csMemoryProfile);
	assert(TLS_OUT_OF_INDEXES!=l_TlsValueIndex);
	const int is_from_hook = (TLS_OUT_OF_INDEXES!=l_TlsValueIndex)?(int)TlsGetValue(l_TlsValueIndex):true;
	if (is_from_hook )
	{
		return g_pHeapFree(hHeap, dwFlags, lpMem);	
	}

	TlsSetValue(l_TlsValueIndex, (void*)1);
	const unsigned int old_memory_size = lpMem?HeapSize(hHeap, dwFlags, lpMem):0;
	sCriticalSectionWrapper _csWrapper(l_client_history.cs_counter);
	const unsigned __int64 cur_counter = l_client_history.counter++;
	BOOL result = g_pHeapFree(hHeap, dwFlags, lpMem);
	_csWrapper.Release();
	if (lpMem && result)
	{
		const DWORD dwTime = a_GetTime();
		char buffer[4096]="";
		client_memory_operate_info operate_info;
		operate_info.type = client_memory_operate_info::TYPE_FREE;
		operate_info.memory_address = (unsigned int)lpMem;
		operate_info.memory_size	= old_memory_size;
		operate_info.time			= dwTime;
		operate_info.hHeap			= (unsigned int)hHeap;
		operate_info.dwFlags		= dwFlags;
		operate_info.log_index		= cur_counter;
		operate_info.to_string(buffer);
		l_client_history.write( buffer, strlen(buffer));	
	}
	TlsSetValue(l_TlsValueIndex, 0);
	return result;
}

LPVOID WINAPI Hook_HeapReAlloc( HANDLE hHeap, DWORD dwFlags, LPVOID lpMem, SIZE_T dwBytes )
{
	sCriticalSectionWrapper _dummy(g_csMemoryProfile);
	assert(TLS_OUT_OF_INDEXES!=l_TlsValueIndex);
	const int is_from_hook = (TLS_OUT_OF_INDEXES!=l_TlsValueIndex)?(int)TlsGetValue(l_TlsValueIndex):true;
	if(is_from_hook)
	{
		return g_pHeapReAlloc(hHeap, dwFlags, lpMem, dwBytes);
	}

	TlsSetValue(l_TlsValueIndex, (void*)1);
	const unsigned int old_memory_size = lpMem?HeapSize(hHeap, dwFlags, lpMem):0;
	sCriticalSectionWrapper _csWrapper(l_client_history.cs_counter);
	unsigned __int64 cur_counter = l_client_history.counter;
	l_client_history.counter += 2;
	LPVOID hRet = g_pHeapReAlloc(hHeap, dwFlags, lpMem, dwBytes);	
	_csWrapper.Release();
	if (hRet)
	{	
		const DWORD dwTime = a_GetTime();
		char buffer[4096]="";
		if(lpMem)
		{
			client_memory_operate_info operate_info;
			operate_info.type = client_memory_operate_info::TYPE_DELETE_BY_REALLOC;
			operate_info.memory_address = (unsigned int)lpMem;
			operate_info.memory_size	= old_memory_size;
			operate_info.time			= dwTime;
			operate_info.hHeap			= (unsigned int)hHeap;
			operate_info.dwFlags		= dwFlags;
			operate_info.log_index		= cur_counter++;
			operate_info.to_string(buffer);
			l_client_history.write( buffer, strlen(buffer));	
		}
		{
			client_memory_operate_info operate_info;
			operate_info.type = client_memory_operate_info::TYPE_REALLOC;
			dump_callstack(operate_info);
			operate_info.memory_address = (unsigned int)hRet;
			operate_info.memory_size	= dwBytes;
			operate_info.time			= dwTime;
			operate_info.hHeap			= (unsigned int)hHeap;
			operate_info.dwFlags		= dwFlags;
			operate_info.log_index		= cur_counter;
			operate_info.to_string(buffer);
			l_client_history.write( buffer, strlen(buffer));	
		}
	}else
	{
		if (lpMem && 0==dwBytes)
		{
			const DWORD dwTime = a_GetTime();
			char buffer[4096]="";
			client_memory_operate_info operate_info;
			operate_info.type = client_memory_operate_info::TYPE_DELETE_BY_REALLOC;
			operate_info.memory_address = (unsigned int)lpMem;
			operate_info.memory_size	= old_memory_size;
			operate_info.time			= dwTime;
			operate_info.hHeap			= (unsigned int)hHeap;
			operate_info.dwFlags		= dwFlags;
			operate_info.log_index		= cur_counter;
			operate_info.to_string(buffer);
			l_client_history.write( buffer, strlen(buffer));	
		}
	}
	
	TlsSetValue(l_TlsValueIndex, 0);
	return hRet;
}

BOOL hook_begin()
{
	sCriticalSectionWrapper _dummy(g_csMemoryProfile);
	DetourTransactionBegin();
	DetourUpdateThread(::GetCurrentThread());
	
	if ( DetourAttach( &(PVOID&)g_pHeapAlloc, Hook_HeapAlloc) != NO_ERROR)
	{
		return FALSE;
	} 
	if ( DetourAttach( &(PVOID&)g_pHeapFree, Hook_HeapFree) != NO_ERROR)
	{
		return FALSE;
	} 
	if ( DetourAttach( &(PVOID&)g_pHeapReAlloc, Hook_HeapReAlloc) != NO_ERROR)
	{
		return FALSE;
	} 
	
	if ( DetourTransactionCommit() != NO_ERROR)
	{
		return FALSE;
	}
	return TRUE;
}

BOOL hook_end()
{
	sCriticalSectionWrapper _dummy(g_csMemoryProfile);
	DetourTransactionBegin();
	DetourUpdateThread( ::GetCurrentThread());
	
	if ( DetourDetach( &(PVOID&)g_pHeapAlloc, Hook_HeapAlloc) != NO_ERROR)
	{
		return FALSE;
	} 
	if ( DetourDetach( &(PVOID&)g_pHeapFree, Hook_HeapFree) != NO_ERROR)
	{
		return FALSE;
	} 
	if ( DetourDetach( &(PVOID&)g_pHeapReAlloc, Hook_HeapReAlloc) != NO_ERROR)
	{
		return FALSE;
	} 
	
	if ( DetourTransactionCommit() != NO_ERROR)
	{
		return FALSE;
	}
	return TRUE;
}

MemoryHookFile::MemoryHookFile()
{
	memset(base_file_name, 0, sizeof(base_file_name));
	cur_index	= 0;
	file_array	= NULL;
	file_num	= 0;

	file_buffer	= new char[FILE_BUFFER_LEN];
	cs_counter = new CRITICAL_SECTION();
	if (cs_counter)
	{	
		InitializeCriticalSectionAndSpinCount(cs_counter, 4000);
	}else
	{
		assert(0);
	}
//	cs_file_ptr = new CRITICAL_SECTION();
// 	if (cs_file_ptr)
// 	{	
// 		InitializeCriticalSectionAndSpinCount(cs_file_ptr, 4000);
// 	}else
// 	{
// 		assert(0);
// 	}
	counter = 1;
}

MemoryHookFile::~MemoryHookFile()
{
	close();
	if (file_buffer)
	{
		delete[] file_buffer;
		file_buffer = NULL;
	}
	if (cs_counter)
	{
		DeleteCriticalSection(cs_counter);
		delete cs_counter;
		cs_counter = NULL;
	}
// 	if (cs_file_ptr)
// 	{
// 		DeleteCriticalSection(cs_file_ptr);
// 		delete cs_file_ptr;
// 		cs_file_ptr = NULL;
// 	}
}

bool MemoryHookFile::open(const char* file_name, const unsigned int hint_file_num)
{
	close();
	if (!_open(file_name, hint_file_num))
	{
		assert(0);
		close();
		return false;
	}
	return true;
}

bool MemoryHookFile::_open(const char* file_name, const unsigned int hint_file_num)
{
	if (!file_name[0])
	{
		assert(0);
		return false;
	}
	memset(base_file_name, 0, sizeof(base_file_name));
	strcpy(base_file_name, file_name);
	file_num = hint_file_num?hint_file_num:128;
	file_array = new FILE*[file_num];
	if (!file_array)
	{
		assert(0);
		return false;
	}
	memset(file_array, 0, sizeof(file_array[0])*file_num);
	char name_buffer[MAX_PATH] = {0};
	for (unsigned int i=0; i<file_num; ++i)
	{
		if(!get_file_name(name_buffer, i))
		{
			assert(0);
			return false;
		}
		file_array[i] = fopen(name_buffer, "wb");
		if (file_array[i] && file_buffer)
		{
			if (0 != setvbuf( file_array[i], file_buffer, _IOFBF, FILE_BUFFER_LEN))
			{
				assert(0);
				return false;
			}
		}else
		{
			assert(0);
			return false;
		}
	}
	cur_index = 0;
	return true;	
}

void  MemoryHookFile::close()
{
	if (file_array)
	{
		for (unsigned int i=0; i<file_num; ++i)
		{
			FILE* file = file_array[i];
			if (file)
			{
				file_array[i] = NULL;
				assert(i>=cur_index);
				fclose(file);
				file = NULL;
			}
		}
		delete[] file_array;
		file_array = NULL;
	}
	if (base_file_name[0])
	{
		char name_buffer[MAX_PATH];
		for (unsigned int i=cur_index+1; i<file_num; ++i)
		{
			get_file_name(name_buffer, i);
			delete_file(name_buffer);
		}
	}
	
	cur_index	= 0;
	file_num = 0;
	memset(base_file_name, 0, sizeof(base_file_name));
}

bool MemoryHookFile::get_file_name(char* buffer, const unsigned int index) const
{
	if (!buffer || !base_file_name[0] || index>=file_num)
	{
		assert(0);
		if (buffer)
		{
			buffer[0] = 0;
		}
		return false;
	}
	sprintf(buffer, "%s_%d", base_file_name, index+1);
	return true;
}

bool MemoryHookFile::delete_file(const char* file_name)
{
	if (!file_name || !file_name[0])
	{
		assert(0);
		return false;
	}
	const unsigned int str_len = strlen(file_name);
	char new_file_name[MAX_PATH*2] = {0};
	memcpy(new_file_name,file_name,str_len);
	new_file_name[str_len] = 0;
	new_file_name[str_len+1] = 0;
	
	SHFILEOPSTRUCTA shStruct;
	shStruct.hwnd                  = NULL;
	shStruct.pTo                   = NULL;
	shStruct.fAnyOperationsAborted = NULL;
	shStruct.hNameMappings         = NULL;
	shStruct.lpszProgressTitle     = NULL;
	shStruct.wFunc                 = FO_DELETE;
	shStruct.pFrom                 = new_file_name;
	shStruct.fFlags                = /*FOF_ALLOWUNDO|*/FOF_NOCONFIRMATION;
	const int result = SHFileOperationA(&shStruct);
	if (result)
	{
		assert(0);
		return false;
	}
	return true;
}

void MemoryHookFile::add_file()
{ 
//	EnterCriticalSection(cs_file_ptr);
	if(file_array && cur_index<file_num)
	{ 
		FILE* cur_file = file_array[cur_index];
		file_array[cur_index] = NULL;
		if (cur_file)
		{
			int result = fclose(cur_file);
			assert(0==result);
			cur_file = NULL;
		}else
		{
			assert(0);
		}
		++cur_index;
	} 
//	LeaveCriticalSection(cs_file_ptr);
}

unsigned int MemoryHookFile::get_cur_file_index()
{ 
//	EnterCriticalSection(cs_file_ptr);
	return cur_index; 
//	LeaveCriticalSection(cs_file_ptr);
}

bool MemoryHookFile::write(const char* buffer, const unsigned int len)
{
	if (!file_array)
	{
		assert(0);
		return false;
	}
	if (!buffer || !len)
	{
		assert(0);
		return true;
	}
//	assert (cs_file_ptr);
//	EnterCriticalSection(cs_file_ptr);
	const int MAX_TRY_NUM = 100;
	for (unsigned int i=0; i<MAX_TRY_NUM; ++i)
	{
		const unsigned int temp_index = cur_index;
		if(temp_index<file_num)
		{
			FILE* file_ptr = file_array[temp_index];
			if (file_ptr)
			{
				if (1==fwrite(buffer, len, 1, file_ptr))
				{
					break;
				}
			}
		}else
		{
			break;
		}
	}
	assert(i<MAX_TRY_NUM);
//	LeaveCriticalSection(cs_file_ptr);
	return (i<MAX_TRY_NUM);
}

bool MemoryHookFile::is_to_add_file()
{
	if (!file_array)
	{
		return false;
	}
//	assert (cs_file_ptr);
	bool result = false;
//	EnterCriticalSection(cs_file_ptr);
	if(cur_index<file_num)
	{
		FILE* pFile = file_array[cur_index];
		if (pFile)
		{
			const long file_len = ftell(pFile);
			if (file_len>=512*1024*1024)//512M 
			{
				result = true;
			}
		}
	}
//	LeaveCriticalSection(cs_file_ptr);
	return result;
}

FILE* MemoryHookFile::get_file()
{
	if (!file_array)
	{
		return NULL;
	}
//	assert (cs_file_ptr);
	FILE* pFile = NULL;
//	EnterCriticalSection(cs_file_ptr);
	if(cur_index<file_num)
	{
		pFile = file_array[cur_index];
	}
//	LeaveCriticalSection(cs_file_ptr);
	return pFile;
}


#endif
