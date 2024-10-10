// Filename	: DlgWikiFeature.h
// Creator	: Feng Ning
// Date		: 2010/09/25

#pragma once

#include "DlgWikiByNameBase.h"
#include "AUIListBox.h"

class CDlgWikiFeature : public CDlgWikiByNameBase
{
	AUI_DECLARE_EVENT_MAP()
	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgWikiFeature();
	
	void OnEventLButtonDown_List(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventKeyDown_List(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
		
	virtual void ResetSearchCommand();
	
protected:
	virtual bool OnInitDialog();
	virtual bool Release(void);
	
protected:
	
	// derived from wiki base
	virtual void OnBeginSearch();
	virtual bool OnAddSearch(WikiEntityPtr p);
	virtual void OnEndSearch();
	virtual void OnCommandCancel();
	
private:
	PAUILISTBOX m_pList;
};
