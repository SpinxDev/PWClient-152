/*
 * FILE: EC_Configs.h
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

#include "AAssist.h"
#include "A3DVector.h"
#include "EC_GPDataType.h"
#include "EC_RTDebug.h"
#include "EC_Optimize.h"

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

#define EC_AUTOREPLY_LEN	64
#define EC_BLACKLIST_LEN	20
#define EC_USERCHANNEL_NUM	6
#define EC_COMBOSKILL_NUM	8
#define EC_COMBOSKILL_LEN	8

enum {
	DEFAULT_UI_THEME_ID = 0,
};

///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////

#pragma pack(1)

struct EC_SYSTEM_SETTING
{
	BYTE	nLevel;
	BYTE	nSight;
	BYTE	nWaterEffect;
	bool	bSimpleTerrain;
	BYTE	nTreeDetail;
	BYTE	nGrassDetail;
	BYTE	nCloudDetail;
	bool	bShadow;
	bool	bMipMapBias;
	bool	bFullGlow;
	bool	bSpaceWarp;
	bool	bSunFlare;
	bool	bAdvancedWater;
	BYTE	nSoundVol;
	BYTE	nMusicVol;
	int		iGamma;
	int		iRndWidth;
	int		iRndHeight;
	int		iTexDetail;
	int		iSoundQuality;
	bool	bFullScreen;
	bool	bWideScreen;
	bool	bVSync;
	bool	bScaleUI;
	int		iTheme;

	void Reset()
	{
		nLevel = 0;
		nSight = 0;
		nWaterEffect = 0;
		bSimpleTerrain = true;
		nTreeDetail = 0;
		nGrassDetail = 0;
		nCloudDetail = 0;
		bShadow = false;
		bMipMapBias = false;
		bFullGlow = false;
		bSpaceWarp = false;
		bSunFlare = false;
		bAdvancedWater = false;
		nSoundVol = 100;
		nMusicVol = 100;
		iGamma = 41;
		iRndWidth = 800;
		iRndHeight = 600;
		iTexDetail = 2;
		iSoundQuality = 2;
		bFullScreen = true;
		bWideScreen = false;
		bVSync = true;
		bScaleUI = false;
		iTheme = DEFAULT_UI_THEME_ID;
	}
};

struct ConfigReader
{
	//	读取配置辅助函数
	template <typename T>
	static void Read(T & t, CECDataReader &dr)
	{
		memcpy(&t, dr.Read_Data(sizeof(t)), sizeof(t));
	}
	
	template <typename L, typename R>
	static void Read(L & left, R &right, CECDataReader &dr)
	{
		Read(right, dr);
		for (int i = 0; i < sizeof(left)/sizeof(left[0]) && i < sizeof(right)/sizeof(right[0]); ++ i)
		{
			for (int j = 0; j < sizeof(left[0])/sizeof(left[0][0]) && j < sizeof(right[0])/sizeof(right[0][0]); ++ j)
				left[i][j] = right[i][j];
		}
	}
};

struct EC_COMBOSKILL
{
	BYTE	nIcon;
	short	idSkill[EC_COMBOSKILL_LEN];

	bool Clear(int id)
	{
		//	清除 idSkill 中所有为 id 的项，返回 true 表明有项目被清除
		bool bClear(false);
		for (int i(0); i < EC_COMBOSKILL_LEN; ++ i)
		{
			if (idSkill[i] == (short)id)
			{
				idSkill[i] = 0;
				bClear = true;
			}
		}
		return bClear;
	}
};

struct EC_VIDEO_SETTING
{
	bool bPlayerHP;
	bool bPlayerMP;
	bool bPlayerEXP;
	bool bPlayerHeadText;
	bool bPlayerName;
	bool bPlayerFaction;
	bool bPlayerTitle;
	bool bPlayerShop;
	bool bPlayerSelfName;
	bool bPlayerTalk;
	bool bPlayerBubble;
	bool bMonsterName;
	bool bNPCName;
	bool bGoodsName;
	bool bModelLimit;
	BYTE nDistance;
	bool bMapMonster;
	bool bMapNPC;
	bool bMapPlayer;
	BYTE nEffect;
	bool bShowCustomize;
	EC_COMBOSKILL comboSkill[EC_COMBOSKILL_NUM];
	bool bPlayerForce;
	bool bMailToFriendsRemind;
	int iMailToFriendsCheckTime;
	char cTabSelType;
	bool bActivityRemind;
	int ibActivityReminderCheckTime;
	bool bCloseMeridiansHint;

	void Reset()
	{
		bPlayerHP = false;
		bPlayerMP = false;
		bPlayerEXP = false;
		bPlayerHeadText = true;
		bPlayerName = true;
		bPlayerFaction = true;
		bPlayerTitle = true;
		bPlayerShop = true;
		bPlayerSelfName = true;
		bPlayerTalk = true;
		bPlayerBubble = false;
		bMonsterName = true;
		bNPCName = true;
		bGoodsName = true;
		bModelLimit = false;
		nDistance = 50;
		bMapMonster = false;
		bMapNPC = true;
		bMapPlayer = true;
		nEffect = 4;
		bShowCustomize = true;
		bPlayerForce = true;
		bMailToFriendsRemind = true;
		iMailToFriendsCheckTime = 0;
		bActivityRemind = true;
		ibActivityReminderCheckTime = 0;
		cTabSelType = 0;
		bCloseMeridiansHint = false;
		
		for(int i = 0; i < EC_COMBOSKILL_NUM; i++ )
			comboSkill[i].nIcon = 0;
	}

	void Read(CECDataReader &dr, DWORD dwVer)
	{
		Reset();

		if (dwVer < 15) return;

		ConfigReader::Read(bPlayerHP, dr);
		ConfigReader::Read(bPlayerMP, dr);
		ConfigReader::Read(bPlayerEXP, dr);
		ConfigReader::Read(bPlayerHeadText, dr);
		ConfigReader::Read(bPlayerName, dr);
		ConfigReader::Read(bPlayerFaction, dr);
		ConfigReader::Read(bPlayerTitle, dr);
		ConfigReader::Read(bPlayerShop, dr);
		ConfigReader::Read(bPlayerSelfName, dr);
		ConfigReader::Read(bPlayerTalk, dr);
		ConfigReader::Read(bPlayerBubble, dr);
		ConfigReader::Read(bMonsterName, dr);
		ConfigReader::Read(bNPCName, dr);
		ConfigReader::Read(bGoodsName, dr);
		ConfigReader::Read(bModelLimit, dr);
		ConfigReader::Read(nDistance, dr);
		ConfigReader::Read(bMapMonster, dr);
		ConfigReader::Read(bMapNPC, dr);
		ConfigReader::Read(bMapPlayer, dr);
		ConfigReader::Read(nEffect, dr);
		ConfigReader::Read(bShowCustomize, dr);
		ConfigReader::Read(comboSkill, dr);
		
		if (dwVer >= 21)
			ConfigReader::Read(bPlayerForce, dr);
		
		if (dwVer >= 22)
		{
			ConfigReader::Read(bMailToFriendsRemind, dr);
			ConfigReader::Read(iMailToFriendsCheckTime, dr);
		}
		
		if (dwVer >= 24)
			ConfigReader::Read(cTabSelType, dr);
		if (dwVer >= 28)
		{
			ConfigReader::Read(bActivityRemind, dr);
			ConfigReader::Read(ibActivityReminderCheckTime, dr);
		}
		if (dwVer >= 31)
		{
			ConfigReader::Read(bCloseMeridiansHint, dr);
		}
	}
};

struct EC_GAME_SETTING
{
	bool bNoTeamRequest;
	bool bNoTradeRequest;
	bool bTurnaround;
	bool bReverseWheel;
	bool bChannel[EC_USERCHANNEL_NUM][GP_CHAT_MAX];
	bool bAutoReply;
	ACHAR szAutoReply[EC_AUTOREPLY_LEN+1];
	float fCamTurnSpeed;
	float fCamZoomSpeed;
	BYTE nFontSize;
	bool bAtk_Player;
	bool bAtk_NoMafia;
	bool bAtk_NoWhite;
	bool bFontBold;
	bool bBls_NoRed;
	bool bBls_NoMafia;
	bool bBls_Self;
	bool bBlsRefuse_Neutral;
	bool bHideAutoGuide;
	bool bAtk_NoAlliance;
	bool bBls_NoAlliance;
	bool bBlsRefuse_NonTeammate;
	bool bAtk_NoForce;
	bool bBls_NoForce;
	bool bLockQuickBar;
	bool bPetAutoSkill;
	bool bAutoTeamForTask;
	bool bDisableAutoWikiHelp;
	bool bExclusiveAwardMode;
	bool bHideIceThunderBall;
	void Reset()
	{
		bNoTeamRequest = false;
		bNoTradeRequest = false;
		bTurnaround = false;
		bReverseWheel = false;
		bAutoReply = false;
		szAutoReply[0] = '\0';
		fCamTurnSpeed = 10.0f;
		fCamZoomSpeed = 1.0f;
		nFontSize = 2;
		bAtk_Player = false;
		bAtk_NoMafia = true;
		bAtk_NoWhite = false;
		bBls_NoRed = false;
		bBls_NoMafia = false;
		bBls_Self = false;
		bBlsRefuse_Neutral = false;
		bFontBold = true;
		bHideAutoGuide = false;
		bAtk_NoAlliance = true;
		bBls_NoAlliance = false;
		bBlsRefuse_NonTeammate = false;
		bAtk_NoForce = false;
		bBls_NoForce = false;
		bLockQuickBar = false;
		bPetAutoSkill = false;
		bAutoTeamForTask = true;
		bDisableAutoWikiHelp = false;
		bExclusiveAwardMode = true;
		bHideIceThunderBall = false;
		bool a_bChannel[EC_USERCHANNEL_NUM][GP_CHAT_MAX] =
		{	// 本地  世界   组队   帮派   密语   伤害   战斗   交易   系统  广播  其它    副本  号角  战场	国家
			{ true,  true,  true,  true,  true,  false, false, true,  true,  true,  false,  true, true, true, true },// 普通
			{ false, false, true,  false, false, false, false, false, true,  true,  false,  true, true, true, true },// 组队
			{ false, false, false, true,  false, false, false, false, true,  true,	false,  true, true, true, true },// 帮派
			{ false, false, false, false, true,  false, false, false, true,  true,  false,  true, true, true, true },// 密语
			{ false, false, false, false, false, false, false, true,  true,  true,  false,  true, true, true, true },// 交易
			{ false, false, false, false, false, false, true,  false, false, false, true,   false,false,false,false} // 通用
		};
		
		memcpy(bChannel, a_bChannel, sizeof(bChannel));
	}

	void Read(CECDataReader &dr, DWORD dwVer)
	{
		Reset();

		if (dwVer < 15) return;

		ConfigReader::Read(bNoTeamRequest, dr);
		ConfigReader::Read(bNoTradeRequest, dr);
		ConfigReader::Read(bTurnaround, dr);
		ConfigReader::Read(bReverseWheel, dr);
		
		if (dwVer >= 29)
			ConfigReader::Read(bChannel, dr);
		else if (dwVer >= 26)
		{			
			bool temp[6][14];	//	EC_USERCHANNEL_NUM = 6 && GP_CHAT_MAX = 14
			ConfigReader::Read(bChannel, temp, dr);
		}
		else
		{
			bool temp[6][13];	//	EC_USERCHANNEL_NUM = 6 && GP_CHAT_MAX = 13
			ConfigReader::Read(bChannel, temp, dr);
		}
		
		ConfigReader::Read(bAutoReply, dr);
		ConfigReader::Read(szAutoReply, dr);
		ConfigReader::Read(fCamTurnSpeed, dr);
		ConfigReader::Read(fCamZoomSpeed, dr);
		ConfigReader::Read(nFontSize, dr);
		ConfigReader::Read(bAtk_Player, dr);
		ConfigReader::Read(bAtk_NoMafia, dr);
		ConfigReader::Read(bAtk_NoWhite, dr);
		ConfigReader::Read(bFontBold, dr);
		ConfigReader::Read(bBls_NoRed, dr);
		ConfigReader::Read(bBls_NoMafia, dr);
		ConfigReader::Read(bBls_Self, dr);
		
		if (dwVer >= 17)
			ConfigReader::Read(bBlsRefuse_Neutral, dr);
		
		if (dwVer >= 18)
			ConfigReader::Read(bHideAutoGuide, dr);
		
		if (dwVer >= 19)
			ConfigReader::Read(bAtk_NoAlliance, dr);
		
		if (dwVer >= 20)
			ConfigReader::Read(bBls_NoAlliance, dr);
		
		if (dwVer >= 23)
			ConfigReader::Read(bBlsRefuse_NonTeammate, dr);
		
		if (dwVer >= 25)
		{
			ConfigReader::Read(bAtk_NoForce, dr);
			ConfigReader::Read(bBls_NoForce, dr);
		}

		if (dwVer >= 32)
		{
			ConfigReader::Read(bLockQuickBar, dr);
			ConfigReader::Read(bPetAutoSkill, dr);
		}

		if (dwVer >= 33)
			ConfigReader::Read(bAutoTeamForTask, dr);

		if(dwVer >= 34)
			ConfigReader::Read(bDisableAutoWikiHelp,dr);
		if(dwVer >= 35)
			ConfigReader::Read(bExclusiveAwardMode,dr);
		if(dwVer >= 36)
			ConfigReader::Read(bHideIceThunderBall,dr);
	}
};

struct EC_BLACKLIST_SETTING
{
	int idPlayer[EC_BLACKLIST_LEN];
	int levelBlock;

	void Reset()
	{
		memset(this, 0, sizeof(EC_BLACKLIST_SETTING));
	}
	
	void Read(CECDataReader &dr, DWORD dwVer)
	{
		Reset();

		if (dwVer < 15) return;

		ConfigReader::Read(idPlayer, dr);

		if (dwVer >= 18)
			ConfigReader::Read(levelBlock, dr);
	}

};

struct EC_COMPUTER_AIDED_SETTING
{
	struct ITEM_GROUP 
	{
		bool	enable;
		int		item;
		char	percent;
		short	slot;

		void Reset(){
			enable = false;
			item = 0;
			percent = 0;
			slot = -1;
		}
	};

	enum {GROUP_COUNT = 5};
	struct ITEM_GROUP group[GROUP_COUNT];

	void Reset(){
		for (int i(0); i < GROUP_COUNT; ++ i)
			group[i].Reset();
	}

	void DisableAll(){
		for (int i(0); i < GROUP_COUNT; ++ i)
			group[i].enable = false;
	}

	void Read(CECDataReader &dr, DWORD dwVer)
	{
		Reset();

		if (dwVer < 27) return;
		
		if (dwVer < 30)
		{
			struct ITEM_GROUP temp[4];
			ConfigReader::Read(temp, dr);
			memcpy(group, temp, sizeof(temp));
		}
		else
			ConfigReader::Read(group, dr);

		DisableAll();
	}
};

struct EC_OPTIMIZE_SETTING
{
	CECOptimize::GFX gfx;
	bool bAutoSimplify;
	int iSimplifyMode;

	EC_OPTIMIZE_SETTING()
		: bAutoSimplify(false), iSimplifyMode(0)
	{
	}
};

#pragma pack()

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CECConfigs
//	
///////////////////////////////////////////////////////////////////////////

class CECConfigs
{
public:		//	Types

public:		//	Constructor and Destructor

	CECConfigs();
	virtual ~CECConfigs();

public:		//	Attributes

	//	Default environments
	float		m_fDefFogStart;
	float		m_fDefFogEnd;
	float		m_fDefFogDensity;
	DWORD		m_dwDefFogColor;
	AString		m_strDefSkyFile0;
	AString		m_strDefSkyFile1;
	AString		m_strDefSkyFile2;
	DWORD		m_dwDefLightDiff;
	DWORD		m_dwDefLightSpec;
	A3DVECTOR3	m_vDefLightDir;
	DWORD		m_dwDefAmbient;

	//	Test code, temporary parameters
	bool		m_bShowForest;		//	true, render forest, false, no render
	bool		m_bShowGrassLand;	//	true, render grass land, false, no render

public:		//	Operations

	//	Initialize object
	bool Init(const char* szCfgFile, const char* szClientIDFile, const char* szSSFile);

	//	Save system settings to local disk
	bool SaveSystemSettings();
	//	Save user configs (except system settings) to specified buffer
	bool SaveUserConfigData(void* pDataBuf, int* piSize);
	//	Load shortcut configs (except system settings) from specified buffer
	bool LoadUserConfigData(const void* pDataBuf, int iDataSize);
	//	Restore user settings to default value
	void DefaultUserSettings(EC_SYSTEM_SETTING *pss,
		EC_VIDEO_SETTING *pvs, EC_GAME_SETTING *pgs,
		EC_BLACKLIST_SETTING *pbs = NULL, EC_COMPUTER_AIDED_SETTING *pcas = NULL);
	//  Apply the data to game logic
	void ApplyUserSetting();

	//	Configs user cannot change ...
	int GetLanCodePage() { return m_iCodePage; }
	float GetDefFogStart() { return m_fDefFogStart; }
	float GetDefFogEnd() { return m_fDefFogEnd; }
	float GetDefFogDensity() { return m_fDefFogDensity; }
	DWORD GetDefFogColor() { return m_dwDefFogColor; }
	float GetSevActiveRadius() { return m_fSevActiveRad; }
	void SetSceneLoadRadius(float fRadius);
	float GetSceneLoadRadius();
	void SetPlayerVisRadius(float fRadius);
	float GetPlayerVisRadius() { return m_fCurPVRadius; } 
	float CalcPlayerVisRadius(int iPercent) { return m_fMinPVRadius + (m_fSevActiveRad - m_fMinPVRadius) * iPercent / 100.0f; }
	void SetForestDetail(float fDetail);
	void SetGrassLandDetail(float fDetail);
	float GetForestDetail() { return m_fForestDetail; }
	float GetGrassLandDetail() { return m_fGrassLandDetail; }
	float GetTerrainLODDist1() { return m_fTrnLODDist1; } 
	float GetTerrainLODDist2() { return m_fTrnLODDist2; } 
	DWORD GetPlayerTextFlags() { return m_dwPlayerText; }
	DWORD GetNPCTextFlags() { return m_dwNPCText; }
	int	GetMaxNameLen() { return m_iMaxNameLen; }
	int GetMultiClient() { return m_iMultiClient; }
	const ACHAR* GetWindowsTitle() { return m_strWindowsTitle; }
	const ACHAR* GetMiniDLTitle() { return m_strMiniDLTitle; }
	int	GetClientID() { return m_iClientID; }
	bool GetRegisterScriptFunc() { return m_bRegisterUIScriptFunc; }
	bool GetEnableGT(){ return m_bEnableGT; }
	bool GetEnableArc(){ return m_bEnableArc; }
	bool GetEnableArcAsia() { return m_bEnableArcAsia; }
	bool IsMiniClient() { return m_bMiniClient; }
	bool IsSendLogicCheckInfo(){ return m_bSendLogicCheckInfo; }
	AString GetRandomThemeFile();

	//	DEBUG only settings ...
	bool HasConsole() { return m_bConsole; }
	void EnableConsole(bool bEnable){ m_bConsole = bEnable; }
	void SetRTDebugLevel(int iLevel) { m_iRTDebug = iLevel; }
	int GetRTDebugLevel() { return m_iRTDebug; }
	void ShowNPCID(bool bShow) { m_bShowNPCID = bShow; }
	bool IsNPCIDShown() { return m_bShowNPCID; }
	void SetHostRunSpeed(float fSpeed)
	{ 
		if (fSpeed > 0.0f && fSpeed <= 15.0f)
			m_fRunSpeed = fSpeed;
	}
	float GetHostRunSpeed() { return m_fRunSpeed; }
	float GetHostWalkSpeed() { return m_fWalkSpeed; }
	bool GetTestDistFlag() { return m_bTestDist; }
	void SetTestDistFlag(bool bFlag) { m_bTestDist = bFlag; }
	bool GetShowPosFlag() { return m_bShowPos; }
	void SetShowPosFlag(bool bFlag) { m_bShowPos = bFlag; }
	bool GetShowForestFlag() { return m_bShowForest; }
	void SetShowForestFlag(bool bFlag) { m_bShowForest = bFlag; }
	bool GetShowGameStatFlag() { return m_bGameStat; }
	void SetShowGameStatFlag(bool bFlag) { m_bGameStat = bFlag; }
	bool GetShowIDFlag() { return m_bShowID; }
	void SetShowIDFlag(bool bFlag) { m_bShowID = bFlag; }
	bool GetSkipFrameFlag() { return m_bSkipFrame; }
	void SetSkipFrameFlag(bool bTrue) { m_bSkipFrame = bTrue; }
	bool GetModelUpdateFlag() { return m_bModelUpt; }
	void SetModelUpdateFlag(bool bTrue) { m_bModelUpt = bTrue; }
	void SetDebugFog(bool bDebug){ m_bDebugFog = bDebug; }
	bool IsDebugFog()const{ return m_bDebugFog; }
	void SetDebugFogParameter(float fFogStart, float fFogEnd, float fFogDensity, DWORD fogColor){
		m_fDebugFogStart	= fFogStart;
		m_fDebugFogEnd		= fFogEnd;
		m_fDebugFogDensity	= fFogDensity;
		m_dwDebugFogColor	= fogColor;
	}
	float GetDebugFogStart()const{ return m_fDebugFogStart; }
	float GetDebugFogEnd()const{ return m_fDebugFogEnd; }
	float GetDebugFogDensity()const{ return m_fDebugFogDensity; }
	DWORD GetDebugFogColor()const{ return m_dwDebugFogColor; }

	bool IsAdjusting() { return m_bAdjusting; }

	//	User settings ...
	const EC_SYSTEM_SETTING& GetSystemSettings() { return m_ss; }
	const EC_VIDEO_SETTING& GetVideoSettings() { return m_vs; }
	const EC_GAME_SETTING& GetGameSettings() { return m_gs; }
	const EC_BLACKLIST_SETTING& GetBlackListSettings() { return m_bs; }
	const EC_COMPUTER_AIDED_SETTING &GetComputerAidedSetting()const {return m_cas; }
	const EC_OPTIMIZE_SETTING& GetOptimizeSettings() { return m_ops; }
	
	EC_SYSTEM_SETTING * GetSystemSettingsPtr() { return &m_ss; }
	void SetSystemSettings(const EC_SYSTEM_SETTING& ss, bool bCallFromWndProc=false);
	void SetVideoSettings(const EC_VIDEO_SETTING& vs);
	void SetGameSettings(const EC_GAME_SETTING& gs);
	void SetGameSettings_OptimizeFunction(const EC_GAME_SETTING& gs);		// 设置游戏设置中优化设置部分，对应优化设置-功能设置界面
	void SetGameSettings_NoOptimizeFunction(const EC_GAME_SETTING& gs);		// 设置游戏设置中非优化设置部分，对应游戏设置界面
	void SetBlackListSettings(const EC_BLACKLIST_SETTING& bs);
	void SetComputerAidedSetting(const EC_COMPUTER_AIDED_SETTING& cas);

	void SetGfxOptimize(const CECOptimize::GFX& gfx) { m_ops.gfx = gfx; }
	void SetModelOptimize(bool bAutoSimplify, int iSimplifyMode);
	void ApplyOptimizeSetting();

	// get the block list by adding sequence
	const abase::vector<int>& GetBlockedList() const { return m_BlockedArr; }
	bool IsPlayerBlocked(int idPlayer);
	// add a name to block list, empty name means remove from list
	void SetBlockedName(int idPlayer, const ACString* pName);
	//	load and merge the blocked role id from network and local setting
	int	LoadBlockedList();
	//	save current blocked list to local file and network data
	void SaveBlockedID();

	void ApplyWorldSpecificSetting();

	static bool ShouldForceWaterRefract();
	static bool IsUseWaterRefract(const EC_SYSTEM_SETTING &ss);
	bool IsUseWaterRefract()const;
	static bool CanUseWaterRefract();
	static void UseWaterRefract(EC_SYSTEM_SETTING &ss);
	void UseWaterRefract();

	static bool ShouldForceWaterReflect();
	static bool IsUseWaterReflect(const EC_SYSTEM_SETTING &ss);
	bool IsUseWaterReflect()const;
	static bool CanUseWaterReflect();
	static void UseWaterReflect(EC_SYSTEM_SETTING &ss);
	void UseWaterReflect();

protected:	//	Attributes

	//	Configs user cannot change ...
	int			m_iCodePage;		//	Language code page
	float		m_fSevActiveRad;	//	Server active radius
	float		m_fSceLoadRadius;	//	Current scene loading radius
	float		m_fMinPVRadius;		//	Minimum player visible radius
	float		m_fCurPVRadius;		//	Current player visible radius
	float		m_fForestDetail;	//	Forest detail (0-1)
	float		m_fGrassLandDetail;	//	Grass land detail (0-1)
	float		m_fTrnLODDist1;		//	Terrain LOD distance 1
	float		m_fTrnLODDist2;		//	Terrain LOD distance 2
	DWORD		m_dwPlayerText;		//	Player text flags
	DWORD		m_dwNPCText;		//	NPC text flags
	int			m_iRTDebug;			//	Run-time debug message level
	int			m_iMaxNameLen;		//	Maximum name length in character
	int			m_iMultiClient;		//  Allow multi clients 1
	ACString		m_strWindowsTitle;	//  Game windows title text 
	ACString	m_strMiniDLTitle;	//  迷你客户端下载器的窗口名字
	int			m_iClientID;			//	客户端版本标识，可用于区分不同国家地区版本
	bool		m_bRegisterUIScriptFunc;//	是否注册脚本函数供UI脚本调用
	bool		m_bEnableGT;			//	是否启用 GT 模块
	bool		m_bEnableArc;	//	是否启用 Arc SDK 模块
	bool		m_bEnableArcAsia;	//  是否启用ArcAsia
	bool		m_bMiniClient;		//  是否是迷你客户端
	bool		m_bSendLogicCheckInfo;	//	是否向服务器发送客户端机器信息
	abase::vector<AString>	m_strThemeFiles;	//	主题音乐

	//	DEBUG only settings ...
	bool		m_bConsole;			//	true, using console
	bool		m_bShowNPCID;		//	Show NPC's ID on it's head	
	float		m_fRunSpeed;		//	Host run speed.
	float		m_fWalkSpeed;		//	Host walk speed
	bool		m_bTestDist;		//	Test distance
	bool		m_bShowPos;			//	Show position
	bool		m_bGameStat;		//	Show game statistic
	bool		m_bShowID;			//	Show player ID
	bool		m_bSkipFrame;		//	Skip frame flag
	bool		m_bModelUpt;		//	Model update optimization flag
	bool		m_bDebugFog;
	float		m_fDebugFogStart, m_fDebugFogEnd, m_fDebugFogDensity;
	DWORD		m_dwDebugFogColor;

	bool		m_bAdjusting;		//	flag indicates the system setting is being adjusted

	//	User settings ...
	EC_SYSTEM_SETTING		m_ss;		//	System settings
	EC_VIDEO_SETTING		m_vs;		//	Video settings
	EC_GAME_SETTING			m_gs;		//	Game settings
	EC_BLACKLIST_SETTING	m_bs;		//	Blacklist settings
	EC_COMPUTER_AIDED_SETTING	m_cas;
	EC_OPTIMIZE_SETTING		m_ops;		//  Optimize settings

	//	Blocked name list by adding sequence
	abase::vector<int> m_BlockedArr;

protected:	//	Operations

	//	Load configs from file
	bool LoadConfigs(const char* szFile, const char* szClientIDFile);
	//	Load system settings from local disk file
	bool LoadSystemSettings(const char* szFile);
	//	Set default user config data
	void DefaultUserConfigData();
	
	//	Convert sight radius setting
	void ConvertSightSetting(int iSight);
	//	Build player and NPC text flags
	void BuildTextFlags();
	//	Verify all config settings
	void Verify();
	//	Verify combo-skill ids
	void VerifyComboSkills();

	void ApplyWorldSpecificSetting(EC_SYSTEM_SETTING& ss)const;
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////


