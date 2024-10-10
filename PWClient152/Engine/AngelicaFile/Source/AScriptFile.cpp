/*
 * FILE: AScriptFile.cpp
 *
 * DESCRIPTION: Routines for script file
 *
 * CREATED BY: duyuxin, 2002/2/23
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

////////////////////////////////////////////////////////////////////////////////////
//
//	Script file is pure text file without the first 4-byte flag of Angelica files. When
//	a script file is opened, it will be loaded into memory and freed when Close() is
//	called. So never forget to close a script file after you opened it.
//
//	Script file has the same comment style as C/C++, that means all content in a line
//	after // or paragraph between /* */ pair will be ignored.
//
//	Token: token is a text string. It only contain characters whose corresponding
//	ASCII codes > 32 except some special characters: , ; ( ) ", you can use these
//	characters and spece to separate tokens. Of course //, /* and */ won't be consided
//	as tokens.
//
//	For example:
//
//		abdad		is a token
//		123.22		is a token
//		[p%^@1]		is a token
//
//	A special case is, all text in a line and between "" or () will be consided as ONE
//	token. examples:
//
//	"293, is a number",	'293, is a number' will be read as a token
//	(213, 222, 10.2),	'213, 222, 10.2' will be read as a token
//	"(%$# QWE)"			'(%$# QWE)' will be read as a token
//
//	Use GetNextToken() or PeekNextToken to next token and it will be stored in
//	m_szToken of A3DScriptFile object. m_szToken is a public member so it can be accessd freely.
//
////////////////////////////////////////////////////////////////////////////////////

#include "AFPI.h"
#include "AScriptFile.h"
#include "AFileImage.h"
#include "AMemory.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

#define new A_DEBUG_NEW

///////////////////////////////////////////////////////////////////////////
//
//	Reference to External variables and functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Local Types and Variables and Global variables
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Local functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Implement
//
///////////////////////////////////////////////////////////////////////////

AScriptFile::AScriptFile()
{
	memset(&m_Script, 0, sizeof (m_Script));
}

AScriptFile::~AScriptFile()
{
	AScriptFile::Close();
}

/*	Open an existing script file and load it into memory

	Return true for success, otherwise return false

	pFile: script file's pointer. In order to be sure that script file operations
		   are always right, this file should be opened as binary file !!!
*/
bool AScriptFile::Open(AFile *pFile)
{
	DWORD dwFileLen = pFile->GetFileLength();
	if (!dwFileLen)
	{
		AFERRLOG(("AScriptFile::Open, Empty ini file"));
		return true;
	}

	BYTE* pBuf = (BYTE*)a_malloc(dwFileLen);
	if (!pBuf)
	{
		AFERRLOG(("AScriptFile::Open, Not enough memory"));
		return false;
	}

	DWORD dwRead;

	//	Read whole file into memory
	if (!pFile->Read(pBuf, dwFileLen, &dwRead) || dwRead != dwFileLen)
	{
		a_free(pBuf);
		AFERRLOG(("AScriptFile::Open, Failed to read file content"));
		return false;
	}

	m_Script.pStart	= pBuf;
	m_Script.pCur	= pBuf;
	m_Script.pEnd	= pBuf + dwFileLen;
	m_Script.iLine	= 0;

	return true;
}

bool AScriptFile::Open(const char* szFile)
{
	AFileImage File;

	if (!File.Open("", szFile, AFILE_OPENEXIST | AFILE_BINARY | AFILE_TEMPMEMORY))
	{
		File.Close();
		return false;
	}

	if (!Open(&File))
	{
		File.Close();
		return false;
	}

	File.Close();
	return true;
}

//	Close file
void AScriptFile::Close()
{
	if (m_Script.pStart)
		a_free(m_Script.pStart);
	
	memset(&m_Script, 0, sizeof (m_Script));
}

//	Reset pointers so that this file can be prase again
void AScriptFile::ResetScriptFile()
{
	m_Script.pCur	= m_Script.pStart;
	m_Script.iLine	= 0;
}

/*	Get next token and move file pointer forward.

	Return true for success, otherwise return false

	bCrossLine: true, search next token until it is found or all buffer has been checked
				false, only search next token in current line
*/	
bool AScriptFile::GetNextToken(bool bCrossLine)
{
NewLine:
	//	Search for the first character which large than 32
	while (m_Script.pCur < m_Script.pEnd)
	{
		if (*m_Script.pCur > 32 && *m_Script.pCur != ';' && 
			*m_Script.pCur != ',')
			break;

		if (*m_Script.pCur++ == '\n')
		{
			if (!bCrossLine)
			{
				m_Script.pCur--;	//	Let search pointer still stop in this line
				return false;
			}

			m_Script.iLine++;
		}
	}

	if (m_Script.pCur >= m_Script.pEnd)
		return false;

	//	Skip comment lines those begin with '//'
	if (m_Script.pCur[0] == '/' && m_Script.pCur[1] == '/')
	{
		//	This is a note line, search it's ending.
		while (m_Script.pCur < m_Script.pEnd && *m_Script.pCur != '\n')
			m_Script.pCur++;

		if (m_Script.pCur >= m_Script.pEnd)	//	Found nothing
			return false;
		else if (!bCrossLine)	//	Don't search cross line
			return false;

		m_Script.pCur++;	//	Skip '\n'
		m_Script.iLine++;
		goto NewLine;
	}

	//	Text between /* */ are also comment
	if (m_Script.pCur[0] == '/' && m_Script.pCur[1] == '*')
	{
		bool bError = false;

		m_Script.pCur += 2;		//	Skip /*

		while (m_Script.pCur[0] != '*' || m_Script.pCur[1] != '/')
		{
			if (m_Script.pCur >= m_Script.pEnd)		//	Found nothing
				return false;
			else if (*m_Script.pCur == '\n')
			{
				if (!bCrossLine)
				{
					//	This is a fatal error, we should return false. 
					//	But we must search the '*/' so that next time our begin point
					//	isn't in comment paragraph
					bError = true;
				}

				m_Script.iLine++;
			}

			m_Script.pCur++;
		}

		m_Script.pCur += 2;	//	Skip */

		if (bError)
			return false;

		goto NewLine;
	}

	int i = 0;

	//	Copy string in "" or () pair
	if (*m_Script.pCur == '"' || *m_Script.pCur == '(')
	{
		char cEnd;
		if (*m_Script.pCur == '"')
			cEnd = '"';
		else
			cEnd = ')';

		//	Quoted token
		m_Script.pCur++;	//	Skip " or (

		while (m_Script.pCur < m_Script.pEnd && *m_Script.pCur != cEnd)
		{
			if (i >= AFILE_LINEMAXLEN-1)
				return false;
			
			m_szToken[i++] = *m_Script.pCur++;
		}

		m_Script.pCur++;	//	Skip " or )
	}
	else	//	Is a normal token
	{
		while (m_Script.pCur < m_Script.pEnd && *m_Script.pCur > 32 && 
			   *m_Script.pCur != ';' && *m_Script.pCur != ',')
		{
			if (i >= AFILE_LINEMAXLEN-1)
				return FALSE;
			
			m_szToken[i++] = *m_Script.pCur++;
		}
	}

	m_szToken[i] = '\0';

	return true;
}

/*	Peek next token and don't move file pointer

	Return true for success, otherwise return false

	bCrossLine: true, search next token until it is found or all buffer has been checked
				false, only search next token in current line
*/
bool AScriptFile::PeekNextToken(bool bCrossLine)
{
	//	Record current pointer and line
	BYTE* pCur	= m_Script.pCur;
	int iLine	= m_Script.iLine;

	bool bRet = GetNextToken(bCrossLine);
	
	//	Restore pointer and line
	m_Script.pCur  = pCur;
	m_Script.iLine = iLine;

	return bRet;
}

/*	Search specified token. This function get next token and check whether it match
	specified string, if match, then stop and return true, otherwise get next token
	again until all file is checked or token is found.

	Note: This will be crossing-line search.

	Return true for success, otherwise return false.

	szToken: specified token will be searched
	bCaseSensitive: true, case sensitive
*/
bool AScriptFile::MatchToken(char* szToken, bool bCaseSensitive)
{
	while (GetNextToken(true))
	{
		if (bCaseSensitive)
		{
			if (!_stricmp(m_szToken, szToken))
				return true;
		}
		else
		{
			if (!_stricmp(m_szToken, szToken))
				return true;
		}
	}
	
	return false;
}

/*	Skip current line and ignore all rest content in it.

	Return true for success, otherwise return false
*/
bool AScriptFile::SkipLine()
{
	while (m_Script.pCur < m_Script.pEnd)
	{
		if (*m_Script.pCur++ == '\n')
			break;
	}

	m_Script.iLine++;

	return true;
}


