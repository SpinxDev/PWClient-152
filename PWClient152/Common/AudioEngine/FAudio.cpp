#include "FAudio.h"

#include "FEventSystem.h"
#include "FAudioBank.h"
#include "FEventLayer.h"
#include "FSoundDef.h"
#include "FEvent.h"
#include "FEventProject.h"
#include "FAudioManager.h"
#include "xml/xmlcommon.h"

#include <fmod.hpp>
#include <cmath>
#include <AFileImage.h>

using namespace AudioEngine;

static const unsigned int version = 0x100001;

Audio::Audio(void)
: m_pAudioBank(0)
, m_pEventProject(0)
, m_pParentContainer(0)
, m_bAccurateTime(false)
{
	m_strName.reserve(128);
	m_strNote.reserve(128);
}

Audio::~Audio(void)
{
	release();
}

void Audio::release()
{
}

bool Audio::Init(EventProject* pEventProject)
{
	if(!pEventProject)
		return false;
	m_pEventProject = pEventProject;
	return true;
}

bool Audio::Load(AFileImage* pFile)
{
	DWORD dwReadLen = 0;
	unsigned int ver = 0;
	pFile->Read(&ver, sizeof(ver), &dwReadLen);

	int iNameLen = 0;
	pFile->Read(&iNameLen, sizeof(iNameLen), &dwReadLen);
	char szName[1024] = {0};
	pFile->Read(szName, iNameLen, &dwReadLen);
	m_strName = szName;

	pFile->Read(&m_bAccurateTime, sizeof(m_bAccurateTime), &dwReadLen);

	int iNoteLen = 0;
	pFile->Read(&iNoteLen, sizeof(iNoteLen), &dwReadLen);
	char szNote[1024] = {0};
	pFile->Read(szNote, iNoteLen, &dwReadLen);
	m_strNote = szNote;

	int iPathLen = 0;
	pFile->Read(&iPathLen, sizeof(iPathLen), &dwReadLen);
	char szPath[1024] = {0};
	pFile->Read(szPath, iPathLen, &dwReadLen);
	m_strPath = szPath;

	pFile->Read(&m_Guid.guid, sizeof(m_Guid.guid), &dwReadLen);
	return true;
}

bool Audio::Save(AFile* pFile)
{
	DWORD dwWriteLen = 0;
	pFile->Write((void*)&version, sizeof(version), &dwWriteLen);

	int iNameLen = (int)m_strName.size();
	pFile->Write(&iNameLen, sizeof(iNameLen), &dwWriteLen);
	pFile->Write((void*)m_strName.c_str(), iNameLen, &dwWriteLen);

	pFile->Write(&m_bAccurateTime, sizeof(m_bAccurateTime), &dwWriteLen);

	int iNoteLen = (int)m_strNote.size();
	pFile->Write(&iNoteLen, sizeof(iNoteLen), &dwWriteLen);
	pFile->Write((void*)m_strNote.c_str(), iNoteLen, &dwWriteLen);

	int iPathLen = (int)m_strPath.size();
	pFile->Write(&iPathLen, sizeof(iPathLen), &dwWriteLen);
	pFile->Write((void*)m_strPath.c_str(), iPathLen, &dwWriteLen);

	pFile->Write(&m_Guid.guid, sizeof(m_Guid.guid), &dwWriteLen);
	return true;
}

bool Audio::LoadXML(TiXmlElement* root)
{
	unsigned int ver = 0;
	QueryElement(root, "version", ver);
	QueryElement(root, "name", m_strName);
	QueryElement(root, "accurate_time", m_bAccurateTime);
	QueryElement(root, "note", m_strNote);
	QueryElement(root, "path", m_strPath);
	QueryElement(root, "GUID", m_Guid.guid);
	return true;
}

bool Audio::SaveXML(TiXmlElement* pParent)
{
	TiXmlElement* root = new TiXmlElement("Audio");
	pParent->LinkEndChild(root);

	AddElement(root, "version", version);
	AddElement(root, "name", m_strName);
	AddElement(root, "accurate_time", m_bAccurateTime);
	AddElement(root, "note", m_strNote);
	AddElement(root, "path", m_strPath);
	AddElement(root, "GUID", m_Guid.guid);
	return true;
}