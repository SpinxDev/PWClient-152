// Filename	: DlgChannalPW.h
// Creator	: Xiao Zhou
// Date		: 2005/10/21

#pragma once

#include "DlgBase.h"
#include "AUIEditBox.h"

class CDlgChannelPW : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP();

public:
	CDlgChannelPW();
	virtual ~CDlgChannelPW();

	void OnCommandConfirm(const char* szCommand);

protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();

	PAUIOBJECT		m_pTxt_Input;
};
