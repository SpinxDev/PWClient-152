#pragma once

#include "FAudioEngine.h"
#include "FExportDecl.h"

#include <string>


class TiXmlElement;
class AFileImage;
class AFile;

namespace AudioEngine
{
	class _EXPORT_DLL_ SoundDefArchive
	{
		friend class SoundDefManager;
	public:
		SoundDefArchive(void);
		~SoundDefArchive(void);
	public:
		bool					Init(EventProject* pEventProject);
		const char*				GetName() const { return m_strName.c_str(); }
		void					SetName(const char* szName) { m_strName = szName; }
		const char*				GetNote() const { return m_strNote.c_str(); }
		void					SetNote(const char* szNote) { m_strNote = szNote; }
		int						GetSoundDefNum() const { return (int)m_listSoundDef.size(); }
		bool					Load(AFileImage* pFile);
		bool					Save(AFile* pFile);
		bool					LoadXML(TiXmlElement* root);
		bool					SaveXML(TiXmlElement* pParent);
		int						GetSoundDefArchiveNum() const { return (int)m_listSoundDefArchive.size(); }
		SoundDefArchive*		GetSoundDefArchiveByIndex(int idx) const;
		SoundDef*				GetSoundDefByIndex(int idx) const;
		void					SetParentArchive(SoundDefArchive* pParentArchive) { m_pParentArchive = pParentArchive; }
		SoundDefArchive*		GetParentArchive() const { return m_pParentArchive; }
	protected:
		void					release();
		SoundDef*				createSoundDef(const char* szName);
		SoundDefArchive*		createSoundDefArchive(const char* szName);
		bool					deleteSoundDef(const char* szName);
		bool					deleteSoundDefArchive(const char* szName);
		bool					removeSoundDef(SoundDef* pSoundDef);
		bool					addSoundDef(SoundDef* pSoundDef);
		bool					removeSoundDefArchive(SoundDefArchive* pSoundDefArchive);
		bool					addSoundDefArchive(SoundDefArchive* pSoundDefArchive);
	protected:
		std::string				m_strName;
		std::string				m_strNote;
		SoundDefList			m_listSoundDef;
		SoundDefArchiveList		m_listSoundDefArchive;
		EventProject*			m_pEventProject;
		SoundDefArchive*		m_pParentArchive;
	};
}