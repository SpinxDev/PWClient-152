/*
 * FILE: SessionManager.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Shizhenhua, 2012/8/28
 *
 * HISTORY: 
 *
 * Copyright (c) 2011 ACE Studio, All Rights Reserved.
 */


#include "Common.h"
#include "gameclient.h"
#include "SessionManager.h"
#include "gnconf.h"
#include "gnet.h"
#include "GameSession.h"
#include <AFI.h>


///////////////////////////////////////////////////////////////////////////
//  
//  Define and Macro
//  
///////////////////////////////////////////////////////////////////////////

const char* ERROR_CONFIGFILE = "找不到配置文件，网络初始化失败！";
const char* ERROR_SERVERERRORFILE = "找不到server_error.txt文件，网络初始化失败！";
const char* ERROR_SOCKETFAIL = "初始化SOCKET失败！";
const char* ERROR_NEEDTESTACCOUNT = "测试失败，给定的测试账号不足！";

#define SHOW_ERRORMSG(str) ::MessageBox(GetActiveWindow(), str, "错误", MB_OK|MB_ICONERROR);


///////////////////////////////////////////////////////////////////////////
//  
//  Types and Global variables
//  
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//  
//  Implement class LoginQueue
//  
///////////////////////////////////////////////////////////////////////////

LoginQueue& LoginQueue::GetSingleton()
{
	static LoginQueue lq;
	return lq;
}

// 添加一个登陆请求
bool LoginQueue::LoginServer(int zone_id, int line, const char* ip, int port)
{
	_Server server;
	ASSERT(ip && port);
	CsLockScoped lock(m_csServers);
	server.zone_id = zone_id;
	server.line = line;
	server.address = ip;
	server.port = port;
	m_Servers.push_back(server);
	return true;
}

// 更新队列
void LoginQueue::Tick()
{
	CsLockScoped lock(m_csServers);

	ServerList::iterator it = m_Servers.begin();
	while( it != m_Servers.end() )
	{
		_Server& svr = *it;
		if( SessionManager::GetSingleton().Login(svr.zone_id, svr.line, svr.address, svr.port) )
			it = m_Servers.erase(it);
		else
			++it;
	}
}


///////////////////////////////////////////////////////////////////////////
//  
//  Implement class SessionManager
//  
///////////////////////////////////////////////////////////////////////////

// 获取实例
SessionManager& SessionManager::GetSingleton()
{
	static SessionManager mgr;
	return mgr;
}

// 构造函数
SessionManager::SessionManager()
{
	m_pCollector = NULL;
	m_dwLastTime = GetTickCount();
	m_bTestCrossServer = false;
	m_bAllUserIsCool = false;
}

// 析构函数
SessionManager::~SessionManager()
{
}

// 初始化模拟客户端
// Param: 服务器错误信息文件
bool SessionManager::Init( const char* cfg_file )
{
	glb_InitLogSystem("logintest.log");

	if( access(cfg_file, R_OK) == -1 )
	{
		SHOW_ERRORMSG(ERROR_CONFIGFILE);
		a_LogOutput(1, "SessionManager::Init, Can't find the config file %s !", cfg_file);
		return false;
	}

	// 加载服务器错误描述
	if( !m_ErrorMsgs.Init("server_error.txt", true) )
	{
		SHOW_ERRORMSG(ERROR_SERVERERRORFILE);
		a_LogOutput(1, "SessionManager::Init, Can't find the server_error.txt!");
		return false;
	}

	// 初始化网络
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		SHOW_ERRORMSG(ERROR_SOCKETFAIL);
		a_LogOutput(1, "_InitNetwork, Cannot initialize Windows Sockets !");
		return false;
	}

	a_InitRandom();
	GNET::Conf* conf = GNET::Conf::GetInstance(cfg_file);
	GNET::PollIO::Init();

	return true;
}

// 释放会话管理器
void SessionManager::Release()
{
	GNET::PollIO::Close();
	Sleep(1000);

	if( CsLockScoped lock(m_csSessions) ) // 加锁
	{
		GameSessionMap::iterator it;
		for( it=m_NewSessions.begin();it!=m_NewSessions.end();++it )
		{
			if( it->second )
			{
				it->second->Close();
				delete it->second;
				it->second = NULL;
			}
		}

		for( it=m_Sessions.begin();it!=m_Sessions.end();++it )
		{
			if( it->second )
			{
				it->second->Close();
				delete it->second;
				it->second = NULL;
			}
		}

		m_NewSessions.clear();
		m_Sessions.clear();
	}

	m_ErrorMsgs.Release();
	WSACleanup();
	glb_CloseLogSystem();
}

// 客户端Tick
void SessionManager::Tick()
{
	DWORD dwCurTime = GetTickCount();
	DWORD dwFrameTime = dwCurTime - m_dwLastTime;
	m_dwLastTime = dwCurTime;

	// 更新服务器冷却时间
	UpdateCoolTime(dwFrameTime);

	// 更新可用账号
	UpdateCoolUser(dwFrameTime);

	// 更新登陆队列
	LoginQueue::GetSingleton().Tick();

	if( CsLockScoped lock(m_csSessions) ) // 加锁
	{
		// 处理已经连接的Session
		GameSessionMap::iterator it;
		for( it=m_Sessions.begin();it!=m_Sessions.end();++it )
		{
			GameSession* pSession = it->second;

			if( pSession )
			{
				pSession->Update(dwFrameTime);
				pSession->ProcessNewProtocols();
				pSession->ClearOldProtocols();

				// 完成测试，断开连接
				if( pSession->TestFinished())
					pSession->Close();
			}
		}

		// 处理尚未连接的Session
		for( it=m_NewSessions.begin();it!=m_NewSessions.end();++it )
		{
			if( it->second )
				it->second->Update(dwFrameTime);
		}

		// 处理过期的Session
		BrokenSession();
	}
}

// 添加一个测试账号
bool SessionManager::AddTestUser( const char* username,const char* password )
{
	ASSERT( username && password );

	TestUserList::iterator it;
	for( it=m_TestUsers.begin();it!=m_TestUsers.end();++it )
	{
		if( it->username == username )
			return false;
	}

	LoginUser user;
	user.username = username;
	user.password = password;
	user.loginCount = MAX_LOGINTEST_COUNT;
	m_TestUsers.push_back(user);
	return true;
}

// 删除一个测试账号
void SessionManager::RemoveTestUser( const char* username )
{
	ASSERT( username );

	TestUserList::iterator it;
	for( it=m_TestUsers.begin();it!=m_TestUsers.end();++it )
	{
		if( it->username == username )
		{
			m_TestUsers.erase(it);
			break;
		}
	}
}

// 登陆一个服务器
bool SessionManager::Login( int zone_id, int line, const char* ip, int port )
{
	ASSERT( ip && port );

	// 连接数达到最大值
	if( m_Sessions.size() + m_NewSessions.size() >= MAX_CLIENT_SESSION )
		return false;

	// 只允许不同服务器并行测试
	if( m_NewSessions.find(zone_id) != m_NewSessions.end() ||
		m_Sessions.find(zone_id) != m_Sessions.end() )
		return false;

	// 检测是否正在冷却
	if( IsServerCooling(zone_id) )
		return false;

	// 取末尾的用户名
	LoginUser user;
	if( !SelectLoginUser(user) )
	{
		if( !m_bTestCrossServer )
		{
			ConnectResult result;
			result.zone_id = zone_id;
			result.address = ip;
			result.port = port;
			result.result = TEST_ERROR;
			result.failReason = ERROR_NEEDTESTACCOUNT;
			m_pCollector->AddResult(result);
			return true;
		}
		else
		{
			// 跨服测试是用GM号，由于GM比较少，这里等待账号冷却
			return false;
		}
	}

	// 创建一个Session
	GameSession* pNewSession = new GameSession();
	pNewSession->SetUserName(user.username);
	pNewSession->SetUserPassword(user.password);
	pNewSession->SetOrgAddress(ip);
	pNewSession->SetServerPort(port);
	pNewSession->SetOrgZoneID(zone_id);
	pNewSession->SetServerLine(line);
	pNewSession->SetKickUserFlag(GNET::Conf::GetInstance()->find("GameClient", "force_login") == "true");

	if( CsLockScoped lock(m_csSessions) ) // 加锁
	{
		m_NewSessions[zone_id] = pNewSession;
	}

	// 开始连接新玩家
	return pNewSession->Open();
}

// 响应Session连接成功
void SessionManager::OnConnect( GameSession* pSession )
{
	ASSERT(pSession);
	if( CsLockScoped lock(m_csSessions) ) // 加锁
	{
		// 从待连接移到已连接
		GameSessionMap::iterator it;
		if( (it = m_NewSessions.find(pSession->GetOrgZoneID()))
			!= m_NewSessions.end() )
		{
			m_Sessions[it->first] = it->second;
			m_NewSessions.erase(it);
		}
	}
}

// 响应Session连接断开
void SessionManager::OnDisconnect( GameSession* pSession )
{
	ASSERT(pSession);
	if( CsLockScoped lock(m_csSessions) ) // 加锁
	{
		m_OldClients.push_back(pSession->GetNetManager());
	}
}

// 处理过期Session
void SessionManager::BrokenSession()
{
	for( size_t i=0;i<m_OldClients.size();i++ )
	{
		GameSession* pSession = (GameSession*)m_OldClients[i]->GetGameSession();
		if( !pSession ) continue;

		if( pSession->IsConnected() )
			pSession->Close();

		// 跨服重新登录
		if( pSession->IsWaitingRelogin() && pSession->Relogin() )
			continue;

		// 设置冷却时间
		m_ServerCools[pSession->GetOrgZoneID()] = atoi(GNET::Conf::GetInstance()->find("GameClient", "timeout").c_str());

		// 输出测试结果
		ConnectResult result;
		result.address = pSession->GetOrgAddress();
		result.port = pSession->GetServerPort();
		result.zone_id = pSession->GetOrgZoneID();
		result.line = pSession->GetServerLine();
		result.result = pSession->GetGameResult();
		result.failReason = pSession->GetBreakReason();
		result.versionErr = pSession->GetVersionError();
		result.dest_port = pSession->GetDestPort();
		result.is_on_cross = pSession->IsOnSpecialServer();
		pSession->GetQueryResults(result.queryResult);
		if( m_pCollector ) m_pCollector->AddResult(result);

		bool bFound = false;

		// 从活动列表中删除
		GameSessionMap::iterator it;
		if( (it = m_Sessions.find(pSession->GetOrgZoneID())) != m_Sessions.end() )
		{
			delete it->second;
			m_Sessions.erase(it);
			bFound = true;
		}

		if( !bFound )
		{
			// 删除正在连接中的Session
			if( (it = m_NewSessions.find(pSession->GetOrgZoneID())) != m_NewSessions.end() )
			{
				delete it->second;
				m_NewSessions.erase(it);
			}
		}
	}

	m_OldClients.clear();
}

// 更新冷却时间
void SessionManager::UpdateCoolTime(DWORD dwFrameTime)
{
	ServerCooltime::iterator ct;
	for( ct=m_ServerCools.begin();ct!=m_ServerCools.end();++ct )
	{
		if( ct->second > 0 )
		{
			if( ct->second > dwFrameTime )
				ct->second -= dwFrameTime;
			else
				ct->second = 0;
		}
	}
}

// 检测指定服务器是否正在冷却
bool SessionManager::IsServerCooling( int iZoneID ) const
{
	ServerCooltime::const_iterator it = m_ServerCools.find(iZoneID);
	return (it != m_ServerCools.end()) ? (it->second > 0) : false;
}

// 更新冷却中的账号
void SessionManager::UpdateCoolUser( DWORD dwFrameTime )
{
	TestUserList::iterator it;
	for(it=m_TestUsers.begin(); it!=m_TestUsers.end(); ++it)
	{
		if( it->loginCount > 0 )
			continue;

		if( it->waittime <= dwFrameTime )
		{
			it->waittime = 0;
			it->loginCount = MAX_LOGINTEST_COUNT;
			m_bAllUserIsCool = false;
		}
		else
		{
			it->waittime -= dwFrameTime;
		}
	}
}

// 选取一个可用的账号
bool SessionManager::SelectLoginUser(LoginUser& user)
{
	TestUserList::iterator it;
	for(it=m_TestUsers.begin(); it!=m_TestUsers.end(); ++it)
	{
		if( it->loginCount > 0 )
		{
			user = *it;
			if( --(it->loginCount) <= 0 )
				it->waittime = MAX_COOLUSER_WAITTIME;
			return true;
		}
	}

	m_bAllUserIsCool = true;
	return false;
}


///////////////////////////////////////////////////////////////////////////
//  
//  对外接口
//  
///////////////////////////////////////////////////////////////////////////

extern char g_szWorkDir[];
extern char g_szIniFile[];

// 当前程序是否运行
volatile bool g_bGameRun = true;

// 逻辑线程句柄
HANDLE g_hThreadLogic = NULL;

/** 跑客户端逻辑的线程
*/
DWORD WINAPI LogicTickThread( LPVOID lpParameter )
{
#ifndef _NOMINIDUMP
	__try
	{
#endif

	SessionManager& clientMgr = SessionManager::GetSingleton();
	if( !clientMgr.Init(g_szIniFile) )
		return 0;

	while( g_bGameRun )
	{
		clientMgr.Tick();

		Sleep(50);
	}

	clientMgr.Release();

#ifndef _NOMINIDUMP
	}
	__except(glb_HandleException(GetExceptionInformation()))
	{
		::OutputDebugString("Exception occurred...\n");
		a_LogOutput(1, "Exception occurred in Logic tick thread... mini dumped!");
		::ExitProcess(-1);
	}
#endif

	return 1;
}


// 初始化网络环境
bool InitNetwork(const char* cfg_file)
{
	DWORD dwThreadID;
	g_csException = new CsMutex;
	a_InitRandom();
	af_Initialize();
	GetCurrentDirectory(MAX_PATH, g_szWorkDir);
	sprintf(g_szIniFile, "%s\\%s", g_szWorkDir, cfg_file);
	af_SetBaseDir(g_szWorkDir);

	// 获取客户端版本信息
	GNET::Conf::GetInstance(g_szIniFile);
	GetClientVersion();

	// 开启逻辑线程
	g_hThreadLogic = CreateThread(NULL, 0, LogicTickThread, NULL, 0, &dwThreadID);
	return true;
}

// 登陆一个服务器
bool LoginServer(int zone_id, int line, const char* ip, int port)
{
	return LoginQueue::GetSingleton().LoginServer(zone_id, line, ip, port);
}

// 设置一个结果收集器，开始测试
void SetResultCollector(ResultCollector* pCollector)
{
	SessionManager::GetSingleton().SetResultCollector(pCollector);
}

// 设置跨服测试标记
void SetTestCrossServer(bool bTest)
{
	SessionManager::GetSingleton().SetTestCrossServer(bTest);
}

// 所有账号正在冷却
bool IsAllUserCooldown()
{
	return SessionManager::GetSingleton().IsAllUserCool();
}

// 添加一个测试账号
bool AddTestUser(const char* user, const char* password)
{
	return SessionManager::GetSingleton().AddTestUser(user, password);
}

// 清除所有测试账号
void ClearTestUser()
{
	SessionManager::GetSingleton().RemoveAllUsers();
}

// 结束网络环境
void Finalize()
{
	g_bGameRun = false;
	WaitForSingleObject(g_hThreadLogic, INFINITE);
	CloseHandle(g_hThreadLogic);
	af_Finalize();
	delete g_csException;
}