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
		//	��ѯ��ǰ����Ƿ�Ⱦ��ɫ�����Ⱦ��ɫ�����ر�Ⱦ֮�����ɫ
		bool bRet(false);
		if (c & (1<<(sizeof(c)*8-1)))
		{
			//	unsigned short ��λΪ1ʱ����ʾȾ��ɫ����ʱ�ɻ�ȡ��ɫ
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
			max_holder_level = 5,	// Ŀǰ�汾ֻ���ŵ�5��
		};
	};
	static const PLAYER_SPIRIT_CONFIG * GetPlayerSpiritConfig(int type);
	enum
	{
		FRONT_MERIDIAN_HUIYIN ,		// ����Ѩ
		FRONT_MERIDIAN_QUGU,		// ����Ѩ
		FRONT_MERIDIAN_SHIMEN,		// ʯ��Ѩ
		FRONT_MERIDIAN_QIHAI,		// ����Ѩ
		FRONT_MERIDIAN_JUQUE,		 // ����Ѩ
		FRONT_MERIDIAN_SHANZHONG,	// ����Ѩ
		FRONT_MERIDIAN_ZIGONG,		// �Ϲ�Ѩ
		FRONT_MERIDIAN_XUANJI,		// ���Ѩ
		FRONT_MERIDIAN_TIANTU,		// ��ͻѨ

		BACK_MERIDIAN_CHANGQIANG,   // ��ǿѨ
		BACK_MERIDIAN_YAOYU,		// ����Ѩ
		BACK_MERIDIAN_YANGGUAN,		// ����Ѩ
		BACK_MERIDIAN_MINGMEN,		// ����Ѩ
		BACK_MERIDIAN_ZHIYANG,		// ����Ѩ
		BACK_MERIDIAN_LINGTAI,		// ��̨Ѩ
		BACK_MERIDIAN_SHENDAO,		// ���Ѩ
		BACK_MERIDIAN_FENGFU,		// �縮Ѩ
		BACK_MERIDIAN_BAIHUI,		// �ٻ�Ѩ
		BACK_MERIDIAN_SHENTING,		// ��ͥѨ

		MERIDIAN_LEVEL_THRESHOLD,   // ����Ѩλ
		MERIDIAN_LEVEL_COUNT,       // Ѩλ����

		MeridiansLevelLayer = 4,	// ��������
	};
	
	struct MeridianlevelParam
	{
		int successGate;            // ��������
		int continuousSuccessGateNum; // �����������ŵĴ���
		int bonusMultiplier;        // ����ϵ�� ��Ҫ����100
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

