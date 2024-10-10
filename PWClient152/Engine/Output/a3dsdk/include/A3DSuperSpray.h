/*
 * FILE: A3DSuperSpray.h
 *
 * DESCRIPTION: Particle System, which use off axis and off plance
		to determine the start position and speed of the particle, this is infact a 
		SUPER SPRY Particle System in 3DS MAX for Angelica 3D Engine
 *
 * CREATED BY: Hedi, 2001/12/3
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DSUPERSPRAY_H_
#define _A3DSUPERSPRAY_H_

#include "A3DPlatform.h"
#include "A3DParticleSystem.h"

class A3DDevice;
class AFile;

class A3DSuperSpray : public A3DParticleSystem
{
private:
	// Standard Particle System Control Sets;
	FLOAT m_vOffAxis; // In degree; The axis along which particles are emitted;
	FLOAT m_vOffAxisSpread; // In degree; How wide the particles are spreaded; The OffAxis's Variation

	FLOAT m_vOffPlane; // In degree; The plane on which the particles are spread;
	FLOAT m_vOffPlaneSpread; // In degree; The plane's variation;

protected:
	inline FLOAT GetNextParticleOffAxis()
	{
		return RandFloat(m_vOffAxis, m_vOffAxisSpread);
	}
	inline FLOAT GetNextParticleOffPlane()
	{
		return RandFloat(m_vOffPlane, m_vOffPlaneSpread);
	}
	
public:
	A3DSuperSpray();
	~A3DSuperSpray();

	bool Release();

	bool Save(AFile * pFileToSave);
	bool Load(A3DDevice * pA3DDevice, AFile * pFileToLoad);

	bool EmitParticles();
	bool MakeDead(LPVOID pParticle);
public:
	inline void SetOffAxis(FLOAT vOffAxis) { m_vOffAxis = vOffAxis; }
	inline FLOAT GetOffAxis() { return m_vOffAxis; }
	inline void SetOffAxisSpread(FLOAT vOffAxisSpread) { m_vOffAxisSpread = vOffAxisSpread; }
	inline FLOAT GetOffAxisSpread() { return m_vOffAxisSpread; }
	inline void SetOffPlane(FLOAT vOffPlane) { m_vOffPlane = vOffPlane; }
	inline FLOAT GetOffPlane() { return m_vOffPlane; }
	inline void SetOffPlaneSpread(FLOAT vOffPlaneSpread) { m_vOffPlaneSpread = vOffPlaneSpread; }
	inline FLOAT GetOffPlaneSpread() { return m_vOffPlaneSpread; }
};

typedef A3DSuperSpray * PA3DSuperSpray;

#endif//_A3DSUPERSPRAY_H_
