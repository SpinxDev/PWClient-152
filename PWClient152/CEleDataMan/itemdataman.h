#ifndef _ITEMDATAMAN_H_
#define _ITEMDATAMAN_H_

#include <vector.h>
#include <hashmap.h>

#ifdef LINUX
#include <arandomgen.h>
#include <amemory.h>
#include "exptypes.h"
#include "../itemdata.h"
#include "../item/item_addon.h"
#else	// Windows
#include "../celementdata/exptypes.h"
#include <Windows.h>
namespace abase
{
	inline int Rand(int lower, int upper)	
	{ 
		if(upper==lower)
			return lower;
		else	
			return rand()%(upper-lower)+lower; 
	}
	
	inline float Rand(float lower, float upper) 
	{ return lower+(upper-lower)*rand()/(float)RAND_MAX; }
	
	inline int RandNormal(int lower, int upper) { return Rand(lower, upper); }
	inline float RandUniform() { return Rand(0.f, 1.f); }
	
	inline int RandSelect(const void * option, int stride, int num) 
	{ 
		const char * tmp = (const char *)option;
		float op = RandUniform();
		for(int i =0;i < num; i ++)
		{
			float prob = *(float*)tmp;
			if(op < prob ) 
				return i;
			op -= prob;
			tmp += stride;
		}
		assert(false);
		return 0;
	}
	inline void * fastalloc(size_t size) { return malloc(size); }
	inline void   fastfree(void * buf, size_t size) {free(buf); }
};

struct item_data
{
	unsigned int type;   		//物品的模板ID
	size_t count;  				//物品的数量
	size_t pile_limit;			//物品的堆叠上限
	int equip_mask;  			//物品的可装备标志，0x8000表示是镶嵌物
	int proc_type;				//物品的处理方式
	int classid;  				//物品对应的类别ID
	struct
	{ 
		int guid1;
		int guid2;
	} guid;   				//物品的GUID
	int price;   				//物品的价格
	int expire_date;			//到期时间
	size_t content_length;
	char * item_content;
};

struct addon_data
{
	int id;
	int arg[3];
};

struct prerequisition
{       
	short level;
	short race;
	short strength;
	short vitality;
	short agility;
	short energy;
	int durability;
	int max_durability;
};      

#endif

class elementdataman;
namespace element_data
{
	
	enum LOWER{LOWER_TREND};
	enum UPPER{UPPER_TREND};
	enum MIDDLE{MIDDLE_TREND};
	enum ANY{ANY_TREND};
	
	enum NORMAL{ NORMAL_RAND};
	enum SPECIFIC{ SPECIFIC_RAND};

	struct SpecRand
	{
		int * IndexList;
		int   IdxCap;
		int   IdxIndex;
		float * RandList;
		int RandCap;
		int  RandIndex;
		SpecRand():IndexList(0),IdxCap(0),IdxIndex(0),RandList(0),RandCap(0),RandIndex(0){}		
		int RandSelect(int num)
		{
			if(!IndexList) return 0;
			if(IdxIndex >= IdxCap) return 0;
			if(num <=0) return 0;
			int idx =IndexList[IdxIndex++];
			if(idx >= num) idx = num - 1;
			return idx;
		}

		int RandNormal(int lower, int upper)
		{
			if(!RandList) return lower;
			if(RandIndex >= RandCap) return lower;
			float r = RandList[RandIndex ++];
			if(r <0) r = 0.f;
			if(r >=1.0f) r = 0.9999999f;
			return (int)((upper - lower + 1)* r + lower);
		}
		
		float Rand(float lower, float upper)
		{
			if(!RandList) return lower;
			if(RandIndex >= RandCap) return lower;
			float r = RandList[RandIndex ++];
			if(r <0) r = 0.f;
			if(r >=1.0f) r = 0.9999999f;
			return (float)((upper - lower)* r + lower);
		}
	};

	struct SpecCls
	{
		SpecRand * imp;
		SpecCls(SpecRand * tmp):imp(tmp){}
		int RandSelect(const void * option, int stride, int num)
		{
			if(imp) return imp->RandSelect(num);
			return 0;
		}

		int RandNormal(int lower, int upper)
		{
			if(imp) return imp->RandNormal(lower,upper);
			return lower;
		}

		float Rand(float lower, float upper)
		{
			if(imp) return imp->Rand(lower,upper);
			return lower;
		}

	};
	
	template<typename cls,typename trend>  int RandNormal(int lower, int upper,cls c,trend)
	{
		return c.RandNormal(lower,upper);
	}

	template<typename cls,typename trend> int RandSelect(const void * option, int stride, int num,cls c,trend)
	{
		return c.RandSelect(option,stride,num);
	}

	template<typename cls,typename trend>  float Rand(float lower, float upper,cls c,trend)
	{
		return c.Rand(lower,upper);
	}
	
	template<> 
		inline	int RandSelect<NORMAL,LOWER>(const void * option, int stride, int num,NORMAL,LOWER)
	{
		return abase::RandSelect(option, stride, num);
	}
	
	template<> 
		inline	int RandSelect<NORMAL,MIDDLE>(const void * option, int stride, int num,NORMAL,MIDDLE)
	{
		return abase::RandSelect(option, stride, num);
	}
	
	template<> 
		inline	int RandSelect<SPECIFIC,LOWER>(const void * option, int stride,int num,SPECIFIC,LOWER)
	{
		return 0;
	}
	template<> 
		inline	int RandSelect<SPECIFIC,MIDDLE>(const void * option, int stride, int num,SPECIFIC,MIDDLE)
	{
		return num / 2;
	}

	
	template<>
		inline int RandNormal<NORMAL,LOWER>(int lower,int upper,NORMAL,LOWER)
	{
		return abase::RandNormal(lower,upper);
	}
	
	template<>
		inline int RandNormal<NORMAL,UPPER>(int lower,int upper,NORMAL,UPPER)
	{
		return abase::RandNormal(lower,upper);
	}
	
	template<>
		inline int RandNormal<NORMAL,MIDDLE>(int lower,int upper,NORMAL,MIDDLE)
	{
		return abase::RandNormal(lower,upper);
	}
	template<>
		inline int RandNormal<NORMAL,ANY>(int lower,int upper,NORMAL,ANY)
	{
		return abase::RandNormal(lower,upper);
	}	
	
	template<>
		inline int RandNormal<SPECIFIC,LOWER>(int lower,int upper,SPECIFIC,LOWER)
	{
		return lower;
	}
	
	template<>
		inline int RandNormal<SPECIFIC,UPPER>(int lower,int upper,SPECIFIC,UPPER)
	{
		return upper;
	}
	
	template<>
		inline int RandNormal<SPECIFIC,MIDDLE>(int lower,int upper,SPECIFIC,MIDDLE)
	{
		return (upper + lower) / 2;
	}
	template<>
		inline int RandNormal<SPECIFIC,ANY>(int lower,int upper,SPECIFIC,ANY)
	{
		return abase::RandNormal(lower,upper);
	}	

	template <>
		inline float Rand<NORMAL,LOWER>(float lower, float upper,NORMAL,LOWER)
	{
		return abase::Rand(lower,upper);
	}

	template <>
		inline float Rand<NORMAL,MIDDLE>(float lower, float upper,NORMAL,MIDDLE)
	{
		return abase::Rand(lower,upper);
	}

	template <>
		inline float Rand<NORMAL,UPPER>(float lower, float upper,NORMAL,UPPER)
	{
		return abase::Rand(lower,upper);
	}

	template <>
		inline float Rand<NORMAL,ANY>(float lower, float upper,NORMAL,ANY)
	{
		return abase::Rand(lower,upper);
	}
	
	template<>
		inline float Rand<SPECIFIC,LOWER>(float lower,float upper,SPECIFIC,LOWER)
	{
		return lower;
	}
	
	template<>
		inline float Rand<SPECIFIC,UPPER>(float lower,float upper,SPECIFIC,UPPER)
	{
		return upper;
	}
	
	template<>
		inline float Rand<SPECIFIC,MIDDLE>(float lower,float upper,SPECIFIC,MIDDLE)
	{
		return (upper + lower) / 2;
	}
	
	template<>
		inline float Rand<SPECIFIC,ANY>(float lower,float upper,SPECIFIC,ANY)
	{
		return abase::Rand(lower,upper);
	}

	enum GEN_ADDON_MODE
	{
		ADDON_LIST_SHOP,
		ADDON_LIST_DROP,
		ADDON_LIST_PRODUCE,
		ADDON_LIST_SPEC,
	};

	enum ITEM_MAKE_TAG
	{	
		IMT_NULL,
		IMT_CREATE,
		IMT_DROP,
		IMT_SHOP,
		IMT_PRODUCE,
	};
#pragma pack(1)
	struct item_tag_t
	{
		char type;
		char size;
	};
#pragma pack()


};// name space element_data

//  move into class itemdataman?
void set_to_classid(DATA_TYPE type, item_data * data, int major_type);
void get_item_guid(int id,int & g1, int & g2);
int addon_generate_arg(DATA_TYPE type, addon_data & data, int arg_num/*初始的参数个数*/); //返回最终的参数个数（不会超过初始个数）
int addon_update_ess_data(const addon_data & data, void * essence,size_t ess_size, prerequisition * require);
void update_require_data(prerequisition *require);

class itemdataman
{
protected:
	
#define ELEMENTDATAMAN_EQUIP_MASK_WEAPON       0x0001
#define ELEMENTDATAMAN_EQUIP_MASK_HEAD         0x0002
#define ELEMENTDATAMAN_EQUIP_MASK_NECK         0x0004
#define ELEMENTDATAMAN_EQUIP_MASK_SHOULDER     0x0008
#define ELEMENTDATAMAN_EQUIP_MASK_BODY         0x0010
#define ELEMENTDATAMAN_EQUIP_MASK_WAIST        0x0020
#define ELEMENTDATAMAN_EQUIP_MASK_LEG          0x0040
#define ELEMENTDATAMAN_EQUIP_MASK_FOOT         0x0080
#define ELEMENTDATAMAN_EQUIP_MASK_WRIST        0x0100
#define ELEMENTDATAMAN_EQUIP_MASK_FINGER1      0x0200
#define ELEMENTDATAMAN_EQUIP_MASK_FINGER2	0x0400
#define ELEMENTDATAMAN_EQUIP_MASK_PROJECTILE	0x0800
#define ELEMENTDATAMAN_EQUIP_MASK_FLYSWORD	0x1000
#define ELEMENTDATAMAN_EQUIP_MASK_DAMAGERUNE	0x20000
#define ELEMENTDATAMAN_EQUIP_MASK_BIBLE		0x40000
#define ELEMENTDATAMAN_EQUIP_MASK_SPEAKER       0x80000
#define ELEMENTDATAMAN_EQUIP_MASK_AUTO_HP       0x100000
#define ELEMENTDATAMAN_EQUIP_MASK_AUTO_MP       0x200000
#define	ELEMENTDATAMAN_EQUIP_MASK_ELF			0x800000	//lgc
#define	ELEMENTDATAMAN_EQUIP_MASK_STALLCARD		0x1000000
	
	enum
	{
		ELEMENTDATAMAN_EQUIP_INDEX_WEAPON          = 0,
			ELEMENTDATAMAN_EQUIP_INDEX_HEAD        = 1,
			ELEMENTDATAMAN_EQUIP_INDEX_NECK        = 2,
			ELEMENTDATAMAN_EQUIP_INDEX_SHOULDER    = 3,
			ELEMENTDATAMAN_EQUIP_INDEX_BODY        = 4,
			ELEMENTDATAMAN_EQUIP_INDEX_WAIST       = 5,
			ELEMENTDATAMAN_EQUIP_INDEX_LEG         = 6,
			ELEMENTDATAMAN_EQUIP_INDEX_FOOT        = 7,
			ELEMENTDATAMAN_EQUIP_INDEX_WRIST       = 8,
			ELEMENTDATAMAN_EQUIP_INDEX_FINGER1     = 9,
			ELEMENTDATAMAN_EQUIP_INDEX_FINGER2     = 10,
			ELEMENTDATAMAN_EQUIP_INDEX_PROJECTILE  = 11,
			ELEMENTDATAMAN_EQUIP_INDEX_FLYSWORD    = 12,
	};
	
	
#define ELEMENTDATAMAN_MAX_NUM_ADDON_PARAM 3
	struct _addon		//属性条目
	{
		int addon_type;
		int addon_arg[ELEMENTDATAMAN_MAX_NUM_ADDON_PARAM]; 		// 0 ~ 3 数目是 ((type & 0x6000)>>13)
	};
	
#define ELEMENTDATAMAN_MAX_NUM_HOLES 5
#define ELEMENTDATAMAN_MAX_NUM_ADDONS 32
	
#pragma  pack(1)	
	struct _item_content
	{
		prerequisition preq;
		short sizeofessence;					//装备本体大小（字节）;
		//	essence							//char 本体[];							//每种不同装备的本体结构不同
		int num_hole;						//孔洞的数目（个数）;
		//	int hole_type[MAX_NUM_HOLES];	//孔洞里嵌入物的类型[孔洞的数目];       //如果孔洞数目为0,则这项不存在
		int num_addon;						//属性表条目的数目（个数）;
		//	_addon ad[MAX_NUM_ADDONS];		//[属性表条目的数目];
	};
#pragma  pack()	

	struct _weapon_essence
	{
		enum
		{
			WEAPON_TYPE_MELEE = 0,
			WEAPON_TYPE_RANGE = 1,
			WEAPON_TYPE_MELEE_ASN = 2,	//刺客使用的近程武器，除敏捷影响物攻外，其他与近程相同
		};
		
		short weapon_type;       	//武器类别 对应模板里的进程远程标志
		short weapon_delay;		//武器的攻击延迟时间，以50ms为单位
		int weapon_class;       	//武器子类 对应模板里的大类 比如刀剑 长兵等
		int weapon_level;		//武器级别 某些操作需要武器级别
		int require_projectile; 	//需要弹药的类型
		int damage_low;         	//物理攻击最小加值
		int damage_high;        	//物理攻击最大加值
		int magic_damage_low;   	//魔法攻击
		int magic_damage_high;  	//魔法攻击
		int attack_speed;
		float attack_range;
		float attack_short_range;
	};
	
	struct _projectile_essence
	{
		int projectile_type;		//弹药类型
		int enhance_damage;		//增加武器的攻击力
		int scale_enhance_damage; 	//按照比例增加攻击力
		int weapon_level_low;
		int weapon_level_high;
	};

	struct _stone_essence
	{
	};
	
	struct _fashion_essence
	{
		int require_level;
		unsigned short color;
		unsigned short gender;
	};

	
#define ELEMENTDATAMAN_MAGIC_CLASS 5
	struct _armor_essence
	{
		int defense;
		int armor;
		int mp_enhance;
		int hp_enhance;
		int resistance[ELEMENTDATAMAN_MAGIC_CLASS];
	};
	
	struct _decoration_essence
	{
		int damage;
		int magic_damage;
		int defense;
		int armor;
		int resistance[ELEMENTDATAMAN_MAGIC_CLASS];
	};
	
	struct _medicine_essence
	{
		
	};
	
	struct _material_essence
	{
		
	};
	
	struct _damagerune_essence
	{
		short rune_type;               
		short require_weapon_level_min;
		short require_weapon_level_max;
		short enhance_damage;
	};
	
	struct _armorrune_essence
	{
		int rune_type; 
		int require_player_level;
		float enhance_percent;
		int reduce_times;
	};
	
	struct _skilltome_essence
	{
		
	};
	
	struct _flysword_essence
	{
		int cur_time;
		size_t max_time;
		short require_level;		//需求玩家等级
		short element_level;		//需求元石等级
		int   require_class;
		size_t time_per_element;
		float speed_increase1;		//普通速度
		float speed_increase2;          //冲刺速度
	};

	struct _wingmanwing_essence
	{
		int require_level;
		size_t mp_launch;
		size_t mp_per_second;
		float speed_increase;
	};

	struct _taskdice_essence
	{
		int task_id;
	};
	
	struct _townscroll_essence
	{
		int duration;
	};
	
	struct _unionscroll_essence
	{
		
	};
	
	struct _revivescroll_essence
	{
		
	};
	
	struct _element_essence
	{
		
	};
	
	struct _taskmatter_essence
	{
		
	};
	
	struct _tossmatter_essence
	{
		int require_level;
		int require_strength;
		int require_agility;
		int damage_low;
		int damage_high;
		float attack_range;
		size_t use_time;
	};
	
	struct _facepill_essence
	{
		int duration;
		int require_class;
	};

	struct _petegg_essence
	{
		int req_level;          //需求玩家级别
		int req_class;          //需求玩家职业
		int honor_point;        //好感度
		int pet_tid;            //宠物的模板ID
		int pet_vis_tid;        //宠物的可见ID（如果为0，则表示无特殊可见ID）
		int pet_egg_tid;        //宠物蛋的ID
		int pet_class;          //宠物类型 战宠，骑宠，观赏宠
		short level;            //宠物级别
		unsigned short color;   //宠物颜色，最高位为1表示有效，目前仅对骑宠有效
		int exp;                //宠物当前经验
		int skill_point;        //剩余技能点
		unsigned short name_len;//名字长度 
		unsigned short skill_count;//技能数量		
		char name[16];          //名字内容
		//这里跟随技能表
		/*
		 *	
		 struct
		 {	
			int skill
			int level;
		 }skills[]
		 */
	};
	
	struct _petfood_essense
	{
		int honor_effect;	//点数效果
		int food_mask;		//食物种类
	};
	
	struct _monster_essence
	{
		
	};
	
	struct _npc_essence
	{
		
	};

#pragma pack(1)			//lgc
	struct _elf_skill_data
	{
		unsigned short id;
		short level;	
	};

	struct _elf_essence
	{
		unsigned int exp;
		short level;
	
		short total_attribute;	//升级产生的属性点总数，不包括装备增加的及各属性初始值
		short strength;			//由加属性点而产生的属性值，不包括装备增加的及各属性初始值
		short agility;
		short vitality;
		short energy;

		short total_genius;		//天赋点，不包括装备增加的
		short genius[5];			//金木水火土0-4
	
		short refine_level;
		int stamina; 			//耐力
		int status_value;		//0:安全 -1:可交易 正数:转化状态
	};
	

	struct _elf_item_content
	{
		struct _elf_essence ess;	//小精灵本体
		int equip_cnt;				//已装备的装备数量
		//unsigned int equipid[equip_cnt];		//装备id
		int skill_cnt;				//已学会的技能数
		//struct _elf_skill_data skill[skill_cnt];	//技能 id和等级
	};

	struct _elf_exppill_essence
	{
		unsigned int exp;	
		int level;
	};

#pragma pack()
	
	struct _wedding_bookcard_essence
	{
		int year;
		int month;
		int day;
	};

	struct _wedding_invitecard_essence
	{
		int start_time;
		int end_time;
		int groom;
		int bride;
		int scene;
		int invitee;
	};

	public:
		itemdataman();
		~itemdataman();
		
		int load_data(const char * pathname, bool disable_bind2 = false);
		
		unsigned int get_data_id(ID_SPACE idspace, unsigned int index, DATA_TYPE& datatype);
		unsigned int get_first_data_id(ID_SPACE idspace, DATA_TYPE& datatype);
		unsigned int get_next_data_id(ID_SPACE idspace, DATA_TYPE& datatype);
		
		unsigned int get_data_num(ID_SPACE idspace);
		DATA_TYPE get_data_type(unsigned int id, ID_SPACE idspace);
		
		const void * get_data_ptr(unsigned int id, ID_SPACE idspace, DATA_TYPE& datatype);
		
		int generate_item_from_monster(unsigned int id, int * list,  size_t size,int *money);
		void get_monster_drop_money(unsigned int id,int & low, int &high);
		int get_monster_drop_times(unsigned int id);

		const void* get_item_for_sell(unsigned int id);
		
		bool generate_addon(unsigned int addon_id, addon_data & data);
		int generate_addon(unsigned int item_id, unsigned int addon_id, addon_data & data);
		int generate_addon_from_rands(unsigned int item_id, unsigned int addon_id, addon_data & data);

		item_data * generate_item_for_shop(unsigned int id,const void * tag, size_t tag_size);
		item_data * generate_item_for_drop(unsigned int id,const void * tag, size_t tag_size);
		item_data * generate_item_from_player(unsigned int id,const void * tag, size_t tag_size); //生产出来的物品

		item_data * generate_equipment(unsigned int id, float rlist[32], int ridxlist[32], int addon[ELEMENTDATAMAN_MAX_NUM_ADDONS]);

		int get_refine_meterial_id();
		int get_item_refine_addon(unsigned int id,int &material_need);
		int get_item_sell_price(unsigned int id);
		int get_item_shop_price(unsigned int id);
		int get_item_pile_limit(unsigned int id);
		int get_item_repair_fee(unsigned int id);
		int get_item_proc_type(unsigned int id);
		int get_cool_time(unsigned int id);
		int get_item_damaged_drop(unsigned int id,unsigned int &damaged_drop);	//damaged_drop是损毁后掉落物品的id，返回值是掉落数量
		int get_item_class_limit(unsigned int id);
		int get_item_reputation_limit(unsigned id);
		int get_item_level(unsigned int id);	//获取物品的品阶
		int reset_classid(item_data * data);

#include "generate_item_temp.h"


	protected:
		int generate_addon(DATA_TYPE datatype,unsigned int addon_id, addon_data &pData);
		size_t generate_addon_buffer(DATA_TYPE datatype,unsigned int addon_id, char * buf);

		int generate_item_for_sell(bool disable_bind2);
		int duplicate_static_item(unsigned int id, char ** list,  size_t& size);
		
	protected:
		int	generate_equipment_addon(DATA_TYPE essencetype,char * header, size_t num_candidate, addon_data & data);
		size_t generate_equipment_addon_buffer(DATA_TYPE essencetype, const char * candidate_header, size_t candidate_num, char * addon_buffer,size_t & addon_num);
		size_t generate_equipment_addon_buffer_2(DATA_TYPE essencetype, int * candidate_addon,size_t candidate_addon_size,size_t candidate_num, char * addon_buffer,size_t & addon_num);
		size_t generate_spec_addon_buffer(DATA_TYPE essencetype, char * addon_buffer, size_t max_addon_size, size_t &addon_num,int * sa_list);	
		
	protected:
		elementdataman * _edm;
	public:
		elementdataman * GetElementDataMan() { return _edm;}
		
	private:
		template<class T>
			class array
		{
		public:
			inline size_t size() { return _v.size(); }
			inline void push_back(const T & x) { _v.push_back(x); }
			inline const T & operator [](size_t pos) const { return _v[pos]; }
			inline T & operator [](size_t pos) {return _v[pos];}
		protected:
			abase::vector<T> _v;
		};
		
		array<void *>				sale_item_ptr_array;
		array<size_t>				sale_item_size_array;
		
		struct LOCATION
		{
			DATA_TYPE		type;
			unsigned int	pos;
		};
		
		typedef abase::hash_map<unsigned int, LOCATION> IDToLOCATIONMap;
		IDToLOCATIONMap				sale_item_id_index_map;
};
#endif
