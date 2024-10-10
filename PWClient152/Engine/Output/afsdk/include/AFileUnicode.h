/*
 * FILE: AFileUnicode.h
 *
 * DESCRIPTION: The class which operating the file on the disk using an unicode version API but ANSI filename for Angelica Engine
 *				This is only used for stream file reading for music
 *
 * CREATED BY: Hedi, 2008/6/10
 *
 * HISTORY:
 *
 * Copyright (c) 2008 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _AFILEUNICODE_H_
#define _AFILEUNICODE_H_

#include "AFPlatform.h"
#include "AFile.h"

class AFileUnicode : public AFile
{
private:
	HANDLE		m_hFile;			// file handle

public:
	AFileUnicode();
	virtual ~AFileUnicode();

	virtual bool Open(const char* szFullPath, DWORD dwFlags);
	virtual bool Open(const char* szFolderName, const char* szFileName, DWORD dwFlags);
	virtual bool Close();

	virtual bool Read(LPVOID pBuffer, DWORD dwBufferLength, DWORD * pReadLength);
	virtual bool Write(LPVOID pBuffer, DWORD dwBufferLength, DWORD * pWriteLength);
 
	virtual bool ReadLine(char * szLineBuffer, DWORD dwBufferLength, DWORD * pdwReadLength);
	virtual bool ReadString(char * szLineBuffer, DWORD dwBufferLength, DWORD * pdwReadLength);
	virtual bool WriteLine(const char* szLineBuffer);
	virtual bool WriteString(const AString& str);
	virtual bool ReadString(AString& str);
	
	virtual DWORD GetPos();
	virtual bool Seek(int iOffset, AFILE_SEEK origin);
	virtual bool ResetPointer();
	//	Get file length
	virtual DWORD GetFileLength();

	static bool IsFileUnicodeExist(const char * szFullPath);
};

typedef AFileUnicode * PAFileUnicode;

#endif //_AFILEUNICODE_H_
