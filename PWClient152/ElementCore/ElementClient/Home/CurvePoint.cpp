/*
 * FILE: CurvePoint.cpp
 *
 * DESCRIPTION: Class for handle 2D curve points
 *
 * CREATED BY: Jiang Dalong, 2005/09/08
 *
 * HISTORY: 
 *
 * Copyright (c) 2001-2008 Archosaur Studio, All Rights Reserved.
 */

#include "CurvePoint.h"
#include "AutoSceneFunc.h"
#include <AAssist.h>
#include <A3DTypes.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCurvePoint::CCurvePoint()
{
	m_pPointValues = NULL;
}

CCurvePoint::~CCurvePoint()
{
	Release();
}

void CCurvePoint::Release()
{
	SAFERELEASE(m_pPointValues);
}

// Set curve points
bool CCurvePoint::SetPointValues(int nNumPoints, float* pValues)
{
	if (nNumPoints <= 0)
		return false;

	m_nNumPoints = nNumPoints;
	SAFERELEASE(m_pPointValues);
	m_pPointValues = new float[nNumPoints];
	if (NULL == m_pPointValues)
		return false;
	memcpy(m_pPointValues, pValues, nNumPoints*sizeof(float));

	return true;
}

// Get value at a position
float CCurvePoint::GetValue(float fPos)
{
	if (m_nNumPoints < 2)
		return 0.0f;

	a_Clamp(fPos, 0.0f, 1.0f);
	fPos *= (m_nNumPoints - 1);
	
	float p0, p1, p2, p3;
	int i1 = (int)floor(fPos);
	int i2 = i1 + 1;
	float dt = fPos - (float)(i1);
	
	p1 = m_pPointValues[i1];   
	p2 = m_pPointValues[i2];
	
	if (i1 == 0)
		p0 = p2;
	else
		p0 = m_pPointValues[i1-1]; 
	
	if (i2 == m_nNumPoints - 1)
		p3 = p1;
	else
		p3 = m_pPointValues[i2+1];
	
	float fResult = Catmullrom(dt, p0, p1, p2, p3);
//	a_Clamp(fResult, 0.0f, 1.0f);
	
	return fResult;
}

// Get value by interpolation
float CCurvePoint::Catmullrom(float dt, float p0, float p1, float p2, float p3)
{
   return 0.5f * ((3.0f*p1 - 3.0f*p2 + p3 - p0)*dt*dt*dt
                  +  (2.0f*p0 - 5.0f*p1 + 4.0f*p2 - p3)*dt*dt
                  +  (p2-p0)*dt
                  +  2.0f*p1);
}