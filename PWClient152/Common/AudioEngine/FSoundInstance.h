#pragma once

#include "FAudioEngine.h"
#include "FExportDecl.h"
#include "FSoundPlayList.h"
#include "FCriticalSectionWrapper.h"

#include <set>
#include <vector>

namespace FMOD
{
	class ChannelGroup;
	class Channel;
}

namespace AudioEngine
{
	class SoundDef;
	class EventLayer;
	class EventProject;
	class SoundInstanceTemplate;
	class EventLayerInstance;

	typedef std::vector<unsigned int> LengthArray;

	class _EXPORT_DLL_ SoundInstance
	{
		friend class EventLayer;
		friend class EventLayerInstance;
		friend class AudioInstance;
	public:
		class _EXPORT_DLL_ SoundInstanceListener
		{
		public:
			SoundInstanceListener(){}
			~SoundInstanceListener(){}
		public:
			virtual bool		OnPlay(SoundInstance* pSoundInstance) { return true; }
			virtual bool		OnStop(SoundInstance* pSoundInstance) { return true; }
		};
		typedef std::set<SoundInstanceListener*> ListenerSet;
	public:
		SoundInstance();
		~SoundInstance(void);
	public:
		bool					Init(EventLayerInstance* pEventLayerInstance, SoundInstanceTemplate* pSoundInstanceTemplate, EventProject* pEventProject);
		bool					Init(SoundInstanceTemplate* pSoundInstanceTemplate, EventProject* pEventProject);
		bool					PlayForSoundDef();
		EventLayerInstance*		GetEventLayerInstance() const { return m_pEventLayerInstance; }
		FMOD::ChannelGroup*		GetChannelGroup() const { return m_pChannelGroup; }
		bool					Stop(bool bForced = false, bool bEventStop = false);
		bool					Pause(bool bPaused);
		int						GetIntervalTime() const;
		int						GetPreIntervalTime() const;
		void					AddListener(SoundInstanceListener* pListener);
		void					RemoveListener(SoundInstanceListener* pListener);
		bool					IsPlaying() const { return m_bPlaying; }
		bool					IsStoping() const { return m_bStoping; }
		void					SetPlaying(bool bPlaying) { m_bPlaying = bPlaying; }
		SoundInstanceTemplate*	GetSoundInstanceTemplate() const { return m_pSoundInstanceTemplate; }
		SoundPlayList&			GetSoundPlayList() { return m_SoundPlayList; }
		bool					IsPlayForEvent() const { return m_bPlayForEvent; }
		bool					SetLoopOff();
		bool					SetLoopOn();
		void					SetLeftIntervalTime(int iLeftIntervalTime) { m_iLeftIntervalTime = iLeftIntervalTime; }
		void					EllapseIntervalTime(int iEllapseTime) { m_iLeftIntervalTime -= iEllapseTime; }
		int						GetLeftIntervalTime() const { return m_iLeftIntervalTime; }
		CriticalSectionWrapper*	GetCriticalSectionWrapper() { return &m_csForThreadSafe; }
		void					AddChannel(FMOD::Channel* pChannel);
		void					RemoveChannel(FMOD::Channel* pChannel);
		void					GetPlayingChannelList(ChannelList& listPlayingChannel);
	protected:
		bool					update(DWORD dwTickTime);
		void					release();		
		void					setEventLayerInstance(EventLayerInstance* pEventLayerInstance) { m_pEventLayerInstance = pEventLayerInstance; }
		bool					playForEvent();
		bool					reset();
		bool					listenerPlay();
		bool					listenerStop();
		void					fadein();
		void					fadeout();
		bool					firstPlay();
		void					clearChannel();
	protected:
		SoundDef*				m_pSoundDef;
		EventProject*			m_pEventProject;
		EventLayerInstance*		m_pEventLayerInstance;
		FMOD::ChannelGroup*		m_pChannelGroup;
		bool					m_bPlaying;
		bool					m_bStoping;
		bool					m_bPlayForEvent;
		DWORD					m_dwStartTickCount;
		DWORD					m_dwStopTickCount;
		ListenerSet				m_setListener;
		SoundPlayList			m_SoundPlayList;
		SoundInstanceTemplate*	m_pSoundInstanceTemplate;
		SOUND_INSTANCE_PROPERTY	m_SoundInstanceProperty;	
		CriticalSectionWrapper	m_csForThreadSafe;
		int						m_iLeftIntervalTime;
		bool					m_bFirstPlay;
		int						m_iLeftPreIntervalTime;
		ChannelList				m_listChannel;
	};
}