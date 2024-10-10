/*
 * FILE: APerlinNoise1D.h
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

/*
  buffer will be looping and smoothing in 1D direction;
  each octave has its own start position and wave length, so it take different random value
  from the buffer
*/

#ifndef _APERLINNOISE1D_H_
#define _APERLINNOISE1D_H_

#include "APerlinNoiseBase.h"

class APerlinNoise1D : public APerlinNoiseBase
{
private:
	int				m_nBufferLen;	
	NOISEVALUE		*m_pValues;		// the spool buffer containing rand values, maximum contains 3 number value;

protected:
	inline void GetRandValues(int n, float * pvValues, int nNumValue);

public:
	APerlinNoise1D();
	~APerlinNoise1D();
	
	// The peroid will be nBufferLen * nWaveLength
	bool Init(int nBufferLen, float vAmplitude, int nWaveLength, float vPersistence, int nOctaveNum, DWORD dwRandSeed);
	bool Release();

	// Get a value, containing 1D, 2D or 3D continuous and smooth random data. 
	// input
	//		x		the position of the value in the perlin noise series, can be a value in real situation
	void GetValue(float x, float * pvValue, int nNumValue);
};

void APerlinNoise1D::GetRandValues(int n, float * pvValues, int nNumValue)
{
	// Clamp to buffer range;
	while(n < 0) n += m_nBufferLen;
	n = n % m_nBufferLen; // this is based on the n must be larger than 0
	
	if( nNumValue > 3 ) nNumValue = 3;
	
	for(int i=0; i<nNumValue; i++)
	{
		pvValues[i] = m_pValues[n].v[i];
	}
}
#endif//_APERLINNOISE1D_H_