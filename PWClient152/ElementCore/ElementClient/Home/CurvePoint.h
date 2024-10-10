/*
 * FILE: CurvePoint.h
 *
 * DESCRIPTION: Class for handle 2D curve points
 *
 * CREATED BY: Jiang Dalong, 2005/09/08
 *
 * HISTORY: 
 *
 * Copyright (c) 2001-2008 Archosaur Studio, All Rights Reserved.
 */

#ifndef _CURVEPOINT_H_
#define _CURVEPOINT_H_

class CCurvePoint  
{
public:
	CCurvePoint();
	virtual ~CCurvePoint();

	void Release();
	// Set curve points
	bool SetPointValues(int nNumPoints, float* pValues);
	// Get value at a position
	float GetValue(float fPos);

	inline int GetNumPoints() { return m_nNumPoints; }
	inline float* GetPointValues() { return m_pPointValues; }

protected:
	int				m_nNumPoints;
	float*			m_pPointValues;

protected:
	// Get value by interpolation
	float Catmullrom(float dt, float p0, float p1, float p2, float p3);

};

#endif // #ifndef _CURVEPOINT_H_
