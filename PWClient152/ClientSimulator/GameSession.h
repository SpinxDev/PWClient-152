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
#include "EC_StringTab.h"
#include "cslock.h"
#include <string>


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

namespace GNET
{
	class RoleInfo;
	class Protocol;
	class GameClient;
	class Octets;
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
		int gender;
		int worldid;
	};

	typedef abase::vector<ROLEINFO> ROLELIST;

	static int IOCallBack(void* pSession, void* pData, unsigned int idLink, int iEvent);
	static DWORD WINAPI ConnectThread(LPVOID pArg);

public:
	GameSession();
	virtual ~GameSession();

	//	Open session
	bool Open();
	//	Close sesssion
	void Close();
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
	//  Get the role's gender
	int GetGender();

	//	Get connected flag
	bool IsConnected() { return m_bConnected; }

	//	Start / End overtime
	void DoOvertimeCheck(bool bStart, int iCheckID, DWORD dwTime);
	//	On overtime happens
	void OnOvertimeHappen();
	//	Get overtime counter
	bool GetOvertimeCnt(DWORD& dwTotalTime, DWORD& dwCnt);

	///////////////////////////////////////////////////////////////////////////
	//  General C2S Command

	void c2s_CmdGetAllData(bool bpd, bool bed, bool btd);

	//	Get net manager
	GNET::GameClient* GetNetManager() { return m_pNetMan; }
	//  Get game object
	GameObject* GetGameObject() { return m_pGame; }
	//  Get the error message
	std::string GetErrorMessage(int iErrCode);

protected:

	APtrArray<GNET::Protocol*>	m_aNewProtocols;	//	New protocol array
	APtrArray<GNET::Protocol*>	m_aOldProtocols;	//	Old protocol array
	APtrArray<GNET::Protocol*>	m_aTempProtocols;	//	Temporary protocol array

	GameObject*			m_pGame;					//  Game Object
	GNET::GameClient*	m_pNetMan;					//	Net manager
	DWORD				m_idLink;					//	Link ID
	HANDLE				m_hConnectThread;			//	Handle of connect thread
	volatile bool		m_bConnected;				//	Connected flag
	CsMutex         	m_csSession;				//  Mutex object

	bool				m_bLinkBroken;				//	Connection was broken
	OVERTIME			m_ot;						//	Overtime info
	volatile bool		m_bExitConnect;

	AString				m_strUserName;				//	User name
	AString				m_strPassword;				//	User password
	int					m_iUserID;					//	User ID
	int					m_iCharID;					//	User character ID, this ID should be same as host player's cid
	bool				m_bKickUser;				//  Kick the online user before login ?

	ROLELIST			m_RoleList;					//  角色列表
	size_t				m_iCurRoleIdx;				//  当前角色的索引

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
	bool GetServerError(int iRetCode, AString& errMsg);
	void OutputLinkSevError(int iRetCode);

	///////////////////////////////////////////////////////////////////////////
	//  Notify the protocol

	void OnPrtcChallenge(GNET::Protocol* pProtocol);
	void OnPrtcKeyExchange(GNET::Protocol* pProtocol);
	void OnPrtcOnlineAnnounce(GNET::Protocol* pProtocol);
	void OnPrtcMatrixChallenge(GNET::Protocol* pProtocol);
	void OnPrtcRoleListRe(GNET::Protocol* pProtocol);
	void OnPrtcSelectRoleRe(GNET::Protocol* pProtocol);
	void OnPrtcCreateRoleRe(GNET::Protocol* pProtocol);
	void OnPrtcDeleteRoleRe(GNET::Protocol* pProtocol);
	void OnPtrcUndoDeleteRoleRe(GNET::Protocol* pProtocol);
	void OnPrtcErrorInfo(GNET::Protocol* pProtocol);
	void OnPrtcPlayerLogout(GNET::Protocol* pProtocol);
	void OnPrtcPrivateChat(GNET::Protocol* pProtocol);
	void OnPrtcChatMessage(GNET::Protocol* pProtocol);
};

///////////////////////////////////////////////////////////////////////////
//  
//  Inline functions
//  
///////////////////////////////////////////////////////////////////////////
