// File		: DlgGoldPassword.h
// Creator	: Xiao Zhou
// Date		: 2007/3/9

#pragma once

#include "DlgBase.h"

class CDlgGoldPassword : public CDlgBase  
{
	friend class CDlgGoldTrade;

	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgGoldPassword();
	virtual ~CDlgGoldPassword();

	void OnCommandConfirm(const char * szCommand);

protected:
	virtual void OnShowDialog();
};
