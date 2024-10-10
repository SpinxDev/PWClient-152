/*
 * FILE: AList2.h
 *
 * DESCRIPTION: List template
 *
 * CREATED BY: duyuxin, 2003/6/4
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#ifndef _ALIST2_H_
#define _ALIST2_H_

#include "ABaseDef.h"
#include "AMemory.h"
#include "ALog.h"

#pragma warning (disable: 4786)

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Types and Global variables
//
///////////////////////////////////////////////////////////////////////////

typedef struct s_LISTPOSITION {} *ALISTPOSITION;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Template AList2
//
///////////////////////////////////////////////////////////////////////////

template <class T, class ARG_T=T&>
class AList2
{
	DISABLE_COPY_AND_ASSIGNMENT(AList2);
public:		//	Types

	//	List node
	struct s_NODE
	{
		s_NODE*		pNext;		//	Point to next node
		s_NODE*		pPrev;		//	Point to previous node
		T			Data;		//	Data
	};

	class c_NodePool
	{
	public:

		c_NodePool*	m_pNext;	//	Next node block
		s_NODE*		m_aNodes;	//	Node pool
		int			m_iSize;	//	Number of node in node buffer

		c_NodePool(int iSize)
		{
			ASSERT(iSize > 0);
			m_iSize = iSize;

			//	TODO: throw a exception if memory allocation fail
			m_aNodes = new s_NODE[iSize];

			for (int i=0; i < iSize-1; i++)
				m_aNodes[i].pNext = &m_aNodes[i+1];
			
			m_aNodes[iSize-1].pNext	= NULL;

			m_pNext = NULL;
		}

		virtual ~c_NodePool()
		{
			if (m_aNodes)
				delete [] m_aNodes;
		}
	};

public:		//	Constructors and Destructors

	AList2(int iPoolSize=10);
	virtual ~AList2();

public:		//	Attributes

public:		//	Operations

	//	Get head position of list
	ALISTPOSITION GetHeadPosition() const;
	//	Get tail position of list
	ALISTPOSITION GetTailPosition() const;

	//	Add a item to head of list
	ALISTPOSITION AddHead(ARG_T Data);
	//	Add a item to tail of list
	ALISTPOSITION AddTail(ARG_T Data);
	//	Link another list to tail of this one
	void AddListToTail(const AList2<T, ARG_T>* pList);
	//	Insert a item after specified position
	ALISTPOSITION InsertAfter(ALISTPOSITION Pos, ARG_T Data);
	//	Insert a item before specified position
	ALISTPOSITION InsertBefore(ALISTPOSITION Pos, ARG_T Data);

	//	Search a item in list
	ALISTPOSITION Find(ARG_T Data, ALISTPOSITION StartPos=NULL) const;
	//	Search a item with speicified index
	ALISTPOSITION FindByIndex(int iIndex) const;

	//	Remove head item of list
	T RemoveHead();
	//	Remove tail item of list
	T RemoveTail();
	//	Remove item at specified position
	T RemoveAt(ALISTPOSITION Pos);
	//	Remove all items in list
	void RemoveAll();

	//	Set item
	void SetByIndex(int iIndex, ARG_T Data);
	void SetAt(ALISTPOSITION Pos, ARG_T Data);
	//	Get item
	const T& GetByIndex(int iIndex) const;
	T& GetByIndex(int iIndex);
	const T& GetAt(ALISTPOSITION Pos) const;
	T& GetAt(ALISTPOSITION Pos);
	//	Get previous item
	const T& GetPrev(ALISTPOSITION& Pos) const;
	T& GetPrev(ALISTPOSITION& Pos);
	//	Get next item
	const T& GetNext(ALISTPOSITION& Pos) const;
	T& GetNext(ALISTPOSITION& Pos);
	//	Get the first item
	const T& GetHead() const;
	T& GetHead();
	//	Get the last item
	const T& GetTail() const;
	T& GetTail();

	//	Get item number
	int	GetCount() const { return m_iCount; }

protected:	//	Attributes

	s_NODE*		m_pHead;		//	List head
	s_NODE*		m_pTail;		//	List tail
	s_NODE*		m_pFree;		//	Free list nodes
	int			m_iCount;		//	List item counter
	int			m_iPoolSize;	//	Size of each node pool
	c_NodePool*	m_pPools;		//	Node pool list

protected:	//	Operations

	//	Allocate a new node
	s_NODE*	AllocNode();
	//	Free a node
	void FreeNode(s_NODE* pNode);
};

///////////////////////////////////////////////////////////////////////////
//
//	Implemen AList2
//
///////////////////////////////////////////////////////////////////////////

template <class T, class ARG_T>
AList2<T, ARG_T>::AList2(int iPoolSize/* 10 */)
{
	m_pHead		= NULL;
	m_pTail		= NULL;
	m_pFree		= NULL;
	m_iCount	= NULL;
	m_iPoolSize	= iPoolSize;
	m_pPools	= 0;
}

template <class T, class ARG_T>
AList2<T, ARG_T>::~AList2()
{
	//	Remove all nodes
	RemoveAll();

	//	Release node pools
	while (m_pPools)
	{
		c_NodePool* m_pPool = m_pPools;
		m_pPools = m_pPools->m_pNext;
		delete m_pPool;
	}
}

//	Allocate a new node
template <class T, class ARG_T>
typename AList2<T, ARG_T>::s_NODE* AList2<T, ARG_T>::AllocNode()
{
	if (!m_pFree)
	{
		//	Allocate a new node pool
		c_NodePool* pPool = new c_NodePool(m_iPoolSize);
		if (!pPool)
		{
			a_LogOutput(1, "AList2::AllocNode, Not enough memory!");
			return NULL;
		}

		m_pFree	= pPool->m_aNodes;
		pPool->m_pNext = m_pPools;
		m_pPools = pPool;
	}

	//	Get a free node
	s_NODE* pNode = m_pFree;
	m_pFree	= m_pFree->pNext;
	m_iCount++;

	pNode->pNext = NULL;
	pNode->pPrev = NULL;

	return pNode;
}

//	Free a node
template <class T, class ARG_T>
void AList2<T, ARG_T>::FreeNode(s_NODE* pNode)
{
	ASSERT(pNode);

	pNode->pNext = m_pFree;
	m_pFree		 = pNode;

	m_iCount--;
}

//	Get head position of list
template <class T, class ARG_T>
ALISTPOSITION AList2<T, ARG_T>::GetHeadPosition() const
{
	return (ALISTPOSITION)m_pHead;
}

//	Get tail position of list
template <class T, class ARG_T>
ALISTPOSITION AList2<T, ARG_T>::GetTailPosition() const
{
	return (ALISTPOSITION)m_pTail;
}

//	Add a item to head of list
template <class T, class ARG_T>
ALISTPOSITION AList2<T, ARG_T>::AddHead(ARG_T Data)
{
	s_NODE* pNode = AllocNode();

	pNode->Data	 = Data;
	pNode->pNext = m_pHead;

	if (m_pHead)
		m_pHead->pPrev = pNode;
	else
		m_pTail	= pNode;
	
	m_pHead	= pNode;

	return (ALISTPOSITION)pNode;
}

//	Add a item to tail of list
template <class T, class ARG_T>
ALISTPOSITION AList2<T, ARG_T>::AddTail(ARG_T Data)
{
	s_NODE* pNode = AllocNode();

	pNode->Data	 = Data;
	pNode->pPrev = m_pTail;

	if (m_pTail)
		m_pTail->pNext = pNode;
	else
		m_pHead	= pNode;
	
	m_pTail	= pNode;

	return (ALISTPOSITION)pNode;
}

//	Link another list to tail of this one
template <class T, class ARG_T>
void AList2<T, ARG_T>::AddListToTail(const AList2<T, ARG_T>* pList)
{
	ASSERT(pList);

	ALISTPOSITION pos = pList->GetHeadPosition();

	while (pos)
		AddTail(pList->GetNext(pos));
}

//	Insert a item after specified position
template <class T, class ARG_T>
ALISTPOSITION AList2<T, ARG_T>::InsertAfter(ALISTPOSITION Pos, ARG_T Data)
{
	if (!Pos)
		return AddTail(Data);
	
	s_NODE* pThisNode = (s_NODE*)Pos;
	s_NODE* pNode = AllocNode();

	pNode->Data	 = Data;
	pNode->pPrev = pThisNode;
	pNode->pNext = pThisNode->pNext;

	if (pThisNode->pNext)
		pThisNode->pNext->pPrev = pNode;
	else
		m_pTail	= pNode;

	pThisNode->pNext = pNode;

	return (ALISTPOSITION)pNode;
}

//	Insert a item before specified position
template <class T, class ARG_T>
ALISTPOSITION AList2<T, ARG_T>::InsertBefore(ALISTPOSITION Pos, ARG_T Data)
{
	if (!Pos)
		return AddHead(Data);
	
	s_NODE* pThisNode = (s_NODE*)Pos;
	s_NODE* pNode = AllocNode();

	pNode->Data	 = Data;
	pNode->pNext = pThisNode;
	pNode->pPrev = pThisNode->pPrev;

	if (pThisNode->pPrev)
		pThisNode->pPrev->pNext = pNode;
	else
		m_pHead	= pNode;
	
	pThisNode->pPrev = pNode;

	return (ALISTPOSITION)pNode;
}

//	Search a item in list
template <class T, class ARG_T>
ALISTPOSITION AList2<T, ARG_T>::Find(ARG_T Data, ALISTPOSITION StartPos/* NULL */) const
{
	s_NODE* pNode = StartPos ? (s_NODE*)StartPos : m_pHead;

	while (pNode)
	{
		if (pNode->Data == Data)
			break;
		
		pNode = pNode->pNext;
	}

	return (ALISTPOSITION)pNode;
}

//	Search a item with speicified index
template <class T, class ARG_T>
ALISTPOSITION AList2<T, ARG_T>::FindByIndex(int iIndex) const
{
	if (iIndex < 0 || iIndex >= (int)m_iCount)
		return NULL;
	
	if (iIndex > ((int)m_iCount >> 1))
	{
		//	Search from tail
		s_NODE* pNode = m_pTail;
		for (int i = (int)m_iCount-iIndex-1; i > 0; i--)
			pNode = pNode->pPrev;

		return (ALISTPOSITION)pNode;
	}
	else	//	Search from head
	{
		s_NODE* pNode = m_pHead;
		for (int i=0; i < iIndex; i++)
			pNode = pNode->pNext;
		
		return (ALISTPOSITION)pNode;
	}
}

//	Remove head item of list
template <class T, class ARG_T>
T AList2<T, ARG_T>::RemoveHead()
{
	ASSERT(m_pHead);

	s_NODE* pNode = m_pHead;

	T Ret	= pNode->Data;
	m_pHead	= pNode->pNext;

	if (m_pHead)
		m_pHead->pPrev = NULL;
	else
		m_pTail	= NULL;
	
	FreeNode(pNode);

	return Ret;
}

//	Remove tail item of list
template <class T, class ARG_T>
T AList2<T, ARG_T>::RemoveTail()
{
	ASSERT(m_pTail);

	s_NODE* pNode = m_pTail;

	T Ret	= pNode->Data;
	m_pTail	= pNode->pPrev;

	if (m_pTail)
		m_pTail->pNext = NULL;
	else
		m_pHead	= NULL;
	
	FreeNode(pNode);
	
	return Ret;
}

//	Remove item at specified position
template <class T, class ARG_T>
T AList2<T, ARG_T>::RemoveAt(ALISTPOSITION Pos)
{
	ASSERT(Pos);

	s_NODE* pNode = (s_NODE*)Pos;
	T Ret = pNode->Data;

	s_NODE*	pNext = pNode->pNext;
	s_NODE*	pPrev = pNode->pPrev;

	if (pNext)
		pNext->pPrev = pPrev;
	else
		m_pTail	= pPrev;
	
	if (pPrev)
		pPrev->pNext = pNext;
	else
		m_pHead	= pNext;
	
	FreeNode(pNode);

	return Ret;
}

//	Remove all items in list
template <class T, class ARG_T>
void AList2<T, ARG_T>::RemoveAll()
{
	while (m_pHead != NULL)
	{
		s_NODE* pNode = m_pHead;
		m_pHead	= pNode->pNext;
		FreeNode(pNode);
	}

	m_pTail	 = NULL;
	m_iCount = 0;
}

//	Set item
template <class T, class ARG_T>
void AList2<T, ARG_T>::SetByIndex(int iIndex, ARG_T Data)
{
	s_NODE* pNode = (s_NODE*)FindByIndex(iIndex);
	if (pNode)
		pNode->Data = Data;
}

template <class T, class ARG_T>
void AList2<T, ARG_T>::SetAt(ALISTPOSITION Pos, ARG_T Data)
{
	ASSERT(Pos);
	((s_NODE*)Pos)->Data = Data;
}

//	Get item
template <class T, class ARG_T>
const T& AList2<T, ARG_T>::GetByIndex(int iIndex) const
{
	return ((s_NODE*)FindByIndex(iIndex))->Data;
}

template <class T, class ARG_T>
T& AList2<T, ARG_T>::GetByIndex(int iIndex)
{
	return ((s_NODE*)FindByIndex(iIndex))->Data;
}

template <class T, class ARG_T>
const T& AList2<T, ARG_T>::GetAt(ALISTPOSITION Pos) const
{
	ASSERT(Pos);
	return ((s_NODE*)Pos)->Data;
}

template <class T, class ARG_T>
T& AList2<T, ARG_T>::GetAt(ALISTPOSITION Pos)
{
	ASSERT(Pos);
	return ((s_NODE*)Pos)->Data;
}

//	Get previous item
template <class T, class ARG_T>
const T& AList2<T, ARG_T>::GetPrev(ALISTPOSITION& Pos) const
{
	ASSERT(Pos);
	T& Ret	= ((s_NODE*)Pos)->Data;
	Pos		= (ALISTPOSITION)((s_NODE*)Pos)->pPrev;
	return Ret;
}

template <class T, class ARG_T>
T& AList2<T, ARG_T>::GetPrev(ALISTPOSITION& Pos)
{
	ASSERT(Pos);
	T& Ret	= ((s_NODE*)Pos)->Data;
	Pos		= (ALISTPOSITION)((s_NODE*)Pos)->pPrev;
	return Ret;
}

//	Get next item
template <class T, class ARG_T>
const T& AList2<T, ARG_T>::GetNext(ALISTPOSITION& Pos) const
{
	ASSERT(Pos);
	T& Ret	= ((s_NODE*)Pos)->Data;
	Pos		= (ALISTPOSITION)((s_NODE*)Pos)->pNext;
	return Ret;
}

template <class T, class ARG_T>
T& AList2<T, ARG_T>::GetNext(ALISTPOSITION& Pos)
{
	ASSERT(Pos);
	T& Ret	= ((s_NODE*)Pos)->Data;
	Pos		= (ALISTPOSITION)((s_NODE*)Pos)->pNext;
	return Ret;
}

//	Get the first item
template <class T, class ARG_T>
const T& AList2<T, ARG_T>::GetHead() const
{
	ASSERT(m_pHead);
	return m_pHead->Data;
}

template <class T, class ARG_T>
T& AList2<T, ARG_T>::GetHead()
{
	ASSERT(m_pHead);
	return m_pHead->Data;
}

//	Get the last item
template <class T, class ARG_T>
const T& AList2<T, ARG_T>::GetTail() const
{
	ASSERT(m_pTail);
	return m_pTail->Data;
}

template <class T, class ARG_T>
T& AList2<T, ARG_T>::GetTail()
{
	ASSERT(m_pTail);
	return m_pTail->Data;
}

///////////////////////////////////////////////////////////////////////////
//
//	Template APtrList
//
///////////////////////////////////////////////////////////////////////////

template <class T>
class APtrList
{
public:		//	Types

public:		//	Constructors and Destructors

	APtrList(int iPoolSize=10) : m_PointerList(iPoolSize) { ASSERT((T)0 == (void*)0); }
	virtual ~APtrList() {}

public:		//	Attributes

public:		//	Operations

	//	Get head position of list
	ALISTPOSITION GetHeadPosition() const { return m_PointerList.GetHeadPosition(); }
	//	Get tail position of list
	ALISTPOSITION GetTailPosition() const { return m_PointerList.GetTailPosition(); }

	//	Add a item to head of list
	ALISTPOSITION AddHead(T Data) { return m_PointerList.AddHead(Data); }
	//	Add a item to tail of list
	ALISTPOSITION AddTail(T Data) { return m_PointerList.AddTail(Data); }
	//	Link another list to tail of this one
	void AddListToTail(const APtrList* pList) { m_PointerList.AddListToTail(&pList->m_PointerList); }
	//	Insert a item after specified position
	ALISTPOSITION InsertAfter(ALISTPOSITION Pos, T Data) { return m_PointerList.InsertAfter(Pos, Data); }
	//	Insert a item before specified position
	ALISTPOSITION InsertBefore(ALISTPOSITION Pos, T Data) { return m_PointerList.InsertBefore(Pos, Data); }

	//	Search a item in list
	ALISTPOSITION Find(T Data, ALISTPOSITION StartPos=NULL) const { return m_PointerList.Find(Data, StartPos); }
	//	Search a item with speicified index
	ALISTPOSITION FindByIndex(int iIndex) const { return m_PointerList.FindByIndex(iIndex); }

	//	Remove head item of list
	T RemoveHead() { return (T)m_PointerList.RemoveHead(); }
	//	Remove tail item of list
	T RemoveTail() { return (T)m_PointerList.RemoveTail(); }
	//	Remove item at specified position
	T RemoveAt(ALISTPOSITION Pos) { return (T)m_PointerList.RemoveAt(Pos); }
	//	Remove all items in list
	void RemoveAll() { m_PointerList.RemoveAll(); }

	//	Set item
	void SetByIndex(int iIndex, T Data) { m_PointerList.SetByIndex(iIndex, Data); }
	void SetAt(ALISTPOSITION Pos, T Data) { m_PointerList.SetAt(Pos, Data); }
	//	Get item
	const T& GetByIndex(int iIndex) const { return (const T&)m_PointerList.GetByIndex(iIndex); }
	T& GetByIndex(int iIndex) { return (T&)m_PointerList.GetByIndex(iIndex); }
	const T& GetAt(ALISTPOSITION Pos) const { return (const T&)m_PointerList.GetAt(Pos); }
	T& GetAt(ALISTPOSITION Pos) { return (T&)m_PointerList.GetAt(Pos); }
	//	Get previous item
	const T& GetPrev(ALISTPOSITION& Pos) const { return (const T&)m_PointerList.GetPrev(Pos); }
	T& GetPrev(ALISTPOSITION& Pos) { return (T&)m_PointerList.GetPrev(Pos); }
	//	Get next item
	const T& GetNext(ALISTPOSITION& Pos) const { return (const T&)m_PointerList.GetNext(Pos); }
	T& GetNext(ALISTPOSITION& Pos) { return (T&)m_PointerList.GetNext(Pos); }
	//	Get the first item
	const T& GetHead() const { return (const T&)m_PointerList.GetHead(); }
	T& GetHead() { return (T&)m_PointerList.GetHead(); }
	//	Get the last item
	const T& GetTail() const { return (const T&)m_PointerList.GetTail(); }
	T& GetTail() { return (T&)m_PointerList.GetTail(); }

	//	Get item number
	int	GetCount() const { return m_PointerList.GetCount(); }

protected:	//	Attributes

	//	Define pointer list
	AList2<void*, void*>	m_PointerList;

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//
//	Implemen AList2
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_ALIST2_H_
