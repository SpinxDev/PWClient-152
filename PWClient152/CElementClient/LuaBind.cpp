/********************************************************************
  created:	   19/4/2006  
  filename:	   LuaBind.cpp
  author:      Wangkuiwu  
  description: 
  Copyright (c) , All Rights Reserved.
*********************************************************************/

#include "LuaBind.h"
#include "LuaUtil.h"
#include "LuaState.h"
#include "ScriptValue.h"

#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_RTDebug.h"
#include "EC_CommandLine.h"



namespace LuaBind
{



bool PreLoad()
{

	return  true;

}

void ScriptErrHandler( const char* szMsg )
{
	if( g_pGame->GetRTDebug() )
		g_pGame->GetRTDebug()->OutputNotifyMessage(0xff0000ff, AS2AC(szMsg));
	a_LogOutput(1, szMsg);
}

bool Init()
{
	bool bLuaDebug = CECCommandLine::GetEnableLuaDebug();
	if (!g_LuaStateMan.Init(bLuaDebug))
	{
		return false;
	}

	RegisterLuaErrHandler(ScriptErrHandler);	

	if (!PreLoad())
	{
	  return false;
	}
	


		
	
	//lua_checkstack(g_LuaStateMan.m_MasterState, 100);
	//DumpStack(g_LuaStateMan.m_MasterState, __FILE__, __LINE__);
	//DumpStack(g_LuaStateMan.GetConfigState()->GetState(), __FILE__, __LINE__);

	return true;
}

void Release()
{
	g_LuaStateMan.Release();
}


}