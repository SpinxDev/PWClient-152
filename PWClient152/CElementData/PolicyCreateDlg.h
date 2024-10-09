#if !defined(AFX_POLICYCREATEDLG_H__D70C409E_BAD5_4054_B5DD_28DF05C3162D__INCLUDED_)
#define AFX_POLICYCREATEDLG_H__D70C409E_BAD5_4054_B5DD_28DF05C3162D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PolicyCreateDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPolicyCreateDlg dialog

class CPolicyCreateDlg : public CDialog
{
// Construction
public:
	CPolicyCreateDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPolicyCreateDlg)
	enum { IDD = IDD_DIALOG_POLICY_CREATE };
	CString	m_szPolicyName;
	//}}AFX_DATA
	CString m_szPolicyCopyPath;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPolicyCreateDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPolicyCreateDlg)
		// NOTE: the ClassWizard will add member functions here
		afx_msg void OnBtnSelCopy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_POLICYCREATEDLG_H__D70C409E_BAD5_4054_B5DD_28DF05C3162D__INCLUDED_)
