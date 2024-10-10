#ifndef __SKILL_SKILL_H
#define __SKILL_SKILL_H

#pragma warning(disable:4786)
#include "gnstring.h"
#include <vector.h>
#include <hashmap.h>
#include "playerwrapper.h"
#include "skillwrapper.h"
#include "ElementSkill.h"
#include <map>
#include <vector>

#pragma pack(1)

namespace GNET
{

typedef GnetString wsstream;

class Skill;

class Range
{
public:
    char    type;   // 0点 1线 2自身球 3目标球 4圆锥形 5自身
public:
    Range(char t=1) : type(t) { }
    Range& operator = (const Range& rhs)
    {
        return *this;
    }
    bool IsPoint() const { return 0 == type; }
    bool IsLine() const { return 1 == type; }
    bool IsSelfBall() const { return 2 == type; } 
    bool IsTargetBall() const { return 3 == type; }
    bool IsSector() const { return 4 == type; }
    bool IsSelf() const { return 5 == type; }
	bool NoTarget() const { return 2 == type || 5 == type; }
};

class SkillStub
{
public:
	typedef unsigned int ID;
	enum { MIN_LEVEL = 1, MAX_LEVEL = 10 };

public:
	// base info
	ID				id;					// 唯一数字标识
	int				cls;				// 职业
	const wchar_t*  name;               // 技能名字
	const char*		nativename;			// 技能名
	const char*		icon;				// 技能图标
	int				max_level;			// 技能最大级别
	char            type;               // 技能类别 1主动攻击 2主动辅助 3主动召唤 4被动 5激活

	// execute condition
	bool			allow_ride;			// 骑乘有效
    short           attr;               // 主技能属性, 1物理、2金、3木、4水、5火、6土 六选一 默认（物理）
    int             rank;               // 修真级别
	int             eventflag;
	char            is_senior;
	bool			is_inherent;		//天生技能，不用学就能用
	bool			is_movingcast;		//移动施法
	int             npcdelay;
	int             showorder;
	char            allow_forms;
	int             apcost;
	int             apgain;
	char            doenchant;
	char            dobless;
	int             arrowcost;
	std::vector<std::pair<unsigned int,int> > pre_skills;

    // execute condition
    bool            allow_land;             // 陆地有效
    bool            allow_air;              // 空中有效
    bool            allow_water;            // 水中有效
	bool			notuse_in_combat;	//战斗状态不可用
	int             restrict_corpse;        // 只对尸体有效
    bool            restrict_change;        // 是否必须变身后
    bool            restrict_attach;        // 是否必须附体后
	bool            auto_attack;            // 使用后是否自动攻击
	char            time_type;              // 瞬发技能
	char            long_range;
	char			posdouble;				// 空地选择
	int				clslimit;				// 职业限制
	
	int commoncooldown;					//公共冷却mask bit0-4 技能冷却0-4 bit5-9 物品冷却0-4
	int commoncooldowntime;			//冷却时间，单位毫秒
	
	int itemcost;		//吟唱时所需物品 >0有效
	//连续技
	int combosk_preskill;
	int combosk_interval;
	int combosk_nobreak;

	const char* effect;
	const char* aerial_effect;

	Range range;
	bool has_stateattack;

	abase::vector<int> restrict_weapons;  
	typedef abase::hash_map<ID, SkillStub*, abase::_hash_function> Map;
	static Map& GetMap();

	typedef std::vector<ID> ID_LIST;
	static ID_LIST & GetInherentSkillList(int cls);

	typedef std::map<ID, ID_LIST> COMBOSK_MAP;
	static COMBOSK_MAP & GetComboSkMap();

protected:
	SkillStub(ID i) : id(i), is_inherent(false),is_movingcast(false), itemcost(0), combosk_preskill(0), combosk_interval(0),combosk_nobreak(0)
	{ 
		if(!GetStub(id)) 
			GetMap().insert(Map::value_type(id, this)); 
	}
	
	~SkillStub();

public:

	static const SkillStub *GetStub(ID i)
	{
		Map::iterator it = GetMap().find(i);
		return it == GetMap().end() ? NULL : it->second;
	}

	static void InitStaticData()
	{
		Map & m = GetMap();
		for(Map::iterator it = m.begin(); it != m.end(); ++it)
		{
			SkillStub & sk = *(it->second);
			if(sk.is_inherent) GetInherentSkillList(sk.cls).push_back(sk.id);
			if(sk.combosk_preskill) GetComboSkMap()[sk.combosk_preskill].push_back(sk.id);
		}
	}

	static ID_LIST & GetPostComboSkill(ID id)
	{
		static ID_LIST null_list;
		COMBOSK_MAP & m = GetComboSkMap();
		COMBOSK_MAP::iterator it = m.find(id);
		if(it == m.end()) return null_list;
		return it->second;
	}

	ID GetId() const { return id; }
	int GetCls() const { return cls; }
	const wchar_t* GetName() const { return name; }
	const char* GetIcon() const { return icon; }
	int GetMaxLevel() const { return max_level; }
	const Range & GetRange() const { return range; }
	int GetItemCost() { return itemcost; }
	bool IsMovingSkill() const { return is_movingcast; }

	char GetType() const { return type; }
	
	virtual int GetCoolingtime(Skill *skill) const = 0;
	virtual int GetRequiredSp(Skill *skill) const { return 0; }
	virtual int GetRequiredLevel(Skill *skill) const { return 0; }
	virtual int GetMaxAbility(Skill* skill) const { return 0; }
	virtual int GetRequiredItem(Skill* skill) const { return 0; }
	virtual int GetRequiredMoney(Skill* skill) const { return 0; }
	virtual int GetRequiredRealmLevel(Skill* skill) const { return 0; }
	virtual float GetPraydistance(Skill *skill) const = 0;
	virtual float GetMpcost(Skill* skill) const{ return 0;}
	virtual int GetExecutetime(Skill *skill) const { return 1000; }
	virtual bool CheckHpCondition(int hp, int max_hp) const{ return true; }
	virtual bool CheckComboSkExtraCondition(Skill* skill) const { return true; }
	virtual int GetIntroduction(Skill*,wchar_t*,int,wchar_t*) const {return 0;}
	
	bool ValidWeapon(int weapon) const
	{
		unsigned int i;
		for(i=0; i<restrict_weapons.size(); i++ )
		{
			if( weapon == restrict_weapons[i] )
				return true;
		}   
		if( i>0 && i >= restrict_weapons.size() )
			return false;
		return true;
	}
};

class Skill : public ElementSkill
{
protected:
	const SkillStub * stub;
	PlayerWrapper player;

	ID id;
	unsigned int level;

	Skill(ID i, const SkillStub* s) : stub(s), id(i) 
	{	
	}
	
	virtual ~Skill() { }
public:

	static Skill* Create(ID id, int n)
	{
		const SkillStub * stub = SkillStub::GetStub(id);
		if(!stub)
			return NULL;
		Skill* skill = new Skill(id, stub);
		int max = skill->GetMaxLevel();
		if(n>max)
			skill->SetLevel(max);
		else
			skill->SetLevel(n);
		return skill;
	}

	void Destroy() 
	{ 
		delete this; 
	}


	bool ReSetStub() { 
		stub = SkillStub::GetStub(id); 
		return NULL != stub; 
	}
	
	const SkillStub * GetStub() const {	return stub; }
	PlayerWrapper * GetPlayer()  { return &player; }

	ID GetId() const { return id; }
	int SetLevel(int l) 
	{
		int tmp = level;
		level = l; 
		return tmp;
	}
	int GetLevel() const { return level; }
	void SetPerform(int p) { }
	int GetPerform() { return 0; }

	char GetType() const { return stub->type; }
	const wchar_t* GetName() { return stub->GetName();}

	float GetPrayRange(float range, float prayplus) {
		player.SetRange(range);
		player.SetPrayrangeplus(prayplus);
		if(stub->type==TYPE_ATTACK || stub->type==TYPE_CURSE)
		{
			if(stub->auto_attack)
			{
				float r = 0.3f * stub->GetPraydistance(this);
				if(r>=1.0)
					return stub->GetPraydistance(this) - 1.0f;
				else
					return (float)0.7*stub->GetPraydistance(this);
			}
			else
				return stub->GetPraydistance(this);
		}
		if(stub->type==TYPE_JUMP)
			return stub->GetPraydistance(this); 
		if(stub->GetRange().NoTarget())
			return -1;
		else
			return stub->GetPraydistance(this); 
	}

	const char* GetIcon() { return stub->GetIcon(); }
	int GetRangeType() { return stub->GetRange().type; }
	int GetCls() const { return stub->GetCls(); }
	int GetCoolingTime() { return stub->GetCoolingtime(this); }
	int GetArrowCost () { return stub->arrowcost; }
	int GetMaxLevel()  { return stub->GetMaxLevel(); }
	int GetShowOrder() { return stub->showorder; }
	int GetRand() { return 100; }
	int GetRank() { return stub->rank; }
	int GetMpCost() { return (int)stub->GetMpcost(this); }
	int GetApCost() { return stub->apcost; }
	int GetAbility() { return SkillWrapper::Instance()->GetAbility(id);}

	char GetAllowForms() { return stub->allow_forms; }
	int GetMaxability() { return stub->GetMaxAbility(this); }
	int GetRequiredSp() { return stub->GetRequiredSp(this); }
	int GetRequiredLevel() { return  stub->GetRequiredLevel(this); } 
	int GetRequiredBook() { return stub->GetRequiredItem(this); }
	int GetRequiredMoney() { return  stub->GetRequiredMoney(this); }
	int GetRequiredRealmLevel() { return stub->GetRequiredRealmLevel(this); }
	const std::vector<std::pair<unsigned int, int> > & GetRequiredSkill() { return stub->pre_skills; }
	std::vector<std::pair<unsigned int, int> > GetJunior() { return stub->is_senior ? stub->pre_skills : std::vector<std::pair<unsigned int, int> >(); }
	int GetClsLimit() { return stub->clslimit; }
	int GetCommonCoolDown() { return stub->commoncooldown; }
	int GetCommonCoolDownTime() { return stub->commoncooldowntime; }
	int GetItemCost() { return stub->itemcost; }
	int GetComboSkPreSkill(){ return stub->combosk_preskill; }
	int GetComboSkInterval(){ return stub->combosk_interval; }
		//检查施放时hp限制
	bool CheckHpCondition(int hp, int max_hp) { return stub->CheckHpCondition(hp, max_hp); }
	bool CheckComboSkExtraCondition() { return stub->CheckComboSkExtraCondition(this); }

	bool IsWarmup()     { return stub->time_type==3; }
	bool IsInstant()    { return stub->time_type==1; }
	bool IsDurative()   { return stub->time_type==2; }
	bool IsAutoAttack() { return stub->auto_attack; }
	
	bool IsAllowLand()  { return stub->allow_land; }
	bool IsAllowWater() { return stub->allow_water; }
	bool IsAllowAir()   { return stub->allow_air; }
	bool GetNotuseInCombat() { return stub->notuse_in_combat; }
	bool IsMovingSkill() { return stub->IsMovingSkill(); }
 
	int GetTargetType()
	{
		if(stub->restrict_corpse==1)
			return 2;
		if(stub->restrict_corpse==2)
			return 3;
		if(stub->type==TYPE_ATTACK || stub->type==TYPE_CURSE)
			return 1;
		if(stub->type==TYPE_BLESSPET)
			return 4;
		if(stub->GetRange().NoTarget())
			return 0;
		return 1;
	}
	
	int GetCastEnv()
	{
		int iRet = 0;
		if(stub->allow_land)
			iRet += 1;
		if(stub->allow_air)
			iRet += 2;
		if(stub->allow_water)
			iRet += 4;

		return iRet;
	}
	
	int* GetRequiredGenius(int idSkill)
	{
		int* iGenius = new int[5];
		if(!iGenius || stub->cls != 258)
			return NULL;
			
		//  iReqLevel为7位整数，最后两位为等级，前5位为天赋点需求，最高位为金
		int iLevelReq = GetRequiredLevel()/100;

		for(int i=0;i<5;i++)
		{
			iGenius[4-i] = iLevelReq%10;
			iLevelReq /= 10;
		}

		return iGenius;
	}

	int GetExecuteTime()
	{
		return stub->GetExecutetime(this);
	}

	const wchar_t* GetIntroduction(wchar_t* buf,int len,const SkillStr& table)
	{
		int n = stub->GetIntroduction(this,buf,len,table.Find(stub->id*10+1)); 
		if(n<0)
			return L"";
		return buf;
	}

	int GetAbilityPercent()
	{
		int now = SkillWrapper::Instance()->GetAbility(id);
		int max = 0;
		if(level>0)
			max = GetMaxability();
		int min = 0;
		if(level>1)
		{
			SetLevel(level-1);
			min = GetMaxability();
			SetLevel(level);
		}
		if(max==min)
			return 100;
		return 100*(now-min)/(max-min);
	}

	bool ValidWeapon(int w) const { return stub->ValidWeapon(w); }
	const char* GetEffect() { return stub->effect;}
	const char* GetElseEffect() { return stub->aerial_effect;}
	const char* GetNativeName() { return stub->nativename;}
};

class VisibleStateImp : public VisibleState
{
	int id;
	const wchar_t*	name;
	const char* effect;
	const char* hh;
	typedef abase::hash_map<int, const VisibleState*, abase::_hash_function> Map;
	static Map stubmap;
public:
	VisibleStateImp(int i, const wchar_t* n, const char* e, const char* h):id(i), name(n), effect(e), hh(h)
	{
		stubmap.insert(Map::value_type(id, this));  
	}
	static const VisibleState* Query(int prof, int id);
	const int GetID() const { return id; }
	const wchar_t* GetName() const {  return name; }
	const char* GetHH() const { return hh; }
	const char* GetEffect() const {  return effect; }
};

class TeamStateImp : public TeamState
{
	int id;
	const wchar_t* name;
	const char*   icon;
	typedef abase::hash_map<int, const TeamState*, abase::_hash_function> Map;
	static Map stubmap;
public:
	TeamStateImp(int i, const wchar_t* n, const char* c):id(i), name(n), icon(c)
	{
		stubmap.insert(Map::value_type(id, this)); 
	}
	static const TeamState* Query(int id);
	const int GetID() const { return id; }
	const wchar_t* GetName() const {  return name; }
	const char* GetIcon() const { return icon; }
};

};
#pragma pack()

#endif

