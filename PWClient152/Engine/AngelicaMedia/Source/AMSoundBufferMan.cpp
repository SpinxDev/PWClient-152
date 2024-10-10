/*
 * FILE: AMSoundBufferMan.cpp
 *
 * DESCRIPTION: sound buffer manager class
 *
 * CREATED BY: Hedi, duyuxin, 2003/10/18
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "AMSoundBufferMan.h"
#include "AMPI.h"
#include "AMConfig.h"
#include "AMSoundEngine.h"
#include "AM3DSoundBuffer.h"
#include "AMSoundBuffer.h"
#include "AMEngine.h"
#include "AMemory.h"
#include "AScriptFile.h"
#include "AFI.h"
#include "AFileImage.h"
#include "ATime.h"

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


///////////////////////////////////////////////////////////////////////////
//
//	Local functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Implement AMSoundFileCache
//
///////////////////////////////////////////////////////////////////////////

//	Load file
AFileImage* AMSoundFileCache::LoadFile(const char* szFile)
{
	AFileImage* pFileImage = new AFileImage;
	if (!pFileImage)
	{
		AMERRLOG(("AMSoundFileCache::LoadFile, Not enough memory!"));
		return NULL;
	}

	if (!pFileImage->Open("", szFile, AFILE_OPENEXIST))
	{
		delete pFileImage;
		AMERRLOG(("AMSoundFileCache::LoadFile, Can't load sound file [%s]", szFile));
		return NULL;
	}

	return pFileImage;
}

//	Close file
void AMSoundFileCache::CloseFile(AFileImage* pFile)
{
	if (pFile)
	{
		pFile->Close();
		delete pFile;
	}
}

///////////////////////////////////////////////////////////////////////////
//
//	Implement AMSoundBufferMan
//
///////////////////////////////////////////////////////////////////////////

AMSoundBufferMan::AMSoundBufferMan() : m_aCachedAutoSnds(0, 50)
{
	m_pAMSEngine			= NULL;
	m_iCachedAutoSoundSize	= 0;
	m_pUserCache			= NULL;
}

AMSoundBufferMan::~AMSoundBufferMan()
{
}

//	Initialize object
bool AMSoundBufferMan::Init(AMSoundEngine* pAMSoundEngine)
{
	m_pAMSEngine = pAMSoundEngine;
	AMConfig* pAMConfig = pAMSoundEngine->GetAMEngine()->GetAMConfig();

	m_aAuto2DSounds.SetSize(pAMConfig->GetMax2DSoundBuf(), 10);
	m_aAuto3DGroups.SetSize(pAMConfig->Get3DSndGroupNum(), 10);

	int i;
	int* aMax3DBufNums = pAMConfig->GetAll3DSndGroupSize();

	//	Initialize auto 2D sound buffer
	for (i=0; i < m_aAuto2DSounds.GetSize(); i++)
	{
		m_aAuto2DSounds[i].pSoundBuffer = NULL;
		m_aAuto2DSounds[i].dwTimeStamp	= 0;
	}

	//	Initialize auto 3D sound groups
	for (i=0; i < m_aAuto3DGroups.GetSize(); i++)
	{
		int iNumBuf = aMax3DBufNums[i];

		m_aAuto3DGroups[i].a3DSounds = new AUTOBUFFER[iNumBuf];
		if (!m_aAuto3DGroups[i].a3DSounds)
		{
			AMERRLOG(("AMSoundBufferMan::Init, Not enough memory !"));
			return false;
		}

		memset(m_aAuto3DGroups[i].a3DSounds, 0, iNumBuf * sizeof (AUTOBUFFER));

		m_aAuto3DGroups[i].iNumBuf = iNumBuf;
	}

	m_dwLastTick			= a_GetTime();
	m_iCachedAutoSoundSize	= 0;

	return true;
}

//	Release object
void AMSoundBufferMan::Release()
{
	//	All loaded sound buffer should be released before manager is released
	ASSERT(!m_2DLoadedList.GetCount());
	ASSERT(!m_3DLoadedList.GetCount());

	//	Release all auto buffers
	ReleaseAllAutoBuffers();

	//	Release all cached auto sounds
	ReleaseCachedAutoSounds();

	for (int i=0; i < m_aAuto3DGroups.GetSize(); i++)
		delete [] m_aAuto3DGroups[i].a3DSounds;

	m_aAuto2DSounds.RemoveAll();
	m_aAuto3DGroups.RemoveAll();
}

//	Reset manager
bool AMSoundBufferMan::Reset()
{
	//	All loaded sound buffer should be released before manager is reset
	ASSERT(!m_2DLoadedList.GetCount());
	ASSERT(!m_3DLoadedList.GetCount());

	//	Release all auto buffers
	ReleaseAllAutoBuffers();

	//	Release all cached auto sounds
	ReleaseCachedAutoSounds();

	return true;
}

//	Release all auto sound buffers
void AMSoundBufferMan::ReleaseAllAutoBuffers()
{
	int i;

	//	Release auto 2D buffers
	for (i=0; i < m_aAuto2DSounds.GetSize(); i++)
	{
		AMSoundBuffer* pSoundBuffer = m_aAuto2DSounds[i].pSoundBuffer;
		ReturnAutoSoundToCache(pSoundBuffer);
		m_aAuto2DSounds[i].pSoundBuffer = NULL;
	}

	//	Release auto 3D buffers
	for (i=0; i < m_aAuto3DGroups.GetSize(); i++)
	{
		AUTO3DGROUP& Group = m_aAuto3DGroups[i];
		if (Group.a3DSounds)
		{
			for (int j=0; j < Group.iNumBuf; j++)
			{
				AM3DSoundBuffer* pSoundBuffer = (AM3DSoundBuffer*)Group.a3DSounds[j].pSoundBuffer;
				if (pSoundBuffer)
					ReturnAutoSoundToCache(pSoundBuffer);

				Group.a3DSounds[j].pSoundBuffer	= NULL;
				Group.a3DSounds[j].dwTimeStamp	= 0;
			}
		}
	}
}

//	Load 2D sound frmo file
AMSoundBuffer* AMSoundBufferMan::Load2DSound(const char* szFile)
{
	ASSERT(m_pAMSEngine);

	AMSoundBuffer* pSoundBuffer = LoadSoundFromFile(szFile, false);
	if (!pSoundBuffer)
		return NULL;

	pSoundBuffer->m_dwPosInMan = (DWORD)m_2DLoadedList.AddTail(pSoundBuffer);

	return pSoundBuffer;
}

//	Release 2D sound file created by Load2DSound()
void AMSoundBufferMan::Release2DSound(AMSoundBuffer** ppSoundBuffer)
{
	AMSoundBuffer* pSoundBuf = *ppSoundBuffer;
	if (!pSoundBuf || !pSoundBuf->m_dwPosInMan)
		return;

	m_2DLoadedList.RemoveAt((ALISTPOSITION)pSoundBuf->m_dwPosInMan);

	pSoundBuf->Release();
	delete pSoundBuf;
	*ppSoundBuffer = NULL;
}

//	Load 3D sound from file
AM3DSoundBuffer* AMSoundBufferMan::Load3DSound(const char* szFile, bool bUpdate)
{
	ASSERT(m_pAMSEngine);

	AM3DSoundBuffer* pSoundBuffer = (AM3DSoundBuffer*)LoadSoundFromFile(szFile, true);
	if (!pSoundBuffer)
		return NULL;

	//	Add to list
	LOADED3D* pNode = new LOADED3D;

	pNode->pSoundBuf	= pSoundBuffer;
	pNode->bNeedUpdate	= bUpdate;

	pSoundBuffer->m_dwPosInMan = (DWORD)m_3DLoadedList.AddTail(pNode);

	return pSoundBuffer;
}

//	Release 3D sound file created by Load3DSound()
void AMSoundBufferMan::Release3DSound(AM3DSoundBuffer** ppSoundBuffer)
{
	AM3DSoundBuffer* pSoundBuf = *ppSoundBuffer;
	if (!pSoundBuf || !pSoundBuf->m_dwPosInMan)
		return;

	//	Remove from list
	ALISTPOSITION pos = (ALISTPOSITION)pSoundBuf->m_dwPosInMan;
	delete m_3DLoadedList.GetAt(pos);
	m_3DLoadedList.RemoveAt(pos);

	//	Release sound buffer
	pSoundBuf->Release();
	delete pSoundBuf;
	*ppSoundBuffer = NULL;
}

//	Load 2D sound from file
AMSoundBuffer* AMSoundBufferMan::LoadSoundFromFile(const char* szFile, bool b3DSound)
{
	//	Create a sound buffer object here
	AMSoundBuffer* pSoundBuffer = NULL;
	
	if (b3DSound)
		pSoundBuffer = new AM3DSoundBuffer;
	else
		pSoundBuffer = new AMSoundBuffer;

	if (!pSoundBuffer)
	{
		AMERRLOG(("AMSoundBufferMan::LoadSoundFromFile, Not enough memory!"));
		return false;
	}

	AMConfig* pAMConfig = m_pAMSEngine->GetAMEngine()->GetAMConfig();
	if (pAMConfig->GetRunEnv() == AMRUNENV_PURESERVER)
	{
		if (!pSoundBuffer->Load(m_pAMSEngine, NULL, (char*)szFile))
		{
			delete pSoundBuffer;
			AMERRLOG(("AMSoundBufferMan::LoadSoundFromFile, The file [%s] loaded failure!", szFile));
			return NULL;
		}

		return pSoundBuffer;
	}

	//	Load sound file
	AMSoundFileCache* pFileCache = GetCurFileCache();
	AFileImage* pFile = pFileCache->LoadFile(szFile);

	if (!pFile)
	{
		delete pSoundBuffer;
		AMERRLOG(("AMSoundBufferMan::LoadSoundFromFile, Cannot open file %s!", szFile));
		return false;
	}

	if (!pSoundBuffer->Load(m_pAMSEngine, pFile, (char*)szFile))
	{
		delete pSoundBuffer;
		AMERRLOG(("AMSoundBufferMan::LoadSoundFromFile, The file [%s] loaded failure!", szFile));
		return false;
	}

	pFileCache->CloseFile(pFile);

	return pSoundBuffer;
}

//	Play 2D auto sound
bool AMSoundBufferMan::Play2DAutoSound(const char* szFile, bool bImmEffect/* false */)
{
	int i, iReplace = 0;
	DWORD dwMinTime = 0xffffffff;

	//	Search a empty slot
	for (i=0; i < m_aAuto2DSounds.GetSize(); i++)
	{
		AUTOBUFFER& Buf = m_aAuto2DSounds[i];
		if (!Buf.pSoundBuffer)
		{
			iReplace = i;
			break;
		}

		if (Buf.dwTimeStamp < dwMinTime)
		{
			iReplace	= i;
			dwMinTime	= Buf.dwTimeStamp;
		}
	}

	AUTOBUFFER& Buf = m_aAuto2DSounds[iReplace];

	if (Buf.pSoundBuffer)
	{
		//	Return sound buffer to cache
		ReturnAutoSoundToCache(Buf.pSoundBuffer);
	}

	if (!(Buf.pSoundBuffer = LoadAutoSoundFromFile(szFile, false)))
		return false;

	Buf.dwTimeStamp = a_GetTime();

	Buf.pSoundBuffer->Play(false);

	return true;
}

//	Play 3D auto sound
bool AMSoundBufferMan::Play3DAutoSound(int iGroup, const char* szFile, const A3DVECTOR3& vPos,
									   float fMinDist, float fMaxDist, bool bImmEffect/* false */)
{
	AUTO3DGROUP& Group = m_aAuto3DGroups[iGroup];
	if (!Group.a3DSounds)
		return false;

	int i, iReplace = 0;
	DWORD dwMinTime = 0xffffffff;

	//	Search a empty slot
	for (i=0; i < Group.iNumBuf; i++)
	{
		AUTOBUFFER* pBuf = &Group.a3DSounds[i];
		if (!pBuf->pSoundBuffer)
		{
			iReplace = i;
			break;
		}

		if (pBuf->dwTimeStamp < dwMinTime)
		{
			iReplace	= i;
			dwMinTime	= pBuf->dwTimeStamp;
		}
	}

	AUTOBUFFER* pBuf = &Group.a3DSounds[iReplace];

	if (pBuf->pSoundBuffer)
	{
		//	Return sound buffer to cache
		ReturnAutoSoundToCache(pBuf->pSoundBuffer);
	}

	if (!(pBuf->pSoundBuffer = LoadAutoSoundFromFile(szFile, true)))
		return false;

	pBuf->dwTimeStamp = a_GetTime();

	AM3DSoundBuffer* pSoundBuffer = (AM3DSoundBuffer*)pBuf->pSoundBuffer;

	pSoundBuffer->SetPosition(vPos);
	pSoundBuffer->SetMinDistance(fMinDist);
	pSoundBuffer->SetMaxDistance(fMaxDist);
	pSoundBuffer->UpdateChanges();

	pSoundBuffer->Play(false);

	return true;
}

/*	Add auto sound files to sound cache

	szScriptFile: script file which should have formats as below example

	*************** Text file *******************

	//	Is3D: 1，3D音效；0，2D音效
	//	MaxSnd: 该声音对象最多缓冲的个数
	//	NumSnd: 预先加载该声音对象的个数，<= MaxSnd
	//	File: 声音文件相对路径

	//	Is3D	MaxSnd	NumSnd	File

		0		4		0		"Sounds\Bulletway04.wav"
		0		4		2		"Sounds\Bulletway09.wav"
		1		3		0		"sfx\BulletHitG3st1.wav"
		1		3		1		"sfx\VietNamRush_04.wav"

	*************** Text file *******************
*/
bool AMSoundBufferMan::AddCachedAutoSounds(const char* szScriptFile)
{
	AScriptFile ScriptFile;
	if (!ScriptFile.Open(szScriptFile))
	{
		AMERRLOG(("AMSoundBufferMan::AddCachedAutoSounds, Failed to open file %s !", szScriptFile));
		return false;
	}

	while (ScriptFile.PeekNextToken(true))
	{
		//	Is 3D sound ?
		bool b3D = ScriptFile.GetNextTokenAsInt(true) ? true : false;

		//	Get maximum number of cached sound buffer
		int iMaxNumSnd = ScriptFile.GetNextTokenAsInt(false);

		//	Get preloaded sound buffer number
		int iNumSnd = ScriptFile.GetNextTokenAsInt(false);

		//	Get sound file name
		ScriptFile.GetNextToken(false);

		//	Allocate sound cache item
		ASCACHEITEM* pItem = AllocASCacheItem(b3D, iMaxNumSnd, iNumSnd, ScriptFile.m_szToken);
		if (pItem)
			m_aCachedAutoSnds.Add(pItem);
		
		ScriptFile.SkipLine();
	}

	ScriptFile.Close();

	return true;
}

//	Allocate a auto sound cache item
AMSoundBufferMan::ASCACHEITEM* AMSoundBufferMan::AllocASCacheItem(bool b3D, int iMaxNumSnd, int iNumSnd, const char* szFile)
{
	ASCACHEITEM* pItem = new ASCACHEITEM;
	if (!pItem)
	{
		AMERRLOG(("AMSoundBufferMan::AllocASCacheItem, Not enough memory !"));
		return NULL;
	}

	ASSERT(iMaxNumSnd >= 1);

	typedef AMSoundBuffer* AMSoundBufferPtr;

	if (!(pItem->aSoundBuffers = new AMSoundBufferPtr[iMaxNumSnd]))
	{
		delete pItem;
		AMERRLOG(("AMSoundBufferMan::AllocASCacheItem, Not enough memory !"));
		return NULL;
	}

	memset(pItem->aSoundBuffers, 0, sizeof (AMSoundBuffer*) * iMaxNumSnd);

	pItem->dwFileID			= a_MakeIDFromLowString(szFile);
	pItem->iMaxNumBuffer	= iMaxNumSnd;
	pItem->b3DSound			= b3D;
	pItem->iNumBuffer		= 0;

	//	Number of preload sound buffer
	a_Clamp(iNumSnd, 0, iMaxNumSnd);

	if (iNumSnd)
	{
		//	Preload sound buffers
		for (int i=0; i < iNumSnd; i++)
		{
			if (!(pItem->aSoundBuffers[i] = LoadSoundFromFile(szFile, b3D)))
				break;

			m_iCachedAutoSoundSize += (int)pItem->aSoundBuffers[i]->GetBufferDesc()->dwBufferBytes;
		}

		pItem->iNumBuffer = iNumSnd;
	}

	return pItem;
}

//	Free a auto sound cache item
void AMSoundBufferMan::FreeASCacheItem(ASCACHEITEM* pItem)
{
	if (!pItem)
		return;

	for (int i=0; i < pItem->iNumBuffer; i++)
	{
		AMSoundBuffer* pSoundBuf = pItem->aSoundBuffers[i];
		if (pSoundBuf)
		{
			m_iCachedAutoSoundSize -= (int)pSoundBuf->GetBufferDesc()->dwBufferBytes;
			pSoundBuf->Release();
			delete pSoundBuf;
		}
	}

	delete [] pItem->aSoundBuffers;

	delete pItem;
}

//	Release all cached auto sounds
void AMSoundBufferMan::ReleaseCachedAutoSounds()
{
	for (int i=0; i < m_aCachedAutoSnds.GetSize(); i++)
	{
		FreeASCacheItem(m_aCachedAutoSnds[i]);
	}

	m_aCachedAutoSnds.RemoveAll();

	m_iCachedAutoSoundSize = 0;
}

//	Load auto sound from file
AMSoundBuffer* AMSoundBufferMan::LoadAutoSoundFromFile(const char* szFile, bool b3DSound)
{
	int i, iItem;
	DWORD dwFileID = a_MakeIDFromLowString(szFile);

	for (i=0; i < m_aCachedAutoSnds.GetSize(); i++)
	{
		ASCACHEITEM* pItem = m_aCachedAutoSnds[i];
		if (pItem->dwFileID == dwFileID && pItem->b3DSound == b3DSound)
			break;
	}

	if (i >= m_aCachedAutoSnds.GetSize())
		return LoadSoundFromFile(szFile, b3DSound);
	
	iItem = i;
	ASCACHEITEM* pItem = m_aCachedAutoSnds[iItem];

	//	Try to find a idle sound buffer object
	for (i=0; i < pItem->iNumBuffer; i++)
	{
		AMSoundBuffer* pSound = pItem->aSoundBuffers[i];
		if (!pSound->m_dwPosInMan)
		{
			pSound->m_dwPosInMan = iItem + 1;
			return pSound;
		}
	}

	AMSoundBuffer* pSound = LoadSoundFromFile(szFile, b3DSound);
	if (!pSound)
		return NULL;

	if (pItem->iNumBuffer < pItem->iMaxNumBuffer)
	{
		pSound->m_dwPosInMan = iItem + 1;
		pItem->aSoundBuffers[pItem->iNumBuffer] = pSound;
		pItem->iNumBuffer++;

		m_iCachedAutoSoundSize += (int)pSound->GetBufferDesc()->dwBufferBytes;
	}

	return pSound;
}

//	Return a auto sound to cache
void AMSoundBufferMan::ReturnAutoSoundToCache(AMSoundBuffer* pSoundBuffer)
{
	if (!pSoundBuffer)
		return;

	if (!pSoundBuffer->m_dwPosInMan)
	{
		//	This sound buffer doesn't belong to cache.
		pSoundBuffer->Release();
		delete pSoundBuffer;
		return;
	}

	ASSERT((int)pSoundBuffer->m_dwPosInMan > 0);

	pSoundBuffer->Stop();
	pSoundBuffer->m_dwPosInMan = 0;
}

//	Tick routine
bool AMSoundBufferMan::Tick()
{
	AMConfig* pAMConfig = m_pAMSEngine->GetAMEngine()->GetAMConfig();

	int	i;

	//	Check 2D auto sound buffers
	for (i=0; i < m_aAuto2DSounds.GetSize(); i++)
	{
		AUTOBUFFER& Buf = m_aAuto2DSounds[i];
		if (!Buf.pSoundBuffer)
			continue;

		if (!Buf.pSoundBuffer->CheckEnd())
		{
			AMERRLOG(("AMSoundBufferMan::Tick(), Error check one buffer's end!"));
			return false;
		}

		if (Buf.pSoundBuffer->IsStopped())
		{
			//	Return sound buffer to cache
			ReturnAutoSoundToCache(Buf.pSoundBuffer);
			Buf.pSoundBuffer = NULL;
		}
	}

	//	Check 3D auto sound buffers
	for (i=0; i < m_aAuto3DGroups.GetSize(); i++)
	{
		AUTO3DGROUP& Group = m_aAuto3DGroups[i];
		if (!Group.a3DSounds)
			continue;

		for (int j=0; j < Group.iNumBuf; j++)
		{
			AM3DSoundBuffer* pSoundBuffer = (AM3DSoundBuffer*)Group.a3DSounds[j].pSoundBuffer;
			if (!pSoundBuffer)
				continue;

			if (!pSoundBuffer->CheckEnd())
			{
				AMERRLOG(("AMSoundBufferMan::Tick(), Error check one buffer's end!"));
				return false;
			}

			if (pSoundBuffer->IsStopped())
			{
				//	Return sound buffer to cache
				ReturnAutoSoundToCache(pSoundBuffer);
				Group.a3DSounds[j].pSoundBuffer	= NULL;
			}
		}
	}

	//	It's time to update 3D sounds ?
	DWORD dwTime = a_GetTime();
	if (dwTime >= m_dwLastTick + pAMConfig->Get3DSoundUpdateInterval())
	{
		m_dwLastTick = dwTime;

		//	Update 3D sounds
		ALISTPOSITION pos = m_3DLoadedList.GetHeadPosition();
		while (pos)
		{
			LOADED3D* pNode = m_3DLoadedList.GetNext(pos);
			if (pNode->bNeedUpdate)
				pNode->pSoundBuf->UpdateChanges();
		}
	}

	return true;
}

//	Pause sounds
void AMSoundBufferMan::PauseSounds(bool bPause, DWORD dwSndFlags)
{
	int i;

	if (bPause)
	{
		if (dwSndFlags & SND_2DAUTO)
		{
			for (i=0; i < m_aAuto2DSounds.GetSize(); i++)
			{
				AUTOBUFFER& Buf = m_aAuto2DSounds[i];
				if (!Buf.pSoundBuffer || Buf.pSoundBuffer->IsPaused())
					continue;

				Buf.pSoundBuffer->Pause();
			}
		}

		if (dwSndFlags & SND_3DAUTO)
		{
			for (i=0; i < m_aAuto3DGroups.GetSize(); i++)
			{
				AUTO3DGROUP& Group = m_aAuto3DGroups[i];
				if (!Group.a3DSounds)
					continue;

				for (int j=0; j < Group.iNumBuf; j++)
				{
					AM3DSoundBuffer* pSoundBuffer = (AM3DSoundBuffer*)Group.a3DSounds[j].pSoundBuffer;
					if (!pSoundBuffer || pSoundBuffer->IsPaused())
						continue;

					pSoundBuffer->Pause();
				}
			}
		}

		if (dwSndFlags & SND_2DLOADED)
		{
			ALISTPOSITION pos = m_2DLoadedList.GetHeadPosition();
			while (pos)
			{
				AMSoundBuffer* pSoundBuffer = m_2DLoadedList.GetNext(pos);
				if (!pSoundBuffer->IsPaused())
					pSoundBuffer->Pause();
			}
		}

		if (dwSndFlags & SND_3DLOADED)
		{
			ALISTPOSITION pos = m_3DLoadedList.GetHeadPosition();
			while (pos)
			{
				LOADED3D* pNode = m_3DLoadedList.GetNext(pos);
				if (!pNode->pSoundBuf->IsPaused())
					pNode->pSoundBuf->Pause();
			}
		}
	}
	else	//	Continue play
	{
		if (dwSndFlags & SND_2DAUTO)
		{
			for (i=0; i < m_aAuto2DSounds.GetSize(); i++)
			{
				AUTOBUFFER& Buf = m_aAuto2DSounds[i];
				if (!Buf.pSoundBuffer || !Buf.pSoundBuffer->IsPaused())
					continue;

				Buf.pSoundBuffer->Play(false);
			}
		}

		if (dwSndFlags & SND_3DAUTO)
		{
			for (i=0; i < m_aAuto3DGroups.GetSize(); i++)
			{
				AUTO3DGROUP& Group = m_aAuto3DGroups[i];
				if (!Group.a3DSounds)
					continue;

				for (int j=0; j < Group.iNumBuf; j++)
				{
					AM3DSoundBuffer* pSoundBuffer = (AM3DSoundBuffer*)Group.a3DSounds[j].pSoundBuffer;
					if (!pSoundBuffer || !pSoundBuffer->IsPaused())
						continue;

					pSoundBuffer->Play(false);
				}
			}
		}

		if (dwSndFlags & SND_2DLOADED)
		{
			ALISTPOSITION pos = m_2DLoadedList.GetHeadPosition();
			while (pos)
			{
				AMSoundBuffer* pSoundBuffer = m_2DLoadedList.GetNext(pos);
				if (pSoundBuffer->IsPaused())
					pSoundBuffer->Play(pSoundBuffer->IsLooping());
			}
		}

		if (dwSndFlags & SND_3DLOADED)
		{
			ALISTPOSITION pos = m_3DLoadedList.GetHeadPosition();
			while (pos)
			{
				LOADED3D* pNode = m_3DLoadedList.GetNext(pos);
				if (pNode->pSoundBuf->IsPaused())
					pNode->pSoundBuf->Play(pNode->pSoundBuf->IsLooping());
			}
		}
	}
}

//	Stop sounds
void AMSoundBufferMan::StopSounds(DWORD dwSndFlags)
{
	int i;

	if (dwSndFlags & SND_2DAUTO)
	{
		for (i=0; i < m_aAuto2DSounds.GetSize(); i++)
		{
			AUTOBUFFER& Buf = m_aAuto2DSounds[i];
			if (Buf.pSoundBuffer)
				Buf.pSoundBuffer->Stop();
		}
	}

	if (dwSndFlags & SND_3DAUTO)
	{
		for (i=0; i < m_aAuto3DGroups.GetSize(); i++)
		{
			AUTO3DGROUP& Group = m_aAuto3DGroups[i];
			if (!Group.a3DSounds)
				continue;

			for (int j=0; j < Group.iNumBuf; j++)
			{
				AM3DSoundBuffer* pSoundBuffer = (AM3DSoundBuffer*)Group.a3DSounds[j].pSoundBuffer;
				if (pSoundBuffer)
					pSoundBuffer->Stop();
			}
		}
	}

	if (dwSndFlags & SND_2DLOADED)
	{
		ALISTPOSITION pos = m_2DLoadedList.GetHeadPosition();
		while (pos)
		{
			AMSoundBuffer* pSoundBuffer = m_2DLoadedList.GetNext(pos);
			pSoundBuffer->Stop();
		}
	}

	if (dwSndFlags & SND_3DLOADED)
	{
		ALISTPOSITION pos = m_3DLoadedList.GetHeadPosition();
		while (pos)
		{
			LOADED3D* pNode = m_3DLoadedList.GetNext(pos);
			pNode->pSoundBuf->Stop();
		}
	}
}

//	Set volume
bool AMSoundBufferMan::SetVolume(DWORD dwVolume)
{
	ALISTPOSITION pos = m_2DLoadedList.GetHeadPosition();
	while (pos)
	{
		AMSoundBuffer* pSoundBuffer = m_2DLoadedList.GetNext(pos);

		if( !pSoundBuffer->IsFadeIn() && !pSoundBuffer->IsFadeOut() )
			pSoundBuffer->SetVolume(dwVolume);
	}
	
	pos = m_3DLoadedList.GetHeadPosition();
	while (pos)
	{
		LOADED3D* pNode = m_3DLoadedList.GetNext(pos);

		if( !pNode->pSoundBuf->IsFadeIn() && !pNode->pSoundBuf->IsFadeOut() )
			pNode->pSoundBuf->SetVolume(dwVolume);
	}

	return true;
}

bool AMSoundBufferMan::SetRelativeVolume(DWORD dwVolume)
{
	ALISTPOSITION pos = m_2DLoadedList.GetHeadPosition();
	while (pos)
	{
		AMSoundBuffer* pSoundBuffer = m_2DLoadedList.GetNext(pos);

		if( !pSoundBuffer->IsFadeIn() && !pSoundBuffer->IsFadeOut() )
			pSoundBuffer->SetVolume(dwVolume * pSoundBuffer->GetRelativeVolume() / 100);
	}
	
	pos = m_3DLoadedList.GetHeadPosition();
	while (pos)
	{
		LOADED3D* pNode = m_3DLoadedList.GetNext(pos);

		if( !pNode->pSoundBuf->IsFadeIn() && !pNode->pSoundBuf->IsFadeOut() )
			pNode->pSoundBuf->SetVolume(dwVolume * pNode->pSoundBuf->GetRelativeVolume() / 100);
	}

	return true;
}

//	Set SFX play speed
bool AMSoundBufferMan::SetSFXPlaySpeed(float fSFXPlaySpeed)
{
	ALISTPOSITION pos = m_2DLoadedList.GetHeadPosition();
	while (pos)
	{
		AMSoundBuffer* pSoundBuffer = m_2DLoadedList.GetNext(pos);
		pSoundBuffer->SetPlaySpeed(fSFXPlaySpeed);
	}

	pos = m_3DLoadedList.GetHeadPosition();
	while (pos)
	{
		LOADED3D* pNode = m_3DLoadedList.GetNext(pos);
		pNode->pSoundBuf->SetPlaySpeed(fSFXPlaySpeed);
	}

	return true;
}

bool AMSoundBufferMan::ReloadAll()
{
	AMSoundFileCache* pFileCache = GetCurFileCache();

	ALISTPOSITION pos = m_2DLoadedList.GetHeadPosition();
	while (pos)
	{
		AMSoundBuffer* pSoundBuffer = m_2DLoadedList.GetNext(pos);
		AFileImage* pFile = pFileCache->LoadFile(pSoundBuffer->GetFileName());
		if (!pFile)
			continue;
		
		pSoundBuffer->Reload(pFile);
		pFileCache->CloseFile(pFile);
	}

	pos = m_3DLoadedList.GetHeadPosition();
	while (pos)
	{
		LOADED3D* pNode = m_3DLoadedList.GetNext(pos);
		AFileImage* pFile = pFileCache->LoadFile(pNode->pSoundBuf->GetFileName());
		if (!pFile)
			continue;

		pNode->pSoundBuf->Reload(pFile);
		pFileCache->CloseFile(pFile);
	}

	for (int i=0; i < m_aCachedAutoSnds.GetSize(); i++)
	{
		ASCACHEITEM * pItem = m_aCachedAutoSnds[i];
		int iSize = pItem->iNumBuffer;
		for(int j=0; j<iSize; j++)
		{
			if( pItem->aSoundBuffers[j] )
			{
				AFileImage* pFile = pFileCache->LoadFile(pItem->aSoundBuffers[i]->GetFileName());
				if (!pFile)
					continue;
				pItem->aSoundBuffers[j]->Reload(pFile);
				pFileCache->CloseFile(pFile);
			}
		}
	}


	return true;
}