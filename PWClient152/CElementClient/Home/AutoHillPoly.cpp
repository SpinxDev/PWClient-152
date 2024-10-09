/*
 * FILE: AutoHillPoly.cpp
 *
 * DESCRIPTION: Class for generating polygon hill
 *
 * CREATED BY: Jiang Dalong, 2005/05/05
 *
 * HISTORY: 
 *
 * Copyright (c) 2001-2008 Archosaur Studio, All Rights Reserved.
 */
#pragma warning (disable: 4244)
#pragma warning (disable: 4018)

#include "AutoHillPoly.h"
#include "AutoHome.h"
#include "CurvePoint.h"
#include "Render.h"
#include "PerlinNoise2D.h"
#include "AutoTerrain.h"
#include "AutoTexture.h"
#include <A3DWireCollector.h>
#include <A3DEngine.h>
#include <AAssist.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAutoHillPoly::CAutoHillPoly(CAutoHome* pAutoHome)
	: CAutoPartTerrainBase(pAutoHome)
{
	m_pInArea = NULL;
	m_pDistance = NULL;
	m_pInPolygon = NULL;
	m_pMinDisType = NULL;
	m_pNeedDraw = NULL;
}

CAutoHillPoly::~CAutoHillPoly()
{
	Release();
}

void CAutoHillPoly::Release()
{
	ReleaseArea();
}
/*
void CAutoHillPoly::OnLButtonDown(UINT nFlags, APointI point)
{
	POINT_FLOAT pt;
	pt.x = point.x;
	pt.y = point.y;

	if (!ScreenToHeightmap(pt))
		return;

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

void CAutoHillPoly::OnRButtonUp(UINT nFlags, APointI point)
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

void CAutoHillPoly::OnRButtonDblClk(UINT nFlags, APointI point) 
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
*/
bool CAutoHillPoly::Render()
{
	return true;
}

bool CAutoHillPoly::CalArea()
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

	m_fTransformBorder = a_Min((nRight-nLeft), (nBottom-nTop)) / 3.0f;
	a_ClampRoof(m_fTransformBorder, 10.0f);

	nLeft -= m_fTransformBorder;
	nRight += m_fTransformBorder;
	nTop -= m_fTransformBorder;
	nBottom += m_fTransformBorder;
	a_ClampFloor(nLeft, 1);
	a_ClampRoof(nRight, m_nTerrainWidth - 2);
	a_ClampFloor(nTop, 1);
	a_ClampRoof(nBottom, m_nTerrainHeight - 2);
/*
	(nLeft < m_fTransformBorder) ? nLeft = 0 : nLeft -= m_fTransformBorder;
	(m_nTerrainWidth - nRight - 1 < m_fTransformBorder) ? nRight = m_nTerrainWidth - 1 : nRight += m_fTransformBorder;
	(nTop < m_fTransformBorder) ? nTop = 0 : nTop -= m_fTransformBorder;
	(m_nTerrainHeight - nBottom - 1 < m_fTransformBorder) ? nBottom = m_nTerrainHeight - 1 : nBottom += m_fTransformBorder;
*/
	m_ptAreaLeftTop.x = nLeft;
	m_ptAreaLeftTop.y = nTop;
	m_nAreaWidth = nRight - nLeft + 1;
	m_nAreaHeight = nBottom - nTop + 1;

	ReleaseArea();
	m_Area.ReleaseSalient();
	
	return true;
}

// Transform area
bool CAutoHillPoly::TransformArea(float fMainSmooth, float fPartSmooth, DWORD dwSeed, int nNumCurve, float* pCurvePoints, float fBorderErodibility)
{
	ReleaseArea();
	m_pInArea = new float[m_nAreaWidth*m_nAreaHeight];
	memset(m_pInArea, 0, m_nAreaWidth*m_nAreaHeight*sizeof(float));
	m_pDistance = new float[m_nAreaWidth*m_nAreaHeight];
	memset(m_pDistance, 0, m_nAreaWidth*m_nAreaHeight*sizeof(float));
	m_pInPolygon = new bool[m_nAreaWidth*m_nAreaHeight];
	memset(m_pInPolygon, 0, m_nAreaWidth*m_nAreaHeight*sizeof(bool));
	m_pMinDisType = new int[m_nAreaWidth*m_nAreaHeight];
	memset(m_pMinDisType, 0, m_nAreaWidth*m_nAreaHeight*sizeof(int));
	m_pNeedDraw = new float[m_nAreaWidth*m_nAreaHeight];
	memset(m_pNeedDraw, 0, m_nAreaWidth*m_nAreaHeight*sizeof(float));

	// Set curve points
	CCurvePoint cp;
	cp.SetPointValues(nNumCurve, pCurvePoints);

	m_fMaxDis = 1.0f;
	int nPos;
	POINT_FLOAT point;

	// 找出凸角
	m_Area.FindSalient(-0.3f);

	// 计算区域内每点离边的距离
	int i,j;
	for (i=0; i<m_nAreaHeight; i++)
	{
		for (j=0; j<m_nAreaWidth; j++)
		{
			nPos = i*m_nAreaWidth+j;
			point.x = m_ptAreaLeftTop.x + j;
			point.y = m_ptAreaLeftTop.y + i;
			m_pInPolygon[nPos] = m_Area.IsInArea(point);
			m_pDistance[nPos] = m_Area.DistanceToEdge(point, m_pInPolygon[nPos], &m_pMinDisType[nPos]);

			if (m_pInPolygon[nPos])
			{
				if (m_pDistance[nPos] > m_fMaxDis)
					m_fMaxDis = m_pDistance[nPos];
			}
		}
	}


	// 平滑距离
	BlurArea(m_pDistance, m_nAreaWidth, m_nAreaHeight);
	BlurArea(m_pDistance, m_nAreaWidth, m_nAreaHeight);

	// 计算区域高度系数
	CPerlinNoise2D noise;
	if (!noise.Init(m_nAreaWidth, m_nAreaHeight, 1.0, (int)fMainSmooth, fPartSmooth, 2, -1, true))
		return false;

	CPerlinNoise2D noiseBorder;
	if (!noiseBorder.Init(m_nAreaWidth, m_nAreaHeight, fBorderErodibility, 10, 1.0f, 2, -1, true))
		return false;

	float fMinDis;
	float fValue;
	float fDisWeight;

	// Get max curve point value
	float fMaxCurveValue = 0.0f;
	for (i=0; i<nNumCurve; i++)
	{
		if (fMaxCurveValue < (1-pCurvePoints[i]))
			fMaxCurveValue = 1 - pCurvePoints[i];
	}
	fMaxCurveValue *= 0.8f;//1.2f;

	float fDisRate, fBorderDown;
	for (i=0; i<m_nAreaHeight; i++)
	{
		for (j=0; j<m_nAreaWidth; j++)
		{
			nPos = i*m_nAreaWidth+j;
			fMinDis = m_pDistance[nPos];
			if (m_pInPolygon[nPos])
			{
				fDisWeight = (fMinDis + m_fTransformBorder) / (m_fMaxDis + m_fTransformBorder);
				fDisWeight = cp.GetValue(fDisWeight);
				fBorderDown = fDisWeight * noiseBorder.GetValue(j, i) * fMaxCurveValue;
				a_ClampFloor(fBorderDown, 0.0f);
				fDisWeight = 1 - fDisWeight;
				fDisWeight -= fBorderDown;
				fValue = fDisWeight * noise.GetValue(j, i);
			}
			else
			{
				if (fMinDis >= m_fTransformBorder)
					fValue = 0;
				else
				{
					fDisWeight = (m_fTransformBorder - fMinDis) / (m_fMaxDis + m_fTransformBorder);
					fDisRate = fDisWeight;
					fDisWeight = cp.GetValue(fDisWeight);;
					fBorderDown = fDisWeight * noiseBorder.GetValue(j, i) * fMaxCurveValue;
					a_ClampFloor(fBorderDown, 0.0f);
					fDisWeight = 1 - fDisWeight;
					fDisWeight = 0.5 - cos(fDisWeight * 3.14159) * 0.5;
					fDisWeight -= fBorderDown;
					fValue = fDisWeight * noise.GetValue(j, i);
//					fValue *= 0.5 - cos(fDisRate * 3.1415927) * 0.5;
				}
			}
			m_pInArea[nPos] = fValue;
		}
	}

	// 平滑高度
	BlurArea(m_pInArea, m_nAreaWidth, m_nAreaHeight);
	BlurArea(m_pInArea, m_nAreaWidth, m_nAreaHeight);
	BlurArea(m_pInArea, m_nAreaWidth, m_nAreaHeight);


	return true;
}

bool CAutoHillPoly::BlurArea(float* pData, int nWidth, int nHeight)
{
	float* pTemp = new float[nWidth*nHeight];
	if (NULL == pTemp)
	{
		return false;
	}
	memcpy(pTemp, pData, nWidth*nHeight*sizeof(float));

	int i, j;
	int nPos;
	for (i=2; i<nHeight-2; i++)
	{
		for (j=2; j<nWidth-2; j++)
		{
			nPos = i*nWidth+j;
			pTemp[nPos] = (pData[nPos-2*nWidth-2] + 1*pData[nPos-2*nWidth-1] + 1*pData[nPos-2*nWidth] + 1*pData[nPos-2*nWidth+1] + pData[nPos-2*nWidth+2]
							+ 1*pData[nPos-nWidth-2] + 1*pData[nPos-nWidth-1] + 1*pData[nPos-nWidth] + 1*pData[nPos-nWidth+1] + 1*pData[nPos-nWidth+2]
							+ 1*pData[nPos-2] + 1*pData[nPos-1] + 1*pData[nPos] + 1*pData[nPos+1] + 1*pData[nPos+2]
							+ 1*pData[nPos+nWidth-2] + 1*pData[nPos+nWidth-1] + 1*pData[nPos+nWidth] + 1*pData[nPos+nWidth+1] + 1*pData[nPos+nWidth+2]
							+ pData[nPos+2*nWidth-2] + 1*pData[nPos+2*nWidth-1] + 1*pData[nPos+2*nWidth] + 1*pData[nPos+2*nWidth+1] + pData[nPos+2*nWidth+2]
							) / 25;
		}
	}
	memcpy(pData, pTemp, nWidth*nHeight*sizeof(float));
	delete[] pTemp;

	return true;
}

// Apply height to terrain
bool CAutoHillPoly::ApplyHeightToTerrain(float fMaxHeight, bool bHillUp)
{
	float fHeight;
	int i, j;
	int x, y;
	int nPosTerrain;
	for (i=0; i<m_nAreaHeight; i++)
	{
		for (j=0; j<m_nAreaWidth; j++)
		{
			x = j + m_ptAreaLeftTop.x;
			y = i + m_ptAreaLeftTop.y;
			nPosTerrain = y * m_nTerrainWidth + x;
			fHeight = m_pInArea[i*m_nAreaWidth+j] * fMaxHeight;
			if (bHillUp)
			{
				if (fHeight > m_pAutoTerrain->GetHeightMap()->pAbsHeightUp[nPosTerrain])
					m_pAutoTerrain->GetHeightMap()->pAbsHeightUp[nPosTerrain] = fHeight;
			}
			else
			{
				if (-fHeight < m_pAutoTerrain->GetHeightMap()->pAbsHeightDown[nPosTerrain])
					m_pAutoTerrain->GetHeightMap()->pAbsHeightDown[nPosTerrain] = -fHeight;
			}
			m_pAutoTerrain->UpdateAbsHeightAtPos(nPosTerrain);
		}
	}

	if (CAutoTerrain::UDP_BOTH != m_pAutoTerrain->GetUpDownPri())
		m_pAutoTerrain->BlurArea(m_pAutoTerrain->GetHeightMap()->pAbsHeight, m_pAutoTerrain->GetHeightMap()->iWidth, m_pAutoTerrain->GetHeightMap()->iHeight);

//	m_pAutoTerrain->ResetHeightData(MAX_HEIGHT);
//	m_pAutoTerrain->UpdateVertexNormals();

	m_pAutoTerrain->SetNeedRefreshHeight(true);

	return true;
}

bool CAutoHillPoly::AddTopTexture(float fTextureRatio, float fMainTexture, float fPatchTexture, 
								  float fPatchDivision, float fTopScope, float fBridgeDepth)
{
	// Add top texture
	ARectI rect;
	rect.left = m_ptAreaLeftTop.x;
	rect.right = m_ptAreaLeftTop.x + m_nAreaWidth - 1;
	rect.top = m_ptAreaLeftTop.y;
	rect.bottom = m_ptAreaLeftTop.y + m_nAreaHeight - 1;
	CAutoTexture* pAutoTexture = m_pAutoHome->GetAutoTexture();
	pAutoTexture->RenderTopTexture(
		rect, 
		TT_BRIDGE, 
		TT_BRIDGE_PATCH, 
		fTextureRatio, 
		fMainTexture, 
		fPatchTexture, 
		fPatchDivision,
		fTopScope);	

//	pAutoTexture->ApplyPatchOnMain(TT_BRIDGE, TT_BRIDGE_PATCH);

	// 扩展山脊区域
	GetMountainBridge();

	//Add mountain bridge texture
	float fMaskWeight = fTextureRatio;// Ratio of main mask and patch mask
	float fMainSelfWeight = fMainTexture;
	float fPatchSelfWeight = fPatchTexture;
	float fPatchNumber = fPatchDivision;

	// Create main texture and patch noise
	CPerlinNoise2D noise;
	noise.Init(m_nAreaWidth, m_nAreaHeight, 1.0f, fPatchNumber, 0.5f, 2, -1, true);
	float fPatchHeight = (noise.GetMaxValue() + noise.GetMinValue()) * 0.5f
						+ (noise.GetMaxValue() - noise.GetMinValue()) * 0.5f * fMaskWeight;
	float fTransWeight;
	fTransWeight = (noise.GetMaxValue() - fPatchHeight) / fPatchSelfWeight;

	float fMaskTransWeight;
	fMaskTransWeight = (noise.GetMaxValue() - noise.GetMinValue());
	float fNoiseAvg = (noise.GetMaxValue() - noise.GetMinValue()) / 2;

	// Area in texture map
	int nLeft = int((float)m_ptAreaLeftTop.x * MASK_SIZE / m_nTerrainWidth + 0.5);
	int nTop = int((float)m_ptAreaLeftTop.y * MASK_SIZE / m_nTerrainHeight + 0.5);
	int nRight = int((float)(m_ptAreaLeftTop.x + m_nAreaWidth - 1) * MASK_SIZE / m_nTerrainWidth + 0.5);
	int nBottom = int((float)(m_ptAreaLeftTop.y + m_nAreaHeight - 1) * MASK_SIZE / m_nTerrainHeight + 0.5);


	int nPosX,nPosY;// coordinate in adding area
	int i,j;
	float fAreaValue;
	BYTE btColor;
	int nPos;
	for (i=nTop; i<=nBottom; i++)
	{
		for (j=nLeft; j<=nRight; j++)
		{
			nPosX = int((float)j * m_nTerrainWidth / MASK_SIZE + 0.5) - m_ptAreaLeftTop.x;
			nPosY = int((float)i * m_nTerrainHeight / MASK_SIZE + 0.5) - m_ptAreaLeftTop.y;
			a_ClampRoof(nPosX, m_nAreaWidth-1);
			a_ClampRoof(nPosY, m_nAreaHeight-1);
			nPos = nPosY*m_nAreaWidth+nPosX;

			fMaskWeight = 255 * ((noise.GetValue(nPosX, m_nAreaHeight - nPosY - 1) - noise.GetMinValue()) / fMaskTransWeight + fMainSelfWeight);
			a_ClampRoof(fMaskWeight, 255.0f);

			if (m_pNeedDraw[nPos] <= 0.4)
				continue;
			fAreaValue = (m_pNeedDraw[nPos] - 0.4) / 0.6 * 255;
			fAreaValue *= 0.5f / fNoiseAvg;
			if (m_pDistance[nPos] < m_fMaxDis / 6)
				fAreaValue *= 1 - (m_fMaxDis / 6 - m_pDistance[nPos]) / (m_fMaxDis / 6);
			fAreaValue *= noise.GetValue(nPosX, nPosY) * fBridgeDepth;
			a_ClampRoof(fAreaValue, 255.0f);


			pAutoTexture->GetMaskPoint(TT_BRIDGE, j, i, &btColor);
			btColor = a_Max(btColor, (BYTE)fAreaValue);
			pAutoTexture->UpdateMaskPoint(TT_BRIDGE, j, i, btColor);
		}
	}
	
	return true;
}

bool CAutoHillPoly::GetMountainBridge()
{
	CPolygonMountain::PARTMAXDISINFO* pSalientPointsMidLine = m_Area.GetSalientPointsMidLine();
	int nSalientPoints = m_Area.GetNumSalientPoints();

	int i,j;
	float fSlope;
	int nCount, nIndex;
	int nDelta;
	POINT_FLOAT ptTest, ptTestSide, ptTemp;
	float fTotalCos;
	A3DVECTOR3 vTest, vTestSide;
	int nPos;
	float fCosValue;
	for (i=0; i<nSalientPoints; i++)
	{
		fTotalCos = 0;
		nCount = pSalientPointsMidLine[i].nCount;
		if (nCount < 10)
			continue;
		nDelta = nCount / 10;
		fSlope = ::LineSlope(pSalientPointsMidLine[i].point[0], pSalientPointsMidLine[i].point[nCount - 1]);
		(fabs(fSlope)<0.0001) ? fSlope = 9999.0f : fSlope = -1 / fSlope;

		for (j=1; j<10; j++)
		{
			nIndex = nDelta * j;
			ptTest = pSalientPointsMidLine[i].point[nIndex];

			ptTemp.x = ptTest.x + 2;
			ptTemp.y = fSlope * (ptTemp.x - ptTest.x) + ptTest.y;
			ptTestSide = ::PointAtLine(ptTest, ptTemp, 3);
			a_Clamp(ptTestSide.x, 0.0f, float(m_nTerrainWidth-1));
			a_Clamp(ptTestSide.y, 0.0f, float(m_nTerrainHeight-1));
			vTest.x = ptTestSide.x - ptTest.x;
			vTest.y = 0;
			vTest.z = -(ptTestSide.y - ptTest.y);
			nPos = int(ptTestSide.y) * m_nTerrainWidth + int(ptTestSide.x);
			vTestSide = m_pAutoTerrain->GetHeightMap()->aNormals[nPos];
			vTest.Normalize();
			vTestSide.Normalize();
			fCosValue = DotProduct(vTest, vTestSide);
			fTotalCos += fCosValue;

			ptTemp.x = ptTest.x - 2;
			ptTemp.y = fSlope * (ptTemp.x - ptTest.x) + ptTest.y;
			ptTestSide = ::PointAtLine(ptTest, ptTemp, 3);
			a_Clamp(ptTestSide.x, 0.0f, float(m_nTerrainWidth-1));
			a_Clamp(ptTestSide.y, 0.0f, float(m_nTerrainHeight-1));
			vTest.x = ptTestSide.x - ptTest.x;
			vTest.y = 0;
			vTest.z = -(ptTestSide.y - ptTest.y);
			nPos = int(ptTestSide.y) * m_nTerrainWidth + int(ptTestSide.x);
			vTestSide = m_pAutoTerrain->GetHeightMap()->aNormals[nPos];
			vTest.Normalize();
			vTestSide.Normalize();
			fCosValue = DotProduct(vTest, vTestSide);
			fTotalCos += fCosValue;
		}

		if (fTotalCos > 3.0f)
		{
			for (j=0; j<nCount; j++)
			{
				ExpandPoints(
					pSalientPointsMidLine[i].point[j].x - m_ptAreaLeftTop.x,
					pSalientPointsMidLine[i].point[j].y - m_ptAreaLeftTop.y,
					10, 
					m_pNeedDraw);
			}
		}
	}

	return true;
}

void CAutoHillPoly::ExpandPoints(int x, int y, float fExpandDis, float* pNeedDraw)
{
	int i, j;
	int nMaxDis;
	int nPos;
	POINT_FLOAT point, ptRoot;
	float fDis, fValue;
	ptRoot.x = x;
	ptRoot.y = y;

	nMaxDis = (int)fExpandDis;
	for (i=y-nMaxDis; i<=y+nMaxDis; i++)
	{
		for (j=x-nMaxDis; j<=x+nMaxDis; j++)
		{
			if (j < 0 || j >= m_nAreaWidth)
				continue;
			if (i < 0 || i >= m_nAreaHeight)
				continue;

			nPos = i*m_nAreaWidth+j;
			point.x = j;
			point.y = i;
			fDis = ::Distance(point, ptRoot);
			if (fDis < fExpandDis)
			{
				fValue = (1 - fDis / fExpandDis);
				a_Clamp(fValue, 0.0f, 1.0f);
				if (fValue > pNeedDraw[nPos])
				{
					pNeedDraw[nPos] = fValue;
				}
			}
		}// for j
	}// for i
}

void CAutoHillPoly::ReleaseArea()
{
	if (m_pInArea)
	{
		delete[] m_pInArea;
		m_pInArea = NULL;
	}
	if (m_pDistance)
	{
		delete[] m_pDistance;
		m_pDistance = NULL;
	}
	if (m_pInPolygon)
	{
		delete[] m_pInPolygon;
		m_pInPolygon = NULL;
	}
	if (m_pMinDisType)
	{
		delete[] m_pMinDisType;
		m_pMinDisType = NULL;
	}
	if (m_pNeedDraw)
	{
		delete[] m_pNeedDraw;
		m_pNeedDraw = NULL;
	}
}

// Create polygon hill
bool CAutoHillPoly::CreateHillPoly(float fMaxHeight, float fMainSmooth, float fPartSmooth, DWORD dwSeed, int nNumCurve, float* pCurvePoints, bool bHillUp, float fBorderErodibility)
{
	srand(dwSeed);
	
	if (!TransformArea(fMainSmooth, fPartSmooth, -1, nNumCurve, pCurvePoints, fBorderErodibility))
		return false;
	if (!ApplyHeightToTerrain(fMaxHeight, bHillUp))
		return false;

	return true;
}

// Render hill top
bool CAutoHillPoly::RenderHillTop(float fTextureRatio, float fMainTexture, float fPatchTexture, 
								  float fPatchDivision, float fTopScope, float fBridgeDepth, bool bBlurMask)
{
	CAutoTexture* pAutoTexture = m_pAutoHome->GetAutoTexture();

//	pAutoTexture->RestoreBridgeMask();

	if (!AddTopTexture(fTextureRatio, fMainTexture, fPatchTexture, fPatchDivision, fTopScope, fBridgeDepth))
		return false;
	if (bBlurMask)
	{
		pAutoTexture->BlurMask(TT_BRIDGE);
		pAutoTexture->BlurMask(TT_BRIDGE_PATCH);
	}

//	pAutoTexture->StoreBridgeMask();
	
	return true;
}

// Is polygon valid
bool CAutoHillPoly::IsPolygonValid()
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
bool CAutoHillPoly::SetPoints(int nNumPoints, POINT_FLOAT* pPoints)
{
	if (!m_Area.SetPoints(nNumPoints, pPoints))
		return false;

	if (!CalArea())
		return false;

	return true;
}

// Get area points
bool CAutoHillPoly::GetAreaPoints(int* pNumPoints, POINT_FLOAT** pPoints)
{
	*pNumPoints = m_Area.GetNumFinalPoints();
	*pPoints = m_Area.GetFinalPoints();

	return true;
}

// Close area
bool CAutoHillPoly::CloseArea()
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
bool CAutoHillPoly::StorePoints()
{
	return m_Area.StorePoints();
}

// Restore points
bool CAutoHillPoly::RestorePoints()
{
	if (!m_Area.RestorePoints())
		return false;
	if (!CalArea())
		return false;

	return true;
}
