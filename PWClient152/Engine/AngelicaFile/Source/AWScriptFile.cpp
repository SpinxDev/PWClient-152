/*
 * FILE: AWScriptFile.cpp
 *
 * DESCRIPTION: Unicode version of AScriptFile
 *
 * CREATED BY: duyuxin, 2002/6/4
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#include "AFPI.h"
#include "AWScriptFile.h"
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

AWScriptFile::AWScriptFile()
{
	memset(&m_Script, 0, sizeof (m_Script));
}

AWScriptFile::~AWScriptFile()
{
	AWScriptFile::Close();
}

/*	Open an existing script file and load it into memory

	Return true for success, otherwise return false

	pFile: script file's pointer. In order to be sure that script file operations
		   are always right, this file should be opened as binary file !!!
*/
bool AWScriptFile::Open(AFile *pFile)
{
	DWORD dwFileLen = pFile->GetFileLength();
	if (!dwFileLen)
	{
		AFERRLOG(("AWScriptFile::Open, Empty ini file"));
		return true;
	}

	BYTE* pBuf = (BYTE*)a_malloc(dwFileLen);
	if (!pBuf)
	{
		AFERRLOG(("AWScriptFile::Open, Not enough memory"));
		return false;
	}

	DWORD dwRead;

	//	Read whole file into memory
	if (!pFile->Read(pBuf, dwFileLen, &dwRead) || dwRead != dwFileLen)
	{
		a_free(pBuf);
		AFERRLOG(("AWScriptFile::Open, Failed to read file content"));
		return false;
	}

	//	Check unicode file header
	wchar_t wChar = *((wchar_t*)pBuf);
	if (wChar != 0xfeff)
	{
		a_free(pBuf);
		AFERRLOG(("AWScriptFile::Open, Couldn't handle non-unicode file !"));
		return false;
	}

	m_Script.pFileBuf	= pBuf;
	m_Script.pStart		= ((wchar_t*)pBuf) + 1;		//	Skip unicode magic number
	m_Script.pCur		= m_Script.pStart;
	m_Script.pEnd		= (wchar_t*)(pBuf + dwFileLen);
	m_Script.iLine		= 0;

	return true;
}

bool AWScriptFile::Open(const char* szFile)
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
void AWScriptFile::Close()
{
	if (m_Script.pFileBuf)
		a_free(m_Script.pFileBuf);
	
	memset(&m_Script, 0, sizeof (m_Script));
}

//	Reset pointers so that this file can be prase again
void AWScriptFile::ResetScriptFile()
{
	m_Script.pCur	= m_Script.pStart;
	m_Script.iLine	= 0;
}

/*	Get next token and move file pointer forward.

	Return true for success, otherwise return false

	bCrossLine: true, search next token until it is found or all buffer has been checked
				false, only search next token in current line
*/	
bool AWScriptFile::GetNextToken(bool bCrossLine)
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
			if (i >= MAX_LINELEN-1)
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
			if (i >= MAX_LINELEN-1)
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
bool AWScriptFile::PeekNextToken(bool bCrossLine)
{
	//	Record current pointer and line
	wchar_t* pCur = m_Script.pCur;
	int iLine = m_Script.iLine;

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

	wszToken: specified token will be searched
	bCaseSensitive: true, case sensitive
*/
bool AWScriptFile::MatchToken(wchar_t* wszToken, bool bCaseSensitive)
{
	while (GetNextToken(true))
	{
		if (bCaseSensitive)
		{
			if (!_wcsicmp(m_szToken, wszToken))
				return true;
		}
		else
		{
			if (!_wcsicmp(m_szToken, wszToken))
				return true;
		}
	}
	
	return false;
}

/*	Skip current line and ignore all rest content in it.

	Return true for success, otherwise return false
*/
bool AWScriptFile::SkipLine()
{
	while (m_Script.pCur < m_Script.pEnd)
	{
		if (*m_Script.pCur++ == '\n')
			break;
	}

	m_Script.iLine++;

	return true;
}


