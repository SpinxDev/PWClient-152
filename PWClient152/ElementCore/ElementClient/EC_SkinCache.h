/*
 * FILE: EC_SkinCache.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/8/19
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include "AArray.h"
#include "AString.h"

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

class A3DSkin;
class A3DSkeleton;
class A3DSkinMan;
class A3DMuscleData;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CECSkinCache
//	
///////////////////////////////////////////////////////////////////////////

class CECSkinCache
{
public:		//	Types

	//	Skin cache node
	struct SKINNODE
	{
		DWORD		dwTimeStamp;	//	Time stamp
		int			iSize;			//	Skin size in bytes
		int			iKSize;			//	Skin size in K bytes
		A3DSkin*	pA3DSkin;
	};

	//	Muscle data node
	struct MUSCLEDATA
	{
		DWORD			dwDataID;			//	Muscle data ID made from file name
		AString			strFile;			//	Muscle data file name
		A3DMuscleData*	pA3DMuscleData;		//	A3D muscle data
	};

public:		//	Constructor and Destructor

	CECSkinCache();
	virtual ~CECSkinCache();

public:		//	Attributes

public:		//	Operations

	//	Initialize object
	bool Init(int iSkinCacheSize, int iMaxSkinSize);
	//	Release object
	void Release();

	//	Remove all cached objects
	void Reset();
	//	Reset skin cache
	void ResetSkinCache();
	//	Reset muscle cache
	void ResetMuscleCache();

	//	Add a skin to cache
	bool AddSkinToCache(A3DSkin* pA3DSkin);
	//	Load muscle data from file
	A3DMuscleData* LoadMuscleData(const char* szFile);
	
	//	Get skin cache informatino
	int GetSkinCacheTotalSize() { return m_iSkinCacheSize; }
	int GetMaxSingleSkinSize() { return m_iMaxSkinSize; }
	int GetCurSkinCacheSize() { return m_iSkinSizeCnt; }

protected:	//	Attributes

	A3DSkinMan*		m_pA3DSkinMan;	//	A3D Skin manager

	int		m_iSkinCacheSize;		//	Skin cache size
	int		m_iMaxSkinSize;			//	Maximum size of single cachable skin
	int		m_iSkinSizeCnt;			//	Skin size counter

	APtrArray<SKINNODE*>	m_SkinCache;	//	Skin cache
	APtrArray<MUSCLEDATA*>	m_MuscleCache;	//	Muscle data cache	

protected:	//	Operations

	//	Remove one skin form cache
	bool RemoveSkinFromCache(int iNeedSize, DWORD dwCurTime);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

