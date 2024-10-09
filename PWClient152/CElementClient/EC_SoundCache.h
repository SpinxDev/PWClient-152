/*
 * FILE: CECSoundCache.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2005/7/11
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include "AMSoundBufferMan.h"
#include "hashtab.h"
#include "AString.h"
#include "AList2.h"

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

class AFileImage;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CECSoundCache
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CECSoundCache
//	
///////////////////////////////////////////////////////////////////////////

enum SoundLoadType
{
	enumSoundLoadAlways,
	enumSoundLoadNonLoop,
	enumSoundLoadLoop,
	enumSoundLoadTypeNum
};

class CECSoundCache : public AMSoundFileCache
{
public:		//	Types

	//	File cache item
	struct FILECACHEITEM
	{
		AFileImage*		pFile;			//	File image object
		DWORD			dwTimeStamp;	//	Time stamp
		ALISTPOSITION	pos;			//	Item position in
	};

	typedef abase::hashtab<FILECACHEITEM*, AString, abase::_hash_function>	FileTable;

public:		//	Constructor and Destructor

	CECSoundCache();
	virtual ~CECSoundCache();

public:		//	Attributes

public:		//	Operations

	//	Release object
	void Release();

	//	Load file
	virtual AFileImage* LoadFile(const char* szFile);
	//	Close file
	virtual void CloseFile(AFileImage* pFile);

	//	For performance analyse
	int GetLoadFileCnt() { return m_iLoadFileCnt; }
	int GetFileCacheHitCnt() { return m_iFileCacheHitCnt; }

	//	Set / Get file cache sizes
	int GetMaxFileSize() { return m_iMaxFileSize; }
	void SetMaxFileSize(int iSize) { m_iMaxFileSize = iSize; }
	int GetMaxCacheSize() { return m_iMaxCacheSize; }
	void SetMaxCacheSize(int iSize) { m_iMaxCacheSize = iSize; }

	int GetCacheSize() { return m_iCurCacheSize; }

	AM3DSoundBuffer* LoadSound(const char* szFile, SoundLoadType enumType);
	void ReleaseSound(AM3DSoundBuffer*& pSound, SoundLoadType enumType);

	int GetLoadCount(SoundLoadType type) { return m_LoadCounts[type]; }

protected:	//	Attributes

	int		m_iMaxFileSize;		//	Maximum size (byte) of single cached file
	int		m_iMaxCacheSize;	//	Maximum size (byte) of file cache
	int		m_iCurCacheSize;	//	Current file cache size (byte)

	int		m_LoadCounts[enumSoundLoadTypeNum];

	FileTable					m_FileTab;		//	File table
	APtrList<FILECACHEITEM*>	m_OrderList;	//	File item list ordered by time stamp

	//	For performance analyse
	int		m_iLoadFileCnt;			//	Total number of file load operation
	int		m_iFileCacheHitCnt;		//	Times of file cache hit

protected:	//	Operations

	//	Allocate a file cache item
	FILECACHEITEM* AllocFileCacheItem(AFileImage* pFile);
	//	Free a file cache item
	void FreeFileCacheItem(FILECACHEITEM* pFileItem);
	//	Add file image into cache
	bool AddFileToCache(const AString& strRelFile, AFileImage* pFile);
	//	Remove files from file cache to make more free space
	bool RemoveFilesFromCache(int iFreeSpace);
	//	Find a file in file cache
	FILECACHEITEM* FindCachedFile(const AString& strRelFile);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

