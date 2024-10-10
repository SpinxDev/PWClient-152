/*
 * FILE: A3DESPMove.cpp
 *
 * DESCRIPTION: Part of CA3DESP
 *
 * CREATED BY: duyuxin, 2001/10/31
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "A3DPI.h"
#include "A3DESP.h"
#include "A3DCollision.h"
#include "A3DEngine.h"

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
//	Implement
//
///////////////////////////////////////////////////////////////////////////

/*	Do AABB trace in ESP world.

	Return true if AABB collision something, otherwise return false.

	pTrace: object used to receive trace result.
	pInfo: AABB trace information created by TRA_AABBTraceInit().
*/	
bool A3DESP::AABBTrace(AABBTRACERT* pTrace, AABBTRACEINFO* pInfo)
{
	//	ESP file may not contain brushes
	if (!GetAABBTraceEnable() || !m_aPlaneRefs)
		return false;

	if (m_pA3DEngine)
		m_pA3DEngine->BeginPerformanceRecord(A3DEngine::PERF_TICK_SCENEAABBTRACE);

	bool bRet = false;

	m_dwTraceCnt++;

	//	Trace AABB
	m_AABB.pInfo	 = pInfo;
	m_AABB.fFraction = 1.0f;

	m_AABBTraceRt.iNumCheckedSide = 0;

	if (CLS_AABBAABBOverlap(pInfo->BoundAABB.Center, pInfo->BoundAABB.Extents, 
					  m_World.vCenter, m_World.vExtents))
	{		
		if (TraceAABBInWorld())
		{
			//	Calculate real fraction between start and end point
			pTrace->fFraction	 = m_AABB.fFraction;
			pTrace->vDestPos	 = m_AABB.pInfo->vStart + m_AABB.pInfo->vDelta * pTrace->fFraction;
			pTrace->vNormal		 = m_AABB.vNormal;

			m_AABBTraceRt.MeshProperty = m_AABB.pSide->MeshProps;

			bRet = true;
		}
		else
		{
			pTrace->fFraction	= 1.0f;
			pTrace->vDestPos	= m_AABB.pInfo->vStart + m_AABB.pInfo->vDelta;
		}
	}

	if (m_pA3DEngine)
		m_pA3DEngine->EndPerformanceRecord(A3DEngine::PERF_TICK_SCENEAABBTRACE);

	return bRet;
}

//	Trace AABB in world
bool A3DESP::TraceAABBInWorld()
{
	bool bHit = false;
	int i, x, y, z, vMins[3], vMaxs[3];
	A3DVECTOR3 vBoxMins, vBoxMaxs;

	vBoxMins = m_AABB.pInfo->BoundAABB.Mins;
	vBoxMaxs = m_AABB.pInfo->BoundAABB.Maxs;

	for (i=0; i < 3; i++)
	{
		vMins[i] = (int)((vBoxMins.m[i] - m_World.vAlignMins.m[i]) * m_World.vInvLength.m[i]);
		vMaxs[i] = (int)((vBoxMaxs.m[i] - m_World.vAlignMins.m[i]) * m_World.vInvLength.m[i]);

		if (vMins[i] >= m_World.vSize[i] || vMaxs[i] < 0)
			return false;

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
					if (!pSide->pPlane || pSide->dwTraceCnt == m_dwTraceCnt)
						continue;

					m_AABBTraceRt.iNumCheckedSide++; 

					if (AABBMoveToSide(pSide))
						bHit = true;

					pSide->dwTraceCnt = m_dwTraceCnt;
				}
			}
		}
	}

	return bHit;
}

/*	Collision routine for moving AABB and side.

	Return true if OBB and quadrangle collision and their intersection point is the
	nearest one, otherwise return false.

	pSide: side will be checked
*/
bool A3DESP::AABBMoveToSide(ESPSIDE* pSide)
{
	float fDist;
	int i;

	for (i=0; i < 3; i++)
	{
		fDist = pSide->vCenter.m[i] - m_AABB.pInfo->BoundAABB.Center.m[i];
		if (fDist < 0)
			fDist = -fDist;

		if (pSide->vExtents.m[i] + m_AABB.pInfo->BoundAABB.Extents.m[i] < fDist)
			return false;
	}

	A3DVECTOR3 vPos, vNormal = pSide->pPlane->vNormal;
	fDist = pSide->pPlane->fDist;

	//	Start position is behind this side ?
	vPos.x = vNormal.x >= 0.0f ? m_AABB.pInfo->vStart.x + m_AABB.pInfo->vExtents.x :
			 m_AABB.pInfo->vStart.x - m_AABB.pInfo->vExtents.x;
	vPos.y = vNormal.y >= 0.0f ? m_AABB.pInfo->vStart.y + m_AABB.pInfo->vExtents.y :
			 m_AABB.pInfo->vStart.y - m_AABB.pInfo->vExtents.y;
	vPos.z = vNormal.z >= 0.0f ? m_AABB.pInfo->vStart.z + m_AABB.pInfo->vExtents.z :
			 m_AABB.pInfo->vStart.z - m_AABB.pInfo->vExtents.z;

	if (DotProduct(vPos, vNormal) - fDist < 0.0f)
	{
		if (pSide->MeshProps.Get2SidesFlag())
		{
			vNormal = -vNormal;
			fDist	= -fDist;
		}
		else if (m_AABB.pInfo->fTime != 0.0f)
			return false;
	}

	//	Ignore sides which will leave us
	if (m_AABB.pInfo->fTime != 0.0f)
	{
		if (!pSide->MeshProps.Get2SidesFlag() &&
			DotProduct(vNormal, m_AABB.pInfo->vDelta) > 0.0f)
			return false;
	}

	ESPBRUSH* pBrush = pSide->pBrush;

	if (TRA_AABBMoveToBrush(m_AABB.pInfo, pBrush->aPlanes, pBrush->iNumPlane))
	{
		if (m_AABB.pInfo->fFraction < m_AABB.fFraction)
		{
			m_AABB.fFraction = m_AABB.pInfo->fFraction;
			m_AABB.pSide	 = pSide;

			if (m_AABB.pInfo->bStartSolid || m_AABB.pInfo->bAllSolid)
			//	This most likely happens when side has 2-sides property
				m_AABB.vNormal = vNormal;
			else
				m_AABB.vNormal = m_AABB.pInfo->ClipPlane.vNormal;

			return true;
		}
	}

	return false;
}

