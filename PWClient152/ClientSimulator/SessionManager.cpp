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

// ��ȡʵ��
SessionManager& SessionManager::GetSingleton()
{
	static SessionManager mgr;
	return mgr;
}

// ���캯��
SessionManager::SessionManager()
{
	m_dwLastTime = GetTickCount();

}

// ��������
SessionManager::~SessionManager()
{
}

// ��ʼ��ģ��ͻ���
// Param: �����ļ�·��
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

	// ���ط�������������
	if( !m_ErrorMsgs.Init("Configs\\server_error.txt", true) )
	{
		glb_ErrorMessage(ECERR_SERVERERROR);
		return false;
	}

	// ����elements.data
	m_pDataMan = new elementdataman();
	if( m_pDataMan->load_data("data\\elements.data") != 0 )
	{
		glb_ErrorMessage(ECERR_NOELEMENTDATA);
		a_LogOutput(1, "SessionManager::Init, Load the elements.data failed!");
		return false;
	}

	// ��ʼ������
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

	// �����ƶ�����
	AString movefile;
	movefile.Format("%s\\move.data", g_szWorkDir);
	MoveAgent::GetSingleton().Init(movefile);

	// ���ؽű�����
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

	// ��½һ���˺�
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
// 		pSession->SetKickUserFlag(conf->find("Account", "force_login") == "true");	// �����ǰ���ڵ�½����ǿ��������
// 
// 		// ��ӵ��Ự�б�
// 		if( CsLockScoped lock(m_csSessions) ) // ����
// 		{
// 			m_NewSessions[account.c_str()] = pSession;
// 		}
// 
// 		// ���ӷ�����
// 		pSession->Open();
// 	}

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

	// �����ƶ�����
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

// �ͻ���Tick
void SessionManager::Tick()
{
	DWORD dwCurTime = GetTickCount();
	DWORD dwFrameTime = dwCurTime - m_dwLastTime;
	m_dwLastTime = dwCurTime;

	// ������Ϣ����
	MsgManager::GetSingleton().Tick();

	if( CsLockScoped lock(m_csSessions) ) // ����
	{
		// �����Ѿ����ӵ�Session
		GameSessionMap::iterator it;
		for( it=m_SessionMap.begin();it!=m_SessionMap.end();++it )
		{
			GameSession* pSession = it->second;
			
			if( pSession )
			{
				pSession->Update(dwFrameTime);
				pSession->ProcessNewProtocols();
				pSession->ClearOldProtocols();
				
				// ������Ϸ����
				pSession->GetGameObject()->Tick();
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

	// ���½ű�
	g_LuaStateMan.Tick(dwFrameTime);
}

// ����ҵ�½
bool SessionManager::AddPlayer( const char* username,const char* password )
{
	ASSERT( username && password );
	if( m_NewSessions.find(username) != m_NewSessions.end() )
	{
		a_LogOutput(1, "A same session is waiting to connect! username:%s", username);
		return false;
	}

	// �������ﵽ���ֵ
	if( m_SessionMap.size() + m_NewSessions.size() >= MAX_CLIENT_SESSION )
	{
		a_LogOutput(1, "Application has reached the maximum number of connections!");
		return false;
	}

	GameSession* pNewSession = new GameSession();
	pNewSession->SetUserName(username);
	pNewSession->SetUserPassword(password);
	pNewSession->SetKickUserFlag(GNET::Conf::GetInstance()->find("Account", "force_login") == "true");

	if( CsLockScoped lock(m_csSessions) ) // ����
	{
		m_NewSessions[username] = pNewSession;
	}

	// ��ʼ���������
	pNewSession->Open();
	return true;
}

// ע��һ�����
void SessionManager::RemovePlayer( const char* username )
{
	ASSERT( username );
	if( CsLockScoped lock(m_csSessions) ) // ����
	{
		GameSessionMap::iterator it = m_SessionMap.find(username);
		if( it != m_SessionMap.end() ) it->second->Close();
	}
}

// ��ӦSession���ӳɹ�
void SessionManager::OnConnect( GameSession* pSession )
{
	if( CsLockScoped lock(m_csSessions) ) // ����
	{	
		// �Ӵ������Ƶ�������
		GameSessionMap::iterator it = m_NewSessions.find(pSession->GetUserName());
		if( it != m_NewSessions.end() )
		{
			m_SessionMap.insert(GameSessionMap::value_type(pSession->GetUserName(), pSession));
			m_NewSessions.erase(it);
		}
	}
}

// ��ӦSession���ӶϿ�
void SessionManager::OnDisconnect( GameSession* pSession )
{
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
		if( pSession )
		{
			if( pSession->IsConnected() )
				pSession->Close();
		}

		bool bFound = false;

		// �ӻ�б���ɾ��
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
			// ɾ�����������е�Session
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

// ö�������������
void SessionManager::EnumPlayers( StringVector& usernames )
{
	usernames.clear();
	for( GameSessionMap::iterator it=m_SessionMap.begin();it!=m_SessionMap.end();++it )
	{
		if( it->second->IsConnected() )
			usernames.push_back(it->first);
	}
}

// ������Ϣ����
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
	if( CsLockScoped lock(m_csSessions) ) // ����
	{
		GameSessionMap::iterator it = m_SessionMap.find(username);
		if( it != m_SessionMap.end() ) m_theSession = it->second;
	}
}

void SessionManager::SetTheSession(GameSession* pSession)
{
	m_theSession = pSession;
}

// ��ȡָ��Session
GameSession* SessionManager::GetSessionByName(const char* username)
{
	CsLockScoped lock(m_csSessions);
	GameSessionMap::iterator it = m_SessionMap.find(username);
	return it != m_SessionMap.end() ? it->second : NULL;
}

// ��½�����
void SessionManager::OnMsgLogin( MESSAGE* pMsg )
{
	if( pMsg->content.size() >= 2)
		AddPlayer(pMsg->content[0], pMsg->content[1]);
}

// �˳����
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

// ���µ�¼�������������
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

		// �ȴ�����Session�ն�����
		Sleep(3000);
		BrokenSession();

		// ���µ�¼�������
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

			// ���µ�¼
			AddPlayer(user, password);
		}
	}
}

// ���˲��
void SessionManager::OnMsgPlayerMove( MESSAGE* pMsg )
{
	if( pMsg->iParam == 0 )			// �������˲��
	{
		CsLockScoped lock(m_csSessions);
		GameSessionMap::iterator it;
		for( it=m_SessionMap.begin();it!=m_SessionMap.end();++it )
			it->second->GetGameObject()->GetHostPlayer()->RandomMove();
	}
	else if( pMsg->iParam == 1 )	// ָ�����˲��
	{
		ASSERT(pMsg->content.size() >= 1);
		CsLockScoped lock(m_csSessions);
		GameSessionMap::iterator it = m_SessionMap.find(pMsg->content[0]);
		if( it != m_SessionMap.end() )
			it->second->GetGameObject()->GetHostPlayer()->RandomMove();
	}
	else if( pMsg->iParam == 2 )	// ��������ƶ���ָ����
	{
		ASSERT(pMsg->content.size() >= 2);
		float fX = (float)atof(pMsg->content[0]);
		float fZ = (float)atof(pMsg->content[1]);

		CsLockScoped lock(m_csSessions);
		GameSessionMap::iterator it;
		for( it=m_SessionMap.begin();it!=m_SessionMap.end();++it )
			it->second->GetGameObject()->GetHostPlayer()->Goto(fX, fZ);
	}
	else if( pMsg->iParam == 3 )	// ָ������ƶ���ָ����
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

// ִ��d_c2scmd����
void SessionManager::OnMsgC2SCommand(MESSAGE* pMsg)
{
	if(pMsg->iParam == 0)
	{
		// �ȿ�������
		abase::vector<GameSession*> tempSessions;
		abase::vector<GameSession*>::iterator itr;
		if( CsLockScoped lock(m_csSessions) ) // ����
		{
			GameSessionMap::iterator it2;
			for( it2=m_SessionMap.begin();it2!=m_SessionMap.end();++it2 )
			{
				GameSession* pSession = it2->second;
				tempSessions.push_back(pSession);
			}
		}

		// ��ÿһ����ɫ���д���
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

// �ı��ɫAI����
void SessionManager::OnMsgHostPlayerAI( MESSAGE* pMsg )
{
	if(pMsg->iParam == 0) // �����н�ɫ
	{
		GameSessionMap::iterator itr;
		if( CsLockScoped lock(m_csSessions) ) // ����
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
	else if(pMsg->iParam == 1) // ���ض���ɫ
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

// �ı����ǿ��PK��־
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

// ��ʾ���λ��
void SessionManager::OnShowPos( MESSAGE* pMsg )
{
	AString strInfo;

	if(pMsg->iParam == 0) // �����н�ɫ
	{
		GameSessionMap::iterator itr;
		if( CsLockScoped lock(m_csSessions) ) // ����
		{
			for( itr=m_SessionMap.begin();itr!=m_SessionMap.end();++itr )
			{
				GameSession* pSession = itr->second;
				const A3DVECTOR3& pos = pSession->GetGameObject()->GetHostPlayer()->GetCurPos();
				strInfo.Format("%s ��λ���ǣ� x = %f,y = %f,z = %f\n",pSession->GetUserName(),pos.x,pos.y,pos.z);
				AddConsoleText(strInfo, LOGCOLOR_PURPLE);
			}
		}
		
	}
	else if(pMsg->iParam == 1) // ���ض���ɫ
	{
		AString roleName = pMsg->content.back();

		if( CsLockScoped lock(m_csSessions) )
		{
			GameSessionMap::iterator it = m_SessionMap.find(roleName);
			if( it != m_SessionMap.end() )
			{
				SetTheSession(it->second);
				const A3DVECTOR3& pos = m_theSession->GetGameObject()->GetHostPlayer()->GetCurPos();
				strInfo.Format("%s ��λ���ǣ� x = %f,y = %f,z = %f\n",roleName,pos.x,pos.y,pos.z);
				AddConsoleText(strInfo, LOGCOLOR_PURPLE);
			}
		}
	}
}

// ��ʾ�����Ϣ
void SessionManager::OnMsgShowInfo( MESSAGE* pMsg )
{
	const char* PROF_STR[NUM_PROFESSION] = { "����", "��ʦ", "��ʦ", "����", "����", "�̿�", "��â", "����", "����", "����" };
	const char* GENDER_STR[NUM_GENDER] = { "��", "Ů" };

	AString strInfo;
	if(pMsg->iParam == 0) // �����н�ɫ
	{
		GameSessionMap::iterator itr;
		if( CsLockScoped lock(m_csSessions) ) // ����
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
	else if(pMsg->iParam == 1) // ���ض���ɫ
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

// ������ֱ�ӵ���lua�ű�
void SessionManager::OnLuaCall( MESSAGE* pMsg )
{
	abase::vector<CScriptValue> args;
	abase::vector<CScriptValue> results;
	
	AString luaNamespace = pMsg->content[0];
	AString luaFileName  = pMsg->content[1]; 
	AString luaFuncName  = pMsg->content[2];      
	
	// ����ָ����lua�ļ���ָ������
	LuaFuncFactory::GetSingleton().ReflashSrciptFile(luaFileName);
	LuaBind::Call(g_LuaStateMan.GetConfigState()->GetVM(), luaNamespace, luaFuncName, args, results);
	if( results.size() != 1 || !results[0].GetBool() )
	{
		AddConsoleText("lua ����ִ��ʧ��!\n", LOGCOLOR_RED);
	}
	else
	{
		AddConsoleText("lua OK!\n", LOGCOLOR_GREEN);
	}
}

// ������һ����
void SessionManager::OnMsgSetRegion( MESSAGE* pMsg )
{
	A3DVECTOR3 vRegionMin(0);
	A3DVECTOR3 vRegionMax(0);

	vRegionMin.x = max((float)atof(pMsg->content[0]), MOVEAGENT_X_START);
	vRegionMin.z = max((float)atof(pMsg->content[1]), MOVEAGENT_Z_START);
	vRegionMax.x = min((float)atof(pMsg->content[2]), MOVEAGENT_X_END);
	vRegionMax.z = min((float)atof(pMsg->content[3]), MOVEAGENT_Z_END);
	MoveAgent::GetSingleton().SetRegion(vRegionMin, vRegionMax);

	// �����������ڵ�������·�һ��
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

// ִ��AI����
void SessionManager::OnMsgAICommand( MESSAGE* pMsg )
{
	if( pMsg->iParam == 0 )
	{
		GameSessionMap::iterator itr;
		if( CsLockScoped lock(m_csSessions) ) // ����
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