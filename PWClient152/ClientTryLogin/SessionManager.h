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

#define MAX_CLIENT_SESSION 50			// ���ͻ���������
#define MAX_LOGINTEST_COUNT 20			// �����˺�10����֮�����ĵ�¼����
#define MAX_COOLUSER_WAITTIME 600000	// �˺ŵ�½�����������ȴ�10����
#define MAX_SERVER_COOLTIME 3000		// ������½һ������������ȴʱ��Ϊ3��


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
	TEST_SUCCESSFUL = 0x0001,				// ���ӳɹ�
	TEST_FAIL		= 0x0002,				// ����ʧ��
	TEST_ERROR		= 0x0004,				// ����������Ϣ
	TEST_VERSION_ERR= 0x0008,				// �汾��ƥ��

	TEST_COUNTRYWAR_BONUS_ERR = 0x0010,		// ��ս�ܽ���������
	TEST_CROSSSERVER_ERR = 0x0020,			// ����������쳣
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

// ���ӽ��
struct ConnectResult
{
	int zone_id;			// ����������
	int line;				// �ߣ����Ż���ͨ��
	AString address;		// ��������ַ
	int port;				// �˿�
	int dest_port;			// ����˿�
	bool is_on_cross;		// �Ƿ��ڿ����
	unsigned int result;	// ���Խ��
	AString failReason;		// ʧ��ԭ��
	AString versionErr;		// �汾�Ƚ�
	AString queryResult;	// ������ѯ���
};

typedef abase::vector<ConnectResult> ConnectResults;

// �����ռ����ӽ������
class ResultCollector
{
public:

	// ���һ�����ӽ��
	void AddResult( const ConnectResult& result )
	{
		CsLockScoped lock(m_csContext);
		m_Context.push_back(result);
	}

	// ��ȡ���ӽ���ĸ���
	size_t GetResultCount() const
	{
		CsLockScoped lock(m_csContext);
		return m_Context.size();
	}

	// ��ȡָ�����
	void GetResult(size_t idx, ConnectResult& ret) const
	{
		ASSERT(idx < m_Context.size());
		CsLockScoped lock(m_csContext);
		ret = m_Context[idx];
	}

	// ����ռ����Ľ��
	void ClearResults(int beg, int end)
	{
		CsLockScoped lock(m_csContext);
		ConnectResults::iterator it1 = m_Context.begin() + beg;
		ConnectResults::iterator it2 = m_Context.begin() + end;
		m_Context.erase(it1, it2);
	}

protected:
	ConnectResults m_Context;		// ���Խ��
	mutable CsMutex m_csContext;	// �Ӹ���
};


///////////////////////////////////////////////////////////////////////////
//  
//  class LoginQueue
//  
///////////////////////////////////////////////////////////////////////////

// ��½����
// ��ͬ��������ַ���в��ԣ���ͬ��ַ��ͬ�˿ڴ��в���
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
		int loginCount;		// ��½����
		DWORD waittime;		// ��½���������ĵȴ�ʱ��
	};

	typedef abase::vector<LoginUser> TestUserList;
	typedef abase::hash_map<int,DWORD> ServerCooltime;

public:
	~SessionManager();

	bool Init( const char* cfg_file );
	void Release();

	void Tick();

	// ���ò����˺ţ����ڷ�����½����֮ǰ���
	bool AddTestUser( const char* username,const char* password );
	void RemoveTestUser( const char* username );
	size_t GetTestUserCount() const { return m_TestUsers.size(); }
	void RemoveAllUsers() { m_TestUsers.clear(); }

	// ���ý��ܽ��������
	void SetResultCollector( ResultCollector* pCollector )
	{
		m_pCollector = pCollector;
	}

	// ���Ե�½һ��������
	bool Login(int zone_id, int line, const char* ip, int port);

	// ��Ӧ���ӻ�Ͽ�
	void OnConnect( GameSession* pSession );
	void OnDisconnect( GameSession* pSession );

	// ���ÿ�����
	void SetTestCrossServer(bool bFlag) { m_bTestCrossServer = bFlag; }
	// �Ƿ���Կ��
	bool IsTestCrossServer() const { return m_bTestCrossServer; }

	// �����˺Ŷ�����ȴ
	bool IsAllUserCool() const { return m_bAllUserIsCool; }

	// ��ȡ��������������
	CECStringTab& GetErrorMsgs() { return m_ErrorMsgs; }

	static SessionManager& GetSingleton();

protected:
	GameSessionMap m_NewSessions;	// �ȴ����ӵ�Session
	OldClientList m_OldClients;		// �Ѿ����ڵ�Session, ���Ͽ���������ʧ��
	GameSessionMap m_Sessions;		// ���ӳɹ���Session
	CsMutex m_csSessions;			// ���⼸��Session��������

	DWORD m_dwLastTime;				// ��һ֡��ʱ��
	CECStringTab m_ErrorMsgs;		// ������������Ϣ����
	ResultCollector* m_pCollector;	// ���Խ��������
	TestUserList m_TestUsers;		// �����˺��б�ÿ���˺�10����֮��ֻ�ܵ�½20��
	ServerCooltime m_ServerCools;	// ��½��ȴʱ�䣬��ֹ������½ͬһ��������

	bool m_bTestCrossServer;		// �Ƿ���Կ��
	bool m_bAllUserIsCool;			// �Ƿ������˺ŵ�½����������ȴ

protected:
	SessionManager();

	// ������ڵ�Session
	void BrokenSession();

	// ���·�������ȴʱ��
	void UpdateCoolTime( DWORD dwFrameTime );

	// ������ȴ�е��û�
	void UpdateCoolUser( DWORD dwFrameTime );

	// ָ���������Ƿ�����ȴ
	bool IsServerCooling( int zone_id ) const;

	// ѡȡһ�����õ��˺�
	bool SelectLoginUser(LoginUser& user);
};


///////////////////////////////////////////////////////////////////////////
//  
//  ����ӿ�
//  
///////////////////////////////////////////////////////////////////////////

// ��ʼ�����绷��
bool InitNetwork(const char* cfg_file);
// ���ò����˺�
bool AddTestUser(const char* user, const char* password);
// ������в����˺�
void ClearTestUser();
// ���ý���ռ�
void SetResultCollector(ResultCollector* pCollector);
// �Ƿ���Կ��
void SetTestCrossServer(bool bTest);
// �����˺�������ȴ
bool IsAllUserCooldown();
// ��½һ��ָ���ķ�����
bool LoginServer(int zone_id, int line, const char* ip, int port);
// �������绷��
void Finalize();


///////////////////////////////////////////////////////////////////////////
//  
//  Inline functions
//  
///////////////////////////////////////////////////////////////////////////
