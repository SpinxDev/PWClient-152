#include "FEventGroup.h"
#include "FEvent.h"
#include "FEventSystem.h"
#include "FEventProject.h"
#include "FEventManager.h"
#include "FLogManager.h"
#include "xml\xmlcommon.h"
#include <fmod.hpp>
#include <AFileImage.h>

using namespace AudioEngine;

static const unsigned int version = 0x100002;

EventGroup::EventGroup(void)
: m_pEventProject(0)
, m_pChannelGroup(0)
, m_pParentGroup(0)
, m_iClass(-1)
, m_fGroupVolume(1.0f)
, m_fClassVolume(1.0f)
{
	m_strName.reserve(128);
	m_strNote.reserve(128);
}

EventGroup::~EventGroup(void)
{
	release();
}

bool EventGroup::Init(EventProject* pEventProject)
{
	if(!pEventProject)
		return false;	
	m_pEventProject = pEventProject;
	EventSystem* pEventSystem = m_pEventProject->GetEventSystem();
	if(!pEventSystem)
		return false;
	FMOD::System* pSystem = pEventSystem->GetSystem();
	if(!pSystem)
		return false;
	if(FMOD_OK != pSystem->createChannelGroup("", &m_pChannelGroup))
		return false;
	if(GetParentGroup())
	{
		GetParentGroup()->GetChannelGroup()->addGroup(m_pChannelGroup);
	}
	else
	{
		FMOD::ChannelGroup* pMasterChannelGroup = 0;
		if(FMOD_OK == pSystem->getMasterChannelGroup(&pMasterChannelGroup))
		{
			pMasterChannelGroup->addGroup(m_pChannelGroup);
		}
	}
	return true;
}

void EventGroup::release()
{
	EventGroupList::const_iterator groupIt = m_listEventGroup.begin();
	for (; groupIt!=m_listEventGroup.end(); ++groupIt)
	{
		EventGroup* pEventGroup = *groupIt;
		if(!pEventGroup)
			continue;
		delete pEventGroup;
	}
	m_listEventGroup.clear();

	EventList::const_iterator eventIt = m_listEvent.begin();
	for (; eventIt!=m_listEvent.end(); ++eventIt)
	{
		Event* pEvent = *eventIt;
		if(!pEvent)
			continue;
		delete pEvent;
	}
	m_listEvent.clear();

	if(m_pChannelGroup)
	{
		CriticalSectionController csc(m_pEventProject->GetEventSystem()->GetCriticalSectionWrapper());
		m_pChannelGroup->stop();
		m_pChannelGroup->release();
	}
	m_pChannelGroup = 0;
}

Event* EventGroup::GetEventByName(const char* szName) const
{
	EventList::const_iterator it = m_listEvent.begin();
	for (; it!=m_listEvent.end(); ++it)
	{
		Event* pEvent = *it;
		if(!pEvent)
			continue;
		if(0 == strcmp(pEvent->GetName(), szName))
			return pEvent;
	}
	return 0;
}

Event* EventGroup::GetEventByIndex(int idx) const
{	
	EventList::const_iterator it = m_listEvent.begin();
	for (int i=0; it!=m_listEvent.end(); ++it, ++i)
	{
		if(i != idx)
			continue;
		return *it;
	}
	return 0;
}

bool EventGroup::LoadEventData()
{
	EventGroupList::const_iterator itG = m_listEventGroup.begin();
	for (; itG!=m_listEventGroup.end(); ++itG)
	{
		EventGroup* pEventGroup = *itG;
		if(!pEventGroup)
			return false;
		if(!pEventGroup->LoadEventData())
			return false;
	}

	EventList::const_iterator itE = m_listEvent.begin();
	for (; itE!=m_listEvent.end(); ++itE)
	{
		Event* pEvent = *itE;
		if(!pEvent)
			return false;
		if(!pEvent->loadData())
			return false;
	}

	return true;
}

EventGroup* EventGroup::GetEventGroupByName(const char* szName) const
{
	EventGroupList::const_iterator it = m_listEventGroup.begin();
	for (; it!=m_listEventGroup.end(); ++it)
	{
		EventGroup* pEventGroup = *it;
		if(!pEventGroup)
			continue;
		if(0 == strcmp(pEventGroup->GetName(), szName))
			return pEventGroup;
	}
	return 0;
}

EventGroup* EventGroup::GetEventGroupByIndex(int idx) const
{
	EventGroupList::const_iterator it = m_listEventGroup.begin();
	for (int i=0; it!=m_listEventGroup.end(); ++it, ++i)
	{
		if(i == idx)
			return *it;
	}
	return 0;
}

bool EventGroup::SetClassVolume(float volume)
{
	m_fClassVolume = volume;
	if(FMOD_OK != m_pChannelGroup->setVolume(m_fClassVolume * m_fGroupVolume))
		return false;
	return true;
}

bool EventGroup::SetClassMute(bool bMute)
{
	if(FMOD_OK != m_pChannelGroup->setMute(bMute))
		return false;
	return true;
}

bool EventGroup::SetGroupVolume(float volume)
{
	m_fGroupVolume = volume;
	if(FMOD_OK != m_pChannelGroup->setVolume(m_fClassVolume * m_fGroupVolume))
		return false;
	return true;
}

Event* EventGroup::createEvent(const char* szName)
{
	Event* pEvent = new Event;
	if(!pEvent->Init(m_pEventProject))
	{
		delete pEvent;
		return 0;
	}
	pEvent->SetName(szName);	
	pEvent->m_Guid = GuidGen::GetInstance().Generate();
	pEvent->SetParentGroup(this);
	m_pEventProject->GetEventManager()->AddToMap(pEvent);
	m_listEvent.push_back(pEvent);
	return pEvent;
}

EventGroup* EventGroup::createEventGroup(const char* szName)
{
	EventGroup* pEventGroup = new EventGroup;
	pEventGroup->SetParentGroup(this);
	if(!pEventGroup->Init(m_pEventProject))
	{
		delete pEventGroup;
		return 0;
	}
	pEventGroup->SetName(szName);	
	m_listEventGroup.push_back(pEventGroup);
	return pEventGroup;
}

bool EventGroup::deleteEvent(const char* szName)
{
	EventList::iterator it = m_listEvent.begin();
	for (; it!=m_listEvent.end(); ++it)
	{
		Event* pEvent = *it;
		if(!pEvent)
			return false;
		if(GF_CompareNoCase(szName, strlen(szName), pEvent->GetName(), strlen(pEvent->GetName())))
		{
			delete pEvent;
			m_listEvent.erase(it);
			return true;
		}
	}
	return false;
}

bool EventGroup::deleteEventGroup(const char* szName)
{
	EventGroupList::iterator it = m_listEventGroup.begin();
	for (; it!=m_listEventGroup.end(); ++it)
	{
		EventGroup* pEventGroup = *it;
		if(!pEventGroup)
			return false;
		if(GF_CompareNoCase(szName, strlen(szName), pEventGroup->GetName(), strlen(pEventGroup->GetName())))
		{
			while(pEventGroup->GetEventNum())
			{
				Event* pEvent = pEventGroup->GetEventByIndex(0);
				if(!pEvent)
					return false;
				if(!m_pEventProject->GetEventManager()->DeleteEvent(pEvent))
					return false;
			}

			while(pEventGroup->GetEventGroupNum())
			{
				EventGroup* pChildEventGroup = pEventGroup->GetEventGroupByIndex(0);
				if(!pChildEventGroup)
					return false;
				if(!pEventGroup->deleteEventGroup(pChildEventGroup->GetName()))
					return false;
			}

			delete pEventGroup;
			m_listEventGroup.erase(it);
			return true;
		}
	}
	return false;
}

bool EventGroup::removeEvent(Event* pEvent)
{
	if(!pEvent)
		return false;
	EventList::iterator it = m_listEvent.begin();
	for (; it!=m_listEvent.end(); ++it)
	{		
		if(pEvent == *it)
		{
			m_listEvent.erase(it);
			return true;
		}
	}
	return false;
}

bool EventGroup::addEvent(Event* pEvent)
{
	if(!pEvent)
		return false;
	EventList::const_iterator it = m_listEvent.begin();
	for (; it!=m_listEvent.end(); ++it)
	{		
		if(pEvent == *it)
			return false;
	}
	if(m_pEventProject->GetEventManager()->IsEventExist(this, pEvent->GetName()))
		return false;
	pEvent->SetParentGroup(this);
	m_listEvent.push_back(pEvent);
	return true;
}

bool EventGroup::removeEventGroup(EventGroup* pEventGroup)
{
	if(!pEventGroup)
		return false;
	EventGroupList::iterator it = m_listEventGroup.begin();
	for (; it!=m_listEventGroup.end(); ++it)
	{
		if(pEventGroup == *it)
		{
			m_listEventGroup.erase(it);
			return true;
		}
	}
	return false;
}

bool EventGroup::addEventGroup(EventGroup* pEventGroup)
{
	if(!pEventGroup)
		return false;
	EventGroupList::const_iterator it = m_listEventGroup.begin();
	for (; it!=m_listEventGroup.end(); ++it)
	{
		if(pEventGroup == *it)
			return false;
	}
	if(m_pEventProject->GetEventManager()->IsEventGroupExist(this, pEventGroup->GetName()))
		return false;
	pEventGroup->SetParentGroup(this);
	m_listEventGroup.push_back(pEventGroup);
	return true;
}

bool EventGroup::Load(AFileImage* pFile)
{
	release();
	if(!Init(m_pEventProject))
		return false;

	DWORD dwReadLen = 0;

	unsigned int ver = 0;
	pFile->Read(&ver, sizeof(ver), &dwReadLen);

	int iNameLen = 0;
	pFile->Read(&iNameLen, sizeof(iNameLen), &dwReadLen);
	char szName[1024] = {0};
	pFile->Read(szName, iNameLen, &dwReadLen);
	m_strName = szName;

	pFile->Read(&m_iClass, sizeof(m_iClass), &dwReadLen);

	int iNoteLen = 0;
	pFile->Read(&iNoteLen, sizeof(iNoteLen), &dwReadLen);
	char szNote[1024] = {0};
	pFile->Read(szNote, iNoteLen, &dwReadLen);
	m_strNote = szNote;

	pFile->Read(&m_fGroupVolume, sizeof(m_fGroupVolume), &dwReadLen);
	SetGroupVolume(m_fGroupVolume);

	int iEventGroupNum = 0;
	pFile->Read(&iEventGroupNum, sizeof(iEventGroupNum), &dwReadLen);
	for (int i=0; i<iEventGroupNum; ++i)
	{
		EventGroup* pEventGroup = new EventGroup;
		if(!pEventGroup->Init(m_pEventProject))
		{
			delete pEventGroup;
			return false;
		}
		pEventGroup->SetParentGroup(this);
		if(!pEventGroup->Load(pFile))
		{
			m_pEventProject->GetEventSystem()->GetLogManager()->GetEngineLog()->Write(LOG_ERROR, "EventGroup::Load ¼ÓÔØEventGroup:%sÊ§°Ü", pEventGroup->GetName());
			return false;
		}
		m_listEventGroup.push_back(pEventGroup);
	}

	int iEventNum = 0;
	pFile->Read(&iEventNum, sizeof(iEventNum), &dwReadLen);
	for (int i=0; i<iEventNum; ++i)
	{
		Event* pEvent = new Event;
		if(!pEvent->Init(m_pEventProject))
		{
			delete pEvent;
			return false;
		}
		pEvent->SetParentGroup(this);
		if(!pEvent->Load(pFile))
		{
			m_pEventProject->GetEventSystem()->GetLogManager()->GetEngineLog()->Write(LOG_ERROR, "EventGroup::Load ¼ÓÔØEvent:%sÊ§°Ü", pEvent->GetName());
			return false;
		}
		m_pEventProject->GetEventManager()->AddToMap(pEvent);
		m_listEvent.push_back(pEvent);
	}

	return true;
}

bool EventGroup::Save(AFile* pFile)
{
	DWORD dwWriteLen = 0;
	pFile->Write((void*)&version, sizeof(version), &dwWriteLen);

	int iNameLen = (int)m_strName.size();
	pFile->Write(&iNameLen, sizeof(iNameLen), &dwWriteLen);
	pFile->Write((void*)m_strName.c_str(), iNameLen, &dwWriteLen);

	pFile->Write(&m_iClass, sizeof(m_iClass), &dwWriteLen);

	int iNoteLen = (int)m_strNote.size();
	pFile->Write(&iNoteLen, sizeof(iNoteLen), &dwWriteLen);
	pFile->Write((void*)m_strNote.c_str(), iNoteLen, &dwWriteLen);
	
	pFile->Write(&m_fGroupVolume, sizeof(m_fGroupVolume), &dwWriteLen);

	int iSubGroupNum = GetEventGroupNum();
	pFile->Write(&iSubGroupNum, sizeof(iSubGroupNum), &dwWriteLen);
	EventGroupList::const_iterator itGroup = m_listEventGroup.begin();
	for (; itGroup!=m_listEventGroup.end(); ++itGroup)
	{
		EventGroup* pEventGroup = *itGroup;
		if(!pEventGroup)
			return false;
		if(!pEventGroup->Save(pFile))
		{
			m_pEventProject->GetEventSystem()->GetLogManager()->GetEngineLog()->Write(LOG_ERROR, "EventGroup::Save ±£´æEventGroup:%sÊ§°Ü", pEventGroup->GetName());
			return false;
		}
	}

	int iEventNum = GetEventNum();
	pFile->Write(&iEventNum, sizeof(iEventNum), &dwWriteLen);
	EventList::const_iterator itEvent = m_listEvent.begin();
	for (; itEvent!=m_listEvent.end(); ++itEvent)
	{
		Event* pEvent = *itEvent;
		if(!pEvent)
			return false;
		if(!pEvent->Save(pFile))
		{
			m_pEventProject->GetEventSystem()->GetLogManager()->GetEngineLog()->Write(LOG_ERROR, "EventGroup::Save ±£´æEvent:%sÊ§°Ü", pEvent->GetName());
			return false;
		}
	}

	return true;
}

bool EventGroup::LoadXML(TiXmlElement* root)
{
	release();
	if(!Init(m_pEventProject))
		return false;

	unsigned int ver = 0;
	QueryElement(root, "version", ver);
	QueryElement(root, "name", m_strName);
	QueryElement(root, "class", m_iClass);
	QueryElement(root, "note", m_strNote);

	if(ver > 0x100001)
	{
		QueryElement(root, "EventGroupVolume", m_fGroupVolume);
		SetGroupVolume(m_fGroupVolume);
	}
	
	int iEventGroupNum = 0;
	QueryElement(root, "EventGroupNum", iEventGroupNum);
	TiXmlNode* pEventGroupNode = root->FirstChild("EventGroup");
	for (int i=0; i<iEventGroupNum; ++i)
	{
		EventGroup* pEventGroup = new EventGroup;
		if(!pEventGroup->Init(m_pEventProject))
		{
			delete pEventGroup;
			return false;
		}
		pEventGroup->SetParentGroup(this);
		if(!pEventGroup->LoadXML(pEventGroupNode->ToElement()))
		{
			m_pEventProject->GetEventSystem()->GetLogManager()->GetEngineLog()->Write(LOG_ERROR, "EventGroup::LoadXML ¼ÓÔØEventGroup:%sÊ§°Ü", pEventGroup->GetName());
			return false;
		}
		m_listEventGroup.push_back(pEventGroup);
		pEventGroupNode = pEventGroupNode->NextSibling("EventGroup");
	}

	int iEventNum = 0;
	QueryElement(root, "EventNum", iEventNum);
	TiXmlNode* pEventNode = root->FirstChild("Event");
	for (int i=0; i<iEventNum; ++i)
	{
		Event* pEvent = new Event;
		if(!pEvent->Init(m_pEventProject))
		{
			delete pEvent;
			return false;
		}
		pEvent->SetParentGroup(this);
		if(!pEvent->LoadXML(pEventNode->ToElement()))
		{
			m_pEventProject->GetEventSystem()->GetLogManager()->GetEngineLog()->Write(LOG_ERROR, "EventGroup::LoadXML ¼ÓÔØEvent:%sÊ§°Ü", pEvent->GetName());
			return false;
		}
		m_pEventProject->GetEventManager()->AddToMap(pEvent);
		m_listEvent.push_back(pEvent);
		pEventNode = pEventNode->NextSibling("Event");
	}
	return true;
}

bool EventGroup::SaveXML(TiXmlElement* pParent)
{
	TiXmlElement* root = new TiXmlElement("EventGroup");
	pParent->LinkEndChild(root);

	AddElement(root, "version", version);
	AddElement(root, "name", m_strName);
	if(GetParentGroup())
		m_iClass = -1;
	AddElement(root, "class", m_iClass);
	AddElement(root, "note", m_strNote);

	AddElement(root, "EventGroupVolume", m_fGroupVolume);
	AddElement(root, "EventGroupNum", GetEventGroupNum());
	EventGroupList::const_iterator itGroup = m_listEventGroup.begin();
	for (; itGroup!=m_listEventGroup.end(); ++itGroup)
	{
		EventGroup* pEventGroup = *itGroup;
		if(!pEventGroup)
			return false;
		if(!pEventGroup->SaveXML(root))
		{
			m_pEventProject->GetEventSystem()->GetLogManager()->GetEngineLog()->Write(LOG_ERROR, "EventGroup::SaveXML ±£´æEventGroup:%sÊ§°Ü", pEventGroup->GetName());
			return false;
		}
	}

	AddElement(root, "EventNum", GetEventNum());
	EventList::const_iterator itEvent = m_listEvent.begin();
	for (; itEvent!=m_listEvent.end(); ++itEvent)
	{
		Event* pEvent = *itEvent;
		if(!pEvent)
			return false;
		if(!pEvent->SaveXML(root))
		{
			m_pEventProject->GetEventSystem()->GetLogManager()->GetEngineLog()->Write(LOG_ERROR, "EventGroup::SaveXML ±£´æEvent:%sÊ§°Ü", pEvent->GetName());
			return false;
		}
	}
	return true;
}