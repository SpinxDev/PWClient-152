#include "FAudioGroup.h"
#include "FAudioTemplate.h"
#include "xml/xmlcommon.h"
#include <AFileImage.h>

using namespace AudioEngine;

static const unsigned int version = 0x100001;

AudioGroup::AudioGroup(void)
: m_pEventProject(0)
, m_bSingle(true)
, m_iPlayMode(AGPM_SEQUENTIAL)
{
}

AudioGroup::~AudioGroup(void)
{
	release();
}

bool AudioGroup::Init(EventProject* pEventProject)
{
	release();

	if(!pEventProject)
		return false;
	m_pEventProject = pEventProject;
	return true;
}

void AudioGroup::release()
{
	AudioTemplateList::const_iterator it = m_listAudioTemplate.begin();
	for (; it!=m_listAudioTemplate.end(); ++it)
	{
		delete *it;
	}
	m_listAudioTemplate.clear();
}

AudioTemplate* AudioGroup::AddAudioTemplate(Audio* pAudio)
{
	AudioTemplate* pAudioTemplate = new AudioTemplate;
	pAudioTemplate->SetAudio(pAudio);
	pAudioTemplate->SetParentGroup(this);
	m_listAudioTemplate.push_back(pAudioTemplate);
	return pAudioTemplate;
}

bool AudioGroup::DeleteAudioTemplate(AudioTemplate* pAudioTemplate)
{
	AudioTemplateList::iterator it = m_listAudioTemplate.begin();
	for (; it!=m_listAudioTemplate.end(); ++it)
	{
		if(*it == pAudioTemplate)
		{
			delete pAudioTemplate;
			m_listAudioTemplate.erase(it);
			return true;
		}
	}
	return false;
}

bool AudioGroup::MoveUpAudioTemplate(AudioTemplate* pAudioTemplate)
{
	if(!pAudioTemplate)
		return false;
	AudioTemplateList::iterator it = m_listAudioTemplate.begin();
	for (; it!=m_listAudioTemplate.end(); ++it)
	{		
		if(pAudioTemplate == *it)
		{
			if(it == m_listAudioTemplate.begin())
				return true;
			AudioTemplateList::iterator lastit = --it;
			++it;
			m_listAudioTemplate.erase(it);
			m_listAudioTemplate.insert(lastit, pAudioTemplate);
			return true;
		}
	}
	return false;
}

bool AudioGroup::MoveDownAudioTemplate(AudioTemplate* pAudioTemplate)
{
	if(!pAudioTemplate)
		return false;
	AudioTemplateList::iterator it = m_listAudioTemplate.begin();
	for (; it!=m_listAudioTemplate.end(); ++it)
	{
		if(pAudioTemplate == *it)
		{			
			if(it == --m_listAudioTemplate.end())
				return true;
			m_listAudioTemplate.insert(++++it, pAudioTemplate);
			m_listAudioTemplate.erase(------it);
			return true;
		}
	}
	return false;
}

int AudioGroup::GetAudioTemplateNum() const
{
	return (int)m_listAudioTemplate.size();
}

AudioTemplate* AudioGroup::GetAudioTemplateByIndex(int idx) const
{
	AudioTemplateList::const_iterator it = m_listAudioTemplate.begin();
	for (int i=0; it!=m_listAudioTemplate.end(); ++it, ++i)
	{
		if(idx == i)
		{
			return *it;
		}
	}
	return 0;
}

bool AudioGroup::Load(AFileImage* pFile)
{
	release();

	DWORD dwReadLen = 0;

	unsigned int ver = 0;
	pFile->Read(&ver, sizeof(ver), &dwReadLen);
	pFile->Read(&m_bSingle, sizeof(m_bSingle), &dwReadLen);
	int iAudioTemplateNum = 0;
	pFile->Read(&iAudioTemplateNum, sizeof(iAudioTemplateNum), &dwReadLen);
	pFile->Read(&m_iPlayMode, sizeof(m_iPlayMode), &dwReadLen);
	for (int i=0; i<iAudioTemplateNum; ++i)
	{
		AudioTemplate* pAudioTemplate = new AudioTemplate;
		if(!pAudioTemplate->Init(m_pEventProject))
		{
			delete pAudioTemplate;
			return false;
		}
		if(!pAudioTemplate->Load(pFile))
		{
			delete pAudioTemplate;
			return false;
		}
		pAudioTemplate->SetParentGroup(this);
		m_listAudioTemplate.push_back(pAudioTemplate);
	}
	return true;
}

bool AudioGroup::Save(AFile* pFile)
{
	DWORD dwWriteLen = 0;

	pFile->Write((void*)&version, sizeof(version), &dwWriteLen);
	pFile->Write(&m_bSingle, sizeof(m_bSingle), &dwWriteLen);
	int iAudioTemplateNum = GetAudioTemplateNum();
	pFile->Write(&iAudioTemplateNum, sizeof(iAudioTemplateNum), &dwWriteLen);
	pFile->Write(&m_iPlayMode, sizeof(m_iPlayMode), &dwWriteLen);
	AudioTemplateList::const_iterator it = m_listAudioTemplate.begin();
	for (; it!=m_listAudioTemplate.end(); ++it)
	{
		AudioTemplate* pAudioTemplate = *it;
		if(!pAudioTemplate->Save(pFile))
			return false;
	}
	return true;
}

bool AudioGroup::LoadXML(TiXmlElement* root)
{
	release();

	unsigned int ver = 0;
	QueryElement(root, "version", ver);
	QueryElement(root, "single", m_bSingle);
	int iAudioTemplateNum = 0;
	QueryElement(root, "AudioTemplateNum", iAudioTemplateNum);
	int iLoopMode;
	QueryElement(root, "LoopMode", iLoopMode);
	m_iPlayMode = static_cast<AUDIO_GROUP_PLAY_MODE>(iLoopMode);
	TiXmlNode* pAudioTemplateNode = root->FirstChild("AudioTemplate");
	for (int i=0; i<iAudioTemplateNum; ++i)
	{
		AudioTemplate* pAudioTemplate = new AudioTemplate;
		if(!pAudioTemplate->Init(m_pEventProject))
		{
			delete pAudioTemplate;
			return false;
		}
		if(!pAudioTemplate->LoadXML(pAudioTemplateNode->ToElement()))
		{
			delete pAudioTemplate;
			return false;
		}
		pAudioTemplate->SetParentGroup(this);
		m_listAudioTemplate.push_back(pAudioTemplate);
		pAudioTemplateNode = pAudioTemplateNode->NextSibling("AudioTemplate");
	}
	return true;
}

bool AudioGroup::SaveXML(TiXmlElement* pParent)
{
	TiXmlElement* root = new TiXmlElement("AudioGroup");
	pParent->LinkEndChild(root);

	AddElement(root, "version", version);
	AddElement(root, "single", m_bSingle);
	AddElement(root, "AudioTemplateNum", GetAudioTemplateNum());
	AddElement(root, "LoopMode", m_iPlayMode);
	AudioTemplateList::const_iterator it = m_listAudioTemplate.begin();
	for (; it!=m_listAudioTemplate.end(); ++it)
	{
		AudioTemplate* pAudioTemplate = *it;
		if(!pAudioTemplate->SaveXML(root))
			return false;
	}
	return true;
}