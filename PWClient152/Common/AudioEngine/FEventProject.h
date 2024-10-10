#pragma once

#include "FAudioEngine.h"
#include "FGuid.h"
#include "FExportDecl.h"

#include <string>

namespace AudioEngine
{
	class EventSystem;
	class SoundDefManager;
	class AudioManager;
	class EventManager;

	class _EXPORT_DLL_ EventProject
	{
		friend class EventSystem;
	public:
		EventProject(void);
		~EventProject(void);
	public:
		bool					Init(EventSystem* pEventSystem);
		void					SetName(const char* szName) { m_strName = szName; }
		const char*				GetName() const { return m_strName.c_str(); }
		EventManager*			GetEventManager() const { return m_pEventManager; }
		AudioManager*			GetAudioManager() const { return m_pAudioManager; }
		SoundDefManager*		GetSoundDefManager() const { return m_pSoundDefManager; }
		bool					Load(const char* szPath);
		bool					Save(const char* szPath);
		bool					LoadXML(const char* szPath);
		bool					SaveXML(const char* szPath);
		EventSystem*			GetEventSystem() const { return m_pEventSystem; }
		bool					SetClassVolume(int iClass, float fVolume);
		bool					SetClassMute(int iClass, bool bMute);
	protected:
		void					release();
	protected:
		EventManager*			m_pEventManager;
		SoundDefManager*		m_pSoundDefManager;
		AudioManager*			m_pAudioManager;		
		EventSystem*			m_pEventSystem;
		std::string				m_strName;
	};
}