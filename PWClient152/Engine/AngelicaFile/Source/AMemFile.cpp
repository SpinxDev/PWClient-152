/*
 * FILE: AMemFile.cpp
 *
 * DESCRIPTION: Class for memory file
 *
 * CREATED BY: Duyuxin, 2003/10/19
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "AMemFile.h"
#include "AFPI.h"
#include "AMemory.h"

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

namespace ACommon
{
	int	a_MemUseableSize(void* pMem);
}

///////////////////////////////////////////////////////////////////////////
//
//	Local functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Implement AMemFile
//
///////////////////////////////////////////////////////////////////////////

AMemFile::AMemFile() : AFile()
{
	m_dwBufLen	= 0;
	m_dwFileLen	= 0;
	m_pBuf		= NULL;
	m_dwOffset	= 0;
	m_dwGrowBy	= 1024;
	m_dwFlags	= AFILE_CREATENEW;	//	Read and write
}

AMemFile::~AMemFile()
{
	if (m_pBuf)
	{
		a_free(m_pBuf);
	}
}

/*	Attach memory block

	Return true for success, otherwise return false.

	pBuf: buffer will be attached to memory file object. This buffer must be
		  allocated by t_New in t_Memory.cpp.
	dwBufLen: specifies the size of the buffer in bytes.
	dwGrowBy: the minimum size of memory to be allocated if memory file buffer
			 need to grow.

	Note: If dwGrowBy is zero, AMemFile will set the file length to dwBufLen. 
	This means that the data in the memory block before it was attached to 
	AMemFile will be used as the file. Memory files created in this manner 
	cannot be grown. If dwGrowBy is greater than zero, AMemFile will ignore 
	the contents of the memory block you have attached. If you attempt to 
	write past the end of the file or grow the file by calling the 
	AMemFile:SetFileLength.
*/
bool AMemFile::Attach(BYTE* pBuf, DWORD dwBufLen, DWORD dwGrowBy)
{
	//	A block has been attached before
	if (m_pBuf)
	{
		ASSERT(0 && "Memory file is attached twice!");
		return false;
	}

	if (!dwGrowBy)
	{
		//	Read-only file
		ASSERT(pBuf);
		m_dwFileLen = dwBufLen;
	}
	else
	{
		//	Read-write file
		ASSERT(dwGrowBy > 0);
		m_dwFileLen = 0;
	}

	m_dwGrowBy	= dwGrowBy;
	m_dwBufLen	= dwBufLen;
	m_pBuf		= pBuf;
	m_dwOffset	= 0;

	return true;
}

//	Detach memory block
BYTE* AMemFile::Detach()
{
	m_dwBufLen	= 0;
	m_dwFileLen	= 0;
	BYTE* pBuf	= m_pBuf;
	m_pBuf		= NULL;

	return pBuf;
}

/*	Extend size of memory file buffer before write data

	Return true for success, otherwise return false.

	dwWriteSize: size of data will be writen
*/
bool AMemFile::ExtendFileBeforeWrite(DWORD dwWriteSize)
{
	if (!dwWriteSize)
		return true;

	//	Memory file buffer is length enough ?
	if (m_dwOffset + dwWriteSize <= m_dwBufLen)
		return true;	//	Needn't extend

	DWORD dwAppendSize = m_dwOffset + dwWriteSize - m_dwBufLen;
	if (dwAppendSize < m_dwGrowBy)
		dwAppendSize = m_dwGrowBy;

	//	Extend memory file buffer
	BYTE* pNewBuf;

	if (!m_pBuf)
		pNewBuf = (BYTE*)a_malloc(m_dwBufLen + dwAppendSize);
	else
		pNewBuf = (BYTE*)a_realloc(m_pBuf, m_dwBufLen + dwAppendSize);
	
	if (!pNewBuf)
	{
		//	Not enough memory to extend file
		AFERRLOG(("AMemFile::ExtendFileBeforeWrite, Not enough memory"));
		return false;
	}

	m_pBuf		= pNewBuf;
	m_dwBufLen += dwAppendSize;

	return true;
}

/*	Read data from file

	Return true for success, otherwise return false.

	pBuffer: buffer used to receive read data
	dwBufferLength: Length of data wanted to be read
	pReadLength (out): size data actually read
*/
bool AMemFile::Read(LPVOID pBuffer, DWORD dwBufferLength, DWORD* pReadLength)
{
	ASSERT(m_pBuf && pBuffer);

	*pReadLength = 0;

	if (m_dwOffset >= m_dwFileLen)
		return false;

	if (!dwBufferLength)
		return true;

	DWORD dwRead = dwBufferLength;
	if (m_dwOffset + dwBufferLength > m_dwFileLen)
		dwRead = m_dwFileLen - m_dwOffset;

	memcpy(pBuffer, m_pBuf + m_dwOffset, dwRead);

	m_dwOffset += dwRead;

	*pReadLength = dwRead;

	return true;
}

/*	Write data to file

	Return true for success, otherwise return false.

	pBuffer: buffer which contains data will be written
	dwBufferLength: Length of data wanted to be written
	pWriteLength (out): size data actually written
*/
bool AMemFile::Write(LPVOID pBuffer, DWORD dwBufferLength, DWORD* pWriteLength)
{
	ASSERT(pBuffer && m_dwGrowBy);

	*pWriteLength = 0;

	if (m_dwOffset > m_dwFileLen)
		return false;
	
	if (!dwBufferLength)
		return true;

	if (!ExtendFileBeforeWrite(dwBufferLength))
	{
		AFERRLOG(("AMemFile::Write, Failed to allocate memory"));
		return false;
	}

	//	Memory file buffer is length enough ?
	memcpy(m_pBuf+m_dwOffset, pBuffer, dwBufferLength);
	
	if (m_dwOffset + dwBufferLength > m_dwFileLen)
		m_dwFileLen = m_dwOffset + dwBufferLength;

	m_dwOffset	   += dwBufferLength;
	*pWriteLength	= dwBufferLength;

	return true;
}

//	Set file length
bool AMemFile::SetFileLength(DWORD dwFileLen)
{
	if (dwFileLen <= m_dwBufLen)
		m_dwFileLen = dwFileLen;
	else
	{
		//	Extend memory buffer
		ExtendFileBeforeWrite(dwFileLen - m_dwBufLen);
		m_dwFileLen = dwFileLen;
	}

	return false;
}

/*	Seek file pointer

	Return true for success, otherwise return false.

	iOffset: move offset
	iOrigin: origin flags AFILE_SEEK_* defined in AFile.h
*/
bool AMemFile::Seek(int iOffset, AFILE_SEEK origin)
{
	DWORD dwOff;

	switch (origin)
	{
	case AFILE_SEEK_SET:	
		
		dwOff = iOffset < 0 ? 0 : iOffset;
		break;

	case AFILE_SEEK_CUR:

		if (iOffset < 0)
			dwOff = (DWORD)(-iOffset) >= m_dwOffset ? 0 : m_dwOffset + iOffset;
		else
			dwOff = m_dwOffset + iOffset;

		break;

	case AFILE_SEEK_END:

		if (iOffset <= 0)
			dwOff = (DWORD)(-iOffset) >= m_dwFileLen ? 0 : m_dwFileLen + iOffset;
		else
			dwOff = m_dwOffset + iOffset;

		break;

	default:
		ASSERT(0);
		return false;
	}

	m_dwOffset = dwOff > m_dwFileLen ? m_dwFileLen : dwOff;

	return true;
}

bool AMemFile::ReadLine(char* szLineBuffer, DWORD dwBufferLength, DWORD* pdwReadLength)
{
	DWORD dwSizeRead = 0;

	while (m_dwOffset < m_dwFileLen)
	{
		BYTE byteThis = m_pBuf[m_dwOffset];

		if (byteThis != 0x0d && byteThis != 0x0a)
		{
			//	Not \n or \r, so copy it into the buffer;
			szLineBuffer[dwSizeRead++] = m_pBuf[m_dwOffset++];
		}
		else
		{
			m_dwOffset++;

			if (byteThis == 0x0d)
			{
				//	We need to check if next byte is \r, if so, just remove it;
				if (m_dwOffset < m_dwFileLen)
				{
					if (m_pBuf[m_dwOffset] == 0x0a)
						m_dwOffset++;
				}
			}
			
			break;
		}
	}

	szLineBuffer[dwSizeRead] = '\0';

	if (!dwSizeRead)
		return true;

	*pdwReadLength = dwSizeRead + 1;

	return true;
}

bool AMemFile::ReadString(char* szLineBuffer, DWORD dwBufferLength, DWORD* pdwReadLength)
{
	char ch;
	DWORD dwReadLength;
	DWORD dwStrLen = 0;

	Read(&ch, 1, &dwReadLength);
	while (ch) 
	{
		szLineBuffer[dwStrLen++] = ch;

		if (dwStrLen >= dwBufferLength)
			return false;

		Read(&ch, 1, &dwReadLength);
	}

	szLineBuffer[dwStrLen] = '\0';

	*pdwReadLength = dwStrLen + 1;

	return true;
}

bool AMemFile::WriteLine(const char* szLineBuffer)
{
	ASSERT(m_dwGrowBy);

	DWORD dwWrite;

	int iLen = strlen(szLineBuffer);
	if (iLen)
	{
		if (!Write((void*)szLineBuffer, iLen, &dwWrite))
			return false;
	}

	//	Write a '\n'
	char ch = '\n';
	Write(&ch, sizeof (ch), &dwWrite);

	return true;
}

bool AMemFile::WriteString(const AString& str)
{
	DWORD dwWrite;

	//	Write length of string
	int iLen = str.GetLength();
	Write(&iLen, sizeof (int), &dwWrite);

	//	Write string data
	if (iLen)
	{
		if (!Write((void*)(const char*)str, iLen, &dwWrite))
			return false;
	}

	return true;
}

bool AMemFile::ReadString(AString& str)
{
	DWORD dwRead;

	//	Read length of string
	int iLen;
	Read(&iLen, sizeof (int), &dwRead);

	//	Read string data
	if (iLen)
	{
		char* szBuf = new char [iLen+1];
		if (!szBuf)
			return false;

		if (!Read(szBuf, iLen, &dwRead))
			return false;
		
		szBuf[iLen] = '\0';
		str = szBuf;

		delete [] szBuf;
	}
	else
		str = "";

	return true;
}

