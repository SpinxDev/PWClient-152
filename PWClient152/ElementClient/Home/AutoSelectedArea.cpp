/*
 * FILE: AutoSelectedArea.cpp
 *
 * DESCRIPTION: Class for selecting a area or line
 *
 * CREATED BY: Jiang Dalong, 2006/04/06
 *
 * HISTORY: 
 *
 * Copyright (c) 2001-2008 Archosaur Studio, All Rights Reserved.
 */

#include "AutoSelectedArea.h"
#include "AutoHome.h"
#include "AutoTerrain.h"
#include "AutoHomeMan.h"
#include "Render.h"
#include <A3DEngine.h>
#include <A3DWireCollector.h>
#include <A3DCollision.h>

const int EDIT_BORDER		= 5;

CAutoSelectedArea::CAutoSelectedArea(CAutoHome* pAutoHome)
{
	m_pAutoHome				= pAutoHome;
	m_pViewPoints			= NULL;
	m_pPolyEditPoints		= NULL;
	m_bPolyEditAreaClosed	= false;
	m_bViewAreaClosed		= false;
	m_bPolyEditAreaChanged	= true;
	m_fMaxPolyEditAreaEdge	= 100.0f;
	m_pViewPoints			= NULL;
	m_pLineEditPoints		= NULL;
	m_bLineEditAreaChanged	= true;
	m_fMaxLineEditAreaEdge	= 100.0f;
	m_nAreatype				= AT_NONE;
}

CAutoSelectedArea::~CAutoSelectedArea()
{
	Release();
}

void CAutoSelectedArea::Release()
{
	SAFERELEASE(m_pViewPoints);
	SAFERELEASE(m_pPolyEditPoints);
	SAFERELEASE(m_pLineEditPoints);
}

bool CAutoSelectedArea::AddPolyEditPoint(const POINT_FLOAT& pt, const A3DVECTOR3& vStart, const A3DVECTOR3& vEnd)
{
	POINT_FLOAT point;
	if (!WorldToHomeRect(pt, point))
		return false;

	// If this point is very closed to the first point, the area is close
	if (m_aPolyEditPoints.GetSize() >= 3)
	{
		if (RayTracePoint(vStart, vEnd, m_aPolyEditPoints[0]))
		{
			if (!IsNextPointCrossLine(m_aPolyEditPoints[0], m_aPolyEditPoints, false))
			{
				m_bPolyEditAreaClosed = true;
				return true;
			}
		}
	}

	if (IsNextPointCrossLine(point, m_aPolyEditPoints))
		return false;

	m_aPolyEditPoints.Add(point);

	m_bPolyEditAreaChanged = true;
	
	return true;
}

bool CAutoSelectedArea::DeletePolyLastEditPoint()
{
	if (m_aPolyEditPoints.GetSize() < 0)
		return false;

	m_aPolyEditPoints.RemoveTail();

	m_bPolyEditAreaChanged = true;

	return true;
}

bool CAutoSelectedArea::DeletePolyEditPoint(int nIndex)
{
	if (nIndex < 0 || nIndex >= m_aPolyEditPoints.GetSize())
		return false;

	m_aPolyEditPoints.RemoveAt(nIndex);

	m_bPolyEditAreaChanged = true;

	return true;
}

void CAutoSelectedArea::DeleteAllPolyEditPoints()
{
	m_aPolyEditPoints.RemoveAll();

	m_bPolyEditAreaChanged = true;
	
	m_bPolyEditAreaClosed = false;
}

POINT_FLOAT* CAutoSelectedArea::GetPolyEditPoints()
{
	int nNumPoints = m_aPolyEditPoints.GetSize();

	if (nNumPoints < 0)
		return NULL;

	SAFERELEASE(m_pPolyEditPoints);
	m_pPolyEditPoints = new POINT_FLOAT[nNumPoints];

	int i;
	for (i=0; i<nNumPoints; i++)
	{
		m_pPolyEditPoints[i].x = m_aPolyEditPoints[i].x;
		m_pPolyEditPoints[i].y = m_aPolyEditPoints[i].y;
	}

	m_bPolyEditAreaChanged = true;

	return m_pPolyEditPoints;
}

bool CAutoSelectedArea::SetPolyEditPoints(int nNumPoints, POINT_FLOAT* pPoints)
{
	if (nNumPoints <= 0)
		return false;

	m_aPolyEditPoints.RemoveAll();

	int i;
	for (i=0; i<nNumPoints; i++)
	{
		m_aPolyEditPoints.Add(pPoints[i]);		
	}

	return true;
}

void CAutoSelectedArea::DeleteAllViewPoints()
{
	m_aViewPoints.RemoveAll();
}

POINT_FLOAT* CAutoSelectedArea::GetViewPoints()
{
	int nNumPoints = m_aViewPoints.GetSize();

	if (nNumPoints < 0)
		return NULL;

	SAFERELEASE(m_pViewPoints);
	m_pViewPoints = new POINT_FLOAT[nNumPoints];

	int i;
	for (i=0; i<nNumPoints; i++)
	{
		m_pViewPoints[i].x = m_aViewPoints[i].x;
		m_pViewPoints[i].y = m_aViewPoints[i].y;
	}

	return m_pViewPoints;
}

bool CAutoSelectedArea::SetViewPoints(int nNumPoints, POINT_FLOAT* pPoints)
{
	if (nNumPoints <= 0)
		return false;

	m_aViewPoints.RemoveAll();

	int i;
	for (i=0; i<nNumPoints; i++)
	{
		m_aViewPoints.Add(pPoints[i]);		
	}

	return true;
}

bool CAutoSelectedArea::CopyPointsFromViewToPolyEdit()
{
	m_aPolyEditPoints.RemoveAll();

	int nNumPoints = m_aViewPoints.GetSize();
	if (nNumPoints <= 0)
		return true;

	m_aPolyEditPoints.Append(m_aViewPoints);

	m_bPolyEditAreaChanged = true;
	
	return true;
}

bool CAutoSelectedArea::CopyPointsFromPolyEditToView()
{
	m_aViewPoints.RemoveAll();

	int nNumPoints = m_aPolyEditPoints.GetSize();
	if (nNumPoints <= 0)
		return true;

	m_aViewPoints.Append(m_aPolyEditPoints);

	return true;
}

bool CAutoSelectedArea::Render()
{
	A3DWireCollector* pWireCollector = g_Render.GetA3DEngine()->GetA3DWireCollector();
	A3DVECTOR3 v0, v1, v2;

	float fPointRadius = 2.0f;
	DWORD dwPointColor;
	int i;
	int nNumPoints;

	if (m_nAreatype == AT_NONE)
		return true;

	// Draw view points
	nNumPoints = m_aViewPoints.GetSize();
	if (nNumPoints > 0)
	{
		dwPointColor = 0xff0000ff;
		m_pAutoHome->HeightMapToWorld(int(m_aViewPoints[0].x), int(m_aViewPoints[0].y), v0);
		v1 = v0;
		pWireCollector->AddSphere(v0, fPointRadius, dwPointColor);
		for (i=1; i<nNumPoints; i++)
		{
			m_pAutoHome->HeightMapToWorld(int(m_aViewPoints[i].x), int(m_aViewPoints[i].y), v2);
			pWireCollector->AddSphere(v2, fPointRadius, dwPointColor);
			pWireCollector->Add3DLine(v1, v2, dwPointColor);
			v1 = v2;
		}
		if (m_bViewAreaClosed)
			pWireCollector->Add3DLine(v2, v0, dwPointColor);
	}
	
	// Draw poly edit points
	nNumPoints = m_aPolyEditPoints.GetSize();
	if (nNumPoints > 0)
	{
		if (m_nAreatype != AT_POLY)
			dwPointColor = 0xffa0a0a0;
		else if (m_bPolyEditAreaClosed)
			dwPointColor = 0xff00ff00;
		else
			dwPointColor = 0xffff0000;
		m_pAutoHome->HeightMapToWorld(int(m_aPolyEditPoints[0].x), int(m_aPolyEditPoints[0].y), v0);
		v1 = v0;
		pWireCollector->AddSphere(v0, fPointRadius, dwPointColor);
		for (i=1; i<nNumPoints; i++)
		{
			m_pAutoHome->HeightMapToWorld(int(m_aPolyEditPoints[i].x), int(m_aPolyEditPoints[i].y), v2);
			pWireCollector->AddSphere(v2, fPointRadius, dwPointColor);
			pWireCollector->Add3DLine(v1, v2, dwPointColor);
			v1 = v2;
		}
		if (m_bPolyEditAreaClosed)
			pWireCollector->Add3DLine(v2, v0, dwPointColor);
	}

	// Draw line edit points
	nNumPoints = m_aLineEditPoints.GetSize();
	if (nNumPoints > 0)
	{
		if (m_nAreatype != AT_LINE)
			dwPointColor = 0xffa0a0a0;
		else
			dwPointColor = 0xff00ff00;
		m_pAutoHome->HeightMapToWorld(int(m_aLineEditPoints[0].x), int(m_aLineEditPoints[0].y), v0);
		v1 = v0;
		pWireCollector->AddSphere(v0, fPointRadius, dwPointColor);
		for (i=1; i<nNumPoints; i++)
		{
			m_pAutoHome->HeightMapToWorld(int(m_aLineEditPoints[i].x), int(m_aLineEditPoints[i].y), v2);
			pWireCollector->AddSphere(v2, fPointRadius, dwPointColor);
			pWireCollector->Add3DLine(v1, v2, dwPointColor);
			v1 = v2;
		}
	}

	return true;
}

bool CAutoSelectedArea::SetViewAreaClosed(bool bClose)
{
	if (!bClose)
	{
		m_bViewAreaClosed = false;
		return true;
	}

	if (!PolyAreaCanClosed(m_aViewPoints))
	{
		m_bViewAreaClosed = false;
		return false;
	}

	m_bViewAreaClosed = true;
	return true;
}

bool CAutoSelectedArea::SetPolyEditAreaClosed(bool bClose)
{
	if (!bClose)
	{
		m_bPolyEditAreaClosed = false;
		return true;
	}

	if (!PolyAreaCanClosed(m_aPolyEditPoints))
	{
		m_bPolyEditAreaClosed = false;
		return false;
	}

	m_bPolyEditAreaClosed = true;
	return true;
}

// Is a new point make the lines cross?
bool CAutoSelectedArea::IsNextPointCrossLine(const POINT_FLOAT& point, AArray<POINT_FLOAT, POINT_FLOAT&>& pPoints, bool bFromFirstPoint)
{
	bool bCross = false;

	int nNumPoints = pPoints.GetSize();
	if (nNumPoints > 2)
	{
		int nStart = bFromFirstPoint ? 0 : 1;
		for (int i=nStart; i<nNumPoints-2; i++)
		{
			if (IsTwoLineCross(pPoints[i], pPoints[i+1], pPoints[nNumPoints-1], point))
			{
				bCross = true;
				break;
			}
		}
	}

	return bCross;
}

// Calculate center and min edge of edit area
bool CAutoSelectedArea::GetEditAreaCenterAndMaxEdge(AArray<POINT_FLOAT, POINT_FLOAT&>& aPoints, float* pfMaxAreaEdge, POINT_FLOAT& ptCenter, bool* pbChanged)
{
	int nNumPoints = aPoints.GetSize();
	if (nNumPoints <= 0)
		return false;

	int i;
	POINT_FLOAT ptTotal;
	float fLeft, fRight, fTop, fBottom;
	ptTotal.x = 0.0f;
	ptTotal.y = 0.0f;
	fLeft = 100000.0f;
	fRight = 0.0f;
	fTop = 0.0f;
	fBottom = 100000.0f;
	for (i=0; i<nNumPoints; i++)
	{
		ptTotal.x += aPoints[i].x;
		ptTotal.y += aPoints[i].y;
		if (aPoints[i].x < fLeft)
			fLeft = aPoints[i].x;
		if (aPoints[i].x > fRight)
			fRight = aPoints[i].x;
		if (aPoints[i].y < fBottom)
			fBottom = aPoints[i].y;
		if (aPoints[i].y > fTop)
			fTop = aPoints[i].y;
	}

	ptCenter.x = ptTotal.x / nNumPoints;
	ptCenter.y = ptTotal.y / nNumPoints;

	float fWidth, fHeight;
	fWidth = fRight - fLeft;
	fHeight = fTop - fBottom;
	if (fWidth > fHeight)
		*pfMaxAreaEdge = fWidth;
	else
		*pfMaxAreaEdge = fHeight;

	*pbChanged = false;

	return true;
}

bool CAutoSelectedArea::TranslateEditPoints(A3DVECTOR3& vTrans, AArray<POINT_FLOAT, POINT_FLOAT&>& aPoints, bool* pbChanged)
{
	POINT_FLOAT pt;

	float fTerrainSize = m_pAutoHome->GetAutoTerrain()->GetTerrainSize() / 2;
	float fRatio = m_pAutoHome->GetAutoTerrain()->GetHeightMap()->iWidth / fTerrainSize;
	pt.x = vTrans.x / fRatio;
	pt.y = -vTrans.z / fRatio;

	AArray<POINT_FLOAT, POINT_FLOAT&> aTempPoints;

	aTempPoints.Append(aPoints);

	int i;
	for (i=0; i<aPoints.GetSize(); i++)
	{
		aTempPoints[i].x += pt.x;
		aTempPoints[i].y += pt.y;
		if (!IsPointInMap(aTempPoints[i]))
			return false;
	}

	aPoints.RemoveAll();
	aPoints.Append(aTempPoints);

	*pbChanged = true;

	return true;
}

bool CAutoSelectedArea::IsPointInMap(const POINT_FLOAT& point)
{
	float fTerrainSize = m_pAutoHome->GetAutoTerrain()->GetTerrainSize() / 2;

	if (point.x < 0 || point.x > fTerrainSize || point.y < 0 || point.y > fTerrainSize)
		return false;

	return true;
}

bool CAutoSelectedArea::RotateEditPoints(float fAngle, AArray<POINT_FLOAT, POINT_FLOAT&>& aPoints, float* pfMaxAreaEdge, POINT_FLOAT& ptCenter, bool* pbChanged)
{
	if (*pbChanged)
		GetEditAreaCenterAndMaxEdge(aPoints, pfMaxAreaEdge, ptCenter, pbChanged);

	AArray<POINT_FLOAT, POINT_FLOAT&> aTempPoints;
	aTempPoints.Append(aPoints);

	int i;
	double dSin, dCos;
	float fTempX, fTempY;
	dSin = sin(fAngle);
	dCos = cos(fAngle);
	for (i=0; i<aPoints.GetSize(); i++)
	{
		fTempX = aTempPoints[i].x - ptCenter.x;
		fTempY = aTempPoints[i].y - ptCenter.y;
		aTempPoints[i].x = (float)(fTempX * dCos - fTempY * dSin) + ptCenter.x;
		aTempPoints[i].y = (float)(fTempX * dSin + fTempY * dCos) + ptCenter.y;
		if (!IsPointInMap(aTempPoints[i]))
			return false;
	}

	aPoints.RemoveAll();
	aPoints.Append(aTempPoints);

	return true;
}

bool CAutoSelectedArea::ScaleEditPoints(float fScale, AArray<POINT_FLOAT, POINT_FLOAT&>& aPoints, float* pfMaxAreaEdge, POINT_FLOAT& ptCenter, bool* pbChanged)
{
	if (*pbChanged)
		GetEditAreaCenterAndMaxEdge(aPoints, pfMaxAreaEdge, ptCenter, pbChanged);

	if (fScale < 1.0f && *pfMaxAreaEdge < 5)
		return false;

	AArray<POINT_FLOAT, POINT_FLOAT&> aTempPoints;
	aTempPoints.Append(aPoints);

	int i;
	for (i=0; i<aPoints.GetSize(); i++)
	{
		aTempPoints[i].x -= ptCenter.x;
		aTempPoints[i].y -= ptCenter.y;
		aTempPoints[i].x *= fScale;
		aTempPoints[i].y *= fScale;
		aTempPoints[i].x += ptCenter.x;
		aTempPoints[i].y += ptCenter.y;

		if (!IsPointInMap(aTempPoints[i]))
			return false;
	}

	aPoints.RemoveAll();
	aPoints.Append(aTempPoints);

	*pfMaxAreaEdge *= fScale;

	return true;
}

// Check whether area can be closed
bool CAutoSelectedArea::PolyAreaCanClosed(AArray<POINT_FLOAT, POINT_FLOAT&>& aPoints)
{
	int nNumPoints = aPoints.GetSize();
	if (nNumPoints < 3)
		return false;

	int i, j;
	for (i=2; i<nNumPoints-1; i++)
	{
		for (j=0; j<i-1; j++)
		{
			if (IsTwoLineCross(aPoints[i], aPoints[i+1],
							aPoints[j], aPoints[j+1]))
				return false;
		}
	}
	for (j=1; j<nNumPoints-2; j++)
	{
		if (IsTwoLineCross(aPoints[nNumPoints-1], aPoints[0],
							aPoints[j], aPoints[j+1]))
				return false;
	}

	return true;
}

bool CAutoSelectedArea::TranslatePolyEditPoints(A3DVECTOR3& vTrans)
{
	return TranslateEditPoints(vTrans, m_aPolyEditPoints, &m_bPolyEditAreaChanged);
}

bool CAutoSelectedArea::RotatePolyEditPoints(float fAngle)
{
	return RotateEditPoints(fAngle, m_aPolyEditPoints, &m_fMaxPolyEditAreaEdge, m_ptPolyEditCenter, &m_bPolyEditAreaChanged);
}

bool CAutoSelectedArea::ScalePolyEditPoints(float fScale)
{
	return ScaleEditPoints(fScale, m_aPolyEditPoints, &m_fMaxPolyEditAreaEdge, m_ptPolyEditCenter, &m_bPolyEditAreaChanged);
}

bool CAutoSelectedArea::AddLineEditPoint(const POINT_FLOAT& pt)
{
	POINT_FLOAT point;
	if (!WorldToHomeRect(pt, point))
		return false;

	m_aLineEditPoints.Add(point);

	m_bLineEditAreaChanged = true;
	
	return true;
}

bool CAutoSelectedArea::DeleteLineLastEditPoint()
{
	if (m_aLineEditPoints.GetSize() < 0)
		return false;

	m_aLineEditPoints.RemoveTail();

	m_bLineEditAreaChanged = true;

	return true;
}

bool CAutoSelectedArea::DeleteLineEditPoint(int nIndex)
{
	if (nIndex < 0 || nIndex >= m_aLineEditPoints.GetSize())
		return false;

	m_aLineEditPoints.RemoveAt(nIndex);

	m_bLineEditAreaChanged = true;

	return true;
}

void CAutoSelectedArea::DeleteAllLineEditPoints()
{
	m_aLineEditPoints.RemoveAll();

	m_bLineEditAreaChanged = true;
}

POINT_FLOAT* CAutoSelectedArea::GetLineEditPoints()
{
	int nNumPoints = m_aLineEditPoints.GetSize();

	if (nNumPoints < 0)
		return NULL;

	SAFERELEASE(m_pLineEditPoints);
	m_pLineEditPoints = new POINT_FLOAT[nNumPoints];

	int i;
	for (i=0; i<nNumPoints; i++)
	{
		m_pLineEditPoints[i].x = m_aLineEditPoints[i].x;
		m_pLineEditPoints[i].y = m_aLineEditPoints[i].y;
	}

	m_bLineEditAreaChanged = true;

	return m_pLineEditPoints;
}

bool CAutoSelectedArea::SetLineEditPoints(int nNumPoints, POINT_FLOAT* pPoints)
{
	if (nNumPoints <= 0)
		return false;

	m_aLineEditPoints.RemoveAll();

	int i;
	for (i=0; i<nNumPoints; i++)
	{
		m_aLineEditPoints.Add(pPoints[i]);		
	}

	return true;
}

bool CAutoSelectedArea::TranslateLineEditPoints(A3DVECTOR3& vTrans)
{
	return TranslateEditPoints(vTrans, m_aLineEditPoints, &m_bLineEditAreaChanged);
}

bool CAutoSelectedArea::RotateLineEditPoints(float fAngle)
{
	return RotateEditPoints(fAngle, m_aLineEditPoints, &m_fMaxLineEditAreaEdge, m_ptLineEditCenter, &m_bLineEditAreaChanged);
}

bool CAutoSelectedArea::ScaleLineEditPoints(float fScale)
{
	return ScaleEditPoints(fScale, m_aLineEditPoints, &m_fMaxLineEditAreaEdge, m_ptLineEditCenter, &m_bLineEditAreaChanged);
}

bool CAutoSelectedArea::CopyPointsFromViewToLineEdit()
{
	m_aLineEditPoints.RemoveAll();

	int nNumPoints = m_aViewPoints.GetSize();
	if (nNumPoints <= 0)
		return true;

	m_aLineEditPoints.Append(m_aViewPoints);

	m_bLineEditAreaChanged = true;
	
	return true;
}

bool CAutoSelectedArea::CopyPointsFromLineEditToView()
{
	m_aViewPoints.RemoveAll();

	int nNumPoints = m_aLineEditPoints.GetSize();
	if (nNumPoints <= 0)
		return true;

	m_aViewPoints.Append(m_aLineEditPoints);

	return true;
}

// Is a new point make the lines cross?
bool CAutoSelectedArea::IsNextPointCrossLine(const POINT_FLOAT& point, AArray<POINT_FLOAT, POINT_FLOAT&>& aPoints)
{
	bool bCross = false;

	int nNumPoints = aPoints.GetSize();
	if (nNumPoints > 2)
	{
		for (int i=0; i<nNumPoints-2; i++)
		{
			if (IsTwoLineCross(aPoints[i], aPoints[i+1], aPoints[nNumPoints-1], point))
			{
				bCross = true;
				break;
			}
		}
	}

	return bCross;
}

// Check ray trace with a height map point
bool CAutoSelectedArea::RayTracePoint(const A3DVECTOR3& vStart, const A3DVECTOR3& vEnd, const POINT_FLOAT& pt)
{
	A3DVECTOR3 ptWorld;
	if (!m_pAutoHome->HeightMapToWorld(int(pt.x), int(pt.y), ptWorld))
		return false;

	A3DVECTOR3 vMins, vMaxs;
	A3DVECTOR3 vExt(2.0f, 2.0f, 2.0f);
	vMins = ptWorld - vExt;
	vMaxs = ptWorld + vExt;

	A3DVECTOR3 vPoint, vNormal;
	float fFraction;
	if (CLS_RayToAABB3(vStart, vEnd-vStart, vMins, vMaxs, vPoint, &fFraction, vNormal))
		return true;

	return false;
}

// From world to home rect
bool CAutoSelectedArea::WorldToHomeRect(const POINT_FLOAT& ptWorld, POINT_FLOAT& ptHome)
{
	// Is point in the home block?
	A3DVECTOR3 vPos(ptWorld.x, 0, ptWorld.y);
	int x, z;
	if (!m_pAutoHome->WorldToHeightMap(vPos, &x, &z))
		return false;

	// Is point in the edit rect?
	int nHomeWidth = m_pAutoHome->GetAutoTerrain()->GetHeightMap()->iWidth;
	int nHomeHeight = m_pAutoHome->GetAutoTerrain()->GetHeightMap()->iHeight;
	if (x < EDIT_BORDER || x > nHomeWidth - EDIT_BORDER - 1
		|| z < EDIT_BORDER || z > nHomeHeight - EDIT_BORDER - 1)
		return false;

	ptHome.x = (float)x;
	ptHome.y = (float)z;

	return true;
}