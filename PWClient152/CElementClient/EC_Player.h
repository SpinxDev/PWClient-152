/*
 * FILE: EC_Player.h
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

#include "EC_MsgDef.h"
#include "EC_StringTab.h"
#include "EC_Object.h"
#include "EC_GPDataType.h"
#include "EC_RoleTypes.h"
#include "EC_Counter.h"
#include "EC_IvtrTypes.h"
#include "EC_ChangePill.h"

#include "AAssist.h"
#include "AList2.h"
#include "A3DGeometry.h"

#include "EC_Face.h"
#include "hashmap.h"
//#include "EC_CustomizeBound.h"
#include <A3DMacros.h>

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

#define DEFAULT_ACTION_TYPE		((unsigned int)(-1))
// 转生次数，版本上限
#define MAX_REINCARNATION 2
///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////

class CECPlayerMan;
class CECViewport;
class CECModel;
class CECTeam;
class CECIconDecal;
class A3DViewport;
class A3DGFXEx;
class CECSkill;
class CECPateText;
class CECSprite;
class CECSpriteDecal;
class CECNPC;
class CECBubbleDecalList;
class CECGoblin;

class CECCustomizeBound;

struct EC_PLAYERLOADRESULT;
struct PLAYER_ACTION_INFO_CONFIG;
struct WEAPON_ESSENCE;
struct ARMOR_ESSENCE;
struct FASHION_ESSENCE;
struct FASHION_WEAPON_CONFIG;
struct TITLE_CONFIG;

class CECIvtrItem;
class CECPlayerActionController;
class CECPlayerBodyController;

extern const char* _multiobject_effect[3];
///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CECPlayer
//	
///////////////////////////////////////////////////////////////////////////
enum
{
	enumSkinShowNone = 0,
	enumSkinShowUpperBody,
	enumSkinShowWrist,
	enumSkinShowLowerBody,
	enumSkinShowFoot,
	enumSkinShowUpperAndLower,
	enumSkinShowUpperAndWrist,
	enumSkinShowLowerAndFoot,
	enumSkinShowUpperLowerAndWrist,
	enumSkinShowArmet,
	enumSkinShowHand,
};

//	Skin index
enum
{
	SKIN_BODY_INDEX = 0,
	SKIN_UPPER_BODY_INDEX,
	SKIN_WRIST_INDEX,
	SKIN_LOWER_INDEX,
	SKIN_FOOT_INDEX,
	SKIN_HEAD_INDEX,
	SKIN_FASHION_UPPER_BODY_INDEX,
	SKIN_FASHION_WRIST_INDEX,
	SKIN_FASHION_LOWER_INDEX,
	SKIN_FASHION_FOOT_INDEX,
	NUM_SKIN_INDEX,
};

//	Skin sort index
enum
{
	SKIN_SORT_DEFAULT			= 0,
	SKIN_SORT_FOOT,
	SKIN_SORT_LOWER,
	SKIN_SORT_UPPER,
	SKIN_SORT_WRIST,
	SKIN_SORT_HEAD,
	SKIN_SORT_WEAPON,
	SKIN_SORT_WING,
};

enum
{
	PLAYERMODEL_MAJOR,
	PLAYERMODEL_PROFESSION,		// player profession related
	PLAYERMODEL_DUMMYTYPE2,		// player buff related
	PLAYERMODEL_DUMMYTYPE3,		// not use
	
	PLAYERMODEL_MAX,
	PLAYERMODEL_TYPEALL = 0xffffffff
};

enum enumWingType {
	WINGTYPE_WING,				// 飞行器类型：翅膀
	WINGTYPE_FLYSWORD,			// 飞行器类型：飞剑
	WINGTYPE_DOUBLEWHEEL,		// 飞行器类型：双脚飞行器
};

void _GenSkinPath(char* szPath, int nProfession, int nGender, const char* szSkinName);
void _GenDefaultSkinPath(char* szPath, int nProfession, int nGender, const char* szSkinName);

class CECPlayer : public CECObject
{
	friend class CECEPWorkStand;
	friend class CECPlayerBodyController;
public:		//	Types
	struct MeridiansProp
	{
		int level;	
		int hp;
		int phyDefence;
		int mgiDefence;
		int phyAttack;
		int mgiAttack;
		int eightTrigramsState1; // 经脉冲击界面按钮状态[48个按钮，每个按钮三个状态，通过2个bit表示]
		int eightTrigramsState2;
		int eightTrigramsState3;
		int chargableUpgradeNum; // 乾坤石冲穴次数
		int freeUpgradeNum;      // 免费冲穴次数
		int continuousLoginDays; // 连续登陆天数
	};

	struct EquipsLoadResult
	{
		A3DSkin*		aSkins[NUM_SKIN_INDEX][3];
		DWORD			dwSkinChangeMask;
		DWORD			dwShowMask;
		DWORD			dwFashionShowMask;
		bool			bWeaponChanged;
		bool			bFashionWeaponChanged;
		CECModel*		pLeftHandWeapon;
		CECModel*		pRightHandWeapon;
		unsigned int	uAttackType;
		unsigned int	uFashionAttackType;
		bool			bWingChanged;
		CECModel*		pWing;
		CECModel*		pWing2;
		enumWingType	wingType;
		WORD			stoneWeapon;
		WORD			stoneUpperBody;
		WORD			stoneWrist;
		WORD			stoneLowerBody;
		WORD			stoneFoot;
		const char*		strLeftWeapon;
		const char*		strRightWeapon;
		const char*		strLeftFashionWeapon;
		const char*		strRightFashionWeapon;
	};

#define CUSTOMIZE_DATA_VERSION_1		0x10007000
	struct PLAYER_CUSTOMIZEDATA_1
	{
		DWORD							dwVersion;		// version

		CECFace::FACE_CUSTOMIZEDATA_1	faceData;		// data defined face

		unsigned short					bodyID;			// 妖兽或妖精使用的形象ID
		A3DCOLOR						colorBody;		// 身体的颜色

		unsigned char					headScale;		// 头大小
		unsigned char					upScale;		// 上身健壮度
		unsigned char					waistScale;		// 腰部围度
		unsigned char					armWidth;		// 上肢围度
		unsigned char					legWidth;		// 下肢围度
		unsigned char					breastScale;	// 胸部高低（女性才有）
	};

#define CUSTOMIZE_DATA_VERSION			0x10007001
	struct PLAYER_CUSTOMIZEDATA
	{
		DWORD							dwVersion;		// version

		CECFace::FACE_CUSTOMIZEDATA		faceData;		// data defined face

		unsigned short					bodyID;			// 妖兽或妖精使用的形象ID
		A3DCOLOR						colorBody;		// 身体的颜色

		unsigned char					headScale;		// 头大小
		unsigned char					upScale;		// 上身健壮度
		unsigned char					waistScale;		// 腰部围度
		unsigned char					armWidth;		// 上肢围度
		unsigned char					legWidth;		// 下肢围度
		unsigned char					breastScale;	// 胸部高低（女性才有）

		static PLAYER_CUSTOMIZEDATA From(const PLAYER_CUSTOMIZEDATA_1 &rhs)
		{
			PLAYER_CUSTOMIZEDATA result;

			result.dwVersion	= rhs.dwVersion;

			result.faceData		= CECFace::FACE_CUSTOMIZEDATA::From(rhs.faceData);

			result.bodyID		= rhs.bodyID;
			result.colorBody	= rhs.colorBody;

			result.headScale	= rhs.headScale;
			result.upScale		= rhs.upScale;
			result.waistScale	= rhs.waistScale;
			result.armWidth		= rhs.armWidth;
			result.legWidth		= rhs.legWidth;
			result.breastScale	= rhs.breastScale;

			return result;
		}

		static PLAYER_CUSTOMIZEDATA From(const void *rhs, size_t size)
		{
			if (rhs)
			{
				if (size == sizeof(PLAYER_CUSTOMIZEDATA_1))
					return From(*(PLAYER_CUSTOMIZEDATA_1*)rhs);
				if (size == sizeof(PLAYER_CUSTOMIZEDATA))
					return *(PLAYER_CUSTOMIZEDATA *)rhs;
			}
			PLAYER_CUSTOMIZEDATA result;
			::memset(&result, 0, sizeof(result));
			return result;
		}
	};

#define SCALE_HEAD_FACTOR		0.99f
#define SCALE_UP_FACTOR			0.99f
#define SCALE_WAIST_FACTOR		0.99f
#define WIDTH_ARM_FACTOR		0.99f
#define WIDTH_LEG_FACTOR		0.99f
#define	SCALE_BREAST_FACTOR		0.99f

	struct PLAYER_CUSTOMIZE_FACTOR
	{
		float fScaleHeadFactor;
		float fScaleUpFactor;
		float fScaleWaistFactor;
		float fWidthArmFactor;
		float fWidthLegFactor;
		float fScaleBreastFactor;
	};

	//	Player action type
	enum
	{
		ACTTYPE_SH = 0,	//	Single hand weapon
		ACTTYPE_FH,		//	Free hands
		ACTTYPE_BHF,	//	Both hands far range weapon
		ACTTYPE_BHN,	//	Both hands near range weapon
		NUM_ACTTYPE,
	};

	//	Player action index
	enum PLAYER_ACTION_TYPE
	{
		// 0
		ACT_STAND = 0,				// 站立
		ACT_FIGHTSTAND,				// 战斗站立
		ACT_WALK,					// 行走
		ACT_RUN,					// 奔跑
		ACT_JUMP_START,				// 起跳

		// 5
		ACT_JUMP_LOOP,				// 起跳空中循环
		ACT_JUMP_LAND,				// 跳跃落地
		ACT_SWIM,					// 游动
		ACT_HANGINWATER,			// 水中漂浮
		ACT_TAKEOFF,				// 翅膀起飞 should be ACT_TAKEOFF_WING

		// 10
		ACT_HANGINAIR,				// 翅膀悬停 should be ACT_HANGINAIR_WING
		ACT_FLY,					// 翅膀前进 should be ACT_FLY_WING
		ACT_FLYDOWN,				// 翅膀高空下降 should be ACT_FLYDOWN_WING_HIGH
		ACT_FLYDOWN_WING_LOW,		// 翅膀低空下降 should be ACT_FLYDOWN_WING_LOW
		ACT_LANDON,					// 翅膀落地 should be ACT_LAND_WING

		// 15
		ACT_TAKEOFF_SWORD,			// 飞剑起飞
		ACT_HANGINAIR_SWORD,		// 飞剑悬停
		ACT_FLY_SWORD,				// 飞剑前进
		ACT_FLYDOWN_SWORD_HIGH,		// 飞剑高空下降
		ACT_FLYDOWN_SWORD_LOW,		// 飞剑低空下降

		// 20
		ACT_LANDON_SWORD, 			// 飞剑落地 
		ACT_SITDOWN,				// 打坐
		ACT_SITDOWN_LOOP,			// 打坐循环
		ACT_STANDUP,				// 打坐站起
		ACT_WOUNDED,				// 受伤

		// 25
		ACT_GROUNDDIE,				// 陆地死亡
		ACT_GROUNDDIE_LOOP,			// 死亡地面循环
		ACT_WATERDIE,				// 水中死亡
		ACT_WATERDIE_LOOP,			// 死亡水中循环
		ACT_AIRDIE_ST,				// 空中死亡

		// 30
		ACT_AIRDIE,					// 空中死亡下落循环
		ACT_AIRDIE_ED,				// 空中死亡落地
		ACT_AIRDIE_LAND_LOOP,		// 死亡落地循环
		ACT_REVIVE,					// 复活
		ACT_CUSTOMIZE,				// 长休闲动作

		// 35
		ACT_STRIKEBACK,				// 被击退
		ACT_STRIKEDOWN,				// 被击倒
		ACT_STRIKEDOWN_LOOP,		// 被击倒倒地循环
		ACT_STRIKEDOWN_STANDUP,		// 被击倒站起
		ACT_PICKUP,					// 采摘

		// 40
		ACT_PICKUP_LOOP,			// 采摘植物循环
		ACT_PICKUP_STANDUP,			// 采摘站起
		ACT_PICKUP_MATTER,			// 捡东西
		ACT_GAPE,					// 伸懒腰
		ACT_LOOKAROUND,				// 四处张望

		// 45
		ACT_PLAYWEAPON,				// 转动兵器
		ACT_EXP_WAVE,				// 招手
		ACT_EXP_NOD,				// 点头
		ACT_EXP_SHAKEHEAD,			// 摇头
		ACT_EXP_SHRUG,				// 耸肩膀

		// 50
		ACT_EXP_LAUGH,				// 大笑
		ACT_EXP_ANGRY,				// 生气
		ACT_EXP_STUN,				// 晕倒
		ACT_EXP_DEPRESSED,			// 沮丧
		ACT_EXP_KISSHAND,			// 飞吻

		// 55
		ACT_EXP_SHY,				// 害羞
		ACT_EXP_SALUTE,				// 抱拳
		ACT_EXP_SITDOWN,			// 坐下
		ACT_EXP_SITDOWN_LOOP,		// 坐下循环
		ACT_EXP_SITDOWN_STANDUP,	// 坐下站起

		// 60
		ACT_EXP_ASSAULT,			// 冲锋
		ACT_EXP_THINK,				// 思考
		ACT_EXP_DEFIANCE,			// 挑衅
		ACT_EXP_VICTORY,			// 胜利
		ACT_EXP_KISS,				// 亲吻

		// 65
		ACT_EXP_KISS_LOOP,			// 亲吻循环
		ACT_EXP_KISS_END,			// 亲吻结束
		ACT_ATTACK_1,				// 普攻1
		ACT_ATTACK_2,				// 普攻2
		ACT_ATTACK_3,				// 普攻3

		// 70
		ACT_ATTACK_4,				// 普攻4
		ACT_ATTACK_TOSS,			// 放暗器
		ACT_TRICK_RUN,				// 跑动中的花招
		ACT_TRICK_JUMP,				// 跳跃中的花招
		ACT_FLY_GLIDE,				// 翅膀滑翔

		// 75
		ACT_FLY_GLIDE_SWORD,		// 飞剑滑翔
		ACT_EXP_FIGHT,				// 战斗
		ACT_EXP_ATTACK1,			// 攻击1
		ACT_EXP_ATTACK2,			// 攻击2
		ACT_EXP_ATTACK3,			// 攻击3

		// 80
		ACT_EXP_ATTACK4,			// 攻击4
		ACT_EXP_DEFENCE,			// 防御
		ACT_EXP_FALL,				// 摔倒
		ACT_EXP_FALLONGROUND,		// 倒地
		ACT_EXP_LOOKAROUND,			// 张望

		// 85
		ACT_EXP_DANCE,				// 舞蹈
		ACT_EXP_FASHIONWEAPON,		// 时装武器动作
		ACT_USEITEM,				// 通用的使用物品动作
		ACT_USEITMELOOP,			// 通用的使用物品循环动作
		ACT_TWO_KISS,				// 双人亲吻
		
		// 90
		ACT_USING_TARGET_ITEM,		// 使用道具
		ACT_SWIM_FOR_MOVESKILL,		// 水中移动攻击时播放的游泳动作

		ACT_MAX,
		ACT_CASTSKILL,				// !! This is only a placeholder which represents skill actions
	};

	struct PLAYER_ACTION
	{
		PLAYER_ACTION_TYPE				type;
		PLAYER_ACTION_INFO_CONFIG *		data;
	};

	//	Move mode
	enum
	{
		MOVE_STAND = 0,
		MOVE_MOVE,		//	Normal move, walk, run, swim or fly
		MOVE_JUMP,
		MOVE_FREEFALL,
		MOVE_SLIDE,
	};

	//	Player resources ready flag
	enum
	{
		RESFG_SKELETON	= 0x01,
		RESFG_SKIN		= 0x02,
		RESFG_CUSTOM	= 0x04,
		RESFG_ASSEMBLED	= 0x08,
		RESFG_ALL		= 0x0f,
	};

	//	Render Name Flag
	enum
	{
		RNF_NAME		= 0x01,
		RNF_TITLE		= 0x02,
		RNF_FACTION		= 0x04,
		RNF_SELL		= 0x08,
		RNF_BUY			= 0x10,
		RNF_WORDS		= 0x20,
		RNF_FORCE		= 0x40,
		RNF_ALL			= 0x4f,
	};

	//	Bubble text
	enum
	{
		BUBBLE_DAMAGE = 0,
		BUBBLE_EXP,
		BUBBLE_SP,
		BUBBLE_MONEY,
		BUBBLE_LEVELUP,
		BUBBLE_HITMISSED,
		BUBBLE_INVALIDHIT,
		BUBBLE_IMMUNE,
		BUBBLE_HPWARN,
		BUBBLE_MPWARN,
		BUBBLE_REBOUND,		// 反弹
		BUBBLE_BEAT_BACK,	// 反击	
		BUBBLE_ADD,			// 吸血的加号
		BUBBLE_DODGE_DEBUFF,
		BUBBLE_REALMEXP,
	};

	// Player Attach mode
	enum AttachMode
	{
		enumAttachNone,
		enumAttachRideOnPet,
		enumAttachRideOnPlayer,
		enumAttachHugPlayer,
	};

	//	Effect type
	enum
	{
		EFF_FACEPILL = 1,
	};

	//	Duel state
	enum
	{
		DUEL_ST_NONE = 0,
		DUEL_ST_PREPARE,
		DUEL_ST_INDUEL,
		DUEL_ST_STOPPING,
	};

	//	GM flags
	enum
	{
		GMF_IAMGM		= 0x0001,	//	I'm GM
		GMF_INVISIBLE	= 0x0002,
		GMF_INVINCIBLE	= 0x0004,
	};

	//	Player information got from server
	struct INFO
	{
		int		cid;	//	Character ID
		int		crc_c;	//	customized data crc
		int		crc_e;	//	Equipment data crc
	};

	//	Team requirment
	struct TEAMREQ
	{
		bool	bShowReq;		//	true, show team requirment
		bool	bShowLevel;		//	true, show host's level and profession
		int		iType;			//	0: search team; 1: search member
		int		iLevel;			//	Player's level
		int		iProfession;	//	Player's profession
	};
	
	//	Pate content render info
	struct PATECONTENT
	{
		int		iVisible;	//	Visible flag. 0, not set; 1, not visible; 2, visible
		int		iBaseX;		//	Base x
		int		iBaseY;		//	Base y
		int		iCurY;		//	Current y
		float	z;			//	z value
	};

	//	PVP infomation
	struct PVPINFO
	{
		bool	bEnable;		//	PVP switch
		DWORD	dwCoolTime;
		DWORD	dwMaxCoolTime;
		bool	bFreePVP;		//	Free PVP flag, ignore bEnable flag
		bool	bInPVPCombat;	//	true, in PVP combat
		int		iDuelState;		//	Duel state
		int		idDuelOpp;		//	Duel opponent
		int		iDuelTimeCnt;	//	Duel time counter
		int		iDuelRlt;		//	Duel result. 0, no defined; 1-win; 2-lose; 3-draw
	};
	
	//	Constants used in moving control
	struct MOVECONST
	{
		float	fStepHei;		//	Maximum step height
		float	fMinAirHei;		//	Minimum distance to terrain (or water) when fly
		float	fMinWaterHei;	//	Minimum distance to terrain when swim
		float	fShoreDepth;	//	Shore depth
		float	fWaterSurf;		//	Water surface depth
	};

	//	Riding pet info.
	struct RIDINGPET
	{
		int		id;
		unsigned short color;

		RIDINGPET()
		{
			Reset();
		}

		void Reset()
		{
			id = 0;
			color = 0;
		}

		bool GetColor(A3DCOLOR &clr)const
		{
			//	获取当前骑宠的染色颜色
			return id>0 && GetColor(color, clr);
		}

		static bool GetColor(unsigned short c, A3DCOLOR &clr)
		{
			//	查询当前骑宠是否染过色，如果染过色，返回被染之后的颜色
			bool bRet(false);
			if (c & (1<<(sizeof(c)*8-1)))
			{
				//	unsigned short 首位为1时，表示染过色，此时可获取颜色
				clr = A3DCOLORRGB(((c) & (0x1f << 10)) >> 7, ((c) & (0x1f << 5)) >> 2, ((c) & 0x1f) << 3);
				bRet = true;
			}
			return bRet;
		}

		static A3DCOLOR GetDefaultColor()
		{
			return A3DCOLORRGB(255, 255, 255);
		}
	};

	struct GFXRECORD
	{
		AString strPath;
		AString strHook;
		float fScale;
	};

	struct MULTIOBJECT_EFFECT
	{
		int iTarget;
		char cType;

		bool operator == (const MULTIOBJECT_EFFECT& src) const
		{
			return iTarget == src.iTarget && cType == src.cType;
		}
	};
	struct _mo_hash_function
	{
		unsigned long operator()(const MULTIOBJECT_EFFECT &rhs)const{ return rhs.iTarget; }
	};
	
public:		//	Constructor and Destructor

	CECPlayer(CECPlayerMan* pPlayerMan);
	virtual ~CECPlayer();

public:		//	Attributes

public:		//	Operations

	//	Release object
	virtual void Release();

	//	Tick routine
	virtual bool Tick(DWORD dwDeltaTime);
	//	Render routine
	virtual bool Render(CECViewport* pViewport, int iRenderFlag=0) { return true; }
	//	Render when player is opening booth
	virtual bool RenderForBooth(CECViewport* pViewport, int iRenderFlag=0) { return true; }

	//	Process message
	virtual bool ProcessMessage(const ECMSG& Msg);

	//	Set absolute position
	virtual void SetPos(const A3DVECTOR3& vPos);
protected:
	//	不应直接调用SetDirAndUp修改模型朝向，应直接调用 StartModelMove、StopModelMove、ChangeModelMoveDirAndUp 等
	virtual void SetDirAndUp(const A3DVECTOR3& vDir, const A3DVECTOR3& vUp);
public:
	bool SupportCastSkillWhenMove()const;
	bool CanCombineWithMoveForSkill(int idSkill=-1)const;
	void StartModelMove(const A3DVECTOR3& vMoveDir, const A3DVECTOR3& vHeadUp, DWORD dwSwitchDirTime);
	void StopModelMove(const A3DVECTOR3& vMoveDir, const A3DVECTOR3& vHeadUp, DWORD dwSwitchDirTime);
	void StopModelMove();
	void ChangeModelMoveDirAndUp(const A3DVECTOR3& vMoveDir, const A3DVECTOR3& vHeadUp);
	void ChangeModelTargetDirAndUp(const A3DVECTOR3 &vDir, const A3DVECTOR3 &vUp);
	A3DVECTOR3 GetModelMoveDir()const;
	//	Player was killed
	virtual void Killed(int idKiller) {};
	virtual void TurnFaceTo(int idTarget, DWORD dwTime=200);

	virtual int GetEquippedItem(int index)const;
	//	Get number of equipped items of specified suite
	virtual int GetEquippedSuiteItem(int idSuite, int* aItems=NULL);

	//	Set loaded model to player object, this function is used in multithread loading process
	virtual bool SetPlayerLoadedResult(EC_PLAYERLOADRESULT& Ret);
	bool SetEquipsLoadedResult(EquipsLoadResult& ret, bool bUpdateAtOnce);
	virtual bool SetPetLoadResult(CECModel* pPetModel);
	virtual bool SetDummyLoadResult(int iShape, CECModel* pModel);

	void CloneSimplePropertyTo(CECPlayer *player)const;

	//	Set player's transparence
	void SetTransparent(float fTrans);

	//	Get player information got from server
	const INFO& GetPlayerInfo() const { return m_PlayerInfo; }
	//	Get basic properties
	const ROLEBASICPROP& GetBasicProps() const { return m_BasicProps; }
	//	Get extend properties
	const ROLEEXTPROP& GetExtendProps() const { return m_ExtProps; }
	//	Set properties
	void SetProps(const ROLEBASICPROP* pBase, const ROLEEXTPROP* pExp)
	{ 
		if(pBase){m_BasicProps = *pBase;}
		if(pExp){m_ExtProps = *pExp;}
	}

	//	Get selected target
	int GetSelectedTarget() { return m_idSelTarget; }
	//	Set selected target
	void SetSelectedTarget(int id) { m_idSelTarget = id; }

	//	Set part extend properties
	void SetPartExtendProps(int iPropIdx, void* pData);

	//	Get player name
	const ACHAR* GetName()const{ return m_strName; }
	void SetName(const ACHAR *szName);
	//	当前名称是否受国战影响
	bool GetShowNameInCountryWar();
	//	计算受国战影响后的名称
	ACString GetNameInCountryWar();
	//	Get player name color
	DWORD GetNameColor();
	//	Get character ID
	int GetCharacterID()const{ return m_PlayerInfo.cid; }
	//	Get move mode
	int GetMoveMode()const{ return m_iMoveMode; }
	//	Set move mode
	void SetMoveMode(int iMode) { m_iMoveMode = iMode; }
	//	Get move environment
	int GetMoveEnv()const{ return m_iMoveEnv; }
	//	Set move environment
	void SetMoveEnv(int iEnv) { m_iMoveEnv = iEnv; }
	//	Get player's AABB
	const A3DAABB& GetPlayerAABB() { return m_aabb; }
	//	Get player's pick AABB
	const A3DAABB& GetPlayerPickAABB();
	//	Get player's AABB from the skin model
	const A3DAABB & GetModelAABB();
	//	Get player's skin model object
	A3DSkinModel * GetA3DSkinModel();
	//	Get touch radius
	float GetTouchRadius()const{ return m_fTouchRad; }
	//	Set money amount
	void SetMoneyAmount(int iMoneyCnt) { m_iMoneyCnt = iMoneyCnt; }
	//	Get money amount
	int GetMoneyAmount()const{ return m_iMoneyCnt; }
	//	Add money amount
	int AddMoneyAmount(int iAmount);
	//	Get maximum money amount
	int GetMaxMoneyAmount()const{ return m_iMaxMoney; }
	//	Get walk-run flag
	bool GetWalkRunFlag()const{ return m_bWalkRun; }
	//	Set walk-run flag
	void SetWalkRunFlag(bool bFlag) { m_bWalkRun = bFlag; }
	//	Get rush fly flag
	bool GetRushFlyFlag()const{ return m_bRushFly; }
	//	Set rush fly flag
	void SetRushFlyFlag(bool bRushFly) { m_bRushFly = bRushFly; }
	//	Set / Get name rendering flag
	void SetRenderNameFlag(bool bTrue) { m_bRenderName = bTrue; }
	bool GetRenderNameFlag() { return m_bRenderName; }
	//	Set / Get bars rendering flag
	void SetRenderBarFlag(bool bTrue) { m_bRenderBar = bTrue; }
	bool GetRenderBarFlag() { return m_bRenderBar; }
	//	Get race
	int GetRace()const;
	//	Get profession
	int GetProfession()const{ return m_iProfession; }
	//	Get gender
	int GetGender()const{ return m_iGender; }
	CECSkill * GetCurSkill() { return m_pCurSkill; }
	//	Set last said words
	void SetLastSaidWords(const ACHAR* szWords, int nEmotionSet, CECIvtrItem *pItem);
	//	Get player's reputation
	int GetReputation()const{ return m_iReputation; }
	//	Set player's reputation
	void SetReputation(int iRep) { m_iReputation = iRep; }
	//	Get player's current model type
	int GetShapeType() const;
	//	Get player's current model id
	int GetShapeID() const;
	//	Get original shape id
	int GetOriginalShapeID()const{ return m_iShape;}
	//  Get player's shape mask
	unsigned char GetShapeMask() const;
	// Shape info compatible fix
	void FixOldShapeInfo(int& iShape);
	//	Get spouse
	int GetSpouse()const{ return m_idSpouse; }
	//	Get force
	int GetForce() const { return m_idForce; }
	int	GetCountry()const { return m_idCountry; }
	void SetCountry(int id);
	bool IsKing() const { return (m_dwStates2 & GP_STATE2_ISKING) != 0; }
	bool GetShowName()const;
	//	Set spouse
	void SetSpouse(int idSpouse);
	//	Get PVP information
	const PVPINFO& GetPVPInfo()const{ return m_pvp; }
	//	Is PVP open ?
	bool IsPVPOpen() { return m_pvp.bEnable; }
	//	Is player in duel ?
	bool IsInDuel() { return m_pvp.iDuelState == DUEL_ST_INDUEL; }
	//	Set duel result
	void SetDuelResult(int iResult) { m_pvp.iDuelRlt = iResult; }
	//	Is in fashion mode ?
	bool InFashionMode()const{ return m_bFashionMode; }
	//	Set fashion mode
	void SetFashionMode(bool b){ m_bFashionMode = b; }
	//
	void SetFactionPVPMask(unsigned char mask);
	bool IsInFactionPVP()const;
	bool CanAttackFactionPVPMineCar()const;
	bool CanAttackFactionPVPMineBase()const;
	//	Get faction ID
	int GetFactionID()const{ return m_idFaction; }
	//	Set faction ID
	void SetFactionID(int id);
	void OnFactionNameChange();
	//	Get faction role
	int GetFRoleID()const{ return m_idFRole; }
	//	Set faction role
	void SetFRoleID(int role);
	//	On start binding buddy
	virtual void OnStartBinding(int idMule, int idRider);
	//	Get buddy state
	int GetBuddyState()const
	{
		if (m_bHangerOn) return 2;
		else if (m_iBuddyId) return 1;
		else return 0;
	}
	//	Get buddy id
	int GetCandBuddyID()const{ return m_idCandBuddy; }
	int GetBuddyID()const{ return m_iBuddyId; }
	CECPlayer * GetBuddy()const;
	bool CanBindBuddy()const;
	
	//  Get current pet id
	int GetCurPetID()const{ return m_idCurPet; }
	//  Set current pet id
	void SetCurPetID(int id) { m_idCurPet = id; }
	
	//	Get battle camp this player belongs to
	int GetBattleCamp()const{ return m_iBattleCamp; }
	//	Check whether player in a same battle camp
	bool InSameBattleCamp(CECPlayer* pPlayer);
	bool InSameBattleCamp(CECNPC* pNPC);
	//	Is player in battle
	bool IsInBattle() { return m_iBattleCamp != GP_BATTLE_CAMP_NONE; }
	//	Change player's tank leader state
	void ChangeTankLeader(int idTank, bool bBecomeLeader);
	//	Update player's tank leader state
	void UpdateTankLeader();
	//	Get number of player controlled tank
	int GetBattleTankNum() { return m_aBattleTanks.GetSize(); }
	//	Get tank id controlled by player
	int GetBattleTank(int n) { return m_aBattleTanks[n]; }

	//	Get booth state
	int GetBoothState()const{ return m_iBoothState; }
	//	Set booth state
	void SetBoothState(int iState) { m_iBoothState = iState; }
	//	Set booth name
	void SetBoothName(const ACHAR* szName);
	//	Get booth name
	const ACHAR* GetBoothName()const{ return m_strBoothName; }
	//	Get booth CRC
	int GetBoothCRC() { return m_crcBooth; }

	//	Get player state
	bool IsDead()const{ return (m_dwStates & GP_STATE_CORPSE) ? true : false; }
	bool IsFlying()const{ return (m_dwStates & GP_STATE_FLY) ? true : false; }
	bool IsSitting()const{ return (m_dwStates & GP_STATE_SITDOWN) ? true : false; }
	bool IsInvader()const{ return (m_dwStates & GP_STATE_INVADER) ? true : false; }
	bool IsPariah()const{ return (m_dwStates & GP_STATE_PARIAH) ? true : false; }
	bool IsGoblinRefineActive()const{ return (m_dwStates & GP_STATE_GOBLINREFINE) ? true : false; }
	bool IsInvisible()const{ return (m_dwStates & GP_STATE_INVISIBLE) ? true : false;}
	BYTE GetPariahLevel()const{ return m_byPariahLvl; }
	bool IsInSanctuary()const{ return m_bInSanctuary; }
	bool IsPetInSanctuary()const{ return m_bPetInSanctuary; }
	bool IsEquipDisabled(int iEquipIndex)const{ return iEquipIndex >= SIZE_ALL_EQUIPIVTR ? true : ((__int64(1)<<iEquipIndex) & m_i64EquipDisabled)!=0;}
	

	//	Is resources ready ?
	bool IsSkeletonReady()const { return (m_dwResFlags & RESFG_SKELETON) ? true : false; }
	bool IsSkinReady()const { return (m_dwResFlags & RESFG_SKIN) ? true : false; }
	bool IsCustomizeReady() const{ return (m_dwResFlags & RESFG_CUSTOM) ? true : false; } 
	bool IsAllResReady()const { return (m_dwResFlags & RESFG_ALL) == RESFG_ALL; }

	bool IsAboutToDie()const { return m_bAboutToDie; }
	void SetAboutToDie(bool bFlag) { m_bAboutToDie = bFlag; }
	//	Check whether a action index is valid
	bool IsValidAction(int iIndex) { return (iIndex >= 0 && iIndex < ACT_MAX) ? true : false; }

	//	Is host a GM
	bool IsGM() { return (m_dwGMFlags & GMF_IAMGM) ? true : false; }
	bool IsGMInvisible() { return (m_dwGMFlags & GMF_INVISIBLE) ? true : false; }
	bool IsGMInvincible() { return (m_dwGMFlags & GMF_INVINCIBLE) ? true : false; }
	
	//  Is host shield user
	bool IsShieldUser()	{ return (m_dwStates & GP_STATE_SHIELDUSER) ? true : false; }
	
	//  is a frog?
	bool IsFrog();

	//	Get team to which this player belongs to
	CECTeam* GetTeam() { return m_pTeam; }
	//	Set team to which this player belongs to
	void SetTeam(CECTeam* pTeam) { m_pTeam = pTeam; } 
	//	Is specified id a member of our team ?
	bool IsTeamMember(int idPlayer);
	//	Get team requirement
	const TEAMREQ& GetTeamRequire()const{ return m_TeamReq; }
	//	Set team requirement
	void SetTeamRequireText(const TEAMREQ& Req);
	void SetTeamRequire(const TEAMREQ& Req, bool bConfirm);
	//	Get team requirement string
	const ACHAR* GetTeamReqText() { return m_strTeamReq; }

	//	Check extend state (bit index)
	bool GetExtState(int n);

	//	Get icon state array
	const S2C::IconStates& GetIconStates() const { return m_aIconStates; }

	//	Is in fighting state ?
	virtual bool IsFighting() { return m_bFight; }
	void SetFightFlag(bool bTrue) { m_bFight = bTrue; }
	//	Check whether player has effect of specified type
	bool HasEffectType(int iEffType);
	float GetPortraitCameraScale() { return m_vPortraitCamScale; }
	bool IsChangingFace() const { return m_bIsChangingFace; }
	
	//	Get move speed
	float GetFlySpeed() { return m_ExtProps.mv.flight_speed; }
	float GetSwimSpeed() { return m_ExtProps.mv.swim_speed; }
//	float GetGroundSpeed() { return m_bWalkRun ? m_ExtProps.mv.run_speed : m_ExtProps.mv.walk_speed; }
	float GetGroundSpeed();

	CECModel *	GetPlayerModel()		{ return m_pPlayerModel; }
	CECModel *	GetDummyModel(int i)	{ return (i!=PLAYERMODEL_MAJOR&&i<PLAYERMODEL_MAX) ? m_pModels[i]:NULL; }
	CECModel *	GetMajorModel()			{ return m_pModels[PLAYERMODEL_MAJOR]; }
	CECModel *	GetPetModel()			{ return m_pPetModel; }
	CECFace *	GetFaceModel()			{ return m_pFaceModel; }
	CECSprite *	GetSpriteModel()		{ return m_pSprite; }
	bool		HasWingModel()const;
	CECGoblin * GetGoblinModel()		{ return m_pGoblin;	}
	bool		IsCurrentModel(CECModel *pModel){ return GetPlayerModel() != NULL && pModel == GetPlayerModel(); }
	bool		IsMajorModel(CECModel *pModel){ return GetMajorModel() != NULL && pModel == GetMajorModel(); }

	void SetGoblinRenderCnt(int iCnt);
	void SetRenderGoblin(bool bRender);
	bool GetRenderGoblin()				{ return m_bRenderGoblin; }

	/*
	 *	Add By Zhangyu, 12.22.04
	 */
	void ShowEquipments(const int* pEquipmentID, bool bLoadAtOnce = false, bool bForceLoad = false);
	void ChangeEquipDisableMask(__int64 Mask);
	//	Show / hide wing
	void ShowWing(bool bShow);
	//	Show / hide weapn
	void ShowWeapon(bool bShow);
	void ShowWeaponByConfig(const PLAYER_ACTION_INFO_CONFIG *p);
	enumWingType GetWingType() const { return m_wingType; }
	bool UsingWing() const { return m_wingType == WINGTYPE_WING; }

	static enumWingType FlyMode2WingType(unsigned int flymode);

	//	Get id of full suite
	int GetFullSuiteID() { return m_idFullSuite; }

	//	Initialize static resources
	static bool InitStaticRes();
	//	Release static resources
	static void ReleaseStaticRes();
	//	Build action list
	static void BuildActionList();

	//	Load player model
	static bool LoadPlayerModel(int iProfession, int iGender, int iCustom, const int* pEquips, const char* szPetPath, EC_PLAYERLOADRESULT& Ret /* out */,
		bool bSimpleFace = false, bool bSimpleModel = false);
	//	Load player equips
	static bool LoadPlayerEquips(int iProf, int iGender, __int64 Mask, const int* pEquips, EquipsLoadResult& Ret /* out */, bool bSimpleModel = false);
	//	Load pet
	static bool LoadPetModel(const char* szPetPath, CECModel** ppPetModel /* out */);
	//	Load dummy model
	static bool LoadDummyModel(int iShape, CECModel** ppDummyModel /* out */);

	//	Release player model
	static void ReleasePlayerModel(EC_PLAYERLOADRESULT& Ret);
	//	Get exp of specified level
	static int GetLevelUpExp(int iLevel);
	//	Load body skin, static func
	static bool LoadBodySkin(
		int nBodyID,
		int iProfession,
		int iGender,
		A3DSkin* aSkins[NUM_SKIN_INDEX][3],
		A3DShader* pBodyShaders[3],
		bool bSimpleModel = false);

	static CECFace* ThreadLoadFaceModel(int nCurCharacter, int nCurGender, int nFaceID);

	//	Build riding pet file name
	static const char* GetRidingPetFileName(int idPet);

	bool IsClothesOn();	
	void SetCurSkillTarget(int idTarget)  { m_idCurSkillTarget = idTarget; };

	void SetNamePos(A3DVECTOR3 vPos) { m_vNamePos = vPos; };

	bool PKLevelCheck() const { return m_BasicProps.iLevel < 30;}

	//修真等级改变时判断是否为仙魔转换的情况	
	bool IsGodEvilConvert(int originalLevel2, int newLevel2);
	//针对self_info_00与OnMsgPlayerLevel2对Level2修真等级修改无逻辑先后顺序，提供Level2修改接口
	void SetLevel2(int level2, bool bFirstTime);
	//播放修真提升效果
	void PlayTaoistEffect();
	bool CanPlayTaoistEffect(int originalLevel2, int newLevel2, bool bFirstTime);	
	bool IsPlayingAction()const;
	bool IsPlayingAction(int iAction)const;
	bool IsPlayingCastingSkillAction()const;
	bool IsPlayingMoveAction()const;
	bool IsPlayingCastingSkillAndMoveActions()const;

public:	// For customizing

	void SetBodyColor(A3DCOLOR color);
	void SetBodyHeadScale(unsigned char vScale);
	void SetBodyUpScale(unsigned char vScale);
	void SetBodyWaistScale(unsigned char vScale);
	void SetBodyArmWidth(unsigned char vScale);
	void SetBodyLegWidth(unsigned char vScale);
	void SetBodyBreastScale(unsigned char vScale);

	// 更新形象的默认个性化数据
	bool SaveAsDefaultCustomizeData();
	// 载入形象的默认个性化数据
	static bool LoadDefaultCustomizeData(int prof, int gender, PLAYER_CUSTOMIZEDATA &data);

	bool LoadCustomizeFactorFromIni(void);

	bool ChangeCustomizeData(const PLAYER_CUSTOMIZEDATA& data, bool bApply=true);
	PLAYER_CUSTOMIZEDATA& GetCustomizeData() { return m_CustomizeData; }
	
	// 验证和修正时装头巾被用来替换默认头发的BUG
	static bool ValidateCustomizeData(int nProf, int nGender, PLAYER_CUSTOMIZEDATA &data);

	void StoreCustomizeData();
	void RestoreCustomizeData();

	//把比例从int 转换到float
	float TransformScaleFromIntToFloat(int nScale, float fScaleFactor, float fMax);
	
	//获取个性化限制
	//CECCustomizeBound* GetCustomizeBound(void) const { return m_pCustomizeBound;};

	//吃变形丸
	bool EatChgPill(const char* pszPillFile, float vCamScale);
	
	//设置变形丸数据
	bool SetChgPillData(int nID, int nData);

	//备份个性化数据(与变形丸相关)
	void BackupCustomizeData(void);

	//还原个性化数据(与变形丸相关)
	bool DiscardChgPill(void);

	void UpdateBodyScales();

	//更新头发模型及贴图(与头盔模型显示相关)
	void UpdateHairModel(bool bUpdateAll, int iArmetID = 0);
	
	AString GetFashionActionName();
	bool ChangeFashionColor(int equipSlot, unsigned short newColor);

	// 获取经脉属性
	const MeridiansProp& GetMeridiansProp()const{ return m_meridiansProp;}
	// 更新经脉属性
	void SetMeridiansProp(const MeridiansProp& meridianProp);
	//
	void SetCurrentTitle(unsigned short id){m_TitleID = id;};
	unsigned short GetCurrentTitle()const{return m_TitleID;}

	void SetReincarnationCount(unsigned char times) { m_ReincarnationCount = times; }
	unsigned char GetReincarnationCount()const{ return m_ReincarnationCount; }
	void SetRealmLevel(unsigned char level) { m_RealmLevel = level; }
	unsigned char GetRealmLevel()const{ return m_RealmLevel; }
	int	GetRealmLayer()const {return GetRealmLayer(m_RealmLevel); }
	int GetRealmSubLevel()const{ return GetRealmSubLevel(m_RealmLevel); }
	static int GetRealmLayer(int realmLevel){return realmLevel ? (realmLevel + 9) / 10 : 0;}
	static int GetRealmSubLevel(int realmLevel){return realmLevel ? (realmLevel % 10 ? realmLevel % 10 : 10) : 0;}
	// 人物变大缩小
	virtual void ScaleBody(float fScale);
	float GetScaleBySkill()const{ return m_fScaleBySkill; }
	void StartMonsterSpiritConnectGfx(int mine_id, A3DVECTOR3 pos);
	void StopMonsterSpiritConnectGfx();
	void StartMonsterSpiritBallGfx();
	void UpdateMonsterSpiritGfx(DWORD dwDeltaTime);
	void RenderMonsterSpiritGfx();

protected:

	static void ChangeDefaultUpper(
		A3DSkin* aSkins[3],
		int nProf,
		int nGender
		)
	{
		char szPath[MAX_PATH];
		_GenDefaultSkinPath(szPath, nProf, nGender, "默认上衣");
		ChangeArmor(szPath, aSkins, enumSkinShowUpperBody, true, nProf, nGender);
	}
	static void ChangeDefaultLower(
		A3DSkin* aSkins[3],
		int nProf,
		int nGender
		)
	{
		char szPath[MAX_PATH];
		_GenDefaultSkinPath(szPath, nProf, nGender, "默认裤子");
		ChangeArmor(szPath, aSkins, enumSkinShowLowerBody, true, nProf, nGender);
	}
	static void ChangeDefaultWrist(
		A3DSkin* aSkins[3],
		int nProf,
		int nGender
		)
	{
		char szPath[MAX_PATH];
		_GenDefaultSkinPath(szPath, nProf, nGender, "默认护腕");
		ChangeArmor(szPath, aSkins, enumSkinShowWrist, true, nProf, nGender);
	}
	static void ChangeDefaultFoot(
		A3DSkin* aSkins[3],
		int nProf,
		int nGender
		)
	{
		char szPath[MAX_PATH];
		_GenDefaultSkinPath(szPath, nProf, nGender, "默认靴子");
		ChangeArmor(szPath, aSkins, enumSkinShowFoot, true, nProf, nGender);
	}

	static void ChangeDefaultFashionUpper(
		A3DSkin* aSkins[3],
		int nProf,
		int nGender
		)
	{
		if( nGender == GENDER_MALE )
			return;
		char szPath[MAX_PATH];
		_GenSkinPath(szPath, nProf, nGender, "时装默认上衣");
		ChangeFashion(szPath, aSkins, enumSkinShowUpperBody, true, nProf, nGender, 0xffffffff);
	}
	static void ChangeDefaultFashionLower(
		A3DSkin* aSkins[3],
		int nProf,
		int nGender
		)
	{
		char szPath[MAX_PATH];
		_GenSkinPath(szPath, nProf, nGender, "时装默认裤子");
		ChangeFashion(szPath, aSkins, enumSkinShowLowerBody, true, nProf, nGender, 0xffffffff);
	}
	static void ChangeDefaultFashionWrist(
		A3DSkin* aSkins[3],
		int nProf,
		int nGender
		)
	{
		return;
		char szPath[MAX_PATH];
		_GenSkinPath(szPath, nProf, nGender, "时装默认护腕");
		ChangeFashion(szPath, aSkins, enumSkinShowWrist, true, nProf, nGender, 0xffffffff);
	}
	static void ChangeDefaultFashionFoot(
		A3DSkin* aSkins[3],
		int nProf,
		int nGender
		)
	{
		if( nGender == GENDER_MALE )
			return;
		char szPath[MAX_PATH];
		_GenSkinPath(szPath, nProf, nGender, "时装默认鞋子");
		ChangeFashion(szPath, aSkins, enumSkinShowFoot, true, nProf, nGender, 0xffffffff);
	}

protected:	//	Attributes
	PLAYER_ACTION*	m_PlayerActions;	// Action List
	
	PLAYER_CUSTOMIZEDATA	m_CustomizeData;		//	customize data for this player
	PLAYER_CUSTOMIZEDATA	m_OldCustomizeData;		//	old customize data for this player
	PLAYER_CUSTOMIZE_FACTOR m_CustomizeFactor;		//  Customize Factor
	//CECCustomizeBound		*m_pCustomizeBound;		//	Customize Bound

	bool					m_bShowWeapon;			//	current weapon show flag.

	CECChangePill			m_ChangePill;			//	Change Pill
	PLAYER_CUSTOMIZEDATA	m_ChgPllCustomizeData;	//	Change Pill customize data
	bool					m_bIsChangingFace;
	float					m_vPortraitCamScale;	//	portrait camera scale when using change pill
	DWORD					m_aExtStates[OBJECT_EXT_STATE_COUNT];	//	Visible extend states from server
	DWORD					m_aExtStatesShown[OBJECT_EXT_STATE_COUNT];	//	Visible extend states currently shown

	S2C::IconStates			m_aIconStates;			//	Icon states (un-visible extend states)
	AArray<int, int>		m_aCurEffects;			//	Current effects

	CECPlayerMan*	m_pPlayerMan;		//	Player manager
	CECModel*		m_pPlayerModel;		//	Player model
	CECFace*		m_pFaceModel;		//	Player face model
	CECModel*		m_pModels[PLAYERMODEL_MAX];	//	Player's transformed and normal models
	int				m_aShapeID[PLAYERMODEL_MAX];//	Player's model id
	CECModel*		m_pLeftHandWeapon;
	CECModel*		m_pRightHandWeapon;
	bool			m_bWeaponAttached;
	CECModel*		m_pPetModel;		//	Pet model
	CECPlayerActionController *	m_pActionController;
	CECPlayerBodyController	*	m_pBodyController;

	CECCounter		m_GoblinRenderCnt;	//  Render goblin or sprite counter 
	bool			m_bRenderGoblin;	//  true, current rendering goblin

	CECSprite *		m_pSprite;			//	sprite model
	CECGoblin *		m_pGoblin;			//  Goblin model

	A3DSkin*		m_aSkins[NUM_SKIN_INDEX][3];//	Player's skins, managed by player
	A3DSkin*		m_aCurSkins[NUM_SKIN_INDEX];//	currently used skins

	A3DShader*		m_pBodyShader[3];	//	Player's body skin shader
	INFO			m_PlayerInfo;		//	Player information got from server
	ROLEBASICPROP	m_BasicProps;		//	Basic properties
	ROLEEXTPROP		m_ExtProps;			//	Extend properties
	int				m_idSelTarget;		//	选中目标的ID
	int				m_iMoneyCnt;		//	Money count
	int				m_iMaxMoney;		//	Maximum money value
	int				m_aEquips[SIZE_ALL_EQUIPIVTR];	//	Equipment item ID array
	__int64			m_i64EquipDisabled;	//	Disable specific equipments (bit = 1 means disabled)
	int				m_iReputation;		//	Player's reputation
	int				m_iShape;			//	Player's shape
	float			m_fDistToCamera;	//	Distance to camera
	bool			m_bInSanctuary;		//	true, player is in sanctuary
	bool			m_bPetInSanctuary;	//  true, the pet pet of the player is in sanctuary
	PVPINFO			m_pvp;				//	pvp information
	int				m_iBoothState;		//	Booth state. 0, none; 1, prepare; 2, open booth; 3, visite other's booth
	int				m_crcBooth;			//	Booth crc
	bool			m_bFashionMode;		//	true, in fashion mode
	unsigned char	m_factionPVPMask;	//	pvp mask
	int				m_idFaction;		//	ID of player's faction
	int				m_idFRole;			//	ID of player's faction role
	RIDINGPET		m_RidingPet;		//	Riding pet information
	int				m_iBattleCamp;		//	Battle this player belongs to
	DWORD			m_dwGMFlags;		//	GM flags
	AArray<int, int> m_aBattleTanks;	//	Battle tanks controlled by this player
	int				m_idSpouse;			//	id of spouse
	int				m_idForce;			//	id of the player's force
	int				m_idCountry;		//	国战阵营 id
	
	typedef abase::hash_map<MULTIOBJECT_EFFECT, A3DGFXEx*, _mo_hash_function> MOEffectMAP;
	MOEffectMAP m_mapMOEffect;			//   Multi Object Effect

	ACString		m_strName;			//	Player name
	int				m_iProfession;		//	Profession
	int				m_iGender;			//	Gender

	int				m_iMoveMode;		//	Player's move mode
	int				m_iMoveEnv;			//	Move environment
	bool			m_bWalkRun;			//	Walk-run switch, 0-walk, 1-run
	bool			m_bRushFly;			//	true, in rush fly mode
	MOVECONST		m_MoveConst;		//	Const used when moving control

	A3DAABB			m_aabbServer;		//  与服务器保持一致的aabb， 不受缩放影响
	A3DAABB			m_aabb;				//	Player's aabb，用于显示的aabb，受缩放影响
	DWORD			m_dwStates;			//	Player's basic states
	DWORD			m_dwStates2;		//  Player's basic states 2
	bool			m_bCastShadow;		//	flag indicates whether it will cast shadow
	float			m_fTouchRad;		//	Touch radius
	bool			m_bRenderName;		//	Render name flag
	bool			m_bRenderBar;		//	Render HP, MP, EXP bars
	DWORD			m_dwResFlags;		//	Resources ready flag
	bool			m_bFight;			//	true, Is fighting
	TEAMREQ			m_TeamReq;			//	Team requirment
	PATECONTENT		m_PateContent;		//	Pate content
	BYTE			m_byPariahLvl;		//	Pariah level

	CECTeam*		m_pTeam;			//	The team to which player belongs
	A3DGFXEx*		m_pLevelUpGFX;		//	Level up gfx
	A3DGFXEx*		m_pWaterWaveStill;
	A3DGFXEx*		m_pWaterWaveMove;
	A3DGFXEx*		m_pTransformGfx;
	A3DGFXEx*		m_pAirBubble;
	A3DGFXEx*		m_pSwimBubble;
	CECSkill*		m_pCurSkill;		//	Current used skill
	int				m_idCurSkillTarget;	//	Current skill target id
	A3DGFXEx*		m_pDuelStateGFX;	//	a gfx standing for in duel state.
	A3DGFXEx*		m_pPetCureGFX;		//	人冶疗宠的位于人宠之间的特殊电弧光效
	int				m_pPetCureGFXtate;	//	与m_pPetCureGFX对应的state下标
	A3DGFXEx*		m_pMonsterSpiritGFX[2]; // 元魂特效，第一个为连线特效，另一个为球特效
	int				m_iMonsterSpiritMineID;
	A3DVECTOR3		m_posMonsterSpirit;
	enum BALL_STATE{
		BALL_STATE_NONE,
		BALL_STATE_SHOW,
		BALL_STATE_RISING,
		BALL_STATE_FOLLOW,
		BALL_STATE_DISAPPER,
	};
	BALL_STATE		m_stateMonsterSpirit;

	CECPateText*	m_pPateName;		//	Player name
	CECPateText*	m_pPateMarry;		//	Title
	CECPateText*	m_pPateLastWords1;	//	The words player said last time, line 1
	CECPateText*	m_pPateLastWords2;	//	The words player said last time, line 2
	CECCounter		m_strLastSayCnt;	//	Time counter of last said words
	CECPateText*	m_pPateTeamReq;		//	Team requirement
	CECPateText*	m_pPateBooth;		//	Booth name
	CECPateText*	m_pPateFaction;		//	Faction name
	CECSpriteDecal*	m_pFactionDecal;	//	Faction icon
	ACString		m_strTeamReq;		//	Team requirement string
	ACString		m_strBoothName;		//	Booth name string
	CECPateText*	m_pPateForce;		//	Force name
	CECPateText*	m_pPateCountry;		//	Country name
	CECPateText*	m_pPateTitle;		//	Title
	CECSpriteDecal*	m_pCountryDecal;	//	Country icon

	CECCounter		m_TransCnt;			//	Transparent counter
	float			m_fCurTrans;
	float			m_fDstTrans;
	float			m_fTransDelta;

	// stones effects.
	WORD			m_stoneUpperBody;	//	stone effects on upper body
	WORD			m_stoneWrist;		//	stone effects on wrist
	WORD			m_stoneLowerBody;	//	stone effects on lower body
	WORD			m_stoneFoot;		//	stone effects on foot
	WORD			m_stoneWeapon;		//	stone effects on weapon
	int				m_idFullSuite;		//	id of the full suite

	WORD			m_stoneUpperBodyShown;
	WORD			m_stoneWristShown;
	WORD			m_stoneLowerBodyShown;
	WORD			m_stoneFootShown;
	WORD			m_stoneWeaponShown;	//	m_stoneWeapon 经 CECOptimize 优化处理后的结果
	int				m_idFullSuiteShown;
	
	CECBubbleDecalList*	m_pBubbleTexts;	//	Bubble text list

	// 当前玩家的内存使用状态
	int				m_iMemUsage;
	
	// 下身的装配方法
	int				m_nLowerEquipMethod;
	// 时装下身的装配方法
	int				m_nLowerFashionEquipMethod;
	// 当前攻击方式
	unsigned int	m_uAttackType;
	// 当前时装武器类型
	int				m_iFashionWeaponType;
	// 翅膀或飞剑或风火轮
	enumWingType	m_wingType;
	// 已收到并处理了deadly_strike标志为true的attack_result
	bool			m_bAboutToDie;
	// 依附类型
	AttachMode		m_AttachMode;
	// 是否是依附者
	bool			m_bHangerOn;
	// 依附者或被依附者id
	int				m_iBuddyId;
	int				m_idCandBuddy;		//	ID of candidate buddy
	bool			m_bCandHangerOn;
	RIDINGPET		m_CandPet;
	
	//  当前召唤出的宠物的ID
	int				m_idCurPet;	
	//  骑乘状态玩家姓名的调整
	A3DVECTOR3		m_vNamePos;

	//  摆摊模型
	CECModel *	m_pBoothModel;
	int			m_iBoothModelCertificateID;
	bool		m_bBoothModelLoaded;

	//	摆摊标题
	int			m_iBoothBarCertificateID;
		
	//  The gfx info that playing on player
	abase::hash_map<AString, GFXRECORD> m_GfxRecords;

	//	武器悬挂位置
	enum WeaponHangerPosition
	{
		WEAPON_HANGER_HAND,			//	武器握在手上
		WEAPON_HANGER_SHOULDER,		//	武器背在背上
	};
	WeaponHangerPosition m_weaponHangerPos;	//	当前武器悬挂位置
	// 普通武器模型
	AString			m_strLeftWeapon;
	AString			m_strRightWeapon;
	// 时装武器模型
	AString			m_strLeftFashionWeapon;
	AString			m_strRightFashionWeapon;
	// 经脉属性
	MeridiansProp   m_meridiansProp;
	// 当前称号
	unsigned short	m_TitleID;
	// 转生次数
	unsigned char	m_ReincarnationCount;
	// 境界等级
	unsigned char	m_RealmLevel;
	// 技能缩放
	float			m_fScaleBySkill;
	// 记录技能id，播放击中、倒地动作
	int				m_SkillIDForStateAction;
	// 用来播放动作的特殊状态表, 为了避免策划出错，把状态id写死；如果策划要添加新状态修改此数组即可
	static const int m_sciStateIDForStateAction[1];

	//	获取人身上的武器挂点名称（依赖当前武器类型和悬挂点位置）
	const char *GetLeftWeaponHookPos(WeaponHangerPosition);
	const char *GetRightWeaponHookPos(WeaponHangerPosition);

	//	获取与人对应的武器上的挂点名称（直接依赖指定武器模型）
	const char *GetLeftWeaponOwnHookPos(CECModel *);
	const char *GetRightWeaponOwnHookPos(CECModel *);
	const char *GetWeaponOwnHookPos(CECModel *, bool bLeft);

	//	获取武器上GFX的挂点
	const char *GetLeftWeaponGFXHookPos(CECModel *);
	const char *GetRightWeaponGFXHookPos(CECModel *);
	const char *GetWeaponGFXHookPos(CECModel *, bool bLeft);

	//	获取当前武器
	CECModel * GetLeftHandWeapon();
	CECModel * GetRightHandWeapon();
	void DetachWeapon();
	bool AttachWeapon();
	void ReleaseWeapon();
	bool IsWeaponAttached()const;

	//	判断给定挂点名称是否用于悬挂武器，并返回对应武器模型
	bool IsWeaponHookPos(const char *szHH, bool *pbLeft = NULL, CECModel **ppWeaponModel = NULL);

	//	根据要执行的技能或动作更新（或维持）当前武器悬挂点
	bool SetWeaponHangerPos(WeaponHangerPosition);
	void UpdateWeaponHangerPosBySkillAction(int idSkill);
	void UpdateWeaponHangerPosByAction(int iAction);

	//	判断给定武器是否为法宝武器
	bool IsMagicWeapon(CECModel *);

	static const FASHION_WEAPON_CONFIG* GetFashionConfig();
	bool CanShowFashionWeapon(int weapon_type, int fashion_weapon_type);
	void DecideWeaponLoad(int* pEquipmentID, __int64& Mask);
	void SetWeaponResult(EquipsLoadResult& Result);
	void OnSwitchFashionWeapon();
	static int GetWeaponType(int iWeaponType){ return iWeaponType == DEFAULT_ACTION_TYPE ? 10 : iWeaponType; }
	int GetShowingWeaponType();
	bool IsShowFashionWeapon();

	void RecreateActionController();
	void RecreateBodyController();

protected:	//	Operations
	
	void InitCustomizeFactor();
	
	//	When all resources are ready, this function will be called
	virtual void OnAllResourceReady();
	//	Apply effect on player
	virtual bool ApplyEffect(int iEffect, bool bCheckArray);
	//	Discard effect from player
	virtual void DiscardEffect(int iEffect);
	
	//	Search the full suite
	int SearchFullSuite();

	//	Load body skin
	bool LoadBodySkin(int nBodyID, bool bSimpleModel = false);

	//	Load host's skeleton, all the data should be prepared already
	bool LoadPlayerSkeleton(bool bAtOnce);

	bool QueueLoadEquips(const int* pEquips, __int64 Mask, bool bAtOnce);
	bool QueueLoadPetModel(const char* szPetPath, bool bAtOnce);
	bool QueueLoadDummyModel(int iShape, bool bAtOnce);
	bool QueueLoadFace(bool bAtOnce);


	//	Update current skins
	bool UpdateCurSkins();
	//	Update God Evil sprite
	bool UpdateGodEvilSprite();
	//  Update Goblin
	bool UpdateGoblin();

	//  Render goblin or sprite
	void RenderGoblinOrSprite(CECViewport* pViewport);

	//	Release player model
	void ReleasePlayerModel();

	bool LoadFaceModel(int nCurCharacter, int nCurGender, int nFaceID);
	void ReleaseFaceModel();
	void AttachFaceModel();
	void SetFaceModel(CECFace *pFaceModel);

	bool PlayNonSkillActionWithName(int iAction, const char* szActName, bool bRestart=true, int nTransTime=200, bool bNoFx=false, bool *pActFlag=NULL, DWORD dwFlagMode=COMACT_FLAG_MODE_NONE);
	bool QueueNonSkillActionWithName(int iAction, const char* szActName, int nTransTime=200, bool bForceStopPrevAct=false, bool bNoFx=false, bool bResetSpeed=false, bool bResetActFlag=false, bool *pNewActFlag=NULL, DWORD dwNewFlagMode=COMACT_FLAG_MODE_NONE);	
	bool PlaySkillCastActionWithName(int idSkill, const char *szActName, bool bNoFX=false);
	bool PlaySkillAttackActionWithName(int idSkill, const char *szActName, bool bNoFX=false, bool *pActFlag=NULL, DWORD dwFlagMode=COMACT_FLAG_MODE_NONE);
	bool QueueSkillAttackActionWithName(int idSkill, const char *szActName, int nTransTime=200, bool bNoFX=false, bool bResetSpeed=false, bool bResetActFlag=false, bool *pNewActFlag=NULL, DWORD dwNewFlagMode=COMACT_FLAG_MODE_NONE);
	bool PlayWoundActionWithName(const char* szActName);
	void ClearComActFlagAllRankNodes(bool bSignalCurrent);
	void StopChannelAction();
	void StopSkillCastAction();
	void StopSkillAttackAction();
	int  GetLowerBodyAction()const;
	// Play model action by weapon and relative action index
	bool PlayAction(int iAction, bool bRestart=true, int iTransTime=200, bool bQueue=false);
	bool PlayActionWithConfig(int iAction, int actionConfigID, bool bRestart=true, int iTransTime=200, bool bQueue=false);
	bool PlayActionWithConfig(int iAction, const PLAYER_ACTION &actionConfig, bool bRestart=true, int iTransTime=200, bool bQueue=false);
	// Play common attack action
	bool PlayAttackAction(int nAttackSpeed, int* piAttackTime=NULL, bool *pActFlag=NULL);
	// Play skill cast action
	bool PlaySkillCastAction(int idSkill);
	// Play skill attack action
	bool PlaySkillAttackAction(int idSkill, int nAttackSpeed, int* piAttackTime=NULL,int nSection = 0, bool *pActFlag=NULL);
	// Play start use item action
	bool PlayStartUseItemAction(int tid);
	// 播放采集元魂动作
	bool PlayGatherMonsterSpiritAction();
	// Play use item effect
	bool PlayUseItemEffect(int tid, const void* pData = NULL, size_t sz = 0);
	//	Caculate player's AABB base on profession and gender
	void CalcPlayerAABB();
	//	Get move or stand action
	int GetMoveStandAction(bool bMove, bool bFight=false)const;
	static bool IsMoveStandAction(int iAction);
	//	Print bubble text
	void BubbleText(int iIndex, DWORD dwNum, int p1=0);
	//	Fill pate content
	bool FillPateContent(CECViewport* pViewport);
	//	Check water moving environment
	bool CheckWaterMoveEnv(const A3DVECTOR3& vPos, float fWaterHei, float fGndHei);

	//	Set resources ready flag
	void SetResReadyFlag(DWORD dwFlag, bool bSet);
	//	Render titles / names / talks above player's header
	bool RenderName(CECViewport* pViewport, DWORD dwFlags);
	//	Render bar above player's header
	bool RenderBars(CECViewport* pViewport);
	//	Render booth name
	int RenderBoothName(CECViewport* pViewport, int y);
	//	Set new visible extend states
	void SetNewExtendStates(int start, const DWORD* pData, int count);
	void ClearShowExtendStates();
	void ShowExtendStates(int start, const DWORD* pData, int count, bool bIgnoreOptimize=false);
	//	将当前的特效状态，更新到当前的武器上（可能是新加载的武器）
	void SetExtendStatesToWeapon();
	//	Decompress advertisement data
	void DecompAdvData(int iData1, int iData2);
	//  Set player's shape
	void SetShape(int iShape);
	//  Play Gfx on models
	bool PlayGfx(const char* szPath, const char* szHook, float fScale = 1.0f, unsigned int iShapeTypeMask = (1<<PLAYERMODEL_MAJOR), bool bForceNoRecord=false);
	//  Remove Gfx on models
	void RemoveGfx(const char* szPath, const char* szHook, unsigned int iShapeTypeMask = (1<<PLAYERMODEL_MAJOR));
	
	float GetEquipGfxScale();
	void RemoveEquipGfx();
	void AddEquipGfx();

	void RemoveUpperBodyStones();
	void RemoveWristStones();
	void RemoveLowerBodyStones();
	void RemoveFootStones();
	void RemoveWeaponStones();
	void RemoveFullSuiteGFX();

	void AddUpperBodyStones();
	void AddWristStones();
	void AddLowerBodyStones();
	void AddFootStones();
	void AddWeaponStones();
	void AddFullSuiteGFX();

	AString GetWeaponStoneGfx(WORD status);
	AString GetArmorStoneGfx(WORD status, int nEquipIndex, int nEquipParam0 = 0);
	AString GetSharpenerGfx(WORD status);
	
	//	Message handlers
	virtual void OnMsgPlayerExtState(const ECMSG& Msg);
	virtual void OnMsgEnchantResult(const ECMSG& Msg);
	virtual void OnMsgPlayerAdvData(const ECMSG& Msg);
	virtual void OnMsgPlayerPVP(const ECMSG& Msg);
	virtual void OnMsgSwitchFashionMode(const ECMSG& Msg);
	virtual void OnMsgPlayerEffect(const ECMSG& Msg);
	virtual void OnMsgChangeNameColor(const ECMSG& Msg);
	virtual void OnMsgPlayerMount(const ECMSG& Msg);
	virtual void OnMsgPlayerLevel2(const ECMSG& Msg);
	virtual void OnMsgKingChanged(const ECMSG &Msg);

	//	Get distance to camera
	float GetDistToCamera() { return m_fDistToCamera; }
	// Does an equipment need to show
	bool IsShownEquip(int nEquipIndex);
	// Get real element id
	DWORD GetRealElementID(int nEquipIndex, DWORD dwEquipID);

	// Replace current skin in A3DSkinModel
	void ReplaceCurSkin(int nSkinIndex, A3DSkin *pNewSkin);

	//	宠物治疗 GFX 相关
	
	//	判断是否为宠物相关路径
	bool IsPetCureGFX(const AString &strGFXFile);

	//	判断是否为宠物相关路径并加载
	bool TestProcessPetCureGFX(const AString &strGFXFile, bool bLoad, int iState);

	//	根据玩家当前与宠物的状态更新宠物治疗 GFX
	void UpdatePetCureGFX(DWORD dwDeltaTime);

	//	显示宠物治疗 GFX
	void RenderPetCureGFX();

	//	调整当前透明度
	bool StartAdjustTransparency(float fCur, float fDest, DWORD dwTime);
	float UpdateTransparency(DWORD dwDeltaTime);

	//	根据当前状态决定透明度最大值
	float GetTransparentLimit();

	//  multi object effect
	void AddMultiObjectEffect(int idTarget,char cType);
	void RemoveMultiObjectEffect(int idTarget,char cType);
	void UpdateMultiObjectEffect(DWORD dwDeltaTime);
	void UpdateOneMultiObjectEffect(int idTarget,A3DGFXEx* pGfx,DWORD dwDeltaTime);
	void RenderMultiObjectGFX();
	A3DBone* ScaleRootBone(CECModel* pModel, float scale);
	void ScaleChildModel();

	virtual bool OnDamaged(int skill); // 在头顶冒字的时候记录技能id，这样尽可能保证动作和冒字、光效同步
	virtual void DoSkillStateAction(){}; // 根据技能和状态来播放击中和倒地动作
	void PlayEnterBattleGfx();
		
	CECModel * LoadBoothModel(const AString path);
	void SetBoothModel(const char *idBoothModel);
	void ClearBoothModel();
	void UpdateBoothModel();

	void SetBoothBar(const char* szFile);
	void ClearBoothBar();
	void UpdateBoothBar();

public:
	int		 HasSkillStateForAction(); // 判断特殊状态

	//	Play a damaged effect
	void Damaged(int nDamage, DWORD dwModifier=0,int skill = 0);

	//	Play an attack effect
	void PlayAttackEffect(int idTarget, int idSkill, int skillLevel, int nDamage, DWORD dwModifier, int nAttackSpeed, int* piAttackTime=NULL, int nSection = 0);

	// 变身
	bool IsShapeChanged() const { return m_iShape != 0; } // identify logic shape changing
	void TransformShape(int iShape, bool bLoadAtOnce=false);
	
	bool IsShapeModelReady() const; // whether the transformed model is ready
	bool IsShapeModelChanged() const; // identify render model changing
	void ApplyShapeModelChange(CECModel* pModel);
	bool ShouldUseGroundNormalForCurrentShapeModel()const;

	// 依附关系
	virtual void AttachBuddy(int iBuddy);
	virtual void DetachBuddy(CECPlayer* pBuddy = NULL, bool bResetData = true);
	AttachMode GetAttachMode() const { return m_AttachMode; }

	// 骑乘
	virtual void RideOnPet(int id, unsigned short color);
	virtual void GetOffPet(bool bResetData = true);
	bool IsRidingOnPet() { return m_RidingPet.id ? true : false; }
	//	Get riding pet info.
	const RIDINGPET& GetRidingPetInfo()const{ return m_RidingPet; } 
	void SetRidingPetColor(unsigned short clr);
	static void SetRidingPetColor(CECModel *pModel, A3DCOLOR clr);

	//	Do emote action
	virtual bool DoEmote(int idEmote) { return true; }

	virtual bool IsPlayerMoving() { return false; }
	virtual bool IsWorkMoveRunning()const{ return false; }
	virtual bool IsWorkSpellRunning()const{ return false; }

	bool IsHangerOn() const { return m_bHangerOn; }
	
	bool GetWeaponMajorType(unsigned int &id_major_type);
	bool IsUsingMagicWeapon();
	bool IsUsingOboroWeapon();
	bool IsUsingLongKnifeWeapon();
	bool IsUsingSickleWeapon();

	int GetWeaponID()	{ return m_aEquips[EQUIPIVTR_WEAPON] & 0xffff; }
	virtual int GetCertificateID()const { return m_aEquips[EQUIPIVTR_CERTIFICATE] & 0xffff; }
	CECModel * GetBoothModel();

	void UpdatePosRelatedGFX(DWORD dwDeltaTime);
	void LoadMonsterSpiritGFX(int index, int res_index);
	
	void ClearBubbleText();

	A3DAABB GetShadowAABB();	//	返回用于计算阴影的AABB

	void OptimizeShowExtendStates();
	void OptimizeWeaponStoneGfx();
	void OptimizeArmorStoneGfx();
	void OptimizeSuiteGfx();

	// 获取当前的内存使用状态
	int GetMemUsage() const { return m_iMemUsage; }
	bool ShouldUseFaceModel()const;
	bool ShouldUseClothedModel()const;
	bool ShouldUseModel()const;
	bool ShouldUseBoothModel()const;
	void SetMemUsage(int iMemUsage) { m_iMemUsage = iMemUsage; }

	// static funcs
	static bool ChangeWing		(EquipsLoadResult* pResult, const char* szModeFile/*, bool bWing*/, const char* szModeFile2 = NULL);
	static bool ChangeWeapon	(EquipsLoadResult* pResult, const char* szLeft, const char* szRight);
	static void ChangeWeapon	(EquipsLoadResult* pResult, const WEAPON_ESSENCE* pWeapon);
	static bool LoadPlayerSkin	(A3DSkin* aSkins[3], int iIndex, const char* szFile);
	static bool ChangeArmor		(const char* strSkinFile, A3DSkin* aSkins[3], int nLocation, bool bDefault, int nProf, int nGender);
	static void ChangeArmor		(const ARMOR_ESSENCE* pArmor, DWORD& dwShowMask, DWORD& dwSkinChangeMask, int nProf, int nGender, A3DSkin* pSkins[][3]);
	static bool ChangeFashion	(const char* strSkinFile, A3DSkin* aSkins[3], int nLocation, bool bDefault, int nProf, int nGender, A3DCOLOR color);
	static void ChangeFashion	(const FASHION_ESSENCE* pFashion, DWORD& dwShowMask, DWORD& dwSkinChangeMask, int nProf, int nGender, A3DSkin* pSkins[][3], A3DCOLOR color);
	static void ShowEquipments	(
		int nProf,
		int nGender,
		const int* pEquipmentID,
		__int64 ChangeMask,
		EquipsLoadResult* pResult,
		bool bSimpleModel = false
		);
	static const TITLE_CONFIG* GetTitleConfig(unsigned short id);
	bool IsInChariot();
	
	static bool IsFashionWeaponTypeFit(int weapon_type, int fashion_weapon_type);

	int GetEffectCount()const{ return m_aCurEffects.GetSize();}
	int GetEffect(int i)const{ return m_aCurEffects.GetAt(i);}
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

bool PlayerRenderForShadow(A3DViewport * pViewport, void * pArg);
void PlayerRenderPortrait(const A3DRECT &rc, DWORD param1, DWORD param2, DWORD param3);
void PlayerRenderPortraitNoFace(const A3DRECT &rc, DWORD param1, DWORD param2, DWORD param3);
void PlayerRenderDemonstration(const A3DRECT &rc, DWORD param1, DWORD param2, DWORD param3);
