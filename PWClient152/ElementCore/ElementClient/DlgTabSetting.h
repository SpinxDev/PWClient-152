// Filename	: DlgTabSetting.h
// Creator	: Wang Yongdong
// Date		: 2012/03/12

#pragma once

#include "DlgBase.h"

class CDlgTabSetting : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP();

public:
	CDlgTabSetting();
	virtual ~CDlgTabSetting();

	virtual void OnShowDialog();

public:
	void OnCommandSelType(const char * szCommand);
};
