// Filename	: DlgReincarnationRewrite.h
// Creator	: Han Guanghui
// Date		: 2013/08/06

#pragma once

#include "DlgBase.h"
#include "AUIEditBox.h"

class CDlgReincarnationRewrite : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP();

public:
	CDlgReincarnationRewrite();
	virtual ~CDlgReincarnationRewrite();

	void OnCommandRewrite(const char* szCommand);
	void OnCommandCancel(const char* szCommand);
	void OnCommandSleepWake(const char* szCommand);
	void Update();

protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnHideDialog();
	void UpdateBookUI();

};
