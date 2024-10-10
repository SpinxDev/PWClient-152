#ifndef __ELEMENT_SKILL_H
#define __ELEMENT_SKILL_H

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the ELEMENTSKILL_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// ELEMENTSKILL_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef ELEMENTSKILL_EXPORTS
#define ELEMENTSKILL_API __declspec(dllexport)
#else
#define ELEMENTSKILL_API __declspec(dllimport)
#endif

#include <vector>
#include <utility>

namespace GNET
{
enum skill_type{
    TYPE_ATTACK   = 1,  // 主动攻击
    TYPE_BLESS    = 2,  // 主动祝福
    TYPE_CURSE    = 3,  // 主动诅咒
    TYPE_SUMMON   = 4,  // 物品技能
    TYPE_PASSIVE  = 5,  // 被动
    TYPE_ENABLED  = 6,  // 武器附加
    TYPE_LIVE     = 7,  // 生活
    TYPE_JUMP     = 8,  // 瞬移
	TYPE_PRODUCE  = 9,  // 生产(互斥)
	TYPE_BLESSPET = 10,  // 宠物祝福
	TYPE_NEUTRALBLESS = 11,	//中性祝福
};

enum range_type{
	TYPE_POINT = 0,          // 点 
	TYPE_LINE  = 1,          // 线 
	TYPE_SELFSPHERE  = 2,    // 自身为中心的球
	TYPE_TARGETSPHERE = 3,   // 目标为中心的球
	TYPE_TAPER  = 4,         // 圆锥
	TYPE_SLEF  = 5,          // 自身
};

enum form{
	FORM_MASK_HIGH	= 0xC0,
	FORM_NORMAL		= 0,	//普通形态
	FORM_CLASS		= 1,	//职业变身
	FORM_BEASTIE	= 2,	//变小动物
};

struct LearnRequirement
{
	int level;		//玩家级别
	int sp;         //技能点
	int money;      //金钱
	int profession; //职业
	int rank;       //修真级别
	int realm_level;//境界等级
};

struct PetRequirement
{
	int  level;		 //宠物级别
	int  sp;         //主人技能点
	int* list;       //现有技能列表
	int  lsize;      //列表大小
};

struct GoblinRequirement
{
	int  genius[5];	  //小精灵技能天赋点
	int  profession;  //职业
	int  sp;		  //主人元神
	int  money;		  //主人金钱
	int  level;		  //小精灵等级
	int  mp;		  //小精灵当前能量上限
};

struct ComboSkillState
{
	enum{ MAX_COMBO_ARG = 3 };
	unsigned int skillid;
	int arg[MAX_COMBO_ARG];
	ComboSkillState():skillid(0)
	{
		memset(arg, 0, sizeof(arg));
	}
};

struct UseRequirement
{
	int mp;            //魔法
	int ap;            //怒气值
	int form;          //变身状态
	int weapon;        //武器
	int freepackage;   //包裹栏剩余数目
	int arrow;         //装备箭支数目
	int move_env;	//移动环境
	bool is_combat;   //是否战斗状态
	int hp;				//当前hp
	int max_hp;			//最大hp
	ComboSkillState combo_state;	//连续技数据
};

struct GoblinUseRequirement
{
	int mp;			//能量(元气)
	int ap;			//体力(耐力)
	int genius[5];	//小精灵技能天赋点
	int profession; //主人职业
	int level;		//小精灵等级，用于计算耐力消耗
	int move_env;	//主人当前移动环境 
};

class SkillStub;

class ELEMENTSKILL_API SkillStr
{
public:
	virtual wchar_t* Find(int id) const {  return L""; }
};

class ELEMENTSKILL_API ElementSkill
{
public:
	enum {
		SKILL_PERFORM,
		SKILL_DONE,
	};
	typedef unsigned int ID;
	
	static ID NextSkill(ID id = 0);

	// 技能名称
	virtual const wchar_t* GetName() { return NULL;}
	virtual const char* GetNativeName() { return NULL;}
	// 技能类别,见skill_type
	virtual char GetType() const { return 1; }        
	// 技能图标
	virtual const char* GetIcon() { return NULL; }
	// 技能说明
	virtual const wchar_t* GetIntroduction(wchar_t* buf,int len,const SkillStr& table) { return L""; }
	// 技能职业限制
	virtual int GetCls() const { return -1; }
	// 技能冷却时间，单位毫秒
	virtual int GetCoolingTime() { return 5000; }
	// 技能执行时间，单位毫秒
	virtual int GetExecuteTime() { return 1000; }
	// 目标类型限制, 0:无需目标，1:需要目标，2:目标仅限于尸体, 3:目标必须为怪物, 4:目标必须为宠物 
	virtual int GetTargetType(){return 0;}
	// 技能有效释放距离: <-0.001无需目标 -0.001-0.001 默认攻击距离，>0.001 释放距离
	virtual float GetPrayRange(float range, float prayplus) { return 0; }  


	// 技能修真级别
	virtual int GetRank() { return 0; }
	// 学习n级技能要求的玩家级别
	virtual int GetRequiredLevel() { return 0;}
	// 学习n级技能需要的技能点
	virtual int GetRequiredSp() { return 0;}
	// 学习n级需要的技能书
	virtual int GetRequiredBook() { return 0;}
	// 学习需要金钱
	virtual int GetRequiredMoney() { return 0;}
	// 学习需求境界等级
	virtual int GetRequiredRealmLevel() { return 0; }
	// 前提技能
	virtual const std::vector<std::pair<unsigned int, int> > & GetRequiredSkill() = 0;
	// 显示顺序
	virtual int GetShowOrder() { return 0; }
	// 设置技能级别
	virtual int SetLevel(int) { return 0; } 
	// 技能最大级别
	virtual int GetMaxLevel() { return 0; }

	// 是否蓄力技能
	virtual bool IsWarmup() { return false; }
	// 使用后是否自动攻击
	virtual bool IsAutoAttack(){return false;}
	// 瞬发技能
	virtual bool IsInstant(){return false;}
	// 是否持续技能
	virtual bool IsDurative() { return false; }
	// 杀伤范围的类型
	virtual int  GetRangeType() { return 0; }
	// 释放环境：空中、地面、水中
	virtual int  GetCastEnv() { return 0; }
	// 获取小精灵学习和释放技能所需天赋点
	virtual int* GetRequiredGenius(int idSkill) { return NULL; }

	// 效果文件名
	virtual const char* GetEffect() { return NULL;}
	virtual const char* GetElseEffect() { return NULL;}

	// 使用需要MP
	virtual int GetMpCost(){return 1;}

	// 使用需要AP
	virtual int GetApCost(){return 0;}

	// 箭支消耗
	virtual int GetArrowCost(){return 0;}

	// 武器条件判断
	virtual bool ValidWeapon(int w) const { return true; }
	// 0, 成功；1，武器不匹配；2, mp不足；3，位置条件不满足；4，骑乘条件不满足；5，错误ID, 6，未选择目标
	int Condition(UseRequirement& info);

	virtual bool IsAllowLand(){ return true; }
	virtual bool IsAllowWater(){ return true; }
	virtual bool IsAllowAir(){ return true; }
	virtual bool GetNotuseInCombat(){ return false; }
	//是否移动施法
	virtual bool IsMovingSkill() { return false; }
	// 技能能否在当前变身状态下使用
	bool IsValidForm(char form);
	virtual char GetAllowForms() { return 0; }
	virtual bool Interrupt() { return true; }
	int GetAbilityPercent();

	// 获取相对应的初级技能
	virtual std::vector<std::pair<unsigned int, int> > GetJunior() { return std::vector<std::pair<unsigned int, int> >(); };

	// 获取小精灵技能职业限制
	virtual int GetClsLimit() { return 0; }
	
	//获取公共冷却mask bit0-4 技能冷却0-4 bit5-9 物品冷却0-4
	virtual int GetCommonCoolDown() { return 0; }

	//获取公共冷却时间，单位毫秒
	virtual int GetCommonCoolDownTime() { return 0; }

	//获取吟唱所需物品
	virtual int GetItemCost() { return 0; }

	//获取连续技施放前提
	virtual int GetComboSkPreSkill(){ return 0; }

	//检查施放时hp限制
	virtual bool CheckHpCondition(int hp, int max_hp) { return true; }

	//检查连续技额外条件
	virtual bool CheckComboSkExtraCondition(){ return true; } 
	
	// 0:成功            1:SP不足            2:级别不够
	// 3:满级            4:种族不匹配        5:错误ID
	// 6:金钱不足        7:修真级别不满足    8:没有技能书
	// 9:没有前提技能    10:熟练度不足       11:已经学到了高级技能
	// 12:境界条件不满足
	static int LearnCondition(ID id, LearnRequirement& info, int level ); 

	// 0:成功            1:武器不匹配        2:mp不足
	// 3:位置条件不满足  4:骑乘条件不满足    5:错误ID
	// 6:未选择目标      7:变身状态错误      8:怒气值不足
	// 9:箭支不足        10:包裹栏没有剩余位置 11: 不能在战斗中使用
	// 12:HP条件不满足   13:连续技前提不满足
	static int Condition(ID id, UseRequirement& performer, int level );

	// 0:成功            1:主人SP不足        2:级别不够
	// 3:满级            9:前提技能级别不够  5:错误ID
	static int PetLearn(ID id, PetRequirement& info, int level); 

	// 0:成功            1:主人SP不足        2:技能天赋点不足
	// 3:满级            4:技能个数限制		 5:错误ID
	// 6:金钱不足		 7:非小精灵技能		 8:没有技能书
	// 9:等级不足       10:能量上限不足     11:职业不匹配
    // 12:能量上限不足，且职业不匹配
	static int GoblinLearn(ID id, GoblinRequirement& info, int level); 

	// 0:成功            1:职业不匹配        2:mp不足
	// 3:AP不足			 4:天赋点不足		 5:错误ID
	// 6:未选择目标      7:非小精灵技能      8:移动环境不满足
	static int GoblinCondition(ID id, GoblinUseRequirement& info, int level );

	//获取职业天生技能
	static const std::vector<ID>& GetInherentSkills(int cls);

	//获取激活的连续技列表
	static void GetComboSkActivated(const ComboSkillState & combo_state, std::vector<std::pair<ID, int> >& list);
	//是否移动施法
	static bool IsMovingSkill(ID id);

	static char GetType(ID id);  
	static const char* GetIcon(ID id);
	static const wchar_t* GetName(ID id);
	static const char* GetNativeName(ID id);
	static const char* GetEffect(ID id);
	static const char* GetElseEffect(ID id);
	static const wchar_t* GetIntroduction(ID id, int level, wchar_t* buf, unsigned int len, const SkillStr& table);
	static int GetRequiredLevel(ID id, int level);
	static int GetRequiredSp(ID id, int level);
	static int GetRequiredBook(ID id, int level);
	static int GetRequiredMoney(ID id, int level);
	static int GetRequiredRealmLevel(ID id, int level);
	static bool IsValidForm(ID id, char form);
	static int GetExecuteTime(ID id, int level);
	static int GetAbilityPercent(ID id);
	static std::vector<std::pair<unsigned int, int> > GetJunior(ID id);
	static int GetCommonCoolDown(ID id);
	static int GetCommonCoolDownTime(ID id);
	static int GetItemCost(ID id);
	
	static bool IsGoblinSkill(ID id);
	static int GetComboSkPreSkill(ID id);
	
	//主角技能熟练度  <0没有学会此技能 >=0 熟练度
	static int GetAbility(ID id); 
	static int SetAbility(ID id, int ability);
	static int SetLevel(ID id, int level);
	static int GetLevel(ID id);
	static bool IsOverridden(ID id);
	static int GetMaxAbility(ID id, int level);

	static void LoadSkillData(void * data);

	static ElementSkill* Create(ID id, int level);
	void Destroy();

	static int GetVersion();
	
	//
	// 初始化技能内部静态数据，在使用库之前调用！！！
	//
	static void InitStaticData();
};

class ELEMENTSKILL_API VisibleState
{
public:
	virtual const int GetID() const { return 0; }
	virtual const wchar_t* GetName() const {  return NULL; }
	virtual const char* GetHH() const { return NULL; }
	virtual const char* GetEffect() const {  return NULL; }
	static const VisibleState* Query(int profession, int id);
};

class ELEMENTSKILL_API TeamState
{
public:
	virtual const int GetID() const { return 0; }
	virtual const wchar_t* GetName() const {  return NULL; }
	virtual const char* GetIcon() const { return NULL; }
	static const TeamState* Query(int id);
};

}

#endif
