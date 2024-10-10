/*
 * FILE: A3DBSP.cpp
 *
 * DESCRIPTION: Routines for BSP algorithm and file
 *
 * CREATED BY: duyuxin, 2002/4/1
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "A3DPI.h"
#include "A3DBSP.h"
#include "AFileImage.h"
#include "A3DCollision.h"
#include "A3DFrustum.h"
#include "A3DViewport.h"
#include "A3DCameraBase.h"
#include "AFile.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

//	Leaf index mask
#define MASK_BSPLEAFINDEX		0x3fffffff

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
//	Implement
//
///////////////////////////////////////////////////////////////////////////

A3DBSP::A3DBSP()
{
	m_aNodes		= NULL;	
	m_aLeaves		= NULL;	
	m_aPlanes		= NULL;	
	m_aSurfRefs		= NULL;
	m_aPVS			= NULL;
	m_aClusters		= NULL;

	m_iNumNode		= 0;
	m_iNumLeaf		= 0;
	m_iNumPlane		= 0;
	m_iNumSurfRef	= 0;
	m_iPVSSize		= 0;
	m_iNumCluster	= 0;

	m_bRecursive	= false;
	m_iViewLeaf		= -1;
	m_pFrustum		= NULL;
	m_aSRFlags		= NULL;
	m_aRSurfs		= NULL;
	m_iNumDrawSurf	= 0;
	m_iNumVisSurf	= 0;

	//	Used by quake's recursive mothod when call GetVisibleSurfs()
	m_dwFrameCnt	= 0;

	//	Used by enumerate method when call GetVisibleSurfs()
	m_aFCFlags		= NULL;
	m_aRClusters	= NULL;
	m_iNumRCluster	= 0;

	_m_pCurCluster	= NULL;
	_m_iPVSCnt		= 0;
}

A3DBSP::~A3DBSP()
{
}

/*	Load ESP data from file and initialize object

	Return true for success, otherwise return false.

	szFileName: BSP file's name
*/
bool A3DBSP::Load(char* szFileName)
{
	AFileImage File;

	if (!File.Open(szFileName, AFILE_OPENEXIST))
	{
		File.Close();
		g_A3DErrLog.Log("A3DBSP::Load(), Cannot open file %s!", szFileName);
		return false;
	}

	//	Release old resources
	Release();

	BSPFILEHEADER Header;
	DWORD dwRead;

	//	Read BSP file header
	if (!File.Read(&Header, sizeof (Header), &dwRead) || dwRead != sizeof (Header))
	{
		g_A3DErrLog.Log("A3DBSP::Load(), Failed to read file header");
		File.Close();
		return false;
	}

	//	Check format and version
	if (Header.dwIdentify != BSP_IDENTIFY || Header.dwVersion != BSP_VERSION)
	{
		g_A3DErrLog.Log("A3DBSP::Load(), Wrong file format or version");
		File.Close();
		return false;
	}

	//	Read lumps
	ReadPlaneLump(&File, &Header);		//	Plane lump before node lump
	ReadSurfRefLump(&File, &Header);	//	Draw surface reference lump before leaf lump
	ReadPVSLump(&File, &Header);		//	PVS lump before leaf lump
	ReadNodeLump(&File, &Header);
	ReadLeafLump(&File, &Header);		//	Leaf lump before cluster lump

	File.Close();

	//	Initialize all other resources
	if (!InitOthers())
		return false;

	return true;
}

//	Release all resources
void A3DBSP::Release()
{
	if (m_aNodes)
	{
		free(m_aNodes);
		m_aNodes = NULL;
		m_iNumNode = 0;
	}

	if (m_aLeaves)
	{
		free(m_aLeaves);
		m_aLeaves = NULL;
		m_iNumLeaf = 0;
	}

	if (m_aPlanes)
	{
		free(m_aPlanes);
		m_aPlanes = NULL;
		m_iNumPlane	= 0;
	}

	if (m_aSurfRefs)
	{
		free(m_aSurfRefs);
		m_aSurfRefs	= NULL;
		m_iNumSurfRef = 0;
	}

	if (m_aPVS)
	{
		free(m_aPVS);
		m_aPVS = NULL;
		m_iPVSSize = 0;
	}

	if (m_aClusters)
	{
		free(m_aClusters);
		m_aClusters = NULL;
		m_iNumCluster = 0;
	}

	if (m_aFCFlags)
	{
		free(m_aFCFlags);
		m_aFCFlags = NULL;
	}

	if (m_aSRFlags)
	{
		free(m_aSRFlags);
		m_aSRFlags = NULL;
	}

	if (m_aRSurfs)
	{
		free(m_aRSurfs);
		m_aRSurfs = NULL;
	}

	if (m_aRClusters)
	{
		free(m_aRClusters);
		m_aRClusters = NULL;
		m_iNumRCluster = 0;
	}
}

/*	read data of a lump in BSP file.

	Return number of item in this lump

	pFile: file's pointer
	pHeader: BSP file header
	iLump: lump will be read.
	ppBuf (out): address of a variable that will be set to lump data buffer's pointer.
	iSize: item size in lump
*/
int	A3DBSP::ReadLump(AFile* pFile, PBSPFILEHEADER pHeader, int iLump, void** ppBuf, int iSize)
{
	PBFLUMP pLump = &pHeader->aLumps[iLump];
	DWORD dwRead;

	if (pLump->lSize % iSize)
		return -1;
	
	void *pBuf;
	if (!(pBuf = malloc(pLump->lSize)))
		return -1;

	pFile->Seek(pLump->lOff, AFILE_SEEK_SET);
	pFile->Read(pBuf, pLump->lSize, &dwRead);

	*ppBuf = pBuf;

	return pLump->lSize / iSize;
}

//	Read plane lump
bool A3DBSP::ReadPlaneLump(AFile* pFile, PBSPFILEHEADER pHeader)
{
	PBFPLANE aBFPlanes;
	int iNumPlane = ReadLump(pFile, pHeader, BFLUMP_PLANES, (void**) &aBFPlanes, sizeof (BFPLANE));

	if (iNumPlane == -1)
	{
		g_A3DErrLog.Log("Failed to read plane lump A3DBSP::ReadPlaneLump");
		return false;
	}
	
	A3DSPLANE* aPlanes = (A3DSPLANE*)malloc(iNumPlane * sizeof (A3DSPLANE));
	if (!aPlanes)
	{
		g_A3DErrLog.Log("Not enough memory in A3DBSP::ReadPlaneLump");
		return false;
	}

	for (int i=0; i < iNumPlane; i++)
	{
		aPlanes[i].vNormal.x	= aBFPlanes[i].vNormal[0];
		aPlanes[i].vNormal.y	= aBFPlanes[i].vNormal[1];
		aPlanes[i].vNormal.z	= aBFPlanes[i].vNormal[2];
		aPlanes[i].fDist		= aBFPlanes[i].fDist;
		aPlanes[i].MakeTypeAndSignBits();
	}

	m_aPlanes	= aPlanes;
	m_iNumPlane	= iNumPlane;

	free(aBFPlanes);
	
	return true;
}

//	Read node lump
bool A3DBSP::ReadNodeLump(AFile* pFile, PBSPFILEHEADER pHeader)
{
	PBFNODE aBFNodes;
	int iNumNode = ReadLump(pFile, pHeader, BFLUMP_NODES, (void**) &aBFNodes, sizeof (BFNODE));

	if (iNumNode == -1)
	{
		g_A3DErrLog.Log("Failed to read plane lump A3DBSP::ReadNodeLump");
		return false;
	}
	
	PBSPNODE aNodes = (PBSPNODE)malloc(iNumNode * sizeof (BSPNODE));
	if (!aNodes)
	{
		g_A3DErrLog.Log("Not enough memory in A3DBSP::ReadNodeLump");
		return false;
	}

	int i, j;
	for (i=0; i < iNumNode; i++)
	{
		aNodes[i].pPlane		= &m_aPlanes[aBFNodes[i].lPlane];
		aNodes[i].aChildren[0]	= aBFNodes[i].Children[0];
		aNodes[i].aChildren[1]	= aBFNodes[i].Children[1];
		aNodes[i].dwFrameCnt	= 0;
		aNodes[i].pParent		= NULL;
		aNodes[i].bHasurf		= false;

		for (j=0; j < 3; j++)
		{
			aNodes[i].vMins.m[j] = aBFNodes[i].vMins[j];
			aNodes[i].vMaxs.m[j] = aBFNodes[i].vMaxs[j];
		}
	}

	m_aNodes	= aNodes;
	m_iNumNode	= iNumNode;

	free(aBFNodes);

	return true;
}

//	Read leaf lump
bool A3DBSP::ReadLeafLump(AFile* pFile, PBSPFILEHEADER pHeader)
{
	PBFLEAF aBFLeaves;
	int iNumLeaf = ReadLump(pFile, pHeader, BFLUMP_LEAVES, (void**) &aBFLeaves, sizeof (BFLEAF));

	if (iNumLeaf == -1)
	{
		g_A3DErrLog.Log("Failed to read plane lump A3DBSP::ReadLeafLump");
		return false;
	}
	
	PBSPLEAF aLeaves = (PBSPLEAF)malloc(iNumLeaf * sizeof (BSPLEAF));
	if (!aLeaves)
	{
		g_A3DErrLog.Log("Not enough memory in A3DBSP::ReadLeafLump");
		return false;
	}

	int i, j, iNumBytes = *((int *)m_aPVS + 1);

	for (i=0; i < iNumLeaf; i++)
	{
		aLeaves[i].iCluster		= aBFLeaves[i].lCluster;
		aLeaves[i].iArea		= aBFLeaves[i].lArea;
		aLeaves[i].aSurfRefs	= &m_aSurfRefs[aBFLeaves[i].lFirstLeafSurf];
		aLeaves[i].iNumSurfRef	= aBFLeaves[i].lNumLeafSurfs;
		aLeaves[i].dwFrameCnt	= 0;
		aLeaves[i].pParent		= NULL;

		if (aLeaves[i].iCluster >= 0)
		{
			aLeaves[i].aPVS	= m_aPVS + SIZE_PVSHEADER + iNumBytes * aLeaves[i].iCluster;

			//	Add to cluster array
			m_aClusters[aLeaves[i].iCluster] = &aLeaves[i];
		}
		else
			aLeaves[i].aPVS = NULL;

		for (j=0; j < 3; j++)
		{
			aLeaves[i].vMins.m[j] = aBFLeaves[i].vMins[j];
			aLeaves[i].vMaxs.m[j] = aBFLeaves[i].vMaxs[j];
		}
	}

	m_aLeaves	= aLeaves;
	m_iNumLeaf	= iNumLeaf;

	free(aBFLeaves);

	return true;
}

//	Read draw surface reference lump
bool A3DBSP::ReadSurfRefLump(AFile* pFile, PBSPFILEHEADER pHeader)
{
	int* aSurfRef;
	int iNumSurfRef = ReadLump(pFile, pHeader, BFLUMP_LEAFSURFS, (void**) &aSurfRef, sizeof (int));

	if (iNumSurfRef == -1)
	{
		g_A3DErrLog.Log("Failed to read plane lump A3DBSP::ReadSurfRefLump");
		return false;
	}

	m_aSurfRefs		= aSurfRef;
	m_iNumSurfRef	= iNumSurfRef;

	return true;
}

//	Read PVS lump
bool A3DBSP::ReadPVSLump(AFile* pFile, PBSPFILEHEADER pHeader)
{
	BYTE* aPVS;
	int iPVSSize = ReadLump(pFile, pHeader, BFLUMP_VISIBILITY, (void**) &aPVS, sizeof (BYTE));

	if (iPVSSize == -1)
	{
		g_A3DErrLog.Log("Failed to read plane lump A3DBSP::ReadPVSLump");
		return false;
	}

	m_aPVS			= aPVS;
	m_iPVSSize		= iPVSSize;
	m_iLeafPVSSize	= *((int *)m_aPVS + 1);

	//	Create cluster buffer
	m_iNumCluster = *((int *)m_aPVS);
	m_aClusters	  = (PBSPLEAF*)malloc(m_iNumCluster * sizeof (PBSPLEAF));
	
	if (!m_aClusters)
	{
		g_A3DErrLog.Log("A3DBSP::ReadPVSLump(), not enough memory");
		return false;
	}

	memset(m_aClusters, 0, m_iNumCluster * sizeof (PBSPLEAF));

	return true;
}

/*	Initialize all other buffers, this function should be called after
	BSP file has been loaded by Load().

	Return true for success, otherwise return false.
*/
bool A3DBSP::InitOthers()
{
	int i, iLeaf;
	PBSPNODE pNode;

	//	Initialize m_aFCFlags
	if (!(m_aFCFlags = (BYTE*)malloc(m_iLeafPVSSize)))
	{
		g_A3DErrLog.Log("A3DBSP::InitOthers, Not enough memory for cluster fill flags");
		return false;
	}

	memset(m_aFCFlags, 0, m_iLeafPVSSize);

	for (i=0; i < m_iNumCluster; i++)
	{
		if (m_aClusters[i]->iNumSurfRef)
			SETBSPPVSBIT(m_aFCFlags, i);
	}

	//	Calculate number of draw surface. This is not a good way, maybe we
	//	should record it in BSP file.
	m_iNumDrawSurf = -1;

	for (i=0; i < m_iNumSurfRef; i++)
	{
		if (m_aSurfRefs[i] > m_iNumDrawSurf)
			m_iNumDrawSurf = m_aSurfRefs[i];
	}
	
	m_iNumDrawSurf++;

	//	Initialize m_aSRFlags
	if (!(m_aSRFlags = (BYTE*)malloc(m_iNumDrawSurf)))
	{
		g_A3DErrLog.Log("A3DBSP::InitOthers, Not enough memory for surface rendered flags");
		return false;
	}

	memset(m_aSRFlags, 0, m_iNumDrawSurf);

	//	Initialize m_aRSurfs
	if (!(m_aRSurfs = (int*)malloc(m_iNumDrawSurf * sizeof (int))))
	{
		g_A3DErrLog.Log("A3DBSP::InitOthers, Not enough memory for surface index buffer");
		return false;
	}

	memset(m_aRSurfs, 0, m_iNumDrawSurf * sizeof (int));

	//	Initialize m_aRClusters
	if (!(m_aRClusters = (PBSPLEAF*)malloc(m_iNumCluster * sizeof (PBSPLEAF))))
	{
		g_A3DErrLog.Log("A3DBSP::InitOthers, Not enough memory for rendered cluster buffer");
		return false;
	}

	memset(m_aRClusters, 0, m_iNumCluster * sizeof (PBSPLEAF));

	//	Set parent for all nodes and leaves
	m_aNodes[0].pParent = NULL;

	for (i=0; i < m_iNumNode; i++)
	{
		pNode = &m_aNodes[i];

		if (pNode->aChildren[0] >= 0)
			m_aNodes[pNode->aChildren[0]].pParent = pNode;
		else	//	Child is a leaf
		{
			iLeaf = -(pNode->aChildren[0] + 1);
			m_aLeaves[iLeaf].pParent = pNode;
		}

		if (pNode->aChildren[1] >= 0)
			m_aNodes[pNode->aChildren[1]].pParent = pNode;
		else	//	Child is a leaf
		{
			iLeaf = -(pNode->aChildren[1] + 1);
			m_aLeaves[iLeaf].pParent = pNode;
		}
	}

	//	Set bHasSurf of every node
	for (i=0; i < m_iNumCluster; i++)
	{
		if (!m_aClusters[i]->iNumSurfRef)
			continue;

		pNode = m_aClusters[i]->pParent;
		while (pNode)
		{
			if (pNode->bHasurf)
				break;

			pNode->bHasurf = true;
			pNode = pNode->pParent;
		}
	}

	return true;
}

/*	Find the leaf which contain specified point.

	Return true if leaf which contains specified point is a cluster, otherwise return
	false if it's a solid leaf or vPos is out of BSP space.

	vPos: point's position in world coordiantes
	piLeafID (out): used to receive leaf's ID
*/
bool A3DBSP::PointInLeaf(const A3DVECTOR3& vPos, int* piLeafID)
{
	A3DSPLANE* pPlane;
	float d;
	int iLeaf;

	//	Search from root node
	PBSPNODE pNode = &m_aNodes[0];

	while (1)
	{
		pPlane = pNode->pPlane;
	
		if (pPlane->byType < 3)
			d = vPos.m[pPlane->byType] - pPlane->fDist;
		else if (pPlane->byType < 6)
			d = -vPos.m[pPlane->byType-3] - pPlane->fDist;
		else
			d = DotProduct(vPos, pPlane->vNormal) - pPlane->fDist;

		if (d >= 0)		//	Point is in front of plane
		{
			if (pNode->aChildren[0] < 0)		//	Is a leaf
			{
				iLeaf = -(pNode->aChildren[0] + 1);
				goto End;
			}
			
			pNode = &m_aNodes[pNode->aChildren[0]];
		}
		else
		{
			if (pNode->aChildren[1] < 0)		//	Is a leaf
			{
				iLeaf = -(pNode->aChildren[1] + 1);
				goto End;
			}
			
			pNode = &m_aNodes[pNode->aChildren[1]];
		}
	}

End:	

	*piLeafID = iLeaf;

	if (m_aLeaves[iLeaf].iCluster < 0)
		return false;
	
	return true;
}

/*	Check whether a position can be seen from specified leaf

	Return true if specified postion can be seen, otherwise return false.

	vPos: position will be checked
	iLeaf: leaf view will see from
*/
bool A3DBSP::CanBeSeenFromLeaf(const A3DVECTOR3& vPos, int iLeaf)
{
	if (m_aLeaves[iLeaf].iCluster < 0)
		return false;

	int iPosLeaf;
	if (!PointInLeaf(vPos, &iPosLeaf))
		return true;	//	In our game, we assume all objects out bsp space are visible.
	
	if (GETBSPPVSBIT(m_aLeaves[iLeaf].aPVS, m_aLeaves[iPosLeaf].iCluster))
		return true;

	return false;
}

/*	Get the first visible cluster from specified poistion

	Return cluster's index for success, otherwise return -1

	vPos: specified position.
	piLeafID (out): the leaf's ID in which vPos lies, can be NULL
*/
int	A3DBSP::GetFirstVisibleCluster(A3DVECTOR3& vPos, int* piLeafID)
{
	int iLeaf;
	if (!PointInLeaf(vPos, &iLeaf))
		return -1;

	if (piLeafID)
		*piLeafID = iLeaf;

	_m_pCurCluster	= &m_aLeaves[iLeaf];
	_m_iPVSCnt		= 0;

	int i;
	BYTE* pPVS = _m_pCurCluster->aPVS;

	for (i=0; i < m_iNumCluster; i++)
	{
		if (GETBSPPVSBIT(pPVS, i))
		{
			_m_iPVSCnt = i;
			return i;
		}
	}

	return -1;
}

/*	Get next visible cluster

	Return cluster's index
*/
int	A3DBSP::GetNextVisibleCluster()
{
	int i;
	BYTE* pPVS = _m_pCurCluster->aPVS;

	for (i=_m_iPVSCnt+1; i < m_iNumCluster; i++)
	{
		if (GETBSPPVSBIT(pPVS, i))
		{
			_m_iPVSCnt = i;
			return i;
		}
	}

	return -1;
}

/*	Get cluster's bounding box

	iCluster: cluster's index, this value can be got from GetFirstVisibleCluster() and
			  GetNextVisibleCluster().
	vMins, vMaxs (out): used to receive cluster's bounding box
*/
void A3DBSP::GetClusterAABB(int iCluster, A3DVECTOR3& vMins, A3DVECTOR3& vMaxs)
{
	vMins = m_aClusters[iCluster]->vMins;
	vMaxs = m_aClusters[iCluster]->vMaxs;
}

/*	Get visible draw surfaces from specified point

	Return valued is defined in class A3DSceneVisible, GVS_*

	pViewport: current viewport
	ppSurfs (out): *ppSurfs will point to surface index buffer if GVS_OK is returned.
	piNumSurf (out): number of surface in buffer pointed by *ppSurfs.
*/
DWORD A3DBSP::GetVisibleSurfs(A3DViewport* pViewport, int** ppSurfs, int* piNumSurf)
{
	A3DCameraBase* pCamera = pViewport->GetCamera();	

	int iLeaf;
	if (!PointInLeaf(pCamera->GetPos(), &iLeaf))
	{
		*piNumSurf	= 0;
		*ppSurfs	= NULL;

		return GVS_INVALIDEYE;	//	vPos is in solid leaf
	}

	m_pFrustum	= pCamera->GetWorldFrustum();
	m_vViewPos	= pCamera->GetPos();

	//	Disable near and far clipping planes
	bool bNearEnable = m_pFrustum->PlaneIsEnable(A3DFrustum::VF_NEAR);
	bool bFarEnable	 = m_pFrustum->PlaneIsEnable(A3DFrustum::VF_FAR);
	m_pFrustum->EnablePlane(A3DFrustum::VF_NEAR, false);
	m_pFrustum->EnablePlane(A3DFrustum::VF_FAR, false);

	//	Clear surface rendered flags
	memset(m_aSRFlags, 0, m_iNumDrawSurf);

	bool bRet;

	if (m_bRecursive)
		bRet = R_GetVisibleSurfs(iLeaf);
	else
		bRet = E_GetVisibleSurfs(iLeaf);

	//	Restore clipping planes
	m_pFrustum->EnablePlane(A3DFrustum::VF_NEAR, bNearEnable);
	m_pFrustum->EnablePlane(A3DFrustum::VF_FAR, bFarEnable);

	if (!bRet)
	{
		*piNumSurf	= 0;
		*ppSurfs	= NULL;
	}
	else
	{
		*piNumSurf	= m_iNumVisSurf;
		*ppSurfs	= m_aRSurfs;
	}

	return GVS_OK;
}

/*	Get visible surfaces use enumerate method

	Return true if there are some surfaces will be drawn

	iLeaf: index of the leaf current view point is in
*/
bool A3DBSP::E_GetVisibleSurfs(int iLeaf)
{
	int i, j, k, iSurf;
	BYTE *pByte, *pFlag, byAnd, byBit;
	BSPLEAF* pLeaf;

	if (iLeaf == m_iViewLeaf)
	{
		m_iNumVisSurf = 0;

		for (i=0; i < m_iNumRCluster; i++)
		{
			pLeaf = m_aRClusters[i];
			
			if (m_pFrustum->AABBInFrustum(pLeaf->vMins, pLeaf->vMaxs) < 0)
				continue;
			
			for (k=0; k < pLeaf->iNumSurfRef; k++)
			{
				if (!m_aSRFlags[(iSurf = pLeaf->aSurfRefs[k])])
				{
					m_aSRFlags[iSurf] = 1;
					m_aRSurfs[m_iNumVisSurf++] = iSurf;
				}
			}
		}

		return true;
	}

	m_iViewLeaf		= iLeaf;
	m_iNumRCluster	= 0;
	m_iNumVisSurf	= 0;

	pByte = m_aLeaves[iLeaf].aPVS;
	pFlag = m_aFCFlags;

	for (i=0; i < m_iLeafPVSSize; i++, pByte++, pFlag++)
	{
		if ((byAnd = *pByte & *pFlag))
		{
			byBit = 1;

			for (j=0; j < 8; j++)
			{
				if (byAnd & byBit)
				{
					pLeaf = m_aClusters[(i << 3) + j];

					if (!pLeaf->iNumSurfRef)
						goto NextCluster;

					m_aRClusters[m_iNumRCluster++] = pLeaf;

					if (m_pFrustum->AABBInFrustum(pLeaf->vMins, pLeaf->vMaxs) < 0)
						goto NextCluster;

					for (k=0; k < pLeaf->iNumSurfRef; k++)
					{
						if (!m_aSRFlags[(iSurf = pLeaf->aSurfRefs[k])])
						{
							m_aSRFlags[iSurf] = 1;
							m_aRSurfs[m_iNumVisSurf++] = iSurf;
						}
					}
				}

			NextCluster:	
				byBit <<= 1;
			}
		}
	}

	return m_iNumVisSurf ? true : false;
}

/*	Get visible surfaces use recursive method

  	Return true if there are some surfaces will be drawn

	iLeaf: index of the leaf current view point is in
*/
bool A3DBSP::R_GetVisibleSurfs(int iLeaf)
{
	if (iLeaf != m_iViewLeaf)
	{
		MarkLeaves(m_aLeaves[iLeaf].iCluster);
		m_iViewLeaf = iLeaf;
	}

	m_iNumVisSurf = 0;
	RecursiveWorldNode(&m_aNodes[0], true);

	return m_iNumVisSurf ? true : false;
}

/*	Mark leaves can be seen

	iViewCluster: index of cluster current viewpoint is in
*/
void A3DBSP::MarkLeaves(int iViewCluster)
{
	int i;
	BSPNODE* pNode;
	BYTE* aPVS = m_aClusters[iViewCluster]->aPVS;

	m_dwFrameCnt++;

	for (i=0; i < m_iNumCluster; i++)
	{
		if (GETBSPPVSBIT(aPVS, i))
		{
			m_aClusters[i]->dwFrameCnt = m_dwFrameCnt;
		
			pNode = m_aClusters[i]->pParent;
			while (pNode)
			{
				if (pNode->dwFrameCnt == m_dwFrameCnt)
					break;

				pNode->dwFrameCnt = m_dwFrameCnt;
				pNode = pNode->pParent;
			}
		}
	}
}

/*	Record all surfaces will be drawn

	pNode: specified node
	bNeedClip: true, use view frustum to clip this node,
*/
void A3DBSP::RecursiveWorldNode(BSPNODE* pNode, bool bNeedClip)
{
	if (pNode->dwFrameCnt != m_dwFrameCnt || !pNode->bHasurf)
		return;

	int i, iLeaf, iFront, iBack, iSurf;
	float fDist;
	A3DSPLANE* pPlane = pNode->pPlane;
	BSPLEAF* pLeaf;

	//	Clip this node with view frustum
	if (bNeedClip)
	{
		int iSide = m_pFrustum->AABBInFrustum(pNode->vMins, pNode->vMaxs);
		if (iSide < 0)
			return;

		bNeedClip = (iSide == 0) ? true : false;
	}

	if (pPlane->byType < 3)
		fDist = m_vViewPos.m[pPlane->byType] - pPlane->fDist;
	else if (pPlane->byType < 6)
		fDist = -m_vViewPos.m[pPlane->byType-3] - pPlane->fDist;
	else
		fDist = DotProduct(m_vViewPos, pPlane->vNormal) - pPlane->fDist;
	
	if (fDist >= 0)
	{
		iFront = 0;
		iBack  = 1;
	}
	else
	{
		iFront = 1;
		iBack  = 0;
	}

	//	Handle front nodes
	if (pNode->aChildren[iFront] < 0)	//	Child is a leaf ?
	{
		iLeaf = -(pNode->aChildren[iFront] + 1);
		pLeaf = &m_aLeaves[iLeaf];

		if (pLeaf->dwFrameCnt == m_dwFrameCnt)
		{
			for (i=0; i < pLeaf->iNumSurfRef; i++)
			{
				if (!m_aSRFlags[(iSurf = pLeaf->aSurfRefs[i])])
				{
					m_aSRFlags[iSurf] = 1;
					m_aRSurfs[m_iNumVisSurf++] = iSurf;
				}
			}
		}
	}
	else
		RecursiveWorldNode(&m_aNodes[pNode->aChildren[iFront]], bNeedClip);

	//	Handle back nodes
	if (pNode->aChildren[iBack] < 0)	//	Child is a leaf
	{
		iLeaf = -(pNode->aChildren[iBack] + 1);
		pLeaf = &m_aLeaves[iLeaf];

		if (pLeaf->dwFrameCnt == m_dwFrameCnt)
		{
			for (i=0; i < pLeaf->iNumSurfRef; i++)
			{
				if (!m_aSRFlags[(iSurf = pLeaf->aSurfRefs[i])])
				{
					m_aSRFlags[iSurf] = 1;
					m_aRSurfs[m_iNumVisSurf++] = iSurf;
				}
			}
		}
	}
	else
		RecursiveWorldNode(&m_aNodes[pNode->aChildren[iBack]], bNeedClip);
}

