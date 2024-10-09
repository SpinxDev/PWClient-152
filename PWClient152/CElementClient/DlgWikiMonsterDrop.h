// Filename	: DlgWikiMonsterDrop.h
// Creator	: Xu Wenbin
// Date		: 2010/04/07

#pragma once

#include "DlgWikiItemListBase.h"

class CDlgWikiMonsterDrop : public CDlgWikiItemListBase
{
	AUI_DECLARE_EVENT_MAP()
	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgWikiMonsterDrop();
	
	virtual void SetSearchDataPtr(void* ptr);
protected:
	virtual bool OnModityItem(WikiEntityPtr p, PAUIOBJECT pObj, PAUIOBJECT pName);
};