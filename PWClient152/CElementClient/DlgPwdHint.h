// Filename	: DlgPwdHint.h
// Creator	: Xu Wenbin
// Date		: 2010/02/26

#pragma once

#include "DlgBase.h"
#include "AUITextArea.h"

class CDlgPwdHint : public CDlgBase  
{
	AUI_DECLARE_EVENT_MAP();
	AUI_DECLARE_COMMAND_MAP()
	
public:
	
	void OnCommand_CANCEL(const char * szCommand);
	
	void OnEventLButtonDown_Txt_Content(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventMouseMove_Txt_Content(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	
protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnTick();

	bool GetNameLinkMouseOn(int x, int y,PAUIOBJECT pObj, P_AUITEXTAREA_NAME_LINK pLink);

	PAUITEXTAREA m_pTxt_Content;
};