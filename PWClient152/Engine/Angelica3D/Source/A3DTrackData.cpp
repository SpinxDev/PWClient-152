
#include "A3DTrackData.h"
#include "AMemory.h"

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


namespace TrackDataCompression
{
	struct Float3
	{
		float x;
		float y;
		float z;

		void ToQuat(A3DQUATERNION& q)
		{
			q.x = x;
			q.y = y;
			q.z = z;
			float s = 1.0f - x*x - y*y - z*z;
			q.w = s > 0.0f ? sqrt(s): 0.0f;
		}

		void FromQuat(const A3DQUATERNION& q)
		{
			A3DQUATERNION tmpQ(q);
			if(tmpQ.w<0.0f)
			{
				tmpQ.x = - tmpQ.x;
				tmpQ.y = - tmpQ.y;
				tmpQ.z = - tmpQ.z;
				tmpQ.w = - tmpQ.w;
			}

			tmpQ.Normalize();

			x = tmpQ.x;
			y = tmpQ.y;
			z = tmpQ.z;

		}
	};

}


///////////////////////////////////////////////////////////////////////////
//
//	Local functions
//
///////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
//
//	Implement A3DQuaternionTrack
//
///////////////////////////////////////////////////////////////////////////////

A3DQUATERNION A3DQuaternionTrack::GetKeyValue(int nAbsTime, int* pRefKeyLoc)
{
	if (m_interType != TRACKDATA_INTERTYPE_SLERP || m_dwTrackSetVersion < 2)
		return A3DTrackData::GetKeyValue(nAbsTime);


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

		//	Do interpolation

		const A3DQUATERNION& value1 = m_pKeyFrames[iKeyIDLocation];
		const A3DQUATERNION& value2 = m_pKeyFrames[iKeyIDLocation + 1];

		a_Clamp(f, 0.0f, 1.0f);

		FLOAT f1, f2;						// factors that has been transformed using current interpolation type.


		FLOAT cosine, sign;
		cosine = DotProduct(value1, value2);
		if (cosine < 0.0f)
		{
			cosine = -cosine;
			sign = -1.0f;
		}
		else
			sign = 1.0f;

		if (cosine > 1.0f - m_fSlerpEpsilon)
		{
			// the from and to value are very close, so use LERP will be ok
			f1 = 1.0f - f;
			f2 = f * sign;
		}
		else
		{
			FLOAT theta;
			theta = acosf(cosine);
			FLOAT sine;
			sine = sinf(theta);
			f1 = sinf((1.0f - f) * theta) / sine;
			f2 = (sinf(f * theta) / sine) * sign;
		}

		return (value1 * f1 + value2 * f2);
	}
	else
		return m_pKeyFrames[iKeyIDLocation];
}


//	Load track data
bool A3DQuaternionTrack::ReadKeyFrameData(AFile* pFile, DWORD dwTrackSetVersion)
{
	ASSERT(m_pKeyFrames);

	DWORD dwRead;

	if(dwTrackSetVersion < 2)
	{
		if(!A3DTrackData<A3DQUATERNION>::ReadKeyFrameData(pFile, dwTrackSetVersion))
			return false;
	}
	else
	{
		// new version with new data

		if (!pFile->Read(&m_qCompFmt, sizeof (AQuatCompressionFormat), &dwRead))
			return false;

		if(m_qCompFmt == AQUAT_COMPFMT_NO_COMPRESSION)
		{
			if (m_pKeyFrames)
			{
				//	Write key frame data
				if (!pFile->Read(m_pKeyFrames, m_nNumKeys * sizeof (A3DQUATERNION), &dwRead))
					return false;
			}
		}
		else if (m_qCompFmt == AQUAT_COMPFMT_NO_W)
		{

			TrackDataCompression::Float3* f3Data = new TrackDataCompression::Float3[m_nNumKeys];
			if (!pFile->Read(f3Data, m_nNumKeys * sizeof (TrackDataCompression::Float3), &dwRead))
			{
				delete f3Data;
				return false;
			}

			for(int i=0; i<m_nNumKeys; ++i)
			{
				f3Data[i].ToQuat(m_pKeyFrames[i]);
			}


			delete f3Data;

		}		
		
	}

	return true;
}


//	Save track to file
bool A3DQuaternionTrack::WriteKeyFrameData(AFile* pFile)
{

	DWORD dwWrite;

	if (!pFile->Write(&m_qCompFmt, sizeof (AQuatCompressionFormat), &dwWrite))
		return false;

	if(m_qCompFmt == AQUAT_COMPFMT_NO_COMPRESSION)
	{
		if (m_pKeyFrames)
		{
			//	Write key frame data
			if (!pFile->Write(m_pKeyFrames, m_nNumKeys * sizeof (A3DQUATERNION), &dwWrite))
				return false;
		}
	}
	else if (m_qCompFmt == AQUAT_COMPFMT_NO_W)
	{
		
		TrackDataCompression::Float3* f3Data = new TrackDataCompression::Float3[m_nNumKeys];
		for(int i=0; i<m_nNumKeys; ++i)
		{
			f3Data[i].FromQuat(m_pKeyFrames[i]);
		}
		
		//	Write key frame data
		if (!pFile->Write(f3Data, m_nNumKeys * sizeof (TrackDataCompression::Float3), &dwWrite))
		{
			delete f3Data;
			return false;
		}

		delete f3Data;

	}

	return true;
}

