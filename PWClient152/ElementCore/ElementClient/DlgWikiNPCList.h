// Filename	: DlgWikiNPCList.h
// Creator	: Feng Ning
// Date		: 2010/07/23

#pragma once

#include "DlgWikiItemListBase.h"
#include "WikiSearcher.h"

struct NPC_ESSENCE;
// the base class for searching NPC by specific command
class WikiNPCEssenceProvider : public WikiSearcher::ContentProvider
{
public:
	class Entity : public WikiEntity
	{
	public:
		Entity(const NPC_ESSENCE* ptr):m_Ptr(ptr) {}
		virtual bool operator==(const WikiEntity& rhs) const
		{
			const Entity* ptr = dynamic_cast<const Entity*>(&rhs);
			return !ptr ? false : m_Ptr == ptr->m_Ptr;
		}

		const NPC_ESSENCE* GetNPC() const { return m_Ptr; }
	private:
		const NPC_ESSENCE* m_Ptr;
	};
	
	// should be deleted outside
	virtual CDlgNameLink::LinkCommand* CreateLinkCommand(WikiNPCEssenceProvider::Entity* ptr)  = 0;
};

class CDlgWikiNPCList : public CDlgWikiItemListBase
{
	AUI_DECLARE_EVENT_MAP()
	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgWikiNPCList();
	
	void SetNPCProvider(const WikiNPCEssenceProvider* pData, bool isAutoSelectNearest);

protected:
	virtual void OnBeginSearch();
	virtual void OnEndSearch();
	virtual bool OnModityItem(WikiEntityPtr p, PAUIOBJECT pObj, PAUIOBJECT pName);

	// derived from AUI
	virtual bool OnInitDialog();
	virtual bool Release();

	void SetDefaultCommand(LinkCommand* pCmd);

	bool m_isAutoSelectNearest;
	A3DVECTOR3 m_Nearest;
	LinkCommand* m_pCmd;
};
