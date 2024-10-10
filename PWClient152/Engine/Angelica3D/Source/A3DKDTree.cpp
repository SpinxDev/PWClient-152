/*
 * FILE: A3DKDTree.cpp
 *
 * DESCRIPTION: Routines for KD-Tree algorithm
 *
 * CREATED BY: duyuxin, 2003/8/21
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#include "A3DPI.h"
#include "A3DFuncs.h"
#include "A3DKDTree.h"
#include "A3DCollision.h"
#include "A3DEngine.h"
#include "AMemory.h"
#include "AFileImage.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

#define new A_DEBUG_NEW

#define ESPBRUSH_THICK		0.5f
#define	DIST_EPSILON		0.03125f

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

A3DKDTree::A3DKDTree()
{
	m_aPlanes		= NULL;
	m_aSides		= NULL;
	m_aNodes		= NULL;
	m_aVerts		= NULL;
	m_aSideRefs		= NULL;
	m_aBrushes		= NULL;
	m_aPlaneRefs	= NULL;
	m_iNumPlane		= 0;
	m_iNumSide		= 0;
	m_iNumNode		= 0;
	m_iNumVert		= 0;
	m_iNumSideRef	= 0;
	m_iNumBrush		= 0;
	m_iNumPlaneRef	= 0;
	m_dwTraceCnt	= 0;
}

A3DKDTree::~A3DKDTree()
{
}

/*	Load KD-Tree data from file and initialize object.

	Return true for success, otherwise return false.

	szFileName: .kdt file's name.
*/
bool A3DKDTree::Load(const char* szFileName)
{
	AFileImage File;

	if (!File.Open("", szFileName, AFILE_OPENEXIST))
	{
		File.Close();
		g_A3DErrLog.Log("A3DKDTree::Load(), Cannot open file %s!", szFileName);
		return false;
	}

	//	Release old resources
	Release();

	KDTFILEHEADER Header;
	DWORD dwRead;

	//	Read ESP file header
	if (!File.Read(&Header, sizeof (Header), &dwRead) || dwRead != sizeof (Header))
	{
		g_A3DErrLog.Log("A3DKDTree::Load(), Failed to read file header");
		File.Close();
		return false;
	}

	//	Check format and version
	if (Header.dwIdentify != KDTFILE_IDENTIFY || Header.dwVersion != KDTFILE_VERSION)
	{
		g_A3DErrLog.Log("A3DKDTree::Load(), Wrong file format or version");
		File.Close();
		return false;
	}

	KDTFLUMP* aLumps = new KDTFLUMP[Header.iNumLump];
	if (!aLumps)
	{
		g_A3DErrLog.Log("A3DKDTree::Load(), Not enough memory");
		File.Close();
		return false;
	}

	//	Read lump information
	File.Read(aLumps, Header.iNumLump * sizeof (KDTFLUMP), &dwRead);

	//	Read lumps
	ReadVertexLump(&File, aLumps);		//	Vertex lump before side lump
	ReadPlaneLump(&File, aLumps);		//	Plane lump before side and brush lump
	ReadBrushPlaneLump(&File, aLumps);	//	Plane reference lump before brush lump
	ReadBrushLump(&File, aLumps);		//	Brush lump before side lump
	ReadSideRefLump(&File, aLumps);		//	Side reference lump before leaf lump
	ReadSideLump(&File, aLumps);
	ReadNodeLump(&File, aLumps);

	delete [] aLumps;

	File.Close();

	return true;
}

//	Release all resources
void A3DKDTree::Release()
{
	if (m_aPlanes)
	{
		a_free(m_aPlanes);
		m_aPlanes = NULL;
		m_iNumPlane = 0;
	}

	if (m_aSides)
	{
		a_free(m_aSides);
		m_aSides = NULL;
		m_iNumSide = 0;
	}

	if (m_aNodes)
	{
		a_free(m_aNodes);
		m_aNodes = NULL;
		m_iNumNode = 0;
	}

	if (m_aVerts)
	{
		a_free(m_aVerts);
		m_aVerts = NULL;
		m_iNumVert = 0;
	}

	if (m_aSideRefs)
	{
		a_free(m_aSideRefs);
		m_aSideRefs = NULL;
		m_iNumSideRef = 0;
	}

	if (m_aBrushes)
	{
		a_free(m_aBrushes);
		m_aBrushes	= NULL;
		m_iNumBrush = 0;
	}

	if (m_aPlaneRefs)
	{
		a_free(m_aPlaneRefs);
		m_aPlaneRefs	= NULL;
		m_iNumPlaneRef	= 0;
	}
}

/*	Read a lump in KD-Tree file.

	Return number of item in this lump

	pFile: file's pointer
	aLumps: lump informations
	iLump: lump will be read.
	ppBuf (out): address of a variable that will be set to lump data buffer's pointer.
	dwSize: item size in lump
*/	
int	A3DKDTree::ReadLump(AFile* pFile, KDTFLUMP* aLumps, int iLump, void** ppBuf, DWORD dwSize)
{
	KDTFLUMP* pLump = &aLumps[iLump];
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

//	Read plane lump
bool A3DKDTree::ReadPlaneLump(AFile* pFile, KDTFLUMP* aLumps)
{
	KDTFPLANE* aFPlanes;
	int iNumPlane = ReadLump(pFile, aLumps, KDTLUMP_PLANES, (void**)&aFPlanes, sizeof (KDTFPLANE));

	if (iNumPlane == -1)
	{
		g_A3DErrLog.Log("Failed to read plane lump A3DKDTree::ReadPlaneLump");
		return false;
	}
	
	A3DSPLANE* aPlanes = (A3DSPLANE*)a_malloc(iNumPlane * sizeof (A3DSPLANE));
	if (!aPlanes)
	{
		g_A3DErrLog.Log("Not enough memory in A3DKDTree::ReadPlaneLump");
		return false;
	}

	for (int i=0; i < iNumPlane; i++)
	{
		A3DSPLANE* pDest = &aPlanes[i];
		KDTFPLANE* pSrc = &aFPlanes[i];

		pDest->vNormal.x	= pSrc->vNormal[0];
		pDest->vNormal.y	= pSrc->vNormal[1];
		pDest->vNormal.z	= pSrc->vNormal[2];
		pDest->fDist		= pSrc->fDist;
		pDest->MakeTypeAndSignBits();
	}

	m_aPlanes	= aPlanes;
	m_iNumPlane	= iNumPlane;

	a_free(aFPlanes);

	return true;
}

//	Read side lump
bool A3DKDTree::ReadSideLump(AFile* pFile, KDTFLUMP* aLumps)
{
	KDTFSIDE* aFSides;
	int iNumSide = ReadLump(pFile, aLumps, KDTLUMP_SIDES, (void**)&aFSides, sizeof (KDTFSIDE));

	if (iNumSide == -1)
	{
		g_A3DErrLog.Log("Failed to read side lump A3DKDTree::ReadSideLump");
		return false;
	}
	
	SIDE* aSides = (SIDE*)a_malloc(iNumSide * sizeof (SIDE));
	if (!aSides)
	{
		g_A3DErrLog.Log("Not enough memory in A3DKDTree::ReadSideLump");
		return false;
	}

	int i, j;
	A3DVECTOR3 vVert, vNormal;

	for (i=0; i < iNumSide; i++)
	{
		SIDE* pDest = &aSides[i];
		KDTFSIDE* pSrc = &aFSides[i];

		pDest->pPlane		= pSrc->iPlane < 0 ? NULL : &m_aPlanes[pSrc->iPlane];
		pDest->aVerts		= &m_aVerts[pSrc->iFirstVert];
		pDest->iNumVert		= pSrc->iNumVert;
		pDest->dwTraceCnt	= 0;
		pDest->pBrush		= &m_aBrushes[pSrc->iBrush];

		pDest->MeshProps.dwProps = pSrc->dwMeshProps;
		pDest->MeshProps.MtlType = (A3DMATERIALTYPE)pSrc->iMtlType;
	
		//	Calculate side's bounding box
		a3d_ClearAABB(pDest->vMins, pDest->vMaxs);

		//	Nudge side out along it's normal
		if (pDest->pPlane)
			vNormal = pDest->pPlane->vNormal * (-ESPBRUSH_THICK);	//	Brush thick
		else
			vNormal = A3DVECTOR3(0.0f);

		for (j=0; j < pDest->iNumVert; j++)
		{
			vVert = pDest->aVerts[j];

			a3d_AddVertexToAABB(pDest->vMins, pDest->vMaxs, vVert);
			a3d_AddVertexToAABB(pDest->vMins, pDest->vMaxs, vVert + vNormal);

			//	Conside two sides
			if (pDest->MeshProps.Get2SidesFlag())
				a3d_AddVertexToAABB(pDest->vMins, pDest->vMaxs, vVert - vNormal);
		}

		//	Side's center and extents
		pDest->vCenter  = (pDest->vMins + pDest->vMaxs) * 0.5f;
		pDest->vExtents = (pDest->vMaxs - pDest->vMins) * 0.5f;
	}

	m_aSides	= aSides;
	m_iNumSide	= iNumSide;

	a_free(aFSides);

	return true;
}

//	Read leaf lump
bool A3DKDTree::ReadNodeLump(AFile* pFile, KDTFLUMP* aLumps)
{
	KDTFNODE* aFNodes;
	int iNumNode = ReadLump(pFile, aLumps, KDTLUMP_NODES, (void**)&aFNodes, sizeof (KDTFNODE));

	if (iNumNode == -1)
	{
		g_A3DErrLog.Log("Failed to read leaf lump A3DKDTree::ReadNodeLump");
		return false;
	}
	
	NODE* aNodes = (NODE*)a_malloc(iNumNode * sizeof (NODE));
	if (!aFNodes)
	{
		g_A3DErrLog.Log("Not enough memory in A3DKDTree::ReadNodeLump");
		return false;
	}

	for (int i=0; i < iNumNode; i++)
	{
		NODE* pDest = &aNodes[i];
		KDTFNODE* pSrc = &aFNodes[i];

		pDest->aSideRefs	= &m_aSideRefs[pSrc->iFirstSide];
		pDest->iNumSideRef	= pSrc->iNumSide;
		pDest->pParent		= pSrc->iParent < 0 ? NULL : &aNodes[pSrc->iParent];
		pDest->aChildren[0]	= pSrc->aChildren[0] < 0 ? NULL : &aNodes[pSrc->aChildren[0]];
		pDest->aChildren[1]	= pSrc->aChildren[1] < 0 ? NULL : &aNodes[pSrc->aChildren[1]];
		pDest->pPlane		= pSrc->iPlane < 0 ? NULL : &m_aPlanes[pSrc->iPlane];

		pDest->aabb.Mins.Set(pSrc->vMins[0], pSrc->vMins[1], pSrc->vMins[2]);
		pDest->aabb.Maxs.Set(pSrc->vMaxs[0], pSrc->vMaxs[1], pSrc->vMaxs[2]);
		a3d_CompleteAABB(&pDest->aabb);
	}

	m_aNodes	= aNodes;
	m_iNumNode	= iNumNode;

	a_free(aFNodes);

	return true;
}

//	Read vertex lump
bool A3DKDTree::ReadVertexLump(AFile* pFile, KDTFLUMP* aLumps)
{
	KDTFVERTEX* aFVerts;
	int iNumVert = ReadLump(pFile, aLumps, KDTLUMP_VERTICES, (void**)&aFVerts, sizeof (KDTFVERTEX));

	if (iNumVert == -1)
	{
		g_A3DErrLog.Log("Failed to read vertex lump A3DKDTree::ReadVertexLump");
		return false;
	}
	
	A3DVECTOR3* aVerts = (A3DVECTOR3*)a_malloc(iNumVert * sizeof (A3DVECTOR3));
	if (!aVerts)
	{
		g_A3DErrLog.Log("Not enough memory in A3DKDTree::ReadVertexLump");
		return false;
	}

	for (int i=0; i < iNumVert; i++)
	{
		aVerts[i].x = aFVerts[i].vPos[0];
		aVerts[i].y = aFVerts[i].vPos[1];
		aVerts[i].z = aFVerts[i].vPos[2];
	}

	m_aVerts	= aVerts;
	m_iNumVert	= iNumVert;

	a_free(aFVerts);

	return true;
}

//	Read side reference lump
bool A3DKDTree::ReadSideRefLump(AFile* pFile, KDTFLUMP* aLumps)
{
	int* aRefs;
	int iNumRef = ReadLump(pFile, aLumps, KDTLUMP_LEAFSIDES, (void**)&aRefs, sizeof (int));

	if (iNumRef == -1)
	{
		g_A3DErrLog.Log("Failed to read leaf side lump A3DKDTree::ReadSideRefLump");
		return false;
	}
	
	m_aSideRefs		= aRefs;
	m_iNumSideRef	= iNumRef;

	return true;
}

//	Read brush lump
bool A3DKDTree::ReadBrushLump(AFile* pFile, KDTFLUMP* aLumps)
{
	KDTFBRUSH* aFBrushes;
	int iNumBrush = ReadLump(pFile, aLumps, KDTLUMP_BRUSHES, (void**)&aFBrushes, sizeof (KDTFBRUSH));

	if (iNumBrush == -1)
	{
		g_A3DErrLog.Log("A3DKDTree::ReadBrushLump, Failed to read brush lump");
		return false;
	}

	if (!iNumBrush)		//	ESP file may not contain brushes
	{
		m_aBrushes  = NULL;
		m_iNumBrush = 0;
		a_free(aFBrushes);
		return true;
	}

	BRUSH* aBrushes = (BRUSH*)a_malloc(iNumBrush * sizeof (BRUSH));
	if (!aBrushes)
	{
		g_A3DErrLog.Log("A3DKDTree::ReadBrushLump, Not enough memory");
		return false;
	}

	int i;
	for (i=0; i < iNumBrush; i++)
	{
		aBrushes[i].aPlanes		= &m_aPlaneRefs[aFBrushes[i].iFirstPlane];
		aBrushes[i].iNumPlane	= aFBrushes[i].iNumPlane;
	}

	m_aBrushes  = aBrushes;
	m_iNumBrush = iNumBrush;

	a_free(aFBrushes);

	return true;
}

//	Read brush plane reference lump
bool A3DKDTree::ReadBrushPlaneLump(AFile* pFile, KDTFLUMP* aLumps)
{
	int* aRefs;
	int iNumRef = ReadLump(pFile, aLumps, KDTLUMP_BRUSHPLANES, (void**)&aRefs, sizeof (int));

	if (iNumRef == -1)
	{
		g_A3DErrLog.Log("Failed to read plane reference lump A3DKDTree::ReadBrushPlaneLump");
		return false;
	}

	if (!iNumRef)		//	ESP file may not contain brushes
	{
		m_aPlaneRefs	= NULL;
		m_iNumPlaneRef	= 0;
		a_free(aRefs);
		return true;
	}

	A3DSPLANE** aPlanes = (A3DSPLANE**)a_malloc(iNumRef * sizeof (A3DSPLANE*));
	if (!aPlanes)
	{
		g_A3DErrLog.Log("Not enough memory, A3DKDTree::ReadBrushPlaneLump");
		return false;
	}
	
	for (int i=0; i < iNumRef; i++)
		aPlanes[i] = &m_aPlanes[aRefs[i]];

	m_aPlaneRefs	= aPlanes;
	m_iNumPlaneRef	= iNumRef;

	a_free(aRefs);

	return true;
}

/*	Do ray trace. This function search the side hit by ray and calculate the collision
	point.

	Return true ray hit a side, otherwise return false.

	pTraceRt (out): trace information.
	vStart: ray's start point of segment.
	vVelocity: velocity.
	fTime: time
*/
bool A3DKDTree::RayTrace(RAYTRACERT* pTraceRt, const A3DVECTOR3& vStart, const A3DVECTOR3& vVelocity, FLOAT fTime)
{
	if (!GetRayTraceEnable() || !m_aSides)
		return false;

	if (m_pA3DEngine)
	{
		m_pA3DEngine->BeginPerformanceRecord(A3DEngine::PERF_TICK_SCENERAYTRACE);
		m_pA3DEngine->IncFrameCount(A3DEngine::FRAMECNT_SCENERAYTRACE);
	}

	m_dwTraceCnt++;

	//	Initialize data for ray trace
	m_Trace.iTrace		= TRACE_RAY;
	m_Trace.vStart		= vStart;
	m_Trace.vDelta		= vVelocity * fTime;
	m_Trace.vEnd		= vStart + m_Trace.vDelta;
	m_Trace.pSide		= NULL;
	m_Trace.fFraction	= 1.0f;
	m_Trace.fHitFrac	= 1.0f;

	pTraceRt->fFraction	= 1.0f;
	pTraceRt->vPoint	= m_Trace.vEnd;

	m_RayTraceRt.iNumCheckedSide = 0;

	RecursiveTraceCheck(&m_aNodes[0], 0.0f, 1.0f, vStart, m_Trace.vEnd);

	bool bRet = m_Trace.fHitFrac < 1.0f ? true : false;
	if (bRet)
	{
		pTraceRt->fFraction	= m_Trace.fFraction;
		pTraceRt->vPoint	= m_Trace.vPoint;
		pTraceRt->vNormal	= m_Trace.vNormal;
		pTraceRt->vHitPos	= m_Trace.vStart + m_Trace.vDelta * m_Trace.fHitFrac;

		m_RayTraceRt.MeshProperty = m_Trace.pSide->MeshProps;
	}

	if (m_pA3DEngine)
		m_pA3DEngine->EndPerformanceRecord(A3DEngine::PERF_TICK_SCENERAYTRACE);

	return bRet;
}

//	Recursive trace checking
void A3DKDTree::RecursiveTraceCheck(NODE* pNode, float f1, float f2, const A3DVECTOR3& vP1, const A3DVECTOR3& vP2)
{
	//	Already hit something nearer
	if (m_Trace.fHitFrac <= f1)
		return;

	//	Is a leaf node ?
	if (!pNode->pPlane)
	{
		switch (m_Trace.iTrace)
		{
		case TRACE_RAY:		TraceRayInLeaf(pNode);		break;
		case TRACE_AABB:	TraceAABBInLeaf(pNode);		break;
		case TRACE_SPHERE:	TraceSphereInLeaf(pNode);	break;
		}

		return;
	}

	//	Find the point distances to the seperating plane and the offset for the
	//	size of the box. Note: In our KD-Tree, all seperating planes are axis
	//	parallel and point to positive direction.
	A3DSPLANE* pPlane = pNode->pPlane;
	ASSERT(pPlane->byType < 3);

	float t1 = vP1.m[pPlane->byType] - pPlane->fDist;
	float t2 = vP2.m[pPlane->byType] - pPlane->fDist;
	float fOffset;
	
	switch (m_Trace.iTrace)
	{
	case TRACE_RAY:		fOffset = 0.0f;	break;
	case TRACE_AABB:	fOffset = m_pAABBTraceInfo->vExtents.m[pPlane->byType];	break;
	case TRACE_SPHERE:	fOffset	= m_pSphereTraceInfo->fRadius;	break;
	}

	//	See which sides we need to consider
	if (t1 >= fOffset && t2 >= fOffset)
	{
		RecursiveTraceCheck(pNode->aChildren[0], f1, f2, vP1, vP2);
		return;
	}

	if (t1 < -fOffset && t2 < -fOffset)
	{
		RecursiveTraceCheck(pNode->aChildren[1], f1, f2, vP1, vP2);
		return;
	}

	A3DVECTOR3 vMid;
	float fFrac1, fFrac2, fDist, fMid;
	int iSide;

	//	Put the crosspoint DIST_EPSILON pixels on the near side
	if (t1 < t2)
	{
		fDist	= 1.0f / (t1 - t2);
		iSide	= 1;
		fFrac2	= (t1 + fOffset + DIST_EPSILON) * fDist;
		fFrac1	= (t1 - fOffset + DIST_EPSILON) * fDist;
	}
	else if (t1 > t2)
	{
		fDist	= 1.0f / (t1 - t2);
		iSide	= 0;
		fFrac2	= (t1 - fOffset - DIST_EPSILON) * fDist;
		fFrac1	= (t1 + fOffset + DIST_EPSILON) * fDist;
	}
	else
	{
		iSide	= 0;
		fFrac1	= 1.0f;
		fFrac2	= 0.0f;
	}

	//	Move up to the node
	a_Clamp(fFrac1, 0.0f, 1.0f);
		
	fMid = f1 + (f2 - f1) * fFrac1;
	vMid = vP1 + (vP2 - vP1) * fFrac1;

	RecursiveTraceCheck(pNode->aChildren[iSide], f1, fMid, vP1, vMid);

	//	Go past the node
	a_Clamp(fFrac2, 0.0f, 1.0f);
		
	fMid = f1 + (f2 - f1) * fFrac2;
	vMid = vP1 + (vP2 - vP1) * fFrac2;

	RecursiveTraceCheck(pNode->aChildren[iSide^1], fMid, f2, vMid, vP2);
}

/*	Trace ray in leaf.

	Return true if ray hit something in the leaf.

	pLeaf: spedified leaf
*/
bool A3DKDTree::TraceRayInLeaf(NODE* pLeaf)
{
	bool bRet = false;

	for (int i=0; i < pLeaf->iNumSideRef; i++)
	{
		SIDE* pSide = &m_aSides[pLeaf->aSideRefs[i]];
		if (pSide->dwTraceCnt == m_dwTraceCnt)
			continue;

		m_AABBTraceRt.iNumCheckedSide++; 

		if (RayToSide(pSide))
			bRet = true;

		pSide->dwTraceCnt = m_dwTraceCnt;
	}

	return bRet;
}

/*	Check whether a ray segment hit specified side and hit point is in
	specified leaf.

	Return true if the nearest hit occures in specified leaf, otherwise return false.

	pSide: specified side
*/	
bool A3DKDTree::RayToSide(SIDE* pSide)
{
	A3DSPLANE* pPlane;

	if (!(pPlane = pSide->pPlane))
		return false;

	float fFrac, d1, d2;
	A3DVECTOR3 vNormal;

	if (pPlane->byType > 5)
	{
		d1 = DotProduct(m_Trace.vStart, pPlane->vNormal) - pPlane->fDist;
		d2 = DotProduct(m_Trace.vEnd, pPlane->vNormal) - pPlane->fDist;
	}
	else if (pPlane->byType < 3)
	{
		d1 = m_Trace.vStart.m[pPlane->byType] - pPlane->fDist;
		d2 = m_Trace.vEnd.m[pPlane->byType] - pPlane->fDist;
	}
	else	//	pPlane->byType < 6
	{
		d1 = -m_Trace.vStart.m[pPlane->byType-3] - pPlane->fDist;
		d2 = -m_Trace.vEnd.m[pPlane->byType-3] - pPlane->fDist;
	}

	if ((d1 < 0.0f && d2 < 0.0f) || (d1 > 0.0f && d2 > 0.0f))
		return false;

	if (pSide->MeshProps.Get2SidesFlag())
		vNormal = d1 < 0.0f ? -pPlane->vNormal : pPlane->vNormal;
	else
	{
		//	Start point is behind side
		if (d1 < 0)
			return false;
	
		vNormal = pPlane->vNormal;
	}

	if (DotProduct(m_Trace.vDelta, vNormal) >= 0.0f)
		return false;

	if (d1 < 0.0f)
		fFrac = d1 / (d1 - d2);
	else
		fFrac = d1 / (d1 - d2);

	if (fFrac < 0.0f)
		fFrac = 0.0f;

	if (fFrac >= m_Trace.fFraction)
		return false;

	A3DVECTOR3 vInter;
	bool bInter=false, bFlag1, bFlag2;
	int i, i0, i1;
	float *vert1, *vert2, *vHit;

	//	Calculate intersection point of line and plane
	vInter	= m_Trace.vDelta * fFrac + m_Trace.vStart;
	vHit	= vInter.m;

	switch (pPlane->byType)
	{
	case A3DSPLANE::TYPE_PX:
	case A3DSPLANE::TYPE_NX:
	case A3DSPLANE::TYPE_MAJORX:	i0 = 1;	i1 = 2;	break;

	case A3DSPLANE::TYPE_PY:
	case A3DSPLANE::TYPE_NY:
	case A3DSPLANE::TYPE_MAJORY:	i0 = 0; i1 = 2; break;

	case A3DSPLANE::TYPE_PZ:
	case A3DSPLANE::TYPE_NZ:
	case A3DSPLANE::TYPE_MAJORZ:	i0 = 0; i1 = 1; break;

	default:	return false;
	}

	vert1	= pSide->aVerts[pSide->iNumVert-1].m;
	bFlag1	= (vHit[i1] >= vert1[i1]);

	for (i=0; i < pSide->iNumVert; i++)
	{
		vert2	= pSide->aVerts[i].m;
		bFlag2	= (vHit[i1] >= vert2[i1]);

		if (bFlag1 != bFlag2)
		{
			if (((vert2[i1] - vHit[i1]) * (vert1[i0] - vert2[i0]) >=
				(vert2[i0] - vHit[i0]) * (vert1[i1] - vert2[i1])) == bFlag2)
				bInter = !bInter;
		}

		vert1  = vert2;
		bFlag1 = bFlag2;
	}

	//	Calculate intersection point's position
	if (bInter)
	{
		m_Trace.fHitFrac	= fFrac;
		m_Trace.fFraction	= d1 < 0.0f ? (d1+0.01f) / (d1 - d2) : (d1-0.01f) / (d1 - d2);
		m_Trace.vPoint		= m_Trace.vStart + m_Trace.vDelta * m_Trace.fFraction;
		m_Trace.pSide		= pSide;

		if (m_Trace.fFraction < 0.0f)
			m_Trace.fFraction = 0.0f;

		if (d1 >= 0)
			m_Trace.vNormal = pPlane->vNormal;
		else
			m_Trace.vNormal = A3DVECTOR3(0.0f) - pPlane->vNormal;
	}

	return false;
}

//	Do AABB trace
bool A3DKDTree::AABBTrace(AABBTRACERT* pTraceRt, AABBTRACEINFO* pInfo)
{
	if (!GetAABBTraceEnable() || !m_aSides)
		return false;

	if (m_pA3DEngine)
	{
		m_pA3DEngine->BeginPerformanceRecord(A3DEngine::PERF_TICK_SCENEAABBTRACE);
		m_pA3DEngine->IncFrameCount(A3DEngine::FRAMECNT_SCENEAABBTRACE);
	}

	bool bRet = false;

	m_dwTraceCnt++;

	pTraceRt->fFraction	= 1.0f;
	pTraceRt->vDestPos	= pInfo->vEnd;

	if (pInfo->iType == 0)	//	Static AABB
	{
		int aLeaves[128];
		m_AABBLeaf.vMins		= pInfo->vStart - pInfo->vExtents;
		m_AABBLeaf.vMaxs		= pInfo->vStart + pInfo->vExtents;
		m_AABBLeaf.iTopNode		= -1;
		m_AABBLeaf.iMaxLeafNum	= 128;
		m_AABBLeaf.iNumLeaf		= 0;
		m_AABBLeaf.aLeaves		= aLeaves;

		AABBIntersectLeaves(&m_aNodes[0]);

		m_pAABBTraceInfo	= pInfo;
		m_Trace.iTrace		= TRACE_AABB;
		m_Trace.fHitFrac	= 1.0f;	
	
		for (int i=0; i < m_AABBLeaf.iNumLeaf; i++)
		{
			if (TraceAABBInLeaf(&m_aNodes[aLeaves[i]]))
			{
				pTraceRt->fFraction	= 0.0f;
				pTraceRt->vDestPos	= pInfo->vStart;
				pTraceRt->vNormal	= m_Trace.vNormal;

				m_AABBTraceRt.MeshProperty = m_Trace.pSide->MeshProps;

				bRet = true;
				break;
			}
		}
	}
	else
	{
		//	Initialize data for AABB trace
		m_Trace.iTrace		= TRACE_AABB;
		m_Trace.vStart		= pInfo->vStart;
		m_Trace.vDelta		= pInfo->vDelta;
		m_Trace.vEnd		= pInfo->vEnd;
		m_Trace.pSide		= NULL;
		m_Trace.fFraction	= 1.0f;
		m_Trace.fHitFrac	= 1.0f;

		m_pAABBTraceInfo	= pInfo;

		m_AABBTraceRt.iNumCheckedSide = 0;

		RecursiveTraceCheck(&m_aNodes[0], 0.0f, 1.0f, m_Trace.vStart, m_Trace.vEnd);

		if (m_Trace.fHitFrac < 1.0f)
		{
			pTraceRt->fFraction	= m_Trace.fHitFrac;
			pTraceRt->vDestPos	= m_Trace.vStart + m_Trace.vDelta * pTraceRt->fFraction;
			pTraceRt->vNormal	= m_Trace.vNormal;

			m_AABBTraceRt.MeshProperty = m_Trace.pSide->MeshProps;

			bRet = true;
		}
	}

	if (m_pA3DEngine)
		m_pA3DEngine->EndPerformanceRecord(A3DEngine::PERF_TICK_SCENEAABBTRACE);

	return bRet;
}

/*	Trace AABB in leaf.

	Return true if AABB hit something in the leaf.

	pLeaf: spedified leaf
*/
bool A3DKDTree::TraceAABBInLeaf(NODE* pLeaf)
{
	bool bRet = false;

	for (int i=0; i < pLeaf->iNumSideRef; i++)
	{
		SIDE* pSide = &m_aSides[pLeaf->aSideRefs[i]];
		if (pSide->dwTraceCnt == m_dwTraceCnt)
			continue;

		m_AABBTraceRt.iNumCheckedSide++; 

		if (AABBToSide(pSide))
			bRet = true;

		pSide->dwTraceCnt = m_dwTraceCnt;
	}

	return bRet;
}

/*	Collision routine for moving AABB and side.

	Return true if AABB hit side otherwise return false.

	pSide: side will be checked
*/
bool A3DKDTree::AABBToSide(SIDE* pSide)
{
	float fDist;
	int i;

	for (i=0; i < 3; i++)
	{
		fDist = pSide->vCenter.m[i] - m_pAABBTraceInfo->BoundAABB.Center.m[i];
		if (fDist < 0)
			fDist = -fDist;

		if (pSide->vExtents.m[i] + m_pAABBTraceInfo->BoundAABB.Extents.m[i] < fDist)
			return false;
	}

	A3DVECTOR3 vPos, vNormal = pSide->pPlane->vNormal;
	fDist = pSide->pPlane->fDist;

	//	AABB at start position is completely behind this side ?
	vPos.x = vNormal.x >= 0.0f ? m_pAABBTraceInfo->vStart.x + m_pAABBTraceInfo->vExtents.x :
			 m_pAABBTraceInfo->vStart.x - m_pAABBTraceInfo->vExtents.x;
	vPos.y = vNormal.y >= 0.0f ? m_pAABBTraceInfo->vStart.y + m_pAABBTraceInfo->vExtents.y :
			 m_pAABBTraceInfo->vStart.y - m_pAABBTraceInfo->vExtents.y;
	vPos.z = vNormal.z >= 0.0f ? m_pAABBTraceInfo->vStart.z + m_pAABBTraceInfo->vExtents.z :
			 m_pAABBTraceInfo->vStart.z - m_pAABBTraceInfo->vExtents.z;

	if (DotProduct(vPos, vNormal) - fDist < 0.0f)
	{
		if (pSide->MeshProps.Get2SidesFlag())
		{
			vNormal = -vNormal;
			fDist	= -fDist;
		}
		else if (m_pAABBTraceInfo->fTime != 0.0f)
			return false;
	}

	//	Ignore sides which will leave us
	if (m_pAABBTraceInfo->fTime != 0.0f)
	{
		if (!pSide->MeshProps.Get2SidesFlag() &&
			DotProduct(vNormal, m_pAABBTraceInfo->vDelta) > 0.0f)
			return false;
	}

	BRUSH* pBrush = pSide->pBrush;

	if (TRA_AABBMoveToBrush(m_pAABBTraceInfo, pBrush->aPlanes, pBrush->iNumPlane))
	{
		if (m_pAABBTraceInfo->fFraction < m_Trace.fHitFrac)
		{
			m_Trace.fHitFrac = m_pAABBTraceInfo->fFraction;
			m_Trace.pSide	 = pSide;

			if (m_pAABBTraceInfo->bStartSolid || m_pAABBTraceInfo->bAllSolid)
			//	This most likely happens when side has 2-sides property
				m_Trace.vNormal = vNormal;
			else
				m_Trace.vNormal = m_pAABBTraceInfo->ClipPlane.vNormal;

			return true;
		}
	}

	return false;
}

//	Intersection of AABB and leaves
void A3DKDTree::AABBIntersectLeaves(NODE* pNode)
{
	while (1)
	{
		if (!pNode->pPlane)
		{
			//	Is a leaf
			if (m_AABBLeaf.iNumLeaf >= m_AABBLeaf.iMaxLeafNum)
			{
				g_A3DErrLog.Log("A3DKDTree::AABBIntersectLeaves, too many leaves");
				return;
			}

			m_AABBLeaf.aLeaves[m_AABBLeaf.iNumLeaf++] = pNode - m_aNodes;
			return;
		}
	
		//	Is a node, then check children
		A3DSPLANE* pPlane = pNode->pPlane;
		ASSERT(pPlane->byType < 3);

		int s = 0;
		if (pPlane->fDist <= m_AABBLeaf.vMins.m[pPlane->byType])
			s = 1;
		else if (pPlane->fDist >= m_AABBLeaf.vMaxs.m[pPlane->byType])
			s = -1;

		if (s == 1)
			pNode = pNode->aChildren[0];
		else if (s == -1)
			pNode = pNode->aChildren[1];
		else
		{
			//	Go down both children
			if (m_AABBLeaf.iTopNode == -1)
				m_AABBLeaf.iTopNode = pNode - m_aNodes;

			AABBIntersectLeaves(pNode->aChildren[0]);
			pNode = pNode->aChildren[1];
		}
	}
}

//	Do sphere trace
bool A3DKDTree::SphereTrace(SPHERETRACERT* pTraceRt, SPHERETRACEINFO* pInfo)
{
	if (!GetSphereTraceEnable() || !m_aSides)
		return false;

	if (m_pA3DEngine)
	{
		m_pA3DEngine->BeginPerformanceRecord(A3DEngine::PERF_TICK_SCENESPHERETRACE);
		m_pA3DEngine->IncFrameCount(A3DEngine::FRAMECNT_SCENESPHERETRACE);
	}

	bool bRet = false;

	m_dwTraceCnt++;

	pTraceRt->fFraction	= 1.0f;
	pTraceRt->vDestPos	= pInfo->vEnd;

	if (pInfo->iType == 0)	//	Static sphere
	{
		int aLeaves[128];
		m_AABBLeaf.vMins		= pInfo->vStart - pInfo->fRadius;
		m_AABBLeaf.vMaxs		= pInfo->vStart + pInfo->fRadius;
		m_AABBLeaf.iTopNode		= -1;
		m_AABBLeaf.iMaxLeafNum	= 128;
		m_AABBLeaf.iNumLeaf		= 0;
		m_AABBLeaf.aLeaves		= aLeaves;

		AABBIntersectLeaves(&m_aNodes[0]);

		m_pSphereTraceInfo	= pInfo;
		m_Trace.iTrace		= TRACE_SPHERE;
		m_Trace.fHitFrac	= 1.0f;	
	
		for (int i=0; i < m_AABBLeaf.iNumLeaf; i++)
		{
			if (TraceSphereInLeaf(&m_aNodes[aLeaves[i]]))
			{
				pTraceRt->fFraction	= 0.0f;
				pTraceRt->vDestPos	= pInfo->vStart;
				pTraceRt->vNormal	= m_Trace.vNormal;

				m_SphereTraceRt.MeshProperty = m_Trace.pSide->MeshProps;

				bRet = true;
				break;
			}
		}
	}
	else
	{
		//	Initialize data for sphere trace
		m_Trace.iTrace		= TRACE_SPHERE;
		m_Trace.vStart		= pInfo->vStart;
		m_Trace.vDelta		= pInfo->vDelta;
		m_Trace.vEnd		= pInfo->vEnd;
		m_Trace.pSide		= NULL;
		m_Trace.fFraction	= 1.0f;
		m_Trace.fHitFrac	= 1.0f;

		m_pSphereTraceInfo	= pInfo;

		m_SphereTraceRt.iNumCheckedSide = 0;

		RecursiveTraceCheck(&m_aNodes[0], 0.0f, 1.0f, m_Trace.vStart, m_Trace.vEnd);

		if (m_Trace.fHitFrac < 1.0f)
		{
			pTraceRt->fFraction	= m_Trace.fHitFrac;
			pTraceRt->vDestPos	= m_Trace.vStart + m_Trace.vDelta * pTraceRt->fFraction;
			pTraceRt->vNormal	= m_Trace.vNormal;

			m_SphereTraceRt.MeshProperty = m_Trace.pSide->MeshProps;

			bRet = true;
		}
	}

	if (m_pA3DEngine)
		m_pA3DEngine->EndPerformanceRecord(A3DEngine::PERF_TICK_SCENESPHERETRACE);

	return bRet;
}

//	Trace sphere in leaf
bool A3DKDTree::TraceSphereInLeaf(NODE* pLeaf)
{
	bool bRet = false;

	for (int i=0; i < pLeaf->iNumSideRef; i++)
	{
		SIDE* pSide = &m_aSides[pLeaf->aSideRefs[i]];
		if (pSide->dwTraceCnt == m_dwTraceCnt)
			continue;

		m_SphereTraceRt.iNumCheckedSide++; 

		if (SphereToSide(pSide))
			bRet = true;

		pSide->dwTraceCnt = m_dwTraceCnt;
	}

	return bRet;
}

//	Collision routine for moving sphere and side
bool A3DKDTree::SphereToSide(SIDE* pSide)
{
	float fDist;
	int i;

	for (i=0; i < 3; i++)
	{
		fDist = pSide->vCenter.m[i] - m_pSphereTraceInfo->BoundAABB.Center.m[i];
		if (fDist < 0)
			fDist = -fDist;

		if (pSide->vExtents.m[i] + m_pSphereTraceInfo->BoundAABB.Extents.m[i] < fDist)
			return false;
	}

	A3DVECTOR3 vNormal(pSide->pPlane->vNormal);
	fDist = pSide->pPlane->fDist;

	//	Sphere at start position is completely behind this side ?
	if (DotProduct(m_pSphereTraceInfo->vStart, vNormal) - fDist < -m_pSphereTraceInfo->fRadius)
	{
		if (pSide->MeshProps.Get2SidesFlag())
		{
			vNormal = -vNormal;
			fDist	= -fDist;
		}
		else if (m_pSphereTraceInfo->fTime != 0.0f)
			return false;
	}

	//	Ignore sides which will leave us
	if (m_pSphereTraceInfo->fTime != 0.0f)
	{
		if (!pSide->MeshProps.Get2SidesFlag() &&
			DotProduct(vNormal, m_pSphereTraceInfo->vDelta) > 0.0f)
			return false;
	}

	BRUSH* pBrush = pSide->pBrush;

	if (TRA_SphereMoveToBrush(m_pSphereTraceInfo, pBrush->aPlanes, pBrush->iNumPlane))
	{
		if (m_pSphereTraceInfo->fFraction < m_Trace.fHitFrac)
		{
			m_Trace.fHitFrac = m_pSphereTraceInfo->fFraction;
			m_Trace.pSide	 = pSide;

			if (m_pSphereTraceInfo->bStartSolid || m_pSphereTraceInfo->bAllSolid)
			//	This most likely happens when side has 2-sides property
				m_Trace.vNormal = vNormal;
			else
				m_Trace.vNormal = m_pSphereTraceInfo->ClipPlane.vNormal;

			return true;
		}
	}

	return false;
}

/*	Split explosion mark.

	Return true for success, otherwise return false.
	
	aabb: explosion aabb.
	vNormal: normal of plane on which explosion occured.
	aVerts: buffer used to receive splited marks
	aIndices: buffer used to receive index
	bJog: true, push mark out a little.
	piNumVert (out): number of vertices in aVerts.
	piNumIdx (out): number of indices in aIndices.
*/
bool A3DKDTree::SplitMark(A3DAABB& aabb, A3DVECTOR3& vNormal, A3DLVERTEX* aVerts, 
						  WORD* aIndices, bool bJog, int* piNumVert, int* piNumIdx,
						  float fRadiusScale/* 0.2f */)
{
	m_dwTraceCnt++;

	MARKSPLIT MarkSplit;

	MarkSplit.paabb		= &aabb;
	MarkSplit.pvNormal	= &vNormal;
	MarkSplit.aVerts	= aVerts;
	MarkSplit.aIndices	= aIndices;
	MarkSplit.iNumVert	= 0;
	MarkSplit.iNumIdx	= 0;
	MarkSplit.fRadius	= aabb.Extents.x * fRadiusScale;
	MarkSplit.bJog		= bJog;
	MarkSplit.vSumDir	= A3DVECTOR3(0.0f);

	int aLeaves[128];
	m_AABBLeaf.vMins		= aabb.Mins;
	m_AABBLeaf.vMaxs		= aabb.Maxs;
	m_AABBLeaf.iTopNode		= -1;
	m_AABBLeaf.iMaxLeafNum	= 128;
	m_AABBLeaf.iNumLeaf		= 0;
	m_AABBLeaf.aLeaves		= aLeaves;

	AABBIntersectLeaves(&m_aNodes[0]);

	int i, j;

	for (i=0; i < m_AABBLeaf.iNumLeaf; i++)
	{
		NODE* pLeaf = &m_aNodes[m_AABBLeaf.aLeaves[i]];

		for (j=0; j < pLeaf->iNumSideRef; j++)
		{
			SIDE* pSide = &m_aSides[pLeaf->aSideRefs[j]];
			if (!pSide->pPlane || pSide->dwTraceCnt == m_dwTraceCnt ||
				pSide->MeshProps.GetNoMarkFlag())
				continue;

			MarkSplit.pSide = pSide;
			if (!SplitMarkBySide(&MarkSplit))
				goto End;	//	Buffer has been full

			pSide->dwTraceCnt = m_dwTraceCnt;
		}
	}

End:

	//	Push all clipped vertices along a proper direction, this may fix
	//	splits in mark
	A3DVECTOR3 vJog = Normalize(MarkSplit.vSumDir) * (bJog ? 0.1f : 0.0f);

	for (i=0; i < MarkSplit.iNumVert; i++)
	{
		aVerts[i].x = aVerts[i].x + vJog.x;
		aVerts[i].y = aVerts[i].y + vJog.y;
		aVerts[i].z = aVerts[i].z + vJog.z;
	}

	*piNumVert	= MarkSplit.iNumVert;
	*piNumIdx	= MarkSplit.iNumIdx;

	return true;
}

/*	Split mark using a side.

	Return false if vertex or index buffer has been full, otherwise return true;

	pMarkSplit: mark splitting information
*/
bool A3DKDTree::SplitMarkBySide(MARKSPLIT* pMarkSplit)
{
	SIDE* pSide = pMarkSplit->pSide;
	A3DSPLANE* pPlane = pSide->pPlane;
	A3DAABB* paabb = pMarkSplit->paabb;
	int i;
	float fDist;

	//	Check whether side is in affected area
	fDist = DotProduct(paabb->Center, pPlane->vNormal) - pPlane->fDist;
	if (fDist < -pMarkSplit->fRadius || fDist > pMarkSplit->fRadius)
		return true;

	if (DotProduct(*pMarkSplit->pvNormal, pPlane->vNormal) < 0.0f)
		return true;

	//	Judge the main axis of side plane
	int t0, t1, t2;
	float fBd1, fBd2, fBd3, fBd4;

	switch (pPlane->byType)
	{
	case A3DSPLANE::TYPE_PX:
	case A3DSPLANE::TYPE_NX:
	case A3DSPLANE::TYPE_MAJORX:	
		
		t0 = 0;	t1 = 2; t2 = 1;
		fBd1 = paabb->Center.z + paabb->Extents.z;
		fBd2 = paabb->Center.y + paabb->Extents.y;
		fBd3 = paabb->Center.z - paabb->Extents.z;
		fBd4 = paabb->Center.y - paabb->Extents.y;
		break;

	case A3DSPLANE::TYPE_PY:
	case A3DSPLANE::TYPE_NY:
	case A3DSPLANE::TYPE_MAJORY:

		t0 = 1;	t1 = 0; t2 = 2;
		fBd1 = paabb->Center.x + paabb->Extents.x;
		fBd2 = paabb->Center.z + paabb->Extents.z;
		fBd3 = paabb->Center.x - paabb->Extents.x;
		fBd4 = paabb->Center.z - paabb->Extents.z;
		break;

	case A3DSPLANE::TYPE_PZ:
	case A3DSPLANE::TYPE_NZ:
	case A3DSPLANE::TYPE_MAJORZ:

		t0 = 2;	t1 = 0; t2 = 1;
		fBd1 = paabb->Center.x + paabb->Extents.x;
		fBd2 = paabb->Center.y + paabb->Extents.y;
		fBd3 = paabb->Center.x - paabb->Extents.x;
		fBd4 = paabb->Center.y - paabb->Extents.y;
		break;

	default:
		return true;
	}

	A3DVECTOR3 aProj1[16], aProj2[16];	//	Vertices projection on main plane
	A3DVECTOR3 *v1, *v2, *aOld, *aNew;
	bool bIn1, bIn2;
	int iNumOld, iNumNew;
	float d, d1, d2;

	//	Copy side's vertex from mesh vertex array
	for (i=0; i < pSide->iNumVert; i++)
		aProj2[i] = pSide->aVerts[i];

	//	Clip side using the first border plane
	v1		= &aProj2[pSide->iNumVert-1];
	bIn1	= ((d1 = v1->m[t1] - fBd1) <= 0);
	iNumNew = 0;
	aNew	= aProj1;

	for (i=0; i < pSide->iNumVert; i++)
	{
		v2	 = &aProj2[i];
		bIn2 = ((d2 = v2->m[t1] - fBd1) <= 0);
	
		if (bIn1)
		{
			aNew[iNumNew] = *v1;
			iNumNew++;

			if (!bIn2)
			{
				d = d1 / (d1 - d2);
				aNew[iNumNew].m[t0] = v1->m[t0] + (v2->m[t0] - v1->m[t0]) * d;
				aNew[iNumNew].m[t1] = fBd1;
				aNew[iNumNew].m[t2] = v1->m[t2] + (v2->m[t2] - v1->m[t2]) * d;
				iNumNew++;
			}
		}
		else if (bIn2)
		{
			d = d1 / (d1 - d2);
			aNew[iNumNew].m[t0] = v1->m[t0] + (v2->m[t0] - v1->m[t0]) * d;
			aNew[iNumNew].m[t1] = fBd1;
			aNew[iNumNew].m[t2] = v1->m[t2] + (v2->m[t2] - v1->m[t2]) * d;
			iNumNew++;
		}

		bIn1 = bIn2;
		v1	 = v2;
		d1	 = d2;
	}

	if ((iNumOld = iNumNew) < 3)
		return true;

	//	Clip side using next border plane
	aOld	= aNew;
	aNew	= aProj2;
	v1		= &aOld[iNumOld-1];
	bIn1	= ((d1 = v1->m[t2] - fBd2) <= 0);
	iNumNew = 0;

	for (i=0; i < iNumOld; i++)
	{
		v2	 = &aOld[i];
		bIn2 = ((d2 = v2->m[t2] - fBd2) <= 0);
	
		if (bIn1)
		{
			aNew[iNumNew] = *v1;
			iNumNew++;

			if (!bIn2)
			{
				d = d1 / (d1 - d2);
				aNew[iNumNew].m[t0] = v1->m[t0] + (v2->m[t0] - v1->m[t0]) * d;
				aNew[iNumNew].m[t1] = v1->m[t1] + (v2->m[t1] - v1->m[t1]) * d;
				aNew[iNumNew].m[t2] = fBd2;
				iNumNew++;
			}
		}
		else if (bIn2)
		{
			d = d1 / (d1 - d2);
			aNew[iNumNew].m[t0] = v1->m[t0] + (v2->m[t0] - v1->m[t0]) * d;
			aNew[iNumNew].m[t1] = v1->m[t1] + (v2->m[t1] - v1->m[t1]) * d;
			aNew[iNumNew].m[t2] = fBd2;
			iNumNew++;
		}

		bIn1 = bIn2;
		v1	 = v2;
		d1	 = d2;
	}

	if ((iNumOld = iNumNew) < 3)
		return true;

	//	Clip side using next border plane
	aOld	= aNew;
	aNew	= aProj1;
	v1		= &aOld[iNumOld-1];
	bIn1	= ((d1 = v1->m[t1] - fBd3) >= 0);
	iNumNew = 0;

	for (i=0; i < iNumOld; i++)
	{
		v2	 = &aOld[i];
		bIn2 = ((d2 = v2->m[t1] - fBd3) >= 0);
	
		if (bIn1)
		{
			aNew[iNumNew] = *v1;
			iNumNew++;

			if (!bIn2)
			{
				d = d1 / (d1 - d2);
				aNew[iNumNew].m[t0] = v1->m[t0] + (v2->m[t0] - v1->m[t0]) * d;
				aNew[iNumNew].m[t1] = fBd3;
				aNew[iNumNew].m[t2] = v1->m[t2] + (v2->m[t2] - v1->m[t2]) * d;
				iNumNew++;
			}
		}
		else if (bIn2)
		{
			d = d1 / (d1 - d2);
			aNew[iNumNew].m[t0] = v1->m[t0] + (v2->m[t0] - v1->m[t0]) * d;
			aNew[iNumNew].m[t1] = fBd3;
			aNew[iNumNew].m[t2] = v1->m[t2] + (v2->m[t2] - v1->m[t2]) * d;
			iNumNew++;
		}

		bIn1 = bIn2;
		v1	 = v2;
		d1	 = d2;
	}

	if ((iNumOld = iNumNew) < 3)
		return true;

	//	Clip side using the last border plane
	aOld	= aNew;
	aNew	= aProj2;
	v1		= &aOld[iNumOld-1];
	bIn1	= ((d1 = v1->m[t2] - fBd4) >= 0);
	iNumNew = 0;

	for (i=0; i < iNumOld; i++)
	{
		v2	 = &aOld[i];
		bIn2 = ((d2 = v2->m[t2] - fBd4) >= 0);
	
		if (bIn1)
		{
			aNew[iNumNew] = *v1;
			iNumNew++;

			if (!bIn2)
			{
				d = d1 / (d1 - d2);
				aNew[iNumNew].m[t0] = v1->m[t0] + (v2->m[t0] - v1->m[t0]) * d;
				aNew[iNumNew].m[t1] = v1->m[t1] + (v2->m[t1] - v1->m[t1]) * d;
				aNew[iNumNew].m[t2] = fBd4;
				iNumNew++;
			}
		}
		else if (bIn2)
		{
			d = d1 / (d1 - d2);
			aNew[iNumNew].m[t0] = v1->m[t0] + (v2->m[t0] - v1->m[t0]) * d;
			aNew[iNumNew].m[t1] = v1->m[t1] + (v2->m[t1] - v1->m[t1]) * d;
			aNew[iNumNew].m[t2] = fBd4;
			iNumNew++;
		}

		bIn1 = bIn2;
		v1	 = v2;
		d1	 = d2;
	}

	if (iNumNew < 3)
		return true;

	//	Check whether buffer has been full
	if (pMarkSplit->iNumVert + iNumNew > MAXNUM_MARKVERT ||
		pMarkSplit->iNumIdx + (iNumNew - 2) * 3 > MAXNUM_MARKINDEX)
		return false;

	//	Calculate texture coordinates for left vertex
	A3DLVERTEX* pVert = &pMarkSplit->aVerts[pMarkSplit->iNumVert];
//	A3DVECTOR3 vJog = pPlane->vNormal * (pMarkSplit->bJog ? 0.1f : 0.0f);

	d1 = 1.0f / (fBd1 - fBd3);
	d2 = 1.0f / (fBd2 - fBd4);

	for (i=0; i < iNumNew; i++, pVert++)
	{
		pVert->x	= aNew[i].x; // + vJog.x;
		pVert->y	= aNew[i].y; // + vJog.y;
		pVert->z	= aNew[i].z; // + vJog.z;
		pVert->tu	= (aNew[i].m[t1] - fBd3) * d1;
		pVert->tv	= (aNew[i].m[t2] - fBd4) * d2;
	}

	//	Build triangle list
	WORD* aIndices = &pMarkSplit->aIndices[pMarkSplit->iNumIdx];
	
	iNumOld = 0;
	for (i=2; i < iNumNew; i++, aIndices+=3, iNumOld+=3)
	{
		aIndices[0] = pMarkSplit->iNumVert;
		aIndices[1] = pMarkSplit->iNumVert + i - 1;
		aIndices[2] = pMarkSplit->iNumVert + i;
	}

	pMarkSplit->vSumDir	= pMarkSplit->vSumDir + pPlane->vNormal;

	pMarkSplit->iNumVert += iNumNew;
	pMarkSplit->iNumIdx	 += iNumOld;

	return true;
}

