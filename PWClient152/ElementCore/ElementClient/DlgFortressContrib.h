// Filename	: DlgFortressContrib.h
// Creator	: Xu Wenbin
// Date		: 2010/09/29

#pragma once

#include "DlgBase.h"
#include "AUIEditbox.h"

class CDlgFortressContrib : public CDlgBase
{
	AUI_DECLARE_EVENT_MAP();
	AUI_DECLARE_COMMAND_MAP()
	
public:
	CDlgFortressContrib();
	
	void OnCommand_CANCEL(const char *szCommand);
	void OnCommand_Contrib(const char *szCommand);

	void OnEventKeyDown_Edit_01(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	
	void UpdateInfo();
	
protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnTick();

	int GetContribToSend();
	
private:
	
	PAUIOBJECT	m_pBtn_Contrib;
	PAUIOBJECT	m_pLab_Contrib;
	PAUIOBJECT	m_pEdit_01;
};