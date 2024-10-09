/*
 * FILE: EC_IvtrTypes.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/11/19
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#pragma once

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

#ifndef NUM_MAGICCLASS
#define NUM_MAGICCLASS		5
#endif	//	NUM_MAGICCLASS

//	Equipment endurance scale
#define ENDURANCE_SCALE		100
//	Player item price scale
#define PLAYER_PRICE_SCALE	1.0f
//	Weapon endurance reduce speed
#define WEAPON_RUIN_SPEED	-2
//	Armor endurance reduce speed
#define ARMOR_RUIN_SPEED	-25

//	Inventory type
enum
{
	IVTRTYPE_PACK = 0,		//	Normal pack
	IVTRTYPE_EQUIPPACK,		//	Equipment
	IVTRTYPE_TASKPACK,		//	Task pack
	IVTRTYPE_TRASHBOX,		//	Trash box
	IVTRTYPE_TRASHBOX2,		//	Trash box - material box
	IVTRTYPE_TRASHBOX3,		//	Trash box - fashion box
	IVTRTYPE_ACCOUNT_BOX,	//  User account box
	IVTRTYPE_GENERALCARD_BOX,	// ���ư���
};

// ע�� IVTRTYPE_CLIENT_GENERALCARD_PACK ö��ֵ���ܺ������ Inventory type ֵ�ظ�����
enum
{
	IVTRTYPE_CLIENT_GENERALCARD_PACK = 1024, // �ͻ��˱��ذ����� ���ڿ���ͼ��Ҫ����ѻ�ÿ���ͨ�����췢�͡�Ϊ��ͳһ�������촰�ڵ���Ʒ����ӱ��ذ�����
};

//	Index of item in equipment inventory
enum
{
	EQUIPIVTR_WEAPON = 0,
	EQUIPIVTR_HEAD,
	EQUIPIVTR_NECK,
	EQUIPIVTR_SHOULDER,
	EQUIPIVTR_BODY,
	EQUIPIVTR_WAIST,
	EQUIPIVTR_LEG,
	EQUIPIVTR_FOOT,
	EQUIPIVTR_WRIST,
	EQUIPIVTR_FINGER1,
	EQUIPIVTR_FINGER2,
	EQUIPIVTR_PROJECTILE,
	EQUIPIVTR_FLYSWORD,
	EQUIPIVTR_FASHION_BODY,
	EQUIPIVTR_FASHION_LEG,
	EQUIPIVTR_FASHION_FOOT,
	EQUIPIVTR_FASHION_WRIST,
	EQUIPIVTR_RUNE,
	EQUIPIVTR_BIBLE,
	EQUIPIVTR_SPEAKER,
	EQUIPIVTR_AUTOHP,
	EQUIPIVTR_AUTOMP,
	EQUIPIVTR_POCKET,
	EQUIPIVTR_GOBLIN,
	EQUIPIVTR_CERTIFICATE,
	EQUIPIVTR_FASHION_HEAD,
	EQUIPIVTR_FORCE_TOKEN,
	EQUIPIVTR_DYNSKILLEQUIP1,
	EQUIPIVTR_DYNSKILLEQUIP2,
	EQUIPIVTR_FASHION_WEAPON,
	SIZE_EQUIPIVTR,
	EQUIPIVTR_UNUSED1 = SIZE_EQUIPIVTR,
	EQUIPIVTR_UNUSED2,
	EQUIPIVTR_GENERALCARD1,
	EQUIPIVTR_GENERALCARD2,
	EQUIPIVTR_GENERALCARD3,
	EQUIPIVTR_GENERALCARD4,
	EQUIPIVTR_GENERALCARD5,
	EQUIPIVTR_GENERALCARD6,
	SIZE_ALL_EQUIPIVTR,
	SIZE_GENERALCARD_EQUIPIVTR = SIZE_ALL_EQUIPIVTR - EQUIPIVTR_GENERALCARD1,
};

//	Inventory size
enum
{
	IVTRSIZE_EQUIPPACK	= SIZE_ALL_EQUIPIVTR,	//	Equipment
	IVTRSIZE_TASKPACK	= 32,	//	Task pack
	IVTRSIZE_DEALPACK	= 24,	//	Deal pack
	IVTRSIZE_NPCPACK	= 32,	//	NPC pack
	IVTRSIZE_TRASHBOX	= 16,	//	Trash box
	IVTRSIZE_BUYPACK	= 12,	//	Buy pack
	IVTRSIZE_SELLPACK	= 12,	//	Sell pack
	IVTRSIZE_BOOTHSPACK	= 12,	//	Default booth pack for selling
	IVTRSIZE_BOOTHBPACK = 12,	//	Default booth pack for buying
	IVTRSIZE_BOOTHSPACK_MAX	= 20,	//	Max booth pack for selling (player may use certificate...)
	IVTRSIZE_BOOTHBPACK_MAX = 20,	//	Max booth pack for buying
	
	IVTRSIZE_CLIENTCARDPACK	= 32,	// client pack for general card collection 

	NUM_NPCIVTR			= 8,	//	NPC inventory number
};

//	Equip mask

static const __int64	EQUIP_MASK64_WEAPON			  	= 0x0000000000000001;
static const __int64	EQUIP_MASK64_HEAD			  	= 0x0000000000000002;
static const __int64	EQUIP_MASK64_NECK			  	= 0x0000000000000004;
static const __int64	EQUIP_MASK64_SHOULDER		  	= 0x0000000000000008;
static const __int64	EQUIP_MASK64_BODY			  	= 0x0000000000000010;
static const __int64	EQUIP_MASK64_WAIST			   	= 0x0000000000000020;
static const __int64	EQUIP_MASK64_LEG			  	= 0x0000000000000040;
static const __int64	EQUIP_MASK64_FOOT		      	= 0x0000000000000080;
static const __int64	EQUIP_MASK64_WRIST		      	= 0x0000000000000100;
static const __int64	EQUIP_MASK64_FINGER1		  	= 0x0000000000000200;
static const __int64	EQUIP_MASK64_FINGER2			= 0x0000000000000400;
static const __int64	EQUIP_MASK64_PROJECTILE		   	= 0x0000000000000800;
static const __int64	EQUIP_MASK64_FLYSWORD			= 0x0000000000001000;
static const __int64	EQUIP_MASK64_FASHION_BODY		= 0x0000000000002000;
static const __int64	EQUIP_MASK64_FASHION_LEG		= 0x0000000000004000;
static const __int64	EQUIP_MASK64_FASHION_FOOT		= 0x0000000000008000;
static const __int64	EQUIP_MASK64_FASHION_WRIST		= 0x0000000000010000;
static const __int64	EQUIP_MASK64_RUNE				= 0x0000000000020000;
static const __int64	EQUIP_MASK64_BIBLE				= 0x0000000000040000;
static const __int64	EQUIP_MASK64_SPEAKER			= 0x0000000000080000;
static const __int64	EQUIP_MASK64_AUTOHP				= 0x0000000000100000;
static const __int64	EQUIP_MASK64_AUTOMP				= 0x0000000000200000;
static const __int64	EQUIP_MASK64_POCKET				= 0x0000000000400000;
static const __int64	EQUIP_MASK64_GOBLIN				= 0x0000000000800000;
static const __int64	EQUIP_MASK64_CERTIFICATE		= 0x0000000001000000;
static const __int64	EQUIP_MASK64_FASHION_HEAD		= 0x0000000002000000;
static const __int64	EQUIP_MASK64_FORCE_TICKET		= 0x0000000004000000;
static const __int64	EQUIP_MASK64_DYNSKILLEQUIP1		= 0x0000000008000000;
static const __int64	EQUIP_MASK64_DYNSKILLEQUIP2		= 0x0000000010000000;
static const __int64	EQUIP_MASK64_FASHION_WEAPON		= 0x0000000020000000;
static const __int64	EQUIP_MASK64_USED1				= 0x0000000040000000;
static const __int64	EQUIP_MASK64_USED2				= 0x0000000080000000;
static const __int64	EQUIP_MASK64_GENERALCARD1		= 0x0000000100000000;
static const __int64	EQUIP_MASK64_GENERALCARD2		= 0x0000000200000000;
static const __int64	EQUIP_MASK64_GENERALCARD3		= 0x0000000400000000;
static const __int64	EQUIP_MASK64_GENERALCARD4		= 0x0000000800000000;
static const __int64	EQUIP_MASK64_GENERALCARD5		= 0x0000001000000000;
static const __int64	EQUIP_MASK64_GENERALCARD6		= 0x0000002000000000;
static const __int64	EQUIP_MASK64_ALL				= 0x0000003f3fffffff;


//	Weapon type
enum
{
    WEAPONTYPE_MELEE = 0,
    WEAPONTYPE_RANGE = 1,
};

//	Shortcut
enum
{
	NUM_HOSTSCSETS1 = 5,	// expanded from 3 to 5 (2009.05.27)
	NUM_HOSTSCSETS2 = 3,
	SIZE_HOSTSCSET1 = 9,	// expanded from 6 to 9 (2009.05.27)
	SIZE_HOSTSCSET2 = 8,	

	SIZE_FASHIONSCSET = 240,
};

enum
{
	NUM_SYSMODSETS = 4,// ϵͳ���ܿ����
	SIZE_SYSMODSCSET = 4,
};

//	Pet food type
enum
{
	PET_FOOD_GRASS = 0,
	PET_FOOD_MEAT,
	PET_FOOD_VEGETABLE,
	PET_FOOD_FRUIT,
	PET_FOOD_WATER,
	MAX_PET_FOOD,
};

///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////

#pragma pack(1)

struct IVTR_ESSENCE_WEAPON
{
    short	weapon_type;
	short	weapon_dealy;
	int		weapon_class;
	int		weapon_level;
    int		require_projectile; 	//	��Ҫ��ҩ������
    int		damage_low;         	//	��������С��ֵ
    int 	damage_high;        	//	����������ֵ
    int 	magic_damage_low;		//	ħ������
    int 	magic_damage_high;		//	ħ������
//  int 	attack;					//	������
    int 	attack_speed;
    float	attack_range;
    float	attack_short_range;
};

struct IVTR_ESSENCE_ARROW
{
	DWORD	dwBowMask;
	int		iDamage;
	int		iDamageScale;			//	Percent
	int		iWeaponReqLow;
	int		iWeaponReqHigh;
};

struct IVTR_ESSENCE_DECORATION
{
	int		damage;
	int		magic_damage;
	int		defense;
	int		armor;
	int		resistance[NUM_MAGICCLASS];
};

struct IVTR_ESSENCE_ARMOR
{
	int		defense;
	int		armor;
	int		mp_enhance;
	int		hp_enhance;
	int		resistance[NUM_MAGICCLASS];
};

struct IVTR_ESSENCE_FASHION
{
	int require_level;
	unsigned short color;
	unsigned short gender;
};

struct IVTR_ESSENCE_FLYSWORD
{
	int		cur_time;
	int		max_time;
	short	require_level;
    char	level;
	char	improve_level;
	int		profession;
	size_t	time_per_element;
	float	speed_increase;
	float	speed_increase2;		//	���ٷ����ٶ�
};

struct IVTR_ESSENCE_WING
{
	int		require_level;
	size_t	mp_launch;
	size_t	mp_per_second;
	float	speed_increase;
};

struct IVTR_ESSENCE_AUTOHP
{
	int		hp_left;
	float	trigger;
};

struct IVTR_ESSENCE_AUTOMP
{
	int		mp_left;
	float	trigger;
};

struct IVTR_ESSENCE_PETEGG
{
	int		req_level;          //	������Ҽ���
	int		req_class;          //	�������ְҵ
	int		honor_point;        //	�øж�
	int		pet_tid;            //	�����ģ��ID
	int		pet_vis_tid;        //	����Ŀɼ�ID�����Ϊ0�����ʾ������ɼ�ID��
	int		pet_egg_tid;        //	���ﵰ��ID
	int		pet_class;          //	�������� ս�裬��裬���ͳ�
	short	level;				//	���Ｖ��
	unsigned short	color;			//	������ɫ�����λΪ1��ʾ��Ч��Ŀǰ���������Ч
	int		exp;                //	���ﵱǰ����
	int		skill_point;        //	ʣ�༼�ܵ�
	unsigned short name_len;	//	���ֳ��� 
	unsigned short skill_count;	//	��������		
	wchar_t	name[8];			//	��������
	//	������漼�ܱ�
	/*
	struct
	{	
		int skill
		int level;

	} skills[]
	 */
};

struct IVTR_ESSENCE_DESTROYING
{
	int tid;
};

struct IVTR_ESSENCE_GOBLIN
{
	struct _GOBLIN_DATA
	{
		unsigned int exp;			//	��ǰ����ֵ
		short	level;				//	��ǰ�ȼ�
		
		short	total_attribute;	//	�������������Ե�������������װ�����ӵļ������Գ�ʼֵ
		short	strength;			//	����, �ɼ����Ե������������ֵ��������װ�����ӵļ������Գ�ʼֵ
		short	agility;			//	����
		short	vitality;			//	����
		short	energy;				//	����

		short	total_genius;		//	�����������츳��������������װ�����ӵ�
		short	genius[5];			//	��ľˮ����0-4 //�ɼ��츳����������츳ֵ��������װ�����ӵ�
		
		short	refine_level;		//	�����ȼ�
		int		stamina; 			//	����
		int		status_value;		//	0: ��ȫ״̬��>0��ת��״̬��-1:�ɽ���״̬ 
	};

	_GOBLIN_DATA	data;

	int equip_cnt;					//	װ������
//	unsigned int equipid[];			//	װ��id
	int skill_cnt;					//	��ѧ��ļ����� 
/*	struct
	{	
		unsigned short skill
		short level;

	} skills[]; */
};

struct IVTR_ESSENCE_WEDDING_BOOKCARD 
{
	int	year;
	int	month;
	int	day;
};

struct IVTR_ESSENCE_WEDDING_INVITECARD 
{
	int	start_time;	//	����ʼʱ��
	int	end_time;	//	�������ʱ��
	int	groom;		//	���ɽ�ɫ ID
	int	bride;		//	�����ɫ ID
	int	scene;		//	���������������������ƺ;���λ��
	int	invitee;	//	�������� ID
};

struct IVTR_ESSENCE_FORCE_TOKEN
{
	int require_force;	// ��Ʒʹ��������������
	int repu_total;		// �洢�Ķ������������ܶ�
	int repu_inc_ratio; // ÿ�λ������ʱ�������ӵ�����ϵ����1~100��Ӧ0%~100%
};

struct IVTR_ESSENCE_MONSTERSPIRIT
{
	int level;  //Ԫ��ĵȼ�
    int type;   //Ԫ�������
    int power;  //Ԫ���̺�������
};
struct IVTR_ESSENCE_GENERALCARD
{
	int type;					// ����, ��װ��λ�����Ӧ, �ƾ�, ����, ����, ���, ����
	int rank;					// Ʒ��, C, B, A, S, S+
	int require_level;			// ����ȼ�
	int require_leadership;		// ����ͳ����
	int max_level;				// ���ɳ��ȼ�
	int level;					// �ɳ��ȼ�
	int exp;					// �ɳ�����
	int rebirth_times;			// ת������
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



