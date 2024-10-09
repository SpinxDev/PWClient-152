#if !defined(AFX_CREATEWORKSPACEDLG_H__2DB02B10_0CCC_47BB_99EB_FCF79D18EF05__INCLUDED_)
#define AFX_CREATEWORKSPACEDLG_H__2DB02B10_0CCC_47BB_99EB_FCF79D18EF05__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CreateWorkspaceDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCreateWorkspaceDlg dialog

class CCreateWorkspaceDlg : public CDialog
{
// Construction
public:
	CCreateWorkspaceDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CCreateWorkspaceDlg)
	enum { IDD = IDD_DIALOG_CREATE_WORKSPACE };
	CString	m_strNewWorkspace;
	BOOL m_bRandomMap;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCreateWorkspaceDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCreateWorkspaceDlg)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CREATEWORKSPACEDLG_H__2DB02B10_0CCC_47BB_99EB_FCF79D18EF05__INCLUDED_)
