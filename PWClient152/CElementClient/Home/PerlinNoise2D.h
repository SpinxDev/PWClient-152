/*
 * FILE: PerlinNoise2D.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Jiang Dalong, 2005/1/12
 *
 * HISTORY: 
 *
 * Copyright (c) 2001-2008 Archosaur Studio, All Rights Reserved.
 */

#ifndef _PERLINNOISE2D_H_
#define _PERLINNOISE2D_H_

#pragma once

//#define MRANDOM (float)(rand() & (RAND_MAX))/(RAND_MAX)
#define MRANDOM (((float)rand())/(RAND_MAX))
#define SRANDOM (((MRANDOM) * 2) - 1)

class CPerlinNoise2D  
{
public:
	CPerlinNoise2D();
	virtual ~CPerlinNoise2D();

public:
	// Initialize noise
	bool Init(int nWidth, int nHeight, float fAmp, float fFre, float fPer, int nPass, int seed, bool bUpZero=true);
	// Get a point's value
	float GetValue(int x, int y);
	// Get all noise value
	bool CopyAllNoise(float* pDst);
	// Get max value
	float GetMaxValue();
	// Get min value
	float GetMinValue();

	inline float* GetBuffer() { return m_pNoise; }


protected:
	float*		m_pNoise;
	int			m_nWidth;
	int			m_nHeight;
	float		m_fAmplitude;
	float		m_fFrequency;
	float		m_fPersistence;
	int			m_nPass;
	int			m_nSeed;
	bool		m_bUpZero;			// true:[0, amp]; false:[-amp, amp]

	float		m_fMaxValue;
	float		m_fMinValue;
	bool		m_bHasGetMaxMin;

protected:
	float Spline( float x, int nknots, float *knot );
	void FracSynthPass( float *hbuf, float freq, float zscale, int xres, int zres );
	void Release();
	void GetMaxAndMinValue();
};

#endif // #ifndef _PERLINNOISE2D_H_
