/*
 * FILE: EC_IvtrStone.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2005/1/21
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
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

struct STONE_ESSENCE;
struct STONE_SUB_TYPE;
struct REFINE_TICKET_ESSENCE;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CECIvtrStone
//	
///////////////////////////////////////////////////////////////////////////

class CECIvtrStone : public CECIvtrItem
{
public:		//	Types

public:		//	Constructor and Destructor

	CECIvtrStone(int tid, int expire_date);
	CECIvtrStone(const CECIvtrStone& s);
	virtual ~CECIvtrStone();

public:		//	Attributes

public:		//	Operations

	//	Set item detail information
	virtual bool SetItemInfo(BYTE* pInfoData, int iDataLen);
	//	Get item icon file name
	virtual const char* GetIconFile();
	//	Get item name
	virtual const wchar_t* GetName();
	//	Clone item
	virtual CECIvtrItem* Clone() { return new CECIvtrStone(*this); }
	//	Get drop model for shown
	virtual const char * GetDropModel();

	const STONE_ESSENCE* GetDBEssence() { return m_pDBEssence; }
	const STONE_SUB_TYPE* GetDBSubType() { return m_pDBSubType; }

protected:	//	Attributes

	//	Data in database
	STONE_ESSENCE*		m_pDBEssence;
	STONE_SUB_TYPE*		m_pDBSubType;

protected:	//	Operations

	//	Get item description text
	virtual const wchar_t* GetNormalDesc(bool bRepair);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECIvtrRefineTicket
//	
///////////////////////////////////////////////////////////////////////////

class CECIvtrRefineTicket : public CECIvtrItem
{
public:		//	Types

public:		//	Constructor and Destructor

	CECIvtrRefineTicket(int tid, int expire_date);
	CECIvtrRefineTicket(const CECIvtrRefineTicket& s);
	virtual ~CECIvtrRefineTicket();

public:		//	Attributes

public:		//	Operations

	//	Set item detail information
	virtual bool SetItemInfo(BYTE* pInfoData, int iDataLen);
	//	Get item icon file name
	virtual const char* GetIconFile();
	//	Get item name
	virtual const wchar_t* GetName();
	//	Clone item
	virtual CECIvtrItem* Clone() { return new CECIvtrRefineTicket(*this); }
	//	Get drop model for shown
	virtual const char * GetDropModel();

	//	Get database data
	const REFINE_TICKET_ESSENCE* GetDBEssence() { return m_pDBEssence; }

protected:	//	Operations

	//	Data in database
	REFINE_TICKET_ESSENCE*		m_pDBEssence;

protected:	//	Operations

	//	Get item description text
	virtual const wchar_t* GetNormalDesc(bool bRepair);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////


