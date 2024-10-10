/*
 * FILE: A3DESP.cpp
 *
 * DESCRIPTION: Routines for ESP algorithm and file
 *
 * CREATED BY: duyuxin, 2001/10/12
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "A3DESP.h"
#include "A3DPI.h"
#include "A3DFuncs.h"
#include "AFile.h"
#include "AFileImage.h"
#include "A3DCollision.h"
#include "A3DEngine.h"
#include "AMemory.h"

///////////////////////////////////////////////////////////////////////////
//
//	Define and Macro
//
///////////////////////////////////////////////////////////////////////////

#define new A_DEBUG_NEW

#define ESPBRUSH_THICK		0.5f

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

A3DESP::A3DESP()
{
	m_pA3DEngine	= NULL;
	m_aPlanes		= NULL;
	m_aSides		= NULL;
	m_aLeaves		= NULL;
	m_aVerts		= NULL;
	m_aSideRefs		= NULL;
	m_aBrushes		= NULL;
	m_aPlaneRefs	= NULL;
	m_iNumPlane		= 0;
	m_iNumSide		= 0;
	m_iNumLeaf		= 0;
	m_iNumVert		= 0;
	m_iNumSideRef	= 0;
	m_iNumBrush		= 0;
	m_iNumPlaneRef	= 0;
	m_dwTraceCnt	= 0;
}

A3DESP::~A3DESP()
{
}

/*	Load ESP data from file and initialize object.

	Return true for success, otherwise return false.

	szFileName: .esp file's name.
*/
bool A3DESP::Load(const char* szFileName)
{
	AFileImage File;

	if (!File.Open(szFileName, AFILE_OPENEXIST))
	{
		File.Close();
		g_A3DErrLog.Log("A3DESP::Load(), Cannot open file %s!", szFileName);
		return false;
	}

	//	Release old resources
	Release();

	ESPFILEHEADER Header;
	DWORD dwRead;

	//	Read ESP file header
	if (!File.Read(&Header, sizeof (Header), &dwRead) || dwRead != sizeof (Header))
	{
		g_A3DErrLog.Log("A3DESP::Load(), Failed to read file header");
		File.Close();
		return false;
	}

	//	Check format and version
	if (Header.dwIdentify != ESP_IDENTIFY || Header.dwVersion != ESP_VERSION)
	{
		g_A3DErrLog.Log("A3DESP::Load(), Wrong file format or version");
		File.Close();
		return false;
	}

	EFLUMP* aLumps = new EFLUMP[Header.iNumLump];
	if (!aLumps)
	{
		g_A3DErrLog.Log("A3DESP::Load(), Not enough memory");
		File.Close();
		return false;
	}

	//	Read lump information
	File.Read(aLumps, Header.iNumLump * sizeof (EFLUMP), &dwRead);

	//	Read lumps
	ReadVertexLump(&File, aLumps);		//	Vertex lump before side lump
	ReadPlaneLump(&File, aLumps);		//	Plane lump before side and brush lump
	ReadBrushPlaneLump(&File, aLumps);	//	Plane reference lump before brush lump
	ReadBrushLump(&File, aLumps);		//	Brush lump before side lump
	ReadSideRefLump(&File, aLumps);		//	Side reference lump before leaf lump
	ReadSideLump(&File, aLumps);		//	Side lump before world lump
	ReadLeafLump(&File, aLumps);		//	Leaf lump before world lump
	ReadWorldInfoLump(&File, aLumps);

	delete [] aLumps;

	File.Close();

	return true;
}

//	Release all resources
void A3DESP::Release()
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

	if (m_aLeaves)
	{
		a_free(m_aLeaves);
		m_aLeaves = NULL;
		m_iNumLeaf = 0;
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

/*	Read a lump in ESP file.

	Return number of item in this lump

	pFile: file's pointer
	aLumps: lump informations
	iLump: lump will be read.
	ppBuf (out): address of a variable that will be set to lump data buffer's pointer.
	iSize: item size in lump
*/	
int	A3DESP::ReadLump(AFile* pFile, EFLUMP* aLumps, int iLump, void** ppBuf, int iSize)
{
	EFLUMP* pLump = &aLumps[iLump];
	DWORD dwRead;

	if (pLump->lSize % iSize)
		return -1;
	
	void *pBuf;
	if (!(pBuf = a_malloc(pLump->lSize)))
		return -1;

	pFile->Seek(pLump->lOff, AFILE_SEEK_SET);
	pFile->Read(pBuf, pLump->lSize, &dwRead);

	*ppBuf = pBuf;

	return pLump->lSize / iSize;
}

//	Read plane lump
bool A3DESP::ReadPlaneLump(AFile* pFile, EFLUMP* aLumps)
{
	EFPLANE* aEFPlanes;
	int iNumPlane = ReadLump(pFile, aLumps, EFLUMP_PLANES, (void**)&aEFPlanes, sizeof (EFPLANE));

	if (iNumPlane == -1)
	{
		g_A3DErrLog.Log("Failed to read plane lump A3DESP::ReadPlaneLump");
		return false;
	}
	
	A3DSPLANE* aPlanes = (A3DSPLANE*)a_malloc(iNumPlane * sizeof (A3DSPLANE));
	if (!aPlanes)
	{
		g_A3DErrLog.Log("Not enough memory in A3DESP::ReadPlaneLump");
		return false;
	}

	for (int i=0; i < iNumPlane; i++)
	{
		aPlanes[i].vNormal.x	= aEFPlanes[i].vNormal[0];
		aPlanes[i].vNormal.y	= aEFPlanes[i].vNormal[1];
		aPlanes[i].vNormal.z	= aEFPlanes[i].vNormal[2];
		aPlanes[i].fDist		= aEFPlanes[i].fDist;
		aPlanes[i].MakeTypeAndSignBits();
	}

	m_aPlanes	= aPlanes;
	m_iNumPlane	= iNumPlane;

	a_free(aEFPlanes);

	return true;
}

//	Read side lump
bool A3DESP::ReadSideLump(AFile* pFile, EFLUMP* aLumps)
{
	EFSIDE* aEFSides;
	int iNumSide = ReadLump(pFile, aLumps, EFLUMP_SIDES, (void**)&aEFSides, sizeof (EFSIDE));

	if (iNumSide == -1)
	{
		g_A3DErrLog.Log("Failed to read side lump A3DESP::ReadSideLump");
		return false;
	}
	
	ESPSIDE* aSides = (ESPSIDE*)a_malloc(iNumSide * sizeof (ESPSIDE));
	if (!aSides)
	{
		g_A3DErrLog.Log("Not enough memory in A3DESP::ReadSideLump");
		return false;
	}

	int i, j;
	A3DVECTOR3 vVert, vNormal;

	for (i=0; i < iNumSide; i++)
	{
		if (aEFSides[i].lPlane >= 0)
			aSides[i].pPlane = &m_aPlanes[aEFSides[i].lPlane];
		else
			aSides[i].pPlane = NULL;

		aSides[i].aVerts	 = &m_aVerts[aEFSides[i].lFirstVert];
		aSides[i].iNumVert	 = aEFSides[i].lNumVert;
		aSides[i].dwTraceCnt = 0;
		aSides[i].pBrush	 = &m_aBrushes[aEFSides[i].lBrush];

		aSides[i].MeshProps.dwProps = aEFSides[i].dwMeshProps;
		aSides[i].MeshProps.MtlType = (A3DMATERIALTYPE)aEFSides[i].iMtlType;
	
		//	Calculate side's bounding box
		a3d_ClearAABB(aSides[i].vMins, aSides[i].vMaxs);

		//	Nudge side out along it's normal
		if (aSides[i].pPlane)
			vNormal = aSides[i].pPlane->vNormal * (-ESPBRUSH_THICK);	//	Brush thick
		else
			vNormal = A3DVECTOR3(0.0f);

		for (j=0; j < aSides[i].iNumVert; j++)
		{
			vVert = aSides[i].aVerts[j];

			a3d_AddVertexToAABB(aSides[i].vMins, aSides[i].vMaxs, vVert);
			a3d_AddVertexToAABB(aSides[i].vMins, aSides[i].vMaxs, vVert + vNormal);

			//	Conside two sides
			if (aSides[i].MeshProps.Get2SidesFlag())
				a3d_AddVertexToAABB(aSides[i].vMins, aSides[i].vMaxs, vVert - vNormal);
		}

		//	Side's center and extents
		aSides[i].vCenter  = (aSides[i].vMins + aSides[i].vMaxs) * 0.5f;
		aSides[i].vExtents = (aSides[i].vMaxs - aSides[i].vMins) * 0.5f;
	}

	m_aSides	= aSides;
	m_iNumSide	= iNumSide;

	a_free(aEFSides);

	return true;
}

//	Read leaf lump
bool A3DESP::ReadLeafLump(AFile* pFile, EFLUMP* aLumps)
{
	EFLEAF* aEFLeaves;
	int iNumLeaf = ReadLump(pFile, aLumps, EFLUMP_LEAVES, (void**)&aEFLeaves, sizeof (EFLEAF));

	if (iNumLeaf == -1)
	{
		g_A3DErrLog.Log("Failed to read leaf lump A3DESP::ReadLeafLump");
		return false;
	}
	
	ESPLEAF* aLeaves = (ESPLEAF*)a_malloc(iNumLeaf * sizeof (ESPLEAF));
	if (!aLeaves)
	{
		g_A3DErrLog.Log("Not enough memory in A3DESP::ReadLeafLump");
		return false;
	}

	for (int i=0; i < iNumLeaf; i++)
	{
		aLeaves[i].aSideRefs	= &m_aSideRefs[aEFLeaves[i].lFirstLeafSide];
		aLeaves[i].iNumRefs		= aEFLeaves[i].lNumLeafSides;
	}

	m_aLeaves	= aLeaves;
	m_iNumLeaf	= iNumLeaf;

	a_free(aEFLeaves);

	return true;
}

//	Read ESP world information lump
bool A3DESP::ReadWorldInfoLump(AFile* pFile, EFLUMP* aLumps)
{
	EFWORLDINFO* aEFWorld;
	int iNumCluster = ReadLump(pFile, aLumps, EFLUMP_WORLDINFO, (void**)&aEFWorld, sizeof (EFLUMP_WORLDINFO));

	if (iNumCluster == -1)
	{
		g_A3DErrLog.Log("Failed to read cluster lump A3DESP::ReadClusterLump");
		return false;
	}
	
	for (int j=0; j < 3; j++)
	{
		m_World.vLength[j]		= aEFWorld[0].vLength[j];
		m_World.vSize[j]		= aEFWorld[0].vSize[j];
		m_World.vAlignMaxs.m[j]	= aEFWorld[0].vAlignMaxs[j];
		m_World.vAlignMins.m[j]	= aEFWorld[0].vAlignMins[j];
		m_World.vMaxs.m[j]		= aEFWorld[0].vMaxs[j];
		m_World.vMins.m[j]		= aEFWorld[0].vMins[j];
		m_World.vInvLength.m[j]	= 1.0f / aEFWorld[0].vLength[j];
	}

	m_World.vCenter		= (m_World.vMins + m_World.vMaxs) * 0.5f;
	m_World.vExtents	= (m_World.vMaxs - m_World.vMins) * 0.5f;
	m_World.iNumLeaves	= m_World.vSize[0] * m_World.vSize[1] * m_World.vSize[2];
	m_World.vSpans[0]	= 1;
	m_World.vSpans[1]	= m_World.vSize[0] * m_World.vSize[2];
	m_World.vSpans[2]	= m_World.vSize[0];
	m_World.dwTraceCnt	= 0;

	a_free(aEFWorld);

	return true;
}

//	Read vertex lump
bool A3DESP::ReadVertexLump(AFile* pFile, EFLUMP* aLumps)
{
	EFVERTEX* aEFVerts;
	int iNumVert = ReadLump(pFile, aLumps, EFLUMP_VERTICES, (void**)&aEFVerts, sizeof (EFVERTEX));

	if (iNumVert == -1)
	{
		g_A3DErrLog.Log("Failed to read vertex lump A3DESP::ReadVertexLump");
		return false;
	}
	
	A3DVECTOR3* aVerts = (A3DVECTOR3*)a_malloc(iNumVert * sizeof (A3DVECTOR3));
	if (!aVerts)
	{
		g_A3DErrLog.Log("Not enough memory in A3DESP::ReadVertexLump");
		return false;
	}

	for (int i=0; i < iNumVert; i++)
	{
		aVerts[i].x = aEFVerts[i].vPos[0];
		aVerts[i].y = aEFVerts[i].vPos[1];
		aVerts[i].z = aEFVerts[i].vPos[2];
	}

	m_aVerts	= aVerts;
	m_iNumVert	= iNumVert;

	a_free(aEFVerts);

	return true;
}

//	Read side reference lump
bool A3DESP::ReadSideRefLump(AFile* pFile, EFLUMP* aLumps)
{
	int* aRefs;
	int iNumRef = ReadLump(pFile, aLumps, EFLUMP_LEAFSIDES, (void**)&aRefs, sizeof (int));

	if (iNumRef == -1)
	{
		g_A3DErrLog.Log("Failed to read leaf side lump A3DESP::ReadSideRefLump");
		return false;
	}
	
	m_aSideRefs		= aRefs;
	m_iNumSideRef	= iNumRef;

	return true;
}

//	Read brush lump
bool A3DESP::ReadBrushLump(AFile* pFile, EFLUMP* aLumps)
{
	EFBRUSH* aEFBrushes;
	int iNumBrush = ReadLump(pFile, aLumps, EFLUMP_BRUSHES, (void**)&aEFBrushes, sizeof (EFBRUSH));

	if (iNumBrush == -1)
	{
		g_A3DErrLog.Log("A3DESP::ReadBrushLump, Failed to read brush lump");
		return false;
	}

	if (!iNumBrush)		//	ESP file may not contain brushes
	{
		m_aBrushes  = NULL;
		m_iNumBrush = 0;
		a_free(aEFBrushes);
		return true;
	}

	ESPBRUSH* aBrushes = (ESPBRUSH*)a_malloc(iNumBrush * sizeof (ESPBRUSH));
	if (!aBrushes)
	{
		g_A3DErrLog.Log("A3DESP::ReadBrushLump, Not enough memory");
		return false;
	}

	int i;
	for (i=0; i < iNumBrush; i++)
	{
		aBrushes[i].aPlanes		= &m_aPlaneRefs[aEFBrushes[i].lFirstPlane];
		aBrushes[i].iNumPlane	= aEFBrushes[i].lNumPlane;
	}

	m_aBrushes  = aBrushes;
	m_iNumBrush = iNumBrush;

	a_free(aEFBrushes);

	return true;
}

//	Read brush plane reference lump
bool A3DESP::ReadBrushPlaneLump(AFile* pFile, EFLUMP* aLumps)
{
	int* aRefs;
	int iNumRef = ReadLump(pFile, aLumps, EFLUMP_BRUSHPLANES, (void**)&aRefs, sizeof (int));

	if (iNumRef == -1)
	{
		g_A3DErrLog.Log("Failed to read plane reference lump A3DESP::ReadBrushPlaneLump");
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
		g_A3DErrLog.Log("Not enough memory, A3DESP::ReadBrushPlaneLump");
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

	pTrace (out): trace information.
	vStart: ray's start point of segment.
	vVelocity: velocity.
	fTime: time
*/
bool A3DESP::RayTrace(RAYTRACERT* pTrace, const A3DVECTOR3& vStart, const A3DVECTOR3& vVelocity, FLOAT fTime)
{
	if (!GetRayTraceEnable())
		return false;

	if (m_pA3DEngine)
		m_pA3DEngine->BeginPerformanceRecord(A3DEngine::PERF_TICK_SCENERAYTRACE);

	m_dwTraceCnt++;

	bool bRet = false;
	A3DVECTOR3 vDelta = vVelocity * fTime;

	//	Initialize data for ray trace
	m_Ray.vStart	= vStart;
	m_Ray.vEnd		= vStart + vDelta;
	m_Ray.vDir		= m_Ray.vEnd - m_Ray.vStart;
	m_Ray.pSide		= NULL;
	m_Ray.fFraction	= 1.0f;

	//	Is axial ray ?
	if (m_Ray.vDir.x == 0.0f)
	{
		if (m_Ray.vDir.z == 0.0f)
			m_Ray.iAxial = 1;
		else if (m_Ray.vDir.y == 0.0f)
			m_Ray.iAxial = 2;
	}
	else if (m_Ray.vDir.y == 0.0f && m_Ray.vDir.z == 0.0f)
		m_Ray.iAxial = 0;
	else
		m_Ray.iAxial = -1;

	pTrace->fFraction = 1.0f;
	pTrace->vPoint	  = m_Ray.vEnd;
	
	m_RayTraceRt.iNumCheckedSide = 0;

	if (RayToWorld())
	{
		pTrace->fFraction	 = m_Ray.fFraction;
		pTrace->vPoint		 = m_Ray.vPoint;
		pTrace->vNormal		 = m_Ray.vNormal;
		pTrace->vHitPos		 = m_Ray.vStart + m_Ray.vDir * m_Ray.fHitFrac;

		m_RayTraceRt.MeshProperty = m_Ray.pSide->MeshProps;

		bRet = true;
	}

	if (m_pA3DEngine)
		m_pA3DEngine->EndPerformanceRecord(A3DEngine::PERF_TICK_SCENERAYTRACE);

	return bRet;
}

/*	Check whether a ray collision with specified cluster, if true, trace ray into
	cluster and check which side in it hit by the ray.

	Return true if hit point in this cluster is the nearest one.
*/
bool A3DESP::RayToWorld()
{
	A3DVECTOR3 vHitCluster1, vHitCluster2, vNormal;
	FLOAT fFraction;

	//	No sides in this cluster
	if (!m_aSides)
		return false;
	
	A3DVECTOR3 vMins = m_World.vMins - A3DVECTOR3(0.2f);
	A3DVECTOR3 vMaxs = m_World.vMaxs + A3DVECTOR3(0.2f);

	//	Calculate hit point of ray (from start to end) and cluster
	if (!CLS_RayToAABB3(m_Ray.vStart, m_Ray.vDir, vMins, vMaxs, vHitCluster1, 
						&fFraction, vNormal))
		return false;

	//	Convert ray's direction
	A3DVECTOR3 vDir = m_Ray.vStart - m_Ray.vEnd;

	//	Calculate hit point of ray (from end to start) and cluster
	if (!CLS_RayToAABB3(m_Ray.vEnd, vDir, vMins, vMaxs, vHitCluster2, 
						&fFraction, vNormal))
		return false;

	m_pEverHitLeaf = NULL;

	if (m_Ray.iAxial >= 0)
	{
		//	Prepare to use 3DDDA algorithm
		AxialInit3DDDA(vHitCluster1, vHitCluster2);

		//	Trace ray in cluster with 3DDDA algorithm
		if (AxialTraceRayInWorld() || m_pEverHitLeaf)
			return true;
	}
	else
	{
		//	Prepare to use 3DDDA algorithm
		Init3DDDA(vHitCluster1, vHitCluster2);

		//	Trace ray in cluster with 3DDDA algorithm
		if (TraceRayInWorld() || m_pEverHitLeaf)
			return true;
	}

	return false;
}

/*	Initialize variables for tracing ray in cluster.

	v0: ray's start point
	v1: ray's end point
*/
void A3DESP::Init3DDDA(A3DVECTOR3& v0, A3DVECTOR3& v1)
{
	A3DVECTOR3 vDelta = v1 - v0;
	A3DVECTOR3 vAbs;
	int i, s[3], vStart[3], vEnd[3], vSteps[3];
	float d[3], fSlope1, fSlope2;

	for (i=0; i < 3; i++)
	{
		vStart[i]	= (int)((v0.m[i] - m_World.vAlignMins.m[i]) * m_World.vInvLength.m[i]);
		vEnd[i]		= (int)((v1.m[i] - m_World.vAlignMins.m[i]) * m_World.vInvLength.m[i]);

		if (vDelta.m[i] < 0.0f)
		{
			vAbs.m[i]	= -vDelta.m[i];
			s[i]		= -1;
			vSteps[i]	= vStart[i] - vEnd[i];
			d[i]		= v0.m[i] - (m_World.vAlignMins.m[i] + vStart[i] * m_World.vLength[i]);
		}
		else
		{
			vAbs.m[i]	= vDelta.m[i];
			s[i]		= 1;
			vSteps[i]	= vEnd[i] - vStart[i];
			d[i]		= m_World.vAlignMins.m[i] + (vStart[i] + 1) * m_World.vLength[i] - v0.m[i]; 
		}

		m_3DDDA.vStart[i]	= vStart[i];
		m_3DDDA.vEnd[i]		= vEnd[i];
	}

	if (vAbs.x > vAbs.y)
	{
		if (vAbs.x > vAbs.z)	//	x major
		{
			m_3DDDA.iMajor = 0;
			fSlope1 = vAbs.m[1] / vAbs.m[0];
			fSlope2 = vAbs.m[2] / vAbs.m[0];

			m_3DDDA.dy	= d[1];
			m_3DDDA.dz	= d[2];
			m_3DDDA.ey	= m_World.vLength[1] - d[1];
			m_3DDDA.ez	= m_World.vLength[2] - d[2];
			m_3DDDA.iy	= fSlope1 * m_World.vLength[0];
			m_3DDDA.iz	= fSlope2 * m_World.vLength[0];
			m_3DDDA.cy	= fSlope1 * d[0];
			m_3DDDA.cz	= fSlope2 * d[0];
		}
		else	//	z major
			m_3DDDA.iMajor = 2;
	}
	else
	{
		if (vAbs.y > vAbs.z)	//	y major
		{
			m_3DDDA.iMajor = 1;
			fSlope1 = vAbs.m[0] / vAbs.m[1];
			fSlope2 = vAbs.m[2] / vAbs.m[1];

			m_3DDDA.dx	= d[0];
			m_3DDDA.dz	= d[2];
			m_3DDDA.ex	= m_World.vLength[0] - d[0];
			m_3DDDA.ez	= m_World.vLength[2] - d[2];
			m_3DDDA.ix	= fSlope1 * m_World.vLength[1];
			m_3DDDA.iz	= fSlope2 * m_World.vLength[1];
			m_3DDDA.cx	= fSlope1 * d[1];
			m_3DDDA.cz	= fSlope2 * d[1];
		}
		else	//	z major
			m_3DDDA.iMajor = 2;
	}

	if (m_3DDDA.iMajor == 2)
	{
		fSlope1 = vAbs.m[0] / vAbs.m[2];
		fSlope2 = vAbs.m[1] / vAbs.m[2];
		
		m_3DDDA.dx	= d[0];
		m_3DDDA.dy	= d[1];
		m_3DDDA.ex	= m_World.vLength[0] - d[0];
		m_3DDDA.ey	= m_World.vLength[1] - d[1];
		m_3DDDA.ix	= fSlope1 * m_World.vLength[2];
		m_3DDDA.iy	= fSlope2 * m_World.vLength[2];
		m_3DDDA.cx	= fSlope1 * d[2];
		m_3DDDA.cy	= fSlope2 * d[2];
	}

	m_3DDDA.sx = s[0];
	m_3DDDA.sy = s[1];
	m_3DDDA.sz = s[2];
	m_3DDDA.iNumSteps = vSteps[m_3DDDA.iMajor];
}

//	Initialize variable for axial ray tracing in cluster
void A3DESP::AxialInit3DDDA(A3DVECTOR3& v0, A3DVECTOR3& v1)
{
	int i, s;

	m_3DDDA.iMajor = m_Ray.iAxial;

	for (i=0; i < 3; i++)
	{
		m_3DDDA.vStart[i]	= (int)((v0.m[i] - m_World.vAlignMins.m[i]) * m_World.vInvLength.m[i]);
		m_3DDDA.vEnd[i]		= (int)((v1.m[i] - m_World.vAlignMins.m[i]) * m_World.vInvLength.m[i]);

		if (i == m_3DDDA.iMajor)
		{
			if (m_3DDDA.vStart[i] <= m_3DDDA.vEnd[i])
			{
				m_3DDDA.iNumSteps = m_3DDDA.vEnd[i] - m_3DDDA.vStart[i];
				s = 1;
			}
			else
			{
				m_3DDDA.iNumSteps = m_3DDDA.vStart[i] - m_3DDDA.vEnd[i];
				s = -1;
			}
		}
	}

	m_3DDDA.sx = s;
	m_3DDDA.sy = s;
	m_3DDDA.sz = s;
}

/*	Trace ray in cluster with 3DDDA algorighm

	Return true if ray hit one side in current cluster
*/
bool A3DESP::TraceRayInWorld()
{
	int iSpanX, i, x, y, z;
	int iSpanY = m_World.vSpans[1];
	int iSpanZ = m_World.vSpans[2];

	x = m_3DDDA.vStart[0];
	y = m_3DDDA.vStart[1];
	z = m_3DDDA.vStart[2];

	ESPLEAF* pLeaf = &m_aLeaves[iSpanY*y + iSpanZ*z + x];

	iSpanX = m_3DDDA.sx;

	if (m_3DDDA.sy < 0)
		iSpanY = -iSpanY;

	if (m_3DDDA.sz < 0)
		iSpanZ = -iSpanZ;

	int sx = m_3DDDA.sx;
	int sy = m_3DDDA.sy;
	int sz = m_3DDDA.sz;

	//	Visit the start voxel
	if (TraceRayInLeaf(pLeaf))
		return true;

	//	Handle a special case, voxel increament on major axis is 0. When
	//	start and end point are near to each other, this case will occur
	if (!m_3DDDA.iNumSteps)
	{
		if (m_3DDDA.iMajor == 0)	//	x major
		{
			if (y != m_3DDDA.vEnd[1] && z != m_3DDDA.vEnd[2])
			{
				if (m_3DDDA.dy * m_3DDDA.iz - m_3DDDA.dz * m_3DDDA.iy > 0)
				{
					//	Visit (x, y, z+sz)
					if (TraceRayInLeaf(pLeaf + iSpanZ))
						return true;
				}
				else
				{
					//	Visit (x, y+sy, z)
					if (TraceRayInLeaf(pLeaf + iSpanY))
						return true;
				}

				//	Visit (x, y+sy, z+sz)
				if (TraceRayInLeaf(pLeaf + iSpanY + iSpanZ))
					return true;
			}
			else if (y != m_3DDDA.vEnd[1])
			{
				//	Visit (x, y+sy, z)
				if (TraceRayInLeaf(pLeaf + iSpanY))
					return true;
			}
			else if (z != m_3DDDA.vEnd[2])
			{
				//	Visit (x, y, z+sz)
				if (TraceRayInLeaf(pLeaf + iSpanZ))
					return true;
			}
		}
		else if (m_3DDDA.iMajor == 1)	//	y major
		{
			if (x != m_3DDDA.vEnd[0] && z != m_3DDDA.vEnd[2])
			{
				if (m_3DDDA.dx * m_3DDDA.iz - m_3DDDA.dz * m_3DDDA.ix > 0)
				{
					//	Visit (x, y, z+sz)
					if (TraceRayInLeaf(pLeaf + iSpanZ))
						return true;
				}
				else
				{
					//	Visit (x+sx, y, z)
					if (TraceRayInLeaf(pLeaf + iSpanX))
						return true;
				}

				//	Visit (x+sx, y, z+sz)
				if (TraceRayInLeaf(pLeaf + iSpanX + iSpanZ))
					return true;
			}
			else if (x != m_3DDDA.vEnd[0])
			{
				//	Visit (x+sx, y, z)
				if (TraceRayInLeaf(pLeaf + iSpanX))
					return true;
			}
			else if (z != m_3DDDA.vEnd[2])
			{
				//	Visit (x, y, z+sz)
				if (TraceRayInLeaf(pLeaf + iSpanZ))
					return true;
			}
		}
		else	//	z major
		{
			if (x != m_3DDDA.vEnd[0] && y != m_3DDDA.vEnd[1])
			{
				if (m_3DDDA.dx * m_3DDDA.iy - m_3DDDA.dy * m_3DDDA.ix > 0)
				{
					//	Visit (x, y+sy, z)
					if (TraceRayInLeaf(pLeaf + iSpanY))
						return true;
				}
				else
				{
					//	Visit (x+sx, y, z)
					if (TraceRayInLeaf(pLeaf + iSpanX))
						return true;
				}

				//	Visit (x+sx, y+sy, z)
				if (TraceRayInLeaf(pLeaf + iSpanX + iSpanY))
					return true;
			}
			else if (x != m_3DDDA.vEnd[0])
			{
				//	Visit (x+sx, y, z)
				if (TraceRayInLeaf(pLeaf + iSpanX))
					return true;
			}
			else if (y != m_3DDDA.vEnd[1])
			{
				//	Visit (x, y+sy, z)
				if (TraceRayInLeaf(pLeaf + iSpanY))
					return true;
			}
		}

		return false;
	}

	if (m_3DDDA.iMajor == 0)		//	x major
	{
		float ey	= m_3DDDA.ey;
		float ez	= m_3DDDA.ez;
		float fLenY = (float)m_World.vLength[1];
		float fLenZ = (float)m_World.vLength[2];
		bool bIncy	= false;
		bool bIncz	= false;
		int y1 = y, z1 = z;

		//	<= here is necessary. Through we may take one more step, but this
		//	ensure us won't miss any possible leaves.
		for (i=0; i <= m_3DDDA.iNumSteps; i++)
		{
			ey += m_3DDDA.iy;
			ez += m_3DDDA.iz;

			if (ey > fLenY)
			{
				y	   += m_3DDDA.sy;
				ey	   -= fLenY;
				pLeaf  += iSpanY;
				bIncy	= true;

				if (m_3DDDA.cy > m_3DDDA.dy)
					y1 = y;
			}

			if (ez > fLenZ)
			{
				z	   += m_3DDDA.sz;
				ez	   -= fLenZ;
				pLeaf  += iSpanZ;
				bIncz	= true;

				if (m_3DDDA.cz > m_3DDDA.dz)
					z1 = z;
			}

			x		+= m_3DDDA.sx;
			pLeaf	+= iSpanX;

			//	Ensure voxels are 6-connected
			if (bIncy && bIncz)
			{
				bIncy = bIncz = false;

				if (m_3DDDA.dy * m_3DDDA.iz - m_3DDDA.dz * m_3DDDA.iy > 0)
				{
					if (z1 == z)
					{
						//	Visit (x-sx, y-sy, z)
						if (TraceRayInLeaf(pLeaf - iSpanX - iSpanY))
							return true;

						if (y1 == y)
						{
							//	Visit (x-sx, y, z)
							if (TraceRayInLeaf(pLeaf - iSpanX))
								return true;
						}
						else
						{
							//	Visit (x, y-sy, z)
							if (TraceRayInLeaf(pLeaf - iSpanY))
								return true;
						}
					}
					else
					{
						//	Visit (x, y-sy, z-sz)
						if (TraceRayInLeaf(pLeaf - iSpanY - iSpanZ))
							return true;

						//	Visit (x, y-sy, z)
						if (TraceRayInLeaf(pLeaf - iSpanY))
							return true;
					}
				}
				else
				{
					if (y1 == y)
					{
						//	Visit (x-sx, y, z-sz)
						if (TraceRayInLeaf(pLeaf - iSpanX - iSpanZ))
							return true;

						if (z1 == z)
						{
							//	Visit (x-sx, y, z)
							if (TraceRayInLeaf(pLeaf - iSpanX))
								return true;
						}
						else
						{
							//	Visit (x, y, z-sz)
							if (TraceRayInLeaf(pLeaf - iSpanZ))
								return true;
						}
					}
					else
					{
						//	Visit (x, y-sy, z-sz)
						if (TraceRayInLeaf(pLeaf - iSpanY - iSpanZ))
							return true;

						//	Visit (x, y, z-sx)
						if (TraceRayInLeaf(pLeaf - iSpanZ))
							return true;
					}
				}
			}
			else if (bIncy)
			{
				if (y1 == y)
				{
					//	Visit (x-sx, y, z)
					if (TraceRayInLeaf(pLeaf - iSpanX))
						return true;
				}
				else
				{
					//	Visit (x, y-sy, z)
					if (TraceRayInLeaf(pLeaf - iSpanY))
						return true;
				}

				bIncy = false;
			}
			else if (bIncz)
			{
				if (z1 == z)
				{
					//	Visit (x-sx, y, z)
					if (TraceRayInLeaf(pLeaf - iSpanX))
						return true;
				}
				else
				{
					//	Visit (x, y, z-sz)
					if (TraceRayInLeaf(pLeaf - iSpanZ))
						return true;
				}

				bIncz = false;
			}

			//	Visit (x, y, z)
			if (TraceRayInLeaf(pLeaf))
				return true;

			m_3DDDA.dy = fLenY - ey;
			m_3DDDA.dz = fLenZ - ez;
		}
	}
	else if (m_3DDDA.iMajor == 1)	//	y major
	{
		float ex	= m_3DDDA.ex;
		float ez	= m_3DDDA.ez;
		float fLenX = (float)m_World.vLength[0];
		float fLenZ = (float)m_World.vLength[2];
		bool bIncx	= false;
		bool bIncz	= false;
		int x1 = x, z1 = z;

		//	<= here is necessary. Through we may take one more step, but this
		//	ensure us won't miss any possible leaves.
		for (i=0; i <= m_3DDDA.iNumSteps; i++)
		{
			ex += m_3DDDA.ix;
			ez += m_3DDDA.iz;

			if (ex > fLenX)
			{
				x	   += m_3DDDA.sx;
				ex	   -= fLenX;
				pLeaf  += iSpanX;
				bIncx	= true;

				if (m_3DDDA.cx > m_3DDDA.dx)
					x1 = x;
			}

			if (ez > fLenZ)
			{
				z	   += m_3DDDA.sz;
				ez	   -= fLenZ;
				pLeaf  += iSpanZ;
				bIncz	= true;

				if (m_3DDDA.cz > m_3DDDA.dz)
					z1 = z;
			}

			y		+= m_3DDDA.sy;
			pLeaf	+= iSpanY;

			//	Ensure voxels are 6-connected
			if (bIncx && bIncz)
			{
				bIncx = bIncz = false;

				if (m_3DDDA.dx * m_3DDDA.iz - m_3DDDA.dz * m_3DDDA.ix > 0)
				{
					if (z1 == z)
					{
						//	Visit (x-sx, y-sy, z)
						if (TraceRayInLeaf(pLeaf - iSpanX - iSpanY))
							return true;

						if (x1 == x)
						{
							//	Visit (x, y-sy, z)
							if (TraceRayInLeaf(pLeaf - iSpanY))
								return true;
						}
						else
						{
							//	Visit (x-sx, y, z)
							if (TraceRayInLeaf(pLeaf - iSpanX))
								return true;
						}
					}
					else
					{
						//	Visit (x-sx, y, z-sz)
						if (TraceRayInLeaf(pLeaf - iSpanX - iSpanZ))
							return true;

						//	Visit (x-sx, y, z)
						if (TraceRayInLeaf(pLeaf - iSpanX))
							return true;
					}
				}
				else
				{
					if (x1 == x)
					{
						//	Visit (x, y-sy, z-sz)
						if (TraceRayInLeaf(pLeaf - iSpanY - iSpanZ))
							return true;

						if (z1 == z)
						{
							//	Visit (x, y-sy, z)
							if (TraceRayInLeaf(pLeaf - iSpanY))
								return true;
						}
						else
						{
							//	Visit (x, y, z-sz)
							if (TraceRayInLeaf(pLeaf - iSpanZ))
								return true;
						}
					}
					else
					{
						//	Visit (x-sx, y, z-sz)
						if (TraceRayInLeaf(pLeaf - iSpanX - iSpanZ))
							return true;

						//	Visit (x, y, z-sz)
						if (TraceRayInLeaf(pLeaf - iSpanZ))
							return true;
					}
				}
			}
			else if (bIncx)
			{
				if (x1 == x)
				{
					//	Visit (x, y-sy, z)
					if (TraceRayInLeaf(pLeaf - iSpanY))
						return true;
				}
				else
				{
					//	Visit (x-sx, y, z)
					if (TraceRayInLeaf(pLeaf - iSpanX))
						return true;
				}

				bIncx = false;
			}
			else if (bIncz)
			{
				if (z1 == z)
				{
					//	Visit (x, y-sy, z)
					if (TraceRayInLeaf(pLeaf - iSpanY))
						return true;
				}
				else
				{
					//	Visit (x, y, z-sz)
					if (TraceRayInLeaf(pLeaf - iSpanZ))
						return true;
				}

				bIncz = false;
			}

			//	Visit (x, y, z)
			if (TraceRayInLeaf(pLeaf))
				return true;

			m_3DDDA.dx = fLenX - ex;
			m_3DDDA.dz = fLenZ - ez;
		}
	}
	else	//	z major
	{
		float ex	= m_3DDDA.ex;
		float ey	= m_3DDDA.ey;
		float fLenX = (float)m_World.vLength[0];
		float fLenY = (float)m_World.vLength[1];
		bool bIncx	= false;
		bool bIncy	= false;
		int x1 = x, y1 = y;

		//	<= here is necessary. Through we may take one more step, but this
		//	ensure us won't miss any possible leaves.
		for (i=0; i <= m_3DDDA.iNumSteps; i++)
		{
			ex += m_3DDDA.ix;
			ey += m_3DDDA.iy;

			if (ex > fLenX)
			{
				x	   += m_3DDDA.sx;
				ex	   -= fLenX;
				pLeaf  += iSpanX;
				bIncx	= true;

				if (m_3DDDA.cx > m_3DDDA.dx)
					x1 = x;
			}

			if (ey > fLenY)
			{
				y	   += m_3DDDA.sy;
				ey	   -= fLenY;
				pLeaf  += iSpanY;
				bIncy	= true;

				if (m_3DDDA.cy > m_3DDDA.dy)
					y1 = y;
			}

			z		+= m_3DDDA.sz;
			pLeaf	+= iSpanZ;

			//	Ensure voxels are 6-connected
			if (bIncx && bIncy)
			{
				bIncx = bIncy = false;

				if (m_3DDDA.dy * m_3DDDA.ix - m_3DDDA.dx * m_3DDDA.iy > 0)
				{
					if (x1 == x)
					{
						//	Visit (x, y-sy, z-sz)
						if (TraceRayInLeaf(pLeaf - iSpanY - iSpanZ))
							return true;

						if (y1 == y)
						{
							//	Visit (x, y, z-sz)
							if (TraceRayInLeaf(pLeaf - iSpanZ))
								return true;
						}
						else
						{
							//	Visit (x, y-sy, z)
							if (TraceRayInLeaf(pLeaf - iSpanY))
								return true;
						}
					}
					else
					{
						//	Visit (x-sx, y-sy, z)
						if (TraceRayInLeaf(pLeaf - iSpanX - iSpanY))
							return true;

						//	Visit (x, y-sy, z)
						if (TraceRayInLeaf(pLeaf - iSpanY))
							return true;
					}
				}
				else
				{
					if (y1 == y)
					{
						//	Visit (x-sx, y, z-sz)
						if (TraceRayInLeaf(pLeaf - iSpanX - iSpanZ))
							return true;

						if (x1 == x)
						{
							//	Visit (x, y, z-sz)
							if (TraceRayInLeaf(pLeaf - iSpanZ))
								return true;
						}
						else
						{
							//	Visit (x-sx, y, z)
							if (TraceRayInLeaf(pLeaf - iSpanX))
								return true;
						}
					}
					else
					{
						//	Visit (x-sx, y-sy, z)
						if (TraceRayInLeaf(pLeaf - iSpanX - iSpanY))
							return true;

						//	Visit (x-sx, y, z)
						if (TraceRayInLeaf(pLeaf - iSpanX))
							return true;
					}
				}
			}
			else if (bIncx)
			{
				if (x1 == x)
				{
					//	Visit (x, y, z-sz)
					if (TraceRayInLeaf(pLeaf - iSpanZ))
						return true;
				}
				else
				{
					//	Visit (x-sx, y, z)
					if (TraceRayInLeaf(pLeaf - iSpanX))
						return true;
				}

				bIncx = false;
			}
			else if (bIncy)
			{
				if (y1 == y)
				{
					//	Visit (x, y, z-sz)
					if (TraceRayInLeaf(pLeaf - iSpanZ))
						return true;
				}
				else
				{
					//	Visit (x, y-sy, z)
					if (TraceRayInLeaf(pLeaf - iSpanY))
						return true;
				}

				bIncy = false;
			}

			//	Visit (x, y, z)
			if (TraceRayInLeaf(pLeaf))
				return true;

			m_3DDDA.dx = fLenX - ex;
			m_3DDDA.dy = fLenY - ey;
		}	
	}

	return false;
}

//	Axial trace ray in current cluster
bool A3DESP::AxialTraceRayInWorld()
{
	int iSpan, i;

	ESPLEAF* pLeaf = &m_aLeaves[m_World.vSpans[1] * m_3DDDA.vStart[1] + 
								m_World.vSpans[2] * m_3DDDA.vStart[2] + 
								m_3DDDA.vStart[0]];
	switch (m_3DDDA.iMajor)
	{
	case 0:	//	x major

		iSpan = m_3DDDA.sx;
		break;

	case 1:	//	y major

		iSpan = m_World.vSpans[1];

		if (m_3DDDA.sy < 0)
			iSpan = -iSpan;
	
		break;

	case 2:	//	z major

		iSpan = m_World.vSpans[2];

		if (m_3DDDA.sz < 0)
			iSpan = -iSpan;

		break;
	}

	for (i=0; i <= m_3DDDA.iNumSteps; i++)
	{
		//	Visit voxel
		if (TraceRayInLeaf(pLeaf))
			return true;

		pLeaf += iSpan;
	}

	return false;
}

/*	Trace ray in leaf.

	Return true if hit point in this leaf is the nearest one.

	pLeaf: spedified leaf
*/
bool A3DESP::TraceRayInLeaf(ESPLEAF* pLeaf)
{
	//	This is necessary if we use "i <= steps" rather than 
	//	"i < steps" in TraceRayInCluster()
	if (pLeaf < m_aLeaves || pLeaf >= m_aLeaves + m_iNumLeaf)
		return false;

	int i;
	ESPSIDE* pSide;
	bool bRet = false;

	for (i=0; i < pLeaf->iNumRefs; i++)
	{
		pSide = &m_aSides[pLeaf->aSideRefs[i]];
		if (pSide->dwTraceCnt == m_dwTraceCnt)
			continue;

		m_RayTraceRt.iNumCheckedSide++; 

		if (RayToSide(pSide, pLeaf))
			bRet = true;

		pSide->dwTraceCnt = m_dwTraceCnt;
	}

	if (bRet)
		return true;
	else if (m_pEverHitLeaf == pLeaf)	//	Hit ever occures in this leaf ?
		return true;
	
	return false;
}

/*	Check whether a ray hit specified side and hit point is in specified leaf.

	Return true if the nearest hit occures in specified leaf, otherwise return false.

	pSide: specified side
	pLeaf: collision detection is be doing in this leaf
*/	
bool A3DESP::RayToSide(ESPSIDE* pSide, ESPLEAF* pLeaf)
{
	float d, d1, d2;
	A3DSPLANE* pPlane;
	A3DVECTOR3 vNormal;

	if (!(pPlane = pSide->pPlane))
		return false;

	if (pPlane->byType > 5)
	{
		d1 = DotProduct(m_Ray.vStart, pPlane->vNormal) - pPlane->fDist;
		d2 = DotProduct(m_Ray.vEnd, pPlane->vNormal) - pPlane->fDist;
	}
	else if (pPlane->byType < 3)
	{
		d1 = m_Ray.vStart.m[pPlane->byType] - pPlane->fDist;
		d2 = m_Ray.vEnd.m[pPlane->byType] - pPlane->fDist;
	}
	else	//	pPlane->byType < 6
	{
		d1 = -m_Ray.vStart.m[pPlane->byType-3] - pPlane->fDist;
		d2 = -m_Ray.vEnd.m[pPlane->byType-3] - pPlane->fDist;
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

	if (DotProduct(m_Ray.vDir, vNormal) >= 0.0f)
		return false;

	if (d1 < 0.0f)
		d = d1 / (d1 - d2);
	else
		d = d1 / (d1 - d2);

	if (d < 0.0f)
		d = 0.0f;

	if (d >= m_Ray.fFraction)
		return false;

	A3DVECTOR3 vInter;
	bool bInter=false, bFlag1, bFlag2;
	int i, i0, i1;
	float *vert1, *vert2, *vHit;

	//	Calculate intersection point of line and plane
	vInter	= m_Ray.vDir * d + m_Ray.vStart;
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
		m_Ray.fHitFrac	= d1 / (d1 - d2);
		m_Ray.fFraction = d1 < 0.0f ? (d1+0.01f) / (d1 - d2) : (d1-0.01f) / (d1 - d2);
		m_Ray.vPoint	= m_Ray.vStart + m_Ray.vDir * m_Ray.fFraction;
		m_Ray.pSide		= pSide;

		if (m_Ray.fFraction < 0.0f)
			m_Ray.fFraction = 0.0f;

		if (d1 >= 0)
			m_Ray.vNormal = pPlane->vNormal;
		else
			m_Ray.vNormal = A3DVECTOR3(0, 0, 0) - pPlane->vNormal;

		//	Check whether hit point is in current leaf
		int x = (int)((vInter.m[0] - m_World.vAlignMins.m[0]) * m_World.vInvLength.x);
		int y = (int)((vInter.m[1] - m_World.vAlignMins.m[1]) * m_World.vInvLength.y);
		int z = (int)((vInter.m[2] - m_World.vAlignMins.m[2]) * m_World.vInvLength.z);
		m_pEverHitLeaf = &m_aLeaves[y * m_World.vSpans[1] + z * m_World.vSpans[2] + x];

		if (m_pEverHitLeaf == pLeaf)
			return true;
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
bool A3DESP::SplitMark(const A3DAABB& aabb, const A3DVECTOR3& vNormal, A3DLVERTEX* aVerts, 
					   WORD* aIndices, bool bJog, int* piNumVert, int* piNumIdx,
					   float fRadiusScale/* 0.2f */)
{
	m_dwTraceCnt++;

	MARKSPLIT MarkSplit;

	MarkSplit.paabb		= (A3DAABB*) &aabb;
	MarkSplit.pvNormal	= (A3DVECTOR3*) &vNormal;
	MarkSplit.aVerts	= aVerts;
	MarkSplit.aIndices	= aIndices;
	MarkSplit.iNumVert	= 0;
	MarkSplit.iNumIdx	= 0;
	MarkSplit.fRadius	= aabb.Extents.x * fRadiusScale;
	MarkSplit.bJog		= bJog;
	MarkSplit.vSumDir	= A3DVECTOR3(0.0f);

	//	Check whether aabb intersect with world
	if (CLS_AABBAABBOverlap(aabb.Center, aabb.Extents, m_World.vCenter, m_World.vExtents))
		SplitMarkInWorld(&MarkSplit);

	//	Push all clipped vertices along a proper direction, this may fix
	//	splits in mark
	A3DVECTOR3 vJog = Normalize(MarkSplit.vSumDir) * (bJog ? 0.1f : 0.0f);

	for (int i=0; i < MarkSplit.iNumVert; i++)
	{
		aVerts[i].x = aVerts[i].x + vJog.x;
		aVerts[i].y = aVerts[i].y + vJog.y;
		aVerts[i].z = aVerts[i].z + vJog.z;
	}

	*piNumVert	= MarkSplit.iNumVert;
	*piNumIdx	= MarkSplit.iNumIdx;

	return true;
}

/*	Split mark in a cluster

	pMarkSplit: mark splitting information
*/
void A3DESP::SplitMarkInWorld(MARKSPLIT* pMarkSplit)
{
	int i, x, y, z, vMins[3], vMaxs[3];

	for (i=0; i < 3; i++)
	{
		vMins[i] = (int)((pMarkSplit->paabb->Mins.m[i] - m_World.vAlignMins.m[i]) * m_World.vInvLength.m[i]);
		vMaxs[i] = (int)((pMarkSplit->paabb->Maxs.m[i] - m_World.vAlignMins.m[i]) * m_World.vInvLength.m[i]);

		if (vMins[i] >= m_World.vSize[i] || vMaxs[i] < 0)
			return;

		if (vMins[i] < 0)
			vMins[i] = 0;

		if (vMaxs[i] >= m_World.vSize[i])
			vMaxs[i] = m_World.vSize[i] - 1;
	}

	//	Check all sides in those clusters
	int iSpanY = m_World.vSpans[1];
	int iSpanZ = m_World.vSpans[2];

	ESPLEAF *pBaseLeaf = &m_aLeaves[iSpanY*vMins[1] + iSpanZ*vMins[2] + vMins[0]];
	ESPLEAF *pLeafY, *pLeafZ, *pLeaf;
	ESPSIDE *pSide;

	pLeafY = pBaseLeaf;

	for (y=vMins[1]; y <= vMaxs[1]; y++, pLeafY+=iSpanY)
	{
		pLeafZ = pLeafY;

		for (z=vMins[2]; z <= vMaxs[2]; z++, pLeafZ+=iSpanZ)
		{
			pLeaf = pLeafZ;

			for (x=vMins[0]; x <= vMaxs[0]; x++, pLeaf++)
			{
				for (i=0; i < pLeaf->iNumRefs; i++)
				{
					pSide = &m_aSides[pLeaf->aSideRefs[i]];
					if (!pSide->pPlane || pSide->dwTraceCnt == m_dwTraceCnt ||
						pSide->MeshProps.GetNoMarkFlag())
						continue;

					pMarkSplit->pSide = pSide;
					if (!SplitMarkBySide(pMarkSplit))
						return;	//	Buffer has been full

					pSide->dwTraceCnt = m_dwTraceCnt;
				}
			}
		}
	}
}

/*	Split mark using a side.

	Return false if vertex or index buffer has been full, otherwise return true;

	pMarkSplit: mark splitting information
*/
bool A3DESP::SplitMarkBySide(MARKSPLIT* pMarkSplit)
{
	ESPSIDE* pSide	 = pMarkSplit->pSide;
	A3DSPLANE* pPlane = pSide->pPlane;
	A3DAABB* paabb	 = pMarkSplit->paabb;
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


