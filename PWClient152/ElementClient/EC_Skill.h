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
		TYPE_ATTACK	= 1,	//	��������
		TYPE_BLESS,			//	����ף��
		TYPE_CURSE,			//	��������
		TYPE_SUMMON,		//	�ٻ�
		TYPE_PASSIVE,		//	����
		TYPE_ENABLED,		//	����
		TYPE_LIVE,			//	����
		TYPE_FLASHMOVE,		//	˲��
		TYPE_PRODUCE,		//	����
		TYPE_BLESSPET,		//	����ף��
		TYPE_NEUTRALBLESS,	//	����ף��
	};

	//	Range type
	enum
	{
		RANGE_POINT = 0,	//	�� 
		RANGE_LINE,			//	�� 
		RANGE_SELFSPHERE,	//	����Ϊ���ĵ���
		RANGE_TARGETSPHERE, //	Ŀ��Ϊ���ĵ���
		RANGE_TAPER,		//	Բ׶
		RANGE_SLEF,			//	����
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
	//	Get skill icon file
	const char* GetIconFile() { return m_pSkillCore->GetIcon(); }
	const wchar_t* GetName() { return m_pSkillCore->GetName(); }
	const wchar_t* GetNameDisplay();
	const wchar_t* GetDesc();
	int GetCoreCoolingTime() { return m_pSkillCore->GetCoolingTime(); }
	int GetExecuteTime() { return m_pSkillCore->GetExecuteTime(); }
	int GetType() { return m_pSkillCore->GetType(); }
	int GetRangeType() { return m_pSkillCore->GetRangeType(); }
	float GetCastRange(float fAtkDist, float fPrayDistancePlus) { return m_pSkillCore->GetPrayRange(fAtkDist, fPrayDistancePlus); }
	const char* GetEffect() { return m_pSkillCore->GetEffect(); }
	int GetTargetType() { return m_pSkillCore->GetTargetType(); }
	int GetCastEnv() { return m_pSkillCore->GetCastEnv(); }
	int* GetRequiredGenius() { return m_pSkillCore->GetRequiredGenius(m_idSkill); }
	int GetShowOrder() { return m_pSkillCore->GetShowOrder(); }
	bool IsChargeable() { return m_pSkillCore->IsWarmup(); }
	const char* GetNativeName() { return m_pSkillCore->GetNativeName(); }
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

	//��ȡ������ȴmask�� ��������bit0-4Ϊ������ȴ��bit5-9Ϊ��Ʒ��ȴ
	int GetCommonCoolDown() { return m_pSkillCore->GetCommonCoolDown(); }

	//��ȡ������ȴʱ�䣬��λ����
	int GetCommonCoolDownTime() { return m_pSkillCore->GetCommonCoolDownTime(); }

	//	Get skill description
	static bool GetDesc(int idSkill, int iLevel, ACHAR* szText, int iBufLen);

	//  Check skill type
	bool IsGoblinSkill() const;
	bool IsPlayerSkill() const;
	bool IsGeneralSkill()const;
	bool IsPositiveSkill();

	int  GetCls() const { return m_pSkillCore->GetCls(); }

	//  ����ȼ�
	int GetRank() { return m_pSkillCore->GetRank(); }

	//  �������ȼ�
	int GetMaxLevel() { return m_pSkillCore->GetMaxLevel(); }

	int GetComboSkPreSkill() { return m_pSkillCore->GetComboSkPreSkill(); }

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



