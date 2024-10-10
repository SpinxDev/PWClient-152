/*
* FILE: A3DGFXSoundImp.h
*
* DESCRIPTION: 
*
* CREATED BY: Zhangyachuan, 2011/7/27
*
* HISTORY: 
*
* Copyright (c) 2011 Archosaur Studio, All Rights Reserved.
*/

#ifndef _A3DGFXSoundImp_H_
#define _A3DGFXSoundImp_H_


///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////
#include "CriticalSectionWrapper.h"

class RandStringContainer;
class A3DGFXSound;
namespace AudioEngine
{
	class EventInstance;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	class IGFXSOUNDIMP
//	
///////////////////////////////////////////////////////////////////////////

class IGFXSOUNDIMP
{
public:
	virtual bool Load(AFile* pFile, DWORD dwVersion) = 0;
	virtual bool Save(AFile* pFile) = 0;
	virtual void PrePlay() = 0;
	virtual void InitData() = 0;
	virtual void PostPlay(A3DGFXSound* pSound) = 0;
	virtual void StopSound() = 0;
	virtual void Stop(A3DGFXSound* pSound) = 0;
	virtual void ReleaseSound(A3DGFXSound* pSound) = 0;
	virtual void ClearSoundFile() = 0;
	virtual bool TickSound(A3DGFXSound* pSound, DWORD dwTickTime) = 0;
	virtual void ResumeLoop() = 0;
	virtual bool IsValid() const = 0;
	virtual bool SetProperty(int nOp, const GFX_PROPERTY& prop) = 0;
	virtual bool GetProperty(int nOp, GFX_PROPERTY& prop) const = 0;
};

///////////////////////////////////////////////////////////////////////////
//	
//	class GFXSOUNDIMP
//	
///////////////////////////////////////////////////////////////////////////

class GFXSOUNDIMP : public IGFXSOUNDIMP
{
public:
	GFXSOUNDIMP();
	~GFXSOUNDIMP();
	GFXSOUNDIMP(const GFXSOUNDIMP& rhs);
	GFXSOUNDIMP& operator = (const GFXSOUNDIMP& rhs);

	virtual bool Load(AFile* pFile, DWORD dwVersion);
	virtual bool Save(AFile* pFile);
	virtual void PrePlay();
	virtual void InitData();
	virtual void PostPlay(A3DGFXSound* pSound);
	virtual void StopSound();
	virtual void Stop(A3DGFXSound* pSound);
	virtual void ReleaseSound(A3DGFXSound* pSound);
	virtual void ClearSoundFile();
	virtual bool TickSound(A3DGFXSound* pSound, DWORD dwTickTime);
	virtual void ResumeLoop();
	virtual bool IsValid() const { return m_pBuf != NULL; }
	virtual bool SetProperty(int nOp, const GFX_PROPERTY& prop);
	virtual bool GetProperty(int nOp, GFX_PROPERTY& prop) const;

private:

	GfxSoundParamInfo m_ParamInfo;
	AM3DSoundBuffer* m_pBuf;
	bool m_bStart;
	bool m_bLoadFail;
	DWORD m_dwCheckTime;
	RandStringContainer* m_pFiles;
	AString m_strLastSound;
	int m_iSoundCount;
	DWORD	m_dwRelativeVolume;
	DWORD	m_dwLastRelativeVolume;

private:
	void UpdateSoundChange();
	bool CheckDist(A3DGFXSound* pSound);
	bool ChangeSoundFile(A3DGFXSound* pSound);
	const char* GetRandSfxFilePath() const;

	//	This function makes sure that no sound file would be played more than 3 times continuely
	void CheckSoundFileChange(AString& strFile);
	void UpdateLastSoundFile(const AString& strFile);
};

///////////////////////////////////////////////////////////////////////////
//	
//	class GFXSOUNDIMP22
//	
///////////////////////////////////////////////////////////////////////////

#ifdef _USEAUDIOENGINE
class GFXSOUNDIMP22 : public IGFXSOUNDIMP, public AudioEngine::EventInstance::EventInstanceListener
#else
class GFXSOUNDIMP22 : public IGFXSOUNDIMP
#endif
{
public:
	GFXSOUNDIMP22();
	~GFXSOUNDIMP22();
	GFXSOUNDIMP22(const GFXSOUNDIMP22& rhs);
	GFXSOUNDIMP22& operator = (const GFXSOUNDIMP22& rhs);

	virtual bool Load(AFile* pFile, DWORD dwVersion);
	virtual bool Save(AFile* pFile);

	virtual void PrePlay();
	virtual void InitData();
	virtual void PostPlay(A3DGFXSound* pSound);
	virtual void Stop(A3DGFXSound* pSound);
	virtual void StopSound();
	virtual void ReleaseSound(A3DGFXSound* pSound);
	virtual void ClearSoundFile();
	virtual bool TickSound(A3DGFXSound* pSound, DWORD dwTickTime);
	virtual void ResumeLoop();
	virtual bool IsValid() const { return m_pEventInstance != NULL; }
	virtual bool SetProperty(int nOp, const GFX_PROPERTY& prop);
	virtual bool GetProperty(int nOp, GFX_PROPERTY& prop) const;

	virtual bool OnStop(AudioEngine::EventInstance* pEventInstance);

private:

	AString m_strEventPath;
	float m_fMinDist, m_fMaxDist;
	bool m_bUseCustom;
	AudioEngine::EventInstance*	m_pEventInstance;
	A3DVECTOR3 m_vLastTickPos;
	bool m_bPosInited;
	CSWrapper m_csForThreadSafe;

private:
	A3DVECTOR3 GetCurPos(A3DGFXSound* pSound);
	void ReleaseEventInstance();
};


///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////


#endif	//	_A3DGFXSoundImp_H_

