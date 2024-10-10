#include "stdafx.h"
#include "Engine.h"
#include "Project.h"
#include "ProjectList.h"
#include "Global.h"
#include "FLogManager.h"
#include "FLog.h"
#include "Global.h"
#include "FReverbDataManager.h"

Engine::Engine(void)
: m_pEventSystem(0)
, m_pCurProject(0)
{
}

Engine::~Engine(void)
{
	Release();
}

bool Engine::Init(int maxchannels, INITFLAGS flags, void *extradriverdata)
{	
	m_pEventSystem = new EventSystem;
	if(!m_pEventSystem->Init(maxchannels, flags, extradriverdata))
	{
		return false;
	}
	if(!m_pEventSystem->GetLogManager()->CreateLog("AudioEditor", "AudioEditor.log"))
		return false;
	m_pEditorLog = m_pEventSystem->GetLogManager()->GetLog("AudioEditor");	
	if(!m_pEditorLog)
		return false;
	m_pEditorLog->EnableFlushImmediate();
	GF_Log(LOG_NORMAL, "创建音频编辑器日志");
	
	return true;
}

void Engine::Release()
{
	delete m_pCurProject;
	m_pCurProject = 0;
	delete m_pEventSystem;
	m_pEventSystem = 0;
}

Engine& Engine::GetInstance()
{
	static Engine instance;
	return instance;
}

bool Engine::CreateProject(const char* szName)
{
	if(!CloseProject())
		return false;
	m_pCurProject = new Project;
	if(!m_pCurProject->Create(szName))
		return false;
	return true;
}

bool Engine::LoadProject(const char* szName)
{
	if(!CloseProject())
		return false;
	m_pCurProject = new Project;
	if(!m_pCurProject->Load(szName))
		return false;
	return true;
}

bool Engine::CloseProject()
{
	delete m_pCurProject;
	m_pCurProject = 0;
	return true;
}

bool Engine::DeleteProject(const char* szName)
{
	if(!CloseProject())
		return false;
	if(!ProjectList::GetInstance().Delete(szName))
		return false;
	if(!ProjectList::GetInstance().SaveXML(g_Configs.szProjectListFile))
		return false;
	char szPath[MAX_PATH] = {0};
	sprintf_s(szPath, MAX_PATH, "%sAudio\\Projects\\%s", g_Configs.szRootPath, szName);
	if(!GF_DeleteFolder(szPath))
		return false;
	return true;
}