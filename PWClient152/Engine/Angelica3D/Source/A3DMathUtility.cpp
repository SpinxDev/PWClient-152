/*
 * FILE: A3DMathUtil.cpp
 *
 * DESCRIPTION: The math utility class for Angelica 3D Engine, which using table searching for
				many math calculations;
 *
 * CREATED BY: Hedi, 2001/12/3
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "A3DMathUtility.h"
#include "A3DPI.h"
#include "A3DMacros.h"
#include "A3DFuncs.h"
#include "AAssist.h"

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

#define LUM_R	0.3086f
#define LUM_G	0.6094f
#define LUM_B	0.0820f

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

A3DMathUtility g_A3DMathUtility;
A3DMathUtility* g_pA3DMathUtility = &g_A3DMathUtility;
///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Implement A3DWaveFunc
//	
///////////////////////////////////////////////////////////////////////////

A3DWaveFunc::A3DWaveFunc()
{
	iFunc		= WAVE_NONE;
	fBase		= 0.0f;
	fAmp		= 1.0f;
	fPhase		= 0.0f;
	dwPeriod	= 1000;

	m_fValue	= 0.0f;
	m_dwTimeCnt	= 0;
}

//	Tick routine
void A3DWaveFunc::Tick(DWORD dwDeltaTime)
{
	if (!dwPeriod)
		return;

	m_dwTimeCnt += dwDeltaTime;
	m_dwTimeCnt %= dwPeriod;

	float x, y;
	x  = (float)m_dwTimeCnt / dwPeriod + fPhase;
	x -= (float)floor(x);		//	Normalize x to 0.0f -- 1.0f

	switch (iFunc)
	{
	case WAVE_SIN:

		y = g_A3DMathUtility.SIN(x * 360.0f);
		break;

	case WAVE_TRIANGLE:

		y = (x <= 0.5f) ? x * 2.0f : (1.0f - x) * 2.0f;
		y -= 1.0f;	//	[0, 2] to [-1, 1]
		break;

	case WAVE_SQUARE:

		y = (x <= 0.5f) ? 1.0f : -1.0f;
		break;

	case WAVE_INVSAWTOOTH:

		y = (1.0f - x) * 2.0f - 1.0f;
		break;

	case WAVE_SAWTOOTH:
	default:

		y = x * 2.0f - 1.0f;
		break;
	}

	m_fValue = y * fAmp + fBase;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement A3DMathUtility
//	
///////////////////////////////////////////////////////////////////////////

A3DMathUtility::A3DMathUtility() :
m_matPreHue(A3DMATRIX4::IDENTITY),
m_matPostHue(A3DMATRIX4::IDENTITY)
{
	m_vPrecision = 360.0f / TABLE_LENGTH;

	for(int i=0; i< TABLE_LENGTH; i++)
	{
		FLOAT vDegree = i * m_vPrecision;
		m_SinTable[i] = (FLOAT)sin(DEG2RAD(vDegree));
		m_CosTable[i] = (FLOAT)cos(DEG2RAD(vDegree));
	}

	//	Init hue matrices
	InitHueMatrix();
}

A3DMathUtility::~A3DMathUtility()
{
}

void A3DMathUtility::InitHueMatrix()
{
	//	Build pre hue matrix
	const float greenRotation = 35.0f;

	RotateColor(m_matPreHue, 45.0f, 0);
	RotateColor(m_matPreHue, -greenRotation, 1);

	A3DVECTOR3 lum(LUM_R, LUM_G, LUM_B);
	lum = lum * m_matPreHue;
	float red = lum.x / lum.z;
	float green = lum.y / lum.z;

	ShearColor(m_matPreHue, 2, (int)red, (int)green);

	//	Build post hue matrix
	ShearColor(m_matPostHue, 2, -(int)red, -(int)green);
	RotateColor(m_matPostHue, greenRotation, 1);
	RotateColor(m_matPostHue, -45.0f, 0);
}

void A3DMathUtility::ShearColor(A3DMATRIX4& matrix, int whichColor, int color1, int color2)
{
	A3DMATRIX4 matShearBlue(A3DMATRIX4::IDENTITY);

	switch (whichColor)
	{
	case 0:	//	Remove red
		matShearBlue.m[1][0] = (float)color1;
		matShearBlue.m[2][0] = (float)color2;
		break;

	case 1:	//	Remove green
		matShearBlue.m[0][1] = (float)color1;
		matShearBlue.m[2][1] = (float)color2;
		break;

	case 2:	//	Remove blue
		matShearBlue.m[0][2] = (float)color1;
		matShearBlue.m[1][2] = (float)color2;
		break;

	default:
		matShearBlue.m[1][0] = (float)color1;
		matShearBlue.m[2][0] = (float)color2;
		break;
	}

	matrix = matrix * matShearBlue;
}

void A3DMathUtility::RotateColor(A3DMATRIX4& matrix, float fDegree, int axiIndex)
{
	float fRadian = DEG2RAD(fDegree);

	switch (axiIndex)
	{
	case 0:	//	x axis
		matrix = a3d_RotateX(matrix, fRadian);
		break;

	case 1:	//	y axis
		matrix = a3d_RotateY(matrix, fRadian);
		break;

	case 2:	//	z axis
		matrix = a3d_RotateZ(matrix, fRadian);
		break;

	default:
		matrix = a3d_RotateX(matrix, fRadian);
		break;
	}
}

//	Build color matrix used to do hue/saturation operation
//	iHue: [-180, 180]
//	iSaturation: [-100, 100]
//	bGray: true, convert to gray color
A3DMATRIX4 A3DMathUtility::BuildHueSaturationMat(int iHue, int iSaturation, bool bGray)
{
	a_Clamp(iHue, -180, 180);
	a_Clamp(iSaturation, -100, 100);

	A3DMATRIX4 colorMatrix(A3DMATRIX4::IDENTITY);

	if (bGray)
	{
		A3DMATRIX4 grayMatrix(A3DMATRIX4::IDENTITY);

		for (int i=0; i < 3; i++)
		{
			grayMatrix.m[0][i] = LUM_R;
			grayMatrix.m[1][i] = LUM_G;
			grayMatrix.m[2][i] = LUM_B;
		}

		colorMatrix = grayMatrix;
	}

	//	Saturation
	float fSat = (iSaturation + 100) / 100.0f;
	if (iSaturation > 0)
		fSat = fSat * fSat;

	float satCompl = 1.0f - fSat;
	float satComplR = LUM_R * satCompl;
	float satComplG = LUM_G * satCompl;
	float satComplB = LUM_B * satCompl;

	A3DMATRIX4 matSaturation(A3DMATRIX4::IDENTITY);

	matSaturation.m[0][0] = satComplR + fSat;
	matSaturation.m[0][1] = satComplR;
	matSaturation.m[0][2] = satComplR;
	matSaturation.m[1][0] = satComplG;
	matSaturation.m[1][1] = satComplG + fSat;
	matSaturation.m[1][2] = satComplG;
	matSaturation.m[2][0] = satComplB;
	matSaturation.m[2][1] = satComplB;
	matSaturation.m[2][2] = satComplB + fSat;

	colorMatrix = matSaturation * colorMatrix;

	//	Hue
	colorMatrix = colorMatrix * m_matPreHue;
	RotateColor(colorMatrix, (float)iHue, 2);
	colorMatrix = colorMatrix * m_matPostHue;

	return colorMatrix;
}


