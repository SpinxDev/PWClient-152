/*
 * FILE: EC_Skill.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2005/1/17
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */


#include "Common.h"
#include "EC_Skill.h"
#include "EC_RoleTypes.h"

#include "AChar.h"
#include <algorithm>

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

static bool CanRemove(const SkillArrayWrapper::IDLevelPair & rhs)
{
	return rhs.first == 0;
}

void SkillArrayWrapper::RemoveInvalid()
{
	//	m_array 中可能有0值，此处除去
	m_array.erase(std::remove_if(m_array.begin(), m_array.end(), CanRemove), m_array.end());
}

bool SkillArrayWrapper::Find(unsigned int id)const
{
	for (SkillArray::const_iterator cit = m_array.begin(); cit != m_array.end(); ++ cit)
	{
		if (cit->first == id){
			return true;
		}
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECSkill
//	
///////////////////////////////////////////////////////////////////////////

CECSkill::CECSkill(int id, int iLevel)
{
	if (!(m_pSkillCore = GNET::ElementSkill::Create(id, iLevel)))
	{
		ASSERT(m_pSkillCore);
		m_pSkillCore = GNET::ElementSkill::Create(1, 1);
		a_LogOutput(1, "Invalid skill id!");
	}

	m_idSkill		= id;
	m_iLevel		= iLevel;
	m_iCoolTime		= m_pSkillCore->GetCoolingTime();
	m_iCoolCnt		= 0;
	m_bCooling		= false;
	m_iChargeCnt	= 0;
	m_iChargeMax	= 0;
	m_bCharging		= false;
}

CECSkill::~CECSkill()
{
	if (m_pSkillCore)
		m_pSkillCore->Destroy();
}

//	Start into cooling state
//	iTotalTime: total cooling time, 0 means to use cooling time in database
void CECSkill::StartCooling(int iTotalTime, int iStartCnt)
{
	m_iCoolTime	= iTotalTime ? iTotalTime : GetCoreCoolingTime();
	m_iCoolCnt	= iStartCnt;
	m_bCooling	= true;
}

//	Start charging
void CECSkill::StartCharging(int iChargeMax)
{
	if (m_pSkillCore->IsWarmup())
	{
		m_iChargeMax = iChargeMax;
		m_iChargeCnt = 0;
		m_bCharging	 = true;
	}
}

//	Change full
bool CECSkill::ChargeFull()
{
	return m_pSkillCore->IsWarmup() && m_iChargeCnt >= m_iChargeMax;
}

//	Tick routine
void CECSkill::Tick()
{
// 	if (m_bCooling)
// 	{
// 		//	In cooling state
// 		m_iCoolCnt -= (int)g_pGame->GetRealTickTime();
// 		if (m_iCoolCnt <= 0)
// 		{
// 			m_iCoolCnt = 0;
// 			m_bCooling = false;
// 
// 			//	TODO: do something here ?
// 		}
// 	}
// 
// 	if (m_bCharging)
// 	{
// 		//	In charging state
// 		m_iChargeCnt += (int)g_pGame->GetRealTickTime();
// 		if (m_iChargeCnt >= m_iChargeMax)
// 		{
// 			m_iChargeCnt = m_iChargeMax;
// 			m_bCharging	 = false;
// 
// 			//	TODO: do something here ?
// 		}
// 	}
}

//	Skill level up
void CECSkill::LevelUp()
{ 
	m_iLevel++;
	m_pSkillCore->SetLevel(m_iLevel);
}

//  Set skill level
void CECSkill::SetLevel(int iLevel)
{
	m_iLevel = iLevel;
	m_pSkillCore->SetLevel(m_iLevel);
}

bool CECSkill::IsGoblinSkill() const
{
	return m_pSkillCore->GetCls() == 258;
}

bool CECSkill::IsPlayerSkill() const
{
	return m_pSkillCore->GetCls() >= 0 && m_pSkillCore->GetCls() < NUM_PROFESSION;
}

bool CECSkill::IsGeneralSkill()const
{
	return GetCls() == 255;
}

bool CECSkill::IsPositiveSkill()
{
	int t = GetType();
	return t != CECSkill::TYPE_PASSIVE
		&& t != CECSkill::TYPE_PRODUCE
		&& t != CECSkill::TYPE_LIVE;
}