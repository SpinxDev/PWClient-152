#pragma once

#include "FAudioEngine.h"
#include "FGuid.h"
#include "FExportDecl.h"

#include <map>


class TiXmlElement;
class AFileImage;
class AFile;

namespace AudioEngine
{
	class EventProject;
	class SoundDef;
	typedef std::map<Guid, SoundDef*> SoundDefMap;

	class _EXPORT_DLL_ SoundDefManager
	{
	public:
		SoundDefManager(void);
		~SoundDefManager(void);
	public:
		bool					Init(EventProject* pEventProject);
		SoundDef*				CreateSoundDef(SoundDefArchive* pSoundDefArchive, const char* szName);
		SoundDef*				GetSoundDef(const Guid& guid) const;
		bool					Load(AFileImage* pFile);
		bool					Save(AFile* pFile);
		bool					LoadXML(TiXmlElement* root);
		bool					SaveXML(TiXmlElement* pParent);
		SoundDefArchive*		CreateSoundDefArchive(const char* szName);
		SoundDefArchive*		CreateSoundDefArchive(SoundDefArchive* pSoundDefArchive, const char* szName);
		int						GetSoundDefArchiveNum() const { return (int)m_listSoundDefArchive.size(); }
		SoundDefArchive*		GetSoundDefArchiveByIndex(int idx) const;
		bool					AddToMap(SoundDef* pSoundDef);
		bool					IsArchiveExist(SoundDefArchive* pSoundDefArchive, const char* szName);
		bool					IsArchiveExist(const char* szName);
		bool					IsSoundDefExist(SoundDefArchive* pSoundDefArchive, const char* szName);
		bool					DeleteSoundDef(SoundDef* pSoundDef);
		bool					DeleteSoundDefArchive(SoundDefArchive* pSoundDefArchive);
		bool					MoveSoundDef(SoundDef* pSoundDef, SoundDefArchive* pDestArchive);
		bool					MoveArchive(SoundDefArchive* pArchive, SoundDefArchive* pDestArchive);
		bool					MoveArchive(SoundDefArchive* pArchive);
		const SoundDefMap&		GetSoundDefMap() const { return m_mapSoundDef; }
	protected:
		void					release();
		bool					deleteSoundDefArchive(const char* szName);
		bool					removeSoundDefArchive(SoundDefArchive* pSoundDefArchive);
		bool					addSoundDefArchive(SoundDefArchive* pSoundDefArchive);
	protected:
		SoundDefMap				m_mapSoundDef;
		EventProject*			m_pEventProject;
		SoundDefArchiveList		m_listSoundDefArchive;
	};
}
