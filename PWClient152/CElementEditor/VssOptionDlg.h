#if !defined(AFX_VSSOPTIONDLG_H__F643C578_B8A3_45D9_88B2_8ABDB884C006__INCLUDED_)
#define AFX_VSSOPTIONDLG_H__F643C578_B8A3_45D9_88B2_8ABDB884C006__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VssOptionDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CVssOptionDlg dialog

class CVssOptionDlg : public CDialog
{
// Construction
public:
	CVssOptionDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CVssOptionDlg)
	enum { IDD = IDD_DIALOG_VSS_OPTION };
	CString	m_editVssPassword;
	CString	m_editVssServerPath;
	CString	m_editVssUserName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVssOptionDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CVssOptionDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VSSOPTIONDLG_H__F643C578_B8A3_45D9_88B2_8ABDB884C006__INCLUDED_)
