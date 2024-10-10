/*
 * FILE: AMSoundDevice.h
 *
 * DESCRIPTION: This is in fact the primary sound buffer, but it decide the sound
 *			formats on the card, so this will act as a device
 *
 * CREATED BY: Hedi, 2002/1/15
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _AMSOUNDDEVICE_H_
#define _AMSOUNDDEVICE_H_

#include "AMTypes.h"
#include "AMPlatform.h"

typedef class AMSoundEngine * PAMSoundEngine;

class AMSoundDevice
{
private:
	PAMSoundEngine		m_pAMSoundEngine;
	IDirectSoundBuffer *m_pDSPrimaryBuffer;
	AMSOUNDDEVFORMAT	m_deviceFormat;

protected:
	DWORD				m_dwFlags;

public:
	AMSoundDevice();
	~AMSoundDevice();

	virtual bool Init(PAMSoundEngine pAMSoundEngine, AMSOUNDDEVFORMAT * pDevFormat);
	virtual bool Release();

	inline IDirectSoundBuffer * GetPrimaryBuffer() { return m_pDSPrimaryBuffer; }
	inline const AMSOUNDDEVFORMAT& GetDeviceFormat() { return m_deviceFormat; }
};

typedef class AMSoundDevice * PAMSoundDevice;
#endif//_AMSOUNDDEVICE_H_