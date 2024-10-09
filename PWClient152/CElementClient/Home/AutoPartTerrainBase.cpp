/*
 * FILE: AutoPartTerrainBase.cpp
 *
 * DESCRIPTION: Class for base operation for part terrain
 *
 * CREATED BY: Jiang Dalong, 2005/05/06
 *
 * HISTORY: 
 *
 * Copyright (c) 2001-2008 Archosaur Studio, All Rights Reserved.
 */

#include "AutoPartTerrainBase.h"
#include "AutoHome.h"
#include "Render.h"
#include "AutoTerrain.h"
#include <A3DWireCollector.h>
#include <A3DEngine.h>
#include <A3DViewport.h>
#include <A3DTrace.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAutoPartTerrainBase::CAutoPartTerrainBase(CAutoHome* pAutoHome)
{
	m_pAutoHome = pAutoHome;
	m_pAutoTerrain = pAutoHome->GetAutoTerrain();
	m_nTerrainWidth = m_pAutoTerrain->GetHeightMap()->iWidth;
	m_nTerrainHeight = m_pAutoTerrain->GetHeightMap()->iWidth;

	m_status = ST_NOTSTART;
}

CAutoPartTerrainBase::~CAutoPartTerrainBase()
{
	Release();
}

void CAutoPartTerrainBase::OnLButtonDown(UINT nFlags, APointI point)
{
}

void CAutoPartTerrainBase::OnRButtonUp(UINT nFlags, APointI point)
{
}

void CAutoPartTerrainBase::OnRButtonDblClk(UINT nFlags, APointI point)
{
}

bool CAutoPartTerrainBase::Render()
{
	return true;
}

void CAutoPartTerrainBase::Release()
{
}
/*
bool CAutoPartTerrainBase::ScreenToHeightmap(POINT_FLOAT& pt)
{
	A3DVIEWPORTPARAM* pViewParam = g_Render.GetA3DViewport()->GetParam();
	// Check whether the point is in viewport
	if (pt.x < 0 || pt.x >= pViewParam->Width ||
		pt.y < 0 || pt.y >= pViewParam->Height)
		return false;

	// Remove screen border
	pt.x += pViewParam->X;
	pt.y += pViewParam->Y;

	A3DVECTOR3 vPos(pt.x, pt.y, 0);
	if (false)//m_pAutoTerrain->GetLineHeightFixed())
	{
		g_Render.GetA3DViewport()->InvTransform(vPos, vPos);
	}
	else
	{
		A3DVECTOR3 vEnd(pt.x, pt.y, 1.0f);
		A3DViewport* pViewport =g_Render.GetA3DEngine()->GetActiveViewport();
		A3DVECTOR3 vStart(pt.x, pt.y, 0.0f);
		pViewport->InvTransform(vStart,vStart);
		pViewport->InvTransform(vEnd,vEnd);

		RAYTRACERT TraceRt;	//	Used to store trace result
		if(!m_pAutoHome->GetTracePos(vStart, vEnd, vPos, false)) 
			return false;
	}

	vPos = vPos / 2;
	float fTerrainSize = m_pAutoTerrain->GetTerrainSize() / 2;
	float fHalfTerrainSize = fTerrainSize / 2;
	pt.x = vPos.x + fHalfTerrainSize;
	pt.y = fHalfTerrainSize - vPos.z;

	if (pt.x < 0 || pt.x >fTerrainSize || pt.y < 0 || pt.y > fTerrainSize)
		return false;

	return true;
}

bool CAutoPartTerrainBase::HeightmapToWorld(POINT_FLOAT pt, A3DVECTOR3& vPos)
{
	float fHalfTerrainSize = m_pAutoTerrain->GetTerrainSize() / 2;
	vPos.x = pt.x * 2 - fHalfTerrainSize;
	vPos.z = fHalfTerrainSize - pt.y * 2;
	vPos.y = 2000.0f;
	// If the height needs to be close to terrain
	if (true)//!m_pAutoTerrain->GetLineHeightFixed())
	{
		vPos.y = m_pAutoTerrain->GetPosHeight(vPos) + 5.0f;
	}

	return true;
}

bool CAutoPartTerrainBase::WorldToHeightmap(const A3DVECTOR3& vecIn, APointI* ptOut)
{
	float fTerrainSize = m_pAutoTerrain->GetTerrainSize() / 2;
	float fHalfTerrainSize = fTerrainSize / 2;
	ptOut->x = int(vecIn.x / 2 + fHalfTerrainSize + 0.5);
	ptOut->y = int(fTerrainSize - (vecIn.z / 2 + fHalfTerrainSize) + 0.5);

	return true;
}

ARectI CAutoPartTerrainBase::GetOrthoTerrainRect()
{
	float fHalfTerrainSize = m_pAutoTerrain->GetTerrainSize() / 2;
	A3DVECTOR3 vLeftTop(-fHalfTerrainSize, 0, fHalfTerrainSize);
	A3DVECTOR3 vBottomRight(fHalfTerrainSize, 0, -fHalfTerrainSize);
	g_Render.GetA3DViewport()->Transform(vLeftTop, vLeftTop);
	g_Render.GetA3DViewport()->Transform(vBottomRight, vBottomRight);

	ARectI rcOrthoTerrain;
	rcOrthoTerrain.left = int(vLeftTop.x + 0.5);
	rcOrthoTerrain.right = int(vBottomRight.x);
	rcOrthoTerrain.top = int(vLeftTop.y);
	rcOrthoTerrain.bottom = int(vBottomRight.y);

	return rcOrthoTerrain;
}
*/