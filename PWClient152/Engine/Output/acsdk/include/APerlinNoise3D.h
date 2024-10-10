/*
 * FILE: APerlinNoise3D.h
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

/*
  buffer will be looping and smoothing in 2D direction;
  each octave has its own start position and wave length, so it take different random value
  from the buffer
*/

#ifndef _APERLINNOISE3D_H_
#define _APERLINNOISE3D_H_

#include "APerlinNoiseBase.h"

class APerlinNoise3D : public APerlinNoiseBase
{
private:
	int				m_nBufferWidth;	
	int				m_nBufferHeight;	
	int				m_nBufferDepth;
	NOISEVALUE		*m_pValues;		// the spool buffer containing rand values, maximum contains 3 number value;

protected:
	inline void GetRandValues(int x, int y, int z, float * pvValues, int nNumValue);

public:
	APerlinNoise3D();
	~APerlinNoise3D();
	
	// the peroid will be nBufferWidth * nWaveLength and nBufferHeight * nWaveLength
	bool Init(int nBufferWidth, int nBufferHeight, int nBufferDepth, float vAmplitude, int nWaveLength, float vPersistence, int nOctaveNum, DWORD dwRandSeed);
	bool Release();

	// Get a value, containing 1D, 2D or 3D continuous and smooth random data. 
	// input
	//		x, y, z	the position of the point
	void GetValue(float x, float y, float z, float * pvValue, int nNumValue);
};

void APerlinNoise3D::GetRandValues(int x, int y, int z, float * pvValues, int nNumValue)
{
	while(x < 0) x += m_nBufferWidth;
	while(y < 0) y += m_nBufferHeight;
	while(z < 0) z += m_nBufferDepth;
	// Clamp to buffer range;
	x = x % m_nBufferWidth; // this is based on x must be larger than 0
	y = y % m_nBufferHeight; // this is based on y must be larger than 0
	z = z % m_nBufferDepth; // this is based on z must be larger than 0
	
	int n = z * m_nBufferWidth * m_nBufferHeight + y * m_nBufferWidth + x;
	for(int i=0; i<nNumValue; i++)
	{
		pvValues[i] = m_pValues[n].v[i];
	}
}

#endif//_APERLINNOISE2D_H_