/*
 * FILE: CECSoundCache.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2005/7/11
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#pragma warning (disable: 4284)

#include "EC_SoundCache.h"
#include "EC_Global.h"
#include "AFileImage.h"
#include "AFI.h"
#include "AMemory.h"
#include "EC_Game.h"
#include "A3DEngine.h"
#include "AMSoundEngine.h"
#include "AMSoundBufferMan.h"

#define new A_DEBUG_NEW

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

static const int _sound_load_max_counts[enumSoundLoadTypeNum] =
{
	0x7FFFFFFF,
	10,
	3
};

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
//	Implement CECSoundCache
//	
///////////////////////////////////////////////////////////////////////////

CECSoundCache::CECSoundCache() : m_FileTab(512)
{
	m_iMaxFileSize		= 250 * 1024;
	m_iMaxCacheSize		= 10 * 1024 * 1024;
	m_iCurCacheSize		= 0;
	m_iLoadFileCnt		= 0;
	m_iFileCacheHitCnt	= 0;
	
	memset(m_LoadCounts, 0, sizeof(m_LoadCounts));
}

CECSoundCache::~CECSoundCache()
{
}

//	Release object
void CECSoundCache::Release()
{
	//	Release all cached files
	FileTable::iterator it = m_FileTab.begin();
	for (; it != m_FileTab.end(); ++it)
	{
		FILECACHEITEM* pFileItem = *it.value();
		FreeFileCacheItem(pFileItem);
	}

	m_FileTab.clear();

	//	Remove order list
	m_OrderList.RemoveAll();

	memset(m_LoadCounts, 0, sizeof(m_LoadCounts));
}

//	Load file
AFileImage* CECSoundCache::LoadFile(const char* szFile)
{
	if (!szFile || !szFile[0])
	{
		ASSERT(szFile && szFile[0]);
		return NULL;
	}

	m_iLoadFileCnt++;

	//	Make relative file name
	AString strRelFile;
	af_GetRelativePath(szFile, strRelFile);
	strRelFile.MakeLower();

	//	Check whether this file has existed in cache
	FILECACHEITEM* pCacheItem = FindCachedFile(strRelFile);
	if (pCacheItem)
	{
		m_iFileCacheHitCnt++;

		pCacheItem->pFile->ResetPointer();
		pCacheItem->dwTimeStamp = a_GetTime();

		//	This file is newest now, so put it to order list head
		if (pCacheItem->pos)
			m_OrderList.RemoveAt(pCacheItem->pos);

		pCacheItem->pos = m_OrderList.AddHead(pCacheItem);

		return pCacheItem->pFile;
	}

	AFileImage* pFileImage = new AFileImage;
	if (!pFileImage)
	{
		glb_ErrorOutput(ECERR_NOTENOUGHMEMORY, "CECSoundCache::LoadFile", __LINE__);
		return NULL;
	}

	if (!pFileImage->Open("", szFile, AFILE_OPENEXIST))
	{
		delete pFileImage;
		a_LogOutput(1, "CECSoundCache::LoadFile, Can't load sound file [%s]", szFile);
		return NULL;
	}

	//	Try to add file to cache
	AddFileToCache(strRelFile, pFileImage);

	return pFileImage;
}

//	Close file
void CECSoundCache::CloseFile(AFileImage* pFile)
{
	ASSERT(pFile);

	AString strRelFile = pFile->GetRelativeName();
	strRelFile.MakeLower();
	
	FILECACHEITEM* pCacheItem = FindCachedFile(strRelFile);
	if (!pCacheItem)
	{
		//	This file isn't in cache, release directly
		pFile->Close();
		delete pFile;
	}
}

//	Allocate a file cache item
CECSoundCache::FILECACHEITEM* CECSoundCache::AllocFileCacheItem(AFileImage* pFile)
{
	FILECACHEITEM* pItem = new FILECACHEITEM;
	if (!pItem)
	{
		glb_ErrorOutput(ECERR_NOTENOUGHMEMORY, "CECSoundCache::AllocFileCacheItem", __LINE__);
		return false;
	}

	pItem->pFile		= pFile;
	pItem->dwTimeStamp	= a_GetTime();
	pItem->pos			= NULL;

	return pItem;
}

//	Free a file cache item
void CECSoundCache::FreeFileCacheItem(FILECACHEITEM* pFileItem)
{
	if (pFileItem)
	{	
		pFileItem->pFile->Close();
		delete pFileItem->pFile;
		delete pFileItem;
	}
}

//	Find a file in file cache
CECSoundCache::FILECACHEITEM* CECSoundCache::FindCachedFile(const AString& strRelFile)
{
	//	First try to search in hash table
	FileTable::pair_type Pair = m_FileTab.get(strRelFile);
	if (Pair.second)
		return *Pair.first;

	return NULL;
}

//	Add file image into cache
bool CECSoundCache::AddFileToCache(const AString& strRelFile, AFileImage* pFile)
{
	int iFileLen = pFile->GetFileLength();

	//	File is too large ?
	if (iFileLen > m_iMaxFileSize)
		return false;

	if (m_iCurCacheSize + iFileLen > m_iMaxCacheSize)
	{
		//	Make enough free space for new file
	//	RemoveFilesFromCache(iFileLen);
		RemoveFilesFromCache(m_iMaxFileSize * 5);
	}

	FILECACHEITEM* pItem = AllocFileCacheItem(pFile);
	if (!pItem)
		return false;

	m_FileTab.put(strRelFile, pItem);

	//	This file is newest, so put it to order list head
	pItem->pos = m_OrderList.AddHead(pItem);

	m_iCurCacheSize += iFileLen;

	return true;
}

//	Remove files from file cache to make more free space
//	Return true for success, otherwise return false
bool CECSoundCache::RemoveFilesFromCache(int iFreeSpace)
{
	int iFreeCnt = 0;
	AString strRelFile;

	//	Remove from order tail
	while (iFreeCnt < iFreeSpace && m_OrderList.GetCount())
	{
		FILECACHEITEM* pItem = m_OrderList.RemoveTail();
		iFreeCnt += pItem->pFile->GetFileLength();

		//	Don't forget delete item from file table
		strRelFile = pItem->pFile->GetRelativeName();
		strRelFile.MakeLower();
		m_FileTab.erase(strRelFile);

		//	Release file item
		FreeFileCacheItem(pItem);
	}

	m_iCurCacheSize -= iFreeCnt;

	return true;
}

AM3DSoundBuffer* CECSoundCache::LoadSound(const char* szFile, SoundLoadType enumType)
{
	if (m_LoadCounts[enumType] >= _sound_load_max_counts[enumType])
		return NULL;

	AMSoundBufferMan* pMan = g_pGame->GetA3DEngine()->GetAMSoundEngine()->GetAMSoundBufferMan();
	AM3DSoundBuffer* pBuffer = pMan->Load3DSound(szFile, false);
	if (pBuffer) m_LoadCounts[enumType]++;
	return pBuffer;
}

void CECSoundCache::ReleaseSound(AM3DSoundBuffer*& pSound, SoundLoadType enumType)
{
	m_LoadCounts[enumType]--;
	g_pGame->GetA3DEngine()->GetAMSoundEngine()->GetAMSoundBufferMan()->Release3DSound(&pSound);
}
