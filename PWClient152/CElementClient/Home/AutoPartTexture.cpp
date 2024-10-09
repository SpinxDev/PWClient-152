/*
 * FILE: AutoPartTexture.cpp
 *
 * DESCRIPTION: Class for generating part texture
 *
 * CREATED BY: Jiang Dalong, 2005/05/06
 *
 * HISTORY: 
 *
 * Copyright (c) 2001-2008 Archosaur Studio, All Rights Reserved.
 */
#pragma warning (disable: 4244)
#pragma warning (disable: 4018)

#include "AutoPartTexture.h"
#include "AutoHome.h"
#include "Render.h"
#include "PerlinNoise2D.h"
#include "AutoTexture.h"
#include "AutoTerrain.h"
#include <A3DWireCollector.h>
#include <A3DEngine.h>
#include <AAssist.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAutoPartTexture::CAutoPartTexture(CAutoHome* pAutoHome)
	: CAutoPartTerrainBase(pAutoHome)
{
	m_pInArea = NULL;
}

CAutoPartTexture::~CAutoPartTexture()
{
	Release();
}

void CAutoPartTexture::Release()
{
	ReleaseArea();
}

void CAutoPartTexture::OnLButtonDown(UINT nFlags, APointI point)
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

void CAutoPartTexture::OnRButtonUp(UINT nFlags, APointI point)
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

void CAutoPartTexture::OnRButtonDblClk(UINT nFlags, APointI point) 
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

bool CAutoPartTexture::Render()
{
	return true;
}

bool CAutoPartTexture::CalArea()
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

	m_fTransformBorder = a_Min((nRight-nLeft), (nBottom-nTop)) / 2.0f;
//	a_ClampFloor(m_fTransformBorder, 20.0f);
	a_ClampRoof(m_fTransformBorder, 5.0f);
	
	(nLeft < m_fTransformBorder) ? nLeft = 0 : nLeft -= m_fTransformBorder;
	(m_nTerrainWidth - nRight - 1 < m_fTransformBorder) ? nRight = m_nTerrainWidth - 1 : nRight += m_fTransformBorder;
	(nTop < m_fTransformBorder) ? nTop = 0 : nTop -= m_fTransformBorder;
	(m_nTerrainHeight - nBottom - 1 < m_fTransformBorder) ? nBottom = m_nTerrainHeight - 1 : nBottom += m_fTransformBorder;

	m_ptAreaLeftTop.x = nLeft;
	m_ptAreaLeftTop.y = nTop;
	m_nAreaWidth = nRight - nLeft;
	m_nAreaHeight = nBottom - nTop;

	ReleaseArea();
	m_pInArea = (BYTE*)malloc(m_nAreaWidth*m_nAreaHeight*sizeof(BYTE));
	memset(m_pInArea, 0, m_nAreaWidth*m_nAreaHeight*sizeof(BYTE));
	
	return true;
}

void CAutoPartTexture::ReleaseArea()
{
	if (m_pInArea)
	{
		free(m_pInArea);
		m_pInArea = NULL;
	}
}

// Add polygon texture
bool CAutoPartTexture::AddPolygonTexture(float fTextureRatio, float fMainTexture, float fPatchTexture, float fPatchDivision)
{
	float fMaskWeight = fTextureRatio;// Ratio of main mask and patch mask
	float fMainSelfWeight = fMainTexture;
	float fPatchSelfWeight = fPatchTexture;
	float fPatchNumber = fPatchDivision;

	// Create main texture and patch noise
	CPerlinNoise2D noise;
	noise.Init(m_nAreaWidth, m_nAreaHeight, 1.0f, fPatchNumber, 1.0f, 2, -1, false);
	float fPatchHeight = (noise.GetMaxValue() + noise.GetMinValue()) * 0.5f
						+ (noise.GetMaxValue() - noise.GetMinValue()) * 0.5f * fMaskWeight;
	float fTransWeight;
	fTransWeight = (noise.GetMaxValue() - fPatchHeight) / fPatchSelfWeight;

	float fMaskTransWeight;
	fMaskTransWeight = (noise.GetMaxValue() - noise.GetMinValue());

	// Area in texture map
	int nLeft = int((float)m_ptAreaLeftTop.x * MASK_SIZE / m_nTerrainWidth + 0.5);
	int nTop = int((float)m_ptAreaLeftTop.y * MASK_SIZE / m_nTerrainHeight + 0.5);
	int nRight = int((float)(m_ptAreaLeftTop.x + m_nAreaWidth - 1) * MASK_SIZE / m_nTerrainWidth + 0.5);
	int nBottom = int((float)(m_ptAreaLeftTop.y + m_nAreaHeight - 1) * MASK_SIZE / m_nTerrainHeight + 0.5);

	CAutoTexture* pTexture = m_pAutoHome->GetAutoTexture();

	int nPosX,nPosY;// coordinate in adding area
	int i,j;
	float fAreaValue, fMainValue, fPatchValue, fOtherValueRatio;
	BYTE btColor;
	for (i=nTop; i<=nBottom; i++)
	{
		for (j=nLeft; j<=nRight; j++)
		{
			nPosX = int((float)j * m_nTerrainWidth / MASK_SIZE + 0.5) - m_ptAreaLeftTop.x;
			nPosY = int((float)i * m_nTerrainHeight / MASK_SIZE + 0.5) - m_ptAreaLeftTop.y;
			a_ClampRoof(nPosX, m_nAreaWidth-1);
			a_ClampRoof(nPosY, m_nAreaHeight-1);

			fMaskWeight = 255 * ((noise.GetValue(nPosX, m_nAreaHeight - nPosY - 1) - noise.GetMinValue()) / fMaskTransWeight + fMainSelfWeight);
			a_ClampRoof(fMaskWeight, 255.0f);
			fAreaValue = m_pInArea[nPosY*m_nAreaWidth+nPosX];
			if (fAreaValue>0)
			{
				// Blur main mask by patch color
				fMainValue = fAreaValue;
				fPatchValue = (255-fMaskWeight) * fAreaValue / 255;
				a_ClampRoof(fPatchValue, 255.0f);
				fOtherValueRatio = (255 - fMainValue - fPatchValue) / 255.0f;
				a_Clamp(fOtherValueRatio, 0.0f,1.0f);

				pTexture->GetMaskPoint(TT_PART, j, i, &btColor);
				btColor = a_Max(btColor, (BYTE)fMainValue);
				pTexture->UpdateMaskPoint(TT_PART, j, i, btColor);
				pTexture->GetMaskPoint(TT_PART_PATCH, j, i, &btColor);
				btColor = a_Max(btColor, (BYTE)fPatchValue);
				pTexture->UpdateMaskPoint(TT_PART_PATCH, j, i, btColor);

				// Add patch
				if (noise.GetValue(nPosX, nPosY)>fPatchHeight)
				{
					fPatchValue = fAreaValue * (noise.GetValue(nPosX, nPosY) - fPatchHeight) / fTransWeight;
					a_ClampRoof(fPatchValue, 255.0f);
					pTexture->GetMaskPoint(TT_PART_PATCH, j, i, &btColor);
					btColor = a_Max(btColor, (BYTE)fPatchValue);
					pTexture->UpdateMaskPoint(TT_PART_PATCH, j, i, btColor);
				}
			}
		}
	}
	pTexture->BlurMask(TT_PART);
	pTexture->BlurMask(TT_PART_PATCH);

//	pTexture->ApplyPatchOnMain(TT_PART, TT_PART_PATCH);

	return true;
}

// Transform area
bool CAutoPartTexture::TransformArea(DWORD dwSeed, float fBorderScope, bool bNoise)
{
	CPerlinNoise2D noise;
	if (bNoise)
	{
		if (!noise.Init(m_nAreaWidth, m_nAreaHeight, 1.5, 8, 0.5, 1, dwSeed, true))
			return false;
	}
	int i,j;
	int nPos;
	float fMinDis;
	bool bInArea;
	float fValue;
	POINT_FLOAT point;
	float fOutBorder = m_fTransformBorder * fBorderScope;
	float fInBorder = m_fTransformBorder * fBorderScope * 2;

	for (i=0; i<m_nAreaHeight; i++)
	{
		for (j=0; j<m_nAreaWidth; j++)
		{
			nPos = i*m_nAreaWidth+j;
			point.x = m_ptAreaLeftTop.x + j;
			point.y = m_ptAreaLeftTop.y + i;
			bInArea = m_Area.IsInArea(point);
			fMinDis = m_Area.DistanceToPoint(point);
			if (bInArea)
			{
				if (fMinDis >= fInBorder)
					 fValue = 255.0;
				else
				{
					fMinDis = fMinDis / fInBorder * 0.5f + 0.5f;
//					fMinDis = (fMinDis + fOutBorder) / (fInBorder + fOutBorder);
					fValue = 255 * fMinDis;
//					fValue *= noise.GetValue(j, i);
				}
			}
			else
			{
				if (fMinDis >= fOutBorder)
					fValue = 0;
				else
				{
					fMinDis = -fMinDis / fOutBorder * 0.5f + 0.5f;
//					fMinDis = (fOutBorder - fMinDis) / (fInBorder + fOutBorder);
					fValue = 255 * fMinDis;
//					fValue *= noise.GetValue(j, i);
				}
			}
			if (bNoise)
				fValue *= noise.GetValue(j, i)+0.8;
			a_ClampRoof(fValue, 255.0f);
			m_pInArea[i*m_nAreaWidth+j] = BYTE(fValue);
		}
	}

	return true;
}

// Create part texture
bool CAutoPartTexture::CreatePartTexture(float fTextureRatio, float fMainTexture, float fPatchTexture, float fPatchDivision, DWORD dwSeed, float fBorderScope, int type)
{
	switch (type)
	{
	case PTT_MAIN:// Only main texture
		if (!SingleTexture(fBorderScope, PTT_MAIN))
			return false;
		break;
	case PTT_PATCH:// Only patch texture
		if (!SingleTexture(fBorderScope, PTT_PATCH))
			return false;
		break;
	case PTT_BLEND:// Blend main and patch texture
		if (!TransformArea(dwSeed, fBorderScope, true))
			return false;

		if (!AddPolygonTexture(
			fTextureRatio,
			fMainTexture,
			fPatchTexture,
			fPatchDivision
			))
			return false;
		break;
	}

	return true;
}

// Is polygon valid
bool CAutoPartTexture::IsPolygonValid()
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
bool CAutoPartTexture::SetPoints(int nNumPoints, POINT_FLOAT* pPoints)
{
	if (!m_Area.SetPoints(nNumPoints, pPoints))
		return false;

	if (!CalArea())
		return false;

	return true;
}

// Attach single texture on terrain
bool CAutoPartTexture::SingleTexture(float fBorderScope, PARTTEXTURETYPE type)
{
	if (!TransformArea(0, fBorderScope, false))
		return false;

	int nMask;
	if (PTT_MAIN == type)
		nMask = TT_PART;
	else
		nMask = TT_PART_PATCH;

	int i, j;
	BYTE btColor;
	int nPosX, nPosY;
	CAutoTexture* pTexture = m_pAutoHome->GetAutoTexture();
	// Area in texture map
	int nLeft = int((float)m_ptAreaLeftTop.x * MASK_SIZE / m_nTerrainWidth + 0.5);
	int nTop = int((float)m_ptAreaLeftTop.y * MASK_SIZE / m_nTerrainHeight + 0.5);
	int nRight = int((float)(m_ptAreaLeftTop.x + m_nAreaWidth - 1) * MASK_SIZE / m_nTerrainWidth + 0.5);
	int nBottom = int((float)(m_ptAreaLeftTop.y + m_nAreaHeight - 1) * MASK_SIZE / m_nTerrainHeight + 0.5);
	for (i=nTop; i<=nBottom; i++)
	{
		for (j=nLeft; j<=nRight; j++)
		{
			nPosX = int((float)j * m_nTerrainWidth / MASK_SIZE + 0.5) - m_ptAreaLeftTop.x;
			nPosY = int((float)i * m_nTerrainHeight / MASK_SIZE + 0.5) - m_ptAreaLeftTop.y;
			a_ClampRoof(nPosX, m_nAreaWidth-1);
			a_ClampRoof(nPosY, m_nAreaHeight-1);

			pTexture->GetMaskPoint(nMask, j, i, &btColor);
			btColor = a_Max(btColor, m_pInArea[nPosY*m_nAreaWidth+nPosX]);
			pTexture->UpdateMaskPoint(nMask, j, i, btColor);
		}
	}
	pTexture->BlurMask(nMask);

	return true;
}

// Close area
bool CAutoPartTexture::CloseArea()
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
bool CAutoPartTexture::StorePoints()
{
	return m_Area.StorePoints();
}

// Restore points
bool CAutoPartTexture::RestorePoints()
{
	if (!m_Area.RestorePoints())
		return false;
	if (!CalArea())
		return false;

	return true;
}
