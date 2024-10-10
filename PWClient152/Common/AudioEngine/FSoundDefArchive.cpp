#include "FSoundDefArchive.h"
#include "FSoundDef.h"
#include "FEventProject.h"
#include "FSoundDefManager.h"
#include "FLogManager.h"
#include "xml\xmlcommon.h"
#include "FEventSystem.h"
#include <AFileImage.h>

static const unsigned int version = 0x100001;

using namespace AudioEngine;

SoundDefArchive::SoundDefArchive(void)
: m_pEventProject(0)
, m_pParentArchive(0)
{
	m_strName.reserve(128);
	m_strNote.reserve(128);
}

SoundDefArchive::~SoundDefArchive(void)
{
	release();
}

bool SoundDefArchive::Init(EventProject* pEventProject)
{
	if(!pEventProject)
		return false;
	m_pEventProject = pEventProject;
	return true;
}

void SoundDefArchive::release()
{
	SoundDefList::const_iterator it = m_listSoundDef.begin();
	for (; it!=m_listSoundDef.end(); ++it)
	{
		delete *it;
	}
	m_listSoundDef.clear();

	SoundDefArchiveList::const_iterator it2 = m_listSoundDefArchive.begin();
	for (; it2!=m_listSoundDefArchive.end(); ++it2)
	{
		delete *it2;
	}
	m_listSoundDefArchive.clear();
}

SoundDef* SoundDefArchive::createSoundDef(const char* szName)
{
	SoundDef* pSoundDef = new SoundDef;
	if(!pSoundDef->Init(m_pEventProject))
	{
		delete pSoundDef;
		return 0;
	}
	pSoundDef->SetParentArchive(this);
	pSoundDef->SetName(szName);
	pSoundDef->m_Guid = GuidGen::GetInstance().Generate();
	m_listSoundDef.push_back(pSoundDef);

	return pSoundDef;
}

SoundDefArchive* SoundDefArchive::createSoundDefArchive(const char* szName)
{
	SoundDefArchive* pSoundDefArchive = new SoundDefArchive;
	if(!pSoundDefArchive->Init(m_pEventProject))
	{
		delete pSoundDefArchive;
		return 0;
	}
	pSoundDefArchive->SetParentArchive(this);
	pSoundDefArchive->SetName(szName);
	m_listSoundDefArchive.push_back(pSoundDefArchive);
	return pSoundDefArchive;
}

SoundDef* SoundDefArchive::GetSoundDefByIndex(int idx) const
{
	if(idx<0 || idx>=GetSoundDefNum())
		return 0;
	SoundDefList::const_iterator it=m_listSoundDef.begin();
	for (int i=0; it!=m_listSoundDef.end(); ++it, ++i)
	{
		if(i == idx)
			return *it;
	}
	return 0;
}

SoundDefArchive* SoundDefArchive::GetSoundDefArchiveByIndex(int idx) const
{
	if(idx<0 || idx>=GetSoundDefArchiveNum())
		return 0;
	SoundDefArchiveList::const_iterator it=m_listSoundDefArchive.begin();
	for (int i=0; it!=m_listSoundDefArchive.end(); ++it, ++i)
	{
		if(i == idx)
			return *it;
	}
	return 0;
}

bool SoundDefArchive::deleteSoundDef(const char* szName)
{
	SoundDefList::iterator it=m_listSoundDef.begin();
	for (; it!=m_listSoundDef.end(); ++it)
	{
		SoundDef* pSoundDef = *it;
		if(!pSoundDef)
			return false;
		if(GF_CompareNoCase(szName, strlen(szName), pSoundDef->GetName(), strlen(pSoundDef->GetName())))
		{
			delete pSoundDef;
			m_listSoundDef.erase(it);
			return true;
		}		
	}
	return true;
}

bool SoundDefArchive::deleteSoundDefArchive(const char* szName)
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
				if(!m_pEventProject->GetSoundDefManager()->DeleteSoundDef(pSoundDef))
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

bool SoundDefArchive::removeSoundDef(SoundDef* pSoundDef)
{
	if(!pSoundDef)
		return false;
	SoundDefList::iterator it = m_listSoundDef.begin();
	for (; it!=m_listSoundDef.end(); ++it)
	{		
		if(pSoundDef == *it)
		{
			m_listSoundDef.erase(it);
			return true;
		}
	}
	return false;
}

bool SoundDefArchive::addSoundDef(SoundDef* pSoundDef)
{
	if(!pSoundDef)
		return false;
	SoundDefList::const_iterator it = m_listSoundDef.begin();
	for (; it!=m_listSoundDef.end(); ++it)
	{		
		if(pSoundDef == *it)
			return false;
	}
	if(m_pEventProject->GetSoundDefManager()->IsSoundDefExist(this, pSoundDef->GetName()))
		return false;
	pSoundDef->SetParentArchive(this);
	m_listSoundDef.push_back(pSoundDef);
	return true;
}

bool SoundDefArchive::removeSoundDefArchive(SoundDefArchive* pSoundDefArchive)
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

bool SoundDefArchive::addSoundDefArchive(SoundDefArchive* pSoundDefArchive)
{
	if(!pSoundDefArchive)
		return false;
	SoundDefArchiveList::const_iterator it = m_listSoundDefArchive.begin();
	for (; it!=m_listSoundDefArchive.end(); ++it)
	{
		if(pSoundDefArchive == *it)
			return false;
	}
	if(m_pEventProject->GetSoundDefManager()->IsArchiveExist(this, pSoundDefArchive->GetName()))
		return false;
	pSoundDefArchive->SetParentArchive(this);
	m_listSoundDefArchive.push_back(pSoundDefArchive);
	return true;
}

bool SoundDefArchive::Load(AFileImage* pFile)
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

	int iNum = 0;
	pFile->Read(&iNum, sizeof(iNum), &dwReadLen);

	for (int i=0; i<iNum; ++i)
	{
		SoundDef* pSoundDef = new SoundDef;
		if(!pSoundDef->Init(m_pEventProject))
		{
			delete pSoundDef;
			return false;
		}
		pSoundDef->SetParentArchive(this);
		if(!pSoundDef->Load(pFile))
		{
			m_pEventProject->GetEventSystem()->GetLogManager()->GetEngineLog()->Write(LOG_ERROR, "SoundDefArchive::Load ¼ÓÔØSoundDef:%sÊ§°Ü", pSoundDef->GetName());
			delete pSoundDef;
			return false;
		}		
		m_listSoundDef.push_back(pSoundDef);
		if(!m_pEventProject->GetSoundDefManager()->AddToMap(pSoundDef))
			return false;
	}

	int iArchiveNum = 0;
	pFile->Read(&iArchiveNum, sizeof(iArchiveNum), &dwReadLen);

	for (int i=0; i<iArchiveNum; ++i)
	{
		SoundDefArchive* pSoundDefArchive = new SoundDefArchive;
		if(!pSoundDefArchive->Init(m_pEventProject))
			return false;
		pSoundDefArchive->SetParentArchive(this);
		if(!pSoundDefArchive->Load(pFile))
		{
			m_pEventProject->GetEventSystem()->GetLogManager()->GetEngineLog()->Write(LOG_ERROR, "SoundDefArchive::Load ¼ÓÔØSoundDefArchive:%sÊ§°Ü", pSoundDefArchive->GetName());
			return false;
		}
		m_listSoundDefArchive.push_back(pSoundDefArchive);
	}
	return true;
}

bool SoundDefArchive::Save(AFile* pFile)
{
	DWORD dwWriteLen = 0;
	pFile->Write((void*)&version, sizeof(version), &dwWriteLen);

	int iNameLen = (int)m_strName.size();
	pFile->Write(&iNameLen, sizeof(iNameLen), &dwWriteLen);
	pFile->Write((void*)m_strName.c_str(), iNameLen, &dwWriteLen);

	int iNoteLen = (int)m_strNote.size();
	pFile->Write(&iNoteLen, sizeof(iNoteLen), &dwWriteLen);
	pFile->Write((void*)m_strNote.c_str(), iNoteLen, &dwWriteLen);

	int iNum = GetSoundDefNum();
	pFile->Write(&iNum, sizeof(iNum), &dwWriteLen);
	SoundDefList::const_iterator it = m_listSoundDef.begin();
	for (; it!=m_listSoundDef.end(); ++it)
	{
		SoundDef* pSoundDef = *it;
		if(!pSoundDef->Save(pFile))
		{
			m_pEventProject->GetEventSystem()->GetLogManager()->GetEngineLog()->Write(LOG_ERROR, "SoundDefArchive::Save ±£´æSoundDef:%sÊ§°Ü", pSoundDef->GetName());
			return false;
		}
	}

	int iArchiveNum = GetSoundDefArchiveNum();
	pFile->Write(&iArchiveNum, sizeof(iArchiveNum), &dwWriteLen);
	SoundDefArchiveList::const_iterator it2 = m_listSoundDefArchive.begin();
	for (; it2!=m_listSoundDefArchive.end(); ++it2)
	{
		SoundDefArchive* pSoundDefArchive = *it2;
		if(!pSoundDefArchive->Save(pFile))
		{
			m_pEventProject->GetEventSystem()->GetLogManager()->GetEngineLog()->Write(LOG_ERROR, "SoundDefArchive::Save ±£´æSoundDefArchive:%sÊ§°Ü", pSoundDefArchive->GetName());
			return false;
		}
	}

	return true;
}

bool SoundDefArchive::LoadXML(TiXmlElement* root)
{
	release();

	unsigned int ver = 0;
	QueryElement(root, "version", ver);
	QueryElement(root, "name", m_strName);
	QueryElement(root, "note", m_strNote);

	int iNum = 0;
	QueryElement(root, "SoundDefNum", iNum);

	TiXmlNode* pSoundDefNode = root->FirstChild("SoundDef");
	for (int i=0; i<iNum; ++i)
	{
		SoundDef* pSoundDef = new SoundDef;
		if(!pSoundDef->Init(m_pEventProject))
		{
			delete pSoundDef;
			return false;
		}
		pSoundDef->SetParentArchive(this);
		if(!pSoundDef->LoadXML(pSoundDefNode->ToElement()))
		{
			m_pEventProject->GetEventSystem()->GetLogManager()->GetEngineLog()->Write(LOG_ERROR, "SoundDefArchive::LoadXML ¼ÓÔØSoundDef:%sÊ§°Ü", pSoundDef->GetName());
			delete pSoundDef;
			return false;
		}		
		m_listSoundDef.push_back(pSoundDef);
		if(!m_pEventProject->GetSoundDefManager()->AddToMap(pSoundDef))
			return false;
		pSoundDefNode = pSoundDefNode->NextSibling("SoundDef");
	}

	int iArchiveNum = 0;
	QueryElement(root, "ArchiveNum", iArchiveNum);
	TiXmlNode* pArchiveNode = root->FirstChild("SoundDefArchive");
	for (int i=0; i<iArchiveNum; ++i)
	{
		SoundDefArchive* pSoundDefArchive = new SoundDefArchive;
		if(!pSoundDefArchive->Init(m_pEventProject))
			return false;
		pSoundDefArchive->SetParentArchive(this);
		if(!pSoundDefArchive->LoadXML(pArchiveNode->ToElement()))
		{
			m_pEventProject->GetEventSystem()->GetLogManager()->GetEngineLog()->Write(LOG_ERROR, "SoundDefArchive::LoadXML ¼ÓÔØSoundDefArchive:%sÊ§°Ü", pSoundDefArchive->GetName());
			return false;
		}
		m_listSoundDefArchive.push_back(pSoundDefArchive);
		pArchiveNode = pArchiveNode->NextSibling("SoundDefArchive");
	}
	return true;
}

bool SoundDefArchive::SaveXML(TiXmlElement* pParent)
{
	TiXmlElement* root = new TiXmlElement("SoundDefArchive");
	pParent->LinkEndChild(root);

	AddElement(root, "version", version);
	AddElement(root, "name", m_strName);
	AddElement(root, "note", m_strNote);
	AddElement(root, "SoundDefNum", GetSoundDefNum());
	SoundDefList::const_iterator it = m_listSoundDef.begin();
	for (; it!=m_listSoundDef.end(); ++it)
	{
		SoundDef* pSoundDef = *it;
		if(!pSoundDef->SaveXML(root))
		{
			m_pEventProject->GetEventSystem()->GetLogManager()->GetEngineLog()->Write(LOG_ERROR, "SoundDefArchive::SaveXML ±£´æSoundDef:%sÊ§°Ü", pSoundDef->GetName());
			return false;
		}
	}

	AddElement(root, "ArchiveNum", GetSoundDefArchiveNum());
	SoundDefArchiveList::const_iterator it2 = m_listSoundDefArchive.begin();
	for (; it2!=m_listSoundDefArchive.end(); ++it2)
	{
		SoundDefArchive* pSoundDefArchive = *it2;
		if(!pSoundDefArchive->SaveXML(root))
		{
			m_pEventProject->GetEventSystem()->GetLogManager()->GetEngineLog()->Write(LOG_ERROR, "SoundDefArchive::SaveXML ±£´æSoundDefArchive:%sÊ§°Ü", pSoundDefArchive->GetName());
			return false;
		}
	}
	return true;
}