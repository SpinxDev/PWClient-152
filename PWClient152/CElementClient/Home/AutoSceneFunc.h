/*
 * FILE: AutoSceneFunc.h
 *
 * DESCRIPTION: Some base functions for terrain
 *
 * CREATED BY: Jiang Dalong, 2005/03/25
 *
 * HISTORY: 
 *
 * Copyright (c) 2001-2008 Archosaur Studio, All Rights Reserved.
 */
#ifndef _BASEFUNC_H_
#define _BASEFUNC_H_

#include <stdio.h>

class A3DVECTOR3;

const float FLOAT_ZERO		= 0.001f;
typedef struct
{
	float x;
	float y;
}POINT_FLOAT;

// Distance between 2 points
float Distance(POINT_FLOAT pt1, POINT_FLOAT pt2);
// Shortest distance between a point and a part line. pType: 0: To edge; 1: To ptLine1; 2: To ptLine2
float DistanceToPartLine(POINT_FLOAT pt, POINT_FLOAT ptLine1, POINT_FLOAT ptLine2, POINT_FLOAT* ptCross, int* pType);
// Slope of a line
float LineSlope(POINT_FLOAT pt1, POINT_FLOAT pt2);
// Which side of the line does a point lie
float AtLineSide(POINT_FLOAT pt, POINT_FLOAT ptLine1, POINT_FLOAT ptLine2);
// Law of cosine
float CosByLawOfCosine(POINT_FLOAT pt, POINT_FLOAT ptLine1, POINT_FLOAT ptLine2);
// Distance between 2 3D points
float Distance3D(A3DVECTOR3& v1, A3DVECTOR3& v2);
// Find the point on a line by some distance from a start point
POINT_FLOAT PointAtLine(POINT_FLOAT pt1, POINT_FLOAT pt2, float fDis);
// Calculate a point's position weight on a line
float WeightOnLine(POINT_FLOAT pt, POINT_FLOAT pt1, POINT_FLOAT pt2);
// Whether 2 lines are crossing
bool IsTwoLineCross(POINT_FLOAT ptStart1, POINT_FLOAT ptEnd1, POINT_FLOAT ptStart2, POINT_FLOAT ptEnd2);

// Change line weight to cos weight
float CosWeight(float fWeight);

float _getAngle(float X,float Y);

#define SAFERELEASE(p) \
{ \
	if (NULL != p) \
	{ \
		delete[] p; \
		p = NULL; \
	} \
}

#endif // #ifndef _BASEFUNC_H_
