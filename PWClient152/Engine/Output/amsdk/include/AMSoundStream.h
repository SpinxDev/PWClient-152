/*
 * FILE: AMSoundStream.h
 *
 * DESCRIPTION: sound stream class
 *
 * CREATED BY: Hedi, 2002/1/16
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _AMSOUNDSTREAM_H_
#define _AMSOUNDSTREAM_H_

#include "AMTypes.h"
#include "AMPlatform.h"
#include "FFT.h"

class AMSoundEngine;
class AMWaveFile;
class AMMp3File;
class AMOggFile;
class AFile;

#define AMSS_MAX_BUFFER_NUM		256

typedef void (*LPDATAREADCALLBACK)(DWORD dwDataRead, void * pArg);

class AMSoundStream
{
private:
	AMSoundEngine*				m_pAMSoundEngine;
	AMSSTREAMDESC				m_streamDesc;

	CRITICAL_SECTION			m_csExit;
	bool						m_bQuit;

	bool						m_bHasInit;
	char						m_szStreamFile[MAX_PATH];

	IDirectSoundBuffer *		m_pDSBuffer;
	IDirectSoundNotify *		m_pDSNotify;

	int							m_nBufferLen;
	int							m_nBufferNum;

	int							m_nPlayedBufferNum;
	int							m_nLeftBufferNum;
	DWORD						m_dwBufferSize;
	bool						m_bEnd;
	HANDLE						m_hEvent[AMSS_MAX_BUFFER_NUM];

	AMSOUNDFILETYPE				m_fileType;
	AMWaveFile					*m_pWaveFile;
	AMMp3File					*m_pMp3File;
	AMOggFile					*m_pOggFile;

	AFile						*m_pAFile;

	AMSS_STATE					m_state;
	bool						m_bLoop;

	int							m_nGapToInsert;
	int							m_nGapToWait;
	int							m_nLastWaitTime;

	DWORD						m_dwVolume;

	int							m_nTotalSample;
	DWORD						m_dwDataSize;
	DWORD						m_dwPlayedSize;

	bool						m_bDynamicSpectrum;		// flag indicates whether or not we calculate the dynamic spectrum for the stream
	int							m_nDynamicSampleNum;	// number of samples in the buffer for calculate the spectrum
	float *						m_pDynamicBuffer;		// data for calculate the spectrum

	float *						m_pSpectrumBuffer[AMSS_MAX_BUFFER_NUM];		// data buffer that contains spectrum
	float						m_fDynamicLeft[AMSS_MAX_BUFFER_NUM];		// dynamic vumeter for left channel
	float						m_fDynamicRight[AMSS_MAX_BUFFER_NUM];		// dynamic vumeter for right channel

	CFFT						m_fft;					// fft utility

protected:
	
	bool Init_Internal(AMSoundEngine* pAMSoundEngine, AMSSTREAMDESC* pDesc);
	bool LoadStreamFromFile(const char* szStreamFile);
	bool LoadStreamFromOggData(AFile* pFile, DWORD dwDataLen);
	bool ReleaseResource();
	bool CreateStreamBuffer();
	bool Play_Internal();

	bool FillBuffer(int index);
	bool HandleNotifyMessage();

	bool UpdateSpectrum(int index, void * pBuffer);

public:
	AMSoundStream();
	virtual ~AMSoundStream();

	// Create the sound stream;
	bool Init(AMSoundEngine* pAMSoundEngine, const char* pszStreamFile, AMSSTREAMDESC * pDesc=NULL);
	bool InitFromOggData(AMSoundEngine* pAMSoundEngine, AFile* pFile, DWORD dwDataLen, AMSSTREAMDESC* pDesc=NULL);
	bool Release();
	bool ReadAllData(LPBYTE pBuffer, DWORD * pdwBufferSize, LPDATAREADCALLBACK pfnReadCallBack, void * pArg);

	bool Play(char * szStreamFile=NULL);
	bool PlayOggData(AFile* pFile, DWORD dwDataLen);
	bool Stop();
	bool Pause(bool bPause);
	bool SetPlaySpeed(float fSpeed);

	bool Seek(int nSample);
	int GetCurrentPos();

	bool Tick();
	void SetVolume(DWORD dwVolume);

	bool SetDynamicSpectrum(bool bFlag);
	inline bool GetDynamicSpectrum() { return m_bDynamicSpectrum; }
	float GetSpectrumForFreq(float vFreq);
	float GetDynamicLeft(); 
	float GetDynamicRight();

	friend DWORD WINAPI BgmHandle(LPVOID pArg);

	inline int GetTotalSample() { return m_nTotalSample; }
	inline DWORD GetDataSize() { return m_dwDataSize; }
	inline const AMSSTREAMDESC& GetStreamDesc() { return m_streamDesc; }
	inline int GetBufferLen() { return m_nBufferLen; }
	inline int GetBufferNum() { return m_nBufferNum; }
	inline bool IsPlaying() { return m_state == AMSS_PLAYING; }
	inline bool IsStopped() { return m_state == AMSS_STOPPED; }
	inline bool IsPaused() { return m_state == AMSS_PAUSED; }
	inline char * GetStreamFileName() { return m_szStreamFile; }
	inline bool GetLoopFlag() { return m_bLoop; }
	inline void SetLoopFlag(bool bLoop) { m_bLoop = bLoop; }
	inline void SetGapToInsert(int nGapToInsert) { m_nGapToInsert = nGapToInsert; }
	inline int GetGapToInsert() { return m_nGapToInsert; }
	inline DWORD GetVolume() { return m_dwVolume; }
};

typedef class AMSoundStream * PAMSoundStream;
#endif//_AMSOUNDSTREAM_H_
