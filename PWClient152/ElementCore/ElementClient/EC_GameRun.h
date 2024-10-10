/*
 * FILE: EC_GameRun.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/8/27
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include <Windows.h>
#include <hashtab.h>
#include <AAssist.h>
#include "EC_GPDataType.h"
#include "EC_PendingAction.h"
#include <hashmap.h>
#include "gnmarshal.h"
#include <A3DTypes.h>
#include "EC_MsgDef.h"
#include <AArray.h>
///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////

class A3DVECTOR3;
class CECUIManager;
class CECWorld;
class CECInputCtrl;
class A3DGFXExMan;
class CECLoadProgress;
class CECFullGlowRender;
class CECHostPlayer;
class CECViewport;
class CECInstance;
class CECTeamMan;
class CECShortcutSet;
class CECSCCommand;
class CECMessageMan;
class CECClipboard;
class CECHostInputFilter;
class CECIvtrItem;
class AMSoundBuffer;
class A2DSprite;
class CECRandomMapProcess;
struct ECMSG;
class CECMemSimplify;

namespace GNET
{
	class RoleInfo;
	class Protocol;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CECGameRun
//	
///////////////////////////////////////////////////////////////////////////

class CECGameRun
{
public:		//	Types

	//	Game state
	enum
	{
		GS_NONE = 0,	//	None
		GS_LOGIN,		//	Login in state
		GS_GAME,		//	In game
	};

	//	Double experence mode
	enum
	{
		DBEXP_NONE		= 0x00,
		DBEXP_OWN		= 0x01,
		DBEXP_GLOBAL	= 0x02,
	};

	typedef abase::hashtab<ACString*, int, abase::_hash_function> ID2NameTable;
	typedef abase::hashtab<int, ACString, abase::_hash_function> Name2IDTable;
	typedef abase::hashtab<CECInstance*, int, abase::_hash_function> InstTable;
	typedef abase::hashtab<long, long, abase::_hash_function> CommonDataTable;
	typedef abase::hash_map<int, AString>	SoundStringTable;
	typedef abase::hash_map<AString, AMSoundBuffer*>	SoundTable;

	struct unique_data
	{
		int type;
		GNET::Octets _value;

	//	int GetValueAsInt() { int v; GNET::Marshal::OctetsStream(_value)>>v; return v;}
		int GetValueAsInt() {  return *(int*)_value.begin();}
		double GetValueAsDouble() { return *(double*)_value.begin();}
		void* GetRawData() { return _value.begin();}
		int GetRawDataLen() { return (int)_value.size();}
	};
	typedef abase::hash_map<int,unique_data*> UniqueDataMap;

	struct HistoryProgConfig
	{
		int mapid;
		int x;
		int y;
	};

public:		//	Constructor and Destructor

	CECGameRun();
	virtual ~CECGameRun();

public:		//	Attributes

public:		//	Operations

	//	Initialize object
	bool Init();
	//	Release object
	void Release();

	bool IsFirstShow()const;
	void OnFirstShowEnd();

	//	Start game
	bool StartGame(int idInst, const A3DVECTOR3& vHostPos);
	//	Start login interface
	bool StartLogin();
	//	Deal windows message
	bool DealWindowsMessage(UINT message, WPARAM wParam, LPARAM lParam);
	//	For WM_SIZE message
	void OnWindowSizeChange(A3DRECT rcOld, A3DRECT rcNew);

	//	Game tick routine
	bool Tick(DWORD dwDeltaTime);
	void TickDiscardedFrame(DWORD dwDeltaTime);
	//	Game render routine
	bool Render(bool bPresent=true);
	//	Capture screen
	bool CaptureScreen(int N);

	//  Get game state.
	int GetGameState() { return m_iGameState; }
	// PVP mode
	bool GetPVPMode() { return m_bPVPMode; }
	void SetPVPMode(bool b) { m_bPVPMode = b; }
	//	Get UI manager
	CECUIManager* GetUIManager() { return m_pUIManager; }
	//	Get game world
	CECWorld* GetWorld() { return m_pWorld; }
	//	Get input control
	CECInputCtrl* GetInputCtrl() { return m_pInputCtrl; }
	//	Get host input filter
	CECHostInputFilter* GetHostInputFilter() { return m_pInputFilter; }
	//	Get auto home input filter
// 	CECAutoHomeInputFilter* GetAutoHomeFilter() { return m_pAutoHomeFilter; }
	//	Whether UI has cursor
	bool UIHasCursor() { return m_bUIHasCursor; }
	//	Save necessary user configs (UI, shortcut, accelerate keys) to server
	DWORD SaveConfigsToServer();
	//	Load necessary user configs (UI, shortcut, accelerate keys) from server
	bool LoadConfigsFromServer(const void* pDataBuf, int iDataSize);
	//	Set logout flag
	void SetLogoutFlag(int iFlag);
	int	GetLogoutFlag() const { return m_iLogoutFlag; }
	//	Get double experence mode
	int GetDoubleExpMode() { return m_iDblExpMode; }
	//	Get available double experence time
	int GetAvailableDblExpTime() { return m_iAvaDExpTime; }
	//	Get remain time of current double experence time
	int GetRemainDblExpTime();
	//	Lock game points
	void LockGamePoints(bool bLock) { m_bLockGamePt = bLock; }
	//	Get game points locking flag
	bool IsGamePointsLocked() { return m_bLockGamePt; }
	//	Wallow info
	const S2C::player_wallow_info& GetWallowInfo() { return m_WallowInfo; }
	void SetWallowInfo(const S2C::player_wallow_info& info) { m_WallowInfo = info; }
	//	Account Login Info
	struct AccountLoginInfo 
	{
		int	userid;
		int	login_time;
		int	login_ip;
		int current_ip;

		AccountLoginInfo(){ Reset(); }

		void Reset()
		{
			userid = 0;
			login_time = 0;
			login_ip = 0;
			current_ip = 0;
		}

		bool operator == (const AccountLoginInfo &rhs)const
		{
			return userid == rhs.userid
				&& login_time == rhs.login_time
				&& login_ip == rhs.login_ip
				&& current_ip == rhs.current_ip;
		}

		bool operator != (const AccountLoginInfo &rhs)const
		{
			return !operator == (rhs);
		}
	};
	void SetAccountLoginInfo(const AccountLoginInfo &info);
	void ResetAccountLoginInfo();
	void ShowAccountLoginInfo();
	void SetAccountInfoFlag(char accountinfo_flag);
	void ShowAccountInfo();

	//	Begin load progress
	bool BeginLoadProgress(float Min, float Max);
	//	End load progress
	void EndLoadProgress();
	//	Step load progress
	void StepLoadProgress(float Step);

	//	Set / Get selected character's role info.
	const GNET::RoleInfo& GetSelectedRoleInfo();
	void SetSelectedRoleInfo(const GNET::RoleInfo& Info);
	
	const GNET::RoleInfo& GetRedirectLoginPosRoleInfo();
	void SetRedirectLoginPosRoleInfo(const GNET::RoleInfo& Info);

	//	Add fixed format message to chat window
	void AddFixedMessage(int iMsg, ...);
	void AddFixedChannelMsg(int iMsg, char cChannel, ...);
	//	Add message to chat window
	void AddChatMessage(const ACHAR *pszMsg, char cChannel, int idPlayer=-1, const ACHAR* szName=NULL, BYTE byFlag = 0, char cEmotion = 0, CECIvtrItem *pItem = NULL,const ACHAR *pszMsgOrigion = NULL);

	//	Get host player
	CECHostPlayer* GetHostPlayer() { return m_pHostPlayer; }
	//	Get host player's position or a candidate position
	A3DVECTOR3 GetSafeHostPos(CECViewport* pViewport);
	//	Get player name by ID
	const ACHAR* GetPlayerName(int cid, bool bAutoGen);
	//	Get player ID by name
	int GetPlayerID(const ACHAR* szName);
	//	Add player name to table
	void AddPlayerName(int cid, const ACHAR* szName, bool bOverwrite=false);
	//	Get team manager
	CECTeamMan* GetTeamMan() { return m_pTeamMan; }

	//	Get instance by ID
	CECInstance* GetInstance(int id);
	//	Get instance by name
	CECInstance* GetInstance(const ACHAR* szName);
	//	Jump to specified position in a instance
	bool JumpToInstance(int idInst, const A3DVECTOR3& vPos, int iParallelWorldID = 0);

	const AString & GetSoundStringFromTable(int idString);

	//	Post game message
	bool PostMessage(DWORD dwMsg, int iManager, int iSubID, DWORD p1=0, DWORD p2=0, DWORD p3=0, DWORD p4=0, MsgDataBase *pData=NULL);
	//	Post game message
	bool PostMessage(const ECMSG& Msg);
	//	Process game message
	bool ProcessMessage(const ECMSG& Msg);
	//	Check defence
	void CheckDefenceCode();
	void CheckDefenceCodeImpl();

	//	Get shortcut sets
	CECShortcutSet* GetGenCmdShortcuts() { return m_pNormalSCS; }
	CECShortcutSet* GetTeamCmdShortcuts() { return m_pTeamSCS; }
	CECShortcutSet* GetTradeCmdShortcuts() { return m_pTradeSCS; }
	CECShortcutSet* GetPoseCmdShortcuts() { return m_pPoseSCS; }
	CECShortcutSet* GetFactionCmdShortcuts() { return m_pFactionSCS; }
	//	Get command shortcut (except pose command) by command ID
	CECSCCommand* GetCmdShortcut(int iCommand);
	//	Get pose command shortcut by pose
	CECSCCommand* GetPoseCmdShortcut(int iPose);
	//	Get game message manager
	CECMessageMan* GetMessageMan() { return m_pMessageMan; }
	//	Get game data clipboard
	CECClipboard* GetClipboard() { return m_pClipboard; }

	void ActivateDefInput();
// 	void ActivateAutoHomeInput();

	unsigned int GetGShopTimeStamp() const { return m_nGShopTimeStamp; }
	unsigned int GetGShopTimeStamp2() const { return m_nGShopTimeStamp2; }

	long GetCommonData(long key);
	unique_data* CECGameRun::GetUniqueData(int key);

	//  Get current server's name
	ACString GetServerName() { return m_strCurServer; }
	ACString GetServerGroup(){ return m_strCurGroup; }
	void SetServerName(ACString group, ACString name){
		m_strCurGroup = group;
		m_strCurServer = name;
	}

	//	Get in game network delay (in ms)
	int	GetInGameNetworkDelay(){ return GetGameState() == GS_GAME ? m_iInGameDelay : 0; }

	//	查询职业名称
	const wchar_t * GetProfName(int iProf)const;
	//	查询修真
	ACString GetLevel2Name(int iLevel2)const;
	ACString GetLevel2NameForCondition(int iMinLevel2)const;

	int GetSellingRoleID() const { return m_SellingRoleID;}
	void SetSellingRoleID(int roleid);

	CECPendingActionArray & GetPendingLogOut(){ return m_pendingLogout; }
	CECMemSimplify* GetMemSimplify() { return m_pMemSimplify; }

	void ReleaseSoundTable();
	AMSoundBuffer * GetOrLoadSoundFromTable(int idString);

	bool IsInStage(int mapid,int stage); // mapid 是否在这个阶段
	bool IsInCurStage(int mapid); // map 是否在当前阶段
	int GetStageID(int idx); // 获取历史阶段的tid
	bool GetStageInfo(int id,ACString& name,ACString& desc,float& progress);	//历史阶段的信息
	int GetStageVarKey(int id);//历史阶段对应的变量key
	bool GetCurStageInfo(ACString& name,ACString& desc,float& progress);//当前历史阶段的信息
	int GetCurStageIndex();
	HistoryProgConfig* GetCurStageProgConfig(int mapid);

	void OnUniqueDataBroadcast(GNET::Protocol* p);

protected:	//	Attributes

	bool				m_bFirstShow;		//	首次进入游戏（直到登录框显示）

	//	背景图显示
	A2DSprite*			m_pLogo;
	A2DSprite*			m_pClassification;

	CECUIManager*		m_pUIManager;		//	UI manager
	CECWorld*			m_pWorld;			//	Game world
	CECInputCtrl*		m_pInputCtrl;		//	Input controller
	CECHostInputFilter*	m_pInputFilter;		//	Host input filter
// 	CECAutoHomeInputFilter* m_pAutoHomeFilter;  // Auto home filter
	CECLoadProgress*	m_pLoadProgress;	//	Load progress
	CECFullGlowRender*	m_pFullGlowRender;	//	Full screen glow render

	CECHostPlayer*		m_pHostPlayer;		//	Host player object
	ID2NameTable		m_ID2NameTab;		//	Player ID to name table
	Name2IDTable		m_Name2IDTab;		//	Player name to ID table
	ACString			m_strAutoName;		//	Auto generated name
	CECMessageMan*		m_pMessageMan;		//	Game message manager
	CECClipboard*		m_pClipboard;		//	Clipboard

	InstTable			m_InstTab;			//	Instance table	

	SoundStringTable	m_SoundStringTable;
	SoundTable				m_SoundTable;

	CECTeamMan*			m_pTeamMan;			//	Team manager
	CECShortcutSet*		m_pNormalSCS;		//	Normal shortcut set
	CECShortcutSet*		m_pTeamSCS;			//	Team shortcut set
	CECShortcutSet*		m_pTradeSCS;		//	Trade shortcut set
	CECShortcutSet*		m_pPoseSCS;			//	Pose shortcut set
	CECShortcutSet*		m_pFactionSCS;		//	Faction shortcut set

	int			m_iGameState;		//	Game state
	bool		m_bPVPMode;
	DWORD		m_dwRenderTime;		//	Render time of this tick
	bool		m_bUIHasCursor;		//	true, show UI cursor. false, show game cursor
	int			m_iLogoutFlag;		//	Logout flag
	int			m_iCfgDataSize;		//	Size of config data sent last time
	BYTE*		m_pCfgDataBuf;		//	Config data sent last time
	
	int 		m_iDblExpMode;		//	Double experence mode
	int			m_iDExpEndTime;		//	End time of current double experence
	int			m_iAvaDExpTime;		//	Available double experence time
	bool		m_bLockGamePt;		//	true, lock game points
	S2C::player_wallow_info m_WallowInfo;
	AccountLoginInfo m_AccountLoginInfo;
	bool						m_bAccountLoginInfoShown;
	char		m_accountInfoFlag;	//	bit 0:需要补填身份证和姓名 bit1: 需要补填账号密码邮箱
	bool		m_bAccountInfoShown;

	unsigned int m_nGShopTimeStamp;
	unsigned int m_nGShopTimeStamp2;
	
	CommonDataTable m_CommonDataTab;	//  Common data table
	UniqueDataMap m_UniqueDataMap;	//	unique data for history 
	
	ACString			m_strCurServer;	//  Current server's name
	ACString			m_strCurGroup;	//	Current server's group

	int			m_iInGameDelay;			//	当前网络延迟时间（单位：毫秒）

	int			m_SellingRoleID;		// the role id marked as selling

	CECPendingActionArray	m_pendingLogout;
	CECMemSimplify *	m_pMemSimplify;

	typedef AArray<HistoryProgConfig,HistoryProgConfig> MAPVEC;
	typedef abase::hash_map<int,MAPVEC*> STAGEMAP;

	STAGEMAP m_stageMap;

	CECRandomMapProcess* m_pRandomMapProc;	// 随机地图处理类

	friend class CECWorld;
public:
	CECFullGlowRender* GetFullGlowRender() { return m_pFullGlowRender; }

	//	Release name/id pairs cache
	void ClearNameIDPairs();

	CECRandomMapProcess* GetRandomMapProc() { return m_pRandomMapProc;}
	void OnRandomMap(int row,int col,int* pData);

protected:	//	Operations
	//	Create world
	bool CreateWorld(int id, const A3DVECTOR3& vHostPos, int iParallelWorldID);
	//	Release world
	void ReleaseWorld();
	//	Create Login World
	bool CreateLoginWorld();
	//	Release Login World
	void ReleaseLoginWorld();
	//	Create host player
	bool CreateHostPlayer();
	//	Release host player
	void ReleaseHostPlayer();
	//	Create shortcuts
	bool CreateShortcuts();
	//	Release shortcuts
	void ReleaseShortcuts();

	void ReleasePendingActions();

	//	Load instance information from file
	bool LoadInstanceInfo(const char* szFile);

	bool ImportSoundStringTable(const char* szFile);

	//	End current game state
	void EndGameState(bool bReset=true);
	//	End login state
	void OnEndLoginState();
	//	End game state
	void OnEndGameState();
	//	Re-login
	bool Relogin();
	//	Logout
	void Logout();

	//	Tick game world
	bool TickGameWorld(DWORD dwDeltaTime, CECViewport* pViewport);
	//	Begin render
	bool BeginRender();
	//	End render
	bool EndRender(bool bPresent=true);
	//	Display game statistic
	void DisplayGameStatistic();

	bool LoadHistoryStage(const char* szFile);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

