#pragma once

#include "FAudioContainer.h"

class AFileImage;
class AFile;

namespace AudioEngine
{
	class AudioManager;

	class _EXPORT_DLL_ AudioBankArchive : public AudioContainer
	{
		friend class AudioManager;
		friend class AudioBank;
	public:
		AudioBankArchive(void);
		~AudioBankArchive(void);
	public:
		virtual bool			Load(AFileImage* pFile);
		virtual bool			Save(AFile* pFile);
		virtual bool			LoadXML(TiXmlElement* root);
		virtual bool			SaveXML(TiXmlElement* pParent);
		AudioBank*				GetAudioBank() const { return m_pRootAudioBank; }
		void					SetAudioBank(AudioBank* pAudioBank) { m_pRootAudioBank = pAudioBank; }
		AudioContainer*			GetParentContainer() const { return m_pParentContainer; }
		void					SetParentContainer(AudioContainer* pParentContainer) { m_pParentContainer = pParentContainer; }
	};
}
