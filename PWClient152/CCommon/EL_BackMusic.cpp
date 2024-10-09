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

#include "EL_BackMusic.h"
#include "IO.h"
#include "AFI.h"
#include "AFileUnicode.h"

CELBackMusic::CELBackMusic()
: m_bgmPlayList()
{
	m_pSoundEngine		= NULL;
	m_pSoundStream		= NULL;
	m_pBackSFX			= NULL;
	m_pCommentator		= NULL;

	m_fadeState			= FADE_NONE;
	m_fVolumeDelta		= 0.0f;
	m_nVolume			= 0;
	m_nStreamVolume		= 100;

	m_bFirstBGM			= true;
	m_nSilenceTime		= 1000;
	m_ticksSilenced		= 0;
	m_ticksWait			= 0;
	
	m_idBGMPlaying		= -1;

	m_typeLoop			= LOOP_ALL;

	m_szNextSFX[0]		= '\0';
	m_nSFXVolume		= 0;
	m_nSFXMAXVolume		= 100;
	m_sfxFadeState		= FADE_NONE;
	m_fSFXVolumeDelta	= 0.0f;

	m_szNextCommentator[0]		= '\0';
	m_nCommentatorVolume		= 0;
	m_nCommentatorMAXVolume		= 100;
	m_commentatorFadeState		= FADE_NONE;
	m_fCommentatorVolumeDelta	= 0.0f;
}

CELBackMusic::~CELBackMusic()
{
	Release();
}

bool CELBackMusic::Init(AMSoundEngine * pSoundEngine, int nSilenceTime)
{
	m_pSoundEngine = pSoundEngine;
	m_nSilenceTime = nSilenceTime;

	m_pSoundStream = new AMSoundStream();
	
	if( !m_pSoundStream->Init(m_pSoundEngine, NULL) )
	{
		m_pSoundStream->Release();
		delete m_pSoundStream;
		m_pSoundStream = NULL;
		return false;
	}

	m_pSoundStream->SetLoopFlag(false);
	m_pSoundStream->SetVolume(0);
	m_nVolume = 0;

	m_pBackSFX = new AMSoundStream();
	if( !m_pBackSFX->Init(m_pSoundEngine, NULL) )
	{
		m_pBackSFX->Release();
		delete m_pBackSFX;
		m_pBackSFX = NULL;
		return false;
	}
	m_pBackSFX->SetLoopFlag(true);
	m_pBackSFX->SetVolume(0);
	m_nSFXVolume = 0;

	m_pCommentator = new AMSoundStream();
	if( !m_pCommentator->Init(m_pSoundEngine, NULL) )
	{
		m_pCommentator->Release();
		delete m_pCommentator;
		m_pCommentator = NULL;
		return false;
	}
	m_pCommentator->SetLoopFlag(false);
	m_pCommentator->SetVolume(0);
	m_nCommentatorVolume = 0;
	
	m_nStreamVolume = 100;
	m_nSFXMAXVolume = 100;
	return true;
}

bool CELBackMusic::Release()
{
	if( m_pCommentator )
	{
		m_pCommentator->Stop();

		m_pCommentator->Release();
		delete m_pCommentator;
		m_pCommentator = NULL;
	}

	if( m_pBackSFX )
	{
		m_pBackSFX->Stop();

		m_pBackSFX->Release();
		delete m_pBackSFX;
		m_pBackSFX = NULL;
	}

	if( m_pSoundStream )
	{
		m_pSoundStream->Stop();

		m_pSoundStream->Release();
		delete m_pSoundStream;
		m_pSoundStream = NULL;
	}

	m_szNextSFX[0] = '\0';
	m_idBGMPlaying = -1;
	return true;
}

bool CELBackMusic::Update(int nDeltaTime)
{
	if( m_ticksWait )
	{
		m_ticksWait -= nDeltaTime;
		if( m_ticksWait <= 0 )
			m_ticksWait = 0;

		return true;
	}

	if( m_ticksSilenced > 0 )
	{
		m_ticksSilenced -= nDeltaTime;
		if( m_ticksSilenced <= 0 )
		{
			m_ticksSilenced = 0;
			if( m_bgmPlayList.size() > 0 )
				PlayNextBGM();
		}

		return true;
	}

	if( m_pSoundStream )
	{
		m_pSoundStream->Tick();

		if( m_pSoundStream->IsStopped() )
		{
			if( m_bFirstBGM ) // first bgm, so just play it
			{
				if( m_bgmPlayList.size() > 0 )
					PlayNextBGM();
			}
			else
			{
				// do a long silence after finish playing this bgm
				m_ticksSilenced = max(10, m_nSilenceTime);
			}
		}
	}

	if( m_fadeState == FADE_NONE)
		return true;

	m_fVolumeDelta += (nDeltaTime * 0.001f) * 25;
	int nVolumeDelta = (int)m_fVolumeDelta;
	m_fVolumeDelta -= nVolumeDelta;
	if( nVolumeDelta )
	{
		if( m_fadeState == FADE_IN )
		{
			m_nVolume += nVolumeDelta;
			if( m_nVolume >= 100 )
			{
				m_fadeState = FADE_NONE;
				m_nVolume = 100;
			}

			if( m_pSoundStream )
				m_pSoundStream->SetVolume(m_nVolume * m_nStreamVolume / 100);
		}
		else
		{
			m_nVolume -= nVolumeDelta;
			if( m_nVolume <= 0 )
			{
				m_fadeState = FADE_NONE;
				m_nVolume = 0;
				
				PlayNextBGM();
			}

			if( m_pSoundStream )
				m_pSoundStream->SetVolume(m_nVolume * m_nStreamVolume / 100);
		}
	}

	return true;
}

bool CELBackMusic::PlayMusic(const char * szBGMFile, bool bFadeIn, bool bClearPlayList)
{
	if( bClearPlayList )
	{
		StopMusic(true, true);
		m_idBGMPlaying = -1;
		m_bgmPlayList.clear();

		m_bFirstBGM	= true;
	}

	BGMTAG bgm;

	char sz_file[MAX_PATH];
	strncpy(sz_file, szBGMFile, MAX_PATH);
	strlwr(sz_file);
	if( strstr(sz_file, "music") != sz_file || 0 == _access(szBGMFile, 0) || AFileUnicode::IsFileUnicodeExist(szBGMFile) )
		strncpy(bgm.bgmFile, szBGMFile, MAX_PATH);
	else
	{
		char szNewPath[MAX_PATH];
		strncpy(szNewPath, szBGMFile, MAX_PATH);
		af_ChangeFileExt(szNewPath, MAX_PATH, ".ogg");
		sprintf(bgm.bgmFile, "sfx\\%s", szNewPath);
	}
	 
	bgm.fadeIn = bFadeIn ? 1 : 0;
	m_bgmPlayList.push_back(bgm);

	// clear silence time
	m_ticksSilenced = 0;
	return true;
}

bool CELBackMusic::StopMusic(bool bFadeOut, bool bClearPlayList)
{
	if( !m_pSoundStream || m_pSoundStream->IsStopped() )
		return true;

	if( !bFadeOut )
	{
		m_nVolume = 0;

		if( m_pSoundStream )
		{
			m_pSoundStream->Stop();
			m_pSoundStream->SetVolume(0);
		}
	}
	else
	{
		m_fadeState = FADE_OUT;
	}

	if( bClearPlayList )
	{
		m_bgmPlayList.clear();
	}

	// clear silence time
	m_ticksSilenced = 0;

	return true;
}

bool CELBackMusic::PlayNextBGM()
{
	m_bFirstBGM = false;

	if( m_bgmPlayList.size() == 0 )
		return true;

	if( m_idBGMPlaying == -1 ) // first time first bgm
	{
		m_idBGMPlaying = 0;
	}
	else
	{
		int nNumBGMInList = m_bgmPlayList.size();
		// see if loop or not
		switch(m_typeLoop)
		{
		case LOOP_NONE:
			StopMusic(false, true);
			return true;
			
		case LOOP_ALL:
			m_idBGMPlaying = (m_idBGMPlaying + 1) % nNumBGMInList;
			break;

		case LOOP_EXCEPT_FIRST:
			m_idBGMPlaying = (m_idBGMPlaying + 1) % nNumBGMInList;
			if( m_idBGMPlaying == 0 ) // no first bgm in loop
				m_idBGMPlaying = (m_idBGMPlaying + 1) % nNumBGMInList;
			break;
		}
	}

	BGMTAG& tag = m_bgmPlayList[m_idBGMPlaying];

	if( m_pSoundStream )
	{
		if( !m_pSoundStream->Play(tag.bgmFile) )
		{
			// can't open this bgm file, so remove it from the list
			m_bgmPlayList.erase(&tag);
			return true;
		}
	}

	if( tag.fadeIn )
		m_fadeState = FADE_IN;
	else
	{
		m_fadeState = FADE_NONE;
		m_nVolume = 100;
		if( m_pSoundStream )
			m_pSoundStream->SetVolume(m_nVolume * m_nStreamVolume / 100);
	}
	
	return true;
}

bool CELBackMusic::SetVolume(int nVolume)
{
	if( nVolume >= 0 )
		m_nStreamVolume = nVolume;

	if( m_pSoundStream )
		m_pSoundStream->SetVolume(m_nVolume * m_nStreamVolume / 100);
	return true;
}

/////////////////////////////////////////////////////////////////////////////////
// back sfx section
/////////////////////////////////////////////////////////////////////////////////
bool CELBackMusic::UpdateSFX(int nDeltaTime)
{
	if( m_ticksWait )
		return true;
	
	if( m_pBackSFX )
	{
		m_pBackSFX->Tick();
		if( m_pBackSFX->IsStopped() && m_szNextSFX[0] )
		{
			m_pBackSFX->Play(m_szNextSFX);
			m_sfxFadeState = FADE_IN;
			m_szNextSFX[0] = '\0';
		}
	}

	if( m_sfxFadeState == FADE_NONE)
		return true;

	m_fSFXVolumeDelta += (nDeltaTime * 0.001f) * 25;
	int nVolumeDelta = (int)m_fSFXVolumeDelta;
	m_fSFXVolumeDelta -= nVolumeDelta;
	if( nVolumeDelta )
	{
		if( m_sfxFadeState == FADE_IN )
		{
			m_nSFXVolume += nVolumeDelta;
			if( m_nSFXVolume >= 100 )
			{
				m_sfxFadeState = FADE_NONE;
				m_nSFXVolume = 100;
			}

			if( m_pBackSFX )
				m_pBackSFX->SetVolume(m_nSFXVolume * m_nSFXMAXVolume / 100);
		}
		else
		{
			m_nSFXVolume -= nVolumeDelta;
			if( m_nSFXVolume <= 0 )
			{
				m_sfxFadeState = FADE_NONE;
				m_nSFXVolume = 0;
				
				if( m_pBackSFX )
				{
					if( m_szNextSFX[0] )
					{
						m_pBackSFX->Play(m_szNextSFX);
						m_sfxFadeState = FADE_IN;
						m_szNextSFX[0] = '\0';
					}
					else
					{
						m_pBackSFX->Stop();
					}
				}
			}

			if( m_pBackSFX )
				m_pBackSFX->SetVolume(m_nSFXVolume * m_nSFXMAXVolume / 100);
		}
	}

	return true;
}

bool CELBackMusic::SetSFXVolume(int nVolume)
{
	if( nVolume >= 0 )
		m_nSFXMAXVolume = nVolume;

	if( m_pBackSFX )
		m_pBackSFX->SetVolume(m_nSFXVolume * m_nSFXMAXVolume / 100);

	return true;
}

bool CELBackMusic::PlayBackSFX(const char * szSFXFile)
{
	strncpy(m_szNextSFX, szSFXFile, MAX_PATH);

	if( m_pBackSFX && m_pBackSFX->IsPlaying() )
		m_sfxFadeState = FADE_OUT;
	
	return true;
}

bool CELBackMusic::StopBackSFX(bool bFadeOut)
{
	if( !m_pBackSFX || m_pBackSFX->IsStopped() )
		return true;

	if( bFadeOut && m_pBackSFX->IsPlaying() )
	{
		m_sfxFadeState = FADE_OUT;
		m_szNextSFX[0] = '\0';
	}
	else
	{
		m_pBackSFX->Stop();
		m_sfxFadeState = FADE_NONE;
		m_szNextSFX[0] = '\0';
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////////
// Commentator section
/////////////////////////////////////////////////////////////////////////////////
bool CELBackMusic::UpdateCommentator(int nDeltaTime)
{
	if( m_ticksWait )
		return true;
	
	if( m_pCommentator )
	{
		m_pCommentator->Tick();
		if( m_pCommentator->IsStopped() && m_szNextCommentator[0] )
		{
			m_pCommentator->Play(m_szNextCommentator);
			m_commentatorFadeState = FADE_IN;
			m_szNextCommentator[0] = '\0';
		}
	}

	// we need sfx fade test later...
	if( m_commentatorFadeState == FADE_NONE)
		return true;

	m_fCommentatorVolumeDelta += (nDeltaTime * 0.001f) * 25;
	int nVolumeDelta = (int)m_fCommentatorVolumeDelta;
	m_fCommentatorVolumeDelta -= nVolumeDelta;
	if( nVolumeDelta )
	{
		if( m_commentatorFadeState == FADE_IN )
		{
			m_nCommentatorVolume += nVolumeDelta;
			if( m_nCommentatorVolume >= 100 )
			{
				m_commentatorFadeState = FADE_NONE;
				m_nCommentatorVolume = 100;
			}

			if( m_pCommentator )
				m_pCommentator->SetVolume(m_nCommentatorVolume * m_nCommentatorMAXVolume / 100);
		}
		else
		{
			m_nCommentatorVolume -= nVolumeDelta;
			if( m_nCommentatorVolume <= 0 )
			{
				m_commentatorFadeState = FADE_NONE;
				m_nCommentatorVolume = 0;
				
				if( m_pCommentator )
				{
					if( m_szNextCommentator[0] )
					{
						m_pCommentator->Play(m_szNextCommentator);
						m_commentatorFadeState = FADE_IN;
						m_szNextCommentator[0] = '\0';
					}
					else
					{
						m_pCommentator->Stop();
					}
				}
			}

			if( m_pCommentator )
				m_pCommentator->SetVolume(m_nCommentatorVolume * m_nCommentatorMAXVolume / 100);
		}
	}

	return true;
}

bool CELBackMusic::SetCommentatorVolume(int nVolume)
{
	if( nVolume >= 0 )
		m_nCommentatorMAXVolume = nVolume;

	if( m_pCommentator )
		m_pCommentator->SetVolume(m_nCommentatorVolume * m_nCommentatorMAXVolume / 100);

	return true;
}

bool CELBackMusic::PlayCommentator(const char * szCommentatorFile)
{
	strncpy(m_szNextCommentator, szCommentatorFile, MAX_PATH);

	if( m_pCommentator && m_pCommentator->IsPlaying() )
		m_commentatorFadeState = FADE_OUT;
	
	return true;
}

bool CELBackMusic::StopCommentator(bool bFadeOut)
{
	if( !m_pCommentator || m_pCommentator->IsStopped() )
		return true;

	if( bFadeOut && m_pCommentator->IsPlaying() )
	{
		m_commentatorFadeState = FADE_OUT;
		m_szNextCommentator[0] = '\0';
	}
	else
	{
		m_pCommentator->Stop();
		m_commentatorFadeState = FADE_NONE;
		m_szNextCommentator[0] = '\0';
	}

	return true;
}

bool CELBackMusic::RestartMusic()
{
	// now we can play music again, but before that insert some silence before we can play it
	m_ticksWait = 1000;
	return true;
}
