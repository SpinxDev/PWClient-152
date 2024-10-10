/*
* FILE: RandStringContainer.h
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan, 2010/05/13
*
* HISTORY: 
*
* Copyright (c) 2010 Archosaur Studio, All Rights Reserved.
*/

#ifndef _RandStringContainer_H_
#define _RandStringContainer_H_

#include <AArray.h>
#include <AString.h>

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
//	Class RandStringContainer
//	
///////////////////////////////////////////////////////////////////////////

class RandStringContainer
{

public:		//	Types

public:		//	Constructor and Destructor

	explicit RandStringContainer(int iMaxNum);
	RandStringContainer(const RandStringContainer& rhs);
	RandStringContainer& operator = (const RandStringContainer& rhs);
	virtual ~RandStringContainer(void);

public:		//	Attributes

public:		//	Operations

	void RemoveAll() { m_aItems.clear(); }
	//	Unique add one string, the string must not be empty
	bool UniqueAdd(const char* szString);
	const char* GetString(int iIdx) const { return m_aItems[iIdx]; }
	//	Set string at the specified index, the string must not be empty, or already exist
	bool SetString(int iIdx, const char* szString);
	//	Is the string already exist
	bool IsExist(const AString& strFind);
	//	Get random string from existing strings
	const char* GetRandString() const;
	int GetSize() const { return static_cast<int>(m_aItems.size()); }
	int GetMaxNum() const { return m_iMaxNum; }

	bool operator == (const RandStringContainer& rhs) const;

protected:	//	Attributes

	abase::vector<AString> m_aItems;
	int m_iMaxNum;

protected:	//	Operations
	
	void swap(RandStringContainer& rhs);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////


#endif	//	_RandStringContainer_H_


