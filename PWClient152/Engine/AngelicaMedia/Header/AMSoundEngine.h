/*
 * FILE: AMSoundEngine.h
 *
 * DESCRIPTION: The class standing for the sound engine of Angelica Media Engine
 *
 * CREATED BY: Hedi, 2002/1/15
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _AMSOUNDENGINE_H_
#define _AMSOUNDENGINE_H_

#include "AMTypes.h"
#include "AMPlatform.h"

class AMEngine;
class AMSoundDevice;
class AMSoundBuffer;
class AM3DSoundDevice;
class AM3DSoundBuffer;
class AMSoundBufferMan;

class AMSoundEngine
{
private:
	HWND					m_hWnd;
	IDirectSound8*			m_pDS;

	AMSoundDevice*			m_pAMSoundDevice;
	AM3DSoundDevice*		m_pAM3DSoundDevice;
	AMSoundBufferMan*		m_pAMSoundBufferMan;

	AMEngine*				m_pAMEngine;

	DWORD					m_dwVolume;
	float					m_fSFXPlaySpeed;		// current sound sfx play back speed to the original speed

protected:

public:
	AMSoundEngine();
	virtual ~AMSoundEngine();

	bool Init(AMEngine* pAMEngine, HWND hWnd, AMSOUNDDEVFORMAT* pDevFormat, bool bUse3DDevice);
	bool Release();

	bool Tick();

	bool Reset();

	inline IDirectSound8* GetDS() { return m_pDS; }
	inline HWND GetHWnd() { return m_hWnd; }
	inline AMEngine* GetAMEngine() { return m_pAMEngine; }
	inline AMSoundDevice* GetAMSoundDevice() { return m_pAMSoundDevice; }
	inline AM3DSoundDevice* GetAM3DSoundDevice() { return m_pAM3DSoundDevice; }
	inline AMSoundBufferMan* GetAMSoundBufferMan() { return m_pAMSoundBufferMan; }
	inline DWORD GetVolume() { return m_dwVolume; }
	inline float GetSFXPlaySpeed() { return m_fSFXPlaySpeed; }

	//	Set all active sound buffers' volume, see reference to AMSoundBufferMan::SetVolume
	bool SetVolume(DWORD dwVolume);
	//	Set all active sound buffers' relative volume, see reference to AMSoundBufferMan::SetRelativeVolume
	bool SetRelativeVolume(DWORD dwVolume);
	bool SetSFXPlaySpeed(float fSFXPlaySpeed);

	int GetLoaded2DSoundNum();
	int GetLoaded3DSoundNum();
};


#endif	//	_AMSOUNDENGINE_H_
