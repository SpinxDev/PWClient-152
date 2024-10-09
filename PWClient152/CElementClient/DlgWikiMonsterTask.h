// Filename	: DlgWikiMonsterTask.h
// Creator	: Xu Wenbin
// Date		: 2010/04/07

#pragma once

#include "DlgWikiTaskList.h"

class CDlgWikiMonsterTask : public CDlgWikiTaskList
{
	AUI_DECLARE_EVENT_MAP()
	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgWikiMonsterTask();
	
	virtual void SetSearchDataPtr(void* ptr);
};