// FWSharedFile.cpp: implementation of the FWSharedFile class.
//
//////////////////////////////////////////////////////////////////////

#include "FWSharedFile.h"



FWSharedFile::FWSharedFile(DWORD dwGrowBytes, DWORD dwAllocFlags)
{
	m_dwAllocFlags = dwAllocFlags;
	m_dwGrowBytes = dwGrowBytes;

	m_hMem = NULL;
	m_dwMemSize = 0;
	m_bAllowGrow = (dwGrowBytes != 0);
	m_dwOffset = 0;
}

FWSharedFile::~FWSharedFile()
{
	Free();
}

HGLOBAL FWSharedFile::Detach()
{
	HGLOBAL ret = m_hMem;
	
	m_hMem = NULL;
	m_dwMemSize = 0;
	m_dwOffset = 0;

	return ret;
}

void FWSharedFile::SetHandle(HGLOBAL hGlobalMemory, bool bAllowGrow)
{
	Free();

	m_hMem = hGlobalMemory;
	m_dwMemSize = ::GlobalSize(m_hMem);
	m_bAllowGrow = bAllowGrow;
	m_dwOffset = 0;
}

bool FWSharedFile::Read(LPVOID pBuffer, DWORD dwBufferLength, DWORD * pReadLength)
{
	if (!IsMemAvailable()) return false;

	DWORD dwToRead = a_Min(m_dwMemSize - m_dwOffset, dwBufferLength);
	if (dwToRead <= 0)
	{
		if (pReadLength) *pReadLength = 0;
	}
	else
	{
		LPVOID pLocalBuf = ::GlobalLock(m_hMem);
		if (!pLocalBuf) return false;
		pLocalBuf = ((BYTE *)pLocalBuf) + m_dwOffset;

		memcpy(pBuffer, pLocalBuf, dwToRead);

		if (!::GlobalUnlock(m_hMem)) return false;
		
		m_dwOffset += dwToRead;

		if (pReadLength) *pReadLength = dwToRead;

	}
	return true;
}

bool FWSharedFile::Write(LPVOID pBuffer, DWORD dwBufferLength, DWORD * pWriteLength)
{
	if (!IsMemAvailable()) 
	{
		if (!Alloc(a_Max(m_dwGrowBytes, dwBufferLength))) 
			return false;
	}

	int nToGrow = (int)dwBufferLength - ((int)m_dwMemSize - (int)m_dwOffset);
	if (nToGrow > 0)
	{
		if (!Realloc(m_dwMemSize + nToGrow)) return false;
	}

	LPVOID pLocalBuf = ::GlobalLock(m_hMem);
	if (!pLocalBuf) return false;
	pLocalBuf = ((BYTE *)pLocalBuf) + m_dwOffset;

	memcpy(pLocalBuf, pBuffer, dwBufferLength);
	m_dwOffset += dwBufferLength;

	if (pWriteLength) *pWriteLength = dwBufferLength;

	return true;
}

bool FWSharedFile::Alloc(DWORD dwBytes)
{
	ASSERT(m_hMem == NULL);

	m_hMem = ::GlobalAlloc(m_dwAllocFlags, dwBytes);
	if (!m_hMem) return false;

	m_dwMemSize = dwBytes;
	m_dwOffset = 0;

	return true;
}

bool FWSharedFile::Free()
{
	if (!m_hMem) return true;

	m_hMem = ::GlobalFree(m_hMem);
	if (m_hMem) return false;
	
	m_dwMemSize = 0;
	m_dwOffset = 0;

	return true;
}

bool FWSharedFile::Realloc(DWORD dwBytes)
{
	// should only called for grow
	ASSERT(dwBytes > m_dwMemSize);

	if (!m_bAllowGrow) return false;

	HGLOBAL hNew = ::GlobalReAlloc(m_hMem, dwBytes, m_dwAllocFlags);
	if (!hNew) return false;

	m_hMem = hNew;
	m_dwMemSize = dwBytes;

	return true;
}

bool FWSharedFile::IsMemAvailable()
{
	return (m_hMem != NULL);
}

bool FWSharedFile::Seek(int iOffset, int iOrigin)
{
	int iNewPos = 0;
	if (iOrigin == AFILE_SEEK_SET)	
		iNewPos = iOffset;
	else if (iOrigin == AFILE_SEEK_END)
		iNewPos = (int)m_dwMemSize - 1 + iOffset;
	else if (iOrigin == AFILE_SEEK_CUR)
		iNewPos = (int)m_dwOffset + iOffset;
	else
		return false;

	if (iNewPos < 0 || iNewPos >= (int)m_dwMemSize) return false;

	m_dwOffset = (DWORD) iNewPos;
	return true;
}
