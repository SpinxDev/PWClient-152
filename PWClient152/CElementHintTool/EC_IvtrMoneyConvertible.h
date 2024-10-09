/*
 * FILE: EC_IvtrMoneyConvertible.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Shizhenhua, 2012/11/29
 *
 * HISTORY: 
 *
 * Copyright (c) 2011 ACE Studio, All Rights Reserved.
 */


#pragma once

#include "EC_IvtrItem.h"
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

struct MONEY_CONVERTIBLE_ESSENCE;


///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CECIvtrMoneyConvertible
//	
///////////////////////////////////////////////////////////////////////////

class CECIvtrMoneyConvertible : public CECIvtrItem
{
public:		//	Types
	
public:		//	Constructor and Destructor
	
	CECIvtrMoneyConvertible(int tid, int expire_date);
	CECIvtrMoneyConvertible(const CECIvtrMoneyConvertible& s);
	virtual ~CECIvtrMoneyConvertible();

public:		//	Attributes

public:		//	Operations

	//	Set item detail information
	virtual bool SetItemInfo(BYTE* pInfoData, int iDataLen);
	//	Get item icon file name
	virtual const char* GetIconFile();
	//	Get item name
	virtual const wchar_t* GetName();
	//	Clone item
	virtual CECIvtrItem* Clone() { return new CECIvtrMoneyConvertible(*this); }

	const MONEY_CONVERTIBLE_ESSENCE* GetDBEssence() { return m_pDBEssence; }

protected:	//	Attributes

	//	Data in database
	MONEY_CONVERTIBLE_ESSENCE*	m_pDBEssence;

protected:	//	Operations

	//	Get item description text
	virtual const wchar_t* GetNormalDesc(bool bRepair);
};

///////////////////////////////////////////////////////////////////////////
//  
//  Inline Functions
//  
///////////////////////////////////////////////////////////////////////////
