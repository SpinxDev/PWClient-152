/*
 * FILE: A3DMathUtil.h
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

#ifndef _A3DMATHUTILITY_H_
#define _A3DMATHUTILITY_H_

#include "A3DTypes.h"
#include "A3DMatrix.h"

///////////////////////////////////////////////////////////////////////////
//
//	Class A3DShader
//
///////////////////////////////////////////////////////////////////////////

class A3DWaveFunc
{
public:		//	Types

	//	Wave function type
	enum
	{
		WAVE_NONE = 0,		//	None
		WAVE_SIN,			//	Sin wave
		WAVE_TRIANGLE,		//	Triangle wave
		WAVE_SQUARE,		//	Square wave
		WAVE_SAWTOOTH,		//	Sawtooth
		WAVE_INVSAWTOOTH,	//	Inverse sawtooth
	};

public:		//	Constructors and Destructors

	A3DWaveFunc();

public:		//	Attributes

	int		iFunc;		//	Wave function type
	float	fBase;		//	Wave base
	float	fAmp;		//	Amplitude
	float	fPhase;		//	Initial phase
	DWORD	dwPeriod;	//	Period (ms)

public:		//	Operations

	//	Tick routine
	void Tick(DWORD dwDeltaTime);
	//	Get current value
	float GetValue() { return m_fValue; }
	//	Reset value
	void ResetValue() { m_fValue = 0.0f; }

protected:	//	Attributes

	DWORD	m_dwTimeCnt;	//	Time counter
	float	m_fValue;		//	Current value

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//
//	Class A3DMathUtility
//
///////////////////////////////////////////////////////////////////////////

class A3DMathUtility
{
public:

	enum
	{
		TABLE_LENGTH = 3600,
	};

public:

	A3DMathUtility();
	~A3DMathUtility();

	inline FLOAT SIN(FLOAT vDegree)
	{
		int nTableIndex = (int)(vDegree / m_vPrecision);
		nTableIndex = nTableIndex % TABLE_LENGTH;
		if( nTableIndex < 0 ) nTableIndex += TABLE_LENGTH;

		return m_SinTable[nTableIndex];
	}

	inline FLOAT COS(FLOAT vDegree)
	{
		int nTableIndex = (int)(vDegree / m_vPrecision);
		nTableIndex = nTableIndex % TABLE_LENGTH;
		if( nTableIndex < 0 ) nTableIndex += TABLE_LENGTH;

		return m_CosTable[nTableIndex];
	}

	//	Build color matrix used to do hue/saturation operation
	//	iHue: [-180, 180]
	//	iSaturation: [-100, 100]
	//	bGray: true, convert to gray color
	A3DMATRIX4 BuildHueSaturationMat(int iHue, int iSaturation, bool bGray);

protected:

	//	The precision of sin and cos table, in degree defualt is 0.1;
	FLOAT	m_vPrecision;
	FLOAT	m_SinTable[TABLE_LENGTH];
	FLOAT	m_CosTable[TABLE_LENGTH];

	A3DMATRIX4	m_matPreHue;
	A3DMATRIX4	m_matPostHue;

protected:

	void InitHueMatrix();
	void ShearColor(A3DMATRIX4& matrix, int whichColor, int color1, int color2);
	void RotateColor(A3DMATRIX4& matrix, float fDegree, int axiIndex);
};

extern A3DMathUtility g_A3DMathUtility;
extern A3DMathUtility* g_pA3DMathUtility;

#endif//_A3DMATHUTIL_H_


