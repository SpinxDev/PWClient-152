/*
 * FILE: UndoMan.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/8/11
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "Global.h"
#include "UndoAction.h"
#include "UndoMan.h"
#include "AMemory.h"

//#define new A_DEBUG_NEW

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


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

CUndoMan	g_UndoMan;

///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Implement CUndoMan
//	
///////////////////////////////////////////////////////////////////////////

CUndoMan::CUndoMan()
{
}

CUndoMan::~CUndoMan()
{
}

//	Initialize object
bool CUndoMan::Init()
{
	return true;
}

//	Release object
void CUndoMan::Release()
{
	//	Release all actions
	ReleaseUndoListActions();
	ReleaseRedoListActions();
}

//	Reset undo manager
bool CUndoMan::Reset()
{
	//	Release all actions
	ReleaseUndoListActions();
	ReleaseRedoListActions();

	return true;
}

//	Add undo action
bool CUndoMan::AddUndoAction(CUndoAction* pAction)
{
	m_UndoList.AddTail(pAction);
	
	//这儿删除最先进去的数，如果表中动作大于指定的最大数
	if(m_UndoList.GetCount()>MAX_UNDO_STEP)
	{
		CUndoAction* pHead = m_UndoList.GetHead();
		pHead->Release();
		delete pHead;
		m_UndoList.RemoveHead();
	}

	//	Release all actions in redo list
	ReleaseRedoListActions();

	return true;
}

//	Release all actions in redo list
void CUndoMan::ReleaseRedoListActions()
{
	if (!m_RedoList.GetCount())
		return;

	ALISTPOSITION pos = m_RedoList.GetHeadPosition();
	while (pos)
	{
		CUndoAction* pAction = m_RedoList.GetNext(pos);
		pAction->Release();
		delete pAction;
	}

	m_RedoList.RemoveAll();
}

//	Release all actions in undo list
void CUndoMan::ReleaseUndoListActions()
{
	if (!m_UndoList.GetCount())
		return;

	ALISTPOSITION pos = m_UndoList.GetHeadPosition();
	while (pos)
	{
		CUndoAction* pAction = m_UndoList.GetNext(pos);
		pAction->Release();
		delete pAction;
	}

	m_UndoList.RemoveAll();
}

//	Undo one step
bool CUndoMan::Undo()
{
	if (!m_UndoList.GetCount())
		return true;	//	No action can be undo
	
	CUndoAction* pAction = m_UndoList.GetTail();
	if (!pAction->Undo())
	{
		g_Log.Log("CUndoMan::Undo, Failed to undo action");
		return false;
	}

	//	Remove action from undo list and add it to redo list
	pAction = m_UndoList.RemoveTail();
	m_RedoList.AddTail(pAction);

	return true;
}

//	Redo one step
bool CUndoMan::Redo()
{
	if (!m_RedoList.GetCount())
		return true;	//	No action can be redo
	
	CUndoAction* pAction = m_RedoList.GetTail();
	if (!pAction->Redo())
	{
		g_Log.Log("CUndoMan::Redo, Failed to redo action");
		return false;
	}

	//	Remove action from redo list and add it to undo list
	pAction = m_RedoList.RemoveTail();
	m_UndoList.AddTail(pAction);

	return true;
}

