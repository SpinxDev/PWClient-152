#pragma once

#include "FAudioEngine.h"
#include "FExportDecl.h"

#include <string>


class TiXmlElement;
class AFileImage;
class AFile;

namespace FMOD
{
	class ChannelGroup;
	class DSP;
}

namespace AudioEngine
{
	struct COMPRESSOR_DATA
	{
		float fThreshold;
		float fAttack;
		float fRelease;
		float fMakeUpGain;
	};

	class _EXPORT_DLL_ EventGroup
	{
		friend class EventManager;
	public:
		EventGroup(void);
		~EventGroup(void);
	public:
		bool					Init(EventProject* pEventProject);
		Event*					GetEventByName(const char* szName) const;
		Event*					GetEventByIndex(int idx) const;
		int						GetEventNum() const { return (int)m_listEvent.size(); }
		EventGroup*				GetEventGroupByName(const char* szName) const;
		EventGroup*				GetEventGroupByIndex(int idx) const;
		int						GetEventGroupNum() const { return (int)m_listEventGroup.size(); }
		bool					LoadEventData();
		void					SetName(const char* szName) { m_strName = szName; }
		const char*				GetName() const { return m_strName.c_str(); }
		const char*				GetNote() const { return m_strNote.c_str(); }
		void					SetNote(const char* szNote) { m_strNote = szNote; }
		bool					SetClassVolume(float volume);
		bool					SetClassMute(bool bMute);
		bool					SetGroupVolume(float volume);
		float					GetGroupVolume() const { return m_fGroupVolume; }
		FMOD::ChannelGroup*		GetChannelGroup() const { return m_pChannelGroup; }
		EventGroup*				GetParentGroup() const { return m_pParentGroup; }
		void					SetParentGroup(EventGroup* pParentGroup) { m_pParentGroup = pParentGroup; }
		bool					Load(AFileImage* pFile);
		bool					Save(AFile* pFile);
		bool					LoadXML(TiXmlElement* root);
		bool					SaveXML(TiXmlElement* pParent);
		void					SetClass(int iClass) { m_iClass = iClass; }
		int						GetClass() const { return m_iClass; }
	protected:
		void					release();
		Event*					createEvent(const char* szName);
		EventGroup*				createEventGroup(const char* szName);
		bool					deleteEvent(const char* szName);
		bool					deleteEventGroup(const char* szName);
		bool					removeEvent(Event* pEvent);
		bool					addEvent(Event* pEvent);
		bool					removeEventGroup(EventGroup* pEventGroup);
		bool					addEventGroup(EventGroup* pEventGroup);
	protected:
		EventGroup*				m_pParentGroup;
		EventGroupList			m_listEventGroup;
		EventList				m_listEvent;
		EventProject*			m_pEventProject;
		std::string				m_strName;
		std::string				m_strNote;
		FMOD::ChannelGroup*		m_pChannelGroup;
		int						m_iClass;
		float					m_fGroupVolume;
		float					m_fClassVolume;
	};
}
