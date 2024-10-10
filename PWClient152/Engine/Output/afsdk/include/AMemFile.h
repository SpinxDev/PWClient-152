/*
 * FILE: AMemFile.h
 *
 * DESCRIPTION: Class for memory file
 *
 * CREATED BY: Duyuxin, 2003/10/19
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _AMEMFILE_H_
#define _AMEMFILE_H_

#include "AFile.h"

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
//	Class AMemFile
//
///////////////////////////////////////////////////////////////////////////

class AMemFile : public AFile
{
public:		//	Types

public:		//	Constructions and Destructions

	AMemFile();
	virtual ~AMemFile();

public:		//	Attributes

public:		//	Operaitons

	//	Attach memory block
	bool Attach(BYTE* pBuf, DWORD dwBufLen, DWORD dwGrowBy);
	//	Detach memory block
	BYTE* Detach();

	//	Read data from file
	virtual bool Read(LPVOID pBuffer, DWORD dwBufferLength, DWORD* pReadLength);
	//	Write data to file
	virtual bool Write(LPVOID pBuffer, DWORD dwBufferLength, DWORD* pWriteLength);

	virtual bool ReadLine(char* szLineBuffer, DWORD dwBufferLength, DWORD* pdwReadLength);
	virtual bool ReadString(char* szLineBuffer, DWORD dwBufferLength, DWORD* pdwReadLength);
	virtual bool WriteLine(const char* szLineBuffer);
	virtual bool WriteString(const AString& str);
	virtual bool ReadString(AString& str);

	//	Get file length
	virtual DWORD GetLength() { return m_dwFileLen; }
	//	Get current position from begining
	virtual DWORD GetPos()  { return m_dwOffset; }
	//	Seek file pointer
	virtual bool Seek(int iOffset, AFILE_SEEK origin);
	//	Reset the file pointer;
	virtual bool ResetPointer() { return Seek(0, AFILE_SEEK_SET); }
	//	Get file length
	virtual DWORD GetFileLength() { return (DWORD)m_dwFileLen; }

	//	Set file length
	bool SetFileLength(DWORD dwFileLen);
	//	Get file buffer
	BYTE* GetFileBuffer() { return m_pBuf; }

protected:	//	Attributes

	DWORD	m_dwBufLen;		//	Buffer size
	DWORD	m_dwFileLen;	//	File length
	BYTE*	m_pBuf;			//	Memory file buffer
	DWORD	m_dwOffset;		//	File pointer
	DWORD	m_dwGrowBy;		//	Minimum size of memory to be allocated when file buffer grow

protected:	//	Operations

	//	Open file
	virtual bool Open(const char* szFullPath, DWORD dwFlags) { return false; }
	virtual bool Open(const char* szFolderName, const char* szFileName, DWORD dwFlags) { return false; }
	virtual bool Close() { return false; }

	//	Extend size of memory file buffer before write data
	bool ExtendFileBeforeWrite(DWORD dwWriteSize);
};


#endif	//	_AMEMFILE_H_

