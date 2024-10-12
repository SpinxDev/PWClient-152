/*
 * FILE: EC_PetCorral.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2005/12/23
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#include "EC_Global.h"
#include "EC_PetCorral.h"
#include "EC_Game.h"
#include "../CCommon/elementdataman.h"
#include "../CElementClient/EC_FixedMsg.h"
#include "../CElementClient/EC_IvtrTypes.h"

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


///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECPetData
//	
///////////////////////////////////////////////////////////////////////////

CECPetData::CECPetData()
{
	m_iIntimacy	= 0;
	m_iHunger	= 0;
	m_tid		= 0;
	m_tidVis	= 0;
	m_idEgg		= 0;
	m_iClass	= GP_PET_CLASS_INVALID;
	m_fHPFactor	= 0.0f;
	m_fMPFactor	= 0.0f;
	m_iLevel	= 0;
	m_isBind = false;
	m_canWebTrade = false;
	m_color	= 0;
	m_iExp		= 0;
	m_iSkillPt	= 0;
	m_iHP		= 0;
	m_iMP		= 0;

	memset(m_aSkills, 0, sizeof (m_aSkills));	
	memset(&m_ExtProps, 0, sizeof (m_ExtProps));
	m_vecDynSkillIndex.clear();
	m_vecNorSkillIndex.clear();
	m_iSpecialSkillIndex = -1;
	m_pDBEssence = NULL;
	m_iAtkRation = 0;
	m_iDefRation = 0;
	m_iHpRation = 0;
	m_iAtkLvlRation = 0;
	m_iDefLvlRation = 0;
	m_iNature = 0;
}

CECPetData::~CECPetData()
{
}

//	Initialize object
bool CECPetData::Init(const S2C::info_pet& Info)
{
	m_iIntimacy	= Info.honor_point;
	m_iHunger	= Info.hunger;
	m_tid		= Info.pet_tid;
	m_tidVis	= Info.pet_vis_tid;
	m_idEgg		= Info.pet_egg_tid;
	m_iClass	= Info.pet_class;
	m_fHPFactor	= Info.hp_factor;
	m_iLevel	= Info.level;
	m_isBind = (Info.is_bind & 0x01)!= 0;
	m_canWebTrade = (Info.is_bind & 0x02) != 0;
	m_color	= Info.color;
	m_iExp		= Info.exp;
	m_iSkillPt	= Info.skill_point;
	
	m_strName = ACString((const ACHAR*) &Info.name[0], Info.name_len / sizeof (ACHAR));
	elementdataman* pDB = g_pGame->GetElementDataMan();
	DATA_TYPE DataType;
	if (!m_strName.GetLength())
	{
		PET_ESSENCE* pe = (PET_ESSENCE*)pDB->get_data_ptr(m_tid, ID_SPACE_ESSENCE, DataType);
		if (pe)
		{
			m_strName = pe->name;
			g_pGame->FilterBadWords(m_strName);
		}		
	}
	// 进化宠性格技能配置表
	const void* pDBData = pDB->get_data_ptr(Info.evo_prop.nature, ID_SPACE_CONFIG, DataType);
	const PET_EVOLVED_SKILL_CONFIG* pDynSkills = NULL;
	if (pDBData && DataType == DT_PET_EVOLVED_SKILL_CONFIG)
	{
		const PET_EVOLVED_SKILL_CONFIG* pSkillConfig = (const PET_EVOLVED_SKILL_CONFIG*)pDBData;
		if (pSkillConfig)			
			pDynSkills = pSkillConfig;
	}

	//	Get database data	
	pDBData = pDB->get_data_ptr(m_tid, ID_SPACE_ESSENCE, DataType);
	if (pDBData)
	{
		if (DataType == DT_MONSTER_ESSENCE)
		{
			const MONSTER_ESSENCE* pMonsterData = (const MONSTER_ESSENCE*)pDBData;			
			const PET_EGG_ESSENCE* pEggData = (const PET_EGG_ESSENCE*)pDB->get_data_ptr(pMonsterData->id_pet_egg_captured, ID_SPACE_ESSENCE, DataType);
			if (pEggData)
				m_pDBEssence = (PET_ESSENCE*)pDB->get_data_ptr(pEggData->id_pet, ID_SPACE_ESSENCE, DataType);
		}
		else if (DataType == DT_PET_ESSENCE)
		{
			m_pDBEssence = (PET_ESSENCE*)pDBData;
		}
	}	
	m_vecDynSkillIndex.clear();
	m_vecNorSkillIndex.clear();

	int i = 0;
	for (i=0; i < GP_PET_SKILL_NUM ; i++)
	{
		int skill_id = Info.skills[i].skill;
		ASSERT(skill_id>=0);

		m_aSkills[i].idSkill = skill_id;
		m_aSkills[i].iLevel	 = Info.skills[i].level;

		if (m_pDBEssence && (skill_id > 0) && (skill_id == m_pDBEssence->specific_skill))
		{
			m_iSpecialSkillIndex = i;
			if (!IsEvolutionPet())
			{
			//	ASSERT(FALSE);
				a_LogOutput(1,"CECPetData::Init, pet(%d) which not evolution should not has special skill(%d)",m_tid,skill_id);
			}
			continue;
		}
		bool bNormal = true;
		if (pDynSkills)
		{
			for (int j=0;j<2;j++)
			{
				if(pDynSkills->skills[j].id == skill_id)
				{
					m_vecDynSkillIndex.push_back(i);
					bNormal = false;
					break; // 
				}
			}			
		}
		if (bNormal)
			m_vecNorSkillIndex.push_back(i);
	}
	
	
	if(m_vecDynSkillIndex.size()>GP_PET_NATURE_SKILL_NUM)
	{
		ASSERT(FALSE);
		a_LogOutput(1, "CECPetData::Init, pet(%d) dyn skill error, count is %d", m_tid,m_vecDynSkillIndex.size());
	}
	if (m_vecNorSkillIndex.size()>GP_PET_SKILL_NUM)
	{
		ASSERT(FALSE);
		a_LogOutput(1, "CECPetData::Init, pet(%d) normal skill error, count is %d", m_tid,m_vecNorSkillIndex.size());
	}

	m_iAtkRation = Info.evo_prop.r_attack;
	m_iDefRation = Info.evo_prop.r_defense;
	m_iHpRation = Info.evo_prop.r_hp;
	m_iAtkLvlRation = Info.evo_prop.r_atk_lvl;
	m_iDefLvlRation = Info.evo_prop.r_def_lvl;
	m_iNature = Info.evo_prop.nature;
	

	return true;
}

//	Add experience
int CECPetData::AddExp(int iExp)
{
	m_iExp += iExp;
	return m_iExp;
}

//	Level up
int CECPetData::LevelUp(int iLevel, int iNewExp)
{
	m_iLevel = iLevel;
	m_iExp = iNewExp;
	return m_iLevel;
}

//	Tick routine
bool CECPetData::Tick(DWORD dwDeltaTime)
{
	//	Update cool time
	for (int i=0; i < GP_PET_SKILL_NUM; i++)
	{
		PETSKILL& s = m_aSkills[i];
		if (s.idSkill && s.iCoolMax && s.iCoolCnt)
		{
			s.iCoolCnt -= dwDeltaTime;
			a_ClampFloor(s.iCoolCnt, 0);
		}
	}

	return true;
}

const CECPetData::PETSKILL* CECPetData::GetSkill(CECPetData::SKILLTYPE iType,int n)
{
	if (iType == EM_SKILL_DEFAULT)
	{
		if (n >= 0 && n < GP_PET_SKILL_NUM)
			return &m_aSkills[n];
	}
	else if (iType == EM_SKILL_NORMAL)
	{
		if (n >= 0 && n < GP_PET_SKILL_NUM && n < (int)m_vecNorSkillIndex.size())
			return &m_aSkills[m_vecNorSkillIndex[n]];
	}
	else if (iType == EM_SKILL_NATURE)
	{
		if (n >= 0 && n < GP_PET_NATURE_SKILL_NUM && n<(int)m_vecDynSkillIndex.size())
			return &m_aSkills[m_vecDynSkillIndex[n]];
	}
	else if (iType == EM_SKILL_SPECIAL)
	{
		if(m_iSpecialSkillIndex>=0)
			return &m_aSkills[m_iSpecialSkillIndex];
	}
	
	//	ASSERT(0);
	return NULL;
}
//	Get skill by id
const CECPetData::PETSKILL* CECPetData::GetSkillByID(int id)
{
	for (int i=0; i < GP_PET_SKILL_NUM; i++)
	{
		PETSKILL& s = m_aSkills[i];
		if (s.idSkill == id)
			return &s;
	}

	return NULL;
}

//	Get valid skill number
int CECPetData::GetSkillNum(CECPetData::SKILLTYPE iType)
{
	int i, iCount = 0;

	if (iType == EM_SKILL_DEFAULT)
	{
		for (i=0; i < GP_PET_SKILL_NUM; i++)
		{
			if (m_aSkills[i].idSkill)
				iCount++;
		}

		return iCount;
	}
	else if (iType == EM_SKILL_NORMAL)
	{
		return m_vecNorSkillIndex.size();
	}
	else if (iType == EM_SKILL_NATURE)
	{
		return m_vecDynSkillIndex.size();
	}
	else if (iType == EM_SKILL_SPECIAL)
	{
		return 1;
	}
	else
	{
		ASSERT(FALSE);
		a_LogOutput(1, "CECPetData::GetSkillNum, type error (%d)", iType);
	}

	return 0; // 
}

bool CECPetData::IsFollowPet()const
{
	return m_pDBEssence
		&& m_pDBEssence->id_type == 8783;
}

bool CECPetData::IsMountPet()const
{
	return m_pDBEssence
		&& m_pDBEssence->id_type == 8781;
}

bool CECPetData::IsCombatPet()const
{
	return m_pDBEssence
		&& m_pDBEssence->id_type == 8782;
}

bool CECPetData::IsSummonPet()const
{
	return m_pDBEssence
		&& m_pDBEssence->id_type == 28752;
}

bool CECPetData::IsPlantPet()const
{
	return m_pDBEssence
		&& m_pDBEssence->id_type == 28913;
}
bool CECPetData::IsEvolutionPet() const 
{
	return m_pDBEssence && m_pDBEssence->id_type == 37698;
}
AWString CECPetData::GetFoodStr()const
{
	//	仅用于骑宠战宠
	//
	AWString strFood;

	const PET_ESSENCE *pDB = GetDBEssence();
	if (pDB)
	{
		CECStringTab* pDescTab = g_pGame->GetItemDesc();
		
		for (int i = 0; i < MAX_PET_FOOD; i++)
		{
			if (pDB->food_mask & (1 << i))
			{
				if( strFood != _AL(""))
					strFood += _AL(",");
				strFood += pDescTab->GetWideString(ITEMDESC_FOOD_GRASS + i);
			}
		}
	}
	
	return strFood;
}
AWString CECPetData::GetNature()
{
	elementdataman* pDB = g_pGame->GetElementDataMan();
	DATA_TYPE DataType;
	const void* pDBData = pDB->get_data_ptr(m_iNature, ID_SPACE_CONFIG, DataType);
	if (pDBData)
	{
		if (DataType == DT_PET_EVOLVED_SKILL_CONFIG)
		{
			const PET_EVOLVED_SKILL_CONFIG* pSkillConfig = (const PET_EVOLVED_SKILL_CONFIG*)pDBData;			
			if (pSkillConfig)
			{
				return pSkillConfig->name;	
			}		
		}
	}
	return _AL("");
}
AWString CECPetData::GetMoveSpeedStr()const
{
	//	通用
	//
	AWString str;

	const PET_ESSENCE *pDB = GetDBEssence();
	if (pDB)
	{
		float speed = pDB->speed_a + pDB->speed_b * (GetLevel() - 1);
		str.Format(_AL("%3.1f %s"), speed, g_pGame->GetStringFromTable(280));
	}

	return str;
}

AWString CECPetData::GetHungerStr()const
{
	//	仅用于骑宠战宠
	//
	AWString str;

	int nHunger(0);	
	if( GetHunger() == HUNGER_LEVEL_0 )
		nHunger = 0;
	else if( GetHunger() == HUNGER_LEVEL_1 )
		nHunger = 1;
	else if( GetHunger() == HUNGER_LEVEL_2 )
		nHunger = 2;
	else if( GetHunger() <= HUNGER_LEVEL_4 )
		nHunger = 3;
	else if( GetHunger() <= HUNGER_LEVEL_7 )
		nHunger = 4;
	else
		nHunger = 5;
	str = g_pGame->GetStringFromTable(3611 + nHunger);

	return str;
}

int CECPetData::GetLoyalty()const
{
	//	仅用于骑宠战宠
	//
	int nLoyalty(0);
	if( GetIntimacy() <= 50 )
		nLoyalty = 0;
	else if( GetIntimacy() <= 150 )
		nLoyalty = 1;
	else if( GetIntimacy() <= 500 )
		nLoyalty = 2;
	else
		nLoyalty = 3;
	return nLoyalty;
}

AWString CECPetData::GetLoyaltyStr()const
{
	//	仅用于骑宠战宠
	//
	AWString str;
	str.Format(_AL("%s %d"), g_pGame->GetStringFromTable(3601 + GetLoyalty()), GetIntimacy());
	return str;
}

int CECPetData::GetLevelRequirement()const
{
	//	通用
	int ret(-1);
	if (GetDBEssence())
		ret = GetDBEssence()->level_require;
	return ret;
}

int CECPetData::GetMaxHP()const
{
	//	仅限观赏宠、战宠、进化宠
	//
	int ret(-1);

	const PET_ESSENCE *pDB = GetDBEssence();
	if (pDB)
		ret = int(pDB->hp_a * (GetLevel() - pDB->hp_b * pDB->level_require + pDB->hp_c));

	return ret;
}

int CECPetData::GetAttack()const
{
	//	仅限战宠
	int ret(-1);
	
	const PET_ESSENCE *pDB = GetDBEssence();
	if (pDB)
	{
		static const short hintAttack[] = { 60, 80, 100, 120 };
		ret = int((pDB->damage_a * (pDB->damage_b * GetLevel() * GetLevel() + pDB->damage_c * GetLevel() + pDB->damage_d))) * hintAttack[GetLoyalty()] / 100;
	}

	return ret;
}

int CECPetData::GetPhyicDefence()const
{
	//	仅限战宠
	int ret(-1);
	
	const PET_ESSENCE *pDB = GetDBEssence();
	if (pDB)
		ret = int(pDB->physic_defence_a * (pDB->physic_defence_b * (GetLevel() - pDB->physic_defence_c * pDB->level_require) + pDB->physic_defence_d));

	return ret;
}

int CECPetData::GetMagicDefence()const
{
	//	仅限战宠
	int ret(-1);
	
	const PET_ESSENCE *pDB = GetDBEssence();
	if (pDB)
		ret = int(pDB->magic_defence_a * (pDB->magic_defence_b * (GetLevel() - pDB->magic_defence_c * pDB->level_require) + pDB->magic_defence_d));

	return ret;
}

int CECPetData::GetDefiniton()const
{
	//	仅限战宠
	int ret(-1);
	
	const PET_ESSENCE *pDB = GetDBEssence();
	if (pDB)
		ret = int(pDB->attack_a * (GetLevel() - pDB->attack_b * pDB->level_require + pDB->attack_c));

	return ret;
}

int CECPetData::GetEvade()const
{
	//	仅限战宠
	int ret(-1);
	
	const PET_ESSENCE *pDB = GetDBEssence();
	if (pDB)
		ret = int(pDB->armor_a * (GetLevel() - pDB->armor_b * pDB->level_require + pDB->armor_c));

	return ret;
}

AWString CECPetData::GetAttackSpeedStr()const
{
	//	仅用于战宠
	//
	AWString str;

	const PET_ESSENCE *pDB = GetDBEssence();
	if (pDB)
		str.Format(_AL("%4.2f%s"), 1.0f / pDB->attack_speed, g_pGame->GetStringFromTable(279));

	return str;
}

AWString CECPetData::GetInhabitTypeStr()const
{
	//	仅用于战宠
	//
	AWString str;

	const PET_ESSENCE *pDB = GetDBEssence();
	if (pDB)
		str = g_pGame->GetStringFromTable(1401 + pDB->inhabit_type);

	return str;
}

int	 CECPetData::GetMaxAtkRation() const
{
	return m_pDBEssence ? m_pDBEssence->attack_inherit_max_rate : 0;
}

int	 CECPetData::GetMaxDefRation() const
{
	return m_pDBEssence ? m_pDBEssence->defence_inherit_max_rate : 0;
}

int	 CECPetData::GetMaxHpRation() const
{
	return m_pDBEssence ? m_pDBEssence->hp_inherit_max_rate : 0;
}

int	 CECPetData::GetMaxAtkLvlRation() const
{
	return m_pDBEssence ? m_pDBEssence->attack_level_inherit_max_rate : 0;
}

int	 CECPetData::GetMaxDefLvlRation() const
{
	return m_pDBEssence ? m_pDBEssence->defence_level_inherit_max_rate : 0;
}