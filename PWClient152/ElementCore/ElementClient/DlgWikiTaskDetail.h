// Filename	: DlgWikiTaskDetail.h
// Creator	: Feng Ning
// Date		: 2010/05/11

#pragma once

#include "DlgTask.h"

class CDlgWikiTaskDetail : public CDlgTask
{
	AUI_DECLARE_EVENT_MAP()
	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgWikiTaskDetail();

protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual bool Release(void);
};