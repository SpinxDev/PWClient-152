#pragma once

#include "FAudioEngine.h"
#include "FExportDecl.h"
#include "FGuid.h"

#include <map>


class TiXmlElement;
class AFileImage;
class AFile;

namespace AudioEngine
{
	typedef std::map<Guid, Event*> EventMap;

	class _EXPORT_DLL_ EventManager
	{
	public:
		EventManager(void);
		~EventManager(void);
	public:
		bool					Init(EventProject* pEventProject);
		EventGroup*				CreateEventGroup(const char* szName);
		EventGroup*				CreateEventGroup(EventGroup* pEventGroup, const char* szName);
		Event*					CreateEvent(EventGroup* pEventGroup, const char* szName);
		bool					DeleteEventGroup(EventGroup* pEventGroup);
		bool					DeleteEvent(Event* pEvent);
		Event*					GetEvent(const Guid& guid);
		int						GetEventGroupNum() const { return (int)m_listEventGroup.size(); }
		EventGroup*				GetEventGroupByIndex(int idx) const;
		EventGroup*				GetEventGroupByName(const char* szName) const;
		bool					Load(AFileImage* pFile);
		bool					Save(AFile* pFile);
		bool					LoadXML(TiXmlElement* root);
		bool					SaveXML(TiXmlElement* pParent);
		bool					AddToMap(Event* pEvent);
		bool					IsEventGroupExist(const char* szName);
		bool					IsEventGroupExist(EventGroup* pEventGroup, const char* szName);
		bool					IsEventExist(EventGroup* pEventGroup, const char* szName);
		bool					Update(DWORD dwTickTime);
		bool					MoveEvent(Event* pEvent, EventGroup* pDestEventGroup);
		bool					MoveEventGroup(EventGroup* pEventGroup, EventGroup* pDestEventGroup);
		bool					MoveEventGroup(EventGroup* pEventGroup);
		const EventMap&			GetEventMap() const { return m_mapEvent; }
	protected:
		void					release();
		bool					deleteEventGroup(const char* szName);
		bool					removeEventGroup(EventGroup* pEventGroup);
		bool					addEventGroup(EventGroup* pEventGroup);
	protected:
		EventProject*			m_pEventProject;
		EventGroupList			m_listEventGroup;
		EventMap				m_mapEvent;		
	};
}
