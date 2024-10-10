/*
 * FILE: AMSoundStream.cpp
 *
 * DESCRIPTION: sound stream class
 *
 * CREATED BY: Hedi, 2002/1/16
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.
 */

#include "AMPI.h"
#include "AMSoundEngine.h"
#include "AMSoundDevice.h"
#include "AMSoundStream.h"
#include "AMWaveFile.h"
#include "AMMp3File.h"
#include "AMOggFile.h"
#include "AFile.h"
#include "AFileUnicode.h"
#include "AFileImage.h"
#include <fcntl.h>
#include <io.h>

#define SPECTRUM_NUMBER		1024

AMSoundStream::AMSoundStream()
{
	m_pAMSoundEngine	= NULL;
	m_pDSBuffer			= NULL;
	m_pDSNotify			= NULL;
	m_pWaveFile			= NULL;
	m_pMp3File			= NULL;
	m_pOggFile			= NULL;
	m_pAFile			= NULL;
	m_fileType			= AMS_UNKOWNFILE;
	m_state				= AMSS_STOPPED;

	m_bLoop				= true; // By default we must loop again and again;
	m_nGapToInsert		= 3000;
	m_nGapToWait		= 0;

	m_bHasInit			= false;
	m_bQuit				= false;

	ZeroMemory(&m_streamDesc, sizeof(m_streamDesc));
	ZeroMemory(m_hEvent, sizeof(HANDLE) * AMSS_MAX_BUFFER_NUM);
	m_szStreamFile[0]	= '\0';

	m_nTotalSample		= 0;
	m_dwDataSize		= 0;
	m_dwPlayedSize		= 0;

	m_bDynamicSpectrum	= false;
	m_pDynamicBuffer	= NULL;
	memset(m_pSpectrumBuffer, 0, sizeof(float *) * AMSS_MAX_BUFFER_NUM);
	memset(m_fDynamicLeft, 0, sizeof(float) * AMSS_MAX_BUFFER_NUM);
	memset(m_fDynamicRight, 0, sizeof(float) * AMSS_MAX_BUFFER_NUM);

	m_dwVolume			= 100;
}

AMSoundStream::~AMSoundStream()
{
}

// Create the sound buffer;
bool AMSoundStream::Init(AMSoundEngine* pAMSoundEngine, const char* pszStreamFile, AMSSTREAMDESC * pDesc)
{
	if (!Init_Internal(pAMSoundEngine, pDesc))
	{
		AMERRLOG(("AMSoundStream::Init(), failed to call Init_Internal !"));
		return false;
	}

	if( pszStreamFile )
	{
		if( !LoadStreamFromFile(pszStreamFile) )
		{
			AMERRLOG(("AMSoundStream::Init(), LoadStreamFromFile fail!"));
			return false;
		}
	}

	return true;
}

bool AMSoundStream::InitFromOggData(AMSoundEngine* pAMSoundEngine, AFile* pFile, DWORD dwDataLen, AMSSTREAMDESC* pDesc/* NULL */)
{
	if (!Init_Internal(pAMSoundEngine, pDesc))
	{
		AMERRLOG(("AMSoundStream::Init(), failed to call Init_Internal !"));
		return false;
	}

	if (pFile && dwDataLen)
	{
		if (!LoadStreamFromOggData(pFile, dwDataLen))
		{
			AMERRLOG(("AMSoundStream::InitFromOggData(), LoadStreamFromOggData fail!"));
			return false;
		}
	}

	return true;
}

bool AMSoundStream::Init_Internal(AMSoundEngine* pAMSoundEngine, AMSSTREAMDESC* pDesc)
{
	m_pAMSoundEngine = pAMSoundEngine;
	if( pDesc )
	{
		m_streamDesc.dwBufferLen = pDesc->dwBufferLen;
		m_streamDesc.dwBufferNum = pDesc->dwBufferNum;
	}
	else
	{
		m_streamDesc.dwBufferLen = 60;
		m_streamDesc.dwBufferNum = 10;
	}
	m_nBufferLen = m_streamDesc.dwBufferLen; //ms
	m_nBufferNum = m_streamDesc.dwBufferNum;
	if( m_nBufferNum > AMSS_MAX_BUFFER_NUM )
	{
		AMERRLOG(("AMSoundStream::Init_Internal(), buffer number must be less than %d", AMSS_MAX_BUFFER_NUM));
		return false;
	}

	int					i;
	for(i=0; i<m_nBufferNum; i++)
	{
		m_hEvent[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
		ResetEvent(m_hEvent[0]);
	}

	InitializeCriticalSection(&m_csExit);

	// If we did not create direct sound object;
	if( NULL == m_pAMSoundEngine->GetDS() )
	{
		// Set init flag to false, and set state to playing, so this will be playing for ever;
		m_bHasInit = false;
		m_state = AMSS_PLAYING;
		return true;
	}


	if( !m_fft.Init(SPECTRUM_NUMBER) )
	{
		AMERRLOG(("AMSoundStream::Init_Internal(), Init fft utility fail!"));
		return false;
	}

	return true;
}

bool AMSoundStream::Release()
{
	m_fft.Release();

	if( m_pDynamicBuffer )
	{
		delete [] m_pDynamicBuffer;
		m_pDynamicBuffer = NULL;
	}

	ReleaseResource();

	DeleteCriticalSection(&m_csExit);
	for(int i=0; i<m_nBufferNum; i++)
	{
		if( m_hEvent[i] )
		{
			CloseHandle(m_hEvent[i]);
			m_hEvent[i] = NULL;
		}

		if( m_pSpectrumBuffer[i] )
		{
			delete [] m_pSpectrumBuffer[i];
			m_pSpectrumBuffer[i] = NULL;
		}
	}
	return true;
}

bool AMSoundStream::Play(char * pszStreamFile)
{
	if( pszStreamFile && 0 != _stricmp(m_szStreamFile, pszStreamFile) )
	{
		// Now we need to change a stream file;
		ReleaseResource();
		if( !LoadStreamFromFile(pszStreamFile) )
		{
			AMERRLOG(("AMSoundStream::Play(), Call LoadStreamFromFile [%s] again fail!", pszStreamFile));
			return false;
		}
	}
	else
	{
		// we must stop current bgm handle thread here
		Stop();
	}

	if (!Play_Internal())
	{
		AMERRLOG(("AMSoundStream::Play(), Play_Internal fail!"));
		return false;
	}

	return true;
}

bool AMSoundStream::PlayOggData(AFile* pFile/* NULL */, DWORD dwDataLen/* 0 */)
{
	if (pFile && dwDataLen && 0 != _stricmp(m_szStreamFile, pFile->GetFileName()))
	{
		// Now we need to change a stream file;
		ReleaseResource();
		if (!LoadStreamFromOggData(pFile, dwDataLen))
		{
			AMERRLOG(("AMSoundStream::PlayOggData(), LoadStreamFromOggData fail!"));
			return false;
		}
	}
	else
	{
		// we must stop current bgm handle thread here
		Stop();
	}

	if (!Play_Internal())
	{
		AMERRLOG(("AMSoundStream::PlayOggData(), Play_Internal fail!"));
		return false;
	}

	return true;
}

bool AMSoundStream::Play_Internal()
{
	if( !m_bHasInit )
		return true;

	if( m_fileType == AMS_WAVEFILE )
		m_pWaveFile->Seek(0);
	else if( m_fileType == AMS_MP3FILE )
		m_pMp3File->Seek(0);
	else if( m_fileType == AMS_OGGFILE )
		m_pOggFile->Seek(0);

	m_nLeftBufferNum = 0;
	m_nPlayedBufferNum = 0;
	m_dwPlayedSize	= 0;
	m_bEnd = false;

	for(int i=0; i<m_nBufferNum; i++)
	{
		if( !FillBuffer(i) )
		{
			AMERRLOG(("AMSoundStream::Play_Internal(), Fill Buffer [%d] fail!", i));
			return false;
		}
		ResetEvent(m_hEvent[0]);
	}

	HANDLE hThread;
	DWORD  dwThreadID;
	m_bQuit = false;

	hThread = CreateThread(NULL, 0, BgmHandle, this, 0, &dwThreadID);

	SetThreadPriority(hThread, THREAD_PRIORITY_TIME_CRITICAL);
	CloseHandle(hThread);

	m_pDSBuffer->SetCurrentPosition(0);
	m_pDSBuffer->Play(0, 0, DSBPLAY_LOOPING);

	m_state = AMSS_PLAYING;

	return true;
}

bool AMSoundStream::Stop()
{
	if( !m_bHasInit )
		return true;

	m_pDSBuffer->Stop();
	m_pDSBuffer->SetCurrentPosition(0);

	m_bQuit = true;
	EnterCriticalSection(&m_csExit);
	LeaveCriticalSection(&m_csExit);

	if( m_fileType == AMS_WAVEFILE )
		m_pWaveFile->Seek(0);
	else if( m_fileType == AMS_MP3FILE )
		m_pMp3File->Seek(0);
	else if( m_fileType == AMS_OGGFILE )
		m_pOggFile->Seek(0);

	for(int i=0; i<m_nBufferNum; i++)
		ResetEvent(m_hEvent[i]);

	m_nLeftBufferNum = 0;
	m_nPlayedBufferNum = 0;
	m_dwPlayedSize = 0;
	m_nGapToWait = 0;
	m_state = AMSS_STOPPED;
	return true;
}

bool AMSoundStream::Pause(bool bPause)
{
	if( !m_bHasInit )
		return true;

	if( IsStopped() )
		return true;

	if( bPause )
	{
		m_pDSBuffer->Stop();
		m_state = AMSS_PAUSED;
	}
	else
	{
		m_pDSBuffer->Play(0, 0, DSBPLAY_LOOPING);
		m_state = AMSS_PLAYING;
	}
		
	return true;
}

DWORD WINAPI BgmHandle(LPVOID pArg)
{
	AMSoundStream * pThis = (AMSoundStream *) pArg;

	EnterCriticalSection(&pThis->m_csExit);
	
	while( !pThis->m_bQuit )
	{
		if( !pThis->HandleNotifyMessage() )
			break;
		//Sleep(pThis->GetBufferLen() / 2);
		Sleep(10);
	}

	LeaveCriticalSection(&pThis->m_csExit);
	return 0x88;
}

bool AMSoundStream::FillBuffer(int index)
{
	DWORD				dwStart, dwLen, dwReadLen;
	DWORD				dwBytes1, dwBytes2;
	LPVOID				psndBuffer;
	LPBYTE				pFillBuffer;

	dwStart = index * m_dwBufferSize;
	dwLen	= m_dwBufferSize;
	if (FAILED(m_pDSBuffer->Lock(dwStart, dwLen, &psndBuffer, &dwBytes1, NULL, &dwBytes2, NULL))) 
	{
		AMERRLOG(("AMSoundStream::FillBuffer(), Lock sound buffer fail!"));
		return false;
	}

	if( dwBytes1 < dwLen )
	{
		AMERRLOG(("AMSoundStream::FillBuffer(), Lock result error!"));
		return false;
	}

	if( m_bEnd && !m_bLoop )
	{
		//Insert solince here;
		ZeroMemory(psndBuffer, dwLen);
		goto UNLOCKDATA;
	}

	pFillBuffer = (LPBYTE) psndBuffer;
FILLMORE:
	if( m_fileType == AMS_WAVEFILE )
	{
		if( !m_pWaveFile->ReadData(pFillBuffer, dwLen, &dwReadLen, &m_bEnd) )
		{
			AMERRLOG(("AMSoundStream::FillBuffer(), read wave file data fail!"));
			return false;
		}
	}
	else if( m_fileType == AMS_MP3FILE )
	{
		if( !m_pMp3File->ReadData(pFillBuffer, dwLen, &dwReadLen, &m_bEnd) )
		{
			AMERRLOG(("AMSoundStream::FillBuffer(), read mp3 file data fail!"));
			return false;
		}
	}
	else if( m_fileType == AMS_OGGFILE )
	{
		if( !m_pOggFile->ReadData(pFillBuffer, dwLen, &dwReadLen, &m_bEnd) )
		{
			AMERRLOG(("AMSoundStream::FillBuffer(), read ogg file data fail!"));
			return false;
		}
	}

	if( m_bEnd )
	{
		if( m_bLoop )
		{
			if( m_fileType == AMS_WAVEFILE )
			{
				if( !m_pWaveFile->Seek(0) )
				{
					Stop();
					AMERRLOG(("AMSoundStream::FillBuffer(), failed to seek to wave file head!"));
					return false;
				}
			}
			else if( m_fileType == AMS_MP3FILE )
			{
				if( !m_pMp3File->Seek(0) )
				{
					Stop();
					AMERRLOG(("AMSoundStream::FillBuffer(), failed to seek to mp3 file head!"));
					return false;
				}
			}
			else if( m_fileType == AMS_OGGFILE )
			{
				if( !m_pOggFile->Seek(0) )
				{
					Stop();
					AMERRLOG(("AMSoundStream::FillBuffer(), failed to seek to ogg file head!"));
					return false;
				}
			}

			if( dwReadLen < dwLen )
			{
				dwLen -= dwReadLen;
				pFillBuffer += dwReadLen;
				goto FILLMORE;
			}
		}
		else
		{
			if( dwReadLen < dwLen )
			{
				ZeroMemory((LPBYTE)psndBuffer + dwReadLen, dwLen - dwReadLen);
			}
		}
	}
	
	m_nLeftBufferNum ++;

	if( m_bDynamicSpectrum )
	{
		UpdateSpectrum(index, psndBuffer);
	}

UNLOCKDATA:
	if (FAILED(m_pDSBuffer->Unlock(psndBuffer, dwBytes1, NULL, 0)))
	{
		AMERRLOG(("AMSoundStream::FillBuffer(), Unlock dsound buffer fail!"));
		return false;
	}
	return true;
}

bool AMSoundStream::HandleNotifyMessage()
{
	DWORD	dwRet; 
	int		idEvent = m_nPlayedBufferNum % m_nBufferNum;

	dwRet = WaitForSingleObject(m_hEvent[0], 0);
	if( dwRet == WAIT_TIMEOUT )
		return true;
	else
	{
		// see if the sound stream has gone out of sync.
		DWORD dwStart = idEvent * m_dwBufferSize;
		DWORD dwEnd = dwStart + m_dwBufferSize;

		DWORD dwPlayPos;
		DWORD dwWritePos;

CHECKAGAIN:
		if( DS_OK == m_pDSBuffer->GetCurrentPosition(&dwPlayPos, &dwWritePos) )
		{
			// see if loop
			if( dwPlayPos > dwWritePos )
				dwWritePos += m_dwBufferSize * m_nBufferNum;

			if( dwStart < dwWritePos && dwEnd > dwPlayPos )
			{
				Sleep(m_nBufferLen >> 1);
				goto CHECKAGAIN;
			}
		}

		m_nPlayedBufferNum ++;
		m_dwPlayedSize += m_dwBufferSize;

		ResetEvent(m_hEvent[0]);
		m_nLeftBufferNum --;
		if( m_nLeftBufferNum == 0 )
		{
			// we should stop here;
			m_pDSBuffer->Stop();
			m_pDSBuffer->SetCurrentPosition(0);

			if( m_fileType == AMS_WAVEFILE )
				m_pWaveFile->Seek(0);
			else if( m_fileType == AMS_MP3FILE )
				m_pMp3File->Seek(0);
			else if( m_fileType == AMS_OGGFILE )
				m_pOggFile->Seek(0);

			for(int i=0; i<m_nBufferNum; i++)
				ResetEvent(m_hEvent[0]);

			m_nLeftBufferNum = 0;
			m_nPlayedBufferNum = 0;
			m_dwPlayedSize = 0;
			m_state = AMSS_ENDTRACK;
			return false;
		}

		if( !FillBuffer(idEvent) )
		{
			AMERRLOG(("AMSoundStream::HandleNotifyMessage(), FillBuffer fail!"));
			return false;
		}
	}

	return true;
}

bool AMSoundStream::LoadStreamFromFile(const char* szStreamFile)
{
	char szFileNameLwr[MAX_PATH];

	strncpy(m_szStreamFile, szStreamFile, MAX_PATH);

	// If we did not create the direct sound object;
	if( NULL == m_pAMSoundEngine->GetDS() )
		return true;

	strncpy(szFileNameLwr, szStreamFile, MAX_PATH);
	_strlwr(szFileNameLwr);

	if( strstr(szFileNameLwr, ".wav") )
	{
		m_fileType = AMS_WAVEFILE;

		// We use wave file here;
		if( 0 == _access(szStreamFile, 0) )
			m_pAFile = new AFile();
		else if( AFileUnicode::IsFileUnicodeExist(szStreamFile) )
			m_pAFile = new AFileUnicode();
		else
			m_pAFile = new AFileImage();

		if( NULL == m_pAFile )
		{
			AMERRLOG(("AMSoundStream::LoadStreamFromFile(), Not enough memory!"));
			return false;
		}
		if( !m_pAFile->Open(szStreamFile, AFILE_OPENEXIST) )
		{
			m_pAFile->Close();
			delete m_pAFile;
			m_pAFile = NULL;
			AMERRLOG(("AMSoundStream::LoadStreamFromFile(), Can not open file [%s]!", szStreamFile));
			return false;
		}

		m_pWaveFile = new AMWaveFile();
		if( NULL == m_pWaveFile )
		{
			m_pAFile->Close();
			delete m_pAFile;
			m_pAFile = NULL;
			AMERRLOG(("AMSoundStream::LoadStreamFromFile(), Not enough memory!"));
			return false;
		}
		if( !m_pWaveFile->Open(m_pAFile) )
		{
			m_pAFile->Close();
			delete m_pAFile;
			m_pAFile = NULL;
			m_pWaveFile->Close();
			delete m_pWaveFile;
			m_pWaveFile = NULL;
			AMERRLOG(("AMSoundStream::LoadStreamFromFile(), Can not open [%s] as a wave file!", szStreamFile));
			return false;
		}
		m_streamDesc.wfxFormat = m_pWaveFile->GetWaveformatEx();
		m_nTotalSample = m_pWaveFile->GetTotalSample();
		m_dwDataSize = m_pWaveFile->GetDataSize();
	}
	else if( strstr(szFileNameLwr, ".mp3") )
	{
		m_fileType = AMS_MP3FILE;

		// We use mp3 file here;
		m_pMp3File = new AMMp3File();
		if( NULL == m_pMp3File )
		{
			AMERRLOG(("AMSoundStream::LoadStreamFromFile(), Not enough memory!"));
			return false;
		}
		if( !m_pMp3File->Open(szStreamFile) )
		{
			delete m_pMp3File;
			m_pMp3File = NULL;
			AMERRLOG(("AMSoundStream::LoadStreamFromFile(), Can not open [%s] as a mp3 file!", szStreamFile));
			return false;
		}
		m_streamDesc.wfxFormat = m_pMp3File->GetWaveformatEx();
		m_nTotalSample = m_pMp3File->GetTotalSample();
		m_dwDataSize = m_pMp3File->GetDataSize();
	}
	else if( strstr(szFileNameLwr, ".ogg") )
	{
		m_fileType = AMS_OGGFILE;

		// We use ogg file here;
		m_pOggFile = new AMOggFile();
		if( NULL == m_pOggFile )
		{
			AMERRLOG(("AMSoundStream::LoadStreamFromFile(), Not enough memory!"));
			return false;
		}
		if( !m_pOggFile->Open(szStreamFile) )
		{
			m_pOggFile->Close();
			delete m_pOggFile;
			m_pOggFile = NULL;
			AMERRLOG(("AMSoundStream::LoadStreamFromFile(), Can not open [%s] as a ogg file!", szStreamFile));
			return false;
		}
		m_streamDesc.wfxFormat = m_pOggFile->GetWaveformatEx();
		m_nTotalSample = m_pOggFile->GetTotalSample();
		m_dwDataSize = m_pOggFile->GetDataSize();
	}

	//	Create the dsound buffer
	if (!CreateStreamBuffer())
	{
		AMERRLOG(("AMSoundStream::LoadStreamFromFile(), CreateStreamBuffer fail!"));
		return false;
	}

	return true;
}

bool AMSoundStream::LoadStreamFromOggData(AFile* pFile, DWORD dwDataLen)
{
	if (!pFile || !dwDataLen || dwDataLen > pFile->GetFileLength())
		return false;

	strncpy(m_szStreamFile, pFile->GetFileName(), MAX_PATH);

	// If we did not create the direct sound object;
	if (NULL == m_pAMSoundEngine->GetDS())
		return true;

	m_fileType = AMS_OGGFILE;

	// We use ogg file here;
	m_pOggFile = new AMOggFile();
	if( NULL == m_pOggFile )
	{
		AMERRLOG(("AMSoundStream::LoadStreamFromOggData(), Not enough memory!"));
		return false;
	}
	if( !m_pOggFile->Open(pFile, dwDataLen) )
	{
		delete m_pOggFile;
		m_pOggFile = NULL;
		AMERRLOG(("AMSoundStream::LoadStreamFromOggData(), Can not open ogg file!"));
		return false;
	}

	m_streamDesc.wfxFormat = m_pOggFile->GetWaveformatEx();
	m_nTotalSample = m_pOggFile->GetTotalSample();
	m_dwDataSize = m_pOggFile->GetDataSize();

	//	Create the dsound buffer
	if (!CreateStreamBuffer())
	{
		AMERRLOG(("AMSoundStream::LoadStreamFromOggData(), CreateStreamBuffer fail!"));
		return false;
	}

	return true;
}

bool AMSoundStream::CreateStreamBuffer()
{
	int					i;
	DSBPOSITIONNOTIFY	dspn[AMSS_MAX_BUFFER_NUM];

	m_dwBufferSize = m_nBufferLen * m_streamDesc.wfxFormat.nAvgBytesPerSec / 1000;
	ASSERT(m_dwBufferSize * 1000 == m_nBufferLen * m_streamDesc.wfxFormat.nAvgBytesPerSec);

	HRESULT			hval;
	DSBUFFERDESC	dsbd;
	ZeroMemory(&dsbd, sizeof(DSBUFFERDESC));
	dsbd.dwSize = sizeof(DSBUFFERDESC); 
    dsbd.dwFlags = DSBCAPS_LOCSOFTWARE | DSBCAPS_CTRLPOSITIONNOTIFY | DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY;
	if( m_pAMSoundEngine->GetAMSoundDevice()->GetDeviceFormat().bGlobalFocus )
		dsbd.dwFlags |= DSBCAPS_GLOBALFOCUS;
    dsbd.dwBufferBytes = m_nBufferNum * m_dwBufferSize;
    dsbd.lpwfxFormat = &m_streamDesc.wfxFormat; 
	
	hval = m_pAMSoundEngine->GetDS()->CreateSoundBuffer(&dsbd, &m_pDSBuffer, NULL);
	if( hval != DS_OK )
	{
		AMERRLOG(("AMSoundStream::CreateStreamBuffer(), CreateSoundBuffer fail!"));
		return false;
	}

	hval = m_pDSBuffer->QueryInterface(IID_IDirectSoundNotify, (LPVOID *)&m_pDSNotify);
	if( hval != DS_OK )
	{
		AMERRLOG(("AMSoundStream::CreateStreamBuffer(), Query DSNotify fail!"));
		return false;
	}

	for(i=0; i<m_nBufferNum; i++)
	{
		ResetEvent(m_hEvent[0]);

		dspn[i].dwOffset = (i + 1) * m_dwBufferSize - 1;
		dspn[i].hEventNotify = m_hEvent[0];
	}

	hval = m_pDSNotify->SetNotificationPositions(m_nBufferNum, dspn);
	if( hval != DS_OK )
	{
		AMERRLOG(("AMSoundStream::CreateStreamBuffer(), SetNotificationPositions Fail!"));
		return false;
	}

	m_state = AMSS_STOPPED;

	m_bHasInit = true;
	SetVolume(m_dwVolume);

	return true;
}

bool AMSoundStream::ReleaseResource()
{
	Stop();

	if( m_pWaveFile )
	{
		m_pWaveFile->Close();
		delete m_pWaveFile;
		m_pWaveFile = NULL;
	}
	if( m_pAFile )
	{
		m_pAFile->Close();
		delete m_pAFile;
		m_pAFile = NULL;
	}
	if( m_pMp3File )
	{
		m_pMp3File->Close();
		delete m_pMp3File;
		m_pMp3File = NULL;
	}
	if( m_pOggFile )
	{
		m_pOggFile->Close();
		delete m_pOggFile;
		m_pOggFile = NULL;
	}
	if( m_pDSNotify )
	{
		m_pDSNotify->Release();
		m_pDSNotify = NULL;
	}
	if( m_pDSBuffer )
	{
		m_pDSBuffer->Stop();
		m_pDSBuffer->Release();
		m_pDSBuffer = NULL;
	}

	m_fileType  = AMS_UNKOWNFILE;
	m_state		= AMSS_STOPPED;

	m_bHasInit = false;
	return true;
}

bool AMSoundStream::Tick()
{
	// Here we should do something on stop;
	// For example, we should check if there is some queued stream or loop again?
	if( m_state == AMSS_ENDTRACK )
	{
		// Just end of playing one track, so check if we need to play another track;
		if( m_bLoop )
		{
			/*
			m_nGapToWait = m_nGapToInsert;
			m_nLastWaitTime = GetTickCount();
			m_state = AMSS_BEGINTRACK;
			*/
		}
		else
			return Stop();
	}
	else if( m_state == AMSS_BEGINTRACK )
	{
		m_nGapToWait -= GetTickCount() - m_nLastWaitTime;
		m_nLastWaitTime = GetTickCount();

		if( m_nGapToWait <= 0 )
		{
			if( !Play() )
			{
				AMERRLOG(("AMSoundStream::Tick() Replay the stream again!"));
				return false;
			}
		}
	}

	return true;
}

void AMSoundStream::SetVolume(DWORD dwVolume)
{
	m_dwVolume = dwVolume;
	if( m_pDSBuffer )
	{
		if( m_dwVolume == 0 )
			m_pDSBuffer->SetVolume(DSBVOLUME_MIN);
		else
			m_pDSBuffer->SetVolume(AMSOUND_MINVOLUME + (AMSOUND_MAXVOLUME - AMSOUND_MINVOLUME) * m_dwVolume / 100);
	}
}

bool AMSoundStream::Seek(int nSample)
{
	// we can only seek while the stream is currently playing
	if( !IsPlaying() )
		return false;

	// first pause the stream before we can seek
	Pause(true);
	
	if( m_fileType == AMS_WAVEFILE )
	{
		if( !m_pWaveFile->Seek(nSample) )
		{
			AMERRLOG(("AMSoundStream::Seek(), seek mp3 file fail!"));
			return false;
		}
	}
	else if( m_fileType == AMS_MP3FILE )
	{
		if( !m_pMp3File->Seek(nSample) )
		{
			AMERRLOG(("AMSoundStream::Seek(), seek mp3 file fail!"));
			return false;
		}
	}
	else if( m_fileType == AMS_OGGFILE )
	{
		if( !m_pOggFile->Seek(nSample) )
		{
			AMERRLOG(("AMSoundStream::Seek(), seek ogg file fail!"));
			return false;
		}
	}

	m_nLeftBufferNum = 0;
	m_nPlayedBufferNum = 0;
	m_dwPlayedSize = nSample * m_streamDesc.wfxFormat.nBlockAlign;
	m_bEnd = false;

	for(int i=0; i<m_nBufferNum; i++)
	{
		if( !FillBuffer(i) )
		{
			AMERRLOG(("AMSoundStream::Play(), Fill Buffer [%d] fail!", i));
			return false;
		}
		ResetEvent(m_hEvent[0]);
	}
	m_pDSBuffer->SetCurrentPosition(0);

	Pause(false);
	return true;
}

bool AMSoundStream::ReadAllData(LPBYTE pBuffer, DWORD * pdwBufferSize, LPDATAREADCALLBACK pfnReadCallBack, void * pArg)
{
	// stop can seek the stream to the head of the stream
	Stop();

	DWORD dwReadLen = 0;

	if( *pdwBufferSize > m_dwDataSize )
	{
		*pdwBufferSize = 0;
		return false;
	}

	bool bEnd = false;
	if( m_fileType == AMS_WAVEFILE )
	{
		DWORD dwOneRead = 0;
		while(!bEnd && dwReadLen < *pdwBufferSize)
		{
			DWORD dwToRead = 4096;
			if( dwToRead + dwReadLen > *pdwBufferSize )
				dwToRead = *pdwBufferSize - dwReadLen;
			if( !m_pWaveFile->ReadData(pBuffer + dwReadLen, dwToRead, &dwOneRead, &bEnd) )
			{
				AMERRLOG(("AMSoundStream::FillBuffer(), read wave file data fail!"));
				return false;
			}

			dwReadLen += dwOneRead;
			if(pfnReadCallBack)
				(*pfnReadCallBack)(dwReadLen, pArg);
		}
	}
	else if( m_fileType == AMS_MP3FILE )
	{
		DWORD dwOneRead = 0;
		while(!bEnd && dwReadLen < *pdwBufferSize)
		{
			DWORD dwToRead = 4096;
			if( dwToRead + dwReadLen > *pdwBufferSize )
				dwToRead = *pdwBufferSize - dwReadLen;
			if( !m_pMp3File->ReadData(pBuffer + dwReadLen, dwToRead, &dwOneRead, &bEnd) )
			{
				AMERRLOG(("AMSoundStream::FillBuffer(), read wave file data fail!"));
				return false;
			}

			dwReadLen += dwOneRead;
			if(pfnReadCallBack)
				(*pfnReadCallBack)(dwReadLen, pArg);
		}
	}
	else if( m_fileType == AMS_OGGFILE )
	{
		DWORD dwOneRead = 0;
		while(!bEnd && dwReadLen < *pdwBufferSize)
		{
			DWORD dwToRead = 4096;
			if( dwToRead + dwReadLen > *pdwBufferSize )
				dwToRead = *pdwBufferSize - dwReadLen;
			if( !m_pOggFile->ReadData(pBuffer + dwReadLen, dwToRead, &dwOneRead, &bEnd) )
			{
				AMERRLOG(("AMSoundStream::FillBuffer(), read ogg file data fail!"));
				return false;
			}

			dwReadLen += dwOneRead;
			if(pfnReadCallBack)
				(*pfnReadCallBack)(dwReadLen, pArg);
		}
	}
	// Last seek to beginning of the stream again by calling stop;
	Stop();

	*pdwBufferSize = dwReadLen;
	return true;
}

int AMSoundStream::GetCurrentPos()
{
	DWORD dwPlayPos = 0;
	DWORD dwWritePos;
	if( DS_OK == m_pDSBuffer->GetCurrentPosition(&dwPlayPos, &dwWritePos) )
		dwPlayPos %= m_dwBufferSize;
	
	return (m_dwPlayedSize + dwPlayPos) / m_streamDesc.wfxFormat.nBlockAlign;
}

bool AMSoundStream::SetDynamicSpectrum(bool bFlag)
{
	// we can not change the dynamic spectrum state while playing the music, because that will need extra synchronization
	if( IsPlaying() )
		return false;

	if( m_bDynamicSpectrum == bFlag )
		return true;

	if( bFlag )
	{
		m_nDynamicSampleNum = SPECTRUM_NUMBER;
		m_pDynamicBuffer = new float[m_nDynamicSampleNum];

		for(int i=0; i<m_nBufferNum; i++)
		{
			m_pSpectrumBuffer[i] = new float[m_nDynamicSampleNum];
			memset(m_pSpectrumBuffer[i], 0, sizeof(float) * m_nDynamicSampleNum);
		}

		m_bDynamicSpectrum = true;
	}
	else
	{
		if( m_pDynamicBuffer )
		{
			delete [] m_pDynamicBuffer;
			m_pDynamicBuffer = NULL;
		}

		for(int i=0; i<m_nBufferNum; i++)
		{
			if( m_pSpectrumBuffer[i] )
			{
				delete [] m_pSpectrumBuffer[i];
				m_pSpectrumBuffer[i] = NULL;
			}
		}

		m_nDynamicSampleNum = 0;
		m_bDynamicSpectrum = false;
	}
	
	return true;
}

float AMSoundStream::GetSpectrumForFreq(float vFreq)
{
	if( !m_bDynamicSpectrum || m_streamDesc.wfxFormat.nSamplesPerSec == 0 )
		return 0.0f;

	int index = (int)(vFreq / (m_streamDesc.wfxFormat.nSamplesPerSec / m_nDynamicSampleNum) + 0.5f);

	a_Clamp(index, 0, m_nDynamicSampleNum - 1);

	int indexBuffer = m_nPlayedBufferNum % m_nBufferNum;
	return m_pSpectrumBuffer[indexBuffer][index];
}

float AMSoundStream::GetDynamicLeft()
{
	int indexBuffer = m_nPlayedBufferNum % m_nBufferNum;
	return m_fDynamicLeft[indexBuffer];
}

float AMSoundStream::GetDynamicRight()
{
	int indexBuffer = m_nPlayedBufferNum % m_nBufferNum;
	return m_fDynamicRight[indexBuffer];
}

bool AMSoundStream::UpdateSpectrum(int index, void * pBuffer)
{
	bool bStereo = false;
	bool b8Bit = false;

	if( m_streamDesc.wfxFormat.nChannels == 2 )
		bStereo = true;
	else
		bStereo = false;
	if( m_streamDesc.wfxFormat.wBitsPerSample == 8 )
		b8Bit = true;
	else
		b8Bit = false;

	BYTE * pBufferByte = (BYTE *) pBuffer;
	short int * pBufferWord = (short int *) pBuffer;

	float fDynamicLeft = 0.0f;
	float fDynamicRight = 0.0f;

	int nSampleBuffer = m_nBufferLen * m_streamDesc.wfxFormat.nSamplesPerSec / 1000;

	// Copy the front part of the old buffer if one sound buffer is not long enough for spectrum analysis.
	int nLeftSample = 0;
	if( m_nDynamicSampleNum > (int)nSampleBuffer )
	{
		nLeftSample = m_nDynamicSampleNum - nSampleBuffer;
		memcpy(m_pDynamicBuffer, m_pDynamicBuffer + nSampleBuffer, sizeof(float) * nLeftSample);
	}

	int nSampleValueLeft, nSampleValueRight;
	int nSampleValue;
	int nOrgID = 0;
	for(int i=0; i<nSampleBuffer; i++ )
	{
		if( b8Bit == 1 )
		{
			if( bStereo )
			{
				nSampleValueLeft = pBufferByte[nOrgID];
				nSampleValueRight = pBufferByte[nOrgID + 1];
				nSampleValue = nSampleValueLeft / 2 + nSampleValueRight / 2;
				nOrgID += 2;
			}
			else //Mono;
			{
				nSampleValue = pBufferByte[nOrgID];
				nSampleValueLeft = nSampleValueRight = nSampleValue;
				nOrgID ++;
			}
		}
		else//16-bit sampling;
		{
			if( bStereo )
			{
				nSampleValueLeft = pBufferWord[nOrgID];
				nSampleValueRight = pBufferWord[nOrgID + 1];
				nSampleValue = nSampleValueLeft / 2 + nSampleValueRight / 2;
				nOrgID += 2;
			}
			else
			{
				nSampleValue = pBufferWord[nOrgID];
				nSampleValueLeft = nSampleValueRight = nSampleValue;
				nOrgID ++;
			}
		}

		if( i + nLeftSample < m_nDynamicSampleNum )
		{
			if( !b8Bit )
			{
				m_pDynamicBuffer[i + nLeftSample] = ((float)nSampleValue) / 0x7fff;
				fDynamicLeft += (float)(fabs((float)nSampleValueLeft) / 0x7fff);
				fDynamicRight += (float)(fabs((float)nSampleValueRight) / 0x7fff);
			}
			else 
			{
				m_pDynamicBuffer[i + nLeftSample] = ((float)nSampleValue - 0x80) / 0x80;
				fDynamicLeft += (float)(fabs((float)(nSampleValueLeft - 0x80)) / 0x80);
				fDynamicRight += (float)(fabs((float)(nSampleValueRight - 0x80)) / 0x80);
			}
		}
	}

	m_fDynamicLeft[index] = fDynamicLeft / nSampleBuffer;
	m_fDynamicRight[index] = fDynamicRight / nSampleBuffer;
	
	if( !m_fft.SetBuffer(m_pDynamicBuffer, m_nDynamicSampleNum) )
		return false;
	if( !m_fft.FFT() )
		return false; 
	if( !m_fft.GetBuffer(m_pSpectrumBuffer[index], m_nDynamicSampleNum) )
		return false;

	return true;
}

bool AMSoundStream::SetPlaySpeed(float fSpeed)
{
	if( m_pDSBuffer )
	{
		if( fSpeed == 1.0f )
		{
			m_pDSBuffer->SetFrequency(DSBFREQUENCY_ORIGINAL);
		}
		else
		{
			DWORD dwNewFrequency = DWORD(m_streamDesc.wfxFormat.nSamplesPerSec * fSpeed);
			dwNewFrequency = max2(DSBFREQUENCY_MIN, dwNewFrequency);
			dwNewFrequency = min2(DSBFREQUENCY_MAX, dwNewFrequency);
			m_pDSBuffer->SetFrequency(dwNewFrequency);
		}
	}

	return true;
}
