// File		: EC_ProfConfig.cpp
// Creator	: Xu Wenbin
// Date		: 2013/9/10

#include "EC_ProfConfigs.h"

#include <ABaseDef.h>
#include <EC_RoleTypes.h>

CECProfConfig::CECProfConfig()
{
}

CECProfConfig & CECProfConfig::Instance()
{
	static CECProfConfig s_dummy;
	return s_dummy;
}


bool CECProfConfig::IsProfession(int prof)const
{
	return prof >= 0 && prof < NUM_PROFESSION;
}

bool CECProfConfig::IsGender(int gender)const
{
	return gender >= 0 && gender < NUM_GENDER;
}

bool CECProfConfig::IsRace(int race)const
{
	return race >= 0 && race < NUM_RACE;
}

bool CECProfConfig::IsExist(int prof, int gender)const
{
	if (IsProfession(prof) && IsGender(gender)){
		static bool s_bExist[NUM_PROFESSION][NUM_GENDER] = {
			{ true,  true},		//	0:武侠
			{ true,  true},		//	1:法师
			{ true,  true},		//	2:巫师
			{ false, true},		//	3:妖精
			{ true,  false},	//	4:妖兽
			{ true,  true},		//	5:刺客
			{ true,  true},		//	6:羽芒
			{ true,  true},		//	7:羽灵
			{ true,  true},		//	8:剑灵
			{ true,  true},		//	9:魅灵
			{ true,  true},		//	10:夜影
			{ true,  true},		//	11:月仙
		};
		return s_bExist[prof][gender];
	}else{
		assert(false);
		return false;
	}
}

int	CECProfConfig::GetCounterpartGender(int gender)const{
	int result = -1;
	switch (gender){
	case GENDER_MALE:
	case GENDER_FEMALE:
		result = 1-gender;
		break;
	default:
		ASSERT(false);
	}
	return result;
}

bool CECProfConfig::CanShowOnCreate(int prof, int gender)const
{
	//	创建角色时是否应该显示此职业模型（每职业显示一种性别）
	if (IsProfession(prof) && IsGender(gender)){
		static bool s_bShowMale[NUM_PROFESSION] = {
			true,		//	0:武侠男
			false,		//	1:法师男
			false,		//	2:巫师男
			false,		//	3:妖精男
			true,		//	4:妖兽男
			true,		//	5:刺客男
			true,		//	6:羽芒男
			false,		//	7:羽灵男
			true,		//	8:剑灵男
			false,		//	9:魅灵男
			true,		//	8:夜影男
			false,		//	9:月仙男
		};
		return (gender == GENDER_MALE) ? s_bShowMale[prof] : !s_bShowMale[prof];
	}
	assert(false);
	return false;
}

int  CECProfConfig::GetRaceShowOrder(int race)const
{
	//	值越小优先级越大
	if (IsRace(race)){
		static int s_nRaceOrder[NUM_RACE] = {
			1,
			2,
			3,
			4,
			5,
			0,
		};
		return s_nRaceOrder[race];
	}
	assert(false);
	return -1;
}

int	 CECProfConfig::GetRaceByProfession(int prof)const
{
	if (IsProfession(prof)){
		static int s_nProfRace[NUM_PROFESSION] = {
			RACE_HUMAN,
			RACE_HUMAN,
			RACE_GHOST,
			RACE_ORC,
			RACE_ORC,
			RACE_GHOST,
			RACE_ELF,
			RACE_ELF,
			RACE_LING,
			RACE_LING,
			RACE_OBORO,
			RACE_OBORO,
		};
		return s_nProfRace[prof];
	}
	assert(false);
	return -1;
}

int  CECProfConfig::GetProfessionShowOrderInRace(int prof)const
{
	//	值越小优先级越大
	if (IsProfession(prof)){
		static int s_nRaceZeroShowOrderProf[NUM_RACE] = {
			PROF_WARRIOR,	//	人类：武侠
			PROF_ORC,		//	妖族：妖兽
			PROF_ANGEL,		//	羽人：羽灵
			PROF_MONK,		//	汐族：巫师
			PROF_JIANLING,	//	灵族：剑灵
			PROF_YEYING,	//	胧族：夜影
		};
		int race = GetRaceByProfession(prof);
		return prof == s_nRaceZeroShowOrderProf[race] ? 0 : 1;
	}
	assert(false);
	return -1;
}

bool CECProfConfig::ContainsAllProfession(unsigned int mask)const{
	return (GetAllProfessionMask() & mask) == GetAllProfessionMask();
}

unsigned int CECProfConfig::GetAllProfessionMask()const{
	static const unsigned int ALL_PROFESSION_MASK = (1 << NUM_PROFESSION)-1;
	return ALL_PROFESSION_MASK;
}

int CECProfConfig::GetMaxBodyID(int prof)const{
	if (IsProfession(prof)){
		// PROF_ANGEL, PROF_ARCHOR, PROF_MAGE, PROF_WARRIOR didn't have nBodyID
		static int s_maxBodyID[NUM_PROFESSION] = {-1, -1, 4, 5, 3, 4, -1, -1, 4, 4, 1, 1 };
		return s_maxBodyID[prof];
	}
	return -1;
}