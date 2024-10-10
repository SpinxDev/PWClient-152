/********************************************************************
	created:	2006/07/10
	author:		kuiwu
	
	purpose:	lua script manager
	history:    refactor to singleton and semi-factory pattern  [6/13/2007 kuiwu]

	Copyright (C) 2006 - All Rights Reserved
*********************************************************************/

#include "LuaManager.h"
#include "LuaScript.h"
#include <string.h>

/*
CLuaFileMan * CLuaFileMan::m_Instance = NULL;

CLuaFileMan * CLuaFileMan::GetInstance()
{
	if (m_Instance== NULL)
	{
		m_Instance = new CLuaFileMan;
	}

	return m_Instance;
}
*/

CLuaFileMan   g_LuaFileMan;

bool CLuaFileMan::Init()
{
	return true;
}

void CLuaFileMan::Release()
{

	abase::hash_map<AString, CLuaScript *>::iterator it;
	for (it = m_LuaScripts.begin(); it != m_LuaScripts.end(); ++it)
	{
		if (it->second)
		{
			delete it->second;
		}
	}

	m_LuaScripts.clear();

}

CLuaScript * CLuaFileMan::Register(const CLuaScript& prototype)
{
	
	CLuaScript * script = m_LuaScripts[prototype.GetName()];

	if (script)
	{
		delete script;
	}
	script = prototype.Clone();
	
	m_LuaScripts[script->GetName()] = script;


	return script;
}

void CLuaFileMan::UnRegister(const char * szName)
{

	CLuaScript * script = m_LuaScripts[szName];
	if (script)
	{
		delete script;
		m_LuaScripts[szName] = NULL;
	}
	//seems no need to erase
}



CLuaScript * CLuaFileMan::GetScript(const char * szName)
{
	return m_LuaScripts[szName];
}

