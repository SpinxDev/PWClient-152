/********************************************************************
	created:	2005/08/15
	created:	15:8:2005   15:03
	file name:	DlgSkillTree.h
	author:		yaojun
	
	purpose:	
*********************************************************************/


#pragma once

#include "DlgBase.h"

class CDlgSkillTree : public CDlgBase  
{
	AUI_DECLARE_EVENT_MAP()
protected:
	virtual void OnShowDialog();

public:
	void UpdateView();
	
	void OnEventLButtonSkill(WPARAM wParam, LPARAM lParam, AUIObject *pObj);

	CDlgSkillTree();
	virtual ~CDlgSkillTree();
};
