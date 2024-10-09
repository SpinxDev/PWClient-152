/*
 * FILE: EC_GPDataType.h
 *
 * DESCRIPTION: Game protocol data type
 *
 * CREATED BY: CuiMing, Duyuxin, 2004/9/9
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include "EC_RoleTypes.h"
#include <vector.h>
#include <A3DVector.h>
///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

#define ISPLAYERID(id)	((id) && !((id) & 0x80000000))
#define ISNPCID(id)		(((id) & 0x80000000) && !((id) & 0x40000000))
#define ISMATTERID(id)	(((id) & 0xC0000000) == 0xC0000000)

#define ISMONEYTID(tid) ((tid) == 3044)

//	Move mode
enum
{
	GP_MOVE_WALK	= 0,
	GP_MOVE_RUN		= 1,
	GP_MOVE_STAND	= 2,
	GP_MOVE_FALL	= 3,
	GP_MOVE_SLIDE	= 4,
	GP_MOVE_PUSH	= 5,		// only sent to NPC
	GP_MOVE_FLYFALL	= 6,
	GP_MOVE_RETURN	= 7,
	GP_MOVE_JUMP	= 8,
	GP_MOVE_PULL	= 9,		// only sent to NPC
	GP_MOVE_BLINK	= 10,		// only sent to NPC（瞬移）
	GP_MOVE_MASK	= 0x0f,

	GP_MOVE_TURN	= 0x10,		//	Turnaround
	GP_MOVE_DEAD	= 0x20,

	GP_MOVE_AIR		= 0x40,
	GP_MOVE_WATER	= 0x80,
	GP_MOVE_ENVMASK	= 0xc0,
};

//	Leave team reason
enum
{
	GP_LTR_LEAVE = 0,		//	Normal leave
	GP_LTR_KICKEDOUT,		//	Was kicked out
	GP_LTR_LEADERCANCEL,	//	Leader cancel the team
};

//	Team member pickup flag
enum
{
	GP_TMPU_RANDOM = 0,
	GP_TMPU_FREEDOM,
};

//	NPC service type
enum
{
	GP_NPCSEV_SELL = 1,			//	1, NPC sell to player
	GP_NPCSEV_BUY,				//	NPC buy from player
	GP_NPCSEV_REPAIR,
	GP_NPCSEV_HEAL,
	GP_NPCSEV_TRANSMIT,			//	Transmit to somewhere
	
	GP_NPCSEV_TASK_RETURN,		//	6, Return task
	GP_NPCSEV_TASK_ACCEPT,		//	Accept task
	GP_NPCSEV_TASK_MATTER,		//	Task matter
	GP_NPCSEV_LEARN,			//	Learn skill
	GP_NPCSEV_EMBED,			//	Embed stone

	GP_NPCSEV_CLEAR_TESSERA,	//	11, Clear tessear
	GP_NPCSEV_MAKEITEM,
	GP_NPCSEV_BREAKITEM,
	GP_NPCSEV_TRASHPSW,			//	Change trash password
	GP_NPCSEV_OPENTRASH,		//	Open trash

	GP_NPCSEV_RESERVED,			//	16
	GP_NPCSEV_IDENTIFY,			//	Identify item
	GP_NPCSEV_FACTION,			//	About faction
	GP_NPCSEV_BOOTHSELL,		//	Player booth sell		
	GP_NPCSEV_TRAVEL,			//	Travel

	GP_NPCSEV_BOOTHBUY,			//	21, Player booth buy
	GP_NPCSEV_WAYPOINT,
	GP_NPCSEV_FORGETSKILL,
	GP_NPCSEV_FACECHANGE,
	GP_NPCSEV_MAIL,

	GP_NPCSEV_VENDUE,			//	26
	GP_NPCSEV_DBLEXPTIME,
	GP_NPCSEV_HATCHPET,
	GP_NPCSEV_RESTOREPET,
	GP_NPCSEV_BATTLE,
	
	GP_NPCSEV_BUILDTOWER,		//	31
	GP_NPCSEV_LEAVEBATTLE,
	GP_NPCSEV_RETURNSTATUSPT,
	GP_NPCSEV_ACCOUNTPOINT,
	GP_NPCSEV_REFINE,
	
	GP_NPCSEV_PETNAME,			//	36, change pet name
	GP_NPCSEV_PETSKILL_DEL,		//	Delete pet skill
	GP_NPCSEV_PETSKILL_LEARN,	//	Learn pet skill
	GP_NPCSEV_BIND_ITEM,		//	Bind item
	GP_NPCSEV_DESTROY_BIND,		//	Destroy bind item

	CP_NPCSEV_CANCEL_DESTROY,	//	41, Cancel destroy bind item
	GP_NPCSEV_STOCK_TRANSACTION,
	GP_NPCSEV_STOCK_OPERATION,
	GP_NPCSEV_DYE,
	GP_NPCSEV_REFINE_TRANS,

	GP_NPCSEV_COMPOSE,			//	46
	GP_NPCSEV_MAKE_SLOT,
	GP_NPCSEV_GOBLIN_RETURNSTATUSPT,	// Return goblin status point
	GP_NPCSEV_GOBLIN_RETURNGENIUSPT,	// Return goblin genius point
	GP_NPCSEV_GOBLINSKILL_LEARN,		// Learn goblin skill

	GP_NPCSEV_GOBLINSKILL_DEL,	//  51, delete goblin skill
	GP_NPCSEV_GOBLIN_REFINE,			//  Refine goblin
	GP_NPCSEV_GOBLIN_REFINETRANSFER,	//  Transfer refine level
	GP_NPCSEV_GOBLIN_DESTROY,			//  Destroy goblin
	GP_NPCSEV_GOBLINEQUIP_DESTROY,		//  Destroy goblin's equipment
	
	GP_NPCSEV_DYE_BY_SUIT,		//	56, Dye by suit
	GP_NPCSEV_REPAIR_DESTROYING_ITEM,	//  Repair destroying item
	GP_NPCSEV_LEVELUP_PRODUCE,			//  Level up produce
	GP_NPCSEV_OPEN_ACCOUNT_BOX,			//  Open account box
	GP_NPCSEV_WEBTRADE,					//	Web trade

	GP_NPCSEV_GODEVILCONVERT,	//	61	Convert between god and evil
	GP_NPCSEV_WEDDING_BOOK,				//	结婚预订服务
	GP_NPCSEV_WEDDING_INVITE,			//	填写请柬服务
	GP_NPCSEV_FACTION_FORTRESS_SERVICE_1,	//	帮派基地服务1（开启、宣战）
	GP_NPCSEV_FACTION_FORTRESS_SERVICE_2,	//	帮派基地服务2（基地升级、设施升级、提交材料、提交贡献度）

	GP_NPCSEV_FACTION_FORTRESS_SERVICE_3,	//	66	帮派基地材料转换服务
	GP_NPCSEV_PET_DYE,
	GP_NPCSEV_VIEW_TRASHBOX,					// check the trash box without password
	GP_NPCSEV_ENGRAVE,					//	镌刻
	GP_NCPSEV_DPS_DPH_RANK,		//	沙包排行榜

	GP_NPCSEV_ADDONREGEN,		//	71 重新生成随机属性
	GP_NPCSEV_FORCE,			//	势力NPC服务
	GP_NPCSEV_TRANSMIT_DIRECT,	//	NPC 直接传送
	GP_NPCSEV_PREVIEW_PRODUCE,  //  可预览生产
	GP_NPCSEV_COUNTRY_JOINLEAVE,//	国战进退逻辑

	GP_NPCSEV_COUNTRY_LEAVEWAR,	//	76 国战离开当前战场
	GP_NPCSEV_MARK,
	GP_NPCSEV_CROSSSERVER_GETIN,
	GP_NPCSEV_CROSSSERVER_GETOUT,

	GP_NPCSEV_PLAYER_RENAME,    // 80 玩家改名服务
	GP_NPCSEV_STONE_TRANSFER,	// 魂石转化
	GP_NPCSEV_STONE_REPLACE,	// 魂石替换
	GP_NPCSEV_KINGSEV,			// 国王相关服务
	GP_NPCSEV_SPLIT_FASHION,	// 时装拆分

	GP_NPCSEV_OFFLINESHOP,		// 离线商店(寄卖服务)
	GP_NPCSEV_REINCARNATION,
	GP_NPCSEV_GIFTCARD,

	GP_NPCSEV_TRICKBATTLE,		// 88, 跨服活动
	GP_NPCSEV_CARDRESPAWN,		// 卡牌转生
	GP_NPCSEV_FLYSWORDIMPROVE,  // 飞剑强化
	GP_NPCSEV_OPEN_FACTION_PVP,	//	开启帮派掠夺战

	GP_NPCSEV_ADVANCED_PRODUCE, //92,新生产服务5
	GP_NPCSEV_GOLD_SHOP,		//	NPC 元宝商城
	GP_NPCSEV_DIVIDEND_GOLD_SHOP,//	NPC 鸿利元宝商城
	GP_NPCSEV_PLAYER_CHANGE_GENDER,// NPC 修改性别
};

//	Player and NPC state
enum
{
    GP_STATE_SHAPE				= 0x00000001,
	GP_STATE_EMOTE				= 0x00000002,
	GP_STATE_INVADER			= 0x00000004,
    GP_STATE_SITDOWN			= 0x00000020,
    GP_STATE_EXTEND_PROPERTY	= 0x00000040,
    GP_STATE_CORPSE				= 0x00000080,

	//	Used only by player
	GP_STATE_PARIAH				= 0x00000008,
    GP_STATE_FLY				= 0x00000010,
	GP_STATE_TEAM				= 0x00000100,
	GP_STATE_TEAMLEADER			= 0x00000200,
	GP_STATE_ADV_MODE			= 0x00000400,
	GP_STATE_FACTION			= 0x00000800,
	GP_STATE_BOOTH				= 0x00001000,
	GP_STATE_FASHION			= 0x00002000,
	GP_STATE_GMFLAG				= 0x00004000,
	GP_STATE_PVPFLAG			= 0x00008000,
	GP_STATE_EFFECT				= 0x00010000,
	GP_STATE_INPVPCOMBAT		= 0x00020000,
	GP_STATE_IN_DUEL			= 0x00040000,	//	是否正在决斗中
	GP_STATE_IN_MOUNT			= 0x00080000,	//	正在骑乘中
	GP_STATE_IN_BIND			= 0x00100000,	//	和别人绑在一起
	GP_STATE_BC_INVADER			= 0x00200000,	//	Battle camp: invader
	GP_STATE_BC_DEFENDER		= 0x00400000,	//	Battle camp: defender
	GP_STATE_SPOUSE				= 0x00800000,	//	配偶id
	GP_STATE_GOBLINREFINE		= 0x01000000,	//  小精灵是否已经激活
	GP_STATE_SHIELDUSER			= 0x02000000,	//  是否完美神盾用户
	GP_STATE_INVISIBLE			= 0x04000000,	//  隐身
	GP_STATE_EQUIPDISABLED		= 0x08000000,	//  Equipment disabled
	GP_STATE_FORBIDBESELECTED	= 0x10000000,	//	禁止被选中 (NPC也有效)
	GP_STATE_PLAYERFORCE		= 0x20000000,	//	已加入势力
	GP_STATE_MULTIOBJ_EFFECT	= 0x40000000,	//	与其他对象存在特殊效果
	GP_STATE_COUNTRY             = 0x80000000,		//已加入国家

	//	Used only by NPC
	GP_STATE_NPC_DELAYDEAD		= 0x00000008,
	GP_STATE_NPC_ADDON1			= 0x00000100,
	GP_STATE_NPC_ADDON2			= 0x00000200,
	GP_STATE_NPC_ADDON3			= 0x00000400,
	GP_STATE_NPC_ADDON4			= 0x00000800,
	GP_STATE_NPC_ALLADDON		= 0x00000F00,
	GP_STATE_NPC_PET			= 0x00001000,	//	Pet flag
	GP_STATE_NPC_NAME			= 0x00002000,
	GP_STATE_NPC_FIXDIR			= 0x00004000,	//	方向固定
	GP_STATE_NPC_MAFIA			= 0x00008000,	//	所属帮派（用于帮派PVP中矿车等）
	GP_STATE_NPC_FLY			= 0x00010000,
	GP_STATE_NPC_SWIM			= 0x00020000,
};

//  Player and NPC state2
enum
{
	GP_STATE2_ISKING			= 0x00000001,	//  玩家是否国王
	GP_STATE2_TITLE				= 0x00000002,   //	称号
	GP_STATE2_REINCARNATION		= 0x00000004,	//	转生
	GP_STATE2_REALM				= 0x00000008,	//	境界
	GP_STATE2_IN_BATTLE			= 0x00000010,	//	战斗状态
	GP_STATE2_FACTION_PVP_MASK	= 0X00000020,	//	帮派 PVP Mask
	GP_STATE2_GENDER			= 0x00000040,	//	性别
};

//	Chat channel
enum
{
	//这个枚举定义如果改动的话，要通知所有人，包括服务器程序员
	//如非必要，尽可能在后面添加，避免删除和更改前面channel的顺序
	GP_CHAT_LOCAL = 0,
	GP_CHAT_FARCRY,
	GP_CHAT_TEAM,
	GP_CHAT_FACTION,
	GP_CHAT_WHISPER,
	GP_CHAT_DAMAGE,
	GP_CHAT_FIGHT,
	GP_CHAT_TRADE,
	GP_CHAT_SYSTEM,
	GP_CHAT_BROADCAST,
	GP_CHAT_MISC,
	GP_CHAT_INSTANCE,
	GP_CHAT_SUPERFARCRY,
	GP_CHAT_BATTLE,
	GP_CHAT_COUNTRY,
	GP_CHAT_MAX,
};

//	The reason of dropping item
enum
{
	GP_DROP_GM,
	GP_DROP_PLAYER,
	GP_DROP_TAKEOUT,
	GP_DROP_TASK,
	GP_DROP_RECHARGE,
	GP_DROP_DESTROY,
	GP_DROP_DEATH,
	GP_DROP_PRODUCE,
	GP_DROP_DECOMPOSE,
	GP_DROP_TRADEAWAY,
	GP_DROP_RESURRECT,
	GP_DROP_USED,
	GP_DROP_CONSUMERUNE,
	GP_DROP_EXPIRED,
};

//	Cool time index
enum
{
	GP_CT_NULL = 0,					//	空，保留
	GP_CT_PVP,                     	//	没用，先占位
	GP_CT_EMOTE,                   	//	做表情的冷却时间 
	GP_CT_REJUVENATION_POTION,     	//	九阳丹的冷却时间
	GP_CT_SWITCH_FASHION,          	//	切换时装模式的冷却时间
	GP_CT_DROP_MONEY,              	//	往地上扔钱的冷却时间
	GP_CT_DROP_ITEM,               	//	往地上扔物品的冷却时间
	GP_CT_FACEPILL,                	//	变形丸的冷却时间
	GP_CT_FACETICKET,              	//	整容的冷却时间
	GP_CT_RECURRECT_SCROLL,        	//	别人复活卷的冷却时间，此物品不存在，未生效
	GP_CT_SOUL_STONE,              	//	现在复活卷轴的冷却时间
	GP_CT_HP_POTION,				//	回血药
	GP_CT_MP_POTION,				//	回魔药
	GP_CT_ANTIDOTE_POTION,			//	解毒剂
	GP_CT_FLY_RUSH,					//	加速飞行
	GP_CT_TOWNSCROLL,				//	回城卷轴
	GP_CT_GM_GENITEM,				//	GM 生成怪物的物品
	GP_CT_VIEWOTHEREQUIP,			//	查看别人的装备
	GP_CT_FEED_PET,					//	喂养宠物
	GP_CT_FIREWORKS,				//	施放烟花
	GP_CT_FARCRY,					//	far cry 频道说话
	GP_CT_SKILLMATTER,				//	使用技能物品
	GP_CT_REFINE,					//	精炼
	GP_CT_ARMORRUNE,				//	防御优化符
	GP_CT_AUTOHP,					//	护身符冷却时间
	GP_CT_AUTOMP,					//	守神符冷却时间
	GP_CT_DOUBLEEXP,				//	双倍经验道具冷却时间
	GP_CT_DYETICKET,				//	染色道具
	GP_CT_TEAMRELATION,				//	组队关系冷却时间
	GP_CT_REFINETRANS,				//	精炼等级转移冷却时间
	
	GP_CT_CAST_ELF_SKILL,			//  小精灵技能冷却
	GP_CT_ELF_CMD,					//  先占位
	GP_CT_GET_MALL_PRICE,			//  获取商城数据的冷却时间
	GP_CT_QUERY_OTHER_PROPERTY,		//  查询目标属性 5秒
	GP_CT_SKILLTRIGGER2,			//	使用技能物品1秒

	GP_CT_SKILLCOMMONCOOLDOWN0,		//  用于技能公共冷却
	GP_CT_SKILLCOMMONCOOLDOWN1,
	GP_CT_SKILLCOMMONCOOLDOWN2,
	GP_CT_SKILLCOMMONCOOLDOWN3,
	GP_CT_SKILLCOMMONCOOLDOWN4,

	GP_CT_RUNECOMMONCOOLDOWN0,		//  用于物品技能公共冷却
	GP_CT_RUNECOMMONCOOLDOWN1,
	GP_CT_RUNECOMMONCOOLDOWN2,
	GP_CT_RUNECOMMONCOOLDOWN3,
	GP_CT_RUNECOMMONCOOLDOWN4,

	GP_CT_EQUIP_FASHION_ITEM,		//	自动换装冷却时间 60s
	GP_CT_GET_DIVIDEND_MALL_PRICE,	//  获取鸿利商城数据的冷却时间 30s
	GP_CT_MULTI_EXCHANGE_ITEM,      //  整理包裹冷却时间 30s

	GP_CT_TEAM_CONGREGATE,			//	队伍集结令
	GP_CT_FACTION_CONGREGATE,		//	帮派集结令
	GP_CT_DPS_DPH_RANK,				//	获取沙包排行榜
	GP_CT_JOIN_PLAYER_FORCE,		//	加入势力
	GP_CT_LEAVE_PLAYER_FORCE,		//	离开势力
	GP_CT_TOGGLE_ONLINE_AWARD,		//	切换挂机岛在线奖励
	GP_CT_COUNTRY_BATTLE_APPLY,		//	加入国战申请
	GP_CT_COUNTRY_CHAT,				//	国战频道发言
	GP_CT_CROSS_SERVER_APPLY,		//	前往跨服申请

	GP_CT_TOUCHTRADE,				//	Touch 点购物或查询
	GP_CT_SWITCH_PARALLEL_WORLD,	//	切换分线
	GP_CT_QUERY_PARALLEL_WORLD,		//	查询分线

	GP_CT_GIFTCARD_REDEEM,
    GP_CT_TRICKBATTLE_APPLY,		//  跨服活动, 战车
	GP_CT_AUTOTEAM,					//  自动组队操作，地图跳转
	GP_CT_PLAYER_GATHER,			//	防止采集过快 (GATHER_MATERIAL)
	GP_CT_COUNTRYBATTLE_LIVESHOW,	//	查询国战rank和死亡数据 (COUNTRYBATTLE_LIVE_SHOW)

	GP_CT_QUERY_MAFIA_PVP_INFO,		//	65: 帮派 PVP 信息查询 (QUERY_MAFIA_PVP_INFO)

	GP_CT_MAX,
	GP_CT_SKILL_START = 1024,
};

//	PVP mask
enum
{
	GP_PVPMASK_FORCE		= 0x0001, // 强力攻击
	GP_PVPMASK_NOMAFIA		= 0x0002,
	GP_PVPMASK_NOWHITE		= 0x0004,
	GP_PVPMASK_NOALLIANCE	= 0x0008,
	GP_PVPMASK_NOFORCE		= 0x0010,//不攻击同势力的
	
	GP_BLSMASK_NORED		= 0x0008,
	GP_BLSMASK_NOMAFIA		= 0x0010,
	GP_BLSMASK_SELF			= 0x0020,
	GP_BLSMASK_NOALLIANCE	= 0x0040,
	GP_BLSMASK_NOFORCE		= 0x0080, // 势力屏蔽
};

//	Refuse Bless Mask
enum REFUSE_BLESS_MASK
{
	REFUSE_NEUTRAL_BLESS = 0x0001,				//	不接受中性祝福
	REFUSE_NON_TEAMMATE_BLESS = 0x0002,	//	不接受非队友祝福
};

enum
{
	GP_PET_SKILL_NUM	= 8
};

enum
{
	GP_PET_NATURE_SKILL_NUM = 2
};

enum
{
	GP_PET_SPECIAL_SKILL_NUM = 1 // 专属技能，目前只有进化宠有一个此技能
};

//	Pet type
enum
{   
	GP_PET_CLASS_INVALID = -1,
	GP_PET_CLASS_MOUNT = 0, //	骑宠
	GP_PET_CLASS_COMBAT,    //	战斗宠物
	GP_PET_CLASS_FOLLOW,    //	跟随宠物
	GP_PET_CLASS_SUMMON,	//	召唤宠物
	GP_PET_CLASS_PLANT,		//  植物，只是为了和服务器的值保持一致，客户端没有用到
	GP_PET_CLASS_EVOLUTION, //	进化宠
	GP_PET_CLASS_MAX,
};

//	Player camp in battle
enum
{
	GP_BATTLE_CAMP_NONE = 0,
	GP_BATTLE_CAMP_INVADER,
	GP_BATTLE_CAMP_DEFENDER,
};

//   Elf cmd
enum
{   
    ELF_LEVELUP =0,
    ELF_LEARN_SKILL,
    ELF_FORGET_SKILL,
    ELF_REFINE,
	ELF_DECOMPOSE,
	ELF_DEC_ATTRIBUTE,
	ELF_ADD_GENIUS,
	ELF_EQUIP_ITEM,
	ELF_DESTROY_ITEM,
	ELF_RECHARGE,
};

//	帮派基地
enum
{
	RT_LEVEL_UP = 1,		//	基地升级
	RT_SET_TECH_POINT,		//	科技升级，参数0：要升级哪个科技
	RT_CONSTRUCT,			//	设施升级，参数0：设施id，参数1：建造加速值（0不加速，1加速10%,...）
	RT_HAND_IN_MATERIAL,	//	材料上缴，参数0：材料在包中索引，参数1：材料id，参数2：上缴数量
	RT_HAND_IN_CONTRIB,		//	贡献度上缴，参数0：贡献度数量
	RT_LEAVE_FORTRESS,		//	离开基地
	RT_DISMANTLE,			//	销毁基地设施
	RT_RESET_TECH_POINT,	//  重置科技点
	RT_MAX
};

// 集结令类型
enum
{
    CONGREGATE_TYPE_TEAM,
    CONGREGATE_TYPE_FACTION,
	CONGREGATE_TYPE_TEAM_MEMBER,
};

// 收益时间级别
enum
{
	PROFIT_LEVEL_NONE,		// 无收益时间
	PROFIT_LEVEL_YELLOW,	// 收益地图且收益时间小于1小时
	PROFIT_LEVEL_NORMAL,	// 有收益
};

enum
{
    PLAYER_PVP_CLIENT,      //客户端主动操作,开启或关闭pvp状态(适用于PVE服务器)
	PLAYER_PVP_PROTECTED,   //玩家进入或脱离新手保护区域
	PLAYER_PVP_LEVELUP,		//升级导致的pvp状态
};

//	随机商城操作类型
enum
{
	RAND_MALL_OP_QUERY,		//请求随机商品结果数据
	RAND_MALL_OP_ROLL,		//开始随机商城商品随机
	RAND_MALL_OP_PAY,		//确认支付随机商城商品
};

enum
{
	OBJECT_EXT_STATE_COUNT = 6,	//	玩家/NPC 身上状态光效 DWORD 个数
};

//	随机商城操作结果
#define RAND_MALL_SHOPPING_RES_OK					0		//	操作成功
#define RAND_MALL_SHOPPING_RES_INVALID_CONFIG		-1		//	配置ID非法
#define RAND_MALL_SHOPPING_RES_INVALID_ROLE_STATE	-2		//	角色状态不适合执行当前操作
#define RAND_MALL_SHOPPING_RES_INVALID_OP			-3		//	请求操作不符合既定逻辑（如反复请求随机）
#define RAND_MALL_SHOPPING_RES_CASH_NOT_ENOUGH		-4		//	元宝不足
#define RAND_MALL_SHOPPING_RES_PACK_FULL			-5		//	背包空间不足

///////////////////////////////////////////////////////////////////////////
//	
//	Reference to External variables and functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local Types and Variables and Global variables
//	
///////////////////////////////////////////////////////////////////////////

#pragma pack(1)

namespace S2C
{
	typedef unsigned char byte;

	//	提取压缩数据辅助模板类
	template <typename T>
	bool Extract(T &ret, const byte *&pDataBuf, DWORD &dwDataSize)
	{
		//	提取单个数据类型
		if (dwDataSize >= sizeof(T))
		{
			ret = *(const T*)pDataBuf;
			pDataBuf += sizeof(T);
			dwDataSize -= sizeof(T);
			return true;
		}
		return false;
	}

	template <typename T>
	bool Extract(T *pRet, int count, const byte *&pDataBuf, DWORD &dwDataSize)
	{
		//	提取数组
		if (count >=0)
		{
			DWORD s = sizeof(T)*count;
			if (dwDataSize >= s)
			{
				::memcpy(pRet, pDataBuf, s);
				pDataBuf += s;
				dwDataSize -= s;
				return true;
			}
		}
		return false;
	}

	//	Data type ---------------------------
	struct info_player_1
	{
		int cid;
		A3DVECTOR3 pos;
		unsigned short crc_e;
		unsigned short crc_c;
		unsigned char dir;		//256的定点数表示度数
		unsigned char level2;
		int state;
		int state2;
		//	if (state & GP_STATE_ADV_MODE), 2 DWORD inserted here
		//	if (state & GP_STATE_SHAPE), 1 char inserted here
		//	if (state & GP_STATE_EMOTE), 1 char inserted here
		//	If (state & GP_STATE_EXTEND_PROPERTY), OBJECT_EXT_STATE_COUNT DWORD inserted here
		//	If (state & GP_STATE_FACTION), 1 int + 1 char inserted here
		//	If (state & GP_STATE_BOOTH), 1 char inserted here
		//	If (state & GP_STATE_EFFECT), effect data inserted here
		//	If (state & GP_STATE_PARIAH), 1 char inserted here
		//	If (state & GP_STATE_IN_MOUNT), 1 char + 1 int inserted here
		//	If (state & GP_STATE_IN_BIND), 1 char + 1 int inserted here
		//	If (state & GP_STATE_SPOUSE), 1 int inserted here

		bool CheckValid(size_t buf_size, size_t& sz) const
		{
			if (buf_size < sizeof(*this))
				return false;

			sz = sizeof(*this);

			if (state & GP_STATE_ADV_MODE)
				sz += sizeof(DWORD) * 2;

			if (state & GP_STATE_SHAPE)
				sz += sizeof(char);

			if (state & GP_STATE_EMOTE)
				sz += sizeof(char);

			if (state & GP_STATE_EXTEND_PROPERTY)
				sz += sizeof(DWORD) * OBJECT_EXT_STATE_COUNT;
		
			if (state & GP_STATE_FACTION)
				sz += sizeof(int) + sizeof(char);
			
			if (state & GP_STATE_BOOTH)
				sz += sizeof(char);

			if (state & GP_STATE_EFFECT)
			{
				const BYTE* pData = reinterpret_cast<const BYTE*>(this) + sz;
				sz += sizeof(BYTE);

				if (buf_size < sz)
					return false;

				sz += *pData * sizeof(short);
			}

			if (state & GP_STATE_PARIAH)
				sz += sizeof(char);

			if (state & GP_STATE_IN_MOUNT)
				sz += sizeof (unsigned short) + sizeof (int);

			if (state & GP_STATE_IN_BIND)
				sz += sizeof (char) + sizeof (int);

			if (state & GP_STATE_SPOUSE)
				sz += sizeof(int);

			if (state & GP_STATE_EQUIPDISABLED)
				sz += sizeof(__int64);

			if (state & GP_STATE_PLAYERFORCE)
				sz += sizeof(int);

			if (state & GP_STATE_MULTIOBJ_EFFECT)
			{
				const BYTE* pData = reinterpret_cast<const BYTE*>(this) + sz;
				sz += sizeof(int);

				if (buf_size < sz)
					return false;

				sz += *(int *)pData * (sizeof(int) + sizeof(char));
			}

			if (state & GP_STATE_COUNTRY)
				sz += sizeof(int);
			if (state2 & GP_STATE2_TITLE)
				sz += sizeof(unsigned short);
			if (state2 & GP_STATE2_REINCARNATION)
				sz += sizeof(unsigned char);
			if (state2 & GP_STATE2_REALM)
				sz += sizeof(unsigned char);
			if (state2 & GP_STATE2_FACTION_PVP_MASK)
				sz += sizeof(unsigned char);

			return buf_size >= sz;
		}
		unsigned char GetGender()const{
			return (state2 & GP_STATE2_GENDER) ? GENDER_FEMALE : GENDER_MALE;
		}
	};

	struct info_player_2		//name ,customize data 不常改变
	{
		unsigned char size;
		char data[1];
	};

	struct info_player_3		//viewdata of equipments and special effect
	{
		unsigned char size;
		char  data[1];
	};

	struct info_player_4		//detail
	{
		unsigned short size;
		char	data[1];
	};

	struct info_npc
	{
		int nid;
		int tid;			//template id
		int vis_tid;		//
		A3DVECTOR3 pos;
		unsigned short seed;		// seed of customize data
		unsigned char dir;
		int state;
		int state2;
		//	If (state & GP_STATE_EXTEND_PROPERTY), OBJECT_EXT_STATE_COUNT DWORD inserted here
		//	If (state & GP_STATE_NPC_PET), 1 int inserted here
		//	If (state & GP_STATE_NPC_NAME), 1 BYTE + name string here

		bool CheckValid(size_t buf_size, size_t& sz) const
		{
			if (buf_size < sizeof(*this))
				return false;

			sz = sizeof(*this);
			
			if (state & GP_STATE_EXTEND_PROPERTY)
				sz += sizeof(DWORD) * OBJECT_EXT_STATE_COUNT;
			
			if (state & GP_STATE_NPC_PET)
				sz += sizeof (int);

			if (state & GP_STATE_NPC_NAME)
			{
				const BYTE* pData = reinterpret_cast<const BYTE*>(this) + sz;
				BYTE len = *(BYTE*)pData;
				sz += sizeof (BYTE) + len;
			}

			if (state & GP_STATE_MULTIOBJ_EFFECT)
			{
				const BYTE* pData = reinterpret_cast<const BYTE*>(this) + sz;
				sz += sizeof(int);

				if (buf_size < sz)
					return false;

				sz += *(int *)pData * (sizeof(int) + sizeof(char));
			}
			if (state & GP_STATE_NPC_MAFIA){
				sz += sizeof (int);
			}

			return buf_size >= sz;
		}
	};

	struct info_matter
	{
		int mid;
		int tid;
		A3DVECTOR3 pos;
		unsigned char dir0;
        unsigned char dir1;
        unsigned char rad;
        unsigned char state;
		unsigned char value;	//	Maybe 'scale' or something others
	};

	struct info_pet
	{
		int honor_point;        	//	好感度
		int hunger;					//	饥饿度
		int feed_time;				//	上次喂养到现在的时间
		int pet_tid;            	//	宠物的模板ID
		int pet_vis_tid;        	//	宠物的可见ID（如果为0，则表示无特殊可见ID）
		int pet_egg_tid;        	//	宠物蛋的ID
		int pet_class;          	//	宠物类型 战宠，骑宠，观赏宠
		float hp_factor;        	//	血量比例（复活和收回时使用） 0则为死亡
		short level;            	//	宠物级别
		unsigned short color;	//	宠物颜色，最高位为1表示有效，目前仅对骑宠有效
		int exp;                	//	宠物当前经验
		int skill_point;        	//	剩余技能点
		char is_bind;				//	是否天人合一，现在是一个Mask，0x01 天人合一，0x02 寻宝网可交易
		char unused;			//   目前此项无效
		unsigned short name_len;	//	名字长度 目前此项无效（因为策划尚无名字需求）
		char name[16];          	//	名字内容

		struct
		{
			int skill;
			int level;

		} skills [GP_PET_SKILL_NUM];

		struct _evo_prop
		{
			int r_attack;
			int r_defense;
			int r_hp;
			int r_atk_lvl;
			int r_def_lvl;
			int nature;
		} evo_prop;

		int reserved[10]; // 未用
	};

	//	Commands ----------------------------

	enum	//	Command ID
	{
		PROTOCOL_COMMAND = -1,	//	Reserved for protocol
		
		PLAYER_INFO_1 = 0,
		PLAYER_INFO_2,
		PLAYER_INFO_3,
		PLAYER_INFO_4,
		PLAYER_INFO_1_LIST,

		PLAYER_INFO_2_LIST,		//	5
		PLAYER_INFO_3_LIST,
		PLAYER_INFO_23_LIST,
		SELF_INFO_1,
		NPC_INFO_LIST,

		MATTER_INFO_LIST,		//	10
		NPC_ENTER_SLICE,
		PLAYER_ENTER_SLICE,
		OBJECT_LEAVE_SLICE,
		NOTIFY_HOSTPOS,

		OBJECT_MOVE,			//	15
		NPC_ENTER_WORLD,
		PLAYER_ENTER_WORLD,
		MATTER_ENTER_WORLD,
		PLAYER_LEAVE_WORLD,
		
		NPC_DIED,				//	20
		OBJECT_DISAPPEAR,
		OBJECT_STARTATTACK,		//	已作废
		HOST_STOPATTACK,
		HOST_ATTACKRESULT,

		ERROR_MESSAGE,			//	25
		HOST_ATTACKED,
		PLAYER_DIED,
		HOST_DIED,
		PLAYER_REVIVE,

		PICKUP_MONEY,			//	30
		PICKUP_ITEM,
		PLAYER_INFO_00,
		NPC_INFO_00,
		OUT_OF_SIGHT_LIST,
		
		OBJECT_STOP_MOVE,		//	35
		RECEIVE_EXP,
		LEVEL_UP,
		SELF_INFO_00,
		UNSELECT,

		OWN_ITEM_INFO,			//	40, Own item information
		EMPTY_ITEM_SLOT,		//	Empty item slot notify
		OWN_IVTR_DATA,			//	Own inventory data
		OWN_IVTR_DETAIL_DATA,
		EXG_IVTR_ITEM,

		MOVE_IVTR_ITEM,			//	45
		PLAYER_DROP_ITEM,
		EXG_EQUIP_ITEM,
		EQUIP_ITEM,
		MOVE_EQUIP_ITEM,

		OWN_EXT_PROP,			//	50
		ADD_STATUS_POINT,
		SELECT_TARGET,
		PLAYER_EXT_PROP_BASE,
		PLAYER_EXT_PROP_MOVE,

		PLAYER_EXT_PROP_ATK,	//	55
		PLAYER_EXT_PROP_DEF,
		TEAM_LEADER_INVITE,
		TEAM_REJECT_INVITE,
		TEAM_JOIN_TEAM,

		TEAM_MEMBER_LEAVE,		//	60
		TEAM_LEAVE_PARTY,
		TEAM_NEW_MEMBER,
		TEAM_LEADER_CACEL_PARTY,
		TEAM_MEMBER_DATA,

		TEAM_MEMBER_POS,		//	65
		EQUIP_DATA,				//	用户的装备数据，影响表现
		EQUIP_DATA_CHANGED,
		EQUIP_DAMAGED,			//	装备损坏
		TEAM_MEMBER_PICKUP,		//	队友捡起装备

		NPC_GREETING,			//	70
		NPC_SERVICE_CONTENT,
		PURCHASE_ITEM,
		ITEM_TO_MONEY,
		REPAIR_ALL,

		REPAIR,					//	75
		RENEW,
		SPEND_MONEY,
		GAIN_MONEY_IN_TRADE,
		GAIN_ITEM_IN_TRADE,

		GAIN_MONEY_AFTER_TRADE,	//	80
		GAIN_ITEM_AFTER_TRADE,
		GET_OWN_MONEY,
		ATTACK_ONCE,
		HOST_START_ATTACK,

		OBJECT_CAST_SKILL,		//	85
		SKILL_INTERRUPTED,
		SELF_SKILL_INTERRUPTED,
		SKILL_PERFORM,
		OBJECT_BE_ATTACKED,		//	已作废

		SKILL_DATA,				//	90
		HOST_USE_ITEM,
		EMBED_ITEM,
		CLEAR_TESSERA,
		COST_SKILL_POINT,

		LEARN_SKILL,			//	95
		OBJECT_TAKEOFF,
		OBJECT_LANDING,
		FLYSWORD_TIME,
		HOST_OBTAIN_ITEM,

		PRODUCE_START,			//	100
		PRODUCE_ONCE,
		PRODUCE_END,
		DECOMPOSE_START,
		DECOMPOSE_END,

		TASK_DATA,				//	105
		TASK_VAR_DATA,
		OBJECT_START_USE,
		OBJECT_CANCEL_USE,
		OBJECT_USE_ITEM,

		OBJECT_START_USE_T,		//	110
		OBJECT_SIT_DOWN,
		OBJECT_STAND_UP,
		OBJECT_DO_EMOTE,
		SERVER_TIME,

		OBJECT_ROOT,			//	115
		HOST_DISPEL_ROOT,
		INVADER_RISE,
		PARIAH_RISE,
		INVADER_FADE,

		OBJECT_ATTACK_RESULT,	//	120
		BE_HURT,
		HURT_RESULT,
		HOST_STOP_SKILL,
		UPDATE_EXT_STATE,

		ICON_STATE_NOTIFY,		//	125
		PLAYER_GATHER_START,
		PLAYER_GATHER_STOP,
		TRASHBOX_PWD_CHANGED,
		TRASHBOX_PWD_STATE,

		TRASHBOX_OPEN,			//	130
		TRASHBOX_CLOSE, 
		TRASHBOX_WEALTH,
		EXG_TRASHBOX_ITEM,
		MOVE_TRASHBOX_ITEM,

		EXG_TRASHBOX_IVTR,		//	135
		IVTR_ITEM_TO_TRASH,
		TRASH_ITEM_TO_IVTR,
		EXG_TRASH_MONEY,
		ENCHANT_RESULT,
		
		HOST_NOTIFY_ROOT,		//	140
		OBJECT_DO_ACTION,
		HOST_SKILL_ATTACK_RESULT,
		OBJECT_SKILL_ATTACK_RESULT,
		HOST_SKILL_ATTACKED,

		PLAYER_SET_ADV_DATA,	//	145
		PLAYER_CLR_ADV_DATA,
		PLAYER_IN_TEAM,
		TEAM_ASK_TO_JOIN,
		OBJECT_EMOTE_RESTORE,

		CON_EMOTE_REQUEST,		//	150, concurrent emote request
		DO_CONCURRENT_EMOTE,
		MATTER_PICKUP,
		MAFIA_INFO_NOTIFY,
		MAFIA_TRADE_START,

		MAFIA_TRADE_END,		//	155
		TASK_DELIVER_ITEM,
		TASK_DELIVER_REP,
		TASK_DELIVER_EXP,
		TASK_DELIVER_MONEY,

		TASK_DELIVER_LEVEL2,	//	160
		HOST_REPUTATION,
		ITEM_IDENTIFY,
		PLAYER_CHGSHAPE,
		ENTER_SANCTUARY,

		LEAVE_SANCTUARY,		//	165
		PLAYER_OPEN_BOOTH,
		SELF_OPEN_BOOTH,
		PLAYER_CLOSE_BOOTH,
		PLAYER_BOOTH_INFO,

		BOOTH_TRADE_SUCCESS,	//	170
		BOOTH_NAME,
		PLAYER_START_TRAVEL,
		HOST_START_TRAVEL,
		PLAYER_END_TRAVEL,

		GM_INVINCIBLE,			//	175
		GM_INVISIBLE,
		HOST_CORRECT_POS,
		OBJECT_CAST_INSTANT_SKILL,
		ACTIVATE_WAYPOINT,

		WAYPOINT_LIST,			//	180
		UNFREEZE_IVTR_SLOT,		//	Unfreeze inventory item
		TEAM_INVITE_TIMEOUT,
		PLAYER_ENABLE_PVP,
		PLAYER_DISABLE_PVP,

		HOST_PVP_COOLDOWN,		//	185
		COOLTIME_DATA,
		SKILL_ABILITY,
		OPEN_BOOTH_TEST,
		BREATH_DATA,

		HOST_STOP_DIVE,			//	190
		BOOTH_SELL_ITEM,
		PLAYER_ENABLE_FASHION,
		HOST_ENABLE_FREEPVP,
		INVALID_OBJECT,

		PLAYER_ENABLE_EFFECT,	//	195
		PLAYER_DISABLE_EFFECT,
		REVIVAL_INQUIRE,
		SET_COOLDOWN,
		CHANGE_TEAM_LEADER,

		EXIT_INSTANCE,			//	200
		CHANGE_FACE_START,
		CHANGE_FACE_END,
		PLAYER_CHG_FACE,		//	Player change face completed
		OBJECT_CAST_POS_SKILL,

		SET_MOVE_STAMP,			//	205
		INST_DATA_CHECKOUT,
		HOST_RUSH_FLY,
		TRASHBOX_SIZE,
		NPC_DIED2,
		
		PRODUCE_NULL,			//	210
		PVP_COMBAT,
		DOUBLE_EXP_TIME,
		AVAILABLE_DOUBLE_EXP_TIME,
		DUEL_RECV_REQUEST,

		DUEL_REJECT_REQUEST,	//	215
		DUEL_PREPARE,
		DUEL_CANCEL,
		HOST_DUEL_START,
		DUEL_STOP,

		DUEL_RESULT,			//	220
		PLAYER_BIND_REQUEST,
		PLAYER_BIND_INVITE,
		PLAYER_BIND_REQUEST_REPLY,         
		PLAYER_BIND_INVITE_REPLY,

		PLAYER_BIND_START,		//	225
		PLAYER_BIND_STOP,
		PLAYER_MOUNTING,
		PLAYER_EQUIP_DETAIL,
		PLAYER_DUEL_START,
		
		PARIAH_TIME,			//	230
		GAIN_PET,
		FREE_PET,
		SUMMON_PET,
		RECALL_PET,

		PLAYER_START_PET_OP,	//	235
		PLAYER_STOP_PET_OP,
		PET_RECEIVE_EXP,
		PET_LEVELUP,
		PET_ROOM,

		PET_ROOM_CAPACITY,		//	240
		PET_HONOR_POINT,
		PET_HUNGER_GAUGE,
		HOST_ENTER_BATTLE,
		TANK_LEADER_NOTIFY,
		
		BATTLE_RESULT,			//	245
		BATTLE_SCORE,
		PET_DEAD,
		PET_REVIVE,
		PET_HP_NOTIFY,		
		
		PET_AI_STATE,			//	250
		REFINE_RESULT,
		PET_SET_COOLDOWN,
		PLAYER_CASH,
		PLAYER_BIND_SUCCESS,
		
		CHANGE_IVTR_SIZE,		//	255
		PVP_MODE,
		PLAYER_WALLOW_INFO,
		PLAYER_USE_ITEM_WITH_ARG,
		OBJECT_USE_ITEM_WITH_ARG,

		PLAYER_CHANGE_SPOUSE,	//	260
		NOTIFY_SAFE_LOCK,
		ELF_VIGOR,
		ELF_ENHANCE,
		ELF_STAMINA,

		ELF_CMD_RESULT,			//  265
		COMMON_DATA_NOTIFY,
		COMMON_DATA_LIST,
		ELF_REFINE_ACTIVATE,
		CAST_ELF_SKILL,

		MALL_ITEM_PRICE,        //	270
		MALL_ITEM_BUY_FAILED, 
		GOBLIN_LEVEL_UP,
		PLAYER_PROPERTY,
		PLAYER_CAST_RUNE_SKILL,

        PLAYER_CAST_RUNE_INSTANT_SKILL, //275
		PLAYER_EQUIP_TRASHBOX_ITEM,
		SECURITY_PASSWD_CHECKED,
		OBJECT_INVISIBLE,
		PLAYER_HP_STEAL,
		
		PLAYER_DIVIDEND,		//	280
		DIVIDEND_MALL_ITEM_PRICE,
		DIVIDEND_MALL_ITEM_BUY_FAILED,
		ELF_EXP,
		PUBLIC_QUEST_INFO,
	
		PUBLIC_QUEST_RANKS,		//	285
		MULTI_EXP_INFO,
		CHANGE_MULTI_EXP_STATE,
		WORLD_LIFE_TIME,
		WEDDING_BOOK_LIST,

		WEDDING_BOOK_SUCCESS,	//	290
		CALC_NETWORK_DELAY_RE,
		PLAYER_KNOCKBACK,
		SUMMON_PLANT_PET,
		PLANT_PET_DISAPPEAR,

		PLANT_PET_HP_NOTIFY,	//	295
		PET_PROPERTY,
		FACTION_CONTRIB_NOTIFY,
		FACTION_FORTRESS_INFO,
		ENTER_FACTIONFORTRESS,
		
		FACTION_RELATION_NOTIFY,	//	300
		PLAYER_EQUIP_DISABLED,
		PLAYER_SPEC_ITEM_LIST,		//  return value of GM_QUERY_SPEC_ITEM
		OBJECT_START_PLAY_ACTION,
		OBJECT_STOP_PLAY_ACTION,

		CONGREGATE_REQUEST,			//	305
		REJECT_CONGREGATE,
		CONGREGATE_START,
		CANCEL_CONGREGATE,
		ENGRAVE_START,

		ENGRAVE_END,				//	310
		ENGRAVE_RESULT,
		DPS_DPH_RANK,
		ADDONREGEN_START,
		ADDONREGEN_END,

		ADDONREGEN_RESULT,			// 315
		INVISIBLE_OBJ_LIST,
		SET_PLAYER_LIMIT,
		PLAYER_TELEPORT,
		OBJECT_FORBID_BE_SELECTED,

		PLAYER_INVENTORY_DETAIL,	// 320
		PLAYER_FORCE_DATA,
		PLAYER_FORCE_CHANGED,
		PLAYER_FORCE_DATA_UPDATE,
		FORCE_GLOBAL_DATA,

		ADD_MULTIOBJECT_EFFECT,		// 325
		REMOVE_MULTIOBJECT_EFFECT,
		ENTER_WEDDING_SCENE,
		PRODUCE4_ITEM_INFO,
		ONLINE_AWARD_DATA, //在线奖励

		TOGGLE_ONLINE_AWARD,		// 330 玩家切换了在线奖励的开启状态
		PLAYER_PROFIT_TIME,			// 玩家收益时间
		ENTER_NONPENALTY_PVP_STATE, // PK无惩罚
		SELF_COUNTRY_NOTIFY,
		PLAYER_COUNTRY_CHANGED,

		ENTER_COUNTRYBATTLE,	//	335
		COUNTRYBATTLE_RESULT,
		COUNTRYBATTLE_SCORE,
		COUNTRYBATTLE_RESURRECT_REST_TIMES,
		COUNTRYBATTLE_FLAG_CARRIER_NOTIFY,

		COUNTRYBATTLE_BECAME_FLAG_CARRIER,	//	340
		COUNTRYBATTLE_PERSONAL_SCORE,
		COUNTRYBATTLE_FLAG_MSG_NOTIFY,
		DEFENSE_RUNE_ENABLED,
		COUNTRYBATTLE_INFO,

		SET_PROFIT_TIME, //345
		CASH_MONEY_EXCHG_RATE,
		PET_REBUILD_INHERIT_START,
		PET_REBUILD_INHERIT_INFO,
		PET_REBUILD_INHERIT_END,

		PET_EVOLUTION_DONE,		//350
		PET_REBUILD_NATURE_START,	
		PET_REBUILD_NATURE_INFO,
		PET_REBUILD_NATURE_END,	
		EQUIP_ADDON_UPDATE_NOTIFY, //354 通知装备魂石变更

		SELF_KING_NOTIFY,		// 355
		PLAYER_KING_CHANGED,
		MERIDIANS_NOTIFY,          // 经脉信息变更
		MERIDIANS_RESULT,		   // 经脉冲穴结果
		COUNTRYBATTLE_STRONGHOND_STATE_NOTIFY,

		QUERY_TOUCH_POINT,		// 360
		SPEND_TOUCH_POINT,
		TOTAL_RECHARGE,
		QUERY_TITLE_RE,		
		CHANGE_CURR_TITLE_RE,   

		MODIFY_TITLE_NOFIFY,	// 365
		REFRESH_SIGNIN,         // 签到状态
		PARALLEL_WORLD_INFO,	
		PLAYER_REINCARNATION,	// 玩家转生
		REINCARNATION_TOME_INFO, // 转生卷轴信息

		ACTIVATE_REINCARNATION_TOME,	// 370 转生卷轴是否激活
        UNIQUE_DATA_NOTIFY, // 历史推进 ，全局数据
		USE_GIFTCARD_RESULT,
		REALM_EXP,				// 境界经验
		REALM_LEVEL,			// 境界等级

		ENTER_TRICKBATTLE,  // 375   进入战场  具体含义同国战进入战场
		TRICKBATTLE_PERSONAL_SCORE, //      战场自己得分
		TRICKBATTLE_CHARIOT_INFO, // 战车信息
		PLAYER_LEADERSHIP,					// 玩家统率力
		GENERALCARD_COLLECTION_DATA,		// 武将卡图鉴数据

		ADD_GENERALCARD_COLLECTION,			// 380 收藏卡牌
		REFRESH_MONSTERSPIRIT_LEVEL,		// 命轮
		MINE_GATHERED,						// 矿物成功采集广播
		PLAYER_IN_OUT_BATTLE,				// 其他玩家进入/脱离战斗状态
		PLAYER_QUERY_CHARIOTS,				// 战车数量查询

		COUNTRYBATTLE_LIVE_SHOW_RESULT,		// 385 国战战场rank和死亡数据
		RANDOM_MALL_SHOPPING_RES,			//	随机商城查询返回
		FACTION_PVP_MASK_MODIFY,			//	帮派PVP状态改变
		PLAYER_WORLD_CONTRIBUTION,			//	世界贡献度
		RANDOM_MAP_ORDER ,					//	随机地图

		SCENE_SERVICE_NPC_LIST,			//	390当前场景中学习技能的NPC列表
		NPC_VISIBLE_TID_NOTIFY,			//	广播通知视野内npc变换可视模型
		CLIENT_SCREEN_EFFECT,			//	屏幕特效
        EQUIP_CAN_INHERIT_ADDONS, 		//	生产类型5可继承的附加属性
		COMBO_SKILL_PREPARE,

		INSTANCE_REENTER_NOTIFY,		//	395 副本重入通知
		PRAY_DISTANCE_CHANGE,			//	吟唱距离参数
	};

	struct cmd_header
	{
        unsigned short cmd;
	};

	//	object 离开可见区域
	struct cmd_leave_slice
	{
		int id;
	};

	//	player更新位置
	struct cmd_notify_hostpos
	{
		A3DVECTOR3 vPos;
		int tag;
		int	line;
	};

	//	player list
	struct cmd_player_info_1_list
	{
		unsigned short count;
		BYTE placeholder;		//	info_player_1 list

		bool CheckValid(size_t buf_size, size_t& sz) const
		{
			if (buf_size < sizeof(*this))
				return false;

			size_t sz_org = buf_size;
			buf_size -= sizeof(count);
			const BYTE* pData = &placeholder;

			for (unsigned short i = 0; i < count; i++)
			{
				if (!reinterpret_cast<const info_player_1*>(pData)->CheckValid(buf_size, sz))
					return false;

				buf_size -= sz;
				pData += sz;
			}

			sz = sz_org - buf_size;
			return true;
		}
	};

	struct cmd_player_info_2_list 
	{
		unsigned short count;
		/*
		   struct		//假设的代码，实际上由于info2是变长的结构，所以无法这样组织
		   {
		   int cid;
		   player_info_2 info;
		   }list[];
		 */
		char data[1];
	};

	struct cmd_player_info_3_list 
	{
		unsigned short count;
		/*
		   struct		//假设的代码，实际上由于info3是变长的结构，所以无法这样组织
		   {
		   int cid;
		   player_info_3 info;
		   }list[];
		 */
		char data[1];
	};

	struct cmd_player_info_23_list 
	{
		unsigned short count;
		/*
		   struct		//假设的代码，实际上由于info2/3是变长的结构，所以无法这样组织
		   {
		   int cid;
		   player_info_2 info2;
		   player_info_3 info3;
		   }list[];
		 */
		char data[1];
	};

	struct cmd_npc_info_list 	//	npc list
	{
		unsigned short count;
		BYTE placeholder;		//	info_npc list

		bool CheckValid(size_t buf_size, size_t& sz) const
		{
			if (buf_size < sizeof(*this))
				return false;

			size_t sz_org = buf_size;
			buf_size -= sizeof(count);
			const BYTE* pData = &placeholder;

			for (unsigned short i = 0; i < count; i++)
			{
				if (!reinterpret_cast<const info_npc*>(pData)->CheckValid(buf_size, sz))
					return false;

				buf_size -= sz;
				pData += sz;
			}

			sz = sz_org - buf_size;
			return true;
		}
	};

	struct cmd_matter_info_list
	{
		unsigned short count;
		info_matter list[1];

		bool CheckValid(size_t buf_size, size_t& sz) const
		{
			sz = sizeof(*this) - sizeof(list);

			if (buf_size < sz)
				return false;

			sz += count * sizeof(info_matter);
			return buf_size >= sz;
		}
	};

	struct cmd_object_move
	{
		int id;
		A3DVECTOR3 dest;
		unsigned short use_time;
		short sSpeed;				//	Move speed 8.8 fix-point
		unsigned char move_mode;
	};

	//	player 离开游戏
	struct cmd_player_leave_world
	{
		int id;
	};

	struct cmd_npc_died
	{
		int id;
		int idKiller;
	};

	struct cmd_object_disappear
	{
		int id;
	};

	struct cmd_host_stop_attack
	{
		int	iReason;		//	Stop reason
	};

	struct cmd_host_attack_result
	{
        int idTarget;
        int iDamage;				//	如果是0表示没有击中
        int attack_flag;			//	标记该攻击是否有攻击优化符和防御优化符和重击发生
		unsigned char attack_speed;
	};

	struct cmd_error_msg
	{
		int	iMessage;
	};

	struct cmd_host_attacked		// with complete information from object_attack_result
	{
		int idAttacker;
		int	iDamage;
		char cEquipment;	//	The equipment which is mangled, 高位代表这次攻击是不是应该变橙色

		int attack_flag;      //标记该攻击是否有攻击优化符和防御优化符和重击发生
		char speed;                     //攻击速度 speed * 50 ms
	};

	struct cmd_player_died
	{
		int idKiller;
		int idPlayer;
	};

	struct cmd_host_died
	{
		int idKiller;
		A3DVECTOR3 pos;
	};

	struct cmd_player_revive
	{
		int idPlayer;
		short sReviveType;	//	Revive type
		A3DVECTOR3 pos;
	};

	struct cmd_pickup_money
	{
		int	iAmount;
	};

	struct cmd_pickup_item
	{
		int	tid;		//	id of template
		int expire_date;//  过期时间
		unsigned int	iAmount;		//	Total amoumt
		unsigned int	iSlotAmount;	//	Last slot amount
		unsigned char byPackage;	
		unsigned char bySlot;		//	Last slot's index
	};

	struct cmd_player_info_00
	{
		int	idPlayer;
		short sLevel;
		unsigned char State;
		unsigned char Level2;
		int	iHP;
		int	iMaxHP;
		int	iMP;
		int	iMaxMP;
		int iTargetID;
	};

	struct cmd_npc_info_00
	{
		int	idNPC;
		int	iHP;
		int	iMaxHP;
		int iTargetID;
	};

	struct cmd_out_of_sight_list
	{
		unsigned int uCount;
		int	idList[1];

		bool CheckValid(size_t buf_size, size_t& sz) const
		{
			sz = sizeof(*this) - sizeof(idList);

			if (buf_size < sz)
				return false;

			sz += uCount * sizeof(int);
			return buf_size >= sz;
		}
	};

	struct cmd_object_stop_move
	{
		int id;
		A3DVECTOR3 dest;
		short sSpeed;				//	Move speed 8.8 fix-point
		unsigned char dir;
		unsigned char move_mode;
	};

	struct cmd_matter_enter_world
	{
		info_matter	Info;
	};

	struct cmd_receive_exp
	{
		int exp;
		int sp;
	};
	
	struct cmd_level_up
	{
		int id;
	};

	struct cmd_self_info_1
	{
		int	iExp;
		int iSP;
		int cid;
		A3DVECTOR3 pos;
		unsigned short crc_e;
		unsigned short crc_c;
		unsigned char  dir;		//256的定点数表示度数
		unsigned char level2;
		int state;
		int state2;
		//	if (state & GP_STATE_ADV_MODE), 2 DWORD inserted here
		//	if (state & GP_STATE_SHAPE), 1 char inserted here
		//	if (state & GP_STATE_EMOTE), 1 char inserted here
		//	If (state & GP_STATE_EXTEND_PROPERTY), OBJECT_EXT_STATE_COUNT DWORD inserted here
		//	If (state & GP_STATE_FACTION), 1 int + 1 char inserted here
		//	If (state & GP_STATE_BOOTH), 1 char inserted here
		//	If (state & GP_STATE_EFFECT), effect data inserted here
		//	If (state & GP_STATE_PARIAH), 1 char inserted here

		bool CheckValid(size_t buf_size, size_t& sz) const
		{
			if (buf_size < sizeof(*this))
				return false;

			sz = sizeof(*this);

			if (state & GP_STATE_ADV_MODE)
				sz += sizeof(DWORD) * 2;

			if (state & GP_STATE_SHAPE)
				sz += sizeof(char);

			if (state & GP_STATE_EMOTE)
				sz += sizeof(char);

			if (state & GP_STATE_EXTEND_PROPERTY)
				sz += sizeof(DWORD) * OBJECT_EXT_STATE_COUNT;
		
			if (state & GP_STATE_FACTION)
				sz += sizeof(int) + sizeof(char);
			
			if (state & GP_STATE_BOOTH)
				sz += sizeof(char);

			if (state & GP_STATE_EFFECT)
			{
				const BYTE* pData = reinterpret_cast<const BYTE*>(this) + sz;
				sz += sizeof(BYTE);

				if (buf_size < sz)
					return false;

				sz += *pData * sizeof(short);
			}

			if (state & GP_STATE_PARIAH)
				sz += sizeof(char);

			if (state & GP_STATE_IN_MOUNT)
				sz += sizeof (unsigned short) + sizeof (int);

			if (state & GP_STATE_IN_BIND)
				sz += sizeof (char) + sizeof (int);

			if (state & GP_STATE_SPOUSE)
				sz += sizeof(int);

			if (state & GP_STATE_EQUIPDISABLED)
				sz += sizeof(__int64);

			if (state & GP_STATE_PLAYERFORCE)
				sz += sizeof(int);

			if (state & GP_STATE_MULTIOBJ_EFFECT)
			{
				const BYTE* pData = reinterpret_cast<const BYTE*>(this) + sz;
				sz += sizeof(int);

				if (buf_size < sz)
					return false;

				sz += *(int *)pData * (sizeof(int) + sizeof(char));
			}
			
			if (state & GP_STATE_COUNTRY)
				sz += sizeof(int);
			if (state2 & GP_STATE2_TITLE)
				sz += sizeof(unsigned short);
			if (state2 & GP_STATE2_REINCARNATION)
				sz += sizeof(unsigned char);
			if (state2 & GP_STATE2_REALM)
				sz += sizeof(unsigned char);
			if (state2 & GP_STATE2_FACTION_PVP_MASK)
				sz += sizeof(unsigned char);

			return buf_size >= sz;
		}
	};

	struct cmd_self_info_00
	{
		short sLevel;
		unsigned char State;
		unsigned char Level2;
		int	iHP;
		int	iMaxHP;
		int	iMP;
		int	iMaxMP;
		int iExp;
		int iSP;
		int iAP;
		int iMaxAP;
	};

	struct cmd_own_ivtr_info
	{ 
		unsigned char byPackage;
		unsigned char ivtr_size;
		size_t content_length;
		char content[1];

		bool CheckValid(size_t buf_size, size_t& sz) const
		{
			sz = sizeof(*this) - sizeof(content);

			if (buf_size < sz)
				return false;

			sz += content_length;
			return buf_size >= sz;
		}
	};

	struct cmd_own_ivtr_detail_info
	{
		unsigned char byPackage;
		unsigned char ivtr_size;
		size_t content_length;
		char content[1];

		bool CheckValid(size_t buf_size, size_t& sz) const
		{
			sz = sizeof(*this) - sizeof(content);

			if (buf_size < sz)
				return false;

			sz += content_length;
			return buf_size >= sz;
		}
	};

    struct cmd_own_item_info
	{
		unsigned char byPackage;
		unsigned char bySlot;
		int type;	 
		int expire_date;
		int state;
		size_t count;
		unsigned short crc;
		unsigned short content_length;
		char content[1];

		bool CheckValid(size_t buf_size, size_t& sz) const
		{
			sz = sizeof(*this) - sizeof(content);

			if (buf_size < sz)
				return false;

			sz += content_length;
			return buf_size >= sz;
		}
    };

    struct cmd_empty_item_slot
    {
		unsigned char byPackage;
		unsigned char bySlot;
    };

	struct cmd_exg_ivtr_item
	{
		unsigned char index1;
		unsigned char index2;
	};
	
	struct cmd_move_ivtr_item
	{       
        unsigned char src;
        unsigned char dest;
        unsigned int count;
	};
	
	struct cmd_player_drop_item
	{
        unsigned char byPackage;
        unsigned char bySlot;
        unsigned int count;
        int tid;
		unsigned char reason;
	};

	struct cmd_exg_equip_item
	{
        unsigned char index1;
        unsigned char index2;
	};

	struct cmd_equip_item
	{
        unsigned char index_inv;
        unsigned char index_equip;
		unsigned int count_inv;
		unsigned int count_equip;
	};

	struct cmd_move_equip_item
	{
        unsigned char index_inv;
        unsigned char index_equip;
        unsigned int amount;
	};

	struct cmd_own_ext_prop
	{
		size_t status_point;
		int attack_degree;   //攻击等级
		int defend_degree;   //防御等级
		int crit_rate;
		int crit_damage_bonus;
		int invisible_degree;
		int anti_invisible_degree;
		int penetration;
		int resilience;
		int vigour;
		ROLEEXTPROP prop;
	};

	struct cmd_add_status_point
	{
		size_t vitality;
		size_t energy;
		size_t strength;
		size_t agility;
		size_t remain;		
	};

	struct cmd_select_target
	{
		int idTarget;
	};

	//	PLAYER_EXT_PROP_BASE
	struct cmd_pep_base
	{
		int	idPlayer;
		ROLEEXTPROP_BASE ep_base;
	};

	//	PLAYER_EXT_PROP_MOVE
	struct cmd_pep_move
	{
		int idPlayer;
		ROLEEXTPROP_MOVE ep_move;
	};

	//	PLAYER_EXT_PROP_ATK
	struct cmd_pep_attack
	{
		int idPlayer;
		ROLEEXTPROP_ATK ep_attack;
	};

	//	PLAYER_EXT_PROP_DEF
	struct cmd_pep_def
	{
		int idPlayer;
		ROLEEXTPROP_DEF ep_def;
	};

	struct cmd_team_leader_invite
	{
		int idLeader;
		int seq;
		unsigned short wPickFlag;
	};

	struct cmd_team_reject_invite
	{
		int idPlayer;		//	Who reject our invitation
	};

	struct cmd_team_join_team
	{
		int idLeader;
		unsigned short wPickFlag;
	};

	struct cmd_team_member_leave
	{
		int idLeader;
		int idMember;
		short reason;
	};

	//	表明自己离开了队伍
	struct cmd_team_leave_party
	{
		int idLeader;
		short reason;
	};

	struct cmd_team_new_member
	{
		int idMember;
	};

	struct cmd_team_leader_cancel_party
	{
		int idLeader;
	};

	struct cmd_team_member_data
	{
		unsigned char member_count;
		unsigned char data_count;
		int idLeader;

		struct MEMBER
		{
			int idMember;
			short level;
			unsigned char state;
			unsigned char level2;
			unsigned char reincarnation_times;
			char wallow_level;
			int hp;
			int mp;
			int max_hp;
			int max_mp;
			int force_id;
			int profit_level;	// 收益时间级别
		} data[1];

		bool CheckValid(size_t buf_size, size_t& sz) const
		{
			sz = sizeof(*this) - sizeof(data);

			if (buf_size < sz)
				return false;

			sz += data_count * sizeof(MEMBER);
			return buf_size >= sz;
		}
	};

	struct cmd_team_member_pos
	{
		int idMember;
		A3DVECTOR3 pos;
		int idInst;
		char bSameInstance;
	};

	struct cmd_equip_data
	{
		unsigned short crc;
		int	idPlayer;
		__int64 mask;
		int data[1];     //0 ~ 29

		bool CheckValid(size_t buf_size, size_t& sz) const
		{
			sz = sizeof(*this) - sizeof(data);

			if (buf_size < sz)
				return false;

			for (int i = 0; i < 32; i++)
			{
				if (mask & (1 << i))
					sz += sizeof(int);
			}

			return buf_size >= sz;
		}
	};

	struct cmd_equip_data_changed
	{
		unsigned short crc;
		int idPlayer;
		__int64 mask_add;
		__int64 mask_del;
		int data_add[1]; //0 ~ 29

		bool CheckValid(size_t buf_size, size_t& sz) const
		{
			sz = sizeof(*this) - sizeof(data_add);

			if (buf_size < sz)
				return false;

			for (int i = 0; i < 32; i++)
			{
				if (mask_add & (1 << i))
					sz += sizeof(int);
			}

			return buf_size >= sz;
		}
	};

	struct cmd_equip_damaged
	{
		unsigned char index;
		char reason;    //0-无耐久 1-死亡引起的装备损毁
	};

	struct cmd_team_member_pickup
	{
		int idMember;
		int tid;
		int count;
	};

	struct cmd_npc_greeting
	{
		int idObject;	//	ID of NPC or player
	};

	struct cmd_npc_sev_content
	{
		int idNPC;
		int type;       //	服务的类型
		size_t length;
		char data[1];

		bool CheckValid(size_t buf_size, size_t& sz) const
		{
			sz = sizeof(*this) - sizeof(data);

			if (buf_size < sz)
				return false;

			sz += length;
			return buf_size >= sz;
		}
	};

	struct cmd_purchase_item
	{
		size_t cost;
		size_t yinpiao;			//  顾客购买时有效，表示银票的数目
		unsigned char flag;		//	1, booth_slot is valid
		unsigned short item_count;

		struct ITEM
		{
			int item_id;
			int expire_date;
			unsigned int count;
			unsigned short inv_index;
			unsigned char booth_slot;

		} item_list[1];

		bool CheckValid(size_t buf_size, size_t& sz) const
		{
			sz = sizeof(*this) - sizeof(item_list);

			if (buf_size < sz)
				return false;

			sz += item_count * sizeof(ITEM);
			return buf_size >= sz;
		}
	};

	struct cmd_item_to_money
	{
		unsigned short index;           //	在包裹栏里的索引号码
		int type;                       //	物品的类型
		size_t count;
		size_t money;
	};

	struct cmd_repair_all
	{
		size_t cost;
	};

	struct cmd_repair
	{
		unsigned char byPackage;
		unsigned char bySlot;
		size_t cost;
	};

	struct cmd_spend_money
	{
		size_t cost;
	};

	struct cmd_gain_money_in_trade
	{
		int amount;
	};

	struct cmd_gain_item_in_trade
	{
		int type;
		unsigned int amount;
	};

	struct cmd_gain_money_after_trade
	{
		size_t amount;
	};

	struct cmd_gain_item_after_trade
	{
		int tid;
		int expire_date;			// expiration date
		unsigned int amount;
		unsigned int iSlotAmount;	//	Last slot amount
		unsigned short index;
	};

	struct cmd_get_own_money
	{
		size_t amount;
		size_t max_amount;
	};

	struct cmd_host_start_attack
	{
		int idTarget;		//	target id
		unsigned short ammo_remain;
		unsigned char attack_speed;
	};

	struct cmd_object_cast_skill
	{
		int caster;
		int target;
		int skill;
		unsigned short time;
		unsigned char level;
	};

	struct cmd_skill_interrupted
	{
		int caster;
	};

	struct cmd_self_skill_interrupted
	{
		unsigned char reason;
	};

	struct cmd_skill_data
	{
		size_t skill_count;

		struct SKILL
		{
			short id_skill;
			unsigned char level;
			short ability;

		} skill_list[1];

		bool CheckValid(size_t buf_size, size_t& sz) const
		{
			sz = sizeof(*this) - sizeof(skill_list);

			if (buf_size < sz)
				return false;

			sz += skill_count * sizeof(SKILL);
			return buf_size >= sz;
		}
	};

	struct cmd_host_use_item
	{
		unsigned char byPackage;
		unsigned char bySlot;
		int item_id;
		unsigned short use_count;
	};

	struct cmd_embed_item
	{                                       
		unsigned char chip_idx;
		unsigned char equip_idx;
	};

	struct cmd_clear_tessera
	{
		unsigned short equip_idx;
		size_t cost;
	};

	struct cmd_cost_skill_point
	{
		int skill_point;
	};

	struct cmd_learn_skill
	{
		int skill_id;
		int skill_level;
	};

	struct cmd_object_takeoff
	{
		int object_id;
	};

	struct cmd_object_landing
	{
		int object_id;
	};

	struct cmd_flysword_time
	{
		unsigned char where;
		unsigned char index;
		int cur_time;
	};

	struct cmd_host_obtain_item
	{
		int type;
		int expire_date;
		unsigned int amount;
		unsigned int slot_amount;
		unsigned char where;            //在哪个包裹栏，0 标准，2 任务，1 装备
		unsigned char index;            //最后部分放在哪个位置
	};      
 
	struct cmd_produce_start
	{
		unsigned short use_time;
		unsigned short count;
		int type;
	};

	struct cmd_produce_once
	{
		int type;
		unsigned int amount;
		unsigned int slot_amount;
		unsigned char where;            //在哪个包裹栏，0 标准，2 任务，1 装备
		unsigned char index;            //最后部分放在哪个位置
	};
	
	struct cmd_decompose_start
	{
		unsigned short use_time;
		int type;
	};

	struct cmd_task_data
	{
		size_t active_list_size;
		char active_list[1];

		size_t finished_list_size;
		char finished_list[1];

		size_t finished_time_list_size;
		char finished_time_list[1];	

		size_t finished_count_size;
		char finished_count[1];
		
		size_t storage_task_size;
		char storage_task[1];


		bool CheckValid(size_t buf_size, size_t& sz) const
		{
			const BYTE* pData = (const BYTE*)this;
			size_t len;

			//	active list
			sz = sizeof(size_t);
			if (buf_size < sz)
				return false;

			len = *(size_t*)pData;
			pData += sizeof(size_t);
			sz += len;

			if (buf_size < sz)
				return false;
			pData += len;

			//	finished list
			sz += sizeof(size_t);
			if (buf_size < sz)
				return false;

			len = *(size_t*)pData;
			pData += sizeof(size_t);
			sz += len;

			if (buf_size < sz)
				return false;
			pData += len;

			//	finished time list
			sz += sizeof(size_t);
			if (buf_size < sz)
				return false;

			len = *(size_t*)pData;
			pData += sizeof(size_t);
			sz += len;

			if (buf_size < sz)
				return false;

			pData += len;
			
			//	finished count list
			sz += sizeof(size_t);
			if (buf_size < sz)
				return false;

 			len = *(size_t*)pData;
 			pData += sizeof(size_t);
 			sz += len;

			if (buf_size < sz)
				return false;

			pData += len;

			//	storage task
			sz += sizeof(size_t);
			if (buf_size < sz)
				return false;

			len = *(size_t*)pData;
			pData += sizeof(size_t);
			sz += len;

			return buf_size >= sz;
		}
	};

	struct cmd_task_var_data
	{
		size_t size;
		char data[1];

		bool CheckValid(size_t buf_size, size_t& sz) const
		{
			sz = sizeof(*this) - sizeof(data);

			if (buf_size < sz)
				return false;

			sz += size;
			return buf_size >= sz;
		}	
	};

	struct cmd_object_start_use
	{
		int user;
		int item;
		unsigned short time;
	};

	struct cmd_object_cancel_use
	{
		int user;
	};

	struct cmd_object_use_item
	{
		int user;
		int item;
	};

	struct cmd_object_start_use_t
	{
		int user;
		int target;
		int item;
		unsigned short time;
	};

	struct cmd_object_sit_down
	{
		int id;
	};      

	struct cmd_object_stand_up
	{
		int id;
	};

	struct cmd_object_do_emote
	{       
		int id;
		unsigned short emotion;
	};

	struct cmd_server_time
	{
		int time;
		int timebias;
		int lua_version;
	};

	struct cmd_object_root
	{
		int id;
		A3DVECTOR3 pos;
	};

	struct cmd_invader_rise
	{
		int id;
	};

	struct cmd_pariah_rise
	{
		int id;
		unsigned char pariah_lvl;
	};

	struct cmd_invader_fade
	{
		int id;
	};

	struct cmd_hurt_result
	{
		int target_id;
		int damage;
	};
	
	struct cmd_object_atk_result
	{
		int attacker_id;
		int target_id;
		int damage;
		int attack_flag;				//标记该攻击是否有攻击优化符和防御优化符和重击发生
		char speed;                     //攻击速度 speed * 50 ms
	};

	struct cmd_be_hurt
	{
		int attacker_id;
		int damage;
		unsigned char flag;		//	1, attacker 变橙名
	};

	struct cmd_update_ext_state
	{
		int id; 
		DWORD states[OBJECT_EXT_STATE_COUNT];
	};

	struct cmd_attack_once
	{
		unsigned char ammo_num;
	};

	struct IconState 
	{
		unsigned short id;
		int	pcount;
		int	param[3];
	};
	typedef abase::vector<IconState> IconStates;
	struct cmd_icon_state_notify
	{
		int id;
		IconStates states;

		bool Initialize(const void *pDataBuf, DWORD dwDataSize, DWORD *pSizeUsed = NULL)
		{
			//	提供初始化函数从缓冲中抽取“复杂”结构
			//
			bool bRet(false);
			while (true)
			{
				const byte *p = (const byte *)pDataBuf;
				DWORD s = dwDataSize;

				//	抽取玩家ID
				if (!Extract(id, p, s))
					break;

				unsigned short u(0);

				//	读取state数组
				unsigned short scount(0);
				if (!Extract(scount, p, s))
					break;
				abase::vector<unsigned short> state;
				if (scount > 0)
				{
					state.reserve(scount);
					for (u = 0; u < scount; ++ u)
						state.push_back(0);
					if (!Extract(&state[0], scount, p, s))
						break;
				}

				//	读取state参数个数数组
				unsigned short pcount;
				if (!Extract(pcount, p, s))
					break;
				abase::vector<int> param;
				if (pcount > 0)
				{
					param.reserve(pcount);
					for (u = 0; u < pcount; ++ u)
						param.push_back(0);
					if (!Extract(&param[0], pcount, p, s))
						break;
				}

				//	生成客户端可直接使用的 States 数组
				states.clear();
				states.reserve(scount);
				IconState dummy;
				int param_offset(0);
				for (u = 0; u < scount; ++ u)
				{
					unsigned short s = state[u];
					dummy.id = (s & 0x3fff);
					dummy.pcount = ((s >> 14) & 0x3);
					if (dummy.pcount > 0)
					{
						if ( param_offset+dummy.pcount <= pcount)
						{
							for (int k = 0; k < dummy.pcount; ++ k)
								dummy.param[k] = param[param_offset++];
						}
						else
						{
							//	服务器传递的参数总数量错误
							break;
						}
					}
					//	其它参数置0
					for (int k=dummy.pcount; k<sizeof(dummy.param)/sizeof(dummy.param[0]); ++ k)
						dummy.param[k] = 0;

					states.push_back(dummy);
				}
				if (u < scount)
					break;

				if (pSizeUsed)
					*pSizeUsed = dwDataSize-s;
				bRet = true;
				break;
			}
			return bRet;
		}
	};

	struct cmd_player_gather_start
	{
		int pid;                //	player id
		int mid;                //	mine id
		unsigned char use_time; //	use time in sec;
	};

	struct cmd_player_gather_stop
	{
		int pid;                //	player id
	};

	struct cmd_trashbox_pwd_changed
	{
		unsigned char has_passwd;
	};

	struct cmd_trashbox_pwd_state
	{
		unsigned char has_passwd;
	};

	struct cmd_trashbox_open
	{
		char is_accountbox;		// 1 账号仓库，此时仅slot_size有效；0 角色仓库，三个值都有效
		unsigned short slot_size;
		unsigned short slot_size2;
		unsigned short slot_size3;
	};

	struct cmd_trashbox_close
	{
		char is_accountbox;
	};

	struct cmd_trashbox_wealth
	{
		char is_accountbox;
		size_t money;
	};

	struct cmd_exg_trashbox_item
	{
		unsigned char where;
		unsigned char idx1;
		unsigned char idx2;
	};

	struct cmd_move_trashbox_item
	{
		unsigned char where;
		unsigned char src;
		unsigned char dest;
		size_t amount;
	};

	struct cmd_exg_trashbox_ivtr
	{
		unsigned char where;
		unsigned char idx_tra;
		unsigned char idx_inv;
	};

	struct cmd_trash_item_to_ivtr
	{
		unsigned char where;
		unsigned char src;
		unsigned char dest;
		size_t amount;
	};

	struct cmd_ivty_item_to_trash
	{
		unsigned char where;
		unsigned char src;
		unsigned char dest;
		size_t amount;
	};

	struct cmd_exg_trash_money
	{
		char is_accountbox;
		int inv_delta;		//	自己的钱数变化
		int tra_delta;		//	仓库的钱数变化
	};

	struct cmd_enchant_result
	{
		int caster;
		int target;
		int skill;
		char level;
		char orange_name;
		int attack_flag;
		byte section;
	};

	struct cmd_host_notify_root
	{
		A3DVECTOR3 pos;
		unsigned char type;
	};

	struct cmd_host_dispel_root
	{
		unsigned char type;
	};

	struct cmd_obj_do_action
	{
        int id;
        unsigned char action;
	};

	struct cmd_host_skill_attack_result
	{
		int idTarget;
		int idSkill;
        int iDamage;				//	如果是0表示没有击中
        int attack_flag;			//	标记该攻击是否有攻击优化符和防御优化符和重击发生
		unsigned char attack_speed;
		byte section;
	};

	struct cmd_object_skill_attack_result
	{
		int attacker_id;
		int target_id;
		int skill_id;
		int damage;
		int attack_flag;		      //标记该攻击是否有攻击优化符和防御优化符和重击发生
		char speed;                   //攻击速度 speed * 50 ms
		byte section;
	};

	struct cmd_host_skill_attacked
	{
		int		idAttacker;
		int     idSkill;
		int		iDamage;
		char	cEquipment;				//	The equipment which is mangled, 高位代表这次攻击是不是应该变橙色

		int attack_flag;			    //标记该攻击是否有攻击优化符和防御优化符和重击发生
		char speed;                     //攻击速度 speed * 50 ms
		unsigned char section;			// skill section
	};

	struct cmd_player_set_adv_data
	{
		int id;
		int data1;
		int data2;
	};

	struct cmd_player_clr_adv_data
	{
		int id;
	};

	struct cmd_player_in_team
	{
		int id;
		unsigned char state;    // 0 no team 1, leader, 2 member
	};

	struct cmd_team_ask_join
	{
		int idAsker;
	};

	struct cmd_object_emote_restore
	{
		int id;
		unsigned short emotion;
	};

	struct cmd_con_emote_request
	{
		int id;
		unsigned short emotion;
	};

	struct cmd_do_concurrent_emote
	{
		int id1;
		int id2;
		unsigned short emotion;
	};

	struct cmd_matter_pickup
	{
		int matter_id;
		int who;
	};

	struct cmd_mafia_info_notify
	{
		int idPlayer;				//	player id
		int idFaction;				//	帮派的 id
		unsigned char player_rank;	//	玩家在帮派内的等级
	};

	struct cmd_task_deliver_item
	{
		int type;
		int	expire_date;
		unsigned int amount;
		unsigned int slot_amount;
		unsigned char where;            //在哪个包裹栏，0 标准，2 任务，1 装备
		unsigned char index;            //最后部分放在哪个位置
	};

	struct cmd_task_deliver_rep
	{
		int delta;
		int cur_reputaion;
	};

	struct cmd_task_deliver_exp
	{
		int exp;
		int sp;
	};

	struct cmd_task_deliver_money
	{
		size_t amount;
		size_t cur_money;
	};

	struct cmd_task_deliver_level2
	{
		int id_player;
		int level2;
	};

	struct cmd_host_reputation
	{
		int reputation;
	};

	struct cmd_item_identify
	{
		unsigned char ivtr_index;
		unsigned char result;		//  0, success
	};

	struct cmd_player_chgshape
	{
		int idPlayer;
		unsigned char shape;
	};

	struct cmd_object_enter_sanctuary
	{
		int id;		// self id or pet id.
	};

	struct cmd_object_leave_sanctuary
	{
		int id;		// self id or pet id.
	};

	struct cmd_player_open_booth
	{
		int pid;
		unsigned char booth_crc;
		unsigned char name_len;		//	name length in bytes
		char name[1];				//	最大62字节

		bool CheckValid(size_t buf_size, size_t& sz) const
		{
			sz = sizeof(*this) - sizeof(name);

			if (buf_size < sz)
				return false;

			sz += name_len;
			return buf_size >= sz;
		}
	};

	struct cmd_self_open_booth
	{
		unsigned short count;

		struct item_t
		{
			int type;               //	物品类型
			unsigned short index;
			unsigned int count;   //	卖多少个
			size_t price;           //	单价

		} goods_list[1];

		bool CheckValid(size_t buf_size, size_t& sz) const
		{
			sz = sizeof(*this) - sizeof(goods_list);

			if (buf_size < sz)
				return false;

			sz += count * sizeof(item_t);
			return buf_size >= sz;
		}
	};

	struct cmd_player_close_booth
	{
		int pid;
	};

	struct cmd_player_booth_info
	{
		int pid;
		int booth_id;
		size_t count;
		char goods_list[1];

	/*  each booth good struct
		union booth_goods
		{
			struct
			{
				int type;		//	物品类型, 0 表示用这个结构

			} empty_item;

			struct
			{
				int type;       //	物品类型
				int count;		//	> 0
				size_t price;   //	单价
				int expire_date;//  过期时间
				unsigned short content_length;
				char content[];

			} sell_item;

			struct
			{
				int type;       //	物品类型
				int count;		//	< 0
				size_t price;   //	单价

			} buy_item;

		} item_list[];	*/

		bool CheckValid(size_t buf_size, size_t& sz) const
		{
			#define CHECK_DATA(type) \
			{\
				if (buf_size < sizeof(type))\
					return false;\
				buf_size -= sizeof(type);\
				sz += sizeof(type);\
			}

			sz = sizeof(*this) - sizeof(goods_list);

			if (buf_size < sz)
				return false;

			buf_size -= sz;
			const BYTE* pData = (const BYTE*)goods_list;
			size_t iCount = 0;

			while (iCount < count)
			{
				CHECK_DATA(int)
				int idItem = *(int*)pData;
				pData += sizeof(int);

				if (idItem)
				{
					CHECK_DATA(int)
					int iAmount = *(int*)pData;
					pData += sizeof (int);

					CHECK_DATA(int)
					int iPrice = *(int*)pData;
					pData += sizeof (int);

					if (iAmount > 0)
					{
						CHECK_DATA(int)
						int iExpireDate = *(int*)pData;
						pData += sizeof (int);

						CHECK_DATA(WORD)
						WORD wDataLen = *(WORD*)pData;
						pData += sizeof (WORD);

						if (buf_size < wDataLen)
							return false;

						buf_size -= wDataLen;
						sz += wDataLen;
						pData += wDataLen;
					}
				}

				iCount++;
			}

			return true;
		}
	};

	struct cmd_booth_trade_success
	{
		int trader;
	};

	struct cmd_booth_name
	{
		int pid;
		unsigned char crc_name;		//	Booth name
		unsigned char name_len;		//	name length in bytes
		char name[1];				//	最大62字节

		bool CheckValid(size_t buf_size, size_t& sz) const
		{
			sz = sizeof(*this) - sizeof(name);

			if (buf_size < sz)
				return false;

			sz += name_len;
			return buf_size >= sz;
		}
	};

	struct cmd_player_start_travel	//	广播
	{
		int pid;
		unsigned char vehicle;
	};

	struct cmd_host_start_travel
	{
		float speed;				//	速度增量（可能也可以从交通工具决定）
		A3DVECTOR3 dest;			//	目标点
		int line_no;				//	航线号
		unsigned char vehicle;		//	交通工具代号
	};

	struct cmd_player_end_travel	//	广播
	{
		int pid;
		unsigned char vehicle;
	};

	struct player_change_spouse
	{
		int who;
		int spouse;
	};

	struct gm_cmd_invisible
	{
		unsigned char is_visible;
	};

	struct gm_cmd_invincible
	{
		unsigned char is_invincible;
	};

	struct cmd_host_correct_pos
	{
		A3DVECTOR3 pos;
		unsigned short stamp;
	};

	struct cmd_object_cast_instant_skill
	{
		int caster;
		int target;
		int skill;
		unsigned char level;
	};

	struct cmd_activate_waypoint
	{
		unsigned short waypoint;
	};

	struct cmd_waypoint_list
	{
		size_t count;                   
		unsigned short list[1];

		bool CheckValid(size_t buf_size, size_t& sz) const
		{
			sz = sizeof(*this) - sizeof(list);

			if (buf_size < sz)
				return false;

			sz += count * sizeof(unsigned short);
			return buf_size >= sz;
		}
	};                                      

	struct cmd_unfreeze_ivtr_slot
	{
		unsigned char where;
		unsigned short index;
	};

	struct cmd_team_invite_timeout
	{
		 int who;
	};

	struct cmd_player_enable_pvp
	{
		 int who;
		 char type;// pk保护区域新加
	};

	struct cmd_player_disable_pvp
	{
		 int who;
		 char type; // pk保护区域新加
	};

	struct cmd_host_pvp_cooldown
	{
		 int cool_time;
		 int max_cool_time;
	};

	struct cmd_cooltime_data
	{
		unsigned short count;

		struct item_t
		{
			unsigned short idx;
			int cooldown;
			int max_cooltime;

		} list[1];

		bool CheckValid(size_t buf_size, size_t& sz) const
		{
			sz = sizeof(*this) - sizeof(list);

			if (buf_size < sz)
				return false;

			sz += count * sizeof(item_t);
			return buf_size >= sz;
		}
	};

	struct cmd_skill_ability
	{
		int skill_id;
		int skill_ability;
	};

	struct cmd_breath_data
	{
		int breath;     
		int breath_capacity;
	};

	struct cmd_booth_sell_item
	{
		short inv_index;
		int item_type;
		size_t item_count;
		int idBuyer;
	};

	struct cmd_player_enable_fashion
	{
		int idPlayer;
		unsigned char is_enabble;
	};

	struct cmd_host_enable_freepvp
	{
		unsigned char is_enable;  //是否激活
	};

	struct cmd_invalid_object
	{
		int id;
	};

	struct cmd_player_enable_effect
	{
		short effect;
		int id;
	};

	struct cmd_player_disable_effect
	{
		short effect;
		int id;
	};

	struct cmd_revival_inquire
	{
		float exp_reduce;
	};

	struct cmd_set_cooldown
	{
		int cooldown_index;
		int cooldown_time;
	};

	struct cmd_change_team_leader
	{
		int old_leader;
		int new_leader;
	};

	struct cmd_exit_instance
	{
		int	idInst;
		char reason;
		int iLeftTime;
	};

	struct cmd_change_face_start
	{
		unsigned short ivtr_idx;
	};

	struct cmd_change_face_end
	{
		unsigned short ivtr_idx;
	};

	struct cmd_player_chg_face
	{
		unsigned short crc_c;
		int idPlayer;
	};

	struct cmd_object_cast_pos_skill
	{       
		int caster;
		A3DVECTOR3 pos;
		int target;
		int skill;
		unsigned short time;
		unsigned char level;
	};
	
	struct cmd_set_move_stamp
	{
		unsigned short move_stamp;
	};

	struct cmd_inst_data_checkout
	{
		int idInst;
		unsigned int region_time_stamp;
		unsigned int precinct_time_stamp;
		unsigned int gshop_time_stamp;
		unsigned int gshop_time_stamp2;
	};

	struct cmd_host_rush_fly
	{
		unsigned char is_active;
	};

	struct cmd_trashbox_size
	{
		unsigned char where;
		int iNewSize;
	};

	struct cmd_npc_died2
	{
		int id;
		int idKiller;
	};

	struct cmd_produce_null
	{
		int idRecipe;
	};

	struct cmd_pvp_combat
	{
		int idPlayer;
		char state;
	};

	struct cmd_double_exp_time
	{
		int mode;		//	非0表示双倍经验状态,0表示非双倍经验状态
		int end_time;	//	结束时间戳
	};

	struct cmd_available_double_exp_time
	{
		int available_time;	//	剩余时间,单位秒
	};

	struct cmd_duel_recv_request
	{
		int player_id;
	};

	struct cmd_duel_reject_request
	{
		int player_id;
		int reason;
	};

	struct cmd_duel_prepare
	{
		int player_id;
		int delay;              //几秒延时
	};

	struct cmd_duel_cancel
	{
		int player_id;
	};

	struct cmd_host_duel_start
	{
		int idOpponent;
	};

	struct cmd_duel_stop
	{
		int player_id;
	};

	struct cmd_duel_result
	{
		int id1;
		int id2;
		char result;    //1 id1 win 2 draw
	};

	struct cmd_player_bind_request
	{
		int who;
	};

	struct cmd_player_bind_invite
	{
		int who;
	};

	struct cmd_player_bind_request_reply
	{
		int who;
		int param;	//	只会非0 不同意
	};

	struct cmd_player_bind_invite_reply
	{
		int who;
		int param;	//	只会非0不同意
	};

	struct cmd_player_bind_start
	{
		int mule;
		int rider;
	};

	struct cmd_player_bind_stop
	{
		int who;
	};

	struct cmd_player_mounting
	{
		int id;
		int mount_id;
		unsigned short mount_color;
	};

	struct cmd_player_equip_detail
	{       
		int id;
		size_t content_length;
		char content[1];
	};

	struct cmd_player_duel_start
	{
		int player_id;
	};

	struct cmd_pariah_time
	{
		int pariah_time;
	};

	struct cmd_gain_pet
	{
		int slot_index;		//	此宠物放于宠物栏内的位置
		info_pet data;		//	pet data
	};

	struct cmd_free_pet
	{
		int slot_index;
		int pet_id;			//	宠物的id，无大用
	};

	struct cmd_summon_pet
	{
		int slot_index;
		int pet_tid;
		int pet_pid;
		int life_time;
	};
	
	enum PET_RECALL_REASON
	{
		PET_RECALL_DEFAULT,			//	默认召回，主要原因为玩家主动召回、玩家召唤出其它原因导致原有宠物召回等
		PET_RECALL_DEATH,			//	宠物死亡导致召回消息
		PET_RECALL_LIFE_EXHAUST,	//	宠物时限到
		PET_RECALL_SACRIFICE,		//	宠物牺牲（玩家使用技能等导致）
	};

	struct cmd_recall_pet
	{
		int slot_index;
		int pet_id;
		char reason;
	};

	struct cmd_player_start_pet_op
	{
		int slot_index;
		int pet_id;
		int delay;			//	延迟时间，单位是50ms的tick
		int op;				//	操作类型  0:放出 1:召回 2:放生
	};

	struct cmd_pet_receive_exp
	{
		int slot_index;
		int pet_id;
		int exp;
	};

	struct cmd_pet_levelup
	{
		int slot_index;
		int pet_id;
		int level;			//	新级别
		int exp;			//	当前的经验值 
	};

	struct cmd_pet_room
	{
		unsigned short count;
	//	重复 count 次
	//	int index;
	//	info_pet data;
	};

	struct cmd_pet_room_capacity
	{
		size_t capacity;
	};

	struct cmd_pet_honor_point
	{
		int index;
		int cur_honor_point;
	};

	struct cmd_pet_hunger_gauge
	{
		int index;
		int cur_hunge_gauge;
	};

	struct cmd_host_enter_battle
	{
		int battle_camp;
		int id_battle;
		int end_time;
	};

	struct cmd_tank_leader_notify
	{
		int idTank;
		int idLeader;
	};

	struct cmd_battle_result
	{
		int result;		//	1, invader win; 2, defender win; 3, over time
	};

	struct cmd_battle_score
	{
		int	invader_score;
		int invader_target;
		int def_score;
		int def_target;
	};

	struct cmd_pet_dead
	{
		size_t pet_index;
	};

	struct cmd_pet_revive
	{
		size_t pet_index;
		float hp_factor;
	};

	struct cmd_pet_hp_notify
	{
		size_t pet_index;
		float hp_factor;
		int cur_hp;
		float mp_factor;
		int cur_mp;
	};

	struct cmd_pet_ai_state
	{
		unsigned char attack;	//	0 防御型; 1 主动型; 2 被动型
		unsigned char move;		//	0 跟随; 1 原地停留
	};

	struct cmd_refine_result
	{
		int result;		//	精炼结果  0 成功 1 失败（材料消失） 2失败（材料消失，级别归０）　3失败（材料和装备消失）
	};

	struct cmd_pet_set_cooldown
	{
		int pet_index;
		int cooldown_index;
		int cooldown_time; 
	};

	struct player_cash //告知玩家自身的游戏点数，此点数用于百宝阁购买物品
	{
		int cash_amount;
	};

	struct cmd_player_bind_success
	{
		unsigned short inv_index;
		int item_id;
	};

	struct cmd_change_ivtr_size
	{
		int size;
	};

	struct pvp_mode
	{
		unsigned char mode;
	};

	struct player_wallow_info  //玩家当前的沉迷信息, 刚进入游戏的时候服务器会主动发送，沉迷等级改变时也会发送，沉迷被GM开关若未引起沉迷等级变化，服务器不会主动发送
	{
        unsigned char anti_wallow_active;  //游戏服务器端根据沉迷等级修改经验和掉落标志是否打开
        unsigned char wallow_level;        //沉迷等级 0 1 2
		int play_time;
        int light_timestamp;               //中度沉迷的时间点
        int heavy_timestamp;               //重度沉迷的时间点
		int	reason;								//	沉迷原因：0身份证号标识的是未成年人；1未填身份证号（走快速注册）
	};

	struct player_use_item_with_arg          //玩家自己使用物品，带有自定义参数，只有自己收到
	{
		unsigned char where;            //哪里的物品
		unsigned char index;            //物品的位置
		int item_id;                    //物品的类型
		unsigned short use_count;       //使用的个数
		unsigned short size;
		char arg[1];                     //物品的自定参数
	};

	struct object_use_item_with_arg         //除自己外收到的自己定参数使用物品 
	{
		int user;
		int item;
		unsigned short size;
		char arg[1];
	};

	struct notify_safe_lock         //安全锁是否打开
	{
        unsigned char active;
		int time;
		int total_time;
	};

	struct cmd_elf_vigor
	{
		int vigor; 			//元气
		int max_vigor;		//元气上限
		int vigor_gen;		//元气回复
	};

	struct cmd_elf_enhance
	{
		short str_en;		//使用小精灵的Buff技能产生的属性增强
		short agi_en;
		short vit_en;
		short eng_en;
	};

	struct cmd_elf_stamina
	{
		int stamina;   //小精灵的耐力
	};

	struct cmd_elf_result
	{   
		int cmd;		//命令id
		int result;		//命令执行结果，对不同的命令具有不同含义
		int param1;
		int param2;
	};

	struct common_data_notify   //全局数据修正，这些全局数据将会用于任务
	{
		struct _node
		{
			int key;
			int value;
		};
	};

	struct common_data_list     //用于全局数据的列表，注意这个结构使用了整个数据区的size来计算全局数据的个数
	{
		struct _node
		{
			int key;
			int value;
		};
	};

	struct cmd_elf_refine_activate
	{
		int pid;	//player id
		char status; //1 激活 0 取消激活
	};

	struct cmd_cast_elf_skill
	{
		int pid;				//player id
		int target;				//target id
		int skill;
		unsigned char level;
	};

	struct cmd_mall_item_price	// 在收到GET_MALL_ITEM_PRICE协议时发送给客户端
	{
		short start_index;
		short end_index;
		short count;
		struct good_item
		{
			short good_index;
			char  good_slot;
			int   good_id;
			char  expire_type;
			int   expire_time;
			int   goods_price;
			char  good_status;
		}list[1];
	};

	// 如果由于某些原因导致服务器商城数据和客户端不一致
	//（例如player在打开商城后，停留商城时间长，服务器的商城数据发生改变）
	// 而在这种情况下player发生购买操作，则gs通过以下协议通知客户端购买失败
	// 这是客户端会通过GET _MALL_ITEM_RECENT _PRICE协议去获取更新数据
	struct cmd_mall_item_buy_failed	
	{
		short index;
		char  reason;
	};

	struct cmd_goblin_level_up
	{
		int player_id;
	};

	struct cmd_player_property
	{
		int id;
		int hp;
		int mp;
		int damage_low;			//最低damage
		int damage_high;		//最大物理damage
		int damage_magic_low;	//魔法最低攻击力
		int damage_magic_high;	//魔法最高攻击力
		int defense;			//防御力
		int resistance[5];		//魔法抗性
		int attack;				//攻击率 attack rate
		int armor;				//闪躲率（装甲等级）
		int attack_speed;		//攻击时间间隔 以tick为单位
		float run_speed;		//奔跑速度 单位  m/s
		int attack_degree;		//攻击等级
		int defend_degree;		//防御等级
		int crit_rate;			//暴击
		int damage_reduce;		//伤害减免百分比，可正负
		int prayspeed;			//吟唱速度增加百分比,可正负
		int crit_damage_bonus;  //暴伤百分比增加值
		int invisible_degree;   //遁隐等级
		int anti_invisible_degree; //反遁隐等级
		int vigour;
		
		int self_damage_reduce;	//物理减免
		int self_prayspeed;		//吟唱速度
	};

	struct cmd_player_cast_rune_skill
	{
		int caster;
		int target;
		int skill;
		unsigned short time;
		unsigned char level;
	};

	struct cmd_player_cast_rune_instant_skill
	{
		int caster;
		int target;
		int skill;
		unsigned char level;
	};

	struct cmd_player_equip_trashbox_item
	{
		unsigned char trashbox_index; // 随身仓库共两个，时装和卡牌。目前服务器只支持卡牌仓库与装备栏之间的物品移动
		unsigned char trash_idx;
		unsigned char equip_idx;
	};
	
	struct cmd_object_invisible
	{
		int id;					//可以是player和npc
		int invisible_degree;	//0 非隐身； >0 隐身等级
	};

	// 玩家被吸血时收到此协议
	struct cmd_player_hp_steal
	{
		int hp;
	};

	struct cmd_player_dividend //告知玩家自身的鸿利数，此鸿利数用于鸿利商城购买物品
	{
		int dividend;
	};

	struct cmd_dividend_mall_item_price
	{
		short start_index;
		short end_index;
		short count;

		struct good_info
		{
			short good_index;
			char good_slot;
			int good_id;
			char expire_type;
			int expire_time;
			int good_price;
			char good_status;
		}list[1];
	};

	struct cmd_dividend_mall_item_buy_failed
	{
		short index;
		char reason;
	};

	struct cmd_elf_exp
	{
		int exp;
	};

	struct cmd_public_quest_info
	{
		int task_id;
		int child_task_id;

		int score;
		int cls_place;
		int all_place;
	};

	struct cmd_public_quest_ranks
	{
		int task_id;
		int player_count;
		int ranks_size[NUM_PROFESSION+1];
		//后面是各个职业排行榜和一个总排行榜
		struct ranks_entry
		{
			int roleid;
			int race;
			int score;
			int rand_score;
			int place;
		}list[1];
	};

	struct cmd_multi_exp_info
	{
		int last_timestamp;     // 上次选择套餐的服务器时间
		float enhance_factor;   // 聚灵倍率
	};

	struct cmd_change_multi_exp_state
	{
		char state;					// 0-3，对应正常、聚灵、聚灵缓冲、散灵等状态
		int enchance_time;      // 聚灵剩余时间(s)
		int buffer_time;        // 聚灵缓冲剩余时间(s)
		int impair_time;        // 散灵剩余时间(s)
		int activate_times_left;	// 今天还剩余的开启聚灵次数
	};

	struct cmd_world_life_time
	{
		int life_time; // >=0:剩余时间 -1:无限
	};

	struct cmd_wedding_book_list 
	{
		int count;
		struct 
		{
			int start_time;
			int end_time;
			int groom;
			int bride;
			int scene;
			char status;
			char special;
		}list[1];
	};

	enum WEDDING_BOOK_STATUS
	{
		WBS_INVALID = 0,
		WBS_UNBOOKED,
		WBS_BOOKED,
		WBS_ONGOING,
		WBS_FINISH,
	};
	
	struct cmd_wedding_book_success
	{
		int type;
	};


	struct cmd_network_delay_re
	{
		int timestamp;
	};

	struct cmd_player_knockback
	{
		int id;
		A3DVECTOR3 pos;
		int time;
	};
	
	struct cmd_summon_plant_pet
	{
		int	plant_tid;
		int	plant_nid;
		int life_time;
	};

	struct cmd_plant_pet_disapper 
	{
		int plant_nid;
		char reason;	//	0 死亡，1 寿命到，2 超出范围，3 自爆，4 数量超出组上限
	};

	struct cmd_plant_pet_hp_notify 
	{
		int plant_nid;
		float hp_factor;
		int cur_hp;
		float mp_factor;
		int cur_mp;
	};

	struct cmd_pet_property
	{
		int pet_index;
		ROLEEXTPROP prop;
	};

	struct cmd_faction_contrib_notify 
	{
		int	consume_contrib;	//	可消费贡献度
		int	exp_contrib;		//	可兑换帮派经验贡献度
		int	cumulate_contrib;	//	累积贡献度
	};

	struct cmd_faction_fortress_info 
	{
		int	faction_id;			//	帮派id
		int	level;				//	基地等级
		int	exp;				//	基地经验
		int	exp_today;			//	当天获得的经验
		int	exp_today_time;		//	当天对应的零点
		int	tech_point;			//	剩余科技点数
		int	technology[5];		//	科技天赋的等级
		int	material[8];		//	各材料剩余数
		int	building_count;		//	已有基地设施数
		struct building_data 
		{
			int	id;				//	基地设施对应id
			int	finish_time;	//	基地设施建造完成时间点
		};
		abase::vector<building_data> building;
		int	health;				//	基地健康度

		bool Initialize(const void *pDataBuf, DWORD dwDataSize, DWORD *pSizeUsed = NULL)
		{
			//	提供初始化函数从缓冲中抽取“复杂”结构
			//
			bool bRet(false);
			while (true)
			{
				const byte *p = (const byte *)pDataBuf;
				DWORD s = dwDataSize;

				if (!Extract(faction_id, p, s) ||
					!Extract(level, p, s) ||
					!Extract(exp, p, s) ||
					!Extract(exp_today, p, s) ||
					!Extract(exp_today_time, p, s) ||
					!Extract(tech_point, p, s) ||
					!Extract(&technology[0], sizeof(technology)/sizeof(technology[0]), p, s) ||
					!Extract(&material[0], sizeof(material)/sizeof(material[0]), p, s) ||
					!Extract(building_count, p, s) || building_count <0)
					break;
				
				building.clear();
				if (building_count > 0)
				{
					building.reserve(building_count);
					building_data dummy;
					::memset(&dummy, 0, sizeof(dummy));
					for (int i = 0; i < building_count; ++ i)
						building.push_back(dummy);
					if (!Extract(&building[0], building_count, p, s))
						break;
				}
				if (!Extract(health, p, s))
					break;

				if (pSizeUsed)
					*pSizeUsed = dwDataSize-s;
				bRet = true;
				break;
			}
			return bRet;
		}
	};

	struct cmd_enter_factionfortress
	{
		int	role_in_war;
		int	factionid;
		int offense_endtime;
	};

	struct cmd_faction_relation_notify
	{
		typedef abase::vector<int> FIDArray;

		int	alliance_count;
		FIDArray alliance;		//	同盟帮派 id 列表

		int hostile_count;
		FIDArray hostile;		//	敌对帮派 id 列表
		
		bool Initialize(const void *pDataBuf, DWORD dwDataSize, DWORD *pSizeUsed = NULL)
		{
			//	提供初始化函数从缓冲中抽取“复杂”结构
			//
			bool bRet(false);
			while (true)
			{
				const byte *p = (const byte *)pDataBuf;
				DWORD s = dwDataSize;

				//	提取同盟帮派个数
				if (!Extract(alliance_count, p, s) || alliance_count <0)
					break;

				//	提取同盟帮派ID列表
				alliance.clear();
				if (alliance_count > 0)
				{
					alliance.reserve(alliance_count);
					for (int i = 0; i < alliance_count; ++ i)
						alliance.push_back(0);
					if (!Extract(&alliance[0], alliance_count, p, s))
						break;
				}

				//	提取敌对帮派个数
				if (!Extract(hostile_count, p, s) || hostile_count <0)
					break;

				//	提取敌对帮派ID列表
				hostile.clear();
				if (hostile_count > 0)
				{
					hostile.reserve(hostile_count);
					for (int i = 0; i < hostile_count; ++ i)
						hostile.push_back(0);
					if (!Extract(&hostile[0], hostile_count, p, s))
						break;
				}

				if (pSizeUsed)
					*pSizeUsed = dwDataSize-s;

				bRet = true;
				break;
			}
			return bRet;
		}
	};

	struct cmd_player_equip_disabled
	{
		int		id;		//roleid
		__int64 mask;	//当前失效的装备mask
	};

	struct player_spec_item_list
	{
		int roleid;
		int type;
		struct entry_t
		{
			unsigned char where;
			unsigned char index;
			size_t count;
		} list[1];

		bool CheckValid(size_t buf_size, size_t& sz) const
		{
			if(buf_size < sizeof(type) + sizeof(roleid))
				return false;

			sz = buf_size;
			size_t left = buf_size - sizeof(type) - sizeof(roleid);
			return (left % sizeof(entry_t)) == 0;
		}
	};

	struct cmd_object_start_play_action 
	{
		int	id;
		int	play_times;
		int	action_last_time;
		int	interval_time;
		size_t	name_length;
		char	action_name[1];
			
		bool CheckValid(size_t buf_size, size_t& sz) const
		{
			sz = sizeof(*this) - sizeof(action_name);

			if (buf_size < sz)
				return false;

			sz += name_length * sizeof(char);
			return buf_size == sz;
		}
	};

	struct cmd_object_stop_play_action 
	{
		int	id;
	};

	struct cmd_congregate_request
	{
		 char type;		// congregate type
		 int sponsor;	// sender role id
		 int timeout;	// NOTICE: unit is second
	};

	struct cmd_reject_congregate
	{
		char type;		// congregate type
		int id;			// rejector role id
	};

	// broadcast
	struct cmd_congregate_start
	{
		char type;		// congregate type
		int id;			// role id
		int time;       // NOTICE: unit is millisecond
	};

	// broadcast
	struct cmd_cancel_congregate
	{
		char type;		// congregate type
		int id;			// role id
	};

	struct cmd_engrave_start 
	{
		int id_engrave;
		int use_time;
	};

	struct cmd_engrave_result
	{
		int addon_num;		//	成功添加的镌刻属性数
	};

	struct cmd_dps_dph_rank 
	{
		int			next_refresh_sec;
		byte	rank_count;
		byte	rank_mask;
		struct _entry 
		{
			int		roleid;
			byte	level;
			int		value;
		}entry_list[1];
		
		bool CheckValid(size_t buf_size, size_t& sz) const
		{
			sz = sizeof(*this) - sizeof(entry_list);

			if (buf_size < sz)
				return false;

			sz += (rank_count) * sizeof(_entry);
			return buf_size == sz;
		}
	};

	struct cmd_addonregen_start 
	{       
		int recipe_id;
		int use_time;
	};      
	
	struct cmd_addonregen_result
	{       
		int addon_num;
	};

	struct cmd_invisible_obj_list 
	{
		int id;	// skill cast player id
		unsigned int count;
		int	id_list[1];

		bool CheckValid(size_t buf_size, size_t& sz) const
		{
			sz = sizeof(*this) - sizeof(id_list);

			if (buf_size < sz)
				return false;

			sz += count * sizeof(id_list[0]);
			return buf_size >= sz;
		}
	};

	struct cmd_set_player_limit
	{
		int	index;
		char	b;				//	1 限制 0 不限制
	};

	struct cmd_player_teleport
	{
		int	id;
		A3DVECTOR3	pos;
		unsigned short	use_time;
		char	mode;
	};

	struct cmd_object_forbid_be_selected
	{
		int id;
		char b;	//	1 限制 0 不限制
	};

	struct cmd_player_inventory_detail
	{
		int id;
		size_t money;
		unsigned char inv_size;
		size_t content_length;
		char content[1];

		bool CheckValid(size_t buf_size, size_t& sz) const
		{
			sz = sizeof(*this) - sizeof(content);

			if (buf_size < sz)
				return false;

			sz += content_length;
			return buf_size >= sz;
		}
	};

	struct cmd_player_force_data
	{
		int cur_force_id;
		size_t count;
		struct _entry{
			int force_id;
			int reputation;
			int contribution;
		}entry_list[1];

		bool CheckValid(size_t buf_size, size_t& sz) const
		{
			sz = sizeof(*this) - sizeof(entry_list);

			if (buf_size < sz)
				return false;

			sz += count * sizeof(_entry);
			return buf_size >= sz;
		}
	};

	struct cmd_player_force_changed
	{
		int id;
		int cur_force_id;
	};

	struct cmd_player_force_data_update
	{
		int force_id;
		int reputation;
		int contribution;
	};

	struct cmd_force_global_data
	{
		char data_ready;
		size_t count;
		struct _entry{
			int force_id;
			int player_count;
			int development;
			int construction;
			int activity;		// 当日的活跃度
			int activity_level; // 活跃度排名
		}entry_list[1];

		bool CheckValid(size_t buf_size, size_t& sz) const
		{
			sz = sizeof(*this) - sizeof(entry_list);

			if (buf_size < sz)
				return false;

			sz += count * sizeof(_entry);
			return buf_size >= sz;
		}
	};

	struct cmd_multiobj_effect
	{
		int id;
		int target;
		char type;
	};
	
	struct enter_wedding_scene
	{
		int groom;
		int bride; 
	};

	struct produce4_item_info
	{
		int stime;
		int type;
		int expire_date;
		int proc_type;
		size_t count;
		unsigned short crc;
		unsigned short content_length;
		char content[1];
		bool CheckValid(size_t buf_size, size_t& sz) const
		{
			sz = sizeof(*this) - sizeof(content);
			
			if (buf_size < sz)
				return false;
			
			sz += content_length;
			return buf_size >= sz;
		}
	};

	struct cmd_online_award_data 
	{
		int total_award_time;//总奖励时间
		size_t count;
		struct _entry{
			int type; //奖励类型
			int time;//奖励剩余时间
			int reserved;//
		}entry_list[1];

		bool CheckValid(size_t buf_size, size_t& sz) const
		{
			sz = sizeof(*this) - sizeof(entry_list);
			
			if (buf_size < sz)
				return false;
			
			sz += count * sizeof(_entry);
			return buf_size >= sz;
		}
	};

	struct cmd_toggle_online_award
	{
		int type; //奖励类型
		char activate;
    };

	enum SERVER_SEND_PROFIT_TIME_REASON
	{
		PLAYER_QUERY,		// 客户端请求
		PLAYER_ONLINE,		// 玩家上线
		PLAYER_SWITCH_SCENE,// 切换地图
		PROFIT_LEVEL_CHANGE,// 收益状态改变
	};
	struct cmd_player_profit_time
	{
		char reason;
		char profit_map;
		int profit_time;
		int profit_level;
	};

	struct cmd_nonpenalty_pvp_state
	{
		char state;
	};

	struct cmd_self_country_notify
	{
		int	country_id;
	};

	struct cmd_player_country_changed
	{
		int	id;
		int	country_id;
	};

	struct cmd_enter_countrybattle
	{
		int	role_in_war;	//	0 中立 1 攻 2守
		int	battle_id;
		int	end_time;
		int	offense_country;
		int	defence_country;
	};

	struct cmd_countrybattle_result
	{
		int	result;
	};

	struct cmd_countrybattle_score
	{
		int offense_score;
		int offense_goal;
		int defence_score;
		int defence_goal;
	};
	
	struct cmd_countrybattle_resurrect_rest_times
	{
		int times;
	};
	
	struct cmd_countrybattle_flag_carrier_notify
	{
		int			id;
		A3DVECTOR3	pos;
		char		offense;
	};
	
	struct cmd_countrybattle_became_flag_carrier
	{
		char is_carrier;
	};
	
	struct cmd_countrybattle_personal_score
	{
		int		combat_time;
		int		attend_time;
		int		kill_count;
		int		death_count;
		int		country_kill_count;
		int		country_death_count;
	};

	enum
	{
		FLAG_MSG_GENERATE,  //	旗子生成
		FLAG_MSG_PICKUP,	//	拔起旗子
		FLAG_MSG_HANDIN,	//	交纳旗子
		FLAG_MSG_LOST,		//	旗帜丢失
	};
	struct cmd_countrybattle_flag_msg_notify
	{
		int		msg;
		char	offense;
	};
	struct cmd_defense_rune_enable
	{
		char	rune_type;	//	0:物理防御优化符 1:法术防御优化符
		char	enable;
	};
	
	struct cmd_countrybattle_info
	{
		int		attacker_count;
		int		defender_count;
	};

	struct cmd_set_profit_time
	{
		char enable;	//1: 开始扣减收益时间， 0:停止扣减收益时间
	};

	struct cmd_cash_money_exchg_rate
	{
		bool open;
		int rate;
	};
	struct cmd_pet_rebuild_inherit_start
	{
		size_t index;
		int use_time;
	};
	
	struct cmd_pet_rebuild_inherit_info
	{
		int stime; //超时时间
		int pet_id;
		size_t index;
		int r_attack;
		int r_defense;
		int r_hp;
		int r_atk_lvl;
		int r_def_lvl;
	};
	
	struct cmd_pet_rebuild_inherit_end
	{
		size_t index;
	};
	
	struct cmd_pet_evolution_done
	{
		size_t index;
	};
	
	struct cmd_pet_rebuild_nature_start
	{
		size_t index;
		int use_time;
	};
	
	struct cmd_pet_rebuild_nature_info
	{
		int stime;
		int pet_id;
		size_t index;
		int nature;
	};
	
	struct cmd_pet_rebuild_nature_end
	{
		size_t index;
	};

	struct cmd_equip_addon_update_notify //通知装备魂石变更cmd结构
	{
		unsigned char update_type; //0 for change 1 for replace
		unsigned char equip_idx;
		unsigned char equip_socket_idx;
		int old_stone_type;
		int new_stone_type;
	};

	struct cmd_self_king_notify
	{
		char is_king;           // 是否为国王
		int expire_time;		// 过期时间点 服务器时间
	};
	
	struct cmd_player_king_changed
	{
		int id;
		char is_king;			// 是否为国王
	};
	struct cmd_notify_meridian_data
	{
		int meridian_level; //经脉等级(0-80)
		int lifegate_times; //已经连续点开的生门数
		int deathgate_times; //已经点开的死门数
		int free_refine_times; //剩余免费精炼次数
		int paid_refine_times; //剩余收费精炼次数
		int continu_login_days; //连续登陆天数
		int trigrams_map[3];//八卦图，每2位表示一个门，00为未冲击，01为死门，10为生门
	};

	struct cmd_try_refine_meridian_result
	{
		int index; //八卦门序号
		int result; //操作结果 1:生门，但是经脉未升级
					//			2：生门且升级
					//				3：死门，但是未失败
					//				4：死门，且失败
	};

	struct cmd_stronghold_state_notify
	{
		int	count;
		int	state[1];
		
		bool CheckValid(size_t buf_size, size_t& sz) const
		{
			if (count < 0) return false;
			sz = sizeof(*this) - sizeof(state);
			if (count > 0){
				sz += count * sizeof(state[0]);
			}
			return buf_size >= sz;
		}
	};
	struct cmd_query_touch_point
	{
		__int64	income; // 总计获得点数
		__int64 remain; // 当前可用点数
		int retcode;
	};
	
	struct cmd_spend_touch_point
	{
		__int64	income; // 总计获得点数
		__int64 remain; // 当前可用点数
		unsigned int cost; // 本次消费点数
		unsigned int index; // 商品索引
		unsigned int lots; // 是否
		int retcode;
	};
	struct cmd_total_recharge
	{
		__int64 recharge;
	};
	struct cmd_query_title_re
	{
		int roleid;
		int titlescount;
		int expirecount;
		unsigned short titles[1];
		struct _entry
		{
			unsigned short id;
			int time;
		}data[1];

		bool CheckValid(size_t buf_size, size_t& sz) const
		{
			sz = sizeof(*this) - sizeof(titles) - sizeof(data);
			
			if (buf_size < sz)
				return false;
			
			sz += titlescount * sizeof(unsigned short) + expirecount * sizeof(_entry);
			return buf_size >= sz;
		}
	};

	struct cmd_change_curr_title_re
	{
		int roleid;
		unsigned short titleid;
	};
	struct cmd_modify_title_notify
	{
		unsigned short id;
		int expiretime;
		char flag;
	};
	struct cmd_refresh_signin
	{
		char	type; // 同步原因 0初始1数据不同步2签到3补签4领奖
		int		monthcalendar;// 当前月签到日历
		int		curryearstate; // 当前年月份签到状态
		int		lastyearstate; // 去年月份签到状态
		int		updatetime;  // 签到数据最后一次变更时间  
		int		localtime;   // 当前服务器时间
		char	awardedtimes;// 本月已领奖次数
		char	latesignintimes;//本月已补签次数
	};
	struct cmd_parallel_world_info
	{
		int	worldtag;
		int	count;
		struct _entry
		{
			int		line;
			float	load;
		}list[1];		
		
		bool CheckValid(size_t buf_size, size_t& sz) const
		{
			if (count < 0) return false;
			sz = sizeof(*this) - sizeof(list);
			if (count > 0){
				sz += count * sizeof(list[0]);
			}
			return buf_size >= sz;
		}
	};
	struct cmd_player_reincarnation
	{
		int id;
		int reincarnation_times;
	};
	struct cmd_reincarnation_tome_info
	{
		int tome_exp;
		char tome_active;   // 1激活0未激活
		int count;
		struct _entry
		{
			int level;
			int timestamp;
			int exp;
		}records[1];
		bool CheckValid(size_t buf_size, size_t& sz) const
		{
			if (count < 0)
				return false;
			sz = sizeof(*this) - sizeof(records);	
			sz += count * sizeof(_entry);
			return buf_size >= sz;
		}
	};
	struct cmd_activate_reincarnation_tome
	{
		char active;
	};


	struct cmd_unique_data_notify 
	{
		int count;
		struct UNIQUEDATA {
			UNIQUEDATA() { key=0;type=0;len=0;pValue=NULL;}
			int key;
			int type; //全局数据类型 |0 未初始化|1 INT| 2 DOUBLE| 3 变长
			int len;
			char* pValue;
		} *data;

		cmd_unique_data_notify(){ count = 0; data = NULL;}
		~cmd_unique_data_notify()
		{
			for (int i=0;i<count;i++)
			{
				if(data && data[i].pValue)
					delete [] data[i].pValue;
			}
			if(data && count)
				delete [] data;
		}

		bool Initialize(const void *pDataBuf, DWORD buf_size,DWORD* used_size=NULL)
		{
			const byte* pData = (const byte*)pDataBuf;
			DWORD totalLen = buf_size;
			bool bRet = false;
			while(true)
			{
				if(!Extract(count,pData,buf_size))
					break;
				if(count<1)
					break;
				data = new UNIQUEDATA[count];
				int i;
				for (i=0;i<count;i++)
				{
					if(!Extract(data[i].key,pData,buf_size))
						break;
					if(!Extract(data[i].type,pData,buf_size))
						break;
					if(!Extract(data[i].len,pData,buf_size) || data[i].len==0)
						break;
					if (buf_size>=(DWORD)data[i].len)
					{
						data[i].pValue = new char[data[i].len];
						if(!Extract(data[i].pValue,data[i].len,pData,buf_size))
							break;
					}
					else
						break;
				}
				bRet = true;
				break;
			}	
			if(used_size)
				*used_size = totalLen - buf_size;
			if(buf_size==0 && bRet)
				return true;
			else
				return false;
		}
	};
	struct cmd_use_giftcard_result
	{
		int retcode;
		int cardtype;
		int parenttype;
		char cardnumber[20];
	};
	struct cmd_realm_exp
	{
		int exp;
		int receive_exp;
	};
	struct cmd_realm_level
	{
		int roleid;
		unsigned char level;
	};

	struct cmd_enter_trickbattle // 跨服活动
	{
		int role_in_war;    //
		int battle_id;
		int end_time;
	};
	struct cmd_trickbattle_personal_score // 跨服活动
	{
		int kill;
		int death;
		int score;
		int multi_kill;
	};
	struct cmd_trickbattle_chariot_info
	{
		int chariot;
		int energy;
	};
	struct cmd_player_leadership
	{
		int leadership_total;
		int leadership_added;
	};
	struct cmd_generalcard_collection_data
	{
		unsigned int count;
		char state[1];
		
		bool CheckValid(size_t buf_size, size_t& sz) const
		{
			if (count < 0) return false;
			sz = sizeof(*this) - sizeof(state);
			if (count > 0){
				sz += count * sizeof(state[0]);
			}
			return buf_size >= sz;
		}
	};
	struct cmd_add_generalcard_collection
	{
		unsigned int show_order;
	};
	struct cmd_refresh_monsterspirit_level
	{
		int gain_times;
		unsigned int count;
		struct _entry
		{
			int level;
			int exp;
		}entries[1];
		bool CheckValid(size_t buf_size, size_t& sz) const
		{
			if (count != 6)
				return false;
			sz = sizeof(*this) - sizeof(entries);	
			sz += count * sizeof(_entry);
			return buf_size >= sz;
		}
	};
	struct cmd_mine_gathered
	{
		int mine_id;
		int player_id;
		int item_type;
	};
	struct cmd_player_in_out_battle
	{
		int player_id;
		bool in_out; // true:进入战斗；false:脱离战斗
	};

	struct cmd_player_query_chariots
	{
		unsigned int attacker_count;
		unsigned int defender_count;

		struct  chariot
		{
			int type;
			int count;
		} chariots[1];
	
		bool CheckValid(size_t buf_size, size_t& sz) const
		{
			sz = sizeof(*this) - sizeof(chariots);
			
			if (buf_size < sz)
				return false;
			
			sz += (attacker_count + defender_count)  * sizeof(chariot);

			return buf_size >= sz;
		}
	};
	
	struct cmd_countrybattle_live_show_result
	{
		struct score_rank_entry
		{
			int roleid;
			int rank;
			A3DVECTOR3 pos;

			score_rank_entry():roleid(0), rank(0), pos(0.f, 0.f, 0.f){}
		};
		struct death_entry
		{
			int roleid;
			int timestamp;
			A3DVECTOR3 pos;

			death_entry():roleid(0), timestamp(0), pos(0.f, 0.f, 0.f){}
		};
		typedef abase::vector<score_rank_entry> ScoreRankContainer;
		typedef abase::vector<death_entry> DeathContainer;
		ScoreRankContainer defence_ranks;
		DeathContainer defence_deaths;
		ScoreRankContainer offense_ranks;
		DeathContainer offense_deaths;

		bool Initialize(const void *pDataBuf, DWORD dwDataSize, DWORD *pSizeUsed = NULL)
		{
			bool ret(false);
			while (true)
			{
				const byte *p = (const byte *)pDataBuf;
				DWORD s = dwDataSize;
				unsigned int count;
				// 守方排名个数
				if (!Extract(count, p, s))
					break;
				if (count > 0) {
					// 初始化守方排名
					bool error(false);
					defence_ranks.reserve(count);
					for (unsigned int i = 0; i < count; ++i) {
						score_rank_entry ent;
						if (!Extract(ent, p, s)) {
							error = true;
							break;
						}
						defence_ranks.push_back(ent);
					}
					if (error) break;
				}
				// 守方死亡个数
				if (!Extract(count, p, s))
					break;
				if (count > 0) {
					// 初始化守方死亡
					bool error(false);
					defence_deaths.reserve(count);
					for (unsigned int i = 0; i < count; ++i) {
						death_entry ent;
						if (!Extract(ent, p, s)) {
							error = true;
							break;
						}
						defence_deaths.push_back(ent);
					}
					if (error) break;
				}
				// 攻方排名个数
				if (!Extract(count, p, s))
					break;
				if (count > 0) {
					// 初始化攻方排名
					bool error(false);
					offense_ranks.reserve(count);
					for (unsigned int i = 0; i < count; ++i) {
						score_rank_entry ent;
						if (!Extract(ent, p, s)) {
							error = true;
							break;
						}
						offense_ranks.push_back(ent);
					}
					if (error) break;
				}
				// 攻方死亡个数
				if (!Extract(count, p, s))
					break;
				if (count > 0) {
					// 初始化攻方死亡
					bool error(false);
					offense_deaths.reserve(count);
					for (unsigned int i = 0; i < count; ++i) {
						death_entry ent;
						if (!Extract(ent, p, s)) {
							error = true;
							break;
						}
						offense_deaths.push_back(ent);
					}
					if (error) break;
				}
				if (pSizeUsed)
					*pSizeUsed = dwDataSize-s;
				ret = true;
				break;
			}
			return ret;
		}
	};

	struct  cmd_rand_mall_shopping_res
	{
		int		config_id;		//	随机商城配置表ID
		int		op;				//	对此随机配置表的操作
		int		result;			//	操作结果（参照 RAND_MALL_SHOPPING_RES_OK 等）
		int		item_to_pay;	//	已随机、待确认付款的物品ID
		int		price;			//	待付款/已付款物品价格（元宝）
		bool	firstflag;		//	是否首次购买
	};
	struct cmd_faction_pvp_mask_modify
	{
		int		roleid;			//	接受改变的玩家ID
		unsigned char mask;		//	新的状态Mask
	};
	struct cmd_player_world_contribution
	{
		int contribution;
		int change;
		int total_spend;
	};

	struct cmd_random_map_order
	{
		int world_tag;
		int row;
		int col;
		int room_order[1];
		
		bool CheckValid(size_t buf_size, size_t& sz) const
		{
			int num = row * col;
			if (num < 0) return false;
			sz = sizeof(*this) - sizeof(room_order);
			if (num > 0){
				sz += num * sizeof(room_order[0]);
			}
			return buf_size >= sz;
		}
	};

	struct cmd_scene_service_npc_list
	{
		unsigned int count;
		struct {
			int tid;			// npc模板的编号
			int nid;			// npc编号
		} list[1];
		bool CheckValid(size_t buf_size, size_t& sz) const {
			if (buf_size < sizeof(count)) {
				return false;
			}
			sz = sizeof(count) + count * sizeof(list);
			return buf_size >= sz;
		}
	};

	struct cmd_equip_can_inherit_addons
	{       
		int equip_id;
		unsigned char inv_idx;
		unsigned char addon_num;
		int addon_id_list[1];
		bool CheckValid(size_t buf_size, size_t& sz) const
		{
			if (addon_num < 0) return false;
			sz = sizeof(*this) - sizeof(addon_id_list[0]);
			if (addon_num > 0){
				sz += addon_num * sizeof(addon_id_list[0]);
			}
			return buf_size >= sz;
		}
    }; 
	struct cmd_npc_visible_tid_notify
	{
		int nid;
		int vis_tid;
	};
	
	struct cmd_client_screen_effect
	{
		int type;		// efect type: 0 screen, 1 gfx;
		int eid;		// effect id
		int state;		// 0: close, 1: open
	};

	struct cmd_combo_skill_prepare
	{
		int skill_id;
		int timestamp;
		int args[3];
	};

	struct cmd_instance_reenter_notify
	{
		int	world_tag;	// 副本地图 ID
		int time_out;	// 截止时戳
	};
	
	struct cmd_pray_distance_change{
		float pray_distance_plus;
	};
}

namespace C2S
{
	typedef unsigned char byte;

	//	Data type ---------------------------
	struct npc_trade_item
	{
		int tid;
		size_t index;
		size_t count;
	};

	struct npc_sell_item
	{
		int tid;
		size_t index;
		size_t count;
		int price;
	};
	
	struct npc_booth_item
	{
		int tid;
		unsigned short index;
		unsigned short inv_index;
		unsigned int count;
	};

	//	Commands ----------------------------
	enum
	{
		PLAYER_MOVE,		//	0
		LOGOUT,
		SELECT_TARGET,
		NORMAL_ATTACK,
		REVIVE_VILLAGE,		//	Revive in near village

		REVIVE_ITEM,		//	5, Revive by using item
		PICKUP,
		STOP_MOVE,
		UNSELECT,
		GET_ITEM_INFO,

		GET_IVTR,			//	10, Get inventory information
		GET_IVTR_DETAIL,	//	Get inventory detail information
		EXG_IVTR_ITEM,
		MOVE_IVTR_ITEM,
		DROP_IVTR_ITEM,

		DROP_EQUIP_ITEM,	//	15
		EXG_EQUIP_ITEM,
		EQUIP_ITEM,
		MOVE_ITEM_TO_EQUIP,
		GOTO,
		
		THROW_MONEY,		//	20
		GET_EXT_PROP,
		SET_STATUS_POINT,	//	Set state points
		GET_EXT_PROP_BASE,
		GET_EXT_PROP_MOVE,

		GET_EXT_PROP_ATK,	//	25
		GET_EXT_PROP_DEF,
		TEAM_INVITE,
		TEAM_AGREE_INVITE,
		TEAM_REJECT_INVITE,

		TEAM_LEAVE_PARTY,	//	30
		TEAM_KICK_MEMBER,
		TEAM_MEMBER_POS,	//	Get team member's position
		GET_OTHER_EQUIP,
		TEAM_SET_PICKUP,	//	Change team pickup flag

		SEVNPC_HELLO,		//	35, say hello to service NPC
		SEVNPC_GET_CONTENT,
		SEVNPC_SERVE,
		GET_OWN_WEALTH,
		GET_ALL_DATA,		//  取得所有数据 进入游戏时使用，传回所有的包裹，金钱和技能

		USE_ITEM,			//	40, 使用一个物品
		CAST_SKILL,
		CANCEL_ACTION,
		CHARGE_E_FLYSWORD,	//	Charge flysword which is on equipment bar
		CHARGE_FLYSWORD,

		USE_ITEM_T,			//	45, use item with target
        SIT_DOWN,
        STAND_UP,
        EMOTE_ACTION,
        TASK_NOTIFY,

		ASSIST_SELECT,		//	50
		CONTINUE_ACTION,
		STOP_FALL,			//	终止跌落
		GET_ITEM_INFO_LIST,
		GATHER_MATERIAL,

		GET_TRASHBOX_INFO,	//	55
		EXG_TRASHBOX_ITEM,
		MOVE_TRASHBOX_ITEM,
		EXG_TRASHBOX_IVTR,
		TRASHBOX_ITEM_TO_IVTR,

		IVTR_ITEM_TO_TRASHBOX,	//	60
		EXG_TRASHBOX_MONEY,
		TRICK_ACTION,
        SET_ADV_DATA,
        CLR_ADV_DATA,

        TEAM_ASK_TO_JOIN,		//	65
        TEAM_REPLY_JOIN_ASK,
		QUERY_PLAYER_INFO_1,
		QUERY_NPC_INFO_1,
		SESSION_EMOTE,

		CON_EMOTE_REQUEST,		//	70
		CON_EMOTE_REPLY,
		CHANGE_TEAM_LEADER,		
		DEAD_MOVE,
		DEAD_STOP_MOVE,

        ENTER_SANCTUARY,		//	75
		OPEN_BOOTH,
		CLOSE_BOOTH,
		QUERY_BOOTH_NAME,
		COMPLETE_TRAVEL,

		CAST_INSTANT_SKILL,		//	80
		DESTROY_ITEM,
		ENABLE_PVP_STATE,
		DISABLE_PVP_STATE,
		OPEN_BOOTH_TEST,

		SWITCH_FASHION_MODE,	//	85
		ENTER_INSTANCE,
		REVIVAL_AGREE,
		NOTIFY_POS_IN_TEAM,
		CAST_POS_SKILL,

		ACTIVE_RUSH_FLY,		//	90
		QUERY_DOUBLE_EXP,
		DUEL_REQUEST,
		DUEL_REPLY,
		BIND_PLAYER_REQUEST,

		BIND_PLAYER_INVITE,		//	95
		BIND_PLAYER_REQUEST_REPLY,
		BIND_PLAYER_INVITE_REPLY,
		CANCEL_BIND_PLAYER,
		GET_OTHER_EQUIP_DETAIL,

		SUMMON_PET,				//	100
		RECALL_PET,
		BANISH_PET,
		PET_CTRL,
		DEBUG_DELIVER_CMD,

		DEBUG_GS_CMD,			//	105
		MALL_SHOPPING,
		GET_WALLOW_INFO,
		TEAM_DISMISS_PARTY,
		USE_ITEM_WITH_ARG,
		
		QUERY_CASH_INFO,		//	110
		ELF_ADD_ATTRIBUTE,
		ELF_ADD_GENIUS,
		ELF_PLAYER_INSERT_EXP,
		ELF_EQUIP_ITEM,

		ELF_CHANGE_SECURE_STATUS,	//	115
		CAST_ELF_SKILL,
		RECHARGE_EQUIPPED_ELF,
		GET_MALL_ITEM_PRICE,
		EQUIP_FASHION_ITEM,
		
		CHECK_SECURITY_PASSWD,	//	120
		NOTIFY_FORCE_ATTACK,
		DIVIDEND_MALL_SHOPPING,
		GET_DIVIDEND_MALL_ITEM_PRICE,
		CHOOSE_MULTI_EXP,

		TOGGLE_MULTI_EXP,       // 125
		MULTI_EXCHANGE_ITEM,
		SYSAUCTION_OP,
		CALC_NETWORK_DELAY,
		GET_FACTION_FORTRESS_INFO,

		CONGREGATE_REPLY,		// 130
		GET_FORCE_GLOBAL_DATA,
		PRODUCE4_CHOOSE,    
		RECHARGE_ONLINE_AWARD, //冲在线奖励时间请求
		TOGGLE_ONLINE_AWARD,  //切换在线奖励请求

		QUERY_PROFIT_TIME,		// 查询当前收益数据
		ENTER_PK_PROTECTED,		// 进入pk保护区
		COUNTRYBATTLE_GET_PERSONAL_SCORE,	// 获取国战个人积分
		GET_SERVER_TIMESTAMP,	//	同步GS服务器时间
		COUNTRYBATTLE_LEAVE,	// 离开国战战场

		GET_CASH_MONEY_EXCHG_RATE,	// 140
		EVOLUTION_PET,					// 宠物进化   
		ADD_PET_EXP,					// 喂养
		REBUILD_PET_NATURE,				// 性格培训
		REBUILD_PET_INHERIT_RATIO,		// 洗髓

		PET_REBUILD_INHERIT_CHOOSE,			// 是否选择新属性，洗髓   145
		PET_REBUILD_NATURE_CHOOSE,			// 是否选择新性格，性格培训  
		EXCHANGE_WANMEI_YINPIAO,
		PLAYER_GIVE_PRESENT,
		PLAYER_ASK_FOR_PRESENT,

		MERIDIANS_IMPACT, // 经脉冲击 150
		COUNTRYBATTLE_GET_STRONGHOLD_STATE,
		QUERY_TOUCH_POINT,
		SPEND_TOUCH_POINT,
		QUERY_TITLE,

		CHANGE_CURR_TITLE, // 155
		DAILY_SIGNIN,	   // 每日签到
		LATE_SIGNIN,	   // 补签
		APPLY_SIGNINAWARD, // 领取签到奖励
		REFRESH_SIGNIN,	   // 刷新签到数据

		SWITCH_IN_PARALLEL_WORLD,	//	160 切换分线
		QUERY_PARALLEL_WORLD,		//	查询当前所在地图分线
		GET_REINCARNATION_TOME,
		REWRITE_REINCARNATION_TOME,
		ACTIVATE_REINCARNATION_TOME,
		
		QUERY_UNIQUE_DATA,   // 165 , 请求历史推进 key值所对应全局数据
		AUTO_TEAM_SET_GOAL,
		AUTO_TEAM_JUMP_TO_GOAL,
		TRICKBATTLE_LEAVE,   //    离开跨服活动战场
		TRICKBATTLE_UPGRADE_CHARIOT, //  升级战车

		SWALLOW_GENERALCARD,	// 170 吞噬卡牌或卡牌随机发生器
		EQUIP_TRASHBOX_ITEM,	// 从随身仓库(时装，卡牌)装备物品
		QUERY_TRICKBATTLE_CHARIOTS, // 查询战车数量
		COUNTRYBATTLE_LIVE_SHOW, // 查询国战rank和死亡数据
		SEND_MASS_MAIL,			// 群发邮件
		
		RANDOM_MALL_SHOPPING,	//	175 随机商城请求
		QUERY_MAFIA_PVP_INFO,	//	请求帮派PVP数据
		QUERY_CAN_INHERIT_ADDONS, // 请求筛选可继承附加属性数据		
		ACTIVATE_REGION_WAYPOINTS,// 激活安全区传送点
		INSTANCE_REENTER_REQUEST,	//	重入副本
		

		NUM_C2SCMD,				//	Number of C2S commands.
		
		//	Below are GM commands
		GM_CMD_START = 200,		//	200
		GM_MOVETO_PLAYER,
		GM_CALLIN_PLAYER,
		GM_KICK_PLAYER,			//	需要选中目标,无参数
		GM_INVISIBLE,			//	切换自身隐身,无参数

		GM_INVINCIBLE,			//	205, 切换自身无敌,无参数
		GM_GENERATE,
		GM_ACTIVE_SPAWNER,
		GM_GENERATE_MOB,

		//209 ~ 217 obsoleted
        
        GM_QUERY_SPEC_ITEM = 218,	// query player's trashbox detail
        GM_REMOVE_SPEC_ITEM,		// remove items in trashbox
		GM_OPEN_ACTIVITY,			// GM open activity
		GM_CHANGE_DS,				// GM 跨服
	};

	struct cmd_header
	{
        unsigned short cmd;
	};

	struct cmd_player_move
	{
		A3DVECTOR3 vCurPos;
		A3DVECTOR3 vNextPos;
		unsigned short use_time;
		short sSpeed;				//	Move speed 8.8 fix-point
		unsigned char move_mode;	//	Walk run swim fly .... walk_back run_back
		unsigned short stamp;		//	move command stamp
	};

	struct cmd_stop_move
	{
		A3DVECTOR3 vCurPos;
		short sSpeed;				//	Moving speed in 8.8 fix-point
		unsigned char dir;			//	对象的方向
		unsigned char move_mode;	//	Walk run swim fly .... walk_back run_back
		unsigned short stamp;		//	move command stamp
		unsigned short use_time;
	};

	struct cmd_player_logout
	{
		int iOutType;
	};
	
	struct cmd_select_target
	{
		int id;
	};

	struct cmd_normal_attack
	{
		unsigned char pvp_mask;
	};

	struct cmd_pickup
	{
		int	idItem;
		int tid;			//	id of template
	};

	struct cmd_get_item_info
	{
		unsigned char byPackage;
		unsigned char bySlot;
	};

	struct cmd_get_ivtr
	{
		unsigned char byPackage;
	};

	struct cmd_get_ivtr_detail
	{
		unsigned char byPackage;
	};

	struct cmd_exg_ivtr_item
	{
		unsigned char index1;
		unsigned char index2;
	};

	struct cmd_move_ivtr_item
	{
		unsigned char src;
		unsigned char dest;
		unsigned int amount;
	};

	struct cmd_drop_ivtr_item
	{
		unsigned char index;
		unsigned int amount;
	};

	struct cmd_drop_equip_item	
	{
		unsigned char index;
	};

	struct cmd_exg_equip_item
	{
		unsigned char idx1;
		unsigned char idx2;
	};

	struct cmd_equip_item
	{
		unsigned char idx_inv;
		unsigned char idx_eq;
	};

	struct cmd_move_item_to_equip
	{
		unsigned char idx_inv;  //src
		unsigned char idx_eq;   //dest
	};

	struct cmd_goto
	{
		A3DVECTOR3 vDest;
	};

	struct cmd_throw_money
	{
		size_t amount;
	};

	struct cmd_set_status_pt
	{
		size_t vitality;
		size_t energy;
		size_t strength;
		size_t agility;
	};

	struct cmd_team_invite
	{
		int idPlayer;	//	向谁发起邀请 
	};

	struct cmd_team_agree_invite
	{
		int idLeader;	//	谁进行的邀请
		int team_seq;
	};

	struct cmd_team_reject_invite
	{
		int idLeader;	//	谁进行的邀请
	};

	struct cmd_team_kick_member
	{
		int idMember;
	};

	struct cmd_team_member_pos
	{
		unsigned short wMemCnt;
		int aMemIDs[1];
	};

	struct cmd_get_other_equip
	{
		unsigned short size;
		int idlist[1];
	};

	struct cmd_team_set_pickup
	{
		short pickup_flag;
	};

	struct cmd_sevnpc_hello
	{
		int id;
	};

	struct cmd_sevnpc_get_content
	{
		int service_id;
	};

	struct cmd_sevnpc_serve
	{
		int service_type;
		size_t len;
	};

	struct cmd_get_own_wealth
	{
		BYTE byPack;	//	Get detail info. flag
		BYTE byEquip;
		BYTE byTask;
	};

	struct cmd_get_all_data
	{
		BYTE byPack;	//	Get detail info. flag
		BYTE byEquip;
		BYTE byTask;
	};

	struct cmd_use_item
	{
		unsigned char where;
		unsigned char byCount;
		unsigned short index;
		int item_id;
	};

	struct cmd_cast_skill
	{
		int skill_id;
		unsigned char pvp_mask;
		unsigned char target_count;
		int targets[1];
	};

	struct cmd_charge_equipped_flysword
	{
		unsigned char element_index; 
		int count;
	};

	struct cmd_charge_flysword
	{
		unsigned char element_index;  
		unsigned char flysword_index; 
		int count;
		int flysword_id;
	};

	struct cmd_use_item_t
	{
		unsigned char where;
		unsigned char pvp_mask;		//	只对攻击性物品有效
		unsigned short index;
		int item_id;
	};

	struct cmd_emote_action
	{
		unsigned short action;
	};

	struct cmd_task_notify
	{
		unsigned int size;
		byte placeholder;	//	Task data ...
	}; 

	struct cmd_get_item_info_list
	{
		char where;
		unsigned char count;
		//	follows: unsigned char item_list[1];
	};

	struct cmd_assist_sel
	{
		int idTeamMember;
	};

	struct cmd_gather_material
	{
		int mid;
		unsigned short tool_pack;
		unsigned short tool_index;
		int tool_type;
		int id_task;
	};

	struct cmd_get_trashbox_info
	{
		char is_accountbox;		// 1-帐号仓库  0-角色仓库
		char detail;
	};

	struct cmd_exg_trashbox_item
	{
		unsigned char where;
		unsigned char index1;
		unsigned char index2;
	};

	struct cmd_move_trashbox_item
	{
		unsigned char where;
		unsigned char src;
		unsigned char dest;
		unsigned int amount;
	};

	struct cmd_exg_trashbox_ivtr
	{
		unsigned char where;
		unsigned char idx_tra;
		unsigned char idx_inv;
	};

	struct cmd_trashbox_item_to_ivtr
	{
		unsigned char where;
		unsigned char idx_tra;
		unsigned char idx_inv;
		unsigned int amount;
	};

	struct cmd_ivtr_item_to_trashbox
	{
		unsigned char where;
		unsigned char idx_inv;
		unsigned char idx_tra;
		unsigned int amount;
	};

	struct cmd_exg_trashbox_money
	{
		char is_accountbox;
		unsigned int inv_money;   //从身上取出的钱数
		unsigned int trashbox_money;  //从仓库中取得钱数
	};

	struct cmd_trick_action
	{
		unsigned char trick;
	};

	struct cmd_set_adv_data
	{
		int data1;
		int data2;
	};

	struct cmd_team_ask_join
	{
		int idTarget;
	};

	struct cmd_team_reply_join_ask
	{
		int idAsker;
		unsigned char result;
	};

	struct cmd_query_player_info_1
	{
		unsigned short count;
		int id[1];
	};

	struct cmd_query_npc_info_1
	{
		unsigned short count;
		int id[1];
	};

	struct cmd_session_emote
	{
		unsigned char action;
	};

	struct cmd_con_emote_request
	{
		unsigned short action;
		int target;
	};

	struct cmd_con_emote_reply
	{
		unsigned short result;
		unsigned short action;
		int target;
	};

	struct cmd_change_team_leader
	{
		int idLeader;
	};

	struct cmd_dead_move
	{
		float y;
		unsigned short use_time;
		unsigned short speed;
		unsigned char move_mode;
	};

	struct cmd_dead_stop_move
	{
		float y;
		unsigned short speed;
		unsigned char dir;
		unsigned char move_mode; 
	};

	struct cmd_open_booth
	{
		unsigned short count;
		char name[62];

		struct entry_t
		{
			int type;
			size_t index;
			size_t count;
			size_t price;

		} list[1];
	};

	struct cmd_query_booth_name
	{       
		unsigned short count;
		int list[1];
	};

	struct cmd_enter_instance
	{
		int iTransIndex;
		int idInst;
	};

	struct cmd_enter_sanctuary
	{
		int id;		// self id or pet id.
	};

	struct cmd_cast_pos_skill
	{
		int skill_id;
		A3DVECTOR3 pos;
		unsigned char force_attack;
		unsigned char target_count;
		int targets[1];
	};

	struct cmd_active_rush_fly
	{      
		int is_active;
	};

	struct cmd_duel_request
	{       
		int target;
	};

	struct cmd_duel_reply
	{
		int who;
		int param; // 0 同意  非0，拒绝的原因
	};

	struct cmd_bind_player_request
	{
		int target;
	};

	struct cmd_bind_player_invite
	{
		int target;
	};

	struct cmd_bind_player_request_reply
	{
		int who;
		int param;  //	0 同意
	};

	struct cmd_bind_player_invite_reply
	{
		int who;
		int param;  //	0 同意
	};

	struct cmd_get_other_equip_detail 
	{
		int target;
	};

	struct cmd_summon_pet
	{
		size_t pet_index;	//	宠物栏内的索引
	};

	struct cmd_banish_pet
	{
		size_t pet_index;
	};

	/*	pet_cmd的分类：
		pet_cmd = 1 ，攻击target所代表的目标，要求target有效
						参数为char: pvp mask 攻击保护状态
		pet_cmd = 2 ，修改宠物的跟随策略 target无效 
					  参数为int 0 代表跟随玩家（默认值）
								1 代表原地停留
					  这个命令发出时，会试图中止宠物当前的动作，执行新的跟随策略
		pet_cmd = 3 ， 修改宠物的遇敌逻辑 target 无效
					   参数为int 0 代表 防御型，当受到攻击时会进行反击， 主人受到攻击时也会进行反击（目前未实现）
								 1 代表 主动型，会自动攻击视野范围内的敌对目标
								 2 代表 被动型，除非玩家发出指令，否则不会做出应对
		pet_cmd = 4,  要求宠物使用技能  target代表使用技能的目标
						参数为 int，表示要求宠物使用的技能号 
						参数为 char: pvp mask 攻击保护状态
		pet_cmd = 5, 要求宠物自动使用技能使用技能  target无效
					参数为 int ，表示要求宠物使用的技能号 如果该技能为0，则不自动使用						
	*/
	struct cmd_pet_ctrl
	{
		int target;		//	操作的目标，如果不需要目标，指令可填0
		int pet_cmd;	//	对宠物的控制指令
	//	char buf[];		//	宠物控制指令的参数
	};

	struct cmd_debug_deliver_cmd
	{
		unsigned short type;
	//	char buf[];		//	command buffer
	};

	struct mall_shopping
	{
		unsigned int count;

		struct goods
		{
			int goods_id;
			int goods_index;
			int goods_pos;
		} list[1];
	};

	struct use_item_with_arg      //带参数使用物品
	{
        unsigned char where;  //物品在哪里
        unsigned char count;  //使用几个
        unsigned short index; //物品的位置
        int  item_id;         //物品的类型
        // char arg[1];           //物品的自定义参数区域
	};

	struct cmd_elf_add_attribute
	{   
		short str; //属性增加值，>= 0
		short agi;
		short vit;
		short eng;
	};  

	struct cmd_elf_add_genius
	{   
		short genius[5];  //天赋增加值, >=0
	};

	struct cmd_elf_player_insert_exp
	{   
		unsigned int exp;    //玩家准备向小精灵注入的经验值，use_sp为1则为元神数,限定为8.4e
		char use_sp;		//为1则使用元神，为0使用经验
	};

	struct cmd_elf_equip_item
	{
		unsigned char index_inv;		//小精灵的装备在包裹中的索引
	};  

	struct cmd_elf_change_secure_status
	{
		unsigned char status;   //0-将小精灵设置为安全状态 1-设置为转化状态
	};

	struct cmd_cast_elf_skill
	{
		unsigned short skill_id;
//		short skill_level;
		unsigned char force_attack;
		unsigned char target_count;
		int targets[1];
	};

	struct recharge_equipped_elf    //实现与飞剑完全相同，仅命令号不同
	{
		unsigned char element_index;
		int count;
	};

	//用于player获取商城更新数据,在player每次打开商城时使用
	//由于该协议会让gs扫描mall所有商品，所以客户端需控制协议发送速度
	struct cmd_get_mall_item_price
	{
		short start_index;	// 若两参数均为0 则扫描整个表
		short end_index;	// 否则[start,end)将被扫描		
	};

	//  更换时装仓库里的时装（自动换装功能）
	struct cmd_equip_fashion_item
	{
		unsigned char idx_body;		// 衣服
		unsigned char idx_leg;		// 裤子
		unsigned char idx_foot;		// 鞋
		unsigned char idx_wrist;	// 手套	
		unsigned char idx_head;		// 头饰
		unsigned char idx_weapon;	// 武器
	};

	//  第一次打开时装包裹时发送密码
	struct cmd_check_security_passwd
	{
		size_t passwd_size;
	};

	//  发送PK设置给服务器
	struct cmd_notify_force_attack
	{
		char force_attack;
		char refuse_bless;
	};

	struct cmd_dividend_mall_shopping
	{
		unsigned int count;
		struct goods
		{
			int goods_id;
			int goods_index;
			int goods_slot;
		}list[1];

	};

	struct cmd_get_dividend_mall_item_price
	{
		short start_index;
		short end_index;
	};

	struct cmd_choose_multi_exp
	{
		int index;
	};

	struct cmd_toggle_muti_exp
	{
		char is_activate;
	};

	struct cmd_multi_exchange_item
	{
		unsigned char location;
		unsigned char count;
		struct operation
		{
			unsigned char index1;
			unsigned char index2;
		}operations[1];
	};

	struct cmd_network_delay 
	{
		int timestamp;
	};

	struct cmd_congregate_reply
	{
        char type;		// congregate
        char agree;		// bool type
        int sponsor;	// sender role id
	};

	struct cmd_preview_choose
	{
		char remain;	// 0代表选择新装备，1代表选择旧装备
	};

	
	struct cmd_recharge_online_award
	{
		int type;   //奖励类型
		int count;
		struct entry
		{
			int type;   //物品id
			int index; //物品位置
			int count;  //物品数量
		}entry_list[1];
    };

	struct cmd_toggle_online_award
	{
		int type; //奖励类型
		char activate;
	};

	struct cmd_exchange_wanmei_yinpiao
	{
		char is_sell;
		size_t count;
	};

	struct cmd_query_title
	{
		int roleid;
	};
	struct cmd_change_curr_title
	{
		unsigned short id;
	};
	struct cmd_late_signin 
	{ 
		char type;    // 0 天补签1月补签2年补签
		int itempos;   // 消耗物品位置
		int desttime;   // 补签时间点 当月第一未签订日|月第一天|年第一天
	};
	struct cmd_apply_signinaward
	{
		char type;  // 0x1月度0x2年度0x4全勤(可同时获取，获取前需判断背包)
		int mon;    // 月份 填0x1时有效
	};
	struct cmd_rewrite_reincarnation_tome
	{
		unsigned int record_index;
		int record_level;
	};
	struct cmd_activate_reincarnation_tome
	{
		char active;
	};

	struct cmd_auto_team_set_goal
	{
		char goal_type;		// 0 unknown, 1 task, 2 Activity
		char op;
		int goal_id;
	};

	struct cmd_auto_team_jump_to_goal
	{
		int goal_id;
	};

	///////////////////////////////////////////////////////////////////
	//
	//	GM commands
	//
	///////////////////////////////////////////////////////////////////

	struct gm_cmd_moveto_player
	{
		int pid;
	};

	struct gm_cmd_callin_player
	{
		int pid;
	};

	struct gm_cmd_generate
	{
		int tid;
	};

	struct gm_cmd_active_spawner
	{
		unsigned char is_active;
		int sp_id;		//	生成器ID值
	};

	struct gm_cmd_generate_mob
	{
		int mob_id;			//	生成那种怪物
		int vis_id;			//	期望这种怪物看起来什么样，0则无效
		short count;		//	生成的数量
		short life;			//	生成怪物的生存期，0为无限
		size_t name_len;
	//	char name[];
	};

	struct gmcmd_query_spec_item
	{
		int roleid;			// role id
		int type;			// item id
	};

	struct gm_cmd_remove_spec_item
	{
		int roleid;				// role id
		int type;				// item id
		unsigned char where;
		unsigned char index;
		size_t count;
	};
	struct gmcmd_open_activity
	{
		int activity_id;
		char is_open;
	};	
	struct gmcmd_change_ds
	{
		int flag;
	};
	struct cmd_evolution_pet
	{
		size_t pet_index;
		size_t formula_index;
	};
	struct cmd_add_pet_exp
	{
		size_t pet_index;
		size_t item_num;
	};
	struct cmd_rebuild_pet_nature
	{
		size_t pet_index;
		size_t formula_index;
	};
	struct cmd_rebuild_pet_inherit_ratio
	{
		size_t pet_index;
		size_t formula_index;
	};
	struct cmd_rebuild_inherit_choose
	{
		char isaccept;
	};
	struct cmd_rebuild_nature_choose
	{
		char isaccept;
	};
	struct cmd_try_refine_meridian
	{
		int index; //门的序号
    };
	struct cmd_player_give_present
	{
		int roleid;                 //赠予对象的roleid
		int mail_id;                //索取物品邮件的索引，没有的话为-1
		int goods_id;               //赠予物品的id
		int goods_index;            //赠予物品在商城中的索引
		int goods_slot;             //赠予物品的销售信息索引
	};
	struct cmd_player_ask_for_present
	{       
		int roleid;                 //索取对象的roleid
		int goods_id;               //索取物品的id
		int goods_index;            //索取物品在商城中的索引
		int goods_slot;             //索取物品的销售信息索引
	};
	struct cmd_spend_touch_point
	{
		unsigned int index;
		unsigned int lots;
		int id;
		unsigned int num;
		unsigned int price;
		int expire_time;
	};
	struct cmd_query_unique_data 
	{ 
		int count;
		int key[1];
	};

	struct cmd_revive
    {    
		int param;		//增加参数  对于战车战场来说是chariotid，其他场景无意义
	};

	struct cmd_trickbattle_upgrade_chariot
	{
		int chariot;
	};
	struct cmd_swallow_generalcard
	{
		unsigned char equip_index;				// 装备上卡牌的索引
		unsigned char is_inv;					// 1-包裹, 0-卡牌仓库
		unsigned char swallowed_equip_index;	// 被吞噬的包裹栏上卡牌/随机发生器的索引
		unsigned int count;						// 被吞噬的包裹栏上卡牌/随机发生器的数量
	};
	struct cmd_equip_trashbox_item
	{
		unsigned char trash_box_index;			// // 随身仓库共两个，时装和卡牌。目前服务器只支持卡牌仓库与装备栏之间的物品移动
		unsigned char item_index_in_box;
		unsigned char equip_index;
	};
	struct cmd_send_mass_mail
	{
		int	service_id;
		char data[1];
	};
	struct cmd_rand_mall_shopping 
	{
		int config_id;	//	随机配置表ID
		int	op;			//	对此随机配置表的操作
	};
	struct cmd_query_faction_pvp_info
	{
		int faction_id;
	};
	struct cmd_query_can_inherit_addons
	{
		int equip_id;
		unsigned char inv_idx;  
    };
	struct cmd_activate_region_waypoints
	{
		unsigned char num;
		int waypoints[1];
    };
	struct cmd_instance_reenter_request{
		bool agree;
	};
}

#pragma pack()

