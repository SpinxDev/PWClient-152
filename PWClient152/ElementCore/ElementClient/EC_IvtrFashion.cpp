/*
 * FILE: EC_IvtrFashion.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Hedi, 2005/7/18
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#include <A3DMacros.h>

#include "EC_Global.h"
#include "EC_IvtrFashion.h"
#include "EC_Game.h"
#include "EC_RoleTypes.h"
#include "EC_FixedMsg.h"
#include "EC_GameRun.h"
#include "EC_HostPlayer.h"
#include "elementdataman.h"
#include "EC_ManPlayer.h"
#include "EC_RTDebug.h"
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
//	Implement CECIvtrArmor
//	
///////////////////////////////////////////////////////////////////////////

CECIvtrFashion::CECIvtrFashion(int tid, int expire_date) : CECIvtrEquip(tid, expire_date)
{
	m_iCID	= ICID_FASHION;

	memset(&m_Essence, 0, sizeof (m_Essence));

	//	Get database data
	elementdataman* pDB = g_pGame->GetElementDataMan();
	DATA_TYPE DataType;
	m_pDBEssence	= (FASHION_ESSENCE*)pDB->get_data_ptr(tid, ID_SPACE_ESSENCE, DataType);
	m_pDBMajorType	= (FASHION_MAJOR_TYPE*)pDB->get_data_ptr(m_pDBEssence->id_major_type, ID_SPACE_ESSENCE, DataType);
	m_pDBSubType	= (FASHION_SUB_TYPE*)pDB->get_data_ptr(m_pDBEssence->id_sub_type, ID_SPACE_ESSENCE, DataType);

	m_iPileLimit	= m_pDBEssence->pile_num_max;
	m_iPrice		= m_pDBEssence->price;
	m_iShopPrice	= m_pDBEssence->shop_price;
	m_i64EquipMask	= m_pDBSubType->equip_fashion_mask;
	m_iProcType		= m_pDBEssence->proc_type;

	// reset the info
	DefaultInfo();
}

CECIvtrFashion::CECIvtrFashion(const CECIvtrFashion& s) : CECIvtrEquip(s)
{
	m_pDBEssence	= s.m_pDBEssence;
	m_pDBMajorType	= s.m_pDBMajorType;
	m_pDBSubType	= s.m_pDBSubType;
	m_Essence		= s.m_Essence;
	
	m_iGender		= s.m_iGender;
	m_wColor		= s.m_wColor;
	m_color			= s.m_color;
}

CECIvtrFashion::~CECIvtrFashion()
{
}

//	Set item detail information
bool CECIvtrFashion::SetItemInfo(BYTE* pInfoData, int iDataLen)
{
	//	Note: because fashion is not an absolute equipment, so skip
	//	CECIvtrEquip::SetItemInfo().

	if (!pInfoData || !iDataLen)
	{
		m_bNeedUpdate = false;
		return true;
	}

	try
	{
		CECDataReader dr(pInfoData, iDataLen);
		m_Essence = *(IVTR_ESSENCE_FASHION*)dr.Read_Data(sizeof (IVTR_ESSENCE_FASHION));
	}
	catch (CECException& e)
	{
		ASSERT(0);
		a_LogOutput(1, "CECIvtrFashion::SetItemInfo, data read error (%d)", e.GetType());
		return false;
	}

	m_iLevelReq	= m_Essence.require_level;
	m_wColor	= m_Essence.color;
	m_iGender	= m_Essence.gender;

	m_color = FASHION_WORDCOLOR_TO_A3DCOLOR(m_wColor);

	m_bNeedUpdate = false;
	return true;
}

//	Get item default information from database
void CECIvtrFashion::DefaultInfo()
{
	m_iLevelReq = m_pDBEssence->require_level;
	m_wColor = 0x7fff;
	m_iGender = m_pDBEssence->gender;
	m_color	= 0xffffffff;

	// these data also stored in essence, sync it here
	m_Essence.require_level = m_iLevelReq;
	m_Essence.color = m_wColor;
	m_Essence.gender = m_iGender;
}
//	Get item icon file name
const char* CECIvtrFashion::GetIconFile()
{
	return m_pDBEssence->file_icon;
}

//	Get item name
const wchar_t* CECIvtrFashion::GetName()
{
	return m_pDBEssence->name;
}

//	Get item description text
const wchar_t* CECIvtrFashion::GetNormalDesc(bool bRepair)
{
	if (m_bNeedUpdate)
		return NULL;

	m_strDesc = _AL("");

	//	Try to build item description
	CECStringTab* pDescTab = g_pGame->GetItemDesc();
	CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
	int white = ITEMDESC_COL_WHITE;
	int red = ITEMDESC_COL_RED;
	int namecol = DecideNameCol();

	//	Item name
	AddDescText(namecol, true, pDescTab->GetWideString(ITEMDESC_NAME), GetName());

	AddIDDescText();
	
	AddBindDescText();

	AddExpireTimeDesc();

	//	Sub class name
	AddDescText(white, true, pDescTab->GetWideString(ITEMDESC_CLASSNAME), m_pDBSubType->name);

	//	Color
	//	Use m_iScaleType == SCALE_BUY to judge whether this item is in NPC's pack, 
	//	this may not be a valid way
	if (m_pDBEssence->equip_location != enumSkinShowArmet && m_pDBEssence->equip_location != enumSkinShowHand)
	{
		if (m_iScaleType == SCALE_BUY)
		{
			AddDescText(white, false, pDescTab->GetWideString(ITEMDESC_COLOR));
			AddDescText(white, true, _AL(" ???"));
		}
		else
		{
			ACHAR szCol[10];
			a_sprintf(szCol, _AL("^%02x%02x%02x"), A3DCOLOR_GETRED(m_color), A3DCOLOR_GETGREEN(m_color), A3DCOLOR_GETBLUE(m_color));
			
			AddDescText(white, false, pDescTab->GetWideString(ITEMDESC_COLOR));
			m_strDesc += _AL(" ");
			AddDescText(-1, true, pDescTab->GetWideString(ITEMDESC_COLORRECT), szCol);
		}

	}
	//	Level requirment
	if (m_iLevelReq)
	{
		int col = pHost->GetMaxLevelSofar() >= m_iLevelReq ? white : red;
		AddDescText(col, true, pDescTab->GetWideString(ITEMDESC_LEVELREQ), m_iLevelReq);
	}
	//  fashion weapon profession requirment and weapon action type
	if (m_pDBEssence->equip_location == enumSkinShowHand)
	{
		AddDescText(-1, true, pDescTab->GetWideString(ITEMDESC_LEVEL), m_pDBEssence->level);
		AddProfReqDesc(m_pDBEssence->character_combo_id);
		AddActionTypeDescText(m_pDBEssence->action_type);
	}

	//	Gender requirement
	CECStringTab* pFixMsg = g_pGame->GetFixedMsgTab();
	int col = (pHost->GetGender() == m_iGender) ? white : red;
	AddDescText(col, false, pDescTab->GetWideString(ITEMDESC_GENDERREQ));
	m_strDesc += _AL(" ");
	if (m_iGender == GENDER_MALE)
		AddDescText(col, true, pFixMsg->GetWideString(FIXMSG_GENDER_MALE));
	else
		AddDescText(col, true, pFixMsg->GetWideString(FIXMSG_GENDER_FEMALE));
	
	//	Price
	AddPriceDesc(white, bRepair);

	//	Fashion level
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

//	Does this equipment has random property ?
bool CECIvtrFashion::HasRandomProp()
{
	return false;
}

//	Get drop model for shown
const char * CECIvtrFashion::GetDropModel()
{
	return m_pDBEssence->file_matter;
}

int CECIvtrFashion::GetFashionSuiteID()const
{
	CECGame::SuiteEquipTable& SuiteTab = g_pGame->GetSuiteFashionTable();
	CECGame::SuiteEquipTable::pair_type pair = SuiteTab.get(m_tid);
	return pair.second ? *pair.first : 0;
}
