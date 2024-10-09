/*
 * FILE: EC_IvtrGoblin.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Sunxuewei, 2008/11/04
 *
 */

#pragma once

#include "EC_IvtrEquip.h"
#include "EC_IvtrTypes.h"

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////
#define MAX_ELF_REFINE_LEVEL 36

///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////

struct GOBLIN_EQUIP_TYPE;
struct GOBLIN_ESSENCE;
struct GOBLIN_EQUIP_ESSENCE;
struct GOBLIN_EXPPILL_ESSENCE;

class CECSkill;
///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////
struct GOBLINSKILL
{
	unsigned short skill;
	short level;
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECIvtrGoblin
//	
///////////////////////////////////////////////////////////////////////////

class CECIvtrGoblin : public CECIvtrEquip
{
public:		//  Static data & operations

	static int m_iRefineEffect[MAX_ELF_REFINE_LEVEL][3];
	static int m_iRefineMaterial[MAX_ELF_REFINE_LEVEL];
	static int elf_exp_loss_constant[151];

	//不使用rmb材料目标精炼等级对应的成功率
	static float elf_refine_succ_prob_ticket0[MAX_ELF_REFINE_LEVEL+1];
	//使用玲珑丹(提高成功率，失败等级归0)目标精炼等级对应的成功率
	static float elf_refine_succ_prob_ticket1[MAX_ELF_REFINE_LEVEL+1];
	//使用神韵丹(降成功率，失败等级降1)目标精炼等级对应的成功率
	static float elf_refine_succ_prob_ticket2[MAX_ELF_REFINE_LEVEL+1];
	//使用梦幻丹(成功率由使用个数决定，有个数上限，失败等级不败)目标精炼等级对应的最大使用量，成功率=道具数量/可以放的最大数量
	static int elf_refine_max_use_ticket3[MAX_ELF_REFINE_LEVEL+1];

	static float GetRefineSuccProb0(int iLevel);
	static float GetRefineSuccProb1(int iLevel);
	static float GetRefineSuccProb2(int iLevel);
	static float GetRefineSuccProb3(int iLevel, int iNum);

	// Get increased life
	static int GetRefineLife(int iLevel);
	// Get attack level
	static int GetRefineAtkLvl(int iLevel);
	// Get defence level
	static int GetRefineDfsLvl(int iLevel);
	// Get refine material count
	static int GetRefineMaterial( int iLevel);

	
	enum
	{
		MAX_SKILLNUM = 8,		// Max skill num
//		SKILL_LEARN_PT = 40,	// 灵力每提高SKILL_LEARN_PT点（本体+装备+模板），能多学一个技能 (已过时 2009-08-24)
		INIT_SKILL_NUM = 4,		// 初始技能数量
	};

public:		//	Types

public:		//	Constructor and Destructor

	CECIvtrGoblin(int tid, int expire_date);
	CECIvtrGoblin(const CECIvtrGoblin& s);
	virtual ~CECIvtrGoblin();

public:		//	Attributes
public:		//	Operations

	//	Set item detail information
	virtual bool SetItemInfo(BYTE* pInfoData, int iDataLen);
	//	Get item icon file name
	virtual const char* GetIconFile();
	//	Get item name
	virtual const wchar_t* GetName();
	//	Get drop model for shown
	virtual const char * GetDropModel();
	
	//	Clone item
	virtual CECIvtrItem* Clone() { return new CECIvtrGoblin(*this); }

	//  Get the experience when destroy the goblin
	unsigned int GetDestroyExp();
	
	//  Check whether goblin can trade
	bool IsTradeable() const;

	//	Check whether goblin can learn specified skill
	int CheckSkillLearnCondition(int idSkill, bool bCheckBook);
	//	Check whether goblin can cast specified skill
	bool CheckSkillCastCondition(int index);

	//	Get essence data
	const IVTR_ESSENCE_GOBLIN& GetEssence() { return m_Essence; }

	//	Get database data
	const GOBLIN_ESSENCE* GetDBEssence()	{ return m_pDBEssence; }

	//  Get skill number
	int GetSkillNum()						{ return m_aSkills.GetSize(); }

	//  Get current max skill number
	int GetCurrMaxSkillNum();

	GOBLINSKILL GetSkill(int id)			{ ASSERT(id>=0 && id<m_aSkills.GetSize());  return m_aSkills[id]; }
	unsigned int GetEquip(int id);

	//  Set Goblin inventory stamina
	void SetStamina(int iStamina)			{ m_Essence.data.stamina = iStamina; }
	
	//  Set Goblin Exp
	void SetExp(int iExp)					{ m_Essence.data.exp = iExp; }
	
	//  Get grow degree
	int GetGrowDegree();
	//  Current random status point
	int GetRandomStatusPt();
	//  Currently max random status point that can get
	int GetMaxRandomStatusPt();
	
	//  Max random status points that this goblin can get, depending on player's RP value
	int GetMaxStautsPt();
	
	//  Model level of goblin, return 1~4
	int GetModelLevel();

	//  Get basic property
	int GetBasicProp(int iIndex);
	//  Get genius point
	int GetGenius(int iIndex);
protected:	//	Attributes
	AArray<GOBLINSKILL, GOBLINSKILL&>	m_aSkills;
	AArray<unsigned int, unsigned int&>	m_aEquipID;

	IVTR_ESSENCE_GOBLIN	m_Essence;	//	Goblin essence data

	//	Data in database
	GOBLIN_ESSENCE*		m_pDBEssence;

protected:	//	Operations

	//	Get item description text
	virtual const wchar_t* GetNormalDesc(bool bRepair);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECIvtrGoblinEquip
//	
///////////////////////////////////////////////////////////////////////////

class CECIvtrGoblinEquip : public CECIvtrItem
{
public:		//	Types

public:		//	Constructor and Destructor

	CECIvtrGoblinEquip(int tid, int expire_date);
	CECIvtrGoblinEquip(const CECIvtrGoblinEquip& s);
	virtual ~CECIvtrGoblinEquip();

public:		//	Attributes
public:		//	Operations

	//	Set item detail information
	virtual bool SetItemInfo(BYTE* pInfoData, int iDataLen);
	//	Get item icon file name
	virtual const char* GetIconFile();
	//	Get item name
	virtual const wchar_t* GetName();
	//	Get drop model for shown
	virtual const char * GetDropModel();
	//	Clone item
	virtual CECIvtrItem* Clone() { return new CECIvtrGoblinEquip(*this); }
	//  Get goblin equipment type
	int GetGoblinEquipType() const;

	//	Can this item be equipped to specified position ?
	bool CanEquippedTo(int iSlot) const;

	//	Get database data
	const GOBLIN_EQUIP_ESSENCE* GetDBEssence() { return m_pDBEssence; }
protected:	//	Attributes

//	IVTR_ESSENCE_GOBLIN	m_Essence;	//	Goblin essence data

	//	Data in database
	GOBLIN_EQUIP_ESSENCE*		m_pDBEssence;

protected:	//	Operations

	//	Get item description text
	virtual const wchar_t* GetNormalDesc(bool bRepair);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECIvtrGoblinEquip
//	
///////////////////////////////////////////////////////////////////////////

class CECIvtrGoblinExpPill : public CECIvtrItem
{
public:		//	Types

public:		//	Constructor and Destructor

	CECIvtrGoblinExpPill(int tid, int expire_date);
	CECIvtrGoblinExpPill(const CECIvtrGoblinExpPill& s);
	virtual ~CECIvtrGoblinExpPill();

public:		//	Attributes
public:		//	Operations

	//	Set item detail information
	virtual bool SetItemInfo(BYTE* pInfoData, int iDataLen);
	//	Get item icon file name
	virtual const char* GetIconFile();
	//	Get item name
	virtual const wchar_t* GetName();
	//	Get drop model for shown
	virtual const char * GetDropModel();
	//	Clone item
	virtual CECIvtrItem* Clone() { return new CECIvtrGoblinExpPill(*this); }
	//  Get experience
	const unsigned int GetExp()		{ return m_iExp; }
	//  Get level
	const int GetLevel()	{ return m_iLevel; }

	//	Get database data
	const GOBLIN_EXPPILL_ESSENCE* GetDBEssence() { return m_pDBEssence; }

	//	Whether a rare item
	virtual bool IsRare() const;

protected:	//	Attributes

	// Experiences in pill
	unsigned int m_iExp;
	// Level of exp pill
	int  m_iLevel;

	//	Data in database
	GOBLIN_EXPPILL_ESSENCE*		m_pDBEssence;

protected:	//	Operations

	//	Get item description text
	virtual const wchar_t* GetNormalDesc(bool bRepair);
};