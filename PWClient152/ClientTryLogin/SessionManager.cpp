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

const char* ERROR_CONFIGFILE = "�Ҳ��������ļ��������ʼ��ʧ�ܣ�";
const char* ERROR_SERVERERRORFILE = "�Ҳ���server_error.txt�ļ��������ʼ��ʧ�ܣ�";
const char* ERROR_SOCKETFAIL = "��ʼ��SOCKETʧ�ܣ�";
const char* ERROR_NEEDTESTACCOUNT = "����ʧ�ܣ������Ĳ����˺Ų��㣡";

#define SHOW_ERRORMSG(str) ::MessageBox(GetActiveWindow(), str, "����", MB_OK|MB_ICONERROR);


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

// ���һ����½����
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

// ���¶���
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

// ��ȡʵ��
SessionManager& SessionManager::GetSingleton()
{
	static SessionManager mgr;
	return mgr;
}

// ���캯��
SessionManager::SessionManager()
{
	m_pCollector = NULL;
	m_dwLastTime = GetTickCount();
	m_bTestCrossServer = false;
	m_bAllUserIsCool = false;
}

// ��������
SessionManager::~SessionManager()
{
}

// ��ʼ��ģ��ͻ���
// Param: ������������Ϣ�ļ�
bool SessionManager::Init( const char* cfg_file )
{
	glb_InitLogSystem("logintest.log");

	if( access(cfg_file, R_OK) == -1 )
	{
		SHOW_ERRORMSG(ERROR_CONFIGFILE);
		a_LogOutput(1, "SessionManager::Init, Can't find the config file %s !", cfg_file);
		return false;
	}

	// ���ط�������������
	if( !m_ErrorMsgs.Init("server_error.txt", true) )
	{
		SHOW_ERRORMSG(ERROR_SERVERERRORFILE);
		a_LogOutput(1, "SessionManager::Init, Can't find the server_error.txt!");
		return false;
	}

	// ��ʼ������
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

// �ͷŻỰ������
void SessionManager::Release()
{
	GNET::PollIO::Close();
	Sleep(1000);

	if( CsLockScoped lock(m_csSessions) ) // ����
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

// �ͻ���Tick
void SessionManager::Tick()
{
	DWORD dwCurTime = GetTickCount();
	DWORD dwFrameTime = dwCurTime - m_dwLastTime;
	m_dwLastTime = dwCurTime;

	// ���·�������ȴʱ��
	UpdateCoolTime(dwFrameTime);

	// ���¿����˺�
	UpdateCoolUser(dwFrameTime);

	// ���µ�½����
	LoginQueue::GetSingleton().Tick();

	if( CsLockScoped lock(m_csSessions) ) // ����
	{
		// �����Ѿ����ӵ�Session
		GameSessionMap::iterator it;
		for( it=m_Sessions.begin();it!=m_Sessions.end();++it )
		{
			GameSession* pSession = it->second;

			if( pSession )
			{
				pSession->Update(dwFrameTime);
				pSession->ProcessNewProtocols();
				pSession->ClearOldProtocols();

				// ��ɲ��ԣ��Ͽ�����
				if( pSession->TestFinished())
					pSession->Close();
			}
		}

		// ������δ���ӵ�Session
		for( it=m_NewSessions.begin();it!=m_NewSessions.end();++it )
		{
			if( it->second )
				it->second->Update(dwFrameTime);
		}

		// ������ڵ�Session
		BrokenSession();
	}
}

// ���һ�������˺�
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

// ɾ��һ�������˺�
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

// ��½һ��������
bool SessionManager::Login( int zone_id, int line, const char* ip, int port )
{
	ASSERT( ip && port );

	// �������ﵽ���ֵ
	if( m_Sessions.size() + m_NewSessions.size() >= MAX_CLIENT_SESSION )
		return false;

	// ֻ����ͬ���������в���
	if( m_NewSessions.find(zone_id) != m_NewSessions.end() ||
		m_Sessions.find(zone_id) != m_Sessions.end() )
		return false;

	// ����Ƿ�������ȴ
	if( IsServerCooling(zone_id) )
		return false;

	// ȡĩβ���û���
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
			// �����������GM�ţ�����GM�Ƚ��٣�����ȴ��˺���ȴ
			return false;
		}
	}

	// ����һ��Session
	GameSession* pNewSession = new GameSession();
	pNewSession->SetUserName(user.username);
	pNewSession->SetUserPassword(user.password);
	pNewSession->SetOrgAddress(ip);
	pNewSession->SetServerPort(port);
	pNewSession->SetOrgZoneID(zone_id);
	pNewSession->SetServerLine(line);
	pNewSession->SetKickUserFlag(GNET::Conf::GetInstance()->find("GameClient", "force_login") == "true");

	if( CsLockScoped lock(m_csSessions) ) // ����
	{
		m_NewSessions[zone_id] = pNewSession;
	}

	// ��ʼ���������
	return pNewSession->Open();
}

// ��ӦSession���ӳɹ�
void SessionManager::OnConnect( GameSession* pSession )
{
	ASSERT(pSession);
	if( CsLockScoped lock(m_csSessions) ) // ����
	{
		// �Ӵ������Ƶ�������
		GameSessionMap::iterator it;
		if( (it = m_NewSessions.find(pSession->GetOrgZoneID()))
			!= m_NewSessions.end() )
		{
			m_Sessions[it->first] = it->second;
			m_NewSessions.erase(it);
		}
	}
}

// ��ӦSession���ӶϿ�
void SessionManager::OnDisconnect( GameSession* pSession )
{
	ASSERT(pSession);
	if( CsLockScoped lock(m_csSessions) ) // ����
	{
		m_OldClients.push_back(pSession->GetNetManager());
	}
}

// �������Session
void SessionManager::BrokenSession()
{
	for( size_t i=0;i<m_OldClients.size();i++ )
	{
		GameSession* pSession = (GameSession*)m_OldClients[i]->GetGameSession();
		if( !pSession ) continue;

		if( pSession->IsConnected() )
			pSession->Close();

		// ������µ�¼
		if( pSession->IsWaitingRelogin() && pSession->Relogin() )
			continue;

		// ������ȴʱ��
		m_ServerCools[pSession->GetOrgZoneID()] = atoi(GNET::Conf::GetInstance()->find("GameClient", "timeout").c_str());

		// ������Խ��
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

		// �ӻ�б���ɾ��
		GameSessionMap::iterator it;
		if( (it = m_Sessions.find(pSession->GetOrgZoneID())) != m_Sessions.end() )
		{
			delete it->second;
			m_Sessions.erase(it);
			bFound = true;
		}

		if( !bFound )
		{
			// ɾ�����������е�Session
			if( (it = m_NewSessions.find(pSession->GetOrgZoneID())) != m_NewSessions.end() )
			{
				delete it->second;
				m_NewSessions.erase(it);
			}
		}
	}

	m_OldClients.clear();
}

// ������ȴʱ��
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

// ���ָ���������Ƿ�������ȴ
bool SessionManager::IsServerCooling( int iZoneID ) const
{
	ServerCooltime::const_iterator it = m_ServerCools.find(iZoneID);
	return (it != m_ServerCools.end()) ? (it->second > 0) : false;
}

// ������ȴ�е��˺�
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

// ѡȡһ�����õ��˺�
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
//  ����ӿ�
//  
///////////////////////////////////////////////////////////////////////////

extern char g_szWorkDir[];
extern char g_szIniFile[];

// ��ǰ�����Ƿ�����
volatile bool g_bGameRun = true;

// �߼��߳̾��
HANDLE g_hThreadLogic = NULL;

/** �ܿͻ����߼����߳�
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


// ��ʼ�����绷��
bool InitNetwork(const char* cfg_file)
{
	DWORD dwThreadID;
	g_csException = new CsMutex;
	a_InitRandom();
	af_Initialize();
	GetCurrentDirectory(MAX_PATH, g_szWorkDir);
	sprintf(g_szIniFile, "%s\\%s", g_szWorkDir, cfg_file);
	af_SetBaseDir(g_szWorkDir);

	// ��ȡ�ͻ��˰汾��Ϣ
	GNET::Conf::GetInstance(g_szIniFile);
	GetClientVersion();

	// �����߼��߳�
	g_hThreadLogic = CreateThread(NULL, 0, LogicTickThread, NULL, 0, &dwThreadID);
	return true;
}

// ��½һ��������
bool LoginServer(int zone_id, int line, const char* ip, int port)
{
	return LoginQueue::GetSingleton().LoginServer(zone_id, line, ip, port);
}

// ����һ������ռ�������ʼ����
void SetResultCollector(ResultCollector* pCollector)
{
	SessionManager::GetSingleton().SetResultCollector(pCollector);
}

// ���ÿ�����Ա��
void SetTestCrossServer(bool bTest)
{
	SessionManager::GetSingleton().SetTestCrossServer(bTest);
}

// �����˺�������ȴ
bool IsAllUserCooldown()
{
	return SessionManager::GetSingleton().IsAllUserCool();
}

// ���һ�������˺�
bool AddTestUser(const char* user, const char* password)
{
	return SessionManager::GetSingleton().AddTestUser(user, password);
}

// ������в����˺�
void ClearTestUser()
{
	SessionManager::GetSingleton().RemoveAllUsers();
}

// �������绷��
void Finalize()
{
	g_bGameRun = false;
	WaitForSingleObject(g_hThreadLogic, INFINITE);
	CloseHandle(g_hThreadLogic);
	af_Finalize();
	delete g_csException;
}