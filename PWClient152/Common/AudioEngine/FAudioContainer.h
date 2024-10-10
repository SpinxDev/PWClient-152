#pragma once

#include "FAudioEngine.h"
#include "FExportDecl.h"

#include <string>


class TiXmlElement;
class AFileImage;
class AFile;

namespace AudioEngine
{	
	class _EXPORT_DLL_ AudioContainer
	{
		friend class AudioManager;
	public:
		AudioContainer(void);
		virtual ~AudioContainer(void);
	public:		
		bool					Init(EventProject* pEventProject);
		void					SetName(const char* szName) { m_strName = szName; }
		const char*				GetName() const { return m_strName.c_str(); }
		const char*				GetNote() const { return m_strNote.c_str(); }
		void					SetNote(const char* szNote) { m_strNote = szNote; }
		AudioBankArchive*		GetAudioBankArchiveByIndex(int idx) const;
		int						GetAudioBankArchiveNum() const { return (int)m_listAudioBankArchive.size(); }
		Audio*					GetAudioByIndex(int idx) const;
		int						GetAudioNum() const { return (int)m_listAudio.size(); }
		virtual bool			Load(AFileImage* pFile) = 0;
		virtual bool			Save(AFile* pFile) = 0;
		virtual bool			LoadXML(TiXmlElement* root) = 0;
		virtual bool			SaveXML(TiXmlElement* pParent) = 0;
		AudioContainer*			GetParentAudioContainer() const { return m_pParentContainer; }
	protected:
		void					release();
		Audio*					createAudio(const char* szName, const char* szPath);
		bool					deleteAudio(const char* szName);
		AudioBankArchive*		createAudioBankArchive(const char* szName);
		bool					deleteAudioBankArchive(const char* szName);

		bool					addAudio(Audio* pAudio);
		bool					removeAudio(Audio* pAudio);
		bool					addAudioBankArchive(AudioBankArchive* pAudioBankArchive);
		bool					removeAudioBankArchive(AudioBankArchive* pAudioBankArchive);
	protected:
		std::string				m_strName;
		std::string				m_strNote;
		AudioList				m_listAudio;
		AudioBankArchiveList	m_listAudioBankArchive;
		EventProject*			m_pEventProject;
		AudioBank*				m_pRootAudioBank;
		AudioContainer*			m_pParentContainer;
	};
}
