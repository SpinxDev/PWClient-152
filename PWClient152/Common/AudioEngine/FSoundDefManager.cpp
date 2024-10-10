#include "FSoundDefManager.h"
#include "FLogManager.h"
#include "FSoundDef.h"
#include "FSoundDefArchive.h"
#include "FEventProject.h"
#include "FEventSystem.h"

#include "xml/xmlcommon.h"
#include <AFileImage.h>

using namespace AudioEngine;

static const unsigned int version = 0x100001;

SoundDefManager::SoundDefManager(void)
: m_pEventProject(0)
{
}

SoundDefManager::~SoundDefManager(void)
{
	release();
}

bool SoundDefManager::Init(EventProject* pEventProject)
{
	if(!pEventProject)
		return false;
	m_pEventProject = pEventProject;
	return true;
}

SoundDef* SoundDefManager::CreateSoundDef(SoundDefArchive* pSoundDefArchive, const char* szName)
{
	if(!pSoundDefArchive)
		return 0;
	SoundDef* pSoundDef = pSoundDefArchive->createSoundDef(szName);
	if(!pSoundDef)
		return 0;
	m_mapSoundDef[pSoundDef->GetGuid()] = pSoundDef;
	return pSoundDef;
}

SoundDefArchive* SoundDefManager::CreateSoundDefArchive(const char* szName)
{
	SoundDefArchive* pSoundDefArchive = new SoundDefArchive;
	if(!pSoundDefArchive->Init(m_pEventProject))
	{
		delete pSoundDefArchive;
		return 0;
	}
	pSoundDefArchive->SetName(szName);
	m_listSoundDefArchive.push_back(pSoundDefArchive);
	return pSoundDefArchive;
}

SoundDef* SoundDefManager::GetSoundDef(const Guid& guid) const
{
	SoundDefMap::const_iterator it = m_mapSoundDef.find(guid);
	if(it != m_mapSoundDef.end())
	{
		return it->second;
	}
	return 0;
}

void SoundDefManager::release()
{
	SoundDefArchiveList::const_iterator it2 = m_listSoundDefArchive.begin();
	for (; it2!=m_listSoundDefArchive.end(); ++it2)
	{
		delete *it2;
	}
	m_listSoundDefArchive.clear();

	m_mapSoundDef.clear();
}

SoundDefArchive* SoundDefManager::GetSoundDefArchiveByIndex(int idx) const
{
	if(idx<0 || idx>=GetSoundDefArchiveNum())
		return 0;
	SoundDefArchiveList::const_iterator it = m_listSoundDefArchive.begin();
	for (int i=0; it!=m_listSoundDefArchive.end(); ++it,++i)
	{
		if(i == idx)
			return *it;
	}
	return 0;
}

bool SoundDefManager::AddToMap(SoundDef* pSoundDef)
{
	if(!pSoundDef)
		return false;
	m_mapSoundDef[pSoundDef->GetGuid()] = pSoundDef;
	return true;
}

SoundDefArchive* SoundDefManager::CreateSoundDefArchive(SoundDefArchive* pSoundDefArchive, const char* szName)
{
	if(!pSoundDefArchive)
		return 0;
	SoundDefArchive* pNewSoundDefArchive = pSoundDefArchive->createSoundDefArchive(szName);
	return pNewSoundDefArchive;
}

bool SoundDefManager::IsArchiveExist(SoundDefArchive* pSoundDefArchive, const char* szName)
{
	int iArchiveNum = pSoundDefArchive->GetSoundDefArchiveNum();
	if(0 == iArchiveNum)
		return false;
	for (int i=0; i<iArchiveNum; ++i)
	{
		SoundDefArchive* pArchive = pSoundDefArchive->GetSoundDefArchiveByIndex(i);
		if(GF_CompareNoCase(pArchive->GetName(), strlen(pArchive->GetName()), szName, strlen(szName)))
			return true;
	}
	return false;
}

bool SoundDefManager::IsArchiveExist(const char* szName)
{
	if(0 == GetSoundDefArchiveNum())
		return false;
	SoundDefArchiveList::const_iterator it = m_listSoundDefArchive.begin();
	for (; it!=m_listSoundDefArchive.end(); ++it)
	{
		SoundDefArchive* pSoundDefArchive = *it;
		if(GF_CompareNoCase(pSoundDefArchive->GetName(), strlen(pSoundDefArchive->GetName()), szName, strlen(szName)))
			return true;
	}
	return false;
}

bool SoundDefManager::IsSoundDefExist(SoundDefArchive* pSoundDefArchive, const char* szName)
{
	int iSoundDefNum = pSoundDefArchive->GetSoundDefNum();
	for (int i=0; i<iSoundDefNum; ++i)
	{
		SoundDef* pSoundDef = pSoundDefArchive->GetSoundDefByIndex(i);
		if(GF_CompareNoCase(pSoundDef->GetName(), strlen(pSoundDef->GetName()), szName, strlen(szName)))
			return true;
	}
	return false;
}

bool SoundDefManager::DeleteSoundDef(SoundDef* pSoundDef)
{
	if(!pSoundDef)
		return false;
	Guid guid = pSoundDef->GetGuid();
	if(pSoundDef->GetParentArchive())
	{
		if(!pSoundDef->GetParentArchive()->deleteSoundDef(pSoundDef->GetName()))
			return false;
	}
	m_mapSoundDef.erase(m_mapSoundDef.find(guid));
	return true;
}

bool SoundDefManager::DeleteSoundDefArchive(SoundDefArchive* pSoundDefArchive)
{
	if(!pSoundDefArchive)
		return false;
	if(pSoundDefArchive->GetParentArchive())
	{
		return pSoundDefArchive->GetParentArchive()->deleteSoundDefArchive(pSoundDefArchive->GetName());
	}
	else
	{
		return deleteSoundDefArchive(pSoundDefArchive->GetName());
	}
	return false;
}

bool SoundDefManager::deleteSoundDefArchive(const char* szName)
{
	SoundDefArchiveList::iterator it=m_listSoundDefArchive.begin();
	for (int i=0; it!=m_listSoundDefArchive.end(); ++it, ++i)
	{
		SoundDefArchive* pSoundDefArchive = *it;
		if(!pSoundDefArchive)
			return false;
		if(GF_CompareNoCase(szName, strlen(szName), pSoundDefArchive->GetName(), strlen(pSoundDefArchive->GetName())))
		{
			while(pSoundDefArchive->GetSoundDefNum())
			{
				SoundDef* pSoundDef = pSoundDefArchive->GetSoundDefByIndex(0);
				if(!pSoundDef)
					return false;
				if(!DeleteSoundDef(pSoundDef))
					return false;
			}

			while(pSoundDefArchive->GetSoundDefArchiveNum())
			{
				SoundDefArchive* pArchive = pSoundDefArchive->GetSoundDefArchiveByIndex(0);
				if(!pArchive)
					return false;
				if(!pSoundDefArchive->deleteSoundDefArchive(pArchive->GetName()))
					return false;
			}

			delete pSoundDefArchive;
			m_listSoundDefArchive.erase(it);
			return true;
		}
	}
	return false;
}

bool SoundDefManager::removeSoundDefArchive(SoundDefArchive* pSoundDefArchive)
{
	if(!pSoundDefArchive)
		return false;
	SoundDefArchiveList::iterator it = m_listSoundDefArchive.begin();
	for (; it!=m_listSoundDefArchive.end(); ++it)
	{
		if(pSoundDefArchive == *it)
		{
			m_listSoundDefArchive.erase(it);
			return true;
		}
	}
	return false;
}

bool SoundDefManager::addSoundDefArchive(SoundDefArchive* pSoundDefArchive)
{
	if(!pSoundDefArchive)
		return false;
	SoundDefArchiveList::const_iterator it = m_listSoundDefArchive.begin();
	for (; it!=m_listSoundDefArchive.end(); ++it)
	{
		if(pSoundDefArchive == *it)
			return false;
	}
	if(IsArchiveExist(pSoundDefArchive->GetName()))
		return false;
	pSoundDefArchive->SetParentArchive(0);
	m_listSoundDefArchive.push_back(pSoundDefArchive);
	return true;
}

bool SoundDefManager::MoveSoundDef(SoundDef* pSoundDef, SoundDefArchive* pDestArchive)
{
	if(!pSoundDef || !pDestArchive)
		return false;
	SoundDefArchive* pOldParentArchive = pSoundDef->GetParentArchive();
	if(pOldParentArchive == pDestArchive)
		return true;
	if(!pDestArchive->addSoundDef(pSoundDef))
		return false;
	if(!pOldParentArchive->removeSoundDef(pSoundDef))
	{
		if(!pDestArchive->removeSoundDef(pSoundDef))
			return false;
		return false;
	}
	return true;
}

bool SoundDefManager::MoveArchive(SoundDefArchive* pArchive, SoundDefArchive* pDestArchive)
{
	if(!pArchive || !pDestArchive)
		return false;
	SoundDefArchive* pOldParentArvhie = pArchive->GetParentArchive();
	SoundDefArchive* pParentArchive = pDestArchive;
	while (pParentArchive)
	{
		if(pParentArchive == pArchive)
			return false;
		pParentArchive = pParentArchive->GetParentArchive();
	}
	if(pOldParentArvhie == pDestArchive)
		return true;
	if(!pDestArchive->addSoundDefArchive(pArchive))
		return false;
	if(pOldParentArvhie)
	{
		if(!pOldParentArvhie->removeSoundDefArchive(pArchive))
		{
			if(!pDestArchive->removeSoundDefArchive(pArchive))
				return false;
			return false;
		}		
	}
	else
	{
		if(!removeSoundDefArchive(pArchive))
		{
			if(!pDestArchive->removeSoundDefArchive(pArchive))
				return false;
			return false;
		}
	}
	return true;
}

bool SoundDefManager::MoveArchive(SoundDefArchive* pArchive)
{
	if(!pArchive)
		return false;
	SoundDefArchive* pOldParentArchive = pArchive->GetParentArchive();
	if(!pOldParentArchive)
		return true;
	if(!addSoundDefArchive(pArchive))
		return false;
	if((!pOldParentArchive->removeSoundDefArchive(pArchive)))
	{
		if(!removeSoundDefArchive(pArchive))
			return false;
		return false;
	}
	return true;
}

bool SoundDefManager::Load(AFileImage* pFile)
{
	release();

	DWORD dwReadLen = 0;

	unsigned int ver = 0;
	pFile->Read(&ver, sizeof(ver), &dwReadLen);

	int iArchiveNum = 0;
	pFile->Read(&iArchiveNum, sizeof(iArchiveNum), &dwReadLen);

	for (int i=0; i<iArchiveNum; ++i)
	{
		SoundDefArchive* pSoundDefArchive = new SoundDefArchive;
		if(!pSoundDefArchive->Init(m_pEventProject))
			return false;
		if(!pSoundDefArchive->Load(pFile))
		{
			m_pEventProject->GetEventSystem()->GetLogManager()->GetEngineLog()->Write(LOG_ERROR, "SoundDefManager::Load ¼ÓÔØSoundDefArchive:%sÊ§°Ü", pSoundDefArchive->GetName());
			return false;
		}
		m_listSoundDefArchive.push_back(pSoundDefArchive);
	}
	return true;
}

bool SoundDefManager::Save(AFile* pFile)
{
	DWORD dwWriteLen = 0;
	pFile->Write((void*)&version, sizeof(version), &dwWriteLen);

	int iArchiveNum = GetSoundDefArchiveNum();
	pFile->Write(&iArchiveNum, sizeof(iArchiveNum), &dwWriteLen);
	SoundDefArchiveList::const_iterator it2 = m_listSoundDefArchive.begin();
	for (; it2!=m_listSoundDefArchive.end(); ++it2)
	{
		SoundDefArchive* pSoundDefArchive = *it2;
		if(!pSoundDefArchive->Save(pFile))
		{
			m_pEventProject->GetEventSystem()->GetLogManager()->GetEngineLog()->Write(LOG_ERROR, "SoundDefManager::Save ±£´æSoundDefArchive:%sÊ§°Ü", pSoundDefArchive->GetName());
			return false;
		}
	}

	return true;
}

bool SoundDefManager::LoadXML(TiXmlElement* root)
{
	release();

	unsigned int ver = 0;
	QueryElement(root, "version", ver);
	int iArchiveNum = 0;
	QueryElement(root, "ArchiveNum", iArchiveNum);
	TiXmlNode* pArchiveNode = root->FirstChild("SoundDefArchive");
	for (int i=0; i<iArchiveNum; ++i)
	{
		SoundDefArchive* pSoundDefArchive = new SoundDefArchive;
		if(!pSoundDefArchive->Init(m_pEventProject))
			return false;
		if(!pSoundDefArchive->LoadXML(pArchiveNode->ToElement()))
		{
			m_pEventProject->GetEventSystem()->GetLogManager()->GetEngineLog()->Write(LOG_ERROR, "SoundDefManager::LoadXML ¼ÓÔØSoundDefArchive:%sÊ§°Ü", pSoundDefArchive->GetName());
			return false;
		}
		m_listSoundDefArchive.push_back(pSoundDefArchive);
		pArchiveNode = pArchiveNode->NextSibling("SoundDefArchive");
	}
	return true;
}

bool SoundDefManager::SaveXML(TiXmlElement* pParent)
{
	TiXmlElement* root = new TiXmlElement("SoundDefManager");
	pParent->LinkEndChild(root);

	AddElement(root, "version", version);
	AddElement(root, "ArchiveNum", GetSoundDefArchiveNum());
	SoundDefArchiveList::const_iterator it2 = m_listSoundDefArchive.begin();
	for (; it2!=m_listSoundDefArchive.end(); ++it2)
	{
		SoundDefArchive* pSoundDefArchive = *it2;
		if(!pSoundDefArchive->SaveXML(root))
		{
			m_pEventProject->GetEventSystem()->GetLogManager()->GetEngineLog()->Write(LOG_ERROR, "SoundDefManager::SaveXML ±£´æSoundDefArchive:%sÊ§°Ü", pSoundDefArchive->GetName());
			return false;
		}
	}
	return true;
}