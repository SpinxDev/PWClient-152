/*
 * FILE: EC_IvtrArrow.h
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

struct PROJECTILE_TYPE;
struct PROJECTILE_ESSENCE;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CECIvtrArrow
//	
///////////////////////////////////////////////////////////////////////////

class CECIvtrArrow : public CECIvtrEquip
{
public:		//	Types

public:		//	Constructor and Destructor

	CECIvtrArrow(int tid, int expire_date);
	CECIvtrArrow(const CECIvtrArrow& s);
	virtual ~CECIvtrArrow();

public:		//	Attributes

public:		//	Operations

	//	Set item detail information
	virtual bool SetItemInfo(BYTE* pInfoData, int iDataLen);
	//	Get item icon file name
	virtual const char* GetIconFile();
	//	Get item name
	virtual const wchar_t* GetName();
	//	Clone item
	virtual CECIvtrItem* Clone() { return new CECIvtrArrow(*this); }

	//	Get raw repair cost
	virtual float GetRawRepairCost() { return 0.0f; }
	//	Get repair cost
	virtual int GetRepairCost() { return 0; }
	//	Get drop model for shown
	virtual const char * GetDropModel();

	//	Get essence data
	const IVTR_ESSENCE_ARROW& GetEssence() { return m_Essence; }

	//	Get database data
	const PROJECTILE_TYPE* GetDBSubType() { return m_pDBType; }
	const PROJECTILE_ESSENCE* GetDBEssence() { return m_pDBEssence; }

protected:	//	Attributes

	IVTR_ESSENCE_ARROW	m_Essence;	//	Arrow essence data

	//	Data in database
	PROJECTILE_TYPE*	m_pDBType;
	PROJECTILE_ESSENCE*	m_pDBEssence;

protected:	//	Operations

	//	Get item description text
	virtual const wchar_t* GetNormalDesc(bool bRepair);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

