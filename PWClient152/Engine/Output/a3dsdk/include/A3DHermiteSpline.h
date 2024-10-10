/*
* FILE: A3DHermiteSpline.h
*
* DESCRIPTION: Hermite spline could generate a Catmull-Rom class spline from a serial of control points
*
* CREATED BY: Zhangyachuan, 2010/04/22
*
* HISTORY: 
*
* Copyright (c) 2009 Archosaur Studio, All Rights Reserved.
*/

#ifndef _A3DHermiteSpline_H_
#define _A3DHermiteSpline_H_

#include "ABaseDef.h"
#include "A3DVector.h"
#include "A3DMatrix.h"
#include "vector.h"

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class A3DHermiteSpline
//	
///////////////////////////////////////////////////////////////////////////

class A3DHermiteSpline
{
 
public:		//	Types

public:		//	Constructor and Destructor

	A3DHermiteSpline(void);
	virtual ~A3DHermiteSpline(void);

public:		//	Attributes

public:		//	Operations

	//	Add one point to the end of spline
	void AddPoint(const A3DVECTOR3& vPoint);
	
    //	Add one point to the end of spline, with specified tangent
    void AddPointAndTangent(const A3DVECTOR3& vPoint, const A3DVECTOR3& vTangent);

    // Get one of the control points of the spline
	const A3DVECTOR3& GetPoint(unsigned short nIndex) const;
	
	// Get the number of control points in the spline
	unsigned short GetNumPoints(void) const;
	
	// Clears all the points in the spline
	void Clear(void);

	// Erase some points and recalculate
	void Erase(unsigned short nStart, unsigned short nEnd);
	
	//	Updates a single point in the spline. 
	void UpdatePoint(unsigned short nIndex, const A3DVECTOR3& vPoint);
    void UpdateTangent(unsigned short nIndex, const A3DVECTOR3& vTangent);
	
	//	Returns an interpolated point based on a parametric value over the whole series.
	//	Given a t value between 0 and 1 representing the parametric distance along the whole length of the spline
	A3DVECTOR3 Interpolate(FLOAT t);
	
	//	Interpolates a single segment of the spline given a parametric value.
	//	Point[nIndex] is treated as t = 0, Point[nIndex + 1] is treated as t = 1
	A3DVECTOR3 Interpolate(unsigned int nIndex, FLOAT t);
	
	//	Tells the spline whether it should automatically calculate tangents on demand as points are added.
	void SetAutoCalculate(bool bAutoCalc);
	bool GetAutoCalculate() const { return m_bAutoCalc; }	
	
	//	Recalculates the tangents associated with this spline. 
	virtual void RecalcTangents(void);

protected:	//	Attributes

	bool m_bAutoCalc;						// Is auto calculate tangents once a point be added
	
	abase::vector<A3DVECTOR3> m_aPoints;
	abase::vector<A3DVECTOR3> m_aTangents;

	static const A3DMATRIX4 m_CoEffs;

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////


#endif	//	_A3DHermiteSpline_H_


