// Filename	: DlgWikiBase.h
// Creator	: Xu Wenbin
// Date		: 2010/04/06

#pragma once

#include "DlgNameLink.h"
#include "AUIStillImageButton.h"
#include "WikiSearcher.h"

class CDlgWikiBase : public CDlgNameLink, public WikiSearcher
{
	AUI_DECLARE_EVENT_MAP()
	AUI_DECLARE_COMMAND_MAP()
	
public:
	CDlgWikiBase();
	
	void OnCommand_Prev(const char *szCommand);
	void OnCommand_Next(const char *szCommand);
	void OnCommand_CANCEL(const char * szCommand);

	bool RefreshByNewCommand(const SearchCommand* pCommand, bool bForceShow = false);
	
	virtual void SetSearchDataPtr(void* ptr);

protected:

	// sync the UI buttons status
	virtual void OnEndSearch();

	// this will be called when the UI was closed
	virtual void OnCommandCancel(){}

protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnTick();
	virtual bool Release(void);

	PAUISTILLIMAGEBUTTON m_pBtn_Prev;
	PAUISTILLIMAGEBUTTON m_pBtn_Next;
};