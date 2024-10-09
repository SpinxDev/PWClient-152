// File		: DlgItemDesc.h
// Creator	: Feng Ning
// Date		: 2010/11/01

#pragma once

#include "DlgBase.h"

class CECIvtrItem;
class CDlgItemDesc : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP();
	AUI_DECLARE_EVENT_MAP();
public:
	CDlgItemDesc();
	void OnCommandCancel(const char* szCommand);
	void ShowDesc(CECIvtrItem* pItem, int x, int y);
	void ShowDesc(const ACString& strDesc, int x, int y);
	static bool CallBackGeneralCard(const POINT &pt, DWORD param1, DWORD param2, DWORD param3);
	bool IsShowForHover() { return IsShow() && m_pItemHover; }
	CECIvtrItem* GetItemHover() { return m_pItemHover; }

protected:
	virtual void OnChangeLayoutEnd(bool bAllDone);
	void RefreshDesc();
	bool IsDlgOverlaped(PAUIDIALOG dlg1, PAUIDIALOG dlg2);

private:
	ACString m_Desc;
	int m_HintX, m_HintY;
	CECIvtrItem* m_pItemHover;
};
