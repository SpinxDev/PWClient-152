/*
 * FILE: AString.cpp
 *
 * DESCRIPTION: String operating class
 *
 * CREATED BY: duyuxin, 2003/6/4
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#include "ABaseDef.h"
#include "AString.h"
#include "AMemory.h"
#include <locale.h>
#include <mbstring.h>

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

struct s_EMPTYSTRING
{
	AString::s_STRINGDATA	Data;

	char	szStr[1];

	s_EMPTYSTRING()
	{
		Data.iRefs		= 0;
		Data.iDataLen	= 0;
		Data.iMaxLen	= 0;
		szStr[0]		= '\0';
	}
};

//	For an empty string, m_pchData will point here
static s_EMPTYSTRING l_EmptyString;
char* AString::m_pEmptyStr = l_EmptyString.szStr;

///////////////////////////////////////////////////////////////////////////
//
//	Local functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Implement AString
//
///////////////////////////////////////////////////////////////////////////

/*	Alocate string buffer

	iLen: length of data (not including terminator)
*/
char* AString::AllocBuffer(int iLen)
{
	s_STRINGDATA* pData;
	char* pBuf;

	if (iLen < 64)
	{
		pBuf  = new char [64 + sizeof (s_STRINGDATA)];
		pData = (s_STRINGDATA*)pBuf;

		pData->iRefs	= 1;
		pData->iDataLen	= iLen;
		pData->iMaxLen	= 63;
	}
	else if (iLen < 128)
	{
		pBuf  = new char [128 + sizeof (s_STRINGDATA)];
		pData = (s_STRINGDATA*)pBuf;

		pData->iRefs	= 1;
		pData->iDataLen	= iLen;
		pData->iMaxLen	= 127;
	}
	else if (iLen < 256)
	{
		pBuf  = new char [256 + sizeof (s_STRINGDATA)];
		pData = (s_STRINGDATA*)pBuf;

		pData->iRefs	= 1;
		pData->iDataLen	= iLen;
		pData->iMaxLen	= 255;
	}
	else if (iLen < 512)
	{
		pBuf  = new char [512 + sizeof (s_STRINGDATA)];
		pData = (s_STRINGDATA*)pBuf;

		pData->iRefs	= 1;
		pData->iDataLen	= iLen;
		pData->iMaxLen	= 511;
	}
	else
	{
		pBuf  = new char [iLen + 1 + sizeof (s_STRINGDATA)];
		pData = (s_STRINGDATA*)pBuf;

		pData->iRefs	= 1;
		pData->iDataLen	= iLen;
		pData->iMaxLen	= iLen;
	}

	return pBuf + sizeof (s_STRINGDATA);
}

//	Free string data buffer
void AString::FreeBuffer(s_STRINGDATA* pStrData)
{
	switch (pStrData->iRefs)
	{
	case 0:	return;
	case 1:

		delete [] pStrData;
		break;

	default:

		pStrData->iRefs--;
		break;
	}
}

//	Copy iLen characters from szSrc to szDest and add terminator at the tail of szDest
void AString::StringCopy(char* szDest, const char* szSrc, int iLen)
{
	int i, iSpan = sizeof (long);
	const long *p1 = (const long*)szSrc;
	long *p2 = (long*)szDest;

	for (i=0; i < iLen / iSpan; i++, p1++, p2++)
		*p2 = *p1;

	for (i*=iSpan; i < iLen; i++)
		szDest[i] = szSrc[i];

	szDest[i] = '\0';
}

//	Judge whether two strings are equal
bool AString::StringEqual(const char* s1, const char* s2, int iLen)
{
	int i, iSpan = sizeof (long);
	const long *p1 = (const long *)s1;
	const long *p2 = (const long *)s2;

	for (i=0; i < iLen / iSpan; i++, p1++, p2++)
	{
		if (*p1 != *p2)
			return false;
	}

	for (i*=iSpan; i < iLen; i++)
	{
		if (s1[i] != s2[i])
			return false;
	}

	return true;
}

//	Allocate memory and copy string
char* AString::AllocThenCopy(const char* szSrc, int iLen)
{
	if (!iLen)
		return m_pEmptyStr;
	
	char* s = AllocBuffer(iLen);
	StringCopy(s, szSrc, iLen);

	return s;
}

//	Allocate a new string which is merged by szSrc + ch
char* AString::AllocThenCopy(const char* szSrc, char ch, int iLen)
{
	if (!ch)
		return AllocThenCopy(szSrc, iLen-1);

	char* s = AllocBuffer(iLen);
	StringCopy(s, szSrc, iLen-1);

	s[iLen-1]	= ch;
	s[iLen]		= '\0';

	return s;
}

//	Allocate a new string which is merged by ch + szSrc
char* AString::AllocThenCopy(char ch, const char* szSrc, int iLen)
{
	if (!ch)
		return l_EmptyString.szStr;

	char* s = AllocBuffer(iLen);
	
	s[0] = ch;
	StringCopy(s+1, szSrc, iLen-1);

	return s;
}

//	Allocate a new string which is merged by s1 + s2
char* AString::AllocThenCopy(const char* s1, const char* s2, int iLen1, int iLen2)
{
	if (!iLen2)
		return AllocThenCopy(s1, iLen1);
	
	int iLen = iLen1 + iLen2;
	char* s = AllocBuffer(iLen);

	StringCopy(s, s1, iLen1);
	StringCopy(s+iLen1, s2, iLen2);

	return s;
}

AString::AString(const AString& str)
{
	if (str.IsEmpty())
	{
		m_pStr = m_pEmptyStr;
		return;
	}

	s_STRINGDATA* pSrcData = str.GetData();

	if (pSrcData->iRefs == -1)	//	Source string is being locked
	{
		s_STRINGDATA* pData = GetData();
		m_pStr = AllocThenCopy(str.m_pStr, pSrcData->iDataLen);
	}
	else
	{
		pSrcData->iRefs++;
		m_pStr = str.m_pStr;
	}
}

AString::AString(const char* szStr)
{
	int iLen = SafeStrLen(szStr);
	m_pStr = AllocThenCopy(szStr, iLen);
}

AString::AString(const char* szStr, int iLen)
{
	m_pStr = AllocThenCopy(szStr, iLen);
}

AString::AString(char ch, int iRepeat)
{
	m_pStr = AllocBuffer(iRepeat);
	memset(m_pStr, ch, iRepeat);
	m_pStr[iRepeat] = '\0';
}

AString::AString(const AString& str1, const AString& str2)
{
	m_pStr = AllocThenCopy(str1.m_pStr, str2.m_pStr, str1.GetLength(), str2.GetLength());
}

AString::AString(char ch, const AString& str)
{
	m_pStr = AllocThenCopy(ch, str.m_pStr, str.GetLength() + 1);
}

AString::AString(const AString& str, char ch)
{
	m_pStr = AllocThenCopy(str.m_pStr, ch, str.GetLength() + 1);
}

AString::AString(const char* szStr, const AString& str)
{
	m_pStr = AllocThenCopy(szStr, str.m_pStr, SafeStrLen(szStr), str.GetLength());
}

AString::AString(const AString& str, const char* szStr)
{
	m_pStr = AllocThenCopy(str.m_pStr, szStr, str.GetLength(), SafeStrLen(szStr));
}

AString::~AString()
{
	s_STRINGDATA* pData = GetData();

	if (pData->iRefs == -1)	//	Buffer is being locked
		pData->iRefs = 1;

	FreeBuffer(pData);
	m_pStr = m_pEmptyStr;
}

const AString& AString::operator = (char ch)
{
	if (!ch)
	{
		Empty();
		return *this;
	}

	s_STRINGDATA* pData = GetData();

	if (IsEmpty())
		m_pStr = AllocBuffer(1);
	else if (pData->iRefs > 1)
	{
		pData->iRefs--;
		m_pStr = AllocBuffer(1);
	}
	else
		pData->iRefs = 1;
	
	m_pStr[0] = ch;
	m_pStr[1] = '\0';
	
	GetData()->iDataLen = 1;

	return *this;
}

const AString& AString::operator = (const char* szStr)
{
	int iLen = SafeStrLen(szStr);
	if (!iLen)
	{
		Empty();
		return *this;
	}

	s_STRINGDATA* pData = GetData();

	if (pData->iRefs > 1)
	{
		pData->iRefs--;
		m_pStr = AllocThenCopy(szStr, iLen);
	}
	else
	{
		if (iLen <= pData->iMaxLen)
		{
			StringCopy(m_pStr, szStr, iLen);
			pData->iDataLen = iLen;
		}
		else
		{
			FreeBuffer(pData);
			m_pStr = AllocThenCopy(szStr, iLen);
		}
	}

	return *this;
}

const AString& AString::operator = (const AString& str)
{
	if (m_pStr == str.m_pStr)
		return *this;

	if (str.IsEmpty())
	{ 
		Empty();
		return *this; 
	}

	s_STRINGDATA* pSrcData = str.GetData();

	if (pSrcData->iRefs == -1)	//	Source string is being locked
	{
		s_STRINGDATA* pData = GetData();

		if (pData->iRefs > 1)
		{
			pData->iRefs--;
			m_pStr = AllocThenCopy(str.m_pStr, pSrcData->iDataLen);
		}
		else
		{
			if (pSrcData->iDataLen <= pData->iMaxLen)
			{
				StringCopy(m_pStr, str.m_pStr, pSrcData->iDataLen);
				pData->iDataLen = pSrcData->iDataLen;
			}
			else
			{
				FreeBuffer(pData);
				m_pStr = AllocThenCopy(str.m_pStr, pSrcData->iDataLen);
			}
		}
	}
	else
	{
		FreeBuffer(GetData());
		pSrcData->iRefs++;
		m_pStr = str.m_pStr;
	}

	return *this;
}

const AString& AString::operator += (char ch)
{
	if (!ch)
		return *this;

	s_STRINGDATA* pData = GetData();

	if (pData->iRefs > 1)
	{
		pData->iRefs--;
		m_pStr = AllocThenCopy(m_pStr, ch, pData->iDataLen+1);
		return *this;
	}

	int iLen = pData->iDataLen + 1;
	if (iLen <= pData->iMaxLen)
	{
		m_pStr[iLen-1]	= ch;
		m_pStr[iLen]	= '\0';
		pData->iDataLen++;
	}
	else
	{
		m_pStr = AllocThenCopy(m_pStr, ch, iLen);
		FreeBuffer(pData);
	}

	return *this;
}

const AString& AString::operator += (const char* szStr)
{
	int iLen2 = SafeStrLen(szStr);
	if (!iLen2)
		return *this;

	s_STRINGDATA* pData = GetData();

	if (pData->iRefs > 1)
	{
		pData->iRefs--;
		m_pStr = AllocThenCopy(m_pStr, szStr, pData->iDataLen, iLen2);
		return *this;
	}

	int iLen = pData->iDataLen + iLen2;
	if (iLen <= pData->iDataLen)
	{
		StringCopy(m_pStr+pData->iDataLen, szStr, iLen2);
		pData->iDataLen = iLen;
	}
	else
	{
		m_pStr = AllocThenCopy(m_pStr, szStr, pData->iDataLen, iLen2);
		FreeBuffer(pData);
	}

	return *this;
}

const AString& AString::operator += (const AString& str)
{
	int iLen2 = str.GetLength();
	if (!iLen2)
		return *this;

	s_STRINGDATA* pData = GetData();

	if (pData->iRefs > 1)
	{
		pData->iRefs--;
		m_pStr = AllocThenCopy(m_pStr, str.m_pStr, pData->iDataLen, iLen2);
		return *this;
	}

	int iLen = pData->iDataLen + iLen2;
	if (iLen <= pData->iMaxLen)
	{
		StringCopy(m_pStr+pData->iDataLen, str.m_pStr, iLen2);
		pData->iDataLen = iLen;
	}
	else
	{
		m_pStr = AllocThenCopy(m_pStr, str.m_pStr, pData->iDataLen, iLen2);
		FreeBuffer(pData);
	}

	return *this;
}

int AString::Compare(const char* szStr) const
{
	if (m_pStr == szStr)
		return 0;

	return strcmp(m_pStr, szStr);
}

int AString::CompareNoCase(const char* szStr) const
{
	if (m_pStr == szStr)
		return 0;

	return _stricmp(m_pStr, szStr);
}

bool AString::operator == (const char* szStr) const
{
	//	Note: szStr's boundary may be crossed when StringEqual() do
	//		  read operation, if szStr is shorter than 'this'. Now, this
	//		  read operation won't cause problem, but in the future,
	//		  should we check the length of szStr at first, and put the 
	//		  shorter one between 'this' and szStr front when we call StringEqual ?
	int iLen = GetLength();
	return StringEqual(m_pStr, szStr, iLen+1);
}

bool AString::operator == (const AString& str) const
{
	if (m_pStr == str.m_pStr)
		return true;

	int iLen = GetLength();
	if (iLen != str.GetLength())
		return false;

	return StringEqual(m_pStr, str.m_pStr, iLen);
}

char& AString::operator [] (int n)
{
	ASSERT(n >= 0 && n <= GetLength());

	s_STRINGDATA* pData = GetData();
	if (pData->iRefs > 1)
	{
		pData->iRefs--;
		m_pStr = AllocThenCopy(m_pStr, GetLength());
	}

	return m_pStr[n];
}

//	Convert to upper case
void AString::MakeUpper()
{
	int iLen = GetLength();
	if (!iLen)
		return;

	s_STRINGDATA* pData = GetData();
	if (pData->iRefs > 1)
	{
		pData->iRefs--;
		m_pStr = AllocThenCopy(m_pStr, iLen);
	}

	strupr(m_pStr);
}

//	Convert to lower case
void AString::MakeLower()
{
	int iLen = GetLength();
	if (!iLen)
		return;

	s_STRINGDATA* pData = GetData();
	if (pData->iRefs > 1)
	{
		pData->iRefs--;
		m_pStr = AllocThenCopy(m_pStr, iLen);
	}

	strlwr(m_pStr);
}

//	Format string
AString& AString::Format(const char* szFormat, ...)
{
	va_list argList;
	va_start(argList, szFormat);

	int iNumWritten, iMaxLen = GetFormatLen(szFormat, argList) + 1;

	s_STRINGDATA* pData = GetData();

	if (pData->iRefs > 1)
		pData->iRefs--;
	else if (iMaxLen <= pData->iMaxLen)
	{
		vsprintf(m_pStr, szFormat, argList);
		pData->iDataLen = SafeStrLen(m_pStr);
		goto End;
	}
	else	//	iMaxLen > pData->iMaxLen
		FreeBuffer(pData);

	m_pStr = AllocBuffer(iMaxLen);
	iNumWritten = vsprintf(m_pStr, szFormat, argList);
	ASSERT(iNumWritten < iMaxLen);
	GetData()->iDataLen = SafeStrLen(m_pStr);

End:

	va_end(argList);
	return *this;
}

/*	Get buffer. If you have changed content buffer returned by GetBuffer(), you
	must call ReleaseBuffer() later. Otherwise, ReleaseBuffer() isn't necessary.

	Return buffer's address for success, otherwise return NULL.

	iMinSize: number of bytes in string buffer user can changed.
*/
char* AString::GetBuffer(int iMinSize)
{
	if (iMinSize < 0)
	{
		ASSERT(iMinSize >= 0);
		return NULL;
	}

	//	Ensure we won't allocate an empty string when iMinSize == 1
	if (!iMinSize)
		iMinSize = 1;

	s_STRINGDATA* pData = GetData();

	if (IsEmpty())
	{
		m_pStr = AllocBuffer(iMinSize);
		m_pStr[0] = '\0';
		GetData()->iDataLen = 0;
	}
	else if (pData->iRefs > 1)
	{
		pData->iRefs--;

		if (iMinSize <= pData->iDataLen)
		{
			m_pStr = AllocThenCopy(m_pStr, pData->iDataLen);
		}
		else
		{
			char* szOld = m_pStr;
			m_pStr = AllocBuffer(iMinSize);
			StringCopy(m_pStr, szOld, pData->iDataLen);
			GetData()->iDataLen = pData->iDataLen;
		}
	}
	else if (iMinSize > pData->iMaxLen)
	{
		char* szOld = m_pStr;
		m_pStr = AllocBuffer(iMinSize);
		StringCopy(m_pStr, szOld, pData->iDataLen);
		GetData()->iDataLen = pData->iDataLen;
		FreeBuffer(pData);
	}

	return m_pStr;
}

/*	If you have changed content of buffer returned by GetBuffer(), you must call
	ReleaseBuffer() later. Otherwise, ReleaseBuffer() isn't necessary.

	iNewSize: new size in bytes of string. -1 means string is zero ended and it's
			  length can be got by strlen().
*/
void AString::ReleaseBuffer(int iNewSize/* -1 */)
{
	s_STRINGDATA* pData = GetData();
	if (pData->iRefs != 1)
	{
		ASSERT(pData->iRefs == 1);	//	Ensure GetBuffer has been called.
		return;
	}
	
	if (iNewSize == -1)
		iNewSize = SafeStrLen(m_pStr);

	if (iNewSize > pData->iMaxLen)
	{
		ASSERT(iNewSize <= pData->iMaxLen);
		return;
	}
	
	if( iNewSize == 0)
	{
		Empty();
	}
	else
	{
		pData->iDataLen = iNewSize;
		m_pStr[iNewSize] = '\0';
	}
}

//	Lock buffer. Locked buffer disable reference counting
char* AString::LockBuffer()
{
	if (IsEmpty())
	{
		ASSERT(!IsEmpty());
		return NULL;
	}

	s_STRINGDATA* pData = GetData();
	if (pData->iRefs <= 0)
	{
		ASSERT(pData->iRefs > 0);	//	Buffer has been locked ?
		return NULL;
	}

	if (pData->iRefs > 1)
	{
		pData->iRefs--;
		m_pStr = AllocThenCopy(m_pStr, pData->iDataLen);
		pData  = GetData();
	}

	pData->iRefs = -1;

	return m_pStr;
}

//	Unlock buffer
void AString::UnlockBuffer()
{
	s_STRINGDATA* pData = GetData();
	if (pData->iRefs >= 0)
	{
		ASSERT(pData->iRefs < 0);	//	Buffer must has been locked.
		return;
	}

	pData->iDataLen	= SafeStrLen(m_pStr);
	pData->iRefs	= 1;
}

/*	Make a guess at the maximum length of the resulting string.
	now this function doesn't support UNICODE string.
	I64 modifier used in WIN32's sprintf is now added in 2010.1.10

	Return estimated length of resulting string.
*/

#define FORCE_ANSI      0x10000
#define FORCE_UNICODE   0x20000
#define FORCE_INT64     0x40000

int AString::GetFormatLen(const char* szFormat, va_list argList)
{
	if (!szFormat || !szFormat[0])
		return 0;

	char* pszTemp = NULL;
	int iMaxLen = 0, iTempBufLen = 0;

	for (const char* pch = szFormat; *pch != '\0'; pch++)
	{
		//	Handle '%' character, but watch out for '%%'
		if (*pch != '%' || *(++pch) == '%')
		{
			iMaxLen++;
			continue;
		}

		int iItemLen = 0, iWidth = 0;

		//	Handle '%' character with format
		for (; *pch != '\0'; pch++)
		{
			//	Check for valid flags
			if (*pch == '#')
				iMaxLen += 2;   // for '0x'
			else if (*pch == '*')
				iWidth = va_arg(argList, int);
			else if (*pch == '-' || *pch == '+' || *pch == '0' || *pch == ' ')
				;
			else	//	Hit non-flag character
				break;
		}

		//	Get width and skip it
		if (iWidth == 0)
		{
			//	Width indicated by digit
			iWidth = atoi(pch);
			for (; *pch != '\0' && (*pch >= '0' && *pch <= '9'); pch++)
				;
		}

		ASSERT(iWidth >= 0);

		int iPrecision = 0;

		if (*pch == '.')
		{
			//	Skip past '.' separator (width.precision)
			pch++;

			//	Get precision and skip it
			if (*pch == '*')
			{
				iPrecision = va_arg(argList, int);
				pch++;
			}
			else
			{
				iPrecision = atoi(pch);
				for (; *pch != '\0' && (*pch >= '0' && *pch <= '9'); pch++)
					;
			}

			ASSERT(iPrecision >= 0);
		}

		// should be on type modifier or specifier
		int nModifier = 0;
		if (strncmp(pch, "I64", 3) == 0)
		{
			pch += 3;
			nModifier = FORCE_INT64;
			// __int64 is only available on X86 and ALPHA platforms
		}
		else
		{
			switch (*pch)
			{
			case 'h':
			case 'l':
			case 'F':
			case 'N':
			case 'L':
				pch++;
				break;
			}
		}

		switch (*pch)
		{
		case 'c':	// Single characters
		case 'C':

			iItemLen = 2;
			va_arg(argList, char);
			break;

		case 's':	// Strings
		{
			const char* pstrNextArg = va_arg(argList, const char*);
			if (!pstrNextArg)
				iItemLen = 6;	//	"(null)"
			else
				iItemLen = pstrNextArg[0] == '\0' ? 1 : strlen(pstrNextArg);

			break;
		}
		case 'S':
		{
			const wchar_t* pstrNextArg = va_arg(argList, const wchar_t*);
			if (!pstrNextArg)
				iItemLen = 6;	//	"(null)"
			else
				iItemLen = pstrNextArg[0] == '\0' ? 1 : wcslen(pstrNextArg);

			break;
		}
		}
		
		//	Adjust iItemLen for strings
		if (iItemLen != 0)
		{
			if (iPrecision != 0 && iPrecision < iItemLen)
				iItemLen = iPrecision;

			if (iWidth > iItemLen)
				iItemLen = iWidth;
		}
		else
		{
			switch (*pch)
			{
			case 'd':	//	Integers
			case 'i':
			case 'u':
			case 'x':
			case 'X':
			case 'o':

				if (nModifier & FORCE_INT64)
					va_arg(argList, __int64);
				else
					va_arg(argList, int);

				iItemLen = iWidth+iPrecision > 32 ? iWidth+iPrecision : 32;
				break;

			case 'e':
			case 'g':
			case 'G':

				va_arg(argList, s_DOUBLE);	//	For _X86_
				iItemLen = iWidth+iPrecision > 128 ? iWidth+iPrecision : 128;
				break;

			case 'f':
			{
				//	312 == strlen("-1+(309 zeroes).")
				//	309 zeroes == max precision of a double
				//	6 == adjustment in case precision is not specified,
				//		 which means that the precision defaults to 6
				int iSize = 312 + iPrecision + 6;
				if (iWidth > iSize)
					iSize = iWidth;

				if (iTempBufLen < iSize)
				{
					if (!pszTemp)
						pszTemp = (char*)a_malloc(iSize);
					else
						pszTemp = (char*)a_realloc(pszTemp, iSize);

					iTempBufLen = iSize;
				}

				double f = va_arg(argList, double);
				sprintf(pszTemp, "%*.*f", iWidth, iPrecision+6, f);
				iItemLen = strlen(pszTemp);

				break;
			}
			case 'p':

				va_arg(argList, void*);
				iItemLen = iWidth+iPrecision > 32 ? iWidth+iPrecision : 32;
				break;

			//	No output
			case 'n':
				
				va_arg(argList, int*);
				break;

			default:

				ASSERT(0);  //	Unknown formatting option
				break;
			}
		}

		//	Adjust iMaxLen for output iItemLen
		iMaxLen += iItemLen;
	}

	if (pszTemp)
		a_free(pszTemp);

	return iMaxLen;
}

//	Cut left sub string
void AString::CutLeft(int n)
{
	if (!GetLength() || n <= 0)
		return;

	s_STRINGDATA* pData = GetData();

	if (n >= pData->iDataLen)
	{
		Empty();
		return;
	}

	int iNewLen = pData->iDataLen - n;

	if (pData->iRefs > 1)
	{
		pData->iRefs--;
		m_pStr = AllocThenCopy(m_pStr+n, iNewLen);
		return;
	}

	for (int i=0; i < iNewLen; i++)
		m_pStr[i] = m_pStr[n+i];

	m_pStr[iNewLen] = '\0';
	pData->iDataLen = iNewLen;
}

//	Cut right sub string
void AString::CutRight(int n)
{
	if (!GetLength() || n <= 0)
		return;

	s_STRINGDATA* pData = GetData();

	if (n >= pData->iDataLen)
	{
		Empty();
		return;
	}

	int iNewLen = pData->iDataLen - n;

	if (pData->iRefs > 1)
	{
		pData->iRefs--;
		m_pStr = AllocThenCopy(m_pStr, iNewLen);
		return;
	}

	m_pStr[iNewLen] = '\0';
	pData->iDataLen = iNewLen;
}

//	Trim left
void AString::TrimLeft()
{
	if (!GetLength())
		return;

	int i;
	BYTE* aStr = (BYTE*)m_pStr;

	for (i=0; aStr[i]; i++)
	{
		if (aStr[i] > 32)
			break;
	}

	CutLeft(i);
}

//	Trim left
void AString::TrimLeft(char ch)
{
	if (!GetLength())
		return;

	int i;

	for (i=0; m_pStr[i]; i++)
	{
		if (m_pStr[i] != ch)
			break;
	}

	CutLeft(i);
}

//	Trim left
void AString::TrimLeft(const char* szChars)
{
	if (!GetLength())
		return;

	int i, j;

	for (i=0; m_pStr[i]; i++)
	{
		for (j=0; szChars[j]; j++)
		{
			if (m_pStr[i] == szChars[j])
				break;
		}

		if (!szChars[j])
			break;
	}

	CutLeft(i);
}

//	Trim right
void AString::TrimRight()
{
	if (!GetLength())
		return;

	int i, iLen = GetLength();
	BYTE* aStr = (BYTE*)m_pStr;

	for (i=iLen-1; i >= 0; i--)
	{
		if (aStr[i] > 32)
			break;
	}

	CutRight(iLen-1-i);
}

//	Trim right
void AString::TrimRight(char ch)
{
	if (!GetLength())
		return;

	int i, iLen = GetLength();

	for (i=iLen-1; i >= 0; i--)
	{
		if (m_pStr[i] != ch)
			break;
	}

	CutRight(iLen-1-i);
}

//	Trim right
void AString::TrimRight(const char* szChars)
{
	if (!GetLength())
		return;

	int i, j, iLen = GetLength();

	for (i=iLen-1; i >= 0; i--)
	{
		for (j=0; szChars[j]; j++)
		{
			if (m_pStr[i] == szChars[j])
				break;
		}

		if (!szChars[j])
			break;
	}

	CutRight(iLen-1-i);
}

//	Finds a character inside a larger string. 
//	Return -1 for failure.
int AString::Find(char ch, int iStart/* 0 */) const
{
	int iLen = GetLength();
	if (!iLen || iStart < 0 || iStart >= iLen)
		return -1;

	for (int i=iStart; i < iLen; i++)
	{
		if (m_pStr[i] == ch)
			return i;
	}

	return -1;
}

//	Finds a substring inside a larger string. 
//	Return -1 for failure.
int AString::Find(const char* szSub, int iStart/* 0 */) const
{
	int iLen = GetLength();
	if (!iLen || iStart < 0 || iStart >= iLen)
		return -1;

	char* pTemp = strstr(m_pStr+iStart, szSub);
	if (!pTemp)
		return -1;

	return pTemp - m_pStr; 
}

//	Finds a character inside a larger string; starts from the end. 
//	Return -1 for failure.
int AString::ReverseFind(char ch) const
{
	if (!GetLength())
		return -1;

	char* pTemp = strrchr(m_pStr, ch);
	if (!pTemp)
		return -1;

	return pTemp - m_pStr; 
}

//	Finds the first matching character from a set. 
//	Return -1 for failure.
int AString::FindOneOf(const char* szCharSet) const
{
	int iLen = GetLength();
	if (!iLen)
		return -1;

	return ((int)strcspn(m_pStr, szCharSet) == iLen) ? -1 : 0;
}

