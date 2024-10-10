/*
 * FILE: ATextTabFile.h
 *
 * DESCRIPTION: The class operate text table exported by Excel
 *
 * CREATED BY: Duyuxin, 2004/3/9
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#ifndef _ATEXTTABFILE_H_
#define _ATEXTTABFILE_H_

#include "AArray.h"
#include "AString.h"

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

class AFile;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class ATextTabFile
//	
///////////////////////////////////////////////////////////////////////////

class ATextTabFile
{
public:		//	Types

	typedef APtrArray<AString*>	ColArray;

	//	Struct used to parse text table file
	struct PARSEINFO
	{
		BYTE*	pStart;		//	Start address of buffer
		BYTE*	pEnd;		//	End address of buffer
		BYTE*	pCur;		//	Current pointer
		int		iLine;		//	Line counter
	};

public:		//	Constructor and Destructor

	ATextTabFile();
	virtual ~ATextTabFile();

public:		//	Attributes

public:		//	Operations

	//	Open an existing text table file
	bool Open(AFile* pFile);
	bool Open(const char* szFile);
	//	Close file
	void Close();

	//	Get table item
	const AString& GetItem(int iRow, int iCol) { return *(*m_aRows[iRow])[iCol]; }
	//	Get row number
	int GetRowNum() { return m_aRows.GetSize(); }
	//	Get column number of specified row
	int GetColNum(int iRow) { return m_aRows[iRow]->GetSize(); }

protected:	//	Attributes

	PARSEINFO	m_ParseInfo;

	APtrArray<ColArray*>	m_aRows;	//	Rows

protected:	//	Operations

	//	Parse text table file
	bool ParseFile();
	//	Parse a line of text file
	ColArray* ParseFileLine();
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////


#endif	//	_ATEXTTABFILE_H_
