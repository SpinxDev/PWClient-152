// Filename	: DlgWikiRandomProperty.h
// Creator	: Feng Ning
// Date		: 2010/04/15

#pragma once

#include "DlgWikiItemListBase.h"

class CDlgWikiRandomProperty : public CDlgWikiItemListBase
{
	AUI_DECLARE_EVENT_MAP()
	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgWikiRandomProperty();
	
	virtual void SetSearchDataPtr(void* ptr);

protected:	
	virtual bool OnModityItem(WikiEntityPtr p, PAUIOBJECT pObj, PAUIOBJECT pName);

private:
	ACString& GetRandomPropName(unsigned int item_id, unsigned int id_rand, ACString& szTxt);
};