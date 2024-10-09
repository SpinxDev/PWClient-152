/*
 * FILE: EC_ElsePlayer.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/9/10
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include "EC_Player.h"
#include "EC_GPDataType.h"
#include "EC_CDR.h"

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


///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CECEPCacheData
//	
///////////////////////////////////////////////////////////////////////////

class CECEPCacheData
{
public:		//	Types

	//	Extend resource flags
	enum
	{
		RESFG_BOOTHNAME	= 0x010000,
	};

public:		//	Constructor and Destructor

	CECEPCacheData();

public:		//	Attributes

	int			m_cid;				//	Character ID
	ACString	m_strName;			//	Player name
	int			m_iProfession;		//	Profession

	DWORD		m_dwResFlags;		//	Resource flags
	int			m_crc_c;			//	customized data crc
	int			m_crc_e;			//	Equipment data crc
	int			m_aEquips[SIZE_ALL_EQUIPIVTR];	//	Equipment item ID array

	int			m_crcBooth;			//	Booth CRC
	ACString	m_strBoothName;		//	Booth name

	CECPlayer::PLAYER_CUSTOMIZEDATA	m_CustomizeData;	//	Customize data

	DWORD		m_dwCacheTime;		//	Cache time stamp (s)

public:		//	Operations

protected:	//	Attributes

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECElsePlayer
//	
///////////////////////////////////////////////////////////////////////////
class CECEPWork;
class CECEPWorkMan;
class CECElsePlayer : public CECPlayer
{
public:
	// Passive move type
	enum
	{
		PASSIVE_DIRECT,		// cmd_player_teleport mode == 0
		PASSIVE_PULL,		// cmd_player_teleport mode == 1
		PASSIVE_KNOCKBACK,	// cmd_player_knockback
	};

	//	Player appear flag
	enum
	{
		APPEAR_DISAPPEAR = -1,		//	Player disappear
		APPEAR_ENTERWORLD = 0,		//	Player join world
		APPEAR_RUNINTOVIEW,			//	Player run into view
		APPEAR_GHOST,				//	Player is in ghost state, in player list but not active
	};

	friend class CECPlayerMan;
	friend class CECMemSimplify;

public:		//	Constructor and Destructor

	CECElsePlayer(CECPlayerMan* pPlayerMan);
	virtual ~CECElsePlayer();

public:		//	Attributes

public:		//	Operations

	//	Initlaize object
	bool Init(const S2C::info_player_1& Info, int iAppearFlag, bool bReInit);
	//	Initialize player from cached data
	bool InitFromCache(const S2C::info_player_1& Info, CECEPCacheData* pCacheData, int iAppearFlag);
	//	Release object
	virtual void Release();

	//	Tick routine
	virtual bool Tick(DWORD dwDeltaTime);
	//	Render routine
	virtual bool Render(CECViewport* pViewport, int iRenderFlag=0) { return InternalRender(pViewport, iRenderFlag, false); }
	bool RenderHighLight(CECViewport* pViewport) { return InternalRender(pViewport, RD_NORMAL, true); }
	//	Render when player is opening booth
	virtual bool RenderForBooth(CECViewport* pViewport, int iRenderFlag=0, bool bHighLight=false) { return RenderSimpleModels(true, pViewport, iRenderFlag, bHighLight); }
	//	Render when player using hint model
	bool RenderForHint(CECViewport* pViewport, int iRenderFlag=0, bool bHighLight=false) { return RenderSimpleModels(false, pViewport, iRenderFlag, bHighLight); }

	//	Process message
	virtual bool ProcessMessage(const ECMSG& Msg);

	//	Player was killed
	virtual void Killed(int idKiller);
	//	Is player in fight state ?
	virtual bool IsFighting();
	//	Get off pet
	virtual void GetOffPet(bool bResetData = true);
	//	Do an emote action
	virtual bool DoEmote(int idEmote);

	//	Set loaded model to player object, this function is used in multithread loading process
	virtual bool SetPlayerLoadedResult(EC_PLAYERLOADRESULT& Ret);

	//	Unload resources
	void UnloadResources();
	//	Create player cache data
	CECEPCacheData* CreateCacheData();
	//  Release the models
	void ReleaseModels();
	//  Reload the models
	void ReloadModels();

	//	Move to a destination
	void MoveTo(const S2C::cmd_object_move& Cmd);
	//	Stop moving to a destination position
	void StopMoveTo(const S2C::cmd_object_stop_move& Cmd);
	//	Revive
	void Revive(const S2C::cmd_player_revive* pCmd);
	//	Level up
	void LevelUp();
	//  Change invisible
	void ChangeInvisible(int iDegree);
	//	Invisible object detected
	void InvisibleDetected();
	//	Enter fight state
	void EnterFightState();
	//	Is base info ready ?
	bool IsBaseInfoReady() { return m_bBaseInfoReady; }
	//	Is customized data ready ?
	bool IsCustomDataReady() { return m_bCustomReady; }
	//	Is equipment data ready ?
	bool IsEquipDataReady() { return m_bEquipReady; }
	//	Get distance to host player
	float GetDistToHost() { return m_fDistToHost; }
	float GetDistToHostH() { return m_fDistToHostH; }
	//	Is spelling magic
	bool IsSpellingMagic();
	//	Turn to ghost mode
	void TurnToGhost() { m_iAppearFlag = APPEAR_GHOST; }
	//	Is player in ghost state ?
	bool InGhostState() { return m_iAppearFlag == APPEAR_GHOST ? true : false; }
	//	Is player jumping ?
	bool IsJumping() { return m_bJumping; }
	//	Get player's real position on server
	const A3DVECTOR3& GetServerPos() { return m_vServerPos; }
	//	Set server position
	void SetServerPos(const A3DVECTOR3& vPos);
	//	Get player in team state
	int GetInTeamState() { return m_iInTeamState; } 
	//	Is player moving
	virtual bool IsPlayerMoving();
	virtual bool IsWorkMoveRunning()const;
	virtual bool IsWorkSpellRunning()const;
	
	virtual int GetCertificateID()const;

	//	Set use hint model flag
	void SetUseHintModelFlag(bool bUse) { m_bUseHintModel = bUse; }
	//	Get use hint model flag
	bool GetUseHintModelFlag() { return m_bUseHintModel; }

	//	Get equipment item
	int GetEquipment(int n) { return m_aEquips[n]; }

	//  Switch the simple model mode
	void SwitchSimpleModel();

	bool IsClickBoothBar(int x, int y);
	void SetMouseOnBoothFlag(bool IsHover);

	friend class CECEPWorkStand;
	friend class CECEPWorkMove;
	friend class CECEPWorkMelee;
	friend class CECEPWorkSpell;
	friend class CECEPWorkPickUp;
	friend class CECEPWorkDead;
	friend class CECEPWorkUseItem;
	friend class CECEPWorkIdle;
	friend class CECEPWorkFlashMove;
	friend class CECEPWorkPassiveMove;
	friend class CECEPWorkCongregate;
	friend class CECEPWorkSkillStateAct;

protected:	//	Attributes

	float		m_fDistToHost;			//	Distance to host player
	float		m_fDistToHostH;			//	Horizontal distance to host player
	
	int			m_iAppearFlag;			//	Player's appearing flag
	bool		m_bBaseInfoReady;		//	true, Basic info is ready
	bool		m_bCustomReady;			//	true, Customized data is ready
	bool		m_bEquipReady;			//	true, Equipment data is ready
	int			m_aNewEquips[SIZE_ALL_EQUIPIVTR];		//	New equipment item ID array
	__int64		m_i64NewEqpMask;			//	New equipment mask
	int			m_iMMIndex;				//	Index in CECPlayerMan::m_aMMPlayers array

	A3DGFXEx*	m_pAppearGFX;			//	Appear GFX
	int			m_iInTeamState;			//	Player team state. 0 no team; 1 leader; 2 member;
	CECCounter	m_FightCnt;				//	Fight time counter

	CECEPWorkMan * m_pEPWorkMan;

	bool		m_bStopMove;			//	Stop move flag
	A3DVECTOR3	m_vMoveDir;				//	Player's velocity
	A3DVECTOR3	m_vServerPos;			//	Player's real position on server
	float		m_fMoveSpeed;			//	Move speed
	float		m_fLastSpeed;			//	Move speed of last time
	DWORD		m_dwLastMoveTime;		//	Last move command arrived time
	A3DVECTOR3	m_vStopDir;				//	The direction when player stop moving
	bool		m_bJumping;				//	Jumping flag
	bool		m_bLoadingModel;		//	Model loading flag
	bool		m_bUseHintModel;		//	true, use hint model
	
	bool		m_bShowCustomize;		//	show customize flag

	OtherPlayer_Move_Info	m_cdr;
	
protected:	//	Operations

	//	When all resources are ready, this function will be called
	virtual void OnAllResourceReady();
	virtual bool SetPetLoadResult(CECModel* pPetModel);
	
	//	Render routine when player use simple models
	bool RenderSimpleModels(bool bBooth, CECViewport* pViewport, int iRenderFlag, bool bHighLight);
	//	Internal render routine
	bool InternalRender(CECViewport* pViewport, int iRenderFlag, bool bHighLight);

	//	Change equipment
	bool ChangeEquipments(bool bReset, int crc, __int64 iAddMask, __int64 iDelMask, int* aAddedEquip);
	//	Load player equipments
	bool LoadPlayerEquipments();
	//	Move step
	A3DVECTOR3 MoveStep(const A3DVECTOR3& vDir, float fSpeed, float fTime);
	//	Set player info
	void SetPlayerInfo(const S2C::info_player_1& Info);
	//	Set player's brief info
	bool SetPlayerBriefInfo(int iProf, int iGender, const ACHAR* szName);
	//  Reload the player's goblin
	bool ReloadGoblin();
	
	//	Move to destination position
	bool MovingTo(DWORD dwDeltaTime);
	//	Start the passive move
	void StartPassiveMove(const A3DVECTOR3& pos, int time, int type);
	
	bool IsCurPosWork()const;
	void StopCurPosWork();
	
	void ClearCastingSkill();

	virtual void DoSkillStateAction();
	
	//	Message handler
	void OnMsgPlayerFly(const ECMSG& Msg);
	void OnMsgPlayerBaseInfo(const ECMSG& Msg);
	void OnMsgPlayerCustomData(const ECMSG& Msg);
	void OnMsgPlayerEquipData(const ECMSG& Msg);
	void OnMsgPlayerSitDown(const ECMSG& Msg);
	void OnMsgPlayerAtkResult(const ECMSG& Msg);
	void OnMsgPlayerSkillResult(const ECMSG& Msg);
	void OnMsgPlayerCastSkill(const ECMSG& Msg);
	void OnMsgPlayerDoEmote(const ECMSG& Msg);
	void OnMsgPlayerUseItem(const ECMSG& Msg);
	void OnMsgPlayerUseItemWithData(const ECMSG& Msg);
	void OnMsgPlayerRoot(const ECMSG& Msg);
	void OnMsgPlayerDoAction(const ECMSG& Msg);
	void OnMsgPlayerInTeam(const ECMSG& Msg);
	void OnMsgPickupMatter(const ECMSG& Msg);
	void OnMsgPlayerGather(const ECMSG& Msg);
	void OnMsgDoConEmote(const ECMSG& Msg);
	void OnMsgPlayerChangeShape(const ECMSG& Msg);
	void OnMsgPlayerGoblinOpt(const ECMSG& Msg);
	void OnMsgBoothOperation(const ECMSG& Msg);
	void OnMsgPlayerTravel(const ECMSG& Msg);
	void OnMsgPlayerChangeFace(const ECMSG& Msg);
	void OnMsgPlayerBindOpt(const ECMSG& Msg);
	void OnMsgPlayerDuelOpt(const ECMSG& Msg);
	void OnMsgPlayerKnockback(const ECMSG& Msg);
	void OnMsgPlayerEquipDisabled(const ECMSG& Msg);
	void OnMsgCongregate(const ECMSG &Msg);
	void OnMsgTeleport(const ECMSG &Msg);
	void OnMsgForce(const ECMSG &Msg);
	void OnMsgMultiobjectEffect(const ECMSG &Msg);
	void OnMsgCountry(const ECMSG &Msg);
	void OnMsgTitle(const ECMSG &Msg);
	void OnMsgReincarnation(const ECMSG &Msg);
	void OnMsgRealmLevel(const ECMSG &Msg);
	void OnMsgPlayerInOutBattle(const ECMSG &Msg);
	void OnMsgFactionPVPMaskModify(const ECMSG &Msg);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

