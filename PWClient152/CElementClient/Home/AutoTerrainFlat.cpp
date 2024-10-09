/*
 * FILE: AutoTerrainFlat.cpp
 *
 * DESCRIPTION: Class for automaticlly flat terrain
 *
 * CREATED BY: Jiang Dalong, 2005/08/08
 *
 * HISTORY: 
 *
 * Copyright (c) 2001-2008 Archosaur Studio, All Rights Reserved.
 */
#pragma warning (disable: 4244)
#pragma warning (disable: 4018)

#include "AutoTerrainFlat.h"
#include "AutoHome.h"
#include "Render.h"
#include "AutoTerrain.h"
#include <A3DWireCollector.h>
#include <A3DEngine.h>
#include <AAssist.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAutoTerrainFlat::CAutoTerrainFlat(CAutoHome* pAutoHome)
	: CAutoPartTerrainBase(pAutoHome)
{

}

CAutoTerrainFlat::~CAutoTerrainFlat()
{
	Release();
}

void CAutoTerrainFlat::Release()
{
}

void CAutoTerrainFlat::OnLButtonDown(UINT nFlags, APointI point)
{
	POINT_FLOAT pt;
	pt.x = point.x;
	pt.y = point.y;

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

void CAutoTerrainFlat::OnRButtonUp(UINT nFlags, APointI point)
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

void CAutoTerrainFlat::OnRButtonDblClk(UINT nFlags, APointI point) 
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

bool CAutoTerrainFlat::Render()
{
	return true;
}

// Is polygon valid
bool CAutoTerrainFlat::IsPolygonValid()
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
bool CAutoTerrainFlat::SetPoints(int nNumPoints, POINT_FLOAT* pPoints)
{
	if (!m_Area.SetPoints(nNumPoints, pPoints))
		return false;

	if (!CalArea())
		return false;

	return true;
}

// Get area points
bool CAutoTerrainFlat::GetAreaPoints(int* pNumPoints, POINT_FLOAT** pPoints)
{
	*pNumPoints = m_Area.GetNumFinalPoints();
	*pPoints = m_Area.GetFinalPoints();

	return true;
}

bool CAutoTerrainFlat::CalArea()
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

	m_fTransformBorder = a_Min((nRight-nLeft), (nBottom-nTop)) / 3.0f;
	a_ClampRoof(m_fTransformBorder, 30.0f);
/*
	(nLeft < m_fTransformBorder) ? nLeft = 0 : nLeft -= m_fTransformBorder;
	(m_nWidth - nRight - 1 < m_fTransformBorder) ? nRight = m_nWidth - 1 : nRight += m_fTransformBorder;
	(nTop < m_fTransformBorder) ? nTop = 0 : nTop -= m_fTransformBorder;
	(m_nHeight - nBottom - 1 < m_fTransformBorder) ? nBottom = m_nHeight - 1 : nBottom += m_fTransformBorder;
*/

	return true;
}

// Flat terrain
bool CAutoTerrainFlat::FlatTerrain()
{
	float fAverageHeight = GetAverageHeight();

	FlatTerrainPoly(fAverageHeight);
	return true;
}

// Get average height
float CAutoTerrainFlat::GetAverageHeight()
{
	int i, j;
	int nPos;
	float fHeight = 0.0f;
	POINT_FLOAT pt;
	float fTotalHeight = 0.0f;
	int nNumPoint = 0;
	CAutoTerrain::HEIGHTMAP* pHeightMap = m_pAutoTerrain->GetHeightMap();
	for (i=m_ptAreaLeftTop.y; i<m_ptAreaLeftTop.y+m_nAreaHeight; i++)
	{
		for (j=m_ptAreaLeftTop.x; j<m_ptAreaLeftTop.x+m_nAreaWidth; j++)
		{
			nPos = i * m_nTerrainWidth + j;
			pt.x = j;
			pt.y = i;
			if (m_Area.IsInArea(pt))
			{
				fHeight = pHeightMap->pAbsHeightBak[nPos] + pHeightMap->pAbsHeightUp[nPos] + pHeightMap->pAbsHeightDown[nPos];
				fTotalHeight += fHeight;
				nNumPoint++;
			}
		}
	}

	if (nNumPoint > 0)
		fHeight = fTotalHeight / nNumPoint;

	return fHeight;
}

// Flat Terrain poly
bool CAutoTerrainFlat::FlatTerrainPoly(float fHeight)
{
	int i, j;
	int nPos;
	float fDis, fDisWeight, fDeltaHeight;
	POINT_FLOAT pt;
	CAutoTerrain::HEIGHTMAP* pHeightMap = m_pAutoTerrain->GetHeightMap();
	int nLeft, nRight, nTop, nBottom;

	nLeft = m_ptAreaLeftTop.x - m_fTransformBorder;
	a_ClampFloor(nLeft, 0);
	nRight = m_ptAreaLeftTop.x + m_nAreaWidth + m_fTransformBorder;
	a_ClampRoof(nRight, m_nTerrainWidth - 1);
	nTop = m_ptAreaLeftTop.y - m_fTransformBorder;
	a_ClampFloor(nTop, 0);
	nBottom = m_ptAreaLeftTop.y + m_nAreaHeight + m_fTransformBorder;
	a_ClampRoof(nBottom, m_nTerrainHeight - 1);

	for (i=nTop; i<nBottom; i++)
	{
		for (j=nLeft; j<nRight; j++)
		{
			nPos = i * m_nTerrainWidth + j;
			pt.x = j;
			pt.y = i;
			if (m_Area.IsInArea(pt))
			{
				pHeightMap->pAbsHeight[nPos] = fHeight;
			}
			else
			{
				fDis = m_Area.DistanceToPoint(pt);
				if (fDis < m_fTransformBorder)
				{
					fDisWeight = fDis / m_fTransformBorder;
					fDisWeight = cos(fDisWeight*3.1415927) * 0.5f + 0.5f;
					fDeltaHeight = (fHeight - pHeightMap->pAbsHeight[nPos]) * fDisWeight;
					pHeightMap->pAbsHeight[nPos] += fDeltaHeight;
				}
			}
		}
	}
//	m_pAutoTerrain->ResetHeightData(MAX_HEIGHT);
//	m_pAutoTerrain->UpdateVertexNormals();

	m_pAutoTerrain->SetNeedRefreshHeight(true);

	return true;
}

// Close area
bool CAutoTerrainFlat::CloseArea()
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