/*
 * FILE: AWIniFile.cpp
 *
 * DESCRIPTION: Class for ini file operation
 *
 * CREATED BY: duyuxin, 2003/10/25
 *
 * HISTORY:
 *
 * Copyright (c) 2003 Archosaur Studio, All Rights Reserved.
 */

#include "AWIniFile.h"
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
//	Implement AWIniFile
//
///////////////////////////////////////////////////////////////////////////

AWIniFile::AWIniFile()
{
	m_bOpened	= false;
	m_pCurSect	= NULL;
}

AWIniFile::~AWIniFile()
{
	if (m_bOpened)
		AWIniFile::Close();
}

/*	Open ini file

	Return true for success, otherwise return false.

	szFile: ini file's name, can be both absolute path and relative path.
*/
bool AWIniFile::Open(const char* szFile)
{
	AFileImage File;

	if (!File.Open("", szFile, AFILE_OPENEXIST | AFILE_BINARY | AFILE_TEMPMEMORY))
	{
		File.Close();
		AFERRLOG(("AWIniFile::Open Can't open file [%s].", szFile));		
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
bool AWIniFile::Open(AFile* pFile)
{
	ASSERT(pFile);

	if (m_bOpened)
	{
		ASSERT(0);
		AFERRLOG(("AWIniFile::Open, ini file couldn't be opened twice"));
		return false;
	}

	DWORD dwFileLen = pFile->GetFileLength();
	if (!dwFileLen)
	{
		AFERRLOG(("AWIniFile::Open, Empty ini file"));
		return true;
	}

	//	Load all file into memory
	BYTE* pBuf = (BYTE*)a_malloc(dwFileLen);
	if (!pBuf)
	{
		AFERRLOG(("AWIniFile::Open, Not enough memory"));
		return true;
	}

	DWORD dwRead;

	if (!pFile->Read(pBuf, dwFileLen, &dwRead) || dwRead != dwFileLen)
	{
		a_free(pBuf);
		AFERRLOG(("AWIniFile::Open, Failed to read file content"));
		return true;
	}

	//	Check unicode file header
	wchar_t* pBegin = (wchar_t*)pBuf;
	if (*pBegin != 0xfeff)
	{
		a_free(pBuf);
		AFERRLOG(("AWIniFile::Open, file %s is non-unicode!", pFile->GetRelativeName()));
		return false;
	}

	m_pCurSect = NULL;

	if (!ParseFile((BYTE*)(pBegin+1), pBuf + dwFileLen))
		AFERRLOG(("AWIniFile::Open, Failed to parse ini file"));

	a_free(pBuf);

	m_bOpened = true;

	return true;
}

//	Close file
void AWIniFile::Close()
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
bool AWIniFile::ParseFile(BYTE* pBuf, BYTE* pEnd)
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
int AWIniFile::GetLineLength(BYTE* pLine, BYTE* pEnd)
{
	wchar_t* pCur = (wchar_t*)pLine;
	int iLen = 0;

	while (pCur < (wchar_t*)pEnd && *pCur !='\n')
	{
		iLen++;
		pCur++;
	}

	if (pCur < (wchar_t*)pEnd && *pCur == '\n')
		iLen++;

	return iLen * sizeof (wchar_t);
}

/*	Add a section

	Return section's address for success, otherwise return NULL 

	bComment: true, comment section
	strName: section's name
*/
AWIniFile::s_SECTION* AWIniFile::AddSection(bool bComment, const AWString& strName)
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
AWIniFile::s_KEY* AWIniFile::CreateKey(const AWString& strKey)
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
void AWIniFile::ParseLine(BYTE* pLine, BYTE* pEnd)
{
	wchar_t* pCur = (wchar_t*)pLine;

	//	Search for the first character which large than 32
	while (pCur < (wchar_t*)pEnd && *pCur <= 32)
		pCur++;
	
	//	Empty line ?
	if (pCur >= (wchar_t*)pEnd)
		return;

	//	Comment line ?
	if (*pCur == ';')
	{
		const wchar_t* pBegin = pCur;
		while (pCur < (wchar_t*)pEnd && *pCur != '\n')
			pCur++;

		//	Get comment content
		AWString strName(pBegin, pCur-pBegin);
		strName.TrimRight();

		//	Create section
		AddSection(true, strName);
		return;
	}

	//	New section ?
	if (*pCur == '[')
	{
		const wchar_t* pBegin = ++pCur;
		while (pCur < (wchar_t*)pEnd && *pCur != ']')
			pCur++;

		//	[] doesn't match ?
		if (pCur >= (wchar_t*)pEnd)
			return;

		//	Check section name
		AWString strName(pBegin, pCur-pBegin);
		strName.TrimRight();

		//	Create section
		m_pCurSect = AddSection(false, strName);
		return;
	}

	//	Parse key-values ...
	if (!m_pCurSect)
		return;

	const wchar_t* pBegin = pCur;
	while (pCur < (wchar_t*)pEnd && *pCur != '=')
		pCur++;

	//	Didn't find '=' ?
	if (*pCur != '=')
		return;

	//	Get key name
	AWString strName(pBegin, pCur-pBegin);
	strName.TrimRight();

	s_KEY* pKey = CreateKey(strName);
	ASSERT(pKey);

	pKey->strKey = strName;

	if (!ParseValue(pKey, (BYTE*)(pCur+1), pEnd))
		delete pKey;
	else
		m_pCurSect->aKeys.Add(pKey);
}

/*	Parse key value

	Return true for success, otherwise return false.

	pLine: line buffer.
	pEnd: line buffer ending position
*/
bool AWIniFile::ParseValue(s_KEY* pKey, BYTE* pBuf, BYTE* pEnd)
{
	wchar_t* pCur = (wchar_t*)pBuf;

	//	Search for the first character which large than 32
	while (pCur < (wchar_t*)pEnd && (*pCur <= 32 || *pCur == ','))
		pCur++;
	
	if (pCur >= (wchar_t*)pEnd)
		return false;

	pKey->strValue = AWString(pCur, (wchar_t*)pEnd-pCur);
	pKey->strValue.TrimRight();

	return true;
}

/*	Get values as integer

	Return integer value.

	szSect: section name
	szKey: key name
	iDefault: default value will be used if specified value was failed to be found
*/
int AWIniFile::GetValueAsInt(const wchar_t* szSect, const wchar_t* szKey, int iDefault)
{
	if (!m_bOpened)
		return iDefault;

	AWString* pstrVal = SearchValue(szSect, szKey);
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
AWString AWIniFile::GetValueAsString(const wchar_t* szSect, const wchar_t* szKey, 
								   const wchar_t* szDefault/* NULL */)
{
	if (!m_bOpened)
		return AWString(szDefault);

	AWString* pstrVal = SearchValue(szSect, szKey);
	if (pstrVal)
		return *pstrVal;

	return AWString(szDefault);
}

/*	Get values as string

	Return integer value.

	szSect: section name
	szKey: key name
	iDefault: default value will be used if specified value was failed to be found
*/
float AWIniFile::GetValueAsFloat(const wchar_t* szSect, const wchar_t* szKey, float fDefault)
{
	if (!m_bOpened)
		return fDefault;

	AWString* pstrVal = SearchValue(szSect, szKey);
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
bool AWIniFile::GetValueAsIntArray(const wchar_t* szSect, const wchar_t* szKey, int iNumInt, int* pBuf)
{
	if (!m_bOpened)
		return false;

	AWString* pstrVal = SearchValue(szSect, szKey);
	if (!pstrVal)
		return false;

	const wchar_t* pCur = *pstrVal;
	const wchar_t* pEnd = pCur + pstrVal->GetLength();

	for (int i=0; i < iNumInt; i++)
	{
		//	Search for the first character which large than 32
		while (pCur < pEnd && (*pCur <= 32 || *pCur == ','))
			pCur++;
	
		if (pCur >= pEnd)
			return false;

		const wchar_t* pBegin = pCur;
		while (pCur < pEnd && *pCur > 32 && *pCur != ',')
			pCur++;

		AWString strValue(pBegin, pCur-pBegin);
		pBuf[i] = strValue.ToInt();
	}

	return true;
}

bool AWIniFile::GetValueAsFloatArray(const wchar_t* szSect, const wchar_t* szKey, 
								int iNumFloat, float* pBuf)
{
	if (!m_bOpened)
		return false;

	AWString* pstrVal = SearchValue(szSect, szKey);
	if (!pstrVal)
		return false;

	const wchar_t* pCur = *pstrVal;
	const wchar_t* pEnd = pCur + pstrVal->GetLength();

	for (int i=0; i < iNumFloat; i++)
	{
		//	Search for the first character which large than 32
		while (pCur < pEnd && (*pCur <= 32 || *pCur == ','))
			pCur++;
	
		if (pCur >= pEnd)
			return false;

		const wchar_t* pBegin = pCur;
		while (pCur < pEnd && *pCur > 32 && *pCur != ',')
			pCur++;

		AWString strValue(pBegin, pCur-pBegin);
		pBuf[i] = strValue.ToFloat();
	}

	return true;
}

bool AWIniFile::GetValueAsIntRect(const wchar_t* szSect, const wchar_t* szKey, ARectI* pRect)
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
bool AWIniFile::WriteIntValue(const wchar_t* szSect, const wchar_t* szKey, int iValue)
{
	s_KEY* pKey = GetKey(szSect, szKey);
	if (!pKey)
		return false;

	pKey->strValue.Format(L"%d", iValue);

	return true;
}

/*	Write integer value

	Return true for success, otherwise return false.
	
	szSect: section name
	szKey: key name
	szValue: value will be written.
*/
bool AWIniFile::WriteStringValue(const wchar_t* szSect, const wchar_t* szKey, const wchar_t* szValue)
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
bool AWIniFile::WriteFloatValue(const wchar_t* szSect, const wchar_t* szKey, float fValue)
{
	s_KEY* pKey = GetKey(szSect, szKey);
	if (!pKey)
		return false;

	pKey->strValue.Format(L"%f", fValue);

	return true;
}

/*	Search specified value string

	Return string's address for success, otherwise return NULL.

	szSect: section name
	szKey: key name
*/
AWString* AWIniFile::SearchValue(const wchar_t* szSect, const wchar_t* szKey)
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
AWIniFile::s_SECTION* AWIniFile::SearchSection(const wchar_t* szSect)
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
AWIniFile::s_KEY* AWIniFile::SearchKey(s_SECTION* pSection, const wchar_t* szKey)
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
AWIniFile::s_KEY* AWIniFile::GetKey(const wchar_t* szSect, const wchar_t* szKey)
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
bool AWIniFile::Save(const char* szFile)
{
	FILE* fp = fopen(szFile, "wb+");
	if (!fp)
	{
		AFERRLOG(("AWIniFile::Save, Failed to create flie %s", szFile));
		return false;
	}

	//	Write unicode file header
	wchar_t wcFlag = 0xfeff;
	fwprintf(fp, L"%c", wcFlag);

	int i, j;
	AWString strValue;

	for (i=0; i < m_aSects.GetSize(); i++)
	{
		s_SECTION* pSect = m_aSects[i];
		if (pSect->bComment)
		{
			fwprintf(fp, L"%s\n", pSect->strName);
			continue;
		}

		//	Write section name
		strValue = L"[" + pSect->strName + L"]";
		fwprintf(fp, L"%s\n", strValue);

		for (j=0; j < pSect->aKeys.GetSize(); j++)
		{
			s_KEY* pKey = pSect->aKeys[j];
			
			//	Write key name and value
			strValue = pKey->strKey + L" = " + pKey->strValue;
			fwprintf(fp, L"%s\n", strValue);
		}

		fwprintf(fp, L"\n");
	}

	fclose(fp);

	return true;
}


