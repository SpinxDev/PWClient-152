#pragma once

#include "DlgBase.h"

class AUITreeView;
class CECScriptContext;
class AUIStillImageButton;

class CDlgScriptHelpHistory : public CDlgBase
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()
private:
	AUITreeView * m_pTVHistory;
	CECScriptContext * m_pContext;
	AUIStillImageButton * m_pBtnReactive;
protected:
	virtual void DoDataExchange(bool bSave);
	virtual void OnShowDialog();
	virtual bool OnInitDialog();
private:
	void BuildTypeItems();
	void BuildHistory();
public:
	CDlgScriptHelpHistory();
	void SetScriptContext(CECScriptContext * pContext);
	void ClearHistory();
	void AddHistory(int nType, AString strName, int nID);
	void DeleteHistory(int nType, int nID);

	void OnCommandReactive(const char * szCommand);
	void OnLButtonUpHistoryTree(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
};