// Filename	: DlgGuildCreate.h
// Creator	: Tom Zhou
// Date		: October 25, 2005

#pragma once

#include "DlgBase.h"
#include "AUIEditBox.h"

class CDlgGuildCreate : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgGuildCreate();
	virtual ~CDlgGuildCreate();

	void OnCommand_confirm(const char * szCommand);
	void OnCommand_CANCEL(const char * szCommand);

protected:
	PAUIOBJECT m_pDEFAULT_Txt_Input;

	virtual bool OnInitDialog();
};
