/*
 * FILE: EC_IvtrDecoration.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2005/1/3
 *
 * HISTORY: 
 *
 * Copyright (c) 2005 Archosaur Studio, All Rights Reserved.
 */

#include "EC_Global.h"
#include "EC_IvtrDecoration.h"
#include "EC_Game.h"
#include "../CElementClient/EC_RoleTypes.h"
#include "../CElementClient/EC_FixedMsg.h"
#include "EC_RTDebug.h"
#include "../CCommon/elementdataman.h"

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
//	Implement CECIvtrDecoration
//	
///////////////////////////////////////////////////////////////////////////

CECIvtrDecoration::CECIvtrDecoration(int tid, int expire_date) : CECIvtrEquip(tid, expire_date)
{
	m_iCID	= ICID_DECORATION;

	memset(&m_Essence, 0, sizeof (m_Essence));

	//	Get database data
	elementdataman* pDB = g_pGame->GetElementDataMan();
	DATA_TYPE DataType;
	m_pDBEssence	= (DECORATION_ESSENCE*)pDB->get_data_ptr(tid, ID_SPACE_ESSENCE, DataType);
	m_pDBMajorType	= (DECORATION_MAJOR_TYPE*)pDB->get_data_ptr(m_pDBEssence->id_major_type, ID_SPACE_ESSENCE, DataType);
	m_pDBSubType	= (DECORATION_SUB_TYPE*)pDB->get_data_ptr(m_pDBEssence->id_sub_type, ID_SPACE_ESSENCE, DataType);

	m_iPileLimit	= m_pDBEssence->pile_num_max;
	m_iPrice		= m_pDBEssence->price;
	m_iShopPrice	= m_pDBEssence->shop_price;
	m_iRepairFee	= m_pDBEssence->repairfee;
	m_i64EquipMask	= m_pDBSubType->equip_mask;
	m_iFixProps		= m_pDBEssence->fixed_props;
	m_iProcType		= m_pDBEssence->proc_type;
	m_iReputationReq= m_pDBEssence->require_reputation;
}

CECIvtrDecoration::CECIvtrDecoration(const CECIvtrDecoration& s) : CECIvtrEquip(s)
{
	m_pDBEssence	= s.m_pDBEssence;
	m_pDBMajorType	= s.m_pDBMajorType;
	m_pDBSubType	= s.m_pDBSubType;
	m_Essence		= s.m_Essence;
}

CECIvtrDecoration::~CECIvtrDecoration()
{
}

//	Set item detail information
bool CECIvtrDecoration::SetItemInfo(BYTE* pInfoData, int iDataLen)
{
	CECIvtrEquip::SetItemInfo(pInfoData, iDataLen);

	if (!pInfoData || !iDataLen)
		return true;
	
	try
	{
		CECDataReader dr(pInfoData, iDataLen);

		//	Skip equip requirements and endurance
		dr.Offset(6 * sizeof (short), SEEK_CUR);
		dr.Offset(2 * sizeof (int), SEEK_CUR);

		int iEssenceSize = dr.Read_short();
		ASSERT(iEssenceSize == sizeof (IVTR_ESSENCE_DECORATION));

		//	Skip maker's information
		dr.Read_BYTE();
		int iMakerLen = dr.Read_BYTE();
		dr.Offset(iMakerLen, SEEK_CUR);
		
		m_Essence = *(IVTR_ESSENCE_DECORATION*)dr.Read_Data(iEssenceSize);
		
		// 加入附加属性说明
		if(m_pDBEssence->fixed_props != 0 && m_pDBEssence->probability_addon_num0 != 1.0f) 
		{
			//	Get database data
			elementdataman* pDataMan = g_pGame->GetElementDataMan();
			CECGame::ItemExtPropTable& PropTab = g_pGame->GetItemExtPropTable();

			int i, iSize = 0;
			for(i=0;i<32;i++)
			{
				if(m_pDBEssence->addons[i].id_addon != 0)
					iSize ++;
			}

			if(iSize > 0 && m_aProps.GetSize() == 0)
			{
				m_aProps.SetSize(iSize, 5);
				for(i=0;i<32;i++)
				{
					if(m_pDBEssence->addons[i].id_addon != 0)
					{
						PROPERTY& Prop = m_aProps[i];
						Prop.iType = m_pDBEssence->addons[i].id_addon;
						Prop.bEmbed = false;
						Prop.bSuite = false;
						Prop.bEngraved = false;
						Prop.bLocal = true;

						CECGame::ItemExtPropTable::pair_type Pair = PropTab.get(Prop.iType);
						BYTE bType = Pair.second ? *Pair.first : 0xff;

						DATA_TYPE DataType = DT_INVALID;
						const void* pData = pDataMan->get_data_ptr(m_pDBEssence->addons[i].id_addon, ID_SPACE_ADDON, DataType);
						if (DataType != DT_EQUIPMENT_ADDON)
						{
							ASSERT(DataType == DT_EQUIPMENT_ADDON);
							return 0;
						}

						const EQUIPMENT_ADDON* pAddon = (const EQUIPMENT_ADDON*)pData;
						Prop.iNumParam = pAddon->num_params;						

						for(int j=0; j < Prop.iNumParam; j++)
						{
							if(j==0)
								Prop.aParams[0] = pAddon->param1;
							else if(j==1)
								Prop.aParams[1] = pAddon->param2;
							else if(j==2)
								Prop.aParams[2] = pAddon->param3;
						}
					}
				}
			}
		}

	}
	catch (CECException& e)
	{
		ASSERT(0);
		a_LogOutput(1, "CECIvtrDecoration::SetItemInfo, data read error (%d)", e.GetType());
		return false;
	}

	return true;
}

//	Get item default information from database
void CECIvtrDecoration::DefaultInfo()
{
	m_iLevelReq		= m_pDBEssence->require_level;
	m_iStrengthReq	= 0;
	m_iAgilityReq	= 0;
	m_iReputationReq= m_pDBEssence->require_reputation;
	m_iCurEndurance	= m_pDBEssence->durability_min * ENDURANCE_SCALE;
	m_iMaxEndurance = m_pDBEssence->durability_min * ENDURANCE_SCALE;
}

//	Get item icon file name
const char* CECIvtrDecoration::GetIconFile()
{
	return m_pDBEssence->file_icon;
}

//	Get item name
const wchar_t* CECIvtrDecoration::GetName()
{
	return m_pDBEssence->name;
}

//	Get item description text
const wchar_t* CECIvtrDecoration::GetNormalDesc(bool bRepair)
{
	if (m_bNeedUpdate)
		return NULL;

	//	Build addon and refine properties and save it
	int aPEEVals[MAX_PEEINDEX];
	int aRefines[MAX_REFINEINDEX];
	memset(aPEEVals, 0, sizeof (aPEEVals));
	memset(aRefines, 0, sizeof (aRefines));

	m_strDesc = _AL("");
	BuildAddOnPropDesc(aPEEVals, aRefines);
	AWString strAddon = m_strDesc;

	m_strDesc = _AL("");

	CECStringTab* pDescTab = g_pGame->GetItemDesc();

	int lblue = ITEMDESC_COL_LIGHTBLUE;
	int white = ITEMDESC_COL_WHITE;
	int red = ITEMDESC_COL_RED;
	int namecol = DecideNameCol();

	//	Get property effect essence flags
	DWORD dwPEE = PropEffectEssence();

	//	Item name
	AddDescText(namecol, false, pDescTab->GetWideString(ITEMDESC_NAME), GetName());

	//	Refine level
	if (m_iRefineLvl)
		AddDescText(-1, true, _AL(" +%d"), m_iRefineLvl);
	else
		m_strDesc += _AL("\\r");
	
	AddIDDescText();
	
	AddBindDescText();
	
	//  Is destroying?
	AddDestroyingDesc(m_pDBEssence->id_drop_after_damaged, m_pDBEssence->num_drop_after_damaged);

	AddExpireTimeDesc();

	//	Sub class name
	AddDescText(white, true, pDescTab->GetWideString(ITEMDESC_CLASSNAME), m_pDBSubType->name);

	//	Decoration level
	AddDescText(-1, true, pDescTab->GetWideString(ITEMDESC_LEVEL), m_pDBEssence->level);
	
	//	Add physical damage
	if (m_Essence.damage - aPEEVals[PEEI_PHYDAMAGE] + aRefines[REFINE_PHYDAMAGE])
	{
		AddDescText(white, false, pDescTab->GetWideString(ITEMDESC_ADDPHYDAMAGE));
		AddDescText(white, true, _AL(" %+d"), m_Essence.damage - aPEEVals[PEEI_PHYDAMAGE] + aRefines[REFINE_PHYDAMAGE]);
	}

	//	Add magic damage
	if (m_Essence.magic_damage - aPEEVals[PEEI_MAGICDAMAGE] + aRefines[REFINE_MAGICDAMAGE])
	{
		AddDescText(white, false, pDescTab->GetWideString(ITEMDESC_ADDMAGICDAMAGE));
		AddDescText(white, true, _AL(" %+d"), m_Essence.magic_damage - aPEEVals[PEEI_MAGICDAMAGE] + aRefines[REFINE_MAGICDAMAGE]);
	}

	//	Add physical defence
	if (m_Essence.defense - aPEEVals[PEEI_PHYDEF] + aRefines[REFINE_PHYDEF])
	{
		AddDescText(white, false, pDescTab->GetWideString(ITEMDESC_PHYDEFENCE));
		AddDescText(white, true, _AL(" %+d"), m_Essence.defense - aPEEVals[PEEI_PHYDEF] + aRefines[REFINE_PHYDEF]);
	}

	//	Add dodge
	if (m_Essence.armor - aPEEVals[PEEI_DODGE] + aRefines[REFINE_DODGE])
	{
		AddDescText(white, false, pDescTab->GetWideString(ITEMDESC_DODGE));
		AddDescText(white, true, _AL(" %+d"), m_Essence.armor - aPEEVals[PEEI_DODGE] + aRefines[REFINE_DODGE]);
	}

	if (m_Essence.resistance[MAGICCLASS_GOLD] - aPEEVals[PEEI_GOLDDEF] + aRefines[REFINE_GOLDDEF])
	{
		AddDescText(white, false, pDescTab->GetWideString(ITEMDESC_GOLDDEFENCE));
		AddDescText(white, true, _AL(" %+d"), m_Essence.resistance[MAGICCLASS_GOLD] - aPEEVals[PEEI_GOLDDEF] + aRefines[REFINE_GOLDDEF]);
	}

	if (m_Essence.resistance[MAGICCLASS_WOOD] - aPEEVals[PEEI_WOODDEF] + aRefines[REFINE_WOODDEF])
	{
		AddDescText(white, false, pDescTab->GetWideString(ITEMDESC_WOODDEFENCE));
		AddDescText(white, true, _AL(" %+d"), m_Essence.resistance[MAGICCLASS_WOOD] - aPEEVals[PEEI_WOODDEF] + aRefines[REFINE_WOODDEF]);
	}

	if (m_Essence.resistance[MAGICCLASS_WATER] - aPEEVals[PEEI_WATERDEF] + aRefines[REFINE_WATERDEF])
	{
		AddDescText(white, false, pDescTab->GetWideString(ITEMDESC_WATERDEFENCE));
		AddDescText(white, true, _AL(" %+d"), m_Essence.resistance[MAGICCLASS_WATER] - aPEEVals[PEEI_WATERDEF] + aRefines[REFINE_WATERDEF]);
	}

	if (m_Essence.resistance[MAGICCLASS_FIRE] - aPEEVals[PEEI_FIREDEF] + aRefines[REFINE_FIREDEF])
	{
		AddDescText(white, false, pDescTab->GetWideString(ITEMDESC_FIREDEFENCE));
		AddDescText(white, true, _AL(" %+d"), m_Essence.resistance[MAGICCLASS_FIRE] - aPEEVals[PEEI_FIREDEF] + aRefines[REFINE_FIREDEF]);
	}

	if (m_Essence.resistance[MAGICCLASS_EARTH] - aPEEVals[PEEI_EARTHDEF] + aRefines[REFINE_EARTHDEF])
	{
		AddDescText(white, false, pDescTab->GetWideString(ITEMDESC_EARTHDEFENCE));
		AddDescText(white, true, _AL(" %+d"), m_Essence.resistance[MAGICCLASS_EARTH] - aPEEVals[PEEI_EARTHDEF] + aRefines[REFINE_EARTHDEF]);
	}

	//	Endurance
	int col = white;
	if (!m_iCurEndurance)
		col = red;
	else if (dwPEE & PEE_ENDURANCE)
		col = lblue;

	AddDescText(col, false, pDescTab->GetWideString(ITEMDESC_ENDURANCE));
	AddDescText(col, true, _AL(" %d/%d"), VisualizeEndurance(m_iCurEndurance), VisualizeEndurance(m_iMaxEndurance));
	
	//	Profession requirement
	AddProfReqDesc(m_iProfReq);

	//	Level requirment
	if (m_iLevelReq)
	{
		col = white;
		AddDescText(col, true, pDescTab->GetWideString(ITEMDESC_LEVELREQ), m_iLevelReq);
	}
	
	//	Strength requirment
	if (m_iStrengthReq)
	{
		col = (dwPEE & PEE_STRENGTHREQ) ? lblue : white;
		
		AddDescText(col, true, pDescTab->GetWideString(ITEMDESC_STRENGTHREQ), m_iStrengthReq);
	}

	//	Agility requirment
	if (m_iAgilityReq)
	{
		col = (dwPEE & PEE_AGILITYREQ) ? lblue : white;

		AddDescText(col, true, pDescTab->GetWideString(ITEMDESC_AGILITYREQ), m_iAgilityReq);
	}

	//	Vitality requirment
	if (m_iVitalityReq)
	{
		col = (dwPEE & PEE_VITALITYREQ) ? lblue : white;

		AddDescText(col, true, pDescTab->GetWideString(ITEMDESC_VITALITYREQ), m_iVitalityReq);
	}

	//	Energy requirment
	if (m_iEnergyReq)
	{
		col = (dwPEE & PEE_ENERGYREQ) ? lblue : white;

		AddDescText(col, true, pDescTab->GetWideString(ITEMDESC_ENERGYREQ), m_iEnergyReq);
	}

	// Reputation requirement
	AddReputationReqDesc();

	//	Add addon properties
	if (strAddon.GetLength())
		m_strDesc += strAddon;

	if(m_pDBEssence->fixed_props == 0 && m_bIsInNPCPack)
	{
		//  添加 "附加属性随机" 说明
		AddDescText(ITEMDESC_COL2_BRIGHTBLUE, true, pDescTab->GetWideString(ITEMDESC_HASRANDOM_PROP));
	}

	//	Price
	AddPriceDesc(white, bRepair);
	
	//	磨刀石增加属性
	AddSharpenerDesc();

	//	镌刻属性
	AddEngravedDesc();
	
	AddMakerDesc();

	//	Suite description
	AddSuiteDesc();
	
	//	Extend description
	AddExtDescText();	

	return m_strDesc;
}

//	Does this equipment has random property ?
bool CECIvtrDecoration::HasRandomProp()
{
	for (int i=0; i < m_aProps.GetSize(); i++)
	{
		const PROPERTY& prop = m_aProps[i];
		if (!prop.bEmbed && !prop.bEngraved && prop.iType == 410)
			return true;
	}

	return false;
}


bool CECIvtrDecoration::HasViewProp()
{	
	CECGame::ItemExtPropTable& PropTab = g_pGame->GetItemExtPropTable();

	for (int i=0; i < m_aProps.GetSize(); i++)
	{
		const PROPERTY& prop = m_aProps[i];
		if (!prop.bEmbed &&
			!prop.bSuite &&
			!prop.bEngraved)
		{
			CECGame::ItemExtPropTable::pair_type Pair = PropTab.get(prop.iType);
			BYTE byPropType = Pair.second ? *Pair.first : 0xff;
			if (byPropType == 62)
			{
				return true;
			}
		}
	}

	return false;
}

//	Get number of material needed to refine equipment
int CECIvtrDecoration::GetRefineMaterialNum()
{
	return m_pDBEssence->material_need;
}

//	Get drop model for shown
const char * CECIvtrDecoration::GetDropModel()
{
	return m_pDBEssence->file_matter;
}

int CECIvtrDecoration::GetItemLevel() const
{
	return m_pDBEssence->level;
}
