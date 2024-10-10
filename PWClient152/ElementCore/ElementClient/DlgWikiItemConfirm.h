// Filename	: DlgWikiItemConfirm.h
// Creator	: Feng Ning
// Date		: 2010/07/21

#pragma once

#include "DlgWikiItemListBase.h"
#include "WikiSearchCommand.h"

// for convenient we just derived from WikiElementDataProvider
class WikiItemProvider : public WikiElementDataProvider
{
public:
	WikiItemProvider(const ACHAR* pName);
	const ACString& GetName() const { return m_Name; }

	// should be delete outside
	virtual CDlgNameLink::LinkCommand* CreateLinkCommand(WikiItemProvider::Entity* ptr) = 0;
	
	// do *NOT* forget to implement the Clone() in derived class
	virtual WikiSearcher::ContentProvider* Clone() const { return NULL; }

private:
	ACString m_Name;
};

class CDlgWikiItemConfirm : public CDlgWikiItemListBase
{
	AUI_DECLARE_EVENT_MAP()
	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgWikiItemConfirm();	
	void SetItemProvider(WikiItemProvider* pData);

protected:
	virtual void OnBeginSearch();
	virtual void OnEndSearch();
	virtual bool OnModityItem(WikiEntityPtr p, PAUIOBJECT pObj, PAUIOBJECT pName);

	bool	m_OneCmdOnly;
	PAUIOBJECT m_pObj;
};