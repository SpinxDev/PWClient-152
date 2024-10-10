 /*
 * FILE: AFilePackBase.cpp
 *
 * DESCRIPTION: A File Package Class for Angelica Engine
 *
 * CREATED BY: Hedi, 2002/4/15
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "AFPI.h"
#include "AFI.h"
#include "AFilePackBase.h"
#include "AMemory.h"
#include "AAssist.h"
#include <io.h>

#define new A_DEBUG_NEW

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Reference to External variables and functions
//
///////////////////////////////////////////////////////////////////////////

extern int	AFPCK_GUARDBYTE0;
extern int	AFPCK_GUARDBYTE1;
extern int	AFPCK_MASKDWORD;
extern int	AFPCK_CHECKMASK;

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

static const size_t NET_DISK_RW_MAX_SIZE = 1024*1024;	// 初始一次最大读写长度
static const DWORD IO_TIMEOUT_ERROR_COUNT = 120000;		// 读写超时时限

size_t _FileWrite(const void* buffer, const size_t num_byte, FILE* stream)
{
	if (!stream || !buffer)
	{
	//	AFERRLOG(("Write Param ERROR! pBuffer:%d, fp:%d, FileSize:%d\n\n", (void*)buffer, (void*)stream, num_byte));
		return 0;
	}

	const long lBeginOffset = ftell(stream);
	if (lBeginOffset==-1L)
	{
		AFERRLOG(("ftell ERROR, check whether devices support file seeking!!\n\n"));
		return 0;
	}

	const LPBYTE pBuf = (LPBYTE)buffer; 
	size_t sizeMaxOnceWrite = NET_DISK_RW_MAX_SIZE;
	DWORD dwStartTimeCnt = GetTickCount();
	DWORD dwOffset = 0;
	while (dwOffset<num_byte)
	{
		DWORD dwWrite = min(num_byte-dwOffset, sizeMaxOnceWrite);
		DWORD dwActuallyWrite = fwrite(pBuf+dwOffset, 1, dwWrite, stream);
	
		if (dwActuallyWrite==dwWrite)
		{
			// 写入成功
			dwOffset += dwWrite;
			dwStartTimeCnt = GetTickCount();
		}
		else
		{
			if ((int(GetTickCount()-dwStartTimeCnt))>IO_TIMEOUT_ERROR_COUNT)
			{
				// 超过指定时间没有写入成功
				AFERRLOG(("Write ERROR: SIZE:%d, OFFSET:%d, TRY_WRITE:%d\n\n", num_byte, dwOffset, dwWrite));
				return (size_t)dwOffset;
			}

			// 调整一次最大写入上限
			if (sizeMaxOnceWrite>=2)
			{
				sizeMaxOnceWrite >>= 1;
			}

			// 将文件标识归位
			if (fseek(stream, lBeginOffset+dwOffset, SEEK_SET))
			{
				AFERRLOG(("fseek ERROR, check whether devices support file seeking!!\n\n"));			
				return (size_t)dwOffset;
			}			
		}
	}

	return (size_t)dwOffset;
}

size_t _FileRead(void* buffer, const size_t num_byte, FILE* stream)
{
	if (!stream || !buffer)
	{
	//	AFERRLOG(("Read Param ERROR! pBuffer:%d, fp:%d, FileSize:%d\n\n", (void*)buffer, (void*)stream, num_byte));
		return 0;
	}
 
	const long lBeginOffset = ftell(stream);
	if (lBeginOffset==-1L)
	{
		AFERRLOG(("ftell ERROR, check whether devices support file seeking!!\n\n"));			
		return 0;
	}

	LPBYTE pBuf = (LPBYTE)buffer; 
	size_t sizeMaxOnceRead = NET_DISK_RW_MAX_SIZE;
	DWORD dwStartTimeCnt = GetTickCount();
	DWORD dwOffset = 0;
	while (dwOffset<num_byte)
	{
		DWORD dwRead = min(num_byte-dwOffset, sizeMaxOnceRead);
		DWORD dwActuallyRead = fread(pBuf+dwOffset, 1, dwRead, stream);

		if (dwActuallyRead==dwRead)
		{
			// 读取成功
			dwOffset += dwRead;
			dwStartTimeCnt = GetTickCount();
		}
		else
		{
			if ((int(GetTickCount()-dwStartTimeCnt))>IO_TIMEOUT_ERROR_COUNT)
			{
				// 超过指定时间没有读取成功
				AFERRLOG(("Read ERROR: SIZE:%d, OFFSET:%d, TRY_READ:%d\n\n", num_byte, dwOffset, dwRead));
				return (size_t)dwOffset;
			}

			// 调整一次最大读取上限
			if (sizeMaxOnceRead>=2)
			{
				sizeMaxOnceRead >>= 1;
			}

			// 将文件标识归位
			if (fseek(stream, lBeginOffset+dwOffset, SEEK_SET))
			{
				AFERRLOG(("fseek ERROR, check whether devices support file seeking!!\n\n"));			
				return (size_t)dwOffset;
			}			
		}
	}

	return (size_t)dwOffset;
}

///////////////////////////////////////////////////////////////////////////
//
//	Implement of AFilePackBase
//
///////////////////////////////////////////////////////////////////////////

AFilePackBase::CPackageFile::CPackageFile()
{
	m_pFile1 = NULL;
	m_pFile2 = NULL;

	m_size1 = 0;
	m_size2 = 0;

	m_filePos = 0;
}

AFilePackBase::CPackageFile::~CPackageFile()
{
}

bool AFilePackBase::CPackageFile::Open(const char * szFileName, const char * szMode)
{
	Close();

	m_pFile1 = fopen(szFileName, szMode);
	if( NULL == m_pFile1 )
		return false;

	fseek(m_pFile1, 0, SEEK_END);
	m_size1 = ftell(m_pFile1);
	fseek(m_pFile1, 0, SEEK_SET);

	m_filePos = 0;
	strncpy(m_szPath, szFileName, MAX_PATH);
	strncpy(m_szMode, szMode, 32);

	strcpy(m_szPath2, m_szPath);
	af_ChangeFileExt(m_szPath2, MAX_PATH, ".pkx");
	return true;
}

bool AFilePackBase::CPackageFile::Phase2Open(DWORD dwOffset)
{
	if( dwOffset >= MAX_FILE_PACKAGE )
	{
		// we need the second file now;
		m_pFile2 = fopen(m_szPath2, m_szMode);
		if( NULL == m_pFile2 )
		{										  
			if( stricmp(m_szMode, "r+b") == 0 && !af_IsFileExist(m_szPath2) )
			{
				// it is the first time we access the second file	
				m_pFile2 = fopen(m_szPath2, "wb+");
				if( NULL == m_pFile2 )
					return false;
			}
			else
				return false;
		}

		fseek(m_pFile2, 0, SEEK_END);
		m_size2 = ftell(m_pFile2);
		fseek(m_pFile2, 0, SEEK_SET);
	}

	return true;
}

bool AFilePackBase::CPackageFile::Close()
{
	if( m_pFile2 )
	{
		fclose(m_pFile2);
		m_pFile2 = NULL;
	}

	if( m_pFile1 )
	{
		fclose(m_pFile1);
		m_pFile1 = NULL;
	}

	m_size1 = 0;
	m_size2 = 0;
	m_filePos = 0;

	return true;
}

size_t AFilePackBase::CPackageFile::read(void *buffer, size_t size, size_t count)
{
	size_t size_to_read = size * count;
	__int64 new_pos = m_filePos + size_to_read;

	if( new_pos <= MAX_FILE_PACKAGE )
	{
		// completely in file 1
		size_t readsize = _FileRead(buffer, size_to_read, m_pFile1);
		m_filePos += readsize;

		//	Bug fixed by dyx at 2013-4-24: if m_filePos is moved to MAX_FILE_PACKAGE after writting at it happens,
		//	next reading operation will go to case 3 other than case 2, so we should reset m_pFile2's file pointer to
		//	ensure next reading operation to start at correct position.
		if( m_filePos == MAX_FILE_PACKAGE && m_pFile2)
			fseek(m_pFile2, 0, SEEK_SET);

		return readsize;
	}
	else if( m_filePos < MAX_FILE_PACKAGE )
	{
		// partial in file1 and partial in file 2
		size_t size_to_read1 = (size_t)(MAX_FILE_PACKAGE - m_filePos);
		size_t size_to_read2 = (size_t)(size_to_read - size_to_read1);

		// read from file1
		size_t readsize = _FileRead(buffer, size_to_read1, m_pFile1);
		if (readsize != size_to_read1)
		{
			m_filePos += readsize;
			return readsize;
		}

		if( m_pFile2 )
		{
			// read from file2
			fseek(m_pFile2, 0, SEEK_SET);
			readsize += _FileRead((LPBYTE)buffer + size_to_read1, size_to_read2, m_pFile2);
		}

		m_filePos += readsize;
		return readsize;
	}
	else
	{
		// completely in file 2
		size_t readsize = _FileRead(buffer, size_to_read, m_pFile2);
		m_filePos += readsize;
		return readsize;
	}

	return 0;
}

size_t AFilePackBase::CPackageFile::write(const void *buffer, size_t size, size_t count)
{
	size_t size_to_write = size * count;
	__int64 new_size = m_filePos + size_to_write;

	if( new_size <= MAX_FILE_PACKAGE )
	{
		// case 1: completely in file 1
		size_t writesize = _FileWrite(buffer, size_to_write, m_pFile1);
		m_filePos += writesize;
		if( m_filePos > m_size1 )
			m_size1 = m_filePos;
		return writesize;
	}
	else if( m_filePos < MAX_FILE_PACKAGE )
	{
		// case 2: partial in file1 and partial in file 2
		size_t size_to_write1 = MAX_FILE_PACKAGE - m_filePos;
		size_t size_to_write2 = size_to_write - size_to_write1;

		// write to file1
		size_t writesize1 = _FileWrite(buffer, size_to_write1, m_pFile1);
		m_filePos += writesize1;
		if (m_filePos > m_size1)
			m_size1 = m_filePos;

		// By MSDN:
		// fwrite returns the number of full items actually written, which may be less than count if an error occurs. 
		// Also, if an error occurs, the file-position indicator cannot be determined.
		if (writesize1 != size_to_write1)
		{
			fseek(m_pFile1, m_filePos, SEEK_SET);
			return writesize1;
		}

		m_size1 = MAX_FILE_PACKAGE;

		if( !m_pFile2 )
			Phase2Open(MAX_FILE_PACKAGE);

		// write to file2
		fseek(m_pFile2, 0, SEEK_SET);
		size_t writesize2 = _FileWrite((LPBYTE)buffer + size_to_write1, size_to_write2, m_pFile2);
		m_filePos += writesize2;
		if( m_filePos > m_size1 + m_size2 )
			m_size2 = m_filePos - m_size1;
		return writesize1 + writesize2;
	}
	else
	{
		//	case 3: completely in file 2

		//	Bug fixed by dyx at 2013-4-24: If last file was written in the way of case 1, 
		//	and m_filePos was moved to MAX_FILE_PACKAGE after writting at it happens. Then
		//	next write operation will come to case 3 other than case 2, so we should check whether
		//	m_pFile2 has been existed or not.
		if( !m_pFile2 )
		{
			Phase2Open(MAX_FILE_PACKAGE);
			fseek(m_pFile2, 0, SEEK_SET);
		}

		size_t writesize = _FileWrite(buffer, size_to_write, m_pFile2);
		m_filePos += writesize;
		if( m_filePos > m_size1 + m_size2 )
			m_size2 = m_filePos - m_size1;
		return writesize;
	}

	return 0;
}

void AFilePackBase::CPackageFile::seek(__int64 offset, int origin)
{
	__int64 newpos = m_filePos;

	if( m_pFile2 )
	{
		switch(origin)
		{
		case SEEK_SET:
			newpos = offset;
			break;

		case SEEK_CUR:
			newpos = m_filePos + offset;
			break;

		case SEEK_END:
			newpos = m_size1 + m_size2 + offset;
			break;
		}

		if( newpos < 0 )
			newpos = 0;
		if( newpos > m_size1 + m_size2 )
			newpos = m_size1 + m_size2;

		if( newpos < m_size1 )
			fseek(m_pFile1, newpos, SEEK_SET);
		else
			fseek(m_pFile2, newpos - m_size1, SEEK_SET);

		m_filePos = newpos;
	}
	else
	{
		fseek(m_pFile1, offset, origin);
		m_filePos = ftell(m_pFile1);
	}

	return;
}

DWORD AFilePackBase::CPackageFile::tell()
{
	return (DWORD)m_filePos;
}

void AFilePackBase::CPackageFile::SetPackageFileSize(DWORD dwFileSize)
{
	if( m_pFile2 )
	{
		if( dwFileSize <= MAX_FILE_PACKAGE )
		{
			int fileHandle = _fileno(m_pFile1);
			_chsize(fileHandle, dwFileSize);
			m_size1 = dwFileSize;

			fclose(m_pFile2);
			m_pFile2 = NULL;
			remove(m_szPath2);
			m_size2 = 0;
		}
		else
		{
			int fileHandle = _fileno(m_pFile2);
			m_size2 = dwFileSize - MAX_FILE_PACKAGE;
			_chsize(fileHandle, m_size2);
		}
	}
	else
	{
		int fileHandle = _fileno(m_pFile1);
		_chsize(fileHandle, dwFileSize);
		m_size1 = dwFileSize;
	}
}
