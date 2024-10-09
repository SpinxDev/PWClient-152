#pragma once

#include "LuaWrapper/LuaState.h"
#include "LuaWrapper/LuaUtil.h"
#include "LuaWrapper/LuaAPI.h"
#include "LuaWrapper/LuaScript.h"
#include "LuaWrapper/ScriptValue.h"

#include <hashmap.h>
#include <AString.h>
#include "Common.h"

// 获得LUA参数
void LuaGetParam(lua_State * L,StringVector& params);

// LUA通用注册宏
#define LUA_CS_REGISTER_API(pnamespace,funcName,funcUsage) LuaFuncFactory::GetSingleton().Register(#funcName,Imp##funcName,pnamespace);LuaFuncFactory::GetSingleton().RegisterUsage(#funcName,funcUsage,pnamespace)
#define LUA_CS_REGISTER(pnamespace,funcName,funcUsage) LuaFuncFactory::GetSingleton().Register(#funcName,funcName,pnamespace);LuaFuncFactory::GetSingleton().RegisterUsage(#funcName,funcUsage,pnamespace)
// LUA注册命名空间
#define LUA_CS_NAMESPACE_REGISTER "CppCS"
// LUA调用时命名空间
#define LUA_CS_NAMESPACE_CALL "lua_cs"

// API注册基类
class LuaAPIBase
{
public:
	virtual void Register() = 0;
	virtual ~LuaAPIBase(){}
};
// lua工厂
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
	// 注册C++中的函数到LUA中
	void Register(const char*pname,int (*pfunc)(lua_State *),const char*pnamespace);
	// 全部更新到LUA中
	void Flash();
	// 通用注册lua函数
	void InitLuaTest(lua_State * L,const char*pname,int (*pfunc)(lua_State *),const char*pnamespace,const char*pFileName = NULL);
	// 重新注册lua文件，以便保持最新
	void ReflashSrciptFile(const char*pFileName);
	// 对注册函数的使用说明
	void RegisterUsage(const char*pname,const char*pusage,const char*pnamespace);
	// 打印使用说明
	void PrintUsage(const char*pnamespace);
	// 调用lua中函数
	void Call(const char*pnamespace, const char* pfunc, abase::vector<CScriptValue>& args, abase::vector<CScriptValue>& results);
	// 添加API接口
	void AddAPI(LuaAPIBase*papi);
	// 统一注册所有API
	void RegisterAllAPI();
public:
	~LuaFuncFactory();
	static LuaFuncFactory& GetSingleton(); 
private:
	NamespaceLuaFuncMap m_funcMap;
	NamespaceLuaFuncUsageMap m_usageMap;// 对注册函数的使用说明，以便写lua者知道本系统到底注册了什么函数，以及其用法。
	LuaAPI m_apis;
};
