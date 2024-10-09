// Filename	: DlgExit.h
// Creator	: Tom Zhou
// Date		: October 27, 2005

#pragma once

#include "DlgBase.h"
#include "AUICheckBox.h"

class CDlgExit : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgExit();
	virtual ~CDlgExit();

	void OnCommand_confirm(const char * szCommand);

protected:
	PAUICHECKBOX m_pChk_Forcequit;

	virtual bool OnInitDialog();
};
