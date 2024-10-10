#include "FSoundInstance.h"

#include "FEventLayer.h"
#include "FEventSystem.h"
#include "FAudioInstance.h"
#include "FSoundDef.h"
#include "FEventProject.h"
#include "FSoundInstanceTemplate.h"
#include "FEventLayerInstance.h"
#include "FCurve.h"
#include <fmod.hpp>

using namespace AudioEngine;

SoundInstance::SoundInstance(void)
: m_pSoundDef(0)
, m_pEventProject(0)
, m_pEventLayerInstance(0)
, m_pChannelGroup(0)
, m_bPlaying(false)
, m_dwStartTickCount(0)
, m_dwStopTickCount(0)
, m_bPlayForEvent(true)
, m_bStoping(false)
, m_pSoundInstanceTemplate(0)
, m_iLeftIntervalTime(0)
, m_bFirstPlay(true)
, m_iLeftPreIntervalTime(0)
{
	memset(&m_SoundInstanceProperty, 0, sizeof(m_SoundInstanceProperty));
	m_SoundInstanceProperty.fStartPosition = 0.0f;
	m_SoundInstanceProperty.fLength = 1.0f;
	m_SoundInstanceProperty.iLoopCount = 1;
}

SoundInstance::~SoundInstance(void)
{
	listenerStop();
	release();
}

bool SoundInstance::Init(EventLayerInstance* pEventLayerInstance, SoundInstanceTemplate* pSoundInstanceTemplate, EventProject* pEventProject)
{
	CriticalSectionController csc(&m_csForThreadSafe);
	release();
	if(!pEventLayerInstance || !pEventProject || !pSoundInstanceTemplate)
		return false;	
	m_pEventLayerInstance = pEventLayerInstance;
	m_pEventProject = pEventProject;
	m_pSoundInstanceTemplate = pSoundInstanceTemplate;
	m_pSoundInstanceTemplate->GetProperty(m_SoundInstanceProperty);
	EventSystem* pEventSystem = m_pEventProject->GetEventSystem();
	if(!pEventSystem)
		return false;
	FMOD::System* pSystem = pEventSystem->GetSystem();
	{
		CriticalSectionController csces(pEventSystem->GetCriticalSectionWrapper());
		if(FMOD_OK != pSystem->createChannelGroup("", &m_pChannelGroup))
			return false;
	}	
	return true;
}

bool SoundInstance::Init(SoundInstanceTemplate* pSoundInstanceTemplate, EventProject* pEventProject)
{
	CriticalSectionController csc(&m_csForThreadSafe);
	release();
	if(!pEventProject || !pSoundInstanceTemplate)
		return false;	
	m_pEventLayerInstance = 0;
	m_pEventProject = pEventProject;
	m_pSoundInstanceTemplate = pSoundInstanceTemplate;
	m_pSoundInstanceTemplate->GetProperty(m_SoundInstanceProperty);
	EventSystem* pEventSystem = m_pEventProject->GetEventSystem();
	if(!pEventSystem)
		return false;
	FMOD::System* pSystem = pEventSystem->GetSystem();
	{
		CriticalSectionController csces(pEventSystem->GetCriticalSectionWrapper());
		if(FMOD_OK != pSystem->createChannelGroup("", &m_pChannelGroup))
			return false;
	}
	return true;
}

bool SoundInstance::reset()
{
	m_bPlaying = false;
	m_bStoping = false;
	return true;
}

bool SoundInstance::playForEvent()
{
	CriticalSectionController csc(&m_csForThreadSafe);
	if(!reset())
		return false;

	m_bPlaying = true;
	m_bPlayForEvent = true;
	m_pSoundInstanceTemplate->m_bPlayForEvent = true;

	AddListener(m_pEventLayerInstance);	

	if(!m_pSoundInstanceTemplate->loadData())
		return false;

	if(!m_SoundPlayList.Init(m_pSoundInstanceTemplate))
		return false;

	if(m_SoundInstanceProperty.iPreIntervalTime == 0 && m_SoundInstanceProperty.iPreIntervalTimeRandomization == 0)
	{
		if(!firstPlay())
			return false;
	}
	else
	{
		m_iLeftPreIntervalTime = GetPreIntervalTime();
	}

	if(!listenerPlay())
		return false;

	return true;
}

bool SoundInstance::firstPlay()
{
	if(!m_bFirstPlay)
		return true;
	int idx;
	int iLoopCount;
	if(!m_SoundPlayList.GetFirstIndex(idx, iLoopCount))
		return false;
	AudioInstance* pFirstAudioInstance = m_pSoundInstanceTemplate->GetAudioInstanceByIndex(idx, true, true);
	if(!pFirstAudioInstance)
		return false;
	if(!pFirstAudioInstance->PlayForEvent(this, iLoopCount))
		return false;

	m_dwStartTickCount = GetTickCount();
	m_bFirstPlay = false;
	return true;
}

bool SoundInstance::PlayForSoundDef()
{
	CriticalSectionController csc(&m_csForThreadSafe);
	if(!reset())
		return false;

	m_bPlaying = true;
	m_bPlayForEvent = false;
	m_pSoundInstanceTemplate->m_bPlayForEvent = false;

	if(!m_pSoundInstanceTemplate->loadData())
		return false;

	if(!m_SoundPlayList.Init(m_pSoundInstanceTemplate))
		return false;

	int idx;
	int iLoopCount;
	if(!m_SoundPlayList.GetFirstIndex(idx, iLoopCount))
		return false;
	AudioInstance* pFirstAudioInstance = m_pSoundInstanceTemplate->GetAudioInstanceByIndex(idx, false, true);
	if(!pFirstAudioInstance)
		return false;
	if(!pFirstAudioInstance->PlayForDef(this, iLoopCount))
		return false;

	m_dwStartTickCount = GetTickCount();

	if(!listenerPlay())
		return false;	
	
	return true;	
}

bool SoundInstance::Stop(bool bForced /*= false*/, bool bEventStop /*= false*/)
{
	CriticalSectionController csc(&m_csForThreadSafe);
	if(!m_pChannelGroup)
		return false;
	SetLoopOff();
	if(bEventStop || ((m_SoundInstanceProperty.stopMode != STOP_MODE_PLAYTOEND || !m_bPlaying) && m_SoundInstanceProperty.iFadeOutTime == 0) || bForced)
	{
		m_bPlaying = false;
		m_bStoping = false;
		m_bFirstPlay = true;
		clearChannel();
		if(m_pChannelGroup)
		{
			CriticalSectionController csc(m_pEventProject->GetEventSystem()->GetCriticalSectionWrapper());
			m_pChannelGroup->stop();
		}
		if(!listenerStop())
			return false;		
		return true;
	}
	if(!m_bStoping)
		m_dwStopTickCount = GetTickCount();
	m_bPlaying = false;
	m_bStoping = true;
	return true;
}

bool SoundInstance::SetLoopOff()
{
	CriticalSectionController csc(&m_csForThreadSafe);
	ChannelList listChannel = m_listChannel;
	ChannelList::const_iterator it = listChannel.begin();
	for (; it!=listChannel.end(); ++it)
	{
		FMOD::Channel* pChannel = *it;
		if(!pChannel)
			continue;
		bool bIsPlaying = false;
		if(FMOD_OK == pChannel->isPlaying(&bIsPlaying))
		{
			if(bIsPlaying)
			{
				FMOD_MODE mode;
				pChannel->getMode(&mode);
				mode |= FMOD_LOOP_OFF;
				if(FMOD_OK != pChannel->setMode(mode))
					continue;
			}
		}
	}
	return true;
}

bool SoundInstance::SetLoopOn()
{
	return true;
	CriticalSectionController csc(&m_csForThreadSafe);
	ChannelList listChannel = m_listChannel;
	ChannelList::const_iterator it = listChannel.begin();
	for (; it!=listChannel.end(); ++it)
	{
		FMOD::Channel* pChannel = *it;
		if(!pChannel)
			continue;
		bool bIsPlaying = false;
		if(FMOD_OK == pChannel->isPlaying(&bIsPlaying))
		{
			if(bIsPlaying)
			{
				FMOD_MODE mode;
				pChannel->getMode(&mode);
				mode |= FMOD_LOOP_NORMAL;
				mode &= ~FMOD_LOOP_OFF;
				if(FMOD_OK != pChannel->setMode(mode))
					continue;
			}			
		}		
	}
	return true;
}

bool SoundInstance::Pause(bool bPaused)
{
	if(!m_pChannelGroup)
		return false;
	if(FMOD_OK != m_pChannelGroup->setPaused(bPaused))
		return false;
	return true;
}

void SoundInstance::release()
{
	CriticalSectionController csc(&m_csForThreadSafe);	
	clearChannel();
	if(m_pChannelGroup)
	{
		CriticalSectionController csc(m_pEventProject->GetEventSystem()->GetCriticalSectionWrapper());
		m_pChannelGroup->stop();
		m_pChannelGroup->release();
	}
	m_pChannelGroup = 0;
}

bool SoundInstance::update(DWORD dwTickTime)
{
	CriticalSectionController csc(&m_csForThreadSafe);
	if(m_bFirstPlay)
	{
		m_iLeftPreIntervalTime -= dwTickTime;
		if(m_iLeftPreIntervalTime <= 0)
		{			
			firstPlay();			
		}
	}
	fadein();
	fadeout();
	return true;
}

void SoundInstance::fadein()
{
	if(!m_bPlaying || m_bStoping)
		return;
	if(!m_pChannelGroup)
		return;
	if(m_SoundInstanceProperty.iFadeInTime == 0)
	{
		m_pChannelGroup->setVolume(1.0f);
	}
	else
	{
		m_pChannelGroup->setVolume(0.0f);
	}
	DWORD dwElapseTime = GetTickCount() - m_dwStartTickCount;
	if(dwElapseTime < (DWORD)m_SoundInstanceProperty.iFadeInTime)
	{
		Curve curve;
		curve.SetType((CURVE_TYPE)m_SoundInstanceProperty.iFadeInCurveType);
		float fVolume = curve.GetValue(0, 0, (float)m_SoundInstanceProperty.iFadeInTime, 1, (float)dwElapseTime);
		if(FMOD_OK != m_pChannelGroup->setVolume(fVolume))
			return;
	}
	else
	{
		if(FMOD_OK != m_pChannelGroup->setVolume(1.0f))
			return;
	}
}

void SoundInstance::fadeout()
{
	if(m_SoundInstanceProperty.iFadeOutTime == 0 || !m_pChannelGroup || !m_bStoping)
		return;
	DWORD dwElapseTime = GetTickCount() - m_dwStopTickCount;
	if(dwElapseTime > (DWORD)m_SoundInstanceProperty.iFadeOutTime)
	{
		if(FMOD_OK != m_pChannelGroup->setVolume(0.0f))
			return;
		if(!Stop(true))
			return;
	}
	else
	{
		Curve curve;
		curve.SetType((CURVE_TYPE)m_SoundInstanceProperty.iFadeOutCurveType);
		float fVolume = curve.GetValue(0, 1, (float)m_SoundInstanceProperty.iFadeOutTime, 0, (float)dwElapseTime);
		if(FMOD_OK != m_pChannelGroup->setVolume(fVolume))
			return;
	}
}

void SoundInstance::AddListener(SoundInstanceListener* pListener)
{
	CriticalSectionController csc(&m_csForThreadSafe);
	if(!pListener)
		return;	
	m_setListener.insert(pListener);
}

void SoundInstance::RemoveListener(SoundInstanceListener* pListener)
{
	CriticalSectionController csc(&m_csForThreadSafe);
	ListenerSet::iterator it = m_setListener.find(pListener);
	if(it != m_setListener.end())
		m_setListener.erase(m_setListener.find(pListener));
}

bool SoundInstance::listenerPlay()
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

bool SoundInstance::listenerStop()
{
	CriticalSectionController csc(&m_csForThreadSafe);
	ListenerSet setListener = m_setListener;
	ListenerSet::const_iterator it = setListener.begin();
	for (; it!=setListener.end(); ++it)
	{
		if(!(*it)->OnStop(this))
			continue;
	}
	return true;
}

int SoundInstance::GetIntervalTime() const
{
	if(!m_pSoundInstanceTemplate)
		return 0;
	SoundDef* pSoundDef = m_pSoundInstanceTemplate->GetSoundDef();
	if(!pSoundDef)
		return 0;
	SOUND_DEF_PROPERTY soundDefProp;
	pSoundDef->GetProperty(soundDefProp);
	if(soundDefProp.iIntervalTime + soundDefProp.iIntervalTimeRandomization + m_SoundInstanceProperty.iIntervalTime + m_SoundInstanceProperty.iIntervalTimeRandomization == 0)
		return 0;
	int iBase = soundDefProp.iIntervalTime + m_SoundInstanceProperty.iIntervalTime;
	int iRandomization = soundDefProp.iIntervalTimeRandomization + m_SoundInstanceProperty.iIntervalTimeRandomization;
	return abs((int)((float)rand()*iRandomization*2.0f/RAND_MAX + iBase - iRandomization + 0.5f));
}

int SoundInstance::GetPreIntervalTime() const
{
	if(m_SoundInstanceProperty.iPreIntervalTime + m_SoundInstanceProperty.iPreIntervalTimeRandomization == 0)
		return 0;
	int iBase = m_SoundInstanceProperty.iPreIntervalTime;
	int iRandomization = m_SoundInstanceProperty.iPreIntervalTimeRandomization;
	return abs((int)((float)rand()*iRandomization*2.0f/RAND_MAX + iBase - iRandomization + 0.5f));

}

void SoundInstance::AddChannel(FMOD::Channel* pChannel)
{
	CriticalSectionController csc(&m_csForThreadSafe);
	if(!pChannel)
		return;
	m_listChannel.push_back(pChannel);
}

void SoundInstance::RemoveChannel(FMOD::Channel* pChannel)
{
	CriticalSectionController csc(&m_csForThreadSafe);
	ChannelList::iterator it = m_listChannel.begin();
	for(; it!=m_listChannel.end(); ++it)
	{
		if(pChannel == *it)
		{
			if(pChannel)
			{
				pChannel->setUserData(0);
				pChannel->setCallback(0);
				bool bIsPlaying = false;
				if(FMOD_OK == pChannel->isPlaying(&bIsPlaying))
				{
					if(bIsPlaying)
					{
						CriticalSectionController csc(m_pEventProject->GetEventSystem()->GetCriticalSectionWrapper());
						pChannel->stop();						
					}
				}
			}
			int iAudioInstanceNum = m_pSoundInstanceTemplate->GetAudioInstanceNum();
			for (int i=0; i<iAudioInstanceNum; ++i)
			{
				AudioInstance* pAudioInstance = m_pSoundInstanceTemplate->GetAudioInstanceByIndex(i, true, false);
				if(!pAudioInstance)
					continue;
				pAudioInstance->RemoveChannel(pChannel);
			}
			m_listChannel.erase(it);
			break;
		}
	}
}

void SoundInstance::clearChannel()
{
	CriticalSectionController csc(&m_csForThreadSafe);
	ChannelList listChannel = m_listChannel;
	ChannelList::const_iterator it = listChannel.begin();
	for (; it!=listChannel.end(); ++it)
	{
		FMOD::Channel* pChannel = *it;
		RemoveChannel(pChannel);
	}
	m_listChannel.clear();
}

void SoundInstance::GetPlayingChannelList(ChannelList& listPlayingChannel)
{
	CriticalSectionController csc(&m_csForThreadSafe);
	ChannelList listChannel = m_listChannel;
	ChannelList::const_iterator it = listChannel.begin();
	for (; it!=listChannel.end(); ++it)
	{
		FMOD::Channel* pChannel = *it;
		bool bIsPlaying = false;
		if(FMOD_OK == pChannel->isPlaying(&bIsPlaying))
		{
			if(bIsPlaying)
				listPlayingChannel.push_back(pChannel);
		}
	}
}