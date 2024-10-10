#include "stdafx.h"
#include "Project.h"
#include "Engine.h"
#include "FEventGroup.h"
#include "Global.h"

Project::Project(void)
: m_pEventProject(0)
, m_pEventManager(0)
, m_pAudioManager(0)
, m_pSoundDefManager(0)
, m_bModified(false)
{
}

Project::~Project(void)
{
}

bool Project::Load(const char* szName)
{
	sprintf_s(g_Configs.szCurProjectPath, MAX_PATH, "%s\\%s", g_Configs.szProjectsPath, szName);
	if(!GF_CreateDirectory(g_Configs.szCurProjectPath))
		return false;

	char szPath[MAX_PATH] = {0};
	sprintf_s(szPath, MAX_PATH, "%s\\%s.xml", g_Configs.szCurProjectPath, szName);

	if(!Engine::GetInstance().GetEventSystem()->Load(szPath, m_pEventProject))
	{
		GF_Log(LOG_ERROR, "Project::Load 加载工程“%s”失败", szPath);
		return false;
	}
	m_pEventManager = m_pEventProject->GetEventManager();
	m_pAudioManager = m_pEventProject->GetAudioManager();
	m_pSoundDefManager = m_pEventProject->GetSoundDefManager();
	m_strName = m_pEventProject->GetName();
	
	return true;
}

bool Project::Save()
{
	char szPath[MAX_PATH] = {0};
	sprintf_s(szPath, MAX_PATH, "%s\\%s", g_Configs.szProjectsPath, m_pEventProject->GetName());
	if(!GF_CreateDirectory(szPath))
	{
		GF_Log(LOG_ERROR, "Project::Save 创建目录“%s”失败", szPath);
		return false;
	}
	sprintf_s(szPath, MAX_PATH, "%s\\%s.xml", szPath, m_pEventProject->GetName());	
	if(!m_pEventProject->SaveXML(szPath))
	{
		GF_Log(LOG_ERROR, "Project::Save 保存“%s”失败", szPath);
		return false;
	}
	SetModified(false);
	return true;
}

bool Project::Create(const char* szName)
{
	m_pEventProject = Engine::GetInstance().GetEventSystem()->CreateProject(szName);
	if(!m_pEventProject)
		return false;
	m_pEventManager = m_pEventProject->GetEventManager();
	m_pAudioManager = m_pEventProject->GetAudioManager();
	m_pSoundDefManager = m_pEventProject->GetSoundDefManager();
	sprintf_s(g_Configs.szCurProjectPath, MAX_PATH, "%s\\%s", g_Configs.szProjectsPath, szName);
	if(!GF_CreateDirectory(g_Configs.szCurProjectPath))
		return false;
	if(!Save())
		return false;
	return true;
}