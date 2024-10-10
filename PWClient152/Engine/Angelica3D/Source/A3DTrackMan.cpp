/*
 * FILE: A3DTrackMan.cpp
 *
 * DESCRIPTION: A3D trace manager class
 *
 * CREATED BY: duyuxin, 2003/8/26
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "A3DPI.h"
#include "A3DTrackMan.h"
#include "AAssist.h"
#include "AMemory.h"
#include "AFileImage.h"
#include "ACSWrapper.h"

#define new A_DEBUG_NEW

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

//	Identity and version of skeleton track file
#define	STCKFILE_IDENTIFY	(('S'<<24) | ('T'<<16) | ('C'<<8) | 'K')

// Version history:
//
// Version 1:
//
// Version 2:
//		revise the compression algorithm for track data, consequently change 
//	some data members and its save/load method

#define STCKFILE_VERSION	2	

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

//	Skeleton file header
struct A3DSKLTRACKFILEHEADER
{
//	DWORD	dwFlags;		//	File's identity, SKEFILE_IDENTIFY
//	DWORD	dwVersion;		//	File's version, SKEFILE_VERSION
	int		iNumBoneTrack;	//	Number of bone track
	int		iAnimStart;		//	Animation start frame
	int		iAnimEnd;		//	Animation end frame
	int		iAnimFPS;		//	Animation FPS
};

namespace A3D
{
	A3DTrackMan		g_A3DTrackMan;
}

///////////////////////////////////////////////////////////////////////////
//
//	Local functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Implement A3DSklTrackSet
//
///////////////////////////////////////////////////////////////////////////

A3DSklTrackSet::A3DSklTrackSet(bool bBoneFile) :
m_TrackTab(64)
{
	m_bBoneFile		= bBoneFile;
	m_iRefCnt		= 1;
	m_dwFileNameID	= 0;
	m_iDataSize		= 0;

	m_iAnimStart	= 0;
	m_iAnimEnd		= 0;
	m_iAnimFPS		= 0;

	m_dwVersion = 1;
}

A3DSklTrackSet::~A3DSklTrackSet()
{
	//	Release all bone tracks
	TrackTable::iterator it = m_TrackTab.begin();
	for (; it != m_TrackTab.end(); ++it)
	{
		BONETRACK* pTrack = *it.value();
		delete pTrack->pPosTrack;
		delete pTrack->pRotTrack;

		if (pTrack->pAbsPosTrack)
			delete pTrack->pAbsPosTrack;

		if (pTrack->pAbsRotTrack)
			delete pTrack->pAbsRotTrack;

		delete pTrack;
	}

	m_TrackTab.clear();
}

//	Set track set file name
void A3DSklTrackSet::SetFileName(const char* szFileName)
{
	m_strFileName	= szFileName;
	m_dwFileNameID	= a_MakeIDFromLowString(szFileName);
}

//	Get bone track by id
A3DSklTrackSet::BONETRACK* A3DSklTrackSet::GetBoneTrack(int id)
{
	TrackTable::pair_type Pair = m_TrackTab.get(id);
	if (!Pair.second)
		return NULL;

	return *Pair.first;
}

//	Load track data from .stck file
bool A3DSklTrackSet::Load(const char* szFile)
{
	AFileImage File;

	if (!File.Open("", szFile, AFILE_OPENEXIST | AFILE_BINARY | AFILE_TEMPMEMORY))
	{
		File.Close();
		g_A3DErrLog.Log("A3DSklTrackSet::Load, Cannot open file %s!", szFile);
		return false;
	}

	if (!Load(&File))
	{
		File.Close();
		return false;
	}

	File.Close();

	return true;
}

//	Load track data from .stck file
bool A3DSklTrackSet::Load(AFile* pFile)
{
	DWORD dwRead, dwFlag, dwVersion;

	//	Read file flag
	if (!pFile->Read(&dwFlag, sizeof (DWORD), &dwRead))
		return false;

	//	Read file version
	if (!pFile->Read(&dwVersion, sizeof (DWORD), &dwRead))
		return false;

	if (dwFlag != STCKFILE_IDENTIFY || dwVersion > STCKFILE_VERSION)
	{
		g_A3DErrLog.Log("A3DSklTrackSet::Load, file flag or version error !");
		return false;
	}

	m_dwVersion = dwVersion;

	//	Read file header
	A3DSKLTRACKFILEHEADER fh;
	if (!pFile->Read(&fh, sizeof (fh), &dwRead))
	{
		g_A3DErrLog.Log("A3DSklTrackSet::Load, Failed to read file header !");
		return false;
	}

	m_iAnimStart	= fh.iAnimStart;
	m_iAnimEnd		= fh.iAnimEnd;
	m_iAnimFPS		= fh.iAnimFPS;

	//	Set file name
	SetFileName(pFile->GetRelativeName());

	int i, id;

	//	Load tracks' data
	for (i=0; i < fh.iNumBoneTrack; i++)
	{
		//	Read bone track id
		if (!pFile->Read(&id, sizeof (int), &dwRead))
			return false;

		if (!LoadBoneTrack(pFile, id))
		{
			g_A3DErrLog.Log("A3DSklTrackSet::Load, Failed to load bone track ! (id = %d)", id);
			return false;
		}
	}

	return true;
}

//	Create one bone track from file
bool A3DSklTrackSet::LoadBoneTrack(AFile* pFile, int id)
{
	//	Ensure track id collision
	BONETRACK* pTrack = GetBoneTrack(id);
	if (pTrack)
	{
		ASSERT(!pTrack);
		return false;
	}

	A3DTRACKSAVEDATA data;
	A3DVector3Track* pPosTrack = new A3DVector3Track;
	A3DQuaternionTrack* pRotTrack = new A3DQuaternionTrack;

	//	Load position track information
	if (!a3d_LoadTrackInfo(pFile, &data))
	{
		g_A3DErrLog.Log("A3DSklTrackSet::LoadBoneTrack, Failed to load position track information!");
		return false;
	}

	if (!pPosTrack || !pPosTrack->Create(data.iNumKey, data.iFrameRate, data.iNumSegment))
	{
		g_A3DErrLog.Log("A3DSklTrackSet::LoadBoneTrack, Failed to create position track");
		return false;
	}

	//	Load position track data
	if (!pPosTrack->Load(pFile, m_dwVersion))
	{
		g_A3DErrLog.Log("A3DSklTrackSet::LoadBoneTrack, Failed to load rotation track data!");
		return false;
	}

	//	Load rotation track information
	if (!a3d_LoadTrackInfo(pFile, &data))
	{
		g_A3DErrLog.Log("A3DSklTrackSet::LoadBoneTrack, Failed to load rotation track information!");
		return false;
	}

	if (!pRotTrack || !pRotTrack->Create(data.iNumKey, data.iFrameRate, data.iNumSegment))
	{
		g_A3DErrLog.Log("A3DSklTrackSet::LoadBoneTrack, Failed to create rotation track");
		return false;
	}

	pRotTrack->SetInterType(TRACKDATA_INTERTYPE_SLERP);

	//	Load rotation track data
	if (!pRotTrack->Load(pFile, m_dwVersion))
	{
		g_A3DErrLog.Log("A3DSklTrackSet::LoadBoneTrack, Failed to load rotation track data!");
		return false;
	}

	if (!(pTrack = new BONETRACK))
	{
		delete pPosTrack;
		delete pRotTrack;
		return false;
	}

	memset(pTrack, 0, sizeof (BONETRACK));

	m_iDataSize += pPosTrack->GetDataSize();
	m_iDataSize += pRotTrack->GetDataSize();

	pTrack->pPosTrack	= pPosTrack;
	pTrack->pRotTrack	= pRotTrack;

	//	Add track to table
	m_TrackTab.put(id, pTrack);

	return true;
}

//	Save track data to .stck file
bool A3DSklTrackSet::Save(const char* szFile)
{
	AFile File;

	if (!File.Open(szFile, AFILE_BINARY | AFILE_CREATENEW))
	{
		File.Close();
		g_A3DErrLog.Log("A3DSklTrackSet::Save, Cannot create file %s!", szFile);
		return false;
	}

	if (!Save(&File))
	{
		File.Close();
		return false;
	}

	File.Close();

	return true;
}

//	Save track data to .stck file
bool A3DSklTrackSet::Save(AFile* pFile)
{
	DWORD dwWrite;
	DWORD dwFlag = STCKFILE_IDENTIFY;
	DWORD dwVersion = STCKFILE_VERSION;

	//	Write file flag
	if (!pFile->Write(&dwFlag, sizeof (DWORD), &dwWrite))
		return false;

	//	Write file version
	if (!pFile->Write(&dwVersion, sizeof (DWORD), &dwWrite))
		return false;

	//	Write file header
	A3DSKLTRACKFILEHEADER fh;
	fh.iNumBoneTrack	= m_TrackTab.size();
	fh.iAnimStart		= m_iAnimStart;
	fh.iAnimEnd			= m_iAnimEnd;
	fh.iAnimFPS			= m_iAnimFPS;

	if (!pFile->Write(&fh, sizeof (fh), &dwWrite))
	{
		g_A3DErrLog.Log("A3DSklTrackSet::Save, Failed to write file header !");
		return false;
	}

	//	Write tracks' data
	TrackTable::iterator it = m_TrackTab.begin();
	for (; it != m_TrackTab.end(); ++it)
	{
		BONETRACK* pTrack = *it.value();

		//	Write bone track id
		int id = *it.key();
		pFile->Write(&id, sizeof (int), &dwWrite);

		//	Save position track
		if (!pTrack->pPosTrack->Save(pFile))
		{
			g_A3DErrLog.Log("A3DSklTrackSet::Save, Failed to save position track data!");
			return false;
		}

		//	Save rotation track
		if (!pTrack->pRotTrack->Save(pFile))
		{
			g_A3DErrLog.Log("A3DSklTrackSet::Save, Failed to save rotation track data!");
			return false;
		}
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////
//
//	Implement A3DTrackMan
//
///////////////////////////////////////////////////////////////////////////

A3DTrackMan::CCacheSize::CCacheSize()
{
	m_iCurSize	= 0;
	m_iMaxSize	= 0;
	m_iMinSize	= 0;
	m_bFull		= false;
}

//	Reset
void A3DTrackMan::CCacheSize::Reset(int iMax, int iMin)
{
	ASSERT(iMax >= iMin);
	m_iMaxSize	= iMax;
	m_iMinSize	= iMin;

	//	Update m_bFull flag
	AddDataSize(0);
}

//	Add data size
int A3DTrackMan::CCacheSize::AddDataSize(int iSize)
{
	m_iCurSize += iSize;

	if (m_iCurSize >= m_iMaxSize)
		m_bFull = true;
	
	if (m_iCurSize < m_iMinSize)
		m_bFull = false;

	return m_iCurSize;
}

A3DTrackMan::A3DTrackMan() : 
m_SklTrackTab(1024)
{
	m_iMorphDataSize = 0;

	m_ActCache.Reset(16 * 1024 * 1024, 14 * 1024 * 1024);

	InitializeCriticalSection(&m_csSTT);
	InitializeCriticalSection(&m_csMW);
}

A3DTrackMan::~A3DTrackMan()
{
	A3DTrackMan::Release();

	DeleteCriticalSection(&m_csSTT);
	DeleteCriticalSection(&m_csMW);
}

//	Release object
void A3DTrackMan::Release()
{
	//	All skeleton tracks should have been released
	ACSWrapper csa(&m_csSTT);

	if (m_SklTrackTab.size())
	{
		g_A3DErrLog.Log("A3DTrackMan::Release, %d skeleton track sets weren't normally released", m_SklTrackTab.size());

		SklTrackTable::iterator it = m_SklTrackTab.begin();
		for (; it != m_SklTrackTab.end(); ++it)
		{
			A3DSklTrackSet* pTrackSet = *it.value();
			delete pTrackSet;
		}

		m_SklTrackTab.clear();
	}

	csa.Detach(true);

	//	Release morph weight tracks
	csa.Attach(&m_csMW);

	if (m_aMorphWeights.GetCount())
	{
		g_A3DErrLog.Log("A3DTrackMan::Release, %d morph mesh channel weight tracks weren't normally released", m_aMorphWeights.GetCount());
		m_aMorphWeights.RemoveAll();
	}

	csa.Detach(true);
}

/*	Create skeleton track set

	dwFileNameID: track set file name ID
	szFileName: track set file name
	bOldBoneFile: true, track set data stored in old version .bon file; 
		false, track set data stored in .stck file.
	bNewTrackSet (out): true, returned track is created this time;
		false, returned track was created previously
*/
A3DSklTrackSet* A3DTrackMan::CreateSkeletonTrackSet(DWORD dwFileNameID, 
					const char* szFileName, bool bOldBoneFile, bool& bNewTrackSet)
{
	ACSWrapper csa(&m_csSTT);

	A3DSklTrackSet* pTrackSet = GetSkeletonTrackSet(dwFileNameID, szFileName);
	if (pTrackSet)
	{
		pTrackSet->IncRefCnt();
		bNewTrackSet = false;
		return pTrackSet;
	}

	if (!(pTrackSet = new A3DSklTrackSet(bOldBoneFile)))
		return NULL;

	if (bOldBoneFile)
	{
		//	For old version .bon file, track set data will be loaded outside
		pTrackSet->SetFileName(szFileName);
	}
	else
	{
		//	Try to load track set data from .stck file
		if (!pTrackSet->Load(szFileName))
		{
			delete pTrackSet;
			g_A3DErrLog.Log("A3DTrackMan::CreateTrackSet, Failed to load track set file %s", szFileName);
			return NULL;
		}

		AddActTrackDataSize(pTrackSet->GetDataSize());
	}

	//	Add track set to table
	if (!m_SklTrackTab.put(dwFileNameID, pTrackSet))
	{
		//	This shouldn't happen. One case may case this problem is that
		//	different file name generated the same file name ID.
		ASSERT(0);
		delete pTrackSet;
		return NULL;
	}

	bNewTrackSet = true;

	return pTrackSet;
}

//	Get skeleton track set by file name
A3DSklTrackSet* A3DTrackMan::GetSkeletonTrackSet(DWORD dwFileNameID, const char* szFileName)
{
	ACSWrapper csa(&m_csSTT);

	SklTrackTable::pair_type Pair = m_SklTrackTab.get(dwFileNameID);
	if (!Pair.second)
		return NULL;

	A3DSklTrackSet* pTrackSet = *Pair.first;
		
	//	Check file name
	if (_stricmp(pTrackSet->GetFileName(), szFileName))
	{
		//	Name collision: different file name have the same id.
		ASSERT(0);
		return NULL;
	}

	return pTrackSet;
}

//	Release skeleton track set
void A3DTrackMan::ReleaseSkeletonTrackSet(DWORD dwFileNameID, const char* szFileName)
{
	ACSWrapper csa(&m_csSTT);

	A3DSklTrackSet* pTrackSet = GetSkeletonTrackSet(dwFileNameID, szFileName);
	if (!pTrackSet)
		return;

	if (pTrackSet->DecRefCnt() > 0)
		return;

	AddActTrackDataSize(-pTrackSet->GetDataSize());

	//	Remove track set from table and release it.
	m_SklTrackTab.erase(dwFileNameID);
	delete pTrackSet;
}

//	Create morph mesh channel weight track
A3DFloatTrack* A3DTrackMan::CreateMorphChannelWeightTrack(DWORD dwMesh, int iChannel, int iNumKeys, int iFrameRate, int iNumSegment)
{
	ACSWrapper csa(&m_csMW);

	//	Search track in list
	ALISTPOSITION pos = m_aMorphWeights.GetHeadPosition();
	while (pos)
	{
		TRACKNODE& Node = m_aMorphWeights.GetNext(pos);
		if (Node.dwFlag1 == dwMesh && Node.dwFlag2 == (DWORD)iChannel)
		{
			A3DFloatTrack* pTrack = (A3DFloatTrack*)Node.pTrack;

			ASSERT(pTrack->GetNumKeys() == iNumKeys);
			ASSERT(pTrack->GetFrameRate() == iFrameRate);
			ASSERT(pTrack->GetSegmentNum() == iNumSegment);

			pTrack->AddRef();
			return pTrack;
		}
	}

	//	Create a new track
	A3DFloatTrack* pTrack = new A3DFloatTrack;
	if (!pTrack || !pTrack->Create(iNumKeys, iFrameRate, iNumSegment))
	{
		g_A3DErrLog.Log("A3DTrackMan::CreateMorphChannelWeightTrack, Failed to create morph mesh channel weight track");
		return NULL;
	}

	//	Add to list
	TRACKNODE Node = {dwMesh, (DWORD)iChannel, pTrack};
	pTrack->m_dwDelFlag1 = (DWORD)m_aMorphWeights.AddTail(Node);

	m_iMorphDataSize += pTrack->GetDataSize();

	return pTrack;
}

//	Release morph mesh channel weight track
void A3DTrackMan::ReleaseMorphChannelWeightTrack(A3DFloatTrack* pTrack)
{
	if (!pTrack || !pTrack->m_dwDelFlag1)
		return;

	ACSWrapper csa(&m_csMW);

	//	Remove track from list
	if (pTrack->m_nRefCount == 1)
	{
		m_aMorphWeights.RemoveAt((ALISTPOSITION)pTrack->m_dwDelFlag1);
		m_iMorphDataSize -= pTrack->GetDataSize();
	}

	pTrack->Release();
}

	//	Reset action track cache size
	void SetActCacheSize(int iSize);

