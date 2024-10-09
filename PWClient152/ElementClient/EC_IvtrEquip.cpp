/*
 * FILE: EC_IvtrEquip.cpp
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
#include "EC_IvtrEquip.h"
#include "EC_IvtrTypes.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_FixedMsg.h"
#include "EC_GameRun.h"
#include "EC_UIManager.h"
#include "EC_GameUIMan.h"
#include "EC_HostPlayer.h"
#include "EC_IvtrStone.h"
#include "EC_Inventory.h"
#include "EC_RTDebug.h"
#include "EC_Skill.h"
#include "ElementSkill.h"
#include "elementdataman.h"

#include "AAssist.h"

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
//	Implement CECIvtrEquip
//	
///////////////////////////////////////////////////////////////////////////

CECIvtrEquip::CECIvtrEquip(int tid, int expire_date) : CECIvtrItem(tid, expire_date)
{
	m_iCID			= ICID_EQUIP;
	m_iLevelReq		= 0;
	m_iStrengthReq	= 0;
	m_iAgilityReq	= 0;
	m_iProfReq		= 0;
	m_iVitalityReq	= 0;
	m_iEnergyReq	= 0;
	m_iReputationReq= 0;
	m_iCurEndurance	= 0;
	m_iMaxEndurance	= 0;
	m_iRepairFee	= 0;
	m_byMadeFrom	= 0;
	m_wStoneMask	= 0;
	m_iFixProps		= 0;
	m_iRefineLvl	= 0;
	m_byPropNum		= 0;
	m_byEmbedNum	= 0;
}

CECIvtrEquip::CECIvtrEquip(const CECIvtrEquip& s) : CECIvtrItem(s)
{
	m_iLevelReq		= s.m_iLevelReq;
	m_iProfReq		= s.m_iProfReq;
	m_iStrengthReq	= s.m_iStrengthReq;
	m_iAgilityReq	= s.m_iAgilityReq;
	m_iVitalityReq	= s.m_iVitalityReq;
	m_iEnergyReq	= s.m_iEnergyReq;
	m_iReputationReq= s.m_iReputationReq;
	m_iCurEndurance	= s.m_iCurEndurance;
	m_iMaxEndurance	= s.m_iMaxEndurance;
	m_iRepairFee	= s.m_iRepairFee;
	m_byMadeFrom	= s.m_byMadeFrom;
	m_strMaker		= s.GetMakerName();
	m_wStoneMask	= s.m_wStoneMask;
	m_iFixProps		= s.m_iFixProps;
	m_iRefineLvl	= s.m_iRefineLvl;
	m_byPropNum		= s.m_byPropNum;
	m_byEmbedNum	= s.m_byEmbedNum;

	int i;

	m_aHoles.SetSize(s.m_aHoles.GetSize(), 4);
	for (i=0; i < s.m_aHoles.GetSize(); i++)
		m_aHoles[i] = s.m_aHoles[i];

	m_aProps.SetSize(s.m_aProps.GetSize(), 4);
	for (i=0; i < s.m_aProps.GetSize(); i++)
		m_aProps[i] = s.m_aProps[i];
}

CECIvtrEquip::~CECIvtrEquip()
{
}

void CECIvtrEquip::ReadMakerInfo(CECDataReader &dr)
{	
	m_byMadeFrom = dr.Read_BYTE();
	int iMakerLen = dr.Read_BYTE();
	if (iMakerLen > 0)
	{
		if (m_byMadeFrom == IMT_SIGN)
		{
			WORD wColor = dr.Read_WORD();
			iMakerLen -= sizeof(WORD);

			ACString strMaker = ACString((ACHAR*)dr.Read_Data(iMakerLen), iMakerLen / sizeof (ACHAR));
			if (strMaker.IsEmpty())
			{
				a_LogOutput(1, "CECIvtrEquip::ReadMakerInfo, Invalid maker info with iMakerLen=%d", iMakerLen+sizeof(WORD));
				return;
			}

			A3DCOLOR clr = FASHION_WORDCOLOR_TO_A3DCOLOR(wColor);
			SetNewMark(strMaker, clr);
		}
		else
			m_strMaker = ACString((ACHAR*)dr.Read_Data(iMakerLen), iMakerLen / sizeof (ACHAR));
	}
	else
		m_strMaker = _AL("");
}

void CECIvtrEquip::SetNewMark(const ACString &strMark, A3DCOLOR clr)
{
	m_strMaker = strMark;
	if (!m_strMaker.IsEmpty())
	{
		//	ת�������ַ�
		m_strMaker = CECGameUIMan::AUI_ConvertHintString(m_strMaker);

		//	ת�������ַ�
		g_pGame->GetGameRun()->GetUIManager()->FilterBadWords(m_strMaker);
		
		//	�����ɫ
		ACString strColor;
		strColor.Format(_AL("^%02x%02x%02x"), A3DCOLOR_GETRED(clr), A3DCOLOR_GETGREEN(clr), A3DCOLOR_GETBLUE(clr));
		
		m_strMaker = strColor + m_strMaker;

		//	��Ӷ�����ʾ�ַ�
		m_strMaker = ACString().Format(g_pGame->GetItemDesc()->GetWideString(ITEMDESC_EQUIPMARK), m_strMaker);
	}
	m_byMadeFrom = strMark.IsEmpty() ? IMT_NULL : IMT_SIGN;
}

//	Set item detail information
bool CECIvtrEquip::SetItemInfo(BYTE* pInfoData, int iDataLen)
{
	CECIvtrItem::SetItemInfo(pInfoData, iDataLen);

	if (!pInfoData || !iDataLen)
		return true;
	
	try
	{
		CECDataReader dr(pInfoData, iDataLen);

		//	Equip requirements
		m_iLevelReq		= dr.Read_short();
		m_iProfReq		= dr.Read_short();
		m_iStrengthReq	= dr.Read_short();
		m_iVitalityReq	= dr.Read_short();
		m_iAgilityReq	= dr.Read_short();
		m_iEnergyReq	= dr.Read_short();

		m_iCurEndurance = dr.Read_int();
		m_iMaxEndurance = dr.Read_int();

		int iEssenceSize = dr.Read_short();

		ReadMakerInfo(dr);
		
		//	Skip equipment's essence data
		dr.Offset(iEssenceSize, SEEK_CUR);
		if (iEssenceSize < 0)
		{
			ASSERT(iEssenceSize >= 0);
			throw CECException(CECException::TYPE_DATAERR);
		}

		//	Parse hole data
		int iNumHole = dr.Read_short();
		m_wStoneMask = dr.Read_WORD();
		if (iNumHole > 0)
		{
			m_aHoles.SetSize(iNumHole, 5);
			for (int i=0; i < iNumHole; i++)
				m_aHoles[i] = dr.Read_int();
		}
		else if (!iNumHole)
			m_aHoles.RemoveAll(false);
		else
		{
			ASSERT(iNumHole >= 0);
			throw CECException(CECException::TYPE_DATAERR);
		}

		//	Parse property data
		int iNumProp = dr.Read_int();
		if (iNumProp > 0)
		{
			m_aProps.SetSize(iNumProp, 5);
			for (int i=0; i < iNumProp; i++)
			{
				PROPERTY& Prop = m_aProps[i];
				int iType = dr.Read_int();

				//	Parse real type info
				Prop.iType = iType & 0x1fff;
				Prop.iNumParam = (iType & 0x6000) >> 13;
				Prop.bEmbed = (iType & 0x8000) ? true : false;
				Prop.bSuite = (iType & 0x10000) ? true : false;
				Prop.bEngraved = (iType & 0x20000) ? true : false;
				Prop.bLocal = false;

				for (int j=0; j < Prop.iNumParam; j++)
					Prop.aParams[j] = dr.Read_int();
			}
		}
		else if (!iNumProp)
			m_aProps.RemoveAll(false);
		else
		{
			ASSERT(iNumProp >= 0);
			throw CECException(CECException::TYPE_DATAERR);
		}
	}
	catch (CECException& e)
	{
		ASSERT(0);
		a_LogOutput(1, "CECIvtrEquip::SetItemInfo, data read error (%d)", e.GetType());
		return false;
	}

	//	Parse properties
	ParseProperties();

	return true;
}

//	Add current endurance
int CECIvtrEquip::AddCurEndurance(int iValue)
{
	m_iCurEndurance += iValue;
	a_Clamp(m_iCurEndurance, 0, m_iMaxEndurance);
	return m_iCurEndurance;
}

//	Convert endurance real value to displaying value
int CECIvtrEquip::VisualizeEndurance(int v)
{
	return (v+ENDURANCE_SCALE-1) / ENDURANCE_SCALE;
}

//	Get empty hole number
int CECIvtrEquip::GetEmptyHoleNum()
{
	int iCount = 0;

	for (int i=0; i < m_aHoles.GetSize(); i++)
	{
		if (!m_aHoles[i])
			iCount++;
	}

	return iCount;
}

int CECIvtrEquip::GetRepairCost()
{
	if (!m_iMaxEndurance || m_iMaxEndurance == m_iCurEndurance)
		return 0;

	int iCost = (int)GetRawRepairCost();
	if (iCost < 1)
		iCost = 1;

	return iCost;
}

//	Get repair cost
float CECIvtrEquip::GetRawRepairCost()
{
	float fCost = 0.0f;

	if(!IsRepairable())
		return fCost;

	if (!m_iCurEndurance)
	{
		fCost = (float)m_iRepairFee;
	}
	else if (m_iMaxEndurance > 0 && m_iCurEndurance < m_iMaxEndurance)
	{
		float fFactor = (m_iMaxEndurance - m_iCurEndurance) / (float)m_iMaxEndurance;
		fCost = m_iRepairFee * fFactor;
	}

	return fCost;
}

//	Get scaled item
int CECIvtrEquip::GetScaledPrice()
{
	if (m_iScaleType != SCALE_SELL)
		return CECIvtrItem::GetScaledPrice();

	int iPrice = m_iPrice * m_iCount;

	if (m_iMaxEndurance == m_iCurEndurance || !m_iMaxEndurance)
		return (int)(iPrice * m_fPriceScale + 0.5f);
	else	//	m_iMaxEndurance > 0
		return (int)(iPrice * m_fPriceScale * m_iCurEndurance / (float)m_iMaxEndurance + 0.5f);
}

DWORD CECIvtrEquip::PropEffectEssence()
{
	DWORD dwFlags = 0;

	for (int i=0; i < m_aProps.GetSize(); i++)
	{
		dwFlags = PropEffectMask(m_aProps[i], dwFlags);
	}

	return dwFlags;
}

DWORD CECIvtrEquip::PropEffectMask(const PROPERTY& prop, DWORD dwFlags)
{
		CECGame::ItemExtPropTable& PropTab = g_pGame->GetItemExtPropTable();
		CECGame::ItemExtPropTable::pair_type Pair = PropTab.get(prop.iType);
		BYTE byPropType = Pair.second ? *Pair.first : 0xff;

		switch (byPropType)
		{
		case 0:		//	������

			dwFlags |= PEE_PHYDAMAGE;
			break;

		case 1:		//	����������

			dwFlags |= PEE_PHYDAMAGE;
			break;

		case 3:		//	ħ������

			dwFlags |= PEE_MAGICDAMAGE;
			break;

		case 6: 	//	+���-�﹥

			dwFlags |= PEE_PHYDAMAGE;
			dwFlags |= PEE_PHYDEF;
			break;

		case 7: 	//	+�﹥-���

			dwFlags |= PEE_PHYDAMAGE;
			dwFlags |= PEE_PHYDEF;
			break;

		case 8: 	//	+ħ��-ħ��

			dwFlags |= PEE_MAGICDAMAGE;
			dwFlags |= PEE_GOLDDEF;
			dwFlags |= PEE_WOODDEF;
			dwFlags |= PEE_WATERDEF;
			dwFlags |= PEE_FIREDEF;
			dwFlags |= PEE_EARTHDEF;
			break;

		case 9: 	//	�����ٶ�

			dwFlags |= PEE_ATKSPEED;
			break;

		case 10: 	//	��������

			dwFlags |= PEE_ATKDIST;
			break;

		case 12: 	//	�������

			dwFlags |= PEE_PHYDEF;
			break;

		case 14: 	//	���з���

			dwFlags |= PEE_GOLDDEF;
			dwFlags |= PEE_WOODDEF;
			dwFlags |= PEE_WATERDEF;
			dwFlags |= PEE_FIREDEF;
			dwFlags |= PEE_EARTHDEF;
			break;

		case 15: 	//	���

			dwFlags |= PEE_GOLDDEF;
			break;

		case 17: 	//	ľ��

			dwFlags |= PEE_WOODDEF;
			break;

		case 19: 	//	ˮ��

			dwFlags |= PEE_WATERDEF;
			break;

		case 21: 	//	���

			dwFlags |= PEE_FIREDEF;
			break;

		case 23: 	//	����

			dwFlags |= PEE_EARTHDEF;
			break;

		case 30: 	//	+���-���

			dwFlags |= PEE_GOLDDEF;
			dwFlags |= PEE_FIREDEF;
			break;

		case 31: 	//	+ľ��-���

			dwFlags |= PEE_GOLDDEF;
			dwFlags |= PEE_WOODDEF;
			break;

		case 32: 	//	+ˮ��-����

			dwFlags |= PEE_WATERDEF;
			dwFlags |= PEE_EARTHDEF;
			break;

		case 33: 	//	+���-ˮ��

			dwFlags |= PEE_WATERDEF;
			dwFlags |= PEE_FIREDEF;
			break;

		case 34: 	//	+����-ľ��

			dwFlags |= PEE_WOODDEF;
			dwFlags |= PEE_EARTHDEF;
			break;
	
		case 35: 	//	HP

			dwFlags |= PEE_HP;
			break;

		case 36: 	//	MP

			dwFlags |= PEE_MP;
			break;

		case 50: 	//	����

			dwFlags |= PEE_DODGE;
			break;

		case 53: 	//	�;ö�(%)

			dwFlags |= PEE_ENDURANCE;
			break;

		case 56: 	//	װ������

			dwFlags |= PEE_STRENGTHREQ;
			dwFlags |= PEE_AGILITYREQ;
			dwFlags |= PEE_ENERGYREQ;
			dwFlags |= PEE_VITALITYREQ;
			break;
			
		case 92: 	//	+���+ħ��
			
			dwFlags |= PEE_PHYDEF;
			dwFlags |= PEE_GOLDDEF;
			dwFlags |= PEE_WOODDEF;
			dwFlags |= PEE_WATERDEF;
			dwFlags |= PEE_FIREDEF;
			dwFlags |= PEE_EARTHDEF;
			break;

	//	case 2:		//	������(%)
	//	case 4: 	//	ħ����������
	//	case 5: 	//	ħ������(%)
	//	case 11: 	//	����ʱ��
	//	case 13: 	//	�������(%)
	//	case 16: 	//	���(%)
	//	case 18: 	//	ľ��(%)
	//	case 20: 	//	ˮ��(%)
	//	case 22: 	//	���(%)
	//	case 24: 	//	����(%)
	//	case 25: 	//	+���(%)-���(%)
	//	case 26: 	//	+ľ��(%)-���(%)
	//	case 27: 	//	+ˮ��(%)-����(%)
	//	case 28: 	//	+���(%)-ˮ��(%)
	//	case 29: 	//	+����(%)-ľ��(%)
	//	case 37: 	//	HP(%)
	//	case 38: 	//	MP(%)
	//	case 39: 	//	HP�ָ��ٶ�
	//	case 40: 	//	MP�ָ��ٶ�
	//	case 41: 	//	����
	//	case 42: 	//	����
	//	case 43: 	//	����
	//	case 44: 	//	����
	//	case 45: 	//	����һ����
	//	case 46: 	//	����
	//	case 47: 	//	����(%)
	//	case 48: 	//	�ƶ��ٶ�
	//	case 49: 	//	�ƶ��ٶ�(%)
	//	case 51: 	//	����(%)
	//	case 52: 	//	�;ö�
	//	case 54: 	//	��������
	//	case 55: 	//	���Ӽ���
	//	case 57: 	//	δ֪����
	//	case 58:	//	����ֵ�ӳ�
	//	case 59:	//	�����ȼ�
	//	case 60:	//	�����ȼ�
		}

	return dwFlags;
}

//	Get item description for booth buying
const wchar_t* CECIvtrEquip::GetBoothBuyDesc()
{
	m_strDesc = _AL("");

	CECStringTab* pDescTab = g_pGame->GetItemDesc();

	int white = ITEMDESC_COL_WHITE;

	//	Get property effect essence flags
//	DWORD dwPEE = PropEffectEssence();

	//	Item name
	AddDescText(white, true, pDescTab->GetWideString(ITEMDESC_NAME), GetName());

	//	Price
	AddPriceDesc(white, false);

	return m_strDesc;
}

//	Add price description
void CECIvtrEquip::AddPriceDesc(int col, bool bRepair)
{
	CECStringTab* pDescTab = g_pGame->GetItemDesc();
	if (bRepair)
		AddDescText(col, false, pDescTab->GetWideString(ITEMDESC_REPAIRCOST), GetRepairCost());
	else
		CECIvtrItem::AddPriceDesc(col, bRepair);
}
//  Set properties to local
void CECIvtrEquip::SetLocalProps()
{
	if(!m_aProps.GetSize())
		return;

	for(int i=0;i<m_aProps.GetSize();i++)
	{
		PROPERTY& prop = m_aProps[i];
		prop.bLocal = true;
	}
}

//	Add deadly strike rate provided by this equipment
//	bSuite: true, get deadly strike generated by suite
int CECIvtrEquip::GetDeadlyStrikeRate(bool bSuiteGen)
{
	int iVal = 0;

	if (bSuiteGen)
	{
	/*	for (int i=0; i < m_aProps.GetSize(); i++)
		{
			const PROPERTY& prop = m_aProps[i];
			if (!prop.bSuite)	//	Suite property
				continue;
			
			CECGame::ItemExtPropTable::pair_type Pair = PropTab.get(prop.iType);
			BYTE byPropType = Pair.second ? *Pair.first : 0xff;

			switch (byPropType)
			{
			case 150:	iVal += 1;	break;
			case 151:	iVal += 2;	break;
			case 152:	iVal += 3;	break;
			case 153:	iVal += 4;	break;
			case 154:	iVal += 5;	break;
			case 155:	iVal += 6;	break;
			}
		}
	*/
		//	Maximum number of suite items
		enum
		{
			MAX_NUM = 12
		};

		int idSuite = GetSuiteID();
		if (!idSuite)
			return 0;	//	This equipment isn't one of any suite

		CECHostPlayer* pHostPlayer = g_pGame->GetGameRun()->GetHostPlayer();

		//	Get equipped suite item list
		int iItemCnt, aEquipped[MAX_NUM];
		iItemCnt = pHostPlayer->GetEquippedSuiteItem(idSuite, aEquipped);
		if (iItemCnt <= 1)
			return 0;

		//	Get suite info
		DATA_TYPE DataType = DT_INVALID;
		elementdataman* pDataMan = g_pGame->GetElementDataMan();
		const void* pData = pDataMan->get_data_ptr(idSuite, ID_SPACE_ESSENCE, DataType);
		if (DataType != DT_SUITE_ESSENCE)
		{
			ASSERT(DataType == DT_SUITE_ESSENCE);
			return 0;
		}

		const SUITE_ESSENCE* pSuiteEss = (const SUITE_ESSENCE*)pData;
		CECGame::ItemExtPropTable& PropTab = g_pGame->GetItemExtPropTable();

		for (int i=1; i < iItemCnt; i++)
		{
			int idProp = (int)pSuiteEss->addons[i-1].id;

			CECGame::ItemExtPropTable::pair_type Pair = PropTab.get(idProp);
			BYTE byPropType = Pair.second ? *Pair.first : 0xff;

			switch (byPropType)
			{
			case 150:	iVal += 1;	break;
			case 151:	iVal += 2;	break;
			case 152:	iVal += 3;	break;
			case 153:	iVal += 4;	break;
			case 154:	iVal += 5;	break;
			case 155:	iVal += 6;	break;
			}
		}
	}
	else
	{
		CECGame::ItemExtPropTable& PropTab = g_pGame->GetItemExtPropTable();
		
		for (int i=0; i < m_aProps.GetSize(); i++)
		{
			const PROPERTY& prop = m_aProps[i];
			CECGame::ItemExtPropTable::pair_type Pair = PropTab.get(prop.iType);
			BYTE byPropType = Pair.second ? *Pair.first : 0xff;

			if (byPropType == 45 ||
				byPropType == 110)
				iVal += prop.aParams[0];
		}
	}

	return iVal;
}

//	Decide equipment name color
int CECIvtrEquip::DecideNameCol()
{
	int iIndex = GetColorStrID(m_tid);
	if (iIndex >= 0)
		return iIndex;

	int col = ITEMDESC_COL_WHITE;

	switch (m_iFixProps)
	{
	case 1:	col = ITEMDESC_COL_GREEN;		break;
	case 2:	col = ITEMDESC_COL_YELLOW;		break;
	case 3:	col = ITEMDESC_COL_DARKGOLD;	break;

	default:
		
		if (m_byPropNum)
			col = ITEMDESC_COL_LIGHTBLUE;

		break;
	}

	return col;
}

bool CECIvtrEquip::CheckSpecialRefineType(const int* aRefines, const int* types, int type_count)
{
	// check the special refine property
	int same_value = 0;

	for(int refineIndex=0;refineIndex<MAX_REFINEINDEX;refineIndex++)
	{
		bool checked = false;
		for(int i=0;i<type_count;i++)
		{
			if(types[i] == refineIndex)
			{
				if(same_value == 0)
				{
					if(aRefines[refineIndex] == 0)
					{
						// specific property was not found
						return false;
					}
					same_value = aRefines[refineIndex];
				}
				else if(same_value != aRefines[refineIndex])
				{
					// property values were different
					return false;
				}

				checked = true;
				break;
			}
		}

		// check other refine property
		if(!checked && aRefines[refineIndex] != 0)
		{
			// has other refine property
			return false;
		}
	}

	return true;
}

ACString& CECIvtrEquip::FormatPropDesc(const PROPERTY& prop, ACString& szTxt)
{
	// save current member for backup
	ACString tmpDesc = m_strDesc;
	m_strDesc.Empty();

	AddOneAddOnPropDesc(prop.iType, prop.aParams, NULL, NULL, prop.bLocal);

	szTxt = m_strDesc;
	m_strDesc = tmpDesc;
	return szTxt;
}

// Format refine data into a string
// (hack function, do NOT use it in multi-thread environment)
ACString& CECIvtrEquip::FormatRefineData(unsigned int addon_id, ACString& szTxt)
{	
	DATA_TYPE dataType;

	const EQUIPMENT_ADDON* pType = 
		(const EQUIPMENT_ADDON*)g_pGame->GetElementDataMan()->get_data_ptr(addon_id, ID_SPACE_ADDON, dataType);

	szTxt.Empty();


	if(DT_EQUIPMENT_ADDON == dataType)
	{
		// save current member for backup
		ACString tmpDesc = m_strDesc;
		m_strDesc.Empty();

		// get the original refine data
		int aRefines[MAX_REFINEINDEX];
		memset(aRefines, 0, sizeof (aRefines));
		AddOneAddOnPropDesc(pType->id, pType->param1, pType->param2, pType->param3, NULL, aRefines, true);

		// prepare the string formatter
		CECStringTab* pDescTab = g_pGame->GetItemDesc();
		int white = ITEMDESC_COL_WHITE;

		// get splitter
		ACString szSplitter = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan()->GetStringFromTable(8656);
		int findSplitter = 0;
		for(int pos = 0;pos < szSplitter.GetLength();pos++)
		{
			findSplitter = m_strDesc.Find(szSplitter[pos]);
			if(findSplitter >= 0) break;
		}
		szTxt += findSplitter <= 0 ? m_strDesc : m_strDesc.Mid(0, findSplitter);
		// restore member because previous method may modify it
		m_strDesc = tmpDesc;

		// replace all old splitter to new splitter
		const ACString OldSplitter = _AL("\\r");
		const ACString NewSplitter = _AL(" ");
		ACString result;
		findSplitter = 0;
		while(true)
		{
			int curfind = szTxt.Find(OldSplitter, findSplitter);
			if(curfind >= 0)
			{
				if(!result.IsEmpty()) result += NewSplitter;
				if(curfind > findSplitter)
				{
					result += szTxt.Mid(findSplitter, curfind - findSplitter);
				}
				findSplitter = curfind + OldSplitter.GetLength();
				if(findSplitter >= szTxt.GetLength()) break;
			}
			else
			{
				if(!result.IsEmpty()) result += NewSplitter;
				result += szTxt.Mid(findSplitter);
				break;
			}
		}
		szTxt = result;

		// check the special refine property group
		const int ALLMAGIC_REFINE[] = {REFINE_GOLDDEF, REFINE_WOODDEF, REFINE_WATERDEF, REFINE_FIREDEF, REFINE_EARTHDEF};
		const int ALLDAMAGE_REFINE[] = {REFINE_PHYDAMAGE, REFINE_MAGICDAMAGE};
		const int ALLDEFENCE_REFINE[] = {REFINE_PHYDEF, REFINE_GOLDDEF, REFINE_WOODDEF, REFINE_WATERDEF, REFINE_FIREDEF, REFINE_EARTHDEF};

		if(CheckSpecialRefineType(aRefines, ALLMAGIC_REFINE, sizeof(ALLMAGIC_REFINE)/sizeof(int)))
		{
			ACString szRefine;
			szRefine.Format(_AL("%s %+d"), pDescTab->GetWideString(ITEMDESC_ALLMAGICDEF), aRefines[ALLMAGIC_REFINE[0]]);
			if(!szTxt.IsEmpty()) szTxt += NewSplitter;
			szTxt += szRefine;
		}
		else if(CheckSpecialRefineType(aRefines, ALLDAMAGE_REFINE, sizeof(ALLDAMAGE_REFINE)/sizeof(int)))
		{
			ACString szRefine;
			szRefine.Format(_AL("%s %+d"), pDescTab->GetWideString(ITEMDESC_ADDDAMAGE), aRefines[ALLDAMAGE_REFINE[0]]);
			if(!szTxt.IsEmpty()) szTxt += NewSplitter;
			szTxt += szRefine;
		}
		else if(CheckSpecialRefineType(aRefines, ALLDEFENCE_REFINE, sizeof(ALLDEFENCE_REFINE)/sizeof(int)))
		{
			ACString szRefine;
			szRefine.Format(_AL("%s %+d"), pDescTab->GetWideString(ITEMDESC_DEFENCE), aRefines[ALLDEFENCE_REFINE[0]]);
			if(!szTxt.IsEmpty()) szTxt += NewSplitter;
			szTxt += szRefine;
		}
		else
		{
			int descId = 0;
			for(int refineIndex = 0;refineIndex < MAX_REFINEINDEX;refineIndex++)
			{
				if(aRefines[refineIndex] == 0) continue;
				
				// do NOT use loop because the enum value may changed
				switch(refineIndex)
				{
				case REFINE_PHYDAMAGE:		descId = ITEMDESC_ADDPHYDAMAGE;break;
				case REFINE_MAGICDAMAGE:	descId = ITEMDESC_ADDMAGICDAMAGE;break;
				case REFINE_PHYDEF:			descId = ITEMDESC_PHYDEFENCE;break;
				case REFINE_GOLDDEF:		descId = ITEMDESC_GOLDDEFENCE;break;
				case REFINE_WOODDEF:		descId = ITEMDESC_WOODDEFENCE;break;
				case REFINE_WATERDEF:		descId = ITEMDESC_WATERDEFENCE;break;
				case REFINE_FIREDEF:		descId = ITEMDESC_FIREDEFENCE;break;
				case REFINE_EARTHDEF:		descId = ITEMDESC_EARTHDEFENCE;break;
				case REFINE_HP:				descId = ITEMDESC_ADDHP;break;
				case REFINE_DODGE:			descId = ITEMDESC_DODGE;break;
				default:					descId = -1;break;
				}
				
				if(descId >= 0)
				{
					ACString szRefine;
					szRefine.Format(_AL("%s %+d "), pDescTab->GetWideString(descId), aRefines[refineIndex]);
					if(!szTxt.IsEmpty()) szTxt += NewSplitter;
					szTxt += szRefine;
				}
			}
		}
	}
	
	return szTxt;
}

bool CECIvtrEquip::CheckPropRangeValue(const PROPERTY& prop)
{
	int idProp = prop.iType;

	CECStringTab* pDescTab = g_pGame->GetItemDesc();
	
	CECGame::ItemExtPropTable& PropTab = g_pGame->GetItemExtPropTable();
	CECGame::ItemExtPropTable::pair_type Pair = PropTab.get(idProp);
	BYTE byPropType = Pair.second ? *Pair.first : 0xff;

	switch(byPropType)
	{
	case 25: 	//	+���(%)-���(%)
	case 26: 	//	+ľ��(%)-���(%)
	case 27: 	//	+ˮ��(%)-����(%)
	case 28: 	//	+���(%)-ˮ��(%)
	case 29: 	//	+����(%)-ľ��(%)
	case 30: 	//	+���-���
	case 31: 	//	+ľ��-���
	case 32: 	//	+ˮ��-����
	case 33: 	//	+���-ˮ��
	case 34: 	//	+����-ľ��
	case 55: 	//	���Ӽ���
		return false;
	}

	DATA_TYPE dataType = DT_INVALID;
	const EQUIPMENT_ADDON* pType = 
		(const EQUIPMENT_ADDON*)g_pGame->GetElementDataMan()->get_data_ptr(idProp, ID_SPACE_ADDON, dataType);		
	if(DT_EQUIPMENT_ADDON == dataType)
	{
		// here we only check the difference, the float number can be handled correctly
		int params[] = {pType->param1, pType->param2, pType->param3,};
		int paramNum = pType->num_params;
		while(--paramNum)
		{
			int lhs = params[pType->num_params - paramNum - 1];
			int rhs = params[pType->num_params - paramNum];
			if(rhs != lhs) { return true; }
		}
	}

	return false;
}

//	Add one add-on property description
void CECIvtrEquip::AddOneAddOnPropDesc(int idProp, int p0, int p1, int p2, 
								int* aPEEVals, int* aRefines, bool bLocal)
{
	CECStringTab* pDescTab = g_pGame->GetItemDesc();

	CECGame::ItemExtPropTable& PropTab = g_pGame->GetItemExtPropTable();
	CECGame::ItemExtPropTable::pair_type Pair = PropTab.get(idProp);
	BYTE byPropType = Pair.second ? *Pair.first : 0xff;

#define ADD_RANGE_VALUE_DESC_ID(id_first_format_str, second_format_str, value0, value1, value2)	\
		if(bLocal)\
		{\
			if(p0 != p1)\
			{\
				AddDescText(color, false, pDescTab->GetWideString((id_first_format_str)), (value0));\
				AddDescText(color, true, (second_format_str), (value1));\
			}\
			else\
			{\
				AddDescText(color, true, pDescTab->GetWideString((id_first_format_str)), (value0));\
			}\
		}\
		else\
		{\
			AddDescText(color, true, pDescTab->GetWideString((id_first_format_str)), (value2));\
		}

#define ADD_RANGE_VALUE_DESC_STR(id_first_format_1, first_format_str_2, second_format_str, value0, value1, value2)	\
	{\
		ACString strFirstFormat;\
		strFirstFormat.Format(_AL("%s %s"), pDescTab->GetWideString((id_first_format_1)), (first_format_str_2));\
		if(bLocal)\
		{\
			if(p0 != p1)\
			{\
				AddDescText(color, false, strFirstFormat, (value0));\
				AddDescText(color, true, (second_format_str), (value1));\
			}\
			else\
			{\
				AddDescText(color, true, strFirstFormat, (value0));\
			}\
		}\
		else\
		{\
			AddDescText(color, true, strFirstFormat, (value2));\
		}\
	}

#define ADD_RANGE_VALUE_DESC_ID_NORMAL(id_string)					ADD_RANGE_VALUE_DESC_ID((id_string), _AL("~%d"), p0, p1, p0)
#define ADD_RANGE_VALUE_DESC_ID_FLOAT(id_string)						ADD_RANGE_VALUE_DESC_ID((id_string), _AL("~%.2f"), *(float*)&p0, *(float*)&p1, *(float*)&p0)
#define ADD_RANGE_VALUE_DESC_ID_PERCENT(id_string)					ADD_RANGE_VALUE_DESC_ID((id_string), _AL("~%d%%"), VisualizeFloatPercent(p0), VisualizeFloatPercent(p1), p0)
#define ADD_RANGE_VALUE_DESC_ID_MINUS_PERCENT_1(id_string)	ADD_RANGE_VALUE_DESC_ID((id_string), _AL("~%d%%"), -VisualizeFloatPercent(p0), VisualizeFloatPercent(p1), -p0)
#define ADD_RANGE_VALUE_DESC_ID_MINUS_PERCENT_2(id_string)	ADD_RANGE_VALUE_DESC_ID((id_string), _AL("~%d%%"), -VisualizeFloatPercent(p0), VisualizeFloatPercent(p1), -VisualizeFloatPercent(p0))
#define ADD_RANGE_VALUE_DESC_ID_HALF(id_string)						ADD_RANGE_VALUE_DESC_ID((id_string), _AL("~%d"), (p0/2), (p1/2), (p0/2))
#define ADD_RANGE_VALUE_DESC_STR_NORMAL(id_string)					ADD_RANGE_VALUE_DESC_STR((id_string), _AL("%+d"), _AL("~%d"), p0, p1, p0)

	int color = -1;
	
	if (!IsSharpenerProperty(byPropType)){
		switch (byPropType)
		{
		case 0:		//	������
			
			if(!bLocal)
			{
				if (aPEEVals)
					aPEEVals[PEEI_PHYDAMAGE] += p0;
			}
			
			AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_ADDPHYDAMAGE));
			AddDescText(color, true, _AL(" %+d"), p0);
			break;
			
		case 1:		//	����������
			
			if(bLocal)
			{
				if(p0 != p1)
				{
					AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_MAXPHYDAMAGE), p0);
					AddDescText(color, true, _AL("~%d"), p1);
				}
				else
				{
					AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_MAXPHYDAMAGE), p0);
				}
			}
			else
			{
				if (aPEEVals)
					aPEEVals[PEEI_MAX_PHYDAMAGE] += p0;
				
				AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_MAXPHYDAMAGE), p0);
			}
			break;
			
		case 2:		//	������(%)
			if(bLocal)
			{
				AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_PHYDMGEXTRA),  VisualizeFloatPercent(p0));
			}
			else
			{
				AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_PHYDMGEXTRA), p0);
			}
			break;
			
		case 3:		//	ħ������
			
			if(!bLocal)
			{
				if (aPEEVals)
					aPEEVals[PEEI_MAGICDAMAGE] += p0;
			}
			
			AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_ADDMAGICDAMAGE));
			AddDescText(color, true, _AL(" %+d"), p0);
			break;
			
		case 4: 	//	ħ����������
			
			if(bLocal)
			{
				if(p0 != p1)
				{
					AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_MAXMAGICDAMAGE), p0);
					AddDescText(color, true, _AL("~%d"), p1);
				}
				else
				{
					AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_MAXMAGICDAMAGE), p0);		
				}
			}
			else
			{
				if (aPEEVals)
					aPEEVals[PEEI_MAX_MAGICDAMAGE] += p0;
				
				AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_MAXMAGICDAMAGE), p0);		
			}
			break;
			
		case 5: 	//	ħ������(%)
			
			if(bLocal)
			{
				AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_MAGICDMGEXTRA),  VisualizeFloatPercent(p0));
			}
			else
			{
				AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_MAGICDMGEXTRA), p0);
			}
			break;
			
		case 6: 	//	+���-�﹥
			
			if(!bLocal)
			{
				if (aPEEVals)
				{
					aPEEVals[PEEI_PHYDEF]	 += p0;
					aPEEVals[PEEI_PHYDAMAGE] -= p1;
				}
			}
			
			AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_PHYDEFENCE));
			AddDescText(color, true, _AL(" %+d"), p0);
			AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_ADDPHYDAMAGE));
			AddDescText(color, true, _AL(" %+d"), -p1);
			break;
			
		case 7: 	//	+�﹥-���
			
			if(!bLocal)
			{
				if (aPEEVals)
				{
					aPEEVals[PEEI_PHYDAMAGE] += p0;
					aPEEVals[PEEI_PHYDEF]	 -= p1;
				}
			}
			
			AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_ADDPHYDAMAGE));
			AddDescText(color, true, _AL(" %+d"), p0);
			AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_PHYDEFENCE));
			AddDescText(color, true, _AL(" %+d"), -p1);
			break;
			
		case 8: 	//	+ħ��-ħ��
			
			if(!bLocal)
			{
				if (aPEEVals)
				{
					aPEEVals[PEEI_MAGICDAMAGE]	+= p0;
					aPEEVals[PEEI_GOLDDEF]		-= p1;
					aPEEVals[PEEI_WOODDEF]		-= p1;
					aPEEVals[PEEI_WATERDEF]		-= p1;
					aPEEVals[PEEI_FIREDEF]		-= p1;
					aPEEVals[PEEI_EARTHDEF]		-= p1;
				}
			}
			
			AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_ADDMAGICDAMAGE));
			AddDescText(color, true, _AL(" %+d"), p0);
			AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_ALLMAGICDEF));
			AddDescText(color, true, _AL(" %+d"), -p1);
			break;
			
		case 9: 	//	�����ٶ�
			
			if(bLocal)
			{
				AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_ATKTIME),  -*(float*)&p0);
			}
			else
			{
				AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_ATKTIME), -p0 * 0.05f);
			}
			break;
			
		case 10:	//	��������
			{
				
				if(bLocal)
				{
					AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_ADDATKDIST), *(float*)&p0);
				}
				else
				{
					if (aPEEVals)
					{
						float fDist = *(float*)&aPEEVals[PEEI_ATKDIST] + *(float*)&p0;
						aPEEVals[PEEI_ATKDIST] = *(DWORD*)&fDist;
					}
					
					AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_ADDATKDIST), *(float*)&p0);
				}
				break;
			}
		case 11:	//	����ʱ��
			
			if(bLocal)
			{
				AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_CASTTIME), -VisualizeFloatPercent(p0));
			}
			else
			{
				AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_CASTTIME), -p0);
			}
			break;
			
		case 12: 	//	�������
			
			if(!bLocal)
			{
				if (aPEEVals)
					aPEEVals[PEEI_PHYDEF] += p0;
			}
			
			AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_PHYDEFENCE));
			AddDescText(color, true, _AL(" %+d"), p0);
			break;
			
		case 13: 	//	�������(%)
			
			if(bLocal)
				AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_PHYDEFEXTRA), VisualizeFloatPercent(p0));
			else
				AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_PHYDEFEXTRA), p0);
			
			break;
			
		case 14: 	//	���з���
			
			if(!bLocal)
			{
				if (aPEEVals)
				{
					aPEEVals[PEEI_GOLDDEF]	+= p0;
					aPEEVals[PEEI_WOODDEF]	+= p0;
					aPEEVals[PEEI_WATERDEF] += p0;
					aPEEVals[PEEI_FIREDEF]	+= p0;
					aPEEVals[PEEI_EARTHDEF] += p0;
				}
			}
			
			AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_ALLMAGICDEF));
			AddDescText(color, true, _AL(" %+d"), p0);
			break;
			
		case 15: 	//	���
			
			if(bLocal)
			{
				AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_GOLDDEFENCE));
				
				if(p0 != p1)
					AddDescText(color, true, _AL(" %d~%d"), p0, p1);
				else
					AddDescText(color, true, _AL(" %d"), p0);
			}
			else
			{
				if (aPEEVals)
					aPEEVals[PEEI_GOLDDEF] += p0;
				
				AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_GOLDDEFENCE));
				AddDescText(color, true, _AL(" %+d"), p0);
			}
			break;
			
		case 16: 	//	���(%)
			
			if(bLocal)
			{
				if((p0) != (p1))
				{
					AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_GOLDDEFEXTRA), (p0));
					AddDescText(color, true, _AL("~-%.2f%%"), (p1));
				}
				else
				{
					AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_GOLDDEFEXTRA), (p0));
				}
			}
			else
			{
				AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_GOLDDEFEXTRA), (p0));
			}
			break;
			
		case 17: 	//	ľ��
			
			if(bLocal)
			{
				AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_WOODDEFENCE));
				
				if(p0 != p1)
					AddDescText(color, true, _AL(" %d~%d"), p0, p1);
				else
					AddDescText(color, true, _AL(" %d"), p0);
			}
			else
			{
				if (aPEEVals)
					aPEEVals[PEEI_WOODDEF] += p0;
				
				AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_WOODDEFENCE));
				AddDescText(color, true, _AL(" %+d"), p0);
			}
			break;
			
		case 18: 	//	ľ��(%)
			
			if(bLocal)
			{
				if((p0) != (p1))
				{
					AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_WOODDEFEXTRA), (p0));
					AddDescText(color, true, _AL("~-%.2f%%"), (p1));
				}
				else
				{
					AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_WOODDEFEXTRA), (p0));
				}
			}
			else
			{
				AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_WOODDEFEXTRA), (p0));
			}
			break;
			
		case 19: 	//	ˮ��
			
			if(bLocal)
			{
				AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_WATERDEFENCE));
				
				if(p0 != p1)
					AddDescText(color, true, _AL(" %d~%d"), p0, p1);
				else
					AddDescText(color, true, _AL(" %d"), p0);
			}
			else
			{
				if (aPEEVals)
					aPEEVals[PEEI_WATERDEF] += p0;
				
				AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_WATERDEFENCE));
				AddDescText(color, true, _AL(" %+d"), p0);
			}
			break;
			
		case 20: 	//	ˮ��(%)
			
			if(bLocal)
			{
				if((p0) != (p1))
				{
					AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_WATERDEFEXTRA), (p0));
					AddDescText(color, true, _AL("~-%.2f%%"), (p1));
				}
				else
				{
					AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_WATERDEFEXTRA), (p0));
				}
			}
			else
			{
				AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_WATERDEFEXTRA), (p0));
			}
			break;
			
		case 21: 	//	���
			
			if(bLocal)
			{
				AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_FIREDEFENCE));
				
				if(p0 != p1)
					AddDescText(color, true, _AL(" %d~%d"), p0, p1);
				else
					AddDescText(color, true, _AL(" %d"), p0);
			}
			else
			{
				if (aPEEVals)
					aPEEVals[PEEI_FIREDEF] += p0;
				
				AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_FIREDEFENCE));
				AddDescText(color, true, _AL(" %+d"), p0);
			}
			break;
			
		case 22: 	//	���(%)
			
			if(bLocal)
			{
				if((p0) != (p1))
				{
					AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_FIREDEFEXTRA), (p0));
					AddDescText(color, true, _AL("~-%.2f%%"), (p1));
				}
				else
				{
					AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_FIREDEFEXTRA), (p0));
				}
			}
			else
			{
				AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_FIREDEFEXTRA), (p0));
			}
			break;
			
		case 23: 	//	����
			
			if(bLocal)
			{
				AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_EARTHDEFENCE));
				
				if(p0 != p1)
					AddDescText(color, true, _AL(" %d~%d"), p0, p1);
				else
					AddDescText(color, true, _AL(" %d"), p0);
			}
			else
			{
				if (aPEEVals)
					aPEEVals[PEEI_EARTHDEF] += p0;
				
				AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_EARTHDEFENCE));
				AddDescText(color, true, _AL(" %+d"), p0);
			}
			
			break;
			
		case 24: 	//	����(%)
			
			if(bLocal)
			{
				if((p0) != (p1))
				{
					AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_EARTHDEFEXTRA), (p0));
					AddDescText(color, true, _AL("~-%.2f%%"), (p1));
				}
				else
				{
					AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_EARTHDEFEXTRA), (p0));
				}
			}
			else
			{
				AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_EARTHDEFEXTRA), (p0));
			}
			break;
			
		case 25: 	//	+���(%)-���(%)
			
			AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_GOLDDEFEXTRA), VisualizeFloatPercent(p0));
			AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_FIREDEFEXTRA), -VisualizeFloatPercent(p1));
			break;
			
		case 26: 	//	+ľ��(%)-���(%)
			
			AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_WOODDEFEXTRA), VisualizeFloatPercent(p0));
			AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_GOLDDEFEXTRA), -VisualizeFloatPercent(p1));
			break;
			
		case 27: 	//	+ˮ��(%)-����(%)
			
			AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_WATERDEFEXTRA), VisualizeFloatPercent(p0));
			AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_EARTHDEFEXTRA), -VisualizeFloatPercent(p1));
			break;
			
		case 28: 	//	+���(%)-ˮ��(%)
			
			AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_FIREDEFEXTRA), VisualizeFloatPercent(p0));
			AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_WATERDEFEXTRA), -VisualizeFloatPercent(p1));
			break;
			
		case 29: 	//	+����(%)-ľ��(%)
			
			AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_EARTHDEFEXTRA), VisualizeFloatPercent(p0));
			AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_WOODDEFEXTRA), -VisualizeFloatPercent(p1));
			break;
			
		case 30: 	//	+���-���
			
			if(!bLocal)
			{
				if (aPEEVals)
				{
					aPEEVals[PEEI_GOLDDEF]	+= p0;
					aPEEVals[PEEI_FIREDEF]	-= p1;
				}
			}
			
			AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_GOLDDEFENCE));
			AddDescText(color, true, _AL(" %+d"), p0);
			AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_FIREDEFENCE));
			AddDescText(color, true, _AL(" %+d"), -p1);
			break;
			
		case 31: 	//	+ľ��-���
			
			if(!bLocal)
			{
				if (aPEEVals)
				{
					aPEEVals[PEEI_WOODDEF]	+= p0;
					aPEEVals[PEEI_GOLDDEF]	-= p1;
				}
			}
			
			AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_WOODDEFENCE));
			AddDescText(color, true, _AL(" %+d"), p0);
			AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_GOLDDEFENCE));
			AddDescText(color, true, _AL(" %+d"), -p1);
			break;
			
		case 32: 	//	+ˮ��-����
			
			if(!bLocal)
			{
				if (aPEEVals)
				{
					aPEEVals[PEEI_WATERDEF]	+= p0;
					aPEEVals[PEEI_EARTHDEF]	-= p1;
				}
			}
			
			AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_WATERDEFENCE));
			AddDescText(color, true, _AL(" %+d"), p0);
			AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_EARTHDEFENCE));
			AddDescText(color, true, _AL(" %+d"), -p1);
			break;
			
		case 33: 	//	+���-ˮ��
			
			if(!bLocal)
			{
				if (aPEEVals)
				{
					aPEEVals[PEEI_FIREDEF]	+= p0;
					aPEEVals[PEEI_WATERDEF]	-= p1;
				}
			}
			
			AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_FIREDEFENCE));
			AddDescText(color, true, _AL(" %+d"), p0);
			AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_WATERDEFENCE));
			AddDescText(color, true, _AL(" %+d"), -p1);
			break;
			
		case 34: 	//	+����-ľ��
			
			if(!bLocal)
			{
				if (aPEEVals)
				{
					aPEEVals[PEEI_EARTHDEF]	+= p0;
					aPEEVals[PEEI_WOODDEF]	-= p1;
				}
			}
			
			AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_EARTHDEFENCE));
			AddDescText(color, true, _AL(" %+d"), p0);
			AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_WOODDEFENCE));
			AddDescText(color, true, _AL(" %+d"), -p1);
			break;
			
		case 35: 	//	HP
			
			if(!bLocal)
			{
				if (aPEEVals)
					aPEEVals[PEEI_HP] += p0;
			}
			
			AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_ADDHP));
			AddDescText(color, true, _AL(" %+d"), p0);
			break;
			
		case 36: 	//	MP
			
			if(!bLocal)
			{
				if (aPEEVals)
					aPEEVals[PEEI_MP] += p0;
			}
			
			AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_ADDMP));
			AddDescText(color, true, _AL(" %+d"), p0);
			break;
			
		case 37: 	//	HP(%)
			
			if(bLocal)
			{
				AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_HPEXTRA), VisualizeFloatPercent(p0));
			}
			else
			{
				AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_HPEXTRA), p0);
			}		
			break;
			
		case 38: 	//	MP(%)
			
			if(bLocal)
			{
				AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_MPEXTRA), VisualizeFloatPercent(p0));
			}
			else
			{
				AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_MPEXTRA), p0);
			}
			break;
			
		case 39: 	//	HP�ָ��ٶ�
			
			AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_HPRECOVER), p0 / 2);
			break;
			
		case 40: 	//	MP�ָ��ٶ�
			
			AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_MPRECOVER), p0 / 2);
			break;
			
		case 41: 	//	����
			
			if(bLocal)
			{
				if(p0 != p1)
				{
					AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_STRENGTH), p0);
					AddDescText(color, true, _AL("~%d"), p1);
				}
				else
				{
					AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_STRENGTH), p0);
				}
			}
			else
			{
				AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_STRENGTH), p0);
			}
			break;
			
		case 42: 	//	����
			
			if(bLocal)
			{
				if(p0 != p1)
				{
					AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_AGILITY), p0);
					AddDescText(color, true, _AL("~%d"), p1);
				}
				else
				{
					AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_AGILITY), p0);
				}
			}
			else
			{
				AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_AGILITY), p0);
			}
			break;
			
		case 43: 	//	����
			
			if(bLocal)
			{
				if(p0 != p1)
				{
					AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_ENERGY), p0);
					AddDescText(color, true, _AL("~%d"), p1);
				}
				else
				{
					AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_ENERGY), p0);
				}
			}
			else
			{
				AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_ENERGY), p0);
			}
			break;
			
		case 44: 	//	����
			
			if(bLocal)
			{
				if(p0 != p1)
				{
					AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_VITALITY), p0);
					AddDescText(color, true, _AL("~%d"), p1);
				}
				else
				{
					AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_VITALITY), p0);
				}
			}
			else
			{
				AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_VITALITY), p0);
			}
			break;
			
		case 45: 	//	����һ����
			
			if(bLocal)
			{
				AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_DEADLYSTRIKE), VisualizeFloatPercent(p0));
			}
			else
			{
				AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_DEADLYSTRIKE), p0);
			}
			break;
			
		case 46: 	//	����
			
			if(bLocal)
			{
				if(p0 != p1)
				{
					AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_ATKRATING), p0);
					AddDescText(color, true, _AL("~%d"), p1);
				}
				else
				{
					AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_ATKRATING), p0);
				}
			}
			else
			{
				AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_ATKRATING), p0);
			}
			break;
			
		case 47: 	//	����(%)
			
			if(bLocal)
			{
				AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_ATKRATINGEXTRA), VisualizeFloatPercent(p0));
			}
			else
			{
				AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_ATKRATINGEXTRA), p0);
			}
			break;
			
		case 48: 	//	�ƶ��ٶ�
			
			AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_RUNSPEED), *(float*)&p0);
			break;
			
		case 49: 	//	�ƶ��ٶ�(%)
			
			if(bLocal)
			{
				AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_RUNSPEEDEXTRA), VisualizeFloatPercent(p0));
			}
			else
			{
				AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_RUNSPEEDEXTRA), p0);
			}
			break;
			
		case 50: 	//	����
			
			if(!bLocal)
			{
				if (aPEEVals)
					aPEEVals[PEEI_DODGE] += p0;
			}
			
			AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_DODGE));
			AddDescText(color, true, _AL(" %+d"), p0);
			break;
			
		case 51: 	//	����(%)
			
			if(bLocal)
			{
				AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_DODGEEXTRA), VisualizeFloatPercent(p0));
			}
			else
			{
				AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_DODGEEXTRA), p0);
			}
			break;
			
		case 52: 	//	�;ö�
			
			AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_ENDURANCE));
			AddDescText(color, true, _AL(" %+d"), p0);
			break;
			
		case 53: 	//	�;ö�(%)
			
			AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_ENDURANCEEXTRA), VisualizeFloatPercent(p0));
			break;
			
		case 54: 	//	��������
			
			if(bLocal)
			{
				AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_PHYRESIST),  VisualizeFloatPercent(p0));
			}
			else
			{
				AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_PHYRESIST), p0);
			}
			break;
			
		case 55: 	//	���Ӽ���
			{
				//	Get skill description
				wchar_t szDesc[1024];
				CECSkill::GetDesc(p0, 1, szDesc, 1024);
				AddDescText(color, true, _AL("%s"), szDesc);
				break;
			}
		case 56: 	//	װ������
			
			if(bLocal)
			{
				if(VisualizeFloatPercent(p0) != VisualizeFloatPercent(p1))
				{
					AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_REQEXTRA), -VisualizeFloatPercent(p0));
					AddDescText(color, true, _AL("~%d"), VisualizeFloatPercent(p1));
				}
				else
				{
					AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_REQEXTRA), -VisualizeFloatPercent(p0));
				}
			}
			else
			{
				AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_REQEXTRA), -VisualizeFloatPercent(p0));
			}
			break;
			
		case 57: 	//	δ֪����
			
			AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_RANDOMPROP));
			break;
			
		case 58:	//	����ֵ�ӳ�
			
			if(bLocal)
			{
				AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_EXP), VisualizeFloatPercent(p0));
			}
			else
			{
				AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_EXP), p0);
			}
			break;
			
		case 59:	//	�����ȼ�
			
			AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_ATK_DEGREE), p0);
			break;
			
		case 60:	//	�����ȼ�
			
			AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_DEF_DEGREE), p0);
			break;
			
		case 61:	//	���з�����%��
			
			if(bLocal)
				AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_TOTAL_DEFENCE_ADD), VisualizeFloatPercent(p0));
			else
				AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_TOTAL_DEFENCE_ADD), (p0));
			
			break;
			
		case 62:	//	����֮��
			
			AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_PROFVIEW));
			break;
			
		case 63:	//	����
			
			AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_SOULPOWER), p0);
			break;
			
		case 64: 	//	��ϵ����(%)
			
			if(bLocal)
			{
				AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_GOLDRESIST),  VisualizeFloatPercent(p0));
			}
			else
			{
				AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_GOLDRESIST), p0);
			}
			break;
			
		case 65: 	//	ľϵ����(%)
			
			if(bLocal)
			{
				AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_WOODRESIST),  VisualizeFloatPercent(p0));
			}
			else
			{
				AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_WOODRESIST), p0);
			}
			break;
			
		case 66: 	//	ˮϵ����(%)
			
			if(bLocal)
			{
				AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_WATERRESIST),  VisualizeFloatPercent(p0));
			}
			else
			{
				AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_WATERRESIST), p0);
			}
			break;
			
		case 67: 	//	��ϵ����(%)
			
			if(bLocal)
			{
				AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_FIRERESIST),  VisualizeFloatPercent(p0));
			}
			else
			{
				AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_FIRERESIST), p0);
			}
			break;
			
		case 68: 	//	��ϵ����(%)
			
			if(bLocal)
			{
				AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_EARTHRESIST),  VisualizeFloatPercent(p0));
			}
			else
			{
				AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_EARTHRESIST), p0);
			}
			break;
			
		case 69: 	//	���м���(%)
			
			if(bLocal)
			{
				AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_ALLMAGICRESIST),  VisualizeFloatPercent(p0));
			}
			else
			{
				AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_ALLMAGICRESIST), p0);
			}
			break;		
			
		case 70:	//	�����ȼ���Χ������ֵ������ã�
			
			ADD_RANGE_VALUE_DESC_ID_NORMAL(ITEMDESC_ATK_DEGREE)
				break;
			
		case 71:	//	�����ȼ���Χ������ֵ������ã�
			
			ADD_RANGE_VALUE_DESC_ID_NORMAL(ITEMDESC_DEF_DEGREE)
				break;
			
		case 72:	//	����һ����(%)��Χ������ֵ������ã�
			
			ADD_RANGE_VALUE_DESC_ID_PERCENT(ITEMDESC_DEADLYSTRIKE)
				break;
			
		case 73: 	//	HP��Χ������ֵ������ã�
			
			ADD_RANGE_VALUE_DESC_STR_NORMAL(ITEMDESC_ADDHP)
				break;
			
		case 74: 	//	MP��Χ������ֵ������ã�
			
			ADD_RANGE_VALUE_DESC_STR_NORMAL(ITEMDESC_ADDMP)
				break;
			
		case 75:	//	����(%)��Χ������ֵ������ã�
			
			ADD_RANGE_VALUE_DESC_ID_PERCENT(ITEMDESC_ATKRATINGEXTRA)
				break;
			
		case 76:	//	���������Χ������ֵ������ã�
			
			ADD_RANGE_VALUE_DESC_STR_NORMAL(ITEMDESC_PHYDEFENCE)
				break;
			
		case 77:	//	���з�����Χ������ֵ������ã�
			
			ADD_RANGE_VALUE_DESC_STR_NORMAL(ITEMDESC_ALLMAGICDEF)
				break;
			
		case 78:	//	�������(%)��Χ������ֵ������ã�
			
			ADD_RANGE_VALUE_DESC_ID_PERCENT(ITEMDESC_PHYRESIST)
				break;
			
		case 79:	//	���м���(%)��Χ������ֵ������ã�
			
			ADD_RANGE_VALUE_DESC_ID_PERCENT(ITEMDESC_ALLMAGICRESIST)
				break;
			
		case 80:	//	����ʱ��(%)��Χ������ֵ������ã�
			
			ADD_RANGE_VALUE_DESC_ID_MINUS_PERCENT_1(ITEMDESC_CASTTIME)
				break;
			
		case 81:	//	�������뷶Χ������ֵ������ã�
			
			ADD_RANGE_VALUE_DESC_ID_FLOAT(ITEMDESC_ADDATKDIST)
				break;
			
		case 82:	//	MP�ָ��ٶȷ�Χ������ֵ������ã�
			
			ADD_RANGE_VALUE_DESC_ID_HALF(ITEMDESC_MPRECOVER)
				break;
			
		case 83:	//	�������(%)��Χ������ֵ������ã�
			
			ADD_RANGE_VALUE_DESC_ID_PERCENT(ITEMDESC_PHYDEFEXTRA)
				break;
			
		case 84:	//	���з���(%)��Χ������ֵ������ã�
			
			ADD_RANGE_VALUE_DESC_ID_PERCENT(ITEMDESC_TOTAL_DEFENCE_ADD)
				break;
			
		case 85:	//	HP�ָ��ٶȷ�Χ������ֵ������ã�
			
			ADD_RANGE_VALUE_DESC_ID_HALF(ITEMDESC_HPRECOVER)
				break;
			
		case 86:	//	������Χ������ֵ������ã�
			
			ADD_RANGE_VALUE_DESC_STR_NORMAL(ITEMDESC_DODGE)
				break;
			
		case 87:	//	���������޷�Χ������ֵ������ã�
			
			ADD_RANGE_VALUE_DESC_ID_NORMAL(ITEMDESC_MAXPHYDAMAGE)
				break;
			
		case 88:	//	ħ���������޷�Χ������ֵ������ã�
			
			ADD_RANGE_VALUE_DESC_ID_NORMAL(ITEMDESC_MAXMAGICDAMAGE)
				break;
			
		case 89:	//	װ������Χ������ֵ������ã�
			
			ADD_RANGE_VALUE_DESC_ID_MINUS_PERCENT_2(ITEMDESC_REQEXTRA)
				break;
			
		case 90:	//	��ħ�ȼ�
			
			AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_PENETRATION), p0);
			break;
			
		case 91:	//	��ħ�ȼ�
			
			AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_RESILIENCE), p0);
			break;
			
		case 92: 	//	+���+ħ��
			
			if(!bLocal)
			{
				if (aPEEVals)
				{
					aPEEVals[PEEI_PHYDEF]	+= p0;
					aPEEVals[PEEI_GOLDDEF]	+= p1;
					aPEEVals[PEEI_WOODDEF]	+= p1;
					aPEEVals[PEEI_WATERDEF] += p1;
					aPEEVals[PEEI_FIREDEF]	+= p1;
					aPEEVals[PEEI_EARTHDEF] += p1;
				}
			}
			
			AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_PHYDEFENCE));
			AddDescText(color, true, _AL(" %+d"), p0);
			AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_ALLMAGICDEF));
			AddDescText(color, true, _AL(" %+d"), p1);
			break;
			
		case 93: 	//	��ɫHP		
			AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_ADDHP));
			AddDescText(color, true, _AL(" %+d"), p0);
			break;
			
		case 94: 	//	��ɫHP��Χ
			ADD_RANGE_VALUE_DESC_STR_NORMAL(ITEMDESC_ADDHP)
				break;
			
		case 95: 	//	����
			AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_STRENGTH), p0);
			break;
			
		case 96: 	//	����
			AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_AGILITY), p0);
			break;
			
		case 97: 	//	����
			AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_ENERGY), p0);
			break;
			
		case 98:	//	����
			AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_VITALITY), p0);
			break;
			
		case 99: 	//	��ɫMP
			AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_ADDMP));
			AddDescText(color, true, _AL(" %+d"), p0);
			break;
			
		case 160: 	//	�����̶�ֵ
			AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_VIGOUR));
			AddDescText(color, true, _AL(" %+d"), p0);
			break;
			
		case 200:	//	����������
			
			if (aRefines)
				aRefines[REFINE_PHYDAMAGE] += p0;
			
			break;
			
		case 201:	//	����ħ������
			
			if (aRefines)
				aRefines[REFINE_MAGICDAMAGE] += p0;
			
			break;
			
		case 202:	//	�����������
			
			if (aRefines)
				aRefines[REFINE_PHYDEF] += p0;
			
			break;
			
		case 203:	//	�������
			
			if (aRefines)
				aRefines[REFINE_GOLDDEF] += p0;
			
			break;
			
		case 204:	//	����ľ��
			
			if (aRefines)
				aRefines[REFINE_WOODDEF] += p0;
			
			break;
			
		case 205:	//	����ˮ��
			
			if (aRefines)
				aRefines[REFINE_WATERDEF] += p0;
			
			break;
			
		case 206:	//	�������
			
			if (aRefines)
				aRefines[REFINE_FIREDEF] += p0;
			
			break;
			
		case 207:	//	��������
			
			if (aRefines)
				aRefines[REFINE_EARTHDEF] += p0;
			
			break;
			
		case 208:	//	����HP
			
			if (aRefines)
				aRefines[REFINE_HP] += p0;
			
			break;
			
		case 209:	//	��������
			
			if (aRefines)
				aRefines[REFINE_DODGE] += p0;
			
			break;
			
		case 210:	//	�������з���
			
			if (aRefines)
			{
				aRefines[REFINE_GOLDDEF] += p0;
				aRefines[REFINE_WOODDEF] += p0;
				aRefines[REFINE_WATERDEF] += p0;
				aRefines[REFINE_FIREDEF] += p0;
				aRefines[REFINE_EARTHDEF] += p0;
			}
			
			break;
			
		case 211:	//	���������� & ħ������
			
			if (aRefines)
			{
				aRefines[REFINE_PHYDAMAGE] += p0;
				aRefines[REFINE_MAGICDAMAGE] += p0;
			}
			
			break;
			
		case 212:	//	����������� & ħ������
			
			if (aRefines)
			{
				aRefines[REFINE_PHYDEF] += p0;
				aRefines[REFINE_GOLDDEF] += p0;
				aRefines[REFINE_WOODDEF] += p0;
				aRefines[REFINE_WATERDEF] += p0;
				aRefines[REFINE_FIREDEF] += p0;
				aRefines[REFINE_EARTHDEF] += p0;
			}
			
			break;
			
			//	�Կ��������
		case 120: 	//	�Կ̽��
			
			AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_GOLDDEFENCE));
			AddDescText(color, true, _AL(" %+d"), p0);
			break;
			
		case 121: 	//	�Կ�ľ��	
			
			AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_WOODDEFENCE));
			AddDescText(color, true, _AL(" %+d"), p0);
			break;
			
		case 122: 	//	�Կ�ˮ��
			
			AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_WATERDEFENCE));
			AddDescText(color, true, _AL(" %+d"), p0);
			break;
			
		case 123: 	//	�Կ̻��	
			
			AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_FIREDEFENCE));
			AddDescText(color, true, _AL(" %+d"), p0);
			break;
			
		case 124: 	//	�Կ�����
			
			AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_EARTHDEFENCE));
			AddDescText(color, true, _AL(" %+d"), p0);
			break;
			
		case 125: 	//	�Կ�����
			
			AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_ATKRATING), p0);
			break;
			
		case 126: 	//	�Կ̶���
			
			AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_DODGE));
			AddDescText(color, true, _AL(" %+d"), p0);
			break;
			
		case 127: 	//	�Կ�MP
			
			AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_ADDMP));
			AddDescText(color, true, _AL(" %+d"), p0);
			break;
			
		case 128: 	//	�Կ�����
			
			if(bLocal)
			{
				if(p0 != p1)
				{
					AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_VITALITY), p0);
					AddDescText(color, true, _AL("~%d"), p1);
				}
				else
				{
					AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_VITALITY), p0);
				}
			}
			else
			{
				AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_VITALITY), p0);
			}
			break;
			
		case 129: 	//	�Կ�����
			
			if(bLocal)
			{
				if(p0 != p1)
				{
					AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_STRENGTH), p0);
					AddDescText(color, true, _AL("~%d"), p1);
				}
				else
				{
					AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_STRENGTH), p0);
				}
			}
			else
			{
				AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_STRENGTH), p0);
			}
			break;
			
		case 130: 	//	�Կ�����
			
			if(bLocal)
			{
				if(p0 != p1)
				{
					AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_AGILITY), p0);
					AddDescText(color, true, _AL("~%d"), p1);
				}
				else
				{
					AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_AGILITY), p0);
				}
			}
			else
			{
				AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_AGILITY), p0);
			}
			break;
			
		case 131: 	//	�Կ�����
			
			if(bLocal)
			{
				if(p0 != p1)
				{
					AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_ENERGY), p0);
					AddDescText(color, true, _AL("~%d"), p1);
				}
				else
				{
					AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_ENERGY), p0);
				}
			}
			else
			{
				AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_ENERGY), p0);
			}
			break;
			
		case 132: 	//	�Կ�HP
			
			AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_ADDHP));
			AddDescText(color, true, _AL(" %+d"), p0);
			break;
			
		case 133: 	//	�Կ��������
			
			AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_PHYDEFENCE));
			AddDescText(color, true, _AL(" %+d"), p0);
			break;
			
		case 134: 	//	�Կ����з���
			
			AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_ALLMAGICDEF));
			AddDescText(color, true, _AL(" %+d"), p0);
			break;
			
		case 135:	//	�Կ�������
			
			AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_ADDPHYDAMAGE));
			AddDescText(color, true, _AL(" %+d"), p0);
			break;
			
		case 136:	//	�Կ�ħ������
			
			AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_ADDMAGICDAMAGE));
			AddDescText(color, true, _AL(" %+d"), p0);
			break;
			
		case 137: 	//	�Կ̽�ϵ����(%)
			
			if(bLocal)
			{
				AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_GOLDRESIST),  VisualizeFloatPercent(p0));
			}
			else
			{
				AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_GOLDRESIST), p0);
			}
			break;
			
		case 138: 	//	�Կ�ľϵ����(%)
			
			if(bLocal)
			{
				AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_WOODRESIST),  VisualizeFloatPercent(p0));
			}
			else
			{
				AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_WOODRESIST), p0);
			}
			break;
			
		case 139: 	//	�Կ�ˮϵ����(%)
			
			if(bLocal)
			{
				AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_WATERRESIST),  VisualizeFloatPercent(p0));
			}
			else
			{
				AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_WATERRESIST), p0);
			}
			break;
			
		case 140: 	//	�Կ̻�ϵ����(%)
			
			if(bLocal)
			{
				AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_FIRERESIST),  VisualizeFloatPercent(p0));
			}
			else
			{
				AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_FIRERESIST), p0);
			}
			break;
			
		case 141: 	//	�Կ���ϵ����(%)
			
			if(bLocal)
			{
				AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_EARTHRESIST),  VisualizeFloatPercent(p0));
			}
			else
			{
				AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_EARTHRESIST), p0);
			}
			break;
			
		case 142: 	//	�Կ����м���(%)
			
			if(bLocal)
			{
				AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_ALLMAGICRESIST),  VisualizeFloatPercent(p0));
			}
			else
			{
				AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_ALLMAGICRESIST), p0);
			}
			break;
			
		case 143: 	//	�Կ�����һ����
			
			if(bLocal)
			{
				AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_DEADLYSTRIKE), VisualizeFloatPercent(p0));
			}
			else
			{
				AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_DEADLYSTRIKE), p0);
			}
			break;
			
		case 144:	//	�Կ̹����ȼ�
			
			AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_ATK_DEGREE), p0);
			break;
			
		case 145:	//	�Կ̷����ȼ�
			
			AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_DEF_DEGREE), p0);
			break;
			
		case 146: 	//	�Կ���������
			
			if(bLocal)
			{
				AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_PHYRESIST),  VisualizeFloatPercent(p0));
			}
			else
			{
				AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_PHYRESIST), p0);
			}
			break;
			
		default:
			
			AddDescText(color, true, pDescTab->GetWideString(ITEMDESC_ERRORPROP), idProp);
			break;
	}
	}

#undef ADD_RANGE_VALUE_DESC_ID
#undef ADD_RANGE_VALUE_DESC_ID_NORMAL
#undef ADD_RANGE_VALUE_DESC_ID_FLOAT
#undef ADD_RANGE_VALUE_DESC_ID_PERCENT
#undef ADD_RANGE_VALUE_DESC_ID_MINUS_PERCENT_1
#undef ADD_RANGE_VALUE_DESC_ID_MINUS_PERCENT_2
#undef ADD_RANGE_VALUE_DESC_ID_HALF
#undef ADD_RANGE_VALUE_DESC_STR
#undef ADD_RANGE_VALUE_DESC_STR_NORMAL
}

//	Build add-ons properties description
void CECIvtrEquip::BuildAddOnPropDesc(int* aPEEVals, int* aRefines)
{
	if (!m_aProps.GetSize())
		return;

	//	Change color
	CECStringTab* pDescTab = g_pGame->GetItemDesc();
	m_strDesc += pDescTab->GetWideString(ITEMDESC_COL_LIGHTBLUE);

	for (int i=0; i < m_aProps.GetSize(); i++)
	{
		const PROPERTY& prop = m_aProps[i];

		//	Properties added by Embedded stone will be printed by
		//	BuildTesseraDesc() later.
		//	Ignore suite properties also.
		if (prop.bEmbed || prop.bSuite || prop.bEngraved)
			continue;

		AddOneAddOnPropDesc(prop.iType, prop.aParams, aPEEVals, aRefines, prop.bLocal);
	}
}

int	CECIvtrEquip::GetSoulPowerAdded()
{
	//	����������������ӵĻ���
	int nAdded(0);
	int nProperty = GetPropertyNum();
	CECGame::ItemExtPropTable& PropTab = g_pGame->GetItemExtPropTable();
	for (int i(0); i < nProperty; ++i)
	{
		const PROPERTY &prop = GetProperty(i);
		if (prop.iNumParam > 0)
		{
			CECGame::ItemExtPropTable::pair_type Pair = PropTab.get(prop.iType);
			if (Pair.second && (*Pair.first == 63))
			{
				nAdded += prop.aParams[0];
			}
		}
	}
	return nAdded;
}

//	Build tessera description
void CECIvtrEquip::BuildTesseraDesc()
{
	if (!m_aHoles.GetSize())
		return;

	CECStringTab* pDescTab = g_pGame->GetItemDesc();
	int cyanine = ITEMDESC_COL_CYANINE;

	for (int i=0; i < m_aHoles.GetSize(); i++)
	{
		if (!m_aHoles[i])
			continue;

		//	Get item name
		CECIvtrItem* pItem = CECIvtrItem::CreateItem(m_aHoles[i], 0, 1);
		const ACHAR* szText = _AL("null");
		if (pItem->GetClassID() == CECIvtrItem::ICID_STONE)
		{
			if (m_iCID == ICID_WEAPON)
				szText = ((CECIvtrStone*)pItem)->GetDBEssence()->weapon_desc;
			else
				szText = ((CECIvtrStone*)pItem)->GetDBEssence()->armor_desc;
		}
		
		AddDescText(cyanine, true, pDescTab->GetWideString(ITEMDESC_2STRINGS), pItem->GetName(), szText);
		delete pItem;
	}
}

//	Parse properties
void CECIvtrEquip::ParseProperties()
{
	m_iRefineLvl	= 0;
	m_byPropNum		= 0;
	m_byEmbedNum	= 0;

	if (!m_aProps.GetSize())
		return;

	CECGame::ItemExtPropTable& PropTab = g_pGame->GetItemExtPropTable();
	int iLevel = 0;

	for (int i=0; i < m_aProps.GetSize(); i++)
	{
		const PROPERTY& prop = m_aProps[i];

		//	Properties added by Embedded stone will be printed by
		//	BuildTesseraDesc() later
		if (prop.bEmbed)
		{
			m_byEmbedNum++;
			continue;
		}
		else if (prop.bSuite)
			continue;
		else if (prop.bEngraved)
			continue;

		CECGame::ItemExtPropTable::pair_type Pair = PropTab.get(prop.iType);
		BYTE byPropType = Pair.second ? *Pair.first : 0xff;

		switch (byPropType)
		{
		case 200:	//	����������
		case 201:	//	����ħ������
		case 202:	//	�����������
		case 203:	//	�������
		case 204:	//	����ľ��
		case 205:	//	����ˮ��
		case 206:	//	�������
		case 207:	//	��������
		case 208:	//	����HP
		case 209:	//	��������
		case 210:	//	�������з���
		case 211:	//	���������� & ħ������
		case 212:	//	����������� & ħ������

			iLevel = prop.aParams[1];
			break;
		}

		if (iLevel)
			m_iRefineLvl = iLevel;
		else
			m_byPropNum++;
	}
}

//	Check whether this equipment belongs to a suite
int CECIvtrEquip::GetSuiteID()
{
	CECGame::SuiteEquipTable& SuiteTab = g_pGame->GetSuiteEquipTable();
	CECGame::SuiteEquipTable::pair_type pair = SuiteTab.get(m_tid);
	return pair.second ? *pair.first : 0;
}

//	Get item description text for suite information
void CECIvtrEquip::AddSuiteDesc()
{
	int idSuite = GetSuiteID();
	if (!idSuite)
		return;	//	This equipment isn't one of any suite

	//	Get suite info
	DATA_TYPE DataType = DT_INVALID;
	elementdataman* pDataMan = g_pGame->GetElementDataMan();
	const void* pData = pDataMan->get_data_ptr(idSuite, ID_SPACE_ESSENCE, DataType);
	if (DataType != DT_SUITE_ESSENCE)
	{
		ASSERT(DataType == DT_SUITE_ESSENCE);
		return;
	}

	const SUITE_ESSENCE* pSuiteEss = (const SUITE_ESSENCE*)pData;
	CECHostPlayer* pHostPlayer = g_pGame->GetGameRun()->GetHostPlayer();

	//	Colors
	int iNameCol = DecideNameCol();
	int lblue = ITEMDESC_COL_LIGHTBLUE;
	int green = ITEMDESC_COL_GREEN;
	int gray = ITEMDESC_COL_GRAY;
	int white = ITEMDESC_COL_WHITE;
	int yellow = ITEMDESC_COL_YELLOW;

	//	Save current description
	AWString strCurDesc = m_strDesc;
	bool bShowDetail = true;
	int i, j;

	if (m_pDescIvtr != pHostPlayer->GetEquipment())
		bShowDetail = false;
	else
	{
		//	Check whether this equipment is in host's equipment pack
		for (i=0; i < m_pDescIvtr->GetSize(); i++)
		{
			if (this == m_pDescIvtr->GetItem(i))
				break;
		}

		if (i == m_pDescIvtr->GetSize())
			bShowDetail = false;
	}

	if (!bShowDetail)
	{
		//	Isn't equipment inventory, only add total suite number info.
		m_strDesc = _AL("\\r\\r");
		AddDescText(iNameCol, false, _AL("%s (%d)"), pSuiteEss->name, pSuiteEss->max_equips);
		m_strDesc = strCurDesc + m_strDesc;
		return;
	}

	//	Maximum number of suite items
	enum
	{
		MAX_NUM = 12
	};

	struct SUITEITEM
	{
		bool bEnable;
		int tid;
		wchar_t szName[32];

	} aSuiteItems[MAX_NUM];

	//	Fill suite item array
	for (i=0; i < pSuiteEss->max_equips; i++)
	{
		SUITEITEM& item = aSuiteItems[i];
		item.bEnable = false;
		item.tid = (int)pSuiteEss->equipments[i].id;
		item.szName[0] = '\0';

		//	Get item name
		CECIvtrItem* pEquipItem = CECIvtrItem::CreateItem(item.tid, 0, 1);
		if (pEquipItem)
		{
			a_strcpy(item.szName, pEquipItem->GetName());
			delete pEquipItem;
		}
		else
		{
			ASSERT(0);
			item.tid = 0;
		}
	}

	//	Get equipped suite item list
	int iItemCnt, aEquipped[MAX_NUM];
	iItemCnt = pHostPlayer->GetEquippedSuiteItem(idSuite, aEquipped);
	if (!iItemCnt)
		return;

	m_strDesc = _AL("\\r\\r");

	//	Update suite item status
	for (i=0; i < MAX_NUM; i++)
	{
		SUITEITEM& item = aSuiteItems[i];

		for (j=0; j < iItemCnt; j++)
		{
			if (item.tid == aEquipped[j])
			{
				item.bEnable = true;
				break;
			}
		}
	}

	//	Build suite addon properties at first
	if (iItemCnt > 1)
	{
		//	Change color
		AddDescText(lblue, false, _AL(""));

		for (i=1; i < iItemCnt; i++)
		{
			int idProp = (int)pSuiteEss->addons[i-1].id;
			if (!idProp)
				continue;

			pData = pDataMan->get_data_ptr(idProp, ID_SPACE_ADDON, DataType);
			if (DataType != DT_EQUIPMENT_ADDON)
			{
				ASSERT(DataType == DT_EQUIPMENT_ADDON);
				continue;
			}

			const EQUIPMENT_ADDON* pAddOn = (const EQUIPMENT_ADDON*)pData;

			AddDescText(-1, false, _AL("(%d) "), i+1);
		//	AddOneAddOnPropDesc(pAddOn->id, pAddOn->param1, pAddOn->param2, pAddOn->param3, NULL, NULL);
			AddDescText(-1, true, _AL("%s"), pAddOn->name);
		}
	}
	
	//	Add suite name
	AddDescText(yellow/*iNameCol*/, true, _AL("%s (%d / %d)"), pSuiteEss->name, iItemCnt, pSuiteEss->max_equips);
	
	//	List suite item names
	for (i=0; i < pSuiteEss->max_equips; i++)
	{
		SUITEITEM& item = aSuiteItems[i];
		if (!item.tid)
			continue;

		int col = item.bEnable ? green : gray;
		bool bRet = (i == pSuiteEss->max_equips-1) ? false : true;

		//	Add item name
		AddDescText(col, bRet, _AL("  %s"), item.szName);
	}

	m_strDesc = strCurDesc + m_strDesc;
}
//  Get equip destroying description
void CECIvtrEquip::AddDestroyingDesc(int iDropItemID, int iNum)
{
	if(!IsDestroying() || iDropItemID == 0 || iNum == 0)
		return;

	//	Get destroying info
	DATA_TYPE DataType = DT_INVALID;
	elementdataman* pDataMan = g_pGame->GetElementDataMan();
	ELEMENT_ESSENCE* pEssence = (ELEMENT_ESSENCE*)pDataMan->get_data_ptr(iDropItemID, ID_SPACE_ESSENCE, DataType);
	
	if(DataType != DT_ELEMENT_ESSENCE)
	{
		ASSERT(0);
		return;
	}

	CECStringTab* pDescTab = g_pGame->GetItemDesc();
	int red = ITEMDESC_COL_RED;

	m_strDesc += _AL("\\r");
	AddDescText(red, true, pDescTab->GetWideString(ITEMDESC_EQUIP_DESTROYING));

	AddDescText(red, true, pDescTab->GetWideString(ITEMDESC_EQUIP_REPAIR_NEED_ITEM), pEssence->name);
	AddDescText(red, true, pDescTab->GetWideString(ITEMDESC_EQUIP_REPAIR_NEED_ITEMCNT), (int)(ceil(iNum*1.2)));
}

void CECIvtrEquip::AddReputationReqDesc()
{
	if (m_iReputationReq)
	{
		CECStringTab* pDescTab = g_pGame->GetItemDesc();
		CECHostPlayer* pHost = g_pGame->GetGameRun()->GetHostPlayer();
		int col = pHost->GetReputation() >= m_iReputationReq ? ITEMDESC_COL_WHITE : ITEMDESC_COL_RED;
		AddDescText(col, true, pDescTab->GetWideString(ITEMDESC_REPUTATION_REQ), m_iReputationReq);
	}
}

void CECIvtrEquip::AddSharpenerDesc()
{	
	if (!m_aProps.GetSize())
		return;

	CECStringTab* pDescTab = g_pGame->GetItemDesc();
	int color = ITEMDESC_COL_LIGHTBLUE;
	CECGame::ItemExtPropTable& PropTab = g_pGame->GetItemExtPropTable();

	int i(0);
	
	//	����ĥ��ʯ���ԡ��ж������ʱ���Ƿ�һ�£�������ʣ��ʱ����ʾ������ĥ��ʯ���Ե�ĩβ��
	//
	bool bSameExpireTime(true);
	bool bFindFirst(true);
	int  lastExpireTime(0);
	for (i = 0; i < m_aProps.GetSize(); i++)
	{
		const PROPERTY& prop = m_aProps[i];

		if (prop.bEmbed || prop.bSuite || prop.bEngraved)
			continue;

		CECGame::ItemExtPropTable::pair_type Pair = PropTab.get(prop.iType);
		BYTE byPropType = Pair.second ? *Pair.first : 0xff;

		if (!IsSharpenerProperty(byPropType))
			continue;

		int p1 = prop.aParams[1];
		if (bFindFirst)
		{
			//	���ҵ���һ��ĥ��ʯ����
			lastExpireTime = p1;
			bFindFirst = false;
		}
		else
		{
			//	���ҵ����ĥ��ʯ����
			if (p1 != lastExpireTime)
			{
				//	��֮ǰ���ԵĹ���ʱ�䲻ͬ
				bSameExpireTime = false;
				break;
			}
		}
	}

	if (bFindFirst)
	{
		//	��Ȼ����ҵ�һ��ĥ��ʯ���ԣ�˵��ĥ��ʯ���Դ�û�ҵ���
		return;
	}

	bool bFirstProp(true);
	for (i=0; i < m_aProps.GetSize(); i++)
	{
		const PROPERTY& prop = m_aProps[i];

		if (prop.bEmbed || prop.bSuite || prop.bEngraved)
			continue;

		CECGame::ItemExtPropTable::pair_type Pair = PropTab.get(prop.iType);
		BYTE byPropType = Pair.second ? *Pair.first : 0xff;

		if (!IsSharpenerProperty(byPropType))
			continue;

		if (bFirstProp)
		{
			m_strDesc += _AL("\\r");
			bFirstProp = false;
		}

		//	����
		m_strDesc += _AL("\\r");

		int p0 = prop.aParams[0];	//	��һ����ֵ
		int p1 = prop.aParams[1];	//	����ʱ��
		
		//	��������
		switch(byPropType)
		{
		case 100:	//	ĥ��ʯ������
			AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_ADDPHYDAMAGE));
			AddDescText(color, false, _AL(" %+d"), p0);
			break;

		case 101:	//	ĥ��ʯ����������
			AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_MAXPHYDAMAGE), p0);
			break;

		case 102:	//	ĥ��ʯħ������
			AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_ADDMAGICDAMAGE));
			AddDescText(color, false, _AL(" %+d"), p0);
			break;

		case 103:	//	ĥ��ʯħ����������
			AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_MAXMAGICDAMAGE), p0);
			break;

		case 104:	//	ĥ��ʯ�������
			AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_PHYDEFENCE));
			AddDescText(color, false, _AL(" %+d"), p0);
			break;

		case 105:	//	ĥ��ʯHP
			AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_ADDHP));
			AddDescText(color, false, _AL(" %+d"), p0);
			break;

		case 106:	//	ĥ��ʯ����
			AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_STRENGTH), p0);
			break;

		case 107:	//	ĥ��ʯ����
			AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_AGILITY), p0);
			break;

		case 108:	//	ĥ��ʯ����
			AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_ENERGY), p0);
			break;

		case 109:	//	ĥ��ʯ������
			AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_ATKRATING), p0);
			break;

		case 110:	//	ĥ��ʯ����һ����
			if (prop.bLocal)
				AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_DEADLYSTRIKE), VisualizeFloatPercent(p0));
			else
				AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_DEADLYSTRIKE), p0);
			break;

		case 111:	//	ĥ��ʯ�����ȼ�
			AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_ATK_DEGREE), p0);
			break;

		case 112:	//	ĥ��ʯ�����ȼ�
			AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_DEF_DEGREE), p0);
			break;

		case 113:	//	ĥ��ʯ����(%d)
			if (prop.bLocal)
				AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_CASTTIME), -VisualizeFloatPercent(p0));
			else
				AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_CASTTIME), -p0);
			break;

		case 114:	//	ĥ��ʯħ������
			AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_ALLMAGICDEF));
			AddDescText(color, false, _AL(" %+d"), p0);
			break;

		case 115:	//	ĥ��ʯ��˼���
			AddDescText(color, false, pDescTab->GetWideString(ITEMDESC_ADDRIDEONPETSPEED), *(float*)&p0);
			break;

		default:
			ASSERT(false);
			continue;
		}
		
		//	������Թ���ʱ�䲻ͬ�����ڴ��������Թ���ʱ��
		if (!bSameExpireTime)
		{
			if (p1 != 0)
			{
				m_strDesc += _AL("  ");
				AddExpireTimeDesc(p1);
				TrimLastReturn();
			}
		}
	}

	//	������Թ���ʱ����ͬ�����ڴ��������Թ���ʱ��
	if (bSameExpireTime)
	{
		if (lastExpireTime != 0)
		{
			m_strDesc += _AL("\\r");
			AddExpireTimeDesc(lastExpireTime);
			TrimLastReturn();
		}
	}
}

void CECIvtrEquip::AddEngravedDesc()
{
	if (!m_aProps.GetSize())
		return;

	//	Change color
	CECStringTab* pDescTab = g_pGame->GetItemDesc();
	bool bFirstProp(true);

	for (int i=0; i < m_aProps.GetSize(); i++)
	{
		const PROPERTY& prop = m_aProps[i];
		if (prop.bEngraved)
		{
			if (bFirstProp)
			{
				bFirstProp = false;
				m_strDesc += _AL("\\r");
				m_strDesc += pDescTab->GetWideString(ITEMDESC_COL_YELLOW);
			}
			AddOneAddOnPropDesc(prop.iType, prop.aParams, NULL, NULL, prop.bLocal);
		}
	}

	if (!bFirstProp)
	{
		//	�����Ա���ӹ�
		TrimLastReturn();
	}
}

void CECIvtrEquip::AddMakerDesc()
{
	if (!m_strMaker.IsEmpty())
	{
		m_strDesc += _AL("\\r");
		if (GetMadeFromFlag() != IMT_SIGN)
		{
			CECStringTab* pDescTab = g_pGame->GetItemDesc();
			AddDescText(ITEMDESC_COL_GREEN, false, pDescTab->GetWideString(ITEMDESC_MADEFROM), m_strMaker);
		}
		else
		{
			//	Ϊ���� m_strMaker �г��� % �ŵ������ַ�����ʹ�� printf �ȷ�ʽ���
			m_strDesc += m_strMaker;
		}
	}
}

bool CECIvtrEquip::IsRare() const
{
	return CECIvtrItem::IsRare() || m_iRefineLvl >= 3;
}

int CECIvtrEquip::GetEngravedPropertyNum()
{
	int num = 0;
	for (int i=0;i<GetPropertyNum();i++){
		if (GetProperty(i).bEngraved)
			num++;
	}

	return num;
}
///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECIvtrEquipAddonDesc
//	
///////////////////////////////////////////////////////////////////////////
bool CECIvtrEquipAddonDesc::SetAddon(unsigned int id_equip_addon)
{
	bool bRet(false);
	
	while (true)
	{
		if (!id_equip_addon)	break;
		
		elementdataman* pDataMan = g_pGame->GetElementDataMan();
		DATA_TYPE dt = DT_INVALID;
		const EQUIPMENT_ADDON *pAddon = (const EQUIPMENT_ADDON *)pDataMan->get_data_ptr(id_equip_addon, ID_SPACE_ADDON, dt);
		if (!pAddon)	break;

		if (m_pAddon)
		{
			if (pAddon == m_pAddon)
			{
				bRet = true;
				break;
			}

			//	���ԭ������
			m_strDesc.Empty();
		}
		m_pAddon = pAddon;

		AddOneAddOnPropDesc(m_pAddon->id, m_pAddon->param1, m_pAddon->param2, m_pAddon->param3, NULL, NULL, true);

		bRet = true;
		break;		
	}

	return bRet;
}

const wchar_t* CECIvtrEquip::GetPreviewInfo()
{
	m_strDesc = _AL("");
	BuildAddOnPropDesc(NULL,NULL);
	return m_strDesc;
}

bool CECIvtrEquip::GetRefineEffect(abase::vector<ACString> &strEffects, const ACString &clrAttribute, const ACString &clrEffect)
{
	strEffects.clear();
	while (!m_bNeedUpdate)
	{
		//	�����µľ���Ч��
		unsigned int addon_id = GetRefineAddOn();
		DATA_TYPE dataType(DT_INVALID);
		const EQUIPMENT_ADDON* pType = (const EQUIPMENT_ADDON*)g_pGame->GetElementDataMan()->get_data_ptr(addon_id, ID_SPACE_ADDON, dataType);
		if (DT_EQUIPMENT_ADDON != dataType){
			break;
		}
		int aRefineBase[MAX_REFINEINDEX] = {0};
		AWString strDesc = m_strDesc;
		m_strDesc.Empty();
		AddOneAddOnPropDesc(pType->id, pType->param1, pType->param2, pType->param3, NULL, aRefineBase, true);
		m_strDesc = strDesc;
		
		//	���㵱ǰ����Ч��
		int aPEEVals[MAX_PEEINDEX] = {0};
		int aRefines[MAX_REFINEINDEX] = {0};
		strDesc = m_strDesc;
		m_strDesc.Empty();
		BuildAddOnPropDesc(aPEEVals, aRefines);
		m_strDesc = strDesc;
		
		//	���µ�Ч�������������о����������ֵ�����ϵ���Ч����
		RefineEffect effect(aPEEVals, aRefines, clrAttribute, clrEffect);
		ACString strEffect;
		for (int i(0); i < MAX_REFINEINDEX; ++ i)
		{
			if (aRefineBase[i] <= 0){
				continue;
			}
			int newEffect = CalcRefineEffect(GetRefineLevel()+1, aRefineBase[i]);
			int incEffect = newEffect - aRefines[i];
			incEffect = a_Max(0, incEffect);
			effect.Set(i, incEffect);
			if (GetRefineEffectFor(strEffect, effect)){
				strEffects.push_back(strEffect);
			}
		}
		
		break;
	}
	return !strEffects.empty();
}

int CECIvtrEquip::CalcRefineEffect(int refineLevel, int baseEffect)
{
	const int MAX_REFINE_LEVEL = 12;
	static const float refine_factor[MAX_REFINE_LEVEL + 1] = 
	{
		0,
		1.0f,
		2.0f,
		3.05f,
		4.3f,
		5.75f,
		7.55f,
		9.95f,
		13.f,
		17.05f,
		22.3f,
		29.f,
		37.5f,
	};
	if (refineLevel >= 0 && refineLevel <= MAX_REFINE_LEVEL){
		return (int)(baseEffect * refine_factor[refineLevel] + 0.1f);
	}
	return 0;
}

const wchar_t* CECIvtrEquip::GetAddOnPropDesc(int num, int* addons)
{
	int aPEEVals[MAX_PEEINDEX];
	int aRefines[MAX_REFINEINDEX];
	memset(aPEEVals, 0, sizeof (aPEEVals));
	memset(aRefines, 0, sizeof (aRefines));
	
	m_strDesc = _AL("");
	if (!m_aProps.GetSize())
		return m_strDesc;	
	//	Change color
	CECStringTab* pDescTab = g_pGame->GetItemDesc();
	m_strDesc += pDescTab->GetWideString(ITEMDESC_COL_LIGHTBLUE);
	
	for (int i=0; i < m_aProps.GetSize(); i++)
	{
		const PROPERTY& prop = m_aProps[i];
		if (prop.bEmbed || prop.bSuite || prop.bEngraved)
			continue;	
		for (int j = 0; j< num; j++){
			if ((addons[j] & 0x1fff) == prop.iType){		//�����������͵ĸ�������ID����ת����ȥ��������Ϣ
				AddOneAddOnPropDesc(prop.iType, prop.aParams, aPEEVals, aRefines, prop.bLocal);
				break;
			}
		}
	}
	return m_strDesc;
}

const wchar_t* CECIvtrEquip::GetEngraveDesc()
{
	int aPEEVals[MAX_PEEINDEX];
	int aRefines[MAX_REFINEINDEX];
	memset(aPEEVals, 0, sizeof (aPEEVals));
	memset(aRefines, 0, sizeof (aRefines));

	m_strDesc = _AL("");
	if (!m_aProps.GetSize())
		return m_strDesc;	
	//	Change color
	CECStringTab* pDescTab = g_pGame->GetItemDesc();
	m_strDesc += pDescTab->GetWideString(ITEMDESC_COL_YELLOW);
	
	for (int i=0; i < m_aProps.GetSize(); i++)
	{
		const PROPERTY& prop = m_aProps[i];
		if (prop.bEngraved)
			AddOneAddOnPropDesc(prop.iType, prop.aParams, aPEEVals, aRefines, prop.bLocal);	
	}
	return m_strDesc;
}
//	CECIvtrEquip::RefineEffect
void CECIvtrEquip::RefineEffect::Set(int refineIndex, int incEffect)
{
	m_refineIndex = refineIndex;
	m_incEffect = incEffect;
}