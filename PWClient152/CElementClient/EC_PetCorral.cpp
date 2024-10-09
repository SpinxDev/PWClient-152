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
#include "EC_GameRun.h"
#include "EC_GPDataType.h"
#include "EC_UIManager.h"
#include "elementdataman.h"
#include "EC_Inventory.h"
#include "EC_GameUIMan.h"
#include "EC_HostPlayer.h"
#include "EC_GameSession.h"
#include "EC_Skill.h"
#include "EC_Utility.h"

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
	m_cntAutoSkill.SetPeriod(500);
	
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
			g_pGame->GetGameRun()->GetUIManager()->FilterBadWords(m_strName);
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
	m_iSpecialSkillIndex = -1;	

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
				ASSERT(FALSE);
				a_LogOutput(1,"CECPetData::Init, pet(%d) which is not evolution should not has special skill(%d)",m_tid,skill_id);
			}
			continue;
		}

		bool bNormal = true;
		if (pDynSkills)
		{
			for (int j=0;j<2;j++) // 俩个性格技能
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

//	Set skill cool time
void CECPetData::SetSkillCoolTime(int iCoolIdx, int iTime)
{
	if (iCoolIdx <= GP_CT_SKILL_START)
		return;

	int idSkill = iCoolIdx - GP_CT_SKILL_START;
	int i=0;
	for (i=0; i < GP_PET_SKILL_NUM; i++)
	{
		PETSKILL& s = m_aSkills[i];
		if (s.idSkill == idSkill)
		{
			s.iCoolMax	= iTime;
			s.iCoolCnt	= iTime;
			return;
		}
	}
}

//	Get skill cool time
int CECPetData::GetSkillCoolTime(CECPetData::SKILLTYPE iType, int iSkillIdx, int* piMax/* NULL */)
{
	if (iType == EM_SKILL_DEFAULT)
	{
		if (iSkillIdx >= 0 && iSkillIdx < GP_PET_SKILL_NUM)
		{
			const PETSKILL& s = m_aSkills[iSkillIdx];
			if (piMax) *piMax = s.iCoolMax;
			return s.iCoolCnt;
		}
	}
	else if (iType == EM_SKILL_NORMAL)
	{
		if (iSkillIdx >= 0 && iSkillIdx < GP_PET_SKILL_NUM && iSkillIdx < (int)m_vecNorSkillIndex.size())
		{
			const PETSKILL& s = m_aSkills[m_vecNorSkillIndex[iSkillIdx]];
			if (piMax) *piMax = s.iCoolMax;
			return s.iCoolCnt;
		}
	}
	else if (iType == EM_SKILL_NATURE)
	{
		if (iSkillIdx >= 0 && iSkillIdx < GP_PET_NATURE_SKILL_NUM && iSkillIdx<(int)m_vecDynSkillIndex.size())
		{
			const PETSKILL& s = m_aSkills[m_vecDynSkillIndex[iSkillIdx]];
			if (piMax) *piMax = s.iCoolMax;
			return s.iCoolCnt;
		}
	}
	else if (iType == EM_SKILL_SPECIAL)
	{
		if (m_iSpecialSkillIndex>=0)
		{
			const PETSKILL& s = m_aSkills[m_iSpecialSkillIndex];
			if (piMax) *piMax = s.iCoolMax;
			return s.iCoolCnt;
		}
	}

	if (piMax) *piMax = 0;
	return 0;
}


//	Tick routine
bool CECPetData::Tick(DWORD dwDeltaTime)
{
	//	Update cool time
	int i=0;
	for (i=0; i < GP_PET_SKILL_NUM; i++)
	{
		PETSKILL& s = m_aSkills[i];
		if (s.idSkill && s.iCoolMax && s.iCoolCnt)
		{
			s.iCoolCnt -= dwDeltaTime;
			a_ClampFloor(s.iCoolCnt, 0);
		}
	}

	//  Auto cast skill
	if( m_cntAutoSkill.IncCounter(dwDeltaTime) )
	{
		CastAutoSkill();
		m_cntAutoSkill.Reset();
	}

	return true;
}

//	Calculate max hp of pet
int CECPetData::CalcMaxHP()
{
	int iVal = 1;
	if (GetHPFactor() > 1e-6f)
		iVal = (int) (GetHP() / GetHPFactor());
	return iVal;
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
	int i = 0;
	for (i=0; i < GP_PET_SKILL_NUM; i++)
	{
		PETSKILL& s = m_aSkills[i];
		if (s.idSkill == id)
		{
			return &s;
		}
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
		ASSERT(FALSE);

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
int CECPetData::GetMaxExp()
{
	DATA_TYPE DataType;
	PLAYER_LEVELEXP_CONFIG* PetLevelUpExp = (PLAYER_LEVELEXP_CONFIG*)g_pGame->GetElementDataMan()->
		get_data_ptr(592, ID_SPACE_CONFIG, DataType);
	
	return PetLevelUpExp ? PetLevelUpExp->exp[m_iLevel - 1] : 0;
}
bool CECPetData::CanEvolution()const 
{
	return m_pDBEssence && m_pDBEssence->id_pet_egg_evolved != 0;
}
int CECPetData::GetEvolutionID() const 
{
	if(!m_pDBEssence) return 0;

	DATA_TYPE DataType;
	PET_EGG_ESSENCE* pDB = (PET_EGG_ESSENCE*)g_pGame->GetElementDataMan()->
		get_data_ptr(m_pDBEssence->id_pet_egg_evolved, ID_SPACE_ESSENCE, DataType);

	return pDB ? pDB->id_pet:0;
}
int CECPetData::GetEvolutionID(int pet_tid) 
{
	elementdataman* pDB = g_pGame->GetElementDataMan();	
	DATA_TYPE DataType;

	PET_ESSENCE* pESS = (PET_ESSENCE*)pDB->get_data_ptr(pet_tid, ID_SPACE_ESSENCE, DataType);
	if (pESS)
	{
		PET_EGG_ESSENCE* pEgg = (PET_EGG_ESSENCE*)pDB->get_data_ptr(pESS->id_pet_egg_evolved, ID_SPACE_ESSENCE, DataType);
		return pEgg ? pEgg->id_pet:0;
	}
	
	return 0;
}

ACString CECPetData::GetNature()
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

void CECPetData::AddAutoSkill(int skill_id)
{
	m_aAutoSkills.push_back(skill_id);
}

void CECPetData::CastAutoSkill()
{
#pragma pack(1)
	
	struct CMDPARAM
	{
		int idSkill;
		BYTE byPVPMask;
	};
	
#pragma pack()

	if( m_aAutoSkills.empty() )
		return;

	int skill_id = m_aAutoSkills.back();
	CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
	int idPet = pHost->GetPetCorral()->GetActivePetNPCID();

	CMDPARAM param;
	param.idSkill = skill_id;
	param.byPVPMask = glb_BuildPVPMask(glb_GetForceAttackFlag(NULL));

	int iSkillType = GNET::ElementSkill::GetType(param.idSkill);
	int idTarget = 0;

	if (iSkillType == GNET::TYPE_BLESSPET)
	{
		idTarget = idPet;
	}
	else
	{
		idTarget = pHost->GetSelectedTarget();

		if (iSkillType == GNET::TYPE_ATTACK || iSkillType == GNET::TYPE_CURSE)
		{
			bool bForctAttack = glb_GetForceAttackFlag(NULL);
			if (pHost->AttackableJudge(idTarget, bForctAttack) != 1)
			{
				OnAutoCastOver(skill_id);
				return;
			}
		}
	}

	g_pGame->GetGameSession()->c2s_CmdPetCtrl(idTarget, 4, &param, sizeof(param));
}

void CECPetData::OnAutoCastOver(int skill_id)
{
	std::vector<int>::iterator it;
	if( (it = std::find(m_aAutoSkills.begin(), m_aAutoSkills.end(), skill_id))
		!= m_aAutoSkills.end() )
		m_aAutoSkills.erase(it);
}

void CECPetData::OnPetDead()
{
	m_aAutoSkills.clear();
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECPlantPetData
//	
///////////////////////////////////////////////////////////////////////////
CECPlantPetData::CECPlantPetData()
{
	m_tid		= 0;
	m_nid		= 0;
	m_lifeTime	= 0;
	m_lifeTimeLeft = 0;
	m_HPFactor	= 1.0f;
	m_HP		= 0;
	m_MPFactor	= 1.0f;
	m_MP		= 0;
}

void CECPlantPetData::Init(const S2C::cmd_summon_plant_pet &rhs)
{
	m_tid		= rhs.plant_tid;
	m_nid		= rhs.plant_nid;
	m_lifeTime	= rhs.life_time;
	m_lifeTimeLeft = m_lifeTime * 1000;
}

void CECPlantPetData::Info(const S2C::cmd_plant_pet_hp_notify &rhs)
{
	if (rhs.plant_nid == m_nid)
	{
		m_HPFactor	= rhs.hp_factor;
		m_HP		= rhs.cur_hp;
		m_MPFactor	= rhs.mp_factor;
		m_MP		= rhs.cur_mp;
	}
	else
	{
		ASSERT(rhs.plant_nid == m_nid);
	}
}

void CECPlantPetData::Tick(DWORD dwDeltaTime)
{
	if (m_lifeTime <= 0 || m_lifeTimeLeft <= 0)
		return;
	if (m_lifeTimeLeft >= (int)dwDeltaTime)
		m_lifeTimeLeft -= (int)dwDeltaTime;
	else
		m_lifeTimeLeft = 0;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECPetCorral
//	
///////////////////////////////////////////////////////////////////////////

CECPetCorral::CECPetCorral()
{
	m_iActivePet	= -1;
	m_iPetSlotNum	= 1;
	m_iMoveMode		= MOVE_FOLLOW;
	m_iAttackMode	= ATK_DEFENSE;
	m_nidPet		= 0;
	m_iPetLifeTime	= 0;
	m_bHasInit		= false;
	
	memset(m_aPetSlots, 0, sizeof (m_aPetSlots));

	elementdataman* pDB = g_pGame->GetElementDataMan();
	DATA_TYPE DataType;
	unsigned int id = pDB->get_first_data_id(ID_SPACE_CONFIG, DataType);
	while( id )
	{
		if( DataType == DT_PET_EVOLVE_CONFIG )
		{
			m_pDBEvoConfig = (PET_EVOLVE_CONFIG *)pDB->get_data_ptr(id, ID_SPACE_CONFIG, DataType);
			break;
		}
		id = pDB->get_next_data_id(ID_SPACE_CONFIG, DataType);
	}
}

CECPetCorral::~CECPetCorral()
{
	CECPetCorral::RemoveAll();
}

//	Add a pet
bool CECPetCorral::AddPet(int iSlot, const S2C::info_pet& Info)
{
	if (iSlot < 0 || iSlot >= m_iPetSlotNum && (iSlot < MAX_SLOTNUM || iSlot >= MAX_SLOTNUM2))
	{
		ASSERT(iSlot >= 0 && iSlot < m_iPetSlotNum);
		return false;
	}

	if (m_aPetSlots[iSlot])
	{
		ASSERT(!m_aPetSlots[iSlot]);
		delete m_aPetSlots[iSlot];
		m_aPetSlots[iSlot] = NULL;
	}

	CECPetData* pPet = new CECPetData;
	if (!pPet)
	{
		glb_ErrorOutput(ECERR_NOTENOUGHMEMORY, "CECPetCorral::AddPet", __LINE__);
		return false;
	}

	pPet->Init(Info);

	m_aPetSlots[iSlot] = pPet;

	return true;
}

//	Free a pet
void CECPetCorral::FreePet(int iSlot, int idPet)
{
	if (iSlot < 0 || iSlot >= m_iPetSlotNum && (iSlot < MAX_SLOTNUM || iSlot >= MAX_SLOTNUM2))
	{
		ASSERT(iSlot >= 0 && iSlot < m_iPetSlotNum);
		return;
	}

	CECPetData* pPet = m_aPetSlots[iSlot];
	if (pPet)
	{
		ASSERT(pPet->GetTemplateID() == idPet);
		delete pPet;
		m_aPetSlots[iSlot] = NULL;
	}
}

//	Remove all pets
void CECPetCorral::RemoveAll()
{
	for (int i=0; i < MAX_SLOTNUM2; i++)
	{
		if (m_aPetSlots[i])
		{
			delete m_aPetSlots[i];
			m_aPetSlots[i] = NULL;
		}
	}

	m_Plants.clear();
}

//	Update pets data in corral
void CECPetCorral::UpdatePets(const S2C::cmd_pet_room& cmd)
{
	using namespace S2C;

	const BYTE* pData = (const BYTE*)(&cmd+1);

	for (int i=0; i < cmd.count; i++)
	{
		int iSlot = *(int*)pData;
		pData += sizeof (int);
		const info_pet* pInfo = (const info_pet*)pData;
		pData += sizeof (info_pet);

		if (iSlot < 0 || iSlot >= m_iPetSlotNum && (iSlot < MAX_SLOTNUM || iSlot >= MAX_SLOTNUM2))
		{
			ASSERT(iSlot >= 0 && iSlot < m_iPetSlotNum);
			continue;
		}

		//	Free old pet
		if (m_aPetSlots[iSlot])
		{
			delete m_aPetSlots[iSlot];
			m_aPetSlots[iSlot] = NULL;
		}

		AddPet(iSlot, *pInfo);
	}
}

//	Magnify pet slots
void CECPetCorral::MagnifyPetSlots(int iNewNum)
{
	if (iNewNum > MAX_SLOTNUM || m_iPetSlotNum > iNewNum)
	{
		ASSERT(iNewNum <= MAX_SLOTNUM && m_iPetSlotNum <= iNewNum);
		return;
	}

	m_iPetSlotNum = iNewNum;
}

//	Check whether corral has empty slots
int CECPetCorral::GetEmptySlotNum()
{
	int iCount = 0;

	for (int i=0; i < m_iPetSlotNum; i++)
	{
		if (!m_aPetSlots[i])
			iCount++;
	}

	return iCount;
}

//	Tick routine
bool CECPetCorral::Tick(DWORD dwDeltaTime)
{
	for (int i=0; i < m_iPetSlotNum; i++)
	{
		if (m_aPetSlots[i])
			m_aPetSlots[i]->Tick(dwDeltaTime);
	}

	for (int j = MAX_SLOTNUM; j < MAX_SLOTNUM2; ++ j)
	{
		if (m_aPetSlots[j])
			m_aPetSlots[j]->Tick(dwDeltaTime);
	}

	for (int k = 0; k < GetPlantCount(); ++ k)
	{
		CECPlantPetData *pPlant = GetPlant(k);
		if (pPlant)
			pPlant->Tick(dwDeltaTime);
	}

	return true;
}

void CECPetCorral::PlantPetEnter(const S2C::cmd_summon_plant_pet &rhs)
{
	CECPlantPetData plant;
	plant.Init(rhs);
	m_Plants.push_back(plant);
}

void CECPetCorral::PlantPetDisappear(const S2C::cmd_plant_pet_disapper &rhs)
{
	CECPlantPetData *pPlant = GetPlantByID(rhs.plant_nid);
	if (pPlant)
		m_Plants.erase(pPlant);
}

void CECPetCorral::PlantPetInfo(const S2C::cmd_plant_pet_hp_notify &rhs)
{
	CECPlantPetData *pPlant = GetPlantByID(rhs.plant_nid);
	if (pPlant)
		pPlant->Info(rhs);
}

int CECPetCorral::GetPlantCount()const
{
	return (int)m_Plants.size();
}

const CECPlantPetData * CECPetCorral::GetPlantByIDImpl(int nid)const
{
	for (PlantVec::const_iterator it = m_Plants.begin(); it != m_Plants.end(); ++ it)
	{
		const CECPlantPetData &plant = *it;
		if (plant.m_nid == nid)
			return &plant;
	}
	return NULL;
}

CECPlantPetData * CECPetCorral::GetPlantByID(int nid)
{
	const CECPlantPetData *pPlant = GetPlantByIDImpl(nid);
	return const_cast<CECPlantPetData *>(pPlant);
}

const CECPlantPetData * CECPetCorral::GetPlantByID(int nid)const
{
	return GetPlantByIDImpl(nid);
}

int CECPetCorral::GetPlantIndexByID(int nid)const
{
	for (int i = 0; i < (int)m_Plants.size(); ++ i)
	{
		const CECPlantPetData &plant = m_Plants[i];
		if (plant.m_nid == nid)
			return i;
	}
	return -1;
}

const CECPlantPetData * CECPetCorral::GetPlantImpl(int index)const
{
	if (index >= 0 && index < GetPlantCount())
		return &m_Plants[index];
	return NULL;
}

CECPlantPetData * CECPetCorral::GetPlant(int index)
{
	const CECPlantPetData *pPlant = GetPlantImpl(index);
	return const_cast<CECPlantPetData *>(pPlant);
}

const CECPlantPetData * CECPetCorral::GetPlant(int index)const
{
	return GetPlantImpl(index);
}

// 检查 进化、培训、洗髓的物品条件
bool CECPetCorral::CheckRebuildPetItemCond(int iPetIndex,int iSel, int type)
{	
	CECPetData * pPet = GetPetData(iPetIndex);
	if(!pPet) return false;
	
	int (*p)[2] = NULL;
	if (type == 0) // 进化
		p = GetPetEvoConfig()->cost[pPet->GetPetEssence()->cost_index].num_evolve;
	else if(type == 1) // 性格
		p = GetPetEvoConfig()->cost[pPet->GetPetEssence()->cost_index].num_rand_skill;
	else if(type == 2) // 洗髓
		p = GetPetEvoConfig()->cost[pPet->GetPetEssence()->cost_index].num_inherit;
	else 
	{
		ASSERT(FALSE);
		return false;
	}
	
	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	if(!pGameUI) return false;

	CECInventory* pack = g_pGame->GetGameRun()->GetHostPlayer()->GetPack();
	if (pack->GetItemTotalNum(37401) < p[iSel][0])
	{
		pGameUI->MessageBox("",pGameUI->GetStringFromTable(10110),MB_OK,A3DCOLORRGBA(255, 255, 255, 160));
		return false; // 麝灵珠不够
	}
	if (pack->GetItemTotalNum(12980) < p[iSel][1])
	{
		pGameUI->MessageBox("",pGameUI->GetStringFromTable(10111),MB_OK,A3DCOLORRGBA(255, 255, 255, 160));
		return false; // 乾坤石不够
	}
	
	return true;
}