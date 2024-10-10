/*
 * FILE: A3DWorldCDS.cpp
 *
 * DESCRIPTION: The class that collect the collision detection interface of A3DWorld
 *
 * CREATED BY: Hedi, 2001/7/27
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "A3DTypes.h"
#include "A3DPI.h"
#include "A3DWorld.h"
#include "A3DTerrain.h"
#include "A3DWorldCDS.h"
#include "A3DScene.h"
#include "A3DESP.h"

A3DWorldCDS::A3DWorldCDS()
{
	m_pA3DWorld = NULL;
}

A3DWorldCDS::~A3DWorldCDS()
{
}

bool A3DWorldCDS::Init(A3DWorld * pA3DWorld)
{
	m_pA3DWorld = pA3DWorld;
	return true;
}

bool A3DWorldCDS::Release()
{
	return true;
}

bool A3DWorldCDS::RayTrace(const A3DVECTOR3& vStart, const A3DVECTOR3& vVel, FLOAT vTime, 
						   RAYTRACERT* pTraceRt, int iTraceObj, DWORD dwUserData)
{
	A3DModel* pModelExclude = (A3DModel*)dwUserData;
	return m_pA3DWorld->RayTrace(vStart, vVel, vTime, pTraceRt, pModelExclude);
}

bool A3DWorldCDS::AABBTrace(const A3DVECTOR3& vStart, const A3DVECTOR3& vExts, const A3DVECTOR3& vVel, 
							FLOAT fTime, AABBTRACERT* pTraceRt, int iTraceObj, DWORD dwUserData)
{
	A3DModel* pModelExclude = (A3DModel*)dwUserData;
	return m_pA3DWorld->AABBTrace(vStart, vExts, vVel, fTime, pTraceRt, pModelExclude);
}

//	Return true if this CDS can split mark
bool A3DWorldCDS::CanSplitMark()
{
	if (m_pA3DWorld->GetA3DESP() || m_pA3DWorld->GetA3DTerrain())
		return true;

	return false;
}

//	Split mark routine
bool A3DWorldCDS::SplitMark(const A3DAABB& aabb, const A3DVECTOR3& vNormal, A3DLVERTEX* aVerts, WORD* aIndices, 
					bool bJog, int* piNumVert, int* piNumIdx, float fRadiusScale/* 0.2f */)
{
	int iNumVert = 0, iNumIndex = 0;

	if (m_pA3DWorld->GetA3DESP())
	{
		if (!m_pA3DWorld->GetA3DESP()->SplitMark(aabb, vNormal, aVerts, aIndices, bJog, &iNumVert, &iNumIndex, fRadiusScale))
		{
			g_A3DErrLog.Log("A3DWorldCDS::SplitMark, Fail to split the mark on the wall");
			return false;
		}
	}

	if (iNumVert == 0 && m_pA3DWorld->GetA3DTerrain())
	{
		if (!m_pA3DWorld->GetA3DTerrain()->SplitMark(aabb, aVerts, aIndices, true, &iNumVert, &iNumIndex))
		{
			g_A3DErrLog.Log("A3DWorldCDS::SplitMark, Fail to split the mark on the terrain");
			return false;
		}
	}

	*piNumVert	= iNumVert;
	*piNumIdx	= iNumIndex;

	return true;
}

//	Position is visible ?
bool A3DWorldCDS::PosIsVisible(const A3DVECTOR3& vPos, int iVisObj, DWORD dwUserData)
{
	if (m_pA3DWorld->GetA3DScene())
		return m_pA3DWorld->GetA3DScene()->PosIsVisible(vPos);

	return true;
}

//	AABB is visible ?
bool A3DWorldCDS::AABBIsVisible(const A3DAABB& aabb, int iVisObj, DWORD dwUserData)
{
	return true;
}

//	AABB is visible ?
bool A3DWorldCDS::AABBIsVisible(const A3DVECTOR3& vMins, const A3DVECTOR3& vMaxs, int iVisObj, DWORD dwUserData)
{
	return true;
}

