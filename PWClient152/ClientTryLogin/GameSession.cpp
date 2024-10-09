/*
 * FILE: GameSession.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2012/8/29
 *
 * HISTORY: 
 *
 * Copyright (c) 2011 ACE Studio, All Rights Reserved.
 */


#include "Common.h"
#include "GameSession.h"
#include "gnet.h"
#include "gameclient.h"
#include "EC_GPDataType.h"
#include "SessionManager.h"

#include <string>
#include <windows.h>

#include "playerpositionresetrqst.hrp"
#include "getcnetserverconfig.hpp"
#include "getcnetserverconfig_re.hpp"
#include "playerchangeds_re.hpp"
#include "changeds_re.hpp"
#include "keyreestablish.hpp"


///////////////////////////////////////////////////////////////////////////
//  
//  Define and Macro
//  
///////////////////////////////////////////////////////////////////////////

//	Base index of link error message
#define BASEIDX_LINKSEVERROR 10150

const char* ERROR_SERVERTIMEOUT = "服务器无响应，连接已超时！";
const char* ERROR_LINKSERVERFAILED = "无法连接到服务器！";
const char* ERROR_BREAKLINE = "进入游戏时，与服务器的连接已经断开！";
const char* ERROR_LOGINNEEDMATRIX = "该账号需要密保卡验证，登陆不成功，请换一个账号！";
const char* ERROR_CREATEROLEFAIL = "连续10次创建角色失败！";
const char* ERROR_INVALIDPROTO = "解析协议出错，说明本程序与服务器协议不同步！";
const char* ERROR_PLAYERCHANGEDS = "当前玩家正在跨服中，无法测试服务器连通情况！";
const char* ERROR_HAVENOGMROLE = "没有发现合适的GM角色（要求等级>100 修真>20），请换一个账号！";
const char* ERROR_INVALIDDSTZONE = "找不到目标服务器地址，请检查serverlist.txt";
const char* ERROR_CROSSSERVERCOOL = "跨服命令冷却中，请稍后再测！";


///////////////////////////////////////////////////////////////////////////
//  
//  Types and Global variables
//  
///////////////////////////////////////////////////////////////////////////

class DeferedChangeDS : public GameSession::DeferedCmd
{
public:
	DeferedChangeDS(GameSession* pSession) : DeferedCmd(TYPE_CHANGEDS, pSession)
	{
		m_bToCrossServer = false;
	}

	void Send(const GNET::Octets& o)
	{
		int nTmp = 0;
		GNET::Marshal::OctetsStream(o) >> nTmp;
		m_bToCrossServer = nTmp ? true : false;
		DeferedCmd::Send(o);
	}

	void Update(DWORD dwDeltaTime)
	{
		if( m_bToSend && m_pSession->GetCoolTime(GP_CT_CROSS_SERVER_APPLY) <= 0 )
		{
			if( m_pSession->c2s_CmdGMChangeDS(m_bToCrossServer) )
				m_bToSend = false;
		}
	}

protected:
	bool m_bToCrossServer;
};


///////////////////////////////////////////////////////////////////////////
//  
//  Implement class GameSession
//  
///////////////////////////////////////////////////////////////////////////

// 构造函数
GameSession::GameSession() : m_aNewProtocols(128, 128), m_aOldProtocols(128, 128)
{
	if (!(m_pNetMan = new GameClient(this)))
	{
		ASSERT(m_pNetMan);
		a_LogOutput(1, "GameSession::GameSession, Failed to create net manager");
	}

	m_pNetMan->SetZoneID(0);
	m_pNetMan->Attach(IOCallBack);

	m_idLink			= (DWORD)(-1);
	m_iUserID			= 0;
	m_iCharID			= 0;
	m_iOrgZoneID		= 0;
	m_iZoneID			= 0;
	m_iServerLine		= 0;
	m_bLinkBroken		= false;
	m_bConnected		= false;
	m_hConnectThread	= NULL;
	m_bKickUser			= true;
	memset(&m_ot, 0, sizeof (m_ot));
	m_bExitConnect		= false;
	m_iCurRoleIdx		= -1;
	m_iGameResult		= TEST_FAIL;
	m_iCreateRoleErr	= 0;
	m_dwIdleTime		= 0;
	m_bWaitRelogin		= false;
	m_bSessionClosed	= true;
	memset(m_aCoolTimes, 0, sizeof(m_aCoolTimes));

	m_timedQueries.push_back(&m_qCountryWarBonus);
	m_qCrossServer.SetSession(this);
	if( IsTestCrossServer() )
		m_timedQueries.push_back(&m_qCrossServer);

	m_DeferedCmds[DeferedCmd::TYPE_CHANGEDS] = new DeferedChangeDS(this);
}

// 析构函数
GameSession::~GameSession()
{
	if( m_pNetMan )
	{
		m_pNetMan->Detach();
		delete m_pNetMan;
		m_pNetMan = NULL;
	}

	DeferedCmds::iterator it;
	for(it=m_DeferedCmds.begin(); it!=m_DeferedCmds.end(); ++it)
	{
		delete it->second;
	}
}

//	Open session
bool GameSession::Open()
{
	//	Connect server
	Connect();

	return true;
}

//	Close sesssion
void GameSession::Close(bool bRelogin)
{
	if( m_bSessionClosed )
		return;

	CloseConnectThread();

	m_bWaitRelogin = bRelogin;

	//	Close net manager
	if (m_pNetMan)
	{
		m_pNetMan->Disconnect();
		m_idLink = 0;
		PollIO::WakeUp();
	}

	//	Release all unprocessed protocols
	if( CsLockScoped lock(m_csSession) ) // 加锁
	{
		for (int i=0; i < m_aNewProtocols.GetSize(); i++)
		{
			GNET::Protocol* p = m_aNewProtocols[i];
			p->Destroy();
		}
		m_aNewProtocols.RemoveAll();
	}

	//	Release all processed protocols
	ClearOldProtocols();

	m_bConnected = false;
	m_bSessionClosed = true;
}

void GameSession::BreakLink()
{
	//	强制主动断开链接，显示断开链接对话框
	Close();
	m_bLinkBroken = true;
}

DWORD WINAPI GameSession::ConnectThread(LPVOID pArg)
{
	GameSession* pGameSession = (GameSession*)pArg;
	AString ip = pGameSession->m_strAddress;
	int port = pGameSession->m_iPort;

	if (!ip.IsEmpty() && port)
		pGameSession->GetNetManager()->Connect(ip, port);
	else
		pGameSession->GetNetManager()->Connect(NULL, 0);

	//	Start overtime check
	DWORD dwTotal, dwCount;
	if( !pGameSession->GetOvertimeCnt(dwTotal, dwCount) )
	{
		// we only activate the overtime check when the timer has not been activated
		pGameSession->DoOvertimeCheck(true, GameSession::OT_CHALLENGE, 40000);
	}

	//	Wait until connnect is established
	while (!pGameSession->IsConnected() && !pGameSession->m_bExitConnect)
	{
		Sleep(100);
	}

	return 0;
}

//	Connect server
bool GameSession::Connect()
{
	if (!m_pNetMan)
	{
		ASSERT(m_pNetMan);
		return false;
	}

	CloseConnectThread();
	
	// we use a thread to connect with the server, this can avoid the main thread blocked by 
	// firewalls when calling socket's connect
	DWORD dwThreadID;
	m_bExitConnect = false;
	m_bSessionClosed = false;
	m_hConnectThread = CreateThread(NULL, 0, ConnectThread, this, 0, &dwThreadID);

	return true;
}

void GameSession::CloseConnectThread()
{	
	if (m_hConnectThread)
	{
		m_bExitConnect = true;
		WaitForSingleObject(m_hConnectThread, INFINITE);
		m_ot.bCheck = false;
		CloseHandle(m_hConnectThread);
		m_hConnectThread = NULL;
	}
}

//  Send Game data
bool GameSession::SendGameData( void* pData,int iSize )
{
	using namespace GNET;

	GamedataSend p;
	p.data.replace(pData, iSize);
	return SendNetData(p);
}

//	Send net data
bool GameSession::SendNetData(const GNET::Protocol& p, bool bUrg)
{
	if (!m_pNetMan || m_idLink == (DWORD)(-1))
	{
		ASSERT(m_pNetMan && m_idLink != (DWORD)(-1));
		return false;
	}

	return m_pNetMan->Send(m_idLink, p, bUrg);
}

//	Start / End overtime
void GameSession::DoOvertimeCheck(bool bStart, int iCheckID, DWORD dwTime)
{
	if (bStart)
	{
		m_ot.bCheck		= bStart;
		m_ot.iCheckID	= iCheckID;
		m_ot.dwTime		= dwTime;
		m_ot.dwTimeCnt	= 0;
	}
	else
	{
		ASSERT(m_ot.iCheckID == iCheckID);
		m_ot.bCheck	= false;
	}
}

//	On overtime happens
void GameSession::OnOvertimeHappen()
{
	switch (m_ot.iCheckID)
	{
	case OT_CHALLENGE:
		
		m_bExitConnect = true;
		LinkBroken(false);
		break;
		
	case OT_ENTERGAME:
		
		LinkBroken(true);
		break;
	}
}

//	Get overtime counter
bool GameSession::GetOvertimeCnt(DWORD& dwTotalTime, DWORD& dwCnt)
{
	if (m_ot.bCheck)
	{
		dwTotalTime = m_ot.dwTime;
		dwCnt = m_ot.dwTimeCnt;
		return true;
	}
	else
		return false;
}

// 响应协议的到来
int GameSession::IOCallBack(void* pGameSession, void* pData, unsigned int idLink, int iEvent)
{
	using namespace GNET;

	GameSession* pSession = (GameSession*)pGameSession;
	if (!pSession)
	{
		ASSERT(0);
		return 0;
	}

	GNET::GameClient* pNetMan = pSession->GetNetManager();
	if (!pNetMan)
	{
		a_LogOutput(1, "CECGameSession::IOCallBack, callback when pNetMan=NULL !");
		OutputDebugString(_AL("CECGameSession::IOCallBack, callback when pNetMan=NULL !"));
		return 0;
	}

	switch (iEvent)
	{
	case EVENT_ADDSESSION:

		OutputDebugStringA("EVENT_ADDSESSION\n");
		pSession->SetLinkID(idLink);
		pSession->m_bConnected	= true;
		pSession->m_bLinkBroken	= false;
		SessionManager::GetSingleton().OnConnect(pSession);
		return 0;

	case EVENT_DELSESSION:
		//	If we are in game state and were disconnected passively, 
		//	notify player and exit game
		if (pSession->m_bConnected)
			pSession->m_bLinkBroken = true;

		pSession->m_bConnected = false;
		return 0;
	case EVENT_ABORTSESSION:
		// connect to server failed, so pop up a message window
		// make overtime check fire.
		pSession->DoOvertimeCheck(true, GameSession::OT_CHALLENGE, 0);
		
		pSession->m_bConnected = false;
		SessionManager::GetSingleton().OnDisconnect(pSession);
		return 0;
	case EVENT_DISCONNECT:
		SessionManager::GetSingleton().OnDisconnect(pSession);

		// 解析协议出错
		if( !_stricmp(GNET::errormsg, "Decode error\n") )
			pSession->m_strReason = ERROR_INVALIDPROTO;
		return 0;
	}

	GNET::Protocol* pProtocol = (GNET::Protocol*)pData;
	pSession->AddNewProtocol(pProtocol);

	return 0;
}

//	Add a protocol to m_NewPrtcList and prepare to be processed
int GameSession::AddNewProtocol(GNET::Protocol* pProtocol)
{
	if( CsLockScoped lock(m_csSession) ) // 加锁
	{
		int iSize = m_aNewProtocols.Add(pProtocol);
		return iSize;
	}
	return 0;
}

//	When connection was broken, this function is called
void GameSession::LinkBroken(bool bDisconnect)
{
	if( bDisconnect )
	{
		// 断开连接
		// 有可能导致断开连接时，错误信息已经写入，判断一下避免覆盖原来的错误信息
		if( m_strReason.IsEmpty() )
			m_strReason = ERROR_BREAKLINE;
	}
	else
	{
		// 连接失败
		m_strReason = ERROR_LINKSERVERFAILED;
	}

	Close();
	m_bLinkBroken = false;
}

//	Update
bool GameSession::Update(DWORD dwDeltaTime)
{
	//	Check overtime
	if (m_ot.bCheck)
	{
		if ((m_ot.dwTimeCnt += dwDeltaTime) >= m_ot.dwTime)
		{
			m_ot.bCheck = false;
			OnOvertimeHappen();
		}
	}

	if (m_bLinkBroken)
		LinkBroken(true);

	// 一直未收到服务器协议
	// 情况1. GS挂掉，GLINK认为GS还在
	if( m_bConnected )
	{
		m_dwIdleTime += dwDeltaTime;
		if( m_dwIdleTime >= MAX_IDLE_TIME )
		{
			m_strReason = ERROR_SERVERTIMEOUT;
			Close();
		}
		for (TimedQueries::iterator it = m_timedQueries.begin(); it != m_timedQueries.end(); ++ it)
		{
			TimedQuery * q = *it;
			if (q->Tick(dwDeltaTime))
			{
				if (q == &m_qCountryWarBonus)
					m_iGameResult |= TEST_COUNTRYWAR_BONUS_ERR;
				else if(q == &m_qCrossServer)
					m_iGameResult |= TEST_CROSSSERVER_ERR;
			}
		}
	}

	//	Update cool times
	for (int i=0; i < GP_CT_MAX; i++)
		m_aCoolTimes[i].Update(dwDeltaTime);

	//  Process defered send
	DeferedCmds::iterator it;
	for( it=m_DeferedCmds.begin(); it!=m_DeferedCmds.end(); ++it )
	{
		if( it->second && it->second->IsToSend() )
			it->second->Update(dwDeltaTime);
	}

	return true;
}

//	Net pulse
void GameSession::NetPulse()
{
	GNET::Timer::Update();
}

//	Process protocols in m_NewPrtcList
bool GameSession::ProcessNewProtocols()
{
	using namespace GNET;

	int i;

	if( CsLockScoped lock(m_csSession) ) // 加锁
	{
		for (i=0; i < m_aNewProtocols.GetSize(); i++)
			m_aTempProtocols.Add(m_aNewProtocols[i]);
		m_aNewProtocols.RemoveAll(false);
	}

	if( m_aTempProtocols.GetSize() )
		m_dwIdleTime = 0;

	for (i=0; i < m_aTempProtocols.GetSize(); i++)
	{
		bool bAddToOld = true;
		GNET::Protocol* pProtocol = m_aTempProtocols[i];
		
		if (!pProtocol)
		{
			a_LogOutput(1, "CECGameSession::ProcessNewProtocols(), Empty Protocol!");
			continue;
		}

		if (FilterProtocols(pProtocol))
		{
			m_aOldProtocols.Add(pProtocol);
			continue;
		}

		switch (pProtocol->GetType())
		{
		case PROTOCOL_CHALLENGE:				OnPrtcChallenge(pProtocol);				break;
		case PROTOCOL_KEYEXCHANGE:				OnPrtcKeyExchange(pProtocol);			break;
		case PROTOCOL_ONLINEANNOUNCE:			OnPrtcOnlineAnnounce(pProtocol);		break;
		case PROTOCOL_ROLELIST_RE:				OnPrtcRoleListRe(pProtocol);			break;
		case PROTOCOL_SELECTROLE_RE:			OnPrtcSelectRoleRe(pProtocol);			break;
		case PROTOCOL_CREATEROLE_RE:			OnPrtcCreateRoleRe(pProtocol);			break;
		case PROTOCOL_DELETEROLE_RE:			/*OnPrtcDeleteRoleRe(pProtocol);*/			break;
		case PROTOCOL_UNDODELETEROLE_RE:		/*OnPtrcUndoDeleteRoleRe(pProtocol);*/		break;
		case PROTOCOL_ERRORINFO:				OnPrtcErrorInfo(pProtocol);				break;
		case PROTOCOL_PLAYERLOGOUT:				OnPrtcPlayerLogout(pProtocol);			break;
		case PROTOCOL_MATRIXCHALLENGE:			OnPrtcMatrixChallenge(pProtocol);		break;
		case PROTOCOL_PLAYERCHANGEDS_RE:		OnPrtcPlayerChangeDSRe(pProtocol);		break;
		case PROTOCOL_CHANGEDS_RE:				OnPrtcChangeDSRe(pProtocol);			break;
		case RPC_PLAYERPOSITIONRESETRQST:
			OnPrtcResetPosition(pProtocol);
			bAddToOld = false;
			break;

		case PROTOCOL_GAMEDATASEND:	
			
			ProcessGameData(((GamedataSend*)pProtocol)->data);
			break;

		case PROTOCOL_CHATMESSAGE:				break;
		case PROTOCOL_PRIVATECHAT:				break;
		case PROTOCOL_PLAYERBASEINFO_RE:		break;
		case PROTOCOL_GETUICONFIG_RE:			break;
		case PROTOCOL_SETUICONFIG_RE:			break;
		case PROTOCOL_GETHELPSTATES_RE:			break;
		case PROTOCOL_SETHELPSTATES_RE:			break;
		case PROTOCOL_SETCUSTOMDATA_RE:			break;
		case PROTOCOL_GETCUSTOMDATA_RE:			break;
		case PROTOCOL_GETPLAYERBRIEFINFO_RE:	break;
		case PROTOCOL_GETPLAYERIDBYNAME_RE:		break;
		case PROTOCOL_ROLESTATUSANNOUNCE:		break;
		case PROTOCOL_ANNOUNCEFORBIDINFO:		break;
		case PROTOCOL_UPDATEREMAINTIME:			break;
		case PROTOCOL_WORLDCHAT:				break;
		case PROTOCOL_REFGETREFERENCECODE_RE:	break;
		case PROTOCOL_REFLISTREFERRALS_RE:		break;
		case PROTOCOL_REFWITHDRAWBONUS_RE:		break;
		case PROTOCOL_GETREWARDLIST_RE:			break;
		case PROTOCOL_EXCHANGECONSUMEPOINTS_RE: break;
		case PROTOCOL_REWARDMATURENOTICE:		break;
		case PROTOCOL_ACCOUNTLOGINRECORD:		break;
		
		//	Trade protocols
		case PROTOCOL_TRADESTART_RE:
		case PROTOCOL_TRADEMOVEOBJ_RE:
		case PROTOCOL_TRADEADDGOODS_RE:
		case PROTOCOL_TRADEREMOVEGOODS_RE:
		case PROTOCOL_TRADEDISCARD_RE:
		case PROTOCOL_TRADESUBMIT_RE:
		case PROTOCOL_TRADECONFIRM_RE:
		case PROTOCOL_TRADEEND:

			break;

		//  UserCoupon protocols
		case PROTOCOL_USERCOUPON_RE:
		case PROTOCOL_USERCOUPONEXCHANGE_RE:

			break;

		case PROTOCOL_USERADDCASH_RE:

			break;
			
			//  web Trade protocols
		case PROTOCOL_WEBTRADELIST_RE:
		case PROTOCOL_WEBTRADEATTENDLIST_RE:
		case PROTOCOL_WEBTRADEGETITEM_RE:
		case PROTOCOL_WEBTRADEGETDETAIL_RE:
		case PROTOCOL_WEBTRADEPREPOST_RE:
		case PROTOCOL_WEBTRADEPRECANCELPOST_RE:
		case PROTOCOL_WEBTRADEUPDATE_RE:
			
			break;

		// Sys Auction Protocols
		case PROTOCOL_SYSAUCTIONACCOUNT_RE:
		case PROTOCOL_SYSAUCTIONBID_RE:
		case PROTOCOL_SYSAUCTIONGETITEM_RE:
		case PROTOCOL_SYSAUCTIONCASHTRANSFER_RE:
		case PROTOCOL_SYSAUCTIONLIST_RE:

			break;

		case PROTOCOL_CREATEFACTIONFORTRESS_RE:
		case PROTOCOL_FACTIONFORTRESSLIST_RE:
		case PROTOCOL_FACTIONFORTRESSCHALLENGE_RE:
		case PROTOCOL_FACTIONFORTRESSBATTLELIST_RE:
		case PROTOCOL_FACTIONFORTRESSGET_RE:

			break;
			
		case PROTOCOL_STOCKACCOUNT_RE:
		case PROTOCOL_STOCKBILL_RE:
		case PROTOCOL_STOCKCOMMISSION_RE:
		case PROTOCOL_STOCKTRANSACTION_RE:
		case PROTOCOL_STOCKCANCEL_RE:

			break;

		//	GM protocols
		case PROTOCOL_GMONLINENUM_RE:			break;	
		case PROTOCOL_GMLISTONLINEUSER_RE:		break;
		case PROTOCOL_GMKICKOUTUSER_RE:			break;	
		case PROTOCOL_GMSHUTUP_RE:				break;
		case PROTOCOL_GMRESTARTSERVER_RE:		break;
		case PROTOCOL_GMRESTARTSERVER:			break;
		case PROTOCOL_GMKICKOUTROLE_RE:			break;
		case PROTOCOL_GMSHUTUPROLE_RE:			break;
		case PROTOCOL_GMFORBIDROLE_RE:			break;
		case PROTOCOL_GMTOGGLECHAT_RE:			break;

		//	Friend protocols
		case PROTOCOL_ADDFRIEND_RE:
		case PROTOCOL_GETFRIENDS_RE:
		case PROTOCOL_SETGROUPNAME_RE:
		case PROTOCOL_SETFRIENDGROUP_RE:
		case PROTOCOL_DELFRIEND_RE:
		case PROTOCOL_FRIENDSTATUS:
		case PROTOCOL_GETSAVEDMSG_RE:
		case PROTOCOL_FRIENDEXTLIST:
		case PROTOCOL_SENDAUMAIL_RE:

			break;
			
		//	Faction protocols
		case PROTOCOL_FACTIONCHAT:
			break;

		case PROTOCOL_FACTIONACCEPTJOIN_RE:

			break;

		case PROTOCOL_FACTIONRESIGN_RE:

			break;

		case PROTOCOL_FACTIONRENAME_RE:

			break;

		case PROTOCOL_FACTIONMASTERRESIGN_RE:

			break;

		case PROTOCOL_FACTIONLEAVE_RE:

			break;

		case PROTOCOL_FACTIONEXPEL_RE:

			break;

		case PROTOCOL_FACTIONAPPOINT_RE:

			break;

		case PROTOCOL_FACTIONOPREQUEST_RE:
		case PROTOCOL_FACTIONCREATE_RE:
		case PROTOCOL_FACTIONBROADCASTNOTICE_RE:
		case PROTOCOL_FACTIONUPGRADE_RE:
		case PROTOCOL_FACTIONDEGRADE_RE:
		case PROTOCOL_FACTIONLISTMEMBER_RE:
		case PROTOCOL_GETFACTIONBASEINFO_RE:
		case PROTOCOL_FACTIONDISMISS_RE:
		case PROTOCOL_GETPLAYERFACTIONINFO_RE:
		case PROTOCOL_FACTIONCHANGPROCLAIM_RE:

			break;
			
		case PROTOCOL_FACTIONALLIANCEAPPLY_RE:
		case PROTOCOL_FACTIONALLIANCEREPLY_RE:
		case PROTOCOL_FACTIONHOSTILEAPPLY_RE:
		case PROTOCOL_FACTIONHOSTILEREPLY_RE:
		case PROTOCOL_FACTIONREMOVERELATIONAPPLY_RE:
		case PROTOCOL_FACTIONREMOVERELATIONREPLY_RE:
		case PROTOCOL_FACTIONLISTRELATION_RE:
		case PROTOCOL_FACTIONLISTONLINE_RE:
		case PROTOCOL_FACTIONRELATIONRECVAPPLY:
		case PROTOCOL_FACTIONRELATIONRECVREPLY:

			break;

		//	Chatroom protocols
		case PROTOCOL_CHATROOMCREATE_RE:
		case PROTOCOL_CHATROOMINVITE:
		case PROTOCOL_CHATROOMINVITE_RE:
		case PROTOCOL_CHATROOMJOIN_RE:
		case PROTOCOL_CHATROOMLEAVE:
		case PROTOCOL_CHATROOMEXPEL:
		case PROTOCOL_CHATROOMSPEAK:
		case PROTOCOL_CHATROOMLIST_RE:
			break;

		//	Mail protocols
		case PROTOCOL_ANNOUNCENEWMAIL:
		case PROTOCOL_GETMAILLIST_RE:
		case PROTOCOL_GETMAIL_RE:
		case PROTOCOL_GETMAILATTACHOBJ_RE:
		case PROTOCOL_DELETEMAIL_RE:
		case PROTOCOL_PRESERVEMAIL_RE:
		case PROTOCOL_PLAYERSENDMAIL_RE:
			break;

		//	Vendue protocols
		case PROTOCOL_AUCTIONOPEN_RE:
		case PROTOCOL_AUCTIONBID_RE:
		case PROTOCOL_AUCTIONLIST_RE:
		case PROTOCOL_AUCTIONLISTUPDATE_RE:
		case PROTOCOL_AUCTIONCLOSE_RE:
		case PROTOCOL_AUCTIONGET_RE:
		case PROTOCOL_AUCTIONATTENDLIST_RE:
		case PROTOCOL_AUCTIONEXITBID_RE:
		case PROTOCOL_AUCTIONGETITEM_RE:
			break;

		//	Battle protocols
		case PROTOCOL_BATTLECHALLENGE_RE:
		case PROTOCOL_BATTLECHALLENGEMAP_RE:
		case PROTOCOL_BATTLEGETMAP_RE:
		case PROTOCOL_BATTLEENTER_RE:
		case PROTOCOL_BATTLESTATUS_RE:
			break;

		//	Account protocols
		case PROTOCOL_SELLPOINT_RE:
		case PROTOCOL_GETSELLLIST_RE:
		case PROTOCOL_FINDSELLPOINTINFO_RE:
		case PROTOCOL_SELLCANCEL_RE:
		case PROTOCOL_BUYPOINT_RE:
		case PROTOCOL_ANNOUNCESELLRESULT:
			break;

		//	RPCs...
		case RPC_FACTIONINVITEJOIN:
			break;

		case RPC_TRADESTARTRQST:
			break;

		case RPC_ADDFRIENDRQST:
			break;

		case PROTOCOL_KEEPALIVE:
			//	Do nothing ...
			break;

		case PROTOCOL_CASHLOCK_RE:
		case PROTOCOL_CASHPASSWORDSET_RE:
			break;

		case PROTOCOL_ACREMOTECODE:
			break;

		case PROTOCOL_ACQUESTION:
			break;

		case PROTOCOL_AUTOLOCKSET_RE:
			break;

		case PROTOCOL_GMGETPLAYERCONSUMEINFO_RE:
			break;

		case PROTOCOL_SSOGETTICKET_RE:
			break;

		case PROTOCOL_COUNTRYBATTLEMOVE_RE:
		case PROTOCOL_COUNTRYBATTLESYNCPLAYERLOCATION:
		case PROTOCOL_COUNTRYBATTLEGETMAP_RE:
		case PROTOCOL_COUNTRYBATTLEGETSCORE_RE:
		case PROTOCOL_COUNTRYBATTLEGETCONFIG_RE:
		case PROTOCOL_COUNTRYBATTLEPREENTERNOTIFY:
			break;
		case PROTOCOL_COUNTRYBATTLERESULT:
			break;
		case PROTOCOL_GETCNETSERVERCONFIG_RE:
			OnPrtcGetServerConfigRe(pProtocol);
			break;

		default: break;
		}

		//	Add this protocol to old list
		if (bAddToOld)
			m_aOldProtocols.Add(pProtocol);
	}

	m_aTempProtocols.RemoveAll(false);

	return true;
}

//	Clear all processed protocols in m_OldPrtcList
void GameSession::ClearOldProtocols()
{
	if( CsLockScoped lock(m_csSession) ) // 加锁
	{
		for (int i=0; i < m_aOldProtocols.GetSize(); i++)
		{
			GNET::Protocol* p = m_aOldProtocols[i];
			p->Destroy();
		}
		m_aOldProtocols.RemoveAll(false);
	}
}

//	Filter protocols. Return true if we shouldn't handle this protocol in
//	current state
bool GameSession::FilterProtocols(GNET::Protocol* pProtocol)
{
	bool bFilter = false;

	switch (pProtocol->GetType())
	{
	case PROTOCOL_CHALLENGE:
	case PROTOCOL_KEYEXCHANGE:
	case PROTOCOL_ONLINEANNOUNCE:
	case PROTOCOL_ROLELIST_RE:	
	case RPC_PLAYERPOSITIONRESETRQST:
	case PROTOCOL_SELECTROLE_RE:
	case PROTOCOL_CREATEROLE_RE:
	case PROTOCOL_DELETEROLE_RE:
	case PROTOCOL_UNDODELETEROLE_RE:
	case PROTOCOL_MATRIXCHALLENGE:

		//	If we aren't in login state, ignore this data
// 		if (g_pGame->GetGameRun()->GetGameState() != CECGameRun::GS_LOGIN)
// 			bFilter = true;

		break;

	case PROTOCOL_GAMEDATASEND:
	case PROTOCOL_CHATMESSAGE:
	case PROTOCOL_WORLDCHAT:
	case PROTOCOL_PLAYERLOGOUT:
	case PROTOCOL_PRIVATECHAT:
	case PROTOCOL_PLAYERBASEINFO_RE:
	case PROTOCOL_GETUICONFIG_RE:
	case PROTOCOL_SETUICONFIG_RE:
	case PROTOCOL_SETHELPSTATES_RE:
	case PROTOCOL_GETHELPSTATES_RE:
	case PROTOCOL_TRADESTART_RE:
	case PROTOCOL_TRADEMOVEOBJ_RE:
	case PROTOCOL_TRADEADDGOODS_RE:
	case PROTOCOL_TRADEREMOVEGOODS_RE:
	case PROTOCOL_TRADEDISCARD_RE:
	case PROTOCOL_TRADESUBMIT_RE:
	case PROTOCOL_TRADECONFIRM_RE:
	case PROTOCOL_TRADEEND:
	case PROTOCOL_GETPLAYERBRIEFINFO_RE:
	case PROTOCOL_ROLESTATUSANNOUNCE:
	case RPC_TRADESTARTRQST:

	case PROTOCOL_GMONLINENUM_RE:	
	case PROTOCOL_GMLISTONLINEUSER_RE:
	case PROTOCOL_GMKICKOUTUSER_RE:
	case PROTOCOL_GMSHUTUP_RE:
	case PROTOCOL_GMRESTARTSERVER_RE:
	case PROTOCOL_GMKICKOUTROLE_RE:
	case PROTOCOL_GMSHUTUPROLE_RE:

	case PROTOCOL_ADDFRIEND_RE:
	case PROTOCOL_GETFRIENDS_RE:
	case PROTOCOL_SETGROUPNAME_RE:
	case PROTOCOL_SETFRIENDGROUP_RE:
	case PROTOCOL_DELFRIEND_RE:
	case PROTOCOL_FRIENDSTATUS:
	case PROTOCOL_GETSAVEDMSG_RE:
	case RPC_ADDFRIENDRQST:

	case PROTOCOL_FACTIONOPREQUEST_RE:
	case PROTOCOL_FACTIONCREATE_RE:
	case PROTOCOL_FACTIONBROADCASTNOTICE_RE:
	case PROTOCOL_FACTIONUPGRADE_RE:
	case PROTOCOL_FACTIONDEGRADE_RE:
	case PROTOCOL_FACTIONLISTMEMBER_RE:
	case PROTOCOL_GETFACTIONBASEINFO_RE:
	case PROTOCOL_FACTIONDISMISS_RE:
	case PROTOCOL_GETPLAYERFACTIONINFO_RE:
	case PROTOCOL_FACTIONCHANGPROCLAIM_RE:
	case PROTOCOL_FACTIONCHAT:
	case PROTOCOL_FACTIONACCEPTJOIN_RE:
	case PROTOCOL_FACTIONRESIGN_RE:
	case PROTOCOL_FACTIONRENAME_RE:
	case PROTOCOL_FACTIONMASTERRESIGN_RE:
	case PROTOCOL_FACTIONLEAVE_RE:
	case PROTOCOL_FACTIONEXPEL_RE:
	case PROTOCOL_FACTIONAPPOINT_RE:
    case PROTOCOL_FACTIONALLIANCEAPPLY_RE:
    case PROTOCOL_FACTIONALLIANCEREPLY_RE:
    case PROTOCOL_FACTIONHOSTILEAPPLY_RE:
    case PROTOCOL_FACTIONHOSTILEREPLY_RE:
    case PROTOCOL_FACTIONREMOVERELATIONAPPLY_RE:
    case PROTOCOL_FACTIONREMOVERELATIONREPLY_RE:
	case PROTOCOL_FACTIONLISTRELATION_RE:
	case PROTOCOL_FACTIONLISTONLINE_RE:
	case PROTOCOL_FACTIONRELATIONRECVAPPLY:
	case PROTOCOL_FACTIONRELATIONRECVREPLY:

	case PROTOCOL_CHATROOMCREATE_RE:
	case PROTOCOL_CHATROOMINVITE:
	case PROTOCOL_CHATROOMINVITE_RE:
	case PROTOCOL_CHATROOMJOIN_RE:
	case PROTOCOL_CHATROOMLEAVE:
	case PROTOCOL_CHATROOMEXPEL:
	case PROTOCOL_CHATROOMSPEAK:
	case PROTOCOL_CHATROOMLIST_RE:

	case PROTOCOL_ANNOUNCENEWMAIL:
	case PROTOCOL_GETMAILLIST_RE:
	case PROTOCOL_GETMAIL_RE:
	case PROTOCOL_GETMAILATTACHOBJ_RE:
	case PROTOCOL_DELETEMAIL_RE:
	case PROTOCOL_PRESERVEMAIL_RE:
	case PROTOCOL_PLAYERSENDMAIL_RE:

	case PROTOCOL_AUCTIONOPEN_RE:
	case PROTOCOL_AUCTIONBID_RE:
	case PROTOCOL_AUCTIONLIST_RE:
	case PROTOCOL_AUCTIONLISTUPDATE_RE:
	case PROTOCOL_AUCTIONCLOSE_RE:
	case PROTOCOL_AUCTIONGET_RE:
	case PROTOCOL_AUCTIONATTENDLIST_RE:
	case PROTOCOL_AUCTIONEXITBID_RE:
	case PROTOCOL_AUCTIONGETITEM_RE:

	case PROTOCOL_BATTLECHALLENGE_RE:
	case PROTOCOL_BATTLECHALLENGEMAP_RE:
	case PROTOCOL_BATTLEGETMAP_RE:
	case PROTOCOL_BATTLEENTER_RE:
	case PROTOCOL_BATTLESTATUS_RE:

	case PROTOCOL_SELLPOINT_RE:
	case PROTOCOL_GETSELLLIST_RE:
	case PROTOCOL_FINDSELLPOINTINFO_RE:
	case PROTOCOL_SELLCANCEL_RE:
	case PROTOCOL_BUYPOINT_RE:
	case PROTOCOL_ANNOUNCESELLRESULT:

	case PROTOCOL_CASHLOCK_RE:
	case PROTOCOL_CASHPASSWORDSET_RE:
	case PROTOCOL_AUTOLOCKSET_RE:
		
	case PROTOCOL_WEBTRADELIST_RE:
	case PROTOCOL_WEBTRADEATTENDLIST_RE:
	case PROTOCOL_WEBTRADEGETITEM_RE:
	case PROTOCOL_WEBTRADEUPDATE_RE:
		
	case PROTOCOL_SYSAUCTIONACCOUNT_RE:
	case PROTOCOL_SYSAUCTIONBID_RE:
	case PROTOCOL_SYSAUCTIONGETITEM_RE:
	case PROTOCOL_SYSAUCTIONCASHTRANSFER_RE:
	case PROTOCOL_SYSAUCTIONLIST_RE:
		
	case PROTOCOL_CREATEFACTIONFORTRESS_RE:
	case PROTOCOL_GMGETPLAYERCONSUMEINFO_RE:
	case PROTOCOL_SSOGETTICKET_RE:
	case PROTOCOL_COUNTRYBATTLEMOVE_RE:
	case PROTOCOL_COUNTRYBATTLESYNCPLAYERLOCATION:
	case PROTOCOL_COUNTRYBATTLEGETMAP_RE:
	case PROTOCOL_COUNTRYBATTLEGETSCORE_RE:
	case PROTOCOL_COUNTRYBATTLEGETCONFIG_RE:
	case PROTOCOL_COUNTRYBATTLEPREENTERNOTIFY:
	case PROTOCOL_COUNTRYBATTLERESULT:

		//	If we aren't in game state, ignore this data
// 		if (g_pGame->GetGameRun()->GetGameState() != CECGameRun::GS_GAME)
// 			bFilter = true;

		break;

	case PROTOCOL_GETPLAYERIDBYNAME_RE:
	case PROTOCOL_WEBTRADEPREPOST_RE:
	case PROTOCOL_WEBTRADEPRECANCELPOST_RE:
	case PROTOCOL_WEBTRADEGETDETAIL_RE:
	case PROTOCOL_ERRORINFO:
	case PROTOCOL_KEEPALIVE:
	case PROTOCOL_UPDATEREMAINTIME:
	case PROTOCOL_SETCUSTOMDATA_RE:
	case PROTOCOL_GETCUSTOMDATA_RE:
	case PROTOCOL_ANNOUNCEFORBIDINFO:
		
		break;

	default:
		break;
	}

	return bFilter;
}

DWORD GameSession::CalcS2CCmdDataSize(int iCmd, BYTE* pDataBuf, DWORD dwDataSize)
{
	using namespace S2C;

#define CHECK_VALID(type) \
{\
	if (!reinterpret_cast<type*>(pDataBuf)->CheckValid(dwDataSize, sz) || dwDataSize != sz) {\
		assert(0);}\
	else\
		dwSize = (DWORD)(-2);\
}

	DWORD dwSize = (DWORD)(-1);
	size_t sz;

	switch (iCmd)
	{
	case PLAYER_INFO_1:
	case PLAYER_ENTER_WORLD:
	case PLAYER_ENTER_SLICE:

		CHECK_VALID(info_player_1)
		break;

	case PLAYER_INFO_1_LIST:

		CHECK_VALID(cmd_player_info_1_list)
		break;

	case NPC_INFO_LIST:

		CHECK_VALID(cmd_npc_info_list)
		break;

	case PLAYER_INFO_2:				break;
	case PLAYER_INFO_3:				break;
	case PLAYER_INFO_4:				break;
	case PLAYER_INFO_2_LIST:		break;
	case PLAYER_INFO_3_LIST:		break;
	case PLAYER_INFO_23_LIST:		break;

	case SELF_INFO_1:

		CHECK_VALID(cmd_self_info_1)
		break;

	case SELF_INFO_00:				dwSize = sizeof (cmd_self_info_00);			break;

	case MATTER_INFO_LIST:
		CHECK_VALID(cmd_matter_info_list);
		break;

	case NPC_ENTER_WORLD:
	case NPC_ENTER_SLICE:

		CHECK_VALID(info_npc)
		break;

	case OBJECT_LEAVE_SLICE:		dwSize = sizeof (cmd_leave_slice);			break;
	case NOTIFY_HOSTPOS:			dwSize = sizeof (cmd_notify_hostpos);		break;
	case OBJECT_MOVE:				dwSize = sizeof (cmd_object_move);			break;
	case MATTER_ENTER_WORLD:		dwSize = sizeof (cmd_matter_enter_world);	break;
	case PLAYER_LEAVE_WORLD:		dwSize = sizeof (cmd_player_leave_world);	break;
	case NPC_DIED:					dwSize = sizeof (cmd_npc_died);				break;
	case OBJECT_DISAPPEAR:			dwSize = sizeof (cmd_object_disappear);		break;
	case HOST_STOPATTACK:			dwSize = sizeof (cmd_host_stop_attack);		break;
	case HOST_ATTACKRESULT:			dwSize = sizeof (cmd_host_attack_result);	break;
	case ERROR_MESSAGE:				dwSize = sizeof (cmd_error_msg);			break;
	case HOST_ATTACKED:				dwSize = sizeof (cmd_host_attacked);		break;
	case PLAYER_DIED:				dwSize = sizeof (cmd_player_died);			break;
	case HOST_DIED:					dwSize = sizeof (cmd_host_died);			break;
	case PLAYER_REVIVE:				dwSize = sizeof (cmd_player_revive);		break;
	case PICKUP_MONEY:				dwSize = sizeof (cmd_pickup_money);			break;
	case PICKUP_ITEM:				dwSize = sizeof (cmd_pickup_item);			break;
	case PLAYER_INFO_00:			dwSize = sizeof (cmd_player_info_00);		break;
	case NPC_INFO_00:				dwSize = sizeof (cmd_npc_info_00);			break;

	case OUT_OF_SIGHT_LIST:

		CHECK_VALID(cmd_out_of_sight_list);
		break;

	case OBJECT_STOP_MOVE:			dwSize = sizeof (cmd_object_stop_move);		break;
	case RECEIVE_EXP:				dwSize = sizeof (cmd_receive_exp);			break;
	case LEVEL_UP:					dwSize = sizeof (cmd_level_up);				break;
	case UNSELECT:					dwSize = 0;									break;
	case OWN_ITEM_INFO:

		CHECK_VALID(cmd_own_item_info)
		break;

	case EMPTY_ITEM_SLOT:			dwSize = sizeof (cmd_empty_item_slot);		break;
	case OWN_IVTR_DATA:

		CHECK_VALID(cmd_own_ivtr_info)
		break;

	case OWN_IVTR_DETAIL_DATA:

		CHECK_VALID(cmd_own_ivtr_detail_info)
		break;

	case EXG_IVTR_ITEM:				dwSize = sizeof (cmd_exg_ivtr_item);		break;
	case MOVE_IVTR_ITEM:			dwSize = sizeof (cmd_move_ivtr_item);		break;
	case PLAYER_DROP_ITEM:			dwSize = sizeof (cmd_player_drop_item);		break;
	case EXG_EQUIP_ITEM:			dwSize = sizeof (cmd_exg_equip_item);		break;
	case EQUIP_ITEM:				dwSize = sizeof (cmd_equip_item);			break;
	case MOVE_EQUIP_ITEM:			dwSize = sizeof (cmd_move_equip_item);		break;
	case OWN_EXT_PROP:				dwSize = sizeof (cmd_own_ext_prop);			break;
	case ADD_STATUS_POINT:			dwSize = sizeof (cmd_add_status_point);		break;
	case SELECT_TARGET:				dwSize = sizeof (cmd_select_target);		break;
	case PLAYER_EXT_PROP_BASE:		dwSize = sizeof (cmd_pep_base);				break;
	case PLAYER_EXT_PROP_MOVE:		dwSize = sizeof (cmd_pep_move);				break;
	case PLAYER_EXT_PROP_ATK:		dwSize = sizeof (cmd_pep_attack);			break;
	case PLAYER_EXT_PROP_DEF:		dwSize = sizeof (cmd_pep_def);				break;
	case TEAM_LEADER_INVITE:		dwSize = sizeof (cmd_team_leader_invite);	break;
	case TEAM_REJECT_INVITE:		dwSize = sizeof (cmd_team_reject_invite);	break;
	case TEAM_JOIN_TEAM:			dwSize = sizeof (cmd_team_join_team);		break;
	case TEAM_MEMBER_LEAVE:			dwSize = sizeof (cmd_team_member_leave);	break;
	case TEAM_LEAVE_PARTY:			dwSize = sizeof (cmd_team_leave_party);		break;
	case TEAM_NEW_MEMBER:			dwSize = sizeof (cmd_team_new_member);		break;
	case TEAM_LEADER_CACEL_PARTY:	dwSize = sizeof (cmd_team_leader_cancel_party);	break;

	case TEAM_MEMBER_DATA:

		CHECK_VALID(cmd_team_member_data);
		break;

	case TEAM_MEMBER_POS:			dwSize = sizeof (cmd_team_member_pos);		break;
	case EQUIP_DATA:

		CHECK_VALID(cmd_equip_data);
		break;

	case EQUIP_DATA_CHANGED:

		CHECK_VALID(cmd_equip_data_changed);
		break;

	case EQUIP_DAMAGED:				dwSize = sizeof (cmd_equip_damaged);		break;
	case TEAM_MEMBER_PICKUP:		dwSize = sizeof (cmd_team_member_pickup);	break;
	case NPC_GREETING:				dwSize = sizeof (cmd_npc_greeting);			break;

	case NPC_SERVICE_CONTENT:

		CHECK_VALID(cmd_npc_sev_content)
		break;

	case PURCHASE_ITEM:

		CHECK_VALID(cmd_purchase_item)
		break;

	case ITEM_TO_MONEY:				dwSize = sizeof (cmd_item_to_money);		break;
	case REPAIR_ALL:				dwSize = sizeof (cmd_repair_all);			break;
	case REPAIR:					dwSize = sizeof (cmd_repair);				break;

	case RENEW:

		break;

	case SPEND_MONEY:				dwSize = sizeof (cmd_spend_money);			break;
	case GAIN_MONEY_IN_TRADE:		dwSize = sizeof (cmd_gain_money_in_trade);	break;
	case GAIN_ITEM_IN_TRADE:		dwSize = sizeof (cmd_gain_item_in_trade);	break;
	case GAIN_MONEY_AFTER_TRADE:	dwSize = sizeof (cmd_gain_money_after_trade);	break;
	case GAIN_ITEM_AFTER_TRADE:		dwSize = sizeof (cmd_gain_item_after_trade);	break;
	case GET_OWN_MONEY:				dwSize = sizeof (cmd_get_own_money);		break;
	case ATTACK_ONCE:				dwSize = sizeof (cmd_attack_once);			break;
	case HOST_START_ATTACK:			dwSize = sizeof (cmd_host_start_attack);	break;
	case OBJECT_CAST_SKILL:			dwSize = sizeof (cmd_object_cast_skill);	break;
	case PLAYER_CAST_RUNE_SKILL:	dwSize = sizeof (cmd_player_cast_rune_skill);	break;
	case PLAYER_CAST_RUNE_INSTANT_SKILL:	dwSize = sizeof (cmd_player_cast_rune_instant_skill);	break;
	case SKILL_INTERRUPTED:			dwSize = sizeof (cmd_skill_interrupted);	break;
	case SKILL_PERFORM:				dwSize = 0;									break;
	case SELF_SKILL_INTERRUPTED:	dwSize = sizeof (cmd_self_skill_interrupted);	break;

	case SKILL_DATA:

		CHECK_VALID(cmd_skill_data)
		break;

	case HOST_USE_ITEM:				dwSize = sizeof (cmd_host_use_item);		break;
	case PLAYER_USE_ITEM_WITH_ARG:	dwSize = (DWORD)-2;							break;
	case EMBED_ITEM:				dwSize = sizeof (cmd_embed_item);			break;
	case CLEAR_TESSERA:				dwSize = sizeof (cmd_clear_tessera);		break;
	case COST_SKILL_POINT:			dwSize = sizeof (cmd_cost_skill_point);		break;
	case LEARN_SKILL:				dwSize = sizeof (cmd_learn_skill);			break;
	case OBJECT_TAKEOFF:			dwSize = sizeof (cmd_object_takeoff);		break;
	case OBJECT_LANDING:			dwSize = sizeof (cmd_object_landing);		break;
	case FLYSWORD_TIME:				dwSize = sizeof (cmd_flysword_time);		break;
	case HOST_OBTAIN_ITEM:			dwSize = sizeof (cmd_host_obtain_item);		break;
	case PRODUCE_START:				dwSize = sizeof (cmd_produce_start);		break;
	case PRODUCE_ONCE:				dwSize = sizeof (cmd_produce_once);			break;
	case PRODUCE_END:				dwSize = 0;									break;
	case DECOMPOSE_START:			dwSize = sizeof (cmd_decompose_start);		break;
	case DECOMPOSE_END:				dwSize = 0;									break;
	case TASK_DATA:

		CHECK_VALID(cmd_task_data)
		break;

	case TASK_VAR_DATA:

		CHECK_VALID(cmd_task_var_data)
		break;

	case OBJECT_START_USE:			dwSize = sizeof (cmd_object_start_use);		break;
	case OBJECT_CANCEL_USE:			dwSize = sizeof (cmd_object_cancel_use);	break;
	case OBJECT_USE_ITEM:			dwSize = sizeof (cmd_object_use_item);		break;
	case OBJECT_START_USE_T:		dwSize = sizeof (cmd_object_start_use_t);	break;
	case OBJECT_USE_ITEM_WITH_ARG:	dwSize = (DWORD)-2;							break;
	case OBJECT_SIT_DOWN:			dwSize = sizeof (cmd_object_sit_down);		break;
	case OBJECT_STAND_UP:			dwSize = sizeof (cmd_object_stand_up);		break;
	case OBJECT_DO_EMOTE:			dwSize = sizeof (cmd_object_do_emote);		break;
	case SERVER_TIME:				dwSize = sizeof (cmd_server_time);			break;
	case OBJECT_ROOT:				dwSize = sizeof (cmd_object_root);			break;
	case HOST_DISPEL_ROOT:			dwSize = sizeof (cmd_host_dispel_root);		break;
	case INVADER_RISE:				dwSize = sizeof (cmd_invader_rise);			break;
	case PARIAH_RISE:				dwSize = sizeof (cmd_pariah_rise);			break;
	case INVADER_FADE:				dwSize = sizeof (cmd_invader_fade);			break;
	case OBJECT_ATTACK_RESULT:		dwSize = sizeof (cmd_object_atk_result);	break;
	case BE_HURT:					dwSize = sizeof (cmd_be_hurt);				break;
	case HURT_RESULT:				dwSize = sizeof (cmd_hurt_result);			break;
	case HOST_STOP_SKILL:			dwSize = 0;									break;
	case UPDATE_EXT_STATE:			dwSize = sizeof (cmd_update_ext_state);		break;

	case ICON_STATE_NOTIFY:
		{
			cmd_icon_state_notify cmd;
			if (!cmd.Initialize(pDataBuf, dwDataSize, &dwSize))
				dwSize = (DWORD)-1;
			break;
		}
		break;

	case PLAYER_GATHER_START:		dwSize = sizeof (cmd_player_gather_start);	break;
	case PLAYER_GATHER_STOP:		dwSize = sizeof (cmd_player_gather_stop);	break;
	case TRASHBOX_PWD_CHANGED:		dwSize = sizeof (cmd_trashbox_pwd_changed);	break;
	case TRASHBOX_PWD_STATE:		dwSize = sizeof (cmd_trashbox_pwd_state);	break;
	case TRASHBOX_OPEN:				dwSize = sizeof (cmd_trashbox_open);		break;
	case TRASHBOX_CLOSE:			dwSize = sizeof(cmd_trashbox_close);		break;
	case TRASHBOX_WEALTH:			dwSize = sizeof (cmd_trashbox_wealth);		break;
	case EXG_TRASHBOX_ITEM:			dwSize = sizeof (cmd_exg_trashbox_item);	break;	
	case MOVE_TRASHBOX_ITEM:		dwSize = sizeof (cmd_move_trashbox_item);	break;
	case EXG_TRASHBOX_IVTR:			dwSize = sizeof (cmd_exg_trashbox_ivtr);	break;
	case IVTR_ITEM_TO_TRASH:		dwSize = sizeof (cmd_trash_item_to_ivtr);	break;
	case TRASH_ITEM_TO_IVTR:		dwSize = sizeof (cmd_ivty_item_to_trash);	break;
	case EXG_TRASH_MONEY:			dwSize = sizeof (cmd_exg_trash_money);		break;
	case ENCHANT_RESULT:			dwSize = sizeof (cmd_enchant_result);		break;
	case HOST_NOTIFY_ROOT:			dwSize = sizeof (cmd_host_notify_root);		break;
	case OBJECT_DO_ACTION:			dwSize = sizeof (cmd_obj_do_action);		break;
	case HOST_SKILL_ATTACK_RESULT:	dwSize = sizeof (cmd_host_skill_attack_result); break;
	case OBJECT_SKILL_ATTACK_RESULT:dwSize = sizeof (cmd_object_skill_attack_result); break;
	case HOST_SKILL_ATTACKED:		dwSize = sizeof (cmd_host_skill_attacked);	break;
	case PLAYER_SET_ADV_DATA:		dwSize = sizeof (cmd_player_set_adv_data);	break;
	case PLAYER_CLR_ADV_DATA:		dwSize = sizeof (cmd_player_clr_adv_data);	break;
	case PLAYER_IN_TEAM:			dwSize = sizeof (cmd_player_in_team);		break;
	case TEAM_ASK_TO_JOIN:			dwSize = sizeof (cmd_team_ask_join);		break;
	case OBJECT_EMOTE_RESTORE:		dwSize = sizeof (cmd_object_emote_restore);	break;
	case CON_EMOTE_REQUEST:			dwSize = sizeof (cmd_con_emote_request);	break;
	case DO_CONCURRENT_EMOTE:		dwSize = sizeof (cmd_do_concurrent_emote);	break;
	case MATTER_PICKUP:				dwSize = sizeof (cmd_matter_pickup);		break;
	case MAFIA_INFO_NOTIFY:			dwSize = sizeof (cmd_mafia_info_notify);	break;
	case MAFIA_TRADE_START:			dwSize = 0;									break;
	case MAFIA_TRADE_END:			dwSize = 0;									break;
	case TASK_DELIVER_ITEM:			dwSize = sizeof (cmd_task_deliver_item);	break;
	case TASK_DELIVER_REP:			dwSize = sizeof (cmd_task_deliver_rep);		break;
	case TASK_DELIVER_EXP:			dwSize = sizeof (cmd_task_deliver_exp);		break;
	case TASK_DELIVER_MONEY:		dwSize = sizeof (cmd_task_deliver_money);	break;
	case TASK_DELIVER_LEVEL2:		dwSize = sizeof (cmd_task_deliver_level2);	break;
	case HOST_REPUTATION:			dwSize = sizeof (cmd_host_reputation);		break;
	case ITEM_IDENTIFY:				dwSize = sizeof (cmd_item_identify);		break;
	case PLAYER_CHGSHAPE:			dwSize = sizeof (cmd_player_chgshape);		break;
	case ELF_REFINE_ACTIVATE:		dwSize = sizeof (cmd_elf_refine_activate);	break;
	case CAST_ELF_SKILL:			dwSize = sizeof (cmd_cast_elf_skill);		break;
	case ENTER_SANCTUARY:			dwSize = sizeof (cmd_object_enter_sanctuary); break;
	case LEAVE_SANCTUARY:			dwSize = sizeof (cmd_object_leave_sanctuary); break;

	case PLAYER_OPEN_BOOTH:

		CHECK_VALID(cmd_player_open_booth)
		break;

	case SELF_OPEN_BOOTH:

		CHECK_VALID(cmd_self_open_booth)
		break;

	case PLAYER_CLOSE_BOOTH:		dwSize = sizeof (cmd_player_close_booth);	break;
	case PLAYER_BOOTH_INFO:

		CHECK_VALID(cmd_player_booth_info)
		break;

	case BOOTH_TRADE_SUCCESS:		dwSize = sizeof (cmd_booth_trade_success);	break;

	case BOOTH_NAME:

		CHECK_VALID(cmd_booth_name)
		break;

	case PLAYER_START_TRAVEL:		dwSize = sizeof (cmd_player_start_travel);	break;
	case HOST_START_TRAVEL:			dwSize = sizeof (cmd_host_start_travel);	break;
	case PLAYER_END_TRAVEL:			dwSize = sizeof (cmd_player_end_travel);	break;
	case PLAYER_CHANGE_SPOUSE:		dwSize = sizeof (player_change_spouse);		break;
	case GM_INVINCIBLE:				dwSize = sizeof (gm_cmd_invincible);		break;
	case GM_INVISIBLE:				dwSize = sizeof (gm_cmd_invisible);			break;
	case HOST_CORRECT_POS:			dwSize = sizeof (cmd_host_correct_pos);		break;
	case OBJECT_CAST_INSTANT_SKILL:	dwSize = sizeof (cmd_object_cast_instant_skill); break;
	case ACTIVATE_WAYPOINT:			dwSize = sizeof (cmd_activate_waypoint);	break;

	case WAYPOINT_LIST:

		CHECK_VALID(cmd_waypoint_list)
		break;

	case UNFREEZE_IVTR_SLOT:		dwSize = sizeof (cmd_unfreeze_ivtr_slot);	break;
	case TEAM_INVITE_TIMEOUT:		dwSize = sizeof (cmd_team_invite_timeout);	break;
	case PLAYER_ENABLE_PVP:			dwSize = sizeof (cmd_player_enable_pvp);	break;
	case PLAYER_DISABLE_PVP:		dwSize = sizeof (cmd_player_disable_pvp);	break;
	case HOST_PVP_COOLDOWN:			dwSize = sizeof (cmd_host_pvp_cooldown);	break;

	case COOLTIME_DATA:

		CHECK_VALID(cmd_cooltime_data)
		break;

	case SKILL_ABILITY:				dwSize = sizeof (cmd_skill_ability);		break;
	case OPEN_BOOTH_TEST:			dwSize = 0;									break;
	case BREATH_DATA:				dwSize = sizeof (cmd_breath_data);			break;
	case HOST_STOP_DIVE:			dwSize = 0;									break;
	case BOOTH_SELL_ITEM:			dwSize = sizeof (cmd_booth_sell_item);		break;
	case PLAYER_ENABLE_FASHION:		dwSize = sizeof (cmd_player_enable_fashion);break;
	case HOST_ENABLE_FREEPVP:		dwSize = sizeof (cmd_host_enable_freepvp);	break;
	case INVALID_OBJECT:			dwSize = sizeof (cmd_invalid_object);		break;
	case PLAYER_ENABLE_EFFECT:		dwSize = sizeof (cmd_player_enable_effect);	break;
	case PLAYER_DISABLE_EFFECT:		dwSize = sizeof (cmd_player_disable_effect);break;
	case REVIVAL_INQUIRE:			dwSize = sizeof (cmd_revival_inquire);		break;
	case SET_COOLDOWN:				dwSize = sizeof (cmd_set_cooldown);			break;
	case CHANGE_TEAM_LEADER:		dwSize = sizeof (cmd_change_team_leader);	break;
	case EXIT_INSTANCE:				dwSize = sizeof (cmd_exit_instance);		break;
	case CHANGE_FACE_START:			dwSize = sizeof (cmd_change_face_start);	break;
	case CHANGE_FACE_END:			dwSize = sizeof (cmd_change_face_end);		break;
	case PLAYER_CHG_FACE:			dwSize = sizeof (cmd_player_chg_face);		break;
	case OBJECT_CAST_POS_SKILL:		dwSize = sizeof (cmd_object_cast_pos_skill);break;
	case SET_MOVE_STAMP:			dwSize = sizeof (cmd_set_move_stamp);		break;
	case INST_DATA_CHECKOUT:		dwSize = sizeof (cmd_inst_data_checkout);	break;
	case HOST_RUSH_FLY:				dwSize = sizeof (cmd_host_rush_fly);		break;
	case TRASHBOX_SIZE:				dwSize = sizeof (cmd_trashbox_size);		break;
	case NPC_DIED2:					dwSize = sizeof (cmd_npc_died2);			break;
	case PRODUCE_NULL:				dwSize = sizeof (cmd_produce_null);			break;
	case PVP_COMBAT:				dwSize = sizeof (cmd_pvp_combat);			break;
	case DOUBLE_EXP_TIME:			dwSize = sizeof (cmd_double_exp_time);		break;
	case AVAILABLE_DOUBLE_EXP_TIME:	dwSize = sizeof (cmd_available_double_exp_time); break;
	case DUEL_RECV_REQUEST:			dwSize = sizeof (cmd_duel_recv_request);	break;
	case DUEL_REJECT_REQUEST:		dwSize = sizeof (cmd_duel_reject_request);	break;
	case DUEL_PREPARE:				dwSize = sizeof (cmd_duel_prepare);			break;
	case DUEL_CANCEL:				dwSize = sizeof (cmd_duel_cancel);			break;
	case HOST_DUEL_START:			dwSize = sizeof (cmd_host_duel_start);		break;
	case DUEL_STOP:					dwSize = sizeof (cmd_duel_stop);			break;
	case DUEL_RESULT:				dwSize = sizeof (cmd_duel_result);			break;
	case PLAYER_BIND_REQUEST:		dwSize = sizeof (cmd_player_bind_request);	break;
	case PLAYER_BIND_INVITE:		dwSize = sizeof (cmd_player_bind_invite);	break;
	case PLAYER_BIND_REQUEST_REPLY:	dwSize = sizeof (cmd_player_bind_request_reply);	break;
	case PLAYER_BIND_INVITE_REPLY:	dwSize = sizeof (cmd_player_bind_invite_reply);		break;
	case PLAYER_BIND_START:			dwSize = sizeof (cmd_player_bind_start);	break;
	case PLAYER_BIND_STOP:			dwSize = sizeof (cmd_player_bind_stop);		break;
	case PLAYER_MOUNTING:			dwSize = sizeof (cmd_player_mounting);		break;
	case PLAYER_EQUIP_DETAIL:		dwSize = (DWORD)(-2);						break;
	case PLAYER_DUEL_START:			dwSize = sizeof (cmd_player_duel_start);	break;
	case PARIAH_TIME:				dwSize = sizeof (cmd_pariah_time);			break;
	case GAIN_PET:					dwSize = sizeof (cmd_gain_pet);				break;
	case FREE_PET:					dwSize = sizeof (cmd_free_pet);				break;
	case SUMMON_PET:				dwSize = sizeof (cmd_summon_pet);			break;
	case RECALL_PET:				dwSize = sizeof (cmd_recall_pet);			break;
	case PLAYER_START_PET_OP:		dwSize = sizeof (cmd_player_start_pet_op);	break;
	case PLAYER_STOP_PET_OP:		dwSize = 0;									break;
	case PET_RECEIVE_EXP:			dwSize = sizeof (cmd_pet_receive_exp);		break;
	case PET_LEVELUP:				dwSize = sizeof (cmd_pet_levelup);			break;

	case PET_ROOM:
	{
		const cmd_pet_room* pCmd = (const cmd_pet_room*)pDataBuf;
		dwSize = sizeof (cmd_pet_room) + pCmd->count * (sizeof (int) + sizeof (info_pet));
		break;
	}
	case PET_ROOM_CAPACITY:			dwSize = sizeof (cmd_pet_room_capacity);	break;
	case PET_HONOR_POINT:			dwSize = sizeof (cmd_pet_honor_point);		break;
	case PET_HUNGER_GAUGE:			dwSize = sizeof (cmd_pet_hunger_gauge);		break;
	case HOST_ENTER_BATTLE:			dwSize = sizeof (cmd_host_enter_battle);	break;
	case TANK_LEADER_NOTIFY:		dwSize = sizeof (cmd_tank_leader_notify);	break;
	case BATTLE_RESULT:				dwSize = sizeof (cmd_battle_result);		break;
	case BATTLE_SCORE:				dwSize = sizeof (cmd_battle_score);			break;
	case PET_DEAD:					dwSize = sizeof (cmd_pet_dead);				break;
	case PET_REVIVE:				dwSize = sizeof (cmd_pet_revive);			break;
	case PET_HP_NOTIFY:				dwSize = sizeof (cmd_pet_hp_notify);		break;
	case PET_AI_STATE:				dwSize = sizeof (cmd_pet_ai_state);			break;
	case REFINE_RESULT:				dwSize = sizeof (cmd_refine_result);		break;
	case PET_SET_COOLDOWN:			dwSize = sizeof (cmd_pet_set_cooldown);		break;
	case PLAYER_CASH:				dwSize = sizeof	(player_cash);				break;
	case PLAYER_DIVIDEND:			dwSize = sizeof (cmd_player_dividend);		break;
	case PLAYER_BIND_SUCCESS:		dwSize = sizeof (cmd_player_bind_success);	break;
	case CHANGE_IVTR_SIZE:			dwSize = sizeof (cmd_change_ivtr_size);		break;
	case PVP_MODE:					dwSize = sizeof (pvp_mode);					break;
	case PLAYER_WALLOW_INFO:		dwSize = sizeof (player_wallow_info);		break;
	case NOTIFY_SAFE_LOCK:			dwSize = sizeof (notify_safe_lock);			break;
	case ELF_VIGOR:					dwSize = sizeof (cmd_elf_vigor);			break;
	case ELF_ENHANCE:				dwSize = sizeof (cmd_elf_enhance);			break;
	case ELF_STAMINA:				dwSize = sizeof (cmd_elf_stamina);			break;
	case ELF_EXP:					dwSize = sizeof (cmd_elf_exp);				break;
	case ELF_CMD_RESULT:			dwSize = sizeof (cmd_elf_result);			break;
	case COMMON_DATA_NOTIFY:		dwSize = (DWORD)(-2);						break;
	case COMMON_DATA_LIST:			dwSize = (DWORD)(-2);						break;
	case MALL_ITEM_PRICE:			dwSize = (DWORD)(-2);						break;
	case MALL_ITEM_BUY_FAILED:		dwSize = sizeof (cmd_mall_item_buy_failed);	break; 
	case GOBLIN_LEVEL_UP:			dwSize = sizeof (cmd_goblin_level_up);		break;
	case PLAYER_PROPERTY:			dwSize = sizeof (cmd_player_property);		break;
	case PLAYER_EQUIP_TRASHBOX_ITEM: dwSize = sizeof (cmd_player_equip_trashbox_item); break;
	case OBJECT_INVISIBLE:			dwSize = sizeof (cmd_object_invisible);		break;
	case SECURITY_PASSWD_CHECKED:	dwSize = 0;									break;
	case PLAYER_HP_STEAL:			dwSize = sizeof (cmd_player_hp_steal);		break;
	case DIVIDEND_MALL_ITEM_PRICE:	dwSize = (DWORD)(-2);						break;
	case DIVIDEND_MALL_ITEM_BUY_FAILED: dwSize = sizeof (cmd_dividend_mall_item_buy_failed); break;
	case PUBLIC_QUEST_INFO:			dwSize = sizeof (cmd_public_quest_info);	break;
	case PUBLIC_QUEST_RANKS:		dwSize = (DWORD)(-2);						break;
	case MULTI_EXP_INFO:            dwSize = sizeof(cmd_multi_exp_info);        break;
	case CHANGE_MULTI_EXP_STATE:    dwSize = sizeof(cmd_change_multi_exp_state);break;
	case WORLD_LIFE_TIME:			dwSize = sizeof(cmd_world_life_time);		break;
	case WEDDING_BOOK_LIST:			dwSize = (DWORD)(-2);						break;
	case WEDDING_BOOK_SUCCESS:		dwSize = sizeof(cmd_wedding_book_success);	break;
	case CALC_NETWORK_DELAY_RE:		dwSize = sizeof(cmd_network_delay_re);		break;
	case PLAYER_KNOCKBACK:			dwSize = sizeof(cmd_player_knockback);		break;
	case SUMMON_PLANT_PET:			dwSize = sizeof(cmd_summon_plant_pet);		break;
	case PLANT_PET_DISAPPEAR:		dwSize = sizeof(cmd_plant_pet_disapper);	break;
	case PLANT_PET_HP_NOTIFY:		dwSize = sizeof(cmd_plant_pet_hp_notify);	break;
	case PET_PROPERTY:				dwSize = sizeof(cmd_pet_property);			break;
	case FACTION_CONTRIB_NOTIFY:	dwSize = sizeof(cmd_faction_contrib_notify);break;
	case FACTION_FORTRESS_INFO:
		{
			cmd_faction_fortress_info cmd;
			if (!cmd.Initialize(pDataBuf, dwDataSize, &dwSize))
				dwSize = (DWORD)-1;
			break;
		}
	case ENTER_FACTIONFORTRESS:		dwSize = sizeof(cmd_enter_factionfortress);	break;
	case FACTION_RELATION_NOTIFY:
		{
			cmd_faction_relation_notify cmd;
			if (!cmd.Initialize(pDataBuf, dwDataSize, &dwSize))
				dwSize = (DWORD)-1;
			break;
		}
	case PLAYER_EQUIP_DISABLED:		dwSize = sizeof(cmd_player_equip_disabled);		break;
	case PLAYER_SPEC_ITEM_LIST:		CHECK_VALID(player_spec_item_list);	break;

	case OBJECT_START_PLAY_ACTION:

		CHECK_VALID(cmd_object_start_play_action);
		break;

	case OBJECT_STOP_PLAY_ACTION:	dwSize = sizeof(cmd_object_stop_play_action);break;

	case CONGREGATE_REQUEST:		dwSize = sizeof(cmd_congregate_request);break;
	case REJECT_CONGREGATE:			dwSize = sizeof(cmd_reject_congregate);break;
	case CONGREGATE_START:			dwSize = sizeof(cmd_congregate_start);break;
	case CANCEL_CONGREGATE:			dwSize = sizeof(cmd_cancel_congregate);break;
	case ENGRAVE_START:					dwSize = sizeof(cmd_engrave_start);	break;
	case ENGRAVE_END:					dwSize = 0;	break;
	case ENGRAVE_RESULT:				dwSize = sizeof(cmd_engrave_result);	break;

	case DPS_DPH_RANK:

		CHECK_VALID(cmd_dps_dph_rank);
		break;

	case ADDONREGEN_START:				dwSize = sizeof(cmd_addonregen_start);	break;
	case ADDONREGEN_END:				dwSize = 0;	break;
	case ADDONREGEN_RESULT:				dwSize = sizeof(cmd_addonregen_result);	break;
	case INVISIBLE_OBJ_LIST:
		CHECK_VALID(cmd_invisible_obj_list);
		break;

	case SET_PLAYER_LIMIT:		dwSize = sizeof(cmd_set_player_limit);	break;
	case PLAYER_TELEPORT:		dwSize = sizeof(cmd_player_teleport);	break;
	case OBJECT_FORBID_BE_SELECTED:	dwSize = sizeof (cmd_object_forbid_be_selected);	break;

	case PLAYER_INVENTORY_DETAIL:
		CHECK_VALID(cmd_player_inventory_detail);
		break;

	case PLAYER_FORCE_DATA:
		CHECK_VALID(cmd_player_force_data);
		break;

	case PLAYER_FORCE_CHANGED:		dwSize = sizeof (cmd_player_force_changed);	break;
	case PLAYER_FORCE_DATA_UPDATE:	dwSize = sizeof (cmd_player_force_data_update);	break;

	case FORCE_GLOBAL_DATA:
		CHECK_VALID(cmd_force_global_data);
		break;
	case ADD_MULTIOBJECT_EFFECT:
	case REMOVE_MULTIOBJECT_EFFECT:
		dwSize = sizeof(cmd_multiobj_effect); 
		break;
	case ENTER_WEDDING_SCENE:
		dwSize = sizeof(enter_wedding_scene);
		break;
	case PRODUCE4_ITEM_INFO:
		CHECK_VALID(produce4_item_info);
		break;
	case ONLINE_AWARD_DATA:
		CHECK_VALID(cmd_online_award_data);
		break;
	case TOGGLE_ONLINE_AWARD:
		dwSize = sizeof(cmd_toggle_online_award);
		break;
	case PLAYER_PROFIT_TIME:		dwSize = sizeof(cmd_player_profit_time); break;
	case ENTER_NONPENALTY_PVP_STATE: dwSize = sizeof(cmd_nonpenalty_pvp_state); break;
	case SELF_COUNTRY_NOTIFY:		dwSize = sizeof(cmd_self_country_notify); break;
	case PLAYER_COUNTRY_CHANGED: dwSize = sizeof(cmd_player_country_changed); break;
	case ENTER_COUNTRYBATTLE:	dwSize = sizeof(cmd_enter_countrybattle); break;
	case COUNTRYBATTLE_RESULT:	dwSize = sizeof(cmd_countrybattle_result); break;
	case COUNTRYBATTLE_SCORE:	dwSize = sizeof(cmd_countrybattle_score); break;
	case COUNTRYBATTLE_RESURRECT_REST_TIMES:
		dwSize = sizeof(cmd_countrybattle_resurrect_rest_times); break;
	case COUNTRYBATTLE_FLAG_CARRIER_NOTIFY:
		dwSize = sizeof(cmd_countrybattle_flag_carrier_notify); break;
	case COUNTRYBATTLE_BECAME_FLAG_CARRIER:
		dwSize = sizeof(cmd_countrybattle_became_flag_carrier); break;
	case COUNTRYBATTLE_PERSONAL_SCORE:
		dwSize = sizeof(cmd_countrybattle_personal_score); break;
	case COUNTRYBATTLE_FLAG_MSG_NOTIFY:
		dwSize = sizeof(cmd_countrybattle_flag_msg_notify); break;
	case DEFENSE_RUNE_ENABLED:
		dwSize = sizeof(cmd_defense_rune_enable); break;
	case COUNTRYBATTLE_INFO:
		dwSize = sizeof(cmd_countrybattle_info); break;
	case SET_PROFIT_TIME:
		dwSize = sizeof(cmd_set_profit_time); break;
	case CASH_MONEY_EXCHG_RATE:	dwSize = sizeof(cmd_cash_money_exchg_rate);	break;
	case PET_REBUILD_INHERIT_START:
		dwSize = sizeof(cmd_pet_rebuild_inherit_start); break;
	case PET_REBUILD_INHERIT_INFO:
		dwSize = sizeof(cmd_pet_rebuild_inherit_info); break;
	case PET_REBUILD_INHERIT_END:
		dwSize = sizeof(cmd_pet_rebuild_inherit_end); break;
	case PET_EVOLUTION_DONE:
		dwSize = sizeof(cmd_pet_evolution_done); break;
	case PET_REBUILD_NATURE_START:
		dwSize = sizeof(cmd_pet_rebuild_nature_start); break;
	case PET_REBUILD_NATURE_INFO:
		dwSize = sizeof(cmd_pet_rebuild_nature_info); break;
	case PET_REBUILD_NATURE_END:
		dwSize = sizeof(cmd_pet_rebuild_nature_end); break;
	case MERIDIANS_NOTIFY:			
		dwSize = sizeof (cmd_notify_meridian_data);		break;
	case MERIDIANS_RESULT:			
		dwSize = sizeof (cmd_try_refine_meridian_result);		break;
	case EQUIP_ADDON_UPDATE_NOTIFY:
		dwSize = sizeof(cmd_equip_addon_update_notify);break;
	case SELF_KING_NOTIFY:
		dwSize = sizeof(cmd_self_king_notify); break;
	case PLAYER_KING_CHANGED:
		dwSize = sizeof(cmd_player_king_changed); break;
	case COUNTRYBATTLE_STRONGHOND_STATE_NOTIFY:
		CHECK_VALID(cmd_stronghold_state_notify);
		break;
	case QUERY_TOUCH_POINT:		dwSize = sizeof(cmd_query_touch_point); break;
	case SPEND_TOUCH_POINT:		dwSize = sizeof(cmd_spend_touch_point); break;
	case TOTAL_RECHARGE:		dwSize = sizeof(cmd_total_recharge); break;
	case QUERY_TITLE_RE:		CHECK_VALID(cmd_query_title_re); break;
	case CHANGE_CURR_TITLE_RE:	dwSize = sizeof(cmd_change_curr_title_re); break;
	case MODIFY_TITLE_NOFIFY:	dwSize = sizeof(cmd_modify_title_notify); break;
	case REFRESH_SIGNIN:		dwSize = sizeof(cmd_refresh_signin); break;
	case PARALLEL_WORLD_INFO:	CHECK_VALID(cmd_parallel_world_info); break;
	case PLAYER_REINCARNATION:	dwSize = sizeof(cmd_player_reincarnation); break;
	case REINCARNATION_TOME_INFO: CHECK_VALID(cmd_reincarnation_tome_info); break;
	case ACTIVATE_REINCARNATION_TOME: dwSize = sizeof(cmd_activate_reincarnation_tome); break;
	case UNIQUE_DATA_NOTIFY:	
		{
			cmd_unique_data_notify cmd;
			if (!cmd.Initialize((const void*)pDataBuf, dwDataSize, &dwSize))
				dwSize = (DWORD)-1;
		}
		break;
	case USE_GIFTCARD_RESULT:	dwSize = sizeof(cmd_use_giftcard_result); break;
	case REALM_EXP:				dwSize = sizeof(cmd_realm_exp); break;
	case REALM_LEVEL:			dwSize = sizeof(cmd_realm_level); break;
	case ENTER_TRICKBATTLE:		dwSize = sizeof(cmd_enter_trickbattle); break;
	case TRICKBATTLE_CHARIOT_INFO: dwSize = sizeof(cmd_trickbattle_chariot_info) ;break;
	case TRICKBATTLE_PERSONAL_SCORE: dwSize = sizeof(cmd_trickbattle_personal_score); break;
	case PLAYER_LEADERSHIP:		dwSize = sizeof(cmd_player_leadership); break;
	case GENERALCARD_COLLECTION_DATA: CHECK_VALID(cmd_generalcard_collection_data); break;
	case ADD_GENERALCARD_COLLECTION: dwSize = sizeof(cmd_add_generalcard_collection); break;
	case REFRESH_MONSTERSPIRIT_LEVEL: CHECK_VALID(cmd_refresh_monsterspirit_level); break;
	case MINE_GATHERED: dwSize = sizeof(cmd_mine_gathered); break;
	case PLAYER_IN_OUT_BATTLE: dwSize = sizeof(cmd_player_in_out_battle); break;

	default:
		break;
	}

	return dwSize;
}

//	Game data handler
void GameSession::ProcessGameData(const GNET::Octets& Data)
{
	using namespace S2C;

	DWORD dwDataSize = Data.size();

	if (dwDataSize < sizeof(cmd_header))
	{
		assert(0);
		return;
	}

	dwDataSize -= sizeof(cmd_header);

	//	Get common command header
	BYTE* pDataBuf = (BYTE*)Data.begin();
	cmd_header* pCmdHeader = (cmd_header*)pDataBuf;
	pDataBuf += sizeof (cmd_header);

	//	Check command size
	DWORD dwCmdSize = CalcS2CCmdDataSize(pCmdHeader->cmd, pDataBuf, dwDataSize);

	//	Size == -1 means we haven't handled this command
	if (dwCmdSize == (DWORD)(-1))
	{
		return;
	}
	else if (dwCmdSize != (DWORD)(-2))
	{
		ASSERT(dwCmdSize == dwDataSize);
		if (dwCmdSize != dwDataSize)
			return;
	}

	// 执行S2C响应
	switch( pCmdHeader->cmd )
	{
	case SELF_INFO_1:
		{
			cmd_self_info_1* pCmd = (cmd_self_info_1*)pDataBuf;
			DoOvertimeCheck(false, GameSession::OT_ENTERGAME, 0);

			// 玩家EnterWorld成功，开始获取数据
			c2s_CmdGetAllData(true, true, false);
		}
		break;
	case TASK_DATA:
		{
			OutputDebugStringA("TASK_DATA\n");

			// 标志着玩家的基本数据获取完
			if (!IsTestInWorld(m_iGameResult)){
				m_iGameResult = TEST_SUCCESSFUL;
				CheckAddVersionError();
				
				//	获取国战总奖励
				if (m_qCountryWarBonus.BeginQuery(30000)){
					m_iGameResult |= TEST_COUNTRYWAR_BONUS_ERR;	//	默认设置为错误，以防止未设置情形
					getServerConfig(SCK_COUNTRYBATTLE_BONUS);
				}

				//  测试跨服过程
 				if (IsTestCrossServer())
				{
					if( m_qCrossServer.GetOP() == 0 && m_qCrossServer.BeginQuery(20000) )
					{
						// 跨到中央服
						m_iGameResult |= TEST_CROSSSERVER_ERR;
						m_qCrossServer.SetOP(QueryCrossServer::GO);
						gm_ChangeDS(true);
						m_qCrossServer.IncTimeLength(30000);	// ChangeDS有可能冷却30秒
					}
					else if( (m_qCrossServer.GetOP() == QueryCrossServer::GO && m_qCrossServer.BeginQuery(20000))
						 || m_qCrossServer.GetOP() == QueryCrossServer::ENTERWORLD)
					{
						// 跨服测试成功！
						GNET::Marshal::OctetsStream o;
						o << int(2) << int(0);
						if( m_qCrossServer.SetResult(o) == TimedQuery::QR_SUCCESS )
							m_iGameResult &= ~TEST_CROSSSERVER_ERR;	
					}
				}
			}
		}
		break;
	case COOLTIME_DATA:
		{
			cmd_cooltime_data* pCmd = (cmd_cooltime_data*)pDataBuf;
			for (int i=0; i < pCmd->count; i++)
			{
				const cmd_cooltime_data::item_t& item = pCmd->list[i];

				if (item.idx >= 0 && item.idx < GP_CT_MAX)
				{
					//	Other cool time
					COOLTIME& ct = m_aCoolTimes[item.idx];
					ct.iCurTime = item.cooldown;
					ct.iMaxTime	= item.max_cooltime;
					a_Clamp(ct.iCurTime, 0, ct.iMaxTime);
				}
			}
		}
		break;
	case SET_COOLDOWN:
		{
			cmd_set_cooldown* pCmd = (cmd_set_cooldown*)pDataBuf;

			if (pCmd->cooldown_index < 0)
			{
				ASSERT(pCmd->cooldown_index >= 0);
				return;
			}

			if (pCmd->cooldown_index < GP_CT_MAX)
			{
				COOLTIME& ct = m_aCoolTimes[pCmd->cooldown_index];
				ct.iCurTime = pCmd->cooldown_time;
				ct.iMaxTime	= pCmd->cooldown_time;
				a_ClampRoof(ct.iCurTime, ct.iMaxTime);
			}
		}
		break;
	}
}

bool GameSession::TestFinished()
{
	bool bFinished(false);
	if (IsTestInWorld(m_iGameResult)){
		bFinished = true;
		for (TimedQueries::const_iterator cit = m_timedQueries.begin(); cit != m_timedQueries.end(); ++ cit)
		{
			const TimedQuery * q = *cit;
			if (!q->Finished()){
				bFinished = false;
				break;
			}
		}
	}
	return bFinished;
}

void GameSession::CheckAddVersionError()
{
	if (!m_strVersionErr.IsEmpty()){
		m_iGameResult |= TEST_VERSION_ERR;
	}
}

bool GameSession::GetQueryResults(AString &strResults)
{
	bool bAllSuccess(true);
	strResults.Empty();
	AString str;
	for (TimedQueries::const_iterator cit = m_timedQueries.begin(); cit != m_timedQueries.end(); ++ cit)
	{
		const TimedQuery * q = *cit;
		if (!strResults.IsEmpty()){
			strResults += "，";
		}
		if (!q->GetResultAsString(str)){
			bAllSuccess = false;
		}
		strResults += str;
	}
	return bAllSuccess;
}

void GameSession::OnPrtcChallenge(GNET::Protocol* pProtocol)
{
	using namespace GNET;

	DoOvertimeCheck(false, OT_CHALLENGE, 0);
	Challenge* p = (Challenge*)pProtocol;

	if( GAME_VERSION && !GAME_VERSION_STRING.IsEmpty() )
	{
		// 做客户端和服务器的版本验证
		if (p->version != GAME_VERSION)
		{
			m_strVersionErr.Format("大版本不符(服务器版本: %s)", BigVersionStr(p->version));
		}
		else
		{
			AString str((const char *)p->edition.begin(), p->edition.size());
			if( str != GAME_VERSION_STRING )
				m_strVersionErr.Format("小版本不符(服务器版本: %s)", str);
		}
		if (!m_strVersionErr.IsEmpty()){
			m_iGameResult |= TEST_VERSION_ERR;
		}
	}

	AString str;
	str.Format("OnPrtcChallenge sid:%d\n", m_idLink);
	OutputDebugStringA(str);
	//OutputDebugStringA("OnPrtcChallenge sid: %d\n");

	// 处理跨服登录
	if( IsWaitingRelogin() )
	{
		int iSrcZoneID = GetOrgServer().zoneid;
		PlayerChangeDS_Re* pPlayerChangeDS_Re = m_qCrossServer.GetProto();

		//	回复 Challenge
		KeyReestablish kr;
		kr.roleid = pPlayerChangeDS_Re->remote_roleid;
		kr.userid = pPlayerChangeDS_Re->userid;
		kr.flag = pPlayerChangeDS_Re->flag;
		kr.src_zoneid = iSrcZoneID;
		kr.random = pPlayerChangeDS_Re->random;
		SendNetData(kr);

		str.Format("src_zoneid: %d\n", iSrcZoneID);
		OutputDebugStringA(str);

		//	KeyReestablish 协议发送不加密，SendNetData 添加到缓冲区的只是未加密的
		//	因此，下面不能立即设置 OSecurity，否则将导致 KeyReestablish 以加密发出
		
		//	加密接收链接
		m_pNetMan->SetISecurity(m_idLink, DECOMPRESSARCFOURSECURITY, m_qCrossServer.GetISecurity());

		//	加密发送链接等接收到下个协议时进行，即 OnlineAnnounce 中
		return;
	}

	GameClient::LoginParameter para;
	para.name = m_strUserName;
	para.passwd = m_strPassword;
	para.nonce = p->nonce;
	para.algo = p->algo;
	para.use_token = 0;

	Response res;
	m_pNetMan->SetupResponse(res, para);

	//	增加客户端标识，用于服务器端统计
	int iClientID = -1;
	res.cli_fingerprint.insert(res.cli_fingerprint.end(), &iClientID, sizeof(iClientID));
	
	if (!m_strPassword.IsEmpty())
	{
		//	有密码的时候清除密码（使用 token 登录时无密码）
		char * szBuffer = m_strPassword.LockBuffer();
		memset(szBuffer, 0, strlen(szBuffer));
		m_strPassword.UnlockBuffer();
	}
	
	SendNetData(res);
}

void GameSession::OnPrtcKeyExchange(GNET::Protocol* pProtocol)
{
	using namespace GNET;
	
	Octets iKey, oKey;
	KeyExchange* p = (KeyExchange*)pProtocol;
	p->Setup(m_pNetMan, m_idLink, m_strUserName, m_bKickUser, oKey, iKey);
	
	//	记录加密参数用于转服
	m_qCrossServer.SetOSecurity(oKey);
	m_qCrossServer.SetISecurity(iKey);

	SendNetData(p);
}

void GameSession::OnPrtcOnlineAnnounce(GNET::Protocol* pProtocol)
{
	using namespace GNET;
	
	OnlineAnnounce* p = (OnlineAnnounce*)pProtocol;
	m_iUserID = p->userid;
	m_pNetMan->SetZoneID(p->zoneid);
OutputDebugStringA("OnPrtcOnlineAnnounce\n");
	// 处理跨服登录的情况
	if( IsWaitingRelogin() )
	{
		//	加密发送链接
		m_pNetMan->SetOSecurity(m_idLink, ARCFOURSECURITY, m_qCrossServer.GetOSecurity());

		//	此时才修改状态为 STATE_KEEPING，防止 IO 线程发送未加密的 KeepAlive 协议
		m_pNetMan->ChangeState(m_idLink, &state_GSelectRoleClient);

		PlayerChangeDS_Re* pPlayerChangeDS_Re = m_qCrossServer.GetProto();

		//	模仿 RoleList 及 RoleListRe 协议，添加惟一角色
		RoleInfo info;
		Marshal::OctetsStream(pPlayerChangeDS_Re->roleinfo_pack) >> info;

		//	选中角色并进入游戏命令
		SetCharacterID(info.roleid);
		SendNetData(SelectRole(info.roleid, pPlayerChangeDS_Re->flag));
		return;
	}

	m_pNetMan->ChangeState(m_idLink, &state_GSelectRoleClient);
	SendNetData(RoleList(m_iUserID, _SID_INVALID, _HANDLE_BEGIN));
}

void GameSession::OnPrtcMatrixChallenge(GNET::Protocol* pProtocol)
{
	using namespace GNET;

	m_strReason = ERROR_LOGINNEEDMATRIX;
	m_iGameResult = TEST_ERROR;
	CheckAddVersionError();
	Close();
}

void GameSession::OnPrtcResetPosition(GNET::Protocol* pProtocol)
{
	using namespace GNET;

	PlayerPositionResetRqst* p = (PlayerPositionResetRqst*)pProtocol;
	PlayerPositionResetRqstArg* pArg = (PlayerPositionResetRqstArg*)p->GetArgument();

	int roleid = m_RoleList[m_iCurRoleIdx].roleid;
	if (roleid != pArg->roleid)
	{
		//	等待服务器处理
		ASSERT(false);
		a_LogOutput(1, "CECGameSession::OnPrtcResetPosition, different roleid(%d!=%d)", roleid, pArg->roleid);
		p->Destroy();
		return;
	}

	// 如果提示副本进不去，将玩家移到其他地方再登陆
	PlayerPositionResetRqstRes* pResult = (PlayerPositionResetRqstRes*)p->GetResult();
	pResult->retcode = 1;
	SendNetData(p);

	p->Destroy();
}

void GameSession::OnPrtcRoleListRe(GNET::Protocol* pProtocol)
{
	using namespace GNET;
	
	RoleList_Re* p = (RoleList_Re*)pProtocol;
	if (p->result != ERR_SUCCESS)
	{
		if (p->handle != _HANDLE_END)
			SendNetData(RoleList(m_iUserID, _SID_INVALID, p->handle));
		else
		{
			OutputLinkSevError(p->result);
			Close();
		}
		
		return;
	}

	ASSERT(m_iUserID == p->userid);
	
	int iNumRole = p->rolelist.size();
	for( int i=0;i<iNumRole;i++ )
	{
		if( p->rolelist[i].status != _ROLE_STATUS_NORMAL &&
			p->rolelist[i].status != _ROLE_STATUS_ON_CROSSSERVER )
			continue;

		ROLEINFO role;
		role.roleid = p->rolelist[i].roleid;
		role.worldid = p->rolelist[i].worldtag;
		role.roleName = AWString((const wchar_t*)p->rolelist[i].name.begin(), p->rolelist[i].name.size()/sizeof(wchar_t));
		role.profession = p->rolelist[i].occupation;
		role.level = p->rolelist[i].level;
		role.level2 = p->rolelist[i].level2;
		m_RoleList.push_back(role);
	}

	if (p->handle != _HANDLE_END)
		SendNetData(RoleList(m_iUserID, _SID_INVALID, p->handle));
	else
	{
		if( m_RoleList.size() )
		{
			if( !IsTestCrossServer() )
			{
				m_iCurRoleIdx = a_Random(0, m_RoleList.size()-1);
				SetCharacterID(m_RoleList[m_iCurRoleIdx].roleid);
				SendNetData(SelectRole(m_RoleList[m_iCurRoleIdx].roleid));
			}
			else
			{
				bool bFound = false;

				// 使用超过100级，修真过20的GM账号登陆，没有的话则返回失败
				for(size_t n=0; n<m_RoleList.size(); n++)
				{
					if( m_RoleList[n].level >= 100 && m_RoleList[n].level2 > 20 )
					{
						bFound = true;
						m_iCurRoleIdx = (int)n;
						break;
					}
				}

				if( bFound )
				{
					SetCharacterID(m_RoleList[m_iCurRoleIdx].roleid);
					SendNetData(SelectRole(m_RoleList[m_iCurRoleIdx].roleid));
				}
				else
				{
					m_strReason = ERROR_HAVENOGMROLE;
					m_iGameResult = TEST_ERROR;
					CheckAddVersionError();
					Close();
				}
			}
		}
		else
		{
			if( !IsTestCrossServer() )
			{
				// 创建新角色
				m_iCreateRoleErr = 0;

				char temp[100];
				sprintf( temp, "tr_%d", a_Random());
				if( strlen(temp) > 9 ) temp[9] = '\0';
				CreateTheRole(temp);
			}
			else
			{
				m_strReason = ERROR_HAVENOGMROLE;
				m_iGameResult = TEST_ERROR;
				CheckAddVersionError();
				Close();
			}
		}
	}
}

void GameSession::OnPrtcCreateRoleRe(GNET::Protocol* pProtocol)
{
	using namespace GNET;

	CreateRole_Re* p = (CreateRole_Re*)pProtocol;
	if (p->result != ERR_SUCCESS)
	{
		if( p->result == ERR_DUPLICATRECORD ||
			p->result == 25 /* 可能出现了非法字符 如64, 89什么的 */ )
		{
			// 有重复名字存在，则在重复名字后面加上后缀数字
			a_LogOutput(1, "GameSession::OnPrtcCreateRoleRe, The role name is invalid, we will recreate. User:%s", m_strUserName);

			if( m_iCreateRoleErr++ > 10 )
			{
				m_strReason = ERROR_CREATEROLEFAIL;
				Close();
			}
			else
			{
				char temp[100];
				sprintf( temp, "tr_%d", a_Random());
				if( strlen(temp) > 9 ) temp[9] = '\0';

				// 重新创建新角色
				CreateTheRole(temp);
			}
		}
		else
		{
			OutputLinkSevError(p->result);
			Close();
		}

		return;
	}

	// 这里添加新角色信息并自动登陆
	ROLEINFO role;
	role.roleid = p->roleinfo.roleid;
	role.roleName = AWString((const wchar_t*)p->roleinfo.name.begin(), p->roleinfo.name.size()/sizeof(wchar_t));
	role.worldid = p->roleinfo.worldtag;
	role.level = p->roleinfo.level;
	role.level2 = p->roleinfo.level2;
	m_RoleList.push_back(role);
	m_iCurRoleIdx = 0;
	SetCharacterID(m_RoleList[m_iCurRoleIdx].roleid);
	SendNetData(SelectRole(m_RoleList[m_iCurRoleIdx].roleid));
}

// 创建角色
void GameSession::CreateTheRole(const char* rolename)
{
	const int equip_size = 29;
	int i;
	GNET::RoleInfo Info;
	
	std::wstring name = StringToWString(rolename);
	Info.occupation = a_Random(PROF_WARRIOR, PROF_MEILING);
	if( Info.occupation == PROF_HAG )
		Info.gender = GENDER_FEMALE;	// 妖精只可能是女性
	else if( Info.occupation == PROF_ORC )
		Info.gender = GENDER_MALE;		// 妖兽只可能是男性
	else
		Info.gender = a_Random(GENDER_MALE, GENDER_FEMALE);
	Info.name.replace(name.c_str(), name.size() * sizeof(wchar_t));
	for( i = 0; i < equip_size; i++ )
		Info.equipment.add(0);
	
	GNET::Octets refID;
	refID.replace(NULL, 0);
	
	GNET::CreateRole cr;
	cr.userid	= m_iUserID;
	cr.roleinfo	= Info;
	cr.referid  = refID;
    SendNetData(cr);

	a_LogOutput(1, "GameSession::CreateTheRole, Create role(%s) For user:%s", rolename, m_strUserName);
}

void GameSession::OnPrtcSelectRoleRe(GNET::Protocol* pProtocol)
{
	using namespace GNET;
	
	SelectRole_Re* p = (SelectRole_Re*)pProtocol;

	if( p->result == 431 )
	{
		// 角色重新定位后登陆
		int iRoleID = m_RoleList[m_iCurRoleIdx].roleid;
		SetCharacterID(iRoleID);
		SendNetData(GNET::SelectRole(iRoleID));
		return;
	}

	if (p->result != ERR_SUCCESS)
	{
		OutputLinkSevError(p->result);
		Close();
		return;
	}

	OutputDebugStringA("OnPrtcSelectRole_Re\n");
	//g_pGame->ResetPrivilege(p->auth.GetVector().empty() ? NULL : &p->auth.GetVector()[0], p->auth.size());

	if (IsWaitingRelogin())
		m_qCrossServer.OnLoginSuccess();
	StartGame();
}

//	Start game
bool GameSession::StartGame()
{
	m_pNetMan->ChangeState(m_idLink, &GNET::state_GDataExchgClient);
	bool bRet = SendNetData(GNET::EnterWorld(m_iCharID, _SID_INVALID));
	DoOvertimeCheck(true, OT_ENTERGAME, 30000);
	return bRet;
}

void GameSession::OnPrtcErrorInfo(GNET::Protocol* pProtocol)
{
	using namespace GNET;
	ErrorInfo* p = (ErrorInfo*)pProtocol;

	OutputLinkSevError(p->errcode);

	m_bConnected = false;
	m_bExitConnect = true;
	m_bWaitRelogin = false;
	m_pNetMan->Disconnect();
}

//	Output link server error
void GameSession::OutputLinkSevError(int iRetCode)
{
	GetServerError(iRetCode, m_strReason);
}

//	Get the server error string
bool GameSession::GetServerError(int iRetCode, AString& errMsg)
{
	bool bRet(false);
	CECStringTab& ErrorMsgs = SessionManager::GetSingleton().GetErrorMsgs();

	while (true)
	{
		if (iRetCode < 0)
			break;

		const wchar_t* pErr = ErrorMsgs.GetWideString(iRetCode + BASEIDX_LINKSEVERROR);
		if (!pErr)
			break;

		errMsg = WC2AS(pErr);
		bRet = true;
		break;
	}

	if(!bRet)
		errMsg.Format("Unknown error (%d)", iRetCode);

	return bRet;
}

// 玩家登出
void GameSession::OnPrtcPlayerLogout(GNET::Protocol* pProtocol)
{
	using namespace GNET;
	PlayerLogout* p = (PlayerLogout*)pProtocol;
	
	// 只有大退一种情况
	Close();
}

// 当前角色在跨服中
void GameSession::OnPrtcPlayerChangeDSRe(GNET::Protocol* pProtocol)
{
	using namespace GNET;

	PlayerChangeDS_Re* p = (PlayerChangeDS_Re*)pProtocol;
	if( p->retcode != 0 )
	{
		ASSERT(0);
		return;
	}
	
	if( !IsTestCrossServer() )
	{
		m_iGameResult = TEST_ERROR;
		CheckAddVersionError();
		m_strReason = ERROR_PLAYERCHANGEDS;
		Close();
	}
	else
	{
		// 未进入游戏时，直接跨到中央服
		if( p->flag == 3 )
			m_qCrossServer.SetOP(QueryCrossServer::ENTERWORLD);

		GNET::Marshal::OctetsStream o;
		o << int(1) << p->dst_zoneid;
		m_qCrossServer.CopyProto(p);
		m_qCrossServer.SetResult(o);
	}
}

void GameSession::OnPrtcChangeDSRe(GNET::Protocol* pProtocol)
{
	using namespace GNET;

	ChangeDS_Re* p = (ChangeDS_Re*)pProtocol;
	if( IsTestCrossServer() && p->retcode != 0 )
	{
		GNET::Marshal::OctetsStream o;
		o << int(0) << p->retcode;
		m_qCrossServer.SetResult(o);
	}
}

void GameSession::OnPrtcGetServerConfigRe(GNET::Protocol* pProtocol)
{
	GetCNetServerConfig_Re *p = (GetCNetServerConfig_Re *)pProtocol;
	for (size_t i(0); i < p->result.size(); ++ i)
	{
		const IntOctets &tmp = p->result[i];
		if (tmp.m_int == SCK_COUNTRYBATTLE_BONUS){
			if (m_qCountryWarBonus.SetResult(tmp.m_octets) == TimedQuery::QR_SUCCESS){
				m_iGameResult &= ~TEST_COUNTRYWAR_BONUS_ERR;
			}
		}
	}
}

GameSession::TimedQuery::QueryResult GameSession::QueryCountryWarBonus::SetResult(const GNET::Octets &o)
{
	nResult = QR_ERROR;
	if (o.size()){
		try{
			int totalBonus(0);
			Marshal::OctetsStream(o) >> totalBonus;
			if (totalBonus == -1){
				strResult = "国战未开放";
			}else{
				strResult.Format("%d", totalBonus);
				nResult = QR_SUCCESS;
			}
		}catch (...){
			strResult = "协议解析错误";
		}
	}else{
		strResult = "协议数据为空";
	}
	return GetResult();
}

GameSession::QueryCrossServer::~QueryCrossServer()
{
	if( m_pProto )
	{
		delete m_pProto;
		m_pProto = NULL;
	}
}

void GameSession::QueryCrossServer::CopyProto(GNET::PlayerChangeDS_Re* pSrc)
{
	if( m_pProto )
	{
		delete m_pProto;
		m_pProto = NULL;
	}

	m_pProto = (GNET::PlayerChangeDS_Re*)pSrc->Clone();
}

GameSession::TimedQuery::QueryResult GameSession::QueryCrossServer::SetResult(const GNET::Octets &o)
{
	nResult = QR_ERROR;

	if(o.size())
	{
		try
		{
			int nSucc = 0;
			int nParam = 0;
			GNET::Marshal::OctetsStream(o) >> nSucc >> nParam;

			AString strPort;
			strPort.Format(" 端口号: %d", m_iDestPort);
			if( nSucc == 2 )
			{
				strResult = "跨服成功";
				nResult = QR_SUCCESS;
				strResult += strPort;	 
			}
			else if( nSucc == 1 )
			{
				m_iZone = nParam;	// 记录要跨到的服务器ZoneID

				// 中断与当前服务器的连接，开始跨服
				m_pSession->CrossLogin();
			}
			else	// nSucc == 0
			{
				AString strError;
				if( nParam == -1 )
					strError = ERROR_INVALIDDSTZONE;	// 找不到目标服务器地址
				else if( nParam == -2 )
					strError = ERROR_CROSSSERVERCOOL;	// 跨服命令冷却中
				else
					m_pSession->GetServerError(nParam, strError);

				if( m_iOP == GO || m_iOP == ENTERWORLD )
					strResult.Format("跨服到中央服失败(%s)", strError);
				else if( m_iOP == RETURN )
					strResult.Format("回原服失败(%s)", strError);

				strResult += strPort;
			}
		}
		catch (...)
		{
			strResult = "解析数据错误";
		}
	}
	else
	{
		ASSERT(0);
		strResult = "协议数据为空";
	}

	return GetResult();
}

void GameSession::QueryCrossServer::OnLoginSuccess()
{
	if (m_pProto && m_pSession && m_pSession->IsWaitingRelogin()) {
		m_bOnSpecialServer = (m_pProto->flag == 1 || m_pProto->flag == 3);
	}
}

///////////////////////////////////////////////////////////////////////////
//  
//  C2S Command functions
//  
///////////////////////////////////////////////////////////////////////////

void GameSession::c2s_CmdGetAllData(bool bpd, bool bed, bool btd)
{
	using namespace C2S;
	
	int iSize = sizeof (cmd_header) + sizeof (cmd_get_all_data);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return;
	
	((cmd_header*)pBuf)->cmd = C2S::GET_ALL_DATA;
	
	cmd_get_all_data* pCmd = (cmd_get_all_data*)(pBuf + sizeof (cmd_header));
	pCmd->byPack	= (BYTE)bpd;
	pCmd->byEquip	= (BYTE)bed;
	pCmd->byTask	= (BYTE)btd;
	
	SendGameData(pBuf, iSize);
	
	delete [] pBuf;
}

bool GameSession::c2s_CmdGMChangeDS(bool bToCrossServer)
{
	if( GetCoolTime(GP_CT_CROSS_SERVER_APPLY) > 0 )
	{
		GNET::Marshal::OctetsStream o;
		o << int(0) << int(-2);
		m_qCrossServer.SetResult(o);
		return false;
	}

	using namespace C2S;
	
	int iSize = sizeof (cmd_header) + sizeof (gmcmd_change_ds);
	BYTE* pBuf = new BYTE [iSize];
	if (!pBuf)
		return false;
	
	((cmd_header*)pBuf)->cmd = C2S::GM_CHANGE_DS;
	
	gmcmd_change_ds* pCmd = (gmcmd_change_ds*)(pBuf + sizeof (cmd_header));
	pCmd->flag = bToCrossServer ? 1 : 2;
	
	SendGameData(pBuf, iSize);
	
	delete [] pBuf;
	return true;
}

///////////////////////////////////////////////////////////////////////////
//  
//  Protocols
//  
///////////////////////////////////////////////////////////////////////////

void GameSession::getServerConfig(const abase::vector<int>& keys)
{
	if (keys.empty()){
		return;
	}
	GetCNetServerConfig p;
	p.roleid = GetCharacterID();
	p.keys.GetVector().assign(keys.begin(), keys.end());
	SendNetData(p);
}

void GameSession::getServerConfig(int key)
{
	getServerConfig(abase::vector<int>(1, key));
}

// 获取当前角色的名字
const wchar_t* GameSession::GetRoleName()
{
	ASSERT(m_iCurRoleIdx < m_RoleList.size());
	return m_RoleList[m_iCurRoleIdx].roleName;
}

// 获取当前角色的世界ID
int GameSession::GetRoleWorldID()
{
	ASSERT(m_iCurRoleIdx < m_RoleList.size());
	return m_RoleList[m_iCurRoleIdx].worldid;
}

// 获取当前角色的职业
int GameSession::GetProfession()
{
	ASSERT(m_iCurRoleIdx < m_RoleList.size());
	return m_RoleList[m_iCurRoleIdx].profession;
}

// 获取错误信息
std::string GameSession::GetErrorMessage(int iErrCode)
{
	CECStringTab& ErrorMsgs = SessionManager::GetSingleton().GetErrorMsgs();
	const wchar_t* pStr = ErrorMsgs.GetWideString(iErrCode);
	return pStr ? WStringToString(pStr) : "";
}

// 是否测试跨服
bool GameSession::IsTestCrossServer() const
{
	return SessionManager::GetSingleton().IsTestCrossServer();
}

// 跨服登录
bool GameSession::CrossLogin()
{
	if( !IsTestCrossServer() )
	{
		ASSERT(0);
		return false;
	}

	if( m_qCrossServer.GetOP() == QueryCrossServer::GO )
	{
		// 查找中央服信息
		CECCrossServerList::ServerInfo* pInfo = 
			CECCrossServerList::GetSingleton().Find(m_qCrossServer.GetDestZone(), m_iServerLine);
		if (!pInfo)
		{
			GNET::Marshal::OctetsStream o;
			o << int(0) << int(-1);
			m_qCrossServer.SetResult(o);
			Close();
			return false;
		}
	}

	// 断开当前连接但不输出结果，等待再次的连接
	Close(true);

	TimedQueries::iterator it;
	for(it=m_timedQueries.begin(); it!=m_timedQueries.end(); ++it)
		(*it)->Reset();

	m_iGameResult &= ~TEST_SUCCESSFUL;
	return true;
}

bool GameSession::Relogin()
{
	if( m_qCrossServer.GetOP() == QueryCrossServer::GO ||
		m_qCrossServer.GetOP() == QueryCrossServer::ENTERWORLD )
	{
		// 查找中央服信息
		CECCrossServerList::ServerInfo* pInfo = 
			CECCrossServerList::GetSingleton().Find(m_qCrossServer.GetDestZone(), m_iServerLine);
		if (!pInfo)
		{
			ASSERT(0);
			GNET::Marshal::OctetsStream o;
			o << int(0) << int(-1);
			m_qCrossServer.SetResult(o);
			return false;
		}

		// 设置已测试标志
		pInfo->SetTested(true);
		// 保存跨服端口号
		m_qCrossServer.SetDestPort(pInfo->port);
		CECCrossServerList::GetSingleton().AddOneMatchInfo(pInfo->zoneid, m_strAddress, m_iServerLine);
		// 保存原服信息
		CECCrossServerList::ServerInfo orgServer;
		orgServer.address = m_strAddress;
		orgServer.line = m_iServerLine;
		orgServer.port = m_iPort;
		orgServer.zoneid = m_iZoneID;
		SaveOrgServer(orgServer);

		SetAddress(pInfo->address);
		SetServerPort(pInfo->port);
		SetZoneID(pInfo->zoneid);
		SetServerLine(pInfo->line);
	}
	else if( m_qCrossServer.GetOP() == QueryCrossServer::RETURN )
	{
		// 取原服信息
		CECCrossServerList::ServerInfo orgServer = GetOrgServer();

		// 再将中央服信息暂存
		CECCrossServerList::ServerInfo cenServer;
		cenServer.address = m_strAddress;
		cenServer.line = m_iServerLine;
		cenServer.port = m_iPort;
		cenServer.zoneid = m_iZoneID;
		SaveOrgServer(cenServer);

		SetAddress(orgServer.address);
		SetServerPort(orgServer.port);
		SetZoneID(orgServer.zoneid);
		SetServerLine(orgServer.line);
	}

	return Open();
}

//  Get cool time
int GameSession::GetCoolTime(int iIndex, int* piMax/* NULL */)
{
	if (iIndex >= 0 && iIndex < GP_CT_MAX)
	{
		if (piMax)
			*piMax = m_aCoolTimes[iIndex].iMaxTime; 
		
		return m_aCoolTimes[iIndex].iCurTime;
	}
	
	return 0;
}

//  Defered sending ChangeDS
void GameSession::gm_ChangeDS(bool bToCrossServer)
{
	DeferedCmds::iterator it = m_DeferedCmds.find(DeferedCmd::TYPE_CHANGEDS);
	if( it != m_DeferedCmds.end() )
	{
		DeferedCmd* pCmd = it->second;

		GNET::Marshal::OctetsStream os;
		os << (bToCrossServer ? 1 : 0);
		pCmd->Send(os);
	}
}