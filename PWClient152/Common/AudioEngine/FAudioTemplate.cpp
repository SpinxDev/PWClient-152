#include "FAudioTemplate.h"
#include "xml/xmlcommon.h"
#include "FEventProject.h"
#include "FAudioManager.h"
#include "FAudio.h"
#include <AFileImage.h>

using namespace AudioEngine;

static const unsigned int version = 0x100001;

AudioTemplate::AudioTemplate(void)
: m_pAudio(0)
, m_pEventProject(0)
{
	m_AudioProperty.fVolume = 0;
	m_AudioProperty.fVolumeRandomization = 0;
	m_AudioProperty.fPitch = 0;
	m_AudioProperty.fPitchRandomization = 0;
}

AudioTemplate::~AudioTemplate(void)
{
}

bool AudioTemplate::Init(EventProject* pEventProject)
{
	if(!pEventProject)
		return false;
	m_pEventProject = pEventProject;
	return true;
}

bool AudioTemplate::Load(AFileImage* pFile)
{
	DWORD dwReadLen = 0;

	unsigned int ver = 0;
	pFile->Read(&ver, sizeof(ver), &dwReadLen);

	int iNoteLen = 0;
	pFile->Read(&iNoteLen, sizeof(iNoteLen), &dwReadLen);
	char szNote[1024] = {0};
	pFile->Read(szNote, iNoteLen, &dwReadLen);
	m_strNote = szNote;

	AudioManager* pAudioManager = m_pEventProject->GetAudioManager();
	if(!pAudioManager)
		return false;

	Guid guid;
	pFile->Read(&guid.guid, sizeof(guid.guid), &dwReadLen);
	SetAudio(pAudioManager->GetAudio(guid));

	pFile->Read(&m_AudioProperty, sizeof(m_AudioProperty), &dwReadLen);
	return true;
}

bool AudioTemplate::Save(AFile* pFile)
{
	DWORD dwWriteLen = 0;

	pFile->Write((void*)&version, sizeof(version), &dwWriteLen);

	int iNoteLen = (int)m_strNote.size();
	pFile->Write(&iNoteLen, sizeof(iNoteLen), &dwWriteLen);
	pFile->Write((void*)m_strNote.c_str(), iNoteLen, &dwWriteLen);

	Guid guid = m_pAudio->GetGuid();
	pFile->Write(&guid.guid, sizeof(guid.guid), &dwWriteLen);

	pFile->Write(&m_AudioProperty, sizeof(m_AudioProperty), &dwWriteLen);
	return true;
}

bool AudioTemplate::LoadXML(TiXmlElement* root)
{
	unsigned int ver = 0;
	QueryElement(root, "version", ver);

	QueryElement(root, "note", m_strNote);

	AudioManager* pAudioManager = m_pEventProject->GetAudioManager();
	if(!pAudioManager)
		return false;

	Guid guid;
	QueryElement(root, "GUID", guid.guid);
	SetAudio(pAudioManager->GetAudio(guid));

	TiXmlNode* pNode = root->FirstChild("AudioProperty");
	if(!pNode)
		return false;
	QueryElement(pNode, "volume", m_AudioProperty.fVolume);
	QueryElement(pNode, "volumeRanomization", m_AudioProperty.fVolumeRandomization);
	QueryElement(pNode, "pitch", m_AudioProperty.fPitch);	
	QueryElement(pNode, "pitchRandomization", m_AudioProperty.fPitchRandomization);
	return true;
}

bool AudioTemplate::SaveXML(TiXmlElement* pParent)
{
	TiXmlElement* root = new TiXmlElement("AudioTemplate");
	pParent->LinkEndChild(root);

	AddElement(root, "version", version);
	AddElement(root, "note", m_strNote);
	Guid guid = m_pAudio->GetGuid();	
	AddElement(root, "GUID", guid.guid);

	TiXmlElement* pAudioPropertyNode = new TiXmlElement("AudioProperty");
	root->LinkEndChild(pAudioPropertyNode);
	AddElement(pAudioPropertyNode, "volume", m_AudioProperty.fVolume);
	AddElement(pAudioPropertyNode, "volumeRanomization", m_AudioProperty.fVolumeRandomization);
	AddElement(pAudioPropertyNode, "pitch", m_AudioProperty.fPitch);
	AddElement(pAudioPropertyNode, "pitchRandomization", m_AudioProperty.fPitchRandomization);

	return true;
}