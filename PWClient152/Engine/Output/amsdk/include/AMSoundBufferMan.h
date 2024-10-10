/*
 * FILE: AMSoundBufferMan.h
 *
 * DESCRIPTION: sound buffer manager class
 *
 * CREATED BY: Hedi, duyuxin, 2003/10/18
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _AMSOUNDBUFFERMAN_H_
#define _AMSOUNDBUFFERMAN_H_

#include "A3DVector.h"
#include "AMTypes.h"
#include "AList2.h"
#include "AArray.h"

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

class AMSoundEngine;
class AMSoundBuffer;
class AM3DSoundBuffer;
class AFileImage;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class AMSoundFileCache
//
///////////////////////////////////////////////////////////////////////////

class AMSoundFileCache
{
public:		//	Operations

	//	Load file
	virtual AFileImage* LoadFile(const char* szFile);
	//	Close file
	virtual void CloseFile(AFileImage* pFile);

protected:	//	Attributes

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//
//	Class AMSoundBufferMan
//
///////////////////////////////////////////////////////////////////////////

class AMSoundBufferMan
{
public:		//	Types

	//	Sound flag used by PauseSounds()
	enum
	{
		SND_2DAUTO		= 0x0001,
		SND_3DAUTO		= 0x0002,
		SND_2DLOADED	= 0x0004,
		SND_3DLOADED	= 0x0008,
		SND_ALL			= 0x000f,
	};

	//	Structure of node in loaded 3D sound buffer list
	struct LOADED3D
	{
		AM3DSoundBuffer*	pSoundBuf;
		bool				bNeedUpdate;	//	Need update flag
	};

	//	Auto sound buffer
	struct AUTOBUFFER
	{
		AMSoundBuffer*	pSoundBuffer;	//	Sound buffer object
		DWORD			dwTimeStamp;	//	Time stamp
	};

	//	Auto 3D sound group
	struct AUTO3DGROUP
	{
		AUTOBUFFER*		a3DSounds;		//	3D sound buffers
		int				iNumBuf;		//	Number of buffer
	};

	//	Auto sound cache item
	struct ASCACHEITEM
	{
		DWORD			dwFileID;		//	File ID
		bool			b3DSound;		//	true, 3D sound
		AMSoundBuffer**	aSoundBuffers;	//	Sound buffer objects
		int				iMaxNumBuffer;	//	Maximum sound buffer number
		int				iNumBuffer;		//	Current number of sound buffer
	};

public:		//	Constructors and Destructors

	AMSoundBufferMan();
	virtual ~AMSoundBufferMan();

public:		//	Attributes

public:		//	Operations

	//	Initialize object
	bool Init(AMSoundEngine* pAMSoundEngine);
	//	Release object
	void Release();
	//	Reload all sound object
	bool ReloadAll();	

	//	Reset manager
	bool Reset();
	//	Tick routine
	bool Tick();

	//	Add auto sound files to sound cache
	bool AddCachedAutoSounds(const char* szScriptFile);
	//	Release all cached auto sounds
	void ReleaseCachedAutoSounds();

	//	Pause sounds
	void PauseSounds(bool bPause, DWORD dwSndFlags);
	//	Stop sounds
	void StopSounds(DWORD dwSndFlags);

	//	Load 2D sound frmo file
	AMSoundBuffer* Load2DSound(const char* szFile);
	//	Release 2D sound file created by Load2DSound()
	void Release2DSound(AMSoundBuffer** ppSoundBuffer);
	//	Play 2D auto sound
	bool Play2DAutoSound(const char* szFile, bool bImmEffect=false);
	//	Get loaded 2D sound buffer number
	int GetLoaded2DSoundBufferNum() { return m_2DLoadedList.GetCount(); }

	//	Load 3D sound frmo file
	AM3DSoundBuffer* Load3DSound(const char* szFile, bool bUpdate);
	//	Release 3D sound file created by Load3DSound()
	void Release3DSound(AM3DSoundBuffer** ppSoundBuffer);
	//	Play 3D auto sound
	bool Play3DAutoSound(int iGroup, const char* szFile, const A3DVECTOR3& vPos, float fMinDist, float fMaxDist, bool bImmEffect=false);
	//	Get loaded 3D sound buffer number
	int GetLoaded3DSoundBufferNum() { return m_3DLoadedList.GetCount(); }

	//	Set all active AMSoundBuffers' volume directly by the input volume
	//	AMSoundBuffer_Volume = Volume
	bool SetVolume(DWORD dwVolume);
	//	Set all AMSoundBuffers' volume with Calculating the actual volume by AMSoundBuffer's RelativeVolume
	//	The final volume is calculated as below: 
	//	AMSoundBuffer_Volume = AMSoundBufferMan_Volume * AMSoundBuffer_RelativeVolume / 100
	bool SetRelativeVolume(DWORD dwVolume);
	//	Set play speed
	bool SetSFXPlaySpeed(float fSFXPlaySpeed);
	
	//	Set user file cache
	void SetUserFileCache(AMSoundFileCache* pCache) { m_pUserCache = pCache; }
	//	Get user file cache
	AMSoundFileCache* GetUserFileCache() { return m_pUserCache; }
	//	Get current cached auto sound size
	int GetCachedAutoSoundSize() { return m_iCachedAutoSoundSize; }

protected:	//	Attributes

	AMSoundEngine*		m_pAMSEngine;	//	AM sound engine
	DWORD				m_dwLastTick;	//	Last tick's time
	AMSoundFileCache*	m_pUserCache;	//	User file cache
	AMSoundFileCache	m_DefCache;		//	Default file cache

	int		m_iCachedAutoSoundSize;		//	Current cached auto sound size

	APtrList<AMSoundBuffer*>	m_2DLoadedList;		//	2D loaded sound list
	APtrList<LOADED3D*>			m_3DLoadedList;		//	3D loaded sound list

	AArray<AUTOBUFFER, AUTOBUFFER&>		m_aAuto2DSounds;	//	Auto 2D sounds
	AArray<AUTO3DGROUP, AUTO3DGROUP&>	m_aAuto3DGroups;	//	Auto 3D sound groups
	APtrArray<ASCACHEITEM*>				m_aCachedAutoSnds;	//	Cached auto sounds

public:
	const APtrList<LOADED3D *>& GetLoaded3DList() { return m_3DLoadedList; }
	const APtrList<AMSoundBuffer*>& GetLoaded2DList() { return m_2DLoadedList; }

protected:	//	Operations

	//	Allocate a auto sound cache item
	ASCACHEITEM* AllocASCacheItem(bool b3D, int iMaxNumSnd, int iNumSnd, const char* szFile);
	//	Free a auto sound cache item
	void FreeASCacheItem(ASCACHEITEM* pItem);
	//	Load a sound file image
	AFileImage* LoadSoundFileImage(const char* szFile, bool* pbCached);
	//	Release all auto sound buffers
	void ReleaseAllAutoBuffers();
	//	Load sound from file
	AMSoundBuffer* LoadSoundFromFile(const char* szFile, bool b3DSound);
	//	Load auto sound from file
	AMSoundBuffer* LoadAutoSoundFromFile(const char* szFile, bool b3DSound);
	//	Return a auto sound to cache
	void ReturnAutoSoundToCache(AMSoundBuffer* pSoundBuffer);

	//	Get current file cache
	AMSoundFileCache* GetCurFileCache() { return m_pUserCache ? m_pUserCache : &m_DefCache; }
};

#endif	//	_AMSOUNDBUFFERMAN_H_

