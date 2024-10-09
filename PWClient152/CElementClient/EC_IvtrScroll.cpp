/*
 * FILE: EC_IvtrScroll.cpp
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
#include "EC_IvtrScroll.h"
#include "EC_Game.h"
#include "EC_FixedMsg.h"
#include "EC_GameRun.h"
#include "EC_HostPlayer.h"
#include "EC_ManNPC.h"
#include "EC_Monster.h"
#include "EC_World.h"
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
//	Implement CECIvtrSkillTome
//	
///////////////////////////////////////////////////////////////////////////

CECIvtrSkillTome::CECIvtrSkillTome(int tid, int expire_date) : CECIvtrItem(tid, expire_date)
{
	m_iCID	= ICID_SKILLTOME;

	//	Get database data
	elementdataman* pDB = g_pGame->GetElementDataMan();
	DATA_TYPE DataType;
	m_pDBEssence	= (SKILLTOME_ESSENCE*)pDB->get_data_ptr(tid, ID_SPACE_ESSENCE, DataType);
	m_pDBSubType	= (SKILLTOME_SUB_TYPE*)pDB->get_data_ptr(m_pDBEssence->id_sub_type, ID_SPACE_ESSENCE, DataType);

	m_iPileLimit	= m_pDBEssence->pile_num_max;
	m_iPrice		= m_pDBEssence->price;
	m_iShopPrice	= m_pDBEssence->shop_price;
	m_iProcType		= m_pDBEssence->proc_type;
	m_i64EquipMask	= 0;

	m_bNeedUpdate	= false;
}

CECIvtrSkillTome::CECIvtrSkillTome(const CECIvtrSkillTome& s) : CECIvtrItem(s)
{
	m_pDBEssence	= s.m_pDBEssence;
	m_pDBSubType	= s.m_pDBSubType;
}

CECIvtrSkillTome::~CECIvtrSkillTome()
{
}

//	Set item detail information
bool CECIvtrSkillTome::SetItemInfo(BYTE* pInfoData, int iDataLen)
{
	CECIvtrItem::SetItemInfo(pInfoData, iDataLen);
	return true;
}

//	Get item icon file name
const char* CECIvtrSkillTome::GetIconFile()
{
	return m_pDBEssence->file_icon;
}

//	Get item name
const wchar_t* CECIvtrSkillTome::GetName()
{
	return m_pDBEssence->name;
}

//	Get item description text
const wchar_t* CECIvtrSkillTome::GetNormalDesc(bool bRepair)
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
const char * CECIvtrSkillTome::GetDropModel()
{
	return m_pDBEssence->file_matter;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECIvtrTownScroll
//	
///////////////////////////////////////////////////////////////////////////

CECIvtrTownScroll::CECIvtrTownScroll(int tid, int expire_date) : CECIvtrItem(tid, expire_date)
{
	m_iCID	= ICID_TOWNSCROLL;

	//	Get database data
	elementdataman* pDB = g_pGame->GetElementDataMan();
	DATA_TYPE DataType;
	m_pDBEssence = (TOWNSCROLL_ESSENCE*)pDB->get_data_ptr(tid, ID_SPACE_ESSENCE, DataType);

	m_iPileLimit	= m_pDBEssence->pile_num_max;
	m_iPrice		= m_pDBEssence->price;
	m_iShopPrice	= m_pDBEssence->shop_price;
	m_iProcType		= m_pDBEssence->proc_type;
	m_i64EquipMask	= 0;
	m_bUseable		= true;
	m_dwUseFlags	= USE_PERSIST;

	m_bNeedUpdate	= false;
}

CECIvtrTownScroll::CECIvtrTownScroll(const CECIvtrTownScroll& s) : CECIvtrItem(s)
{
	m_pDBEssence	= s.m_pDBEssence;
}

CECIvtrTownScroll::~CECIvtrTownScroll()
{
}

//	Set item detail information
bool CECIvtrTownScroll::SetItemInfo(BYTE* pInfoData, int iDataLen)
{
	CECIvtrItem::SetItemInfo(pInfoData, iDataLen);
	return true;
}

//	Get item icon file name
const char* CECIvtrTownScroll::GetIconFile()
{
	return m_pDBEssence->file_icon;
}

//	Get item name
const wchar_t* CECIvtrTownScroll::GetName()
{
	return m_pDBEssence->name;
}

//	Get item cool time
int CECIvtrTownScroll::GetCoolTime(int* piMax/* NULL */)
{
	CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
	return pHost ? pHost->GetCoolTime(GP_CT_TOWNSCROLL, piMax) : 0;
}

//	Get item description text
const wchar_t* CECIvtrTownScroll::GetNormalDesc(bool bRepair)
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
const char * CECIvtrTownScroll::GetDropModel()
{
	return m_pDBEssence->file_matter;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECIvtrRevScroll
//	
///////////////////////////////////////////////////////////////////////////

CECIvtrRevScroll::CECIvtrRevScroll(int tid, int expire_date) : CECIvtrItem(tid, expire_date)
{
	m_iCID	= ICID_REVSCROLL;

	//	Get database data
	elementdataman* pDB = g_pGame->GetElementDataMan();
	DATA_TYPE DataType;
	m_pDBEssence = (REVIVESCROLL_ESSENCE*)pDB->get_data_ptr(tid, ID_SPACE_ESSENCE, DataType);

	m_iPileLimit	= m_pDBEssence->pile_num_max;
	m_iPrice		= m_pDBEssence->price;
	m_iShopPrice	= m_pDBEssence->shop_price;
	m_iProcType		= m_pDBEssence->proc_type;
	m_i64EquipMask	= 0;
	m_bUseable		= true;

	m_bNeedUpdate	= false;
}

CECIvtrRevScroll::CECIvtrRevScroll(const CECIvtrRevScroll& s) : CECIvtrItem(s)
{
	m_pDBEssence	= s.m_pDBEssence;
}

CECIvtrRevScroll::~CECIvtrRevScroll()
{
}

//	Set item detail information
bool CECIvtrRevScroll::SetItemInfo(BYTE* pInfoData, int iDataLen)
{
	CECIvtrItem::SetItemInfo(pInfoData, iDataLen);
	return true;
}

//	Get item icon file name
const char* CECIvtrRevScroll::GetIconFile()
{
	return m_pDBEssence->file_icon;
}

//	Get item name
const wchar_t* CECIvtrRevScroll::GetName()
{
	return m_pDBEssence->name;
}

//	Get item cool time
int CECIvtrRevScroll::GetCoolTime(int* piMax/* NULL */)
{
	CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
	return pHost ? pHost->GetCoolTime(GP_CT_SOUL_STONE, piMax) : 0;
}

//	Get item description text
const wchar_t* CECIvtrRevScroll::GetNormalDesc(bool bRepair)
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
const char * CECIvtrRevScroll::GetDropModel()
{
	return m_pDBEssence->file_matter;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECIvtrUnionScroll
//	
///////////////////////////////////////////////////////////////////////////

CECIvtrUnionScroll::CECIvtrUnionScroll(int tid, int expire_date) : CECIvtrItem(tid, expire_date)
{
	m_iCID	= ICID_UNIONSCROLL;

	//	Get database data
	elementdataman* pDB = g_pGame->GetElementDataMan();
	DATA_TYPE DataType;
	m_pDBEssence = (UNIONSCROLL_ESSENCE*)pDB->get_data_ptr(tid, ID_SPACE_ESSENCE, DataType);

	m_iPileLimit	= m_pDBEssence->pile_num_max;
	m_iPrice		= m_pDBEssence->price;
	m_iShopPrice	= m_pDBEssence->shop_price;
	m_iProcType		= m_pDBEssence->proc_type;
	m_i64EquipMask	= 0;

	m_bNeedUpdate	= false;
}

CECIvtrUnionScroll::CECIvtrUnionScroll(const CECIvtrUnionScroll& s) : CECIvtrItem(s)
{
	m_pDBEssence	= s.m_pDBEssence;
}

CECIvtrUnionScroll::~CECIvtrUnionScroll()
{
}

//	Set item detail information
bool CECIvtrUnionScroll::SetItemInfo(BYTE* pInfoData, int iDataLen)
{
	CECIvtrItem::SetItemInfo(pInfoData, iDataLen);
	return true;
}

//	Get item icon file name
const char* CECIvtrUnionScroll::GetIconFile()
{
	return m_pDBEssence->file_icon;
}

//	Get item name
const wchar_t* CECIvtrUnionScroll::GetName()
{
	return m_pDBEssence->name;
}

//	Get item description text
const wchar_t* CECIvtrUnionScroll::GetNormalDesc(bool bRepair)
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
const char * CECIvtrUnionScroll::GetDropModel()
{
	return m_pDBEssence->file_matter;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECIvtrFaceTicket
//	
///////////////////////////////////////////////////////////////////////////

CECIvtrFaceTicket::CECIvtrFaceTicket(int tid, int expire_date) : CECIvtrItem(tid, expire_date)
{
	m_iCID	= ICID_FACETICKET;

	//	Get database data
	elementdataman* pDB = g_pGame->GetElementDataMan();
	DATA_TYPE DataType;
	m_pDBEssence	= (FACETICKET_ESSENCE*)pDB->get_data_ptr(tid, ID_SPACE_ESSENCE, DataType);
	m_pDBMajorType	= (FACETICKET_MAJOR_TYPE*)pDB->get_data_ptr(m_pDBEssence->id_major_type, ID_SPACE_ESSENCE, DataType);
	m_pDBSubType	= (FACETICKET_SUB_TYPE*)pDB->get_data_ptr(m_pDBEssence->id_sub_type, ID_SPACE_ESSENCE, DataType);

	m_iPileLimit	= m_pDBEssence->pile_num_max;
	m_iPrice		= m_pDBEssence->price;
	m_iShopPrice	= m_pDBEssence->shop_price;
	m_iLevelReq		= m_pDBEssence->require_level;
	m_iProcType		= m_pDBEssence->proc_type;
	m_i64EquipMask	= 0;

	m_bNeedUpdate	= false;
}

CECIvtrFaceTicket::CECIvtrFaceTicket(const CECIvtrFaceTicket& s) : CECIvtrItem(s)
{
	m_pDBEssence	= s.m_pDBEssence;
	m_pDBMajorType	= s.m_pDBMajorType;
	m_pDBSubType	= s.m_pDBSubType;
	m_iLevelReq		= s.m_iLevelReq;
}

CECIvtrFaceTicket::~CECIvtrFaceTicket()
{
}

//	Set item detail information
bool CECIvtrFaceTicket::SetItemInfo(BYTE* pInfoData, int iDataLen)
{
	CECIvtrItem::SetItemInfo(pInfoData, iDataLen);
	return true;
}

//	Get item icon file name
const char* CECIvtrFaceTicket::GetIconFile()
{
	return m_pDBEssence->file_icon;
}

//	Get item name
const wchar_t* CECIvtrFaceTicket::GetName()
{
	return m_pDBEssence->name;
}

//	Get item cool time
int CECIvtrFaceTicket::GetCoolTime(int* piMax/* NULL */)
{
	CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
	return pHost ? pHost->GetCoolTime(GP_CT_FACETICKET, piMax) : 0;
}

//	Get item description text
const wchar_t* CECIvtrFaceTicket::GetNormalDesc(bool bRepair)
{
	m_strDesc = _AL("");

	//	Try to build item description
	CECStringTab* pDescTab = g_pGame->GetItemDesc();
	int white = ITEMDESC_COL_WHITE;
	int red = ITEMDESC_COL_RED;
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
		int col = g_pGame->GetGameRun()->GetHostPlayer()->GetMaxLevelSofar() >= m_iLevelReq ? white : red;
		AddDescText(col, true, pDescTab->GetWideString(ITEMDESC_LEVELREQ), m_iLevelReq);
	}

	//	Price
	AddPriceDesc(white, bRepair);

	//	Extend description
	AddExtDescText();

	return m_strDesc;
}

//	Check item use condition
bool CECIvtrFaceTicket::CheckUseCondition()
{
	CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();

	if (pHost->GetMaxLevelSofar() < GetLevelRequirement())
		return false;

	return true;
}

//	Get drop model for shown
const char * CECIvtrFaceTicket::GetDropModel()
{
	return m_pDBEssence->file_matter;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECIvtrFacePill
//	
///////////////////////////////////////////////////////////////////////////

CECIvtrFacePill::CECIvtrFacePill(int tid, int expire_date) : CECIvtrItem(tid, expire_date)
{
	m_iCID	= ICID_FACEPILL;

	//	Get database data
	elementdataman* pDB = g_pGame->GetElementDataMan();
	DATA_TYPE DataType;
	m_pDBEssence	= (FACEPILL_ESSENCE*)pDB->get_data_ptr(tid, ID_SPACE_ESSENCE, DataType);
	m_pDBMajorType	= (FACEPILL_MAJOR_TYPE*)pDB->get_data_ptr(m_pDBEssence->id_major_type, ID_SPACE_ESSENCE, DataType);
	m_pDBSubType	= (FACEPILL_SUB_TYPE*)pDB->get_data_ptr(m_pDBEssence->id_sub_type, ID_SPACE_ESSENCE, DataType);

	m_iPileLimit	= m_pDBEssence->pile_num_max;
	m_iPrice		= m_pDBEssence->price;
	m_iShopPrice	= m_pDBEssence->shop_price;
	m_iProcType		= m_pDBEssence->proc_type;
	m_i64EquipMask	= 0;
	m_bUseable		= true;

	m_bNeedUpdate	= false;
}

CECIvtrFacePill::CECIvtrFacePill(const CECIvtrFacePill& s) : CECIvtrItem(s)
{
	m_pDBEssence	= s.m_pDBEssence;
	m_pDBMajorType	= s.m_pDBMajorType;
	m_pDBSubType	= s.m_pDBSubType;
}

CECIvtrFacePill::~CECIvtrFacePill()
{
}

//	Set item detail information
bool CECIvtrFacePill::SetItemInfo(BYTE* pInfoData, int iDataLen)
{
	CECIvtrItem::SetItemInfo(pInfoData, iDataLen);
	return true;
}

//	Get item icon file name
const char* CECIvtrFacePill::GetIconFile()
{
	return m_pDBEssence->file_icon;
}

//	Get item name
const wchar_t* CECIvtrFacePill::GetName()
{
	return m_pDBEssence->name;
}

//	Get item cool time
int CECIvtrFacePill::GetCoolTime(int* piMax/* NULL */)
{
	CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
	return pHost ? pHost->GetCoolTime(GP_CT_FACEPILL, piMax) : 0;
}

//	Get item description text
const wchar_t* CECIvtrFacePill::GetNormalDesc(bool bRepair)
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

	//	Remain time
	AddDescText(white, true, pDescTab->GetWideString(ITEMDESC_VALIDTIME), m_pDBEssence->duration);

	//	Profession requirement
	AddProfReqDesc(m_pDBEssence->character_combo_id);

	//	Price
	AddPriceDesc(white, bRepair);

	//	Extend description
	AddExtDescText();

	return m_strDesc;
}

//	Get pill file name
const char* CECIvtrFacePill::GetPillFile(int iProfession, int iGender)
{
	if (!m_pDBEssence)
	{
		ASSERT(m_pDBEssence);
		return NULL;
	}

	int iIndex = iProfession * NUM_GENDER + iGender;
	ASSERT(iIndex < sizeof(m_pDBEssence->pllfiles)/sizeof(m_pDBEssence->pllfiles[0]));
	return iIndex < sizeof(m_pDBEssence->pllfiles)/sizeof(m_pDBEssence->pllfiles[0])
		? m_pDBEssence->pllfiles[iIndex].file
		: "";
}

//	Check item use condition
bool CECIvtrFacePill::CheckUseCondition()
{
	CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();

	if (!(m_pDBEssence->character_combo_id & (1 << pHost->GetProfession())))
		return false;

	return true;
}

//	Get drop model for shown
const char * CECIvtrFacePill::GetDropModel()
{
	return m_pDBEssence->file_matter;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECIvtrGMGenerator
//	
///////////////////////////////////////////////////////////////////////////

CECIvtrGMGenerator::CECIvtrGMGenerator(int tid, int expire_date) : CECIvtrItem(tid, expire_date)
{
	m_iCID	= ICID_GM_GENERATOR;

	//	Get database data
	elementdataman* pDB = g_pGame->GetElementDataMan();
	DATA_TYPE DataType;
	m_pDBEssence	= (GM_GENERATOR_ESSENCE*)pDB->get_data_ptr(tid, ID_SPACE_ESSENCE, DataType);
	m_pDBType		= (GM_GENERATOR_TYPE*)pDB->get_data_ptr(m_pDBEssence->id_type, ID_SPACE_ESSENCE, DataType);

	m_iPileLimit	= m_pDBEssence->pile_num_max;
	m_iPrice		= 0;
	m_iShopPrice	= 0;
	m_iProcType		= m_pDBEssence->proc_type;
	m_i64EquipMask	= 0;
	m_bUseable		= true;

	m_bNeedUpdate	= false;
}

CECIvtrGMGenerator::CECIvtrGMGenerator(const CECIvtrGMGenerator& s) : CECIvtrItem(s)
{
	m_pDBEssence	= s.m_pDBEssence;
	m_pDBType		= s.m_pDBType;
}

CECIvtrGMGenerator::~CECIvtrGMGenerator()
{
}

//	Set item detail information
bool CECIvtrGMGenerator::SetItemInfo(BYTE* pInfoData, int iDataLen)
{
	CECIvtrItem::SetItemInfo(pInfoData, iDataLen);
	return true;
}

//	Get item icon file name
const char* CECIvtrGMGenerator::GetIconFile()
{
	return m_pDBEssence->file_icon;
}

//	Get item name
const wchar_t* CECIvtrGMGenerator::GetName()
{
	return m_pDBEssence->name;
}

//	Get item cool time
int CECIvtrGMGenerator::GetCoolTime(int* piMax/* NULL */)
{
	CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
	return pHost ? pHost->GetCoolTime(GP_CT_GM_GENITEM, piMax) : 0;
}

//	Get item description text
const wchar_t* CECIvtrGMGenerator::GetNormalDesc(bool bRepair)
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

//	Check item use condition
bool CECIvtrGMGenerator::CheckUseCondition()
{
	CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
	return pHost->IsGM();
}

//	Get drop model for shown
const char * CECIvtrGMGenerator::GetDropModel()
{
	return m_pDBEssence->file_matter;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECIvtrTankCallin
//	
///////////////////////////////////////////////////////////////////////////

CECIvtrTankCallin::CECIvtrTankCallin(int tid, int expire_date) : CECIvtrItem(tid, expire_date)
{
	m_iCID	= ICID_TANKCALLIN;

	//	Get database data
	elementdataman* pDB = g_pGame->GetElementDataMan();
	DATA_TYPE DataType;
	m_pDBEssence	= (WAR_TANKCALLIN_ESSENCE*)pDB->get_data_ptr(tid, ID_SPACE_ESSENCE, DataType);

	m_iPileLimit	= m_pDBEssence->pile_num_max;
	m_iPrice		= m_pDBEssence->price;
	m_iShopPrice	= m_pDBEssence->shop_price;
	m_iProcType		= m_pDBEssence->proc_type;
	m_i64EquipMask	= 0;
	m_bUseable		= true;
	m_dwUseFlags	= USE_PERSIST | USE_TARGET;

	m_bNeedUpdate	= false;
}

CECIvtrTankCallin::CECIvtrTankCallin(const CECIvtrTankCallin& s) : CECIvtrItem(s)
{
	m_pDBEssence	= s.m_pDBEssence;
}

CECIvtrTankCallin::~CECIvtrTankCallin()
{
}

//	Set item detail information
bool CECIvtrTankCallin::SetItemInfo(BYTE* pInfoData, int iDataLen)
{
	CECIvtrItem::SetItemInfo(pInfoData, iDataLen);
	return true;
}

//	Get item icon file name
const char* CECIvtrTankCallin::GetIconFile()
{
	return m_pDBEssence->file_icon;
}

//	Get item name
const wchar_t* CECIvtrTankCallin::GetName()
{
	return m_pDBEssence->name;
}

//	Check item use condition
bool CECIvtrTankCallin::CheckUseCondition()
{
	CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
	if (pHost->GetBattleCamp() == GP_BATTLE_CAMP_NONE)
		return false;

	int idTarget = pHost->GetSelectedTarget();
	if (!ISNPCID(idTarget))
		return false;
	
	CECNPC* pNPC = g_pGame->GetGameRun()->GetWorld()->GetNPCMan()->GetNPC(idTarget);
	if (!pNPC || !pNPC->IsMonsterNPC())
		return false;

	if (!pHost->InSameBattleCamp(pNPC))
		return false;

	return true;
}

//	Get item description text
const wchar_t* CECIvtrTankCallin::GetNormalDesc(bool bRepair)
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
const char * CECIvtrTankCallin::GetDropModel()
{
	return m_pDBEssence->file_matter;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECIvtrShopToken
//	
///////////////////////////////////////////////////////////////////////////

CECIvtrShopToken::CECIvtrShopToken(int tid, int expire_date) : CECIvtrItem(tid, expire_date)
{
	m_iCID	= ICID_SHOPTOKEN;

	//	Get database data
	elementdataman* pDB = g_pGame->GetElementDataMan();
	DATA_TYPE DataType;
	m_pDBEssence	= (SHOP_TOKEN_ESSENCE*)pDB->get_data_ptr(tid, ID_SPACE_ESSENCE, DataType);

	m_iPileLimit	= m_pDBEssence->pile_num_max;
	m_iPrice		= m_pDBEssence->price;
	m_iShopPrice	= m_pDBEssence->shop_price;
	m_iProcType		= m_pDBEssence->proc_type;
	m_i64EquipMask	= 0;
	m_bUseable		= true;
	m_dwUseFlags	= USE_PERSIST;

	m_bNeedUpdate	= false;
}

CECIvtrShopToken::CECIvtrShopToken(const CECIvtrShopToken& s) : CECIvtrItem(s)
{
	m_pDBEssence	= s.m_pDBEssence;
}

CECIvtrShopToken::~CECIvtrShopToken()
{
}

//	Set item detail information
bool CECIvtrShopToken::SetItemInfo(BYTE* pInfoData, int iDataLen)
{
	CECIvtrItem::SetItemInfo(pInfoData, iDataLen);
	return true;
}

//	Get item icon file name
const char* CECIvtrShopToken::GetIconFile()
{
	return m_pDBEssence->file_icon;
}

//	Get item name
const wchar_t* CECIvtrShopToken::GetName()
{
	return m_pDBEssence->name;
}

//	Check item use condition
bool CECIvtrShopToken::CheckUseCondition()
{
	if (!m_pDBEssence)
		return false;
	
	return true;
}

//	Get item description text
const wchar_t* CECIvtrShopToken::GetNormalDesc(bool bRepair)
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
const char * CECIvtrShopToken::GetDropModel()
{
	return m_pDBEssence->file_matter;
}
