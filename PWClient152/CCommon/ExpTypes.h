#ifndef _EXP_TYPES_H_
#define _EXP_TYPES_H_

#include <stdio.h>
#include <stdlib.h>

#if defined(_WINDOWS) || defined(WIN32) || defined(_WIN32)
	typedef	unsigned __int64 UINT64;
	typedef __int64	INT64;
#else
	#include <stdint.h>
	typedef uint64_t UINT64;
	typedef int64_t	INT64;
#endif

#define ELEMENTDATA_VERSION		0x3000007f

#define ELEMENTDATA_NUM_PROFESSION 12	//	职业数目
#define ELEMENTDATA_NUM_POKER_TYPE	6	//	卡牌种类个数
#define ELEMENTDATA_NUM_POKER_RANK	5	//	卡牌品阶个数
#define	ELEMENTDATA_MAX_POKER_LEVEL	100	//	卡牌最高等级
#define ELEMENTDATA_MAX_UNIVERSAL_TOKEN_USAGE 32	//	通用客户端功能物品可使用方式的种类
#define ELEMENTDATA_MAX_ENGRAVE_ADDON_COUNT 3		//	镌刻产生最多属性条数
#define ELEMENTDATA_MAX_INHERIT_ADDON_COUNT 5		//	附加属性继承最多属性条数
enum
{
	ELEMENTDATA_DOMAIN_COUNT = 52,	//	城战领土数
};

enum SERVICE_TYPE
{
	// 交谈服务
	NPC_TALK = 0x80000000,
	// 出售商品的服务
	NPC_SELL,
	// 收购商品的服务
	NPC_BUY,
	// 修理商品的服务
	NPC_REPAIR,
	// 镶嵌服务
	NPC_INSTALL,
	// 拆除服务
	NPC_UNINSTALL,
	// 任务相关服务,分发任务和完成任务以及发放任务物品
	NPC_GIVE_TASK,
	NPC_COMPLETE_TASK,
	NPC_GIVE_TASK_MATTER,
	// 教授相关服务
	NPC_SKILL,
	// 治疗服务
	NPC_HEAL,
	// 传送服务
	NPC_TRANSMIT,
	// 运输服务
	NPC_TRANSPORT,
	// 代售服务
	NPC_PROXY,
	// 存储物品、金钱
	NPC_STORAGE,
	// 生产服务
	NPC_MAKE,
	// 分解服务
	NPC_DECOMPOSE,
	// TALK返回
	TALK_RETURN,
	// 结束对话
	TALK_EXIT,
	// 仓库密码
	NPC_STORAGE_PASSWORD,
	// 鉴定服务
	NPC_IDENTIFY,
	// 放弃任务
	TALK_GIVEUP_TASK,
	// 城战炮塔建造服务
	NPC_WAR_TOWERBUILD,
	// 洗点服务
	NPC_RESETPROP,
	// 宠物改名服务
	NPC_PETNAME,
	// 宠物学习技能服务
	NPC_PETLEARNSKILL,
	// 宠物遗忘技能服务
	NPC_PETFORGETSKILL,
	// 装备绑定服务
	NPC_EQUIPBIND,
	// 装备销毁服务
	NPC_EQUIPDESTROY,
	// 装备解除销毁服务
	NPC_EQUIPUNDESTROY,
	// 帐号仓库
	NPC_ACCOUNT_STORAGE,
	// 镌刻服务
	NPC_ENGRAVE,
	// 装备重铸（随机属性）
	NPC_RANDPROP,
};

#define		NUM_WEAPON_TYPE		15

#pragma pack(push, EXP_TYPES_INC, 4)

#ifdef _WINDOWS
typedef wchar_t namechar;
#else
typedef unsigned short namechar;
#endif

///////////////////////////////////////////////////////////////////////////////////////
//
// 各种装备的模板中要使用的附加属性的数据结构定义
//
//		注意：装备包括武器、护具、饰品等几大类
//
//
///////////////////////////////////////////////////////////////////////////////////////
struct EQUIPMENT_ADDON
{
	unsigned int	id;							// 附加属性的ID，在生成物品时要求第14、13位表示本
												// 附加属性的参数个数，第12位不能占用
												// 所以我们在产生ID时，不能使用12，13，14这三位

	namechar		name[32];					// 附加属性的名字

	int				num_params;					// 本附加属性的参数个数
	int				param1;						// 第1个参数，可以是浮点数
	int				param2;						// 第2个参数，可以是浮点数
	int				param3;						// 第3个参数，可以是浮点数

};


///////////////////////////////////////////////////////////////////////////////////////
//
// 武器模板数据结构定义
//
//
///////////////////////////////////////////////////////////////////////////////////////

// 武器大类别
struct WEAPON_MAJOR_TYPE
{
	unsigned int	id;							// 武器大类别ID
	namechar		name[32];					// 武器大类别名称
};

// 武器小类别
struct WEAPON_SUB_TYPE
{
	unsigned int	id;							// 武器小类别ID
	namechar		name[32];					// 武器小类别名称
	
	char			file_hitgfx[128];			// 击中效果名字
	char			file_hitsfx[128];			// 击中声音效果名字

	float			probability_fastest;		// 攻击频率分布的概率: 极快	-0.1
	float			probability_fast;			// 攻击频率分布的概率: 快	-0.05
	float			probability_normal;			// 攻击频率分布的概率: 普通	0
	float			probability_slow;			// 攻击频率分布的概率: 慢	+0.05
	float			probability_slowest;		// 攻击频率分布的概率: 极慢	+0.1

	float			attack_speed;				// 攻击速度（攻击累加时间），单位为秒
	float			attack_short_range;			// 该小类武器的最小攻击距离，非弓弩类武器为0.0

	// 武器没有显式的逻辑装备Mask, 因为武器只能装在武器槽上

	unsigned int	action_type;				// 该武器播放动作类别
												// 单手短、双手短、双手长、双手短重、双手长重、软鞭、弓、弩、拳套、枪
};

// 武器本体数据
struct WEAPON_ESSENCE
{
	// 基本信息
	unsigned int	id;							// 武器(类型)ID
	unsigned int	id_major_type;				// 武器大类别ID
	unsigned int	id_sub_type;				// 武器小类别ID
	namechar		name[32];					// 武器名称, 最多15个汉字

	unsigned int	require_projectile;			// 需要的弹药类型(从弹药类型中选取), 当武器需要的弹药类型和弹药上的类型一致时可以使用

	// 显示信息
	char			file_model_right[128];		// 右手模型路径名
	char			file_model_left[128];		// 左手模型路径名
	char			file_matter[128];			// 掉在地上的模型路径名
	char			file_icon[128];				// 图标路径名

	// 装配规则
	int				require_strength;			// 力量限制
	int				require_agility;			// 敏捷限制
	int				require_energy;				// 灵力限制
	int				require_tili;				// 体力限制

	unsigned int	character_combo_id;			// 职业限制组合, 从低位到高位的含义为：0-武侠, 1-法师, 2-巫师, 3-妖精, 4-妖兽, 5-刺客, 6-羽芒, 7-羽灵, 8-剑灵, 9-魅灵

	int				require_level;				// 等级限制
	int				require_reputation;			// 声望限制

	// 本体属性
	int				level;						// 自身的等级

	int				fixed_props;				// 生成固定属性类型(0~3)

	int				damage_low;					// 物理攻击力最小值
	int				damage_high_min;			// 物理攻击力最大值的区间最小值
	int				damage_high_max;			// 物理攻击力最大值的区间最大值
				
	int				magic_damage_low;			// 法术攻击力最小值
	int				magic_damage_high_min;		// 法术攻击力最大值的区间最小值
	int				magic_damage_high_max;		// 法术攻击力最大值的区间最大值
				
	float			attack_range;				// 攻击距离
	unsigned int	short_range_mode;			// 物理攻击近远程标志, 0-远程 ,1-近程, 2-刺客近程

	int				durability_min;				// 耐久度最小值
	int				durability_max;				// 耐久度最大值

	int				levelup_addon;				// 升级效果
	int				material_need;				// 升级所需的幻仙石数目

	// 价钱
	int				price;						// 卖店价
	int				shop_price;					// 店卖价

	int				repairfee;					// 修理价格

	// 掉落眼
	float			drop_probability_socket0;	// 掉落时出现0洞的机会
	float			drop_probability_socket1;	// 掉落时出现1洞的机会
	float			drop_probability_socket2;	// 掉落时出现2洞的机会

	// 生产眼
	float			make_probability_socket0;	// 生产时出现0洞的机会
	float			make_probability_socket1;	// 生产时出现1洞的机会
	float			make_probability_socket2;	// 生产时出现2洞的机会
	
	// 附加属性
	float			probability_addon_num0;		// 附加属性出现0个的机会
	float			probability_addon_num1;		// 附加属性出现1个的机会
	float			probability_addon_num2;		// 附加属性出现2个的机会
	float			probability_addon_num3;		// 附加属性出现3个的机会
	float			probability_addon_num4;		// 附加属性出现4个的机会
	float			probability_addon_num5;		// 附加属性出现5个的机会
	float			probability_unique;			// 产生唯一属性的概率
	
	struct
	{
		unsigned int	id_addon;				// 附加属性的类型ID
		float			probability_addon;		// 附加属性出现的概率
	} addons[32];

	struct
	{
		unsigned int	id_rand;				// 随机属性的类型ID
		float			probability_rand;		// 随机属性出现的概率
	} rands[32];

	struct 
	{
		unsigned int	id_unique;				// 唯一属性的类型ID
		float			probability_unique;		// 唯一属性出现的概率
	} uniques[16];

	int				durability_drop_min;		// 掉落时的耐久度最小值
	int				durability_drop_max;		// 掉落时的耐久度最小值

	// 拆分信息
	int				decompose_price;			// 拆分价格
	int				decompose_time;				// 拆分时间
	unsigned int	element_id;					// 拆分成的元石的类型ID
	int				element_num;				// 拆分产生的元石数
	
	unsigned int	id_drop_after_damaged;		// 损毁后掉落的物品
	int				num_drop_after_damaged;		// 损毁后掉落的物品数量

	// 堆叠信息
	int				pile_num_max;				// 该武器的堆叠上限
	// GUID信息
	unsigned int	has_guid;					// 是否为其产生全局唯一ID，以用于Trace。0-否，1-是
	// 处理方式
	unsigned int	proc_type;					// 是以下几种方式的组合: 死亡时是否掉落，是否可以扔在地上，是否可以卖给NPC，是人民币物品，是否可以玩家间交易，是否任务相关物品，装备后绑定，是否可解绑，离开场景消失
};				


///////////////////////////////////////////////////////////////////////////////////////
//
// 护具模板数据结构定义
//
//		我们把护具译为Armor，同时护具中的躲闪度按照Diablo的习惯用法也以armor称呼。
//
//
///////////////////////////////////////////////////////////////////////////////////////

// 护具大类别
struct ARMOR_MAJOR_TYPE
{
	unsigned int	id;							// 护具大类别ID
	namechar		name[32];					// 护具大类别名称
};

// 护具小类别
struct ARMOR_SUB_TYPE
{
	unsigned int	id;							// 护具小类别ID
	namechar		name[32];					// 护具小类别名称

	unsigned int	equip_mask;					// 该装备逻辑上可以装备的部位，由不同部位Mask组成，使用的部位代码为：
												// EQUIP_MASK_WEAPON       = 0x0001,
												// EQUIP_MASK_HEAD         = 0x0002,
												// EQUIP_MASK_NECK         = 0x0004,
												// EQUIP_MASK_SHOULDER     = 0x0008,
												// EQUIP_MASK_BODY         = 0x0010,
												// EQUIP_MASK_WAIST        = 0x0020,
												// EQUIP_MASK_LEG          = 0x0040,
												// EQUIP_MASK_FOOT         = 0x0080,
												// EQUIP_MASK_WRIST        = 0x0100,
												// EQUIP_MASK_FINGER1      = 0x0200,
												// EQUIP_MASK_FINGER2      = 0x0400,
												// EQUIP_MASK_PROJECTILE   = 0x0800,
};

// 护具本体数据
struct ARMOR_ESSENCE
{
	// 基本信息
	unsigned int	id;							// 护具(类型)ID
	unsigned int	id_major_type;				// 护具大类别ID
	unsigned int	id_sub_type;				// 护具小类别ID
	namechar		name[32];					// 护具名称, 最多15个汉字

	char			realname[32];				// 真实装备名

	// 显示信息
	// 在模板中不再定义护具的模型是因为这些护具会为每个形象都作一个不同的模型,
	// 该模型的命名规则是固定的:形象名+装备名.ski
	char			file_matter[128];			// 掉在地上的模型路径名
	char			file_icon[128];				// 图标路径名

	unsigned int	equip_location;				// 该护具模型覆盖的部位，由不同部位Mask组成，可能的部位有：上身、护腕、下身、鞋四个部分

	int				level;						// 自身等级

	// 装配规则
	int				require_strength;			// 力量限制
	int				require_agility;			// 敏捷限制
	int				require_energy;				// 灵力限制
	int				require_tili;				// 体力限制

	unsigned int	character_combo_id;			// 职业限制组合, 从低位到高位的含义为：0-武侠, 1-法师, 2-巫师, 3-妖精, 4-妖兽, 5-刺客, 6-羽芒, 7-羽灵, 8-剑灵, 9-魅灵

	int				require_level;				// 等级限制
	int				require_reputation;			// 声望限制

	int				fixed_props;				// 生成固定属性类型(0~3)

	// 本体属性
	int				defence_low;				// 物理防御力最小值
	int				defence_high;				// 物理防御力最大值
	
	struct {
		int			low;						// 法术防御力最小值
		int			high;						// 法术防御力最大值
	} magic_defences[5];						// 共5个是因为分金木水火土五系

	int				mp_enhance_low;				// 加MP的最小值
	int				mp_enhance_high;			// 加MP的最大值

	int				hp_enhance_low;				// 加HP的最小值
	int				hp_enhance_high;			// 加HP的最大值

	int				armor_enhance_low;			// 加躲闪度的最小值
	int				armor_enhance_high;			// 加躲闪度的最大值

	int				durability_min;				// 耐久度最小值
	int				durability_max;				// 耐久度最大值

	int				levelup_addon;				// 升级效果
	int				material_need;				// 升级所需的幻仙石数目

	// 价钱
	int				price;						// 卖店价
	int				shop_price;					// 店卖价

	int				repairfee;					// 修理价格

	// 掉落眼
	float			drop_probability_socket0;	// 掉落时出现0洞的机会
	float			drop_probability_socket1;	// 掉落时出现1洞的机会
	float			drop_probability_socket2;	// 掉落时出现2洞的机会
	float			drop_probability_socket3;	// 掉落时出现3洞的机会
	float			drop_probability_socket4;	// 掉落时出现4洞的机会

	// 生产眼
	float			make_probability_socket0;	// 生产时出现0洞的机会
	float			make_probability_socket1;	// 生产时出现1洞的机会
	float			make_probability_socket2;	// 生产时出现2洞的机会
	float			make_probability_socket3;	// 生产时出现3洞的机会
	float			make_probability_socket4;	// 生产时出现4洞的机会
	
	// 附加属性
	float			probability_addon_num0;		// 附加属性出现0个的机会
	float			probability_addon_num1;		// 附加属性出现1个的机会
	float			probability_addon_num2;		// 附加属性出现2个的机会
	float			probability_addon_num3;		// 附加属性出现3个的机会
	float			probability_addon_num4;		// 附加属性出现4个的机会
	
	struct
	{
		unsigned int	id_addon;				// 附加属性的类型ID
		float			probability_addon;		// 附加属性出现的概率
	} addons[32];

	struct
	{
		unsigned int	id_rand;				// 随机属性的类型ID
		float			probability_rand;		// 随机属性出现的概率
	} rands[32];

	int				durability_drop_min;		// 掉落时的耐久度最小值
	int				durability_drop_max;		// 掉落时的耐久度最小值
	
	// 拆分信息
	int				decompose_price;			// 拆分价格
	int				decompose_time;				// 拆分时间
	unsigned int	element_id;					// 拆分成的元石的类型ID
	int				element_num;				// 拆分产生的元石数
			   
	unsigned int	id_drop_after_damaged;		// 损毁后掉落的物品
	int				num_drop_after_damaged;		// 损毁后掉落的物品数量
	
	unsigned int	id_hair;					// 头发模型ID，仅当头盔时有效
	unsigned int	id_hair_texture;			// 头发纹理ID，仅当头盔时有效
	
	// 堆叠信息
	int				pile_num_max;				// 该护具的堆叠上限
	// GUID信息
	unsigned int	has_guid;					// 是否为其产生全局唯一ID，以用于Trace。0-否，1-是
	// 处理方式
	unsigned int	proc_type;					// 是以下几种方式的组合: 死亡时是否掉落，是否可以扔在地上，是否可以卖给NPC，是人民币物品，是否可以玩家间交易，是否任务相关物品，装备后绑定，是否可解绑，离开场景消失
	
	unsigned int	force_all_magic_defences;	//	强制生成所有五行防御
};


///////////////////////////////////////////////////////////////////////////////////////
//
// 饰品模板数据结构定义
//
//
///////////////////////////////////////////////////////////////////////////////////////

// 饰品大类别
struct DECORATION_MAJOR_TYPE
{
	unsigned int	id;							// 饰品大类别ID
	namechar		name[32];					// 饰品大类别名称
};

// 饰品小类别
struct DECORATION_SUB_TYPE
{
	unsigned int	id;							// 饰品小类别ID
	namechar		name[32];					// 饰品小类别名称

	unsigned int	equip_mask;					// 该装备逻辑上可以装备的部位，由不同部位Mask组成
};

// 饰品本体数据
struct DECORATION_ESSENCE
{
	// 基本信息
	unsigned int	id;							// 饰品(类型)ID
	unsigned int	id_major_type;				// 饰品大类别ID
	unsigned int	id_sub_type;				// 饰品小类别ID
	namechar		name[32];					// 饰品名称, 最多15个汉字

	// 显示信息
	char			file_model[128];			// 模型路径名
	char			file_matter[128];			// 掉在地上的模型路径名
	char			file_icon[128];				// 图标路径名

	int				level;						// 自身等级

	// 装配规则
	int				require_strength;			// 力量限制
	int				require_agility;			// 敏捷限制
	int				require_energy;				// 灵力限制
	int				require_tili;				// 体力限制
	
	unsigned int	character_combo_id;			// 职业限制组合, 从低位到高位的含义为：0-武侠, 1-法师, 2-巫师, 3-妖精, 4-妖兽, 5-刺客, 6-羽芒, 7-羽灵, 8-剑灵, 9-魅灵

	int				require_level;				// 等级限制
	int				require_reputation;			// 声望限制

	int				fixed_props;				// 生成固定属性类型(0~3)

	// 本体属性
	int				damage_low;					// 物理攻击力最小值
	int				damage_high;				// 物理攻击力最大值
				
	int				magic_damage_low;			// 法术攻击力最小值
	int				magic_damage_high;			// 法术攻击力最大值

	int				defence_low;				// 物理防御力最小值
	int				defence_high;				// 物理防御力最大值
				
	struct {
		int			low;						// 法术防御力最小值
		int			high;						// 法术防御力最大值
	} magic_defences[5];						// 共5个是因为分金木水火土五系

	int				armor_enhance_low;			// 加躲闪度的最小值
	int				armor_enhance_high;			// 加躲闪度的最大值

	int				durability_min;				// 耐久度最小值
	int				durability_max;				// 耐久度最大值

	int				levelup_addon;				// 升级效果
	int				material_need;				// 升级所需的幻仙石数目

	// 价钱
	int				price;						// 卖店价
	int				shop_price;					// 店卖价

	int				repairfee;					// 修理价格

	// 附加属性
	float			probability_addon_num0;		// 附加属性出现0个的机会
	float			probability_addon_num1;		// 附加属性出现1个的机会
	float			probability_addon_num2;		// 附加属性出现2个的机会
	float			probability_addon_num3;		// 附加属性出现3个的机会
	float			probability_addon_num4;		// 附加属性出现4个的机会
	
	struct
	{
		unsigned int	id_addon;				// 附加属性的类型ID
		float			probability_addon;		// 附加属性出现的概率
	} addons[32];

	struct
	{
		unsigned int	id_rand;				// 随机属性的类型ID
		float			probability_rand;		// 随机属性出现的概率
	} rands[32];

	int				durability_drop_min;		// 掉落时的耐久度最小值
	int				durability_drop_max;		// 掉落时的耐久度最小值

	// 拆分信息
	int				decompose_price;			// 拆分价格
	int				decompose_time;				// 拆分时间
	unsigned int	element_id;					// 拆分成的元石的类型ID
	int				element_num;				// 拆分产生的元石数

	unsigned int	id_drop_after_damaged;		// 损毁后掉落的物品
	int				num_drop_after_damaged;		// 损毁后掉落的物品数量
	
	// 堆叠信息
	int				pile_num_max;				// 该饰品的堆叠上限
	// GUID信息
	unsigned int	has_guid;					// 是否为其产生全局唯一ID，以用于Trace。0-否，1-是
	// 处理方式
	unsigned int	proc_type;					// 是以下几种方式的组合: 死亡时是否掉落，是否可以扔在地上，是否可以卖给NPC，是人民币物品，是否可以玩家间交易，是否任务相关物品，装备后绑定，是否可解绑，离开场景消失
};


/////////////////////////////////////////////////////////////////////////////////////////////////
// 套装，共客户端显示时查询数据用
/////////////////////////////////////////////////////////////////////////////////////////////////

// 套装本体
struct SUITE_ESSENCE
{
	// 基本信息
	unsigned int	id;							// 套装(类型)ID
	namechar		name[32];					// 套装名称, 最多15个汉字

	int				max_equips;					// 套装件数

	struct 
	{
		unsigned int	id;						// 套装组件1~12
	} equipments[12];

	struct 
	{
		unsigned int	id;						// 2件至12件能激活的附加属性
	} addons[11];

	char			file_gfx[128];				// 满套后的效果
};

/////////////////////////////////////////////////////////////////////////////////////////////////
// 卡牌套装
/////////////////////////////////////////////////////////////////////////////////////////////////

// 套装本体
struct POKER_SUITE_ESSENCE
{
	// 基本信息
	unsigned int	id;							// 套装(类型)ID
	namechar		name[32];					// 套装名称, 最多15个汉字
	
	float			promote_ratio;				// 激活后基本属性提升比例
	unsigned int    list[6];					// 套装组件
};


///////////////////////////////////////////////////////////////////////////////////////
//
// 卡牌随机发生器
//
///////////////////////////////////////////////////////////////////////////////////////
struct POKER_DICE_ESSENCE
{
	unsigned int	id;							// (类型)ID
	namechar		name[32];					// 名称, 最多15个汉字
	
	char			file_matter[128];			// 掉在地上的模型路径名
	char			file_icon[128];				// 图标路径名

	int				swallow_exp;				// 吞噬后提供经验值
	
	struct
	{
		unsigned int	id;						// 卡牌id
		float			probability;			// 产生概率		
	}list[256];
	
	int				price;						// 卖店价
	int				shop_price;					// 店卖价
	int				pile_num_max;				// 堆叠上限
	unsigned int	has_guid;					// 是否为其产生全局唯一ID，以用于Trace。0-否，1-是
	unsigned int	proc_type;					// 处理方式
};


///////////////////////////////////////////////////////////////////////////////////////
//
// 卡牌小类别
//
///////////////////////////////////////////////////////////////////////////////////////
struct POKER_SUB_TYPE
{
	unsigned int	id;							// (类型)ID
	namechar		name[32];					// 名称, 最多15个汉字

	int				type;						// 类型(type=poker_type_type)

	union
	{
		struct  
		{
			unsigned int equip_mask1;			// 产生作用的逻辑部位1(type=equip_mask)
			unsigned int equip_mask2;			// 产生作用的逻辑部位2(type=equip_mask2)
		};
		UINT64			equip_mask;
	};
};

///////////////////////////////////////////////////////////////////////////////////////
//
// 卡牌本体
//
///////////////////////////////////////////////////////////////////////////////////////
struct POKER_ESSENCE
{
	unsigned int	id;							// (类型)ID
	unsigned int	id_sub_type;				// 小类别ID
	namechar		name[32];					// 名称, 最多15个汉字
	
	char			file_matter[128];			// 掉在地上的模型路径名
	char			file_icon[128];				// 图标路径名	
	char			file_img[128];				// 展示资源路径
	char			file_gfx[128];				// 光效路径

	int				require_level;				// 玩家等级要求（最高历史等级）
	int				require_control_point[2];	// 统御力要求

	int				rank;						// 品阶(type=poker_rank)
	int				max_level;					// 等级上限
	
	int				hp;							// 初始HP
	int				damage;						// 初始物攻
	int				magic_damage;				// 初始法攻
	int				defence;					// 初始物防
	int				magic_defence[5];			// 初始法防
	int				vigour;						// 初始气魄

	int				inc_hp;						// 每级增加HP
	int				inc_damage;					// 每级增加物攻
	int				inc_magic_damage;			// 每级增加法攻
	int				inc_defence;				// 每级增加物防
	int				inc_magic_defence[5];		// 每级增加法防
	int				inc_vigour;					// 每级增加气魄
	
	int				swallow_exp;				// 吞噬后提供经验值	

	int				addon[4];					// 附加属性

	int				show_order;					// 展示顺序
	
	int				price;						// 卖店价
	int				shop_price;					// 店卖价
	int				pile_num_max;				// 堆叠上限
	unsigned int	has_guid;					// 是否为其产生全局唯一ID，以用于Trace。0-否，1-是
	unsigned int	proc_type;					// 处理方式
};

///////////////////////////////////////////////////////////////////////////////////////
//
// 时装模板数据结构定义
//
//
///////////////////////////////////////////////////////////////////////////////////////

// 时装大类别
struct FASHION_MAJOR_TYPE
{
	unsigned int	id;							// 时装大类别ID
	namechar		name[32];					// 时装大类别名称
};

// 时装小类别
struct FASHION_SUB_TYPE
{
	unsigned int	id;							// 时装小类别ID
	namechar		name[32];					// 时装小类别名称

	unsigned int	equip_fashion_mask;			// 该装备逻辑上可以装备的部位，由不同部位Mask组成，与防具不同，使用的部位代码为：
												// EQUIP_MASK_FASHION_BODY = 0x00002000,
												// EQUIP_MASK_FASHION_LEG  = 0x00004000,
												// EQUIP_MASK_FASHION_FOOT = 0x00008000,
												// EQUIP_MASK_FASHION_WRIST= 0x00010000,
												// EQUIP_MASK_FASHION_HEAD = 0x02000000,
												// EQUIP_MASK_FASHION_WEAPON = 0x20000000,
};

// 时装本体数据
struct FASHION_ESSENCE
{
	// 基本信息
	unsigned int	id;							// 时装(类型)ID
	unsigned int	id_major_type;				// 时装大类别ID
	unsigned int	id_sub_type;				// 时装小类别ID
	namechar		name[32];					// 时装名称, 最多15个汉字

	char			realname[32];				// 真实时装名

	// 显示信息
	// 在模板中不再定义护具的模型是因为这些护具会为每个形象都作一个不同的模型,
	// 该模型的命名规则是固定的:形象名+装备名.ski
	char			file_matter[128];			// 掉在地上的模型路径名
	char			file_icon[128];				// 图标路径名

	unsigned int	equip_location;				// 该护具模型覆盖的部位，由不同部位Mask组成，可能的部位有：上身、护腕、下身、鞋四个部分

	int				level;						// 自身等级

	// 装配规则
	int				require_level;				// 等级限制

	// 染色
	int				require_dye_count;			// 染色所需的染色剂数目
	
	// 价钱
	int				price;						// 卖店价
	int				shop_price;					// 店卖价

	// 性别
	int				gender;						// 能使用的人的性别: 0-男，1-女

	unsigned int	id_hair;					// 头发模型ID，仅当头饰时有效
	unsigned int	id_hair_texture;			// 头发纹理ID，仅当头饰时有效

	// 堆叠信息
	int				pile_num_max;				// 该护具的堆叠上限
	// GUID信息
	unsigned int	has_guid;					// 是否为其产生全局唯一ID，以用于Trace。0-否，1-是
	// 处理方式
	unsigned int	proc_type;					// 是以下几种方式的组合: 死亡时是否掉落，是否可以扔在地上，是否可以卖给NPC，是人民币物品，是否可以玩家间交易，是否任务相关物品，装备后绑定，是否可解绑，离开场景消失

	char			wear_action[32];			// 时装动作名称
	
	char			file_model_right[128];		// 时装武器右手模型路径名
	char			file_model_left[128];		// 时装武器左手模型路径名
	unsigned int	character_combo_id;			// 时装武器职业限制
	unsigned int	action_type;				// 时装武器播放动作类别

	unsigned int	combined_switch;			// 各种组合开关（见 FASHION_COMBINED_SWITCH ）

	float			h_min;						// 色调最小值
	float			h_max;						// 色调最大值
	float			s_min;						// 饱和度最小值
	float			s_max;						// 饱和度最大值
	float			v_min;						// 亮度最小值
	float			v_max;						// 亮度最大值
};

enum FASHION_COMBINED_SWITCH
{
	FCS_RANDOM_COLOR_IN_RANGE	= 0x00000001,	// 指定范围内随机颜色
};

///////////////////////////////////////////////////////////////////////////////////////
//
// 各种物品的模板
//
//		注意：在这里,物品是个狭义的物品概念,包括普通物品,任务物品以及特殊物品三大类
//
//
///////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////
//
// 药品模板数据结构定义
//
//
///////////////////////////////////////////////////////////////////////////////////////
// 药品大类别
struct MEDICINE_MAJOR_TYPE
{
	unsigned int	id;							// 药品大类别ID
	namechar		name[32];					// 药品大类别名称
};

// 药品小类别
struct MEDICINE_SUB_TYPE
{
	unsigned int	id;							// 药品小类别ID
	namechar		name[32];					// 药品小类别名称
};

// 药品本体属性
struct MEDICINE_ESSENCE
{
	unsigned int	id;							// 药品(类型)ID
	unsigned int	id_major_type;				// 药品大类别ID
	unsigned int	id_sub_type;				// 药品小类别ID
	namechar		name[32];					// 药品名称, 最多15个汉字

	char			file_matter[128];			// 掉在地上的模型路径名
	char			file_icon[128];				// 图标路径名

	int				require_level;				// 玩家的级别要求
	int				cool_time;					// 冷却时间（毫秒）

	int				hp_add_total;				// 补HP：补充总量
	int				hp_add_time;				// 补HP：补充所需的时间
	int				mp_add_total;				// 补MP：补充总量
	int				mp_add_time;				// 补MP：补充所需的时间

	int				price;						// 卖店价
	int				shop_price;					// 店卖价

	// 堆叠信息
	int				pile_num_max;				// 该药品的堆叠上限
	// GUID信息
	unsigned int	has_guid;					// 是否为其产生全局唯一ID，以用于Trace。0-否，1-是
	// 处理方式
	unsigned int	proc_type;					// 是以下几种方式的组合: 死亡时是否掉落，是否可以扔在地上，是否可以卖给NPC，是人民币物品，是否可以玩家间交易，是否任务相关物品，装备后绑定，是否可解绑，离开场景消失
};

///////////////////////////////////////////////////////////////////////////////////////
//
// 特殊装备物品
//
///////////////////////////////////////////////////////////////////////////////////////
struct BIBLE_ESSENCE
{
	unsigned int	id;							// ID
	namechar		name[32];					// 名称, 最多15个汉字
	char			file_matter[128];			// 掉在地上的模型路径名
	char			file_icon[128];				// 图标路径名

	unsigned int	id_addons[10];				// 10 个附加属性

	int				price;						// 卖店价
	int				shop_price;					// 店卖价

	unsigned int	id_drop_after_damaged;		// 损毁后掉落的物品
	int				num_drop_after_damaged;		// 损毁后掉落的物品数量
	
	// 堆叠信息
	int				pile_num_max;				// 该药品的堆叠上限
	// GUID信息
	unsigned int	has_guid;					// 是否为其产生全局唯一ID，以用于Trace。0-否，1-是
	// 处理方式
	unsigned int	proc_type;					// 是以下几种方式的组合: 死亡时是否掉落，是否可以扔在地上，是否可以卖给NPC，是人民币物品，是否可以玩家间交易，是否任务相关物品，装备后绑定，是否可解绑，离开场景消失
};

struct SPEAKER_ESSENCE
{
	unsigned int	id;							// ID
	namechar		name[32];					// 名称, 最多15个汉字
	char			file_matter[128];			// 掉在地上的模型路径名
	char			file_icon[128];				// 图标路径名

	int				id_icon_set;				// 表情符号套号

	int				price;						// 卖店价
	int				shop_price;					// 店卖价

	// 堆叠信息
	int				pile_num_max;				// 该药品的堆叠上限
	// GUID信息
	unsigned int	has_guid;					// 是否为其产生全局唯一ID，以用于Trace。0-否，1-是
	// 处理方式
	unsigned int	proc_type;					// 是以下几种方式的组合: 死亡时是否掉落，是否可以扔在地上，是否可以卖给NPC，是人民币物品，是否可以玩家间交易，是否任务相关物品，装备后绑定，是否可解绑，离开场景消失
};

struct AUTOHP_ESSENCE
{
	unsigned int	id;							// ID
	namechar		name[32];					// 名称, 最多15个汉字
	char			file_matter[128];			// 掉在地上的模型路径名
	char			file_icon[128];				// 图标路径名

	int				total_hp;					// 回血总量
	float			trigger_amount;				// 触发回血比例
	int				cool_time;					// 冷却时间（毫秒）

	int				price;						// 卖店价
	int				shop_price;					// 店卖价

	// 堆叠信息
	int				pile_num_max;				// 该药品的堆叠上限
	// GUID信息
	unsigned int	has_guid;					// 是否为其产生全局唯一ID，以用于Trace。0-否，1-是
	// 处理方式
	unsigned int	proc_type;					// 是以下几种方式的组合: 死亡时是否掉落，是否可以扔在地上，是否可以卖给NPC，是人民币物品，是否可以玩家间交易，是否任务相关物品，装备后绑定，是否可解绑，离开场景消失
};

struct AUTOMP_ESSENCE
{
	unsigned int	id;							// ID
	namechar		name[32];					// 名称, 最多15个汉字
	char			file_matter[128];			// 掉在地上的模型路径名
	char			file_icon[128];				// 图标路径名

	int				total_mp;					// 回魔总量
	float			trigger_amount;				// 触发回魔比例
	int				cool_time;					// 冷却时间（毫秒）

	int				price;						// 卖店价
	int				shop_price;					// 店卖价

	// 堆叠信息
	int				pile_num_max;				// 该药品的堆叠上限
	// GUID信息
	unsigned int	has_guid;					// 是否为其产生全局唯一ID，以用于Trace。0-否，1-是
	// 处理方式
	unsigned int	proc_type;					// 是以下几种方式的组合: 死亡时是否掉落，是否可以扔在地上，是否可以卖给NPC，是人民币物品，是否可以玩家间交易，是否任务相关物品，装备后绑定，是否可解绑，离开场景消失
};

//	势力令牌
struct FORCE_TOKEN_ESSENCE
{
	unsigned int	id;							// ID
	namechar		name[32];				// 名称, 最多15个汉字

	char					file_matter[128];		//	掉落简化模型
	char					file_icon[128];			//	图标名称

	unsigned int	require_force;			//	势力限制
	int					reputation_add;		//	增加威望
	int					reputation_increase_ratio;	//	威望增加附加百分比

	int					price;							//	卖店价
	int					shop_price;				//	店卖价

	int					pile_num_max;			//	堆叠上限
	unsigned int	has_guid;					//	是否为其产生全局唯一ID
	unsigned int	proc_type;					//	处理方式
};

// 技能装备（获得此装备即获得相应技能）
struct DYNSKILLEQUIP_ESSENCE
{
	unsigned int	id;								// (类型)ID
	namechar		name[32];					// 名称, 最多15个汉字

	char			file_matter[128];			// 掉在地上的模型路径名
	char			file_icon[128];				// 图标路径名

	int				id_skill[4];				// 获得技能

	int				price;							// 卖店价
	int				shop_price;					// 店卖价

	// 堆叠信息
	int				pile_num_max;				// 堆叠上限
	// GUID信息
	unsigned int	has_guid;					// 是否为其产生全局唯一ID，以用于Trace。0-否，1-是
	// 处理方式
	unsigned int	proc_type;					// 是以下几种方式的组合: 死亡时是否掉落，是否可以扔在地上，是否可以卖给NPC，是人民币物品，是否可以玩家间交易，是否任务相关物品，装备后绑定，是否可解绑，离开场景消失
};

///////////////////////////////////////////////////////////////////////////////////////
//
// 生产原料模板数据结构定义
//
///////////////////////////////////////////////////////////////////////////////////////
// 生产原料大类别
struct MATERIAL_MAJOR_TYPE
{
	unsigned int	id;							// 生产原料大类别ID
	namechar		name[32];					// 生产原料大类别名称
};

// 生产原料小类别
struct MATERIAL_SUB_TYPE
{
	unsigned int	id;							// 生产原料小类别ID
	namechar		name[32];					// 生产原料小类别名称
};

// 生产原料本体属性
struct MATERIAL_ESSENCE
{
	unsigned int	id;							// 生产原料(类型)ID
	unsigned int	id_major_type;				// 生产原料大类别ID
	unsigned int	id_sub_type;				// 生产原料小类别ID
	namechar		name[32];					// 生产原料名称, 最多15个汉字

	char			file_matter[128];			// 掉在地上的模型路径名
	char			file_icon[128];				// 图标路径名

	int				price;						// 卖店价
	int				shop_price;					// 店卖价

	// 拆分信息
	int				decompose_price;			// 拆分价格
	int				decompose_time;				// 拆分时间
	unsigned int	element_id;					// 拆分成的元石的类型ID
	int				element_num;				// 拆分产生的元石数

	// 堆叠信息
	int				pile_num_max;				// 该原料的堆叠上限
	// GUID信息
	unsigned int	has_guid;					// 是否为其产生全局唯一ID，以用于Trace。0-否，1-是
	// 处理方式
	unsigned int	proc_type;					// 是以下几种方式的组合: 死亡时是否掉落，是否可以扔在地上，是否可以卖给NPC，是人民币物品，是否可以玩家间交易，是否任务相关物品，装备后绑定，是否可解绑，离开场景消失
};

///////////////////////////////////////////////////////////////////////////////////////
// 精炼概率调整道具
///////////////////////////////////////////////////////////////////////////////////////
struct REFINE_TICKET_ESSENCE
{
	unsigned int	id;							// ID
	namechar		name[32];					// 名称, 最多15个汉字

	char			file_matter[128];			// 掉在地上的模型路径名
	char			file_icon[128];				// 图标路径名

	namechar		desc[16];					// 简单描述

	float			ext_reserved_prob;			// 提高失败后只降一级的概率
	float			ext_succeed_prob;			// 提高精炼成功的概率

	unsigned int	fail_reserve_level;			// 是否失败不降精炼等级
	float			fail_ext_succeed_prob[12];	// 在精炼失败不降等级时的成功概率调整

	unsigned int binding_only;				//	只限天人合一装备
	int				require_level_max;		//	适用品阶上限

	int				price;						// 卖店价
	int				shop_price;					// 店卖价

	// 堆叠信息
	int				pile_num_max;				// 堆叠上限

	// GUID信息
	unsigned int	has_guid;					// 是否产生全局唯一ID，以用于Trace。0-否，1-是
	// 处理方式
	unsigned int	proc_type;					// 是以下几种方式的组合: 死亡时是否掉落，是否可以扔在地上，是否可以卖给NPC，是人民币物品，是否可以玩家间交易，是否任务相关物品，装备后绑定，是否可解绑，离开场景消失
};

///////////////////////////////////////////////////////////////////////////////////////
// 双倍经验道具
///////////////////////////////////////////////////////////////////////////////////////
struct DOUBLE_EXP_ESSENCE
{
	unsigned int	id;							// (类型)ID
	namechar		name[32];					// 名称, 最多15个汉字

	char			file_matter[128];			// 掉在地上的模型路径名
	char			file_icon[128];				// 图标路径名

	int				double_exp_time;			// 双倍经验时间(秒)

	int				price;						// 卖店价
	int				shop_price;					// 店卖价

	// 堆叠信息
	int				pile_num_max;				// 堆叠上限
	// GUID信息
	unsigned int	has_guid;					// 是否为其产生全局唯一ID，以用于Trace。0-否，1-是
	// 处理方式
	unsigned int	proc_type;					// 是以下几种方式的组合: 死亡时是否掉落，是否可以扔在地上，是否可以卖给NPC，是人民币物品，是否可以玩家间交易，是否任务相关物品，装备后绑定，是否可解绑，离开场景消失
};

///////////////////////////////////////////////////////////////////////////////////////
// 时装染色道具
///////////////////////////////////////////////////////////////////////////////////////
struct DYE_TICKET_ESSENCE
{
	unsigned int	id;							// (类型)ID
	namechar		name[32];					// 名称, 最多15个汉字

	char			file_matter[128];			// 掉在地上的模型路径名
	char			file_icon[128];				// 图标路径名

	float			h_min;						// 色调最小值
	float			h_max;						// 色调最大值
	float			s_min;						// 饱和度最小值
	float			s_max;						// 饱和度最大值
	float			v_min;						// 亮度最小值
	float			v_max;						// 亮度最大值
	
	int				price;						// 卖店价
	int				shop_price;					// 店卖价

	// 堆叠信息
	int				pile_num_max;				// 堆叠上限
	// GUID信息
	unsigned int	has_guid;					// 是否为其产生全局唯一ID，以用于Trace。0-否，1-是
	// 处理方式
	unsigned int	proc_type;					// 是以下几种方式的组合: 死亡时是否掉落，是否可以扔在地上，是否可以卖给NPC，是人民币物品，是否可以玩家间交易，是否任务相关物品，装备后绑定，是否可解绑，离开场景消失

	unsigned int	usage;						// 染色剂用途（0:普通染色（时装、宠物等），1:签名墨水等）
};

///////////////////////////////////////////////////////////////////////////////////////
// 正在销毁中的物品
///////////////////////////////////////////////////////////////////////////////////////
struct DESTROYING_ESSENCE
{
	unsigned int	id;							// ID
	namechar		name[32];					// 名称, 最多15个汉字

	char			file_matter[128];			// 掉在地上的模型路径名
	char			file_icon[128];				// 图标路径名

	int				price;						// 卖店价
	int				shop_price;					// 店卖价

	// 堆叠信息
	int				pile_num_max;				// 堆叠上限

	// GUID信息
	unsigned int	has_guid;					// 是否产生全局唯一ID，以用于Trace。0-否，1-是
	// 处理方式
	unsigned int	proc_type;					// 是以下几种方式的组合: 死亡时是否掉落，是否可以扔在地上，是否可以卖给NPC，是人民币物品，是否可以玩家间交易，是否任务相关物品，装备后绑定，是否可解绑，离开场景消失
};

///////////////////////////////////////////////////////////////////////////////////////
//
// 攻击优化符模板数据结构定义
//
//		在这里,我们把"符"参照Diabllo中的使用方法,译为"rune",意为古代北欧使用的文字, 诗歌
//
//
///////////////////////////////////////////////////////////////////////////////////////
// 攻击优化符小类别
struct DAMAGERUNE_SUB_TYPE
{
	unsigned int	id;							// 攻击优化符小类别ID
	namechar		name[32];					// 攻击优化符小类别名称
};

// 攻击优化符本体属性
struct DAMAGERUNE_ESSENCE
{
	unsigned int	id;							// (类型)ID
	unsigned int	id_sub_type;				// 攻击优化符小类别ID
	namechar		name[32];					// 名称, 最多15个汉字

	char			file_matter[128];			// 掉在地上的模型路径名
	char			file_icon[128];				// 图标路径名
	
	unsigned int	damage_type;				// 种类选择：物理、法术选择(0 表示物理, 1 表示法术)

	int				price;						// 卖店价
	int				shop_price;					// 店卖价

	int				require_weapon_level_min;	// 使用武器的级别区间：最小
	int				require_weapon_level_max;	// 使用武器的级别区间：最大
	
	int				damage_increased;			// 增加武器攻击力固定值(int)

	// 堆叠信息
	int				pile_num_max;				// 堆叠上限
	// GUID信息
	unsigned int	has_guid;					// 是否为其产生全局唯一ID，以用于Trace。0-否，1-是
	// 处理方式
	unsigned int	proc_type;					// 是以下几种方式的组合: 死亡时是否掉落，是否可以扔在地上，是否可以卖给NPC，是人民币物品，是否可以玩家间交易，是否任务相关物品，装备后绑定，是否可解绑，离开场景消失
};

///////////////////////////////////////////////////////////////////////////////////////
//
// 防御优化符模板数据结构定义
//
//		在这里,我们把"符"参照Diabllo中的使用方法,译为"rune",意为古代北欧使用的文字, 诗歌
//
//
///////////////////////////////////////////////////////////////////////////////////////
// 防御优化符小类别
struct ARMORRUNE_SUB_TYPE
{
	unsigned int	id;							// 防御优化符小类别ID
	namechar		name[32];					// 防御优化符小类别名称
};

// 防御优化符本体属性
struct ARMORRUNE_ESSENCE
{
	unsigned int	id;							// (类型)ID
	unsigned int	id_sub_type;				// 防御优化符小类别ID
	namechar		name[32];					// 名称, 最多15个汉字

	char			file_matter[128];			// 掉在地上的模型路径名
	char			file_icon[128];				// 图标路径名
	char			file_gfx[128];				// 使用时的效果
	char			file_sfx[128];				// 使用时的音效

	unsigned int	damage_type;				// 种类选择：物理、法术选择(0 表示物理, 1 表示法术)

	int				price;						// 卖店价
	int				shop_price;					// 店卖价

	int				require_player_level_min;	// 使用需要的人物级别区间：最小
	int				require_player_level_max;	// 使用需要的人物级别区间：最大
	
	float			damage_reduce_percent;		// 对攻击方攻击力的降低百分比
	int				damage_reduce_time;			// 抵御攻击的次数：数字输入

	// 堆叠信息
	int				pile_num_max;				// 堆叠上限
	// GUID信息
	unsigned int	has_guid;					// 是否为其产生全局唯一ID，以用于Trace。0-否，1-是
	// 处理方式
	unsigned int	proc_type;					// 是以下几种方式的组合: 死亡时是否掉落，是否可以扔在地上，是否可以卖给NPC，是人民币物品，是否可以玩家间交易，是否任务相关物品，装备后绑定，是否可解绑，离开场景消失
};


///////////////////////////////////////////////////////////////////////////////////////
//
// 技能书模板数据结构定义
//
//		在这里,我们把"书"参照Diabllo中的使用方法,译为"tome",意为册, 卷, 本, 大本书
//
//
///////////////////////////////////////////////////////////////////////////////////////
// 技能书小类别
struct SKILLTOME_SUB_TYPE
{
	unsigned int	id;							// 技能书小类别ID
	namechar		name[32];					// 技能书小类别名称
};
		   
// 技能书本体属性
struct SKILLTOME_ESSENCE
{
	unsigned int	id;							// (类型)ID
	unsigned int	id_sub_type;				// 技能书小类别ID
	namechar		name[32];					// 名称, 最多15个汉字

	char			file_matter[128];			// 掉在地上的模型路径名
	char			file_icon[128];				// 图标路径名

	int				price;						// 卖店价
	int				shop_price;					// 店卖价

	// 堆叠信息
	int				pile_num_max;				// 堆叠上限
	// GUID信息
	unsigned int	has_guid;					// 是否为其产生全局唯一ID，以用于Trace。0-否，1-是
	// 处理方式
	unsigned int	proc_type;					// 是以下几种方式的组合: 死亡时是否掉落，是否可以扔在地上，是否可以卖给NPC，是人民币物品，是否可以玩家间交易，是否任务相关物品，装备后绑定，是否可解绑，离开场景消失
};

// 商城代币
struct SHOP_TOKEN_ESSENCE
{
	unsigned int	id;							// (类型)ID
	namechar		name[32];					// 名称, 最多15个汉字
	
	char			file_matter[128];			// 掉在地上的模型路径名
	char			file_icon[128];				// 图标路径名

	unsigned int	id_token_shop;				// 对应商城
	
	int				price;						// 卖店价
	int				shop_price;					// 店卖价
	
	// 堆叠信息
	int				pile_num_max;				// 堆叠上限
	// GUID信息
	unsigned int	has_guid;					// 是否为其产生全局唯一ID，以用于Trace。0-否，1-是
	// 处理方式
	unsigned int	proc_type;					// 是以下几种方式的组合: 死亡时是否掉落，是否可以扔在地上，是否可以卖给NPC，是人民币物品，是否可以玩家间交易，是否任务相关物品，装备后绑定，是否可解绑，离开场景消失
};

// 通用客户端功能物品
struct UNIVERSAL_TOKEN_ESSENCE
{
	unsigned int	id;							// (类型)ID
	namechar		name[32];					// 名称, 最多15个汉字
	
	char			file_matter[128];			// 掉在地上的模型路径名
	char			file_icon[128];				// 图标路径名
	
	unsigned int	combined_switch;			// 各种组合开关（见 UNIVERSAL_TOKEN_COMBINED_SWITCH ）
	int				config_id[ELEMENTDATA_MAX_UNIVERSAL_TOKEN_USAGE];	//	各组合开关对应的配置参数 （每种对应一个组合开关）
	
	int				price;						// 卖店价
	int				shop_price;					// 店卖价
	
	// 堆叠信息
	int				pile_num_max;				// 堆叠上限
	// GUID信息
	unsigned int	has_guid;					// 是否为其产生全局唯一ID，以用于Trace。0-否，1-是
	// 处理方式
	unsigned int	proc_type;					// 是以下几种方式的组合: 死亡时是否掉落，是否可以扔在地上，是否可以卖给NPC，是人民币物品，是否可以玩家间交易，是否任务相关物品，装备后绑定，是否可解绑，离开场景消失
};

enum	UNIVERSAL_TOKEN_COMBINED_SWITCH
{
	UTCS_VISIT_HTTP_WITH_TOKEN	= 0x00000001,	//	申请 token 并访问网址
};

/////////////////////////////////////////////////////////////////////////////////////////////////
// 时装套装
/////////////////////////////////////////////////////////////////////////////////////////////////
enum	FashionSuiteItemIndex
{
	FASHION_SUITE_HEAD,
	FASHION_SUITE_UPPER_BODY,
	FASHION_SUITE_WRIST,
	FASHION_SUITE_LOWER,
	FASHION_SUITE_FOOT,
	FASHION_SUITE_WEAPON,
	FASHION_SUITE_NUM,
};

struct FASHION_SUITE_ESSENCE
{
	// 基本信息
	unsigned int	id;							//	套装(类型)ID
	namechar		name[32];					//	套装名称, 最多15个汉字

	char			file_icon[128];				//	图标名称
	unsigned	int	list[FASHION_SUITE_NUM];	//	id列表
	int				show_order;					//	展示顺序
};

///////////////////////////////////////////////////////////////////////////////////////
//
// 基地设施模板数据结构定义
//
//
//
///////////////////////////////////////////////////////////////////////////////////////
// 基地设施小类别
struct FACTION_BUILDING_SUB_TYPE
{
	unsigned int	id;							// 基地设施小类别ID
	namechar		name[32];					// 基地设施小类别名称
};
		   
// 基地设施本体属性
struct FACTION_BUILDING_ESSENCE
{
	unsigned int	id;							// (类型)ID
	unsigned int	id_sub_type;				// 基地设施小类别ID
	namechar		name[32];					// 名称, 最多15个汉字

	char			file_icon[128];				//	图标名称

	int		level;								//	等级

	int		require_level;						//	基地等级限制
	int		technology[5];						//	需要科技%天赋点数
	int		money;								//	需要金钱
	int		material[8];						//	需要材料%个数

	int		base_time;							//	建造起步时间
	int		delta_time;							//	材料增加一成减少的建造时间

	int		controller_id0;						//	建造中的控制器ID
	int		controller_id1;						//	建造后的控制器ID
};

///////////////////////////////////////////////////////////////////////////////////////
//
// 基地材料模板数据结构定义
//
//
//
///////////////////////////////////////////////////////////////////////////////////////
struct FACTION_MATERIAL_ESSENCE
{
	unsigned int	id;							//	(类型)ID
	namechar		name[32];					//	名称, 最多15个汉字

	char			file_matter[128];			//	掉落简化模型
	char			file_icon[128];				//	图标名称

	int				material_count[8];			//	等价材料%个数

	int				price;						//	卖店价
	int				shop_price;					//	店卖价

	int				pile_num_max;				//	堆叠上限
	unsigned int	has_guid;					//	是否为其产生全局唯一ID
	unsigned int	proc_type;					//	处理方式
};


///////////////////////////////////////////////////////////////////////////////////////
//
// 飞剑类模板数据结构定义
//
//
//
///////////////////////////////////////////////////////////////////////////////////////
struct FLYSWORD_ESSENCE
{
	unsigned int	id;							// (类型)ID
	namechar		name[32];					// 名称, 最多15个汉字

	char			file_model[128];			// 模型路径名
	char			file_model2[128];			// 附加模型路径名
	char			file_matter[128];			// 掉在地上的模型路径名
	char			file_icon[128];				// 图标路径名

	int				price;						// 卖店价
	int				shop_price;					// 店卖价

	int				level;						// 飞剑等级
	int				require_player_level_min;	// 使用需要的最低人物级别

	float			speed_increase_min;			// 常规飞行增速下限
	float			speed_increase_max;			// 常规飞行增速上限
	float			speed_rush_increase_min;	// 冲刺飞行增速下限
	float			speed_rush_increase_max;	// 冲刺飞行增速上限

	float			time_max_min;				// 最大飞行时间上限
	float			time_max_max;				// 最大飞行时间下限

	float			time_increase_per_element;	// 每装入一个元石增加的飞行时间

	unsigned int	fly_mode;					// 飞行模式，可以是: 0-飞剑, 1-翅膀, 2-风火轮

	unsigned int	character_combo_id;			// 职业限制组合, 从低位到高位的含义为：0-武侠, 1-法师, 2-巫师, 3-妖精, 4-妖兽, 5-刺客, 6-羽芒, 7-羽灵, 8-剑灵, 9-魅灵

	unsigned int	id_drop_after_damaged;		// 损毁后掉落的物品
	int				num_drop_after_damaged;		// 损毁后掉落的物品数量

	int				max_improve_level;			// 强化等级上限
	struct  
	{
		unsigned int require_item_num;			// 消耗物品数量
		float		speed_increase;				// 增加常规飞行速度
		float		speed_rush_increase;		// 增加冲刺飞行速度
	}improve_config[10];						// %级强化
	
	// 堆叠信息
	int				pile_num_max;				// 堆叠上限
	// GUID信息
	unsigned int	has_guid;					// 是否为其产生全局唯一ID，以用于Trace。0-否，1-是
	// 处理方式
	unsigned int	proc_type;					// 是以下几种方式的组合: 死亡时是否掉落，是否可以扔在地上，是否可以卖给NPC，是人民币物品，是否可以玩家间交易，是否任务相关物品，装备后绑定，是否可解绑，离开场景消失

	// 飞剑没有显式的逻辑装备Mask, 因为飞剑只能装在飞剑槽上
};


// 羽人翅膀本体
struct WINGMANWING_ESSENCE
{
	unsigned int	id;							// (类型)ID
	namechar		name[32];					// 名称, 最多15个汉字

	char			file_model[128];			// 模型路径名
	char			file_matter[128];			// 掉在地上的模型路径名
	char			file_icon[128];				// 图标路径名

	int				price;						// 卖店价
	int				shop_price;					// 店卖价

	int				require_player_level_min;	// 使用需要的最低人物级别
	float			speed_increase;				// 对速度的增加数字

	int				mp_launch;					// 起飞耗费的MP
	int				mp_per_second;				// 每秒耗费的MP

	// 堆叠信息
	int				pile_num_max;				// 堆叠上限
	// GUID信息
	unsigned int	has_guid;					// 是否为其产生全局唯一ID，以用于Trace。0-否，1-是
	// 处理方式
	unsigned int	proc_type;					// 是以下几种方式的组合: 死亡时是否掉落，是否可以扔在地上，是否可以卖给NPC，是人民币物品，是否可以玩家间交易，是否任务相关物品，装备后绑定，是否可解绑，离开场景消失

	// 羽人翅膀没有显式的逻辑装备Mask, 因为羽人翅膀只能装在飞剑槽上
};


///////////////////////////////////////////////////////////////////////////////////////
//
// 回城卷轴模板数据结构定义
//
//
//
///////////////////////////////////////////////////////////////////////////////////////
struct TOWNSCROLL_ESSENCE
{
	unsigned int	id;							// (类型)ID
	namechar		name[32];					// 名称, 最多15个汉字

	char			file_matter[128];			// 掉在地上的模型路径名
	char			file_icon[128];				// 图标路径名

	float			use_time;					// 使用时间（秒）

	int				price;						// 卖店价
	int				shop_price;					// 店卖价

	// 堆叠信息
	int				pile_num_max;				// 堆叠上限
	// GUID信息
	unsigned int	has_guid;					// 是否为其产生全局唯一ID，以用于Trace。0-否，1-是
	// 处理方式
	unsigned int	proc_type;					// 是以下几种方式的组合: 死亡时是否掉落，是否可以扔在地上，是否可以卖给NPC，是人民币物品，是否可以玩家间交易，是否任务相关物品，装备后绑定，是否可解绑，离开场景消失
};

///////////////////////////////////////////////////////////////////////////////////////
//
// 传送符模板数据结构定义
//
//
//
///////////////////////////////////////////////////////////////////////////////////////
struct TRANSMITSCROLL_ESSENCE
{
	unsigned int	id;							// (类型)ID
	namechar		name[32];					// 名称, 最多15个汉字

	char			file_matter[128];			// 掉在地上的模型路径名
	char			file_icon[128];				// 图标路径名

	int				price;						// 卖店价
	int				shop_price;					// 店卖价

	// 堆叠信息
	int				pile_num_max;				// 堆叠上限
	// GUID信息
	unsigned int	has_guid;					// 是否为其产生全局唯一ID，以用于Trace。0-否，1-是
	// 处理方式
	unsigned int	proc_type;					// 是以下几种方式的组合: 死亡时是否掉落，是否可以扔在地上，是否可以卖给NPC，是人民币物品，是否可以玩家间交易，是否任务相关物品，装备后绑定，是否可解绑，离开场景消失
};

///////////////////////////////////////////////////////////////////////////////////////
//
// 帮派回城卷轴模板数据结构定义
//
//
//
///////////////////////////////////////////////////////////////////////////////////////
struct UNIONSCROLL_ESSENCE
{
	unsigned int	id;							// (类型)ID
	namechar		name[32];					// 名称, 最多15个汉字

	char			file_matter[128];			// 掉在地上的模型路径名
	char			file_icon[128];				// 图标路径名

	float			use_time;					// 使用时间（秒）

	int				price;						// 卖店价
	int				shop_price;					// 店卖价

	// 堆叠信息
	int				pile_num_max;				// 堆叠上限
	// GUID信息
	unsigned int	has_guid;					// 是否为其产生全局唯一ID，以用于Trace。0-否，1-是
	// 处理方式
	unsigned int	proc_type;					// 是以下几种方式的组合: 死亡时是否掉落，是否可以扔在地上，是否可以卖给NPC，是人民币物品，是否可以玩家间交易，是否任务相关物品，装备后绑定，是否可解绑，离开场景消失
};

///////////////////////////////////////////////////////////////////////////////////////
//
// 复活卷轴模板数据结构定义
//
//
//
///////////////////////////////////////////////////////////////////////////////////////
struct REVIVESCROLL_ESSENCE
{
	unsigned int	id;							// (类型)ID
	namechar		name[32];					// 名称, 最多15个汉字

	char			file_matter[128];			// 掉在地上的模型路径名
	char			file_icon[128];				// 图标路径名

	float			use_time;					// 使用时间（秒）
	int				cool_time;					// 冷却时间（毫秒）

	int				price;						// 卖店价
	int				shop_price;					// 店卖价

	// 堆叠信息
	int				pile_num_max;				// 堆叠上限
	// GUID信息
	unsigned int	has_guid;					// 是否为其产生全局唯一ID，以用于Trace。0-否，1-是
	// 处理方式
	unsigned int	proc_type;					// 是以下几种方式的组合: 死亡时是否掉落，是否可以扔在地上，是否可以卖给NPC，是人民币物品，是否可以玩家间交易，是否任务相关物品，装备后绑定，是否可解绑，离开场景消失
};

///////////////////////////////////////////////////////////////////////////////////////
//
// 元石模板数据结构定义
//
//
//
///////////////////////////////////////////////////////////////////////////////////////
struct ELEMENT_ESSENCE
{
	unsigned int	id;							// (类型)ID
	namechar		name[32];					// 名称, 最多15个汉字

	char			file_matter[128];			// 掉在地上的模型路径名
	char			file_icon[128];				// 图标路径名

	int				level;						// 元石等级

	int				price;						// 卖店价
	int				shop_price;					// 店卖价

	// 堆叠信息
	int				pile_num_max;				// 堆叠上限
	// GUID信息
	unsigned int	has_guid;					// 是否为其产生全局唯一ID，以用于Trace。0-否，1-是
	// 处理方式
	unsigned int	proc_type;					// 是以下几种方式的组合: 死亡时是否掉落，是否可以扔在地上，是否可以卖给NPC，是人民币物品，是否可以玩家间交易，是否任务相关物品，装备后绑定，是否可解绑，离开场景消失
};

///////////////////////////////////////////////////////////////////////////////////////
//
// 金币兑现石
//
//
//
///////////////////////////////////////////////////////////////////////////////////////
struct MONEY_CONVERTIBLE_ESSENCE
{
	unsigned int	id;							// (类型)ID
	namechar		name[32];					// 名称, 最多15个汉字
	
	char			file_matter[128];			// 掉在地上的模型路径名
	char			file_icon[128];				// 图标路径名
	
	int				price;						// 卖店价
	int				shop_price;					// 店卖价
	
	// 堆叠信息
	int				pile_num_max;				// 堆叠上限
	// GUID信息
	unsigned int	has_guid;					// 是否为其产生全局唯一ID，以用于Trace。0-否，1-是
	// 处理方式
	unsigned int	proc_type;					// 是以下几种方式的组合: 死亡时是否掉落，是否可以扔在地上，是否可以卖给NPC，是人民币物品，是否可以玩家间交易，是否任务相关物品，装备后绑定，是否可解绑，离开场景消失
};

///////////////////////////////////////////////////////////////////////////////////////
//
// 任务物品模板数据结构定义
//
//
//
///////////////////////////////////////////////////////////////////////////////////////
struct TASKMATTER_ESSENCE
{
	unsigned int	id;							// (类型)ID
	namechar		name[32];					// 名称, 最多15个汉字

	char			file_icon[128];				// 图标路径名

	// 堆叠信息
	int				pile_num_max;				// 堆叠上限
	// GUID信息
	unsigned int	has_guid;					// 是否为其产生全局唯一ID，以用于Trace。0-否，1-是
	// 处理方式
	unsigned int	proc_type;					// 是以下几种方式的组合: 死亡时是否掉落，是否可以扔在地上，是否可以卖给NPC，是人民币物品，是否可以玩家间交易，是否任务相关物品，装备后绑定，是否可解绑，离开场景消失
};

///////////////////////////////////////////////////////////////////////////////////////
//	用于任务的普通物品
///////////////////////////////////////////////////////////////////////////////////////
struct TASKNORMALMATTER_ESSENCE
{
	unsigned int	id;							// (类型)ID
	namechar		name[32];					// 名称, 最多15个汉字

	char			file_matter[128];			// 掉在地上的模型路径名
	char			file_icon[128];				// 图标路径名

	int				price;						// 卖店价
	int				shop_price;					// 店卖价

	// 堆叠信息
	int				pile_num_max;				// 堆叠上限
	// GUID信息
	unsigned int	has_guid;					// 是否为其产生全局唯一ID，以用于Trace。0-否，1-是
	// 处理方式
	unsigned int	proc_type;					// 是以下几种方式的组合: 死亡时是否掉落，是否可以扔在地上，是否可以卖给NPC，是人民币物品，是否可以玩家间交易，是否任务相关物品，装备后绑定，是否可解绑，离开场景消失
};

///////////////////////////////////////////////////////////////////////////////////////
//
// 任务随机发生器数据结构定义
//
//
//
///////////////////////////////////////////////////////////////////////////////////////
struct TASKDICE_ESSENCE
{
	unsigned int	id;							// (类型)ID
	namechar		name[32];					// 名称, 最多15个汉字

	char			file_matter[128];			// 掉在地上的模型路径名
	char			file_icon[128];				// 图标路径名

	struct
	{
		unsigned int	id;						// 任务id
		float			probability;			// 产生该任务的概率
		
	} task_lists[20];

	// 捡了就用标志
	unsigned int	use_on_pick;				// true or false, 客户端定期检查自己的包裹，如果发现这样的东东就立刻使用
	// 堆叠信息
	int				pile_num_max;				// 堆叠上限
	// GUID信息
	unsigned int	has_guid;					// 是否为其产生全局唯一ID，以用于Trace。0-否，1-是
	// 处理方式
	unsigned int	proc_type;					// 是以下几种方式的组合: 死亡时是否掉落，是否可以扔在地上，是否可以卖给NPC，是人民币物品，是否可以玩家间交易，是否任务相关物品，装备后绑定，是否可解绑，离开场景消失
	int				no_use_in_combat;			//	战斗时是否禁用：1（是），0（否）
};

///////////////////////////////////////////////////////////////////////////////////////
//
// 暗器模板数据结构定义
//
//
//
///////////////////////////////////////////////////////////////////////////////////////
struct TOSSMATTER_ESSENCE
{
	unsigned int	id;							// (类型)ID
	namechar		name[32];					// 名称, 最多15个汉字

	char			file_model[128];			// 模型路径名
	char			file_matter[128];			// 掉在地上的模型路径名
	char			file_icon[128];				// 图标路径名
	char			file_firegfx[128];			// 飞行效果
	char			file_hitgfx[128];			// 击中效果名字
	char			file_hitsfx[128];			// 击中声音效果名字

	// 使用规则
	int				require_strength;			// 力量限制
	int				require_agility;			// 敏捷限制
	int				require_level;				// 等级限制

	int				damage_low;					// 物理攻击力最小值
	int				damage_high_min;			// 物理攻击力最大值的区间最小值
	int				damage_high_max;			// 物理攻击力最大值的区间最大值

	float			use_time;					// 使用时间（秒）
	float			attack_range;				// 攻击距离

	int				price;						// 卖店价
	int				shop_price;					// 店卖价

	// 堆叠信息
	int				pile_num_max;				// 堆叠上限
	// GUID信息
	unsigned int	has_guid;					// 是否为其产生全局唯一ID，以用于Trace。0-否，1-是
	// 处理方式
	unsigned int	proc_type;					// 是以下几种方式的组合: 死亡时是否掉落，是否可以扔在地上，是否可以卖给NPC，是人民币物品，是否可以玩家间交易，是否任务相关物品，装备后绑定，是否可解绑，离开场景消失
};

///////////////////////////////////////////////////////////////////////////////////////
//
// 弹药模板数据结构定义
//		
//		弹药要求先装备, 再使用, 所以属于装备.
//
///////////////////////////////////////////////////////////////////////////////////////
struct PROJECTILE_TYPE
{
	unsigned int	id;							// 弹药类型ID
	namechar		name[32];					// 名称, 最多15个汉字

};

struct PROJECTILE_ESSENCE
{
	unsigned int	id;							// (类型)ID
	unsigned int	type;						// 弹药类型 (从弹药类型中选取)
	namechar		name[32];					// 名称, 最多15个汉字

	char			file_model[128];			// 模型路径名
	char			file_matter[128];			// 掉在地上的模型路径名
	char			file_icon[128];				// 图标路径名

	char			file_firegfx[128];			// 飞行效果
	char			file_hitgfx[128];			// 击中效果名字
	char			file_hitsfx[128];			// 击中声音效果名字

	int				require_weapon_level_min;	// 使用武器级别范围: 最小值 (装配时并不作检查,放在这只是给策划备忘用)
	int				require_weapon_level_max;	// 使用武器级别范围: 最大值

	int				damage_enhance;				// 附加武器物理攻击力
	int				damage_scale_enhance;		// 附加武器物理攻击力比例
	
	int				price;						// 卖店价
	int				shop_price;					// 店卖价

	// 附加属性
	unsigned int	id_addon0;					// 附加属性的类型ID
	unsigned int	id_addon1;					// 附加属性的类型ID
	unsigned int	id_addon2;					// 附加属性的类型ID
	unsigned int	id_addon3;					// 附加属性的类型ID
	
	// 堆叠信息
	int				pile_num_max;				// 堆叠上限
	// GUID信息
	unsigned int	has_guid;					// 是否为其产生全局唯一ID，以用于Trace。0-否，1-是
	// 处理方式
	unsigned int	proc_type;					// 是以下几种方式的组合: 死亡时是否掉落，是否可以扔在地上，是否可以卖给NPC，是人民币物品，是否可以玩家间交易，是否任务相关物品，装备后绑定，是否可解绑，离开场景消失

	// 弹药没有显式制定其装备规则，因为所有的弹药的装备Mask必须为EQUIP_MASK_PROJECTILE
};

// 掉落的箭囊小类别
struct QUIVER_SUB_TYPE
{
	unsigned int	id;							// 箭囊类型ID
	namechar		name[32];					// 箭囊名称, 最多15个汉字

};

// 箭囊本体
struct QUIVER_ESSENCE
{
	unsigned int	id;							// (类型)ID
	unsigned int	id_sub_type;				// 箭囊小类别ID
	namechar		name[32];					// 名称, 最多15个汉字

	char			file_matter[128];			// 掉在地上的模型路径名
	char			file_icon[128];				// 图标路径名

	unsigned int	id_projectile;				// 盛装的弹药个体
	int				num_min;					// 数目低
	int				num_max;					// 数目高

};

// 镶嵌用宝石小类别
struct STONE_SUB_TYPE
{
	unsigned int	id;							// 宝石类型ID
	namechar		name[32];					// 宝石名称, 最多15个汉字

};

// 镶嵌用宝石本体
struct STONE_ESSENCE
{
	unsigned int	id;							// (类型)ID
	unsigned int	id_sub_type;				// 宝石小类别ID
	namechar		name[32];					// 名称, 最多15个汉字

	char			file_matter[128];			// 掉在地上的模型路径名
	char			file_icon[128];				// 图标路径名

	int				level;						// 级别
	int				color;						// 颜色 (0-7)

	int				price;						// 卖店价
	int				shop_price;					// 店卖价

	int				install_price;				// 镶嵌价格
	int				uninstall_price;			// 拆除价格

	unsigned int	id_addon_damage;			// 攻击属性描述id
	unsigned int	id_addon_defence;			// 防御属性描述id

	namechar		weapon_desc[16];			// 镶在武器上的描述
	namechar		armor_desc[16];				// 镶在防具上的描述

	// 堆叠信息
	int				pile_num_max;				// 堆叠上限
	// GUID信息
	unsigned int	has_guid;					// 是否为其产生全局唯一ID，以用于Trace。0-否，1-是
	// 处理方式
	unsigned int	proc_type;					// 是以下几种方式的组合: 死亡时是否掉落，是否可以扔在地上，是否可以卖给NPC，是人民币物品，是否可以玩家间交易，是否任务相关物品，装备后绑定，是否可解绑，离开场景消失

	// 宝石没有显式制定其装备规则，因为所有的宝石只能用于镶嵌，Mask必须为0x8000
};

/////////////////////////////////////////////////////////////////////////////////////////
//
// 个性化相关物品
//
/////////////////////////////////////////////////////////////////////////////////////////

// 整容卷大类别
struct FACETICKET_MAJOR_TYPE
{
	unsigned int	id;							// 整容卷大类别ID
	namechar		name[32];					// 整容卷大类别名称
};

// 整容卷小类别
struct FACETICKET_SUB_TYPE
{
	unsigned int	id;							// 整容卷小类别ID
	namechar		name[32];					// 整容卷小类别名称
};

// 整容卷本体
struct FACETICKET_ESSENCE
{
	unsigned int	id;							// (类型)ID
	unsigned int	id_major_type;				// 整容卷大类别ID
	unsigned int	id_sub_type;				// 整容卷小类别ID
	namechar		name[32];					// 名称, 最多15个汉字

	char			file_matter[128];			// 掉在地上的模型路径名
	char			file_icon[128];				// 图标路径名

	int				require_level;				// 级别限制

	char			bound_file[128];			// 个性化限制文件
	unsigned int	unsymmetrical;				// 是否允许非对称调整

	int				price;						// 卖店价
	int				shop_price;					// 店卖价

	// 堆叠信息
	int				pile_num_max;				// 该整容卷的堆叠上限
	// GUID信息
	unsigned int	has_guid;					// 是否为其产生全局唯一ID，以用于Trace。0-否，1-是
	// 处理方式
	unsigned int	proc_type;					// 是以下几种方式的组合: 死亡时是否掉落，是否可以扔在地上，是否可以卖给NPC，是人民币物品，是否可以玩家间交易，是否任务相关物品，装备后绑定，是否可解绑，离开场景消失
};

// 变形丸大类别
struct FACEPILL_MAJOR_TYPE
{
	unsigned int	id;							// 变形丸大类别ID
	namechar		name[32];					// 变形丸大类别名称
};

// 变形丸小类别
struct FACEPILL_SUB_TYPE
{
	unsigned int	id;							// 变形丸小类别ID
	namechar		name[32];					// 变形丸小类别名称
};

// 变形丸本体
struct FACEPILL_ESSENCE
{
	unsigned int	id;							// (类型)ID
	unsigned int	id_major_type;				// 变形丸大类别ID
	unsigned int	id_sub_type;				// 变形丸小类别ID
	namechar		name[32];					// 名称, 最多15个汉字

	char			file_matter[128];			// 掉在地上的模型路径名
	char			file_icon[128];				// 图标路径名

	int				duration;					// 有效时间(秒)

	float			camera_scale;				// 照相机拉后距离比例
	
	unsigned int	character_combo_id;			// 职业限制组合, 从低位到高位的含义为：0-武侠, 1-法师, 2-巫师, 3-妖精, 4-妖兽, 5-刺客, 6-羽芒, 7-羽灵, 8-剑灵, 9-魅灵

	struct
	{
		char		file[128];					// 变形丸路径

	} pllfiles[ELEMENTDATA_NUM_PROFESSION * 2];				// 各个形象的变形丸文件

	int				price;						// 卖店价
	int				shop_price;					// 店卖价

	// 堆叠信息
	int				pile_num_max;				// 该变形丸的堆叠上限
	// GUID信息
	unsigned int	has_guid;					// 是否为其产生全局唯一ID，以用于Trace。0-否，1-是
	// 处理方式
	unsigned int	proc_type;					// 是以下几种方式的组合: 死亡时是否掉落，是否可以扔在地上，是否可以卖给NPC，是人民币物品，是否可以玩家间交易，是否任务相关物品，装备后绑定，是否可解绑，离开场景消失
};


////////////////////////////////////////////////////////////////////////////////////
// GM 物品生成器
////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
// GM 物品生成器类型定义
///////////////////////////////////////////////////////////////////////////////////////
struct GM_GENERATOR_TYPE
{
	unsigned int	id;							// (类型)ID
	namechar		name[32];					// 名称, 最多15个汉字
};

///////////////////////////////////////////////////////////////////////////////////////
// GM 物品生成器数据结构定义
///////////////////////////////////////////////////////////////////////////////////////
struct GM_GENERATOR_ESSENCE
{
	unsigned int	id;							// ID
	unsigned int	id_type;					// 类别ID
	namechar		name[32];					// 名称, 最多15个汉字

	char			file_matter[128];			// 掉在地上的模型路径名
	char			file_icon[128];				// 图标路径名

	unsigned int	id_object;					// 待生成的物品（怪物）ID

	// 堆叠信息
	int				pile_num_max;				// 堆叠上限
	// GUID信息
	unsigned int	has_guid;					// 是否为其产生全局唯一ID，以用于Trace。0-否，1-是
	// 处理方式
	unsigned int	proc_type;					// 是以下几种方式的组合: 死亡时是否掉落，是否可以扔在地上，是否可以卖给NPC，是人民币物品，是否可以玩家间交易，是否任务相关物品，装备后绑定，是否可解绑，离开场景消失
};

/////////////////////////////////////////////////////////////////////////////////////////
// 宠物相关物品
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
// 宠物蛋
/////////////////////////////////////////////////////////////////////////////////////////
struct PET_EGG_ESSENCE
{
	unsigned int	id;							// ID
	namechar		name[32];					// 名称, 最多15个汉字

	char			file_matter[128];			// 掉在地上的模型路径名
	char			file_icon[128];				// 图标路径名

	int				id_pet;						// 宠物的模板ID

	int				money_hatched;				// 孵化价格
	int				money_restored;				// 还原价格

	int				honor_point;				// 初始忠诚度
	int				level;						// 初始宠物级别
	int				exp;						// 初始宠物经验值
	int				skill_point;				// 初始剩余技能点

    struct 
	{
		int			id_skill;					// 初始技能ID
		int			level;						// 初始技能等级
	} skills[32];

	int				price;						// 卖店价
	int				shop_price;					// 店卖价

	// 堆叠信息
	int				pile_num_max;				// 该宠物蛋的堆叠上限
	// GUID信息
	unsigned int	has_guid;					// 是否为其产生全局唯一ID，以用于Trace。0-否，1-是
	// 处理方式
	unsigned int	proc_type;					// 是以下几种方式的组合: 死亡时是否掉落，是否可以扔在地上，是否可以卖给NPC，是人民币物品，是否可以玩家间交易，是否任务相关物品，装备后绑定，是否可解绑，离开场景消失
};

////////////////////////////////////////////////////////////////////////////////////////
// 宠物食品
////////////////////////////////////////////////////////////////////////////////////////
struct PET_FOOD_ESSENCE
{
	unsigned int	id;							// ID
	namechar		name[32];					// 名称, 最多15个汉字

	char			file_matter[128];			// 掉在地上的模型路径名
	char			file_icon[128];				// 图标路径名

	int				level;						// 等级

	int				hornor;						// 增加的忠诚度
	int				exp;						// 增加的经验值

	int				food_type;					// 食物所属类别Mask，从低到高依次为草料、肉类、野菜、水果、清水

	int				price;						// 卖店价
	int				shop_price;					// 店卖价

	// 堆叠信息
	int				pile_num_max;				// 该宠物食品的堆叠上限
	// GUID信息
	unsigned int	has_guid;					// 是否为其产生全局唯一ID，以用于Trace。0-否，1-是
	// 处理方式
	unsigned int	proc_type;					// 是以下几种方式的组合: 死亡时是否掉落，是否可以扔在地上，是否可以卖给NPC，是人民币物品，是否可以玩家间交易，是否任务相关物品，装备后绑定，是否可解绑，离开场景消失
};

////////////////////////////////////////////////////////////////////////////////////////
// 宠物美容卷
////////////////////////////////////////////////////////////////////////////////////////
struct PET_FACETICKET_ESSENCE
{
	unsigned int	id;							// ID
	namechar		name[32];					// 名称, 最多15个汉字

	char			file_matter[128];			// 掉在地上的模型路径名
	char			file_icon[128];				// 图标路径名

	int				price;						// 卖店价
	int				shop_price;					// 店卖价

	// 堆叠信息
	int				pile_num_max;				// 该宠物美容卷的堆叠上限
	// GUID信息
	unsigned int	has_guid;					// 是否为其产生全局唯一ID，以用于Trace。0-否，1-是
	// 处理方式
	unsigned int	proc_type;					// 是以下几种方式的组合: 死亡时是否掉落，是否可以扔在地上，是否可以卖给NPC，是人民币物品，是否可以玩家间交易，是否任务相关物品，装备后绑定，是否可解绑，离开场景消失
};

////////////////////////////////////////////////////////////////////////////////////////
// 烟花本体
////////////////////////////////////////////////////////////////////////////////////////
struct FIREWORKS_ESSENCE
{
	unsigned int	id;							// ID
	namechar		name[32];					// 名称, 最多15个汉字

	char			file_matter[128];			// 掉在地上的模型路径名
	char			file_icon[128];				// 图标路径名

	char			file_fw[128];				// 组合效果文件
	int				level;						// 等级

	int				time_to_fire;				// 燃放时间(秒)

	int				price;						// 卖店价
	int				shop_price;					// 店卖价

	// 堆叠信息
	int				pile_num_max;				// 该烟花的堆叠上限
	// GUID信息
	unsigned int	has_guid;					// 是否为其产生全局唯一ID，以用于Trace。0-否，1-是
	// 处理方式
	unsigned int	proc_type;					// 是以下几种方式的组合: 死亡时是否掉落，是否可以扔在地上，是否可以卖给NPC，是人民币物品，是否可以玩家间交易，是否任务相关物品，装备后绑定，是否可解绑，离开场景消失
};

////////////////////////////////////////////////////////////////////////////////////////
// 城战相关物品
////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
// 投石车招唤券
////////////////////////////////////////////////////////////////////////////////////////
struct WAR_TANKCALLIN_ESSENCE
{
	unsigned int	id;							// ID
	namechar		name[32];					// 名称, 最多15个汉字

	char			file_matter[128];			// 掉在地上的模型路径名
	char			file_icon[128];				// 图标路径名

	int				price;						// 卖店价
	int				shop_price;					// 店卖价

	// 堆叠信息
	int				pile_num_max;				// 该投石车招唤券的堆叠上限
	// GUID信息
	unsigned int	has_guid;					// 是否为其产生全局唯一ID，以用于Trace。0-否，1-是
	// 处理方式
	unsigned int	proc_type;					// 是以下几种方式的组合: 死亡时是否掉落，是否可以扔在地上，是否可以卖给NPC，是人民币物品，是否可以玩家间交易，是否任务相关物品，装备后绑定，是否可解绑，离开场景消失
};

// 技能书本体属性
struct SKILLMATTER_ESSENCE
{
	unsigned int	id;							// (类型)ID
	namechar		name[32];					// 名称, 最多15个汉字

	char			file_matter[128];			// 掉在地上的模型路径名
	char			file_icon[128];				// 图标路径名

	int				level_required;				// 等级限制

	int				id_skill;					// 技能ID
	int				level_skill;				// 技能等级

	int				price;						// 卖店价
	int				shop_price;					// 店卖价

	// 堆叠信息
	int				pile_num_max;				// 堆叠上限
	// GUID信息
	unsigned int	has_guid;					// 是否为其产生全局唯一ID，以用于Trace。0-否，1-是
	// 处理方式
	unsigned int	proc_type;					// 是以下几种方式的组合: 死亡时是否掉落，是否可以扔在地上，是否可以卖给NPC，是人民币物品，是否可以玩家间交易，是否任务相关物品，装备后绑定，是否可解绑，离开场景消失
};

struct INC_SKILL_ABILITY_ESSENCE
{
	unsigned int	id;							// (类型)ID
	namechar		name[32];					// 名称, 最多15个汉字
	
	char			file_matter[128];			// 掉在地上的模型路径名
	char			file_icon[128];				// 图标路径名
	
	int				id_skill;					// 技能ID
	int				level_required;				// 技能等级前提
	float           inc_ratio;					// 增加熟练度
	
	int				price;						// 卖店价
	int				shop_price;					// 店卖价
	
	// 堆叠信息
	int				pile_num_max;				// 堆叠上限
	// GUID信息
	unsigned int	has_guid;					// 是否为其产生全局唯一ID，以用于Trace。0-否，1-是
	// 处理方式
	unsigned int	proc_type;					// 是以下几种方式的组合: 死亡时是否掉落，是否可以扔在地上，是否可以卖给NPC，是人民币物品，是否可以玩家间交易，是否任务相关物品，装备后绑定，是否可解绑，离开场景消失
};

////////////////////////////////////////////////////////////////////////////////////////
// 婚礼预约凭证
////////////////////////////////////////////////////////////////////////////////////////
struct WEDDING_BOOKCARD_ESSENCE
{
	unsigned int	id;							// ID
	namechar		name[32];					// 名称, 最多15个汉字

	char			file_matter[128];			// 掉在地上的模型路径名
	char			file_icon[128];				// 图标路径名

	int				year;						// 预约时间（年）
	int				month;						// 预约时间（月）
	int				day;						// 预约时间（日）

	int				price;						// 卖店价
	int				shop_price;					// 店卖价

	// 堆叠信息
	int				pile_num_max;				// 堆叠上限
	// GUID信息
	unsigned int	has_guid;					// 是否为其产生全局唯一ID，以用于Trace。0-否，1-是
	// 处理方式
	unsigned int	proc_type;					// 是以下几种方式的组合: 死亡时是否掉落，是否可以扔在地上，是否可以卖给NPC，是人民币物品，是否可以玩家间交易，是否任务相关物品，装备后绑定，是否可解绑，离开场景消失
};

////////////////////////////////////////////////////////////////////////////////////////
// 婚礼请柬
////////////////////////////////////////////////////////////////////////////////////////
struct WEDDING_INVITECARD_ESSENCE
{
	unsigned int	id;							// ID
	namechar		name[32];					// 名称, 最多15个汉字

	char			file_matter[128];			// 掉在地上的模型路径名
	char			file_icon[128];				// 图标路径名
	
	int				price;						// 卖店价
	int				shop_price;					// 店卖价

	// 堆叠信息
	int				pile_num_max;				// 堆叠上限
	// GUID信息
	unsigned int	has_guid;					// 是否为其产生全局唯一ID，以用于Trace。0-否，1-是
	// 处理方式
	unsigned int	proc_type;					// 是以下几种方式的组合: 死亡时是否掉落，是否可以扔在地上，是否可以卖给NPC，是人民币物品，是否可以玩家间交易，是否任务相关物品，装备后绑定，是否可解绑，离开场景消失
};

////////////////////////////////////////////////////////////////////////////////////////
// 磨刀石
////////////////////////////////////////////////////////////////////////////////////////
struct SHARPENER_ESSENCE
{
	unsigned int	id;							// ID
	namechar		name[32];					// 名称, 最多15个汉字

	char			file_matter[128];			// 掉在地上的模型路径名
	char			file_icon[128];				// 图标路径名
	
	int				level;						// 级别

	unsigned int	equip_mask;					// 产生作用的逻辑部位

	int				addon[3];					// 附加属性的类型ID
	int				addon_time;					// 附加属性的有效时间（秒）
	
	int				gfx_index;					// 光效（0~15)
	
	int				price;						// 卖店价
	int				shop_price;					// 店卖价

	// 堆叠信息
	int				pile_num_max;				// 堆叠上限
	// GUID信息
	unsigned int	has_guid;					// 是否为其产生全局唯一ID，以用于Trace。0-否，1-是
	// 处理方式
	unsigned int	proc_type;					// 是以下几种方式的组合: 死亡时是否掉落，是否可以扔在地上，是否可以卖给NPC，是人民币物品，是否可以玩家间交易，是否任务相关物品，装备后绑定，是否可解绑，离开场景消失
};

////////////////////////////////////////////////////////////////////////////////////////
// 集结令
////////////////////////////////////////////////////////////////////////////////////////
struct CONGREGATE_ESSENCE
{
	unsigned int	id;							// ID
	namechar		name[32];					// 名称, 最多15个汉字

	char			file_matter[128];			// 掉在地上的模型路径名
	char			file_icon[128];				// 图标路径名	

	unsigned int congregate_type;				//	集结类型（队伍集结、帮派集结等）

	struct  
	{
		int		id;								//	可使用地图ID
		int		require_level;					//	使用者等级要求
		int		require_reply_level;			//	响应者等级要求
		int		require_reply_level2;			//	响应者修真要求
		int		require_reply_reincarnation_times;	//	响应者转生次数要求
	}area[8];
	
	int				price;						// 卖店价
	int				shop_price;					// 店卖价

	// 堆叠信息
	int				pile_num_max;				// 堆叠上限
	// GUID信息
	unsigned int	has_guid;					// 是否为其产生全局唯一ID，以用于Trace。0-否，1-是
	// 处理方式
	unsigned int	proc_type;					// 是以下几种方式的组合: 死亡时是否掉落，是否可以扔在地上，是否可以卖给NPC，是人民币物品，是否可以玩家间交易，是否任务相关物品，装备后绑定，是否可解绑，离开场景消失
};

////////////////////////////////////////////////////////////////////////////////////////
// 怪物元魂
////////////////////////////////////////////////////////////////////////////////////////
struct MONSTER_SPIRIT_ESSENCE
{
	unsigned int	id;							// ID
	namechar		name[32];					// 名称, 最多15个汉字
	
	char			file_matter[128];			// 掉在地上的模型路径名
	char			file_icon[128];				// 图标路径名

	int				type;						//	类型
	int				level;						//	等级
	int				power;						//	基础能量
	
	int				price;						// 卖店价
	int				shop_price;					// 店卖价
	int				pile_num_max;
	unsigned int	has_guid;
	unsigned int	proc_type;
};

///////////////////////////////////////////////////////////////////////////////////////
//
// 配方模板数据结构定义
//
//
///////////////////////////////////////////////////////////////////////////////////////

// 配方大类别
struct RECIPE_MAJOR_TYPE
{
	unsigned int	id;							// 配方大类别ID
	namechar		name[32];					// 配方大类别名称
};

// 配方小类别
struct RECIPE_SUB_TYPE
{
	unsigned int	id;							// 配方小类别ID
	namechar		name[32];					// 配方小类别名称
};

struct RECIPE_ESSENCE
{
	unsigned int	id;							// 配方本体ID
	unsigned int	id_major_type;				// 配方大类别ID
	unsigned int	id_sub_type;				// 配方小类别ID
	namechar		name[32];					// 配方本体名称

	int				recipe_level;				// 配方等级

	unsigned int	id_skill;					// 需要的技能号
	int				skill_level;				// 需要的技能等级
	int				bind_type;					// 天人合一类型

	struct 
	{
		unsigned int	id_to_make;				// 生成的东西ID
		float			probability;			// 生成的概率

	} targets[4];

	float			fail_probability;			// 生产失败概率
	int				num_to_make;				// 一次生产的个数：默认1
	int				price;						// 一次生产的价格：数字输入
	float			duration;					// 一次生产的时间：数字输入（float）

	int				exp;						// 每次生产获得的经验
	int				skillpoint;					// 每次生产获得的技能点

	struct
	{
		unsigned int	id;						// 原料id
		int				num;					// 原料所需数目

	}  materials[32];
	
	unsigned int	id_upgrade_equip;			// 要升级的装备ID
	float			upgrade_rate;				// 继承费用系数,默认为1，范围0.01-2

	// 处理方式
	unsigned int	proc_type;					// 是以下几种方式的组合: 死亡时是否掉落，是否可以扔在地上，是否可以卖给NPC，是人民币物品，是否可以玩家间交易，是否任务相关物品，装备后绑定，是否可解绑，离开场景消失

	unsigned int	character_combo_id;			//	职业限制组合：0为不限制
	float			engrave_upgrade_rate;		//	镌刻继承费用系数,默认为1，范围0.01-10
	float			addon_inherit_fee_rate;		//	附加属性继承费用系数：0.01~2
};

///////////////////////////////////////////////////////////////////////////////////////
//
// 镌刻模板数据结构定义
//
//
///////////////////////////////////////////////////////////////////////////////////////

// 镌刻大类别
struct ENGRAVE_MAJOR_TYPE
{
	unsigned int	id;						// 镌刻大类别ID
	namechar		name[32];				// 镌刻大类别名称
};

// 镌刻小类别
struct ENGRAVE_SUB_TYPE
{
	unsigned int	id;						// 镌刻小类别ID
	namechar		name[32];				// 镌刻小类别名称
};

struct ENGRAVE_ESSENCE
{
	unsigned int	id;						// 镌刻本体ID
	unsigned int	id_major_type;			// 镌刻大类别ID
	unsigned int	id_sub_type;			// 镌刻小类别ID
	namechar		name[32];				// 镌刻本体名称
	
	char				file_icon[128];		// 图标文件
	int					level;				// 等级
	
	unsigned int		equip_mask;			// 装备限制

	int					require_level_min;	//	适用品阶下限
	int					require_level_max;	//	适用品阶上限

	int					duration;			//	镌刻时间

	struct
	{
		unsigned int	id;					// 原料id
		int				num;				// 原料所需数目
	}  materials[8];
	
	float				probability_addon_num[ELEMENTDATA_MAX_ENGRAVE_ADDON_COUNT+1];	// 镌刻产生%条属性的机会

	struct
	{
		unsigned int	id;					// 附加属性的类型ID
		float			probability;		// 附加属性出现的概率
	} addons[32];
};

///////////////////////////////////////////////////////////////////////////////////////
//
// 属性值随机模板数据结构定义
//
//
///////////////////////////////////////////////////////////////////////////////////////

// 属性值随机类别
struct RANDPROP_TYPE
{
	unsigned int	id;								// 类别ID
	namechar		name[32];					// 类别名称
};

struct RANDPROP_ESSENCE
{
	unsigned int	id;								// 属性值随机本体ID
	unsigned int	id_type;					// 属性值随机类别ID
	namechar		name[32];					// 属性值随机本体名称
	
	unsigned int	id_skill;					// 需要的技能号
	int						skill_level;				// 需要的技能等级
	
	int					money;						//	所需金钱
	int					duration;						//	花费时间

	unsigned int equip_id[32];				//	装备ID列表
	
	struct
	{
		unsigned int	id;						// 原料id
		int				num;						// 原料所需数目
	} materials[8];
};


///////////////////////////////////////////////////////////////////////////////////////
//
// 宝石转化配方模板数据结构定义
//
//
///////////////////////////////////////////////////////////////////////////////////////

// 宝石转化配方类别
struct STONE_CHANGE_RECIPE_TYPE
{
	unsigned int	id;							// 类别ID
	namechar		name[32];					// 类别名称
};

// 宝石转化配方本体
struct STONE_CHANGE_RECIPE
{
	unsigned int	id;							// 本体ID
	unsigned int	id_type;					// 类别ID
	namechar		name[32];					// 本体名称
	
	unsigned int	id_old_stone;				// 转化前宝石
	unsigned int	id_new_stone;				// 转化后宝石
	
	int				money;						// 所需金钱

	struct
	{
		unsigned int	id;						// 转化需要原料id
		int				num;					// 转化需要原料数量
	}materials[8];
};

///////////////////////////////////////////////////////////////////////////////////////
//
// 怪物模板数据结构定义
//
//		注: 怪物只进行依据形象的小分类, 怪物的攻击策略则作为一个属性在个体中进行选择
//
//
///////////////////////////////////////////////////////////////////////////////////////

// 怪物附加属性
struct MONSTER_ADDON
{
	unsigned int	id;							// 此附加属性的ID与物品的附加属性不在一个空间
												// 附加属性的ID，在生成物品时要求第14、13位表示本
												// 附加属性的参数个数，第12位不能占用
												// 所以我们在产生ID时，不能使用12，13，14这三位

	namechar		name[32];					// 附加属性的名字

	int				num_params;					// 本附加属性的参数个数
	int				param1;						// 第1个参数，可以是浮点数
	int				param2;						// 第2个参数，可以是浮点数
	int				param3;						// 第3个参数，可以是浮点数
};

// 怪物类别
struct MONSTER_TYPE
{
	unsigned int	id;							// (类型)ID
	namechar		name[32];					// 名称, 最多15个汉字

	struct {
		unsigned int	id_addon;				// 怪物随机属性, 包括：飞毛腿、双重铠甲等
		float			probability_addon;		// 怪物随机属性出现的概率
	} addons[16];
};

// 怪物本体属性
struct MONSTER_ESSENCE
{
	unsigned int	id;							// 怪物(类型)ID
	unsigned int	id_type;					// 类别ID
	namechar		name[32];					// 名称, 最多15个汉字

	namechar		prop[16];					// 怪物属性, 最多16个汉字
	namechar		desc[16];					// 怪物描述, 最多16个汉字

	unsigned int	faction;					// 阵营组合代码
	unsigned int	monster_faction;			// 怪物的详细派系划分

	char			file_model[128];			// 模型路径名

	char			file_gfx_short[128];		// 物理攻击效果名称
	char			file_gfx_short_hit[128];	// 物理攻击击中效果名称
	
	float			size;						// 大小

	float			damage_delay;				// 怪物普攻伤害滞后时间

	unsigned int	id_strategy;				// 怪物攻击策略ID
												// 0 表示物理肉搏类怪物
												// 1 表示物理弓箭类怪物
												// 2 表示魔法类怪物
												// 3 表示肉搏＋远程类怪物
												// 4 堡垒类
												// 5 场景活物类
												// 6 树桩类
												// 7 堡垒魔法类
											
	unsigned int	role_in_war;				// 城战中所扮演的角色
												// 0 无；1 中心建筑；2 炮塔；3 箭塔；4 投石车；5 传送点；6 复活点；7 服务NPC；8 可被攻击的NPC

	int				level;						// 怪物等级
	int				show_level;					// 显示等级
	unsigned		id_pet_egg_captured;		// 捕获后变为的宠物蛋

	int				life;						// 生命值
	int				defence;					// 物防	

	int				magic_defences[5];			// 共5个是因为分金木水火土五系

	unsigned int	immune_type;				// 免疫类型，为Mask组合方式，各位的含义从低到高依次为：免疫物理攻击、免疫金系、免疫木系、免疫水系
												// 免疫火系、免疫土系、免疫昏迷、免疫睡眠、免疫减速、免疫定身、免疫封印、免疫伤害加倍、免疫击退、免疫HP上限下降，按比例减HP

	int				exp;						// 经验
	int				skillpoint;					// 技能点
					
	int 			money_average;				// 钱：标准值
	int				money_var;					// 钱：浮动值

	unsigned int	short_range_mode;			// 物理攻击近远程标志, 1-近程, 0-远程

	int				sight_range;				// 视野

	int				attack;						// 命中率
	int				armor;						// 回避率

	int				damage_min;					// 物理攻击: 最小攻击力
	int				damage_max;					// 物理攻击: 最大攻击力
	struct {
		int				damage_min;				// 物理攻击附带的五系魔法伤害值: 最小
		int				damage_max;				// 物理攻击附带的五系魔法伤害值: 最大
	} magic_damages_ext[5];

	float			attack_range;				// 物理攻击: 攻击距离
	float			attack_speed;				// 物理攻击: 攻击间隔

	int				magic_damage_min;			// 法术攻击: 最小攻击力
	int				magic_damage_max;			// 法术攻击: 最大攻击力

	unsigned int	id_skill;					// 技能号
	int				skill_level;				// 技能级别

	int				hp_regenerate;				// 回血速度（/秒）：慢速

	unsigned int	aggressive_mode;			// 主被动选择：0 - 被动；1 - 主动

	unsigned int	monster_faction_ask_help;	// 向哪些阵营求助，从怪物详细派系中选择
	unsigned int	monster_faction_can_help;	// 接受哪些阵营求助，从怪物详细派系中选择

	float			aggro_range;				// 仇恨距离
	float			aggro_time;					// 仇恨时间

	unsigned int	inhabit_type;				// 栖息地类别，可能的有：地面、水下、空中、地面加水下、地面加空中、水下加空中、海陆空
	unsigned int	patroll_mode;				// 巡逻方式，可能的有：随机走动，...
	unsigned int	stand_mode;					// 站立方式，可能的有：四足站立、双足站立、悬空飞行等

	float			walk_speed;					// 行走速度
	float			run_speed;					// 奔跑速度
	float			fly_speed;					// 飞行速度
	float			swim_speed;					// 游泳速度

	int				attack_degree;				// 攻击等级
	int				defend_degree;				// 防御等级

	unsigned int	common_strategy;			// 怪物策略

	struct
	{
		unsigned int	id;						// 仇恨度策略，可以是：普通，杀低级，杀血少之一
		float			probability;			// 仇恨度策略出现概率
	} aggro_strategy[4];

	// 三个条件技能
	struct 
	{
		unsigned int	id_skill;				// 怪物的条件技能类型ID	(HP<75%时使用的技能)
		int				level;					// 技能的级别
		float			probability;			// 该条件技能出现的概率
	} skill_hp75[5];

	struct
	{
		unsigned int	id_skill;				// 怪物的条件技能类型ID	(HP<50%时使用的技能)
		int				level;					// 技能的级别
		float			probability;			// 该条件技能出现的概率
	} skill_hp50[5];

	struct
	{
		unsigned int	id_skill;				// 怪物的条件技能类型ID	(HP<25%时使用的技能)
		int				level;					// 技能的级别
		float			probability;			// 该条件技能出现的概率
	} skill_hp25[5];
	
	unsigned int		after_death;			// 死亡后续：无-0、自爆-0x1、重生-0x2
	
	// 普通被动技能列表
	struct
	{
		unsigned int	id_skill;				// 怪物的普通技能类型ID
		int				level;					// 技能的级别
	} skills[32];							
	
	float			probability_drop_num0;		// 掉落的物品出现0个的机会
	float			probability_drop_num1;		// 掉落的物品出现1个的机会
	float			probability_drop_num2;		// 掉落的物品出现2个的机会
	float			probability_drop_num3;		// 掉落的物品出现3个的机会
	int				drop_times;					// 掉落次数(1-10)
	int				drop_protected;				// 掉落物品不保护

	struct {
		unsigned int	id;						// 掉落的物品
		float			probability;			// 掉落的概率
	} drop_matters[32];

	unsigned int		highest_frequency;		// 始终用最高频率执行策略
	int					no_accept_player_buff;
	
	int					invisible_lvl;			// 隐身等级
	int					uninvisible_lvl;		// 反隐等级
	
	int					no_auto_fight;			// 不自动进入战斗，0：否，即自动进入战斗。1：是，即会自动进入战斗
	int					fixed_direction;		// 朝向固定，0：否。1：怪物方向在布怪时决定，以后不再改动
	unsigned	int		id_building;			// 对应帮派基地设施
	
	unsigned	int		combined_switch;	//	各种组合开关（见MONSTER_COMBINED_SWITCH）
	
	int					hp_adjust_common_value;				//	血量调整全局变量ID
	int					defence_adjust_common_value;	//	防御调整全局变量ID
	int					attack_adjust_common_value;		//	攻击调整全局变量ID

	float				max_move_range;			//	最大追击距离
	unsigned	int		drop_for_no_profit_time;//	掉落不受无收益时间角色攻击影响：0：否 1：是

	int					drop_mine_condition_flag;	//	矿物掉落条件标记（匹配后再掉落）
	float				drop_mine_probability;		//	矿物掉落概率
	struct  
	{
		unsigned int	id;						//	矿物ID
		float			probability;			//	掉落概率
		int				life;					//	存在时长（秒）
	}drop_mines[4];

	namechar			label[16];				//	自定义标签
	unsigned int		domain_related;			//	是否领土相关 0-否，1-是
	int					local_var[3];			//	局部变量初始值
};

enum	MONSTER_COMBINED_SWITCH
{
	MCS_SUMMONER_ATTACK_ONLY	= 0x00000001,			//	只接受召唤者攻击
	MCS_FORBID_SELECTION		= 0x00000002,			//	玩家不可交互选中
	MCS_HIDE_IMMUNE				= 0x00000004,			//	头顶不显示免疫
	MCS_RECORD_DPS_RANK			= 0x00000008,			//	死亡记录DPS排行榜
};

///////////////////////////////////////////////////////////////////////////////////////
//
// 宠物模板数据结构定义
//
///////////////////////////////////////////////////////////////////////////////////////
// 宠物类别
struct PET_TYPE
{
	unsigned int	id;							// (类型)ID
	namechar		name[32];					// 名称, 最多15个汉字
};

// 宠物本体属性
struct PET_ESSENCE
{
	unsigned int	id;							// 怪物(类型)ID
	unsigned int	id_type;					// 类别ID
	namechar		name[32];					// 名称, 最多15个汉字

	char			file_model[128];			// 模型路径
	char			file_icon[128];				// 图标文件
	
	unsigned int	character_combo_id;			// 职业限制组合, 从低位到高位的含义为：0-武侠, 1-法师, 2-巫师, 3-妖精, 4-妖兽, 5-刺客, 6-羽芒, 7-羽灵, 8-剑灵, 9-魅灵
	
	int				level_max;					// 级别上限
	int				level_require;				// 等级要求

	unsigned int	pet_snd_type;				// 宠物脚步声音类型

	// 以下参数为与级别相关的系数
	float			hp_a;						// hp系数A
	float			hp_b;						// hp系数B
	float			hp_c;						// hp系数C

	float			hp_gen_a;					// hp恢复系数A
	float			hp_gen_b;					// hp恢复系数B
	float			hp_gen_c;					// hp恢复系数C

	float			damage_a;					// 物理攻击力系数A
	float			damage_b;					// 物理攻击力系数B
	float			damage_c;					// 物理攻击力系数C
	float			damage_d;					// 物理攻击力系数D

	float			speed_a;					// 速度系数A
	float			speed_b;					// 速度系数B

	float			attack_a;					// 命中度系数A
	float			attack_b;					// 命中度系数B
	float			attack_c;					// 命中度系数C

	float			armor_a;					// 闪避度系数A
	float			armor_b;					// 闪避度系数B
	float			armor_c;					// 闪避度系数C

	float			physic_defence_a;			// 物理防御系数A
	float			physic_defence_b;			// 物理防御系数B
	float			physic_defence_c;			// 物理防御系数C
	float			physic_defence_d;			// 物理防御系数D

	float			magic_defence_a;			// 法术防御系数A
	float			magic_defence_b;			// 法术防御系数B
	float			magic_defence_c;			// 法术防御系数C
	float			magic_defence_d;			// 法术防御系数D
	
	float			mp_a;						// mp系数A
	float			mp_gen_a;					// mp恢复系数A
	float			attack_degree_a;			// 攻击等级系数A
	float			defence_degree_a;			// 防御等级系数A
	
	// 以下属性为固定值
	float			size;						// 大小
	float			damage_delay;				// 怪物普攻伤害滞后时间
	float			attack_range;				// 物理攻击的攻击距离
	float			attack_speed;				// 物理攻击的攻击间隔
	int				sight_range;				// 视野

	unsigned int	food_mask;					// 可吃的食物Mask，从低到高依次为草料、肉类、野菜、水果、清水
	
	unsigned int	inhabit_type;				// 栖息地类别，可能的有：地面、水下、空中、地面加水下、地面加空中、水下加空中、海陆空
	unsigned int	stand_mode;					// 站立方式，可能的有：四足站立、双足站立、悬空飞行等

	unsigned int	plant_group;				// 植物宠种群类别，可能的有：普通、终极1、终极2
	int				group_limit;				// 植物宠种群规模
	
	unsigned int	immune_type;				// 免疫类型，为Mask组合方式，各位的含义从低到高依次为：免疫物理攻击、免疫金系、免疫木系、免疫水系
												// 免疫火系、免疫土系、免疫昏迷、免疫睡眠、免疫减速、免疫定身、免疫封印、免疫伤害加倍、免疫击退、免疫HP上限下降，按比例减HP
	
	int				player_gain_skill;			// 牺牲后玩家获得技能
	int				require_dye_count;			// 染色所需的染色剂数目

	unsigned int    id_pet_egg_evolved;			// 进化后宠物蛋
	int				cost_index;					// 在宠物进化配置表中的消耗配置

	int				hp_inherit_max_rate;		// 进化宠最大生命值继承系数(0~100)
	int				attack_inherit_max_rate;	// 进化宠最大攻击力继承系数(0~100)
	int				defence_inherit_max_rate;	// 进化宠最大防御力继承系数(0~100)
	int				attack_level_inherit_max_rate;	// 进化宠最大攻击等级继承系数(0~100)
	int				defence_level_inherit_max_rate;	// 进化宠最大防御等级继承系数(0~100)
	int				specific_skill;				//	进化宠专属技能

	char			file_gfx_short[128];		// 攻击效果名称（战宠、进化宠使用）
	unsigned int	id_evolved_skill_rand;		// 进化宠技能随机配置表
	
	unsigned int	combined_switch;			//  各种组合开关（见 PET_COMBINED_SWITCH ）
};

enum	PET_COMBINED_SWITCH
{
	PCS_USE_MASTER_MODEL	= 0x00000001,		//	使用召唤者模型
	PCS_FORBID_SELECTION	= 0x00000002,		//	玩家不可交互选中
	PCS_HIDE_IMMUNE			= 0x00000004,		//	头顶不显示免疫
	PCS_HIDE_NAME			= 0x00000008,		//	头顶不显示名字
};


///////////////////////////////////////////////////////////////////////////////////////
//
//	小精灵相关模板的数据结构定义
//
///////////////////////////////////////////////////////////////////////////////////////

//	小精灵本体
struct GOBLIN_ESSENCE
{
	unsigned int	id;							// (类型)ID
	namechar		name[32];					// 名称, 最多15个汉字

	char			file_model1[128];			// 状态1模型文件名
	char			file_model2[128];			// 状态2模型文件名
	char			file_model3[128];			// 状态3模型文件名
	char			file_model4[128];			// 状态4模型文件名
	char			file_matter[128];			// 掉在地上的模型路径名
	char			file_icon1[128];			// 状态1图标路径名
	char			file_icon2[128];			// 状态2图标路径名
	char			file_icon3[128];			// 状态3图标路径名
	char			file_icon4[128];			// 状态4图标路径名

	float			exp_factor;					// 升级经验值系数（与人物经验值的比例）
	int				init_strength;				// 初始力量
	int				init_agility;				// 初始敏捷
	int				init_energy;				// 初始灵力
	int				init_tili;					// 初始体力
	int				max_refine_lvl;				// 小精灵的精练上限

	int				price;						// 卖店价
	int				shop_price;					// 店卖价
	
	int				default_skill1;				// 初始技能1
	int				default_skill2;				// 初始技能2
	int				default_skill3;				// 初始技能3
	
	struct 
	{
		int			rand_num;					// 随机属性数值
		float		rand_rate;					// 随机属性概率
	}rand_prop[10];

	// 堆叠信息
	int				pile_num_max;				// 堆叠上限
	// GUID信息
	unsigned int	has_guid;					// 是否为其产生全局唯一ID，以用于Trace。0-否，1-是
	// 处理方式
	unsigned int	proc_type;					// 是以下几种方式的组合: 死亡时是否掉落，是否可以扔在地上，是否可以卖给NPC，是人民币物品，是否可以玩家间交易，是否任务相关物品，装备后绑定，是否可解绑，离开场景消失
};

//	小精灵装备类别
struct GOBLIN_EQUIP_TYPE
{
	unsigned int	id;							// (类型)ID
	namechar		name[32];					// 名称, 最多15个汉字
};

//	小精灵装备
struct GOBLIN_EQUIP_ESSENCE
{
	unsigned int	id;							// (类型)ID
	unsigned int	id_type;					// 类别ID
	namechar		name[32];					// 名称, 最多15个汉字

	char			file_model[128];			// 模型路径
	char			file_icon[128];				// 图标文件

	int				equip_type;					// 装备类型，0-玉；1-符；2-镜；3-珠
	int				req_goblin_level;			// 小精灵等级要求
	int				strength;					// 力量
	int				agility;					// 敏捷
	int				energy;						// 灵力
	int				tili;						// 体力
	int				magic[5];					// 五行属性
	
	int				price;						// 卖店价
	int				shop_price;					// 店卖价

	// 堆叠信息
	int				pile_num_max;				// 堆叠上限
	// GUID信息
	unsigned int	has_guid;					// 是否为其产生全局唯一ID，以用于Trace。0-否，1-是
	// 处理方式
	unsigned int	proc_type;					// 是以下几种方式的组合: 死亡时是否掉落，是否可以扔在地上，是否可以卖给NPC，是人民币物品，是否可以玩家间交易，是否任务相关物品，装备后绑定，是否可解绑，离开场景消失
};

//	小精灵经验丸
struct GOBLIN_EXPPILL_ESSENCE
{
	unsigned int	id;							// (类型)ID
	namechar		name[32];					// 名称, 最多15个汉字

	char			file_matter[128];			// 掉在地上的模型路径名
	char			file_icon[128];				// 图标路径名

	unsigned int	exp;						// 经验值
	int				level;						// 等级

	int				price;						// 卖店价
	int				shop_price;					// 店卖价

	// 堆叠信息
	int				pile_num_max;				// 堆叠上限
	// GUID信息
	unsigned int	has_guid;					// 是否为其产生全局唯一ID，以用于Trace。0-否，1-是
	// 处理方式
	unsigned int	proc_type;					// 是以下几种方式的组合: 死亡时是否掉落，是否可以扔在地上，是否可以卖给NPC，是人民币物品，是否可以玩家间交易，是否任务相关物品，装备后绑定，是否可解绑，离开场景消失
};

//	售卖凭证
struct SELL_CERTIFICATE_ESSENCE
{
	unsigned int	id;							// (类型)ID
	namechar		name[32];					// 名称, 最多15个汉字

	char            show_model[128];            // 摊位模型路径
	char			file_matter[128];			// 掉在地上的模型路径名
	char			file_icon[128];				// 图标路径名

	unsigned int	num_sell_item;				// 出售物品栏数量
	unsigned int	num_buy_item;				// 收购物品栏数量

	unsigned int	max_name_length;			// 摆摊名称长度限制

	int				price;						// 卖店价
	int				shop_price;					// 店卖价

	// 堆叠信息
	int				pile_num_max;				// 堆叠上限
	// GUID信息
	unsigned int	has_guid;					// 是否为其产生全局唯一ID，以用于Trace。0-否，1-是
	// 处理方式
	unsigned int	proc_type;					// 是以下几种方式的组合: 死亡时是否掉落，是否可以扔在地上，是否可以卖给NPC，是人民币物品，是否可以玩家间交易，是否任务相关物品，装备后绑定，是否可解绑，离开场景消失
	char            name_image[128];            // 摊位名称背景图片路径
};

//	对目标使用物品
struct TARGET_ITEM_ESSENCE
{
	unsigned int	id;							// (类型)ID
	namechar		name[32];					// 名称, 最多15个汉字

	char			file_matter[128];			// 掉在地上的模型路径名
	char			file_icon[128];				// 图标路径名

	int				num_use_pertime;			// 每次使用消耗物品数量,0表示不消耗，1表示消耗1个

	int				num_area;					// 可使用区域数量0-10
	int				area_id[10];					// 可使用区域ID

	unsigned int	id_skill;					// 技能ID
	int				skill_level;				// 技能等级

	int				use_in_combat;				// 可否在战斗中使用,1可以0不可以
	int				price;						// 卖店价
	int				shop_price;					// 店卖价

	// 堆叠信息
	int				pile_num_max;				// 堆叠上限
	// GUID信息
	unsigned int	has_guid;					// 是否为其产生全局唯一ID，以用于Trace。0-否，1-是
	// 处理方式
	unsigned int	proc_type;					// 是以下几种方式的组合: 死亡时是否掉落，是否可以扔在地上，是否可以卖给NPC，是人民币物品，是否可以玩家间交易，是否任务相关物品，装备后绑定，是否可解绑，离开场景消失

	unsigned int	use_in_sanctuary_only;		// 仅在安全区使用，0-否，1-是

	unsigned int	combined_switch;			// 各种组合开关（见 TARGET_ITEM_COMBINED_SWITCH ）
	int				target_id_for_pop[8];		// 选中此目标时才在屏幕上提示使用（为0时不限制）
	unsigned int	target_faction;				// 被使用目标阵营筛选
	int				require_level;				// 等级限制
};

enum	TARGET_ITEM_COMBINED_SWITCH
{
	TICS_CAN_POP			= 0x00000001,		//	在屏幕上提示使用
	TICS_TARGET_MY_FACTION_OBJECT	= 0x00000002,	//	只对本帮目标使用
};

//	查看目标属性道具
struct LOOK_INFO_ESSENCE
{
	unsigned int	id;							// (类型)ID
	namechar		name[32];					// 名称, 最多15个汉字

	char			file_matter[128];			// 掉在地上的模型路径名
	char			file_icon[128];				// 图标路径名

	int				price;						// 卖店价
	int				shop_price;					// 店卖价

	// 堆叠信息
	int				pile_num_max;				// 堆叠上限
	// GUID信息
	unsigned int	has_guid;					// 是否为其产生全局唯一ID，以用于Trace。0-否，1-是
	// 处理方式
	unsigned int	proc_type;					// 是以下几种方式的组合: 死亡时是否掉落，是否可以扔在地上，是否可以卖给NPC，是人民币物品，是否可以玩家间交易，是否任务相关物品，装备后绑定，是否可解绑，离开场景消失
};

///////////////////////////////////////////////////////////////////////////////////////
//
// NPC模板数据结构定义
//
//		注: NPC本体具有相同的数据结构，而NPC能够提供的服务则作为一个属性加入到NPC本体中，
//			具体服务相关的数据，如：卖东西时的物品列表等，则作为服务的属性存在服务模板中
//			
//		根据目前的了解，服务一共分以下几类：
//			0、基本交谈功能，并可由基本交谈中引出其他的服务，需要的数据：对话数据
//			1、出售物品，需要的数据：对话数据、可提供的物品列表
//			2、收购物品，需要的数据：对话数据
//			3、修理物品和装备，需要的数据：对话数据
//			4、给有眼的装备镶嵌饰品，需要的数据：对话数据、可以提供的饰品列表等
//			5、拆除已镶嵌在装备上的饰品，需要的数据：对话数据
//			6、发任务、验证任务、进行完成任务奖励等同任务相关的服务，需要的数据：对话数据、可以提供和处理的任务列表
//			7、教授一些技能，需要的数据：对话数据、可以教授的技能列表
//			8、对玩家进行治疗，需要的数据：对话数据
//			9、将玩家进行传送，需要的数据：对话数据，瞬移目标点，费用列表
//			10、为玩家提供交通工具运输服务，需要的数据：对话数据，交通工具及路线，价格
//			11、物品的代售，需要的数据：对话数据
//			12、为玩家存储物品、金钱，需要的数据：对话数据，手续费
//			13、生产、分解特定物品，需要的数据：对话数据、可生产，分解的物品列表，生产及分解物品的时间、价钱影响比例
//			14、向导功能，需要的数据：特定等级对应的对话，特殊区域对应的对话
//			15、猎杀红名/杀死玩家
//			16、杀死某几个帮派玩家
//			17、进入帮派基地：帮派功能NPC
//			18、道路通行验证人：检票员NPC
//
///////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////
//
// NPC服务模板数据结构定义
//
///////////////////////////////////////////////////////////////////////////////////////

// 交谈服务
struct NPC_TALK_SERVICE
{
	unsigned int	id;							// 服务(类型)ID
	namechar		name[32];					// 名称, 最多15个汉字	

	unsigned int	id_dialog;					// 对话ID
};

// 出售商品的服务
struct NPC_SELL_SERVICE
{
	unsigned int	id;							// 服务(类型)ID
	namechar		name[32];					// 名称, 最多15个汉字

	struct
	{
		namechar		page_title[8];			// 页的名字，最多7个汉字

		int				require_contrib;		// 累积贡献度限制

		unsigned int	require_force;		//	势力限制（0为无势力限制）
		int				require_force_reputation;	//	累积势力威望限制

		struct
		{
			unsigned int id;					// 商品id
			int		   	contrib_cost;				// 消耗贡献度
			int			force_contribution_cost;	//	消耗势力战功
		}goods[32];								// 可出售的商品列表

	} pages[8];									// 按页方式存储的商品列表

	unsigned int	id_dialog;					// 对话ID
};

// 收购商品的服务
struct NPC_BUY_SERVICE
{
	unsigned int	id;							// 服务(类型)ID
	namechar		name[32];					// 名称, 最多15个汉字	

	unsigned int	id_dialog;					// 对话ID
};

// 修理商品的服务
struct NPC_REPAIR_SERVICE
{
	unsigned int	id;							// 服务(类型)ID
	namechar		name[32];					// 名称, 最多15个汉字	

	unsigned int	id_dialog;					// 对话ID
};

// 镶嵌服务
struct NPC_INSTALL_SERVICE
{
	unsigned int	id;							// 服务(类型)ID
	namechar		name[32];					// 名称, 最多15个汉字	

	unsigned int	id_goods[32];				// 可镶嵌的商品列表

	unsigned int	id_dialog;					// 对话ID
};

// 拆除服务
struct NPC_UNINSTALL_SERVICE
{
	unsigned int	id;							// 服务(类型)ID
	namechar		name[32];					// 名称, 最多15个汉字	

	unsigned int	id_goods[32];				// 可镶嵌的商品列表

	unsigned int	id_dialog;					// 对话ID
};

// 发放任务服务
struct NPC_TASK_OUT_SERVICE
{
	unsigned int	id;							// 服务(类型)ID
	namechar		name[32];					// 名称, 最多15个汉字	
	
	unsigned int	storage_id;					//	库任务号
	unsigned int	storage_open_item;			//  库任务开启道具
	unsigned int	storage_refresh_item;		//  库任务刷新道具
	int				storage_refresh_per_day;	//	库任务每日可刷新次数
	int				storage_refresh_count_min;	//	库任务每次刷新任务数最小值
	int				storage_refresh_count_max;	//	库任务每次刷新任务数最大值
	int				storage_deliver_per_day;	//  库任务每日领取任务数上限,0-255

	unsigned int	id_tasks[256];				// 可以处理的任务列表
	unsigned int	storage_refresh_on_crossserver;	//	库任务跨服时是否刷新
};

// 验证完成任务服务 
struct NPC_TASK_IN_SERVICE
{
	unsigned int	id;							// 服务(类型)ID
	namechar		name[32];					// 名称, 最多15个汉字	

	unsigned int	id_tasks[256];				// 可以处理的任务列表
};

// 发放任务物品服务
struct NPC_TASK_MATTER_SERVICE
{
	unsigned int	id;							// 服务(类型)ID
	namechar		name[32];					// 名称, 最多15个汉字	

	struct
	{
		unsigned int	id_task;				// 可以处理的任务列表
		struct
		{
		unsigned int	id_matter;				// 任务相关物品id
		int				num_matter;				// 任务相关物品数目
		} taks_matters[4];

	} tasks[16];
};

// 教授相关服务
struct NPC_SKILL_SERVICE
{
	unsigned int	id;							// 服务(类型)ID
	namechar		name[32];					// 名称, 最多15个汉字	

	unsigned int	id_skills[256];				// 可以处理的技能列表

	unsigned int	id_dialog;					// 对话ID
};

// 治疗服务
struct NPC_HEAL_SERVICE
{
	unsigned int	id;							// 服务(类型)ID
	namechar		name[32];					// 名称, 最多15个汉字

	unsigned int	id_dialog;					// 对话ID
};

// 传送服务
struct NPC_TRANSMIT_SERVICE
{
	unsigned int	id;							// 服务(类型)ID
	namechar		name[32];					// 名称, 最多15个汉字

	int				num_targets;				// 目标点数目
	struct {
		int			idTarget;					// 目标驿站ID
		int			fee;						// 费用
		int			required_level;				// 等级要求
	} targets[32];

	unsigned int	id_dialog;					// 对话ID
};

// 运输服务
struct NPC_TRANSPORT_SERVICE
{
	unsigned int	id;							// 服务(类型)ID
	namechar		name[32];					// 名称, 最多15个汉字

	struct 
	{
		unsigned int	id;						// 路线ID
		unsigned int	fee;					// 费用

	} routes[32];

	unsigned int	id_dialog;					// 对话ID
};

// 代售服务
struct NPC_PROXY_SERVICE
{
	unsigned int	id;							// 服务(类型)ID
	namechar		name[32];					// 名称, 最多15个汉字

	unsigned int	id_dialog;					// 对话ID
};

// 存储物品、金钱
struct NPC_STORAGE_SERVICE
{
	unsigned int	id;							// 服务(类型)ID
	namechar		name[32];					// 名称, 最多15个汉字
};

// 生产服务
struct NPC_MAKE_SERVICE
{
	unsigned int	id;							// 服务(类型)ID
	namechar		name[32];					// 名称, 最多15个汉字

	unsigned int	id_make_skill;				// 可用于生产的技能id
	int				produce_type;				// 生产类型, 0 - 普通生产；1 - 合成；2 - 赌博; 3 - 升级生产; 4 - 可预览生产; 5 - 高级升级生产

	struct
	{
		namechar		page_title[8];			// 页的名字，最多7个汉字
		unsigned int	id_goods[32];			// 可生产的商品列表

	} pages[8];									// 按页方式存储的商品列表

};

// 分解服务
struct NPC_DECOMPOSE_SERVICE
{
	unsigned int	id;							// 服务(类型)ID
	namechar		name[32];					// 名称, 最多15个汉字

	unsigned int	id_decompose_skill;			// 可分解的技能id
};

// 鉴定服务
struct NPC_IDENTIFY_SERVICE
{
	unsigned int	id;							// 服务(类型)ID
	namechar		name[32];					// 名称, 最多15个汉字

	int				fee;						// 费用
};

// 城战炮塔建造服务
struct NPC_WAR_TOWERBUILD_SERVICE
{
	unsigned int	id;							// 服务(类型)ID
	namechar		name[32];					// 名称, 最多15个汉字

	struct 
	{
		int				id_in_build;				// 建造中的对象id
		int				id_buildup;					// 建造完成后的对象id
		int				id_object_need;				// 所需要的物品
		int				time_use;					// 建造所需的时间
		int				fee;						// 建造所需的费用

	} build_info[4];
};

// 洗点服务
struct NPC_RESETPROP_SERVICE
{
	unsigned int	id;							// 服务(类型)ID
	namechar		name[32];					// 名称, 最多15个汉字

	struct 
	{
		int				id_object_need;			// 所需要的物品
		int				strength_delta;			// “力”点数变化
		int				agility_delta;			// “敏”点数变化
		int				vital_delta;			// “命”点数变化
		int				energy_delta;			// “灵”点数变化

	} prop_entry[15];
};

// 宠物改名服务
struct NPC_PETNAME_SERVICE
{
	unsigned int	id;							// 服务(类型)ID
	namechar		name[32];					// 名称, 最多15个汉字

	int				id_object_need;				// 所需要的物品
	int				price;						// 所需要的钱
};

// 宠物学习技能服务
struct NPC_PETLEARNSKILL_SERVICE
{
	unsigned int	id;							// 服务(类型)ID
	namechar		name[32];					// 名称, 最多15个汉字

	unsigned int	id_skills[128];				// 可以处理的技能列表

	unsigned int	id_dialog;					// 对话ID
};

// 宠物遗忘技能服务
struct NPC_PETFORGETSKILL_SERVICE
{
	unsigned int	id;							// 服务(类型)ID
	namechar		name[32];					// 名称, 最多15个汉字

	int				id_object_need;				// 所需要的物品
	int				price;						// 所需要的钱
};

// 装备绑定服务
struct NPC_EQUIPBIND_SERVICE
{
	unsigned int	id;							// 服务(类型)ID
	namechar		name[32];					// 名称, 最多15个汉字

	int				id_object_need[4];				// 所需要的物品
	int				price;						// 所需要的钱
	
	unsigned int bind_type;			//	绑定方式(ITEM_BIND_TYPE)
};

enum	ITEM_BIND_TYPE
{
	ITEM_BIND_DEFAULT = 0x0,		//	天人合一绑定
	ITEM_BIND_WEBTRADE = 0x1,	//	寻宝网绑定（天人合一+可寻宝网交易）
};

// 装备销毁服务
struct NPC_EQUIPDESTROY_SERVICE
{
	unsigned int	id;							// 服务(类型)ID
	namechar		name[32];					// 名称, 最多15个汉字

	int				id_object_need;				// 所需要的物品
	int				price;						// 所需要的钱
};

// 装备解除销毁服务
struct NPC_EQUIPUNDESTROY_SERVICE
{
	unsigned int	id;							// 服务(类型)ID
	namechar		name[32];					// 名称, 最多15个汉字

	int				id_object_need;				// 所需要的物品
	int				price;						// 所需要的钱
};

// 镌刻服务
struct NPC_ENGRAVE_SERVICE
{
	unsigned int	id;							// 服务(类型)ID
	namechar		name[32];					// 名称, 最多15个汉字

	unsigned int	id_engrave[16];			//	镌刻模板id
};

// 属性值随机服务
struct NPC_RANDPROP_SERVICE
{
	unsigned int	id;							// 服务(类型)ID
	namechar		name[32];			// 名称, 最多15个汉字

	struct
	{
		namechar		page_title[8];		// 名称
		unsigned int	id_recipe;			// 配方ID
	} pages[8];										// 第%页

};

// 势力服务
struct NPC_FORCE_SERVICE
{
	unsigned int	id;							// 服务(类型)ID
	namechar		name[32];					// 名称, 最多15个汉字

	unsigned int	force_id;					//	势力ID
};

// NPC类型定义
struct NPC_TYPE
{
	unsigned int	id;							// (类型)ID
	namechar		name[32];					// 名称, 最多15个汉字	

};

///////////////////////////////////////////////////////////////////////////////////////
//
// NPC本体模板数据结构定义
//
///////////////////////////////////////////////////////////////////////////////////////
struct NPC_ESSENCE
{
	unsigned int	id;							// NPC(类型)ID
	namechar		name[32];					// 名称, 最多15个汉字

	unsigned int	id_type;					// NPC 类型, 从NPC_TYPE中选取

	float			refresh_time;				// 刷新时间

	unsigned int	attack_rule;				// 是否被攻击 0-不可被攻击，1-可被攻击攻击变粉名，2-可被攻击攻击变红名
	
	char			file_model[128];			// 模型路径名

	float			tax_rate;					// 税率, 默认值0.05
	
	unsigned int	id_src_monster;				// 基本属性来自的怪物ID，非零时表示使用该怪物属性替换本NPC的属性

	namechar		hello_msg[256];				// 开场白，是一个多行文本

	unsigned int	id_to_discover;				// 发现和传送服务的驿站ID

	unsigned int	domain_related;				// 是否领土相关 0-否，1-是

	// 服务列表
	unsigned int	id_talk_service;			// 交谈的服务ID
	unsigned int	id_sell_service;			// 出售商品的服务ID
	unsigned int	id_buy_service;				// 收购商品的服务ID
	unsigned int	id_repair_service;			// 修理商品的服务ID
	unsigned int	id_install_service;			// 安装镶嵌品的服务ID
	unsigned int	id_uninstall_service;		// 拆除镶嵌品的服务ID
	unsigned int	id_task_out_service;		// 任务相关的服务ID: 发放任务服务
	unsigned int	id_task_in_service;			// 任务相关的服务ID: 验证完成任务服务
	unsigned int	id_task_matter_service;		// 任务相关的服务ID: 发放任务物品服务
	unsigned int	id_skill_service;			// 教授技能的服务ID
	unsigned int	id_heal_service;			// 治疗的服务ID
	unsigned int	id_transmit_service;		// 传送的服务ID
	unsigned int	id_transport_service;		// 运输的服务ID
	unsigned int	id_proxy_service;			// 代售的服务ID
	unsigned int	id_storage_service;			// 仓库的服务ID
	unsigned int	id_make_service;			// 生产的服务ID
	unsigned int	id_decompose_service;		// 分解的服务ID
	unsigned int	id_identify_service;		// 鉴定的服务ID
	unsigned int	id_war_towerbuild_service;	// 城战炮塔建造的服务ID
	unsigned int	id_resetprop_service;		// 洗点服务ID
	unsigned int	id_petname_service;			// 宠物改名服务
	unsigned int	id_petlearnskill_service;	// 宠物学习技能服务
	unsigned int	id_petforgetskill_service;	// 宠物遗忘技能服务
	unsigned int	id_equipbind_service;		// 装备绑定服务
	unsigned int	id_equipdestroy_service;	// 装备销毁服务
	unsigned int	id_equipundestroy_service;	// 装备解除销毁服务
	unsigned int	id_goblin_skill_service;	// 小精灵技能学习服务
	unsigned int	combined_services;			// 简单服务组合：其中的每一位代表一个不需要参数的服务，可能的有：
												//		0：遗忘生产技能；1：打孔服务；2：空；3：驿站发现服务；4：帮派服务；5：整容服务；6：邮寄服务；7：拍卖服务；8：双倍经验打卡服务；
												//		9：孵化宠物蛋服务；10：还原宠物蛋服务；11：城战管理服务；12：离开战场服务；13：点卡寄售；14：装备升级服务；15：染色服务；16：扭转乾坤服务
												//		17: 小精灵洗属性点; 18: 小精灵洗技能点; 19: 小精灵遗忘技能; 20: 小精灵精炼服务; 21: 小精灵拆分服务; 22: 小精灵装备拆卸; 23: 修复损毁装备
												//		24: 交易平台服务；25: 仙魔转换服务；26: 结婚相关服务；27: 基地开启服务；28: 基地升级及上缴；29: 基地材料兑换; 30:无密码查看仓库；31:查看沙包排行榜
	unsigned int	id_mine;					// 附带的矿
	unsigned int	no_collision;				//	忽略此 NPC 的碰撞检测
	unsigned int	id_engrave_service;			//	镌刻服务
	unsigned int	id_randprop_service;		//	属性值随机服务
	unsigned int	combined_services2;			//	简单服务组合2: 0: 加入国战 1：离开国战战场 2：装备签名 3：加入跨服 4：返回原服 5：改名服务 6：魂石转化和替换 7：国王选举 8：寄卖服务 9：时装拆分 10：转生服务 11：礼品卡
												//					12：战车战场报名 13：卡牌转生 14：飞剑强化 15：开启帮派掠夺 16：开启专用乾坤袋商城 17：开启专用鸿利商城 18：变性服务
	unsigned int	id_force_service;			//	势力服务
	unsigned int	combined_switch;			//  各种组合开关（见 NPC_COMBINED_SWITCH ）
};

enum	NPC_COMBINED_SWITCH
{
	NCS_IGNORE_DISTANCE_CHECK	= 0x00000001,	//	服务无距离限制
};

///////////////////////////////////////////////////////////////////////////////////////
//
// 矿类别模板数据结构定义
//
///////////////////////////////////////////////////////////////////////////////////////
struct MINE_TYPE
{
	unsigned int	id;							// (类型)ID
	namechar		name[32];					// 名称, 最多15个汉字
};


///////////////////////////////////////////////////////////////////////////////////////
//
// 矿本体模板数据结构定义
//
///////////////////////////////////////////////////////////////////////////////////////
struct MINE_ESSENCE
{
	unsigned int	id;							// 矿(类型)ID
	unsigned int	id_type;					// 类别ID
	namechar		name[32];					// 名称, 最多15个汉字
						  
	unsigned int	level;						// 等级
	unsigned int	level_required;				// 等级限制
	unsigned int	id_equipment_required;		// 所需要的工具类型
	unsigned int	eliminate_tool;				// 消耗采掘用具(true or false)
	unsigned int	time_min;					// 采矿时间下限（秒）
	unsigned int	time_max;					// 采矿时间上限（秒）

	int				exp;						// 每次采集所获的经验
	int				skillpoint;					// 每次采集所获的技能点
	
	char			file_model[128];			// 模型路径名

	struct 
	{
		unsigned int	id;						// 原料ID 
		float			probability;			// 挖出概率 (总和为1)
		int				life;					// 存在时长（秒）
	} materials[16];							// 挖掘出的原料表

	int				num1;						// 原料数目1
	float			probability1;				// 概率1
	int				num2;						// 原料数目2
	float			probability2;				// 概率2

	unsigned int	task_in;					// 需要的任务id
	unsigned int	task_out;					// 采集后相关任务id

	unsigned int	uninterruptable;			// 采集过程不可中断, false-可以中断, true-不可中断

	struct 
	{
		unsigned int	id_monster;				// 产生怪物id
		int				num;					// 产生怪物数目
		float			radius;					// 产生半径（米）
		int			life_time;				//	存在时间（秒）
	} npcgen[4];

	struct 
	{
		unsigned int	monster_faction;		// 仇恨的怪物详细派系
		float			radius;					// 仇恨半径（米）
		int				num;					// 仇恨数值
	} aggros[1];

	unsigned int	permenent;					// 采集后矿物不消失，false-消失，true-不消失

	unsigned int	combined_switch;			// 各种组合开关（见 MINE_COMBINED_SWITCH ）

	int				max_gatherer;				//	同时采集最多人数
	int				mine_type;					//	矿物类型(0:默认，1:怪物魂魄)
	float			gather_dist;				//	采集距离
	float			material_gain_ratio;		//	采集到原料的概率

	unsigned int	gather_start_action_config;	//	采矿开始 PLAYER_ACTION_INFO_CONFIG ID 
	unsigned int	gather_loop_action_config;	//	采矿循环 PLAYER_ACTION_INFO_CONFIG ID
};

enum	MINE_COMBINED_SWITCH
{
	MCS_MINE_BELONG_TO_SOMEONE	= 0x00000001,	//	矿物是否归属某玩家
	MCS_MINE_BROADCAST_ON_GAIN	= 0x00000002,	//	采集成功是否广播
};

// 对话过程对象
struct talk_proc
{
	namechar			text[64];					// 对话的第一个窗口的提示文字，最多63个汉字

	struct option
	{
		unsigned int	id;							// 选项链接的子窗口或功能ID, 如果最高位为1表示是一个预定义的功能, 为0表示一个子窗口
		namechar		text[64];					// 选项链接的提示文字，最多63个汉字
		unsigned int	param;						// 选项相关的参数
	};

	struct window
	{
		unsigned int	id;							// 窗口ID, 最高位不能为1
		unsigned int	id_parent;					// 父窗口ID, 为-1表示根窗口

		int				talk_text_len;				// 对话文字的长度
		namechar * 		talk_text;					// 对话文字

		int				num_option;					// 选项数目
		option *		options;					// 选项列表

		window() { options = 0; }
		~window() { delete [] options; options = 0; delete [] talk_text; talk_text = 0; }

		int save(FILE * fp)
		{
			fwrite(&id, sizeof(id), 1, fp);
			fwrite(&id_parent, sizeof(id_parent), 1, fp);

			fwrite(&talk_text_len, sizeof(talk_text_len), 1, fp);
			fwrite(talk_text, sizeof(namechar), talk_text_len, fp);

			fwrite(&num_option, sizeof(num_option), 1, fp);
			fwrite(options, sizeof(option), num_option, fp);
			return 0;
		}

		int load(FILE * fp)
		{
			fread(&id, sizeof(id), 1, fp);
			fread(&id_parent, sizeof(id_parent), 1, fp);

			fread(&talk_text_len, sizeof(talk_text_len), 1, fp);
			talk_text = new namechar[talk_text_len];
			fread(talk_text, sizeof(namechar), talk_text_len, fp);

			fread(&num_option, sizeof(num_option), 1, fp);
			options = new option[num_option];
			fread(options, sizeof(option), num_option, fp);
			return 0;
		}
	};

	unsigned int		id_talk;				// 对话对象的ID

	int					num_window;				// 带对话文字的窗口个数
	window *			windows;				// 带对话文字的窗口
	
	talk_proc()
	{
		id_talk		= 0;
		num_window	= 0;
		windows		= 0;
	}

	~talk_proc()
	{
		delete [] windows;
		windows = 0;
		num_window = 0;
	}

	// 存盘读盘
	int save(FILE * fp)
	{
		fwrite(&id_talk, sizeof(id_talk), 1, fp);
		fwrite(text, sizeof(text), 1, fp);

		fwrite(&num_window, sizeof(num_window), 1, fp);
		for(int i=0; i<num_window; i++)
			windows[i].save(fp);
		
		return 0;
	}

	int load(FILE * fp)
	{
		fread(&id_talk, sizeof(id_talk), 1, fp);
		fread(text, sizeof(text), 1, fp);

		fread(&num_window, sizeof(num_window), 1, fp);
		windows = new window[num_window]; 
		for(int i=0; i<num_window; i++)
			windows[i].load(fp);

		return 0;
	}
};

///////////////////////////////////////////////////////////////////////////////////////
//
// 人脸个性化所需数据定义模板
//
///////////////////////////////////////////////////////////////////////////////////////

// 某部位的贴图配置方案数据定义
struct FACE_TEXTURE_ESSENCE
{
	unsigned int	id;						// 贴图方案(类型)ID
	namechar		name[32];				// 名称, 最多15个汉字

	char			file_base_tex[128];		// 基层的贴图文件,所有的部位贴图都有这个属性
	char			file_high_tex[128];		// 上层带alpha的贴图文件,除了Face和Nose外都有这个属性
	char			file_icon[128];			// 图标文件

	unsigned int	tex_part_id;			// 部位ID,表示此贴图配置方案是用于哪个部位的, 包括:0-脸、1-眼皮、2-眼影、3-眉、4-唇、5-鼻、6-眼珠、7-小胡子、8-头发、9-大胡子、10-法令
	unsigned int	character_combo_id;		// 职业限制组合, 从低位到高位的含义为：0-武侠, 1-法师, 2-巫师, 3-妖精, 4-妖兽, 5-刺客, 6-羽芒, 7-羽灵, 8-剑灵, 9-魅灵
	unsigned int	gender_id;				// 表明此贴图适用的性别信息,可能的有:0-男, 1-女
	unsigned int	visualize_id;			// 形象属性归类, 是mask组合方式，可能的有：性感、骨感、可爱、冷艳、清秀、俊朗、妖媚、野性、硬朗、阴险、猥琐
	unsigned int	user_data;				// 用户自定义数据
	unsigned int	facepill_only;			// 变形丸专用
	unsigned int    fashion_head_only;      // 时装头巾专用
};

// 某部位的形状数据定义
struct FACE_SHAPE_ESSENCE
{
	unsigned int	id;						// 形状(类型)ID
	namechar		name[32];				// 名称, 最多15个汉字

	char			file_shape[128];		// 形状数据文件
	char			file_icon[128];			// 图标文件

	unsigned int	shape_part_id;			// 部位ID,表示此贴图配置方案是用于哪个部位的, 包括:0-脸型、1-眼型、2-眉型、3-鼻头、4-鼻梁、5-上唇线、6-唇沟、7-下唇线、8-耳型
	unsigned int	character_combo_id;		// 职业限制组合, 从低位到高位的含义为：0-武侠, 1-法师, 2-巫师, 3-妖精, 4-妖兽, 5-刺客, 6-羽芒, 7-羽灵, 8-剑灵, 9-魅灵
	unsigned int	gender_id;				// 表明此形状适用的性别信息,可能的有:0-男, 1-女
	unsigned int	visualize_id;			// 形象属性归类, 是mask组合方式，可能的有：性感、骨感、可爱、冷艳、清秀、俊朗、妖媚、野性、硬朗、阴险、猥琐
	unsigned int	user_data;				// 用户自定义数据, 目前用于对同部位的各种形状数据进行进一步的分类
	unsigned int	facepill_only;			// 变形丸专用
};

// 脸部的表情所表达的情绪类别
struct FACE_EMOTION_TYPE
{
	unsigned int	id;						// 情绪(类型)ID
	namechar		name[32];				// 名称, 最多15个汉字

	char			file_icon[128];			// 图标文件
};

// 脸部的表情数据定义
struct FACE_EXPRESSION_ESSENCE
{
	unsigned int	id;						// 表情(类型)ID
	namechar		name[32];				// 名称, 最多15个汉字

	char			file_expression[128];	// 表情数据文件
	char			file_icon[128];			// 图标文件

	unsigned int	character_combo_id;		// 职业限制组合, 从低位到高位的含义为：0-武侠, 1-法师, 2-巫师, 3-妖精, 4-妖兽, 5-刺客, 6-羽芒, 7-羽灵, 8-剑灵, 9-魅灵
	unsigned int	gender_id;				// 表明此表情适用的性别信息,可能的有:0-男, 1-女
	unsigned int	emotion_id;				// 此表情所表达的情绪, 需要从FACE_EMOTION_TYPE中选取
};

// 备选头发模型数据定义
struct FACE_HAIR_ESSENCE
{
	unsigned int	id;						// 头发(类型)ID
	namechar		name[32];				// 名称, 最多15个汉字

	char			file_hair_skin[128];	// 头发用于同头顶相接的Skin文件
	char			file_hair_model[128];	// 头发的带辫子部分的模型文件
	
	char			file_icon[128];			// 图标路径

	unsigned int	character_combo_id;		// 职业限制组合, 从低位到高位的含义为：0-武侠, 1-法师, 2-巫师, 3-妖精, 4-妖兽, 5-刺客, 6-羽芒, 7-羽灵, 8-剑灵, 9-魅灵
	unsigned int	gender_id;				// 表明此头发适用的性别信息,可能的有:0-男, 1-女

	unsigned int	visualize_id;			// 形象属性归类, 是mask组合方式，可能的有：性感、骨感、可爱、冷艳、清秀、俊朗、妖媚、野性、硬朗、阴险、猥琐
	unsigned int	facepill_only;			// 变形丸专用
	
	unsigned int    fashion_head_only;      // 时装头巾专用
};

// 备选胡子模型数据定义
struct FACE_MOUSTACHE_ESSENCE
{
	unsigned int	id;						// 胡子(类型)ID
	namechar		name[32];				// 名称, 最多15个汉字

	char			file_moustache_skin[128];// 胡子和脸相接的Skin文件
	
	char			file_icon[128];			// 图标路径

	unsigned int	character_combo_id;		// 职业限制组合, 从低位到高位的含义为：0-武侠, 1-法师, 2-巫师, 3-妖精, 4-妖兽, 5-刺客, 6-羽芒, 7-羽灵, 8-剑灵, 9-魅灵
	unsigned int	gender_id;				// 表明此胡子适用的性别信息,可能的有:0-男, 1-女

	unsigned int	visualize_id;			// 形象属性归类, 是mask组合方式，可能的有：性感、骨感、可爱、冷艳、清秀、俊朗、妖媚、野性、硬朗、阴险、猥琐
	unsigned int	facepill_only;			// 变形丸专用
};

// 备选法令模型数据定义
struct FACE_FALING_ESSENCE
{
	unsigned int	id;						// 法令(类型)ID
	namechar		name[32];				// 名称, 最多15个汉字

	char			file_faling_skin[128];	// 法令和脸相接的Skin文件
	
	char			file_icon[128];			// 图标路径

	unsigned int	character_combo_id;		// 职业限制组合, 从低位到高位的含义为：0-武侠, 1-法师, 2-巫师, 3-妖精, 4-妖兽, 5-刺客, 6-羽芒, 7-羽灵, 8-剑灵, 9-魅灵
	unsigned int	gender_id;				// 表明此法令适用的性别信息,可能的有:0-男, 1-女

	unsigned int	visualize_id;			// 形象属性归类, 是mask组合方式，可能的有：性感、骨感、可爱、冷艳、清秀、俊朗、妖媚、野性、硬朗、阴险、猥琐
	unsigned int	facepill_only;			// 变形丸专用
};

// 附眼
struct FACE_THIRDEYE_ESSENCE
{
	unsigned int	id;						// 法令(类型)ID
	namechar		name[32];				// 名称, 最多15个汉字

	char			file_gfx[128];			// 特效文件
	
	char			file_icon[128];			// 图标路径

	unsigned int	character_combo_id;		// 职业限制组合, 从低位到高位的含义为：0-武侠, 1-法师, 2-巫师, 3-妖精, 4-妖兽, 5-刺客, 6-羽芒, 7-羽灵, 8-剑灵, 9-魅灵
	unsigned int	gender_id;				// 表明此法令适用的性别信息,可能的有:0-男, 1-女

	unsigned int	facepill_only;			// 变形丸专用
};

// 颜色选择图像文件数据定义
struct COLORPICKER_ESSENCE
{
	unsigned int	id;						// 颜色选择图像ID
	namechar		name[32];				// 名称，最多15个汉字

	char			file_colorpicker[128];	// 颜色图文件名

	unsigned int	color_part_id;			// 部位ID,表示此颜色选择图是用于哪个部位的, 包括:0-脸、1-眼影、2-眉毛、3-嘴唇、4-头发、5-眼珠、6-胡子
	unsigned int	character_combo_id;		// 职业限制组合, 从低位到高位的含义为：0-武侠, 1-法师, 2-巫师, 3-妖精, 4-妖兽, 5-刺客, 6-羽芒, 7-羽灵, 8-剑灵, 9-魅灵
	unsigned int	gender_id;				// 表明此胡子适用的性别信息,可能的有:0-男, 1-女
};

// 个性化数据定义
struct CUSTOMIZEDATA_ESSENCE
{
	unsigned int	id;						// 个性化数据ID
	namechar		name[32];				// 名称，最多15个汉字

	char			file_data[128];			// 个性化数据文件名
	char			file_icon[128];			// 图标文件

	unsigned int	character_combo_id;		// 职业限制组合, 从低位到高位的含义为：0-武侠, 1-法师, 2-巫师, 3-妖精, 4-妖兽, 5-刺客, 6-羽芒, 7-羽灵, 8-剑灵, 9-魅灵
	unsigned int	gender_id;				// 表明此数据文件适用的性别信息,可能的有:0-男, 1-女
};

///////////////////////////////////////////////////////////////////////////////////////
// 系统配置文件类模板
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
// 阵营敌对列表
///////////////////////////////////////////////////////////////////////////////////////
struct ENEMY_FACTION_CONFIG
{
	unsigned int	id;						// id
	namechar		name[32];				// 名称，最多15个汉字

	unsigned int	enemy_factions[32];		// 0 - "空缺"的敌对阵营列表
											// 1 - "人类普通"的敌对阵营列表
											// 2 - "兽人普通"的敌对阵营列表
											// 3 - "羽人普通"的敌对阵营列表
											// 4 - "汐人普通"的敌对阵营列表
											// 5 - "世界BOSS"的敌对阵营列表
											// 6 - "场景友好阵营"的敌对阵营列表
											// 7 - "场景敌对阵营"的敌对阵营列表

											// 8 - "灵族普通"的敌对阵营列表
											// 9 - "红名状态"的敌对阵营列表
											// 10 - "空缺"的敌对阵营列表
											// 11 - "城战攻方"的敌对阵营列表
											// 12 - "城战守方"的敌对阵营列表
											// 13 - "帮战攻方"的敌对阵营列表
											// 14 - "帮战守方"的敌对阵营列表
											// 15 - "切磋状态"的敌对阵营列表

											// 16 - "空缺"的敌对阵营列表
											// 17 - "仙"的敌对阵营列表
											// 18 - "魔"的敌对阵营列表
											// 19 - "空缺"的敌对阵营列表
											// 20 - "空缺"的敌对阵营列表
											// 21 - "普通NPC(功能、守卫)"的敌对阵营列表
											// 22 - "城战守卫"的敌对阵营列表
											// 23 - "特殊NPC"的敌对阵营列表

											// 24 - "空缺"的敌对阵营列表
											// 25 - "宠物"的敌对阵营列表
											// 26 - "召唤物"的敌对阵营列表
											// 27 - "空缺"的敌对阵营列表
											// 28 - "仙类怪物"的敌对阵营列表
											// 29 - "魔类怪物"的敌对阵营列表
											// 30 - "普通怪物"的敌对阵营列表
											// 31 - "空缺"的敌对阵营列表
};

///////////////////////////////////////////////////////////////////////////////////////
// 职业属性列表（通用属性， 可以统一的参数），象HP，MP，力，敏同具体角色相关的数据放在
//		了数据库中，这些参数的调整通过专门的同数据库关联的页面来进行
///////////////////////////////////////////////////////////////////////////////////////
struct CHARRACTER_CLASS_CONFIG
{
	unsigned int	id;						// id
	namechar		name[32];				// 名称，最多15个汉字

	unsigned int	character_class_id;		// 职业id, 可能的有:0-武侠, 1-法师, 2-巫师, 3-妖精, 4-妖兽, 5-刺客, 6-羽芒, 7-羽灵, 8-剑灵, 9-魅灵
	
	unsigned int	faction;				// 该职业的所属阵营
	unsigned int	enemy_faction;			// 该职业的敌对阵营

	float			attack_speed;			// 攻击时间间隔（秒）
	float			attack_range;			// 攻击距离
	int				hp_gen;					// hp 恢复速度
	int				mp_gen;					// mp 恢复速度

	float			walk_speed;				// 行走速度
	float			run_speed;				// 奔跑速度
	float			swim_speed;				// 游泳速度
	float			fly_speed;				// 飞行速度
	int				crit_rate;				// 重击率（%）
	
	int				vit_hp;					// 1点生命对应的hp
	int				eng_mp;					// 1点真气对应的mp
	int				agi_attack;				// 1点敏捷对应命中率
	int				agi_armor;				// 1点敏捷对应闪避率

	int				lvlup_hp;				// 每升一级所增长的hp
	int				lvlup_mp;				// 每升一级所增长的mp
	float			lvlup_dmg;				// 每升一级所增长的物理攻击力
	float			lvlup_magic;			// 每升一级所增长的魔法攻击力
	float			lvlup_defense;			// 每升一级所增长的防御力
	float			lvlup_magicdefence;		// 每升一级所增长的魔法防御力

	int				angro_increase;			// 一次普攻增加的怒气值
	float			spirit_adjust[ELEMENTDATA_NUM_POKER_TYPE];		// 各型命轮修正系数
	float			poker_adjust[ELEMENTDATA_NUM_POKER_TYPE];		//	卡牌基本属性调整系数
};

///////////////////////////////////////////////////////////////////////////////////////
// 数据修正表，对游戏中计算公式中某些参数的修正
///////////////////////////////////////////////////////////////////////////////////////
struct PARAM_ADJUST_CONFIG
{
	unsigned int	id;						// id
	namechar		name[32];				// 名称，最多15个汉字

	struct
	{
		int			level_diff;				// Player和怪物的级别差(Player-怪物)(当级别差为0时，表示此条失效)
		float		adjust_exp;				// 对杀怪获得经验的修正系数(0.0~1.0)
		float		adjust_sp;				// 对杀怪获得SP的修正系数(0.0~1.0)
		float		adjust_money;			// 对杀怪获得金钱掉落数目的修正系数(0.0~1.0)
		float		adjust_matter;			// 对杀怪掉落物品的概率的修正(0.0~1.0)
		float		adjust_attack;			// 对攻击力的等级惩罚系数(0.0~1.0)
	} level_diff_adjust[16];				// 级别差对杀怪计算修正			

	struct
	{
		float		adjust_exp;				// 对杀怪获得经验的修正系数(0.0~1.0)
		float		adjust_sp;				// 对杀怪获得SP的修正系数(0.0~1.0)
	} team_adjust[11];						// 组队杀怪时的计算修正
	
	struct
	{
		float		adjust_exp;				// 对杀怪获得经验的修正系数(0.0~1.0)
		float		adjust_sp;				// 对杀怪获得SP的修正系数(0.0~1.0)
	} team_profession_adjust[11];			// 队伍中职业个数计算修正

	float		dmg_adj_to_spec_atk_speed;			//	过速伤害修正系数
	float		atk_rate_adj_to_spec_atk_speed;	//	过速命中修正系数

	float		use_monster_spirit_adjust[10];	//	怪物元魂吸收系数（与玩家命轮等级差）
};

///////////////////////////////////////////////////////////////////////////////////////
// 玩家升级曲线表
///////////////////////////////////////////////////////////////////////////////////////
struct PLAYER_LEVELEXP_CONFIG
{
	unsigned int	id;						// id
	namechar		name[32];				// 名称，最多15个汉字

	int				exp[150];				// 150级每级升一级所需要的经验值
};

struct PLAYER_SECONDLEVEL_CONFIG
{
	unsigned int	id;						// id
	namechar		name[32];				// 名称，最多15个汉字

	float			exp_lost[256];			// 经验损失表
};

//	玩家转生配置表
struct  PLAYER_REINCARNATION_CONFIG
{
	unsigned int	id;
	namechar		name[32];

	struct  
	{
		int			require_item;			//	所需物品
		float		exp_premote;			//	经验提升比例
	}level[10];
};

//	玩家境界配置表
struct  PLAYER_REALM_CONFIG
{
	unsigned int	id;
	namechar		name[32];
	
	struct  
	{
		struct LEVEL
		{
			int			require_exp;		//	升至此级别所需境界经验
			int			vigour;				//	此处境界对应气魄
		}level[10];	//	第%级
	}list[10];		//	第%重境界
};

///////////////////////////////////////////////////////////////////////////////////////
// 基地配置表
///////////////////////////////////////////////////////////////////////////////////////
struct FACTION_FORTRESS_CONFIG
{
	unsigned int	id;						//	id
	namechar		name[32];				//	名称，最多15个汉字

	int	require_level;						//	帮派等级限制

	struct 
	{
		int			id;						//	id (type = all_type)
		int			count;					//	个数
	}require_item[8];						//	创建消耗物品%

	struct
	{
		int	exp;							//	升级所需经验
		int	tech_point;						//	升级获得科技点数
	}level[50];								//	第%级

	int	tech_point_cost[5][7];				//	科技升级花费科技点数

	int	init_building[20];					//	初始设施

	int controller_id[100];				//	激活的控制器%的ID

	struct
	{
		int id;										//	的ID
		int value;								//	的值
	}common_value[100];				//	全局变量%
};


///////////////////////////////////////////////////////////////////////////////////////
// 势力配置表
///////////////////////////////////////////////////////////////////////////////////////
struct FORCE_CONFIG
{
	unsigned int	id;						//	id
	namechar		name[32];			//	名称，最多15个汉字

	char					file_icon[128];		//	图标路径名
	unsigned long	color;					//	显示颜色
	namechar		desc[256];			//	介绍文字，最多255个汉字

	int					reputation_max;	//	威望上限
	int					contribution_max;	//	战功上限

	int					join_money_cost;				//	加入消耗金钱
	int					join_item_cost;					//	加入消耗物品id 

	int					quit_repution_cost;		//	退出扣减威望百分比
	int					quit_contribution_cost;	//	退出扣减战功百分比
};

///////////////////////////////////////////////////////////////////////////////////////
// 国战配置表
///////////////////////////////////////////////////////////////////////////////////////
struct COUNTRY_CONFIG
{
	unsigned int		id;					//	id
	namechar			name[32];			//	名称，最多15个汉字
	
	struct  
	{
		namechar		name[32];			//	国家名称
		char			file_icon[128];		//	图标路径
		unsigned long	color;				//	显示颜色
	}country[4];
	
	int					flag_controller_id[3];	//	旗帜对应控制器ID
	unsigned int		flag_mine_id;			//	旗帜对应矿ID
	
	float				attack_flag_goal[3];			//	攻方夺旗目标位置
	float				attack_flag_goal_radius;		//	攻方夺旗目标范围
	float				defence_flag_goal[3];			//	守方夺旗目标位置
	float				defence_flag_goal_radius;		//	守方夺旗目标范围

	struct  
	{
		int				controller_id;			//	控制器ID
		unsigned int	id;						//	怪物ID
		int				group;					//	组ID
	}attack_tower[9];	//	攻方塔
	
	struct  
	{
		int				controller_id;			//	控制器ID
		unsigned int	id;						//	怪物ID
		int				group;					//	组ID
	}defence_tower[9];	//	守方塔

	struct StrongHold
	{
		struct State
		{
			int				controller_id;		//	控制器ID
			unsigned int	mine_id;			//	矿ID
		}state[5];								//	状态
		float				pos[3];				//	位置
		float				radius;				//	范围
		float				mine_pos[3];		//	矿位置
	}stronghold[8];		//	据点
};

///////////////////////////////////////////////////////////////////////////////////////
// 经脉配置表
///////////////////////////////////////////////////////////////////////////////////////
struct MERIDIAN_CONFIG
{
	unsigned int	id;					//	id
	namechar		name[32];			//	名称，最多15个汉字
	
	struct
	{
		int			hp;					//	生命
		int			phy_damage;			//	物攻
		int			magic_damage;		//	法攻
		int			phy_defence;		//	物防
		int			magic_defence;		//	法防
	}prof_para[ELEMENTDATA_NUM_PROFESSION];			//	职业系数
};

///////////////////////////////////////////////////////////////////////////////////////
// GM活动开关配置表
///////////////////////////////////////////////////////////////////////////////////////
struct GM_ACTIVITY_CONFIG
{
	unsigned int		id;					//	id
	namechar			name[32];			//	名称，最多15个汉字

	int					openlist[8];		//	活动开启控制器
	int					closelist[8];		//	活动关闭控制器
	unsigned int		disabled;			//	对GM屏蔽
};

///////////////////////////////////////////////////////////////////////////////////////
// Touch商城
///////////////////////////////////////////////////////////////////////////////////////
struct TOUCH_SHOP_CONFIG
{
	unsigned int	id;							//	id
	namechar		name[32];					//	名称
	
	struct Page
	{
		namechar		title[8];				// 页的名字，最多7个汉字
		struct Item
		{
			unsigned int id;
			unsigned int num;
			unsigned int price;
			int expire_timelength;		//	存在时间（秒）
		}item[16];
	}page[8];
};

///////////////////////////////////////////////////////////////////////////////////////
// 代币兑换商城
///////////////////////////////////////////////////////////////////////////////////////
struct TOKEN_SHOP_CONFIG
{
	unsigned int	id;							//	id
	namechar		name[32];					//	名称

	unsigned int	item[64];					//	商城物品
};

///////////////////////////////////////////////////////////////////////////////////////
// 随机商城
///////////////////////////////////////////////////////////////////////////////////////
struct RAND_SHOP_CONFIG
{
	unsigned int	id;							//	id
	namechar		name[32];					//	名称

	int				first_buy_price;			//	首次购买价格
	int				first_buy_range;			//	首次购买可随机个数（从列表第1位开始）
	int				price;						//	后续价格
	
	struct
	{
		unsigned int	id;						//	id
		float			probability;			//	随机概率		
	}list[256];									//	待随机物品列表
};

///////////////////////////////////////////////////////////////////////////////////////
// 收益时间配置
///////////////////////////////////////////////////////////////////////////////////////
struct PROFIT_TIME_CONFIG
{
	unsigned int	id;							//	id
	namechar		name[32];					//	名称
	
	int				upper_limit;				//	收益时间上限（小时）
};

///////////////////////////////////////////////////////////////////////////////////////
// 简单称号
///////////////////////////////////////////////////////////////////////////////////////
struct TITLE_CONFIG
{
	unsigned int	id;							//	id
	namechar		name[32];					//	名称
	
	namechar		desc[256];					//	简单描述(type=single_text)
	namechar		condition[256];				//	获取条件描述(type=single_text)
	unsigned long	color;						//	显示颜色(type=color)
	unsigned int	after_name;					//	是否后置显示(type=bool)

	int				phy_damage;					//	增加物攻
	int				magic_damage;				//	增加法攻
	int				phy_defence;				//	增加物防
	int				magic_defence;				//	增加法防
	int				attack;						//	增加命中
	int				armor;						//	增加闪避

	unsigned int	broadcast;					//	是否广播(type=bool)
};

///////////////////////////////////////////////////////////////////////////////////////
// 复合称号
///////////////////////////////////////////////////////////////////////////////////////
struct COMPLEX_TITLE_CONFIG : public TITLE_CONFIG
{
	unsigned int	sub_titles[9];				//	子称号列表
};

///////////////////////////////////////////////////////////////////////////////////////
// 动作属性表，游戏中播放动作时所需要的信息
///////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////
// 玩家动作属性表，游戏中播放动作时所需要的信息
///////////////////////////////////////////////////////////////////////////////////////
struct PLAYER_ACTION_INFO_CONFIG
{
	unsigned int	id;						// id
	namechar		name[32];				// 动作名称，最多15个汉字（显示用）

	char			action_name[32];		// 逻辑动作名称，最多15个汉字，与逻辑动作一一对应
	
	char			action_prefix[32];		// 与动作名称对应的动作前缀
	struct 
	{
		char		suffix[32];				// 在某种武器持拿状态下的动作后缀

	} action_weapon_suffix[NUM_WEAPON_TYPE];		// 按顺序分别是单手短、双手短、双手长、
													// 双手短重、双手长重、软鞭、弓、弩、拳套、枪，空手，匕首，法宝 
													// 直接使用武器小类中的action_type进行索引即可，
	
	unsigned int	hide_weapon;			// 是否隐藏武器
};

///////////////////////////////////////////////////////////////////////////////////////
// 升级生产表
///////////////////////////////////////////////////////////////////////////////////////
struct UPGRADE_PRODUCTION_CONFIG
{
	unsigned int	id;						// id
	namechar		name[32];				// 动作名称，最多15个汉字（显示用）

	int				num_refine[12];			// 精炼等级对应继承费用基数
	int				num_weapon[2][20];		// 武器品阶和孔数对应继承费用基数
	int				num_armor[4][20];		// 防具品阶和孔数对应继承费用基数
	int				num_stone[20];			// 宝石等级对应继承费用基数
	int				num_engrave[ELEMENTDATA_MAX_ENGRAVE_ADDON_COUNT];	// 镌刻条数对应继承费用基数
	int				num_addon[ELEMENTDATA_MAX_INHERIT_ADDON_COUNT];		// 附加属性条数对应继承费用基数
};

///////////////////////////////////////////////////////////////////////////////////////
// 不可放入帐号仓库物品列表
///////////////////////////////////////////////////////////////////////////////////////
struct ACC_STORAGE_BLACKLIST_CONFIG
{
	unsigned int	id;						// id
	namechar		name[32];				// 名称，最多15个汉字

	unsigned int	blacklist[512];			// 不可放入帐号仓库物品ID
};

///////////////////////////////////////////////////////////////////////////////////////
// 聚灵套餐表
///////////////////////////////////////////////////////////////////////////////////////
struct MULTI_EXP_CONFIG
{
	unsigned int	id;						// id
	namechar		name[32];				// 名称，最多15个汉字
	
	struct
	{
		int	multi_time;					    // 聚灵时间（分钟）
		float rate;							// 聚灵时经验倍率
		int	item_count;					    // 消耗结丹石数目
		int	demulti_wait_time;				// 聚灵缓冲时间（分钟）
		int	demulti_time;					// 散灵时间（分钟）
	}choice[20];
};

///////////////////////////////////////////////////////////////////////////////////////
// 头发模型和纹理对应表
///////////////////////////////////////////////////////////////////////////////////////
struct FACE_HAIR_TEXTURE_MAP
{
	unsigned int	id;						// id
	namechar		name[32];				// 名称，最多15个汉字
	
	struct
	{
		int	model_id;					    // 模型ID
		int	texture_id;		     			// 纹理ID
	}hair_texture_map[128];
};

///////////////////////////////////////////////////////////////////////////////////////
// 结婚配置表
///////////////////////////////////////////////////////////////////////////////////////
struct WEDDING_CONFIG
{
	unsigned int	id;						// id
	namechar		name[32];				// 名称，最多15个汉字
	
	struct
	{
		int start_hour;						// 场次的开始时间（小时）
		int start_min;						// 场次的开始时间（分钟）
		int end_hour;						// 场次的结束时间（小时）
		int end_min;						// 场次的结束时间（分钟）
	}wedding_session[10];					// 结婚各场次对应的具体时辰

	struct
	{
		int year;							// 年份
		int month;							// 月份
		int day;							// 日子
	}reserved_day[20];						// 保留的各特殊日期

	struct 
	{
		namechar	name[16];				// 婚礼场景名称
		float		pos[3];					// 婚礼地点
	}wedding_scene[10];
};


///////////////////////////////////////////////////////////////////////////////////////
// 仙魔技能转换表
///////////////////////////////////////////////////////////////////////////////////////
struct GOD_EVIL_CONVERT_CONFIG
{
	unsigned int	id;						// id
	namechar		name[32];				// 名称，最多15个汉字
	
	int skill_map[128][2];					// 仙魔技能转换列表
};


///////////////////////////////////////////////////////////////////////////////////////
// 百科查找禁忌表
///////////////////////////////////////////////////////////////////////////////////////
struct WIKI_TABOO_CONFIG
{
	unsigned int	id;						// id
	namechar		name[32];		// 名称，最多15个汉字

	unsigned int	essence[512];				// 普通物品(ID_SPACE_ESSENCE)
	unsigned int	recipe[128];				// 配方(ID_SPACE_RECIPE)
	unsigned int	task[512];					// 任务
	unsigned int	skill[128];					// 技能
};


///////////////////////////////////////////////////////////////////////////////////////
// 玩家死亡掉落表
///////////////////////////////////////////////////////////////////////////////////////
struct PLAYER_DEATH_DROP_CONFIG
{
	unsigned int	id;							// id
	namechar		name[32];				// 名称

	unsigned int	itemlist[256];			// 玩家死亡掉落物品ID
};

///////////////////////////////////////////////////////////////////////////////////////
// 消费积分配置表
///////////////////////////////////////////////////////////////////////////////////////
struct CONSUME_POINTS_CONFIG
{
	unsigned int	id;							// id
	namechar		name[32];			// 名称

	struct  
	{
		unsigned int	id;						//	物品ID
		int					point;					//	获得积分值
	}list[1024];
};

///////////////////////////////////////////////////////////////////////////////////////
// 在线奖励配置表
///////////////////////////////////////////////////////////////////////////////////////
struct ONLINE_AWARDS_CONFIG
{
	unsigned int	id;							// id
	namechar		name[32];			// 名称

	int					max_time;						//	每日最长挂机时间（秒）
	struct
	{
		int					time;						//	充值增加奖励时间（秒）
		int					interval;					//	获取奖励时间间隔（秒）
		unsigned int	ids[8];						//	当前套餐充值消耗物品ID列表
		int					 exp[150];				//	当前套餐挂机时每间隔收获经验值
	}choice[4];
};

///////////////////////////////////////////////////////////////////////////////////////
// 时装武器配置表
///////////////////////////////////////////////////////////////////////////////////////
struct FASHION_WEAPON_CONFIG
{
	unsigned int	id;							//	id
	namechar		name[32];					//	名称

	unsigned int	action_mask[NUM_WEAPON_TYPE];//	时装武器可匹配动作
};

///////////////////////////////////////////////////////////////////////////////////////
// 宠物进化配置表
///////////////////////////////////////////////////////////////////////////////////////
struct PET_EVOLVE_CONFIG
{
	unsigned int	id;						//	id
	namechar		name[32];				//	名称
	
	struct  
	{
		int			num_evolve[3][2];		//	进化消耗物品
		int			num_inherit[3][2];		//	继承系数再随机消耗物品
		int			num_rand_skill[3][2];	//	技能再随机消耗物品
	}cost[5];
};

///////////////////////////////////////////////////////////////////////////////////////
// 进化宠技能配置表
///////////////////////////////////////////////////////////////////////////////////////
struct PET_EVOLVED_SKILL_CONFIG
{
	unsigned int	id;				//	id
	namechar		name[32];		//	名称

	struct
	{
		int		id;					//	技能ID
		int		level;				//	技能等级
	}skills[2];
};

///////////////////////////////////////////////////////////////////////////////////////
// 进化宠技能随机配置表
///////////////////////////////////////////////////////////////////////////////////////
struct PET_EVOLVED_SKILL_RAND_CONFIG
{
	unsigned int	id;						//	id
	namechar		name[32];				//	名称
		
	struct  
	{
		float		probability;			//	分组选中概率
		unsigned int list[30];				//	等概率性格技能列表
	}rand_skill_group[3];					//	进化宠技能分组
};

///////////////////////////////////////////////////////////////////////////////////////
// 自动任务显示配置表
///////////////////////////////////////////////////////////////////////////////////////
struct AUTOTASK_DISPLAY_CONFIG
{
	unsigned int	id;						//	id
	namechar		name[32];				//	名称
	
	struct  
	{
		int			task_id;				//	自动任务ID(type=task_type)
		char		file_icon[128];			//	图标路径名(type=path)
		char		file_gfx[128];			//	完成特效路径名(type=path)
		unsigned long	color;				//	文本颜色(type=color)
	}list[16];
};

///////////////////////////////////////////////////////////////////////////////////////
// 玩家命轮属性配置表
///////////////////////////////////////////////////////////////////////////////////////
struct PLAYER_SPIRIT_CONFIG
{
	unsigned int	id;						//	id
	namechar		name[32];				//	名称

	int				type;					//	类型
	
	int				hp;						// 满级HP
	int				damage;					// 满级物攻
	int				magic_damage;			// 满级法攻
	int				defence;				// 满级物防
	int				magic_defence[5];		// 满级法防
	int				vigour;					// 满级气魄

	struct  
	{
		int			require_level;			//	所需玩家历史最高等级
		int			require_power;			//	所需能量
		float		gain_ratio;				//	获得属性比例
	}list[10];
};

///////////////////////////////////////////////////////////////////////////////////////
// 历史阶段
///////////////////////////////////////////////////////////////////////////////////////
struct HISTORY_STAGE_CONFIG
{
	unsigned int	id;						//	id
	namechar		name[32];				//	名称
	
	namechar		desc[256];				//	本阶段描述
	int				progress_value_id;		//	进度存储位置
	int				progress_value_goal;	//	阶段目标值
};

///////////////////////////////////////////////////////////////////////////////////////
// 历史推进配置表
///////////////////////////////////////////////////////////////////////////////////////
struct HISTORY_ADVANCE_CONFIG
{
	unsigned int	id;						//	id
	namechar		name[32];				//	名称
	
	int				history_stage_id[32];	//	历史阶段ID
};

///////////////////////////////////////////////////////////////////////////////////////
// 自动组队配置表
///////////////////////////////////////////////////////////////////////////////////////
struct AUTOTEAM_CONFIG
{
	unsigned int	id;						//	id
	namechar		name[32];				//	名称

	char			file_icon[128];			//	图标路径名(type=path)
	namechar		desc[256];				//	介绍(type=text_type)

	unsigned int	type;					//	日常、副本、特殊

	int				require_level[2];		//	玩家最低/最高等级（0 为不限制）
	int				require_maxlevel[2];	//	玩家历史最高等级范围（0 为不限制）
	int				require_gender;			//	性别（0 男、1 女、2 不限制）
	int				require_num[2];			//	队伍最少/最多人数
	int				num_prof[ELEMENTDATA_NUM_PROFESSION];	//	各职业人数配置（-1 为不限制）
	int				require_level2;				//	玩家最低修真要求
	int				require_reincarnation_times[2];		//	最少/最多转生次数（0为不限制）
	int				require_realm_level[2];		//	最低/最高境界（0为不限制）
	int				require_soul_power;			//	最小魂力值（0为不限制）

	int				worldtag;				//	活动传送地图ID
	float			trans_pos[3];			//	活动传送位置
	int				worldtag_from[32];		//	可传送地图
	unsigned int	combined_switch;		//	各种组合开关（见 AUTOTEAM_COMBINED_SWITCH ）

	int				time_type;				//	时间类型（0：按周，1：按日）
	unsigned int	week;					//	按周：从低位到高位依次为周一至周日
	int				day[2][3];				//	按日：如[2009][10][01]-[2009][10][07]，年为0时表示每年，月、日为0时类似
	int				daytime[4];				//	每日：如[9][30]-[19][30]，全为0时表示全天

	int				award[3];				//	奖励物品(type=all_type)
	unsigned int	task[32];				//	关联任务(type=task_type)
};

enum AUTOTEAM_COMBINED_SWITCH
{
	ACS_DISABLED	=	0x00000001,			//	此配置当前禁用
	ACS_RECOMMENDED	=	0x00000002,			//	推荐配置
};

///////////////////////////////////////////////////////////////////////////////////////
// 战车配置表
///////////////////////////////////////////////////////////////////////////////////////
struct CHARIOT_CONFIG
{
	unsigned int	id;						//	id
	namechar		name[32];				//	名称

	char			file_icon[128];			//	图标路径名(type=path)
	namechar		desc[256];				//	介绍(type=text_type)

	int				level;					//	等级

	int				shape;					//	变身id

	int				hp;						//	增加生命值
	float			hp_inc_ratio;			//	生命增加百分比
	int				defence;				//	增加物理防御
	int				magic_defences[5];		//	增加法术防御
	int				damage;					//	增加物理攻击
	int				magic_damage;			//	增加法术攻击
	float			speed;					//	增加移动速度

	int				skill[4];				//	附加技能

	int				pre_chariot;			//	前提战车
	int				upgrade_cost;			//	升级消耗能量
};


///////////////////////////////////////////////////////////////////////////////////////
// 战车战场配置
///////////////////////////////////////////////////////////////////////////////////////
struct CHARIOT_WAR_CONFIG
{
	unsigned int	id;						//	id
	namechar		name[32];				//	名称

	struct  
	{
		unsigned int	id;						//	ID
		int				power;					//	增加能量值
	}mines[3];		//	能量矿
};


///////////////////////////////////////////////////////////////////////////////////////
// 卡牌升级配置表
///////////////////////////////////////////////////////////////////////////////////////
struct POKER_LEVELEXP_CONFIG
{
	unsigned int	id;						//	id
	namechar		name[32];				//	名称
	
	int		exp[ELEMENTDATA_MAX_POKER_LEVEL];		//	第%级升级所需经验
	float	exp_adjust[ELEMENTDATA_NUM_POKER_RANK];	//	第%品阶升级经验值调整系数
};


///////////////////////////////////////////////////////////////////////////////////////
// GT配置表
///////////////////////////////////////////////////////////////////////////////////////
struct GT_CONFIG
{
	unsigned int	id;						//	id
	namechar		name[32];				//	名称
	
	int				inc_attack_degree;		// 增加攻击等级
	int				inc_defend_degree;		// 增加防御等级
};


///////////////////////////////////////////////////////////////////////////////////////
// 帮派掠夺战配置表
///////////////////////////////////////////////////////////////////////////////////////
struct FACTION_PVP_CONFIG
{
	unsigned int	id;						//	id
	namechar		name[32];				//	名称

	struct 
	{
		int		domain_count;				//	帮派领土数量参考值
		int		minebase_gen_count;			//	可产出资源基地数量
		int		points_per_gen_minecar;		//	资源基地产出每辆资源车奖励
		int		base_points;				//	每周保底奖励
		int		minecar_count_can_rob;		//	可掠夺资源车数
		int		minebase_count_can_rob;		//	可掠夺资源基地数
	}list[8];

	int		controller_id[ELEMENTDATA_DOMAIN_COUNT];	//	领土%控制器ID
};

///////////////////////////////////////////////////////////////////////////////////////
// 任务列表配置表
///////////////////////////////////////////////////////////////////////////////////////
struct TASK_LIST_CONFIG
{
	unsigned int	id;							// 服务(类型)ID
	namechar		name[32];					// 名称, 最多15个汉字
	
	unsigned int	id_tasks[256];				// 任务列表
};

///////////////////////////////////////////////////////////////////////////////////////
// 权重式任务随机配置表
///////////////////////////////////////////////////////////////////////////////////////
struct TASK_DICE_BY_WEIGHT_CONFIG
{
	unsigned int	id;							// 服务(类型)ID
	namechar		name[32];					// 名称, 最多15个汉字

	unsigned int	storage_id;					// 库任务号（与 NPC_TASK_OUT_SERVICE::storage_id 同空间）

	int				max_weight;					// 总权重

	struct 
	{
		float			probability;			// 概率
		int				weight;					// 权重
		unsigned int	task_list_config_id;	// 配置表ID ( TASK_LIST_CONFIG 类型)
	}uniform_weight_list[10];					// 同权重任务列表%
	
	unsigned int	other_task_list_config_id;	// 其它任务列表配置表 ( TASK_LIST_CONFIG 类型)
	unsigned int	storage_refresh_on_crossserver;	//	跨服时是否刷新
};

///////////////////////////////////////////////////////////////////////////////////////
// 时装最佳色配置表
///////////////////////////////////////////////////////////////////////////////////////
struct FASHION_BEST_COLOR_CONFIG
{
	unsigned int	id;							// 服务(类型)ID
	namechar		name[32];					// 名称, 最多15个汉字

	struct  
	{
		unsigned int	id;						//	时装ID
		unsigned long	color;					//	最佳颜色
	}list[64];
};

///////////////////////////////////////////////////////////////////////////////////////
// 签到奖励配置表
///////////////////////////////////////////////////////////////////////////////////////
struct SIGN_AWARD_CONFIG
{
	unsigned int	id;							// 服务(类型)ID
	namechar		name[32];					// 名称, 最多15个汉字	
	struct  
	{
		unsigned int	id;						//	奖励物品ID
		int				num;					//	奖励物品数量
	}list[31];									//	签到累计%天
};

enum ID_SPACE
{
	ID_SPACE_ESSENCE	= 0,
	ID_SPACE_ADDON		= 1,
	ID_SPACE_TALK		= 2,
	ID_SPACE_FACE		= 3,
	ID_SPACE_RECIPE		= 4,
	ID_SPACE_CONFIG		= 5,
};

enum DATA_TYPE
{
	DT_INVALID					= 0,
	DT_EQUIPMENT_ADDON			,
	DT_WEAPON_MAJOR_TYPE		,
	DT_WEAPON_SUB_TYPE			,
	DT_WEAPON_ESSENCE			,

	DT_ARMOR_MAJOR_TYPE			,	
	DT_ARMOR_SUB_TYPE			,
	DT_ARMOR_ESSENCE			,
	DT_DECORATION_MAJOR_TYPE	,
	DT_DECORATION_SUB_TYPE		,

	DT_DECORATION_ESSENCE		= 10,
	DT_MEDICINE_MAJOR_TYPE		,
	DT_MEDICINE_SUB_TYPE		,
	DT_MEDICINE_ESSENCE			,
	DT_MATERIAL_MAJOR_TYPE		,

	DT_MATERIAL_SUB_TYPE		,
	DT_MATERIAL_ESSENCE			,
	DT_DAMAGERUNE_SUB_TYPE		,
	DT_DAMAGERUNE_ESSENCE		,
	DT_ARMORRUNE_SUB_TYPE		,
	
	DT_ARMORRUNE_ESSENCE		= 20,
	DT_SKILLTOME_SUB_TYPE		,
	DT_SKILLTOME_ESSENCE		,
	DT_FLYSWORD_ESSENCE			,
	DT_WINGMANWING_ESSENCE		,

	DT_TOWNSCROLL_ESSENCE		,
	DT_UNIONSCROLL_ESSENCE		,
	DT_REVIVESCROLL_ESSENCE		,
	DT_ELEMENT_ESSENCE			,		
	DT_TASKMATTER_ESSENCE		,

	DT_TOSSMATTER_ESSENCE		= 30,
	DT_PROJECTILE_TYPE			,
	DT_PROJECTILE_ESSENCE		,
	DT_QUIVER_SUB_TYPE			,
	DT_QUIVER_ESSENCE			,

	DT_STONE_SUB_TYPE			,
	DT_STONE_ESSENCE			,
	DT_MONSTER_ADDON			,		
	DT_MONSTER_TYPE				,
	DT_MONSTER_ESSENCE			,	

	DT_NPC_TALK_SERVICE			= 40,
	DT_NPC_SELL_SERVICE			,
	DT_NPC_BUY_SERVICE			,
	DT_NPC_REPAIR_SERVICE		,
	DT_NPC_INSTALL_SERVICE		,

	DT_NPC_UNINSTALL_SERVICE	,
	DT_NPC_TASK_IN_SERVICE		,
	DT_NPC_TASK_OUT_SERVICE		,
	DT_NPC_TASK_MATTER_SERVICE	,
	DT_NPC_SKILL_SERVICE		,
	
	DT_NPC_HEAL_SERVICE			= 50,
	DT_NPC_TRANSMIT_SERVICE		,
	DT_NPC_TRANSPORT_SERVICE	,
	DT_NPC_PROXY_SERVICE		,
	DT_NPC_STORAGE_SERVICE		,

	DT_NPC_MAKE_SERVICE			,
	DT_NPC_DECOMPOSE_SERVICE	,
	DT_NPC_TYPE					,
	DT_NPC_ESSENCE				,
	DT_TALK_PROC				,

	DT_FACE_TEXTURE_ESSENCE		= 60,
	DT_FACE_SHAPE_ESSENCE		,
	DT_FACE_EMOTION_TYPE		,
	DT_FACE_EXPRESSION_ESSENCE	,
	DT_FACE_HAIR_ESSENCE		,

	DT_FACE_MOUSTACHE_ESSENCE	,
	DT_COLORPICKER_ESSENCE		,
	DT_CUSTOMIZEDATA_ESSENCE	,
	DT_RECIPE_MAJOR_TYPE		,
	DT_RECIPE_SUB_TYPE			,

	DT_RECIPE_ESSENCE			= 70,	
	DT_ENEMY_FACTION_CONFIG		,
	DT_CHARRACTER_CLASS_CONFIG	,
	DT_PARAM_ADJUST_CONFIG		,
	DT_PLAYER_ACTION_INFO_CONFIG,

	DT_TASKDICE_ESSENCE			= 75,
	DT_TASKNORMALMATTER_ESSENCE	,
	DT_FACE_FALING_ESSENCE		,
	DT_PLAYER_LEVELEXP_CONFIG	,
	DT_MINE_TYPE,

	DT_MINE_ESSENCE				= 80,
	DT_NPC_IDENTIFY_SERVICE		,
	DT_FASHION_MAJOR_TYPE		,	
	DT_FASHION_SUB_TYPE			,
	DT_FASHION_ESSENCE			,

	DT_FACETICKET_MAJOR_TYPE	= 85,
	DT_FACETICKET_SUB_TYPE,
	DT_FACETICKET_ESSENCE,
	DT_FACEPILL_MAJOR_TYPE,
	DT_FACEPILL_SUB_TYPE,

	DT_FACEPILL_ESSENCE			= 90,
	DT_SUITE_ESSENCE,
	DT_GM_GENERATOR_TYPE,
	DT_GM_GENERATOR_ESSENCE,
	DT_PET_TYPE,

	DT_PET_ESSENCE				= 95,
	DT_PET_EGG_ESSENCE,
	DT_PET_FOOD_ESSENCE,
	DT_PET_FACETICKET_ESSENCE,
	DT_FIREWORKS_ESSENCE,

	DT_WAR_TANKCALLIN_ESSENCE	= 100,
	DT_NPC_WAR_TOWERBUILD_SERVICE,
	DT_PLAYER_SECONDLEVEL_CONFIG,
	DT_NPC_RESETPROP_SERVICE,
	DT_NPC_PETNAME_SERVICE,

	DT_NPC_PETLEARNSKILL_SERVICE  = 105,
	DT_NPC_PETFORGETSKILL_SERVICE,
	DT_SKILLMATTER_ESSENCE,
	DT_REFINE_TICKET_ESSENCE,
	DT_DESTROYING_ESSENCE,

	DT_NPC_EQUIPBIND_SERVICE	  = 110,
	DT_NPC_EQUIPDESTROY_SERVICE,
	DT_NPC_EQUIPUNDESTROY_SERVICE,
	DT_BIBLE_ESSENCE,
	DT_SPEAKER_ESSENCE,			

	DT_AUTOHP_ESSENCE			  = 115,		
	DT_AUTOMP_ESSENCE,
	DT_DOUBLE_EXP_ESSENCE,
	DT_TRANSMITSCROLL_ESSENCE,
	DT_DYE_TICKET_ESSENCE,

	DT_GOBLIN_ESSENCE			= 120,
	DT_GOBLIN_EQUIP_TYPE,
	DT_GOBLIN_EQUIP_ESSENCE,
	DT_GOBLIN_EXPPILL_ESSENCE,
	DT_SELL_CERTIFICATE_ESSENCE,

	DT_TARGET_ITEM_ESSENCE		= 125,
	DT_LOOK_INFO_ESSENCE,
	DT_UPGRADE_PRODUCTION_CONFIG,
	DT_ACC_STORAGE_BLACKLIST_CONFIG,
	DT_FACE_HAIR_TEXTURE_MAP,

	DT_MULTI_EXP_CONFIG         = 130,
	DT_INC_SKILL_ABILITY_ESSENCE,
	DT_GOD_EVIL_CONVERT_CONFIG,
	DT_WEDDING_CONFIG,
	DT_WEDDING_BOOKCARD_ESSENCE,

	DT_WEDDING_INVITECARD_ESSENCE	=	135,
	DT_SHARPENER_ESSENCE,
	DT_FACE_THIRDEYE_ESSENCE,
	DT_FACTION_FORTRESS_CONFIG,
	DT_FACTION_BUILDING_SUB_TYPE,

	DT_FACTION_BUILDING_ESSENCE	=	140,
	DT_FACTION_MATERIAL_ESSENCE,
	DT_CONGREGATE_ESSENCE,
	DT_ENGRAVE_MAJOR_TYPE,
	DT_ENGRAVE_SUB_TYPE,

	DT_ENGRAVE_ESSENCE	= 145,
	DT_NPC_ENGRAVE_SERVICE,
	DT_NPC_RANDPROP_SERVICE,
	DT_RANDPROP_TYPE,
	DT_RANDPROP_ESSENCE,

	DT_WIKI_TABOO_CONFIG = 150,
	DT_FORCE_CONFIG,
	DT_FORCE_TOKEN_ESSENCE,
	DT_NPC_FORCE_SERVICE,
	DT_PLAYER_DEATH_DROP_CONFIG,

	DT_DYNSKILLEQUIP_ESSENCE = 155,
	DT_CONSUME_POINTS_CONFIG,
	DT_ONLINE_AWARDS_CONFIG,
	DT_COUNTRY_CONFIG,
	DT_GM_ACTIVITY_CONFIG,

	DT_FASHION_WEAPON_CONFIG = 160,
	DT_PET_EVOLVE_CONFIG,
	DT_PET_EVOLVED_SKILL_CONFIG,
	DT_MONEY_CONVERTIBLE_ESSENCE,
	DT_STONE_CHANGE_RECIPE_TYPE,

	DT_STONE_CHANGE_RECIPE = 165,
	DT_MERIDIAN_CONFIG,
	DT_PET_EVOLVED_SKILL_RAND_CONFIG,
	DT_AUTOTASK_DISPLAY_CONFIG,
	DT_TOUCH_SHOP_CONFIG,

	DT_TITLE_CONFIG,
	DT_COMPLEX_TITLE_CONFIG,
	DT_MONSTER_SPIRIT_ESSENCE,
	DT_PLAYER_SPIRIT_CONFIG,
	DT_PLAYER_REINCARNATION_CONFIG,

	DT_HISTORY_STAGE_CONFIG,
	DT_HISTORY_ADVANCE_CONFIG,
	DT_AUTOTEAM_CONFIG,
	DT_PLAYER_REALM_CONFIG,
	DT_CHARIOT_CONFIG,

	DT_CHARIOT_WAR_CONFIG,
	DT_POKER_LEVELEXP_CONFIG,
	DT_POKER_SUITE_ESSENCE,
	DT_POKER_DICE_ESSENCE,
	DT_POKER_SUB_TYPE,

	DT_POKER_ESSENCE,
	DT_TOKEN_SHOP_CONFIG,
	DT_SHOP_TOKEN_ESSENCE,
	DT_GT_CONFIG,
	DT_RAND_SHOP_CONFIG,

	DT_PROFIT_TIME_CONFIG,
	DT_FACTION_PVP_CONFIG,
	DT_UNIVERSAL_TOKEN_ESSENCE,
	DT_TASK_LIST_CONFIG,
	DT_TASK_DICE_BY_WEIGHT_CONFIG,

	DT_FASHION_SUITE_ESSENCE,
	DT_FASHION_BEST_COLOR_CONFIG,
	DT_SIGN_AWARD_CONFIG,
	
	DT_MAX,
	
};

#pragma pack(pop, EXP_TYPES_INC)
#endif//_EXP_TYPES_H_


