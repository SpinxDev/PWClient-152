#pragma once

#include "FExportDecl.h"

#include <list>

class TiXmlElement;
class AFileImage;
class AFile;

namespace AudioEngine
{
	class Audio;
	class AudioTemplate;
	class EventProject;

	typedef enum
	{
		AGPM_SEQUENTIAL,
		AGPM_RANDOM,
		AGPM_RANDOM_NOREPEAT
	}AUDIO_GROUP_PLAY_MODE;

	typedef std::list<AudioTemplate*> AudioTemplateList;

	class _EXPORT_DLL_ AudioGroup
	{
	public:
		AudioGroup(void);
		~AudioGroup(void);
	public:
		bool					Init(EventProject* pEventProject);
		AudioTemplate*			AddAudioTemplate(Audio* pAudio);
		bool					DeleteAudioTemplate(AudioTemplate* pAudioTemplate);
		bool					MoveUpAudioTemplate(AudioTemplate* pAudioTemplate);
		bool					MoveDownAudioTemplate(AudioTemplate* pAudioTemplate);		
		int						GetAudioTemplateNum() const;
		AudioTemplate*			GetAudioTemplateByIndex(int idx) const;
		bool					IsSingle() const { return m_bSingle; }
		void					SetSingle(bool bSingle) { m_bSingle = bSingle; }
		AUDIO_GROUP_PLAY_MODE	GetPlayMode() const { return m_iPlayMode; }
		void					SetPlayMode(AUDIO_GROUP_PLAY_MODE iPlayMode) { m_iPlayMode = iPlayMode; }
		bool					Load(AFileImage* pFile);
		bool					Save(AFile* pFile);
		bool					LoadXML(TiXmlElement* root);
		bool					SaveXML(TiXmlElement* pParent);
	protected:
		void					release();
	protected:
		AudioTemplateList		m_listAudioTemplate;
		EventProject*			m_pEventProject;
		bool					m_bSingle;
		AUDIO_GROUP_PLAY_MODE	m_iPlayMode;
	};
}