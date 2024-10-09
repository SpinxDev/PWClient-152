#if !defined(AFX_TESTDLG_H__234FF96F_21BA_421F_A6F5_A89E45EF6DA3__INCLUDED_)
#define AFX_TESTDLG_H__234FF96F_21BA_421F_A6F5_A89E45EF6DA3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TestDlg.h : header file
//

#include "TalkModifyDlg.h"
class CPtrList;

/////////////////////////////////////////////////////////////////////////////
// CTestDlg dialog

class CTestDlg : public CDialog
{
// Construction
public:
	CTestDlg(CWnd* pParent = NULL);   // standard constructor
	bool  Init(CPtrList *pDataList,TALK_WINDOW *pTWnd);
// Dialog Data
	//{{AFX_DATA(CTestDlg)
	enum { IDD = IDD_DIALOG_TEST };
	CListBox	m_ListBoxCommand;
	CString	m_strWndTxt;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	CPtrList m_listButton;
	CPtrList* m_pDataList;
	TALK_WINDOW *m_pTWnd;

	void FreshList();
	void UpdateWndTxt();
	TALK_WINDOW *FindWnd(DWORD id);
	TALK_WINDOW *GetParentWnd(DWORD id);
	// Generated message map functions
	//{{AFX_MSG(CTestDlg)
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDblclkListCommand();
	afx_msg void OnReturn();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTDLG_H__234FF96F_21BA_421F_A6F5_A89E45EF6DA3__INCLUDED_)
