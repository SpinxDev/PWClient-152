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
	int	 GetDefRation() const	{ return	m_iDefRation;}
	int	 GetHpRation() const	{ return	m_iHpRation;}
	int	 GetAtkLvlRation() const { return	m_iAtkLvlRation;}
	int	 GetDefLvlRation() const { return	m_iDefLvlRation;}
	int	 GetNatureID() const		{ return	m_iNature;}
	ACString GetNature();

	//  ����Զ��ͷż���
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

//	ֲ�������
//
struct CECPlantPetData
{
	CECPlantPetData();
	
	void Init(const S2C::cmd_summon_plant_pet &rhs);
	void Info(const S2C::cmd_plant_pet_hp_notify &rhs);
	void Tick(DWORD dwDeltaTime);

	int	GetLifeTime()const {return m_lifeTime * 1000;}		//	�ܴ��ʱ�䣨���룩
	int GetLifeTimeLeft()const{ return m_lifeTimeLeft; }	//	ʣ����ʱ�䣨���룩

	int	m_tid;			//	�����ģ��ID
	int	m_nid;			//	���������ID
	int m_lifeTime;		//	���ʱ�䣨��������0Ϊ����
	int	m_lifeTimeLeft;	//	���ʱ�䵹��ʱ����������
	float m_HPFactor;	//	��ǰ HP ռ�� HP �ı���
	int	m_HP;			//	��ǰ HP
	float m_MPFactor;	//	��ǰ MP ռ�� MP �ı���
	int m_MP;			//	��ǰ MP
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
		MAX_SLOTNUM	= 20,	//	��ͨ����������
		MAX_SLOTNUM2 = 21,	//	���г���������
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

	//	��ѯ�����õ�ǰ���������ʱ��
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

	bool CheckRebuildPetItemCond(int iPetIndex,int iSel, int type); // ����������ѡ�����Ʒ��ţ�type��0(����),1(�Ը���),2(ϴ��ϵ��)

protected:	//	Attributes
	bool		m_bHasInit;					//	Flag indicates whether we have init the data by calling MagnifyPetSlots

	int			m_iActivePet;				//	Index of current active pet
	int			m_iPetSlotNum;				//	Number of current active pet slots
	CECPetData*	m_aPetSlots[MAX_SLOTNUM2];	//	Pet slots

	int			m_nidPet;			//	ID of Pet as a NPC in world
	int			m_iMoveMode;		//	Current moving mode of pet
	int			m_iAttackMode;		//	Current attacking mode of pet

	int			m_iPetLifeTime;		//	��ǰ����Ĵ���ڣ�0��ʾ���ã�
	
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

