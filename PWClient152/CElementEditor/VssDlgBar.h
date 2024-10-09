#if !defined(AFX_VSSDLGBAR_H__23ED2338_703E_4C9F_A67A_0FF5C2F16EE3__INCLUDED_)
#define AFX_VSSDLGBAR_H__23ED2338_703E_4C9F_A67A_0FF5C2F16EE3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VssDlgBar.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CVssDlgBar dialog

class CVssDlgBar : public CDialogBar
{
// Construction
public:
	CVssDlgBar(CWnd* pParent = NULL);   // standard constructor
	void AddString(const char *szMsg);
// Dialog Data
	//{{AFX_DATA(CVssDlgBar)
	enum { IDD = IDD_VSS_LOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVssDlgBar)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CVssDlgBar)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetfocusEditVssLog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VSSDLGBAR_H__23ED2338_703E_4C9F_A67A_0FF5C2F16EE3__INCLUDED_)
