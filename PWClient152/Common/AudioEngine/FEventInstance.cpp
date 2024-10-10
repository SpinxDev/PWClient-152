//#include "AGPAPerfIntegration.h"
#include "FEventInstance.h"
#include "FEventLayerInstance.h"
#include "FEvent.h"
#include "FEventProject.h"
#include "FEventSystem.h"
#include "FEventParameter.h"
#include "FCurve.h"
#include "FEventGroup.h"
#include "FSoundInstanceTemplate.h"
#include <fmod.hpp>

using namespace AudioEngine;

EventInstance::EventInstance(void)
: m_bPlaying(false)
, m_bStopping(false)
, m_bPaused(false)
, m_pChannelGroup(0)
, m_LayerIndex(LAYER_ALL)
, m_pEventProject(0)
, m_pEvent(0)
, m_dwStartTickCount(0)
, m_dwStopTickCount(0)
, m_bAutoDestroy(true)
, m_vPos(0)
, m_vVel(0)
, m_vLastPos(0)
, m_vLastVel(0)
, m_vOrientation(0, 0, 1)
, m_vLastOrientation(0, 0, 1)
{
	m_bEnableOrientation = false;
	memset(&m_EventProperty, 0, sizeof(m_EventProperty));
	m_EventProperty.bOneShot = true;
	m_EventProperty.fMinDistance = 1.0f;
	m_EventProperty.fMaxDistance = 100.0f;
	m_EventProperty.mode = MODE_2D;
	m_EventProperty.rolloff = ROLLOFF_INVERSE;
	m_EventProperty.maxPlaybacksBehavior = MPB_STEAL_OLDEST;
	m_EventProperty.uiMaxPlaybacks = 10;
	m_EventProperty.iPriority = 10000;
	m_EventProperty.f3DConeInsideAngle = 360.0f;
	m_EventProperty.f3DConeOutsideAngle = 360.0f;
	m_EventProperty.f3DConeOutsideVolume = 1.0f;
	m_fVolume = 1.0f;
	m_fLastFadeInVolume = 1.0f;
}

EventInstance::~EventInstance(void)
{
//	listenerStop();
	release();
}

bool EventInstance::Init(Event* pEvent, EventProject* pEventProject, bool bAutoDestroy /*= true*/)
{
	CriticalSectionController csc(&m_csForThreadSafe);
	release();
	if(!pEventProject || !pEvent)
		return false;
	m_bAutoDestroy = bAutoDestroy;
	m_pEvent = pEvent;
	m_pEvent->GetProperty(m_EventProperty);
	m_pEventProject = pEventProject;
	EventSystem* pEventSystem = m_pEventProject->GetEventSystem();
	FMOD::System* pSystem = pEventSystem->GetSystem();
	if(!pSystem)
		return false;
	{
		CriticalSectionController csces(pEventSystem->GetCriticalSectionWrapper());
		if(FMOD_OK != pSystem->createChannelGroup("", &m_pChannelGroup))
			return false;
		pEvent->GetParentGroup()->GetChannelGroup()->addGroup(m_pChannelGroup);
	}

	int iParamNum = m_pEvent->GetParameterNum();
	for (int i=0; i<iParamNum; ++i)
	{
		EventParameter* pEventParameter = m_pEvent->GetParameterByIndex(i);
		if(!pEventParameter)
			return false;
		EventInstanceParameter* pEventInstanceParameter = new EventInstanceParameter;
		EVENT_PARAM_PROPERTY prop;
		pEventParameter->GetProperty(prop);
		pEventInstanceParameter->SetProperty(prop);
		pEventInstanceParameter->SetName(pEventParameter->GetName());
		pEventInstanceParameter->SetEvent(m_pEvent);
		pEventInstanceParameter->SetValue(pEventParameter->GetValue(), false);
		m_listEventInstanceParameter.push_back(pEventInstanceParameter);
	}


	int iLayerNum = m_pEvent->GetLayerNum();
	for (int i=0; i<iLayerNum; ++i)
	{
		EventLayer* pEventLayer = m_pEvent->GetLayerByIndex(i);
		EventLayerInstance* pEventLayerInstance = new EventLayerInstance;
		if(!pEventLayerInstance->Init(pEventLayer, m_pEventProject))
		{
			delete pEventLayerInstance;
			return false;
		}
		pEventLayerInstance->setEventInstance(this);
		if(!pEventLayerInstance->AttachInstanceParameter(pEventLayer->GetParameterName()))
		{
			delete pEventLayerInstance;
			return false;
		}
		m_listEventLayerInstance.push_back(pEventLayerInstance);
	}	
	return true;
}

void EventInstance::release()
{
	CriticalSectionController csc(&m_csForThreadSafe);
	EventLayerInstanceList::const_iterator itLayer = m_listEventLayerInstance.begin();
	for (; itLayer!=m_listEventLayerInstance.end(); ++itLayer)
	{
		EventLayerInstance* pEventLayerInstance = *itLayer;
		if(!pEventLayerInstance)
			continue;
		pEventLayerInstance->RemoveListener(this);
		delete pEventLayerInstance;
	}
	m_listEventLayerInstance.clear();

	EventInstanceParameterList::const_iterator itParam = m_listEventInstanceParameter.begin();
	for (; itParam!=m_listEventInstanceParameter.end(); ++itParam)
	{
		EventInstanceParameter* pEventInstanceParameter = *itParam;
		if(!pEventInstanceParameter)
			continue;
		delete pEventInstanceParameter;
	}
	m_listEventInstanceParameter.clear();

	if(m_pChannelGroup)
	{
		CriticalSectionController csc(m_pEventProject->GetEventSystem()->GetCriticalSectionWrapper());
		m_pChannelGroup->stop();
		m_pChannelGroup->release();
	}
	m_bPlaying = false;
	m_pChannelGroup = 0;
}

bool EventInstance::IsAutoDestroy() const
{
	return m_bAutoDestroy;
}

Event* EventInstance::GetEvent() const
{
	return m_pEvent;
}

EventInstanceParameter* EventInstance::GetInstanceParameterByName(const char* szName) const
{
	EventInstanceParameterList::const_iterator it = m_listEventInstanceParameter.begin();
	for (; it!=m_listEventInstanceParameter.end(); ++it)
	{
		EventInstanceParameter* pEventInstanceParameter = *it;
		if(!pEventInstanceParameter)
			continue;
		if(0 == strcmp(pEventInstanceParameter->GetName(), szName))
			return pEventInstanceParameter;
	}
	return 0;
}

EventInstanceParameter* EventInstance::GetInstanceParameterByIndex(int idx) const
{
	EventInstanceParameterList::const_iterator it = m_listEventInstanceParameter.begin();
	for (int i=0; it!=m_listEventInstanceParameter.end(); ++it,++i)
	{
		if(i == idx)
			return *it;
	}
	return 0;
}

int EventInstance::GetInstanceParameterNum() const
{
	return static_cast<int>(m_listEventInstanceParameter.size());
}

bool EventInstance::OnPlay(EventLayerInstance* pEventLayerInstance)
{	
	if(!pEventLayerInstance)
		return false;
	CriticalSectionController csc(&m_csForThreadSafe);
	m_setPlayingLayerInstance.insert(pEventLayerInstance);	
	return true;
}

bool EventInstance::OnStop(EventLayerInstance* pEventLayerInstance)
{
	CriticalSectionController csc(&m_csForThreadSafe);
	if(pEventLayerInstance)
		pEventLayerInstance->RemoveListener(this);
	EventLayerInstanceSet::iterator it = m_setPlayingLayerInstance.find(pEventLayerInstance);
	if(it != m_setPlayingLayerInstance.end())
	{		
		m_setPlayingLayerInstance.erase(it);
	}	
	if(m_setPlayingLayerInstance.size() <= 0)
	{
		listenerStop();
	}
	return true;
}

bool EventInstance::listenerPlay()
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

bool EventInstance::listenerStop()
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

void EventInstance::AddListener(EventInstanceListener* pListener)
{
	if(!pListener)
		return;
	CriticalSectionController csc(&m_csForThreadSafe);
	m_setListener.insert(pListener);	
}

void EventInstance::RemoveListener(EventInstanceListener* pListener)
{
	CriticalSectionController csc(&m_csForThreadSafe);
	ListenerSet::iterator it = m_setListener.find(pListener);
	if(it != m_setListener.end())
		m_setListener.erase(m_setListener.find(pListener));	
}

bool EventInstance::update(DWORD dwTickTime)
{
	CriticalSectionController csc(&m_csForThreadSafe);
	if(m_bPaused)
		return true;
	if(!m_bPlaying && !m_bStopping)
		return true;	
	if(!apply3DAttributes())
		return false;
	if(!apply3DCone(true))
		return false;
	updateParameter();
	EventLayerInstanceList::const_iterator it = m_listEventLayerInstance.begin();
	for (int i=0; it!=m_listEventLayerInstance.end(); ++it, ++i)
	{
		if(!isValidLayer(i))
			continue;
		EventLayerInstance* pEventLayerInstance = *it;
		if(!pEventLayerInstance)
			continue;
		if(!pEventLayerInstance->update(dwTickTime))
			continue;
	}
	fadein();
	fadeout();	
	return true;
}

void EventInstance::fadein()
{
	if(!m_bPlaying || m_bStopping)
		return;
	if(!m_pChannelGroup)
		return;
	float fInitVolume;
	if(m_EventProperty.iFadeInTime == 0)
	{
		fInitVolume = 1.0f;
		
	}
	else
	{
		fInitVolume = 0.0f;		
	}
	if(FMOD_OK != m_pChannelGroup->setVolume(m_fVolume * fInitVolume))
		return;
	DWORD dwElapseTime = GetTickCount() - m_dwStartTickCount;
	if(dwElapseTime < (DWORD)m_EventProperty.iFadeInTime)
	{
		Curve curve;
		curve.SetType((CURVE_TYPE)m_EventProperty.iFadeInCurveType);
		float fVolume = curve.GetValue(0, 0, (float)m_EventProperty.iFadeInTime, 1, (float)dwElapseTime);
		if(FMOD_OK != m_pChannelGroup->setVolume(m_fVolume * fVolume))
			return;
		m_fLastFadeInVolume = fVolume;
	}
	else
	{
		if(FMOD_OK != m_pChannelGroup->setVolume(m_fVolume * 1.0f))
			return;
		m_fLastFadeInVolume = 1.0f;
	}
}

void EventInstance::fadeout()
{
	if(!m_bPlaying)
		return;
	if(m_EventProperty.iFadeOutTime == 0 || !m_pChannelGroup || !m_bStopping)
		return;
	DWORD dwElapseTime = GetTickCount() - m_dwStopTickCount;
	if(dwElapseTime > (DWORD)m_EventProperty.iFadeOutTime)
	{
		if(FMOD_OK != m_pChannelGroup->setVolume(m_fVolume * 0.0f))
			return;
		if(!Stop(true))
			return;
	}
	else
	{
		Curve curve;
		curve.SetType((CURVE_TYPE)m_EventProperty.iFadeOutCurveType);
		
		float fVolume = curve.GetValue(0, 1, (float)m_EventProperty.iFadeOutTime, 0, (float)dwElapseTime);
		if(FMOD_OK != m_pChannelGroup->setVolume(m_fVolume * fVolume * m_fLastFadeInVolume))
			return;
	}
}

void EventInstance::resetParameter()
{
	int iParameterNum = GetInstanceParameterNum();
	for (int i=0; i<iParameterNum; ++i)
	{
		EventInstanceParameter* pParameter = GetInstanceParameterByIndex(i);
		if(!pParameter)
			continue;
		pParameter->Reset();
	}
}

void EventInstance::updateParameter()
{
	DWORD dwCurTickCount = GetTickCount();
	DWORD dwElapseTime = dwCurTickCount - m_dwParamLastTickCount;
	m_dwParamLastTickCount = dwCurTickCount;
	int iParameterNum = GetInstanceParameterNum();
	for (int i=0; i<iParameterNum; ++i)
	{
		EventInstanceParameter* pParameter = GetInstanceParameterByIndex(i);
		if(!pParameter)
			continue;
		EVENT_PARAM_PROPERTY prop;
		pParameter->GetProperty(prop);		
		float fValue = pParameter->GetValue();
		if(prop.fSeekSpeed != 0 && pParameter->GetSeekState())
		{
			float fSeekValue = pParameter->GetSeekValue();
			float fNewValue;
			if(pParameter->GetForwardSeekState())
			{
				fNewValue = fValue + prop.fSeekSpeed*dwElapseTime/1000.0f;
				if(fNewValue > fSeekValue)
				{
					fNewValue = fSeekValue;
					pParameter->FinishSeek();
				}
				pParameter->SetValue(fNewValue, false);
			}
			else
			{
				fNewValue = fValue - prop.fSeekSpeed*dwElapseTime/1000.0f;
				if(fNewValue < fSeekValue)
				{
					fNewValue = fSeekValue;
					pParameter->FinishSeek();
				}
				pParameter->SetValue(fNewValue, false);
			}
		}
		else
		{
			if(prop.fVelocity == 0)
				continue;
			float fNewValue = fValue + prop.fVelocity*dwElapseTime/1000.0f;
			if(fNewValue > prop.fMaxValue)
			{
				switch(prop.velocityLoopMode)
				{
				case VELOCITY_ONSHOT:
					fNewValue = prop.fMaxValue;
					break;
				case VELOCITY_ONSHOT_STOP:
					fNewValue = prop.fMaxValue;
					break;
				case VELOCITY_LOOP:
					fNewValue = fNewValue - (int)((fNewValue-prop.fMinValue)/(prop.fMaxValue-prop.fMinValue))*(prop.fMaxValue-prop.fMinValue);
					break;
				}
				pParameter->SetValue(fNewValue, false);
				if(prop.velocityLoopMode == VELOCITY_ONSHOT_STOP)
				{
					if(!Stop())
						return;
				}
			}
			else
			{
				pParameter->SetValue(fNewValue, false);
			}
		}			
	}
}

bool EventInstance::isValidLayer(int idx)
{
	return (m_LayerIndex & (1 << idx)) != 0;
}

bool EventInstance::Start(LAYER_INDEX LayerIndex)
{
	CriticalSectionController csc(&m_csForThreadSafe);
	if(m_bStopping || m_bPlaying)
		if(!Stop(true))
			return false;
	if(GetTickCount() - m_pEvent->GetLastPlayTime() < (DWORD)m_EventProperty.iMinIntervalTime)
		return false;
	m_LayerIndex = LayerIndex;
	if(m_listEventLayerInstance.size() == 0)
		return false;
	
	EventLayerInstanceList::const_iterator it = m_listEventLayerInstance.begin();
	for (int i=0; it!=m_listEventLayerInstance.end(); ++it, ++i)
	{
		if(!isValidLayer(i))
			continue;
		EventLayerInstance* pEventLayerInstance = *it;
		if(!pEventLayerInstance)
			return false;

		{
			CriticalSectionController csces(m_pEvent->GetEventProject()->GetEventSystem()->GetCriticalSectionWrapper());
			if(FMOD_OK != m_pChannelGroup->addGroup(pEventLayerInstance->GetChannelGroup()))
				return false;
		}		
		if(!pEventLayerInstance->play())
			return false;
	}

	if(FMOD_OK != m_pChannelGroup->setVolume(m_fVolume * 0.0f))
		return false;
	
	m_bPlaying = true;
	m_bStopping = false;
	m_bPaused = false;
	Pause(false);
	if(!apply3DAttributes())
		return false;
	if(!apply3DCone(false))
		return false;

	if(!listenerPlay())
		return false;

	AddListener((EventInstanceListener*)m_pEvent);
	m_pEvent->SetLastPlayTime(GetTickCount());
	m_dwStartTickCount = GetTickCount();
	m_dwParamLastTickCount = GetTickCount();

	return true;
}

bool EventInstance::Pause(bool bPause)
{
	if(!m_bPlaying || m_bStopping)
		return true;
	if(m_listEventLayerInstance.size() == 0)
		return true;
	EventLayerInstanceList::const_iterator it = m_listEventLayerInstance.begin();
	for (int i=0; it!=m_listEventLayerInstance.end(); ++it, ++i)
	{
		if(!isValidLayer(i))
			continue;
		EventLayerInstance* pEventLayerInstance = *it;
		if(!pEventLayerInstance)
			return false;
		if(!pEventLayerInstance->pause(bPause))
			return false;
	}
	m_bPaused = bPause;
	return true;
}

bool EventInstance::Stop(bool bForce /*= false*/)
{
	CriticalSectionController csc(&m_csForThreadSafe);
	if(m_listEventLayerInstance.size() == 0)
		return true;
	if(!m_bStopping)
		m_dwStopTickCount = GetTickCount();
	m_bStopping = true;
	if(bForce || m_EventProperty.iFadeOutTime == 0)
	{
		bForce = true;
		EventLayerInstanceList::const_iterator it = m_listEventLayerInstance.begin();
		for (int i=0; it!=m_listEventLayerInstance.end(); ++it, ++i)
		{
			if(!isValidLayer(i))
				continue;
			EventLayerInstance* pEventLayerInstance = *it;
			if(!pEventLayerInstance)
				continue;
			if(!pEventLayerInstance->stop(bForce, true))
				continue;
		}
		if(!listenerStop())
			return false;
		m_bPlaying = false;
		m_bStopping = false;
		m_bPaused = false;
		Pause(false);
		resetParameter();
		resetLastProperty();		
		return true;
	}	
	return true;
}

bool EventInstance::Set3DAttributes(const VECTOR& pos, const VECTOR& vel)
{
	if(m_EventProperty.mode == MODE_2D)
		return false;
	m_vPos = pos;
	m_vVel = vel;
	return true;
}

bool EventInstance::apply3DAttributes()
{
	if(m_EventProperty.mode == MODE_2D)
		return true;
	if(!m_pChannelGroup)
		return false;
	if(m_vLastPos == m_vPos && m_vLastVel == m_vVel)
		return true;
	m_vLastPos = m_vPos;
	m_vLastVel = m_vVel;
	FMOD_VECTOR fmod_pos = {m_vPos.x, m_vPos.y, m_vPos.z};
	FMOD_VECTOR fmod_vel = {m_vVel.x, m_vVel.y, m_vVel.z};
	if(FMOD_OK != m_pChannelGroup->override3DAttributes(&fmod_pos, &fmod_vel))
		return false;	
	return true;
}

bool EventInstance::Set3DConeOrientation(const VECTOR& orientation)
{
	if(m_EventProperty.mode == MODE_2D)
		return false;
	m_vOrientation = orientation;
	m_bEnableOrientation = true;
	return true;
}

bool EventInstance::apply3DCone(bool upate)
{
	if(m_EventProperty.mode == MODE_2D)
		return true;
	if(!m_bEnableOrientation)
		return true;
	if (m_EventProperty.f3DConeInsideAngle == 360.0f && m_EventProperty.f3DConeOutsideAngle == 360.0f && m_EventProperty.f3DConeOutsideVolume == 1.0f)
		return true;
	if(m_vOrientation == m_vLastOrientation)
		return true;
	m_vLastOrientation = m_vOrientation;
	EventLayerInstanceList::const_iterator it = m_listEventLayerInstance.begin();
	for (int i=0; it!=m_listEventLayerInstance.end(); ++it, ++i)
	{
		if(!isValidLayer(i))
			continue;
		EventLayerInstance* pEventLayerInsance = *it;
		if(!pEventLayerInsance)
			return false;
		FMOD::ChannelGroup* pChannelGroup = pEventLayerInsance->GetChannelGroup();
		if(!pChannelGroup)
			return false;
		int iSubGroupNum = 0;
		if(FMOD_OK != pChannelGroup->getNumGroups(&iSubGroupNum))
			return false;
		for (int i=0; i<iSubGroupNum; ++i)
		{
			FMOD::ChannelGroup* pSubGroup = 0;
			if(FMOD_OK != pChannelGroup->getGroup(i, &pSubGroup))
				continue;
			int iChannelNum = 0;
			if(FMOD_OK != pSubGroup->getNumChannels(&iChannelNum))
				return false;
			for (int j=0; j<iChannelNum; ++j)
			{
				FMOD::Channel* pChannel = 0;
				if(FMOD_OK != pSubGroup->getChannel(j, &pChannel))
					continue;
				if(!pChannel)
					return false;
				FMOD_VECTOR fmod_orientation = {m_vOrientation.x, m_vOrientation.y, m_vOrientation.z};
				if(FMOD_OK != pChannel->set3DConeOrientation(&fmod_orientation))
					return false;
				if(!upate)
				{
					float fOriVolume = 1.0f;
					if(FMOD_OK != pChannel->getVolume(&fOriVolume))
						return false;
					if(FMOD_OK != pChannel->set3DConeSettings(m_EventProperty.f3DConeInsideAngle, m_EventProperty.f3DConeOutsideAngle, m_EventProperty.f3DConeOutsideVolume * fOriVolume))
						return false;
				}
			}
		}		
	}
	return true;
}

EventLayerInstance* EventInstance::GetEventLayerInstanceByIndex(int idx) const
{
	EventLayerInstanceList::const_iterator it = m_listEventLayerInstance.begin();
	for (int i=0; it!=m_listEventLayerInstance.end(); ++it, ++i)
	{
		if(i == idx)
			return *it;
	}
	return 0;
}

bool EventInstance::SetVolume(float fVolume)
{
	m_fVolume = fVolume;
	if(IsPlaying())
	{
		if(FMOD_OK != m_pChannelGroup->setVolume(m_fVolume * m_fLastFadeInVolume))
			return false;
	}
	return true;
}

float EventInstance::GetCurVolume() const
{
	if(IsPlaying())
	{
		float fVolume = 1.0f;
		m_pChannelGroup->getVolume(&fVolume);
		return fVolume;
	}
	return 0.0f;
}

bool EventInstance::IsCurInfiniteLoop() const
{
	bool bLoop = false;
	EventLayerInstanceList::const_iterator it = m_listEventLayerInstance.begin();
	for (int i=0; it!=m_listEventLayerInstance.end(); ++it, ++i)
	{
		EventLayerInstance* pLayerInstance = *it;
		if(!pLayerInstance)
			continue;
		SoundInstance* pCurSoundInstance = pLayerInstance->GetCurSoundInstance();
		if(!pCurSoundInstance)
			continue;
		SOUND_INSTANCE_PROPERTY prop;
		pCurSoundInstance->GetSoundInstanceTemplate()->GetProperty(prop);
		if(prop.iLoopCount == 0)
		{
			bLoop = true;
			break;
		}
	}
	return bLoop;
}

void EventInstance::resetLastProperty()
{
	m_vLastVel = VECTOR(0);
	m_vLastPos = VECTOR(0);
	m_vLastOrientation = VECTOR(0, 0, 1);
}

bool EventInstance::GetCurrentChannelPosition(unsigned int& iMillisecond)
{
	CriticalSectionController csc(&m_csForThreadSafe);
	EventLayerInstanceList::const_iterator it = m_listEventLayerInstance.begin();
	for (; it!=m_listEventLayerInstance.end(); ++it)
	{
		EventLayerInstance* pEventLayerInstance = *it;
		if(!pEventLayerInstance)
			continue;
		SoundInstance* pSoundInstance = pEventLayerInstance->GetCurSoundInstance();
		if(!pSoundInstance)
			continue;
		ChannelList listPlayingChannel;
		pSoundInstance->GetPlayingChannelList(listPlayingChannel);
		ChannelList::const_iterator itChannel = listPlayingChannel.begin();
		for (; itChannel!=listPlayingChannel.end(); ++itChannel)
		{
			FMOD::Channel* pChannel = *itChannel;
			bool bIsPlaying = false;
			if(FMOD_OK == pChannel->isPlaying(&bIsPlaying))
			{
				if(!bIsPlaying)
					continue;
				if(FMOD_OK == pChannel->getPosition(&iMillisecond, FMOD_TIMEUNIT_MS))
					return true;
			}
		}
	}
	return false;
}

bool EventInstance::SetCurrentChannelPosition(unsigned int iMillisecond)
{
	CriticalSectionController csc(&m_csForThreadSafe);
	bool bReturn = false;
	EventLayerInstanceList::const_iterator it = m_listEventLayerInstance.begin();
	for (; it!=m_listEventLayerInstance.end(); ++it)
	{
		EventLayerInstance* pEventLayerInstance = *it;
		if(!pEventLayerInstance)
			continue;
		SoundInstance* pSoundInstance = pEventLayerInstance->GetCurSoundInstance();
		if(!pSoundInstance)
			continue;
		ChannelList listPlayingChannel;
		pSoundInstance->GetPlayingChannelList(listPlayingChannel);
		ChannelList::const_iterator itChannel = listPlayingChannel.begin();
		for (; itChannel!=listPlayingChannel.end(); ++itChannel)
		{
			FMOD::Channel* pChannel = *itChannel;
			bool bIsPlaying = false;
			if(FMOD_OK == pChannel->isPlaying(&bIsPlaying))
			{
				if(!bIsPlaying)
					continue;
				if(FMOD_OK == pChannel->setPosition(iMillisecond, FMOD_TIMEUNIT_MS))
				{
					bReturn = true;
				}
			}
		}
	}
	return bReturn;
}