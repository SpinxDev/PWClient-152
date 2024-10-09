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

#define CHECK_ARGNUM(n, func) if( args.size() != n ) { a_LogOutput(1, func ", �ű����ô��� - Ӧ�ð���%d��������", (n)); return; }


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

/** ö�ٵ�ǰ��¼�ɹ����������
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
		LUA_CS_REGISTER_API(pnamespace, LogOutput,			"�����־������̨����־�ļ�������(username,message)"   );
		LUA_CS_REGISTER_API(pnamespace, EnumPlayers,		"ö�ٵ�ǰ��¼�ɹ���������ң���ִ��ָ���Ļص�����������(�ű�������)");
		
		// ȫ��һ��ע��
		LuaFuncFactory::GetSingleton().Flash();
	}
};

// ����һ��ʵ�����Ա��ܹ�ע��API
static UtilApi g_UtilApi;

}