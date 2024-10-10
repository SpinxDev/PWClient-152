/*
 * FILE: AMSoundDevice.cpp
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
#include "AMPI.h"
#include "AMSoundEngine.h"
#include "AMSoundDevice.h"

AMSoundDevice::AMSoundDevice()
{
	m_pAMSoundEngine	= NULL;
	m_pDSPrimaryBuffer	= NULL;

	m_dwFlags			= DSBCAPS_PRIMARYBUFFER;
}

AMSoundDevice::~AMSoundDevice()
{
}

bool AMSoundDevice::Init(PAMSoundEngine pAMSoundEngine, AMSOUNDDEVFORMAT * pDevFormat)
{
	m_pAMSoundEngine	= pAMSoundEngine;
	m_deviceFormat		= *pDevFormat;

	// If we did not create the direct sound object;
	if( NULL == m_pAMSoundEngine->GetDS() )
		return true;

	// We create a dsound buffer for primary buffer;
	WAVEFORMATEX	wavFormat;

	ZeroMemory(&wavFormat, sizeof(wavFormat));
	wavFormat.wFormatTag		= WAVE_FORMAT_PCM;
	wavFormat.nChannels			= m_deviceFormat.nChannels;
	wavFormat.nSamplesPerSec	= m_deviceFormat.nSamplesPerSec;
	wavFormat.wBitsPerSample	= m_deviceFormat.wBitsPerSample;
	wavFormat.nBlockAlign		= wavFormat.wBitsPerSample * wavFormat.nChannels / 8;
	wavFormat.nAvgBytesPerSec	= wavFormat.nBlockAlign * wavFormat.nSamplesPerSec;

	HRESULT			hval;
	DSBUFFERDESC	dsbDesc;

	ZeroMemory(&dsbDesc, sizeof(dsbDesc));
	dsbDesc.dwSize			= sizeof(dsbDesc);
	dsbDesc.dwBufferBytes	= 0; // Must be 0 for primary buffer;
	dsbDesc.dwFlags			= m_dwFlags;

	hval = m_pAMSoundEngine->GetDS()->CreateSoundBuffer(&dsbDesc, &m_pDSPrimaryBuffer, NULL);
	if( hval != DS_OK )
	{
		AMERRLOG(("AMSoundDevice::Init, Init Primary Sound buffer fail!"));
		return false;
	}

	hval = m_pDSPrimaryBuffer->SetFormat(&wavFormat);
	if( hval != DS_OK )
	{
		AMERRLOG(("AMSoundDevice::Init, Set Primary Sound Buffer Format Fail!"));
		return false;
	}

	hval = m_pDSPrimaryBuffer->Play(0, 0, DSBPLAY_LOOPING);
	if( hval != DS_OK )
	{
		AMERRLOG(("AMSoundDevice::Init, Play Primary Buffer Fail!"));
		return false;
	}
	return true;
}

bool AMSoundDevice::Release()
{
	if( m_pDSPrimaryBuffer )
	{
		m_pDSPrimaryBuffer->Stop();
		m_pDSPrimaryBuffer->Release();
		m_pDSPrimaryBuffer = NULL;
	}
	return true;
}