/*
 * FILE: ACounter.h
 *
 * DESCRIPTION: An counter class for A3D Engine
 *
 * CREATED BY: Hedi, 2001/12/3
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _ACounter_H_
#define _ACounter_H_

#include "ABaseDef.h"
#include "ACPlatform.h"

class ACounter
{
private:
	DWORD				m_dwFPSLastTick;
	DWORD				m_dwFPSCounter;
	FLOAT				m_vFPS;

protected:
	static double		m_fCPUFrequency;

public:
	ACounter();
	virtual ~ACounter();

	bool Init();
	bool Release();

	static DWORD GetMilliSecondNow();
	static __int64 GetMicroSecondNow();
	static __int64 GetCPUCycle();
	static double GetCPUFrequency();

	bool ResetFPSCounter();
	bool UpdateFPSCounter(bool * pFPSChanged = NULL);
	inline FLOAT GetFPS() { return m_vFPS; }
};

#endif