/*
 * FILE: EC_RoleTypes.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/11/16
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include <ABaseDef.h>

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

#define EC_MAXNOPKLEVEL		0	//	The maximum no PK level

#define EC_TABSEL_DIST		60.0f	//	Distance of TAB selection

//	Magic class
enum
{
	MAGICCLASS_GOLD	= 0,
	MAGICCLASS_WOOD,
	MAGICCLASS_WATER,
	MAGICCLASS_FIRE,
	MAGICCLASS_EARTH,
};

#ifndef NUM_MAGICCLASS
#define NUM_MAGICCLASS		5
#endif	//	NUM_MAGICCLASS

//	Extend properties class
enum
{
	EXTPROPIDX_BASE = 0,
	EXTPROPIDX_MOVE,
	EXTPROPIDX_ATTACK,
	EXTPROPIDX_DEF,
};

//	Profession
enum
{
	PROF_WARRIOR = 0,	//	0:武侠
	PROF_MAGE,			//	1:法师
	PROF_MONK,			//	2:巫师
	PROF_HAG,			//	3:妖精
	PROF_ORC,			//	4:妖兽
	PROF_GHOST,			//	5:刺客
	PROF_ARCHOR,		//	6:羽芒
	PROF_ANGEL,			//	7:羽灵
	PROF_JIANLING,		//	8:剑灵
	PROF_MEILING,		//	9:魅灵
	PROF_YEYING,		//	10:夜影
	PROF_YUEXIAN,		//	11:月仙
	NUM_PROFESSION,
};

//	Gender
enum
{
	GENDER_MALE = 0,
	GENDER_FEMALE,
	NUM_GENDER,
};

//	Race
enum
{
	RACE_HUMAN = 0,		//	0:人类
	RACE_ORC,			//	1:妖族
	RACE_ELF,			//	2:羽人
	RACE_GHOST,			//	3:汐族
	RACE_LING,			//	4:灵族
	RACE_OBORO,			//	5:胧族
	NUM_RACE,
};

//	Player name color
enum
{
	NAMECOL_RED0	= 0xffffaaaa,
	NAMECOL_RED1	= 0xffff6e6e,
	NAMECOL_RED2	= 0xffff2828,
	NAMECOL_PINK	= 0xffffb4ff,
	NAMECOL_WHITE	= 0xffffffff,
	NAMECOL_BLUE	= 0xff3f5aff,
	NAMECOL_MAUVE	= 0xff96c8ff,
	NAMECOL_BC_RED	= NAMECOL_RED2,		//	Red camp in battle
	NAMECOL_BC_BLUE	= NAMECOL_MAUVE,	//	Blue camp in battle
	NAMECOL_ALLIANCE = 0xff3997CE,
	NAMECOL_HOSTILE	=	NAMECOL_RED2,
	NAMECOL_FACTION_PVP = 0xffdaa520,
};

//	Extend states
enum
{
	EXTST_NONE		= 0,
	NUM_EXTSTATE	= 32,
	NUM_ESBYTE		= (NUM_EXTSTATE+31) >> 5,
};

//	Player expressions
enum
{
	ROLEEXP_WAVE = 0,		// 招手
	ROLEEXP_NOD,			// 点头
	ROLEEXP_SHAKEHEAD,		// 摇头
	ROLEEXP_SHRUG,			// 耸肩膀
	ROLEEXP_LAUGH,			// 大笑

	ROLEEXP_ANGRY,			// 生气
	ROLEEXP_STUN,			// 晕倒
	ROLEEXP_DEPRESSED,		// 沮丧
	ROLEEXP_KISSHAND,		// 飞吻
	ROLEEXP_SHY,			// 害羞

	ROLEEXP_SALUTE,			// 抱拳
	ROLEEXP_SITDOWN,		// 坐下
	ROLEEXP_ASSAULT,		// 冲锋
	ROLEEXP_THINK,			// 思考
	ROLEEXP_DEFIANCE,		// 挑衅

	ROLEEXP_VICTORY,		// 胜利
	ROLEEXP_GAPE,			// 伸懒腰
	ROLEEXP_FIGHT,	
	ROLEEXP_ATTACK1,
	ROLEEXP_ATTACK2,

	ROLEEXP_ATTACK3,
	ROLEEXP_ATTACK4,
	ROLEEXP_DEFENCE,
	ROLEEXP_FALL,	
	ROLEEXP_FALLONGROUND,

	ROLEEXP_LOOKAROUND,
	ROLEEXP_DANCE,
	ROLEEXP_FASHIONWEAPON,
	
	NUM_ROLEEXP,

	ROLEEXP_KISS,			// 亲吻
	ROLEEXP_TWO_KISS,		// 双人亲吻
	ROLEEXP_FIREWORK,		// 放烟花
};

///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////

#pragma pack(1)

//	Role (Player and NPC) base properties
struct ROLEBASICPROP
{
	int 	iLevel;			//	Level
	int		iLevel2;		//	2th. level
	int		iCurHP;			//	Current HP
	int		iCurMP;			//	Current MP
	int		iExp;			//	Experience
	int		iSP;			//	Skill point
	int		iStatusPt;		//	Status point
	int		iCurAP;			//	Current AP
	int		iAtkDegree;		//	Attack degree
	int		iDefDegree;		//	Defence degree
	int		iCritRate;		//  Critical rate
	int		iCritDamageBonus;// Critical damage bonus
	int     iInvisibleDegree;// Invisible degree
	int     iAntiInvisibleDegree; // Anti-invisible degree
	int		iPenetration;		//	穿透力
	int		iResilience;			//	抵抗力
	int		iVigour;		 // 气魄
};

//	Role (Player and NPC) extended properties, base part
struct ROLEEXTPROP_BASE
{
	/* 基础属性 */
	int		vitality;       //	命
	int		energy;         //	神
	int		strength;       //	力
	int 	agility;        //	敏
	int 	max_hp;         //	最大hp
	int 	max_mp;         //	最大mp
	int 	hp_gen;         //	hp恢复速度
	int 	mp_gen;         //	mp恢复速度 
};

//	Role (Player and NPC) extended properties, move part
struct ROLEEXTPROP_MOVE
{
	/* 运动速度*/
	float	walk_speed;     //	行走速度 单位  m/s
	float	run_speed;      //	奔跑速度 单位  m/s
	float	swim_speed;     //	游泳速度 单位  m/s
	float	flight_speed;   //	飞行速度 单位  m/s
};

//	Role (Player and NPC) extended properties, attack part
struct ROLEEXTPROP_ATK
{
	/* 攻击属性*/
	int 	attack;         //	攻击率 attack rate
	int 	damage_low;     //	最低damage
	int 	damage_high;    //	最大物理damage
	int		attack_speed;   //	攻击时间间隔 以tick为单位
	float	attack_range;   //	攻击范围

	struct	//	物理攻击附加魔法伤害
	{
		int damage_low;
		int damage_high;

	} addon_damage[NUM_MAGICCLASS];
	
	int		damage_magic_low;   //	魔法最低攻击力
	int		damage_magic_high;  //	魔法最高攻击力
};

//	Role (Player and NPC) extended properties, defense part
struct ROLEEXTPROP_DEF
{
	/* 防御属性 */
	int		resistance[NUM_MAGICCLASS];		//	魔法抗性
	int		defense;		//	防御力
	int		armor;			//	闪躲率（装甲等级）
};

//	Role (Player and NPC) extended properties
struct ROLEEXTPROP
{
	ROLEEXTPROP_BASE	bs;
	ROLEEXTPROP_MOVE	mv;
	ROLEEXTPROP_ATK		ak;
	ROLEEXTPROP_DEF		df;
	
	int		max_ap;
};

#pragma pack()

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

//	Set extend state
inline void glb_SetExtState(DWORD* p, int n)
{
	p[n >> 5] |= (1 << (n & 31));
}

//	Remove extend state
inline void glb_RemExtState(DWORD* p, int n)
{
	p[n >> 5] &= ~(1 << (n & 31));
}

//	Get extend state
inline bool glb_GetExtState(const DWORD* p, int n)
{
	return (p[n >> 5] & (1 << (n & 31))) ? true : false;
}


