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

#define MAX_CLIENT_SESSION 64	// 最大客户端连接数


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

	// 添加一个玩家
	bool AddPlayer( const char* username,const char* password );
	// 注销一个玩家
	void RemovePlayer( const char* username );

	// 响应链接或断开
	void OnConnect( GameSession* pSession );
	void OnDisconnect( GameSession* pSession );

	// 处理消息队列
	void ProcessMessage( MESSAGE* pMsg );

	// 枚举所有在线玩家
	void EnumPlayers( StringVector& usernames );

	// 获取指定Session
	GameSession* GetSessionByName(const char* username);

	void SetTheSession(const AString& username);
	void SetTheSession(GameSession* pSession);

	GameSession* GetTheSession() { return m_theSession; }
	CECStringTab& GetErrorMsgs() { return m_ErrorMsgs; }
	elementdataman* GetDataMan() { return m_pDataMan; }

	static SessionManager& GetSingleton();

protected:
	GameSessionMap m_NewSessions;	// 等待待连接的Session
	OldClientList m_OldClients;		// 已经过期的Session, 被断开或者链接失败
	GameSessionMap m_SessionMap;	// 连接成功的Session

	DWORD m_dwLastTime;				// 上一帧的时间
	CECStringTab m_ErrorMsgs;		// 服务器错误信息描述
	elementdataman* m_pDataMan;		// ElementData

	CsMutex m_csSessions;
	GameSession* m_theSession;		// 活动Session, c2s_相关函数所作用的Session

protected:
	SessionManager();

	// 处理过期的Session
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

	// 处理debug命令
	void DealMsgC2SCommand(const StringVector&cmdParams);
};

///////////////////////////////////////////////////////////////////////////
//  
//  Inline functions
//  
///////////////////////////////////////////////////////////////////////////
