/*
 * FILE: EC_Cache.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/8/19
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "EC_Global.h"
#include "EC_SkinCache.h"
#include "EC_Game.h"

#include "A3DSkin.h"
#include "A3DSkinMan.h"
#include "A3DEngine.h"
#include "A3DMuscleMesh.h"
#include "A3DMuscleOrgan.h"
#include "A3DMacros.h"
#include "AAssist.h"
#include "ATime.h"

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
//	Implement CECSkinCache
//	
///////////////////////////////////////////////////////////////////////////

CECSkinCache::CECSkinCache() : m_SkinCache(0, 100)
{
	m_pA3DSkinMan		= NULL;
	m_iSkinCacheSize	= 10 * 1024 * 1024;
	m_iMaxSkinSize		= 500 * 1024;
	m_iSkinSizeCnt		= 0;
}

CECSkinCache::~CECSkinCache()
{
	CECSkinCache::Release();
}

//	Initialize object
bool CECSkinCache::Init(int iSkinCacheSize, int iMaxSkinSize)
{
	ASSERT(iMaxSkinSize < iSkinCacheSize);

	m_iSkinCacheSize	= iSkinCacheSize;
	m_iMaxSkinSize		= iMaxSkinSize;
	m_iSkinSizeCnt		= 0;

	m_pA3DSkinMan = g_pGame->GetA3DEngine()->GetA3DSkinMan();

	return true;
}

//	Release object
void CECSkinCache::Release()
{
	ResetSkinCache();
	ResetMuscleCache();
}

//	Reset cached objects
void CECSkinCache::Reset()
{
	ResetSkinCache();
	ResetMuscleCache();
}

//	Reset skin cache
void CECSkinCache::ResetSkinCache()
{
	//	Clear skin cache
	for (int i=0; i < m_SkinCache.GetSize(); i++)
	{
		SKINNODE* pNode = m_SkinCache[i];
		m_pA3DSkinMan->ReleaseSkin(&pNode->pA3DSkin);
		delete pNode;
	}

	m_SkinCache.RemoveAll();

	m_iSkinSizeCnt	= 0;
}

//	Reset muscle cache
void CECSkinCache::ResetMuscleCache()
{
}

//	Add a skin to cache
bool CECSkinCache::AddSkinToCache(A3DSkin* pA3DSkin)
{
	//	The skin is too large ?
	int iSkinSize = pA3DSkin->GetDataSize();
	if (iSkinSize > m_iMaxSkinSize)
		return false;

	int i;
	DWORD dwTimeStamp = a_GetTime();

	//	Check whether the skin has been added
	for (i=0; i < m_SkinCache.GetSize(); i++)
	{
		SKINNODE* pNode = m_SkinCache[i];

		if (pNode->pA3DSkin->GetSkinID() == pA3DSkin->GetSkinID())
		{
			pNode->dwTimeStamp = dwTimeStamp;
			return true;
		}
	}

	while (m_iSkinSizeCnt + iSkinSize > m_iSkinCacheSize)
	{
		if (!RemoveSkinFromCache(iSkinSize, dwTimeStamp))
			return false;
	}

	//	Load a new skin
	A3DSkin* pNewSkin = m_pA3DSkinMan->LoadSkinFile(pA3DSkin->GetFileName());
	if (!pNewSkin)
	{
		glb_ErrorOutput(ECERR_FAILEDTOCALL, "CECSkinCache::AddSkinToCache", __LINE__);
		return false;
	}

	SKINNODE* pNode = new SKINNODE;
	if (!pNode)
	{
		pNewSkin->Release();
		delete pNewSkin;

		glb_ErrorOutput(ECERR_NOTENOUGHMEMORY, "CECSkinCache::AddSkinToCache", __LINE__);
		return false;
	}

	pNode->dwTimeStamp	= dwTimeStamp;
	pNode->pA3DSkin		= pNewSkin;
	pNode->iSize		= iSkinSize;
	pNode->iKSize		= iSkinSize >> 10;

	//	Add to cache
	m_SkinCache.Add(pNode);

	m_iSkinSizeCnt += iSkinSize;

	return true;
}

//	Remove one skin form cache
bool CECSkinCache::RemoveSkinFromCache(int iNeedSize, DWORD dwCurTime)
{
	int iKSize = iNeedSize >> 10;
	int i, iRemove = -1, iMaxValue = -99999999;

	for (i=0; i < m_SkinCache.GetSize(); i++)
	{
		SKINNODE* pNode = m_SkinCache[i];
		
		int iValue = (int)(dwCurTime - pNode->dwTimeStamp) >> 10;

		int iDelta = pNode->iKSize - iKSize;
		if (iDelta < 0)
			iDelta = -iDelta * 2;

		iValue -= iDelta;

		if (iValue > iMaxValue)
		{
			iMaxValue = iValue;
			iRemove	= i;
		}
	}

	if (iRemove < 0)
		return false;

	//	Remove skin from cache
	SKINNODE* pNode = m_SkinCache[iRemove];
	m_iSkinSizeCnt -= pNode->iSize;

	m_pA3DSkinMan->ReleaseSkin(&pNode->pA3DSkin);
	delete pNode;

	m_SkinCache.RemoveAtQuickly(iRemove);
	
	return true;
}

//	Load muscle data from file
A3DMuscleData* CECSkinCache::LoadMuscleData(const char* szFile)
{
	int i;

	//	Check whether this muscle data file has been loaded
	DWORD dwDataID = a_MakeIDFromLowString(szFile);
	for (i=0; i < m_MuscleCache.GetSize(); i++)
	{
		MUSCLEDATA* pNode = m_MuscleCache[i];
		if (pNode->dwDataID == dwDataID && !pNode->strFile.CompareNoCase(szFile))
			break;
	}

	if (i < m_MuscleCache.GetSize())
		return m_MuscleCache[i]->pA3DMuscleData;

	//	Create a new muscle data object
	A3DMuscleData* pMuscleData = new A3DMuscleData;
	if (!pMuscleData)
	{
		glb_ErrorOutput(ECERR_NOTENOUGHMEMORY, "CECSkinCache::LoadMuscleData", __LINE__);
		return NULL;
	}

	pMuscleData->Init();

	if (!pMuscleData->Load(szFile))
	{
		glb_ErrorOutput(ECERR_FAILEDTOCALL, "CECSkinCache::LoadMuscleData", __LINE__);
		return NULL;
	}

	//	Create a new muscle data node
	MUSCLEDATA* pNode = new MUSCLEDATA;
	if (!pNode)
	{
		A3DRELEASE(pMuscleData);
		glb_ErrorOutput(ECERR_NOTENOUGHMEMORY, "CECSkinCache::LoadMuscleData", __LINE__);
		return NULL;
	}

	pNode->strFile			= szFile;
	pNode->dwDataID			= dwDataID;
	pNode->pA3DMuscleData	= pMuscleData;

	m_MuscleCache.Add(pNode);

	return pMuscleData;
}


