/*
 * FILE: A3DPortal.cpp
 *
 * DESCRIPTION: Scene visible solution using portal algorithm.
 *
 * CREATED BY: duyuxin, 2003/6/17
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#include "A3DPortal.h"
#include "A3DPI.h"
#include "A3DFuncs.h"
#include "A3DCollision.h"
#include "A3DCameraBase.h"
#include "A3DFrustum.h"
#include "A3DViewport.h"
#include "AFileImage.h"
#include "AMemory.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

#define new A_DEBUG_NEW

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
//	Implement of A3DPortal
//
///////////////////////////////////////////////////////////////////////////

A3DPortal::A3DPortal()
{
	m_aSectParts		= NULL;
	m_aSectPlaneRefs	= NULL;
	m_aPartSurfRefs		= NULL;
	m_aVertices			= NULL;
	m_aPlanes			= NULL;
	m_aSectPtlRefs		= NULL;
	m_aBSPNodes			= NULL;
	m_iNumSectPart		= 0;
	m_iNumSectPlaneRef	= 0;
	m_iNumPartSurfRef	= 0;
	m_iNumVertex		= 0;
	m_iNumPlanes		= 0;
	m_iNumSectPtlRef	= 0;
	m_iNumBSPNodes		= 0;

	m_bPortalLoaded		= false;
	m_bCompRecord		= true;
	m_aVisSurfs			= NULL;
	m_aVisSurfFlags		= NULL;
	m_aVisSurfFlags2	= NULL;
	m_iNumVisSurf		= 0;
	m_iNumDrawSurf		= 0;
	m_dwTraceCnt		= 0;
	m_bAABBSector		= true;
	m_bCheckParts		= true;
	m_dwSectorCnt		= 0;
	m_aSectorCnts		= NULL;
}

//	Initialize object
bool A3DPortal::Load(const char* szFile)
{
	AFileImage File;

	if (!File.Open("", szFile, AFILE_OPENEXIST))
	{
		g_A3DErrLog.Log("A3DPortal::Load, Cannot open file %s!", szFile);
		return false;
	}

	//	Release old resources
	ReleasePortalResources();

	PTFHEADER Header;
	DWORD dwRead;

	//	Read BSP file header
	if (!File.Read(&Header, sizeof (Header), &dwRead) || dwRead != sizeof (Header))
	{
		g_A3DErrLog.Log("A3DPortal::Load, Failed to read file header");
		File.Close();
		return false;
	}

	//	Check format and version
	if (Header.dwIdentify != PTFILE_IDENTIFY || Header.dwVersion > PTFILE_VERSION)
	{
		g_A3DErrLog.Log("A3DPortal::Load, Wrong file format or version");
		File.Close();
		return false;
	}

	PTFLUMP* aLumps;
	int iNumLump = Header.iNumLump;
	
	if (Header.dwVersion < 3)
	{
		iNumLump = NUM_PTLUMP2;
		File.Seek(sizeof (DWORD) * 2, AFILE_SEEK_SET);
	}

	if (!(aLumps = new PTFLUMP[iNumLump]))
	{
		g_A3DErrLog.Log("A3DPortal::Load, Not enough memory");
		File.Close();
		return false;
	}

	if (!File.Read(aLumps, iNumLump * sizeof (PTFLUMP), &dwRead) ||
		dwRead != iNumLump * sizeof (PTFLUMP))
	{
		g_A3DErrLog.Log("A3DPortal::Load, Failed to read lumps");
		File.Close();
		return false;
	}

	//	Read lumps
	ReadPlaneLump(&File, aLumps);				//	Plane lump before sector lump
	ReadVertexLump(&File, aLumps);				//	Vertex lump before portal lump
	ReadSectorPlaneRefLump(&File, aLumps);		//	Plane ref lump before sector lump
	ReadSectorPortalRefLump(&File, aLumps);		//	Portal ref lump before sector lump
	ReadPartSurfRefLump(&File, aLumps);			//	Part surface ref lump before part lump
	ReadSectorPartLump(&File, aLumps);			//	Part lump before sector lump
	ReadSectorAndPortalLumps(&File, aLumps);

	if (Header.dwVersion >= 3)
		ReadBSPNodesLump(&File, aLumps);		//	BSP node lump after sector lump

	File.Close();

	delete [] aLumps;

	//	Create necessary buffers
	if (!CreateBuffers())
	{
		g_A3DErrLog.Log("A3DPortal::Load, Failed to call CreateBuffers()");
		return false;
	}

	m_bPortalLoaded = true;

	return true;
}

//	Release objects
void A3DPortal::Release()
{
	ReleasePortalResources();
}

void A3DPortal::ReleasePortalResources()
{
	if (m_aSectPlaneRefs)
	{
		a_free(m_aSectPlaneRefs);
		m_aSectPlaneRefs = NULL;
	}

	if (m_aSectPtlRefs)
	{
		a_free(m_aSectPtlRefs);
		m_aSectPtlRefs = NULL;
	}

	if (m_aPartSurfRefs)
	{
		a_free(m_aPartSurfRefs);
		m_aPartSurfRefs = NULL;
	}

	if (m_aVertices)
	{
		a_free(m_aVertices);
		m_aVertices = NULL;
	}

	if (m_aPlanes)
	{
		a_free(m_aPlanes);
		m_aPlanes = NULL;
	}

	if (m_aSectParts)
	{
		a_free(m_aSectParts);
		m_aSectParts = NULL;
	}

	if (m_aBSPNodes)
	{
		a_free(m_aBSPNodes);
		m_aBSPNodes = NULL;
	}

	int i;

	//	Release portals
	for (i=0; i < m_aPortals.GetSize(); i++)
	{
		PORTAL* p = m_aPortals[i];
		if (p->bDynamic)
		{
			delete p->aVerts;
			delete p->pPlane;
		}

		delete p;
	}

	m_aPortals.RemoveAll();

	//	Release sectors
	for (i=0; i < m_aSectors.GetSize(); i++)
	{
		SECTOR* s = m_aSectors[i];
		delete s;
	}

	m_aSectors.RemoveAll();

	m_bPortalLoaded		= false;
	m_iNumSectPart		= 0;
	m_iNumSectPlaneRef	= 0;
	m_iNumPartSurfRef	= 0;
	m_iNumVertex		= 0;
	m_iNumPlanes		= 0;
	m_iNumSectPtlRef	= 0;
	m_iNumBSPNodes		= 0;

	if (m_aVisSurfs)
	{
		delete [] m_aVisSurfs;
		m_aVisSurfs = NULL;
	}

	if (m_aVisSurfFlags)
	{
		delete [] m_aVisSurfFlags;
		m_aVisSurfFlags = NULL;
	}

	if (m_aVisSurfFlags2)
	{
		delete [] m_aVisSurfFlags2;
		m_aVisSurfFlags2 = NULL;
	}

	if (m_aSectorCnts)
	{
		delete [] m_aSectorCnts;
		m_aSectorCnts = NULL;
	}

	m_iNumDrawSurf = 0;
}

/*	Read a lump in portal file.

	Return number of item in this lump

	pFile: file's pointer
	aLump: portal file lumps
	iLump: lump will be read.
	ppBuf (out): address of a variable that will be set to lump data buffer's pointer.
	dwSize: item size in lump
*/	
int	A3DPortal::ReadLump(AFile* pFile, PTFLUMP* aLumps, int iLump, void** ppBuf, DWORD dwSize)
{
	PTFLUMP* pLump = &aLumps[iLump];
	DWORD dwRead;

	if (pLump->dwSize % dwSize)
		return -1;
	
	void *pBuf;
	if (!(pBuf = a_malloc(pLump->dwSize)))
		return -1;

	pFile->Seek(pLump->dwOff, AFILE_SEEK_SET);
	pFile->Read(pBuf, pLump->dwSize, &dwRead);

	*ppBuf = pBuf;

	return pLump->dwSize / dwSize;
}

bool A3DPortal::ReadSectorAndPortalLumps(AFile* pFile, PTFLUMP* aLumps)
{
	PTFPORTAL* aReadPortals;
	int iNumPortal = ReadLump(pFile, aLumps, PTLUMP_PORTAL, (void**)&aReadPortals, sizeof (PTFPORTAL));

	PTFSECTOR* aReadSectors;
	int iNumSector = ReadLump(pFile, aLumps, PTLUMP_SECTOR, (void**)&aReadSectors, sizeof (PTFSECTOR));
		
	ASSERT(iNumPortal >= 0 && iNumSector >= 0);

	m_aSectors.SetSize(iNumSector, 10);
	m_aPortals.SetSize(iNumPortal, 10);

	int i, j;

	//	Copy portal data
	for (i=0; i < iNumPortal; i++)
	{
		PORTAL* pDest = new PORTAL;
		ASSERT(pDest);

		PTFPORTAL* pSrc = &aReadPortals[i];

		pDest->bDynamic		= false;
		pDest->bOpen		= true;
		pDest->pPlane		= &m_aPlanes[pSrc->iPlane];
		pDest->aVerts		= &m_aVertices[pSrc->iFirstVert];
		pDest->iNumVert		= pSrc->iNumVert;
		pDest->pFront		= NULL;
		pDest->pBack		= NULL;
		pDest->iIndex		= i;
		pDest->pOpposite	= NULL;

		strcpy(pDest->szName, pSrc->szName);
		pDest->dwID = pDest->szName[0] ? a_MakeIDFromString(pDest->szName) : 0;

		//	Build bounding box
		a3d_ClearAABB(pDest->vMins, pDest->vMaxs);

		for (j=0; j < pDest->iNumVert; j++)
			a3d_AddVertexToAABB(pDest->vMins, pDest->vMaxs, pDest->aVerts[j]);

		//	Expand bounding box a little
		pDest->vMins -= 0.1f;
		pDest->vMaxs += 0.1f;

		m_aPortals[i] = pDest;
	}

	//	Fill opposite member
	for (i=0; i < iNumPortal; i++)
	{
		PTFPORTAL* pSrc = &aReadPortals[i];
		if (pSrc->iOpposite >= 0)
			m_aPortals[i]->pOpposite = m_aPortals[pSrc->iOpposite];
	}

	//	Copy sector data
	for (i=0; i < iNumSector; i++)
	{
		SECTOR* pDest = new SECTOR;
		ASSERT(pDest);

		PTFSECTOR* pSrc = &aReadSectors[i];

		pDest->aabb.Mins.Set(pSrc->vMins[0], pSrc->vMins[1], pSrc->vMins[2]);
		pDest->aabb.Maxs.Set(pSrc->vMaxs[0], pSrc->vMaxs[1], pSrc->vMaxs[2]);
		a3d_CompleteAABB(&pDest->aabb);

		pDest->aParts		= &m_aSectParts[pSrc->iFirstPart];
		pDest->iNumPart		= pSrc->iNumPart;
		pDest->iIndex		= i;
		pDest->bOnTrail		= false;
		pDest->dwTraceCnt	= 0;

		//	Planes
		pDest->aPlanes.SetSize(pSrc->iNumPlane, 5);

		for (j=0; j < pSrc->iNumPlane; j++)
		{
			int p = m_aSectPlaneRefs[pSrc->iFirstPlane+j];
			pDest->aPlanes[j] = &m_aPlanes[p];
		}

		//	Portals
		pDest->aPortals.SetSize(pSrc->iNumPortal, 5);

		for (j=0; j < pSrc->iNumPortal; j++)
		{
			int p = m_aSectPtlRefs[pSrc->iFirstPortal+j];
			pDest->aPortals[j] = m_aPortals[p];
		}

		m_aSectors[i] = pDest;
	}

	//	Link sectors and portals
	for (i=0; i < iNumPortal; i++)
	{
		PORTAL* pPortal = m_aPortals[i];
		pPortal->pFront	= m_aSectors[aReadPortals[i].iFront];
		pPortal->pBack	= m_aSectors[aReadPortals[i].iBack];
	}

	a_free(aReadPortals);
	a_free(aReadSectors);

	return true;
}

bool A3DPortal::ReadPlaneLump(AFile* pFile, PTFLUMP* aLumps)
{
	PTFPLANE* aReadPlanes;
	int iNumPlane = ReadLump(pFile, aLumps, PTLUMP_PLANES, (void**)&aReadPlanes, sizeof (PTFPLANE));

	ASSERT(iNumPlane >= 0);

	if (!(m_aPlanes = (A3DSPLANE*)a_malloc(sizeof (A3DSPLANE) * iNumPlane)))
	{
		a_free(aReadPlanes);
		return false;
	}

	int i;
	for (i=0; i < iNumPlane; i++)
	{
		A3DSPLANE* pDest = &m_aPlanes[i];
		PTFPLANE* pSrc = &aReadPlanes[i];

		pDest->vNormal.Set(pSrc->vNormal[0], pSrc->vNormal[1], pSrc->vNormal[2]);
		pDest->fDist = pSrc->fDist;
		pDest->MakeTypeAndSignBits();
	}

	m_iNumPlanes = iNumPlane;

	a_free(aReadPlanes);

	return true;
}

bool A3DPortal::ReadSectorPlaneRefLump(AFile* pFile, PTFLUMP* aLumps)
{
	int* aPlaneRef;
	int iNumPlaneRef = ReadLump(pFile, aLumps, PTLUMP_SECTPLANEREF, (void**)&aPlaneRef, sizeof (int));

	ASSERT(iNumPlaneRef >= 0);

	m_aSectPlaneRefs	= aPlaneRef;
	m_iNumSectPlaneRef	= iNumPlaneRef;

	return true;
}

bool A3DPortal::ReadSectorPortalRefLump(AFile* pFile, PTFLUMP* aLumps)
{
	int* aPortalRef;
	int iNumPortalRef = ReadLump(pFile, aLumps, PTLUMP_SECTPORTALREF, (void**)&aPortalRef, sizeof (int));

	ASSERT(iNumPortalRef >= 0);

	m_aSectPtlRefs		= aPortalRef;
	m_iNumSectPtlRef	= iNumPortalRef;

	return true;
}

bool A3DPortal::ReadSectorPartLump(AFile* pFile, PTFLUMP* aLumps)
{
	PTFSECTORPART* aReadParts;
	int iNumPart = ReadLump(pFile, aLumps, PTLUMP_SECTORPART, (void**)&aReadParts, sizeof (PTFSECTORPART));

	ASSERT(iNumPart >= 0);

	if (!(m_aSectParts = (SECTORPART*)a_malloc(sizeof (SECTORPART) * iNumPart)))
	{
		a_free(aReadParts);
		return false;
	}

	int i;
	for (i=0; i < iNumPart; i++)
	{
		SECTORPART* pDest = &m_aSectParts[i];
		PTFSECTORPART* pSrc = &aReadParts[i];

		pDest->aabb.Mins.Set(pSrc->vMins[0], pSrc->vMins[1], pSrc->vMins[2]);
		pDest->aabb.Maxs.Set(pSrc->vMaxs[0], pSrc->vMaxs[1], pSrc->vMaxs[2]);
		a3d_CompleteAABB(&pDest->aabb);

		pDest->aSurfs		= &m_aPartSurfRefs[pSrc->iFirstSurf];
		pDest->iNumSurf		= pSrc->iNumSurf;
		pDest->dwTraceCnt	= 0;
	}

	m_iNumSectPart = iNumPart;

	a_free(aReadParts);

	return true;
}

bool A3DPortal::ReadPartSurfRefLump(AFile* pFile, PTFLUMP* aLumps)
{
	int* aSurfRef;
	int iNumSurfRef = ReadLump(pFile, aLumps, PTLUMP_PARTSURFACEREF, (void**)&aSurfRef, sizeof (int));

	ASSERT(iNumSurfRef >= 0);

	m_aPartSurfRefs		= aSurfRef;
	m_iNumPartSurfRef	= iNumSurfRef;

	return true;
}

bool A3DPortal::ReadVertexLump(AFile* pFile, PTFLUMP* aLumps)
{
	A3DVECTOR3* aVerts;
	int iNumVert = ReadLump(pFile, aLumps, PTLUMP_VERTEX, (void**)&aVerts, sizeof (A3DVECTOR3));

	ASSERT(iNumVert >= 0);

	m_aVertices		= aVerts;
	m_iNumVertex	= iNumVert;

	return true;
}

bool A3DPortal::ReadBSPNodesLump(AFile* pFile, PTFLUMP* aLumps)
{
	PTFBSPNODE* aReadNodes;
	int iNumNode = ReadLump(pFile, aLumps, PTLUMP_BSPNODES, (void**)&aReadNodes, sizeof (PTFBSPNODE));

	ASSERT(iNumNode >= 0);

	if (!(m_aBSPNodes = (BSPNODE*)a_malloc(sizeof (BSPNODE) * iNumNode)))
	{
		a_free(aReadNodes);
		return false;
	}

	int i;
	for (i=0; i < iNumNode; i++)
	{
		BSPNODE* pDest = &m_aBSPNodes[i];
		PTFBSPNODE* pSrc = &aReadNodes[i];

		pDest->pPlane		= pSrc->iPlane < 0 ? NULL : &m_aPlanes[pSrc->iPlane];
		pDest->pSector		= pSrc->iSector < 0 ? NULL : m_aSectors[pSrc->iSector];
		pDest->pParent		= pSrc->iParent < 0 ? NULL : &m_aBSPNodes[pSrc->iParent];
		pDest->aChildren[0]	= pSrc->aChildren[0] < 0 ? NULL : &m_aBSPNodes[pSrc->aChildren[0]];
		pDest->aChildren[1]	= pSrc->aChildren[1] < 0 ? NULL : &m_aBSPNodes[pSrc->aChildren[1]];
	}

	m_iNumBSPNodes = iNumNode;

	a_free(aReadNodes);

	return true;
}

//	Create necessary buffers
bool A3DPortal::CreateBuffers()
{
	int i, iNumSurf = -1;

	for (i=0; i < m_iNumPartSurfRef; i++)
	{
		if (m_aPartSurfRefs[i] > iNumSurf)
			iNumSurf = m_aPartSurfRefs[i];
	}

	//	Convert index to number
	iNumSurf++;

	if (!iNumSurf)
		return true;

	if (!(m_aVisSurfs = new int [iNumSurf]))
	{
		g_A3DErrLog.Log("A3DPortal::CreateBuffers, not enough memory");
		return false;
	}

	if (!(m_aVisSurfFlags = new BYTE [iNumSurf]) ||
		!(m_aVisSurfFlags2 = new BYTE [iNumSurf]))
	{
		g_A3DErrLog.Log("A3DPortal::CreateBuffers, not enough memory");
		return false;
	}

	memset(m_aVisSurfFlags2, 0, iNumSurf);

	m_iNumDrawSurf = iNumSurf;

	//	Temporary sector couters buffer
	if (!(m_aSectorCnts = new DWORD [m_aSectors.GetSize()]))
	{
		g_A3DErrLog.Log("A3DPortal::CreateBuffers, not enough memory");
		return false;
	}

	memset(m_aSectorCnts, 0, sizeof (DWORD) * m_aSectors.GetSize());
	m_dwSectorCnt = 0;

	return true;
}

/*	Get visible draw surfaces from specified point

	Return valued is defined in class A3DSceneVisible, GVS_*

	pViewport: current viewport
	ppSurfs (out): *ppSurfs will point to surface index buffer if GVS_OK is returned.
	piNumSurf (out): number of surface in buffer pointed by *ppSurfs.
*/
DWORD A3DPortal::GetVisibleSurfs(A3DViewport* pViewport, int** ppSurfs, int* piNumSurf)
{
	if (!m_bPortalLoaded)
		return GVS_ERROR;

	if (!GetVisibleSurfaces(pViewport))
		return GVS_INVALIDEYE;

	DWORD dwRet = GVS_OK;

	*piNumSurf	= m_iNumVisSurf;
	*ppSurfs	= m_aVisSurfs;

	if (m_bCompRecord)
	{
		if (!memcmp(m_aVisSurfFlags2, m_aVisSurfFlags, m_iNumDrawSurf))
			dwRet = GVS_OK_NOCHANGE;
	}

	m_bCompRecord = true;
	memcpy(m_aVisSurfFlags2, m_aVisSurfFlags, m_iNumDrawSurf);

	return dwRet;
}

//	Get visible surfaces
bool A3DPortal::GetVisibleSurfaces(A3DViewport* pView)
{
	A3DCameraBase* pCamera = pView->GetCamera();

	m_iNumVisSurf = 0;

	//	Get sector which the camera is in.
	SECTOR* pSector = SearchSector(pCamera->GetPos());
	if (!pSector)
		return true;

	m_dwTraceCnt++;

	//	Build clip frustum
	FRECT rcPortal = {-1.0f, 1.0f, 1.0f, -1.0f};
	A3DFrustum* Frustum = pCamera->GetWorldFrustum();

	m_matView = pCamera->GetVPTM();

	memset(m_aVisSurfFlags, 0, sizeof (BYTE) * m_iNumDrawSurf);

#ifdef _DEBUG
	m_iNumPVSector = 0;
#endif

	//	Clear all portals' pass area
	for (int i=0; i < m_aPortals.GetSize(); i++)
	{
		PORTAL* pPortal = m_aPortals[i];
		pPortal->rcPass.left = pPortal->rcPass.bottom = 2.0f;
		pPortal->rcPass.right = pPortal->rcPass.top = -2.0f;
	}

	GoThroughSector(pCamera, Frustum, &rcPortal, NULL, pSector);

	return true;
}

//	Search sector using BSP tree
A3DPortal::SECTOR* A3DPortal::SearchSector(const A3DVECTOR3& vPos)
{
	if (!m_iNumBSPNodes)
		return NULL;

	//	Search from root node
	BSPNODE* pNode = &m_aBSPNodes[0];

	while (1)
	{
		if (!pNode->pPlane)	//	Is a leaf
			return pNode->pSector;

		A3DSPLANE* pPlane = pNode->pPlane;
		float d;
	
		if (pPlane->byType < 3)
			d = vPos.m[pPlane->byType] - pPlane->fDist;
		else if (pPlane->byType < 6)
			d = -vPos.m[pPlane->byType-3] - pPlane->fDist;
		else
			d = DotProduct(vPos, pPlane->vNormal) - pPlane->fDist;

		if (d >= 0)		//	Point is in front of plane
			pNode = pNode->aChildren[0];
		else
			pNode = pNode->aChildren[1];
	}

	return NULL;
}

/*	Go through sector

	pCamera: current camera;
	pFrustum: clip frustum.
	pPtRect: portal's project rectangle in identity view space.
	pPortalFrom: portal comes from, NULL means eye is in pSector
	pSector: sector will be gone through
*/
bool A3DPortal::GoThroughSector(A3DCameraBase* pCamera, A3DFrustum* pFrustum, 
								FRECT* pPtRect, PORTAL* pPortalFrom, SECTOR* pSector)
{
	//	Sector is on trail, don't loop
	if (pSector->bOnTrail)
		return true;

	pSector->bOnTrail	= true;
	pSector->dwTraceCnt = m_dwTraceCnt;
		
#ifdef _DEBUG

	//	Record potential sectors
	if (m_iNumPVSector >= m_aPVSector.GetSize())
		m_aPVSector.Add(pSector->iIndex);
	else
		m_aPVSector[m_iNumPVSector] = pSector->iIndex;

	m_iNumPVSector++;

#endif

	int i, j;

	//	Add surfaces belong to this sector
	for (i=0; i < pSector->iNumPart; i++)
	{
		SECTORPART* pPart = &pSector->aParts[i];
		if (pPart->dwTraceCnt == m_dwTraceCnt || 
			pFrustum->AABBInFrustum(pPart->aabb.Mins, pPart->aabb.Maxs) < 0)
			continue;

		for (j=0; j < pPart->iNumSurf; j++)
		{
			int iSurf = pPart->aSurfs[j];
			if (!m_aVisSurfFlags[iSurf])
			{
				m_aVisSurfs[m_iNumVisSurf++] = iSurf;
				m_aVisSurfFlags[iSurf] = 1;
			}
		}

		pPart->dwTraceCnt = m_dwTraceCnt;
	}

	//	Go through portals
	A3DFrustum Frustum;
	FRECT rcPortal;
	SECTOR* pDestSector;

	A3DVECTOR3 vEyePos = pCamera->GetPos();
	A3DVECTOR3 vEyeDir = pCamera->GetDir();

	for (i=0; i < pSector->aPortals.GetSize(); i++)
	{
		PORTAL* pPortal = pSector->aPortals[i];

		//	Portal is closed ?
		if (!pPortal->bOpen)
			continue;

		//	Don't trace back
		if (pPortalFrom && pPortal == pPortalFrom->pOpposite)
			continue;

		//	This portal has been fully checked
		if (pPortal->rcPass.left <= -1.0f && pPortal->rcPass.right >= 1.0f &&
			pPortal->rcPass.bottom <= 1.0f && pPortal->rcPass.top >= 1.0f)
			continue;

		//	Skip portals that are back to us
		pDestSector	= pPortal->pFront;

		float fDot = DotProduct(vEyePos, pPortal->pPlane->vNormal) - pPortal->pPlane->fDist;
		if (fDot > 0.1f)
			continue;
		else if (fDot > 0.0f && DotProduct(vEyeDir, pPortal->pPlane->vNormal) <= 0.0f)
			continue;
	
		//	Clip portal with view frustum
		if (pFrustum->AABBInFrustum(pPortal->vMins, pPortal->vMaxs) < 0)
			continue;

		//	Project portal to view space and calculate it's 2D bounding box
		ProjectPortal(pPortal, &rcPortal);
		
		//	Intersect rectangle
		if (!IntersectFRect(&rcPortal, &rcPortal, pPtRect))
			continue;

		if (FRectInFRect(&rcPortal, &pPortal->rcPass))
			continue;

		UnionFRect(&pPortal->rcPass, &pPortal->rcPass, &rcPortal);

		Frustum.Init(pCamera, rcPortal.left, rcPortal.top, rcPortal.right, rcPortal.bottom, false);

		//	Recursively go through sectors
		GoThroughSector(pCamera, &Frustum, &rcPortal, pPortal, pDestSector);

		Frustum.Release();
	}

	//	Remove from trail
	pSector->bOnTrail = false;

	return true;
}

//	Project portal to identity view space and calculate it's 2D bounding box
void A3DPortal::ProjectPortal(PORTAL* pPortal, FRECT* pfRect)
{
	pfRect->left = pfRect->bottom = 999999.0f;
	pfRect->right = pfRect->top = -999999.0f;
	
	A3DVECTOR3 v1, v2;
	float w;
	int iCount = 0;

	for (int i=0; i < pPortal->iNumVert; i++)
	{
		//	Transform vertex from world space to view space
		v1 = pPortal->aVerts[i];

		w = 1.0f / (v1.x * m_matView._14 + v1.y * m_matView._24 + v1.z * m_matView._34 + m_matView._44);
		if (w < 0.0f)
		{
			iCount++;
			continue;
		}

		v2.Set(v1.x * m_matView._11 + v1.y * m_matView._21 + v1.z * m_matView._31 + m_matView._41,
			   v1.x * m_matView._12 + v1.y * m_matView._22 + v1.z * m_matView._32 + m_matView._42,
			   v1.x * m_matView._13 + v1.y * m_matView._23 + v1.z * m_matView._33 + m_matView._43);

		v2 *= w;

		if (v2.x < pfRect->left)	pfRect->left = v2.x;
		if (v2.x > pfRect->right)	pfRect->right = v2.x;
		if (v2.y < pfRect->bottom)	pfRect->bottom = v2.y;
		if (v2.y > pfRect->top)		pfRect->top = v2.y;
	}

	if (!iCount)
		return;

	if (iCount < pPortal->iNumVert)
	{
		pfRect->left	= -1.0f;
		pfRect->right	= 1.0f;
		pfRect->bottom	= -1.0f;
		pfRect->top		= 1.0f;
	}
	else	//	Complete clip out
	{
		pfRect->left	= -2.0f;
		pfRect->right	= -2.0f;
		pfRect->bottom	= -2.0f;
		pfRect->top		= -2.0f;
	}
}

/*	Check whether specified AABB is visible or not at current frame. Call
	GetVisibleSurfs() to update current frame's visible sectors and parts before
	this function is called.

	Return true if aabb visible, otherwise return false.

	aabb: aabb will be checked.
*/
bool A3DPortal::AABBIsVisible(const A3DAABB& aabb)
{
	if (!m_iNumBSPNodes)
		return true;

	m_dwSectorCnt++;

	int aLeaves[128];
	m_AABBLeaf.vMins		= aabb.Mins;
	m_AABBLeaf.vMaxs		= aabb.Maxs;
	m_AABBLeaf.iTopNode		= -1;
	m_AABBLeaf.iMaxLeafNum	= 128;
	m_AABBLeaf.iNumLeaf		= 0;
	m_AABBLeaf.aLeaves		= aLeaves;

	AABBIntersectLeaves(&m_aBSPNodes[0]);

	for (int i=0; i < m_AABBLeaf.iNumLeaf; i++)
	{
		SECTOR* pSector = m_aBSPNodes[aLeaves[i]].pSector;
		if (!pSector || pSector->dwTraceCnt != m_dwTraceCnt ||
			m_aSectorCnts[pSector->iIndex] == m_dwSectorCnt)
			continue;

		m_aSectorCnts[pSector->iIndex] = m_dwSectorCnt;

		//	If sectors are AABB, we can skip AABBCheckWithSector checking,
		//	Because in that case aabb must have intersected with sector now.
		if (!m_bAABBSector && !AABBCheckWithSector(aabb, pSector))
			continue;

		if (!m_bCheckParts)
			return true;

		for (int j=0; j < pSector->iNumPart; j++)
		{
			SECTORPART* pPart = &pSector->aParts[j];
			if (pPart->dwTraceCnt != m_dwTraceCnt)
				continue;

			if (CLS_AABBAABBOverlap(aabb.Center, aabb.Extents, pPart->aabb.Center, pPart->aabb.Extents))
				return true;
		}
	}

	return false;
}

/*	Check whether specified point is visible or not at current frame. Call
	GetVisibleSurfs() to update current frame's visible sectors and parts before
	this function is called.

	Return true if AABB is visible, otherwise return false.

	vPos: point will be checked.
*/
bool A3DPortal::PointIsVisible(const A3DVECTOR3& vPos)
{
	if (!m_iNumBSPNodes)
		return true;

	SECTOR* pSector = NULL;

	//	Search from root node
	BSPNODE* pNode = &m_aBSPNodes[0];

	while (1)
	{
		if (!pNode->pPlane)	//	Is a leaf
		{
			pSector = pNode->pSector;
			break;
		}

		A3DSPLANE* pPlane = pNode->pPlane;
		float d;
	
		if (pPlane->byType < 3)
			d = vPos.m[pPlane->byType] - pPlane->fDist;
		else if (pPlane->byType < 6)
			d = -vPos.m[pPlane->byType-3] - pPlane->fDist;
		else
			d = DotProduct(vPos, pPlane->vNormal) - pPlane->fDist;

		if (d >= 0)		//	Point is in front of plane
			pNode = pNode->aChildren[0];
		else
			pNode = pNode->aChildren[1];
	}

	if (!pSector || pSector->dwTraceCnt != m_dwTraceCnt)
		return false;

	if (!m_bCheckParts)
		return true;

	for (int i=0; i < pSector->iNumPart; i++)
	{
		SECTORPART* pPart = &pSector->aParts[i];
		if (pPart->dwTraceCnt != m_dwTraceCnt)
			continue;

		if (pSector->aabb.IsPointIn(vPos))
			return true;
	}
	
	return false;
}

//	Collision check between AABB and sector
bool A3DPortal::AABBCheckWithSector(const A3DAABB& aabb, SECTOR* pSector)
{
	//	Check sector's AABB first ?
	if (!CLS_AABBAABBOverlap(aabb.Center, aabb.Extents, pSector->aabb.Center, pSector->aabb.Extents))
		return false;
	
	//	Sector is just AABB
	if (m_bAABBSector)
		return true;

	float fDist, d1;
	const A3DVECTOR3& vExts = aabb.Extents;

	for (int i=0; i < pSector->aPlanes.GetSize(); i++)
	{
		A3DSPLANE* pPlane = pSector->aPlanes[i];

		A3DVECTOR3 vOffs;

		//	Push the plane out apropriately for mins/maxs
		switch (pPlane->bySignBits)
		{
		case 0:	vOffs.x = -vExts.x;		vOffs.y = -vExts.y;		vOffs.z = -vExts.z;		break;
		case 1:	vOffs.x = vExts.x;		vOffs.y = -vExts.y;		vOffs.z = -vExts.z;		break;
		case 2:	vOffs.x = -vExts.x;		vOffs.y = vExts.y;		vOffs.z = -vExts.z;		break;
		case 3:	vOffs.x = vExts.x;		vOffs.y = vExts.y;		vOffs.z = -vExts.z;		break;
		case 4:	vOffs.x = -vExts.x;		vOffs.y = -vExts.y;		vOffs.z = vExts.z;		break;
		case 5:	vOffs.x = vExts.x;		vOffs.y = -vExts.y;		vOffs.z = vExts.z;		break;
		case 6:	vOffs.x = -vExts.x;		vOffs.y = vExts.y;		vOffs.z = vExts.z;		break;
		case 7:	vOffs.x = vExts.x;		vOffs.y = vExts.y;		vOffs.z = vExts.z;		break;
		default: return false;
		}
		
		fDist = pPlane->fDist - DotProduct(vOffs, pPlane->vNormal);
		d1 = DotProduct(aabb.Center, pPlane->vNormal) - fDist;

		//	If completely in front of face, no intersection
		if (d1 > 0.0f)
			return false;
	}

	return true;
}

//	Intersection of AABB and leaves
void A3DPortal::AABBIntersectLeaves(BSPNODE* pNode)
{
	while (1)
	{
		if (!pNode->pPlane)
		{
			//	Is a leaf
			if (m_AABBLeaf.iNumLeaf >= m_AABBLeaf.iMaxLeafNum)
			{
				g_A3DErrLog.Log("A3DPortal::AABBIntersectLeaves, too many leaves");
				return;
			}

			m_AABBLeaf.aLeaves[m_AABBLeaf.iNumLeaf++] = pNode - m_aBSPNodes;
			return;
		}
	
		//	Is a node, then check children
		int s = CLS_PlaneAABBOverlap(*pNode->pPlane, m_AABBLeaf.vMins, m_AABBLeaf.vMaxs);

		if (s > 0)
			pNode = pNode->aChildren[0];
		else if (s < 0)
			pNode = pNode->aChildren[1];
		else
		{
			//	Go down both children
			if (m_AABBLeaf.iTopNode == -1)
				m_AABBLeaf.iTopNode = pNode - m_aBSPNodes;

			AABBIntersectLeaves(pNode->aChildren[0]);
			pNode = pNode->aChildren[1];
		}
	}
}

//	Get a portal through it's name
A3DPortal::PORTAL* A3DPortal::GetPortal(const char* szName, int* piIndex/* NULL */)
{
	for (int i=0; i < m_aPortals.GetSize(); i++)
	{
		PORTAL* pPortal = m_aPortals[i];
		if (pPortal->szName[0] && !_stricmp(pPortal->szName, szName))
		{
			if (piIndex)
				*piIndex = i;

			return pPortal;
		}
	}

	return NULL;
}

/*	Calculate sectors occupied by specified aabb

	aabb: aabb will be checked.
	aOccupiedFlags: buffer used to store sector occupied flag. Length (in bytes) of
			buffer must be >= sector number.
*/
void A3DPortal::CalcOccupiedSectors(const A3DAABB& aabb, BYTE* aOccupiedFlags)
{
	memset(aOccupiedFlags, 0, m_aSectors.GetSize());

	if (!m_iNumBSPNodes)
		return;

	int aLeaves[128];
	m_AABBLeaf.vMins		= aabb.Mins;
	m_AABBLeaf.vMaxs		= aabb.Maxs;
	m_AABBLeaf.iTopNode		= -1;
	m_AABBLeaf.iMaxLeafNum	= 128;
	m_AABBLeaf.iNumLeaf		= 0;
	m_AABBLeaf.aLeaves		= aLeaves;

	AABBIntersectLeaves(&m_aBSPNodes[0]);

	for (int i=0; i < m_AABBLeaf.iNumLeaf; i++)
	{
		SECTOR* pSector = m_aBSPNodes[aLeaves[i]].pSector;
		if (!pSector || aOccupiedFlags[pSector->iIndex])
			continue;

		//	If sectors are AABB, we can skip AABBCheckWithSector checking,
		//	Because in that case aabb must have intersected with sector now.
		if (!m_bAABBSector && !AABBCheckWithSector(aabb, pSector))
			continue;

		aOccupiedFlags[pSector->iIndex] = 1;
	}
}


