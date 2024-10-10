/*
 * FILE: EC_IvtrPetItem.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2005/12/27
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#include "EC_Global.h"
#include "EC_IvtrPetItem.h"
#include "EC_Game.h"
#include "EC_FixedMsg.h"
#include "EC_RTDebug.h"
#include "EC_GameRun.h"
#include "EC_PetCorral.h"
#include "EC_HostPlayer.h"
#include "EC_UIManager.h"
#include "elementdataman.h"
#include "ElementSkill.h"
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
//	Implement CECIvtrPetEgg
//	
///////////////////////////////////////////////////////////////////////////

CECIvtrPetEgg::CECIvtrPetEgg(int tid, int expire_date) : CECIvtrItem(tid, expire_date)
{
	m_iCID	= ICID_PETEGG;

	//	Get database data
	elementdataman* pDB = g_pGame->GetElementDataMan();
	DATA_TYPE DataType;
	m_pDBEssence	= (PET_EGG_ESSENCE*)pDB->get_data_ptr(tid, ID_SPACE_ESSENCE, DataType);
	m_pPetEssence	= (PET_ESSENCE*)pDB->get_data_ptr(m_pDBEssence->id_pet, ID_SPACE_ESSENCE, DataType);
//	ASSERT(m_pPetEssence);


	m_pEvoPetEssence = NULL;
	if (m_pPetEssence)
	{
		PET_EGG_ESSENCE* pTempDBEssence	= (PET_EGG_ESSENCE*)pDB->get_data_ptr(m_pPetEssence->id_pet_egg_evolved, ID_SPACE_ESSENCE, DataType);
		if (pTempDBEssence)		
			m_pEvoPetEssence = (PET_ESSENCE*)pDB->get_data_ptr(pTempDBEssence->id_pet, ID_SPACE_ESSENCE, DataType);		
	}


	m_iPileLimit	= m_pDBEssence->pile_num_max;
	m_iPrice		= m_pDBEssence->price;
	m_iShopPrice	= m_pDBEssence->shop_price;
	m_iProcType		= m_pDBEssence->proc_type;
	m_i64EquipMask	= 0;
//	m_bUseable		= true;
//	m_dwUseFlags	= USE_PERSIST;

	memset(&m_Essence, 0, sizeof(m_Essence));
	memset(&m_EvoProp, 0, sizeof(m_EvoProp));

	m_pEvoNatureConfig = NULL;
}

CECIvtrPetEgg::CECIvtrPetEgg(const CECIvtrPetEgg& s) : CECIvtrItem(s)
{
	m_pDBEssence	= s.m_pDBEssence;
	m_pPetEssence	= s.m_pPetEssence;
	m_Essence		= s.m_Essence;
	m_strName		= s.m_strName;

	//	Copy skills
	int i, iNumSkill = s.m_aSkills.GetSize();
	m_aSkills.SetSize(iNumSkill, 4);

	for (i=0; i < iNumSkill; i++)
		m_aSkills[i] = s.m_aSkills[i];

	m_EvoProp = s.m_EvoProp;
	m_pEvoNatureConfig = s.m_pEvoNatureConfig;
	m_pEvoPetEssence = s.m_pEvoPetEssence;
}

CECIvtrPetEgg::~CECIvtrPetEgg()
{
}

//	Set item detail information
bool CECIvtrPetEgg::SetItemInfo(BYTE* pInfoData, int iDataLen)
{
	CECIvtrItem::SetItemInfo(pInfoData, iDataLen);

	if (!pInfoData || !iDataLen)
		return true;
	
	try
	{
		CECDataReader dr(pInfoData, iDataLen);

		m_Essence = *(IVTR_ESSENCE_PETEGG*)dr.Read_Data(sizeof (IVTR_ESSENCE_PETEGG));

		if (m_Essence.name_len)
		{
			m_strName = AWString(m_Essence.name, m_Essence.name_len / sizeof (wchar_t));
			g_pGame->GetGameRun()->GetUIManager()->FilterBadWords(m_strName);
		}

		m_aSkills.SetSize(m_Essence.skill_count, 4);
		for (int i=0; i < m_Essence.skill_count; i++)
		{
			PETSKILL& s = m_aSkills[i];
			s.idSkill = dr.Read_int();
			s.iLevel = dr.Read_int();
		}

		if (m_Essence.pet_class == GP_PET_CLASS_EVOLUTION)
		{
			m_EvoProp.r_attack = dr.Read_int();
			m_EvoProp.r_defense = dr.Read_int();
			m_EvoProp.r_hp = dr.Read_int();
			m_EvoProp.r_atk_lvl = dr.Read_int();
			m_EvoProp.r_def_lvl = dr.Read_int();
			m_EvoProp.nature = dr.Read_int();

			elementdataman* pDB = g_pGame->GetElementDataMan();
			DATA_TYPE DataType;
			const void* pDBData = pDB->get_data_ptr(m_EvoProp.nature, ID_SPACE_CONFIG, DataType);
			if (pDBData && DataType == DT_PET_EVOLVED_SKILL_CONFIG)			
				m_pEvoNatureConfig = (PET_EVOLVED_SKILL_CONFIG*)pDBData;
		}
	}
	catch (CECException& e)
	{
		ASSERT(0);
		a_LogOutput(1, "CECIvtrPetEgg::SetItemInfo, data read error (%d)", e.GetType());
		return false;
	}

	return true;
}

//	Get item icon file name
const char* CECIvtrPetEgg::GetIconFile()
{
	return m_pDBEssence->file_icon;
}

//	Get item name
const wchar_t* CECIvtrPetEgg::GetName()
{
	if (m_strName.GetLength())
		return m_strName;
	else
		return m_pDBEssence->name;
}

//	Get item description text
const wchar_t* CECIvtrPetEgg::GetNormalDesc(bool bRepair)
{
	const wchar_t* szDesc = _AL("");
	switch (m_Essence.pet_class)
	{
	case GP_PET_CLASS_MOUNT:	szDesc = GetRidingPetDesc(bRepair);		break;
	case GP_PET_CLASS_COMBAT:	szDesc = GetCombatPetDesc(bRepair);		break;
//	case GP_PET_CLASS_FOLLOW:	szDesc = GetFollowPetDesc(bRepair);		break;
	case GP_PET_CLASS_FOLLOW:	szDesc = GetRidingPetDesc(bRepair);		break;
	case GP_PET_CLASS_EVOLUTION:szDesc = GetEvolutionPetDesc(bRepair);  break;
	}

	return szDesc;
}

//	Build riding pet description text
const wchar_t* CECIvtrPetEgg::GetRidingPetDesc(bool bRepair)
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

	//	Item name: always use the name in template
	if (m_iCount > 1)
		AddDescText(namecol, true, pDescTab->GetWideString(ITEMDESC_NAMENUMBER), m_pDBEssence->name/* GetName() */, m_iCount);
	else
		AddDescText(namecol, true, pDescTab->GetWideString(ITEMDESC_NAME), m_pDBEssence->name/* GetName() */);

	AddIDDescText();

	AddBindDescText();

	AddExpireTimeDesc();

	//	添加颜色信息
	if (m_pPetEssence && m_pPetEssence->require_dye_count > 0)
	{
		if (m_iScaleType == SCALE_BUY)
		{
			AddDescText(white, false, pDescTab->GetWideString(ITEMDESC_COLOR));
			AddDescText(white, true, _AL(" ???"));
		}
		else
		{
			A3DCOLOR clr;
			if (!CECPlayer::RIDINGPET::GetColor(m_Essence.color, clr))
				clr = CECPlayer::RIDINGPET::GetDefaultColor();
			ACString strColor;
			strColor.Format(_AL("^%02x%02x%02x"), A3DCOLOR_GETRED(clr), A3DCOLOR_GETGREEN(clr), A3DCOLOR_GETBLUE(clr));
			
			AddDescText(white, false, pDescTab->GetWideString(ITEMDESC_COLOR));
			m_strDesc += _AL(" ");
			AddDescText(-1, true, pDescTab->GetWideString(ITEMDESC_COLORRECT), strColor);
		}
	}

	//	Food type requirement
	AddFoodTypeDesc();

	if (m_pPetEssence)
	{
		//	Pet level
		AddDescText(white, true, pDescTab->GetWideString(ITEMDESC_PETLEVEL), m_Essence.level);

		//	Move speed
		float fSpeed = m_pPetEssence->speed_a + (m_Essence.level - 1) * m_pPetEssence->speed_b;
		AddDescText(-1, true, pDescTab->GetWideString(ITEMDESC_MOVESPEED), fSpeed);

		//	Profession requirement
		AddProfReqDesc(m_pPetEssence->character_combo_id);
	}

	//	Level requirement
	int iLevelReq = a_Max((int)m_Essence.level, m_Essence.req_level);
	if (iLevelReq)
	{
		int col = pHost->GetMaxLevelSofar() >= iLevelReq ? white : red;
		AddDescText(col, true, pDescTab->GetWideString(ITEMDESC_LEVELREQ), iLevelReq);
	}

	//	Price
	AddPriceDesc(white, bRepair);

	//	Extend description
	AddExtDescText();

	return m_strDesc;
}

//	Build combat pet description text
const wchar_t* CECIvtrPetEgg::GetCombatPetDesc(bool bRepair)
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

	//	Item name: always use the name in template
	if (m_iCount > 1)
		AddDescText(namecol, true, pDescTab->GetWideString(ITEMDESC_NAMENUMBER), m_pDBEssence->name/* GetName() */, m_iCount);
	else
		AddDescText(namecol, true, pDescTab->GetWideString(ITEMDESC_NAME), m_pDBEssence->name/* GetName() */);

	AddIDDescText();

	AddBindDescText();

	AddExpireTimeDesc();

	//	Food type requirement
	AddFoodTypeDesc();

	if (m_pPetEssence)
	{
		//	Pet level
		AddDescText(white, true, pDescTab->GetWideString(ITEMDESC_PETLEVEL), m_Essence.level);

		//	Physical damage
		int iVal = int(m_pPetEssence->damage_a * (m_pPetEssence->damage_b * m_Essence.level * m_Essence.level + m_pPetEssence->damage_c * m_Essence.level + m_pPetEssence->damage_d));
		AddDescText(-1, true, pDescTab->GetWideString(ITEMDESC_DAMAGE), iVal);

		//	Attack speed
		AddDescText(-1, false, pDescTab->GetWideString(ITEMDESC_ATKSPEED));
		AddDescText(-1, true, _AL(" %.2f"), 1.0f / m_pPetEssence->attack_speed);

		//	Attack rating
		iVal = int(m_pPetEssence->attack_a * (m_Essence.level - m_pPetEssence->attack_b * m_pPetEssence->level_require + m_pPetEssence->attack_c));
		AddDescText(-1, true, pDescTab->GetWideString(ITEMDESC_ATTACKRATE), iVal);

		//	Physical defence
		iVal = int(m_pPetEssence->physic_defence_a * (m_pPetEssence->physic_defence_b * (m_Essence.level - m_pPetEssence->physic_defence_c * m_pPetEssence->level_require) + m_pPetEssence->physic_defence_d));
		AddDescText(-1, false, pDescTab->GetWideString(ITEMDESC_PHYDEFENCE));
		AddDescText(-1, true, _AL(" %d"), iVal);

		//	Magic defence
		iVal = int(m_pPetEssence->magic_defence_a * (m_pPetEssence->magic_defence_b * (m_Essence.level - m_pPetEssence->magic_defence_c * m_pPetEssence->level_require) + m_pPetEssence->magic_defence_d));
		AddDescText(-1, false, pDescTab->GetWideString(ITEMDESC_ALLMAGICDEF));
		AddDescText(-1, true, _AL(" %d"), iVal);

		//	Dodge
		iVal = int(m_pPetEssence->armor_a * (m_Essence.level - m_pPetEssence->armor_b * m_pPetEssence->level_require + m_pPetEssence->armor_c));
		AddDescText(-1, false, pDescTab->GetWideString(ITEMDESC_DODGE));
		AddDescText(-1, true, _AL(" %d"), iVal);

		//	hp
		iVal = int(m_pPetEssence->hp_a * (m_Essence.level - m_pPetEssence->hp_b * m_pPetEssence->level_require + m_pPetEssence->hp_c));
		AddDescText(-1, false, pDescTab->GetWideString(ITEMDESC_ADDHP));
		AddDescText(-1, true, _AL(" %d"), iVal);

		//	Profession requirement
		AddProfReqDesc(m_pPetEssence->character_combo_id);
	}

	//	Level requirement
	int iLevelReq = a_Max((int)m_Essence.level, m_Essence.req_level);
	if (iLevelReq)
	{
		int col = pHost->GetMaxLevelSofar() >= iLevelReq ? white : red;
		AddDescText(col, true, pDescTab->GetWideString(ITEMDESC_LEVELREQ), iLevelReq);
	}

	//	Skill list
	for (int i=0; i < m_aSkills.GetSize(); i++)
	{
		const PETSKILL& skill = m_aSkills[i];
		const wchar_t* szName = g_pGame->GetSkillDesc()->GetWideString(skill.idSkill * 10);
		AddDescText(ITEMDESC_COL_YELLOW, true, pDescTab->GetWideString(ITEMDESC_PETSKILL), szName ? szName : _AL(""), skill.iLevel);
	}
	
	// 可进化
	if (m_pEvoPetEssence)
	{
		AddDescText(ITEMDESC_COL_WHITE, true, pDescTab->GetWideString(ITEMDESC_PET_EVO_NAME), m_pEvoPetEssence->name);
	}

	//	Price
	AddPriceDesc(white, bRepair);

	//	Extend description
	AddExtDescText();

	return m_strDesc;
}

//	Build follow pet description text
const wchar_t* CECIvtrPetEgg::GetFollowPetDesc(bool bRepair)
{
	return m_strDesc;
}
//	Build evolution pet description text
const wchar_t* CECIvtrPetEgg::GetEvolutionPetDesc(bool bRepair)
{
	if (m_bNeedUpdate)
		return NULL;
	
	m_strDesc = _AL("");

	CECStringTab* pDescTab = g_pGame->GetItemDesc();

	m_strDesc = GetCombatPetDesc(bRepair);

	m_strDesc += _AL("\\r");

	AddDescText(ITEMDESC_COL_WHITE, true, pDescTab->GetWideString(ITEMDESC_PET_NATURE),m_pEvoNatureConfig!=NULL ? m_pEvoNatureConfig->name:_AL(""));
	AddDescText(ITEMDESC_COL_WHITE, true, pDescTab->GetWideString(ITEMDESC_PET_ATK_INHERIT), m_EvoProp.r_attack);
	AddDescText(ITEMDESC_COL_WHITE, true, pDescTab->GetWideString(ITEMDESC_PET_DEF_INHERIT), m_EvoProp.r_defense);
	AddDescText(ITEMDESC_COL_WHITE, true, pDescTab->GetWideString(ITEMDESC_PET_HP_INHERIT), m_EvoProp.r_hp);
	AddDescText(ITEMDESC_COL_WHITE, true, pDescTab->GetWideString(ITEMDESC_PET_ATKLVL_INHERIT), m_EvoProp.r_atk_lvl);
	AddDescText(ITEMDESC_COL_WHITE, true, pDescTab->GetWideString(ITEMDESC_PET_DEFLVL_INHERIT), m_EvoProp.r_def_lvl);

	return m_strDesc;
}

//	Add food type requirment description
void CECIvtrPetEgg::AddFoodTypeDesc()
{
	elementdataman* pDB = g_pGame->GetElementDataMan();
	DATA_TYPE DataType;
	PET_ESSENCE* pPetEssence = (PET_ESSENCE*)pDB->get_data_ptr(m_Essence.pet_tid, ID_SPACE_ESSENCE, DataType);
	if (!pPetEssence)
		return;

	CECStringTab* pDescTab = g_pGame->GetItemDesc();
	AddDescText(ITEMDESC_COL_WHITE, false, pDescTab->GetWideString(ITEMDESC_FOODTYPE));

	for (int i=0; i < MAX_PET_FOOD; i++)
	{
		if (pPetEssence->food_mask & (1 << i))
		{
			m_strDesc += _AL(" ");
			AddDescText(-1, false, pDescTab->GetWideString(ITEMDESC_FOOD_GRASS+i));
		}
	}

	m_strDesc += _AL("\\r");
}

//	Get item description for booth buying
const wchar_t* CECIvtrPetEgg::GetBoothBuyDesc()
{
	m_strDesc = _AL("");

	CECStringTab* pDescTab = g_pGame->GetItemDesc();

	int white = ITEMDESC_COL_WHITE;

	//	Item name
	AddDescText(white, true, pDescTab->GetWideString(ITEMDESC_NAME), m_pDBEssence->name);

	//	Price
	AddPriceDesc(white, false);

	return m_strDesc;
}

//	Get drop model for shown
const char * CECIvtrPetEgg::GetDropModel()
{
	return m_pDBEssence->file_matter;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECIvtrPetFood
//	
///////////////////////////////////////////////////////////////////////////

CECIvtrPetFood::CECIvtrPetFood(int tid, int expire_date) : CECIvtrItem(tid, expire_date)
{
	m_iCID	= ICID_PETFOOD;

	//	Get database data
	elementdataman* pDB = g_pGame->GetElementDataMan();
	DATA_TYPE DataType;
	m_pDBEssence	= (PET_FOOD_ESSENCE*)pDB->get_data_ptr(tid, ID_SPACE_ESSENCE, DataType);

	m_iPileLimit	= m_pDBEssence->pile_num_max;
	m_iPrice		= m_pDBEssence->price;
	m_iShopPrice	= m_pDBEssence->shop_price;
	m_iProcType		= m_pDBEssence->proc_type;
	m_i64EquipMask	= 0;
	m_bUseable		= true;

	m_bNeedUpdate	= false;
}

CECIvtrPetFood::CECIvtrPetFood(const CECIvtrPetFood& s) : CECIvtrItem(s)
{
	m_pDBEssence	= s.m_pDBEssence;
}

CECIvtrPetFood::~CECIvtrPetFood()
{
}

//	Set item detail information
bool CECIvtrPetFood::SetItemInfo(BYTE* pInfoData, int iDataLen)
{
	CECIvtrItem::SetItemInfo(pInfoData, iDataLen);
	return true;
}

//	Get item icon file name
const char* CECIvtrPetFood::GetIconFile()
{
	return m_pDBEssence->file_icon;
}

//	Get item name
const wchar_t* CECIvtrPetFood::GetName()
{
	return m_pDBEssence->name;
}

//	Get item cool time
int CECIvtrPetFood::GetCoolTime(int* piMax/* NULL */)
{
	CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
	if (!pHost)
		return 0;

	int iTime = pHost->GetCoolTime(GP_CT_FEED_PET, piMax);
	return iTime;
}

//	Check item use condition
bool CECIvtrPetFood::CheckUseCondition()
{
	CECPetData* pPet = g_pGame->GetGameRun()->GetHostPlayer()->GetPetCorral()->GetActivePet();
	if (!pPet)
		return false;

	DATA_TYPE DataType;
	const PET_ESSENCE* pPetEssence = (const PET_ESSENCE*)g_pGame->GetElementDataMan()->get_data_ptr(pPet->GetTemplateID(), ID_SPACE_ESSENCE, DataType);
	if (!pPetEssence)
		return false;

	return (m_pDBEssence->food_type & pPetEssence->food_mask) ? true : false;
}

//	Get item description text
const wchar_t* CECIvtrPetFood::GetNormalDesc(bool bRepair)
{
	m_strDesc = _AL("");

	//	Try to build item description
	CECStringTab* pDescTab = g_pGame->GetItemDesc();
	int white = ITEMDESC_COL_WHITE;
	int namecol = DecideNameCol();

	//	Item name
	if (m_iCount > 1)
		AddDescText(namecol, true, pDescTab->GetWideString(ITEMDESC_NAMENUMBER), GetName(), m_iCount);
	else
		AddDescText(namecol, true, pDescTab->GetWideString(ITEMDESC_NAME), GetName());

	AddIDDescText();

	AddExpireTimeDesc();

	//	Food type
	for (int i=0; i < MAX_PET_FOOD; i++)
	{
		if (m_pDBEssence->food_type & (1 << i))
		{
			AddDescText(white, false, pDescTab->GetWideString(ITEMDESC_FOODTYPE));
			m_strDesc += _AL(" ");
			AddDescText(-1, true, pDescTab->GetWideString(ITEMDESC_FOOD_GRASS+i));
			break;
		}
	}

	//	Price
	AddPriceDesc(white, bRepair);

	//	Extend description
	AddExtDescText();

	return m_strDesc;
}

//	Get drop model for shown
const char * CECIvtrPetFood::GetDropModel()
{
	return m_pDBEssence->file_matter;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECIvtrPetFaceTicket
//	
///////////////////////////////////////////////////////////////////////////

CECIvtrPetFaceTicket::CECIvtrPetFaceTicket(int tid, int expire_date) : CECIvtrItem(tid, expire_date)
{
	m_iCID	= ICID_PETFACETICKET;

	//	Get database data
	elementdataman* pDB = g_pGame->GetElementDataMan();
	DATA_TYPE DataType;
	m_pDBEssence	= (PET_FACETICKET_ESSENCE*)pDB->get_data_ptr(tid, ID_SPACE_ESSENCE, DataType);

	m_iPileLimit	= m_pDBEssence->pile_num_max;
	m_iPrice		= m_pDBEssence->price;
	m_iShopPrice	= m_pDBEssence->shop_price;
	m_iProcType		= m_pDBEssence->proc_type;
	m_i64EquipMask	= 0;
	m_bUseable		= true;

	m_bNeedUpdate	= false;
}

CECIvtrPetFaceTicket::CECIvtrPetFaceTicket(const CECIvtrPetFaceTicket& s) : CECIvtrItem(s)
{
	m_pDBEssence	= s.m_pDBEssence;
}

CECIvtrPetFaceTicket::~CECIvtrPetFaceTicket()
{
}

//	Set item detail information
bool CECIvtrPetFaceTicket::SetItemInfo(BYTE* pInfoData, int iDataLen)
{
	CECIvtrItem::SetItemInfo(pInfoData, iDataLen);
	return true;
}

//	Get item icon file name
const char* CECIvtrPetFaceTicket::GetIconFile()
{
	return m_pDBEssence->file_icon;
}

//	Get item name
const wchar_t* CECIvtrPetFaceTicket::GetName()
{
	return m_pDBEssence->name;
}

//	Get item description text
const wchar_t* CECIvtrPetFaceTicket::GetNormalDesc(bool bRepair)
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
const char * CECIvtrPetFaceTicket::GetDropModel()
{
	return m_pDBEssence->file_matter;
}
