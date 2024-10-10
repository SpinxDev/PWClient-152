/*
 * FILE: A3DSnow.h
 *
 * DESCRIPTION: Snow system
 *
 * CREATED BY: Hedi, 2001/12/3
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DSNOW_H_
#define _A3DSNOW_H_

#include "A3DTypes.h"
#include "A3DParticleSystem.h"
#include "APerlinNoise1D.h"

class A3DDevice;
class A3DCamera;
class AFile;

class A3DSnow : public A3DParticleSystem
{
private:
	// Perlin Noise used to turb the movement of the snow particle;
	APerlinNoise1D	m_noise;

	// Standard Particle System Control Sets;
	float			m_vAmplitude;
	float			m_vEmitterCenterX;
	float			m_vEmitterCenterZ;
	float			m_vEmitterWidth;	// area width in which snows are falling
	float			m_vEmitterLength;	// area length in which snows are falling
	float			m_vEmitterHeight;	// height above the camera

	A3DCamera		* m_pHostCamera;	// Camera to follow

protected:
	inline FLOAT GetNextParticleXOffset()
	{
		return RandFloat(m_vEmitterCenterX, m_vEmitterWidth);
	}
	inline FLOAT GetNextParticleZOffset()
	{
		return RandFloat(m_vEmitterCenterZ, m_vEmitterLength);
	}
	
public:
	A3DSnow();
	~A3DSnow();

	bool Init(A3DDevice * pA3DDevice, FLOAT vAmplitude);
	bool Release();

	bool StartSnow();
	bool CreateSnow();	// Alloc neccesary memory

	bool Save(AFile * pFileToSave);
	bool Load(A3DDevice * pA3DDevice, AFile * pFileToLoad);

	bool UpdateStandardParticles();

	bool EmitParticles();
	bool MakeDead(void* pParticle);

public:
	inline void SetHostCamera(A3DCamera * pCamera)	{ m_pHostCamera = pCamera; }
	inline A3DCamera * GetHostCamera()				{ return m_pHostCamera; }

	inline void SetEmitterCenterX(FLOAT x)		{ m_vEmitterCenterX = x; }
	inline FLOAT GetEmitterCenterX()			{ return m_vEmitterCenterX; }
	inline void SetEmitterCenterZ(FLOAT z)		{ m_vEmitterCenterZ = z; }
	inline FLOAT GetEmitterCenterZ()			{ return m_vEmitterCenterZ; }

	inline void SetEmitterWidth(FLOAT width)	{ m_vEmitterWidth = width; }
	inline FLOAT GetEmitterWidth()				{ return m_vEmitterWidth; }
	inline void SetEmitterLength(FLOAT length)	{ m_vEmitterLength = length; }
	inline FLOAT GetEmitterLength()				{ return m_vEmitterLength; }
	inline void SetEmitterHeight(FLOAT height)	{ m_vEmitterHeight = height; }
	inline FLOAT GetEmitterHeight()				{ return m_vEmitterHeight; }
};

typedef A3DSnow * PA3DSnow;

#endif//_A3DSNOW_H_
