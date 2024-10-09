/*
 * FILE: AutoHomeBorder.cpp
 *
 * DESCRIPTION: Class for lines of home border
 *
 * CREATED BY: Jiang Dalong, 2006/07/04
 *
 * HISTORY: 
 *
 * Copyright (c) 2001-2008 Archosaur Studio, All Rights Reserved.
 */

#include "AutoHomeBorder.h"
#include "AutoHome.h"
#include "AutoHomeBorderData.h"
#include "AutoSceneFunc.h"
#include "AutoScene.h"
#include "AutoTerrain.h"
#include "Render.h"
#include <A3DWireCollector.h>
#include <A3DEngine.h>
#include <A3DCollision.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const float DRAW_UP_DIS	 = 1.0f;

CAutoHomeBorder::CAutoHomeBorder(CAutoHome* pAutoHome)
{
	m_pAutoHome			= pAutoHome;
	m_pBorderPoints		= NULL;
	m_nNumPoints		= NUM_HOME_BORDER_POINT * 4 - 4;
	m_nNumInside		= NUM_HOME_BORDER_POINT - 2;
	m_bSelectedPoint	= false;
	m_nSelectedPoint	= -1;
}

CAutoHomeBorder::~CAutoHomeBorder()
{

}

void CAutoHomeBorder::Release()
{
	SAFERELEASE(m_pBorderPoints);
}

// Initialize border points
bool CAutoHomeBorder::Init()
{
	SAFERELEASE(m_pBorderPoints);
	m_pBorderPoints = new BORDERPOINT[m_nNumPoints];

	CAutoHomeBorderData* pHomeBorderLine = m_pAutoHome->GetAutoScene()->GetAutoHomeBorderData();
	int i;
	int nRowIndex = m_pAutoHome->GetRowIndex();
	int nColIndex = m_pAutoHome->GetColIndex();
	int nStartIndex;
	int nHeightMapWidth = m_pAutoHome->GetAutoTerrain()->GetHeightMap()->iWidth;
	int nHeightMapHeight = m_pAutoHome->GetAutoTerrain()->GetHeightMap()->iHeight;
	A3DVECTOR3 vPosWorld;
	int nRowDelta = (nHeightMapWidth - 1) / (NUM_HOME_BORDER_POINT - 1);
	int nColDelta = (nHeightMapHeight - 1) / (NUM_HOME_BORDER_POINT - 1);
	int nIndex;
	

	// Left top
	nIndex = 0;
	m_pBorderPoints[nIndex].nType = BPT_CROSS;
	m_pBorderPoints[nIndex].nIndex = nRowIndex * (NUM_BORDER_BLOCK + 1) + nColIndex;
	m_pAutoHome->HeightMapToWorld(0, 0, vPosWorld);
	m_pBorderPoints[nIndex].vPosWorld = vPosWorld;
	m_pBorderPoints[nIndex].vPosWorld.y = pHomeBorderLine->GetPointCrossByIndex(m_pBorderPoints[nIndex].nIndex);

	// top
	nStartIndex = (nRowIndex * NUM_BORDER_BLOCK + nColIndex) * m_nNumInside;
	for (i=1; i<=m_nNumInside; i++)
	{
		m_pBorderPoints[i].nType = BPT_ROW;
		m_pBorderPoints[i].nIndex = nStartIndex + i - 1;
		m_pAutoHome->HeightMapToWorld(nRowDelta*i, 0, vPosWorld);
		m_pBorderPoints[i].vPosWorld = vPosWorld;
		m_pBorderPoints[i].vPosWorld.y = pHomeBorderLine->GetPointRowByIndex(m_pBorderPoints[i].nIndex);
	}

	// right top
	nIndex = m_nNumInside+1;
	m_pBorderPoints[nIndex].nType = BPT_CROSS;
	m_pBorderPoints[nIndex].nIndex = nRowIndex * (NUM_BORDER_BLOCK + 1) + nColIndex + 1;
	m_pAutoHome->HeightMapToWorld(nHeightMapWidth-1, 0, vPosWorld);
	m_pBorderPoints[nIndex].vPosWorld = vPosWorld;
	m_pBorderPoints[nIndex].vPosWorld.y = pHomeBorderLine->GetPointCrossByIndex(m_pBorderPoints[nIndex].nIndex);
	
	// right
	nStartIndex = ((nColIndex + 1) * NUM_BORDER_BLOCK + nRowIndex) * m_nNumInside;
	for (i=NUM_HOME_BORDER_POINT; i<=(m_nNumInside+1)*2-1; i++)
	{
		m_pBorderPoints[i].nType = BPT_COL;
		m_pBorderPoints[i].nIndex = nStartIndex + i - NUM_HOME_BORDER_POINT;
		m_pAutoHome->HeightMapToWorld(nHeightMapWidth-1, nColDelta*(i-NUM_HOME_BORDER_POINT+1), vPosWorld);
		m_pBorderPoints[i].vPosWorld = vPosWorld;
		m_pBorderPoints[i].vPosWorld.y = pHomeBorderLine->GetPointColByIndex(m_pBorderPoints[i].nIndex);
	}
	
	// right bottom
	nIndex = (m_nNumInside+1)*2;
	m_pBorderPoints[nIndex].nType = BPT_CROSS;
	m_pBorderPoints[nIndex].nIndex = (nRowIndex + 1) * (NUM_BORDER_BLOCK + 1) + nColIndex + 1;
	m_pAutoHome->HeightMapToWorld(nHeightMapWidth-1, nHeightMapHeight-1, vPosWorld);
	m_pBorderPoints[nIndex].vPosWorld = vPosWorld;
	m_pBorderPoints[nIndex].vPosWorld.y = pHomeBorderLine->GetPointCrossByIndex(m_pBorderPoints[nIndex].nIndex);

	// bottom
	nStartIndex = ((nRowIndex + 1) * NUM_BORDER_BLOCK + nColIndex) * m_nNumInside;
	for (i=(m_nNumInside+1)*2+1; i<=(m_nNumInside+1)*3-1; i++)
	{
		m_pBorderPoints[i].nType = BPT_ROW;
		m_pBorderPoints[i].nIndex = nStartIndex - i + (m_nNumInside+1)*3-1;
		m_pAutoHome->HeightMapToWorld(nRowDelta*((m_nNumInside+1)*3-i), nHeightMapHeight-1, vPosWorld);
		m_pBorderPoints[i].vPosWorld = vPosWorld;
		m_pBorderPoints[i].vPosWorld.y = pHomeBorderLine->GetPointRowByIndex(m_pBorderPoints[i].nIndex);
	}

	// left bottom
	nIndex = (m_nNumInside+1)*3;
	m_pBorderPoints[nIndex].nType = BPT_CROSS;
	m_pBorderPoints[nIndex].nIndex = (nRowIndex + 1) * (NUM_BORDER_BLOCK + 1) + nColIndex;
	m_pAutoHome->HeightMapToWorld(0, nHeightMapHeight-1, vPosWorld);
	m_pBorderPoints[nIndex].vPosWorld = vPosWorld;
	m_pBorderPoints[nIndex].vPosWorld.y = pHomeBorderLine->GetPointCrossByIndex(m_pBorderPoints[nIndex].nIndex);

	// left
	nStartIndex = (nColIndex * NUM_BORDER_BLOCK + nRowIndex) * m_nNumInside;
	for (i=(m_nNumInside+1)*3+1; i<=(m_nNumInside+1)*4-1; i++)
	{
		m_pBorderPoints[i].nType = BPT_COL;
		m_pBorderPoints[i].nIndex = nStartIndex - i + (m_nNumInside+1)*4-1;
		m_pAutoHome->HeightMapToWorld(0, nColDelta*((m_nNumInside+1)*4-i), vPosWorld);
		m_pBorderPoints[i].vPosWorld = vPosWorld;
		m_pBorderPoints[i].vPosWorld.y = pHomeBorderLine->GetPointColByIndex(m_pBorderPoints[i].nIndex);
	}

	return true;
}

bool CAutoHomeBorder::Render()
{
	if (NULL == m_pBorderPoints)
		return false;

	A3DWireCollector* pWireCollector = g_Render.GetA3DEngine()->GetA3DWireCollector();
	float fPointRadius = 2.0f;
	A3DVECTOR3 vUpDis(0, DRAW_UP_DIS, 0);
	DWORD dwPointColor = 0xff0000ff;
	DWORD dwSelectedColor = 0xffff0000;
	int i;
	A3DVECTOR3 v0, v1;

	v0 = m_pBorderPoints[0].vPosWorld + vUpDis;
	for (i=0; i<m_nNumPoints-1; i++)
	{
		v1 = m_pBorderPoints[i+1].vPosWorld + vUpDis;
		if (m_bSelectedPoint && m_nSelectedPoint == i)
			pWireCollector->AddSphere(v0, fPointRadius, dwSelectedColor);
		else
			pWireCollector->AddSphere(v0, fPointRadius, dwPointColor);
		pWireCollector->Add3DLine(v0, v1, dwPointColor);
		v0 = v1;
	}
	if (m_bSelectedPoint && m_nSelectedPoint == i)
		pWireCollector->AddSphere(v0, fPointRadius, dwSelectedColor);
	else
		pWireCollector->AddSphere(v0, fPointRadius, dwPointColor);
	v1 = m_pBorderPoints[0].vPosWorld + vUpDis;
	pWireCollector->Add3DLine(v0, v1, dwPointColor);

	return true;
}

bool CAutoHomeBorder::GetHomeTopPoints(float* pPoints)
{
	int i;
	for (i=0; i<NUM_HOME_BORDER_POINT; i++)
	{
		pPoints[i] = m_pBorderPoints[i].vPosWorld.y;
	}

	return true;
}

bool CAutoHomeBorder::GetHomeRightPoints(float* pPoints)
{
	int i;
	for (i=0; i<NUM_HOME_BORDER_POINT; i++)
	{
		pPoints[i] = m_pBorderPoints[i+NUM_HOME_BORDER_POINT-1].vPosWorld.y;
	}

	return true;
}

bool CAutoHomeBorder::GetHomeBottomPoints(float* pPoints)
{
	int i;
	for (i=0; i<NUM_HOME_BORDER_POINT; i++)
	{
		pPoints[i] = m_pBorderPoints[(NUM_HOME_BORDER_POINT-1)*3-i].vPosWorld.y;
	}

	return true;
}

bool CAutoHomeBorder::GetHomeLeftPoints(float* pPoints)
{
	int i;
	pPoints[0] = m_pBorderPoints[0].vPosWorld.y;
	for (i=1; i<NUM_HOME_BORDER_POINT; i++)
	{
		pPoints[i] = m_pBorderPoints[(NUM_HOME_BORDER_POINT-1)*4-i].vPosWorld.y;
	}

	return true;
}

// Check ray trace with a world point
bool CAutoHomeBorder::RayTracePoint(const A3DVECTOR3& vStart, const A3DVECTOR3& vEnd, const A3DVECTOR3& vWorld)
{
	A3DVECTOR3 vMins, vMaxs;
	A3DVECTOR3 vExt(2.0f, 2.0f, 2.0f);
	vMins = vWorld - vExt;
	vMaxs = vWorld + vExt;

	A3DVECTOR3 vPoint, vNormal;
	float fFraction;
	if (CLS_RayToAABB3(vStart, vEnd-vStart, vMins, vMaxs, vPoint, &fFraction, vNormal))
		return true;

	return false;
}

bool CAutoHomeBorder::SelectPoint(const A3DVECTOR3& vStart, const A3DVECTOR3& vEnd)
{
	int i;
	A3DVECTOR3 vUpDis(0, DRAW_UP_DIS, 0);
	for (i=0; i<m_nNumPoints; i++)
	{
		if (RayTracePoint(vStart, vEnd, m_pBorderPoints[i].vPosWorld+vUpDis))
		{
			m_bSelectedPoint = true;
			m_nSelectedPoint = i;
			return true;
		}
	}
	
	return false;
}

void CAutoHomeBorder::ReleasePoint()
{ 
	m_bSelectedPoint = false;
	m_nSelectedPoint = -1;
}

bool CAutoHomeBorder::GetSelectedPos(A3DVECTOR3& vPos)
{
	if (!m_bSelectedPoint || -1 == m_nSelectedPoint)
		return false;

	A3DVECTOR3 vUpDis(0, DRAW_UP_DIS, 0);
	vPos = m_pBorderPoints[m_nSelectedPoint].vPosWorld + vUpDis;

	return true;
}

bool CAutoHomeBorder::MoveSelectedPoint(float fDis)
{
	if (!m_bSelectedPoint || -1 == m_nSelectedPoint)
		return false;

	m_pBorderPoints[m_nSelectedPoint].vPosWorld.y += fDis;

	MovePointData(m_nSelectedPoint, fDis);

	// Modify terrain border
	CAutoTerrain* pAutoTerrain = m_pAutoHome->GetAutoTerrain();
	if (!pAutoTerrain->SmoothHeightBorder())
		return false;
	if (!pAutoTerrain->ApplyHeightAndNormal())
		return false;
	if (!pAutoTerrain->ApplyTerrainColor())
		return false;

	int nBlockRow = m_pAutoHome->GetRowIndex();
	int nBlockCol = m_pAutoHome->GetColIndex();
	CAutoHome* pHome;
	if (0 == m_nSelectedPoint)
	{
		pHome = m_pAutoHome->GetAutoScene()->GetHome(nBlockRow - 1, nBlockCol);
		if (NULL != pHome)
			pHome->SmoothAndApplyBorder();
		pHome = m_pAutoHome->GetAutoScene()->GetHome(nBlockRow - 1, nBlockCol - 1);
		if (NULL != pHome)
			pHome->SmoothAndApplyBorder();
		pHome = m_pAutoHome->GetAutoScene()->GetHome(nBlockRow, nBlockCol - 1);
		if (NULL != pHome)
			pHome->SmoothAndApplyBorder();
	}
	else if (m_nSelectedPoint>=1 && m_nSelectedPoint<=m_nNumInside)
	{
		pHome = m_pAutoHome->GetAutoScene()->GetHome(nBlockRow - 1, nBlockCol);
		if (NULL != pHome)
			pHome->SmoothAndApplyBorder();
	}
	else if ((m_nNumInside+1) == m_nSelectedPoint)
	{
		pHome = m_pAutoHome->GetAutoScene()->GetHome(nBlockRow - 1, nBlockCol);
		if (NULL != pHome)
			pHome->SmoothAndApplyBorder();
		pHome = m_pAutoHome->GetAutoScene()->GetHome(nBlockRow - 1, nBlockCol + 1);
		if (NULL != pHome)
			pHome->SmoothAndApplyBorder();
		pHome = m_pAutoHome->GetAutoScene()->GetHome(nBlockRow, nBlockCol + 1);
		if (NULL != pHome)
			pHome->SmoothAndApplyBorder();
	}
	else if (m_nSelectedPoint>=(m_nNumInside+2) && m_nSelectedPoint<=((m_nNumInside+1)*2-1))
	{
		pHome = m_pAutoHome->GetAutoScene()->GetHome(nBlockRow, nBlockCol + 1);
		if (NULL != pHome)
			pHome->SmoothAndApplyBorder();
	}
	else if ((m_nNumInside+1)*2 == m_nSelectedPoint)
	{
		pHome = m_pAutoHome->GetAutoScene()->GetHome(nBlockRow, nBlockCol + 1);
		if (NULL != pHome)
			pHome->SmoothAndApplyBorder();
		pHome = m_pAutoHome->GetAutoScene()->GetHome(nBlockRow + 1, nBlockCol + 1);
		if (NULL != pHome)
			pHome->SmoothAndApplyBorder();
		pHome = m_pAutoHome->GetAutoScene()->GetHome(nBlockRow + 1, nBlockCol);
		if (NULL != pHome)
			pHome->SmoothAndApplyBorder();
	}
	else if (m_nSelectedPoint>=((m_nNumInside+1)*2+1) && m_nSelectedPoint<=((m_nNumInside+1)*3-1))
	{
		pHome = m_pAutoHome->GetAutoScene()->GetHome(nBlockRow + 1, nBlockCol);
		if (NULL != pHome)
			pHome->SmoothAndApplyBorder();
	}
	else if ((m_nNumInside+1)*3 == m_nSelectedPoint)
	{
		pHome = m_pAutoHome->GetAutoScene()->GetHome(nBlockRow, nBlockCol - 1);
		if (NULL != pHome)
			pHome->SmoothAndApplyBorder();
		pHome = m_pAutoHome->GetAutoScene()->GetHome(nBlockRow + 1, nBlockCol - 1);
		if (NULL != pHome)
			pHome->SmoothAndApplyBorder();
		pHome = m_pAutoHome->GetAutoScene()->GetHome(nBlockRow + 1, nBlockCol);
		if (NULL != pHome)
			pHome->SmoothAndApplyBorder();
	}
	else if (m_nSelectedPoint>=((m_nNumInside+1)*3+1) && m_nSelectedPoint<=((m_nNumInside+1)*4-1))
	{
		pHome = m_pAutoHome->GetAutoScene()->GetHome(nBlockRow, nBlockCol - 1);
		if (NULL != pHome)
			pHome->SmoothAndApplyBorder();
	}
	
	return true;
}

bool CAutoHomeBorder::MovePointData(int nIndex, float fDis)
{
	CAutoHomeBorderData* pBorderData = m_pAutoHome->GetAutoScene()->GetAutoHomeBorderData();
	float fPos;

	switch (m_pBorderPoints[nIndex].nType)
	{
	case BPT_CROSS:
		fPos = pBorderData->GetPointCrossByIndex(m_pBorderPoints[nIndex].nIndex);
		fPos += fDis;
		pBorderData->SetPointCrossByIndex(m_pBorderPoints[nIndex].nIndex, fPos);
		break;
	case BPT_ROW:
		fPos = pBorderData->GetPointRowByIndex(m_pBorderPoints[nIndex].nIndex);
		fPos += fDis;
		pBorderData->SetPointRowByIndex(m_pBorderPoints[nIndex].nIndex, fPos);
		break;
	case BPT_COL:
		fPos = pBorderData->GetPointColByIndex(m_pBorderPoints[nIndex].nIndex);
		fPos += fDis;
		pBorderData->SetPointColByIndex(m_pBorderPoints[nIndex].nIndex, fPos);
		break;
	}
	return true;
}