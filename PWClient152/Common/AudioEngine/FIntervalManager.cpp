#include "FIntervalManager.h"
#include "FSoundInstanceTemplate.h"
#include "FAudioInstance.h"
#include "FEventLayerInstance.h"
#include "FEventInstance.h"
#include "FEvent.h"

using namespace AudioEngine;

IntervalManager::IntervalManager(void)
{
}

IntervalManager::~IntervalManager(void)
{
	release();
}

IntervalManager& IntervalManager::GetInstance()
{
	static IntervalManager instance;
	return instance;
}

void IntervalManager::release()
{
	CriticalSectionController csc(&m_csForThreadSafe);
	SoundInstanceSet setSoundInstance = m_setSoundInstance;
	SoundInstanceSet::iterator it = setSoundInstance.begin();
	for (; it!=setSoundInstance.end(); ++it)
	{
		SoundInstance* pSoundInstance = *it;
		if(!pSoundInstance)
			continue;
		EventLayerInstance* pEventLayerInstance = pSoundInstance->GetEventLayerInstance();
		if(!pEventLayerInstance)
			continue;
		EventInstance* pEventInstance = pEventLayerInstance->GetEventInstance();
		if(!pEventInstance)
			continue;
		Event* pEvent = pEventInstance->GetEvent();
		if(!pEvent)
			continue;
		CriticalSectionController csce(pEvent->GetCriticalSectionWrapper());
		CriticalSectionController cscei(pEventInstance->GetCriticalSectionWrapper());
		CriticalSectionController csceli(pEventLayerInstance->GetCriticalSectionWrapper());
		CriticalSectionController cscsi(pSoundInstance->GetCriticalSectionWrapper());
		pSoundInstance->RemoveListener(this);
	}
	m_setSoundInstance.clear();
}

bool IntervalManager::OnStop(SoundInstance* pSoundInstance)
{
	CriticalSectionController csc(&m_csForThreadSafe);
	if(!pSoundInstance)
		return false;
	pSoundInstance->RemoveListener(this);
	RemoveSoundInstance(pSoundInstance);
	return true;
}

void IntervalManager::AddSoundInstance(SoundInstance* pSoundInstance)
{
	CriticalSectionController csc(&m_csForThreadSafe);
	if(!pSoundInstance)
		return;	
	pSoundInstance->AddListener(this);
	m_setSoundInstance.insert(pSoundInstance);
}

void IntervalManager::RemoveSoundInstance(SoundInstance* pSoundInstance)
{
	CriticalSectionController csc(&m_csForThreadSafe);

	if(!pSoundInstance)
		return;
	EventLayerInstance* pEventLayerInstance = pSoundInstance->GetEventLayerInstance();
	if(!pEventLayerInstance)
		return;
	EventInstance* pEventInstance = pEventLayerInstance->GetEventInstance();
	if(!pEventInstance)
		return;
	Event* pEvent = pEventInstance->GetEvent();
	if(!pEvent)
		return;	
	CriticalSectionController csce(pEvent->GetCriticalSectionWrapper());
	CriticalSectionController cscei(pEventInstance->GetCriticalSectionWrapper());
	CriticalSectionController csceli(pEventLayerInstance->GetCriticalSectionWrapper());
	CriticalSectionController cscsi(pSoundInstance->GetCriticalSectionWrapper());
	pSoundInstance->RemoveListener(this);
	SoundInstanceSet::iterator it = m_setSoundInstance.find(pSoundInstance);
	if(it != m_setSoundInstance.end())
	{
		pSoundInstance->SetLeftIntervalTime(0);
		m_setSoundInstance.erase(it);
	}
}

void IntervalManager::Update(DWORD dwEllapseTime)
{
	CriticalSectionController csc(&m_csForThreadSafe);
	SoundInstanceSet setDeleteSoundInstance;
	SoundInstanceSet setSoundInstance = m_setSoundInstance;
	SoundInstanceSet::iterator it = setSoundInstance.begin();
	for (; it!=setSoundInstance.end(); ++it)
	{
		SoundInstance* pSoundInstance = *it;
		if(!pSoundInstance)
			continue;
		EventLayerInstance* pEventLayerInstance = pSoundInstance->GetEventLayerInstance();
		if(!pEventLayerInstance)
			continue;
		EventInstance* pEventInstance = pEventLayerInstance->GetEventInstance();
		if(!pEventInstance)
			continue;
		Event* pEvent = pEventInstance->GetEvent();
		if(!pEvent)
			continue;
		CriticalSectionController csce(pEvent->GetCriticalSectionWrapper());
		CriticalSectionController cscei(pEventInstance->GetCriticalSectionWrapper());
		CriticalSectionController csceli(pEventLayerInstance->GetCriticalSectionWrapper());
		CriticalSectionController cscsi(pSoundInstance->GetCriticalSectionWrapper());
		pSoundInstance->EllapseIntervalTime(dwEllapseTime);
		if(pSoundInstance->GetLeftIntervalTime() <= 0)
		{
			setDeleteSoundInstance.insert(pSoundInstance);
			process(pSoundInstance);
		}
	}

	SoundInstanceSet::iterator itDel = setDeleteSoundInstance.begin();
	for (; itDel!=setDeleteSoundInstance.end(); ++itDel)
	{
		RemoveSoundInstance(*itDel);
	}
}

void IntervalManager::process(SoundInstance* pSoundInstance)
{
	CriticalSectionController csc(&m_csForThreadSafe);
	if(!pSoundInstance)
		return;
	EventLayerInstance* pEventLayerInstance = pSoundInstance->GetEventLayerInstance();
	if(!pEventLayerInstance)
		return;
	EventInstance* pEventInstance = pEventLayerInstance->GetEventInstance();
	if(!pEventInstance)
		return;
	Event* pEvent = pEventInstance->GetEvent();
	if(!pEvent)
		return;	
	CriticalSectionController csce(pEvent->GetCriticalSectionWrapper());
	CriticalSectionController cscei(pEventInstance->GetCriticalSectionWrapper());
	CriticalSectionController csceli(pEventLayerInstance->GetCriticalSectionWrapper());
	CriticalSectionController cscsi(pSoundInstance->GetCriticalSectionWrapper());

	if(!pSoundInstance->IsPlaying())
	{
		if(!pSoundInstance->Stop(true))
			return;
		return;
	}
	int iNextPlayIndex;
	int iNextPlayLoopCount;
	if(!pSoundInstance->GetSoundPlayList().GetNextIndex(iNextPlayIndex, iNextPlayLoopCount))
	{
		if(!pSoundInstance->Stop(true))
			return;
		return;
	}

	{
		if(pSoundInstance->IsPlaying() || pSoundInstance->IsStoping())
		{
			AudioInstance* pNextAudioInstance = pSoundInstance->GetSoundInstanceTemplate()->GetAudioInstanceByIndex(iNextPlayIndex, pSoundInstance->IsPlayForEvent(), true);
			if(pSoundInstance->IsPlayForEvent())
			{
				if(pNextAudioInstance && !pNextAudioInstance->PlayForEvent(pSoundInstance, iNextPlayLoopCount))
					return;
			}
			else
			{
				if(pNextAudioInstance && !pNextAudioInstance->PlayForDef(pSoundInstance, iNextPlayLoopCount))
					return;
			}		
		}
	}
}