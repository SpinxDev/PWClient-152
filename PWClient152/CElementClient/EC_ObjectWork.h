/*
 * FILE: EC_ObjectWork.h
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

#include "ABaseDef.h"

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
//	Class CECObjectWork
//	
///////////////////////////////////////////////////////////////////////////

class CECObjectWork
{
public:		//	Types

public:		//	Constructor and Destructor

	CECObjectWork(int iWorkID)
	{ 
		m_iWorkID		= iWorkID;
		m_bFinished		= false;
		m_dwMask		= 0;
		m_dwTransMask	= 0;
		m_bFinished		= true;
	}

	virtual ~CECObjectWork() {}

public:		//	Attributes

public:		//	Operations

	//	Tick routine
	virtual bool Tick(DWORD dwDeltaTime)
	{
		if (m_bFirstTick)
		{
			m_bFirstTick = false;
			OnFirstTick();
		}

		return true;
	}

	//	Reset work
	virtual void Reset()
	{ 
		m_bFinished	 = false;
		m_bFirstTick = true;
	}

	//	Work is cancel
	virtual void Cancel() {}
	//	Can work transfer to another work with specified mask
	virtual bool CanTransferTo(DWORD dwMask) { return (m_dwTransMask & dwMask) ? true : false; }
	//	On work shift to
	virtual void OnWorkShift() {}

	//	Get work ID
	int GetWorkID()const { return m_iWorkID; }
	//	Is work finished ?
	bool IsFinished() { return m_bFinished; }
	//	Get work mask
	DWORD GetWorkMask() { return m_dwMask; }
	//	Get work transfer works
	DWORD GetTransferMask() { return m_dwTransMask; }

protected:	//	Attributes

	int		m_iWorkID;		//	Work ID
	bool	m_bFinished;	//	true, Work is finished
	DWORD	m_dwMask;		//	Mask of this work
	DWORD	m_dwTransMask;	//	Work transfer masks
	bool	m_bFirstTick;

protected:	//	Operations

	//	On first tick
	virtual void OnFirstTick() {}
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

