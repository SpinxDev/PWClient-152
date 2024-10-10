#include "FEventLayer.h"
#include "FEffect.h"
#include "FEffectFactory.h"
#include "FEventParameter.h"
#include "FEventSystem.h"
#include "FEvent.h"
#include "FGuid.h"
#include "FSoundDef.h"
#include "FEventProject.h"
#include "FSoundDefManager.h"
#include "FSoundInstanceTemplate.h"
#include "FLogManager.h"
#include "xml\xmlcommon.h"
#include <AFileImage.h>

using namespace AudioEngine;

static const unsigned int version = 0x100001;

EventLayer::EventLayer(void)
: m_pEvent(0)
, m_pEventParameter(0)
, m_pEventProject(0)
{
}

EventLayer::~EventLayer(void)
{
	release();
}

bool EventLayer::Init(Event* pEvent, EventProject* pEventProject)
{
	if(!pEventProject || !pEvent)
		return false;
	m_pEventProject = pEventProject;
	m_pEvent = pEvent;
	return true;
}

void EventLayer::release()
{
	SoundInstanceTemplateList::const_iterator it = m_listSoundInstanceTemplate.begin();
	for (; it!=m_listSoundInstanceTemplate.end(); ++it)
	{
		SoundInstanceTemplate* pSoundInstanceTemplate = *it;
		if(!pSoundInstanceTemplate)
			continue;
		delete pSoundInstanceTemplate;
	}
	m_listSoundInstanceTemplate.clear();

	if(!m_pEventProject)
		return;
	EventSystem* pEventSystem = m_pEventProject->GetEventSystem();
	if(!pEventSystem)
		return;
	EffectFactory* pEffectFactory = pEventSystem->GetEffectFactory();
	if(!pEffectFactory)
		return;

	EffectList::const_iterator itEffect = m_listEffect.begin();
	for (; itEffect!=m_listEffect.end(); ++itEffect)
	{
		if(!pEffectFactory->DestroyEffect(*itEffect))
			continue;
	}
	m_listEffect.clear();
}

bool EventLayer::loadData(bool bReload /*= false*/)
{
	SoundInstanceTemplateList::const_iterator it = m_listSoundInstanceTemplate.begin();
	for (; it!=m_listSoundInstanceTemplate.end(); ++it)
	{
		SoundInstanceTemplate* pSoundInstanceTemplate = *it;
		if(!pSoundInstanceTemplate)
			continue;
		if(!pSoundInstanceTemplate->loadData(bReload))
			return false;
	}
	return true;
}

SoundInstanceTemplate* EventLayer::AddSoundInstanceTemplate(SoundDef* pSoundDef)
{
	if(!pSoundDef)
		return 0;
	SoundInstanceTemplate* pSoundInstanceTemplate = new SoundInstanceTemplate;
	if(!pSoundInstanceTemplate->Init(m_pEventProject))
	{
		delete pSoundInstanceTemplate;
		return 0;
	}
	pSoundInstanceTemplate->setEventLayer(this);
	m_listSoundInstanceTemplate.push_back(pSoundInstanceTemplate);
	if(!pSoundInstanceTemplate->SetSoundDef(pSoundDef))
	{
		delete pSoundInstanceTemplate;
		return 0;
	}
	return pSoundInstanceTemplate;
}

bool EventLayer::DeleteSoundInstanceTemplate(SoundInstanceTemplate* pSoundInstanceTemplate)
{
	SoundInstanceTemplateList::iterator it = m_listSoundInstanceTemplate.begin();
	for (; it!=m_listSoundInstanceTemplate.end(); ++it)
	{
		if(pSoundInstanceTemplate == *it)
		{
			delete pSoundInstanceTemplate;
			m_listSoundInstanceTemplate.erase(it);
			return true;
		}
	}
	return false;
}

bool EventLayer::HasEffectType(EFFECT_TYPE type)
{
	EffectList::const_iterator it = m_listEffect.begin();
	for (; it != m_listEffect.end(); ++it)
	{
		Effect* pEffect = *it;
		if(!pEffect)
			continue;
		if(pEffect->GetType() == type)
			return true;
	}
	return false;
}

Effect* EventLayer::AddEffect(EFFECT_TYPE type)
{
	if(!m_pEventProject)
		return 0;
	EventSystem* pEventSystem = m_pEventProject->GetEventSystem();
	if(!pEventSystem)
		return 0;
	EffectFactory* pEffectFactory = pEventSystem->GetEffectFactory();
	if(!pEffectFactory)
		return 0;
	Effect* pEffect = pEffectFactory->CreateEffect(type);
	if(!pEffect)
		return 0;	
	m_listEffect.push_back(pEffect);
	return pEffect;
}

bool EventLayer::DeleteEffect(Effect* pEffect)
{
	if(!pEffect)
		return false;
	if(!m_pEventProject)
		return false;
	EventSystem* pEventSystem = m_pEventProject->GetEventSystem();
	if(!pEventSystem)
		return false;
	EffectFactory* pEffectFactory = pEventSystem->GetEffectFactory();
	if(!pEffectFactory)
		return false;

	EffectList::iterator it = m_listEffect.begin();
	for (; it!=m_listEffect.end(); ++it)
	{
		if(pEffect == *it)
		{
			if(!pEffectFactory->DestroyEffect(pEffect))
				return false;
			m_listEffect.erase(it);
			return true;
		}
	}
	return false;
}

Effect* EventLayer::GetEffectByIndex(int idx) const
{
	if(idx < 0 || idx >= (int)m_listEffect.size())
		return 0;
	EffectList::const_iterator it = m_listEffect.begin();
	for (int i=0; it!=m_listEffect.end(); ++it,++i)
	{
		if(i == idx)
			return *it;
	}
	return 0;
}

bool EventLayer::AttachParameter(const char* szName)
{
	if(!m_pEvent)
		return false;
	m_pEventParameter = m_pEvent->GetParameterByName(szName);
	if(!m_pEventParameter)
		return false;
	return true;
}

const char* EventLayer::GetParameterName() const
{
	return m_pEventParameter->GetName();
}

SoundInstanceTemplate* EventLayer::GetSoundInstanceTemplateByIndex(int idx) const
{
	SoundInstanceTemplateList::const_iterator it = m_listSoundInstanceTemplate.begin();
	for (int i=0; it!=m_listSoundInstanceTemplate.end(); ++it,++i)
	{
		if(i == idx)
			return *it;
	}
	return 0;
}

bool EventLayer::Load(AFileImage* pFile)
{
	release();
	if(!Init(m_pEvent, m_pEventProject))
		return false;

	DWORD dwReadLen = 0;
	unsigned int ver = 0;
	pFile->Read(&ver, sizeof(ver), &dwReadLen);

	int iNameLen = 0;
	pFile->Read(&iNameLen, sizeof(iNameLen), &dwReadLen);
	char szName[1024] = {0};
	pFile->Read(szName, iNameLen, &dwReadLen);
	m_strName = szName;

	int iParamNameLen = 0;
	pFile->Read(&iParamNameLen, sizeof(iParamNameLen), &dwReadLen);
	char szParamName[1024] = {0};
	pFile->Read(szParamName, iParamNameLen, &dwReadLen);
	if(!AttachParameter(szParamName))
		return false;

	int iSoundInstanceNum = 0;
	pFile->Read(&iSoundInstanceNum, sizeof(iSoundInstanceNum), &dwReadLen);
	for (int i=0; i<iSoundInstanceNum; ++i)
	{
		Guid guid;
		pFile->Read(&guid.guid, sizeof(guid.guid), &dwReadLen);
		SoundDefManager* pSoundDefManager = m_pEventProject->GetSoundDefManager();
		if(!pSoundDefManager)
			return false;
		SoundDef* pSoundDef = pSoundDefManager->GetSoundDef(guid);
		if(!pSoundDef)
			return false;
		
		SoundInstanceTemplate* pSoundInstanceTemplate = AddSoundInstanceTemplate(pSoundDef);
		if(!pSoundInstanceTemplate)
			return false;
		if(!pSoundInstanceTemplate->Load(pFile))
		{
			m_pEventProject->GetEventSystem()->GetLogManager()->GetEngineLog()->Write(LOG_ERROR, "EventLayer::Load ¼ÓÔØSoundInstanceTemplate:%sÊ§°Ü", pSoundInstanceTemplate->GetSoundDef()->GetName());
			delete pSoundInstanceTemplate;
			return false;
		}
	}

	int iEffectNum = 0;
	pFile->Read(&iEffectNum, sizeof(iEffectNum), &dwReadLen);
	for (int i=0; i<iEffectNum; ++i)
	{
		EFFECT_TYPE effectType;
		pFile->Read(&effectType, sizeof(effectType), &dwReadLen);
		Effect* pEffect = AddEffect(effectType);
		if(!pEffect)
			return false;
		if(!pEffect->Load(pFile))
		{
			m_pEventProject->GetEventSystem()->GetLogManager()->GetEngineLog()->Write(LOG_ERROR, "EventLayer::Load ¼ÓÔØEffect:%sÊ§°Ü", pEffect->GetName());
			return false;
		}
	}
	return true;
}

bool EventLayer::Save(AFile* pFile)
{
	DWORD dwWriteLen = 0;
	pFile->Write((void*)&version, sizeof(version), &dwWriteLen);

	int iNameLen = (int)m_strName.size();
	pFile->Write(&iNameLen, sizeof(iNameLen), &dwWriteLen);
	pFile->Write((void*)m_strName.c_str(), iNameLen, &dwWriteLen);

	const char* szParamName = GetParameterName();
	int iParamNameLen = (int)strlen(szParamName);
	pFile->Write(&iParamNameLen, sizeof(iParamNameLen), &dwWriteLen);
	pFile->Write((void*)szParamName, iParamNameLen, &dwWriteLen);

	int iSoundInstanceTemplateNum = GetSoundInstanceTemplateNum();
	pFile->Write(&iSoundInstanceTemplateNum, sizeof(iSoundInstanceTemplateNum), &dwWriteLen);
	SoundInstanceTemplateList::const_iterator itSoundIns = m_listSoundInstanceTemplate.begin();
	for (; itSoundIns!=m_listSoundInstanceTemplate.end(); ++itSoundIns)
	{
		SoundInstanceTemplate* pSoundInstanceTemplate = *itSoundIns;
		if(!pSoundInstanceTemplate)
			return false;
		SoundDef* pSoundDef = pSoundInstanceTemplate->GetSoundDef();
		if(!pSoundDef)
			return false;
		Guid guid = pSoundDef->GetGuid();
		pFile->Write(&guid.guid, sizeof(guid.guid), &dwWriteLen);
		if(!pSoundInstanceTemplate->Save(pFile))
		{
			m_pEventProject->GetEventSystem()->GetLogManager()->GetEngineLog()->Write(LOG_ERROR, "EventLayer::Save ±£´æSoundInstanceTemplate:%sÊ§°Ü", pSoundInstanceTemplate->GetSoundDef()->GetName());
			return false;
		}
	}

	int iEffectNum = GetEffectNum();
	pFile->Write(&iEffectNum, sizeof(iEffectNum), &dwWriteLen);
	EffectList::const_iterator itEffect = m_listEffect.begin();
	for (; itEffect!=m_listEffect.end(); ++itEffect)
	{
		Effect* pEffect = *itEffect;
		if(!pEffect)
			return false;
		EFFECT_TYPE effectType = pEffect->GetType();
		pFile->Write(&effectType, sizeof(effectType), &dwWriteLen);
		if(!pEffect->Save(pFile))
		{
			m_pEventProject->GetEventSystem()->GetLogManager()->GetEngineLog()->Write(LOG_ERROR, "EventLayer::Save ±£´æEffect:%sÊ§°Ü", pEffect->GetName());
			return false;
		}
	}
	return true;
}

bool EventLayer::LoadXML(TiXmlElement* root, bool bPreset /*= false*/)
{
	release();
	if(!Init(m_pEvent, m_pEventProject))
		return false;

	unsigned int ver = 0;
	QueryElement(root, "version", ver);
	QueryElement(root, "name", m_strName);
	std::string strParameterName;
	QueryElement(root, "paramName", strParameterName);
	AttachParameter(strParameterName.c_str());

	if(!bPreset)
	{
		int iSoundInstanceNum = 0;
		QueryElement(root, "soundInstanceTemplateNum", iSoundInstanceNum);
		TiXmlNode* pSoundInstanceTemplateNode = root->FirstChild("SoundInstanceTemplate");
		TiXmlNode* pGUIDNode = root->FirstChild("GUID");
		for (int i=0; i<iSoundInstanceNum; ++i)
		{
			Guid guid;
			std::string str;
			unsigned int uData4;
			unsigned int uData5;
			unsigned int uData6;
			unsigned int uData7;
			pGUIDNode->ToElement()->QueryStringAttribute("value", &str);
			sscanf_s(str.c_str(), "{%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x}", &guid.guid.Data1,&guid.guid.Data2,&guid.guid.Data3,
				&guid.guid.Data4[0],
				&guid.guid.Data4[1],
				&guid.guid.Data4[2],
				&guid.guid.Data4[3],
				&uData4,
				&uData5,
				&uData6,
				&uData7);
			guid.guid.Data4[4] = (char)uData4;
			guid.guid.Data4[5] = (char)uData5;
			guid.guid.Data4[6] = (char)uData6;
			guid.guid.Data4[7] = (char)uData7;		

			SoundDefManager* pSoundDefManager = m_pEventProject->GetSoundDefManager();
			if(!pSoundDefManager)
				return false;
			SoundDef* pSoundDef = pSoundDefManager->GetSoundDef(guid);
			if(!pSoundDef)
				return false;

			SoundInstanceTemplate* pSoundInstanceTemplate = AddSoundInstanceTemplate(pSoundDef);
			if(!pSoundInstanceTemplate)
				return false;
			if(!pSoundInstanceTemplate->LoadXML(pSoundInstanceTemplateNode->ToElement()))
			{
				m_pEventProject->GetEventSystem()->GetLogManager()->GetEngineLog()->Write(LOG_ERROR, "EventLayer::LoadXML ¼ÓÔØSoundInstanceTemplate:%sÊ§°Ü", pSoundInstanceTemplate->GetSoundDef()->GetName());
				delete pSoundInstanceTemplate;
				return false;
			}
			pSoundInstanceTemplateNode = pSoundInstanceTemplateNode->NextSibling("SoundInstanceTemplate");
			pGUIDNode = pGUIDNode->NextSibling("GUID");
		}
	}	

	int iEffectNum = 0;
	QueryElement(root, "EffectNum", iEffectNum);
	TiXmlNode* pEffectNode = root->FirstChild("Effect");
	TiXmlNode* pEffectTypeNode = root->FirstChild("EffectType");
	for (int i=0; i<iEffectNum; ++i)
	{		
		int iEffectType = 0;
		pEffectTypeNode->ToElement()->QueryIntAttribute("value", &iEffectType);
		EFFECT_TYPE effectType = (EFFECT_TYPE)iEffectType;
		Effect* pEffect = AddEffect(effectType);
		if(!pEffect)
			return false;
		if(!pEffect->LoadXML(pEffectNode->ToElement()))
		{
			m_pEventProject->GetEventSystem()->GetLogManager()->GetEngineLog()->Write(LOG_ERROR, "EventLayer::LoadXML ¼ÓÔØEffect:%sÊ§°Ü", pEffect->GetName());
			return false;
		}
		pEffectNode = pEffectNode->NextSibling("Effect");
		pEffectTypeNode = pEffectTypeNode->NextSibling("EffectType");
	}
	return true;
}

bool EventLayer::SaveXML(TiXmlElement* pParent, bool bPreset /*= false*/)
{
	TiXmlElement* root = new TiXmlElement("EventLayer");
	pParent->LinkEndChild(root);

	AddElement(root, "version", version);
	AddElement(root, "name", m_strName);
	AddElement(root, "paramName", (std::string)GetParameterName());
	if(!bPreset)
	{
		AddElement(root, "soundInstanceTemplateNum", GetSoundInstanceTemplateNum());
		SoundInstanceTemplateList::const_iterator itSoundIns = m_listSoundInstanceTemplate.begin();
		for (; itSoundIns!=m_listSoundInstanceTemplate.end(); ++itSoundIns)
		{
			SoundInstanceTemplate* pSoundInstanceTemplate = *itSoundIns;
			if(!pSoundInstanceTemplate)
				return false;
			SoundDef* pSoundDef = pSoundInstanceTemplate->GetSoundDef();
			if(!pSoundDef)
				return false;
			Guid guid = pSoundDef->GetGuid();
			AddElement(root, "GUID", guid.guid);
			if(!pSoundInstanceTemplate->SaveXML(root))
			{
				m_pEventProject->GetEventSystem()->GetLogManager()->GetEngineLog()->Write(LOG_ERROR, "EventLayer::SaveXML ±£´æSoundInstanceTemplate:%sÊ§°Ü", pSoundInstanceTemplate->GetSoundDef()->GetName());
				return false;
			}
		}
	}

	AddElement(root, "EffectNum", GetEffectNum());
	EffectList::const_iterator itEffect = m_listEffect.begin();
	for (; itEffect!=m_listEffect.end(); ++itEffect)
	{
		Effect* pEffect = *itEffect;
		if(!pEffect)
			return false;
		AddElement(root, "EffectType", (int)pEffect->GetType());
		if(!pEffect->SaveXML(root))
		{
			m_pEventProject->GetEventSystem()->GetLogManager()->GetEngineLog()->Write(LOG_ERROR, "EventLayer::SaveXML ±£´æEffect:%sÊ§°Ü", pEffect->GetName());
			return false;
		}
	}
	return true;
}