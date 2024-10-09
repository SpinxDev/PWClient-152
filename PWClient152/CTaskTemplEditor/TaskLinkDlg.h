#if !defined(AFX_TASKLINKDLG_H__A8766C01_28D9_40D6_B0BC_71BB1A036EBE__INCLUDED_)
#define AFX_TASKLINKDLG_H__A8766C01_28D9_40D6_B0BC_71BB1A036EBE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TaskLinkDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTaskLinkDlg dialog

class CTaskLinkDlg : public CDialog
{
// Construction
public:
	CTaskLinkDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTaskLinkDlg)
	enum { IDD = IDD_TASK_LINK };
	CTreeCtrl	m_TaskLink;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTaskLinkDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	DWORD m_dwPrev;
	DWORD m_dwNext;

	void InitTree();

	// Generated message map functions
	//{{AFX_MSG(CTaskLinkDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnRclickTaskLink(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNewLink();
	afx_msg void OnPrev();
	afx_msg void OnNext();
	//}}AFX_MSG
	afx_msg void OnInsert();
	afx_msg void OnAdd();
	afx_msg void OnDel();
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TASKLINKDLG_H__A8766C01_28D9_40D6_B0BC_71BB1A036EBE__INCLUDED_)
