/********************************************************************
	created:	2006/07/10
	author:		kuiwu
	
	purpose:	lua script manager
	history:    refactor to  semi-factory pattern  [6/13/2007 kuiwu]

	Copyright (C) 2006 - All Rights Reserved
*********************************************************************/
#pragma  once


#include <vector.h>
#include <hashmap.h>
#include <AString.h>


class CLuaScript;

class CLuaFileMan  
{
public:
	CLuaFileMan(){};
	~CLuaFileMan(){};

	CLuaScript * Register(const CLuaScript& prototype);
	void         UnRegister(const char * szName);
	CLuaScript * GetScript(const char * szName);
	abase::hash_map<AString, CLuaScript*> * GetAllScripts() 
	{
		return &m_LuaScripts;
	}
	bool         Init();
	void         Release();
	/*
	// NO  double-checked lock in multi-thread environment
	//we can get the multi-thread safety by  some tricks in Lua init
	static	CLuaFileMan * GetInstance();
	*/
private:
	abase::hash_map<AString, CLuaScript *> m_LuaScripts;
	//static CLuaFileMan * m_Instance;

};

extern CLuaFileMan  g_LuaFileMan;


