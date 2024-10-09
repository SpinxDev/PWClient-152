/*
 * FILE: EC_IvtrMoneyConvertible.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Shizhenhua, 2012/11/29
 *
 * HISTORY: 
 *
 * Copyright (c) 2011 ACE Studio, All Rights Reserved.
 */


#include "EC_Global.h"
#include "EC_IvtrMoneyConvertible.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_FixedMsg.h"
#include "elementdataman.h"
#include "EC_Configs.h"
#include "EC_HostPlayer.h"

#define new A_DEBUG_NEW



///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECIvtrMoneyConvertible
//	
///////////////////////////////////////////////////////////////////////////

CECIvtrMoneyConvertible::CECIvtrMoneyConvertible(int tid, int expire_date) : CECIvtrItem(tid, expire_date)
{
	m_iCID	= ICID_MONEYCONVERTIBLE;

	//	Get database data
	elementdataman* pDB = g_pGame->GetElementDataMan();
	DATA_TYPE DataType;
	m_pDBEssence = (MONEY_CONVERTIBLE_ESSENCE*)pDB->get_data_ptr(tid, ID_SPACE_ESSENCE, DataType);

	m_iPileLimit	= m_pDBEssence->pile_num_max;
	m_iPrice		= m_pDBEssence->price;
	m_iShopPrice	= m_pDBEssence->shop_price;
	m_iProcType		= m_pDBEssence->proc_type;
	m_i64EquipMask	= 0;

	m_bNeedUpdate	= false;
}

CECIvtrMoneyConvertible::CECIvtrMoneyConvertible(const CECIvtrMoneyConvertible& s) : CECIvtrItem(s)
{
	m_pDBEssence = s.m_pDBEssence;
}

CECIvtrMoneyConvertible::~CECIvtrMoneyConvertible()
{
}

//	Set item detail information
bool CECIvtrMoneyConvertible::SetItemInfo(BYTE* pInfoData, int iDataLen)
{
	CECIvtrItem::SetItemInfo(pInfoData, iDataLen);
	return true;
}

//	Get item icon file name
const char* CECIvtrMoneyConvertible::GetIconFile()
{
	return m_pDBEssence->file_icon;
}

//	Get item name
const wchar_t* CECIvtrMoneyConvertible::GetName()
{
	return m_pDBEssence->name;
}

//  Get the RMB price
__int64 CECIvtrMoneyConvertible::GetRealPrice() const
{
	return (__int64)m_pDBEssence->price * m_iCount * g_pGame->GetGameRun()->GetHostPlayer()->GetCashMoneyRate();
}

//	Get item description text
const wchar_t* CECIvtrMoneyConvertible::GetNormalDesc(bool bRepair)
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

	//	Extend description
	AddExtDescText();

	return m_strDesc;
}