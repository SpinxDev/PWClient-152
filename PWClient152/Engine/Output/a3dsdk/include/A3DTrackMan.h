/*
 * FILE: A3DTrackMan.h
 *
 * DESCRIPTION: A3D trace manager class
 *
 * CREATED BY: duyuxin, 2003/8/26
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DTRACKMAN_H_
#define _A3DTRACKMAN_H_

#include "AList2.h"
#include "hashtab.h"
#include "A3DTypes.h"
#include "A3DQuaternion.h"
#include "A3DTrackData.h"
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

class AFile;
class A3DSkinModel;

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class A3DSklTrackSet
//
///////////////////////////////////////////////////////////////////////////

class A3DSklTrackSet
{
public:		//	Types

	//	Track of a bone
	struct BONETRACK
	{
		A3DVector3Track*		pPosTrack;
		A3DQuaternionTrack*		pRotTrack;
		A3DVector3Track*		pAbsPosTrack;
		A3DQuaternionTrack*		pAbsRotTrack;
	};

	typedef abase::hashtab<BONETRACK*, int, abase::_hash_function> TrackTable;

	friend class A3DSkeleton;
	friend class A3DTrackMan;
	
public:		//	Constructors and Destructors

	A3DSklTrackSet(bool bBoneFile);
	virtual ~A3DSklTrackSet();
	
public:		//	Attributes

public:		//	Operations

	//	Load track data from .stck file
	bool Load(const char* szFile);
	bool Load(AFile* pFile);
	//	Save track data to .stck file
	bool Save(const char* szFile);
	bool Save(AFile* pFile);

	//	Set track set file name
	void SetFileName(const char* szFileName);
	//	Get track set file name
	const char* GetFileName() { return m_strFileName; }
	//	Get track set file name ID
	DWORD GetFileNameID() { return m_dwFileNameID; }

	//	Get bone track by id
	BONETRACK* GetBoneTrack(int id);
	//	Get bone's position track by id
	A3DVector3Track* GetBonePosTrack(int id)
	{
		BONETRACK* pTrack = GetBoneTrack(id);
		return pTrack ? pTrack->pPosTrack : NULL;
	}
	//	Get bone's rotation track by id
	A3DQuaternionTrack* GetBoneRotTrack(int id)
	{
		BONETRACK* pTrack = GetBoneTrack(id);
		return pTrack ? pTrack->pRotTrack : NULL;
	}
	//	Create absolute position and rotate track for specified bone
	bool CreateAbsBoneTrack(int id);
	//	Get bone's absolute position track by id
	A3DVector3Track* GetBoneAbsPosTrack(int id)
	{
		BONETRACK* pTrack = GetBoneTrack(id);
		return pTrack ? pTrack->pAbsPosTrack : NULL;
	}
	//	Get bone's absolute rotation track by id
	A3DQuaternionTrack* GetBoneAbsRotTrack(int id)
	{
		BONETRACK* pTrack = GetBoneTrack(id);
		return pTrack ? pTrack->pAbsRotTrack : NULL;
	}

	//	Get animation start frame
	int GetAnimStartFrame() const { return m_iAnimStart; }
	//	Get animation end frame
	int GetAnimEndFrame() const { return m_iAnimEnd; }
	//	Set animation frame range
	void SetAnimFrameRange(int iStart, int iEnd) { m_iAnimStart = iStart; m_iAnimEnd = iEnd; }
	//	Get animation FPS
	int GetAnimFPS() const { return m_iAnimFPS; }
	//	Set animation FPS
	void SetAnimFPS(int iFPS) { m_iAnimFPS = iFPS; }
	//	Convert frame to time
	int FrameToTime(int iFrame) const { return (int)(iFrame * 1000.0f / m_iAnimFPS); }

	//	Get bone file flag
	bool GetBoneFileFlag() { return m_bBoneFile; }
	//	Get data size
	int GetDataSize() { return m_iDataSize; }

	//	Get track table
	TrackTable& GetTrackTable() { return m_TrackTab; }

protected:	//	Attributes

	AString		m_strFileName;	//	Track set file name
	DWORD		m_dwFileNameID;	//	Track ID generated from file name
	bool		m_bBoneFile;	//	true, track set data is loaded from old version bone file.
									//	false, track set data is loaded from .stck file.
	int			m_iRefCnt;		//	Reference counter
	int			m_iDataSize;	//	Approximate data size of track set

	int			m_iAnimStart;	//	Animation start frame
	int			m_iAnimEnd;		//	Animation end frame
	int			m_iAnimFPS;		//	Animation FPS

	TrackTable	m_TrackTab;		//	Track table

	DWORD		m_dwVersion;

protected:	//	Operations

	//	Increase bone file reference counter
	int IncRefCnt() { return ++m_iRefCnt; }
	int DecRefCnt() { return --m_iRefCnt; }
	//	Create one bone track from file
	bool LoadBoneTrack(AFile* pFile, int id);
};

///////////////////////////////////////////////////////////////////////////
//
//	Class A3DTrackMan
//
///////////////////////////////////////////////////////////////////////////

class A3DTrackMan
{
public:		//	Types

	//	Track node
	struct TRACKNODE
	{
		DWORD	dwFlag1;	//	Flag 1
		DWORD	dwFlag2;	//	Flag 2
		void*	pTrack;		//	Track object
	};

	//	Cache size manager
	class CCacheSize
	{
	public:

		CCacheSize();
		//	Reset
		void Reset(int iMax, int iMin);
		//	Add data size
		int AddDataSize(int iSize);
		//	Get current data size
		int GetDataSize() { return m_iCurSize; }
		//	Is cache is full
		bool IsFull() { return m_bFull; }

	protected:

		int		m_iCurSize;		//	Current data size
		int		m_iMaxSize;		//	Maximum cache data size
		int		m_iMinSize;		//	Minimum cache data size
		bool	m_bFull;		//	true, cache is full
	};

	typedef AList2<TRACKNODE, TRACKNODE&> TrackList;
	typedef abase::hashtab<A3DSklTrackSet*, DWORD, abase::_hash_function> SklTrackTable;

public:		//	Constructors and Destructors

	A3DTrackMan();
	virtual ~A3DTrackMan();
	
public:		//	Attributes

public:		//	Operations

	//	Create skeleton track set
	A3DSklTrackSet* CreateSkeletonTrackSet(DWORD dwFileNameID, const char* szFileName, bool bOldBoneFile, bool& bNewTrackSet);
	//	Get skeleton track set by file name
	A3DSklTrackSet* GetSkeletonTrackSet(DWORD dwFileNameID, const char* szFileName);
	//	Release skeleton track set
	void ReleaseSkeletonTrackSet(DWORD dwFileNameID, const char* szFileName);

	//	Create morph mesh channel weight track
	A3DFloatTrack* CreateMorphChannelWeightTrack(DWORD dwMesh, int iChannel, int iNumKeys, int iFrameRate, int iNumSegment);
	//	Release morph mesh channel weight track
	void ReleaseMorphChannelWeightTrack(A3DFloatTrack* pTrack);

	//	Get total track data size
	int GetTrackDataSize() { return m_ActCache.GetDataSize() + m_iMorphDataSize; }
	//	Get total action track data size
	int GetActTrackDataSize() { return m_ActCache.GetDataSize(); }
	//	Add action track data size
	void AddActTrackDataSize(int iSize) { m_ActCache.AddDataSize(iSize); }
	//	Reset action track cache size
	void SetActCacheSize(int iSize) { m_ActCache.Reset(iSize, (int)(iSize * 0.875f)); }
	//	Check whether action track cache is full
	bool IsActCacheFull() { return m_ActCache.IsFull(); }

protected:	//	Attributes

	SklTrackTable		m_SklTrackTab;		//	Skeleton track table
	TrackList			m_aMorphWeights;	//	Morph mesh channel weight track
	CRITICAL_SECTION	m_csSTT;			//	Skeleton track table lock
	CRITICAL_SECTION	m_csMW;				//	Morph mesh channel weight track lock
	
	int			m_iMorphDataSize;	//	Morph track size
	CCacheSize	m_ActCache;			//	Action track cache size manager

protected:	//	Operations

	//	Release object
	void Release();
};

namespace A3D
{
	extern A3DTrackMan	g_A3DTrackMan;
}

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////



#endif	//	_A3DTRACKMAN_H_

