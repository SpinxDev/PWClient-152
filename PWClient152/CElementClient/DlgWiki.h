// Filename	: DlgWiki.h
// Creator	: Xu Wenbin
// Date		: 2010/04/08

#pragma once

#include "DlgBase.h"
#include "AUIStillImageButton.h"

class CDlgWiki : public CDlgBase
{
	AUI_DECLARE_EVENT_MAP()
	AUI_DECLARE_COMMAND_MAP()
	
public:
	CDlgWiki();
	
	void OnCommand_ShowDialog(const char *szCommand);
	void OnCommand_CloseAutoHelp(const char* szCommand);
	void OnCommand_OpenAutoHelp(const char* szCommand);
	
protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
};