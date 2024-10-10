/********************************************************************
	created:	2006/09/14
	author:		kuiwu
	
	purpose:	a lua thread(coroutine) 
	Copyright (C) 2006 - All Rights Reserved
*********************************************************************/

#include "LuaThread.h"
#include <lua.hpp>
#include <assert.h>
#include "LuaState.h"
#include "LuaUtil.h"
#include "ScriptValue.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLuaThread::CLuaThread()
{
	m_pMasterState = NULL;
	m_pThreadVM = NULL;
	m_Status = LT_NOTINIT;
}

CLuaThread::~CLuaThread()
{
	Release();
}

bool CLuaThread::Init(CLuaState * pMasterState)
{
	m_pMasterState = pMasterState;
	assert(m_pMasterState);
	m_pMasterState->Lock();
	lua_State * L = m_pMasterState->GetVM();

	m_pThreadVM = lua_newthread(L);
	assert(m_pThreadVM);


	m_refThread = luaL_ref(L, LUA_REGISTRYINDEX); //prevent gc and balance stack
    // save a pointer to CLuaThread object in the global table
    // using the new thread's vm pointer as a key,
	// therefore get the CLuathread when given a thread vm efficiently
	lua_pushlightuserdata(L, m_pThreadVM);
	lua_pushlightuserdata(L, this);
	lua_settable(L, LUA_GLOBALSINDEX);
	
	

// 	lua_gc(L, LUA_GCCOLLECT, 0);
// 	lua_getglobal(m_pThreadVM, "a");
// 	int a = lua_tointeger(m_pThreadVM, -1);
// 	printf("a = %d\n", a);
// 	lua_pop(m_pThreadVM, 1);

//	lua_pop(L, 1);

//	LuaBind::DumpStack(L, __FILE__, __LINE__);
	m_pMasterState->Unlock();
	if (m_pThreadVM )
	{
		m_Status = LT_NOTSTART;
	}
	return (m_pThreadVM != NULL);
}

void CLuaThread::Release()
{
	if (m_pMasterState && m_pThreadVM)
	{
 		m_pMasterState->Lock();
 		lua_State * L = m_pMasterState->GetVM();

		lua_pushlightuserdata(L, m_pThreadVM);
		lua_pushnil(L);
		lua_settable(L, LUA_GLOBALSINDEX);

 		luaL_unref(L, LUA_REGISTRYINDEX, m_refThread);
 		m_pMasterState->Unlock();

		
	}
}

bool CLuaThread::BeginThread(const char * szTbl, const char * szMethod, const abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results )
{
	assert(m_pThreadVM && m_pMasterState);
	//Todo: refine. need to lock master???  
	//      visit  sharing global in lua and sharing global in c++???

	int n = lua_gettop(m_pThreadVM);
	if (!LuaBind::GetField(m_pThreadVM, szTbl, szMethod))
	{
		lua_settop(m_pThreadVM, n);
		return false;
	}
	//swap table and method
	lua_insert(m_pThreadVM, -2);
	LuaBind::SetStack(m_pThreadVM, args);
	int nargs = args.size() +1; //add table

	bool rt = _Resume(nargs, results, n+1);
	lua_settop(m_pThreadVM, n);
//	LuaBind::DumpStack(m_pThreadVM, __FILE__, __LINE__);
	return rt;
}

bool CLuaThread::ResumeThread(const abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results)
{
	//Todo: refine. need to lock master???  
	//      visit  sharing global in lua and sharing global in c++???

	int n = lua_gettop(m_pThreadVM);
	LuaBind::SetStack(m_pThreadVM, args);
	bool rt = _Resume(args.size(), results, n+1);
	lua_settop(m_pThreadVM, n);
//	LuaBind::DumpStack(m_pThreadVM, __FILE__, __LINE__);
	return rt;
}

bool CLuaThread::_Resume(int nargs, abase::vector<CScriptValue>& results, int rt_start)
{
	if (m_Status == LT_ERROR || m_Status == LT_DONE)
	{
		LuaBind::ReportError(NULL, "cannot resume error or dead thread");
		return false;
	}

	m_Status = LT_RUNNING;
	int rt = lua_resume(m_pThreadVM, nargs); 
	if (rt == 0)
	{
		m_Status = LT_DONE;
	}
	else if (rt != LUA_YIELD)
	{
		LuaBind::ReportError(m_pThreadVM, "CLuaThread::_Resume");
		m_Status = LT_ERROR;
		return false;
	}
	LuaBind::GetStack(m_pThreadVM, results, rt_start);

	return true;
}

bool CLuaThread::Call(const char * szTbl, const char * szMethod, const abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results)
{
// 	assert(m_pMasterState);
// 	return m_pMasterState->Call(szTbl, szMethod, args, results);

//note: visit  sharing global in lua and sharing global in c++,
//		seems that locking the master makes no sense to the problem. [9/18/2006 kuiwu]
	return LuaBind::Call(m_pThreadVM, szTbl, szMethod, args, results);

}


CLuaThread * GetLuaThread(lua_State * L)
{
	lua_pushlightuserdata(L, L);
	lua_gettable(L, LUA_GLOBALSINDEX);
	CLuaThread * pLuaThread = (CLuaThread *)lua_touserdata(L, -1);
	lua_pop(L, 1);
	return pLuaThread;
}

int ImpSleep(lua_State *L)
{
	int n = lua_gettop(L);
	CLuaThread * pLuaThread = GetLuaThread(L);
	pLuaThread->Sleep();
	return lua_yield(L, n);
}


void RegisterThreadApi(lua_State * L)
{
	static const luaL_reg threadlib[] =
	{
		{"Sleep", ImpSleep},
		{NULL, NULL	}
	};
	luaL_register(L, "ThreadApi", threadlib);
	lua_pop(L, 1);

}