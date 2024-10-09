#if !defined(AFX_DLGPROGRESSNOTIFY_H__A6A09B3E_A8BC_44DD_ABC5_C47AC6542A57__INCLUDED_)
#define AFX_DLGPROGRESSNOTIFY_H__A6A09B3E_A8BC_44DD_ABC5_C47AC6542A57__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CDlgProgressNotify.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgProgressNotify dialog

class CDlgProgressNotify : public CDialog
{
// Construction
public:
	CDlgProgressNotify(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgProgressNotify)
	enum { IDD = IDD_DIALOG_PROGRESS };
	CStatic	m_Text;
	CProgressCtrl	m_Prgs;
	//}}AFX_DATA

	static CString m_strWndText;
	static	int	m_nCurProgress;
	static	int	m_nProgressMax;

	void Finish();


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgProgressNotify)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	void	UpdateProgress();

	virtual void OnOK();
	virtual void OnCancel();

	// Generated message map functions
	//{{AFX_MSG(CDlgProgressNotify)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGPROGRESSNOTIFY_H__A6A09B3E_A8BC_44DD_ABC5_C47AC6542A57__INCLUDED_)
