/*
 * FILE: AStack.h
 *
 * DESCRIPTION: A class which manage one stack;
 *
 * CREATED BY: Hedi, 2002/3/27
 *
 * HISTORY:
 *				   
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _ASTACK_H_
#define _ASTACK_H_

#include "AArray.h"

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
//	Template AStack
//
///////////////////////////////////////////////////////////////////////////

template <class T, class ARG_T>
class AStack
{
public:		//	Types

public:		//	Constructors and Destructors

	AStack()
	{
		m_iStackPointer = 0;
	}
	
	AStack(int iSize, int iGrowBy) : m_aElements(iSize, iGrowBy)
	{
		m_iStackPointer = 0;
	}

	virtual ~AStack() {}

public:		//	Attributes

public:		//	Operations

	//	Push element
	int Push(ARG_T Element)
	{
		if (m_iStackPointer >= m_aElements.GetSize())
			m_aElements.Add(Element);
		else
			m_aElements[m_iStackPointer] = Element;

		return m_iStackPointer++;
	}

	//	Pop element
	T Pop()
	{
		ASSERT(m_iStackPointer > 0);
		return m_aElements[--m_iStackPointer];
	}

	//	Get the element on stack top but don't pop it
	//	Return false if there isn't element
	T Peek()
	{
		ASSERT(m_iStackPointer > 0);
		return m_aElements[m_iStackPointer-1];
	}

	//	Get element number
	int GetElementNum() { return m_iStackPointer; }
	//	Clear all elements
	void RemoveAll()
	{ 
		m_iStackPointer = 0;
		m_aElements.RemoveAll();
	}

protected:	//	Attributes

	AArray<T, ARG_T>	m_aElements;		//	Element array
	int					m_iStackPointer;	//	A SP reference position;

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//
//	Template APtrStack
//
///////////////////////////////////////////////////////////////////////////

template <class T>
class APtrStack
{
public:		//	Types

public:		//	Constructors and Destructors

	APtrStack() {}
	APtrStack(int iSize, int iGrowBy) : m_PtrStack(iSize, iGrowBy) {}
	virtual ~APtrStack() {}

public:		//	Attributes

public:		//	Operations

	//	Push element
	int Push(T Element) { return m_PtrStack.Push(Element); }
	//	Pop element
	T Pop() { return (T)m_PtrStack.Pop(); }
	//	Get the element on stack top but don't pop it
	//	Return false if there isn't element
	T Peek() { return (T)m_PtrStack.Peek(); }

	//	Get element number
	int GetElementNum() { return m_PtrStack.GetElementNum(); }
	//	Clear all elements
	void RemoveAll() { m_PtrStack.RemoveAll(); }

protected:	//	Attributes

	AStack<void*, void*>	m_PtrStack;		//	Pointer stack

protected:	//	Operations

};

#endif	//	_ASTACK_H_

