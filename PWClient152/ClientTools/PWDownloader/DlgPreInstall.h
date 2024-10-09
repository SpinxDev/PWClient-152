#if !defined(AFX_DLGPREINSTALL_H__2331FA2A_2646_42AD_9BF2_4A441AD78463__INCLUDED_)
#define AFX_DLGPREINSTALL_H__2331FA2A_2646_42AD_9BF2_4A441AD78463__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgPreInstall.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgPreInstall dialog

class CDlgPreInstall : public CDialog
{
// Construction
public:
	CDlgPreInstall(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgPreInstall)
	enum { IDD = IDD_DLG_PREINSTALL };
	CString	m_szExtractDir;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgPreInstall)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CDlgPreInstall)
	afx_msg void OnSelDir();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnNcPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGPREINSTALL_H__2331FA2A_2646_42AD_9BF2_4A441AD78463__INCLUDED_)
