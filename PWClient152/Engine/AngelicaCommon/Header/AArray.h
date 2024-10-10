/*
 * FILE: AArray.h
 *
 * DESCRIPTION: Dynamic array template
 *
 * CREATED BY: duyuxin, 2003/6/4
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#ifndef _AARRAY_H_
#define _AARRAY_H_

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


///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Template AArray
//
///////////////////////////////////////////////////////////////////////////

template <class T, class ARG_T=T&>
class AArray
{
	DISABLE_COPY_AND_ASSIGNMENT(AArray);
public:		//	Types

public:		//	Constructors and Destructors

	AArray();
	AArray(int iSize, int iGrowBy);
	virtual ~AArray();

public:		//	Attributes

public:		//	Operations

	//	Add a element at end of array
	int	Add(ARG_T Elem);
	//	Add a element ensuring there isn't collision
	int UniquelyAdd(ARG_T Elem);
	//	Insert elements
	void InsertAt(int iIndex, ARG_T Elem, int iCount=1);
	void InsertAt(int iIndex, const AArray<T, ARG_T>& NewArray);
	//	Delete one or more elements
	void RemoveAt(int iIndex, int iCount=1);
	//	Remove a element quickly, this function will destory the sequence of elements
	void RemoveAtQuickly(int iIndex);
	//	Delete the last one element
	void RemoveTail() { if (!m_iNumElem) return; m_iNumElem--; }
	//	Delete all elements and release array buffer
	void RemoveAll(bool bReleaseBuf=true);
	//	Append another array to this one
	int Append(const AArray<T, ARG_T>& src);

	//	Set element at specified position
	void SetAt(int iIndex, ARG_T Elem) { (*this)[iIndex] = Elem; }
	//	Get element at specified position
	T GetAt(int iIndex) const { return (*this)[iIndex]; }
	//	Set element at specified position, grow array if necessary
	void SetAtGrow(int iIndex, ARG_T Elem);
	
	//	Find an element
	int Find(ARG_T Elem);
	//	Reverse find an element
	int ReverseFind(ARG_T Elem);

	//	Set new size of array
	bool SetSize(int iSize, int iGrowBy);
	//	Get size of array (number of elements)
	int GetSize() const { return m_iNumElem; }
	//	Get maximum index of element
	int GetUpperBound() const { return m_iNumElem-1; }
	//	Get elements data
	T* GetData() const { return m_aElements; }
	//	Get a element
	T& operator [] (int i) { ASSERT(i >= 0 && i < m_iNumElem); return m_aElements[i]; }
	T operator [] (int i) const { ASSERT(i >= 0 && i < m_iNumElem); return m_aElements[i]; }
	T* ElementAt(int i) { ASSERT(i >= 0 && i < m_iNumElem); return &m_aElements[i]; }

protected:	//	Attributes

	T*		m_aElements;	//	Element buffer
	int		m_iNumElem;		//	Number of valid elements
	int		m_iMaxElem;		//	Maximum number of elements can be hold in this array
	int		m_iGrowBy;		//	The minimum number of element slots to allocate when buffer increase.

protected:	//	Operations

	//	Allocate memory for array
	bool Allocate(int iSize);
	//	Grow array before insert new elements
	int GrowBeforeInsert(int iIndex, int iCount);
};

///////////////////////////////////////////////////////////////////////////
//
//	Implement AArray
//
///////////////////////////////////////////////////////////////////////////

template <class T, class ARG_T>
AArray<T, ARG_T>::AArray()
{
	m_aElements	= NULL;
	m_iNumElem	= 0;
	m_iMaxElem	= 0;
	m_iGrowBy	= 16;
}

template <class T, class ARG_T>
AArray<T, ARG_T>::AArray(int iSize, int iGrowBy)
{
	ASSERT(iSize >= 0 && iGrowBy >= 0);

	m_aElements	= NULL;
	m_iNumElem	= 0;
	m_iMaxElem	= 0;
	m_iGrowBy	= iGrowBy ? iGrowBy : 16;

	Allocate(iSize);
}

template <class T, class ARG_T>
AArray<T, ARG_T>::~AArray()
{
	AArray<T, ARG_T>::RemoveAll(true);
}

/*	Set new size of array. If the new size is smaller than the old size, then 
	the array is truncated and all unused memory is released.

	Return true for success, otherwise return false.

	iSize: new number of elements can be hold in array.
	iGrowBy: The minimum number of element slots to allocate when buffer increase.
			 == 0 means remain current m_iGrowBy
*/
template <class T, class ARG_T>
bool AArray<T, ARG_T>::SetSize(int iSize, int iGrowBy)
{
	ASSERT(iSize >= 0);

	if (iGrowBy)
		m_iGrowBy = iGrowBy;

	if (!iSize)
	{
		RemoveAll(true);
		return true;
	}

	if (iSize <= m_iMaxElem)
	{
		m_iNumElem = iSize;
		return true;
	}

	T* aElem = new T[iSize];
	if (!aElem)
	{
		a_LogOutput(1, "AArray::SetSize, Not enough memory!");
		return false;
	}

	//	Copy data
	if (m_aElements)
	{
		int iNumCopy = (iSize > m_iNumElem) ? m_iNumElem : iSize;
		for (int i=0; i < iNumCopy; i++)
			aElem[i] = m_aElements[i];

		//	Release old array
		delete [] m_aElements;
	}

	m_iNumElem	= iSize;
	m_iMaxElem	= iSize;
	m_aElements	= aElem;

	return true;
}

/*	Add a element at end of array.

	Return new element's index in the array for success, otherwise return -1.

	Elem: new element.
*/
template <class T, class ARG_T>
int AArray<T, ARG_T>::Add(ARG_T Elem)
{
	if (m_iNumElem >= m_iMaxElem)
		Allocate(m_iGrowBy);

	m_aElements[m_iNumElem] = Elem;

	return m_iNumElem++;
}

/*	Add a element ensuring there isn't collision

	Return element's index in array for success, otherwise return -1.

	Elem: new element.
*/
template <class T, class ARG_T>
int AArray<T, ARG_T>::UniquelyAdd(ARG_T Elem)
{
	for (int i=0; i < m_iNumElem; i++)
	{
		if (m_aElements[i] == Elem)
			return i;
	}

	return Add(Elem);
}

/*	Grow array before insert new elements. This function only grow array buffer but
	don't modify the value of m_iNumElem.

	Return new number of element after new elements are inserted.

	iIndex: insert position, the value can be greater than m_iNumElem-1
	iCount: insert number
*/
template <class T, class ARG_T>
int AArray<T, ARG_T>::GrowBeforeInsert(int iIndex, int iCount)
{
	ASSERT(iIndex >= 0 && iCount > 0);

	int iNumElem;
	if (iIndex >= m_iNumElem)
		iNumElem = iIndex + iCount;
	else
		iNumElem = m_iNumElem + iCount;

	if (iNumElem > m_iMaxElem)
	{
		//	Enlarge elements buffer
		if (iNumElem - m_iMaxElem < m_iGrowBy)
			Allocate(m_iGrowBy);
		else
			Allocate(iNumElem - m_iMaxElem);
	}

	return iNumElem;
}

/*	Insert a element at specified index.

	iIndex: specified place new element will be inserted.
	Elem: new element
	iCount: times the new element will be inserted.
*/
template <class T, class ARG_T>
void AArray<T, ARG_T>::InsertAt(int iIndex, ARG_T Elem, int iCount/* 1 */)
{
	ASSERT(iIndex >= 0);

	if (iCount <= 0)
		return;

	int iNumElem = GrowBeforeInsert(iIndex, iCount);

	int i;
	for (i=m_iNumElem-1; i >= iIndex; i--)
		m_aElements[i+iCount] = m_aElements[i];

	for (i=0; i < iCount; i++)
		m_aElements[iIndex+i] = Elem;

	m_iNumElem = iNumElem;
}

//	Insert a array
template <class T, class ARG_T>
void AArray<T, ARG_T>::InsertAt(int iIndex, const AArray<T, ARG_T>& NewArray)
{
	ASSERT(iIndex >= 0);

	int iCount = NewArray.GetSize();
	if (!iCount)
		return;

	int i, iNumElem = GrowBeforeInsert(iIndex, iCount);

	for (i=m_iNumElem-1; i >= iIndex; i--)
		m_aElements[i+iCount] = m_aElements[i];

	for (i=0; i < NewArray.GetSize(); i++)
		m_aElements[iIndex+i] = NewArray[i];

	m_iNumElem = iNumElem;
}

/*	Append another array to this one

	Return index of the first appended element
*/
template <class T, class ARG_T>
int AArray<T, ARG_T>::Append(const AArray<T, ARG_T>& src)
{
	int iRet = m_iNumElem;
	InsertAt(m_iNumElem, src);
	return iRet;
}

/*	Set element at specified index, grow array when necessary

	iIndex: index of new elemnet
	Elem: element's data
*/
template <class T, class ARG_T>
void AArray<T, ARG_T>::SetAtGrow(int iIndex, ARG_T Elem)
{
	ASSERT(iIndex >= 0);

	if (iIndex >= m_iNumElem)
		m_iNumElem = GrowBeforeInsert(iIndex, 1);

	m_aElements[iIndex] = Elem;
}

/*	Delete a element at specified place.

	iIndex: element's place.
	iCount: number of element will be removed
*/
template <class T, class ARG_T>
void AArray<T, ARG_T>::RemoveAt(int iIndex, int iCount/* 1 */)
{
	ASSERT(iIndex >= 0 && iIndex < m_iNumElem);
	ASSERT(iIndex + iCount <= m_iNumElem);

	int iNumMove = m_iNumElem - iIndex - iCount;

	for (int i=0; i < iNumMove; i++)
		m_aElements[iIndex+i] = m_aElements[iIndex+i+iCount];

	m_iNumElem -= iCount;
}

//	Remove a element quickly, this function will destory the sequence of elements
template <class T, class ARG_T>
void AArray<T, ARG_T>::RemoveAtQuickly(int iIndex)
{
	ASSERT(iIndex >= 0 && iIndex < m_iNumElem);

	if (iIndex != m_iNumElem-1)
		m_aElements[iIndex] = m_aElements[m_iNumElem-1];

	m_iNumElem--;
}

/*	Allocate memory for array.

	Return true for success, otherwise return false.

	iSize: number of elements will be added to array.
*/
template <class T, class ARG_T>
bool AArray<T, ARG_T>::Allocate(int iSize)
{
	ASSERT(iSize >= 0);
	
	if (!iSize)
		return true;

	iSize += m_iMaxElem;

	T* aElem = new T[iSize];
	if (!aElem)
	{
		a_LogOutput(1, "AArray::Allocate, Not enough memory!");
		return false;
	}

	//	Copy data
	for (int i=0; i < m_iNumElem; i++)
		aElem[i] = m_aElements[i];

	if (m_aElements)
		delete [] m_aElements;

	m_aElements	= aElem;
	m_iMaxElem	= iSize;

	return true;
}

//	Delete all elements and release array buffer.
template <class T, class ARG_T>
void AArray<T, ARG_T>::RemoveAll(bool bReleaseBuf/* true */)
{
	if (bReleaseBuf)
	{
		if (m_aElements)
		{
			delete [] m_aElements;
			m_aElements = NULL;
		}

		m_iMaxElem = 0;
	}

	m_iNumElem = 0;
}

//	Find an element
template <class T, class ARG_T>
int AArray<T, ARG_T>::Find(ARG_T Elem)
{
	for (int i=0; i < m_iNumElem; i++)
	{
		if (m_aElements[i] == Elem)
			return i;
	}

	return -1;
}

//	Reverse find an element
template <class T, class ARG_T>
int AArray<T, ARG_T>::ReverseFind(ARG_T Elem)
{
	for (int i=m_iNumElem-1; i >= 0; i--)
	{
		if (m_aElements[i] == Elem)
			return i;
	}

	return -1;
}

///////////////////////////////////////////////////////////////////////////
//
//	Template APtrArray
//
///////////////////////////////////////////////////////////////////////////

template <class T>
class APtrArray
{
public:		//	Types

public:		//	Constructors and Destructors

	APtrArray() { ASSERT((T)0 == (void*)0); }
	APtrArray(int iSize, int iGrowBy) : m_aPointers(iSize, iGrowBy) { ASSERT((T)0 == (void*)0); }
	virtual ~APtrArray() {}

public:		//	Attributes

public:		//	Operations

	//	Add a element at end of array
	inline int Add(T Elem) { return m_aPointers.Add(Elem); }
	//	Add a element ensuring there isn't collision
	int UniquelyAdd(T Elem) { return m_aPointers.UniquelyAdd(Elem); }
	//	Insert elements
	inline void InsertAt(int iIndex, T Elem, int iCount=1) { m_aPointers.InsertAt(iIndex, Elem, iCount); }
	inline void InsertAt(int iIndex, const APtrArray& NewArray) { m_aPointers.InsertAt(iIndex, NewArray); }
	//	Delete one or more elements
	inline void RemoveAt(int iIndex, int iCount=1) { m_aPointers.RemoveAt(iIndex, iCount); }
	//	Remove a element quickly, this function will destory the sequence of elements
	inline void RemoveAtQuickly(int iIndex) { m_aPointers.RemoveAtQuickly(iIndex); }
	//	Delete the last one element
	inline void RemoveTail() { m_aPointers.RemoveTail(); }
	//	Delete all elements and release array buffer
	inline void RemoveAll(bool bReleaseBuf=true) { m_aPointers.RemoveAll(bReleaseBuf); }
	//	Append another array to this one
	inline int Append(const APtrArray& src) { m_aPointers.Append(src); }

	//	Set element at specified position
	inline void SetAt(int iIndex, T Elem) { m_aPointers.SetAt(iIndex, Elem); }
	//	Get element at specified position
	inline T GetAt(int iIndex) const { return (T)m_aPointers.GetAt(iIndex); }
	//	Set element at specified position, grow array if necessary
	inline void SetAtGrow(int iIndex, T Elem) { m_aPointers.SetAtGrow(iIndex, Elem); }

	//	Find an element
	inline int Find(T Elem) { return m_aPointers.Find(Elem); }
	//	Reverse find an element
	inline int ReverseFind(T Elem) { return m_aPointers.ReverseFind(Elem); }

	//	Set new size of array
	inline bool SetSize(int iSize, int iGrowBy) { return m_aPointers.SetSize(iSize, iGrowBy); }
	//	Get size of array (number of elements)
	inline int GetSize() const { return m_aPointers.GetSize(); }
	//	Get maximum index of element
	inline int GetUpperBound() const { return m_aPointers.GetUpperBound(); }
	//	Get elements data
	inline T* GetData() const { return (T*)m_aPointers.GetData(); }
	//	Get a element
	inline T& operator [] (int i) { return (T&)m_aPointers[i]; }
	inline T operator [] (int i) const { return (T)m_aPointers[i]; }
	inline T* ElementAt(int i) { return (T*)&m_aElements[i]; }

protected:	//	Attributes

	AArray<void*, void*>	m_aPointers;

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//
//	Implement APtrArray
//
///////////////////////////////////////////////////////////////////////////


#endif	//	_AARRAY_H_

