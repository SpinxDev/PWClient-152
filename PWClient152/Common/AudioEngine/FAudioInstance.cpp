#include "FAudioInstance.h"

#include "FEventSystem.h"
#include "FAudio.h"
#include "FAudioBank.h"
#include "FSoundInstance.h"
#include "FEvent.h"
#include "FEventLayer.h"
#include "FSoundDef.h"
#include "FEventLayerInstance.h"
#include "FSoundInstanceTemplate.h"
#include "FAudioTemplate.h"
#include "FLogManager.h"
#include "FIntervalManager.h"
#include <fmod.hpp>

using namespace AudioEngine;

static const int cs_iReleaseDataInterval = 10000;

FMOD_RESULT F_CALLBACK audioCallback(FMOD_CHANNEL *channel, FMOD_CHANNEL_CALLBACKTYPE type, void *commanddata1, void *commanddata2)
{
	FMOD::Channel* pChannel = (FMOD::Channel*)channel;
	if(!pChannel)
		return FMOD_ERR_INVALID_HANDLE;
	pChannel->setCallback(0);

	switch (type)
	{
	case FMOD_CHANNEL_CALLBACKTYPE_END:
		{
			SoundInstance* pSoundInstance = 0;
			pChannel->getUserData((void**)&pSoundInstance);
			pChannel->setUserData(0);
			if(!pSoundInstance)
				return FMOD_ERR_INVALID_HANDLE;			
 			EventLayerInstance* pEventLayerInstance = pSoundInstance->GetEventLayerInstance();
 			if(!pEventLayerInstance)
 				return FMOD_ERR_INVALID_HANDLE;
			EventInstance* pEventInstance = pEventLayerInstance->GetEventInstance();
			if(!pEventInstance)
				return FMOD_ERR_INVALID_HANDLE;
			Event* pEvent = pEventInstance->GetEvent();
			if(!pEvent)
				return FMOD_ERR_INVALID_HANDLE;
			CriticalSectionController csc(pEvent->GetCriticalSectionWrapper());
			CriticalSectionController cscei(pEventInstance->GetCriticalSectionWrapper());
			CriticalSectionController csceli(pEventLayerInstance->GetCriticalSectionWrapper());
			CriticalSectionController cscsi(pSoundInstance->GetCriticalSectionWrapper());
			pSoundInstance->RemoveChannel(pChannel);
			int iCurIdx = 0;
			if(!pSoundInstance->GetSoundPlayList().GetCurPlayIndex(iCurIdx))
				return FMOD_ERR_INVALID_HANDLE;
			if(pSoundInstance)
			{
				if(pSoundInstance->IsStoping())
					 pSoundInstance->SetPlaying(false);

				if(pSoundInstance->IsPlayForEvent())
				{
					SoundInstance* pCurSoundInstance = pEventLayerInstance->GetCurSoundInstance();
					if(!pCurSoundInstance)
						return FMOD_ERR_INVALID_HANDLE;
					if(pCurSoundInstance != pSoundInstance)
					{
						pSoundInstance->SetPlaying(false);
						pSoundInstance->GetEventLayerInstance()->Update(0);
						/*if(!pCurSoundInstance->playForEvent())
							return FMOD_ERR_INVALID_HANDLE;
						if(!pSoundInstance->Stop(true))
							return FMOD_ERR_INVALID_HANDLE;*/
						return FMOD_OK;
					}
				}
				int iIntervalTime = pSoundInstance->GetIntervalTime();
				if(iIntervalTime == 0)
				{
					if(!pSoundInstance->IsPlaying())
					{
						if(!pSoundInstance->Stop(true))
							return FMOD_ERR_INVALID_HANDLE;
						return FMOD_OK;
					}
					int iNextPlayIndex;
					int iNextPlayLoopCount;
					if(!pSoundInstance->GetSoundPlayList().GetNextIndex(iNextPlayIndex, iNextPlayLoopCount))
					{
						if(!pSoundInstance->Stop(true))
							return FMOD_ERR_INVALID_HANDLE;
						return FMOD_OK;
					}

					AudioInstance* pNextAudioInstance = pSoundInstance->GetSoundInstanceTemplate()->GetAudioInstanceByIndex(iNextPlayIndex, pSoundInstance->IsPlayForEvent(), true);
					if(pSoundInstance->IsPlayForEvent())
					{
						if(pNextAudioInstance && !pNextAudioInstance->PlayForEvent(pSoundInstance, iNextPlayLoopCount))
						{
							return FMOD_ERR_INVALID_HANDLE;
						}
					}
					else
					{
						if(pNextAudioInstance && !pNextAudioInstance->PlayForDef(pSoundInstance, iNextPlayLoopCount))
						{
							return FMOD_ERR_INVALID_HANDLE;
						}
					}
				}
				else
				{
					if(pSoundInstance->IsPlaying() && !pSoundInstance->IsStoping())
					{
						pSoundInstance->SetLeftIntervalTime(iIntervalTime);
						IntervalManager::GetInstance().AddSoundInstance(pSoundInstance);
					}
				}				
			}
		}
		break;
	}
	return FMOD_OK;
}

AudioInstance::AudioInstance(void)
: m_pEventSystem(0)
, m_pSound(0)
, m_pAudioTemplate(0)
, m_bLoaded(false)
, m_iReleaseDataInterval(cs_iReleaseDataInterval)
, m_bCanbeDeleted(false)
{
}

AudioInstance::~AudioInstance(void)
{
	CriticalSectionController csc(&m_csForThreadSafe);
	listenerDelete();
	m_listChannel.clear();
	release();
}

bool AudioInstance::Init(EventSystem* pEventSystem, AudioTemplate* pAudioTemplate)
{
	CriticalSectionController csc(&m_csForThreadSafe);
	if(!pEventSystem || !pAudioTemplate)
		return false;	
	release();
	m_pEventSystem = pEventSystem;
	m_pAudioTemplate = pAudioTemplate;
	return true;
}

bool AudioInstance::createSoundForEvent(bool bReload /*= false*/)
{
	CriticalSectionController csc(&m_csForThreadSafe);
	AudioBank* pAudioBank = m_pAudioTemplate->GetAudio()->GetAudioBank();
	if(m_bLoaded && !bReload)
	{
		if(pAudioBank->GetBankType() != BANK_TYPE_STREAM_FROM_DISK)
			return true;
	}
	release();
	m_iReleaseDataInterval = cs_iReleaseDataInterval;
	m_bCanbeDeleted = false;
	if(!m_pEventSystem)
		return false;
	FMOD::System* pSystem = m_pEventSystem->GetSystem();
	if(!pSystem)
		return false;	
	FMOD_MODE mode = FMOD_SOFTWARE | /*FMOD_ACCURATETIME |*/ FMOD_LOOP_NORMAL;
	if(!pAudioBank)
		return false;
	switch (pAudioBank->GetBankType())
	{
	case BANK_TYPE_LOAD_INTO_MEMORY:
		mode |= FMOD_CREATECOMPRESSEDSAMPLE;
		break;
	case BANK_TYPE_STREAM_FROM_DISK:
		mode |= FMOD_CREATESTREAM;
		break;
	case BANK_TYPE_DECOMPRESS_INTO_MEMORY:
		mode |= FMOD_CREATESAMPLE;
		break;
	}
	if(m_pAudioTemplate->GetAudio()->IsAccurateTime())
	{
		mode |= FMOD_ACCURATETIME;
	}
	m_OriginalMode = mode;
	{
		CriticalSectionController csces(m_pEventSystem->GetCriticalSectionWrapper());
		if(FMOD_OK != pSystem->createSound(m_pAudioTemplate->GetAudio()->GetPath(), mode, 0, &m_pSound))
		{
			m_pSound = 0;
			m_pEventSystem->GetLogManager()->GetEngineLog()->Write(LOG_ERROR, "AudioInstance::createSoundForEvent createSound:%sÊ§°Ü", m_pAudioTemplate->GetAudio()->GetPath());
			return false;
		}
	}
	m_bLoaded = true;
	return true;
}

void AudioInstance::release()
{	
	CriticalSectionController csc(&m_csForThreadSafe);	
	if(!m_pSound)
		return;
	CriticalSectionController csces(m_pEventSystem->GetCriticalSectionWrapper());
	m_pSound->release();
	m_pSound = 0;
}

bool AudioInstance::createSoundForSoundDef(bool bReload /*= false*/)
{
	CriticalSectionController csc(&m_csForThreadSafe);	
	AudioBank* pAudioBank = m_pAudioTemplate->GetAudio()->GetAudioBank();
	if(m_bLoaded && !bReload)
	{
		if(pAudioBank->GetBankType() != BANK_TYPE_STREAM_FROM_DISK)
			return true;
	}
	release();
	m_iReleaseDataInterval = cs_iReleaseDataInterval;
	m_bCanbeDeleted = false;
	if(!m_pEventSystem)
		return false;
	FMOD::System* pSystem = m_pEventSystem->GetSystem();
	if(!pSystem)
		return false;
	FMOD_MODE mode = FMOD_SOFTWARE | /*FMOD_ACCURATETIME |*/ FMOD_LOOP_NORMAL;	
	if(!pAudioBank)
		return false;
	switch (pAudioBank->GetBankType())
	{
	case BANK_TYPE_LOAD_INTO_MEMORY:
		mode |= FMOD_CREATECOMPRESSEDSAMPLE;
		break;
	case BANK_TYPE_STREAM_FROM_DISK:
		mode |= FMOD_CREATESTREAM;
		break;
	case BANK_TYPE_DECOMPRESS_INTO_MEMORY:
		mode |= FMOD_CREATESAMPLE;
		break;
	}
	if(m_pAudioTemplate->GetAudio()->IsAccurateTime())
	{
		mode |= FMOD_ACCURATETIME;
	}
	{
		CriticalSectionController csces(m_pEventSystem->GetCriticalSectionWrapper());
		if(FMOD_OK != pSystem->createSound(m_pAudioTemplate->GetAudio()->GetPath(), mode, 0, &m_pSound))
		{
			m_pSound = 0;
			m_pEventSystem->GetLogManager()->GetEngineLog()->Write(LOG_ERROR, "AudioInstance::createSoundForSoundDef createSound:%sÊ§°Ü", m_pAudioTemplate->GetAudio()->GetPath());
			return false;
		}
	}	
	m_bLoaded = true;
	return true;
}

bool AudioInstance::PlayForDef(SoundInstance* pSoundInstance, int iLoopCount)
{
	CriticalSectionController csc(&m_csForThreadSafe);	
	if(!m_pEventSystem || !m_pSound)
		return false;
	FMOD::System* pSystem = m_pEventSystem->GetSystem();
	if(!pSystem)
		return false;	
	FMOD::ChannelGroup* pChannelGroup = pSoundInstance->GetChannelGroup();
	if(!pChannelGroup)
		return false;
	SoundInstanceTemplate* pSoundInstanceTemplate = pSoundInstance->GetSoundInstanceTemplate();
	if(!pSoundInstanceTemplate)
		return false;
	m_iReleaseDataInterval = cs_iReleaseDataInterval;
	m_bCanbeDeleted = false;
	if(FMOD_OK != m_pSound->setLoopCount(iLoopCount-1))
		return false;
	FMOD::Channel* pChannel = 0;
	{
		CriticalSectionController csces(m_pEventSystem->GetCriticalSectionWrapper());
		if(FMOD_OK != pSystem->playSound(FMOD_CHANNEL_FREE, m_pSound, true, &pChannel))
			return false;
		if(FMOD_OK != pChannel->setChannelGroup(pChannelGroup))
			return false;
	}
	pSoundInstance->AddChannel(pChannel);
	addChannel(pChannel);	
	AUDIO_PROPERTY audioProp;
	m_pAudioTemplate->GetProperty(audioProp);
	float fAudioVolumeRatio = getVolumeRatio(audioProp.fVolume, audioProp.fVolumeRandomization);
	float fAudioPitchRatio = getPitchRatio(audioProp.fPitch, audioProp.fPitchRandomization);
	SoundDef* pSoundDef = pSoundInstanceTemplate->GetSoundDef();
	if(!pSoundDef)
		return false;
	SOUND_DEF_PROPERTY soundDefProp;
	pSoundDef->GetProperty(soundDefProp);
	float fSoundDefVolumeRatio = getVolumeRatio(soundDefProp.fVolume, soundDefProp.fVolumeRandomization);
	if(FMOD_OK != pChannel->setVolume(fSoundDefVolumeRatio*fAudioVolumeRatio))
		return false;
	float fSoundDefPitchRatio = getPitchRatio(soundDefProp.fPitch, soundDefProp.fPitchRandomization);
	if(FMOD_OK != pChannelGroup->setPitch(fSoundDefPitchRatio*fAudioPitchRatio))
		return false;
	if(FMOD_OK != pChannel->setPaused(false))
		return false;
	if(FMOD_OK != pChannel->setUserData(pSoundInstance))
		return false;
	if(FMOD_OK != pChannel->setCallback(audioCallback))
		return false;
	return true;
}

bool AudioInstance::PlayForEvent(SoundInstance* pSoundInstance, int iLoopCount)
{
	CriticalSectionController csc(&m_csForThreadSafe);	

	if(!m_pEventSystem || !m_pSound)
		return false;
	FMOD::System* pSystem = m_pEventSystem->GetSystem();
	if(!pSystem)
		return false;
	FMOD::ChannelGroup* pChannelGroup = pSoundInstance->GetChannelGroup();
	if(!pChannelGroup)
		return false;
	SoundInstanceTemplate* pSoundInstanceTemplate = pSoundInstance->GetSoundInstanceTemplate();
	if(!pSoundInstanceTemplate)
		return false;
	m_iReleaseDataInterval = cs_iReleaseDataInterval;
	m_bCanbeDeleted = false;	

	EventLayer* pEventLayer = pSoundInstanceTemplate->GetEventLayer();
	if(!pEventLayer)
		return false;
	Event* pEvent = pEventLayer->GetEvent();
	if(!pEvent)
		return false;
	FMOD_MODE mode = m_OriginalMode;
	EVENT_PROPERTY EventProp;
	pEvent->GetProperty(EventProp);
	if(EventProp.mode == MODE_2D)
		mode |= FMOD_2D;
	else
	{
		mode |= FMOD_3D;
		if(EventProp.rolloff == ROLLOFF_INVERSE)
			mode |= FMOD_3D_INVERSEROLLOFF;
		else if(EventProp.rolloff == ROLLOFF_LINEAR)
			mode |= FMOD_3D_LINEARROLLOFF;
		else if(EventProp.rolloff == ROLLOFF_LINEAR_SQUARE)
			mode |= FMOD_3D_LINEARSQUAREROLLOFF;
	}
	if(FMOD_OK != m_pSound->setMode(mode))
		return false;
	if(FMOD_OK != m_pSound->setLoopCount(iLoopCount-1))
		return false;
	FMOD::Channel* pChannel = 0;
	{
		CriticalSectionController csces(m_pEventSystem->GetCriticalSectionWrapper());
		if(FMOD_OK != pSystem->playSound(FMOD_CHANNEL_FREE, m_pSound, true, &pChannel))
			return false;
		if(FMOD_OK != pChannel->setChannelGroup(pChannelGroup))
			return false;
	}
	pSoundInstance->AddChannel(pChannel);
	addChannel(pChannel);

	AUDIO_PROPERTY audioProp;
	m_pAudioTemplate->GetProperty(audioProp);
	float fAudioVolumeRatio = getVolumeRatio(audioProp.fVolume, audioProp.fVolumeRandomization);
	float fAudioPitchRatio = getPitchRatio(audioProp.fPitch, audioProp.fPitchRandomization);

	SoundDef* pSoundDef = pSoundInstanceTemplate->GetSoundDef();
	if(!pSoundDef)
		return false;
	SOUND_DEF_PROPERTY soundDefProp;
	pSoundDef->GetProperty(soundDefProp);

	EventLayerInstance* pEventLayerInstance = pSoundInstance->GetEventLayerInstance();
	if(!pEventLayerInstance)
		return false;
	EventInstance* pEventInstance = pEventLayerInstance->GetEventInstance();
	if(!pEventInstance)
		return false;	
	EVENT_PROPERTY EventInsProp;
	pEventInstance->GetProperty(EventInsProp);	

	float fSoundDefVolumeRatio = getVolumeRatio(soundDefProp.fVolume, soundDefProp.fVolumeRandomization);
	float fEventVolumeRatio = getVolumeRatio(EventInsProp.fVolume, EventInsProp.fVolumeRandomization);
	if(FMOD_OK != pChannel->setVolume(fSoundDefVolumeRatio * fEventVolumeRatio * fAudioVolumeRatio))
		return false;
	float fSoundDefPitchRatio = getPitchRatio(soundDefProp.fPitch, soundDefProp.fPitchRandomization);
	float fEventPitchRatio = getPitchRatio(EventInsProp.fPitch, EventInsProp.fPitchRandomization);
	if(FMOD_OK != pChannelGroup->setPitch(fSoundDefPitchRatio * fEventPitchRatio * fAudioPitchRatio))
		return false;
	if(EventInsProp.mode == MODE_3D)
	{
		VECTOR vPos = pEventInstance->GetPos();
		VECTOR vVel = pEventInstance->GetVel();
		FMOD_VECTOR fmod_pos = {vPos.x, vPos.y, vPos.z};
		FMOD_VECTOR fmod_vel = {vVel.x, vVel.y, vVel.z};
		pChannel->set3DAttributes(&fmod_pos, &fmod_vel);
		pChannel->set3DSpread(EventInsProp.f3DSpeakerSpread);
		if(EventInsProp.rolloff != ROLLOFF_CUSTOM)
		{
			if(FMOD_OK != pChannel->set3DMinMaxDistance(EventInsProp.fMinDistance, EventInsProp.fMaxDistance))
				return false;
		}		
	}
	AddListener(pSoundInstance->GetEventLayerInstance());
	if(FMOD_OK != pChannel->setPaused(false))
		return false;
	if(FMOD_OK != pChannel->setUserData(pSoundInstance))
		return false;
	if(FMOD_OK != pChannel->setCallback(audioCallback))
		return false;
	if(!listenerPlay())
		return false;
	return true;
}

bool AudioInstance::SetAudio(AudioTemplate* pAudioTemplate)
{
	m_pAudioTemplate = pAudioTemplate;
	return true;
}

float AudioInstance::getVolumeRatio(float dbVolume, float dbVolumeRandmization)
{
	float fStart = dbVolumeRandmization + dbVolume;
	float fEnd = dbVolume;
	float r = (float)rand() * (fEnd - fStart) / RAND_MAX + fStart;
	return GF_ConvertVolumeFromDB2Ratio(r);
}

float AudioInstance::getPitchRatio(float tonePitch, float tonePitchRandmization)
{
	float toneStart = tonePitch - tonePitchRandmization;
	float tone = (float)rand() * 2 * tonePitchRandmization / RAND_MAX + tonePitch - tonePitchRandmization;
	return GF_ConvertPitchFromTone2Ratio(tone);
}

void AudioInstance::AddListener(AudioInstanceListener* pListener)
{
	CriticalSectionController csc(&m_csForThreadSafe);
	if(!pListener)
		return;	
	m_setListener.insert(pListener);
}

void AudioInstance::RemoveListener(AudioInstanceListener* pListener)
{
	CriticalSectionController csc(&m_csForThreadSafe);
	ListenerSet::iterator it = m_setListener.find(pListener);
	if(it != m_setListener.end())
		m_setListener.erase(m_setListener.find(pListener));
}

bool AudioInstance::listenerPlay()
{
	CriticalSectionController csc(&m_csForThreadSafe);
	ListenerSet setListener = m_setListener;
	ListenerSet::const_iterator it = setListener.begin();
	for (; it!=setListener.end(); ++it)
	{
		if(!(*it)->OnPlay(this))
			continue;
	}
	return true;
}

//bool AudioInstance::listenerStop()
//{
//	CriticalSectionController csc(&m_csForThreadSafe);
//	ListenerSet setListener = m_setListener;
//	ListenerSet::const_iterator it = setListener.begin();
//	for (; it!=setListener.end(); ++it)
//	{
//		if(!(*it)->OnStop(this))
//			continue;
//	}
//	return true;
//}

bool AudioInstance::listenerDelete()
{
	CriticalSectionController csc(&m_csForThreadSafe);
	ListenerSet setListener = m_setListener;
	ListenerSet::const_iterator it = setListener.begin();
	for (; it!=setListener.end(); ++it)
	{
		if(!(*it)->OnDelete(this))
			continue;
	}
	return true;
}

bool AudioInstance::Update(DWORD dwTickTime)
{
	CriticalSectionController csc(&m_csForThreadSafe);
	if(m_listChannel.size() == 0)
	{
		if(m_iReleaseDataInterval < 0)
		{
			m_bCanbeDeleted = true;
			m_iReleaseDataInterval = cs_iReleaseDataInterval;
		}
		m_iReleaseDataInterval -= dwTickTime;
	}
	else
	{
		m_iReleaseDataInterval = cs_iReleaseDataInterval;
		m_bCanbeDeleted = false;
	}
	return true;
}

void AudioInstance::addChannel(FMOD::Channel* pChannel)
{
	CriticalSectionController csc(&m_csForThreadSafe);
	if(!pChannel)
		return;
	m_listChannel.push_back(pChannel);
}

void AudioInstance::removeChannel(FMOD::Channel* pChannel)
{
	CriticalSectionController csc(&m_csForThreadSafe);
	if(!pChannel)
		return;
	ChannelList::iterator it = m_listChannel.begin();
	for (; it!=m_listChannel.end(); ++it)
	{
		if(pChannel == *it)
		{
			m_listChannel.erase(it);
			break;
		}
	}
}