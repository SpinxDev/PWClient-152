/*
 * FILE: EC_ManPlayer.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/8/30
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include "EC_Manager.h"
#include "EC_GPDataType.h"
#include "EC_Counter.h"
#include "AArray.h"
#include "hashtab.h"
#include "vector.h"
#include "EC_Player.h"
#include "EC_Observer.h"

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

class CECPlayer;
class CECHostPlayer;
class CECElsePlayer;
class CECEPCacheData;
class CECLoginPlayer;
class CECModel;
class A3DSkinModel;

class CECFactionMan;
typedef CECObserver<CECFactionMan>	CECFactionManObserver;

#define PLAYERLOADRESULT_PLAYERMODEL	0x1
#define PLAYERLOADRESULT_PETMODEL		0x2
#define PLAYERLOADRESULT_EQUIPMODEL		0x4
#define PLAYERLOADRESULT_DUMMYMODEL		0x8
#define PLAYERLOADRESULT_PLAYERFACE		0x10

//	Player load result structure used by CECPlayer::LoadPlayerModel
//	dwValidMask should be set to match the data in this structure
struct EC_PLAYERLOADRESULT
{
	DWORD		dwValidMask;
	CECModel*	pPlayerModel;
	int			iShape;
	CECModel*	pDummyModel;
	CECModel*	pPetModel;
	CECFace*	pFaceModel;
	A3DShader*	pBodyShaders[3];
	CECModel*	pFlyNviagteModel;
	CECPlayer::EquipsLoadResult	EquipResult;
};

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECPlayerMan
//	
///////////////////////////////////////////////////////////////////////////

class CECPlayerMan : public CECManager, public CECFactionManObserver
{
public:		//	Types

	//	Loaded player model information
	struct PLAYERMODEL
	{
		int	cid;					//	Player's character ID
		DWORD dwBornStamp;
		bool bClientModel;
		EC_PLAYERLOADRESULT Ret;	//	Model loaded result
	};

	typedef abase::hashtab<CECElsePlayer*, int, abase::_hash_function>	PlayerTable;
	typedef abase::hashtab<CECEPCacheData*, int, abase::_hash_function>	CacheTable;
	typedef abase::vector<CECLoginPlayer *>	LoginPlayerTable;
	typedef abase::hashtab<int, int, abase::_hash_function> UkPlayerTable;

public:		//	Constructor and Destructor

	CECPlayerMan(CECGameRun* pGameRun);
	virtual ~CECPlayerMan();

public:		//	Attributes

public:		//	Operations

	//	Release manager
	virtual void Release();

	//	Tick routine
	virtual bool Tick(DWORD dwDeltaTime);
	//	Render routine
	virtual bool Render(CECViewport* pViewport);
	//	RenderForReflected routine
	virtual bool RenderForReflect(CECViewport * pViewport);
	//	RenderForRefract routine
	virtual bool RenderForRefract(CECViewport * pViewport);

	//	Process message
	virtual bool ProcessMessage(const ECMSG& Msg);

	//	On entering game world
	virtual bool OnEnterGameWorld();
	//	On leaving game world
	virtual bool OnLeaveGameWorld();

	//	Ray trace
	virtual bool RayTrace(ECRAYTRACE* pTraceInfo);
	
	//	ÅÉÉú×Ô CECFactionManObserver
	virtual void OnModelChange(const CECFactionMan *p, const CECObservableChange *q);

	inline PlayerTable& GetPlayerTable();
	inline LoginPlayerTable& GetLoginPlayerTable() { return m_LoginPlayers; }

	//	Get host player
	CECHostPlayer* GetHostPlayer();
	//	Get else player by id
	CECElsePlayer* GetElsePlayer(int cid, DWORD dwBornStamp=0);
	//	Get a player (may be host or else player) by id
	CECPlayer* GetPlayer(int cid, DWORD dwBornStamp=0);
	//	Tick host player separately
	bool TickHostPlayer(DWORD dwDeltaTime, CECViewport* pViewport);
	//	Get else player number
	int GetElsePlayerNum() { return (int)m_PlayerTab.size(); }
	//	Query player's booth name
	void QueryPlayerBoothName(int idPlayer) { m_aBoothQuery.Add(idPlayer); }

	//	Get else player cache data
	CECEPCacheData* GetPlayerCacheData(int cid);

	//	Get players should appear in mini-map
	const APtrArray<CECElsePlayer*>& GetPlayersInMiniMap() { return m_aMMPlayers; }
	//	When world's loading center changed
	void OnLoadCenterChanged();

	//	Load player model in loading thread
	bool ThreadLoadPlayerModel(int cid, DWORD dwBornStamp, int iProfession, int iGender, int iCustom, const int* pEquips, const char* szPetPath,bool bClient);
	bool ThreadLoadPlayerEquips(int cid, DWORD dwBornStamp, int iProfession, int iGender, const int* pEquips, __int64 EquipMask);
	bool ThreadLoadPetModel(int cid, DWORD dwBornStamp, const char* szPetPath);
	bool ThreadLoadDummyModel(int cid, DWORD dwBornStamp, int iShape,bool bClient );
	bool ThreadLoadPlayerFace(int cid, DWORD dwBornStamp, int iProfession, int iGender, int iCustom);

	//	Get player candidates whom can be auto-selected by 'TAB' key
	void TabSelectCandidates(int idCurSel, APtrArray<CECElsePlayer*>& aCands);

	void OptimizeShowExtendStates();
	void OptimizeWeaponStoneGfx();
	void OptimizeArmorStoneGfx();
	void OptimizeSuiteGfx();
	CECElsePlayer* GetMouseOnPateTextPlayer(int x, int y);

protected:	//	Attributes

	PlayerTable			m_PlayerTab;	//	Players in view area
	UkPlayerTable		m_UkPlayerTab;	//	Unknown player table
	CacheTable			m_PlayerCache;	//	Player cache
	CECCounter			m_QueryCnt;		//	Query unknown player time counter
	CECCounter			m_CacheCnt;		//	Cache garbage collector time counter
	CECCounter			m_TLTimeCnt;	//	Tank leader update time counter

	LoginPlayerTable	m_LoginPlayers;	//	Players in login UI, just for render here

	APtrArray<CECElsePlayer*>	m_aMMPlayers;		//	Player will appear in mini-maps
	APtrArray<PLAYERMODEL*>		m_aLoadedModels;	//	Loaded player models
	CRITICAL_SECTION			m_csLoad;			//	Loaded models lock
	CRITICAL_SECTION			m_csPlayerTab;		//  Player table lock
	AArray<int, int>			m_aBoothQuery;		//	Used to query player's booth name

protected:	//	Operations

	//	Create else player
	CECElsePlayer* CreateElsePlayer(const S2C::info_player_1& Info, int iAppearFlag);
	//	Release else player
	void ReleaseElsePlayer(CECElsePlayer* pPlayer);

	//	One else player enter view area
	CECElsePlayer* ElsePlayerEnter(const S2C::info_player_1& Info, int iCmd);
	//	One else player leave view area or exit game
	void ElsePlayerLeave(int cid, bool bExit, bool bUpdateMMArray=true);
	//	One else player's more information arrived
	bool MoreElsePlayerInfo2(int cid, const S2C::info_player_2& Info);
	//	One else player's more information arrived
	bool MoreElsePlayerInfo3(int cid, const S2C::info_player_3& Info);
	//	One else player's more information arrived
	bool MoreElsePlayerInfo4(int cid, const S2C::info_player_4& Info);
	//	Host information arrived
	bool HostPlayerInfo1(const S2C::cmd_self_info_1& Info);
	//	Update players in various ranges (Active, visible, mini-map etc.)
	void UpdatePlayerInRanges(DWORD dwDeltaTime);
	//	Remove else player from m_aMMPlayers array
	void RemovePlayerFromMiniMap(CECElsePlayer* pPlayer);

	//	Pop player from cache
	CECEPCacheData* PopPlayerFromCache(int cid);
	//	Push player to cache
	bool PushPlayerToCache(CECEPCacheData* pData);
	//	Clear up player data cache
	void ClearUpPlayerCache();
	//	Is player in cache ?
	inline bool IsPlayerInCache(int cid);
	//	Seek out else player with specified id, if he doesn't exist, try to get from server
	CECElsePlayer* SeekOutElsePlayer(int cid);
	//	Update unknown else player table
	void UpdateUnknownElsePlayers();
	//	Deliver loaded player models
	void DeliverLoadedPlayerModels();

	//	Messages handlers
	bool OnMsgPlayerInfo(const ECMSG& Msg);
	bool OnMsgPlayerMove(const ECMSG& Msg);
	bool OnMsgPlayerRunOut(const ECMSG& Msg);
	bool OnMsgPlayerExit(const ECMSG& Msg);
	bool OnMsgPlayerDied(const ECMSG& Msg);
	bool OnMsgPlayerRevive(const ECMSG& Msg);
	bool OnMsgPlayerOutOfView(const ECMSG& Msg);
	bool OnMsgPlayerDisappear(const ECMSG& Msg);
	bool OnMsgPlayerStopMove(const ECMSG& Msg);
	bool OnMsgPlayerLevelUp(const ECMSG& Msg);
	bool OnMsgGoblinLevelUp(const ECMSG& Msg);
	bool OnMsgPlayerInvisible(const ECMSG& Msg);
	bool OnMsgPlayerExtProp(const ECMSG& Msg);
	bool OnMsgPlayerLeaveTeam(const ECMSG& Msg);
	bool OnMsgDoConEmote(const ECMSG& Msg);
	bool OnMsgInvalidObject(const ECMSG& Msg);
	bool OnMsgPlayerExtState(const ECMSG& Msg);
	void OnMsgPickUpMatter(const ECMSG& Msg);
	void OnMsgPlayerBindStart(const ECMSG& Msg);
	void OnMsgPlayerDuelResult(const ECMSG& Msg);
	void OnMsgTankLeader(const ECMSG& Msg);
	void OnMsgPlayerChangeSpouse(const ECMSG& Msg);
	bool OnMsgForbidBeSelected(const ECMSG& Msg);

	//	Transmit message
	bool TransmitMessage(const ECMSG& Msg);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

//	Is player data in cache ?
bool CECPlayerMan::IsPlayerInCache(int cid)
{
	CacheTable::pair_type Pair = m_PlayerCache.get(cid);
	return Pair.second;
}

CECPlayerMan::PlayerTable& CECPlayerMan::GetPlayerTable()
{
	return m_PlayerTab;
}
