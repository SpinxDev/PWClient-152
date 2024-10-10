/*
 * FILE: AWString.h
 *
 * DESCRIPTION: wchar_t String operating class
 *
 * CREATED BY: duyuxin, 2004/2/10
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#ifndef _AWString_H_
#define _AWString_H_

#include <assert.h>
#include <wchar.h>
#include <stdio.h>

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
//	class AWString
//
///////////////////////////////////////////////////////////////////////////

class AWString
{
public:		//	Types

	struct s_STRINGDATA
	{
		int		iRefs;		//	Reference count
		int		iDataLen;	//	Length of data (not including terminator)
		int		iMaxLen;	//	Maximum length of data (not including terminator)
		
		wchar_t*	Data()	{ return (wchar_t*)(this+1); }	//	wchar_t* to managed data
	};

	struct s_DOUBLE
	{ 
		unsigned char	aDoubleBits[sizeof (double)];
	};

public:		//	Constructors and Destructors

	AWString() : m_pStr(m_pEmptyStr) {}
	AWString(const AWString& str);
	AWString(const wchar_t* szStr);
	AWString(const wchar_t* szStr, int iLen);
	AWString(wchar_t ch, int iRepeat);
	~AWString();

public:		//	Attributes

public:		//	Operations

	const AWString& operator = (wchar_t ch);
	const AWString& operator = (const wchar_t* szStr);
	const AWString& operator = (const AWString& str);

	const AWString& operator += (wchar_t ch);
	const AWString& operator += (const wchar_t* szStr);
	const AWString& operator += (const AWString& str);

	friend AWString operator + (const AWString& str1, const AWString& str2) { return AWString(str1, str2); }
	friend AWString operator + (wchar_t ch, const AWString& str) { return AWString(ch, str); }
	friend AWString operator + (const AWString& str, wchar_t ch) { return AWString(str, ch); }
	friend AWString operator + (const wchar_t* szStr, const AWString& str) { return AWString(szStr, str); }
	friend AWString operator + (const AWString& str, const wchar_t* szStr) { return AWString(str, szStr); }

	int Compare(const wchar_t* szStr) const;
	int CompareNoCase(const wchar_t* szStr) const;

	bool operator == (wchar_t ch) const { return (GetLength() == 1 && m_pStr[0] == ch); }
	bool operator == (const wchar_t* szStr) const;
	bool operator == (const AWString& str) const;

	bool operator != (wchar_t ch) const { return !(GetLength() == 1 && m_pStr[0] == ch); }
	bool operator != (const wchar_t* szStr) const { return !((*this) == szStr); }
	bool operator != (const AWString& str) const { return !((*this) == str); }

	bool operator > (const wchar_t* szStr) const { return Compare(szStr) > 0; }
	bool operator < (const wchar_t* szStr) const { return Compare(szStr) < 0; }
	bool operator >= (const wchar_t* szStr) const { return !((*this) < szStr); }
	bool operator <= (const wchar_t* szStr) const { return !((*this) > szStr); }

	wchar_t operator [] (int n) const { assert(n >= 0 && n <= GetLength()); return m_pStr[n]; }
	wchar_t& operator [] (int n);

	operator const wchar_t* () const { return m_pStr; }

	//	Get string length
	int	GetLength()	const { return GetData()->iDataLen; }
	//	String is empty ?
	bool IsEmpty() const { return m_pStr == m_pEmptyStr ? true : false; }
	//	Empty string
	void Empty() { FreeBuffer(GetData()); m_pStr = m_pEmptyStr; }
	//	Convert to upper case
	void MakeUpper();
	//	Convert to lower case
	void MakeLower();
	//	Format string
	AWString& Format(const wchar_t* szFormat, ...);

	//	Get buffer
	wchar_t* GetBuffer(int iMinSize);
	//	Release buffer gotten through GetBuffer()
	void ReleaseBuffer(int iNewSize=-1);
	//	Lock buffer
	wchar_t* LockBuffer();
	//	Unlock buffer
	void UnlockBuffer();

	//	Trim left
	void TrimLeft();
	void TrimLeft(wchar_t ch);
	void TrimLeft(const wchar_t* szChars);
	//	Trim right
	void TrimRight();
	void TrimRight(wchar_t ch);
	void TrimRight(const wchar_t* szChars);

	//	Cut left sub string
	void CutLeft(int n);
	//	Cut right sub string
	void CutRight(int n);

	//	Finds a character or substring inside a larger string. 
	int Find(wchar_t ch, int iStart=0) const;
	int Find(const wchar_t* szSub, int iStart=0) const;
	//	Finds a character inside a larger string; starts from the end. 
	int ReverseFind(wchar_t ch) const;
	//	Finds the first matching character from a set. 
	int FindOneOf(const wchar_t* szCharSet) const;

	//	Get left string
	inline AWString Left(int n) const;
	//	Get right string
	inline AWString Right(int n) const;
	//	Get Mid string
	inline AWString Mid(int iFrom, int iNum=-1) const;

	//	Convert string to integer
	inline int ToInt() const;
	//	Convert string to float
	inline float ToFloat() const;
	//	Convert string to __int64
	inline __int64 ToInt64() const;
	//	Convert string to double
	inline double ToDouble() const;
	
protected:	//	Attributes

	wchar_t*		m_pStr;			//	String buffer
	static wchar_t*	m_pEmptyStr;

protected:	//	Operations

	//	These constructors are used to optimize operations such as 'operator +'
	AWString(const AWString& str1, const AWString& str2);
	AWString(wchar_t ch, const AWString& str);
	AWString(const AWString& str, wchar_t ch);
	AWString(const wchar_t* szStr, const AWString& str);
	AWString(const AWString& str, const wchar_t* szStr);

	//	Get string data
	s_STRINGDATA* GetData() const { return ((s_STRINGDATA*)m_pStr)-1; }

	//	Safed strlen()
	static int SafeStrLen(const wchar_t* szStr) { return szStr ? static_cast<int>(wcslen(szStr)) : 0; }
	//	String copy
	static void StringCopy(wchar_t* szDest, const wchar_t* szSrc, int iLen);
	//	Alocate string buffer
	static wchar_t* AllocBuffer(int iLen);
	//	Free string data buffer
	static void FreeBuffer(s_STRINGDATA* pStrData);
	//	Judge whether two strings are equal
	static bool StringEqual(const wchar_t* s1, const wchar_t* s2, int iLen);

	//	Allocate memory and copy string
	static wchar_t* AllocThenCopy(const wchar_t* szSrc, int iLen);
	static wchar_t* AllocThenCopy(const wchar_t* szSrc, wchar_t ch, int iLen);
	static wchar_t* AllocThenCopy(wchar_t ch, const wchar_t* szSrc, int iLen);
	static wchar_t* AllocThenCopy(const wchar_t* s1, const wchar_t* s2, int iLen1, int iLen2);

	//	Get format string length
	static int GetFormatLen(const wchar_t* szFormat, va_list argList);
};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////

//	Get left string
AWString AWString::Left(int n) const
{
	assert(n >= 0);
	int iLen = GetLength();
	return AWString(m_pStr, iLen < n ? iLen : n);
}

//	Get right string
AWString AWString::Right(int n) const
{
	assert(n >= 0);
	int iFrom = GetLength() - n;
	return Mid(iFrom < 0 ? 0 : iFrom, n);
}

//	Get Mid string
AWString AWString::Mid(int iFrom, int iNum/* -1 */) const
{
	int iLen = GetLength() - iFrom;
	if (iLen <= 0 || !iNum)
		return AWString();	//	Return empty string

	if (iNum > 0 && iLen > iNum)
		iLen = iNum;

	return AWString(m_pStr+iFrom, iLen);
}

//	Convert string to integer
int AWString::ToInt() const
{
	return IsEmpty() ? 0 : _wtoi(m_pStr);
}

//	Convert string to float
float AWString::ToFloat() const
{
	if (IsEmpty())
		return 0.0f;
	
	float fValue;
	swscanf(m_pStr, L"%f", &fValue);
	return fValue;
}

//	Convert string to __int64
__int64 AWString::ToInt64() const
{
	return IsEmpty() ? 0 : _wtoi64(m_pStr);
}

//	Convert string to double
double AWString::ToDouble() const
{
	if (IsEmpty())
		return 0.0;
	
   wchar_t *endptr;
   return wcstod(m_pStr, &endptr);
}

#endif	//	_AWString_H_
