// Filename	: EC_ForceMgr.cpp
// Creator	: Feng Ning
// Date		: 2011/10/26

#include "EC_ForceMgr.h"
#include "EC_Game.h"
#include "EC_Global.h"
#include "elementdataman.h"
#include "A3DDevice.h"
#include "A2DSprite.h"
#include "A3DMacros.h"
#include "EC_IvtrItem.h"
#include "WikiEquipmentProp.h"
#include "EC_Skill.h"
#include <algorithm>

#define new A_DEBUG_NEW

class CECSkillTomeCache
{
public:
	CECSkillTomeCache();
	~CECSkillTomeCache();

	int FindValidSkill(int itemid, int prof);
private:
	typedef abase::hash_map<int, CECSkill*> SkillMap;
	SkillMap m_Skills;
};

CECForceMgr::CECForceMgr()
{
	InitForceResource();
	InitForceShopData();

	m_pSkillCache = NULL;
}

CECForceMgr::~CECForceMgr()
{
	ClearForceResource();

	delete m_pSkillCache;
	m_pSkillCache = NULL;
}

const FORCE_CONFIG* CECForceMgr::GetForceData(int forceID)
{
	elementdataman *pDataMan = g_pGame->GetElementDataMan();

	DATA_TYPE dt = DT_INVALID;
	FORCE_CONFIG *pConfig = (FORCE_CONFIG *)pDataMan->get_data_ptr(forceID, ID_SPACE_CONFIG, dt);
	if(dt != DT_FORCE_CONFIG)
	{
		// invalid force data
		return NULL;
	}

	return pConfig;
}


const CECForceMgr::FORCE_GLOBAL* CECForceMgr::GetForceGlobal(int forceID)
{
	ViewMap::iterator itr = m_Views.find(forceID);
	if(itr == m_Views.end())
	{
		return NULL;
	}

	const FORCE_RESOURCE& view = itr->second;
	return view.ready ? &view.detail : NULL;
}

void CECForceMgr::SetForceGlobal(int forceID, const CECForceMgr::FORCE_GLOBAL* pDetail)
{
	ViewMap::iterator itr = m_Views.find(forceID);
	if(itr == m_Views.end())
	{
		return;
	}

	FORCE_RESOURCE& view = itr->second;
	if(pDetail)
	{
		view.ready = g_pGame->GetServerAbsTime();
		view.detail = *pDetail;
	}
	else
	{
		view.ready = 0;
		memset(&view.detail, 0, sizeof(view.detail));
	}
}

int CECForceMgr::GetValidForce(int* pForceID, size_t size)
{
	size_t count = 0;

	// get all valid force_id
	ViewMap::iterator itr = m_Views.begin();
	for(;itr != m_Views.end();++itr)
	{
		FORCE_RESOURCE& view = itr->second;
		if(view.ready)
		{
			++count;
			if(pForceID && count <= size)
			{
				*pForceID = view.force_id;
				pForceID++;
			}
		}
	}

	return count;
}



A2DSprite* CECForceMgr::GetForceIcon(A3DDevice * pDevice, const FORCE_CONFIG* pConfig, CECForceMgr::FORCE_ICON type)
{
	ViewMap::iterator itr = m_Views.find(pConfig->id);
	if(itr == m_Views.end())
	{
		// invalid config data, maybe this template data is not used
		return NULL;
	}

	FORCE_RESOURCE& view = itr->second;
	A2DSprite** ppSprite = NULL;

	// show the force icon
	AString strOrigin = pConfig->file_icon;
	if(strOrigin.IsEmpty())
	{
		return NULL;
	}

	AString strTarget = strOrigin;

	if(type == FORCE_ICON_FLAG)
	{
		// find the proper icon
		if(view.iconBig)
			return view.iconBig;

		int dotPos = strOrigin.ReverseFind('.');
		if(dotPos > 0)
		{
			// the big icon file is made up from original file
			strTarget = strOrigin.Left(dotPos) + "_flag" + strOrigin.Mid(dotPos);
		}
		ppSprite = &view.iconBig;
	}
	else if(type == FORCE_ICON_MINI)
	{
		// find the proper icon
		if(view.iconMini)
			return view.iconMini;

		int dotPos = strOrigin.ReverseFind('.');
		if(dotPos > 0)
		{
			// the mini icon file is made up from original file
			strTarget = strOrigin.Left(dotPos) + "_mini" + strOrigin.Mid(dotPos);
		}
		ppSprite = &view.iconMini;
	}
	else
	{
		ASSERT(type == FORCE_ICON_NORMAL);
		// find the proper icon
		if(view.iconNormal)
			return view.iconNormal;

		ppSprite = &view.iconNormal;
	}

	*ppSprite = new A2DSprite;
	bool bval = (*ppSprite)->Init(pDevice, strTarget, 0);
	if( !bval )
	{
		A3DRELEASE(*ppSprite);
		a_LogOutput(1, "CECForceMgr::GetForceIcon, Failed to Init %s.", strTarget);
	}

	if(*ppSprite)
	{
		(*ppSprite)->SetLinearFilter(true);
	}
	return *ppSprite;
}

void CECForceMgr::ClearForceResource()
{
	// clear view and release the a2dsprite
	ViewMap::iterator itr = m_Views.begin();
	for(;itr != m_Views.end();++itr)
	{
		FORCE_RESOURCE& view = itr->second;
		view.Release();
	}
	m_Views.clear();
}

void CECForceMgr::FORCE_RESOURCE::Release()
{
	A3DRELEASE(iconMini);
	A3DRELEASE(iconNormal);
	A3DRELEASE(iconBig);
}


void CECForceMgr::InitForceResource()
{
	int index = 0;

	// bind the AUI controls to specific force
	DATA_TYPE DataType;
	elementdataman *pDataMan = g_pGame->GetElementDataMan();
	unsigned int id = pDataMan->get_first_data_id(ID_SPACE_CONFIG, DataType);
	while( id )
	{
		if( DataType == DT_FORCE_CONFIG )
		{
			const FORCE_CONFIG *pConfig = GetForceData(id);
			if(!pConfig)
			{
				continue;
			}

			FORCE_RESOURCE view;
			view.force_id = id;
			m_Views[id] = view;
			index++;
		}

		id = pDataMan->get_next_data_id(ID_SPACE_CONFIG, DataType);
	}
}

FORCE_GLOBAL_SUMMARY::~FORCE_GLOBAL_SUMMARY()
{
	// clear the ratio infos
	count = 0;

	delete[] ppConfig;
	ppConfig = NULL;

	delete[] pRatio;
	pRatio = NULL;
}

FORCE_GLOBAL_SUMMARY* CECForceMgr::GetForceGlobalSummary(int type)
{
	typedef const FORCE_CONFIG* FORCE_CONFIG_PTR;

	if(type >= FORCE_GLOBAL::GetMaxType())
		return NULL;

	FORCE_GLOBAL_SUMMARY* pSummary = NULL;

	int count = GetValidForce(NULL, 0);
	if(count)
	{
		pSummary = new FORCE_GLOBAL_SUMMARY();
		memset(pSummary, 0, sizeof(FORCE_GLOBAL_SUMMARY));

		// create temp buffer
		int* pRatio = (int*)a_malloctemp(sizeof(int)*count);
		if(!pRatio)
		{
			delete pSummary;
			return NULL;
		}

		FORCE_CONFIG_PTR* pConfigs = (FORCE_CONFIG_PTR*)a_malloctemp(sizeof(FORCE_CONFIG_PTR)*count);
		if(!pConfigs)
		{
			delete pSummary;
			a_freetemp(pRatio);
			return NULL;
		}

		int* pBuffer = (int*)a_malloctemp(sizeof(int)*count);
		if(!pBuffer)
		{
			delete pSummary;
			a_freetemp(pRatio);
			a_freetemp(pConfigs);
			return NULL;
		}

		// get data to temp buffer
		GetValidForce(pBuffer, count);
		for(int i=0;i<count;i++)
		{
			int forceID = pBuffer[i];

			FORCE_CONFIG_PTR pConfig = GetForceData(forceID);
			if(!pConfig)
			{
				// avoid to get invalid data
				ASSERT(false);
				continue;
			}

			const CECForceMgr::FORCE_GLOBAL* pDetail = GetForceGlobal(forceID);
			if(!pDetail)
			{
				// avoid to get invalid data
				ASSERT(false);
				continue;
			}

			pConfigs[pSummary->count] = pConfig;
			pRatio[pSummary->count] = pDetail->GetValue(type);

			pSummary->count++;
		}
		a_freetemp(pBuffer);

		// copy data from temp buffer
		ASSERT(pSummary->count <= count);
		if(pSummary->count)
		{
			pSummary->pRatio = new int[pSummary->count];
			memcpy(pSummary->pRatio, pRatio, sizeof(int)*(pSummary->count));
			pSummary->ppConfig = new FORCE_CONFIG_PTR[pSummary->count];
			memcpy(pSummary->ppConfig, pConfigs, sizeof(FORCE_CONFIG_PTR)*(pSummary->count));
		}
		a_freetemp(pRatio);
		a_freetemp(pConfigs);
	}

	return pSummary;
}


bool CECForceMgr::IsForceGlobalDataEmpty()
{
	bool isEmpty = true;

	ViewMap::iterator itr = m_Views.begin();
	for(;itr != m_Views.end();++itr)
	{
		const FORCE_RESOURCE& view = itr->second;
		// found a valid data
		if(view.ready)
		{
			isEmpty = false;
			break;
		}
	}

	return isEmpty;
}

struct ForceRequireLess
{
	ForceRequireLess(int forceid)
		:_forceid(forceid){}

	bool operator()(const NPC_SELL_SERVICE* lhs, const NPC_SELL_SERVICE* rhs)
	{
		const int pageSize = sizeof(lhs->pages) / sizeof(lhs->pages[0]);

		int requires[2] = {0};
		const NPC_SELL_SERVICE* pNPCs[2] = {lhs, rhs};

		// search the lowest requirement in service
		for(int i=0;i<2;i++)
		{
			const NPC_SELL_SERVICE* pNPC = pNPCs[i];
			for(int p=0;p<pageSize;p++)
			{
				if(pNPC->pages[p].require_force == _forceid)
				{
					requires[i] = pNPC->pages[p].require_force_reputation;

					// ==NOTICE==
					// here we assume the force reputation requirement is monotone increasing
					//
					break;
				}
			}
		}
		
		return requires[0] < requires[1];
	}
private:
	int _forceid;
};

// initialize the force shop data
void CECForceMgr::InitForceShopData()
{
	m_Shops.clear();

	// store all NPC seller related to force reputation
	DATA_TYPE DataType;
	elementdataman *pDataMan = g_pGame->GetElementDataMan();
	unsigned int id = pDataMan->get_first_data_id(ID_SPACE_ESSENCE, DataType);
	while( id )
	{
		if( DataType == DT_NPC_SELL_SERVICE )
		{
			NPC_SELL_SERVICE *pNPC = (NPC_SELL_SERVICE *)pDataMan->get_data_ptr(id, ID_SPACE_ESSENCE, DataType);
			if(!pNPC) continue;

			// avoid duplicated
			abase::hash_map<int, bool> duplicated;

			// search in every page if this NPC is related to a specific force
			const int pageSize = sizeof(pNPC->pages) / sizeof(pNPC->pages[0]);
			for(int p=0;p<pageSize;p++)
			{
				int forceid = pNPC->pages[p].require_force;
				
				if(forceid > 0 && duplicated.find(forceid) == duplicated.end())
				{
					FORCE_SHOP* pShop = NULL;

					ShopMap::iterator itr = m_Shops.find(forceid);
					if(itr != m_Shops.end())
					{
						pShop = &(itr->second);
					}
					else
					{
						pShop = &(m_Shops[forceid]);
					}

					pShop->npcs.push_back(pNPC);
					duplicated[forceid] = true;
				}
			}
		}

		id = pDataMan->get_next_data_id(ID_SPACE_ESSENCE, DataType);
	}

	ShopMap::iterator itr = m_Shops.begin();
	for(;itr != m_Shops.end();++itr)
	{
		FORCE_SHOP& shop = itr->second;
		std::sort(shop.npcs.begin(), shop.npcs.end(), ForceRequireLess(itr->first));
	}
}

// check whether force reputation passed a limitation just in time
int CECForceMgr::CheckShopLimit(int forceid, int repBefore, int repCurrent)
{
	// the closest limitation found
	int closet = 0;

	if(repBefore >= repCurrent)
	{
		return closet;
	}
		
	ShopMap::iterator itr = m_Shops.find(forceid);
	if(itr == m_Shops.end())
	{
		return closet;
	}
	
	const FORCE_SHOP::NPCS& npcs = itr->second.npcs;
	for(size_t i=0;i<npcs.size();i++)
	{
		const NPC_SELL_SERVICE* pNPC = npcs[i];
		const int pageSize = sizeof(pNPC->pages) / sizeof(pNPC->pages[0]);

		// check every page and return the first limitation
		for(int p = 0;p<pageSize;p++)
		{
			// skip the page without requirement
			int require = pNPC->pages[p].require_force_reputation;
			if (!require || pNPC->pages[p].page_title[0] == 0)
				continue;

			if( closet < require &&
				pNPC->pages[p].require_force == forceid &&
				repBefore < require && repCurrent >= require )
			{
				closet = require;
			}
		}
	}
	return closet;
}

// get suggest items from database
int CECForceMgr::GetSuggestItems(int forceid, int prof, int rep, CECIvtrItem** pBuf, int size)
{
	memset(pBuf, 0, sizeof(CECIvtrItem*)*size);

	ShopMap::iterator itr = m_Shops.find(forceid);
	if(size <= 0 || itr == m_Shops.end())
	{
		return 0;
	}

	abase::vector<CECIvtrItem*> found;

	// the lower bound for each NPC
	int foundRequire = 99999999;

	// ==NOTICE==
	// here we assume the force reputation requirement is monotone increasing
	// 
	const FORCE_SHOP::NPCS& npcs = itr->second.npcs;
	for(size_t i=0;i<npcs.size();i++)
	{
		const NPC_SELL_SERVICE* pNPC = npcs[i];
		const int pageSize = sizeof(pNPC->pages) / sizeof(pNPC->pages[0]);

		int foundPage = -1;
		int p;

		// check for every page and return lower bound
		for(p=pageSize; p>=0 ;p--)
		{
			if( pNPC->pages[p].page_title[0] == 0 ||
				pNPC->pages[p].require_force != forceid )
				continue;
			
			// search the lower bound in this NPC
			int require = pNPC->pages[p].require_force_reputation;
			if( require >= rep )
			{
				if(require < foundRequire)
				{
					foundPage = p;
					foundRequire = require;
				}
			}
			else
			{
				if(foundPage < 0) foundPage = p;
				break;
			}
		}

		// put all valid item here
		for(p=0; p<=foundPage ;p++)
		{
			if( pNPC->pages[p].page_title[0] == 0 ||
				pNPC->pages[p].require_force != forceid )
				continue;

			const int goodsSize = 
				sizeof(pNPC->pages[p].goods) / sizeof(pNPC->pages[p].goods[0]);

			for(int i=0; i<goodsSize; i++)
			{
				int itemID = pNPC->pages[p].goods[i].id;
				if(!itemID) continue;

				CECIvtrItem* pItem = GetSuggestItem(itemID, prof);
				if(pItem) found.push_back(pItem);
			}
		}
	}

	int realSize = 0;
	if(!found.empty())
	{
		CECIvtrItem** pTail = found.begin() + found.size();
		while( realSize < size && pTail != found.begin() )
		{ 
			pBuf[realSize++] = *(--pTail);
		}
	}
	
	return realSize;
}
CECIvtrItem* CECForceMgr::GetSuggestItem(int itemID, int prof)
{
	bool fitPlayer = true;

	CECIvtrItem* pItem = CECIvtrItem::CreateItem(itemID, 0, 1);
	if(pItem->IsEquipment())
	{
		// check profession
		DWORD profMask = (1 << prof);
		WikiEquipmentProp* pEP = WikiEquipmentProp::CreateProperty(itemID);
		fitPlayer = !pEP ? false : (pEP->GetClass() & profMask) > 0;
		delete pEP;
	}
	else if(pItem->GetClassID() == CECIvtrItem::ICID_SKILLTOME)
	{
		// lazy initialization for cache
		if(!m_pSkillCache)
			m_pSkillCache = new CECSkillTomeCache();

		// check skill tome
		int valid = m_pSkillCache->FindValidSkill(itemID, prof);
		fitPlayer = (valid != 0);
	}

	if(!fitPlayer)
	{
		delete pItem;
		pItem = NULL;
	}

	return pItem;
}
// =======================================================================
// CECSkillTomeCache
// =======================================================================
CECSkillTomeCache::CECSkillTomeCache()
{
	// search all skill which require a item
	int next = 1;
	do 
	{
		CECSkill* pSkill = new CECSkill(next, 1);
		int itemID = pSkill->GetRequiredBook();
		if(itemID > 0 && m_Skills.find(itemID) == m_Skills.end() )
		{
			m_Skills[itemID] = pSkill;
		}
		else
		{
			delete pSkill;
		}

		next = GNET::ElementSkill::NextSkill(next);
	} while (next);
}

CECSkillTomeCache::~CECSkillTomeCache()
{
	SkillMap::iterator itr = m_Skills.begin();
	for(;itr != m_Skills.end(); ++itr)
	{
		delete itr->second;
		itr->second = NULL;
	}
	m_Skills.clear();
}

int CECSkillTomeCache::FindValidSkill(int itemid, int prof)
{
	int skill = 0;
	SkillMap::iterator itr = m_Skills.find(itemid);
	if(itr != m_Skills.end())
	{
		CECSkill* pSkill = itr->second;
		if(prof < 0 || pSkill->GetCls() == prof)
		{
			skill = pSkill->GetSkillID();
		}
	}

	return skill;
}