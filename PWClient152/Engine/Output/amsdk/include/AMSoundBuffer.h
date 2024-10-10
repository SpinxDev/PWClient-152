/*
 * FILE: AMSoundBuffer.h
 *
 * DESCRIPTION: sound buffer class
 *
 * CREATED BY: Hedi, 2002/1/16
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _AMSOUNDBUFFER_H_
#define _AMSOUNDBUFFER_H_

#include "AMTypes.h"
#include "AString.h"
#include "AAssist.h"


//	Sound buffer type ID
enum
{
	AMSNDBUF_2D = 0,
	AMSNDBUF_3D,
};

class AMSoundEngine;
class AFile;
class AFileImage;

class AMSoundBuffer
{
	friend class AMSoundBufferMan;

protected:

	AMSoundEngine*			m_pAMSoundEngine;
	AMSBUFFERDESC			m_bufferDesc;
	IDirectSoundBuffer*		m_pDSBuffer;
	IDirectSoundNotify8*	m_pDSNotify8;
	IDirectSoundBuffer8*	m_pDSBuffer8;

	DWORD			m_dwTypeID;
	DWORD			m_dwBufferID;	//	Buffer ID, calculated by file name
	AString			m_strFileName;
	DWORD			m_dwStartTime;	//	a start time tag used in overtime check
	HANDLE			m_hOffsetStopEvent;

	bool			m_bLooping;
	AMSBSTATE		m_nState;

	DWORD			m_dwVolume;
	DWORD			m_dwRelativeVolume;  // relative volume, so every sound buffer can be adjusted separately

	bool			m_bFadeIn;		// flag indicate fade in
	bool			m_bFadeOut;		// flag indicate fade out
	DWORD			m_dwVolumeSet;	// used for fade in(out);
	float			m_fVolumeRate;	// volume change per second
	float			m_fVolumeDelta;	// volume change value accumulated

	bool			m_bLoaded;
	DWORD			m_dwPosInMan;	//	Position handle in AMSoundBufferMan

public:
	AMSoundBuffer();
	virtual ~AMSoundBuffer();

protected:
	// Create the sound buffer;
	virtual bool Create(AMSoundEngine* pAMSoundEngine, AMSBUFFERDESC * pDesc);
	virtual bool Release();
	bool DownSample(LPBYTE pDestBuf, LPBYTE pSrcBuf, WAVEFORMATEX wfx, int nSampleCount, DWORD dwDownRate);
	
public:
	// Create and load sound data from a file;
	// In:
	//		pFileToLoad	- the opened AMFile pointer;
	//		szFileName  - the file's name, include a relative path maybe;
	virtual bool Load(AMSoundEngine* pAMSoundEngine, AFile* pFileToLoad, const char* szFileName);
	virtual bool Save(AFile* pFileToSave);
	virtual bool Reload(AFile * pFileToLoad);

	// Use this method to free the sound resource when it will not used for sometime;
	virtual bool UnloadSoundData();

	virtual bool Play(bool bLoop);
	virtual bool Stop();
	virtual bool Pause();

	virtual bool CanHear() { return true; }

private:
	DWORD GetPlayPos();
	DWORD GetWritePos();

public:
	// update something;
	bool Tick(int nDeltaTime);

	// See if it has reached the end;
	virtual bool CheckEnd();

	bool FadeIn(int nFadeTime=3000);
	bool FadeOut(int nFadeTime=3000);
	bool FadeInRelatively(int nFadeTime=3000);

public:
	inline IDirectSoundBuffer * GetDSoundBuffer() { return m_pDSBuffer; }
	inline AMSBUFFERDESC * GetBufferDesc() { return &m_bufferDesc; }
	
	inline void UpdateStatus();
	inline bool IsPlaying();											// 当前时间是否播放
	inline bool IsPaused() { return m_nState == AMSBSTATE_PAUSED; }		// 只有手动调用过 Pause() 时才会返回true
	inline bool IsStopped() { return m_nState == AMSBSTATE_STOPPED; }	// 只有当没有进行 Play() 或者手动调用过 Stop() 时才会返回true
	// m_nState 只有用户或者内部函数调用 Pause() 和 Stop() 这两个函数时才会改变
	// IsPlaying() 可以取得音乐是否正在播放, 如果音乐播放完毕, m_nState 有可能是 AMSBSTATE_PLAYING.但此时 IsPlaying() 一定是 false

	inline bool IsFadeIn() { return m_bFadeIn; }
	inline bool IsFadeOut() { return m_bFadeOut; }
	inline AMSBSTATE GetState() { return m_nState; }
	inline const char * GetFileName() { return m_strFileName; }
	inline DWORD GetVolume() { return m_dwVolume; }
	inline DWORD GetTypeID() { return m_dwTypeID; }
	inline DWORD GetBufferID() { return m_dwBufferID; }
	
	inline void SetVolume(DWORD dwVolume) 
	{
		m_dwVolume = dwVolume;
		if( m_pDSBuffer8 )
		{
			if( m_dwVolume == 0 )
				m_pDSBuffer8->SetVolume(DSBVOLUME_MIN);
			else
				m_pDSBuffer8->SetVolume(AMSOUND_MINVOLUME + (AMSOUND_MAXVOLUME - AMSOUND_MINVOLUME) * m_dwVolume / 100);
		}
	}

	inline DWORD GetRelativeVolume() const { return m_dwRelativeVolume; }
	//	Set Relative Volume
	//	RelativeVolume would be considered when client use AMSoundEngine::SetRelativeVolume to set all AMSoundBuffers' volume
	//	And this would also consider the AMSoundEngine::GetVolume() when set its Volume in this function itself 
	//	(at this time, it take the AMSoundEngine_Volume as the relative volume)
	//	Volume = RelativeVolume * AMSoundEngine_Volume / 100
	void SetRelativeVolume(DWORD dwVolume);

	//	Another way to set play speed (use pitch in music)
	//	NEW_FREQUENCY = ORI_FREQUENCY * (2 ^ (Pitch / 12))
	//	ORI_FREQUENCY is the wave file's SamplePerSec, it is very common to be 44100 or 22050 and so on.
	//	iPitch value range is: [-12.0, 12.0]
	void SetPitch(float fPitch);

	inline void SetPlaySpeed(float fPlaySpeed)
	{
		if( m_pDSBuffer8 )
		{
			if( fPlaySpeed == 1.0f )
			{
				m_pDSBuffer8->SetFrequency(DSBFREQUENCY_ORIGINAL);
			}
			else
			{
				DWORD dwNewFrequency = DWORD(m_bufferDesc.wfxFormat.nSamplesPerSec * fPlaySpeed);
				dwNewFrequency = max2(DSBFREQUENCY_MIN, dwNewFrequency);
				dwNewFrequency = min2(DSBFREQUENCY_MAX, dwNewFrequency);
				m_pDSBuffer8->SetFrequency(dwNewFrequency);
			}
		}
	}

	inline bool IsLooping() { return m_bLooping; }
};

void AMSoundBuffer::UpdateStatus()
{
	DWORD dwStatus;
	if(FAILED(m_pDSBuffer->GetStatus(&dwStatus)))
	{
		m_nState = AMSBSTATE_STOPPED;
		return;
	}
	if((m_nState == AMSBSTATE_PLAYING) && (dwStatus & DSBSTATUS_PLAYING) == 0)
		m_nState = AMSBSTATE_STOPPED;
}

bool AMSoundBuffer::IsPlaying()
{
	DWORD dwStatus;
	if(m_pDSBuffer == NULL || FAILED(m_pDSBuffer->GetStatus(&dwStatus)))
		return false;
	return (dwStatus & DSBSTATUS_PLAYING) != 0;
}

typedef class AMSoundBuffer * PAMSoundBuffer;
#endif//_AMSOUNDBUFFER_H_