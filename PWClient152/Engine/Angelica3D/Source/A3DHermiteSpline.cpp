/*
* FILE: A3DHermiteSpline.cpp
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan, 2010/04/22
*
* HISTORY: 
*
* Copyright (c) 2009 Archosaur Studio, All Rights Reserved.
*/

#include "A3DHermiteSpline.h"

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Reference to External variables and functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local Types and Variables and Global variables
//	
///////////////////////////////////////////////////////////////////////////

// Hermite polynomial
const A3DMATRIX4 A3DHermiteSpline::m_CoEffs
( 2, -2,  1,  1,
 -3,  3, -2, -1,
  0,  0,  1,  0,
  1,  0,  0,  0);

///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Implement A3DHermiteSpline
//	
///////////////////////////////////////////////////////////////////////////

A3DHermiteSpline::A3DHermiteSpline(void)
: m_bAutoCalc(true)
{
	m_aPoints.reserve(50);
	m_aTangents.reserve(50);
}

A3DHermiteSpline::~A3DHermiteSpline(void)
{
}

//	Add one point to the end of spline
void A3DHermiteSpline::AddPoint(const A3DVECTOR3& vPoint)
{
	m_aPoints.push_back(vPoint);
	if (m_bAutoCalc)
	{
		RecalcTangents();
	}
}

//	Add one point to the end of spline, with specified tangent
void A3DHermiteSpline::AddPointAndTangent(const A3DVECTOR3& vPoint, const A3DVECTOR3& vTangent)
{
    assert(m_aPoints.size() == m_aTangents.size());
    m_aPoints.push_back(vPoint);
    m_aTangents.push_back(vTangent);
}

//	Returns an interpolated point based on a parametric value over the whole series.
//	Given a t value between 0 and 1 representing the parametric distance along the whole length of the spline
A3DVECTOR3 A3DHermiteSpline::Interpolate(FLOAT t)
{
	// Currently assumes points are evenly spaced, will cause velocity
	// change where this is not the case
	// TODO: base on arclength?
	
	
	// Work out which segment this is in
	FLOAT fSeg = t * (m_aPoints.size() - 1);
	unsigned int segIdx = (unsigned int)fSeg;
	// Apportion t 
	t = fSeg - segIdx;
	
	return Interpolate(segIdx, t);
}

//	Interpolates a single segment of the spline given a parametric value.
//	Point[nIndex] is treated as t = 0, Point[nIndex + 1] is treated as t = 1
A3DVECTOR3 A3DHermiteSpline::Interpolate(unsigned int nIndex, FLOAT t)
{
	// Bounds check
	assert (nIndex < m_aPoints.size() &&
		"nIndex out of bounds");
	
	if ((nIndex + 1) == m_aPoints.size())
	{
		// Duff request, cannot blend to nothing
		// Just return source
		return m_aPoints[nIndex];
		
	}
	
	// Fast special cases
	if (t == 0.0f)
	{
		return m_aPoints[nIndex];
	}
	else if(t == 1.0f)
	{
		return m_aPoints[nIndex + 1];
	}
	
	// FLOAT interpolation
	// Form a vector of powers of t
	FLOAT t2, t3;
	t2 = t * t;
	t3 = t2 * t;
	A3DVECTOR4 powers(t3, t2, t, 1);
	
	
	// Algorithm is ret = powers * mCoeffs * A3DMATRIX4(point1, point2, tangent1, tangent2)
	A3DVECTOR3& point1 = m_aPoints[nIndex];
	A3DVECTOR3& point2 = m_aPoints[nIndex+1];
	A3DVECTOR3& tan1 = m_aTangents[nIndex];
	A3DVECTOR3& tan2 = m_aTangents[nIndex+1];
	A3DMATRIX4 pt(point1.x, point1.y, point1.z, 1.0f
				, point2.x, point2.y, point2.z, 1.0f
				, tan1.x, tan1.y, tan1.z, 1.0f
				, tan2.x, tan2.y, tan2.z, 1.0f);
	
	A3DVECTOR4 ret = pt * (m_CoEffs * powers);
	return A3DVECTOR3(ret.x, ret.y, ret.z);
}

//	Recalculates the tangents associated with this spline. 
void A3DHermiteSpline::RecalcTangents(void)
{
	// Catmull-Rom approach
	// 
	// tangent[i] = 0.5 * (point[i+1] - point[i-1])
	//
	// Assume endpoint tangents are parallel with line with neighbour
	
	size_t i, numPoints;
	bool isClosed;
	
	numPoints = m_aPoints.size();
	if (numPoints < 2)
	{
		// Can't do anything yet
		return;
	}
	
	// Closed or open?
	if (m_aPoints[0] == m_aPoints[numPoints-1])
	{
		isClosed = true;
	}
	else
	{
		isClosed = false;
	}
	
	if (numPoints > m_aTangents.size())
		m_aTangents.insert(m_aTangents.end(), numPoints - m_aTangents.size(), A3DVECTOR3(0.f));
	
	for(i = 0; i < numPoints; ++i)
	{
		if (i ==0)
		{
			// Special case start
			if (isClosed)
			{
				// Use numPoints-2 since numPoints-1 is the last point and == [0]
				m_aTangents[i] = 0.5f * (m_aPoints[1] - m_aPoints[numPoints-2]);
			}
			else
			{
				m_aTangents[i] = 0.5f * (m_aPoints[1] - m_aPoints[0]);
			}
		}
		else if (i == numPoints - 1)
		{
			// Special case end
			if (isClosed)
			{
				// Use same tangent as already calculated for [0]
				m_aTangents[i] = m_aTangents[0];
			}
			else
			{
				m_aTangents[i] = 0.5f * (m_aPoints[i] - m_aPoints[i-1]);
			}
		}
		else
		{
			m_aTangents[i] = 0.5f * (m_aPoints[i+1] - m_aPoints[i-1]);
		}
	}
}

// Get one of the control points of the spline
const A3DVECTOR3& A3DHermiteSpline::GetPoint(unsigned short nIndex) const
{
	assert (nIndex < m_aPoints.size() && "Point index is out of bounds!!");
	
	return m_aPoints[nIndex];
}

// Get the number of control points in the spline
unsigned short A3DHermiteSpline::GetNumPoints(void) const
{
	return (unsigned short)m_aPoints.size();
}

// Clears all the points in the spline
void A3DHermiteSpline::Clear(void)
{
	m_aPoints.clear();
	m_aTangents.clear();
}

// Erase some points and recalculate, erase range from [nStart, nEnd)
void A3DHermiteSpline::Erase(unsigned short nStart, unsigned short nEnd)
{
	assert (nStart < m_aPoints.size() && nEnd <= m_aPoints.size());
	m_aPoints.erase(m_aPoints.begin() + nStart, m_aPoints.begin() + nEnd);
	if (m_bAutoCalc)
		RecalcTangents();
    else if (nStart < m_aTangents.size() && nEnd <= m_aTangents.size())
        m_aTangents.erase(m_aTangents.begin() + nStart, m_aTangents.begin() + nEnd);
}

//	Updates a single point in the spline. 
void A3DHermiteSpline::UpdatePoint(unsigned short nIndex, const A3DVECTOR3& vPoint)
{
	assert (nIndex < m_aPoints.size() && "Point index is out of bounds!!");
	
	m_aPoints[nIndex] = vPoint;
	if (m_bAutoCalc)
	{
		RecalcTangents();
	}
}

void A3DHermiteSpline::UpdateTangent(unsigned short nIndex, const A3DVECTOR3& vTangent)
{
    assert (nIndex < m_aTangents.size() && "Point index is out of bounds!!");

    m_aTangents[nIndex] = vTangent;
}

//	Tells the spline whether it should automatically calculate tangents on demand as points are added.
void A3DHermiteSpline::SetAutoCalculate(bool bAutoCalc)
{
	m_bAutoCalc = bAutoCalc;
}