// Filename	: DlgELFSkillTree.h
// Creator	: Chen Zhixin
// Date		: March 2, 2009

#pragma once

#include "DlgBase.h"


class CDlgELFSkillTree : public CDlgBase  
{
	AUI_DECLARE_EVENT_MAP()
		AUI_DECLARE_COMMAND_MAP()
		
public:
	CDlgELFSkillTree();
	virtual ~CDlgELFSkillTree();
	
	void OnCommand_Senior(const char * szCommand);
	void OnCommand_Junior(const char * szCommand);
	
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnTick(void);
};
