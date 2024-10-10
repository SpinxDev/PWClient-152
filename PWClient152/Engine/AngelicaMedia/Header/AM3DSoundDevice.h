/*
 * FILE: AM3DSoundDevice.h
 *
 * DESCRIPTION: 3d sound listener class
 *
 * CREATED BY: Hedi, 2002/1/22
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#ifndef _AM3DSOUNDDEVICE_H_
#define _AM3DSOUNDDEVICE_H_

#include "AMPlatform.h"
#include "AMTypes.h"
#include "AMSoundDevice.h"

typedef class AMSoundEngine * PAMSoundEngine;

class AM3DSoundDevice : public AMSoundDevice
{
private:
	PAMSoundEngine				m_pAMSoundEngine;
	IDirectSound3DListener8 *	m_pDS3DListener8;
	AMS3DLISTENERDESC			m_3dlistenerDesc;

protected:
	bool RetrieveParameters();

public:
	AM3DSoundDevice();
	~AM3DSoundDevice();

	bool Init(PAMSoundEngine pAMSoundEngine, AMSOUNDDEVFORMAT * pDevFormat);
	bool Release();

	// Set the parameters;
	inline void SetDistanceFactor(FLOAT vDistanceFactor) { m_3dlistenerDesc.vDistanceFactor = vDistanceFactor; }
	inline void SetDopplerFactor(FLOAT vDopplerFactor) { m_3dlistenerDesc.vDopplerFactor = vDopplerFactor; }
	inline void SetOrientation(A3DVECTOR3& vecFront, A3DVECTOR3& vecTop)
	{
		m_3dlistenerDesc.vecOrientFront = vecFront;
		m_3dlistenerDesc.vecOrientTop = vecTop;
	}
	inline void SetPosition(A3DVECTOR3& vecPos) { m_3dlistenerDesc.vecPos = vecPos; }
	inline void SetRolloffFactor(FLOAT vRolloffFactor) { m_3dlistenerDesc.vRolloffFactor = vRolloffFactor; }
	inline void SetVelocity(A3DVECTOR3& vecVel) { m_3dlistenerDesc.vecVel = vecVel; }
	
	inline FLOAT GetDistanceFactor() { return m_3dlistenerDesc.vDistanceFactor; }
	inline FLOAT GetDopplerFactor() { return m_3dlistenerDesc.vDopplerFactor; }
	inline A3DVECTOR3 GetOrientationDir(){ return m_3dlistenerDesc.vecOrientFront; }
	inline A3DVECTOR3 GetOrientationUp() { return m_3dlistenerDesc.vecOrientTop; }
	inline A3DVECTOR3 GetPosition() { return m_3dlistenerDesc.vecPos; }
	inline FLOAT GetRolloffFactor() { return m_3dlistenerDesc.vRolloffFactor; }
	inline A3DVECTOR3 GetVelocity() { return m_3dlistenerDesc.vecVel; }

	bool UpdateChanges();

	// inline functions to get these parameters;
	//...
};

typedef class AM3DSoundDevice * PAM3DSoundDevice;
#endif//_AM3DSOUNDDEVICE_H_