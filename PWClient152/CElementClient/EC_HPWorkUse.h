/*
 * FILE: EC_HPWorkUse.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/10/11
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include "EC_HPWork.h"
#include "EC_Counter.h"

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
//	Class CECHPWorkUse
//	
///////////////////////////////////////////////////////////////////////////

class CECHPWorkUse : public CECHPWork
{
public:		//	Types

public:		//	Constructor and Destructor

	CECHPWorkUse(CECHPWorkMan* pWorkMan);
	virtual ~CECHPWorkUse();

public:		//	Attributes

public:		//	Operations

	//	Set parameters
	void SetParams(int idItem, DWORD dwUseTime, int idTarget, bool bWorkForMonsterSpirit = false);

	//	Tick routine
	virtual bool Tick(DWORD dwDeltaTime);
	//	Reset work
	virtual void Reset();
	//	Copy work data
	virtual bool CopyData(CECHPWork* pWork);
	//	Work is cancel
	virtual void Cancel();

	int GetTarget() { return m_idTarget; }
	int GetItem() { return m_idItem; }
	CECCounter& GetTimeCounter() { return m_UseTimeCnt; }

protected:	//	Attributes

	int			m_idTarget;		//	Target
	int			m_idItem;		//	Item will be used
	CECCounter	m_UseTimeCnt;	//	Use time counter
	bool		m_bWorkForMonsterSpirit;

protected:	//	Operations

	//	On first tick
	virtual void OnFirstTick();
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

