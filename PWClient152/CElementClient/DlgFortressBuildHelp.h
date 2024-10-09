// File		: DlgFortressBuildHelp.h
// Creator	: Feng Ning
// Date		: 2010/11/24

#pragma once

#include "DlgTextHelp.h"

class CDlgFortressBuildHelp : public CDlgTextHelp
{
	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgFortressBuildHelp();
	virtual ~CDlgFortressBuildHelp();

	void OnCommand_Switch(const char* szCommand);
	void ShowTab(int index);

protected:
	virtual void OnShowDialog();
	virtual bool OnInitDialog();
	virtual bool Release();
};
