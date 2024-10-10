/*
 * FILE: EL_Archive.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/6/28
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include <wchar.h>

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
//	Class CELArchive
//	
///////////////////////////////////////////////////////////////////////////

class CELArchive
{
public:		//	Types

public:		//	Constructor and Destructor

	virtual ~CELArchive() {}

public:		//	Attributes

public:		//	Operations

	virtual const char* ReadString(int* iLen=NULL) = 0;
	virtual int WriteString(const char* str) = 0;
	virtual const wchar_t* ReadWString(int* iLen=NULL) = 0;
	virtual int WriteWString(const wchar_t* str) = 0;

	virtual bool Write(const void* pBuf, int iSize) = 0;
	virtual bool Read(void* pBuf, int iSize) = 0;

	virtual bool IsStoring() = 0;
	virtual bool IsLoading() = 0;

	virtual unsigned int GetPos() = 0;
	virtual bool Seek(int iOffset, int iOrigin) = 0;

protected:	//	Attributes

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//	
//	Class CELFileArchive
//	
///////////////////////////////////////////////////////////////////////////

class CELFileArchive : public CELArchive
{
public:		//	Types

public:		//	Constructor and Destructor

	CELFileArchive(AFile* pFile, bool bSave);
	virtual ~CELFileArchive();

public:		//	Attributes

public:		//	Operations

	virtual const char* ReadString(int* piLen=NULL);
	virtual int WriteString(const char* str);
	virtual const wchar_t* ReadWString(int* piLen=NULL);
	virtual int WriteWString(const wchar_t* str);

	virtual bool Write(const void* pBuf, int iSize);
	virtual bool Read(void* pBuf, int iSize);

	virtual bool IsStoring() { return m_bSaving; }
	virtual bool IsLoading() { return !m_bSaving; }

	virtual unsigned int GetPos();
	virtual bool Seek(int iOffset, int iOrigin);

protected:	//	Attributes

	AFile*		m_pFile;		//	File object
	bool		m_bSaving;		//	true, save mode; false, load mode
	char*		m_pszTemp;		//	Used to store temporary strings
	wchar_t*	m_pwszTemp;

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////



