#pragma once

#include "FGuid.h"
#include "FExportDecl.h"

#include <string>

class TiXmlElement;
class AFileImage;
class AFile;

namespace AudioEngine
{
	class EventSystem;
	class EventProject;
	class AudioBankArchive;
	class AudioContainer;

	class _EXPORT_DLL_ Audio
	{
		friend class AudioInstance;
		friend class AudioBank;
	public:
		Audio(void);
		~Audio(void);
	public:
		bool					Init(EventProject* pEventProject);
		void					SetName(const char* szName) { m_strName = szName; }
		const char*				GetName() const { return m_strName.c_str(); }
		const char*				GetNote() const { return m_strNote.c_str(); }
		void					SetNote(const char* szNote) { m_strNote = szNote; }
		const char*				GetPath() const { return m_strPath.c_str(); }
		void					SetPath(const char* szPath) { m_strPath = szPath; }
		bool					Load(AFileImage* pFile);
		bool					Save(AFile* pFile);
		bool					LoadXML(TiXmlElement* root);
		bool					SaveXML(TiXmlElement* pParent);
		void					SetGuid(const Guid& guid) { m_Guid = guid; }
		Guid					GetGuid() const { return m_Guid; }
		AudioBank*				GetAudioBank() const { return m_pAudioBank; }
		void					SetAudioBank(AudioBank* pAudioBank) { m_pAudioBank = pAudioBank; }
		void					SetParentContainer(AudioContainer* pContainer) { m_pParentContainer = pContainer; }
		AudioContainer*			GetParentContainer() const { return m_pParentContainer; }
		void					EnableAccurateTime(bool bAccurateTime) { m_bAccurateTime = bAccurateTime; }
		bool					IsAccurateTime() const { return m_bAccurateTime; }
	protected:
		void					release();
	protected:
		EventProject*			m_pEventProject;
		std::string				m_strName;
		std::string				m_strNote;
		std::string				m_strPath;
		bool					m_bAccurateTime;
		AudioBank*				m_pAudioBank;
		Guid					m_Guid;
		AudioContainer*			m_pParentContainer;
	};
}
