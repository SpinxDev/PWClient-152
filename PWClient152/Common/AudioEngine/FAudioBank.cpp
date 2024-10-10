#include "FAudioBank.h"
#include "FAudio.h"
#include "FAudioBankArchive.h"
#include "FEventProject.h"
#include "FAudioManager.h"
#include "FLogManager.h"
#include "xml/xmlcommon.h"
#include "FEventSystem.h"
#include <AFileImage.h>

using namespace AudioEngine;

static const unsigned int version = 0x100001;

AudioBank::AudioBank(void)
: m_BankType(BANK_TYPE_LOAD_INTO_MEMORY)
{
	m_pRootAudioBank = this;
	m_pParentContainer = 0;
}

AudioBank::~AudioBank(void)
{
}

bool AudioBank::Load(AFileImage* pFile)
{
	release();

	DWORD dwReadLen = 0;
	unsigned int ver = 0;
	pFile->Read(&ver, sizeof(ver), &dwReadLen);

	int iNameLen = 0;
	pFile->Read(&iNameLen, sizeof(iNameLen), &dwReadLen);
	char szName[1024] = {0};
	pFile->Read(szName, iNameLen, &dwReadLen);
	m_strName = szName;

	int iNoteLen = 0;
	pFile->Read(&iNoteLen, sizeof(iNoteLen), &dwReadLen);
	char szNote[1024] = {0};
	pFile->Read(szNote, iNoteLen, &dwReadLen);
	m_strNote = szNote;

	pFile->Read(&m_BankType, sizeof(m_BankType), &dwReadLen);

	int iAudioNum = 0;
	pFile->Read(&iAudioNum, sizeof(iAudioNum), &dwReadLen);
	for (int i=0; i<iAudioNum; ++i)
	{
		Audio* pAudio = new Audio;
		if(!pAudio->Init(m_pEventProject))
		{
			delete pAudio;
			return false;
		}
		pAudio->SetAudioBank(this);
		pAudio->SetParentContainer(this);
		if(!pAudio->Load(pFile))
		{
			m_pEventProject->GetEventSystem()->GetLogManager()->GetEngineLog()->Write(LOG_ERROR, "AudioBank::Load ¼ÓÔØAudio:%sÊ§°Ü", pAudio->GetPath());
			delete pAudio;
			return false;
		}
		m_listAudio.push_back(pAudio);
		if(!m_pEventProject->GetAudioManager()->AddToMap(pAudio))
			return false;
	}

	int iAudioBankArchiveNum = 0;
	pFile->Read(&iAudioBankArchiveNum, sizeof(iAudioBankArchiveNum), &dwReadLen);
	for (int i=0; i<iAudioBankArchiveNum; ++i)
	{
		AudioBankArchive* pAudioBankArchive = new AudioBankArchive;
		if(!pAudioBankArchive->Init(m_pEventProject))
		{
			delete pAudioBankArchive;
			return false;
		}
		pAudioBankArchive->SetAudioBank(this);
		pAudioBankArchive->SetParentContainer(this);
		if(!pAudioBankArchive->Load(pFile))
		{
			m_pEventProject->GetEventSystem()->GetLogManager()->GetEngineLog()->Write(LOG_ERROR, "AudioBank::Load ¼ÓÔØAudioBankArvhie:%sÊ§°Ü", pAudioBankArchive->GetName());
			delete pAudioBankArchive;
			return false;
		}		
		m_listAudioBankArchive.push_back(pAudioBankArchive);
	}

	return true;
}

bool AudioBank::Save(AFile* pFile)
{
	DWORD dwWriteLen = 0;
	pFile->Write((void*)&version, sizeof(version), &dwWriteLen);

	int iNameLen = (int)m_strName.size();
	pFile->Write(&iNameLen, sizeof(iNameLen), &dwWriteLen);
	pFile->Write((void*)m_strName.c_str(), iNameLen, &dwWriteLen);

	int iNoteLen = (int)m_strNote.size();
	pFile->Write(&iNoteLen, sizeof(iNoteLen), &dwWriteLen);
	pFile->Write((void*)m_strNote.c_str(), iNoteLen, &dwWriteLen);

	pFile->Write(&m_BankType, sizeof(m_BankType), &dwWriteLen);
	int iAudioNum = GetAudioNum();
	pFile->Write(&iAudioNum, sizeof(iAudioNum), &dwWriteLen);

	AudioList::const_iterator it = m_listAudio.begin();
	for (; it!=m_listAudio.end(); ++it)
	{
		Audio* pAudio = *it;
		if(!pAudio)
			return false;
		if(!pAudio->Save(pFile))
		{
			m_pEventProject->GetEventSystem()->GetLogManager()->GetEngineLog()->Write(LOG_ERROR, "AudioBank::Save ±£´æAudio:%sÊ§°Ü", pAudio->GetPath());
			return false;
		}
	}

	int iAudioBankArchiveNum = GetAudioBankArchiveNum();
	pFile->Write(&iAudioBankArchiveNum, sizeof(iAudioBankArchiveNum), &dwWriteLen);

	AudioBankArchiveList::const_iterator it2 = m_listAudioBankArchive.begin();
	for (; it2!=m_listAudioBankArchive.end(); ++it2)
	{
		AudioBankArchive* pAudioBankArchive = *it2;
		if(!pAudioBankArchive)
			return false;
		if(!pAudioBankArchive->Save(pFile))
		{
			m_pEventProject->GetEventSystem()->GetLogManager()->GetEngineLog()->Write(LOG_ERROR, "AudioBank::Save ±£´æAudioBankArvhie:%sÊ§°Ü", pAudioBankArchive->GetName());
			return false;
		}
	}
	return true;
}

bool AudioBank::LoadXML(TiXmlElement* root)
{
	release();

	unsigned int ver = 0;
	QueryElement(root, "version", ver);
	QueryElement(root, "name", m_strName);
	QueryElement(root, "note", m_strNote);
	int iBankType = 0;
	QueryElement(root, "bankType", iBankType);
	m_BankType = (BANK_TYPE)iBankType;
	int iAudioNum = 0;
	QueryElement(root, "audioNum", iAudioNum);
	TiXmlNode* pAudioNode = root->FirstChild("Audio");
	for (int i=0; i<iAudioNum; ++i)
	{
		Audio* pAudio = new Audio;
		if(!pAudio->Init(m_pEventProject))
		{
			delete pAudio;
			return false;
		}
		pAudio->SetAudioBank(this);
		pAudio->SetParentContainer(this);
		if(!pAudio->LoadXML(pAudioNode->ToElement()))
		{
			m_pEventProject->GetEventSystem()->GetLogManager()->GetEngineLog()->Write(LOG_ERROR, "AudioBank::LoadXML ¼ÓÔØAudio:%sÊ§°Ü", pAudio->GetPath());
			delete pAudio;
			return false;
		}
		m_listAudio.push_back(pAudio);
		if(!m_pEventProject->GetAudioManager()->AddToMap(pAudio))
			return false;
		pAudioNode = pAudioNode->NextSibling("Audio");
	}

	int iAudioBankArchiveNum = 0;
	QueryElement(root, "archiveNum", iAudioBankArchiveNum);
	TiXmlNode* pArchiveNode = root->FirstChild("AudioBankArchive");
	for (int i=0; i<iAudioBankArchiveNum; ++i)
	{
		AudioBankArchive* pAudioBankArchive = new AudioBankArchive;
		if(!pAudioBankArchive->Init(m_pEventProject))
		{
			delete pAudioBankArchive;
			return false;
		}
		pAudioBankArchive->SetAudioBank(this);
		pAudioBankArchive->SetParentContainer(this);
		if(!pAudioBankArchive->LoadXML(pArchiveNode->ToElement()))
		{
			m_pEventProject->GetEventSystem()->GetLogManager()->GetEngineLog()->Write(LOG_ERROR, "AudioBank::LoadXML ¼ÓÔØAudioBankArvhie:%sÊ§°Ü", pAudioBankArchive->GetName());
			delete pAudioBankArchive;
			return false;
		}
		m_listAudioBankArchive.push_back(pAudioBankArchive);
		pArchiveNode = pArchiveNode->NextSibling("AudioBankArchive");
	}
	return true;
}

bool AudioBank::SaveXML(TiXmlElement* pParent)
{
	TiXmlElement* root = new TiXmlElement("AudioBank");
	pParent->LinkEndChild(root);

	AddElement(root, "version", version);
	AddElement(root, "name", m_strName);
	AddElement(root, "note", m_strNote);
	AddElement(root, "bankType", m_BankType);
	AddElement(root, "audioNum", GetAudioNum());
	AudioList::const_iterator it = m_listAudio.begin();
	for (; it!=m_listAudio.end(); ++it)
	{
		Audio* pAudio = *it;
		if(!pAudio)
			return false;
		if(!pAudio->SaveXML(root))
		{
			m_pEventProject->GetEventSystem()->GetLogManager()->GetEngineLog()->Write(LOG_ERROR, "AudioBank::SaveXML ±£´æAudio:%sÊ§°Ü", pAudio->GetPath());
			return false;
		}
	}

	AddElement(root, "archiveNum", GetAudioBankArchiveNum());
	AudioBankArchiveList::const_iterator it2 = m_listAudioBankArchive.begin();
	for (; it2!=m_listAudioBankArchive.end(); ++it2)
	{
		AudioBankArchive* pAudioBankArchive = *it2;
		if(!pAudioBankArchive)
			return false;
		if(!pAudioBankArchive->SaveXML(root))
		{
			m_pEventProject->GetEventSystem()->GetLogManager()->GetEngineLog()->Write(LOG_ERROR, "AudioBank::SaveXML ±£´æAudioBankArvhie:%sÊ§°Ü", pAudioBankArchive->GetName());
			return false;
		}
	}
	return true;
}