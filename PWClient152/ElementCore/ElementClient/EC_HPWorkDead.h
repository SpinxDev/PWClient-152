/*
 * FILE: EC_HPWorkDead.h
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
//	Class CECHPWorkDead
//	
///////////////////////////////////////////////////////////////////////////

class CECHPWorkDead : public CECHPWork
{
public:		//	Types

public:		//	Constructor and Destructor

	CECHPWorkDead(CECHPWorkMan* pWorkMan);
	virtual ~CECHPWorkDead();

public:		//	Attributes

public:		//	Operations

	//	Set m_bBeDead flag
	void SetBeDeadFlag(bool bTrue) { m_bBeDead = bTrue; }

	//	Tick routine
	virtual bool Tick(DWORD dwDeltaTime);
	//	Reset work
	virtual void Reset();
	//	Copy work data
	virtual bool CopyData(CECHPWork* pWork);
	
protected:	//	Attributes

	bool	m_bBeDead;
	
protected:	//	Operations

	//	On first tick
	virtual void OnFirstTick();
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECHPWorkRevive
//	
///////////////////////////////////////////////////////////////////////////

class CECHPWorkRevive : public CECHPWork
{
public:		//	Types

public:		//	Constructor and Destructor

	CECHPWorkRevive(CECHPWorkMan* pWorkMan);
	virtual ~CECHPWorkRevive();

public:		//	Attributes

public:		//	Operations

	//	Tick routine
	virtual bool Tick(DWORD dwDeltaTime);
	//	Reset work
	virtual void Reset();
	//	Copy work data
	virtual bool CopyData(CECHPWork* pWork);
	
protected:	//	Attributes

	DWORD	m_dwOTCnt;		//	Overtime counter

protected:	//	Operations

	//	On first tick
	virtual void OnFirstTick();
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

