/*
 * FILE: EC_Counter.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2005/2/18
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
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
//	Class CECCounter
//	
///////////////////////////////////////////////////////////////////////////

class CECCounter
{
public:		//	Types

public:		//	Constructor and Destructor

	CECCounter()
	{
		m_dwCounter = 0;
		m_dwPeriod	= 0;
	}

public:		//	Attributes

public:		//	Operations

	//	Set / Get period
	void SetPeriod(DWORD dwPeriod) { m_dwPeriod = dwPeriod; }
	DWORD GetPeriod() { return m_dwPeriod; }
	//	Set / Get counter
	void SetCounter(DWORD dwCounter) { m_dwCounter = dwCounter; }
	DWORD GetCounter() { return m_dwCounter; }

	//	Has counter reached period ?
	bool IsFull() { return (m_dwCounter >= m_dwPeriod) ? true : false; }
	//	Reset counter
	void Reset(bool bFull=false) { m_dwCounter = bFull ? m_dwPeriod : 0; }

	//	Increase counter
	bool IncCounter(DWORD dwCounter)
	{ 
		m_dwCounter += dwCounter;
		return (m_dwCounter >= m_dwPeriod) ? true : false;
	}

	//	Decrease counter
	void DecCounter(DWORD dwCounter)
	{ 
		if (m_dwCounter <= dwCounter)
			m_dwCounter = 0;
		else
			m_dwCounter -= dwCounter;
	}

protected:	//	Attributes

	DWORD	m_dwCounter;		//	Counter
	DWORD	m_dwPeriod;			//	Count period

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////


