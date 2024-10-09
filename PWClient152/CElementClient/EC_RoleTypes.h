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
	PROF_WARRIOR = 0,	//	0:����
	PROF_MAGE,			//	1:��ʦ
	PROF_MONK,			//	2:��ʦ
	PROF_HAG,			//	3:����
	PROF_ORC,			//	4:����
	PROF_GHOST,			//	5:�̿�
	PROF_ARCHOR,		//	6:��â
	PROF_ANGEL,			//	7:����
	PROF_JIANLING,		//	8:����
	PROF_MEILING,		//	9:����
	PROF_YEYING,		//	10:ҹӰ
	PROF_YUEXIAN,		//	11:����
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
	RACE_HUMAN = 0,		//	0:����
	RACE_ORC,			//	1:����
	RACE_ELF,			//	2:����
	RACE_GHOST,			//	3:ϫ��
	RACE_LING,			//	4:����
	RACE_OBORO,			//	5:����
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
	ROLEEXP_WAVE = 0,		// ����
	ROLEEXP_NOD,			// ��ͷ
	ROLEEXP_SHAKEHEAD,		// ҡͷ
	ROLEEXP_SHRUG,			// �ʼ��
	ROLEEXP_LAUGH,			// ��Ц

	ROLEEXP_ANGRY,			// ����
	ROLEEXP_STUN,			// �ε�
	ROLEEXP_DEPRESSED,		// ��ɥ
	ROLEEXP_KISSHAND,		// ����
	ROLEEXP_SHY,			// ����

	ROLEEXP_SALUTE,			// ��ȭ
	ROLEEXP_SITDOWN,		// ����
	ROLEEXP_ASSAULT,		// ���
	ROLEEXP_THINK,			// ˼��
	ROLEEXP_DEFIANCE,		// ����

	ROLEEXP_VICTORY,		// ʤ��
	ROLEEXP_GAPE,			// ������
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

	ROLEEXP_KISS,			// ����
	ROLEEXP_TWO_KISS,		// ˫������
	ROLEEXP_FIREWORK,		// ���̻�
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
	int		iPenetration;		//	��͸��
	int		iResilience;			//	�ֿ���
	int		iVigour;		 // ����
};

//	Role (Player and NPC) extended properties, base part
struct ROLEEXTPROP_BASE
{
	/* �������� */
	int		vitality;       //	��
	int		energy;         //	��
	int		strength;       //	��
	int 	agility;        //	��
	int 	max_hp;         //	���hp
	int 	max_mp;         //	���mp
	int 	hp_gen;         //	hp�ָ��ٶ�
	int 	mp_gen;         //	mp�ָ��ٶ� 
};

//	Role (Player and NPC) extended properties, move part
struct ROLEEXTPROP_MOVE
{
	/* �˶��ٶ�*/
	float	walk_speed;     //	�����ٶ� ��λ  m/s
	float	run_speed;      //	�����ٶ� ��λ  m/s
	float	swim_speed;     //	��Ӿ�ٶ� ��λ  m/s
	float	flight_speed;   //	�����ٶ� ��λ  m/s
};

//	Role (Player and NPC) extended properties, attack part
struct ROLEEXTPROP_ATK
{
	/* ��������*/
	int 	attack;         //	������ attack rate
	int 	damage_low;     //	���damage
	int 	damage_high;    //	�������damage
	int		attack_speed;   //	����ʱ���� ��tickΪ��λ
	float	attack_range;   //	������Χ

	struct	//	����������ħ���˺�
	{
		int damage_low;
		int damage_high;

	} addon_damage[NUM_MAGICCLASS];
	
	int		damage_magic_low;   //	ħ����͹�����
	int		damage_magic_high;  //	ħ����߹�����
};

//	Role (Player and NPC) extended properties, defense part
struct ROLEEXTPROP_DEF
{
	/* �������� */
	int		resistance[NUM_MAGICCLASS];		//	ħ������
	int		defense;		//	������
	int		armor;			//	�����ʣ�װ�׵ȼ���
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


