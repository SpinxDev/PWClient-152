/*
 * FILE: AMSoundBuffer.cpp
 *
 * DESCRIPTION: sound buffer class
 *
 * CREATED BY: Hedi, 2002/1/16
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#include "AMConfig.h"
#include "AMPI.h"
#include "AMEngine.h"
#include "AMSoundEngine.h"
#include "AMSoundDevice.h"
#include "AMSoundBuffer.h"
#include "AMWaveFile.h"
#include "AMMp3File.h"
#include "AMOggFile.h"
#include "AMSoundBufferMan.h"
#include "AAssist.h"
#include "AF.h"

AMSoundBuffer::AMSoundBuffer()
{
	m_dwTypeID			= AMSNDBUF_2D;
	m_pAMSoundEngine	= NULL;
	m_pDSBuffer			= NULL;
	m_pDSBuffer8		= NULL;
	m_pDSNotify8		= NULL;

	m_dwStartTime		= 0;
	m_hOffsetStopEvent	= NULL;

	m_nState			= AMSBSTATE_STOPPED;
	m_bLooping			= false;

	m_bLoaded			= false;

	m_fVolumeDelta		= 0.0f;
	m_fVolumeRate		= 0.0f;

	m_dwVolume			= 100;
	m_dwRelativeVolume	= 100;
	m_bFadeIn			= false;
	m_bFadeOut			= false;
	m_dwPosInMan		= 0;
	m_dwBufferID		= 0;
}

AMSoundBuffer::~AMSoundBuffer()
{
}

// Create the sound buffer;
bool AMSoundBuffer::Create(AMSoundEngine* pAMSoundEngine, AMSBUFFERDESC * pDesc)
{
	m_pAMSoundEngine = pAMSoundEngine;
	m_bufferDesc = *pDesc;

	// If we did not create the direct sound object;
	if( NULL == pAMSoundEngine->GetDS() )
		return true;

	DSBUFFERDESC	dsbDesc;
	ZeroMemory(&dsbDesc, sizeof(dsbDesc));
	dsbDesc.dwSize = sizeof(dsbDesc);
	dsbDesc.dwBufferBytes = m_bufferDesc.dwBufferBytes;
	dsbDesc.lpwfxFormat = &m_bufferDesc.wfxFormat;
	dsbDesc.dwFlags = m_bufferDesc.dwFlags | DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY;
	if( pAMSoundEngine->GetAMSoundDevice()->GetDeviceFormat().bGlobalFocus )
		dsbDesc.dwFlags |= DSBCAPS_GLOBALFOCUS;

	switch( m_bufferDesc.algorithm )
	{
	case AMS3DALG_DEFAULT:
		dsbDesc.guid3DAlgorithm = DS3DALG_DEFAULT;
		break;
	case AMS3DALG_NO_VIRTUALIZATION:
		dsbDesc.guid3DAlgorithm = DS3DALG_NO_VIRTUALIZATION;
		break;
	case AMS3DALG_HRTF_FULL:
		dsbDesc.guid3DAlgorithm = DS3DALG_HRTF_FULL;
		break;
	case AMS3DALG_HRTF_LIGHT:
		dsbDesc.guid3DAlgorithm = DS3DALG_HRTF_LIGHT;
		break;
	}

	HRESULT hval;
	hval = m_pAMSoundEngine->GetDS()->CreateSoundBuffer(&dsbDesc, &m_pDSBuffer, NULL);
	if (FAILED(hval))
	{
		AMERRLOG(("AMSoundBuffer::Init, CreateSoundBuffer fail!"));
		return false;
	}

	// Now we create the notify event here;
	hval = m_pDSBuffer->QueryInterface(IID_IDirectSoundBuffer8, (LPVOID *)&m_pDSBuffer8);
	if( hval != DS_OK )
	{
		AMERRLOG(("AMSoundBuffer::Init(), Query DS3DBuffer8 fail!"));
		return false;
	}

	// Now we create the notify event here;
	hval = m_pDSBuffer8->QueryInterface(IID_IDirectSoundNotify, (LPVOID *)&m_pDSNotify8);
	if( hval != DS_OK )
	{
		AMERRLOG(("AMSoundBuffer::Init(), Query DSNotify fail!"));
		return false;
	}

	m_hOffsetStopEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if( NULL == m_hOffsetStopEvent )
	{
		AMERRLOG(("AMSoundBuffer::Init(), CreateEvent fail!"));
		return false;
	}

	DSBPOSITIONNOTIFY	dspn;
	// we must use DSBPN_OFFSETSTOP instead of last byte of the buffer, because
	// read curor is somewhat beyond the real playcursor
	// but on some card DSBPN_OFFSETSTOP will not signal if the buffer is too short (e.g  < 100ms), so we will force a overtime check in our code
	// it seems in this situation, the short buffer will always loop automatically
	dspn.dwOffset = DSBPN_OFFSETSTOP;
	dspn.hEventNotify = m_hOffsetStopEvent;

	hval = m_pDSNotify8->SetNotificationPositions(1, &dspn);
	if (FAILED(hval)) 
	{
		AMERRLOG(("AMSoundBuffer::Init(), SetNotificationPositions fail!"));
		return false;
	}

	SetVolume(m_pAMSoundEngine->GetVolume());
	SetPlaySpeed(m_pAMSoundEngine->GetSFXPlaySpeed());
	
	m_nState = AMSBSTATE_STOPPED;
	m_bLooping = false;
	return true;
}

bool AMSoundBuffer::Release()
{
	UnloadSoundData();

	return true;
}

bool AMSoundBuffer::UnloadSoundData()
{
	if( m_pDSNotify8 )
	{
		m_pDSNotify8->Release();
		m_pDSNotify8 = NULL;
	}

	if( m_pDSBuffer8 )
	{
		m_pDSBuffer8->Stop();
		m_pDSBuffer8->Release();
		m_pDSBuffer8 = NULL;
	}

	if( m_pDSBuffer )
	{
		m_pDSBuffer->Stop();
		m_pDSBuffer->Release();
		m_pDSBuffer = NULL;
	}

	if( m_hOffsetStopEvent )
	{
		CloseHandle(m_hOffsetStopEvent);
		m_hOffsetStopEvent = NULL;
	}

	m_bLoaded = false;
	return true;
}

bool AMSoundBuffer::Play(bool bLoop)
{
	if (!m_bLoaded)
		return true;

	if (m_pDSBuffer8)
	{
		HRESULT		hval;

		DWORD	dwFlags;

		// We should start the sound from the begin if not be paused;
		if( m_nState != AMSBSTATE_PAUSED )
			m_pDSBuffer8->SetCurrentPosition(0);	
		
		if( bLoop )
			dwFlags = DSBPLAY_LOOPING;
		else
			dwFlags = NULL;

		ResetEvent(m_hOffsetStopEvent);

		hval = m_pDSBuffer8->Play(0, 0, dwFlags);
		if (FAILED(hval))
		{
			AMERRLOG(("AMSoundBuffer::Play, Call IDirectSoundBuffer8::Play fail"));
			return false;
		}
		m_dwStartTime = ACounter::GetMilliSecondNow();
	}

	m_nState = AMSBSTATE_PLAYING;
	m_bLooping = bLoop;

	m_bFadeIn = false;
	m_bFadeOut = false;
	return true;
}

bool AMSoundBuffer::Stop()
{
	if( m_pDSBuffer8 )
	{
		m_pDSBuffer8->Stop();
		m_pDSBuffer8->SetCurrentPosition(0);
	}

	m_nState = AMSBSTATE_STOPPED;
	m_bLooping = false;

	if( m_bFadeIn || m_bFadeOut )
	{
		// If we have done some fade, we must set to default volume;
		SetVolume(m_pAMSoundEngine->GetVolume());
	}
	m_bFadeIn = false;
	m_bFadeOut = false;
	return true;
}

bool AMSoundBuffer::Pause()
{
	if (m_nState != AMSBSTATE_PLAYING)
		return true;

	if (m_pDSBuffer8)
		m_pDSBuffer8->Stop();

	m_nState = AMSBSTATE_PAUSED;
	return true;
}

bool AMSoundBuffer::DownSample(LPBYTE pDestBuf, LPBYTE pSrcBuf, WAVEFORMATEX wfx, int nSampleCount, DWORD dwDownRate)
{
	DWORD dwDestIndex = 0;
	DWORD dwSrcIndex = 0;
	for(int i=0; i<nSampleCount; i+=dwDownRate)
	{
		memcpy(pDestBuf + dwDestIndex, pSrcBuf + dwSrcIndex, wfx.nBlockAlign);
		dwDestIndex += wfx.nBlockAlign;
		dwSrcIndex += dwDownRate * wfx.nBlockAlign;
	}

	return true;
}

// Create and load sound data from a file;
bool AMSoundBuffer::Load(AMSoundEngine* pAMSoundEngine, AFile* pFileToLoad, const char* szFileName)
{
	m_pAMSoundEngine = pAMSoundEngine;
	AMConfig* pAMConfig = pAMSoundEngine->GetAMEngine()->GetAMConfig();

	m_strFileName = szFileName;
	m_strFileName.MakeLower();

	m_dwBufferID = a_MakeIDFromString(m_strFileName);

	// If we did not create the direct sound object;
	if (NULL == pAMSoundEngine->GetDS())
		return true;

	if (strstr(m_strFileName, ".wav"))
	{
		// It's a wave format file
		AMWaveFile waveFile;

		if (!waveFile.Open(pFileToLoad))
		{
			AMERRLOG(("AMSoundBuffer::Load, open the file as a wave file fail!"));
			return false;
		}

		AMSBUFFERDESC bufferDesc;
		ZeroMemory(&bufferDesc, sizeof(bufferDesc));

		bufferDesc.wfxFormat = waveFile.GetWaveformatEx();
		bufferDesc.dwBufferBytes = waveFile.GetDataSize();
		bufferDesc.dwFlags = NULL;
		bufferDesc.algorithm = AMS3DALG_DEFAULT;

		DWORD dwDownRate = 1;
		if( pAMConfig->GetSoundQuality() == AMSOUND_QUALITY_MEDIUM )
		{
			if( bufferDesc.wfxFormat.nSamplesPerSec >= 22050 )
				dwDownRate = 2;
		}
		else if( pAMConfig->GetSoundQuality() == AMSOUND_QUALITY_LOW )
		{
			if( bufferDesc.wfxFormat.nSamplesPerSec >= 44100 )
				dwDownRate = 4;
			else if( bufferDesc.wfxFormat.nSamplesPerSec >= 22050 )
				dwDownRate = 2;
		}

		int nSampleCount = bufferDesc.dwBufferBytes / bufferDesc.wfxFormat.nBlockAlign / dwDownRate;
		if( nSampleCount * dwDownRate < waveFile.GetDataSize() / waveFile.GetWaveformatEx().nBlockAlign )
			nSampleCount ++;
		
		bufferDesc.dwBufferBytes = nSampleCount * bufferDesc.wfxFormat.nBlockAlign;
		bufferDesc.wfxFormat.nSamplesPerSec /= dwDownRate;
		bufferDesc.wfxFormat.nAvgBytesPerSec /= dwDownRate;
		if( !Create(pAMSoundEngine, &bufferDesc) )
		{
			AMERRLOG(("AMSoundBuffer::Load, Call Create Fail!"));
			return false;
		}

		// Now lock the direct sound buffer and load data into it;
		HRESULT hval;
		LPBYTE	pBuffer1;
		DWORD	dwBufferLength1;
		LPBYTE	pBuffer2;
		DWORD	dwBufferLength2;

		hval = m_pDSBuffer8->Lock(0, 0, (LPVOID *)&pBuffer1, &dwBufferLength1, 
			(LPVOID *)&pBuffer2, &dwBufferLength2, DSBLOCK_ENTIREBUFFER);
		if( hval != DS_OK )
		{
			AMERRLOG(("AMSoundBuffer::Load, Lock DSBuffer Fail!"));
			return false;
		}

		if( pBuffer2 || dwBufferLength1 != bufferDesc.dwBufferBytes )
		{
			AMERRLOG(("AMSoundBuffer::Load, Lock DSBuffer has generated an loop back pointer!"));
			return false;
		}

		DWORD dwReadSize;
		bool bEnd;
		
		if( dwDownRate == 1 ) // Origin sample rate;
		{
			if( !waveFile.ReadData(pBuffer1, waveFile.GetDataSize(), &dwReadSize, &bEnd) )
			{
				AMERRLOG(("AMSoundBuffer::Load, Read data into buffer fail!"));
				return false;
			}
		}
		else
		{
			LPBYTE pOrgBuf = (LPBYTE)a_malloctemp(waveFile.GetDataSize());
			if( NULL == pOrgBuf )
				return false;
			if( !waveFile.ReadData(pOrgBuf, waveFile.GetDataSize(), &dwReadSize, &bEnd) )
			{
				a_freetemp(pOrgBuf);
				AMERRLOG(("AMSoundBuffer::Load, Read data into buffer fail!"));
				return false;
			}
			DownSample(pBuffer1, pOrgBuf, waveFile.GetWaveformatEx(), waveFile.GetDataSize() / waveFile.GetWaveformatEx().nBlockAlign, dwDownRate);
			a_freetemp(pOrgBuf);
		}

		hval = m_pDSBuffer8->Unlock(pBuffer1, dwBufferLength1, pBuffer2, dwBufferLength2);
		if( hval != DS_OK )
		{
			AMERRLOG(("AMSoundBuffer::Load, Unlock DSBuffer Fail!"));
			return false;
		}
		waveFile.Close();
	}
	else if (strstr(m_strFileName, ".mp3"))
	{
		// It's a mp3 format file
		AMMp3File mp3File;

		if( !mp3File.Open(pFileToLoad->GetFileName()) )
		{
			AMERRLOG(("AMSoundBuffer::Load, open the file as a mp3 file fail!"));
			return false;
		}
		AMSBUFFERDESC	bufferDesc;

		ZeroMemory(&bufferDesc, sizeof(bufferDesc));
		bufferDesc.wfxFormat = mp3File.GetWaveformatEx();
		bufferDesc.dwBufferBytes = mp3File.GetDataSize();
		bufferDesc.dwFlags = NULL;
		bufferDesc.algorithm = AMS3DALG_DEFAULT;

		DWORD dwDownRate = 1;
		if( pAMConfig->GetSoundQuality() == AMSOUND_QUALITY_MEDIUM )
		{
			if( bufferDesc.wfxFormat.nSamplesPerSec >= 22100 )
				dwDownRate = 2;
		}
		else if( pAMConfig->GetSoundQuality() == AMSOUND_QUALITY_LOW )
		{
			if( bufferDesc.wfxFormat.nSamplesPerSec >= 44100 )
				dwDownRate = 4;
			else if( bufferDesc.wfxFormat.nSamplesPerSec >= 22050 )
				dwDownRate = 2;
		}

		int nSampleCount = bufferDesc.dwBufferBytes / bufferDesc.wfxFormat.nBlockAlign / dwDownRate;
		if( nSampleCount * dwDownRate < mp3File.GetDataSize() / mp3File.GetWaveformatEx().nBlockAlign )
			nSampleCount ++;
		bufferDesc.dwBufferBytes = nSampleCount * bufferDesc.wfxFormat.nBlockAlign;
		bufferDesc.wfxFormat.nSamplesPerSec /= dwDownRate;
		bufferDesc.wfxFormat.nAvgBytesPerSec /= dwDownRate;

		if( !Create(pAMSoundEngine, &bufferDesc) )
		{
			AMERRLOG(("AMSoundBuffer::Load, Call Create Fail!"));
			return false;
		}

		// Now lock the direct sound buffer and load data into it;
		HRESULT hval;
		LPBYTE	pBuffer1;
		DWORD	dwBufferLength1;
		LPBYTE	pBuffer2;
		DWORD	dwBufferLength2;

		hval = m_pDSBuffer8->Lock(0, 0, (LPVOID *)&pBuffer1, &dwBufferLength1, 
			(LPVOID *)&pBuffer2, &dwBufferLength2, DSBLOCK_ENTIREBUFFER);
		if( hval != DS_OK )
		{
			AMERRLOG(("AMSoundBuffer::Load, Lock DSBuffer Fail!"));
			return false;
		}

		if( pBuffer2 || dwBufferLength1 != bufferDesc.dwBufferBytes )
		{
			AMERRLOG(("AMSoundBuffer::Load, Lock DSBuffer has generated an loop back pointer!"));
			return false;
		}

		DWORD dwReadSize;
		bool bEnd;
		
		if( dwDownRate == 1 ) // Origin sample rate;
		{
			if( !mp3File.ReadData(pBuffer1, mp3File.GetDataSize(), &dwReadSize, &bEnd) )
			{
				AMERRLOG(("AMSoundBuffer::Load, Read data into buffer fail!"));
				return false;
			}
		}
		else
		{
			LPBYTE pOrgBuf = (LPBYTE)a_malloctemp(mp3File.GetDataSize());
			if( NULL == pOrgBuf )
				return false;
			if( !mp3File.ReadData(pOrgBuf, mp3File.GetDataSize(), &dwReadSize, &bEnd) )
			{
				a_freetemp(pOrgBuf);
				AMERRLOG(("AMSoundBuffer::Load, Read data into buffer fail!"));
				return false;
			}
			DownSample(pBuffer1, pOrgBuf, mp3File.GetWaveformatEx(), mp3File.GetDataSize() / mp3File.GetWaveformatEx().nBlockAlign, dwDownRate);
			a_freetemp(pOrgBuf);
		}

		hval = m_pDSBuffer8->Unlock(pBuffer1, dwBufferLength1, pBuffer2, dwBufferLength2);
		if( hval != DS_OK )
		{
			AMERRLOG(("AMSoundBuffer::Load, Unlock DSBuffer Fail!"));
			return false;
		}
		mp3File.Close();
	}
	else if (strstr(m_strFileName, ".ogg"))
	{
		// It's a mp3 format file
		AMOggFile oggFile;
		
		if( !oggFile.Open(pFileToLoad->GetFileName()) )
		{
			AMERRLOG(("AMSoundBuffer::Load, open the file as a mp3 file fail!"));
			return false;
		}
		AMSBUFFERDESC	bufferDesc;
		
		ZeroMemory(&bufferDesc, sizeof(bufferDesc));
		bufferDesc.wfxFormat = oggFile.GetWaveformatEx();
		bufferDesc.dwBufferBytes = oggFile.GetDataSize();
		bufferDesc.dwFlags = NULL;
		bufferDesc.algorithm = AMS3DALG_DEFAULT;
		
		DWORD dwDownRate = 1;
		if( pAMConfig->GetSoundQuality() == AMSOUND_QUALITY_MEDIUM )
		{
			if( bufferDesc.wfxFormat.nSamplesPerSec >= 22100 )
				dwDownRate = 2;
		}
		else if( pAMConfig->GetSoundQuality() == AMSOUND_QUALITY_LOW )
		{
			if( bufferDesc.wfxFormat.nSamplesPerSec >= 44100 )
				dwDownRate = 4;
			else if( bufferDesc.wfxFormat.nSamplesPerSec >= 22050 )
				dwDownRate = 2;
		}
		
		int nSampleCount = bufferDesc.dwBufferBytes / bufferDesc.wfxFormat.nBlockAlign / dwDownRate;
		if( nSampleCount * dwDownRate < oggFile.GetDataSize() / oggFile.GetWaveformatEx().nBlockAlign )
			nSampleCount ++;
		bufferDesc.dwBufferBytes = nSampleCount * bufferDesc.wfxFormat.nBlockAlign;
		bufferDesc.wfxFormat.nSamplesPerSec /= dwDownRate;
		bufferDesc.wfxFormat.nAvgBytesPerSec /= dwDownRate;
		
		if( !Create(pAMSoundEngine, &bufferDesc) )
		{
			AMERRLOG(("AMSoundBuffer::Load, Call Create Fail!"));
			return false;
		}
		
		// Now lock the direct sound buffer and load data into it;
		HRESULT hval;
		LPBYTE	pBuffer1;
		DWORD	dwBufferLength1;
		LPBYTE	pBuffer2;
		DWORD	dwBufferLength2;
		
		hval = m_pDSBuffer8->Lock(0, 0, (LPVOID *)&pBuffer1, &dwBufferLength1, 
			(LPVOID *)&pBuffer2, &dwBufferLength2, DSBLOCK_ENTIREBUFFER);
		if( hval != DS_OK )
		{
			AMERRLOG(("AMSoundBuffer::Load, Lock DSBuffer Fail!"));
			return false;
		}
		
		if( pBuffer2 || dwBufferLength1 != bufferDesc.dwBufferBytes )
		{
			AMERRLOG(("AMSoundBuffer::Load, Lock DSBuffer has generated an loop back pointer!"));
			return false;
		}
		
		DWORD dwReadSize;
		bool bEnd;
		
		if( dwDownRate == 1 ) // Origin sample rate;
		{
			if( !oggFile.ReadData(pBuffer1, oggFile.GetDataSize(), &dwReadSize, &bEnd) )
			{
				AMERRLOG(("AMSoundBuffer::Load, Read data into buffer fail!"));
				return false;
			}
		}
		else
		{
			LPBYTE pOrgBuf = (LPBYTE)a_malloctemp(oggFile.GetDataSize());
			if( NULL == pOrgBuf )
				return false;
			if( !oggFile.ReadData(pOrgBuf, oggFile.GetDataSize(), &dwReadSize, &bEnd) )
			{
				a_freetemp(pOrgBuf);
				AMERRLOG(("AMSoundBuffer::Load, Read data into buffer fail!"));
				return false;
			}
			DownSample(pBuffer1, pOrgBuf, oggFile.GetWaveformatEx(), oggFile.GetDataSize() / oggFile.GetWaveformatEx().nBlockAlign, dwDownRate);
			a_freetemp(pOrgBuf);
		}
		
		hval = m_pDSBuffer8->Unlock(pBuffer1, dwBufferLength1, pBuffer2, dwBufferLength2);
		if( hval != DS_OK )
		{
			AMERRLOG(("AMSoundBuffer::Load, Unlock DSBuffer Fail!"));
			return false;
		}
		oggFile.Close();
	}
	else
	{
		AMERRLOG(("AMSoundBuffer::Load, File [%s] Unsupported file extension", pFileToLoad->GetFileName()));
		return false;
	}

	m_bLoaded = true;
	return true;
}

bool AMSoundBuffer::Save(AFile * pFileToSave)
{
	return true;
}

bool AMSoundBuffer::Tick(int nDeltaTime)
{
	if( !m_bFadeIn && !m_bFadeOut )
		return true;

	m_fVolumeDelta += (nDeltaTime * 0.001f) * m_fVolumeRate;
	int nVolumeDelta = (int)m_fVolumeDelta;
	m_fVolumeDelta -= nVolumeDelta;

	if( nVolumeDelta == 0 )
		return true;

	// Do fade in(out)
	if( m_bFadeIn )
	{
		m_dwVolume += nVolumeDelta;
		if( m_dwVolume > m_dwVolumeSet )
			m_dwVolume = m_dwVolumeSet;

		SetVolume(m_dwVolume);

		if( m_dwVolumeSet == m_dwVolume )
		{
			m_bFadeIn = false; // finish
			return true;
		}
	}
	else if( m_bFadeOut )
	{
		if( m_dwVolume <= m_dwVolumeSet + nVolumeDelta)
			m_dwVolume = m_dwVolumeSet;
		else
			m_dwVolume -= nVolumeDelta;

		SetVolume(m_dwVolume);

		if( m_dwVolumeSet >= m_dwVolume )
		{
			m_bFadeOut = false;
			Stop();
			return true;
		}
	}

	return true;
}

// Play position control methods;
bool AMSoundBuffer::CheckEnd()
{
	// If we did not create the direct sound object;
	if( NULL == m_pAMSoundEngine->GetDS() )
	{
		Stop();
		return true;
	}

	if( m_nState == AMSBSTATE_PLAYING && !m_bLooping )
	{
		// We should check if we have reached the end;
		if( WAIT_OBJECT_0 == WaitForSingleObject(m_hOffsetStopEvent, 0) )
		{
			// Because we use position notify value DSBPN_OFFSETSTOP, so only when
			// no loop sound reach the end of buffer, this event will be signaled.
			// now we have change back
			Stop();
		}
		else
		{
			// overtime check
			if( ACounter::GetMilliSecondNow() - m_dwStartTime > DWORD(__int64(m_bufferDesc.dwBufferBytes) * 1000 / m_bufferDesc.wfxFormat.nAvgBytesPerSec + 100) )
				Stop();
		}
	}

	return true;
}

DWORD AMSoundBuffer::GetPlayPos()
{
	DWORD		dwPlayPos, dwWritePos;
	HRESULT		hval;

	// If we did not create the direct sound object;
	if( NULL == m_pAMSoundEngine->GetDS() )
		return 0;

	hval = m_pDSBuffer8->GetCurrentPosition(&dwPlayPos, &dwWritePos);
	if (FAILED(hval)) 
	{
		AMERRLOG(("AMSoundBuffer::GetPlayPos(), Get Current position fail!"));
		return 0;
	}

	return dwPlayPos;
}

DWORD AMSoundBuffer::GetWritePos()
{
	DWORD		dwPlayPos, dwWritePos;
	HRESULT		hval;

	// If we did not create the direct sound object;
	if( NULL == m_pAMSoundEngine->GetDS() )
		return 0;

	hval = m_pDSBuffer8->GetCurrentPosition(&dwPlayPos, &dwWritePos);
	if (FAILED(hval)) 
	{
		AMERRLOG(("AMSoundBuffer::GetPlayPos(), Get Current position fail!"));
		return 0;
	}

	return dwWritePos;
}

bool AMSoundBuffer::FadeIn(int nFadeTime)
{
	m_bFadeIn = true;
	m_bFadeOut = false;

	m_dwVolumeSet = m_pAMSoundEngine->GetVolume();
	m_fVolumeDelta = 0.0f;
	m_fVolumeRate = (int(m_dwVolumeSet) - int(m_dwVolume)) * 1000.0f / nFadeTime;
	if( m_fVolumeRate <= 0.0f )
	{
		m_bFadeIn = false;
		m_fVolumeRate = 0.0f;
		SetVolume(m_dwVolumeSet);
	}
	return true;
}

bool AMSoundBuffer::FadeOut(int nFadeTime)
{
	m_bFadeOut = true;
	m_bFadeIn = false;

	m_dwVolumeSet = 0;
	m_fVolumeDelta = 0.0f;
	m_fVolumeRate = m_dwVolume * 1000.0f / nFadeTime;
	if( m_fVolumeRate <= 0.0f )
	{
		m_bFadeOut = false;
		Stop();
	}
	return true;
}

bool AMSoundBuffer::FadeInRelatively(int nFadeTime)
{
	m_bFadeIn = true;
	m_bFadeOut = false;

	m_dwVolumeSet = m_pAMSoundEngine->GetVolume() * m_dwRelativeVolume / 100;
	m_fVolumeDelta = 0.0f;
	m_fVolumeRate = (int(m_dwVolumeSet) - int(m_dwVolume)) * 1000.0f / nFadeTime;
	if( m_fVolumeRate <= 0.0f )
	{
		m_bFadeIn = false;
		m_fVolumeRate = 0.0f;
		SetVolume(m_dwVolumeSet);
	}
	return true;
}

bool AMSoundBuffer::Reload(AFile * pFileToLoad)
{
	if (!m_pDSBuffer8)
		return true;

	// take down current states.
	bool bPlayFlag = IsPlaying();
	bool bLoopFlag = IsLooping();
	DWORD dwPlay, dwWrite;
	m_pDSBuffer8->GetCurrentPosition(&dwPlay, &dwWrite);
	
	// then make sure to stop the sound buffer from playing
	m_pDSBuffer8->Stop();

	// unload old sound objects
	UnloadSoundData();

	if( !Load(m_pAMSoundEngine, pFileToLoad, m_strFileName) )
		return false;

	// restore old states.
	if( bPlayFlag )
	{
		bool bFadeIn = m_bFadeIn;
		bool bFadeOut = m_bFadeOut;
		Play(bLoopFlag);
		m_bFadeIn = bFadeIn;
		m_bFadeOut = bFadeOut;
		m_pDSBuffer8->SetCurrentPosition(dwPlay);
	}

	SetVolume(GetVolume());
	return true;
}

void AMSoundBuffer::SetRelativeVolume(DWORD dwVolume)
{
	m_dwRelativeVolume = dwVolume;
	SetVolume(dwVolume * m_pAMSoundEngine->GetVolume() / 100);
}

//	NEW_FREQUENCY = ORI_FREQUENCY * (2 ^ (Pitch / 12))
//	ORI_FREQUENCY is the wave file's SamplePerSec, it is very common to be 44100 or 22050 and so on.
//	iPitch value range is: [-12, 12]
void AMSoundBuffer::SetPitch(float fPitch)
{
	if (!m_pDSBuffer8)
		return;

	const float fInv12 = 1.f / 12.f;
	a_Clamp(fPitch, -12.f, 12.f);
	SetPlaySpeed(pow(2.0f, fPitch * fInv12));
}