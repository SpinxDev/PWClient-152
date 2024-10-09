/*
 * FILE: EC_IvtrStone.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2005/1/21
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#include "EC_Global.h"
#include "EC_IvtrStone.h"
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
//	Implement CECIvtrStone
//	
///////////////////////////////////////////////////////////////////////////

CECIvtrStone::CECIvtrStone(int tid, int expire_date) : CECIvtrItem(tid, expire_date)
{
	m_iCID	= ICID_STONE;

	//	Get database data
	elementdataman* pDB = g_pGame->GetElementDataMan();
	DATA_TYPE DataType;
	m_pDBEssence	= (STONE_ESSENCE*)pDB->get_data_ptr(tid, ID_SPACE_ESSENCE, DataType);
	m_pDBSubType	= (STONE_SUB_TYPE*)pDB->get_data_ptr(m_pDBEssence->id_sub_type, ID_SPACE_ESSENCE, DataType);

	m_iPileLimit	= m_pDBEssence->pile_num_max;
	m_iPrice		= m_pDBEssence->price;
	m_iShopPrice	= m_pDBEssence->shop_price;
	m_iProcType		= m_pDBEssence->proc_type;
	m_i64EquipMask	= 0;
	m_bEmbeddable	= true;

	m_bNeedUpdate	= false;
}

CECIvtrStone::CECIvtrStone(const CECIvtrStone& s) : CECIvtrItem(s)
{
	m_pDBEssence	= s.m_pDBEssence;
	m_pDBSubType	= s.m_pDBSubType;
}

CECIvtrStone::~CECIvtrStone()
{
}

//	Set item detail information
bool CECIvtrStone::SetItemInfo(BYTE* pInfoData, int iDataLen)
{
	CECIvtrItem::SetItemInfo(pInfoData, iDataLen);
	return true;
}

//	Get item icon file name
const char* CECIvtrStone::GetIconFile()
{
	return m_pDBEssence->file_icon;
}

//	Get item name
const wchar_t* CECIvtrStone::GetName()
{
	return m_pDBEssence->name;
}

//	Get item description text
const wchar_t* CECIvtrStone::GetNormalDesc(bool bRepair)
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
const char * CECIvtrStone::GetDropModel()
{
	return m_pDBEssence->file_matter;
}


///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECIvtrRefineTicket
//	
///////////////////////////////////////////////////////////////////////////

CECIvtrRefineTicket::CECIvtrRefineTicket(int tid, int expire_date) : CECIvtrItem(tid, expire_date)
{
	m_iCID	= ICID_REFINETICKET;

	//	Get database data
	elementdataman* pDB = g_pGame->GetElementDataMan();
	DATA_TYPE DataType;
	m_pDBEssence	= (REFINE_TICKET_ESSENCE*)pDB->get_data_ptr(tid, ID_SPACE_ESSENCE, DataType);
	
	m_iPileLimit	= m_pDBEssence->pile_num_max;
	m_iPrice		= m_pDBEssence->price;
	m_iShopPrice	= m_pDBEssence->shop_price;
	m_iProcType		= m_pDBEssence->proc_type;
	m_i64EquipMask	= 0;

	m_bNeedUpdate	= false;
}

CECIvtrRefineTicket::CECIvtrRefineTicket(const CECIvtrRefineTicket& s) : CECIvtrItem(s)
{
	m_pDBEssence	= s.m_pDBEssence;
}

CECIvtrRefineTicket::~CECIvtrRefineTicket()
{
}

//	Set item detail information
bool CECIvtrRefineTicket::SetItemInfo(BYTE* pInfoData, int iDataLen)
{
	CECIvtrItem::SetItemInfo(pInfoData, iDataLen);
	return true;
}

//	Get item icon file name
const char* CECIvtrRefineTicket::GetIconFile()
{
	return m_pDBEssence->file_icon;
}

//	Get item name
const wchar_t* CECIvtrRefineTicket::GetName()
{
	return m_pDBEssence->name;
}

//	Get item description text
const wchar_t* CECIvtrRefineTicket::GetNormalDesc(bool bRepair)
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
	
	if (GetDBEssence()->binding_only)
		AddDescText(white, true, pDescTab->GetWideString(ITEMDESC_EQUIP_BIND_ONLY));
	
	if (GetDBEssence()->require_level_max > 1)
		AddDescText(white, true, pDescTab->GetWideString(ITEMDESC_REQUIRE_MAX_LEVEL), GetDBEssence()->require_level_max);
	
	//	Price
	AddPriceDesc(white, bRepair);

	//	Extend description
	AddExtDescText();

	return m_strDesc;
}

//	Get drop model for shown
const char * CECIvtrRefineTicket::GetDropModel()
{
	return m_pDBEssence->file_matter;
}

