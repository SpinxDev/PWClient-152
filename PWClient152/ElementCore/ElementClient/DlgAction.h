// Filename	: DlgAction.h
// Creator	: Tom Zhou
// Date		: October 19, 2005

#pragma once

#include "DlgBase.h"

/*
class CDlgAction : public CDlgBase  
{
	AUI_DECLARE_EVENT_MAP();

public:
	CDlgAction();
	virtual ~CDlgAction();

	void OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject *pObj);

protected:
	virtual void OnTick();
};
*/
//////////////////////////////////////////////////////////////////////////

class CDlgTaskAction: public CDlgBase
{
	AUI_DECLARE_EVENT_MAP();
public:
	void OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
protected:
	virtual void OnShowDialog();
	virtual void OnHideDialog();
	virtual void OnTick();

private:
};
