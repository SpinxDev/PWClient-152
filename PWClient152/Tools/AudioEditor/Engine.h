#pragma once

#include "FEventSystem.h"
#include "FLog.h"

class Project;

using AudioEngine::Log;
using AudioEngine::EventSystem;
using AudioEngine::INITFLAGS;

class Engine
{
protected:
	Engine(void);
	~Engine(void);
public:
	bool					Init(int maxchannels, INITFLAGS flags, void *extradriverdata);
	static Engine&			GetInstance();
	void					Release();
	EventSystem*			GetEventSystem() const { return m_pEventSystem; }
	bool					CreateProject(const char* szName);
	Project*				GetCurProject() const { return m_pCurProject; }
	bool					LoadProject(const char* szName);
	bool					DeleteProject(const char* szName);
	bool					CloseProject();
	Log*					GetLog() const { return m_pEditorLog; }
protected:
	EventSystem*			m_pEventSystem;
	Project*				m_pCurProject;
	Log*					m_pEditorLog;
};
