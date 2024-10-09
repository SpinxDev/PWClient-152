// Filename	: DlgWikiMonsterList.h
// Creator	: Feng Ning
// Date		: 2010/07/20

#pragma once

#include "DlgWikiItemListBase.h"

class CDlgWikiMonsterList : public CDlgWikiItemListBase
{
	AUI_DECLARE_EVENT_MAP()
	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgWikiMonsterList();
	
	virtual void SetSearchDataPtr(void* ptr);

protected:
	virtual bool OnModityItem(WikiEntityPtr p, PAUIOBJECT pObj, PAUIOBJECT pName);

	// derived from AUI
	virtual bool OnInitDialog();
};