// File		: DlgConsole.h
// Creator	: Xiao Zhou
// Date		: 2005/8/16

#pragma once

#include "DlgBase.h"
#include "AUIConsole.h"
#include "AUIEditBox.h"

class CDlgConsole : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()

public:

	CDlgConsole();
	virtual ~CDlgConsole();
	virtual void Resize(A3DRECT rcOld, A3DRECT rcNew);
	
	void OnCommandEdit(const char * szCommand);

	void OnEventKeyDown(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnEventChar(WPARAM wParam, LPARAM lParam, AUIObject * pObj);

protected:
	virtual void DoDataExchange(bool bSave);
	
	PAUIOBJECT	m_pCommand;
	AUIConsole *	m_pCommandEdit;
};
