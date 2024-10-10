/*
 * FILE: ATextTabFile.cpp
 *
 * DESCRIPTION: The class operate text table exported by Excel
 *
 * CREATED BY: Duyuxin, 2004/3/9
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "ATextTabFile.h"
#include "AFPI.h"
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
//	Implement ATextTabFile
//	
///////////////////////////////////////////////////////////////////////////

ATextTabFile::ATextTabFile()
{
}

ATextTabFile::~ATextTabFile()
{
	ATextTabFile::Close();
}

//	Open an existing text table file
bool ATextTabFile::Open(AFile* pFile)
{
	BYTE* pBuf;
	DWORD dwRead;

	pFile->Seek(0, AFILE_SEEK_END);
	int iLen = pFile->GetPos();
	pFile->Seek(0, AFILE_SEEK_SET);

	if (!(pBuf = (BYTE*)a_malloc(iLen + 1)))
		return false;

	//	Read data
	pFile->Read(pBuf, iLen, &dwRead);

	//	Add a 0 at the end of whole buffer
	pBuf[iLen] = 0;

	//	Restore file's pointer so normal file operations also can be done
	pFile->Seek(0, AFILE_SEEK_SET);

	m_ParseInfo.pStart	= pBuf;
	m_ParseInfo.pCur	= pBuf;
	m_ParseInfo.pEnd	= pBuf + iLen;
	m_ParseInfo.iLine	= 0;

	if (!ParseFile())
	{
		a_free(pBuf);
		AFERRLOG(("ATextTabFile::Open, Failed to parse file"));
		return false;
	}

	a_free(pBuf);

	return true;
}

bool ATextTabFile::Open(const char* szFile)
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
void ATextTabFile::Close()
{
	//	Release all table content
	for (int i=0; i < m_aRows.GetSize(); i++)
	{
		ColArray& Columns = *m_aRows[i];

		for (int j=0; j < Columns.GetSize(); j++)
			delete Columns[j];

		delete m_aRows[i];
	}

	m_aRows.RemoveAll();
}

//	Parse text table file
bool ATextTabFile::ParseFile()
{
	while (1)
	{
		ColArray* pColumn = ParseFileLine();
		if (!pColumn)
			break;

		m_aRows.Add(pColumn);
	}

	return true;
}

//	Parse a line of text file
ATextTabFile::ColArray* ATextTabFile::ParseFileLine()
{
	ColArray* pColumn = new ColArray;
	BYTE* pCur = m_ParseInfo.pCur;

	while (pCur < m_ParseInfo.pEnd)
	{
		BYTE* pStart = pCur;
		while (pCur < m_ParseInfo.pEnd && *pCur != '\n' && *pCur != '\t')
			pCur++;

		if (pCur >= m_ParseInfo.pEnd)
			break;

		AString* pStr = new AString((char*)pStart, pCur-pStart);
		pColumn->Add(pStr);

		if (*pCur == '\n')
		{
			pCur++;	//	Skip '\n'
			break;
		}

		pCur++;	//	Skip '\t'
	}

	m_ParseInfo.pCur = pCur;
	m_ParseInfo.iLine++;

	if (!pColumn->GetSize())
	{
		delete pColumn;
		return NULL;
	}

	return pColumn;
}

