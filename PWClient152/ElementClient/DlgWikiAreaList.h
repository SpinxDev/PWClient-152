// Filename	: DlgWikiAreaList.h
// Creator	: Feng Ning
// Date		: 2010/09/30

#pragma once

#include "DlgWikiItemListBase.h"

class CDlgWikiAreaList : public CDlgWikiItemListBase
{
	AUI_DECLARE_EVENT_MAP()
	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgWikiAreaList();

protected:
	virtual bool OnModityItem(WikiEntityPtr p, PAUIOBJECT pObj, PAUIOBJECT pName);

	// derived from AUI
	virtual bool OnInitDialog();
};