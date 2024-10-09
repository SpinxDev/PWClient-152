#if !defined(AFX_TASKREGIONDLG_H__7A9915FC_43B0_420A_B596_A59629D7025F__INCLUDED_)
#define AFX_TASKREGIONDLG_H__7A9915FC_43B0_420A_B596_A59629D7025F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TaskRegionDlg.h : header file
//
#include "TaskTempl.h"
/////////////////////////////////////////////////////////////////////////////
// CTaskRegionDlg dialog

class CTaskRegionDlg : public CDialog
{
// Construction
public:
	CTaskRegionDlg(Task_Region* pRegion,unsigned long& ulCount, CWnd* pParent = NULL);   // standard constructor

	void UpdateList();
// Dialog Data
	//{{AFX_DATA(CTaskRegionDlg)
	enum { IDD = IDD_TASKREGION };
	CListCtrl	m_lstRegion;
	//}}AFX_DATA

public:
	Task_Region* m_pRegion;
	unsigned long& m_ulCount;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTaskRegionDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTaskRegionDlg)
	afx_msg void OnBtnAdd();
	virtual BOOL OnInitDialog();
	afx_msg void OnBtnDel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TASKREGIONDLG_H__7A9915FC_43B0_420A_B596_A59629D7025F__INCLUDED_)
