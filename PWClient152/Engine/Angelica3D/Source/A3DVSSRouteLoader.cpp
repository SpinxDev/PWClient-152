/*
 * FILE: A3DVSSRouteLoader.cpp
 *
 * DESCRIPTION: Loader of route VSS data created by CVSSetRoute
 *
 * CREATED BY: duyuxin, 2002/7/20
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#include "A3DTypes.h"
#include "A3DPI.h"
#include "A3DVSSCommon.h"
#include "A3DVSSRouteLoader.h"

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
//	Implement of A3DVSSRouteLoader
//
///////////////////////////////////////////////////////////////////////////

A3DVSSRouteLoader::A3DVSSRouteLoader()
{
	m_bReady		= false;
	m_bLoadAllOnce	= false;
	m_pFile			= NULL;
	m_dwRouteID		= 0;
	m_aSections		= NULL;
	m_iNumSection	= 0;
	m_iNumVisByte	= 0;
	m_iLastSection	= 0;
	m_aCompVisBuf	= NULL;
	m_iNumObject	= 0;
	m_iNumFace		= 0;
}

/*	Load route vss from file

	Return true for success, otherwise return false.

	szFileName: Route vss data file's name
	bAll: Load all vss data at once
*/
bool A3DVSSRouteLoader::Load(char* szFileName, bool bAll)
{
	if (m_bReady)
	{
		g_A3DErrLog.Log("A3DVSSRouteLoader::Load, one route has been loaded.");
		return false;
	}

	if (!szFileName || !szFileName[0])
	{
		g_A3DErrLog.Log("A3DVSSRouteLoader::Load, Invalid file name.");
		return false;
	}

	FILE* fp = fopen(szFileName, "rb");
	if (!fp)
	{
		g_A3DErrLog.Log("A3DVSSRouteLoader::Load, Failed to open file %s.", szFileName);
		return false;
	}

	//	Read file header
	VSSFILEHEADER Header;
	fread(&Header, 1, sizeof (Header), fp);

	if (Header.dwVersion != VSSFILE_VERSION || Header.dwIdentify != VSSFILE_IDENTIFY)
	{
		fclose(fp);
		g_A3DErrLog.Log("A3DVSSRouteLoader::Load, File version or format is wrong.");
		return false;
	}

	m_iNumSection	= Header.iNumSection;
	m_dwRouteID		= Header.dwRouteID;
	m_iNumVisByte	= Header.iVisBufLen;
	m_bLoadAllOnce	= bAll;
	m_iNumObject	= Header.iNumObject;
	m_iNumFace		= Header.iNumFace;

	//	Read route information
	fread(&m_Params, 1, sizeof (m_Params), fp);

	//	Read section information table
	VSSSECTIONINFO* aInfos = new VSSSECTIONINFO [m_iNumSection];
	if (!aInfos)
	{
		fclose(fp);
		g_A3DErrLog.Log("A3DVSSRouteLoader::Load, Not enough memory for section information table.");
		return false;
	}

	fread(aInfos, 1, sizeof (VSSSECTIONINFO) * m_iNumSection, fp);

	int i, iTotalSize = 0;

	//	Allocate section buffer
	if (!(m_aSections = new SECTION [m_iNumSection]))
	{
		g_A3DErrLog.Log("A3DVSSRouteLoader::Load, Not enough memory for section information table.");
		goto Error;
	}

	for (i=0; i < m_iNumSection; i++)
	{
		SECTION* pSection = &m_aSections[i];

		pSection->iStartCnt		= aInfos[i].iStartCnt;
		pSection->iEndCnt		= aInfos[i].iEndCnt;
		pSection->iNumVisible	= aInfos[i].iNumVisible;
		pSection->dwOff			= aInfos[i].dwOff;
		pSection->iVisLen		= aInfos[i].iVisLen;
		pSection->aVisBits		= NULL;

		iTotalSize += aInfos[i].iVisLen;

	#if VSSFILE_VERSION >= 3

		pSection->iNumVisObj = aInfos[i].iNumVisObj;
		iTotalSize += aInfos[i].iNumVisObj * sizeof (int);

	#endif
	}

	delete [] aInfos;
	aInfos = NULL;

	if (bAll)	//	Load all section vss data
	{
		if (!(m_aCompVisBuf = new BYTE [iTotalSize]))
		{
			g_A3DErrLog.Log("A3DVSSRouteLoader::Load, Not enough memory for compressed vss data.");
			goto Error;
		}

		int iMemOff=0, iMemLen;

		for (i=0; i < m_iNumSection; i++)
		{
			SECTION* pSection = &m_aSections[i];
			iMemLen = pSection->iVisLen;

		#if VSSFILE_VERSION >= 3

			iMemLen += pSection->iNumVisObj * sizeof (int);

		#endif

			pSection->aVisBits = m_aCompVisBuf + iMemOff;

			fseek(fp, pSection->dwOff, SEEK_SET);
			fread(m_aCompVisBuf+iMemOff, 1, iMemLen, fp);

			iMemOff += iMemLen;
		}

		fclose(fp);
		m_pFile = NULL;
	}
	else
		m_pFile = fp;

	m_bReady		= true;
	m_iLastSection	= 0;

	return true;

Error:

	if (aInfos)
		delete [] aInfos;

	Release();

	fclose(fp);
	return false;
}

/*	Load route vss from file. This function use AFile object to load all vss data
	at once.

	Return true for success, otherwise return false.

	pFile: AFile object's pointer
*/
bool A3DVSSRouteLoader::Load(AFile* pFile)
{
	return true;
}

/*	Load route vss from memory file.

	Return ture for success, otherwise return false.

	pBuf: buffer which contain vss data. data format of pBuf should be the same
		  as vss file.
*/
bool A3DVSSRouteLoader::Load(BYTE* pBuf)
{
	if (m_bReady)
	{
		g_A3DErrLog.Log("A3DVSSRouteLoader::Load, one route has been loaded.");
		return false;
	}

	BYTE* pOff = pBuf;

	//	Read file header
	VSSFILEHEADER* pHeader = (VSSFILEHEADER*)pOff;
	pOff += sizeof (VSSFILEHEADER);

	if (pHeader->dwVersion != VSSFILE_VERSION || pHeader->dwIdentify != VSSFILE_IDENTIFY)
	{
		g_A3DErrLog.Log("CVSSetRoute::Load, File version or format is wrong.");
		return false;
	}

	m_iNumSection	= pHeader->iNumSection;
	m_dwRouteID		= pHeader->dwRouteID;
	m_iNumVisByte	= pHeader->iVisBufLen;
	m_bLoadAllOnce	= true;
	m_iNumObject	= pHeader->iNumObject;
	m_iNumFace		= pHeader->iNumFace;

	//	Read route information
	memcpy(&m_Params, pOff, sizeof (m_Params));
	pOff += sizeof (m_Params);

	//	Read section information table
	VSSSECTIONINFO* aInfos = (VSSSECTIONINFO*)pOff;
	pOff += sizeof (VSSSECTIONINFO) * pHeader->iNumSection;

	int i, iMemOff, iTotalSize=0;

	//	Allocate section buffer
	if (!(m_aSections = new SECTION [m_iNumSection]))
	{
		g_A3DErrLog.Log("A3DVSSRouteLoader::Load, Not enough memory for section information table.");
		goto Error;
	}

	for (i=0; i < m_iNumSection; i++)
	{
		SECTION* pSection = &m_aSections[i];

		pSection->iStartCnt		= aInfos[i].iStartCnt;
		pSection->iEndCnt		= aInfos[i].iEndCnt;
		pSection->iNumVisible	= aInfos[i].iNumVisible;
		pSection->dwOff			= aInfos[i].dwOff;
		pSection->iVisLen		= aInfos[i].iVisLen;
		pSection->aVisBits		= NULL;

		iTotalSize += aInfos[i].iVisLen;

	#if VSSFILE_VERSION >= 3

		pSection->iNumVisObj = aInfos[i].iNumVisObj;
		iTotalSize += aInfos[i].iNumVisObj * sizeof (int);

	#endif
	}

	//	Load all section vss data
	if (!(m_aCompVisBuf = new BYTE [iTotalSize]))
	{
		g_A3DErrLog.Log("A3DVSSRouteLoader::Load, Not enough memory for compressed vss data.");
		goto Error;
	}

	//	Copy compressed vss data
	memcpy(m_aCompVisBuf, pOff, iTotalSize);
	
	for (i=0, iMemOff=0; i < m_iNumSection; i++)
	{
		SECTION* pSection = &m_aSections[i];
		int iMemLen = pSection->iVisLen;

	#if VSSFILE_VERSION >= 3

		iMemLen += pSection->iNumVisObj * sizeof (int);

	#endif

		pSection->aVisBits = m_aCompVisBuf + iMemOff;
		iMemOff += iMemLen;
	}
	
	m_bReady		= true;
	m_iLastSection	= 0;

	return true;

Error:

	Release();
	return false;
}

//	Release resources
void A3DVSSRouteLoader::Release()
{
	if (m_bLoadAllOnce)
	{
		if (m_aCompVisBuf)
		{
			delete [] m_aCompVisBuf;
			m_aCompVisBuf = NULL;
		}
	}
	else
	{
		if (m_pFile)
		{
			fclose(m_pFile);
			m_pFile = NULL;
		}
	}

	if (m_aSections)
	{
		delete [] m_aSections;
		m_aSections = NULL;
	}

	m_bReady = false;
}

/*	Get section to which specified sample point blongs

	Return section index for success, otherwise return -1.

	iSample: sample point's index.
*/
int	A3DVSSRouteLoader::GetSectionOfSample(int iSample)
{
	if (!m_bReady)
		return -1;

	if (iSample < 0 || iSample > m_Params.iMaxCount)
		return -1;

	if (iSample < m_aSections[m_iLastSection].iStartCnt)
	{
		for (int i=m_iLastSection-1; i >= 0 ; i--)
		{
			if (iSample >= m_aSections[i].iStartCnt &&
				iSample <= m_aSections[i].iEndCnt)
				return m_iLastSection = i;
		}
	}
	else if (iSample > m_aSections[m_iLastSection].iEndCnt)
	{
		for (int i=m_iLastSection+1; i < m_iNumSection; i++)
		{
			if (iSample >= m_aSections[i].iStartCnt &&
				iSample <= m_aSections[i].iEndCnt)
				return m_iLastSection = i;
		}
	}
	else
		return m_iLastSection;

	return -1;
}

/*	Get specified section's visible surface indices

	Return true for success, otherwise return false.

	iSection: index of section whose vss data will be got
	pBuf (out): buffer used to receive decompressed vss data, the buffer must be
				length enough, cannot be NULL. You can through 
				A3DScene::GetFaceNumber() to get the maximum possible number of
				index which will be pu in this buffer.
	piNumVisSuf (out): used to receive the number of surfaces whom can be seen 
					   in this section, also it's the number of valid index in pBuf.
					   This parameter can be NULL
*/
bool A3DVSSRouteLoader::GetSectionVisSurf(int iSection, int* pBuf, int* piNumVisSurf)
{
	if (!m_bReady)
		return false;

	if (iSection < 0 || iSection >= m_iNumSection)
		return false;

	SECTION* pSection = &m_aSections[iSection];

	//	Decode vss data
	DecodeVSS(pSection->aVisBits, pSection->iVisLen, pBuf);

	if (piNumVisSurf)
		*piNumVisSurf = pSection->iNumVisible;

	return true;
}

/*	Get visible object's ID of specified section

	Return true for success, otherwise return false.

	iSection: index of section whose vss data will be got
	pBuf (out): buffer used to receive visible object's ID, the buffer must be
				length enough, cannot be NULL. You can through
				A3DVSSRouteLoader::GetObjectNum() to get the maximum possible 
				number of ID will be put in this buffer.
	piNumVisObj (out): used to receive the number of object ID that can be seen 
					   in this section, also it's the number of valid ID in pBuf.
					   This parameter can be NULL
*/
bool A3DVSSRouteLoader::GetSectionVisObjs(int iSection, int* pBuf, int* piNumVisObj)
{
#if VSSFILE_VERSION < 3

	if (piNumVisObj)
		*piNumVisObj = 0;

	return false;

#endif

	if (!m_bReady)
		return false;

	if (iSection < 0 || iSection >= m_iNumSection)
		return false;

	SECTION* pSection = &m_aSections[iSection];

	if (pSection->iNumVisObj)
		memcpy(pBuf, pSection->aVisBits + pSection->iVisLen, pSection->iNumVisObj * sizeof (int));

	if (piNumVisObj)
		*piNumVisObj = pSection->iNumVisObj;

	return true;
}

#ifdef COMPRESSED_VSS	//	Compressed VSS

/*	Decode vss data. This function decode source vss data and output indices 
	of visible surfaces to destination buffer.

	Return true for success, otherwise return false.
	
	pSrc: source vss data buffer
	iSrcLen: length of pSrc in bytes
	pDest: destination buffer used to receive visible surface indices
*/
bool A3DVSSRouteLoader::DecodeVSS(BYTE* pSrc, int iSrcLen, int* pDest)
{
	int j, iNumVis, iIndex;
	BYTE* pByte		= pSrc;
	BYTE* pSrcEnd	= pSrc + iSrcLen;
	
	iNumVis	= 0;
	iIndex	= 0;

	while (pByte < pSrcEnd)
	{
		if (!(*pByte))
		{
			pByte++;	//	Skip 0 byte
			iIndex += *pByte++ * 8;
			continue;
		}

		BYTE byByte = *pByte++;
		BYTE byMask = 1;

		for (j=0; j < 8; j++)
		{
			if (byByte & byMask)
				pDest[iNumVis++] = iIndex + j;

			byMask <<= 1;
		}

		iIndex += 8;
	}	

	return true;
}

/*	Decompress section's VSS byte string.

	Return true for success, otherwise return false.

	iSection: index of section whose vss data will be got
	pBuf: buffer used to store decompressed VSS string. This buffer must be
		  length enough (larger or greater than the number returned by
		  A3DVSSRouteLoader::GetVisByteNum().
*/
bool A3DVSSRouteLoader::GetSectionVisBytes(int iSection, BYTE* pBuf)
{
	if (!m_bReady)
		return false;

	if (iSection < 0 || iSection >= m_iNumSection)
		return false;

	SECTION* pSection = &m_aSections[iSection];

	BYTE* pByte		= pSection->aVisBits;
	BYTE* pSrcEnd	= pByte + pSection->iVisLen;

	memset(pBuf, 0, m_iNumVisByte);
	
	while (pByte < pSrcEnd)
	{
		if (!(*pByte))
		{
			pByte++;
			pBuf += *pByte++;
			continue;
		}
		else
			*pBuf++ = *pByte++;
	}	

	return true;
}

#else	//	undefined COMPRESSED_VSS

bool A3DVSSRouteLoader::DecodeVSS(BYTE* pSrc, int iSrcLen, int* pDest)
{
	int i, j, iNumVis, iIndex;
	BYTE* pByte = pSrc;	

	iNumVis	= 0;
	iIndex	= 0;
	
	for (i=0; i < iSrcLen; i++, pByte++)
	{
		if (!(*pByte))
		{
			iIndex += 8;
			continue;
		}

		BYTE byByte = *pByte;
		BYTE byMask = 1;

		for (j=0; j < 8; j++)
		{
			if (byByte & byMask)
				pDest[iNumVis++] = iIndex + j;

			byMask <<= 1;
		}

		iIndex += 8;
	}

	return true;
}

bool A3DVSSRouteLoader::GetSectionVisBytes(int iSection, BYTE* pBuf)
{
	if (!m_bReady)
		return false;

	if (iSection < 0 || iSection >= m_iNumSection)
		return false;

	SECTION* pSection = &m_aSections[iSection];

	memcpy(pBuf, pSection->aVisBits, pSection->iVisLen);

	return true;
}

#endif	//	COMPRESSED_VSS
