/*
 * FILE: EC_IvtrMaterial.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2005/1/4
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#include "EC_Global.h"
#include "EC_IvtrMaterial.h"
#include "EC_Game.h"
#include "../CElementClient/EC_FixedMsg.h"
#include "../CCommon/elementdataman.h"

#define new A_DEBUG_NEW

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Reference to External variables and functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local Types and Variables and Global variables
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECIvtrMaterial
//	
///////////////////////////////////////////////////////////////////////////

CECIvtrMaterial::CECIvtrMaterial(int tid, int expire_date) : CECIvtrItem(tid, expire_date)
{
	m_iCID	= ICID_MATERIAL;

	//	Get database data
	elementdataman* pDB = g_pGame->GetElementDataMan();
	DATA_TYPE DataType;
	m_pDBEssence	= (MATERIAL_ESSENCE*)pDB->get_data_ptr(tid, ID_SPACE_ESSENCE, DataType);
	m_pDBMajorType	= (MATERIAL_MAJOR_TYPE*)pDB->get_data_ptr(m_pDBEssence->id_major_type, ID_SPACE_ESSENCE, DataType);
	m_pDBSubType	= (MATERIAL_SUB_TYPE*)pDB->get_data_ptr(m_pDBEssence->id_sub_type, ID_SPACE_ESSENCE, DataType);

	m_iPileLimit	= m_pDBEssence->pile_num_max;
	m_iPrice		= m_pDBEssence->price;
	m_iShopPrice	= m_pDBEssence->shop_price;
	m_iProcType		= m_pDBEssence->proc_type;
	m_i64EquipMask	= 0;

	m_bNeedUpdate	= false;
}

CECIvtrMaterial::CECIvtrMaterial(const CECIvtrMaterial& s) : CECIvtrItem(s)
{
	m_pDBEssence	= s.m_pDBEssence;
	m_pDBMajorType	= s.m_pDBMajorType;
	m_pDBSubType	= s.m_pDBSubType;
}

CECIvtrMaterial::~CECIvtrMaterial()
{
}

//	Set item detail information
bool CECIvtrMaterial::SetItemInfo(BYTE* pInfoData, int iDataLen)
{
	CECIvtrItem::SetItemInfo(pInfoData, iDataLen);
	return true;
}

//	Get item icon file name
const char* CECIvtrMaterial::GetIconFile()
{
	return m_pDBEssence->file_icon;
}

//	Get item name
const wchar_t* CECIvtrMaterial::GetName()
{
	return m_pDBEssence->name;
}

//	Get item description text
const wchar_t* CECIvtrMaterial::GetNormalDesc(bool bRepair)
{
	m_strDesc = _AL("");

	//	Try to build item description
	CECStringTab* pDescTab = g_pGame->GetItemDesc();
	int white = ITEMDESC_COL_WHITE;
	int namecol = DecideNameCol();

	if (m_iCount > 1)
		AddDescText(namecol, true, pDescTab->GetWideString(ITEMDESC_NAMENUMBER), GetName(), m_iCount);
	else
		AddDescText(namecol, true, pDescTab->GetWideString(ITEMDESC_NAME), GetName());

	AddIDDescText();
	
	AddExpireTimeDesc();

	//	Price
	AddPriceDesc(white, bRepair);

	//	Extend description
	AddExtDescText();

	return m_strDesc;
}

//	Get drop model for shown
const char * CECIvtrMaterial::GetDropModel()
{
	return m_pDBEssence->file_matter;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECIvtrRecipe
//	
///////////////////////////////////////////////////////////////////////////

CECIvtrRecipe::CECIvtrRecipe(int tid, int expire_date) : CECIvtrItem(tid, expire_date)
{
	m_iCID	= ICID_RECIPE;

	//	Get database data
	elementdataman* pDB = g_pGame->GetElementDataMan();
	DATA_TYPE DataType;
	m_pDBEssence	= (RECIPE_ESSENCE*)pDB->get_data_ptr(tid, ID_SPACE_RECIPE, DataType);
	m_pDBMajorType	= (RECIPE_MAJOR_TYPE*)pDB->get_data_ptr(m_pDBEssence->id_major_type, ID_SPACE_RECIPE, DataType);
	m_pDBSubType	= (RECIPE_SUB_TYPE*)pDB->get_data_ptr(m_pDBEssence->id_sub_type, ID_SPACE_RECIPE, DataType);

	m_iPileLimit	= 1;
	m_iPrice		= m_pDBEssence->price;
	m_iShopPrice	= 0;
	m_i64EquipMask	= 0;

	m_bNeedUpdate	= false;

	//	Create first target item of this recipe. 
	m_pGenItem1 = CECIvtrItem::CreateItem(m_pDBEssence->targets[0].id_to_make, 0, 1);
	if (m_pGenItem1)
	{
		m_pGenItem1->GetDetailDataFromLocal();
		m_pGenItem1->SetInNPCPack(true);
		m_pGenItem1->SetPriceScale(CECIvtrItem::SCALE_MAKE, 1.0f);
	}
}

CECIvtrRecipe::CECIvtrRecipe(const CECIvtrRecipe& s) : CECIvtrItem(s)
{
	m_pDBEssence	= s.m_pDBEssence;
	m_pDBMajorType	= s.m_pDBMajorType;
	m_pDBSubType	= s.m_pDBSubType;

	if (s.m_pGenItem1)
		m_pGenItem1 = s.m_pGenItem1->Clone();
	else
		m_pGenItem1 = NULL;
}

CECIvtrRecipe::~CECIvtrRecipe()
{
	if (m_pGenItem1)
		delete m_pGenItem1;
}

//	Set item detail information
bool CECIvtrRecipe::SetItemInfo(BYTE* pInfoData, int iDataLen)
{
	CECIvtrItem::SetItemInfo(pInfoData, iDataLen);
	return true;
}

//	Get item icon file name
const char* CECIvtrRecipe::GetIconFile()
{
	if (m_pGenItem1)
		return m_pGenItem1->GetIconFile();
	else
		return "Unknown.dds";
}

//	Get item name
const wchar_t* CECIvtrRecipe::GetName()
{
	if (m_pGenItem1)
		return m_pGenItem1->GetName();
	else
	{
		CECStringTab* pDescTab = g_pGame->GetItemDesc();
		return pDescTab->GetWideString(ITEMDESC_ERRORITEM);
	}
}

//	Get item description text
const wchar_t* CECIvtrRecipe::GetNormalDesc(bool bRepair)
{
	if (m_pGenItem1)
		return m_pGenItem1->GetDesc(DESC_NORMAL);
	else
	{
		m_strDesc = _AL("");
		CECStringTab* pDescTab = g_pGame->GetItemDesc();
		AddDescText(ITEMDESC_COL_WHITE, false, pDescTab->GetWideString(ITEMDESC_ERRORITEM));
		AddDescText(ITEMDESC_COL_WHITE, false, _AL("(%d)"), m_tid);
		return m_strDesc;
	}
}



