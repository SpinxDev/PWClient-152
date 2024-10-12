  /*
 * FILE: glb_Game.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2003/12/16
 *
 * HISTORY: 
 *
 * Copyright (c) 2003 Archosaur Studio, All Rights Reserved.
 */
#include "EC_Game.h"
#include "../CElementClient/EC_IvtrTypes.h"
#include "EC_Global.h"
#include "EC_Instance.h"
#include "../CElementClient/EC_RoleTypes.h"
#include "../CElementClient/EC_FixedMsg.h"
#include "../CCommon/ExpTypes.h"
#include "../CCommon/itemdataman.h"
#include "../CCommon/elementdataman.h"
#include <ALog.h>
#include <ElementSkill.h>
#include <AScriptFile.h>
#include <AWScriptFile.h>
#include <AFileImage.h>
#include <AFI.h>
#include <AUICTranslate.h>

#define new A_DEBUG_NEW

//	 Gameversion
DWORD GAME_BUILD = 2313;

#define ELEMENTSKILL_VERSION	15

///////////////////////////////////////////////////////////////////////////
// for itemdataman we create some dummy functions
///////////////////////////////////////////////////////////////////////////
void set_to_classid(DATA_TYPE type, item_data * data, int major_type)
{
}
int addon_generate_arg(DATA_TYPE type, addon_data & data, int arg_num/*初始的参数个数*/)
{
	return arg_num;
} 
int addon_update_ess_data(const addon_data & data, void * essence,size_t ess_size, prerequisition * require)
{
	return 0;
}
void update_require_data(prerequisition *require)
{
	require->durability *= ENDURANCE_SCALE;
	require->max_durability *= ENDURANCE_SCALE;
}
void get_item_guid(int id, int& g1, int& g2)
{
	g1 = 0;
	g2 = 1;
}

static CECGame s_Game;

CECGame * g_pGame = NULL;

CECGame::CECGame():
m_ItemExtProps(1024),
m_SuiteEquipTab(512),
m_ItemColTab(64),
m_InstTab(64)
{
	m_pElementDataMan = NULL;
	m_pItemDataMan		= NULL;
	g_pGame = this;
}

CECGame::~CECGame()
{
}

//	Get item's extend description string
const wchar_t* CECGame::GetItemExtDesc(int tid)
{
	return m_ItemExtDesc.GetWideString(tid);
}

//	Get item name color
int CECGame::GetItemNameColorIdx(int tid, int iDefIndex/* 0 */)
{
	int iIndex = iDefIndex;
	
	ItemColTable::pair_type Pair = m_ItemColTab.get(tid);
	if (Pair.second)
		iIndex = *Pair.first;
	
	if (iIndex < 0 || iIndex >= 10)
	{
		ASSERT(iIndex >= 0 && iIndex < 10);
		iIndex = 0;
	}
	
	return iIndex;
}

void CECGame::FilterBadWords(ACString &str)
{
	int i, j, nPos;
	
	ACString strLwr((const ACHAR *)str);
	strLwr.MakeLower();
	for( i = 0; i < (int)m_vecBadWords.size(); i++ )
	{
		nPos = 0;
		while( (nPos = strLwr.Find(m_vecBadWords[i], nPos)) >= 0 )
		{
			for( j = 0; j < m_vecBadWords[i].GetLength(); j++ )
			{
				str[nPos] = '*';
				nPos ++;
			}
		} 
	}
}

void CECGame::AUI_ConvertChatString(const ACHAR *pszChat, ACHAR *pszConv, bool bName)
{
	int i, nLen = 0;
	if (!pszChat || !pszConv)
		return;
	
	pszConv[0] = 0;
	for( i = 0; i < (int)a_strlen(pszChat); i++ )
	{
		if( pszChat[i] == '^' )
		{
			pszConv[nLen] = '^';
			pszConv[nLen + 1] = '^';
			nLen += 2;
		}
		else if( pszChat[i] == '&' )
		{
			pszConv[nLen] = '^';
			pszConv[nLen + 1] = '&';
			nLen += 2;
		}
		else
		{
			pszConv[nLen] = pszChat[i];
			nLen++;
		}
	}
	pszConv[nLen] = 0;
}

ACString CECGame::AUI_ConvertHintString(const ACHAR *szText)
{
	ACString str;
	if (szText) str = szText;
	
	if (!str.IsEmpty())
	{
		//	转换 ^ & 字符
		ACHAR szTemp[1024];
		AUI_ConvertChatString(str, szTemp);
		str = szTemp;
		
		//	转换 \ 字符
		int count = str.GetLength();
		int nLen = 0;
		szTemp[0] = 0;
		for(int i = 0; i < count; i++ )
		{
			if( str[i] == '\\' )
			{
				szTemp[nLen] = '\\';
				szTemp[nLen + 1] = '\\';
				nLen += 2;
			}
			else
			{
				szTemp[nLen] = str[i];
				nLen++;
			}
		}
		szTemp[nLen] = 0;
		str = szTemp;
	}
	
	return str;
}

//	Get instance by ID
CECInstance* CECGame::GetInstance(int id)
{
	InstTable::pair_type Pair = m_InstTab.get(id);
	return Pair.second ? *Pair.first : NULL;
}

bool CECGame::Init()
{
	bool result(false);

	while (true)
	{
		a_LogOutput(1, "Build version %d", GAME_BUILD);

		// first of all verify the ElementSkill version
		if( ELEMENTSKILL_VERSION != GNET::ElementSkill::GetVersion() )
		{
			a_LogOutput(1, "CECGame::Init, ElementSkill version error!");
			break;
		}

		//	Load fixed messages
		if (!m_FixedMsgs.Init("Configs\\fixed_msg.txt", true))
			break;
		
		//	Load item description strings
		if (!m_ItemDesc.Init("Configs\\item_desc.txt", true))
			break;
		
		//	Load item extend description
		if (!m_ItemExtDesc.Init("Configs\\item_ext_desc.txt", true))
			break;
		
		//	Load skill description strings
		if (!m_SkillDesc.Init("Configs\\skillstr.txt", true))
			break;
		
		//	Load item properties
		if (!LoadItemExtProps())
			break;

		// Load Bad Words
		if (!LoadBadWords())
			break;
		
		//	Load strings added by ElementHintTool
		if (!ImportStringTable())
			break;

		// Load instance info
		if (!LoadInstanceInfo())
			break;
		
		char dataPath[] = "data\\elements.data";
		BYTE md5[16];
		if( !glb_CalcFileMD5(dataPath, md5) )
		{
			a_LogOutput(1, "%s is missing...", dataPath);
			break;
		}
		a_LogOutput(1, "elements data's md5: [%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x]", md5[0], md5[1], md5[2], md5[3],
			md5[4], md5[5], md5[6], md5[7], md5[8], md5[9], md5[10], md5[11], md5[12], md5[13], md5[14], md5[15]);
		
		m_pItemDataMan = new itemdataman();
		//	now load templates from file
		if (0 != m_pItemDataMan->load_data(dataPath))
		{
			a_LogOutput(1, "CECGame::Init, failed to load %s", dataPath);
			break;
		}
		m_pElementDataMan = m_pItemDataMan->GetElementDataMan();
		
		//	Build suite equipment table after elementdata.data has been loaded
		BuildSuiteEquipTab();
		
		result = true;
		break;
	}

	return result;
}

void CECGame::Release()
{
	if( m_pItemDataMan )
	{
		delete m_pItemDataMan;
		m_pItemDataMan = NULL;
	}
	m_pElementDataMan = NULL;
	
	m_FixedMsgs.Release();
	m_ItemDesc.Release();
	m_ItemExtDesc.Release();
	m_SkillDesc.Release();
	m_ItemExtProps.clear();
	m_SuiteEquipTab.clear();
	m_ItemColTab.clear();
	m_vecBadWords.clear();

	//	Release all instance information
	InstTable::iterator it = m_InstTab.begin();
	for (; it != m_InstTab.end(); ++it)
	{
		CECInstance* pInst = *it.value();
		delete pInst;
	}	
	m_InstTab.clear();
}

//	Load item extend properties
bool CECGame::LoadItemExtProps()
{
	//	Load item extend properties
	AScriptFile sf;
	char szFile1[] = "Configs\\item_ext_prop.txt";
	if (!sf.Open(szFile1))
	{
		a_LogOutput(1, "CECGame::LoadItemExtProps, failed to open %s", szFile1);
		return false;
	}
	
	BYTE byType = 0xff;
	
	while (sf.GetNextToken(true))
	{
		//	Get current tpye
		byType = (BYTE)sf.GetNextTokenAsInt(false);
		
		if (!sf.MatchToken("{", false))
		{
			ASSERT(0);
			return false;
		}
		
		while (sf.PeekNextToken(true))
		{
			if (!stricmp(sf.m_szToken, "}"))
			{
				sf.GetNextToken(true);
				break;
			}
			
			int idProp = sf.GetNextTokenAsInt(true);
			if (!m_ItemExtProps.put(idProp, byType))
			{
				//	Property id collision, shouldn't happen
				a_LogOutput(1, "%s: ID %d already defined.", szFile1, idProp);
				ASSERT(0);
			}
		}
	}
	
	sf.Close();
	
	//	Load item color table
	char szFile2[] = "Configs\\item_color.txt";
	if (sf.Open(szFile2))
	{
		while (sf.PeekNextToken(true))
		{
			//	Get item id
			int idItem = sf.GetNextTokenAsInt(true);
			//	Get color index
			BYTE byCol = (BYTE)sf.GetNextTokenAsInt(false);
			
			if (!m_ItemColTab.put(idItem, byCol))
			{
				//	Property id collision, shouldn't happen
				a_LogOutput(1, "%s: ID %d already defined.", szFile2, idItem);
				ASSERT(0);
			}
		}
		
		sf.Close();
	}
	else
	{
		a_LogOutput(1, "CECGame::LoadItemExtProps, failed to open file %s", szFile2);
		return false;
	}
	
	return true;
}


//	Build suite equipment table
void CECGame::BuildSuiteEquipTab()
{
	if (!m_pElementDataMan)
		return;
	
	DATA_TYPE DataType = DT_INVALID;
	
	int tid = m_pElementDataMan->get_first_data_id(ID_SPACE_ESSENCE, DataType);
	
	while (tid)
	{
		const void* pData = m_pElementDataMan->get_data_ptr(tid, ID_SPACE_ESSENCE, DataType);
		
		if (DataType == DT_SUITE_ESSENCE)
		{
			SUITE_ESSENCE* pSuiteEss = (SUITE_ESSENCE*)pData;
			pSuiteEss->max_equips = 0;
			for (int i=0; i<12; i++)
			{
				if( pSuiteEss->equipments[i].id )
				{
					pSuiteEss->max_equips ++;
					bool bVal = m_SuiteEquipTab.put(pSuiteEss->equipments[i].id, tid);
					ASSERT(bVal);
				}
			}
		}
		if (DataType == DT_POKER_SUITE_ESSENCE)
		{
			POKER_SUITE_ESSENCE* pSuiteEss = (POKER_SUITE_ESSENCE*)pData;
			for (int i=0; i<6; i++)
			{
				if( pSuiteEss->list[i] )
				{
					bool bVal = m_SuiteEquipTab.put(pSuiteEss->list[i], tid);
					ASSERT(bVal);
				}
			}
		}
		
		//	Get next item
		tid = m_pElementDataMan->get_next_data_id(ID_SPACE_ESSENCE, DataType);
	}
}

//	Load bad words
bool CECGame::LoadBadWords()
{
	AWScriptFile s;
	char szFile[] = "Configs\\BadWords.txt";
	if (!s.Open(szFile)){
		a_LogOutput(1, "CECGame::LoadBadWords, failed to open file %s", szFile);
		return false;
	}
	
	while (s.GetNextToken(true))
	{
		wcslwr(s.m_szToken);
		m_vecBadWords.push_back(s.m_szToken);
	}

	s.Close();

	return true;
}

//	Load instance information from file
bool CECGame::LoadInstanceInfo()
{
	AWScriptFile sf;
	char szFile[] = "Configs\\instance.txt";
	if (!sf.Open(szFile))
	{
		a_LogOutput(1, "CECGame::LoadInstanceInfo, failed to open file %s", szFile);
		return false;
	}
	
	while (sf.PeekNextToken(true))
	{
		CECInstance* pInst = new CECInstance;
		if (!pInst || !pInst->Load(&sf))
		{
			return false;
		}
		
		if (!m_InstTab.put(pInst->GetID(), pInst))
		{
			//	ID collsion ?
			a_LogOutput(1, "%s: ID %d already defined.", szFile, pInst->GetID());
			ASSERT(0);
			delete pInst;
		}
	}
	
	sf.Close();
	
	return true;
}

bool CECGame::ImportStringTable()
{
	bool bval;
	const char *szFile = "ElementHintTool.txt";
	AWScriptFile s;

	bval = s.Open(szFile);
	if( !bval )
	{
		a_LogOutput(1, "CECGame::ImportStringTable, failed to open file %s", szFile);
		return false;
	}

	while( !s.IsEnd() )
	{
		bval = s.GetNextToken(true);
		if( !bval ) break;		// End of file.
		int idString = a_atoi(s.m_szToken);

		bval = s.GetNextToken(true);
		if( !bval )
		{
			a_LogOutput(1, "CECGame::ImportStringTable, failed to read line %d from file %s", s.GetCurLine(), szFile);
			return false;
		}

		AUICTranslate tran;
		ACString str(tran.Translate(s.m_szToken));
		m_strTab[idString] = str;
	}

	s.Close();
	return true;
}

const wchar_t * CECGame::GetProfName(int iProf)
{
	const wchar_t *szRet = NULL;
	if (iProf >= 0 && iProf < NUM_PROFESSION)
	{
		static int s_ProfDesc[NUM_PROFESSION] = {			
			FIXMSG_PROF_WARRIOR,
			FIXMSG_PROF_MAGE,
			FIXMSG_PROF_MONK,
			FIXMSG_PROF_HAG,
			FIXMSG_PROF_ORC,
			FIXMSG_PROF_GHOST,
			FIXMSG_PROF_ARCHOR,
			FIXMSG_PROF_ANGEL,
			FIXMSG_PROF_JIANLING,
			FIXMSG_PROF_MEILING,
			FIXMSG_PROF_YEYING,
			FIXMSG_PROF_YUEXIAN,
		};
		szRet = GetFixedMsgTab()->GetWideString(s_ProfDesc[iProf]);
	}
	else
	{
		ASSERT(false);
	}
	return szRet;
}

ACString CECGame::GetRealmName(int realmLevel)
{
	ACString strRealm;
	if (realmLevel > 0){
		int layer = realmLevel ? (realmLevel + 9) / 10 : 0;
		int subLevel = realmLevel ? (realmLevel % 10 ? realmLevel % 10 : 10) : 0;
		strRealm.Format(GetStringFromTable(11100), GetStringFromTable(11100+layer), GetStringFromTable(11120+subLevel));
	}else{
		strRealm = GetStringFromTable(11099);
	}
	return strRealm;
}

const wchar_t * CECGame::GetStringFromTable(int idString)
{
	return m_strTab[idString];
}

ACString CECGame::GetFormattedPrice(__int64 i)
{
	ACString szRes;

	CECStringTab* pDescTab = GetItemDesc();
	ACString strSeperator = pDescTab->GetWideString(ITEMDESC_PRICE_SEPERATOR);
	if (strSeperator.IsEmpty())
		strSeperator = _AL(",");

	if( i < 1000 )
		szRes.Format(_AL("%d"), (int)i);
	else if( i < 1000000 )
		szRes.Format(_AL("%d%s%03d"), (int)i / 1000, strSeperator, (int)i % 1000);
	else if( i < 1000000000 )
		szRes.Format(_AL("%d%s%03d%s%03d"), (int)i / 1000000, strSeperator, (int)i / 1000 % 1000, strSeperator, (int)i % 1000);
	else
	{
		int nHigh = int(i / 1000000000);
		int nLow = int(i % 1000000000);

		szRes.Format(_AL("%d%s%03d%s%03d%s%03d"), nHigh, strSeperator, nLow / 1000000 % 1000, strSeperator, nLow / 1000 % 1000, strSeperator, nLow % 1000);
	}
	return szRes;
}

const PLAYER_SPIRIT_CONFIG* CECGame::GetPlayerSpiritConfig(int type)
{
	static const PLAYER_SPIRIT_CONFIG* pConfig[CARD_HOLDER::max_holder] = {0};
	const PLAYER_SPIRIT_CONFIG* pRet = NULL;
	if (type >= 0 && type < CARD_HOLDER::max_holder) {
		if (NULL == pConfig[type]) {
			elementdataman *pDataMan = g_pGame->GetElementDataMan();
			DATA_TYPE DataType;
			unsigned int tid = pDataMan->get_first_data_id(ID_SPACE_CONFIG,DataType);
			while(tid) {
				if(DataType == DT_PLAYER_SPIRIT_CONFIG) {
					const PLAYER_SPIRIT_CONFIG *pData = (const PLAYER_SPIRIT_CONFIG *)
						pDataMan->get_data_ptr(tid, ID_SPACE_CONFIG, DataType);
					
					if(pData->type == type) {
						pConfig[type] = pData;
						break;
					}
				}
				tid = pDataMan->get_next_data_id(ID_SPACE_CONFIG, DataType);
			}			
		}
		pRet = pConfig[type];
	}
	return pRet;
}

const CECGame::MeridianlevelParam& CECGame::GetLevelParam(int level)
{
	static MeridianlevelParam m_MeridianlevelParams[MERIDIAN_LEVEL_COUNT * MeridiansLevelLayer];
	static bool s_bInitialized(false);
	if (!s_bInitialized){
		s_bInitialized = true;
		const int levelParams[] = {25,1,5,
			20,1,10,
			15,1,15,
			10,1,20,
			9,1,25,
			9,1,30,
			8,1,35,
			8,1,40,
			7,1,45,
			7,1,50,
			6,1,56,
			6,1,61,
			5,1,67,
			5,1,72,
			5,1,78,
			4,1,83,
			4,1,89,
			4,1,94,
			4,1,100,
			10,2,110,
			8,1,115,
			8,1,120,
			7,1,125,
			7,1,130,
			6,1,136,
			6,1,141,
			5,1,147,
			5,1,152,
			4,1,158,
			4,1,163,
			3,1,169,
			3,1,175,
			2,1,182,
			2,1,188,
			2,1,195,
			1,1,205,
			1,1,215,
			1,1,225,
			1,1,235,
			7,2,254,
			5,1,259,
			4,1,265,
			3,1,271,
			2,1,277,
			1,1,287,
			1,1,297,
			1,1,307,
			10,2,318,
			10,2,329,
			10,2,339,
			10,2,350,
			9,2,363,
			9,2,376,
			9,2,389,
			9,2,402,
			8,2,417,
			8,2,432,
			8,2,447,
			8,2,462,
			12,3,494,
			1,1,504,
			1,1,514,
			1,1,524,
			10,2,534,
			10,2,545,
			9,2,558,
			9,2,571,
			8,2,586,
			8,2,601,
			7,2,620,
			7,2,639,
			6,2,664,
			6,2,689,
			5,2,722,
			5,2,755,
			5,2,788,
			4,2,828,
			4,2,868,
			10,3,912,
			8,3,1000,
		};
		for(int i = 0;i < ARRAY_SIZE(m_MeridianlevelParams);i++)
		{
			m_MeridianlevelParams[i].successGate              = levelParams[i * 3];
			m_MeridianlevelParams[i].continuousSuccessGateNum = levelParams[i * 3 + 1];
			m_MeridianlevelParams[i].bonusMultiplier          = levelParams[i * 3 + 2];
		}
	}
	if(level < 0 || level >= MERIDIAN_LEVEL_COUNT * MeridiansLevelLayer)
	{
		return m_MeridianlevelParams[0];
	}
	
	return m_MeridianlevelParams[level];
}

bool CECGame::GetLevelPropBonus(int profession,int level,int& hp,int& phyDefence,int& mgiDefence,int& phyAttack,int& mgiAttack)
{
	bool bRet(false);
	
	if(level != 0)
	{
		const MeridianlevelParam& curLevelParam = GetLevelParam(level-1);
		elementdataman * pDataMan = g_pGame->GetElementDataMan();
		DATA_TYPE dt = DT_INVALID;
		unsigned int id = pDataMan->get_first_data_id(ID_SPACE_CONFIG, dt);
		while (id > 0)
		{
			if (dt == DT_MERIDIAN_CONFIG) break;
			id = pDataMan->get_next_data_id(ID_SPACE_CONFIG, dt);
		}
		
		if (id > 0)
		{
			const MERIDIAN_CONFIG * pConfig = static_cast<const MERIDIAN_CONFIG *>(pDataMan->get_data_ptr(id, ID_SPACE_CONFIG, dt));
			if(pConfig)
			{
				hp         = pConfig->prof_para[profession].hp * curLevelParam.bonusMultiplier / 100;
				phyDefence = pConfig->prof_para[profession].phy_defence * curLevelParam.bonusMultiplier / 100;
				mgiDefence = pConfig->prof_para[profession].magic_defence * curLevelParam.bonusMultiplier / 100;
				phyAttack  = pConfig->prof_para[profession].phy_damage * curLevelParam.bonusMultiplier / 100;
				mgiAttack  = pConfig->prof_para[profession].magic_damage * curLevelParam.bonusMultiplier / 100;
				bRet = true;
			}
		}
	}
	else
	{
		hp = 0;
		phyDefence = 0;
		mgiDefence = 0;
		phyAttack  = 0;
		mgiAttack  = 0;
		bRet = true;
	}
	
	return bRet;
}