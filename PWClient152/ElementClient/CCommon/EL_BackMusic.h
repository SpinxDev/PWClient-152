/*
 * FILE: EL_BackMusic.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Hedi, 2005/4/30
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#ifndef _EL_BACKMUSIC_H_
#define _EL_BACKMUSIC_H_

#include <AMSoundEngine.h>
#include <AMSoundStream.h>
#include <vector.h>

class CELBackMusic
{
public:
	
	enum FADESTATE
	{
		FADE_NONE = 0,
		FADE_IN,
		FADE_OUT
	};

	enum LOOPTYPE
	{
		LOOP_NONE = 0,
		LOOP_ALL = 1,
		LOOP_EXCEPT_FIRST = 2,
	};

	struct BGMTAG
	{
		int		fadeIn;				// indicates whether this bgm need fading in when played, 1 to be tree
		char	bgmFile[MAX_PATH];	// bgm file name
	};

private:
	AMSoundEngine *			m_pSoundEngine;		// sound engine object
	AMSoundStream *			m_pSoundStream;		// sound stream to play
	AMSoundStream *			m_pBackSFX;			// back ground sfx
	AMSoundStream *			m_pCommentator;		// commentator voice
	
	// back music section
	FADESTATE				m_fadeState;		// fading state
	float					m_fVolumeDelta;		// volume delta
	int						m_nVolume;			// current volume
	int						m_nStreamVolume;	// stream max volume [0~100]

	int						m_idBGMPlaying;		// index of current playing bgm
	abase::vector<BGMTAG>	m_bgmPlayList;		// bgm play list

	bool					m_bFirstBGM;		// flag indicates this is the first bgm

	int						m_nSilenceTime;		// time of silence between two bgm
	int						m_ticksSilenced;	// time that doing silence

	int						m_ticksWait;		// time wait before we can play music again

	LOOPTYPE				m_typeLoop;			// loop type;

	// back sfx section
	FADESTATE				m_sfxFadeState;		// fading state
	float					m_fSFXVolumeDelta;	// volume delta
	int						m_nSFXVolume;		// current volume
	int						m_nSFXMAXVolume;	// sfx max volume
	char 					m_szNextSFX[MAX_PATH];	// next sfx

	// commentator voice section
	FADESTATE				m_commentatorFadeState;		// fading state of commentator
	float					m_fCommentatorVolumeDelta;	// volume delta of commentator
	int						m_nCommentatorVolume;		// current volume of commentator
	int						m_nCommentatorMAXVolume;	// max volume of commentator
	char 					m_szNextCommentator[MAX_PATH];	// next commentator

public:
	void SetLoopType(LOOPTYPE typeLoop)			{ m_typeLoop = typeLoop; }
	void SetSilenceTime(int nMilliseconds)		{ m_nSilenceTime = nMilliseconds; }

protected:
	bool PlayNextBGM(); 

public:
	CELBackMusic();
	~CELBackMusic();

	bool Init(AMSoundEngine * pSoundEngine, int nSilenceTime=180000);
	bool Release();

	// back music section
	bool PlayMusic(const char * szBGMFile, bool bFadeIn=true, bool bClearPlayList=true);
	bool StopMusic(bool bFadeOut=true, bool bClearPlayList=true);

	bool RestartMusic();

	bool Update(int nDeltaTime);
	bool SetVolume(int nVolume);

	// sfx section
	bool UpdateSFX(int nDeltaTime);
	bool SetSFXVolume(int nVolume);
	bool PlayBackSFX(const char * szSFXFile);
	bool StopBackSFX(bool bFadeOut=true);

	// commentator section
	bool UpdateCommentator(int nDeltaTime);
	bool SetCommentatorVolume(int nVolume);
	bool PlayCommentator(const char * szCommentatorFile);
	bool StopCommentator(bool bFadeOut=true);
};

#endif//_EL_BACKMUSIC_H_
