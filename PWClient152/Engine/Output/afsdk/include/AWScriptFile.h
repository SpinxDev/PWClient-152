/*
 * FILE: AWScriptFile.h
 *
 * DESCRIPTION: Unicode version of AScriptFile
 *
 * CREATED BY: duyuxin, 2004/6/4
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#ifndef _AWSCRIPTFILE_H_
#define _AWSCRIPTFILE_H_

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
//	Class AWScriptFile
//
///////////////////////////////////////////////////////////////////////////

class AWScriptFile
{
public:		//	Types

	enum
	{
		MAX_LINELEN = 2048		//	Maximum line length in characters
	};

	struct SCRIPTINFO
	{
		BYTE*		pFileBuf;	//	Pointer to file data buffer
		wchar_t*	pStart;		//	Start address of buffer
		wchar_t*	pEnd;		//	End address of buffer
		wchar_t*	pCur;		//	Current pointer
		int			iLine;		//	Line counter
	};

public:		//	Constructors and Destructors

	AWScriptFile();
	virtual ~AWScriptFile();

public:		//	Attributes

	wchar_t		m_szToken[MAX_LINELEN];

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
	bool MatchToken(wchar_t* wszToken, bool bCaseSensitive);

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
float AWScriptFile::GetNextTokenAsFloat(bool bCrossLine)
{
	GetNextToken(bCrossLine);
	float fVal;
	swscanf_s(m_szToken, L"%f", &fVal);
	return fVal;
}

//	Get next token as int
int	AWScriptFile::GetNextTokenAsInt(bool bCrossLine)
{
	GetNextToken(bCrossLine);
	return _wtoi(m_szToken);
}


#endif	//	_AWSCRIPTFILE_H_



