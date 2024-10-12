/*
 * FILE: GL_Game.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2003/12/16
 *
 * HISTORY: 
 *
 * Copyright (c) 2003 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include <hashtab.h>
#include <hashmap.h>
#include <AAssist.h>
#include "EC_StringTab.h"
#include <A3DMacros.h>

class elementdataman;
class CECInstance;
class itemdataman;
struct PLAYER_SPIRIT_CONFIG;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
//	
//	Class CECGame
//	
///////////////////////////////////////////////////////////////////////////

class CECGame
{
public: // Types
	typedef abase::hashtab<BYTE, int, abase::_hash_function> ItemExtPropTable;
	typedef abase::hashtab<int, int, abase::_hash_function> SuiteEquipTable;
	typedef abase::hashtab<BYTE, int, abase::_hash_function> ItemColTable;
	typedef abase::hashtab<CECInstance*, int, abase::_hash_function> InstTable;
	typedef abase::hashtab<int, int, abase::_hash_function> ItemTypeTable;

public:	//	Constructor and Destructor

	CECGame();
	virtual ~CECGame();
	
	bool Init();
	void Release();

	elementdataman * GetElementDataMan() { return m_pElementDataMan; }
	ItemExtPropTable& GetItemExtPropTable() { return m_ItemExtProps; }
	CECStringTab* GetItemDesc() { return &m_ItemDesc; }
	CECInstance* GetInstance(int id);
	CECStringTab* GetSkillDesc() { return &m_SkillDesc; }
	//	Get real tick time of current frame
	DWORD GetRealTickTime() { return 0; }
	SuiteEquipTable& GetSuiteEquipTable() { return m_SuiteEquipTab; }
	CECStringTab* GetFixedMsgTab() { return &m_FixedMsgs; }
	itemdataman * GetItemDataMan() { return m_pItemDataMan; }
	
	//	Get item's extend description string
	const wchar_t* GetItemExtDesc(int tid);
	CECStringTab* GetItemExtDesc() { return &m_ItemExtDesc; }
	int GetItemNameColorIdx(int tid, int iDefIndex=0);
	//	Get bad words table
	abase::vector<ACString>& GetBadWords() { return m_vecBadWords; }
	//	Filter bad words
	void FilterBadWords(ACString &str);

	static void AUI_ConvertChatString(const ACHAR *pszChat, ACHAR *pszConv, bool bName = true);
	static ACString AUI_ConvertHintString(const ACHAR *szText);
	
	const wchar_t * GetProfName(int iProf);	
	ACString GetRealmName(int realmLevel);

	const wchar_t * GetStringFromTable(int idString);

	ACString	GetFormattedPrice(__int64 i);
	
	static bool GetColor(unsigned short c, A3DCOLOR &clr)
	{
		//	查询当前骑宠是否染过色，如果染过色，返回被染之后的颜色
		bool bRet(false);
		if (c & (1<<(sizeof(c)*8-1)))
		{
			//	unsigned short 首位为1时，表示染过色，此时可获取颜色
			clr = A3DCOLORRGB(((c) & (0x1f << 10)) >> 7, ((c) & (0x1f << 5)) >> 2, ((c) & 0x1f) << 3);
			bRet = true;
		}
		return bRet;
	}
	
	static A3DCOLOR GetDefaultColor()
	{
		return A3DCOLORRGB(255, 255, 255);
	}
	
	struct CARD_HOLDER
	{
		enum
		{
			max_holder = 6,
			max_holder_level = 5,	// 目前版本只开放到5级
		};
	};
	static const PLAYER_SPIRIT_CONFIG * GetPlayerSpiritConfig(int type);
	enum
	{
		FRONT_MERIDIAN_HUIYIN ,		// 会阴穴
		FRONT_MERIDIAN_QUGU,		// 曲骨穴
		FRONT_MERIDIAN_SHIMEN,		// 石门穴
		FRONT_MERIDIAN_QIHAI,		// 气海穴
		FRONT_MERIDIAN_JUQUE,		 // 巨阙穴
		FRONT_MERIDIAN_SHANZHONG,	// 膻中穴
		FRONT_MERIDIAN_ZIGONG,		// 紫宫穴
		FRONT_MERIDIAN_XUANJI,		// 璇玑穴
		FRONT_MERIDIAN_TIANTU,		// 天突穴

		BACK_MERIDIAN_CHANGQIANG,   // 长强穴
		BACK_MERIDIAN_YAOYU,		// 腰俞穴
		BACK_MERIDIAN_YANGGUAN,		// 阳关穴
		BACK_MERIDIAN_MINGMEN,		// 命门穴
		BACK_MERIDIAN_ZHIYANG,		// 至阳穴
		BACK_MERIDIAN_LINGTAI,		// 灵台穴
		BACK_MERIDIAN_SHENDAO,		// 神道穴
		BACK_MERIDIAN_FENGFU,		// 风府穴
		BACK_MERIDIAN_BAIHUI,		// 百会穴
		BACK_MERIDIAN_SHENTING,		// 神庭穴

		MERIDIAN_LEVEL_THRESHOLD,   // 境界穴位
		MERIDIAN_LEVEL_COUNT,       // 穴位个数

		MeridiansLevelLayer = 4,	// 境界数量
	};
	
	struct MeridianlevelParam
	{
		int successGate;            // 生门数量
		int continuousSuccessGateNum; // 连续翻开生门的次数
		int bonusMultiplier;        // 奖励系数 需要除以100
	};
	static const MeridianlevelParam& GetLevelParam(int level);
	static bool GetLevelPropBonus(int profession,int level,int& hp,int& phyDefence,int& mgiDefence,int& phyAttack,int& mgiAttack);

protected:
	
	//	Load item extend properties
	bool LoadItemExtProps();
	
	//	Build suite equipment table
	void BuildSuiteEquipTab();

	//  Load bad words for filter some names
	bool LoadBadWords();
	
	//	Load instance information from file
	bool LoadInstanceInfo();

	//	Load string table with number
	bool ImportStringTable();

protected:	//	Attributes
	elementdataman *	m_pElementDataMan;	//	global element templates manager
	itemdataman *		m_pItemDataMan;		//	global templates manager
	ItemExtPropTable	m_ItemExtProps;		//	Item extend properties table
	CECStringTab		m_ItemDesc;			//	Item desciption string table
	CECStringTab		m_SkillDesc;		//	Skill description string table
	SuiteEquipTable		m_SuiteEquipTab;	//	Suite equipment table
	CECStringTab		m_FixedMsgs;		//	Fixed message table
	CECStringTab		m_ItemExtDesc;		//	Item extend description string table
	ItemColTable		m_ItemColTab;		//	Item color table	
	abase::vector<ACString> m_vecBadWords;
	InstTable			m_InstTab;			//	Instance table
	abase::hash_map<int, ACString> m_strTab;           //  additional string tab added by ElementHintTool only
};

extern CECGame *g_pGame;

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

