/*
 * FILE: EC_IvtrItem.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/11/18
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "EC_Global.h"
#include "EC_IvtrItem.h"
#include "EC_IvtrWeapon.h"
#include "EC_IvtrArrow.h"
#include "EC_IvtrArmor.h"
#include "EC_IvtrFashion.h"
#include "EC_IvtrDecoration.h"
#include "EC_IvtrFlySword.h"
#include "EC_IvtrMaterial.h"
#include "EC_IvtrConsume.h"
#include "EC_IvtrScroll.h"
#include "EC_IvtrTaskItem.h"
#include "EC_IvtrStone.h"
#include "EC_IvtrPetItem.h"
#include "EC_IvtrDestroyingEssence.h"
#include "EC_IvtrEquipMatter.h"
#include "EC_IvtrGoblin.h"
#include "EC_IvtrMoneyConvertible.h"
#include "EC_Game.h"
#include "../CElementClient/EC_FixedMsg.h"

#include "AAssist.h"
#include "ATime.h"
#include "AChar.h"

#include "../CCommon/elementdataman.h"
#include "../CCommon/itemdataman.h"
#include "../CElementClient/EC_RoleTypes.h"

#include <time.h>

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
//	Implement CECIvtrItem
//	
///////////////////////////////////////////////////////////////////////////

CECIvtrItem::CECIvtrItem(int tid, int expire_date)
{
	m_iCID			= ICID_ITEM;
	m_tid			= tid;
	m_expire_date	= expire_date;
	m_iCount		= 0;
	m_iPileLimit	= 1;
	m_iPrice		= 1;
	m_iShopPrice	= 1;
	m_bNeedUpdate	= true;
	m_bUpdating		= false;
	m_dwUptTime		= 0;
	m_i64EquipMask	= 0;
	m_bEmbeddable	= false;
	m_bUseable		= false;
	m_bFrozen		= false;
	m_bNetFrozen	= false;
	m_iProcType		= 0;
	m_dwUseFlags	= 0;
	m_fPriceScale	= PLAYER_PRICE_SCALE;
	m_iScaleType	= SCALE_SELL;
	m_pDescIvtr		= NULL;
	m_bIsInNPCPack	= false;
	m_bLocalDetailData = false;
}

CECIvtrItem::CECIvtrItem(const CECIvtrItem& s)
{
	m_iCID			= s.m_iCID;
	m_tid			= s.m_tid;
	m_expire_date	= s.m_expire_date;
	m_iCount		= s.m_iCount;
	m_iPileLimit	= s.m_iPileLimit;
	m_iPrice		= s.m_iPrice;
	m_iShopPrice	= s.m_iShopPrice;
	m_i64EquipMask	= s.m_i64EquipMask;
	m_bEmbeddable	= s.m_bEmbeddable;
	m_bUseable		= s.m_bUseable;
	m_bFrozen		= false;
	m_bNetFrozen	= false;
	m_dwUseFlags	= s.m_dwUseFlags;
	m_iScaleType	= s.m_iScaleType;
	m_fPriceScale	= s.m_fPriceScale;
	m_bNeedUpdate	= s.m_bNeedUpdate;
	m_iProcType		= s.m_iProcType;
	m_pDescIvtr		= s.m_pDescIvtr;
	m_bUpdating		= false;
	m_dwUptTime		= 0;
	m_bIsInNPCPack	= s.m_bIsInNPCPack;
	m_bLocalDetailData = s.m_bLocalDetailData;
}

//	Create an inventory item
CECIvtrItem* CECIvtrItem::CreateItem(int tid, int expire_date, int iCount, int idSpace/* 0 */)
{
	CECIvtrItem* pItem = NULL;

	//	Get item data type
	DATA_TYPE DataType = g_pGame->GetElementDataMan()->get_data_type(tid, (ID_SPACE)idSpace);

	switch (DataType)
	{
	case DT_WEAPON_ESSENCE:			pItem = new CECIvtrWeapon(tid, expire_date);			break;
	case DT_PROJECTILE_ESSENCE:		pItem = new CECIvtrArrow(tid, expire_date);			break;
	case DT_ARMOR_ESSENCE:			pItem = new CECIvtrArmor(tid, expire_date);			break;
	case DT_DECORATION_ESSENCE:		pItem = new CECIvtrDecoration(tid, expire_date);		break;
	case DT_FASHION_ESSENCE:		pItem = new CECIvtrFashion(tid, expire_date);		break;
	case DT_MEDICINE_ESSENCE:		pItem = new CECIvtrMedicine(tid, expire_date);		break;
	case DT_MATERIAL_ESSENCE:		pItem = new CECIvtrMaterial(tid, expire_date);		break;
	case DT_DAMAGERUNE_ESSENCE:		pItem = new CECIvtrDmgRune(tid, expire_date);		break;
	case DT_ARMORRUNE_ESSENCE:		pItem = new CECIvtrArmorRune(tid, expire_date);		break;
	case DT_SKILLTOME_ESSENCE:		pItem = new CECIvtrSkillTome(tid, expire_date);		break;
	case DT_FLYSWORD_ESSENCE:		pItem = new CECIvtrFlySword(tid, expire_date);		break;
	case DT_TOWNSCROLL_ESSENCE:		pItem = new CECIvtrTownScroll(tid, expire_date);		break;
	case DT_UNIONSCROLL_ESSENCE:	pItem = new CECIvtrUnionScroll(tid, expire_date);	break;
	case DT_REVIVESCROLL_ESSENCE:	pItem = new CECIvtrRevScroll(tid, expire_date);		break;
	case DT_ELEMENT_ESSENCE:		pItem = new CECIvtrElement(tid, expire_date);		break;
	case DT_TOSSMATTER_ESSENCE:		pItem = new CECIvtrTossMat(tid, expire_date);		break;
	case DT_TASKMATTER_ESSENCE:		pItem = new CECIvtrTaskItem(tid, expire_date);		break;
	case DT_STONE_ESSENCE:			pItem = new CECIvtrStone(tid, expire_date);			break;
	case DT_WINGMANWING_ESSENCE:	pItem = new CECIvtrWing(tid, expire_date);			break;
	case DT_TASKDICE_ESSENCE:		pItem = new CECIvtrTaskDice(tid, expire_date);		break;
	case DT_TASKNORMALMATTER_ESSENCE:	pItem = new CECIvtrTaskNmMatter(tid, expire_date);	break;
	case DT_FACETICKET_ESSENCE:		pItem = new CECIvtrFaceTicket(tid, expire_date);		break;
	case DT_FACEPILL_ESSENCE:		pItem = new CECIvtrFacePill(tid, expire_date);		break;
	case DT_GM_GENERATOR_ESSENCE:	pItem = new CECIvtrGMGenerator(tid, expire_date);	break;
	case DT_RECIPE_ESSENCE:			pItem = new CECIvtrRecipe(tid, expire_date);			break;
	case DT_PET_EGG_ESSENCE:		pItem = new CECIvtrPetEgg(tid, expire_date);			break;
	case DT_PET_FOOD_ESSENCE:		pItem = new CECIvtrPetFood(tid, expire_date);		break;
	case DT_PET_FACETICKET_ESSENCE:	pItem = new CECIvtrPetFaceTicket(tid, expire_date);	break;
	case DT_FIREWORKS_ESSENCE:		pItem = new CECIvtrFirework(tid, expire_date);		break;
	case DT_WAR_TANKCALLIN_ESSENCE:	pItem = new CECIvtrTankCallin(tid, expire_date);		break;
	case DT_SKILLMATTER_ESSENCE:	pItem = new CECIvtrSkillMat(tid, expire_date);		break;
	case DT_INC_SKILL_ABILITY_ESSENCE: pItem = new CECIvtrIncSkillAbility(tid, expire_date); break;
	case DT_REFINE_TICKET_ESSENCE:	pItem = new CECIvtrRefineTicket(tid, expire_date);	break;
	case DT_DESTROYING_ESSENCE:		pItem = new CECIvtrDestroyingEssence(tid, expire_date); break;
	case DT_BIBLE_ESSENCE:			pItem = new CECIvtrBible(tid, expire_date); break;
	case DT_SPEAKER_ESSENCE:		pItem = new CECIvtrSpeaker(tid, expire_date); break;
	case DT_AUTOHP_ESSENCE:			pItem = new CECIvtrAutoHP(tid, expire_date); break;
	case DT_AUTOMP_ESSENCE:			pItem = new CECIvtrAutoMP(tid, expire_date); break;
	case DT_DOUBLE_EXP_ESSENCE:		pItem = new CECIvtrDoubleExp(tid, expire_date); break;
	case DT_DYE_TICKET_ESSENCE:		pItem = new CECIvtrDyeTicket(tid, expire_date); break;
	case DT_TRANSMITSCROLL_ESSENCE:	pItem = new CECIvtrTransmitScroll(tid, expire_date); break;
	case DT_GOBLIN_ESSENCE:			pItem = new CECIvtrGoblin(tid, expire_date);		break;
	case DT_GOBLIN_EQUIP_ESSENCE:	pItem = new CECIvtrGoblinEquip(tid, expire_date);	break;
	case DT_GOBLIN_EXPPILL_ESSENCE:	pItem = new CECIvtrGoblinExpPill(tid, expire_date);	break;
	case DT_SELL_CERTIFICATE_ESSENCE: pItem = new CECIvtrCertificate(tid, expire_date); break;
	case DT_TARGET_ITEM_ESSENCE:	pItem = new CECIvtrTargetItem(tid, expire_date);	break;
	case DT_LOOK_INFO_ESSENCE:		pItem = new CECIvtrLookInfoItem(tid, expire_date);	break;
	case DT_WEDDING_BOOKCARD_ESSENCE:	pItem = new CECIvtrWeddingBookCard(tid, expire_date);	break;
	case DT_WEDDING_INVITECARD_ESSENCE:	pItem = new	CECIvtrWeddingInviteCard(tid, expire_date);	break;
	case DT_SHARPENER_ESSENCE:		pItem = new CECIvtrShapener(tid, expire_date);	break;
	case DT_FACTION_MATERIAL_ESSENCE:	pItem = new CECIvtrFactionMaterial(tid, expire_date);	break;
	case DT_CONGREGATE_ESSENCE:		pItem = new CECIvtrCongregate(tid, expire_date);	break;
	case DT_FORCE_TOKEN_ESSENCE:		pItem = new CECIvtrForceToken(tid, expire_date);	break;	
	case DT_DYNSKILLEQUIP_ESSENCE:	pItem = new CECIvtrDynSkillEquip(tid, expire_date); break;
	case DT_MONEY_CONVERTIBLE_ESSENCE:	pItem = new CECIvtrMoneyConvertible(tid, expire_date);	break;
	case DT_MONSTER_SPIRIT_ESSENCE:	pItem = new CECIvtrMonsterSpirit(tid, expire_date); break;
	case DT_POKER_ESSENCE:			pItem = new CECIvtrGeneralCard(tid, expire_date); break;
	case DT_POKER_DICE_ESSENCE:		pItem = new CECIvtrGeneralCardDice(tid, expire_date); break;
	case DT_SHOP_TOKEN_ESSENCE:		pItem = new CECIvtrShopToken(tid, expire_date); break;
	case DT_UNIVERSAL_TOKEN_ESSENCE:pItem = new CECIvtrUniversalToken(tid, expire_date); break;
	default: break;
	}

	if (pItem)
		pItem->SetCount(iCount);

	return pItem;
}

//	Check whether item2 is item1's candidate
bool CECIvtrItem::IsCandidate(int tid1, int tid2)
{
	elementdataman* pDB = g_pGame->GetElementDataMan();

	//	Only medicines have candidates
	DATA_TYPE DataType;
	const MEDICINE_ESSENCE* pEssence1 = (const MEDICINE_ESSENCE*)pDB->get_data_ptr(tid1, ID_SPACE_ESSENCE, DataType);
	if (DataType != DT_MEDICINE_ESSENCE)
		return false;

	const MEDICINE_ESSENCE* pEssence2 = (const MEDICINE_ESSENCE*)pDB->get_data_ptr(tid2, ID_SPACE_ESSENCE, DataType);
	if (DataType != DT_MEDICINE_ESSENCE)
		return false;

	const MEDICINE_SUB_TYPE* pSub1 = (const MEDICINE_SUB_TYPE*)pDB->get_data_ptr(pEssence1->id_sub_type, ID_SPACE_ESSENCE, DataType);
	const MEDICINE_SUB_TYPE* pSub2 = (const MEDICINE_SUB_TYPE*)pDB->get_data_ptr(pEssence2->id_sub_type, ID_SPACE_ESSENCE, DataType);
	
	return pSub1->id == pSub2->id ? true : false;
}

//	Check whether pItem2 is item1's candidate
bool CECIvtrItem::IsCandidate(int tid1, CECIvtrItem* pItem2)
{
	ASSERT(pItem2);

	elementdataman* pDB = g_pGame->GetElementDataMan();

	//	Only medicines have candidates
	if (pItem2->GetClassID() != CECIvtrItem::ICID_MEDICINE)
		return false;

	DATA_TYPE DataType;
	const MEDICINE_ESSENCE* pEssence1 = (const MEDICINE_ESSENCE*)pDB->get_data_ptr(tid1, ID_SPACE_ESSENCE, DataType);
	if (DataType != DT_MEDICINE_ESSENCE)
		return false;

	const MEDICINE_SUB_TYPE* pSub1 = (const MEDICINE_SUB_TYPE*)pDB->get_data_ptr(pEssence1->id_sub_type, ID_SPACE_ESSENCE, DataType);

	return pSub1->id == ((CECIvtrMedicine*)pItem2)->GetDBSubType()->id ? true : false;
}

//	Get scaled price of item
int CECIvtrItem::GetScaledPrice(int iUnitPrice, int iCount, int iScaleType, float fScale)
{
	if (!iCount)
		return 0;

	int iPrice = 0;

	switch (iScaleType)
	{
	case SCALE_BUY:
		
		iPrice = (int)(iUnitPrice * fScale + 0.5f);
		if (iPrice >= 1000)
			iPrice = ((iPrice + 99) / 100) * 100;
		else if (iPrice >= 100)
			iPrice = ((iPrice + 9) / 10) * 10;

		iPrice *= iCount;
		break;

	case SCALE_SELL:

		iPrice = (int)(iUnitPrice * iCount * fScale + 0.5f);
		break;

	case SCALE_BOOTH:
	case SCALE_MAKE:
		
		iPrice = iUnitPrice * iCount;
		break;

	default:
		
		iPrice = iUnitPrice * iCount;
		break;
	}
	
	return iPrice;
}

bool CECIvtrItem::IsSharpenerProperty(BYTE propertyType)
{
	return propertyType >= 100 && propertyType <= 115;
}

//	Set item detail information
bool CECIvtrItem::SetItemInfo(BYTE* pInfoData, int iDataLen)
{
	m_bNeedUpdate	= false;
	m_bUpdating		= false;
	m_strDesc = _AL("");
	return true;
}

//	Merge item amount with another same kind item
//	Return the number of item which was merged.
int CECIvtrItem::MergeItem(int tid, int iAmount)
{
	//	If item type is different or this item has been full, return.
	if (tid != m_tid || m_iCount >= m_iPileLimit)
		return 0;

	int iNumAdd = iAmount;
	if (m_iCount + iNumAdd > m_iPileLimit)
		iNumAdd = m_iPileLimit - m_iCount;

	m_iCount += iNumAdd;

	return iNumAdd;
}

//	Add item amount. Return new amount of item
int CECIvtrItem::AddAmount(int iAmount)
{
	m_iCount += iAmount;
	a_Clamp(m_iCount, 0, m_iPileLimit);
	return m_iCount;
}

//	Get item icon file name
const char* CECIvtrItem::GetIconFile()
{
	return "Unknown.dds";
}

//	Get item name
const wchar_t* CECIvtrItem::GetName()
{
	CECStringTab* pDescTab = g_pGame->GetItemDesc();
	return pDescTab->GetWideString(ITEMDESC_ERRORITEM);
}

//	Get item's detail data from server
//void CECIvtrItem::GetDetailDataFromSev(int iPack, int iSlot)
//{
//	if (!m_bNeedUpdate)
//		return;
//
//	if (m_bUpdating)
//	{
//		//	Update request has been sent
//		if (a_GetTime() < m_dwUptTime + 5000)
//			return;
//
//		//	So long time has passed, update response hasn't arrived, ask again
//	}
//
//	m_dwUptTime = a_GetTime();
//	m_bUpdating = true;
//
//	g_pGame->GetGameSession()->c2s_CmdGetItemInfo((BYTE)iPack, (BYTE)iSlot);
//}

//	Get item's detail data from local database
void CECIvtrItem::GetDetailDataFromLocal()
{
	itemdataman* pItemDataMan = g_pGame->GetItemDataMan();
	item_data* pData = (item_data*)pItemDataMan->get_item_for_sell(m_tid);
	if (pData)
		SetItemInfo((BYTE*)pData->item_content, pData->content_length);

	SetLocalProps();
	m_bLocalDetailData = true;
}

int CECIvtrItem::GetScaledPrice()
{
	int iPrice = m_iScaleType == SCALE_BUY ? m_iShopPrice : m_iPrice;
	return GetScaledPrice(iPrice, m_iCount, m_iScaleType, m_fPriceScale);
}

//	Trim the last '\r' in description string
void CECIvtrItem::TrimLastReturn()
{
	int iLen = m_strDesc.GetLength();
	if (iLen >= 2 && m_strDesc[iLen-2] == '\\' && m_strDesc[iLen-1] == 'r')
		m_strDesc.CutRight(2);
}

//	Add content to description string
void CECIvtrItem::AddDescText(int iCol, bool bRet, const ACHAR* szText, ...)
{
	CECStringTab* pDescTab = g_pGame->GetItemDesc();

	//	Add color
	if ((iCol >= ITEMDESC_COL_LIGHTBLUE && iCol <= ITEMDESC_COL_CYANINE) ||
		(iCol > ITEMDESC_COL2_START && iCol <= ITEMDESC_COL2_NULL07))
	{
		const wchar_t* szCol = pDescTab->GetWideString(iCol);
		m_strDesc += szCol;
	}

	ACHAR szLine[256];

	va_list argList;
	va_start(argList, szText);
#ifdef UNICODE
	_vsnwprintf(szLine, sizeof(szLine)/sizeof(szLine[0]), szText, argList);
#elif
	_vsnprintf(szLine, sizeof(szLine)/sizeof(szLine[0]), szText, argList);
#endif
	va_end(argList);
	
	m_strDesc += szLine;

	if (bRet)
		m_strDesc += _AL("\\r");
}

//	Add extend description to description string
void CECIvtrItem::AddExtDescText()
{
	const wchar_t* szExtDesc = g_pGame->GetItemExtDesc(m_tid);
// 	if (!szExtDesc || !szExtDesc[0])
//		return;

	m_strDesc += _AL("\\r");	

	bool bAddLine = true;
	//  Add special properties description
	CECStringTab* pDescTab = g_pGame->GetItemDesc();
	int green = ITEMDESC_COL2_BRIGHTBLUE;
	if (m_iCID != ICID_GOBLIN)		// goblin does not need to display these special properties
	{
		if ((m_iProcType & PROC_NO_USER_TRASH)
			|| !(m_iProcType & PROC_BINDING )
			&& (m_iProcType & PROC_DROPWHENDIE ||
			m_iProcType & PROC_DROPPABLE ||
			m_iProcType & PROC_SELLABLE ||
			m_iProcType & PROC_TRADEABLE ||
			m_iProcType & PROC_DISAPEAR ||
			m_iProcType & PROC_USE ||
			m_iProcType & PROC_DEADDROP ||
			m_iProcType & PROC_OFFLINE ||
			m_iProcType & PROC_UNREPAIRABLE))
		{
			bAddLine = false;
			const wchar_t* szCol = pDescTab->GetWideString(green);
			m_strDesc += szCol;
			
			if(m_iProcType & PROC_DROPWHENDIE)
			{
				m_strDesc += _AL("\\r");
				m_strDesc += pDescTab->GetWideString(ITEMDESC_DEAD_PROTECT);
			}
			if(m_iProcType & PROC_DROPPABLE)
			{
				m_strDesc += _AL("\\r");
				m_strDesc += pDescTab->GetWideString(ITEMDESC_NO_DROP);
			}
			if(m_iProcType & PROC_SELLABLE)
			{
				m_strDesc += _AL("\\r");
				m_strDesc += pDescTab->GetWideString(ITEMDESC_NO_TRADE);
			}
			if(m_iProcType & PROC_TRADEABLE)
			{
				m_strDesc += _AL("\\r");
				m_strDesc += pDescTab->GetWideString(ITEMDESC_NO_PLAYER_TRADE);
			}
			if(m_iProcType & PROC_DISAPEAR)
			{
				m_strDesc += _AL("\\r");
				m_strDesc += pDescTab->GetWideString(ITEMDESC_LEAVE_SCENE_DISAPEAR);
			}
			if(m_iProcType & PROC_USE)
			{
				m_strDesc += _AL("\\r");
				m_strDesc += pDescTab->GetWideString(ITEMDESC_USE_AFTER_PICK_UP);
			}
			if(m_iProcType & PROC_DEADDROP)
			{
				m_strDesc += _AL("\\r");
				m_strDesc += pDescTab->GetWideString(ITEMDESC_DROP_WHEN_DEAD);
			}
			if(m_iProcType & PROC_OFFLINE)
			{
				m_strDesc += _AL("\\r");
				m_strDesc += pDescTab->GetWideString(ITEMDESC_DROP_WHEN_OFFLINE);
			}
			if(m_iProcType & PROC_UNREPAIRABLE)
			{
				m_strDesc += _AL("\\r");
				m_strDesc += pDescTab->GetWideString(ITEMDESC_UNREPAIRABLE);
			}
			if(m_iProcType & PROC_NO_USER_TRASH)
			{
				m_strDesc += _AL("\\r");
				m_strDesc += pDescTab->GetWideString(ITEMDESC_NO_USER_TRASH);
			}
		}
		else
		{
			TrimLastReturn();
		}
	}
	else
	{
		TrimLastReturn();
	}


	if (!szExtDesc || !szExtDesc[0])
		return;

	//	Extend description is below special properties
	if(bAddLine)
		m_strDesc += _AL("\\r\\r");
	else
		m_strDesc += _AL("\\r");
	m_strDesc += szExtDesc;
}

//	Add price description
void CECIvtrItem::AddPriceDesc(int col, bool bRepair)
{
	if ((!IsEquipment() && bRepair) || m_iPrice == 0 || m_fPriceScale == 0.0f)
	{
		TrimLastReturn();
		return;
	}

	// use specific color for the item price
	if(ITEMDESC_COL_WHITE == col)
	{
		if( m_iPrice >= 100000000) // 100 million
			col = ITEMDESC_COL_GREEN;
		else if ( m_iPrice >= 10000000) // 10 million
			col = ITEMDESC_COL_DARKGOLD;
		else if ( m_iPrice >= 1000000) // 1 million
			col = ITEMDESC_COL_YELLOW;
	}
	
	CECStringTab* pDescTab = g_pGame->GetItemDesc();
	if (m_iScaleType == SCALE_OFFLINESHOP)
	{
		AddDescText(col, false, pDescTab->GetWideString(ITEMDESC_PRICE));

		AWString s1,s2;
		BuildPriceNumberStr((unsigned int)m_iPrice, s1);
		if (GetCount()>1)
		{
			s2 = g_pGame->GetFormattedPrice((unsigned int)m_iPrice * (__int64)GetCount());		
			AddDescText(-1, false, _AL(" %s (%s)"), s1, s2);
		}
		else
			AddDescText(-1, false, _AL(" %s"), s1);		
	}
	else if (m_iScaleType == SCALE_BOOTH || m_tid == 21652) // 21651: yinpiao 
	{
		AWString s1;
		BuildPriceNumberStr(m_iPrice, s1);

		AddDescText(col, false, pDescTab->GetWideString(ITEMDESC_UNITPRICE));
		AddDescText(-1, false, _AL(" %s"), s1);
	}
	else if (m_iScaleType == SCALE_SELL && m_iCount > 1 && m_tid != 21652)
	{
		AWString s1, s2;
		BuildPriceNumberStr(m_iPrice, s1);
		BuildPriceNumberStr(GetScaledPrice(), s2);

		AddDescText(col, false, pDescTab->GetWideString(ITEMDESC_PRICE));
		AddDescText(-1, false, _AL(" %s (%s)"), s1, s2);
	} 
	else
	{
		AWString s1;
		BuildPriceNumberStr(GetScaledPrice(), s1);

		AddDescText(col, false, pDescTab->GetWideString(ITEMDESC_PRICE));
		AddDescText(-1, false, _AL(" %s"), s1);
	}
}

//	Add profession requirment description
void CECIvtrItem::AddProfReqDesc(int iProfReq)
{
	static int iAllProf = (1 << NUM_PROFESSION) - 1;
	if ((iProfReq & iAllProf) == iAllProf)
		return;	//	All profession permit equirement

	CECStringTab* pDescTab = g_pGame->GetItemDesc();

	int col = ITEMDESC_COL_WHITE;
	AddDescText(col, false, pDescTab->GetWideString(ITEMDESC_PROFESSIONREQ));

	for (int i=0; i < NUM_PROFESSION; i++)
	{
		if (iProfReq & (1 << i))
		{
			m_strDesc += _AL(" ");
			AddDescText(col, false, g_pGame->GetProfName(i));
		}
	}

	m_strDesc += _AL("\\r");
}

//	Get item description text for rewarding
const wchar_t* CECIvtrItem::GetRewardDesc()
{
	CECStringTab* pDescTab = g_pGame->GetItemDesc();
	int white = ITEMDESC_COL_WHITE;

	//	Name
	m_strDesc = _AL("");
	AddDescText(white, false, pDescTab->GetWideString(ITEMDESC_NAME), GetName());
	
	//	Extend description
	AddExtDescText();

	return m_strDesc;
}

void CECIvtrItem::AddExpireTimeDesc()
{
	if( m_expire_date == 0 )
		return;

	int green = ITEMDESC_COL_GREEN;
	int yellow = ITEMDESC_COL_YELLOW;
	int gold = ITEMDESC_COL_DARKGOLD;
	int red = ITEMDESC_COL_RED;
	tm t = *localtime((time_t*)&m_expire_date);

	// 过期时间：*年*月*日*时*分*秒
	AddDescText(ITEMDESC_COL_WHITE, true, g_pGame->GetStringFromTable(10),
		t.tm_year+1900,
		t.tm_mon+1,
		t.tm_mday,
		t.tm_hour,
		t.tm_min,
		t.tm_sec);
}

void CECIvtrItem::AddExpireTimeDesc(int expire_date)
{
	int temp = m_expire_date;
	m_expire_date = expire_date;
	AddExpireTimeDesc();
	m_expire_date = temp;
}


//  Add item id desc
void CECIvtrItem::AddIDDescText()
{
	//  Show item id
//	if (g_pGame->GetConfigs()->GetShowIDFlag())
//	{
//		AddDescText(ITEMDESC_COL_GREEN, true, _AL("ID: %d"), m_tid);
// 	}
}

void CECIvtrItem::AddBindDescText()
{
	CECStringTab* pDescTab = g_pGame->GetItemDesc();
	if (m_iProcType & PROC_BINDING)
	{
		ACString strTemp = pDescTab->GetWideString(ITEMDESC_EQUIP_BIND);
		if (m_iProcType & PROC_CAN_WEBTRADE)
		{
			strTemp += _AL("(");
			strTemp += pDescTab->GetWideString(ITEMDESC_CAN_WEBTRADE);
			strTemp += _AL(")");
		}
		AddDescText(ITEMDESC_COL_CYANINE, true, strTemp);
	}
	else if (m_iProcType & PROC_BIND)
		AddDescText(ITEMDESC_COL_DARKGOLD, true, pDescTab->GetWideString(ITEMDESC_EQUIP_BINDAFTERUSE));
}

//	Build price number string
void CECIvtrItem::BuildPriceNumberStr(int iPrice, AWString& str)
{
	str = g_pGame->GetFormattedPrice(iPrice);
}

//	Build price number string
void CECIvtrItem::BuildPriceNumberStr(unsigned int iPrice, AWString& str)
{
	str = g_pGame->GetFormattedPrice(iPrice);
}

const char * CECIvtrItem::GetDropModel()
{ 
	return "Models\\Error\\Error.ecm"; 
}

//	Decide equipment name color
int CECIvtrItem::DecideNameCol()
{
	int iIndex = GetColorStrID(m_tid);
	return iIndex >= 0 ? iIndex : ITEMDESC_COL_WHITE;
}

//	Get item's color string id
int CECIvtrItem::GetColorStrID(int tid)
{
	int iIndex = g_pGame->GetItemNameColorIdx(m_tid);
	if (iIndex <= 0)
		return -1;
	else if (iIndex < 7)
		return ITEMDESC_COL_LIGHTBLUE + iIndex - 1;
	else
		return ITEMDESC_COL2_START + iIndex - 7 + 1;
}

//  Can this item be put to account box ?
bool CECIvtrItem::CanPutIntoAccBox() const
{
	if(m_iProcType & PROC_NO_USER_TRASH)
		return false;

	elementdataman* pDB = g_pGame->GetElementDataMan();

	DATA_TYPE DataType;
	const ACC_STORAGE_BLACKLIST_CONFIG* pConfig = (const ACC_STORAGE_BLACKLIST_CONFIG*)pDB->get_data_ptr(696, ID_SPACE_CONFIG, DataType);
	if (!pConfig || DataType != DT_ACC_STORAGE_BLACKLIST_CONFIG)
		return false;

	for(int i=0;i<512;i++)
	{
		if(m_tid == (int)pConfig->blacklist[i])
			return false;
	}

	return true;

}

void CECIvtrItem::AddActionTypeDescText(int action_type)
{
	CECStringTab* pDescTab = g_pGame->GetItemDesc();
	AddDescText(ITEMDESC_COL_WHITE, false, pDescTab->GetWideString(ITEMDESC_TYPE));
	m_strDesc += _AL(" ");
	static const int WEAPON_TYPE_NAME_INDEX[NUM_WEAPON_TYPE] = {
		ITEMDESC_ONE_HANDED_WEAPON,			
		ITEMDESC_TWO_HANDED_WEAPON,	
		ITEMDESC_TWO_HANDED_POLEARM,
		ITEMDESC_TWO_HANDED_HEAVY,
		ITEMDESC_TWO_HANDED_POLEHEAVY,
		ITEMDESC_WHIP,		
		ITEMDESC_BOW,
		ITEMDESC_CROSSBOW,
		ITEMDESC_BOXING_GLOVES,
		ITEMDESC_PIKE,
		ITEMDESC_EMPTY_HANDED,
		ITEMDESC_DAGGER,
		ITEMDESC_TALISMAN,
		ITEMDESC_OBOROKNIFE_WEAPON,
		ITEMDESC_SICKLE_WEAPON,
	};
	AddDescText(ITEMDESC_COL_WHITE, false, pDescTab->GetWideString(WEAPON_TYPE_NAME_INDEX[action_type]));	
	m_strDesc += _AL("\\r");
}
///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECIvtrUnknown
//	
///////////////////////////////////////////////////////////////////////////

CECIvtrUnknown::CECIvtrUnknown(int tid) : CECIvtrItem(tid, 0)
{
	m_iCID = ICID_ERRORITEM;

	m_bNeedUpdate	= false;
	m_bUpdating		= false;
}

CECIvtrUnknown::CECIvtrUnknown(const CECIvtrUnknown& s) : CECIvtrItem(s)
{
}

//	Get item icon file name
const char* CECIvtrUnknown::GetIconFile()
{
	return "Unknown.dds";
}

//	Get item name
const wchar_t* CECIvtrUnknown::GetName()
{
	CECStringTab* pDescTab = g_pGame->GetItemDesc();
	return pDescTab->GetWideString(ITEMDESC_ERRORITEM);
}

//	Get item description text
const wchar_t* CECIvtrUnknown::GetNormalDesc(bool bRepair)
{
	m_strDesc = _AL("");
	CECStringTab* pDescTab = g_pGame->GetItemDesc();
	AddDescText(ITEMDESC_COL_WHITE, false, pDescTab->GetWideString(ITEMDESC_ERRORITEM));
	AddDescText(ITEMDESC_COL_WHITE, false, _AL("(%d)"), m_tid);
	return m_strDesc;
}

