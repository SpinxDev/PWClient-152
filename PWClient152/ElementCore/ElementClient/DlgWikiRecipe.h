// Filename	: DlgWikiRecipe.h
// Creator	: Feng Ning
// Date		: 2010/07/19

#pragma once

#include "DlgWikiByNameBase.h"
#include "DlgWikiNPCList.h"
#include "DlgWikiItemConfirm.h"

#include "AUILabel.h"
#include "AUIListBox.h"
#include "AUIStillImageButton.h"

#include <map>

struct RECIPE_ESSENCE;
struct NPC_ESSENCE;
struct NPC_MAKE_SERVICE;

class WikiNPCMakingCache;
class WikiRecipeItemCache;
class CDlgWikiItemConfirm;
class CDlgWikiNPCList;
class CDlgWikiRecipe :  public CDlgWikiByNameBase
{
	AUI_DECLARE_EVENT_MAP()
	AUI_DECLARE_COMMAND_MAP()

public:

	// click this link command will jump to current wiki
	class ShowSpecficLinkCommand : public CDlgNameLink::LinkCommand
	{
	public:
		virtual bool operator()(P_AUITEXTAREA_NAME_LINK pLink);
		ShowSpecficLinkCommand(unsigned int item_id, const ACString& name);

	protected:
		virtual ACString GetLinkText()  const;
		virtual LinkCommand* Clone() const;
		
	private:
		unsigned int m_ItemID;
		ACString m_Name;
	};
	
public:
	CDlgWikiRecipe();
	
	void OnCommand_SearchByName(const char *szCommand);
	void OnCommand_MaterialPrev(const char *szCommand);
	void OnCommand_MaterialNext(const char *szCommand);
	void OnCommand_ProductPrev(const char *szCommand);
	void OnCommand_ProductNext(const char *szCommand);

	void OnEventLButtonDBCLK_ListMaterial(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventLButtonDBCLK_ListProduct(WPARAM wParam, LPARAM lParam, AUIObject *pObj);

	bool ConfirmSearch(unsigned int id, const ACString& name);

protected:
	virtual bool OnInitDialog();
	virtual bool Release(void);

	virtual bool OnAddSearch(WikiEntityPtr p) {return true;}
	virtual void OnCommandCancel();

protected:
	
	class MaterialSearcher : public WikiSearcher
	{
	public:
		MaterialSearcher(CDlgWikiRecipe* pDlg);

		const RECIPE_ESSENCE* GetCurEssence() const;
		bool ConfirmSearch(unsigned int id);

		// derived from wiki base
		virtual void OnBeginSearch();
		virtual bool OnAddSearch(WikiEntityPtr p);
		virtual void OnEndSearch();

		CDlgWikiRecipe* m_pDlg;
		PAUILISTBOX m_pList;
		PAUISTILLIMAGEBUTTON m_pBtn_Prev;
		PAUISTILLIMAGEBUTTON m_pBtn_Next;
	};
	friend class CDlgWikiRecipe::MaterialSearcher;

	class ProductSearcher : public WikiSearcher
	{
	public:
		ProductSearcher(CDlgWikiRecipe* pDlg);

		const RECIPE_ESSENCE* GetCurEssence() const;
		bool ConfirmSearch(unsigned int id);

		// derived from wiki base
		virtual void OnBeginSearch();
		virtual bool OnAddSearch(WikiEntityPtr p);
		virtual void OnEndSearch();

		CDlgWikiRecipe* m_pDlg;
		PAUILISTBOX m_pList;
		PAUISTILLIMAGEBUTTON m_pBtn_Prev;
		PAUISTILLIMAGEBUTTON m_pBtn_Next;
	};
	friend class CDlgWikiRecipe::ProductSearcher;

private:
	int GetItemIcon(unsigned int item_id);
	ACString& GetItemName(unsigned int item_id, ACString& szTxt);
	const RECIPE_ESSENCE* GetEssence(unsigned int id) const;

	CDlgWikiItemConfirm* m_pDlgConfirm;
	CDlgWikiNPCList* m_pDlgNpc;
	AUIDialog* m_pDlgDetail;

	MaterialSearcher* m_pMaterialSearcher;
	ProductSearcher* m_pProductSearcher;

	WikiNPCMakingCache* m_pWikiNPCMakingCache;
	WikiRecipeItemCache* m_pWikiRecipeItemCache;
};

// cache for valid NPC who has a valid making service
class WikiNPCMakingCache : public WikiSearcher
{
public:
	struct Info
	{
		const NPC_ESSENCE* pNPC;
		const NPC_MAKE_SERVICE* pSrv;
	};

	WikiNPCMakingCache();
	
	// derived from wiki base
	virtual void OnBeginSearch();
	virtual bool OnAddSearch(WikiEntityPtr p);
	virtual int  GetPageSize() const;
	
	typedef std::pair<unsigned int, Info> CachePair;
	typedef std::multimap<unsigned int, Info> CacheType;
	typedef std::pair<CacheType::iterator, CacheType::iterator> CacheRange;
	
	bool IsValid(unsigned int recipe_id, Info* pData);
	CacheRange GetValidNPC(unsigned int recipe_id);

private:
	CacheType m_NPCCache;
};

class WikiNPCMakeServiceProvider : public WikiNPCEssenceProvider
{
public:
	typedef WikiNPCMakingCache::CacheType::iterator iterator;

	class Entity : public WikiNPCEssenceProvider::Entity
	{
	public:
		Entity(const iterator& itr)
		:WikiNPCEssenceProvider::Entity(itr->second.pNPC)
		,m_Itr(itr) {}
		const iterator& GetIterator() const {return m_Itr;}
		virtual bool operator==(const WikiEntity& rhs) const
		{
			const Entity* ptr = dynamic_cast<const Entity*>(&rhs);
			return !ptr ? false : m_Itr == ptr->m_Itr;
		}
	private:
		iterator m_Itr;	
	};
	
	WikiNPCMakeServiceProvider(WikiNPCMakingCache* pCache, unsigned int recipe_id);
	virtual WikiEntityPtr Next(WikiEntityPtr p);
	virtual WikiEntityPtr Begin();
	virtual WikiSearcher::ContentProvider* Clone() const;

	// should be deleted outside
	virtual CDlgNameLink::LinkCommand* CreateLinkCommand(WikiNPCEssenceProvider::Entity* ptr);
	
private:
	WikiNPCMakingCache::CacheRange m_Range;
	WikiNPCMakingCache* m_pCache;
	unsigned int m_RecipeID;
};

// cache for valid item whose has a valid recipe
class WikiRecipeItemCache : private WikiSearcher
{
public:
	bool IsValid(unsigned int id);

public:
	static bool Warm(unsigned int id);
	static bool IsWarmed() { return s_Warmed; }
	
private:
	// derived from wiki base, for search onlive
	virtual bool OnAddSearch(WikiEntityPtr p) {return true;}
	virtual int  GetPageSize() const {return 1;}
	
	typedef std::map<unsigned int, bool> CacheType;
	typedef std::pair<unsigned int, bool> CachePair;
	static CacheType s_Cache;
	static bool s_Warmed;
};

class WikiRecipeItemProvider : public WikiItemProvider
{
public:
	WikiRecipeItemProvider(WikiRecipeItemCache* pCache, const ACHAR* pName);

	// should be delete outside
	virtual CDlgNameLink::LinkCommand* CreateLinkCommand(WikiItemProvider::Entity* ptr);
	
	// do *NOT* forget to implement the Clone()
	virtual WikiSearcher::ContentProvider* Clone() const;

private:
	WikiRecipeItemCache* m_pCache;
};

//////////////////////////////////////////////////////
class CECIvtrItem;
class CDlgWikiShortcut : public CDlgBase
{
public:
	static void PopSkillWiki(CECGameUIMan* pGameUIMan,int skill_id);
	static void PopQuestWiki(CECGameUIMan* pGameUIMan,int taskid);
	static void PopItemWiki(CECGameUIMan* pGameUIMan,CECIvtrItem* pItem);
	static void PopMonsterWiki(CECGameUIMan* pGameUIMan,int idTarget);
};