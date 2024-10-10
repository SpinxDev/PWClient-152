/*
 * FILE: AIniFile.h
 *
 * DESCRIPTION: Class for ini file operation
 *
 * CREATED BY: duyuxin, 2003/10/25
 *
 * HISTORY:
 *
 * Copyright (c) 2003 Archosaur Studio, All Rights Reserved.
 */

#ifndef _AINIFILE_H_
#define _AINIFILE_H_

#include "AString.h"
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
//	Class AIniFile
//
///////////////////////////////////////////////////////////////////////////

class AIniFile
{
public:		//	Types

	struct s_KEY
	{
		AString		strKey;		//	Key name
		AString		strValue;	//	Value string
	};

	struct s_SECTION
	{
		bool				bComment;	//	true, Comment section
		AString				strName;	//	Section name
		APtrArray<s_KEY*>	aKeys;		//	Keys
	};

public:		//	Constructions and Destructions

	AIniFile();
	virtual ~AIniFile();

public:		//	Attributes

public:		//	Operaitons

	//	Open ini file
	bool Open(const char* szFile);
	bool Open(AFile* pFile);
	//	Close file
	void Close();

	//	Save modified data
	bool Save(const char* szFile);
	bool Save(AFile* pFile);

	//	Get values
	int GetValueAsInt(const char* szSect, const char* szKey, int iDefault);
	AString GetValueAsString(const char* szSect, const char* szKey, const char* szDefault=NULL);
	float GetValueAsFloat(const char* szSect, const char* szKey, float fDefault);

	//	Write values
	bool WriteIntValue(const char* szSect, const char* szKey, int iValue);
	bool WriteStringValue(const char* szSect, const char* szKey, const char* szValue);
	bool WriteFloatValue(const char* szSect, const char* szKey, float fValue);

	//	Some facility functions
	bool GetValueAsIntArray(const char* szSect, const char* szKey, int iNumInt, int* pBuf);
	bool GetValueAsFloatArray(const char* szSect, const char* szKey, int iNumFloat, float* pBuf);
	bool GetValueAsIntRect(const char* szSect, const char* szKey, ARectI* pRect);
	bool GetValueAsBoolean(const char* szSect, const char* szKey, bool bDefault) { return GetValueAsInt(szSect, szKey, bDefault ? 1 : 0) ? true : false; }

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
	s_SECTION* AddSection(bool bComment, const AString& strName);
	//	Create a key
	s_KEY* CreateKey(const AString& strKey);

	//	Search specified value
	AString* SearchValue(const char* szSect, const char* szKey);
	//	Search specified section
	s_SECTION* SearchSection(const char* szSect);
	//	Search key in specified section
	s_KEY* SearchKey(s_SECTION* pSection, const char* szKey);
	//	Get specified key, create if not exist
	s_KEY* GetKey(const char* szSect, const char* szKey);
};


#endif	//	_AINIFILE_H_
