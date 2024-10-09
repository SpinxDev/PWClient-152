/*
 * FILE: EC_IvtrConsume.cpp
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
#include "EC_IvtrConsume.h"
#include "EC_Game.h"
#include "../CElementClient/EC_FixedMsg.h"
#include "../CCommon/elementdataman.h"
#include "../CElementClient/EC_Skill.h"
#include "EC_RTDebug.h"
#include "EC_Instance.h"

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
//	Implement CECIvtrArmorRune
//	
///////////////////////////////////////////////////////////////////////////

CECIvtrArmorRune::CECIvtrArmorRune(int tid, int expire_date) : CECIvtrItem(tid, expire_date)
{
	m_iCID	= ICID_ARMORRUNE;

	//	Get database data
	elementdataman* pDB = g_pGame->GetElementDataMan();
	DATA_TYPE DataType;
	m_pDBEssence	= (ARMORRUNE_ESSENCE*)pDB->get_data_ptr(tid, ID_SPACE_ESSENCE, DataType);
	m_pDBSubType	= (ARMORRUNE_SUB_TYPE*)pDB->get_data_ptr(m_pDBEssence->id_sub_type, ID_SPACE_ESSENCE, DataType);

	m_iPileLimit	= m_pDBEssence->pile_num_max;
	m_iPrice		= m_pDBEssence->price;
	m_iShopPrice	= m_pDBEssence->shop_price;
	m_iProcType		= m_pDBEssence->proc_type;
	m_i64EquipMask	= 0;
	m_bUseable		= true;

	m_bNeedUpdate	= false;
}

CECIvtrArmorRune::CECIvtrArmorRune(const CECIvtrArmorRune& s) : CECIvtrItem(s)
{
	m_pDBEssence	= s.m_pDBEssence;
	m_pDBSubType	= s.m_pDBSubType;
}

CECIvtrArmorRune::~CECIvtrArmorRune()
{
}

//	Set item detail information
bool CECIvtrArmorRune::SetItemInfo(BYTE* pInfoData, int iDataLen)
{
	CECIvtrItem::SetItemInfo(pInfoData, iDataLen);
	return true;
}

//	Get item icon file name
const char* CECIvtrArmorRune::GetIconFile()
{
	return m_pDBEssence->file_icon;
}

//	Get item name
const wchar_t* CECIvtrArmorRune::GetName()
{
	return m_pDBEssence->name;
}

//	Get item cool time
//int CECIvtrArmorRune::GetCoolTime(int* piMax/* NULL */)
//{
//	CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
//	return pHost ? pHost->GetCoolTime(GP_CT_ARMORRUNE, piMax) : 0;
// }

//	Get item description text
const wchar_t* CECIvtrArmorRune::GetNormalDesc(bool bRepair)
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

	//	Using requirment
	if (m_pDBEssence->require_player_level_max > 0)
	{
		AddDescText(white, false, pDescTab->GetWideString(ITEMDESC_USEREQUIREMENT));
		AddDescText(white, true, pDescTab->GetWideString(ITEMDESC_MAXLEVELREQ), m_pDBEssence->require_player_level_max);
	}

	//	Defense enhance
	if (!m_pDBEssence->damage_type)
	{
		AddDescText(white, false, pDescTab->GetWideString(ITEMDESC_USEEFFECT));
		AddDescText(white, true, pDescTab->GetWideString(ITEMDESC_PHYRESIST), (int)(m_pDBEssence->damage_reduce_percent * 100.0f + 0.5f));
	}
	else
	{
		AddDescText(white, false, pDescTab->GetWideString(ITEMDESC_USEEFFECT));
		AddDescText(white, true, pDescTab->GetWideString(ITEMDESC_ALLMAGICRESIST), (int)(m_pDBEssence->damage_reduce_percent * 100.0f + 0.5f));
	}

	//	Price
	AddPriceDesc(white, bRepair);

	//	Extend description
	AddExtDescText();

	return m_strDesc;
}

//	Get drop model for shown
const char * CECIvtrArmorRune::GetDropModel()
{
	return m_pDBEssence->file_matter;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECIvtrDmgRune
//	
///////////////////////////////////////////////////////////////////////////

CECIvtrDmgRune::CECIvtrDmgRune(int tid, int expire_date) : CECIvtrEquip(tid, expire_date)
{
	m_iCID	= ICID_DMGRUNE;

	//	Get database data
	elementdataman* pDB = g_pGame->GetElementDataMan();
	DATA_TYPE DataType;
	m_pDBEssence	= (DAMAGERUNE_ESSENCE*)pDB->get_data_ptr(tid, ID_SPACE_ESSENCE, DataType);
	m_pDBSubType	= (DAMAGERUNE_SUB_TYPE*)pDB->get_data_ptr(m_pDBEssence->id_sub_type, ID_SPACE_ESSENCE, DataType);

	m_iPileLimit	= m_pDBEssence->pile_num_max;
	m_iPrice		= m_pDBEssence->price;
	m_iShopPrice	= m_pDBEssence->shop_price;
	m_iProcType		= m_pDBEssence->proc_type;
	m_i64EquipMask	= EQUIP_MASK64_RUNE;
	m_iCurEndurance	= 1;
	m_iMaxEndurance	= 1;
	
	m_bNeedUpdate	= false;
}

CECIvtrDmgRune::CECIvtrDmgRune(const CECIvtrDmgRune& s) : CECIvtrEquip(s)
{
	m_pDBEssence	= s.m_pDBEssence;
	m_pDBSubType	= s.m_pDBSubType;
}

CECIvtrDmgRune::~CECIvtrDmgRune()
{
}

//	Set item detail information
bool CECIvtrDmgRune::SetItemInfo(BYTE* pInfoData, int iDataLen)
{
	//	Note: because damage rune isn't an absolute equipment, so skip
	//	CECIvtrEquip::SetItemInfo(pInfoData, iDataLen);
	m_bNeedUpdate = false;
	return true;
}

//	Get item icon file name
const char* CECIvtrDmgRune::GetIconFile()
{
	return m_pDBEssence->file_icon;
}

//	Get item name
const wchar_t* CECIvtrDmgRune::GetName()
{
	return m_pDBEssence->name;
}

//	Get item description text
const wchar_t* CECIvtrDmgRune::GetNormalDesc(bool bRepair)
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

	//	Using requirment
	if (m_pDBEssence->require_weapon_level_min > 0 || m_pDBEssence->require_weapon_level_max > 0)
	{
		AddDescText(white, false, pDescTab->GetWideString(ITEMDESC_USEREQUIREMENT));
		AddDescText(white, true, pDescTab->GetWideString(ITEMDESC_WEAPONLVLREQ), 
			m_pDBEssence->require_weapon_level_min, m_pDBEssence->require_weapon_level_max);
	}

	//	Damage enhance
	if (!m_pDBEssence->damage_type)
	{
		AddDescText(white, false, pDescTab->GetWideString(ITEMDESC_USEEFFECT));
		AddDescText(white, true, pDescTab->GetWideString(ITEMDESC_PHYDMGEXTRA), m_pDBEssence->damage_increased);
	}
	else
	{
		AddDescText(white, false, pDescTab->GetWideString(ITEMDESC_USEEFFECT));
		AddDescText(white, true, pDescTab->GetWideString(ITEMDESC_MAGICDMGEXTRA), m_pDBEssence->damage_increased);
	}

	//	Price
	AddPriceDesc(white, bRepair);

	//	Extend description
	AddExtDescText();

	return m_strDesc;
}

//	Get drop model for shown
const char * CECIvtrDmgRune::GetDropModel()
{
	return m_pDBEssence->file_matter;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECIvtrMedicine
//	
///////////////////////////////////////////////////////////////////////////

CECIvtrMedicine::CECIvtrMedicine(int tid, int expire_date) : CECIvtrItem(tid, expire_date)
{
	m_iCID	= ICID_MEDICINE;

	//	Get database data
	elementdataman* pDB = g_pGame->GetElementDataMan();
	DATA_TYPE DataType;
	m_pDBEssence	= (MEDICINE_ESSENCE*)pDB->get_data_ptr(tid, ID_SPACE_ESSENCE, DataType);
	m_pDBMajorType	= (MEDICINE_MAJOR_TYPE*)pDB->get_data_ptr(m_pDBEssence->id_major_type, ID_SPACE_ESSENCE, DataType);
	m_pDBSubType	= (MEDICINE_SUB_TYPE*)pDB->get_data_ptr(m_pDBEssence->id_sub_type, ID_SPACE_ESSENCE, DataType);

	m_iPileLimit	= m_pDBEssence->pile_num_max;
	m_iPrice		= m_pDBEssence->price;
	m_iShopPrice	= m_pDBEssence->shop_price;
	m_i64EquipMask	= 0;
	m_iLevelReq		= m_pDBEssence->require_level;
	m_iProcType		= m_pDBEssence->proc_type;
	m_bUseable		= true;

	m_bNeedUpdate	= false;
}

CECIvtrMedicine::CECIvtrMedicine(const CECIvtrMedicine& s) : CECIvtrItem(s)
{
	m_pDBEssence	= s.m_pDBEssence;
	m_pDBMajorType	= s.m_pDBMajorType;
	m_pDBSubType	= s.m_pDBSubType;
	m_iLevelReq		= s.m_iLevelReq;
}

CECIvtrMedicine::~CECIvtrMedicine()
{
}

//	Set item detail information
bool CECIvtrMedicine::SetItemInfo(BYTE* pInfoData, int iDataLen)
{
	CECIvtrItem::SetItemInfo(pInfoData, iDataLen);
	return true;
}

//	Get item icon file name
const char* CECIvtrMedicine::GetIconFile()
{
	return m_pDBEssence->file_icon;
}

//	Get item name
const wchar_t* CECIvtrMedicine::GetName()
{
	return m_pDBEssence->name;
}

//	Get item cool time
//int CECIvtrMedicine::GetCoolTime(int* piMax/* NULL */)
//{
//	CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
//	if (!pHost)
//		return 0;
//
//	int iTime = 0;
//
//	switch (m_pDBMajorType->id)
//	{
//	case 1810:	iTime = pHost->GetCoolTime(GP_CT_REJUVENATION_POTION, piMax);	break;
//	case 1794:	iTime = pHost->GetCoolTime(GP_CT_HP_POTION, piMax);				break;
//	case 1802:	iTime = pHost->GetCoolTime(GP_CT_MP_POTION, piMax);				break;
//	case 1815:
//	case 2038:	iTime = pHost->GetCoolTime(GP_CT_ANTIDOTE_POTION, piMax);		break;
//	}
//
//	return iTime;
// }

//	Get item description text
const wchar_t* CECIvtrMedicine::GetNormalDesc(bool bRepair)
{
	m_strDesc = _AL("");

	//	Try to build item description
	CECStringTab* pDescTab = g_pGame->GetItemDesc();
	int red = ITEMDESC_COL_RED;
	int white = ITEMDESC_COL_WHITE;
	int namecol = DecideNameCol();

	if (m_iCount > 1)
		AddDescText(namecol, true, pDescTab->GetWideString(ITEMDESC_NAMENUMBER), GetName(), m_iCount);
	else
		AddDescText(namecol, true, pDescTab->GetWideString(ITEMDESC_NAME), GetName());

	AddIDDescText();

	AddExpireTimeDesc();

	//	Level requirement
	if (m_iLevelReq)
	{
		int col = white;
		AddDescText(col, true, pDescTab->GetWideString(ITEMDESC_LEVELREQ), m_iLevelReq);
	}

	//  Show item id
	AddIDDescText();

	//	Build effect description
	BuildEffectDesc();

	//	Price
	AddPriceDesc(white, bRepair);

	//	Extend description
	AddExtDescText();

	return m_strDesc;
}

//	Build medicine effect description
void CECIvtrMedicine::BuildEffectDesc()
{
	CECStringTab* pDescTab = g_pGame->GetItemDesc();
	int white = ITEMDESC_COL_WHITE;

	AddDescText(white, false, pDescTab->GetWideString(ITEMDESC_USEEFFECT));

	switch (m_pDBMajorType->id)
	{
	case 1794:	//	Recruit HP

		if (m_pDBEssence->hp_add_total)
		{
			if (m_pDBEssence->hp_add_time)
				AddDescText(white, true, pDescTab->GetWideString(ITEMDESC_ADDHPINTIME), m_pDBEssence->hp_add_time, m_pDBEssence->hp_add_total);
			else
				AddDescText(white, true, pDescTab->GetWideString(ITEMDESC_RECRUITHP), m_pDBEssence->hp_add_total);
		}

		break;
		
	case 1802:	//	Recruit MP

		if (m_pDBEssence->mp_add_total)
		{
			if (m_pDBEssence->mp_add_time)
				AddDescText(white, true, pDescTab->GetWideString(ITEMDESC_ADDMPINTIME), m_pDBEssence->mp_add_time, m_pDBEssence->mp_add_total);
			else
				AddDescText(white, true, pDescTab->GetWideString(ITEMDESC_RECRUITMP), m_pDBEssence->mp_add_total);
		}

		break;

	case 1810:	//	Recruit HP and MP

		AddDescText(white, true, pDescTab->GetWideString(ITEMDESC_RECRUITHPMP), m_pDBEssence->hp_add_total, m_pDBEssence->mp_add_total);
		break;

	case 1815:

		AddDescText(white, true, pDescTab->GetWideString(ITEMDESC_DECHALFPOISON));
		break;

	case 2038:

		AddDescText(white, true, pDescTab->GetWideString(ITEMDESC_ANTIDOTE));
		break;

	default:
		
		ASSERT(0);
		return;
	}
}

//	Check item use condition
//bool CECIvtrMedicine::CheckUseCondition()
//{
//	CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
//
//	if (pHost->GetBasicProps().iLevel < GetLevelRequirement())
//		return false;
//
//	return true;
//}

//	Get drop model for shown
const char * CECIvtrMedicine::GetDropModel()
{
	return m_pDBEssence->file_matter;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECIvtrElement
//	
///////////////////////////////////////////////////////////////////////////

CECIvtrElement::CECIvtrElement(int tid, int expire_date) : CECIvtrItem(tid, expire_date)
{
	m_iCID	= ICID_ELEMENT;

	//	Get database data
	elementdataman* pDB = g_pGame->GetElementDataMan();
	DATA_TYPE DataType;
	m_pDBEssence = (ELEMENT_ESSENCE*)pDB->get_data_ptr(tid, ID_SPACE_ESSENCE, DataType);

	m_iPileLimit	= m_pDBEssence->pile_num_max;
	m_iPrice		= m_pDBEssence->price;
	m_iShopPrice	= m_pDBEssence->shop_price;
	m_iProcType		= m_pDBEssence->proc_type;
	m_i64EquipMask	= 0;

	m_bNeedUpdate	= false;
}

CECIvtrElement::CECIvtrElement(const CECIvtrElement& s) : CECIvtrItem(s)
{
	m_pDBEssence	= s.m_pDBEssence;
}

CECIvtrElement::~CECIvtrElement()
{
}

//	Set item detail information
bool CECIvtrElement::SetItemInfo(BYTE* pInfoData, int iDataLen)
{
	CECIvtrItem::SetItemInfo(pInfoData, iDataLen);
	return true;
}

//	Get item icon file name
const char* CECIvtrElement::GetIconFile()
{
	return m_pDBEssence->file_icon;
}

//	Get item name
const wchar_t* CECIvtrElement::GetName()
{
	return m_pDBEssence->name;
}

//	Get item description text
const wchar_t* CECIvtrElement::GetNormalDesc(bool bRepair)
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

	//	level
	AddDescText(white, true, pDescTab->GetWideString(ITEMDESC_LEVEL), GetDBEssence()->level);

	//	Price
	AddPriceDesc(white, bRepair);

	//	Extend description
	AddExtDescText();
	
	return m_strDesc;
}

//	Get drop model for shown
const char * CECIvtrElement::GetDropModel()
{
	return m_pDBEssence->file_matter;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECIvtrTossMat
//	
///////////////////////////////////////////////////////////////////////////

CECIvtrTossMat::CECIvtrTossMat(int tid, int expire_date) : CECIvtrItem(tid, expire_date)
{
	m_iCID	= ICID_TOSSMAT;

	//	Get database data
	elementdataman* pDB = g_pGame->GetElementDataMan();
	DATA_TYPE DataType;
	m_pDBEssence = (TOSSMATTER_ESSENCE*)pDB->get_data_ptr(tid, ID_SPACE_ESSENCE, DataType);

	m_iPileLimit	= m_pDBEssence->pile_num_max;
	m_iPrice		= m_pDBEssence->price;
	m_iShopPrice	= m_pDBEssence->shop_price;
	m_iProcType		= m_pDBEssence->proc_type;
	m_i64EquipMask	= 0;
	m_bUseable		= true;
	m_dwUseFlags	= USE_ATKTARGET;

	m_bNeedUpdate	= false;
}

CECIvtrTossMat::CECIvtrTossMat(const CECIvtrTossMat& s) : CECIvtrItem(s)
{
	m_pDBEssence	= s.m_pDBEssence;
}

CECIvtrTossMat::~CECIvtrTossMat()
{
}

//	Set item detail information
bool CECIvtrTossMat::SetItemInfo(BYTE* pInfoData, int iDataLen)
{
	CECIvtrItem::SetItemInfo(pInfoData, iDataLen);
	return true;
}

//	Get item icon file name
const char* CECIvtrTossMat::GetIconFile()
{
	return m_pDBEssence->file_icon;
}

//	Get item name
const wchar_t* CECIvtrTossMat::GetName()
{
	return m_pDBEssence->name;
}

//	Get item description text
const wchar_t* CECIvtrTossMat::GetNormalDesc(bool bRepair)
{
	m_strDesc = _AL("");

	CECStringTab* pDescTab = g_pGame->GetItemDesc();

	int white = ITEMDESC_COL_WHITE;
	int red = ITEMDESC_COL_RED;
	int namecol = DecideNameCol();
	int col;

	if (m_iCount > 1)
		AddDescText(namecol, true, pDescTab->GetWideString(ITEMDESC_NAMENUMBER), GetName(), m_iCount);
	else
		AddDescText(namecol, true, pDescTab->GetWideString(ITEMDESC_NAME), GetName());

	AddIDDescText();

	AddExpireTimeDesc();

	//	Physical damage
	AddDescText(white, true, pDescTab->GetWideString(ITEMDESC_PHYDAMAGE), m_pDBEssence->damage_low, m_pDBEssence->damage_high_min);

	//	Level requirment
	if (m_pDBEssence->require_level)
	{
		col = white;
		AddDescText(col, true, pDescTab->GetWideString(ITEMDESC_LEVELREQ), m_pDBEssence->require_level);
	}

	//	Strength requirment
	if (m_pDBEssence->require_strength)
	{
		col = white;
		AddDescText(col, true, pDescTab->GetWideString(ITEMDESC_STRENGTHREQ), m_pDBEssence->require_strength);
	}
	
	//	Agility requirment
	if (m_pDBEssence->require_agility)
	{
		col = white;
		AddDescText(col, true, pDescTab->GetWideString(ITEMDESC_AGILITYREQ), m_pDBEssence->require_agility);
	}
	
	//	Price
	AddPriceDesc(white, bRepair);

	//	Extend description
	AddExtDescText();

	return m_strDesc;
}

//	Get drop model for shown
const char * CECIvtrTossMat::GetDropModel()
{
	return m_pDBEssence->file_matter;
}


///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECIvtrFirework
//	
///////////////////////////////////////////////////////////////////////////

CECIvtrFirework::CECIvtrFirework(int tid, int expire_date) : CECIvtrItem(tid, expire_date)
{
	m_iCID	= ICID_FIREWORK;

	//	Get database data
	elementdataman* pDB = g_pGame->GetElementDataMan();
	DATA_TYPE DataType;
	m_pDBEssence = (FIREWORKS_ESSENCE*)pDB->get_data_ptr(tid, ID_SPACE_ESSENCE, DataType);

	m_iPileLimit	= m_pDBEssence->pile_num_max;
	m_iPrice		= m_pDBEssence->price;
	m_iShopPrice	= m_pDBEssence->shop_price;
	m_iProcType		= m_pDBEssence->proc_type;
	m_i64EquipMask	= 0;
	m_bUseable		= true;
	m_dwUseFlags	= USE_PERSIST;

	m_bNeedUpdate	= false;
}

CECIvtrFirework::CECIvtrFirework(const CECIvtrFirework& s) : CECIvtrItem(s)
{
	m_pDBEssence	= s.m_pDBEssence;
}

CECIvtrFirework::~CECIvtrFirework()
{
}

//	Set item detail information
bool CECIvtrFirework::SetItemInfo(BYTE* pInfoData, int iDataLen)
{
	CECIvtrItem::SetItemInfo(pInfoData, iDataLen);
	return true;
}

//	Get item icon file name
const char* CECIvtrFirework::GetIconFile()
{
	return m_pDBEssence->file_icon;
}

//	Get item name
const wchar_t* CECIvtrFirework::GetName()
{
	return m_pDBEssence->name;
}

//	Get item cool time
//int CECIvtrFirework::GetCoolTime(int* piMax/* NULL */)
//{
//	CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
//	return pHost ? pHost->GetCoolTime(GP_CT_FIREWORKS, piMax) : 0;
//}

//	Get item description text
const wchar_t* CECIvtrFirework::GetNormalDesc(bool bRepair)
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

	//	Firework level
	if (m_pDBEssence->level)
	{
		m_strDesc += _AL("\\r");
		m_strDesc += pDescTab->GetWideString(ITEMDESC_COL_WHITE);
		for (int i=0; i < m_pDBEssence->level; i++)
			m_strDesc += pDescTab->GetWideString(ITEMDESC_PENTAGON);
	}

	//	Extend description
	AddExtDescText();

	return m_strDesc;
}

//	Get drop model for shown
const char * CECIvtrFirework::GetDropModel()
{
	return m_pDBEssence->file_matter;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECIvtrSkillMat
//	
///////////////////////////////////////////////////////////////////////////

CECIvtrSkillMat::CECIvtrSkillMat(int tid, int expire_date) : CECIvtrItem(tid, expire_date)
{
	m_iCID	= ICID_SKILLMATTER;

	//	Get database data
	elementdataman* pDB = g_pGame->GetElementDataMan();
	DATA_TYPE DataType;
	m_pDBEssence = (SKILLMATTER_ESSENCE*)pDB->get_data_ptr(tid, ID_SPACE_ESSENCE, DataType);

	m_iPileLimit	= m_pDBEssence->pile_num_max;
	m_iPrice		= m_pDBEssence->price;
	m_iShopPrice	= m_pDBEssence->shop_price;
	m_iProcType		= m_pDBEssence->proc_type;
	m_i64EquipMask	= 0;
	m_bUseable		= true;

	m_bNeedUpdate	= false;
}

CECIvtrSkillMat::CECIvtrSkillMat(const CECIvtrSkillMat& s) : CECIvtrItem(s)
{
	m_pDBEssence	= s.m_pDBEssence;
}

CECIvtrSkillMat::~CECIvtrSkillMat()
{
}

//	Set item detail information
bool CECIvtrSkillMat::SetItemInfo(BYTE* pInfoData, int iDataLen)
{
	CECIvtrItem::SetItemInfo(pInfoData, iDataLen);
	return true;
}

//	Get item icon file name
const char* CECIvtrSkillMat::GetIconFile()
{
	return m_pDBEssence->file_icon;
}

//	Get item name
const wchar_t* CECIvtrSkillMat::GetName()
{
	return m_pDBEssence->name;
}

//	Get item cool time
//int CECIvtrSkillMat::GetCoolTime(int* piMax/* NULL */)
//{
//	CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
//	return pHost ? pHost->GetCoolTime(GP_CT_SKILLMATTER, piMax) : 0;
//}

//	Get item description text
const wchar_t* CECIvtrSkillMat::GetNormalDesc(bool bRepair)
{
	m_strDesc = _AL("");

	//	Try to build item description
	CECStringTab* pDescTab = g_pGame->GetItemDesc();
	int red = ITEMDESC_COL_RED;
	int white = ITEMDESC_COL_WHITE;
	int namecol = DecideNameCol();

	if (m_iCount > 1)
		AddDescText(namecol, true, pDescTab->GetWideString(ITEMDESC_NAMENUMBER), GetName(), m_iCount);
	else
		AddDescText(namecol, true, pDescTab->GetWideString(ITEMDESC_NAME), GetName());

	AddIDDescText();

	AddExpireTimeDesc();

	//	Level requirement
	if (m_pDBEssence->level_required)
	{
		int col = white;
		AddDescText(col, true, pDescTab->GetWideString(ITEMDESC_LEVELREQ), m_pDBEssence->level_required);
	}

	//	Price
	AddPriceDesc(white, bRepair);

	//	Extend description
	AddExtDescText();

	return m_strDesc;
}

//	Check item use condition
//bool CECIvtrSkillMat::CheckUseCondition()
//{
//	if (!m_pDBEssence)
//		return false;
//
//	//	Check level requirement
//	if (g_pGame->GetGameRun()->GetHostPlayer()->GetBasicProps().iLevel < m_pDBEssence->level_required)
//		return false;
//
//	return true;
//}

//	Get drop model for shown
const char * CECIvtrSkillMat::GetDropModel()
{
	return m_pDBEssence->file_matter;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECIvtrIncSkillAbility
//	
///////////////////////////////////////////////////////////////////////////

CECIvtrIncSkillAbility::CECIvtrIncSkillAbility(int tid, int expire_date) : CECIvtrItem(tid, expire_date)
{
	m_iCID	= ICID_INCSKILLABILITY;

	//	Get database data
	elementdataman* pDB = g_pGame->GetElementDataMan();
	DATA_TYPE DataType;
	m_pDBEssence = (INC_SKILL_ABILITY_ESSENCE*)pDB->get_data_ptr(tid, ID_SPACE_ESSENCE, DataType);

	m_iPileLimit	= m_pDBEssence->pile_num_max;
	m_iPrice		= m_pDBEssence->price;
	m_iShopPrice	= m_pDBEssence->shop_price;
	m_iProcType		= m_pDBEssence->proc_type;
	m_i64EquipMask	= 0;
	m_bUseable		= true;

	m_bNeedUpdate	= false;
}

CECIvtrIncSkillAbility::CECIvtrIncSkillAbility(const CECIvtrIncSkillAbility& s) : CECIvtrItem(s)
{
	m_pDBEssence	= s.m_pDBEssence;
}

CECIvtrIncSkillAbility::~CECIvtrIncSkillAbility()
{
}

//	Set item detail information
bool CECIvtrIncSkillAbility::SetItemInfo(BYTE* pInfoData, int iDataLen)
{
	CECIvtrItem::SetItemInfo(pInfoData, iDataLen);
	return true;
}

//	Get item icon file name
const char* CECIvtrIncSkillAbility::GetIconFile()
{
	return m_pDBEssence->file_icon;
}

//	Get item name
const wchar_t* CECIvtrIncSkillAbility::GetName()
{
	return m_pDBEssence->name;
}

//	Get item cool time
//int CECIvtrIncSkillAbility::GetCoolTime(int* piMax/* NULL */)
//{
//	return CECIvtrItem::GetCoolTime(piMax);
// }

//	Get item description text
const wchar_t* CECIvtrIncSkillAbility::GetNormalDesc(bool bRepair)
{
	m_strDesc = _AL("");

	//	Try to build item description
	CECStringTab* pDescTab = g_pGame->GetItemDesc();
	int red = ITEMDESC_COL_RED;
	int white = ITEMDESC_COL_WHITE;
	int namecol = DecideNameCol();

	if (m_iCount > 1)
		AddDescText(namecol, true, pDescTab->GetWideString(ITEMDESC_NAMENUMBER), GetName(), m_iCount);
	else
		AddDescText(namecol, true, pDescTab->GetWideString(ITEMDESC_NAME), GetName());

	AddIDDescText();

	AddExpireTimeDesc();

	//	Level requirement
	if (m_pDBEssence->level_required)
	{
		int col = white;
		AddDescText(col, true, pDescTab->GetWideString(ITEMDESC_INC_SKILL_ABILITY_REQUIRE_LEVEL), m_pDBEssence->level_required);
	}

	//  Ratio increased
	AddDescText(white, true, pDescTab->GetWideString(ITEMDESC_INC_SKILL_ABILITY_RATIO), (int)ceil(m_pDBEssence->inc_ratio*100));

	//	Price
	AddPriceDesc(white, bRepair);

	//	Extend description
	AddExtDescText();

	return m_strDesc;
}

//	Check item use condition
//bool CECIvtrIncSkillAbility::CheckUseCondition()
//{
//	if (!m_pDBEssence)
//		return false;
//	
//	CECSkill *pSkill = g_pGame->GetGameRun()->GetHostPlayer()->GetSkill(m_pDBEssence->id_skill);
//	if (!pSkill)
//		return false;
//
//	return true;
//}

//	Get drop model for shown
const char * CECIvtrIncSkillAbility::GetDropModel()
{
	return m_pDBEssence->file_matter;
}


///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECIvtrDoubleExp
//	
///////////////////////////////////////////////////////////////////////////

CECIvtrDoubleExp::CECIvtrDoubleExp(int tid, int expire_date) : CECIvtrItem(tid, expire_date)
{
	m_iCID	= ICID_DOUBLEEXP;

	//	Get database data
	elementdataman* pDB = g_pGame->GetElementDataMan();
	DATA_TYPE DataType;
	m_pDBEssence = (DOUBLE_EXP_ESSENCE*)pDB->get_data_ptr(tid, ID_SPACE_ESSENCE, DataType);

	m_iPileLimit	= m_pDBEssence->pile_num_max;
	m_iPrice		= m_pDBEssence->price;
	m_iShopPrice	= m_pDBEssence->shop_price;
	m_iProcType		= m_pDBEssence->proc_type;
	m_i64EquipMask	= 0;
	m_bUseable		= true;

	m_bNeedUpdate	= false;
}

CECIvtrDoubleExp::CECIvtrDoubleExp(const CECIvtrDoubleExp& s) : CECIvtrItem(s)
{
	m_pDBEssence	= s.m_pDBEssence;
}

CECIvtrDoubleExp::~CECIvtrDoubleExp()
{
}

//	Set item detail information
bool CECIvtrDoubleExp::SetItemInfo(BYTE* pInfoData, int iDataLen)
{
	CECIvtrItem::SetItemInfo(pInfoData, iDataLen);
	return true;
}

//	Get item icon file name
const char* CECIvtrDoubleExp::GetIconFile()
{
	return m_pDBEssence->file_icon;
}

//	Get item name
const wchar_t* CECIvtrDoubleExp::GetName()
{
	return m_pDBEssence->name;
}

//	Get item cool time
//int CECIvtrDoubleExp::GetCoolTime(int* piMax/* NULL */)
//{
//	CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
//	return pHost ? pHost->GetCoolTime(GP_CT_DOUBLEEXP, piMax) : 0;
//}

//	Get item description text
const wchar_t* CECIvtrDoubleExp::GetNormalDesc(bool bRepair)
{
	m_strDesc = _AL("");

	//	Try to build item description
	CECStringTab* pDescTab = g_pGame->GetItemDesc();
	int red = ITEMDESC_COL_RED;
	int white = ITEMDESC_COL_WHITE;
	int namecol = DecideNameCol();

	if (m_iCount > 1)
		AddDescText(namecol, true, pDescTab->GetWideString(ITEMDESC_NAMENUMBER), GetName(), m_iCount);
	else
		AddDescText(namecol, true, pDescTab->GetWideString(ITEMDESC_NAME), GetName());

	AddIDDescText();

	AddExpireTimeDesc();

	//AddDescText(white, true, pDescTab->GetWideString(ITEMDESC_DOUBLEEXP_TIME), m_pDBEssence->double_exp_time / 3600);

	//	Price
	AddPriceDesc(white, bRepair);

	//	Extend description
	AddExtDescText();

	return m_strDesc;
}

//	Check item use condition
//bool CECIvtrDoubleExp::CheckUseCondition()
//{
//	if (!m_pDBEssence)
//		return false;
//
//	return true;
// }

//	Get drop model for shown
const char * CECIvtrDoubleExp::GetDropModel()
{
	return m_pDBEssence->file_matter;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECIvtrTransmitScroll
//	
///////////////////////////////////////////////////////////////////////////

CECIvtrTransmitScroll::CECIvtrTransmitScroll(int tid, int expire_date) : CECIvtrItem(tid, expire_date)
{
	m_iCID	= ICID_TRANSMITSCROLL;

	//	Get database data
	elementdataman* pDB = g_pGame->GetElementDataMan();
	DATA_TYPE DataType;
	m_pDBEssence = (TRANSMITSCROLL_ESSENCE*)pDB->get_data_ptr(tid, ID_SPACE_ESSENCE, DataType);

	m_iPileLimit	= m_pDBEssence->pile_num_max;
	m_iPrice		= m_pDBEssence->price;
	m_iShopPrice	= m_pDBEssence->shop_price;
	m_iProcType		= m_pDBEssence->proc_type;
	m_i64EquipMask	= 0;
	m_bUseable		= true;
	m_dwUseFlags	= USE_PERSIST;

	m_bNeedUpdate	= false;
}

CECIvtrTransmitScroll::CECIvtrTransmitScroll(const CECIvtrTransmitScroll& s) : CECIvtrItem(s)
{
	m_pDBEssence	= s.m_pDBEssence;
}

CECIvtrTransmitScroll::~CECIvtrTransmitScroll()
{
}

//	Set item detail information
bool CECIvtrTransmitScroll::SetItemInfo(BYTE* pInfoData, int iDataLen)
{
	CECIvtrItem::SetItemInfo(pInfoData, iDataLen);
	return true;
}

//	Get item icon file name
const char* CECIvtrTransmitScroll::GetIconFile()
{
	return m_pDBEssence->file_icon;
}

//	Get item name
const wchar_t* CECIvtrTransmitScroll::GetName()
{
	return m_pDBEssence->name;
}

//	Get item cool time
//int CECIvtrTransmitScroll::GetCoolTime(int* piMax/* NULL */)
//{
//	CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
//	return pHost ? pHost->GetCoolTime(GP_CT_TOWNSCROLL, piMax) : 0;
//}

//	Get item description text
const wchar_t* CECIvtrTransmitScroll::GetNormalDesc(bool bRepair)
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
const char * CECIvtrTransmitScroll::GetDropModel()
{
	return m_pDBEssence->file_matter;
}

//	Check item use condition
//bool CECIvtrTransmitScroll::CheckUseCondition()
//{
//	if( g_pGame->GetGameRun()->GetHostPlayer()->IsFighting() )
//		return false;
//
//	return true;
//}


///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECIvtrDyeTicket
//	
///////////////////////////////////////////////////////////////////////////

CECIvtrDyeTicket::CECIvtrDyeTicket(int tid, int expire_date) : CECIvtrItem(tid, expire_date)
{
	m_iCID	= ICID_DYETICKET;

	//	Get database data
	elementdataman* pDB = g_pGame->GetElementDataMan();
	DATA_TYPE DataType;
	m_pDBEssence = (DYE_TICKET_ESSENCE*)pDB->get_data_ptr(tid, ID_SPACE_ESSENCE, DataType);

	m_iPileLimit	= m_pDBEssence->pile_num_max;
	m_iPrice		= m_pDBEssence->price;
	m_iShopPrice	= m_pDBEssence->shop_price;
	m_iProcType		= m_pDBEssence->proc_type;
	m_i64EquipMask	= 0;
	m_bUseable		= true;

	m_bNeedUpdate	= false;
}

CECIvtrDyeTicket::CECIvtrDyeTicket(const CECIvtrDyeTicket& s) : CECIvtrItem(s)
{
	m_pDBEssence	= s.m_pDBEssence;
}

CECIvtrDyeTicket::~CECIvtrDyeTicket()
{
}

//	Set item detail information
bool CECIvtrDyeTicket::SetItemInfo(BYTE* pInfoData, int iDataLen)
{
	CECIvtrItem::SetItemInfo(pInfoData, iDataLen);
	return true;
}

//	Get item icon file name
const char* CECIvtrDyeTicket::GetIconFile()
{
	return m_pDBEssence->file_icon;
}

//	Get item name
const wchar_t* CECIvtrDyeTicket::GetName()
{
	return m_pDBEssence->name;
}

//	Get item cool time
//int CECIvtrDyeTicket::GetCoolTime(int* piMax/* NULL */)
//{
//	CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
//	return pHost ? pHost->GetCoolTime(GP_CT_DYETICKET, piMax) : 0;
//}

//	Get item description text
const wchar_t* CECIvtrDyeTicket::GetNormalDesc(bool bRepair)
{
	m_strDesc = _AL("");

	//	Try to build item description
	CECStringTab* pDescTab = g_pGame->GetItemDesc();
	int red = ITEMDESC_COL_RED;
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

//	Check item use condition
//bool CECIvtrDyeTicket::CheckUseCondition()
//{
//	if (!m_pDBEssence)
//		return false;
//
//	return true;
// }

//	Get drop model for shown
const char * CECIvtrDyeTicket::GetDropModel()
{
	return m_pDBEssence->file_matter;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECIvtrTargetItem
//	
///////////////////////////////////////////////////////////////////////////

CECIvtrTargetItem::CECIvtrTargetItem(int tid, int expire_date) : CECIvtrItem(tid, expire_date)
{
	m_iCID	= ICID_TARGETITEM;

	//	Get database data
	elementdataman* pDB = g_pGame->GetElementDataMan();
	DATA_TYPE DataType;
	m_pDBEssence = (TARGET_ITEM_ESSENCE*)pDB->get_data_ptr(tid, ID_SPACE_ESSENCE, DataType);

	m_iPileLimit	= m_pDBEssence->pile_num_max;
	m_iPrice		= m_pDBEssence->price;
	m_iShopPrice	= m_pDBEssence->shop_price;
	m_iProcType		= m_pDBEssence->proc_type;
	m_i64EquipMask	= 0;
	m_bUseable		= true;
	m_dwUseFlags	= USE_TARGET;

	// Init skill
	m_pTargetSkill = new CECSkill(m_pDBEssence->id_skill, m_pDBEssence->skill_level);
	if(!m_pTargetSkill)
	{
		ASSERT(0);
	}

	m_bNeedUpdate	= false;
}

CECIvtrTargetItem::CECIvtrTargetItem(const CECIvtrTargetItem& s) : CECIvtrItem(s)
{
	m_pDBEssence	= s.m_pDBEssence;

	// Init skill
	m_pTargetSkill  = new CECSkill(s.m_pTargetSkill->GetSkillID(), s.m_pTargetSkill->GetSkillLevel());

	if(!m_pTargetSkill)
	{
		ASSERT(0);
	}
}

CECIvtrTargetItem::~CECIvtrTargetItem()
{
	if(m_pTargetSkill)
	{
		delete m_pTargetSkill;
		m_pTargetSkill = NULL;
	}
}

//	Set item detail information
bool CECIvtrTargetItem::SetItemInfo(BYTE* pInfoData, int iDataLen)
{
	CECIvtrItem::SetItemInfo(pInfoData, iDataLen);
	return true;
}

//	Get item icon file name
const char* CECIvtrTargetItem::GetIconFile()
{
	return m_pDBEssence->file_icon;
}

//	Get item name
const wchar_t* CECIvtrTargetItem::GetName()
{
	return m_pDBEssence->name;
}

//	Get item cool time
//int CECIvtrTargetItem::GetCoolTime(int* piMax/* NULL */)
//{
//	CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
//	if(!pHost)
//	{
//		ASSERT(0);
//		return 0;
//	}
//
//	int i,aCoolTimes[10], iMaxTime = 0, index = -1;
//	for(i=0;i<10;i++)
//			aCoolTimes[i] = 0;
//
//	if(m_pTargetSkill)
//	{
//		CECSkill* pSkill = pHost->GetSkill(m_pTargetSkill->GetSkillID());
//		if(pSkill)
//		{
//			iMaxTime = pSkill->GetCoolingCnt();
//			if(piMax)
//				*piMax = pSkill->GetCoolingTime();
//		}
//
//		for(i=0;i<10;i++)
//		{
//			if(m_pTargetSkill->GetCommonCoolDown() & (1<<i))
//			{
//				aCoolTimes[i] = pHost->GetCoolTime(GP_CT_SKILLCOMMONCOOLDOWN0+i);
//
//				if( aCoolTimes[i] > iMaxTime)
//				{
//					iMaxTime = aCoolTimes[i];
//					index = i;
//				}
//			}
//		}
//	}
//
//	if(index == -1)
//	{
//		if(m_pTargetSkill && piMax)
//		{
//			*piMax = m_pTargetSkill->GetCoolingTime();
//		}
//
//		return iMaxTime;
//	}
//
//	return pHost->GetCoolTime(GP_CT_SKILLCOMMONCOOLDOWN0+index, piMax);
//}

//	Get item description text
const wchar_t* CECIvtrTargetItem::GetNormalDesc(bool bRepair)
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
	
	//	Level requirment
	if (int iLevelReq = m_pDBEssence->require_level){
		int col = white;
		AddDescText(col, true, pDescTab->GetWideString(ITEMDESC_LEVELREQ), iLevelReq);
	}

// 	//  Type
// 	int iCoolDown = m_pTargetSkill->GetCommonCoolDown();
// 	if(iCoolDown & 0x01)
// 		AddDescText(white, true, pDescTab->GetWideString(ITEMDESC_2STRINGS), 
// 					pDescTab->GetWideString(ITEMDESC_TYPE), pDescTab->GetWideString(ITEMDESC_ATTACK_ITEM));
// 	else if(iCoolDown & 0x02)
// 		AddDescText(white, true, pDescTab->GetWideString(ITEMDESC_2STRINGS), 
// 					pDescTab->GetWideString(ITEMDESC_TYPE), pDescTab->GetWideString(ITEMDESC_DEFENCE_ITEM));
// 	else
// 		AddDescText(white, true, pDescTab->GetWideString(ITEMDESC_2STRINGS), 
// 					pDescTab->GetWideString(ITEMDESC_TYPE), pDescTab->GetWideString(ITEMDESC_BATTLE_ITEM));

	//	Price
	AddPriceDesc(white, bRepair);

	//  Use region
	m_strDesc += _AL("\\r");
	if(m_pDBEssence->num_area != 0)
	{
		AddDescText(white, false, pDescTab->GetWideString(ITEMDESC_USE_REGION));
		CECInstance* pInstance;
		for(int i=0;i<m_pDBEssence->num_area;i++)
		{
			pInstance = g_pGame->GetInstance(m_pDBEssence->area_id[i]);
			if (pInstance)
			{
				m_strDesc += _AL(" ");
				AddDescText(white, false, pInstance->GetName());
			}
		}

		m_strDesc += _AL("\\r");
	}

	//  Consume count
	if(m_pDBEssence->num_use_pertime != 0)
		AddDescText(white, true, pDescTab->GetWideString(ITEMDESC_CONSUME_COUNT), m_pDBEssence->num_use_pertime);

// 	//  Use effect
// 	const wchar_t* szSkillName = g_pGame->GetSkillDesc()->GetWideString(m_pTargetSkill->GetSkillID() * 10);
// 	AddDescText(white, true, pDescTab->GetWideString(ITEMDESC_USE_EFFECT), m_pTargetSkill->GetSkillLevel(), szSkillName ? szSkillName : _AL(""));	

	//  Skill desc
// 	AddDescText(white, true, pDescTab->GetWideString(ITEMDESC_SKILL_EFFECT));
	m_strDesc += m_pTargetSkill->GetDesc();

	//  Special desc
	if(m_pDBEssence->use_in_combat == 0)
	{
		m_strDesc += _AL("\\r");
		AddDescText(white, false, pDescTab->GetWideString(ITEMDESC_CANNOT_USE_IN_BATTLE));
	}
	
	if(m_pDBEssence->use_in_sanctuary_only)
	{
		m_strDesc += _AL("\\r");
		AddDescText(white, false, pDescTab->GetWideString(ITEMDESC_USE_IN_SANCTUARY_ONLY));
	}

// 	AddDescText(white, false, pDescTab->GetWideString(ITEMDESC_CONSUME_ANYWAY));

	//	Extend description
	AddExtDescText();

	return m_strDesc;
}

//	Get drop model for shown
const char * CECIvtrTargetItem::GetDropModel()
{
	return m_pDBEssence->file_matter;
}

//	Check item use condition
//bool CECIvtrTargetItem::CheckUseCondition()
//{
//	if( !m_pTargetSkill )
//		return false;
//
//	return true;
// }
///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECIvtrLookInfoItem
//	
///////////////////////////////////////////////////////////////////////////

CECIvtrLookInfoItem::CECIvtrLookInfoItem(int tid, int expire_date) : CECIvtrItem(tid, expire_date)
{
	m_iCID	= ICID_LOOKINFOITEM;

	//	Get database data
	elementdataman* pDB = g_pGame->GetElementDataMan();
	DATA_TYPE DataType;
	m_pDBEssence = (LOOK_INFO_ESSENCE*)pDB->get_data_ptr(tid, ID_SPACE_ESSENCE, DataType);

	m_iPileLimit	= m_pDBEssence->pile_num_max;
	m_iPrice		= m_pDBEssence->price;
	m_iShopPrice	= m_pDBEssence->shop_price;
	m_iProcType		= m_pDBEssence->proc_type;
	m_i64EquipMask	= 0;
	m_bUseable		= true;
	m_dwUseFlags	= USE_TARGET;

	m_bNeedUpdate	= false;
}

CECIvtrLookInfoItem::CECIvtrLookInfoItem(const CECIvtrLookInfoItem& s) : CECIvtrItem(s)
{
	m_pDBEssence	= s.m_pDBEssence;
}

CECIvtrLookInfoItem::~CECIvtrLookInfoItem()
{
}

//	Set item detail information
bool CECIvtrLookInfoItem::SetItemInfo(BYTE* pInfoData, int iDataLen)
{
	CECIvtrItem::SetItemInfo(pInfoData, iDataLen);
	return true;
}

//	Get item icon file name
const char* CECIvtrLookInfoItem::GetIconFile()
{
	return m_pDBEssence->file_icon;
}

//	Get item name
const wchar_t* CECIvtrLookInfoItem::GetName()
{
	return m_pDBEssence->name;
}

//	Get item cool time
//int CECIvtrLookInfoItem::GetCoolTime(int* piMax/* NULL */)
//{
//	CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
//	return pHost ? pHost->GetCoolTime(GP_CT_QUERY_OTHER_PROPERTY, piMax) : 0;
//}

//	Get item description text
const wchar_t* CECIvtrLookInfoItem::GetNormalDesc(bool bRepair)
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
const char * CECIvtrLookInfoItem::GetDropModel()
{
	return m_pDBEssence->file_matter;
}


///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECIvtrWeddingBookCard
//	
///////////////////////////////////////////////////////////////////////////

CECIvtrWeddingBookCard::CECIvtrWeddingBookCard(int tid, int expire_date) : CECIvtrItem(tid, expire_date)
{
	m_iCID	= ICID_WEDDINGBOOKCARD;

	//	Get database data
	elementdataman* pDB = g_pGame->GetElementDataMan();
	DATA_TYPE DataType;
	m_pDBEssence = (WEDDING_BOOKCARD_ESSENCE*)pDB->get_data_ptr(tid, ID_SPACE_ESSENCE, DataType);

	m_iPileLimit	= m_pDBEssence->pile_num_max;
	m_iPrice		= m_pDBEssence->price;
	m_iShopPrice	= m_pDBEssence->shop_price;
	m_iProcType		= m_pDBEssence->proc_type;
	m_i64EquipMask	= 0;

	m_Essence.year = m_pDBEssence->year;
	m_Essence.month = m_pDBEssence->month;
	m_Essence.day = m_pDBEssence->day;
}

CECIvtrWeddingBookCard::CECIvtrWeddingBookCard(const CECIvtrWeddingBookCard& s) : CECIvtrItem(s)
{
	m_pDBEssence	= s.m_pDBEssence;
	m_Essence		= s.m_Essence;
}

CECIvtrWeddingBookCard::~CECIvtrWeddingBookCard()
{
}

//	Set item detail information
bool CECIvtrWeddingBookCard::SetItemInfo(BYTE* pInfoData, int iDataLen)
{
	CECIvtrItem::SetItemInfo(pInfoData, iDataLen);
	
	if (!pInfoData || !iDataLen)
		return true;

	try
	{
		CECDataReader dr(pInfoData, iDataLen);

		m_Essence.year = dr.Read_int();
		m_Essence.month = dr.Read_int();
		m_Essence.day = dr.Read_int();
	}
	catch (CECException& e)
	{
		ASSERT(0);
		a_LogOutput(1, "CECIvtrWeddingBookCard::SetItemInfo, data read error (%d)", e.GetType());
		return false;
	}

	return true;
}

//	Get item icon file name
const char* CECIvtrWeddingBookCard::GetIconFile()
{
	return m_pDBEssence->file_icon;
}

//	Get item name
const wchar_t* CECIvtrWeddingBookCard::GetName()
{
	return m_pDBEssence->name;
}

//	Get item description text
const wchar_t* CECIvtrWeddingBookCard::GetNormalDesc(bool bRepair)
{
	if (m_bNeedUpdate)
		return NULL;

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

	// 结婚日期
	if (m_Essence.year>0 && m_Essence.month>0 && m_Essence.day>0)
	{
		AddDescText(ITEMDESC_COL_RED, true, pDescTab->GetWideString(ITEMDESC_WEDDING_BOOKCARD), m_Essence.year, m_Essence.month, m_Essence.day);
	}

	//	Price
	AddPriceDesc(white, bRepair);

	//	Extend description
	AddExtDescText();

	return m_strDesc;
}

//	Get drop model for shown
const char * CECIvtrWeddingBookCard::GetDropModel()
{
	return m_pDBEssence->file_matter;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECIvtrWeddingInviteCard
//	
///////////////////////////////////////////////////////////////////////////

CECIvtrWeddingInviteCard::CECIvtrWeddingInviteCard(int tid, int expire_date) : CECIvtrItem(tid, expire_date)
{
	m_iCID	= ICID_WEDDINGINVITECARD;

	//	Get database data
	elementdataman* pDB = g_pGame->GetElementDataMan();
	DATA_TYPE DataType;
	m_pDBEssence = (WEDDING_INVITECARD_ESSENCE*)pDB->get_data_ptr(tid, ID_SPACE_ESSENCE, DataType);

	m_iPileLimit	= m_pDBEssence->pile_num_max;
	m_iPrice		= m_pDBEssence->price;
	m_iShopPrice	= m_pDBEssence->shop_price;
	m_iProcType		= m_pDBEssence->proc_type;
	m_i64EquipMask	= 0;
	m_bUseable		= true;
}

CECIvtrWeddingInviteCard::CECIvtrWeddingInviteCard(const CECIvtrWeddingInviteCard& s) : CECIvtrItem(s)
{
	m_pDBEssence	=	s.m_pDBEssence;
	m_Essence		=	s.m_Essence;
}

CECIvtrWeddingInviteCard::~CECIvtrWeddingInviteCard()
{
}

//	Set item detail information
bool CECIvtrWeddingInviteCard::SetItemInfo(BYTE* pInfoData, int iDataLen)
{
	CECIvtrItem::SetItemInfo(pInfoData, iDataLen);
	
	if (!pInfoData || !iDataLen)
		return true;

	try
	{
		CECDataReader dr(pInfoData, iDataLen);

		m_Essence = *(IVTR_ESSENCE_WEDDING_INVITECARD *)dr.Read_Data(sizeof(m_Essence));

		// 查找玩家名称
// 		abase::vector<int> UnknownIDs;
// 
// 		CECGameRun *pGameRun = g_pGame->GetGameRun();
// 		CECHostPlayer *pHost = pGameRun->GetHostPlayer();
// 		int id = 0;
// 
// 		id = m_Essence.groom;
// 		if (id>0 && id!=pHost->GetCharacterID() && !pGameRun->GetPlayerName(id, false))
// 			UnknownIDs.push_back(id);
// 
// 		id = m_Essence.bride;
// 		if (id>0 && id!=pHost->GetCharacterID() && !pGameRun->GetPlayerName(id, false))
// 			UnknownIDs.push_back(id);
// 
// 		id = m_Essence.invitee;
// 		if (id>0 && id!=pHost->GetCharacterID() && !pGameRun->GetPlayerName(id, false))
// 			UnknownIDs.push_back(id);
// 
// 		if (!UnknownIDs.empty())
// 		{
// 			// 查询未知玩家名称
// 			g_pGame->GetGameSession()->GetPlayerBriefInfo((int)UnknownIDs.size(), &UnknownIDs[0], 2);
// 		}
	}
	catch (CECException& e)
	{
		ASSERT(0);
		a_LogOutput(1, "CECIvtrWeddingInviteCard::SetItemInfo, data read error (%d)", e.GetType());
		return false;
	}

	return true;
}

//	Get item icon file name
const char* CECIvtrWeddingInviteCard::GetIconFile()
{
	return m_pDBEssence->file_icon;
}

//	Get item name
const wchar_t* CECIvtrWeddingInviteCard::GetName()
{
	return m_pDBEssence->name;
}

//	Get item description text
const wchar_t* CECIvtrWeddingInviteCard::GetNormalDesc(bool bRepair)
{
	if (m_bNeedUpdate)
		return NULL;

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

	// 填写请柬内容
// 	while (true)
// 	{
// 		//
// 		CECGameRun *pGameRun = g_pGame->GetGameRun();
// 		
// 		// 请柬中的玩家名称
// 		ACString strGroomName = pGameRun->GetPlayerName(m_Essence.groom, false);
// 		ACString strBrideName = pGameRun->GetPlayerName(m_Essence.bride, false);
// 		if (strGroomName.IsEmpty() ||
// 			strBrideName.IsEmpty())
// 			break;
// 		
// 		ACString strInviteeName;
// 		if (m_Essence.invitee > 0)
// 		{
// 			strInviteeName = pGameRun->GetPlayerName(m_Essence.invitee, false);
// 			if (strInviteeName.IsEmpty())
// 				break;
// 		}
// 		
// 		// 婚礼地点
// 		elementdataman* pDB = g_pGame->GetElementDataMan();
// 		DATA_TYPE DataType;
// 		WEDDING_CONFIG *pWeddingConfig = (WEDDING_CONFIG*)pDB->get_data_ptr(WEDDING_CONFIG_ID, ID_SPACE_CONFIG, DataType);
// 		if (!pWeddingConfig || DataType!=DT_WEDDING_CONFIG)
// 			break;
// 
// 		if (m_Essence.scene<0 ||
// 			m_Essence.scene>=sizeof(pWeddingConfig->wedding_scene)/sizeof(pWeddingConfig->wedding_scene[0]) ||
// 			!pWeddingConfig->wedding_scene[m_Essence.scene].name[0])
// 			break;
// 
// 		// 婚礼时间
// 		int timeBias = g_pGame->GetTimeZoneBias() * 60;
// 		int localTime = m_Essence.start_time - timeBias;
// 		tm t = *gmtime((time_t*)&localTime);
// 		
// 		// 添加请柬内容
// 		int id = g_pGame->GetGameRun()->GetHostPlayer()->GetCharacterID();
// 		AddDescText(ITEMDESC_COL_RED, true,
// 			pDescTab->GetWideString(ITEMDESC_WEDDING_INVITECARD),
// 			t.tm_year + 1900,
// 			t.tm_mon + 1,
// 			t.tm_mday,
// 			t.tm_hour,
// 			t.tm_min,
// 			pWeddingConfig->wedding_scene[m_Essence.scene].name,
// 			strGroomName, strBrideName, strInviteeName);
// 
// 		break;
// 	}

	//	Price
	AddPriceDesc(white, bRepair);

	//	Extend description
	AddExtDescText();

	return m_strDesc;
}

//	Get drop model for shown
const char * CECIvtrWeddingInviteCard::GetDropModel()
{
	return m_pDBEssence->file_matter;
}

//	请柬当前是否可使用
// bool CECIvtrWeddingInviteCard::CheckUseCondition()
// {
// 	// 请柬只能由新郎新娘指定的玩家使用
// 	return !m_bNeedUpdate				//	数据已经从服务器得到更新
// 		&& IsUseable()
// 		&& (m_Essence.invitee == 0
// 		|| m_Essence.invitee == g_pGame->GetGameRun()->GetHostPlayer()->GetCharacterID());
// }


///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECIvtrShapener
//	
///////////////////////////////////////////////////////////////////////////

CECIvtrShapener::CECIvtrShapener(int tid, int expire_date) : CECIvtrItem(tid, expire_date)
{
	m_iCID	= ICID_SHARPENER;

	//	Get database data
	elementdataman* pDB = g_pGame->GetElementDataMan();
	DATA_TYPE DataType;
	m_pDBEssence = (SHARPENER_ESSENCE*)pDB->get_data_ptr(tid, ID_SPACE_ESSENCE, DataType);

	m_iPileLimit	= m_pDBEssence->pile_num_max;
	m_iPrice		= m_pDBEssence->price;
	m_iShopPrice	= m_pDBEssence->shop_price;
	m_iProcType		= m_pDBEssence->proc_type;
	m_i64EquipMask	= 0;
	m_bUseable		= true;
}

CECIvtrShapener::CECIvtrShapener(const CECIvtrShapener& s) : CECIvtrItem(s)
{
	m_pDBEssence	=	s.m_pDBEssence;
}

CECIvtrShapener::~CECIvtrShapener()
{
}

//	Set item detail information
bool CECIvtrShapener::SetItemInfo(BYTE* pInfoData, int iDataLen)
{
	CECIvtrItem::SetItemInfo(pInfoData, iDataLen);
	return true;
}

//	Get item icon file name
const char* CECIvtrShapener::GetIconFile()
{
	return m_pDBEssence->file_icon;
}

//	Get item name
const wchar_t* CECIvtrShapener::GetName()
{
	return m_pDBEssence->name;
}

//	Get drop model for shown
const char * CECIvtrShapener::GetDropModel()
{
	return m_pDBEssence->file_matter;
}

int CECIvtrShapener::GetLevel()const
{
	return m_pDBEssence ? m_pDBEssence->level : 0;
}

int CECIvtrShapener::GetItemLevel()const
{
	return GetLevel();
}


//	Get item description text
const wchar_t* CECIvtrShapener::GetNormalDesc(bool bRepair)
{
	if (m_bNeedUpdate)
		return NULL;

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

	//	level
	AddDescText(white, true, pDescTab->GetWideString(ITEMDESC_LEVEL), GetDBEssence()->level);

	//	Price
	AddPriceDesc(white, bRepair);

	//	将为武器增加的附加属性
	AddSharpenerDesc();

	//	Extend description
	AddExtDescText();

	return m_strDesc;
}

void CECIvtrShapener::AddSharpenerDesc()
{
	if (!m_pDBEssence)
		return;

	bool bFirst(true);
	int  nValidAddon(0);

	CECStringTab* pDescTab = g_pGame->GetItemDesc();
	int color = ITEMDESC_COL_LIGHTBLUE;
	CECGame::ItemExtPropTable& PropTab = g_pGame->GetItemExtPropTable();
	elementdataman* pDataMan = g_pGame->GetElementDataMan();
	for (int i = 0; i < sizeof(m_pDBEssence->addon)/sizeof(m_pDBEssence->addon[0]); ++ i)
	{
		int idAddon = m_pDBEssence->addon[i];
		CECGame::ItemExtPropTable::pair_type Pair = PropTab.get(idAddon);
		BYTE byPropType = Pair.second ? *Pair.first : 0xff;

		if (!IsSharpenerProperty(byPropType))
			continue;

		DATA_TYPE DataType = DT_INVALID;
		const void* pData = pDataMan->get_data_ptr(idAddon, ID_SPACE_ADDON, DataType);
		if (DataType != DT_EQUIPMENT_ADDON || !pData)
		{
			ASSERT(false);
			continue;
		}
		const EQUIPMENT_ADDON* pAddon = (const EQUIPMENT_ADDON*)pData;
		if (pAddon->num_params <= 0)
		{
			ASSERT(false);
			continue;
		}

		++ nValidAddon;
		if (bFirst)
		{
			// 添加第一个属性时换行
			m_strDesc += _AL("\\r\\r");
			bFirst = false;
		}

		int p0 = pAddon->param1;

		//	增加属性
		switch(byPropType)
		{
		case 100:	//	磨刀石物理攻击
			AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_ADDPHYDAMAGE));
			AddDescText(color, true, _AL(" %+d"), p0);
			break;

		case 101:	//	磨刀石物理攻击上限
			AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_MAXPHYDAMAGE), p0);
			break;

		case 102:	//	磨刀石魔法攻击
			AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_ADDMAGICDAMAGE));
			AddDescText(color, true, _AL(" %+d"), p0);
			break;

		case 103:	//	磨刀石魔法攻击上限
			AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_MAXMAGICDAMAGE), p0);
			break;

		case 104:	//	磨刀石物理防御
			AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_PHYDEFENCE));
			AddDescText(color, true, _AL(" %+d"), p0);
			break;

		case 105:	//	磨刀石HP
			AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_ADDHP));
			AddDescText(color, true, _AL(" %+d"), p0);
			break;

		case 106:	//	磨刀石力量
			AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_STRENGTH), p0);
			break;

		case 107:	//	磨刀石敏捷
			AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_AGILITY), p0);
			break;

		case 108:	//	磨刀石灵力
			AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_ENERGY), p0);
			break;

		case 109:	//	磨刀石命中率
			AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_ATKRATING), p0);
			break;

		case 110:	//	磨刀石致命一击率
			AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_DEADLYSTRIKE), VisualizeFloatPercent(p0));
			break;

		case 111:	//	磨刀石攻击等级
			AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_ATK_DEGREE), p0);
			break;

		case 112:	//	磨刀石防御等级
			AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_DEF_DEGREE), p0);
			break;

		case 113:	//	磨刀石吟唱(%d)
			AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_CASTTIME), -VisualizeFloatPercent(p0));
			break;

		case 114:	//	磨刀石魔法防御
			AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_ALLMAGICDEF));
			AddDescText(color, true, _AL(" %+d"), p0);
			break;

		default:
			ASSERT(false);
			continue;
		}
	}
	
	//	增加属性有效时间
	if (nValidAddon > 0 && m_pDBEssence->addon_time > 0)
	{
		int timeLeft = m_pDBEssence->addon_time;
		if (timeLeft > 24 * 3600)
			AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_LASTTIME_DAY), timeLeft / (24 * 3600), (timeLeft % (24 * 3600)) / 3600);
		else if( timeLeft > 3600 )
			AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_LASTTIME_HOUR_MIN), timeLeft / 3600, (timeLeft % 3600) / 60);
		else if( timeLeft > 60 )
			AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_LASTTIME_MIN_SEC), timeLeft / 60, timeLeft % 60);
		else
			AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_LASTTIME_SECOND), timeLeft);
	}
}



///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECIvtrFactionMaterial
//	
///////////////////////////////////////////////////////////////////////////

CECIvtrFactionMaterial::CECIvtrFactionMaterial(int tid, int expire_date) : CECIvtrItem(tid, expire_date)
{
	m_iCID	= ICID_FACTIONMATERIAL;

	//	Get database data
	elementdataman* pDB = g_pGame->GetElementDataMan();
	DATA_TYPE DataType;
	m_pDBEssence = (FACTION_MATERIAL_ESSENCE*)pDB->get_data_ptr(tid, ID_SPACE_ESSENCE, DataType);

	m_iPileLimit	= m_pDBEssence->pile_num_max;
	m_iPrice		= m_pDBEssence->price;
	m_iShopPrice	= m_pDBEssence->shop_price;
	m_iProcType		= m_pDBEssence->proc_type;
	m_i64EquipMask	= 0;
	m_bUseable		= true;
}

CECIvtrFactionMaterial::CECIvtrFactionMaterial(const CECIvtrFactionMaterial& s) : CECIvtrItem(s)
{
	m_pDBEssence	=	s.m_pDBEssence;
}

CECIvtrFactionMaterial::~CECIvtrFactionMaterial()
{
}

//	Set item detail information
bool CECIvtrFactionMaterial::SetItemInfo(BYTE* pInfoData, int iDataLen)
{
	CECIvtrItem::SetItemInfo(pInfoData, iDataLen);
	return true;
}

//	Get item icon file name
const char* CECIvtrFactionMaterial::GetIconFile()
{
	return m_pDBEssence->file_icon;
}

//	Get item name
const wchar_t* CECIvtrFactionMaterial::GetName()
{
	return m_pDBEssence->name;
}

//	Get drop model for shown
const char * CECIvtrFactionMaterial::GetDropModel()
{
	return m_pDBEssence->file_matter;
}

//	Get item description text
const wchar_t* CECIvtrFactionMaterial::GetNormalDesc(bool bRepair)
{
	if (m_bNeedUpdate)
		return NULL;

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

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECIvtrCongregate
//	
///////////////////////////////////////////////////////////////////////////

CECIvtrCongregate::CECIvtrCongregate(int tid, int expire_date) : CECIvtrItem(tid, expire_date)
{
	m_iCID	= ICID_CONGREGATE;

	//	Get database data
	elementdataman* pDB = g_pGame->GetElementDataMan();
	DATA_TYPE DataType;
	m_pDBEssence = (CONGREGATE_ESSENCE*)pDB->get_data_ptr(tid, ID_SPACE_ESSENCE, DataType);

	m_iPileLimit	= m_pDBEssence->pile_num_max;
	m_iPrice		= m_pDBEssence->price;
	m_iShopPrice	= m_pDBEssence->shop_price;
	m_iProcType		= m_pDBEssence->proc_type;
	m_i64EquipMask	= 0;
	m_bUseable		= true;
}

CECIvtrCongregate::CECIvtrCongregate(const CECIvtrCongregate& s) : CECIvtrItem(s)
{
	m_pDBEssence	=	s.m_pDBEssence;
}

CECIvtrCongregate::~CECIvtrCongregate()
{
}

//	Set item detail information
bool CECIvtrCongregate::SetItemInfo(BYTE* pInfoData, int iDataLen)
{
	CECIvtrItem::SetItemInfo(pInfoData, iDataLen);
	return true;
}

//	Get item icon file name
const char* CECIvtrCongregate::GetIconFile()
{
	return m_pDBEssence->file_icon;
}

//	Get item name
const wchar_t* CECIvtrCongregate::GetName()
{
	return m_pDBEssence->name;
}

//	Get drop model for shown
const char * CECIvtrCongregate::GetDropModel()
{
	return m_pDBEssence->file_matter;
}

//	Get item cool time
// int CECIvtrCongregate::GetCoolTime(int* piMax/* NULL */)
// {
// 	CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
// 
// 	switch(m_pDBEssence->congregate_type)
// 	{
// 	case CONGREGATE_TYPE_TEAM:
// 		return pHost ? pHost->GetCoolTime(GP_CT_TEAM_CONGREGATE, piMax) : 0;
// 		
// 	case CONGREGATE_TYPE_FACTION:
// 		return pHost ? pHost->GetCoolTime(GP_CT_FACTION_CONGREGATE, piMax) : 0;
// 		
// 	default:
// 		ASSERT(FALSE);
// 		return 0;
// 	}
// }

//	Check item use condition
// bool CECIvtrCongregate::CheckUseCondition()
// {
// 	CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
// 
// 	switch(m_pDBEssence->congregate_type)
// 	{
// 	case CONGREGATE_TYPE_TEAM:
// 		if( !pHost->GetTeam() )
// 		{
// 			return false;
// 		}
// 		break;
// 
// 	case CONGREGATE_TYPE_FACTION:
// 		if( pHost->GetFRoleID() < GNET::_R_MASTER || 
// 			pHost->GetFRoleID() > GNET::_R_BODYGUARD )
// 		{
// 			return false;
// 		}
// 		break;
// 
// 	default:
// 		ASSERT(FALSE);
// 		return false;
// 	}
// 
// 	return true;
// }

//	Get item description text
const wchar_t* CECIvtrCongregate::GetNormalDesc(bool bRepair)
{
	if (m_bNeedUpdate)
		return NULL;

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

	//  Use region
	m_strDesc += _AL("\\r");
	AddDescText(white, false, pDescTab->GetWideString(ITEMDESC_USE_REGION));
	for(int i=0; i<sizeof(m_pDBEssence->area)/sizeof(m_pDBEssence->area[0]); i++)
	{
		CECInstance* pInstance = g_pGame->GetInstance(m_pDBEssence->area[i].id);
		if (pInstance)
		{
			m_strDesc += _AL(" ");
			AddDescText(white, false, pInstance->GetName());
		}
	}
	m_strDesc += _AL("\\r");

	//	Extend description
	AddExtDescText();

	return m_strDesc;
}
///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECIvtrMonsterSpirit
//	
///////////////////////////////////////////////////////////////////////////

CECIvtrMonsterSpirit::CECIvtrMonsterSpirit(int tid, int expire_date) : CECIvtrItem(tid, expire_date)
{
	m_iCID	= ICID_MONSTERSPIRIT;
	
	//	Get database data
	elementdataman* pDB = g_pGame->GetElementDataMan();
	DATA_TYPE DataType;
	m_pDBEssence = (MONSTER_SPIRIT_ESSENCE*)pDB->get_data_ptr(tid, ID_SPACE_ESSENCE, DataType);
	
	m_iPileLimit	= m_pDBEssence->pile_num_max;
	m_iPrice		= m_pDBEssence->price;
	m_iShopPrice	= m_pDBEssence->shop_price;
	m_iProcType		= m_pDBEssence->proc_type;
	m_i64EquipMask	= 0;
	m_bUseable		= true;
}

CECIvtrMonsterSpirit::CECIvtrMonsterSpirit(const CECIvtrMonsterSpirit& s) : CECIvtrItem(s)
{
	m_pDBEssence	=	s.m_pDBEssence;
}

CECIvtrMonsterSpirit::~CECIvtrMonsterSpirit()
{
}

//	Set item detail information
bool CECIvtrMonsterSpirit::SetItemInfo(BYTE* pInfoData, int iDataLen)
{
	CECIvtrItem::SetItemInfo(pInfoData, iDataLen);
	if (!pInfoData || !iDataLen)
		return true;
	
	try
	{
		CECDataReader dr(pInfoData, iDataLen);
		
		m_Essence = *(IVTR_ESSENCE_MONSTERSPIRIT*)dr.Read_Data(sizeof (IVTR_ESSENCE_MONSTERSPIRIT));
	}
	catch (CECException& e)
	{
		ASSERT(0);
		a_LogOutput(1, "CECIvtrGeneralCard::SetItemInfo, data read error (%d)", e.GetType());
		return false;
	}
	
	return true;;
}

//	Get item icon file name
const char* CECIvtrMonsterSpirit::GetIconFile()
{
	return m_pDBEssence->file_icon;
}

//	Get item name
const wchar_t* CECIvtrMonsterSpirit::GetName()
{
	return m_pDBEssence->name;
}

//	Get drop model for shown
const char * CECIvtrMonsterSpirit::GetDropModel()
{
	return m_pDBEssence->file_matter;
}

//	Get item description text
const wchar_t* CECIvtrMonsterSpirit::GetNormalDesc(bool bRepair)
{
	if (m_bNeedUpdate)
		return NULL;
	
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
	
	//	Price
	AddPriceDesc(white, bRepair);
	
	//	Extend description
	AddExtDescText();
	
	return m_strDesc;
}
///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECIvtrGeneralCardDice
//	
///////////////////////////////////////////////////////////////////////////

CECIvtrGeneralCardDice::CECIvtrGeneralCardDice(int tid, int expire_date) : CECIvtrItem(tid, expire_date)
{
	m_iCID	= ICID_GENERALCARD_DICE;
	
	//	Get database data
	elementdataman* pDB = g_pGame->GetElementDataMan();
	DATA_TYPE DataType;
	m_pDBEssence = (POKER_DICE_ESSENCE*)pDB->get_data_ptr(tid, ID_SPACE_ESSENCE, DataType);
	
	m_iPileLimit	= m_pDBEssence->pile_num_max;
	m_iPrice		= m_pDBEssence->price;
	m_iShopPrice	= m_pDBEssence->shop_price;
	m_iProcType		= m_pDBEssence->proc_type;
	m_i64EquipMask	= 0;
	m_bUseable		= true;
}

CECIvtrGeneralCardDice::CECIvtrGeneralCardDice(const CECIvtrGeneralCardDice& s) : CECIvtrItem(s)
{
	m_pDBEssence	=	s.m_pDBEssence;
}

CECIvtrGeneralCardDice::~CECIvtrGeneralCardDice()
{
}

//	Set item detail information
bool CECIvtrGeneralCardDice::SetItemInfo(BYTE* pInfoData, int iDataLen)
{
	CECIvtrItem::SetItemInfo(pInfoData, iDataLen);
	return true;
}

//	Get item icon file name
const char* CECIvtrGeneralCardDice::GetIconFile()
{
	return m_pDBEssence->file_icon;
}

//	Get item name
const wchar_t* CECIvtrGeneralCardDice::GetName()
{
	return m_pDBEssence->name;
}

//	Get drop model for shown
const char * CECIvtrGeneralCardDice::GetDropModel()
{
	return m_pDBEssence->file_matter;
}

//	Get item description text
const wchar_t* CECIvtrGeneralCardDice::GetNormalDesc(bool bRepair)
{
	if (m_bNeedUpdate)
		return NULL;
	
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
	
	//	Price
	AddPriceDesc(white, bRepair);
	
	//	Extend description
	AddExtDescText();
	
	return m_strDesc;
}


///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECIvtrUniversalToken
//	
///////////////////////////////////////////////////////////////////////////

CECIvtrUniversalToken::CECIvtrUniversalToken(int tid, int expire_date) : CECIvtrItem(tid, expire_date)
{
	m_iCID	= ICID_UNIVERSAL_TOKEN;
	
	//	Get database data
	elementdataman* pDB = g_pGame->GetElementDataMan();
	DATA_TYPE DataType;
	m_pDBEssence = (UNIVERSAL_TOKEN_ESSENCE*)pDB->get_data_ptr(tid, ID_SPACE_ESSENCE, DataType);
	
	m_iPileLimit	= m_pDBEssence->pile_num_max;
	m_iPrice		= m_pDBEssence->price;
	m_iShopPrice	= m_pDBEssence->shop_price;
	m_iProcType		= m_pDBEssence->proc_type;
	
	m_bNeedUpdate	= false;
}

CECIvtrUniversalToken::CECIvtrUniversalToken(const CECIvtrUniversalToken& s) : CECIvtrItem(s)
{
	m_pDBEssence	= s.m_pDBEssence;
}

CECIvtrUniversalToken::~CECIvtrUniversalToken()
{
}

bool CECIvtrUniversalToken::SetItemInfo(BYTE* pInfoData, int iDataLen)
{
	CECIvtrItem::SetItemInfo(pInfoData, iDataLen);
	return true;
}

const char* CECIvtrUniversalToken::GetIconFile()
{
	return m_pDBEssence->file_icon;
}

const wchar_t* CECIvtrUniversalToken::GetName()
{
	return m_pDBEssence->name;
}

const char * CECIvtrUniversalToken::GetDropModel()
{
	return m_pDBEssence->file_matter;
}

const wchar_t* CECIvtrUniversalToken::GetNormalDesc(bool bRepair)
{
	m_strDesc = _AL("");
	
	CECStringTab* pDescTab = g_pGame->GetItemDesc();
	int white = ITEMDESC_COL_WHITE;
	int namecol = DecideNameCol();
	
	if (m_iCount > 1)
		AddDescText(namecol, true, pDescTab->GetWideString(ITEMDESC_NAMENUMBER), GetName(), m_iCount);
	else
		AddDescText(namecol, true, pDescTab->GetWideString(ITEMDESC_NAME), GetName());
	AddIDDescText();
	AddExpireTimeDesc();

	int descriptionIndex[ELEMENTDATA_MAX_UNIVERSAL_TOKEN_USAGE] = {
		ITEMDESC_VISIT_HTTP_WITH_TOKEN,
	};
	for (int i(0); i < ELEMENTDATA_MAX_UNIVERSAL_TOKEN_USAGE; ++ i){
		if (m_pDBEssence->combined_switch & (1<<(i))){
			if (descriptionIndex[i] > 0){
				AddDescText(white, true, pDescTab->GetWideString(descriptionIndex[i]));
			}
		}
	}
	
	AddPriceDesc(white, bRepair);
	
	AddExtDescText();
	
	return m_strDesc;
}

bool CECIvtrUniversalToken::HasAnyUsage()const
{
	return m_pDBEssence->combined_switch != 0;
}

bool CECIvtrUniversalToken::HasUsage(unsigned int usageMask)const
{
	return (m_pDBEssence->combined_switch & usageMask) != 0;
}

int	CECIvtrUniversalToken::ConfigIDFor(int usageIndex)const
{
	return m_pDBEssence->config_id[usageIndex];
}

int	CECIvtrUniversalToken::MaskToUsageIndex(unsigned int usageMask)
{
	int usageIndex(-1);
	if (usageMask){
		for (int i(0); i < ELEMENTDATA_MAX_UNIVERSAL_TOKEN_USAGE; ++ i){
			if (((1 << i) & usageMask) == usageMask){
				usageIndex = i;
			}
		}
	}
	return usageIndex;
}

unsigned int CECIvtrUniversalToken::UsageIndexToMask(int usageIndex)
{
	unsigned int usageMask(0);
	if (usageIndex >= 0 && usageIndex < ELEMENTDATA_MAX_UNIVERSAL_TOKEN_USAGE){
		usageMask = 1 << usageIndex;
	}
	return usageMask;
}


int	 CECIvtrUniversalToken::UsageCount()const
{
	int count(0);
	for (int i(0); i < ELEMENTDATA_MAX_UNIVERSAL_TOKEN_USAGE; ++ i){
		if ((1 << i) & m_pDBEssence->combined_switch){
			++ count;
		}
	}
	return count;
}

int	 CECIvtrUniversalToken::UsageIndexAt(int index)const
{
	//	index 为第几个 Usage（从低往到高位），由 UsageCount() 函数控制
	int usageIndex(-1);
	int count(0);
	for (int i(0); i < ELEMENTDATA_MAX_UNIVERSAL_TOKEN_USAGE; ++ i){
		if ((1 << i) & m_pDBEssence->combined_switch){
			if (count++ == index){
				usageIndex = i;
				break;
			}
		}
	}
	return usageIndex;
}