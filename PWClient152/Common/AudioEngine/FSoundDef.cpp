#include <fmod.hpp>

#include "FSoundDef.h"
#include "FAudio.h"
#include "FAudioManager.h"
#include "FEventProject.h"
#include "xml\xmlcommon.h"
#include "FAudioGroup.h"
#include <AFileImage.h>

using namespace AudioEngine;

static const unsigned int version = 0x100001;

SoundDef::SoundDef(void)
: m_pEventProject(0)
, m_pParentArchive(0)
{
	memset(&m_SoundDefProperty, 0, sizeof(m_SoundDefProperty));

	m_strName.reserve(128);
	m_strNote.reserve(128);
}

SoundDef::~SoundDef(void)
{
	release();
}

bool SoundDef::Init(EventProject* pEventProject)
{
	release();

	if(!pEventProject)
		return false;
	m_pEventProject = pEventProject;
	return true;
}

void SoundDef::release()
{
	AudioGroupList::iterator it = m_listAudioGroup.begin();
	for (; it!=m_listAudioGroup.end(); ++it)
	{
		delete *it;		
	}
	m_listAudioGroup.clear();
}

AudioGroup* SoundDef::GetAudioGroupByIndex(int idx) const
{
	if(idx <0 || idx >= GetAudioGroupNum())
		return 0;
	AudioGroupList::const_iterator it = m_listAudioGroup.begin();
	for (int i=0; it!=m_listAudioGroup.end(); ++it, ++i)
	{
		if(i == idx)
			return *it;
	}
	return 0;
}

AudioGroup* SoundDef::AddAudioGroup()
{
	AudioGroup* pAudioGroup = new AudioGroup;
	if(!pAudioGroup->Init(m_pEventProject))
	{
		delete pAudioGroup;
		return 0;
	}
	m_listAudioGroup.push_back(pAudioGroup);
	return pAudioGroup;
}

bool SoundDef::RemoveAudioGroup(AudioGroup* pAudioGroup)
{
	AudioGroupList::iterator it = m_listAudioGroup.begin();
	for (; it!=m_listAudioGroup.end(); ++it)
	{
		if(pAudioGroup == *it)
		{
			m_listAudioGroup.erase(it);
			return true;
		}
	}
	return false;
}

bool SoundDef::MoveUpAudioGroup(AudioGroup* pAudioGroup)
{
	if(!pAudioGroup)
		return false;

	for(size_t i = 1; i < m_listAudioGroup.size(); ++i)
	{
		if(m_listAudioGroup[i] == pAudioGroup)
		{
			m_listAudioGroup[i] = m_listAudioGroup[i - 1];
			m_listAudioGroup[i - 1] = pAudioGroup;
			return true;
		}
	}
	return false;
}

bool SoundDef::MoveDownAudioGroup(AudioGroup* pAudioGroup)
{
	if(!pAudioGroup)
		return false;

	for(size_t j = 1; j < m_listAudioGroup.size(); ++j)
	{
		size_t i = m_listAudioGroup.size() - 1 - j;
		if(m_listAudioGroup[i] == pAudioGroup)
		{
			m_listAudioGroup[i] = m_listAudioGroup[i + 1];
			m_listAudioGroup[i + 1] = pAudioGroup;
			return true;
		}
	}
	return false;
}

bool SoundDef::Load(AFileImage* pFile)
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

	pFile->Read(&m_SoundDefProperty, sizeof(m_SoundDefProperty), &dwReadLen);

	pFile->Read(&m_Guid.guid, sizeof(m_Guid.guid), &dwReadLen);
	int iAudioGroupNum = 0;
	pFile->Read(&iAudioGroupNum, sizeof(iAudioGroupNum), &dwReadLen);

	Guid guid;
	for (int i=0; i<iAudioGroupNum; ++i)
	{
		AudioGroup* pAudioGroup = AddAudioGroup();
		if(!pAudioGroup->Load(pFile))
		{
			delete pAudioGroup;
			return false;
		}
	}
	return true;
}

bool SoundDef::Save(AFile* pFile)
{
	DWORD dwWriteLen = 0;

	pFile->Write((void*)&version, sizeof(version), &dwWriteLen);

	int iNameLen = (int)m_strName.size();
	pFile->Write(&iNameLen, sizeof(iNameLen), &dwWriteLen);
	pFile->Write((void*)m_strName.c_str(), iNameLen, &dwWriteLen);

	int iNoteLen = (int)m_strNote.size();
	pFile->Write(&iNoteLen, sizeof(iNoteLen), &dwWriteLen);
	pFile->Write((void*)m_strNote.c_str(), iNoteLen, &dwWriteLen);

	pFile->Write(&m_SoundDefProperty, sizeof(m_SoundDefProperty), &dwWriteLen);

	pFile->Write(&m_Guid.guid, sizeof(m_Guid.guid), &dwWriteLen);
	int iAudioGroupNum = GetAudioGroupNum();
	pFile->Write(&iAudioGroupNum, sizeof(iAudioGroupNum), &dwWriteLen);
	AudioGroupList::const_iterator it = m_listAudioGroup.begin();
	Guid guid;
	for (; it!=m_listAudioGroup.end(); ++it)
	{
		AudioGroup* pAudioGroup = *it;
		if(!pAudioGroup)
			return false;
		if(!pAudioGroup->Save(pFile))
			return false;
	}
	return true;
}

bool SoundDef::LoadXML(TiXmlElement* root, bool bPreset /*= false*/)
{
	release();

	unsigned int ver = 0;
	QueryElement(root, "version", ver);
	if(!bPreset)
	{
		QueryElement(root, "name", m_strName);		
	}
	QueryElement(root, "note", m_strNote);

	TiXmlNode* pNode = root->FirstChild("SoundDefProperty");
	if (!pNode)
		return false;
	TiXmlElement* pSoundDefPropNode = pNode->ToElement();
	int iPlayMode = 0;
	QueryElement(pSoundDefPropNode, "playMode", iPlayMode);
	m_SoundDefProperty.playMode = (PLAY_MODE)iPlayMode;
	QueryElement(pSoundDefPropNode, "volume", m_SoundDefProperty.fVolume);
	QueryElement(pSoundDefPropNode, "volumeRandomization", m_SoundDefProperty.fVolumeRandomization);
	QueryElement(pSoundDefPropNode, "pitch", m_SoundDefProperty.fPitch);
	QueryElement(pSoundDefPropNode, "pitchRandomization", m_SoundDefProperty.fPitchRandomization);
	QueryElement(pSoundDefPropNode, "intervalTime", m_SoundDefProperty.iIntervalTime);
	QueryElement(pSoundDefPropNode, "intervalTimeRandomization", m_SoundDefProperty.iIntervalTimeRandomization);


	if(!bPreset)
	{
		QueryElement(root, "GUID", m_Guid.guid);

		AudioManager* pAudioManager = m_pEventProject->GetAudioManager();
		if(!pAudioManager)
			return false;

		int iAudioGroupNum = 0;
		QueryElement(root, "AudioGroupNum", iAudioGroupNum);
		TiXmlNode* pAudioGroupNode = root->FirstChild("AudioGroup");
		Guid guid;
		for (int i=0; i<iAudioGroupNum; ++i)
		{
			AudioGroup* pAudioGroup = AddAudioGroup();
			if(!pAudioGroup->LoadXML(pAudioGroupNode->ToElement()))
			{
				delete pAudioGroup;
				return false;
			}
			pAudioGroupNode = pAudioGroupNode->NextSibling("AudioGroup");
		}
	}	
	
	return true;
}

bool SoundDef::SaveXML(TiXmlElement* pParent, bool bPreset /*= false*/)
{
	TiXmlElement* root = new TiXmlElement("SoundDef");
	pParent->LinkEndChild(root);

	AddElement(root, "version", version);
	if(!bPreset)
	{
		AddElement(root, "name", m_strName);		
	}
	AddElement(root, "note", m_strNote);

	TiXmlElement* pSoundDefPropNode = new TiXmlElement("SoundDefProperty");
	root->LinkEndChild(pSoundDefPropNode);
	AddElement(pSoundDefPropNode, "playMode", m_SoundDefProperty.playMode);
	AddElement(pSoundDefPropNode, "volume", m_SoundDefProperty.fVolume);
	AddElement(pSoundDefPropNode, "volumeRandomization", m_SoundDefProperty.fVolumeRandomization);
	AddElement(pSoundDefPropNode, "pitch", m_SoundDefProperty.fPitch);
	AddElement(pSoundDefPropNode, "pitchRandomization", m_SoundDefProperty.fPitchRandomization);
	AddElement(pSoundDefPropNode, "intervalTime", m_SoundDefProperty.iIntervalTime);
	AddElement(pSoundDefPropNode, "intervalTimeRandomization", m_SoundDefProperty.iIntervalTimeRandomization);

	if(!bPreset)
	{
		AddElement(root, "GUID", m_Guid.guid);

		AddElement(root, "AudioGroupNum", GetAudioGroupNum());

		AudioGroupList::const_iterator it = m_listAudioGroup.begin();
		Guid guid;
		for (; it!=m_listAudioGroup.end(); ++it)
		{		
			AudioGroup* pAudioGroup = *it;
			if(!pAudioGroup)
				return false;
			if(!pAudioGroup->SaveXML(root))
				return false;
		}
	}	
	return true;
}