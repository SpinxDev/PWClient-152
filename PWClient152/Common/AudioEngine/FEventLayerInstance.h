#pragma once

#include "FAudioEngine.h"
#include "FExportDecl.h"
#include <vector>
#include "FSoundInstance.h"
#include "FAudioInstance.h"
#include "FEventParameter.h"

namespace FMOD
{
	class ChannelGroup;
}

namespace AudioEngine
{
	class EventLayer;
	class EventProject;
	class EventInstance;

	class _EXPORT_DLL_ EventLayerInstance : public SoundInstance::SoundInstanceListener, public AudioInstance::AudioInstanceListener, public EventParameter::EventParameterListener
	{
		friend class AudioInstance;
		friend class EventInstance;
	public:
		class _EXPORT_DLL_ EventLayerInstanceListener
		{
		public:
			EventLayerInstanceListener(){}
			~EventLayerInstanceListener(){}
		public:
			virtual bool		OnPlay(EventLayerInstance* pEventLayer) { return true; }
			virtual bool		OnStop(EventLayerInstance* pEventLayer) { return true; }
		};
		typedef std::set<EventLayerInstanceListener*> ListenerSet;
	public:
		EventLayerInstance(void);
		~EventLayerInstance(void);
	public:
		bool					Init(EventLayer* pEventLayer, EventProject* pEventProject);
		bool					SetMute(bool bMute);
		void					AddListener(EventLayerInstanceListener* pListener);
		void					RemoveListener(EventLayerInstanceListener* pListener);
		SoundInstance*			GetCurSoundInstance() const;
		FMOD::ChannelGroup*		GetChannelGroup() const { return m_pChannelGroup; }
		//EventInstanceParameter*	GetEventInstanceParameter() const { return m_pEventInstanceParameter; }
		bool					AttachInstanceParameter(const char* szName);

		EventInstance*			GetEventInstance() const { return m_pEventInstance; }

		// 调用Event::Start()之后有效（即播放时有效）
		bool					ApplyEffect(Effect* pEffect);
		bool					RemoveEffect(Effect* pEffect);
		EventLayer*				GetEventLayer() const { return m_pEventLayer; }
		bool					Update(DWORD dwTickTime) { return update(dwTickTime); }
		CriticalSectionWrapper*	GetCriticalSectionWrapper() { return &m_csForThreadSafe; }

	protected:
		bool					listenerPlay();
		bool					listenerStop();
		virtual bool			OnPlay(SoundInstance* pSoundInstance);
		virtual bool			OnStop(SoundInstance* pSoundInstance);
		virtual bool			OnPlay(AudioInstance* pAudioInstance);
		bool					play();
		bool					pause(bool bPause);
		bool					stop(bool bForce = false, bool bEventStop = false);
		bool					update(DWORD dwTickTime);
		bool					loadData();
		bool					applyEffects(float fParamValue);
		bool					removeEffects();
		void					setEventInstance(EventInstance* pEventInstance) { m_pEventInstance = pEventInstance; }
		void					release();
		virtual bool			OnValueChanged(EventParameter* pEventParameter);
		float					getNormalizeParamValue() const;
	protected:
		FMOD::ChannelGroup*		m_pChannelGroup;
		SoundInstanceList		m_listSoundInstance;
		SoundInstanceSet		m_setCurInstances;
		SoundInstance*			m_pLastInstance;
		bool					m_bPlaying;
		bool					m_bStoping;
		ListenerSet				m_setListener;
		EventLayer*				m_pEventLayer;
		EventProject*			m_pEventProject;
		EventInstance*			m_pEventInstance;
		EventParameter*			m_pEventInstanceParameter;
		CriticalSectionWrapper	m_csForThreadSafe;
		bool					m_bBlockListenerStop;
	};
}