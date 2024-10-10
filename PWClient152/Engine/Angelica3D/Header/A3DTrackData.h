/*
 * FILE: A3DTrackData.h
 *
 * DESCRIPTION: a template classes that define keyframe's informations
 *
 * CREATED BY: Hedi, 2003/7/14
 *
 * HISTORY:
 *
 * Copyright (c) 2003 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DTRACKDATA_H_
#define _A3DTRACKDATA_H_

#include "AMemory.h"
#include "AAssist.h"
#include "AFile.h"
#include "A3DTypes.h"

//////////////////////////////////////////////////////////////////////////////////
// interpolation types of keyframe data
//////////////////////////////////////////////////////////////////////////////////

enum TRACKDATA_INTERTYPE
{
	TRACKDATA_INTERTYPE_NULL = 0,
	TRACKDATA_INTERTYPE_NEAREST = 1,
	TRACKDATA_INTERTYPE_LINEAR = 2,
	TRACKDATA_INTERTYPE_SLERP = 3,
	TRACKDATA_INTERTYPE_ACCELERATE = 4,
	TRACKDATA_INTERTYPE_DECELERATE = 5
};

//	Structure used to save and load track data
struct A3DTRACKSAVEDATA
{
	int		iNumKey;
	int		iNumSegment;
	int		iFrameRate;
};

//////////////////////////////////////////////////////////////////////////////////
//
//	Global functions
//
//////////////////////////////////////////////////////////////////////////////////

inline bool a3d_LoadTrackInfo(AFile* pFile, A3DTRACKSAVEDATA* pData)
{
	DWORD dwRead;
	if (!pFile->Read(pData, sizeof (A3DTRACKSAVEDATA), &dwRead) || dwRead != sizeof (A3DTRACKSAVEDATA))
		return false;

	return true;
}

/*
Track cache is not needed at this time because we only use fixed frame rate data source.
//////////////////////////////////////////////////////////////////////////////////
//
// class TRACKCACHE is used to hold the intermedia result of a key interpolating
// algorithm. It will accelerate the process of key index finding the factor 
// interpolating calculating.
// TRACKCACHE is also used to hold the interpolation type information. So when getting
// a key value without specifying a track cache, it will use NEAREST interpolation as default.
// Because different interpolating algorithm will use this structure in different
// way, so when interpolation type changed, the cached data will expired!
//
//////////////////////////////////////////////////////////////////////////////////
class TRACKCACHE
{
private:
	TRACKDATA_INTERTYPE	m_interType;	// interpolate type, this member is used to
										// determine the interpolation type when fetching
										// key values.

public:
	BOOL				m_bValid;		// TRUE if cached data is valid.
	int					m_nKeyFloor;	// the nearest key that before current time
	int					m_nKeyRoof;		// the nearest key that just after current time

	FLOAT				m_vCacheData1;	// some cached datas;
	FLOAT				m_vCacheData2;		

	TRACKCACHE() { m_interType = TRACKDATA_INTERTYPE_LINEAR; m_bValid = FALSE; }

public:
	////////////////////////////////////////////////////////////////////////
	// some inline functions.
	////////////////////////////////////////////////////////////////////////
	inline TRACKDATA_INTERTYPE GetInterType() const		{ return m_interType; }
	inline void SetInterType(TRACKDATA_INTERTYPE type)	{ m_interType = type; m_bValid = FALSE; }

};*/

typedef unsigned short FrameIndexType;		// no more than 65536 frames...

enum ATrackDataCompressionAlgorithm
{
	ATRACKDATA_COMPALGORITHM_RAWDATA,					// raw data without any compression

	ATRACKDATA_COMPALGORITHM_REMOVE_IDENTICAL_KEYS,		// remove all successive identical keys
	ATRACKDATA_COMPALGORITHM_REMOVE_LINEAR_KEYS,		// remove all keys under an error threshold based on linear approximations
};


/////////////////////////////////////////////////////////////////////////////////
//
// class A3DTrackData is the data that a controller or a joint will need when 
// performing control of the animations.
//
// Now A3DTrackData only support fixed framerate data as input datas, because such
// kind of data can be easily convert between its time and frame number. And it can
// specify a track start and end time to save extra space when no animation is carrying
// out.
//
/////////////////////////////////////////////////////////////////////////////////
template <class T> class A3DTrackData
{
public:		//	Types

	//	Track segment
	struct SEGMENT
	{
		int		iStartTime;		//	Segment start time in ms
		int		iEndTime;		//	Segment end time in ms
		int		iStartKey;		//	Segment start key frame
		int		iEndKey;		//	Segment end key frame
	};

public:

	friend class A3DTrackMan;
	friend class A3DAbsTrackMaker;

	inline int GetNumKeys()	const { return m_nNumKeys; }
	inline int GetFrameRate() const { return m_nFrameRate; }
	inline int GetSegmentNum() const { return m_iNumSegment; }
	inline TRACKDATA_INTERTYPE GetInterType() const { return m_interType; }
	inline void SetInterType(TRACKDATA_INTERTYPE type) { m_interType = type; }
	inline int GetDataSize() const { return m_iDataSize; }
	inline int GetRefCount() const { return m_nRefCount; }

	//	Get start time of whole track
	inline int GetTrackStartTime() const
	{
		return 0;
	}

	//	Get end time of whole track
	inline int GetTrackEndTime() const
	{
		return m_iTrackLengthMs;
	}

	//	Set segment's time
	inline void SetSegmentTime(int iSegment, int iStart, int iEnd)
	{
		ASSERT(m_aSegments);
		m_aSegments[iSegment].iStartTime = iStart;
		m_aSegments[iSegment].iEndTime	 = iEnd;
	}

	//	Set segment's key frame
	inline void SetSegmentKeyFrame(int iSegment, int iStart, int iEnd)
	{
		ASSERT(m_aSegments);
		m_aSegments[iSegment].iStartKey	= iStart;
		m_aSegments[iSegment].iEndKey	= iEnd;
	}

	inline T* GetKeyFrames() const { return m_pKeyFrames; }
	inline SEGMENT* GetSegments() const { return m_aSegments; }

	void DownSample(int nFrameRate);

	ATrackDataCompressionAlgorithm GetTrackDataCompressionAlgorithm() 
	{
		return m_TrackDataCompAlgorithm;
	}

	virtual bool DoCompression(ATrackDataCompressionAlgorithm trackDataCompAlgorithm, float fError);

public:

	A3DTrackData();
	virtual ~A3DTrackData();

public:

	//	Get current absolute frame number (not key number)
	float GetCurFrame(int iAbsTime);

	// Reference managing methods.
	int AddRef();
	int Release();

	//	get the nearest key just before current time
	int GetFloorKeyIndex(int nAbsTime, int* piSegment=NULL);
	//	get the nearest key to current time
	int GetNearestKeyIndex(int nAbsTime, int* piSegment=NULL);

	//	get a value according to nAbsTime, using current interpolating type
	//	nAbsTime: query time in millisecond
	virtual T GetKeyValue(int nAbsTime, int* pRefKeyLoc = NULL);
	void SetKeyValue(int nFrameIndex, const T& value);

	T GetKeyValueByIndex(int iKey)
	{
		ASSERT(iKey >= 0 && iKey < m_nNumKeys);
		return m_pKeyFrames[iKey];
	}

	//	functions used to set up the data in the track
	//	nNumKeys		IN			number of key frames
	//	nFrameRate		IN			frame rate of this track
	//	iNumSegment		IN			Number of segment
	bool Create(int nNumKeys, int nFrameRate, int iNumSegment);

	///////////////////////////////////////////////////////////////////////////////
	//
	//	The steps to load a track from file
	//	1. call A3DLoadTrackInfo() to get track's information from file
	//	2. Use A3DTrackMan to create track object.
	//	3. call A3DTrackData::Load() to load track's data.
	//
	///////////////////////////////////////////////////////////////////////////////
	//	Load track data
	bool Load(AFile* pFile, DWORD dwTrackSetVersion);
	//	Save track to file
	bool Save(AFile* pFile);

	//	This function doesn't load track data, just skip track data in file
	bool FakeLoad(AFile* pFile);

protected:

	//---------------------------------------------------------
	// compatible with old version....

	int GetFloorKeyIndex_OV(int nAbsTime, int* piSegment=NULL);
	int GetNearestKeyIndex_OV(int nAbsTime, int* piSegment=NULL);
	virtual T GetKeyValue_OV(int nAbsTime);

	//---------------------------------------------------------
	// locate the Key ID Zone of iKeyID by binary search method
	// return the index of Key ID Zone in the array m_pKeyFrameIDs.
	int LocateKeyID(int iKeyID, int iRefKeyLocation = -1);

	virtual bool ReadKeyFrameData(AFile* pFile, DWORD dwTrackSetVersion)
	{
		DWORD dwRead;
		//	Load key frame data
		if (!pFile->Read(m_pKeyFrames, m_nNumKeys * sizeof (T), &dwRead))
			return false;

		return true;
	}

	virtual bool WriteKeyFrameData(AFile* pFile)
	{
		DWORD dwWrite;
		if (m_pKeyFrames)
		{
			//	Write key frame data
			if (!pFile->Write(m_pKeyFrames, m_nNumKeys * sizeof (T), &dwWrite))
				return false;
		}

		return true;
	}

	virtual bool AreKeyDataIdentical(const T& k1, const T& k2, float fError)
	{
		return false;
	}

protected:

	TRACKDATA_INTERTYPE	m_interType;	//	Value interpolation type;

	float		m_fSlerpEpsilon;	//	Slerp epsilon
	DWORD		m_dwDelFlag1;		//	Position in track manager
	DWORD		m_dwDelFlag2;
	int			m_nRefCount;		//	Reference count, because track data will
	//	Be a kind of shared memory resource.
	int			m_nNumKeys;			//	Key point number
	int			m_nFrameRate;		//	Frame rate of data in this track, it will be
	//	Used in time-frame converts.
	T*			m_pKeyFrames;		//	Key frame's data buffer;
	SEGMENT*	m_aSegments;		//	Segment data
	int			m_iNumSegment;		//	Number of segment
	int			m_iDataSize;		//	Approximate data size of this track ??? not the exact size!!!

	//----------------------------------------------------------
	// revised by Wenfeng, Feb. 3rd, 2012
	FrameIndexType* m_pKeyFrameIDs;		// Key frame's id buffer
	int m_iTrackLengthMs;				// the track's time length (in milliseconds)
	int m_iTrackEndKeyID;				// the track's final key ID, computed from m_iTrackLengthMs.

	ATrackDataCompressionAlgorithm m_TrackDataCompAlgorithm;

	DWORD m_dwTrackSetVersion;
};

enum AQuatCompressionFormat
{
	AQUAT_COMPFMT_NO_COMPRESSION,		// raw data without compression
	AQUAT_COMPFMT_NO_W,					// remove the w component
};

class A3DQuaternionTrack : public A3DTrackData<A3DQUATERNION>
{

public:

	A3DQuaternionTrack() 
	{
		m_qCompFmt = AQUAT_COMPFMT_NO_W;
	}

	virtual A3DQUATERNION GetKeyValue(int nAbsTime, int* pRefKeyLoc = NULL);

	void SetQuatCompressionFormat(AQuatCompressionFormat qCompFmt) { m_qCompFmt = qCompFmt;}
	AQuatCompressionFormat GetQuatCompressionFormat() { return m_qCompFmt; }

protected:

	//	Load track data
	virtual bool ReadKeyFrameData(AFile* pFile, DWORD dwTrackSetVersion);
	//	Save track to file
	virtual bool WriteKeyFrameData(AFile* pFile);

	virtual bool AreKeyDataIdentical(const A3DQUATERNION& k1, const A3DQUATERNION& k2, float fError)
	{
		//----------------------------------------------------
		// note here: 
		//		QuatDiffAngle has a error larger than 0.01f, so if users require more accuracy,
		// we have to use more strict comparement.

		if(fError > 0.001f)
			return ( QuatDiffAngle(k1, k2) < fError );
		else
		{
			if (fabs(k1.x - k2.x) < 0.000001f &&
				fabs(k1.y - k2.y) < 0.000001f &&
				fabs(k1.z - k2.z) < 0.000001f &&
				fabs(k1.w - k2.w) < 0.000001f)
				return true;

			return false;
		}
	}


protected:

	AQuatCompressionFormat m_qCompFmt;
};


class A3DVector3Track : public A3DTrackData<A3DVECTOR3>
{

protected:

	virtual bool AreKeyDataIdentical(const A3DVECTOR3& k1, const A3DVECTOR3& k2, float fError)
	{
		return ( (k1 - k2).Magnitude() < fError );
	}

};

class A3DFloatTrack : public A3DTrackData<float>
{
	virtual bool AreKeyDataIdentical(const float& k1, const float& k2, float fError)
	{
		return ( fabs(k1 - k2) < fError );
	}

};

///////////////////////////////////////////////////////////////////////////////
//
//	Implement A3DTrackData
//
///////////////////////////////////////////////////////////////////////////////

template <class T>
A3DTrackData<T>::A3DTrackData()
{
	m_fSlerpEpsilon	= 0.25f;
	m_interType		= TRACKDATA_INTERTYPE_LINEAR;
	m_nFrameRate	= 30;		// 30 fps
	m_nNumKeys		= 0;
	m_pKeyFrames	= NULL;
	m_nRefCount		= 1;
	m_dwDelFlag1	= 0;
	m_dwDelFlag2	= 0;
	m_aSegments		= NULL;
	m_iNumSegment	= 0;
	m_iDataSize		= 0;

	m_pKeyFrameIDs = NULL;
	m_iTrackLengthMs = 0;
	m_iTrackEndKeyID = 0;
	m_TrackDataCompAlgorithm = ATRACKDATA_COMPALGORITHM_RAWDATA;
	
	// refer to the current version definition in A3DTrackMan.cpp 
	// #define STCKFILE_VERSION	2	
	m_dwTrackSetVersion = 2;

}

template <class T>
A3DTrackData<T>::~A3DTrackData()
{
	if (m_pKeyFrames)
	{
		delete [] m_pKeyFrames;
		m_pKeyFrames = NULL;
	}

	if (m_aSegments)
	{
		delete [] m_aSegments;
		m_aSegments = NULL;
	}

	if(m_pKeyFrameIDs)
	{
		delete [] m_pKeyFrameIDs;
		m_pKeyFrameIDs = NULL;
	}
}

template <class T>
int A3DTrackData<T>::GetFloorKeyIndex_OV(int nAbsTime, int* piSegment/* NULL */)
{
	ASSERT(m_aSegments);

	int iKey, iSegment;

	if (nAbsTime <= m_aSegments[0].iStartTime)
	{
		iSegment = 0;
		iKey = 0;
	}
	else if (nAbsTime >= m_aSegments[m_iNumSegment-1].iEndTime)
	{
		iSegment = m_iNumSegment - 1;
		iKey = m_nNumKeys - 1;
	}
	else
	{
		for (int i=0; i < m_iNumSegment; i++)
		{
			SEGMENT* pSegment = &m_aSegments[i];

			if (pSegment->iEndTime > nAbsTime)
			{
				//	pSegment->iStartKey == pSegment->iEndKey means all keys in the segament are same
				if (pSegment->iStartKey == pSegment->iEndKey)
					iKey = pSegment->iStartKey;
				else
					iKey = pSegment->iStartKey + (nAbsTime - pSegment->iStartTime) * m_nFrameRate / 1000;

				iSegment = i;
				break;
			}
		}
	}

	if (piSegment)
		*piSegment = iSegment;

	return iKey;

}

template <class T>
int A3DTrackData<T>::GetFloorKeyIndex(int nAbsTime, int* piSegment/* NULL */)
{
	if(m_dwTrackSetVersion < 2)
	{
		// compatible with old version
		return GetFloorKeyIndex_OV(nAbsTime, piSegment);
	}
	else
	{
		// new version...

		if(nAbsTime<0) return 0;

		if(nAbsTime > m_iTrackLengthMs) return m_iTrackEndKeyID;

		return (int)(nAbsTime * m_nFrameRate * 0.001f);
	}
}

template <class T>
int A3DTrackData<T>::GetNearestKeyIndex_OV(int nAbsTime, int* piSegment/* NULL */)
{

	ASSERT(m_aSegments);

	int iKey, iSegment;

	if (nAbsTime <= m_aSegments[0].iStartTime)
	{
		iSegment = 0;
		iKey = 0;
	}
	else if (nAbsTime >= m_aSegments[m_iNumSegment-1].iEndTime)
	{
		iSegment = m_iNumSegment - 1;
		iKey = m_nNumKeys - 1;
	}
	else
	{
		for (int i=0; i < m_iNumSegment; i++)
		{
			SEGMENT* pSegment = &m_aSegments[i];

			if (pSegment->iEndTime > nAbsTime)
			{
				//	pSegment->iStartKey == pSegment->iEndKey means all keys in the segament are same
				if (pSegment->iStartKey == pSegment->iEndKey)
					iKey = pSegment->iStartKey;
				else
					iKey = pSegment->iStartKey + (int)((nAbsTime - pSegment->iStartTime) * m_nFrameRate / 1000.0f + 0.5f);

				iSegment = i;
				break;
			}
		}
	}

	if (piSegment)
		*piSegment = iSegment;

	return iKey;

}

template <class T>
int A3DTrackData<T>::GetNearestKeyIndex(int nAbsTime, int* piSegment/* NULL */)
{
	if(m_dwTrackSetVersion < 2)
	{
		// compatible with old version
		return GetNearestKeyIndex_OV(nAbsTime, piSegment);
	}
	else
	{
		// new version...

		if(nAbsTime<0) return 0;

		if(nAbsTime > m_iTrackLengthMs) return m_iTrackEndKeyID;

		return (int)(nAbsTime * m_nFrameRate * 0.001f + 0.5f);

	}
}

//	Get current absolute frame number (not key number)
template <class T>
float A3DTrackData<T>::GetCurFrame(int iAbsTime)
{
	return iAbsTime * m_nFrameRate * 0.001f;
}


template <class T>
T A3DTrackData<T>::GetKeyValue_OV(int nAbsTime)
{
	ASSERT(m_aSegments);

	if (m_interType == TRACKDATA_INTERTYPE_NEAREST)
	{
		int nNearestIndex = GetNearestKeyIndex(nAbsTime);
		return m_pKeyFrames[nNearestIndex];
	}

	if (nAbsTime <= m_aSegments[0].iStartTime)
	{
		//	Return the first frame
		return m_pKeyFrames[0];
	}

	int iSegment, nFloorIndex;

	nFloorIndex = GetFloorKeyIndex(nAbsTime, &iSegment);
	if (nFloorIndex >= m_nNumKeys - 1)
	{
		//	Return the last frame
		return m_pKeyFrames[m_nNumKeys - 1];
	}

	if (m_aSegments[iSegment].iStartKey == m_aSegments[iSegment].iEndKey)
	{
		//	This is a static segment
		return m_pKeyFrames[m_aSegments[iSegment].iStartKey];
	}

	int nRoofIndex = nFloorIndex + 1;

	const T& value1 = m_pKeyFrames[nFloorIndex];
	const T& value2 = m_pKeyFrames[nRoofIndex];

	FLOAT f;		// linear factor of current time between these two key point
	FLOAT f1, f2;	// factors that has been transformed using current interpolation type.
	FLOAT vTimeGap = 1000.0f / m_nFrameRate; // time duration of each frame in millisecond

	int iKeyOffset = nFloorIndex - m_aSegments[iSegment].iStartKey;
	f = (nAbsTime - m_aSegments[iSegment].iStartTime) / vTimeGap - iKeyOffset;
	a_Clamp(f, 0.0f, 1.0f);

	switch (m_interType)
	{
	case TRACKDATA_INTERTYPE_LINEAR:

		f1 = 1.0f - f;
		f2 = f;
		break;

	case TRACKDATA_INTERTYPE_SLERP: // for Quaternion Interpolation only
		{	
			FLOAT cosine, sign;
			cosine = DotProduct(value1, value2);
			if (cosine < 0.0f)
			{
				cosine = -cosine;
				sign = -1.0f;
			}
			else
				sign = 1.0f;

			if (cosine > 1.0f - SLERP_EPSILON)
			{
				// the from and to value are very close, so use LERP will be ok
				f1 = 1.0f - f;
				f2 = f * sign;
			}
			else
			{
				FLOAT theta;
				theta = (FLOAT)acos(cosine);
				FLOAT sine;
				sine = (FLOAT)sin(theta);
				f1 = (FLOAT)(sin((1.0f - f) * theta) / sine);
				f2 = (FLOAT)(sin(f * theta) / sine) * sign;
			}

			break;
		}
	case TRACKDATA_INTERTYPE_ACCELERATE:

		f1 = f * f;
		f2 = 1.0f - f1;
		break;

	case TRACKDATA_INTERTYPE_DECELERATE:

		f1 = (2.0f - f) * f;
		f2 = 1.0f - f1;
		break;
	}

	return (value1 * f1 + value2 * f2);

}

template <class T>
T A3DTrackData<T>::GetKeyValue(int nAbsTime, int* pRefKeyLoc)
{

	if(m_dwTrackSetVersion < 2)
	{
		return GetKeyValue_OV(nAbsTime);
	}
	else
	{
		if (m_interType == TRACKDATA_INTERTYPE_NEAREST)
		{
			int nNearestIndex = GetNearestKeyIndex(nAbsTime);
			int iLocation;

			if(pRefKeyLoc)
			{
				iLocation = LocateKeyID(nNearestIndex, *pRefKeyLoc);
				*pRefKeyLoc = iLocation;
			}
			else
			{
				iLocation = LocateKeyID(nNearestIndex);
			}

			return m_pKeyFrames[iLocation];
		}

		if (nAbsTime <= 0)
		{
			//	Return the first frame
			return m_pKeyFrames[0];
		}


		if (nAbsTime >= m_iTrackLengthMs)
		{
			//	Return the last frame
			return m_pKeyFrames[m_nNumKeys - 1];
		}

		float fFrame = GetCurFrame(nAbsTime);
		int nFloorIndex = (int)fFrame;
		int nRoofIndex = nFloorIndex + 1;

		int iKeyIDLocation;
		if(pRefKeyLoc)
		{
			iKeyIDLocation = LocateKeyID(nFloorIndex, *pRefKeyLoc);
			*pRefKeyLoc = iKeyIDLocation;
		}
		else
		{
			iKeyIDLocation = LocateKeyID(nFloorIndex);
		}

		FLOAT f = 0.0f;					// linear factor of current time between these two key point

		if(iKeyIDLocation + 1 < m_nNumKeys)
		{
			switch (m_TrackDataCompAlgorithm)
			{

			case ATRACKDATA_COMPALGORITHM_RAWDATA:
				{

					f = fFrame - nFloorIndex;

					break;
				}

			case ATRACKDATA_COMPALGORITHM_REMOVE_IDENTICAL_KEYS:
				{
					if(iKeyIDLocation + 1 < m_nNumKeys && nRoofIndex == m_pKeyFrameIDs[iKeyIDLocation + 1])
					{
						f = fFrame - nFloorIndex;
					}

					break;
				}

			case ATRACKDATA_COMPALGORITHM_REMOVE_LINEAR_KEYS:
				{

					f = (fFrame - m_pKeyFrameIDs[iKeyIDLocation]) / (m_pKeyFrameIDs[iKeyIDLocation + 1] - m_pKeyFrameIDs[iKeyIDLocation]);

					break;
				}

			default:

				ASSERT(0);
				break;

			};

		}



		if(f != 0.0f)
		{
			// execute the interpolation...

			const T& value1 = m_pKeyFrames[iKeyIDLocation];
			const T& value2 = m_pKeyFrames[iKeyIDLocation + 1];


			a_Clamp(f, 0.0f, 1.0f);

			FLOAT f1, f2;						// factors that has been transformed using current interpolation type.

			switch (m_interType)
			{
			case TRACKDATA_INTERTYPE_LINEAR:

				f1 = 1.0f - f;
				f2 = f;
				break;

			case TRACKDATA_INTERTYPE_SLERP:
				{
					//	Only quaternion supports this type interpolation
					ASSERT(m_interType != TRACKDATA_INTERTYPE_SLERP);
					break;
				}
			case TRACKDATA_INTERTYPE_ACCELERATE:

				f1 = f * f;
				f2 = 1.0f - f1;
				break;

			case TRACKDATA_INTERTYPE_DECELERATE:

				f1 = (2.0f - f) * f;
				f2 = 1.0f - f1;
				break;
			}

			return (value1 * f1 + value2 * f2);
		}
		else
			return m_pKeyFrames[iKeyIDLocation];

	}
}

template <class T>
int A3DTrackData<T>::AddRef()
{
	return ++ m_nRefCount;
}

template <class T>
int A3DTrackData<T>::Release()
{
	m_nRefCount--;
	if (m_nRefCount)
		return m_nRefCount;

	// Now no one references this object any more, so just delete it.
	delete this;
	return 0;
}

template <class T>
bool A3DTrackData<T>::Create(int nNumKeys, int nFrameRate, int iNumSegment)
{
	ASSERT(nNumKeys > 0);

	m_nNumKeys		= nNumKeys;
	m_iNumSegment	= iNumSegment;
	m_nFrameRate	= nFrameRate;

	// m_iDataSize		= nNumKeys * sizeof (T) + iNumSegment * sizeof (SEGMENT);
	m_iDataSize		= nNumKeys * (sizeof (T) + sizeof (FrameIndexType));


	if (!(m_pKeyFrames = new T[nNumKeys]))
		return false;

	if (iNumSegment > 0 && !(m_aSegments = new SEGMENT[iNumSegment]))
		return false;

	return true;
}

template <class T>
bool A3DTrackData<T>::Load(AFile* pFile, DWORD dwTrackSetVersion)
{
	ASSERT(m_pKeyFrames);

	DWORD dwRead;

	m_dwTrackSetVersion = dwTrackSetVersion;

	if(dwTrackSetVersion < 2)
	{
		//	Load key frame data
		if(!ReadKeyFrameData(pFile, dwTrackSetVersion))
			return false;

		//	Load segment data
		if (!pFile->Read(m_aSegments, m_iNumSegment * sizeof (SEGMENT), &dwRead))
			return false;

		// convert old data to new format...

		ASSERT(m_aSegments);

		int iStartFrame = (int) ((m_aSegments[0].iStartTime + 1) * m_nFrameRate * 0.001f);

		if(m_pKeyFrameIDs) delete [] m_pKeyFrameIDs;
		m_pKeyFrameIDs = new FrameIndexType[m_nNumKeys];

		m_pKeyFrameIDs[0] = 0;
		for(int i=1; i<m_nNumKeys; ++i)
		{
			m_pKeyFrameIDs[i] = iStartFrame + i;
		}

		m_iTrackLengthMs = m_aSegments[m_iNumSegment-1].iEndTime;
		m_iTrackEndKeyID = (int) ((m_iTrackLengthMs + 1) * m_nFrameRate * 0.001f);
		m_TrackDataCompAlgorithm = ATRACKDATA_COMPALGORITHM_REMOVE_IDENTICAL_KEYS;

		//------------------------------------------------------------
		// revised by Wenfeng, Dec. 28, 2012
		// oops, we can't do that since the segment info are useful in some cases
		// after loading old segment, we release it instantly to keep consistent with new data format...
		/*
			delete [] m_aSegments;
			m_aSegments = NULL;
			m_iNumSegment = 0;
		*/
		//------------------------------------------------------------

	}
	else
	{
		// new version with new data

		if(!ReadKeyFrameData(pFile, dwTrackSetVersion))
			return false;

		//	Load track time length
		if (!pFile->Read(&m_iTrackLengthMs, sizeof (int), &dwRead))
			return false;

		// write track data compression algorithm
		if(!pFile->Read(&m_TrackDataCompAlgorithm, sizeof(int), &dwRead))
			return false;

		if(m_TrackDataCompAlgorithm != ATRACKDATA_COMPALGORITHM_RAWDATA)
		{
			//	Load key ID data
			if(m_pKeyFrameIDs) delete [] m_pKeyFrameIDs;
			m_pKeyFrameIDs = new FrameIndexType[m_nNumKeys];
			if (!pFile->Read(m_pKeyFrameIDs, m_nNumKeys * sizeof (FrameIndexType), &dwRead))
				return false;

		}

		if(m_iTrackLengthMs== 0)
			m_iTrackLengthMs = (int)((m_nNumKeys-1) * 1000.0f / m_nFrameRate);

		m_iTrackEndKeyID = (int) ((m_iTrackLengthMs + 1) * m_nFrameRate * 0.001f);
	}

	return true;
}

//	This function doesn't load track data, just skip track data in file
template <class T>
bool A3DTrackData<T>::FakeLoad(AFile* pFile)
{
	//DWORD dwBytes = m_nNumKeys * sizeof (T);
	//dwBytes += m_iNumSegment * sizeof (SEGMENT);
	//return pFile->Seek(dwBytes, AFILE_SEEK_CUR);

	// to do...
	ASSERT(0);
	return false;
}

template <class T>
bool A3DTrackData<T>::Save(AFile* pFile)
{
	//	Fill data
	A3DTRACKSAVEDATA Data;

	Data.iNumKey		= m_nNumKeys;
	Data.iNumSegment	= m_iNumSegment;
	Data.iFrameRate		= m_nFrameRate;

	DWORD dwWrite;

	//	Write track data
	if (!pFile->Write(&Data, sizeof (Data), &dwWrite))
		return false;

	if(!WriteKeyFrameData(pFile))
		return false;

	// write track time length
	if(!pFile->Write(&m_iTrackLengthMs, sizeof(int), &dwWrite))
		return false;

	// write track data compression algorithm
	if(!pFile->Write(&m_TrackDataCompAlgorithm, sizeof(int), &dwWrite))
		return false;


	if(m_pKeyFrameIDs && m_TrackDataCompAlgorithm != ATRACKDATA_COMPALGORITHM_RAWDATA)
	{
		//	Write Key ID data
		if (!pFile->Write(m_pKeyFrameIDs, m_nNumKeys * sizeof (FrameIndexType), &dwWrite))
			return false;		
	}



	return true;
}

template <class T>
void A3DTrackData<T>::SetKeyValue(int nFrameIndex, const T& value)
{
	// If the index is invalid, just ignore it.
	if( nFrameIndex < 0 || nFrameIndex >= m_nNumKeys )
		return;

	m_pKeyFrames[nFrameIndex] = value;
	return;
}

template <class T>
void A3DTrackData<T>::DownSample(int nFrameRate)
{

	// to do...
	ASSERT(0);

	//int rate = m_nFrameRate / nFrameRate;
	//if( rate * nFrameRate != m_nFrameRate )
	//	return;

	//if( rate == 1 )
	//	return;

	//int nNumKeys = m_nNumKeys / rate;
	//if( nNumKeys < 1 )
	//	return;

	//T * pKeyFrames = new T[nNumKeys];

	//int i;
	//for(i=0; i<nNumKeys; i++)
	//	pKeyFrames[i] = m_pKeyFrames[i * rate];

	//// release old ones.
	//delete [] m_pKeyFrames;
	//m_pKeyFrames = NULL;

	//m_nNumKeys = nNumKeys;
	//m_pKeyFrames = pKeyFrames;
	//m_nFrameRate = nFrameRate;

	//for(i=0; i<m_iNumSegment; i++)
	//{
	//	m_aSegments[i].iStartKey /= rate;
	//	m_aSegments[i].iEndKey /= rate;
	//}

}

template <class T>
int A3DTrackData<T>::LocateKeyID(int iKeyID, int iRefKeyLocation)
{
	if(m_TrackDataCompAlgorithm == ATRACKDATA_COMPALGORITHM_RAWDATA) return iKeyID;

	ASSERT(m_pKeyFrameIDs);

	if(iKeyID <= 0) return 0;
	if(iKeyID >= m_pKeyFrameIDs[m_nNumKeys-1])	return m_nNumKeys-1;

	if(iRefKeyLocation >= 0 && iRefKeyLocation < m_nNumKeys)
	{
		// use reference key location to accelerate the location process
		for(int i=0, iCur = iRefKeyLocation; i<m_nNumKeys; ++i)
		{
			int iNext = iCur + 1;
			if(iNext >= m_nNumKeys) iNext = 0;

			if(iKeyID < m_pKeyFrameIDs[iNext] && iKeyID >= m_pKeyFrameIDs[iCur])
				return iCur;

			iCur = iNext;
		}

		// if the program goes here, something may be wrong...
		ASSERT(0);
		return iRefKeyLocation;

	}
	else
	{
			// use binary search to locate iKeyID
		int b = 0, e = m_nNumKeys -1;		// begin, end
		unsigned int c;						// current pos
		while( e - b > 1 )
		{
			c = e - b + 1;
			c = c >> 1;

			if(iKeyID >= m_pKeyFrameIDs[b + c])
				b = b + c;
			else
				e = b + c;
		}

		return b;
	}

}

template<class T>
bool A3DTrackData<T>::DoCompression(ATrackDataCompressionAlgorithm trackDataCompAlgorithm, float fError)
{
	if(m_TrackDataCompAlgorithm == ATRACKDATA_COMPALGORITHM_RAWDATA)
	{
		// we assume that the track data is raw data before doing compression
		m_iTrackEndKeyID = m_nNumKeys - 1;
		m_iTrackLengthMs = (int)((m_nNumKeys - 1) * 1000.0f / m_nFrameRate);

		if(trackDataCompAlgorithm == ATRACKDATA_COMPALGORITHM_REMOVE_IDENTICAL_KEYS)
		{
			// currently we only handle the ATRACKDATA_COMPALGORITHM_REMOVE_IDENTICAL_KEYS algorithm

			AArray<int> arrKeyIDs;
			int iCur = 0;
			arrKeyIDs.Add(iCur);

			// try to remove the key frame data...
			for(int i = 1; i<m_nNumKeys; ++i)
			{
				if(!AreKeyDataIdentical(m_pKeyFrames[iCur], m_pKeyFrames[i], fError))
				{
					iCur = i;
					arrKeyIDs.Add(iCur);
				}
			}

			// set some extra data...
			m_TrackDataCompAlgorithm = ATRACKDATA_COMPALGORITHM_REMOVE_IDENTICAL_KEYS;

			// copy new key frame data...
			m_nNumKeys = arrKeyIDs.GetSize();

			T* pNewKeyFrames = new T[m_nNumKeys];

			if(m_pKeyFrameIDs) delete [] m_pKeyFrameIDs;
			m_pKeyFrameIDs = new FrameIndexType[m_nNumKeys];

			for(int i=0; i< m_nNumKeys; ++i)
			{
				pNewKeyFrames[i] = m_pKeyFrames[arrKeyIDs[i]];
				m_pKeyFrameIDs[i] = arrKeyIDs[i];
			}

			// do some cleanup work...
			delete [] m_pKeyFrames;
			m_pKeyFrames = pNewKeyFrames;



			return true;
		}
		else if(trackDataCompAlgorithm == ATRACKDATA_COMPALGORITHM_RAWDATA)
		{
			m_TrackDataCompAlgorithm = ATRACKDATA_COMPALGORITHM_RAWDATA;
			return true;
		}
		else
		{
			ASSERT(0);
			return false;
		}

	}
	else
		return false;
}

#endif	//	_A3DTRACKDATA_H_

