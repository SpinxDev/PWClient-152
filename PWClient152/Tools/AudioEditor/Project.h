#pragma once

#include "FEventProject.h"
#include "FAudioManager.h"
#include "FSoundDefManager.h"
#include "FEventManager.h"

#include <string>

using AudioEngine::EventProject;
using AudioEngine::EventManager;
using AudioEngine::AudioManager;
using AudioEngine::SoundDefManager;

class Project
{
public:
	Project(void);
	~Project(void);
public:
	bool				Create(const char* szName);
	bool				Load(const char* szName);
	bool				Save();
	EventManager*		GetEventManager() const { return m_pEventManager; }
	AudioManager*		GetAudioManager() const { return m_pAudioManager; }
	SoundDefManager*	GetSoundDefManager() const { return m_pSoundDefManager; }
	EventProject*		GetEventProject() const { return m_pEventProject; }
	void				SetModified(bool bModified) { m_bModified = bModified; }
	bool				IsModified() const { return m_bModified; }
	const char*			GetName() const { return m_strName.c_str(); }
protected:
	EventProject*		m_pEventProject;
	std::string			m_strName;
	EventManager*		m_pEventManager;
	AudioManager*		m_pAudioManager;
	SoundDefManager*	m_pSoundDefManager;
	bool				m_bModified;
};
