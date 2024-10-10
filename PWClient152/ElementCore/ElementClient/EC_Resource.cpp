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
	"��������\\��ɫ��������.gfx",	//	RES_GFX_LEVELUP,
	"��������\\��ɫ��������.gfx",	//	RES_GFX_PLAYERAPPEAR,
	"������\\������Ч��.gfx",		//	RES_GFX_HIT,
	"��������\\ָ���.gfx",			//	RES_GFX_MOVETARGET,
	"��������\\���Ч��.gfx",		//	RES_GFX_ITEMFLASH,
	"��������\\�������.gfx",		//	RES_GFX_CURSORHOVER,
	"��������\\Ŀ�걻ѡ��.gfx",		//	RES_GFX_SELECTED,
	"��������\\����Ʈ��.gfx",		//	RES_GFX_FLOATING_DUST,
	"��������\\�����ζ�ˮȦ.gfx",	//	RES_GFX_WATER_WAVE_STILL,
	"��������\\�����ζ�ˮ��.gfx",	//	RES_GFX_WATER_WAVE_MOVE,
	"��������\\ˮ�к�������.gfx",	//	RES_GFX_AIR_BUBBLE,
	"��������\\ˮ���ζ�����.gfx",	//	RES_GFX_SWIM_BUBBLE,
	"��������\\�׻���.gfx",			//	RES_GFX_TRANSFORM_TIGER,
	"��������\\�����.gfx",			//	RES_GFX_TRANSFORM_FOX,
	"��������\\��Ѫ.gfx",			//	RES_GFX_RED_MEDICINE,
	"��������\\��ħ.gfx",			//	RES_GFX_BLUE_MEDICINE,
	"��������\\��Ѫ��ħ.gfx",		//	RES_GFX_PURPLE_MEDICINE,
	"��������\\�ⶾ.gfx",			//	RES_GFX_JIEDU_MEDICINE,
	"��������\\������ͷ�.gfx",		//	RES_GFX_PHYSIC_ARMORRUNE,
	"��������\\�������ͷ�.gfx",		//	RES_GFX_MAGIC_ARMORRUNE,
	"��������\\����״̬.gfx",		//	RES_GFX_INDUEL,
	"��������\\����Ͷʯ����.gfx",	//	RES_GFX_TANKLEADER_RED,
	"��������\\����Ͷʯ����.gfx",	//	RES_GFX_TANKLEADER_BLUE,
	"����\\����ͨ��\\��������.gfx",//  RES_GFX_GOBLIN_LEVELUP,
	"��������\\��ɫ��������.gfx",	//	RES_GFX_INVISIBLE_DETECTED
	"�߻�����\\״̬Ч��\\��ɫ����.gfx",		// RES_GFX_MONSTER_SPIRIT_LINE
	"��������\\������Ч_��Ч��01.gfx", // RES_GFX_MONSTER_SPIRIT_BALL
	"����\\ͨ��\\����\\��������.gfx",
	"�߻�����\\״̬Ч��\\����%d��.gfx",
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
	"����.dds",
	"����ת��.dds",
	"��ͨ����.dds",
	"Ѱ��Ŀ��.dds",
	"Э������.dds",

	"�������.dds",
	"�������.dds",
	"�߳�����.dds",
	"Ѱ�Ҷ���.dds",
	"��������.dds",

	"��̯��.dds",
	"��̯��.dds",
	"�������.dds",
	"����.dds",
	"����.dds",		//	RES_ICON_CMD_EXP_WAVE,

	"��ͷ.dds",		//	RES_ICON_CMD_EXP_NOD,
	"ҡͷ.dds",		//	RES_ICON_CMD_EXP_SHAKEHEAD,
	"�ʼ��.dds",	//	RES_ICON_CMD_EXP_SHRUG,	
	"��Ц.dds",		//	RES_ICON_CMD_EXP_LAUGH,	
	"����.dds",		//	RES_ICON_CMD_EXP_ANGRY,	

	"�ε�.dds",		//	RES_ICON_CMD_EXP_STUN,	
	"��ɥ.dds",		//	RES_ICON_CMD_EXP_DEPRESSED,
	"����.dds",		//	RES_ICON_CMD_EXP_KISSHAND,
	"����.dds",		//	RES_ICON_CMD_EXP_SHY,	
	"��ȭ.dds",		//	RES_ICON_CMD_EXP_SALUTE,	

	"����.dds",		//	RES_ICON_CMD_EXP_SITDOWN,
	"���.dds",		//	RES_ICON_CMD_EXP_ASSAULT,
	"˼��.dds",		//	RES_ICON_CMD_EXP_THINK,	
	"����.dds",		//	RES_ICON_CMD_EXP_DEFIANCE,
	"ʤ��.dds",		//	RES_ICON_CMD_EXP_VICTORY,

	"������.dds",	//	RES_ICON_CMD_EXP_GAPE
	"����.dds",		//	RES_ICON_CMD_EXP_KISS
	"ս��.dds",		//	RES_ICON_CMD_EXP_FIGHT,	
	"����1.dds",	//	RES_ICON_CMD_EXP_ATTACK1,
	"����2.dds",	//	RES_ICON_CMD_EXP_ATTACK2,

	"����3.dds",	//	RES_ICON_CMD_EXP_ATTACK3,
	"����4.dds",	//	RES_ICON_CMD_EXP_ATTACK4,
	"����.dds",		//	RES_ICON_CMD_EXP_DEFENCE,
	"ˤ��.dds",		//	RES_ICON_CMD_EXP_FALL,	
	"����.dds",		//	RES_ICON_CMD_EXP_FALLONGROUND,

	"����.dds",		//	RES_ICON_CMD_EXP_LOOKAROUND,
	"�赸1.dds",	//	RES_ICON_CMD_EXP_DANCE,
	"�赸2.dds",	//	RES_ICON_CMD_EXP_FASHIONWEAPON
	"ʰȡ.dds",		//	RES_ICON_CMD_PICKUP
	"�ھ�.dds",		//	RES_ICON_CMD_GATHER
	"���ٷ���.dds",	//	RES_ICON_CMD_RUSHFLY

	"�������˵Ķ���.dds",	//	RES_ICON_CMD_BINDBUDDY
	"�������ܵĶ���.dds",	//	RES_ICON_CMD_TWOKISS
	"��Ծ�Ķ���1.dds",	//	RES_ICON_CMD_JUMPTRICK,
	"��Ծ�Ķ���2.dds",	//	RES_ICON_CMD_RUNTRICK,
};

//	Model files
static const char* l_aModelFiles[NUM_RES_MODEL] =
{
	"Models\\Matters\\��Ʒ\\���\\���.ecm",
	"Models\\Matters\\��Ʒ\\Ԫ��\\Ԫ��.ecm",
	// ����
	"Models\\Players\\����\\������\\����\\������.ecm",
	"Models\\Players\\����\\����Ů\\����\\����Ů.ecm",
	"Models\\Players\\����\\��ʦ��\\����\\��ʦ��.ecm",
	"Models\\Players\\����\\��ʦŮ\\����\\��ʦŮ.ecm",
	// ϫ�� ��ʦ
	"Models\\Players\\����\\��ʦ��\\����\\��ʦ��.ecm",
	"Models\\Players\\����\\��ʦŮ\\����\\��ʦŮ.ecm",
	// ����
	"",
	"Models\\Players\\����\\����\\����\\����.ecm",
	"Models\\Players\\����\\������\\����\\������.ecm",
	"",
	// ϫ�� �̿�
	"Models\\Players\\����\\�̿���\\����\\�̿���.ecm",
	"Models\\Players\\����\\�̿�Ů\\����\\�̿�Ů.ecm",
	// ����
	"Models\\Players\\����\\��â��\\����\\��â��.ecm",
	"Models\\Players\\����\\��âŮ\\����\\��âŮ.ecm",
	"Models\\Players\\����\\������\\����\\������.ecm",
	"Models\\Players\\����\\����Ů\\����\\����Ů.ecm",
	// ����
	"Models\\Players\\����\\������\\����\\������.ecm",
	"Models\\Players\\����\\����Ů\\����\\����Ů.ecm",
	"Models\\Players\\����\\������\\����\\������.ecm",
	"Models\\Players\\����\\����Ů\\����\\����Ů.ecm",
	// ְҵ����ģ��
	"Models\\Players\\����\\�׻�\\�׻�.ecm",
	"Models\\Players\\����\\�����\\�����.ecm",
	"Models\\Players\\����\\Ӱ�������\\Ӱ�������.ecm",
	"Models\\Players\\����\\Ӱ�����Ů\\Ӱ�����Ů.ecm",

	// ���ܱ���ģ�ͣ���ID���ɱ䣬��ģ����������ӣ�
	"Models\\Players\\����\\��Ǯ��\\��Ǯ��.ecm",
	"Models\\Players\\����\\����ͯ��\\����ͯ��.ecm",
	"Models\\Players\\����\\����ͯŮ\\����ͯŮ.ecm",
	"Models\\Players\\����\\2011����\\԰�����ڱ�\\԰�����ڱ�.ecm",
	"Models\\Players\\����\\����\\����.ecm",
	"Models\\Players\\����\\Сħ��\\Сħ��.ecm",
	"Models\\Players\\����\\��ɫ����\\��ɫ����.ecm",
	"Models\\Players\\����\\½���ĺ���\\½���ĺ���.ecm",
	"Models\\Players\\����\\����ı��\\����ı��.ecm",
	"Models\\Players\\����\\�ڱ�����\\�ڱ�����.ecm",
	"Models\\Players\\����\\2013����\\������\\������.ecm",
	"Models\\Players\\����\\2013����\\������\\���ʺ�.ecm",
	"Models\\Players\\����\\2013����\\ѩ��\\ѩ��.ecm",
	"Models\\Players\\����\\2013����\\̩����\\̩����.ecm",
	"Models\\Players\\����\\��ɫ����\\��ɫ�����.ecm",
	"Models\\Players\\����\\����\\������.ecm",
	"Models\\Players\\����\\2011����\\԰�����ڱ�\\԰�����ڱ���ŭ.ecm",
	"Models\\Players\\����\\2011����\\԰�����ڱ�\\԰�����ڱ���ɥ.ecm",
	"Models\\Players\\����\\2011����\\԰�����ڱ�\\԰�����ڱ�ʧ��.ecm",
	"Models\\Players\\����\\2013����\\ѩ��\\ѩ�õ��.ecm",
	"Models\\Players\\����\\2013����\\��\\��.ecm",
	// ְҵ������ģ��
	"Models\\Players\\����\\��è\\��è.ecm",
	"Models\\Players\\����\\�����\\�����.ecm",

	// Ĭ�ϰ�̯ģ��
	"Models\\Players\\����\\��̯\\��\\��.ecm",
	"Models\\Players\\����\\��̯\\Ů\\Ů.ecm",

	// ����
	"Models\\Players\\����\\ҹӰ��\\����\\ҹӰ��.ecm",
	"Models\\Players\\����\\ҹӰŮ\\����\\ҹӰŮ.ecm",
	"Models\\Players\\����\\������\\����\\������.ecm",
	"Models\\Players\\����\\����Ů\\����\\����Ů.ecm",
	
	//	����ְҵ����
	"Models\\Players\\����\\�������ҹӰ��\\�������ҹӰ��.ecm",
	"Models\\Players\\����\\�������ҹӰŮ\\�������ҹӰŮ.ecm",
	"Models\\Players\\����\\�������������\\�������������.ecm",
	"Models\\Players\\����\\�����������Ů\\�����������Ů.ecm",
};


//	Model files
static const char* l_aModelFilesForChangeShape2[RES_MOD_FORCHANGESAHPE_NUM] = // С���������������Ϊ 2
{
	"",

	// ս������
	"Models\\Players\\����\\2013����\\Ͷʯ��\\Ͷʯ������.ecm",
	"Models\\Players\\����\\2013����\\Ͷʯ��\\Ͷʯ���м�.ecm",
	"Models\\Players\\����\\2013����\\Ͷʯ��\\Ͷʯ���߼�.ecm",
	"Models\\Players\\����\\2013����\\Զ��ս��\\Զ��ս������.ecm",

	"Models\\Players\\����\\2013����\\Զ��ս��\\Զ��ս���м�.ecm",						// 5
	"Models\\Players\\����\\2013����\\Զ��ս��\\Զ��ս���߼�.ecm", 
	"Models\\Players\\����\\2013����\\����ս��\\����ս������.ecm",
	"Models\\Players\\����\\2013����\\����ս��\\����ս���м�.ecm",
	"Models\\Players\\����\\2013����\\����ս��\\����ս���߼�.ecm",

	"Models\\Players\\����\\2013����\\��������\\��������.ecm", // 10
	"Models\\players\\����\\2013����\\����к�\\����к�.ecm",
	"Models\\Players\\����\\2013����\\С��\\С��.ecm",
	"Models\\Players\\����\\Сѩ��\\Сѩ��.ecm",
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
	
	// ���ܱ���ģ�ͣ���ID���ɱ䣬��ģ����������ӣ�
	"Models\\Players\\����\\��Ǯ��\\��Ǯ��.ecm",
	"Models\\Players\\����\\����ͯ��\\����ͯ��.ecm",
	"Models\\Players\\����\\����ͯŮ\\����ͯŮ.ecm",
	"Models\\Players\\����\\2011����\\԰�����ڱ�\\԰�����ڱ�.ecm",
	"Models\\Players\\����\\����\\����.ecm",
	"Models\\Players\\����\\Сħ��\\Сħ��.ecm",
	"Models\\Players\\����\\��ɫ����\\��ɫ����.ecm",
	"Models\\Players\\����\\½���ĺ���\\½���ĺ���.ecm",
	"Models\\Players\\����\\����ı��\\����ı��.ecm",
	"Models\\Players\\����\\�ڱ�����\\�ڱ�����.ecm",
	"Models\\Players\\����\\2013����\\������\\������.ecm",
	"Models\\Players\\����\\2013����\\������\\���ʺ�.ecm",
	"Models\\Players\\����\\2013����\\ѩ��\\ѩ��.ecm",
	"Models\\Players\\����\\2013����\\̩����\\̩����.ecm",
	"Models\\Players\\����\\��ɫ����\\��ɫ�����.ecm",
	"Models\\Players\\����\\����\\������.ecm",
	"Models\\Players\\����\\2011����\\԰�����ڱ�\\԰�����ڱ���ŭ.ecm",
	"Models\\Players\\����\\2011����\\԰�����ڱ�\\԰�����ڱ���ɥ.ecm",
	"Models\\Players\\����\\2011����\\԰�����ڱ�\\԰�����ڱ�ʧ��.ecm",
	"Models\\Players\\����\\2013����\\ѩ��\\ѩ�õ��.ecm",
	"Models\\Players\\����\\2013����\\��\\��.ecm",
	"Models\\Players\\����\\2014����\\�編ʦ\\�編ʦ.ecm",
	"Models\\Players\\����\\2014����\\ҹӰӰ�ݱ���\\Ӱ��.ecm",
	"Models\\Players\\����\\2015����\\����\\����.ecm",
	"Models\\Players\\����\\2015����\\����\\����.ecm",
	"Models\\Players\\����\\2015����\\��è\\��è.ecm",
	"Models\\Players\\����\\2015����\\������\\������.ecm",
};

static const char* l_szSysIconFile[] = {
	"2013\\�����\\gt(����).dds",
	"2013\\�����\\touch(����).dds",
	"2013\\�����\\����(����).dds",
	"2013\\�����\\�ٿ�(����).dds",
	"2013\\�����\\����(����).dds",
	"2013\\�����\\��������(����).dds",
	"2013\\�����\\��Ե(����).dds",
	"2013\\�����\\����(����).dds",
	"2013\\�����\\�Զ���ҩ(����).dds",
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