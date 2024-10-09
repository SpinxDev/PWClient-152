/*
 * FILE: EC_Skill.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2005/1/17
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include "ElementSkill.h"

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CECSkill
//	
///////////////////////////////////////////////////////////////////////////

struct SkillArrayWrapper
{
	typedef std::pair<unsigned int, int> IDLevelPair;
	typedef std::vector<IDLevelPair> SkillArray;
	
	SkillArrayWrapper(){}
	SkillArrayWrapper(const SkillArray &rhs)
		: m_array(rhs)
	{
		RemoveInvalid();
	}
	
	void RemoveInvalid();
	bool Find(unsigned int id)const;
	bool Empty()const{ return m_array.empty(); }
	size_t Count()const{ return m_array.size(); }
	unsigned int operator[](int index)const{ return m_array[index].first; }
	
	SkillArray	m_array;
};

class CECSkill
{
public:		//	Types

	//	Skill type
	enum
	{
		TYPE_ATTACK	= 1,	//	主动攻击
		TYPE_BLESS,			//	主动祝福
		TYPE_CURSE,			//	主动诅咒
		TYPE_SUMMON,		//	召唤
		TYPE_PASSIVE,		//	被动
		TYPE_ENABLED,		//	激活
		TYPE_LIVE,			//	生活
		TYPE_FLASHMOVE,		//	瞬移
		TYPE_PRODUCE,		//	生产
		TYPE_BLESSPET,		//	宠物祝福
		TYPE_NEUTRALBLESS,	//	中性祝福
	};

	//	Range type
	enum
	{
		RANGE_POINT = 0,	//	点 
		RANGE_LINE,			//	线 
		RANGE_SELFSPHERE,	//	自身为中心的球
		RANGE_TARGETSPHERE, //	目标为中心的球
		RANGE_TAPER,		//	圆锥
		RANGE_SLEF,			//	自身
	};

public:		//	Constructor and Destructor

	CECSkill(int id, int iLevel);
	virtual ~CECSkill();

public:		//	Attributes

public:		//	Operations

	//	Tick routine
	void Tick();
	//	Skill level up
	void LevelUp();
	//  Set Skill level
	void SetLevel(int iLevel);

	//	Start into cooling state
	void StartCooling(int iTotalTime, int iStartCnt);
	//	Ready to be cast ?
	bool ReadyToCast() { return !m_bCooling; }
	//	Get cooling time counter
	int GetCoolingCnt() { return m_iCoolCnt; }
	//	Get total cooling time
	int GetCoolingTime() { return m_iCoolTime; }
	//	Start charging
	void StartCharging(int iChargeMax);
	//	End charging
	void EndCharging() { m_bCharging = false; }
	//	Get charging flag
	bool IsCharging() { return m_bCharging; }
	//	Get charging counter
	int GetChargingCnt() { return m_iChargeCnt; }
	//	Get charging maximum count
	int GetChargingMax() { return m_iChargeMax; }
	//	Change full
	bool ChargeFull();

	//	Get skill ID
	int GetSkillID() { return m_idSkill; }
	//	Get skill level
	int GetSkillLevel() { return m_iLevel; }
	int GetCoreCoolingTime() { return m_pSkillCore->GetCoolingTime(); }
	int GetExecuteTime() { return m_pSkillCore->GetExecuteTime(); }
	int GetType() { return m_pSkillCore->GetType(); }
	int GetRangeType() { return m_pSkillCore->GetRangeType(); }
	float GetCastRange(float fAtkDist, float fPrayDistancePlus) { return m_pSkillCore->GetPrayRange(fAtkDist, fPrayDistancePlus); }
	int GetTargetType() { return m_pSkillCore->GetTargetType(); }
	int GetCastEnv() { return m_pSkillCore->GetCastEnv(); }
	int* GetRequiredGenius() { return m_pSkillCore->GetRequiredGenius(m_idSkill); }
	int GetShowOrder() { return m_pSkillCore->GetShowOrder(); }
	bool IsChargeable() { return m_pSkillCore->IsWarmup(); }
	bool ValidWeapon(int idWeapon) { return m_pSkillCore->ValidWeapon(idWeapon); }
	bool ValidShape(int iShape) { return m_pSkillCore->IsValidForm((char)iShape); }
	bool ChangeToMelee() { return m_pSkillCore->IsAutoAttack(); }
	bool IsInstant() { return m_pSkillCore->IsInstant(); }
	bool IsDurative() { return m_pSkillCore->IsDurative(); }
	SkillArrayWrapper GetJunior() { return m_pSkillCore->GetJunior(); }

	int GetRequiredLevel() { return m_pSkillCore->GetRequiredLevel(); }
	int GetRequiredSp() { return m_pSkillCore->GetRequiredSp(); }
	int GetRequiredBook() { return m_pSkillCore->GetRequiredBook(); }
	SkillArrayWrapper GetRequiredSkill() { return m_pSkillCore->GetRequiredSkill(); }
	int GetRequiredMoney() { return m_pSkillCore->GetRequiredMoney(); }
	int GetRequiredItem(){ return m_pSkillCore->GetItemCost(); }

	int GetRequiredMP() { return m_pSkillCore->GetMpCost(); }
	int GetRequiredAP() { return m_pSkillCore->GetApCost(); }
	int GetRequiredArrow() { return m_pSkillCore->GetArrowCost(); }

	//获取公共冷却mask， 从右往左bit0-4为技能冷却；bit5-9为物品冷却
	int GetCommonCoolDown() { return m_pSkillCore->GetCommonCoolDown(); }

	//获取公共冷却时间，单位毫秒
	int GetCommonCoolDownTime() { return m_pSkillCore->GetCommonCoolDownTime(); }

	//  Check skill type
	bool IsGoblinSkill() const;
	bool IsPlayerSkill() const;
	bool IsGeneralSkill()const;
	bool IsPositiveSkill();

	int  GetCls() const { return m_pSkillCore->GetCls(); }

	//  修真等级
	int GetRank() { return m_pSkillCore->GetRank(); }

	//  技能最大等级
	int GetMaxLevel() { return m_pSkillCore->GetMaxLevel(); }

protected:	//	Attributes

	GNET::ElementSkill*	m_pSkillCore;

	int		m_idSkill;		//	Skill ID
	int		m_iLevel;		//	Skill level
	int		m_iCoolCnt;		//	Cooling time counter
	int		m_iCoolTime;	//	Total cooling time
	bool	m_bCooling;		//	In cooling state
	int		m_iChargeCnt;	//	Charging time counter
	int		m_iChargeMax;	//	Charging time maximum count value
	bool	m_bCharging;	//	In charging state

protected:	//	Operations

};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////



