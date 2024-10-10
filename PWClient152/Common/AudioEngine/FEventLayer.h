#pragma once

#include "FAudioEngine.h"
#include "FSoundInstance.h"
#include "FAudioInstance.h"
#include "FExportDecl.h"
#include <string>


class TiXmlElement;
class AFileImage;
class AFile;

namespace AudioEngine
{
	class EventParameter;	

	class _EXPORT_DLL_ EventLayer
	{
		friend class Event;	
	public:
		EventLayer(void);
		~EventLayer(void);
	public:
		bool					Init(Event* pEvent, EventProject* pEventProject);
		Event*					GetEvent() const { return m_pEvent; }
		SoundInstanceTemplate*	AddSoundInstanceTemplate(SoundDef* pSoundDef);
		bool					DeleteSoundInstanceTemplate(SoundInstanceTemplate* pSoundInstanceTemplate);
		const char*				GetName() const { return m_strName.c_str(); }
		void					SetName(const char* szName) { m_strName = szName; }
		bool					HasEffectType(EFFECT_TYPE type);
		Effect*					AddEffect(EFFECT_TYPE type);
		bool					DeleteEffect(Effect* pEffect);
		int						GetEffectNum() const { return (int)m_listEffect.size(); }
		Effect*					GetEffectByIndex(int idx) const;
		bool					AttachParameter(const char* szName);
		const char*				GetParameterName() const;
		EventParameter*			GetParameter() const { return m_pEventParameter; }		
		SoundInstanceTemplate*	GetSoundInstanceTemplateByIndex(int idx) const;		
		int						GetSoundInstanceTemplateNum() const { return (int)m_listSoundInstanceTemplate.size(); }
		bool					Load(AFileImage* pFile);
		bool					Save(AFile* pFile);
		bool					LoadXML(TiXmlElement* root, bool bPreset = false);
		bool					SaveXML(TiXmlElement* pParent, bool bPreset = false);
	public:		
		void					release();
		bool					loadData(bool bReload = false);
	protected:
		SoundInstanceTemplateList	m_listSoundInstanceTemplate;		
		Event*					m_pEvent;
		std::string				m_strName;
		EffectList				m_listEffect;
		EventParameter*			m_pEventParameter;
		EventProject*			m_pEventProject;
	};
}