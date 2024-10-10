#pragma once

#include "FAudioEngine.h"
#include "FExportDecl.h"
#include "FCriticalSectionWrapper.h"
#include "FAudioInstance.h"
#include <vector>
#include <string>
#include <map>
#include <utility>

class TiXmlElement;
class AFileImage;
class AFile;

namespace AudioEngine
{
	class SoundDef;
	class EventProject;
	class SoundInstance;
	class AudioInstance;
	class SoundDef;
	class EventLayer;

	typedef std::pair<AudioTemplate*, AudioInstance*> AudioInstancePair;
	typedef std::map<int, AudioInstancePair> AudioInstancePairMap;

	class _EXPORT_DLL_ SoundInstanceTemplate : public AudioInstance::AudioInstanceListener
	{
		friend class EventLayer;
		friend class EventLayerInstance;
		friend class SoundInstance;
	public:
		SoundInstanceTemplate(void);
		~SoundInstanceTemplate(void);
	public:
		bool					Init(EventProject* pEventProject);
		const char*				GetNote() const { return m_strNote.c_str(); }
		void					SetNote(const char* szNote) { m_strNote = szNote; }
		SoundDef*				GetSoundDef() const { return m_pSoundDef; }
		bool					SetSoundDef(SoundDef* pSoundDef);
		AudioInstance*			GetAudioInstanceByIndex(int idx, bool bPlayForEvent, bool bReload);
		int						GetAudioInstanceNum() const { return (int)m_mapAudioInstancePair.size(); }
		void					GetProperty(SOUND_INSTANCE_PROPERTY& prop) const { prop = m_SoundInstanceProperty; }
		void					SetProperty(const SOUND_INSTANCE_PROPERTY& prop) { m_SoundInstanceProperty = prop; }
		EventLayer*				GetEventLayer() const { return m_pEventLayer; }
		bool					Load(AFileImage* pFile);
		bool					Save(AFile* pFile);
		bool					LoadXML(TiXmlElement* root, bool bPreset = false);
		bool					SaveXML(TiXmlElement* pParent, bool bPreset = false);
	protected:
		bool					loadData(bool bReload = false);
		void					clearData();
		void					release();
		void					setEventLayer(EventLayer* pEventLayer) { m_pEventLayer = pEventLayer; }
		virtual bool			OnDelete(AudioInstance* pAudioInstance);
	protected:
		std::string				m_strNote;
		SoundDef*				m_pSoundDef;
		EventProject*			m_pEventProject;
		SOUND_INSTANCE_PROPERTY	m_SoundInstanceProperty;
		AudioInstancePairMap	m_mapAudioInstancePair;
		bool					m_bLoaded;
		bool					m_bPlayForEvent;
		EventLayer*				m_pEventLayer;
		CriticalSectionWrapper	m_csForThreadSafe;
	};
}
