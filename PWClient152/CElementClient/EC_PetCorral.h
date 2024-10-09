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
#include "EC_RoleTypes.h"
#include "EC_GPDataType.h"
#include "EC_Counter.h"

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
	struct cmd_pet_room;
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

	//	Set skill cool time
	void SetSkillCoolTime(int iCoolIdx, int iTime);

	//	Get skill cool time
	int GetSkillCoolTime(CECPetData::SKILLTYPE iType,int iSkillIdx, int* piMax=NULL);

	//	Check whether pet is dead
	bool IsDead() { return m_fHPFactor ? false : true; }

	//	Calculate max hp of pet
	int CalcMaxHP();
	
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
	int	 GetDefRation() const	{ return	m_iDefRation;}
	int	 GetHpRation() const	{ return	m_iHpRation;}
	int	 GetAtkLvlRation() const { return	m_iAtkLvlRation;}
	int	 GetDefLvlRation() const { return	m_iDefLvlRation;}
	int	 GetNatureID() const		{ return	m_iNature;}
	ACString GetNature();

	//  添加自动释放技能
	void AddAutoSkill(int skill_id);
	void CastAutoSkill();
	void OnAutoCastOver(int skill_id);
	void OnPetDead();

	bool CanEvolution()const ;
	int GetEvolutionID() const ;
	static int GetEvolutionID(int pet_tid);

	PET_ESSENCE* GetPetEssence() { return m_pDBEssence;}

	int GetMaxExp();

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

	CECCounter	m_cntAutoSkill;
	abase::vector<int> m_aAutoSkills;

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

//	植物宠数据
//
struct CECPlantPetData
{
	CECPlantPetData();
	
	void Init(const S2C::cmd_summon_plant_pet &rhs);
	void Info(const S2C::cmd_plant_pet_hp_notify &rhs);
	void Tick(DWORD dwDeltaTime);

	int	GetLifeTime()const {return m_lifeTime * 1000;}		//	总存活时间（毫秒）
	int GetLifeTimeLeft()const{ return m_lifeTimeLeft; }	//	剩余存活时间（毫秒）

	int	m_tid;			//	宠物的模板ID
	int	m_nid;			//	宠物的世界ID
	int m_lifeTime;		//	存活时间（秒数），0为永久
	int	m_lifeTimeLeft;	//	存活时间倒计时（毫秒数）
	float m_HPFactor;	//	当前 HP 占总 HP 的比例
	int	m_HP;			//	当前 HP
	float m_MPFactor;	//	当前 MP 占总 MP 的比例
	int m_MP;			//	当前 MP
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECPetCorral
//	
///////////////////////////////////////////////////////////////////////////

class CECPetCorral
{
public:		//	Types

	enum
	{
		MAX_SLOTNUM	= 20,	//	普通宠物栏个数
		MAX_SLOTNUM2 = 21,	//	所有宠物栏个数
	};

	//	Moving mode
	enum
	{
		MOVE_FOLLOW = 0,
		MOVE_STAND,
	};

	//	Attacking mode
	enum
	{
		ATK_DEFENSE = 0,
		ATK_POSITIVE,
		ATK_PASSIVE,
	};

public:		//	Constructor and Destructor

	CECPetCorral();
	virtual ~CECPetCorral();

public:		//	Attributes

public:		//	Operations

	//	Tick routine
	bool Tick(DWORD dwDeltaTime);

	//	Add a pet
	bool AddPet(int iSlot, const S2C::info_pet& Info);
	//	Free a pet
	void FreePet(int iSlot, int idPet);
	//	Remove all pets
	void RemoveAll();
	//	Update pets data in corral
	void UpdatePets(const S2C::cmd_pet_room& cmd);

	//	Magnify pet slots
	void MagnifyPetSlots(int iNewNum);
	//	Check whether corral has empty slots
	int GetEmptySlotNum();
	//	Get number of current active pet slots
	int GetPetSlotNum() { return m_iPetSlotNum; }
	//	Get pet data of specified slot
	CECPetData* GetPetData(int iSlot)
	{
		if (iSlot < 0 || iSlot >= MAX_SLOTNUM2)
		{
			ASSERT(iSlot >= 0 && iSlot < MAX_SLOTNUM2);
			return NULL;
		}

		return m_aPetSlots[iSlot];
	}

	//	Get current active pet's index
	int GetActivePetIndex() { return m_iActivePet; }
	//	Set current active pet's index
	void SetActivePetIndex(int iIndex) { m_iActivePet = iIndex; }
	//	Get current active pet's data
	CECPetData* GetActivePet()
	{
		if (m_iActivePet >= 0 && m_iActivePet < MAX_SLOTNUM2)
			return m_aPetSlots[m_iActivePet];
		else
			return NULL;
	}

	//	Get / Set ID of active pet as a NPC in world
	int GetActivePetNPCID() { return m_nidPet; }
	void SetActivePetNPCID(int nid) { m_nidPet = nid; }

	//	查询、设置当前宠物的生存时长
	int GetActivePetLifeTime(){ return m_iPetLifeTime; }
	void SetActivePetLifetime(int lifetime) { m_iPetLifeTime = lifetime; }

	//	Get / Set moving mode
	int GetMoveMode() { return m_iMoveMode; }
	void SetMoveMode(int iMode) { m_iMoveMode = iMode; }
	//	Get / Set attacking mode
	int GetAttackMode() { return m_iAttackMode; }
	void SetAttackMode(int iMode) { m_iAttackMode = iMode; }
	//	Get / Set init flag
	bool HasInit() { return m_bHasInit; }
	void SetHasInit(bool bFlag) { m_bHasInit = bFlag; }

	void PlantPetEnter(const S2C::cmd_summon_plant_pet &rhs);
	void PlantPetDisappear(const S2C::cmd_plant_pet_disapper &rhs);
	void PlantPetInfo(const S2C::cmd_plant_pet_hp_notify &rhs);

	int  GetPlantCount()const;
	const CECPlantPetData * GetPlant(int index)const;
	int  GetPlantIndexByID(int nid)const;
	const CECPlantPetData * GetPlantByID(int nid)const;

	PET_EVOLVE_CONFIG* GetPetEvoConfig() { return m_pDBEvoConfig;}

	bool CheckRebuildPetItemCond(int iPetIndex,int iSel, int type); // 宠物索引，选择的物品序号，type：0(进化),1(性格技能),2(洗髓系数)

protected:	//	Attributes
	bool		m_bHasInit;					//	Flag indicates whether we have init the data by calling MagnifyPetSlots

	int			m_iActivePet;				//	Index of current active pet
	int			m_iPetSlotNum;				//	Number of current active pet slots
	CECPetData*	m_aPetSlots[MAX_SLOTNUM2];	//	Pet slots

	int			m_nidPet;			//	ID of Pet as a NPC in world
	int			m_iMoveMode;		//	Current moving mode of pet
	int			m_iAttackMode;		//	Current attacking mode of pet

	int			m_iPetLifeTime;		//	当前宠物的存活期（0表示永久）
	
	typedef abase::vector<CECPlantPetData> PlantVec;
	PlantVec	m_Plants;

	PET_EVOLVE_CONFIG* m_pDBEvoConfig;
		
protected:	//	Operations
	CECPlantPetData * GetPlant(int index);
	CECPlantPetData * GetPlantByID(int nid);
	
	const CECPlantPetData * GetPlantImpl(int index)const;
	const CECPlantPetData * GetPlantByIDImpl(int nid)const;

};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

