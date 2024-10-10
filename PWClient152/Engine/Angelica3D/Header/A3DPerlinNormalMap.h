/*
 * FILE: A3DPerlinNormalMap.h
 *
 * DESCRIPTION: a normal map generated with perlin noise dynamically, need pixel shader 2.0 at least
 *
 * CREATED BY: Hedi, 2005/8/3
 *
 * HISTORY:
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DPERLINNORMALMAP_H_
#define _A3DPERLINNORMALMAP_H_

#include <A3DTypes.h>

class A3DTexture;
class A3DDevice;
class A3DRenderTarget;
class A3DStream;
class A3DPixelShader;
class A3DCamera;

class A3DPerlinNormalMap 
{
private:
	A3DDevice *				m_pA3DDevice;
	A3DTexture *			m_pNoiseMaps[6];
	A3DRenderTarget *		m_pNormalMapTarget;
	A3DViewport *			m_pViewport;
	A3DCamera *				m_pCamera;

	int						m_nSize;
	A3DStream *				m_pStream;
	A3DPixelShader *		m_pPixelShader;

	float					m_vAmplitude;
	float					m_vPersistence;

protected:

public:
	A3DPerlinNormalMap();
	~A3DPerlinNormalMap();

	bool Init(A3DDevice * pA3DDevice, int nSize, float vAmplitude, float vPersistence);
	bool Release();

	bool UpdateNormalMap(int nDeltaTime);

	bool Appear(int level);
	bool Disappear(int level);
};

#endif
