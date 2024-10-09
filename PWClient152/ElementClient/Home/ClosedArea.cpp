/*
 * FILE: ClosedArea.cpp
 *
 * DESCRIPTION: Class for a closed area
 *
 * CREATED BY: Jiang Dalong, 2005/03/25
 *
 * HISTORY: 
 *
 * Copyright (c) 2001-2008 Archosaur Studio, All Rights Reserved.
 */

#include "ClosedArea.h"
#include <math.h>
#include <AAssist.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CClosedArea::CClosedArea()
{
	m_bAreaFinished = false;
	m_pPoints = NULL;
	m_nNumPoints = 0;
	m_pPointsBak = NULL;
	m_nNumPointsBak = 0;
}

CClosedArea::~CClosedArea()
{
	Release();
}

// Release memory
void CClosedArea::Release()
{
	ReleasePoints();
	ReleasePointsBak();
}

// Release points
void CClosedArea::ReleasePoints()
{
	if (m_pPoints)
	{
		delete[] m_pPoints;
		m_pPoints = NULL;
	}
}

// Release backuped points
void CClosedArea::ReleasePointsBak()
{
	if (m_pPointsBak)
	{
		delete[] m_pPointsBak;
		m_pPointsBak = NULL;
	}
}

// Add a point
void CClosedArea::AddPoint(POINT_FLOAT& point)
{
	m_aPoints.Add(point);
	SetAreaFinished(false);
}

// Finish area
bool CClosedArea::AreaFinish()
{
	ReleasePoints();

	int nNumPoints = m_aPoints.GetSize();
	if (nNumPoints <= 0)
		return false;

	m_pPoints = new POINT_FLOAT[nNumPoints];
	if (!m_pPoints)
		return false;

	for (int i=0; i<nNumPoints; i++)
	{
		m_pPoints[i].x = m_aPoints[i].x;
		m_pPoints[i].y = m_aPoints[i].y;
	}

	m_nNumPoints = nNumPoints;
	SetAreaFinished(true);

	return true;
}

// Get a point
POINT_FLOAT& CClosedArea::GetPoint(int nIndex)
{
	return m_aPoints[nIndex];
}

// Is a new point make the lines cross?
bool CClosedArea::IsNextPointCrossLine(POINT_FLOAT& point, bool bFromFirstPoint)
{
	bool bCross = false;

	int nNumPoints = m_aPoints.GetSize();
	if (nNumPoints > 2)
	{
		int nStart = bFromFirstPoint ? 0 : 1;
		for (int i=nStart; i<nNumPoints-2; i++)
		{
			if (IsTwoLineCross(m_aPoints[i], m_aPoints[i+1], m_aPoints[nNumPoints-1], point))
			{
				bCross = true;
				break;
			}
		}
	}

	return bCross;
}

// Clear area
void CClosedArea::Clear()
{
	m_aPoints.RemoveAll();
	SetAreaFinished(false);
}

// Remove last point
void CClosedArea::RemoveLastPoint()
{
	if (m_aPoints.GetSize() > 0)
		m_aPoints.RemoveAt(m_aPoints.GetSize()-1);
	SetAreaFinished(false);
}

// Is a point in area?
bool CClosedArea::IsInArea(POINT_FLOAT& point)
{
	if (m_nNumPoints < 3)
		return false;

	int nNumCross = 0;

	for (int i=0; i<m_nNumPoints; i++)
	{
		if (IsCrossLine(point, i))// �Ӽ������Ҳ������������Ƿ���ñ��ཻ
			nNumCross++;
	}

	if ((nNumCross & 1) == 0)// ����ż�������㣬���ڶ������
		return false;
	else// �������������㣬�ڶ������
		return true;
}

// �ж������Ƿ�����ཻ
bool CClosedArea::IsCrossLine(POINT_FLOAT pt, int nIndex)
{
	POINT_FLOAT ptLine1 = m_pPoints[nIndex];// �߶ζ���1
	POINT_FLOAT ptLine2 = m_pPoints[(nIndex+1)%m_nNumPoints];// �߶ζ���2

	if (ptLine1.x < pt.x && ptLine2.x < pt.x)// ����߶����������
		return false;

	if (ptLine1.y < pt.y && ptLine2.y < pt.y)// ����߶��������ϱ�
		return false;

	if (ptLine1.y > pt.y && ptLine2.y > pt.y)// ����߶��������±�
		return false;

	if (ptLine1.y == ptLine2.y)// ����߶���������ͬһˮƽ����
		return false;

	if (pt.y == ptLine1.y)// ������ߴ����߶ζ���1
	{
		// �ҵ�����1֮ǰ����������߲���ͬһˮƽ���ϵĵ�
		POINT_FLOAT ptLinePre;// ����1֮ǰ�ĵ�
		bool bFind = false;
		for (int i=1; i<m_nNumPoints; i++)
		{
			ptLinePre = m_pPoints[(nIndex + m_nNumPoints - i) % m_nNumPoints];
			if (ptLinePre.y != pt.y)// ����ɨ������
			{
				bFind = true;
				break;
			}
		}
		if (!bFind)// û�ҵ����������ĵ㣨��������е���ͬһˮƽ���ϣ�
			return false;

		if ((ptLinePre.y < pt.y && ptLine2.y > pt.y)
			|| (ptLinePre.y > pt.y && ptLine2.y < pt.y))// �������1֮ǰ���붥��2����������
			return false;
	}

	// ����������ߵĽ���
	float fSlope = (ptLine2.x - ptLine1.x) / (ptLine2.y - ptLine1.y);
	float fCrossX = (pt.y - ptLine1.y) * fSlope + ptLine1.x;
	if (fCrossX <= pt.x)//�����ڲ��Ե����
		return false;

	return true;
}

// Distance to a point
float CClosedArea::DistanceToPoint(POINT_FLOAT pt)
{
	float fDis;
	float fMinDis = 9999999;
	int nType;

	POINT_FLOAT ptCross;
	for (int i=0; i<m_nNumPoints; i++)
	{
		fDis = ::DistanceToPartLine(pt, m_pPoints[i], m_pPoints[(i+1)%m_nNumPoints], &ptCross, &nType);
		if (fDis < fMinDis)
			fMinDis = fDis;
	}
	
	return fMinDis;
}

// Set points
bool CClosedArea::SetPoints(int nCount, POINT_FLOAT* pPoints)
{
	Clear();
	ReleasePoints();

	if (nCount < 0)
		return false;

	m_pPoints = new POINT_FLOAT[nCount];
	if (!m_pPoints)
		return false;

	for (int i=0; i<nCount; i++)
	{
		m_aPoints.Add(pPoints[i]);
		m_pPoints[i].x = pPoints[i].x;
		m_pPoints[i].y = pPoints[i].y;
	}

	m_nNumPoints = nCount;
	SetAreaFinished(true);
	
	return true;
}

// Store points
bool CClosedArea::StorePoints()
{
	m_nNumPointsBak = m_nNumPoints;
	ReleasePointsBak();

	m_pPointsBak = new POINT_FLOAT[m_nNumPointsBak];
	if (NULL == m_pPointsBak)
		return false;

	memcpy(m_pPointsBak, m_pPoints, sizeof(POINT_FLOAT)*m_nNumPointsBak);

	return true;
}

// Restore points
bool CClosedArea::RestorePoints()
{
	m_nNumPoints = m_nNumPointsBak;
	ReleasePoints();

	m_pPoints = new POINT_FLOAT[m_nNumPoints];
	if (NULL == m_pPoints)
		return false;

	memcpy(m_pPoints, m_pPointsBak, sizeof(POINT_FLOAT)*m_nNumPoints);

	int i;
	m_aPoints.RemoveAll();
	for (i=0; i<m_nNumPoints; i++)
	{
		m_aPoints.Add(m_pPoints[i]);
	}

	return true;
}

// Is polygon valid
bool CClosedArea::IsPolygonValid()
{
	if (!IsAreaFinished())
		return false;

	if (GetNumPoints() < 3)
		return false;

	return true;
}
