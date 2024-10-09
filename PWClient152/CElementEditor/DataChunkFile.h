/*
 * FILE: DataChunkFile.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2006/5/19
 *
 * HISTORY: 
 *
 * Copyright (c) 2006 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include <stdio.h>

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

class CELBitmap;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CDataChunkFile
//	
///////////////////////////////////////////////////////////////////////////

class CDataChunkFile
{
public:		//	Types

protected:	//	Types

	//	File header
	struct FILEHEADER
	{
		DWORD	dwVersion;		//	File version
		int		iNumChunk;		//	Chunk number
		bool	bCompress;		//	Data compress flag
	};

	//	Data chunk info.
	struct CHUNK
	{
		DWORD	dwOffset;		//	Data offset
		DWORD	dwSize;			//	Original data size
		DWORD	dwCompSize;		//	Compressed data size
	};

public:		//	Constructor and Destructor

	CDataChunkFile();
	virtual ~CDataChunkFile();

public:		//	Attributes

public:		//	Operations

	//	Open file
	bool OpenForWrite(const char* szFileName, int iNumChunk, bool bCompress);
	bool OpenForRead(const char* szFileName);
	//	Close file
	void Close();

	//	Write data of current chunk
	bool WriteDataChunk(int iIndex, void* pData, DWORD dwDataSize);
	//	Get chunk original data size
	DWORD GetChunkSize(int iIndex);
	bool ReadDataChunk(int iIndex, void* pDataBuf);

	//	Save gray bitmap to file
	bool SaveGrayBitmap(int iIndex, CELBitmap* pBitmap);
	//	Load gray bitmap from file
	bool LoadGrayBitmap(int iIndex, CELBitmap* pBitmap);

protected:	//	Attributes

	FILE*		m_pFile;		//	File pointer
	bool		m_bWriteData;	//	true, write data to file. false, read data from file

	FILEHEADER	m_Header;		//	File header
	int			m_iNumChunk;	//	Number of chunk
	CHUNK*		m_aChunks;		//	Data chunk info.

protected:	//	Operations

	//	Do finish work of writing
	bool FinishWrite();
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

