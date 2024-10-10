#pragma once

#include "FAudioEngine.h"
#include "FExportDecl.h"
#include "FEventLayerInstance.h"
#include "FCriticalSectionWrapper.h"

namespace AudioEngine
{
	typedef enum LAYER_INDEX
	{
		LAYER_ALL	= -1,
		LAYER_1		=	1,
		LAYER_2		=	2,
		LAYER_3		=	4,
		LAYER_4		=	8,
		LAYER_5		=	16,
		LAYER_6		=	32,
		LAYER_7		=	64,
		LAYER_8		=	128,
		LAYER_9		=	256,
		LAYER_10	=	512,
		LAYER_11	=	1024,
		LAYER_12	=	2048,
		LAYER_13	=	4096,
		LAYER_14	=	8192,
		LAYER_15	=	16384,
		LAYER_16	=	32768,
		LAYER_17	=	65536,
		LAYER_18	=	131072,
		LAYER_19	=	262144,
		LAYER_20	=	524288,
		LAYER_21	=	1048576,
		LAYER_22	=	2097152,
		LAYER_23	=	4194304,
		LAYER_24	=	8388608,
		LAYER_25	=	16777216,
		LAYER_26	=	33554432,
		LAYER_27	=	67108864,
		LAYER_28	=	134217728,
		LAYER_29	=	268435456,
		LAYER_30	=	536870912,
		LAYER_31	=	1073741824,
		LAYER_32	=	2147483648
	}LAYER_INDEX;

	class _EXPORT_DLL_ EventInstance : public EventLayerInstance::EventLayerInstanceListener
	{
		friend class Event;
	public:
		class _EXPORT_DLL_ EventInstanceListener
		{
		public:
			EventInstanceListener(){}
			~EventInstanceListener(){}
		public:
			virtual bool		OnPlay(EventInstance* pEventInstance) { return true; }
			virtual bool		OnStop(EventInstance* pEventInstance) { return true; }
		};
		typedef std::set<EventInstanceListener*> ListenerSet;
	public:
		EventInstance(void);
		~EventInstance(void);
	public:
		bool					Init(Event* pEvent, EventProject* pEventProject, bool bAutoDestroy = true);
		bool					Start(LAYER_INDEX LayerIndex=LAYER_ALL);
		bool					Pause(bool bPause);
		bool					Stop(bool bForce = false);
		void					GetProperty(EVENT_PROPERTY& prop) const { prop = m_EventProperty; }
		void					SetProperty(const EVENT_PROPERTY& prop) { m_EventProperty = prop; }
		void					AddListener(EventInstanceListener* pListener);
		void					RemoveListener(EventInstanceListener* pListener);
		bool					IsPlaying() const { return m_bPlaying; }
		bool					IsStopping() const { return m_bStopping; }
		bool					IsPaused() const { return m_bPaused; }
		bool					Set3DAttributes(const VECTOR& pos, const VECTOR& vel);
		bool					Set3DConeOrientation(const VECTOR& orientation);
		void					Enable3DCone(bool bEnable) { m_bEnableOrientation = bEnable; }
		FMOD::ChannelGroup*		GetChannelGroup() const { return m_pChannelGroup; }
		EventLayerInstance*		GetEventLayerInstanceByIndex(int idx) const;
		EventInstanceParameter*	GetInstanceParameterByName(const char* szName) const;
		EventInstanceParameter*	GetInstanceParameterByIndex(int idx) const;
		int						GetInstanceParameterNum() const;
		VECTOR					GetPos() const { return m_vPos; }
		VECTOR					GetVel() const { return m_vVel; }
		bool					IsAutoDestroy() const;
		Event*					GetEvent() const;
		bool					SetVolume(float fVolume);
		float					GetVolume() const { return m_fVolume; }
		float					GetCurVolume() const;
		bool					IsCurInfiniteLoop() const;
		CriticalSectionWrapper*	GetCriticalSectionWrapper() { return &m_csForThreadSafe; }
		bool					GetCurrentChannelPosition(unsigned int& iMillisecond);
		bool					SetCurrentChannelPosition(unsigned int iMillisecond);
	protected:
		virtual bool			OnPlay(EventLayerInstance* pEventLayer);
		virtual bool			OnStop(EventLayerInstance* pEventLayer);
		bool					listenerPlay();
		bool					listenerStop();
		bool					update(DWORD dwTickTime);
		bool					isValidLayer(int idx);
		bool					apply3DAttributes();
		bool					apply3DCone(bool upate);
		void					release();
		void					fadein();
		void					fadeout();
		void					updateParameter();
		void					resetParameter();
		void					resetLastProperty();
	protected:
		ListenerSet				m_setListener;
		EventLayerInstanceList	m_listEventLayerInstance;
		EventLayerInstanceSet	m_setPlayingLayerInstance;
		VECTOR					m_vPos;
		VECTOR					m_vVel;
		VECTOR					m_vLastPos;
		VECTOR					m_vLastVel;
		VECTOR					m_vOrientation;
		VECTOR					m_vLastOrientation;
		bool					m_bEnableOrientation;
		bool					m_bPlaying;
		bool					m_bStopping;
		bool					m_bPaused;
		LAYER_INDEX				m_LayerIndex;
		EVENT_PROPERTY			m_EventProperty;
		FMOD::ChannelGroup*		m_pChannelGroup;
		EventProject*			m_pEventProject;
		Event*					m_pEvent;
		DWORD					m_dwStartTickCount;
		DWORD					m_dwStopTickCount;
		DWORD					m_dwParamLastTickCount;
		bool					m_bAutoDestroy;
		float					m_fVolume;
		float					m_fLastFadeInVolume;
		EventInstanceParameterList m_listEventInstanceParameter;
		CriticalSectionWrapper	m_csForThreadSafe;
	};
}
