/*
 * FILE: A3DRain.h
 *
 * DESCRIPTION: Rain system
 *
 * CREATED BY: Hedi, 2001/12/3
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DRAIN_H_
#define _A3DRAIN_H_

#include "A3DTypes.h"
#include "A3DParticleSystem.h"
#include "A3DSuperSpray.h"
#include "APerlinNoise1D.h"

class A3DTexture;
class AFile;
class A3DCameraBase;

class A3DRainSplash : public A3DSuperSpray
{
private:
	int				m_nSpawnNumber;		// How many splash will be create for a single rain drop
	float			m_vSplashScatter;	// range of scatter

protected:
public:
	A3DRainSplash();
	virtual ~A3DRainSplash();

	bool Init(A3DDevice * pA3DDevice);
	bool Release();

	bool Save(AFile * pFileToSave);
	bool Load(A3DDevice * pA3DDevice, AFile * pFileToLoad);

	bool TickSplash();

	bool EmitParticles();
	bool MakeDead(LPVOID pParticle);

	bool AddSplash(const A3DVECTOR3& vecPos, const A3DVECTOR3& vecDir, bool bOnTerrain);

	inline void SetSpawnNumber(int nNum)	{ m_nSpawnNumber = nNum; }
	inline int GetSpawnNumber()				{ return m_nSpawnNumber; }

	inline void SetSplashScatter(float vScatter)	{ m_vSplashScatter = vScatter; }
	inline float GetSplashScatter()					{ return m_vSplashScatter; }
};

class A3DRain : public A3DParticleSystem
{
private:
	// Perlin Noise used to turb the movement of the snow particle;
	APerlinNoise1D	m_noise;
	A3DRainSplash	* m_pSplash;

	// Standard Particle System Control Sets;
	float			m_vAmplitude;
	float			m_vEmitterCenterX;
	float			m_vEmitterCenterZ;
	float			m_vEmitterWidth;	// area width in which snows are falling
	float			m_vEmitterLength;	// area length in which snows are falling
	float			m_vEmitterHeight;	// height above camera

	A3DCameraBase	* m_pHostCamera;	// Camera to follow

	A3DTLVERTEX		m_pRainCurtainVerts[6]; // the rain curtain effects;
	A3DTexture		* m_pRainCurtainTexture;

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
	A3DRain();
	~A3DRain();

	bool Init(A3DDevice * pA3DDevice, FLOAT vAmplitude);
	bool Release();

	bool StartRain();
	bool CreateRain();	// Alloc neccesary memory

	bool Save(AFile * pFileToSave);
	bool Load(A3DDevice * pA3DDevice, AFile * pFileToLoad);

	bool UpdateStandardParticles();
	bool RenderStandardParticles(A3DViewport * pCurrentViewport);

	bool TickEmitting();

	bool EmitParticles();
	bool MakeDead(LPVOID pParticle);

	bool ShowRainCurtain(A3DViewport * pCurrentViewport);
	bool SetRainCurtainTexture(char * szTextureMap);

public:
	inline void SetHostCamera(A3DCameraBase * pCamera)	{ m_pHostCamera = pCamera; }
	inline A3DCameraBase * GetHostCamera()				{ return m_pHostCamera; }

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

	inline A3DRainSplash * GetRainSplash()		{ return m_pSplash; }
	inline void SetRainSplash(A3DRainSplash * pSplash) { m_pSplash = pSplash; }
};

typedef A3DRain * PA3DRain;

#endif//_A3DRAIN_H_
