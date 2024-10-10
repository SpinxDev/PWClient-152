#include "FSoundInstanceTemplate.h"
#include <fmod.hpp>
#include "FAudioInstance.h"
#include "FEventProject.h"
#include "FSoundDef.h"
#include "xml/xmlcommon.h"
#include "FAudioGroup.h"
#include "FAudioTemplate.h"
#include "FAudioInstanceManager.h"
#include <AFileImage.h>

using namespace AudioEngine;

static const unsigned int version = 0x100001;

SoundInstanceTemplate::SoundInstanceTemplate(void)
: m_bLoaded(false)
, m_pEventLayer(0)
, m_pEventProject(0)
{
	memset(&m_SoundInstanceProperty, 0, sizeof(m_SoundInstanceProperty));
	m_SoundInstanceProperty.fStartPosition = 0.0f;
	m_SoundInstanceProperty.fLength = 1.0f;
	m_SoundInstanceProperty.iLoopCount = 1;

	m_strNote.reserve(128);
}

SoundInstanceTemplate::~SoundInstanceTemplate(void)
{
	release();
}

bool SoundInstanceTemplate::Init(EventProject* pEventProject)
{
	if(!pEventProject)
		return false;
	m_pEventProject = pEventProject;	
	return true;
}

bool SoundInstanceTemplate::SetSoundDef(SoundDef* pSoundDef)
{
	if(m_pSoundDef == pSoundDef)
		return true;
	m_pSoundDef = pSoundDef;	
	return true;
}

AudioInstance* SoundInstanceTemplate::GetAudioInstanceByIndex(int idx, bool bPlayForEvent, bool bReload)
{
	CriticalSectionController csc(&m_csForThreadSafe);
	if(idx <0 || idx >= GetAudioInstanceNum())
		return 0;
	AudioInstancePairMap::iterator it = m_mapAudioInstancePair.find(idx);
	if (it!=m_mapAudioInstancePair.end())
	{
		AudioInstance* pAudioInstance = it->second.second;
		if(!pAudioInstance)
		{
			if(bReload)
			{
				pAudioInstance = AudioInstanceManager::GetInstance().GetAudioInstance(it->second.first, bPlayForEvent, bReload);
				if(pAudioInstance)
					pAudioInstance->AddListener(this);
				it->second.second = pAudioInstance;
			}
		}
		return it->second.second;
	}
	return 0;
}

bool SoundInstanceTemplate::loadData(bool bReload /*= false*/)
{
	CriticalSectionController csc(&m_csForThreadSafe);
	clearData();
	if(!m_pSoundDef)
		return false;	

	EventSystem* pEventSystem = m_pEventProject->GetEventSystem();
	if(!pEventSystem)
		return false;
	int idx = 0;
	int iAudioGroupNum = m_pSoundDef->GetAudioGroupNum();
	for (int i=0; i<iAudioGroupNum; ++i)
	{
		AudioGroup* pAudioGroup = m_pSoundDef->GetAudioGroupByIndex(i);
		if(!pAudioGroup)
			continue;
		int iAudioTemplateNum = pAudioGroup->GetAudioTemplateNum();
		for (int j=0; j<iAudioTemplateNum; ++j)
		{
			AudioInstance* pAudioInstance = AudioInstanceManager::GetInstance().GetAudioInstance(pAudioGroup->GetAudioTemplateByIndex(j), m_bPlayForEvent, bReload);
			if(pAudioInstance)
				pAudioInstance->AddListener(this);
			m_mapAudioInstancePair[idx++] = std::make_pair(pAudioGroup->GetAudioTemplateByIndex(j), pAudioInstance);
		}
	}

	return true;
}

void SoundInstanceTemplate::clearData()
{
	CriticalSectionController csc(&m_csForThreadSafe);
	m_mapAudioInstancePair.clear();
}

void SoundInstanceTemplate::release()
{
	CriticalSectionController csc(&m_csForThreadSafe);
	clearData();
}

bool SoundInstanceTemplate::Load(AFileImage* pFile)
{
	DWORD dwReadLen = 0;

	unsigned int ver = 0;
	pFile->Read(&ver, sizeof(ver), &dwReadLen);

	int iNoteLen = 0;
	pFile->Read(&iNoteLen, sizeof(iNoteLen), &dwReadLen);
	char szNote[1024] = {0};
	pFile->Read(szNote, iNoteLen, &dwReadLen);
	m_strNote = szNote;

	pFile->Read(&m_SoundInstanceProperty, sizeof(m_SoundInstanceProperty), &dwReadLen);
	return true;
}

bool SoundInstanceTemplate::Save(AFile* pFile)
{
	DWORD dwWriteLen = 0;

	pFile->Write((void*)&version, sizeof(version), &dwWriteLen);

	int iNoteLen = (int)m_strNote.size();
	pFile->Write(&iNoteLen, sizeof(iNoteLen), &dwWriteLen);
	pFile->Write((void*)m_strNote.c_str(), iNoteLen, &dwWriteLen);

	pFile->Write(&m_SoundInstanceProperty, sizeof(m_SoundInstanceProperty), &dwWriteLen);
	return true;
}

bool SoundInstanceTemplate::LoadXML(TiXmlElement* root, bool bPreset /*= false*/)
{
	unsigned int ver = 0;
	QueryElement(root, "version", ver);

	QueryElement(root, "note", m_strNote);

	TiXmlNode* pNode = root->FirstChild("SoundInstanceProperty");
	if(!pNode)
		return false;
	TiXmlElement* pSoundInstancePropertyNode = pNode->ToElement();
	QueryElement(pSoundInstancePropertyNode, "startPosition", m_SoundInstanceProperty.fStartPosition);
	QueryElement(pSoundInstancePropertyNode, "length", m_SoundInstanceProperty.fLength);
	int iStartMode = 0;
	QueryElement(pSoundInstancePropertyNode, "startMode", iStartMode);
	m_SoundInstanceProperty.startMode = (START_MODE)iStartMode;
	int iStopMode = 0;
	QueryElement(pSoundInstancePropertyNode, "stopMode", iStopMode);
	m_SoundInstanceProperty.stopMode = (STOP_MODE)iStopMode;
	QueryElement(pSoundInstancePropertyNode, "loopCount", m_SoundInstanceProperty.iLoopCount);
	int iPlayNum = 0;
	QueryElement(pSoundInstancePropertyNode, "playNum", iPlayNum);
	m_SoundInstanceProperty.playNum = (PLAY_NUM)iPlayNum;
	QueryElement(pSoundInstancePropertyNode, "fadeInTime", m_SoundInstanceProperty.iFadeInTime);
	QueryElement(pSoundInstancePropertyNode, "fadeInCurveType", m_SoundInstanceProperty.iFadeInCurveType);
	QueryElement(pSoundInstancePropertyNode, "fadeOutTime", m_SoundInstanceProperty.iFadeOutTime);
	QueryElement(pSoundInstancePropertyNode, "fadeOutCurveType", m_SoundInstanceProperty.iFadeOutCurveType);
	QueryElement(pSoundInstancePropertyNode, "intervalTime", m_SoundInstanceProperty.iIntervalTime);
	QueryElement(pSoundInstancePropertyNode, "intervalTimeRandomization", m_SoundInstanceProperty.iIntervalTimeRandomization);
	QueryElement(pSoundInstancePropertyNode, "preIntervalTime", m_SoundInstanceProperty.iPreIntervalTime);
	QueryElement(pSoundInstancePropertyNode, "preIntervalTimeRandomization", m_SoundInstanceProperty.iPreIntervalTimeRandomization);
	return true;
}

bool SoundInstanceTemplate::SaveXML(TiXmlElement* pParent, bool bPreset /*= false*/)
{
	TiXmlElement* root = new TiXmlElement("SoundInstanceTemplate");
	pParent->LinkEndChild(root);

	AddElement(root, "version", version);
	AddElement(root, "note", m_strNote);

	TiXmlElement* pSoundInstancePropertyNode = new TiXmlElement("SoundInstanceProperty");
	root->LinkEndChild(pSoundInstancePropertyNode);

	AddElement(pSoundInstancePropertyNode, "startPosition", m_SoundInstanceProperty.fStartPosition);
	AddElement(pSoundInstancePropertyNode, "length", m_SoundInstanceProperty.fLength);
	AddElement(pSoundInstancePropertyNode, "startMode", m_SoundInstanceProperty.startMode);
	AddElement(pSoundInstancePropertyNode, "stopMode", m_SoundInstanceProperty.stopMode);
	AddElement(pSoundInstancePropertyNode, "loopCount", m_SoundInstanceProperty.iLoopCount);
	AddElement(pSoundInstancePropertyNode, "playNum", (int)m_SoundInstanceProperty.playNum);
	AddElement(pSoundInstancePropertyNode, "fadeInTime", m_SoundInstanceProperty.iFadeInTime);
	AddElement(pSoundInstancePropertyNode, "fadeInCurveType", m_SoundInstanceProperty.iFadeInCurveType);
	AddElement(pSoundInstancePropertyNode, "fadeOutTime", m_SoundInstanceProperty.iFadeOutTime);
	AddElement(pSoundInstancePropertyNode, "fadeOutCurveType", m_SoundInstanceProperty.iFadeOutCurveType);
	AddElement(pSoundInstancePropertyNode, "intervalTime", m_SoundInstanceProperty.iIntervalTime);
	AddElement(pSoundInstancePropertyNode, "intervalTimeRandomization", m_SoundInstanceProperty.iIntervalTimeRandomization);
	AddElement(pSoundInstancePropertyNode, "preIntervalTime", m_SoundInstanceProperty.iPreIntervalTime);
	AddElement(pSoundInstancePropertyNode, "preIntervalTimeRandomization", m_SoundInstanceProperty.iPreIntervalTimeRandomization);
	return true;
}

bool SoundInstanceTemplate::OnDelete(AudioInstance* pAudioInstance)
{
	CriticalSectionController csc(&m_csForThreadSafe);
	if(!pAudioInstance)
		return false;
	pAudioInstance->RemoveListener(this);
	AudioInstancePairMap::iterator it = m_mapAudioInstancePair.begin();
	for (; it!=m_mapAudioInstancePair.end(); ++it)
	{
		if(pAudioInstance == it->second.second)
		{
			it->second.second = 0;
		}
	}
	return true;
}