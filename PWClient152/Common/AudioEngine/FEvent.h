#pragma once

#include <string>

#include "FAudioEngine.h"
#include "FGuid.h"
#include "FExportDecl.h"
#include "FEventLayer.h"
#include "FEventInstance.h"
#include "FCriticalSectionWrapper.h"

class TiXmlElement;
class AFileImage;
class AFile;

namespace AudioEngine
{
	class EventParameter;	

	class _EXPORT_DLL_ Event : public EventInstance::EventInstanceListener
	{
		friend class EventGroup;
		friend class EventManager;	
	public:
		Event(void);
		~Event(void);
	public:
		bool					Init(EventProject* pEventProject);
		EventInstance*			CreateInstance(bool bAutoDestroy = true);
		bool					DestroyInstance(EventInstance*& pEventInstance);
		void					GetProperty(EVENT_PROPERTY& prop) const { prop = m_EventProperty; }
		void					SetProperty(const EVENT_PROPERTY& prop) { m_EventProperty = prop; }
		void					SetName(const char* szName) { m_strName = szName; }
		const char*				GetName() const { return m_strName.c_str(); }
		const char*				GetNote() const { return m_strNote.c_str(); }
		void					SetNote(const char* szNote) { m_strNote = szNote; }
		const char*				GetFullPath();
		const char*				GetGuidString();
		EventLayer*				CreateLayer(const char* szName);
		bool					DeleteLayer(EventLayer* pEventLayer);
		EventLayer*				GetLayerByIndex(int idx) const;
		EventLayer*				GetLayerByName(const char* szName) const;
		int						GetLayerNum() const { return (int)m_listEventLayer.size(); }

		int						GetInstanceNum() const { return (int)m_listEventInstance.size(); }
		EventInstance*			GetEventInstanceByIndex(int idx) const;
		
		EventParameter*			GetParameterByIndex(int idx) const;
		EventParameter*			GetParameterByName(const char* szName) const;
		EventParameter*			AddParameter(const char* szName);
		bool					DeleteParameter(EventParameter* pEventParameter);
		bool					IsParameterExist(const char* szName);
		bool					IsParameterUsed(const char* szName);
		int						GetParameterNum() const { return (int)m_listEventParameter.size(); }				
		bool					Load(AFileImage* pFile);
		bool					Save(AFile* pFile);
		bool					LoadXML(TiXmlElement* root, bool bPreset = false);
		bool					SaveXML(TiXmlElement* pParent, bool bPreset = false);
		const Guid&				GetGuid() const { return m_Guid; }
		EventGroup*				GetParentGroup() const { return m_pParentGroup; }
		void					SetParentGroup(EventGroup* pParentGroup) { m_pParentGroup = pParentGroup; }
		virtual bool			OnStop(EventInstance* pEventInstance);
		void					SetLastPlayTime(DWORD dwLastPlayTime) { m_dwLastPlayTime = dwLastPlayTime; }
		DWORD					GetLastPlayTime() const { return m_dwLastPlayTime; }
		EventProject*			GetEventProject() const;
		typedef std::set<std::string> PathSet;
		PathSet					GetRelatedAudioFiles() const;

		bool					Reload();//一般不需要调用（目前只在编辑器切换2D、3D，声音定义里的文件变化时使用）	
		CriticalSectionWrapper*	GetCriticalSectionWrapper() { return &m_csForThreadSafe; }
	protected:		
		bool					loadData(bool bReload = false);
		bool					update(DWORD dwTickTime);
		void					release();		
	protected:
		EventLayerList			m_listEventLayer;
		std::string				m_strName;
		std::string				m_strNote;
		EVENT_PROPERTY			m_EventProperty;
		EventGroup*				m_pParentGroup;
		EventParameterList		m_listEventParameter;
		EventProject*			m_pEventProject;
		Guid					m_Guid;
		EventInstanceList		m_listEventInstance;
		EventInstanceList		m_listDeleteEventInstance;
		DWORD					m_dwLastPlayTime;
		std::string				m_strFullPath;
		CriticalSectionWrapper	m_csForThreadSafe;
		bool					m_bHadCreateInstantces;
	};
}