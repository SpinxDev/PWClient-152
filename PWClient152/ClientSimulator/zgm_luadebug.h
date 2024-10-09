#pragma once
#include "LuaFunc.h"
#include <windows.h>
#include <string>
#include <map>
#include <vector>
#include <list>

// 同步互斥类
class ZgmMutex
{
public:
    ZgmMutex()
    {// 初始化临界区
        ::InitializeCriticalSection(&mtx);
    }
    ~ZgmMutex()
    {// 删除临界区
        ::DeleteCriticalSection(&mtx);
    }
    void Lock()
    {// 进入临界区
        ::EnterCriticalSection(&mtx);
    }
    void UnLock()
    {// 离开临界区
        ::LeaveCriticalSection(&mtx);
    }
private:
    ZgmMutex(const ZgmMutex &);
    ZgmMutex & operator = (const ZgmMutex &);
    CRITICAL_SECTION mtx;
};

class ZgmLockScoped
{
public:
    explicit ZgmLockScoped(ZgmMutex& ht) : host(&ht)
    {
        host->Lock();
    }
    explicit ZgmLockScoped(ZgmMutex* ht) : host(ht)
    {
        host->Lock();
    }
    ~ZgmLockScoped()
    {
        host->UnLock();
    }
private:
    ZgmLockScoped();
    ZgmLockScoped(const ZgmLockScoped&);
    ZgmLockScoped& operator=(const ZgmLockScoped&);
    ZgmMutex* host;
};

class ZgmLuaDebug
{
public:
	typedef std::vector<std::string> SplitStrings;
	typedef std::list<std::string> SendStrings;
	typedef std::vector<int> BreakPoints;
	typedef std::map<std::string,BreakPoints > BreakPointMap;
	typedef std::map<std::string,bool> WatchVars;
	typedef std::map<std::string,int> StackLevels;
	typedef std::map<std::string,int> StepLevels;
	typedef std::map<std::string,bool> FileSends;
	// 初始化lua debug
	void ZgmLuaDebugInit(lua_State *L);
	// 释放lua debug
	void ZgmLuaDebugRelease();
	// 获得单例
	static ZgmLuaDebug& GetSingleton();
	static DWORD WINAPI ThreadRun(void*p);
protected:
	void SplitString(const std::string& str,SplitStrings&splits,const char*sep);
	std::string GetVarValue(const std::string& varname);
public:
	void SetBreakPoint(const char* filename,const BreakPoints& bps);
	void SetWatch(const char* varname,bool bUnWatch);
	void SetState(const char* state);
	void SetSendStrings(const SendStrings& strs);

	void GetBreakPoint(const char*filename,BreakPoints& bps);
	void GetWatch(WatchVars& wvars);
	const char*GetDebugState();

	void DebugHook(const std::string&filename,int line,const std::string&evt);
	int DebugCanContinue();
private:
	ZgmLuaDebug();
	const char* m_filesend;
	const char* m_filesflag;
	const char* m_filerecv;
	const char* m_filerflag;
	ZgmMutex      m_csMutex;
	bool          m_bRunThread;
	BreakPointMap m_bpMaps;
	WatchVars     m_watchs;
	std::string   m_debugState;
	SendStrings   m_sendStrings;
	StackLevels   m_stack_levels;
	StepLevels    m_step_levels;
	lua_State *   m_L;
	FileSends     m_fileSends;
	int           m_debugContinue;
};