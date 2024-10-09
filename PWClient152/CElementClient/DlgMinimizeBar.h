// Filename	: DlgMinimizeBar.h
// Creator	: Xiao Zhou
// Date		: 2005/11/14

#pragma once

#include "DlgBase.h"
#include "AUILabel.h"
#include "AUIDialog.h"

class CDlgMinimizeBar;
class CECMiniBarMgr
{
public:
	struct BarInfo
	{
		ACString subject;
		DWORD color;

		BarInfo():color(0){}

		BarInfo(const ACHAR* szSubject, DWORD dwColor)
			:subject(szSubject), color(dwColor)
		{ }
	};

	CECMiniBarMgr();

	bool Init(AUIManager* pUIMan);
	
	bool MinimizeDialog(PAUIDIALOG pDlg, const BarInfo& info);
	bool RestoreDialog(PAUIDIALOG pDlg);
	void FlashDialog(PAUIDIALOG pDlg);
	void UpdateDialog(PAUIDIALOG pDlg, const CECMiniBarMgr::BarInfo& info);

	void ArrangeMinimizeBar(bool bForceRearrange);

private:
	CDlgMinimizeBar* GetAvailableBar();
	CDlgMinimizeBar* FindBar(PAUIDIALOG pDlg);
	void UpdateDialogImpl(CDlgMinimizeBar *pThis, const CECMiniBarMgr::BarInfo& info);

	AUIManager* m_pUIMan;
	abase::vector<CDlgMinimizeBar*> m_Bars;

	int		m_nTotalBars;
	bool	m_bSystemMenuExpand;
};

class CDlgMinimizeBar : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP();
	AUI_DECLARE_EVENT_MAP();
	
	friend CECMiniBarMgr;

public:
	CDlgMinimizeBar();
	virtual ~CDlgMinimizeBar();
	
	void OnCommandCancel(const char * szCommand);
	void OnEventLButtonDBCLK(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void CopyData(CDlgMinimizeBar* pOther);

protected:
	virtual bool OnInitDialog();
	virtual void OnTick();
	
	PAUILABEL				m_pTxt_Subject;
	bool					m_bFlash;

	CECMiniBarMgr::BarInfo	m_Info;
};
