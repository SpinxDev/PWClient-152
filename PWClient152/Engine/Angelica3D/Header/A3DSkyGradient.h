/*
 * FILE: A3DSkyGradient.h
 *
 * DESCRIPTION: Class that standing for a gradient sky implementation algorithm, which
				take use of a gradient color and an hemisphere cloud sky
 *
 * CREATED BY: Hedi, 2002/5/8
 *
 * HISTORY:
 *
 * Copyright (c) 2001~2003 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DSKYGRADIENT_H_
#define _A3DSKYGRADIENT_H_

#include "A3DSky.h"

class A3DTexture;

class A3DSkyGradient : public A3DSky
{
private:
	A3DTexture *	m_pTextureSkyCloud;
	A3DTexture *	m_pTextureSkyGradient;

	// Shape parameters;
	float			m_vRadius;
	float			m_vTile;
	float			m_vPitchRange; // the pitch range that we take as the sky cone

	// Animation parameters;
	float			m_vFlySpeedU; // how many u v coordinates shit in one second;
	float			m_vFlySpeedV;

	int				m_nSegmentCount;
	int				m_nVertCount;
	int				m_nFaceCount;

	A3DMATRIX4		m_tmCloud;
	A3DStream *		m_pSkyCloud; // a hemisphere
	A3DStream *		m_pSkyGradient; // a hemisphere too

protected:
	bool Build(); // build stream for the sky

public:
	A3DSkyGradient();
	virtual ~A3DSkyGradient();

	bool Init(A3DDevice* pA3DDevice, A3DCameraBase* pCamera, const char* szSkyCloud, const char* szSkyGradient, float vTile=2.0f);
	bool Release();

	bool Render();
	bool TickAnimation();

	// Modify methods;
	bool SetSkyCloudMap(char * szCloudTexture);
	bool SetSkyGradientMap(char * szGradientTexture);

	inline float GetFlySpeedU() { return m_vFlySpeedU; }
	inline float GetFlySpeedV() { return m_vFlySpeedV; }
	inline void SetFlySpeed(float u, float v) { m_vFlySpeedU = u; m_vFlySpeedV = v; }

	inline float GetTile() { return m_vTile; }
	inline void SetTile(float vTile) { m_vTile = vTile; }
};

typedef A3DSkyGradient * PA3DSkyGradient;
#endif//_A3DSKYGRADIENT_H_
