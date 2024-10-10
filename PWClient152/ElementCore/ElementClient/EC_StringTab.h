/*
 * FILE: EC_StringTab.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/9/25
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#pragma warning (disable: 4284)

#include "hashtab.h"
#include "AString.h"
#include "AWString.h"

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
//	Class CECStringTab
//	
///////////////////////////////////////////////////////////////////////////

class CECStringTab
{
public:		//	Types

	typedef abase::hashtab<AString*, int, abase::_hash_function> AStrTable;
	typedef abase::hashtab<AWString*, int, abase::_hash_function> WStrTable;

public:		//	Constructor and Destructor

	CECStringTab();
	virtual ~CECStringTab();

public:		//	Attributes

public:		//	Operations

	//	Initialize object
	bool Init(const char* szFile, bool bUnicode);
	//	Release object
	void Release();

	//	Get specified ANSI string
	const char* GetANSIString(int n)
	{
		AStrTable::pair_type Pair = m_AStrTab.get(n);
		if (Pair.second)
			return **Pair.first;
		else
			return NULL;
	}

	//	Get specified unicode string
	const wchar_t* GetWideString(int n)
	{
		WStrTable::pair_type Pair = m_WStrTab.get(n);
		if (Pair.second)
			return **Pair.first;
		else
			return NULL;
	}
	
	AWString GetWideStringObject(int n)
	{
		return GetWideString(n);
	}

	//	Get initialized flag
	bool IsInitialized() { return m_bInit; }

protected:	//	Attributes

	AStrTable	m_AStrTab;	//	ANSI string table
	WStrTable	m_WStrTab;	//	Unicode string table

	bool	m_bInit;		//	true, string talbe has been initlaized
	bool	m_bUnicode;		//	true, strings in table is unicode

protected:	//	Operations

	//	Load ANSI strings from file
	bool LoadANSIStrings(const char* szFile);
	//	Load Unicode strings from file
	bool LoadWideStrings(const char* szFile);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////


