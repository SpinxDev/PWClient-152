/*
 * FILE: EC_IvtrArmor.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2005/1/3
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
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

struct ARMOR_MAJOR_TYPE;
struct ARMOR_SUB_TYPE;
struct ARMOR_ESSENCE;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CECIvtrArmor
//	
///////////////////////////////////////////////////////////////////////////

class CECIvtrArmor : public CECIvtrEquip
{
public:		//	Types

public:		//	Constructor and Destructor

	CECIvtrArmor(int tid, int expire_date);
	CECIvtrArmor(const CECIvtrArmor& s);
	virtual ~CECIvtrArmor();

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
	virtual CECIvtrItem* Clone() { return new CECIvtrArmor(*this); }

	//	Does this equipment has random property ?
	virtual bool HasRandomProp();
	//	Get number of material needed to refine equipment
	virtual int GetRefineMaterialNum();
	virtual int GetRefineAddOn();
	//	Get drop model for shown
	virtual const char * GetDropModel();
	//	Whether a rare item
	virtual bool IsRare() const;

	virtual int GetEquipmentType() const { return EQUIP_ARMOR; }

	virtual int GetItemLevel() const;

	//	Get essence data
	const IVTR_ESSENCE_ARMOR& GetEssence() { return m_Essence; }

	//	Get database data
	const ARMOR_MAJOR_TYPE* GetDBMajorType() { return m_pDBMajorType; }
	const ARMOR_SUB_TYPE* GetDBSubType() { return m_pDBSubType; }
	const ARMOR_ESSENCE* GetDBEssence() { return m_pDBEssence; }
	
	//	Get preview info text
	virtual const wchar_t* GetPreviewInfo();

protected:	//	Attributes

	IVTR_ESSENCE_ARMOR	m_Essence;	//	Weapon essence data

	//	Data in database
	ARMOR_MAJOR_TYPE*	m_pDBMajorType;
	ARMOR_SUB_TYPE*		m_pDBSubType;
	ARMOR_ESSENCE*		m_pDBEssence;

protected:	//	Operations

	//	Get item description text
	virtual const wchar_t* GetNormalDesc(bool bRepair);
	
	virtual bool GetRefineEffectFor(ACString & strEffect, const RefineEffect &rhs);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

