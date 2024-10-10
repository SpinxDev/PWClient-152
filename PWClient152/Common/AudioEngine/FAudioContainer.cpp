#include "FAudioContainer.h"
#include "FAudio.h"
#include "FAudioBankArchive.h"
#include "FEventProject.h"
#include "FAudioManager.h"
#include "xml/xmlcommon.h"

using namespace AudioEngine;

AudioContainer::AudioContainer(void)
: m_pEventProject(0)
, m_pRootAudioBank(0)
, m_pParentContainer(0)
{
	m_strName.reserve(128);
	m_strNote.reserve(128);
}

AudioContainer::~AudioContainer(void)
{
	release();
}

bool AudioContainer::Init(EventProject* pEventProject)
{
	if(!pEventProject)
		return false;
	m_pEventProject = pEventProject;
	return true;
}

void AudioContainer::release()
{
	AudioList::const_iterator it = m_listAudio.begin();
	for (; it!=m_listAudio.end(); ++it)
	{
		delete *it;
	}
	m_listAudio.clear();

	AudioBankArchiveList::const_iterator it2 = m_listAudioBankArchive.begin();
	for (; it2!=m_listAudioBankArchive.end(); ++it2)
	{
		delete *it2;
	}
	m_listAudioBankArchive.clear();
}

Audio* AudioContainer::GetAudioByIndex(int idx) const
{
	if(idx <0 || idx >= GetAudioNum())
		return 0;
	AudioList::const_iterator it = m_listAudio.begin();
	for (int i=0; it!=m_listAudio.end(); ++it, ++i)
	{
		if(i == idx)
			return *it;
	}
	return 0;
}

AudioBankArchive* AudioContainer::GetAudioBankArchiveByIndex(int idx) const
{
	if(idx <0 || idx >= GetAudioBankArchiveNum())
		return 0;
	AudioBankArchiveList::const_iterator it = m_listAudioBankArchive.begin();
	for (int i=0; it!=m_listAudioBankArchive.end(); ++it, ++i)
	{
		if(i == idx)
			return *it;
	}
	return 0;
}

Audio* AudioContainer::createAudio(const char* szName, const char* szPath)
{
	if(!szName || strlen(szName) == 0)
		return 0;
	Audio* pAudio = new Audio;
	if(!pAudio->Init(m_pEventProject))
	{
		delete pAudio;
		return 0;
	}
	pAudio->SetName(szName);
	pAudio->SetPath(szPath);
	pAudio->SetAudioBank(m_pRootAudioBank);
	pAudio->SetGuid(GuidGen::GetInstance().Generate());
	pAudio->SetParentContainer(this);
	m_listAudio.push_back(pAudio);
	return pAudio;
}

bool AudioContainer::deleteAudio(const char* szName)
{
	AudioList::iterator it = m_listAudio.begin();
	for (; it!=m_listAudio.end(); ++it)
	{
		Audio* pAudio = *it;
		if(!pAudio)
			return false;
		if(GF_CompareNoCase(szName, strlen(szName), pAudio->GetName(), strlen(pAudio->GetName())))
		{
			delete pAudio;
			m_listAudio.erase(it);
			return true;
		}
	}
	return false;
}

AudioBankArchive* AudioContainer::createAudioBankArchive(const char* szName)
{
	if(!szName || strlen(szName) == 0)
		return 0;
	AudioBankArchive* pAudioBankArchive = new AudioBankArchive;
	if(!pAudioBankArchive->Init(m_pEventProject))
	{
		delete pAudioBankArchive;
		return 0;
	}
	pAudioBankArchive->SetName(szName);
	pAudioBankArchive->SetParentContainer(this);
	pAudioBankArchive->SetAudioBank(m_pRootAudioBank);
	m_listAudioBankArchive.push_back(pAudioBankArchive);
	return pAudioBankArchive;
}

bool AudioContainer::deleteAudioBankArchive(const char* szName)
{
	AudioBankArchiveList::iterator it = m_listAudioBankArchive.begin();
	for (; it!=m_listAudioBankArchive.end(); ++it)
	{
		AudioBankArchive* pAudioBankArchive = *it;
		if(!pAudioBankArchive)
			return false;
		if(GF_CompareNoCase(szName, strlen(szName), pAudioBankArchive->GetName(), strlen(pAudioBankArchive->GetName())))
		{
			while(pAudioBankArchive->GetAudioNum())
			{
				Audio* pAudio = pAudioBankArchive->GetAudioByIndex(0);
				if(!pAudio)
					return false;
				if(!m_pEventProject->GetAudioManager()->DeleteAudio(pAudio))
					return false;
			}

			while(pAudioBankArchive->GetAudioBankArchiveNum())
			{
				AudioBankArchive* pChildArchive = pAudioBankArchive->GetAudioBankArchiveByIndex(0);
				if(!pChildArchive)
					return false;
				if(!pAudioBankArchive->deleteAudioBankArchive(pChildArchive->GetName()))
					return false;
			}

			delete pAudioBankArchive;
			m_listAudioBankArchive.erase(it);
			return true;
		}
	}
	return false;
}

bool AudioContainer::addAudio(Audio* pAudio)
{
	if(!pAudio)
		return false;
	AudioList::const_iterator it = m_listAudio.begin();
	for (; it!=m_listAudio.end(); ++it)
	{		
		if(pAudio == *it)
			return false;
	}
	if(m_pEventProject->GetAudioManager()->IsAudioExist(this, pAudio->GetName()))
		return false;
	pAudio->SetAudioBank(m_pRootAudioBank);
	pAudio->SetParentContainer(this);
	m_listAudio.push_back(pAudio);
	return true;
}

bool AudioContainer::removeAudio(Audio* pAudio)
{
	if(!pAudio)
		return false;
	AudioList::iterator it = m_listAudio.begin();
	for (; it!=m_listAudio.end(); ++it)
	{		
		if(pAudio == *it)
		{
			m_listAudio.erase(it);
			return true;
		}
	}
	return false;
}

bool AudioContainer::addAudioBankArchive(AudioBankArchive* pAudioBankArchive)
{
	if(!pAudioBankArchive)
		return false;
	AudioBankArchiveList::const_iterator it = m_listAudioBankArchive.begin();
	for (; it!=m_listAudioBankArchive.end(); ++it)
	{
		if(pAudioBankArchive == *it)
			return false;
	}
	if(m_pEventProject->GetAudioManager()->IsArchiveExist(this, pAudioBankArchive->GetName()))
		return false;
	pAudioBankArchive->SetAudioBank(m_pRootAudioBank);
	pAudioBankArchive->SetParentContainer(this);
	m_listAudioBankArchive.push_back(pAudioBankArchive);
	return true;
}

bool AudioContainer::removeAudioBankArchive(AudioBankArchive* pAudioBankArchive)
{
	if(!pAudioBankArchive)
		return false;
	AudioBankArchiveList::iterator it = m_listAudioBankArchive.begin();
	for (; it!=m_listAudioBankArchive.end(); ++it)
	{
		if(pAudioBankArchive == *it)
		{
			m_listAudioBankArchive.erase(it);
			return true;
		}
	}
	return false;
}