// Filename	: DlgLoginButton.h
// Creator	: Xu Wenbin
// Date		: 2013/9/5
#pragma once

#include "DlgBase.h"
#include "AUIManager.h"

class CDlgLoginButton : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP();

public:
	CDlgLoginButton();

	void OnCommandCancel(const char* szCommand);
	void OnCommandChangeServer(const char* szCommand);
	void OnCommandLink(const char* szCommand);
};

