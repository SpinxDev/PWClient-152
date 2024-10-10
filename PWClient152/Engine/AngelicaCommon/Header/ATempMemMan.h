/*
 * FILE: ATempMemMan.h
 *
 * DESCRIPTION: temp memory manager
 *
 * CREATED BY: doujianwei, 2012/9/6
 */

#ifndef _ATEMPMEMMAN_H_
#define _ATEMPMEMMAN_H_

#include "ACPlatform.h"

class ATempMemMan
{
public:
	ATempMemMan(int nBlockSize = 5*1024*1024);
	~ATempMemMan();
	
//Êý¾Ý
public:
	struct Memory_Node
	{
		int				nSize;
		Memory_Node*	pPrev;		
		Memory_Node*	pNext;		
		
		bool			IsUsed()		{ return (nSize < 0); }
	};
	
protected:
	unsigned char*		m_pBuffer;
	Memory_Node*		m_pFreeNode;
	CRITICAL_SECTION	m_csBuffer;
	
private:
	int				m_nBlockSize;
	bool			m_bIsInit;

	size_t			m_nAllocTimes;
	size_t			m_nFreeTimes;
	
//²Ù×÷
public:
	bool		IsInit()			{ return m_bIsInit; }
	
	size_t		GetAllocTimes()		{ return m_nAllocTimes; }
	size_t		GetFreeTimes()		{ return m_nFreeTimes; }
	
public:
	void		Release();
	
	void*		Alloc(size_t nSize);	
	void		Free(void* p);

private:
	bool		_init(int nBlockSize);
	
private:
	Memory_Node* _addNewNode(unsigned char* pBuf,unsigned char* pBuf_Tail);
	
	Memory_Node* _arrangeFreeNode(Memory_Node* pNode,size_t nSize);
	Memory_Node* _findFreeNode(Memory_Node* pNode);
	Memory_Node* _findFreeNode(Memory_Node* pNode,size_t nSize);
	
	Memory_Node* _mergeNode(Memory_Node* pNode,bool& bMerge);
	Memory_Node* _mergePrevNode(Memory_Node* pNode,bool& bMerge);
	Memory_Node* _mergeNextNode(Memory_Node* pNode,bool& bMerge);
};

#endif