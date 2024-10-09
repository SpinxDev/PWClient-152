/*
 * FILE: EC_HostPlayer.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/9/1
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include "EC_Player.h"
#include "EC_CameraCtrl.h"
#include "EC_HostMove.h"
#include "EC_IvtrTypes.h"
#include "EC_CDR.h"
#include "AArray.h"
#include "EC_Game.h"
#include "EC_GuildDiplomacyReminder.h"
#include "EC_TimeSafeChecker.h"
#include <bitset>

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

class A3DGFXEx;
class A2DSprite;
class AMSoundBuffer;
class CECObjectWork;
class CECInventory;
class CECDealInventory;
class CECNPCInventory;
class CECIvtrItem;
class CECIvtrEquip;
class CECShortcutSet;
class CECSkill;
class CECTaskInterface;
class CECHPWorkMan;
class CECFriendMan;
class CECMatter;
class CECIvtrArrow;
class CECPetCorral;
class CECPetWords;
class CECComboSkill;
class CECIvtrFashion;
struct FACTION_FORTRESS_CONFIG;
struct FACTION_BUILDING_ESSENCE;
class CECForceMgr;
class COnlineAwardCtrl;
class COfflineShopCtrl;
class CECAutoTeam;
struct PLAYER_REINCARNATION_CONFIG;
struct PLAYER_REALM_CONFIG;
struct CHARRACTER_CLASS_CONFIG;
struct PLAYER_SPIRIT_CONFIG;
class CChariot;
class CECActionSwitcherBase;
class CECQuickBuyPopSaveLifeTrigger;
class CECElsePlayer;
class CECNPC;
class CECHostNavigatePlayer;
class TraceObject;
class TraceMouseHit;
class TraceTaskObject;

namespace GNET
{
	class PlayerBriefInfo;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CECHostPlayer
//	
///////////////////////////////////////////////////////////////////////////

class CECHostPlayer : public CECPlayer
{
public:		//	Types

	//	Move relative direction
	enum
	{
		MD_FORWARD	= 0x01,	
		MD_RIGHT	= 0x02,
		MD_BACK		= 0x04,
		MD_LEFT		= 0x08,
		MD_ABSUP	= 0x10,
		MD_ABSDOWN	= 0x20,
		MD_ALL		= 0x3f,
	};

	//	Turning camera flag
	enum
	{
		TURN_LBUTTON = 0x01,
		TURN_RBUTTON = 0x02,
	};

	//	Mask of some special extend states which will influence host game logic.
	//	Logic Influence Extned states
	enum
	{
		LIES_SLEEP		= 0x0001,
		LIES_STUN		= 0x0002,
		LIES_ROOT		= 0x0004,
		LIES_NOFGIHT	= 0x0008,

		LIES_DISABLEFIGHT	= 0x000B,
	};

	//	Behavior id used by CanDo()
	enum
	{
		CANDO_SITDOWN = 0,
		CANDO_MOVETO,
		CANDO_MELEE,
		CANDO_ASSISTSEL,
		CANDO_FLY,
		CANDO_PICKUP,
		CANDO_TRADE,
		CANDO_PLAYPOSE,
		CANDO_SPELLMAGIC,
		CANDO_USEITEM,
		CANDO_JUMP,
		CANDO_FOLLOW,
		CANDO_GATHER,
		CANDO_BOOTH,
		CANDO_FLASHMOVE,
		CANDO_BINDBUDDY,
		CANDO_DUEL,
		CANDO_SUMMONPET,
		CANDO_CHANGESELECT,
		CANDO_REBUILDPET,
		CANDO_SWITCH_PARALLEL_WORLD,
	};

	// 聚灵状态
	enum MultiExpState
	{
		MES_NORMAL = 0,       // 无聚灵状态或聚灵暂停状态（当仅剩余聚灵时长不为0而缓冲和散灵时长为0时）
		MES_ENHANCE,          // 聚灵多倍经验状态
		MES_BUFFER,           // 聚灵缓冲状态
		MES_IMPAIR,           // 散灵状态
		MES_UNINITED,		  // 未初始化状态
	};

	// Tab Sel 
	enum TalSelType
	{
		TSL_NORMAL = 0,		// 普通模式
		TSL_MONSTER,		// 屏蔽怪物
		TSL_PLAYER,			// 屏蔽玩家
		TSL_NUM,
	};

	//	防御优化符类别ID
	enum DefenseRuneType
	{
		DEFENSE_RUNE_PHYSICAL,
		DEFENSE_RUNE_MAGIC,
		DEFENSE_RUNE_NUM,
	};

	//	Current ground information
	struct GNDINFO
	{
		float		fGndHei;		//	Ground height
		float		fWaterHei;		//	Water height
		A3DVECTOR3	vGndNormal;		//	Terrain normal
		bool		bOnGround;		//	On ground flag
	};

	//	Breath data
	struct BREATHDATA
	{
		bool		bDiving;		//	true, is diving
		int			iCurBreath;
		int			iMaxBreath;
	};

	//	Team invite info.
	struct TEAMINV
	{
		int		idLeader;
		int		seq;
		WORD	wPickFlag;
	};

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

	//	Battle info.
	enum BattleType
	{
		BT_NONE,
		BT_GUILD,				//	城战
		BT_COUNTRY,				//	国战
		BT_CHARIOT,				//  战场战车
	};
	struct BATTLEINFO
	{
		BattleType nType;		//	Battle type
		int		idBattle;		//	Battle id
		int		iResult;		//	Battle result
		int		iResultCnt;		//	Result time counter
		int		iMaxScore_I;	//	Maximum score of invader
		int		iMaxScore_D;	//	Maximum score of defender
		int		iScore_I;		//	Score of invader
		int		iScore_D;		//	Score of defender
		int		iEndTime;		//	Battle end time

		//	国战专用数据
		int		iOffenseCountry;
		int		iDefenceCountry;
		int		iReviveTimes;	//	剩余复活次数
		bool	bFlagCarrier;	//	是否是旗手
		int		iCarrierID;		//	扛旗者ID（bFlagCarrier为false时有效）
		A3DVECTOR3 posCarrier;	//	扛旗者位置（bFlagCarrier为false时有效）
		bool	bCarrierInvader;//	扛旗者是攻方（bFlagCarrier为false时有效）
		
		int		iCombatTime;	//	战斗时间（秒）
		int		iAttendTime;	//	参加战场时间（秒）
		int		iKillCount;		//	击杀次数
		int		iDeathCount;	//	死亡次数
		int		iCountryKillCount;	//	同国家击杀次数
		int		iCountryDeathCount;	//	同国家死亡次数
		int		iAttackerCount;	//	攻方人数
		int		iDefenderCount; //  守方人数
		
		int		iStrongHoldCount;		//	据点个数
		int		iStrongHoldState[8];	//	据点占领状态 8 == ARRAY_SIZE(COUNTRY_CONFIG::stronghold)

		typedef std::vector<S2C::cmd_countrybattle_live_show_result::
			score_rank_entry> ScoreRankContainer;
		ScoreRankContainer OffenseRanks;
		ScoreRankContainer DefenceRanks;

		typedef std::vector<S2C::cmd_countrybattle_live_show_result::
			death_entry> DeathContainer;
		DeathContainer OffenseDeaths;
		DeathContainer DefenceDeaths;

		void SetCountryBattleLiveShowInfo(const S2C::cmd_countrybattle_live_show_result& cmd);

		// 战车
		int		iChariot; // 战车id
		int		iEnergy;	// 能量
		int		iScoreSelf; //自己成绩
		int		iMultiKill; // 连杀

		bool IsGuildWar()const{ return nType == BT_GUILD; }
		bool IsCountryWar()const{ return nType == BT_COUNTRY; }
		bool IsFlagCarrier() const{ return IsCountryWar() && bFlagCarrier; }
		bool IsChariotWar() const { return nType == BT_CHARIOT;}		
	};

	//	贡献度
	struct CONTRIB_INFO 
	{
		int	consume_contrib;	//	可消费的
		int	exp_contrib;		//	可兑换帮派经验的
		int	cumulate_contrib;	//	累积值

		CONTRIB_INFO()
			: consume_contrib(0)
			, exp_contrib(0)
			, cumulate_contrib(0)
		{
		}
	};

	// 阵营信息
	struct FORCE_INFO
	{
		int force_id;
		int reputation;
		int contribution;

		FORCE_INFO():force_id(0),reputation(0),contribution(0)
		{}
	};

	//	帮主的帮派基地信息
	struct FACTION_FORTRESS_INFO 
	{
		int	faction_id;			//	帮派id
		int	level;				//	基地等级
		int	exp;				//	基地经验
		int	exp_today;			//	当天获得的经验
		int	exp_today_time;		//	当天对应的零点
		int	tech_point;			//	剩余科技点数
		int	technology[5];		//	科技天赋的等级
		int	material[8];		//	各材料剩余数

		struct building_data 
		{
			int	id;				//	基地设施对应id
			int	finish_time;	//	基地设施建造完成时间点
		};
		abase::vector<building_data> building;

		int	health;				//	基地健康度

		FACTION_FORTRESS_INFO()
			: faction_id(0)
			, level(0)
			, exp(0)
			, exp_today(0)
			, exp_today_time(0)
			, tech_point(0)
			, health(0)
		{
			memset(technology, 0, sizeof(technology));
			memset(material, 0, sizeof(material));
		}
	};
	
	//	进入基地信息
	struct FACTION_FORTRESS_ENTER 
	{
		int	faction_id;
		int	role_in_war;	//	0 : 中立或不在基地 1:攻方 2: 守方
		int end_time;

		FACTION_FORTRESS_ENTER()
			: faction_id(0)
			, role_in_war(0)
			, end_time(0)
		{
		}
	};

	//  婚礼场景信息
	struct WEDDING_SCENE_INFO
	{
		int groom;
		int bride;

		WEDDING_SCENE_INFO()
			: groom(0)
			, bride(0)
		{
		}
	};

	//  玩家收益时间信息
	enum
	{
		PROFIT_MAP_NONE,
		PROFIT_MAP_BATTLE,
		PROFIT_MAP_EXIST,
	};
	struct PROFIT_TIME_INFO
	{
		char profit_map;
		int profit_level;
		int profit_time;
	};

	//  摆摊银票自动转换
	struct BOOTH_AUTO_YINPIAO
	{
		bool open;
		CECCounter cnt;
		int low_money;
		int high_money;

		BOOTH_AUTO_YINPIAO()
			: open(false),low_money(50000000), high_money(100000000)
		{
			cnt.SetPeriod(60000);
			cnt.Reset(true);
		}
	};

	//  目标物品界面控制
	struct TARGETITEM_DLG_CTRL
	{
		CECCounter cntCheck;

		TARGETITEM_DLG_CTRL()
		{
			cntCheck.SetPeriod(1000);
			cntCheck.Reset();
		}

		bool NeedShow(int& iSlot);
		void Update(DWORD dwDeltaTime);
	};

	//	服务器控制的额外操作限制
	enum
	{
		PLAYER_LIMIT_NOFLY,							//	禁止"飞行/取消飞行"
		PLAYER_LIMIT_NOCHANGESELECT,	//	禁止"选中/取消选中/协助攻击"
		PLAYER_LIMIT_NOMOUNT,					//	禁止召唤骑宠
		PLAYER_LIMIT_NOBIND,						//	禁止"发起/接收相依相偎"
		PLAYER_LIMIT_MAX,
	};
	struct TITLE
	{
		unsigned short	id;
		int				expire_time;
		TITLE(unsigned short _id, int _expire_time):id(_id),expire_time(_expire_time){}
		bool operator == (const TITLE& rhs) const {return id == rhs.id;}
	};
	typedef std::vector<TITLE> TITLE_CONTAINER;
	struct REINCARNATION_TOME
	{
		int tome_exp;
		char tome_active;   // 1激活0未激活
		int max_level;		// 历史最高等级
		typedef std::vector<S2C::cmd_reincarnation_tome_info::_entry> EACH_RECARNATION;
		EACH_RECARNATION reincarnations;
		static const int max_exp;
		void Clear(){
			tome_exp = 0;
			tome_active = 0;
			max_level = 0;
			reincarnations.clear();
		}
	};
	typedef char COLLECTION_TYPE;
	typedef std::vector<COLLECTION_TYPE> CARD_COLLECTION;
	struct GENERAL_CARD
	{
		int leader_ship;
		CARD_COLLECTION card_collection;
		static const int max_collection;

		GENERAL_CARD():leader_ship(0){}
		bool HasCard(int show_order) const;
		void AddCollection(int show_order);
		void Init(char* collection, unsigned int size);
		int	 GetObtainedCount() const;
	};
	struct CARD_HOLDER
	{
		enum {
			max_holder = 6,
			max_holder_level = 7, // 目前版本只开放到7级
		};
		int level[max_holder];
		int exp[max_holder];
		int gain_times;
		static const int hp[max_holder];
		static const int damage[max_holder];
		static const int magic_damage[max_holder];
		static const int defense[max_holder];
		static const int magic_defense[max_holder];
		static const int vigour[max_holder];
		static float GetProfessionRatio(int index);
		void Init(S2C::cmd_refresh_monsterspirit_level::_entry* entrys, unsigned int size, int gain_times);
	};
	
	struct SkillShortCutConfig	
	{
		int setNum;
		int slotNum;
		int skillId;
	};

	struct SkillGrpShortCutConfig	
	{
		int setNum;
		int slotNum;
		int groupIndex;
	};	

	friend class CECHPWorkStand;
	friend class CECHPWorkTrace;
	friend class CECHPWorkUse;
	friend class CECHPWorkDead;
	friend class CECHPWorkMelee;
	friend class CECHPWorkMove;
	friend class CECHPWorkFollow;
	friend class CECHPWorkFly;
	friend class CECHPWorkFall;
	friend class CECHPWorkSit;
	friend class CECHPWorkSpell;
	friend class CECHPWorkPick;
	friend class CECHPWorkRevive;
	friend class CECHPWorkFMove;
	friend class CECHPWorkPassiveMove;
	friend class CECHPWorkCongregate;
	friend class CECHPWorkSkillStateAction;
	friend class CECHostInputFilter;

	friend class CECHostMove;
	friend class CECHPWorkMan;

	friend class CECTracedObject;
	friend class CECTracedNPC;
	friend class CECTracedPlayer;
	friend class CECTracedMatter;
	friend class CECTracedTaskNPC;

	friend class CDlgQuickAction;

	friend class CECActionCancelRide;
	friend class CECActionFly;
	friend class CECActionLanding;
	friend class CECActionRide;
	friend class CECActionSkill;
	friend class CECActionUseSkillItem;
	friend class CECActionSwitcher;

	friend class CECNavigateCtrl;

public:		//	Constructor and Destructor

	CECHostPlayer(CECPlayerMan* pPlayerMan);
	virtual ~CECHostPlayer();

public:		//	Attributes

public:		//	Operations

	//	Initlaize object
	bool Init(const S2C::cmd_self_info_1& Info);
	//	Release object
	virtual void Release();

	//	When all initial data is ready, this function is called
	void OnAllInitDataReady();

	//	Tick routine
	virtual bool Tick(DWORD dwDeltaTime);
	void TickDiscardedFrame(DWORD dwDeltaTime);
	//	Render routine
	virtual bool Render(CECViewport* pViewport, int iRenderFlag=0);
	//	Render when player is opening booth
	virtual bool RenderForBooth(CECViewport* pViewport, int iRenderFlag=0);
	//	Render for UI
	virtual bool RenderForUI(CECViewport * pViewport);

	//	Update camera in following mode
	void UpdateFollowCamera(bool bRunning, DWORD dwTime);

	//	Process message
	virtual bool ProcessMessage(const ECMSG& Msg);

	//	Set absolute position
	virtual void SetPos(const A3DVECTOR3& vPos);
	//	Player was killed
	virtual void Killed(int idKiller);
	//	Get off pet
	virtual void GetOffPet(bool bResetData = true);

	virtual int GetEquippedItem(int index)const;
	//	Get number of equipped items of specified suite
	virtual int GetEquippedSuiteItem(int idSuite, int* aItems=NULL);

	//	Do an emote action
	virtual bool DoEmote(int idEmote);

	//	Get camera coordinates
	A3DCoordinate* GetCameraCoord() { return &m_CameraCoord; }
	//	Get cameractrl
	CECCamera* GetCameraCtrl() { return &m_CameraCtrl; }
	//	Save configs data (shortcut, etc.) to specified buffer
	bool SaveConfigData(void* pDataBuf, int* piSize);
	//	Load configs data (shortcut, etc.) from specified buffer
	bool LoadConfigData(const void* pDataBuf);

	//	Revive
	void Revive(const S2C::cmd_player_revive* pCmd);
	//	Begin following the selected target
	void BeginFollow(int idTarget);
	//	Level up
	void LevelUp();
	//  Change invisible
	void ChangeInvisible(int iDegree);
	//	Hatch pet
	bool HatchPet(int iIvtrIdx);
	//	Restore pet
	bool RestorePet(int iPetIdx);
	//	Summon pet
	bool SummonPet(int iPetIdx);
	//	Recall pet
	bool RecallPet();
	//	Banish pet
	bool BanishPet(int iPetIdx);
	//	Is host operating pet ?
	int IsOperatingPet();
	//	Get pet operation time counter
	CECCounter& GetPetOptTime() { return m_PetOptCnt; }

	//	Prepare to begin NPC service
	void PrepareNPCService(int idSev);
	//	End NPC service
	void EndNPCService();
	//	Buy items from NPC
	void BuyItemsFromNPC();
	//	Sell items to NPC
	void SellItemsToNPC();
	//	Buy items from else player's booth
	void BuyItemsFromBooth(int iYinpiao = 0);
	//	Sell items to else player's booth
	void SellItemsToBooth();
	//	When the booth we are visiting closed
	void OnOtherBoothClosed();
	//	Open booth
	void OpenBooth(const ACHAR* szName);
	//	Team invite
	void TeamInvite(const GNET::PlayerBriefInfo& Info);

	//	Commands ...
	bool CmdSitDown(bool bSitDown);		//	Normal commands...
	bool CmdWalkRun(bool bRun);
	bool CmdNormalAttack(bool bMoreClose=false, bool bCombo=false, int idTarget=0, int iForceAtk=-1);
	bool CmdFindTarget();
	bool CmdAssistAttack();
	bool CmdFly(bool bForceFly = true);
	bool CmdPickup();
	bool CmdGather();
	bool CmdRushFly();
	bool CmdBindBuddy(int idTarget);
	bool CmdViewOtherEquips(int idTarget);
	bool CmdViewOtherInfo(int idTarget);
	bool CmdAskDuel(int idTarget);
	bool CmdInviteToTeam();				//	Team commands...
	bool CmdLeaveTeam();
	bool CmdKickTeamMember();
	bool CmdFindTeam();
	bool CmdStartTrade();				//	Trade commands...
	bool CmdSellBooth();
	bool CmdBuyBooth();
	bool CmdStartPose(int iPose);		//	Pose commands...
	bool CmdInviteToFaction();			//	Faction commands...

	bool SelectTarget(int idTarget);
	bool CanAcceptBind()const;
	bool CanSelectTarget(int idTarget);
	static float SafelySelectDistance();
	static bool CanSafelySelectWith(float fDistanceToHostPlayer);
	bool CanSafelySelect(CECElsePlayer *pElsePlayer);
	bool CanSafelySelect(CECNPC *pNPC);

	//	Get inventories interface
	CECInventory* GetPack(int iPack);
	CECInventory* GetPack() { return m_pPack; }
	CECInventory* GetEquipment() { return m_pEquipPack; }
	CECInventory* GetTrashBox() { return m_pTrashBoxPack; }
	CECInventory* GetTrashBox2() { return m_pTrashBoxPack2; }
	CECInventory* GetTrashBox3() { return m_pTrashBoxPack3; }
	CECInventory* GetAccountBox() { return m_pAccountBoxPack; }
	CECInventory* GetGeneralCardBox() { return m_pGeneralCardPack; }
	CECInventory* GetTaskPack() { return m_pTaskPack; }
	CECDealInventory* GetDealPack() { return m_pDealPack; }
	CECDealInventory* GetEPDealPack() { return m_pEPDealPack; }
	CECDealInventory* GetBuyPack() { return m_pBuyPack; }
	CECDealInventory* GetSellPack() { return m_pSellPack; }
	CECNPCInventory* GetNPCSevPack(int n) { ASSERT(n >= 0 && n < NUM_NPCIVTR); return m_aNPCPacks[n]; }
	CECDealInventory* GetBoothSellPack() { return m_pBoothSPack; }
	CECDealInventory* GetBoothBuyPack() { return m_pBoothBPack; }
	CECDealInventory* GetEPBoothSellPack() { return m_pEPBoothSPack; }
	CECDealInventory* GetEPBoothBuyPack() { return m_pEPBoothBPack; }
	CECInventory* GetEPEquipPack() { return m_pEPEquipPack; }
	int GetTrashBoxMoneyCnt() const { return m_iTrashBoxMoneyCnt; }
	int GetAccountBoxMoneyCnt() const { return m_iAccountBoxMoneyCnt; }
	//	Get detail data of host's specified item
	void GetIvtrItemDetailData(int iPack, int iSlot);
	//	Check whether player can use specified equipment
	bool CanUseEquipment(CECIvtrEquip* pEquip, int* piReason=NULL);
	//	Check whether player rcan use specified projectile
	bool CanUseProjectile(CECIvtrArrow* pArrow);
	//	Check whether player can transfer cash
	bool CanTransferCash();
	//	Check whether player has enough ammo
	float CheckAmmoAmount();
	//	Use specified inventory item in pack
	bool UseItemInPack(int iPack, int iSlot, bool showMsg=true);
	bool UseEquipment(int iSlot);
	//	Freeze / Unfreeze specified item
	bool FreezeItem(int iIvtr, int iIndex, bool bFreeze, bool bFreezeByNet);
	//	Check whether player can take specified matter
	bool CanTakeItem(int idItem, int iAmount);

	// Sort pack items
	class PackSorter{
		CECInventory *m_pInventory;
	public:
		PackSorter();
		virtual ~PackSorter(){}
		void SetInventory(CECInventory *pInventory);
		CECIvtrItem *GetItem(int index)const;

		// Return whether index1 should be put before index2
		virtual bool operator()(int index1, int index2)const=0;
	};
	void SortPack(int iPack, PackSorter *pPackSorter = NULL);
	//	验证 Pack 中物品指针引用的有效性
	void ValidatePackItemPointer();
		
	//	Get shortsets
	CECShortcutSet* GetShortcutSet1(int n) { ASSERT(n >= 0 && n < NUM_HOSTSCSETS1); return m_aSCSets1[n]; }
	CECShortcutSet* GetShortcutSet2(int n) { ASSERT(n >= 0 && n < NUM_HOSTSCSETS2); return m_aSCSets2[n]; }

	CECShortcutSet* GetSysModShortcutSet(int n) { if(n >= 0 && n < NUM_SYSMODSETS) return m_aSCSetSysMod[n]; else return NULL;}
	CECShortcutSet* GetCurSysModShortcutSet() { return GetSysModShortcutSet(m_iCurSysModIndex);}
	int				GetCurSysModShortcutSetIndex() const { return m_iCurSysModIndex;}
	void			SetCurSysModShortcutSetIndex(int index);

	//	Apply shortcut of specified inventory item
	bool ApplyItemShortcut(int iPack, int iSlot) { return UseItemInPack(iPack, iSlot); }
	//	Apply shortcut of specified skill
	bool ApplySkillShortcut(int idSkill, bool bCombo=false, int idSelTarget=0, int iForceAtk=-1);
	//	Return to a target town through skill
	bool ReturnToTargetTown(int idTarget, bool bCombo=false);
	//	Summon a player through skill (id == 0 means check the skill condition)
	bool SummonPlayer(int idTarget, bool bCombo=false);
	//	Pickup an object
	bool PickupObject(int idTarget, bool bGather);

	//  Add to goblin skill list
	bool AddGoblinSkill(CECSkill* pSkill);

	//	Get positive skill number
	int GetPositiveSkillNum() { return m_aPtSkills.GetSize(); }
	//	Get positive skill by index
	CECSkill* GetPositiveSkillByIndex(int n) { return m_aPtSkills[n]; }
	//	Get positive skill by id
	CECSkill* GetPositiveSkillByID(int id, bool bSenior=false);
	//	Get passive skill number
	int GetPassiveSkillNum() { return m_aPsSkills.GetSize(); }
	//	Get passive skill by index
	CECSkill* GetPassiveSkillByIndex(int n) { return m_aPsSkills[n]; }
	//	Get passive skill by id
	CECSkill* GetPassiveSkillByID(int id, bool bSenior=false);

	//	技能装备
	int GetEquipSkillNum(){ return m_aEquipSkills.GetSize(); }
	CECSkill *GetEquipSkillByIndex(int n){ return m_aEquipSkills[n]; }
	CECSkill *GetEquipSkillByID(int id);

	bool	UpdateEquipSkills(bool bShowSkillChange=false);	//	更新技能装备带来的技能增减
	bool  IsEquipSkill(CECSkill *pSkill);									//	判断技能是否为当前装备技能
	void	UpdateEquipSkillCoolDown(int cooldown_index=-1);//	更新装备技能冷却

	//  Get goblin skill by id
	CECSkill* GetGoblinSkillByID(int id);

	//	Get skill by id
	CECSkill* GetNormalSkill(int id, bool bSenior=false);
	//	Check whether host can learn specified skill
	int CheckSkillLearnCondition(int idSkill, bool bCheckBook);
	//	Check whether host can cast specified skill
	int CheckSkillCastCondition(CECSkill* pSkill);
	//	Check whether pet learn specified skill
	int CheckPetSkillLearnCondition(int idSkill, bool bCheckBook);
	//	Process the condition message
	bool ProcessSkillCondition(int iCon);

	//	Get prepare skill
	CECSkill* GetPrepSkill() { return m_pPrepSkill; }
	//	Get skill ability
	int GetSkillAbility(int idSkill);
	//	Get skill ability percent
	int GetSkillAbilityPercent(int idSkill);
	//	Remove skill
	void RemoveNormalSkill(int idSkill);
	void RemoveEquipSkill(int idSkill);
	void RemoveSkillReference(int idSkill);
	//	Apply combo skill
	bool ApplyComboSkill(int iGroup, bool bIgnoreAtkLoop = false, int iForceAtk = -1);
	//	Clear combo skill
	void ClearComboSkill();
	//  Get current combo skill
	CECComboSkill* GetComboSkill() { return m_pComboSkill; }
	//	Replace specified skill with it's senior skill
	void ReplaceJuniorSkill(CECSkill* pSeniorSkill);

	void SaveSkillShortcut(std::vector<SkillShortCutConfig>& skillSCConfigArray, CECShortcutSet** aSCSets, int count);
	void SaveSkillGrpShortcut(std::vector<SkillGrpShortCutConfig>& skillGrpSCConfigArray, CECShortcutSet** aSCSets, int count);
	void ConvertSkillShortcut(std::vector<SkillShortCutConfig> & skillSCConfigArray);	
	void ValidateSkillGrpShortcut(std::vector<SkillGrpShortCutConfig> & skillGrpSCConfigArray);
	void AssignSkillShortcut(const std::vector<SkillShortCutConfig> & skillSCConfigArray, CECShortcutSet** aSCSets);
	void AssignSkillGrpShortcut(const std::vector<SkillGrpShortCutConfig> & skillGrpSCConfigArray, CECShortcutSet** aSCSets);
	void ConvertComboSkill();
	

	//	Is host player trading ?
	bool IsTrading() { return m_idTradePlayer ? true : false; }
	//  Get the player's id who is trading with me
	int GetTradePlayer() const { return m_idTradePlayer; }
	//	Is host player open trash box ?
	bool IsUsingTrashBox() const { return m_bUsingTrashBox; }
	//	Is host in root state
	bool IsRooting() { return (m_dwLIES & (LIES_ROOT | LIES_SLEEP | LIES_STUN)) ? true : false; }
	//	Can player attack ?
	bool CannotAttack() { return (m_dwLIES & LIES_DISABLEFIGHT) ? true : false; }
	//	Is host in melee state ?
	bool IsMeleeing() { return m_bMelee; }
	//	Is host player talking with NPC ?
	bool IsTalkingWithNPC() { return m_bTalkWithNPC; }
	//	Is spelling magic
	bool IsSpellingMagic();
	//	Is spelling durative skill ?
	bool IsSpellingDurativeSkill() { return IsSpellingMagic() && m_bSpellDSkill; }
	//	Is flash moving ?
	bool IsFlashMoving();
	//	Is taking off ?
	bool IsTakingOff();
	//	Is jumping
	bool IsJumping() const { return m_iJumpCount > 0; }
	bool IsJumpInWater() const { return m_bJumpInWater; }
	void SetJumpInWater(bool b) { m_bJumpInWater = b; }
	void ResetJump() { m_iJumpCount = 0; m_bJumpInWater = false; }
	//	Is free falling
	bool IsFalling() { return (m_iMoveEnv == MOVEENV_GROUND && !m_GndInfo.bOnGround); }
	//	Is picking up something
	bool IsPicking();
	//	Is gathering resources
	bool IsGathering();
	//	Is reviving
	bool IsReviving();
	//	Is using item ?
	bool IsUsingItem();

	void StartChangeFace();
	void StopChangeFace();
	bool IsChangingFace() { return m_bChangingFace; }
	
	//  Is auto moving ?
	bool IsAutoMoving();

	//	Is doing a passive movement ?
	bool IsPassiveMove();

	//  Is following someone ?
	bool IsFollowing();
	//  Is replying the congregate? (return congregation type + 1)
	int IsCongregating();
	CECCounter GetCongregateCnt();

	//	Is doing session pose ?
	bool DoingSessionPose();
	//	Is adjust orient
	bool IsAdjustOrient() { return m_bAdjustOrient; }
	void SetAdjustOrient(bool bAdjust) { m_bAdjustOrient = bAdjust; }
	//	Can left button turn camera
	bool LeftButtonTurnCamera();
	//	Does trash box have password ?
	bool TrashBoxHasPsw() { return m_bTrashPsw; }
	//	Decide target name color basing on target level
	DWORD DecideTargetNameCol(int iTargetLevel);
	//	Is all initial data ready ?
	bool HostIsReady() { return m_bEnterGame; }
	//	Is player moving
	virtual bool IsPlayerMoving();
	virtual bool IsWorkMoveRunning()const;
	virtual bool IsWorkSpellRunning()const;
	//	Is under water
	bool IsUnderWater() { return m_iMoveEnv == MOVEENV_WATER ? true : false; }
	//	Can jump or take off in water ?
	bool CanTakeOffWater();
	//	按服务器方法计算游泳速度，避免在水面加速游泳时被拉回
	float GetSwimSpeedSev();
	//	Get breath data
	const BREATHDATA& GetBreathData() { return m_BreathData; }
	//	Get the exp will lost if revived by other player
	float GetReviveLostExp() { return m_fReviveExp; }
	//	Get deadly strike rate
	int GetDeadlyStrikeRate();
	//  Get soul power
	int GetSoulPower();
	//	Is host in sliding state (in the state, host is sliding on slope) ?
	bool InSlidingState();
	//	Get left pariah time
	DWORD GetPariahTime() { return m_dwPariahTime; }
	//	Get battle info.
	BATTLEINFO& GetBattleInfo() { return m_BattleInfo; }
	//	Attackable judge
	int AttackableJudge(int idTarget, bool bForceAttack);

	//	On start binding buddy
	virtual void OnStartBinding(int idMule, int idRider);

	//	Get cool time
	int GetCoolTime(int iIndex, int* piMax=NULL);
	bool GetSkillCoolTime(int idSkill, COOLTIME &ct)const;
	//	Get incant counter
	CECCounter GetIncantCnt() { return m_IncantCnt; }
	//	Get gather counter
	CECCounter GetGatherCnt() { return m_GatherCnt; }
	//	Get ext instance left time
	int GetTimeToExitInstance()	{ return m_nTimeToExitInstance; }
	//	Get bind command cool time counter
	CECCounter& GetBindCmdCoolCnt() { return m_BindCmdCoolCnt; }
	//	Get time counter of using item in pack
	bool GetUsingItemTimeCnt(DWORD& dwCurTime, DWORD& dwMaxTime, int* piItem=NULL);
	//	Get auto fashion counter
	CECCounter& GetAutoFashionCnt() { return m_AutoFashionCnt; }
	//	Get right button clicked player
	int	GetClickedMan() { return m_idClickedMan; }
	//	Get current service NPC
	int GetCurServiceNPC() { return m_idSevNPC; }
	//	Get object under cursor
	int GetCursorHoverObject() { return m_idCurHover; }

	//	Get active waypoint number
	int GetWayPointNum() { return m_aWayPoints.GetSize(); }
	//	Get way point
	WORD GetWayPoint(int n) { return m_aWayPoints[n]; } 
	//	Does host player have specified way point ?
	bool HasWayPoint(WORD wID);

	//  Get favorite auction items number
	int GetFavorAucItemNum()	{ return m_aFavorAucItems.GetSize(); }
	//  Get favorite auction item
	int GetFavorAucItem(int index) { ASSERT(index >=0 && index < GetFavorAucItemNum()); return m_aFavorAucItems[index]; }
	//  Add to favorite auction item list
	bool AddToFavorAucItem(int id);
	//  Remove auction item
	bool RemoveFavorAucItem(int item);
	//  Remove all auction items
	void RemoveAllFavorAucItems();

	//  Save favorite auction list to local disk
	bool SaveFavorAucItems();
	//  Load favorite auction list from local disk
	bool LoadFavorAucItems();

	//	Get task interface
	CECTaskInterface* GetTaskInterface() { return m_pTaskInterface; }
	//	Get Portrait
	A2DSprite* GetPortraitSprite() { return m_pSpritePortrait; }
	//	Get friend manager
	CECFriendMan* GetFriendMan() { return m_pFriendMan; }
	//	Get move control
	CECHostMove& GetMoveControl() { return m_MoveCtrl; }
	//	Set player manager
	void SetPlayerMan(CECPlayerMan* pPlayerMan) { m_pPlayerMan = pPlayerMan; }
	//	Get pet corral object
	CECPetCorral* GetPetCorral() { return m_pPetCorral; }

	// Host faction info
	const ACHAR* GetNickname() const { return m_strNickname; } 	//	Get nickname
	void SetNickname(const void* p, int len); 		//	Set nickname

	const FACTION_FORTRESS_CONFIG * GetFactionFortressConfig();
	
	const FACTION_FORTRESS_INFO * GetFactionFortressInfo();
	const CONTRIB_INFO * GetContribInfo();
	void SetFactionFortressBriefInfo(const void *pBriefInfo);
	const FACTION_FORTRESS_ENTER * GetFactionFortressEnter();
	bool IsInFortress() { return m_fortressEnter.role_in_war!=0; }
	bool IsInMyFortress(){ return IsInFortress() && m_fortressEnter.faction_id && m_fortressEnter.faction_id == GetFactionID(); }
	bool IsInFortressWar();
	
	const FACTION_BUILDING_ESSENCE * GetBuildingEssence(int tid);

	int GetCash() const { return m_nCash; }
	int GetCashMoneyRate() const { return m_nCashMoneyRate; }	// 当前元宝与金币的兑换率
	bool IsCashMoneyOpen() const { return m_bCashMoneyOpen; }	// 是否能商城买金
	void SetCash(int nCash) { m_nCash = nCash; }
	int GetCurEmotionSet() const { return m_nCurEmotionSet; }

	int GetDividend() const { return m_nDividend; }
	void SetDividend(int nDividend) { m_nDividend = nDividend; m_bDividentInitialized = true;}
	bool IsDividentInitialized(){ return m_bDividentInitialized; }

	bool NeedPassword();
	
	bool CheckCanDoFly() { return CanDo(CANDO_FLY); }

	// Get key object(NPC..) coordinates
	A3DVECTOR3 GetObjectCoordinates(int idTarget, CECGame::ObjectCoords & TargetCoord, bool& bInTable);

	//  Get Yinpiao amount
	int GetYinpiaoTotalAmount();
	
	//  Update booth pack slot number
	void UpdateBoothPack(int iSellSize, int iBuySize);

	//  Get auto fashion flag
	bool GetAutoFashion()	{ return m_bAutoFashion; }

	//	Start auto fashion
	int StartAutoFashion(int iTime);
	void StopAutoFashion()	{ m_bAutoFashion = false; }
	
	//  Check auto fashion condition, return 0 means successful
	int CheckAutoFashionCondition();


	//  Get auto fashion time: mintues
	int GetAutoFashionTime()	{ return m_AutoFashionCnt.GetPeriod()/60000; }

	//  Get fashion by id & color
	CECIvtrFashion* GetFashionByID(int tid, int color, int& index);
	CECIvtrFashion* GetFashionByID(int id, int& index);	// id: high byte is tid, low byte is color

	//  Get fashion shortcut sets by index
	int	GetFashionSCSets(int index) { ASSERT(index>=0 && index <SIZE_FASHIONSCSET); return m_aFashionSCSets[index]; }
	void SetFashionSCSets(int index, int value) { ASSERT(index>=0 && index <SIZE_FASHIONSCSET); m_aFashionSCSets[index] = value; }
	void SetFashionSCSets(int index, CECIvtrFashion* pFashion);

	//  Get current fashion suit id
	int GetCurFashionSuitID()	{ return m_iCurFashionSuitID; }
	void SetCurFashionSuitID(int id) { m_iCurFashionSuitID = id; }

	//  Get max fashion suit number
	int GetMaxFashionSuitNum();

	//  Equip fashion by suit id
	bool EquipFashionBySuitID(int id);
	
	//
	void ResetAutoSelMap();
	//	Auto select a attackable target
	int AutoSelectTarget();
	//	Remove player from tab-selected array
	void RemoveObjectFromTabSels(CECObject* pObject);

	//  Get role create time
	int GetRoleCreateTime() { return m_iRoleCreateTime; }
	//  Get role last login time
	int GetRoleLastLoginTime() { return m_iRoleLastLoginTime; }
	//  Get account total cash
	int GetAccountTotalCash() { return m_iAccountTotalCash; }

	// Multi exp
	MultiExpState MultiExp_GetState(){ return m_multiExpState; }
	bool  MultiExp_Inited();
	bool  MultiExp_CanChooseNow();
	float MultiExp_GetFactor() { return m_multiExpFactor; }
	int	  MultiExp_GetEnhanceTime();
	int   MultiExp_GetBufferTime();
	int   MultiExp_GetImpairTime();
	int	  MultiExp_GetCurStateTimeLeft();
	int	  MultiExp_GetBeginTimesLeft(){ return m_multiExpBeginTimesLeft; }
	bool  MultiExp_StateEmpty();
	bool  MultiExp_EnhanceStateEmpty();
	bool  MultiExp_EnhanceStateOnly();

	// omit blocking method if this id belongs to player's friend/faction/team
	bool IsOmitBlocking(int roleid);

	bool  OnPetSays(int tid, int nid, int type);
	
	CECGuildDiplomacyReminder & GetGuildDiplomacyReminder() { return m_DiplomacyReminder; }

	const FORCE_INFO* GetForceInfo(int force_id);
	void SetForceInfo(int force_id, const FORCE_INFO* pInfo); // pInfo == NULL means delete
	CECForceMgr* GetForceMgr() { return m_pForceMgr; }

	bool		IsFactionMember(int idTargetFaction);
	bool		IsFactionAllianceMember(int idTargetFaction);

	const WEDDING_SCENE_INFO * GetWeddingSceneInfo() {return &m_weddingSceneInfo;}

	const PROFIT_TIME_INFO* GetProfitTimeInfo() { return &m_profitInfo; }

	COnlineAwardCtrl* GetOnlineAwardCtrl() { return m_pOnlineAwardCtrl;}

	void SetPVPNoPenalty(bool bState) { m_bPVPNoPenaltyState = bState;}
	bool IsPVPNoPenalty() const { return m_bPVPNoPenaltyState;}

	bool IsInCountryWar(){ return IsInBattle() && GetBattleInfo().IsCountryWar(); }
	bool HasCountryWarFlagCarrier();
	A3DVECTOR3 GetCountryWarFlagCarrierPos();
	bool HasCountryWarChannel();
	bool HasCountryChannel();

	// 国王统帅分
	int GetKingPoint() const { return m_iKingScore; }
	void SetKingPoint(int iKingScore) { m_iKingScore = iKingScore; }

	bool Goto(int idInst, A3DVECTOR3 vPos, int iParallelWorldID);
	bool CanMove(){ return CanDo(CANDO_MOVETO); }
	bool CanSwitchParallelWorld() { return CanDo(CANDO_SWITCH_PARALLEL_WORLD); }

	//	战略图场景(143)内、Win_CountryMap 显示时位置变动不即时生效
	//	可保证 Win_CountryMap 上各领土间流畅移动、
	//	以及避免玩家由于频繁加载场景资源而无法及时响应 CountryBattlePreEnterNotify，从而有机会无法进入国战战场副本(144)
	//	此时服务器玩家位置发生变动，但客户端延迟更新位置，双方位置不同步。
	//  而收到cmd_notify_hostpos的同时，会收到新位置周围NPC，PLAYER, MATTER列表信息。
	//  客户端HOSTPLAYER仍位于老位置，TICK的时候，上述三个MANAGER会依据距离将NPC等删除，这里加一个标记，避免上述情况。
	bool IsDeferedUpdateSlice() const;

	bool GetDefenseRuneEnable(int type)const;

	void ExchangeYinpiao();
	void GetAutoYinpiaoMoney(int& lowMoney, int& highMoney)
	{
		lowMoney = m_AutoYinpiao.low_money;
		highMoney = m_AutoYinpiao.high_money;
	}
	void SetAutoYinpiaoMoney(int lowMoney, int highMoney)
	{
		m_AutoYinpiao.low_money = lowMoney;
		m_AutoYinpiao.high_money = highMoney;
	}

	// 国王服务，解决安全锁问题
	// 安全锁内打开国王服务，服务器回cmd_error_msg, 打不开对应的操作界面，导致玩家一直处于NPC服务状态
	bool IsInKingService() const { return m_bIsInKingService; }
	void SetInKingService(bool bInKingService) { m_bIsInKingService = bInKingService; }
	
	bool RebuildPet(int iPetIdx, int iSelItem, bool bNature);
	bool IsRebuildingPet() const { return m_bInRebuildPet;}
	void SetRebuildPetFlag(bool bDo) { m_bInRebuildPet = bDo;}
	__int64 GetTouchPoint(){return m_iTouchPoint;}
	void SetTouchPoint(__int64 iPoint){m_iTouchPoint = iPoint;}

	// 最近关系密切的玩家
	void AddRelatedPlayer(int cid);
	bool IsRelatedPlayer(int cid) const;

	CECAutoTeam* GetAutoTeam() { return m_pAutoTeam; }

	COfflineShopCtrl* GetOfflineShopCtrl() { return m_pOffShopCtrl;}
	void ModifyTitle(unsigned short id, int expire_time, bool bAdd);
	void InitTitle(int count, unsigned short* id);
	void InitTitlePlus(int count, void* id_time_pair);
	const TITLE_CONTAINER& GetTitles(){return m_Titles;}
	bool IsTitleDataReady() {return m_bTitleDataReady;}
	int GetSignInMonthCalendar() const {return m_iSignInMonthCalendar;}
	int GetSignInYearStat() const {return m_iSignInThisYearStat;}
	int GetSignInLastYearStat() const {return m_iSignInLastYearStat;}
	int GetSignInAwardedCountThisMonth() const {return m_iSignInAwardedCountThisMonth;}
	int GetSignInLateCountThisMonth() const {return m_iSignLateCountThisMonth;}
	const REINCARNATION_TOME& GetReincarnationTome() {return m_ReincarnationTome;}
	int GetMaxLevelSofar() { return a_Max(m_ReincarnationTome.max_level, m_BasicProps.iLevel);}
	bool IsLevelMeetMeridianReq(int& minLevelToImpact)
	{
		int iLevel = GetMaxLevelSofar();
		minLevelToImpact = GetMeridiansProp().level * 2 + 40;
		return iLevel >= 100 || iLevel >= minLevelToImpact;
	}
	// 需要经脉提示
	bool NeedMeridiansGuide();
	const PLAYER_REINCARNATION_CONFIG* GetReincarnationConfig();
	const PLAYER_REALM_CONFIG* GetRealmConfig();
	int GetRealmExp() { return m_iRealmExp; }
	void SetRealmExp(int exp) { m_iRealmExp = exp; }
	bool IsRealmExpFull();

	CChariot* GetChariot() { return m_pChariot;}
	void UpdatePositiveSkillByChariot(int old_chariot_id,int new_chariot_id = 0);
	bool IsGatheringMonsterSpirit(){ return m_iMonsterSpiritMineID != 0; }
	
	const GENERAL_CARD& GetGeneralCardData() { return m_GeneralCard; }
	const CHARRACTER_CLASS_CONFIG* GetCharacterClassConfig();
	const PLAYER_SPIRIT_CONFIG* GetPlayerSpiritConfig(int type);
	const CARD_HOLDER& GetCardHolder() { return m_CardHolder; }
	int GetUsedLeaderShip();
	bool CanEquipCard(CECIvtrEquip* pEquip);

	CECActionSwitcherBase* GetActionSwitcher() { return m_pActionSwitcher;}
	CECHPWorkMan* GetWorkMan() { return m_pWorkMan;}
	void OnJumpInstance();
	void UpdateActionSwitcher(DWORD dwDeltaTime);
	bool PlayerLimitTest(int flag) ;
	int GetWorldContribution() { return m_iWorldContribution; }
	int GetWorldContributionSpend() { return m_iWorldContributionSpend; }
	bool HaveHealthStones();
	
	CECHostNavigatePlayer* GetNavigatePlayer() { return m_pNavigatePlayer;}
	bool IsInForceNavigateState();									// 强制移动状态
	void OnNaviageEvent(int task,int e);							// 强制移动事件响应
	CECHostNavigatePlayer* CreateNavigatePlayer();
	void ReleaseNavigatePlayer();

	float GetPrayDistancePlus()const{ return m_fPrayDistancePlus; }
	void SetPrayDistancePlus(float prayDistancePlus);

	DWORD GetReviveTime()const{ return m_dwReviveTime; }

protected:	//	Attributes

	float			m_fCameraPitch;		//	Camera pitch
	float			m_fCameraYaw;		//	Camera yaw
	A3DCoordinate	m_CameraCoord;		//	Camera coordinates
	CECCamera		m_CameraCtrl;		//	Camera control
	CECHostMove		m_MoveCtrl;			//	Move control
	int				m_iTurnCammera;		//	0x01: left button turning camera's direction; 0x02 right button
	DWORD			m_dwMoveRelDir;		//	Move relative direction flags
	float			m_fFlyHeight;		//	Fly height
	bool			m_bBeRoot;			//	Host is in root state
	bool			m_bMelee;			//	Host is in melee state
	bool			m_bTrashPsw;		//	true, trash box has password
	DWORD			m_dwLIES;			//	Logic-influence extend states
	float			m_fTransparent;		//	Transparent of host model
	bool			m_bEnterGame;		//	true, all data ready, can play game now
	bool			m_bPrepareFight;	//	true, prepare to fight
	float			m_fReviveExp;		//	The exp will lost if revived by other player. < 0: invalid exp

	CECCounter		m_TaskCounter;		//	Task time counter
	CECCounter		m_TrickCnt;			//	Trick action time counter
	CECCounter		m_IncantCnt;		//	Incant counter
	CECCounter		m_GatherCnt;		//	Gather counter
	CECCounter		m_TLPosCnt;			//	Team leader position counter
	CECCounter		m_TMPosCnt;			//	Team member position counter


	COOLTIME		m_aCoolTimes[GP_CT_MAX];	//	Cool times
	typedef abase::hash_map<int, COOLTIME> SkillCoolTime;
	SkillCoolTime		m_skillCoolTime;

	int				m_nTimeToExitInstance;		//	Exit current instance time left
	CECCounter		m_BindCmdCoolCnt;	//	Bind command cool time counter
	int				m_iGetFriendCnt;	//	Time counter used to get friend list
	DWORD			m_dwPariahTime;		//	Left pariah time
	CECCounter		m_PetOptCnt;		//	Pet operation time counter

	CECTimeSafeChecker	m_selectTargetUpdateTimer;	//	定期检查 m_idSelTarget 的有效性
	int				m_idUCSelTarget;	//	Uncertificately selected object's ID
	int				m_idClickedMan;		//	ID of the right button clicked player
	int				m_idSevNPC;			//	Current service NPC
	int				m_idTradePlayer;	//	ID of player who is trading with us
	int				m_idCurHover;		//	ID of object under cursor
	bool			m_bUsingTrashBox;	//	Whether being using trash box
	bool			m_bUsingAccountBox;	//  Whether being using account box
	bool			m_bTalkWithNPC;		//	true, is talking with NPC
	bool			m_bChangingFace;	//	true, host is changing face
	bool			m_bSpellDSkill;		//	true, is spelling durative skill	
	bool			m_bAutoFashion;		//	true, auto fashion is open, default is false
	CECCounter		m_AutoFashionCnt;	//  Auto fashion counter

	A3DVECTOR3		m_vVelocity;			//	Velocity
	A3DVECTOR3		m_vAccel;				//	Accelerate
	GNDINFO			m_GndInfo;
	int				m_iOldWalkMode;			//	Copy of work mode
	CDR_INFO		m_CDRInfo;
	ON_AIR_CDR_INFO	m_AirCDRInfo;
	int				m_iJumpCount;
	bool			m_bJumpInWater;
	float			m_fVertSpeed;
	BREATHDATA		m_BreathData;			//	Breath data
	BATTLEINFO		m_BattleInfo;

	int				m_iTrashBoxMoneyCnt;
	int				m_iAccountBoxMoneyCnt;
	bool			m_bFirstTBOpen;			//	true, it's the first time trash box is opened
	bool			m_bFirstAccountBoxOpen;	//  true, it's the first time account box is opened
	bool			m_bFirstFashionOpen;	//	true, it's the first time fashion box is opened

	int				m_idBoothTrade;			//	Booth trade id

	BOOTH_AUTO_YINPIAO	m_AutoYinpiao;
	TARGETITEM_DLG_CTRL	m_TargetItemDlgCtrl;

	CECInventory*		m_pPack;			//	Normal package
	CECInventory*		m_pEquipPack;		//	Equipment package
	CECInventory*		m_pTaskPack;		//	Task package
	CECDealInventory*	m_pDealPack;		//	Deal package
	CECDealInventory*	m_pEPDealPack;		//	Else player deal package
	CECDealInventory*	m_pBuyPack;			//	Buy pack 
	CECDealInventory*	m_pSellPack;		//	Sell pack
	CECNPCInventory*	m_aNPCPacks[NUM_NPCIVTR];		//	NPC packages
	CECInventory*		m_pTrashBoxPack;
	CECInventory*		m_pTrashBoxPack2;
	CECInventory*		m_pTrashBoxPack3;
	CECInventory*		m_pAccountBoxPack;	//  User Account box package
	CECInventory*		m_pGeneralCardPack; //  卡牌包裹
	CECInventory*		m_pClientGenCardPack; //  客户端卡牌包裹。服务器没有
	CECDealInventory*	m_pBoothSPack;		//	Booth pack for selling
	CECDealInventory*	m_pBoothBPack;		//	Booth pack for buying
	CECDealInventory*	m_pEPBoothSPack;	//	Else player's booth pack for selling
	CECDealInventory*	m_pEPBoothBPack;	//	Else player's booth pack for buying
	CECInventory*		m_pEPEquipPack;		//	Else player's equipment package

	CECShortcutSet*		m_aSCSets1[NUM_HOSTSCSETS1];	//	Shortcut sets 1
	CECShortcutSet*		m_aSCSets2[NUM_HOSTSCSETS2];	//	Shortcut sets 2

	CECShortcutSet*		m_aSCSetSysMod[NUM_SYSMODSETS]; // 系统模块快捷栏
	int					m_iCurSysModIndex;
	
	int					m_aFashionSCSets[SIZE_FASHIONSCSET];	// Fashion shortcut sets
	int					m_iCurFashionSuitID;	//  Current fashion suit id

	CECHPWorkMan*		m_pWorkMan;			//	Host work manager
	int					m_iDelayWork;		//	Delayed work
	CECTaskInterface*	m_pTaskInterface;	//	Task interface
	CECSkill*			m_pPrepSkill;		//	The skill prepare to be spelled
	CECSkill*			m_pTargetItemSkill;	//  Target item skill
	CECFriendMan*		m_pFriendMan;		//	Friend manager
	CECPetCorral*		m_pPetCorral;		//	Pet corral
	CECPetWords *		m_pPetWords;		//	宠物有话说
	CECComboSkill*		m_pComboSkill;		//	Combo skill

	APtrArray<CECSkill*>		m_aPtSkills;	//	Positive skill array
	APtrArray<CECSkill*>		m_aPsSkills;	//	Passive skill array
	APtrArray<CECSkill*>		m_aGoblinSkills;//	Goblin skill (stores cooldown data)
	APtrArray<CECSkill*>		m_aEquipSkills;	//	技能装备带来的技能

	AArray<WORD, WORD>			m_aWayPoints;	//	Active way points
	AArray<TEAMINV, TEAMINV&>	m_aTeamInvs;	//	Team invitations

	A3DGFXEx*		m_pMoveTargetGFX;
	A3DGFXEx*		m_pSelectedGFX;
	A3DGFXEx*		m_pHoverGFX;
	A3DGFXEx*		m_pFloatDust;
	A2DSprite*		m_pSpritePortrait;			//	The portrait sprite
	A2DSprite*		m_pDuelCountDown[3];		//	Duel count down sprites.
	A2DSprite*		m_pDuelStates[3];			//	Duel state, 0 - start, 1 - victory, 2 - lost
	AMSoundBuffer*	m_pCurMoveSnd;				//	Current move sound

	// Host faction info
	ACString		m_strNickname;				//	Nick name
	// Cash
	int				m_nCash;
	int				m_nCashMoneyRate;
	bool			m_bCashMoneyOpen;
	// Dividend
	int				m_nDividend;
	bool          m_bDividentInitialized;
	// Current emotion set
	int				m_nCurEmotionSet;

	int				m_iKingScore;				// 国王战场统帅分

	//  Favorite auction items saved in local machine
	AArray<int, int>	m_aFavorAucItems;

	int				m_iRoleCreateTime;			//  Role create time 
	int				m_iRoleLastLoginTime;		//  Role last login time
	int				m_iAccountTotalCash;		//  Account total cash

	APtrArray<CECObject*>		m_aTabSels;		//	'Tab' key selected enemies

	MultiExpState   m_multiExpState;            // 聚灵状态
	float           m_multiExpFactor;           // 聚灵多倍经验倍率
	int             m_multiExpLastTimeStamp;    // 上次选择聚灵的绝对时间(s)
	unsigned int    m_multiExpEnhanceTime;      // 聚灵多倍经验剩余时间(s)
	unsigned int    m_multiExpBufferTime;       // 聚灵缓冲剩余时间(s)
	unsigned int    m_multiExpImpairTime;       // 散灵剩余时间(s)
	time_t			m_multiExpExpireMoment;		// 聚灵当前状态(聚灵多倍经验/缓冲/散灵)截止时刻
	int				m_multiExpBeginTimesLeft;	// 今天还剩余的开启聚灵机会

	CONTRIB_INFO			m_contribInfo;		//	贡献度信息
	bool					m_contribInfoInitialized;	//	贡献度信息初始化标识
	FACTION_FORTRESS_INFO	m_fortressInfo;		//	帮主的帮派基地信息
	FACTION_FORTRESS_ENTER	m_fortressEnter;	//	进入基地信息

	CECGuildDiplomacyReminder	m_DiplomacyReminder;	//	帮派外交定时提醒

	std::bitset<PLAYER_LIMIT_MAX>		m_playerLimits;			//	服务器额外设定的禁止操作

	AArray<FORCE_INFO, FORCE_INFO&>	m_aForceInfo;	//	玩家势力信息
	CECForceMgr* m_pForceMgr;

	WEDDING_SCENE_INFO		m_weddingSceneInfo;	// 婚礼场景信息
	PROFIT_TIME_INFO m_profitInfo;			// 玩家收益时间信息

	COnlineAwardCtrl* m_pOnlineAwardCtrl; // 在线奖励
	bool				m_bPVPNoPenaltyState; // 是否在PVP无惩罚状态

	bool			m_bDefenseRuneEnable[DEFENSE_RUNE_NUM];	//	当前角色是否已有优化符
	bool			m_bIsInKingService;		// 是否正在进行国王服务操作

	bool			m_bInRebuildPet;// 是否洗髓或培训
	__int64			m_iTouchPoint;			//  Touch点数

	abase::hash_map<int, int> m_RelatedPlayer;	// 最近和自己关系密切的玩家

	COfflineShopCtrl* m_pOffShopCtrl; // 离线商店，寄卖
	CECAutoTeam*	m_pAutoTeam;			// 玩家自动组队

	TITLE_CONTAINER m_Titles;
	bool			m_bTitleDataReady;

	int				m_iSignInMonthCalendar; // 当前月签到日历
	int				m_iSignInThisYearStat;  // 当前年月份签到状态
	int				m_iSignInLastYearStat;  // 去年月份签到状态
	int				m_iSignInAwardedCountThisMonth;	// 当前月份已经领奖次数
	int				m_iSignLateCountThisMonth; // 当前月份已补签次数
	REINCARNATION_TOME m_ReincarnationTome; // 转生
	int				m_iRealmExp;			// 境界经验

	CChariot*		m_pChariot;				// 战车信息
	GENERAL_CARD	m_GeneralCard;			// 武将卡数据
	CARD_HOLDER		m_CardHolder;			// 武将栏位

	typedef std::set<int>	SlotSet;
	SlotSet			m_CardsToMove;			// 需要转移到仓库的卡牌
	
	CECActionSwitcherBase* m_pActionSwitcher;
	CECQuickBuyPopSaveLifeTrigger * m_pSaveLifeTrigger;	//	保命符弹出提示触发器
	int				m_iWorldContribution;
	int				m_iWorldContributionSpend;
	
	CECHostNavigatePlayer*	m_pNavigatePlayer; // 客户端player
	
	float			m_fPrayDistancePlus;
	
	DWORD			m_dwReviveTime;

protected:	//	Operations

	//	Load host resources
	bool LoadResources();
	//	When all resources are ready, this function is called
	virtual void OnAllResourceReady();
	virtual bool SetPetLoadResult(CECModel* pPetModel);


	//	Create inventories
	bool CreateInventories();
	//	Create shortcut sets
	bool CreateShortcutSets();
	//	Update item shortcut when item position changed
	void UpdateMovedItemSC(int tidItem, int iSrcIvtr, int iSrcSlot, int iDstIvtr, int iDstSlot);
	//	Update item shortcut when item removed
	void UpdateRemovedItemSC(int tidItem, int iIvtr, int iSlot, int* aExcSlots=NULL, int iNumExcSlot=0);
	//	Update item shortcut when two items exchanged
	void UpdateExchangedItemSC(int tidItem1, int iIvtr1, int iSlot1, int tidItem2, int iIvtr2, int iSlot2);
	//	Update pet shortcut when pet freed
	void UpdateFreedPetSC(int iPetIndex);
	//	Release skills
	void ReleaseSkills();
	//	Load sounds
	bool LoadSounds();
	//	Load duel images
	bool LoadDuelImages();
	//	Release duel images
	void ReleaseDuelImages();

	//	Cast skill
	bool CastSkill(int idTarget, bool bForceAttack, CECObject* pTarget=NULL);
	//	Estimate mouse cursor
	void EstimateCursor();
	//	Update time counters
	void UpdateTimers(DWORD dwDeltaTime);
	//	Update GFXs
	void UpdateGFXs(DWORD dwDeltaTime);
	//	Attack an object
	bool NormalAttackObject(int idTarget, bool bForceAttack, bool bMoreClose=false);
	//	Check whether host can gather specified matter
	bool CanGatherMatter(CECMatter* pMatter);
	//  商城购买、生产、拾取、交易获得卡牌或卡牌随机发生器时，自动从普通包裹移动到卡牌仓库
	bool MoveCardAuto(CECIvtrItem* pItem, int where, int slot);

	virtual void DoSkillStateAction();
	
	//	Message handler
	virtual void OnMsgPlayerExtState(const ECMSG& Msg);
	void OnMsgGstMove(const ECMSG& Msg);
	void OnMsgGstPitch(const ECMSG& Msg);
	void OnMsgGstYaw(const ECMSG& Msg);
	void OnMsgGstMoveAbsUp(const ECMSG& Msg);
	void OnMsgHstPushMove(const ECMSG& Msg);
	void OnMsgHstAutoMove(const ECMSG& Msg);
	void OnMsgHstPitch(const ECMSG& Msg);
	void OnMsgHstYaw(const ECMSG& Msg);
	void OnMsgLBtnClick(const ECMSG& Msg);
	void OnMsgRBtnClick(const ECMSG& Msg);
	void OnMsgHstWheelCam(const ECMSG& Msg);
	void OnMsgHstCamDefault(const ECMSG& Msg);
	void OnMsgHstCamPreset(const ECMSG& Msg);
	void OnMsgHstCamUserGet(const ECMSG& Msg);
	void OnMsgHstCamUserSet(const ECMSG& Msg);
	void OnMsgHstAttackResult(const ECMSG& Msg);
	void OnMsgHstAttacked(const ECMSG& Msg);
	void OnMsgHstDied(const ECMSG& Msg);
	void OnMsgHstPickupMoney(const ECMSG& Msg);
	void OnMsgHstPickupItem(const ECMSG& Msg);
	void OnMsgHstReceiveExp(const ECMSG& Msg);
	void OnMsgHstInfo00(const ECMSG& Msg);
	void OnMsgHstGoto(const ECMSG& Msg);
	void OnMsgHstOwnItemInfo(const ECMSG& Msg);
	void OnMsgHstSelTarget(const ECMSG& Msg);
	void OnMsgHstFixCamera(const ECMSG& Msg);
	void OnMsgHstExtProp(const ECMSG& Msg);
	void OnMsgHstAddStatusPt(const ECMSG& Msg);
	void OnMsgHstJoinTeam(const ECMSG& Msg);
	void OnMsgHstLeaveTeam(const ECMSG& Msg);
	void OnMsgHstNewTeamMem(const ECMSG& Msg);
	void OnMsgHstItemOperation(const ECMSG& Msg);
	void OnMsgHstTrashBoxOperation(const ECMSG& Msg);
	void OnMsgHstTeamInvite(const ECMSG& Msg);
	void OnMsgHstTeamReject(const ECMSG& Msg);
	void OnMsgHstTeamMemPos(const ECMSG& Msg);
	void OnMsgHstEquipDamaged(const ECMSG& Msg);
	void OnMsgHstTeamMemPickup(const ECMSG& Msg);
	void OnMsgHstNPCGreeting(const ECMSG& Msg);
	void OnMsgHstTradeStart(const ECMSG& Msg);
	void OnMsgHstTradeRequest(const ECMSG& Msg);
	void OnMsgHstTradeMoveItem(const ECMSG& Msg);
	void OnMsgHstTradeCancel(const ECMSG& Msg);
	void OnMsgHstTradeSubmit(const ECMSG& Msg);
	void OnMsgHstTradeConfirm(const ECMSG& Msg);
	void OnMsgHstTradeEnd(const ECMSG& Msg);
	void OnMsgHstTradeAddGoods(const ECMSG& Msg);
	void OnMsgHstTradeRemGoods(const ECMSG& Msg);
	void OnMsgHstWebTradeList(const ECMSG& Msg);
	void OnMsgHstWebTradeAttendList(const ECMSG& Msg);
	void OnMsgHstWebTradeGetItem(const ECMSG& Msg);
	void OnMsgHstWebTradeGetDetail(const ECMSG& Msg);
	void OnMsgHstWebTradePrePost(const ECMSG& Msg);
	void OnMsgHstWebTradeCancelPost(const ECMSG& Msg);
	void OnMsgHstWebTradeUpdate(const ECMSG& Msg);
	void OnMsgHstSysAuctionAccount(const ECMSG &Msg);
	void OnMsgHstSysAuctionBid(const ECMSG &Msg);
	void OnMsgHstSysAuctionCashTransfer(const ECMSG &Msg);
	void OnMsgHstSysAuctionGetItem(const ECMSG &Msg);
	void OnMsgHstSysAuctionList(const ECMSG &Msg);
	void OnMsgHstWeddingBookList(const ECMSG &Msg);
	void OnMsgHstWeddingBookSuccess(const ECMSG &Msg);
	void OnMsgHstIvtrInfo(const ECMSG& Msg);
	void OnMsgHstStartAttack(const ECMSG& Msg);
	void OnMsgHstGainItem(const ECMSG& Msg);
	void OnMsgHstPurchaseItems(const ECMSG& Msg);
	void OnMsgHstSpendMoney(const ECMSG& Msg);
	void OnMsgHstItemToMoney(const ECMSG& Msg);
	void OnMsgHstRepair(const ECMSG& Msg);
	void OnMsgHstUseItem(const ECMSG& Msg);
	void OnMsgHstUseItemWithData(const ECMSG& Msg);
	void OnMsgHstSkillData(const ECMSG& Msg);
	void OnMsgHstEmbedItem(const ECMSG& Msg);
	void OnMsgHstClearTessera(const ECMSG& Msg);
	void OnMsgHstCostSkillPt(const ECMSG& Msg);
	void OnMsgHstLearnSkill(const ECMSG& Msg);
	void OnMsgHstFlySwordTime(const ECMSG& Msg);
	void OnMsgHstProduceItem(const ECMSG& Msg);
	void OnMsgHstBreakItem(const ECMSG& Msg);
	void OnMsgHstTaskData(const ECMSG& Msg);
	void OnMsgHstTargetIsFar(const ECMSG& Msg);
	void OnMsgHstCameraMode(const ECMSG& Msg);
	void OnMsgHstPressCancel(const ECMSG& Msg);
	void OnMsgHstRootNotify(const ECMSG& Msg);
	void OnMsgHstStopAttack(const ECMSG& Msg);
	void OnMsgHstJump(const ECMSG& Msg);
	void OnMsgHstHurtResult(const ECMSG& Msg);
	void OnMsgHstAttackOnce(const ECMSG& Msg);
	void OnMsgHstPlayTrick(const ECMSG& Msg);
	void OnMsgHstSkillResult(const ECMSG& Msg);
	void OnMsgHstSkillAttacked(const ECMSG& Msg);
	void OnMsgHstAskToJoinTeam(const ECMSG& Msg);
	void OnMsgHstFaction(const ECMSG& Msg);
	void OnMsgHstTaskDeliver(const ECMSG& Msg);
	void OnMsgHstReputation(const ECMSG& Msg);
	void OnMsgHstItemIdentify(const ECMSG& Msg);
	void OnMsgHstSanctuary(const ECMSG& Msg);
	void OnMsgHstCorrectPos(const ECMSG& Msg);
	void OnMsgHstFriendOpt(const ECMSG& Msg);
	void OnMsgHstWayPoint(const ECMSG& Msg);
	void OnMsgHstBreathData(const ECMSG& Msg);
	void OnMsgHstSkillAbility(const ECMSG& Msg);
	void OnMsgHstCoolTimeData(const ECMSG& Msg);
	void OnMsgHstRevivalInquire(const ECMSG& Msg);
	void OnMsgHstSetCoolTime(const ECMSG& Msg);
	void OnMsgHstChangeTeamLeader(const ECMSG& Msg);
	void OnMsgHstExitInstance(const ECMSG& Msg);
	void OnMsgHstChangeFace(const ECMSG& Msg);
	void OnMsgHstTeamMemberData(const ECMSG& Msg);
	void OnMsgHstSetMoveStamp(const ECMSG& Msg);
	void OnMsgHstChatroomOpt(const ECMSG& Msg);
	void OnMsgHstMailOpt(const ECMSG& Msg);
	void OnMsgHstVendueOpt(const ECMSG& Msg);
	void OnMsgHstViewOtherEquips(const ECMSG& Msg);
	void OnMsgHstPariahTime(const ECMSG& Msg);
	void OnMsgHstPetOpt(const ECMSG& Msg);
	void OnMsgHstBattleOpt(const ECMSG& Msg);
	void OnMsgHstAccountPoint(const ECMSG& Msg);
	void OnMsgHstGMOpt(const ECMSG& Msg);
	void OnMsgHstRefineOpt(const ECMSG& Msg);
	void OnMsgHstItemBound(const ECMSG& Msg);
	void OnMsgHstSafeLock(const ECMSG& Msg);
	void OnMsgHstBuySellFail(const ECMSG& Msg);
	void OnMsgHstHpSteal(const ECMSG& Msg);
	void OnMsgHstMultiExpInfo(const ECMSG &Msg);
	void OnMsgHstMultiExpState(const ECMSG &Msg);
	void OnMsgChangePVPNoPenalty(const ECMSG &Msg);
	void OnMsgContinueComboSkill(const ECMSG &Msg);

	void OnMsgPlayerBaseInfo(const ECMSG& Msg);
	void OnMsgPlayerCustomData(const ECMSG& Msg);
	void OnMsgPlayerFly(const ECMSG& Msg);
	void OnMsgPlayerSitDown(const ECMSG& Msg);
	void OnMsgPlayerCastSkill(const ECMSG& Msg);
	void OnMsgPlayerGather(const ECMSG& Msg);
	void OnMsgPlayerDoEmote(const ECMSG& Msg);
	void OnMsgDoConEmote(const ECMSG& Msg);
	void OnMsgPickupMatter(const ECMSG& Msg);
	void OnMsgPlayerChangeShape(const ECMSG& Msg);
	void OnMsgPlayerGoblinOpt(const ECMSG& Msg);
	void OnMsgBoothOperation(const ECMSG& Msg);
	void OnMsgPlayerTravel(const ECMSG& Msg);
	virtual void OnMsgPlayerPVP(const ECMSG& Msg);
	void OnMsgPlayerUseItem(const ECMSG& Msg);
	void OnMsgPlayerChangeFace(const ECMSG& Msg);
	void OnMsgPlayerBindOpt(const ECMSG& Msg);
	void OnMsgPlayerDuelOpt(const ECMSG& Msg);
	virtual void OnMsgPlayerLevel2(const ECMSG& Msg);
	void OnMsgPlayerKnockback(const ECMSG& Msg);
	void OnMsgPlayerEquipDisabled(const ECMSG& Msg);
	void OnMsgCongregate(const ECMSG &Msg);
	void	OnMsgTeleport(const ECMSG &Msg);
	void OnMsgForce(const ECMSG &Msg);
	void OnMsgCountry(const ECMSG &Msg);
	void OnMsgTrickBattle(const ECMSG &Msg);

	void OnMsgHstGoblinInfo(const ECMSG& Msg);
	void OnMsgPlayerProperty(const ECMSG& Msg);
	void OnMsgPlayerPasswdChecked(const ECMSG& Msg);
	
	void OnMsgHstFactionContribNotify(const ECMSG &Msg);
	void OnMsgHstFactionFortressInfo(const ECMSG &Msg);
	void OnMsgHstEnterFactionFortress(const ECMSG &Msg);
	void OnMsgHstEngraveItem(const ECMSG &Msg);
	void OnMsgHstDPSDPHRank(const ECMSG &Msg);
	void OnMsgHstAddonRegen(const ECMSG &Msg);
	void OnMsgHstInvisibleObjList(const ECMSG &Msg);
	void OnMsgHstSetPlayerLimit(const ECMSG &Msg);
	void OnMsgInventoryDetail(const ECMSG &Msg);
	void OnMsgMultiobject(const ECMSG &Msg);
	void OnMsgWeddingSceneInfo(const ECMSG &Msg);
	
	void OnMsgOnlineAward(const ECMSG& Msg);
	void OnMsgProfitTime(const ECMSG& Msg);
	void OnMsgDefenseRuneEnable(const ECMSG& Msg);
	void OnMsgCashMoneyRate(const ECMSG& Msg);
	void OnMsgMeridiansNotify(const ECMSG& Msg);
	void OnMsgMeridiansResult(const ECMSG& Msg);

	void OnMsgStoneChangeEnd(const ECMSG& Msg);
	void OnMsgTouchPoint(const ECMSG& Msg);
	void OnMsgTitle(const ECMSG& Msg);
	void OnMsgSignIn(const ECMSG& Msg);
	void OnMsgUseGiftCard(const ECMSG& Msg);
	void OnMsgReincarnation(const ECMSG &Msg);
	void OnMsgRealm(const ECMSG &Msg);
	void OnMsgGeneralCard(const ECMSG& Msg);
	void OnMsgMonsterSpiritLevel(const ECMSG& Msg);
	void OnMsgRandMallShopping(const ECMSG& Msg);
	void OnMsgFactionPVPMaskModify(const ECMSG &Msg);
	void OnMsgWorldContribution(const ECMSG &Msg);
	void OnMsgUpdateInheritableEquipAddons(const ECMSG &Msg);
	void OnMsgClientScreenEffect(const ECMSG& Msg);
	void OnMsgComboSkillPrepare(const ECMSG& Msg);
	void OnMsgPrayDistanceChange(const ECMSG& Msg);

	//	Camera pitch
	void CameraPitch(float fDelta);
	//	Camera yaw
	void CameraYaw(float fDelta);
	//	Get push direction
	bool GetPushDir(A3DVECTOR3& vPushDir, DWORD dwMask, float fDeltaTime = 0);
	//	Can host touch target ?
	bool CanTouchTarget(const A3DAABB& aabbTarget);
	//	Can host touch target ?
	bool CanTouchTarget(const A3DVECTOR3& vTargetPos, float fTargetRad, int iReason, float fMaxCut=1.0f);
	bool CanTouchTarget(const A3DVECTOR3& vHostPos, const A3DVECTOR3& vTargetPos, float fTargetRad, int iReason, float fMaxCut=1.0f);
	//	Calculate distance between host and specified object
	bool CalcDist(int idObject, float* pfDist, CECObject** ppObject=NULL);
	//	Update selected target
	void UpdateSelectedTarget();
	//	Estimate move environment
	void EstimateMoveEnv(const A3DVECTOR3& vPos);
	//	Update equipment skins
	bool UpdateEquipSkins();
	//	Check whether host can do a behavior
	bool CanDo(int iThing);
	//	Fill NPC packs
	void FillNPCPack(int iIndex, const ACHAR* szName, int* aItems, float fPriceScale, bool bRecipe);
	//	Find mine tool in packages
	bool FindMineTool(int tidMine, int* piPack, int* piIndex, int* pidTool);
	//	When host is going to move, this function will be called
	void GoingMove();
	//	Play move sound
	void PlayMoveSound(int iIndex);
	//	Update move sound
	void UpdateMoveSound();
	//	Adjust host's transparency
	void AdjustTransparency(float fDistToCam, const A3DVECTOR3& vDir, DWORD dwTime);
	//	Update team member's position
	void UpdateTeamMemberPos(DWORD dwDeltaTime);
	//	Rearrange booth packages
	void RearrangeBoothPacks();
	//	Stop host moving naturally
	bool NaturallyStopMoving();
	//	Start the passive move
	void StartPassiveMove(const A3DVECTOR3& pos, int time, int type);

	//  Update Goblin
	bool UpdateGoblin();

	//	判断 HostPlayer 当前位置离目标是否太近
	bool IsTooNear( const A3DVECTOR3 &vTarget, float &fNearDist);

	//	判断 HostPlayer 当前位置离要移往的位置是否太近
	bool IsTooNear(const A3DVECTOR3 &vMovePos);

	//	判断指定位置是否无地形及凸包碰撞
	bool IsPosCollideFree(const A3DVECTOR3 &vTargetPos);

	//	计算 HostPlayer 从当前位置向指定 AABB 移动时恰好碰撞时两者的距离
	float CalcAABBOnCollidePos(const A3DAABB &aabbTarget);

	//	计算指定位置附近凸包无碰撞位置
	bool CalcBrushOnCollidePos(const A3DVECTOR3 &vTestPos, const A3DVECTOR3 &vDelta, const A3DVECTOR3 &vExtents, A3DVECTOR3 &vPos, bool &bNoCollide);

	//	计算 HostPlayer 与指定 AABB 接近但无碰撞的位置
	bool CalcCollideFreePos(const A3DAABB &aabbTarget, A3DVECTOR3 &vPos);

	//	计算指定位置处竖直方向上无碰撞的位置
	bool CalcVerticalCollideFreePos(const A3DVECTOR3 &vRefPos, A3DVECTOR3 &vPos);

	//	竖直方向上调整给定位置 vPos 使满足地面以上等条件
	float ClampAboveGround(const A3DVECTOR3 &vPos);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

