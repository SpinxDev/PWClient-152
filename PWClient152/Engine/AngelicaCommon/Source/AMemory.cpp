  /*
 * FILE: AMemory.cpp
 *
 * DESCRIPTION: Routines for memory allocating and freeing
 *
 * CREATED BY: duyuxin, 2003/6/4
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#include <malloc.h>
#include <assert.h>
#include <stdio.h>
#include "AMemory.h"
#include "ALog.h"
#include "hashmap.h"
#include "ATempMemMan.h"

/*	init_seg (lib) ensure ACommon::l_MemoryMan to be constructed before and
	destructed after all other application objects. This is very
	important, because some application's global or static objects may allocate
	or free memory in their constructor or destructors.

	init_seg (lib) will cause a VC compiling warning 4073, disable it.
*/
#pragma warning (disable: 4073)
#pragma init_seg (lib)

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

//	Memory align size
#define MEM_ALIGN					16
#define MEM_POOL_DATA_SIZE			512 * 1024
#define MEM_POOL_MAX_BLOCK_COUNT	MEM_POOL_DATA_SIZE / 16
#define MEM_SMALLSIZE				1024
#define MEM_SLOTNUM					(MEM_SMALLSIZE / MEM_ALIGN)
#define MEM_GC_COUNTER				1000

#define MEM_ALLOC_FLAG_S			0x0100
#define MEM_FREE_FLAG_S				0x0101
#define MEM_ALLOC_FLAG_L			0x0200
#define MEM_FREE_FLAG_L				0x0201

#define MEM_ALLOC_FILL				0xcd
#define MEM_FREE_FILL				0xfe
#define MEM_SLOP_OVER				0x98989898

//	Non-zero means thread safe opening
#define MEM_THREADSAFE		1

//	Size used for slop-over checking
#define SLOPOVER_SIZE		(sizeof (DWORD) * 2)

//	Max call stack level
#define MAX_CALLSTACK_LV	8
#define MAX_SYM_CALLSTACK_LV	256

#if MAX_CALLSTACK_LV < 8
#error "Max record call stack frame count should not be less than 8"
#endif

///////////////////////////////////////////////////////////////////////////
//
//	Reference to External variables and functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Local Types and Variables and Global variables
//
///////////////////////////////////////////////////////////////////////////

/*	In current version, two things must be guaranteed to be true.

		1. sizeof (s_MEMLARGEBLK) >= sizeof (s_MEMSMALLBLK)
		2. size from iRawSize to the end of s_MEMLARGEBLK and s_MEMSMALLBLK
		   must be kept same.
*/

//	Memory block used by AMemSmall
struct s_MEMSMALLBLK
{
#ifdef _DEBUG
	
	int		iRawSize;	//	Raw memory size
	DWORD	callers[MAX_CALLSTACK_LV];	//	max 4 level call stack
	
#endif

	short	sFlag;		//	Memory flag
	short	sPoolSlot;	//	Pool slot index
				  
	s_MEMSMALLBLK*	pNext;	//	Next free block

#ifdef _DEBUG

	DWORD	aSOFlags[2];	//	Slop over flags

#endif
};

//	Memory block used by AMemLarge
struct s_MEMLARGEBLK
{
#ifdef _DEBUG

	s_MEMLARGEBLK*	pPrev;	//	Previous block
	s_MEMLARGEBLK*	pNext;	//	Next block

	int		iRawSize;	//	Raw memory size
	DWORD	callers[MAX_CALLSTACK_LV];	//	max 4 level call stack

#endif

	short	sFlag;		//	Memory flag
	short	sReserved;	//	Reserved
	int		iBlkSize;	//	Memory block size

#ifdef _DEBUG

	DWORD	aSOFlags[2];	//	Slop over flags

#endif
};



#ifdef _DEBUG
FILE * GetMemoryHistoryLog();
void DumpAdditionalInfo(FILE* pAddiInfoFile);
void DumpDeleteHistory(FILE* pf, const s_MEMSMALLBLK* pBlock);
void DumpDeleteHistory(FILE* pf, const s_MEMLARGEBLK* pBlock);
void DumpAllocHistory(FILE* pf, const s_MEMSMALLBLK* pBlock);
void ExportSymbolInfo(void * ptr);
template<typename _TBlk>
static void DumpBlockInfo(const _TBlk* pBlock)
{
	char buffer[1024];

	_snprintf(buffer, sizeof(buffer) / sizeof(buffer[0]), 
		"------------------------------------\n"
		"Leak %d byte(s):\n", pBlock->iRawSize);
	OutputDebugStringA(buffer);

	for (int i = 0; i < MAX_SYM_CALLSTACK_LV; ++i)
	{
		if (pBlock->callers[i] < 0xffff)
			break;

		ExportSymbolInfo((void*)pBlock->callers[i]);
	}
}

void DumpMemSmallBlkToALogOutput(const s_MEMSMALLBLK* pBlock)
{
	char szMsg[1024] = {0};
	sprintf(szMsg, "Memory [%d bytes@%p] leak at (", pBlock->iRawSize, (char*)pBlock + sizeof(s_MEMSMALLBLK));
	for (int i = 0; i < MAX_CALLSTACK_LV; ++i)
	{
		char szPart[64] = {0};
		if (i == MAX_CALLSTACK_LV - 1 || (!pBlock->callers[i] && i >= 7))
		{
			sprintf(szPart, "0x%p)", pBlock->callers[i]);
			strcat(szMsg, szPart);
			break;
		}
		else
		{
			sprintf(szPart, "0x%p <- ", pBlock->callers[i]);
			strcat(szMsg, szPart);
		}
	}

	a_LogOutput(1, szMsg);

	DumpBlockInfo(pBlock);
}

void DumpMemLargeBlkToALogOutput(const s_MEMLARGEBLK* pBlock)
{
	char szMsg[1024] = {0};
	sprintf(szMsg, "Memory [%d bytes@%p] leak at (", pBlock->iRawSize, (char*)pBlock + sizeof(s_MEMLARGEBLK));
	for (int i = 0; i < MAX_CALLSTACK_LV; ++i)
	{
		char szPart[64] = {0};
		if (i == MAX_CALLSTACK_LV - 1 || (!pBlock->callers[i] && i >= 7))
		{
			sprintf(szPart, "0x%p)", pBlock->callers[i]);
			strcat(szMsg, szPart);
			break; 
		}
		else
		{
			sprintf(szPart, "0x%p <- ", pBlock->callers[i]);
			strcat(szMsg, szPart);
		}
	}

	a_LogOutput(1, szMsg);

	DumpBlockInfo(pBlock);
}

#endif

#ifdef _DEBUG

static char _mem_info_map_buf[1024 * 1024 * 8];
static size_t _mem_info_buf_cur_index = 0;

void* callers_info_alloc::allocate(size_t size)
{
	void* ret = _mem_info_map_buf + _mem_info_buf_cur_index;
	_mem_info_buf_cur_index += size;
	return ret;
}

MemCallersInfoMan::MemCallersInfoMan() : m_MemCallersInfo(100000)
{
	::InitializeCriticalSection(&m_csMemCallersInfo);
}

MemCallersInfoMan::~MemCallersInfoMan()
{
	::DeleteCriticalSection(&m_csMemCallersInfo);
}

MemCallersInfoMan l_MemCallersInfo;

void LockMemCallersInfoModule()
{
	l_MemCallersInfo.Lock();
}

void UnlockMemCallersInfoModule()
{
	l_MemCallersInfo.Unlock();
}

size_t GetMemCallersInfoCount()
{
	return l_MemCallersInfo.GetMap().size();
}

static MemCallersInfoMap::iterator itRet;

bool GetMemCallersInfo(bool bFirst, DWORD& dwKey, DWORD* callers, DWORD& call_count, DWORD& total_size)
{
	MemCallersInfoMap::iterator it = (bFirst ? l_MemCallersInfo.GetMap().begin() : ++itRet);
	dwKey = it->first;
	memcpy(callers, it->second->callers, sizeof(it->second->callers));
	call_count = it->second->call_count;
	total_size = it->second->total_size;
	itRet = it;
	return true;
}

#endif

///////////////////////////////////////////////////////////////////////////
//
//	Local functions
//
///////////////////////////////////////////////////////////////////////////

inline void* _RawMemAlloc(size_t size) { return malloc(size); }
inline void _RawMemFree(void* p) { free(p);	}

#ifdef _DEBUG

static void _MemThreadLock(long* plAtom)
{
	if (InterlockedExchange(plAtom, 1))		//	Held by other ?
	{
		int i;
		for (i=0; i < 128 && InterlockedExchange(plAtom, 1); i++)
			Sleep(0);
		
		if (i >= 128)
		{
			//	Because of garbage collecting, so lock time maybe a 
			//	litter longer
			for (i=0; i < 64 && InterlockedExchange(plAtom, 1); i++)
				Sleep(50);

			if (i >= 64)
				assert(0);	//	Holding too long
		}
	}
}

#else

static void _MemThreadLock(long* plAtom)
{
	while (InterlockedExchange(plAtom, 1))
		Sleep(0);
}

#endif	//	_DEBUG

static void _MemThreadUnlock(long* plAtom)
{
	InterlockedExchange(plAtom, 0);
}

//	Get block address from a allocated memory address
static s_MEMSMALLBLK* _GetMemBlockInfo(void* pMem)
{
	//	In both debug and release modes, sizeof (s_MEMSMALLBLK) <= sizeof (s_MEMLARGEBLK).
	return (s_MEMSMALLBLK*)((char*)pMem - sizeof (s_MEMSMALLBLK));
}

//	Fill slop over flags
static void _FillSlopOverFlags(void* pBuf)
{
	DWORD* pdw = (DWORD*)pBuf;
	*(pdw + 0) = MEM_SLOP_OVER;
	*(pdw + 1) = MEM_SLOP_OVER;
}

//	Check slop over flags
static bool _CheckSlopOver(const void* pBuf)
{
	const DWORD* pdw = (const DWORD*)pBuf;
	if (*(pdw+0) != MEM_SLOP_OVER || *(pdw+1) != MEM_SLOP_OVER)
		return false;
	else
		return true;
}

///////////////////////////////////////////////////////////////////////////
//
//	AMemVector
//
///////////////////////////////////////////////////////////////////////////

template <class T>
class AMemVector
{
public:		//	Types

public:		//	Constructors and Destructors

	AMemVector() : m_pData(NULL), m_iSize(0), m_iMaxSize(0) {}

	virtual ~AMemVector()
	{ 
		if (m_pData)
			_RawMemFree(m_pData);
	}

public:		//	Attributes

public:		//	Operations

	//	Add a element to vector
	void push_back(const T &t);

	//	Get a element
	const T& operator [] (int n) const
	{
		assert(n >= 0 && n < m_iSize);
		return m_pData[n];
	}

	//	Remove an element quickly
	void remove_quickly(int n)
	{
		assert(n >= 0 && n < m_iSize);
		if (m_iSize == 1 || n == m_iSize-1)
			m_iSize--;
		else
		{
			m_pData[n] = m_pData[m_iSize-1];
			m_iSize--;
		}
	}

	//	Get number of element
	int	size() const { return m_iSize; }
	//	Get maximum size of vector
	int	max_size() const { return m_iMaxSize; }

protected:	//	Attributes

	T*		m_pData;		//	Array pointer
	int		m_iSize;		//	Number of current used object
	int		m_iMaxSize;		//	Total size of array in object

protected:	//	Operations

};

template <class T>
void AMemVector<T>::push_back(const T& t)
{
	if (m_iSize >= m_iMaxSize)
	{
		int iNewSize = (m_iMaxSize + 16) * 3 / 2;
		T* pNewData	= (T*)_RawMemAlloc(iNewSize * sizeof (T));

		if (m_pData)
		{
			memcpy(pNewData, m_pData, m_iSize * sizeof (T));
			_RawMemFree(m_pData);
		}

		m_pData		= pNewData;
		m_iMaxSize	= iNewSize;
	}

	m_pData[m_iSize++] = t;
}

///////////////////////////////////////////////////////////////////////////
//
//	AMemPool
//
///////////////////////////////////////////////////////////////////////////

class AMemPool
{
public:		//	Types

	struct POOLSLOT
	{
		s_MEMSMALLBLK*	pPool;
	};

public:		//	Constructors and Destructors

	AMemPool()
	{
		m_iBlkCount = 256;
		m_iDataSize	= 16;
		m_iBlkSize	= CalcBlockSize();
		m_iGCCnt	= 0;
	}

	virtual ~AMemPool();

public:		//	Attributes

	//	Allocate memory pool
	s_MEMSMALLBLK* Allocate();

public:		//	Operations

	//	Get slot number
	int	GetSlotNum() const { return m_Pool.size(); }
	//	Get pool of specified slot
	s_MEMSMALLBLK* GetPool(int iSlot) const { return m_Pool[iSlot].pPool; }
	//  Get block count
	int	GetBlockCount() const { return m_iBlkCount; }
	//	Set block count 
	void SetBlockCount(int iCount) { m_iBlkCount = iCount; }
	//	Get block size
	int GetBlockSize() const { return m_iBlkSize; }
	//	Set data size
	void SetDataSize(int iSize)
	{
		m_iDataSize = iSize;
		m_iBlkSize	= CalcBlockSize(); 
	}
	//	Get data size
	int GetDataSize() const { return m_iDataSize; }
	//	Get garbage collect counter
	int GetGCCounter() { return m_iGCCnt; }
	//	Inc garbage collect counter
	int IncGCCounter() { return ++m_iGCCnt; }
	//	Clear garbage collect counter
	void ClearGCCounter() { m_iGCCnt = 0; }
	//	Release pool slot
	void ReleasePool(int iSlot);

protected:	//	Attributes

	int		m_iBlkCount;	//	Count of memory block managed by this pool
	int		m_iBlkSize;		//	Size of memory block managed by this pool
	int		m_iDataSize;	//	Data size of memory block
	int		m_iGCCnt;		//	Garbage collect counter	

	AMemVector<POOLSLOT>	m_Pool;		//	Memory pool

protected:	//	Operations

	//	Initialize a pool
	void InitPool(POOLSLOT* pSlot);
	//	Calculate block size basing on raw size
	int CalcBlockSize() { return m_iDataSize + sizeof (s_MEMSMALLBLK); }
};

AMemPool::~AMemPool()
{
	for (int i=0; i < m_Pool.size(); i++)
		_RawMemFree(m_Pool[i].pPool);
}

/*	Initialize memory pool buffer

	pSlot: pool slot's address
*/
void AMemPool::InitPool(POOLSLOT* pSlot)
{
	char* pNext, *pByte = (char*)pSlot->pPool;
	for (int i=0; i<m_iBlkCount-1; i++, pByte=pNext)
	{
		pNext = pByte + m_iBlkSize;
		((s_MEMSMALLBLK*)pByte)->pNext = (s_MEMSMALLBLK*)pNext;
	}

	//	The last block
	((s_MEMSMALLBLK*)pByte)->pNext = NULL;
}

/*	Create memory pool

	iBlkSize: size of eack block in pool
*/
s_MEMSMALLBLK* AMemPool::Allocate()
{
	//	Allocate pool buffer
	POOLSLOT Slot;
//	Slot.pPool = (s_MEMSMALLBLK*)_RawMemAlloc(m_iBlkCount * m_iBlkSize);
	Slot.pPool = (s_MEMSMALLBLK*)_RawMemAlloc(MEM_POOL_DATA_SIZE);
	if (!Slot.pPool)
		return NULL;

	//	Initialize memory pool
	InitPool(&Slot);

	m_Pool.push_back(Slot);

	return Slot.pPool;
}

//	Release pool slot
void AMemPool::ReleasePool(int iSlot)
{
	_RawMemFree(m_Pool[iSlot].pPool);
	m_Pool.remove_quickly(iSlot);
}

///////////////////////////////////////////////////////////////////////////
//
//	AMemSmall
//
///////////////////////////////////////////////////////////////////////////

class AMemSmall
{
public:		//	Types

	struct POOLSLOT
	{
		s_MEMSMALLBLK*	pFreeBlks;	//	Free blocks
		int				iDataSize;	//	Data size
		int				iBlockSize;	//	Block size
		int				iBlockCnt;	//	Total allocated block counter
		int				iFreeCnt;	//	Free block counter
	};

public:		//	Constructors and Destructors

	AMemSmall() : m_pMemMan(NULL)
	{
		for (int i=0; i < MEM_SLOTNUM; i++)
		{	
			//	Set data size first, so that block size can be calculated.
			int iSize = (i + 1) * MEM_ALIGN;
			m_aPoolMans[i].SetDataSize(iSize);

			int iCount = MEM_POOL_DATA_SIZE / m_aPoolMans[i].GetBlockSize();
			m_aPoolMans[i].SetBlockCount(iCount);

			m_aPools[i].pFreeBlks	= NULL;
			m_aPools[i].iDataSize	= m_aPoolMans[i].GetDataSize();
			m_aPools[i].iBlockSize	= m_aPoolMans[i].GetBlockSize();
			m_aPools[i].iBlockCnt	= 0;
			m_aPools[i].iFreeCnt	= 0;

			m_aThreadAtoms[i]	= 0;
			m_dwGCCounter		= 0;
		}
	}

	virtual ~AMemSmall();

public:		//	Attributes

	friend class AMemoryMan;

public:		//	Operations

	//	Allocate memory
	void* Allocate(size_t size);
	//	Free memory
	void Free(void *p);
	
	//  Get pool slot num
	int GetPoolSlotNum(int iSlot) { return m_aPoolMans[iSlot].GetSlotNum(); }
	//	Get pool slot block num
	int GetPoolSlotBlockNum(int iSlot) { return m_aPoolMans[iSlot].GetBlockCount(); }
	//	Get block size of specified pool slot
	int GetBlockSize(int iSlot) { return m_aPools[iSlot].iBlockSize; }
	//	Get data size of specified pool slot
	int GetDataSize(int iSlot) { return m_aPools[iSlot].iDataSize; }
	//	Get total allocated block counter of specified pool slot
	int GetTotalBlockCnt(int iSlot) { return m_aPools[iSlot].iBlockCnt; }
	//	Get free block counter of specified pool slot
	int GetFreeBlockCnt(int iSlot) { return m_aPools[iSlot].iFreeCnt; }

	//	Garbage collect
	void GarbageCollect();

#ifdef _DEBUG

	void DumpMemoryBlocks(FILE* pFile);

#endif

protected:	//	Attributes

	long		m_aThreadAtoms[MEM_SLOTNUM];	//	Atom used to ensure thread safe
	AMemoryMan*	m_pMemMan;		//	Manager manager
	DWORD		m_dwGCCounter;	//	GC counter

	AMemPool	m_aPoolMans[MEM_SLOTNUM];		//	Pool managers
	POOLSLOT	m_aPools[MEM_SLOTNUM];			//	Pool array

protected:	//	Operations

#ifdef _DEBUG

	//	Dump memory leak information
	void Dump();

	//	Dump memory leak of one pool slot
	template < class T > 
	void Dump(int iSlot, s_MEMSMALLBLK *p,int iBlkCount, int iBlkSize, T dumpFunction);

#endif

};

///////////////////////////////////////////////////////////////////////////
//
//	AMemLarge
//
///////////////////////////////////////////////////////////////////////////

class AMemLarge
{
public:		//	Types

public:		//	Constructors and Destructors

	AMemLarge()
	{
		m_BlockList		= NULL;
		m_pMemMan		= NULL;
		m_iBlockCnt		= 0;
		m_iAllocSize	= 0;
	}

	virtual ~AMemLarge()
	{
	#ifdef _DEBUG
		Dump();
	#endif
	}

public:		//	Attributes

	friend class AMemoryMan;

public:		//	Operations

	//	Allocate memory
	void* Allocate(size_t size);
	//	Free memory
	void Free(void* p);

	//	Get total size allocated by AMemLarge
	int GetAllocSize() { return m_iAllocSize; } 
	//	Get current block counter
	int GetBlockCnt() { return m_iBlockCnt; }

#ifdef _DEBUG

	void DumpMemoryBlocks(FILE* pFile);

#endif

protected:	//	Attributes

	static long		m_lThreadAtom;	//	Atom used to ensure thread safe
	AMemoryMan*		m_pMemMan;		//	Manager manager
	s_MEMLARGEBLK*	m_BlockList;	//	Block list
	int				m_iBlockCnt;	//	Block counter
	int				m_iAllocSize;	//	Total size allocated by AMemLarge

protected:	//	Operations

#ifdef _DEBUG

	//	Dump memory leak
	void Dump();

#endif
};

long AMemLarge::m_lThreadAtom = 0;

///////////////////////////////////////////////////////////////////////////
//
//	AMemoryMan
//
///////////////////////////////////////////////////////////////////////////

class AMemoryMan
{
public:		//	Types

public:		//	Constructors and Destructors

	AMemoryMan();
	virtual ~AMemoryMan();

public:		//	Attributes

public:		//	Operations

	AMemSmall* GetMemSamll() { return &m_MemSmall; }
	AMemLarge* GetMemLarge() { return &m_MemLarge; }
	ATempMemMan* GetTempMemMan() { return &m_TempMemMan; }

	//	Get next memory block's ID
	long GetNextID() { return ::InterlockedIncrement(&m_lIDCnt); }
	//	Get current memory block ID
	long GetID() { return m_lIDCnt; }
	//	Get peak size of memory
	int	GetPeakSize() { return m_iPeakSize; }
	//	Get allocation counter
	int GetAllocSize() { return m_iSizeCnt; }
	//	Get allocated raw size
	int GetAllocRawSize() { return m_lRawSizeCnt; }

	//	Add total allocated size
	void AddAllocSize(int iSize)
	{
		_MemThreadLock(&m_lThreadAtom);

		if ((m_iSizeCnt += iSize) > m_iPeakSize)
			m_iPeakSize = m_iSizeCnt;

		_MemThreadUnlock(&m_lThreadAtom);
	}

	//	Add total allocated raw size
	void AddAllocRawSize(int iSize)
	{
		::InterlockedExchangeAdd(&m_lRawSizeCnt, iSize);
	}

	//	Round up memory size
	static int RoundUp(int iSize)
	{
		if (0 == iSize)
			return MEM_ALIGN;
		else
			return (iSize + MEM_ALIGN - 1) & ~(MEM_ALIGN - 1);
	}

protected:	//	Attributes

	AMemSmall		m_MemSmall;
	AMemLarge		m_MemLarge;
	ATempMemMan		m_TempMemMan;

	long	m_lThreadAtom;	//	Atom used to ensure thread safe
	long	m_lIDCnt;		//	Allocate ID counter
	int		m_iPeakSize;	//	Peak size (maximum size of memory)
	int		m_iSizeCnt;		//	Allocate size counter
	long	m_lRawSizeCnt;	//	Raw size counter

protected:	//	Operations

};

AMemoryMan::AMemoryMan() :
m_TempMemMan(20 * 1024 * 1024)
{
	m_MemSmall.m_pMemMan = this;
	m_MemLarge.m_pMemMan = this;

	m_lIDCnt		= 0;
	m_iPeakSize		= 0;
	m_iSizeCnt		= 0;
	m_lThreadAtom	= 0;
	m_lRawSizeCnt	= 0;
}

AMemoryMan::~AMemoryMan()
{
	m_TempMemMan.Release();

#ifdef _DEBUG

	a_LogOutput(0,"Maximum memory used: %d (K)", (m_iPeakSize + 1023) / 1024);

#endif
}

///////////////////////////////////////////////////////////////////////////
//
//	Implement AMemSmall
//
///////////////////////////////////////////////////////////////////////////

AMemSmall::~AMemSmall()
{
#ifdef _DEBUG
	Dump();
#endif
}

//	Allocate memory
void* AMemSmall::Allocate(size_t size)
{
#ifdef _DEBUG
	//	Add 2 DWORDs to do slop-over checking
	int iDataSize = AMemoryMan::RoundUp(size + SLOPOVER_SIZE);
#else
	int iDataSize = AMemoryMan::RoundUp(size);
#endif

	//	Block size
	if (iDataSize > MEM_SMALLSIZE)
		return m_pMemMan->GetMemLarge()->Allocate(size);

	int iPoolSlot = (iDataSize-1) / MEM_ALIGN;

	//	Record allocated size
#ifdef _DEBUG
	m_pMemMan->AddAllocSize(GetBlockSize(iPoolSlot));
	m_pMemMan->AddAllocRawSize((int)size);
#endif

	//	------ Get free block from manager ------ 
	if (MEM_THREADSAFE)
		_MemThreadLock(&m_aThreadAtoms[iPoolSlot]);

	//	Get proper memory pool slot
	POOLSLOT& Slot = m_aPools[iPoolSlot];

	if (!Slot.pFreeBlks)
	{
		Slot.pFreeBlks = m_aPoolMans[iPoolSlot].Allocate();
		int iBlkCount  = m_aPoolMans[iPoolSlot].GetBlockCount();
		Slot.iBlockCnt += iBlkCount;
		Slot.iFreeCnt  += iBlkCount;
	}

	s_MEMSMALLBLK* pFreeBlk = Slot.pFreeBlks;
	Slot.pFreeBlks = pFreeBlk->pNext;
	Slot.iFreeCnt--;

	if (MEM_THREADSAFE)
		_MemThreadUnlock(&m_aThreadAtoms[iPoolSlot]);

	//	------ Initialize block ------ 
	char* pData = (char*)pFreeBlk + sizeof (s_MEMSMALLBLK);

#ifdef _DEBUG

	//	Fill data with special code
	memset(pData, MEM_ALLOC_FILL, size);

	pFreeBlk->iRawSize = size;

	//	Fill slop-over checking flags
	_FillSlopOverFlags(pFreeBlk->aSOFlags);
	_FillSlopOverFlags(pData + size);

#endif

	pFreeBlk->sPoolSlot = (short)iPoolSlot;
	pFreeBlk->pNext		= NULL;
	pFreeBlk->sFlag		= MEM_ALLOC_FLAG_S;

	return pData;
}

//	Free memory
void AMemSmall::Free(void *p)
{
	if (!p)
		return;

	s_MEMSMALLBLK* pBlock = _GetMemBlockInfo(p);

#ifdef _DEBUG

	/*
	BYTE* pCallers = (BYTE*)pBlock->callers;
	DWORD hash_val = 0;

	for (int i = 0; i < sizeof(pBlock->callers); i++)
	{
		hash_val = hash_val * 31 + pCallers[i];
	}

	LockMemCallersInfoModule();

	MemCallersInfoMap::iterator it = l_MemCallersInfo.GetMap().find(hash_val);

	if (it != l_MemCallersInfo.GetMap().end())
	{
		if (it->second->call_count)
		{
			it->second->call_count--;
			it->second->total_size -= (DWORD)pBlock->iRawSize;
		}
		else
			assert(false);
	}
	else
	{
		assert(false);
	}

	UnlockMemCallersInfoModule();
	*/

#endif

	if (pBlock->sFlag == MEM_FREE_FLAG_S)
	{
		//	Memory has been freed
		assert(pBlock->sFlag != MEM_FREE_FLAG_S);
		return;
	}
	else if (pBlock->sFlag == MEM_ALLOC_FLAG_L || pBlock->sFlag == MEM_FREE_FLAG_L)
	{
		m_pMemMan->GetMemLarge()->Free(p);
		return;
	}
	else if (pBlock->sFlag != MEM_ALLOC_FLAG_S)
	{
		assert(pBlock->sFlag == MEM_ALLOC_FLAG_S);
		return;
	}

	int iSlot = pBlock->sPoolSlot;
	if (iSlot < 0 || iSlot >= MEM_SLOTNUM)
	{
		assert(iSlot >= 0 && iSlot < MEM_SLOTNUM);
		return;
	}

	//	Get proper memory pool
	POOLSLOT& Slot = m_aPools[iSlot];

#ifdef _DEBUG

    DumpDeleteHistory(GetMemoryHistoryLog(), pBlock);

	//	Slop-over checking
	if (!_CheckSlopOver(pBlock->aSOFlags) ||
		!_CheckSlopOver((char*)p + pBlock->iRawSize))
	{
		assert(0 && "Memory slop over !");
	}

	//	Fill all usable buffer with special code
	memset(p, MEM_FREE_FILL, Slot.iDataSize);

	m_pMemMan->AddAllocSize(-Slot.iBlockSize);
	m_pMemMan->AddAllocRawSize(-pBlock->iRawSize);

#endif

	//	Set freed flag
	pBlock->sFlag = MEM_FREE_FLAG_S;

	//	------- Return free block to manager --------
	if (MEM_THREADSAFE)
		_MemThreadLock(&m_aThreadAtoms[iSlot]);

	s_MEMSMALLBLK** ppFreeList = &Slot.pFreeBlks;
	pBlock->pNext = *ppFreeList;
	*ppFreeList = pBlock;
	Slot.iFreeCnt++;

	m_aPoolMans[iSlot].IncGCCounter();

	if (MEM_THREADSAFE)
		_MemThreadUnlock(&m_aThreadAtoms[iSlot]);
}

//	Garbage collect
void AMemSmall::GarbageCollect()
{
	int iSlot = m_dwGCCounter % MEM_SLOTNUM;
	m_dwGCCounter++;

	AMemPool& PoolMan = m_aPoolMans[iSlot];
	POOLSLOT& PoolSlot = m_aPools[iSlot];
	int iBlkCount = PoolMan.GetBlockCount();

	//	Do garbage collection only when free block number is enough
	if (PoolMan.GetGCCounter() < MEM_GC_COUNTER ||
		PoolSlot.iFreeCnt < iBlkCount * 15)
		return;

	PoolMan.ClearGCCounter();

	//	Free block record
	struct FREEBLKLIST
	{
		s_MEMSMALLBLK* pHead;
		s_MEMSMALLBLK* pTail;
	};

	int i, iNumPoolSlot = PoolMan.GetSlotNum();
	int iTotalFree = PoolSlot.iFreeCnt;
	FREEBLKLIST TempFreeList = {NULL, NULL};
	AMemVector<int> aRemPools;

	for (i=0; i < iNumPoolSlot; i++)
	{
		if (iTotalFree <= iBlkCount * 5)
			break;

		s_MEMSMALLBLK* pStart = PoolMan.GetPool(i);
		s_MEMSMALLBLK* pEnd = (s_MEMSMALLBLK*)((BYTE*)pStart + iBlkCount * PoolMan.GetBlockSize());

		s_MEMSMALLBLK* pCur = PoolSlot.pFreeBlks;
		s_MEMSMALLBLK* pPrev = NULL;
		FREEBLKLIST PoolFreeList = {NULL, NULL};
		int iBlkCnt = 0;

		while (pCur)
		{
			if (pCur >= pStart && pCur < pEnd)
			{
				assert(iBlkCnt < iBlkCount);
				iBlkCnt++;

				//	Remove block from total free list and add it to pool free list
				s_MEMSMALLBLK* pNext = pCur->pNext;

				if (!PoolFreeList.pHead)
				{
					pCur->pNext = NULL;
					PoolFreeList.pHead = pCur;
					PoolFreeList.pTail = pCur;
				}
				else
				{
					pCur->pNext = PoolFreeList.pHead;
					PoolFreeList.pHead = pCur;
				}

				if (pPrev)
					pPrev->pNext = pNext;
				else
					PoolSlot.pFreeBlks = pNext;

				pCur = pNext;
			}
			else
			{
				pPrev = pCur;
				pCur = pCur->pNext;
			}
		}

		if (iBlkCnt == iBlkCount)
		{
			//	Ok, all blocks belong to this pool are free now.
			aRemPools.push_back(i);
			iTotalFree -= iBlkCnt;
		}
		else if (PoolFreeList.pHead)
		{
			//	Add whole pool free list to temp free list
			if (!TempFreeList.pHead)
			{
				TempFreeList.pHead = PoolFreeList.pHead;
				TempFreeList.pTail = PoolFreeList.pTail;
			}
			else
			{
				PoolFreeList.pTail->pNext = TempFreeList.pHead;
				TempFreeList.pHead = PoolFreeList.pHead;
			}
		}
	}

	//	Return temp free list
	if (TempFreeList.pHead)
	{
		TempFreeList.pTail->pNext = PoolSlot.pFreeBlks;
		PoolSlot.pFreeBlks = TempFreeList.pHead;
	}

	//	Remove all pools in remove array. 
	//	Note: we must remove from tail to head !!!
	for (i=aRemPools.size()-1; i >= 0; i--)
	{
		PoolMan.ReleasePool(aRemPools[i]);
		PoolSlot.iBlockCnt	-= iBlkCount;
		PoolSlot.iFreeCnt	-= iBlkCount;
	}
}

#ifdef _DEBUG

void _a_TouchToLogAddressSymbol(void* pAddress);
//	Dump memory usage status
void AMemSmall::DumpMemoryBlocks(FILE* pFile)
{
	struct __X__ {
		struct DumpSmallBlkToFile
		{
			DumpSmallBlkToFile(FILE * pOutFile) : m_pOutFile(pOutFile) {}
			FILE * m_pOutFile;
			void operator () (const s_MEMSMALLBLK* pBlock) const
			{
				for (int i = 0; i < MAX_CALLSTACK_LV; ++i)
				{
					fprintf(m_pOutFile, "0x%p ", pBlock->callers[i]);
					_a_TouchToLogAddressSymbol(reinterpret_cast<void*>(pBlock->callers[i]));
				}
				fprintf(m_pOutFile, "%d\n", pBlock->iRawSize);
			}
		};
	};

	__X__::DumpSmallBlkToFile dumpFunc(pFile);
	int i, j;
	for (j = 0; j < MEM_SLOTNUM; ++j)
	{
		const AMemPool& PoolMan = m_aPoolMans[j];

		int iNum = PoolMan.GetSlotNum();
		for (i = 0; i < iNum; ++i)
		{
			Dump(j, PoolMan.GetPool(i), PoolMan.GetBlockCount(), PoolMan.GetBlockSize(), dumpFunc);
		}
	}
}

//	Dump memory leak
void AMemSmall::Dump()
{
	int i, j;
	
	for (j=0; j < MEM_SLOTNUM; j++)
	{
		const AMemPool& PoolMan = m_aPoolMans[j];

		int iNum = PoolMan.GetSlotNum();
		for (i=0; i < iNum; i++)
			Dump(j, PoolMan.GetPool(i), PoolMan.GetBlockCount(), PoolMan.GetBlockSize(), DumpMemSmallBlkToALogOutput);
	}
}

//	Dump memory leak
template<class T>
void AMemSmall::Dump(int iSlot, s_MEMSMALLBLK* p,int iBlkCount, int iBlkSize, T dumpFunction)
{
	s_MEMSMALLBLK* aBlocks[MEM_POOL_MAX_BLOCK_COUNT];
	char* pBegin = (char*)p;
	int i;

	for (i=0; i < iBlkCount; i++, pBegin += iBlkSize)
		aBlocks[i] = (s_MEMSMALLBLK*)pBegin;

	pBegin = (char*)p;
	s_MEMSMALLBLK* pCur = m_aPools[iSlot].pFreeBlks;

	while (pCur)
	{
		int n = ((char*)pCur - pBegin) / iBlkSize;
		if (n >= 0 && n < iBlkCount)
			aBlocks[n] = NULL;

		pCur = pCur->pNext;
	}

	for (i=0; i < iBlkCount; i++)
	{
		if (aBlocks[i])
		{
			dumpFunction(aBlocks[i]);
		}
	}
}

#endif	//	_DEBUG

///////////////////////////////////////////////////////////////////////////
//
//	Implement AMemLarge
//
///////////////////////////////////////////////////////////////////////////

//	Allocate memory
void* AMemLarge::Allocate(size_t size)
{
#ifdef _DEBUG
	//	Add 2 DWORDs to do slop-over checking
	int iBlkSize = size + sizeof (s_MEMLARGEBLK) + SLOPOVER_SIZE;
#else
	int iBlkSize = size + sizeof (s_MEMLARGEBLK);
#endif

	s_MEMLARGEBLK* p = (s_MEMLARGEBLK*)_RawMemAlloc(iBlkSize);
	if (!p)
		return NULL;

	p->iBlkSize	 = iBlkSize;
	p->sReserved = 0;
	p->sFlag	 = MEM_ALLOC_FLAG_L;

#ifdef _DEBUG

	p->iRawSize = size;

	//	Fill data with special code
	char* pData = (char*)p + sizeof (s_MEMLARGEBLK);
	memset(pData, MEM_ALLOC_FILL, size);

	//	Fill slop-over checking flags
	_FillSlopOverFlags(p->aSOFlags);
	_FillSlopOverFlags(pData + size);

	//	Record allocated size
	m_pMemMan->AddAllocSize(iBlkSize);
	m_pMemMan->AddAllocRawSize((int)size);

	//	Get free block from manager
	if (MEM_THREADSAFE)
		_MemThreadLock(&m_lThreadAtom);

	m_iAllocSize += iBlkSize;
	m_iBlockCnt++;

	p->pPrev = NULL;
	p->pNext = m_BlockList;

	if (m_BlockList)
		m_BlockList->pPrev = p;

	m_BlockList = p;

	if (MEM_THREADSAFE)
		_MemThreadUnlock(&m_lThreadAtom);

#endif

	return ++p;
}

//	Free memory
void AMemLarge::Free(void* p)
{
	if (!p)
		return;

	s_MEMLARGEBLK* pBlock = (s_MEMLARGEBLK*)((char*)(p) - sizeof (s_MEMLARGEBLK));

#ifdef _DEBUG

	if (pBlock->sFlag != MEM_ALLOC_FLAG_L)
	{
		assert(pBlock->sFlag == MEM_ALLOC_FLAG_L);
		return;
	}

    DumpDeleteHistory(GetMemoryHistoryLog(), pBlock);

	//	Slop-over checking
	if (!_CheckSlopOver(pBlock->aSOFlags) ||
		!_CheckSlopOver((char*)p + pBlock->iRawSize))
	{
		assert(0 && "Memory slop over !");
	}

	//	Fill all usable buffer with special code
	int iUseableSize = pBlock->iBlkSize - sizeof (s_MEMLARGEBLK);
	memset(p, MEM_FREE_FILL, iUseableSize);

	m_pMemMan->AddAllocSize(-pBlock->iBlkSize);
	m_pMemMan->AddAllocRawSize(-pBlock->iRawSize);

	//	------- Unlink free block from manager -------
	if (MEM_THREADSAFE)
		_MemThreadLock(&m_lThreadAtom);

	m_iAllocSize -= pBlock->iBlkSize;
	m_iBlockCnt--;

	s_MEMLARGEBLK* pp = pBlock->pPrev;
	s_MEMLARGEBLK* pn = pBlock->pNext;

	if (pp)
		pp->pNext = pn;
	else
		m_BlockList = pn;

	if (pn)
		pn->pPrev = pp;

	if (MEM_THREADSAFE)
		_MemThreadUnlock(&m_lThreadAtom);

#endif

	_RawMemFree(pBlock);
}

#ifdef _DEBUG

void AMemLarge::DumpMemoryBlocks(FILE* pFile)
{
	s_MEMLARGEBLK* pBlock = m_BlockList;
	while (pBlock)
	{		
		for (int i = 0; i < MAX_CALLSTACK_LV; ++i)
		{
			fprintf(pFile, "0x%p ", pBlock->callers[i]);
			_a_TouchToLogAddressSymbol(reinterpret_cast<void*>(pBlock->callers[i]));
		}
		fprintf(pFile, "%d\n", pBlock->iRawSize);

		pBlock = pBlock->pNext;
	}
}

//	Dump memory leak
void AMemLarge::Dump()
{
	while (m_BlockList)
	{
		DumpMemLargeBlkToALogOutput(m_BlockList);

		m_BlockList = m_BlockList->pNext;
	}
}

#endif	//	_DEBUG

///////////////////////////////////////////////////////////////////////////
//
//	Implement
//
///////////////////////////////////////////////////////////////////////////

namespace ACommon
{
	AMemoryMan	l_MemoryMan;

	/*	Get a memory block's usable size. This function is only used internally.

		pMem: point to a memory allocated by _a_New.
	*/
	int a_MemUseableSize(void* pMem)
	{
		s_MEMSMALLBLK* pBlock = _GetMemBlockInfo(pMem);
		int iMaxSize = 0;

		if (pBlock->sFlag <= MEM_ALLOC_FLAG_S)
		{
			int iDataSize = ACommon::l_MemoryMan.GetMemSamll()->GetDataSize(pBlock->sPoolSlot);
		#ifdef _DEBUG
			iMaxSize = iDataSize - SLOPOVER_SIZE;	//	Slop-over checking flags
		#else
			iMaxSize = iDataSize;
		#endif
		}
		else if (pBlock->sFlag <= MEM_ALLOC_FLAG_L)
		{	
			s_MEMLARGEBLK* pLargeBlock = (s_MEMLARGEBLK*)((char*)pMem - sizeof (s_MEMLARGEBLK));
		#ifdef _DEBUG
			iMaxSize = pLargeBlock->iBlkSize - sizeof (s_MEMLARGEBLK) - SLOPOVER_SIZE;	//	Slop-over checking flags
		#else
			iMaxSize = pLargeBlock->iBlkSize - sizeof (s_MEMLARGEBLK);
		#endif
		}
		else
		{
			assert(0);
			return 0;
		}
	
		return iMaxSize;
	}
}

int a_GetMemPeakSize()
{
	return ACommon::l_MemoryMan.GetPeakSize();
}

DWORD a_GetMemAllocCounter()
{
	return (DWORD)ACommon::l_MemoryMan.GetID();
}

int a_GetMemCurSize()
{
	return ACommon::l_MemoryMan.GetAllocSize();
}

int a_GetMemRawSize()
{
	return ACommon::l_MemoryMan.GetAllocRawSize();
}

void a_GetSmallMemInfo(int iSlot, int* piBlkSize, int* piBlkCnt, int* piFreeCnt)
{
	using namespace ACommon;
	AMemSmall* pMem = l_MemoryMan.GetMemSamll();
	*piBlkSize	= pMem->GetBlockSize(iSlot);
	*piBlkCnt	= pMem->GetTotalBlockCnt(iSlot);
	*piFreeCnt	= pMem->GetFreeBlockCnt(iSlot);
}

void a_GetLargeMemInfo(int* piBlkCnt, int* piAllocSize)
{
	using namespace ACommon;
	AMemLarge* pMem = l_MemoryMan.GetMemLarge();
	*piBlkCnt		= pMem->GetBlockCnt();
	*piAllocSize	= pMem->GetAllocSize();
}

void a_MemGarbageCollect()
{
	using namespace ACommon;
	AMemSmall* pMem = l_MemoryMan.GetMemSamll();
	pMem->GarbageCollect();
}

void a_ExportMemLog(const char* szPath)
{
	using namespace ACommon;

	FILE* pFile = fopen(szPath,"wt"); 
	if(!pFile)
		return;

	int nSize = 0;
	char szBuf[256];
	memset(szBuf,0,256);
	////////////////////MemLog//////////////////////
	nSize = sprintf_s(szBuf,256,"////////////////////MemLog//////////////////////\n");
	fwrite(szBuf,sizeof(char),nSize,pFile);

#ifdef _DEBUG
	nSize = sprintf_s(szBuf,256,"PeakSize=\t%d\n",l_MemoryMan.GetPeakSize());
	fwrite(szBuf,sizeof(char),nSize,pFile);
	nSize = sprintf_s(szBuf,256,"AllocCounter=\t%d\n",l_MemoryMan.GetID());
	fwrite(szBuf,sizeof(char),nSize,pFile);
	nSize = sprintf_s(szBuf,256,"CurSize=\t%d\n",l_MemoryMan.GetAllocSize());
	fwrite(szBuf,sizeof(char),nSize,pFile);
	nSize = sprintf_s(szBuf,256,"RawSize=\t%d\n",l_MemoryMan.GetAllocRawSize());
	fwrite(szBuf,sizeof(char),nSize,pFile);
#else

#endif

	nSize = sprintf_s(szBuf,256,"\n");
	fwrite(szBuf,sizeof(char),nSize,pFile);
	////////////////////MemLarge////////////////////
	AMemLarge* pMemLarge = l_MemoryMan.GetMemLarge();
	nSize = sprintf_s(szBuf,256,"////////////////////MemLarge////////////////////\n");
	fwrite(szBuf,sizeof(char),nSize,pFile);

#ifdef _DEBUG
	int iLMemAllocSize = pMemLarge->GetAllocSize();
	int iLMemBlkCnt    = pMemLarge->GetBlockCnt();
	nSize = sprintf_s(szBuf,256,"LargeTotalBlkSize= %-20d, LargeTotalBlkCount= %-12d\n",iLMemAllocSize,iLMemBlkCnt);
	fwrite(szBuf,sizeof(char),nSize,pFile);
#else
	
#endif
	
	nSize = sprintf_s(szBuf,256,"\n");
	fwrite(szBuf,sizeof(char),nSize,pFile);
	////////////////////MemSmall////////////////////
	AMemSmall* pMemSmall = l_MemoryMan.GetMemSamll();
	nSize = sprintf_s(szBuf,256,"////////////////////MemSmall////////////////////\n");
	fwrite(szBuf,sizeof(char),nSize,pFile);

#ifdef _DEBUG
	int iSMemAllocSize		= 0;
	int iSMemBlkCnt			= 0;
	int iSMemAllocFreeSize	= 0;
	int iSMemBlkCntFree		= 0;
	for (int i=0; i < MEM_SLOTNUM; i++)
	{
		int iPoolSlotNum		= pMemSmall->GetPoolSlotNum(i);
		int iPoolSlotBlkCount	= pMemSmall->GetPoolSlotBlockNum(i);
		int iBlkSize     		= pMemSmall->GetBlockSize(i);
		int iDataSize			= pMemSmall->GetDataSize(i);
		int iBlkCount    		= pMemSmall->GetTotalBlockCnt(i);
		int iBlkFree     		= pMemSmall->GetFreeBlockCnt(i);
		int iTotal       		= iBlkSize * iBlkCount;
		iSMemAllocSize		+= iTotal;
		iSMemBlkCnt			+= iBlkCount;

		iSMemAllocFreeSize	+= iBlkSize * iBlkFree;
		iSMemBlkCntFree		+= iBlkFree;

		nSize = sprintf_s(szBuf,256,"PoolSlotNum= %-8d, PoolSlotBlkCount= %-8d, PoolSlotDataSize= %-4d, BlkSize= %-4d, BlkCount= %-12d, BlkTotalSize= %-12d, BlkFree= %-12d\n",iPoolSlotNum,iPoolSlotBlkCount,iDataSize,iBlkSize,iBlkCount,iTotal,iBlkFree);
		fwrite(szBuf,sizeof(char),nSize,pFile);
	}

	nSize = sprintf_s(szBuf,256,"SmallTotalBlkSize= %-20d, SmallTotalBlkCount= %-12d\n",iSMemAllocSize,iSMemBlkCnt);
	fwrite(szBuf,sizeof(char),nSize,pFile);

	nSize = sprintf_s(szBuf,256,"SmallFreeTotalBlkSize= %-20d, SmallFreeTotalBlkCount= %-12d\n",iSMemAllocFreeSize,iSMemBlkCntFree);
	fwrite(szBuf,sizeof(char),nSize,pFile);
#else
	
#endif

	fclose(pFile);
}

///////////////////////////////////////////////////////////////////////////
//
//	Memory operation
//
///////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
			  
void* _a_New_Debug(size_t size)
{
	char* pData = (char*)(ACommon::l_MemoryMan.GetMemSamll()->Allocate(size));
	s_MEMSMALLBLK* pBlock = _GetMemBlockInfo(pData);

	// now do a call stack backtrace
	DWORD frame_cur = 0;
	int c = 0;
	const int skip = 1;

	__asm
	{
		mov frame_cur, ebp
	}

	memset(pBlock->callers, 0, sizeof(pBlock->callers));
	while (!IsBadReadPtr((LPVOID)(4+frame_cur), 4))
	{
		DWORD thisCall = *(DWORD*)(frame_cur + 4);
		if (c >= skip)
			pBlock->callers[c-skip] = thisCall;

		c++;

		if (c >= MAX_CALLSTACK_LV + skip || thisCall == 0)
			break;

		DWORD frame_up = *(DWORD *)frame_cur;
		if (frame_up <= frame_cur)
			break;

		frame_cur = frame_up;
	}

	/*
	BYTE* pCallers = (BYTE*)pBlock->callers;
	DWORD hash_val = 0;

	for (int i = 0; i < sizeof(pBlock->callers); i++)
	{
		hash_val = hash_val * 31 + pCallers[i];
	}

	LockMemCallersInfoModule();
	MemCallersInfoMap::iterator it = l_MemCallersInfo.GetMap().find(hash_val);

	if (it != l_MemCallersInfo.GetMap().end())
	{
		it->second->call_count++;
		it->second->total_size += (DWORD)pBlock->iRawSize;
	}
	else
	{
		s_MEM_CALLER_INFO* pInfo = (s_MEM_CALLER_INFO*)malloc(sizeof(s_MEM_CALLER_INFO));
		memcpy(pInfo->callers, pBlock->callers, 32);
		pInfo->call_count = 1;
		pInfo->total_size = pBlock->iRawSize;
		(l_MemCallersInfo.GetMap())[hash_val] = pInfo;
	}

	UnlockMemCallersInfoModule();
	*/

    DumpAllocHistory(GetMemoryHistoryLog(), pBlock);

	return pData;
}

//	Reallocate, pMem must point to a mmeory block that is allocated by
//	a_malloc or a_realloc.
void* _a_Realloc_Debug(void* pMem, size_t size)
{
	if (!size && pMem)
	{
		a_free(pMem);
		return NULL;
	}

	if (!pMem)	//	Act as malloc
		return _a_New_Debug(size);

	//	Get old memory block
	DWORD dwMaxSize = (DWORD)ACommon::a_MemUseableSize(pMem);

	//	If old memory block is big enough, return memory block unchanged.
	if (size <= dwMaxSize)
	{
		s_MEMSMALLBLK* pBlock = _GetMemBlockInfo(pMem);

		int iDelta = (int)size - pBlock->iRawSize;
		ACommon::l_MemoryMan.AddAllocRawSize(iDelta);

		pBlock->iRawSize = size;

		//	Re-fill slop-over checking flag at data tail
		BYTE* pData = (BYTE*)pMem;
		_FillSlopOverFlags(pData + size);

		return pMem;
	}

	//	Allocate new memory
	void* pNewMem = _a_New_Debug(size);
	if (!pNewMem)
		return NULL;

	if (pNewMem != pMem)
	{
		//	Duplicate data
		memcpy(pNewMem, pMem, dwMaxSize);
		_a_Delete(pMem);
	}

	return pNewMem;
}

//	Allocate a aligned memory block.
void* _a_NewAlign_Debug(size_t size, int align)
{
	int tsize = size + align + sizeof (int);
	char* data = (char*)_a_New_Debug(tsize);
	char* data2;

	if (data)
	{
		if ((unsigned int)(data) % align)
			data2 = (char*)(((unsigned int)data & (~(align -1))) + align);
		else
			data2 = data + ((align > sizeof (int)) ? align : sizeof (int));
		
		if (data2 - data < sizeof (int))
			data2 += align;

		*(int*)(data2 - sizeof (int)) = data2 - data;

		return data2;
	}

	return NULL;
}

#else	//	Release version

void* _a_New_Release(size_t size)
{
	return ACommon::l_MemoryMan.GetMemSamll()->Allocate(size);
}

//	Reallocate, pMem must point to a mmeory block that is allocated by
//	a_malloc or a_realloc.
void* _a_Realloc_Release(void* pMem, size_t size)
{
	if (!size && pMem)
	{
		a_free(pMem);
		return NULL;
	}

	if (!pMem)	//	Act as malloc
		return _a_New_Release(size);

	//	Get old memory block
	DWORD dwMaxSize = (DWORD)ACommon::a_MemUseableSize(pMem);

	//	If old memory block is big enough, return memory block unchanged.
	if (size <= dwMaxSize)
		return pMem;

	//	Allocate new memory
	void* pNewMem = _a_New_Release(size);
	if (!pNewMem)
		return NULL;

	if (pNewMem != pMem)
	{
		//	Duplicate data
		memcpy(pNewMem, pMem, dwMaxSize);
		_a_Delete(pMem);
	}

	return pNewMem;
}

//	Allocate a aligned memory block.
void* _a_NewAlign_Release(size_t size, int align)
{
	int tsize = size + align + sizeof (int);
	char* data = (char*)_a_New_Release(tsize);
	char* data2;

	if (data)
	{
		if ((unsigned int)(data) % align)
			data2 = (char*)(((unsigned int)data & (~(align -1))) + align);
		else
			data2 = data + ((align > sizeof (int)) ? align : sizeof (int));

		if (data2 - data < sizeof (int))
			data2 += align;

		*(int*)(data2 - sizeof (int)) = data2 - data;

		return data2;
	}

	return NULL;
}

#endif	//	_DEBUG

void* _a_New(size_t size)
{
#ifndef _A_FORBID_MALLOC
	#ifdef _DEBUG
		return _a_New_Debug(size);
	#else
		return _a_New_Release(size);
	#endif
#else
	return malloc(size);
#endif
}

void* _a_Realloc(void* pMem, size_t size)
{
#ifndef _A_FORBID_MALLOC
	#ifdef _DEBUG
		return _a_Realloc_Debug(pMem, size);
	#else
		return _a_Realloc_Release(pMem, size);
	#endif
#else
	return realloc(pMem, size);
#endif
}

void* _a_NewAlign(size_t size, int align)
{
#ifndef _A_FORBID_MALLOC
	#ifdef _DEBUG
		return _a_NewAlign_Debug(size, align);
	#else
		return _a_NewAlign_Release(size, align);
	#endif
#else
	return NULL;	//	TODO: to implement it
#endif
}

void _a_Delete(void *p)
{
#ifndef _A_FORBID_MALLOC
	ACommon::l_MemoryMan.GetMemSamll()->Free(p);
#else
	free(p);
#endif
}

//	Free a aligned memory block allocated by _a_NewAlign
void _a_DeleteAlign(void* pMem)
{
#ifndef _A_FORBID_MALLOC
	char* data2;
	data2  = (char*)pMem;
	data2 -= *(int*)(data2 - sizeof (int));
	_a_Delete(data2);
#else
	ASSERT(0 && "_a_DeleteAlign isn't implemented.");
#endif
}

void* _a_MallocTemp(size_t size)
{
#ifndef _A_FORBID_MALLOC
	return ACommon::l_MemoryMan.GetTempMemMan()->Alloc(size);
#else
	return malloc(size);
#endif
}

void _a_FreeTemp(void* p)
{
#ifndef _A_FORBID_MALLOC
	ACommon::l_MemoryMan.GetTempMemMan()->Free(p);
#else
	free(p);
#endif
}

#ifndef _A_FORBID_NEWDELETE

void* operator new (size_t size)
{
#ifdef _DEBUG
	return _a_New_Debug(size);
#else
	return _a_New_Release(size);
#endif
}

void operator delete (void *p)
{
	ACommon::l_MemoryMan.GetMemSamll()->Free(p);
}

void* operator new [] (size_t size)
{
#ifdef _DEBUG
	return _a_New_Debug(size);
#else
	return _a_New_Release(size);
#endif
}

void operator delete [] (void *p)
{
	ACommon::l_MemoryMan.GetMemSamll()->Free(p);
}

#ifdef _DEBUG

void* operator new (size_t size, const char* szFile, int iLine)
{
	return _a_New_Debug(size);
}

void operator delete (void* p, const char* szFile, int iLine)
{
	ACommon::l_MemoryMan.GetMemSamll()->Free(p);
}

void* operator new [] (size_t size, const char* szFile, int iLine)
{
	return _a_New_Debug(size);
}

void operator delete [] (void* p, const char* szFile, int iLine)
{
	ACommon::l_MemoryMan.GetMemSamll()->Free(p);
}

#endif	//	_DEBUG
#endif	//	_A_FORBID_NEWDELETE

///////////////////////////////////////////////////////////////////////////
//
//	Symbol Info operation
//
///////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#include "hashmap.h"
#include "imagehlp.h"
#include <tchar.h>
#include <Shlwapi.h>

struct SymbloInfo
{
	const void* pAddr;
	CHAR		Name[128];
	CHAR		FileName[128];
	DWORD       LineNumber;
};
typedef abase::hash_map<void*, SymbloInfo> SymbloInfoMap;
static SymbloInfoMap _addr_info_map;

void _a_Dump_AdditionalInfoMap(FILE* pAddiInfoFile)
{
	for (SymbloInfoMap::iterator itr = _addr_info_map.begin()
		; itr != _addr_info_map.end()
		; ++itr)
	{
		fprintf(pAddiInfoFile, "0x%p %s\n(%d): %s\n", itr->first, itr->second.FileName, itr->second.LineNumber, itr->second.Name);
	}
}
void _a_Dump_AdditionalInfo(FILE* pAddiInfoFile)
{
	//	Additional information
	int iTotalFreeBytes = 0;
	for (int i = 0; i < MEM_SLOTNUM; ++i)
	{
		int iBlkSize, iBlkCnt, iFreeCnt;
		a_GetSmallMemInfo(i, &iBlkSize, &iBlkCnt, &iFreeCnt);
		fprintf (pAddiInfoFile, "%d slot has %d free bytes\n", iBlkSize, iBlkSize * iFreeCnt);
		iTotalFreeBytes += iBlkSize * iFreeCnt;
	}
	fprintf(pAddiInfoFile, "Totally %d free bytes\n", iTotalFreeBytes);

	fprintf (pAddiInfoFile, "----- ----- ----- -----\n");

	_a_Dump_AdditionalInfoMap(pAddiInfoFile);
}

void _a_DumpMemoryBlocks(FILE* pFile, FILE* pAddiInfoFile)
{
	if (!pFile)
		return;

	ACommon::l_MemoryMan.GetMemLarge()->DumpMemoryBlocks(pFile);
	ACommon::l_MemoryMan.GetMemSamll()->DumpMemoryBlocks(pFile);

	_a_Dump_AdditionalInfo(pAddiInfoFile);
}

#endif


#ifdef _DEBUG

static HANDLE _LoadSymForCurrentProcess( const char* UserSearchPath )
{
    static HANDLE hProcess;
    static const  int bufsz = 256;
    static char   szLastPath[bufsz];

    if( !hProcess || (UserSearchPath && strncmp(szLastPath, UserSearchPath, bufsz-1)) )
    {
        if (UserSearchPath)
        {
            strncpy( szLastPath, UserSearchPath, bufsz-1 );
        }
        else
            szLastPath[0] = 0;
        szLastPath[bufsz-1] = 0;

        hProcess = GetCurrentProcess();
        DWORD flags = SymSetOptions(SYMOPT_LOAD_LINES);
        bool bSuccess = SymInitialize(hProcess, (char*)UserSearchPath, TRUE) ? true : false;
//        assert(bSuccess);
    }

    return hProcess;
}


static SymbloInfo _AddressToSymbol(const void* pAddr, HANDLE hProcess )
{
    SymbloInfo si;
    memset(&si, 0, sizeof(si));
    si.pAddr = pAddr;

    DWORD  error;
    DWORD  dwAddress = (DWORD)pAddr;
    ULONG64 buffer[(sizeof(SYMBOL_INFO) +
        MAX_SYM_NAME * sizeof(TCHAR) +
        sizeof(ULONG64) - 1) / 
        sizeof(ULONG64)];
    PSYMBOL_INFO pSymbol = (PSYMBOL_INFO) buffer;

    pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
    pSymbol->MaxNameLen = MAX_SYM_NAME;
    DWORD64 dw64Offset;
    if (SymFromAddr(hProcess, dwAddress, &dw64Offset, pSymbol))
    {
        _tcsncpy(si.Name, pSymbol->Name, sizeof(si.Name));
    }
    else
    {
        error = GetLastError();
    }

    IMAGEHLP_LINE line;
    line.SizeOfStruct = sizeof(IMAGEHLP_LINE);
    DWORD dwOffset = (DWORD)dw64Offset;
    if (SymGetLineFromAddr(hProcess, dwAddress, &dwOffset, &line))
    {
        strncpy(si.FileName, line.FileName, sizeof(si.FileName));
        si.LineNumber = line.LineNumber;
    }
    else
    {
        error = GetLastError();
    }

    return si;
}

void _a_TouchToLogAddressSymbol(void* pAddress)
{
	if (pAddress == NULL)
		return;

	static HANDLE hProcess = NULL;
	if (hProcess == NULL)
	{
		char szFileName[MAX_PATH];
		GetModuleFileNameA(NULL, szFileName, sizeof(szFileName));
		if (LPSTR pStr = PathFindFileNameA(szFileName))
			*pStr = NULL;
		hProcess = _LoadSymForCurrentProcess(szFileName);
	}

	if (_addr_info_map.find(pAddress) == _addr_info_map.end())
	{
		_addr_info_map[pAddress] = _AddressToSymbol(pAddress, hProcess);
	}
}


static void ExportSymbolInfo(void * ptr)
{
    char szFileName[MAX_PATH];
    GetModuleFileNameA(NULL, szFileName, sizeof(szFileName));
    if (LPSTR pStr = PathFindFileNameA(szFileName))
        *pStr = NULL;
    HANDLE hProcess = _LoadSymForCurrentProcess(szFileName);
    SymbloInfo si;
    si = _AddressToSymbol(ptr, hProcess);

    char szMsg[1024];
	if(si.LineNumber == 0)
	{
		_snprintf(szMsg, sizeof(szMsg), " %s -- 0x%p\n", si.Name, si.pAddr);
	}
	else
	{
		_snprintf(szMsg, sizeof(szMsg), " %s (%d): %s -- 0x%p\n", si.FileName, si.LineNumber, si.Name, si.pAddr);
	}
    OutputDebugStringA(szMsg);
}

#endif

///////////////////////////////////////////////////////////////////////

// added according to angelic 2.0
#ifdef _DEBUG

static FILE * g_pLogFile;
void _a_EnableMemoryHistoryLog(FILE * pMemHisFile)
{
    g_pLogFile = pMemHisFile;
}

FILE * GetMemoryHistoryLog()
{
    return g_pLogFile;
}

#endif	//	_DEBUG

void DumpDeleteHistory(FILE* pf, const s_MEMSMALLBLK* pBlock)
{
#ifdef _DEBUG
    if (!pf)
        return;

    char szMsg[1024] = {0};
    sprintf(szMsg, "D %d %d ", timeGetTime(), pBlock->iRawSize);
    for (int i = 0; i < MAX_CALLSTACK_LV; ++i)
    {
        char szPart[64] = {0};
        sprintf(szPart, "0x%p ", pBlock->callers[i]);
        strcat(szMsg, szPart);
    }

    fprintf(pf, "%s\n", szMsg);
#endif
}

void DumpDeleteHistory(FILE* pf, const s_MEMLARGEBLK* pBlock)
{
#ifdef _DEBUG
    if (!pf)
        return;

    char szMsg[1024] = {0};
    sprintf(szMsg, "D %d %d ", timeGetTime(), pBlock->iRawSize);
    for (int i = 0; i < MAX_CALLSTACK_LV; ++i)
    {
        char szPart[64] = {0};
        sprintf(szPart, "0x%p ", pBlock->callers[i]);
        strcat(szMsg, szPart);
    }

    fprintf(pf, "%s\n", szMsg);	
#endif
}

void DumpAllocHistory(FILE* pf, const s_MEMSMALLBLK* pBlock)
{
#ifdef _DEBUG
    if (!pf)
        return;

    char szMsg[1024] = {0};
    sprintf(szMsg, "A %d %d ", timeGetTime(), pBlock->iRawSize);
    for (int i = 0; i < MAX_CALLSTACK_LV; ++i)
    {
        char szPart[64] = {0};
        sprintf(szPart, "0x%p ", pBlock->callers[i]);
        strcat(szMsg, szPart);
    }

    fprintf(pf, "%s\n", szMsg);	
#endif
}





