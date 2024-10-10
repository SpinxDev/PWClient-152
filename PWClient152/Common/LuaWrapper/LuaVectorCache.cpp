/********************************************************************
	created:	2013/2/5
	author:		liudong
	purpose:	Cache lua script vector. It's purpose is to reduce memory fragments.
	Copyright (C) All Rights Reserved
*********************************************************************/
#include "LuaVectorCache.h"
#include <ACriticalSectionUtility.h>
#include <stdio.h>
#include <ALog.h>

static unsigned int l_cache_vector_index = 0;
CLuaVectorCacheMan CLuaVectorCacheMan::s_instance;

//////////////////////////////////////////////////////////////////////////
// CLuaVectorCacheMan
CLuaVectorCacheMan::CLuaVectorCacheMan()
{
	cache_num	= 0;
	element_num	= 0;
	is_log		= false;
	::InitializeCriticalSection(&cs);
	Init();
}

CLuaVectorCacheMan::~CLuaVectorCacheMan()
{
	Release();
	::DeleteCriticalSection(&cs);	
}

bool CLuaVectorCacheMan::Init(const unsigned int max_element_num, const unsigned int max_cache_num, const bool log)
{
	Release();
	if (!_Init(max_element_num,max_cache_num,log))
	{
		Release();
		return false;
	}
	return true;
}

void CLuaVectorCacheMan::Release()
{
	ACriticalSectionAutoLock _auto_lock(&cs);
	for (unsigned int i=0; i<cache.size(); ++i)
	{
		_Release(cache[i]);
	}
	cache.clear();
	cache_num	= 0;
	element_num	= 0;
	is_log		= false;
}

bool CLuaVectorCacheMan::_Init(const unsigned int max_element_num, const unsigned int max_cache_num, const bool log)
{
	assert((max_element_num*max_cache_num)<10*1024*1024 && "lua vector's cache is too big!");
	ACriticalSectionAutoLock _auto_lock(&cs);
	cache_num	= max_cache_num;
	element_num	= max_element_num;
	is_log		= log;	
	if (max_element_num && max_cache_num)
	{
		assert(!cache.size());
		cache.reserve(cache_num);
		for (unsigned int i=0; i<cache_num; ++i)
		{
			CLuaVectorCache* pCache = _Create(max_element_num, max_element_num);
			if (!pCache)
			{
				assert(0);
				return false;
			}
			pCache->is_cache = true;
			cache.push_back(pCache);
		}
	}
	return true;
}

CLuaVectorCache* CLuaVectorCacheMan::_Create(const unsigned int size1, const unsigned int size2)
{
	CLuaVectorCache* p = new CLuaVectorCache();
	assert(p);
	if (p)
	{
		p->p1 = new abase::vector<CScriptValue>();
		p->p2 = new abase::vector<CScriptValue>();
		if (!p->p1 || !p->p2)
		{
			assert(0);
			if (p->p1)
			{
				delete p->p1;
			}
			if (p->p2)
			{
				delete p->p2;
			}
			delete p;
			p = NULL;
		}
	}
	if (p)
	{
		if (size1)
		{
			p->p1->reserve(size1);
		}
		if (size2)
		{
			p->p2->reserve(size2);
		}
	}
	return p;
}

void CLuaVectorCacheMan::_Release(CLuaVectorCache*& p)
{
	if (p)
	{
		if (p->p1)
		{
			delete p->p1;
		}
		if (p->p2)
		{
			delete p->p2;
		}
		delete p;
		p = NULL;
	}
}

CLuaVectorCache* CLuaVectorCacheMan::Create(const unsigned int hint_size_1, const unsigned int hint_size_2)
{
	if (is_log)
	{
		ACriticalSectionAutoLock _auto_lock(&cs);
		const unsigned int log_index = ++l_cache_vector_index;
		const unsigned int cur_cache_size = cache.size();
		_auto_lock.Unlock();
		char msg[512];
		sprintf(msg, 
				"%-12d lua push cache_size:%-8d (%d,%d)", 
				log_index, 
				cur_cache_size,
				hint_size_1, 
				hint_size_2);
		a_LogOutput(1, msg);
	}
	if (_CheckSize(hint_size_1) && _CheckSize(hint_size_2))
	{
		ACriticalSectionAutoLock _auto_lock(&cs);
		if (cache.size())
		{
			CLuaVectorCache* p = cache.back();
			cache.pop_back();
			_auto_lock.Unlock();
			assert(p && p->is_cache);
			return p;
		}
	}
	
	return _Create(hint_size_1, hint_size_2);
}

void CLuaVectorCacheMan::Release(CLuaVectorCache*& p)
{
	if (is_log)
	{
		ACriticalSectionAutoLock _auto_lock(&cs);
		const unsigned int log_index = ++l_cache_vector_index;
		const unsigned int cur_cache_size = cache.size();
		_auto_lock.Unlock();
		char msg[512];
		sprintf(msg, 
				"%-12d lua push cache_size:%-8d             (%d,%d)",
				log_index, 
				cur_cache_size,
				p?p->p1->capacity():0, 
				p?p->p2->capacity():0);
		a_LogOutput(1, msg);
	}
	if (!p)
	{
		return;
	}
	if (!p->p1 || !p->p2)
	{
		assert(0);
		return;
	}
	if (!p->is_cache)
	{
		_Release(p);
		p = NULL;
		return;
	}
	if (_CheckSize(p->p1->capacity()) && _CheckSize(p->p2->capacity()))
	{
		p->p1->clear();
		p->p2->clear();
		ACriticalSectionAutoLock _auto_lock(&cs);
		cache.push_back(p);
		assert(cache.size()<=cache_num);
	}else
	{
		_Release(p);
		CLuaVectorCache* pNew = _Create(element_num,element_num);
		if (pNew)
		{
			pNew->is_cache = true;
			ACriticalSectionAutoLock _auto_lock(&cs);
			cache.push_back(pNew);
			assert(cache.size()<=cache_num);
		}
	}
	p = NULL;
}