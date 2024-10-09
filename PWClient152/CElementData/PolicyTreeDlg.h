#if !defined(AFX_POLICYTREEDLG_H__882287F4_B2FE_4C0C_BBEC_6432A69FE4A1__INCLUDED_)
#define AFX_POLICYTREEDLG_H__882287F4_B2FE_4C0C_BBEC_6432A69FE4A1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PolicyTreeDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPolicyTreeDlg dialog

class CPolicyTreeDlg : public CDialog
{
// Construction
public:
	CPolicyTreeDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPolicyTreeDlg)
	enum { IDD = IDD_DIALOG_POLICY_TREE };
	CTreeCtrl	m_Tree;
	CString		m_strSelPolicyPath;
	//}}AFX_DATA


protected:
	void	EnumPolicy(HTREEITEM hRoot);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPolicyTreeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPolicyTreeDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_POLICYTREEDLG_H__882287F4_B2FE_4C0C_BBEC_6432A69FE4A1__INCLUDED_)
