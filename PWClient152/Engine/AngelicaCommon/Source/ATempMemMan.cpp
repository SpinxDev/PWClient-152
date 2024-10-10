#include "ATempMemMan.h"
#include "AMemory.h"
#include <ACSWrapper.h>
#include <assert.h>

ATempMemMan::ATempMemMan(int nBlockSize /*= 5*1024*1024*/)
	: m_pBuffer(0)
	, m_pFreeNode(0)
	, m_nBlockSize(nBlockSize)
	, m_nAllocTimes(0)
	, m_nFreeTimes(0)
	, m_bIsInit(false)
{
	assert(m_nBlockSize > 0 && "ATempMemMan::ATempMemMan");

	InitializeCriticalSection(&m_csBuffer);
}

ATempMemMan::~ATempMemMan()
{
	Release();
}

void	ATempMemMan::Release()
{
	if(!m_bIsInit)
		return;

	//	Check malloc and free times
	ASSERT(m_nAllocTimes == m_nFreeTimes);

	if(m_pBuffer)
	{
		a_free(m_pBuffer);
		m_pBuffer = NULL;
	}
	m_pFreeNode = 0;

	DeleteCriticalSection(&m_csBuffer);
	m_bIsInit = false;
}

bool	ATempMemMan::_init(int nBlockSize)
{
	if(m_bIsInit)
		return true;

	m_pBuffer   = (unsigned char*)a_malloc(m_nBlockSize);
	m_pFreeNode = (Memory_Node*)m_pBuffer;
	m_pFreeNode->nSize	 = m_nBlockSize - sizeof(Memory_Node);	
	m_pFreeNode->pPrev	 = 0;
	m_pFreeNode->pNext	 = 0;
	
	m_bIsInit = true;
	return true;
}

void*	ATempMemMan::Alloc(size_t nSize)
{
	ACSWrapper cs(&m_csBuffer);

	if (!m_bIsInit)
	{
		if (!_init(m_nBlockSize))
			return NULL;
	}

	m_nAllocTimes++;

	void* pData = 0;
	bool bGlobalMalloc = false;
	if(nSize <= 0 || nSize > m_nBlockSize-sizeof(Memory_Node))
	{
		bGlobalMalloc = true;
	}
	else
	{
		if(m_pFreeNode)
		{
			Memory_Node* pFreeNode = m_pFreeNode;
			if((int)nSize > pFreeNode->nSize)
			{
				pFreeNode = _findFreeNode(pFreeNode,nSize);
			}
			
			if(pFreeNode)
			{
				pData	= ((unsigned char*)pFreeNode) + sizeof(Memory_Node); 
				pFreeNode->nSize   = -(int)nSize;
				m_pFreeNode = _arrangeFreeNode(pFreeNode,nSize);
			}
		}

		if(!pData)
		{
			bGlobalMalloc = true;
		}
	}

	if(bGlobalMalloc)
	{
		pData = a_malloc(nSize);
	}

	return pData;
}

ATempMemMan::Memory_Node* ATempMemMan::_arrangeFreeNode(Memory_Node* pNode,size_t nSize)
{
	assert(pNode && "ATempMemMan::_arrangeFreeNode");

	Memory_Node* pRet  = 0;
	Memory_Node* pNext = pNode->pNext;

	unsigned char* pBufTail = 0;

	if(pNext)
	{
		pBufTail = (unsigned char*)pNext;
	}
	else
	{	
		pBufTail = m_pBuffer+m_nBlockSize;
	}

	unsigned char* pData = (unsigned char*)pNode+sizeof(Memory_Node);
	if(pData+nSize+sizeof(Memory_Node) < pBufTail)
	{
		pRet = _addNewNode(pData+nSize,pBufTail);
		pNode->pNext = pRet;
		pRet->pPrev	 = pNode;
		if(pNext)
		{
			pRet->pNext	 = pNext;
			pNext->pPrev = pRet;
		}
	}
	else
	{
		pRet = _findFreeNode(pNode);
	}

	return pRet;
}

ATempMemMan::Memory_Node*	ATempMemMan::_addNewNode(unsigned char* pBuf,unsigned char* pBuf_Tail)
{	
	Memory_Node* pNewNode = (Memory_Node*)pBuf;
	pNewNode->nSize	   = size_t(pBuf_Tail - pBuf - sizeof(Memory_Node));	
	pNewNode->pPrev	   = 0;
	pNewNode->pNext	   = 0;

	return pNewNode;
}

ATempMemMan::Memory_Node* ATempMemMan::_findFreeNode(Memory_Node* pNode)
{
	assert(pNode && "ATempMemMan::_findFreeNode");
	Memory_Node* pRet   = 0;
	
	Memory_Node* pPrev  = pNode->pPrev;
	while (pPrev)
	{
		if(!pPrev->IsUsed())	
		{
			pRet = pPrev;
			break;
		}

		pPrev = pPrev->pPrev;
	}

	if(!pRet)
	{
		Memory_Node* pNext = pNode->pNext;
		while(pNext)
		{
			if(!pNext->IsUsed())	
			{
				pRet = pNext;
				break;
			}

			pNext = pNext->pNext;
		}
	}

	return pRet;
}

ATempMemMan::Memory_Node* ATempMemMan::_findFreeNode(Memory_Node* pNode,size_t nSize)
{
	assert(pNode && "ATempMemMan::_findFreeNode");
	Memory_Node* pRet   = 0;

	Memory_Node* pPrev  = pNode->pPrev;
	while (pPrev)
	{
		if(!pPrev->IsUsed() && pPrev->nSize > (int)nSize)	
		{
			pRet = pPrev;
			break;
		}

		pPrev = pPrev->pPrev;
	}

	if(!pRet)
	{
		Memory_Node* pNext = pNode->pNext;
		while(pNext)
		{
			if(!pNext->IsUsed() && pNext->nSize > (int)nSize)	
			{
				pRet = pNext;
				break;
			}

			pNext = pNext->pNext;
		}
	}

	return pRet;
}

void	ATempMemMan::Free(void* p)
{
	ACSWrapper cs(&m_csBuffer);
	m_nFreeTimes++;

	if(!p)
		return;

	if(p < m_pBuffer || p >= m_pBuffer+m_nBlockSize)
	{
		a_free(p);
		return;
	}

	Memory_Node* pNode = (Memory_Node*)((unsigned char*)p - sizeof(Memory_Node));
	assert(pNode != NULL && pNode->IsUsed() && "ATempMemMan::Free");
	pNode->nSize = -pNode->nSize;

	bool bMerge = false;
	pNode = _mergeNode(pNode,bMerge);
	if(!m_pFreeNode || bMerge)
	{
		m_pFreeNode = pNode;
	}
}

ATempMemMan::Memory_Node*	ATempMemMan::_mergeNode(Memory_Node* pNode,bool& bMerge)
{
	bool bMergePrev = false;
	bool bMergeNext = false;
	Memory_Node* pMergePrevNode = _mergePrevNode(pNode,bMergePrev);
	Memory_Node* pMergeNextNode	= _mergeNextNode(pMergePrevNode,bMergeNext);

	if(bMergePrev || bMergeNext)
		bMerge = true;
	else
		bMerge = false;

	return pMergeNextNode;
}

ATempMemMan::Memory_Node*	ATempMemMan::_mergePrevNode(Memory_Node* pNode,bool& bMerge)
{
	Memory_Node* pRet  = pNode;
	Memory_Node* pPrev = pNode->pPrev;
	if(pPrev && !pPrev->IsUsed())
	{
		bMerge = true;

		Memory_Node* pNext = pNode->pNext;

		pPrev->nSize += (pNode->nSize + sizeof(Memory_Node));
		pPrev->pNext = pNext;

		if(pNext)
			pNext->pPrev = pPrev;

		pRet = pPrev;
		
		//impossible case

		//while(pPrev)
		//{
		//	bool bM = false;
		//	pRet = _mergePrevNode(pPrev,bM);
		// 	if(pRet == pPrev)
		// 		break;
		// 
		// 	pPrev = pPrev->pPrev;
		//}
	}

	return pRet;
}

ATempMemMan::Memory_Node*	ATempMemMan::_mergeNextNode(Memory_Node* pNode,bool& bMerge)
{
	Memory_Node* pRet  = pNode;
	Memory_Node* pNext = pNode->pNext;
	if(pNext && !pNext->IsUsed())
	{
		bMerge = true;

		Memory_Node* pNextNext = pNext->pNext;

		pNode->nSize += (pNext->nSize + sizeof(Memory_Node));
		pNode->pNext  = pNextNext;

		if(pNextNext)
			pNextNext->pPrev = pNode;

		pRet = pNode;

		//impossible case

		//while(pNextNext)
		//{
		//	bool bM = false;
		//	pRet = _mergeNextNode(pNode,bM);
		//	if(pRet == pNode)
		//		break;
		//
		//	pNextNext = pNode->pNext;
		//}
	}

	return pRet;
}