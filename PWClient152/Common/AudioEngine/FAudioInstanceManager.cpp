#include "FAudioInstanceManager.h"
#include "FAudioInstance.h"

using namespace AudioEngine;

AudioInstanceManager::AudioInstanceManager(void)
: m_pEventSystem(0)
{
}

AudioInstanceManager::~AudioInstanceManager(void)
{
	Release();
}

AudioInstanceManager& AudioInstanceManager::GetInstance()
{
	static AudioInstanceManager instance;
	return instance;
}

bool AudioInstanceManager::Init(EventSystem* pEventSystem)
{
	if(!pEventSystem)
		return false;
	m_pEventSystem = pEventSystem;
	return true;
}

AudioInstance* AudioInstanceManager::GetAudioInstance(AudioTemplate* pAudioTemplate, bool bPlayForEvent, bool bReload)
{
	CriticalSectionController csc(&m_csForThreadSafe);
	if(!m_pEventSystem || !pAudioTemplate)
		return 0;
	AudioInstanceMap::const_iterator it = m_mapAudioInstance.find(pAudioTemplate);
	if(it == m_mapAudioInstance.end())
	{
		AudioInstance* pAudioInstance = new AudioInstance;
		if(!pAudioInstance->Init(m_pEventSystem, pAudioTemplate))
		{
			delete pAudioInstance;
			return 0;
		}		
		if(bPlayForEvent)
		{
			if(!pAudioInstance->createSoundForEvent(bReload))
			{
				delete pAudioInstance;
				return 0;
			}
		}
		else
		{
			if(!pAudioInstance->createSoundForSoundDef(bReload))
			{
				delete pAudioInstance;
				return 0;
			}
		}
		m_mapAudioInstance[pAudioTemplate] = pAudioInstance;
		return pAudioInstance;
	}
	else
	{
		AudioInstance* pAudioInstance = it->second;
		return pAudioInstance;
	}
}

bool AudioInstanceManager::Update(DWORD dwTickTime)
{
	AudioInstanceMap mapAudioInstance = m_mapAudioInstance;
	AudioInstanceMap::const_iterator it = mapAudioInstance.begin();
	for (; it!=mapAudioInstance.end(); ++it)
	{
		AudioInstance* pAudioInstance = it->second;
		pAudioInstance->Update(dwTickTime);
		if(pAudioInstance->IsCanbeDeleted())
			releaseAudioInstance(pAudioInstance);
	}
	return true;
}

void AudioInstanceManager::releaseAudioInstance(AudioInstance* pAudioInstance)
{
	if(!pAudioInstance)
		return;
	AudioInstanceMap::iterator it = m_mapAudioInstance.find(pAudioInstance->GetAudioTemplate());
	if(it != m_mapAudioInstance.end())
	{
		delete pAudioInstance;
		m_mapAudioInstance.erase(it);
	}
}

void AudioInstanceManager::Release()
{
	AudioInstanceMap mapAudioInstance = m_mapAudioInstance;
	AudioInstanceMap::const_iterator it = mapAudioInstance.begin();
	for (; it!=mapAudioInstance.end(); ++it)
	{
		AudioInstance* pAudioInstance = it->second;
		releaseAudioInstance(pAudioInstance);
	}
	m_mapAudioInstance.clear();
}