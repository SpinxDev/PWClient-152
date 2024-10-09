// Filename	: DlgWikiItemListBase.h
// Creator	: Feng Ning
// Date		: 2010/04/26

#pragma once

#include "DlgWikiBase.h"
#include "AUIStillImageButton.h"

class CDlgWikiItemListBase : public CDlgWikiBase
{
	AUI_DECLARE_EVENT_MAP()
	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgWikiItemListBase();
	
protected:

	virtual void OnBeginSearch();
	virtual bool OnAddSearch(WikiEntityPtr p);
	virtual void OnEndSearch();

	virtual void OnCommandCancel();
	virtual int  GetPageSize() const;

	// derived from AUI
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual bool Release(void);

	// derived class should implement this
	// return true if this is invalid
	virtual bool OnModityItem(WikiEntityPtr p, PAUIOBJECT pObj, PAUIOBJECT pName) = 0;

private:
	int m_ObjIndex;
	int m_ObjCount;
};