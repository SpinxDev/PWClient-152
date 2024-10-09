// Filename	: DlgWikiRecipe.cpp
// Creator	: Feng Ning
// Date		: 2010/07/19

#include "DlgWikiRecipe.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_UIManager.h"
#include "EC_GameUIMan.h"
#include "EC_IvtrItem.h"
#include "AFI.h"
#include "AUIImagePicture.h" 
#include "AUICTranslate.h"
#include "WikiSearchCommand.h"
#include "WikiSearchNPC.h"
#include "WikiItemProp.h"
#include "elementdataman.h"
#include "DlgWikiRecipeDetail.h"
#include "DlgWikiMonster.h"
#include "DlgWikiEquipment.h"
#include "DlgWikiItem.h"
#include "EC_NPC.h"
#include "EC_World.h"
#include "EC_ManNPC.h"
#include "DlgWikiTask.h"
#include "DlgAutoHelp.h"
#include "TaskTempl.h"
#include "TaskTemplMan.h"
#include "WikiSearchTask.h"
#include "DlgWikiSkill.h"
// =======================================================================
#include <algorithm>
#include <iostream>
#include <fstream>

class ElementNameCache
{
public:
	typedef std::pair<ACHAR, unsigned int> CachePair;
	
	enum { CACHE_SIZE = 1 << (sizeof(ACHAR) * 8) };
	CachePair m_Cache[CACHE_SIZE];
	
	// TEMPLATE STRUCT greater
	template<class _Ty>
		struct greater {
		bool operator()(const _Ty& _X, const _Ty& _Y) const
		{return (_X.second > _Y.second); }
	};
	
	ElementNameCache();
};
// =======================================================================

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgWikiRecipe, CDlgWikiByNameBase)
AUI_ON_COMMAND("search",		OnCommand_SearchByName)
AUI_ON_COMMAND("material_prev",	OnCommand_MaterialPrev)
AUI_ON_COMMAND("material_next",	OnCommand_MaterialNext)
AUI_ON_COMMAND("product_prev",	OnCommand_ProductPrev)
AUI_ON_COMMAND("product_next",	OnCommand_ProductNext)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgWikiRecipe, CDlgWikiByNameBase)
AUI_ON_EVENT("List_Material",	WM_LBUTTONDBLCLK,	OnEventLButtonDBCLK_ListMaterial)
AUI_ON_EVENT("List_Product",	WM_LBUTTONDBLCLK,	OnEventLButtonDBCLK_ListProduct)
AUI_END_EVENT_MAP()

// =======================================================================
// Link Command
// =======================================================================
CDlgWikiRecipe::ShowSpecficLinkCommand::ShowSpecficLinkCommand(unsigned int item_id, const ACString& name)
:m_ItemID(item_id)
,m_Name(name)
{
}

CDlgNameLink::LinkCommand* CDlgWikiRecipe::ShowSpecficLinkCommand::Clone() const
{
	return new ShowSpecficLinkCommand(m_ItemID, m_Name);
}

bool CDlgWikiRecipe::ShowSpecficLinkCommand::operator()(P_AUITEXTAREA_NAME_LINK pLink)
{
	// goto search item
	CECGameUIMan* pGameUI = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan();
	CDlgWikiRecipe* pDlg = dynamic_cast<CDlgWikiRecipe*>(pGameUI->GetDialog("Win_WikiRecipeSearch"));
	ASSERT(pDlg); // should always get this dialog

	if(pDlg)
	{
		bool has = pDlg->ConfirmSearch(m_ItemID, m_Name);
		if (has)
		{
			if(!pDlg->IsShow()) pDlg->Show(true);
			pGameUI->BringWindowToTop(pDlg);

			return true;
		}		
	}
	
	return false;
}

ACString CDlgWikiRecipe::ShowSpecficLinkCommand::GetLinkText() const
{
	ACString strLinkedName;
	
	strLinkedName.Format(_AL("^00FF00&%s&^FFFFFF"), m_Name);

	return strLinkedName;
}

// =======================================================================
// MaterialSearcher
// =======================================================================
CDlgWikiRecipe::MaterialSearcher::MaterialSearcher(CDlgWikiRecipe* pDlg)
:m_pDlg(pDlg)
{
	m_pList = dynamic_cast<PAUILISTBOX>(pDlg->GetDlgItem("List_Material"));
	ASSERT(m_pList);

	m_pBtn_Prev = dynamic_cast<PAUISTILLIMAGEBUTTON>(pDlg->GetDlgItem("Btn_Material_Prev"));
	m_pBtn_Next = dynamic_cast<PAUISTILLIMAGEBUTTON>(pDlg->GetDlgItem("Btn_Material_Next"));

	m_pBtn_Prev->Enable(false);
	m_pBtn_Next->Enable(false);

	SetContentProvider(&WikiElementDataProvider(ID_SPACE_RECIPE));
}

bool CDlgWikiRecipe::MaterialSearcher::ConfirmSearch(unsigned int id)
{
	return Refresh(&WikiSearchRecipeByProductID(id));
}

void CDlgWikiRecipe::MaterialSearcher::OnBeginSearch()
{
 	m_pList->ResetContent();
}

bool CDlgWikiRecipe::MaterialSearcher::OnAddSearch(WikiEntityPtr p)
{
	WikiElementDataProvider::Entity* pEE = dynamic_cast<WikiElementDataProvider::Entity*>(p.Get());
	ASSERT(pEE); // should always got the ptr
	if(!pEE) return false;

	unsigned int id = pEE->GetID();

	ASSERT(m_pList->GetCount() < GetPageSize());

	// get data from entity first
	const RECIPE_ESSENCE* pEssence = NULL;
	if(!pEE->GetConstData(pEssence))
	{
		pEssence = m_pDlg->GetEssence(id);
	}
	ASSERT(pEssence);
	if (!pEssence) return false;

	// no NPC binded
	if(!m_pDlg->m_pWikiNPCMakingCache->IsValid(pEssence->id, NULL))
	{
		return false;
	}

	ACString strItem;
	ACString strHint = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan()->GetStringFromTable(8665);

	// set the core equip name
	if(pEssence->id_upgrade_equip > 0)
	{
		ACString strName;
		m_pDlg->GetItemName(pEssence->id_upgrade_equip, strName);
		if(!strName.IsEmpty())
		{
			if(!strItem.IsEmpty()) strItem += _AL(",");
			strItem += strName;
			
			strHint += _AL("\\r");
			strHint += strName;
		}
	}

	// set the material name
	size_t maxItem = sizeof(pEssence->materials) / sizeof(pEssence->materials[0]);
	for(size_t i=0;i<maxItem;i++)
	{
		if(pEssence->materials[i].num <= 0)
		{
			continue;
		}

		ACString strName;
		m_pDlg->GetItemName(pEssence->materials[i].id, strName);
		if(!strName.IsEmpty())
		{
			if(!strItem.IsEmpty()) strItem += _AL(",");
			strItem += strName;

			strHint += _AL("\\r");
			strHint += strName;
		}
	}

	// empty recipe
	if(strItem.IsEmpty())
	{
		return false;
	}

	m_pList->AddString(strItem);
	m_pList->SetItemData(m_pList->GetCount()-1, id);
	AUICTranslate trans;
	m_pList->SetItemHint(m_pList->GetCount()-1, trans.Translate(strHint));
	
	return true;
}

void CDlgWikiRecipe::MaterialSearcher::OnEndSearch()
{
	WikiSearcher::OnEndSearch();

	m_pBtn_Next->Enable(HaveNextPage());
	m_pBtn_Prev->Enable(HavePrevPage());

	if (m_pList->GetCount() <= 0)
	{
		return;
	}

	m_pList->SetCurSel(0);
}

const RECIPE_ESSENCE * CDlgWikiRecipe::MaterialSearcher::GetCurEssence() const
{
	const RECIPE_ESSENCE *pEssence = NULL;
	
	if (m_pList->GetCount() > 0)
	{
		int nCurSel = m_pList->GetCurSel();
		if (nCurSel >= 0 && nCurSel < m_pList->GetCount())
		{
			unsigned int id = m_pList->GetItemData(nCurSel);
			pEssence = m_pDlg->GetEssence(id);
		}
	}
	return pEssence;
}

// =======================================================================
// ProductSearcher
// =======================================================================
CDlgWikiRecipe::ProductSearcher::ProductSearcher(CDlgWikiRecipe* pDlg)
:m_pDlg(pDlg)
{
	m_pList = dynamic_cast<PAUILISTBOX>(pDlg->GetDlgItem("List_Product"));
	ASSERT(m_pList);

	m_pBtn_Prev = dynamic_cast<PAUISTILLIMAGEBUTTON>(pDlg->GetDlgItem("Btn_Product_Prev"));
	m_pBtn_Next = dynamic_cast<PAUISTILLIMAGEBUTTON>(pDlg->GetDlgItem("Btn_Product_Next"));

	m_pBtn_Prev->Enable(false);
	m_pBtn_Next->Enable(false);

	SetContentProvider(&WikiElementDataProvider(ID_SPACE_RECIPE));
}

bool CDlgWikiRecipe::ProductSearcher::ConfirmSearch(unsigned int id)
{
	return Refresh(&WikiSearchRecipeByMaterialID(id));
}

void CDlgWikiRecipe::ProductSearcher::OnBeginSearch()
{
	m_pList->ResetContent();
}

bool CDlgWikiRecipe::ProductSearcher::OnAddSearch(WikiEntityPtr p)
{
	WikiElementDataProvider::Entity* pEE = dynamic_cast<WikiElementDataProvider::Entity*>(p.Get());
	ASSERT(pEE); // should always got the ptr
	if(!pEE) return false;
	
	unsigned int id = pEE->GetID();
	
	ASSERT(m_pList->GetCount() < GetPageSize());
	
	// get data from entity first
	const RECIPE_ESSENCE* pEssence = NULL;
	if(!pEE->GetConstData(pEssence))
	{
		pEssence = m_pDlg->GetEssence(id);
	}
	ASSERT(pEssence);
	if (!pEssence) return false;

	// no NPC binded
	if(!m_pDlg->m_pWikiNPCMakingCache->IsValid(pEssence->id, NULL))
	{
		return false;
	}
	
	ACString strItem;
	ACString strHint = g_pGame->GetGameRun()->GetUIManager()->GetInGameUIMan()->GetStringFromTable(8665);
	size_t maxItem = sizeof(pEssence->targets) / sizeof(pEssence->targets[0]);
	for(size_t i=0;i<maxItem;i++)
	{
		if(pEssence->targets[i].id_to_make != 0 && pEssence->targets[i].probability > 0.f)
		{
			ACString strName;
			m_pDlg->GetItemName(pEssence->targets[i].id_to_make,strName);
			if(!strName.IsEmpty())
			{
				if(!strItem.IsEmpty()) strItem += _AL(",");
				strItem += strName;
				
				strHint += _AL("\\r");
				strHint += strName;
			}
		}
	}

	// empty recipe
	if(strItem.IsEmpty())
	{
		return false;
	}

	m_pList->AddString(strItem);
	m_pList->SetItemData(m_pList->GetCount()-1, id);
	AUICTranslate trans;
	m_pList->SetItemHint(m_pList->GetCount()-1, trans.Translate(strHint));
		
	return true;
}

void CDlgWikiRecipe::ProductSearcher::OnEndSearch()
{
	WikiSearcher::OnEndSearch();

	m_pBtn_Next->Enable(HaveNextPage());
	m_pBtn_Prev->Enable(HavePrevPage());
	
	if (m_pList->GetCount() <= 0)
	{
		return;
	}
	
	m_pList->SetCurSel(0);
}

const RECIPE_ESSENCE * CDlgWikiRecipe::ProductSearcher::GetCurEssence() const
{
	const RECIPE_ESSENCE *pEssence = NULL;
	
	if (m_pList->GetCount() > 0)
	{
		int nCurSel = m_pList->GetCurSel();
		if (nCurSel >= 0 && nCurSel < m_pList->GetCount())
		{
			unsigned int id = m_pList->GetItemData(nCurSel);
			pEssence = m_pDlg->GetEssence(id);
		}
	}
	return pEssence;
}

// =======================================================================
CDlgWikiRecipe::CDlgWikiRecipe()
:m_pDlgConfirm(NULL)
,m_pDlgNpc(NULL)
,m_pMaterialSearcher(NULL)
,m_pProductSearcher(NULL)
,m_pWikiNPCMakingCache(NULL)
,m_pWikiRecipeItemCache(NULL)
,m_pDlgDetail(NULL)
{
}

bool CDlgWikiRecipe::OnInitDialog()
{
	if (!CDlgWikiByNameBase::OnInitDialog())
		return false;

	m_pDlgConfirm = dynamic_cast<CDlgWikiItemConfirm*>(GetGameUIMan()->GetDialog("Win_WikiItemConfirm"));
	m_pDlgNpc = dynamic_cast<CDlgWikiNPCList*>(GetGameUIMan()->GetDialog("Win_WikiNpcSelect"));
	m_pDlgDetail = GetGameUIMan()->GetDialog("Win_WikiRecipeDetail");
	m_pMaterialSearcher = new MaterialSearcher(this);
	m_pProductSearcher = new ProductSearcher(this);
	m_pWikiRecipeItemCache = new WikiRecipeItemCache();

	return true;
}

bool CDlgWikiRecipe::Release(void)
{
	delete m_pMaterialSearcher;
	m_pMaterialSearcher = NULL;
	
	delete m_pProductSearcher;
	m_pProductSearcher = NULL;
	
	delete m_pWikiNPCMakingCache;
	m_pWikiNPCMakingCache = NULL;

	delete m_pWikiRecipeItemCache;
	m_pWikiRecipeItemCache = NULL;
	
	return CDlgWikiByNameBase::Release();
}

bool CDlgWikiRecipe::ConfirmSearch(unsigned int id, const ACString& name)
{
	// cache warming
	if(!m_pWikiNPCMakingCache)
	{
		m_pWikiNPCMakingCache = new WikiNPCMakingCache();
	}

	m_pDlgDetail->Show(false);
	m_pDlgNpc->Show(false);
	m_pDlgConfirm->Show(false);
	m_pTxt_Search->SetText(name);
	bool has1 = m_pMaterialSearcher->ConfirmSearch(id);
	bool has2 = m_pProductSearcher->ConfirmSearch(id);

	return has1 || has2;
}

ACString& CDlgWikiRecipe::GetItemName(unsigned int item_id, ACString& szTxt)
{
	szTxt.Empty();
	
	if(item_id > 0)
	{
		CECIvtrItem *pItem = CECIvtrItem::CreateItem(item_id, 0, 1, ID_SPACE_ESSENCE);
		szTxt = pItem->GetName();
		delete pItem;
	}

	return szTxt;
}

int CDlgWikiRecipe::GetItemIcon(unsigned int item_id)
{
	if(item_id > 0)
	{
		AString szFile;
		CECIvtrItem *pItem = CECIvtrItem::CreateItem(item_id, 0, 1, ID_SPACE_ESSENCE);
		af_GetFileTitle(pItem->GetIconFile(), szFile);
		delete pItem;

		szFile.MakeLower();
		return GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_INVENTORY][szFile];
	}
	
	return -1;
}

const RECIPE_ESSENCE* CDlgWikiRecipe::GetEssence(unsigned int id) const
{
	DATA_TYPE dataType = DT_INVALID;
	const RECIPE_ESSENCE *pEssence = (const RECIPE_ESSENCE *)g_pGame->GetElementDataMan()->
		get_data_ptr(id, ID_SPACE_RECIPE, dataType);

	return (dataType == DT_RECIPE_ESSENCE) ? pEssence : NULL;
}

void CDlgWikiRecipe::OnCommand_SearchByName(const char *szCommand)
{
	const ACHAR* szTxt = m_pTxt_Search->GetText();
	
	// seal the direct call
	if(!WikiSearchByName::IsSearchPatternValid(szTxt))
	{
		return;
	}

	// cache warming
	if(!m_pWikiNPCMakingCache)
	{
		m_pWikiNPCMakingCache = new WikiNPCMakingCache();
	}
	
	m_pDlgConfirm->SetItemProvider(&WikiRecipeItemProvider(m_pWikiRecipeItemCache, szTxt));
	m_pDlgConfirm->Show(!m_pDlgConfirm->TestEmpty(), true);
}

void CDlgWikiRecipe::OnCommand_MaterialPrev(const char *szCommand)
{
	m_pMaterialSearcher->TurnPageUp();
}
void CDlgWikiRecipe::OnCommand_MaterialNext(const char *szCommand)
{
	m_pMaterialSearcher->TurnPageDown();
}
void CDlgWikiRecipe::OnCommand_ProductPrev(const char *szCommand)
{
	m_pProductSearcher->TurnPageUp();
}
void CDlgWikiRecipe::OnCommand_ProductNext(const char *szCommand)
{
	m_pProductSearcher->TurnPageDown();
}

void CDlgWikiRecipe::OnEventLButtonDBCLK_ListMaterial(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	const RECIPE_ESSENCE* pEssence = m_pMaterialSearcher->GetCurEssence();
	if (pEssence)
	{
		m_pDlgNpc->SetNPCProvider(&WikiNPCMakeServiceProvider(m_pWikiNPCMakingCache, pEssence->id), true);
		m_pDlgNpc->Show(!m_pDlgNpc->TestEmpty(), true);
	}
}

void CDlgWikiRecipe::OnEventLButtonDBCLK_ListProduct(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	const RECIPE_ESSENCE* pEssence = m_pProductSearcher->GetCurEssence();
	if (pEssence)
	{
		m_pDlgNpc->SetNPCProvider(&WikiNPCMakeServiceProvider(m_pWikiNPCMakingCache, pEssence->id), true);
		m_pDlgNpc->Show(!m_pDlgNpc->TestEmpty(), true);
	}
}


// =======================================================================
// WikiNPCMakingCache
// =======================================================================
WikiNPCMakingCache::WikiNPCMakingCache()
{
	SetContentProvider(&WikiElementDataProvider(ID_SPACE_ESSENCE));
	Refresh(&WikiSearchNPCByServiceMake());
}

int  WikiNPCMakingCache::GetPageSize() const
{
	// set it a big number for cache
	return 1000000;
}

void WikiNPCMakingCache::OnBeginSearch()
{
	m_NPCCache.clear();
}

bool WikiNPCMakingCache::IsValid(unsigned int recipe_id, WikiNPCMakingCache::Info* pData)
{
	CacheType::iterator itr = m_NPCCache.find(recipe_id);
	
	if(itr == m_NPCCache.end())
	{
		return false;
	}

	if(pData)
	{
		*pData = itr->second;
	}

	return true;
}

WikiNPCMakingCache::CacheRange WikiNPCMakingCache::GetValidNPC(unsigned int recipe_id)
{
	return m_NPCCache.equal_range(recipe_id);
}

bool WikiNPCMakingCache::OnAddSearch(WikiEntityPtr p)
{
	WikiElementDataProvider::Entity* pEE = dynamic_cast<WikiElementDataProvider::Entity*>(p.Get());
	ASSERT(pEE); // should always got the ptr
	if(!pEE) return false;
	
	// get data from entity first
	const NPC_ESSENCE* pEssence = NULL;
	if(!pEE->GetConstData(pEssence)) return false;

	unsigned int idMake = pEssence->id_make_service;
	if(!idMake) return false;

	DATA_TYPE dataType = DT_INVALID;
	const NPC_MAKE_SERVICE *pMake = (const NPC_MAKE_SERVICE *)g_pGame->GetElementDataMan()->
		get_data_ptr(idMake, ID_SPACE_ESSENCE, dataType);
	if(dataType != DT_NPC_MAKE_SERVICE) return false;

	size_t maxPage = sizeof(pMake->pages) / sizeof(pMake->pages[0]);
	size_t maxGoods = sizeof(pMake->pages[0].id_goods) / sizeof(pMake->pages[0].id_goods[0]);
	WikiNPCMakingCache::Info npcMake = {pEssence, pMake};

	for(size_t i=0;i<maxPage;i++)
	{
		for(size_t j=0;j<maxGoods;j++)
		{
			unsigned int recipeID = pMake->pages[i].id_goods[j];
			if(recipeID == 0) continue;
			m_NPCCache.insert(CachePair(recipeID,npcMake));
			
			// warm the item cache when find a valid recipe.
			// remove this if you want search valid item onlive.
			WikiRecipeItemCache::Warm(recipeID);
		}
	}
	
	return true;
}

// =======================================================================
// WikiNPCMakeServiceProvider
// =======================================================================
WikiNPCMakeServiceProvider::WikiNPCMakeServiceProvider(WikiNPCMakingCache* pCache, unsigned int recipe_id)
:m_pCache(pCache)
,m_RecipeID(recipe_id)
{
}

WikiEntityPtr WikiNPCMakeServiceProvider::Next(WikiEntityPtr p)
{
	Entity* ptr = dynamic_cast<Entity*>(p.Get());
	ASSERT(ptr);
	if(!ptr) return NULL;
	
	iterator itr = ptr->GetIterator();
	if(itr == m_Range.second) return NULL;

	return (++itr == m_Range.second) ? NULL:new Entity(itr);
}

WikiEntityPtr WikiNPCMakeServiceProvider::Begin()
{
	m_Range = m_pCache->GetValidNPC(m_RecipeID);
	return (m_Range.first == m_Range.second) ? NULL:new Entity(m_Range.first);
}

WikiSearcher::ContentProvider* WikiNPCMakeServiceProvider::Clone() const
{
	return new WikiNPCMakeServiceProvider(m_pCache, m_RecipeID);
}

CDlgNameLink::LinkCommand* WikiNPCMakeServiceProvider::CreateLinkCommand(WikiNPCEssenceProvider::Entity* ptr)
{
	WikiNPCMakeServiceProvider::Entity* pEE = dynamic_cast<WikiNPCMakeServiceProvider::Entity*>(ptr);
	if(!pEE) return NULL;
	
	iterator itr = pEE->GetIterator();
	const NPC_ESSENCE* pNPC = itr->second.pNPC;

	return new CDlgWikiRecipeDetail::ShowSpecficLinkCommand((*itr), pNPC->name);
}

// =======================================================================
// WikiRecipeItemCache
// =======================================================================
WikiRecipeItemCache::CacheType WikiRecipeItemCache::s_Cache;
bool WikiRecipeItemCache::s_Warmed = false;

bool WikiRecipeItemCache::Warm(unsigned int id)
{
	DATA_TYPE dataType = DT_INVALID;
	const RECIPE_ESSENCE *pEssence = (const RECIPE_ESSENCE *)g_pGame->GetElementDataMan()->
		get_data_ptr(id, ID_SPACE_RECIPE, dataType);
	
	if(dataType != DT_RECIPE_ESSENCE)
	{
		return false;
	}

	s_Warmed = true;

	size_t i, maxItem;

	maxItem = sizeof(pEssence->materials) / sizeof(pEssence->materials[0]);
	for(i=0;i<maxItem;i++)
	{
		unsigned int id = pEssence->materials[i].id;
		if(id != 0 && pEssence->materials[i].num > 0)
		{
			if(s_Cache.find(id) == s_Cache.end())
			{
				s_Cache.insert(CachePair(id, true));
			}
		}
	}

	maxItem = sizeof(pEssence->targets) / sizeof(pEssence->targets[0]);
	for(i=0;i<maxItem;i++)
	{
		unsigned int id = pEssence->targets[i].id_to_make;
		if(id != 0 && pEssence->targets[i].probability > 0.f)
		{
			if(s_Cache.find(id) == s_Cache.end())
			{
				s_Cache.insert(CachePair(id, true));
			}
		}
	}

	return true;
}

bool WikiRecipeItemCache::IsValid(unsigned int id)
{
	CacheType::iterator itr = s_Cache.find(id);
	
	bool isValid = false;

	if(s_Warmed)
	{		
		isValid = (itr != s_Cache.end());
	}
	else
	{
		if(itr != s_Cache.end())
		{
			isValid = (*itr).second;
		}
		else
		{
			// search onlive
			isValid  = Refresh(&WikiSearchRecipeByProductID(id)) || 
				Refresh(&WikiSearchRecipeByMaterialID(id));
			s_Cache.insert(CachePair(id, isValid));
		}
	}

	return isValid;
}

// =======================================================================
// WikiRecipeItemProvider
// =======================================================================
WikiRecipeItemProvider::WikiRecipeItemProvider(WikiRecipeItemCache* pCache, const ACHAR* pName)
:WikiItemProvider(pName)
,m_pCache(pCache)
{}

WikiSearcher::ContentProvider* WikiRecipeItemProvider::Clone() const
{ 
	return new WikiRecipeItemProvider(m_pCache, GetName());
}

CDlgNameLink::LinkCommand* WikiRecipeItemProvider::CreateLinkCommand(WikiItemProvider::Entity* ptr)
{
	WikiItemProp* pEP = NULL;
	if (!ptr || !ptr->GetData(pEP))
	{
		return NULL;
	}
	
	unsigned int id = ptr->GetID();
	if(!m_pCache || !m_pCache->IsValid(id))
	{
		return NULL;
	}
	
	return new CDlgWikiRecipe::ShowSpecficLinkCommand(id, pEP->GetName());
}

// =======================================================================
ElementNameCache::ElementNameCache()
{
	memset(m_Cache, 0, sizeof(m_Cache));
	
	CachePair* ptr = m_Cache;
	while(ptr != m_Cache + CACHE_SIZE)
	{
		ptr->first = ptr - m_Cache;
		ptr++;
	}
	
	// 根据名称模糊查询物品及其ID
#define CHECK_ESSENCE(e) if (dataType == DT_##e) \
	{ \
	e *pEssence = (e *)pElementDataMan->get_data_ptr(idTemp, ID_SPACE_ESSENCE, dataType); \
	const ACHAR* pName = pEssence->name;	\
	const ACHAR* pNameEnd = pEssence->name + sizeof(pEssence->name) / sizeof(ACHAR);	\
	while(*pName && pName < pNameEnd) ++m_Cache[*(pName++)].second;	\
	}
	
	DATA_TYPE dataType;
	elementdataman *pElementDataMan = g_pGame->GetElementDataMan();
	unsigned int idTemp = pElementDataMan->get_first_data_id(ID_SPACE_ESSENCE, dataType);
	while (idTemp)
	{
		CHECK_ESSENCE(WEAPON_ESSENCE)
			else CHECK_ESSENCE(ARMOR_ESSENCE)
			else CHECK_ESSENCE(DECORATION_ESSENCE)
			else CHECK_ESSENCE(MEDICINE_ESSENCE)
			else CHECK_ESSENCE(MATERIAL_ESSENCE)
			else CHECK_ESSENCE(DAMAGERUNE_ESSENCE)
			else CHECK_ESSENCE(ARMORRUNE_ESSENCE)
			else CHECK_ESSENCE(SKILLTOME_ESSENCE)
			else CHECK_ESSENCE(FLYSWORD_ESSENCE)
			else CHECK_ESSENCE(WINGMANWING_ESSENCE)
			else CHECK_ESSENCE(TOWNSCROLL_ESSENCE)
			else CHECK_ESSENCE(UNIONSCROLL_ESSENCE)
			else CHECK_ESSENCE(REVIVESCROLL_ESSENCE)
			else CHECK_ESSENCE(ELEMENT_ESSENCE)
			else CHECK_ESSENCE(TASKMATTER_ESSENCE)
			else CHECK_ESSENCE(TOSSMATTER_ESSENCE)
			else CHECK_ESSENCE(PROJECTILE_ESSENCE)
			else CHECK_ESSENCE(QUIVER_ESSENCE)
			else CHECK_ESSENCE(STONE_ESSENCE)
			else CHECK_ESSENCE(MONSTER_ESSENCE)
			else CHECK_ESSENCE(NPC_ESSENCE)
			else CHECK_ESSENCE(FACE_TEXTURE_ESSENCE)
			else CHECK_ESSENCE(FACE_SHAPE_ESSENCE)
			else CHECK_ESSENCE(FACE_EXPRESSION_ESSENCE)
			else CHECK_ESSENCE(FACE_HAIR_ESSENCE)
			else CHECK_ESSENCE(FACE_MOUSTACHE_ESSENCE)
			else CHECK_ESSENCE(COLORPICKER_ESSENCE)
			else CHECK_ESSENCE(CUSTOMIZEDATA_ESSENCE)
			else CHECK_ESSENCE(RECIPE_ESSENCE)
			else CHECK_ESSENCE(TASKDICE_ESSENCE)
			else CHECK_ESSENCE(TASKNORMALMATTER_ESSENCE)
			else CHECK_ESSENCE(FACE_FALING_ESSENCE)
			else CHECK_ESSENCE(MINE_ESSENCE)
			else CHECK_ESSENCE(FASHION_ESSENCE)
			else CHECK_ESSENCE(FACETICKET_ESSENCE)
			else CHECK_ESSENCE(FACEPILL_ESSENCE)
			else CHECK_ESSENCE(SUITE_ESSENCE)
			else CHECK_ESSENCE(GM_GENERATOR_ESSENCE)
			else CHECK_ESSENCE(PET_ESSENCE)
			else CHECK_ESSENCE(PET_EGG_ESSENCE)
			else CHECK_ESSENCE(PET_FOOD_ESSENCE)
			else CHECK_ESSENCE(PET_FACETICKET_ESSENCE)
			else CHECK_ESSENCE(FIREWORKS_ESSENCE)
			else CHECK_ESSENCE(WAR_TANKCALLIN_ESSENCE)
			else CHECK_ESSENCE(SKILLMATTER_ESSENCE)
			else CHECK_ESSENCE(REFINE_TICKET_ESSENCE)
			else CHECK_ESSENCE(DESTROYING_ESSENCE)
			else CHECK_ESSENCE(BIBLE_ESSENCE)
			else CHECK_ESSENCE(SPEAKER_ESSENCE)
			else CHECK_ESSENCE(AUTOHP_ESSENCE)
			else CHECK_ESSENCE(AUTOMP_ESSENCE)
			else CHECK_ESSENCE(DOUBLE_EXP_ESSENCE)
			else CHECK_ESSENCE(TRANSMITSCROLL_ESSENCE)
			else CHECK_ESSENCE(DYE_TICKET_ESSENCE)
			else CHECK_ESSENCE(GOBLIN_ESSENCE)
			else CHECK_ESSENCE(GOBLIN_EQUIP_ESSENCE)
			else CHECK_ESSENCE(GOBLIN_EXPPILL_ESSENCE)
			else CHECK_ESSENCE(SELL_CERTIFICATE_ESSENCE)
			else CHECK_ESSENCE(TARGET_ITEM_ESSENCE)
			else CHECK_ESSENCE(LOOK_INFO_ESSENCE)
			else CHECK_ESSENCE(INC_SKILL_ABILITY_ESSENCE)
			else CHECK_ESSENCE(WEDDING_BOOKCARD_ESSENCE)
			else CHECK_ESSENCE(WEDDING_INVITECARD_ESSENCE)
			else CHECK_ESSENCE(SHARPENER_ESSENCE)
			else CHECK_ESSENCE(FACTION_MATERIAL_ESSENCE)
			else CHECK_ESSENCE(CONGREGATE_ESSENCE)
			else CHECK_ESSENCE(FORCE_TOKEN_ESSENCE)
			else CHECK_ESSENCE(DYNSKILLEQUIP_ESSENCE)
			else CHECK_ESSENCE(MONEY_CONVERTIBLE_ESSENCE)
			else CHECK_ESSENCE(MONSTER_SPIRIT_ESSENCE)
			else CHECK_ESSENCE(POKER_DICE_ESSENCE)
			else CHECK_ESSENCE(POKER_ESSENCE)
			else CHECK_ESSENCE(SHOP_TOKEN_ESSENCE)
			else CHECK_ESSENCE(UNIVERSAL_TOKEN_ESSENCE)
			idTemp = pElementDataMan->get_next_data_id(ID_SPACE_ESSENCE, dataType);
	}
	
	std::sort(m_Cache, m_Cache + CACHE_SIZE, greater<CachePair>());

	std::ofstream f("word.csv");
	ACHAR szTxt[2] = {0};
	CachePair* p = m_Cache;
	while(p->second > 0 && p!= m_Cache + CACHE_SIZE)
	{
		szTxt[0] = p->first;
		f<<std::string(AC2AS(ACString(szTxt))).c_str()<<","<<p->second<<std::endl;
		p++;
	}
	f.close();
}

void CDlgWikiRecipe::OnCommandCancel()
{
	m_pDlgDetail->Show(false);
	m_pDlgNpc->OnCommand_CANCEL("");	
	m_pDlgConfirm->OnCommand_CANCEL("");
	CDlgWikiByNameBase::OnCommandCancel();
}

//////////////////////////////////////////////////////////////////////////

void CDlgWikiShortcut::PopSkillWiki(CECGameUIMan* pGameUIMan,int skill_id)
{
	pGameUIMan->m_pDlgAutoHelp->ResetAutoHelpTime();
	CDlgWikiByNameBase* pDlg = dynamic_cast<CDlgWikiByNameBase*>( pGameUIMan->GetDialog("Win_WikiSkillSearch") );
	if (pDlg)
	{
		ACString skill_name = g_pGame->GetSkillDesc()->GetWideString(skill_id * 10);
		pDlg->SetSearchText(skill_name);
	//	pDlg->OnCommand_SearchByName(NULL);
	//	pDlg->Show(true);
		CDlgWikiSkill::ShowSpecficLinkCommand cmd(skill_id,skill_name);
		cmd(NULL);
	}
}
void CDlgWikiShortcut::PopQuestWiki(CECGameUIMan* pGameUIMan,int taskid)
{
/*	CDlgWikiByNameBase* pDlg = dynamic_cast<CDlgWikiByNameBase*>( pGameUIMan->GetDialog("Win_WikiTaskSearch") );
	if (pDlg)
	{
		pDlg->SetSearchText(task_name);
		pDlg->OnCommand_SearchByName(NULL);
		pDlg->Show(true);
	}*/
	ATaskTempl* pTaskTempl = GetTaskTemplMan()->GetTaskTemplByID(taskid);
	if(pTaskTempl&& g_Checker(pTaskTempl))
	{
		pGameUIMan->m_pDlgAutoHelp->ResetAutoHelpTime();
		CDlgWikiTask* pDlg = dynamic_cast<CDlgWikiTask*>(pGameUIMan->GetDialog("Win_WikiTaskSearch"));

		ACString name = pTaskTempl->GetName();
		int start = 0;
		int end = 0;
		ACString str;
		while(true && start < name.GetLength())
		{
			int end = name.Find(_AL("^"),start);
			if(end<0) 
			{
				str += name.Mid(start);
				break;
			}
			if (start == end)
			{
				start = end + 7;
				continue;
			}		
			str += name.Mid(start,end-start);
			start = end + 7;
		}
		if(pDlg) pDlg->SetSearchText(str);
		
	//	pDlg->OnCommand_SearchByName(NULL);
	//	pDlg->Show(true);
		CDlgWikiTask::ShowSpecficLinkCommand wiki(taskid,pTaskTempl->GetName());
		wiki(NULL);
	}
	else
		pGameUIMan->MessageBox("",pGameUIMan->GetStringFromTable(10750),MB_OK,A3DCOLORRGB(255,255,255));	
}

void CDlgWikiShortcut::PopItemWiki(CECGameUIMan* pGameUIMan,CECIvtrItem* pItem)
{
	if(!pItem) return;
	pGameUIMan->m_pDlgAutoHelp->ResetAutoHelpTime();

	CDlgWikiRecipe* pDlg = dynamic_cast<CDlgWikiRecipe*>(pGameUIMan->GetDialog("Win_WikiRecipeSearch"));
	if(pDlg) pDlg->SetSearchText(pItem->GetName());
	CDlgWikiRecipe::ShowSpecficLinkCommand wikiProduce(pItem->GetTemplateID(),pItem->GetName());
	if (!wikiProduce(NULL))
	{
		if (pItem->IsEquipment())
		{
			CDlgWikiEquipment* pDlg = dynamic_cast<CDlgWikiEquipment*>(pGameUIMan->GetDialog("Win_WikiEquipmentSearch"));
			if(pDlg) pDlg->SetSearchText(pItem->GetName());
			CDlgWikiEquipment::ShowSpecficLinkCommand wiki(pItem->GetTemplateID(),pItem->GetName());
			wiki(NULL);
		}
		else
		{
			CDlgWikiItem* pDlg = dynamic_cast<CDlgWikiItem*>(pGameUIMan->GetDialog("Win_WikiItemSearch"));
			if(pDlg) pDlg->SetSearchText(pItem->GetName());
			CDlgWikiItem::ShowSpecficLinkCommand wiki(pItem->GetTemplateID(),pItem->GetName());
			wiki(NULL);
		}
	}
}
void CDlgWikiShortcut::PopMonsterWiki(CECGameUIMan* pGameUIMan,int idTarget)
{
	pGameUIMan->m_pDlgAutoHelp->ResetAutoHelpTime();
	CECNPC* pTarget = g_pGame->GetGameRun()->GetWorld()->GetNPCMan()->GetNPC(idTarget);
	if(pTarget && pTarget->IsMonsterNPC())
	{
		CECNPC* npc = dynamic_cast<CECNPC*>(pTarget);
		if (npc)
		{
			CDlgWikiMonster* pDlg = dynamic_cast<CDlgWikiMonster*>(pGameUIMan->GetDialog("Win_WikiMonsterSearch"));
			if(pDlg)pDlg->SetSearchText(npc->GetName());
			CDlgWikiMonster::ShowSpecficLinkCommand wiki(npc->GetTemplateID(),npc->GetName());
			wiki(NULL);
		}
	}
}