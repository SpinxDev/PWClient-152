/*
 * FILE: APerlinNoiseBase.h
 *
 * DESCRIPTION: class for generating 1D, 2D, and 3D PerlinNoise
 *				This class is the base class for all kinds of perlin noise
 *
 * CREATED BY: Hedi, 2002/9/13
 *
 * HISTORY:
 *
 * Copyright (c) 2001~2008 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _APERLINNOISEBASE_H_
#define _APERLINNOISEBASE_H_

#include "ACPlatform.h"

#define MAX_OCTAVE				16
#define LERP(s, a, b) ((a) * (1.0f - (s)) + (b) * (s))
#define S_CURVE(t) ((t) * (t) * (3.0f - 2.0f * (t)))

class APerlinNoiseBase
{
protected:
	typedef struct _NOISEVALUE
	{
		float	v[3];
	} NOISEVALUE;

	bool			m_bTurbulence;		//flag indicates whether use sum of fractal fabs value or not
	DWORD			m_dwSeed;
	int				m_nBaseWaveLength;
	float			m_vBaseAmplitude;
	float			m_vPersistence;

	int				m_nActiveOctave;
	int				m_nOctaveNum;
	int				m_nStartPos[MAX_OCTAVE];
	int				m_nWaveLength[MAX_OCTAVE];
	float			m_vAmplitude[MAX_OCTAVE];

protected:
	DWORD	RandInteger();
	float	RandFloat();

	APerlinNoiseBase();
	~APerlinNoiseBase();

	bool InitParams(float vAmplitude, int nWaveLength, float vPersistence, int nOctaveNum);

public:
	inline void SetTurbulence(bool bTurbulence) { m_bTurbulence = bTurbulence; }
	inline bool GetTurbulence() { return m_bTurbulence; }
	inline void SetActiveOctave(int nIndex)		{ m_nActiveOctave = nIndex; }
	inline FLOAT GetBaseAmplitude()				{ return m_vBaseAmplitude; }
};

#endif//_APERLINNOISEBASE_H_