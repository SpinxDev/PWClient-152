#include "FAudioManager.h"
#include "FAudioBank.h"
#include "FAudio.h"
#include "FAudioBankArchive.h"
#include "FAudioContainer.h"
#include "FLogManager.h"
#include "xml/xmlcommon.h"
#include "FEventSystem.h"
#include "FEventProject.h"
#include <AFileImage.h>

using namespace AudioEngine;

static const unsigned int version = 0x100001;

AudioManager::AudioManager(void)
: m_pEventProject(0)
{
}

AudioManager::~AudioManager(void)
{
	release();
}

bool AudioManager::Init(EventProject* pEventProject)
{
	if(!pEventProject)
		return false;
	m_pEventProject = pEventProject;
	return true;
}

AudioBank* AudioManager::CreateAudioBank(const char* szName)
{
	AudioBank* pAudioBank = new AudioBank;
	if(!pAudioBank->Init(m_pEventProject))
	{
		delete pAudioBank;
		return 0;
	}
	pAudioBank->SetName(szName);
	m_listAudioBank.push_back(pAudioBank);
	return pAudioBank;
}

AudioBankArchive* AudioManager::CreateAudioBankArchive(AudioContainer* pAudioContainer, const char* szName)
{
	if(!pAudioContainer)
		return 0;
	if(IsArchiveExist(pAudioContainer, szName))
		return 0;
	AudioBankArchive* pChildAudioBankArchive = pAudioContainer->createAudioBankArchive(szName);
	return pChildAudioBankArchive;
}

Audio* AudioManager::GetAudio(const Guid& guid) const
{
	AudioMap::const_iterator it = m_mapAudio.find(guid);
	if(it == m_mapAudio.end())
		return 0;
	return it->second;
}

void AudioManager::release()
{
	AudioBankList::const_iterator it = m_listAudioBank.begin();
	for (; it!=m_listAudioBank.end(); ++it)
	{
		delete *it;
	}
	m_listAudioBank.clear();

	m_mapAudio.clear();
}

AudioBank* AudioManager::GetAudioBankByName(const char* szName) const
{
	AudioBankList::const_iterator it = m_listAudioBank.begin();
	for (; it!=m_listAudioBank.end(); ++it)
	{
		AudioBank* pAudioBank = *it;
		if(!pAudioBank)
			continue;
		if(0 == strcmp(szName, pAudioBank->GetName()))
			return pAudioBank;
	}
	return 0;
}

AudioBank* AudioManager::GetAudioBankByIndex(int idx) const
{
	AudioBankList::const_iterator it = m_listAudioBank.begin();
	for (int i=0; it!=m_listAudioBank.end(); ++it, ++i)
	{
		if(i == idx)
			return *it;
	}
	return 0;
}

Audio* AudioManager::CreateAudio(AudioContainer* pAudioContainer, const char* szFileName, const char* szPath)
{
	if(!pAudioContainer)
		return 0;
	Audio* pAudio = pAudioContainer->createAudio(szFileName, szPath);
	if(!pAudio)
		return 0;
	m_mapAudio[pAudio->GetGuid()] = pAudio;
	return pAudio;
}

bool AudioManager::IsArchiveExist(AudioContainer* pAudioContainer, const char* szName) const
{
	int iArchiveNum = pAudioContainer->GetAudioBankArchiveNum();
	if(iArchiveNum == 0)
		return false;
	for (int i=0; i<iArchiveNum; ++i)
	{
		AudioBankArchive* pChildAudioBankArchive = pAudioContainer->GetAudioBankArchiveByIndex(i);
		if(GF_CompareNoCase(szName, strlen(szName), pChildAudioBankArchive->GetName(), strlen(pChildAudioBankArchive->GetName())))
			return true;
	}
	return false;
}

bool AudioManager::IsAudioExist(AudioContainer* pAudioContainer, const char* szName) const
{
	int iAudioNum = pAudioContainer->GetAudioNum();
	if(iAudioNum == 0)
		return false;
	for (int i=0; i<iAudioNum; ++i)
	{
		Audio* pAudio = pAudioContainer->GetAudioByIndex(i);
		if(GF_CompareNoCase(szName, strlen(szName), pAudio->GetName(), strlen(pAudio->GetName())))
			return true;
	}
	return false;
}

bool AudioManager::IsBankExist(const char* szName) const
{
	int iBankNum = GetAudioBankNum();
	if(iBankNum == 0)
		return false;
	for (int i=0; i<iBankNum; ++i)
	{
		AudioBank* pAudioBank = GetAudioBankByIndex(i);
		if(!pAudioBank)
			continue;
		if(GF_CompareNoCase(pAudioBank->GetName(), strlen(pAudioBank->GetName()), szName, strlen(szName)))
			return true;
	}
	return false;
}

bool AudioManager::AddToMap(Audio* pAudio)
{
	if(!pAudio)
		return false;
	m_mapAudio[pAudio->GetGuid()] = pAudio;
	return true;
}

bool AudioManager::DeleteAudio(Audio* pAudio)
{
	if(!pAudio)
		return false;
	Guid guid = pAudio->GetGuid();
	if(!pAudio->GetParentContainer()->deleteAudio(pAudio->GetName()))
		return false;
	m_mapAudio.erase(m_mapAudio.find(guid));
	return true;
}

bool AudioManager::DeleteAudioBankArchive(AudioBankArchive* pAudioBankArchive)
{
	if(!pAudioBankArchive)
		return false;
	return pAudioBankArchive->GetParentContainer()->deleteAudioBankArchive(pAudioBankArchive->GetName());
}

bool AudioManager::DeleteAudioBank(const char* szName)
{
	int iBankNum = GetAudioBankNum();
	if(iBankNum == 0)
		return false;
	AudioBankList::iterator it = m_listAudioBank.begin();
	for (; it!=m_listAudioBank.end(); ++it)
	{
		AudioBank* pAudioBank = *it;
		if(!pAudioBank)
			return false;
		if(GF_CompareNoCase(pAudioBank->GetName(), strlen(pAudioBank->GetName()), szName, strlen(szName)))
		{
			while(pAudioBank->GetAudioNum())
			{
				Audio* pAudio = pAudioBank->GetAudioByIndex(0);
				if(!pAudio)
					return false;
				if(!DeleteAudio(pAudio))
					return false;
			}

			while(pAudioBank->GetAudioBankArchiveNum())
			{
				AudioBankArchive* pAudioBankArchive = pAudioBank->GetAudioBankArchiveByIndex(0);
				if(!pAudioBankArchive)
					return false;
				if(!DeleteAudioBankArchive(pAudioBankArchive))
					return false;
			}

			delete pAudioBank;
			m_listAudioBank.erase(it);
			return true;
		}
	}
	return false;
}

bool AudioManager::MoveAudio(Audio* pAudio, AudioContainer* pDestContainer)
{
	if(!pAudio || !pDestContainer)
		return false;
	AudioContainer* pOldParentContainer = pAudio->GetParentContainer();
	if(pOldParentContainer == pDestContainer)
		return true;
	if(!pDestContainer->addAudio(pAudio))
		return false;
	if(!pOldParentContainer->removeAudio(pAudio))
	{
		if(!pDestContainer->removeAudio(pAudio))
			return false;
		return false;
	}
	return true;
}

bool AudioManager::MoveAudioBankArchive(AudioBankArchive* pArchive, AudioContainer* pDestContainer)
{
	if(!pArchive || !pDestContainer)
		return false;
	AudioContainer* pOldParentContainer = pArchive->GetParentContainer();
	if(pOldParentContainer == pDestContainer)
		return true;
	if(!pDestContainer->addAudioBankArchive(pArchive))
		return false;
	if(!pOldParentContainer->removeAudioBankArchive(pArchive))
	{
		if(!pDestContainer->removeAudioBankArchive(pArchive))
			return false;
		return false;
	}
	return true;
}

bool AudioManager::Load(AFileImage* pFile)
{
	release();

	DWORD dwReadLen = 0;

	unsigned int ver = 0;
	pFile->Read(&ver, sizeof(ver), &dwReadLen);

	int iBankNum = 0;
	pFile->Read(&iBankNum, sizeof(iBankNum), &dwReadLen);
	for (int i=0; i<iBankNum; ++i)
	{
		AudioBank* pAudioBank = new AudioBank;
		if(!pAudioBank->Init(m_pEventProject))
			return false;
		if(!pAudioBank->Load(pFile))
		{
			m_pEventProject->GetEventSystem()->GetLogManager()->GetEngineLog()->Write(LOG_ERROR, "AudioManager::Load ¼ÓÔØbank:%sÊ§°Ü", pAudioBank->GetName());
			delete pAudioBank;
			return false;
		}
		m_listAudioBank.push_back(pAudioBank);
	}
	return true;
}

bool AudioManager::Save(AFile* pFile)
{
	DWORD dwWriteLen = 0;
	pFile->Write((void*)&version, sizeof(version), &dwWriteLen);

	int iBankNum = GetAudioBankNum();
	pFile->Write(&iBankNum, sizeof(iBankNum), &dwWriteLen);
	AudioBankList::const_iterator it = m_listAudioBank.begin();
	for (; it!=m_listAudioBank.end(); ++it)
	{
		AudioBank* pAudioBank = *it;
		if(!pAudioBank)
			return false;
		if(!pAudioBank->Save(pFile))
		{
			m_pEventProject->GetEventSystem()->GetLogManager()->GetEngineLog()->Write(LOG_ERROR, "AudioManager::Save ±£´æbank:%sÊ§°Ü", pAudioBank->GetName());
			return false;
		}
	}
	return true;
}

bool AudioManager::LoadXML(TiXmlElement* root)
{
	release();

	unsigned int ver = 0;
	QueryElement(root, "version", ver);
	int iBankNum = 0;
	QueryElement(root, "bankNum", iBankNum);
	TiXmlNode* pAudioBankNode = root->FirstChild("AudioBank");
	for (int i=0; i<iBankNum; ++i)
	{
		AudioBank* pAudioBank = new AudioBank;
		if(!pAudioBank->Init(m_pEventProject))
			return false;
		if(!pAudioBank->LoadXML(pAudioBankNode->ToElement()))
		{
			m_pEventProject->GetEventSystem()->GetLogManager()->GetEngineLog()->Write(LOG_ERROR, "AudioManager::LoadXML ¼ÓÔØbank:%sÊ§°Ü", pAudioBank->GetName());
			delete pAudioBank;
			return false;
		}
		m_listAudioBank.push_back(pAudioBank);
		pAudioBankNode = pAudioBankNode->NextSibling("AudioBank");
	}
	return true;
}

bool AudioManager::SaveXML(TiXmlElement* pParent)
{
	TiXmlElement* root = new TiXmlElement("AudioManager");
	pParent->LinkEndChild(root);

	AddElement(root, "version", version);
	AddElement(root, "bankNum", GetAudioBankNum());
	AudioBankList::const_iterator it = m_listAudioBank.begin();
	for (; it!=m_listAudioBank.end(); ++it)
	{
		AudioBank* pAudioBank = *it;
		if(!pAudioBank)
			return false;
		if(!pAudioBank->SaveXML(root))
		{
			m_pEventProject->GetEventSystem()->GetLogManager()->GetEngineLog()->Write(LOG_ERROR, "AudioManager::SaveXML ±£´æbank:%sÊ§°Ü", pAudioBank->GetName());
			return false;
		}
	}
	return true;
}