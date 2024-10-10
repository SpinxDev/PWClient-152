
#include "AList.h"
#include "AMemory.h"

#define new A_DEBUG_NEW

AList::AList()
{
	m_pHead = m_pTail = NULL;
	m_nSize = 0;
}

AList::~AList()
{
	
}

bool AList::Init()
{
	m_pHead = (ALISTELEMENT *)a_malloc(sizeof(ALISTELEMENT));
	if( NULL == m_pHead )
		return false;

	m_pTail = (ALISTELEMENT *)a_malloc(sizeof(ALISTELEMENT));
	if( NULL == m_pTail )
		return false;

	m_pHead->pData = m_pTail->pData = NULL;
	m_pHead->pLast = m_pTail->pNext = NULL;
	m_pHead->pNext = m_pTail;
	m_pTail->pLast = m_pHead;
	m_nSize = 0;
	return true;
}

bool AList::Release()
{
	ALISTELEMENT * pThisElement = m_pHead;
	ALISTELEMENT * pElementToDel;

	//We do not carry out the work to release Element.pData, 
	//That is the caller's task :(
	while( pThisElement )
	{
		pElementToDel = pThisElement;
		pThisElement = pThisElement->pNext;

		a_free(pElementToDel);
	}

	m_pHead = m_pTail = NULL;
	m_nSize = 0;
	return true;
}

bool AList::Append(void* pDataToAppend, ALISTELEMENT ** ppElement)
{
	return Insert(pDataToAppend, m_pTail, ppElement);
}
	
bool AList::Insert(void* pDataToInsert, ALISTELEMENT * pElement, ALISTELEMENT ** ppElement)
{
	ALISTELEMENT * pNewElement;
	if( NULL == pElement )
		return false;

	pNewElement = (ALISTELEMENT *)a_malloc(sizeof(ALISTELEMENT));
	if( NULL == pNewElement )
		return false;

	pNewElement->pData = pDataToInsert;

	pElement->pLast->pNext = pNewElement;
	pNewElement->pLast = pElement->pLast;
	pNewElement->pNext = pElement;
	pElement->pLast = pNewElement;
	
	if( ppElement )
		*ppElement = pNewElement;

	m_nSize ++;
	return true;
}
	
bool AList::Insert(void* pDataToInsert, void* pDataInsertBefore, ALISTELEMENT ** ppElement)
{
	if( NULL == pDataInsertBefore )
		return false;

	ALISTELEMENT * pElement = FindElementByData(pDataInsertBefore);
	if( NULL == pElement )
		return false;

	return Insert(pDataToInsert, pElement, ppElement);
}
	
bool AList::Delete(ALISTELEMENT * pElement)
{
	if( NULL == pElement )
		return false;

	pElement->pLast->pNext = pElement->pNext;
	pElement->pNext->pLast = pElement->pLast;

	a_free(pElement);
	m_nSize --;
	return true;
}
	
bool AList::Delete(void* pData)
{
	ALISTELEMENT * pElement;
	if( NULL == pData )
		return false;

	pElement = FindElementByData(pData);
	if( NULL == pElement )
		return false;

	return Delete(pElement);
}

ALISTELEMENT * AList::FindElementByData(void* pData)
{
	ALISTELEMENT * pThisElement = m_pHead->pNext;

	while(pThisElement != m_pTail)
	{
		if( pThisElement->pData == pData )
			return pThisElement;
		else
			pThisElement = pThisElement->pNext;
	}

	//Nothing be found;
	return NULL;
}

bool AList::IsValid(ALISTELEMENT * pElement)
{
	ALISTELEMENT * pThisElement = m_pHead->pNext;

	while( pThisElement != m_pTail )
	{
		if( pThisElement == pElement )
			return true;
		else
			pThisElement = pThisElement->pNext;
	}

	return false;
}

bool AList::Reset()
{
	ALISTELEMENT * pThisElement = m_pHead->pNext;
	ALISTELEMENT * pElementToDel;

	//We do not carry out the work to release Element.pData, 
	//That is the caller's task :(
	while( pThisElement != m_pTail )
	{
		pElementToDel = pThisElement;
		pThisElement = pThisElement->pNext;

		a_free(pElementToDel);
	}

	m_pHead->pData = m_pTail->pData = NULL;
	m_pHead->pLast = m_pTail->pNext = NULL;
	m_pHead->pNext = m_pTail;
	m_pTail->pLast = m_pHead;
	m_nSize = 0;
	return true;
}

ALISTELEMENT * AList::GetElementByOrder(int nOrder)
{
	if( nOrder >= m_nSize )
		return NULL;

	ALISTELEMENT * pThisElement = m_pHead->pNext;

	int nCount = 0;
	while(pThisElement != m_pTail)
	{
		if( nCount == nOrder )
			return pThisElement;
		else
			pThisElement = pThisElement->pNext;

		nCount ++;
	}

	return NULL;
}