/*
 * FILE: AMemory.h
 *
 * DESCRIPTION: Routines for memory allocating and freeing
 *
 * CREATED BY: duyuxin, 2003/6/4
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#ifndef _AMEMORY_H_
#define _AMEMORY_H_

#include "ABaseDef.h"
#include "hashmap.h"

#pragma once

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

#if !defined(_DEBUG) || !defined(UNICODE)

//	Enable below line to forbid new and delete operators
//	#define	_A_FORBID_NEWDELETE
//	Enable below line ot forbid a_malloc and a_free functions
//	#define _A_FORBID_MALLOC

#endif

#if (defined (_DEBUG) && !defined (_A_FORBID_NEWDELETE))
//	#define A_DEBUG_NEW new (__FILE__, __LINE__)
	#define A_DEBUG_NEW new
#else
	#define A_DEBUG_NEW new
#endif

#define a_malloc(size) _a_New(size)
#define a_realloc(pMem, size) _a_Realloc(pMem, size)
#define a_malloc_align(size, align) _a_NewAlign(size, align)
#define a_free(p) _a_Delete(p)
#define a_free_align(p) _a_DeleteAlign(p)
#define a_malloctemp(size) _a_MallocTemp(size)
#define a_freetemp(p) _a_FreeTemp(p)

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

//	Don't use below functions directly, instead to use new, delete, a_malloc, 
//	a_realloc, a_free etc.
void* _a_New(size_t size);
void* _a_Realloc(void* pMem, size_t size);
void* _a_NewAlign(size_t size, int align);

void _a_Delete(void *p);
void _a_DeleteAlign(void* pData);

void* _a_MallocTemp(size_t size);
void _a_FreeTemp(void* p);

#ifdef _DEBUG
	void _a_DumpMemoryBlocks(FILE* pFile, FILE* pAddiInfoFile);
	void _a_EnableMemoryHistoryLog(FILE * pMemHisFile);
#endif

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////

#ifndef _A_FORBID_NEWDELETE

	void* operator new (size_t size);
	void operator delete (void *p);
	void* operator new [] (size_t size);
	void operator delete [] (void *p);

	#ifdef _DEBUG
		void* operator new (size_t size, const char* szFile, int iLine);
		void operator delete (void* p, const char* szFile, int iLine);
		void* operator new [] (size_t size, const char* szFile, int iLine);
		void operator delete [] (void* p, const char* szFile, int iLine);
	#endif	//	_DEBUG

#endif	//	_A_FORBID_NEWDELETE

///////////////////////////////////////////////////////////////////////////
//
//	Symbol functions
//
///////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG

#include <Windows.h>

struct s_MEM_CALLER_INFO
{
	DWORD	callers[16];
	DWORD	call_count;
	DWORD	total_call;
	DWORD	total_size;
};

class callers_info_alloc
{
public: 

	static void* allocate(size_t size);

	static void deallocate(void * ptr,size_t size)
	{ 
	}

	inline static void deallocate(void * ptr) 
	{ 
	}	
};

typedef abase::hash_map<DWORD, s_MEM_CALLER_INFO*, abase::_hash_function, callers_info_alloc>  MemCallersInfoMap;

class MemCallersInfoMan
{
public:

	MemCallersInfoMan();
	~MemCallersInfoMan();

protected:

	CRITICAL_SECTION m_csMemCallersInfo;
	MemCallersInfoMap m_MemCallersInfo;

public:

	MemCallersInfoMap& GetMap() { return m_MemCallersInfo; }
	void Lock() { ::EnterCriticalSection(&m_csMemCallersInfo); }
	void Unlock() { ::LeaveCriticalSection(&m_csMemCallersInfo); }
};

#endif

#endif	//	_AMEMORY_H_
