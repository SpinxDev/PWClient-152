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

#include "../CElementClient/EC_RoleTypes.h"

enum
{
	GP_PET_SKILL_NUM	= 8
};

enum
{
	GP_PET_NATURE_SKILL_NUM = 2
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

	struct cmd_pet_property
	{
		int pet_index;
		ROLEEXTPROP prop;
	};
}

#pragma pack()

