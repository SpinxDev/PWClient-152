#pragma once

#include <string>
#include "FExportDecl.h"
#include <set>


class TiXmlElement;
class AFileImage;
class AFile;

namespace AudioEngine
{
	class Event;

	typedef enum
	{
		VELOCITY_ONSHOT,
		VELOCITY_ONSHOT_STOP,
		VELOCITY_LOOP
	}VELOCIY_LOOP_MODE;

	typedef struct EVENT_PARAM_PROPERTY 
	{
		float					fMinValue;
		float					fMaxValue;
		float					fVelocity;		
		VELOCIY_LOOP_MODE		velocityLoopMode;
		float					fSeekSpeed;
	}EVENT_PARAM_PROPERTY;

	class _EXPORT_DLL_ EventParameter
	{
	public:
		class _EXPORT_DLL_ EventParameterListener
		{
		public:
			EventParameterListener(){}
			~EventParameterListener(){}
		public:
			virtual bool OnValueChanged(EventParameter* pEventParameter) { return true; }
		};
		typedef std::set<EventParameterListener*> ListenerSet;
	public:
		EventParameter(void);
		~EventParameter(void);
	public:
		void					GetProperty(EVENT_PARAM_PROPERTY& prop) const { prop = m_EventParamProperty; }
		void					SetProperty(const EVENT_PARAM_PROPERTY& prop) { m_EventParamProperty = prop; }
		const char*				GetName() const { return m_strName.c_str(); }
		void					SetName(const char* szName) { m_strName = szName; }
		float					GetValue() const { return m_fValue; }
		void					SetValue(float value, bool bSeek = true);
		bool					Load(AFileImage* pFile);
		bool					Save(AFile* pFile);
		bool					LoadXML(TiXmlElement* root);
		bool					SaveXML(TiXmlElement* pParent);
		void					SetEvent(Event* pEvent) { m_pEvent = pEvent; }
		Event*					GetEvent() const { return m_pEvent; }
		void					AddListener(EventParameterListener* pListener);
		void					RemoveListener(EventParameterListener* pListener);
		void					FinishSeek();
		bool					GetSeekState() const { return m_bSeek; }
		bool					GetForwardSeekState() const { return m_bForwardSeek; }
		float					GetSeekValue() const { return m_fSeekValue; }
		void					Reset();
	protected:
		bool					listenerValueChanged();
	protected:
		ListenerSet				m_setListener;
		EVENT_PARAM_PROPERTY	m_EventParamProperty;
		std::string				m_strName;
		float					m_fValue;
		Event*					m_pEvent;
		float					m_fSeekValue;
		bool					m_bSeek;
		bool					m_bForwardSeek;
	};
}
