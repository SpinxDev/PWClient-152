// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__693FBFDB_2192_497D_BD10_B692361554C6__INCLUDED_)
#define AFX_MAINFRM_H__693FBFDB_2192_497D_BD10_B692361554C6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ListDlgBar.h" 

class CMainFrame : public CMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	
	void UpdateSearchFile();

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;
	CListDlgBar m_wndListBar;

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnOptionVss();
	afx_msg void OnVssFresh();
	afx_msg void OnExportData();
	afx_msg void OnTestTalk();
	afx_msg void OnCheck();
	afx_msg void OnPolicyEditor();
	afx_msg void OnExportPolicyData();
	afx_msg void OnTransVersion();
	afx_msg void OnGetPolicyData();
	afx_msg void OnMounsterFallItemEditor();
	afx_msg void OnItemsEditor();
	afx_msg void OnItems2Editor();
	afx_msg void OnQuicklyGetAllData();
	afx_msg void OnFindTemplate();
	afx_msg void OnTestExportData();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__693FBFDB_2192_497D_BD10_B692361554C6__INCLUDED_)
