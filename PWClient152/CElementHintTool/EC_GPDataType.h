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
	GP_PET_CLASS_MOUNT = 0, //	���
	GP_PET_CLASS_COMBAT,    //	ս������
	GP_PET_CLASS_FOLLOW,    //	�������
	GP_PET_CLASS_SUMMON,	//	�ٻ�����
	GP_PET_CLASS_PLANT,		//  ֲ�ֻ��Ϊ�˺ͷ�������ֵ����һ�£��ͻ���û���õ�
	GP_PET_CLASS_EVOLUTION, //	������
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
		int honor_point;        	//	�øж�
		int hunger;					//	������
		int feed_time;				//	�ϴ�ι�������ڵ�ʱ��
		int pet_tid;            	//	�����ģ��ID
		int pet_vis_tid;        	//	����Ŀɼ�ID�����Ϊ0�����ʾ������ɼ�ID��
		int pet_egg_tid;        	//	���ﵰ��ID
		int pet_class;          	//	�������� ս�裬��裬���ͳ�
		float hp_factor;        	//	Ѫ��������������ջ�ʱʹ�ã� 0��Ϊ����
		short level;            	//	���Ｖ��
		unsigned short color;	//	������ɫ�����λΪ1��ʾ��Ч��Ŀǰ���������Ч
		int exp;                	//	���ﵱǰ����
		int skill_point;        	//	ʣ�༼�ܵ�
		char is_bind;				//	�Ƿ����˺�һ��������һ��Mask��0x01 ���˺�һ��0x02 Ѱ�����ɽ���
		char unused;			//   Ŀǰ������Ч
		unsigned short name_len;	//	���ֳ��� Ŀǰ������Ч����Ϊ�߻�������������
		char name[16];          	//	��������

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

		int reserved[10]; // δ��
	};

	struct cmd_pet_property
	{
		int pet_index;
		ROLEEXTPROP prop;
	};
}

#pragma pack()

