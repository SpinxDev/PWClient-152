/*
 * FILE: AScriptFile.h
 *
 * DESCRIPTION: Routines for script file
 *
 * CREATED BY: duyuxin, 2002/2/23
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#ifndef _ASCRIPTFILE_H_
#define _ASCRIPTFILE_H_

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

class AFile;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class AScriptFile
//
///////////////////////////////////////////////////////////////////////////

class AScriptFile
{
public:		//	Types

	enum
	{
		MAX_LINELEN = 2048		//	Maximum line length in characters
	};

	struct SCRIPTINFO
	{
		BYTE*	pStart;		//	Start address of buffer
		BYTE*	pEnd;		//	End address of buffer
		BYTE*	pCur;		//	Current pointer
		int		iLine;		//	Line counter
	};

public:		//	Constructors and Destructors

	AScriptFile();
	virtual ~AScriptFile();

public:		//	Attributes

	char	m_szToken[MAX_LINELEN];

public:		//	Operations

	//	Open an existing script file
	bool Open(AFile* pFile);
	bool Open(const char* szFile);
	//	Close file
	void Close();

	//	Reset pointers
	void ResetScriptFile();
	//	Get next token and move file pointer forward
	bool GetNextToken(bool bCrossLine);
	//	Peek next token and don't move file pointer
	bool PeekNextToken(bool bCrossLine);
	//	Skip current line
	bool SkipLine();
	//	Search specified token
	bool MatchToken(char* szToken, bool bCaseSensitive);

	//	Get next token as float
	inline float GetNextTokenAsFloat(bool bCrossLine);
	//	Get next token as int
	inline int GetNextTokenAsInt(bool bCrossLine);

	//	Reach end of file ?
	bool IsEnd() {	return m_Script.pCur >= m_Script.pEnd;	}
	//	Get current line
	int GetCurLine() { return m_Script.iLine; }

protected:	//	Attributes

	SCRIPTINFO	m_Script;		//	Script file information

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////

//	Get next token as float
float AScriptFile::GetNextTokenAsFloat(bool bCrossLine)
{
	GetNextToken(bCrossLine);
	return (float)atof(m_szToken);
}

//	Get next token as int
int	AScriptFile::GetNextTokenAsInt(bool bCrossLine)
{
	GetNextToken(bCrossLine);
	return atoi(m_szToken);
}


#endif	//	_ASCRIPTFILE_H_



