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

#include "EC_Global.h"
#include "EC_Skill.h"
#include "EC_Game.h"
#include "../CElementClient/EC_RoleTypes.h"

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


///////////////////////////////////////////////////////////////////////////
//	
//	class CECSkillStr
//	
///////////////////////////////////////////////////////////////////////////

class CECSkillStr : public GNET::SkillStr
{
public:

	virtual wchar_t* Find(int id) const
	{
		CECStringTab* pStrTab = g_pGame->GetSkillDesc();
		const wchar_t* str = pStrTab->GetWideString(id);
		return str ? (wchar_t*)str : L"";
	}
};

static CECSkillStr l_SkillStr;


///////////////////////////////////////////////////////////////////////////
//	
//	Implement SkillArrayWrapper
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
	if (m_bCooling)
	{
		//	In cooling state
		m_iCoolCnt -= (int)g_pGame->GetRealTickTime();
		if (m_iCoolCnt <= 0)
		{
			m_iCoolCnt = 0;
			m_bCooling = false;

			//	TODO: do something here ?
		}
	}

	if (m_bCharging)
	{
		//	In charging state
		m_iChargeCnt += (int)g_pGame->GetRealTickTime();
		if (m_iChargeCnt >= m_iChargeMax)
		{
			m_iChargeCnt = m_iChargeMax;
			m_bCharging	 = false;

			//	TODO: do something here ?
		}
	}
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
const wchar_t* CECSkill::GetDesc()
{
	//	Note: use static buffer to receive description, so this function isn't
	//		thread safe.
/*	static wchar_t szDesc[512];
	unsigned int dwLen = 512;
	GNET::ElementSkill::GetDescription(m_idSkill, iLevel, szDesc, &dwLen);
	ASSERT(dwLen < 512);
	return szDesc;
*/
	static wchar_t szDesc[1024];
	m_pSkillCore->GetIntroduction(szDesc, 1024, l_SkillStr);
	return szDesc;
}

const wchar_t * CECSkill::GetNameDisplay()
{
	return g_pGame->GetSkillDesc()->GetWideString(GetSkillID() * 10);
}

//	Get skill description
bool CECSkill::GetDesc(int idSkill, int iLevel, ACHAR* szText, int iBufLen)
{
	if (!szText || !iBufLen)
		return false;

	CECSkill* pSkill = new CECSkill(idSkill, iLevel);
	if (!pSkill)
	{
		szText[0] = '\0';
		return false;
	}
	
	const wchar_t* sz = pSkill->GetDesc();
	int iLen = a_strlen(sz);
	if (iLen >= iBufLen)
	{
		delete pSkill;
		ASSERT(iLen < iBufLen);
		return false;
	}

	a_strcpy(szText, sz);
	delete pSkill;

	return true;
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