// Filename	: EC_ForceMgr.h
// Creator	: Feng Ning
// Date		: 2011/10/26

#pragma once

#include "AString.h"
#include "hashmap.h"

//------------------------------------------------------------------------
// Class Declaration
//------------------------------------------------------------------------
class A3DDevice;
struct FORCE_CONFIG;
class A2DSprite;
class CECPlayer;
struct NPC_SELL_SERVICE;
class CECIvtrItem;
class CECSkillTomeCache;

// store last updated result
struct FORCE_GLOBAL_SUMMARY
{
	int count;
	const FORCE_CONFIG** ppConfig;
	int* pRatio;

	~FORCE_GLOBAL_SUMMARY();
};

//------------------------------------------------------------------------
// Manager for all forces
//------------------------------------------------------------------------
class CECForceMgr
{
public:
	CECForceMgr();
	~CECForceMgr();

public:
	// helper function for getting the template
	static const FORCE_CONFIG* GetForceData(int forceID);

	// get all valid force id, return the actual size
	int GetValidForce(int* pForceID, size_t size);

	enum FORCE_ICON
	{
		FORCE_ICON_NORMAL,
		FORCE_ICON_MINI,
		FORCE_ICON_FLAG,

		FORCE_ICON_MAX,
	};
	// get or generated the icon, all files were stored in this dialog
	A2DSprite* GetForceIcon(A3DDevice * pDevice, const FORCE_CONFIG* pConfig, FORCE_ICON type);

	// detailed info from server
	struct FORCE_GLOBAL
	{
		int player_count;
		int development;
		int construction;
		int activity;
		int activity_level;

		// *NOTICE* we assume all members are integer
		int GetValue(int type) const { return type >= GetMaxType() ? 0 : *(((const int*)this) + type); }

		static int GetMaxType() { return sizeof(FORCE_GLOBAL)/sizeof(int); }
	};
	
	// get the detailed information for each force
	// return NULL if the force data is not ready or not existed
	const FORCE_GLOBAL* GetForceGlobal(int forceID);

	// update the detailed information from server, and mark data ready
	void SetForceGlobal(int forceID, const FORCE_GLOBAL* pDetail);

	// create a summary from global data used to draw the diagram
	// the type is the data index in FORCE_GLOBAL
	// *NOTICE*  we assume all member in FORCE_GLOBAL is integer
	FORCE_GLOBAL_SUMMARY* GetForceGlobalSummary(int type);

	// check whether server had sent valid global data
	bool IsForceGlobalDataEmpty();

	// check whether force reputation passed a limitation just in time
	int CheckShopLimit(int forceid, int repBefore, int repCurrent);

	// get suggest items from database
	int GetSuggestItems(int forceid, int prof, int rep, CECIvtrItem** pBuf, int size);

protected:
	// clear the data stored in view info
	void ClearForceResource();

	// init view info from template, all data were not ready after init
	void InitForceResource();

	struct FORCE_RESOURCE
	{
		int force_id;
		A2DSprite* iconMini;
		A2DSprite* iconNormal;
		A2DSprite* iconBig;

		int ready; // last updated time
		FORCE_GLOBAL detail;

		// assume this structure is a POD
		FORCE_RESOURCE() { memset(this, 0, sizeof(FORCE_RESOURCE)); }
		void Release();
	};
	typedef abase::hash_map<int,FORCE_RESOURCE> ViewMap;
	ViewMap m_Views;

	struct FORCE_SHOP
	{
		typedef abase::vector<NPC_SELL_SERVICE*> NPCS;

		int force;
		NPCS npcs;
	};
	typedef abase::hash_map<int, FORCE_SHOP> ShopMap;
	ShopMap m_Shops;

	CECSkillTomeCache* m_pSkillCache;

	// initialize the force shop data
	void InitForceShopData();
	CECIvtrItem* GetSuggestItem(int itemID, int prof);
};