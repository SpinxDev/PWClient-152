// Filename	: DlgWikiSkill.h
// Creator	: Feng Ning
// Date		: 2010/09/14

#pragma once

#include "DlgWikiByNameBase.h"
#include "DlgWikiNPCList.h"
#include "AUIListBox.h"
#include "AUIComboBox.h"

class CECSkill;
class WikiNPCSkillCache;
class CDlgWikiSkill : public CDlgWikiByNameBase
{
	AUI_DECLARE_EVENT_MAP()
	AUI_DECLARE_COMMAND_MAP()

public:

	// click this link command will jump to current wiki
	class ShowSpecficLinkCommand : public CDlgNameLink::LinkCommand
	{
	public:
		virtual bool operator()(P_AUITEXTAREA_NAME_LINK pLink);
		ShowSpecficLinkCommand(unsigned int skill_id, const ACString& name);
		
	protected:
		virtual void AppendText();
		virtual ACString GetLinkText()  const;
		virtual LinkCommand* Clone() const;
		
	private:
		unsigned int m_SkillID;
		ACString m_Name;
	};

	// click this link command will set npc name to skill wiki
	class SetNPCNameLinkCommand : public CDlgNameLink::LinkCommand
	{
	public:
		virtual bool operator()(P_AUITEXTAREA_NAME_LINK pLink);
		SetNPCNameLinkCommand(const NPC_ESSENCE* pNPC);
		
	protected:
		virtual ACString GetLinkText()  const;
		virtual LinkCommand* Clone() const;
		
	private:
		const NPC_ESSENCE* m_pNPC;
	};
	
public:
	CDlgWikiSkill();
		
	void OnEventLButtonDown_List(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventKeyDown_List(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventLButtonDBLCLK_Icon(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnCommand_SelectClass(const char* szCommand);

	virtual void ResetSearchCommand();

	void SetCurrentNPCLink(const NPC_ESSENCE* pNPC);
	ACString GetWeaponLimit(CECSkill& skill);

protected:
	virtual bool OnInitDialog();
	virtual bool Release(void);
	virtual void OnShowDialog();
	void ClearStoredInfo();

protected:
	
	// derived from wiki base
	virtual void OnBeginSearch();
	virtual bool OnAddSearch(WikiEntityPtr p);
	virtual void OnEndSearch();
	virtual void OnCommandCancel();

private:
	PAUILISTBOX m_pList;
	PAUICOMBOBOX m_pClassList;

	WikiNPCSkillCache* m_pWikiNPCSkillCache;
};

struct NPC_SKILL_SERVICE;
// cache for valid NPC who has a valid skill service
class WikiNPCSkillCache : public WikiSearcher
{
public:
	struct Info
	{
		const NPC_ESSENCE* pNPC;
		const NPC_SKILL_SERVICE* pSrv;
	};
	
	WikiNPCSkillCache();
	
	// derived from wiki base
	virtual void OnBeginSearch();
	virtual bool OnAddSearch(WikiEntityPtr p);
	virtual int  GetPageSize() const;
	
	typedef std::pair<unsigned int, Info> CachePair;
	typedef std::multimap<unsigned int, Info> CacheType;
	typedef std::pair<CacheType::iterator, CacheType::iterator> CacheRange;
	
	bool IsValid(unsigned int skill_id, Info* pData);
	CacheRange GetValidNPC(unsigned int skill_id);
	
	private:
		CacheType m_NPCCache;
};

class WikiNPCSkillServiceProvider : public WikiNPCEssenceProvider
{
public:
	typedef WikiNPCSkillCache::CacheType::iterator iterator;
	
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
	
	WikiNPCSkillServiceProvider(WikiNPCSkillCache* pCache, unsigned int skill_id);
	virtual WikiEntityPtr Next(WikiEntityPtr p);
	virtual WikiEntityPtr Begin();
	virtual WikiSearcher::ContentProvider* Clone() const;
	
	// should be deleted outside
	virtual CDlgNameLink::LinkCommand* CreateLinkCommand(WikiNPCEssenceProvider::Entity* ptr);
	
	private:
		WikiNPCSkillCache::CacheRange m_Range;
		WikiNPCSkillCache* m_pCache;
		unsigned int m_SkillID;
};