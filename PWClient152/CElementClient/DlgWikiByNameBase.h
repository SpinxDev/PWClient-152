// Filename	: DlgWikiByNameBase.h
// Creator	: Feng Ning
// Date		: 2010/04/26

#pragma once

#include "DlgWikiBase.h"
#include "AUIEditBox.h"

class CDlgWikiByNameBase : public CDlgWikiBase
{
	AUI_DECLARE_EVENT_MAP()
	AUI_DECLARE_COMMAND_MAP()
	
public:
	CDlgWikiByNameBase();
	
	void OnCommand_SearchByName(const char *szCommand);
	void OnEventLButtonDown_Txt_Search(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnEventKeyDown(WPARAM wParam, LPARAM lParam, AUIObject * pObj);

	void SetSearchText(const ACHAR * szPattern);
	void ShowHint(bool isShow);

protected:
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnTick();
	virtual bool Release(void);

	bool m_ShowHint;
	PAUIOBJECT m_pTxt_Search;	
	PAUISTILLIMAGEBUTTON m_pBtn_Search;
};