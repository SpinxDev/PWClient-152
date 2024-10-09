// ElementSkill.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "ElementSkill.h"
#include "playerwrapper.h"
#include "skillwrapper.h"
#include "skill.h"

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}

namespace GNET{

	
ELEMENTSKILL_API void SkillTimer()
{
}

ElementSkill* ElementSkill::Create(ID id, int n)
{
	return Skill::Create(id, n);
}

void ElementSkill::Destroy() 
{ 
	delete this; 
}

int ElementSkill::LearnCondition(ID id, LearnRequirement& info, int ilevel )
{
	Skill* s =  Skill::Create(id, ilevel);
	if(!s)
		return 5;
	int ret = 0;
	SkillWrapper* wrapper = SkillWrapper::Instance();

	if(wrapper->IsOverridden(id))
		return 11;
	int srank, prank;
	srank = s->GetRank();
	prank = info.rank;
	if( srank>prank)
		ret = 7;
	else
	{
		srank = (int)(srank*0.1);
		prank = (int)(prank*0.1);
		if(srank!=prank && srank !=0)
			ret = 7;
	}

	if(ilevel<1 || ilevel> s->GetMaxLevel())
		ret = 3;
	else  if(info.profession!=s->GetCls() && s->GetCls()!=255)
		ret = 4;
	else if(info.level<s->GetRequiredLevel())
		ret = 2;
	
	if(ret)
	{
		s->Destroy();
		return ret;
	}
	if(info.sp<s->GetRequiredSp())
		ret = 1;
	else if(info.money<s->GetRequiredMoney())
		ret = 6;
	
	const std::vector<std::pair<unsigned int, int> > & pre_skills = s->GetRequiredSkill();
	for(size_t i=0; i<pre_skills.size(); i++)
	{
		unsigned int pre_id = pre_skills[i].first;
		int pre_level = pre_skills[i].second;
		if(pre_id > 0 && wrapper->GetLevel(pre_id) < pre_level && !wrapper->IsOverridden(pre_id))
		{
			ret = 9;
			break;
		}		
	}

	if(ilevel>1)
	{
		s->SetLevel(ilevel-1);
		int ability = s->GetMaxability();
		if(ability>0 && wrapper->GetAbility(id)<ability)
			ret = 10;
	}
	
	if(info.realm_level < s->GetRequiredRealmLevel())
		ret = 12;

	s->Destroy();
	return ret;
}

int ElementSkill::Condition(ID id, UseRequirement& info, int ilevel )
{
	Skill* s =  Skill::Create(id, ilevel);
	if(!s)
		return 5;

	ComboArg * arg = s->GetPlayer()->GetComboarg();
	for(int i=0; i<ComboSkillState::MAX_COMBO_ARG; i++)
		arg->SetValue(i, info.combo_state.arg[i]);
	
	int ret = ((ElementSkill*)s)->Condition(info);
	
	s->Destroy();
	return ret;
}

const char* ElementSkill::GetIcon(ID id)
{
	const SkillStub * s = SkillStub::GetStub(id);
	if(s)
		return s->GetIcon();
	return "";
}

const char* ElementSkill::GetNativeName(ID id)
{
	const SkillStub * s = SkillStub::GetStub(id);
	if(s)
		return s->nativename;
	return "";
}

const wchar_t* ElementSkill::GetName(ID id)
{
	const SkillStub * s = SkillStub::GetStub(id);
	if(s)
		return s->GetName();
	return L"";
}

char ElementSkill::GetType(ID id)
{
	const SkillStub * s = SkillStub::GetStub(id);
	if(s)
		return s->GetType();
	return 0;
}

ElementSkill::ID ElementSkill::NextSkill(ID start)
{
	SkillStub::Map& m = SkillStub::GetMap();
	SkillStub::Map::iterator it; 
	if(start == 0 )
		it = m.begin();
	else
	{
		it = m.find(start);
		++it;
	}
	if(it==m.end())
		return 0;
	else
		return it->second->GetId();
}

const char* ElementSkill::GetEffect(ID id) 
{ 
	const SkillStub * stub = SkillStub::GetStub(id);
	if(stub)
		return stub->effect;
	return "";
}

const char* ElementSkill::GetElseEffect(ID id) 
{ 
	const SkillStub * stub = SkillStub::GetStub(id);
	if(stub)
		return stub->aerial_effect;
	return "";
}

int ElementSkill::Condition(UseRequirement& info)
{
	if(info.arrow < GetArrowCost())
		return 9;
	if(!ValidWeapon(info.weapon))
		return 1;
	if(info.mp < GetMpCost())
		return 2;
	int form_type = (info.form & FORM_MASK_HIGH) >> 6;
	if((GetAllowForms() & (1 << form_type))==0)
		return 7;
	if(info.ap < GetApCost())
		return 8;
	if(info.freepackage==0 && GetTargetType()==3)
		return 10;
	//  移动环境
	if( (info.move_env == 0 && !IsAllowLand()) ||
		(info.move_env == 1 && !IsAllowWater()) ||
		(info.move_env == 2 && !IsAllowAir()))
		return 3;

	if(info.is_combat && GetNotuseInCombat())
		return 11;

	if(!CheckHpCondition(info.hp, info.max_hp))
		return 12;

	int preskill = GetComboSkPreSkill();
	if(preskill)
	{
		if(info.combo_state.skillid != (ID)preskill
			|| !CheckComboSkExtraCondition())
			return 13;
	}
	
	return 0;
}

const wchar_t* ElementSkill::GetIntroduction(ID id, int level, wchar_t* buf, unsigned int len, const SkillStr& table)
{
	Skill* s =  Skill::Create(id, level);
	if(!s)
		return L"";
	
	const wchar_t* ret = s->GetIntroduction(buf, len, table);
	s->Destroy();
	return ret;
}

int ElementSkill::GetRequiredLevel(ID id, int level)
{
	Skill* s =  Skill::Create(id, level);
	if(!s)
		return 0;
	
	int ret = s->GetRequiredLevel(); 
	s->Destroy();
	return ret;
}

int ElementSkill::GetRequiredSp(ID id, int level)
{
	Skill* s =  Skill::Create(id, level);
	if(!s)
		return 0;
	
	int ret = s->GetRequiredSp(); 
	s->Destroy();
	return ret;
}

int ElementSkill::GetRequiredMoney(ID id, int level)
{
	Skill* s =  Skill::Create(id, level);
	if(!s)
		return 0;
	
	int ret = s->GetRequiredMoney(); 
	s->Destroy();
	return ret;
}

int ElementSkill::GetRequiredRealmLevel(ID id, int level)
{
	Skill* s =  Skill::Create(id, level);
	if(!s)
		return 0;
	
	int ret = s->GetRequiredRealmLevel(); 
	s->Destroy();
	return ret;
}

int ElementSkill::GetRequiredBook(ID id, int level)
{
	Skill* s =  Skill::Create(id, level);
	if(!s)
		return 0;
	
	int ret = s->GetRequiredBook(); 
	s->Destroy();
	return ret;
}

bool ElementSkill::IsValidForm(char form)
{
	char form_type = (form & FORM_MASK_HIGH) >> 6;
	return ((GetAllowForms() & (1 << form_type))!=0);
}

bool ElementSkill::IsValidForm(ID id, char form)
{
	Skill* s =  Skill::Create(id, 1);
	if(!s)
		return 0;
	
	bool ret = s->IsValidForm(form); 
	s->Destroy();
	return ret;
}

bool ElementSkill::IsGoblinSkill(ID id)
{	
	const SkillStub * s = SkillStub::GetStub(id);
	return s && (s->GetCls()==258);
}

int ElementSkill::GetComboSkPreSkill(ID id)
{
	const SkillStub * s = SkillStub::GetStub(id);
	if (s)
	{
		return s->combosk_preskill;
	}
	return 0;
}

const VisibleState* VisibleState::Query(int profession, int id)
{
	return VisibleStateImp::Query(profession, id);
}

const TeamState* TeamState::Query(int id)
{
	return TeamStateImp::Query(id);
}

int ElementSkill::GetVersion()
{
	return 15;
}

int ElementSkill::GetExecuteTime(ID id, int level)
{
	Skill* s =  Skill::Create(id, level);
	if(!s)
		return 0;
	
	int ret = s->GetExecuteTime(); 
	s->Destroy();
	return ret;
}

void ElementSkill::LoadSkillData(void * data)
{
	SkillWrapper::Instance()->LoadData((cmd_skill_data*)data);
}

int ElementSkill::GetAbility(ID id)
{
	return SkillWrapper::Instance()->GetAbility(id);
}

int ElementSkill::SetAbility(ID id, int ability)
{
	return SkillWrapper::Instance()->SetAbility(id, ability);
}

int ElementSkill::SetLevel(ID id, int level)
{
	return SkillWrapper::Instance()->SetLevel(id, level);
}

int ElementSkill::GetLevel(ID id)
{
	return SkillWrapper::Instance()->GetLevel(id);
}

bool ElementSkill::IsOverridden(ID id)
{
	return SkillWrapper::Instance()->IsOverridden(id);
}

std::vector<std::pair<unsigned int, int> > ElementSkill::GetJunior(ID id)
{
	const SkillStub * stub = SkillStub::GetStub(id);
	if(stub && stub->is_senior)
		return stub->pre_skills;
	return std::vector<std::pair<unsigned int, int> >();
}

int ElementSkill::GetCommonCoolDown(ID id)
{
	const SkillStub * s = SkillStub::GetStub(id);
	if(s)
		return s->commoncooldown;
	return 0;
}

int ElementSkill::GetCommonCoolDownTime(ID id)
{
	const SkillStub * s = SkillStub::GetStub(id);
	if(s)
		return s->commoncooldowntime;
	return 0;
}

int ElementSkill::GetItemCost(ID id)
{
	const SkillStub * s = SkillStub::GetStub(id);
	if(s)
		return s->itemcost;
	return 0;
}

int ElementSkill::GetMaxAbility(ID id, int level) {
	if (id <= 0) {
		return 0;
	}
	Skill* s = Skill::Create(id, level);
	int maxAbility = s->GetMaxability();
	s->Destroy();
	return maxAbility;
}

int ElementSkill::GetAbilityPercent(ID id)
{
	int now = SkillWrapper::Instance()->GetAbility(id);
	int level = SkillWrapper::Instance()->GetLevel(id);
	Skill* s =  Skill::Create(id, level);
	int max = 0;
	if(level>0)
		max = s->GetMaxability();
	int min = 0;
	if(level>1)
	{
		s->SetLevel(level-1);
		min = s->GetMaxability();
	}
	s->Destroy();
	if(max==min)
		return 100;
	return 100*(now-min)/(max-min);
}
	// 0:成功            1:主人SP不足        2:级别不够
	// 3:满级            9:前提技能级别不够  5:错误ID
int ElementSkill::PetLearn(ID id, PetRequirement& info, int level)
{
	Skill* s =  Skill::Create(id, level);
	if(!s)
		return 5;
	int ret = 0;

	if(level<1 || level> s->GetMaxLevel())
		ret = 3;
	else if(info.level<s->GetRequiredLevel())
		ret = 2;
	else if(info.sp<s->GetRequiredSp())
		ret = 1;
	
	const std::vector<std::pair<unsigned int, int> > & pre_skills = s->GetRequiredSkill();
	for(size_t i=0; i<pre_skills.size(); i++)
	{
		unsigned int pre_id = pre_skills[i].first;
		int pre_level = pre_skills[i].second;
		if(pre_id > 0)
		{
			int curlevel = 0;
			for(int i=0;i<info.lsize;i+=2)
			{
				if(info.list[i]==(int)pre_id)
				{
					curlevel = info.list[i+1];
					break;
				}
			}
			if(pre_level > curlevel)
			{
				ret = 9;
				break;
			}
		}
	}

	s->Destroy();
	return ret;
}
	// 0:成功            1:主人SP不足        2:技能天赋点不足
	// 3:满级            4:技能个数限制		 5:错误ID
	// 6:金钱不足		 7:非小精灵技能		 8:没有技能书
	// 9:等级不足       10:能量上限不足     11:职业不匹配
    // 12:能量上限不足，且职业不匹配
int ElementSkill::GoblinLearn(ID id, GoblinRequirement& info, int level)
{
	Skill* s = Skill::Create(id, level);
	if(!s)
		return 5;
	if(level<1 || level> s->GetMaxLevel())
		return 3;
	if(s->GetCls() != 258)
		return 7;
	
	int ret = 0;
	
	int iReqGen[5];
	int iReqLevel = s->GetRequiredLevel();
	//  iReqLevel为7位整数，最后两位为等级，前5位为天赋点需求，最高位为金
	int iLevelRequirement = iReqLevel%100;
	if(info.level < iLevelRequirement)
		return 9;

	iReqLevel /= 100;

	int i;
	for(i=0;i<5;i++)
	{
		iReqGen[4-i] = iReqLevel%10;
		iReqLevel /= 10;
	}

	for(i=0;i<5;i++)
	{
		if(info.genius[i] < iReqGen[i])
			return 2;
	}
	
	if(info.sp < s->GetRequiredSp())
		ret = 1;
	//else if(info.money<s->GetRequiredMoney(id, level))
	// 	ret = 6;

	if(info.mp < s->GetMpCost() && 
		((s->GetClsLimit() != 0) && (((1 << info.profession) & s->GetClsLimit()) == 0)))
		ret = 12;
	else if(info.mp < s->GetMpCost())
		ret = 10;
	else if((s->GetClsLimit() != 0) && (((1 << info.profession) & s->GetClsLimit()) == 0))
		ret = 11;

	s->Destroy();
	return ret;
}
	// 0:成功            1:职业不匹配        2:mp不足
	// 3:AP不足			 4:天赋点不足		 5:错误ID
	// 6:未选择目标      7:非小精灵技能		 8:移动环境不满足
int ElementSkill::GoblinCondition(ID id, GoblinUseRequirement& info, int level )
{
	Skill* s =  Skill::Create(id, level);
	if(!s)
		return 5;
	
	s->GetPlayer()->SetElflevel(info.level);
	if(s->GetCls() != 258)
		return 7;

	int iReqGen[5];
	int iReqLevel = s->GetRequiredLevel();
	//  iReqLevel为7位整数，最后两位为等级，前5位为天赋点需求，最高位为金

	iReqLevel /= 100; // 去掉末两位等级
	int i;
	for(i=0;i<5;i++)
	{
		iReqGen[4-i] = iReqLevel%10;
		iReqLevel /= 10;
	}

	for(i=0;i<5;i++)
	{
		if((iReqGen[i] != 0) && (info.genius[i] < iReqGen[i]))
			return 4;
	}
	
	//  移动环境
	if( (info.move_env == 0 && !s->IsAllowLand()) ||
		(info.move_env == 1 && !s->IsAllowWater()) ||
		(info.move_env == 2 && !s->IsAllowAir()))
		return 8;
	
	/*
		stamina消耗
		一共6位
		低三位为系数 其余为数值
		消耗=数值+系数*(skill_level-1)
	*/
	int iApCost = (s->GetApCost()/1000) + (s->GetApCost()%1000)*(level - 1);
	int ret = 0;

	if((s->GetClsLimit() != 0) && (((1 << info.profession) & s->GetClsLimit()) == 0))
		ret = 1;
	else if(info.mp < s->GetMpCost())
		ret = 2;
	else if(info.ap < iApCost)
		ret = 3;
	
	s->Destroy();
	return ret;
}

const std::vector<ElementSkill::ID>& ElementSkill::GetInherentSkills(int cls)
{
	return SkillStub::GetInherentSkillList(cls);
}

void ElementSkill::GetComboSkActivated(const ComboSkillState & combo_state, std::vector<std::pair<ElementSkill::ID, int> >& res)
{
	if(combo_state.skillid == 0) return;

	SkillStub::ID_LIST & list = SkillStub::GetPostComboSkill(combo_state.skillid);
	for(size_t i=0; i<list.size(); i++)
	{
		Skill * s = Skill::Create(list[i], 1);	//这里都按照1级技能处理，以后有问题再修改
		if(!s) continue;

		ComboArg * arg = s->GetPlayer()->GetComboarg();
		for(int i=0; i<ComboSkillState::MAX_COMBO_ARG; i++)
			arg->SetValue(i, combo_state.arg[i]);

		if(s->CheckComboSkExtraCondition()) res.push_back(std::make_pair(s->GetId(), s->GetComboSkInterval()));

		s->Destroy();
	}
}

bool ElementSkill::IsMovingSkill(ID id)
{
	const SkillStub * s = SkillStub::GetStub(id);
	if(s)
		return s->IsMovingSkill();
	return false;
}

void ElementSkill::InitStaticData()
{
	SkillStub::InitStaticData();
}

}
