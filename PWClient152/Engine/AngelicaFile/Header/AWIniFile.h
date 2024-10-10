/*
 * FILE: AWIniFile.h
 *
 * DESCRIPTION: Class for ini file operation
 *
 * CREATED BY: duyuxin, 2003/10/25
 *
 * HISTORY:
 *
 * Copyright (c) 2003 Archosaur Studio, All Rights Reserved.
 */

#ifndef _AWINIFILE_H_
#define _AWINIFILE_H_

#include "AWString.h"
#include "AArray.h"
#include "ARect.h"

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
//	Class AWIniFile
//
///////////////////////////////////////////////////////////////////////////

class AWIniFile
{
public:		//	Types

	struct s_KEY
	{
		AWString	strKey;		//	Key name
		AWString	strValue;	//	Value string
	};

	struct s_SECTION
	{
		bool				bComment;	//	true, Comment section
		AWString			strName;	//	Section name
		APtrArray<s_KEY*>	aKeys;		//	Keys
	};

public:		//	Constructions and Destructions

	AWIniFile();
	virtual ~AWIniFile();

public:		//	Attributes

public:		//	Operaitons

	//	Open ini file
	bool Open(const char* szFile);
	bool Open(AFile* pFile);
	//	Close file
	void Close();

	//	Save modified data
	bool Save(const char* szFile);

	//	Get values
	int GetValueAsInt(const wchar_t* szSect, const wchar_t* szKey, int iDefault);
	AWString GetValueAsString(const wchar_t* szSect, const wchar_t* szKey, const wchar_t* szDefault=NULL);
	float GetValueAsFloat(const wchar_t* szSect, const wchar_t* szKey, float fDefault);

	//	Write values
	bool WriteIntValue(const wchar_t* szSect, const wchar_t* szKey, int iValue);
	bool WriteStringValue(const wchar_t* szSect, const wchar_t* szKey, const wchar_t* szValue);
	bool WriteFloatValue(const wchar_t* szSect, const wchar_t* szKey, float fValue);

	//	Some facility functions
	bool GetValueAsIntArray(const wchar_t* szSect, const wchar_t* szKey, int iNumInt, int* pBuf);
	bool GetValueAsFloatArray(const wchar_t* szSect, const wchar_t* szKey, int iNumFloat, float* pBuf);
	bool GetValueAsIntRect(const wchar_t* szSect, const wchar_t* szKey, ARectI* pRect);
	bool GetValueAsBoolean(const wchar_t* szSect, const wchar_t* szKey, bool bDefault) { return GetValueAsInt(szSect, szKey, bDefault ? 1 : 0) ? true : false; }

protected:	//	Attributes

	bool		m_bOpened;		//	Open flag
	s_SECTION*	m_pCurSect;		//	Current section

	APtrArray<s_SECTION*>	m_aSects;	//	Sections

protected:	//	Operations

	//	Parse file
	bool ParseFile(BYTE* pBuf, BYTE* pEnd);
	//	Get line length
	int GetLineLength(BYTE* pLine, BYTE* pEnd);
	//	Parse a line
	void ParseLine(BYTE* pLine, BYTE* pEnd);
	//	Parse key value
	bool ParseValue(s_KEY* pKey, BYTE* pBuf, BYTE* pEnd);
	//	Add a section
	s_SECTION* AddSection(bool bComment, const AWString& strName);
	//	Create a key
	s_KEY* CreateKey(const AWString& strKey);

	//	Search specified value
	AWString* SearchValue(const wchar_t* szSect, const wchar_t* szKey);
	//	Search specified section
	s_SECTION* SearchSection(const wchar_t* szSect);
	//	Search key in specified section
	s_KEY* SearchKey(s_SECTION* pSection, const wchar_t* szKey);
	//	Get specified key, create if not exist
	s_KEY* GetKey(const wchar_t* szSect, const wchar_t* szKey);
};


#endif	//	_AWINIFILE_H_
