/*
 * FILE: EC_HPWorkSit.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/12/27
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
//	Class CECHPWorkSit
//	
///////////////////////////////////////////////////////////////////////////

class CECHPWorkSit : public CECHPWork
{
public:		//	Types

public:		//	Constructor and Destructor

	CECHPWorkSit(CECHPWorkMan* pWorkMan);
	virtual ~CECHPWorkSit();

public:		//	Attributes

public:		//	Operations

	//	Set m_bBeSitting flag
	void SetBeSittingFlag(bool bTrue) { m_bBeSitting = bTrue; }

	//	Tick routine
	virtual bool Tick(DWORD dwDeltaTime);
	//	Reset work
	virtual void Reset();
	//	Copy work data
	virtual bool CopyData(CECHPWork* pWork);

protected:	//	Attributes

	bool	m_bBeSitting;
	
protected:	//	Operations

	//	On first tick
	virtual void OnFirstTick();
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECHPWorkPick
//	
///////////////////////////////////////////////////////////////////////////

class CECHPWorkPick : public CECHPWork
{
public:		//	Types

public:		//	Constructor and Destructor

	CECHPWorkPick(CECHPWorkMan* pWorkMan);
	virtual ~CECHPWorkPick();

public:		//	Attributes
	
public:		//	Operations

	bool IsGather() const { return m_bIsGather; }
	void SetGather(bool bGather, int gatherItemID = 0);

	//	Tick routine
	virtual bool Tick(DWORD dwDeltaTime);
	//	Reset work
	virtual void Reset();
	//	Copy work data
	virtual bool CopyData(CECHPWork* pWork);
	//  Work is cancel
	virtual void Cancel();

protected:	//	Attributes

	CECCounter	m_TimeCnt;		//	Pick time counter
	bool		m_bIsGather;
	int			m_gatherItemID;

protected:	//	Operations

	//	On first tick
	virtual void OnFirstTick();
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECHPWorkConcentrate
//	
///////////////////////////////////////////////////////////////////////////

class CECHPWorkConcentrate : public CECHPWork
{
public:		//	Types

	enum
	{
		DO_SUMMONPET = 0,	//	Summon pet
		DO_RECALLPET,
		DO_BANISHPET,
		DO_RESTOREPET,
		DO_UNKNOWN,
	};

public:		//	Constructor and Destructor

	CECHPWorkConcentrate(CECHPWorkMan* pWorkMan);
	virtual ~CECHPWorkConcentrate();

public:		//	Attributes
	
public:		//	Operations

	//	Set / Get do what flag
	void SetDoWhat(int iDoWhat) { m_iDoWhat = iDoWhat; }
	int GetDoWhat() { return m_iDoWhat; }

	//	Tick routine
	virtual bool Tick(DWORD dwDeltaTime);
	//	Reset work
	virtual void Reset();
	//	Copy work data
	virtual bool CopyData(CECHPWork* pWork);

protected:	//	Attributes

	int		m_iDoWhat;

protected:	//	Operations

	//	On first tick
	virtual void OnFirstTick();
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECHPWorkBeBound
//	
///////////////////////////////////////////////////////////////////////////

class CECHPWorkBeBound : public CECHPWork
{
public:		//	Types

public:		//	Constructor and Destructor

	CECHPWorkBeBound(CECHPWorkMan* pWorkMan);
	virtual ~CECHPWorkBeBound();

public:		//	Attributes
	
public:		//	Operations

	//	Tick routine
	virtual bool Tick(DWORD dwDeltaTime);
	//	Reset work
	virtual void Reset();
	//	Copy work data
	virtual bool CopyData(CECHPWork* pWork);

protected:	//	Attributes

protected:	//	Operations

	//	On first tick
	virtual void OnFirstTick();
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECHPWorkCongregate
//	
///////////////////////////////////////////////////////////////////////////

class CECHPWorkCongregate : public CECHPWork
{
public:		//	Types
	
public:		//	Constructor and Destructor
	
	CECHPWorkCongregate(CECHPWorkMan* pWorkMan);
	virtual ~CECHPWorkCongregate();
	
public:		//	Attributes
	
public:		//	Operations
	
	//	Set / Get the timeout
	void SetTimeout(int iType, int iTimeout);
	CECCounter GetCounter();
	int GetConType() const;

	//	Tick routine
	virtual bool Tick(DWORD dwDeltaTime);
	//	Reset work
	virtual void Reset();
	//	Copy work data
	virtual bool CopyData(CECHPWork* pWork);
	
protected:	//	Attributes
	
	int			m_ConType;
	CECCounter	m_TimeCnt;		//	congregate time counter

protected:	//	Operations
	
	//	On first tick
	virtual void OnFirstTick();
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

