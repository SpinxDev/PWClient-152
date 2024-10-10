/*
 * FILE: AFileImage.h
 *
 * DESCRIPTION: The class which operating the file images in memory for Angelica Engine
 *
 * CREATED BY: Hedi, 2001/12/31
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _AFILEIMAGE_H_
#define _AFILEIMAGE_H_

#include "AFPlatform.h"
#include "AFile.h"
#include "AFilePackage.h"

class AFileImage : public AFile
{
private:
	AFilePackBase *	m_pPackage;		//	package object this file image open with
	LPBYTE			m_pFileImage;	//	Memory pointer of the file image in memory;
	int				m_nCurPtr;		//	In index into the file image buffer;
	int				m_nFileLength;	//	File length;
	DWORD			m_dwHandle;		//	Handle in file package
	bool			m_bTempMem;		//	true, use temporary memory alloctor

	bool fimg_read(LPBYTE pBuffer, int nSize, int * pReadSize); // read some size of data into a buffer;
	bool fimg_read_line(char * szLineBuffer, int nMaxLength, int * pReadSize); // read a line into a buffer;
	bool fimg_seek(int nOffset, int startPos); // offset current pointer

protected:

	bool Init(const char* szFullPath, bool bTempMem);
	bool Release();

public:
	
	AFileImage();
	virtual ~AFileImage();

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
	virtual DWORD GetFileLength() { return (DWORD)m_nFileLength; }

	inline LPBYTE GetFileBuffer() { return m_pFileImage; }
};

typedef AFileImage * PAFileImage;

#endif //_AFILEIMAGE_H_
