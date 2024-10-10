#include "FEventProject.h"
#include "FEventManager.h"
#include "FSoundDefManager.h"
#include "FAudioManager.h"
#include "FEvent.h"
#include "FLogManager.h"
#include "xml\xmlcommon.h"
#include "FEventSystem.h"
#include "FEventGroup.h"
#include <AMemory.h>
#include <AFileImage.h>

using namespace AudioEngine;

static const unsigned int version = 0x100001;

EventProject::EventProject(void)
: m_pEventSystem(0)
, m_pEventManager(0)
, m_pSoundDefManager(0)
, m_pAudioManager(0)
{
}

EventProject::~EventProject(void)
{
	release();
}

void EventProject::release()
{
	delete m_pEventManager;
	m_pEventManager = 0;

	delete m_pSoundDefManager;
	m_pSoundDefManager = 0;

	delete m_pAudioManager;
	m_pAudioManager = 0;
}

bool EventProject::Init(EventSystem* pEventSystem)
{
	if(!pEventSystem)
		return false;
	m_pEventSystem = pEventSystem;
	m_pEventManager = new EventManager;
	if(!m_pEventManager->Init(this))
	{
		delete m_pEventManager;
		return false;
	}

	m_pSoundDefManager = new SoundDefManager;
	if(!m_pSoundDefManager->Init(this))
	{
		delete m_pSoundDefManager;
		return false;
	}

	m_pAudioManager = new AudioManager;
	if(!m_pAudioManager->Init(this))
	{
		delete m_pAudioManager;
		return false;
	}
	return true;
}

bool EventProject::Load(const char* szPath)
{
	release();
	if(!Init(m_pEventSystem))
		return false;

	AFileImage file;
	if(!file.Open(szPath, AFILE_OPENEXIST|AFILE_BINARY|AFILE_TEMPMEMORY))
	{
		m_pEventSystem->GetLogManager()->GetEngineLog()->Write(LOG_ERROR, "EventProject::Load 无法打开文件%s", szPath);
		return false;
	}
	DWORD dwReadLen = 0;
	unsigned int ver = 0;
	file.Read(&ver, sizeof(ver), &dwReadLen);
	int iNameLen = 0;
	file.Read(&iNameLen, sizeof(iNameLen), &dwReadLen);
	char szName[1024] = {0};
	file.Read(szName, iNameLen, &dwReadLen);
	m_strName = szName;

	if(!m_pAudioManager->Load(&file))
	{
		m_pEventSystem->GetLogManager()->GetEngineLog()->Write(LOG_ERROR, "EventProject::Load 加载AudioManager失败");
		file.Close();
		return false;
	}
	if(!m_pSoundDefManager->Load(&file))
	{
		m_pEventSystem->GetLogManager()->GetEngineLog()->Write(LOG_ERROR, "EventProject::Load 加载SoundDefManager失败");
		file.Close();
		return false;
	}
	if(!m_pEventManager->Load(&file))
	{
		m_pEventSystem->GetLogManager()->GetEngineLog()->Write(LOG_ERROR, "EventProject::Load 加载EventManager失败");
		file.Close();
		return false;
	}
	file.Close();
	return true;
}

bool EventProject::Save(const char* szPath)
{
	AFile file;
	if(!file.Open(szPath, AFILE_CREATENEW | AFILE_BINARY))
	{
		m_pEventSystem->GetLogManager()->GetEngineLog()->Write(LOG_ERROR, "EventProject::Load 无法打开文件%s", szPath);
		return false;
	}
	DWORD dwWriteLen = 0;
	file.Write((void*)&version, sizeof(version), &dwWriteLen);

	int iNameLen = (int)m_strName.size();
	file.Write(&iNameLen, sizeof(iNameLen), &dwWriteLen);
	file.Write((void*)m_strName.c_str(), iNameLen, &dwWriteLen);

	if(!m_pAudioManager->Save(&file))
	{
		m_pEventSystem->GetLogManager()->GetEngineLog()->Write(LOG_ERROR, "EventProject::Save 保存AudioManager失败");
		file.Close();
		return false;
	}
	if(!m_pSoundDefManager->Save(&file))
	{
		m_pEventSystem->GetLogManager()->GetEngineLog()->Write(LOG_ERROR, "EventProject::Save 保存SoundDefManager失败");
		file.Close();
		return false;
	}
	if(!m_pEventManager->Save(&file))
	{
		m_pEventSystem->GetLogManager()->GetEngineLog()->Write(LOG_ERROR, "EventProject::Save 保存EventManager失败");
		file.Close();
		return false;
	}
	file.Close();
	return true;
}

bool EventProject::LoadXML(const char* szPath)
{
	release();
	if(!Init(m_pEventSystem))
		return false;
	AFileImage file;
	if(!file.Open(szPath, AFILE_OPENEXIST|AFILE_TEXT|AFILE_TEMPMEMORY))
	{
		m_pEventSystem->GetLogManager()->GetEngineLog()->Write(LOG_ERROR, "EventProject::Load 无法打开文件%s", szPath);
		return false;
	}

	DWORD dwLength = file.GetFileLength();
	char* pBuf = (char *)a_malloctemp(dwLength + 1);
	memset(pBuf, 0, dwLength + 1);
	DWORD dwRead;
	file.Read(pBuf, dwLength, &dwRead);
	file.Close();
	pBuf[dwLength] = 0;

	TiXmlDocument doc;
	doc.Parse(pBuf);
	a_freetemp(pBuf);

	TiXmlNode* pNode = doc.FirstChild("EventProject");
	if (!pNode)
		return false;

	unsigned int ver = 0;

	TiXmlElement* root = pNode->ToElement();
	QueryElement(root, "version", ver);
	QueryElement(root, "name", m_strName);
	
	if(!m_pAudioManager->LoadXML(root->FirstChild("AudioManager")->ToElement()))
	{
		m_pEventSystem->GetLogManager()->GetEngineLog()->Write(LOG_ERROR, "EventProject::LoadXML 加载AudioManager失败");
		return false;
	}
	if(!m_pSoundDefManager->LoadXML(root->FirstChild("SoundDefManager")->ToElement()))
	{
		m_pEventSystem->GetLogManager()->GetEngineLog()->Write(LOG_ERROR, "EventProject::LoadXML 加载SoundDefManager失败");
		return false;
	}
	if(!m_pEventManager->LoadXML(root->FirstChild("EventManager")->ToElement()))
	{
		m_pEventSystem->GetLogManager()->GetEngineLog()->Write(LOG_ERROR, "EventProject::LoadXML 加载EventManager失败");
		return false;
	}


	return true;
}

bool EventProject::SaveXML(const char* szPath)
{
	TiXmlDocument doc;
	TiXmlElement* root = new TiXmlElement("EventProject");
	doc.LinkEndChild(root);

	AddElement(root, "version", version);
	AddElement(root, "name", m_strName);
	

	if(!m_pAudioManager->SaveXML(root))
	{
		m_pEventSystem->GetLogManager()->GetEngineLog()->Write(LOG_ERROR, "EventProject::SaveXML 保存AudioManager失败");
		return false;
	}
	
	if(!m_pSoundDefManager->SaveXML(root))
	{
		m_pEventSystem->GetLogManager()->GetEngineLog()->Write(LOG_ERROR, "EventProject::SaveXML 保存SoundDefManager失败");
		return false;
	}
	if(!m_pEventManager->SaveXML(root))
	{
		m_pEventSystem->GetLogManager()->GetEngineLog()->Write(LOG_ERROR, "EventProject::SaveXML 保存EventManager失败");
		return false;
	}

	TiXmlPrinter printer;
	doc.Accept(&printer);

	FILE* pFile = 0;
	if(0 != fopen_s(&pFile, szPath, "w"))
	{
		m_pEventSystem->GetLogManager()->GetEngineLog()->Write(LOG_ERROR, "EventProject::SaveXML 无法打开文件%s", szPath);
		return false;
	}
	fwrite((LPVOID)printer.CStr(), printer.Size(), 1, pFile);
	fclose(pFile);
	return true;
}

bool EventProject::SetClassVolume(int iClass, float fVolume)
{
	if(!m_pEventManager)
		return false;
	int iEventGroupNum = m_pEventManager->GetEventGroupNum();
	for (int i=0; i<iEventGroupNum; ++i)
	{
		EventGroup* pEventGroup = m_pEventManager->GetEventGroupByIndex(i);
		if(!pEventGroup)
			continue;
		if(pEventGroup->GetClass() == iClass)
		{
			if(!pEventGroup->SetClassVolume(fVolume))
				continue;
		}
	}
	return true;
}

bool EventProject::SetClassMute(int iClass, bool bMute)
{
	if(!m_pEventManager)
		return false;
	int iEventGroupNum = m_pEventManager->GetEventGroupNum();
	for (int i=0; i<iEventGroupNum; ++i)
	{
		EventGroup* pEventGroup = m_pEventManager->GetEventGroupByIndex(i);
		if(!pEventGroup)
			continue;
		if(pEventGroup->GetClass() == iClass)
		{
			if(!pEventGroup->SetClassMute(bMute))
				continue;
		}
	}
	return true;
}