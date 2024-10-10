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
	GP_MOVE_BLINK	= 10,		// only sent to NPC��˲�ƣ�
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
	GP_NPCSEV_WEDDING_BOOK,				//	���Ԥ������
	GP_NPCSEV_WEDDING_INVITE,			//	��д������
	GP_NPCSEV_FACTION_FORTRESS_SERVICE_1,	//	���ɻ��ط���1����������ս��
	GP_NPCSEV_FACTION_FORTRESS_SERVICE_2,	//	���ɻ��ط���2��������������ʩ�������ύ���ϡ��ύ���׶ȣ�

	GP_NPCSEV_FACTION_FORTRESS_SERVICE_3,	//	66	���ɻ��ز���ת������
	GP_NPCSEV_PET_DYE,
	GP_NPCSEV_VIEW_TRASHBOX,					// check the trash box without password
	GP_NPCSEV_ENGRAVE,					//	�Կ�
	GP_NCPSEV_DPS_DPH_RANK,		//	ɳ�����а�

	GP_NPCSEV_ADDONREGEN,		//	71 ���������������
	GP_NPCSEV_FORCE,			//	����NPC����
	GP_NPCSEV_TRANSMIT_DIRECT,	//	NPC ֱ�Ӵ���
	GP_NPCSEV_PREVIEW_PRODUCE,  //  ��Ԥ������
	GP_NPCSEV_COUNTRY_JOINLEAVE,//	��ս�����߼�

	GP_NPCSEV_COUNTRY_LEAVEWAR,	//	76 ��ս�뿪��ǰս��
	GP_NPCSEV_MARK,
	GP_NPCSEV_CROSSSERVER_GETIN,
	GP_NPCSEV_CROSSSERVER_GETOUT,

	GP_NPCSEV_PLAYER_RENAME,    // 80 ��Ҹ�������
	GP_NPCSEV_STONE_TRANSFER,	// ��ʯת��
	GP_NPCSEV_STONE_REPLACE,	// ��ʯ�滻
	GP_NPCSEV_KINGSEV,			// ������ط���
	GP_NPCSEV_SPLIT_FASHION,	// ʱװ���

	GP_NPCSEV_OFFLINESHOP,		// �����̵�(��������)
	GP_NPCSEV_REINCARNATION,
	GP_NPCSEV_GIFTCARD,

	GP_NPCSEV_TRICKBATTLE,		// 88, ����
	GP_NPCSEV_CARDRESPAWN,		// ����ת��
	GP_NPCSEV_FLYSWORDIMPROVE,  // �ɽ�ǿ��
	GP_NPCSEV_OPEN_FACTION_PVP,	//	���������Ӷ�ս

	GP_NPCSEV_ADVANCED_PRODUCE, //92,����������5
	GP_NPCSEV_GOLD_SHOP,		//	NPC Ԫ���̳�
	GP_NPCSEV_DIVIDEND_GOLD_SHOP,//	NPC ����Ԫ���̳�
	GP_NPCSEV_PLAYER_CHANGE_GENDER,// NPC �޸��Ա�
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
	GP_STATE_IN_DUEL			= 0x00040000,	//	�Ƿ����ھ�����
	GP_STATE_IN_MOUNT			= 0x00080000,	//	���������
	GP_STATE_IN_BIND			= 0x00100000,	//	�ͱ��˰���һ��
	GP_STATE_BC_INVADER			= 0x00200000,	//	Battle camp: invader
	GP_STATE_BC_DEFENDER		= 0x00400000,	//	Battle camp: defender
	GP_STATE_SPOUSE				= 0x00800000,	//	��żid
	GP_STATE_GOBLINREFINE		= 0x01000000,	//  С�����Ƿ��Ѿ�����
	GP_STATE_SHIELDUSER			= 0x02000000,	//  �Ƿ���������û�
	GP_STATE_INVISIBLE			= 0x04000000,	//  ����
	GP_STATE_EQUIPDISABLED		= 0x08000000,	//  Equipment disabled
	GP_STATE_FORBIDBESELECTED	= 0x10000000,	//	��ֹ��ѡ�� (NPCҲ��Ч)
	GP_STATE_PLAYERFORCE		= 0x20000000,	//	�Ѽ�������
	GP_STATE_MULTIOBJ_EFFECT	= 0x40000000,	//	�����������������Ч��
	GP_STATE_COUNTRY             = 0x80000000,		//�Ѽ������

	//	Used only by NPC
	GP_STATE_NPC_DELAYDEAD		= 0x00000008,
	GP_STATE_NPC_ADDON1			= 0x00000100,
	GP_STATE_NPC_ADDON2			= 0x00000200,
	GP_STATE_NPC_ADDON3			= 0x00000400,
	GP_STATE_NPC_ADDON4			= 0x00000800,
	GP_STATE_NPC_ALLADDON		= 0x00000F00,
	GP_STATE_NPC_PET			= 0x00001000,	//	Pet flag
	GP_STATE_NPC_NAME			= 0x00002000,
	GP_STATE_NPC_FIXDIR			= 0x00004000,	//	����̶�
	GP_STATE_NPC_MAFIA			= 0x00008000,	//	�������ɣ����ڰ���PVP�п󳵵ȣ�
	GP_STATE_NPC_FLY			= 0x00010000,
	GP_STATE_NPC_SWIM			= 0x00020000,
};

//  Player and NPC state2
enum
{
	GP_STATE2_ISKING			= 0x00000001,	//  ����Ƿ����
	GP_STATE2_TITLE				= 0x00000002,   //	�ƺ�
	GP_STATE2_REINCARNATION		= 0x00000004,	//	ת��
	GP_STATE2_REALM				= 0x00000008,	//	����
	GP_STATE2_IN_BATTLE			= 0x00000010,	//	ս��״̬
	GP_STATE2_FACTION_PVP_MASK	= 0X00000020,	//	���� PVP Mask
	GP_STATE2_GENDER			= 0x00000040,	//	�Ա�
};

//	Chat channel
enum
{
	//���ö�ٶ�������Ķ��Ļ���Ҫ֪ͨ�����ˣ���������������Ա
	//��Ǳ�Ҫ���������ں�����ӣ�����ɾ���͸���ǰ��channel��˳��
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
	GP_CT_NULL = 0,					//	�գ�����
	GP_CT_PVP,                     	//	û�ã���ռλ
	GP_CT_EMOTE,                   	//	���������ȴʱ�� 
	GP_CT_REJUVENATION_POTION,     	//	����������ȴʱ��
	GP_CT_SWITCH_FASHION,          	//	�л�ʱװģʽ����ȴʱ��
	GP_CT_DROP_MONEY,              	//	��������Ǯ����ȴʱ��
	GP_CT_DROP_ITEM,               	//	����������Ʒ����ȴʱ��
	GP_CT_FACEPILL,                	//	���������ȴʱ��
	GP_CT_FACETICKET,              	//	���ݵ���ȴʱ��
	GP_CT_RECURRECT_SCROLL,        	//	���˸�������ȴʱ�䣬����Ʒ�����ڣ�δ��Ч
	GP_CT_SOUL_STONE,              	//	���ڸ���������ȴʱ��
	GP_CT_HP_POTION,				//	��Ѫҩ
	GP_CT_MP_POTION,				//	��ħҩ
	GP_CT_ANTIDOTE_POTION,			//	�ⶾ��
	GP_CT_FLY_RUSH,					//	���ٷ���
	GP_CT_TOWNSCROLL,				//	�سǾ���
	GP_CT_GM_GENITEM,				//	GM ���ɹ������Ʒ
	GP_CT_VIEWOTHEREQUIP,			//	�鿴���˵�װ��
	GP_CT_FEED_PET,					//	ι������
	GP_CT_FIREWORKS,				//	ʩ���̻�
	GP_CT_FARCRY,					//	far cry Ƶ��˵��
	GP_CT_SKILLMATTER,				//	ʹ�ü�����Ʒ
	GP_CT_REFINE,					//	����
	GP_CT_ARMORRUNE,				//	�����Ż���
	GP_CT_AUTOHP,					//	�������ȴʱ��
	GP_CT_AUTOMP,					//	�������ȴʱ��
	GP_CT_DOUBLEEXP,				//	˫�����������ȴʱ��
	GP_CT_DYETICKET,				//	Ⱦɫ����
	GP_CT_TEAMRELATION,				//	��ӹ�ϵ��ȴʱ��
	GP_CT_REFINETRANS,				//	�����ȼ�ת����ȴʱ��
	
	GP_CT_CAST_ELF_SKILL,			//  С���鼼����ȴ
	GP_CT_ELF_CMD,					//  ��ռλ
	GP_CT_GET_MALL_PRICE,			//  ��ȡ�̳����ݵ���ȴʱ��
	GP_CT_QUERY_OTHER_PROPERTY,		//  ��ѯĿ������ 5��
	GP_CT_SKILLTRIGGER2,			//	ʹ�ü�����Ʒ1��

	GP_CT_SKILLCOMMONCOOLDOWN0,		//  ���ڼ��ܹ�����ȴ
	GP_CT_SKILLCOMMONCOOLDOWN1,
	GP_CT_SKILLCOMMONCOOLDOWN2,
	GP_CT_SKILLCOMMONCOOLDOWN3,
	GP_CT_SKILLCOMMONCOOLDOWN4,

	GP_CT_RUNECOMMONCOOLDOWN0,		//  ������Ʒ���ܹ�����ȴ
	GP_CT_RUNECOMMONCOOLDOWN1,
	GP_CT_RUNECOMMONCOOLDOWN2,
	GP_CT_RUNECOMMONCOOLDOWN3,
	GP_CT_RUNECOMMONCOOLDOWN4,

	GP_CT_EQUIP_FASHION_ITEM,		//	�Զ���װ��ȴʱ�� 60s
	GP_CT_GET_DIVIDEND_MALL_PRICE,	//  ��ȡ�����̳����ݵ���ȴʱ�� 30s
	GP_CT_MULTI_EXCHANGE_ITEM,      //  ���������ȴʱ�� 30s

	GP_CT_TEAM_CONGREGATE,			//	���鼯����
	GP_CT_FACTION_CONGREGATE,		//	���ɼ�����
	GP_CT_DPS_DPH_RANK,				//	��ȡɳ�����а�
	GP_CT_JOIN_PLAYER_FORCE,		//	��������
	GP_CT_LEAVE_PLAYER_FORCE,		//	�뿪����
	GP_CT_TOGGLE_ONLINE_AWARD,		//	�л��һ������߽���
	GP_CT_COUNTRY_BATTLE_APPLY,		//	�����ս����
	GP_CT_COUNTRY_CHAT,				//	��սƵ������
	GP_CT_CROSS_SERVER_APPLY,		//	ǰ���������

	GP_CT_TOUCHTRADE,				//	Touch �㹺����ѯ
	GP_CT_SWITCH_PARALLEL_WORLD,	//	�л�����
	GP_CT_QUERY_PARALLEL_WORLD,		//	��ѯ����

	GP_CT_GIFTCARD_REDEEM,
    GP_CT_TRICKBATTLE_APPLY,		//  ����, ս��
	GP_CT_AUTOTEAM,					//  �Զ���Ӳ�������ͼ��ת
	GP_CT_PLAYER_GATHER,			//	��ֹ�ɼ����� (GATHER_MATERIAL)
	GP_CT_COUNTRYBATTLE_LIVESHOW,	//	��ѯ��սrank���������� (COUNTRYBATTLE_LIVE_SHOW)

	GP_CT_QUERY_MAFIA_PVP_INFO,		//	65: ���� PVP ��Ϣ��ѯ (QUERY_MAFIA_PVP_INFO)

	GP_CT_MAX,
	GP_CT_SKILL_START = 1024,
};

//	PVP mask
enum
{
	GP_PVPMASK_FORCE		= 0x0001, // ǿ������
	GP_PVPMASK_NOMAFIA		= 0x0002,
	GP_PVPMASK_NOWHITE		= 0x0004,
	GP_PVPMASK_NOALLIANCE	= 0x0008,
	GP_PVPMASK_NOFORCE		= 0x0010,//������ͬ������
	
	GP_BLSMASK_NORED		= 0x0008,
	GP_BLSMASK_NOMAFIA		= 0x0010,
	GP_BLSMASK_SELF			= 0x0020,
	GP_BLSMASK_NOALLIANCE	= 0x0040,
	GP_BLSMASK_NOFORCE		= 0x0080, // ��������
};

//	Refuse Bless Mask
enum REFUSE_BLESS_MASK
{
	REFUSE_NEUTRAL_BLESS = 0x0001,				//	����������ף��
	REFUSE_NON_TEAMMATE_BLESS = 0x0002,	//	�����ܷǶ���ף��
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
	GP_PET_SPECIAL_SKILL_NUM = 1 // ר�����ܣ�Ŀǰֻ�н�������һ���˼���
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

//	���ɻ���
enum
{
	RT_LEVEL_UP = 1,		//	��������
	RT_SET_TECH_POINT,		//	�Ƽ�����������0��Ҫ�����ĸ��Ƽ�
	RT_CONSTRUCT,			//	��ʩ����������0����ʩid������1���������ֵ��0�����٣�1����10%,...��
	RT_HAND_IN_MATERIAL,	//	�����Ͻɣ�����0�������ڰ�������������1������id������2���Ͻ�����
	RT_HAND_IN_CONTRIB,		//	���׶��Ͻɣ�����0�����׶�����
	RT_LEAVE_FORTRESS,		//	�뿪����
	RT_DISMANTLE,			//	���ٻ�����ʩ
	RT_RESET_TECH_POINT,	//  ���ÿƼ���
	RT_MAX
};

// ����������
enum
{
    CONGREGATE_TYPE_TEAM,
    CONGREGATE_TYPE_FACTION,
	CONGREGATE_TYPE_TEAM_MEMBER,
};

// ����ʱ�伶��
enum
{
	PROFIT_LEVEL_NONE,		// ������ʱ��
	PROFIT_LEVEL_YELLOW,	// �����ͼ������ʱ��С��1Сʱ
	PROFIT_LEVEL_NORMAL,	// ������
};

enum
{
    PLAYER_PVP_CLIENT,      //�ͻ�����������,������ر�pvp״̬(������PVE������)
	PLAYER_PVP_PROTECTED,   //��ҽ�����������ֱ�������
	PLAYER_PVP_LEVELUP,		//�������µ�pvp״̬
};

//	����̳ǲ�������
enum
{
	RAND_MALL_OP_QUERY,		//���������Ʒ�������
	RAND_MALL_OP_ROLL,		//��ʼ����̳���Ʒ���
	RAND_MALL_OP_PAY,		//ȷ��֧������̳���Ʒ
};

enum
{
	OBJECT_EXT_STATE_COUNT = 6,	//	���/NPC ����״̬��Ч DWORD ����
};

//	����̳ǲ������
#define RAND_MALL_SHOPPING_RES_OK					0		//	�����ɹ�
#define RAND_MALL_SHOPPING_RES_INVALID_CONFIG		-1		//	����ID�Ƿ�
#define RAND_MALL_SHOPPING_RES_INVALID_ROLE_STATE	-2		//	��ɫ״̬���ʺ�ִ�е�ǰ����
#define RAND_MALL_SHOPPING_RES_INVALID_OP			-3		//	������������ϼȶ��߼����練�����������
#define RAND_MALL_SHOPPING_RES_CASH_NOT_ENOUGH		-4		//	Ԫ������
#define RAND_MALL_SHOPPING_RES_PACK_FULL			-5		//	�����ռ䲻��

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

	//	��ȡѹ�����ݸ���ģ����
	template <typename T>
	bool Extract(T &ret, const byte *&pDataBuf, DWORD &dwDataSize)
	{
		//	��ȡ������������
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
		//	��ȡ����
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
		unsigned char dir;		//256�Ķ�������ʾ����
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

	struct info_player_2		//name ,customize data �����ı�
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
		OBJECT_STARTATTACK,		//	������
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
		EQUIP_DATA,				//	�û���װ�����ݣ�Ӱ�����
		EQUIP_DATA_CHANGED,
		EQUIP_DAMAGED,			//	װ����
		TEAM_MEMBER_PICKUP,		//	���Ѽ���װ��

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
		OBJECT_BE_ATTACKED,		//	������

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
		ONLINE_AWARD_DATA, //���߽���

		TOGGLE_ONLINE_AWARD,		// 330 ����л������߽����Ŀ���״̬
		PLAYER_PROFIT_TIME,			// �������ʱ��
		ENTER_NONPENALTY_PVP_STATE, // PK�޳ͷ�
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
		EQUIP_ADDON_UPDATE_NOTIFY, //354 ֪ͨװ����ʯ���

		SELF_KING_NOTIFY,		// 355
		PLAYER_KING_CHANGED,
		MERIDIANS_NOTIFY,          // ������Ϣ���
		MERIDIANS_RESULT,		   // ������Ѩ���
		COUNTRYBATTLE_STRONGHOND_STATE_NOTIFY,

		QUERY_TOUCH_POINT,		// 360
		SPEND_TOUCH_POINT,
		TOTAL_RECHARGE,
		QUERY_TITLE_RE,		
		CHANGE_CURR_TITLE_RE,   

		MODIFY_TITLE_NOFIFY,	// 365
		REFRESH_SIGNIN,         // ǩ��״̬
		PARALLEL_WORLD_INFO,	
		PLAYER_REINCARNATION,	// ���ת��
		REINCARNATION_TOME_INFO, // ת��������Ϣ

		ACTIVATE_REINCARNATION_TOME,	// 370 ת�������Ƿ񼤻�
        UNIQUE_DATA_NOTIFY, // ��ʷ�ƽ� ��ȫ������
		USE_GIFTCARD_RESULT,
		REALM_EXP,				// ���羭��
		REALM_LEVEL,			// ����ȼ�

		ENTER_TRICKBATTLE,  // 375   ����ս��  ���庬��ͬ��ս����ս��
		TRICKBATTLE_PERSONAL_SCORE, //      ս���Լ��÷�
		TRICKBATTLE_CHARIOT_INFO, // ս����Ϣ
		PLAYER_LEADERSHIP,					// ���ͳ����
		GENERALCARD_COLLECTION_DATA,		// �佫��ͼ������

		ADD_GENERALCARD_COLLECTION,			// 380 �ղؿ���
		REFRESH_MONSTERSPIRIT_LEVEL,		// ����
		MINE_GATHERED,						// ����ɹ��ɼ��㲥
		PLAYER_IN_OUT_BATTLE,				// ������ҽ���/����ս��״̬
		PLAYER_QUERY_CHARIOTS,				// ս��������ѯ

		COUNTRYBATTLE_LIVE_SHOW_RESULT,		// 385 ��սս��rank����������
		RANDOM_MALL_SHOPPING_RES,			//	����̳ǲ�ѯ����
		FACTION_PVP_MASK_MODIFY,			//	����PVP״̬�ı�
		PLAYER_WORLD_CONTRIBUTION,			//	���繱�׶�
		RANDOM_MAP_ORDER ,					//	�����ͼ

		SCENE_SERVICE_NPC_LIST,			//	390��ǰ������ѧϰ���ܵ�NPC�б�
		NPC_VISIBLE_TID_NOTIFY,			//	�㲥֪ͨ��Ұ��npc�任����ģ��
		CLIENT_SCREEN_EFFECT,			//	��Ļ��Ч
        EQUIP_CAN_INHERIT_ADDONS, 		//	��������5�ɼ̳еĸ�������
		COMBO_SKILL_PREPARE,

		INSTANCE_REENTER_NOTIFY,		//	395 ��������֪ͨ
		PRAY_DISTANCE_CHANGE,			//	�����������
	};

	struct cmd_header
	{
        unsigned short cmd;
	};

	//	object �뿪�ɼ�����
	struct cmd_leave_slice
	{
		int id;
	};

	//	player����λ��
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
		   struct		//����Ĵ��룬ʵ��������info2�Ǳ䳤�Ľṹ�������޷�������֯
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
		   struct		//����Ĵ��룬ʵ��������info3�Ǳ䳤�Ľṹ�������޷�������֯
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
		   struct		//����Ĵ��룬ʵ��������info2/3�Ǳ䳤�Ľṹ�������޷�������֯
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

	//	player �뿪��Ϸ
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
        int iDamage;				//	�����0��ʾû�л���
        int attack_flag;			//	��Ǹù����Ƿ��й����Ż����ͷ����Ż������ػ�����
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
		char cEquipment;	//	The equipment which is mangled, ��λ������ι����ǲ���Ӧ�ñ��ɫ

		int attack_flag;      //��Ǹù����Ƿ��й����Ż����ͷ����Ż������ػ�����
		char speed;                     //�����ٶ� speed * 50 ms
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
		int expire_date;//  ����ʱ��
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
		unsigned char  dir;		//256�Ķ�������ʾ����
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
		int attack_degree;   //�����ȼ�
		int defend_degree;   //�����ȼ�
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

	//	�����Լ��뿪�˶���
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
			int profit_level;	// ����ʱ�伶��
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
		char reason;    //0-���;� 1-���������װ�����
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
		int type;       //	���������
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
		size_t yinpiao;			//  �˿͹���ʱ��Ч����ʾ��Ʊ����Ŀ
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
		unsigned short index;           //	�ڰ����������������
		int type;                       //	��Ʒ������
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
		unsigned char where;            //���ĸ���������0 ��׼��2 ����1 װ��
		unsigned char index;            //��󲿷ַ����ĸ�λ��
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
		unsigned char where;            //���ĸ���������0 ��׼��2 ����1 װ��
		unsigned char index;            //��󲿷ַ����ĸ�λ��
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
		int attack_flag;				//��Ǹù����Ƿ��й����Ż����ͷ����Ż������ػ�����
		char speed;                     //�����ٶ� speed * 50 ms
	};

	struct cmd_be_hurt
	{
		int attacker_id;
		int damage;
		unsigned char flag;		//	1, attacker �����
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
			//	�ṩ��ʼ�������ӻ����г�ȡ�����ӡ��ṹ
			//
			bool bRet(false);
			while (true)
			{
				const byte *p = (const byte *)pDataBuf;
				DWORD s = dwDataSize;

				//	��ȡ���ID
				if (!Extract(id, p, s))
					break;

				unsigned short u(0);

				//	��ȡstate����
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

				//	��ȡstate������������
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

				//	���ɿͻ��˿�ֱ��ʹ�õ� States ����
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
							//	���������ݵĲ�������������
							break;
						}
					}
					//	����������0
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
		char is_accountbox;		// 1 �˺Ųֿ⣬��ʱ��slot_size��Ч��0 ��ɫ�ֿ⣬����ֵ����Ч
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
		int inv_delta;		//	�Լ���Ǯ���仯
		int tra_delta;		//	�ֿ��Ǯ���仯
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
        int iDamage;				//	�����0��ʾû�л���
        int attack_flag;			//	��Ǹù����Ƿ��й����Ż����ͷ����Ż������ػ�����
		unsigned char attack_speed;
		byte section;
	};

	struct cmd_object_skill_attack_result
	{
		int attacker_id;
		int target_id;
		int skill_id;
		int damage;
		int attack_flag;		      //��Ǹù����Ƿ��й����Ż����ͷ����Ż������ػ�����
		char speed;                   //�����ٶ� speed * 50 ms
		byte section;
	};

	struct cmd_host_skill_attacked
	{
		int		idAttacker;
		int     idSkill;
		int		iDamage;
		char	cEquipment;				//	The equipment which is mangled, ��λ������ι����ǲ���Ӧ�ñ��ɫ

		int attack_flag;			    //��Ǹù����Ƿ��й����Ż����ͷ����Ż������ػ�����
		char speed;                     //�����ٶ� speed * 50 ms
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
		int idFaction;				//	���ɵ� id
		unsigned char player_rank;	//	����ڰ����ڵĵȼ�
	};

	struct cmd_task_deliver_item
	{
		int type;
		int	expire_date;
		unsigned int amount;
		unsigned int slot_amount;
		unsigned char where;            //���ĸ���������0 ��׼��2 ����1 װ��
		unsigned char index;            //��󲿷ַ����ĸ�λ��
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
		char name[1];				//	���62�ֽ�

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
			int type;               //	��Ʒ����
			unsigned short index;
			unsigned int count;   //	�����ٸ�
			size_t price;           //	����

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
				int type;		//	��Ʒ����, 0 ��ʾ������ṹ

			} empty_item;

			struct
			{
				int type;       //	��Ʒ����
				int count;		//	> 0
				size_t price;   //	����
				int expire_date;//  ����ʱ��
				unsigned short content_length;
				char content[];

			} sell_item;

			struct
			{
				int type;       //	��Ʒ����
				int count;		//	< 0
				size_t price;   //	����

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
		char name[1];				//	���62�ֽ�

		bool CheckValid(size_t buf_size, size_t& sz) const
		{
			sz = sizeof(*this) - sizeof(name);

			if (buf_size < sz)
				return false;

			sz += name_len;
			return buf_size >= sz;
		}
	};

	struct cmd_player_start_travel	//	�㲥
	{
		int pid;
		unsigned char vehicle;
	};

	struct cmd_host_start_travel
	{
		float speed;				//	�ٶ�����������Ҳ���Դӽ�ͨ���߾�����
		A3DVECTOR3 dest;			//	Ŀ���
		int line_no;				//	���ߺ�
		unsigned char vehicle;		//	��ͨ���ߴ���
	};

	struct cmd_player_end_travel	//	�㲥
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
		 char type;// pk���������¼�
	};

	struct cmd_player_disable_pvp
	{
		 int who;
		 char type; // pk���������¼�
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
		unsigned char is_enable;  //�Ƿ񼤻�
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
		int mode;		//	��0��ʾ˫������״̬,0��ʾ��˫������״̬
		int end_time;	//	����ʱ���
	};

	struct cmd_available_double_exp_time
	{
		int available_time;	//	ʣ��ʱ��,��λ��
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
		int delay;              //������ʱ
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
		int param;	//	ֻ���0 ��ͬ��
	};

	struct cmd_player_bind_invite_reply
	{
		int who;
		int param;	//	ֻ���0��ͬ��
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
		int slot_index;		//	�˳�����ڳ������ڵ�λ��
		info_pet data;		//	pet data
	};

	struct cmd_free_pet
	{
		int slot_index;
		int pet_id;			//	�����id���޴���
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
		PET_RECALL_DEFAULT,			//	Ĭ���ٻأ���Ҫԭ��Ϊ��������ٻء�����ٻ�������ԭ����ԭ�г����ٻص�
		PET_RECALL_DEATH,			//	�������������ٻ���Ϣ
		PET_RECALL_LIFE_EXHAUST,	//	����ʱ�޵�
		PET_RECALL_SACRIFICE,		//	�������������ʹ�ü��ܵȵ��£�
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
		int delay;			//	�ӳ�ʱ�䣬��λ��50ms��tick
		int op;				//	��������  0:�ų� 1:�ٻ� 2:����
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
		int level;			//	�¼���
		int exp;			//	��ǰ�ľ���ֵ 
	};

	struct cmd_pet_room
	{
		unsigned short count;
	//	�ظ� count ��
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
		unsigned char attack;	//	0 ������; 1 ������; 2 ������
		unsigned char move;		//	0 ����; 1 ԭ��ͣ��
	};

	struct cmd_refine_result
	{
		int result;		//	�������  0 �ɹ� 1 ʧ�ܣ�������ʧ�� 2ʧ�ܣ�������ʧ������飰����3ʧ�ܣ����Ϻ�װ����ʧ��
	};

	struct cmd_pet_set_cooldown
	{
		int pet_index;
		int cooldown_index;
		int cooldown_time; 
	};

	struct player_cash //��֪����������Ϸ�������˵������ڰٱ�������Ʒ
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

	struct player_wallow_info  //��ҵ�ǰ�ĳ�����Ϣ, �ս�����Ϸ��ʱ����������������ͣ����Եȼ��ı�ʱҲ�ᷢ�ͣ����Ա�GM������δ������Եȼ��仯��������������������
	{
        unsigned char anti_wallow_active;  //��Ϸ�������˸��ݳ��Եȼ��޸ľ���͵����־�Ƿ��
        unsigned char wallow_level;        //���Եȼ� 0 1 2
		int play_time;
        int light_timestamp;               //�жȳ��Ե�ʱ���
        int heavy_timestamp;               //�ضȳ��Ե�ʱ���
		int	reason;								//	����ԭ��0���֤�ű�ʶ����δ�����ˣ�1δ�����֤�ţ��߿���ע�ᣩ
	};

	struct player_use_item_with_arg          //����Լ�ʹ����Ʒ�������Զ��������ֻ���Լ��յ�
	{
		unsigned char where;            //�������Ʒ
		unsigned char index;            //��Ʒ��λ��
		int item_id;                    //��Ʒ������
		unsigned short use_count;       //ʹ�õĸ���
		unsigned short size;
		char arg[1];                     //��Ʒ���Զ�����
	};

	struct object_use_item_with_arg         //���Լ����յ����Լ�������ʹ����Ʒ 
	{
		int user;
		int item;
		unsigned short size;
		char arg[1];
	};

	struct notify_safe_lock         //��ȫ���Ƿ��
	{
        unsigned char active;
		int time;
		int total_time;
	};

	struct cmd_elf_vigor
	{
		int vigor; 			//Ԫ��
		int max_vigor;		//Ԫ������
		int vigor_gen;		//Ԫ���ظ�
	};

	struct cmd_elf_enhance
	{
		short str_en;		//ʹ��С�����Buff���ܲ�����������ǿ
		short agi_en;
		short vit_en;
		short eng_en;
	};

	struct cmd_elf_stamina
	{
		int stamina;   //С���������
	};

	struct cmd_elf_result
	{   
		int cmd;		//����id
		int result;		//����ִ�н�����Բ�ͬ��������в�ͬ����
		int param1;
		int param2;
	};

	struct common_data_notify   //ȫ��������������Щȫ�����ݽ�����������
	{
		struct _node
		{
			int key;
			int value;
		};
	};

	struct common_data_list     //����ȫ�����ݵ��б�ע������ṹʹ����������������size������ȫ�����ݵĸ���
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
		char status; //1 ���� 0 ȡ������
	};

	struct cmd_cast_elf_skill
	{
		int pid;				//player id
		int target;				//target id
		int skill;
		unsigned char level;
	};

	struct cmd_mall_item_price	// ���յ�GET_MALL_ITEM_PRICEЭ��ʱ���͸��ͻ���
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

	// �������ĳЩԭ���·������̳����ݺͿͻ��˲�һ��
	//������player�ڴ��̳Ǻ�ͣ���̳�ʱ�䳤�����������̳����ݷ����ı䣩
	// �������������player���������������gsͨ������Э��֪ͨ�ͻ��˹���ʧ��
	// ���ǿͻ��˻�ͨ��GET _MALL_ITEM_RECENT _PRICEЭ��ȥ��ȡ��������
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
		int damage_low;			//���damage
		int damage_high;		//�������damage
		int damage_magic_low;	//ħ����͹�����
		int damage_magic_high;	//ħ����߹�����
		int defense;			//������
		int resistance[5];		//ħ������
		int attack;				//������ attack rate
		int armor;				//�����ʣ�װ�׵ȼ���
		int attack_speed;		//����ʱ���� ��tickΪ��λ
		float run_speed;		//�����ٶ� ��λ  m/s
		int attack_degree;		//�����ȼ�
		int defend_degree;		//�����ȼ�
		int crit_rate;			//����
		int damage_reduce;		//�˺�����ٷֱȣ�������
		int prayspeed;			//�����ٶ����Ӱٷֱ�,������
		int crit_damage_bonus;  //���˰ٷֱ�����ֵ
		int invisible_degree;   //�����ȼ�
		int anti_invisible_degree; //�������ȼ�
		int vigour;
		
		int self_damage_reduce;	//�������
		int self_prayspeed;		//�����ٶ�
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
		unsigned char trashbox_index; // ����ֿ⹲������ʱװ�Ϳ��ơ�Ŀǰ������ֻ֧�ֿ��Ʋֿ���װ����֮�����Ʒ�ƶ�
		unsigned char trash_idx;
		unsigned char equip_idx;
	};
	
	struct cmd_object_invisible
	{
		int id;					//������player��npc
		int invisible_degree;	//0 ������ >0 ����ȼ�
	};

	// ��ұ���Ѫʱ�յ���Э��
	struct cmd_player_hp_steal
	{
		int hp;
	};

	struct cmd_player_dividend //��֪�������ĺ��������˺��������ں����̳ǹ�����Ʒ
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
		//�����Ǹ���ְҵ���а��һ�������а�
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
		int last_timestamp;     // �ϴ�ѡ���ײ͵ķ�����ʱ��
		float enhance_factor;   // ���鱶��
	};

	struct cmd_change_multi_exp_state
	{
		char state;					// 0-3����Ӧ���������顢���黺�塢ɢ���״̬
		int enchance_time;      // ����ʣ��ʱ��(s)
		int buffer_time;        // ���黺��ʣ��ʱ��(s)
		int impair_time;        // ɢ��ʣ��ʱ��(s)
		int activate_times_left;	// ���컹ʣ��Ŀ����������
	};

	struct cmd_world_life_time
	{
		int life_time; // >=0:ʣ��ʱ�� -1:����
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
		char reason;	//	0 ������1 ��������2 ������Χ��3 �Ա���4 ��������������
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
		int	consume_contrib;	//	�����ѹ��׶�
		int	exp_contrib;		//	�ɶһ����ɾ��鹱�׶�
		int	cumulate_contrib;	//	�ۻ����׶�
	};

	struct cmd_faction_fortress_info 
	{
		int	faction_id;			//	����id
		int	level;				//	���صȼ�
		int	exp;				//	���ؾ���
		int	exp_today;			//	�����õľ���
		int	exp_today_time;		//	�����Ӧ�����
		int	tech_point;			//	ʣ��Ƽ�����
		int	technology[5];		//	�Ƽ��츳�ĵȼ�
		int	material[8];		//	������ʣ����
		int	building_count;		//	���л�����ʩ��
		struct building_data 
		{
			int	id;				//	������ʩ��Ӧid
			int	finish_time;	//	������ʩ�������ʱ���
		};
		abase::vector<building_data> building;
		int	health;				//	���ؽ�����

		bool Initialize(const void *pDataBuf, DWORD dwDataSize, DWORD *pSizeUsed = NULL)
		{
			//	�ṩ��ʼ�������ӻ����г�ȡ�����ӡ��ṹ
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
		FIDArray alliance;		//	ͬ�˰��� id �б�

		int hostile_count;
		FIDArray hostile;		//	�ж԰��� id �б�
		
		bool Initialize(const void *pDataBuf, DWORD dwDataSize, DWORD *pSizeUsed = NULL)
		{
			//	�ṩ��ʼ�������ӻ����г�ȡ�����ӡ��ṹ
			//
			bool bRet(false);
			while (true)
			{
				const byte *p = (const byte *)pDataBuf;
				DWORD s = dwDataSize;

				//	��ȡͬ�˰��ɸ���
				if (!Extract(alliance_count, p, s) || alliance_count <0)
					break;

				//	��ȡͬ�˰���ID�б�
				alliance.clear();
				if (alliance_count > 0)
				{
					alliance.reserve(alliance_count);
					for (int i = 0; i < alliance_count; ++ i)
						alliance.push_back(0);
					if (!Extract(&alliance[0], alliance_count, p, s))
						break;
				}

				//	��ȡ�ж԰��ɸ���
				if (!Extract(hostile_count, p, s) || hostile_count <0)
					break;

				//	��ȡ�ж԰���ID�б�
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
		__int64 mask;	//��ǰʧЧ��װ��mask
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
		int addon_num;		//	�ɹ���ӵ��Կ�������
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
		char	b;				//	1 ���� 0 ������
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
		char b;	//	1 ���� 0 ������
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
			int activity;		// ���յĻ�Ծ��
			int activity_level; // ��Ծ������
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
		int total_award_time;//�ܽ���ʱ��
		size_t count;
		struct _entry{
			int type; //��������
			int time;//����ʣ��ʱ��
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
		int type; //��������
		char activate;
    };

	enum SERVER_SEND_PROFIT_TIME_REASON
	{
		PLAYER_QUERY,		// �ͻ�������
		PLAYER_ONLINE,		// �������
		PLAYER_SWITCH_SCENE,// �л���ͼ
		PROFIT_LEVEL_CHANGE,// ����״̬�ı�
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
		int	role_in_war;	//	0 ���� 1 �� 2��
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
		FLAG_MSG_GENERATE,  //	��������
		FLAG_MSG_PICKUP,	//	��������
		FLAG_MSG_HANDIN,	//	��������
		FLAG_MSG_LOST,		//	���Ķ�ʧ
	};
	struct cmd_countrybattle_flag_msg_notify
	{
		int		msg;
		char	offense;
	};
	struct cmd_defense_rune_enable
	{
		char	rune_type;	//	0:��������Ż��� 1:���������Ż���
		char	enable;
	};
	
	struct cmd_countrybattle_info
	{
		int		attacker_count;
		int		defender_count;
	};

	struct cmd_set_profit_time
	{
		char enable;	//1: ��ʼ�ۼ�����ʱ�䣬 0:ֹͣ�ۼ�����ʱ��
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
		int stime; //��ʱʱ��
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

	struct cmd_equip_addon_update_notify //֪ͨװ����ʯ���cmd�ṹ
	{
		unsigned char update_type; //0 for change 1 for replace
		unsigned char equip_idx;
		unsigned char equip_socket_idx;
		int old_stone_type;
		int new_stone_type;
	};

	struct cmd_self_king_notify
	{
		char is_king;           // �Ƿ�Ϊ����
		int expire_time;		// ����ʱ��� ������ʱ��
	};
	
	struct cmd_player_king_changed
	{
		int id;
		char is_king;			// �Ƿ�Ϊ����
	};
	struct cmd_notify_meridian_data
	{
		int meridian_level; //�����ȼ�(0-80)
		int lifegate_times; //�Ѿ������㿪��������
		int deathgate_times; //�Ѿ��㿪��������
		int free_refine_times; //ʣ����Ѿ�������
		int paid_refine_times; //ʣ���շѾ�������
		int continu_login_days; //������½����
		int trigrams_map[3];//����ͼ��ÿ2λ��ʾһ���ţ�00Ϊδ�����01Ϊ���ţ�10Ϊ����
	};

	struct cmd_try_refine_meridian_result
	{
		int index; //���������
		int result; //������� 1:���ţ����Ǿ���δ����
					//			2������������
					//				3�����ţ�����δʧ��
					//				4�����ţ���ʧ��
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
		__int64	income; // �ܼƻ�õ���
		__int64 remain; // ��ǰ���õ���
		int retcode;
	};
	
	struct cmd_spend_touch_point
	{
		__int64	income; // �ܼƻ�õ���
		__int64 remain; // ��ǰ���õ���
		unsigned int cost; // �������ѵ���
		unsigned int index; // ��Ʒ����
		unsigned int lots; // �Ƿ�
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
		char	type; // ͬ��ԭ�� 0��ʼ1���ݲ�ͬ��2ǩ��3��ǩ4�콱
		int		monthcalendar;// ��ǰ��ǩ������
		int		curryearstate; // ��ǰ���·�ǩ��״̬
		int		lastyearstate; // ȥ���·�ǩ��״̬
		int		updatetime;  // ǩ���������һ�α��ʱ��  
		int		localtime;   // ��ǰ������ʱ��
		char	awardedtimes;// �������콱����
		char	latesignintimes;//�����Ѳ�ǩ����
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
		char tome_active;   // 1����0δ����
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
			int type; //ȫ���������� |0 δ��ʼ��|1 INT| 2 DOUBLE| 3 �䳤
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

	struct cmd_enter_trickbattle // ����
	{
		int role_in_war;    //
		int battle_id;
		int end_time;
	};
	struct cmd_trickbattle_personal_score // ����
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
		bool in_out; // true:����ս����false:����ս��
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
				// �ط���������
				if (!Extract(count, p, s))
					break;
				if (count > 0) {
					// ��ʼ���ط�����
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
				// �ط���������
				if (!Extract(count, p, s))
					break;
				if (count > 0) {
					// ��ʼ���ط�����
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
				// ������������
				if (!Extract(count, p, s))
					break;
				if (count > 0) {
					// ��ʼ����������
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
				// ������������
				if (!Extract(count, p, s))
					break;
				if (count > 0) {
					// ��ʼ����������
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
		int		config_id;		//	����̳����ñ�ID
		int		op;				//	�Դ�������ñ�Ĳ���
		int		result;			//	������������� RAND_MALL_SHOPPING_RES_OK �ȣ�
		int		item_to_pay;	//	���������ȷ�ϸ������ƷID
		int		price;			//	������/�Ѹ�����Ʒ�۸�Ԫ����
		bool	firstflag;		//	�Ƿ��״ι���
	};
	struct cmd_faction_pvp_mask_modify
	{
		int		roleid;			//	���ܸı�����ID
		unsigned char mask;		//	�µ�״̬Mask
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
			int tid;			// npcģ��ı��
			int nid;			// npc���
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
		int	world_tag;	// ������ͼ ID
		int time_out;	// ��ֹʱ��
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
		GET_ALL_DATA,		//  ȡ���������� ������Ϸʱʹ�ã��������еİ�������Ǯ�ͼ���

		USE_ITEM,			//	40, ʹ��һ����Ʒ
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
		STOP_FALL,			//	��ֹ����
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
		RECHARGE_ONLINE_AWARD, //�����߽���ʱ������
		TOGGLE_ONLINE_AWARD,  //�л����߽�������

		QUERY_PROFIT_TIME,		// ��ѯ��ǰ��������
		ENTER_PK_PROTECTED,		// ����pk������
		COUNTRYBATTLE_GET_PERSONAL_SCORE,	// ��ȡ��ս���˻���
		GET_SERVER_TIMESTAMP,	//	ͬ��GS������ʱ��
		COUNTRYBATTLE_LEAVE,	// �뿪��սս��

		GET_CASH_MONEY_EXCHG_RATE,	// 140
		EVOLUTION_PET,					// �������   
		ADD_PET_EXP,					// ι��
		REBUILD_PET_NATURE,				// �Ը���ѵ
		REBUILD_PET_INHERIT_RATIO,		// ϴ��

		PET_REBUILD_INHERIT_CHOOSE,			// �Ƿ�ѡ�������ԣ�ϴ��   145
		PET_REBUILD_NATURE_CHOOSE,			// �Ƿ�ѡ�����Ը��Ը���ѵ  
		EXCHANGE_WANMEI_YINPIAO,
		PLAYER_GIVE_PRESENT,
		PLAYER_ASK_FOR_PRESENT,

		MERIDIANS_IMPACT, // ������� 150
		COUNTRYBATTLE_GET_STRONGHOLD_STATE,
		QUERY_TOUCH_POINT,
		SPEND_TOUCH_POINT,
		QUERY_TITLE,

		CHANGE_CURR_TITLE, // 155
		DAILY_SIGNIN,	   // ÿ��ǩ��
		LATE_SIGNIN,	   // ��ǩ
		APPLY_SIGNINAWARD, // ��ȡǩ������
		REFRESH_SIGNIN,	   // ˢ��ǩ������

		SWITCH_IN_PARALLEL_WORLD,	//	160 �л�����
		QUERY_PARALLEL_WORLD,		//	��ѯ��ǰ���ڵ�ͼ����
		GET_REINCARNATION_TOME,
		REWRITE_REINCARNATION_TOME,
		ACTIVATE_REINCARNATION_TOME,
		
		QUERY_UNIQUE_DATA,   // 165 , ������ʷ�ƽ� keyֵ����Ӧȫ������
		AUTO_TEAM_SET_GOAL,
		AUTO_TEAM_JUMP_TO_GOAL,
		TRICKBATTLE_LEAVE,   //    �뿪����ս��
		TRICKBATTLE_UPGRADE_CHARIOT, //  ����ս��

		SWALLOW_GENERALCARD,	// 170 ���ɿ��ƻ������������
		EQUIP_TRASHBOX_ITEM,	// ������ֿ�(ʱװ������)װ����Ʒ
		QUERY_TRICKBATTLE_CHARIOTS, // ��ѯս������
		COUNTRYBATTLE_LIVE_SHOW, // ��ѯ��սrank����������
		SEND_MASS_MAIL,			// Ⱥ���ʼ�
		
		RANDOM_MALL_SHOPPING,	//	175 ����̳�����
		QUERY_MAFIA_PVP_INFO,	//	�������PVP����
		QUERY_CAN_INHERIT_ADDONS, // ����ɸѡ�ɼ̳и�����������		
		ACTIVATE_REGION_WAYPOINTS,// ���ȫ�����͵�
		INSTANCE_REENTER_REQUEST,	//	���븱��
		

		NUM_C2SCMD,				//	Number of C2S commands.
		
		//	Below are GM commands
		GM_CMD_START = 200,		//	200
		GM_MOVETO_PLAYER,
		GM_CALLIN_PLAYER,
		GM_KICK_PLAYER,			//	��Ҫѡ��Ŀ��,�޲���
		GM_INVISIBLE,			//	�л���������,�޲���

		GM_INVINCIBLE,			//	205, �л������޵�,�޲���
		GM_GENERATE,
		GM_ACTIVE_SPAWNER,
		GM_GENERATE_MOB,

		//209 ~ 217 obsoleted
        
        GM_QUERY_SPEC_ITEM = 218,	// query player's trashbox detail
        GM_REMOVE_SPEC_ITEM,		// remove items in trashbox
		GM_OPEN_ACTIVITY,			// GM open activity
		GM_CHANGE_DS,				// GM ���
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
		unsigned char dir;			//	����ķ���
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
		int idPlayer;	//	��˭�������� 
	};

	struct cmd_team_agree_invite
	{
		int idLeader;	//	˭���е�����
		int team_seq;
	};

	struct cmd_team_reject_invite
	{
		int idLeader;	//	˭���е�����
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
		unsigned char pvp_mask;		//	ֻ�Թ�������Ʒ��Ч
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
		char is_accountbox;		// 1-�ʺŲֿ�  0-��ɫ�ֿ�
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
		unsigned int inv_money;   //������ȡ����Ǯ��
		unsigned int trashbox_money;  //�Ӳֿ���ȡ��Ǯ��
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
		int param; // 0 ͬ��  ��0���ܾ���ԭ��
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
		int param;  //	0 ͬ��
	};

	struct cmd_bind_player_invite_reply
	{
		int who;
		int param;  //	0 ͬ��
	};

	struct cmd_get_other_equip_detail 
	{
		int target;
	};

	struct cmd_summon_pet
	{
		size_t pet_index;	//	�������ڵ�����
	};

	struct cmd_banish_pet
	{
		size_t pet_index;
	};

	/*	pet_cmd�ķ��ࣺ
		pet_cmd = 1 ������target�������Ŀ�꣬Ҫ��target��Ч
						����Ϊchar: pvp mask ��������״̬
		pet_cmd = 2 ���޸ĳ���ĸ������ target��Ч 
					  ����Ϊint 0 ���������ң�Ĭ��ֵ��
								1 ����ԭ��ͣ��
					  ��������ʱ������ͼ��ֹ���ﵱǰ�Ķ�����ִ���µĸ������
		pet_cmd = 3 �� �޸ĳ���������߼� target ��Ч
					   ����Ϊint 0 ���� �����ͣ����ܵ�����ʱ����з����� �����ܵ�����ʱҲ����з�����Ŀǰδʵ�֣�
								 1 ���� �����ͣ����Զ�������Ұ��Χ�ڵĵж�Ŀ��
								 2 ���� �����ͣ�������ҷ���ָ����򲻻�����Ӧ��
		pet_cmd = 4,  Ҫ�����ʹ�ü���  target����ʹ�ü��ܵ�Ŀ��
						����Ϊ int����ʾҪ�����ʹ�õļ��ܺ� 
						����Ϊ char: pvp mask ��������״̬
		pet_cmd = 5, Ҫ������Զ�ʹ�ü���ʹ�ü���  target��Ч
					����Ϊ int ����ʾҪ�����ʹ�õļ��ܺ� ����ü���Ϊ0�����Զ�ʹ��						
	*/
	struct cmd_pet_ctrl
	{
		int target;		//	������Ŀ�꣬�������ҪĿ�ָ꣬�����0
		int pet_cmd;	//	�Գ���Ŀ���ָ��
	//	char buf[];		//	�������ָ��Ĳ���
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

	struct use_item_with_arg      //������ʹ����Ʒ
	{
        unsigned char where;  //��Ʒ������
        unsigned char count;  //ʹ�ü���
        unsigned short index; //��Ʒ��λ��
        int  item_id;         //��Ʒ������
        // char arg[1];           //��Ʒ���Զ����������
	};

	struct cmd_elf_add_attribute
	{   
		short str; //��������ֵ��>= 0
		short agi;
		short vit;
		short eng;
	};  

	struct cmd_elf_add_genius
	{   
		short genius[5];  //�츳����ֵ, >=0
	};

	struct cmd_elf_player_insert_exp
	{   
		unsigned int exp;    //���׼����С����ע��ľ���ֵ��use_spΪ1��ΪԪ����,�޶�Ϊ8.4e
		char use_sp;		//Ϊ1��ʹ��Ԫ��Ϊ0ʹ�þ���
	};

	struct cmd_elf_equip_item
	{
		unsigned char index_inv;		//С�����װ���ڰ����е�����
	};  

	struct cmd_elf_change_secure_status
	{
		unsigned char status;   //0-��С��������Ϊ��ȫ״̬ 1-����Ϊת��״̬
	};

	struct cmd_cast_elf_skill
	{
		unsigned short skill_id;
//		short skill_level;
		unsigned char force_attack;
		unsigned char target_count;
		int targets[1];
	};

	struct recharge_equipped_elf    //ʵ����ɽ���ȫ��ͬ��������Ų�ͬ
	{
		unsigned char element_index;
		int count;
	};

	//����player��ȡ�̳Ǹ�������,��playerÿ�δ��̳�ʱʹ��
	//���ڸ�Э�����gsɨ��mall������Ʒ�����Կͻ��������Э�鷢���ٶ�
	struct cmd_get_mall_item_price
	{
		short start_index;	// ����������Ϊ0 ��ɨ��������
		short end_index;	// ����[start,end)����ɨ��		
	};

	//  ����ʱװ�ֿ����ʱװ���Զ���װ���ܣ�
	struct cmd_equip_fashion_item
	{
		unsigned char idx_body;		// �·�
		unsigned char idx_leg;		// ����
		unsigned char idx_foot;		// Ь
		unsigned char idx_wrist;	// ����	
		unsigned char idx_head;		// ͷ��
		unsigned char idx_weapon;	// ����
	};

	//  ��һ�δ�ʱװ����ʱ��������
	struct cmd_check_security_passwd
	{
		size_t passwd_size;
	};

	//  ����PK���ø�������
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
		char remain;	// 0����ѡ����װ����1����ѡ���װ��
	};

	
	struct cmd_recharge_online_award
	{
		int type;   //��������
		int count;
		struct entry
		{
			int type;   //��Ʒid
			int index; //��Ʒλ��
			int count;  //��Ʒ����
		}entry_list[1];
    };

	struct cmd_toggle_online_award
	{
		int type; //��������
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
		char type;    // 0 �첹ǩ1�²�ǩ2�겹ǩ
		int itempos;   // ������Ʒλ��
		int desttime;   // ��ǩʱ��� ���µ�һδǩ����|�µ�һ��|���һ��
	};
	struct cmd_apply_signinaward
	{
		char type;  // 0x1�¶�0x2���0x4ȫ��(��ͬʱ��ȡ����ȡǰ���жϱ���)
		int mon;    // �·� ��0x1ʱ��Ч
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
		int sp_id;		//	������IDֵ
	};

	struct gm_cmd_generate_mob
	{
		int mob_id;			//	�������ֹ���
		int vis_id;			//	�������ֹ��￴����ʲô����0����Ч
		short count;		//	���ɵ�����
		short life;			//	���ɹ���������ڣ�0Ϊ����
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
		int index; //�ŵ����
    };
	struct cmd_player_give_present
	{
		int roleid;                 //��������roleid
		int mail_id;                //��ȡ��Ʒ�ʼ���������û�еĻ�Ϊ-1
		int goods_id;               //������Ʒ��id
		int goods_index;            //������Ʒ���̳��е�����
		int goods_slot;             //������Ʒ��������Ϣ����
	};
	struct cmd_player_ask_for_present
	{       
		int roleid;                 //��ȡ�����roleid
		int goods_id;               //��ȡ��Ʒ��id
		int goods_index;            //��ȡ��Ʒ���̳��е�����
		int goods_slot;             //��ȡ��Ʒ��������Ϣ����
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
		int param;		//���Ӳ���  ����ս��ս����˵��chariotid����������������
	};

	struct cmd_trickbattle_upgrade_chariot
	{
		int chariot;
	};
	struct cmd_swallow_generalcard
	{
		unsigned char equip_index;				// װ���Ͽ��Ƶ�����
		unsigned char is_inv;					// 1-����, 0-���Ʋֿ�
		unsigned char swallowed_equip_index;	// �����ɵİ������Ͽ���/���������������
		unsigned int count;						// �����ɵİ������Ͽ���/���������������
	};
	struct cmd_equip_trashbox_item
	{
		unsigned char trash_box_index;			// // ����ֿ⹲������ʱװ�Ϳ��ơ�Ŀǰ������ֻ֧�ֿ��Ʋֿ���װ����֮�����Ʒ�ƶ�
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
		int config_id;	//	������ñ�ID
		int	op;			//	�Դ�������ñ�Ĳ���
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

