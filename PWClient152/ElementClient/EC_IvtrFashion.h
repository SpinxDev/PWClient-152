/*
 * FILE: EC_IvtrFashion.h
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

#include <A3DTypes.h>

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

struct FASHION_MAJOR_TYPE;
struct FASHION_SUB_TYPE;
struct FASHION_ESSENCE;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CECIvtrFashion
//	
///////////////////////////////////////////////////////////////////////////

class CECIvtrFashion : public CECIvtrEquip
{
public:		//	Types

public:		//	Constructor and Destructor

	CECIvtrFashion(int tid, int expire_date);
	CECIvtrFashion(const CECIvtrFashion& s);
	virtual ~CECIvtrFashion();

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
	virtual CECIvtrItem* Clone() { return new CECIvtrFashion(*this); }
	//	Get drop model for shown
	virtual const char * GetDropModel();

	//	Does this equipment has random property ?
	virtual bool HasRandomProp();

	//	Get raw repair cost
	virtual float GetRawRepairCost() { return 0.0f; }
	//	Get repair cost
	virtual int GetRepairCost() { return 0; }

	virtual int GetEquipmentType() const { return EQUIP_FASHION; }
	
	//	Get essence data
	const IVTR_ESSENCE_FASHION& GetEssence() { return m_Essence; }

	//	Get database data
	const FASHION_MAJOR_TYPE* GetDBMajorType() { return m_pDBMajorType; }
	const FASHION_SUB_TYPE* GetDBSubType() { return m_pDBSubType; }
	const FASHION_ESSENCE* GetDBEssence() { return m_pDBEssence; }

	WORD GetWordColor() { return m_wColor; }
	A3DCOLOR GetColor() { return m_color; }
	int GetGenderRequirement() { return m_iGender; }
	int GetFashionSuiteID()const;

protected:	//	Attributes
	
	int						m_iGender;	//	required gender
	WORD					m_wColor;	//	color in X1R5G5B5 format
	A3DCOLOR				m_color;	//	color in X8R8G8B8 format

	IVTR_ESSENCE_FASHION	m_Essence;	//	Fashion essence data

	//	Data in database
	FASHION_MAJOR_TYPE*		m_pDBMajorType;
	FASHION_SUB_TYPE*		m_pDBSubType;
	FASHION_ESSENCE*		m_pDBEssence;

protected:	//	Operations

	//	Get item description text
	virtual const wchar_t* GetNormalDesc(bool bRepair);

	//	Build add-ons properties description
	void BuildAddOnPropDesc();
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

