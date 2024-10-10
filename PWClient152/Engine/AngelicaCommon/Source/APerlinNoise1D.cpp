/*
 * FILE: APerlinNoise1D.cpp
 *
 * DESCRIPTION: class for generating continuous PerlinNoise on 1D space
 *				This class is used for generate 1D continuous perlin noise value only
 *
 * CREATED BY: Hedi, 2002/9/13
 *
 * HISTORY:
 *
 * Copyright (c) 2001~2008 Archosaur Studio, All Rights Reserved.	
 */

#include "APerlinNoise1D.h"
#include "AMemory.h"
#include <math.h>

APerlinNoise1D::APerlinNoise1D() 
{
	m_pValues = NULL;
	m_nBufferLen = 0;
}

APerlinNoise1D::~APerlinNoise1D()
{
	Release();
}

bool APerlinNoise1D::Init(int nBufferLen, float vAmplitude, int nWaveLength, float vPersistence, int nOctaveNum, DWORD dwRandSeed)
{
	// generate loop back smoothed random buffer
	if (nBufferLen <= 0)
		return false;

	//	Release old resource if necessary
	if (m_nBufferLen != nBufferLen)
	{
		Release();

		m_nBufferLen = nBufferLen;

		m_pValues = (NOISEVALUE*)a_malloc(sizeof(NOISEVALUE) * m_nBufferLen);
		if (NULL == m_pValues)
			return false;
	}

	int	i, k;

	m_dwSeed = dwRandSeed;

	float * pValues = (float*)a_malloctemp(sizeof(float) * m_nBufferLen);
	if (NULL == pValues)
		return false;

	for(k=0; k<3; k++)
	{
		// First create random number buffer;
		for(i=0; i<m_nBufferLen; i++)
			pValues[i] = RandFloat();
		
		// Now smooth the random number buffer;
		for(i=0; i<m_nBufferLen; i++)
		{
			int nLast, nNext;
			nLast = i - 1;
			if( nLast < 0 ) nLast += m_nBufferLen;
			nNext = i + 1;
			if( nNext >= m_nBufferLen ) nNext -= m_nBufferLen;
			m_pValues[i].v[k] = 0.25f * pValues[nLast] + 0.5f * pValues[i] + 0.25f * pValues[nNext];
		}
	}

	a_freetemp(pValues);

	return InitParams(vAmplitude, nWaveLength, vPersistence, nOctaveNum);
}

bool APerlinNoise1D::Release()
{
	if( m_pValues )
	{
		a_free(m_pValues);
		m_pValues = NULL;
	}

	m_nBufferLen = 0;

	return true;
}

void APerlinNoise1D::GetValue(float x, float * pvValue, int nNumValue)
{
	int			i, k;
	float		v, s;
	int			n1, n2;
	float		value1[3], value2[3];
	float		value[3];
	float		vFinal[3];

	ZeroMemory(vFinal, sizeof(float) * nNumValue);
	for(i=0; i<m_nOctaveNum; i++)
	{
		v = m_nStartPos[i] + x / m_nWaveLength[i];
		n1 = int(v);
		s = v - n1;
		n2 = n1 + 1;
		
		GetRandValues(n1, value1, nNumValue);
		GetRandValues(n2, value2, nNumValue);

		if( m_bTurbulence )
		{
			for(k=0; k<nNumValue; k++)
			{
				value[k] = (float)fabs(LERP(s, value1[k], value2[k]));
				vFinal[k] += m_vAmplitude[i] * value[k];
			}
		}
		else
		{
			for(k=0; k<nNumValue; k++)
			{
				value[k] = LERP(s, value1[k], value2[k]);
				vFinal[k] += m_vAmplitude[i] * value[k];
			}
		}
	}

	for(k=0; k<nNumValue; k++)
		pvValue[k] = vFinal[k];
}
