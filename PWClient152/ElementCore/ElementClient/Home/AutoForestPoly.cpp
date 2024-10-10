/*
 * FILE: AutoForestPoly.cpp
 *
 * DESCRIPTION: Class for automaticlly generating polygon forest
 *
 * CREATED BY: Jiang Dalong, 2005/07/12
 *
 * HISTORY: 
 *
 * Copyright (c) 2001-2008 Archosaur Studio, All Rights Reserved.
 */

#pragma warning (disable: 4244)
#pragma warning (disable: 4018)

#include "AutoForestPoly.h"
#include "AutoHome.h"
#include "Render.h"
#include "AutoTexture.h"
#include "AutoWater.h"
#include "AutoTerrain.h"
#include "AutoForest.h"
#include "AutoHomeCommon.h"
#include <A3DWireCollector.h>
#include <A3DEngine.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAutoForestPoly::CAutoForestPoly(CAutoHome* pAutoHome)
	: CAutoPartTerrainBase(pAutoHome)
{
	m_pAutoForest = pAutoHome->GetAutoForest();
}

CAutoForestPoly::~CAutoForestPoly()
{
	Release();
}

void CAutoForestPoly::Release()
{
}

void CAutoForestPoly::AddPoint(POINT_FLOAT pt)
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

void CAutoForestPoly::RemoveLastPoint()
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

void CAutoForestPoly::RemoveAllPoints() 
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

bool CAutoForestPoly::Render()
{
	return true;
}

// Is polygon valid
bool CAutoForestPoly::IsPolygonValid()
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
bool CAutoForestPoly::SetPoints(int nNumPoints, POINT_FLOAT* pPoints)
{
	if (!m_Area.SetPoints(nNumPoints, pPoints))
		return false;

	if (!CalArea())
		return false;

	return true;
}

// Get area points
bool CAutoForestPoly::GetAreaPoints(int* pNumPoints, POINT_FLOAT** pPoints)
{
	*pNumPoints = m_Area.GetNumFinalPoints();
	*pPoints = m_Area.GetFinalPoints();

	return true;
}

bool CAutoForestPoly::CalArea()
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

// Create forest poly
bool CAutoForestPoly::CreateForestPoly(int nPlantType, float fDensityX, float fDensityY, float fMaxSlope, 
	int nNumTreeId, DWORD* pTreeIDs, float* pTreeWeights, DWORD dwSeed)
{
	int i, j;
	float fDensity;
	if (nPlantType == PT_RANDOM)
		fDensity = fDensityX / TREE_DENSITY;
	float fTotalWeights = 0.0f;
	// Add tree types
	for (i=0; i<nNumTreeId; i++)
	{
		m_pAutoForest->AddTreeType(pTreeIDs[i]);
		fTotalWeights += pTreeWeights[i];
	}
	// Normalize weights
	for (i=0; i<nNumTreeId; i++)
	{
		pTreeWeights[i] /= fTotalWeights;
	}

	POINT_FLOAT point;
	float fRandom;
	float fMaxSlopeCos = (float)cos(fMaxSlope * 0.017453);
	float fSlopeCos;
	A3DVECTOR3 vNormal;
	int nSpaceX, nSpaceY;
	if (nPlantType == PT_RANDOM)
	{
		nSpaceX = 1;
		nSpaceY = 1;
	}
	else
	{
		nSpaceX = int(fDensityX + 0.5);
		nSpaceY = int(fDensityY + 0.5);
	}
	CAutoTexture* pTexture = m_pAutoHome->GetAutoTexture();
	int nMaskPosX, nMaskPosY;	// Position in mask map
	BYTE btColor;				// color in mask map
	srand(dwSeed);
	for (i=m_ptAreaLeftTop.y; i<m_ptAreaLeftTop.y+m_nAreaHeight; i+=nSpaceY)
	{
		for (j=m_ptAreaLeftTop.x; j<m_ptAreaLeftTop.x+m_nAreaWidth; j+=nSpaceX)
		{
			point.x = j;
			point.y = i;
			// If not in polygon, no tree
			if (!m_Area.IsInArea(point))
				continue;
			// If already be water or sand, no tree
			if (m_pAutoHome->GetAutoWater()->CheckWater(j, i) > WT_NO_WATER)
				continue;
			// If Is road, no tree
			nMaskPosX = (int)((float)j * MASK_SIZE / m_nTerrainWidth);
			nMaskPosY = (int)((float)i * MASK_SIZE / m_nTerrainHeight);
			pTexture->GetMaskPoint(TT_ROADBED, nMaskPosX, nMaskPosY, &btColor);
			if (btColor > 150)
				continue;
			pTexture->GetMaskPoint(TT_ROAD, nMaskPosX, nMaskPosY, &btColor);
			if (btColor > 10)
				continue;
			// To keep tree interval, some trees are not planted.
			if (nPlantType == PT_RANDOM)
			{
				fRandom = MRANDOM;
				if (fRandom >= fDensity)
					continue;
			}
			
			// If slope < max slope, then may be tree
			vNormal = m_pAutoTerrain->GetHeightMap()->aNormals[i*m_nTerrainWidth+j];
			vNormal.Normalize();
			fSlopeCos = DotProduct(vNormal, A3DVECTOR3(0, 1, 0));
			if (fSlopeCos < fMaxSlopeCos)
				continue;

			// Plant one tree
			m_pAutoForest->PlantTree(j, i, nNumTreeId, pTreeIDs, pTreeWeights);
		}
	}

	return true;
}

// Close area
bool CAutoForestPoly::CloseArea()
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
bool CAutoForestPoly::StorePoints()
{
	return m_Area.StorePoints();
}

// Restore points
bool CAutoForestPoly::RestorePoints()
{
	if (!m_Area.RestorePoints())
		return false;
	if (!CalArea())
		return false;

	return true;
}
