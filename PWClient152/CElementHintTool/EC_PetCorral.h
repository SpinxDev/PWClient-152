/*
 * FILE: EC_PetCorral.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2005/12/23
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include "AAssist.h"
#include "../CElementClient/EC_RoleTypes.h"
#include "EC_GPDataType.h"
#include <vector.h>

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

namespace S2C
{
	struct info_pet;
}

struct PET_ESSENCE;
struct PET_EVOLVE_CONFIG;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CECPetData
//	
///////////////////////////////////////////////////////////////////////////

class CECPetData
{
public:		//	Types

	//	Hunger level
	enum
	{
		HUNGER_LEVEL_0,			//	��ʳ
		HUNGER_LEVEL_1,			//	����
		HUNGER_LEVEL_2,			//	���̶�һ�� 
		HUNGER_LEVEL_3,		
		HUNGER_LEVEL_4,			//	���̶ȶ���
		HUNGER_LEVEL_5,		
		HUNGER_LEVEL_6,		
		HUNGER_LEVEL_7,			//	���̶�����
		HUNGER_LEVEL_8,		
		HUNGER_LEVEL_9,		
		HUNGER_LEVEL_10,		
		HUNGER_LEVEL_11,		//	���̶��ļ�
		HUNGER_LEVEL_COUNT,
	};

	//	Intimacy level
	enum
	{
		INTIMACY_LEVEL_0,		//	Ұ����ѱ, 0-50
		INTIMACY_LEVEL_1,		//	�����޳�, 51-150
		INTIMACY_LEVEL_2,		//	��������, 151-500
		INTIMACY_LEVEL_3,		//	���Ĺ���, 501-999
		INTIMACY_LEVEL_COUNT,
		INTIMACY_POINT_MAX = 999,
	};

// 	enum
// 	{
// 		MAX_SKILLNUM = 4,	// �������Ը���
// 	};

	struct PETSKILL
	{
		int idSkill;
		int iLevel;
		int	iCoolCnt;
		int	iCoolMax;
	};

	enum SKILLTYPE
	{
		EM_SKILL_DEFAULT = 0, // �������Ը���
		EM_SKILL_NORMAL,	// ��ͨ����
		EM_SKILL_NATURE,	// �Ը���
		EM_SKILL_SPECIAL,	// ר������
	};

	friend class CECPetCorral;

public:		//	Constructor and Destructor

	CECPetData();
	virtual ~CECPetData();

public:		//	Attributes

public:		//	Operations

	//	Initialize object
	bool Init(const S2C::info_pet& Info);

	//	Add experience
	int AddExp(int iExp);
	//	Level up
	int LevelUp(int iLevel, int iNewExp);

	//	Get properties
	int GetIntimacy() const { return m_iIntimacy; }
	void SetIntimacy(int iValue) { m_iIntimacy = iValue; }
	int GetHunger() const { return m_iHunger; }
	void SetHunger(int iValue) { m_iHunger = iValue; }
	int	GetTemplateID() const { return m_tid; }
	int GetVisibleID() const { return m_tidVis; }
	int GetEggID() const { return m_idEgg; }
	int GetClass() const { return m_iClass; }
	int GetLevel() const { return m_iLevel; }
	unsigned short GetColor() const { return m_color; }
	bool IsBind()const { return m_isBind; }
	bool CanWebTrade()const{ return m_canWebTrade; }
	int GetExp() const { return m_iExp; }
	int GetSkillPt() const { return m_iSkillPt; }
	const ACHAR* GetName() const { return m_strName; }

	float GetHPFactor() const { return m_fHPFactor; }
	void SetHPFactor(float fFactor) { m_fHPFactor = fFactor; }

	void SetMPFactor(float fFactor) { m_fMPFactor = fFactor; }
	float GetMPFactor()const { return m_fMPFactor; }

	int GetHP() const { return m_iHP; }
	void SetHP(int iHP) { m_iHP = iHP; }

	int GetMP() const { return m_iMP; }
	void SetMP(int iMP) { m_iMP = iMP; }

	//	Check whether pet is dead
	bool IsDead() { return m_fHPFactor ? false : true; }
	
	//	Get skill by index
	const PETSKILL* GetSkill(SKILLTYPE iType,int n);	

	//	Get skill by id
	const PETSKILL* GetSkillByID(int id);
	//	Get valid skill number
	int GetSkillNum(SKILLTYPE iType);

	//	��ȡ������ʾ������
	const ROLEEXTPROP& GetExtendProps() const { return m_ExtProps; }
	void SetExtendProps(const ROLEEXTPROP& prop) { m_ExtProps = prop; }

	bool IsFollowPet()const;
	bool IsMountPet()const;
	bool IsCombatPet()const;
	bool IsSummonPet()const;
	bool IsPlantPet()const;
	bool IsEvolutionPet() const;

	int	 GetAtkRation() const	{return		m_iAtkRation;}
	int	 GetMaxAtkRation() const;
	int	 GetDefRation() const	{ return	m_iDefRation;}
	int	 GetMaxDefRation() const;
	int	 GetHpRation() const	{ return	m_iHpRation;}
	int	 GetMaxHpRation() const;
	int	 GetAtkLvlRation() const { return	m_iAtkLvlRation;}
	int	 GetMaxAtkLvlRation() const;
	int	 GetDefLvlRation() const { return	m_iDefLvlRation;}
	int	 GetMaxDefLvlRation() const;
	int	 GetNatureID() const		{ return	m_iNature;}
	AWString GetNature();

	const PET_ESSENCE *GetDBEssence()const{return m_pDBEssence;}

	AWString GetFoodStr()const;
	AWString GetMoveSpeedStr()const;
	AWString GetHungerStr()const;

	int	GetLoyalty()const;
	AWString GetLoyaltyStr()const;

	int GetLevelRequirement()const;
	int GetMaxHP()const;
	int GetAttack()const;
	int GetPhyicDefence()const;
	int GetMagicDefence()const;
	int GetDefiniton()const;
	int GetEvade()const;

	AWString GetAttackSpeedStr()const;
	AWString GetInhabitTypeStr()const;

protected:	//	Attributes

	int			m_iIntimacy; 	//	�øж�
	int			m_iHunger;		//	������
	int			m_tid;       	//	�����ģ��ID
	int			m_tidVis;     	//	���������ģ��ID�����Ϊ0�����ʾ������ɼ�ID��
	int			m_idEgg;     	//	���ﵰ��ID
	int			m_iClass;    	//	�������� ս�裬��裬���ͳ�
	float		m_fHPFactor; 	//	Ѫ��������������ջ�ʱʹ�ã� 0��Ϊ����
	float		m_fMPFactor;
	int			m_iLevel;    	//	���Ｖ��
	bool		m_isBind;	//	�Ƿ����˺�һ
	bool		m_canWebTrade;	//	�Ƿ�Ѱ�����ɽ���
	unsigned short m_color;	//	���Ⱦɫ��ɫ����λΪ1ʱ��Ч
	int			m_iExp;      	//	���ﵱǰ����
	int			m_iSkillPt;  	//	ʣ�༼�ܵ�
	ACString	m_strName;		

	int			m_iHP;			//	Only fight pets have this
	int			m_iMP;
	PETSKILL	m_aSkills[GP_PET_SKILL_NUM];
	
	abase::vector<int> m_vecNorSkillIndex; // ��ֹ�Ը��ܺ���ͨ����˳����ҵ���������漼���� m_aSkills������
	abase::vector<int> m_vecDynSkillIndex;
	int			m_iSpecialSkillIndex; // ����ר��������m_aSkills�����ţ�ֻ�н�������һ���˼���
	ROLEEXTPROP m_ExtProps;
	PET_ESSENCE*	m_pDBEssence;

	int			m_iAtkRation;
	int			m_iDefRation;
	int			m_iHpRation;
	int			m_iAtkLvlRation;
	int			m_iDefLvlRation;
	int			m_iNature;
protected:	//	Operations

	//	Tick routine
	bool Tick(DWORD dwDeltaTime);
};
///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

