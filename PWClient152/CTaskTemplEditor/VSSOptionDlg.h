#if !defined(AFX_VSSOPTIONDLG_H__21BD2037_60F2_47BD_A366_49F41D0E9364__INCLUDED_)
#define AFX_VSSOPTIONDLG_H__21BD2037_60F2_47BD_A366_49F41D0E9364__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VSSOptionDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CVSSOptionDlg dialog

class CVSSOptionDlg : public CDialog
{
// Construction
public:
	CVSSOptionDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CVSSOptionDlg)
	enum { IDD = IDD_DIALOG_OPTION_VSS };
	CString	m_strUser;
	CString	m_strPwd;
	CString	m_strVSSPath;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVSSOptionDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CVSSOptionDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnCheckAutoUpdate();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VSSOPTIONDLG_H__21BD2037_60F2_47BD_A366_49F41D0E9364__INCLUDED_)
