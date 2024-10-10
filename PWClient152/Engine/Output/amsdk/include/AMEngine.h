/*
 * FILE: AMEngine.h
 *
 * DESCRIPTION: Interface class for Angelica Media Engine
 *
 * CREATED BY: Hedi, 2002/1/15
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _AMENGINE_H_
#define _AMENGINE_H_

#include "AMTypes.h"
#include "AMPlatform.h"

class AMSoundEngine;
class AMVideoEngine;
class AMConfig;

class AMEngine
{
private:

	HWND			m_hWnd;
	DWORD			m_dwTicks;

	AMSoundEngine*	m_pAMSoundEngine;
	AMVideoEngine*	m_pAMVideoEngine;
	AMConfig*		m_pAMConfig;

	

protected:
public:
	AMEngine();
	virtual ~AMEngine();

	bool Init(HWND hWnd);
	bool Release();

	bool Tick();

	bool Reset();

	bool InitSoundEngine(AMSOUNDDEVFORMAT * pDevFormat, bool bUse3DDevice); 
	bool InitVideoEngine(); // Must cooperate with sound engine;
	bool InitMusicEngine(); // Maybe derived class from sound engine;

	inline AMSoundEngine* GetAMSoundEngine() { return m_pAMSoundEngine; }
	inline AMVideoEngine* GetAMVideoEngine() { return m_pAMVideoEngine; }
	inline DWORD GetTicks() { return m_dwTicks; }
	inline AMConfig* GetAMConfig() { return m_pAMConfig; }
};

#endif	//	_AMENGINE
