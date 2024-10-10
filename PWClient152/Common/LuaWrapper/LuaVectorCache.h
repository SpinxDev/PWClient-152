/********************************************************************
	created:	2013/2/5
	author:		liudong
	purpose:	Cache lua script vector. It's purpose is to reduce memory fragments.
	Copyright (C) All Rights Reserved
*********************************************************************/
#pragma once
#include <windows.h>
#include <vector.h>
#include <AMiscUtility.h>
#include "ScriptValue.h"

namespace LuaBind
{
	class ScriptVectorCache;
}

class CLuaVectorCache
{
public:
	CLuaVectorCache()
	{
		is_cache= false;
		p1		= NULL;
		p2		= NULL;
	}
	abase::vector<CScriptValue>*	p1;
	abase::vector<CScriptValue>*	p2;

private:
	bool	is_cache;
	friend class CLuaVectorCacheMan;
};

class CLuaVectorCacheMan
{
public:
	~CLuaVectorCacheMan();
	bool Init(const unsigned int max_element_num=128, const unsigned int max_cache_num=32, const bool log = false);
	void Release();
	static CLuaVectorCacheMan& GetInstance(){ return s_instance; }

private:
	
	CLuaVectorCache* Create(const unsigned int hint_size_1=0, const unsigned int hint_size_2=0);
	void Release(CLuaVectorCache*& p);
	
	bool _Init(const unsigned int max_element_num, const unsigned int max_cache_num, const bool log);
	CLuaVectorCache* _Create(const unsigned int size1, const unsigned int size2);
	void _Release(CLuaVectorCache*& p);
	bool _CheckSize(const unsigned int size) const { return size<=element_num; }
	
private:
	abase::vector<CLuaVectorCache*>	cache;
	unsigned int					cache_num;
	unsigned int					element_num;
	bool							is_log;
	CRITICAL_SECTION				cs;
	
	static CLuaVectorCacheMan		s_instance;
	
	friend class LuaBind::ScriptVectorCache;
	friend class CScriptValue;
	DECLARE_SINGLETON(CLuaVectorCacheMan);
};