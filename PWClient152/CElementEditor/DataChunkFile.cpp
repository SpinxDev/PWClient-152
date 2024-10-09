/*
 * FILE: DataChunkFile.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2006/5/19
 *
 * HISTORY: 
 *
 * Copyright (c) 2006 Archosaur Studio, All Rights Reserved.
 */

#include "Global.h"
#include "DataChunkFile.h"
#include "AFilePackage.h"
#include "Bitmap.h"

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

#define DATACHUNKFILE_VERSION	1

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


///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Implement CDataChunkFile
//	
///////////////////////////////////////////////////////////////////////////

CDataChunkFile::CDataChunkFile()
{
	m_pFile			= NULL;
	m_bWriteData	= false;
	m_aChunks		= NULL;
	m_iNumChunk		= 0;

	memset(&m_Header, 0, sizeof (m_Header));
}

CDataChunkFile::~CDataChunkFile()
{
}

//	Open file for writing
bool CDataChunkFile::OpenForWrite(const char* szFileName, int iNumChunk, bool bCompress)
{
	if (!(m_pFile = fopen(szFileName, "wb+")))
		return false;

	if (!(m_aChunks = new CHUNK [iNumChunk]))
	{
		fclose(m_pFile);
		m_pFile = NULL;
		return false;
	}
	
	m_bWriteData	= true;
	m_iNumChunk		= iNumChunk;

	DWORD dwChunkSize = sizeof (CHUNK) * iNumChunk;
	memset(m_aChunks, 0, dwChunkSize);

	m_Header.dwVersion	= DATACHUNKFILE_VERSION;
	m_Header.iNumChunk	= iNumChunk;
	m_Header.bCompress	= bCompress;

	//	Write file header
	fwrite(&m_Header, sizeof (m_Header), 1, m_pFile);
	//	Write chunk info. This time is just to hold place
	fwrite(m_aChunks, dwChunkSize, 1, m_pFile);

	return true;
}

//	Open file for reading
bool CDataChunkFile::OpenForRead(const char* szFileName)
{
	m_bWriteData = false;

	if (!(m_pFile = fopen(szFileName, "rb")))
		return false;

	//	Read file header
	fread(&m_Header, sizeof (m_Header), 1, m_pFile);

	//	Read chunk info.
	if (m_Header.iNumChunk)
	{
		if (!(m_aChunks = new CHUNK [m_Header.iNumChunk]))
			return false;

		DWORD dwChunkSize = sizeof (CHUNK) * m_Header.iNumChunk;
		fread(m_aChunks, dwChunkSize, 1, m_pFile);
	}

	m_iNumChunk = m_Header.iNumChunk;

	return true;
}

//	Close file
void CDataChunkFile::Close()
{
	if (m_pFile)
	{
		if (m_bWriteData)
			FinishWrite();

		fclose(m_pFile);
		m_pFile = NULL;
	}

	if(m_aChunks) delete[] m_aChunks;
}

//	Write a data chunk
bool CDataChunkFile::WriteDataChunk(int iIndex, void* pData, DWORD dwDataSize)
{
	if (!m_pFile || !m_bWriteData || !m_aChunks)
		return false;

	if (iIndex <  0 || iIndex >= m_iNumChunk)
		return false;

	CHUNK& Chunk = m_aChunks[iIndex];
	Chunk.dwOffset		= ftell(m_pFile);
	Chunk.dwSize		= dwDataSize;
	Chunk.dwCompSize	= 0;

	BYTE* pWriteData = (BYTE*)pData;
	DWORD dwWriteSize = dwDataSize;

	if (m_Header.bCompress)
	{
		if (!(pWriteData = new BYTE [dwDataSize * 2]))
			return false;

		dwWriteSize = dwDataSize * 2;
		AFilePackage::Compress((BYTE*)pData, dwDataSize, pWriteData, &dwWriteSize);
		Chunk.dwCompSize = dwWriteSize;
	}

	fwrite(pWriteData, dwWriteSize, 1, m_pFile);

	if (m_Header.bCompress && pWriteData)
	{
		delete [] pWriteData;
		pWriteData = NULL;
	}

	return true;
}

//	Do finish work of writing
bool CDataChunkFile::FinishWrite()
{
	if (!m_pFile || !m_bWriteData || !m_aChunks)
		return false;

	//	Re-write chunk information
	fseek(m_pFile, sizeof (FILEHEADER), SEEK_SET);
	fwrite(m_aChunks, sizeof (CHUNK) * m_iNumChunk, 1, m_pFile);

	return true;
}

//	Get chunk original data size
DWORD CDataChunkFile::GetChunkSize(int iIndex)
{
	if (iIndex >=  0 || iIndex < m_iNumChunk)
		return m_aChunks[iIndex].dwSize;

	return 0;
}

bool CDataChunkFile::ReadDataChunk(int iIndex, void* pDataBuf)
{
	if (!m_pFile || m_bWriteData || !m_aChunks)
		return false;

	if (iIndex <  0 || iIndex >= m_iNumChunk)
		return false;

	CHUNK& Chunk = m_aChunks[iIndex];
	fseek(m_pFile, Chunk.dwOffset, SEEK_SET);

	if (m_Header.bCompress)
	{
		BYTE* pCompBuf = new BYTE [Chunk.dwCompSize];
		if (!pCompBuf)
			return false;

		fread(pCompBuf, Chunk.dwCompSize, 1, m_pFile);

		DWORD dwSize = Chunk.dwSize;
		AFilePackage::Uncompress(pCompBuf, Chunk.dwCompSize, (BYTE*)pDataBuf, &dwSize);
		delete [] pCompBuf;
	}
	else
	{
		fread(pDataBuf, Chunk.dwSize, 1, m_pFile);
	}

	return true;
}

//	Save gray bitmap to file
bool CDataChunkFile::SaveGrayBitmap(int iIndex, CELBitmap* pBitmap)
{
	if (!pBitmap)
		return false;

	int iTotalSize = pBitmap->CalcBitmapFileSize();
	BYTE* pBuffer = (BYTE*)a_malloc(iTotalSize);
	if (!pBuffer)
	{
		g_Log.Log("CDataChunkFile::SaveGrayBitmap, Not enough memory");
		return false;
	}
	
	if (!pBitmap->SaveToMemory(pBuffer))
	{
		a_free(pBuffer);
		g_Log.Log("CDataChunkFile::SaveGrayBitmap, Failed to save bitmap data");
		return false;
	}

	//	Write whole bitmap to file
	WriteDataChunk(iIndex, pBuffer, (DWORD)iTotalSize);

	a_free(pBuffer);

	return true;
}

//	Load gray bitmap from file
bool CDataChunkFile::LoadGrayBitmap(int iIndex, CELBitmap* pBitmap)
{
	if (!pBitmap)
		return false;

	DWORD dwSize = GetChunkSize(iIndex);
	if (!dwSize)
		return false;

	BYTE* pBuffer = (BYTE*)a_malloc(dwSize);
	if (!pBuffer)
	{
		g_Log.Log("CDataChunkFile::LoadGrayBitmap, Not enough memory");
		return false;
	}

	ReadDataChunk(iIndex, pBuffer);

	if (!pBitmap->LoadFromMemory(pBuffer))
	{
		a_free(pBuffer);
		g_Log.Log("CDataChunkFile::LoadGrayBitmap, Failed to load bitmap data");
		return false;
	}

	a_free(pBuffer);

	return true;
}

