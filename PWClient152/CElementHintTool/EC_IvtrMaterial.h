/*
 * FILE: EC_IvtrMaterial.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2005/1/4
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

struct MATERIAL_MAJOR_TYPE;
struct MATERIAL_SUB_TYPE;
struct MATERIAL_ESSENCE;
struct RECIPE_MAJOR_TYPE;
struct RECIPE_SUB_TYPE;
struct RECIPE_ESSENCE;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CECIvtrMaterial
//	
///////////////////////////////////////////////////////////////////////////

class CECIvtrMaterial : public CECIvtrItem
{
public:		//	Types

public:		//	Constructor and Destructor

	CECIvtrMaterial(int tid, int expire_date);
	CECIvtrMaterial(const CECIvtrMaterial& s);
	virtual ~CECIvtrMaterial();

public:		//	Attributes

public:		//	Operations

	//	Set item detail information
	virtual bool SetItemInfo(BYTE* pInfoData, int iDataLen);
	//	Get item icon file name
	virtual const char* GetIconFile();
	//	Get item name
	virtual const wchar_t* GetName();
	//	Clone item
	virtual CECIvtrItem* Clone() { return new CECIvtrMaterial(*this); }
	//	Get drop model for shown
	virtual const char * GetDropModel();

	//	Get database data
	const MATERIAL_MAJOR_TYPE* GetDBMajorType() { return m_pDBMajorType; }
	const MATERIAL_SUB_TYPE* GetDBSubType() { return m_pDBSubType; }
	const MATERIAL_ESSENCE* GetDBEssence() { return m_pDBEssence; }

protected:	//	Operations

	//	Data in database
	MATERIAL_MAJOR_TYPE*	m_pDBMajorType;
	MATERIAL_SUB_TYPE*		m_pDBSubType;
	MATERIAL_ESSENCE*		m_pDBEssence;

protected:	//	Operations

	//	Get item description text
	virtual const wchar_t* GetNormalDesc(bool bRepair);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECIvtrRecipe
//	
///////////////////////////////////////////////////////////////////////////

class CECIvtrRecipe : public CECIvtrItem
{
public:		//	Types

public:		//	Constructor and Destructor

	CECIvtrRecipe(int tid, int expire_date);
	CECIvtrRecipe(const CECIvtrRecipe& s);
	virtual ~CECIvtrRecipe();

public:		//	Attributes

public:		//	Operations

	//	Set item detail information
	virtual bool SetItemInfo(BYTE* pInfoData, int iDataLen);
	//	Get item icon file name
	virtual const char* GetIconFile();
	//	Get item name
	virtual const wchar_t* GetName();
	//	Clone item
	virtual CECIvtrItem* Clone() { return new CECIvtrRecipe(*this); }
	
	//	Get frist target item in this recipe
	CECIvtrItem* GetItem1() { return m_pGenItem1; }

	//	Get database data
	const RECIPE_MAJOR_TYPE* GetDBMajorType() { return m_pDBMajorType; }
	const RECIPE_SUB_TYPE* GetDBSubType() { return m_pDBSubType; }
	const RECIPE_ESSENCE* GetDBEssence() { return m_pDBEssence; }

protected:	//	Attributes

	//	Data in database
	RECIPE_MAJOR_TYPE*	m_pDBMajorType;
	RECIPE_SUB_TYPE*	m_pDBSubType;
	RECIPE_ESSENCE*		m_pDBEssence;

	CECIvtrItem*		m_pGenItem1;

protected:	//	Operations

	//	Get item description text
	virtual const wchar_t* GetNormalDesc(bool bRepair);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////


