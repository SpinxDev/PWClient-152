/*
 * FILE: EC_IvtrFlySword.cpp
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
#include "EC_IvtrFlySword.h"
#include "EC_Game.h"
#include "EC_FixedMsg.h"
#include "EC_GameRun.h"
#include "EC_HostPlayer.h"
#include "EC_RTDebug.h"
#include "elementdataman.h"
#include "EC_Configs.h"
#include "EC_UIConfigs.h"

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
//	Implement CECIvtrFlySword
//	
///////////////////////////////////////////////////////////////////////////

CECIvtrFlySword::CECIvtrFlySword(int tid, int expire_date) : CECIvtrEquip(tid, expire_date)
{
	m_iCID	= ICID_FLYSWORD;

	//	Get database data
	elementdataman* pDB = g_pGame->GetElementDataMan();
	DATA_TYPE DataType;
	m_pDBEssence = (FLYSWORD_ESSENCE*)pDB->get_data_ptr(tid, ID_SPACE_ESSENCE, DataType);

	memset(&m_Essence, 0, sizeof(m_Essence));
	m_iPileLimit	= m_pDBEssence->pile_num_max;
	m_iPrice		= m_pDBEssence->price;
	m_iShopPrice	= m_pDBEssence->shop_price;
	m_iProcType		= m_pDBEssence->proc_type;
	m_i64EquipMask	= EQUIP_MASK64_FLYSWORD;
	m_iCurTime		= 0;
}

CECIvtrFlySword::CECIvtrFlySword(const CECIvtrFlySword& s) : CECIvtrEquip(s)
{
	m_iCurTime		= s.m_iCurTime;
	m_Essence		= s.m_Essence;
	m_pDBEssence	= s.m_pDBEssence;
	m_byMadeFrom	= s.m_byMadeFrom;
	m_strMaker		= s.GetMakerName();
}

CECIvtrFlySword::~CECIvtrFlySword()
{
}

//	Set item detail information
bool CECIvtrFlySword::SetItemInfo(BYTE* pInfoData, int iDataLen)
{
	//	Note: because fly sword isn't an absolute equipment, so skip
	//	CECIvtrEquip::SetItemInfo().

	if (!pInfoData || !iDataLen)
	{
		m_bNeedUpdate = false;
		return true;
	}
	
	try
	{
		CECDataReader dr(pInfoData, iDataLen);

		m_Essence = *(IVTR_ESSENCE_FLYSWORD*)dr.Read_Data(sizeof (IVTR_ESSENCE_FLYSWORD));
		ReadMakerInfo(dr);
	}
	catch (CECException& e)
	{
		ASSERT(0);
		a_LogOutput(1, "CECIvtrFlySword::SetItemInfo, data read error (%d)", e.GetType());
		return false;
	}

	m_iLevelReq		= m_Essence.require_level;
	m_bNeedUpdate	= false;
	m_iCurTime		= m_Essence.cur_time * 1000;

	return true;
}

//	Get item icon file name
const char* CECIvtrFlySword::GetIconFile()
{
	return m_pDBEssence->file_icon;
}

//	Get item name
const wchar_t* CECIvtrFlySword::GetName()
{
	return m_pDBEssence->name;
}

//	Get item description text
const wchar_t* CECIvtrFlySword::GetNormalDesc(bool bRepair)
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

	if (m_iCount > 1)
		AddDescText(namecol, true, pDescTab->GetWideString(ITEMDESC_NAMENUMBER), GetName(), m_iCount);
	else
		AddDescText(namecol, true, pDescTab->GetWideString(ITEMDESC_NAME), GetName());

	AddIDDescText();

	AddBindDescText();

	//  Is destroying?
	AddDestroyingDesc(m_pDBEssence->id_drop_after_damaged, m_pDBEssence->num_drop_after_damaged);

	AddExpireTimeDesc();

	//	level
	AddDescText(white, true, pDescTab->GetWideString(ITEMDESC_LEVEL), m_Essence.level);

	// whether can be improved
	if (IsImprovable())
		AddDescText(ITEMDESC_COL_GREEN,true,pDescTab->GetWideString(ITEMDESC_FLYSWORD_IMPROVE),m_Essence.improve_level,GetMaxImproveLevel());		
	else
		AddDescText(white,true,pDescTab->GetWideString(ITEMDESC_FLYSWORD_NOIMPROVED));		

	//	Normal fly speed bonus
	if (m_Essence.speed_increase)
		AddDescText(white, true, pDescTab->GetWideString(ITEMDESC_ADDFLYSPEED), m_Essence.speed_increase);

	//	Quick fly speed bonus
	if (m_Essence.speed_increase2)
		AddDescText(white, true, pDescTab->GetWideString(ITEMDESC_ADDFLYSPEED2), m_Essence.speed_increase2);

	//	Remain time
	if (GetMaxTime())
		AddDescText(white, true, pDescTab->GetWideString(ITEMDESC_REMAINTIME), GetCurTime(), GetMaxTime());

	//	Profession requirement
	AddProfReqDesc(m_Essence.profession);
	
	//	Level requirement
	if (m_iLevelReq)
	{
		int col = pHost->GetMaxLevelSofar() >= m_iLevelReq ? white : red;
		AddDescText(col, true, pDescTab->GetWideString(ITEMDESC_LEVELREQ), m_iLevelReq);
	}
	
	//	Element consume
//	AddDescText(white, true, pDescTab->GetWideString(ITEMDESC_ELEMENTTIME), m_Essence.time_per_element);

	//	Price
	AddPriceDesc(white, bRepair);
	
	AddMakerDesc();

	//	Suite description
	AddSuiteDesc();
	
	//	Extend description
	AddExtDescText();
	
	return m_strDesc;
}

//	Get max time
int CECIvtrFlySword::GetMaxTime()
{
	return (int)m_Essence.max_time;
}

//	Get max element number
int CECIvtrFlySword::GetMaxElement()
{
	return (int)((float)m_Essence.max_time / m_Essence.time_per_element + 0.5f);
}

//	Get time each element equal to
int CECIvtrFlySword::GetElementTime()
{
	return m_Essence.time_per_element;
}

//	Get number of element if time is filled to full. This is just the number
//	of element which has been used
int CECIvtrFlySword::GetUsedElementNum()
{
	return (int)((m_Essence.max_time - GetCurTime()) / (float)m_Essence.time_per_element);
}

//	Get drop model for shown
const char * CECIvtrFlySword::GetDropModel()
{
	return m_pDBEssence->file_matter;
}

bool CECIvtrFlySword::IsRare() const
{ 
	return CECIvtrEquip::IsRare() || m_Essence.level >= 6;
}
bool CECIvtrFlySword::IsImprovable()
{
	return GetDBEssence()->max_improve_level>0 && GetDBEssence()->improve_config[0].require_item_num>0;
}
bool CECIvtrFlySword::CanBeImproved()
{
	if(GetDBEssence()->max_improve_level <=0) return false;

	if(m_Essence.improve_level>= GetMaxImproveLevel()) return false;

	return GetDBEssence()->improve_config[m_Essence.improve_level].require_item_num > 0;
}
int CECIvtrFlySword::GetMaxImproveLevel()
{
	int maxL= sizeof(GetDBEssence()->improve_config) / sizeof(GetDBEssence()->improve_config[0]);
	int i = 0;
	for (i=0;i<maxL;i++)
	{
		if(GetDBEssence()->improve_config[i].require_item_num==0)
			break;
	}

	return min(GetDBEssence()->max_improve_level,i);
}
///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECIvtrWing
//	
///////////////////////////////////////////////////////////////////////////

CECIvtrWing::CECIvtrWing(int tid, int expire_date) : CECIvtrEquip(tid, expire_date)
{
	m_iCID	= ICID_WING;

	//	Get database data
	elementdataman* pDB = g_pGame->GetElementDataMan();
	DATA_TYPE DataType;
	m_pDBEssence = (WINGMANWING_ESSENCE*)pDB->get_data_ptr(tid, ID_SPACE_ESSENCE, DataType);

	m_iPileLimit	= m_pDBEssence->pile_num_max;
	m_iPrice		= m_pDBEssence->price;
	m_iShopPrice	= m_pDBEssence->shop_price;
	m_iProcType		= m_pDBEssence->proc_type;
	m_i64EquipMask	= EQUIP_MASK64_FLYSWORD;
}

CECIvtrWing::CECIvtrWing(const CECIvtrWing& s) : CECIvtrEquip(s)
{
	m_Essence		= s.m_Essence;
	m_pDBEssence	= s.m_pDBEssence;
	m_byMadeFrom	= s.m_byMadeFrom;
	m_strMaker		= s.GetMakerName();
}

CECIvtrWing::~CECIvtrWing()
{
}

//	Set item detail information
bool CECIvtrWing::SetItemInfo(BYTE* pInfoData, int iDataLen)
{
	//	Note: because wing isn't an absolute equipment, so skip
	//	CECIvtrEquip::SetItemInfo().

	if (!pInfoData || !iDataLen)
	{
		m_bNeedUpdate = false;
		return true;
	}

	try
	{
		CECDataReader dr(pInfoData, iDataLen);

		m_Essence = *(IVTR_ESSENCE_WING*)dr.Read_Data(sizeof (IVTR_ESSENCE_WING));
		ReadMakerInfo(dr);
	}
	catch (CECException& e)
	{
		ASSERT(0);
		a_LogOutput(1, "CECIvtrWing::SetItemInfo, data read error (%d)", e.GetType());
		return false;
	}

	m_iLevelReq		= m_Essence.require_level;
	m_bNeedUpdate	= false;

	return true;
}

//	Get item icon file name
const char* CECIvtrWing::GetIconFile()
{
	return m_pDBEssence->file_icon;
}

//	Get item name
const wchar_t* CECIvtrWing::GetName()
{
	return m_pDBEssence->name;
}

//	Get item description text
const wchar_t* CECIvtrWing::GetNormalDesc(bool bRepair)
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

	if (m_iCount > 1)
		AddDescText(namecol, true, pDescTab->GetWideString(ITEMDESC_NAMENUMBER), GetName(), m_iCount);
	else
		AddDescText(namecol, true, pDescTab->GetWideString(ITEMDESC_NAME), GetName());

	AddIDDescText();

	AddExpireTimeDesc();

	//	Fly speed bonus
	AddDescText(white, true, pDescTab->GetWideString(ITEMDESC_ADDFLYSPEED), m_Essence.speed_increase);

	//	Profession requirement
	if (1)
	{
		CECStringTab* pFixMsg = g_pGame->GetFixedMsgTab();
		int col = (pHost->GetProfession() == PROF_ARCHOR || pHost->GetProfession() == PROF_ANGEL) ? white : red;
		AddDescText(col, false, pDescTab->GetWideString(ITEMDESC_PROFESSIONREQ));
		m_strDesc += _AL(" ");
		AddDescText(col, false, g_pGame->GetGameRun()->GetProfName(PROF_ARCHOR));
		m_strDesc += _AL(" ");
		AddDescText(col, true, g_pGame->GetGameRun()->GetProfName(PROF_ANGEL));
	}

	//	Level requirment
	if (m_iLevelReq)
	{
		int col = pHost->GetMaxLevelSofar() >= m_iLevelReq ? white : red;
		AddDescText(col, true, pDescTab->GetWideString(ITEMDESC_LEVELREQ), m_iLevelReq);
	}

	//	MP consume
	AddDescText(white, true, pDescTab->GetWideString(ITEMDESC_MPCONSUME), m_Essence.mp_per_second);

	//	Price
	AddPriceDesc(white, bRepair);
	
	AddMakerDesc();

	//	Suite description
	AddSuiteDesc();

	//	Extend description
	AddExtDescText();

	return m_strDesc;
}

//	Get the MP consumed every seconds
int CECIvtrWing::GetMPPerSecond()
{
	return m_Essence.mp_per_second;
}

//	Get the launch MP cost 
int CECIvtrWing::GetLaunchMP()
{ 
	return m_Essence.mp_launch; 
}

//	Get drop model for shown
const char * CECIvtrWing::GetDropModel()
{
	return m_pDBEssence->file_matter;
}
