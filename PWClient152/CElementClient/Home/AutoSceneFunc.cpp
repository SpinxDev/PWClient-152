/*
 * FILE: AutoSceneFunc.cpp
 *
 * DESCRIPTION: Some base functions for terrain
 *
 * CREATED BY: Jiang Dalong, 2005/03/25
 *
 * HISTORY: 
 *
 * Copyright (c) 2001-2008 Archosaur Studio, All Rights Reserved.
 */

#include "AutoSceneFunc.h"
#include <AAssist.h>
#include <math.h>
#include <A3DVector.h>

float Distance(POINT_FLOAT pt1, POINT_FLOAT pt2)
{
	return (float)sqrt((pt1.x-pt2.x)*(pt1.x-pt2.x)+(pt1.y-pt2.y)*(pt1.y-pt2.y));
}

// Shortest distance between a point and a part line
float DistanceToPartLine(POINT_FLOAT pt, POINT_FLOAT ptLine1, POINT_FLOAT ptLine2, POINT_FLOAT* ptCross, int* pType)
{
	double k1, k2;// slope rate
	double fDis;
	int nSwap = 0;

	// The line is a point
	if (fabs(ptLine1.x-ptLine2.x)<FLOAT_ZERO && fabs(ptLine1.y-ptLine2.y)<FLOAT_ZERO)
	{
		fDis = Distance(pt, ptLine1);
		ptCross->x = ptLine1.x;
		ptCross->y = ptLine1.y;
		*pType = 1;
	}
	// The line is verticle
	else if (fabs(ptLine1.x-ptLine2.x)<FLOAT_ZERO)
	{
		if (ptLine1.y > ptLine2.y)
		{
			a_Swap(ptLine1.x, ptLine2.x);
			a_Swap(ptLine1.y, ptLine2.y);
			nSwap = 1;
		}
		if (pt.y < ptLine1.y) 
		{
			fDis = Distance(pt, ptLine1);
			ptCross->x = ptLine1.x;
			ptCross->y = ptLine1.y;
			*pType = 1 + nSwap;
		}
		else if (pt.y > ptLine2.y)
		{
			fDis = Distance(pt, ptLine2);
			ptCross->x = ptLine2.x;
			ptCross->y = ptLine2.y;
			*pType = 2 - nSwap;
		}
		else
		{
			fDis = fabs(pt.x - ptLine1.x);
			ptCross->x = ptLine1.x;
			ptCross->y = ptLine1.y;
			*pType = 0;
		}
	}
	// The line is horizontal
	else if (fabs(ptLine1.y-ptLine2.y)<FLOAT_ZERO)
	{
		if (ptLine1.x > ptLine2.x)
		{
			a_Swap(ptLine1.x, ptLine2.x);
			a_Swap(ptLine1.y, ptLine2.y);
			nSwap = 1;
		}
		if (pt.x < ptLine1.x) 
		{
			fDis = Distance(pt, ptLine1);
			ptCross->x = ptLine1.x;
			ptCross->y = ptLine1.y;
			*pType = 1 + nSwap;
		}
		else if (pt.x > ptLine2.x)
		{
			fDis = Distance(pt, ptLine2);
			ptCross->x = ptLine2.x;
			ptCross->y = ptLine2.y;
			*pType = 2 - nSwap;
		}
		else
		{
			fDis = fabs(pt.y - ptLine1.y);
			ptCross->x = ptLine1.x;
			ptCross->y = ptLine1.y;
			*pType = 0;
		}
	}
	// The line is gradient
	else 
	{
		double dCrossX;
		if (ptLine1.x > ptLine2.x)
		{
			a_Swap(ptLine1.x, ptLine2.x);
			a_Swap(ptLine1.y, ptLine2.y);
			nSwap = 1;
		}
		k1 = (ptLine1.y-ptLine2.y) / (ptLine1.x-ptLine2.x);
		k2 = -1 / k1;
		dCrossX = (k1*ptLine1.x-k2*pt.x-ptLine1.y+pt.y) / (k1-k2);
		if (dCrossX < ptLine1.x)
		{
			fDis = Distance(pt, ptLine1);
			ptCross->x = ptLine1.x;
			ptCross->y = ptLine1.y;
			*pType = 1 + nSwap;
		}
		else if (dCrossX > ptLine2.x)
		{
			fDis = Distance(pt, ptLine2);
			ptCross->x = ptLine2.x;
			ptCross->y = ptLine2.y;
			*pType = 2 - nSwap;
		}
		else
		{
			ptCross->y = float(dCrossX*k1 - ptLine1.x*k1 + ptLine1.y);
			ptCross->x = float(dCrossX);
			fDis = Distance(pt, *ptCross);
			*pType = 0;
		}
	}

	return (float)fDis;
}

float LineSlope(POINT_FLOAT pt1, POINT_FLOAT pt2)
{
	if (fabs(pt1.x - pt2.x) < 0.0001f)
		return 99999.0f;

	return (pt1.y - pt2.y) / (pt1.x - pt2.x);
}

// Which side of the line does a point lie
float AtLineSide(POINT_FLOAT pt, POINT_FLOAT ptLine1, POINT_FLOAT ptLine2)
{
	float fSlope = LineSlope(ptLine1, ptLine2);
	float fSide = pt.y - ptLine1.y - fSlope * (pt.x - ptLine1.x);
	
	return fSide;
}

// Law of cosine
float CosByLawOfCosine(POINT_FLOAT pt, POINT_FLOAT ptLine1, POINT_FLOAT ptLine2)
{
	float a, b, c;
	a = Distance(pt, ptLine1);
	b = Distance(pt, ptLine2);
	c = Distance(ptLine1, ptLine2);

	float fCos = (a * a + b * b - c * c) / a / b / 2;

	return fCos;
}

// Distance between 2 3D points
float Distance3D(A3DVECTOR3& v1, A3DVECTOR3& v2)
{
	return (float)sqrt(
		(v1.x-v2.x)*(v1.x-v2.x)
		+(v1.y-v2.y)*(v1.y-v2.y)
		+(v1.z-v2.z)*(v1.z-v2.z)
		);
}

POINT_FLOAT PointAtLine(POINT_FLOAT pt1, POINT_FLOAT pt2, float fDis)
{
	POINT_FLOAT point;
	float fX;
	if (fabs(pt1.x-pt2.x)<0.0001f)
	{
		point.x = pt1.x;
		if (pt1.y>pt2.y)
			point.y = pt1.y - fDis;
		else
			point.y = pt1.y + fDis;
	}
	else
	{
		float fSlope = LineSlope(pt1, pt2);
		float fTemp = (float)(fDis * sqrt(1/(1+fSlope*fSlope)));
		if (pt1.x > pt2.x)
			fX = pt1.x - fTemp;
		else
			fX = pt1.x + fTemp;
		point.y = fSlope * (fX - pt1.x) + pt1.y;
		point.x = fX;
	}

	return point;
}

float _getAngle(float X,float Y)
{
	if(Y==0) return 0;
	
	if(X==0)
	{
		if(Y>0) return 90.0f;
		if(Y<0) return 270.0f;
	}
	
	float temp = float(atan(Y/X) * 180 / 3.1415927);
	if(X>0 && Y>0) return temp;
	else if( X>0 && Y<0) return 360 + temp;
	else if( X<0 && Y>0) return 180 + temp;
	else if( X<0 && Y<0) return 180 + temp;
	else return 0;
}

// Calculate a point's position weight on a line
float WeightOnLine(POINT_FLOAT pt, POINT_FLOAT pt1, POINT_FLOAT pt2)
{
	float fWeight = 0.0f;
	if (fabs(pt1.x - pt2.x) < FLOAT_ZERO)
	{
		if (fabs(pt1.y - pt2.y) < FLOAT_ZERO)
		{
			fWeight = 0.0f;
		}
		else
		{
			fWeight = (pt.y - pt1.y) / (pt2.y - pt1.y);
		}
	}
	else
	{
		fWeight = (pt.x - pt1.x) / (pt2.x - pt1.x);
	}

	return fWeight;
}

// Whether 2 lines are crossing
bool IsTwoLineCross(POINT_FLOAT ptStart1, POINT_FLOAT ptEnd1, POINT_FLOAT ptStart2, POINT_FLOAT ptEnd2)
{
	bool bCross;
	POINT_FLOAT ptCross;
	
	float k1, k2;// Slope rate
	// Both lines are vertical
	if (fabs(ptStart1.x-ptEnd1.x)<FLOAT_ZERO && fabs(ptStart2.x-ptEnd2.x)<FLOAT_ZERO)
		bCross = false;
	// Line1 is vertical
	else if (fabs(ptStart1.x-ptEnd1.x)<FLOAT_ZERO)
	{
		k2 = (ptStart2.y-ptEnd2.y) / (ptStart2.x-ptEnd2.x);
		ptCross.x = ptStart1.x;
		ptCross.y = k2*ptCross.x - k2*ptStart2.x + ptStart2.y;
		if (((ptCross.y>a_Min(ptStart1.y, ptEnd1.y)) || (fabs(ptCross.y-a_Min(ptStart1.y, ptEnd1.y))<FLOAT_ZERO))
			&& ((ptCross.y<a_Max(ptStart1.y, ptEnd1.y)) || (fabs(ptCross.y-a_Max(ptStart1.y, ptEnd1.y))<FLOAT_ZERO))
			&& ((ptCross.x>a_Min(ptStart2.x, ptEnd2.x)) || (fabs(ptCross.x-a_Min(ptStart2.x, ptEnd2.x))<FLOAT_ZERO))
			&& ((ptCross.x<a_Max(ptStart2.x, ptEnd2.x)) || (fabs(ptCross.x-a_Max(ptStart2.x, ptEnd2.x))<FLOAT_ZERO)))
			bCross = true; 
		else
			bCross = false;
	}
	// Line2 is vertical
	else if (fabs(ptStart2.x-ptEnd2.x)<FLOAT_ZERO)
	{
		k1 = (ptStart1.y-ptEnd1.y) / (ptStart1.x-ptEnd1.x);
		ptCross.x = ptStart2.x;
		ptCross.y = k1*ptCross.x - k1*ptStart1.x + ptStart1.y;
		if (((ptCross.x>a_Min(ptStart1.x, ptEnd1.x)) || (fabs(ptCross.x-a_Min(ptStart1.x, ptEnd1.x))<FLOAT_ZERO))
			&& ((ptCross.x<a_Max(ptStart1.x, ptEnd1.x)) || (fabs(ptCross.x-a_Max(ptStart1.x, ptEnd1.x))<FLOAT_ZERO))
			&& ((ptCross.y>a_Min(ptStart2.y, ptEnd2.y)) || (fabs(ptCross.y-a_Min(ptStart2.y, ptEnd2.y))<FLOAT_ZERO))
			&& ((ptCross.y<a_Max(ptStart2.y, ptEnd2.y)) || (fabs(ptCross.y-a_Max(ptStart2.y, ptEnd2.y))<FLOAT_ZERO)))
			bCross = true; 
		else
			bCross = false;
	}
	// Both lines are gradient 
	else
	{
		k1 = (ptStart1.y-ptEnd1.y) / (ptStart1.x-ptEnd1.x);
		k2 = (ptStart2.y-ptEnd2.y) / (ptStart2.x-ptEnd2.x);

		if (fabs(k1 - k2) < FLOAT_ZERO)
			bCross = false;
		else
		{
			ptCross.x = (k1*ptStart1.x-k2*ptStart2.x-ptStart1.y+ptStart2.y) / (k1-k2);
			ptCross.y = k1*ptCross.x - k1*ptStart1.x + ptStart1.y;
			if (((ptCross.x>a_Min(ptStart1.x, ptEnd1.x)) || (fabs(ptCross.x-a_Min(ptStart1.x, ptEnd1.x))<FLOAT_ZERO))
				&& ((ptCross.x<a_Max(ptStart1.x, ptEnd1.x)) || (fabs(ptCross.x-a_Max(ptStart1.x, ptEnd1.x))<FLOAT_ZERO))
				&& ((ptCross.x>a_Min(ptStart2.x, ptEnd2.x)) || (fabs(ptCross.x-a_Min(ptStart2.x, ptEnd2.x))<FLOAT_ZERO))
				&& ((ptCross.x<a_Max(ptStart2.x, ptEnd2.x)) || (fabs(ptCross.x-a_Max(ptStart2.x, ptEnd2.x))<FLOAT_ZERO)))
				bCross = true; 
			else
				bCross = false;
		}
	}

	return bCross;
}

// Change line weight to cos weight
float CosWeight(float fWeight)
{
	return float(0.5f - cos(fWeight * 3.1415927) * 0.5f);
}
