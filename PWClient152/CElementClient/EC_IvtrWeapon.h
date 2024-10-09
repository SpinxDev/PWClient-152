/*
 * FILE: EC_IvtrWeapon.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/11/19
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include "EC_IvtrEquip.h"
#include "EC_IvtrTypes.h"

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

struct WEAPON_MAJOR_TYPE;
struct WEAPON_SUB_TYPE;
struct WEAPON_ESSENCE;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CECIvtrWeapon
//	
///////////////////////////////////////////////////////////////////////////

class CECIvtrWeapon : public CECIvtrEquip
{
public:		//	Types

public:		//	Constructor and Destructor

	CECIvtrWeapon(int tid, int expire_date);
	CECIvtrWeapon(const CECIvtrWeapon& s);
	virtual ~CECIvtrWeapon();

public:		//	Attributes

public:		//	Operations

	//	Set item detail information
	virtual bool SetItemInfo(BYTE* pInfoData, int iDataLen);
	//	Get item default information from database
	virtual void DefaultInfo();
	//	Get item icon file name
	virtual const char* GetIconFile();
	//	Get item name
	virtual const wchar_t* GetName();
	//	Clone item
	virtual CECIvtrItem* Clone() { return new CECIvtrWeapon(*this); }
	//	Get drop model for shown
	virtual const char * GetDropModel();
	//	Whether a rare item
	virtual bool IsRare() const;

	//	Does this equipment has random property ?
	virtual bool HasRandomProp();
	//	Get number of material needed to refine equipment
	virtual int GetRefineMaterialNum();
	virtual int GetRefineAddOn();

	virtual int GetEquipmentType() const { return EQUIP_WEAPON; }

	virtual int GetItemLevel()const;

	//	The weapon is range weapon ?
	bool IsRangeWeapon() { return m_Essence.weapon_type == WEAPONTYPE_RANGE; }

	//	Get essence data
	const IVTR_ESSENCE_WEAPON& GetEssence() { return m_Essence; }

	//	Get database data
	const WEAPON_MAJOR_TYPE* GetDBMajorType() { return m_pDBMajorType; }
	const WEAPON_SUB_TYPE* GetDBSubType() { return m_pDBSubType; }
	const WEAPON_ESSENCE* GetDBEssence() { return m_pDBEssence; }

	//	Get preview info text
	virtual const wchar_t* GetPreviewInfo();

protected:	//	Attributes

	IVTR_ESSENCE_WEAPON	m_Essence;	//	Weapon essence data

	//	Data in database
	WEAPON_MAJOR_TYPE*	m_pDBMajorType;
	WEAPON_SUB_TYPE*	m_pDBSubType;
	WEAPON_ESSENCE*		m_pDBEssence;

protected:	//	Operations

	//	Get item description text
	virtual const wchar_t* GetNormalDesc(bool bRepair);

	//	Get weapon attack speed index
	int GetAttackSpeedIndex();

	virtual bool GetRefineEffectFor(ACString & strEffect, const RefineEffect &rhs);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

