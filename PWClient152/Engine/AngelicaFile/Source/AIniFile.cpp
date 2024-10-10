/*
 * FILE: AIniFile.cpp
 *
 * DESCRIPTION: Class for ini file operation
 *
 * CREATED BY: duyuxin, 2003/10/25
 *
 * HISTORY:
 *
 * Copyright (c) 2003 Archosaur Studio, All Rights Reserved.
 */

#include "AIniFile.h"
#include "AFileImage.h"
#include "AFI.h"
#include "AFPI.h"
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
//	Implement AIniFile
//
///////////////////////////////////////////////////////////////////////////

AIniFile::AIniFile()
{
	m_bOpened	= false;
	m_pCurSect	= NULL;
}

AIniFile::~AIniFile()
{
	if (m_bOpened)
		AIniFile::Close();
}

/*	Open ini file

	Return true for success, otherwise return false.

	szFile: ini file's name, can be both absolute path and relative path.
*/
bool AIniFile::Open(const char* szFile)
{
	AFileImage File;

	if (!File.Open("", szFile, AFILE_OPENEXIST | AFILE_BINARY | AFILE_TEMPMEMORY))
	{
		File.Close();
		AFERRLOG(("AIniFile::Open Can't open file [%s].", szFile));		
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

/*	Open ini file

	Return true for success, otherwise return false.

	pFile: AFile object which has been opened using binary mode.
*/
bool AIniFile::Open(AFile* pFile)
{
	ASSERT(pFile);

	if (m_bOpened)
	{
		ASSERT(0);
		AFERRLOG(("AIniFile::Open, ini file couldn't be opened twice"));
		return false;
	}

	DWORD dwFileLen = pFile->GetFileLength();
	if (!dwFileLen)
	{
		AFERRLOG(("AIniFile::Open, Empty ini file"));
		return true;
	}

	//	Load all file into memory
	BYTE* pBuf = (BYTE*)a_malloctemp(dwFileLen);
	if (!pBuf)
	{
		AFERRLOG(("AIniFile::Open, Not enough memory"));
		return true;
	}

	DWORD dwRead;

	if (!pFile->Read(pBuf, dwFileLen, &dwRead) || dwRead != dwFileLen)
	{
		a_freetemp(pBuf);
		AFERRLOG(("AIniFile::Open, Failed to read file content"));
		return true;
	}

	m_pCurSect = NULL;

	if (!ParseFile(pBuf, pBuf + dwFileLen))
		AFERRLOG(("AIniFile::Open, Failed to parse ini file"));

	a_freetemp(pBuf);

	m_bOpened = true;

	return true;
}

//	Close file
void AIniFile::Close()
{
	//	Release all sections and keys
	for (int i=0; i < m_aSects.GetSize(); i++)
	{
		s_SECTION* pSect = m_aSects[i];

		for (int j=0; j < pSect->aKeys.GetSize(); j++)
			delete pSect->aKeys[j];

		delete pSect;
	}

	m_aSects.RemoveAll();

	m_bOpened = false;
}

/*	Parse file.

	Return true for success, otherwise return false.

	pBuf: file buffer.
	pEnd: file buffer ending position
*/
bool AIniFile::ParseFile(BYTE* pBuf, BYTE* pEnd)
{
	BYTE* pLine = pBuf;
	int iLineLen;

	while (1)
	{
		if (!(iLineLen = GetLineLength(pLine, pEnd)))
			break;
		
		ParseLine(pLine, pLine+iLineLen);

		pLine += iLineLen;
	}

	return true;
}

/*	Get line length

	Return line's length in bytes

	pLine: line buffer.
	pEnd: file buffer ending position
*/
int AIniFile::GetLineLength(BYTE* pLine, BYTE* pEnd)
{
	BYTE* pCur = pLine;
	int iLen = 0;

	while (pCur < pEnd && *pCur !='\n')
	{
		iLen++;
		pCur++;
	}

	if (pCur < pEnd && *pCur == '\n')
		iLen++;

	return iLen;
}

/*	Add a section

	Return section's address for success, otherwise return NULL 

	bComment: true, comment section
	strName: section's name
*/
AIniFile::s_SECTION* AIniFile::AddSection(bool bComment, const AString& strName)
{
	s_SECTION* pSection = new s_SECTION;
	if (!pSection)
		return NULL;

	pSection->bComment = bComment;
	pSection->strName  = strName;

	m_aSects.Add(pSection);

	return pSection;
}

/*	Create a key

	Return key's address for success, otherwise return NULL.

	strKey: key's name
*/
AIniFile::s_KEY* AIniFile::CreateKey(const AString& strKey)
{
	s_KEY* pKey = new s_KEY;
	if (!pKey)
		return NULL;

	pKey->strKey = strKey;
	return pKey;
}

/*	Parse a line.

	pLine: line buffer.
	pEnd: line buffer ending position
*/
void AIniFile::ParseLine(BYTE* pLine, BYTE* pEnd)
{
	BYTE* pCur = pLine;

	//	Search for the first character which large than 32
	while (pCur < pEnd && *pCur <= 32)
		pCur++;
	
	//	Empty line ?
	if (pCur >= pEnd)
		return;

	//	Comment line ?
	if (*pCur == ';')
	{
		BYTE* pBegin = pCur;
		while (pCur < pEnd && *pCur != '\n')
			pCur++;

		//	Get comment content
		AString strName((const char*)pBegin, pCur-pBegin);
		strName.TrimRight();

		//	Create section
		AddSection(true, strName);
		return;
	}

	//	New section ?
	if (*pCur == '[')
	{
		BYTE* pBegin = ++pCur;
		while (pCur < pEnd && *pCur != ']')
			pCur++;

		//	[] doesn't match ?
		if (pCur >= pEnd)
			return;

		//	Check section name
		AString strName((const char*)pBegin, pCur-pBegin);
		strName.TrimRight();

		//	Create section
		m_pCurSect = AddSection(false, strName);
		return;
	}

	//	Parse key-values ...
	if (!m_pCurSect)
		return;

	BYTE* pBegin = pCur;
	while (pCur < pEnd && *pCur != '=')
		pCur++;

	//	Didn't find '=' ?
	if (*pCur != '=')
		return;

	//	Get key name
	AString strName((const char*)pBegin, pCur-pBegin);
	strName.TrimRight();

	s_KEY* pKey = CreateKey(strName);
	ASSERT(pKey);

	pKey->strKey = strName;

	if (!ParseValue(pKey, pCur+1, pEnd))
		delete pKey;
	else
		m_pCurSect->aKeys.Add(pKey);
}

/*	Parse key value

	Return true for success, otherwise return false.

	pLine: line buffer.
	pEnd: line buffer ending position
*/
bool AIniFile::ParseValue(s_KEY* pKey, BYTE* pBuf, BYTE* pEnd)
{
	BYTE* pCur = pBuf;

	//	Search for the first character which large than 32
	while (pCur < pEnd && (*pCur <= 32 || *pCur == ','))
		pCur++;
	
	if (pCur >= pEnd)
		return false;

	pKey->strValue = AString((const char*)pCur, pEnd-pCur);
	pKey->strValue.TrimRight();

	return true;
}

/*	Get values as integer

	Return integer value.

	szSect: section name
	szKey: key name
	iDefault: default value will be used if specified value was failed to be found
*/
int AIniFile::GetValueAsInt(const char* szSect, const char* szKey, int iDefault)
{
	if (!m_bOpened)
		return iDefault;

	AString* pstrVal = SearchValue(szSect, szKey);
	if (pstrVal)
		return pstrVal->ToInt();

	return iDefault;
}

/*	Get values as string

	Return integer value.

	szSect: section name
	szKey: key name
	iDefault: default value will be used if specified value was failed to be found
*/
AString AIniFile::GetValueAsString(const char* szSect, const char* szKey, 
								   const char* szDefault/* NULL */)
{
	if (!m_bOpened)
		return AString(szDefault);

	AString* pstrVal = SearchValue(szSect, szKey);
	if (pstrVal)
		return *pstrVal;

	return AString(szDefault);
}

/*	Get values as string

	Return integer value.

	szSect: section name
	szKey: key name
	iDefault: default value will be used if specified value was failed to be found
*/
float AIniFile::GetValueAsFloat(const char* szSect, const char* szKey, float fDefault)
{
	if (!m_bOpened)
		return fDefault;

	AString* pstrVal = SearchValue(szSect, szKey);
	if (pstrVal)
		return pstrVal->ToFloat();

	return fDefault;
}

/*	Get values as integer array

	Return true for success, otherwise return false.

	szSect: section name
	szKey: key name
	iNumInt: number of integer
	pBuf: array buffer
*/
bool AIniFile::GetValueAsIntArray(const char* szSect, const char* szKey, int iNumInt, int* pBuf)
{
	if (!m_bOpened)
		return false;

	AString* pstrVal = SearchValue(szSect, szKey);
	if (!pstrVal)
		return false;

	const char* pCur = *pstrVal;
	const char* pEnd = pCur + pstrVal->GetLength();

	for (int i=0; i < iNumInt; i++)
	{
		//	Search for the first character which large than 32
		while (pCur < pEnd && (*pCur <= 32 || *pCur == ','))
			pCur++;
	
		if (pCur >= pEnd)
			return false;

		const char* pBegin = pCur;
		while (pCur < pEnd && *pCur > 32 && *pCur != ',')
			pCur++;

		AString strValue((const char*)pBegin, pCur-pBegin);
		pBuf[i] = strValue.ToInt();
	}

	return true;
}

bool AIniFile::GetValueAsFloatArray(const char* szSect, const char* szKey, int iNumFloat, float* pBuf)
{
	if (!m_bOpened)
		return false;

	AString* pstrVal = SearchValue(szSect, szKey);
	if (!pstrVal)
		return false;

	const char* pCur = *pstrVal;
	const char* pEnd = pCur + pstrVal->GetLength();

	for (int i=0; i < iNumFloat; i++)
	{
		//	Search for the first character which large than 32
		while (pCur < pEnd && (*pCur <= 32 || *pCur == ','))
			pCur++;
	
		if (pCur >= pEnd)
			return false;

		const char* pBegin = pCur;
		while (pCur < pEnd && *pCur > 32 && *pCur != ',')
			pCur++;

		AString strValue((const char*)pBegin, pCur-pBegin);
		pBuf[i] = strValue.ToFloat();
	}

	return true;
}

bool AIniFile::GetValueAsIntRect(const char* szSect, const char* szKey, ARectI* pRect)
{
	int aVals[4];
	if (!GetValueAsIntArray(szSect, szKey, 4, aVals))
		return false;
	
	pRect->SetRect(aVals[0], aVals[1], aVals[2], aVals[3]);
	return true;
}

/*	Write integer value

	Return true for success, otherwise return false.
	
	szSect: section name
	szKey: key name
	iValue: value will be written.
*/
bool AIniFile::WriteIntValue(const char* szSect, const char* szKey, int iValue)
{
	s_KEY* pKey = GetKey(szSect, szKey);
	if (!pKey)
		return false;

	pKey->strValue.Format("%d", iValue);

	return true;
}

/*	Write integer value

	Return true for success, otherwise return false.
	
	szSect: section name
	szKey: key name
	szValue: value will be written.
*/
bool AIniFile::WriteStringValue(const char* szSect, const char* szKey, const char* szValue)
{
	s_KEY* pKey = GetKey(szSect, szKey);
	if (!pKey)
		return false;

	pKey->strValue = szValue;

	return true;
}

/*	Write integer value

	Return true for success, otherwise return false.
	
	szSect: section name
	szKey: key name
	fValue: value will be written.
*/
bool AIniFile::WriteFloatValue(const char* szSect, const char* szKey, float fValue)
{
	s_KEY* pKey = GetKey(szSect, szKey);
	if (!pKey)
		return false;

	pKey->strValue.Format("%f", fValue);

	return true;
}

/*	Search specified value string

	Return string's address for success, otherwise return NULL.

	szSect: section name
	szKey: key name
*/
AString* AIniFile::SearchValue(const char* szSect, const char* szKey)
{
	ASSERT(szSect && szSect[0]);
	ASSERT(szKey && szKey[0]);

	int i, j;

	for (i=0; i < m_aSects.GetSize(); i++)
	{
		s_SECTION* pSect = m_aSects[i];

		if (pSect->bComment || pSect->strName.CompareNoCase(szSect))
			continue;

		for (j=0; j < pSect->aKeys.GetSize(); j++)
		{
			s_KEY* pKey = pSect->aKeys[j];
			if (pKey->strKey.CompareNoCase(szKey))
				continue;

			return &pKey->strValue;
		}
	}

	return NULL;
}

/*	Search specified section

	Return section's address for success, otherwise return NULL.

	szSect: section's name
*/
AIniFile::s_SECTION* AIniFile::SearchSection(const char* szSect)
{
	for (int i=0; i < m_aSects.GetSize(); i++)
	{
		s_SECTION* pSection = m_aSects[i];

		if (!pSection->bComment && !pSection->strName.CompareNoCase(szSect))
			return pSection;
	}

	return NULL;
}

/*	Search key in specified section

	Return key's address for success, otherwise return NULL.

	pSection: section in key expected to exist.
	szKey: key's name
*/
AIniFile::s_KEY* AIniFile::SearchKey(s_SECTION* pSection, const char* szKey)
{
	for (int i=0; i < pSection->aKeys.GetSize(); i++)
	{
		s_KEY* pKey = pSection->aKeys[i];
		if (!pKey->strKey.CompareNoCase(szKey))
			return pKey;
	}

	return NULL;
}

/*	Get specified key, create if it doesn't exist

	Return key's address for success, otherwise return false.

	szSect: section name
	szKey: key name
*/
AIniFile::s_KEY* AIniFile::GetKey(const char* szSect, const char* szKey)
{
	ASSERT(szSect && szSect[0]);
	ASSERT(szKey && szKey[0]);

	s_SECTION* pSection = SearchSection(szSect);
	if (!pSection)
	{
		//	Create a new section
		pSection = AddSection(false, szSect);
	}

	s_KEY* pKey = SearchKey(pSection, szKey);
	if (!pKey)
	{
		//	Create a new key
		pKey = CreateKey(szKey);
		pSection->aKeys.Add(pKey);
	}

	return pKey;
}

//	Save modified data
bool AIniFile::Save(const char* szFile)
{
	FILE* fp = fopen(szFile, "w+");
	if (!fp)
	{
		AFERRLOG(("AIniFile::Save, Failed to create flie %s", szFile));
		return false;
	}

	int i, j;
	AString strValue;

	for (i=0; i < m_aSects.GetSize(); i++)
	{
		s_SECTION* pSect = m_aSects[i];
		if (pSect->bComment)
		{
			fprintf(fp, "%s\n", pSect->strName);
			continue;
		}

		//	Write section name
		strValue = "[" + pSect->strName + "]";
		fprintf(fp, "%s\n", strValue);

		for (j=0; j < pSect->aKeys.GetSize(); j++)
		{
			s_KEY* pKey = pSect->aKeys[j];
			
			//	Write key name and value
			strValue = pKey->strKey + " = " + pKey->strValue;
			fprintf(fp, "%s\n", strValue);
		}

		fprintf(fp, "\n");
	}

	fclose(fp);

	return true;
}

bool AIniFile::Save(AFile* pFile)
{
	int i, j;
	AString strValue;

	for (i=0; i < m_aSects.GetSize(); i++)
	{
		s_SECTION* pSect = m_aSects[i];
		if (pSect->bComment)
		{
			pFile->WriteLine(pSect->strName);
			continue;
		}

		//	Write section name
		strValue = "[" + pSect->strName + "]";
		pFile->WriteLine(strValue);

		for (j=0; j < pSect->aKeys.GetSize(); j++)
		{
			s_KEY* pKey = pSect->aKeys[j];
			
			//	Write key name and value
			strValue = pKey->strKey + " = " + pKey->strValue;
			pFile->WriteLine(strValue);
		}

		pFile->WriteLine("");
	}

	return true;
}

