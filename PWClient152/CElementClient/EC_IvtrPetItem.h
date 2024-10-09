/*
 * FILE: EC_IvtrPetItem.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2005/12/27
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include "EC_IvtrItem.h"
#include "EC_IvtrTypes.h"
#include "AArray.h"
#include "AWString.h"
#include "EC_GPDataType.h"
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

struct PET_EGG_ESSENCE;
struct PET_FOOD_ESSENCE;
struct PET_FACETICKET_ESSENCE;
struct PET_ESSENCE;
struct PET_EVOLVED_SKILL_CONFIG;
///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CECIvtrPetEgg
//	
///////////////////////////////////////////////////////////////////////////

class CECIvtrPetEgg : public CECIvtrItem
{
public:		//	Types

	struct PETSKILL
	{
		int idSkill;
		int iLevel;
	};

public:		//	Constructor and Destructor

	CECIvtrPetEgg(int tid, int expire_date);
	CECIvtrPetEgg(const CECIvtrPetEgg& s);
	virtual ~CECIvtrPetEgg();

public:		//	Attributes

public:		//	Operations

	//	Set item detail information
	virtual bool SetItemInfo(BYTE* pInfoData, int iDataLen);
	//	Get item icon file name
	virtual const char* GetIconFile();
	//	Get item name
	virtual const wchar_t* GetName();
	//	Clone item
	virtual CECIvtrItem* Clone() { return new CECIvtrPetEgg(*this); }
	//	Get item description for booth buying
	virtual const wchar_t* GetBoothBuyDesc();
	//	Get drop model for shown
	virtual const char * GetDropModel();

	//	Get skill number
	int GetSkillNum() { return m_aSkills.GetSize(); }
	//	Get skill
	const PETSKILL& GetSkill(int n) { return m_aSkills[n]; }

	//	Get essence data
	const IVTR_ESSENCE_PETEGG& GetEssence() { return m_Essence; }
	//	Get database data
	const PET_EGG_ESSENCE* GetDBEssence() { return m_pDBEssence; }
	//	Get pet essence
	const PET_ESSENCE* GetPetEssence() { return m_pPetEssence; }

protected:	//	Operations

	AArray<PETSKILL, PETSKILL&>	m_aSkills;

	IVTR_ESSENCE_PETEGG		m_Essence;
	AWString				m_strName;

	//	Data in database
	PET_EGG_ESSENCE*		m_pDBEssence;
	PET_ESSENCE*			m_pPetEssence;
	PET_ESSENCE*			m_pEvoPetEssence;
	PET_EVOLVED_SKILL_CONFIG* m_pEvoNatureConfig;

	S2C::info_pet::_evo_prop	m_EvoProp; // 进化宠继承数据

protected:	//	Operations

	//	Get item description text
	virtual const wchar_t* GetNormalDesc(bool bRepair);
	
	//	Build riding pet description text
	const wchar_t* GetRidingPetDesc(bool bRepair);
	//	Build combat pet description text
	const wchar_t* GetCombatPetDesc(bool bRepair);
	//	Build follow pet description text
	const wchar_t* GetFollowPetDesc(bool bRepair);
	//	Build evolution pet description text
	const wchar_t* GetEvolutionPetDesc(bool bRepair);
	//	Add food type requirment description
	void AddFoodTypeDesc();
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECIvtrPetFood
//	
///////////////////////////////////////////////////////////////////////////

class CECIvtrPetFood : public CECIvtrItem
{
public:		//	Types

public:		//	Constructor and Destructor

	CECIvtrPetFood(int tid, int expire_date);
	CECIvtrPetFood(const CECIvtrPetFood& s);
	virtual ~CECIvtrPetFood();

public:		//	Attributes

public:		//	Operations

	//	Set item detail information
	virtual bool SetItemInfo(BYTE* pInfoData, int iDataLen);
	//	Get item icon file name
	virtual const char* GetIconFile();
	//	Get item name
	virtual const wchar_t* GetName();
	//	Clone item
	virtual CECIvtrItem* Clone() { return new CECIvtrPetFood(*this); }
	//	Get item cool time
	virtual int GetCoolTime(int* piMax=NULL);
	//	Check item use condition
	virtual bool CheckUseCondition();
	//	Get drop model for shown
	virtual const char * GetDropModel();

	//	Get database data
	const PET_FOOD_ESSENCE* GetDBEssence() { return m_pDBEssence; }

protected:	//	Operations

	//	Data in database
	PET_FOOD_ESSENCE*		m_pDBEssence;

protected:	//	Operations

	//	Get item description text
	virtual const wchar_t* GetNormalDesc(bool bRepair);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECIvtrPetFaceTicket
//	
///////////////////////////////////////////////////////////////////////////

class CECIvtrPetFaceTicket : public CECIvtrItem
{
public:		//	Types

public:		//	Constructor and Destructor

	CECIvtrPetFaceTicket(int tid, int expire_date);
	CECIvtrPetFaceTicket(const CECIvtrPetFaceTicket& s);
	virtual ~CECIvtrPetFaceTicket();

public:		//	Attributes

public:		//	Operations

	//	Set item detail information
	virtual bool SetItemInfo(BYTE* pInfoData, int iDataLen);
	//	Get item icon file name
	virtual const char* GetIconFile();
	//	Get item name
	virtual const wchar_t* GetName();
	//	Clone item
	virtual CECIvtrItem* Clone() { return new CECIvtrPetFaceTicket(*this); }
	//	Get drop model for shown
	virtual const char * GetDropModel();

	//	Get database data
	const PET_FACETICKET_ESSENCE* GetDBEssence() { return m_pDBEssence; }

protected:	//	Operations

	//	Data in database
	PET_FACETICKET_ESSENCE*	m_pDBEssence;

protected:	//	Operations

	//	Get item description text
	virtual const wchar_t* GetNormalDesc(bool bRepair);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

