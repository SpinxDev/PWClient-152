/*
 * FILE: EC_IvtrDecoration.h
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
#include "../CElementClient/EC_IvtrTypes.h"

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

struct DECORATION_MAJOR_TYPE;
struct DECORATION_SUB_TYPE;
struct DECORATION_ESSENCE;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CECIvtrDecoration
//	
///////////////////////////////////////////////////////////////////////////

class CECIvtrDecoration : public CECIvtrEquip
{
public:		//	Types

public:		//	Constructor and Destructor

	CECIvtrDecoration(int tid, int expire_date);
	CECIvtrDecoration(const CECIvtrDecoration& s);
	virtual ~CECIvtrDecoration();

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
	virtual CECIvtrItem* Clone() { return new CECIvtrDecoration(*this); }
	//	Get drop model for shown
	virtual const char * GetDropModel();

	//	Does this equipment has random property ?
	virtual bool HasRandomProp();
	virtual bool HasViewProp();
	//	Get number of material needed to refine equipment
	virtual int GetRefineMaterialNum();

	virtual int GetEquipmentType() const { return EQUIP_DECORATION; }

	virtual int GetItemLevel() const;

	//	Get essence data
	const IVTR_ESSENCE_DECORATION& GetEssence() { return m_Essence; }

	//	Get database data
	const DECORATION_MAJOR_TYPE* GetDBMajorType() { return m_pDBMajorType; }
	const DECORATION_SUB_TYPE* GetDBSubType() { return m_pDBSubType; }
	const DECORATION_ESSENCE* GetDBEssence() { return m_pDBEssence; }

protected:	//	Attributes

	IVTR_ESSENCE_DECORATION	m_Essence;	//	Weapon essence data

	//	Data in database
	DECORATION_MAJOR_TYPE*	m_pDBMajorType;
	DECORATION_SUB_TYPE*	m_pDBSubType;
	DECORATION_ESSENCE*		m_pDBEssence;

protected:	//	Operations

	//	Get item description text
	virtual const wchar_t* GetNormalDesc(bool bRepair);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////


