// Filename	: DlgFortressMini.h
// Creator	: Xu Wenbin
// Date		: 2010/11/18

#pragma once

#include "DlgBase.h"

class CDlgFortressMini : public CDlgBase
{
	AUI_DECLARE_EVENT_MAP();
	AUI_DECLARE_COMMAND_MAP()
	
public:

	CDlgFortressMini();
	
	void OnCommand_CANCEL(const char *szCommand);
	void OnEventLButtonUp_Img_FortressStatus(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
};