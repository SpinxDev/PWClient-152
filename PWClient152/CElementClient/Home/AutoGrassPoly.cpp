/*
 * FILE: AutoGrassPoly.h
 *
 * DESCRIPTION: Class for automaticlly generating polygon grass
 *
 * CREATED BY: Jiang Dalong, 2005/07/16
 *
 * HISTORY: 
 *
 * Copyright (c) 2001-2008 Archosaur Studio, All Rights Reserved.
 */

#pragma warning (disable: 4244)
#pragma warning (disable: 4018)

#include "AutoGrassPoly.h"
#include "AutoHome.h"
#include "AutoGrass.h"
#include "Render.h"
#include "AutoWater.h"
#include "AutoTerrain.h"
#include "AutoHomeCommon.h"
#include <A3DWireCollector.h>
#include <A3DEngine.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAutoGrassPoly::CAutoGrassPoly(CAutoHome* pAutoHome)
	: CAutoPartTerrainBase(pAutoHome)
{
	m_pAutoGrass = pAutoHome->GetAutoGrass();
}

CAutoGrassPoly::~CAutoGrassPoly()
{
	Release();
}

void CAutoGrassPoly::Release()
{
}

void CAutoGrassPoly::AddPoint(POINT_FLOAT pt)
{
	switch(m_status)
	{
	case ST_NOTSTART:
		m_Area.AddPoint(pt);
		m_status = ST_START;
		break;
	case ST_START:
		if (::Distance(pt, m_Area.GetPoint(0))<5)
		{
			CloseArea();
		}
		else if (!m_Area.IsNextPointCrossLine(pt, true))
		{
			m_Area.AddPoint(pt);
		}
		break;
	case ST_END:
		break;
	}

}

void CAutoGrassPoly::RemoveLastPoint()
{
	switch(m_status)
	{
	case ST_NOTSTART:
		break;
	case ST_START:
		if (m_Area.GetNumPoints()>0)
			m_Area.RemoveLastPoint();
		if (m_Area.GetNumPoints()==0)
			m_status = ST_NOTSTART;
		break;
	case ST_END:
		m_status = ST_START;
		break;
	}
}

void CAutoGrassPoly::RemoveAllPoints()
{
	// TODO: Add your message handler code here and/or call default
	switch(m_status)
	{
	case ST_NOTSTART:
		break;
	case ST_START:
		m_status = ST_NOTSTART;
		break;
	case ST_END:
		m_status = ST_NOTSTART;
		break;
	}

	m_Area.Clear();
}

bool CAutoGrassPoly::Render()
{
	return true;
}

// Is polygon valid
bool CAutoGrassPoly::IsPolygonValid()
{
#ifdef AUTO_TERRAIN_EDITOR
	if (m_status != ST_END)
		return false;
#endif

	if (!m_Area.IsPolygonValid())
		return false;

	return true;
}

// Set area points
bool CAutoGrassPoly::SetPoints(int nNumPoints, POINT_FLOAT* pPoints)
{
	if (!m_Area.SetPoints(nNumPoints, pPoints))
		return false;

	if (!CalArea())
		return false;

	return true;
}

// Get area points
bool CAutoGrassPoly::GetAreaPoints(int* pNumPoints, POINT_FLOAT** pPoints)
{
	*pNumPoints = m_Area.GetNumFinalPoints();
	*pPoints = m_Area.GetFinalPoints();

	return true;
}

bool CAutoGrassPoly::CalArea()
{
	int nLeft = m_nTerrainWidth;
	int nRight = -1;
	int nTop = m_nTerrainHeight;
	int nBottom = -1;
	int i;
	for (i=0; i<m_Area.GetNumPoints(); i++)
	{
		if (nLeft > m_Area.GetPoint(i).x)
			nLeft = m_Area.GetPoint(i).x;
		if (nRight < m_Area.GetPoint(i).x)
			nRight = m_Area.GetPoint(i).x;
		if (nTop > m_Area.GetPoint(i).y)
			nTop = m_Area.GetPoint(i).y;
		if (nBottom < m_Area.GetPoint(i).y)
			nBottom = m_Area.GetPoint(i).y;
	}

	m_ptAreaLeftTop.x = nLeft;
	m_ptAreaLeftTop.y = nTop;
	m_nAreaWidth = nRight - nLeft + 1;
	m_nAreaHeight = nBottom - nTop + 1;

	return true;
}

// Create grass poly
bool CAutoGrassPoly::CreateGrassPoly(int nGrassKind, float fDensity, int nNumGrassId, DWORD* pGrassIDs, float* pGrassWeights, DWORD dwSeed)
{
	int i, j;
	fDensity /= GRASS_DENSITY;
	
	float fTotalWeights = 0.0f;
	// Add grass types
	for (i=0; i<nNumGrassId; i++)
	{
		m_pAutoGrass->AddGrassType(pGrassIDs[i]);
		fTotalWeights += pGrassWeights[i];
	}
	// Normalize weights
	for (i=0; i<nNumGrassId; i++)
	{
		pGrassWeights[i] /= fTotalWeights;
	}

	srand(dwSeed);

	// Get area coordinate in grass map
	int nPosGrassMapLeft, nPosGrassMapRight, nPosGrassMapTop, nPosGrassMapBottom;
	m_pAutoGrass->HeightMapToGrassMap(m_ptAreaLeftTop.x, m_ptAreaLeftTop.y, &nPosGrassMapLeft, &nPosGrassMapTop);
	m_pAutoGrass->HeightMapToGrassMap(m_ptAreaLeftTop.x+m_nAreaWidth, m_ptAreaLeftTop.y+m_nAreaHeight, &nPosGrassMapRight, &nPosGrassMapBottom);

	int nPosHMX, nPosHMY;
	POINT_FLOAT ptHeightMap;
	int nWaterType;
	float fRandom;
	CAutoWater* pAutoWater = m_pAutoHome->GetAutoWater();
	for (i=nPosGrassMapTop; i<nPosGrassMapBottom; i++)
	{
		for (j=nPosGrassMapLeft; j<nPosGrassMapRight; j++)
		{
			m_pAutoGrass->GrassMapToHeightMap(j, i, &nPosHMX, &nPosHMY);
			ptHeightMap.x = nPosHMX;
			ptHeightMap.y = nPosHMY;
			// Is in area?
			if (!m_Area.IsInArea(ptHeightMap))
				continue;
			// Is water or sand?
			nWaterType = pAutoWater->CheckWater(nPosHMX, nPosHMY);
			if (nGrassKind == GK_LAND && nWaterType == WT_WATER)
				continue;
			if (nGrassKind == GK_WATER)
			{
				if (!pAutoWater->GetWaterReady())
					continue;
				if (nWaterType != WT_WATER)
					continue;
				if (pAutoWater->GetWaterHeight() - BELOW_WATER_HEIGHT < m_pAutoTerrain->GetHeightMap()->pAbsHeight[nPosHMY*m_nTerrainWidth+nPosHMX])
					continue;
			}
			// Keep density
			fRandom = MRANDOM;
			if (fRandom >= fDensity)
				continue;
			// Plant one grass on grass map
			m_pAutoGrass->PlantGrass(j, i, nNumGrassId, pGrassIDs, pGrassWeights);
		}
	}

	// Update grass by grass map
	m_pAutoGrass->UpdateGrassMaps();

	return true;
}

// Close area
bool CAutoGrassPoly::CloseArea()
{
	if (m_Area.GetNumPoints() < 3)
		return true;
	if (!m_Area.IsNextPointCrossLine((*m_Area.GetPoints())[0], false))
	{
		m_status = ST_END;
		m_Area.AreaFinish();
		if (!CalArea())
			return false;
	}
	return true;
}

// Store points
bool CAutoGrassPoly::StorePoints()
{
	return m_Area.StorePoints();
}

// Restore points
bool CAutoGrassPoly::RestorePoints()
{
	if (!m_Area.RestorePoints())
		return false;
	if (!CalArea())
		return false;

	return true;
}
