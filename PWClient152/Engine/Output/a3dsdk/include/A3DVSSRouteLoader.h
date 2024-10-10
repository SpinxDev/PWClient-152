/*
 * FILE: A3DVSSRouteLoader.h
 *
 * DESCRIPTION: Loader of route VSS data created by CVSSetRoute
 *
 * CREATED BY: duyuxin, 2002/7/20
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#ifndef _A3DVSSROUTELOADER_H_
#define _A3DVSSROUTELOADER_H_

#include "A3DVSSCommon.h"

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

///////////////////////////////////////////////////////////////////////////
//
//	Declare of Global functions
//
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//
//	Class A3DVSSRouteLoader
//
///////////////////////////////////////////////////////////////////////////

class A3DVSSRouteLoader
{
public:		//	Types

public:		//	Constructors and Destructors

	A3DVSSRouteLoader();
	virtual ~A3DVSSRouteLoader() {}

public:		//	Attributes

	typedef struct _SECTION
	{
		int		iStartCnt;		//	Start sampling count of this section
		int		iEndCnt;		//	End sampling count of this section
		int		iNumVisible;	//	Number of visible surface
		DWORD	dwOff;			//	Visible Set's offset in file
		int		iVisLen;		//	Length (in bytes) of compressed visible set
		BYTE*	aVisBits;		//	Surface visible bits
		int		iNumVisObj;		//	Number of visible scene object	

	} SECTION, *PSECTION;

public:		//	Operations

	bool		Load(char* szFileName, bool bAll);		//	Load route vss from file
	bool		Load(AFile* pFile);		//	Load route vss from file
	bool		Load(BYTE* pBuf);		//	Load route vss from memory
	void		Release();				//	Release resources

	int			GetSectionOfSample(int iSample);		//	Get section to which specified sample point blongs
	bool		GetSectionVisSurf(int iSection, int* pBuf, int* piNumVisSurf);	//	Get specified section's visible surface indices
	bool		GetSectionVisBytes(int iSection, BYTE* pBuf);	//	Decompress section's VSS byte string.
	bool		GetSectionVisObjs(int iSection, int* pBuf, int* piNumVisObj);	//	Get visible object's ID of specified section

	DWORD		GetRouteID()		{	return m_dwRouteID;		}
	int			GetSectionNumber()	{	return m_iNumSection;	}
	VSSETPARAMS	GetRouteParams()	{	return m_Params;		}
	int			GetVisByteNum()		{	return m_iNumVisByte;	}
	int			GetObjectNum()		{	return m_iNumObject;	}
	int			GetFaceNum()		{	return m_iNumFace;		}

	inline int	GetFirstSampleOfSection(int iSection);	//	Get the first sample of specified section
	inline int	GetLastSampleOfSection(int iSection);	//	Get the last sample of specified section
	inline int	GetVisibleSurfaceNumber(int iSection);	//	Get visible surface number of specified section

private:	//	Attributes

	bool		m_bReady;			//	Ready to get section's vss data
	DWORD		m_dwRouteID;		//	Route ID
	VSSETPARAMS	m_Params;			//	Route parameters

	bool		m_bLoadAllOnce;		//	true, load all vss data once time
	FILE*		m_pFile;			//	vss data file's pointer
	BYTE*		m_aCompVisBuf;		//	Compressed vss buffer

	SECTION*	m_aSections;		//	Section information array
	int			m_iNumSection;		//	Number of section
	int			m_iNumVisByte;		//	Length of visible buffer in bytes
	int			m_iLastSection;		//	Last section user references
	int			m_iNumObject;		//	Number of scene object
	int			m_iNumFace;			//	Number of scene face

private:	//	Operations

	bool		DecodeVSS(BYTE* pSrc, int iSrcLen, int* pDest);		//	Decode vss data
};

///////////////////////////////////////////////////////////////////////////
//
//	Inline functions
//
///////////////////////////////////////////////////////////////////////////

/*	Get the first sample of specified section

	Return the first sample of specified section for success, otherwise return -1.

	iSection: specified section's index
*/
int	A3DVSSRouteLoader::GetFirstSampleOfSection(int iSection)
{
	if (!m_bReady)
		return -1;

	if (iSection < 0 || iSection >= m_iNumSection)
		return -1;

	return m_aSections[iSection].iStartCnt;
}

/*	Get the last sample of specified section

	Return the last sample of specified section for success, otherwise return -1.

	iSection: specified section's index
*/
int	A3DVSSRouteLoader::GetLastSampleOfSection(int iSection)
{
	if (!m_bReady)
		return -1;

	if (iSection < 0 || iSection >= m_iNumSection)
		return -1;

	return m_aSections[iSection].iEndCnt;
}

/*	Get visible surface number of specified section

	Return visible surface number for success, otherwise return -1.

	iSection: specified section's index
*/
int	A3DVSSRouteLoader::GetVisibleSurfaceNumber(int iSection)
{
	if (!m_bReady)
		return -1;

	if (iSection < 0 || iSection >= m_iNumSection)
		return -1;

	return m_aSections[iSection].iNumVisible;
}


#endif	//	_A3DVSSROUTELOADER_H_

