#pragma once

#include "FAudioContainer.h"

class AFileImage;
class AFile;

namespace AudioEngine
{
	class AudioManager;

	typedef enum
	{
		BANK_TYPE_LOAD_INTO_MEMORY,			// 不解压直接加载进内存
		BANK_TYPE_STREAM_FROM_DISK,			// 流加载
		BANK_TYPE_DECOMPRESS_INTO_MEMORY,	// 解压后加载进内存
	}BANK_TYPE;

	class _EXPORT_DLL_ AudioBank : public AudioContainer
	{
		friend class AudioManager;
	public:
		AudioBank(void);
		~AudioBank(void);
	public:
		void					SetBankType(BANK_TYPE type) { m_BankType = type; }
		BANK_TYPE				GetBankType() const { return m_BankType; }		
		virtual bool			Load(AFileImage* pFile);
		virtual bool			Save(AFile* pFile);
		virtual bool			LoadXML(TiXmlElement* root);
		virtual bool			SaveXML(TiXmlElement* pParent);
	protected:		
		BANK_TYPE				m_BankType;		
		std::string				m_strNotes;
		
	};
}
