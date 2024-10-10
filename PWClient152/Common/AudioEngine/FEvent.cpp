#include "FEvent.h"
#include "FEventLayer.h"
#include "FEventParameter.h"
#include "FEventSystem.h"
#include "FEventProject.h"
#include "FEventInstance.h"
#include "FLogManager.h"
#include "FEventGroup.h"
#include "FSoundInstanceTemplate.h"
#include "FSoundDef.h"
#include "FAudioGroup.h"
#include "FAudioTemplate.h"
#include "FAudio.h"
#include <fmod.hpp>
#include "xml\xmlcommon.h"
#include <AFileImage.h>

using namespace AudioEngine;

static const unsigned int version = 0x100001;

Event::Event(void)
: m_pParentGroup(0)
, m_pEventProject(0)
, m_bHadCreateInstantces(false)
{
	memset(&m_EventProperty, 0, sizeof(m_EventProperty));
	m_EventProperty.bOneShot = true;
	m_EventProperty.fMinDistance = 1.0f;
	m_EventProperty.fMaxDistance = 100.0f;
	m_EventProperty.mode = MODE_2D;
	m_EventProperty.rolloff = ROLLOFF_LINEAR;
	m_EventProperty.maxPlaybacksBehavior = MPB_STEAL_OLDEST;
	m_EventProperty.uiMaxPlaybacks = 10;
	m_EventProperty.iPriority = 10000;
	m_EventProperty.f3DConeInsideAngle = 360.0f;
	m_EventProperty.f3DConeOutsideAngle = 360.0f;
	m_EventProperty.f3DConeOutsideVolume = 1.0f;
	m_EventProperty.f3DSpeakerSpread = 0.0f;
	m_dwLastPlayTime = 0;

	m_strName.reserve(128);
	m_strNote.reserve(128);
}

Event::~Event(void)
{
	release();
}

bool Event::Init(EventProject* pEventProject)
{
	CriticalSectionController csc(&m_csForThreadSafe);
	release();
	if(!pEventProject)
		return false;
	m_pEventProject = pEventProject;
	if(!AddParameter("default"))
		return false;
	return true;
}

void Event::release()
{
	CriticalSectionController csc(&m_csForThreadSafe);

	EventInstanceList::const_iterator itEventIns = m_listEventInstance.begin();
	for (; itEventIns!=m_listEventInstance.end(); ++itEventIns)
	{
		EventInstance* pEventInstance = *itEventIns;
		if(!pEventInstance)
			continue;
		pEventInstance->RemoveListener(this);
		delete pEventInstance;
	}
	m_listEventInstance.clear();

	EventInstanceList::const_iterator itDelete = m_listDeleteEventInstance.begin();
	for (; itDelete!=m_listDeleteEventInstance.end(); ++itDelete)
	{
		EventInstance* pEventInstance = *itDelete;
		if(!pEventInstance)
			continue;
		pEventInstance->RemoveListener(this);
		delete pEventInstance;
	}
	m_listDeleteEventInstance.clear();


	EventLayerList::const_iterator itLayer = m_listEventLayer.begin();
	for (; itLayer!=m_listEventLayer.end(); ++itLayer)
	{
		EventLayer* pEventLayer = *itLayer;
		if(!pEventLayer)
			continue;
		delete pEventLayer;
	}
	m_listEventLayer.clear();

	EventParameterList::const_iterator itParam = m_listEventParameter.begin();
	for (; itParam!=m_listEventParameter.end(); ++itParam)
	{
		EventParameter* pEventParameter = *itParam;
		if(!pEventParameter)
			continue;
		delete pEventParameter;
	}
	m_listEventParameter.clear();	
}

bool Event::loadData(bool bReload /*= false*/)
{
	CriticalSectionController csc(&m_csForThreadSafe);
	EventLayerList::const_iterator it = m_listEventLayer.begin();
	for (int i=0; it!=m_listEventLayer.end(); ++it, ++i)
	{
		EventLayer* pEventLayer = *it;
		if(!pEventLayer)
			return false;
		if(!pEventLayer->loadData(bReload))
			return false;
	}
	return true;
}

EventLayer* Event::CreateLayer(const char* szName)
{
	EventLayer* pEventLayer = new EventLayer;
	if(!pEventLayer->Init(this, m_pEventProject))
	{
		delete pEventLayer;
		return 0;
	}
	pEventLayer->SetName(szName);	
	m_listEventLayer.push_back(pEventLayer);
	return pEventLayer;
}

EventLayer* Event::GetLayerByIndex(int idx) const
{
	if(idx <0 || idx >= GetLayerNum())
		return 0;
	EventLayerList::const_iterator it = m_listEventLayer.begin();
	for (int i=0; it!=m_listEventLayer.end(); ++it, ++i)
	{
		if(i == idx)
			return *it;
	}
	return 0;
}

EventLayer* Event::GetLayerByName(const char* szName) const
{
	if(szName == 0 || strlen(szName) == 0)
		return 0;
	EventLayerList::const_iterator it = m_listEventLayer.begin();
	for (int i=0; it!=m_listEventLayer.end(); ++it, ++i)
	{
		EventLayer* pEventLayer = *it;
		if(!pEventLayer)
			return false;
		if(strcmp(pEventLayer->GetName(), szName) == 0)
			return pEventLayer;
	}
	return 0;
}

EventParameter* Event::GetParameterByIndex(int idx) const
{
	EventParameterList::const_iterator it = m_listEventParameter.begin();
	for (int i=0; it!=m_listEventParameter.end(); ++it,++i)
	{
		if(i == idx)
			return *it;
	}
	return 0;
}

EventParameter* Event::GetParameterByName(const char* szName) const
{
	EventParameterList::const_iterator it = m_listEventParameter.begin();
	for (; it!=m_listEventParameter.end(); ++it)
	{
		EventParameter* pEventParameter = *it;
		if(!pEventParameter)
			continue;
		if(0 == strcmp(pEventParameter->GetName(), szName))
			return pEventParameter;
	}
	return 0;
}

EventParameter* Event::AddParameter(const char* szName)
{
	EventParameter* pEventParameter = new EventParameter;
	pEventParameter->SetEvent(this);
	pEventParameter->SetName(szName);
	m_listEventParameter.push_back(pEventParameter);
	return pEventParameter;
}

bool Event::IsParameterExist(const char* szName)
{
	EventParameterList::const_iterator it = m_listEventParameter.begin();
	for (; it!=m_listEventParameter.end(); ++it)
	{
		EventParameter* pEventParameter = *it;
		if(!pEventParameter)
			continue;
		if(0 == strcmp(pEventParameter->GetName(), szName))
			return true;
	}
	return false;
}

bool Event::IsParameterUsed(const char* szName)
{
	EventLayerList::const_iterator it = m_listEventLayer.begin();
	for (; it!=m_listEventLayer.end(); ++it)
	{
		EventLayer* pEventLayer = *it;
		if(!pEventLayer)
			continue;
		if(0 == strcmp(pEventLayer->GetParameterName(), szName))
			return true;
	}
	return false;
}

bool Event::DeleteParameter(EventParameter* pEventParameter)
{
	EventParameterList::iterator it = m_listEventParameter.begin();
	for (; it!=m_listEventParameter.end(); ++it)
	{
		if(pEventParameter == *it)
		{
			delete pEventParameter;
			m_listEventParameter.erase(it);
			return true;
		}
	}
	return false;
}

bool Event::DeleteLayer(EventLayer* pEventLayer)
{
	if(!pEventLayer)
		return false;
	EventLayerList::iterator it = m_listEventLayer.begin();
	for (; it!=m_listEventLayer.end(); ++it)
	{
		if(pEventLayer == *it)
		{
			delete pEventLayer;
			m_listEventLayer.erase(it);
			return true;
		}
	}
	return false;
}

bool Event::update(DWORD dwTickTime)
{
	if(!m_bHadCreateInstantces)
		return true;
	CriticalSectionController csc(&m_csForThreadSafe);
	{
	//	A3D_ITTX_SCOPED_TASK_NAMED(AGPAPerfIntegration::GetInstance().GetA3DDomain(), TEXT("Event::update_1"));
		EventInstanceList listEventInstance = m_listEventInstance;

		EventInstanceList::const_iterator it = listEventInstance.begin();
		for (; it!=listEventInstance.end(); ++it)
		{
			EventInstance* pEventInstance = *it;
			if(!pEventInstance->update(dwTickTime))
				return false;
		}
	}

	{
	//	A3D_ITTX_SCOPED_TASK_NAMED(AGPAPerfIntegration::GetInstance().GetA3DDomain(), TEXT("Event::update_2"));
		EventInstanceList::iterator itDelete = m_listDeleteEventInstance.begin();
		for (; itDelete!=m_listDeleteEventInstance.end(); ++itDelete)
		{
			delete *itDelete;
			*itDelete = 0;
		}
		m_listDeleteEventInstance.clear();
	}
	
	return true;
}

EventInstance* Event::CreateInstance(bool bAutoDestroy /*= true*/)
{
	CriticalSectionController csc(&m_csForThreadSafe);
	if(m_listEventInstance.size() == m_EventProperty.uiMaxPlaybacks)
	{
		switch (m_EventProperty.maxPlaybacksBehavior)
		{
		case MPB_STEAL_OLDEST:
			m_listEventInstance.front()->Stop();
			break;
		case MPB_STEAL_NEWEST:
			m_listEventInstance.back()->Stop();
			break;
		case MPB_STEAL_QUIETEST:
			if(m_EventProperty.mode == MODE_2D)
				m_listEventInstance.front()->Stop();
			else
			{
				if(!m_pEventProject)
					return 0;
				if(!m_pEventProject->GetEventSystem())
					return 0;
				FMOD::System* pSystem = m_pEventProject->GetEventSystem()->GetSystem();
				if(!pSystem)
					return 0;		
				FMOD_VECTOR pos,others;
				if(FMOD_OK!=pSystem->get3DListenerAttributes(1, &pos, &others, &others, &others))
					return 0;
				EventInstance* pFurthestInstance = 0;
				float fMaxDistance = 0;
				EventInstanceList::const_iterator it = m_listEventInstance.begin();
				for (; it!=m_listEventInstance.end(); ++it)
				{					
					EventInstance* pEventInstance = *it;
					if(!pEventInstance)
					{
						break;
					}
					VECTOR vPos = pEventInstance->GetPos();
					float fDistance = (pos.x-vPos.x)*(pos.x-vPos.x) + (pos.y-vPos.y)*(pos.y-vPos.y) + (pos.z-vPos.z)*(pos.z-vPos.z);
					if(fDistance > fMaxDistance)
					{
						fMaxDistance = fDistance;
						pFurthestInstance = pEventInstance;
					}					
				}
				pFurthestInstance->Stop();
			}
			break;
		case MPB_JUST_FAIL:
			return 0;
		}
	}
	EventInstance* pEventInstance = new EventInstance;
	if(!pEventInstance->Init(this, m_pEventProject, bAutoDestroy))
	{
		delete pEventInstance;
		return 0;
	}	
	m_listEventInstance.push_back(pEventInstance);
	m_bHadCreateInstantces = true;
	return pEventInstance;
}

bool Event::OnStop(EventInstance* pEventInstance)
{
	CriticalSectionController csc(&m_csForThreadSafe);
	if(pEventInstance)
		pEventInstance->RemoveListener(this);
	if(pEventInstance->IsAutoDestroy())
	{
		if(!DestroyInstance(pEventInstance))
			return false;
	}
	return true;
}

bool Event::DestroyInstance(EventInstance*& pEventInstance)
{
	CriticalSectionController csc(&m_csForThreadSafe);
	if(pEventInstance)
		pEventInstance->RemoveListener(this);
	EventInstanceList::iterator it = m_listEventInstance.begin();
	for (; it!=m_listEventInstance.end(); ++it)
	{
		if(pEventInstance == *it)
		{			
			m_listDeleteEventInstance.push_back(*it);
			m_listEventInstance.erase(it);
			pEventInstance = 0;
			break;
		}
	}
	return true;
}

EventInstance* Event::GetEventInstanceByIndex(int idx) const
{
	EventInstanceList::const_iterator it=m_listEventInstance.begin();
	for (int i=0; it!=m_listEventInstance.end(); ++it, ++i)
	{
		if(i == idx)
			return *it;
	}
	return 0;
}

bool Event::Reload()
{
	return loadData(true);
}

EventProject* Event::GetEventProject() const
{
	return m_pEventProject;
}

const char* Event::GetFullPath()
{
	m_strFullPath = m_pParentGroup->GetName();
	m_strFullPath += "\\";
	m_strFullPath += GetName();
	EventGroup* pEventGroup = m_pParentGroup;
	while(pEventGroup)
	{
		pEventGroup = pEventGroup->GetParentGroup();
		if(!pEventGroup)
			break;
		m_strFullPath = "\\" + m_strFullPath;
		m_strFullPath = pEventGroup->GetName() + m_strFullPath;
	}
	m_strFullPath = "\\" + m_strFullPath;
	m_strFullPath = m_pEventProject->GetName() + m_strFullPath;
	return m_strFullPath.c_str();
}

const char* Event::GetGuidString()
{
	return m_Guid.GetString();
}

bool Event::Load(AFileImage* pFile)
{
	release();

	DWORD dwReadLen = 0;
	unsigned int ver = 0;
	pFile->Read(&ver, sizeof(ver), &dwReadLen);

	pFile->Read(&m_Guid.guid, sizeof(m_Guid.guid), &dwReadLen);

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

	pFile->Read(&m_EventProperty, sizeof(m_EventProperty), &dwReadLen);

	int iParamNum = 0;
	pFile->Read(&iParamNum, sizeof(iParamNum), &dwReadLen);
	for (int i=0; i<iParamNum; ++i)
	{
		EventParameter* pEventParameter = new EventParameter;
		if(!pEventParameter->Load(pFile))
		{
			m_pEventProject->GetEventSystem()->GetLogManager()->GetEngineLog()->Write(LOG_ERROR, "Event::Load ¼ÓÔØEventParameter:%sÊ§°Ü", pEventParameter->GetName());
			delete pEventParameter;
			return false;
		}
		pEventParameter->SetEvent(this);
		m_listEventParameter.push_back(pEventParameter);
	}

	int iLayerNum = 0;
	pFile->Read(&iLayerNum, sizeof(iLayerNum), &dwReadLen);
	for (int i=0; i<iLayerNum; ++i)
	{
		EventLayer* pEventLayer = new EventLayer;
		if(!pEventLayer->Init(this, m_pEventProject))
		{
			delete pEventLayer;
			return false;
		}		
		if(!pEventLayer->Load(pFile))
		{
			m_pEventProject->GetEventSystem()->GetLogManager()->GetEngineLog()->Write(LOG_ERROR, "Event::Load ¼ÓÔØEventLayer:%sÊ§°Ü", pEventLayer->GetName());
			delete pEventLayer;
			return false;
		}
		m_listEventLayer.push_back(pEventLayer);
	}
	return true;
}

bool Event::Save(AFile* pFile)
{
	DWORD dwWriteLen = 0;
	pFile->Write((void*)&version, sizeof(version), &dwWriteLen);

	pFile->Write(&m_Guid.guid, sizeof(m_Guid.guid), &dwWriteLen);

	int iNameLen = (int)m_strName.size();
	pFile->Write(&iNameLen, sizeof(iNameLen), &dwWriteLen);
	pFile->Write((void*)m_strName.c_str(), iNameLen, &dwWriteLen);

	int iNoteLen = (int)m_strNote.size();
	pFile->Write(&iNoteLen, sizeof(iNoteLen), &dwWriteLen);
	pFile->Write((void*)m_strNote.c_str(), iNoteLen, &dwWriteLen);

	pFile->Write(&m_EventProperty, sizeof(m_EventProperty), &dwWriteLen);

	int iParamNum = GetParameterNum();
	pFile->Write(&iParamNum, sizeof(iParamNum), &dwWriteLen);
	EventParameterList::const_iterator itParam = m_listEventParameter.begin();
	for (; itParam!=m_listEventParameter.end(); ++itParam)
	{
		EventParameter* pEventParameter = *itParam;
		if (!pEventParameter)
			return false;
		if (!pEventParameter->Save(pFile))
		{
			m_pEventProject->GetEventSystem()->GetLogManager()->GetEngineLog()->Write(LOG_ERROR, "Event::Save ±£´æEventParameter:%sÊ§°Ü", pEventParameter->GetName());
			return false;
		}
	}

	int iLayerNum = GetLayerNum();
	pFile->Write(&iLayerNum, sizeof(iLayerNum), &dwWriteLen);
	EventLayerList::const_iterator itLayer = m_listEventLayer.begin();
	for (; itLayer!=m_listEventLayer.end(); ++itLayer)
	{
		EventLayer* pEventLayer = *itLayer;
		if(!pEventLayer)
			return false;
		if(!pEventLayer->Save(pFile))
		{
			m_pEventProject->GetEventSystem()->GetLogManager()->GetEngineLog()->Write(LOG_ERROR, "Event::Save ±£´æEventLayer:%sÊ§°Ü", pEventLayer->GetName());
			return false;
		}
	}
	
	return true;
}

bool Event::LoadXML(TiXmlElement* root, bool bPreset /*= false*/)
{
	release();

	unsigned int ver = 0;
	QueryElement(root, "version", ver);
	if(!bPreset)
	{
		QueryElement(root, "GUID", m_Guid.guid);
		QueryElement(root, "name", m_strName);
	}
	QueryElement(root, "note", m_strNote);

	TiXmlNode* pNode = root->FirstChild("EventProperty");
	if (!pNode)
		return false;

	TiXmlElement* pEventPropNode = pNode->ToElement();

	QueryElement(pEventPropNode, "ifOneShot", m_EventProperty.bOneShot);
	QueryElement(pEventPropNode, "volume", m_EventProperty.fVolume);
	QueryElement(pEventPropNode, "volumeRanomization", m_EventProperty.fVolumeRandomization);
	QueryElement(pEventPropNode, "pitch", m_EventProperty.fPitch);	
	QueryElement(pEventPropNode, "pitchRandomization", m_EventProperty.fPitchRandomization);
	QueryElement(pEventPropNode, "fadeInTime", m_EventProperty.iFadeInTime);
	QueryElement(pEventPropNode, "fadeInCurveType", m_EventProperty.iFadeInCurveType);
	QueryElement(pEventPropNode, "fadeOutTime", m_EventProperty.iFadeOutTime);
	QueryElement(pEventPropNode, "fadeOutCurveType", m_EventProperty.iFadeOutCurveType);
	int iMode = 0;
	QueryElement(pEventPropNode, "mode", iMode);
	m_EventProperty.mode = (EVENT_MODE)iMode;
	int iRollOff = 0;
	QueryElement(pEventPropNode, "rolloff", iRollOff);
	m_EventProperty.rolloff = (EVENT_3D_ROLLOFF)iRollOff;
	QueryElement(pEventPropNode, "minDistance", m_EventProperty.fMinDistance);
	QueryElement(pEventPropNode, "maxDistance", m_EventProperty.fMaxDistance);
	QueryElement(pEventPropNode, "ConeInsideAngle", m_EventProperty.f3DConeInsideAngle);
	QueryElement(pEventPropNode, "ConeOutsideAngle", m_EventProperty.f3DConeOutsideAngle);
	QueryElement(pEventPropNode, "ConeOutsideVolume", m_EventProperty.f3DConeOutsideVolume);
	QueryElement(pEventPropNode, "SpeakerSpread", m_EventProperty.f3DSpeakerSpread);
	QueryElement(pEventPropNode, "maxPlaybacks", m_EventProperty.uiMaxPlaybacks);
	int iMaxPlaybacksBehavior = 0;
	QueryElement(pEventPropNode, "maxPlaybacesBehavior", iMaxPlaybacksBehavior);
	m_EventProperty.maxPlaybacksBehavior = (MAX_PLAYBACK_BEHAVIOR)(iMaxPlaybacksBehavior);
	QueryElement(pEventPropNode, "priority", m_EventProperty.iPriority);

	int iParamNum = 0;
	QueryElement(root, "ParamNum", iParamNum);
	TiXmlNode* pEventParameterNode = root->FirstChild("EventParameter");
	for (int i=0; i<iParamNum; ++i)
	{
		EventParameter* pEventParameter = new EventParameter;
		if(!pEventParameter->LoadXML(pEventParameterNode->ToElement()))
		{
			m_pEventProject->GetEventSystem()->GetLogManager()->GetEngineLog()->Write(LOG_ERROR, "Event::LoadXML ¼ÓÔØEventParameter:%sÊ§°Ü", pEventParameter->GetName());
			delete pEventParameter;
			return false;
		}
		pEventParameter->SetEvent(this);
		m_listEventParameter.push_back(pEventParameter);
		pEventParameterNode = pEventParameterNode->NextSibling("EventParameter");
	}

	int iLayerNum = 0;
	QueryElement(root, "LayerNum", iLayerNum);
	TiXmlNode* pEventLayerNode = root->FirstChild("EventLayer");
	for (int i=0; i<iLayerNum; ++i)
	{
		EventLayer* pEventLayer = new EventLayer;
		if(!pEventLayer->Init(this, m_pEventProject))
		{
			delete pEventLayer;
			return false;
		}
		if(!pEventLayer->LoadXML(pEventLayerNode->ToElement(), bPreset))
		{
			m_pEventProject->GetEventSystem()->GetLogManager()->GetEngineLog()->Write(LOG_ERROR, "Event::LoadXML ¼ÓÔØEventLayer:%sÊ§°Ü", pEventLayer->GetName());
			delete pEventLayer;
			return false;
		}
		m_listEventLayer.push_back(pEventLayer);
		pEventLayerNode = pEventLayerNode->NextSibling("EventLayer");
	}

	return true;
}

bool Event::SaveXML(TiXmlElement* pParent, bool bPreset /*= false*/)
{
	TiXmlElement* root = new TiXmlElement("Event");
	pParent->LinkEndChild(root);

	AddElement(root, "version", version);
	if(!bPreset)
	{
		AddElement(root, "GUID", m_Guid.guid);
		AddElement(root, "name", m_strName);
	}	
	AddElement(root, "note", m_strNote);

	TiXmlElement* pEventPropNode = new TiXmlElement("EventProperty");
	root->LinkEndChild(pEventPropNode);

	AddElement(pEventPropNode, "ifOneShot", m_EventProperty.bOneShot);
	AddElement(pEventPropNode, "volume", m_EventProperty.fVolume);
	AddElement(pEventPropNode, "volumeRanomization", m_EventProperty.fVolumeRandomization);
	AddElement(pEventPropNode, "pitch", m_EventProperty.fPitch);
	AddElement(pEventPropNode, "pitchRandomization", m_EventProperty.fPitchRandomization);
	AddElement(pEventPropNode, "fadeInTime", m_EventProperty.iFadeInTime);
	AddElement(pEventPropNode, "fadeInCurveType", m_EventProperty.iFadeInCurveType);
	AddElement(pEventPropNode, "fadeOutTime", m_EventProperty.iFadeOutTime);
	AddElement(pEventPropNode, "fadeOutCurveType", m_EventProperty.iFadeOutCurveType);
	AddElement(pEventPropNode, "mode", m_EventProperty.mode);
	AddElement(pEventPropNode, "rolloff", m_EventProperty.rolloff);
	AddElement(pEventPropNode, "minDistance", m_EventProperty.fMinDistance);
	AddElement(pEventPropNode, "maxDistance", m_EventProperty.fMaxDistance);
	AddElement(pEventPropNode, "ConeInsideAngle", m_EventProperty.f3DConeInsideAngle);
	AddElement(pEventPropNode, "ConeOutsideAngle", m_EventProperty.f3DConeOutsideAngle);
	AddElement(pEventPropNode, "ConeOutsideVolume", m_EventProperty.f3DConeOutsideVolume);
	AddElement(pEventPropNode, "SpeakerSpread", m_EventProperty.f3DSpeakerSpread);
	AddElement(pEventPropNode, "maxPlaybacks", m_EventProperty.uiMaxPlaybacks);
	AddElement(pEventPropNode, "maxPlaybacesBehavior", m_EventProperty.maxPlaybacksBehavior);
	AddElement(pEventPropNode, "priority", m_EventProperty.iPriority);

	AddElement(root, "ParamNum", GetParameterNum());
	EventParameterList::const_iterator itParam = m_listEventParameter.begin();
	for (; itParam!=m_listEventParameter.end(); ++itParam)
	{
		EventParameter* pEventParameter = *itParam;
		if (!pEventParameter)
			return false;
		if (!pEventParameter->SaveXML(root))
		{
			m_pEventProject->GetEventSystem()->GetLogManager()->GetEngineLog()->Write(LOG_ERROR, "Event::SaveXML ±£´æEventParameter:%sÊ§°Ü", pEventParameter->GetName());
			return false;
		}
	}

	AddElement(root, "LayerNum", GetLayerNum());
	EventLayerList::const_iterator itLayer = m_listEventLayer.begin();
	for (; itLayer!=m_listEventLayer.end(); ++itLayer)
	{
		EventLayer* pEventLayer = *itLayer;
		if(!pEventLayer)
			return false;		
		if(!pEventLayer->SaveXML(root, bPreset))
		{
			m_pEventProject->GetEventSystem()->GetLogManager()->GetEngineLog()->Write(LOG_ERROR, "Event::SaveXML ±£´æEventLayer:%sÊ§°Ü", pEventLayer->GetName());
			return false;
		}
	}

	return true;
}

Event::PathSet Event::GetRelatedAudioFiles() const
{
	PathSet setPath;
	EventLayerList::const_iterator itLayer = m_listEventLayer.begin();
	for (; itLayer!=m_listEventLayer.end(); ++itLayer)
	{
		EventLayer* pEventLayer = *itLayer;
		if(!pEventLayer)
			continue;
		int iSoundInsTemplateNum = pEventLayer->GetSoundInstanceTemplateNum();
		for (int i=0; i<iSoundInsTemplateNum; ++i)
		{
			SoundInstanceTemplate* pSoundInsTemplate = pEventLayer->GetSoundInstanceTemplateByIndex(i);
			if(!pSoundInsTemplate)
				continue;
			SoundDef* pSoundDef = pSoundInsTemplate->GetSoundDef();
			int iAudioGroupNum = pSoundDef->GetAudioGroupNum();
			for (int j=0; j<iAudioGroupNum; ++j)
			{
				AudioGroup* pAudioGroup = pSoundDef->GetAudioGroupByIndex(j);
				if(!pAudioGroup)
					continue;
				int iAudioTemplateNum=pAudioGroup->GetAudioTemplateNum();
				for (int k=0; k<iAudioTemplateNum; ++k)
				{
					AudioTemplate* pAudioTemplate = pAudioGroup->GetAudioTemplateByIndex(k);
					if(!pAudioTemplate)
						continue;
					setPath.insert(pAudioTemplate->GetAudio()->GetPath());
				}
			}
		}
	}
	return setPath;
}