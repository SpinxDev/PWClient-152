#include "FEventLayerInstance.h"
#include "FSoundInstance.h"
#include <fmod.hpp>
#include "FEventLayer.h"
#include "FEventSystem.h"
#include "FEventProject.h"
#include "FSoundInstanceTemplate.h"
#include "FEffect.h"
#include "FEventInstance.h"

using namespace AudioEngine;

EventLayerInstance::EventLayerInstance(void)
: m_pChannelGroup(0)
, m_bPlaying(false)
, m_bStoping(false)
, m_pEventLayer(0)
, m_pEventProject(0)
, m_pEventInstance(0)
, m_pEventInstanceParameter(0)
, m_pLastInstance(0)
, m_bBlockListenerStop(false)
{
}

EventLayerInstance::~EventLayerInstance(void)
{
	listenerStop();
	release();
}

bool EventLayerInstance::Init(EventLayer* pEventLayer, EventProject* pEventProject)
{
	CriticalSectionController csc(&m_csForThreadSafe);
	if(!pEventLayer || !pEventProject)
		return false;	
	release();

	m_pEventLayer = pEventLayer;
	m_pEventProject = pEventProject;

	EventSystem* pEventSystem = m_pEventProject->GetEventSystem();
	if(!pEventSystem)
		return false;

	FMOD::System* pSystem = pEventSystem->GetSystem();
	if(!pSystem)
		return false;
	{
		CriticalSectionController csces(m_pEventProject->GetEventSystem()->GetCriticalSectionWrapper());
		if(FMOD_OK != pSystem->createChannelGroup("", &m_pChannelGroup))
			return false;
	}	
	
	int iSoundInsNum = m_pEventLayer->GetSoundInstanceTemplateNum();
	for (int i=0; i<iSoundInsNum; ++i)
	{
		SoundInstanceTemplate* pSoundInstanceTemplate = m_pEventLayer->GetSoundInstanceTemplateByIndex(i);
		if(!pSoundInstanceTemplate)
			continue;
		SoundInstance* pSoundInstance = new SoundInstance;
		if(!pSoundInstance->Init(this, pSoundInstanceTemplate, m_pEventProject))
		{
			delete pSoundInstance;
			return false;
		}
		m_listSoundInstance.push_back(pSoundInstance);
	}
	SoundInstanceList::const_iterator itSoundIns = m_listSoundInstance.begin();
	for (; itSoundIns!=m_listSoundInstance.end(); ++itSoundIns)
	{
		SoundInstance* pSoundInstance = *itSoundIns;
		if(!pSoundInstance)
			continue;
		{
			CriticalSectionController csces(m_pEventProject->GetEventSystem()->GetCriticalSectionWrapper());
			m_pChannelGroup->addGroup(pSoundInstance->GetChannelGroup());
		}		
	}
	return true;
}

bool EventLayerInstance::AttachInstanceParameter(const char* szName)
{
	if(!m_pEventInstance)
		return false;
	m_pEventInstanceParameter = m_pEventInstance->GetInstanceParameterByName(szName);
	if(!m_pEventInstanceParameter)
		return false;
	m_pEventInstanceParameter->AddListener((EventParameter::EventParameterListener*)this);
	return true;
}

void EventLayerInstance::release()
{
	CriticalSectionController csc(&m_csForThreadSafe);
	SoundInstanceList::const_iterator it = m_listSoundInstance.begin();
	for (; it!=m_listSoundInstance.end(); ++it)
	{
		SoundInstance* pSoundInstance = *it;
		if(pSoundInstance)
			pSoundInstance->RemoveListener(this);
		delete pSoundInstance;
	}
	m_listSoundInstance.clear();

	if(m_pEventInstanceParameter)
		m_pEventInstanceParameter->RemoveListener((EventParameter::EventParameterListener*)this);
	if(m_pChannelGroup)
	{
		CriticalSectionController csc(m_pEventProject->GetEventSystem()->GetCriticalSectionWrapper());
		m_pChannelGroup->stop();
		m_pChannelGroup->release();
	}
	m_pChannelGroup = 0;
}

bool EventLayerInstance::SetMute(bool bMute)
{
	if(!m_pChannelGroup)
		return false;
	if(FMOD_OK != m_pChannelGroup->setMute(bMute))
		return false;
	return true;
}

bool EventLayerInstance::listenerPlay()
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

bool EventLayerInstance::listenerStop()
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

void EventLayerInstance::AddListener(EventLayerInstanceListener* pListener)
{
	CriticalSectionController csc(&m_csForThreadSafe);
	if(!pListener)
		return;	
	m_setListener.insert(pListener);
}

void EventLayerInstance::RemoveListener(EventLayerInstanceListener* pListener)
{
	CriticalSectionController csc(&m_csForThreadSafe);
	ListenerSet::iterator it = m_setListener.find(pListener);
	if(it != m_setListener.end())
		m_setListener.erase(m_setListener.find(pListener));
}

bool EventLayerInstance::OnPlay(SoundInstance* pSoundInstance)
{
	CriticalSectionController csc(&m_csForThreadSafe);
	if(!pSoundInstance)
		return false;	
	m_setCurInstances.insert(pSoundInstance);
	m_bPlaying = true;
	m_bStoping = false;
	return true;
}

bool EventLayerInstance::OnStop(SoundInstance* pSoundInstance)
{
	CriticalSectionController csc(&m_csForThreadSafe);
	if(!pSoundInstance)
		return false;
	pSoundInstance->RemoveListener(this);
	SoundInstanceSet::iterator it = m_setCurInstances.find(pSoundInstance);
	if(it != m_setCurInstances.end())
	{		
		m_setCurInstances.erase(it);
	}
	if(m_setCurInstances.size() == 0)
	{

		m_bStoping = false;
		if(m_bPlaying)
		{
			m_bPlaying = false;
			if(!m_bBlockListenerStop)
				listenerStop();
		}
	}
	return true;
}

bool EventLayerInstance::OnPlay(AudioInstance* pAudioInstance)
{
	CriticalSectionController csc(&m_csForThreadSafe);
	if(!pAudioInstance || !m_pEventLayer)
		return false;	
	pAudioInstance->RemoveListener(this);
	float fNormalizeValue = getNormalizeParamValue();
	if(!applyEffects(fNormalizeValue))
		return false;
	return true;
}

bool EventLayerInstance::play()
{
	CriticalSectionController csc(&m_csForThreadSafe);
	AddListener((EventLayerInstanceListener*)m_pEventInstance);	
	SoundInstance* pCurSoundInstance = GetCurSoundInstance();	
	if(!pCurSoundInstance)
		return true;
	if(!pCurSoundInstance->playForEvent())
		return false;
	if(!listenerPlay())
		return false;
	return true;
}

bool EventLayerInstance::pause(bool bPause)
{
	SoundInstance* pCurSoundInstance = GetCurSoundInstance();
	if(!pCurSoundInstance)
		return true;
	if(!pCurSoundInstance->Pause(bPause))
		return false;
	return true;
}

bool EventLayerInstance::stop(bool bForce /*= false*/, bool bEventStop /*= false*/)
{
	CriticalSectionController csc(&m_csForThreadSafe);
	if(m_setCurInstances.size() == 0)
		return true;
	m_bStoping = true;
	m_bPlaying = false;
	if(bForce)
	{
		m_bStoping = false;
		removeEffects();
	}
	SoundInstanceSet setCurInstances = m_setCurInstances;
	SoundInstanceSet::const_iterator it = setCurInstances.begin();
	for (; it!=setCurInstances.end(); ++it)
	{
		SoundInstance* pSoundInstance = *it;
		if(!pSoundInstance)
			continue;
		if(!pSoundInstance->Stop(bForce, bEventStop))
			return false;
	}
	if(!listenerStop())
		return false;
	return true;
}

SoundInstance* EventLayerInstance::GetCurSoundInstance() const
{
	if(!m_pEventInstanceParameter)
		return 0;
	float value = m_pEventInstanceParameter->GetValue();
	EVENT_PARAM_PROPERTY eventParamProp;
	m_pEventInstanceParameter->GetProperty(eventParamProp);
	float fNormalizeValue = (value - eventParamProp.fMinValue) / (eventParamProp.fMaxValue - eventParamProp.fMinValue);
	SoundInstanceList::const_iterator it = m_listSoundInstance.begin();
	for (; it!=m_listSoundInstance.end(); ++it)
	{
		SoundInstance* pSoundInstance = *it;
		if(!pSoundInstance)
			continue;
		SOUND_INSTANCE_PROPERTY soundInsProp;
		pSoundInstance->GetSoundInstanceTemplate()->GetProperty(soundInsProp);
		if(fNormalizeValue <= soundInsProp.fLength + soundInsProp.fStartPosition && fNormalizeValue >= soundInsProp.fStartPosition)
			return pSoundInstance;
	}
	return 0;
}

bool EventLayerInstance::update(DWORD dwTickTime)
{
	CriticalSectionController csc(&m_csForThreadSafe);
	if(!m_bPlaying)
		return true;
	SoundInstance* pCurSoundInstance = GetCurSoundInstance();
	m_pLastInstance = pCurSoundInstance;
	if(pCurSoundInstance && m_setCurInstances.find(pCurSoundInstance) == m_setCurInstances.end())
	{
		SOUND_INSTANCE_PROPERTY newProp;
		pCurSoundInstance->GetSoundInstanceTemplate()->GetProperty(newProp);
		if(newProp.startMode == START_MODE_IMMEDIATE || m_setCurInstances.size() == 0)
		{
			if(!m_bStoping)
			{				
				m_setCurInstances.insert(pCurSoundInstance);				
			}
		}

		std::vector<SoundInstance*> stopVector;
		SoundInstanceSet::const_iterator it = m_setCurInstances.begin();
		for (; it!=m_setCurInstances.end(); ++it)
		{
			SoundInstance* pSoundInstance = *it;
			if(!pSoundInstance)
				continue;
			if(pSoundInstance == pCurSoundInstance)
				continue;
			SOUND_INSTANCE_PROPERTY prop;
			pSoundInstance->GetSoundInstanceTemplate()->GetProperty(prop);
			if(prop.stopMode == STOP_MODE_CUTOFF)
			{
				stopVector.push_back(pSoundInstance);
			}
			else if(prop.stopMode == STOP_MODE_PLAYTOEND)
			{
				if(!pSoundInstance->IsPlaying())
					stopVector.push_back(pSoundInstance);
				if(!pSoundInstance->SetLoopOff())
					return false;
			}
		}

		size_t iStopNum = stopVector.size();
		size_t iCurInsNum = m_setCurInstances.size();

		m_bBlockListenerStop = true;
		for (size_t i=0; i<iStopNum; ++i)
		{
			if(!stopVector[i]->Stop())
				return false;
		}
		m_bBlockListenerStop = false;

		if(newProp.startMode == START_MODE_IMMEDIATE || iCurInsNum == 0)
		{
			if(!m_bStoping)
			{
				if(!play())
					return false;
			}
		}

		if(newProp.startMode == START_MODE_WAIT_FOR_PREVIOUS && iCurInsNum == iStopNum && !m_bStoping)
		{
			if(!m_bStoping)
			{
				if(!play())
					return false;
			}
		}
	}
	else
	{
		if(pCurSoundInstance)
		{
			if(!pCurSoundInstance->SetLoopOn())
				return false;
		}
		else
		{
			m_bPlaying = false;
		}
		std::vector<SoundInstance*> stopVector;
		SoundInstanceSet::const_iterator it = m_setCurInstances.begin();
		for (; it!=m_setCurInstances.end(); ++it)
		{
			SoundInstance* pSoundInstance = *it;
			if(!pSoundInstance)
				continue;
			if(pSoundInstance == pCurSoundInstance)
				continue;
			SOUND_INSTANCE_PROPERTY prop;
			pSoundInstance->GetSoundInstanceTemplate()->GetProperty(prop);
			if(prop.stopMode == STOP_MODE_CUTOFF)
			{
				stopVector.push_back(pSoundInstance);
			}
			else if(prop.stopMode == STOP_MODE_PLAYTOEND)
			{
				if(!pSoundInstance->IsPlaying())
					stopVector.push_back(pSoundInstance);
			}
		}

		for (size_t i=0; i<stopVector.size(); ++i)
		{
			if(!stopVector[i]->Stop())
				return false;
		}
	}

	SoundInstanceSet setCurInstances = m_setCurInstances;

	SoundInstanceSet::const_iterator it = setCurInstances.begin();
	for (; it!=setCurInstances.end(); ++it)
	{
		SoundInstance* pSoundInstance = *it;
		if(!pSoundInstance)
			continue;
		if(!pSoundInstance->update(dwTickTime))			
			return false;
	}
	if(m_setCurInstances.size() == 0 && m_bPlaying)
	{
		if(!stop(true))
			return false;
	}
	return true;
}

bool EventLayerInstance::loadData()
{
	CriticalSectionController csc(&m_csForThreadSafe);
	SoundInstanceList::const_iterator it = m_listSoundInstance.begin();
	for (; it!=m_listSoundInstance.end(); ++it)
	{
		SoundInstance* pSoundInstance = *it;
		if(!pSoundInstance)
			continue;
		if(!pSoundInstance->GetSoundInstanceTemplate()->loadData())
			return false;
	}
	return true;
}

bool EventLayerInstance::applyEffects(float fParamValue)
{
	if(!m_pChannelGroup)
		return false;

	for (int i=0; i<m_pEventLayer->GetEffectNum(); ++i)
	{
		Effect* pEffect = m_pEventLayer->GetEffectByIndex(i);
		if(!pEffect)
			continue;
		if(!pEffect->IsEnable())
			continue;
		if(!pEffect->Apply(m_pChannelGroup, fParamValue))
			continue;
	}
	
	return true;
}

bool EventLayerInstance::removeEffects()
{
	if(!m_pChannelGroup)
		return false;

	for (int i=0; i<m_pEventLayer->GetEffectNum(); ++i)
	{
		Effect* pEffect = m_pEventLayer->GetEffectByIndex(i);
		if(!pEffect)
			continue;
		if(!pEffect->Remove(m_pChannelGroup))
			continue;
	}

	return true;
}

bool EventLayerInstance::ApplyEffect(Effect* pEffect)
{
	if(!pEffect || !m_pChannelGroup)
		return false;
	float fNormalizeValue = getNormalizeParamValue();
	pEffect->Enable(true);
	if(!pEffect->Apply(m_pChannelGroup, fNormalizeValue))
		return false;
	return true;
}

bool EventLayerInstance::RemoveEffect(Effect* pEffect)
{
	if(!pEffect || !m_pChannelGroup)
		return false;
	pEffect->Enable(false);
	if(!pEffect->Remove(m_pChannelGroup))
		return false;
	return true;
}

float EventLayerInstance::getNormalizeParamValue() const
{
	if(!m_pEventInstanceParameter)
		return 0;
	float value = m_pEventInstanceParameter->GetValue();
	EVENT_PARAM_PROPERTY eventParamProp;
	m_pEventInstanceParameter->GetProperty(eventParamProp);
	return (value - eventParamProp.fMinValue) / (eventParamProp.fMaxValue - eventParamProp.fMinValue);
}

bool EventLayerInstance::OnValueChanged(EventParameter* pEventParameter)
{
	CriticalSectionController csc(&m_csForThreadSafe);
	if(!GetEventInstance()->IsPlaying())
		return false;
	else
	{
		if(m_pLastInstance != GetCurSoundInstance())
			m_bPlaying = true;
	}
	if(!update(0))
		return false;
	float fNormalizeValue = getNormalizeParamValue();
	if(!applyEffects(fNormalizeValue))
		return false;
	return true;
}