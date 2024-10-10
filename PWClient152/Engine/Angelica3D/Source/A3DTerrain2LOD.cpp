/*
 * FILE: A3DTerrain2LOD.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/7/9
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "A3DTerrain2LOD.h"
#include "A3DPI.h"
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


///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Implement A3DTerrain2LOD
//	
///////////////////////////////////////////////////////////////////////////

A3DTerrain2LOD::A3DTerrain2LOD()
{
	m_iBlockGrid	= 0;
	m_iVertPitch	= 0;
	m_aIndexMaps	= NULL;
	m_aGrids		= NULL;
	m_iRefCnt		= 1;

	memset(m_aLevels, 0, sizeof (m_aLevels));
}

A3DTerrain2LOD::~A3DTerrain2LOD()
{
}

//	Intialize object
bool A3DTerrain2LOD::Init(int iBlockGrid)
{
	ASSERT(iBlockGrid >= 8);

	m_iBlockGrid = iBlockGrid;
	m_iVertPitch = iBlockGrid + 1;

	//	Create index maps
	if (!CreateIndexMaps())
	{
		g_A3DErrLog.Log("A3DTerrain2LOD::Init, Failed to create index maps");
		return false;
	}

	bool bRet = true;

	//	Create indices
	for (int i=0; i < NUM_LODLEVEL; i++)
	{
		bRet &= CreateCenterIndices(i+1);
		bRet &= CreateLeftIndices(i+1);
		bRet &= CreateTopIndices(i+1);
		bRet &= CreateRightIndices(i+1);
		bRet &= CreateBottomIndices(i+1);

		//	Vertex number of each level
		int iTemp = (iBlockGrid >> i) + 1;
		m_aLevels[NUM_LODLEVEL-i-1].iNumVert = iTemp * iTemp;
	}

	if (!bRet)
	{
		g_A3DErrLog.Log("A3DTerrain2LOD::Init, Failed to create indices");
		return false;
	}

	//	Create block grids
	if (!CreateGrids())
	{
		g_A3DErrLog.Log("A3DTerrain2LOD::Init, Failed to create grids");
		return false;
	}

	return true;
}

//	Release object
void A3DTerrain2LOD::Release()
{
	//	Release all level indices
	for (int i=0; i < NUM_LODLEVEL; i++)
	{
		LODLEVEL& Level = m_aLevels[i];

		for (int j=0; j < 5; j++)
		{
			if (Level.aOrigin[j].aIndices)
			{
				delete [] Level.aOrigin[j].aIndices;
				delete [] Level.aOrigin[j].aMappedIndices;
			}

			if (Level.aDescend1[j].aIndices)
			{
				delete [] Level.aDescend1[j].aIndices;
				delete [] Level.aDescend1[j].aMappedIndices;
			}

			if (Level.aDescend2[j].aIndices)
			{
				delete [] Level.aDescend2[j].aIndices;
				delete [] Level.aDescend2[j].aMappedIndices;
			}
		}
	}

	memset(m_aLevels, 0, sizeof (m_aLevels));

	//	Release index maps
	if (m_aIndexMaps)
	{
		delete [] m_aIndexMaps;
		m_aIndexMaps = NULL;
	}

	//	Release grids
	if (m_aGrids)
	{
		delete [] m_aGrids;
		m_aGrids = NULL;
	}
}

/*	Get index stream

	iLevel: level of LOD, [1, NUM_LODLEVEL].
	iDir: direction defined in A3DTerrain2LOD.h
	iDescend: descend level [1, NUM_LODLEVEL-1]
	piNumIdx (out): used to receive index number, cannot be NULL.
	bMapped: true, get mapped indices; false, get original indices
*/
WORD* A3DTerrain2LOD::GetIndexStream(int iLevel, int iDir, int iDescend, int* piNumIdx, bool bMapped)
{
	ASSERT(iLevel >= 0 && iLevel <= NUM_LODLEVEL);
	ASSERT(iDir >= 0 && iDir < 5);

	LODLEVEL& LevelData = m_aLevels[iLevel-1];
	INDEXSTRM* pIdxStream;

	switch (iDescend)
	{
	case 0:	pIdxStream = &LevelData.aOrigin[iDir];		break;
	case 1:	pIdxStream = &LevelData.aDescend1[iDir];	break;
	case 2:	pIdxStream = &LevelData.aDescend2[iDir];	break;
	default:
		ASSERT(0);
		return NULL;
	}

	*piNumIdx = pIdxStream->iNumIndex;

	return bMapped ? pIdxStream->aMappedIndices : pIdxStream->aIndices;
}

//	Create center indices for specified LOD level
bool A3DTerrain2LOD::CreateCenterIndices(int iLevel)
{
	int iStepSize = 1 << (NUM_LODLEVEL - iLevel);
	int iNumStep = (m_iBlockGrid - iStepSize * 2) / iStepSize;
	int i, j, v1, v2, iCount;

	//	Original indices ...
	INDEXSTRM& OriginStrm = m_aLevels[iLevel-1].aOrigin[D_CENTER];
	OriginStrm.iNumIndex = iNumStep * iNumStep * 6;

	WORD* aIndices = new WORD [OriginStrm.iNumIndex];
	if (!aIndices)
		return false;

	iCount = 0;

	for (i=0; i < iNumStep; i++)
	{
		v1 = m_iVertPitch * (i + 1) * iStepSize + iStepSize;
		v2 = v1 + m_iVertPitch * iStepSize;

		for (j=0; j < iNumStep; j++)
		{
			aIndices[iCount++] = v1;
			aIndices[iCount++] = v1 + iStepSize;
			aIndices[iCount++] = v2 + iStepSize;

			aIndices[iCount++] = v1;
			aIndices[iCount++] = v2 + iStepSize;
			aIndices[iCount++] = v2;

			v1 += iStepSize;
			v2 += iStepSize;
		}
	}

	OriginStrm.aIndices = aIndices;
	OriginStrm.aMappedIndices = CreateMappedIndices(aIndices, OriginStrm.iNumIndex);

	//	Descend indices ...
	//	For center area, there are no descend indices
	m_aLevels[iLevel-1].aDescend1[D_CENTER].aIndices		= NULL;
	m_aLevels[iLevel-1].aDescend1[D_CENTER].aMappedIndices	= NULL;
	m_aLevels[iLevel-1].aDescend1[D_CENTER].iNumIndex		= 0;
	m_aLevels[iLevel-1].aDescend2[D_CENTER].aIndices		= NULL;
	m_aLevels[iLevel-1].aDescend2[D_CENTER].aMappedIndices	= NULL;
	m_aLevels[iLevel-1].aDescend2[D_CENTER].iNumIndex		= 0;

	return true;
}

//	Create left boder indices for specified LOD level
bool A3DTerrain2LOD::CreateLeftIndices(int iLevel)
{
	int iStepSize = 1 << (NUM_LODLEVEL - iLevel);
	int iNumStep = (m_iBlockGrid - iStepSize * 2) / iStepSize;
	int i, v1, v2, iCount;

	//	Original indices ...
	INDEXSTRM& OriginStrm = m_aLevels[iLevel-1].aOrigin[D_LEFT];
	OriginStrm.iNumIndex = (iNumStep+1) * 6;

	WORD* aIndices = new WORD [OriginStrm.iNumIndex];
	if (!aIndices)
		return false;

	iCount = 0;

	//	Left-top corner
	aIndices[iCount+0]	= 0;
	aIndices[iCount+1]	= iStepSize * m_iVertPitch + iStepSize;
	aIndices[iCount+2]	= iStepSize * m_iVertPitch;
	iCount += 3;

	//	Left-bottom corner
	aIndices[iCount+0]	= (m_iBlockGrid - iStepSize) * m_iVertPitch;
	aIndices[iCount+1]	= aIndices[iCount+0] + iStepSize;
	aIndices[iCount+2]	= aIndices[iCount+0] + iStepSize * m_iVertPitch;
	iCount += 3;

	//	Left border
	v1 = m_iVertPitch * iStepSize;
	v2 = v1 + m_iVertPitch * iStepSize;

	for (i=0; i < iNumStep; i++)
	{
		aIndices[iCount++] = v1;
		aIndices[iCount++] = v1 + iStepSize;
		aIndices[iCount++] = v2 + iStepSize;

		aIndices[iCount++] = v1;
		aIndices[iCount++] = v2 + iStepSize;
		aIndices[iCount++] = v2;

		v1 += m_iVertPitch * iStepSize;
		v2 += m_iVertPitch * iStepSize;
	}

	OriginStrm.aIndices = aIndices;
	OriginStrm.aMappedIndices = CreateMappedIndices(aIndices, OriginStrm.iNumIndex);

	//	Descend indices ...
	for (int d=1; d <= 2; d++)
	{
		INDEXSTRM* pDescStrm = NULL;

		switch (d)
		{
		case 1:	pDescStrm = &m_aLevels[iLevel-1].aDescend1[D_LEFT];	break;
		case 2: pDescStrm = &m_aLevels[iLevel-1].aDescend2[D_LEFT];	break;
		}
	
		iStepSize *= 2;
		iNumStep = m_iBlockGrid / iStepSize;

		//	Level 1 doesn't has descend left border
		if (iLevel - d <= 0)
		{
			pDescStrm->aIndices	 = NULL;
			pDescStrm->iNumIndex = 0;
			continue;
		}

		pDescStrm->iNumIndex = iNumStep * 3 + (iNumStep - 1) * 6;
		if (!(aIndices = new WORD [pDescStrm->iNumIndex]))
			return false;

		int iPitch  = (iStepSize >> 1) * m_iVertPitch + (iStepSize >> 1);
		int iPitch2 = (iStepSize >> 1);

		v1 = 0;
		v2 = v1 + m_iVertPitch * iStepSize;
		iCount = 0;

		for (i=0; i < iNumStep; i++)
		{
			aIndices[iCount++] = v1;
			aIndices[iCount++] = v1 + iPitch;
			aIndices[iCount++] = v2;

			if (i == iNumStep - 1)
				break;

			aIndices[iCount++] = v1 + iPitch;
			aIndices[iCount++] = v2 + iPitch2;
			aIndices[iCount++] = v2;

			aIndices[iCount++] = v2;
			aIndices[iCount++] = v2 + iPitch2;
			aIndices[iCount++] = v2 + iPitch;
			
			v1 += m_iVertPitch * iStepSize;
			v2 += m_iVertPitch * iStepSize;
		}

		pDescStrm->aIndices = aIndices;
		pDescStrm->aMappedIndices = CreateMappedIndices(aIndices, pDescStrm->iNumIndex);
	}

	return true;
}

//	Create top border indices for specified LOD level
bool A3DTerrain2LOD::CreateTopIndices(int iLevel)
{
	int iStepSize = 1 << (NUM_LODLEVEL - iLevel);
	int iNumStep = (m_iBlockGrid - iStepSize * 2) / iStepSize;
	int i, v1, v2, iCount;

	//	Original indices ...
	INDEXSTRM& OriginStrm = m_aLevels[iLevel-1].aOrigin[D_TOP];
	OriginStrm.iNumIndex = (iNumStep+1) * 6;

	WORD* aIndices = new WORD [OriginStrm.iNumIndex];
	if (!aIndices)
		return false;

	iCount = 0;

	//	Left-top corner
	aIndices[iCount+0]	= 0;
	aIndices[iCount+1]	= iStepSize;
	aIndices[iCount+2]	= iStepSize * m_iVertPitch + iStepSize;
	iCount += 3;

	//	right-top corner
	aIndices[iCount+0]	= m_iBlockGrid - iStepSize;
	aIndices[iCount+1]	= aIndices[iCount+0] + iStepSize;
	aIndices[iCount+2]	= aIndices[iCount+0] + iStepSize * m_iVertPitch;
	iCount += 3;

	//	Top border
	v1 = iStepSize;
	v2 = v1 + m_iVertPitch * iStepSize;

	for (i=0; i < iNumStep; i++)
	{
		aIndices[iCount++] = v1;
		aIndices[iCount++] = v1 + iStepSize;
		aIndices[iCount++] = v2 + iStepSize;

		aIndices[iCount++] = v1;
		aIndices[iCount++] = v2 + iStepSize;
		aIndices[iCount++] = v2;

		v1 += iStepSize;
		v2 += iStepSize;
	}

	OriginStrm.aIndices = aIndices;
	OriginStrm.aMappedIndices = CreateMappedIndices(aIndices, OriginStrm.iNumIndex);

	//	Descend indices ...
	for (int d=1; d <= 2; d++)
	{
		INDEXSTRM* pDescStrm;

		switch (d)
		{
		case 1:	pDescStrm = &m_aLevels[iLevel-1].aDescend1[D_TOP];	break;
		case 2: pDescStrm = &m_aLevels[iLevel-1].aDescend2[D_TOP];	break;
		}
	
		iStepSize *= 2;
		iNumStep = m_iBlockGrid / iStepSize;

		//	Level 1 doesn't has descend left border
		if (iLevel - d <= 0)
		{
			pDescStrm->aIndices	 = NULL;
			pDescStrm->iNumIndex = 0;
			continue;
		}

		pDescStrm->iNumIndex = iNumStep * 3 + (iNumStep - 1) * 6;
		if (!(aIndices = new WORD [pDescStrm->iNumIndex]))
			return false;

		int iPitch  = (iStepSize >> 1) * m_iVertPitch + (iStepSize >> 1);
		int iPitch2 = (iStepSize >> 1) * m_iVertPitch;

		v1 = 0;
		v2 = iStepSize;
		iCount = 0;

		for (i=0; i < iNumStep; i++)
		{
			aIndices[iCount++] = v1;
			aIndices[iCount++] = v2;
			aIndices[iCount++] = v1 + iPitch;

			if (i == iNumStep - 1)
				break;

			aIndices[iCount++] = v2;
			aIndices[iCount++] = v2 + iPitch2;
			aIndices[iCount++] = v1 + iPitch;

			aIndices[iCount++] = v2;
			aIndices[iCount++] = v2 + iPitch;
			aIndices[iCount++] = v2 + iPitch2;
			
			v1 += iStepSize;
			v2 += iStepSize;
		}

		pDescStrm->aIndices = aIndices;
		pDescStrm->aMappedIndices = CreateMappedIndices(aIndices, pDescStrm->iNumIndex);
	}

	return true;
}

//	Create right border indices for specified LOD level
bool A3DTerrain2LOD::CreateRightIndices(int iLevel)
{
	int iStepSize = 1 << (NUM_LODLEVEL - iLevel);
	int iNumStep = (m_iBlockGrid - iStepSize * 2) / iStepSize;
	int i, v1, v2, iCount;

	//	Original indices ...
	INDEXSTRM& OriginStrm = m_aLevels[iLevel-1].aOrigin[D_RIGHT];
	OriginStrm.iNumIndex = (iNumStep+1) * 6;

	WORD* aIndices = new WORD [OriginStrm.iNumIndex];
	if (!aIndices)
		return false;

	iCount = 0;

	//	right-top corner
	aIndices[iCount+0]	= m_iBlockGrid;
	aIndices[iCount+1]	= aIndices[iCount+0] + iStepSize * m_iVertPitch;
	aIndices[iCount+2]	= aIndices[iCount+0] + iStepSize * m_iVertPitch - iStepSize;
	iCount += 3;

	//	right-bottom corner
	aIndices[iCount+0]	= (m_iBlockGrid - iStepSize) * m_iVertPitch + m_iBlockGrid - iStepSize;
	aIndices[iCount+1]	= aIndices[iCount+0] + iStepSize;
	aIndices[iCount+2]	= aIndices[iCount+1] + iStepSize * m_iVertPitch;
	iCount += 3;

	//	Right border
	v1 = m_iVertPitch * iStepSize + m_iBlockGrid - iStepSize;
	v2 = v1 + m_iVertPitch * iStepSize;

	for (i=0; i < iNumStep; i++)
	{
		aIndices[iCount++] = v1;
		aIndices[iCount++] = v1 + iStepSize;
		aIndices[iCount++] = v2 + iStepSize;

		aIndices[iCount++] = v1;
		aIndices[iCount++] = v2 + iStepSize;
		aIndices[iCount++] = v2;

		v1 += m_iVertPitch * iStepSize;
		v2 += m_iVertPitch * iStepSize;
	}

	OriginStrm.aIndices = aIndices;
	OriginStrm.aMappedIndices = CreateMappedIndices(aIndices, OriginStrm.iNumIndex);

	//	Descend indices ...
	for (int d=1; d <= 2; d++)
	{
		INDEXSTRM* pDescStrm;

		switch (d)
		{
		case 1:	pDescStrm = &m_aLevels[iLevel-1].aDescend1[D_RIGHT];	break;
		case 2: pDescStrm = &m_aLevels[iLevel-1].aDescend2[D_RIGHT];	break;
		}
	
		iStepSize *= 2;
		iNumStep = m_iBlockGrid / iStepSize;

		//	Level 1 doesn't has descend left border
		if (iLevel - d <= 0)
		{
			pDescStrm->aIndices	 = NULL;
			pDescStrm->iNumIndex = 0;
			continue;
		}

		pDescStrm->iNumIndex = iNumStep * 3 + (iNumStep - 1) * 6;
		if (!(aIndices = new WORD [pDescStrm->iNumIndex]))
			return false;

		int iPitch  = (iStepSize >> 1) * m_iVertPitch - (iStepSize >> 1);
		int iPitch2 = -(iStepSize >> 1);

		v1 = m_iBlockGrid;
		v2 = v1 + m_iVertPitch * iStepSize;
		iCount = 0;

		for (i=0; i < iNumStep; i++)
		{
			aIndices[iCount++] = v1;
			aIndices[iCount++] = v2;
			aIndices[iCount++] = v1 + iPitch;

			if (i == iNumStep - 1)
				break;

			aIndices[iCount++] = v1 + iPitch;
			aIndices[iCount++] = v2;
			aIndices[iCount++] = v2 + iPitch2;
		
			aIndices[iCount++] = v2 + iPitch2;
			aIndices[iCount++] = v2;
			aIndices[iCount++] = v2 + iPitch;

			v1 += m_iVertPitch * iStepSize;
			v2 += m_iVertPitch * iStepSize;
		}

		pDescStrm->aIndices = aIndices;
		pDescStrm->aMappedIndices = CreateMappedIndices(aIndices, pDescStrm->iNumIndex);
	}

	return true;
}

//	Create bottom border indices for specified LOD level
bool A3DTerrain2LOD::CreateBottomIndices(int iLevel)
{
	int iStepSize = 1 << (NUM_LODLEVEL - iLevel);
	int iNumStep = (m_iBlockGrid - iStepSize * 2) / iStepSize;
	int i, v1, v2, iCount;

	//	Original indices ...
	INDEXSTRM& OriginStrm = m_aLevels[iLevel-1].aOrigin[D_BOTTOM];
	OriginStrm.iNumIndex = (iNumStep+1) * 6;

	WORD* aIndices = new WORD [OriginStrm.iNumIndex];
	if (!aIndices)
		return false;

	iCount = 0;

	//	Left-bottom corner
	aIndices[iCount+0]	= m_iBlockGrid * m_iVertPitch;
	aIndices[iCount+1]	= aIndices[iCount+0] - iStepSize * m_iVertPitch + iStepSize;
	aIndices[iCount+2]	= aIndices[iCount+0] + iStepSize;
	iCount += 3;

	//	right-bottom corner
	aIndices[iCount+0]	= m_iBlockGrid * m_iVertPitch + m_iBlockGrid - iStepSize;
	aIndices[iCount+1]	= aIndices[iCount+0] - iStepSize * m_iVertPitch;
	aIndices[iCount+2]	= aIndices[iCount+0] + iStepSize;
	iCount += 3;

	//	Top border
	v1 = (m_iBlockGrid - iStepSize) * m_iVertPitch + iStepSize;
	v2 = v1 + m_iVertPitch * iStepSize;

	for (i=0; i < iNumStep; i++)
	{
		aIndices[iCount++] = v1;
		aIndices[iCount++] = v1 + iStepSize;
		aIndices[iCount++] = v2 + iStepSize;

		aIndices[iCount++] = v1;
		aIndices[iCount++] = v2 + iStepSize;
		aIndices[iCount++] = v2;

		v1 += iStepSize;
		v2 += iStepSize;
	}

	OriginStrm.aIndices = aIndices;
	OriginStrm.aMappedIndices = CreateMappedIndices(aIndices, OriginStrm.iNumIndex);

	//	Descend indices ...
	for (int d=1; d <= 2; d++)
	{
		INDEXSTRM* pDescStrm;

		switch (d)
		{
		case 1:	pDescStrm = &m_aLevels[iLevel-1].aDescend1[D_BOTTOM];	break;
		case 2: pDescStrm = &m_aLevels[iLevel-1].aDescend2[D_BOTTOM];	break;
		}
	
		iStepSize *= 2;
		iNumStep = m_iBlockGrid / iStepSize;

		//	Level 1 doesn't has descend left border
		if (iLevel - d <= 0)
		{
			pDescStrm->aIndices	 = NULL;
			pDescStrm->iNumIndex = 0;
			continue;
		}
	
		pDescStrm->iNumIndex = iNumStep * 3 + (iNumStep - 1) * 6;
		if (!(aIndices = new WORD [pDescStrm->iNumIndex]))
			return false;

		int iPitch  = (iStepSize >> 1) * m_iVertPitch - (iStepSize >> 1);
		int iPitch2 = (iStepSize >> 1);

		v1 = (m_iBlockGrid - (iStepSize >> 1)) * m_iVertPitch + (iStepSize >> 1);
		v2 = v1 + iStepSize;
		iCount = 0;

		for (i=0; i < iNumStep; i++)
		{
			aIndices[iCount++] = v1;
			aIndices[iCount++] = v2 + iPitch;
			aIndices[iCount++] = v1 + iPitch;

			if (i == iNumStep - 1)
				break;

			aIndices[iCount++] = v1;
			aIndices[iCount++] = v1 + iPitch2;
			aIndices[iCount++] = v2 + iPitch;
		
			aIndices[iCount++] = v1 + iPitch2;
			aIndices[iCount++] = v2;
			aIndices[iCount++] = v2 + iPitch;
			
			v1 += iStepSize;
			v2 += iStepSize;
		}

		pDescStrm->aIndices = aIndices;
		pDescStrm->aMappedIndices = CreateMappedIndices(aIndices, pDescStrm->iNumIndex);
	}

	return true;
}

//	Create index maps
bool A3DTerrain2LOD::CreateIndexMaps()
{
	if (!(m_aIndexMaps = new WORD [m_iVertPitch * m_iVertPitch]))
	{
		g_A3DErrLog.Log("A3DTerrain2LOD::CreateIndexMaps, Not enough memory !");
		return false;
	}

	int r, c, iStepSize = 1 << (NUM_LODLEVEL-1);
	int iIndex, iCount = 0;

	//	Index maps of level 1
	for (r=0; r <= m_iBlockGrid; r+=iStepSize)
	{
		for (c=0; c <= m_iBlockGrid; c+=iStepSize)
		{
			iIndex = r * m_iVertPitch + c;
			m_aIndexMaps[iIndex] = iCount++;
		}
	}

	//	Index maps of level 2 and level 1
	for (int i=0; i < 2; i++)
	{
		iStepSize /= 2;
		bool bSkip = true;

		for (r=0; r <= m_iBlockGrid; r+=iStepSize)
		{
			if (bSkip)
			{
				for (c=iStepSize; c < m_iBlockGrid; c+=iStepSize*2)
				{
					iIndex = r * m_iVertPitch + c;
					m_aIndexMaps[iIndex] = iCount++;
				}
			}
			else
			{
				for (c=0; c <= m_iBlockGrid; c+=iStepSize)
				{
					iIndex = r * m_iVertPitch + c;
					m_aIndexMaps[iIndex] = iCount++;
				}
			}

			bSkip = !bSkip;
		}
	}

	ASSERT(iCount == m_iVertPitch * m_iVertPitch);

	return true;
}

/*	Create mapped indices

	Return mapped indices buffer for success, otherwise return NULL

	aOrigin: original indices.
	iNumIndex: number index
*/
WORD* A3DTerrain2LOD::CreateMappedIndices(const WORD* aOrigin, int iNumIndex)
{
	if (!aOrigin || !iNumIndex)
		return NULL;

	ASSERT(m_aIndexMaps);

	WORD* aDest = new WORD [iNumIndex];
	if (!aDest)
	{
		g_A3DErrLog.Log("A3DTerrain2LOD::CreateMappedIndices, Not enough memory !");
		return NULL;
	}

	for (int i=0; i < iNumIndex; i++)
		aDest[i] = m_aIndexMaps[aOrigin[i]];

	return aDest;
}

//	Create block grids
bool A3DTerrain2LOD::CreateGrids()
{
	int iNumGrid = m_iBlockGrid * m_iBlockGrid;

	if (!(m_aGrids = new GRID [iNumGrid]))
	{
		g_A3DErrLog.Log("A3DTerrain2LOD::CreateGrids, Not enough memory !");
		return false;
	}

	int iCount = 0;

	for (int r=0; r < m_iBlockGrid; r++)
	{
		int iBase = r * m_iVertPitch;

		for (int c=0; c < m_iBlockGrid; c++, iCount++)
		{
			GRID& Grid = m_aGrids[iCount];
			int v0 = iBase + c;

			//	v3  v0----v1
			//	| \  \    |	
			//	|  \  \   |	
			//	|   \  \  |	
			//	|	 \  \ |	
			//	v5---v4  v2
			Grid.v0 = v0;
			Grid.v1 = v0 + 1;
			Grid.v2 = v0 + m_iVertPitch + 1;

			Grid.v3 = v0;
			Grid.v4 = v0 + m_iVertPitch + 1;
			Grid.v5 = v0 + m_iVertPitch;
		}
	}

	//	Change grids on right-top corner
	//	v0----v1 v3
	//	|    /  / |	
	//	|   /  /  |	
	//	|  /  /   |	
	//	| /  /    |	
	//	v2  v5----v4
	GRID* pGrid = &m_aGrids[m_iBlockGrid-1];
	iCount = m_iBlockGrid - 1;
	pGrid->v0 = iCount;
	pGrid->v1 = iCount + 1;
	pGrid->v2 = iCount + m_iVertPitch;

	pGrid->v3 = iCount + 1;
	pGrid->v4 = iCount + m_iVertPitch + 1;
	pGrid->v5 = iCount + m_iVertPitch;

	//	Change grids on left-bottom corner
	//	v0----v1 v3
	//	|    /  / |	
	//	|   /  /  |	
	//	|  /  /   |	
	//	| /  /    |	
	//	v2  v5----v4
	pGrid = &m_aGrids[m_iBlockGrid * (m_iBlockGrid-1)];
	iCount = (m_iBlockGrid - 1) * m_iVertPitch;
	pGrid->v0 = iCount;
	pGrid->v1 = iCount + 1;
	pGrid->v2 = iCount + m_iVertPitch;

	pGrid->v3 = iCount + 1;
	pGrid->v4 = iCount + m_iVertPitch + 1;
	pGrid->v5 = iCount + m_iVertPitch;

	//	Remap all indices
	for (int i=0; i < iNumGrid; i++)
	{
		GRID& Grid = m_aGrids[i];
		Grid.v0 = m_aIndexMaps[Grid.v0];
		Grid.v1 = m_aIndexMaps[Grid.v1];
		Grid.v2 = m_aIndexMaps[Grid.v2];
		Grid.v3 = m_aIndexMaps[Grid.v3];
		Grid.v4 = m_aIndexMaps[Grid.v4];
		Grid.v5 = m_aIndexMaps[Grid.v5];
	}

	return true;
}

