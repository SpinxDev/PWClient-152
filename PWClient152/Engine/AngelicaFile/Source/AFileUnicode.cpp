 /*
 * FILE: AFileUnicode.cpp
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

#include "AFPI.h"
#include "AFileUnicode.h"
#include "AFI.h"

#define new A_DEBUG_NEW

AFileUnicode::AFileUnicode() : AFile()
{
	m_hFile = NULL;
}

AFileUnicode::~AFileUnicode()
{
	Close();
}

bool AFileUnicode::Open(const char* szFolderName, const char * szFileName, DWORD dwFlags)
{
	char szFullPath[MAX_PATH];
	af_GetFullPath(szFullPath, szFolderName, szFileName);
	return Open(szFullPath, dwFlags);
}

bool AFileUnicode::Open(const char* szFullPath, DWORD dwFlags)
{
	// If already opened, we must first close it!
	if( m_bHasOpened )
		Close();

	strncpy(m_szFileName, szFullPath, MAX_PATH);
	// Get a relative path name of this file, may use a little time, but
	// this call is not too often, so this is not matter
	af_GetRelativePath(szFullPath, m_szRelativeName);

	// now this class can only support read music file
	if( 0 == (dwFlags & AFILE_OPENEXIST) )
		return false;

	wchar_t szUniName[MAX_PATH];
	if( !a_MultiByteToWideChar(936, szFullPath, strlen(szFullPath) + 1, szUniName, MAX_PATH) )
		return false;

	m_hFile = CreateFileW(szUniName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if( INVALID_HANDLE_VALUE == m_hFile )
		return false;
	
	m_dwTimeStamp = 0;
	m_bHasOpened = true;
	return true;
}

bool AFileUnicode::Close()
{
	if( m_hFile )
	{
		CloseHandle(m_hFile);
		m_hFile = NULL;
	}

	m_bHasOpened = false;
	return true;
}

bool AFileUnicode::ResetPointer()
{
	SetFilePointer(m_hFile, 0, NULL, FILE_BEGIN);
	return true;
}

bool AFileUnicode::ReadLine(char * szLineBuffer, DWORD dwBufferLength, DWORD * pdwReadLength)
{
	// now this class can only support read music file
	return false;
}

bool AFileUnicode::Read(LPVOID pBuffer, DWORD dwBufferLength, DWORD * pReadLength)
{
	if( 0 == ReadFile(m_hFile, pBuffer, dwBufferLength, pReadLength, NULL) )
		return false;
	return true;
}

bool AFileUnicode::Write(LPVOID pBuffer, DWORD dwBufferLength, DWORD * pWriteLength)
{
	return false;
}

DWORD AFileUnicode::GetPos()
{
	DWORD dwCurrentFilePosition = 0;

	dwCurrentFilePosition = SetFilePointer(m_hFile, 0, NULL, FILE_CURRENT);
	return dwCurrentFilePosition;
}

bool AFileUnicode::Seek(int iOffset, AFILE_SEEK origin)
{
	DWORD dwMoveMethod = FILE_BEGIN;

	switch (origin)
	{
	case AFILE_SEEK_SET:	dwMoveMethod = FILE_BEGIN;		break;
	case AFILE_SEEK_CUR:	dwMoveMethod = FILE_CURRENT;	break;
	case AFILE_SEEK_END:	dwMoveMethod = FILE_END;		break;
	default:
	{
		ASSERT(0);
		return false;
	}
	}

	if( INVALID_SET_FILE_POINTER == SetFilePointer(m_hFile, iOffset, NULL, dwMoveMethod) )
		return false;

	return true;
}

bool AFileUnicode::WriteLine(const char* szLineBuffer)
{
	// now this class can only support read music file
	return false;
}

bool AFileUnicode::ReadString(char * szLineBuffer, DWORD dwBufferLength, DWORD * pdwReadLength)
{
	// now this class can only support read music file
	return false;
}

bool AFileUnicode::WriteString(const AString& str)
{
	// now this class can only support read music file
	return false;
}

bool AFileUnicode::ReadString(AString& str)
{
	// now this class can only support read music file
	return false;
}

//	Get file length
DWORD AFileUnicode::GetFileLength()
{
	ASSERT(m_hFile);

	DWORD dwPos, dwEnd;

	dwPos = GetPos();
	dwEnd = SetFilePointer(m_hFile, 0, NULL, FILE_END);
	SetFilePointer(m_hFile, dwPos, NULL, FILE_BEGIN);
	return dwEnd;
}

bool AFileUnicode::IsFileUnicodeExist(const char * szFullPath)
{
	HANDLE hFile = NULL;

 	wchar_t szUniName[MAX_PATH];
	if( !a_MultiByteToWideChar(936, szFullPath, strlen(szFullPath) + 1, szUniName, MAX_PATH) )
		return false;

	hFile = CreateFileW(szUniName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if( INVALID_HANDLE_VALUE == hFile )
		return false;

	CloseHandle(hFile);
	return true;
}