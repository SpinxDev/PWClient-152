#include "A3DVector.h"

#pragma once

#define AXIS_X	0x1
#define AXIS_Y	0x2
#define AXIS_Z	0x4
#define PI 3.1415926

bool GenerateMask(int N, float* pHeightMap, A3DVECTOR3* aNormals, float fCellSize,
				  float fAltitudeStart, float fAltitudeEnd,
				  float fSlopeStart, float fSlopeEnd,
				  int axisUsed, int M, BYTE* pMaskMap,
				  float fSmoothRange);

void GaussianBlur(BYTE * pImage, int nSize, double vBlurRange);