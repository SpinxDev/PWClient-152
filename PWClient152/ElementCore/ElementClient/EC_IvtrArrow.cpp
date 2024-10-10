/*
 * FILE: EC_IvtrArrow.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/11/19
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "EC_Global.h"
#include "EC_IvtrArrow.h"
#include "EC_Game.h"
#include "EC_FixedMsg.h"
#include "EC_RTDebug.h"
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
//	Implement CECIvtrArrow
//	
///////////////////////////////////////////////////////////////////////////

CECIvtrArrow::CECIvtrArrow(int tid, int expire_date) : CECIvtrEquip(tid, expire_date)
{
	m_iCID	= ICID_ARROW;

	memset(&m_Essence, 0, sizeof (m_Essence));

	//	Get database data
	elementdataman* pDB = g_pGame->GetElementDataMan();
	DATA_TYPE DataType;
	m_pDBEssence	= (PROJECTILE_ESSENCE*)pDB->get_data_ptr(tid, ID_SPACE_ESSENCE, DataType);
	m_pDBType		= (PROJECTILE_TYPE*)pDB->get_data_ptr(m_pDBEssence->type, ID_SPACE_ESSENCE, DataType);

	m_iPileLimit	= m_pDBEssence->pile_num_max;
	m_iPrice		= m_pDBEssence->price;
	m_iShopPrice	= m_pDBEssence->shop_price;
	m_iProcType		= m_pDBEssence->proc_type;
	m_i64EquipMask	= EQUIP_MASK64_PROJECTILE;
}

CECIvtrArrow::CECIvtrArrow(const CECIvtrArrow& s) : CECIvtrEquip(s)
{
	m_pDBType		= s.m_pDBType;
	m_pDBEssence	= s.m_pDBEssence;
	m_Essence		= s.m_Essence;
}

CECIvtrArrow::~CECIvtrArrow()
{
}

//	Set item detail information
bool CECIvtrArrow::SetItemInfo(BYTE* pInfoData, int iDataLen)
{
	CECIvtrEquip::SetItemInfo(pInfoData, iDataLen);

	if (!pInfoData || !iDataLen)
		return true;

	try
	{
		CECDataReader dr(pInfoData, iDataLen);

		//	Skip equip requirements and endurance
		dr.Offset(5 * sizeof (int), SEEK_CUR);

		int iEssenceSize = dr.Read_int();
		ASSERT(iEssenceSize == sizeof (IVTR_ESSENCE_ARROW));
	
		m_Essence = *(IVTR_ESSENCE_ARROW*)dr.Read_Data(iEssenceSize);
	}
	catch (CECException& e)
	{
		ASSERT(0);
		a_LogOutput(1, "CECIvtrArrow::SetItemInfo, data read error (%d)", e.GetType());
		return false;
	}

	return true;
}

//	Get item icon file name
const char* CECIvtrArrow::GetIconFile()
{
	return m_pDBEssence->file_icon;
}

//	Get item name
const wchar_t* CECIvtrArrow::GetName()
{
	return m_pDBEssence->name;
}

//	Get item description text
const wchar_t* CECIvtrArrow::GetNormalDesc(bool bRepair)
{
	if (m_bNeedUpdate)
		return NULL;

	//	Build addon and refine properties and save it
	m_strDesc = _AL("");
	BuildAddOnPropDesc(NULL, NULL);
	AWString strAddon = m_strDesc;

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

	//	Weapon requirement
	AddDescText(white, true, pDescTab->GetWideString(ITEMDESC_WEAPONREQ), m_Essence.iWeaponReqLow, 
			m_Essence.iWeaponReqHigh, m_pDBType->name);

	//	Damage enhance
	if (m_pDBEssence->damage_enhance)
	{
		AddDescText(-1, false, pDescTab->GetWideString(ITEMDESC_ADDPHYDAMAGE));
		AddDescText(-1, true, _AL(" %+d"), m_pDBEssence->damage_enhance);
	}
	
	//	Add addon properties
	if (strAddon.GetLength())
		m_strDesc += strAddon;

	//	Price
	AddPriceDesc(white, bRepair);

	//	Suite description
	AddSuiteDesc();
	
	//	Extend description
	AddExtDescText();

	return m_strDesc;
}

//	Get drop model for shown
const char * CECIvtrArrow::GetDropModel()
{
	return m_pDBEssence->file_matter;
}
