/*
 * FILE: GameSession.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2012/8/28
 *
 * HISTORY: 
 *
 * Copyright (c) 2011 ACE Studio, All Rights Reserved.
 */


#pragma once

#include <ABaseDef.h>
#include <AArray.h>
#include <AAssist.h>
#include "gnetdef.h"
#include "gnoctets.h"
#include "EC_StringTab.h"
#include "EC_GPDataType.h"
#include "EC_CrossServerList.h"
#include "cslock.h"
#include <string>


///////////////////////////////////////////////////////////////////////////
//  
//  Define and Macro
//  
///////////////////////////////////////////////////////////////////////////

#define MAX_IDLE_TIME 40000		// 如果Session空等待40秒以上，认为服务器超时


///////////////////////////////////////////////////////////////////////////
//  
//  Types and Global variables
//  
///////////////////////////////////////////////////////////////////////////

namespace GNET
{
	class RoleInfo;
	class Protocol;
	class GameClient;
	class Octets;

	class PlayerChangeDS_Re;
}

class GameObject;

// 单字节字符串转成双字节字符串
std::wstring StringToWString( const std::string& str);
// 双字节字符串转成单字节字符串
std::string WStringToString( const std::wstring& wstr);


///////////////////////////////////////////////////////////////////////////
//  
//  class GameSession
//  
///////////////////////////////////////////////////////////////////////////

class GameSession
{
public:
	//	Overtime check ID
	enum
	{
		OT_CHALLENGE = 0,
		OT_ENTERGAME,
	};
	
	//	Overtime check
	struct OVERTIME
	{
		int		iCheckID;
		bool	bCheck;
		DWORD	dwTime;
		DWORD	dwTimeCnt;
	};

	struct ROLEINFO
	{
		int roleid;
		AWString roleName;
		int profession;
		int worldid;
		int level;
		int level2;
	};
	typedef abase::vector<ROLEINFO> ROLELIST;
	
	//	Cool time
	struct COOLTIME
	{
		int		iCurTime;
		int		iMaxTime;
		
		void Update(int iRealTime)
		{
			if (iCurTime > 0)
			{
				iCurTime -= iRealTime;
				a_ClampFloor(iCurTime, 0);
			}
		}
	};

	// 延迟发送的命令
	class DeferedCmd
	{
	public:
		enum
		{
			TYPE_CHANGEDS,
		};

	public:
		DeferedCmd(int iType, GameSession* pSession)
			: m_iType(iType), m_pSession(pSession), m_bToSend(false)
		{
		}

		bool IsToSend() const { return m_bToSend; }

		virtual void Send(const GNET::Octets& o) { m_bToSend = true; }
		virtual void Update(DWORD dwDeltaTime) = 0;

		int GetType() const { return m_iType; }

	protected:
		int m_iType;				// Type
		GameSession* m_pSession;	// Session
		bool m_bToSend;				// Will be sended
	};

	typedef abase::hash_map<int, DeferedCmd*> DeferedCmds;

///////////////////////////////////////////////////////////////////////////

	class  TimedQuery
	{
	public:
		enum QueryResult{
			QR_NOT_START,		//	未开始查询
				QR_WAIT,			//	查询中
				QR_TIMEOUT,			//	查询超时
				QR_ERROR,			//	内容有误
				QR_SUCCESS,			//	正确获取
		};
	protected:
		AString		strName;		//	查询的含义
		QueryResult	nResult;		//	查询结果
		AString		strResult;		//	结果字符串
		DWORD		dwTime;			//	查询倒计时
		DWORD		dwTimeLength;
		
	public:
		TimedQuery(const char *szName) : strName(szName), nResult(QR_NOT_START), dwTime(0), dwTimeLength(0){}
		virtual ~TimedQuery(){} 
		bool BeginQuery(DWORD timeLength){
			if (nResult == QR_NOT_START){
				nResult = QR_WAIT;
				dwTime = 0;
				dwTimeLength = timeLength;
				return true;
			}
			return false;
		}
		bool Tick(DWORD t){
			//	超时返回 true
			if (nResult == QR_WAIT && (dwTime += t) >= dwTimeLength){
				nResult = QR_TIMEOUT;
				return true;
			}
			return false;
		}
		bool Finished()const{
			return nResult > QR_WAIT;
		}
		QueryResult GetResult()const{ return nResult; }
		bool GetResultAsString(AString &rhs)const{
			const char *szResult = NULL;
			switch (nResult){
			case QR_NOT_START:	szResult = "尚未查询";	break;
			case QR_WAIT:		szResult = "查询中";	break;
			case QR_TIMEOUT:	szResult = "查询超时";	break;
			default: szResult = strResult;
			}
			rhs.Format("%s：%s", strName, szResult);
			return nResult == QR_SUCCESS;
		}
		void Reset() { nResult = QR_NOT_START; dwTime = 0; dwTimeLength = 0; }
		void ForceFinish(bool bSucc = true) { nResult = bSucc ? QR_SUCCESS : QR_ERROR; }
		void IncTimeLength(DWORD t) { dwTimeLength += t; }
		virtual QueryResult SetResult(const GNET::Octets &o) = 0;
	};
	typedef abase::vector<TimedQuery *> TimedQueries;
	
	class QueryCountryWarBonus : public TimedQuery
	{
	public:
		QueryCountryWarBonus():TimedQuery("国战总奖励"){}
		virtual QueryResult SetResult(const GNET::Octets &o);
	};

	class QueryCrossServer : public TimedQuery
	{
	public:
		enum { GO = 1, RETURN, ENTERWORLD };

	public:
		QueryCrossServer()
			: TimedQuery("跨服测试"), m_pSession(NULL), m_pProto(NULL), m_iOP(0), m_iDestPort(-1), m_bOnSpecialServer(false) {}
		~QueryCrossServer();

		void SetSession(GameSession* pSession) { m_pSession = pSession; }
		void SetOP(int iOP) { m_iOP = iOP; }
		int GetOP() const { return m_iOP; }
		void SetDestZone(int iZone) { m_iZone = iZone; }
		int GetDestZone() const { return m_iZone; }
		void SetDestPort(int iPort) { m_iDestPort = iPort; }
		int GetDestPort() { return m_iDestPort; }
		void CopyProto(GNET::PlayerChangeDS_Re* pSrc);
		GNET::PlayerChangeDS_Re* GetProto() { return m_pProto; }

		void SetOSecurity(const GNET::Octets &o) { m_OSecurity = o; }
		const GNET::Octets & GetOSecurity()const{ return m_OSecurity; }
		void SetISecurity(const GNET::Octets &o) { m_ISecurity = o; }
		const GNET::Octets & GetISecurity()const{ return m_ISecurity; }

		virtual QueryResult SetResult(const GNET::Octets &o);

		void OnLoginSuccess();
		bool IsOnSpecialServer() { return m_bOnSpecialServer; }

	protected:
		int m_iOP;
		int m_iZone;
		int m_iDestPort;				//	跨服端口号
		bool m_bOnSpecialServer;
		GNET::PlayerChangeDS_Re* m_pProto;
		GNET::Octets m_OSecurity;	//	发出协议加密参数
		GNET::Octets m_ISecurity;	//	收取协议加密参数
		GameSession* m_pSession;
	};

	static int IOCallBack(void* pSession, void* pData, unsigned int idLink, int iEvent);
	static DWORD WINAPI ConnectThread(LPVOID pArg);

public:
	GameSession();
	virtual ~GameSession();

	//	Open session
	bool Open();
	//	Close sesssion
	void Close(bool bRelogin = false);
	//	Break Link myself
	void BreakLink();
	
	//	Update
	bool Update(DWORD dwDeltaTime);
	//	Net pulse
	void NetPulse();

	bool SendGameData( void* pData,int iSize );
	bool SendNetData( const GNET::Protocol& p,bool bUrg=false );

	//	Set link ID
	void SetLinkID(DWORD idLink) { m_idLink = idLink; }
	//	Get link ID
	DWORD GetLinkID() { return m_idLink; }

	//	Add a protocol to m_aNewProtocols and prepare to be processed
	int AddNewProtocol(GNET::Protocol* pProtocol);
	//	Process protocols in m_aNewProtocols
	bool ProcessNewProtocols();
	//	Clear all processed protocols in m_aOldProtocols
	void ClearOldProtocols();

	//	Set user name
	void SetUserName(const char* szName) { m_strUserName = szName; }
	//	Get user name
	const char* GetUserName() { return m_strUserName; }
	//	Set user password
	void SetUserPassword(const char* szPass) { m_strPassword = szPass; }
	//	Get user password
	const char* GetUserPassword() { return m_strPassword; }
	//	Get kick user flag
	bool GetKickUserFlag() { return m_bKickUser; }
	//	Set kick user flag
	void SetKickUserFlag(bool bKick) { m_bKickUser = bKick; }
	//  Set the server address
	void SetAddress(const char* szAddress) { m_strAddress = szAddress; }
	//  Set the original address
	void SetOrgAddress(const char* szAddress) { m_strOrgAddress = szAddress; m_strAddress = szAddress; }
	//  Set the port
	void SetServerPort(int iPort) { m_iPort = iPort; }
	//  Get the server address
	const char* GetAddress() { return m_strAddress; }
	//  Get the original address
	const char* GetOrgAddress() { return m_strOrgAddress; }
	//  Get the server port
	int GetServerPort() { return m_iPort; }
	//  Get the server zone id
	int GetZoneID() const { return m_iZoneID; }
	//  Set the server zone id
	void SetZoneID(int iZoneID) { m_iZoneID = iZoneID; }
	//  Get the original zone id, because we maybe login the cross server
	int GetOrgZoneID() const { return m_iOrgZoneID; }
	//  Set the original zone id
	void SetOrgZoneID(int iZoneID) { m_iOrgZoneID = iZoneID; m_iZoneID = iZoneID; }
	//  Get the server line
	int GetServerLine() const { return m_iServerLine; }
	//  Set the server line
	void SetServerLine(int iLine) { m_iServerLine = iLine; }
	//  Cross server ?
	bool IsTestCrossServer() const;

	//	Set user ID
	void SetUserID(int iUserID) { m_iUserID = iUserID; }
	//	Get user ID
	int GetUserID() { return m_iUserID; }
	//	Set user character ID
	void SetCharacterID(int iCharID) { m_iCharID = iCharID; }
	//	Get user character ID
	int GetCharacterID() { return m_iCharID; }
	//  Get the role's name
	const wchar_t* GetRoleName();
	//  Get the role's world id
	int GetRoleWorldID();
	//  Get the role's profession type
	int GetProfession();
	//  Get the link break reason
	const char* GetBreakReason() { return m_strReason; }
	//  Is the game ready
	unsigned int GetGameResult() { return m_iGameResult; }
	//  Get the version error
	const char* GetVersionError() { return m_strVersionErr; }
	void CheckAddVersionError();
	bool GetQueryResults(AString &strResults);
	bool TestFinished();

	//	Get connected flag
	bool IsConnected() { return m_bConnected; }

	//  Are we waiting relogin ?
	bool IsWaitingRelogin() const { return m_bWaitRelogin; }
	//  Start to Login the cross server
	bool CrossLogin();
	//  Relogin the server
	bool Relogin();

	//	Start / End overtime
	void DoOvertimeCheck(bool bStart, int iCheckID, DWORD dwTime);
	//	On overtime happens
	void OnOvertimeHappen();
	//	Get overtime counter
	bool GetOvertimeCnt(DWORD& dwTotalTime, DWORD& dwCnt);

	//  Get cool time
	int GetCoolTime(int iIndex, int* piMax = NULL);

	//  GM Change the server
	void gm_ChangeDS(bool bToCrossServer);
	//	save the cross-starting server
	void SaveOrgServer(const CECCrossServerList::ServerInfo& osi) { m_OrgServer = osi; }
	const CECCrossServerList::ServerInfo& GetOrgServer() const { return m_OrgServer; }
	//	get dest server port
	int GetDestPort() { return m_qCrossServer.GetDestPort(); }
	bool IsOnSpecialServer() { return m_qCrossServer.IsOnSpecialServer(); }

	///////////////////////////////////////////////////////////////////////////
	//  General C2S Command

	void c2s_CmdGetAllData(bool bpd, bool bed, bool btd);
	bool c2s_CmdGMChangeDS(bool bToCrossServer);

	///////////////////////////////////////////////////////////////////////////
	//	Protocols

	enum ServerConfigKeyType
	{
		SCK_COUNTRYBATTLE_BONUS = 0,		//	国战总奖励（可能为跨服或单服）
	};
	void getServerConfig(int key);
	void getServerConfig(const abase::vector<int>& keys);

	//	Get net manager
	GNET::GameClient* GetNetManager() { return m_pNetMan; }

	//  Get the error message for S2C
	std::string GetErrorMessage(int iErrCode);
	//  Get the error message for protocol
	bool GetServerError(int iRetCode, AString& errMsg);

protected:

	APtrArray<GNET::Protocol*>	m_aNewProtocols;	//	New protocol array
	APtrArray<GNET::Protocol*>	m_aOldProtocols;	//	Old protocol array
	APtrArray<GNET::Protocol*>	m_aTempProtocols;	//	Temporary protocol array

	GNET::GameClient*	m_pNetMan;					//	Net manager
	DWORD				m_idLink;					//	Link ID
	HANDLE				m_hConnectThread;			//	Handle of connect thread
	volatile bool		m_bConnected;				//	Connected flag
	volatile bool		m_bSessionClosed;
	CsMutex         	m_csSession;				//  Mutex object

	bool				m_bLinkBroken;				//	Connection was broken
	OVERTIME			m_ot;						//	Overtime info
	volatile bool		m_bExitConnect;

	int					m_iZoneID;					//  Zone ID of the server
	int					m_iOrgZoneID;				//  Original zone id
	int					m_iServerLine;				//  Server line
	AString				m_strAddress;				//  Address of the server
	AString				m_strOrgAddress;			//  Original address
	int					m_iPort;					//  Port of the server
	AString				m_strUserName;				//	User name
	AString				m_strPassword;				//	User password
	int					m_iUserID;					//	User ID
	int					m_iCharID;					//	User character ID, this ID should be same as host player's cid
	bool				m_bKickUser;				//  Kick the online user before login ?
	DWORD				m_dwIdleTime;				//  Session空等待时间，一直没有服务器响应

	COOLTIME			m_aCoolTimes[GP_CT_MAX];	//	Cool times

	ROLELIST			m_RoleList;					//  角色列表
	size_t				m_iCurRoleIdx;				//  当前角色的索引
	AString				m_strReason;				//  断开连接的原因
	unsigned int		m_iGameResult;				//  连接结果
	int					m_iCreateRoleErr;			//  计算创建角色失败的次数
	AString				m_strVersionErr;			//  版本验证错误
	bool				m_bWaitRelogin;				//  等待跨服

	DeferedCmds			m_DeferedCmds;				//  延迟发送的命令

	TimedQueries			m_timedQueries;
	QueryCountryWarBonus	m_qCountryWarBonus;		//	国战总奖励获取
	QueryCrossServer		m_qCrossServer;			//  跨服测试
	CECCrossServerList::ServerInfo m_OrgServer;		//  从原服到中央服时保存原服信息；从中央服回原服时保存中央服信息

protected:
	void CreateTheRole(const char* rolename);

	bool Connect();
	void CloseConnectThread();
	//	Send net data
	bool SendNetData(const GNET::Protocol* p) { return p ? SendNetData(*p) : false; }
	//	Proecess game data
	void ProcessGameData(const GNET::Octets& Data);
	//	Calculate S2C command data size
	DWORD CalcS2CCmdDataSize(int iCmd, BYTE* pDataBuf, DWORD dwDataSize);
	//	When connection was broken, this function is called
	void LinkBroken(bool bDisconnect);
	//	Filter protocols
	bool FilterProtocols(GNET::Protocol* pProtocol);

	bool StartGame();

	void OutputLinkSevError(int iRetCode);

	///////////////////////////////////////////////////////////////////////////
	//  Notify the protocol

	void OnPrtcChallenge(GNET::Protocol* pProtocol);
	void OnPrtcKeyExchange(GNET::Protocol* pProtocol);
	void OnPrtcOnlineAnnounce(GNET::Protocol* pProtocol);
	void OnPrtcRoleListRe(GNET::Protocol* pProtocol);
	void OnPrtcSelectRoleRe(GNET::Protocol* pProtocol);
	void OnPrtcCreateRoleRe(GNET::Protocol* pProtocol);
	void OnPrtcDeleteRoleRe(GNET::Protocol* pProtocol);
	void OnPtrcUndoDeleteRoleRe(GNET::Protocol* pProtocol);
	void OnPrtcErrorInfo(GNET::Protocol* pProtocol);
	void OnPrtcPlayerLogout(GNET::Protocol* pProtocol);
	void OnPrtcMatrixChallenge(GNET::Protocol* pProtocol);
	void OnPrtcResetPosition(GNET::Protocol* pProtocol);
	void OnPrtcPlayerChangeDSRe(GNET::Protocol* pProtocol);
	void OnPrtcChangeDSRe(GNET::Protocol* pProtocol);
	void OnPrtcGetServerConfigRe(GNET::Protocol* pProtocol);
};

///////////////////////////////////////////////////////////////////////////
//  
//  Inline functions
//  
///////////////////////////////////////////////////////////////////////////
