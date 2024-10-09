#pragma once

#include "LuaWrapper/LuaState.h"
#include "LuaWrapper/LuaUtil.h"
#include "LuaWrapper/LuaAPI.h"
#include "LuaWrapper/LuaScript.h"
#include "LuaWrapper/ScriptValue.h"

#include <hashmap.h>
#include <AString.h>
#include "Common.h"

// ���LUA����
void LuaGetParam(lua_State * L,StringVector& params);

// LUAͨ��ע���
#define LUA_CS_REGISTER_API(pnamespace,funcName,funcUsage) LuaFuncFactory::GetSingleton().Register(#funcName,Imp##funcName,pnamespace);LuaFuncFactory::GetSingleton().RegisterUsage(#funcName,funcUsage,pnamespace)
#define LUA_CS_REGISTER(pnamespace,funcName,funcUsage) LuaFuncFactory::GetSingleton().Register(#funcName,funcName,pnamespace);LuaFuncFactory::GetSingleton().RegisterUsage(#funcName,funcUsage,pnamespace)
// LUAע�������ռ�
#define LUA_CS_NAMESPACE_REGISTER "CppCS"
// LUA����ʱ�����ռ�
#define LUA_CS_NAMESPACE_CALL "lua_cs"

// APIע�����
class LuaAPIBase
{
public:
	virtual void Register() = 0;
	virtual ~LuaAPIBase(){}
};
// lua����
class LuaFuncFactory
{
protected:
	LuaFuncFactory();
public:
	typedef abase::hash_map<AString,int (*)(lua_State *)> LuaFuncMap;
	typedef abase::hash_map<AString,AString> LuaFuncUsageMap;

	typedef abase::hash_map<AString,LuaFuncMap* > NamespaceLuaFuncMap;
	typedef abase::hash_map<AString,LuaFuncUsageMap*> NamespaceLuaFuncUsageMap;

	typedef abase::vector<LuaAPIBase*> LuaAPI;
public:
	// ע��C++�еĺ�����LUA��
	void Register(const char*pname,int (*pfunc)(lua_State *),const char*pnamespace);
	// ȫ�����µ�LUA��
	void Flash();
	// ͨ��ע��lua����
	void InitLuaTest(lua_State * L,const char*pname,int (*pfunc)(lua_State *),const char*pnamespace,const char*pFileName = NULL);
	// ����ע��lua�ļ����Ա㱣������
	void ReflashSrciptFile(const char*pFileName);
	// ��ע�ắ����ʹ��˵��
	void RegisterUsage(const char*pname,const char*pusage,const char*pnamespace);
	// ��ӡʹ��˵��
	void PrintUsage(const char*pnamespace);
	// ����lua�к���
	void Call(const char*pnamespace, const char* pfunc, abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results);
	// ���API�ӿ�
	void AddAPI(LuaAPIBase*papi);
	// ͳһע������API
	void RegisterAllAPI();
public:
	~LuaFuncFactory();
	static LuaFuncFactory& GetSingleton(); 
private:
	NamespaceLuaFuncMap m_funcMap;
	NamespaceLuaFuncUsageMap m_usageMap;// ��ע�ắ����ʹ��˵�����Ա�дlua��֪����ϵͳ����ע����ʲô�������Լ����÷���
	LuaAPI m_apis;
};
