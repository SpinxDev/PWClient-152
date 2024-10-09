/*
 * FILE: AutoHomeBorderData.h
 *
 * DESCRIPTION: Class for lines of home border data
 *
 * CREATED BY: Jiang Dalong, 2006/06/19
 *
 * HISTORY: 
 *
 * Copyright (c) 2001-2008 Archosaur Studio, All Rights Reserved.
 */

#include "AutoHomeBorderData.h"
#include "CurvePoint.h"
#include "AutoSceneFunc.h"

#include <AFile.h>
#include <AFileImage.h>

const int NUM_BORDER_BLOCK = 8;
const int NUM_HOME_BORDER_POINT = 7;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAutoHomeBorderData::CAutoHomeBorderData()
{
	m_pPointCross	= NULL;
	m_pPointRow		= NULL;
	m_pPointCol		= NULL;
	m_bEdited		= false;
}

CAutoHomeBorderData::~CAutoHomeBorderData()
{

}

void CAutoHomeBorderData::Release()
{
	SAFERELEASE(m_pPointCross);
	SAFERELEASE(m_pPointRow);
	SAFERELEASE(m_pPointCol);
}

bool CAutoHomeBorderData::SavePoints(const char* szFile)
{
	AFile f;
	if (!f.Open(szFile, AFILE_CREATENEW))
		return false;

	DWORD dwLen;
	
	// Write cross points
	if (!f.Write(&m_nNumPointCross, sizeof(int), &dwLen))
		goto WRITEFAIL;
	if (!f.Write(m_pPointCross, sizeof(int)*m_nNumPointCross, &dwLen))
		goto WRITEFAIL;

	// Write row points
	if (!f.Write(&m_nNumPointRow, sizeof(int), &dwLen))
		goto WRITEFAIL;
	if (!f.Write(m_pPointRow, sizeof(int)*m_nNumPointRow, &dwLen))
		goto WRITEFAIL;

	// Write column points
	if (!f.Write(&m_nNumPointCol, sizeof(int), &dwLen))
		goto WRITEFAIL;
	if (!f.Write(m_pPointCol, sizeof(int)*m_nNumPointCol, &dwLen))
		goto WRITEFAIL;

	m_bEdited = false;

	f.Close();
	return true;

WRITEFAIL:
	f.Close();
	return true;
}

bool CAutoHomeBorderData::LoadPoints(const char* szFile)
{
	AFileImage f;
	if (!f.Open(szFile, AFILE_OPENEXIST))
		return false;
/*	
	m_nNumPointCross = (NUM_BORDER_BLOCK+1)*(NUM_BORDER_BLOCK+1);
	m_pPointCross = new float[m_nNumPointCross];
	int i;
	for (i=0; i<m_nNumPointCross; i++)
		m_pPointCross[i] = 20.0f;
	m_nNumPointRow = (NUM_HOME_BORDER_POINT-2)*NUM_BORDER_BLOCK*(NUM_BORDER_BLOCK+1);
	m_pPointRow = new float[m_nNumPointRow];
	for (i=0; i<m_nNumPointRow; i++)
		m_pPointRow[i] = 20.0f;
	m_nNumPointCol = (NUM_HOME_BORDER_POINT-2)*NUM_BORDER_BLOCK*(NUM_BORDER_BLOCK+1);
	m_pPointCol = new float[m_nNumPointCol];
	for (i=0; i<m_nNumPointCol; i++)
		m_pPointCol[i] = 20.0f;
*/
	DWORD dwLen;

	// Release old points
	Release();

	// Read cross points
	if (!f.Read(&m_nNumPointCross, sizeof(int), &dwLen))
		goto READFAIL;
	m_pPointCross = new float[m_nNumPointCross];
	if (!f.Read(m_pPointCross, sizeof(int)*m_nNumPointCross, &dwLen))
		goto READFAIL;

	// Read row points
	if (!f.Read(&m_nNumPointRow, sizeof(int), &dwLen))
		goto READFAIL;
	m_pPointRow = new float[m_nNumPointRow];
	if (!f.Read(m_pPointRow, sizeof(int)*m_nNumPointRow, &dwLen))
		goto READFAIL;

	// Read column points
	if (!f.Read(&m_nNumPointCol, sizeof(int), &dwLen))
		goto READFAIL;
	m_pPointCol = new float[m_nNumPointCol];
	if (!f.Read(m_pPointCol, sizeof(int)*m_nNumPointCol, &dwLen))
		goto READFAIL;

	f.Close();
	return true;

READFAIL:
	f.Close();
	return false;	
}

// Get home top value
float CAutoHomeBorderData::GetTopValue(int r, int c, float fWeight)
{
	float* pBorderPoints = new float[NUM_HOME_BORDER_POINT];

	GetHomeTopPoints(r, c, pBorderPoints);

	CCurvePoint cp;
	cp.SetPointValues(NUM_HOME_BORDER_POINT, pBorderPoints);
	float fResult = cp.GetValue(fWeight);

	return fResult;
}

// Get home bottom value
float CAutoHomeBorderData::GetBottomValue(int r, int c, float fWeight)
{
	float* pBorderPoints = new float[NUM_HOME_BORDER_POINT];

	GetHomeBottomPoints(r, c, pBorderPoints);

	CCurvePoint cp;
	cp.SetPointValues(NUM_HOME_BORDER_POINT, pBorderPoints);
	float fResult = cp.GetValue(fWeight);

	return fResult;
}

// Get home left value
float CAutoHomeBorderData::GetLeftValue(int r, int c, float fWeight)
{
	float* pBorderPoints = new float[NUM_HOME_BORDER_POINT];

	GetHomeLeftPoints(r, c, pBorderPoints);

	CCurvePoint cp;
	cp.SetPointValues(NUM_HOME_BORDER_POINT, pBorderPoints);
	float fResult = cp.GetValue(fWeight);

	return fResult;
}

// Get home right value
float CAutoHomeBorderData::GetRightValue(int r, int c, float fWeight)
{
	float* pBorderPoints = new float[NUM_HOME_BORDER_POINT];

	GetHomeRightPoints(r, c, pBorderPoints);

	CCurvePoint cp;
	cp.SetPointValues(NUM_HOME_BORDER_POINT, pBorderPoints);
	float fResult = cp.GetValue(fWeight);

	return fResult;
}

// Get home top points
bool CAutoHomeBorderData::GetHomeTopPoints(int r, int c, float* pPoints)
{
	int nCrossIndex = r * (NUM_BORDER_BLOCK + 1) + c;
	pPoints[0] = m_pPointCross[nCrossIndex];
	pPoints[NUM_HOME_BORDER_POINT-1] = m_pPointCross[nCrossIndex+1];

	int nRowIndex = (r * NUM_BORDER_BLOCK + c) * (NUM_HOME_BORDER_POINT - 2);
	for (int i=0; i<NUM_HOME_BORDER_POINT-2; i++)
	{
		pPoints[i+1] = m_pPointRow[nRowIndex+i];
	}

	return true;
}

// Get home bottom points
bool CAutoHomeBorderData::GetHomeBottomPoints(int r, int c, float* pPoints)
{
	int nCrossIndex = (r + 1) * (NUM_BORDER_BLOCK + 1) + c;
	pPoints[0] = m_pPointCross[nCrossIndex];
	pPoints[NUM_HOME_BORDER_POINT-1] = m_pPointCross[nCrossIndex+1];

	int nRowIndex = ((r + 1) * NUM_BORDER_BLOCK + c) * (NUM_HOME_BORDER_POINT - 2);
	for (int i=0; i<NUM_HOME_BORDER_POINT-2; i++)
	{
		pPoints[i+1] = m_pPointRow[nRowIndex+i];
	}
	return true;
}

// Get home left points
bool CAutoHomeBorderData::GetHomeLeftPoints(int r, int c, float* pPoints)
{
	int nCrossIndex = r * (NUM_BORDER_BLOCK + 1) + c;
	pPoints[0] = m_pPointCross[nCrossIndex];
	pPoints[NUM_HOME_BORDER_POINT-1] = m_pPointCross[nCrossIndex+NUM_BORDER_BLOCK];

	int nColIndex = (c * NUM_BORDER_BLOCK + r) * (NUM_HOME_BORDER_POINT - 2);
	for (int i=0; i<NUM_HOME_BORDER_POINT-2; i++)
	{
		pPoints[i+1] = m_pPointCol[nColIndex+i];
	}

	return true;
}

// Get home right points
bool CAutoHomeBorderData::GetHomeRightPoints(int r, int c, float* pPoints)
{
	int nCrossIndex = (r + 1) * (NUM_BORDER_BLOCK + 1) + c;
	pPoints[0] = m_pPointCross[nCrossIndex];
	pPoints[NUM_HOME_BORDER_POINT-1] = m_pPointCross[nCrossIndex+NUM_BORDER_BLOCK];

	int nColIndex = ((c + 1) * NUM_BORDER_BLOCK + r) * (NUM_HOME_BORDER_POINT - 2);
	for (int i=0; i<NUM_HOME_BORDER_POINT-2; i++)
	{
		pPoints[i+1] = m_pPointCol[nColIndex+i];
	}

	return true;
}

void CAutoHomeBorderData::SetPointCrossByIndex(int nIndex, float fValue)	
{ 
	m_pPointCross[nIndex] = fValue; 
	m_bEdited = true;
}

void CAutoHomeBorderData::SetPointRowByIndex(int nIndex, float fValue)	
{ 
	m_pPointRow[nIndex] = fValue; 
	m_bEdited = true;
}

void CAutoHomeBorderData::SetPointColByIndex(int nIndex, float fValue)	
{ 
	m_pPointCol[nIndex] = fValue; 
	m_bEdited = true;
}
