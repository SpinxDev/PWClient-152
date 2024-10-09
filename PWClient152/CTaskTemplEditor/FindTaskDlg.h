#if !defined(AFX_FINDTASKDLG_H__A199426D_9B06_4E6F_AE7B_55021E012ED5__INCLUDED_)
#define AFX_FINDTASKDLG_H__A199426D_9B06_4E6F_AE7B_55021E012ED5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FindTaskDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFindTaskDlg dialog

class CFindTaskDlg : public CDialog
{
// Construction
public:
	CFindTaskDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFindTaskDlg)
	enum { IDD = IDD_FIND_TASK };
	CString	m_strTask;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFindTaskDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFindTaskDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FINDTASKDLG_H__A199426D_9B06_4E6F_AE7B_55021E012ED5__INCLUDED_)
