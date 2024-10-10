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
	class AudioContainer;

	typedef std::map<Guid, Audio*> AudioMap;

	class _EXPORT_DLL_ AudioManager
	{
	public:
		AudioManager(void);
		~AudioManager(void);
	public:
		bool					Init(EventProject* pEventProject);
		AudioBank*				CreateAudioBank(const char* szName);
		AudioBankArchive*		CreateAudioBankArchive(AudioContainer* pAudioContainer, const char* szName);
		Audio*					GetAudio(const Guid& guid) const;
		AudioBank*				GetAudioBankByName(const char* szName) const;
		AudioBank*				GetAudioBankByIndex(int idx) const;
		int						GetAudioBankNum() const { return (int)m_listAudioBank.size(); }
		Audio*					CreateAudio(AudioContainer* pAudioContainer, const char* szFileName, const char* szPath);
		bool					Load(AFileImage* pFile);
		bool					Save(AFile* pFile);
		bool					LoadXML(TiXmlElement* root);
		bool					SaveXML(TiXmlElement* pParent);
		bool					IsAudioExist(AudioContainer* pAudioContainer, const char* szName) const;
		bool					IsArchiveExist(AudioContainer* pAudioContainer, const char* szName) const;
		bool					IsBankExist(const char* szName) const;
		bool					AddToMap(Audio* pAudio);
		bool					DeleteAudio(Audio* pAudio);
		bool					DeleteAudioBankArchive(AudioBankArchive* pAudioBankArchive);
		bool					DeleteAudioBank(const char* szName);
		bool					MoveAudioBankArchive(AudioBankArchive* pArchive, AudioContainer* pDestContainer);
		bool					MoveAudio(Audio* pAudio, AudioContainer* pDestContainer);
	protected:
		void					release();		
	protected:
		AudioMap				m_mapAudio;
		AudioBankList			m_listAudioBank;
		EventProject*			m_pEventProject;
	};
}
