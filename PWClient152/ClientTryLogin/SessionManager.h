/*
 * FILE: SessionManager.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Shizhenhua, 2012/8/28
 *
 * HISTORY: 
 *
 * Copyright (c) 2011 ACE Studio, All Rights Reserved.
 */


#pragma once

#include <ABaseDef.h>
#include <vector.h>
#include <hashmap.h>
#include <AString.h>
#include "EC_StringTab.h"
#include "cslock.h"


///////////////////////////////////////////////////////////////////////////
//  
//  Define and Macro
//  
///////////////////////////////////////////////////////////////////////////

#define MAX_CLIENT_SESSION 50			// 最大客户端连接数
#define MAX_LOGINTEST_COUNT 20			// 单个账号10分钟之内最大的登录次数
#define MAX_COOLUSER_WAITTIME 600000	// 账号登陆次数用完后需等待10分钟
#define MAX_SERVER_COOLTIME 3000		// 连续登陆一个服务器的冷却时间为3秒


///////////////////////////////////////////////////////////////////////////
//  
//  Types and Global variables
//  
///////////////////////////////////////////////////////////////////////////

class GameSession;
class elementdataman;

namespace GNET {
	class GameClient;
}


///////////////////////////////////////////////////////////////////////////
//  
//  class ResultCollector
//  
///////////////////////////////////////////////////////////////////////////

enum
{
	TEST_SUCCESSFUL = 0x0001,				// 连接成功
	TEST_FAIL		= 0x0002,				// 连接失败
	TEST_ERROR		= 0x0004,				// 其他错误信息
	TEST_VERSION_ERR= 0x0008,				// 版本不匹配

	TEST_COUNTRYWAR_BONUS_ERR = 0x0010,		// 国战总奖励不正常
	TEST_CROSSSERVER_ERR = 0x0020,			// 跨服过程中异常
};
inline bool IsTestAllRight(unsigned int result)
{
	return result == TEST_SUCCESSFUL;
}
inline bool IsTestError(unsigned int result)
{
	return result == TEST_ERROR;
}
inline bool IsTestFail(unsigned int result)
{
	return result == TEST_FAIL;
}
inline bool IsTestInWorld(unsigned int result)
{
	return (result & TEST_SUCCESSFUL) != 0;
}
inline bool IsTestVersionErr(unsigned int result)
{
	return (result & TEST_VERSION_ERR) != 0;
}
inline bool IsTestCountryWarBonusErr(unsigned int result)
{
	return (result & TEST_COUNTRYWAR_BONUS_ERR) != 0;
}
inline bool IsTestCrossServerErr(unsigned int result)
{
	return (result & TEST_CROSSSERVER_ERR) != 0;
}
inline bool IsTestInWorldWithError(unsigned int result)
{
	return IsTestInWorld(result) && (IsTestVersionErr(result) || IsTestCountryWarBonusErr(result) || IsTestCrossServerErr(result));
}

// 连接结果
struct ConnectResult
{
	int zone_id;			// 服务器区域
	int line;				// 线（电信或网通）
	AString address;		// 服务器地址
	int port;				// 端口
	int dest_port;			// 跨服端口
	bool is_on_cross;		// 是否在跨服上
	unsigned int result;	// 测试结果
	AString failReason;		// 失败原因
	AString versionErr;		// 版本比较
	AString queryResult;	// 其它查询结果
};

typedef abase::vector<ConnectResult> ConnectResults;

// 用于收集连接结果的类
class ResultCollector
{
public:

	// 添加一个连接结果
	void AddResult( const ConnectResult& result )
	{
		CsLockScoped lock(m_csContext);
		m_Context.push_back(result);
	}

	// 获取连接结果的个数
	size_t GetResultCount() const
	{
		CsLockScoped lock(m_csContext);
		return m_Context.size();
	}

	// 获取指定结果
	void GetResult(size_t idx, ConnectResult& ret) const
	{
		ASSERT(idx < m_Context.size());
		CsLockScoped lock(m_csContext);
		ret = m_Context[idx];
	}

	// 清除收集到的结果
	void ClearResults(int beg, int end)
	{
		CsLockScoped lock(m_csContext);
		ConnectResults::iterator it1 = m_Context.begin() + beg;
		ConnectResults::iterator it2 = m_Context.begin() + end;
		m_Context.erase(it1, it2);
	}

protected:
	ConnectResults m_Context;		// 测试结果
	mutable CsMutex m_csContext;	// 加个锁
};


///////////////////////////////////////////////////////////////////////////
//  
//  class LoginQueue
//  
///////////////////////////////////////////////////////////////////////////

// 登陆队列
// 不同服务器地址并行测试，相同地址不同端口串行测试
class LoginQueue
{
	struct _Server
	{
		int zone_id;
		int line;
		AString address;
		int port;
	};

	typedef abase::vector<_Server> ServerList;

public:

	bool LoginServer(int zone_id, int line, const char* ip, int port);
	void Tick();

	static LoginQueue& GetSingleton();

protected:
	ServerList m_Servers;
	CsMutex m_csServers;
};


///////////////////////////////////////////////////////////////////////////
//  
//  class SessionManager
//  
///////////////////////////////////////////////////////////////////////////

class SessionManager
{
public:

	typedef abase::hash_map<int,GameSession*> GameSessionMap;
	typedef abase::vector<GNET::GameClient*> OldClientList;

	struct LoginUser
	{
		AString username;
		AString password;
		int loginCount;		// 登陆次数
		DWORD waittime;		// 登陆次数用完后的等待时间
	};

	typedef abase::vector<LoginUser> TestUserList;
	typedef abase::hash_map<int,DWORD> ServerCooltime;

public:
	~SessionManager();

	bool Init( const char* cfg_file );
	void Release();

	void Tick();

	// 设置测试账号，需在发出登陆请求之前设好
	bool AddTestUser( const char* username,const char* password );
	void RemoveTestUser( const char* username );
	size_t GetTestUserCount() const { return m_TestUsers.size(); }
	void RemoveAllUsers() { m_TestUsers.clear(); }

	// 设置接受结果的容器
	void SetResultCollector( ResultCollector* pCollector )
	{
		m_pCollector = pCollector;
	}

	// 测试登陆一个服务器
	bool Login(int zone_id, int line, const char* ip, int port);

	// 响应链接或断开
	void OnConnect( GameSession* pSession );
	void OnDisconnect( GameSession* pSession );

	// 设置跨服标记
	void SetTestCrossServer(bool bFlag) { m_bTestCrossServer = bFlag; }
	// 是否测试跨服
	bool IsTestCrossServer() const { return m_bTestCrossServer; }

	// 所有账号都在冷却
	bool IsAllUserCool() const { return m_bAllUserIsCool; }

	// 获取服务器错误描述
	CECStringTab& GetErrorMsgs() { return m_ErrorMsgs; }

	static SessionManager& GetSingleton();

protected:
	GameSessionMap m_NewSessions;	// 等待连接的Session
	OldClientList m_OldClients;		// 已经过期的Session, 被断开或者连接失败
	GameSessionMap m_Sessions;		// 连接成功的Session
	CsMutex m_csSessions;			// 对这几个Session容器加锁

	DWORD m_dwLastTime;				// 上一帧的时间
	CECStringTab m_ErrorMsgs;		// 服务器错误信息描述
	ResultCollector* m_pCollector;	// 测试结果接受器
	TestUserList m_TestUsers;		// 测试账号列表，每个账号10分钟之内只能登陆20次
	ServerCooltime m_ServerCools;	// 登陆冷却时间，防止连续登陆同一个服务器

	bool m_bTestCrossServer;		// 是否测试跨服
	bool m_bAllUserIsCool;			// 是否所有账号登陆次数都在冷却

protected:
	SessionManager();

	// 处理过期的Session
	void BrokenSession();

	// 更新服务器冷却时间
	void UpdateCoolTime( DWORD dwFrameTime );

	// 更新冷却中的用户
	void UpdateCoolUser( DWORD dwFrameTime );

	// 指定服务器是否在冷却
	bool IsServerCooling( int zone_id ) const;

	// 选取一个可用的账号
	bool SelectLoginUser(LoginUser& user);
};


///////////////////////////////////////////////////////////////////////////
//  
//  对外接口
//  
///////////////////////////////////////////////////////////////////////////

// 初始化网络环境
bool InitNetwork(const char* cfg_file);
// 设置测试账号
bool AddTestUser(const char* user, const char* password);
// 清除所有测试账号
void ClearTestUser();
// 设置结果收集
void SetResultCollector(ResultCollector* pCollector);
// 是否测试跨服
void SetTestCrossServer(bool bTest);
// 所有账号正在冷却
bool IsAllUserCooldown();
// 登陆一个指定的服务器
bool LoginServer(int zone_id, int line, const char* ip, int port);
// 结束网络环境
void Finalize();


///////////////////////////////////////////////////////////////////////////
//  
//  Inline functions
//  
///////////////////////////////////////////////////////////////////////////
