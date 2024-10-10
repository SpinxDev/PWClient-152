/*
 * FILE: AMEngine.cpp
 *
 * DESCRIPTION: Interface class for Angelica Media engine
 *
 * CREATED BY: Hedi, 2002/1/15
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#include "AMPI.h"
#include "AMEngine.h"
#include "AMSoundEngine.h"
#include "AMVideoEngine.h"
#include "AMConfig.h"

AMEngine::AMEngine()
{
	m_hWnd				= NULL;
	m_pAMSoundEngine	= NULL;
	m_pAMVideoEngine	= NULL;
	m_dwTicks			= 0;
}

AMEngine::~AMEngine()
{
}

bool AMEngine::Init(HWND hWnd)
{
	m_hWnd = hWnd;

	if (!(g_pAMErrLog = new ALog))
		return false;

	if (!g_pAMErrLog->Init("AM.log", "Angelica Media Error Log", false))
		return false;

	if (!(m_pAMConfig = new AMConfig))
	{
		AMERRLOG(("AMEngine::Init(), Not enough memory!"));
		return false;
	}

	return true;
}

bool AMEngine::Release()
{
	if (m_pAMVideoEngine)
	{
		m_pAMVideoEngine->Release();
		delete m_pAMVideoEngine;
		m_pAMVideoEngine = NULL;
	}

	if (m_pAMSoundEngine)
	{
		m_pAMSoundEngine->Release();
		delete m_pAMSoundEngine;
		m_pAMSoundEngine = NULL;
	}


	if (m_pAMConfig)
	{
		delete m_pAMConfig;
		m_pAMConfig = NULL;
	}

	if (g_pAMErrLog)
	{
		g_pAMErrLog->Release();
		delete g_pAMErrLog;
		g_pAMErrLog = NULL;
	}

	m_hWnd = NULL;
	return true;
}

bool AMEngine::InitSoundEngine(AMSOUNDDEVFORMAT * pDevFormat, bool bUse3DDevice)
{
	m_pAMSoundEngine = new AMSoundEngine();
	if( NULL == m_pAMSoundEngine )
	{
		AMERRLOG(("AMEngine::InitSoundEngine, Not enough memory!"));
		return false;
	}

	if( !m_pAMSoundEngine->Init(this, m_hWnd, pDevFormat, bUse3DDevice) )
	{
		AMERRLOG(("AMEngine::InitSoundEngine, Call AMSoundEngine::Init Fail!"));
		return false;
	}
	return true;
}

bool AMEngine::InitVideoEngine()
{
	m_pAMVideoEngine = new AMVideoEngine();
	if( NULL == m_pAMVideoEngine )
	{
		AMERRLOG(("AMEngine::InitVideoEngine, Not enough memory!"));
		return false;
	}

	if( !m_pAMVideoEngine->Init(this) )
	{
		AMERRLOG(("AMEngine::InitVideoEngine, Call AMVideoEngine::Init Fail!"));
		return false;
	}
	return true;
}

bool AMEngine::Reset()
{
	if( m_pAMSoundEngine )
	{
		m_pAMSoundEngine->Reset();
	}
	if( m_pAMVideoEngine )
	{
		m_pAMVideoEngine->Reset();
	}
	return true;
}

bool AMEngine::Tick()
{
	if( m_pAMSoundEngine )
	{
		m_pAMSoundEngine->Tick();
	}

	if( m_pAMVideoEngine )
	{
		m_pAMVideoEngine->Tick();
	}

	m_dwTicks ++;

	return true;
}

