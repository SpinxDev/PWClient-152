/*
 * FILE: EC_IvtrTaskItem.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2005/1/7
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include "EC_IvtrItem.h"
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

struct TASKMATTER_ESSENCE;
struct TASKDICE_ESSENCE;
struct TASKNORMALMATTER_ESSENCE;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CECIvtrTaskItem
//	
///////////////////////////////////////////////////////////////////////////

class CECIvtrTaskItem : public CECIvtrItem
{
public:		//	Types

public:		//	Constructor and Destructor

	CECIvtrTaskItem(int tid, int expire_date);
	CECIvtrTaskItem(const CECIvtrTaskItem& s);
	virtual ~CECIvtrTaskItem();

public:		//	Attributes

public:		//	Operations

	//	Set item detail information
	virtual bool SetItemInfo(BYTE* pInfoData, int iDataLen);
	//	Get item icon file name
	virtual const char* GetIconFile();
	//	Get item name
	virtual const wchar_t* GetName();
	//	Clone item
	virtual CECIvtrItem* Clone() { return new CECIvtrTaskItem(*this); }
	
	const TASKMATTER_ESSENCE* GetDBEssence() { return m_pDBEssence; }

protected:	//	Attributes

	//	Data in database
	TASKMATTER_ESSENCE*	m_pDBEssence;

protected:	//	Operations

	//	Get item description text
	virtual const wchar_t* GetNormalDesc(bool bRepair);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECIvtrTaskDice
//	
///////////////////////////////////////////////////////////////////////////

class CECIvtrTaskDice : public CECIvtrItem
{
public:		//	Types

public:		//	Constructor and Destructor

	CECIvtrTaskDice(int tid, int expire_date);
	CECIvtrTaskDice(const CECIvtrTaskDice& s);
	virtual ~CECIvtrTaskDice();

public:		//	Attributes

public:		//	Operations

	//	Set item detail information
	virtual bool SetItemInfo(BYTE* pInfoData, int iDataLen);
	//	Get item icon file name
	virtual const char* GetIconFile();
	//	Get item name
	virtual const wchar_t* GetName();
	//	Clone item
	virtual CECIvtrItem* Clone() { return new CECIvtrTaskDice(*this); }
	//	Get drop model for shown
	virtual const char * GetDropModel();

	const TASKDICE_ESSENCE* GetDBEssence() { return m_pDBEssence; }

protected:	//	Attributes

	//	Data in database
	TASKDICE_ESSENCE*	m_pDBEssence;

protected:	//	Operations

	//	Get item description text
	virtual const wchar_t* GetNormalDesc(bool bRepair);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECIvtrTaskNmMatter
//	
///////////////////////////////////////////////////////////////////////////

class CECIvtrTaskNmMatter : public CECIvtrItem
{
public:		//	Types

public:		//	Constructor and Destructor

	CECIvtrTaskNmMatter(int tid, int expire_date);
	CECIvtrTaskNmMatter(const CECIvtrTaskNmMatter& s);
	virtual ~CECIvtrTaskNmMatter();

public:		//	Attributes

public:		//	Operations

	//	Set item detail information
	virtual bool SetItemInfo(BYTE* pInfoData, int iDataLen);
	//	Get item icon file name
	virtual const char* GetIconFile();
	//	Get item name
	virtual const wchar_t* GetName();
	//	Clone item
	virtual CECIvtrItem* Clone() { return new CECIvtrTaskNmMatter(*this); }
	//	Get drop model for shown
	virtual const char * GetDropModel();

	const TASKNORMALMATTER_ESSENCE* GetDBEssence() { return m_pDBEssence; }

protected:	//	Attributes

	//	Data in database
	TASKNORMALMATTER_ESSENCE*	m_pDBEssence;

protected:	//	Operations

	//	Get item description text
	virtual const wchar_t* GetNormalDesc(bool bRepair);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

