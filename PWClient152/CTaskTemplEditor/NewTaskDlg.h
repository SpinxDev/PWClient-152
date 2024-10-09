#if !defined(AFX_NEWTASKDLG_H__D6A71814_62E5_4EDE_9C65_9CEBC2A2BEBD__INCLUDED_)
#define AFX_NEWTASKDLG_H__D6A71814_62E5_4EDE_9C65_9CEBC2A2BEBD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NewTaskDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNewTaskDlg dialog

class CNewTaskDlg : public CDialog
{
// Construction
public:
	CNewTaskDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CNewTaskDlg)
	enum { IDD = IDD_NEW_TASK };
	CString	m_strName;
	CString	m_strSrc;
	BOOL	m_bCopySubTask;
	//}}AFX_DATA

	unsigned long m_ulSrc;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewTaskDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNewTaskDlg)
	afx_msg void OnSelSrc();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWTASKDLG_H__D6A71814_62E5_4EDE_9C65_9CEBC2A2BEBD__INCLUDED_)
