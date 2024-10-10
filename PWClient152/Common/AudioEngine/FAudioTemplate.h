#pragma once

#include "FExportDecl.h"

#include <string>

class TiXmlElement;
class AFileImage;
class AFile;

namespace AudioEngine
{
	class Audio;
	class EventProject;
	class AudioGroup;

	struct AUDIO_PROPERTY
	{
		float				fVolume;
		float				fVolumeRandomization;
		float				fPitch;
		float				fPitchRandomization;
	};

	class _EXPORT_DLL_ AudioTemplate
	{
	public:
		AudioTemplate(void);
		~AudioTemplate(void);
	public:
		bool					Init(EventProject* pEventProject);
		const char*				GetNote() const { return m_strNote.c_str(); }
		void					SetNote(const char* szNote) { m_strNote = szNote; }
		Audio*					GetAudio() const { return m_pAudio; }
		void					SetAudio(Audio* pAudio) { m_pAudio = pAudio; }
		void					GetProperty(AUDIO_PROPERTY& prop) const { prop = m_AudioProperty; }
		void					SetProperty(const AUDIO_PROPERTY& prop) { m_AudioProperty = prop; }
		bool					Load(AFileImage* pFile);
		bool					Save(AFile* pFile);
		bool					LoadXML(TiXmlElement* root);
		bool					SaveXML(TiXmlElement* pParent);
		void					SetParentGroup(AudioGroup* pAudioGruop) { m_pParentGroup = pAudioGruop; }
		AudioGroup*				GetParentGroup() const { return m_pParentGroup; }
	protected:
		AUDIO_PROPERTY			m_AudioProperty;
		Audio*					m_pAudio;
		EventProject*			m_pEventProject;
		std::string				m_strNote;
		AudioGroup*				m_pParentGroup;
	};
}




