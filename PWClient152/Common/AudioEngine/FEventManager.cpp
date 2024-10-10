//#include "AGPAPerfIntegration.h"
#include "FEventManager.h"
#include "FEventGroup.h"
#include "FEvent.h"
#include "FEventProject.h"
#include "FLogManager.h"
#include "xml\xmlcommon.h"
#include "FEventSystem.h"
#include <AFileImage.h>

using namespace AudioEngine;

static const unsigned int version = 0x100001;

EventManager::EventManager(void)
: m_pEventProject(0)
{
}

EventManager::~EventManager(void)
{
	release();
}

bool EventManager::Init(EventProject* pEventProject)
{
	if(!pEventProject)
		return false;
	m_pEventProject = pEventProject;
	return true;
}

void EventManager::release()
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

	m_mapEvent.clear();
}

EventGroup* EventManager::CreateEventGroup(const char* szName)
{
	EventGroup* pEventGroup = new EventGroup;
	if(!pEventGroup->Init(m_pEventProject))
	{
		delete pEventGroup;
		return 0;
	}
	pEventGroup->SetName(szName);
	m_listEventGroup.push_back(pEventGroup);
	return pEventGroup;
}

EventGroup* EventManager::CreateEventGroup(EventGroup* pEventGroup, const char* szName)
{
	if(!pEventGroup)
		return 0;
	if(!szName || strlen(szName) == 0)
		return 0;
	return pEventGroup->createEventGroup(szName);
}

Event* EventManager::CreateEvent(EventGroup* pEventGroup, const char* szName)
{
	if(!pEventGroup)
		return 0;
	if(!szName || strlen(szName) == 0)
		return 0;
	return pEventGroup->createEvent(szName);
}

bool EventManager::DeleteEventGroup(EventGroup* pEventGroup)
{
	if(pEventGroup->GetParentGroup())
	{
		return pEventGroup->GetParentGroup()->deleteEventGroup(pEventGroup->GetName());
	}
	else
	{
		return deleteEventGroup(pEventGroup->GetName());			
	}
	return false;
}

bool EventManager::DeleteEvent(Event* pEvent)
{
	Guid guid = pEvent->GetGuid();
	if(!pEvent->GetParentGroup()->deleteEvent(pEvent->GetName()))
		return false;
	m_mapEvent.erase(m_mapEvent.find(guid));
	return true;
}

Event* EventManager::GetEvent(const Guid& guid)
{
	EventMap::const_iterator it = m_mapEvent.find(guid);
	if(it != m_mapEvent.end())
		return it->second;
	return 0;
}

EventGroup* EventManager::GetEventGroupByIndex(int idx) const
{
	if(idx < 0 || idx >= GetEventGroupNum())
		return 0;
	EventGroupList::const_iterator it = m_listEventGroup.begin();
	for (int i=0; it!=m_listEventGroup.end(); ++it, ++i)
	{
		if(i == idx)
			return *it;
	}
	return 0;
}

EventGroup* EventManager::GetEventGroupByName(const char* szName) const
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

bool EventManager::deleteEventGroup(const char* szName)
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

bool EventManager::AddToMap(Event* pEvent)
{
	if(!pEvent)
		return false;
	m_mapEvent[pEvent->GetGuid()] = pEvent;
	return true;
}

bool EventManager::IsEventGroupExist(const char* szName)
{
	EventGroupList::const_iterator it = m_listEventGroup.begin();
	for (; it!=m_listEventGroup.end(); ++it)
	{
		EventGroup* pEventGroup = *it;
		if(GF_CompareNoCase(pEventGroup->GetName(), strlen(pEventGroup->GetName()), szName, strlen(szName)))
			return true;
	}
	return false;
}

bool EventManager::IsEventGroupExist(EventGroup* pEventGroup, const char* szName)
{
	int iEventGroupNum = pEventGroup->GetEventGroupNum();
	for (int i=0; i<iEventGroupNum; ++i)
	{
		EventGroup* pChildEventGroup = pEventGroup->GetEventGroupByIndex(i);
		if(!pChildEventGroup)
			continue;
		if(GF_CompareNoCase(pChildEventGroup->GetName(), strlen(pChildEventGroup->GetName()), szName, strlen(szName)))
			return true;
	}
	return false;
}

bool EventManager::IsEventExist(EventGroup* pEventGroup, const char* szName)
{
	int iEventNum = pEventGroup->GetEventNum();
	for (int i=0; i<iEventNum; ++i)
	{
		Event* pEvent = pEventGroup->GetEventByIndex(i);
		if(!pEvent)
			continue;
		if(GF_CompareNoCase(pEvent->GetName(), strlen(pEvent->GetName()), szName, strlen(szName)))
			return true;
	}
	return false;
}

bool EventManager::Update(DWORD dwTickTime)
{
//	A3D_ITTX_SCOPED_TASK_NAMED(AGPAPerfIntegration::GetInstance().GetA3DDomain(), TEXT(__FUNCTION__));
	EventMap::const_iterator it = m_mapEvent.begin();
	for (; it!=m_mapEvent.end(); ++it)
	{
		Event* pEvent = it->second;
		if(!pEvent)
			return false;
		if(!pEvent->update(dwTickTime))
			return false;
	}
	return true;
}

bool EventManager::MoveEvent(Event* pEvent, EventGroup* pDestEventGroup)
{
	if(!pEvent || !pDestEventGroup)
		return false;
	EventGroup* pOldParentGroup = pEvent->GetParentGroup();
	if(pOldParentGroup == pDestEventGroup)
		return true;
	if(!pDestEventGroup->addEvent(pEvent))
		return false;
	if(!pOldParentGroup->removeEvent(pEvent))
	{
		if(!pDestEventGroup->removeEvent(pEvent))
			return false;
		return false;
	}
	return true;
}

bool EventManager::MoveEventGroup(EventGroup* pEventGroup, EventGroup* pDestEventGroup)
{
	if(!pEventGroup || !pDestEventGroup)
		return false;
	EventGroup* pOldParentGroup = pEventGroup->GetParentGroup();
	EventGroup* pParentGroup = pDestEventGroup;
	while (pParentGroup)
	{
		if(pParentGroup == pEventGroup)
			return false;
		pParentGroup = pParentGroup->GetParentGroup();
	}
	if(pOldParentGroup == pDestEventGroup)
		return true;
	if(!pDestEventGroup->addEventGroup(pEventGroup))
		return false;
	if(pOldParentGroup)
	{
		if(!pOldParentGroup->removeEventGroup(pEventGroup))
		{
			if(!pDestEventGroup->removeEventGroup(pEventGroup))
				return false;
			return false;
		}		
	}
	else
	{
		if(!removeEventGroup(pEventGroup))
		{
			if(!pDestEventGroup->removeEventGroup(pEventGroup))
				return false;
			return false;
		}
	}
	return true;
}

bool EventManager::MoveEventGroup(EventGroup* pEventGroup)
{
	if(!pEventGroup)
		return false;
	EventGroup* pOldParentGroup = pEventGroup->GetParentGroup();
	if(!pOldParentGroup)
		return true;
	if(!addEventGroup(pEventGroup))
		return false;
	if((!pOldParentGroup->removeEventGroup(pEventGroup)))
	{
		if(!removeEventGroup(pEventGroup))
			return false;
		return false;
	}
	return true;
}

bool EventManager::removeEventGroup(EventGroup* pEventGroup)
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

bool EventManager::addEventGroup(EventGroup* pEventGroup)
{
	if(!pEventGroup)
		return false;
	EventGroupList::const_iterator it = m_listEventGroup.begin();
	for (; it!=m_listEventGroup.end(); ++it)
	{
		if(pEventGroup == *it)
			return false;
	}
	if(IsEventGroupExist(pEventGroup->GetName()))
		return false;
	pEventGroup->SetParentGroup(0);
	m_listEventGroup.push_back(pEventGroup);
	return true;
}

bool EventManager::Load(AFileImage* pFile)
{
	release();
	if(!Init(m_pEventProject))
		return false;

	DWORD dwReadLen = 0;
	unsigned int ver = 0;
	pFile->Read(&ver, sizeof(ver), &dwReadLen);

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
		if(!pEventGroup->Load(pFile))
		{
			m_pEventProject->GetEventSystem()->GetLogManager()->GetEngineLog()->Write(LOG_ERROR, "EventManager::Load ¼ÓÔØEventGroup:%sÊ§°Ü", pEventGroup->GetName());
			return false;
		}
		m_listEventGroup.push_back(pEventGroup);
	}
	return true;
}

bool EventManager::Save(AFile* pFile)
{
	DWORD dwWriteLen = 0;

	pFile->Write((void*)&version, sizeof(version), &dwWriteLen);

	int iEventGroupNum = GetEventGroupNum();
	pFile->Write(&iEventGroupNum, sizeof(iEventGroupNum), &dwWriteLen);
	EventGroupList::const_iterator itGroup = m_listEventGroup.begin();
	for (; itGroup!=m_listEventGroup.end(); ++itGroup)
	{
		EventGroup* pEventGroup = *itGroup;
		if(!pEventGroup)
			return false;
		if(!pEventGroup->Save(pFile))
		{
			m_pEventProject->GetEventSystem()->GetLogManager()->GetEngineLog()->Write(LOG_ERROR, "EventManager::Save ±£´æEventGroup:%sÊ§°Ü", pEventGroup->GetName());
			return false;
		}
	}
	return true;
}

bool EventManager::LoadXML(TiXmlElement* root)
{
	release();
	if(!Init(m_pEventProject))
		return false;

	unsigned int ver = 0;
	QueryElement(root, "version", ver);
	int iEventGroupNum = 0;
	QueryElement(root, "eventGroupNum", iEventGroupNum);
	TiXmlNode* pEventGroupNode = root->FirstChild("EventGroup");
	for (int i=0; i<iEventGroupNum; ++i)
	{
		EventGroup* pEventGroup = new EventGroup;
		if(!pEventGroup->Init(m_pEventProject))
		{
			delete pEventGroup;
			return false;
		}
		if(!pEventGroup->LoadXML(pEventGroupNode->ToElement()))
		{
			m_pEventProject->GetEventSystem()->GetLogManager()->GetEngineLog()->Write(LOG_ERROR, "EventManager::LoadXML ¼ÓÔØEventGroup:%sÊ§°Ü", pEventGroup->GetName());
			return false;
		}
		m_listEventGroup.push_back(pEventGroup);
		pEventGroupNode = pEventGroupNode->NextSibling("EventGroup");
	}
	return true;
}

bool EventManager::SaveXML(TiXmlElement* pParent)
{
	TiXmlElement* root = new TiXmlElement("EventManager");
	pParent->LinkEndChild(root);

	AddElement(root, "version", version);
	AddElement(root, "eventGroupNum", GetEventGroupNum());
	EventGroupList::const_iterator itGroup = m_listEventGroup.begin();
	for (; itGroup!=m_listEventGroup.end(); ++itGroup)
	{
		EventGroup* pEventGroup = *itGroup;
		if(!pEventGroup)
			return false;
		if(!pEventGroup->SaveXML(root))
		{
			m_pEventProject->GetEventSystem()->GetLogManager()->GetEngineLog()->Write(LOG_ERROR, "EventManager::SaveXML ±£´æEventGroup:%sÊ§°Ü", pEventGroup->GetName());
			return false;
		}
	}
	return true;
}