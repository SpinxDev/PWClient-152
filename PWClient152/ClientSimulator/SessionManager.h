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
#include "MsgManager.h"
#include "EC_StringTab.h"
#include "cslock.h"


///////////////////////////////////////////////////////////////////////////
//  
//  Define and Macro
//  
///////////////////////////////////////////////////////////////////////////

#define MAX_CLIENT_SESSION 64	// ���ͻ���������


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
//  class SessionManager
//  
///////////////////////////////////////////////////////////////////////////

class SessionManager
{
public:
	typedef abase::hash_map<AString,GameSession*> GameSessionMap;
	typedef abase::vector<GNET::GameClient*> OldClientList;

public:
	~SessionManager();

	bool Init( const char* cfg_file );
	void Release();

	void Tick();

	// ���һ�����
	bool AddPlayer( const char* username,const char* password );
	// ע��һ�����
	void RemovePlayer( const char* username );

	// ��Ӧ���ӻ�Ͽ�
	void OnConnect( GameSession* pSession );
	void OnDisconnect( GameSession* pSession );

	// ������Ϣ����
	void ProcessMessage( MESSAGE* pMsg );

	// ö�������������
	void EnumPlayers( StringVector& usernames );

	// ��ȡָ��Session
	GameSession* GetSessionByName(const char* username);

	void SetTheSession(const AString& username);
	void SetTheSession(GameSession* pSession);

	GameSession* GetTheSession() { return m_theSession; }
	CECStringTab& GetErrorMsgs() { return m_ErrorMsgs; }
	elementdataman* GetDataMan() { return m_pDataMan; }

	static SessionManager& GetSingleton();

protected:
	GameSessionMap m_NewSessions;	// �ȴ������ӵ�Session
	OldClientList m_OldClients;		// �Ѿ����ڵ�Session, ���Ͽ���������ʧ��
	GameSessionMap m_SessionMap;	// ���ӳɹ���Session

	DWORD m_dwLastTime;				// ��һ֡��ʱ��
	CECStringTab m_ErrorMsgs;		// ������������Ϣ����
	elementdataman* m_pDataMan;		// ElementData

	CsMutex m_csSessions;
	GameSession* m_theSession;		// �Session, c2s_��غ��������õ�Session

protected:
	SessionManager();

	// ������ڵ�Session
	void BrokenSession();

	///////////////////////////////////////////////////////////////////////////
	// Message Handlers

	void OnMsgLogin( MESSAGE* pMsg );
	void OnMsgLogout( MESSAGE* pMsg );
	void OnMsgReloginAll( MESSAGE* pMsg );
	void OnMsgC2SCommand( MESSAGE* pMsg );
	void OnMsgPlayerMove( MESSAGE* pMsg );
	void OnMsgHostPlayerAI( MESSAGE* pMsg );
	void OnShowPos( MESSAGE* pMsg );
	void OnLuaCall( MESSAGE* pMsg );
	void OnMsgForceAttack( MESSAGE* pMsg );
	void OnMsgSetRegion( MESSAGE* pMsg );
	void OnMsgAICommand( MESSAGE* pMsg );
	void OnMsgShowInfo( MESSAGE* pMsg );

	// ����debug����
	void DealMsgC2SCommand(const StringVector&cmdParams);
};

///////////////////////////////////////////////////////////////////////////
//  
//  Inline functions
//  
///////////////////////////////////////////////////////////////////////////
