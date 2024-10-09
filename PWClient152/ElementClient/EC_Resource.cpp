/*
 * FILE: EC_Resource.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/12/18
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "EC_Global.h"
#include "EC_Resource.h"

#define new A_DEBUG_NEW

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


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

//	Cursor resource
static const char* l_aCurFiles[NUM_RES_CURSOR] = 
{
	"Cursors\\normal.ani",
	"Cursors\\attack.cur",
	"Cursors\\talk.cur",
	"Cursors\\pick.cur",
	"Cursors\\repair.cur",
	"Cursors\\hand.cur",
	"Cursors\\flag.cur",
	"Cursors\\dig.ani",
	"Cursors\\gourd.cur",
};

//	GFX resource
static const char* l_aGFXFiles[NUM_RES_GFX] = 
{
	"程序联入\\角色升级人类.gfx",	//	RES_GFX_LEVELUP,
	"程序联入\\角色出现人类.gfx",	//	RES_GFX_PLAYERAPPEAR,
	"被击中\\被击中效果.gfx",		//	RES_GFX_HIT,
	"程序联入\\指向标.gfx",			//	RES_GFX_MOVETARGET,
	"程序联入\\金币效果.gfx",		//	RES_GFX_ITEMFLASH,
	"程序联入\\鼠标悬浮.gfx",		//	RES_GFX_CURSORHOVER,
	"程序联入\\目标被选中.gfx",		//	RES_GFX_SELECTED,
	"程序联入\\海底飘尘.gfx",		//	RES_GFX_FLOATING_DUST,
	"程序联入\\人物游动水圈.gfx",	//	RES_GFX_WATER_WAVE_STILL,
	"程序联入\\人物游动水波.gfx",	//	RES_GFX_WATER_WAVE_MOVE,
	"程序联入\\水中呼吸气泡.gfx",	//	RES_GFX_AIR_BUBBLE,
	"程序联入\\水中游动气泡.gfx",	//	RES_GFX_SWIM_BUBBLE,
	"程序联入\\白虎变.gfx",			//	RES_GFX_TRANSFORM_TIGER,
	"程序联入\\火狐变.gfx",			//	RES_GFX_TRANSFORM_FOX,
	"程序联入\\加血.gfx",			//	RES_GFX_RED_MEDICINE,
	"程序联入\\加魔.gfx",			//	RES_GFX_BLUE_MEDICINE,
	"程序联入\\加血加魔.gfx",		//	RES_GFX_PURPLE_MEDICINE,
	"程序联入\\解毒.gfx",			//	RES_GFX_JIEDU_MEDICINE,
	"程序联入\\物防符释放.gfx",		//	RES_GFX_PHYSIC_ARMORRUNE,
	"程序联入\\法防符释放.gfx",		//	RES_GFX_MAGIC_ARMORRUNE,
	"程序联入\\决斗状态.gfx",		//	RES_GFX_INDUEL,
	"程序联入\\控制投石车红.gfx",	//	RES_GFX_TANKLEADER_RED,
	"程序联入\\控制投石车蓝.gfx",	//	RES_GFX_TANKLEADER_BLUE,
	"怪物\\宠物通用\\精灵升级.gfx",//  RES_GFX_GOBLIN_LEVELUP,
	"程序联入\\角色升级人类.gfx",	//	RES_GFX_INVISIBLE_DETECTED
	"策划联入\\状态效果\\黑色连线.gfx",		// RES_GFX_MONSTER_SPIRIT_LINE
	"程序联入\\光球特效_子效果01.gfx", // RES_GFX_MONSTER_SPIRIT_BALL
	"人物\\通用\\其它\\境界提升.gfx",
	"策划联入\\状态效果\\斗气%d级.gfx",
};

//	Shader resource
static const char* l_aShaderFiles[NUM_RES_SHADER] =
{
	"Hair.sdr",		//	0
	"Goatee.sdr",
	"Face.sdr",
	"Eye.sdr",
	"Brow.sdr",

	"Mouth.sdr",	//	5
	"Nose.sdr",		
	"EyeBall.sdr",
	"Moustache.sdr",
	"Body.sdr"
};

//	Icon files
static const char* l_aIconFiles[NUM_RES_ICON] = 
{
	"打坐.dds",
	"走跑转换.dds",
	"普通攻击.dds",
	"寻找目标.dds",
	"协助攻击.dds",

	"邀请加入.dds",
	"脱离队伍.dds",
	"踢出队伍.dds",
	"寻找队伍.dds",
	"交易命令.dds",

	"摆摊卖.dds",
	"摆摊买.dds",
	"邀请加入.dds",
	"飞行.dds",
	"招手.dds",		//	RES_ICON_CMD_EXP_WAVE,

	"点头.dds",		//	RES_ICON_CMD_EXP_NOD,
	"摇头.dds",		//	RES_ICON_CMD_EXP_SHAKEHEAD,
	"耸肩膀.dds",	//	RES_ICON_CMD_EXP_SHRUG,	
	"大笑.dds",		//	RES_ICON_CMD_EXP_LAUGH,	
	"生气.dds",		//	RES_ICON_CMD_EXP_ANGRY,	

	"晕倒.dds",		//	RES_ICON_CMD_EXP_STUN,	
	"沮丧.dds",		//	RES_ICON_CMD_EXP_DEPRESSED,
	"飞吻.dds",		//	RES_ICON_CMD_EXP_KISSHAND,
	"害羞.dds",		//	RES_ICON_CMD_EXP_SHY,	
	"抱拳.dds",		//	RES_ICON_CMD_EXP_SALUTE,	

	"坐下.dds",		//	RES_ICON_CMD_EXP_SITDOWN,
	"冲锋.dds",		//	RES_ICON_CMD_EXP_ASSAULT,
	"思考.dds",		//	RES_ICON_CMD_EXP_THINK,	
	"挑衅.dds",		//	RES_ICON_CMD_EXP_DEFIANCE,
	"胜利.dds",		//	RES_ICON_CMD_EXP_VICTORY,

	"伸懒腰.dds",	//	RES_ICON_CMD_EXP_GAPE
	"亲吻.dds",		//	RES_ICON_CMD_EXP_KISS
	"战斗.dds",		//	RES_ICON_CMD_EXP_FIGHT,	
	"攻击1.dds",	//	RES_ICON_CMD_EXP_ATTACK1,
	"攻击2.dds",	//	RES_ICON_CMD_EXP_ATTACK2,

	"攻击3.dds",	//	RES_ICON_CMD_EXP_ATTACK3,
	"攻击4.dds",	//	RES_ICON_CMD_EXP_ATTACK4,
	"防御.dds",		//	RES_ICON_CMD_EXP_DEFENCE,
	"摔倒.dds",		//	RES_ICON_CMD_EXP_FALL,	
	"倒地.dds",		//	RES_ICON_CMD_EXP_FALLONGROUND,

	"张望.dds",		//	RES_ICON_CMD_EXP_LOOKAROUND,
	"舞蹈1.dds",	//	RES_ICON_CMD_EXP_DANCE,
	"舞蹈2.dds",	//	RES_ICON_CMD_EXP_FASHIONWEAPON
	"拾取.dds",		//	RES_ICON_CMD_PICKUP
	"挖掘.dds",		//	RES_ICON_CMD_GATHER
	"加速飞行.dds",	//	RES_ICON_CMD_RUSHFLY

	"相依相偎的动作.dds",	//	RES_ICON_CMD_BINDBUDDY
	"亲亲密密的动作.dds",	//	RES_ICON_CMD_TWOKISS
	"跳跃的动作1.dds",	//	RES_ICON_CMD_JUMPTRICK,
	"跳跃的动作2.dds",	//	RES_ICON_CMD_RUNTRICK,
};

//	Model files
static const char* l_aModelFiles[NUM_RES_MODEL] =
{
	"Models\\Matters\\物品\\金币\\金币.ecm",
	"Models\\Matters\\物品\\元晶\\元晶.ecm",
	// 人类
	"Models\\Players\\形象\\武侠男\\躯干\\武侠男.ecm",
	"Models\\Players\\形象\\武侠女\\躯干\\武侠女.ecm",
	"Models\\Players\\形象\\法师男\\躯干\\法师男.ecm",
	"Models\\Players\\形象\\法师女\\躯干\\法师女.ecm",
	// 汐族 巫师
	"Models\\Players\\形象\\巫师男\\躯干\\巫师男.ecm",
	"Models\\Players\\形象\\巫师女\\躯干\\巫师女.ecm",
	// 妖族
	"",
	"Models\\Players\\形象\\妖精\\躯干\\妖精.ecm",
	"Models\\Players\\形象\\妖兽男\\躯干\\妖兽男.ecm",
	"",
	// 汐族 刺客
	"Models\\Players\\形象\\刺客男\\躯干\\刺客男.ecm",
	"Models\\Players\\形象\\刺客女\\躯干\\刺客女.ecm",
	// 羽族
	"Models\\Players\\形象\\羽芒男\\躯干\\羽芒男.ecm",
	"Models\\Players\\形象\\羽芒女\\躯干\\羽芒女.ecm",
	"Models\\Players\\形象\\羽灵男\\躯干\\羽灵男.ecm",
	"Models\\Players\\形象\\羽灵女\\躯干\\羽灵女.ecm",
	// 灵族
	"Models\\Players\\形象\\剑灵男\\躯干\\剑灵男.ecm",
	"Models\\Players\\形象\\剑灵女\\躯干\\剑灵女.ecm",
	"Models\\Players\\形象\\魅灵男\\躯干\\魅灵男.ecm",
	"Models\\Players\\形象\\魅灵女\\躯干\\魅灵女.ecm",
	// 职业变身模型
	"Models\\Players\\形象\\白虎\\白虎.ecm",
	"Models\\Players\\形象\\火狐狸\\火狐狸.ecm",
	"Models\\Players\\形象\\影族变身男\\影族变身男.ecm",
	"Models\\Players\\形象\\影族变身女\\影族变身女.ecm",

	// 技能变身模型（此ID不可变，新模型请往后添加）
	"Models\\Players\\形象\\金钱蛙\\金钱蛙.ecm",
	"Models\\Players\\形象\\婚礼童男\\婚礼童男.ecm",
	"Models\\Players\\形象\\婚礼童女\\婚礼童女.ecm",
	"Models\\Players\\形象\\2011变身\\园艺区哨兵\\园艺区哨兵.ecm",
	"Models\\Players\\形象\\树鸡\\树鸡.ecm",
	"Models\\Players\\形象\\小魔虎\\小魔虎.ecm",
	"Models\\Players\\形象\\红色松鼠\\红色松鼠.ecm",
	"Models\\Players\\形象\\陆化的海龟\\陆化的海龟.ecm",
	"Models\\Players\\形象\\新娘的表哥\\新娘的表哥.ecm",
	"Models\\Players\\形象\\哨兵傀儡\\哨兵傀儡.ecm",
	"Models\\Players\\形象\\2013变身\\企鹅帝王\\企鹅帝王.ecm",
	"Models\\Players\\形象\\2013变身\\企鹅帝王\\企鹅皇后.ecm",
	"Models\\Players\\形象\\2013变身\\雪兔\\雪兔.ecm",
	"Models\\Players\\形象\\2013变身\\泰迪熊\\泰迪熊.ecm",
	"Models\\Players\\形象\\红色松鼠\\红色松鼠大.ecm",
	"Models\\Players\\形象\\树鸡\\树鸡大.ecm",
	"Models\\Players\\形象\\2011变身\\园艺区哨兵\\园艺区哨兵愤怒.ecm",
	"Models\\Players\\形象\\2011变身\\园艺区哨兵\\园艺区哨兵沮丧.ecm",
	"Models\\Players\\形象\\2011变身\\园艺区哨兵\\园艺区哨兵失落.ecm",
	"Models\\Players\\形象\\2013变身\\雪兔\\雪兔电光.ecm",
	"Models\\Players\\形象\\2013变身\\龙\\龙.ecm",
	// 职业变身新模型
	"Models\\Players\\形象\\熊猫\\熊猫.ecm",
	"Models\\Players\\形象\\蝶羽狐\\蝶羽狐.ecm",

	// 默认摆摊模型
	"Models\\Players\\形象\\摆摊\\男\\男.ecm",
	"Models\\Players\\形象\\摆摊\\女\\女.ecm",

	// 胧族
	"Models\\Players\\形象\\夜影男\\躯干\\夜影男.ecm",
	"Models\\Players\\形象\\夜影女\\躯干\\夜影女.ecm",
	"Models\\Players\\形象\\月仙男\\躯干\\月仙男.ecm",
	"Models\\Players\\形象\\月仙女\\躯干\\月仙女.ecm",
	
	//	胧族职业变身
	"Models\\Players\\形象\\胧族变身夜影男\\胧族变身夜影男.ecm",
	"Models\\Players\\形象\\胧族变身夜影女\\胧族变身夜影女.ecm",
	"Models\\Players\\形象\\胧族变身月仙男\\胧族变身月仙男.ecm",
	"Models\\Players\\形象\\胧族变身月仙女\\胧族变身月仙女.ecm",
};


//	Model files
static const char* l_aModelFilesForChangeShape2[RES_MOD_FORCHANGESAHPE_NUM] = // 小动物变身，变身类型为 2
{
	"",

	// 战车变身
	"Models\\Players\\形象\\2013变身\\投石车\\投石车初级.ecm",
	"Models\\Players\\形象\\2013变身\\投石车\\投石车中级.ecm",
	"Models\\Players\\形象\\2013变身\\投石车\\投石车高级.ecm",
	"Models\\Players\\形象\\2013变身\\远程战车\\远程战车初级.ecm",

	"Models\\Players\\形象\\2013变身\\远程战车\\远程战车中级.ecm",						// 5
	"Models\\Players\\形象\\2013变身\\远程战车\\远程战车高级.ecm", 
	"Models\\Players\\形象\\2013变身\\弓弩战车\\弓弩战车初级.ecm",
	"Models\\Players\\形象\\2013变身\\弓弩战车\\弓弩战车中级.ecm",
	"Models\\Players\\形象\\2013变身\\弓弩战车\\弓弩战车高级.ecm",

	"Models\\Players\\形象\\2013变身\\忧郁中年\\忧郁中年.ecm", // 10
	"Models\\players\\形象\\2013变身\\浣熊男红\\浣熊男红.ecm",
	"Models\\Players\\形象\\2013变身\\小马车\\小马车.ecm",
	"Models\\Players\\形象\\小雪人\\小雪人.ecm",
	"",
	
	"", //15
	"",
	"",
	"",
	"",
	
	"", // 20
	"",
	"",
	"",
	"",
	
	"", // 25 
	
	// 技能变身模型（此ID不可变，新模型请往后添加）
	"Models\\Players\\形象\\金钱蛙\\金钱蛙.ecm",
	"Models\\Players\\形象\\婚礼童男\\婚礼童男.ecm",
	"Models\\Players\\形象\\婚礼童女\\婚礼童女.ecm",
	"Models\\Players\\形象\\2011变身\\园艺区哨兵\\园艺区哨兵.ecm",
	"Models\\Players\\形象\\树鸡\\树鸡.ecm",
	"Models\\Players\\形象\\小魔虎\\小魔虎.ecm",
	"Models\\Players\\形象\\红色松鼠\\红色松鼠.ecm",
	"Models\\Players\\形象\\陆化的海龟\\陆化的海龟.ecm",
	"Models\\Players\\形象\\新娘的表哥\\新娘的表哥.ecm",
	"Models\\Players\\形象\\哨兵傀儡\\哨兵傀儡.ecm",
	"Models\\Players\\形象\\2013变身\\企鹅帝王\\企鹅帝王.ecm",
	"Models\\Players\\形象\\2013变身\\企鹅帝王\\企鹅皇后.ecm",
	"Models\\Players\\形象\\2013变身\\雪兔\\雪兔.ecm",
	"Models\\Players\\形象\\2013变身\\泰迪熊\\泰迪熊.ecm",
	"Models\\Players\\形象\\红色松鼠\\红色松鼠大.ecm",
	"Models\\Players\\形象\\树鸡\\树鸡大.ecm",
	"Models\\Players\\形象\\2011变身\\园艺区哨兵\\园艺区哨兵愤怒.ecm",
	"Models\\Players\\形象\\2011变身\\园艺区哨兵\\园艺区哨兵沮丧.ecm",
	"Models\\Players\\形象\\2011变身\\园艺区哨兵\\园艺区哨兵失落.ecm",
	"Models\\Players\\形象\\2013变身\\雪兔\\雪兔电光.ecm",
	"Models\\Players\\形象\\2013变身\\龙\\龙.ecm",
	"Models\\Players\\形象\\2014变身\\风法师\\风法师.ecm",
	"Models\\Players\\形象\\2014变身\\夜影影遁变身\\影遁.ecm",
	"Models\\Players\\形象\\2015变身\\蛋蛋\\蛋蛋.ecm",
	"Models\\Players\\形象\\2015变身\\肥羊\\肥羊.ecm",
	"Models\\Players\\形象\\2015变身\\龙猫\\龙猫.ecm",
	"Models\\Players\\形象\\2015变身\\泡泡鱼\\泡泡鱼.ecm",
};

static const char* l_szSysIconFile[] = {
	"2013\\快捷栏\\gt(正常).dds",
	"2013\\快捷栏\\touch(正常).dds",
	"2013\\快捷栏\\修炼(正常).dds",
	"2013\\快捷栏\\百科(正常).dds",
	"2013\\快捷栏\\寄卖(正常).dds",
	"2013\\快捷栏\\完美服务(正常).dds",
	"2013\\快捷栏\\情缘(正常).dds",
	"2013\\快捷栏\\聚灵(正常).dds",
	"2013\\快捷栏\\自动喝药(正常).dds",
	"",
};
///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Implement
//	
///////////////////////////////////////////////////////////////////////////

const char* res_CursorFile(int n)
{
	ASSERT(n >= 0 && n < NUM_RES_CURSOR);
	return l_aCurFiles[n];
}

const char* res_GFXFile(int n)
{
	ASSERT(n >= 0 && n < NUM_RES_GFX);
	return l_aGFXFiles[n];
}

const char* res_SoundFile(int n)
{
	return NULL;
}

const char* res_TextureFile(int n)
{
	return NULL;
}

const char* res_ShaderFile(int n)
{
	ASSERT(n >= 0 && n < NUM_RES_SHADER);
	return l_aShaderFiles[n];
}

const char* res_IconFile(int n)
{
	ASSERT(n >= 0 && n < NUM_RES_ICON);
	return l_aIconFiles[n];
}

const char* res_ModelFile(int n)
{
	ASSERT(n >= 0 && n < NUM_RES_MODEL);
	return l_aModelFiles[n];
}
const char* res_SysModuleIconFile(int n)
{
	if (n>=0 && n< sizeof(l_szSysIconFile)/sizeof(l_szSysIconFile[0]))
	{
		return l_szSysIconFile[n];
	}
	return NULL;
}
const char* res_ModelFileForChangeShape2(int n)
{
	if(n >= 0 && n < RES_MOD_FORCHANGESAHPE_NUM)
		return l_aModelFilesForChangeShape2[n];
	else
		return NULL;
}