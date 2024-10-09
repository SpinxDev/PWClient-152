/*
 * FILE: UtilApi.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Shizhenhua, 2013/3/21
 *
 * HISTORY: 
 *
 * Copyright (c) 2011 ACE Studio, All Rights Reserved.
 */


#include "Common.h"
#include "luaFunc.h"
#include "SessionManager.h"
#include "GameSession.h"
#include "GameObject.h"


///////////////////////////////////////////////////////////////////////////
//  
//  Define and Macro
//  
///////////////////////////////////////////////////////////////////////////

#define GET_LUA_STRING(dest, val) AString dest; val.RetrieveAString(dest)
#define GET_LUA_WSTRING(dest, val) AWString dest; val.RetrieveAWString(dest)
#define SET_LUA_TABLE(key, value) val.SetVal(key); keyvec.push_back(val); val.SetVal(value); valvec.push_back(val);

#define CHECK_ARGNUM(n, func) if( args.size() != n ) { a_LogOutput(1, func ", 脚本调用错误 - 应该包含%d个参数！", (n)); return; }


///////////////////////////////////////////////////////////////////////////
//  
//  Types and Global functions
//  
///////////////////////////////////////////////////////////////////////////

static GameObject* _GetGameObject(const char* username)
{
	GameSession* pSession = SessionManager::GetSingleton().GetSessionByName(username);
	return pSession ? pSession->GetGameObject() : NULL;
}


///////////////////////////////////////////////////////////////////////////
//  
//  Lua API Functions
//  
///////////////////////////////////////////////////////////////////////////

namespace LuaBind
{

/** 
param: text to write to log file
*/
void LogOutput(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_ARGNUM(2, "Util.LogOutput");

	GET_LUA_STRING(username, args[0]);
	GameObject* pGame = _GetGameObject(username);
	if( pGame )
	{
		GET_LUA_STRING(strMsg, args[1]);
		a_LogOutput(1, "%s User:%s", strMsg, username);
	}
}

/** 枚举当前登录成功的所有玩家
*/
void EnumPlayers(abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& ret)
{
	CHECK_ARGNUM(1, "Util.EnumPlayers");

	GET_LUA_STRING(funcName, args[0]);
	if( !funcName.IsEmpty() )
	{
		CScriptValue val;
		abase::vector<CScriptValue> args;
		abase::vector<CScriptValue> ret;

		StringVector players;
		SessionManager::GetSingleton().EnumPlayers(players);
		for( StringVector::iterator it=players.begin();it!=players.end();++it )
		{
			args.clear();
			ret.clear();
			val.SetVal(*it);
			args.push_back(val);
			LuaFuncFactory::GetSingleton().Call(NULL, funcName, args, ret);
		}
	}
	else
	{
		a_LogOutput(1, "Util.EnumPlayers, Invalid callback function name!");
	}
}


IMPLEMENT_SCRIPT_API(LogOutput)
IMPLEMENT_SCRIPT_API(EnumPlayers)

struct UtilApi : public LuaAPIBase
{
	UtilApi()
	{
		LuaFuncFactory::GetSingleton().AddAPI(this);
	}

	virtual void Register()
	{
		const char* pnamespace = "Util";
		LUA_CS_REGISTER_API(pnamespace, LogOutput,			"输出日志到控制台和日志文件，参数(username,message)"   );
		LUA_CS_REGISTER_API(pnamespace, EnumPlayers,		"枚举当前登录成功的所有玩家，并执行指定的回调函数，参数(脚本函数名)");
		
		// 全部一起注册
		LuaFuncFactory::GetSingleton().Flash();
	}
};

// 产生一个实例，以便能够注册API
static UtilApi g_UtilApi;

}