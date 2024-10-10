#pragma once

#include "FAudioEngine.h"
#include "FCriticalSectionWrapper.h"
#include <map>

namespace AudioEngine
{
	class EventSystem;
	class AudioInstance;
	class AudioTemplate;

	typedef std::map<AudioTemplate*, AudioInstance*> AudioInstanceMap;

	class AudioInstanceManager
	{
	protected:
		AudioInstanceManager(void);
		~AudioInstanceManager(void);
	public:
		bool							Init(EventSystem* pEventSystem);
		static AudioInstanceManager&	GetInstance();
		AudioInstance*					GetAudioInstance(AudioTemplate* pAudioTemplate, bool bPlayForEvent, bool bReload);
		bool							Update(DWORD dwTickTime);
		void							Release();
	protected:
		void							releaseAudioInstance(AudioInstance* pAudioInstance);
	protected:
		EventSystem*					m_pEventSystem;
		AudioInstanceMap				m_mapAudioInstance;
		CriticalSectionWrapper			m_csForThreadSafe;
	};
}