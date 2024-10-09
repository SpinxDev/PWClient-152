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
		HUNGER_LEVEL_0,			//	饱食
		HUNGER_LEVEL_1,			//	正常
		HUNGER_LEVEL_2,			//	饿程度一级 
		HUNGER_LEVEL_3,		
		HUNGER_LEVEL_4,			//	饿程度二级
		HUNGER_LEVEL_5,		
		HUNGER_LEVEL_6,		
		HUNGER_LEVEL_7,			//	饿程度三级
		HUNGER_LEVEL_8,		
		HUNGER_LEVEL_9,		
		HUNGER_LEVEL_10,		
		HUNGER_LEVEL_11,		//	饿程度四级
		HUNGER_LEVEL_COUNT,
	};

	//	Intimacy level
	enum
	{
		INTIMACY_LEVEL_0,		//	野性难驯, 0-50
		INTIMACY_LEVEL_1,		//	反复无偿, 51-150
		INTIMACY_LEVEL_2,		//	乖巧听话, 151-500
		INTIMACY_LEVEL_3,		//	忠心耿耿, 501-999
		INTIMACY_LEVEL_COUNT,
		INTIMACY_POINT_MAX = 999,
	};

// 	enum
// 	{
// 		MAX_SKILLNUM = 4,	// 不包括性格技能
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
		EM_SKILL_DEFAULT = 0, // 不区分性格技能
		EM_SKILL_NORMAL,	// 普通技能
		EM_SKILL_NATURE,	// 性格技能
		EM_SKILL_SPECIAL,	// 专属技能
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

	//	获取用于显示的属性
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

	int			m_iIntimacy; 	//	好感度
	int			m_iHunger;		//	饥饿度
	int			m_tid;       	//	宠物的模板ID
	int			m_tidVis;     	//	宠物形象的模板ID（如果为0，则表示无特殊可见ID）
	int			m_idEgg;     	//	宠物蛋的ID
	int			m_iClass;    	//	宠物类型 战宠，骑宠，观赏宠
	float		m_fHPFactor; 	//	血量比例（复活和收回时使用） 0则为死亡
	float		m_fMPFactor;
	int			m_iLevel;    	//	宠物级别
	bool		m_isBind;	//	是否天人合一
	bool		m_canWebTrade;	//	是否寻宝网可交易
	unsigned short m_color;	//	骑宠染色颜色，高位为1时有效
	int			m_iExp;      	//	宠物当前经验
	int			m_iSkillPt;  	//	剩余技能点
	ACString	m_strName;		

	int			m_iHP;			//	Only fight pets have this
	int			m_iMP;
	PETSKILL	m_aSkills[GP_PET_SKILL_NUM];
	
	abase::vector<int> m_vecNorSkillIndex; // 防止性格技能和普通技能顺序混乱的情况，保存技能在 m_aSkills里的序号
	abase::vector<int> m_vecDynSkillIndex;
	int			m_iSpecialSkillIndex; // 保存专属技能在m_aSkills里的序号，只有进化宠有一个此技能
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

