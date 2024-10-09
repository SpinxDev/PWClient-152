#pragma once

#include "DlgBase.h"

class CDlgLoginReward : public CDlgBase  
{	
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()

public:

	void OnCommandReward(const char *szCommand);

	CDlgLoginReward();
	virtual ~CDlgLoginReward();


protected:

};
