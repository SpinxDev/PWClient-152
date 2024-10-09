/*
 * FILE: UndoMan.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/8/11
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#define MAX_UNDO_STEP	50

#include "AList2.h"

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

class CUndoAction;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CUndoMan
//	
///////////////////////////////////////////////////////////////////////////

class CUndoMan
{
public:		//	Types

public:		//	Constructor and Destructor

	CUndoMan();
	virtual ~CUndoMan();

public:		//	Attributes

public:		//	Operations

	//	Initialize object
	bool Init();
	//	Release object
	void Release();
	//	Reset undo manager
	bool Reset();
	
	//	Add undo action
	bool AddUndoAction(CUndoAction* pAction);
	//	Undo one step
	bool Undo();
	//	Redo one step
	bool Redo();

	//	Get action number in undo list
	int GetUndoListActionNum() { return m_UndoList.GetCount(); }
	//	Get action number in redo list
	int GetRedoListActionNum() { return m_RedoList.GetCount(); }

protected:	//	Attributes

	APtrList<CUndoAction*>	m_UndoList;		//	Undo list
	APtrList<CUndoAction*>	m_RedoList;		//	Redo list

protected:	//	Operations

	//	Release all actions in undo list
	void ReleaseUndoListActions();
	//	Release all actions in redo list
	void ReleaseRedoListActions();
};

extern CUndoMan	g_UndoMan;

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

