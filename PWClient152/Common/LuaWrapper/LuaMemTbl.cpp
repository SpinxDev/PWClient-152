/********************************************************************
	created:	2006/08/21
	author:		kuiwu
	
	purpose:	construct a lua table (only function) from  memory
	Copyright (C) 2006 - All Rights Reserved
*********************************************************************/
#include "LuaMemTbl.h"
#include "LuaState.h"
#include "LuaUtil.h"
#include <AString.h>
#include "LuaScript.h"
#include "ScriptValue.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLuaMemTbl::CLuaMemTbl()
{
	m_pState = NULL;
	m_Ref    =  LUA_NOREF;
}

CLuaMemTbl::~CLuaMemTbl()
{
	Release();
}

bool CLuaMemTbl::Init(const char * szTblName, bool bConfig)
{
	m_pState = (bConfig)? (g_LuaStateMan.GetConfigState()):(g_LuaStateMan.GetAIState());
	assert(m_pState);
	m_szTblName = szTblName;
	m_Methods.clear();

	m_pState->Lock();
	
	lua_State * L = m_pState->GetVM();
	lua_newtable(L);
	lua_pushvalue(L, -1);
	m_Ref =	luaL_ref(L, LUA_REGISTRYINDEX);
	
	lua_setglobal(L, m_szTblName);
	
	//test
	//LuaBind::DumpStack(L, __FILE__, __LINE__ );
	


	bool rt = (m_Ref != LUA_REFNIL);
	m_pState->Unlock();
	return rt;
	
}

void CLuaMemTbl::Release()
{
	if (m_Ref != LUA_NOREF)
	{
		m_pState->Lock();

		
		//unregister buffer
		unsigned int i;
		for (i = 0; i < m_Methods.size(); ++i)
		{
			m_pState->UnRegister(m_Methods[i]);
		}

		lua_State * L = m_pState->GetVM();
		luaL_unref(L, LUA_REGISTRYINDEX, m_Ref);
		LuaBind::ReleaseTbl(L, m_szTblName);
		m_Ref = LUA_NOREF;
		//extra check the table release??
		m_pState->Unlock();
	}

	m_Methods.clear();


	
}

bool CLuaMemTbl::AddMethod(const char * szName, const abase::vector<AString>& vArgs, const char * szBody)
{
	assert(m_pState && m_Ref != LUA_NOREF);

	AString  final;
	AString  body(szBody);
	AString  head;

	if (vArgs.size() > 0)
	{
		AString  args;
		int i;
		for (i = 0; i < (int)vArgs.size(); ++i)
		{
			args += vArgs[i];
			args += ",";
		}
		int len = args.GetLength();
		args[len-1] = '\0';  //last comma
		head.Format("function %s:%s(%s)\n", m_szTblName, szName, args);
	}
	else
	{
		head.Format("function %s:%s()\n", m_szTblName, szName);
	}

	final = head + szBody;
	final += "\nend";

	AString    buf_name;
	buf_name.Format("%s:%s", m_szTblName, szName);

//	CScriptString script_str;
//	script_str.SetAString(final);
	const char * buffer = final.GetBuffer(0);
	int          buf_len = final.GetLength();

	m_pState->Lock();
	//register
	//CLuaScript  * pScript = m_pState->RegisterBuffer(buf_name, script_str.GetUtf8(), script_str.GetLen());
	 CLuaScript  * pScript = m_pState->RegisterBuffer(buf_name, buffer, buf_len);
	m_pState->Unlock();

	final.ReleaseBuffer();	
	//save the added method
	m_Methods.push_back(buf_name);


	//todo: set modify handler

	return (pScript != NULL);
}

void CLuaMemTbl::RemoveMethod(const char * szName)
{
	assert(m_pState && m_Ref != LUA_NOREF);

	//find the added method
	AString    buf_name;
	buf_name.Format("%s:%s", m_szTblName, szName);
	abase::vector<AString>::iterator it;
	for (it = m_Methods.begin(); it != m_Methods.end(); ++it)
	{
		if (*it == buf_name)
		{
			break;
		}
	}
	
	if (it == m_Methods.end())
	{
		return;
	}

	m_Methods.erase(it);
	
	m_pState->Lock();
	//unregister buffer
	m_pState->UnRegister(buf_name);
	
	
	lua_State * L = m_pState->GetVM();
	lua_rawgeti(L, LUA_REGISTRYINDEX, m_Ref);
	lua_pushstring(L, szName);
	lua_pushnil(L);
	lua_rawset(L, -3);
	lua_pop(L, 1);

	m_pState->Unlock();
}


bool CLuaMemTbl::Call(const char * szMethod, const abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results)
{
	assert(m_pState);
	return m_pState->LockCall(m_szTblName, szMethod, args, results);
}