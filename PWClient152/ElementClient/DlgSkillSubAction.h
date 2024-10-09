// Filename	: DlgSkillSubAction.h
// Creator	: zhangyitian
// Date		: 2014/7/7

#ifndef _ELEMENTCLIENT_DLGSKILLSUBACTION_H_
#define _ELEMENTCLIENT_DLGSKILLSUBACTION_H_

#include "DlgBase.h"

class CDlgSkillSubAction : public CDlgBase {

	AUI_DECLARE_EVENT_MAP();

public:
	CDlgSkillSubAction();
	void OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnMouseWheel(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	virtual void OnChangeLayoutEnd(bool bAllDone);
protected:
	virtual void OnTick();
	virtual void OnShowDialog();
private:
	bool m_bSetVScroll;
};

#endif