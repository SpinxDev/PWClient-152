#pragma once

#include "FAudioEngine.h"
#include "FExportDecl.h"
#include "FCriticalSectionWrapper.h"
#include <set>

namespace FMOD
{
	class Sound;	
}

namespace AudioEngine
{
	class EventSystem;
	class SoundInstanceTemplate;
	class AudioTemplate;
	class SoundInstance;

	class _EXPORT_DLL_ AudioInstance
	{
		friend class AudioInstanceManager;
	public:
		class _EXPORT_DLL_ AudioInstanceListener
		{
		public:
			AudioInstanceListener(){}
			~AudioInstanceListener(){}
		public:
			virtual bool		OnPlay(AudioInstance* pAudioInstance) { return true; }
			//virtual bool		OnStop(AudioInstance* pAudioInstance) { return true; }
			virtual bool		OnDelete(AudioInstance* pAudioInstance) { return true; }
		};
		typedef std::set<AudioInstanceListener*> ListenerSet;
	public:
		AudioInstance(void);
		~AudioInstance(void);
	public:
		bool					Init(EventSystem* pEventSystem, AudioTemplate* pAudioTemplate);

		// iLoopCound -1:无限循环；0:1次；n>0:n次
		bool					PlayForDef(SoundInstance* pSoundInstance, int iLoopCount);

		// iLoopCound -1:无限循环；0:1次；n>0:n次
		bool					PlayForEvent(SoundInstance* pSoundInstance, int iLoopCount);
		bool					SetAudio(AudioTemplate* pAudioTemplate);
		void					AddListener(AudioInstanceListener* pListener);
		void					RemoveListener(AudioInstanceListener* pListener);
		bool					ListenerPlay() { return listenerPlay(); }
		//bool					ListenerStop() { return listenerStop(); }		
		AudioTemplate*			GetAudioTemplate() const { return m_pAudioTemplate; }
		bool					Update(DWORD dwTickTime);
		bool					IsCanbeDeleted() const { return m_bCanbeDeleted; }
		void					RemoveChannel(FMOD::Channel* pChannel) { removeChannel(pChannel); }
	protected:
		bool					listenerPlay();
		//bool					listenerStop();
		bool					listenerDelete();
		void					release();
		void					setEventSystem(EventSystem* pEventSystem) { m_pEventSystem = pEventSystem; }
		bool					createSoundForEvent(bool bReload = false);
		bool					createSoundForSoundDef(bool bReload = false);		
		float					getVolumeRatio(float dbVolume, float dbVolumeRandmization);
		float					getPitchRatio(float tonePitch, float tonePitchRandmization);
		FMOD::Sound*			getSound() const { return m_pSound; }
		void					addChannel(FMOD::Channel* pChannel);
		void					removeChannel(FMOD::Channel* pChannel);

	protected:
		EventSystem*			m_pEventSystem;
		FMOD::Sound*			m_pSound;		
		AudioTemplate*			m_pAudioTemplate;
		ListenerSet				m_setListener;
		bool					m_bLoaded;
		CriticalSectionWrapper	m_csForThreadSafe;
		int						m_iReleaseDataInterval;
		ChannelList				m_listChannel;
		bool					m_bCanbeDeleted;
		unsigned int			m_OriginalMode;
	};
}
