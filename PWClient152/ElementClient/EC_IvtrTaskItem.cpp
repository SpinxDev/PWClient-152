/*
 * FILE: EC_IvtrTaskItem.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2005/1/7
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#include "EC_Global.h"
#include "EC_IvtrTaskItem.h"
#include "EC_Game.h"
#include "EC_FixedMsg.h"
#include "elementdataman.h"
#include "EC_Configs.h"

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
//	Implement CECIvtrTaskItem
//	
///////////////////////////////////////////////////////////////////////////

CECIvtrTaskItem::CECIvtrTaskItem(int tid, int expire_date) : CECIvtrItem(tid, expire_date)
{
	m_iCID	= ICID_TASKITEM;

	//	Get database data
	elementdataman* pDB = g_pGame->GetElementDataMan();
	DATA_TYPE DataType;
	m_pDBEssence = (TASKMATTER_ESSENCE*)pDB->get_data_ptr(tid, ID_SPACE_ESSENCE, DataType);

	m_iPileLimit	= m_pDBEssence->pile_num_max;
	m_iPrice		= 0;
	m_iShopPrice	= 0;
	m_iProcType		= m_pDBEssence->proc_type;
	m_i64EquipMask	= 0;

	m_bNeedUpdate	= false;
}

CECIvtrTaskItem::CECIvtrTaskItem(const CECIvtrTaskItem& s) : CECIvtrItem(s)
{
	m_pDBEssence	= s.m_pDBEssence;
}

CECIvtrTaskItem::~CECIvtrTaskItem()
{
}

//	Set item detail information
bool CECIvtrTaskItem::SetItemInfo(BYTE* pInfoData, int iDataLen)
{
	CECIvtrItem::SetItemInfo(pInfoData, iDataLen);
	return true;
}

//	Get item icon file name
const char* CECIvtrTaskItem::GetIconFile()
{
	return m_pDBEssence->file_icon;
}

//	Get item name
const wchar_t* CECIvtrTaskItem::GetName()
{
	return m_pDBEssence->name;
}

//	Get item description text
const wchar_t* CECIvtrTaskItem::GetNormalDesc(bool bRepair)
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

	//	Task item
	AddDescText(white, false, pDescTab->GetWideString(ITEMDESC_TASKITEM));

	//	Extend description
	AddExtDescText();

	return m_strDesc;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECIvtrTaskDice
//	
///////////////////////////////////////////////////////////////////////////

CECIvtrTaskDice::CECIvtrTaskDice(int tid, int expire_date) : CECIvtrItem(tid, expire_date)
{
	m_iCID	= ICID_TASKDICE;

	//	Get database data
	elementdataman* pDB = g_pGame->GetElementDataMan();
	DATA_TYPE DataType;
	m_pDBEssence = (TASKDICE_ESSENCE*)pDB->get_data_ptr(tid, ID_SPACE_ESSENCE, DataType);

	m_iPileLimit	= m_pDBEssence->pile_num_max;
	m_iPrice		= 0;
	m_iShopPrice	= 0;
	m_iProcType		= m_pDBEssence->proc_type;
	m_bUseable		= true;

	m_bNeedUpdate	= false;
}

CECIvtrTaskDice::CECIvtrTaskDice(const CECIvtrTaskDice& s) : CECIvtrItem(s)
{
	m_pDBEssence	= s.m_pDBEssence;
}

CECIvtrTaskDice::~CECIvtrTaskDice()
{
}

//	Set item detail information
bool CECIvtrTaskDice::SetItemInfo(BYTE* pInfoData, int iDataLen)
{
	CECIvtrItem::SetItemInfo(pInfoData, iDataLen);
	return true;
}

//	Get item icon file name
const char* CECIvtrTaskDice::GetIconFile()
{
	return m_pDBEssence->file_icon;
}

//	Get item name
const wchar_t* CECIvtrTaskDice::GetName()
{
	return m_pDBEssence->name;
}

//	Get item description text
const wchar_t* CECIvtrTaskDice::GetNormalDesc(bool bRepair)
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

	AddPriceDesc(white, bRepair);

	//	Extend description
	AddExtDescText();

	return m_strDesc;
}

//	Get drop model for shown
const char * CECIvtrTaskDice::GetDropModel()
{
	return m_pDBEssence->file_matter;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECIvtrTaskNmMatter
//	
///////////////////////////////////////////////////////////////////////////

CECIvtrTaskNmMatter::CECIvtrTaskNmMatter(int tid, int expire_date) : CECIvtrItem(tid, expire_date)
{
	m_iCID	= ICID_TASKNMMATTER;

	//	Get database data
	elementdataman* pDB = g_pGame->GetElementDataMan();
	DATA_TYPE DataType;
	m_pDBEssence = (TASKNORMALMATTER_ESSENCE*)pDB->get_data_ptr(tid, ID_SPACE_ESSENCE, DataType);

	m_iPileLimit	= m_pDBEssence->pile_num_max;
	m_iPrice		= m_pDBEssence->price;
	m_iShopPrice	= m_pDBEssence->shop_price;
	m_iProcType		= m_pDBEssence->proc_type;

	m_bNeedUpdate	= false;
}

CECIvtrTaskNmMatter::CECIvtrTaskNmMatter(const CECIvtrTaskNmMatter& s) : CECIvtrItem(s)
{
	m_pDBEssence	= s.m_pDBEssence;
}

CECIvtrTaskNmMatter::~CECIvtrTaskNmMatter()
{
}

//	Set item detail information
bool CECIvtrTaskNmMatter::SetItemInfo(BYTE* pInfoData, int iDataLen)
{
	CECIvtrItem::SetItemInfo(pInfoData, iDataLen);
	return true;
}

//	Get item icon file name
const char* CECIvtrTaskNmMatter::GetIconFile()
{
	return m_pDBEssence->file_icon;
}

//	Get item name
const wchar_t* CECIvtrTaskNmMatter::GetName()
{
	return m_pDBEssence->name;
}

//	Get item description text
const wchar_t* CECIvtrTaskNmMatter::GetNormalDesc(bool bRepair)
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
const char * CECIvtrTaskNmMatter::GetDropModel()
{
	return m_pDBEssence->file_matter;
}



