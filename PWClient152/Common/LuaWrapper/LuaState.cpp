/********************************************************************
	created:	2006/08/07
	author:		kuiwu
	
	purpose:	an independent lua state.
	Copyright (C) 2006 - All Rights Reserved
*********************************************************************/


#include "LuaState.h"
#include <lua.hpp>
#include <assert.h>
#include "LuaUtil.h"
//#include "LuaThread.h"
#include "ScriptValue.h"
#include "LuaScript.h"
#include "LuaDebug.h"
#include "LuaDebugMsg.h"
#include "LuaAPI.h"

CLuaStateMan g_LuaStateMan;

CLuaState::CLuaState()
{
	m_pState = NULL;
	m_pLuaDebug = NULL;
	::InitializeCriticalSection(&m_csVisit);
}

CLuaState::~CLuaState()
{
	Release();

	::LeaveCriticalSection(&m_csVisit);
	::DeleteCriticalSection(&m_csVisit);
}

int LogPrint(lua_State * L)
{
	const char * msg = luaL_checkstring(L, 1);
	CScriptString str;
	str.SetUtf8(msg, strlen(msg));
	AString out;
	str.RetrieveAString(out);
	LUA_DEBUG_INFO(out);
	return 0;
}

int LuaInclude(lua_State * L)
{
	const char * name = luaL_checkstring(L, 1);
	CScriptString str;
	str.SetUtf8(name, strlen(name));
	AString out;
	str.RetrieveAString(out);
	CLuaState * pState = g_LuaStateMan.GetState(L);
	if (pState)
	{
		if (!pState->RegisterFile(out))
		{
			a_LogOutput(1, "LuaInclude: fail to register file %s", name);
		}
	}
	return 0;
}



bool CLuaState::Init(int debugType)
{
	m_pState = lua_open();
	if (m_pState)
	{
		//open default libs
		luaL_openlibs(m_pState);
		lua_register(m_pState, "LogPrint", LogPrint);
		lua_register(m_pState, "LuaInclude", LuaInclude);
		lua_register(m_pState, "_ERRORMESSAGE", LuaBind::ErrorCallback);

	}
	else
	{
		return false;
	}

	if (debugType != D_NONE) 
	{
		m_pLuaDebug = new CLuaDebug(debugType);
		m_pLuaDebug->SetState(this);
		if (!m_pLuaDebug->Init())
		{
			delete m_pLuaDebug;
			m_pLuaDebug = NULL;
			//let it work without debug
			//return false;
		}
	}
	else
	{
		m_pLuaDebug = NULL;
	}

	return true;
}

void CLuaState::Release()
{
	Lock();

	abase::hash_map<AString, CLuaScript *>::iterator it;
	for (it = m_LuaScripts.begin(); it != m_LuaScripts.end(); ++it)
	{
		if (it->second)
		{
			delete it->second;
		}
	}

	m_LuaScripts.clear();

	if (m_pState)
	{
		lua_close(m_pState);
		m_pState = NULL;
	}

	if (m_pLuaDebug)
	{
		delete m_pLuaDebug;
		m_pLuaDebug = NULL;
	}

	Unlock();
}


void CLuaState::Tick()
{
	if (!IsDebugEnabled())
	{
		return;
	}
	Lock();
	m_pLuaDebug->Tick();
	Unlock();
}

CLuaScript * CLuaState::Register(const char * szName)
{
	CLuaScript * script = m_LuaScripts[szName];
	if (!script)
	{
		script = new CLuaScript(this);
		m_LuaScripts[szName]  = script;
	}
	script->IncRefCount();
	return script;
}

CLuaScript * CLuaState::RegisterFile(const char * szFileName)
{
 	CLuaScript * script = m_LuaScripts[szFileName];

	if (!script)
	{
		script = new CLuaScript(this);
		m_LuaScripts[szFileName]  = script;
		if (!script->FromFile(szFileName) || script->Execute() == CLuaScript::EXECUTE_FAIL)
		{
			delete script;
			AString msg;
			msg.Format("CLuaState::RegisterFile, register %s fail\n", szFileName);
			LUA_DEBUG_INFO(msg);
			m_LuaScripts[szFileName]  = NULL;
			return NULL;
		}
	}
	script->IncRefCount();
	return script;
}

CLuaScript * CLuaState::RegisterBuffer(const char * szBufName, const char * buf, int len)
{
 	CLuaScript * script = m_LuaScripts[szBufName];

	if (!script)
	{
		script = new CLuaScript(this);
		if (!script->FromBuffer(szBufName, buf, len) || script->Execute() == CLuaScript::EXECUTE_FAIL)
		{
			delete script;
			AString msg;
			msg.Format("CLuaState::RegisterBuffer, register %s fail\n", szBufName);
			LUA_DEBUG_INFO(msg);
			return NULL;
		}
		m_LuaScripts[szBufName]  = script;
	}
	script->IncRefCount();
	return script;
}

void CLuaState::UnRegister(const char * szName, abase::vector<AString>* rmTbls /* = NULL */)
{
	CLuaScript * script = m_LuaScripts[szName];

	if (script)
	{
		script->DecRefCount();
		if (script->GetRefCount() > 0)
		{
			return;
		}
	}

	m_LuaScripts.erase(szName);
	if (script)
	{
		if (rmTbls)
		{
			unsigned int i;
			for (i = 0; i < rmTbls->size(); i++)
			{
				LuaBind::ReleaseTbl(GetVM(), rmTbls->at(i));
			}
		}
		delete script;
	}


}


void CLuaState::RegisterLibApi(const char * libName)
{
	LuaBind::Reg2Script(m_pState, libName);
}

bool CLuaState::IsDebugEnabled() const
{
	return (m_pLuaDebug && m_pLuaDebug->IsInDebug());
}

void CLuaState::EnableDebug(bool enable)
{
	if (m_pLuaDebug)
	{
		m_pLuaDebug->SetInDebug(enable);
	}
}

CLuaScript * CLuaState::GetScript(const char * szName)
{
	return m_LuaScripts[szName];
}

void CLuaState::Lock()
{
	::EnterCriticalSection(&m_csVisit);
}

void CLuaState::Unlock()
{
	::LeaveCriticalSection(&m_csVisit);
}

bool CLuaState::LockCall(const char * szTbl, const char * szMethod, const abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results)
{
	if (!m_pState)
	{
		return false;
	}
	Lock();
	bool rt = LuaBind::Call(m_pState, szTbl, szMethod, args, results);
	Unlock();

	return rt;
}

bool CLuaState::Call(const char * szTbl, const char * szMethod, const abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results)
{
	if (!m_pState)
	{
		return false;
	}
	
	return LuaBind::Call(m_pState, szTbl, szMethod, args, results);
}

void CLuaState::ReleaseTbl(const char * tblName)
{
	LuaBind::ReleaseTbl(m_pState, tblName);
}

/*
CLuaStateMan * CLuaStateMan::m_Instance = NULL;

CLuaStateMan * CLuaStateMan::GetInstance()
{
	if (m_Instance == NULL)
	{
		m_Instance = new CLuaStateMan;
	}

	return m_Instance;
}
*/


bool CLuaStateMan::Init(bool bEnableDebug)
{
	
	m_ConfigState = new CLuaState;
	//m_UIState   = new CLuaState;
	m_AIState   = new CLuaState;
	if (bEnableDebug) {
		if (!m_ConfigState->Init(D_CFG) || !m_AIState->Init(D_AI))
		{
			return false;
		}
	}
	else {
		if (!m_ConfigState->Init(D_NONE) || !m_AIState->Init(D_NONE))
		{
			return false;
		}
	}

// 	if (m_AIState->GetVM())
// 	{
// 		RegisterThreadApi(m_AIState->GetVM());
// 	}
	

	return true;
}

CLuaState * CLuaStateMan::GetState(lua_State * L)
{
	if (m_ConfigState->GetVM() == L)
	{
		return m_ConfigState;
	}
// 	if (m_UIState->GetVM() == L)
// 	{
// 		return m_UIState;
// 	}
	if (m_AIState->GetVM() == L)
	{
		return m_AIState;
	}

	return NULL;
}

void CLuaStateMan::Release()
{
	if (m_ConfigState)
	{
		delete m_ConfigState;
		m_ConfigState = NULL;
	}
// 	if (m_UIState)
// 	{
// 		delete m_UIState;
// 		m_UIState = NULL;
// 	}
	if (m_AIState)
	{
		delete m_AIState;
		m_AIState = NULL;
	}


}


void CLuaStateMan::Tick(DWORD dwTick)
{
	static DWORD period = 0;
	period += dwTick;
	if (period > 200)
	{
		m_AIState->Tick();
		m_ConfigState->Tick();
		period = 0;
	}
}