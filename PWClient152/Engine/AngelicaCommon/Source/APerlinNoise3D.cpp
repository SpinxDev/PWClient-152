/*
 * FILE: APerlinNoise3D.cpp
 *
 * DESCRIPTION: class for generating continuous PerlinNoise on 3D space
 *				This class is used for generate 3D continuous perlin noise value only
 *
 * CREATED BY: Hedi, 2002/9/13
 *
 * HISTORY:
 *
 * Copyright (c) 2001~2008 Archosaur Studio, All Rights Reserved.	
 */

#include "APerlinNoise3D.h"
#include "AMemory.h"
#include <math.h>

APerlinNoise3D::APerlinNoise3D() 
{
	m_pValues		= NULL;
	m_nBufferWidth	= 0;
	m_nBufferHeight = 0;
}

APerlinNoise3D::~APerlinNoise3D()
{
	Release();
}

bool APerlinNoise3D::Init(int nBufferWidth, int nBufferHeight, int nBufferDepth, float vAmplitude, int nWaveLength, float vPersistence, int nOctaveNum, DWORD dwRandSeed)
{
	// generate loop back smoothed random buffer
	if( nBufferWidth <= 0 || nBufferHeight <= 0 || nBufferDepth <= 0 )
		return false;

	//	Release old resource if necessary
	if (m_nBufferWidth != nBufferWidth || m_nBufferHeight != nBufferHeight || m_nBufferDepth != nBufferDepth)
	{
		m_nBufferWidth = nBufferWidth;
		m_nBufferHeight = nBufferHeight;
		m_nBufferDepth = nBufferDepth;

		Release();

		m_pValues = (NOISEVALUE *) a_malloc(sizeof(NOISEVALUE) * m_nBufferWidth * m_nBufferHeight * m_nBufferDepth);
		if( NULL == m_pValues )
			return false;
	}

	int	i, x, y, z, k;

	m_dwSeed = dwRandSeed;

	float * pValues = (float *) a_malloctemp(sizeof(float) * m_nBufferWidth * m_nBufferHeight * m_nBufferDepth);
	if( NULL == pValues )
		return false;

	for(k=0; k<3; k++)
	{
		// First create random number buffer;
		for(i=0; i<m_nBufferWidth * m_nBufferHeight * m_nBufferDepth; i++)
			pValues[i] = RandFloat();
		
		// Now smooth the random number buffer;
		for(x=0; x<m_nBufferWidth; x++)
		{
			for(y=0; y<m_nBufferHeight; y++)
			{
				for(z=0; z<m_nBufferDepth; z++)
				{
					int n = z * m_nBufferWidth * m_nBufferHeight + y * m_nBufferWidth + x;

					int nLastX, nNextX, nLastY, nNextY, nLastZ, nNextZ;
					nLastX = x - 1;
					if( nLastX < 0 ) nLastX += m_nBufferWidth;
					nNextX = x + 1;
					if( nNextX >= m_nBufferWidth ) nNextX -= m_nBufferWidth;

					nLastY = y - 1;
					if( nLastY < 0 ) nLastY += m_nBufferHeight;
					nNextY = y + 1;
					if( nNextY >= m_nBufferHeight ) nNextY -= m_nBufferHeight;

					nLastZ = z - 1;
					if( nLastZ < 0 ) nLastZ += m_nBufferDepth;
					nNextZ = z + 1;
					if( nNextZ >= m_nBufferDepth ) nNextZ -= m_nBufferDepth;

#define NOISEVALUE(x, y, z) (pValues[(z) * m_nBufferWidth * m_nBufferHeight + (y) * m_nBufferWidth + (x)])
					m_pValues[n].v[k] = 
						1.0f / 8.0f * (NOISEVALUE(x, y, nLastZ) + NOISEVALUE(x, y, nNextZ) + NOISEVALUE(x, nLastY, z) + NOISEVALUE(x, nNextY, z) + NOISEVALUE(nLastX, y, z) + NOISEVALUE(nNextX, y, z)) + 
						1.0f / 4.0f * (NOISEVALUE(x, y, z));
				}
			}
		}
	}

	a_freetemp(pValues);

	return InitParams(vAmplitude, nWaveLength, vPersistence, nOctaveNum);
}

bool APerlinNoise3D::Release()
{
	if( m_pValues )
	{
		a_free(m_pValues);
		m_pValues = NULL;
	}

	m_nBufferWidth = 0;
	m_nBufferHeight = 0;
	m_nBufferDepth = 0;

	return true;
}

void APerlinNoise3D::GetValue(float x, float y, float z, float * pvValue, int nNumValue)
{
	int			i, k;
	float		vx, sx, vy, sy, vz, sz;
	int			x1, x2, y1, y2, z1, z2;
	float		value1[3], value2[3];
	float		valueX1[3], valueX2[3], valueY1[3], valueY2[3];
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

		vy = (m_nStartPos[i] / m_nBufferWidth) % m_nBufferHeight + y / m_nWaveLength[i];
		y1 = int(vy);
		sy = vy - y1;
		sy = S_CURVE(sy);
		y2 = y1 + 1;

		vz = m_nStartPos[i] / (m_nBufferWidth * m_nBufferDepth) + z / m_nWaveLength[i];
		z1 = int(vz);
		sz = vz - z1;
		sz = S_CURVE(sz);
		z2 = z1 + 1;
		
		GetRandValues(x1, y1, z1, value1, nNumValue);
		GetRandValues(x2, y1, z1, value2, nNumValue);
		for(k=0; k<nNumValue; k++)
			valueX1[k] = LERP(sx, value1[k], value2[k]);

		GetRandValues(x1, y2, z1, value1, nNumValue);
		GetRandValues(x2, y2, z1, value2, nNumValue);
		for(k=0; k<nNumValue; k++)
			valueX2[k] = LERP(sx, value1[k], value2[k]);

		for(k=0; k<nNumValue; k++)
			valueY1[k] = LERP(sy, valueX1[k], valueX2[k]);

		GetRandValues(x1, y1, z2, value1, nNumValue);
		GetRandValues(x2, y1, z2, value2, nNumValue);
		for(k=0; k<nNumValue; k++)
			valueX1[k] = LERP(sx, value1[k], value2[k]);

		GetRandValues(x1, y2, z2, value1, nNumValue);
		GetRandValues(x2, y2, z2, value2, nNumValue);
		for(k=0; k<nNumValue; k++)
			valueX2[k] = LERP(sx, value1[k], value2[k]);

		for(k=0; k<nNumValue; k++)
			valueY2[k] = LERP(sy, valueX1[k], valueX2[k]);

		if( m_bTurbulence )
		{
			for(k=0; k<nNumValue; k++)
			{
				value[k] = (float)fabs(LERP(sz, valueY1[k], valueY2[k]));
				vFinal[k] += m_vAmplitude[i] * value[k];
			}
		}
		else
		{
			for(k=0; k<nNumValue; k++)
			{
				value[k] = LERP(sz, valueY1[k], valueY2[k]);
				vFinal[k] += m_vAmplitude[i] * value[k];
			}
		}
	}

	for(k=0; k<nNumValue; k++)
		pvValue[k] = vFinal[k];
}
