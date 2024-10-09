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
#include "ec_sendc2scmds.h"
#include "GameObject.h"
#include "MoveAgent.h"
#include "elementdataman.h"
#include "LuaFunc.h"

///////////////////////////////////////////////////////////////////////////
//  
//  Define and Macro
//  
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//  
//  Types and Global variables
//  
///////////////////////////////////////////////////////////////////////////

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
	m_dwLastTime = GetTickCount();

}

// 析构函数
SessionManager::~SessionManager()
{
}

// 初始化模拟客户端
// Param: 配置文件路径
bool SessionManager::Init( const char* cfg_file )
{
	int i;
	glb_InitLogSystem("sclient.log");

	if( access(cfg_file, R_OK) == -1 )
	{
		glb_ErrorMessage(ECERR_NOCONFIGFILE);
		a_LogOutput(1, "SessionManager::Init, Can't find the config file %s !", cfg_file);
		return false;
	}

	// 加载服务器错误描述
	if( !m_ErrorMsgs.Init("Configs\\server_error.txt", true) )
	{
		glb_ErrorMessage(ECERR_SERVERERROR);
		return false;
	}

	// 加载elements.data
	m_pDataMan = new elementdataman();
	if( m_pDataMan->load_data("data\\elements.data") != 0 )
	{
		glb_ErrorMessage(ECERR_NOELEMENTDATA);
		a_LogOutput(1, "SessionManager::Init, Load the elements.data failed!");
		return false;
	}

	// 初始化网络
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		glb_ErrorMessage(ECERR_INITSOCKETERR);
		a_LogOutput(1, "_InitNetwork, Cannot initialize Windows Sockets !");
		return false;
	}

	a_InitRandom();
	GNET::Conf* conf = GNET::Conf::GetInstance(cfg_file);
	GNET::PollIO::Init();

	// 加载移动数据
	AString movefile;
	movefile.Format("%s\\move.data", g_szWorkDir);
	MoveAgent::GetSingleton().Init(movefile);

	// 加载脚本数据
	AString pattern;
	StringVector ai_files;
	std::string ai_folder = conf->find("AI", "lua_folder");
	pattern.Format("%s\\%s\\*.lua", g_szWorkDir, ai_folder.c_str());
	EnumFolder(pattern, ai_files);
	for( i=0;i<(int)ai_files.size();i++ )
	{
		AString ai_file = AString(g_szWorkDir) + "\\" + ai_folder.c_str() + "\\" + ai_files[i];
		LuaFuncFactory::GetSingleton().ReflashSrciptFile(ai_file);
	}

	// 登陆一批账号
// 	char buf[20];
// 	std::string prefix = conf->find("Account","prefix");
// 	int start = atoi(conf->find("Account","start").c_str());
// 	int end = atoi(conf->find("Account","end").c_str());
// 	std::string password = conf->find("Account","password");
// 	int role = atoi(conf->find("Account","role").c_str());
// 
// 	for( i=start;i<=end;i++ )
// 	{
// 		GameSession* pSession = new GameSession();
// 
// 		sprintf(buf, "%d", i);
// 		std::string account = prefix + std::string(buf);
// 
// 		pSession->SetUserName(account.c_str());
// 		pSession->SetUserPassword(password.c_str());
// 		pSession->SetKickUserFlag(conf->find("Account", "force_login") == "true");	// 如果当前正在登陆，则强制其下线
// 
// 		// 添加到会话列表
// 		if( CsLockScoped lock(m_csSessions) ) // 加锁
// 		{
// 			m_NewSessions[account.c_str()] = pSession;
// 		}
// 
// 		// 连接服务器
// 		pSession->Open();
// 	}

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
		
		for( it=m_SessionMap.begin();it!=m_SessionMap.end();++it )
		{
			if( it->second )
			{
				it->second->Close();
				delete it->second;
				it->second = NULL;
			}
		}
		
		m_NewSessions.clear();
		m_SessionMap.clear();
	}

	// 保存移动数据
	MoveAgent::GetSingleton().Quit();

	if( m_pDataMan )
	{
		delete m_pDataMan;
		m_pDataMan = NULL;
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

	// 处理消息队列
	MsgManager::GetSingleton().Tick();

	if( CsLockScoped lock(m_csSessions) ) // 加锁
	{
		// 处理已经连接的Session
		GameSessionMap::iterator it;
		for( it=m_SessionMap.begin();it!=m_SessionMap.end();++it )
		{
			GameSession* pSession = it->second;
			
			if( pSession )
			{
				pSession->Update(dwFrameTime);
				pSession->ProcessNewProtocols();
				pSession->ClearOldProtocols();
				
				// 更新游戏对象
				pSession->GetGameObject()->Tick();
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

	// 更新脚本
	g_LuaStateMan.Tick(dwFrameTime);
}

// 新玩家登陆
bool SessionManager::AddPlayer( const char* username,const char* password )
{
	ASSERT( username && password );
	if( m_NewSessions.find(username) != m_NewSessions.end() )
	{
		a_LogOutput(1, "A same session is waiting to connect! username:%s", username);
		return false;
	}

	// 连接数达到最大值
	if( m_SessionMap.size() + m_NewSessions.size() >= MAX_CLIENT_SESSION )
	{
		a_LogOutput(1, "Application has reached the maximum number of connections!");
		return false;
	}

	GameSession* pNewSession = new GameSession();
	pNewSession->SetUserName(username);
	pNewSession->SetUserPassword(password);
	pNewSession->SetKickUserFlag(GNET::Conf::GetInstance()->find("Account", "force_login") == "true");

	if( CsLockScoped lock(m_csSessions) ) // 加锁
	{
		m_NewSessions[username] = pNewSession;
	}

	// 开始连接新玩家
	pNewSession->Open();
	return true;
}

// 注销一个玩家
void SessionManager::RemovePlayer( const char* username )
{
	ASSERT( username );
	if( CsLockScoped lock(m_csSessions) ) // 加锁
	{
		GameSessionMap::iterator it = m_SessionMap.find(username);
		if( it != m_SessionMap.end() ) it->second->Close();
	}
}

// 响应Session连接成功
void SessionManager::OnConnect( GameSession* pSession )
{
	if( CsLockScoped lock(m_csSessions) ) // 加锁
	{	
		// 从待连接移到已连接
		GameSessionMap::iterator it = m_NewSessions.find(pSession->GetUserName());
		if( it != m_NewSessions.end() )
		{
			m_SessionMap.insert(GameSessionMap::value_type(pSession->GetUserName(), pSession));
			m_NewSessions.erase(it);
		}
	}
}

// 响应Session连接断开
void SessionManager::OnDisconnect( GameSession* pSession )
{
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
		if( pSession )
		{
			if( pSession->IsConnected() )
				pSession->Close();
		}

		bool bFound = false;

		// 从活动列表中删除
		GameSessionMap::iterator it;
		for( it=m_SessionMap.begin();it!=m_SessionMap.end();++it )
		{
			if( it->second == pSession )
			{
				delete it->second;
				m_SessionMap.erase(it);
				bFound = true;
				break;
			}
		}

		if( !bFound )
		{
			// 删除正在连接中的Session
			GameSessionMap::iterator it2 = m_NewSessions.find(pSession->GetUserName());
			if( it2 != m_NewSessions.end() )
			{
				delete it2->second;
				m_NewSessions.erase(it2);
			}
		}
	}

	m_OldClients.clear();
}

// 枚举所有在线玩家
void SessionManager::EnumPlayers( StringVector& usernames )
{
	usernames.clear();
	for( GameSessionMap::iterator it=m_SessionMap.begin();it!=m_SessionMap.end();++it )
	{
		if( it->second->IsConnected() )
			usernames.push_back(it->first);
	}
}

// 处理消息队列
void SessionManager::ProcessMessage(MESSAGE* pMsg)
{
	switch( pMsg->iMsg )
	{
	case MSG_LOGIN:
		OnMsgLogin(pMsg);
		break;
	case MSG_LOGOUT:
		OnMsgLogout(pMsg);
		break;
	case MSG_RELOGINALL:
		OnMsgReloginAll(pMsg);
		break;
	case MSG_C2SCOMMAND:
		OnMsgC2SCommand(pMsg);
		break;
	case MSG_HOSTPLAYERAI:
		OnMsgHostPlayerAI(pMsg);
		break;
	case MSG_FORCEATTACK:
		OnMsgForceAttack(pMsg);
		break;
	case MSG_SHOWPOS:
		OnShowPos(pMsg);
		break;
	case MSG_LUACALL:
		OnLuaCall(pMsg);
		break;
	case MSG_SETREGION:
		OnMsgSetRegion(pMsg);
		break;
	case MSG_AICOMMAND:
		OnMsgAICommand(pMsg);
		break;
	case MSG_PLAYERMOVE:
		OnMsgPlayerMove(pMsg);
		break;
	case MSG_SHOWINFO:
		OnMsgShowInfo(pMsg);
		break;
	}
}

void SessionManager::SetTheSession(const AString& username) 
{
	if( CsLockScoped lock(m_csSessions) ) // 加锁
	{
		GameSessionMap::iterator it = m_SessionMap.find(username);
		if( it != m_SessionMap.end() ) m_theSession = it->second;
	}
}

void SessionManager::SetTheSession(GameSession* pSession)
{
	m_theSession = pSession;
}

// 获取指定Session
GameSession* SessionManager::GetSessionByName(const char* username)
{
	CsLockScoped lock(m_csSessions);
	GameSessionMap::iterator it = m_SessionMap.find(username);
	return it != m_SessionMap.end() ? it->second : NULL;
}

// 登陆新玩家
void SessionManager::OnMsgLogin( MESSAGE* pMsg )
{
	if( pMsg->content.size() >= 2)
		AddPlayer(pMsg->content[0], pMsg->content[1]);
}

// 退出玩家
void SessionManager::OnMsgLogout( MESSAGE* pMsg )
{
	if( pMsg->iParam == 1 )
	{
		ASSERT(pMsg->content.size() >= 1);
		RemovePlayer(pMsg->content[0]);
	}
	else
	{
		CsLockScoped lock(m_csSessions);
		for( GameSessionMap::iterator it=m_SessionMap.begin();it!=m_SessionMap.end();++it )
			RemovePlayer(it->first);
	}
}

// 重新登录所有已连接玩家
void SessionManager::OnMsgReloginAll( MESSAGE* pMsg )
{
	if( pMsg->iParam == 0 )
	{
		StringVector users, passwords;
		if( CsLockScoped lock(m_csSessions) )
		{
			for( GameSessionMap::iterator it=m_SessionMap.begin();it!=m_SessionMap.end();++it )
			{
				users.push_back(it->first);
				passwords.push_back(it->second->GetUserPassword());
			}
		}

		MESSAGE tempMsg;
		tempMsg.iMsg = MSG_LOGOUT;
		tempMsg.iParam = 0;
		OnMsgLogout(&tempMsg);

		// 等待所有Session终端链接
		Sleep(3000);
		BrokenSession();

		// 重新登录所有玩家
		for( size_t i=0;i<users.size();++i )
		{
			AddPlayer(users[i], passwords[i]);
		}
	}
	else if( pMsg->iParam == 1 )
	{
		ASSERT(pMsg->content.size() >= 1);
		AString user, password;

		if( CsLockScoped lock(m_csSessions) )
		{
			GameSessionMap::iterator it = m_SessionMap.find(pMsg->content[0]);
			if( it != m_SessionMap.end() )
			{
				user = it->first;
				password = it->second->GetUserPassword();
			}
		}

		if( !user.IsEmpty() )
		{
			RemovePlayer(user);
			Sleep(3000);
			BrokenSession();

			// 重新登录
			AddPlayer(user, password);
		}
	}
}

// 玩家瞬移
void SessionManager::OnMsgPlayerMove( MESSAGE* pMsg )
{
	if( pMsg->iParam == 0 )			// 所有玩家瞬移
	{
		CsLockScoped lock(m_csSessions);
		GameSessionMap::iterator it;
		for( it=m_SessionMap.begin();it!=m_SessionMap.end();++it )
			it->second->GetGameObject()->GetHostPlayer()->RandomMove();
	}
	else if( pMsg->iParam == 1 )	// 指定玩家瞬移
	{
		ASSERT(pMsg->content.size() >= 1);
		CsLockScoped lock(m_csSessions);
		GameSessionMap::iterator it = m_SessionMap.find(pMsg->content[0]);
		if( it != m_SessionMap.end() )
			it->second->GetGameObject()->GetHostPlayer()->RandomMove();
	}
	else if( pMsg->iParam == 2 )	// 所有玩家移动到指定点
	{
		ASSERT(pMsg->content.size() >= 2);
		float fX = (float)atof(pMsg->content[0]);
		float fZ = (float)atof(pMsg->content[1]);

		CsLockScoped lock(m_csSessions);
		GameSessionMap::iterator it;
		for( it=m_SessionMap.begin();it!=m_SessionMap.end();++it )
			it->second->GetGameObject()->GetHostPlayer()->Goto(fX, fZ);
	}
	else if( pMsg->iParam == 3 )	// 指定玩家移动到指定点
	{
		ASSERT(pMsg->content.size() >= 3);
		float fX = (float)atof(pMsg->content[0]);
		float fZ = (float)atof(pMsg->content[1]);

		CsLockScoped lock(m_csSessions);
		GameSessionMap::iterator it = m_SessionMap.find(pMsg->content[2]);
		if( it != m_SessionMap.end() )
			it->second->GetGameObject()->GetHostPlayer()->Goto(fX, fZ);
	}
}

// 执行d_c2scmd命令
void SessionManager::OnMsgC2SCommand(MESSAGE* pMsg)
{
	if(pMsg->iParam == 0)
	{
		// 先拷贝出来
		abase::vector<GameSession*> tempSessions;
		abase::vector<GameSession*>::iterator itr;
		if( CsLockScoped lock(m_csSessions) ) // 加锁
		{
			GameSessionMap::iterator it2;
			for( it2=m_SessionMap.begin();it2!=m_SessionMap.end();++it2 )
			{
				GameSession* pSession = it2->second;
				tempSessions.push_back(pSession);
			}
		}

		// 对每一个角色进行处理
		for( itr=tempSessions.begin();itr!=tempSessions.end();++itr )
		{
			GameSession* pSession = *itr;
			m_theSession = pSession;
			DealMsgC2SCommand(pMsg->content);
		}
	}
	else if(pMsg->iParam == 1)
	{
		AString roleName = pMsg->content.back();
		pMsg->content.pop_back();

		if( CsLockScoped lock(m_csSessions) )
		{
			GameSessionMap::iterator it = m_SessionMap.find(roleName);
			if( it != m_SessionMap.end() )
			{
				SetTheSession(it->second);
				DealMsgC2SCommand(pMsg->content);
			}
		}
	}
}

void SessionManager::DealMsgC2SCommand(const StringVector&cmdParams)
{
	int iNumParam = cmdParams.size() - 2;
	int iCmd = cmdParams[1].ToInt();
	
	if (iNumParam <= 0)
		c2s_SendDebugCmd(iCmd, 0);
	else if (iNumParam == 1)
		c2s_SendDebugCmd(iCmd, 1, cmdParams[2].ToInt());
	else if (iNumParam == 2)
		c2s_SendDebugCmd(iCmd, 2, cmdParams[2].ToInt(), cmdParams[3].ToInt());
	else if (iNumParam == 3)
		c2s_SendDebugCmd(iCmd, 3, cmdParams[2].ToInt(), cmdParams[3].ToInt(), cmdParams[4].ToInt());
	else if (iNumParam == 4)
		c2s_SendDebugCmd(iCmd, 4, cmdParams[2].ToInt(), cmdParams[3].ToInt(), cmdParams[4].ToInt(), cmdParams[5].ToInt());
	else if (iNumParam == 5)
		c2s_SendDebugCmd(iCmd, 5, cmdParams[2].ToInt(), cmdParams[3].ToInt(), cmdParams[4].ToInt(), cmdParams[5].ToInt(), cmdParams[6].ToInt());
	else if (iNumParam == 6)
		c2s_SendDebugCmd(iCmd, 6, cmdParams[2].ToInt(), cmdParams[3].ToInt(), cmdParams[4].ToInt(), cmdParams[5].ToInt(), cmdParams[6].ToInt(), cmdParams[7].ToInt());
	else if (iNumParam == 7)
		c2s_SendDebugCmd(iCmd, 7, cmdParams[2].ToInt(), cmdParams[3].ToInt(), cmdParams[4].ToInt(), cmdParams[5].ToInt(), cmdParams[6].ToInt(), cmdParams[7].ToInt(), cmdParams[8].ToInt());
	else if (iNumParam == 8)
		c2s_SendDebugCmd(iCmd, 8, cmdParams[2].ToInt(), cmdParams[3].ToInt(), cmdParams[4].ToInt(), cmdParams[5].ToInt(), cmdParams[6].ToInt(), cmdParams[7].ToInt(), cmdParams[8].ToInt(), cmdParams[9].ToInt());
}

// 改变角色AI策略
void SessionManager::OnMsgHostPlayerAI( MESSAGE* pMsg )
{
	if(pMsg->iParam == 0) // 对所有角色
	{
		GameSessionMap::iterator itr;
		if( CsLockScoped lock(m_csSessions) ) // 加锁
		{
			for( itr=m_SessionMap.begin();itr!=m_SessionMap.end();++itr )
			{
				GameSession* pSession = itr->second;
				if(pMsg->content[0].IsEmpty())
					pSession->GetGameObject()->GetHostPlayer()->SetAIPolicy(NULL);
				else
					pSession->GetGameObject()->GetHostPlayer()->SetAIPolicy(pMsg->content[0]);
			}
		}
		
	}
	else if(pMsg->iParam == 1) // 对特定角色
	{
		AString roleName = pMsg->content.back();

		if( CsLockScoped lock(m_csSessions) )
		{
			GameSessionMap::iterator it = m_SessionMap.find(roleName);
			if( it != m_SessionMap.end() )
			{
				SetTheSession(it->second);

				if(pMsg->content[0].IsEmpty())
					m_theSession->GetGameObject()->GetHostPlayer()->SetAIPolicy(NULL);
				else
					m_theSession->GetGameObject()->GetHostPlayer()->SetAIPolicy(pMsg->content[0]);
			}
		}
	}
}

// 改变玩家强制PK标志
void SessionManager::OnMsgForceAttack( MESSAGE* pMsg )
{
	if( pMsg->iParam == 0 )
	{
		int iVal = pMsg->content[0].ToInt();
		GameSessionMap::iterator it;
		if( CsLockScoped lock(m_csSessions) )
		{
			for( it=m_SessionMap.begin();it!=m_SessionMap.end();++it )
			{
				GameSession* pSession = it->second;
				pSession->GetGameObject()->GetHostPlayer()->SetForceAttack(iVal ? true : false);
			}
		}
	}
	else
	{
		int iVal = pMsg->content[0].ToInt();
		AString username = pMsg->content[1];

		if( CsLockScoped lock(m_csSessions) )
		{
			GameSessionMap::iterator it = m_SessionMap.find(username);
			if( it != m_SessionMap.end() )
			{
				GameSession* pSession = it->second;
				pSession->GetGameObject()->GetHostPlayer()->SetForceAttack(iVal ? true : false);
			}
		}
	}
}

// 显示玩家位置
void SessionManager::OnShowPos( MESSAGE* pMsg )
{
	AString strInfo;

	if(pMsg->iParam == 0) // 对所有角色
	{
		GameSessionMap::iterator itr;
		if( CsLockScoped lock(m_csSessions) ) // 加锁
		{
			for( itr=m_SessionMap.begin();itr!=m_SessionMap.end();++itr )
			{
				GameSession* pSession = itr->second;
				const A3DVECTOR3& pos = pSession->GetGameObject()->GetHostPlayer()->GetCurPos();
				strInfo.Format("%s 的位置是： x = %f,y = %f,z = %f\n",pSession->GetUserName(),pos.x,pos.y,pos.z);
				AddConsoleText(strInfo, LOGCOLOR_PURPLE);
			}
		}
		
	}
	else if(pMsg->iParam == 1) // 对特定角色
	{
		AString roleName = pMsg->content.back();

		if( CsLockScoped lock(m_csSessions) )
		{
			GameSessionMap::iterator it = m_SessionMap.find(roleName);
			if( it != m_SessionMap.end() )
			{
				SetTheSession(it->second);
				const A3DVECTOR3& pos = m_theSession->GetGameObject()->GetHostPlayer()->GetCurPos();
				strInfo.Format("%s 的位置是： x = %f,y = %f,z = %f\n",roleName,pos.x,pos.y,pos.z);
				AddConsoleText(strInfo, LOGCOLOR_PURPLE);
			}
		}
	}
}

// 显示玩家信息
void SessionManager::OnMsgShowInfo( MESSAGE* pMsg )
{
	const char* PROF_STR[NUM_PROFESSION] = { "武侠", "法师", "巫师", "妖精", "妖兽", "刺客", "羽芒", "羽灵", "剑灵", "魅灵" };
	const char* GENDER_STR[NUM_GENDER] = { "男", "女" };

	AString strInfo;
	if(pMsg->iParam == 0) // 对所有角色
	{
		GameSessionMap::iterator itr;
		if( CsLockScoped lock(m_csSessions) ) // 加锁
		{
			for( itr=m_SessionMap.begin();itr!=m_SessionMap.end();++itr )
			{
				GameSession* pSession = itr->second;
				PlayerObject* pPlayer = pSession->GetGameObject()->GetHostPlayer();
				AString strProf = (pSession->GetProfession() >= 0 && pSession->GetProfession() < NUM_PROFESSION) ? PROF_STR[pSession->GetProfession()] : "-";
				AString strGender = (pSession->GetGender() >= 0 && pSession->GetGender() < NUM_GENDER) ? GENDER_STR[pSession->GetGender()] : "-";
				strInfo.Format("username:%s\troleid:%d\tname:%s(%s,%s)\tlevel:%d\n", itr->first, pSession->GetCharacterID(), WC2AS(pSession->GetRoleName()), strProf, strGender, pPlayer->GetLevel());
				AddConsoleText(strInfo, LOGCOLOR_PURPLE);
			}
		}
	}
	else if(pMsg->iParam == 1) // 对特定角色
	{
		AString roleName = pMsg->content.back();

		if( CsLockScoped lock(m_csSessions) )
		{
			GameSessionMap::iterator it = m_SessionMap.find(roleName);
			if( it != m_SessionMap.end() )
			{
				SetTheSession(it->second);
				GameSession* pSession = it->second;
				PlayerObject* pPlayer = pSession->GetGameObject()->GetHostPlayer();
				AString strProf = (pSession->GetProfession() >= 0 && pSession->GetProfession() < NUM_PROFESSION) ? PROF_STR[pSession->GetProfession()] : "-";
				AString strGender = (pSession->GetGender() >= 0 && pSession->GetGender() < NUM_GENDER) ? GENDER_STR[pSession->GetGender()] : "-";
				strInfo.Format("username:%s\troleid:%d\tname:%s(%s,%s)\tlevel:%d\n", it->first, pSession->GetCharacterID(), WC2AS(pSession->GetRoleName()), strProf, strGender, pPlayer->GetLevel());
				AddConsoleText(strInfo, LOGCOLOR_PURPLE);
			}
		}
	}
}

// 命令行直接调用lua脚本
void SessionManager::OnLuaCall( MESSAGE* pMsg )
{
	abase::vector<CScriptValue> args;
	abase::vector<CScriptValue> results;
	
	AString luaNamespace = pMsg->content[0];
	AString luaFileName  = pMsg->content[1]; 
	AString luaFuncName  = pMsg->content[2];      
	
	// 调用指定的lua文件的指定函数
	LuaFuncFactory::GetSingleton().ReflashSrciptFile(luaFileName);
	LuaBind::Call(g_LuaStateMan.GetConfigState()->GetVM(), luaNamespace, luaFuncName, args, results);
	if( results.size() != 1 || !results[0].GetBool() )
	{
		AddConsoleText("lua 函数执行失败!\n", LOGCOLOR_RED);
	}
	else
	{
		AddConsoleText("lua OK!\n", LOGCOLOR_GREEN);
	}
}

// 设置玩家活动区域
void SessionManager::OnMsgSetRegion( MESSAGE* pMsg )
{
	A3DVECTOR3 vRegionMin(0);
	A3DVECTOR3 vRegionMax(0);

	vRegionMin.x = max((float)atof(pMsg->content[0]), MOVEAGENT_X_START);
	vRegionMin.z = max((float)atof(pMsg->content[1]), MOVEAGENT_Z_START);
	vRegionMax.x = min((float)atof(pMsg->content[2]), MOVEAGENT_X_END);
	vRegionMax.z = min((float)atof(pMsg->content[3]), MOVEAGENT_Z_END);
	MoveAgent::GetSingleton().SetRegion(vRegionMin, vRegionMax);

	// 将不再区域内的玩家重新飞一便
	if( CsLockScoped lock(m_csSessions) )
	{
		GameSessionMap::iterator it;
		for( it=m_SessionMap.begin();it!=m_SessionMap.end();++it )
		{
			PlayerObject* pPlayer = it->second->GetGameObject()->GetHostPlayer();

			if( pPlayer->GetCurPos().x < vRegionMin.x ||
				pPlayer->GetCurPos().x > vRegionMax.x ||
				pPlayer->GetCurPos().z < vRegionMin.z ||
				pPlayer->GetCurPos().z > vRegionMax.z )
			{
				pPlayer->RandomMove();
			}
		}
	}
}

// 执行AI命令
void SessionManager::OnMsgAICommand( MESSAGE* pMsg )
{
	if( pMsg->iParam == 0 )
	{
		GameSessionMap::iterator itr;
		if( CsLockScoped lock(m_csSessions) ) // 加锁
		{
			for( itr=m_SessionMap.begin();itr!=m_SessionMap.end();++itr )
			{
				GameSession* pSession = itr->second;
				pSession->GetGameObject()->GetHostPlayer()->AICommand(pMsg->content);
			}
		}
	}
	else if( pMsg->iParam == 1 )
	{
		AString roleName = pMsg->content.back();
		pMsg->content.pop_back();

		if( CsLockScoped lock(m_csSessions) )
		{
			GameSessionMap::iterator it = m_SessionMap.find(roleName);
			if( it != m_SessionMap.end() )
			{
				GameSession* pSession = it->second;
				pSession->GetGameObject()->GetHostPlayer()->AICommand(pMsg->content);
			}
		}
	}
}