/*
 * FILE: APerlinNoise2D.cpp
 *
 * DESCRIPTION: class for generating continuous PerlinNoise on 2D space
 *				This class is used for generate 2D continuous perlin noise value only
 *
 * CREATED BY: Hedi, 2002/9/13
 *
 * HISTORY:
 *
 * Copyright (c) 2001~2008 Archosaur Studio, All Rights Reserved.	
 */

#include "APerlinNoise2D.h"
#include "AMemory.h"
#include <math.h>

APerlinNoise2D::APerlinNoise2D() 
{
	m_pValues		= NULL;
	m_nBufferWidth	= 0;
	m_nBufferHeight = 0;
}

APerlinNoise2D::~APerlinNoise2D()
{
	Release();
}

bool APerlinNoise2D::Init(int nBufferWidth, int nBufferHeight, float vAmplitude, int nWaveLength, float vPersistence, int nOctaveNum, DWORD dwRandSeed)
{
	// generate loop back smoothed random buffer
	if( nBufferWidth <= 0 || nBufferHeight <= 0 )
		return false;

	//	Release old resource if necessary
	if (m_nBufferWidth != nBufferWidth || m_nBufferHeight != nBufferHeight)
	{
		m_nBufferWidth = nBufferWidth;
		m_nBufferHeight = nBufferHeight;

		Release();

		m_pValues = (NOISEVALUE *) a_malloc(sizeof(NOISEVALUE) * m_nBufferWidth * m_nBufferHeight);
		if( NULL == m_pValues )
			return false;
	}


	int	i, x, y, k;

	m_dwSeed = dwRandSeed;

	float * pValues = (float *) a_malloctemp(sizeof(float) * m_nBufferWidth * m_nBufferHeight);
	if( NULL == pValues )
		return false;

	for(k=0; k<3; k++)
	{
		// First create random number buffer;
		for(i=0; i<m_nBufferWidth * m_nBufferHeight; i++)
			pValues[i] = RandFloat();
		
		// Now smooth the random number buffer;
		for(x=0; x<m_nBufferWidth; x++)
		{
			for(y=0; y<m_nBufferHeight; y++)
			{
				int n = y * m_nBufferWidth + x;

				int nLastX, nNextX, nLastY, nNextY;
				nLastX = x - 1;
				if( nLastX < 0 ) nLastX += m_nBufferWidth;
				nNextX = x + 1;
				if( nNextX >= m_nBufferWidth ) nNextX -= m_nBufferWidth;

				nLastY = y - 1;
				if( nLastY < 0 ) nLastY += m_nBufferHeight;
				nNextY = y + 1;
				if( nNextY >= m_nBufferHeight ) nNextY -= m_nBufferHeight;

#define NOISEVALUE(x, y) (pValues[(y) * m_nBufferWidth + (x)])
				m_pValues[n].v[k] = 
					1.0f / 16.0f * (NOISEVALUE(nLastX, nLastY) + NOISEVALUE(nLastX, nNextY) + NOISEVALUE(nNextX, nLastY) + NOISEVALUE(nNextX, nNextY)) + 
					1.0f / 8.0f * (NOISEVALUE(x, nLastY) + NOISEVALUE(x, nNextY) + NOISEVALUE(nLastX, y) + NOISEVALUE(nNextX, y)) + 
					1.0f / 4.0f * (NOISEVALUE(x, y));
			}
		}
	}

	a_freetemp(pValues);

	return InitParams(vAmplitude, nWaveLength, vPersistence, nOctaveNum);
}

bool APerlinNoise2D::Release()
{
	if( m_pValues )
	{
		a_free(m_pValues);
		m_pValues = NULL;
	}

	m_nBufferWidth = 0;
	m_nBufferHeight = 0;

	return true;
}

void APerlinNoise2D::GetValue(float x, float y, float * pvValue, int nNumValue)
{
	int			i, k;
	float		vx, sx, vy, sy;
	int			x1, x2, y1, y2;
	float		value1[3], value2[3];
	float		valueX1[3], valueX2[3];
	float		value[3];
	float		vFinal[3];

	ZeroMemory(vFinal, sizeof(float) * nNumValue);
	for(i=0; i<m_nOctaveNum; i++)
	{
		if( m_nActiveOctave != -1 && m_nActiveOctave != i )
			continue;

		// Get Horizon interpolated value;
		vx = m_nStartPos[i] % m_nBufferWidth + x / m_nWaveLength[i];
		x1 = int(vx);
		sx = vx - x1;
		sx = S_CURVE(sx);
		x2 = x1 + 1;

		vy = m_nStartPos[i] / m_nBufferWidth + y / m_nWaveLength[i];
		y1 = int(vy);
		sy = vy - y1;
		sy = S_CURVE(sy);
		y2 = y1 + 1;
		
		GetRandValues(x1, y1, value1, nNumValue);
		GetRandValues(x2, y1, value2, nNumValue);
		for(k=0; k<nNumValue; k++)
			valueX1[k] = LERP(sx, value1[k], value2[k]);

		GetRandValues(x1, y2, value1, nNumValue);
		GetRandValues(x2, y2, value2, nNumValue);
		for(k=0; k<nNumValue; k++)
			valueX2[k] = LERP(sx, value1[k], value2[k]);

		if( m_bTurbulence )
		{
			for(k=0; k<nNumValue; k++)
			{
				value[k] = (float)fabs(LERP(sy, valueX1[k], valueX2[k]));
				vFinal[k] += m_vAmplitude[i] * value[k];
			}
		}
		else
		{
			for(k=0; k<nNumValue; k++)
			{
				value[k] = LERP(sy, valueX1[k], valueX2[k]);
				vFinal[k] += m_vAmplitude[i] * value[k];
			}
		}
	}

	for(k=0; k<nNumValue; k++)
		pvValue[k] = vFinal[k];
}
